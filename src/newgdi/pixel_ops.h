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
** pixel_ops.h: inline pixel operations.
*/

#ifndef GUI_GDI_PIXELOPS_H
    #define GUI_GDI_PIXELOPS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL _dc_cliphline (const RECT* cliprc, int* px, int* py, int* pw);
BOOL _dc_clipvline (const RECT* cliprc, int* px, int* py, int* ph);

void _dc_drawvline (PDC pdc, int h);
void _set_pixel_helper (PDC pdc, int x, int y);

gal_pixel _dc_get_pixel_cursor (PDC pdc, int x, int y);

PCLIPRECT _dc_which_region_ban (PDC pdc, int y);

/* the callback functions of generators */
void _dc_set_pixel_noclip (void* context, int stepx, int stepy);
void _dc_set_pixel_clip (void* context, int x, int y);
void _dc_set_pixel_pair_clip (void* context, int x1, int x2, int y);
void _dc_draw_hline_clip (void* context, int x1, int x2, int y);
void _dc_draw_vline_clip (void* context, int y1, int y2, int x);

#ifdef _MGHAVE_ADV_2DAPI
void _dc_fill_hline_clip (void* context, int x1, int x2, int y);
void _dc_fill_span_brush_helper (PDC pdc, int x, int y, int w);
#endif

/* defined in bitmap.c */
void _dc_fillbox_clip (PDC pdc, const GAL_Rect* rect);
void _dc_fillbox_bmp_clip (PDC pdc, const GAL_Rect* rect, BITMAP* bmp);

/* defined in icon.c */
void _dc_restore_alpha_in_bitmap (const GAL_PixelFormat* format,
                void* dst_bits, unsigned int nr_bytes);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_PIXELOPS_H

