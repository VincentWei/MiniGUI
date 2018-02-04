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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h> 

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "list.h"

#ifdef _MGGAL_MSTAR
#include "mstar_video.h"

#define SDL_SURFACE_ALPHA_BLEND_TYPE 0x000F0000

#define DEV_MEM     "/dev/mem"
#define MMAP_DFB_FRAME_BUFFER ("DFB_FRAME_BUFFER")
#define MMAP_GE_VQ_BUFFER      ("GE_VQ_BUFFER")
#define MMAP_GOP_REGDMABASE    ("GOP_REGDMABASE_APP")

#undef USE_MSTAR_CHAKRA
#undef USE_MUTI_LAYER 
#define VIDEO_OFFSET	0x38000000 
#define SURFACE_GAP 0x10 
#define GFX_CURSOR_OFFSET 0 
#define IS_MSTAR_BLIT_OP(op)  ((op) & 0xffff0000)
#define MStarVID_DRIVER_NAME "mstar" 

#define FUNC_ENTER()    fprintf(stderr, ">>> entering %s %s\n", __FILE__, __FUNCTION__)
#define FUNC_LEAVE()    fprintf(stderr, "<<< leaving  %s %s\n", __FILE__, __FUNCTION__)
#define dbg()    fprintf(stderr, "%s %d\n", __FUNCTION__, __LINE__)
#define GLH_DEBUG(x...) //fprintf(stderr,x)

typedef MS_S8                   FONTHANDLE;      ///< Font handle
typedef MS_S16                  BMPHANDLE;       ///< Bitmap handle
typedef MS_S8                   DBHANDLE;

typedef enum
{
    MSTAR_FILL_RECT = 0x00000001,
    MSTAR_DRAW_RECT = 0x00000002,
    MSTAR_DRAW_LINE = 0x00000004,
    MSTAR_TRAPEZOID_FILL = 0x00000008,
    MSTAR_BIT_BLIT = 0x00010000,
    MSTAR_STRETCH = 0x00020000,
    MSTAR_TRAPEZOID_BLIT = 0x00040000,
    MSTAR_OPT_MAX = 0xffffffff
}MSTAR_OP_TYPE; 


/* Initialization/Query functions */
static GAL_VideoDevice * MStar_CreateDevice(int devindex);
static int MStar_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **MStar_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *MStar_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static void MStar_VideoQuit(_THIS);

/* Hardware surface functions */
static int MStar_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size);
static int MStar_AllocHWSurface(_THIS, GAL_Surface *surface);
static int MStar_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *dstrect, Uint32 color);
static void MStar_FreeHWSurface(_THIS, GAL_Surface *surface);
static int MStar_CheckHWBlit(_THIS, GAL_Surface *src, GAL_Surface *dst);
static int MStar_HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect);
static int MStar_SetHWColorKey(_THIS, GAL_Surface *surface, Uint32 key);
static int MStar_SetHWAlpha(_THIS, GAL_Surface *surface, Uint8 alpha);

/* Various screen update functions available */
static void MStar_DirectUpdate(_THIS, int numrects, GAL_Rect *rects);
static int MapMemAndReg(_THIS);


static Uint32 GALFmtToMStarFmt(GAL_PixelFormat  *fmt)
{
    if(fmt->BitsPerPixel == 16)
        return GFX_FMT_ARGB1555;
    else if(fmt->BitsPerPixel == 32)
        return GFX_FMT_ARGB8888;
    else if(fmt->BitsPerPixel == 8)
        return GFX_FMT_I8;
    else
        return GFX_FMT_ARGB1555;
}

static int MStar_Available(void)
{
    return (1);
} 

VideoBootStrap MSTAR_bootstrap = {
    MStarVID_DRIVER_NAME, "MStar video driver",
    MStar_Available, MStar_CreateDevice
};


//static GAL_Rect* GAL_modearray[3];
static GAL_Rect* GAL_modearray[6];

/* This is the rect EnumModes2 uses */
/*
struct MStarEnumRect {
    GAL_Rect r;
    struct MStarEnumRect* next;
};*/

