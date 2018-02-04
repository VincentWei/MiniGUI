/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
 **  $Id: em86gfx.c 8944 2007-12-29 08:29:16Z xwyan $
 **  
 **  em86gfx.c: NEWGAL driver for EM86xx GFX.
 ** 
 **  Copyright (C) 2005 Feynman Software.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "list.h"

#ifdef _MGRM_PROCESSES
#include "client.h"
#endif 

#ifdef _MGGAL_SIGMA8654

#include "sigma8654.h"
#include "sigma8654_pri.h"
#include "dcc/include/dcc_macros.h"
#include "rmcore/include/rmcore.h"
#include "rmlibcw/include/rmlibcw.h"
#include "rmdisplay/include/rmdisplay.h"

struct hwsrfc_item{
    list_t list;
    GAL_Surface* surface; 
};
static struct list_head surface_list;

#define PRUA            (((struct GAL_PrivateVideoData *)this->hidden)->pRUA)
#define PDCC            (((struct GAL_PrivateVideoData *)this->hidden)->pDCC)
#define DCC_PROFILE     (((struct GAL_PrivateVideoData *)this->hidden)->DCC_profile)
#define POSD_SOURCE     (((struct GAL_PrivateVideoData *)this->hidden)->pOSD_source)
#define OSD_SCALER      (((struct GAL_PrivateVideoData *)this->hidden)->OSD_scaler)
#define LUMA_ADDR        (((struct GAL_PrivateVideoData *)this->hidden)->LumaAddr)
#define LUMA_ADDR_MAPPED        (((struct GAL_PrivateVideoData *)this->hidden)->LumaAddrMapped)
#define LUMA_SIZE        (((struct GAL_PrivateVideoData *)this->hidden)->LumaSize)
#define CHROMA_ADDR      (((struct GAL_PrivateVideoData *)this->hidden)->ChromaAddr)
#define CHROMA_ADDR_MAPPED      (((struct GAL_PrivateVideoData *)this->hidden)->ChromaAddrMapped)
#define CHROMA_SIZE      (((struct GAL_PrivateVideoData *)this->hidden)->ChromaSize)
#define GFX_PROFILE     (((struct GAL_PrivateVideoData *)this->hidden)->GFX_profile)
#define GFX_TARGET       (((struct GAL_PrivateVideoData *)this->hidden)->GFXtarget)

#define TIMEOUT_US 1000000

#define WAIT_FOR_COMMANDS
#ifdef WAIT_FOR_COMMANDS
#define SEND_GFX_COMMAND(pRUA, moduleID, propertyID, pValue, ValueSize) \
{ \
    RMstatus err; \
    RMuint32 n = 5; \
    struct RUAEvent evt; \
    evt.ModuleID = moduleID; \
    evt.Mask = RUAEVENT_COMMANDCOMPLETION; \
    do { \
        err = RUASetProperty(pRUA, moduleID, propertyID, pValue, ValueSize, 0); \
        if(err == RM_PENDING) { \
            while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
            RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
        } \
        n--; \
    } while ((n > 0) && (err == RM_PENDING)); \
    if (err != RM_OK) { \
        RMDBGLOG((ENABLE, "Can't send command to command fifo\n" )); \
        return err; \
    } \
    while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
    RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
} \

#else													

#define SEND_GFX_COMMAND(pRUA, moduleID, propertyID, pValue, ValueSize) \
{ \
    RMstatus err; \
    RMuint32 n; \
    n = 5; \
    do { \
        err = RUASetProperty(pRUA, moduleID, propertyID, pValue, ValueSize, 0); \
        if ((err == RM_PENDING)) { \
            struct RUAEvent evt; \
            evt.ModuleID = moduleID; \
            evt.Mask = RUAEVENT_COMMANDCOMPLETION; \
            while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
            RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
        } \
        n--; \
    } while((n > 0) && (err == RM_PENDING)); \
    if (err != RM_OK) { \
        RMDBGLOG((ENABLE, "Can't send command to command fifo\n" )); \
        return err; \
    } \
}

#endif /* WAIT_FOR_COMMANDS */

#define FUNC_ENTER()    printf(">>> entering %s %s\n", __FILE__, __FUNCTION__)
#define FUNC_LEAVE()    printf("<<< leaving  %s %s\n", __FILE__, __FUNCTION__)

// HDMI thread handling
static RMbool run_hdmi = TRUE;
struct local_context {
    struct RUA *prua;
    struct dh_context *dh_info;
    struct rmdisplay_video *video_opt;
    struct rmdisplay_audio *audio_opt;
};
static struct local_context context;
static  RMint32 nice = 0;
static  RMthread hdmi_thread = NULL;
static  RMsemaphore hdmi_semaphore = NULL;

// Display setup options
struct dh_context dh_info;  // HDMI context
struct rmdisplay_video video_opt;  // video related display options
struct rmdisplay_audio audio_opt;  // audio related display options

static int GFX_VideoInit(_THIS, GAL_PixelFormat * vformat);
static GAL_Rect ** GFX_ListModes(_THIS, GAL_PixelFormat * format, Uint32 flags);
static GAL_Surface * GFX_SetVideoMode(_THIS, GAL_Surface * current, int width, int height, int bpp, Uint32 flags);
static void GFX_VideoQuit(_THIS);
static int GFX_FillHWRect(_THIS, GAL_Surface * dst, GAL_Rect * rect, Uint32 color);
static int GFX_AllocHWSurface(_THIS, GAL_Surface * surface);
static void GFX_FreeHWSurface(_THIS, GAL_Surface * surface);
static int GFX_HWAccelBlit(GAL_Surface * src, GAL_Rect * srcrect, GAL_Surface * dst, GAL_Rect * dstrect);
static int GFX_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst);
static int init_GFXengine(_THIS);
int GFX_SetHWAlpha(_THIS, GAL_Surface * surface, Uint8 value);
static void __GFX_FreeHWSurface(_THIS, GAL_Surface* surface);

static int GFX_Available(void)
{
    return 1;
}

