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
** pixel_ops.c: pixel, horizontal, and vertical line operations
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"

BOOL _dc_cliphline (const RECT* cliprc, int* px, int* py, int* pw)
{
    if ( (*px >= cliprc->right) || (*py >= cliprc->bottom) || 
            (*px + *pw < cliprc->left) || (*py < cliprc->top) )
        return FALSE;

    if ( (*px >= cliprc->left) && (*py >= cliprc->top) && 
            (*px + *pw < cliprc->right) && (*py < cliprc->bottom) )
        return TRUE;
            
    if (*px < cliprc->left) {
        *pw -= cliprc->left - *px;
        *px = cliprc->left;
    }
    if (*px + *pw >= cliprc->right)
        *pw = cliprc->right - *px;

    if (*pw <= 0)
        return FALSE;

    return TRUE;        
}

BOOL _dc_clipvline (const RECT* cliprc, int* px, int* py, int* ph)
{
    if ( (*px >= cliprc->right) || (*py >= cliprc->bottom) || 
            (*px < cliprc->left) || (*py + *ph < cliprc->top) )
        return FALSE;

    if ( (*px >= cliprc->left) && (*py >= cliprc->top) && 
            (*px < cliprc->right) && (*py + *ph < cliprc->bottom) )
        return TRUE;
            
    if (*py < cliprc->top) {
        *ph -= cliprc->top - *py;
        *py = cliprc->top;
    }
    if (*py + *ph >= cliprc->bottom)    
        *ph = cliprc->bottom - *py;

    if (*ph <= 0)
        return FALSE;

    return TRUE;        
}

void _dc_drawvline (PDC pdc, int h)
{
    while (h > 0) {
        pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
        _dc_step_y (pdc, pdc->step);
        h -= pdc->step;
    }
}

/* find the ban in which the scanline lies */
PCLIPRECT _dc_which_region_ban (PDC pdc, int y)
{
    CLIPRGN* region = &pdc->ecrgn;
    PCLIPRECT cliprect;

    /* check with bounding rect of clipping region */
    if (y >= region->tail->rc.bottom || y < region->head->rc.top) {
        pdc->cur_ban = NULL;
        goto ret;
    }

    if (pdc->cur_ban)
        cliprect = pdc->cur_ban;
    else
        cliprect = region->head;

    /* find the ban in which this point lies */
    if (y < cliprect->rc.top) { /* backward */
        while (cliprect && y < cliprect->rc.top) {
            cliprect = cliprect->prev;
        }

        if (cliprect) {
            int top;
            top = cliprect->rc.top;
            while (cliprect && top == cliprect->rc.top) {
                cliprect = cliprect->prev;
            }

            if (cliprect)
                cliprect = cliprect->next;
            else
                cliprect = region->head;
        }
    }
    else { /* forward */
        while (cliprect && y >= cliprect->rc.bottom) {
            cliprect = cliprect->next;
        }
    }

    pdc->cur_ban = cliprect;
ret:
    return pdc->cur_ban;
}

#define INRECT(r, x, y) \
      ( ( ((r).right >  x)) && \
      ( ((r).left <= x)) && \
      ( ((r).bottom >  y)) && \
      ( ((r).top <= y)) )

void _dc_set_pixel_clip (void* context, int x, int y)
{
    PDC pdc = (PDC)context;
    PCLIPRECT cliprect;

    if (_dc_which_region_ban (pdc, y)) {
        int top;

        /* draw in this ban */
        cliprect = pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (INRECT (pdc->rc_output, x, y) && INRECT (cliprect->rc, x, y)) {
                pdc->move_to (pdc, x, y);
                pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
                break;
            }

            cliprect = cliprect->next;
        }
    }
}

void _dc_set_pixel_noclip (void* context, int stepx, int stepy)
{
    PDC pdc = (PDC)context;

    if (stepx) pdc->step_x (pdc, stepx);
    if (stepy) _dc_step_y (pdc, stepy);

    pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
}

void _dc_set_pixel_pair_clip (void* context, int x1, int x2, int y)
{
    PDC pdc = (PDC)context;
    PCLIPRECT cliprect;
    BOOL first_drawn = FALSE;

    if (x1 > x2) {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if (_dc_which_region_ban (pdc, y)) {
        int top;

        /* draw in this ban */
        cliprect = pdc->cur_ban;
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            if (INRECT (pdc->rc_output, x1, y) && INRECT (cliprect->rc, x1, y)) {
                pdc->move_to (pdc, x1, y);
                pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
                first_drawn = TRUE;
                break;
            }

            cliprect = cliprect->next;
        }

        if (x1 != x2) {
            if (!first_drawn) cliprect = pdc->cur_ban;
            while (cliprect && cliprect->rc.top == top) {
                if (INRECT (pdc->rc_output, x2, y) && INRECT (cliprect->rc, x2, y)) {
                    pdc->move_to (pdc, x2, y);
                    pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
                    break;
                }

                cliprect = cliprect->next;
            }
        }
    }
}

