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
** compsor-fallback.c: the fallback compositor.
**
** Create date: 2020-01-19
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING)

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "debug.h"

#define SIZE_CLIPRC_HEAP        64

struct _CompositorCtxt {
    RECT        rc_screen;  // screen rect - avoid duplicated GetScreenRect calls
    BLOCKHEAP   cliprc_heap;// heap for clipping rects
    CLIPRGN     wins_rgn;   // visible region for all windows (subtract popup menus)
    CLIPRGN     dirty_rgn;  // the dirty region
    CLIPRGN     inv_rgn;    // the invalid region for a specific znode
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
        InitClipRgn (&ctxt->inv_rgn, &ctxt->cliprc_heap);

        SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);
    }

    return ctxt;
}

static void terminate (CompositorCtxt* ctxt)
{
    if (ctxt) {
        EmptyClipRgn (&ctxt->wins_rgn);
        EmptyClipRgn (&ctxt->dirty_rgn);
        EmptyClipRgn (&ctxt->inv_rgn);
        DestroyFreeClipRectList (&ctxt->cliprc_heap);
        free (ctxt);
    }
}

/* return the number of subtracted windows above */
static int subtract_opaque_win_znodes_above (CompositorCtxt* ctxt, int from)
{
    const ZNODEHEADER* znode_hdr;
    int nr_subtracted = 0;
    int prev;

    /* subtract opaque window znodes */
    prev = ServerGetPrevZNode (NULL, from, NULL);
    while (prev > 0) {
        CLIPRGN* rgn;

        znode_hdr = ServerGetWinZNodeHeader (NULL, prev, (void**)&rgn, FALSE);
        assert (znode_hdr);

        if ((znode_hdr->flags & ZNIF_VISIBLE) &&
                (znode_hdr->ct & CT_SYSTEM_MASK) == CT_OPAQUE) {

            assert (rgn);
            if (SubtractRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, rgn)) {
                nr_subtracted++;
            }
        }

        prev = ServerGetPrevZNode (NULL, prev, NULL);
    }

    return nr_subtracted;
}

static int get_lucent_win_znodes_above (CompositorCtxt* ctxt, int from)
{
    int nr_lucent = 0;
    int prev;

    /* subtract opaque window znodes */
    if (from > 0)
        prev = from;
    else
        prev = ServerGetPrevZNode (NULL, 0, NULL);

    while (prev > 0) {
        const ZNODEHEADER* znode_hdr;
        CLIPRGN* rgn;

        znode_hdr = ServerGetWinZNodeHeader (NULL, prev, (void**)&rgn, FALSE);
        assert (znode_hdr);

        if ((znode_hdr->flags & ZNIF_VISIBLE) &&
                (znode_hdr->ct & CT_SYSTEM_MASK) != CT_OPAQUE &&
                AreRegionsIntersected (&ctxt->dirty_rgn, rgn)) {
            nr_lucent++;
        }

        prev = ServerGetPrevZNode (NULL, prev, NULL);
    }

    return nr_lucent;
}

/*
 * This helper is useful for transparent or blur popup menus
 * The fallback compositor only supports opaque popup menus.
static void subtract_opaque_ppp_znodes (CompositorCtxt* ctxt)
{
    int i, nr_ppp;

    // subtract opaque menu znodes
    nr_ppp = ServerGetPopupMenusCount();
    for (i = 0; i < nr_ppp; i++) {
        const ZNODEHEADER* znode_hdr;
        CLIPRGN* rgn;

        znode_hdr = ServerGetPopupMenuZNodeHeader (i, (void**)rgn, FALSE);
        if (znode_hdr && znode_hdr->ct == CT_OPAQUE) {
            SubtractRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, rgn);
        }
    }
}
*/

