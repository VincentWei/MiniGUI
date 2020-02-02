///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** compsor-manager.c: the compositor manager module for MiniGUI.
**
** Create date: 2020-01-19
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define _DEBUG
#include "common.h"

#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING)

#include "minigui.h"
#include "gdi.h"
#include "window.h"

#define SIZE_CLIPRC_HEAP        64

struct _CompositorCtxt {
    RECT        rc_screen;  // screen rect - avoid duplicated GetScreenRect calls
    BLOCKHEAP   cliprc_heap;// heap for clipping rects
    CLIPRGN     wins_rgn;   // visible region for all windows (subtract popup menus)
    CLIPRGN     dirty_rgn;  // the dirty region
    CLIPRGN     comps_rgn;  // the region needed to re-composite
    CLIPRGN     left_rgn;   // the left region
};

static CompositorCtxt* initialize (const char* name)
{
    CompositorCtxt* ctxt;

    ctxt = malloc (sizeof (CompositorCtxt));
    if (ctxt) {
        ctxt->rc_screen = GetScreenRect();
        InitFreeClipRectList (&ctxt->cliprc_heap, SIZE_CLIPRC_HEAP);
        InitClipRgn (&ctxt->wins_rgn, &ctxt->cliprc_heap);
        InitClipRgn (&ctxt->dirty_rgn, &ctxt->cliprc_heap);
        InitClipRgn (&ctxt->comps_rgn, &ctxt->cliprc_heap);
        InitClipRgn (&ctxt->left_rgn, &ctxt->cliprc_heap);

        SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);
    }

    _DBG_PRINTF("called: %p\n", ctxt);
    return ctxt;
}

static void terminate (CompositorCtxt* ctxt)
{
    if (ctxt) {
        EmptyClipRgn (&ctxt->wins_rgn);
        EmptyClipRgn (&ctxt->dirty_rgn);
        EmptyClipRgn (&ctxt->comps_rgn);
        EmptyClipRgn (&ctxt->left_rgn);
        DestroyFreeClipRectList (&ctxt->cliprc_heap);
        free (ctxt);
    }

    _DBG_PRINTF("called\n");
}

static void on_dirty_ppp (CompositorCtxt* ctxt, int zidx)
{
}

static void composite_with_wallpaper (CompositorCtxt* ctxt,
            const RECT* dirty_rc)
{
    int wp_w = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
    int wp_h = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);

    _DBG_PRINTF("called with dirty rect (%d, %d, %d, %d), wallpaper pattern size (%d, %d)\n",
            dirty_rc->left, dirty_rc->top, dirty_rc->right, dirty_rc->bottom,
            wp_w, wp_h);

    //SelectClipRect (HDC_SCREEN_SYS, dirty_rc);
    if (wp_w > 0 && wp_h > 0) {

        // tile the wallpaper pattern
        int y = 0;
        int left_h = RECTH (ctxt->rc_screen);
        while (left_h > 0) {

            int x = 0;
            int left_w = RECTW (ctxt->rc_screen);
            while (left_w > 0) {
                RECT rc = { x, y, x + wp_w, y + wp_h };
                if (IntersectRect (&rc, dirty_rc, &rc)) {
                    _DBG_PRINTF ("Blitting wallpaper pattern to (%d, %d)\n", rc.left, rc.top);
                    BitBlt (HDC_SCREEN, rc.left - dirty_rc->left,
                        rc.top - dirty_rc->top, RECTW (rc), RECTH (rc),
                        HDC_SCREEN_SYS, rc.left, rc.top, 0);
                }

                x += wp_w;
                left_w -= wp_w;
            }

            left_h -= wp_h;
            y += wp_h;
        }
    }
    else {
        SetBrushColor (HDC_SCREEN_SYS,
            GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP));
        FillBox (HDC_SCREEN_SYS, dirty_rc->left, dirty_rc->top,
                RECTWP(dirty_rc), RECTHP(dirty_rc));
    }

    SyncUpdateDC (HDC_SCREEN_SYS);
}

