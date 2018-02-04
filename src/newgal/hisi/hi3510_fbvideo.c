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
** $Id: hi3510_fbvideo.c 10629 2008-08-07 03:45:43Z tangjianbin $
**
** hi3510_fbvideo.c: NewGAL Engine for hi3510
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/page.h>        /* For definition of PAGE_SIZE */
#include <assert.h>
#include "common.h"
#ifdef _MGGAL_HI3510
#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "pixels_c.h"
#include "hi3510_fb.h"
#include "hi3510_fbvideo.h" 


#ifndef _MGRM_THREADS
#define REQID_HWSURFACE         0x0013
#endif

//#define FBCON_DEBUG 1
//#define _PC_DEBUG 1  

#ifdef _USE_2D_ACCEL
#include "hi_tde.h" 
#endif

#ifdef _PC_DEBUG
/*hi3510fb extend*/
typedef struct tagfb_colorkey
{
    unsigned long key;
    unsigned char key_enable;	
    unsigned char mask_enable;	
    unsigned char rmask;
    unsigned char gmask;
    unsigned char bmask;
    char  reserved[3];
}fb_colorkey;

#define FBIOGET_COLORKEY_HI3510       0x4690  /*hi3510 extend*/
#define FBIOPUT_COLORKEY_HI3510       0x4691  /*hi3510 extend*/
#define FBIOGET_ALPHA_HI3510          0x4692  /*hi3510 extend*/
#define FBIOPUT_ALPHA_HI3510          0x4693  /*hi3510 extend*/
#define FBIOPUT_SCREEN_ORIGIN_HI3510  0x4694  /*hi3510 extend*/
#define FBIOGET_SCREEN_ORIGIN_HI3510  0x4695  /*hi3510 extend*/
#endif

extern BOOL mgIsServer;
//extern GAL_VideoDevice* current_video;

#ifndef _MGRM_THREADS
struct private_hwdata
{
	const vidmem_bucket* pBucket;
    GAL_VideoDevice* pDevice; /*该变量用于区分哪个图层，在硬件加速blit中用到*/
};

static int HI3510_SetScreenAttr(_THIS, Uint8 siAttr, void* pValue);
//int HI3510_SetScreenAttr(Uint8 siAttr, void* pValue);
static struct private_hwdata* HI3510_CreateHwData(_THIS, const vidmem_bucket* pBucket);
#endif

/* Initialization/Query functions */
static int HI3510_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **HI3510_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *HI3510_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static void HI3510_VideoQuit(_THIS);

/* Hardware surface functions */
static int HI3510_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size);
static void HI3510_FreeHWSurfaces(_THIS, GAL_Surface *surface);
static void HI3510_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
static int HI3510_AllocHWSurface(_THIS, GAL_Surface *surface);
static void HI3510_FreeHWSurface(_THIS, GAL_Surface *surface);
static void HI3510_WaitVBL(_THIS);
static void HI3510_WaitIdle(_THIS);

static int hi3510_console = 0;

static int HI3510_Available(void)
{
    const char *GAL_fbdev;

    GAL_fbdev = getenv("FRAMEBUFFER");
    if ( GAL_fbdev == NULL ) {
        GAL_fbdev = "/dev/fb/0";
    }

    hi3510_console = open(GAL_fbdev, O_RDWR, 0);

    if ( hi3510_console >= 0 ) {
        //close(hi3510_console);
    }
    else {
    	fprintf(stderr, "failed to open file :%s!\n", GAL_fbdev);
        return 0;
    }

    return(hi3510_console >= 0);
}


#ifdef _MGRM_THREADS
struct GAL_PrivateVideoData g_stHidden_3510 = { 0 };
#else
struct GAL_PrivateVideoData g_stHidden_3510[2]; //two layers
#endif

#ifdef _MGRM_THREADS
struct GAL_VideoDevice *g_pVideo_3510 = NULL;
#endif

GAL_Surface logosurface;

#include "pix_array.c"

#ifdef _USE_2D_ACCEL
#ifdef _USE_DBUF
#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
	|| defined(_TAP2_VERT_HORI) || defined(_TAP3_VERT))
    
static int HI3510_AntiFlicker(_THIS, GAL_Surface *src, 
	                                GAL_Rect *srcrect, GAL_Surface *dst, 
	                                GAL_Rect *dstrect, Uint32 *puiOffsetX, 
	                                Uint32 *puiOffsetY);

static int HI3510_SetOffset(int fd, 
                                  Uint32 uiOffsetX, 
                                  Uint32 uiOffsetY);

#endif
#define HI3510_ALIGN_4(x) (((x)+3)&~3)
static  int HI3510_GetColorFmt(const GAL_PixelFormat* pixel_fmt, TDE_COLOR_FORMAT* fmt)
{
	int ret = -1;
    Uint32 Rmask, Gmask, Bmask, Amask;

    Rmask = pixel_fmt->Rmask;
    Gmask = pixel_fmt->Gmask;
    Bmask = pixel_fmt->Bmask;
    Amask = pixel_fmt->Amask;
    
    switch (pixel_fmt->BitsPerPixel)
    {
       case 12 : //RGB,4:4:4
           if (Rmask==0x0f00 && (Gmask == 0x00f0) && Bmask == 0x000f)
           {
           	   *fmt = TDE_COLOR_FORMAT_RGB444;
           	   ret = 0;
           }
           break;
           
       case 16 :
           if (Rmask == 0x0f00 && (Gmask == 0x00f0) && Bmask == 0x000f && Amask == 0xf000)
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);
               *fmt = TDE_COLOR_FORMAT_RGB4444;
               ret = 0;
           }
           else if (Rmask == 0x7c00 && (Gmask == 0x03e0) && Bmask == 0x001f && Amask == 0x0000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);
               *fmt = TDE_COLOR_FORMAT_RGB555;
               ret = 0;
           }
           else if (Rmask == 0x7c00 && (Gmask == 0x03e0) && Bmask == 0x001f && Amask == 0x8000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);           	
               *fmt = TDE_COLOR_FORMAT_RGB1555;
               ret = 0;
           }
           else if (Rmask == 0xf800 && (Gmask == 0x07e0) && Bmask == 0x001f && Amask == 0x0000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);           	
               *fmt = TDE_COLOR_FORMAT_RGB565;
               ret = 0;
           }
           break;
       case 24 :
           if (Rmask == 0xff0000 && (Gmask == 0x00ff0) && Bmask == 0x0000ff && Amask == 0x000000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffffff);           	
               *fmt = TDE_COLOR_FORMAT_RGB888;
               ret = 0;
           }
           break;
       case 32 :
           if (Rmask == 0xff0000 && (Gmask == 0x00ff00) && Bmask == 0x0000ff && Amask == 0xff000000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffffffff);           	
               *fmt = TDE_COLOR_FORMAT_RGB8888;
               ret = 0;
           }
           break;
           
       default:
           ret = -1;
    }

	return ret;
}
	
#define HI3510_GET_PHY(virtual_addr) ((U8*)(virtual_addr-mapped_mem + saved_finfo.smem_start))
#define HI3510_CHECK_VIRTUAL_ADDR(virtual_addr)  assert(virtual_addr-mapped_mem < saved_finfo.smem_len)

