///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "newgal.h"
#include "sysvideo.h"
#include "blit.h"
#include "RLEaccel_c.h"
#include "pixels_c.h"
#include "memops.h"
#include "leaks.h"

#ifdef _MGRM_PROCESSES
#include <sys/mman.h>   /* for munmap */
#endif  /* _MGRM_PROCESSES */

/* Public routines */
/*
 * Create an empty RGB surface of the appropriate depth
 */
GAL_Surface * GAL_CreateRGBSurface (Uint32 flags,
            int width, int height, int depth,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_VideoDevice *video = __mg_current_video;
    GAL_VideoDevice *this  = __mg_current_video;
    GAL_Surface *screen;
    GAL_Surface *surface;

    /* Check to see if we desire the surface in video memory */
    if (video) {
        screen = GAL_PublicSurface;
    } else {
        screen = NULL;
    }
    if (screen && ((screen->flags&GAL_HWSURFACE) == GAL_HWSURFACE)) {
        if ((flags&(GAL_SRCCOLORKEY|GAL_SRCALPHA)) != 0) {
            flags |= GAL_HWSURFACE;
        }
        if ((flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
            if (! __mg_current_video->info.blit_hw_CC) {
                flags &= ~GAL_HWSURFACE;
            }
        }
        if ((flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
            if (! __mg_current_video->info.blit_hw_A) {
                flags &= ~GAL_HWSURFACE;
            }
        }
    } else {
        flags &= ~GAL_HWSURFACE;
    }

    /* Allocate the surface */
    surface = (GAL_Surface *)malloc(sizeof(*surface));
    if (surface == NULL) {
        GAL_OutOfMemory();
        return(NULL);
    }
    if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE)
        surface->video = __mg_current_video;
    else
        surface->video = NULL;

    surface->flags = GAL_SWSURFACE;

    /* 2011-03-04 WanZheng,
     * make it able to create a hardware surface
     * with different color format against the screen surface
     */
#if 0
    if ((flags & GAL_HWSURFACE) == GAL_HWSURFACE) {
        depth = screen->format->BitsPerPixel;
        Rmask = screen->format->Rmask;
        Gmask = screen->format->Gmask;
        Bmask = screen->format->Bmask;
        Amask = screen->format->Amask;
    }
#endif
    if (Amask){
        surface->flags |= GAL_SRCPIXELALPHA;
    }
    surface->format = GAL_AllocFormat(depth, Rmask, Gmask, Bmask, Amask);
    if (surface->format == NULL) {
        free (surface);
        return(NULL);
    }
    surface->w = width;
    surface->h = height;
    surface->pitch = GAL_CalculatePitch(surface);
    surface->pixels = NULL;
    surface->pixels_off = 0;
    // for off-screen surface, DPI always be the default value
    surface->dpi = GDCAP_DPI_DEFAULT;
    surface->hwdata = NULL;
    surface->map = NULL;
    surface->format_version = 0;
#ifdef _MGUSE_PIXMAN
    surface->pix_img = NULL;
    surface->msk_img = NULL;
#endif
#if IS_COMPOSITING_SCHEMA
    surface->shared_header = NULL;
    surface->dirty_info = NULL;
#endif
    GAL_SetClipRect(surface, NULL);

#ifdef _MGUSE_UPDATE_REGION
    /* Initialize update region */
    InitClipRgn (&surface->update_region, &__mg_free_update_region_list);
#endif

    /* Get the pixels */
    if (surface->w && surface->h) {
        if (((flags&GAL_HWSURFACE) == GAL_SWSURFACE) ||
                (video->AllocHWSurface(this, surface) < 0)) {
            surface->pixels = malloc(surface->h*surface->pitch);
            if (surface->pixels == NULL) {
                GAL_FreeSurface(surface);
                GAL_OutOfMemory();
                return(NULL);
            }
            /* This is important for bitmaps */
            memset(surface->pixels, 0, surface->h*surface->pitch);
            surface->flags &= ~GAL_HWSURFACE;
        }
    }
    else {
        /* set a static buff to pixels for empty surface */
        static char buff[8];
        surface->flags |= GAL_PREALLOC;
        surface->pixels = buff;
    }

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if (surface->map == NULL) {
        GAL_FreeSurface(surface);
        return(NULL);
    }

    /* The surface is ready to go */
    surface->refcount = 1;
#ifdef CHECK_LEAKS
    ++surfaces_allocated;
#endif
    return(surface);
}
/*
 * Create an RGB surface from an existing memory buffer
 */
GAL_Surface * GAL_CreateRGBSurfaceFrom (void *pixels,
            int width, int height, int depth, int pitch,
            Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface *surface;

    surface = GAL_CreateRGBSurface(GAL_SWSURFACE, 0, 0, depth,
                                   Rmask, Gmask, Bmask, Amask);
    if (surface != NULL) {
        surface->flags |= GAL_PREALLOC;
        surface->pixels = pixels;
        surface->w = width;
        surface->h = height;
        surface->pitch = pitch;
        GAL_SetClipRect(surface, NULL);
    }
    return(surface);
}
/*
 * Set the color key in a blittable surface
 */
int GAL_SetColorKey (GAL_Surface *surface, Uint32 flag, Uint32 key)
{
    /* Sanity check the flag as it gets passed in */
    if (flag & GAL_SRCCOLORKEY) {
        if (flag & (GAL_RLEACCEL|GAL_RLEACCELOK)) {
            flag = (GAL_SRCCOLORKEY | GAL_RLEACCELOK);
        } else {
            flag = GAL_SRCCOLORKEY;
        }
    } else {
        flag = 0;
    }

    /* Optimize away operations that don't change anything */
    if ((flag == (surface->flags & (GAL_SRCCOLORKEY|GAL_RLEACCELOK))) &&
         (key == surface->format->colorkey)) {
        return(0);
    }

    /* UnRLE surfaces before we change the colorkey */
    if (surface->flags & GAL_RLEACCEL) {
            GAL_UnRLESurface(surface, 1);
    }

    if (flag) {
        surface->flags |= GAL_SRCCOLORKEY;
        surface->format->colorkey = key;
#if 0
        GAL_VideoDevice *video = __mg_current_video;
        GAL_VideoDevice *this  = __mg_current_video;

        if ((surface->flags & GAL_HWACCEL) == GAL_HWACCEL) {
            if ((video->SetHWColorKey == NULL) ||
                 (video->SetHWColorKey(this, surface, key) < 0)) {
                surface->flags &= ~GAL_HWACCEL;
            }
        }
#endif
        if (flag & GAL_RLEACCELOK) {
            surface->flags |= GAL_RLEACCELOK;
        } else {
            surface->flags &= ~GAL_RLEACCELOK;
        }
    } else {
        surface->flags &= ~(GAL_SRCCOLORKEY|GAL_RLEACCELOK);
        surface->format->colorkey = 0;
    }
    GAL_InvalidateMap(surface->map);
    return(0);
}
int GAL_SetAlpha (GAL_Surface *surface, Uint32 flag, Uint8 value)
{
    Uint32  oldflags = surface->flags;
    Uint32  oldalpha = surface->format->alpha;

    /* Sanity check the flag as it gets passed in */
    if (flag & GAL_SRCALPHA) {
        if (flag & (GAL_RLEACCEL|GAL_RLEACCELOK)) {
            flag = GAL_SRCALPHA|GAL_RLEACCELOK;
        } else {
            flag = GAL_SRCALPHA;
        }

        /* Optimize away operations that don't change anything */
        if ((flag == (surface->flags & (GAL_SRCALPHA|GAL_RLEACCELOK)))
                && (!flag || value == oldalpha)) {
            return(0);
        }
    } else if (flag & GAL_SRCPIXELALPHA){
        if (flag & (GAL_RLEACCEL|GAL_RLEACCELOK)) {
            flag = GAL_SRCPIXELALPHA|GAL_RLEACCELOK;
        } else {
            flag = GAL_SRCPIXELALPHA;
        }
    } else {
        flag = 0;
    }

    if(!(flag & GAL_RLEACCELOK) && (surface->flags & GAL_RLEACCEL))
        GAL_UnRLESurface(surface, 1);

    if (flag) {
        if (flag & GAL_SRCALPHA) {
            surface->flags |= GAL_SRCALPHA;
            surface->format->alpha = value;
        } else if (flag & GAL_SRCPIXELALPHA){
            surface->flags |= GAL_SRCPIXELALPHA;
        }

#if 0
        GAL_VideoDevice *video = __mg_current_video;
        GAL_VideoDevice *this  = __mg_current_video;

        if ((surface->flags & GAL_HWACCEL) == GAL_HWACCEL) {
            if ((video->SetHWAlpha == NULL) ||
                 (video->SetHWAlpha(this, surface, value) < 0)) {
                surface->flags &= ~GAL_HWACCEL;
            }
        }
#endif

        if (flag & GAL_RLEACCELOK) {
                surface->flags |= GAL_RLEACCELOK;
        } else {
                surface->flags &= ~GAL_RLEACCELOK;
        }
    } else {
        surface->flags &= ~GAL_SRCALPHA;
        surface->flags &= ~GAL_SRCPIXELALPHA;
        surface->format->alpha = GAL_ALPHA_OPAQUE;
    }
    /*
     * The representation for software surfaces is independent of
     * per-surface alpha, so no need to invalidate the blit mapping
     * if just the alpha value was changed. (If either is 255, we still
     * need to invalidate.)
     */
    if((surface->flags & GAL_HWACCEL) == GAL_HWACCEL
       || oldflags != surface->flags
       || (((oldalpha + 1) ^ (value + 1)) & 0x100))
        GAL_InvalidateMap(surface->map);
    return(0);
}

/*
 * Set the clipping rectangle for a blittable surface
 */
GAL_bool GAL_SetClipRect(GAL_Surface *surface, GAL_Rect *rect)
{
    GAL_Rect full_rect;

    /* Don't do anything if there's no surface to act on */
    if (! surface) {
        return GAL_FALSE;
    }

    /* Set up the full surface rectangle */
    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = surface->w;
    full_rect.h = surface->h;

    /* Set the clipping rectangle */
    if (! rect) {
        surface->clip_rect = full_rect;
        return 1;
    }
    return GAL_IntersectRect(rect, &full_rect, &surface->clip_rect);
}

void GAL_GetClipRect(GAL_Surface *surface, GAL_Rect *rect)
{
    if (surface && rect) {
        *rect = surface->clip_rect;
    }
}

#if defined (_MGGAL_SIGMA8654) || defined (_MGGAL_MSTAR) || defined (_MGGAL_STGFB)
#   define MG_CONFIG_USE_OWN_OVERLAPPED_BITBLIT
#endif

#ifdef MG_CONFIG_USE_OWN_OVERLAPPED_BITBLIT
#include <assert.h>

static int own_overlapped_bitblit(GAL_blit real_blit, struct GAL_Surface *src, GAL_Rect *srcrect,
        struct GAL_Surface *dst, GAL_Rect *dstrect) {
    int w, W, x;
    int h, H, y;
    int delta;
    int ret = 0;
    RECT intersect, src_rc, dst_rc;

    assert(srcrect->w == dstrect->w || srcrect->h == dstrect->h);

    GAL_Rect2RECT(srcrect, &src_rc);
    GAL_Rect2RECT(dstrect, &dst_rc);

    /* don't intersect or horizontal left or up or left-up overlapped blit directly */
    if (! IntersectRect(&intersect, &src_rc, &dst_rc)
            || (dst_rc.top <= src_rc.top && dst_rc.left <= src_rc.left)) {
        return real_blit(src, srcrect, dst, dstrect);
    }

    GAL_Rect src1, dst1;

    /* vertical overlapped.
     * if left-up right-up left-down right-down, horizontal is faster than vertical than use horizonatl mode. */
    if ((dst_rc.top != src_rc.top) && (RECTH(intersect) <= RECTW(intersect))) {
        h = RECTH(intersect);
        H = RECTH(src_rc);
        delta = H - h;

        /* down or left-down or right-down overlapped, separate into per not-Intersect rect to blit,
         * from bottom to up */
        if (dst_rc.top > src_rc.top) {
            for (y = H; y > 0; y -= delta) {
                if (y < delta) {
                    delta = y;
                }
                src1.x = src_rc.left;
                src1.y = src_rc.top + y - delta;

                dst1.x = dst_rc.left;
                dst1.y = dst_rc.top + y - delta;

                src1.w = dst1.w = srcrect->w;
                src1.h = dst1.h = delta;

                ret |= real_blit(src, &src1, dst, &dst1);
            }
        }
        /* up or left-up or right-up overlapped, just the same down except from up to bottom */
        else {
            for (y = 0; y < H; y += delta) {
                if (y + delta > H) {
                    delta = H - y;
                }
                src1.x = src_rc.left;
                src1.y = src_rc.top + y;

                dst1.x = dst_rc.left;
                dst1.y = dst_rc.top + y;

                src1.w = dst1.w = srcrect->w;
                src1.h = dst1.h = delta;

                ret |= real_blit(src, &src1, dst, &dst1);
            }
        }
    }
    /* horizontal overlapped */
    else {
        w = RECTW(intersect);
        W = RECTW(src_rc);
        delta = W - w;

        /* horizontal right overlapped, separate into per not-Intersect rect to blit,
         * from right to left */
        if (dst_rc.left > src_rc.left) {
            for (x = W; x > 0; x -= delta) {
                if (x < delta) {
                    delta = x;
                }
                src1.x = src_rc.left + x - delta;
                src1.y = src_rc.top;

                dst1.x = dst_rc.left + x - delta;
                dst1.y = dst_rc.top;

                src1.w = dst1.w = delta;
                src1.h = dst1.h = srcrect->h;

                ret |= real_blit(src, &src1, dst, &dst1);
            }
        }
        /* horizontal left overlapped. just the same right except from left to right.
         * this just for optimization overlapped blit speed. */
        else {
            for (x = 0; x < W; x += delta) {
                if (x + delta > W) {
                    delta = W - x;
                }
                src1.x = src_rc.left + x;
                src1.y = src_rc.top;

                dst1.x = dst_rc.left + x;
                dst1.y = dst_rc.top;

                src1.w = dst1.w = delta;
                src1.h = dst1.h = srcrect->h;

                ret |= real_blit(src, &src1, dst, &dst1);
            }
        }
    }

    return ret;
}
#endif

/*
 * Set up a blit between two surfaces -- split into three parts:
 * The upper part, GAL_UpperBlit(), performs clipping and rectangle
 * verification.  The lower part is a pointer to a low level
 * accelerated blitting function.
 *
 * These parts are separated out and each used internally by this
 * library in the optimimum places.  They are exported so that if
 * you know exactly what you are doing, you can optimize your code
 * by calling the one(s) you need.
 */
int GAL_LowerBlit (GAL_Surface *src, GAL_Rect *srcrect,
        GAL_Surface *dst, GAL_Rect *dstrect, DWORD op)
{
    GAL_blit do_blit;
    int ret;

    /* Check to make sure the blit mapping is valid */
    if ((src->map->dst != dst) ||
             (src->map->dst->format_version != src->map->format_version)) {
        if (GAL_MapSurface(src, srcrect, dst, dstrect, op) < 0) {
            return(-1);
        }
    }

    /* Figure out which blitter to use */
    if ((src->flags & GAL_HWACCEL) == GAL_HWACCEL) {
        do_blit = src->map->hw_blit;
    } else {
        do_blit = src->map->sw_blit;
    }

#ifdef _MGGAL_S3C6410
    {
        extern int s3c6410_size_table[48];
        int H = src->h / 10;
        if (H >= sizeof(s3c6410_size_table)/sizeof(s3c6410_size_table[0])) {
            H = sizeof(s3c6410_size_table)/sizeof(s3c6410_size_table[0]) - 1;
        }
        if (src->w/10 < s3c6410_size_table[H]) {
            do_blit = src->map->sw_blit;
        }
    }
#endif

#ifdef MG_CONFIG_USE_OWN_OVERLAPPED_BITBLIT
    ret = own_overlapped_bitblit(do_blit, src, srcrect, dst, dstrect);
#else
    _DBG_PRINTF("calling do_blit: from (%d, %d, %d, %d) to (%d, %d)\n",
                srcrect->x, srcrect->y, srcrect->w, srcrect->h,
                dstrect->x, dstrect->y);
    ret = do_blit(src->map->video, src, srcrect, dst, dstrect);
#endif

    return ret;
}


int GAL_UpperBlit (GAL_Surface *src, GAL_Rect *srcrect,
           GAL_Surface *dst, GAL_Rect *dstrect, DWORD op)
{
    GAL_Rect my_dst, my_src;
    int w, h;

    if (! src || ! dst) {
        GAL_SetError("NEWGAL: GAL_UpperBlit: passed a NULL surface.\n");
        return(-1);
    }

    /* If the destination rectangle is NULL, use the entire dest surface */
    if (dstrect == NULL) {
        my_dst.x = my_dst.y = 0;
        dstrect = &my_dst;
    }

    if (srcrect == NULL) {
        my_src.x = my_src.y = 0;
        srcrect = &my_src;

        srcrect->w = src->w;
        srcrect->h = src->h;
    }

    /* make sure that two rectangles have the same size */
    w = dstrect->w = srcrect->w;
    h = dstrect->h = srcrect->h;

    _DBG_PRINTF("before clipping: from (%d, %d, %d, %d) to (%d, %d)\n",
                srcrect->x, srcrect->y, srcrect->w, srcrect->h,
                dstrect->x, dstrect->y);

    /* XXX: We must offset and clipping the source rectangle properly. */
    int offx = dstrect->x - srcrect->x;
    int offy = dstrect->y - srcrect->y;

    /* clip the destination rectangle against the clip rectangle. */
    if (TRUE) {
        GAL_Rect *clip = &dst->clip_rect;
        int dx, dy;

        dx = clip->x - dstrect->x;
        if (dx > 0) {
            w -= dx;
            dstrect->x += dx;
        }
        dx = dstrect->x + w - clip->x - clip->w;
        if (dx > 0)
            w -= dx;

        dy = clip->y - dstrect->y;
        if (dy > 0) {
            h -= dy;
            dstrect->y += dy;
        }
        dy = dstrect->y + h - clip->y - clip->h;
        if (dy > 0)
            h -= dy;
    }

    if (w <= 0 || h <= 0) {
        _DBG_PRINTF("destrect clipped\n");
        goto done;
    }

    /* reset the size */
    srcrect->w = dstrect->w = w;
    srcrect->h = dstrect->h = h;

    /* offset the source rectangle */
    srcrect->x = dstrect->x - offx;
    srcrect->y = dstrect->y - offy;

    int old_src_x = srcrect->x;
    int old_src_y = srcrect->y;

    /* clip the source rectangle against the surface. */
    if (TRUE) {
        GAL_Rect surrect = { 0, 0, src->w, src->h };
        if (!GAL_IntersectRect(srcrect, &surrect, srcrect)) {
            _WRN_PRINTF("srcrect clipped\n");
            goto done;
        }

        /* offset the destination rectangle propertly */
        offx = srcrect->x - old_src_x;
        offy = srcrect->y - old_src_y;

        dstrect->x += offx;
        dstrect->y += offy;
        dstrect->w = srcrect->w;
        dstrect->h = srcrect->h;
    }

    _DBG_PRINTF("After clipping: from (%d, %d, %d, %d) to (%d, %d)\n",
                srcrect->x, srcrect->y, srcrect->w, srcrect->h,
                dstrect->x, dstrect->y);

    return GAL_LowerBlit(src, srcrect, dst, dstrect, op);

done:
    dstrect->w = dstrect->h = 0;
    return 0;
}

static void GAL_SoftFillRect (GAL_Surface *dst, const GAL_Rect *dstrect, Uint32 color)
{
    int x, y;
    Uint8 *row;

    row = (Uint8 *)dst->pixels + dst->pixels_off + dstrect->y * dst->pitch +
            dstrect->x * dst->format->BytesPerPixel;

    if (dst->format->palette || (color == 0)) {
        x = dstrect->w*dst->format->BytesPerPixel;
        if (!color && !((long)row&3) && !(x&3) && !(dst->pitch&3)) {
            int n = x >> 2;
            for (y=dstrect->h; y; --y) {
                GAL_memset4(row, 0, n);
                row += dst->pitch;
            }
        } else {
            {
                for(y = dstrect->h; y; y--) {
                    memset(row, color, x);
                    row += dst->pitch;
                }
            }
        }
    } else {
        switch (dst->format->BytesPerPixel) {
            case 2:
            for (y=dstrect->h; y; --y) {
                Uint16 *pixels = (Uint16 *)row;
                Uint16 c = color;
                Uint32 cc = (Uint32)c << 16 | c;
                int n = dstrect->w;
                if((unsigned long)pixels & 3) {
                    *pixels++ = c;
                    n--;
                }
                if(n >> 1)
                    GAL_memset4(pixels, cc, n >> 1);
                if(n & 1)
                    pixels[n - 1] = c;
                row += dst->pitch;
            }
            break;

            case 3:
            if(GAL_BYTEORDER == GAL_BIG_ENDIAN)
                color <<= 8;
            for (y=dstrect->h; y; --y) {
                Uint8 *pixels = row;
                for (x=dstrect->w; x; --x) {
                    memcpy(pixels, &color, 3);
                    pixels += 3;
                }
                row += dst->pitch;
            }
            break;

            case 4:
            for(y = dstrect->h; y; --y) {
                GAL_memset4(row, color, dstrect->w);
                row += dst->pitch;
            }
            break;
        }
    }
}

#ifdef _MGUSE_PIXMAN
#include <pixman.h>

struct rgbamasks_pixman_format_map {
    uint32_t pixman_format;
    Uint32 Rmask, Gmask, Bmask, Amask;
};

static struct rgbamasks_pixman_format_map _format_map_8bpp [] = {
    { PIXMAN_r3g3b2,    0xE0, 0x1C, 0x03, 0x00 },
    { PIXMAN_b2g3r3,    0x0E, 0x38, 0xC0, 0x00 },
    { PIXMAN_a2r2g2b2,  0x30, 0x0C, 0x03, 0xC0 },
    { PIXMAN_a2b2g2r2,  0x03, 0x0C, 0x03, 0xC0 },
};

static struct rgbamasks_pixman_format_map _format_map_16bpp [] = {
    { PIXMAN_x4r4g4b4,  0x0F00, 0x00F0, 0x000F, 0x0000 },
    { PIXMAN_x4b4g4r4,  0x000F, 0x00F0, 0x0F00, 0x0000 },
    // { PIXMAN_r4g4b4x4,  0xF000, 0x0F00, 0x00F0, 0x0000 },
    // { PIXMAN_b4g4r4x4,  0x00F0, 0x0F00, 0xF000, 0x0000 },
    { PIXMAN_a4r4g4b4,  0x0F00, 0x00F0, 0x000F, 0xF000 },
    { PIXMAN_a4b4g4r4,  0x000F, 0x00F0, 0x0F00, 0xF000 },
    // { PIXMAN_r4g4b4a4,  0xF000, 0x0F00, 0x00F0, 0x000F },
    // { PIXMAN_b4g4r4a4,  0x00F0, 0x0F00, 0xF000, 0x000F },
    { PIXMAN_x1r5g5b5,  0x7C00, 0x03E0, 0x001F, 0x0000 },
    { PIXMAN_x1b5g5r5,  0x001F, 0x03E0, 0x7C00, 0x0000 },
    // { PIXMAN_r5g5b5x1,  0xF800, 0x07C0, 0x003E, 0x0000 },
    // { PIXMAN_b5g5r5x1,  0x003E, 0x07C0, 0xF800, 0x0000 },
    { PIXMAN_a1r5g5b5,  0x7C00, 0x03E0, 0x001F, 0x8000 },
    { PIXMAN_a1b5g5r5,  0x001F, 0x03E0, 0x7C00, 0x8000 },
    // { PIXMAN_r5g5b5a1,  0xF800, 0x07C0, 0x003E, 0x0001 },
    // { PIXMAN_b5g5r5a1,  0x003E, 0x07C0, 0xF800, 0x0001 },
    { PIXMAN_r5g6b5,    0xF800, 0x07E0, 0x001F, 0x0000 },
    { PIXMAN_b5g6r5,    0x001F, 0x07E0, 0xF800, 0x0000 },
};

static struct rgbamasks_pixman_format_map _format_map_24bpp [] = {
    { PIXMAN_r8g8b8,    0xFF0000, 0x00FF00, 0x0000FF, 0x000000 },
    { PIXMAN_b8g8r8,    0x0000FF, 0x00FF00, 0xFF0000, 0x000000 },
};

static struct rgbamasks_pixman_format_map _format_map_32bpp [] = {
    { PIXMAN_x8r8g8b8,  0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { PIXMAN_x8b8g8r8,  0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { PIXMAN_r8g8b8x8,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 },
    { PIXMAN_b8g8r8x8,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000 },
    { PIXMAN_a8r8g8b8,  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
    { PIXMAN_a8b8g8r8,  0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 },
    { PIXMAN_r8g8b8a8,  0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF },
    { PIXMAN_b8g8r8a8,  0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF },
};

static uint32_t translate_gal_format(const GAL_PixelFormat *gal_format)
{
    struct rgbamasks_pixman_format_map* map;
    size_t i, n;

    // XXX: ignore format with palette
    if (gal_format->BitsPerPixel <= 8 && gal_format->palette)
        return 0;

    switch (gal_format->BitsPerPixel) {
    case 8:
        map = _format_map_8bpp;
        n = TABLESIZE(_format_map_8bpp);
        break;

    case 16:
        map = _format_map_16bpp;
        n = TABLESIZE(_format_map_16bpp);
        break;

    case 24:
        map = _format_map_24bpp;
        n = TABLESIZE(_format_map_24bpp);
        break;

    case 32:
        map = _format_map_32bpp;
        n = TABLESIZE(_format_map_32bpp);
        break;

    default:
        return 0;
    }

    for (i = 0; i < n; i++) {
        if (gal_format->Rmask == map[i].Rmask &&
                gal_format->Gmask == map[i].Gmask &&
                gal_format->Bmask == map[i].Bmask &&
                gal_format->Amask == map[i].Amask) {
            return map[i].pixman_format;
        }
    }

    return 0;
}

BOOL GAL_CreatePixmanImage (GAL_Surface *surface)
{
    if (!(surface->flags & GAL_FORMAT_CHECKED)) {
        uint32_t pixman_format;

        assert (surface->pix_img == NULL);

        pixman_format = translate_gal_format (surface->format);

        if (pixman_format) {
            surface->pix_img = pixman_image_create_bits_no_clear (
                    (pixman_format_code_t)pixman_format,
                    surface->w, surface->h,
                    (uint32_t *)((char*)surface->pixels + surface->pixels_off),
                    surface->pitch);
        }
        else
            surface->pix_img = NULL;

        surface->flags |= GAL_FORMAT_CHECKED;
    }

    return surface->pix_img != NULL;
}

#endif /* defined _MGUSE_PIXMAN */

/*
 * This function performs a fast fill of the given rectangle with 'color'
 */
int GAL_FillRect(GAL_Surface *dst, const GAL_Rect *dstrect, Uint32 color)
{
    GAL_VideoDevice *video = dst->video;
    GAL_VideoDevice *this  = dst->video;
    GAL_Rect my_dstrect;

    if (!video) {
        video = __mg_current_video;
        this = __mg_current_video;
    }

    /* If 'dstrect' == NULL, then fill the whole surface */
    if (dstrect) {
        /* Perform clipping */
        if (!GAL_IntersectRect(dstrect, &dst->clip_rect, &my_dstrect)) {
            return 0;
        }
    } else {
        my_dstrect = dst->clip_rect;
    }

    /* Check for hardware acceleration */
    if (((dst->flags & GAL_HWSURFACE) == GAL_HWSURFACE) &&
                    video->info.blit_fill) {
        if (video->FillHWRect(this, dst, &my_dstrect, color) == 0)
            return 0;
    }

#ifdef _MGUSE_PIXMAN
    if (GAL_CreatePixmanImage (dst)) {
        pixman_box32_t pixman_box;
        pixman_color_t pixman_color;
        Uint8 r, g, b, a;

        GAL_GetRGBA (color, dst->format, &r, &g, &b, &a);
        pixman_color.red = (uint16_t)r << 8;
        pixman_color.green = (uint16_t)g << 8;
        pixman_color.blue = (uint16_t)b << 8;
        pixman_color.alpha = (uint16_t)a << 8;

        pixman_box.x1 = my_dstrect.x;
        pixman_box.y1 = my_dstrect.y;
        pixman_box.x2 = my_dstrect.x + my_dstrect.w;
        pixman_box.y2 = my_dstrect.y + my_dstrect.h;

        pixman_image_fill_boxes (PIXMAN_OP_SRC, dst->pix_img,
                &pixman_color, 1, &pixman_box);
        return 0;
    }
    else {
        GAL_SoftFillRect (dst, &my_dstrect, color);
    }
#else
    GAL_SoftFillRect (dst, &my_dstrect, color);
#endif /* _MGUSE_PIXMAN */

    /* We're done! */
    return(0);
}

/*
 * Calculate the pad-aligned box sizew in a surface
 */
Uint32 GAL_GetBoxSize (GAL_Surface *surface, Uint32 w, Uint32 h, Uint32* pitch_p)
{
    Uint32 pitch;

    /* Box should be 4-byte aligned for speed */
    pitch = w * surface->format->BytesPerPixel;
    switch (surface->format->BitsPerPixel) {
        case 1:
            pitch = (pitch+7)/8;
            break;
        case 4:
            pitch = (pitch+1)/2;
            break;
        default:
            break;
    }

    pitch = (pitch + 3) & ~3;    /* 4-byte aligning */

    if (pitch_p)
        *pitch_p = pitch;

    return pitch * h;
}

/*
 * This function performs a fast HW -> SW box copying.
 */
int GAL_GetBox (GAL_Surface *src, const GAL_Rect *rect, BITMAP* box)
{
    Sint32 dst_x, dst_y, off_x, off_y, y;
    Uint8 *srcrow, *dstrow;
    Uint32 linelen;
    GAL_Rect srcrect = {0, 0, 0, 0};
    srcrect.w = src->w;
    srcrect.h = src->h;

    if (rect->h <= 0 || rect->w <= 0)
        return -1;

    box->bmType = BMP_TYPE_NORMAL;
    box->bmBitsPerPixel = src->format->BitsPerPixel;
    box->bmBytesPerPixel = src->format->BytesPerPixel;
    box->bmWidth = rect->w;
    box->bmHeight = rect->h;
    box->bmAlphaMask = NULL;
    box->bmAlphaPitch = 0;

    dst_x = rect->x;
    dst_y = rect->y;

    /* Perform clipping */
    if (!GAL_IntersectRect (rect, &srcrect, &srcrect)) {
        return 0;
    }

    off_x = srcrect.x - dst_x;
    off_y = srcrect.y - dst_y;

    if ((box->bmPitch == 0) || (box->bmBits == NULL)) {
        box->bmBits = malloc (GAL_GetBoxSize (src, box->bmWidth, box->bmHeight, &box->bmPitch));
        if (box->bmBits == NULL) {
            GAL_OutOfMemory ();
            return -1;
        }
    }

    linelen = srcrect.w * box->bmBytesPerPixel;
    srcrow = (Uint8 *)src->pixels + srcrect.y * src->pitch +
            srcrect.x * src->format->BytesPerPixel;
    dstrow = box->bmBits + off_y * box->bmPitch +
            off_x * box->bmBytesPerPixel;

    switch (box->bmBytesPerPixel) {
        case 1:
            if (((DWORD)srcrow & 3) || ((DWORD)dstrow & 3)
                    || (linelen & 3) || (box->bmPitch & 3) || (src->pitch & 3))
                goto slow_copy;
            else {
                int n = linelen >> 2;
                for (y = srcrect.h; y; --y) {
                    GAL_memcpy4 (dstrow, srcrow, n);
                    srcrow += src->pitch;
                    dstrow += box->bmPitch;
                }
            }
            break;

        case 2:
            if ((((DWORD)srcrow & 3) != ((DWORD)dstrow & 3))
                    || (box->bmPitch & 3) || (src->pitch & 3))
                goto slow_copy;

            for (y = srcrect.h; y; --y) {
                Uint16 *dstpixels = (Uint16 *)dstrow;
                Uint16 *srcpixels = (Uint16 *)srcrow;
                int n = srcrect.w;
                if ((DWORD)dstpixels & 3) {
                    *dstpixels = *srcpixels;
                    dstpixels++;
                    srcpixels++;
                    n--;
                }

                if (n >> 1)
                    GAL_memcpy4 (dstpixels, srcpixels, n >> 1);
                if (n & 1)
                    dstpixels [n - 1] = srcpixels [n - 1];

                srcrow += src->pitch;
                dstrow += box->bmPitch;
            }
            break;

        case 3:
            goto slow_copy;

        case 4:
            for (y = srcrect.h; y; --y) {
                GAL_memcpy4 (dstrow, srcrow, srcrect.w);
                srcrow += src->pitch;
                dstrow += box->bmPitch;
            }
            break;
    }

    return(0);

slow_copy:
    for (y = srcrect.h; y; --y) {
        GAL_memcpy (dstrow, srcrow, linelen);
        srcrow += src->pitch;
        dstrow += box->bmPitch;
    }

    return(0);
}

/*
 * This function performs a fast SW -> HW box copying.
 */

// helpers

#include "blit.h"

/* aligned 32-bit bound for alpha mask array index  */
#define GET_ALPHA_MASK_INDEX(srcrow, box, pitch, row)           \
do {                                                            \
    int x = (srcrow - (box)->bmBits) % (box)->bmPitch;          \
    int y = (srcrow - (box)->bmBits) / (box)->bmPitch;          \
    (pitch) = (box)->bmAlphaPitch;                              \
    (row) = (y * (pitch)) + (x / (box)->bmBytesPerPixel);       \
} while(0)

static int _PutBoxAlphaChannelEx (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = box->bmAlpha;


    if (bpp == 1)
        return -1;

    if (!(box->bmType & BMP_TYPE_ALPHA_MASK)) {
        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            DUFFS_LOOP(
                {
                    Uint32 pixel;
                    unsigned sR;
                    unsigned sG;
                    unsigned sB;
                    unsigned sA;
                    unsigned dR;
                    unsigned dG;
                    unsigned dB;
                    DISEMBLE_RGBA (srcpixels, bpp, dstfmt, pixel, sR, sG, sB, sA);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    ALPHA_BLEND (sR, sG, sB, (alpha*sA)/255, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                    dstpixels += bpp;
                    srcpixels += bpp;
                },
                w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
        }
    } else {
        /* for aligned 32-bits bound */
        int alpha_mask_index;
        int alpha_mask_pitch;
        int alpha_mask_row;
        GET_ALPHA_MASK_INDEX (srcrow, box, alpha_mask_pitch, alpha_mask_row);

        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            alpha_mask_index = alpha_mask_row;
            DUFFS_LOOP(
                {
                    Uint32 pixel;
                    unsigned sR;
                    unsigned sG;
                    unsigned sB;
                    unsigned sA;
                    unsigned dR;
                    unsigned dG;
                    unsigned dB;
                    DISEMBLE_RGB (srcpixels, bpp, dstfmt, pixel, sR, sG, sB);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    sA = box->bmAlphaMask[alpha_mask_index];
                    ALPHA_BLEND (sR, sG, sB, (alpha*sA)/255, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                    dstpixels += bpp;
                    srcpixels += bpp;
                    alpha_mask_index++;
                },
                w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
            alpha_mask_row += alpha_mask_pitch;
        }
    }

    return 0;
}

static int _PutBoxAlpha (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;


    if (bpp == 1)
        return -1;

    if (!(box->bmType & BMP_TYPE_ALPHA_MASK)) {
        if (dstfmt->Amask == 0xff000000 && dstfmt->BitsPerPixel == 32)
        {
            while(h--) {
                Uint32 *srcp = (Uint32 *)srcrow;
                Uint32 *dstp = (Uint32 *)dstrow;
                unsigned char alpha;
                DUFFS_LOOP4(
                        {
                        Uint32 s;
                        Uint32 d;
                        Uint32 s1;
                        Uint32 d1;
                        Uint32 sA;
                        Uint32 dA;
                        s = *srcp;
                        d = *dstp;
                        sA = s >> 24;
                        dA = d >> 24;
                        dA = sA + dA - ((sA * dA) >> 8);
                        if(dA > 255) dA = 255; //alpha may be greater than 255
                        alpha = s >> 24;
                        s1 = s & 0xff00ff;
                        d1 = d & 0xff00ff;
                        d1 = (d1 + (((s1 - d1) * alpha) >> 8))
                        & 0xff00ff;
                        s &= 0xff00;
                        d &= 0xff00;
                        d = (d + (((s - d) * alpha) >> 8)) & 0xff00;
                        *dstp = d1 | d | (dA << 24);
                        ++srcp;
                        ++dstp;
                        }, w);
                srcrow += box->bmPitch;
                dstrow += dst->pitch;
            }
        }
        else
        {
            while (h--) {
                dstpixels = dstrow;
                srcpixels = srcrow;
                DUFFS_LOOP(
                        {
                        Uint32 pixel;
                        unsigned sR = 0;
                        unsigned sG = 0;
                        unsigned sB = 0;
                        unsigned sA = 0;
                        unsigned dR = 0;
                        unsigned dG = 0;
                        unsigned dB = 0;
                        unsigned dA = 0;
                        DISEMBLE_RGBA (srcpixels, bpp, dstfmt, pixel, sR, sG, sB, sA);
                        DISEMBLE_RGBA (dstpixels, bpp, dstfmt, pixel, dR, dG, dB, dA);
                        ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
                        dA = sA + dA - ((sA * dA) >> 8);
                        if(dA > 255) dA = 255; //alpha may be greater than 255
                        ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, dA);
                        dstpixels += bpp;
                        srcpixels += bpp;
                        },
                        w);
                srcrow += box->bmPitch;
                dstrow += dst->pitch;
            }
        }
    } else {
        /* for aligned 32-bits bound */
        int alpha_mask_index;
        int alpha_mask_pitch;
        int alpha_mask_row;
        GET_ALPHA_MASK_INDEX (srcrow, box, alpha_mask_pitch, alpha_mask_row);

        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            alpha_mask_index = alpha_mask_row;
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                DISEMBLE_RGB (srcpixels, bpp, dstfmt, pixel, sR, sG, sB);
                DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                sA = box->bmAlphaMask[alpha_mask_index];
                ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
                ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                dstpixels += bpp;
                srcpixels += bpp;
                alpha_mask_index++;
            },
            w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
            alpha_mask_row += alpha_mask_pitch;
        }
    }

    return 0;
}

static int _PutBoxAlphaChannel (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = box->bmAlpha;

    if (bpp == 1)
        return -1;

    while (h--) {
        dstpixels = dstrow;
        srcpixels = srcrow;
        DUFFS_LOOP(
        {
            Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            DISEMBLE_RGB (srcpixels, bpp, dstfmt, pixel, sR, sG, sB);
            DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
            ALPHA_BLEND (sR, sG, sB, alpha, dR, dG, dB);
            ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
            dstpixels += bpp;
            srcpixels += bpp;
        },
        w);
        srcrow += box->bmPitch;
        dstrow += dst->pitch;
    }

    return 0;
}

static int _PutBoxKeyAlphaChannelEx (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    Uint32 ckey = box->bmColorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    Uint32 rgbmask = ~dstfmt->Amask;
    unsigned alpha = box->bmAlpha;


    if (bpp == 1)
        return -1;

    if (!(box->bmType & BMP_TYPE_ALPHA_MASK)) {
        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
                if ((pixel & rgbmask) != ckey) {
                    RGBA_FROM_PIXEL (pixel, dstfmt, sR, sG, sB, sA);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    ALPHA_BLEND (sR, sG, sB, (alpha*sA)/255, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                }
                dstpixels += bpp;
                srcpixels += bpp;
            },
            w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
        }
    } else {
        /* for aligned 32-bits bound */
        int alpha_mask_index;
        int alpha_mask_pitch;
        int alpha_mask_row;
        GET_ALPHA_MASK_INDEX (srcrow, box, alpha_mask_pitch, alpha_mask_row);

        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            alpha_mask_index = alpha_mask_row;
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
                if ((pixel & rgbmask) != ckey) {
                    RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    sA = box->bmAlphaMask[alpha_mask_index];
                    ALPHA_BLEND (sR, sG, sB, (alpha*sA)/255, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                }
                dstpixels += bpp;
                srcpixels += bpp;
                alpha_mask_index++;
            },
            w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
            alpha_mask_row += alpha_mask_pitch;
        }
    }

    return 0;
}

static int _PutBoxKeyAlpha (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    Uint32 ckey = box->bmColorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    Uint32 rgbmask = ~dstfmt->Amask;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;


    if (bpp == 1)
        return -1;

    if (!(box->bmType & BMP_TYPE_ALPHA_MASK)) {
        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
                if ((pixel & rgbmask) != ckey) {
                    RGBA_FROM_PIXEL (pixel, dstfmt, sR, sG, sB, sA);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                }
                dstpixels += bpp;
                srcpixels += bpp;
            },
            w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
        }
    } else {
        /* for aligned 32-bits bound */
        int alpha_mask_index;
        int alpha_mask_pitch;
        int alpha_mask_row;
        GET_ALPHA_MASK_INDEX (srcrow, box, alpha_mask_pitch, alpha_mask_row);

        while (h--) {
            dstpixels = dstrow;
            srcpixels = srcrow;
            alpha_mask_index = alpha_mask_row;
            DUFFS_LOOP(
            {
                Uint32 pixel;
                unsigned sR;
                unsigned sG;
                unsigned sB;
                unsigned sA;
                unsigned dR;
                unsigned dG;
                unsigned dB;
                RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
                if ((pixel & rgbmask) != ckey) {
                    RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
                    DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                    sA = box->bmAlphaMask[alpha_mask_index];
                    ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
                    ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
                }
                dstpixels += bpp;
                srcpixels += bpp;
                alpha_mask_index++;
            },
            w);
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
            alpha_mask_row += alpha_mask_pitch;
         }
    }

    return 0;
}

