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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"

#ifdef _MGGAL_S3C6410

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef _MGRM_PROCESSES
#   include "client.h"
#endif
#include "gal-s3c6410.h"

#define S3C6410VID_DRIVER_NAME "s3c6410"

#ifdef _MGRM_PROCESSES
#   define ISSERVER (mgIsServer)
#else
#   define ISSERVER (1)
#endif

int s3c6410_size_table[48] = {
	290, /* speed: 4775 */
	370, /* speed: 4688 */
	220, /* speed: 5125 */
	240, /* speed: 3925 */
	210, /* speed: 3856 */
	170, /* speed: 3968 */
	170, /* speed: 3461 */
	140, /* speed: 3861 */
	130, /* speed: 3616 */
	120, /* speed: 3413 */
	110, /* speed: 3530 */
	100, /* speed: 3584 */
	90, /* speed: 3530 */
	80, /* speed: 3664 */
	90, /* speed: 3285 */
	80, /* speed: 3552 */
	80, /* speed: 3184 */
	80, /* speed: 3088 */
	70, /* speed: 3306 */
	70, /* speed: 3200 */
	70, /* speed: 3061 */
	60, /* speed: 3280 */
	60, /* speed: 3189 */
	50, /* speed: 3733 */
	70, /* speed: 2698 */
	50, /* speed: 3296 */
	40, /* speed: 3813 */
	50, /* speed: 3194 */
	50, /* speed: 3082 */
	70, /* speed: 2330 */
	40, /* speed: 3477 */
	30, /* speed: 4458 */
	50, /* speed: 2810 */
	30, /* speed: 3957 */
	40, /* speed: 3168 */
	50, /* speed: 2656 */
	40, /* speed: 3061 */
	30, /* speed: 3658 */
	30, /* speed: 3610 */
	40, /* speed: 3061 */
	30, /* speed: ? */
	30, /* speed: ? */
	30, /* speed: ? */
	30, /* speed: ? */
	20, /* speed: ? */
	20, /* speed: ? */
	20, /* speed: ? */
	20, /* speed: ? */
};

/* Initialization/Query functions */
static int S3C6410_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **S3C6410_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *S3C6410_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int S3C6410_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void S3C6410_VideoQuit(_THIS);

/* Hardware surface functions */
#ifdef _MGRM_PROCESSES
static void S3C6410_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
#endif
static int S3C6410_AllocHWSurface(_THIS, GAL_Surface *surface);
static void S3C6410_FreeHWSurface(_THIS, GAL_Surface *surface);
static int S3C6410_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst);
static int S3C6410_HWAccelBlit(GAL_Surface * src, GAL_Rect * srcrect, GAL_Surface * dst, GAL_Rect * dstrect);

/* S3C6410 driver bootstrap functions */

static int S3C6410_Available(void)
{
    return(1);
}

static void S3C6410_DeleteDevice(GAL_VideoDevice *device)
{
    if (ISSERVER) {
        gal_vmbucket_destroy(&device->hidden->vmbucket);
    }
    free(device->hidden);
    free(device);
}

static GAL_VideoDevice *S3C6410_CreateDevice(int devindex)
{
    GAL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if ( device ) {
        memset(device, 0, (sizeof *device));
        device->hidden = (struct GAL_PrivateVideoData *)
                malloc((sizeof *device->hidden));
    }
    if ( (device == NULL) || (device->hidden == NULL) ) {
        GAL_OutOfMemory();
        if ( device ) {
            free(device);
        }
        return(0);
    }
    memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = S3C6410_VideoInit;
    device->ListModes = S3C6410_ListModes;
    device->SetVideoMode = S3C6410_SetVideoMode;
    device->SetColors = S3C6410_SetColors;
    device->VideoQuit = S3C6410_VideoQuit;
#ifdef _MGRM_PROCESSES
    device->RequestHWSurface = S3C6410_RequestHWSurface;
#endif
    device->AllocHWSurface = S3C6410_AllocHWSurface;
    device->CheckHWBlit = S3C6410_CheckHWBlit;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->FreeHWSurface = S3C6410_FreeHWSurface;

    device->free = S3C6410_DeleteDevice;

    return device;
}

