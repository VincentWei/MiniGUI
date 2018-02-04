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

#ifdef _MGGAL_EM86GFX

#include "em86gfx.h"

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

VideoBootStrap EM86GFX_bootstrap = {
    "em86gfx", "EM86 GFX Video Driver",
    GFX_Available, GFX_CreateDevice
};

static int GFX_VideoInit(_THIS, GAL_PixelFormat * vformat)
{
	RMstatus err;

	err = RUACreateInstance(&pRUA, 0);
	if (RMFAILED(err)) {
		fprintf(stderr, "NEWGAL>EM86GFX: Error creating RUA instance (%d!)\n", err);
		return -1;
	}

	err = DCCOpen(pRUA, &pDCC);
	if (RMFAILED(err)) {
		fprintf(stderr, "NEWGAL>EM86GFX: Error Opening DCC (%d)!\n", err);
		return err;
	}

	err = DCCInitChainEx(pDCC, 0);
	if (RMFAILED(err)) {
		fprintf(stderr, "NEWGAL>EM86GFX: Cannot initialize microcode (%d)!\n", err);
		return err;
	}

    err = DCCSetStandard(pDCC, DCCRoute_Main, EMhwlibTVStandard_NTSC_M);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot open display (%d)!\n", err));
        return err;
    }

    DCCEnableVideoConnector(pDCC, DCCRoute_Main, DCCVideoConnector_SVIDEO, TRUE);

    return(0);
}

static GAL_Rect ** GFX_ListModes(_THIS, GAL_PixelFormat * format, Uint32 flags)
{
    return (GAL_Rect **)-1;
}

static GAL_Surface * GFX_SetVideoMode(_THIS, GAL_Surface * surface, int width, int height, int bpp, Uint32 flags)
{
    RMstatus err;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    err = DCCGetScalerModuleID(pDCC, DCCRoute_Main, DCCSurface_OSD, 0, &OSD_scaler);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot get surface to display OSD source %d\n", err));
        return NULL;
    }

    DCC_profile.ColorSpace = EMhwlibColorSpace_RGB_0_255;
    DCC_profile.SamplingMode = EMhwlibSamplingMode_444;
    DCC_profile.PixelAspectRatio.X = 1;
    DCC_profile.PixelAspectRatio.Y = 1;
    DCC_profile.Width = width;
    DCC_profile.Height = height;

    switch (bpp) {
    case 16:
        DCC_profile.ColorMode = EMhwlibColorMode_TrueColor;
        DCC_profile.ColorFormat = EMhwlibColorFormat_16BPP_565;
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
        DCC_profile.ColorMode = EMhwlibColorMode_TrueColor;
        DCC_profile.ColorFormat = EMhwlibColorFormat_32BPP;
    break;
    }

    err = DCCOpenOSDVideoSource(pDCC, &DCC_profile, &pOSD_source);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot open OSD decoder (%d)!\n", err);
        return NULL;
    }

    err = DCCGetOSDVideoSourceInfo(pOSD_source, &LumaAddr, &LumaSize, &ChromaAddr, &ChromaSize);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot get osd buffer info (%d)!\n", err);
        return NULL;
    }

    err = DCCSetSurfaceSource(pDCC, OSD_scaler, pOSD_source);
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot set the surface source (%d)!\n", err);
        return NULL;
    }

    surface->flags = (GAL_FULLSCREEN | GAL_HWSURFACE);
    surface->w = width;
    surface->h = height;
    surface->pitch = width * bpp / 8;
    surface->pixels = (char *)LumaAddr;
    if (!GAL_ReallocFormat(surface, bpp, Rmask, Gmask, Bmask, Amask)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Reallocating format error!\n");
        return NULL;
    }

    if (!init_GFXengine(this)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot init GFX engine!\n");
        return NULL;
    }

    return surface;
}

