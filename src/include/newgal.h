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
** newgal.h: the head file of New Graphics Abstraction Layer.
** 
** Note: The architechture of new GAL is borrowed from SDL.
** Thank Sam Lantinga and many others for their great work.
** 
** Created date: 2001/10/03
*/

#ifndef GUI_NEWGAL_H
    #define GUI_NEWGAL_H

#include "gdi.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define DISABLE_THREADS
#define GAL_mutex       int

#define GAL_bool        BOOL
#define GAL_TRUE        TRUE
#define GAL_FALSE       FALSE
#define GAL_BYTEORDER   MGUI_BYTEORDER
#define GAL_LIL_ENDIAN  MGUI_LIL_ENDIAN
#define GAL_BIG_ENDIAN  MGUI_BIG_ENDIAN

#define GAL_OutOfMemory()   _ERR_PRINTF("NEWGAL: Out of memory\n")
#define GAL_SetError        _ERR_PRINTF
#define GAL_ClearError()

/* Transparency definitions: These define alpha as the opacity of a surface */
#define GAL_ALPHA_OPAQUE        255
#define GAL_ALPHA_TRANSPARENT   0

/*
 * Allocate a pixel format structure and fill it according to the given info.
 */
GAL_PixelFormat *GAL_AllocFormat (int bpp,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

/*
 * Free a previously allocated format structure
 */
void GAL_FreeFormat(GAL_PixelFormat *format);

/* typedef for private surface blitting functions */
struct GAL_Surface;
typedef int (*GAL_blit)(struct GAL_Surface *src, GAL_Rect *srcrect,
                        struct GAL_Surface *dst, GAL_Rect *dstrect);

/* This structure should be treated as read-only, except for 'pixels',
   which, if not NULL, contains the raw pixel data for the surface.
*/
typedef struct GAL_Surface {
    Uint32 flags;                       /* Read-only */
    GAL_PixelFormat *format;            /* Read-only */
    void *video;                        /* Read-only */
    int w, h;                           /* Read-only */
    /* VW[2018-01-18]: For 64b, use signed int instead of Uint32 for pitch. */
    int pitch;                          /* Read-only */
    void *pixels;                       /* Read-write */
    int offset;                         /* Private */

    /* Hardware-specific surface info */
    struct private_hwdata *hwdata;

    /* clipping information */
    GAL_Rect clip_rect;                 /* Read-only */

    /* info for fast blit mapping to other surfaces */
    struct GAL_BlitMap *map;             /* Private */

    /* format version, bumped at every change to invalidate blit maps */
    unsigned int format_version;        /* Private */

    /* Reference count -- used when freeing surface */
    int refcount;                       /* Read-mostly */
} GAL_Surface;

/* These are the currently supported flags for the GAL_surface */
/* Available for GAL_CreateRGBSurface() or GAL_SetVideoMode() */
#define GAL_SWSURFACE        MEMDC_FLAG_SWSURFACE   /* Surface is in system memory */
#define GAL_HWSURFACE        MEMDC_FLAG_HWSURFACE   /* Surface is in video memory */
#define GAL_ASYNCBLIT        0x00000004             /* Use asynchronous blits if possible */
/* Available for GAL_SetVideoMode() */
#define GAL_ANYFORMAT        0x10000000             /* Allow any video depth/pixel-format */
#define GAL_HWPALETTE        0x20000000             /* Surface has exclusive palette */
#define GAL_DOUBLEBUF        0x40000000             /* Set up double-buffered video mode */
#define GAL_FULLSCREEN       0x80000000             /* Surface is a full screen display */
/* Used internally (read-only) */
#define GAL_HWACCEL          0x00000100                 /* Blit uses hardware acceleration */
#define GAL_SRCCOLORKEY      MEMDC_FLAG_SRCCOLORKEY     /* Blit uses a source color key */
#define GAL_RLEACCELOK       0x00002000                 /* Private flag */
#define GAL_RLEACCEL         MEMDC_FLAG_RLEACCEL        /* Surface is RLE encoded */
#define GAL_SRCALPHA         MEMDC_FLAG_SRCALPHA        /* Blit uses source alpha blending */
#define GAL_SRCPIXELALPHA    MEMDC_FLAG_SRCPIXELALPHA   /* Blit uses source per-pixel alpha blending */
#define GAL_PREALLOC         0x01000000                 /* Surface uses preallocated memory */

/* Evaluates to true if the surface needs to be locked before access */
#define GAL_MUSTLOCK(surface)   \
  (surface->offset ||       \
  ((surface->flags & (GAL_HWSURFACE|GAL_ASYNCBLIT|GAL_RLEACCEL)) != 0))

/* Useful for determining the video hardware capabilities */
typedef struct {
    Uint32 hw_available :1;         /* Flag: Can you create hardware surfaces? */
    Uint32 mlt_surfaces :1;         /* Flag: Does VideoInit return different surfaces? */
    Uint32 UnusedBits   :8;         /* Flag: Can you talk to a window manager? */
    Uint32 blit_hw      :1;         /* Flag: Accelerated blits HW --> HW */
    Uint32 blit_hw_CC   :1;         /* Flag: Accelerated blits with Colorkey */
    Uint32 blit_hw_A    :1;         /* Flag: Accelerated blits with Alpha */
    Uint32 blit_sw      :1;         /* Flag: Accelerated blits SW --> HW */
    Uint32 blit_sw_CC   :1;         /* Flag: Accelerated blits with Colorkey */
    Uint32 blit_sw_A    :1;         /* Flag: Accelerated blits with Alpha */
    Uint32 blit_fill    :1;         /* Flag: Accelerated color fill */
    Uint32 UnusedBits3  :16;
    Uint32 video_mem;               /* The total amount of video memory (in K) */
    GAL_PixelFormat *vfmt;          /* Value: The format of the video surface */
} GAL_VideoInfo;

/* flags for GAL_SetPalette() */
#define GAL_LOGPAL 0x01
#define GAL_PHYSPAL 0x02

/* Function prototypes */

/* These functions are used internally, and should not be used unless you
 * have a specific need to specify the video driver you want to use.
 * You should normally use GAL_Init() or GAL_InitSubSystem().
 *
 * GAL_VideoInit() initializes the video subsystem -- sets up a connection
 * to the window manager, etc, and determines the current video mode and
 * pixel format, but does not initialize a window or graphics mode.
 * Note that event handling is activated by this routine.
 *
 * If you use both sound and video in your application, you need to call
 * GAL_Init() before opening the sound device, otherwise under Win32 DirectX,
 * you won't be able to set full-screen display modes.
 */
extern int GAL_VideoInit (const char *driver_name, Uint32 flags);
extern void GAL_VideoQuit (void);

extern void gal_SlaveVideoQuit (GAL_Surface * surface);
extern GAL_Surface *gal_SlaveVideoInit(const char* driver_name, const char* mode);

/* This function fills the given character buffer with the name of the
 * video driver, and returns a pointer to it if the video driver has
 * been initialized.  It returns NULL if no driver has been initialized.
 */
extern char *GAL_VideoDriverName (char *namebuf, int maxlen);

/*
 * This function returns a pointer to the current display surface.
 * If GAL is doing format conversion on the display surface, this
 * function returns the publicly visible surface, not the real video
 * surface.
 */
extern GAL_Surface * GAL_GetVideoSurface (void);

/*
 * This function returns a read-only pointer to information about the
 * video hardware.  If this is called before GAL_SetVideoMode(), the 'vfmt'
 * member of the returned structure will contain the pixel format of the
 * "best" video mode.
 */
extern const GAL_VideoInfo * GAL_GetVideoInfo (void);

/* 
 * Check to see if a particular video mode is supported.
 * It returns 0 if the requested mode is not supported under any bit depth,
 * or returns the bits-per-pixel of the closest available mode with the
 * given width and height.  If this bits-per-pixel is different from the
 * one used when setting the video mode, GAL_SetVideoMode() will succeed,
 * but will emulate the requested bits-per-pixel with a shadow surface.
 *
 * The arguments to GAL_VideoModeOK() are the same ones you would pass to
 * GAL_SetVideoMode()
 */
extern int GAL_VideoModeOK (int width, int height, int bpp, Uint32 flags);

/*
 * Return a pointer to an array of available screen dimensions for the
 * given format and video flags, sorted largest to smallest.  Returns 
 * NULL if there are no dimensions available for a particular format, 
 * or (GAL_Rect **)-1 if any dimension is okay for the given format.
 *
 * If 'format' is NULL, the mode list will be for the format given 
 * by GAL_GetVideoInfo()->vfmt
 */
extern GAL_Rect ** GAL_ListModes (GAL_PixelFormat *format, Uint32 flags);

/*
 * Set up a video mode with the specified width, height and bits-per-pixel.
 *
 * If 'bpp' is 0, it is treated as the current display bits per pixel.
 *
 * If GAL_ANYFORMAT is set in 'flags', the GAL library will try to set the
 * requested bits-per-pixel, but will return whatever video pixel format is
 * available.  The default is to emulate the requested pixel format if it
 * is not natively available.
 *
 * If GAL_HWSURFACE is set in 'flags', the video surface will be placed in
 * video memory, if possible, and you may have to call GAL_LockSurface()
 * in order to access the raw framebuffer.  Otherwise, the video surface
 * will be created in system memory.
 *
 * If GAL_ASYNCBLIT is set in 'flags', GAL will try to perform rectangle
 * updates asynchronously, but you must always lock before accessing pixels.
 * GAL will wait for updates to complete before returning from the lock.
 *
 * If GAL_HWPALETTE is set in 'flags', the GAL library will guarantee
 * that the colors set by GAL_SetColors() will be the colors you get.
 * Otherwise, in 8-bit mode, GAL_SetColors() may not be able to set all
 * of the colors exactly the way they are requested, and you should look
 * at the video surface structure to determine the actual palette.
 * If GAL cannot guarantee that the colors you request can be set, 
 * i.e. if the colormap is shared, then the video surface may be created
 * under emulation in system memory, overriding the GAL_HWSURFACE flag.
 *
 * If GAL_FULLSCREEN is set in 'flags', the GAL library will try to set
 * a fullscreen video mode.  The default is to create a windowed mode
 * if the current graphics system has a window manager.
 * If the GAL library is able to set a fullscreen video mode, this flag 
 * will be set in the surface that is returned.
 *
 * If GAL_DOUBLEBUF is set in 'flags', the GAL library will try to set up
 * two surfaces in video memory and swap between them when you call 
 * GAL_Flip().  This is usually slower than the normal single-buffering
 * scheme, but prevents "tearing" artifacts caused by modifying video 
 * memory while the monitor is refreshing.  It should only be used by 
 * applications that redraw the entire screen on every update.
 *
 * If GAL_RESIZABLE is set in 'flags', the GAL library will allow the
 * window manager, if any, to resize the window at runtime.  When this
 * occurs, GAL will send a GAL_VIDEORESIZE event to you application,
 * and you must respond to the event by re-calling GAL_SetVideoMode()
 * with the requested size (or another size that suits the application).
 *
 * If GAL_NOFRAME is set in 'flags', the GAL library will create a window
 * without any title bar or frame decoration.  Fullscreen video modes have
 * this flag set automatically.
 *
 * This function returns the video framebuffer surface, or NULL if it fails.
 *
 * If you rely on functionality provided by certain video flags, check the
 * flags of the returned surface to make sure that functionality is available.
 * GAL will fall back to reduced functionality if the exact flags you wanted
 * are not available.
 */
extern GAL_Surface *GAL_SetVideoMode
                        (int width, int height, int bpp, Uint32 flags);

/*
 * Makes sure the given list of rectangles is updated on the given screen.
 * If 'x', 'y', 'w' and 'h' are all 0, GAL_UpdateRect will update the entire
 * screen.
 * These functions should not be called while 'screen' is locked.
 */
extern void GAL_UpdateRects
                (GAL_Surface *screen, int numrects, GAL_Rect *rects);
extern void GAL_UpdateRect
                (GAL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

#if 0
/*
 * On hardware that supports double-buffering, this function sets up a flip
 * and returns.  The hardware will wait for vertical retrace, and then swap
 * video buffers before the next video surface blit or lock will return.
 * On hardware that doesn not support double-buffering, this is equivalent
 * to calling GAL_UpdateRect(screen, 0, 0, 0, 0);
 * The GAL_DOUBLEBUF flag must have been passed to GAL_SetVideoMode() when
 * setting the video mode for this function to perform hardware flipping.
 * This function returns 0 if successful, or -1 if there was an error.
 */
extern int GAL_Flip (GAL_Surface *screen);
#endif

/*
 * Sets a portion of the colormap for the given 8-bit surface.  If 'surface'
 * is not a palettized surface, this function does nothing, returning 0.
 * If all of the colors were set as passed to GAL_SetColors(), it will
 * return 1.  If not all the color entries were set exactly as given,
 * it will return 0, and you should look at the surface palette to
 * determine the actual color palette.
 *
 * When 'surface' is the surface associated with the current display, the
 * display colormap will be updated with the requested colors.  If 
 * GAL_HWPALETTE was set in GAL_SetVideoMode() flags, GAL_SetColors()
 * will always return 1, and the palette is guaranteed to be set the way
 * you desire, even if the window colormap has to be warped or run under
 * emulation.
 */
extern int GAL_SetColors (GAL_Surface *surface, 
                        GAL_Color *colors, int firstcolor, int ncolors);

/*
 * Sets a portion of the colormap for a given 8-bit surface.
 * 'flags' is one or both of:
 * GAL_LOGPAL  -- set logical palette, which controls how blits are mapped
 *                to/from the surface,
 * GAL_PHYSPAL -- set physical palette, which controls how pixels look on
 *                the screen
 * Only screens have physical palettes. Separate change of physical/logical
 * palettes is only possible if the screen has GAL_HWPALETTE set.
 *
 * The return value is 1 if all colours could be set as requested, and 0
 * otherwise.
 *
 * GAL_SetColors() is equivalent to calling this function with
 *     flags = (GAL_LOGPAL|GAL_PHYSPAL).
 */
extern int GAL_SetPalette (GAL_Surface *surface, int flags,
                                   GAL_Color *colors, int firstcolor,
                                   int ncolors);

extern void GAL_DitherColors(GAL_Color *colors, int bpp);

/*
 * Maps an RGB triple to an opaque pixel value for a given pixel format
 */
extern Uint32 GAL_MapRGB (GAL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b);

/*
 * Maps an RGBA quadruple to a pixel value for a given pixel format
 */
extern Uint32 GAL_MapRGBA (GAL_PixelFormat *format,
                                   Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/*
 * Maps a pixel value into the RGB components for a given pixel format
 */
extern void GAL_GetRGB (Uint32 pixel, GAL_PixelFormat *fmt,
                                Uint8 *r, Uint8 *g, Uint8 *b);

/*
 * Maps a pixel value into the RGBA components for a given pixel format
 */
extern void GAL_GetRGBA (Uint32 pixel, GAL_PixelFormat *fmt,
                                 Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);

/*
 * Match an RGB value to a particular palette index
 */
extern Uint8 GAL_FindColor(GAL_Palette *pal, Uint8 r, Uint8 g, Uint8 b);

/*
 * Allocate and free an RGB surface (must be called after GAL_SetVideoMode)
 * If the depth is 4 or 8 bits, an empty palette is allocated for the surface.
 * If the depth is greater than 8 bits, the pixel format is set using the
 * flags '[RGB]mask'.
 * If the function runs out of memory, it will return NULL.
 *
 * The 'flags' tell what kind of surface to create.
 * GAL_SWSURFACE means that the surface should be created in system memory.
 * GAL_HWSURFACE means that the surface should be created in video memory,
 * with the same format as the display surface.  This is useful for surfaces
 * that will not change much, to take advantage of hardware acceleration
 * when being blitted to the display surface.
 * GAL_ASYNCBLIT means that GAL will try to perform asynchronous blits with
 * this surface, but you must always lock it before accessing the pixels.
 * GAL will wait for current blits to finish before returning from the lock.
 * GAL_SRCCOLORKEY indicates that the surface will be used for colorkey blits.
 * If the hardware supports acceleration of colorkey blits between
 * two surfaces in video memory, GAL will try to place the surface in
 * video memory. If this isn't possible or if there is no hardware
 * acceleration available, the surface will be placed in system memory.
 * GAL_SRCALPHA means that the surface will be used for alpha blits and 
 * if the hardware supports hardware acceleration of alpha blits between
 * two surfaces in video memory, to place the surface in video memory
 * if possible, otherwise it will be placed in system memory.
 * If the surface is created in video memory, blits will be _much_ faster,
 * but the surface format must be identical to the video surface format,
 * and the only way to access the pixels member of the surface is to use
 * the GAL_LockSurface() and GAL_UnlockSurface() calls.
 * If the requested surface actually resides in video memory, GAL_HWSURFACE
 * will be set in the flags member of the returned surface.  If for some
 * reason the surface could not be placed in video memory, it will not have
 * the GAL_HWSURFACE flag set, and will be created in system memory instead.
 */
#define GAL_AllocSurface    GAL_CreateRGBSurface
extern GAL_Surface *GAL_CreateRGBSurface
                        (Uint32 flags, int width, int height, int depth, 
                        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern GAL_Surface *GAL_CreateRGBSurfaceFrom (void *pixels,
                        int width, int height, int depth, int pitch,
                        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern void GAL_FreeSurface (GAL_Surface *surface);

typedef struct REQ_HWSURFACE {
    /* for allocation */
    Uint32 w;
    Uint32 h;
    Uint32 pitch;

    /* for free */
    Uint32 offset;
    void* bucket;
} REQ_HWSURFACE;

typedef struct REP_HWSURFACE {
    Uint32 offset;
    Uint32 pitch;
    void*  bucket;
} REP_HWSURFACE;

#ifdef _MGGAL_NEXUS
typedef struct _REQ_NEXUS_GETSURFACE {
    int width;
    int height;
} REQ_NEXUS_GETSURFACE;

#include "../newgal/nexus/nexusvideo_pri.h"
typedef struct _REP_NEXUS_GETSURFACE {
    int width;
    int height;
    unsigned int pitch;
    NexusPrivate_HWSurface_hwdata hwdata;
} REP_NEXUS_GETSURFACE;
#endif

#ifdef _MGGAL_SIGMA8654
typedef struct _REQ_SIGMA8654_GETSURFACE {
    int width;
    int height;
} REQ_SIGMA8654_GETSURFACE;

#include "../newgal/sigma8654/sigma8654_pri.h"
typedef struct _REP_SIGMA8654_GETSURFACE {
    int width;
    int height;
    unsigned int pitch;
    Sigma8654Private_HWSurface_hwdata hwdata;
} REP_SIGMA8654_GETSURFACE;
#endif

#ifndef _MGRM_THREADS
extern void GAL_RequestHWSurface (const REQ_HWSURFACE* request, REP_HWSURFACE* reply);
#endif

#if 0
/*
 * GAL_LockSurface() sets up a surface for directly accessing the pixels.
 * Between calls to GAL_LockSurface()/GAL_UnlockSurface(), you can write
 * to and read from 'surface->pixels', using the pixel format stored in 
 * 'surface->format'.  Once you are done accessing the surface, you should 
 * use GAL_UnlockSurface() to release it.
 *
 * Not all surfaces require locking.  If GAL_MUSTLOCK(surface) evaluates
 * to 0, then you can read and write to the surface at any time, and the
 * pixel format of the surface will not change.  In particular, if the
 * GAL_HWSURFACE flag is not given when calling GAL_SetVideoMode(), you
 * will not need to lock the display surface before accessing it.
 * 
 * No operating system or library calls should be made between lock/unlock
 * pairs, as critical system locks may be held during this time.
 *
 * GAL_LockSurface() returns 0, or -1 if the surface couldn't be locked.
 */
extern int GAL_LockSurface (GAL_Surface *surface);
extern void GAL_UnlockSurface (GAL_Surface *surface);
#endif

/*
 * Sets the color key (transparent pixel) in a blittable surface.
 * If 'flag' is GAL_SRCCOLORKEY (optionally OR'd with GAL_RLEACCEL), 
 * 'key' will be the transparent pixel in the source image of a blit.
 * GAL_RLEACCEL requests RLE acceleration for the surface if present,
 * and removes RLE acceleration if absent.
 * If 'flag' is 0, this function clears any current color key.
 * This function returns 0, or -1 if there was an error.
 */
extern int GAL_SetColorKey (GAL_Surface *surface, Uint32 flag, Uint32 key);

/*
 * This function sets the alpha value for the entire surface, as opposed to
 * using the alpha component of each pixel. This value measures the range
 * of transparency of the surface, 0 being completely transparent to 255
 * being completely opaque. An 'alpha' value of 255 causes blits to be
 * opaque, the source pixels copied to the destination (the default). Note
 * that per-surface alpha can be combined with colorkey transparency.
 *
 * If 'flag' is 0, alpha blending is disabled for the surface.
 * If 'flag' is GAL_SRCALPHA, alpha blending is enabled for the surface.
 * OR:ing the flag with GAL_RLEACCEL requests RLE acceleration for the
 * surface; if GAL_RLEACCEL is not specified, the RLE accel will be removed.
 */
extern int GAL_SetAlpha (GAL_Surface *surface, Uint32 flag, Uint8 alpha);

/*
 * Sets the clipping rectangle for the destination surface in a blit.
 *
 * If the clip rectangle is NULL, clipping will be disabled.
 * If the clip rectangle doesn't intersect the surface, the function will
 * return GAL_FALSE and blits will be completely clipped.  Otherwise the
 * function returns GAL_TRUE and blits to the surface will be clipped to
 * the intersection of the surface area and the clipping rectangle.
 *
 * Note that blits are automatically clipped to the edges of the source
 * and destination surfaces.
 */
extern GAL_bool GAL_SetClipRect (GAL_Surface *surface, GAL_Rect *rect);

/*
 * Gets the clipping rectangle for the destination surface in a blit.
 * 'rect' must be a pointer to a valid rectangle which will be filled
 * with the correct values.
 */
extern void GAL_GetClipRect (GAL_Surface *surface, GAL_Rect *rect);

/*
 * Creates a new surface of the specified format, and then copies and maps 
 * the given surface to it so the blit of the converted surface will be as 
 * fast as possible.  If this function fails, it returns NULL.
 *
 * The 'flags' parameter is passed to GAL_CreateRGBSurface() and has those 
 * semantics.  You can also pass GAL_RLEACCEL in the flags parameter and
 * GAL will try to RLE accelerate colorkey and alpha blits in the resulting
 * surface.
 *
 * This function is used internally by GAL_DisplayFormat().
 */
extern GAL_Surface *GAL_ConvertSurface
                        (GAL_Surface *src, GAL_PixelFormat *fmt, Uint32 flags);

/*
 * This performs a fast blit from the source surface to the destination
 * surface.  It assumes that the source and destination rectangles are
 * the same size.  If either 'srcrect' or 'dstrect' are NULL, the entire
 * surface (src or dst) is copied.  The final blit rectangles are saved
 * in 'srcrect' and 'dstrect' after all clipping is performed.
 * If the blit is successful, it returns 0, otherwise it returns -1.
 *
 * The blit function should not be called on a locked surface.
 *
 * The blit semantics for surfaces with and without alpha and colorkey
 * are defined as follows:
 *
 * RGBA->RGB:
 *     GAL_SRCALPHA set:
 *         alpha-blend (using alpha-channel).
 *         GAL_SRCCOLORKEY ignored.
 *     GAL_SRCALPHA not set:
 *         copy RGB.
 *         if GAL_SRCCOLORKEY set, only copy the pixels matching the
 *         RGB values of the source colour key, ignoring alpha in the
 *         comparison.
 * 
 * RGB->RGBA:
 *     GAL_SRCALPHA set:
 *         alpha-blend (using the source per-surface alpha value);
 *         set destination alpha to opaque.
 *     GAL_SRCALPHA not set:
 *         copy RGB, set destination alpha to opaque.
 *     both:
 *         if GAL_SRCCOLORKEY set, only copy the pixels matching the
 *         source colour key.
 * 
 * RGBA->RGBA:
 *     GAL_SRCALPHA set:
 *         alpha-blend (using the source alpha channel) the RGB values;
 *         leave destination alpha untouched. [Note: is this correct?]
 *         GAL_SRCCOLORKEY ignored.
 *     GAL_SRCALPHA not set:
 *         copy all of RGBA to the destination.
 *         if GAL_SRCCOLORKEY set, only copy the pixels matching the
 *         RGB values of the source colour key, ignoring alpha in the
 *         comparison.
 * 
 * RGB->RGB: 
 *     GAL_SRCALPHA set:
 *         alpha-blend (using the source per-surface alpha value).
 *     GAL_SRCALPHA not set:
 *         copy RGB.
 *     both:
 *         if GAL_SRCCOLORKEY set, only copy the pixels matching the
 *         source colour key.
 *
 * If either of the surfaces were in video memory, and the blit returns -2,
 * the video memory was lost, so it should be reloaded with artwork and 
 * re-blitted:
        while ( GAL_BlitSurface(image, imgrect, screen, dstrect) == -2 ) {
                while ( GAL_LockSurface(image) < 0 )
                        Sleep(10);
                -- Write image pixels to image->pixels --
                GAL_UnlockSurface(image);
        }
 * This happens under DirectX 5.0 when the system switches away from your
 * fullscreen application.  The lock will also fail until you have access
 * to the video memory again.
 */
/* You should call GAL_BlitSurface() unless you know exactly how GAL
   blitting works internally and how to use the other blit functions.
*/
#define GAL_BlitSurface GAL_UpperBlit

/* This is the public blit function, GAL_BlitSurface(), and it performs
   rectangle validation and clipping before passing it to GAL_LowerBlit()
*/
extern int GAL_UpperBlit (GAL_Surface *src, GAL_Rect *srcrect,
                         GAL_Surface *dst, GAL_Rect *dstrect);
/* This is a semi-private blit function and it performs low-level surface
   blitting only.
*/
extern int GAL_LowerBlit (GAL_Surface *src, GAL_Rect *srcrect,
                         GAL_Surface *dst, GAL_Rect *dstrect);

/*
 * This function performs a fast fill of the given rectangle with 'color'
 * The given rectangle is clipped to the destination surface clip area
 * and the final fill rectangle is saved in the passed in pointer.
 * If 'dstrect' is NULL, the whole surface will be filled with 'color'
 * The color should be a pixel of the format used by the surface, and 
 * can be generated by the GAL_MapRGB() function.
 * This function returns 0 on success, or -1 on error.
 */
extern int GAL_FillRect
                (GAL_Surface *dst, const GAL_Rect *dstrect, Uint32 color);

/*
 * This function returns the size of pad-aligned box bitmap.
 */
extern Uint32 GAL_GetBoxSize
                (GAL_Surface *src, Uint32 w, Uint32 h, Uint32* pitch_p);

/*
 * This function copies pixels in a rect from the surface to a box.
 * You can calculate the box size like:
 *
 *  size = GAL_GetBoxSize (src, &rect, NULL);
 *
 */
extern int GAL_GetBox (GAL_Surface *src, const GAL_Rect* srcrect, BITMAP* box);

/*
 * This function put pixels in a box to the rect on the surface.
 *
 * Note that the size of box should be equal to:
 *
 *  size = GAL_GetBoxSize (src, &rect, NULL);
 *
 */
extern int GAL_PutBox (GAL_Surface *dst, const GAL_Rect* dstrect, BITMAP* box);

/* 
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer, suitable for fast
 * blitting onto the display surface.  It calls GAL_ConvertSurface()
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern GAL_Surface * GAL_DisplayFormat (GAL_Surface *surface);

/* 
 * This function takes a surface and copies it to a new surface of the
 * pixel format and colors of the video framebuffer (if possible),
 * suitable for fast alpha blitting onto the display surface.
 * The new surface will always have an alpha channel.
 *
 * If you want to take advantage of hardware colorkey or alpha blit
 * acceleration, you should set the colorkey and alpha value before
 * calling this function.
 *
 * If the conversion fails or runs out of memory, it returns NULL
 */
extern GAL_Surface * GAL_DisplayFormatAlpha (GAL_Surface *surface);

/* Not in public API at the moment - do not use! */
extern int GAL_SoftStretch (GAL_Surface *src, GAL_Rect *srcrect,
                                    GAL_Surface *dst, GAL_Rect *dstrect);

extern GAL_Surface* __gal_screen;

#define WIDTHOFPHYGC        (__gal_screen->w)
#define HEIGHTOFPHYGC       (__gal_screen->h)
#define BYTESPERPHYPIXEL    (__gal_screen->format->BytesPerPixel)
#define BITSPERPHYPIXEL     (__gal_screen->format->BitsPerPixel)

#define GAL_BytesPerPixel(surface)  (surface->format->BytesPerPixel)
#define GAL_BitsPerPixel(surface)   (surface->format->BitsPerPixel)
#define GAL_Width(surface)          (surface->w)
#define GAL_Height(surface)         (surface->h)
#define GAL_RMask(surface)          (surface->format->Rmask)
#define GAL_GMask(surface)          (surface->format->Gmask)
#define GAL_BMask(surface)          (surface->format->Bmask)
#define GAL_AMask(surface)          (surface->format->Amask)

extern BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* bpp);
extern int mg_InitGAL (void);
extern BYTE*  gal_PutPixelAlphaChannel (GAL_Surface* dst, BYTE* dstrow, Uint32 pixel, MYBITMAP_CONTXT* mybmp);
extern BYTE*  gal_PutPixelKey (GAL_Surface* dst, BYTE* dstrow, Uint32 pixel, MYBITMAP_CONTXT* mybmp);
extern BYTE*  gal_PutPixelKeyAlphaChannel (GAL_Surface* dst, BYTE* dstrow, Uint32 pixel, MYBITMAP_CONTXT* mybmp);
extern BYTE*  gal_PutPixelAlpha (GAL_Surface* dst, BYTE* dstrow, Uint32 pixel, MYBITMAP_CONTXT* mybmp);
extern BYTE*  gal_PutPixelKeyAlpha (GAL_Surface* dst, BYTE* dstrow, Uint32 pixel, MYBITMAP_CONTXT* mybmp);


#define mg_TerminateGAL GAL_VideoQuit

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_NEWGAL_H */