static inline void tile_dirty_region_for_wallpaper (CompositorCtxt* ctxt)
{
    int wp_w = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
    int wp_h = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);

    if (wp_w > 0 && wp_h > 0) {

        // tile the dirty region of the wallpaper pattern
        int y = 0;
        int left_h = RECTH (ctxt->rc_screen);

        CopyRegion (&ctxt->inv_rgn, &ctxt->dirty_rgn);
        while (left_h > 0) {

            int x = 0;
            int left_w = RECTW (ctxt->rc_screen);

            while (left_w > 0) {
                if (x > 0 || y > 0) {
                    OffsetRegion (&ctxt->inv_rgn, x, y);
                    UnionRegion (&ctxt->dirty_rgn,
                            &ctxt->dirty_rgn, &ctxt->inv_rgn);
                    OffsetRegion (&ctxt->inv_rgn, -x, -y);
                }

                x += wp_w;
                left_w -= wp_w;
            }

            left_h -= wp_h;
            y += wp_h;
        }

        EmptyClipRgn (&ctxt->inv_rgn);
    }
}

static BOOL generate_dirty_region (CompositorCtxt* ctxt,
        const ZNODEHEADER* znode_hdr, const CLIPRGN* znode_rgn)
{
    EmptyClipRgn (&ctxt->dirty_rgn);
    if (znode_hdr && znode_hdr->dirty_rcs) {
        int i;
        for (i = 0; i < znode_hdr->nr_dirty_rcs; i++) {
            RECT rc;

            // device coordinates to screen coordinates
            rc = znode_hdr->dirty_rcs [i];
            OffsetRect (&rc, znode_hdr->rc.left, znode_hdr->rc.top);
            AddClipRect (&ctxt->dirty_rgn, &rc);
        }

        if (znode_rgn)
            IntersectRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, znode_rgn);
        else
            IntersectClipRect (&ctxt->dirty_rgn, &ctxt->rc_screen);
    }

    return !IsEmptyClipRgn (&ctxt->dirty_rgn);
}

static void composite_wallpaper_rect (CompositorCtxt* ctxt,
            const RECT* dirty_rc)
{
    int wp_w = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
    int wp_h = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);

    if (wp_w > 0 && wp_h > 0) {

        // tile the wallpaper pattern
        int y = 0;
        int left_h = RECTH (ctxt->rc_screen);
        while (left_h > 0) {

            int x = 0;
            int left_w = RECTW (ctxt->rc_screen);
            while (left_w > 0) {
                RECT rc = { x, y, x + wp_w, y + wp_h };
                RECT eff_rc;
                if (IntersectRect (&eff_rc, dirty_rc, &rc)) {
                    SelectClipRect (HDC_SCREEN_SYS, &eff_rc);
                    BitBlt (HDC_SCREEN, 0, 0, RECTW (rc), RECTH (rc),
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
            GetWindowElementPixelEx (HWND_DESKTOP, HDC_SCREEN_SYS,
                WE_BGC_DESKTOP));
        SelectClipRect (HDC_SCREEN_SYS, &ctxt->rc_screen);
        FillBox (HDC_SCREEN_SYS, dirty_rc->left, dirty_rc->top,
                RECTWP(dirty_rc), RECTHP(dirty_rc));
    }
}

static inline void composite_wallpaper (CompositorCtxt* ctxt)
{
    if (!IsEmptyClipRgn (&ctxt->dirty_rgn)) {

        CLIPRECT* crc = ctxt->dirty_rgn.head;
        while (crc) {
            composite_wallpaper_rect (ctxt, &crc->rc);
            crc = crc->next;
        }
    }
}

static void composite_ppp_znodes (CompositorCtxt* ctxt)
{
    int zidx, nr_ppps;

    nr_ppps = ServerGetPopupMenusCount ();
    /* we start from the topmost popup menu znode */
    for (zidx = (nr_ppps - 1); zidx >= 0; zidx--) {
        const ZNODEHEADER* znode_hdr = NULL;
        CLIPRGN* rgn;
        znode_hdr = ServerGetPopupMenuZNodeHeader (zidx, (void**)&rgn, TRUE);
        assert (znode_hdr);
        assert (rgn);

        // we use invalid region as the clipping region of the popup menu znode
        ClipRgnIntersect (&ctxt->inv_rgn, &ctxt->dirty_rgn, rgn);
        if (IsEmptyClipRgn (&ctxt->inv_rgn)) {
            ServerReleasePopupMenuZNodeHeader (zidx);
            break;
        }
        else {
            // blit this popup menu to screen
            SelectClipRegion (HDC_SCREEN_SYS, &ctxt->inv_rgn);
            BitBlt (znode_hdr->mem_dc, 0, 0,
                    RECTW (znode_hdr->rc), RECTH (znode_hdr->rc),
                    HDC_SCREEN_SYS, znode_hdr->rc.left, znode_hdr->rc.top, 0);

            // subtract the clipping region of the popup menu znode
            // from the dirty region, because this compositor
            // handles any popup menu as opaque.
            SubtractRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, &ctxt->inv_rgn);
        }

        ServerReleasePopupMenuZNodeHeader (zidx);
    }

    EmptyClipRgn (&ctxt->inv_rgn);
}

static void composite_opaque_win_znode (CompositorCtxt* ctxt, int from)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn;

    if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
        return;
    }

    znode_hdr = ServerGetWinZNodeHeader (NULL, from, (void**)&rgn, TRUE);
    assert (znode_hdr);

    if ((znode_hdr->flags & ZNIF_VISIBLE) &&
            ((znode_hdr->ct & CT_SYSTEM_MASK) == CT_OPAQUE) &&
            IntersectRegion (&ctxt->inv_rgn, &ctxt->dirty_rgn, rgn)) {

        assert (rgn);
        SetMemDCColorKey (znode_hdr->mem_dc, 0, 0);
        SetMemDCAlpha (znode_hdr->mem_dc, 0, 0);
        SelectClipRegion (HDC_SCREEN_SYS, &ctxt->inv_rgn);
        BitBlt (znode_hdr->mem_dc,
                0, 0, RECTW(znode_hdr->rc), RECTH(znode_hdr->rc),
                HDC_SCREEN_SYS,
                znode_hdr->rc.left, znode_hdr->rc.top, 0);

        SubtractRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, &ctxt->inv_rgn);
        EmptyClipRgn (&ctxt->inv_rgn);
    }

    ServerReleaseWinZNodeHeader (NULL, from);
}

