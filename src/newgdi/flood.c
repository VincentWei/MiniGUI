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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
#include "cursor.h"

typedef struct FLOODED_LINE      /* store segments which have been flooded */
{
   short flags;                  /* status of the segment */
   short lpos, rpos;             /* left and right ends of segment */
   short y;                      /* y coordinate of the segment */
   short next;                   /* linked list if several per line */
} FLOODED_LINE;

typedef struct FLOODER_INFO
{
    void* context;                  /* context information */
    const RECT* dst_rc;             /* rect of destination */
    CB_EQUAL_PIXEL cb_equal_pixel;  /* call back function to test pixel */
    CB_FLOOD_FILL cb_flood_fill;    /* call back function to draw a horizontal line */

    FLOODED_LINE* flooded_lines;    /* flooded segments */
    int flood_count;                /* number of flooded segments */
} FLOODER_INFO;


#define FLOOD_IN_USE             1
#define FLOOD_TODO_ABOVE         2
#define FLOOD_TODO_BELOW         4

#define FLOOD_LINE_P(c)          (fi->flooded_lines + (c))
#define FLOOD_LINE(c)            (fi.flooded_lines + (c))

/* flooder:
 *  Fills a horizontal line around the specified position, and adds it
 *  to the list of drawn segments. Returns the first x coordinate after 
 *  the part of the line which it has dealt with.
 */
static int flooder (FLOODER_INFO* fi, int x, int y)
{
    FLOODED_LINE *p;
    int left = 0, right = 0;
    int c;

    /* check start pixel */
    if (!fi->cb_equal_pixel (fi->context, x, y))
        return x + 1;

    /* work left from starting point */ 
    for (left = x - 1; left >= fi->dst_rc->left; left--)
        if (!fi->cb_equal_pixel (fi->context, left, y))
            break;

    /* work right from starting point */ 
    for (right = x + 1; right < fi->dst_rc->right; right++)
        if (!fi->cb_equal_pixel (fi->context, right, y))
            break;

    left++;
    right--;

    /* draw the line */
    fi->cb_flood_fill (fi->context, left, right, y);

    /* store it in the list of flooded segments */
    c = y - fi->dst_rc->top;
    p = FLOOD_LINE_P (c);

    if (p->flags) {
        while (p->next) {
            c = p->next;
            p = FLOOD_LINE_P (c);
        }

        p->next = c = fi->flood_count++;
        fi->flooded_lines = realloc (fi->flooded_lines, 
                        sizeof(FLOODED_LINE) * fi->flood_count);
        p = FLOOD_LINE_P (c);
    }

    p->flags = FLOOD_IN_USE;
    p->lpos = left;
    p->rpos = right;
    p->y = y;
    p->next = 0;

    if (y > fi->dst_rc->top)
        p->flags |= FLOOD_TODO_ABOVE;

    if (y + 1 < fi->dst_rc->bottom)
        p->flags |= FLOOD_TODO_BELOW;

    return right + 2;
}

/* check_flood_line:
 *  Checks a line segment, using the scratch buffer is to store a list of 
 *  segments which have already been drawn in order to minimise the required 
 *  number of tests.
 */
static BOOL check_flood_line (FLOODER_INFO* fi, int y, int left, int right)
{
   int c;
   FLOODED_LINE *p;
   int ret = FALSE;

   while (left <= right) {
      c = y - fi->dst_rc->top;

      for (;;) {
         p = FLOOD_LINE_P(c);

         if ((left >= p->lpos) && (left <= p->rpos)) {
            left = p->rpos+2;
            break;
         }

         c = p->next;

         if (!c) {
            left = flooder(fi, left, y);
            ret = TRUE;
            break; 
         }
      }
   }

   return ret;
}

/*
 * FloodFillGenerator:
 */