static int _PutBoxKey (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    Uint32 ckey = box->bmColorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

    if (bpp == 1) {
        while (h--) {
            unsigned int i;
            dstpixels = dstrow;
            srcpixels = srcrow;
            for (i = 0; i < w; i++) {
                if (*srcpixels != ckey)
                    *dstpixels = *srcpixels;
                dstpixels += bpp;
                srcpixels += bpp;
            }
            srcrow += box->bmPitch;
            dstrow += dst->pitch;
        }
    }
    else while (h--) {
        dstpixels = dstrow;
        srcpixels = srcrow;
        DUFFS_LOOP(
        {
            Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
            if (pixel != ckey || (pixel == ckey && box->bmType & BMP_TYPE_REPLACEKEY)) {
                if (pixel == ckey)
                    pixel = box->bmColorRep;
                RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
                ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, sR, sG, sB, alpha);
            }
            dstpixels += bpp;
            srcpixels += bpp;
        },
        w);
        srcrow += box->bmPitch;
        dstrow += dst->pitch;
    }

    return 0;
}

static int _PutBoxKeyAlphaChannel (GAL_Surface* dst, BYTE* dstrow, BYTE* srcrow, Uint32 w, Uint32 h, BITMAP* box)
{
    BYTE* dstpixels = dstrow;
    BYTE* srcpixels = srcrow;
    Uint32 ckey = box->bmColorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = box->bmAlpha;

    if (bpp == 1)
        return -1;

    while (h--) {
        dstpixels = dstrow;
        srcpixels = srcrow;
        DUFFS_LOOP(
        {
            Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
            if (pixel != ckey) {
                RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
                DISEMBLE_RGB (dstpixels, bpp, dstfmt, pixel, dR, dG, dB);
                ALPHA_BLEND (sR, sG, sB, alpha, dR, dG, dB);
                ASSEMBLE_RGBA (dstpixels, bpp, dstfmt, dR, dG, dB, alpha);
            }
            dstpixels += bpp;
            srcpixels += bpp;
        },
        w);
        srcrow += box->bmPitch;
        dstrow += dst->pitch;
    }

    return 0;
}