static void GFX_DeleteDevice(GAL_VideoDevice * device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice * GFX_CreateDevice(int devindex)
{
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (this) {
        memset(this, 0, sizeof(*this));
        this->hidden = (struct GAL_PrivateVideoData *)malloc(sizeof(*this->hidden));
    }
    if ((this == NULL) || (this->hidden == NULL)) {
        GAL_OutOfMemory();
        if (this) free(this);
        return(0);
    }
    memset(this->hidden, 0, sizeof(*this->hidden));

    this->VideoInit = GFX_VideoInit;
    this->ListModes = GFX_ListModes;
    this->SetVideoMode = GFX_SetVideoMode;
    this->SetColors = NULL;
    this->VideoQuit = GFX_VideoQuit;
    this->AllocHWSurface = GFX_AllocHWSurface;
    this->CheckHWBlit = GFX_CheckHWBlit;
    this->FillHWRect = GFX_FillHWRect;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;
    this->UpdateRects = NULL;
    this->FreeHWSurface = GFX_FreeHWSurface;

    this->free = GFX_DeleteDevice;

    return this;
}

VideoBootStrap SIGMA8654GFX_bootstrap = {
    "sigma8654", "sigma8654 GFX Video Driver",
    GFX_Available, GFX_CreateDevice
};

static int GFX_VideoInit(_THIS, GAL_PixelFormat * vformat)
{
    FUNC_ENTER();
    RMstatus err;

    INIT_LIST_HEAD(&surface_list);

    err = RUACreateInstance(&PRUA, 0);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error creating RUA instance (%d!)\n", err);
        return -1;
    }

    err = DCCOpen(PRUA, &PDCC);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error Opening DCC (%d)!\n", err);
        return err;
    }

    err = DCCInitChainEx(PDCC, 0);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot initialize microcode (%d)!\n", err);
        return err;
    }

    {
        GAL_VideoInfo *video_info = &this->info;
        // Determine the screen depth (use default 8-bit depth) 
        // we change this during the GAL_SetVideoMode implementation... 
        vformat->BitsPerPixel = 32;
        vformat->BytesPerPixel = 4;
        vformat->Amask = 0xff000000;
        vformat->Rmask = 0x00ff0000;
        vformat->Gmask = 0x0000ff00;
        vformat->Bmask = 0x000000ff;
        {
#define FLAGS_BLIT_FILL  0x01
#define FLAGS_BLIT_HW    0x02
#define FLAGS_BLIT_HW_CC    0x04
#define FLAGS_BLIT_HW_A    0x08
            char *env = getenv("MG_ENV_HWACCEL_FLAGS");
            int flags = 0;
            if (! env)
            {
                flags = FLAGS_BLIT_HW | FLAGS_BLIT_HW_CC | FLAGS_BLIT_HW_A;
            }
            else
            {
                flags = atoi(env);
            }
            flags = 0;
            // FillRect. Disabled because it is too slow with the stupid flush and wait policy 
            video_info->blit_fill = (flags & FLAGS_BLIT_FILL) ? 1 : 0; 
            video_info->blit_hw = (flags & FLAGS_BLIT_HW) ? 1 : 0;   // BitBlit //
            video_info->blit_hw_CC = (flags & FLAGS_BLIT_HW_CC) ? 1 : 0; // Colorkey //
            video_info->blit_hw_A  = (flags & FLAGS_BLIT_HW_A) ? 1 : 0; // Alpha 
        }
        video_info->mlt_surfaces = 0;
    }
    FUNC_LEAVE();
    return(0);
}

static GAL_Rect ** GFX_ListModes(_THIS, GAL_PixelFormat * format, Uint32 flags)
{
    return (GAL_Rect **)-1;
}
static RMstatus dccsperrok(RMstatus ErrOK, struct RUA* prua, RMuint32 moduleID, RMuint32 propertyID, void* pValue, RMuint32 ValueSize)
{
    DCCSPERROK(ErrOK, prua, moduleID, propertyID, pValue, ValueSize, " ");
    return RM_OK;
}


static void local_hdmi_update_loop(void *c)
{
    struct local_context *context = (struct local_context *)c;

    do {
        rmdisplay_update(context->prua, context->dh_info, context->video_opt, context->audio_opt);
        RMMicroSecondSleep(50*1000);

        if (dh_info.CEC.Update) {
            if (dh_info.CEC.Standby) {
                RMDBGLOG((ENABLE, "CEC REQUEST:      ---   S T A N D B Y   ---\n"));
                dh_info.CEC.Standby = FALSE;
            }
            if (dh_info.CEC.Wakeup) {
                RMDBGLOG((ENABLE, "CEC REQUEST:      ---   W A K E U P   ---\n"));
                dh_info.CEC.Wakeup = FALSE;
            }
            dh_info.CEC.Update = FALSE;
        }
    } while(run_hdmi);
}