VideoBootStrap S3C6410_bootstrap = {
    S3C6410VID_DRIVER_NAME, "S3C6410 video driver",
    S3C6410_Available, S3C6410_CreateDevice
};

static int S3C6410_VideoInit(_THIS, GAL_PixelFormat *vformat)
{
    struct GAL_PrivateVideoData *hidden;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    int fd_fb = -1;
    int fd_g2d = -1;
    unsigned char *pixels = MAP_FAILED;

    fprintf (stderr, "NEWGAL>S3C6410: Calling init method!\n");

    fd_fb = open("/dev/fb0", O_RDWR);
    if (fd_fb < 0) {
        perror("open()");
        goto err;
    }

    fd_g2d = open("/dev/s3c-g2d", O_RDWR);
    if (fd_g2d < 0) {
        perror("open()");
        goto err;
    }

    if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo) < 0) {
        perror("ioctl(FBIOGET_FSCREENINFO)");
        goto err;
    }
    assert(finfo.smem_start && finfo.smem_len);

    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl(FBIOGET_VSCREENINFO)");
        goto err;
    }

    pixels = (unsigned char *) mmap(NULL, finfo.smem_len,
            PROT_READ|PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if (pixels == MAP_FAILED) {
        perror("mmap()");
        goto err;
    }

    /* Determine the screen depth (use default 8-bit depth) */
    /* we change this during the GAL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 8;
    vformat->BytesPerPixel = 1;

    this->hidden = (struct GAL_PrivateVideoData *) calloc(1, sizeof(struct GAL_PrivateVideoData));
    hidden = this->hidden;
    hidden->fd_fb = fd_fb;
    hidden->fd_g2d = fd_g2d;
    hidden->smem_start = finfo.smem_start;
    hidden->smem_len = finfo.smem_len;
    hidden->pitch = finfo.line_length;
    hidden->pixels = pixels;
    hidden->width = vinfo.width;
    hidden->height = vinfo.height;
    if (ISSERVER) {
        memset (hidden->pixels, 0, hidden->smem_len);
        gal_vmbucket_init(&hidden->vmbucket, hidden->pixels, hidden->smem_len);
    }

    {
        GAL_VideoInfo *video_info = &this->info;
        video_info->blit_fill = 0;
        video_info->blit_hw = 1;
        video_info->blit_hw_CC = 0;
        video_info->blit_hw_A  = 1;
    }

    return(0);

err:
    assert(0);
    if (pixels != MAP_FAILED) {
        munmap(pixels, finfo.smem_len);
    }
    if (fd_fb >= 0) {
        close(fd_fb);
    }
    if (fd_g2d >= 0) {
        close(fd_g2d);
    }
    return -1;
}

static GAL_Rect **S3C6410_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel != 16) {
        /* support bpp==16-bit only */
        return NULL;
    }else{
        /* any size is ok */
        return (GAL_Rect**) -1;
    }
}

static GAL_Surface *S3C6410_SetVideoMode(_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    struct GAL_PrivateVideoData *hidden = this->hidden;

    /* Allocate the new pixel format for the screen */
    if (!GAL_ReallocFormat (current, bpp, 0xF800, 0x07E0, 0x001F, 0)) {
        fprintf (stderr, "NEWGAL>S3C6410: "
                "Couldn't allocate new pixel format for requested mode\n");
        return(NULL);
    }

    /* Set up the new mode framebuffer */
    current->w = width;
    current->h = height;
    current->pitch = hidden->pitch;

    if (ISSERVER) {
        S3C6410_AllocHWSurface(this, current);
    } else {
        current->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        current->hwdata->vmblock = NULL;
        current->hwdata->addr_phy = hidden->smem_start + 0;
        current->hwdata->fd_g2d = hidden->fd_g2d;

        current->pixels = hidden->pixels + 0;
        current->pitch = this->hidden->pitch;
        current->flags |= GAL_HWSURFACE;
    }

    return current;
}