void _dc_draw_hline_clip (void* context, int x1, int x2, int y)
{
    PDC pdc = (PDC)context;
    PCLIPRECT cliprect;
    int top, w;

    if (x1 == x2) {
        if (!(x1 % pdc->step))
            _dc_set_pixel_clip (pdc, x1, y);
        return;
    }

    if (x1 > x2) {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if (_dc_which_region_ban (pdc, y)) {
        /* draw this horizontal line in this ban */
        cliprect = pdc->cur_ban;
        top = cliprect->rc.top;
        w = x2 - x1/*+ 1*/;
        while (cliprect && cliprect->rc.top == top) {
            RECT eff_rc;
            int _x = x1, _y = y, _w = w;

            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && _dc_cliphline (&eff_rc, &_x, &_y, &_w)) {

                /* hack for dot line */
                int tmp = ((_x-x1) % pdc->step);
                _x += tmp;
                _w -= tmp;

                if (_w > 0) {
                    pdc->move_to (pdc, _x, _y);
                    pdc->draw_pixel_span (PDC_TO_COMP_CTXT(pdc), _w);
                }
            }

            cliprect = cliprect->next;
        }
    }
}

void _dc_draw_vline_clip (void* context, int y1, int y2, int x)
{
    PDC pdc = (PDC)context;
    PCLIPRECT cliprect;
    int top, h;

    if (y1 == y2) {
        if (!(y1 % pdc->step))
            _dc_set_pixel_clip (pdc, x, y1);
        return;
    }

    if (y1 > y2) {
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }

    /* check with bounding rect of clipping region */
    if (y1 >= pdc->ecrgn.tail->rc.bottom || y2 < pdc->ecrgn.head->rc.top) {
        return;
    }

    cliprect = pdc->ecrgn.head;
    do {
        /* find the next ban intersects with this vertical line */
        while (cliprect && y1 >= cliprect->rc.bottom) {
            cliprect = cliprect->next;
        }

        if (!cliprect) return;

        /* find the clipping rect intersects with this vertical line in this ban */
        top = cliprect->rc.top;
        while (cliprect && cliprect->rc.top == top) {
            int _y, _h;
            RECT eff_rc;
            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && (x >= eff_rc.left && x < eff_rc.right)) {

                if (y1 < eff_rc.top)
                    y1 = eff_rc.top;

                if (y2 >= eff_rc.bottom)
                    h = eff_rc.bottom - y1;
                else
                    h = y2 - y1 + 1;

                /* hack for dot line */
                _y = y1 + (y1 % pdc->step);
                _h = h - (y1 % pdc->step) * 2;

                if (_h > 0) {
                    pdc->move_to (pdc, x, _y);
                    _dc_drawvline (pdc, _h);
                }
                
                if (h > 0)
                    y1 += h;
            }

            cliprect = cliprect->next;
        }

    } while (cliprect && y2 > y1);
}

#ifdef _MGHAVE_ADV_2DAPI

#include "mi.h"
#include "midc.h"

void _dc_fill_spans (PDC pdc, Span* spans, int nspans, BOOL sorted)
{
    Span* span;
    PCLIPRECT cliprect;
    int i, top;

    pdc->step = 1;
    pdc->cur_ban = NULL;

    for (i = 0; i < nspans; i++) {
        span = spans + i;

        if (sorted) {
            if (i == 0)
                _dc_which_region_ban (pdc, span->y);
            else if (span->y >= pdc->cur_ban->rc.bottom)
                _dc_which_region_ban (pdc, span->y);

            if (pdc->cur_ban == NULL)
                return;
        }
        else {
            pdc->cur_ban = NULL;
            _dc_which_region_ban (pdc, span->y);
            if (pdc->cur_ban == NULL)
                continue;
        }

        if (span->width == 1) {
            _dc_set_pixel_clip (pdc, span->x, span->y);
        }
        else {
            /* draw this horizontal line in this ban */
            cliprect = pdc->cur_ban;
            top = cliprect->rc.top;

            while (cliprect && cliprect->rc.top == top) {
                RECT eff_rc;
                int _x = span->x, _y = span->y, _w = span->width;

                if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                        && _dc_cliphline (&eff_rc, &_x, &_y, &_w)) {

                    if (_w > 0) {
                        pdc->move_to (pdc, _x, _y);
                        pdc->draw_pixel_span (PDC_TO_COMP_CTXT(pdc), _w);
                    }
                }
    
                cliprect = cliprect->next;
            }
        }
    }
}