/* MStar driver bootstrap functions */ 
static void MStar_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice * MStar_CreateDevice(int devindex)
{
    FUNC_ENTER();
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *) malloc(sizeof(GAL_VideoDevice));
    if (device)
    {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *) malloc (sizeof (*device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL))
    {
        GAL_OutOfMemory();
        if (device)
        {
            free(device);
        }
        return (0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = MStar_VideoInit;
    device->ListModes = MStar_ListModes;
    device->SetVideoMode = MStar_SetVideoMode;
    device->SetColors = NULL; 
    device->VideoQuit = MStar_VideoQuit;
    device->AllocHWSurface = MStar_AllocHWSurface;
    device->CheckHWBlit = MStar_CheckHWBlit;
    device->FillHWRect = MStar_FillHWRect;
    device->SetHWColorKey = MStar_SetHWColorKey;
    device->SetHWAlpha = MStar_SetHWAlpha;
    device->FreeHWSurface = MStar_FreeHWSurface;
    device->UpdateRects = MStar_DirectUpdate;
    device->UpdateSurfaceRects = NULL; 
    device->free = MStar_DeleteDevice;

  	MAdp_MSGCH_Init(); 
    return device;
}


static Uint32 mstar_build_color(Uint32 pixel,  GAL_PixelFormat *fmt)
{
    return 0xFF000000 | ((pixel & fmt->Rmask) << (16 - fmt->Rshift + fmt->Rloss)) |
        ((pixel & fmt->Gmask) << (8 - fmt->Gshift + fmt->Gloss)) |
        ((pixel & fmt->Bmask) << (fmt->Bloss - fmt->Bshift));
}

static MS_U32 mstarGFXAddr(unsigned long phys)
{
#ifdef MSTAR_T3_PLATFORM 
    if(phys >= 0x40000000)
        return (phys-VIDEO_OFFSET);
    return phys;
#else
    return phys;
#endif
}

static MS_U32 mstar_SDL_OSD_RESOURCE_SetFBFmt(MS_U16 pitch,MS_U32 addr , MS_U16 fmt )
{
   GLH_DEBUG("set osd resource %08x, %d, %04x\n", addr, pitch, fmt);
   return 0;
}

static MS_BOOL mstar_sc_is_interlace(void)
{
    return TRUE;
}
static MS_U16 mstar_sc_get_h_cap_start(void)
{
    return (MS_U16)MAdp_SYS_GetPanelHStart();
}

static void mstar_XC_Sys_PQ_ReduceBW_ForOSD(MS_U8 PqWin, MS_BOOL bOSD_On)
{
	  GLH_DEBUG("\nmstar_XC_Sys_PQ_ReduceBW_ForOSD>>>PqwIN=%d,bOSD_On=%d.",PqWin,bOSD_On);
}

static MS_U32 mstar_OSD_RESOURCE_GetFontInfoGFX(FONTHANDLE handle, GFX_FontInfo* pinfo)   
{
   printf("Should not arrive here!!!\n");
   return 0;
}
static MS_U32 msAPI_OSD_RESOURCE_GetBitmapInfoGFX(BMPHANDLE handle, GFX_BitmapInfo* pinfo)
{
   printf("Should not arrive here!!!\n");
   return 0;
}

static int driver_init_device(MSTARDeviceData * sdev)
{
    unsigned short pnlW;
    unsigned short pnlH;
    GOP_InitInfo gopInitInfo;

    Uint32 u32GOP_Regdma_addr,u32GE_VQ_aligned,u32GE_VQ_miu;
    Uint32 u32GOP_Regdma_size,u32GOP_Regdma_aligned,u32GOP_Regdma_miu;
    Uint32 u32GE_VQ_addr;
    Uint32 u32VQBufSize;
    GFX_Config gfx_config;
    memset(sdev,0,sizeof(MSTARDeviceData));

    sdev->src_ge_format = GFX_FMT_ARGB8888;
    sdev->dst_ge_format = GFX_FMT_ARGB8888;
    sdev->ge_render_coef = 0xFFFFFFFF;
    sdev->ge_render_alpha_from = (unsigned int)ABL_FROM_ASRC;

    gfx_config.u8Dither = FALSE;
    gfx_config.bIsCompt = FALSE; 
    gfx_config.bIsHK = TRUE;
    gfx_config.u8Miu = 1;

#ifdef MSTAR_T3_PLATFORM    
    sdev->gfx_gop_index = MAdp_SYS_GetGFXGOPIndex();
#else
    sdev->gfx_gop_index = 0;
#endif
    MsOS_Init();
    MDrv_SEM_Init(); 
#ifdef USE_MSTAR_CHAKRA		
    APP_REGISTER_INFO stAppReg;
    MAdp_APMNG_Init();
    //MAdp_IR_Init();
    stAppReg.enType = KEY_DISPATCH_APP;
    stAppReg.exit = NULL;
    stAppReg.resume = NULL;
    stAppReg.suspend = NULL;
    MAdp_APMNG_AppRegister(&stAppReg);
#endif

    MAdp_SYS_GetPanelResolution(&pnlW,&pnlH); 
    sdev->gfx_max_width = 0;
    sdev->gfx_max_height = 0;
    sdev->lcd_width = pnlW;
    sdev->lcd_height= pnlH; 
    sdev->gfx_max_width  &= ~3; 
    sdev->gfx_h_offset = 0;
    sdev->gfx_v_offset= 0;

    gopInitInfo.u16PanelWidth = pnlW;
    gopInitInfo.u16PanelHeight= pnlH;
    gopInitInfo.u16PanelHStr = MAdp_SYS_GetPanelHStart();
    gopInitInfo.u32GOPRBAdr = 0;
    gopInitInfo.u32GOPRBLen = 0;
    gopInitInfo.bEnableVsyncIntFlip = TRUE;

#ifndef MSTAR_T3_PLATFORM
    u32GOP_Regdma_addr= GOP_REGDMABASE_ADR;
    u32GOP_Regdma_size = GOP_REGDMABASE_LEN; 
#else
    u32GOP_Regdma_addr= 0;
    u32GOP_Regdma_size = 0; 

    if(!MAdp_SYS_GetMemoryInfo(MMAP_GOP_REGDMABASE, (unsigned long *)&u32GOP_Regdma_addr, (unsigned long *)&u32GOP_Regdma_size,
                (unsigned long *)&u32GOP_Regdma_aligned, (unsigned long *)&u32GOP_Regdma_miu)) {
        u32GOP_Regdma_addr = 0;
        u32GOP_Regdma_size = 0;
    }
#endif

    gopInitInfo.u32GOPRegdmaAdr = u32GOP_Regdma_addr;
    gopInitInfo.u32GOPRegdmaLen = u32GOP_Regdma_size;

    MApi_GOP_RegisterFBFmtCB(mstar_SDL_OSD_RESOURCE_SetFBFmt);
    MApi_GOP_RegisterXCIsInterlaceCB(mstar_sc_is_interlace);
    MApi_GOP_RegisterXCGetCapHStartCB(mstar_sc_get_h_cap_start);
    MApi_GOP_RegisterXCReduceBWForOSDCB(mstar_XC_Sys_PQ_ReduceBW_ForOSD);
#ifdef MSTAR_T3_PLATFORM 
#ifdef USE_MUTI_LAYER   
    MApi_GOP_InitByGOP(&gopInitInfo,  0);
    MApi_GOP_GWIN_SwitchGOP(0);
    MApi_GOP_GWIN_SetGOPDst(0, 2);
#endif

    MApi_GOP_InitByGOP(&gopInitInfo,  2);
    MApi_GOP_GWIN_SwitchGOP(2);
    MApi_GOP_GWIN_SetGOPDst(2, 2);
#else
    MApi_GOP_InitByGOP(&gopInitInfo,  sdev->gfx_gop_index);
#endif

    MApi_GOP_GWIN_EnableTransClr(GOPTRANSCLR_FMT0, FALSE); 
    MApi_GOP_GWIN_SetTransClr_8888(0xFF000000,0x00FFFFFF);
    MApi_GFX_RegisterGetFontCB(mstar_OSD_RESOURCE_GetFontInfoGFX);
    MApi_GFX_RegisterGetBMPCB(msAPI_OSD_RESOURCE_GetBitmapInfoGFX);  

#ifndef MSTAR_T3_PLATFORM
    u32GE_VQ_addr = GE_VQ_BUFFER_ADR + VIDEO_OFFSET;
    u32VQBufSize = GE_VQ_BUFFER_LEN;
#else
    u32GE_VQ_addr = 0;
    u32VQBufSize = 0;
    if(!MAdp_SYS_GetMemoryInfo(MMAP_GE_VQ_BUFFER, (unsigned long *)&u32GE_VQ_addr, (unsigned long *)&u32VQBufSize,
                (unsigned long *)&u32GE_VQ_aligned, (unsigned long *)&u32GE_VQ_miu)) {
        u32GE_VQ_addr = 0;
        u32VQBufSize = 0;
    }
#endif

    gfx_config.u32VCmdQAddr = u32GE_VQ_addr;
    gfx_config.u32VCmdQSize =  u32VQBufSize ;
    GLH_DEBUG("\nafter MApi_GFX_Init!!!1");
    MApi_GFX_Init(&gfx_config);

    if(u32VQBufSize>=4*1024) {
        GFX_VcmqBufSize vqSize;

        if(u32VQBufSize>=512*1024)
            vqSize = GFX_VCMD_512K;
        else if(u32VQBufSize>=256*1024)
            vqSize = GFX_VCMD_256K;
        else if(u32VQBufSize>=128*1024)
            vqSize = GFX_VCMD_128K;          
        else if(u32VQBufSize>=64*1024)
            vqSize = GFX_VCMD_64K;  
        else if(u32VQBufSize>=32*1024)
            vqSize = GFX_VCMD_32K;    
        else if(u32VQBufSize>=16*1024)
            vqSize = GFX_VCMD_16K;  
        else if(u32VQBufSize>=8*1024)
            vqSize = GFX_VCMD_8K;    
        else
            vqSize = GFX_VCMD_4K; 

        MApi_GFX_SetVCmdBuffer(u32GE_VQ_addr, vqSize);
        MApi_GFX_EnableVCmdQueue(TRUE);
        GLH_DEBUG("\nMApi_GFX_EnableVCmdQueue TRUE,vq=%d.",vqSize);
    } else {
        MApi_GFX_EnableVCmdQueue(FALSE);
    } 
    return 0;
}

int MStar_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    GAL_Rect *rect;
    MAdp_SYS_Init();
    MDrv_MMIO_Init();

    if(!MDrv_SYS_Init()) {
        GLH_DEBUG("MStar_VideoInit>>>sys_init failed!!!");
        return -1;
    }

    MapMemAndReg(this);
    driver_init_device(&HIDDEN->sdev);

    rect = malloc(sizeof(GAL_Rect)*5);
    GAL_modearray[0] = rect;
    rect[0].x = 0;
    rect[0].y = 0;
    rect[0].w = 720;
    rect[0].h = 480;

    rect[1].x = 0;
    rect[1].y = 0;
    rect[1].w = 720;
    rect[1].h = 576;

    rect[2].x = 0;
    rect[2].y = 0;
    rect[2].w = 1280;
    rect[2].h = 720;

    rect[3].x = 0;
    rect[3].y = 0;
    rect[3].w = 1366;
    rect[3].h = 768;

    rect[4].x = 0;
    rect[4].y = 0;
    rect[4].w = 1920;
    rect[4].h = 1080;
    GAL_modearray[1] = &rect[1];
    GAL_modearray[2] = &rect[2];
    GAL_modearray[3] = &rect[3];
    GAL_modearray[4] = &rect[4];
    GAL_modearray[5] = NULL;

    this->info.hw_available = 1;
    this->info.blit_hw      = 1;
    this->info.blit_hw_CC   = 1;
    this->info.blit_hw_A    = 1;
    this->info.blit_fill    = 1;
    this->info.video_mem    = DFB_FRAME_BUFFER_LEN/1024; 
    HIDDEN->initialized = 1;
    GLH_DEBUG("\nMStar_VideoInit>>>leave!!!");
    return 0;
}

