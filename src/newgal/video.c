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

/* The high-level video driver subsystem */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "blit.h"
#include "pixels_c.h"
#include "license.h"

/* Available video drivers */
static VideoBootStrap *bootstrap[] = {
#ifdef _MGGAL_DUMMY
    &DUMMY_bootstrap,
#endif
#ifdef _MGGAL_FBCON
    &FBCON_bootstrap,
#endif
#ifdef _MGGAL_QVFB
    &QVFB_bootstrap,
#endif
#ifdef _MGGAL_PCXVFB
    &PCXVFB_bootstrap,
#endif
#ifdef _MGGAL_RTOSXVFB
    &RTOS_XVFB_bootstrap,
#endif
#ifdef _MGGAL_COMMLCD
    &COMMLCD_bootstrap,
#endif
#ifdef _MGGAL_SHADOW
    &SHADOW_bootstrap,
#endif
#ifdef _MGGAL_MLSHADOW
    &MLSHADOW_bootstrap,
#endif
#ifdef _MGGAL_EM85XXYUV
    &EM85XXYUV_bootstrap,
#endif
#ifdef _MGGAL_EM85XXOSD
    &EM85XXOSD_bootstrap,
#endif
#ifdef _MGGAL_X11
    &X11_bootstrap,
#endif
#ifdef _MGGAL_DGA
    &DGA_bootstrap,
#endif
#ifdef _MGGAL_GGI
    &GGI_bootstrap,
#endif
#ifdef _MGGAL_SVGALIB
    &SVGALIB_bootstrap,
#endif
#ifdef _MGGAL_SVPXXOSD
    &SVPXXOSD_bootstrap,
#endif
#ifdef _MGGAL_BF533
    &BF533_bootstrap,
#endif
#ifdef _MGGAL_MB93493
    &MB93493_bootstrap,
#endif
#ifdef _MGGAL_WVFB
    &WVFB_bootstrap,
#endif
#ifdef _MGGAL_UTPMC
    &UTPMC_bootstrap,
#endif
#ifdef _MGGAL_DFB
    &DFB_bootstrap,
#endif
#ifdef _MGGAL_EM86GFX
    &EM86GFX_bootstrap,
#endif
#ifdef _MGGAL_HI3510
    &HI3510_bootstrap,
#endif
#ifdef _MGGAL_HI3560
    &HI3560_bootstrap,
#endif
#ifdef _MGGAL_HI3560A
    &HI3560A_bootstrap,
#endif
#ifdef _MGGAL_NEXUS
    &NEXUS_bootstrap,
#endif
#ifdef _MGGAL_S3C6410
    &S3C6410_bootstrap,
#endif
#ifdef _MGGAL_SIGMA8654
    &SIGMA8654GFX_bootstrap,
#endif
#ifdef _MGGAL_MSTAR
    &MSTAR_bootstrap,
#endif
#ifdef _MGGAL_CUSTOMGAL
    &CUSTOMGAL_bootstrap,
#endif
#ifdef _MGGAL_GDL
    &GDL_bootstrap,
#endif
#ifdef _MGGAL_STGFB
    &STGFB_bootstrap,
#endif
    NULL
};

GAL_VideoDevice *current_video = NULL;

/* Various local functions */
int GAL_VideoInit(const char *driver_name, Uint32 flags);
void GAL_VideoQuit(void);

GAL_VideoDevice *GAL_GetVideo(const char* driver_name)
{
    GAL_VideoDevice *video;
    int index;
    int i;

    index = 0;
    video = NULL;
    if ( driver_name != NULL ) {
        for ( i=0; bootstrap[i]; ++i ) {
            if ( strncmp(bootstrap[i]->name, driver_name,
                        strlen(bootstrap[i]->name)) == 0 ) {
                if ( bootstrap[i]->available() ) {
                    video = bootstrap[i]->create(index);
                    break;
                }
            }
        }
    } else {
        for ( i=0; bootstrap[i]; ++i ) {
            if ( bootstrap[i]->available() ) {
                video = bootstrap[i]->create(index);
                if ( video != NULL ) {
                    break;
                }
            }
        }
    }
    if (video == NULL)
        return NULL;

    video->name = bootstrap[i]->name;
    /* Do some basic variable initialization */
    if (current_video == NULL)
        video->screen = NULL;

    video->physpal = NULL;
    video->offset_x = 0;
    video->offset_y = 0;
    memset(&video->info, 0, (sizeof video->info));

    return video;
}

/*
 * Initialize the video subsystems -- determine native pixel format
 */
