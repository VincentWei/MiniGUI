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
**  $Id: stgfb_video.c 13621 2010-11-10 08:14:16Z humingming $
**  
**  Copyright (C) 2003 ~ 2010 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming
**
**  All rights reserved by Feynman Software.
**
**  This is the NEWGAL STGFB engine, based ST native ioctl.
**
*/

#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

/* MiniGUI header files */
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_STGFB

/* ST special header files */
#include <stgfb.h>
#include <stgxobj.h>
#include <stblit.h>
#include <stlayer.h>
#include <stblit_ioctl.h>
#include <stlayer_ioctl.h>

#include "stgfb_video.h"


//#define STGFB_DEBUG 

#define STGFBVID_DRIVER_NAME "stgfb"

/* set TRUE to enable stblit complete notify, not use yet.
 * we can just use STGFB_IO_SYNC_BLITTER ioctl to sync instead of register blit event */
#define GLOBAL_NBC TRUE


#ifdef _MGRM_PROCESSES
#   include "client.h"
#   define ISSERVER (mgIsServer)
#else
#   define ISSERVER (1)
#endif


static int framebuffer_fd = -1;


/* Initialization/Query functions */
static int STGFB_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **STGFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *STGFB_SetVideoMode (_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int STGFB_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void STGFB_VideoQuit (_THIS);
static void STGFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects);

/* Hardware surface functions */
static void STGFB_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
static int STGFB_AllocHWSurface (_THIS, GAL_Surface *surface);
static void STGFB_FreeHWSurface (_THIS, GAL_Surface *surface);
static int STGFB_HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect);
static int STGFB_CheckHWBlit (_THIS, GAL_Surface * src, GAL_Surface * dst);
static int STGFB_FillHWRect (_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color);

/* STAPI functions */
static int STAPI_Init (_THIS);
static int STAPI_Term (_THIS);
static int STAPI_Colorkey2ST (Uint32 colorkey, STGXOBJ_ColorType_t color_type, 
        STGXOBJ_ColorKeyType_t colorkey_type, BOOL bOut, STGXOBJ_ColorKey_t *st_colorkey);
static int STAPI_FillRectangle (_THIS, int fd, GAL_Surface* dst, GAL_Rect* rect, 
        GAL_Color* color, BOOL wait_sync);
static int STAPI_Blit (_THIS, int fd, GAL_Surface* src, GAL_Rect* srcrect, 
        GAL_Surface* dst, GAL_Rect* dstrect, BOOL wait_sync);
#if 0
static BOOL STAPI_DoesIntersect (const GAL_Rect* psrc1, const GAL_Rect* psrc2);
static unsigned int STAPI_CalcPitch(int w, int bpp);
static int STAPI_BlitWrapper (_THIS, int fd, GAL_Surface* src, GAL_Rect* srcrect, 
        GAL_Surface* dst, GAL_Rect* dstrect, BOOL wait_sync);
#endif


#ifdef STGFB_DEBUG
    #define debug_print printf

static void print_vinfo (struct fb_var_screeninfo *fb_vinfo)
{
    fprintf(stderr, "\nPrinting vinfo:\n");
    fprintf(stderr, "txres: %d\n", fb_vinfo->xres);
    fprintf(stderr, "tyres: %d\n", fb_vinfo->yres);
    fprintf(stderr, "txres_virtual: %d\n", fb_vinfo->xres_virtual);
    fprintf(stderr, "tyres_virtual: %d\n", fb_vinfo->yres_virtual);
    fprintf(stderr, "txoffset: %d\n", fb_vinfo->xoffset);
    fprintf(stderr, "tyoffset: %d\n", fb_vinfo->yoffset);
    fprintf(stderr, "tbits_per_pixel: %d\n", fb_vinfo->bits_per_pixel);
    fprintf(stderr, "tgrayscale: %d\n", fb_vinfo->grayscale);
    fprintf(stderr, "tnonstd: %d\n", fb_vinfo->nonstd);
    fprintf(stderr, "tactivate: %d\n", fb_vinfo->activate);
    fprintf(stderr, "theight: %d\n", fb_vinfo->height);
    fprintf(stderr, "twidth: %d\n", fb_vinfo->width);
    fprintf(stderr, "taccel_flags: %d\n", fb_vinfo->accel_flags);
    fprintf(stderr, "tpixclock: %d\n", fb_vinfo->pixclock);
    fprintf(stderr, "tleft_margin: %d\n", fb_vinfo->left_margin);
    fprintf(stderr, "tright_margin: %d\n", fb_vinfo->right_margin);
    fprintf(stderr, "tupper_margin: %d\n", fb_vinfo->upper_margin);
    fprintf(stderr, "tlower_margin: %d\n", fb_vinfo->lower_margin);
    fprintf(stderr, "thsync_len: %d\n", fb_vinfo->hsync_len);
    fprintf(stderr, "tvsync_len: %d\n", fb_vinfo->vsync_len);
    fprintf(stderr, "tsync: %d\n", fb_vinfo->sync);
    fprintf(stderr, "tvmode: %d\n", fb_vinfo->vmode);
    fprintf(stderr, "tred: %d/%d\n", fb_vinfo->red.length, fb_vinfo->red.offset);
    fprintf(stderr, "tgreen: %d/%d\n", fb_vinfo->green.length, fb_vinfo->green.offset);
    fprintf(stderr, "tblue: %d/%d\n", fb_vinfo->blue.length, fb_vinfo->blue.offset);
    fprintf(stderr, "talpha: %d/%d\n", fb_vinfo->transp.length, fb_vinfo->transp.offset);
}

static void print_finfo(struct fb_fix_screeninfo *fb_finfo)
{
    fprintf(stderr, "\nPrinting finfo:\n");
    fprintf(stderr, "tsmem_start = %p\n", (char *)fb_finfo->smem_start);
    fprintf(stderr, "tsmem_len = %d\n", fb_finfo->smem_len);
    fprintf(stderr, "ttype = %d\n", fb_finfo->type);
    fprintf(stderr, "ttype_aux = %d\n", fb_finfo->type_aux);
    fprintf(stderr, "tvisual = %d\n", fb_finfo->visual);
    fprintf(stderr, "txpanstep = %d\n", fb_finfo->xpanstep);
    fprintf(stderr, "typanstep = %d\n", fb_finfo->ypanstep);
    fprintf(stderr, "tywrapstep = %d\n", fb_finfo->ywrapstep);
    fprintf(stderr, "tline_length = %d\n", fb_finfo->line_length);
    fprintf(stderr, "tmmio_start = %p\n", (char *)fb_finfo->mmio_start);
    fprintf(stderr, "tmmio_len = %d\n", fb_finfo->mmio_len);
    fprintf(stderr, "taccel = %d\n", fb_finfo->accel);
}