static RMstatus CreateSurface(_THIS) 
{
    RMstatus err;
    RMuint32 mixer = EMHWLIB_MODULE(DispMainMixer, 0);
    enum EMhwlibMixerSourceState state = EMhwlibMixerSourceState_Master;
    RMuint32 lock_scaler = 0;
    struct EMhwlibDisplayWindow output_window;
    struct EMhwlibDisplayWindow source_window;
    rmdisplay_set_fullscreen_window(&output_window);
    rmdisplay_set_fullscreen_window(&source_window);
    RMbool enable;
    RMuint32 src_index, mixer_src;
    mixer_src = EMHWLIB_TARGET_MODULE(mixer, 0, src_index);
    struct EMhwlibNonLinearScalingMode nonlinearmode;
    struct EMhwlibBlackStripMode blackstrip;
    struct EMhwlibCutStripMode cutstrip;

    nonlinearmode.Width = 0;
    nonlinearmode.Level = 0;
    blackstrip.Horizontal = 4096;
    blackstrip.Vertical = 4096;
    cutstrip.Horizontal = 4096;
    cutstrip.Vertical = 4096;

    err = DCCGetScalerModuleID(PDCC, DCCRoute_Main, DCCSurface_OSD, 0, &OSD_SCALER);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot get surface to display OSD source %d\n", err));
        return err; 
    }
    err = RUAExchangeProperty(PRUA, mixer, RMGenericPropertyID_MixerSourceIndex, &(OSD_SCALER), sizeof(OSD_SCALER),
            &src_index, sizeof(src_index));
    if (err != RM_OK) {
        RMDBGLOG((ENABLE, "Cannot get scaler index: %s\n", RMstatusToString(err)));
        return err;
    }

    mixer_src = EMHWLIB_TARGET_MODULE(EMHWLIB_MODULE_CATEGORY(mixer), 0 , src_index);
    DCCSPERR(PRUA, mixer_src, RMGenericPropertyID_MixerSourceState, &state, sizeof(state), "Can not set scaler's state on mixer");
    DCCSPERR(PRUA, mixer_src, RMGenericPropertyID_MixerSourceWindow, &output_window, sizeof(output_window), "Can not set scaler input window");
    DCCSPERR(PRUA, mixer_src, RMGenericPropertyID_LockMixerSourceScalingMode, &lock_scaler, sizeof(lock_scaler), "Can not set lock scaler");
    DCCSPERR(PRUA, mixer_src, RMGenericPropertyID_Validate, NULL, 0, "Can not validate mixer");

    enable = FALSE;
    err =  RUASetProperty(PRUA, OSD_SCALER, RMGenericPropertyID_Enable, &enable, sizeof(enable), 0);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not disable scaler\n"));
        return err;
    }
    DCCSPERR(PRUA, mixer_src, RMGenericPropertyID_Validate, NULL, 0, "Can not validate mixer");

    DCCSPERR(PRUA, OSD_SCALER, RMGenericPropertyID_ScalerInputWindow, &source_window, sizeof(source_window), "Can not set scaler input window on OSD surface");
    DCCSPERROK(RM_INVALIDMODE, PRUA, OSD_SCALER, RMGenericPropertyID_NonLinearScalingMode, &nonlinearmode, sizeof(nonlinearmode), "Error setting current scaler non-linear selection");
    DCCSPERROK(RM_INVALIDMODE, PRUA, OSD_SCALER, RMGenericPropertyID_BlackStripMode, &blackstrip, sizeof(blackstrip), "Error setting current scaler blackstrip mode");
    DCCSPERROK(RM_INVALIDMODE, PRUA, OSD_SCALER, RMGenericPropertyID_CutStripMode, &cutstrip, sizeof(cutstrip), "Error setting current scaler cutstrip mode");
    enable = TRUE;
    err =  RUASetProperty(PRUA, OSD_SCALER, RMGenericPropertyID_Enable, &enable, sizeof(enable), 0);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not enable scaler\n"));
        return err;
    }

    //open OSD Video Source
    err = DCCOpenOSDVideoSource(PDCC, &DCC_PROFILE, &POSD_SOURCE);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot open OSD decoder (%d)!\n", err);
        return err;
    }

    //chech if the profile colormode is surpported  
    err = RUASetProperty(PRUA, OSD_SCALER, RMGenericPropertyID_IsColorModeSupported, 
            &(DCC_PROFILE.ColorMode), sizeof(DCC_PROFILE.ColorMode) ,0);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "The scaler you selected does not support colorspace %d\n", DCC_PROFILE.ColorMode));
        return err;
    }
    err = DCCSetSurfaceSource(PDCC, OSD_SCALER, NULL);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not set the surface source: %s\n", RMstatusToString(err)));
        return err;
    }

    err = DCCSetSurfaceSource(PDCC, OSD_SCALER, POSD_SOURCE);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not set the surface source: %s\n", RMstatusToString(err)));
        return err;
    }
#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
        RMbool persistent = TRUE;
        DCCSPERR(PRUA, OSD_SCALER, RMGenericPropertyID_PersistentSurface, 
                &persistent, sizeof(persistent), "can not set persistent surface");
        DCCSPERR(PRUA, OSD_SCALER, RMGenericPropertyID_Validate, NULL, 0, "Cannot validate scaler configure");	
    }
#endif
    DCCSPERR(PRUA, OSD_SCALER, RMGenericPropertyID_Validate, NULL, 0, "Cannot validate scaler input window");	
    err = DCCGetOSDVideoSourceInfo(POSD_SOURCE, &LUMA_ADDR, &LUMA_SIZE, &CHROMA_ADDR, &CHROMA_SIZE);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot get osd buffer info (%d)!\n", err);
        return err;
    }
    return RM_OK;
} 


static RMstatus ForceMixerSourceToSlave(_THIS) 
{
    enum EMhwlibMixerSourceState state;
    RMstatus err = RM_OK;
    RMuint32 mixer = EMHWLIB_MODULE(DispMainMixer, 0);


    RMuint32 scaler= EMHWLIB_MODULE(DispGFXMultiScaler,0);
    RMuint32 src_index; 
    /* set a NULL surface, this will force a full register update when next surface is set */
    err = DCCSetSurfaceSource(PDCC, scaler, NULL);
    if (RMFAILED(err)) {
        fprintf(stderr, "Cannot unset gfx scaler's surface\n");
        return RM_ERROR;
    }

    err = RUAExchangeProperty(PRUA, mixer, RMGenericPropertyID_MixerSourceIndex, &scaler, sizeof(scaler), 
            &src_index, sizeof(src_index));
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot get scaler index\n"));
        return err;
    }

    RMuint32 mixer_src = EMHWLIB_TARGET_MODULE(mixer, 0 , src_index );
    state = EMhwlibMixerSourceState_Slave;

    while((err =  RUASetProperty(PRUA, mixer_src, RMGenericPropertyID_MixerSourceState, &state, sizeof(state), 0))==RM_PENDING);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot set scaler's state on mixer\n"));
        return err;
    }

    while ((err = RUASetProperty(PRUA, mixer, RMGenericPropertyID_Validate, NULL, 0, 0)) == RM_PENDING);
    if (RMFAILED(err)) {
        fprintf(stderr, "Cannot validate mixer\n");
        return err;
    }
    do{
        err = RUAGetProperty(PRUA, mixer_src, RMGenericPropertyID_MixerSourceState, &state, sizeof(state));
        if(RMFAILED(err)){
            RMDBGLOG((ENABLE, "error getting source state %s\n", RMstatusToString(err)));
        }
        /* this is just to avoid busy loops, can be substituted by a sleep function or removed */
        if(state != EMhwlibMixerSourceState_Slave){
            struct RUAEvent evt;

            evt.ModuleID = EMHWLIB_MODULE(DisplayBlock, 0);
            evt.Mask = EMHWLIB_DISPLAY_EVENT_ID(mixer);

            err = RUAWaitForMultipleEvents(PRUA, &evt, 1, TIMEOUT_US, NULL);
            if(RMFAILED(err)){
                RMDBGLOG((ENABLE, "wait for display update event completion failed, %s\n", RMstatusToString(err)));
            }
        }
    }while(state != EMhwlibMixerSourceState_Slave);
    return err;
}