static void GFX_VideoQuit (_THIS)
{
    RMstatus err;
    struct EMhwlibDisplayWindow source_window;
    RMuint32 close_profile;

    err = RUASetProperty(pRUA,
            GFXtarget,
            RMGFXEnginePropertyID_Close,
            &close_profile, sizeof(close_profile), 0);

    if (RMFAILED(err)) 
        fprintf(stderr, "NEWGAL>EM86GFX: Cannot close gfx accelerator\n");

    if (GFX_profile.CachedAddress)
        RUAFree(pRUA, GFX_profile.CachedAddress);

    if (GFX_profile.UncachedAddress)
        RUAFree(pRUA, GFX_profile.UncachedAddress);

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

    while ((err = RUASetProperty(pRUA, OSD_scaler,
                    RMGenericPropertyID_ScalerInputWindow,
                    &source_window, sizeof(source_window), 0)) == RM_PENDING);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot set scaler input window on OSD surface (%d)!\n", err));
        return;
    }
 
    while ((err = RUASetProperty(pRUA, OSD_scaler, RMGenericPropertyID_Validate, NULL, 0, 0)) == RM_PENDING);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot validate scaler input window %d\n", err));
        return;
    }

    err = DCCCloseVideoSource(pOSD_source);
    if (RMFAILED(err)) {
        RMDBGLOG((ENABLE, "Cannot close osd source %s\n", RMstatusToString(err)));
        return;
    }

    return;
}

static int GFX_FillHWRect(_THIS, GAL_Surface * surface, GAL_Rect * rect, Uint32 color)
{
    struct GFXEngine_FillRectangle_type fill_param;
    struct GFXEngine_Surface_type surface_param;
    struct GFXEngine_ColorFormat_type format_param;

    format_param.SurfaceID = GFX_SURFACE_ID_NX;
    format_param.MainMode = DCC_profile.ColorMode;
    format_param.SubMode = DCC_profile.ColorFormat;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_ColorFormat, &format_param, sizeof(format_param));
    surface_param.SurfaceID = GFX_SURFACE_ID_NX;
    surface_param.StartAddress = (RMuint32)surface->pixels;
    surface_param.TotalWidth = (RMuint32)surface->w;
    surface_param.Tiled = FALSE;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_Surface, &surface_param, sizeof(surface_param));

    fill_param.X = rect->x;
    fill_param.Y = rect->y;
    fill_param.Width = rect->w;
    fill_param.Height = rect->h;
    fill_param.Color = color;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_FillRectangle, &fill_param, sizeof(fill_param));

	return 0;
}

static int init_GFXengine(_THIS)
{
    RMuint32 err, gfx_count, i;
    struct GFXEngine_DRAMSize_in_type  dramSizeIn;
    struct GFXEngine_DRAMSize_out_type dramSizeOut;

    dramSizeIn.CommandFIFOCount = 10;
    err = RUAExchangeProperty(pRUA,
            EMHWLIB_MODULE(GFXEngine, 0),
            RMGFXEnginePropertyID_DRAMSize,
            &dramSizeIn, sizeof(dramSizeIn),
            &dramSizeOut, sizeof(dramSizeOut));
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error getting dram size for gfx engine\n");
        return 0;
    }

    GFX_profile.CommandFIFOCount = dramSizeIn.CommandFIFOCount;
    GFX_profile.Priority = 1;
    GFX_profile.CachedSize = dramSizeOut.CachedSize;
    GFX_profile.UncachedSize = dramSizeOut.UncachedSize;

    if (GFX_profile.CachedSize > 0) {
        fprintf (stderr, "NEWGAL>EM86GFX: about to malloc cached, size %ld\n", 
                        GFX_profile.CachedSize);
        GFX_profile.CachedAddress = RUAMalloc(pRUA, 0, RUA_DRAM_CACHED, 
                        GFX_profile.CachedSize);
    }
    else
        GFX_profile.CachedAddress = 0;

    GFX_profile.UncachedSize = dramSizeOut.UncachedSize;
    if (GFX_profile.UncachedSize > 0)
        GFX_profile.UncachedAddress = RUAMalloc(pRUA, 0, RUA_DRAM_UNCACHED, 
                        GFX_profile.UncachedSize);
    else
        GFX_profile.UncachedAddress = 0;

    GFXtarget = GFXEngine;
    err = RUAExchangeProperty(pRUA,
            EMHWLIB_MODULE(Enumerator,0),
            RMEnumeratorPropertyID_CategoryIDToNumberOfInstances,
            &GFXtarget, sizeof(GFXtarget),
            &gfx_count, sizeof(gfx_count));
    if (RMFAILED(err)) {
        fprintf(stderr, "NEWGAL>EM86GFX: Error getting gfx engine count\n");
        return 0;
    }

    for (i = 0; i < gfx_count; i++) {
        GFXtarget = EMHWLIB_MODULE(GFXEngine, i);
        RMDBGLOG((ENABLE, "Trying on target %ld\n", i));
        err = RUASetProperty(pRUA, GFXtarget, RMGFXEnginePropertyID_Open, &GFX_profile, sizeof(GFX_profile), 0);
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

    return 1;
}