#else
    #define debug_print
    #define print_vinfo(fb_vinfo)
    #define print_finfo(fb_finfo)
#endif


/* ========================================================================
   STAPI functions
   ======================================================================== */
static int STAPI_Init (_THIS)
{
    int enable = 1;
    char *devpath = NULL;

    STLAYER_Ioctl_GetViewPortParams_t   STLAYER_Ioctl_GetViewPortParams;

    /* check whether framebuffer is opened */
    if(console_fd < 0) {
        perror(strerror(errno));
        fprintf(stderr, "STAPI_Init >> before call STAPI_Init must open stgfb device!\n"); 
        goto stapi_init_err;
    }
    
    /* open stlayer device */
    devpath = getenv("STLAYER_IOCTL_DEV_PATH");  
    if (devpath == NULL )
        devpath = "/dev/stapi/stlayer_ioctl";

    stlayer_fd = open(devpath, O_RDWR); 
    if(stlayer_fd < 0) {
        perror(strerror(errno));
        fprintf(stderr, "STAPI_Init >> open layer device failed!\n"); 
        goto stapi_init_err;
    }

    /* get primary layer handle */
    if (ioctl(console_fd, STGFB_IO_GET_VIEWPORT_HANDLE, &viewport_handle) != 0) {
        perror(strerror(errno));
        fprintf(stderr, "STAPI_Init >> Unable to get layer handle !\n");
        goto stapi_init_err;
    }

    debug_print("STAPi_Init >> get layer handle ok =[%x]\n", viewport_handle);
    
    /* enable primary layer */
    if (ioctl(console_fd, STGFB_IO_ENABLE_LAYER, &enable) != 0) {
        perror(strerror(errno));
        fprintf(stderr, "STPI_Init >> Unable to enable layer !\n");
        goto stapi_init_err;
    }
   
	/* get the layer informations */
    memset(&STLAYER_Ioctl_GetViewPortParams, 0, sizeof(STLAYER_Ioctl_GetViewPortParams_t));
	STLAYER_Ioctl_GetViewPortParams.VPHandle = viewport_handle;

	if (ioctl(stlayer_fd, STLAYER_IOC_GETVIEWPORTPARAMS, &STLAYER_Ioctl_GetViewPortParams) != 0) {
        perror(strerror(errno));
		fprintf(stderr, "STAPI_Init >> Unable to get the layer parameters!\n");
		goto stapi_init_err;
	}

	/* get ViewPort params */
    screen_bmp = STLAYER_Ioctl_GetViewPortParams.FlatParams.Source.Data.BitMap;
    screen_bmp.Height = vinfo.yres;

    debug_print("STAPI_Init >> colorType:%d\n\
            bitmapType:%d\n\
            w:%d, h:%d, pitch:%d, offset:%d, size1:%d\n\
            data1_p:%p, data1_Cp:%p, data1_NCp:%p\n\
            data2_p:%p, data2_Cp:%p, data2_NCp:%p\n\n", 
            screen_bmp.ColorType,
            screen_bmp.BitmapType,
            screen_bmp.Width, screen_bmp.Height, screen_bmp.Pitch, 
            screen_bmp.Offset, screen_bmp.Size1,
            (char*)screen_bmp.Data1_p, (char*)screen_bmp.Data1_Cp, (char*)screen_bmp.Data1_NCp,
            (char*)screen_bmp.Data2_p, (char*)screen_bmp.Data2_Cp, (char*)screen_bmp.Data2_NCp);

    return 0;
    
stapi_init_err:
    if (stlayer_fd > 0) 
        close(stlayer_fd);
    return -1;
}
    

static int STAPI_Term (_THIS)
{
    if (stlayer_fd > 0) 
        close(stlayer_fd);

    stlayer_fd = -1;

    return 0;
}


#if 0
BOOL STAPI_DoesIntersect (const GAL_Rect* psrc1, const GAL_Rect* psrc2)
{
    int left, top, right, bottom;
    
    left = (psrc1->x > psrc2->x) ? psrc1->x : psrc2->x;
    top  = (psrc1->y > psrc2->y) ? psrc1->y : psrc2->y;
    right = ((psrc1->x + psrc1->w) < (psrc2->x + psrc2->w)) 
        ? (psrc1->x + psrc1->w) : (psrc2->x + psrc2->w);
    bottom = ((psrc1->y + psrc1->h) < (psrc2->y + psrc2->h)) 
        ? (psrc1->y + psrc1->h) : (psrc2->y + psrc2->h);

    if(left >= right || top >= bottom)
        return FALSE;

    return TRUE;
}

static unsigned int STAPI_CalcPitch (int w, int bbp)
{
    unsigned int pitch;

    /* Box should be 4-byte aligned for speed */
    pitch = w * bbp;
    switch (bbp) {
        case 1:
            pitch = (pitch+7)/8;
            break;
        case 4:
            pitch = (pitch+1)/2;
            break;
        default:
            break;
    }

    /* 4-byte aligning */
    pitch = (pitch + 3) & ~3;    
    return(pitch);
}
#endif