BOOL GUIAPI FloodFillGenerator (void* context, const RECT* dst_rc, int x, int y,
                CB_EQUAL_PIXEL cb_equal_pixel, CB_FLOOD_FILL cb_flood_fill)
{
    FLOODER_INFO fi;
    int c, done;
    FLOODED_LINE *p;

    /* make sure we have a valid starting point */ 
    if ((x < dst_rc->left) || (x >= dst_rc->right) || (y < dst_rc->top) || (y >= dst_rc->bottom)) {
        return TRUE;
    }

    /* set up the list of flooded segments */
    fi.flood_count = RECTHP (dst_rc);
    fi.flooded_lines = (FLOODED_LINE*) malloc (sizeof(FLOODED_LINE) * fi.flood_count);
    if (!(p = fi.flooded_lines)) {
        return FALSE;
    }

    fi.context = context;
    fi.dst_rc = dst_rc;
    fi.cb_equal_pixel = cb_equal_pixel;
    fi.cb_flood_fill = cb_flood_fill;

    for (c = 0; c < fi.flood_count; c++) {
        p[c].flags = 0;
        p[c].lpos = SHRT_MAX;
        p[c].rpos = SHRT_MIN;
        p[c].y = y;
        p[c].next = 0;
    }

    /* start up the flood algorithm */
    flooder (&fi, x, y);

    /* continue as long as there are some segments still to test */
    do {
        done = TRUE;

        /* for each line on the screen */
        for (c = 0; c < fi.flood_count; c++) {

            p = FLOOD_LINE (c);
    
            /* check below the segment? */
            if (p->flags & FLOOD_TODO_BELOW) {
                p->flags &= ~FLOOD_TODO_BELOW;
            if (check_flood_line (&fi, p->y+1, p->lpos, p->rpos)) {
                done = FALSE;
                p = FLOOD_LINE (c);
            }
        }

        /* check above the segment? */
        if (p->flags & FLOOD_TODO_ABOVE) {
            p->flags &= ~FLOOD_TODO_ABOVE;
            if (check_flood_line (&fi, p->y-1, p->lpos, p->rpos)) {
                done = FALSE;
                /* special case shortcut for going backwards */
                if ((c < RECTHP (dst_rc)) && (c > 0))
                    c -= 2;
                }
            }
        }

    } while (!done);

    free (fi.flooded_lines);
    return TRUE;
}

static void _flood_fill_draw_hline (void* context, int x1, int x2, int y)
{
    PDC pdc = (PDC)context;

    SetRect (&pdc->rc_output, MIN (x1, x2), y, MAX (x1, x2) + 1, y + 1);

    ENTER_DRAWING (pdc);

#ifdef _MGHAVE_ADV_2DAPI
    if (pdc->brush_type == BT_SOLID)
        _dc_draw_hline_clip (context, x1, x2, y);
    else
        _dc_fill_hline_clip (context, x1, x2, y);
#else
    _dc_draw_hline_clip (context, x1, x2, y);
#endif

    LEAVE_DRAWING (pdc);
}

static BOOL equal_pixel (void* context, int x, int y)
{
    gal_pixel pixel = _dc_get_pixel_cursor ((PDC)context, x, y);

    return ((PDC)context)->skip_pixel == pixel;
}

/* FloodFill
 * Fills an enclosed area (starting at point x, y).
 */ 
BOOL GUIAPI FloodFill (HDC hdc, int x, int y)
{
    PDC pdc;
    BOOL ret = TRUE;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return TRUE;

    /* hide cursor tempororily */
    ShowCursor (FALSE);

    coor_LP2SP (pdc, &x, &y);

    pdc->cur_pixel = pdc->brushcolor;
    pdc->cur_ban = NULL;

    pdc->skip_pixel = _dc_get_pixel_cursor (pdc, x, y);

    /* does the start point have a equal value? */
    if (pdc->skip_pixel == pdc->brushcolor)
        goto equal_pixel;

    ret = FloodFillGenerator (pdc, &pdc->DevRC, x, y, 
                    equal_pixel, _flood_fill_draw_hline);

equal_pixel:
    UNLOCK_GCRINFO (pdc);

    /* Show cursor */
    ShowCursor (TRUE);

    return ret;
}