static void composite_all_lucent_win_znodes (CompositorCtxt* ctxt,
        const CLIPRGN* dirty_rgn)
{
    int prev;

    if (IsEmptyClipRgn (dirty_rgn)) {
        return;
    }

    prev = ServerGetPrevZNode (NULL, 0, NULL);
    while (prev > 0) {
        const ZNODEHEADER* znode_hdr;
        CLIPRGN* rgn;

        znode_hdr = ServerGetWinZNodeHeader (NULL, prev, (void**)&rgn, TRUE);
        assert (znode_hdr);

        if ((znode_hdr->flags & ZNIF_VISIBLE) &&
               (znode_hdr->ct & CT_SYSTEM_MASK) != CT_OPAQUE &&
               IntersectRegion (&ctxt->inv_rgn, dirty_rgn, rgn)) {

            switch (znode_hdr->ct & CT_SYSTEM_MASK) {
            case CT_COLORKEY:
                SetMemDCColorKey (znode_hdr->mem_dc,
                        MEMDC_FLAG_SRCCOLORKEY,
                        DWORD2Pixel (znode_hdr->mem_dc, znode_hdr->ct_arg));
                break;

            case CT_LOGICALPIXEL:
                _WRN_PRINTF("CT_LOGICALPIXEL is not implemented\n");
                break;

            case CT_ALPHACHANNEL:
                SetMemDCColorKey (znode_hdr->mem_dc, 0, 0);
                SetMemDCAlpha (znode_hdr->mem_dc,
                        MEMDC_FLAG_SRCALPHA, (BYTE)znode_hdr->ct_arg);
                break;

            case CT_ALPHAPIXEL:
                SetMemDCAlpha (znode_hdr->mem_dc,
                        MEMDC_FLAG_SRCPIXELALPHA, 0);
                break;

            case CT_BLURRED:
                _WRN_PRINTF("CT_BLURRED is not implemented\n");
                break;

            default:
                assert (0); // never touch here.
                break;
            }

            SelectClipRegion (HDC_SCREEN_SYS, &ctxt->inv_rgn);
            //SelectClipRect (HDC_SCREEN_SYS, &znode_hdr->rc);
            BitBlt (znode_hdr->mem_dc,
                    0, 0, RECTW(znode_hdr->rc), RECTH(znode_hdr->rc),
                    HDC_SCREEN_SYS,
                    znode_hdr->rc.left, znode_hdr->rc.top, 0);
            EmptyClipRgn (&ctxt->inv_rgn);
        }

        ServerReleaseWinZNodeHeader (NULL, prev);

        prev = ServerGetPrevZNode (NULL, prev, NULL);
    }
}