static int STAPI_Colorkey2ST (Uint32 colorkey, STGXOBJ_ColorType_t color_type, 
        STGXOBJ_ColorKeyType_t colorkey_type, BOOL bOut, STGXOBJ_ColorKey_t *st_colorkey)
{
	Uint8 Rmin, Rmax, Gmin, Gmax, Bmin, Bmax;

	switch (color_type)
	{
		/* NOTE: we're only concerned about getting the right value (pDev->colourKey)
		       into the register, so unstuffing then restuffing the color components
		       should always work with an RGB888 type. */
  		case STGXOBJ_COLOR_TYPE_RGB888:
		case STGXOBJ_COLOR_TYPE_ARGB8888:
		case STGXOBJ_COLOR_TYPE_ARGB8888_255:
			Rmin = Rmax = (colorkey >> 16) & 0xFF;
			Gmin = Gmax = (colorkey >> 8) & 0xFF;
			Bmin = Bmax = (colorkey >> 0) & 0xFF;
			break;

		case STGXOBJ_COLOR_TYPE_ARGB1555:
			Rmin = ((colorkey >> 10) & 0x1F) << 3;
			Rmax = Rmin + 0x7;
			Gmin = ((colorkey >> 5) & 0x1F) << 3;
			Gmax = Gmin + 0x7;
			Bmin = ((colorkey >> 0) & 0x1F) << 3;
			Bmax = Bmin + 0x7;
			break;

        case STGXOBJ_COLOR_TYPE_ARGB4444:
            Rmin = ((colorkey >> 8) & 0x1F) << 4;
			Rmax = Rmin + 0x7;
            Gmin = ((colorkey >> 4) & 0x1F) << 4;
			Gmax = Gmin + 0x7;
            Bmin = ((colorkey >> 0) & 0x1F) << 4;
			Bmax = Bmin + 0x7;
			break;

        case STGXOBJ_COLOR_TYPE_RGB565:
            Rmin = ((colorkey >> 11) & 0x1F) << 3;
            Rmax = Rmin + 0x7;
            Gmin = ((colorkey >> 5) & 0x3F) << 2;
            Gmax = Gmin + 0x3;
            Bmin = ((colorkey >> 0) & 0x1F) << 3;
            Bmax = Bmin + 0x7;
			break;

  		default:
  			fprintf(stderr, "Src ColorKey: colortype error -- source type is %d\n", color_type);
            return -1;
	}

	st_colorkey->Type = colorkey_type;

    switch (colorkey_type)
	{
		case STGXOBJ_COLOR_KEY_TYPE_RGB888:
	        st_colorkey->Type = STGXOBJ_COLOR_KEY_TYPE_RGB888;
	        st_colorkey->Value.RGB888.REnable = TRUE;
	        st_colorkey->Value.RGB888.GEnable = TRUE;
	        st_colorkey->Value.RGB888.BEnable = TRUE;
	        st_colorkey->Value.RGB888.ROut = bOut;
	        st_colorkey->Value.RGB888.GOut = bOut;
	        st_colorkey->Value.RGB888.BOut = bOut;
	        st_colorkey->Value.RGB888.RMin = Rmin;
	        st_colorkey->Value.RGB888.RMax = Rmax;
	        st_colorkey->Value.RGB888.GMin = Gmin;
	        st_colorkey->Value.RGB888.GMax = Gmax;
	        st_colorkey->Value.RGB888.BMin = Bmin;
	        st_colorkey->Value.RGB888.BMax = Bmax;
  			debug_print("Src ColorKey set to RGB888: colourKey=0x%x; Rmax=%d, Gmax=%d, Bmax=%d\n",
  				   colorkey,
                   st_colorkey->Value.RGB888.RMax,
                   st_colorkey->Value.RGB888.GMax,
                   st_colorkey->Value.RGB888.BMax );
  			debug_print("                                            Rmin=%d, Gmin=%d, Bmin=%d\n",
	                   st_colorkey->Value.RGB888.RMin,
	                   st_colorkey->Value.RGB888.GMin,
	                   st_colorkey->Value.RGB888.BMin );
		    break;

		default:
  			fprintf(stderr, "Src ColorKey: colorkey type error -- type is %d\n", colorkey_type);
  			return -1;
	}

	return 0;
}