static GAL_Rect **MStar_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if ((format->BitsPerPixel == 32)) {
        return GAL_modearray;
    }
    return NULL;
}

static int MapMemAndReg(_THIS)
{
    FUNC_ENTER(); 
    int fd;
    printf("MSTAR_T3_PLATFORM\n");
    unsigned long dfb_base,video_aligned,video_miu,video_len;
    unsigned int reg_phys = 0;
    unsigned int reg_length = 0;

    if(!MAdp_SYS_GetMemoryInfo(MMAP_DFB_FRAME_BUFFER, &dfb_base,&video_len, &video_aligned, &video_miu))
    {
        GLH_DEBUG("\nMapMemAndReg>>>ERROR!!!");
        return -1;
    }

    GLH_DEBUG("\nMapMemAndReg>>>dfbbase=%x,len=%x,align=%d,miu=%d.",dfb_base,video_len,video_aligned,video_miu);
    HIDDEN->dfb_base = DFB_FRAME_BUFFER_ADR + MIU_DRAM_LEN0;

    if(MADP_MIU1 == video_miu)
    {
        GLH_DEBUG("\nMapMemAndReg>>>MIU1");
        HIDDEN->dfb_base += VIDEO_OFFSET;
    }

    HIDDEN->video_length = DFB_FRAME_BUFFER_LEN;

    HIDDEN->video_miu = video_miu;
    GLH_DEBUG("\nMapMemAndReg>>>MAdp_SYS_GetMemoryInfo:::%x,%x,%x,%x.",HIDDEN->dfb_base,HIDDEN->video_length,video_aligned,video_miu);

    fd = open( DEV_MEM, O_RDWR | O_SYNC ); 
    if (fd < 0) {
        GLH_DEBUG( "System/DevMem: Opening '%s' failed!\n", DEV_MEM );
        return -1;
    }

    HIDDEN->video_phys = mmap( NULL, HIDDEN->video_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, HIDDEN->dfb_base);
    if (HIDDEN->video_phys == MAP_FAILED) {
        GLH_DEBUG( "System/DevMem: Mapping %d bytes at 0x%08lx via '%s' failed!\n", GE_FRAMEBUFFER_LEN, GE_FRAMEBUFFER_ADR, DEV_MEM );
        return -1;
    }

    if (reg_phys && reg_length) {
        HIDDEN->mmio_phys = mmap( NULL, reg_length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, reg_phys );
        if (HIDDEN->mmio_phys == MAP_FAILED) {
            GLH_DEBUG( "System/DevMem: Mapping %d bytes at 0x%08lx via '%s' failed!\n", reg_length, reg_phys, DEV_MEM );
            munmap( HIDDEN->video_phys, reg_length );
            close( fd );
            return -1;
        }
    }
    close( fd );
    GLH_DEBUG("\nMapMemAndReg>>>%x,%x",HIDDEN->mmio_phys,HIDDEN->video_phys);
    FUNC_LEAVE();
    return 0;
}