static void composite_on_dirty_region (CompositorCtxt* ctxt, int from)
{
    CLIPRGN tmp_rgn;
    int next;

    /* tmp_rgn will be used to save the dirty region for lucent window znodes
       above the current znode. */
    InitClipRgn (&tmp_rgn, &ctxt->cliprc_heap);

    /* subtract the opaque window znodes above the current znode from
       the dirty region. */
    if (from > 0 && subtract_opaque_win_znodes_above (ctxt, from) > 0) {
        if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
            return;
        }
    }

    /* generate the effective dirty region for windows by substracting
       the region of visible popup menus. */
    IntersectRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, &ctxt->wins_rgn);
    if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
        return;
    }

    /* save dirty region for compositing the lucent znodes above
       the current znode. */
    if (get_lucent_win_znodes_above (ctxt, from))
        CopyRegion (&tmp_rgn, &ctxt->dirty_rgn);

    /* compositing the current window znode and the znodes below it */
    if (from <= 0)
        next = ServerGetNextZNode (NULL, 0, NULL);
    else
        next = from;

    while (next > 0) {
        composite_opaque_win_znode (ctxt, next);
        next = ServerGetNextZNode (NULL, next, NULL);
    }

    /* compositing the wallpaper */
    composite_wallpaper (ctxt);

    /* composite all lucent znodes intersected with dirty region. */
    composite_all_lucent_win_znodes (ctxt, &tmp_rgn);

    EmptyClipRgn (&tmp_rgn);
    SyncUpdateDC (HDC_SCREEN_SYS);
}

static void on_dirty_win (CompositorCtxt* ctxt, MG_Layer* layer, int zidx)
{
    const ZNODEHEADER* znode_hdr = NULL;
    CLIPRGN *rgn;

    /* the fallback compositor only manages znodes on the topmost layer. */
    if (layer != mgTopmostLayer || zidx <= 0)
        return;

    _DBG_PRINTF("called: %d\n", zidx);

    /* generate the dirty region */
    znode_hdr = ServerGetWinZNodeHeader (NULL, zidx, (void**)&rgn, TRUE);
    assert (znode_hdr);

    if (generate_dirty_region (ctxt, znode_hdr, rgn)) {
        if (znode_hdr->changes == 0) {
            /* It's the first exposure of the window. */
            _MG_PRINTF ("It is the first exposure of the window (%d): %u\n",
                    zidx, znode_hdr->dirty_age);
        }

        if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
            goto ret;
        }

        composite_on_dirty_region (ctxt, zidx);
    }

ret:
    ServerReleaseWinZNodeHeader (NULL, zidx);
}

static void on_dirty_wpp (CompositorCtxt* ctxt)
{
    const ZNODEHEADER* znode_hdr;
 
    //_DBG_PRINTF ("called\n");
    znode_hdr = ServerGetWinZNodeHeader (NULL, 0, NULL, TRUE);
    assert (znode_hdr);

    if (generate_dirty_region (ctxt, znode_hdr, NULL)) {
        tile_dirty_region_for_wallpaper (ctxt);
        subtract_opaque_win_znodes_above (ctxt, 0);
        //subtract_opaque_ppp_znodes (ctxt);

        if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
            goto ret;
        }

        composite_on_dirty_region (ctxt, 0);
    }

ret:
    ServerReleaseWinZNodeHeader (NULL, 0);
}