static int
S3C6410_CheckHWBlit(_THIS, GAL_Surface * src, GAL_Surface * dst)
{
    src->flags &= ~GAL_HWACCEL;
    src->map->hw_blit = NULL;

    // only supported the hw surface accelerated.
    if (!(src->flags & GAL_HWSURFACE) || !(dst->flags & GAL_HWSURFACE))
    {
        printf("src(%s) dst(%s)\n", 
                (src->flags & GAL_HWSURFACE) ? "HW" : "SW",
                (dst->flags & GAL_HWSURFACE) ? "HW" : "SW");
        return -1;
    }

    src->flags |= GAL_HWACCEL;
    src->map->hw_blit = S3C6410_HWAccelBlit;
    return 0;
}

static unsigned int get_colormode(GAL_Surface *surface) {
    GAL_PixelFormat *format = surface->format;
    switch (format->BytesPerPixel) {
        case 2:
            if (1
                    && format->Amask == 0x0000
                    && format->Rmask == 0xF800
                    && format->Gmask == 0x07E0
                    && format->Bmask == 0x001F) {
                return S3C_G2D_COLOR_RGB_565;
            }else if (1
                    && format->Rmask == 0x7C00
                    && format->Gmask == 0x03E0
                    && format->Bmask == 0x001F
                    ) {
                if ((surface->flags & GAL_SRCPIXELALPHA) && format->Amask == 0x8000){
                    return S3C_G2D_COLOR_ARGB_1555;
                }else{
                    break;
                }
            }else if (1
                    && format->Rmask == 0xF800
                    && format->Gmask == 0x07C0
                    && format->Bmask == 0x003E){
                if ((surface->flags & GAL_SRCPIXELALPHA) && format->Amask == 0x0001) {
                    return S3C_G2D_COLOR_RGBA_5551;
                }else{
                    break;
                }
            }else{
                break;
            }
        case 4:
            if (1
                    && format->Rmask == 0x00FF0000
                    && format->Gmask == 0x0000FF00
                    && format->Bmask == 0x000000FF){
                if ((surface->flags & GAL_SRCPIXELALPHA) && format->Amask == 0xFF000000) {
                    return S3C_G2D_COLOR_ARGB_8888;
                }else{
                    return S3C_G2D_COLOR_XRGB_8888;
                }
            }else if (1
                    && format->Rmask == 0xFF000000
                    && format->Gmask == 0x00FF0000
                    && format->Bmask == 0x0000FF00){
                if ((surface->flags & GAL_SRCPIXELALPHA) && format->Amask == 0x000000FF) {
                    return S3C_G2D_COLOR_RGBA_8888;
                }else{
                    return S3C_G2D_COLOR_RGBX_8888;
                }
            }else{
                break;
            }
        default:
            break;
    }
    fprintf(stderr, "Format(Bpp:%d A:%08x R:%08x G:%08x B:%08x) not supported\n",
            format->BytesPerPixel, format->Amask, format->Rmask, format->Gmask, format->Bmask);
    assert(0);
    return (unsigned int)-1;
}