static void HI3510_GetRGB(gal_pixel color, GAL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8 *b)
{
    GAL_GetRGB(color, fmt, r, g, b);
    if (fmt->Rloss && (*r & 0x80))
    {
        *r += 0xff >> (8-fmt->Rloss);
    }
    if (fmt->Gloss && (*g & 0x80))
    {
        *g += 0xff >> (8-fmt->Gloss);
    }

    if (fmt->Bloss && (*b & 0x80))
    {
        *b += 0xff >> (8-fmt->Bloss);
    }
}

#if 0
static int ShowLogo (_THIS, Uint32* dst)
{
    TDE_MoveBlitParam param;
    Uint8* pvirtual_addr;
    GAL_Rect OutRect = {470, 490, 198, 51};

    logosurface.pitch = BM_WIDTH*2;
    memset(&param, 0x00, sizeof(param));
    param.DataOpt = TDE_DATA_OPT_ALPHA_INTERNAL;
    param.InternalAlpha = 30;

    param.opt_width = (U16)(OutRect.w);
    param.opt_height = (U16)(OutRect.h);

    param.out_stride = (U16)HI3510_ALIGN_4(__gal_screen->pitch);
    param.out_pixels = (Uint8*)dst + OutRect.x*2+ OutRect.y*__gal_screen->pitch;
    param.InColorFormat = param.OutColorFormat = TDE_COLOR_FORMAT_RGB1555;
    param.dst_pixels = param.out_pixels;
    param.dst_stride = HI3510_ALIGN_4(param.out_stride);

    pvirtual_addr = (Uint8*)logosurface.pixels;

    HI3510_CHECK_VIRTUAL_ADDR(pvirtual_addr);    
    param.src_pixels = HI3510_GET_PHY(pvirtual_addr);
    param.src_stride = HI3510_ALIGN_4(logosurface.pitch);
    printMBParam(&param);

#if 0
    {
      Uint8  *psrc, *pdst;

      psrc = logosurface.pixels;
      pdst = (Uint8*)dst + OutRect.x * 2+ OutRect.y*__gal_screen->pitch;

      printf ("pitch = %d\n", HI3510_ALIGN_4(param.out_stride));
     
      for (i = 0; i < OutRect.h; i++)
      {
          memcpy(pdst, psrc, HI3510_ALIGN_4(param.out_stride));          
          psrc += HI3510_ALIGN_4(logosurface.pitch);
          pdst += HI3510_ALIGN_4(param.out_stride);

      }

      return 0;
    }
#else
    if(HI_SUCCESS != TDE_MoveBlit(&param))
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }
#endif

    return 0;            
}
#endif

static int HI3510_HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
	TDE_MoveBlitParam param;
	Uint8 r,g,b;
	Uint8* pvirtual_addr;
    TDE_COLOR_FORMAT color_fmt;
#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF)     
    Uint32 uiOffsetX = 0;
    Uint32 uiOffsetY = 0;
#endif
    
#ifndef _MGRM_THREADS
    GAL_VideoDevice* this;
    this = ((struct private_hwdata*)src->hwdata)->pDevice;
    #ifdef FBCON_DEBUG    
    if (this == NULL || this != ((struct private_hwdata*)dst->hwdata)->pDevice)
    	fprintf(stderr, "the video device is error! src->pdevice:%p, dst->pdevice:%p",
    	        this, ((struct private_hwdata*)dst->hwdata)->pDevice);
     #endif    	
#endif

#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF)
    /*if dst is current video surface, should do antiflicker*/
#ifdef _MGRM_THREADS
    if ((src == __gal_screen) && (src->flags & GAL_HWSURFACE) 	
    	&& (dst == current_video->screen))
#else
    if (mgIsServer && (src->flags & GAL_DOUBLEBUF) 
    	&& (src->flags & GAL_HWSURFACE)	&& (dst == this->screen))
#endif
    {
       if ((TAP_MIN_VALUE <= tap_level) && (TAP_MAX_VALUE >= tap_level))
       {
#ifdef _MGRM_THREADS
	       if (0 == HI3510_AntiFlicker(current_video, src, srcrect, dst, 
	       	                           dstrect, &uiOffsetX, &uiOffsetY))
#else
           if (0 == HI3510_AntiFlicker(this, src, srcrect, dst, dstrect,
           	                           &uiOffsetX, &uiOffsetY))
#endif
	       {
	           HI3510_SetOffset(console_fd, uiOffsetX, uiOffsetY); 
	           return 0;
	       }
	   }

      }
#endif

    memset(&param, 0x00, sizeof(param));
    if (HI3510_GetColorFmt(src->format, &color_fmt) < 0)
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
    	return -1;
    }
    
    if (src->flags & GAL_SRCCOLORKEY)
    {
        param.ColorSpaceOpt = TDE_COLORSPACE_ENABLE;
        HI3510_GetRGB(src->format->colorkey, src->format, &r, &g, &b);
        param.ColorSpaceMin = MAKERGB(r, g, b);
        HI3510_GetRGB(src->format->colorkey, src->format, &r, &g, &b);
        param.ColorSpaceMax = MAKERGB(r, g, b);
    }

    if (src->flags & GAL_SRCALPHA)
    {
    	if (0)//(src->format->Amask != 0)
    	{
    	    param.DataOpt = TDE_DATA_OPT_ALPHA_SRC;    		
    	}
    	else
    	{
    	    param.DataOpt = TDE_DATA_OPT_ALPHA_INTERNAL;
    	    param.InternalAlpha = src->format->alpha;
    	}
    }

#if 0
    if ((src->flags & GAL_ROP) && src->format->rop != ROP_SET)
    {
    	if (src->format->rop == ROP_AND)
    	    param.RopCode = TDE_ROP_PSDa;
    	else if (src->format->rop == ROP_OR)
    		param.RopCode = TDE_ROP_PSDo;
    	else if (src->format->rop == ROP_XOR)
    		param.RopCode = TDE_ROP_PSDx;
        else 
           printf("the rop is invalid, file:%s, line:%d!\n", __FILE__,__LINE__);
        param.DataOpt = TDE_DATA_OPT_ROP;
    }
#endif

    assert(dstrect->w==srcrect->w && dstrect->h==srcrect->h);
    param.opt_width = (U16)(dstrect->w);
    param.opt_height = (U16)(dstrect->h);

    param.out_stride = (U16)HI3510_ALIGN_4(dst->pitch);
    pvirtual_addr = (Uint8*)dst->pixels + dstrect->x*dst->format->BytesPerPixel+ dstrect->y*dst->pitch;
    HI3510_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.out_pixels = HI3510_GET_PHY(pvirtual_addr);
    param.InColorFormat = param.OutColorFormat = color_fmt;
    param.dst_pixels = param.out_pixels;
    param.dst_stride = HI3510_ALIGN_4(param.out_stride);

    pvirtual_addr = (Uint8*)src->pixels + srcrect->x*src->format->BytesPerPixel+ srcrect->y*src->pitch;
    HI3510_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.src_pixels = HI3510_GET_PHY(pvirtual_addr);
    param.src_stride = HI3510_ALIGN_4(src->pitch);
    printMBParam(&param);

    if(HI_SUCCESS != TDE_MoveBlit(&param))
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }
    
    return 0;
}

static int HI3510_CheckHWBlit(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    TDE_COLOR_FORMAT color_fmt;

    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    //only supported the hw surface accelerated.
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE))
    {
    	return -1;
    }

#if 0
    if ((src->flags&GAL_DESTCOLORKEY))
    {
    	fprintf(stderr, "src->flags:%x,unsupported GAL_DESTCOLORKEY\n", src->flags);
        return -1;
    }

    if ((src->flags&GAL_SRCALPHA) && (src->flags&GAL_ROP) && (src->format->rop != ROP_SET))
    {
    	fprintf(stderr, "unsupported GAL_SRCALPHA|GAL_ROP\n");
        return -1;	
    }