static int STAPI_FillRectangle (_THIS, int fd, GAL_Surface* dst, GAL_Rect* dstrect, 
        GAL_Color* color, BOOL wait_sync)
{
    if (color == NULL) {
        fprintf(stderr, "STAPI_FillRect >> color NULL\n");
        return -1;
    }

    STGXOBJ_Color_t         stgxobj_color;
    STGXOBJ_Bitmap_t*       st_bitmap = &dst->hwdata->st_bitmap;
	STGFB_BLIT_Command_t    Command;

	memset(&Command, 0, sizeof(STGFB_BLIT_Command_t));
    memset(&stgxobj_color, 0, sizeof(STGXOBJ_Color_t));

    /* set colorkey */
    if (dst->flags & GAL_SRCCOLORKEY) {
        STAPI_Colorkey2ST(dst->format->colorkey, st_bitmap->ColorType, 
                STGXOBJ_COLOR_KEY_TYPE_RGB888, FALSE, &Command.Params.Blit.Context.ColorKey);
	    Command.Params.Blit.Context.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_DST;
    }
    else {
	    Command.Params.Blit.Context.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_NONE;
    }

    /* note: the alpha range is [0, 128] */
    if (dst->flags & GAL_SRCALPHA) {
        Command.Params.Blit.Context.AluMode = STBLIT_ALU_ALPHA_BLEND;
        if ( STGXOBJ_COLOR_TYPE_ARGB8888_255 == st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ARGB8565_255 == st_bitmap->ColorType
                || STGXOBJ_COLOR_TYPE_ACLUT88_255 == st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ALPHA8_255 == st_bitmap->ColorType
                )
            Command.Params.Blit.Context.GlobalAlpha = dst->format->alpha;
        else
            Command.Params.Blit.Context.GlobalAlpha = (dst->format->alpha+1)/2;
    }
    else {
        Command.Params.Blit.Context.AluMode = STBLIT_ALU_COPY;
        if ( STGXOBJ_COLOR_TYPE_ARGB8888_255 == st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ARGB8565_255 == st_bitmap->ColorType
                || STGXOBJ_COLOR_TYPE_ACLUT88_255 == st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ALPHA8_255 == st_bitmap->ColorType
                )
            Command.Params.Blit.Context.GlobalAlpha = 255;
        else
            Command.Params.Blit.Context.GlobalAlpha = 128;
    }

    /* set the fill color */
    stgxobj_color.Type = st_bitmap->ColorType;
    switch (stgxobj_color.Type) {
        case STGXOBJ_COLOR_TYPE_RGB565:
            stgxobj_color.Value.RGB565.R     = color->r;
            stgxobj_color.Value.RGB565.G     = color->g;
            stgxobj_color.Value.RGB565.B     = color->b;
            break;

        case STGXOBJ_COLOR_TYPE_ARGB1555:
            stgxobj_color.Value.ARGB1555.Alpha = color->a;
            stgxobj_color.Value.ARGB1555.R     = color->r;
            stgxobj_color.Value.ARGB1555.G     = color->g;
            stgxobj_color.Value.ARGB1555.B     = color->b;
            Command.Params.Blit.Context.AluMode = STBLIT_ALU_ALPHA_BLEND;
            break;

        case STGXOBJ_COLOR_TYPE_ARGB8888:
            stgxobj_color.Value.ARGB8888.Alpha = color->a;
            stgxobj_color.Value.ARGB8888.R     = color->r;
            stgxobj_color.Value.ARGB8888.G     = color->g;
            stgxobj_color.Value.ARGB8888.B     = color->b;
            Command.Params.Blit.Context.AluMode = STBLIT_ALU_ALPHA_BLEND;
            break;

        default:
		    fprintf(stderr, "STAPI_FillRect >>  color type: [%d] not supported !\n",
                    stgxobj_color.Type);
            return -1;
    }

    debug_print("STAPI_FillRect >>  type:%d, r:%d, g:%d, b:%d, a:%d, alu:%d, alpha:%d\n", 
            stgxobj_color.Type, color->r, color->g, color->b, color->a, 
            Command.Params.Blit.Context.AluMode, Command.Params.Blit.Context.GlobalAlpha);

    /* set blit operation to blit */
	Command.Operation = STGFB_BLIT_BLIT;
	Command.Params.Blit.Handle = 0;

    /* set blit src context */
    Command.Params.Blit.SrcType = STBLIT_SOURCE_TYPE_COLOR;
    Command.Params.Blit.SrcData_p = &stgxobj_color; 

    /* set blit dst context */
    memcpy(&Command.Params.Blit.DestBitmap, st_bitmap, sizeof(STGXOBJ_Bitmap_t));
    Command.Params.Blit.DestRectangle.PositionX = dstrect->x;
    Command.Params.Blit.DestRectangle.PositionY = dstrect->y;
    Command.Params.Blit.DestRectangle.Width = dstrect->w;
    Command.Params.Blit.DestRectangle.Height = dstrect->h;

    /* set blit context */
    Command.Params.Blit.Context.EnableMaskWord = FALSE;
    Command.Params.Blit.Context.EnableMaskBitmap = FALSE;
    Command.Params.Blit.Context.WorkBuffer_p = 0;
    Command.Params.Blit.Context.EnableColorCorrection = FALSE;
    Command.Params.Blit.Context.EnableClipRectangle = FALSE;
    Command.Params.Blit.Context.WriteInsideClipRectangle = FALSE;
    Command.Params.Blit.Context.EnableFlickerFilter = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
    Command.Params.Blit.Context.EnableResizeFilter	= FALSE;
#endif
    Command.Params.Blit.Context.JobHandle = STBLIT_NO_JOB_HANDLE;

    /* set blit event */
    Command.Params.Blit.Context.UserTag_p = NULL;
    Command.Params.Blit.Context.NotifyBlitCompletion = GLOBAL_NBC;  

    /* blit io command */
 	if (ioctl(fd, STGFB_IO_BLIT_COMMAND, &Command) != 0) {
        perror(strerror(errno));
        fprintf(stderr, "STAPI_FillRect >> ioctl: STGFB_IO_BLIT_COMMAND error\n");
		return -1;
	}

    /* sync the blitter, no need any params */
    if (wait_sync) {
        if (ioctl(fd, STGFB_IO_SYNC_BLITTER, 0) != 0) {
            perror(strerror(errno));
            fprintf(stderr, "STAPI_FillRect >> Sync Engine failed\n");
        }
    }

	return 0;
}


#if 0
static void STBlit_Callback (STEVT_CallReason_t Reason, const ST_DeviceName_t RegistrantName, 
        STEVT_EventConstant_t Event, const void *EventData, const void *SubscriberData_p)
{
    debug_print("STBli_Callback >> call ... ...\n");

    /* ignore all blitter callback not for the rack */
    if (SubscriberData_p != STBLIT_CALLBACK_MAGIC) 
        return;
    
    switch (Event) {
        case STBLIT_BLIT_COMPLETED_EVT:
            debug_print("STBli_Callback >> blit completed\n");
            break;

        case STBLIT_JOB_COMPLETED_EVT:
            debug_print("STBli_Callback >> job completed\n");
            break;

        default:
            debug_print("STBlit_Callback >> Undefined event received !\n");
            break;
     }

    return;
}


static int STAPI_SubscribeDeviceEvent (int fd, STEVT_Handle_t Handle, 
        const ST_DeviceName_t RegistrantName, STEVT_EventConstant_t EventConst, 
        const STEVT_DeviceSubscribeParams_t* SubscribeParams, STEVT_SubscriberID_t* SubscriberID)
{
    STEVT_Ioctl_GetSubscriberID_t       EVT_Ioctl_GetSubscribeIDParams;
    STEVT_Ioctl_SubscribeDeviceEvent_t  EVT_Ioctl_SubscribeDeviceParams;

    EVT_Ioctl_SubscribeDeviceParams.Handle = Handle;
    EVT_Ioctl_SubscribeDeviceParams.EventConst = EventConst;

    if (RegistrantName)
        strcpy(EVT_Ioctl_SubscribeDeviceParams.RegistrantName, RegistrantName); 
    else
        EVT_Ioctl_SubscribeDeviceParams.RegistrantName[0] = '\0'; 

    debug_print("STAPI_SubscribeDevieceEvent >> fd:%d, %s\n", fd,
            EVT_Ioctl_SubscribeDeviceParams.RegistrantName);

    /* set subscribe device event */
    if (SubscribeParams != NULL ) {
        if (SubscribeParams->NotifyCallback) {
            EVT_Ioctl_SubscribeDeviceParams.CBAck = CBAck; 
            EVT_Ioctl_SubscribeDeviceParams.SubscribeParams = *SubscribeParams;

            if (ioctl(fd, STEVT_IOC_SUBSCRIBEDEVICEEVENT, &EVT_Ioctl_SubscribeDeviceParams) == -1) {
                perror(strerror(errno));
                fprintf(stderr, "STAPI_SubscribeDeviceEvent >> subscribe device event failed\n");
                return -1;
            }
        }
        else {
            fprintf(stderr, "STAPI_SubscribeDeviceEvent >> callback functions NULL\n");
            return -1;
        }
    }
    else {
        fprintf(stderr, "STAPI_SubscribeDeviceEvent >> device params NULL\n");
        return -1;
    }

    debug_print("STAPI_SubscribeDevieceEvent >> done subscribe device event\n");

    /* get subscribe event ID */
    EVT_Ioctl_GetSubscribeIDParams.Handle = Handle;

    if (SubscriberID) {
        if (ioctl(fd, STEVT_IOC_GETSUBSCRIBERID, &EVT_Ioctl_GetSubscribeIDParams) == -1) {
            perror(strerror(errno));
            fprintf(stderr, "STAPI_SubscribeDeviceEvent >> get subscribe event ID failed\n");
            return -1;
        }
        *SubscriberID = EVT_Ioctl_GetSubscribeIDParams.SubscriberID;
    }

    debug_print("STAPI_SubscribeDevieceEvent >> done get ID\n");
    return 0;
}
#endif