static GAL_Surface* MStar_SetVideoMode(_THIS, GAL_Surface * surface, int width, int height, int bpp, Uint32 flags)
{
    FUNC_ENTER();
    int pitch;
    int dbuf = 1;
    unsigned char u8GOP_Ret,u8FBId;
    unsigned long phys_start;
    MS_ColorFormat mfmt = GFX_FMT_ARGB8888;

    int sdl_gwinid;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask,Amask;
    char *surfaces_mem;
    int surfaces_len;
    HIDDEN->sdev.gfx_max_width = width;
    HIDDEN->sdev.gfx_max_height = height;

    HIDDEN->sdev.gfx_max_width &= ~3;

    pitch = (((width * 4)+15)/16)*16;
    phys_start = HIDDEN->video_length - dbuf * (pitch * height + SURFACE_GAP); 
    u8GOP_Ret = MApi_GOP_GWIN_CreateFBFrom3rdSurf(width, height, mfmt, HIDDEN->dfb_base + phys_start, pitch, &u8FBId); 
    HIDDEN->u16FBId[0] = u8FBId;
    switched_away = pitch * height + SURFACE_GAP;
    HIDDEN->bdoublebuf = 0;
    HIDDEN->flip = 0;
    sdl_gwinid = MApi_GOP_GWIN_CreateWin_Assign_FB(HIDDEN->sdev.gfx_gop_index, u8FBId, 0, 0);
    MApi_GOP_GWIN_SetGWinShared(sdl_gwinid,1); 
    MApi_GOP_GWIN_SetGWinSharedCnt(sdl_gwinid,1);

    if(HIDDEN->sdev.gfx_max_width == HIDDEN->sdev.lcd_width) {
        MApi_GOP_Initialize_StretchWindow(HIDDEN->sdev.gfx_gop_index);
        MApi_GOP_GWIN_Set_HSCALE(FALSE, 1, 1);
        MApi_GOP_GWIN_Set_VSCALE(FALSE, 1, 1);
    } else {
        MApi_GOP_GWIN_Set_STRETCHWIN(HIDDEN->sdev.gfx_gop_index, E_GOP_DST_OP0, 0, 0,
                HIDDEN->sdev.gfx_max_width, HIDDEN->sdev.gfx_max_height);
        MApi_GOP_GWIN_Set_HSCALE(TRUE, HIDDEN->sdev.gfx_max_width, HIDDEN->sdev.lcd_width) ;
        MApi_GOP_GWIN_Set_VSCALE(TRUE, HIDDEN->sdev.gfx_max_height, HIDDEN->sdev.lcd_height) ;
    }

    //MApi_GOP_GWIN_SetBlending(sdl_gwinid, TRUE, 0xFF>>2); 
    MApi_GOP_GWIN_SetBlending(sdl_gwinid, TRUE, 0xff); 
    MApi_GOP_GWIN_SetWinPosition(sdl_gwinid,0,0);  
    MApi_GOP_GWIN_Enable(sdl_gwinid, TRUE) ; 

    surface->w     = width;
    surface->h     = height;
    surface->flags = GAL_HWSURFACE | GAL_FULLSCREEN;

    surface->pixels = HIDDEN->video_phys + phys_start;
    surface->pitch = pitch;
    surface->offset = 0;
    HIDDEN->gwinid = sdl_gwinid;
    HIDDEN->layerid[0] = sdl_gwinid;

    Rmask = 0x00FF0000;
    Gmask = 0x0000FF00;
    Bmask = 0x000000FF;
    Amask = 0xFF000000;
    if (!GAL_ReallocFormat(surface, 32,Rmask, Gmask, Bmask, Amask) ) {
        return(NULL);
    }

    /* Set up the information for hardware surfaces */
    surfaces_mem = (char *)(HIDDEN->video_phys + GFX_CURSOR_OFFSET);
    surfaces_len = ( HIDDEN->video_length - switched_away * dbuf - GFX_CURSOR_OFFSET);
    memset(surface->pixels,0x00,switched_away * dbuf);
    GLH_DEBUG("\nafter memset>>>pixels=%x,surfaces_len=%d", surface->pixels,surfaces_len);

    //MStar_FreeHWSurfaces(this);
    vidmem_bucket *bucket, *freeable; 
    bucket = surfaces.next;
    while ( bucket ) {
        freeable = bucket;
        bucket = bucket->next;
        free(freeable);
    }
    surfaces.next = NULL;

    MStar_InitHWSurfaces(this, surface, surfaces_mem, surfaces_len);
    GLH_DEBUG("\nMStar_SetVideoMode>>>leave,flags=%x.",flags);
    FUNC_LEAVE();
    return surface;
}

