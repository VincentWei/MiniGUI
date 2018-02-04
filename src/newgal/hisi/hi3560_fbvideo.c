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
** $Id: hi3560_fbvideo.c 12360 2009-12-16 09:56:03Z dongkai $
**
** hi3560_fbvideo.c: NewGAL Engine for hi3560
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/page.h>        /* For definition of PAGE_SIZE */
#include <assert.h>

#include "common.h"

#ifdef _MGGAL_HI3560

#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "pixels_c.h"
#include "hi3560_fb.h"
#include "hi3560_fbvideo.h" 
#include "hi_api.h"
#include "api_display.h"
#include "hi_type.h"

//#define DBGLINE() printf("---%s;%s:%d\n",__FILE__,__func__,__LINE__);
#define DBGLINE()  
#define  GAL_ROP            0x00100000 
#define  GAL_DESTCOLORKEY   0x00002000
static char devname[15];

typedef struct taghifb_argb
{
	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
	struct fb_bitfield green;	/* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp;	/* transparency			*/	
}HI_GC_CMAP_S;

#ifndef _MGRM_THREADS
#define REQID_HWSURFACE         0x0013
#endif

//#define FBCON_DEBUG 1
//#define _PC_DEBUG 1  
#define _USE_2D_ACCEL 1

#ifdef _USE_2D_ACCEL
#include "tde_interface.h"
//#include "hi_tde.h" 
#endif

#ifdef _PC_DEBUG
/*hi3560fb extend*/
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

#define FBIOGET_COLORKEY_HI3560       0x4690  /*hi3560 extend*/
#define FBIOPUT_COLORKEY_HI3560       0x4691  /*hi3560 extend*/
#define FBIOGET_ALPHA_HI3560          0x4692  /*hi3560 extend*/
#define FBIOPUT_ALPHA_HI3560          0x4693  /*hi3560 extend*/
#define FBIOPUT_SCREEN_ORIGIN_HI3560  0x4694  /*hi3560 extend*/
#define FBIOGET_SCREEN_ORIGIN_HI3560  0x4695  /*hi3560 extend*/
#endif

//#define     TDE_OFFSET      0x300 /* 3560 */
#define     TDE_OFFSET      0x800   /* 3560 */
typedef enum {
	HI_GC_PF_PALETTE,
	HI_GC_PF_RGB4444,
	HI_GC_PF_RGB1555,
	HI_GC_PF_RGB565,
	HI_GC_PF_RGB8888,
	HI_GC_PF_MAX,
}HI_GC_PIXEL_FMT;


//extern BOOL mgIsServer;
extern GAL_VideoDevice* current_video;

#ifndef _MGRM_THREADS
struct private_hwdata
{
	const vidmem_bucket* pBucket;
    GAL_VideoDevice* pDevice; //该变量用于区分哪个图层，在硬件加速blit中用到
};

static int HI3560_SetLayerAlpha(int  fd,  Uint32* pAlpha);
static int HI3560_SetLayerAttr(_THIS, Uint8 siAttr, void * pValue);
static struct private_hwdata* HI3560_CreateHwData(_THIS, const vidmem_bucket* pBucket);
#endif

/* Initialization/Query functions */
static int HI3560_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **HI3560_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *HI3560_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static void HI3560_VideoQuit(_THIS);

/* Hardware surface functions */
static int HI3560_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size);
static void HI3560_FreeHWSurfaces(_THIS);
static void HI3560_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
static int HI3560_AllocHWSurface(_THIS, GAL_Surface *surface);
static void HI3560_FreeHWSurface(_THIS, GAL_Surface *surface);
static void HI3560_WaitVBL(_THIS);
static void HI3560_WaitIdle(_THIS);

static int hi3560_console = 0;
static int g_AntiFlickerLevel =HI3560_AF_TDE_HVDF;
static int  s_TDEAbandFlag =0;

static HWDISPLAY_ATTR_S hw_display_attr;