static int STAPI_Blit (_THIS, int fd, GAL_Surface* src, GAL_Rect* srcrect, 
        GAL_Surface* dst, GAL_Rect* dstrect, BOOL wait_sync)
{

    STGXOBJ_Bitmap_t*               src_st_bitmap = &src->hwdata->st_bitmap;
    STGXOBJ_Bitmap_t*               dst_st_bitmap = &dst->hwdata->st_bitmap;
	STGFB_BLIT_Command_t            Command;

	memset(&Command, 0, sizeof(STGFB_BLIT_Command_t));

    debug_print("STAPI_Blit >> src:(%d,%d,%d,%d), dst:(%d,%d,%d,%d)\n", 
            srcrect->x, srcrect->y, srcrect->w, srcrect->h,
            dstrect->x, dstrect->y, dstrect->w, dstrect->h);

    /* set the colorkey */
    if (src->flags & GAL_SRCCOLORKEY) {
        STAPI_Colorkey2ST(src->format->colorkey, src_st_bitmap->ColorType, 
                STGXOBJ_COLOR_KEY_TYPE_RGB888, FALSE, &Command.Params.Blit.Context.ColorKey);
	    Command.Params.Blit.Context.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_SRC;
    }
    else {
	    Command.Params.Blit.Context.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_NONE;
    }

    /* note: the alpha range is [0, 128] */
    if (src->flags & GAL_SRCALPHA) {
        Command.Params.Blit.Context.AluMode = STBLIT_ALU_ALPHA_BLEND;
        if ( STGXOBJ_COLOR_TYPE_ARGB8888_255 == src_st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ARGB8565_255 == src_st_bitmap->ColorType
                || STGXOBJ_COLOR_TYPE_ACLUT88_255 == src_st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ALPHA8_255 == src_st_bitmap->ColorType
                )
            Command.Params.Blit.Context.GlobalAlpha = src->format->alpha;
        else
            Command.Params.Blit.Context.GlobalAlpha = (src->format->alpha+1)/2;
    }
    else {
        Command.Params.Blit.Context.AluMode = STBLIT_ALU_COPY;
        if ( STGXOBJ_COLOR_TYPE_ARGB8888_255 == src_st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ARGB8565_255 == src_st_bitmap->ColorType
                || STGXOBJ_COLOR_TYPE_ACLUT88_255 == src_st_bitmap->ColorType 
                || STGXOBJ_COLOR_TYPE_ALPHA8_255 == src_st_bitmap->ColorType
                )
            Command.Params.Blit.Context.GlobalAlpha = 255;
        else
            Command.Params.Blit.Context.GlobalAlpha = 128;
    }

    /* set blit operation to blit */
	Command.Operation = STGFB_BLIT_BLIT;
	Command.Params.Blit.Handle = 0;

    /* set blit src context */
    Command.Params.Blit.SrcType = STBLIT_SOURCE_TYPE_BITMAP;
    Command.Params.Blit.SrcData_p = src_st_bitmap; 
    Command.Params.Blit.SrcRectangle.PositionX = srcrect->x;
    Command.Params.Blit.SrcRectangle.PositionY = srcrect->y;
    Command.Params.Blit.SrcRectangle.Width = srcrect->w;
    Command.Params.Blit.SrcRectangle.Height = srcrect->h;

    /* set blit dst context */
    memcpy(&Command.Params.Blit.DestBitmap, dst_st_bitmap, sizeof(STGXOBJ_Bitmap_t));
    Command.Params.Blit.DestRectangle.PositionX = dstrect->x;
    Command.Params.Blit.DestRectangle.PositionY = dstrect->y;
    Command.Params.Blit.DestRectangle.Width = dstrect->w;
    Command.Params.Blit.DestRectangle.Height = dstrect->h;

    /* set blit context */
    Command.Params.Blit.Context.EnableMaskWord = FALSE;
    Command.Params.Blit.Context.EnableMaskBitmap = FALSE;
    Command.Params.Blit.Context.WorkBuffer_p = 0;
    Command.Params.Blit.Context.EnableColorCorrection = FALSE;
    Command.Params.Blit.Context.EnableClipRectangle = FALSE;
    Command.Params.Blit.Context.WriteInsideClipRectangle = FALSE;
    Command.Params.Blit.Context.EnableFlickerFilter = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
    Command.Params.Blit.Context.EnableResizeFilter	= FALSE;
#endif
    Command.Params.Blit.Context.JobHandle = STBLIT_NO_JOB_HANDLE;

    /* set blit event */
    Command.Params.Blit.Context.UserTag_p = NULL;
    Command.Params.Blit.Context.NotifyBlitCompletion = GLOBAL_NBC;  

    /* blit io command */
 	if (ioctl(fd, STGFB_IO_BLIT_COMMAND, &Command) != 0) {
        perror(strerror(errno));
        fprintf(stderr, "STAPI_Blit >> ioctl: STGFB_IO_BLIT_COMMAND error\n");
		return -1;
	}

    /* sync the blitter, no need any params */
    if (wait_sync) {
        if (ioctl(fd, STGFB_IO_SYNC_BLITTER, 0) != 0) {
            perror(strerror(errno));
            fprintf(stderr, "STAPI_Blit >> Sync Engine failed\n");
        }
    }

	return 0;
}