int GAL_VideoInit (const char *driver_name, Uint32 flags)
{
    GAL_VideoDevice *video;
    GAL_PixelFormat vformat;
    Uint32 video_flags;

    /* Check to make sure we don't overwrite 'current_video' */
    if ( current_video != NULL ) {
        GAL_VideoQuit();
    }

    video = GAL_GetVideo(driver_name);

    if ( video == NULL ) {
        return (-1);
    }
    video->screen = NULL;
    current_video = video;

    /* Initialize the video subsystem */
    memset(&vformat, 0, sizeof(vformat));
    if ( video->VideoInit(video, &vformat) < 0 ) {
        GAL_VideoQuit();
        return(-1);
    }

    {
#define FLAGS_BLIT_FILL  0x01
#define FLAGS_BLIT_HW    0x02
#define FLAGS_BLIT_HW_CC    0x04
#define FLAGS_BLIT_HW_A    0x08
        char *env;
        if ((env = getenv("MG_ENV_HWACCEL_FLAGS"))) {
            GAL_VideoInfo *video_info = &video->info;
            int flags = atoi(env);

            /* FillRect */
            video_info->blit_fill = (video_info->blit_fill && (flags & FLAGS_BLIT_FILL)) ? 1 : 0;
            /* BitBlit */
            video_info->blit_hw = (video_info->blit_hw && (flags & FLAGS_BLIT_HW)) ? 1 : 0;
            /* Colorkey */
            video_info->blit_hw_CC = (video_info->blit_hw_CC && (flags & FLAGS_BLIT_HW_CC)) ? 1 : 0;
            /* Alpha */
            video_info->blit_hw_A  = (video_info->blit_hw_A && (flags & FLAGS_BLIT_HW_A)) ? 1 : 0;
        }
    }

    /* Create a zero sized video surface of the appropriate format */
    video_flags = GAL_SWSURFACE;
    GAL_VideoSurface = GAL_CreateRGBSurface(video_flags, 0, 0,
            vformat.BitsPerPixel,
            vformat.Rmask, vformat.Gmask, vformat.Bmask, vformat.Amask);

    if ( GAL_VideoSurface == NULL ) {
        GAL_VideoQuit();
        return(-1);
    }

    GAL_VideoSurface->video = current_video;

    video->info.vfmt = GAL_VideoSurface->format;

    return(0);
}

char *GAL_VideoDriverName(char *namebuf, int maxlen)
{
    if ( current_video != NULL ) {
        strncpy(namebuf, current_video->name, maxlen-1);
        namebuf[maxlen-1] = '\0';
        return(namebuf);
    }
    return(NULL);
}

/*
 * Get the current display surface
 */
GAL_Surface *GAL_GetVideoSurface(void)
{
    GAL_Surface *visible;

    visible = NULL;
    if ( current_video ) {
        visible = current_video->screen;
    }
    return(visible);
}

/*
 * Get the current information about the video hardware
 */
const GAL_VideoInfo *GAL_GetVideoInfo(void)
{
    const GAL_VideoInfo *info;

    info = NULL;
    if ( current_video ) {
        info = &current_video->info;
    }
    return(info);
}

/*
 * Return a pointer to an array of available screen dimensions for the
 * given format, sorted largest to smallest.  Returns NULL if there are
 * no dimensions available for a particular format, or (GAL_Rect **)-1
 * if any dimension is okay for the given format.  If 'format' is NULL,
 * the mode list will be for the format given by GAL_GetVideoInfo()->vfmt
 */
GAL_Rect ** GAL_ListModes (GAL_PixelFormat *format, Uint32 flags)
{
    GAL_VideoDevice *video = current_video;
    GAL_VideoDevice *this  = current_video;
    GAL_Rect **modes;

    modes = NULL;
    if ( GAL_VideoSurface ) {
        if ( format == NULL ) {
            format = GAL_VideoSurface->format;
        }
        modes = video->ListModes(this, format, flags);
    }
    return(modes);
}

/*
 * Check to see if a particular video mode is supported.
 * It returns 0 if the requested mode is not supported under any bit depth,
 * or returns the bits-per-pixel of the closest available mode with the
 * given width and height.  If this bits-per-pixel is different from the
 * one used when setting the video mode, GAL_SetVideoMode() will succeed,
 * but will emulate the requested bits-per-pixel with a shadow surface.
 */