/*void hwDisplaySetting( HWDISPLAY_ATTR_S *hwdisplay_attr)
{
    hw_display_attr.enable = hwdisplay_attr->enable;
    hw_display_attr.gpc_layer_enable[2] = hwdisplay_attr->gpc_layer_enable[2];
    hw_display_attr.display_mode = hwdisplay_attr->display_mode;
    hw_display_attr.lcd_config.HACT = hwdisplay_attr->lcd_config.HACT;
    hw_display_attr.lcd_config.VBB = hwdisplay_attr->lcd_config.VBB;
    hw_display_attr.lcd_config.VFB = hwdisplay_attr->lcd_config.VFB;
    hw_display_attr.lcd_config.HBB = hwdisplay_attr->lcd_config.HBB;
    hw_display_attr.lcd_config.HFB = hwdisplay_attr->lcd_config.HFB;
    hw_display_attr.lcd_config.VPW = hwdisplay_attr->lcd_config.VPW;
    hw_display_attr.lcd_config.HPW = hwdisplay_attr->lcd_config.HPW;
    hw_display_attr.lcd_config.ClockReversal=hwdisplay_attr->lcd_config.ClockReversal;
    hw_display_attr.lcd_config.DataWidth = hwdisplay_attr->lcd_config.DataWidth;
    hw_display_attr.lcd_config.ItfFormat = hwdisplay_attr->lcd_config.ItfFormat;
    hw_display_attr.lcd_config.DitherEnable = hw_display_attr->lcd_config.DitherEnable;
    hw_display_attr.lcd_config.IDV = hwdisplay_attr->lcd_config.IDV;
    hw_display_attr.lcd_config.IHS = hwdisplay_attr->lcd_config.IHS;
    hw_display_attr.lcd_config.IVS = hwdisplay_attr->lcd_config.IVS;
    strncpy(devname,hwdisplay_attr->dev_name,sizeof(hwdisplay_attr->dev_name));
}
*/
static void HI_DEVICE_OPEN(void)
{
    DISPLAY_ATTR_S l_display_attr;
#ifdef _MGRM_PROCESSES
    if(mgIsServer)
#endif
    {
        if(HI_API_DISPLAY_Open() != HI_RET_SUCCESS){
            printf("HI_API_DISPLAY_Open error\n");
            return ;
        }

        if(HI_API_DISPLAY_GetAttr (&l_display_attr) != HI_RET_SUCCESS){
            printf("HI_API_DISPLAY_GetArrt error\n");
            return ;
        }
        /*Sansung LCD screen configure*/
        l_display_attr.enable = hw_display_attr.enable ;
        l_display_attr.gpc_layer_enable[2] = hw_display_attr.gpc_layer_enable[2];
        l_display_attr.display_mode = hw_display_attr.display_mode;
        l_display_attr.lcd_config.HACT = hw_display_attr.lcd_config.HACT;
        l_display_attr.lcd_config.VBB = hw_display_attr.lcd_config.VBB;   //7;   //10;
        l_display_attr.lcd_config.VFB = hw_display_attr.lcd_config.VFB;   //10;
        l_display_attr.lcd_config.HBB = hw_display_attr.lcd_config.HBB;   //160;
        l_display_attr.lcd_config.HFB = hw_display_attr.lcd_config.HFB;    //10;
        l_display_attr.lcd_config.VPW = hw_display_attr.lcd_config.VPW;   //10;
        l_display_attr.lcd_config.HPW = hw_display_attr.lcd_config.HPW;   //10;
        l_display_attr.lcd_config.ClockReversal = hw_display_attr.lcd_config.ClockReversal;   //10;
        l_display_attr.lcd_config.DataWidth = hw_display_attr.lcd_config.DataWidth;   //10;
        l_display_attr.lcd_config.ItfFormat = hw_display_attr.lcd_config.ItfFormat;   //10;
        l_display_attr.lcd_config.DitherEnable = hw_display_attr.lcd_config.DitherEnable;   //10;
        l_display_attr.lcd_config.IDV = hw_display_attr.lcd_config.IDV;   //10;
        l_display_attr.lcd_config.IHS = hw_display_attr.lcd_config.IHS;   //10;
        l_display_attr.lcd_config.IVS = hw_display_attr.lcd_config.IVS;   //10;

#if 0
        /*others LCD screen configure*/
        l_display_attr.enable = HI_TRUE ;
        l_display_attr.gpc_layer_enable[2] = HI_FALSE;
        l_display_attr.display_mode = DISPLAY_MODE_DIGITAL;
        l_display_attr.lcd_config.HACT =800;
        l_display_attr.lcd_config.VBB =23;   //10;
        l_display_attr.lcd_config.VFB =4;   //10;
        l_display_attr.lcd_config.HBB =46;   //160;
        l_display_attr.lcd_config.HFB =8;    //10;
        l_display_attr.lcd_config.VPW =2;   //10;
        l_display_attr.lcd_config.HPW =3;   //10;
        l_display_attr.lcd_config.ClockReversal =HI_FALSE;   //10;
        l_display_attr.lcd_config.DataWidth =VOU_DATA_WIDTH24;   //10;
        l_display_attr.lcd_config.ItfFormat =VOU_FORMAT_RGB888;   //10;
        l_display_attr.lcd_config.DitherEnable =HI_FALSE;   //10;
        l_display_attr.lcd_config.IDV =HI_FALSE;   //10;
        l_display_attr.lcd_config.IHS =HI_FALSE;   //10;
        l_display_attr.lcd_config.IVS =HI_FALSE;   //10;
#endif

        if(HI_API_DISPLAY_SetAttr (&l_display_attr) != HI_RET_SUCCESS){
            printf("HI_API_DISPLAY_SetAttr error\n");
            return ;
        }
   }
}

static void HI_DEVICE_CLOSE(void)
{
#ifdef _USE_2D_ACCEL
        if(HI_API_TDE_Close() !=HI_SUCCESS){
            printf("HI_API_TDE_Close error\n");
            return ;
        }
#endif
 
#ifdef _MGRM_PROCESSES
    if(mgIsServer)
#endif
    {
        HI_API_DISPLAY_Close();
    }
}

static int HI3560_Available(void)
{
    const char *GAL_fbdev;

    hwDisplaySetting(&hw_display_attr);
    strncpy(devname, hw_display_attr.dev_name, sizeof(hw_display_attr.dev_name));
    
    GAL_fbdev = getenv("FRAMEBUFFER");
    if ( GAL_fbdev == NULL ) {
        //strncpy(GAL_fbdev,devname,sizeof(devname));
        GAL_fbdev = devname;
    }

    HI_DEVICE_OPEN();

    hi3560_console = open(GAL_fbdev, O_RDWR, 0);

    if ( hi3560_console >= 0 ) {
        close(hi3560_console);
    }
    else {
    	fprintf(stderr, "NEWGAL>HI3560: failed to open file :%s!\n", GAL_fbdev);
        return 0;
    }

    return(hi3560_console >= 0);
}

#ifdef _MGRM_THREADS
struct GAL_PrivateVideoData g_stHidden_3560 = { 0 };
#else
static struct GAL_PrivateVideoData g_stHidden_3560[2]; //two layers
#endif

#ifdef _MGRM_THREADS
struct GAL_VideoDevice *g_pVideo_3560 = NULL;
#endif

GAL_Surface logosurface;

#include "pix_array.c"

#ifdef _USE_2D_ACCEL
//#ifdef _USE_DBUF

#define HI3560_ALIGN_4(x) (((x)+3)&~3)