static int setup_param(GAL_Surface *src, GAL_Surface *dst, s3c_g2d_params *param) 
{
    param->src_base_addr = src->hwdata->addr_phy; //Base address of the source image	
    param->src_full_width = src->w; //source screen full width
    param->src_full_height = src->h; //source screen full heiht
    param->src_start_x = 0;			//coordinate start x of source image
    param->src_start_y = 0;			//coordinate start y of source image
    param->src_work_width = 0;			//source image width for work
    param->src_work_height = 0;		//source image height for work
    param->src_colormode = get_colormode(src);

    param->dst_base_addr = dst->hwdata->addr_phy;			//Base address of the destination image	
    param->dst_full_width = dst->w;			//destination screen full width
    param->dst_full_height = dst->h;		//destination screen full height
    param->dst_start_x = 0;			//coordinate start x of destination screen
    param->dst_start_y = 0;			//coordinate start y of destination screen
    param->dst_work_width = 0;			//destination screen width for work
    param->dst_work_height = 0;		//destination screen height for work
    param->dst_colormode = get_colormode(dst);

    // cos
    if (param->dst_colormode == S3C_G2D_COLOR_ARGB_8888) {
        param->dst_colormode = S3C_G2D_COLOR_XRGB_8888;
    }else if (param->dst_colormode == S3C_G2D_COLOR_RGBA_8888) {
        param->dst_colormode = S3C_G2D_COLOR_RGBX_8888;
    }

    /* XXX: clip window */
    param->cw_x1 = 0;
    param->cw_y1 = 0;
    param->cw_x2 = dst->w;
    param->cw_y2 = dst->h;

    param->color_val[G2D_BLACK] = 0;
    param->color_val[G2D_WHITE] = 0xffffffff;
    param->color_val[G2D_BLUE] = 0x77777777; // TODO:

    param->alpha_mode = 0;
    param->alpha_val = 0;

    param->color_key_mode = 0;			//true : enable, false : disable
    param->color_key_val = 0;			//transparent color value

    if ((src->flags & GAL_SRCPIXELALPHA)) {
        param->alpha_mode |= S3C_G2D_ROP_REG_ABM_SRC_BITMAP;
        //param->alpha_mode |= S3C_G2D_ROP_REG_ABM_NO_BLENDING;
    }
    if ((src->flags & GAL_SRCALPHA)) {
        param->alpha_mode |= S3C_G2D_ROP_REG_ABM_REGISTER;
        param->alpha_val = src->format->alpha;
    }

    if (param->src_colormode == (unsigned int)-1 || param->dst_colormode == (unsigned int)-1) {
        return -1;
    }else{
        return 0;
    }
}

static int
S3C6410_HWAccelBlit(GAL_Surface *src, GAL_Rect *srcrect, GAL_Surface *dst, GAL_Rect *dstrect)
{
    int ret;
    s3c_g2d_params param;
    // TODO: alpha, alpha channel, colorkey

    if (dstrect->w == 0 || dstrect->h == 0) {
        return -1;
    }

    if (setup_param(src, dst, &param) < 0) {
        return -1;
    }
    param.src_start_x = srcrect->x;
    param.src_start_y = srcrect->y;
    param.src_work_width = srcrect->w;
    param.src_work_height = srcrect->h;
    
    param.dst_start_x = dstrect->x;
    param.dst_start_y = dstrect->y;
    param.dst_work_width = dstrect->w;
    param.dst_work_height = dstrect->h;

    /* only for driver's bug */
    param.src_work_width--;
    param.src_work_height--;
    param.dst_work_width--;
    param.dst_work_height--;

    clock_t start, end;
    static int n;
    static int t0;
    start = times(NULL);
    if (n++ == 0) {
        t0 = start;
    }
    // printf("%d x %d = %d\n", srcrect->w, srcrect->h, srcrect->w * srcrect->h);
    ret = ioctl (src->hwdata->fd_g2d, S3C_G2D_ROTATOR_0, &param);
    end = times(NULL);
    if (end - start >= 2) {
        printf("[%d] %ld %ld ret=%d\n", n, end-t0, end-start, ret);
        n = 0;
    }
    return ret;
}