#if 0
static int STAPI_BlitWrapper (_THIS, int fd, GAL_Surface* src, GAL_Rect* srcrect, 
        GAL_Surface* dst, GAL_Rect* dstrect, BOOL wait_sync)
{
    /* the src is the same of the dst, and srcrect is overlap with dstrect.
     * is can't be blit directly, because it's will broken the original pixels. */
    if ((src->hwdata->st_bitmap.Data1_p == dst->hwdata->st_bitmap.Data1_p)
            && (STAPI_DoesIntersect(srcrect, dstrect) == TRUE)) {

        GAL_Rect tmp_rc;
        GAL_Surface tmp;
        
        memcpy(&tmp, src, sizeof(GAL_Surface));
        tmp.w = srcrect->w;
        tmp.h = srcrect->h;
        tmp.pitch = STAPI_CalcPitch(tmp.w, tmp.format->BytesPerPixel);

        /* don't use any colorkey or alpha blend.
         * we just need original pixels data. */
        tmp.flags &= ~GAL_SRCCOLORKEY;
        tmp.flags &= ~GAL_SRCALPHA;

        if (STGFB_AllocHWSurface(this, &tmp) < 0)
            goto blitwrapper_err;
        
        tmp_rc.x = 0;
        tmp_rc.y = 0;
        tmp_rc.w = tmp.hwdata->st_bitmap.Width;
        tmp_rc.h = tmp.hwdata->st_bitmap.Height;

        if (STAPI_Blit(this, fd, src, srcrect, &tmp, &tmp_rc, wait_sync) == -1)
            goto blitwrapper_err;

        /* restore the src_surface colorkey and alpha flags */
        tmp.flags = src->flags;

        if (STAPI_Blit(this, fd, &tmp, &tmp_rc, dst, dstrect, wait_sync) == -1)
            goto blitwrapper_err;

        STGFB_FreeHWSurface(this, &tmp);
        return 0;

blitwrapper_err:
        fprintf(stderr, "STAPI_BlitWraaper >> error\n");
        STGFB_FreeHWSurface(this, &tmp);
        return -1;
    }
    else {
        return STAPI_Blit(this, fd, src, srcrect, dst, dstrect, wait_sync);
    }

    return STAPI_Blit(this, fd, src, srcrect, dst, dstrect, wait_sync);
}
#endif


/* ========================================================================
   STGFB driver bootstrap functions
   ======================================================================== */
static int STGFB_Available (void)
{
    return 1;
}


static void STGFB_DeleteDevice (GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}


static GAL_VideoDevice *STGFB_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (this) {
        memset(this, 0, (sizeof *this));
        this->hidden = (struct GAL_PrivateVideoData *)malloc((sizeof *this->hidden));
    }

    if ((this == NULL) || (this->hidden == NULL)) {
        GAL_OutOfMemory();
        if (this)
            free(this);

        return 0;
    }

    memset(this->hidden, 0, (sizeof *this->hidden));

    /* Set the function pointers */
    this->VideoInit = STGFB_VideoInit;
    this->ListModes = STGFB_ListModes;
    this->SetVideoMode = STGFB_SetVideoMode;
    this->SetColors = STGFB_SetColors;
    this->VideoQuit = STGFB_VideoQuit;
    this->UpdateRects = STGFB_UpdateRects;
#ifdef _MGRM_PROCESSES
    this->RequestHWSurface = STGFB_RequestHWSurface;
#endif
    this->AllocHWSurface = STGFB_AllocHWSurface;
    this->FreeHWSurface = STGFB_FreeHWSurface;
    this->CheckHWBlit = STGFB_CheckHWBlit;
    this->FillHWRect = STGFB_FillHWRect;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;
    this->free = STGFB_DeleteDevice;

    return this;
}


VideoBootStrap STGFB_bootstrap = {
    "stgfb", "ST Framebuffer Console",
    STGFB_Available, STGFB_CreateDevice
};


static int STGFB_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    const char *GAL_fbdev;

    /* open framebuffer device */
    GAL_fbdev = getenv("FRAMEBUFFER");
    if ( GAL_fbdev == NULL )
        GAL_fbdev = "/dev/fb0";

    console_fd = open(GAL_fbdev, O_RDWR, 0);
    if (console_fd < 0) {
        GAL_SetError("NEWGAL>STGFB: Unable to open %s\n", GAL_fbdev);
        goto init_err;
    }
    framebuffer_fd = console_fd;

    /* Get the type of video hardware */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) {
        GAL_SetError("NEWGAL>STGFB: Couldn't get console hardware info\n");
        goto init_err;
    }
    print_finfo(&finfo);

    switch (finfo.type) {
        case FB_TYPE_PACKED_PIXELS:
            /* Supported, no worries.. */
            break;

        default:
            GAL_SetError("NEWGAL>STGFB: Unsupported console hardware\n");
            goto init_err;
    }

    switch (finfo.visual) {
        case FB_VISUAL_TRUECOLOR:
        case FB_VISUAL_PSEUDOCOLOR:
        case FB_VISUAL_STATIC_PSEUDOCOLOR:
        case FB_VISUAL_DIRECTCOLOR:
            break;

        default:
            GAL_SetError("NEWGAL>STGFB: Unsupported console hardware\n");
            goto init_err;
    }

    /* Memory map the device */
    mapped_offset = (((long)finfo.smem_start) -
                    (((long)finfo.smem_start)&~(getpagesize () - 1)));
    mapped_memlen = finfo.smem_len + mapped_offset;
    
    mapped_mem = mmap(NULL, mapped_memlen,
                            PROT_READ|PROT_WRITE, MAP_SHARED, console_fd, 0);
    
    if (mapped_mem == (char *)-1) {
        GAL_SetError("NEWGAL>STGFB: Unable to memory map the video hardware\n");
        mapped_mem = NULL;
        goto init_err;
    }

    /* Determine the current screen depth */
    if (ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        GAL_SetError("NEWGAL>STGFB: Couldn't get console pixel format\n");
        goto init_err;
    }
    print_vinfo(&vinfo);

    /* init ST special device */
    if (STAPI_Init(this) < 0) {
        GAL_SetError("NEWGAL>STGFB: STAPI_Init failed\n");
        goto init_err;
    }

    /* init video memory bucket */
    if (ISSERVER) {
        gal_vmbucket_init(&vmem_bucket, (unsigned char*)finfo.smem_start, finfo.smem_len);
    }

    /* set the acceleration info flags */
    this->info.hw_available = 1;
    this->info.video_mem = finfo.smem_len / 1024;
    this->info.blit_fill = 0;
    this->info.blit_hw = 1;
    this->info.blit_hw_A = 1;
    this->info.blit_hw_CC = 1;

    /* We're done! */
    return 0;

init_err:
    if (mapped_mem != MAP_FAILED)
        munmap(mapped_mem, finfo.smem_len);
    if (console_fd >= 0)
        close(console_fd);
    return -1;
}