static int CheckAlphaBlendType(MSTARDeviceData *sdev,GAL_Surface *src,GAL_Surface * dst)
{
    sdev->ge_render_coef = COEF_ONE;
    sdev->ge_src_colorkey_enabled  = 0;
    sdev->ge_dst_colorkey_enabled = 0;
    sdev->dst_ge_format = GALFmtToMStarFmt(dst->format);
    sdev->src_ge_format = GALFmtToMStarFmt(src->format);

    if(src->flags & GAL_SRCCOLORKEY) {
        sdev->ge_src_colorkey_enabled = 1;
        sdev->src_ge_clr_key = mstar_build_color(src->format->colorkey,src->format);
    }

    if(src->flags & GAL_SRCALPHA) {
        //printf("GAL_SRCALPHA\n");
        sdev->ge_render_coef = COEF_CONST; 
        sdev->ge_render_alpha_from = (Uint32)ABL_FROM_ADST;
        sdev->ge_alpha_blend_enabled = 1;
        sdev->color16 = src->format->alpha;
    }else if(src->flags & GAL_SRCPIXELALPHA) {
        //printf("GAL_SRCPIXELALPHA\n");
        sdev->ge_render_coef = COEF_ASRC;
        sdev->ge_render_alpha_from = (Uint32)ABL_FROM_ADST;
        sdev->ge_alpha_blend_enabled = 1;
        sdev->color16 = 0xFFFFFFFF;
    } else {
        sdev->ge_render_alpha_from = (unsigned int)ABL_FROM_ASRC;
        sdev->ge_alpha_blend_enabled = 0;
    } 
    return 0;  	
}