#endif

    if(HI3510_GetColorFmt(src->format, &color_fmt) < 0)
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }
    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = HI3510_HWAccelBlit;
    return 0;
}

 /* Fills a surface rectangle with the given color */
static int HI3510_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
	Uint8 r,g,b;
	Uint8* pvirtual_addr;
	TDE_SolidDrawParam param;
    TDE_COLOR_FORMAT color_fmt;
    	
	if (!(dst->flags & GAL_HWSURFACE))
	{
		return -1;
	}

    memset(&param, 0x00, sizeof(param));

    GAL_GetRGB(color, dst->format, &r, &g, &b);
    param.sd_fill_color = MAKERGB(r, g, b);

    param.ColorSpaceOpt = TDE_COLORSPACE_DISABLE;

    if (dst->format->Amask == 0xff000000)
    {
 	    param.DataOpt = TDE_DATA_OPT_ALPHA_INTERNAL;
    	    param.InternalAlpha = 0xff;
            param.OutAlphaFrom = TDE_OUT_ALPHAFROM_INTERNAL;
    }
    else
    {
        param.DataOpt = TDE_DATA_OPT_NONE;
        param.OutAlphaFrom = TDE_OUT_ALPHAFROM_SRC;
    }

    if (HI3510_GetColorFmt(dst->format, &color_fmt) < 0)
    {
        fprintf(stderr, "error: file:%s, line:%d\n", __FILE__, __LINE__);
    	return -1;
    }
    
    param.InColorFormat = color_fmt;
    param.OutColorFormat = color_fmt;

    param.opt_width = (U16)(rect->w);
    param.opt_height = (U16)(rect->h);
    param.out_stride = (U16)HI3510_ALIGN_4(dst->pitch);
    pvirtual_addr = (Uint8*)dst->pixels + rect->x*dst->format->BytesPerPixel+ rect->y*dst->pitch;
    HI3510_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.out_pixels = HI3510_GET_PHY(pvirtual_addr);

    if(HI_SUCCESS != TDE_SolidDraw(&param))
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }

    	
    printSDParam(&param);
	return 0;
}