static void rebuild_wins_region (CompositorCtxt* ctxt)
{
    int i, nr_ppps;
    const ZNODEHEADER* znode_hdr = NULL;

    SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);

    nr_ppps = ServerGetPopupMenusCount ();
    for (i = 0; i < nr_ppps; i++) {
        CLIPRGN* rgn;
        znode_hdr = ServerGetPopupMenuZNodeHeader (i, (void**)&rgn, FALSE);
        assert (znode_hdr);
        assert (rgn);

        SubtractRegion (&ctxt->wins_rgn, &ctxt->wins_rgn, rgn);
    }
}

static void calc_mainwin_pos (CompositorCtxt* ctxt, MG_Layer* layer,
            DWORD zt_type, int first_for_type, CALCPOSINFO* info)
{
    /* give a default size first */
    if (IsRectEmpty (&info->rc)) {

        info->rc.left = 0;
        info->rc.top = 0;
        if (info->ex_style & WS_EX_DIALOGBOX) {
            info->rc.right = g_rcScr.right / 2;
            info->rc.bottom = g_rcScr.bottom / 3;
        }
        else {
            info->rc.right = g_rcScr.right / 2;
            info->rc.bottom = g_rcScr.bottom;
        }
    }

    if (info->ex_style & WS_EX_DIALOGBOX) {
        // center the window vertically and horinzontally
        int width = info->rc.right - info->rc.left;
        int height = info->rc.bottom - info->rc.top;

        OffsetRect (&info->rc, (g_rcScr.right - width) >> 1,
                    (g_rcScr.bottom - height) >> 1);
    }
    else {
        if (first_for_type == 0) {
            info->rc.left = 0;
            info->rc.top = 0;
        }
        else {
            const ZNODEHEADER* znode_hdr;
            znode_hdr =
                ServerGetWinZNodeHeader (layer, first_for_type, NULL, FALSE);

            info->rc = znode_hdr->rc;
            OffsetRect (&info->rc,
                    DEF_OVERLAPPED_OFFSET_X, DEF_OVERLAPPED_OFFSET_Y);
        }
    }
}

static void purge_ppp_data (CompositorCtxt* ctxt, int zidx, void* data)
{
    EmptyClipRgn ((CLIPRGN*)data);
    mg_slice_delete (CLIPRGN, data);
}

static void purge_win_data (CompositorCtxt* ctxt, MG_Layer* layer,
        int zidx, void* data)
{
    EmptyClipRgn ((CLIPRGN*)data);
    mg_slice_delete (CLIPRGN, data);
}

static void on_showing_ppp (CompositorCtxt* ctxt, int zidx)
{
    CLIPRGN* rgn;

    rgn = mg_slice_new (CLIPRGN);
    InitClipRgn (rgn, &ctxt->cliprc_heap);
    ServerGetPopupMenuZNodeRegion (zidx, RGN_OP_SET | RGN_OP_FLAG_ABS, rgn);
    ServerSetPopupMenuZNodePrivateData (zidx, rgn);

    /* subtract region of this ppp from wins_rgn */
    SubtractRegion (&ctxt->wins_rgn, &ctxt->wins_rgn, rgn);
}

static void on_dirty_ppp (CompositorCtxt* ctxt, int zidx)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn;

    _DBG_PRINTF ("called\n");
    znode_hdr = ServerGetPopupMenuZNodeHeader (zidx, (void**)&rgn, TRUE);
    assert (znode_hdr);
    assert (rgn);

    if (generate_dirty_region (ctxt, znode_hdr, rgn)) {
        SelectClipRegion (HDC_SCREEN_SYS, &ctxt->dirty_rgn);
        BitBlt (znode_hdr->mem_dc, 0, 0,
                RECTW (znode_hdr->rc), RECTH (znode_hdr->rc),
                HDC_SCREEN_SYS, znode_hdr->rc.left, znode_hdr->rc.top, 0);

        SelectClipRect (HDC_SCREEN_SYS, &ctxt->rc_screen);
        EmptyClipRgn (&ctxt->dirty_rgn);
    }

    ServerReleasePopupMenuZNodeHeader (zidx);

    SyncUpdateDC (HDC_SCREEN_SYS);
}