static void composite_win_znode (CompositorCtxt* ctxt,
            int from, const RECT* dirty_rc, BOOL is_top)
{
    RECT rc;
    const ZNODEHEADER* znode_hdr;

    znode_hdr = ServerGetWinZNodeHeader (NULL, from, NULL, TRUE);
    if (znode_hdr == NULL)
        return;

    if (znode_hdr->flags & ZNIF_VISIBLE) {
        if (IntersectRect (&rc, dirty_rc, &znode_hdr->rc)) {
            if (znode_hdr->ct == CT_OPAQUE) {
                SetMemDCColorKey (znode_hdr->mem_dc, 0, 0);
                SetMemDCAlpha (znode_hdr->mem_dc, 0, 0);
                BitBlt (znode_hdr->mem_dc,
                        rc.left - znode_hdr->rc.left,
                        rc.top - znode_hdr->rc.top,
                        RECTW(rc), RECTH(rc),
                        HDC_SCREEN_SYS, rc.left, rc.top, 0);
            }
            else {
                // composite znodes below from first
                int next = ServerGetNextZNode (NULL, from, NULL);
                if (next) {
                    composite_win_znode (ctxt, next, &rc, FALSE);
                }
                else {
                    composite_with_wallpaper (ctxt, &rc);
                }

                switch (znode_hdr->ct) {
                case CT_COLORKEY:
                    SetMemDCColorKey (znode_hdr->mem_dc, MEMDC_FLAG_SRCCOLORKEY,
                        DWORD2Pixel (znode_hdr->mem_dc, znode_hdr->ct_arg));
                    break;

                case CT_ALPHACHANNEL:
                    SetMemDCAlpha (znode_hdr->mem_dc, MEMDC_FLAG_SRCALPHA,
                            (BYTE)znode_hdr->ct_arg);
                    break;

                case CT_ALPHAPIXEL:
                    SetMemDCAlpha (znode_hdr->mem_dc, MEMDC_FLAG_SRCPIXELALPHA, 0);
                    break;

                case CT_BLURRED:
                    _WRN_PRINTF("CT_BLURRED is not implemented\n");
                    break;

                default:
                    assert (0); // never touch here.
                    break;
                }
            }

            if (is_top)
                SubtractClipRect (&ctxt->left_rgn, &rc);
        }
    }
    else {
        // not visible znode; composite with znodes below
        int next = ServerGetNextZNode (NULL, from, NULL);
        if (next) {
            composite_win_znode (ctxt, next, dirty_rc, FALSE);
        }
        else {
            composite_with_wallpaper (ctxt, dirty_rc);
        }
    }

    ServerReleaseWinZNodeHeader (NULL, from);
}

static void on_dirty_win (CompositorCtxt* ctxt, int zidx)
{
    int i, next, prev;
    const ZNODEHEADER* znode_hdr = NULL;
    CLIPRECT *crc;

    if (zidx > 0) {
        znode_hdr = ServerGetWinZNodeHeader (NULL, zidx, NULL, TRUE);
        if (znode_hdr && znode_hdr->dirty_rcs) {
            EmptyClipRgn (&ctxt->dirty_rgn);
            for (i = 0; i < znode_hdr->nr_dirty_rcs; i++) {
                RECT rc;

                // device coordinates to screen coordinates
                rc = znode_hdr->dirty_rcs [i];
                OffsetRect (&rc, znode_hdr->rc.left, znode_hdr->rc.top);
                AddClipRect (&ctxt->dirty_rgn, &rc);
            }
        }

        // subtract opaque znodes above current znode.
        prev = ServerGetPrevZNode (NULL, zidx, NULL);
        while (prev > 0) {

            znode_hdr = ServerGetWinZNodeHeader (NULL, prev, NULL, FALSE);
            if (znode_hdr->ct == CT_OPAQUE) {
                SubtractClipRect (&ctxt->dirty_rgn, &znode_hdr->rc);
            }

            prev = ServerGetPrevZNode (NULL, prev, NULL);
        }
        ServerReleaseWinZNodeHeader (NULL, zidx);
    }
    else {
        SetClipRgn (&ctxt->dirty_rgn, &ctxt->rc_screen);
    }

    if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
        _DBG_PRINTF ("The dirty region is empty\n");
        return;
    }

    //SelectClipRect (HDC_SCREEN_SYS, &ctxt->rc_screen);
    IntersectRegion (&ctxt->comps_rgn, &ctxt->dirty_rgn, &ctxt->wins_rgn);
    EmptyClipRgn (&ctxt->dirty_rgn);

    if (IsEmptyClipRgn (&ctxt->comps_rgn)) {
        _DBG_PRINTF ("The compositing region is empty\n");
        return;
    }

    CopyRegion (&ctxt->left_rgn, &ctxt->comps_rgn);

    next = ServerGetNextZNode (NULL, 0, NULL);
    while (next > 0) {
        crc = ctxt->comps_rgn.head;
        while (crc) {
            composite_win_znode (ctxt, next, &crc->rc, TRUE);

            crc = crc->next;
        }

        next = ServerGetNextZNode (NULL, next, NULL);
    }
    EmptyClipRgn (&ctxt->comps_rgn);

    if (IsEmptyClipRgn (&ctxt->left_rgn)) {
        _DBG_PRINTF ("The left compositing region is empty\n");
        return;
    }

    // fill left region with wallpaper
    crc = ctxt->left_rgn.head;
    while (crc) {
        composite_with_wallpaper (ctxt, &crc->rc);

        crc = crc->next;
    }

    EmptyClipRgn (&ctxt->left_rgn);
}

static void subtract_opaque_win_znodes_above (CompositorCtxt* ctxt, int from)
{
    int prev;

    // subtract opaque window znodes
    prev = ServerGetPrevZNode (NULL, from, NULL);
    while (prev > 0) {
        const ZNODEHEADER* znode_hdr;

        znode_hdr = ServerGetWinZNodeHeader (NULL, prev, NULL, FALSE);
        if (znode_hdr && znode_hdr->ct == CT_OPAQUE) {
            SubtractClipRect (&ctxt->dirty_rgn, &znode_hdr->rc);
        }

        prev = ServerGetPrevZNode (NULL, prev, NULL);
    }
}