#ifdef _MGRM_PROCESSES
static RMstatus GetSurface(_THIS) 
{
    printf(">>>>>>>> Enter GetSurface\n");
    RMstatus err = RM_OK;
    if (PDCC == NULL || PRUA == NULL) {
        return RM_ERROR;
    }
    err = DCCGetScalerModuleID(PDCC, DCCRoute_Main, DCCSurface_OSD, 0, &OSD_SCALER);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot get surface to display OSD source %d\n", err));
        return err; 
    }
    struct DisplayBlock_SurfaceSize_in_type size_in;
    struct DisplayBlock_SurfaceSize_out_type size_out;
    struct DisplayBlock_SurfaceInfo_out_type config;
    RMuint32 surface;

    err = RUAGetProperty(PRUA, OSD_SCALER, RMGenericPropertyID_Surface, &surface, sizeof(surface));
    if (RMFAILED(err) || surface == 0){
        printf("can not get osd scaler input surface\n");
        return RM_ERROR;
    }
    printf("get persistent surface successfully\n");

    err = RUAExchangeProperty(PRUA, DisplayBlock, RMDisplayBlockPropertyID_SurfaceInfo,
            (void *)&surface, sizeof(void *), &config, sizeof(config));
    if (RMFAILED(err)){
        printf("can not get surface info\n");
        return err;
    }
    size_in.Width = config.Width;
    size_in.Height = config.Height;
    size_in.ColorMode = config.ColorMode;
    size_in.ColorFormat = config.ColorFormat;
    size_in.SamplingMode = config.SamplingMode;

    err = RUAExchangeProperty(PRUA, DisplayBlock, RMDisplayBlockPropertyID_SurfaceSize, 
            &size_in, sizeof(size_in), &size_out, sizeof(size_out));
    if (RMFAILED(err)) {
        printf("can not get surface size\n");
        return err;
    }

    LUMA_ADDR = config.LumaAddress; 
    LUMA_SIZE = size_out.LumaSize; 
    CHROMA_ADDR = config.ChromaAddress; 
    CHROMA_SIZE = size_out.ChromaSize; 
    printf("LUMA_ADDR = %p, LUMA_SIZE = %p\n", (void*)LUMA_ADDR, (void*)LUMA_SIZE);
    printf("<<<<<<<< Leave  GetSurface\n");
    return err;
}
#endif

int Sigma8654_ServerOnGetSurface(REQ_SIGMA8654_GETSURFACE * request, REP_SIGMA8654_GETSURFACE * reply)
{
    //memcpy(reply, &reply_primary_surface_info, sizeof(reply_primary_surface_info));
    return 0;
}
static GAL_Surface* GFX_SetVideoMode(_THIS, GAL_Surface * surface, int width, int height, int bpp, Uint32 flags)
{
    FUNC_ENTER();
    RMstatus err = RM_OK;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
#endif
        hdmi_clear_context(&dh_info, NULL);
        rmdisplay_init_video(&video_opt);
        rmdisplay_init_audio(&audio_opt);
        char *cmd_arg[4] = {"-o", "hdmi", "-r", "1080i60"};
        int i=0;
        while (4  > i) {
            err = rmdisplay_parse_video(4, cmd_arg, &i, &video_opt);
            if (err != RM_PENDING) continue;
            i++;
        }
#ifdef _MGRM_PROCESSES
    }
#endif
    Sigma8654Private_HWSurface_hwdata* hwdata = calloc(1, sizeof(Sigma8654Private_HWSurface_hwdata));
    err = DCCGetScalerModuleID(PDCC, DCCRoute_Main, DCCSurface_OSD, 0, &OSD_SCALER);
    if (RMFAILED(err)) {
        free(hwdata);
        RMDBGLOG((ENABLE, "Cannot get surface to display OSD source %d\n", err));
        return NULL;
    }
    DCC_PROFILE.ColorSpace = EMhwlibColorSpace_RGB_0_255;
    DCC_PROFILE.SamplingMode = EMhwlibSamplingMode_444;
    DCC_PROFILE.PixelAspectRatio.X = 1;
    DCC_PROFILE.PixelAspectRatio.Y = 1;
    DCC_PROFILE.Width = width;
    DCC_PROFILE.Height = height;

    switch (bpp) {
        case 16:
            DCC_PROFILE.ColorMode = EMhwlibColorMode_TrueColor;
            DCC_PROFILE.ColorFormat = EMhwlibColorFormat_16BPP_565;
            Rmask = 0xF800;
            Gmask = 0x07E0;
            Bmask = 0x001F;
            Amask = 0;
            break;

        case 24:
        case 32:
            bpp = 32;
            Amask = 0xFF000000;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            DCC_PROFILE.ColorMode = EMhwlibColorMode_TrueColor;
            DCC_PROFILE.ColorFormat = EMhwlibColorFormat_32BPP;
            break;
    }

    video_opt.mixer_color_space = DCC_PROFILE.ColorSpace;
    err = rmdisplay_apply_options(PRUA, &dh_info, &video_opt, &audio_opt);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not set display options!\n"));
        return NULL;
    }

