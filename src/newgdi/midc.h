/*
** $Id: midc.h 7359 2007-08-16 05:08:40Z xgwang $
**
** midc.h: dc operations for advanced 2D graphics APIs.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
*/

#ifndef MIDC_H
#define MIDC_H 1

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void _dc_fill_spans (PDC pdc, Span* spans, int nspans, BOOL sorted);
void _dc_fill_spans_brush (PDC pdc, Span* spans, int nspans, BOOL sorted);

static inline void _dc_fill_rect (PDC pdc, int x, int y, int w, int h)
{
    GAL_Rect rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    _dc_fillbox_clip (pdc, &rect);
}

void _dc_dump_spans (Span* spans, int nspans);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