/* Fills a surface rectangle with the given bitmap */
int HI3510_FillHWRectWithPattern(_THIS, GAL_Surface	*dst, GAL_Rect *rect, PBITMAP pattern)
{
	Uint8* pvirtual_addr;
	TDE_PatternFillParam param;
    TDE_COLOR_FORMAT color_fmt;
    
	if (!(dst->flags & GAL_HWSURFACE))
	{
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
		return -1;
	}
  
    if (pattern->bmPitch > 64)
    {
        return -1;
    }
    //not allocate from vram
    if (!(pattern->bmType & 0x80))
    {
        fprintf(stderr, "error,file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }

    memset(&param, 0x00, sizeof(param));
    if (HI3510_GetColorFmt(dst->format, &color_fmt) < 0)
    {
        fprintf(stderr, "error,file:%s, line:%d\n", __FILE__, __LINE__);
    	return -1;
    }

    if (rect == NULL)
    {
        fprintf(stderr, "error,file:%s, line:%d\n", __FILE__, __LINE__);
    	return -1;
    }

    if (pattern->bmType & BMP_TYPE_COLORKEY)
    {
        param.ColorSpaceOpt = TDE_COLORSPACE_ENABLE;
        param.ColorSpaceMin = pattern->bmColorKey;
        param.ColorSpaceMax = pattern->bmColorKey;
    }

    if (pattern->bmType & BMP_TYPE_ALPHACHANNEL)
    {
    	param.DataOpt = TDE_DATA_OPT_ALPHA_INTERNAL;
    	param.InternalAlpha = pattern->bmAlpha;
    }

    param.opt_width = (U16)(rect->w);
    param.opt_height = (U16)(rect->h);
    param.out_stride = (U16)HI3510_ALIGN_4(dst->pitch);
    pvirtual_addr = (Uint8*)dst->pixels + rect->x*dst->format->BytesPerPixel+ rect->y*dst->pitch;
    HI3510_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.out_pixels = HI3510_GET_PHY(pvirtual_addr);
    param.InColorFormat = param.OutColorFormat = color_fmt;
    param.pattern_width= pattern->bmWidth;
    param.pattern_height = pattern->bmHeight;
    
    //memmove(pattern_addr, pattern->bmBits, pattern_size);
    param.src_pixels = HI3510_GET_PHY(pattern->bmBits);
    param.src_stride = HI3510_ALIGN_4(pattern->bmPitch);
    if(HI_SUCCESS != TDE_PatternFill(&param))
    {
        fprintf(stderr, "error,file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }
    printPFParam(&param);
	return 0;
}

#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF)  
    
/*obligate middle video memory for antiflicker*/
static int HI3510_GetAllAFVMem(_THIS, 
                                  Uint32 iPitch, 
                                  Uint32 iHeight,
                                  Uint32 *puiVirtualYRes)
{    
    struct fb_fix_screeninfo finfo;
    Uint32 iSize = iPitch * iHeight;
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) 
    {
	    fprintf (stderr, "Couldn't get console hardware info");	    
	    return -1;
    }
#if defined(_TAP_ALL) || defined(_TAP3_VERT_HORI)
    if (mapped_memlen >= iSize*5)
    {
        middle_buf1 = (Uint32)finfo.smem_start + mapped_offset;
        middle_buf2 = middle_buf1 + iSize;
        middle_buf3 = middle_buf2 + iSize;  
        current_buf = middle_buf3 + iSize;
        tap_level = TAP_2_VERT;//TAP_3_VERT_HORI;   
        *puiVirtualYRes = iHeight << 2;
        return iSize*3;
    }
    else
    {
       return -1;
    }
#elif defined(_TAP2_VERT)
    if (mapped_memlen >= iSize*3)
    {
        middle_buf1 = (Uint32)finfo.smem_start + mapped_offset;  
        current_buf = middle_buf1 + iSize;
        tap_level = TAP_2_VERT;
        *puiVirtualYRes = iHeight << 1;
        return iSize;
    }
    else
    {
       return -1;
    }
#elif defined(_TAP2_VERT_HORI)
    if (mapped_memlen >= (iSize<<2))
    {
        middle_buf1 = (Uint32)finfo.smem_start + mapped_offset;
        middle_buf2 = middle_buf1 + iSize; 
        current_buf = middle_buf2 + iSize;
        tap_level = TAP_2_VERT_HORI;
        *puiVirtualYRes = 3 * iHeight;
        return iSize<<1;
    }
    else
    {
       return -1;
    }
#else //defined(_TAP3_VERT)
    if (mapped_memlen >= iSize<<2)
    {
        middle_buf1 = (Uint32)finfo.smem_start + mapped_offset;
        middle_buf2 = middle_buf1 + iSize;  
        current_buf = middle_buf2 + iSize;
        tap_level = TAP_3_VERT;
        *puiVirtualYRes = 3 * iHeight;
        return iSize<<1;
    }
    else
    {
       return -1;
    }
#endif
}

static int HI3510_SetOffset(int fd, 
                                  Uint32 uiOffsetX, 
                                  Uint32 uiOffsetY)
{
    struct fb_var_screeninfo vinfo;
    memset(&vinfo, 0, sizeof(struct fb_var_screeninfo));
    /* Set the video mode and get the final screen format */
    if ( ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0 )
    {
        fprintf (stderr, "Couldn't get console screen info");
        return -1;
    }
    vinfo.xoffset = uiOffsetX;
    vinfo.yoffset = uiOffsetY;
    if (ioctl(fd, FBIOPAN_DISPLAY, &vinfo) < 0)
    {
        fprintf(stderr, "Serious error, offset framebuffer failed.\n");
        return -1;
    }
    return 0;    
}

#ifdef _TAP_ALL

/*set antiflicker level*/
static int SetTapLevel(int iLayerIndex, int iLevel)
{ 

#ifndef _MGRM_THREADS
    if ((0 > iLayerIndex) || (1 < iLayerIndex)
    	|| (0 > iLevel) || (TAP_MAX_VALUE < iLevel)
    	|| (0 == g_stHidden_3510[iLayerIndex].uiMiddleBuf1))
    {
        return -1;
    }
    TDE_WaitForDone();
    g_stHidden_3510[iLayerIndex].uiTapLevel = (Uint32)iLevel;   
   
    if (0 == iLevel)
    {                
        HI3510_SetOffset(g_stHidden_3510[iLayerIndex].console_fd1,
                         0, 
                        (g_stHidden_3510[iLayerIndex].uiCurrentBuf
                        - g_stHidden_3510[iLayerIndex].uiMiddleBuf1)/
                        g_stHidden_3510[iLayerIndex].finfo.line_length);
                   
    }
#endif
    return 0;
}
#endif
struct timeval tv1, tv2;
static int HI3510_AntiFlicker(_THIS, GAL_Surface *src, 
	                                GAL_Rect *srcrect, GAL_Surface *dst, 
	                                GAL_Rect *dstrect, Uint32 *puiOffsetX, 
	                                Uint32 *puiOffsetY)
{
    static Uint32 uiDisplay = 0;
    static Uint32 uiOffset = 0;
    Uint8 *pBuf;
    if (shadow_buf == 0)
    {
        shadow_buf = current_buf
                     + (src->pixels - dst->pixels);
        uiOffset = this->offset_y * this->screen->pitch 
        	       + this->offset_x * this->screen->format->BytesPerPixel;
    }
    switch (tap_level)
    {
        case 1:
        {        
        	if (uiDisplay == current_buf)
        	{
        	    uiDisplay = middle_buf1;
        	}
        	else
        	{
        	    uiDisplay = current_buf;
        	}
            pBuf = (Uint8 *)(mapped_mem + (uiDisplay - middle_buf1));
            memset(pBuf, 
            	   0, 
            	   this->screen->pitch * this->screen->h);
            TDE_AntiFlicker(shadow_buf, 
	                    shadow_buf + this->screen->pitch, 
	                    uiDisplay, 
	                    this->screen->w - this->offset_x, 
	                    this->screen->h - 1 - this->offset_y,
	                    this->screen->pitch, 50, 1, uiOffset);
            
    	    
        	break;
        }
        case 2:
        {
            if (uiDisplay == current_buf)
            {
                uiDisplay = middle_buf2;
            }
            else
            {
                uiDisplay = current_buf;
            }
#if 1
            pBuf = (Uint8 *)(mapped_mem + (uiDisplay - middle_buf1));
            memset(pBuf, 
            	   0, 
            	   this->screen->pitch * this->screen->h);
#endif     
   	TDE_AntiFlicker(shadow_buf, 
                    shadow_buf + this->screen->pitch, 
                    middle_buf1, this->screen->w, 
                    this->screen->h - 1,
                    this->screen->pitch, 50, 0, 0);
            TDE_AntiFlicker(middle_buf1, 
            	middle_buf1 + this->screen->format->BytesPerPixel, 
            	uiDisplay, 
	            this->screen->w - 1 - - this->offset_x, 
	            this->screen->h - 2 - - this->offset_y, 
	            this->screen->pitch, 50, 1, uiOffset); 
        	break;
        }
        case 3:
        {
            if (uiDisplay == current_buf)
            {
                uiDisplay = middle_buf2;
            }
            else
            {
                uiDisplay = current_buf;
            }
            pBuf = (Uint8 *)(mapped_mem + (uiDisplay - middle_buf1));
            memset(pBuf, 
            	   0, 
            	   this->screen->pitch * this->screen->h);
            TDE_AntiFlicker(shadow_buf,
                shadow_buf + this->screen->pitch,
                middle_buf1, this->screen->w,
                this->screen->h - 1,
                this->screen->pitch, 33, 0, 0);
            TDE_AntiFlicker(middle_buf1, 
            	shadow_buf + (this->screen->pitch<<1), 
           	    uiDisplay,
                this->screen->w  - this->offset_x,
                this->screen->h  - this->offset_y,
                this->screen->pitch, 75, 1, uiOffset);

        	break;
        }
        case 4:
        {
            // gettimeofday(&tv1, NULL);
            if (uiDisplay == current_buf)
            {
                uiDisplay = middle_buf3;
            }
            else
            {
                uiDisplay = current_buf;
            }
#if 1
            pBuf = (Uint8 *)(mapped_mem + (uiDisplay - middle_buf1));
            memset(pBuf, 
            	   0, 
            	   this->screen->pitch * this->screen->h);
#endif     
       TDE_AntiFlicker(shadow_buf,
                shadow_buf + this->screen->pitch,
                middle_buf1, this->screen->w,
                this->screen->h - 1,
                this->screen->pitch, 33, 0, 0);
            TDE_AntiFlicker(middle_buf1, 
            	shadow_buf + (this->screen->pitch<<1) , 
            	middle_buf1,
                this->screen->w,
                this->screen->h - 2,
                this->screen->pitch, 75, 0, 0);
            TDE_AntiFlicker(middle_buf1, middle_buf1 + 2 , middle_buf2,
               this->screen->w - 1,
               this->screen->h - 2,
               this->screen->pitch, 33, 0, 0);
            TDE_AntiFlicker(middle_buf2, middle_buf1 + 4 , uiDisplay,
               this->screen->w - 2 - this->offset_x,
               this->screen->h - 2 - this->offset_y,
               this->screen->pitch, 75, 1, uiOffset);
           // gettimeofday(&tv2, NULL);
           // printf("anti, use time:%d\n",
             //   (tv2.tv_sec - tv1.tv_sec)*1000000 + tv2.tv_usec - tv1.tv_usec);
        	break;
        }
        default:
        	return -1;
    }
    *puiOffsetX = 0;
    *puiOffsetY = (uiDisplay - middle_buf1)/this->screen->pitch;
    //printf("zzzzzzzzzzzzzzzzzzzzzzz\n");
    //ShowLogo (this, uiDisplay);
    
    return 0;
}
 
//
#endif
#endif
#endif
static void HI3510_DeleteDevice(GAL_VideoDevice *device)
{
    free(device->hidden);
    free(device);
}

#if 0
BOOL FB_CreateDevice(GAL_VideoDevice *device)
{
    int console;
    const char *GAL_fbdev;
    GAL_VideoDevice *this = device;

    if (device == NULL)
    {
        return FALSE;
    }

    GAL_fbdev = "/dev/fb/0";
    console_fd = open(GAL_fbdev, O_RDWR, 0);
    if ( console_fd < 0 ) 
    {
        return FALSE;
    }
    
    
    memset(this, 0, (sizeof *this));
 
    wait_vbl = HI3510_WaitVBL;
    wait_idle = HI3510_WaitIdle;

    this->name = "hi3510";

    /* Set the function pointers */
    this->VideoInit = HI3510_VideoInit;
    this->ListModes = HI3510_ListModes;
    this->SetVideoMode = HI3510_SetVideoMode;
    this->VideoQuit = HI3510_VideoQuit;
    this->AllocHWSurface = HI3510_AllocHWSurface;
#ifdef _USE_2D_ACCEL
#ifdef _USE_DBUF
    this->CheckHWBlit = HI3510_CheckHWBlit;
    this->FillHWRect = HI3510_FillHWRect;
#endif
#endif
    this->FreeHWSurface = HI3510_FreeHWSurface;
    this->FreeVideo = HI3510_DeleteDevice;
    g_pVideo_3510 = this;

    return TRUE;
}
#endif

static int HI3510_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    GAL_VideoInfo* paccel_info =  &this->info;
    int i;
#ifdef _PC_DEBUG
    static unsigned char tde_register_base[0x100];
#endif

    console_fd = hi3510_console;
    /* Get the type of video hardware */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) 
    {
        fprintf (stderr, "Couldn't get console hardware info\n");
        HI3510_VideoQuit(this);
        return(-1);
    }

    /* Memory map the device, compensating for buggy PPC mmap() */
    mapped_offset = (((long)finfo.smem_start) -
                    (((long)finfo.smem_start)&~(PAGE_SIZE-1)));
    mapped_memlen = finfo.smem_len+mapped_offset;
    mapped_mem = mmap(NULL, mapped_memlen,
                      PROT_READ|PROT_WRITE, MAP_SHARED, console_fd, 0);

    if ( mapped_mem == (Uint8 *)-1 ) {
        fprintf (stderr, "Unable to memory map the video hardware\n");
        mapped_mem = NULL;
        HI3510_VideoQuit(this);
        return(-1);
    }

    /* Determine the current screen depth */
    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
        fprintf (stderr, "Couldn't get console pixel format\n");
        HI3510_VideoQuit(this);
        return(-1);
    }
    vformat->BitsPerPixel = vinfo.bits_per_pixel;
 
    for ( i=0; i<vinfo.red.length; ++i ) {
        vformat->Rmask <<= 1;
        vformat->Rmask |= (0x00000001<<vinfo.red.offset);
    }
    for ( i=0; i<vinfo.green.length; ++i ) {
        vformat->Gmask <<= 1;
        vformat->Gmask |= (0x00000001<<vinfo.green.offset);
    }
    for ( i=0; i<vinfo.blue.length; ++i ) {
        vformat->Bmask <<= 1;
        vformat->Bmask |= (0x00000001<<vinfo.blue.offset);
    }

    for ( i=0; i<vinfo.transp.length; ++i ) {
        vformat->Amask <<= 1;
        vformat->Amask |= (0x00000001<<vinfo.transp.offset);
    }

    if ( finfo.accel && finfo.mmio_len ) {
        mapped_iolen = finfo.mmio_len;
        mapped_io = mmap(NULL, mapped_iolen, PROT_READ|PROT_WRITE,
                         MAP_SHARED, console_fd, mapped_memlen);
        if ( mapped_io == (Uint8 *)-1 ) {
            /* Hmm, failed to memory map I/O registers */
            mapped_io = NULL;
            fprintf(stderr, "failed to mmap 2d register!\n");
            return -1;
        }
    }

    paccel_info->hw_available = 1;
    paccel_info->video_mem = finfo.smem_len/1024;
#ifdef _USE_2D_ACCEL
    paccel_info->blit_fill = 1;
    paccel_info->blit_hw = 1;
    paccel_info->blit_hw_A = 1;
    paccel_info->blit_hw_CC = 1;
#if 0
    paccel_info->blit_hw_CS = 1;
    paccel_info->blit_hw_rop = 1;
    paccel_info->blit_patternfill =0;
#endif
#ifndef _PC_DEBUG
    TDE_Init(mapped_io+0x300);
#else
    TDE_Init(tde_register_base);
#endif
#endif

    /* We're done! */
    printf ("you are using console frame buffer video driver!\n");
    return(0);
}