#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
#endif
        context.prua = PRUA;
        context.dh_info = &dh_info;
        context.video_opt = &video_opt;
        context.audio_opt = &audio_opt;

        // Start HDMI updater thread
        run_hdmi = TRUE;
        hdmi_semaphore = RMCreateSemaphore(1);
        hdmi_thread = RMCreateThreadWithPriority("HDMIUpdateThread", (RMfunc)local_hdmi_update_loop, (void *)(&context), FALSE, nice);
        if (hdmi_thread == NULL) {
            run_hdmi = FALSE;
            fprintf(stderr, "Failed to start HDMI thread!\n");
            run_hdmi = FALSE;

            RMWaitForThreadToFinish(hdmi_thread);
            if (hdmi_semaphore) {
                RMDeleteSemaphore(hdmi_semaphore);
                hdmi_semaphore = NULL;
            }
            hdmi_thread = NULL;
            // Close display
            err = rmdisplay_close(PRUA, &dh_info, &video_opt, &audio_opt);
            if (RMFAILED(err)) {
                RMDBGLOG((ENABLE, "Can not close display! %s\n", RMstatusToString(err)));
            }
        }

        err = CreateSurface(this);
        if (err != RM_OK) {
            RMDBGLOG((ENABLE, "Cannot create surface: %s\n", RMstatusToString(err)));
            return NULL;
        }

        // the mixer should not modify the GFX scaler's config 
        err = ForceMixerSourceToSlave(this);
        if (RMFAILED(err)) {
            fprintf(stderr, "can not force mixer source state to slave\n");
            return NULL;
        }
#ifdef _MGRM_PROCESSES
    }
    else {
        err = GetSurface(this);
        if (err != RM_OK) {
            RMDBGLOG((ENABLE, "Cannot get surface params: %s\n", RMstatusToString(err)));
            return NULL;
        }
    }
#endif
    err = RUALock(PRUA, LUMA_ADDR, LUMA_SIZE);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Error locking OSD buffer at 0x%08lX (0x%08lX bytes)\n", LUMA_ADDR, LUMA_SIZE));
        return NULL;
    }
    LUMA_ADDR_MAPPED = RUAMap(PRUA, LUMA_ADDR, LUMA_SIZE);
    if ((void*)LUMA_ADDR_MAPPED == NULL) {
        RMDBGLOG((ENABLE, "Error mapping OSD buffer at 0x%08lX (0x%08lX bytes)\n", LUMA_ADDR, LUMA_SIZE));
        return NULL;
    }
    if(CHROMA_SIZE){
        err = RUALock(PRUA, CHROMA_ADDR, CHROMA_SIZE);
        if (RMFAILED(err)) {
            RMDBGLOG((ENABLE, "Error locking OSD buffer at 0x%08lX (0x%08lX bytes)\n", CHROMA_ADDR, CHROMA_SIZE));
            return NULL;
        }
        CHROMA_ADDR_MAPPED = RUAMap(PRUA, CHROMA_ADDR, CHROMA_SIZE);
        if ((void*)CHROMA_ADDR_MAPPED == NULL) {
            RMDBGLOG((ENABLE, "Error mapping OSD buffer at 0x%08lX (0x%08lX bytes)\n", CHROMA_ADDR, CHROMA_SIZE));
            return NULL;
        }
    }

    hwdata->osd_source = POSD_SOURCE;
    hwdata->gfx_addr = LUMA_ADDR;
    surface->flags = (GAL_FULLSCREEN | GAL_HWSURFACE);
    surface->w = width;
    surface->h = height;
    surface->pitch = width * bpp / 8;
    surface->pixels = (char *)LUMA_ADDR_MAPPED;
    surface->hwdata = (void*)hwdata;

#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
#endif
        if (!GAL_ReallocFormat(surface, bpp, Rmask, Gmask, Bmask, Amask)) {
            fprintf(stderr, "NEWGAL>EM86GFX: Reallocating format error!\n");
            return NULL;
        }
        //printf("hw surface pixels=%p, gfx_addr=%p, osd_source is %p\n", surface->pixels, hwdata->gfx_addr, hwdata->osd_source);

        if (!init_GFXengine(this)) {
            fprintf(stderr, "NEWGAL>EM86GFX: Cannot init GFX engine!\n");
            return NULL;
        }
#ifdef _MGRM_PROCESSES
    }
