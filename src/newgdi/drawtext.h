/*
** $Id: drawtext.h 10886 2008-08-29 12:01:54Z houhuihua $
**
** drawtext.h: The common text drawing routines.
**
** Copyright (C) 2003 ~ 2008 Feynman Software
** Copyright (C) 2000 ~ 2002 Wei Yongming.
*/

#ifndef GUI_GDI_DRAWTEXT_H
    #define GUI_GDI_DRAWTEXT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void _gdi_get_point_at_parallel(int x1, int y1, int x2, int y2, \
          int advance, int* parallel_x1, int* parallel_y1, int* parallel_x2, \
          int* parallel_y2, PDC pdc);

void _gdi_get_baseline_point (PDC pdc, int* x, int* y);

void _gdi_get_glyph_box_vertices (int x1, int y1, int x2, int y2, 
        POINT* pts, PDC pdc);

void _gdi_start_new_line (PDC pdc);

int _gdi_get_italic_added_width (LOGFONT* logfont);

typedef struct _BBOX
{
    int x, y;
    int w, h;
} BBOX;

int _gdi_get_glyph_advance (PDC pdc, Glyph32 glyph_value, 
            BOOL direction, int x, int y, int* adv_x, int* adv_y, BBOX* bbox);

int _gdi_draw_one_glyph (PDC pdc, Glyph32 glyph_value, BOOL direction,
            int x, int y, int* adv_x, int* adv_y);

int _gdi_get_null_glyph_advance (PDC pdc, int advance, BOOL direction, 
        int x, int y, int* adv_x, int* adv_y);

int _gdi_draw_null_glyph (PDC pdc, int advance, BOOL direction,
        int x, int y, int* adv_x, int* adv_y);

void _gdi_calc_glyphs_size_from_two_points (PDC pdc, int x0, int y0, 
        int x1, int y1, SIZE* size);

typedef BOOL (*CB_ONE_GLYPH) (void* context, Glyph32 glyph_value, 
        int glyph_type);

int _gdi_output_visual_glyphs(PDC pdc, Glyph32* visual_glyphs, int nr_glyphs, 
        BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context);

int _gdi_reorder_text (PDC pdc, const unsigned char* text, int text_len, 
                BOOL ta_state, CB_ONE_GLYPH cb_one_glyph, void* context);

int _gdi_reorder_text_break (PDC pdc, const unsigned char* text, int text_len, 
                BOOL direction, CB_ONE_GLYPH cb_one_glyph, void* context);

int _gdi_text_out (PDC pdc, int x, int y, 
                const unsigned char* text, int len, POINT* cur_pos);

int _gdi_tabbed_text_out (PDC pdc, int x, int y, 
                const unsigned char* text, int len, int tab_width,
                BOOL only_extent, POINT* cur_pos, SIZE* size);

int _gdi_get_text_extent (PDC pdc, const unsigned char* text, int len, 
                SIZE* size);

int _gdi_get_drawtext_extent (PDC pdc, const unsigned char* text, int len, 
                void* content, SIZE* size);

BOOL InitTextBitmapBuffer (void);
void TermTextBitmapBuffer (void);

typedef struct _DRAWTEXTEX2_CTXT
{
    PDC pdc;
    int start_x, start_y;
    int max_advance;
    int tab_width;
    int line_height;

    int x, y; 
    int advance;

    int nFormat;
    int nCount;
    Uint32 max_extent;
    BOOL   only_extent;
} DRAWTEXTEX2_CTXT;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_DRAWTEXT_H

