/*
** $Id: pixel_ops.h 11752 2009-06-13 08:27:26Z weiym $
**
** pixel_ops.h: inline pixel operations.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming.
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