#endif
    FUNC_LEAVE();
    return surface;
}
static void GFX_VideoQuit (_THIS)
{
    FUNC_ENTER();
    RMstatus err; 
    //delete all hardware surface allocated
    struct list_head* ptr, *n;
    struct hwsrfc_item* item; 
    list_for_each_safe(ptr, n,  &surface_list){
        list_del(ptr);
        item = list_entry(ptr, struct hwsrfc_item, list);
        __GFX_FreeHWSurface(this, item->surface);
        free(item);
    }

#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
#endif
        //wait for all commands to be finished 
        struct RUAEvent evt;
        RMbool empty_queue = FALSE;
        evt.ModuleID = GFX_TARGET;
        evt.Mask = RUAEVENT_COMMANDCOMPLETION;
        while(!empty_queue){
            RUAGetProperty(PRUA, GFX_TARGET, RMGFXEnginePropertyID_CommandQueueEmpty, &empty_queue, sizeof(empty_queue));
            if(!empty_queue) {
                while( RUAWaitForMultipleEvents(PRUA, &evt, 1, TIMEOUT_US, NULL)!=RM_OK){
                    RMDBGLOG((ENABLE, "Waiting for a command to finish\n"));
                }
            }
        }
        
        RMuint32 close_profile = 0;
        while ((err = RUASetProperty(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Close, &close_profile, sizeof(close_profile), 0)) == RM_PENDING); 
        if (RMFAILED(err)) {
            RMDBGLOG((ENABLE, "Cannot close the gfx accelerator\n"));
            return;
        } 

        if (GFX_PROFILE.CachedAddress)
            RUAFree(PRUA, GFX_PROFILE.CachedAddress);

        if (GFX_PROFILE.UncachedAddress)
            RUAFree(PRUA, GFX_PROFILE.UncachedAddress);


        //stop hdmi update thread
        run_hdmi = FALSE;
        RMWaitForThreadToFinish(hdmi_thread);
        if (hdmi_semaphore) {
            RMDeleteSemaphore(hdmi_semaphore);
            hdmi_semaphore = NULL;
        }
        hdmi_thread = NULL;
        // Close display
        err = rmdisplay_close(PRUA, &dh_info, &video_opt, &audio_opt);
        if (RMFAILED(err)) {
            RMDBGLOG((ENABLE, "Can not close display! %s\n", RMstatusToString(err)));
        }

        struct EMhwlibDisplayWindow source_window;
        source_window.X = 2048;
        source_window.Y = 2048;
        source_window.Width = 4096;
        source_window.Height = 4096;
        source_window.XPositionMode = EMhwlibDisplayWindowPositionMode_FrontEdgeToCenter;
        source_window.YPositionMode = EMhwlibDisplayWindowPositionMode_FrontEdgeToCenter;
        source_window.XMode = EMhwlibDisplayWindowValueMode_Relative;
        source_window.YMode = EMhwlibDisplayWindowValueMode_Relative;
        source_window.WidthMode = EMhwlibDisplayWindowValueMode_Relative;
        source_window.HeightMode = EMhwlibDisplayWindowValueMode_Relative;
        if( RM_OK != dccsperrok(0, PRUA, OSD_SCALER, RMGenericPropertyID_ScalerInputWindow, &source_window, sizeof(source_window))) {
            RMDBGLOG((ENABLE, "Cannot set scaler input window on OSD surface (%d)!\n", err));
            return;
        }

        if( RM_OK != dccsperrok(0, PRUA, OSD_SCALER, RMGenericPropertyID_Validate, NULL, 0)) {
            RMDBGLOG((ENABLE, "Cannot validate scaler input window %d\n", err));
            return;
        }
#ifdef _MGRM_PROCESSES
    }
#endif
    if(LUMA_ADDR) {
        RUAUnMap(PRUA, LUMA_ADDR_MAPPED, LUMA_SIZE); 
        err = RUAUnLock(PRUA, LUMA_ADDR, LUMA_SIZE); 
        if (RMFAILED(err)) { 
            RMDBGLOG((ENABLE, "Error unlocking OSD buffer at 0x%08lX (0x%08lX bytes)\n", LUMA_ADDR, LUMA_SIZE)); 
            return; 
        }
        LUMA_ADDR = 0;
    } 
    if(CHROMA_SIZE && CHROMA_ADDR){ 
        RUAUnMap(PRUA, CHROMA_ADDR_MAPPED, CHROMA_SIZE); 
        err = RUAUnLock(PRUA, CHROMA_ADDR, CHROMA_SIZE); 
        if (RMFAILED(err)) { 
            RMDBGLOG((ENABLE, "Error unlocking OSD buffer at 0x%08lX (0x%08lX bytes)\n", CHROMA_ADDR, CHROMA_SIZE)); 
            return; 
        } 
        CHROMA_ADDR = 0;
    }
#ifdef _MGRM_PROCESSES
    if(mgIsServer) {
#endif
        err = DCCCloseVideoSource(POSD_SOURCE);
        if (RMFAILED(err)) {
            RMDBGLOG((ENABLE, "Cannot close osd source %s\n", RMstatusToString(err)));
            return;
        }
#ifdef _MGRM_PROCESSES
    }
#endif
    this->screen = NULL;
    err = DCCClose(PDCC);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not close DCC! %s\n", RMstatusToString(err)));
        return;
    }

    err = RUADestroyInstance(PRUA);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Can not close RUA! %s\n", RMstatusToString(err)));
        return;
    }
    FUNC_LEAVE();
    return;
}

static int GFX_FillHWRect(_THIS, GAL_Surface * surface, GAL_Rect * rect, Uint32 color)
{
    FUNC_ENTER();
    struct GFXEngine_FillRectangle_type fill_param;
    struct GFXEngine_Surface_type surface_param;
    struct GFXEngine_ColorFormat_type format_param;
    Sigma8654Private_HWSurface_hwdata* hwdata = (Sigma8654Private_HWSurface_hwdata*) surface->hwdata;
    format_param.SurfaceID = GFX_SURFACE_ID_NX;
    format_param.MainMode = DCC_PROFILE.ColorMode;
    format_param.SubMode = DCC_PROFILE.ColorFormat;
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_ColorFormat, &format_param, sizeof(format_param));
    //printf(">>>>Enter GFX_FillHWRect ColorMode is %d, ColorFormat is %d\n", DCC_PROFILE.ColorMode, DCC_PROFILE.ColorFormat);
    surface_param.SurfaceID = GFX_SURFACE_ID_NX;

    surface_param.StartAddress = hwdata->gfx_addr;
    surface_param.TotalWidth = (RMuint32)surface->w;
    surface_param.Tiled = FALSE;
    //printf(">>>>Enter GFX_FillHWRect x is %d, y is %d, w=%d, h=%d\n", rect->x, rect->y, rect->w, rect->h);
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Surface, &surface_param, sizeof(surface_param));
    fill_param.X = rect->x;
    fill_param.Y = rect->y;
    fill_param.Width = rect->w;
    fill_param.Height = rect->h;
    fill_param.Color = color;
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_FillRectangle, &fill_param, sizeof(fill_param));

    FUNC_LEAVE();
    return 0;
}