//static  int HI3560_GetColorFmt(const GAL_PixelFormat* pixel_fmt, TDE_COLOR_FORMAT* fmt)
static  int HI3560_GetColorFmt(const GAL_PixelFormat* pixel_fmt, TDE_COLOR_FMT_E* fmt)
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
           	   *fmt = TDE_COLOR_FMT_RGB444;
           	   ret = 0;
           }
           break;
           
       case 16 :
           if (Rmask == 0x0f00 && (Gmask == 0x00f0) && Bmask == 0x000f && Amask == 0xf000)
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);
               *fmt = TDE_COLOR_FMT_RGB4444;
               ret = 0;
           }
           else if (Rmask == 0x7c00 && (Gmask == 0x03e0) && Bmask == 0x001f && Amask == 0x0000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);
               *fmt = TDE_COLOR_FMT_RGB555;
               ret = 0;
           }
           else if (Rmask == 0x7c00 && (Gmask == 0x03e0) && Bmask == 0x001f && Amask == 0x8000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);           	
               *fmt = TDE_COLOR_FMT_RGB1555;
               ret = 0;
           }
           else if (Rmask == 0xf800 && (Gmask == 0x07e0) && Bmask == 0x001f && Amask == 0x0000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffff);           	
               *fmt = TDE_COLOR_FMT_RGB565;
               ret = 0;
           }
           break;
       case 24 :
           if (Rmask == 0xff0000 && (Gmask == 0x00ff0) && Bmask == 0x0000ff && Amask == 0x000000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffffff);           	
               *fmt = TDE_COLOR_FMT_RGB888;
               ret = 0;
           }
           break;
       case 32 :
           if (Rmask == 0xff0000 && (Gmask == 0x00ff00) && Bmask == 0x0000ff && Amask == 0xff000000)	
           {
           	   assert(Rmask+Amask+Gmask+Bmask == 0xffffffff);           	
               *fmt = TDE_COLOR_FMT_RGB8888;
               ret = 0;
           }
           break;
           
       default:
           ret = -1;
    }

	return ret;
}
	
#define HI3560_GET_PHY(virtual_addr) ((U8*)(virtual_addr-mapped_mem + saved_finfo.smem_start))
#define HI3560_CHECK_VIRTUAL_ADDR(virtual_addr)  assert(virtual_addr-mapped_mem < saved_finfo.smem_len)

static void HI3560_GetRGB(gal_pixel color, GAL_PixelFormat* fmt, Uint8* r, Uint8* g, Uint8 *b)
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


static void Hi3560_ColorFmtConvert(TDE_COLOR_FMT_E encolorfmt, HI_U32 u32color,
        TDE_RGB_S *pstrutdeRGB)
{
    switch(encolorfmt)
    {
        case TDE_COLOR_FMT_RGB888:
        case TDE_COLOR_FMT_RGB8888:
            pstrutdeRGB->u8R = (u32color >>16) & 0xff;
            pstrutdeRGB->u8G = (u32color >>8) & 0xff;
            pstrutdeRGB->u8B = u32color & 0xff;
            break;
        case TDE_COLOR_FMT_RGB555:
        case TDE_COLOR_FMT_RGB1555:
            pstrutdeRGB->u8R = (((u32color >>10) & 0x1f) <<3);
            pstrutdeRGB->u8G = (((u32color >>5) & 0x1f) <<3);
            pstrutdeRGB->u8B = ((u32color & 0x1f) <<3);

            pstrutdeRGB->u8R += (pstrutdeRGB->u8R >>5);
            pstrutdeRGB->u8G += (pstrutdeRGB->u8G >>5);
            pstrutdeRGB->u8B += (pstrutdeRGB->u8B >>5);
            break;
        case TDE_COLOR_FMT_RGB444:
        case TDE_COLOR_FMT_RGB4444:
            pstrutdeRGB->u8R = (((u32color >>8) & 0xf) <<4);
            pstrutdeRGB->u8G = (((u32color >>4) & 0xf) <<4);
            pstrutdeRGB->u8B = ((u32color & 0xf) <<4);

            pstrutdeRGB->u8R += (pstrutdeRGB->u8R >>4);
            pstrutdeRGB->u8G += (pstrutdeRGB->u8G >>4);
            pstrutdeRGB->u8B += (pstrutdeRGB->u8B >>4);
            break;
        case TDE_COLOR_FMT_RGB565:
            pstrutdeRGB->u8R = (((u32color >>11) & 0x1f) <<3);
            pstrutdeRGB->u8G = (((u32color >>5) & 0x3f) <<2);
            pstrutdeRGB->u8B = ((u32color & 0x1f) <<3);

            pstrutdeRGB->u8R += (pstrutdeRGB->u8R >>5);
            pstrutdeRGB->u8G += (pstrutdeRGB->u8G >>6);
            pstrutdeRGB->u8B += (pstrutdeRGB->u8B >>5);
            break;
        default:
            break;
    }
}


static TDE_ROP_CODE_E s_ropmap[4] ={TDE_ROP_PS, TDE_ROP_PSDa, TDE_ROP_PSDo, TDE_ROP_PSDx};
static HI_S32 Hi3560_InitParam(GAL_Surface *psurface, TDE_OPT_S *param)
{
    TDE_COLOR_FMT_E ensrccolorfmt;
    HI_U32 flags = psurface->flags;

    memset(param,0,sizeof(TDE_OPT_S));

    if(flags & GAL_ROP){
        //assert(psurface->format->rop !=0);
        param->enDataConv = TDE_DATA_CONV_ROP;
        //param->enRopCode = s_ropmap[psurface->format->rop];
        param->enRopCode = TDE_ROP_PS;
    }
    else if(flags & GAL_SRCALPHA){
        param->enDataConv = TDE_DATA_CONV_ALPHA_SRC;
        param->stAlpha.bAlphaGlobal = HI_TRUE;
        param->stAlpha.u8AlphaGlobal = psurface->format->alpha;
        param->stAlpha.enOutAlphaFrom = TDE_OUTALPHA_FROM_DST;
    }
    else{
        param->enDataConv =TDE_DATA_CONV_NONE;
        param->stAlpha.enOutAlphaFrom = TDE_OUTALPHA_FROM_DST;
        param->stAlpha.u8AlphaGlobal = 0xff;
        param->stAlpha.u8Alpha0 = 0xff;
        param->stAlpha.u8Alpha1 = 0xff;
    }

    if((flags & GAL_SRCCOLORKEY) || (flags & GAL_DESTCOLORKEY)){
        //HI_U8 r,g,b;

        param->stColorSpace.bColorSpace = HI_TRUE;
        if(flags & GAL_DESTCOLORKEY){
            param->stColorSpace.enColorSpaceTarget = TDE_COLORSPACE_DST;
        }
        else{
            param->stColorSpace.enColorSpaceTarget = TDE_COLORSPACE_SRC;
        }

        HI3560_GetColorFmt(psurface->format, &ensrccolorfmt);
        /*Hi3560_ColorFmtConvert(ensrccolorfmt, psurface->format->colorkey.low,
                &param->stColorSpace.stColorMin);
        Hi3560_ColorFmtConvert(ensrccolorfmt, psurface->format->colorkey.high,
                &param->stColorSpace.stColorMax);*/
        Hi3560_ColorFmtConvert(ensrccolorfmt, psurface->format->colorkey,
                &param->stColorSpace.stColorMin);
        Hi3560_ColorFmtConvert(ensrccolorfmt, psurface->format->colorkey,
                &param->stColorSpace.stColorMax);
    }
    else{
        param->stColorSpace.bColorSpace =HI_FALSE;
    }

    return 0;
}