static void on_hiding_ppp (CompositorCtxt* ctxt, int zidx)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn = NULL;

    znode_hdr = ServerGetPopupMenuZNodeHeader (zidx, (void**)&rgn, FALSE);
    assert (znode_hdr);

    assert (rgn);
    EmptyClipRgn ((CLIPRGN*)rgn);
    mg_slice_delete (CLIPRGN, rgn);
    ServerSetPopupMenuZNodePrivateData (zidx, NULL);
}

static void on_dirty_screen (CompositorCtxt* ctxt,
        MG_Layer* layer, DWORD cause_type, const RECT* rc_dirty)
{
    /* this compositor only manages znodes on the topmost layer. */
    if (layer == NULL)
        layer = mgTopmostLayer;
    else if (layer != mgTopmostLayer)
        return;

    _DBG_PRINTF ("called\n");
    /* generate the dirty region */
    if (rc_dirty) {
        SetClipRgn (&ctxt->dirty_rgn, &ctxt->rc_screen);
        IntersectClipRect (&ctxt->dirty_rgn, rc_dirty);
        if (IsEmptyClipRgn (&ctxt->dirty_rgn)) {
            return;
        }
    }
    else {
        // all screen is dirty.
        SetClipRgn (&ctxt->dirty_rgn, &ctxt->rc_screen);
    }

    /* if the cause type is popup menu:
     * - rebuild the region for windows;
     * - composite all popup menus.
     *
     * note that if any popup menu was composited to the screen,
     * the dirty region will be shrank, because this compositor
     * handles any popup menu as opaque.
     */
    if (cause_type == ZNIT_POPUPMENU) {
        rebuild_wins_region (ctxt);
        composite_ppp_znodes (ctxt);
    }

    composite_on_dirty_region (ctxt, 0);
}

static void refresh (CompositorCtxt* ctxt)
{
    rebuild_wins_region (ctxt);
    on_dirty_screen (ctxt, NULL, ZNIT_NULL, NULL);
}

static void on_closed_menu (CompositorCtxt* ctxt, const RECT* rc_bound)
{
    _DBG_PRINTF ("called\n");
    /* reset the wins_rgn with screen rectangle, because the menu closed */
    SetClipRgn (&ctxt->wins_rgn, &ctxt->rc_screen);

    /* composite the dirty screen rectangle.
     * we pass cause_type with ZNIT_NULL for no any popup menu currently. */
    on_dirty_screen (ctxt, NULL, ZNIT_NULL, rc_bound);
}

static void on_showing_win (CompositorCtxt* ctxt, MG_Layer* layer, int zidx)
{
    CLIPRGN* rgn;

    // the fallback compositor only manages znodes on the topmost layer.
    if (layer != mgTopmostLayer)
        return;

    _DBG_PRINTF ("called: %d\n", zidx);
    rgn = mg_slice_new (CLIPRGN);
    InitClipRgn (rgn, &ctxt->cliprc_heap);
    ServerGetWinZNodeRegion (layer, zidx, RGN_OP_SET | RGN_OP_FLAG_ABS, rgn);
    ServerSetWinZNodePrivateData (layer, zidx, rgn);
}

static void on_hiding_win (CompositorCtxt* ctxt, MG_Layer* layer, int zidx)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn = NULL;

    // the fallback compositor only manages znodes on the topmost layer.
    if (layer != mgTopmostLayer)
        return;

    _DBG_PRINTF ("called: %d\n", zidx);
    znode_hdr = ServerGetWinZNodeHeader (layer, zidx, (void**)&rgn, FALSE);
    assert (znode_hdr);

    assert (rgn);
    EmptyClipRgn (rgn);
    mg_slice_delete (CLIPRGN, rgn);
    ServerSetWinZNodePrivateData (layer, zidx, NULL);
}