#ifdef _MGRM_PROCESSES
static void S3C6410_RequestHWSurface (_THIS, const REQ_HWSURFACE* request, REP_HWSURFACE* reply) {
    gal_vmblock_t *block;

    if (request->bucket == NULL) { /* alloc */
        block = gal_vmbucket_alloc(&this->hidden->vmbucket, request->pitch, request->h);
        if (block) {
            reply->offset = block->offset;
            reply->pitch = block->pitch;
            reply->bucket = block;
        }else{
            reply->bucket = NULL;
        }
    }else{ /* free */
        if (request->offset != -1) {
            gal_vmbucket_free(&this->hidden->vmbucket, (gal_vmblock_t *)request->bucket);
        }
    }
}
#endif

/* We don't actually allow hardware surfaces other than the main one */
static int S3C6410_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    int offset, pitch;
    struct GAL_PrivateVideoData *hidden = this->hidden;
    gal_vmblock_t *block;

    if (surface->w !=1 || surface->h != 1) { /* 1X1 is a reference DC */
        int H = surface->h / 10;
        if (H >= sizeof(s3c6410_size_table)/sizeof(s3c6410_size_table[0])) {
            H = sizeof(s3c6410_size_table)/sizeof(s3c6410_size_table[0]) - 1;
        }
        if (surface->w/10 < s3c6410_size_table[H]) {
            surface->flags &= ~GAL_HWSURFACE;
            return -1;
        }
    }

    if (ISSERVER) {
        /* force 4-byte align */
        if (surface->format->BytesPerPixel != 4) {
            surface->w = (surface->w + 1) & ~1;
            surface->pitch = surface->w * surface->format->BytesPerPixel;
        }

        block = gal_vmbucket_alloc(&hidden->vmbucket, surface->pitch, surface->h);
        if (block) {
            offset = block->offset;
            pitch = block->pitch;
        }
#ifdef _MGRM_PROCESSES
    }else{ // for client
        REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, 0, NULL};
        REP_HWSURFACE reply = {0, 0, NULL};

        REQUEST req;

        req.id = REQID_HWSURFACE;
        req.data = &request;
        req.len_data = sizeof (REQ_HWSURFACE);

        ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));

        block = (gal_vmblock_t *)reply.bucket;
        offset = reply.offset;
        pitch = reply.pitch;
#endif
    }

    if (block) {
        surface->hwdata = (struct private_hwdata *)calloc(1, sizeof(struct private_hwdata));
        surface->hwdata->vmblock = block;
        surface->hwdata->addr_phy = hidden->smem_start + offset;
        surface->hwdata->fd_g2d = hidden->fd_g2d;

        surface->pixels = hidden->pixels + offset;
        surface->pitch = pitch;
        memset (surface->pixels, 0, surface->pitch * surface->h);
        surface->flags |= GAL_HWSURFACE;
        return 0;
    }else{
        surface->flags &= ~GAL_HWSURFACE;
        return -1;
    }
}

static void S3C6410_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    struct GAL_PrivateVideoData *hidden = this->hidden;
    struct private_hwdata *hwdata = surface->hwdata;

    if (hwdata->vmblock) {
        if (ISSERVER) {
            gal_vmbucket_free(&hidden->vmbucket, hwdata->vmblock);
#ifdef _MGRM_PROCESSES
        }else{
            REQ_HWSURFACE request = {surface->w, surface->h, surface->pitch, -1, surface->hwdata->vmblock};
            REP_HWSURFACE reply = {0, 0};

            REQUEST req;

            req.id = REQID_HWSURFACE;
            req.data = &request;
            req.len_data = sizeof (REQ_HWSURFACE);

            ClientRequest (&req, &reply, sizeof (REP_HWSURFACE));
#endif
        }
    }

    free(surface->hwdata);
    surface->hwdata = NULL;
    surface->pixels = NULL;
}

static int S3C6410_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another video routine -- notably UpdateRects.
*/
static void S3C6410_VideoQuit(_THIS)
{
    struct GAL_PrivateVideoData *data = this->hidden;
    memset(data->pixels, 0, data->smem_len);
    munmap(data->pixels, data->smem_len);
    close(data->fd_g2d);
    close(data->fd_fb);
}

#endif /* _MGGAL_S3C6410 */