static GAL_Rect **HI3510_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect**)(-1);
}


#ifdef FBCON_DEBUG
static void print_vinfo(struct fb_var_screeninfo *vinfo)
{
    fprintf(stderr, "Printing vinfo:\n");
    fprintf(stderr, "txres: %d\n", vinfo->xres);
    fprintf(stderr, "tyres: %d\n", vinfo->yres);
    fprintf(stderr, "txres_virtual: %d\n", vinfo->xres_virtual);
    fprintf(stderr, "tyres_virtual: %d\n", vinfo->yres_virtual);
    fprintf(stderr, "txoffset: %d\n", vinfo->xoffset);
    fprintf(stderr, "tyoffset: %d\n", vinfo->yoffset);
    fprintf(stderr, "tbits_per_pixel: %d\n", vinfo->bits_per_pixel);
    fprintf(stderr, "tgrayscale: %d\n", vinfo->grayscale);
    fprintf(stderr, "tnonstd: %d\n", vinfo->nonstd);
    fprintf(stderr, "tactivate: %d\n", vinfo->activate);
    fprintf(stderr, "theight: %d\n", vinfo->height);
    fprintf(stderr, "twidth: %d\n", vinfo->width);
    fprintf(stderr, "taccel_flags: %d\n", vinfo->accel_flags);
    fprintf(stderr, "tpixclock: %d\n", vinfo->pixclock);
    fprintf(stderr, "tleft_margin: %d\n", vinfo->left_margin);
    fprintf(stderr, "tright_margin: %d\n", vinfo->right_margin);
    fprintf(stderr, "tupper_margin: %d\n", vinfo->upper_margin);
    fprintf(stderr, "tlower_margin: %d\n", vinfo->lower_margin);
    fprintf(stderr, "thsync_len: %d\n", vinfo->hsync_len);
    fprintf(stderr, "tvsync_len: %d\n", vinfo->vsync_len);
    fprintf(stderr, "tsync: %d\n", vinfo->sync);
    fprintf(stderr, "tvmode: %d\n", vinfo->vmode);
    fprintf(stderr, "tred: %d/%d\n", vinfo->red.length, vinfo->red.offset);
    fprintf(stderr, "tgreen: %d/%d\n", vinfo->green.length, vinfo->green.offset);
    fprintf(stderr, "tblue: %d/%d\n", vinfo->blue.length, vinfo->blue.offset);
    fprintf(stderr, "talpha: %d/%d\n", vinfo->transp.length, vinfo->transp.offset);
}

static void print_finfo(struct fb_fix_screeninfo *finfo)
{
    fprintf(stderr, "Printing finfo:\n");
    fprintf(stderr, "tsmem_start = %p\n", (char *)finfo->smem_start);
    fprintf(stderr, "tsmem_len = %d\n", finfo->smem_len);
    fprintf(stderr, "ttype = %d\n", finfo->type);
    fprintf(stderr, "ttype_aux = %d\n", finfo->type_aux);
    fprintf(stderr, "tvisual = %d\n", finfo->visual);
    fprintf(stderr, "txpanstep = %d\n", finfo->xpanstep);
    fprintf(stderr, "typanstep = %d\n", finfo->ypanstep);
    fprintf(stderr, "tywrapstep = %d\n", finfo->ywrapstep);
    fprintf(stderr, "tline_length = %d\n", finfo->line_length);
    fprintf(stderr, "tmmio_start = %p\n", (char *)finfo->mmio_start);
    fprintf(stderr, "tmmio_len = %d\n", finfo->mmio_len);
    fprintf(stderr, "taccel = %d\n", finfo->accel);
}
#endif