static void mstarSetEngineState(MSTAR_OP_TYPE op, MSTARDeviceData *sdev)
{
    if(IS_MSTAR_BLIT_OP(op)) {
        MApi_GFX_SetSrcColorKey(sdev->ge_src_colorkey_enabled,
                CK_OP_EQUAL, (GFX_Buffer_Format)sdev->src_ge_format, &sdev->src_ge_clr_key, &sdev->src_ge_clr_key);
    } else {
        MApi_GFX_SetSrcColorKey(FALSE,
                CK_OP_EQUAL, (GFX_Buffer_Format)sdev->src_ge_format, &sdev->src_ge_clr_key, &sdev->src_ge_clr_key);
    }

    MApi_GFX_SetDstColorKey(FALSE,CK_OP_NOT_EQUAL, 
            (GFX_Buffer_Format)sdev->dst_ge_format, &sdev->dst_ge_clr_key, &sdev->dst_ge_clr_key);

    if(sdev->ge_alpha_blend_enabled) {
        MApi_GFX_EnableAlphaBlending(TRUE);
        MApi_GFX_SetAlphaBlending((GFX_BlendCoef)sdev->ge_render_coef, sdev->color16);
    } else {
        MApi_GFX_EnableAlphaBlending(FALSE);
    }
    MApi_GFX_SetAlphaSrcFrom((GFX_AlphaSrcFrom)sdev->ge_render_alpha_from);
}

static int MStar_CheckHWBlit(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    if ((src->flags & GAL_HWSURFACE)
            && (src->format->Rmask == dst->format->Rmask)
            && (src->format->Gmask == dst->format->Gmask)
            && (src->format->Bmask == dst->format->Bmask)
            && (src->format->Amask == dst->format->Amask)) {
        src->flags |= GAL_HWACCEL;
        src->map->hw_blit = MStar_HWAccelBlit;
        return 1;
    }
    return 0;
}

static int MStar_HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect)
{
    MSTARDeviceData sdev; 
    GFX_DrawRect rectInfo;
    GFX_BufferInfo srcBuf, dstBuf;
    GFX_Point v0, v1;

    v0.x = 0, v0.y = 0, v1.x = dst->w-1, v1.y = dst->h-1;

    dstBuf.u32Addr = mstarGFXAddr(((vidmem_bucket*)(dst->hwdata))->fbid) ;
    dstBuf.u32ColorFmt = GALFmtToMStarFmt(dst->format);
    dstBuf.u32Width = dst->w ;
    dstBuf.u32Height = dst->h ;
    dstBuf.u32Pitch = dst->pitch ;

    srcBuf.u32Addr = mstarGFXAddr(((vidmem_bucket*)(src->hwdata))->fbid);
    srcBuf.u32ColorFmt = GALFmtToMStarFmt(src->format) ;
    srcBuf.u32Width = src->w ;
    srcBuf.u32Height = src->h ;
    srcBuf.u32Pitch = src->pitch ;
    rectInfo.srcblk.x = srcrect->x, rectInfo.srcblk.y = srcrect->y;
    rectInfo.srcblk.width = srcrect->w, rectInfo.srcblk.height = srcrect->h;
    rectInfo.dstblk.x = dstrect->x;
    rectInfo.dstblk.y = dstrect->y;
    rectInfo.dstblk.width = srcrect->w, rectInfo.dstblk.height = srcrect->h;

    CheckAlphaBlendType(&sdev,src,dst);
    MApi_GFX_BeginDraw();
    mstarSetEngineState(MSTAR_BIT_BLIT, &sdev);
    MApi_GFX_SetClip(&v0, &v1);
    MApi_GFX_SetDstBufferInfo(&dstBuf, 0);
    MApi_GFX_SetSrcBufferInfo(&srcBuf, 0);
    if(src == dst) {
        MApi_GFX_BitBlt(&rectInfo, GFXDRAW_FLAG_DUPLICAPE);
    } else { 
        MApi_GFX_BitBlt(&rectInfo, GFXDRAW_FLAG_DEFAULT);
    }
    MApi_GFX_SetTAGID(MApi_GFX_GetNextTAGID(TRUE));
    MApi_GFX_EndDraw();

    MApi_GFX_WaitForTAGID(MApi_GFX_GetNextTAGID(FALSE));
    return TRUE ;
}