int GAL_PutBox (GAL_Surface *dst, const GAL_Rect *dstrect, BITMAP* box)
{
    Uint32 box_x, box_y, off_x, off_y;
    int y, w, h;
    Uint8 *srcrow, *dstrow;
    Uint32 linelen;
    GAL_Rect my_dstrect;

    /* If 'dstrect' == NULL, then put to (0, 0) */
    if (dstrect) {
        box_x = dstrect->x;
        box_y = dstrect->y;

        /* Perform clipping */
        if (!GAL_IntersectRect (dstrect, &dst->clip_rect, &my_dstrect)) {
            return 0;
        }
    } else {
        box_x = 0;
        box_y = 0;

        my_dstrect = dst->clip_rect;
    }

    off_x = my_dstrect.x - box_x;
    off_y = my_dstrect.y - box_y;
    if (off_x >= box->bmWidth || off_y >= box->bmHeight ||
            box->bmBytesPerPixel != dst->format->BytesPerPixel) {
        return 0;
    }

    dstrow = (Uint8 *)dst->pixels + my_dstrect.y * dst->pitch +
            my_dstrect.x * dst->format->BytesPerPixel;
    srcrow = (Uint8 *)box->bmBits + off_y * box->bmPitch +
            off_x * box->bmBytesPerPixel;
    w = MIN (my_dstrect.w, box->bmWidth);
    h = MIN (my_dstrect.h, box->bmHeight);
    linelen = w * box->bmBytesPerPixel;

    /* TODO: Check for hardware acceleration here */

    if (box->bmType & BMP_TYPE_ALPHA) {
        if ((box->bmType & BMP_TYPE_ALPHACHANNEL) && (box->bmType & BMP_TYPE_COLORKEY)) {
            return _PutBoxKeyAlphaChannelEx (dst, dstrow, srcrow, w, h, box);
        }
        if ((box->bmType & BMP_TYPE_ALPHACHANNEL)) {
            return _PutBoxAlphaChannelEx (dst, dstrow, srcrow, w, h, box);
        }
        else if ((box->bmType & BMP_TYPE_ALPHA) && (box->bmType & BMP_TYPE_COLORKEY)) {
            return _PutBoxKeyAlpha (dst, dstrow, srcrow, w, h, box);
        }
        else if (box->bmType & BMP_TYPE_ALPHA) {
            return _PutBoxAlpha (dst, dstrow, srcrow, w, h, box);
        }
    }
    else if ((box->bmType & BMP_TYPE_ALPHACHANNEL) && (box->bmType & BMP_TYPE_COLORKEY)) {
        return _PutBoxKeyAlphaChannel (dst, dstrow, srcrow, w, h, box);
    }
    else if (box->bmType & BMP_TYPE_COLORKEY) {
        return _PutBoxKey (dst, dstrow, srcrow, w, h, box);
    }
    else if (box->bmType & BMP_TYPE_ALPHACHANNEL) {
        return _PutBoxAlphaChannel (dst, dstrow, srcrow, w, h, box);
    }


    switch (dst->format->BytesPerPixel) {
        case 1:
            if (((DWORD)srcrow & 3) || ((DWORD)dstrow & 3)
                    || (linelen & 3) || (box->bmPitch & 3) || (dst->pitch & 3))
                goto slow_copy;
            else {
                int n = linelen >> 2;
                for (y = h; y; --y) {
                    GAL_memcpy4 (dstrow, srcrow, n);
                    srcrow += box->bmPitch;
                    dstrow += dst->pitch;
                }
            }
            break;

        case 2:
            if ((((DWORD)dstrow & 3) != ((DWORD)srcrow & 3))
                    || (box->bmPitch & 3) || (dst->pitch & 3))
                goto slow_copy;

            for (y = h; y; --y) {
                Uint16 *dstpixels = (Uint16 *)dstrow;
                Uint16 *srcpixels = (Uint16 *)srcrow;
                int n = w;
                if ((DWORD)dstpixels & 3) {
                    *dstpixels = *srcpixels;
                    dstpixels++;
                    srcpixels++;
                    n--;
                }

                if (n >> 1)
                    GAL_memcpy4 (dstpixels, srcpixels, n >> 1);
                if (n & 1)
                    dstpixels [n - 1] = srcpixels [n - 1];

                srcrow += box->bmPitch;
                dstrow += dst->pitch;
            }
            break;

        case 3:
            goto slow_copy;

        case 4:
            for (y = h; y; --y) {
                GAL_memcpy4 (dstrow, srcrow, w);
                srcrow += box->bmPitch;
                dstrow += dst->pitch;
            }
            break;
    }

    return 0;

slow_copy:
    for (y = h; y; --y) {
        GAL_memcpy (dstrow, srcrow, linelen);
        srcrow += box->bmPitch;
        dstrow += dst->pitch;
    }

    return 0;
}

