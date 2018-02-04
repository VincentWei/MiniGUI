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
** scrolled.h: header file of scrolled window support module.
*/

#ifndef _SCROLLED_H
#define _SCROLLED_H

#ifdef __cplusplus
extern "C"
{
#endif


/* default size and position values */
#define HSCROLL                 5   // h scroll value
#define VSCROLL                 15  // v scroll value

typedef struct _scrdata
{
    unsigned short hScrollVal;
    unsigned short vScrollVal;
    unsigned short hScrollPageVal;
    unsigned short vScrollPageVal;

    /* scroll offset */
    int nContX;
    int nContY;
    int prevnContX;
    int prevnContY;
    /* content range */
    int nContWidth;
    int nContHeight;
    /* visible content range */
    unsigned short visibleWidth;
    unsigned short visibleHeight;

    /* content margins */
    unsigned short leftMargin;
    unsigned short topMargin;
    unsigned short rightMargin;
    unsigned short bottomMargin;

    HWND hSV;

    /* scrollbar mode */
    SBPolicyType sbPolicy;

    void (*move_content) (HWND, struct _scrdata *, BOOL);

} SCRDATA;
typedef SCRDATA *PSCRDATA;


MG_EXPORT int     DefaultScrolledProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
MG_EXPORT int     scrolled_init (HWND hWnd, PSCRDATA pscrdata, int w, int h);
MG_EXPORT void    scrolled_destroy (PSCRDATA pscrdata);
MG_EXPORT void    scrolled_hscroll (HWND hWnd, PSCRDATA pscrdata, WPARAM wParam, LPARAM lParam);
MG_EXPORT void    scrolled_vscroll (HWND hWnd, PSCRDATA pscrdata, WPARAM wParam, LPARAM lParam);
MG_EXPORT void    scrolled_reset_content (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT int     scrolled_set_cont_height (HWND hWnd, PSCRDATA pscrdata, int cont_h);
MG_EXPORT int     scrolled_set_cont_width (HWND hWnd, PSCRDATA pscrdata, int cont_w);
MG_EXPORT BOOL    scrolled_set_cont_pos (HWND hWnd, PSCRDATA pscrdata, int cont_x, int cont_y);
MG_EXPORT BOOL    scrolled_make_pos_visible (HWND hWnd, PSCRDATA pscrdata, int pos_x, int pos_y);
MG_EXPORT void    scrolled_refresh_container (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT void    scrolled_set_scrollval (PSCRDATA pscrdata, int hval, int vval);
MG_EXPORT void    scrolled_set_scrollpageval (PSCRDATA pscrdata, int hval, int vval);
MG_EXPORT int     scrolled_set_hscrollinfo (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT int     scrolled_set_vscrollinfo (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT void    scrolled_init_contsize (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT void    scrolled_set_visible (HWND hWnd, PSCRDATA pscrdata);
MG_EXPORT int     scrolled_content_to_visible (PSCRDATA pscrdata, int *x, int *y);
MG_EXPORT int     scrolled_visible_to_content (PSCRDATA pscrdata, int *x, int *y);
MG_EXPORT void    scrolled_recalc_areas (HWND hWnd, PSCRDATA pscrdata, int new_w, int new_h);
MG_EXPORT void    scrolled_init_margins (PSCRDATA pscrdata, int left, int top,
                               int right, int bottom);
MG_EXPORT void    scrolled_refresh_rect (PSCRDATA pscrdata, RECT *rc);
MG_EXPORT void    scrolled_set_margins (PSCRDATA pscrdata, RECT *rc);


static inline int scrolled_get_contwidth (PSCRDATA pscrdata)
{
    return pscrdata->nContWidth;
}

static inline void scrolled_set_content (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    pscrdata->move_content (hWnd, pscrdata, visChanged);
}

static inline void scrolled_get_content_rect (PSCRDATA pscrdata, RECT *rc)
{
    if (rc) {
        rc->left = -pscrdata->nContX;
        rc->top = -pscrdata->nContY;
        rc->right = rc->left + pscrdata->nContWidth;
        rc->bottom = rc->top + pscrdata->nContHeight;
    }
}

/* decides whether a position in the virtual content window is visible */
static inline BOOL scrolled_is_pos_visible (PSCRDATA pscrdata, int x, int y)
{
    if (x < pscrdata->nContX || x > pscrdata->nContX + pscrdata->visibleWidth ||
               y < pscrdata->nContY || y > pscrdata->nContY + pscrdata->visibleHeight)
        return FALSE;
    return TRUE;
}
    
/* ------------------------------------------------------------------------- */
// newly added

static inline void scrolled_visible_to_window (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x += pscrdata->leftMargin;
    if (y)
        *y += pscrdata->topMargin;
}

static inline void scrolled_window_to_visible (PSCRDATA pscrdata, int *x, int *y)
{
    if (x)
        *x -= pscrdata->leftMargin;
    if (y)
        *y -= pscrdata->topMargin;
}

static inline void scrolled_content_to_window (PSCRDATA pscrdata, int *x, int *y)
{
    scrolled_content_to_visible (pscrdata, x, y);
    scrolled_visible_to_window (pscrdata, x, y);
}

static inline void scrolled_window_to_content (PSCRDATA pscrdata, int *x, int *y)
{
    scrolled_window_to_visible (pscrdata, x, y);
    scrolled_visible_to_content (pscrdata, x, y);
}

static inline void scrolled_get_margins (PSCRDATA pscrdata, RECT *rc)
{
    if (rc) {
        rc->left = pscrdata->leftMargin;
        rc->top = pscrdata->topMargin;
        rc->right = pscrdata->rightMargin;
        rc->bottom = pscrdata->bottomMargin;
    }
}

static inline void scrolled_get_visible_rect (PSCRDATA pscrdata, RECT *rcVis)
{
    if (rcVis) {
        rcVis->left = pscrdata->leftMargin;
        rcVis->top = pscrdata->topMargin;
        rcVis->right = pscrdata->leftMargin + pscrdata->visibleWidth;
        rcVis->bottom = pscrdata->topMargin + pscrdata->visibleHeight;
    }
}
            
static inline void scrolled_refresh_content (PSCRDATA pscrdata)
{
    RECT rcVis;
    scrolled_get_visible_rect (pscrdata, &rcVis);
    InvalidateRect (pscrdata->hSV, &rcVis, TRUE);
}

static inline void scrolled_refresh_view (PSCRDATA pscrdata)
{
    RECT rcVis;
    RECT rcClient;
    RECT rcMargin;

    scrolled_get_visible_rect (pscrdata, &rcVis);
    ScrollWindow(pscrdata->hSV, pscrdata->prevnContX - pscrdata->nContX, 
            pscrdata->prevnContY - pscrdata->nContY, &rcVis, NULL);

    /* [humingming/2010/9/21]: fix Bug5027.
     * update the margin, beacause ScrollWindow will narrow the cilprect.
     * this maybe exclude the margin cilprect.
     */
     GetClientRect(pscrdata->hSV, &rcClient);

     rcMargin = rcClient;
     if (pscrdata->topMargin > 0) {
         rcMargin.top = rcClient.top;
         rcMargin.bottom = pscrdata->topMargin;
         InvalidateRect (pscrdata->hSV, &rcMargin, TRUE);
     }

     if (pscrdata->bottomMargin > 0) {
        rcMargin.top = rcClient.bottom - pscrdata->bottomMargin;
        rcMargin.bottom = rcClient.bottom;
        InvalidateRect (pscrdata->hSV, &rcMargin, TRUE);
     }

     rcMargin = rcClient;
     if (pscrdata->leftMargin > 0) {
        rcMargin.left = rcClient.left;
        rcMargin.right = pscrdata->rightMargin;
        InvalidateRect (pscrdata->hSV, &rcMargin, TRUE);
     }

     if (pscrdata->rightMargin > 0) {
        rcMargin.left = rcClient.right - pscrdata->rightMargin;
        rcMargin.right = rcClient.right;
        InvalidateRect (pscrdata->hSV, &rcMargin, TRUE);
     }
}

/* ------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SCROLLED_H */