void cleanEngineState(MSTARDeviceData *sdev)
{
    sdev->ge_render_coef = 0xFFFFFFFF;
    sdev->ge_render_alpha_from = (unsigned int)ABL_FROM_ASRC;
    sdev->ge_src_colorkey_enabled  = 0;
    sdev->ge_dst_colorkey_enabled = 0;
    sdev->ge_alpha_blend_enabled = 0;
    sdev->dst_ge_format = GFX_FMT_ARGB8888;
}

static int MStar_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    GFX_RectFillInfo rectInfo;
    GFX_Point v0, v1;
    GFX_BufferInfo buf ;
    GAL_PixelFormat  *dfmt = dst->format;
    MSTARDeviceData sdev;

    if(!(dst->flags & GAL_HWSURFACE))
        return -1;

    buf.u32ColorFmt = GALFmtToMStarFmt(dfmt);
    buf.u32Addr = mstarGFXAddr(((vidmem_bucket*)(dst->hwdata))->fbid);
    buf.u32Width = dst->w ;
    buf.u32Height = dst->h ;
    buf.u32Pitch = dst->pitch ;

    rectInfo.dstBlock.x = rect->x ;
    rectInfo.dstBlock.y = rect->y ;
    rectInfo.dstBlock.width = rect->w ;
    rectInfo.dstBlock.height = rect->h ;

    rectInfo.fmt = buf.u32ColorFmt;
    if(dfmt->Amask != 0xFF)
        rectInfo.colorRange.color_s.a = (color & dfmt->Amask) >> (dfmt->Ashift - dfmt->Aloss);
    else
        rectInfo.colorRange.color_s.a = dfmt->alpha;

    rectInfo.colorRange.color_s.r = (color & dfmt->Rmask) >> (dfmt->Rshift - dfmt->Rloss);
    rectInfo.colorRange.color_s.g = (color & dfmt->Gmask) >> (dfmt->Gshift - dfmt->Gloss);
    rectInfo.colorRange.color_s.b = (color & dfmt->Bmask) >> (dfmt->Bshift - dfmt->Bloss);	

    cleanEngineState(&sdev);
    rectInfo.flag = 0 ;

    v0.x = 0, v0.y = 0, v1.x = buf.u32Width-1, v1.y = buf.u32Height-1;
    MApi_GFX_BeginDraw();
    mstarSetEngineState(MSTAR_FILL_RECT, &sdev);
    MApi_GFX_SetClip(&v0, &v1);
    MApi_GFX_SetDstBufferInfo(&buf, 0) ;
    MApi_GFX_RectFill( &rectInfo ) ;
    MApi_GFX_SetTAGID(MApi_GFX_GetNextTAGID(TRUE));
    MApi_GFX_EndDraw();

    MApi_GFX_WaitForTAGID(MApi_GFX_GetNextTAGID(FALSE));
    return 0;
}

static int MStar_SetHWColorKey(_THIS, GAL_Surface *src, Uint32 key)
{
    FUNC_ENTER();
    src->format->colorkey = key;
    src->flags |= GAL_SRCCOLORKEY;
    FUNC_LEAVE();
    return 0;
}

static int MStar_SetHWAlpha(_THIS, GAL_Surface *surface, Uint8 alpha)
{
    FUNC_ENTER();
    surface->format->alpha = alpha;
    surface->flags |= GAL_SRCALPHA;
    FUNC_LEAVE();
    return 0;
}

static void MStar_DirectUpdate(_THIS, int numrects, GAL_Rect *rects)
{
    MApi_GFX_WaitForTAGID(MApi_GFX_GetNextTAGID(FALSE));
}