static GAL_Surface *HI3510_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct fb_var_screeninfo vinfo;
    int i;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    Sint8 *surfaces_mem;
    int surfaces_len;
    int iAFBufLen = -1;
    
#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF)    
	Uint32 uiVirtualYRes = 0;     
#endif   

    /* Set the video mode and get the final screen format */
    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) 
    {
        fprintf (stderr, "Couldn't get console screen info");
        return(NULL);
    }
    
#ifdef FBCON_DEBUG
    fprintf(stderr, "Printing original vinfo:\n");
    print_vinfo(&vinfo);
#endif

  	if (mgIsServer)
  	{
	    vinfo.activate = FB_ACTIVATE_NOW;
	    vinfo.accel_flags = 0;
	    vinfo.bits_per_pixel = bpp;
	    vinfo.xres = width;
	    vinfo.xres_virtual = width;
	    vinfo.yres = height;
	    vinfo.yres_virtual = height;
	    vinfo.xoffset = 0;
	    vinfo.yoffset = 0;
	    vinfo.red.length = vinfo.red.offset = 0;
	    vinfo.green.length = vinfo.green.offset = 0;
	    vinfo.blue.length = vinfo.blue.offset = 0;
	    vinfo.transp.length = vinfo.transp.offset = 0;

#ifdef FBCON_DEBUG
	    fprintf(stderr, "Printing wanted vinfo:\n");
	    print_vinfo(&vinfo);
#endif
	    if ( ioctl(console_fd, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) 
	    {
	            fprintf (stderr, "Couldn't set console screen info");
	            return(NULL);
	    }
    }

    /* Get the fixed information about the console hardware.
       This is necessary since finfo.line_length changes.
     */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &saved_finfo) < 0) 
    {
        fprintf (stderr, "Couldn't get console hardware info");
        return(NULL);
    }
    

#if (defined(_TAP_ALL) || defined(_TAP3_VERT_HORI) || defined(_TAP2_VERT) \
     || defined(_TAP2_VERT_HORI)|| defined(_TAP3_VERT)) \
     && defined(_USE_2D_ACCEL) && defined(_USE_DBUF)
    if (-1 == (iAFBufLen = HI3510_GetAllAFVMem(this,
    	   saved_finfo.line_length, vinfo.yres, &uiVirtualYRes)))
    {
       fprintf(stderr, "Don't have enough video mem for AntiFlicker\n");
       tap_level = 0;
    }   
    else
    {
        if (mgIsServer)
        {
	        /* Set the video mode and get the final screen format */
	        memset(&vinfo, 0, sizeof(struct fb_var_screeninfo));       
		    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) 
		    {
		        fprintf (stderr, "Couldn't get console screen info");
		        return(NULL);
		    }
		    vinfo.yres_virtual = uiVirtualYRes;
		    
#ifdef FBCON_DEBUG
		    fprintf(stderr, "Printing final vinfo:\n");
		    print_vinfo(&vinfo);
#endif

		    if ( ioctl(console_fd, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) 
	        {
	            fprintf (stderr, "Couldn't set console screen info");
	            return(NULL);
	        }
	    }
        
        /*obligate some video memory for antiflicker*/
	    current->pixels = (void *)((Uint32)mapped_mem 
	                      + (Uint32)mapped_offset + (Uint32)iAFBufLen); 
	    surfaces_mem = (Uint8 *)((Uint32)current->pixels 
	    	            + (Uint32)saved_finfo.line_length * vinfo.yres);
#if 0         
   icount = 0;
            k = 0;
            while(icount < 1000)
            {
                printf("i = %d\n", icount);
                printf("i = %d\n", icount);
               if (k == 0)
               {
                memset(mapped_mem + mapped_offset, 0x55, saved_finfo.line_length*vinfo.yres);
                k = 1;
                printf("res:0\n");
	        HI3510_SetOffset(console_fd, 0, 0);
               }
               else
               {
                memset(mapped_mem + mapped_offset + saved_finfo.line_length*vinfo.yres, 0x55, saved_finfo.line_length*(vinfo.yres - 500));
                memset(mapped_mem + mapped_offset + saved_finfo.line_length*(1*vinfo.yres-500), 0x99, saved_finfo.line_length*400);
                memset(mapped_mem + mapped_offset + saved_finfo.line_length*(1*vinfo.yres-100), 0x55, saved_finfo.line_length*100);
                k = 0;
                printf("res:%d\n", vinfo.yres);
	        HI3510_SetOffset(console_fd, 0, vinfo.yres);
               
               }           
                usleep(50000);
               icount++;
           }
#endif
            
	    if (mgIsServer)
	    {
	        HI3510_SetOffset(console_fd, 0, iAFBufLen/saved_finfo.line_length);
	    }
	}
#endif
     
    
#ifdef FBCON_DEBUG
    fprintf (stderr, "Printing actual vinfo:\n");
    print_vinfo(&vinfo);
#endif
    Rmask = 0;
    for ( i=0; i<vinfo.red.length; ++i ) {
        Rmask <<= 1;
        Rmask |= (0x00000001<<vinfo.red.offset);
    }
    Gmask = 0;
    for ( i=0; i<vinfo.green.length; ++i ) {
        Gmask <<= 1;
        Gmask |= (0x00000001<<vinfo.green.offset);
    }
    Bmask = 0;
    for ( i=0; i<vinfo.blue.length; ++i ) {
        Bmask <<= 1;
        Bmask |= (0x00000001<<vinfo.blue.offset);
    }

    Amask = 0;
    for ( i=0; i<vinfo.transp.length; ++i ) {
        Amask <<= 1;
        Amask |= (0x00000001<<vinfo.transp.offset);
    }

    if (!GAL_ReallocFormat(current, vinfo.bits_per_pixel,
                                      Rmask, Gmask, Bmask, Amask) ) 
    {
        return(NULL);
    }    

    /* Set up the new mode framebuffer */
    //current->flags = (GAL_FULLSCREEN|GAL_HWSURFACE);
    current->w = vinfo.xres;
    current->h = vinfo.yres;
    current->pitch = saved_finfo.line_length;

    if (-1 == iAFBufLen)
    {
        current->pixels = mapped_mem+mapped_offset;

        /* Set up the information for hardware surfaces */
        surfaces_mem = (char *)current->pixels + vinfo.yres * current->pitch;
    }
    
    surfaces_len = mapped_memlen - (surfaces_mem - (Sint8*)mapped_mem);

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
          
        HI3510_FreeHWSurfaces(this, current);
        HI3510_InitHWSurfaces(this, current, surfaces_mem, surfaces_len);

        logosurface.w = BM_WIDTH;
        logosurface.h = BM_HEIGHT;
        logosurface.pitch = 2 * BM_WIDTH;
        this->screen = current;
        HI3510_AllocHWSurface (this, &logosurface);
        FillMemWithData(logosurface.pixels);     
    }
#ifdef _MGRM_PROCESSES
    else 
    {
    	static struct private_hwdata data;
    	data.pDevice = this;
        current->hwdata = &data;
    }
#endif

   current->flags = (GAL_FULLSCREEN|GAL_HWSURFACE);

    /* We're done */
    return(current);
}