static int GFX_AllocHWSurface(_THIS, GAL_Surface * surface)
{
    surface->pixels = (BYTE *)malloc(surface->pitch * surface->h);
    surface->hwdata = NULL;
    surface->flags |= GAL_HWSURFACE;

    return 0;
}

static void GFX_FreeHWSurface(_THIS, GAL_Surface * surface)
{
    RMuint32 err, close_profile = 0;

    err = RUASetProperty(pRUA,
            GFXtarget,
            RMGFXEnginePropertyID_Close,
            &close_profile, sizeof(close_profile), 0);
    if (RMFAILED(err)) {
        fprintf (stderr, "NEWGAL>EM86GFX: Cannot close the gfx accelerator\n");
    }

    if (GFX_profile.CachedAddress)
        RUAFree(pRUA, GFX_profile.CachedAddress);

    if (GFX_profile.UncachedAddress)
        RUAFree(pRUA, GFX_profile.UncachedAddress);

    free(surface->pixels);
    free(surface->hwdata);
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

static int GFX_HWAccelBlit (GAL_Surface* src, GAL_Rect* srec, 
                GAL_Surface* dst, GAL_Rect* drec)
{
	GAL_VideoDevice * this = current_video;

    struct GFXEngine_BlendAndScaleRectangles_type blend_param;
    struct GFXEngine_Surface_type surface_param;
    struct GFXEngine_ColorFormat_type format_param;

    format_param.SurfaceID = GFX_SURFACE_ID_Z;
    format_param.MainMode = DCC_profile.ColorMode;
    format_param.SubMode = DCC_profile.ColorFormat;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_ColorFormat, 
                    &format_param, sizeof(format_param));
    surface_param.SurfaceID = GFX_SURFACE_ID_Z;
    surface_param.StartAddress = (RMuint32)src->pixels;
    surface_param.TotalWidth = (RMuint32)src->w;
    surface_param.Tiled = FALSE;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_Surface, 
                    &surface_param, sizeof(surface_param));

    format_param.SurfaceID = GFX_SURFACE_ID_NX;
    format_param.MainMode = DCC_profile.ColorMode;
    format_param.SubMode = DCC_profile.ColorFormat;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_ColorFormat, 
                    &format_param, sizeof(format_param));
    surface_param.SurfaceID = GFX_SURFACE_ID_NX;
    surface_param.StartAddress = (RMuint32)dst->pixels;
    surface_param.TotalWidth = (RMuint32)dst->w;
    surface_param.Tiled = FALSE;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_Surface, 
                    &surface_param, sizeof(surface_param));

    format_param.SurfaceID = GFX_SURFACE_ID_X;
    format_param.MainMode = DCC_profile.ColorMode;
    format_param.SubMode = DCC_profile.ColorFormat;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_ColorFormat, 
                    &format_param, sizeof(format_param));
    surface_param.SurfaceID = GFX_SURFACE_ID_X;
    surface_param.StartAddress = (RMuint32)dst->pixels;
    surface_param.TotalWidth = (RMuint32)dst->w;
    surface_param.Tiled = FALSE;
    SEND_GFX_COMMAND(pRUA, GFXtarget, RMGFXEnginePropertyID_Surface, 
                    &surface_param, sizeof(surface_param));

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

    SEND_GFX_COMMAND(pRUA,
            GFXtarget,
            RMGFXEnginePropertyID_BlendAndScaleRectangles,
            &blend_param, sizeof(blend_param));

    return 0;
}

#endif /* _MGGAL_EM86GFX */