HI_U8* Hi3560_GetPhyAddr(HI_VOID* pvirtaddr)
{
    HI_U32 offset;

    GAL_VideoDevice *this =current_video;
    offset =(HI_U32)pvirtaddr - (HI_U32)mapped_mem;

    return (HI_U8 *)saved_finfo.smem_start + offset;
}


HI_S32 Hi3560_InitTdeSurface(TDE_SURFACE_S *ptdesurface, const GAL_Surface *dst,
                             const GAL_Rect *rect)
{
    memset(ptdesurface,0,sizeof(TDE_SURFACE_S));

    if(HI3560_GetColorFmt(dst->format,&ptdesurface->enColorFmt) != 0){
        return -1;
    }

    ptdesurface->pu8PhyAddr =Hi3560_GetPhyAddr(dst->pixels) +rect->x * 
        dst->format->BytesPerPixel + rect->y * dst->pitch;
    ptdesurface->u16Height =rect->h;
    ptdesurface->u16Width =rect->w;
    ptdesurface->u16Stride =dst->pitch;
    ptdesurface->u16Reserved =0;

    return 0;
}


static int HI3560_HWAccelBlit(GAL_Surface *gal_src, GAL_Rect *psrcrect,
                       GAL_Surface *gal_dst, GAL_Rect *pdstrect)
{
    //TDE_COLOR_FMT_E ensrccolorfmt;
    TDE_SURFACE_S srcsurface,*psrcsurface;
    TDE_SURFACE_S dstsurface,*pdstsurface;
    //TDE_COLOR_FMT_E color_fmt;
    TDE_OPT_S param;
    HI_S32  ret;
    GAL_Rect srcrect,dstrect;

    GAL_VideoDevice* this = current_video;
    TDE_HANDLE tdehandle;

    psrcsurface =&srcsurface;
    pdstsurface =&dstsurface;

#ifdef _MGRM_PROCESSES
    if(mgIsServer && (gal_src->flags & GAL_DOUBLEBUF) 
            && (gal_src->flags & GAL_HWSURFACE) && 
            (gal_dst == this->screen)){
#else
        if((gal_src->flags & GAL_DOUBLEBUF) 
            && (gal_src->flags & GAL_HWSURFACE) && 
            (gal_dst == this->screen)){
#endif
        srcrect.x =0;
        srcrect.y =0;
        srcrect.w = this->screen->w;
        srcrect.h = this->screen->h;
        dstrect = srcrect;
        assert(pdstrect->w == this->screen->w && 
                pdstrect->h == this->screen->h);
    }
    else{
        srcrect = *psrcrect;
        dstrect = *pdstrect;
    }

    if(Hi3560_InitTdeSurface(psrcsurface,gal_src,&srcrect) <0 || 
            Hi3560_InitTdeSurface(pdstsurface,gal_dst,&dstrect) <0){
        return -1;
    }

    if(Hi3560_InitParam(gal_src,&param) <0){
        return -1;
    }

    tdehandle = HI_API_TDE_BeginJob();
    if(tdehandle == HI_ERR_TDE_INVALID_HANDLE || 
       tdehandle == HI_ERR_TDE_DEV_NOT_OPEN){
        printf("----HI_API_TDE_BeginJob error\n");
        return -1;
    }

#ifdef _MGRM_PROCESSES
    if(mgIsServer && (gal_src->flags & GAL_DOUBLEBUF) &&
            (gal_src->flags & GAL_HWSURFACE) && 
            (gal_src == this->screen)){
#else
    if((gal_src->flags & GAL_DOUBLEBUF) &&
            (gal_src->flags & GAL_HWSURFACE) && 
            (gal_src == this->screen)){   
#endif
        TDE_DEFLICKER_COEF_S deflickerparam={0};
        HI_U8 alpha =0x80;
        deflickerparam.pu8HDfCoef =&alpha;
        deflickerparam.pu8VDfCoef =&alpha;

        switch(g_AntiFlickerLevel)
        {
            case HI3560_AF_TDE_HVDF:
            case HI3560_AF_TDE:
            case HI3560_AF_TDE_VDF:
                deflickerparam.u32HDfLevel =0;
                deflickerparam.u32VDfLevel =2;
                ret = HI_API_TDE_Deflicker(tdehandle,psrcsurface,pdstsurface,
                        deflickerparam);

                return HI_API_TDE_EndJob(tdehandle,HI_TRUE,100000);
            default:
                break;
        }
    }

    ret =HI_API_TDE_BitBlt(tdehandle,psrcsurface,pdstsurface,&param);

    return HI_API_TDE_EndJob(tdehandle,HI_TRUE,100000);
}


static int HI3560_CheckHWBlit(_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    TDE_COLOR_FMT_E color_fmt;

    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    //only supported the hw surface accelerated.
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE))
    {
    	return -1;
    }

    if ((src->flags&GAL_DESTCOLORKEY)) {
    	fprintf(stderr, "NEWGAL>HI3560: src->flags:%x,unsupported GAL_DESTCOLORKEY\n", src->flags);
        return -1;
    }
    
    //if ((src->flags&GAL_SRCALPHA) && (src->flags&GAL_ROP) && (src->format->rop != ROP_SET))
    if ((src->flags & GAL_SRCALPHA) && (src->flags & GAL_ROP)) {
    	fprintf(stderr, "NEWGAL>HI3560: unsupported GAL_SRCALPHA|GAL_ROP\n");
        return -1;	
    }

    if(HI3560_GetColorFmt(src->format, &color_fmt) < 0)
    {
        return -1;
    }
    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = HI3560_HWAccelBlit;
    return 0;

}