static Uint8 GAL_closest_depths[4][8] = {
    /* 8 bit closest depth ordering */
    { 0, 8, 16, 15, 32, 24, 0, 0 },
    /* 15,16 bit closest depth ordering */
    { 0, 16, 15, 32, 24, 8, 0, 0 },
    /* 24 bit closest depth ordering */
    { 0, 24, 32, 16, 15, 8, 0, 0 },
    /* 32 bit closest depth ordering */
    { 0, 32, 16, 15, 24, 8, 0, 0 }
};

int GAL_VideoModeOK (int width, int height, int bpp, Uint32 flags) 
{
    int table, b, i;
    int supported;
    GAL_PixelFormat format;
    GAL_Rect **sizes;

    /* Currently 1 and 4 bpp are not supported */
    if ( bpp < 8 || bpp > 32 ) {
        return(0);
    }
    if ( (width == 0) || (height == 0) ) {
        return(0);
    }

    /* Search through the list valid of modes */
    memset(&format, 0, sizeof(format));
    supported = 0;
    table = ((bpp+7)/8)-1;
    GAL_closest_depths[table][0] = bpp;
    GAL_closest_depths[table][7] = 0;
    for ( b = 0; !supported && GAL_closest_depths[table][b]; ++b ) {
        format.BitsPerPixel = GAL_closest_depths[table][b];
        sizes = GAL_ListModes(&format, flags);
        if ( sizes == (GAL_Rect **)0 ) {
            /* No sizes supported at this bit-depth */
            continue;
        } else 
            if ( (sizes == (GAL_Rect **)-1) ||
                    current_video->handles_any_size ) {
                /* Any size supported at this bit-depth */
                supported = 1;
                continue;
            } else
                for ( i=0; sizes[i]; ++i ) {
                    if ((sizes[i]->w == width) && (sizes[i]->h == height)) {
                        supported = 1;
                        break;
                    }
                }
    }
    if ( supported ) {
        --b;
        return(GAL_closest_depths[table][b]);
    }
    else 
        return(0);
}

/*
 * Get the closest non-emulated video mode to the one requested
 */
static int GAL_GetVideoMode (int *w, int *h, int *BitsPerPixel, Uint32 flags)
{
    int table, b, i;
    int supported;
    int native_bpp;
    GAL_PixelFormat format;
    GAL_Rect **sizes;

    /* Try the original video mode, get the closest depth */
    native_bpp = GAL_VideoModeOK(*w, *h, *BitsPerPixel, flags);

    if (native_bpp == 0)
        return 0;

    if ( native_bpp == *BitsPerPixel ) {
        return(1);
    }
    if ( native_bpp > 0 ) {
        *BitsPerPixel = native_bpp;
        return(1);
    }

    /* No exact size match at any depth, look for closest match */
    memset(&format, 0, sizeof(format));
    supported = 0;
    table = ((*BitsPerPixel+7)/8)-1;
    GAL_closest_depths[table][0] = *BitsPerPixel;
    GAL_closest_depths[table][7] = GAL_VideoSurface->format->BitsPerPixel;

    for ( b = 0; !supported && GAL_closest_depths[table][b]; ++b ) {
        format.BitsPerPixel = GAL_closest_depths[table][b];
        sizes = GAL_ListModes(&format, flags);
        if ( sizes == (GAL_Rect **)0 ) {
            /* No sizes supported at this bit-depth */
            continue;
        }
        for ( i=0; sizes[i]; ++i ) {
            if ((sizes[i]->w < *w) || (sizes[i]->h < *h)) {
                if ( i > 0 ) {
                    --i;
                    *w = sizes[i]->w;
                    *h = sizes[i]->h;
                    *BitsPerPixel = GAL_closest_depths[table][b];
                    supported = 1;
                } else {
                    /* Largest mode too small... */;
                }
                break;
            }
        }
        if ( (i > 0) && ! sizes[i] ) {
            /* The smallest mode was larger than requested, OK */
            --i;
            *w = sizes[i]->w;
            *h = sizes[i]->h;
            *BitsPerPixel = GAL_closest_depths[table][b];
            supported = 1;
        }
    }
    if ( ! supported ) {
        GAL_SetError("NEWGAL: No video mode large enough for the resolution "
                "specified.\n");
    }
    return(supported);
}

/* This should probably go somewhere else -- like GAL_surface.c */
static void GAL_ClearSurface(GAL_Surface *surface)
{
    Uint32 black;

    black = GAL_MapRGB(surface->format, 0, 0, 0);
    GAL_FillRect(surface, NULL, black);
}

/*
 * Set the requested video mode, allocating a shadow buffer if necessary.
 */