/* for both on_raised_win and on_changed_ct */
static void refresh_win (CompositorCtxt* ctxt, MG_Layer* layer, int zidx)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn = NULL;

    // the fallback compositor only manages znodes on the topmost layer.
    if (layer != mgTopmostLayer)
        return;

    _DBG_PRINTF ("called\n");
    znode_hdr = ServerGetWinZNodeHeader (layer, zidx, (void**)&rgn, FALSE);
    assert(znode_hdr);

    if (!(znode_hdr->flags & ZNIF_VISIBLE)) {
        return;
    }

    assert (rgn);
    CopyRegion (&ctxt->dirty_rgn, rgn);
    if (IntersectClipRect (&ctxt->dirty_rgn, &ctxt->rc_screen))
        composite_on_dirty_region (ctxt, zidx);
}

static void on_moved_win (CompositorCtxt* ctxt, MG_Layer* layer, int zidx,
        const RECT* rc_org)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn = NULL;

    _DBG_PRINTF("called: %d\n", zidx);
    // the fallback compositor only manages znodes on the topmost layer.
    if (layer != mgTopmostLayer)
        return;

    znode_hdr = ServerGetWinZNodeHeader (layer, zidx, (void**)&rgn, FALSE);
    assert (znode_hdr);

    if (!(znode_hdr->flags & ZNIF_VISIBLE)) {
        return;
    }

    assert (rgn);
    CopyRegion (&ctxt->dirty_rgn, rgn);
    // re-generate the region
    ServerGetWinZNodeRegion (layer, zidx,
            RGN_OP_SET | RGN_OP_FLAG_ABS, rgn);
    ServerSetWinZNodePrivateData (layer, zidx, rgn);
    UnionRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, rgn);

    /* calculate the dirty region */
    if (IntersectClipRect (&ctxt->dirty_rgn, &ctxt->rc_screen))
        composite_on_dirty_region (ctxt, zidx);
}

static void on_changed_rgn (CompositorCtxt* ctxt, MG_Layer* layer,
        int zidx, const RECT* rc_org_bound)
{
    const ZNODEHEADER* znode_hdr;
    CLIPRGN* rgn = NULL;

    _DBG_PRINTF("called: %d\n", zidx);
    /* the fallback compositor only manages znodes on the topmost layer. */
    if (layer != mgTopmostLayer)
        return;

    /* update the region of the current window znode. */
    znode_hdr = ServerGetWinZNodeHeader (layer, zidx, (void**)&rgn, FALSE);
    assert (znode_hdr);

    if (!(znode_hdr->flags & ZNIF_VISIBLE)) {
        return;
    }

    assert (rgn);
    CopyRegion (&ctxt->dirty_rgn, rgn);

    // re-generate the region
    ServerGetWinZNodeRegion (layer, zidx,
            RGN_OP_SET | RGN_OP_FLAG_ABS, rgn);
    ServerSetWinZNodePrivateData (layer, zidx, rgn);

    /* calculate the dirty region */
    UnionRegion (&ctxt->dirty_rgn, &ctxt->dirty_rgn, rgn);
    if (IntersectClipRect (&ctxt->dirty_rgn, &ctxt->rc_screen))
        composite_on_dirty_region (ctxt, zidx);
}

CompositorOps __mg_fallback_compositor = {
    initialize: initialize,
    terminate: terminate,
    refresh: refresh,
    calc_mainwin_pos: calc_mainwin_pos,
    purge_ppp_data: purge_ppp_data,
    purge_win_data: purge_win_data,
    on_dirty_ppp: on_dirty_ppp,
    on_dirty_win: on_dirty_win,
    on_dirty_wpp: on_dirty_wpp,
    on_dirty_screen: on_dirty_screen,
    on_showing_ppp: on_showing_ppp,
    on_hiding_ppp: on_hiding_ppp,
    on_closed_menu: on_closed_menu,
    on_showing_win: on_showing_win,
    on_hiding_win: on_hiding_win,
    on_raised_win: refresh_win,
    on_changed_ct: refresh_win,
    on_changed_rgn: on_changed_rgn,
    on_moved_win: on_moved_win,
};

#endif /* defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING) */

