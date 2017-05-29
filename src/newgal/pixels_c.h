/*
**  $Id: pixels_c.h 12918 2010-05-14 05:37:10Z wanzheng $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

/* Useful functions and variables from pixel.c */

#include "blit.h"

/* Pixel format functions */
extern GAL_PixelFormat *GAL_AllocFormat(int bpp,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern GAL_PixelFormat *GAL_ReallocFormat(GAL_Surface *surface, int bpp,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern void GAL_FormatChanged(GAL_Surface *surface);
extern void GAL_FreeFormat(GAL_PixelFormat *format);

/* Blit mapping functions */
extern GAL_BlitMap *GAL_AllocBlitMap(void);
extern void GAL_InvalidateMap(GAL_BlitMap *map);
extern int GAL_MapSurface (GAL_Surface *src, GAL_Surface *dst);
extern void GAL_FreeBlitMap(GAL_BlitMap *map);

/* Miscellaneous functions */
extern Uint32 GAL_CalculatePitch(GAL_Surface *surface);
extern void GAL_DitherColors(GAL_Color *colors, int bpp);
extern Uint8 GAL_FindColor(GAL_Palette *pal, Uint8 r, Uint8 g, Uint8 b);
extern void GAL_ApplyGamma(Uint16 *gamma, GAL_Color *colors, GAL_Color *output, int ncolors);