static int MStar_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size)
{
    FUNC_ENTER();
    vidmem_bucket *bucket;

    surfaces_memtotal = size;
    surfaces_memleft = size;

    if ( surfaces_memleft > 0 ) {
        bucket = (vidmem_bucket *)malloc(sizeof(*bucket));
        if ( bucket == NULL ) {
            GAL_OutOfMemory();
            return(-1);
        }
        bucket->prev = &surfaces;
        bucket->used = 0;
        bucket->dirty = 0;
        bucket->fbid = HIDDEN->dfb_base;
        bucket->fbaway = 0;
        bucket->base = base;
        bucket->size = size;
        bucket->next = NULL;
    } else {
        bucket = NULL;
    }
    GLH_DEBUG("\n  MStar_InitHWSurfaces  memleft=%d\n",surfaces_memleft );

    surfaces.prev = NULL;
    surfaces.used = 1;
    surfaces.dirty = 0;
    surfaces.base = screen->pixels;
    surfaces.fbid = HIDDEN->dfb_base + size;
    surfaces.fbaway = switched_away;

    surfaces.size = 0;
    surfaces.next = bucket;
    surfaces.bscreen = 1;
    screen->hwdata = (struct private_hwdata *)&(surfaces);
    FUNC_LEAVE();
    return(0);
}

static int MStar_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    FUNC_ENTER();
    vidmem_bucket *bucket;
    int size;
    int extra;
    if (surface->w < 8 || surface->h < 8 )
        return -1;

    /* Temporarily, we only allow surfaces the same width as display.
       Some blitters require the pitch between two hardware surfaces
       to be the same.  Others have interesting alignment restrictions.
       Until someone who knows these details looks at the code...  */ 
    size = ((surface->h * ((surface->pitch+15)/16)*16) + 255)/256 * 256;
    /* Quick check for available mem */
    if ( size > surfaces_memleft ) {
        fprintf(stderr, "Not enough video memory");
        return(-1);
    }

    /* Search for an empty bucket big enough */
    for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
        if ( ! bucket->used && (size <= bucket->size) ) {
            break;
        }
    }
    if ( bucket == NULL ) {
        fprintf(stderr, "Video memory too fragmented");
        return(-1);
    }

    /* Create a new bucket for left-over memory */
    extra = (bucket->size - size);
    if ( extra ) {
        vidmem_bucket *newbucket;

        newbucket = (vidmem_bucket *)malloc(sizeof(*newbucket));
        if ( newbucket == NULL ) {
            GAL_OutOfMemory();
            return(-1);
        }
        newbucket->prev = bucket;
        newbucket->used = 0;
        newbucket->base = bucket->base+size;
        newbucket->fbid = bucket->fbid + size;
        newbucket->fbaway = 0;
        newbucket->size = extra;
        newbucket->next = bucket->next;
        if ( bucket->next ) {
            bucket->next->prev = newbucket;
        }
        bucket->next = newbucket;
    }

    /* Set the current bucket values and return it! */
    bucket->used = 1;
    bucket->size = size;
    bucket->dirty = 0;

    surfaces_memleft -= size;
    surface->flags |= GAL_HWSURFACE;
    surface->pixels = bucket->base;
    surface->hwdata = (struct private_hwdata *)bucket;

    FUNC_LEAVE();
    return 0;
}



static void MStar_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    FUNC_ENTER();
    vidmem_bucket *bucket, *freeable;

    /* Look for the bucket in the current list */
    for (bucket=&surfaces; bucket; bucket=bucket->next ) {
        if ( bucket == (vidmem_bucket *)surface->hwdata ) {
            break;
        }
    }
    if ( bucket && bucket->used ) {
        /* Add the memory back to the total */
        surfaces_memleft += bucket->size;
        /* Can we merge the space with surrounding buckets? */
        bucket->used = 0;
        if ( bucket->next && ! bucket->next->used ) {
            freeable = bucket->next;
            bucket->size += bucket->next->size;
            bucket->next = bucket->next->next;
            if ( bucket->next ) {
                bucket->next->prev = bucket;
            }
            free(freeable);
        }
        if ( bucket->prev && ! bucket->prev->used ) {
            freeable = bucket;
            bucket->prev->size += bucket->size;
            bucket->prev->next = bucket->next;
            if ( bucket->next ) {
                bucket->next->prev = bucket->prev;
            }
            free(freeable);
        }
    }
    surface->pixels = NULL;
    surface->hwdata = NULL;
    FUNC_LEAVE();
}

void MStar_VideoQuit(_THIS)
{
    FUNC_ENTER();
    MApi_GOP_GWIN_Enable(HIDDEN->gwinid, FALSE) ;

    if(HIDDEN->flip == 1)
    {
        unsigned short u16QueueCnt = 1;
        MApi_GOP_Switch_GWIN_2_FB(HIDDEN->gwinid, HIDDEN->u16FBId[1],
                MApi_GFX_GetNextTAGID(FALSE), &u16QueueCnt);
    }
    MApi_GOP_GWIN_DestroyWin(HIDDEN->gwinid);	
    MApi_GOP_GWIN_DestroyFB(HIDDEN->u16FBId[0]);

    if(HIDDEN->bdoublebuf)
        MApi_GOP_GWIN_DestroyFB(HIDDEN->u16FBId[1]);

    munmap(HIDDEN->video_phys,HIDDEN->video_length);	
    free(GAL_modearray[0]);
    HIDDEN->initialized = 0;
    FUNC_LEAVE();
}
#endif