GAL_Surface * GAL_SetVideoMode (int width, int height, int bpp, Uint32 flags)
{
    GAL_VideoDevice *video, *this;
    GAL_Surface *prev_mode, *mode;
    int video_w;
    int video_h;
    int video_bpp;

    this = video = current_video;

    /* Default to the current video bpp */
    if ( bpp == 0 ) {
        flags |= GAL_ANYFORMAT;
        bpp = GAL_VideoSurface->format->BitsPerPixel;
    }

    /* Get a good video mode, the closest one possible */
    video_w = width;
    video_h = height;
    video_bpp = bpp;
#ifdef _MGRM_PROCESSES
    if (mgIsServer && !GAL_GetVideoMode(&video_w, &video_h, &video_bpp, flags)){
#else

    if ( !GAL_GetVideoMode(&video_w, &video_h, &video_bpp, flags) ) {
#endif
            GAL_SetError ("NEWGAL: GAL_GetVideoMode error, "
                            "not supported video mode.\n");
            return(NULL);
        }
    /* Check the requested flags */
    /* There's no palette in > 8 bits-per-pixel mode */
    if ( video_bpp > 8 ) {
        flags &= ~GAL_HWPALETTE;
    }

    if ( video->physpal ) {
        free(video->physpal->colors);
        free(video->physpal);
        video->physpal = NULL;
    }

    /* Try to set the video mode, along with offset and clipping */
    prev_mode = GAL_VideoSurface;
    GAL_VideoSurface = NULL;    /* In case it's freed by driver */
    mode = video->SetVideoMode(this, prev_mode,video_w,video_h,video_bpp,flags);

    GAL_VideoSurface = (mode != NULL) ? mode : prev_mode;

    if ((mode != NULL)) {
        /* Sanity check */
        if ( (mode->w < width) || (mode->h < height) ) {
            GAL_SetError("NEWGAL: Video mode smaller than requested.\n");
            return(NULL);
        }

        /* If we have a palettized surface, create a default palette */
        if ( mode->format->palette ) {
            GAL_PixelFormat *vf = mode->format;
            GAL_DitherColors(vf->palette->colors, vf->BitsPerPixel);
            vf->DitheredPalette = TRUE;

            if (video->info.mlt_surfaces == 0){
                if (video->SetColors)
                    video->SetColors(this, 0, vf->palette->ncolors, 
                            vf->palette->colors);
            }
            else{
                if (video->SetSurfaceColors)
                    video->SetSurfaceColors(this->screen, 0, vf->palette->ncolors, 
                            vf->palette->colors);
            }
        }

        /* Clear the surface to black */
        video->offset_x = 0;
        video->offset_y = 0;
        mode->offset = 0;
#ifdef _MGRM_PROCESSES
        if (mgIsServer) {
#endif
            GAL_SetClipRect(mode, NULL);
            GAL_ClearSurface(mode);
#ifdef _MGRM_PROCESSES
        }
#endif

#ifdef DEBUG_VIDEO
        fprintf(stderr,
                "NEWGAL: Requested mode: %dx%dx%d, obtained mode %dx%dx%d "
                "(offset %d)\n",
                width, height, bpp,
                mode->w, mode->h, mode->format->BitsPerPixel, mode->offset);
#endif
        mode->w = width;
        mode->h = height;
        GAL_SetClipRect(mode, NULL);
    }

    /* If we failed setting a video mode, return NULL... (Uh Oh!) */
    if ( mode == NULL ) {
        return(NULL);
    }

#ifndef _MG_MINIMALGDI
    license_get_processor_id(); 
#endif

    video->info.vfmt = GAL_VideoSurface->format;
    GAL_VideoSurface->video = current_video;

    return(GAL_PublicSurface);
}

/* 
 * Convert a surface into the video pixel format.
 */
GAL_Surface * GAL_DisplayFormat (GAL_Surface *surface)
{
    Uint32 flags;

    if (!GAL_PublicSurface) {
        GAL_SetError("NEWGAL: No video mode has been set.\n");
        return(NULL);
    }
    /* Set the flags appropriate for copying to display surface */
    flags  = (GAL_PublicSurface->flags&GAL_HWSURFACE);
#ifdef AUTORLE_DISPLAYFORMAT
    flags |= (surface->flags & (GAL_SRCCOLORKEY|GAL_SRCALPHA));
    flags |= GAL_RLEACCELOK;
#else
    flags |= surface->flags & (GAL_SRCCOLORKEY|GAL_SRCALPHA|GAL_RLEACCELOK);
#endif
    return(GAL_ConvertSurface(surface, GAL_PublicSurface->format, flags));
}

/*
 * Convert a surface into a format that's suitable for blitting to
 * the screen, but including an alpha channel.
 */
GAL_Surface *GAL_DisplayFormatAlpha(GAL_Surface *surface)
{
    GAL_PixelFormat *vf;
    GAL_PixelFormat *format;
    GAL_Surface *converted;
    Uint32 flags;
    /* default to ARGB8888 */
    Uint32 amask = 0xff000000;
    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;

    if (!GAL_PublicSurface) {
        GAL_SetError("NEWGAL: No video mode has been set.\n");
        return(NULL);
    }
    vf = GAL_PublicSurface->format;

    switch(vf->BytesPerPixel) {
        case 2:
            /* For XGY5[56]5, use, AXGY8888, where {X, Y} = {R, B}.
               For anything else (like ARGB4444) it doesn't matter
               since we have no special code for it anyway */
            if ( (vf->Rmask == 0x1f) &&
                    (vf->Bmask == 0xf800 || vf->Bmask == 0x7c00)) {
                rmask = 0xff;
                bmask = 0xff0000;
            }
            break;

        case 3:
        case 4:
            /* Keep the video format, as long as the high 8 bits are
               unused or alpha */
            if ( (vf->Rmask == 0xff) && (vf->Bmask == 0xff0000) ) {
                rmask = 0xff;
                bmask = 0xff0000;
            }
            break;

        default:
            /* We have no other optimised formats right now. When/if a new
               optimised alpha format is written, add the converter here */
            break;
    }
    format = GAL_AllocFormat(32, rmask, gmask, bmask, amask);
    flags = GAL_PublicSurface->flags & GAL_HWSURFACE;
    flags |= surface->flags & (GAL_SRCALPHA | GAL_RLEACCELOK);
    converted = GAL_ConvertSurface(surface, format, flags);
    GAL_FreeFormat(format);
    return(converted);
}

/*
 * Update a specific portion of the physical screen
 */
void GAL_UpdateRect(GAL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    GAL_VideoDevice *video = (GAL_VideoDevice *) screen->video;
    if (!video)
        return;

    if ( screen && (video->UpdateRects ||video->UpdateSurfaceRects)) {
        GAL_Rect rect;

        /* Perform some checking */
        if ( w == 0 )
            w = screen->w;
        if ( h == 0 )
            h = screen->h;

        if ( (int)(x+w) > screen->w ) 
            w = screen->w - x;
        if ( (int)(y+h) > screen->h )
            h = screen->h - x;

        /* Fill the rectangle */
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        GAL_UpdateRects(screen, 1, &rect);
    }
}

void GAL_UpdateRects (GAL_Surface *screen, int numrects, GAL_Rect *rects)
{
    GAL_VideoDevice *this =(GAL_VideoDevice *) screen->video;

    if (this->info.mlt_surfaces == 0){
        if (this && this->UpdateRects)
            this->UpdateRects(this, numrects, rects);
    }
    else{
        if(this && this->UpdateSurfaceRects){
            this->UpdateSurfaceRects(this, screen, numrects, rects);
        }
    }
}

static void SetPalette_logical(GAL_Surface *screen, GAL_Color *colors,
        int firstcolor, int ncolors)
{
    GAL_Palette *pal = screen->format->palette;

    if ( colors != (pal->colors + firstcolor) ) {
        memcpy(pal->colors + firstcolor, colors,
                ncolors * sizeof(*colors));
    }

    GAL_FormatChanged(screen);
}

static int SetPalette_physical(GAL_Surface *screen,
        GAL_Color *colors, int firstcolor, int ncolors)
{
    /* GAL_VideoDevice *video = current_video; */
    GAL_VideoDevice *video;
    int gotall = 1;
    if ( screen->video == NULL ) {
        return 0;
    }

    video = screen->video;

    if ( video->physpal ) {
        /* We need to copy the new colors, since we haven't
         * already done the copy in the logical set above.
         */
        memcpy(video->physpal->colors + firstcolor,
                colors, ncolors * sizeof(*colors));
    }

    if (video->info.mlt_surfaces == 0){
        if (video->SetColors)
            gotall = video->SetColors(video, firstcolor, ncolors, colors);
    }
    else
        if (video->SetSurfaceColors)
            gotall = video->SetSurfaceColors(screen, firstcolor, ncolors, colors);

    if ( ! gotall ) {
        /* The video flags shouldn't have GAL_HWPALETTE, and
           the video driver is responsible for copying back the
           correct colors into the video surface palette.
           */
        ;
    }

    return gotall;
}

/*
 * Set the physical and/or logical colormap of a surface:
 * Only the screen has a physical colormap. It determines what is actually
 * sent to the display.
 * The logical colormap is used to map blits to/from the surface.
 * 'which' is one or both of GAL_LOGPAL, GAL_PHYSPAL
 *
 * Return nonzero if all colours were set as requested, or 0 otherwise.
 */
int GAL_SetPalette(GAL_Surface *screen, int which,
        GAL_Color *colors, int firstcolor, int ncolors)
{
    GAL_Palette *pal;
    int gotall;
    int palsize;
    if ( ! current_video ) {
        return 0;
    }
    if ( screen->video == NULL ) {
        /* only screens have physical palettes */
        which &= ~GAL_PHYSPAL;
    } else if( (screen->flags & GAL_HWPALETTE) != GAL_HWPALETTE ) {
        /* hardware palettes required for split colormaps */
        which |= GAL_PHYSPAL | GAL_LOGPAL;
    }

    /* Verify the parameters */
    pal = screen->format->palette;
    if( !pal ) {
        return 0;    /* not a palettized surface */
    }
    gotall = 1;
    palsize = 1 << screen->format->BitsPerPixel;
    if ( ncolors > (palsize - firstcolor) ) {
        ncolors = (palsize - firstcolor);
        gotall = 0;
    }

    if (which & GAL_LOGPAL) {
        /*
         * Logical palette change: The actual screen isn't affected,
         * but the internal colormap is altered so that the
         * interpretation of the pixel values (for blits etc) is
         * changed.
         */
        SetPalette_logical(screen, colors, firstcolor, ncolors);
    }
    if (which & GAL_PHYSPAL) {
        /* GAL_VideoDevice *video = current_video; */
        GAL_VideoDevice *video = (GAL_VideoDevice *)(screen->video);
        /*
         * Physical palette change: This doesn't affect the
         * program's idea of what the screen looks like, but changes
         * its actual appearance.
         */
        if(!video)
            return gotall;    /* video not yet initialized */

        if(!video->physpal && !(which & GAL_LOGPAL) ) {
            /* Lazy physical palette allocation */
            int size;
            GAL_Palette *pp = malloc(sizeof(*pp));
            /* current_video->physpal = pp; */
            video->physpal = pp;
            pp->ncolors = pal->ncolors;
            size = pp->ncolors * sizeof(GAL_Color);
            pp->colors = malloc(size);
            memcpy(pp->colors, pal->colors, size);
        }
        if ( ! SetPalette_physical(screen,
                    colors, firstcolor, ncolors) ) {
            gotall = 0;
        }
    }
    screen->format->DitheredPalette = FALSE;

    return gotall;
}

int GAL_SetColors(GAL_Surface *screen, GAL_Color *colors, int firstcolor,
        int ncolors)
{
    return GAL_SetPalette(screen, GAL_LOGPAL | GAL_PHYSPAL,
            colors, firstcolor, ncolors);
}

/*
 * Clean up the video subsystem
 */
void GAL_VideoQuit (void)
{
    GAL_Surface *ready_to_go;

    if ( current_video ) {
        GAL_VideoDevice *video = current_video;
        GAL_VideoDevice *this  = current_video;
        /* Clean up the system video */
        video->VideoQuit (this);

        if (GAL_VideoSurface != NULL) {
            ready_to_go = GAL_VideoSurface;
            GAL_VideoSurface = NULL;
            GAL_FreeSurface (ready_to_go);
        }
        GAL_PublicSurface = NULL;
        /* Clean up miscellaneous memory */
        if ( video->physpal ) {
            free(video->physpal->colors);
            free(video->physpal);
            video->physpal = NULL;
        }
        /* Finish cleaning up video subsystem */
        video->free(this);
        current_video = NULL;
    }
    return;
}

void Slave_FreeSurface (GAL_Surface *surface)
{
    /* Free anything that's not NULL, and not the screen surface */
    if (surface == NULL) {
        return;
    }

    if (surface->format) {
        GAL_FreeFormat (surface->format);
        surface->format = NULL;
    }

    if (surface->map != NULL) {
        GAL_FreeBlitMap (surface->map);
        surface->map = NULL;
    }
    if ((surface->flags & GAL_HWSURFACE) == GAL_HWSURFACE) {
        GAL_VideoDevice *video =(GAL_VideoDevice*) surface->video;
        GAL_VideoDevice *this  =(GAL_VideoDevice*) surface->video;
        video->FreeHWSurface (this, surface);
    }
    if (surface->pixels &&
            ((surface->flags & GAL_PREALLOC) != GAL_PREALLOC)) {
        free (surface->pixels);
        surface->pixels = NULL;
    }
    free (surface);
}

void gal_SlaveVideoQuit (GAL_Surface * surface)
{
    GAL_VideoDevice *video;

    if(surface == NULL)
        return ;

    video = surface->video;
    if (video) {
        /* Clean up the system video */
        if (!(video->info.mlt_surfaces)){
            Slave_FreeSurface (surface);
            video->VideoQuit(video);
            if (video->screen != NULL) {
                video->screen = NULL;
            }

            /* Clean up miscellaneous memory */
            if ( video->physpal ) {
                free(video->physpal->colors);
                free(video->physpal);
                video->physpal = NULL;
            }

            /* Finish cleaning up video subsystem */

            video->free(video);
            video = NULL;
        }
        else{  
            video->DeleteSurface(video, surface);
            Slave_FreeSurface (surface);
        }
    }
    return;
}

static GAL_Surface *Slave_CreateSurface (GAL_VideoDevice *this, 
        int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface *surface;

    surface = (GAL_Surface *)malloc(sizeof(*surface));

    if ( surface == NULL ) {
        GAL_OutOfMemory(); 
        return(NULL);
    }
    surface->flags = GAL_HWSURFACE;
    if (Amask){
        surface->flags |= GAL_SRCPIXELALPHA;
    }
    surface->format = GAL_AllocFormat(depth, Rmask, Gmask, Bmask, Amask);

    if ( surface->format == NULL ) {
        free(surface);
        return(NULL);
    }

    surface->video = this;
    surface->w = 0;
    surface->h = 0;
    surface->pitch = GAL_CalculatePitch(surface);
    surface->pixels = NULL;
    surface->offset = 0;
    surface->hwdata = NULL;
    surface->map = NULL;
    surface->format_version = 0;
    GAL_SetClipRect(surface, NULL);

    /* Allocate an empty mapping */
    surface->map = GAL_AllocBlitMap();
    if ( surface->map == NULL ) {
        Slave_FreeSurface(surface);
        return(NULL);
    }

    /* The surface is ready to go */
    surface->refcount = 1;
    this->info.vfmt = surface->format;
    
    return (surface);
}

static GAL_Rect ** Slave_ListModes (GAL_VideoDevice *this, 
        GAL_PixelFormat *format, Uint32 flags)
{
    GAL_Rect **modes;
    GAL_Surface *surface = this->screen;

    modes = NULL;
    if (surface) {
        if (format == NULL) {
            format = surface->format;
        }

        modes = this->ListModes (this, format, flags);
    }
    return (modes);
}

static int Slave_GetVideoMode (GAL_VideoDevice *this, 
        int *w, int *h, int *BitsPerPixel, Uint32 flags)
{
    int table, b, i;
    int supported;
    int native_bpp;
    GAL_PixelFormat format;
    GAL_Rect **sizes;
    GAL_Surface *surface;

    if (this->screen == NULL)
        return(1);
    else {
        surface = this->screen;
    }

    /* Try the original video mode, get the closest depth */
    native_bpp = GAL_VideoModeOK (*w, *h, *BitsPerPixel, flags);

    if (native_bpp == 0)
        return 0;

    if (native_bpp == *BitsPerPixel) {
        return(1);
    }
    
    if (native_bpp > 0) {
        *BitsPerPixel = native_bpp;
        return(1);
    }

    /* No exact size match at any depth, look for closest match */
    memset(&format, 0, sizeof(format));
    supported = 0;
    table = ((*BitsPerPixel+7)/8)-1;
    GAL_closest_depths[table][0] = *BitsPerPixel;
    GAL_closest_depths[table][7] = surface->format->BitsPerPixel;
    
    for ( b = 0; !supported && GAL_closest_depths[table][b]; ++b ) {
        format.BitsPerPixel = GAL_closest_depths[table][b];
        sizes = Slave_ListModes(this, &format, flags);
    
        if ( sizes == (GAL_Rect **)0 ) {
            /* No sizes supported at this bit-depth */
            continue;
        }

        for ( i=0; sizes[i]; ++i ) {
            if ((sizes[i]->w < *w) || (sizes[i]->h < *h)) {
                if ( i > 0 ) {
                    --i;
                    *w = sizes[i]->w;
                    *h = sizes[i]->h;
                    *BitsPerPixel = GAL_closest_depths[table][b];
                    supported = 1;
                } else {
                    /* Largest mode too small... */;
                }
                break;
            }
        }

        if ( (i > 0) && ! sizes[i] ) {
            /* The smallest mode was larger than requested, OK */
            --i;
            *w = sizes[i]->w;
            *h = sizes[i]->h;
            *BitsPerPixel = GAL_closest_depths[table][b];
            supported = 1;
        }
    }

    if (!supported) {
        GAL_SetError("NEWGAL: No video mode large enough for "
                "the resolution specified.\n");
    }
    
    return(supported);
}

static GAL_Surface * Slave_SetVideoMode (GAL_VideoDevice *device, 
                GAL_Surface* surface, int width, int height, int bpp, Uint32 flags)
{
    GAL_VideoDevice *video, *this;
    int video_w;
    int video_h;
    int video_bpp;

    this = video = device;

    if (bpp == 0) {
        return NULL;
    }

    /* Get a good video mode, the closest one possible */
    video_w = width;
    video_h = height;
    video_bpp = bpp;
    
    if (!Slave_GetVideoMode(this, &video_w, &video_h, &video_bpp, flags)){
        GAL_SetError ("NEWGAL: Slave_GetVideoMode error, "
                "not supported video mode.\n");
        return(NULL);
    }
    /* Check the requested flags */
    /* There's no palette in > 8 bits-per-pixel mode */
    if (video_bpp > 8) {
        flags &= ~GAL_HWPALETTE;
    }

    if (video->physpal) {
        free (video->physpal->colors);
        free (video->physpal);
        video->physpal = NULL;
    }

    if (!(video->SetVideoMode(this, surface, 
                    video_w, video_h, video_bpp, flags))) {
        return NULL;
    }
    else {
        if ((surface->w < width) || (surface->h < height)) {
            GAL_SetError("NEWGAL: Slave video mode smaller than requested.\n");
            return (NULL);
        }
        /* If we have a palettized surface, create a default palette */
        if ( surface->format->palette ) {
            GAL_PixelFormat *vf = surface->format;
            GAL_DitherColors(vf->palette->colors, vf->BitsPerPixel);
            vf->DitheredPalette = TRUE;
            if (video->info.mlt_surfaces == 0){
                if (video->SetColors)
                    video->SetColors(this, 0, vf->palette->ncolors, 
                            vf->palette->colors);
            }
            else{
                if (video->SetSurfaceColors)
                    video->SetSurfaceColors(surface, 0, vf->palette->ncolors, 
                            vf->palette->colors);
            }
        }

        /* Clear the surface to black */
        video->offset_x = 0;
        video->offset_y = 0;
        surface->offset = 0;

        surface->w = width;
        surface->h = height;
        GAL_SetClipRect (surface, NULL);
    }

    video->info.vfmt = surface->format;
    surface->video = device;

    return surface;
}

GAL_Surface *gal_SlaveVideoInit(const char* driver_name, const char* mode)
{
    GAL_Surface* surface; 
    GAL_VideoDevice *video;
    GAL_PixelFormat vformat;
    unsigned int w, h, depth;

    /* Select the proper video driver */
    video = GAL_GetVideo(driver_name);

    if (video == NULL) {
        fprintf (stderr, "NEWGAL: Does not find the slave video engine: %s.\n", 
                        driver_name);
        return NULL;
    }

    /* Initialize the video subsystem */
    memset(&vformat, 0, sizeof(vformat));

    if ( video->VideoInit(video, &vformat) < 0 ) {
        fprintf (stderr, "NEWGAL: Can not init the slave video engine: %s.\n", 
                        driver_name);
        gal_SlaveVideoQuit (video->screen);
        return NULL;
    }

    surface = Slave_CreateSurface (video, 0, 0, vformat.BitsPerPixel,
            vformat.Rmask, vformat.Gmask, vformat.Bmask, 0);
    if (!surface) {
        fprintf (stderr, "NEWGAL: Create slave video surface failure.\n");
        return NULL;
    }

    /*get mode variables*/
    w = atoi (mode);
    h = atoi (strchr (mode, 'x') + 1);
    depth = atoi (strrchr (mode, '-') + 1);

    if (!(Slave_SetVideoMode(video, surface, w, h, depth, GAL_HWPALETTE))) {
        gal_SlaveVideoQuit (video->screen);
        fprintf (stderr, "NEWGAL: Set video mode failure.\n");
        return NULL;
    }
    return surface;
}