void _dc_fill_span_brush_helper (PDC pdc, int x, int y, int w)
{
    int ts_orig_x = pdc->brush_orig.x;
    int ts_orig_y = pdc->brush_orig.y;
    int left = x, right = x + w;
    int curx, xstep, draww, drawh;
    const BITMAP* tiled;
    const STIPPLE* stipple;
    BOOL solid_stipple;

    coor_LP2SP (pdc, &ts_orig_x, &ts_orig_y);

    switch (pdc->brush_type) {
        case BT_TILED:
            if ((tiled = pdc->brush_tile) == NULL)
                return;

            drawh = (y - ts_orig_y) % tiled->bmHeight;
            if (drawh < 0)
                drawh += tiled->bmHeight;

            for (curx = left; curx < right; curx += xstep, w -= xstep) {
                BYTE* row;
      
                draww = (curx - ts_orig_x) % tiled->bmWidth;
                if (draww < 0)
                    draww += tiled->bmWidth;
      
                xstep = MIN (tiled->bmWidth - draww, w);

                row = tiled->bmBits + tiled->bmPitch * drawh + tiled->bmBytesPerPixel * draww;

                if (pdc->rop == ROP_SET) {
                    BITMAP bmp = *tiled;
                    GAL_Rect rect;
                    
                    rect.x = curx;
                    rect.y = y;
                    rect.w = xstep;
                    rect.h = 1;

                    bmp.bmBits = row;
                    bmp.bmWidth = xstep;
                    bmp.bmHeight = 1;
                    pdc->surface->clip_rect = rect;
                    GAL_PutBox (pdc->surface, &rect, &bmp);
                }
                else {
                    pdc->move_to (pdc, curx, y);
                    pdc->draw_src_span (PDC_TO_COMP_CTXT(pdc),
                            row, pdc->bkmode, xstep);
                }
            }
            break;

        case BT_STIPPLED:
        case BT_OPAQUE_STIPPLED:
            if ((stipple = pdc->brush_stipple) == NULL)
                return;

            solid_stipple = (pdc->brush_type == BT_OPAQUE_STIPPLED);
            for (curx = left; curx < right; curx++) {
                int maskx = curx, masky = y;
                int wid, hih;
                unsigned char foo;

                wid = stipple->width;
                hih = stipple->height;

                maskx = (curx - ts_orig_x) % wid;
                if (maskx < 0)
                    maskx += wid;

                masky = (y - ts_orig_y) % hih;
                if (masky < 0)
                    masky += hih;

                foo = stipple->bits [(maskx >> 3) + stipple->pitch * masky];
                if (foo & (1 << (maskx % 8)))
                    pdc->cur_pixel = pdc->brushcolor;
                else if (solid_stipple)
                    pdc->cur_pixel = pdc->bkcolor;
                else
                    continue;

                pdc->move_to (pdc, curx, y);
                pdc->draw_pixel(PDC_TO_COMP_CTXT(pdc));
            }
            break;

        default:
            break;
    }
}

void _dc_fill_hline_clip (void* context, int x1, int x2, int y)
{
    PDC pdc = (PDC)context;
    PCLIPRECT cliprect;
    int top, w;

    if (x1 > x2) {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }

    if (_dc_which_region_ban (pdc, y)) {
        cliprect = pdc->cur_ban;
        top = cliprect->rc.top;
        w = x2 - x1 + 1;
        while (cliprect && cliprect->rc.top == top) {
            RECT eff_rc;
            int _x = x1, _y = y, _w = w;

            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && _dc_cliphline (&eff_rc, &_x, &_y, &_w) && _w > 0) {
                _dc_fill_span_brush_helper (pdc, _x, _y, _w);
            }

            cliprect = cliprect->next;
        }
    }
}

void _dc_fill_spans_brush (PDC pdc, Span* spans, int nspans, BOOL sorted)
{
    Span* span;
    PCLIPRECT cliprect;
    int i, top;

    pdc->step = 1;
    pdc->cur_ban = NULL;

    for (i = 0; i < nspans; i++) {
        span = spans + i;

        if (sorted) {
            if (i == 0)
                _dc_which_region_ban (pdc, span->y);
            else if (span->y >= pdc->cur_ban->rc.bottom)
                _dc_which_region_ban (pdc, span->y);

            if (pdc->cur_ban == NULL)
                return;
        }
        else {
            pdc->cur_ban = NULL;
            _dc_which_region_ban (pdc, span->y);
            if (pdc->cur_ban == NULL)
                continue;
        }

        cliprect = pdc->cur_ban;
        top = cliprect->rc.top;

        while (cliprect && cliprect->rc.top == top) {
            RECT eff_rc;
            int _x = span->x, _y = span->y, _w = span->width;

            if (IntersectRect (&eff_rc, &pdc->rc_output, &cliprect->rc)
                    && _dc_cliphline (&eff_rc, &_x, &_y, &_w) && _w > 0)
                _dc_fill_span_brush_helper (pdc, _x, _y, _w);
    
            cliprect = cliprect->next;
        }
    }
}

#if 0
void _dc_dump_spans (Span* spans, int nspans)
{
    Span* span;
    int i;

    for (i = 0; i < nspans; i++) {
        span = spans + i;
        printf ("span %3d: (%d, %d, %d)\n", i, span->x, span->y, span->width);
    }
}
#endif

#endif /* _MGHAVE_ADV_2DAPI */