/*
 * Convert a surface into the specified pixel format.
 */
GAL_Surface * GAL_ConvertSurface (GAL_Surface *surface,
                    GAL_PixelFormat *format, Uint32 flags)
{
    GAL_Surface *convert;
    Uint32 colorkey = 0;
    Uint8 alpha = 0;
    Uint32 surface_flags;
    GAL_Rect bounds;

    /* Check for empty destination palette! (results in empty image) */
    if (format->palette != NULL) {
        int i;
        for (i=0; i<format->palette->ncolors; ++i) {
            if ((format->palette->colors[i].r != 0) ||
                 (format->palette->colors[i].g != 0) ||
                 (format->palette->colors[i].b != 0))
                break;
        }
        if (i == format->palette->ncolors) {
            GAL_SetError("NEWGAL: Empty destination palette.\n");
            return(NULL);
        }
    }

    /* Create a new surface with the desired format */
    convert = GAL_CreateRGBSurface(flags,
                surface->w, surface->h, format->BitsPerPixel,
        format->Rmask, format->Gmask, format->Bmask, format->Amask);
    if (convert == NULL) {
        return(NULL);
    }

    /* Copy the palette if any */
    if (format->palette && convert->format->palette) {
        memcpy(convert->format->palette->colors,
                format->palette->colors,
                format->palette->ncolors*sizeof(GAL_Color));
        convert->format->palette->ncolors = format->palette->ncolors;
    }

    /* Save the original surface color key and alpha */
    surface_flags = surface->flags;
    if ((surface_flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        /* Convert colourkeyed surfaces to RGBA if requested */
        if((flags & GAL_SRCCOLORKEY) != GAL_SRCCOLORKEY
           && format->Amask) {
            surface_flags &= ~GAL_SRCCOLORKEY;
        } else {
            colorkey = surface->format->colorkey;
            GAL_SetColorKey(surface, 0, 0);
        }
    }
    if ((surface_flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
        alpha = surface->format->alpha;
        GAL_SetAlpha(surface, 0, 0);
    }

    /* Copy over the image data */
    bounds.x = 0;
    bounds.y = 0;
    bounds.w = surface->w;
    bounds.h = surface->h;
    GAL_LowerBlit(surface, &bounds, convert, &bounds, 0);

    /* Clean up the original surface, and update converted surface */
    if (convert != NULL) {
        GAL_SetClipRect(convert, &surface->clip_rect);
    }
    if ((surface_flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        Uint32 cflags = surface_flags&(GAL_SRCCOLORKEY|GAL_RLEACCELOK);
        if (convert != NULL) {
            Uint8 keyR, keyG, keyB;

            GAL_GetRGB(colorkey,surface->format,&keyR,&keyG,&keyB);
            GAL_SetColorKey(convert, cflags|(flags&GAL_RLEACCELOK),
                GAL_MapRGB(convert->format, keyR, keyG, keyB));
        }
        GAL_SetColorKey(surface, cflags, colorkey);
    }
    if ((surface_flags & GAL_SRCALPHA) == GAL_SRCALPHA) {
        Uint32 aflags = surface_flags&(GAL_SRCALPHA|GAL_RLEACCELOK);
        if (convert != NULL) {
                GAL_SetAlpha(convert, aflags|(flags&GAL_RLEACCELOK),
                alpha);
        }
        GAL_SetAlpha(surface, aflags, alpha);
    }

    /* We're ready to go! */
    return(convert);
}

/*
 * Free a surface created by the above function.
 */
void GAL_FreeSurface (GAL_Surface *surface)
{
    /* Free anything that's not NULL, and not the screen surface */
    if ((surface == NULL) ||
            (__mg_current_video && (surface == GAL_VideoSurface))) {
        return;
    }

    if (--surface->refcount > 0) {
        return;
    }

#ifdef _MGUSE_UPDATE_REGION
    EmptyClipRgn (&surface->update_region);
#endif

    if ((surface->flags & GAL_RLEACCEL) == GAL_RLEACCEL) {
        GAL_UnRLESurface(surface, 0);
    }

    if (surface->format) {
        GAL_FreeFormat(surface->format);
        surface->format = NULL;
    }

    if (surface->map != NULL) {
        GAL_FreeBlitMap(surface->map);
        surface->map = NULL;
    }

#if IS_COMPOSITING_SCHEMA
    if (surface->shared_header == NULL && surface->dirty_info != NULL) {
        free (surface->dirty_info);
    }

    if (surface->shared_header) {
        if (surface->flags & GAL_SSURF_ATTACHED)
            GAL_DetachSharedSurfaceData(surface);
        else
            GAL_FreeSharedSurfaceData(surface);

        if (surface->fd >= 0) {
            close(surface->fd);
            surface->fd = -1;
        }

        surface->hwdata = NULL;
        surface->pixels = NULL;
    }
    else
#endif

    if ((surface->flags & GAL_HWSURFACE) == GAL_HWSURFACE) {
        GAL_VideoDevice *video = surface->video;

        assert (video);
        assert (video->FreeHWSurface);
        video->FreeHWSurface (video, surface);
    }
#ifdef _MGRM_PROCESSES
    else if ((surface->flags & GAL_HWSURFACE) == GAL_SWSURFACE &&
            (surface->flags & GAL_PREALLOC) == GAL_PREALLOC &&
            surface->hwdata) {
        // This is a surface created in / attached to a named shared memory.

        size_t map_size;
        uint8_t* buff;

        map_size = (size_t)surface->hwdata;
        buff = surface->pixels - surface->pixels_off;
        munmap (buff, map_size);
    }
#endif  /* _MGRM_PROCESSES */
    else if (surface->pixels &&
            ((surface->flags & GAL_PREALLOC) != GAL_PREALLOC)) {
        free (surface->pixels);
    }

#ifdef _MGUSE_PIXMAN
    if (surface->pix_img) {
        _DBG_PRINTF ("There is not cleaned up blitting pixel image: %p\n", surface->pix_img);
        pixman_image_unref (surface->pix_img);
        surface->pix_img = NULL;
    }

    if (surface->msk_img) {
        _DBG_PRINTF ("There is not cleaned up blitting mask image: %p\n", surface->msk_img);
        pixman_image_unref (surface->msk_img);
        surface->msk_img = NULL;
    }
#endif

    free (surface);

#ifdef CHECK_LEAKS
    --surfaces_allocated;
#endif
}

#if IS_SHAREDFB_SCHEMA_PROCS
void GAL_RequestHWSurface (const REQ_HWSURFACE* request, REP_HWSURFACE* reply)
{
    if (__mg_current_video->RequestHWSurface) {
        __mg_current_video->RequestHWSurface (__mg_current_video, request, reply);
    } else if (request->offset == 0) {
        reply->offset = 0;
    } else {
        /* NULL */
    }
}
#endif

BYTE*  gal_PutPixelKeyAlpha (GAL_Surface* dst, BYTE* dstrow,
                Uint32 pixel, MYBITMAP_CONTXT* mybmp)
{
    //BYTE* dstpixels = dstrow;
    //BYTE* srcpixels = srcrow;
    Uint32 ckey = mybmp->colorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    GAL_PixelFormat *srcfmt = mybmp->AlphaPixelFormat;
    int bpp = dstfmt->BytesPerPixel;
    Uint32 rgbmask = ~srcfmt->Amask;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

    if (bpp == 1)
        return dstrow;

    //while (h--) {
        //dstpixels = dstrow;
        //srcpixels = srcrow;
        //DUFFS_LOOP(
        {
            //Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned sA;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            //RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
            if ((pixel & rgbmask) != ckey) {
                RGBA_FROM_PIXEL (pixel, srcfmt, sR, sG, sB, sA);
                DISEMBLE_RGB (dstrow, bpp, dstfmt, pixel, dR, dG, dB);
                ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
                ASSEMBLE_RGBA (dstrow, bpp, dstfmt, dR, dG, dB, alpha);
            }
            dstrow += bpp;
            //srcpixels += bpp;
        }
        //w);
        //srcrow += box->bmPitch;
        //dstrow += dst->pitch;
    //}

    return dstrow;
}

BYTE* gal_PutPixelAlpha (GAL_Surface* dst, BYTE* dstrow,
                Uint32 pixel, MYBITMAP_CONTXT* mybmp)
{
    //BYTE* dstpixels = dstrow;
    //BYTE* srcpixels = srcrow;
    GAL_PixelFormat *dstfmt = dst->format;
    GAL_PixelFormat *srcfmt = mybmp->AlphaPixelFormat;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

    if (bpp == 1)
        return dstrow;

    //while (h--) {
        //dstpixels = dstrow;
        //srcpixels = srcrow;
        //DUFFS_LOOP(
        {
            //Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned sA;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            //RGB_FROM_PIXEL (pixel, srcfmt, sR, sG, sB);
            RGBA_FROM_PIXEL(pixel, srcfmt, sR, sG, sB, sA);
            pixel &= ~srcfmt->Amask;
            //DISEMBLE_RGBA (srcpixels, bpp, srcfmt, pixel, sR, sG, sB, sA);
            DISEMBLE_RGB (dstrow, bpp, dstfmt, pixel, dR, dG, dB);
            ALPHA_BLEND (sR, sG, sB, sA, dR, dG, dB);
            ASSEMBLE_RGBA (dstrow, bpp, dstfmt, dR, dG, dB, alpha);
            dstrow += bpp;
            //srcpixels += bpp;
        }
        //w);
        //srcrow += box->bmPitch;
        //dstrow += dst->pitch;
    //}

    return dstrow;
}

BYTE*  gal_PutPixelKeyAlphaChannel (GAL_Surface* dst, BYTE* dstrow,
                Uint32 pixel, MYBITMAP_CONTXT* mybmp)
{
    //BYTE* dstpixels = dstrow;
    //BYTE* srcpixels = srcrow;
    Uint32 ckey = mybmp->colorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = mybmp->mybmp->alpha;

    if (bpp == 1)
        return dstrow;

    //while (h--) {
        //dstpixels = dstrow;
        //srcpixels = srcrow;
        //DUFFS_LOOP(
        {
            //Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            //RETRIEVE_RGB_PIXEL (srcpixels, bpp, pixel);
            if (pixel != ckey) {
                RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
                DISEMBLE_RGB (dstrow, bpp, dstfmt, pixel, dR, dG, dB);
                ALPHA_BLEND (sR, sG, sB, alpha, dR, dG, dB);
                ASSEMBLE_RGBA (dstrow, bpp, dstfmt, dR, dG, dB, alpha);
            }
            dstrow += bpp;
            //srcpixels += bpp;
        }//,
        //w);
        //srcrow += box->bmPitch;
        //dstrow += dst->pitch;
    //}

    return dstrow;
}

BYTE*  gal_PutPixelKey (GAL_Surface* dst, BYTE* dstrow,
            Uint32 pixel, MYBITMAP_CONTXT* mybmp)
{
    Uint32 ckey = mybmp->colorKey;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = dstfmt->Amask ? GAL_ALPHA_OPAQUE : 0;

    if (bpp == 1) {
        if (pixel != ckey)
            *dstrow = pixel;
        dstrow += bpp;
    }
    else {
        unsigned sR;
        unsigned sG;
        unsigned sB;
        if (pixel != ckey) {
            RGB_FROM_PIXEL (pixel, dstfmt, sR, sG, sB);
            ASSEMBLE_RGBA (dstrow, bpp, dstfmt, sR, sG, sB, alpha);
        }
        dstrow += bpp;
    }

    return dstrow;
}

BYTE* gal_PutPixelAlphaChannel (GAL_Surface* dst, BYTE* dstrow,
        Uint32 pixel, MYBITMAP_CONTXT* mybmp)
{
    //BYTE* dstpixels = dstrow;
    //BYTE* srcpixels = srcrow;
    GAL_PixelFormat *dstfmt = dst->format;
    int bpp = dstfmt->BytesPerPixel;
    unsigned alpha = mybmp->mybmp->alpha;

    if (bpp == 1)
        return dstrow;

    //while (h--) {
        //dstpixels = dstrow;
        //srcpixels = srcrow;
        //DUFFS_LOOP(
        {
            //Uint32 pixel;
            unsigned sR;
            unsigned sG;
            unsigned sB;
            unsigned dR;
            unsigned dG;
            unsigned dB;
            RGB_FROM_PIXEL(pixel, dstfmt, sR, sG, sB);
            //DISEMBLE_RGB (srcpixels, bpp, dstfmt, pixel, sR, sG, sB);
            DISEMBLE_RGB (dstrow, bpp, dstfmt, pixel, dR, dG, dB);
            ALPHA_BLEND (sR, sG, sB, alpha, dR, dG, dB);
            ASSEMBLE_RGBA (dstrow, bpp, dstfmt, dR, dG, dB, alpha);
            dstrow += bpp;
            //srcpixels += bpp;
        }//,
        //w);
        //srcrow += box->bmPitch;
        //dstrow += dst->pitch;
    //}

    return dstrow;
}

int GAL_LockSurface(GAL_Surface *surface)
{
    GAL_VideoDevice *video = surface->video;
    if (video && GAL_MUSTLOCK(surface) && video->LockHWSurface) {
        video->LockHWSurface(video, surface);
        return 0;
    }

    return -1;
}

void GAL_UnlockSurface (GAL_Surface *surface)
{
    GAL_VideoDevice *video = surface->video;
    if (video && GAL_MUSTLOCK(surface) && video->UnlockHWSurface) {
        video->UnlockHWSurface(video, surface);
    }
}