static GAL_Rect **STGFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect**) -1;
}


static GAL_Surface *STGFB_SetVideoMode (_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int depth;
    Uint32 Rmask, Gmask, Bmask, Amask;

    switch (screen_bmp.ColorType) {
  		case STGXOBJ_COLOR_TYPE_RGB888:
            depth = 32;
            Amask = 0x00000000;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;

  		case STGXOBJ_COLOR_TYPE_ARGB8888:
  		case STGXOBJ_COLOR_TYPE_ARGB8888_255:
            depth = 32;
            Amask = 0xFF000000;
            Rmask = 0x00FF0000;
            Gmask = 0x0000FF00;
            Bmask = 0x000000FF;
            break;

		case STGXOBJ_COLOR_TYPE_RGB565:
            depth = 16;
            Amask = 0x00000000;
            Rmask = 0x0000F800;
            Gmask = 0x000007E0;
            Bmask = 0x0000001F;
            break;

		case STGXOBJ_COLOR_TYPE_ARGB1555:
            depth = 16;
            Amask = 0x00008000;
            Rmask = 0x00007C00;
            Gmask = 0x000003E0;
            Bmask = 0x0000001F;
            break;

        default:
            fprintf(stderr, "NEWGAL>STGFB: Unsupported pixel format: %d\n", screen_bmp.ColorType);
            return NULL;
    }

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat(current, depth, Rmask, Gmask, Bmask, Amask)) {
        fprintf(stderr, "NEWGAL>STGFB: Couldn't allocate new pixel format for requested mode\n");
        return NULL;
    }

    debug_print("R:mask:%x, shift:%d, loss:%d\n\
            G:mask:%x, shift:%d, loss:%d\n\
            B:mask:%x, shift:%d, loss:%d\n\
            A:mask:%x, shift:%d, loss:%d\n",
            current->format->Rmask, current->format->Rshift, current->format->Rloss,
            current->format->Gmask, current->format->Gshift, current->format->Gloss,
            current->format->Bmask, current->format->Bshift, current->format->Bloss,
            current->format->Amask, current->format->Ashift, current->format->Aloss
            );

    /* Set up the new mode framebuffer */
    current->w = vinfo.xres;
    current->h = vinfo.yres;
    current->pitch = finfo.line_length;

    if (ISSERVER) {
        STGFB_AllocHWSurface(this, current);
        current->flags |= GAL_FULLSCREEN;
    } else {
        current->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        current->hwdata->user_addr = mapped_mem + mapped_offset;
        current->hwdata->block = NULL;

	    memset(&current->hwdata->st_bitmap, 0, sizeof(STGXOBJ_Bitmap_t));
        memcpy(&current->hwdata->st_bitmap, &screen_bmp, sizeof(STGXOBJ_Bitmap_t));

        current->pixels = (void*)(mapped_mem + mapped_offset);
        current->pitch = finfo.line_length;
        current->flags = GAL_FULLSCREEN | GAL_HWSURFACE;
    }

    debug_print("STGFB_SetVideoMode >> screen:%p, screen_usradder:%p, pitch:%d\n", 
            current, (char*)current->pixels, current->pitch);

    /* We're done */
    return current;
}


static void STGFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    return;
}


static int STGFB_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    int offset, pitch;
    gal_vmblock_t *block;

    surface->hwdata = NULL;

#ifdef _MGRM_PROCESSES
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};

    if (ISSERVER) {
        block = gal_vmbucket_alloc(&vmem_bucket, surface->pitch, surface->h);
        if (block) {
            offset = block->offset;
            pitch = block->pitch;
        }
    }
    else {
        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof(REQ_HWSURFACE);

        ClientRequest(&req, &reply, sizeof (REP_HWSURFACE));

        block = (gal_vmblock_t *)reply.bucket;
        offset = reply.offset;
        pitch = reply.pitch;
    }
#else
    block = gal_vmbucket_alloc(&vmem_bucket, surface->pitch, surface->h);
    if (block) {
        offset = block->offset;
        pitch = block->pitch;
    }
#endif

    if (block) {
        surface->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        if (surface->hwdata == NULL)
            return -1;

        surface->hwdata->block = block;
        surface->hwdata->offset = offset;

	    memset(&surface->hwdata->st_bitmap, 0, sizeof(STGXOBJ_Bitmap_t));
        memcpy(&surface->hwdata->st_bitmap, &screen_bmp, sizeof(STGXOBJ_Bitmap_t));

        surface->hwdata->st_bitmap.Width = surface->w;
        surface->hwdata->st_bitmap.Height = surface->h;
        surface->hwdata->st_bitmap.Pitch = surface->pitch;
        surface->hwdata->st_bitmap.Size1 = surface->pitch * surface->h;
        surface->hwdata->st_bitmap.Data1_p = (void*)(finfo.smem_start + offset);
        surface->hwdata->user_addr = mapped_mem + offset; 

        debug_print("addr:%p, user_addr;%p, size1:%d, pitch:%d, block:%p\n",
                surface->hwdata->st_bitmap.Data1_p, surface->hwdata->user_addr, 
                surface->hwdata->st_bitmap.Size1, surface->hwdata->st_bitmap.Pitch,
                surface->hwdata->block);

        surface->flags |= GAL_HWSURFACE;
        surface->pixels = surface->hwdata->user_addr;
        surface->pitch = surface->hwdata->st_bitmap.Pitch;
        return 0;
    } 
    else {
        surface->flags &= ~GAL_HWSURFACE;
        return -1;
    }

    return 0;
}


static void STGFB_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    if (surface->hwdata == NULL) {
        surface->pixels = NULL;
        return;
    }

    if (surface->hwdata->block == NULL) {
        free(surface->hwdata);
        surface->pixels = NULL;
        surface->hwdata = NULL;
        return;
    }

#ifdef _MGRM_PROCESSES
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, surface->hwdata->block};
    REP_HWSURFACE reply = {0, 0};

    if (ISSERVER) {
        gal_vmbucket_free(&vmem_bucket, surface->hwdata->block);
    }
    else {
        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest(&req, &reply, sizeof (REP_HWSURFACE));
    }
#else
    gal_vmbucket_free(&vmem_bucket, surface->hwdata->block);
#endif

    free(surface->hwdata);
    surface->pixels = NULL;
    surface->hwdata = NULL;
}