static int init_GFXengine(_THIS)
{
    FUNC_ENTER();
    RMuint32 err, gfx_count, i;
    struct GFXEngine_DRAMSize_in_type  dramSizeIn;
    struct GFXEngine_DRAMSize_out_type dramSizeOut;

    dramSizeIn.CommandFIFOCount = 10;
    err = RUAExchangeProperty(PRUA,
            EMHWLIB_MODULE(GFXEngine, 0),
            RMGFXEnginePropertyID_DRAMSize,
            &dramSizeIn, sizeof(dramSizeIn),
            &dramSizeOut, sizeof(dramSizeOut));
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error getting dram size for gfx engine\n");
        return 0;
    }

    GFX_PROFILE.CommandFIFOCount = dramSizeIn.CommandFIFOCount;
    GFX_PROFILE.Priority = 1;
    GFX_PROFILE.CachedSize = dramSizeOut.CachedSize;
    GFX_PROFILE.UncachedSize = dramSizeOut.UncachedSize;

    if (GFX_PROFILE.CachedSize > 0) {
        fprintf (stderr, "NEWGAL>EM86GFX: about to malloc cached, size %ld\n", 
                GFX_PROFILE.CachedSize);
        GFX_PROFILE.CachedAddress = RUAMalloc(PRUA, 0, RUA_DRAM_CACHED, 
                GFX_PROFILE.CachedSize);
    }
    else
        GFX_PROFILE.CachedAddress = 0;

    GFX_PROFILE.UncachedSize = dramSizeOut.UncachedSize;
    if (GFX_PROFILE.UncachedSize > 0)
        GFX_PROFILE.UncachedAddress = RUAMalloc(PRUA, 0, RUA_DRAM_UNCACHED, 
                GFX_PROFILE.UncachedSize);
    else
        GFX_PROFILE.UncachedAddress = 0;

    GFX_TARGET = GFXEngine;
    err = RUAExchangeProperty(PRUA,
            EMHWLIB_MODULE(Enumerator,0),
            RMEnumeratorPropertyID_CategoryIDToNumberOfInstances,
            &GFX_TARGET, sizeof(GFX_TARGET),
            &gfx_count, sizeof(gfx_count));
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error getting gfx engine count\n");
        return 0;
    }

    for (i = 0; i < gfx_count; i++) {
        GFX_TARGET = EMHWLIB_MODULE(GFXEngine, i);
        RMDBGLOG((ENABLE, "Trying on target %ld\n", i));
        err = RUASetProperty(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Open, &GFX_PROFILE, sizeof(GFX_PROFILE), 0);
        if (err == RM_OK) 
            break;
    }

    if (i == (RMint32)gfx_count) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot open a gfx engine [0..%lu[\n", gfx_count);
        return 0;
    }
    RMDBGLOG((ENABLE, "opened target %ld\n", i));

    this->info.blit_hw = 1;
    this->info.blit_hw_CC = 1;
    this->info.blit_hw_A = 1;
    this->info.blit_fill = 1;

    FUNC_LEAVE();
    return 1;
}


static int GFX_AllocHWSurface(_THIS, GAL_Surface * surface)
{
    FUNC_ENTER();
    //printf("surface->pitch = %d, surface->w = %d, surface->h = %d\n", surface->pitch,  surface->w, surface->h);
    Sigma8654Private_HWSurface_hwdata* hwdata = calloc(1, sizeof(Sigma8654Private_HWSurface_hwdata));
    RMstatus err;
    struct DCCOSDProfile profile = (DCC_PROFILE);
    profile.Width = surface->w;
    profile.Height = surface->h;
    RMuint32 luma_addr, luma_size; 
    RMuint8* pLuma;

    struct DisplayBlock_SurfaceSize_in_type dram_in;
    struct DisplayBlock_SurfaceSize_out_type dram_out;
    struct DisplayBlock_InitSurface_in_type valueIn;
    struct DisplayBlock_InitSurface_out_type valueOut;
    RMuint32 addr;

    dram_in.ColorMode = profile.ColorMode;
    dram_in.ColorFormat = profile.ColorFormat;
    dram_in.SamplingMode = profile.SamplingMode;
    dram_in.Width = profile.Width;
    dram_in.Height = profile.Height;
    err = RUAExchangeProperty(PRUA, DisplayBlock, RMDisplayBlockPropertyID_SurfaceSize, 
            &dram_in, sizeof(dram_in), &dram_out, sizeof(dram_out));
    if (RMFAILED(err)) {
        free(hwdata);
        RMDBGLOG((ENABLE, "Error Cannot get size to allocate the OSD surface, %s\n", RMstatusToString(err)));
        return err;
    }
    addr = RUAMalloc(PRUA, 0, RUA_DRAM_UNCACHED, dram_out.BufferSize); 
    if (addr == 0) {
        RMDBGLOG((ENABLE, "ERROR: could not allocate 0x%08lX bytes in system memory %lu!\n", dram_out.BufferSize));
        return RM_FATALOUTOFMEMORY;
    }

    valueIn.ColorMode = dram_in.ColorMode;
    valueIn.ColorFormat = dram_in.ColorFormat;
    valueIn.SamplingMode = dram_in.SamplingMode;
    valueIn.Width = dram_in.Width;
    valueIn.Height = dram_in.Height;
    valueIn.Address = addr;
    valueIn.LumaSize = dram_out.LumaSize;
    valueIn.ChromaSize = dram_out.ChromaSize;
    valueIn.ColorSpace = profile.ColorSpace;
    valueIn.PixelAspectRatio = profile.PixelAspectRatio;
    err = RUAExchangeProperty(PRUA, DisplayBlock, RMDisplayBlockPropertyID_InitSurface, 
            &valueIn, sizeof(valueIn), &valueOut, sizeof(valueOut));
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot initialize OSD surface, %s\n", RMstatusToString(err)));
        return err;
    }

    hwdata->surface_addr = addr;
    luma_size = valueIn.LumaSize;
    luma_addr = valueOut.LumaAddress;
    //chroma_size = valueIn.CHROMA_SIZE;
    //chorma_addr = valueOut.ChromaAddress;

    err = RUALock(PRUA, luma_addr, luma_size);
    if (RMFAILED(err)) {
        return err;
    }

    //printf("lumaAddr: pointer %p size: %d\n", (void*)luma_addr,(int)luma_size);
    pLuma = RUAMap(PRUA,luma_addr, luma_size); 
    if (pLuma == NULL) {
        return RM_ERROR;
    }

    hwdata->unmapped_addr = luma_addr;
    hwdata->size=luma_size;
    hwdata->gfx_addr =luma_addr; 
    surface->pixels = (void*)pLuma;
    surface->hwdata = (void*)hwdata;
    surface->flags |= GAL_HWSURFACE;
    surface->flags |= GAL_PREALLOC;
    
    struct hwsrfc_item* item = malloc(sizeof(struct hwsrfc_item));
    item->surface = surface;
    list_add(&item->list, &surface_list);
    //printf("hw surface pixels=%p, mapped_addr=%p, osd_source is %p\n", surface->pixels, (void*)hwdata->gfx_addr, hwdata->unmapped_addr);
    //printf(">>>>Leave GFX_AllocHWSurface().\n");
    FUNC_LEAVE();
    return 0;
}