static void subtract_opaque_ppp_znodes (CompositorCtxt* ctxt)
{
    int i, nr_ppp;

    // subtract opaque menu znodes
    nr_ppp = ServerGetPopupMenusCount();
    for (i = 0; i < nr_ppp; i++) {
        const ZNODEHEADER* znode_hdr;

        znode_hdr = ServerGetPopupMenuZNodeHeader (i, NULL, FALSE);
        if (znode_hdr && znode_hdr->ct == CT_OPAQUE) {
            SubtractClipRect (&ctxt->dirty_rgn, &znode_hdr->rc);
        }
    }
}

static void on_dirty_wpp (CompositorCtxt* ctxt)
{
    int next;
    const ZNODEHEADER* znode_hdr;
    CLIPRECT *crc;

    _DBG_PRINTF("called\n");

    znode_hdr = ServerGetWinZNodeHeader (NULL, 0, NULL, TRUE);
    if (znode_hdr && znode_hdr->dirty_rcs) {
        int i;

        EmptyClipRgn (&ctxt->dirty_rgn);
        for (i = 0; i < znode_hdr->nr_dirty_rcs; i++) {
            RECT rc;

            // device coordinates to screen coordinates
            rc = znode_hdr->dirty_rcs [i];
            OffsetRect (&rc, znode_hdr->rc.left, znode_hdr->rc.top);
            AddClipRect (&ctxt->dirty_rgn, &rc);
        }

        subtract_opaque_win_znodes_above (ctxt, 0);
        subtract_opaque_ppp_znodes (ctxt);
    }
    else {
        _DBG_PRINTF ("failed to get z-node header\n");
        goto ret;
    }

    if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
        _DBG_PRINTF ("The dirty region is empty\n");
        goto ret;
    }

    IntersectRegion (&ctxt->comps_rgn, &ctxt->dirty_rgn, &ctxt->wins_rgn);
    EmptyClipRgn (&ctxt->dirty_rgn);

    if (IsEmptyClipRgn (&ctxt->comps_rgn)) {
        _DBG_PRINTF ("The compositing region is empty\n");
        goto ret;
    }

    CopyRegion (&ctxt->left_rgn, &ctxt->comps_rgn);

    next = ServerGetNextZNode (NULL, 0, NULL);
    while (next > 0) {
        crc = ctxt->comps_rgn.head;
        while (crc) {
            composite_win_znode (ctxt, next, &crc->rc, TRUE);

            crc = crc->next;
        }

        next = ServerGetNextZNode (NULL, next, NULL);
    }
    EmptyClipRgn (&ctxt->comps_rgn);

    if (IsEmptyClipRgn (&ctxt->left_rgn)) {
        _DBG_PRINTF ("The left compositing region is empty\n");
        goto ret;
    }

    // fill left region with wallpaper
    crc = ctxt->left_rgn.head;
    while (crc) {
        composite_with_wallpaper (ctxt, &crc->rc);

        crc = crc->next;
    }

    EmptyClipRgn (&ctxt->left_rgn);

ret:
    if (znode_hdr)
        ServerReleaseWinZNodeHeader (NULL, 0);
}

static const ZNODEHEADER* rebuild_wins_region (CompositorCtxt* ctxt)
{
    int i, nr_ppps;
    const ZNODEHEADER* znode_hdr = NULL;

    SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);

    nr_ppps = ServerGetPopupMenusCount ();
    for (i = 0; i < nr_ppps; i++) {
        znode_hdr = ServerGetPopupMenuZNodeHeader (i, NULL, FALSE);
        if (znode_hdr)
            SubtractClipRect (&ctxt->wins_rgn, &znode_hdr->rc);
    }

    return znode_hdr;
}

static void refresh (CompositorCtxt* ctxt)
{
    _DBG_PRINTF("called\n");
    rebuild_wins_region (ctxt);
    on_dirty_win (ctxt, 0);
}

static void on_showing_ppp (CompositorCtxt* ctxt, int zidx)
{
    const ZNODEHEADER* znode_hdr = ServerGetPopupMenuZNodeHeader (zidx, NULL, FALSE);
    if (znode_hdr)
        SubtractClipRect (&ctxt->wins_rgn, &znode_hdr->rc);
}

static void on_hiding_ppp (CompositorCtxt* ctxt, int zidx)
{
    const ZNODEHEADER* znode_hdr;

    rebuild_wins_region (ctxt);
    znode_hdr = ServerGetPopupMenuZNodeHeader (zidx, NULL, FALSE);

    if (znode_hdr) {
        on_dirty_win (ctxt, 0);
    }
}

static void on_closing_menu (CompositorCtxt* ctxt)
{
    SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);
}

CompositorOps __mg_fallback_compositor = {
    initialize: initialize,
    terminate: terminate,
    refresh: refresh,
    on_dirty_ppp: on_dirty_ppp,
    on_dirty_win: on_dirty_win,
    on_dirty_wpp: on_dirty_wpp,
    on_showing_ppp: on_showing_ppp,
    on_hiding_ppp: on_hiding_ppp,
    on_closing_menu: on_closing_menu,
};

#endif /* defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING) */