#ifdef FBCON_DEBUG
static void HI3510_DumpHWSurfaces(_THIS)
{
    vidmem_bucket *bucket;

    fprintf(stderr, "Memory left: %d (%d total)\n", surfaces_memleft, surfaces_memtotal);
    fprintf(stderr, "\n");
    fprintf(stderr, "         Base  Size\n");
    for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
        fprintf(stderr, "Bucket:  %p, %d (%s)\n", bucket->base, bucket->size, bucket->used ? "used" : "free");
        if ( bucket->prev ) {
            if ( bucket->base != bucket->prev->base+bucket->prev->size ) {
                fprintf(stderr, "Warning, corrupt bucket list! (prev)\n");
            }
        } else {
            if ( bucket != &surfaces ) {
                fprintf(stderr, "Warning, corrupt bucket list! (!prev)\n");
            }
        }
        if ( bucket->next ) {
            if ( bucket->next->base != bucket->base+bucket->size ) {
                fprintf(stderr, "Warning, corrupt bucket list! (next)\n");
            }
        }
    }
    fprintf(stderr, "\n");
}
#endif

static int HI3510_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size)
{
    vidmem_bucket *bucket;

    surfaces_memtotal = size;
    surfaces_memleft = size;

    if ( surfaces_memleft > 0 ) {
        bucket = (vidmem_bucket *)malloc(sizeof(*bucket));
        if ( bucket == NULL ) {
            fprintf (stderr, "Out of memory\n");
            return(-1);
        }
        bucket->prev = &surfaces;
        bucket->used = 0;

        bucket->base = base;
        bucket->size = size;
        bucket->next = NULL;
    } else {
        bucket = NULL;
    }

    surfaces.prev = NULL;
    surfaces.used = 1;

    surfaces.base = screen->pixels;
    surfaces.size = (unsigned int)((long)base - (long)surfaces.base);
    surfaces.next = bucket;
#ifndef _MGRM_THREADS
    screen->hwdata = HI3510_CreateHwData(this, &surfaces);
#else
    screen->hwdata = (struct private_hwdata *)&surfaces;
#endif    
    return(0);
}

static void HI3510_FreeHWSurfaces(_THIS, GAL_Surface *surface)
{
    vidmem_bucket *bucket, *freeable;

    bucket = surfaces.next;
    while ( bucket ) {
        freeable = bucket;
        bucket = bucket->next;
        free(freeable);
    }
    surfaces.next = NULL;
}

static void HI3510_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply)
{
    if (request->bucket == NULL) {     /* alloc hw surface */
        vidmem_bucket *bucket;
        int size;
        int extra;

        reply->bucket = NULL;
        /* Temporarily, we only allow surfaces the same width as display.
           Some blitters require the pitch between two hardware surfaces
           to be the same.  Others have interesting alignment restrictions.
           Until someone who knows these details looks at the code...
        */
        if (request->pitch > this->screen->pitch) {
            fprintf (stderr, "Surface requested wider than screen\n");
            return;
        }

        reply->pitch = this->screen->pitch;
        size = request->h * reply->pitch;
#ifdef FBCON_DEBUG
        fprintf(stderr, "reply->pitch: %d, request->h:%d, Allocating bucket of %d bytes\n", reply->pitch, request->h, size);
#endif

        /* Quick check for available mem */
        if ( size > surfaces_memleft ) {
            fprintf (stderr, "Not enough video memory\n");
            return;
        }

        /* Search for an empty bucket big enough */
        for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
            if ( ! bucket->used && (size <= bucket->size) ) {
                break;
            }
        }
        if ( bucket == NULL ) {
            fprintf (stderr, "Video memory too fragmented\n");
            return;
        }

        /* Create a new bucket for left-over memory */
        extra = (bucket->size - size);
        if ( extra ) {
            vidmem_bucket *newbucket;

#ifdef FBCON_DEBUG
            fprintf(stderr, "Adding new free bucket of %d bytes\n", extra);
#endif
            newbucket = (vidmem_bucket *)malloc(sizeof(*newbucket));
            if ( newbucket == NULL ) {
                fprintf (stderr, "Out of memory\n");
                return;
            }
            newbucket->prev = bucket;
            newbucket->used = 0;
            newbucket->base = bucket->base + size;
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

#ifdef FBCON_DEBUG
        fprintf(stderr, "Allocated %d bytes at %p\n", bucket->size, bucket->base);
#endif
        surfaces_memleft -= size;

        reply->bucket = bucket;
        reply->offset = bucket->base - mapped_mem;
    }
    else {  /* free hw surface */
        vidmem_bucket *bucket, *freeable;

        /* Look for the bucket in the current list */
        for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
            if ( bucket == (vidmem_bucket *)request->bucket) {
                break;
            }
        }
        if ( bucket && bucket->used ) {
        /* Add the memory back to the total */
#ifdef FBCON_DEBUG
            fprintf(stderr, "Freeing bucket of %d bytes\n", bucket->size);
#endif
            surfaces_memleft += bucket->size;

            /* Can we merge the space with surrounding buckets? */
            bucket->used = 0;
            if ( bucket->next && ! bucket->next->used ) {
#ifdef FBCON_DEBUG
                fprintf(stderr, "Merging with next bucket, for %d total bytes\n", bucket->size+bucket->next->size);
#endif
                freeable = bucket->next;
                bucket->size += bucket->next->size;
                bucket->next = bucket->next->next;
                if ( bucket->next ) {
                    bucket->next->prev = bucket;
                }
                free(freeable);
            }
            if ( bucket->prev && ! bucket->prev->used ) {
#ifdef FBCON_DEBUG
                fprintf(stderr, "Merging with previous bucket, for %d total bytes\n", bucket->prev->size+bucket->size);
#endif
                freeable = bucket;
                bucket->prev->size += bucket->size;
                bucket->prev->next = bucket->next;
                if ( bucket->next ) {
                    bucket->next->prev = bucket->prev;
                }
                free(freeable);
            }
        }
    }
}

static int HI3510_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
        
        HI3510_RequestHWSurface (this, &request, &reply);
    else {
        REQUEST req;
        if (surface->w*surface->h == 0)
        {
            goto NEXT;   	
        }
        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));
    }
#else
    HI3510_RequestHWSurface (this, &request, &reply);
#endif

    if (reply.bucket == NULL)
        return -1;
NEXT:
    surface->flags |= GAL_HWSURFACE;
    surface->pitch = reply.pitch;
    surface->pixels = mapped_mem + reply.offset;
#ifndef _MGRM_THREADS
    surface->hwdata = (struct private_hwdata *)HI3510_CreateHwData(this, reply.bucket);
#else
    surface->hwdata = (struct private_hwdata *)reply.bucket;
#endif
    return 0;
}

static void HI3510_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, surface->hwdata};
    REP_HWSURFACE reply = {0, 0};

    request.offset = (char*)surface->pixels - (char*)mapped_mem;

#ifdef _MGRM_PROCESSES
    request.bucket = (void*)((struct private_hwdata*)surface->hwdata)->pBucket;
    if (mgIsServer)
        HI3510_RequestHWSurface (this, &request, &reply);
    else {
        REQUEST req;

        if (surface->hwdata == NULL) {
            surface->pixels = NULL;
            return;
        }

        //request.bucket = surface->hwdata;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));
    }

    free(surface->hwdata);
#else
    HI3510_RequestHWSurface (this, &request, &reply);
#endif

    surface->pixels = NULL;
    surface->hwdata = NULL;
}


static void HI3510_WaitVBL(_THIS)
{
    return;
}