static void __GFX_FreeHWSurface(_THIS, GAL_Surface* surface) 
{
    FUNC_ENTER();
    if(!surface) {
        printf("surface == NULL\n");
        return;
    }
    Sigma8654Private_HWSurface_hwdata* hwdata = (Sigma8654Private_HWSurface_hwdata*)surface->hwdata;

    RMuint32 err;
    if(!((Sigma8654Private_HWSurface_hwdata*)surface->hwdata)->gfx_addr){
        printf(" gfx == NULL\n");
        return;
    };

    RUAUnMap(PRUA, surface->pixels, hwdata->size);
    err = RUAUnLock(PRUA, hwdata->unmapped_addr, hwdata->size);
    if (RMFAILED(err)) {
        printf("can not unmapped hwdata->unmapped_addr\n");
        return;
    }

    RUAFree(PRUA, hwdata->surface_addr);
    free(surface->hwdata);
    surface->hwdata = NULL;
    hwdata->gfx_addr = 0;
    FUNC_LEAVE();
 
}
static void GFX_FreeHWSurface(_THIS, GAL_Surface * surface) 
{
    __GFX_FreeHWSurface(this, surface);

    struct list_head* ptr;
    struct hwsrfc_item* item; 
    list_for_each(ptr, &surface_list){
        item = list_entry(ptr, struct hwsrfc_item, list);
        if(item->surface == surface) {
            list_del(ptr);
            break;
        }
    }
    free(item);
}

static int GFX_CheckHWBlit (_THIS, GAL_Surface* src, GAL_Surface* dst)
{
    if ((src->flags & GAL_HWSURFACE)
            && (src->format->Rmask == dst->format->Rmask)
            && (src->format->Gmask == dst->format->Gmask)
            && (src->format->Bmask == dst->format->Bmask)
            && (src->format->Amask == dst->format->Amask)) {
        src->flags |= GAL_HWACCEL;
        src->map->hw_blit = GFX_HWAccelBlit;
        return 1;
    }
    return 0;
}

static int GFX_HWAccelBlit (GAL_Surface* src, GAL_Rect* srec, GAL_Surface* dst, GAL_Rect* drec)
{
    FUNC_ENTER();
    GAL_VideoDevice * this = current_video;
    struct GFXEngine_BlendAndScaleRectangles_type blend_param;
    struct GFXEngine_Surface_type surface_param;
    struct GFXEngine_ColorFormat_type format_param;
    Sigma8654Private_HWSurface_hwdata* src_hwdata = (Sigma8654Private_HWSurface_hwdata*) src->hwdata;
    Sigma8654Private_HWSurface_hwdata* dst_hwdata = (Sigma8654Private_HWSurface_hwdata*) dst->hwdata;

    format_param.MainMode = DCC_PROFILE.ColorMode;
    format_param.SubMode = DCC_PROFILE.ColorFormat;
    format_param.SamplingMode = DCC_PROFILE.SamplingMode;
    format_param.ColorSpace = DCC_PROFILE.ColorSpace; 

    surface_param.Tiled = FALSE;

    //destination
    surface_param.StartAddress = dst_hwdata->gfx_addr; 
    surface_param.TotalWidth = (RMuint32)dst->w; 
    format_param.SurfaceID = GFX_SURFACE_ID_NX;
    surface_param.SurfaceID = GFX_SURFACE_ID_NX;
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_ColorFormat, &format_param, sizeof(format_param));
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Surface, &surface_param, sizeof(surface_param));

    //source of pixels
    surface_param.StartAddress = src_hwdata->gfx_addr; 
    surface_param.TotalWidth = (RMuint32)src->w;
    surface_param.SurfaceID = GFX_SURFACE_ID_X;
    format_param.SurfaceID = GFX_SURFACE_ID_X;
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_ColorFormat, &format_param, sizeof(format_param));
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Surface, &surface_param, sizeof(surface_param));

    //source of alpha 
    surface_param.StartAddress = src_hwdata->gfx_addr; 
    surface_param.TotalWidth = (RMuint32)src->w;
    format_param.SurfaceID = GFX_SURFACE_ID_Z;
    surface_param.SurfaceID = GFX_SURFACE_ID_Z;
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_ColorFormat, &format_param, sizeof(format_param));
    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_Surface, &surface_param, sizeof(surface_param));

    blend_param.Src1X = srec->x;
    blend_param.Src1Y = srec->y;
    blend_param.Src2X = drec->x;
    blend_param.Src2Y = drec->y;
    blend_param.SrcWidth = srec->w;
    blend_param.SrcHeight = srec->h;
    blend_param.DstX = drec->x;
    blend_param.DstY = drec->y;
    blend_param.DstWidth = drec->w;
    blend_param.DstHeight = drec->h;
    blend_param.SaturateAlpha = 0;

    SEND_GFX_COMMAND(PRUA, GFX_TARGET, RMGFXEnginePropertyID_BlendAndScaleRectangles, &blend_param, sizeof(blend_param));
    FUNC_LEAVE();
    return 0;
}

#endif /* _MGGAL_SIGMA8654*/