static Sint32 Hi3560_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    TDE_SURFACE_S surface ={0};
    TDE_OPT_S param;
    TDE_HANDLE tdehandle;
    TDE_RGB_S tdecolor ={0};
    //HI_U8 r,g,b;

    if(s_TDEAbandFlag){
        return -1;
    }
    assert(!(dst->pitch &3));

    if(Hi3560_InitTdeSurface(&surface, dst, rect) <0){
        return -1;
    }

    if(Hi3560_InitParam(dst, &param) != 0){
        return -1;
    }

    Hi3560_ColorFmtConvert(surface.enColorFmt,color,&tdecolor);
    param.enDataConv = TDE_DATA_CONV_NONE;
    param.stAlpha.enOutAlphaFrom = TDE_OUTALPHA_FROM_REG;
    param.stAlpha.u8AlphaGlobal = 0xff;
    tdehandle = HI_API_TDE_BeginJob();
    if(tdehandle == HI_ERR_TDE_INVALID_HANDLE || 
       tdehandle == HI_ERR_TDE_DEV_NOT_OPEN){
        printf("----%s,HI_API_TDE_BeginJob error\n");
        return -1;
    }
    HI_API_TDE_SolidDraw(tdehandle, &surface, tdecolor,&param);

    return HI_API_TDE_EndJob(tdehandle,HI_TRUE,100000);
}


#if 0
 /* Fills a surface rectangle with the given color */
int HI3560_FillHWRect(_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
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
    if (HI3560_GetColorFmt(dst->format, &color_fmt) < 0)
    {
        fprintf(stderr, "error: file:%s, line:%d\n", __FILE__, __LINE__);
    	return -1;
    }
    
    param.InColorFormat = color_fmt;
    param.OutColorFormat = color_fmt;

    param.opt_width = (U16)(rect->w);
    param.opt_height = (U16)(rect->h);
    param.out_stride = (U16)HI3560_ALIGN_4(dst->pitch);
    pvirtual_addr = (Uint8*)dst->pixels + rect->x*dst->format->BytesPerPixel+ rect->y*dst->pitch;
    HI3560_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.out_pixels = HI3560_GET_PHY(pvirtual_addr);

    if(HI_SUCCESS != TDE_SolidDraw(&param))
    {
        fprintf(stderr,"error: file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }

    	
    printSDParam(&param);
	return 0;
}
#endif


/* Fills a surface rectangle with the given bitmap */
#if 0
int HI3560_FillHWRectWithPattern(_THIS, GAL_Surface	*dst, GAL_Rect *rect, PBITMAP pattern)
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
    if (HI3560_GetColorFmt(dst->format, &color_fmt) < 0)
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
    param.out_stride = (U16)HI3560_ALIGN_4(dst->pitch);
    pvirtual_addr = (Uint8*)dst->pixels + rect->x*dst->format->BytesPerPixel+ rect->y*dst->pitch;
    HI3560_CHECK_VIRTUAL_ADDR(pvirtual_addr);
    param.out_pixels = HI3560_GET_PHY(pvirtual_addr);
    param.InColorFormat = param.OutColorFormat = color_fmt;
    param.pattern_width= pattern->bmWidth;
    param.pattern_height = pattern->bmHeight;
    
    //memmove(pattern_addr, pattern->bmBits, pattern_size);
    param.src_pixels = HI3560_GET_PHY(pattern->bmBits);
    param.src_stride = HI3560_ALIGN_4(pattern->bmPitch);
    if(HI_SUCCESS != TDE_PatternFill(&param))
    {
        fprintf(stderr, "error,file:%s, line:%d\n", __FILE__, __LINE__);
        return -1;
    }
    printPFParam(&param);
	return 0;
}
#endif

//#endif
#endif
static void HI3560_DeleteDevice(GAL_VideoDevice *device)
{
}

#ifdef _MGRM_THREADS
BOOL HI3560_CreateDevice(GAL_VideoDevice *device)
{
    int console;
    const char *GAL_fbdev;
    GAL_VideoDevice *this = device;

    //hwDisplaySetting(&hw_display_attr);
    if (device == NULL)
    {
        return FALSE;
    }

//  GAL_fbdev = "/dev/fb/0";
    //strncpy(GAL_fbdev,devname,sizeof(devname));
    GAL_fbdev = devname;
    console_fd = open(GAL_fbdev, O_RDWR, 0);
    if ( console_fd < 0 ) 
    {
        return FALSE;
    }
    
    memset(this, 0, (sizeof *this));
 
    wait_vbl = HI3560_WaitVBL;
    wait_idle = HI3560_WaitIdle;

    this->name = "fbcon";

    /* Set the function pointers */
    this->VideoInit = HI3560_VideoInit;
    this->ListModes = HI3560_ListModes;
    this->SetVideoMode = HI3560_SetVideoMode;
    this->VideoQuit = HI3560_VideoQuit;
    this->AllocHWSurface = HI3560_AllocHWSurface;
#ifdef _USE_2D_ACCEL
#ifdef _USE_DBUF
    this->CheckHWBlit = HI3560_CheckHWBlit;
    this->FillHWRect = Hi3560_FillHWRect;
    this->FillHWRectWithPattern = NULL;/*the pattern fill is slower than do by software,so do not use it.*/
#endif
#endif
    this->FreeHWSurface = HI3560_FreeHWSurface;
    this->free = HI3560_DeleteDevice;
    g_pVideo_3560 = this;

    return TRUE;
}
#endif

static int HI3560_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    GAL_VideoInfo* paccel_info =  &this->info;
    int i;
#ifdef _PC_DEBUG
    static unsigned char tde_register_base[0x100];
#endif
    console_fd = hi3560_console;
#ifdef _USE_2D_ACCEL
        if(HI_API_TDE_Open() !=HI_SUCCESS){
            printf("HI_API_TDE_Open error\n");
            return ;
        }
#endif
 
    /* Get the type of video hardware */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) 
    {
        fprintf (stderr, "NEWGAL>HI3560: Couldn't get console hardware info\n");
        HI3560_VideoQuit(this);
        return(-1);
    }

    /* Memory map the device, compensating for buggy PPC mmap() */
    mapped_offset = (((long)finfo.smem_start) -
                    (((long)finfo.smem_start)&~(PAGE_SIZE-1)));
    mapped_memlen = finfo.smem_len+mapped_offset;
    mapped_mem = mmap(NULL, mapped_memlen,
                      PROT_READ|PROT_WRITE, MAP_SHARED, console_fd, 0);

    if ( mapped_mem == (Uint8 *)-1 ) {
        fprintf (stderr, "NEWGAL>HI3560: Unable to memory map the video hardware\n");
        mapped_mem = NULL;
        HI3560_VideoQuit(this);
        return(-1);
    }

    /* Determine the current screen depth */
    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
        fprintf (stderr, "NEWGAL>HI3560: Couldn't get console pixel format\n");
        HI3560_VideoQuit(this);
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
            fprintf(stderr, "NEWGAL>HI3560: failed to mmap 2d register!\n");
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
//#ifndef _PC_DEBUG
//    TDE_Init(mapped_io+TDE_OFFSET);
//#else
//    TDE_Init(tde_register_base);
//#endif
#endif

    /* We're done! */
    return(0);
}