static void STGFB_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply)
{
    /* alloc hw surface */
    if (request->bucket == NULL) {     
        gal_vmblock_t* block = gal_vmbucket_alloc(&vmem_bucket, request->pitch, request->h);
        if (block) {
            debug_print("STGFB_RequestHWSurface >> alloc an block:%p\n", block);
            reply->offset = block->offset;
            reply->pitch = block->pitch;
            reply->bucket = (void*)block;
        } 
        else {
            reply->bucket = NULL;
        }

    }
    /* free hw surface */
    else { 
        gal_vmblock_t* block = (gal_vmblock_t*)request->bucket;
        debug_print("STGFB_RequestHWSurface >> free an block:%p\n", block);
        if (block != NULL) {
            gal_vmbucket_free(&vmem_bucket, block);
            block = NULL;
        }
    }
}


static int STGFB_FillHWRect (_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    if (dst == NULL || rect == NULL) {
        fprintf(stderr, "STGFB_FillHWRect >> dst or dstrect NULL\n");
        return -1;
    }

    if (rect->w <= 0 || rect->h <= 0) {
        fprintf(stderr, "STGFB_FillHWRect >> dstrect invaildate\n");
        return -1;
	}

    if (dst->hwdata == NULL) {
        fprintf(stderr, "STGFB_FillHWRect >> dst hwdata NULL\n");
        return -1;
    }

    if (dst->hwdata->st_bitmap.Data1_p == NULL) {
        fprintf(stderr, "STGFB_FillHWRect >> dst video memory NULL\n");
        return -1;
    }

    GAL_Color gal_color;

    /* limit the rect */
    if (rect->w > dst->hwdata->st_bitmap.Width)
        rect->w = dst->hwdata->st_bitmap.Width;

    if (rect->h > dst->hwdata->st_bitmap.Height)
        rect->h = dst->hwdata->st_bitmap.Height;

    if (rect->x + rect->w > dst->hwdata->st_bitmap.Width)
        rect->w = dst->hwdata->st_bitmap.Width - rect->x;
    
    if (rect->y + rect->h > dst->hwdata->st_bitmap.Height)
        rect->h = dst->hwdata->st_bitmap.Height - rect->y;

    /* conver the pixel to RGB or RGBA */
    GAL_GetRGBA(color, dst->format, &gal_color.r, &gal_color.g, &gal_color.b, &gal_color.a);
    //gal_color.a = 255;
    debug_print("color:%d, r:%d, g:%d, b:%d, a:%d\n", 
            color, gal_color.r, gal_color.g, gal_color.b, gal_color.a);

    return STAPI_FillRectangle(this, console_fd, dst, rect, &gal_color, TRUE);
}


static int STGFB_CheckHWBlit (_THIS, GAL_Surface * src, GAL_Surface * dst)
{
    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    /* only supported the hw surface accelerated. */
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE)) {
        fprintf(stderr, "src(%s) dst(%s)\n", 
                (src->flags & GAL_HWSURFACE) ? "HW" : "SW",
                (dst->flags & GAL_HWSURFACE) ? "HW" : "SW");
        return -1;
    }

    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = STGFB_HWAccelBlit;
    return 0;
}


static int STGFB_HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect)
{
    if (src == NULL || srcrect == NULL || dst == NULL || dstrect == NULL)
        return -1;

    if (srcrect->w <= 0 || srcrect->h <= 0 || dstrect->w <= 0 || dstrect->h <= 0) {
        fprintf(stderr, "STGFB_HWBlit >> srcrect or dstrect invaildate\n");
        return -1;
	}

    if (src->hwdata == NULL || dst->hwdata == NULL) {
        fprintf(stderr, "STGFB_HWBlit >> src or dst hwdata NULL\n");
        return -1;
    }

    if (src->hwdata->st_bitmap.Data1_p == NULL || dst->hwdata->st_bitmap.Data1_p == NULL) {
        fprintf(stderr, "STGFB_HWBlit >> src or dst video memory address NULL\n");
        return -1;
    }

    STGXOBJ_Bitmap_t* src_st_bitmap = &src->hwdata->st_bitmap;
    STGXOBJ_Bitmap_t* dst_st_bitmap = &dst->hwdata->st_bitmap;

    /* limit the rect */
    if (srcrect->w > src_st_bitmap->Width)
        srcrect->w = src_st_bitmap->Width;

    if (srcrect->h > src_st_bitmap->Height)
        srcrect->h = src_st_bitmap->Height;

    if (srcrect->x + srcrect->w > src_st_bitmap->Width)
        srcrect->w = src_st_bitmap->Width - srcrect->x;
    
    if (srcrect->y + srcrect->h > src_st_bitmap->Height)
        srcrect->h = src_st_bitmap->Height - srcrect->y;

    if (dstrect->w > dst_st_bitmap->Width)
        dstrect->w = dst_st_bitmap->Width;

    if (dstrect->h > dst_st_bitmap->Height)
        dstrect->h = dst_st_bitmap->Height;

    if (dstrect->x + dstrect->w > dst_st_bitmap->Width)
        dstrect->w = dst_st_bitmap->Width - dstrect->x;
    
    if (dstrect->y + dstrect->h > dst_st_bitmap->Height)
        dstrect->h = src_st_bitmap->Height - srcrect->y;

#if 1
    return STAPI_Blit(current_video, framebuffer_fd, src, srcrect, dst, dstrect, TRUE);
#else
    return STAPI_BlitWrapper(current_video, framebuffer_fd, src, srcrect, dst, dstrect, TRUE);
#endif
}


static int STGFB_SetColors (_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return 1;
}


/* Note: If we are terminated, this could be called in the middle of
 * another video routine -- notably UpdateRects. */
static void STGFB_VideoQuit (_THIS)
{
    if (ISSERVER) {
        int i;

        /* set the screen to black -- software */
        for (i=0; i < mapped_memlen; i++ )
            *(mapped_mem+i) = 0x00;

        /* destroy video memory bucket */
        gal_vmbucket_destroy(&vmem_bucket);
    }

    /* close console and input file descriptors */
    if (console_fd > 0) {
        /* unmap the video framebuffer */
        if ( mapped_mem ) {
            munmap(mapped_mem, mapped_memlen);
            mapped_mem = NULL;
        }

        close(console_fd);
        console_fd = -1;
    }

    /* close ST device */
    STAPI_Term(this);
}

#endif /* _MGGAL_STGFB */