static void HI3510_WaitIdle(_THIS)
{
    return;
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
static void HI3510_VideoQuit(_THIS)
{
#ifdef _MGRM_PROCESSES
    if ( mgIsServer && this->screen ) {
#else
    if ( this->screen ) {
#endif
        /* Clear screen and tell SDL not to free the pixels */
        if ( this->screen->pixels ) {
            memset(this->screen->pixels,0,this->screen->h*this->screen->pitch);
        }
        /* This test fails when using the VGA16 shadow memory */
        if ( ((Uint8*)this->screen->pixels >= mapped_mem) &&
             ((Uint8*)this->screen->pixels < (mapped_mem+mapped_memlen)) ) {
            this->screen->pixels = NULL;
        }
    }

    /* Clear the lock mutex */
#ifdef _MGRM_PROCESSES
    if ( mgIsServer && hw_lock ) {
#else
    if ( hw_lock ) {
#endif
        GAL_DestroyMutex(hw_lock);
        hw_lock = NULL;
    }

    /* Clean up the memory bucket list */
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
        HI3510_FreeHWSurfaces(this, this->screen);
#ifndef _MGRM_THREADS
        HI3510_SetScreenAttr(this, SCREEN_ATTR_ALPHA_CHANNEL, NULL);
        HI3510_SetScreenAttr(this, SCREEN_ATTR_COLORKEY, NULL);
#endif
		HI3510_FreeHWSurface (this, &logosurface);
    }


    /* Close console and input file descriptors */
    if ( console_fd > 0 ) {
        /* Unmap the video framebuffer and I/O registers */
        if ( mapped_mem ) {
            munmap(mapped_mem, mapped_memlen);
            mapped_mem = NULL;
        }
        if ( mapped_io ) {
            munmap(mapped_io, mapped_iolen);
            mapped_io = NULL;
        }

        /* We're all done with the framebuffer */
        close(console_fd);
        console_fd = -1;
    }

}

#ifdef _MGRM_THREADS
void* HI3510_MallocFromVram(int size)
{
    REQ_HWSURFACE request = {1, 0, 1, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};
    Uint8* pBlockHead;
    Uint32 pitch = g_pVideo_3510->screen->pitch;

    if (size == 0)
       return NULL;

    size += sizeof(struct private_hwdata *);
    request.h = (size+pitch-1)/pitch;

    HI3510_RequestHWSurface (current_video, &request, &reply);
    
    if (reply.bucket == NULL)
    {
        fprintf(stderr, "failed to malloc vram, size:%d!\n", size); 
        return NULL;
    }

    pBlockHead = mapped_mem + reply.offset;
    *(struct private_hwdata **)pBlockHead = (struct private_hwdata *)reply.bucket;
    return pBlockHead + sizeof(struct private_hwdata *);
}

void HI3510_FreeToVram(void* p)
{
    REQ_HWSURFACE request = {1, 0, 1, 0, 0};
    REP_HWSURFACE reply = {0, 0};
    fprintf(stderr, "hi3510 free vram\n");
    request.offset = (char*)p - (char*)mapped_mem + sizeof(struct private_hwdata *);
    request.bucket = *(char**)((char*)p - sizeof(struct private_hwdata *));
    request.h = request.offset;

    HI3510_RequestHWSurface (current_video, &request, &reply);
}
#endif

#ifndef _MGRM_THREADS
static struct private_hwdata* HI3510_CreateHwData(_THIS, const vidmem_bucket* pBucket)
{
	struct private_hwdata* pData;
	pData = (struct private_hwdata*)malloc(sizeof(struct private_hwdata));
	if (pData == NULL)
	{
		fprintf(stderr, "failed to create private_hwdata!\n");
		exit(-1);
	}
	else
	{
		pData->pBucket = pBucket;
		pData->pDevice = this;
	}
		
	return pData;
}

int HI3510_ServerSetScreenAttr(Uint8 siAttr, void* pValue)
{

    return HI3510_SetScreenAttr(current_video, siAttr, pValue);
}    


static int HI3510_SetScreenAttr(_THIS, Uint8 siAttr, void* pValue)
{
	int ret = 0;

	switch(siAttr)
	{
		case SCREEN_ATTR_ALPHA_CHANNEL:
		{
    		Uint8 alpha0, alpha1;
    		Uint32 alpha;
			if (pValue != NULL)
			{
				alpha1 = alpha0 = *(Uint8*)pValue* 0x80 / 255 ;
			}
			else
			{
				alpha1 = alpha0 = 0x80;
			}
			
			alpha  = (alpha1 << 8) + alpha0; 
			if (ioctl(console_fd, FBIOPUT_ALPHA_HI3510,  &alpha) <0)
			{
				fprintf(stderr, "failed to set layer alpha attribute!\n");
				ret = -1;
			}
			break;
		}

		case SCREEN_ATTR_COLORKEY:
		{
			fb_colorkey ck = {0};

			if (pValue != NULL)
			{
                ck.key = *(gal_pixel*)pValue;
                ck.key_enable = 1;
			}
			else
			{
                ck.key_enable = 0;
			}

		    if (ioctl(console_fd, FBIOPUT_COLORKEY_HI3510, &ck) < 0)
            {
				fprintf(stderr, "failed to set layer color key attribute!\n");
				ret = -1;
	    	}
		    break;
		}
		
		default:
			fprintf(stderr, "the attribute :%d is not supported!\n", siAttr);
			ret = -1;
	}

   return ret;
}

//pszLayer maybe is "fb0" or "fb1"
static GAL_VideoDevice *HI3510_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));

    if ( this ) {
        memset(this, 0, (sizeof *this));
        this->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *this->hidden));

        this->name = "hi3510";
        this->gamma = (Uint16*)&g_stHidden_3510[0];
    }

    if ( (this == NULL) || (this->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( this ) {
            free(this);
        }
        return(0);
    }

    memset(this->hidden, 0, (sizeof *this->hidden));
    wait_vbl = HI3510_WaitVBL;
    wait_idle = HI3510_WaitIdle;


    /* Set the function pointers */
    this->VideoInit = HI3510_VideoInit;
    this->ListModes = HI3510_ListModes;
    this->SetVideoMode = HI3510_SetVideoMode;
    //this->SetColors = FB_SetColors;
    this->VideoQuit = HI3510_VideoQuit;
    this->RequestHWSurface = HI3510_RequestHWSurface;
    this->AllocHWSurface = HI3510_AllocHWSurface;
#ifdef _USE_2D_ACCEL
#ifdef _USE_DBUF
    this->CheckHWBlit = HI3510_CheckHWBlit;
    this->FillHWRect = HI3510_FillHWRect;
#endif
#endif
    this->FreeHWSurface = HI3510_FreeHWSurface;
    this->free = HI3510_DeleteDevice;
    this->SetHWAlpha = NULL;
    this->SetHWColorKey = NULL;

    return this;
}
#else
static GAL_VideoDevice *HI3510_CreateDevice (int devindex)
{
    return NULL;
}
#endif

VideoBootStrap HI3510_bootstrap = {
    "hi3510", "HI3510 Framebuffer Console",
    HI3510_Available, HI3510_CreateDevice
};

//////////////////////////////////////////////////////////////
//Added by l56244 on 27,July 2006
int HI3510_GetVideoFD(void)
{
	GAL_VideoDevice* this = current_video;
	return console_fd;
}

void* HI3510_GetMM_0(void)
{
	GAL_VideoDevice* this = current_video;
	return mapped_mem;
}

#endif
//Added Ends