static GAL_Rect **HI3560_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
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

//#ifdef FBCON_DEBUG
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

static const HI_GC_CMAP_S s_fbargb[] = 
{
#if 0	
    {
        .red    = {8, 4, 0}, //RGB444
        .green  = {4, 4, 0},
        .blue   = {0, 4, 0},
        .transp = {0, 0, 0},
    },
#endif    
    {
        .red    = {8, 4, 0}, //RGB4444
        .green  = {4, 4, 0},
        .blue   = {0, 4, 0},
        .transp = {12, 4, 0},
    },
#if 0
	{
        .red    = {10, 5, 0}, //RGB555
        .green  = {5, 5, 0},
        .blue   = {0, 5, 0},
        .transp = {0, 0, 0},
    },
 #endif   
    {
        .red    = {10, 5, 0},//RGB1555
        .green  = {5, 5, 0},
        .blue   = {0, 5, 0},
        .transp = {15, 1, 0},
    },

    {
        .red    = {11, 5, 0},//RGB565
        .green  = {5, 6, 0},
        .blue   = {0, 5, 0},
        .transp = {0, 0, 0},
    },
    
#if 0
	{
        .red    = {16, 8, 0},//RGB888
        .green  = {8, 8, 0},
        .blue   = {0, 8, 0},
        .transp = {0, 0, 0},
    },
 #endif   
    {
        .red    = {16, 8, 0},//RGB8888
        .green  = {8, 8, 0},
        .blue   = {0, 8, 0},
        .transp = {24, 8, 0},
    }
};
static GAL_Surface *HI3560_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct fb_var_screeninfo vinfo;
    int i;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    Uint8 *surfaces_mem;
    int surfaces_len;
    int iAFBufLen = -1;
    
    /* Set the video mode and get the final screen format */
    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) 
    {
        fprintf (stderr, "NEWGAL>HI3560: Couldn't get console screen info");
        return(NULL);
    }
    
#ifdef FBCON_DEBUG
    fprintf(stderr, "Printing original vinfo:\n");
    print_vinfo(&vinfo);
#endif

#ifdef _MGRM_PROCESSES
  	if (mgIsServer)
#endif
  	{
	    vinfo.activate = FB_ACTIVATE_NOW;
	    vinfo.accel_flags = 0;
	    vinfo.bits_per_pixel = bpp;
	    vinfo.xres = width;
	    vinfo.xres_virtual = width;
	    //vinfo.xres_virtual = 800;
	    vinfo.yres = height;
	    vinfo.yres_virtual = height;
	    //vinfo.yres_virtual = 480;
	    vinfo.xoffset = 0;
	    vinfo.yoffset = 0;
#if 0	    
	    vinfo.red.length = vinfo.red.offset = 0;
	    vinfo.green.length = vinfo.green.offset = 0;
	    vinfo.blue.length = vinfo.blue.offset = 0;
	    vinfo.transp.length = vinfo.transp.offset = 0;
#endif
        if(bpp == 16)
        {
            vinfo.red  = s_fbargb[HI_GC_PF_RGB565-1].red;
		    vinfo.blue = s_fbargb[HI_GC_PF_RGB565-1].blue;
		    vinfo.green = s_fbargb[HI_GC_PF_RGB565-1].green;		
		    vinfo.transp = s_fbargb[HI_GC_PF_RGB565-1].transp;
        }
        else if(bpp == 32)
        {
		    vinfo.red  = s_fbargb[HI_GC_PF_RGB8888-1].red;
		    vinfo.blue = s_fbargb[HI_GC_PF_RGB8888-1].blue;
		    vinfo.green = s_fbargb[HI_GC_PF_RGB8888-1].green;		
		    vinfo.transp = s_fbargb[HI_GC_PF_RGB8888-1].transp;
        }
#ifdef FBCON_DEBUG
	    fprintf(stderr, "Printing wanted vinfo:\n");
	    print_vinfo(&vinfo);
#endif
	    if ( ioctl(console_fd, FBIOPUT_VSCREENINFO, &vinfo) < 0 ) 
	    {
	            fprintf (stderr, "NEWGAL>HI3560: Couldn't set console screen info");
	            return(NULL);
	    }

#ifdef FBCON_DEBUG
        if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) 
        {
            fprintf (stderr, "NEWGAL>HI3560: Couldn't get console screen info");
            return(NULL);
        }   
	    print_vinfo(&vinfo);
#endif
    }

    /* Get the fixed information about the console hardware.
       This is necessary since finfo.line_length changes.
     */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &saved_finfo) < 0) 
    {
        fprintf (stderr, "NEWGAL>HI3560: Couldn't get console hardware info");
        return(NULL);
    }
    
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
    
    surfaces_len = (mapped_memlen -(surfaces_mem-mapped_mem));    

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
        DBGLINE();
        HI3560_FreeHWSurfaces(this);
        HI3560_InitHWSurfaces(this, current, surfaces_mem, surfaces_len);

        /*logosurface.w = BM_WIDTH;
        logosurface.h = BM_HEIGHT;
        logosurface.pitch = 2 * BM_WIDTH;
        this->screen = current;
        HI3560_AllocHWSurface (this, &logosurface);
        FillMemWithData(logosurface.pixels);     */
        DBGLINE();
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
static void HI3560_DumpHWSurfaces(_THIS)
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

