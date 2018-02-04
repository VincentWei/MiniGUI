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