static int HI3560_InitHWSurfaces(_THIS, GAL_Surface *screen, char *base, int size)
{
    vidmem_bucket *bucket;

    surfaces_memtotal = size;
    surfaces_memleft = size;

    if ( surfaces_memleft > 0 ) {
        bucket = (vidmem_bucket *)malloc(sizeof(*bucket));
        if ( bucket == NULL ) {
            fprintf (stderr, "NEWGAL>HI3560: Out of memory\n");
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
    screen->hwdata = HI3560_CreateHwData(this, &surfaces);
#else
    screen->hwdata = (struct private_hwdata *)&surfaces;
#endif    
    return(0);
}

static void HI3560_FreeHWSurfaces(_THIS)
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

static void HI3560_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply)
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

        //reply->pitch = this->screen->pitch;
        reply->pitch = (request->pitch + 3) & ~3;
        size = request->h * reply->pitch;
#ifdef FBCON_DEBUG
        fprintf(stderr, "reply->pitch: %d, request->h:%d, Allocating bucket of %d bytes\n", reply->pitch, request->h, size);
#endif

        /* Quick check for available mem */
        if ( size > surfaces_memleft ) {
            fprintf (stderr, "NEWGAL>HI3560: Not enough video memory\n");
            return;
        }

        /* Search for an empty bucket big enough */
        for ( bucket=&surfaces; bucket; bucket=bucket->next ) {
            if ( ! bucket->used && (size <= bucket->size) ) {
                break;
            }
        }
        if ( bucket == NULL ) {
            fprintf (stderr, "NEWGAL>HI3560: Video memory too fragmented\n");
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
                fprintf (stderr, "NEWGAL>HI3560: Out of memory\n");
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

static int HI3560_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
        
        HI3560_RequestHWSurface (this, &request, &reply);
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
    HI3560_RequestHWSurface (this, &request, &reply);
#endif

    if (reply.bucket == NULL)
        return -1;
NEXT:
    surface->flags |= GAL_HWSURFACE;
    surface->pitch = reply.pitch;
    surface->pixels = mapped_mem + reply.offset;
#ifndef _MGRM_THREADS
    surface->hwdata = (struct private_hwdata *)HI3560_CreateHwData(this, reply.bucket);
#else
    surface->hwdata = (struct private_hwdata *)reply.bucket;
#endif
    return 0;
}

static void HI3560_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, surface->hwdata};
    REP_HWSURFACE reply = {0, 0};

    request.offset = (char*)surface->pixels - (char*)mapped_mem;

#ifdef _MGRM_PROCESSES
    request.bucket = ((struct private_hwdata*)surface->hwdata)->pBucket;
    if (mgIsServer)
        HI3560_RequestHWSurface (this, &request, &reply);
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
    HI3560_RequestHWSurface (this, &request, &reply);
#endif

    surface->pixels = NULL;
    surface->hwdata = NULL;
}

static void HI3560_WaitVBL(_THIS)
{
    return;
}

static void HI3560_WaitIdle(_THIS)
{
    return;
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/

static void HI3560_VideoQuit(_THIS)
{
    DBGLINE();
#ifdef _MGRM_PROCESSES
    if ( mgIsServer && this->screen ) {
#else
    if ( this->screen ) {
#endif
    DBGLINE();
        /* Clear screen and tell SDL not to free the pixels */
        if ( this->screen->pixels ) {
            memset(this->screen->pixels,0,this->screen->h*this->screen->pitch);
        }
        /* This test fails when using the VGA16 shadow memory */
        if ( ((Uint8*)this->screen->pixels >= mapped_mem) &&
             ((Uint8*)this->screen->pixels < (mapped_mem+mapped_memlen)) ) {
            this->screen->pixels = NULL;
        }
    DBGLINE();
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

    DBGLINE();
    /* Clean up the memory bucket list */
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
    DBGLINE();
        HI3560_FreeHWSurfaces(this);
#ifndef _MGRM_THREADS
        HI3560_SetLayerAttr(this, SCREEN_ATTR_ALPHA_CHANNEL, NULL);
        HI3560_SetLayerAttr(this, SCREEN_ATTR_COLORKEY, NULL);
    DBGLINE();
#endif
    }

    DBGLINE();
    //HI3560_FreeHWSurface (this, &logosurface);

    DBGLINE();
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

#if 1
    DBGLINE();
    HI_DEVICE_CLOSE();
    DBGLINE();
#endif
}

#ifdef _MGRM_THREADS
void* HI3560_MallocFromVram(int size)
{
    REQ_HWSURFACE request = {1, 0, 1, 0, NULL};
    REP_HWSURFACE reply = {0, 0, NULL};
    Uint8* pBlockHead;
    Uint32 pitch = g_pVideo_3560->screen->pitch;

    if (size == 0)
       return NULL;

    size += sizeof(struct private_hwdata *);
    request.h = (size+pitch-1)/pitch;

    HI3560_RequestHWSurface (current_video, &request, &reply);
    
    if (reply.bucket == NULL)
    {
        fprintf(stderr, "NEWGAL>HI3560: failed to malloc vram, size:%d!\n", size); 
        return NULL;
    }

    pBlockHead = mapped_mem + reply.offset;
    *(struct private_hwdata **)pBlockHead = (struct private_hwdata *)reply.bucket;
    return pBlockHead + sizeof(struct private_hwdata *);
}

void HI3560_FreeToVram(void* p)
{
    REQ_HWSURFACE request = {1, 0, 1, 0, 0};
    REP_HWSURFACE reply = {0, 0};
    request.offset = (char*)p - (char*)mapped_mem + sizeof(struct private_hwdata *);
    request.bucket = *(char**)((char*)p - sizeof(struct private_hwdata *));
    request.h = request.offset;

    HI3560_RequestHWSurface (current_video, &request, &reply);
}
#endif

#ifndef _MGRM_THREADS
static struct private_hwdata* HI3560_CreateHwData(_THIS, const vidmem_bucket* pBucket)
{
	struct private_hwdata* pData;
	pData = (struct private_hwdata*)malloc(sizeof(struct private_hwdata));
	if (pData == NULL)
	{
		fprintf(stderr, "NEWGAL>HI3560: failed to create private_hwdata!\n");
		exit(-1);
	}
	else
	{
		pData->pBucket = pBucket;
		pData->pDevice = this;
	}
		
	return pData;
}

static int HI3560_SetLayerAlpha(int  fd,  Uint32* pAlpha)
{
    fb_alpha Alpha = {0};

    if (pAlpha == NULL) {
        return 0;
    }
    else {
        Alpha.bAlphaEnable = HI_TRUE;
        Alpha.bAlphaChannel = HI_FALSE;
        Alpha.u8Alpha0 = (HI_U8)(*pAlpha);
        Alpha.u8Alpha1 = (HI_U8)(*pAlpha);
    }

    if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &Alpha) <0) {
        fprintf(stderr, "NEWGAL>HI3560: failed to set layer alpha attribute!\n");
        return -1;
    }

    return 0;
}

static int HI3560_SetLayerAttr(_THIS, Uint8 siAttr, void * pValue)
{
	int ret = 0;

	switch(siAttr) {
		case SCREEN_ATTR_ALPHA_CHANNEL:
		{
#if 0			
    		Uint8 alpha0, alpha1;
    		Uint32 alpha;
			if (pValue != NULL)
			{
				alpha1 = alpha0 = *(Uint8*)pValue;
			}
			else
			{
				alpha1 = alpha0 = 0xff;
			}
			
			alpha  = (alpha1 << 8) + alpha0; 
			if (ioctl(console_fd, FBIOPUT_ALPHA_HIFB,  &alpha) <0)
			{
				fprintf(stderr, "failed to set layer alpha attribute!\n");
				ret = -1;
			}
#endif
            ret = HI3560_SetLayerAlpha(console_fd, (Uint32*)pValue);
			break;
		}

		case SCREEN_ATTR_COLORKEY:
        {
            fb_colorkey ck = {0};

            if (pValue != NULL) {
                ck.u32Key = *(HI_U32*)pValue;
                ck.bKeyEnable = HI_TRUE;
                ck.bMaskEnable = HI_FALSE;
            }
            else {
                ck.bKeyEnable = HI_FALSE;
                ck.bMaskEnable = HI_FALSE;
            }

            if (ioctl(console_fd, FBIOPUT_COLORKEY_HIFB, &ck) < 0) {
                fprintf(stderr, "NEWGAL>HI3560: failed to set layer color key attribute!\n");
                ret = -1;
            }
            break;
        }
		
		default:
			fprintf(stderr, "NEWGAL>HI3560: the attribute :%d is not supported!\n", siAttr);
			ret = -1;
	}

   return ret;
}

/*pszLayer maybe is "fb0" or "fb1"*/
static GAL_VideoDevice *HI3560_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;
    const char *GAL_fbdev;

    //hwDisplaySetting(&hw_display_attr);
    strncpy(devname, hw_display_attr.dev_name, sizeof(hw_display_attr.dev_name));

    if(devindex ==0){
    //    GAL_fbdev = "/dev/fb/0";
        //strncpy(GAL_fbdev,devname,sizeof(devname));
        GAL_fbdev = devname;
    }
    else if(devindex ==1){
        GAL_fbdev = "/dev/fb/1";
    }

    hi3560_console = open(GAL_fbdev, O_RDWR, 0);
    if(hi3560_console <0){
        fprintf(stderr, "NEWGAL>HI3560: failed to open file :%s!\n", GAL_fbdev);
        return 0;
    }

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    memset(this, 0, (sizeof *this));

    if ( this ) {
        memset(this, 0, (sizeof *this));
        this->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *this->hidden));

        this->name = "hi3560";
        if(devindex ==0)
            this->gamma = (Uint16*)&g_stHidden_3560[0];
        else if(devindex ==1)
            this->gamma = (Uint16*)&g_stHidden_3560[1];
    }

    if ( (this == NULL) || (this->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( this ) {
            free(this);
        }
        return(0);
    }

    memset(this->hidden, 0, (sizeof *this->hidden));
    wait_vbl = HI3560_WaitVBL;
    wait_idle = HI3560_WaitIdle;


    /* Set the function pointers */
    this->VideoInit = HI3560_VideoInit;
    this->ListModes = HI3560_ListModes;
    this->SetVideoMode = HI3560_SetVideoMode;
    this->VideoQuit = HI3560_VideoQuit;
#ifdef _MGRM_PROCESSES
    this->RequestHWSurface = HI3560_RequestHWSurface;
#endif
    this->AllocHWSurface = HI3560_AllocHWSurface;
#ifdef _USE_2D_ACCEL
//#ifdef _USE_DBUF
    this->CheckHWBlit = HI3560_CheckHWBlit;
    this->FillHWRect = Hi3560_FillHWRect;
    //this->FillHWRectWithPattern = NULL;//HI3560_FillHWRectWithPattern;
//#endif
#endif
    this->FreeHWSurface = HI3560_FreeHWSurface;
    this->SetHWAlpha = NULL;
    this->SetHWColorKey = NULL;
    this->free = HI3560_DeleteDevice;
    //this->SetAttr   = HI3560_SetLayerAttr;

    return this;
}
#endif

VideoBootStrap HI3560_bootstrap = {
    "hi3560", "HI3560 Framebuffer Console",
    HI3560_Available, HI3560_CreateDevice
};

/* some APIs specific to this engine */
int hi3560GetVideoFD (void)
{
	GAL_VideoDevice* this = current_video;
	return console_fd;
}

void* hi3560GetFBAddress (void)
{
	GAL_VideoDevice* this = current_video;
	return mapped_mem;
}

int hi3560SetScreenAttr (Uint8 siAttr, void* pValue)
{
    return HI3560_SetLayerAttr (current_video, siAttr, pValue);
}    

#endif /* _MGGAL_HI3560 */

