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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** desktop.c: The Desktop module.
**
** Current maintainer: Wei Yongming
**
*/

#include "license.h"
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/00_minigui.dat.c"
#endif

ZORDERINFO* __mg_zorder_info;

/* pointer to desktop window */
PMAINWIN __mg_dsk_win;

/* handle to desktop window */
HWND __mg_hwnd_desktop;

/* the capture window */
HWND __mg_captured_wnd;

/* handle to the ime window - server only. */
HWND __mg_ime_wnd;

IME_STATUS_INFO __mg_ime_status_info;

/* default window procedures */
#ifdef _MGHAVE_VIRTUAL_WINDOW
WNDPROC __mg_def_proc[4];
#else
WNDPROC __mg_def_proc[3];
#endif

static BLOCKHEAP sg_FreeInvRectList;

#ifndef _MGSCHEMA_COMPOSITING
static BLOCKHEAP sg_FreeClipRectList;
static GCRINFO sg_ScrGCRInfo;
static CLIPRGN sg_UpdateRgn;

#else /* defined _MGSCHEMA_COMPOSITING */

#define DO_COMPSOR_OP(op)                                               \
do {                                                                    \
    CompositorCtxt* ctxt;                                               \
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);    \
    if (ops && ops->op) {                                               \
        ops->op(ctxt);                                                  \
    }                                                                   \
} while (0)

#define DO_COMPSOR_OP_ARGS(op, ...)                                     \
do {                                                                    \
    CompositorCtxt* ctxt;                                               \
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);    \
    if (ops && ops->op) {                                               \
        ops->op(ctxt, __VA_ARGS__);                                     \
    }                                                                   \
} while (0)

#endif /* defined _MGSCHEMA_COMPOSITING */

#define ZT_TOOLTIP      0x00000080
#define ZT_GLOBAL       0x00000040
#define ZT_SCREENLOCK   0x00000020
#define ZT_DOCKER       0x00000010
#define ZT_HIGHER       0x00000008
#define ZT_NORMAL       0x00000004
#define ZT_LAUNCHER     0x00000002
#define ZT_ALL          0x000000FF

static const int _zts_for_level [] = {
    ZT_TOOLTIP, ZT_GLOBAL, ZT_SCREENLOCK, ZT_DOCKER,
    ZT_HIGHER, ZT_NORMAL, ZT_LAUNCHER };

static const int _zof_types_for_level [] = {
    ZOF_TYPE_TOOLTIP, ZOF_TYPE_GLOBAL, ZOF_TYPE_SCREENLOCK, ZOF_TYPE_DOCKER,
    ZOF_TYPE_HIGHER, ZOF_TYPE_NORMAL, ZOF_TYPE_LAUNCHER };

typedef BOOL (* CB_ONE_ZNODE) (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node);

#if 0   /* deprecated code */
PGCRINFO kernel_GetGCRgnInfo (HWND hWnd)
{
    return ((PMAINWIN)hWnd)->pGCRInfo;
}
#endif  /* deprecated code */

typedef struct _DEF_CONTEXT
{
    PBITMAP bg;
    int x, y;
} DEF_CONTEXT;

#if defined(_MGRM_PROCESSES)
static DEF_CONTEXT g_def_context;
#endif

static inline ZORDERINFO* get_zorder_info (int cli)
{
#if defined (_MGRM_THREADS) || defined (_MGRM_STANDALONE)
    return __mg_zorder_info;
#else
    return get_zi_from_client (cli);
#endif
}

#ifndef _MGSCHEMA_COMPOSITING
static BOOL subtract_rgn_by_node (PCLIPRGN region, const ZORDERINFO* zi,
               const ZORDERNODE* node)
{
    if (node->idx_mask_rect != 0) {
        MASKRECT *firstmaskrect=NULL, *maskrect;
        int idx, x, y;
        RECT tmprc={0};

        firstmaskrect = GET_MASKRECT(zi);
        idx = node->idx_mask_rect;
        /* clip all mask rect */
        while(idx) {
            maskrect = firstmaskrect + idx;
            x = maskrect->left;
            y = maskrect->top;

            /* convert to screen coordinate*/
            x = x + node->rc.left;
            y = y + node->rc.top;

            SetRect(&tmprc, x, y,
                        x+maskrect->right - maskrect->left,
                        y+maskrect->bottom - maskrect->top);

            IntersectRect(&tmprc, &tmprc, &(node->rc));

            SubtractClipRect(region, &tmprc);

            idx = maskrect->next;
        }
    }
    else
        SubtractClipRect(region, &(node->rc));

    return !IsEmptyClipRgn(region);
}

#else   /* not defined _MGSCHEMA_COMPOSITING */

static int get_znode_mask_bound (int cli, int idx_znode, RECT* rc_bound)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes, *node;

    zi = get_zorder_info (cli);
    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    node = nodes + idx_znode;
    if (node->idx_mask_rect != 0) {
        MASKRECT *firstmaskrect = NULL, *maskrect;
        int idx;
        RECT rc;

        SetRect (rc_bound, 0, 0, 0, 0);

        firstmaskrect = GET_MASKRECT (zi);
        idx = node->idx_mask_rect;
        while (idx) {
            maskrect = firstmaskrect + idx;
            rc.left = maskrect->left + node->rc.left;
            rc.top  = maskrect->top + node->rc.top;
            rc.right = rc.left + (maskrect->right - maskrect->left);
            rc.bottom = rc.top + (maskrect->bottom - maskrect->top);

            IntersectRect (&rc, &rc, &(node->rc));
            UnionRect (rc_bound, rc_bound, &rc);

            idx = maskrect->next;
        }
    }
    else
        *rc_bound = node->rc;

    return 0;
}

#endif /* defined _MGSCHEMA_COMPOSITING */

/* TODO: check round corners here */
static int pt_in_maskrect (const ZORDERINFO* zi,
       const ZORDERNODE* nodes, int x, int y)
{
    int cx, cy;
    RECT tmprc;
    MASKRECT *maskrect, *firstmaskrect;
    int idx = nodes->idx_mask_rect;

    if (idx != 0 && PtInRect (&(nodes->rc), x, y)) {
        cx = x - nodes->rc.left;
        cy = y - nodes->rc.top;

        firstmaskrect = GET_MASKRECT(zi);
        while(idx != 0) {
            maskrect = firstmaskrect + idx;
            SetRect (&tmprc, maskrect->left, maskrect->top,
                        maskrect->right, maskrect->bottom);

            if (PtInRect (&tmprc, cx, cy))
                return TRUE;

            idx = maskrect->next;
        }
        return FALSE;
    }
    else {
        return PtInRect(&(nodes->rc), x, y);
    }
}

static int get_znode_at_point (const ZORDERINFO* zi,
                const ZORDERNODE* nodes, int x, int y)
{
    int level;
    int slot = 0;

    /* Since 5.0.0 */
    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        slot = zi->first_in_levels[level];
        for (; slot > 0; slot = nodes[slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                            pt_in_maskrect (zi, &nodes[slot], x, y))
                goto ret;
        }
    }

ret:
    return slot;
}

static void unchain_znode (unsigned char* usage_bmp,
                ZORDERNODE* nodes, int idx_znode)
{
    if (nodes [idx_znode].prev) {
        nodes [nodes [idx_znode].prev].next = nodes [idx_znode].next;
    }
    if (nodes [idx_znode].next) {
        nodes [nodes [idx_znode].next].prev = nodes [idx_znode].prev;
    }

    __mg_slot_clear_use (usage_bmp, idx_znode);
    nodes [idx_znode].flags = 0;
}

static void clean_znode_maskrect (ZORDERINFO* zi, ZORDERNODE* nodes,
        int idx_znode)
{
    int idx, tmp;
    MASKRECT *first;

    first = GET_MASKRECT(zi);
    idx = nodes [idx_znode].idx_mask_rect;

    while (idx) {
       __mg_slot_clear_use((unsigned char*)GET_MASKRECT_USAGEBMP(zi), idx);
       (first+idx)->prev = 0;
       tmp = (first+idx)->next;
       (first+idx)->next = 0;
       idx = tmp;
    }

    nodes[idx_znode].idx_mask_rect = 0;
}

static int do_for_all_znodes (void* context, const ZORDERINFO* zi,
                CB_ONE_ZNODE cb, DWORD types)
{
    int slot;
    int count = 0;
    int level;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        if (types & _zts_for_level[level]) {

            slot = zi->first_in_levels[level];
            for (; slot > 0; slot = nodes[slot].next) {
                if (cb (context, zi, nodes + slot))
                    count ++;
            }
        }
    }

    return count;
}

static void dskScreenToClient (PMAINWIN pWin,
            const RECT* rcScreen, RECT* rcClient)
{
    PCONTROL pParent;

    rcClient->top = rcScreen->top - pWin->ct;
    rcClient->left = rcScreen->left - pWin->cl;
    rcClient->right = rcScreen->right - pWin->cl;
    rcClient->bottom = rcScreen->bottom - pWin->ct;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcClient->top -= pParent->ct;
        rcClient->left -= pParent->cl;
        rcClient->right -= pParent->cl;
        rcClient->bottom -= pParent->ct;
    }
}

static void dskScreenToWindow (PMAINWIN pWin,
                const RECT* rcScreen, RECT* rcWindow)
{
    PCONTROL pParent;

    rcWindow->top = rcScreen->top - pWin->top;
    rcWindow->left = rcScreen->left - pWin->left;
    rcWindow->right = rcScreen->right - pWin->left;
    rcWindow->bottom = rcScreen->bottom - pWin->top;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcWindow->top -= pParent->ct;
        rcWindow->left -= pParent->cl;
        rcWindow->right -= pParent->cl;
        rcWindow->bottom -= pParent->ct;
    }
}

static void dskClientToScreen (PMAINWIN pWin,
                const RECT* rcClient, RECT* rcScreen)
{
    PCONTROL pParent;

    rcScreen->top = rcClient->top + pWin->ct;
    rcScreen->left = rcClient->left + pWin->cl;
    rcScreen->right = rcClient->right + pWin->cl;
    rcScreen->bottom = rcClient->bottom + pWin->ct;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcScreen->top += pParent->ct;
        rcScreen->left += pParent->cl;
        rcScreen->right += pParent->cl;
        rcScreen->bottom += pParent->ct;
    }
}

static void dskGetWindowRectInScreen (PMAINWIN pWin, RECT* prc)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    pParent = pCtrl = (PCONTROL)pWin;

    prc->left = pCtrl->left;
    prc->top  = pCtrl->top;
    prc->right = pCtrl->right;
    prc->bottom = pCtrl->bottom;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

void __mg_update_window (HWND hwnd,
        int left, int top, int right, int bottom)
{
    PMAINWIN pWin = (PMAINWIN)hwnd;

    if (pWin &&
            ((pWin->WinType == TYPE_CONTROL &&
              (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) ||
             pWin->WinType != TYPE_CONTROL) && pWin->dwStyle & WS_VISIBLE) {
        RECT rcInv;
        SetRect(&rcInv, left, top, right, bottom);

        _DBG_PRINTF ("Update window (%s): %d, %d, %d, %d\n",
                pWin->spCaption, left, top, right, bottom);

        if (IsRectEmpty (&rcInv)) {
            SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
            InvalidateRect ((HWND)pWin, NULL, TRUE);
        }
        else {
            RECT rcTemp, rcWin;

            if (pWin->WinType == TYPE_CONTROL &&
                    (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)){
                dskGetWindowRectInScreen (pWin, &rcWin);
            }
            else
                GetWindowRect(hwnd, &rcWin);

            /* XXX: when we move a window fast, there are some dirty rectangles
               were not updated. We update whole area of the window for a work
               around. */
            if (IntersectRect (&rcTemp, &rcWin, &rcInv)) {
                dskScreenToWindow (pWin, &rcTemp, &rcInv);
                SendAsyncMessage ((HWND)pWin,
                                MSG_NCPAINT, 0, 0); // (LPARAM)(&rcInv));
                dskScreenToClient (pWin, &rcTemp, &rcInv);
                InvalidateRect ((HWND)pWin, NULL/*&rcInv*/, TRUE);
            }
            else {
                _DBG_PRINTF ("IGNORED update\n");
            }
        }
    }
}

#ifndef _MGSCHEMA_COMPOSITING
static int update_client_window (ZORDERNODE* znode, const RECT* rc)
{
#if defined(_MGRM_PROCESSES)
    if (!mgIsServer)
        return -1;

    if (znode->cli != 0) {
        if (rc) {
            _DBG_PRINTF ("Update window (%s): %d, %d, %d x %d\n",
                    znode->caption, rc->left, rc->top,
                    RECTWP(rc), RECTHP(rc));

            if (IsRectEmpty (&znode->dirty_rc)) {
                SetRect (&znode->dirty_rc,
                    rc->left, rc->top, rc->right, rc->bottom);
            }
            else {
                GetBoundRect (&znode->dirty_rc, &znode->dirty_rc, rc);
            }
            mgClients [znode->cli].has_dirty = TRUE;
        }
    }
    else
#endif /* defined _MGRM_PROCESSES */
    {
        if (rc)
            __mg_update_window (znode->hwnd, rc->left, rc->top,
                            rc->right, rc->bottom);
        else
            __mg_update_window (znode->hwnd, 0, 0, 0, 0);
    }

    return 0;
}

static BOOL _cb_update_znode (void* context,
                const ZORDERINFO* zi, ZORDERNODE* znode)
{
    const RECT* rc = (RECT*)context;

    if (znode->flags & ZOF_VISIBLE &&
            znode->flags & ZOF_IF_REFERENCE) {
        update_client_window (znode, rc);
        znode->flags &= ~ZOF_IF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}

static BOOL _cb_intersect_rc (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    RECT* rc = (RECT*)context;

    if (node->flags & ZOF_VISIBLE && DoesIntersect (rc, &node->rc)) {
        node->age++;
        return TRUE;
    }

    return FALSE;
}

static BOOL _cb_update_rc (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    CLIPRGN* cliprgn = (CLIPRGN*)context;

    if (node->flags & ZOF_VISIBLE &&
                    subtract_rgn_by_node(cliprgn, zi, node)) {
        node->age++;
        node->flags |= ZOF_IF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}

static BOOL _cb_exclude_rc (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    if (!(node->flags & ZOF_VISIBLE))
        return FALSE;

    if (node->idx_mask_rect != 0) {
        int idx = node->idx_mask_rect;
        MASKRECT *first = GET_MASKRECT(zi);
        int x, y;
        RECT rc;

        while (idx != 0) {
            x = node->rc.left + (first+idx)->left;
            y = node->rc.top + (first+idx)->top;
            SetRect(&rc, x, y,
                        x+(first+idx)->right-(first+idx)->left,
                        y+(first+idx)->bottom-(first+idx)->top);
            ExcludeClipRect (HDC_SCREEN_SYS, &rc);
            idx = (first+idx)->next;
        }
    }
    else
        ExcludeClipRect (HDC_SCREEN_SYS, &node->rc);

    return TRUE;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

#ifdef _MGSCHEMA_COMPOSITING
static inline void reset_window (PMAINWIN pWin, RECT* rcWin) { }
#else
static void reset_window (PMAINWIN pWin, RECT* rcWin)
{
    PGCRINFO pGCRInfo;
    RECT rcScr, rcTemp;

    pGCRInfo = pWin->pGCRInfo;
    rcScr = GetScreenRect();
    IntersectRect (&rcTemp, rcWin, &rcScr);
    SetClipRgn (&pGCRInfo->crgn, &rcTemp);
}
#endif

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
/*for unix system, using read/write lock*/
static inline void lock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_rwlock_wrlock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void unlock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_rwlock_unlock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void lock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_rwlock_rdlock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void unlock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_rwlock_unlock(&((ZORDERINFO*)zi)->rwlock);
}
#else /* __NOUNIX__ */
/*for non-unix system, using mutex*/
static inline void lock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_mutex_lock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void unlock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_mutex_unlock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void lock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_mutex_lock(&((ZORDERINFO*)zi)->rwlock);
}

static inline void unlock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_mutex_unlock(&((ZORDERINFO*)zi)->rwlock);
}
#endif /* __NOUNIX__ */

#elif defined(_MGRM_STANDALONE)
static inline void lock_zi_for_change (const ZORDERINFO* zi) { }
static inline void unlock_zi_for_change (const ZORDERINFO* zi) { }
static inline void lock_zi_for_read (const ZORDERINFO* zi) { }
static inline void unlock_zi_for_read (const ZORDERINFO* zi) { }
#else /* for procs */
/* NULL. see desktop-procs.c */
#endif /* _MGRM_THREADS */

static int RestrictControlRect(PMAINWIN pWin, RECT *minimal)
{
    RECT rcMove;
    int off_x = 0, off_y = 0;

    PCONTROL pCtrl = gui_Control((HWND)pWin);
    PCONTROL pRoot = (PCONTROL)(pWin->pMainWin);

    if (pCtrl == NULL)
        return FALSE;

    do {
        PCONTROL pParent = pCtrl;

        rcMove.left = pRoot->cl + off_x;
        rcMove.top  = pRoot->ct + off_y;
        rcMove.right = pRoot->cr + off_x;
        rcMove.bottom = pRoot->cb + off_y;

        dskScreenToClient (pWin, &rcMove, &rcMove);

        if (!IntersectRect (minimal, minimal, &rcMove)) {
            SetRect (minimal, 0, 0, 0, 0);
            return FALSE;
        }

        off_x += pRoot->cl;
        off_y += pRoot->ct;

        if (pRoot == pCtrl->pParent)
            break;

        while (TRUE) {
            if (pRoot->children == pParent->pParent->children) {
                pRoot = pParent;
                break;
            }
            pParent = pParent->pParent;
        }
    } while (TRUE);

    return TRUE;
}

static int dskScrollMainWindow (PMAINWIN pWin, PSCROLLWINDOWINFO pswi)
{
    HDC hdc;
    RECT rcClient, rcScreen, rcInvalid;
    BOOL inved = FALSE;
#ifdef _MGSCHEMA_COMPOSITING
    CLIPRECT crcOne;
#endif
    PCLIPRECT pcrc;
    PINVRGN pInvRgn;
    PCLIPRGN prgn = NULL;

    dskClientToScreen (pWin, pswi->rc1, &rcScreen);

    //BUGFIX: if the MainWindow is AutoSecondaryDC, the secondaryDC and
    //client dc would be diffirent, so we must get the scondaryDC,
    //the update to client dc (dongjunjie 2010/7/28)
    //hdc = GetClientDC ((HWND)pWin);
    hdc = get_effective_dc (pWin, TRUE);

#ifndef _MGSCHEMA_COMPOSITING
    pcrc = pWin->pGCRInfo->crgn.head;
#else
    crcOne.next = crcOne.prev = NULL;
    crcOne.rc.left = pWin->pMainWin->left;
    crcOne.rc.top = pWin->pMainWin->top;
    crcOne.rc.right = pWin->pMainWin->right;
    crcOne.rc.bottom = pWin->pMainWin->bottom;
    pcrc = &crcOne;
#endif
    while (pcrc) {
        RECT rcMove;

        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);
        if (!IntersectRect (&rcMove, &rcMove, pswi->rc1)) {
            pcrc = pcrc->next;
            continue;
        }

        if (pWin->dwExStyle & WS_EX_TRANSPARENT) {
            /* set invalidate rect. */
            InvalidateRect ((HWND)pWin, &rcMove, TRUE);
            inved = TRUE;
        }
        else {
            RestrictControlRect (pWin, &rcMove);

            SelectClipRect (hdc, &rcMove);

            BitBlt (hdc, rcMove.left, rcMove.top,
                    rcMove.right - rcMove.left,
                    rcMove.bottom - rcMove.top,
                    hdc,
                    pswi->iOffx + rcMove.left, pswi->iOffy + rcMove.top, 0);
        }

        pcrc = pcrc->next;
    }
    //ReleaseDC (hdc);
    //BUGFIXED: we must update the secondaryDC to clientDC, to ensure
    //the secondaryDC and clientDC are same (dongjunjie 2010/07/08)
    if (pWin->pMainWin->secondaryDC) {
        HDC real_dc = GetClientDC ((HWND)pWin->pMainWin);
        __mg_update_secondary_dc(pWin, hdc, real_dc, pswi->rc1, HT_CLIENT);
        ReleaseDC (real_dc);
    }
    release_effective_dc(pWin, hdc);

    GetClientRect((HWND)pWin, &rcClient);

    pInvRgn = &pWin->InvRgn;
    if (!pInvRgn->frozen) {
#ifdef _MGRM_THREADS
        pthread_mutex_lock (&pInvRgn->lock);
#endif
        /*scroll whole screen, offset invalid region*/
        if (EqualRect (pswi->rc1, &rcClient))
            OffsetRegion (&(pInvRgn->rgn), pswi->iOffx, pswi->iOffy);
        else
            OffsetRegionEx (&(pInvRgn->rgn), &rcClient,
                pswi->rc1, pswi->iOffx, pswi->iOffy);
#ifdef _MGRM_THREADS
        pthread_mutex_unlock (&pInvRgn->lock);
#endif
    }

#ifndef _MGSCHEMA_COMPOSITING
    prgn = CreateClipRgn ();
    lock_zi_for_read (__mg_zorder_info);
    CopyRegion (prgn, &pWin->pGCRInfo->crgn);
    pcrc = prgn->head;
    unlock_zi_for_read (__mg_zorder_info);
#else
    pcrc = &crcOne;
#endif

    while (pcrc) {
        BOOL bNeedInvalidate = FALSE;
        RECT rcMove;
        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);

        RestrictControlRect (pWin, &rcMove);

        rcInvalid = rcMove;

        if (pswi->iOffx < 0) {
            rcInvalid.left = rcInvalid.right + pswi->iOffx;
            bNeedInvalidate = TRUE;
        }
        else if (pswi->iOffx > 0) {
            rcInvalid.right = rcInvalid.left + pswi->iOffx;
            bNeedInvalidate = TRUE;
        }

        /*
         * BUGFIXED: offx and offy would make the two diffrent areas invalidate
         * we should invalid both them (dongjunjie) 2010/07/30
         *
         *                   content
         *  ---------------------------
         *  |//: offX                 |
         *  |//:                      |
         *  |//:       View           |
         *  |//:                      |
         *  |//:                      |
         *  |/////////////////////////| offY
         *  ---------------------------
         *  // - represent area need the Need invalidate
         *  see the to area must be invalidate
         */
        if (bNeedInvalidate) {
            InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            rcInvalid = rcMove; //restore the invalidate area
            bNeedInvalidate = FALSE; //resotre the inved value
            inved = TRUE;
        }

        if (pswi->iOffy < 0) {
            rcInvalid.top = rcInvalid.bottom + pswi->iOffy;
            bNeedInvalidate = TRUE;
        }
        else if (pswi->iOffy > 0) {
            rcInvalid.bottom = rcInvalid.top + pswi->iOffy;
            bNeedInvalidate = TRUE;
        }

        if (bNeedInvalidate) {
            InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }

        pcrc = pcrc->next;
    }

    if (prgn)
        DestroyClipRgn (prgn);

    if (inved) {
        PostMessage ((HWND)pWin, MSG_PAINT, 0, 0);
    }

    return 0;
}

#ifndef _MGSCHEMA_COMPOSITING
void __mg_unlock_gcrinfo (PDC pdc)
{
#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&pdc->pGCRInfo->lock);
#endif
    unlock_zi_for_read (__mg_zorder_info);
    return;
}

static BOOL _cb_recalc_gcrinfo (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    PGCRINFO gcrinfo;

    if (!(node->flags & ZOF_VISIBLE))
        return FALSE;

    gcrinfo = (PGCRINFO)context;
    return subtract_rgn_by_node(&gcrinfo->crgn, zi, node);
}

void __mg_lock_recalc_gcrinfo (PDC pdc)
{
    PGCRINFO gcrinfo;
    PMAINWIN mainwin;
    ZORDERINFO* zi = __mg_zorder_info;
    ZORDERNODE* menu_nodes, *nodes;
    int i, slot, idx_znode, level_from, level_to;
    unsigned short idx;
    CLIPRGN invisible_rgn;

    mainwin = (PMAINWIN)(pdc->hwnd);
    gcrinfo = mainwin->pGCRInfo;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&pdc->pGCRInfo->lock);
#endif
    lock_zi_for_read (zi);

    menu_nodes = GET_MENUNODE(zi);
    nodes = menu_nodes + DEF_NR_POPUPMENUS;
    idx_znode = mainwin->idx_znode;

    if (gcrinfo->old_zi_age == nodes [idx_znode].age) {
        return;
    }

    idx = nodes [idx_znode].idx_mask_rect;
    reset_window (mainwin, &nodes [idx_znode].rc);
    /* when idx=0, this is HDC_SCREEN_SYS */
    if (idx != 0) {
        InitClipRgn (&invisible_rgn, &sg_FreeClipRectList);
        SetClipRgn (&invisible_rgn, &nodes [idx_znode].rc);

        subtract_rgn_by_node(&invisible_rgn, zi, &nodes[idx_znode]);
        SubtractRegion (&gcrinfo->crgn, &gcrinfo->crgn, &invisible_rgn);
        EmptyClipRgn(&invisible_rgn);
    }

    /* clip by popup menus */
    for (i = 0; i < zi->nr_popupmenus; i++) {
        SubtractClipRect (&gcrinfo->crgn, &menu_nodes [i].rc);
    }

    /* Since 5.0.0: check all levels */
    slot = 0;
    level_from = -1;
    level_to = -1;
    switch (nodes[idx_znode].flags & ZOF_TYPE_MASK) {
    case ZOF_TYPE_TOOLTIP:
        slot = zi->first_tooltip;
        break;

    case ZOF_TYPE_GLOBAL:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_TOOLTIP;
        slot = zi->first_global;
        break;

    case ZOF_TYPE_SCREENLOCK:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_GLOBAL;
        slot = zi->first_screenlock;
        break;

    case ZOF_TYPE_DOCKER:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_SCREENLOCK;
        slot = zi->first_docker;
        break;

    case ZOF_TYPE_HIGHER:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_DOCKER;
        slot = zi->first_topmost;
        break;

    case ZOF_TYPE_NORMAL:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_TOPMOST;
        slot = zi->first_normal;
        break;

    case ZOF_TYPE_LAUNCHER:
        level_from  = ZLIDX_TOOLTIP;
        level_to    = ZLIDX_NORMAL;
        slot = zi->first_launcher;
        break;

    case ZOF_TYPE_DESKTOP:
        level_from  = -1;
        level_to    = 0;
        break;

    default:
        break;
    }

    /* subtract by all visible znodes above current level */
    if (level_from >= 0) {
        for (i = level_from; i <= level_to; i++) {
            do_for_all_znodes (gcrinfo, zi,
                    _cb_recalc_gcrinfo, _zts_for_level[i]);
        }
    }
    else if (level_to == 0) {
        do_for_all_znodes (gcrinfo, zi,
                        _cb_recalc_gcrinfo, ZT_ALL);
    }

    /* subtract by visible znodes above the znode in current level */
    while (slot && slot != idx_znode) {

        if (nodes [slot].flags & ZOF_VISIBLE)
           subtract_rgn_by_node (&gcrinfo->crgn, zi, &nodes[slot]);

        slot = nodes [slot].next;
    }
    gcrinfo->old_zi_age = nodes [idx_znode].age;
    gcrinfo->age++;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

/*
 * Init a window's global clipping region.
 */
#ifdef _MGSCHEMA_COMPOSITING
static inline void dskInitGCRInfo (PMAINWIN pWin) {}
#else
static void dskInitGCRInfo (PMAINWIN pWin)
{
    RECT rcWin, rcScr, rcTemp;

    dskGetWindowRectInScreen (pWin, &rcWin);

#ifdef _MGRM_THREADS
    pthread_mutex_init (&pWin->pGCRInfo->lock, NULL);
#endif
    pWin->pGCRInfo->age = 0;
    pWin->pGCRInfo->old_zi_age = 0;

    InitClipRgn (&pWin->pGCRInfo->crgn, &sg_FreeClipRectList);
    rcScr = GetScreenRect();
    IntersectRect (&rcTemp, &rcWin, &rcScr);
    SetClipRgn (&pWin->pGCRInfo->crgn, &rcTemp);
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

/*
 * Init a window's invalid region.
 */
static void dskInitInvRgn (PMAINWIN pWin)
{
#ifdef _MGRM_THREADS
    pthread_mutex_init (&pWin->InvRgn.lock, NULL);
#endif
    pWin->InvRgn.frozen = 0;
    InitClipRgn (&pWin->InvRgn.rgn, &sg_FreeInvRectList);
    MAKE_REGION_INFINITE(&pWin->InvRgn.rgn);
}

int __kernel_get_next_znode (const ZORDERINFO* zi, int from)
{
    int next = 0;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    if (from <= 0) {
        next = zi->first_tooltip;
        if (next == 0)
            next = zi->first_global;
        if (next == 0)
            next = zi->first_screenlock;
        if (next == 0)
            next = zi->first_docker;
        if (next == 0)
            next = zi->first_topmost;
        if (next == 0)
            next = zi->first_normal;
        if (next == 0)
            next = zi->first_launcher;

        return next;
    }

    next = nodes [from].next;
    if (next > 0)
        return next;

    /* next is still zero. */
    switch (nodes [from].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_TOOLTIP:
            next = zi->first_global;
            if (next == 0)
                next = zi->first_screenlock;
            if (next == 0)
                next = zi->first_docker;
            if (next == 0)
                next = zi->first_topmost;
            if (next == 0)
                next = zi->first_normal;
            if (next == 0)
                next = zi->first_launcher;
            break;

        case ZOF_TYPE_GLOBAL:
            next = zi->first_screenlock;
            if (next == 0)
                next = zi->first_docker;
            if (next == 0)
                next = zi->first_topmost;
            if (next == 0)
                next = zi->first_normal;
            if (next == 0)
                next = zi->first_launcher;
            break;

        case ZOF_TYPE_SCREENLOCK:
            next = zi->first_docker;
            if (next == 0)
                next = zi->first_topmost;
            if (next == 0)
                next = zi->first_normal;
            if (next == 0)
                next = zi->first_launcher;
            break;

        case ZOF_TYPE_DOCKER:
            next = zi->first_topmost;
            if (next == 0)
                next = zi->first_normal;
            if (next == 0)
                next = zi->first_launcher;
            break;

        case ZOF_TYPE_HIGHER:
            next = zi->first_normal;
            if (next == 0)
                next = zi->first_launcher;
            break;

        case ZOF_TYPE_NORMAL:
            next = zi->first_launcher;
            break;

        default:
            return -1;
    }

    return next;
}

int __kernel_get_prev_znode (const ZORDERINFO* zi, int from)
{
    int prev = 0;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    if (from <= 0) {
        if (zi->first_launcher) {
            prev = zi->first_launcher;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_normal) {
            prev = zi->first_normal;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_topmost) {
            prev = zi->first_topmost;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_docker) {
            prev = zi->first_docker;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_screenlock) {
            prev = zi->first_screenlock;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }

        return prev;
    }

    prev = nodes [from].prev;
    if (prev > 0)
        return prev;

    /* prev is still zero. */
    switch (nodes [from].flags & ZOF_TYPE_MASK) {
    case ZOF_TYPE_LAUNCHER:
        if (zi->first_normal) {
            prev = zi->first_normal;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_topmost) {
            prev = zi->first_topmost;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_docker) {
            prev = zi->first_docker;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_screenlock) {
            prev = zi->first_screenlock;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        break;

    case ZOF_TYPE_NORMAL:
        if (zi->first_topmost) {
            prev = zi->first_topmost;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_docker) {
            prev = zi->first_docker;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_screenlock) {
            prev = zi->first_screenlock;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }

        break;

    case ZOF_TYPE_HIGHER:
        if (zi->first_docker) {
            prev = zi->first_docker;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_screenlock) {
            prev = zi->first_screenlock;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }

        break;

    case ZOF_TYPE_DOCKER:
        if (zi->first_screenlock) {
            prev = zi->first_screenlock;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }

        break;

    case ZOF_TYPE_SCREENLOCK:
        if (zi->first_global) {
            prev = zi->first_global;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        else if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }

        break;

    case ZOF_TYPE_GLOBAL:
        if (zi->first_tooltip) {
            prev = zi->first_tooltip;
            while (nodes [prev].next) {
                prev = nodes [prev].next;
            }
        }
        break;

    default:
        return -1;
    }

    return prev;
}

static int get_next_activable_mainwin (const ZORDERINFO* zi, int from)
{
    int next;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    do {
        next = __kernel_get_next_znode (zi, from);
        if (next <= 0)
            break;

        if (nodes [next].flags & ZOF_TF_MAINWIN
                && (nodes [next].flags & ZOF_VISIBLE)
                && !(nodes [next].flags & ZOF_DISABLED))
            return next;

        from = next;
    } while (1);

    return 0;
}

#if 0   /* move to window.c since 5.0.0 */
/*
 * Add new hosted main window.
 */
static void dskAddNewHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;

    pHosted->pNextHosted = NULL;

    head = pHosting->pFirstHosted;
    if (head) {
        while (head) {
            prev = head;
            head = head->pNextHosted;
        }

        prev->pNextHosted = pHosted;
    }
    else
        pHosting->pFirstHosted = pHosted;

    return;
}

/*
 * Remove a hosted main window.
 */
static void dskRemoveHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;

    head = pHosting->pFirstHosted;
    if (head == pHosted) {
        pHosting->pFirstHosted = head->pNextHosted;
        return;
    }

    while (head) {
        prev = head;
        head = head->pNextHosted;

        if (head == pHosted) {
            prev->pNextHosted = head->pNextHosted;
            return;
        }
    }

    return;
}
#endif  /* moved code */

/*
 * Moves a window to topmost.
 *
 * dskIsTopMost
 * dskMove2Top
 */
static void dskSetPrimitiveChildren (PMAINWIN pWin, BOOL bSet)
{
    PMAINWIN pParent = (PMAINWIN) pWin->hParent;

    if (bSet) {
        while (pParent) {
            pParent->hPrimitive = (HWND)pWin;

            pWin = pParent;
            pParent = (PMAINWIN) pWin->hParent;
        }
    }
    else {
        while (pParent) {
            pParent->hPrimitive = 0;

            pWin = pParent;
            pParent = (PMAINWIN) pWin->hParent;
        }
    }
}

static BOOL dskIsTopMost (PMAINWIN pWin)
{
    int level;
    BOOL ret = FALSE;

    lock_zi_for_read (__mg_zorder_info);

    /* Since 5.0.0: handle all levels */
    for (level = 0; level < NR_ZORDER_LEVELS; level++) {
        if (pWin->idx_znode == __mg_zorder_info->first_in_levels[level]) {
            ret = TRUE;
            break;
        }
    }

    unlock_zi_for_read (__mg_zorder_info);
    return ret;
}

#ifdef _MGHAVE_MENU
static PTRACKMENUINFO sg_ptmi;
static HMENU sg_DesktopMenu = 0;      /* system global desktop menu handle */

// call back proc of tracking menu.
// defined in Menu module.
int PopupMenuTrackProc (PTRACKMENUINFO ptmi,
        int message, WPARAM wParam, LPARAM lParam);

static int srvForceCloseMenu (int cli)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    int i, ret = 0;
#if defined(_MGRM_PROCESSES)
    int cli_trackmenu;
#endif
    ZORDERNODE* menu_nodes;
    RECT rc_bound;
#ifndef _MGSCHEMA_COMPOSITING
    ZORDERNODE* win_nodes;
    RECT rc_screen;
#endif

    if (zi->cli_trackmenu < 0 || zi->nr_popupmenus == 0)
        return 0;

    menu_nodes = GET_MENUNODE(zi);
#ifndef _MGSCHEMA_COMPOSITING
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;
#endif
    SetRect (&rc_bound, 0, 0, 0, 0);

    /* lock zi for change */
    lock_zi_for_change (zi);

    for (i = 0; i < zi->nr_popupmenus; i++) {
        GetBoundRect (&rc_bound, &rc_bound, &menu_nodes [i].rc);
    }

#ifndef _MGSCHEMA_COMPOSITING
    SetClipRgn (&sg_UpdateRgn, &rc_bound);

    /* check influenced window zorder nodes */
    do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_ALL);

    rc_screen = GetScreenRect ();
    if (SubtractClipRect (&sg_UpdateRgn, &rc_screen)) {
        win_nodes [0].age++;
        win_nodes [0].flags |= ZOF_IF_REFERENCE;
    }
#else   /* defined _MGSCHEMA_COMPOSITING */
    for (i = (zi->nr_popupmenus - 1); i >= 0; i--) {
        DO_COMPSOR_OP_ARGS (on_hiding_ppp, i);
        DeleteMemDC (menu_nodes[i].mem_dc);
    }
#endif  /* defined _MGSCHEMA_COMPOSITING */

#if defined(_MGRM_PROCESSES)
    cli_trackmenu = zi->cli_trackmenu;
#endif
    zi->cli_trackmenu = -1;
    zi->nr_popupmenus = 0;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* update all znode if it's dirty */
    do_for_all_znodes (&rc_bound, zi, _cb_update_znode, ZT_ALL);

    if (win_nodes [0].flags & ZOF_IF_REFERENCE) {
        SendMessage (HWND_DESKTOP,
                        MSG_ERASEDESKTOP, 0, (LPARAM)&rc_bound);
        win_nodes [0].flags &= ~ZOF_IF_REFERENCE;
    }
#else   /* not defined _MGSCHEMA_COMPOSITING */
    DO_COMPSOR_OP_ARGS (on_closed_menu, &rc_bound);
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /* notify the client to close the menu */
    {
#if defined(_MGRM_PROCESSES)
        MSG msg = {0, MSG_CLOSEMENU, 0, 0, __mg_tick_counter};

        if (cli_trackmenu)
            ret = __mg_send2client (&msg, mgClients + cli_trackmenu);
        else
#endif
            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);
    }

    return ret;
}

static int srvStartTrackPopupMenu (int cli, const RECT* rc, HWND ptmi,
        Uint32 surf_flags, size_t surf_size, int fd)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* menu_nodes;
#ifdef _MGSCHEMA_COMPOSITING
    HDC memdc = HDC_INVALID;
#else
    ZORDERNODE* win_nodes;
    RECT rc_screen;
#endif

    if (zi->cli_trackmenu >= 0 && zi->cli_trackmenu != cli) {
        srvForceCloseMenu (0);
    }

    if (zi->nr_popupmenus == zi->max_nr_popupmenus)
        return -1;

#ifdef _MGSCHEMA_COMPOSITING
    {
        GAL_Surface* surf;
        if (cli == 0) {
            surf = GetSurfaceFromDC (((PTRACKMENUINFO)ptmi)->dc);
            // prevent to free this surface when deleting memdc for znode.
            surf->refcount++;
        }
        else if (fd >= 0) {
            surf = GAL_AttachSharedRGBSurface (fd, surf_size, surf_flags, TRUE);
            close (fd);
        }
        else {
            _WRN_PRINTF("not server but fd for shared surface is invalid\n");
            return -1;
        }

        if (surf) {
            memdc = CreateMemDCFromSurface (surf);
            if (memdc == HDC_INVALID) {
                GAL_FreeSurface (surf);
                _WRN_PRINTF("failed to create memory dc for znode\n");
                return -1;
            }
        }
        else {
            _WRN_PRINTF("KERNEL: failed to attach to surface\n");
            return -1;
        }
    }
#endif /* def _MGSCHEMA_COMPOSITING */

    menu_nodes = GET_MENUNODE(zi);

    /* lock zi for change */
    lock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;

    /* check influenced window zorder nodes */
    do_for_all_znodes ((void*)rc, zi, _cb_intersect_rc, ZT_ALL);

    rc_screen = GetScreenRect();
    if (DoesIntersect (rc, &rc_screen)) {
        win_nodes [0].age++;
    }
#endif /* not defined _MGSCHEMA_COMPOSITING */

    menu_nodes [zi->nr_popupmenus].flags = ZOF_TYPE_POPUPMENU;
    menu_nodes [zi->nr_popupmenus].rc = *rc;
    menu_nodes [zi->nr_popupmenus].hwnd = ptmi;
#ifdef _MGSCHEMA_COMPOSITING
    menu_nodes [zi->nr_popupmenus].changes = 0;
    menu_nodes [zi->nr_popupmenus].ct = CT_OPAQUE;
    menu_nodes [zi->nr_popupmenus].ct_arg = 0;
    menu_nodes [zi->nr_popupmenus].mem_dc = memdc;
    menu_nodes [zi->nr_popupmenus].dirty_rcs = NULL;
#endif
    menu_nodes [zi->nr_popupmenus].priv_data = NULL;

    if (zi->cli_trackmenu == -1)
        zi->cli_trackmenu = cli;
    zi->nr_popupmenus ++;

#ifdef _MGSCHEMA_COMPOSITING
    DO_COMPSOR_OP_ARGS (on_showing_ppp, zi->nr_popupmenus - 1);
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    return zi->nr_popupmenus - 1;
}

static int srvEndTrackPopupMenu (int cli, int idx_znode)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* menu_nodes;
#ifndef _MGSCHEMA_COMPOSITING
    ZORDERNODE* win_nodes;
    RECT rc, rc_screen;
#endif

    if (zi->cli_trackmenu != cli
                    || zi->nr_popupmenus != (idx_znode + 1))
        return -1;

    menu_nodes = GET_MENUNODE(zi);
#ifndef _MGSCHEMA_COMPOSITING
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

#ifdef _MGSCHEMA_COMPOSITING
    DO_COMPSOR_OP_ARGS (on_hiding_ppp, idx_znode);
    DeleteMemDC (menu_nodes [idx_znode].mem_dc);
#else   /* not defined _MGSCHEMA_COMPOSITING */
    rc = menu_nodes [idx_znode].rc;
    SetClipRgn (&sg_UpdateRgn, &rc);

    /* check influenced window zorder nodes */
    do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_ALL);

    rc_screen = GetScreenRect();
    if (SubtractClipRect (&sg_UpdateRgn, &rc_screen)) {
        win_nodes [0].age++;
        win_nodes [0].flags |= ZOF_IF_REFERENCE;
    }
#endif  /* not defined _MGSCHEMA_COMPOSITING */

    zi->nr_popupmenus --;
    if (zi->nr_popupmenus == 0)
        zi->cli_trackmenu = -1;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* update all znode if it's dirty */
    do_for_all_znodes (&rc, zi, _cb_update_znode, ZT_ALL);

    if (win_nodes [0].flags & ZOF_IF_REFERENCE) {
        SendMessage (HWND_DESKTOP,
                        MSG_ERASEDESKTOP, 0, (LPARAM)&rc);
        win_nodes [0].flags &= ~ZOF_IF_REFERENCE;
    }
#else /* not defined _MGSCHEMA_COMPOSITING */
    DO_COMPSOR_OP_ARGS (on_dirty_screen, NULL,
            menu_nodes [idx_znode].flags & ZOF_TYPE_FLAG_MASK,
            &menu_nodes [idx_znode].rc);
#endif /* defined _MGSCHEMA_COMPOSITING */

    return 0;
}

static BOOL dskCloseMenu (void)
{
    if (sg_ptmi == NULL)
        return FALSE;

    SendNotifyMessage (sg_ptmi->hwnd,
            MSG_DEACTIVEMENU,
                       (WPARAM)sg_ptmi->pmb,
                       (LPARAM)sg_ptmi->pmi);

    PopupMenuTrackProc (sg_ptmi, MSG_CLOSEMENU, 0, 0);

    sg_ptmi = NULL;

    return TRUE;
}

#endif  /* defined _MGHAVE_MENU */

int __mg_post_msg_by_znode (const ZORDERINFO* zi, int znode,
                int message, WPARAM wParam, LPARAM lParam)
{
    int ret = 0;
    ZORDERNODE* nodes;

    if (znode < 0)
        return -1;

    nodes = GET_ZORDERNODE(zi);

    if (nodes [znode].cli == 0) {
        ret = PostMessage (nodes [znode].main_win,
                            message, wParam, lParam);
    }
#if defined (_MGRM_PROCESSES)
    else {
        MSG msg = {nodes [znode].main_win,
                message, wParam, lParam, __mg_tick_counter};

        ret = __mg_send2client (&msg, mgClients + nodes [znode].cli);
    }
#endif

    return ret;
}

static int
post_msg_by_znode_p (const ZORDERINFO* zi, const ZORDERNODE* znode,
                int message, WPARAM wParam, LPARAM lParam)
{
    int ret = 0;

    if (!znode->main_win) {
        return 0;
    }
    if (znode->cli == 0) {
        ret = PostMessage (znode->main_win, message, wParam, lParam);
    }
#if defined (_MGRM_PROCESSES)
    else {
        MSG msg = {znode->main_win,
                message, wParam, lParam, __mg_tick_counter};

        ret = __mg_send2client (&msg, mgClients + znode->cli);
    }
#endif

    return ret;
}

static HWND dskSetActiveZOrderNode (int cli, int idx_znode)
{
    int old_active = 0;
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;
    HWND old_hwnd = HWND_NULL, new_hwnd = HWND_NULL;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return HWND_INVALID;
    }

    nodes = GET_ZORDERNODE(zi);

    if ((__mg_ime_wnd && __mg_ime_wnd == nodes [idx_znode].main_win) ||
                    nodes [idx_znode].flags & ZOF_TF_TOOLWIN)
        return HWND_INVALID;

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

    if (zi->active_win) {
        old_active = zi->active_win;
    }

    zi->active_win = idx_znode;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    if (old_active) {
        if (nodes [old_active].cli == cli) {
            old_hwnd = nodes [old_active].hwnd;
        }
        else
            old_hwnd = HWND_OTHERPROC;
    }

    if (idx_znode) {
        if (nodes [idx_znode].cli == cli) {
            new_hwnd = nodes [idx_znode].hwnd;
        }
        else
            new_hwnd = HWND_OTHERPROC;
    }

    if (old_active && (nodes [zi->active_win].flags & ZOF_VISIBLE)) {
        post_msg_by_znode_p (zi, nodes + old_active,
                        MSG_NCACTIVATE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + old_active,
                        MSG_ACTIVE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + old_active,
                        MSG_KILLFOCUS, (WPARAM)new_hwnd, 0);
    }

    if (idx_znode) {
        post_msg_by_znode_p (zi, nodes + idx_znode,
                        MSG_NCACTIVATE, TRUE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode,
                        MSG_ACTIVE, TRUE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode,
                        MSG_SETFOCUS, (WPARAM)old_hwnd, 0);
    }

    return old_hwnd;
}

#if 0 /* Since 5.0.0, use strdup to duplicate the caption */
static void get_text_char_pos (PLOGFONT log_font, const char *text,
               int len, int fit_bytes, int *fit_chars, int *pos_chars)
{
    DEVFONT* sbc_devfont = log_font->devfonts[0];
    DEVFONT* mbc_devfont = log_font->devfonts[1];
    int len_cur_char;
    int left_bytes = len;
    int char_count = 0, bytes = 0;

    while (left_bytes > 0 &&  bytes < fit_bytes) {
        if (pos_chars) {
            pos_chars[char_count] = len - left_bytes;
        }
        if ((mbc_devfont) &&
            (len_cur_char = mbc_devfont->charset_ops->len_first_char
             ((const unsigned char*)text, left_bytes)) > 0) {
            char_count ++;
            left_bytes -= len_cur_char;
            text += len_cur_char;
            bytes += len_cur_char;
        } else {
            if ((len_cur_char = sbc_devfont->charset_ops->len_first_char
                 ((const unsigned char*)text, left_bytes)) > 0) {
                char_count ++;
                left_bytes -= len_cur_char;
                text += len_cur_char;
                bytes += len_cur_char;
            } else {
                break;
            }
        }
    }

    if (fit_chars) {
        *fit_chars = char_count;
    }
}
#endif

/* Since 5.0.0:
   Under compositing schema, we do not allocate mask rectangles
   for round corners. Instead, we get the region of window znode
   in ServerGetWinZNodeRegion.
 */
#ifndef _MGSCHEMA_COMPOSITING

#define do_with_round_rect(src, drc, idx)           \
    if (idx != 0) {                                 \
        if (drc[idx-1].left == src.left &&          \
                RECTW(drc[idx-1]) == RECTW(src)) {  \
            SetRect(&(drc[idx-1]),                  \
                    drc[idx-1].left,                \
                    drc[idx-1].top,                 \
                    drc[idx-1].right,               \
                    src.bottom);                    \
        }                                           \
        else {                                      \
            CopyRect(&(drc[idx]), &src);            \
            idx++;                                  \
        }                                           \
    }                                               \
    else {                                          \
        CopyRect(&(drc[idx]), &src);                \
        idx++;                                      \
    }

static int alloc_mask_rects_for_round_corners (ZORDERINFO* zi,
        ZORDERNODE* node, const DWORD flags, const RECT *rc)
{
    RECT* roundrc = NULL;
    RECT rect;
    int max_rect_num = 1, free_slot, rc_idx;
    int i, z, r = RADIUS_WINDOW_CORNERS;
    unsigned short idx;
    MASKRECT *firstmaskrect;

    if (flags & ZOF_TW_TROUNDCNS)
        max_rect_num += r;
    if (flags & ZOF_TW_BROUNDCNS)
        max_rect_num += r;

    roundrc = calloc (1, max_rect_num * sizeof(RECT));
    if (!roundrc)
        return -1;

    /*generate region of top round corner*/
    rc_idx = 0;
    if (flags & ZOF_TW_TROUNDCNS) {
        for(i = 0; i < r; i++)
        {
            z = sqrt (2*i*r - i*i);
            SetRect (&rect, rc->left+r-z, rc->top+i,
                            rc->left+RECTWP(rc)-r+z, rc->top+i+1);
            do_with_round_rect (rect, roundrc, rc_idx);
        }
    }
    else{
        SetRect (&rect, rc->left, rc->top,
                    rc->left+RECTWP(rc), rc->top+r);
        CopyRect (&(roundrc[rc_idx]), &rect);
        rc_idx++;
    }

    SetRect (&rect, rc->left, rc->top+r,
                rc->left+RECTWP(rc), rc->top+RECTHP(rc)-r);
    do_with_round_rect (rect, roundrc, rc_idx);

    /* generate region of bottom round corner*/
    if (flags & ZOF_TW_BROUNDCNS) {
        for(i = 1; i < r; i++)
        {
            z = sqrt (r*r-i*i);
            SetRect (&rect, rc->left+r-z,
                            rc->top+RECTHP(rc)-r+i-1,
                            rc->left+RECTWP(rc)-r+z,
                            rc->top+RECTHP(rc)-r+i+1);

            do_with_round_rect (rect, roundrc, rc_idx);
        }
    }
    else {
        SetRect (&rect, rc->left, rc->top+RECTHP(rc)-r,
                    rc->left+RECTWP(rc), rc->top+RECTHP(rc));
        do_with_round_rect (rect, roundrc, rc_idx);
    }

    /* if have enough mask rect */
    if (rc_idx >
            __mg_get_nr_idle_slots ((unsigned char*)GET_MASKRECT_USAGEBMP(zi),
                zi->size_maskrect_usage_bmp)) {
        free (roundrc);
        return -1;
    }

    firstmaskrect = GET_MASKRECT(zi);

    /* allocate space*/
    idx = 0;
    for (i = 0; i < rc_idx; i++) {
        free_slot = __mg_lookfor_unused_slot (
                            (unsigned char*)GET_MASKRECT_USAGEBMP(zi),
                            zi->size_maskrect_usage_bmp, 1);
        if (free_slot == -1) {
            _WRN_PRINTF ("KERNEL: __mg_lookfor_unused_slot failed\n");
            return -1;
        }

        (firstmaskrect + free_slot)->prev = idx;
        if (idx != 0) {
            (firstmaskrect + idx)->next = free_slot;
        }
        else {
            node->idx_mask_rect = free_slot;
        }

        idx = free_slot;
    }

    if (idx != 0) {
        (firstmaskrect + idx)->prev = 0;
    }

    /* get value */
    idx = node->idx_mask_rect;
    i = 0;
    while (idx != 0) {
        (firstmaskrect + idx)->left = (unsigned short)roundrc[i].left;
        (firstmaskrect + idx)->top = (unsigned short)roundrc[i].top;
        (firstmaskrect + idx)->right = (unsigned short)roundrc[i].right;
        (firstmaskrect + idx)->bottom = (unsigned short)roundrc[i].bottom;
        idx = (firstmaskrect + idx)->next;
        i++;
    }

    free (roundrc);

    return 0;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

#ifdef _MGSCHEMA_COMPOSITING
static inline int validate_compositing_type (DWORD flags, int type)
{
    if (flags & ZOF_TF_CONTROL) {
        return CT_OPAQUE;
    }
    else if (type < CT_OPAQUE || type > CT_MAX_VALUE)
        return CT_OPAQUE;

    return type;
}
#endif /* defined _MGSCHEMA_COMPOSITING */

static int AllocZOrderNodeEx (ZORDERINFO* zi, int cli, HWND hwnd, HWND main_win,
                DWORD flags, const RECT *rc, const char *caption,
                HDC mem_dc, int ct, DWORD ct_arg)
{
    DWORD type = flags & ZOF_TYPE_MASK;
    int *first = NULL, *nr_nodes = NULL;
    int free_slot = -1, old_first;
    ZORDERNODE* nodes;

    nodes = GET_ZORDERNODE(zi);

    /* Since 5.0.0: check special znode type first for MiniGUI-Processes */
#ifdef _MGRM_PROCESSES
    switch (type) {
    case ZOF_TYPE_TOOLTIP:
        if (zi->first_tooltip > 0 &&
                nodes [zi->first_tooltip].cli != cli) {
            flags &= ~ZOF_TYPE_MASK;
            flags |= ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_GLOBAL:
        if (cli != 0) {
            flags &= ~ZOF_TYPE_MASK;
            flags |= ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_SCREENLOCK:
        if (zi->first_screenlock > 0 &&
                nodes [zi->first_screenlock].cli != cli) {
            flags &= ~ZOF_TYPE_MASK;
            flags |= ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_DOCKER:
        if (zi->first_docker > 0 &&
                nodes [zi->first_docker].cli != cli) {
            flags &= ~ZOF_TYPE_MASK;
            flags |= ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_LAUNCHER:
        if (zi->first_launcher > 0 &&
                nodes [zi->first_launcher].cli != cli) {
            flags &= ~ZOF_TYPE_MASK;
            flags |= ZOF_TYPE_NORMAL;
        }
        break;
    }
#endif  /* defined _MGRM_PROCESSES */

    /* reacquire type */
    type = flags & ZOF_TYPE_MASK;
    switch (type) {
    case ZOF_TYPE_TOOLTIP:
        if (zi->nr_tooltips < zi->max_nr_tooltips) {
            first = &zi->first_tooltip;
            nr_nodes = &zi->nr_tooltips;
        }
        break;

    case ZOF_TYPE_GLOBAL:
        if (zi->nr_globals < zi->max_nr_globals) {
            first = &zi->first_global;
            nr_nodes = &zi->nr_globals;
        }
        break;

    case ZOF_TYPE_SCREENLOCK:
        if (zi->nr_screenlocks < zi->max_nr_screenlocks) {
            first = &zi->first_screenlock;
            nr_nodes = &zi->nr_screenlocks;
        }
        break;

    case ZOF_TYPE_DOCKER:
        if (zi->nr_dockers < zi->max_nr_dockers) {
            first = &zi->first_docker;
            nr_nodes = &zi->nr_dockers;
        }
        break;

    case ZOF_TYPE_HIGHER:
        if (zi->nr_topmosts < zi->max_nr_topmosts) {
            first = &zi->first_topmost;
            nr_nodes = &zi->nr_topmosts;
        }
        break;

    case ZOF_TYPE_NORMAL:
        if (zi->nr_normals < zi->max_nr_normals) {
            first = &zi->first_normal;
            nr_nodes = &zi->nr_normals;
        }
        break;

    case ZOF_TYPE_LAUNCHER:
        if (zi->nr_launchers < zi->max_nr_launchers) {
            first = &zi->first_launcher;
            nr_nodes = &zi->nr_launchers;
        }
        break;

    default:
        break;
    }

    if (first == NULL) {
        int level = ZOF_TYPE_TO_LEVEL_IDX (type);
        _WRN_PRINTF ("no free slot for the new znode: "
                "level (%d), max_nr (%d), nr (%d)\n",
                level, zi->nr_nodes_in_levels[level],
                zi->max_nr_nodes_in_levels[level]);
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0 && flags & ZOF_VISIBLE)
        srvForceCloseMenu (0);
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

    /* Since 5.0.0: tune code to avoid potential errors */
    {
        int len_bmp_specials = LEN_USAGE_BMP_SPECIAL(zi);

        if (IS_TYPE_SPECIAL (type)) {
            free_slot = __mg_lookfor_unused_slot (
                    (BYTE*)(zi + 1), len_bmp_specials, 1);

            if (free_slot >= MAX_NR_SPECIAL_ZNODES(zi))
                free_slot = -1;
        }
        else {
            free_slot = __mg_lookfor_unused_slot (
                    (BYTE*)(zi + 1) + len_bmp_specials,
                    LEN_USAGE_BMP_GENERAL(zi), 1);

            if (free_slot >= 0) {
                free_slot += len_bmp_specials << 3;
            }

            assert (free_slot >= MAX_NR_SPECIAL_ZNODES(zi));

            if (free_slot >= MAX_NR_ZNODES(zi))
                free_slot = -1;
        }
    }

    if (-1 == free_slot) {
        /* unlock zorderinfo for change. */
        unlock_zi_for_change (zi);
        _WRN_PRINTF ("cannot allocate slot for new znode\n");
        return -1;
    }

    nodes [free_slot].flags = flags;
    nodes [free_slot].rc = *rc;
    nodes [free_slot].cli = cli;
    nodes [free_slot].hwnd = hwnd;
    nodes [free_slot].main_win = main_win;
    nodes [free_slot].lock_count = 0;
#ifdef _MGSCHEMA_COMPOSITING
    nodes [free_slot].changes = 0;
    nodes [free_slot].ct = validate_compositing_type (flags, ct);
    nodes [free_slot].ct_arg = ct_arg;
    nodes [free_slot].mem_dc = mem_dc;
    nodes [free_slot].dirty_rcs = NULL;
#else
    nodes [free_slot].age = 1;
    nodes [free_slot].dirty_rc.left = 0;
    nodes [free_slot].dirty_rc.top = 0;
    nodes [free_slot].dirty_rc.right = 0;
    nodes [free_slot].dirty_rc.bottom = 0;
#endif
    nodes [free_slot].idx_mask_rect = 0;
    nodes [free_slot].priv_data = NULL;

#ifndef _MGSCHEMA_COMPOSITING
    if (flags & ZOF_TW_TROUNDCNS || flags & ZOF_TW_BROUNDCNS) {
        RECT cli_rect;

        SetRect (&cli_rect, 0, 0, RECTW(nodes[free_slot].rc),
                RECTH(nodes[free_slot].rc));
        alloc_mask_rects_for_round_corners (__mg_zorder_info,
                &nodes[free_slot], flags, &cli_rect);
    }
#endif /* not defined _MGSCHEMA_COMPOSITING */

    if (caption) {
        /* Since 5.0.0, use strdup to duplicate the caption */
        nodes[free_slot].caption = strdup (caption);
    }
    else {
        nodes[free_slot].caption = NULL;
    }

    /* check influenced zorder nodes */
    if (flags & ZOF_VISIBLE) {
        /* Since 5.0.0 */
#ifndef _MGSCHEMA_COMPOSITING
        int level, slot;
        RECT rc_screen = GetScreenRect();

        for (level = 0; level < NR_ZORDER_LEVELS; level++) {

            if (type >= _zof_types_for_level[level]) {
                slot = zi->first_in_levels[level];
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE &&
                            DoesIntersect (&nodes [free_slot].rc,
                                &nodes [slot].rc)) {
                        nodes [slot].age++;
                    }
                }
            }
        }

        /* for destkop */
        if (DoesIntersect (&nodes [free_slot].rc, &rc_screen)) {
            nodes [0].age++;
        }
#else   /* not defined _MGSCHEMA_COMPOSITING */
        DO_COMPSOR_OP_ARGS (on_showing_win,
                __mg_get_layer_from_zi(zi), free_slot);
#endif  /* defined _MGSCHEMA_COMPOSITING */
    }

    /* Since 5.0.0. Support for always top znode. */
    if (*first == 0 || !(nodes [*first].flags & ZOF_IF_ALWAYSTOP)) {
        old_first = *first;
        nodes [old_first].prev = free_slot;
        nodes [free_slot].prev = 0;
        nodes [free_slot].next = old_first;
    }
    else {
        int pre_idx = *first;
        while (*first) {
            if ((nodes [*first].flags & ZOF_IF_ALWAYSTOP)) {
                pre_idx = *first;
                first = &nodes[*first].next;
            }
            else
                break;
        }

        old_first = pre_idx;
        nodes [free_slot].prev = old_first;
        nodes [free_slot].next = nodes [old_first].next;
        nodes [nodes [old_first].next].prev = free_slot;
        nodes [old_first].next = free_slot;
    }

    *first = free_slot;

#if 0   /* old code without ZOF_IF_ALWAYSTOP */
    if (first) {    // not a fixed znode
        /* chain the new node */
        old_first = *first;
        nodes [old_first].prev = free_slot;
        nodes [free_slot].prev = 0;
        nodes [free_slot].next = old_first;
        *first = free_slot;
    }
#endif  /* old code without ZOF_IF_ALWAYSTOP */

    if (nr_nodes)
        *nr_nodes += 1;

    /* unlock zi for change ... */
    unlock_zi_for_change (zi);

    return free_slot;
}

static inline int AllocZOrderNode (int cli, HWND hwnd, HWND main_win,
                DWORD flags, const RECT *rc, const char *caption,
                HDC mem_dc, int ct, DWORD ct_arg)
{
    return AllocZOrderNodeEx (get_zorder_info(cli), cli, hwnd, main_win,
            flags, rc, caption, mem_dc, ct, ct_arg);
}

static int FreeZOrderNodeEx (ZORDERINFO* zi, int idx_znode, HDC* memdc)
{
    DWORD flags, type;
    RECT rc;
    int level, *first = NULL, *nr_nodes = NULL;
    ZORDERNODE* nodes;
    int old_active, next_active;
#ifdef _MGSCHEMA_COMPOSITING
    MG_Layer* layer = __mg_get_layer_from_zi(zi);
#endif

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0 && nodes [idx_znode].flags & ZOF_VISIBLE)
        srvForceCloseMenu (0);
#endif

    flags = nodes [idx_znode].flags;
    type = flags & ZOF_TYPE_MASK;
    rc = nodes [idx_znode].rc;

    /* Since 5.0.0 */
    level = ZOF_TYPE_TO_LEVEL_IDX(type);
    if (MG_UNLIKELY (level < 0 || level > ZLIDX_LAUNCHER)) {
        assert (0);
        return -1;
    }

    first = zi->first_in_levels + level;
    nr_nodes = zi->nr_nodes_in_levels + level;

#ifdef _MGSCHEMA_COMPOSITING
    if (memdc)
        *memdc = nodes[idx_znode].mem_dc;
#endif

    /* please lock zi for change*/
    lock_zi_for_change (zi);

    /* Since 5.0.0, use strdup to duplicate the caption */
    if (nodes[idx_znode].caption) {
        free (nodes[idx_znode].caption);
        nodes[idx_znode].caption = NULL;
    }

    /* Free round corners mask rect. */
    if (flags & ZOF_TW_TROUNDCNS || flags & ZOF_TW_BROUNDCNS) {
        clean_znode_maskrect (zi, nodes, idx_znode);
    }

#ifndef _MGSCHEMA_COMPOSITING
    SetClipRgn (&sg_UpdateRgn, &nodes [idx_znode].rc);

    /* check influenced zorder nodes */
    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int slot;
        RECT rc_screen = GetScreenRect ();

        slot = nodes [idx_znode].next;
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                nodes [slot].age++;
                nodes [slot].flags |= ZOF_IF_REFERENCE;
            }
        }

        /* Since 5.0.0 */
        for (level = 0; level < NR_ZORDER_LEVELS; level++) {
            if (type > _zof_types_for_level[level]) {
                slot = zi->first_in_levels[level];
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE &&
                            subtract_rgn_by_node(&sg_UpdateRgn, zi,
                                &nodes[slot])) {
                        nodes [slot].age++;
                        nodes [slot].flags |= ZOF_IF_REFERENCE;
                    }
                }
            }
        }

        if (SubtractClipRect (&sg_UpdateRgn, &rc_screen)) {
            nodes [0].age++;
            nodes [0].flags |= ZOF_IF_REFERENCE;
        }
    }
#else   /* defined _MGSCHEMA_COMPOSITING */
    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        DO_COMPSOR_OP_ARGS (on_hiding_win, layer, idx_znode);
    }
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /* XXX: get next activable window znode before freeing the current znode */
    next_active = get_next_activable_mainwin (zi, idx_znode);
    old_active = zi->active_win;
    if (idx_znode == zi->active_win)
        zi->active_win = 0;

    /* unchain it */
    unchain_znode ((unsigned char*)(zi+1), nodes, idx_znode);
    nodes [idx_znode].hwnd = HWND_NULL;
    nodes [idx_znode].cli = -1;

    if (*first == idx_znode) {
        *first = nodes [idx_znode].next;
    }
    *nr_nodes -= 1;

    /* unlock zi for change  */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* update all znode if it's dirty */
    do_for_all_znodes (&rc, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_IF_REFERENCE) {
        SendMessage (HWND_DESKTOP,
                        MSG_ERASEDESKTOP, 0, (WPARAM)&rc);
        nodes [0].flags &= ~ZOF_IF_REFERENCE;
    }
#else  /* not defined _MGSCHEMA_COMPOSITING */
    if (flags & ZOF_VISIBLE) {
        DO_COMPSOR_OP_ARGS (on_dirty_screen,
                layer, flags & ZOF_TYPE_FLAG_MASK, &rc);
    }
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /* if active_win is this window, change it */
    if (idx_znode == old_active) {
        dskSetActiveZOrderNode (nodes [next_active].cli, next_active);
    }

    return 0;
}

static inline int FreeZOrderNode (int cli, int idx_znode, HDC* memdc)
{
    return FreeZOrderNodeEx (get_zorder_info(cli), idx_znode, memdc);
}

static DWORD get_znode_flags_from_style (PMAINWIN pWin)
{
    DWORD zt_type = 0;
    DWORD ex_styles;

    if (pWin->dwStyle & WS_CHILD)
        ex_styles = pWin->pMainWin->dwExStyle;
    else
        ex_styles = pWin->dwExStyle;

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        /* Always be global ones */
        zt_type |= ZOF_TYPE_GLOBAL;
    } else
#endif

    {
        /* Since 5.0.0 */
        switch (ex_styles & WS_EX_WINTYPE_MASK) {
        case WS_EX_WINTYPE_TOOLTIP:
            zt_type |= ZOF_TYPE_TOOLTIP;
            break;

#ifndef _MGRM_PROCESSES
        case WS_EX_WINTYPE_GLOBAL:
            zt_type |= ZOF_TYPE_GLOBAL;
            break;
#endif
        case WS_EX_WINTYPE_SCREENLOCK:
            zt_type |= ZOF_TYPE_SCREENLOCK;
            break;

        case WS_EX_WINTYPE_DOCKER:
            zt_type |= ZOF_TYPE_DOCKER;
            break;

        case WS_EX_WINTYPE_LAUNCHER:
            zt_type |= ZOF_TYPE_LAUNCHER;
            break;

        case WS_EX_WINTYPE_HIGHER:
            zt_type |= ZOF_TYPE_HIGHER;
            break;

        default:
            zt_type |= ZOF_TYPE_NORMAL;
            break;
        }
    }

    if (pWin->dwStyle & WS_CHILD) {
        zt_type |= ZOF_TF_CONTROL;
    }
    else {
        if (pWin->dwExStyle & WS_EX_TOOLWINDOW)
            zt_type |= ZOF_TF_TOOLWIN;
        else
            zt_type |= ZOF_TF_MAINWIN;

        /* XXX: Since 5.2.0 */
        if (pWin->dwStyle & WS_MINIMIZE) {
            zt_type |= ZOF_MINIMIZED;
        }
        else if (pWin->dwStyle & WS_MAXIMIZE) {
            zt_type |= ZOF_MAXIMIZED;
        }

        if (pWin->dwStyle & WS_ALWAYSTOP)
            zt_type |= ZOF_IF_ALWAYSTOP;
    }

    if (pWin->dwStyle & WS_VISIBLE) {
        zt_type |= ZOF_VISIBLE;
    }

    if (pWin->dwStyle & WS_DISABLED)
        zt_type |= ZOF_DISABLED;

    if (pWin->dwExStyle & WS_EX_TROUNDCNS)
        zt_type |= ZOF_TW_TROUNDCNS;

    if (pWin->dwExStyle & WS_EX_BROUNDCNS)
        zt_type |= ZOF_TW_BROUNDCNS;

    return zt_type;
}

static int AllocZOrderMaskRect (int cli, int idx_znode,
        int flags, const RECT4MASK *rc, const int nr_rc)
{
    MASKRECT *firstmaskrect;
    ZORDERNODE* nodes;
    ZORDERINFO* zi = get_zorder_info(cli);
    int free_slot, i, cur_idx, idx, old_num = 0;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    /* lock zi for change */
    lock_zi_for_change (zi);

    nodes = GET_ZORDERNODE(zi);
    firstmaskrect = GET_MASKRECT(zi);

    /*get mask rect number*/
    idx = nodes[idx_znode].idx_mask_rect;

    while(idx) {
        old_num++;
        idx = ((MASKRECT *)(firstmaskrect+idx))->next;
    }

    if (nr_rc > old_num) {
        /* check the number of mask rect if enough */
        int idle =
            __mg_get_nr_idle_slots((unsigned char*)GET_MASKRECT_USAGEBMP(zi),
            zi->size_maskrect_usage_bmp);
        if (idle < nr_rc-old_num) {
            unlock_zi_for_change (zi);
            return -1;
        }

        /*add new space to Znode*/
        idx = nodes[idx_znode].idx_mask_rect;
        for(i = 0; i < nr_rc-old_num; i++) {
            free_slot = __mg_lookfor_unused_slot (
                            (unsigned char*)GET_MASKRECT_USAGEBMP(zi),
                            zi->size_maskrect_usage_bmp, 1);
            if (free_slot == -1) {
                unlock_zi_for_change (zi);
                return -1;
            }

            (firstmaskrect+free_slot)->next = idx;
            if (idx != 0) {
                (firstmaskrect+idx)->prev = free_slot;
            }
            idx = free_slot;
        }

        if (idx != 0) {
            (firstmaskrect+idx)->prev = 0;
        }
    }
    else {
        for(i =0; i < old_num-nr_rc; i++) {
            idx = nodes[idx_znode].idx_mask_rect;
            __mg_slot_clear_use((unsigned char*)GET_MASKRECT_USAGEBMP(zi), idx);
            idx = ((MASKRECT *)(firstmaskrect+idx))->next;
        }
    }

    nodes[idx_znode].idx_mask_rect = idx;

    /* the slot must be larger than zero */
    i = 0;
    cur_idx = nodes[idx_znode].idx_mask_rect;
    while(cur_idx) {
        /* there is no slots in the bitmap */
        (firstmaskrect+cur_idx)->left   = rc[i].left;
        (firstmaskrect+cur_idx)->top    = rc[i].top;
        (firstmaskrect+cur_idx)->right  = rc[i].right;
        (firstmaskrect+cur_idx)->bottom = rc[i].bottom;

        cur_idx = (firstmaskrect+cur_idx)->next;
        i++;
    }

    /* unlock zi for change ... */
    unlock_zi_for_change (zi);

    return 0;
}

static int FreeZOrderMaskRect (int cli, int idx_znode)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);

    /* lock zi for change */
    lock_zi_for_change (zi);

    clean_znode_maskrect (zi, nodes, idx_znode);

    /* unlock zi for change  */
    unlock_zi_for_change (zi);
    return 0;
}

#ifndef _MGSCHEMA_COMPOSITING
static int update_client_window_rgn (int cli, HWND hwnd);
#endif

static int dskMove2Top (int cli, int idx_znode);
static int dskShowWindow (int cli, int idx_znode);
static int dskHideWindow (int cli, int idx_znode);
static ZORDERINFO* get_zorder_info (int cli);

#ifdef _MG_ENABLE_SCREENSAVER
static void dskRefreshAllClient (const RECT* invrc)
{
#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        ZORDERINFO* zi = get_zorder_info (0);
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0,
                (LPARAM)(invrc));

        lock_zi_for_read (zi);
        SetClipRgn (&sg_UpdateRgn, invrc);
        /* check influenced window zorder nodes */
        do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_ALL);
        /* update all znode if it's dirty */
        do_for_all_znodes ((void*)invrc, zi, _cb_update_znode, ZT_ALL);
        unlock_zi_for_read (zi);
    }
#endif
}

static int dskCreateTopZOrderNode (int cli, const RECT *rc)
{
    int idx_znode = 0;
    int zt_type = ZOF_VISIBLE | ZOF_TF_MAINWIN;

#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
        zt_type |= ZOF_TYPE_GLOBAL;
    } else
#endif
        zt_type |= ZOF_TYPE_TOOLTIP;

    idx_znode = AllocZOrderNode (0, 0,
            (HWND)0,
            zt_type,
            rc, "", HDC_SCREEN_SYS);
    dskShowWindow (cli, idx_znode);
    return idx_znode;
}

static int dskDestroyTopZOrderNode (int cli, int idx_znode)
{
    return FreeZOrderNode (cli, idx_znode, NULL);
}

static int _screensaver_node;

void __mg_screensaver_show(void)
{
    dskShowWindow (0, _screensaver_node);
    dskMove2Top (0, _screensaver_node);
}

void __mg_screensaver_hide(void)
{
    dskHideWindow (0, _screensaver_node);
}

void __mg_screensaver_create(void)
{
    /* create screensaver node. */
    if (!_screensaver_node) {
        RECT rcScr = GetScreenRect();
        _screensaver_node  = dskCreateTopZOrderNode (0, &rcScr);
        dskSetZNodeAlwaysTop (0, _screensaver_node, TRUE);
        dskHideWindow (0, _screensaver_node);
    }
}

void __mg_screensaver_destroy(void)
{
    /* destroy screensaver node. */
    if (_screensaver_node) {
        dskDestroyTopZOrderNode(0, _screensaver_node);
        _screensaver_node = 0;
    }
}
#endif /* defined _MG_ENABLE_SCREENSAVER */

static int dskSetZNodeAlwaysTop (int cli, int idx_znode, BOOL fSet)
{
    ZORDERINFO* zi = get_zorder_info (cli);
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    /* lock zi for change */
    lock_zi_for_change (zi);
    if (fSet) {
        nodes[idx_znode].flags |= ZOF_IF_ALWAYSTOP;
    }
    else {
        nodes[idx_znode].flags &= ~ZOF_IF_ALWAYSTOP;
    }
    /* unlock zi for change */
    unlock_zi_for_change (zi);

    dskMove2Top (cli, idx_znode);
    return 0;
}

#ifdef _MGSCHEMA_COMPOSITING
/* Since 5.0.0 */
static int dskSetZNodeCompositing (int cli, int idx_znode, int ct, DWORD ct_arg)
{
    ZORDERINFO* zi = get_zorder_info (cli);
    DWORD type;
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    type = nodes[idx_znode].flags & ZOF_TYPE_MASK;
    if (type == ZOF_TYPE_NULL || type >= ZOF_TYPE_DESKTOP) {
        return -1;
    }

    if ((nodes[idx_znode].ct == ct) && (nodes[idx_znode].ct_arg == ct_arg)) {
        return 0;
    }

    /* lock zi for change */
    lock_zi_for_change (zi);

    nodes [idx_znode].ct = ct;
    nodes [idx_znode].ct_arg = ct_arg;

    DO_COMPSOR_OP_ARGS (on_changed_ct,
            get_layer_from_client (cli), idx_znode);

    /* unlock zi for change */
    unlock_zi_for_change (zi);
    return 0;
}
#endif /* defined _MGSCHEMA_COMPOSITING */

static int dskMove2Top (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = get_zorder_info (cli);
    int level, *first = NULL;
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    level = ZOF_TYPE_TO_LEVEL_IDX (type);
    if (MG_UNLIKELY (level < 0 || level > ZLIDX_LAUNCHER)) {
        assert (0);
        return -1;
    }

    first = zi->first_in_levels + level;
    assert (*first);
    if (*first == idx_znode)    // already at top
        return 0;

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    EmptyClipRgn (&sg_UpdateRgn);

    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int slot;
        RECT rc = nodes [idx_znode].rc;

        slot = zi->first_in_levels[level];
        for (; slot != idx_znode; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                            DoesIntersect (&rc, &nodes [slot].rc)) {
                nodes [slot].age++;
                AddClipRect(&sg_UpdateRgn, &nodes [slot].rc);
            }
        }
    }

#endif /* not defined _MGSCHEMA_COMPOSITING */

    /* unchain it and move to top */
    if (nodes [idx_znode].prev) {
        nodes [nodes [idx_znode].prev].next = nodes [idx_znode].next;
    }
    if (nodes [idx_znode].next) {
        nodes [nodes [idx_znode].next].prev = nodes [idx_znode].prev;
    }

    /* Since 5.0.0. Support for always top znode. */
    if (!(nodes [*first].flags & ZOF_IF_ALWAYSTOP)) {
        nodes [idx_znode].prev = nodes[*first].prev;
        nodes [idx_znode].next = *first;
        nodes [*first].prev = idx_znode;
        *first = idx_znode;
    }
    else {
        int pre_idx = *first;
        while (*first) {
            if (nodes [*first].flags & ZOF_IF_ALWAYSTOP) {
                pre_idx = *first;
                first = &nodes[*first].next;
            }
            else
                break;
        }

        nodes [idx_znode].prev = pre_idx;
        nodes [idx_znode].next = nodes [pre_idx].next;
        nodes [nodes[pre_idx].next].prev = idx_znode;
        nodes [pre_idx].next = idx_znode;
    }

#if 0   /* old code without ZOF_IF_ALWAYSTOP */
    nodes [idx_znode].prev = 0;
    nodes [idx_znode].next = *first;
    nodes [*first].prev = idx_znode;
    *first = idx_znode;
#endif  /* old code without ZOF_IF_ALWAYSTOP */

#ifdef _MGSCHEMA_COMPOSITING
    DO_COMPSOR_OP_ARGS (on_raised_win, get_layer_from_client (cli), idx_znode);
#else
    nodes [idx_znode].age++;
#endif

    /* unlock zi for change */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    if ((nodes [idx_znode].flags & ZOF_VISIBLE) && nodes [idx_znode].hwnd) {
        update_client_window_rgn (nodes [idx_znode].cli,
                        nodes [idx_znode].hwnd);
    }
#endif

    return 0;
}

static int dskShowWindow (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;
    int level, *first = NULL;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(__mg_zorder_info);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    level = ZOF_TYPE_TO_LEVEL_IDX (type);
    if (MG_UNLIKELY (level < 0 || level > ZLIDX_LAUNCHER)) {
        assert (0);
        return -1;
    }

    first = zi->first_in_levels + level;
    assert (*first);

    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        _WRN_PRINTF ("showing a visible window (%s)\n",
                nodes[idx_znode].caption);
        return 0;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    {
        int slot;
        RECT rc = nodes [idx_znode].rc;
        RECT rc_screen = GetScreenRect();

        if (type > ZOF_TYPE_LAUNCHER) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_LAUNCHER);
        }
        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_NORMAL);
        }
        if (type > ZOF_TYPE_HIGHER) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_HIGHER);
        }
        if (type > ZOF_TYPE_DOCKER) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_DOCKER);
        }
        if (type > ZOF_TYPE_SCREENLOCK) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_SCREENLOCK);
        }
        if (type > ZOF_TYPE_GLOBAL) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_GLOBAL);
        }
        if (type > ZOF_TYPE_TOOLTIP) {
            // never touch here.
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_TOOLTIP);
        }

        slot = nodes [idx_znode].next;
        for (; slot != 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                            DoesIntersect (&rc, &nodes [slot].rc)) {
                nodes [slot].age++;
            }
        }

        if (DoesIntersect (&rc, &rc_screen)) {
            nodes [0].age++;
        }

        nodes [idx_znode].age++;
        nodes [idx_znode].flags |= ZOF_VISIBLE;
    }
#else   /* defined _MGSCHEMA_COMPOSITING */
    nodes [idx_znode].flags |= ZOF_VISIBLE;
    DO_COMPSOR_OP_ARGS (on_showing_win, get_layer_from_client (cli), idx_znode);
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /* unlock zi for change ... */
    unlock_zi_for_change (zi);

    return 0;
}

static int dskHideWindow (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = get_zorder_info(cli);
    int level, *first = NULL;
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    level = ZOF_TYPE_TO_LEVEL_IDX (type);
    if (MG_UNLIKELY (level < 0 || level > ZLIDX_LAUNCHER)) {
        assert (0);
        return -1;
    }

    first = zi->first_in_levels + level;
    assert (*first);

    if (!(nodes[idx_znode].flags & ZOF_VISIBLE)) {
        return 0;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    /* lock zi for change */
    lock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* check influenced zorder nodes */
    SetClipRgn (&sg_UpdateRgn, &nodes [idx_znode].rc);
    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int slot;
        RECT rcScr = GetScreenRect ();

        slot = nodes [idx_znode].next;
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                nodes [slot].age++;
                nodes [slot].flags |= ZOF_IF_REFERENCE;
            }
        }

        if (type > ZOF_TYPE_GLOBAL) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_GLOBAL);
        }
        if (type > ZOF_TYPE_SCREENLOCK) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_SCREENLOCK);
        }
        if (type > ZOF_TYPE_DOCKER) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_DOCKER);
        }
        if (type > ZOF_TYPE_HIGHER) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_HIGHER);
        }
        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_NORMAL);
        }
        if (type > ZOF_TYPE_LAUNCHER) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_LAUNCHER);
        }

        if (SubtractClipRect (&sg_UpdateRgn, &rcScr)) {
            nodes [0].age++;
            nodes [0].flags |= ZOF_IF_REFERENCE;
        }
    }
#else   /* not defined _MGSCHEMA_COMPOSITING */
    DO_COMPSOR_OP_ARGS (on_hiding_win, get_layer_from_client (cli), idx_znode);
#endif  /* defined _MGSCHEMA_COMPOSITING */

    if (idx_znode && (nodes [idx_znode].flags & ZOF_TF_MAINWIN
         && (nodes [idx_znode].flags & ZOF_VISIBLE))) {
        post_msg_by_znode_p (zi, nodes + idx_znode, MSG_NCACTIVATE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode, MSG_ACTIVE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode, MSG_KILLFOCUS, 0, 0);
    }

    nodes [idx_znode].flags &= ~ZOF_VISIBLE;
    /*
     * do not reset the age to zero.
     * nodes [idx_znode].age = 0;
     */

    /* unlock zi for change */
    unlock_zi_for_change (zi);

#ifndef _MGSCHEMA_COMPOSITING
    /* update all znode if it's dirty */
    do_for_all_znodes (&nodes [idx_znode].rc, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_IF_REFERENCE) {
        SendMessage (HWND_DESKTOP,
                        MSG_ERASEDESKTOP, 0, (WPARAM)&nodes [idx_znode].rc);
        nodes [0].flags &= ~ZOF_IF_REFERENCE;
    }
#else   /* not defined _MGSCHEMA_COMPOSITING */
    DO_COMPSOR_OP_ARGS (on_dirty_screen,
            get_layer_from_client (cli),
            nodes [idx_znode].flags & ZOF_TYPE_FLAG_MASK,
            &nodes [idx_znode].rc);
#endif  /* defined _MGSCHEMA_COMPOSITING */

    return 0;
}

static int dskMoveWindow (int cli, int idx_znode, HDC memdc, const RECT* rcWin)
{
    DWORD type;
    int level, *first = NULL;
    ZORDERNODE* nodes;
#ifndef _MGSCHEMA_COMPOSITING
    unsigned short idx;
    MASKRECT *firstmaskrect;
    int i, slot, nInvCount;
    RECT rcInv[4], rcOld, rcInter, tmprc;
    CLIPRGN bblt_rgn;
    MASKRECT *maskrect;
#endif
    ZORDERINFO* zi = get_zorder_info(cli);

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    level = ZOF_TYPE_TO_LEVEL_IDX (type);
    if (MG_UNLIKELY (level < 0 || level > ZLIDX_LAUNCHER)) {
        assert (0);
        return -1;
    }

    first = zi->first_in_levels + level;
    assert (*first);

    if (memcmp (&nodes [idx_znode].rc, rcWin, sizeof (RECT)) == 0)
        return 0;

#ifndef _MGSCHEMA_COMPOSITING
    if ((RECTW(nodes[idx_znode].rc) != RECTWP(rcWin)  ||
            RECTH(nodes[idx_znode].rc) != RECTHP(rcWin)) &&
            (nodes[idx_znode].flags & ZOF_TW_TROUNDCNS ||
             nodes[idx_znode].flags & ZOF_TW_BROUNDCNS)) {
        RECT cli_rect;
        SetRect(&cli_rect, 0, 0, RECTWP(rcWin), RECTHP(rcWin));

        lock_zi_for_change(zi);

        if (nodes[idx_znode].idx_mask_rect != 0) {

            idx = nodes[idx_znode].idx_mask_rect;
            firstmaskrect = GET_MASKRECT(zi);

            while (idx) {
                __mg_slot_clear_use((unsigned char*)GET_MASKRECT_USAGEBMP(zi),
                        idx);
                idx = ((MASKRECT *)(firstmaskrect+idx))->next;
            }
        }

        alloc_mask_rects_for_round_corners (zi, &nodes[idx_znode],
                nodes[idx_znode].flags, &cli_rect);

        unlock_zi_for_change(zi);
    }
#endif /* not defined _MGSCHEMA_COMPOSITING */

#ifndef _MGSCHEMA_COMPOSITING
    /* no need force to close the menu when moving a window
       under compositing schema */
#ifdef _MGHAVE_MENU
    if (nodes [idx_znode].flags & ZOF_VISIBLE && zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nInvCount = SubtractRect (rcInv, &nodes [idx_znode].rc, rcWin);

    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int level_from, level_to;

        RECT rcScr = GetScreenRect ();

        /* lock zi for change */
        lock_zi_for_change (zi);

        if (type > ZOF_TYPE_LAUNCHER) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_LAUNCHER);
        }
        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_NORMAL);
        }
        if (type > ZOF_TYPE_HIGHER) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_HIGHER);
        }
        if (type > ZOF_TYPE_DOCKER) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_DOCKER);
        }
        if (type > ZOF_TYPE_SCREENLOCK) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_DOCKER);
        }
        if (type > ZOF_TYPE_GLOBAL) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_GLOBAL);
        }

        slot = nodes [idx_znode].next;
        for (; slot != 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                    DoesIntersect (rcWin, &nodes [slot].rc)) {
                nodes [slot].age++;
            }
        }

        if (DoesIntersect (rcWin, &rcScr)) {
            nodes [0].age++;
        }

        /* check influenced zorder nodes */
        //SetClipRgn (&sg_UpdateRgn, rcInv + i);
        SetClipRgn (&sg_UpdateRgn, &(nodes [idx_znode].rc));

        /* znodes below current znode in the same level */
        slot = nodes [idx_znode].next;
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                    !(nodes [slot].flags & ZOF_IF_REFERENCE) &&
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                nodes [slot].age++;
                nodes [slot].flags |= ZOF_IF_REFERENCE;
            }
        }

        /* znodes below current level */
        for (level = 0; level < NR_ZORDER_LEVELS; level++) {
            if (type > _zof_types_for_level[level]) {
                slot = zi->first_in_levels[level];
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE &&
                            !(nodes [slot].flags & ZOF_IF_REFERENCE) &&
                            subtract_rgn_by_node(&sg_UpdateRgn,
                                zi, &nodes[slot])) {
                        nodes [slot].age++;
                        nodes [slot].flags |= ZOF_IF_REFERENCE;
                    }
                }
            }
        }

        if (!(nodes [0].flags & ZOF_IF_REFERENCE) &&
                SubtractClipRect (&sg_UpdateRgn, &rcScr)) {
            nodes [0].age++;
            nodes [0].flags |= ZOF_IF_REFERENCE;
        }

        rcOld = nodes [idx_znode].rc;
        nodes [idx_znode].rc = *rcWin;
        nodes [idx_znode].age++;

#if defined (_MGRM_PROCESSES)
        if (cli == 0 || mgClients [cli].layer == SHAREDRES_TOPMOST_LAYER) {
#endif
            /* Copy window content to new postion */
            InitClipRgn (&bblt_rgn, &sg_FreeClipRectList);
            if (nodes[idx_znode].idx_mask_rect == 0){
                SelectClipRect (HDC_SCREEN_SYS, rcWin);
            }
            else {
                firstmaskrect = GET_MASKRECT(zi);
                idx = nodes [idx_znode].idx_mask_rect;

                while (idx) {
                    maskrect = firstmaskrect + idx;
                    SetRect (&tmprc, rcWin->left + maskrect->left,
                            rcWin->top + maskrect->top,
                            rcWin->left + maskrect->right,
                            rcWin->top + maskrect->bottom);
                    if ( DoesIntersect (&tmprc, &rcScr)) {
                        IntersectRect (&tmprc, &tmprc, &rcScr);
                        AddClipRect (&bblt_rgn, &tmprc);
                    }
                    idx = maskrect->next;
                }
                SelectClipRegion(HDC_SCREEN_SYS, &bblt_rgn);
            }

            /* Since 5.0.0: handle all levels */
            slot = 0;
            level_from = -1;
            level_to = -1;
            switch (type) {
            case ZOF_TYPE_TOOLTIP:
                slot = zi->first_tooltip;
                break;

            case ZOF_TYPE_GLOBAL:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_TOOLTIP;
                slot = zi->first_global;
                break;

            case ZOF_TYPE_SCREENLOCK:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_GLOBAL;
                slot = zi->first_screenlock;
                break;

            case ZOF_TYPE_DOCKER:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_SCREENLOCK;
                slot = zi->first_docker;
                break;

            case ZOF_TYPE_HIGHER:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_DOCKER;
                slot = zi->first_topmost;
                break;

            case ZOF_TYPE_NORMAL:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_TOPMOST;
                slot = zi->first_normal;
                break;

            case ZOF_TYPE_LAUNCHER:
                level_from = ZLIDX_TOOLTIP;
                level_to = ZLIDX_NORMAL;
                slot = zi->first_launcher;
                break;

            case ZOF_TYPE_DESKTOP:
                level_to = 0;
                break;
            default:
                break;
            }

            if (level_from >= 0) {
                for (level = level_from; level <= level_to; level++) {
                    do_for_all_znodes (NULL, zi, _cb_exclude_rc,
                            _zts_for_level[level]);
                }
            }
            else if (level_to == 0) {
                do_for_all_znodes (NULL, zi, _cb_exclude_rc, ZT_ALL);
            }

            while (slot) {
               if (slot == idx_znode)
                   break;

                /* houhh 20090730, if slot window is no regular.*/
                if (nodes [slot].flags & ZOF_VISIBLE) {
                    if (nodes[slot].idx_mask_rect == 0) {
                        ExcludeClipRect (HDC_SCREEN_SYS, &nodes [slot].rc);
                    }
                    else {
                        RECT rc;
                        GetWindowRect(nodes [slot].hwnd, &rc);
                        firstmaskrect = GET_MASKRECT(zi);
                        idx = nodes [slot].idx_mask_rect;
                        while (idx) {
                            maskrect = firstmaskrect + idx;
                            SetRect (&tmprc, rc.left + maskrect->left,
                                    rc.top + maskrect->top,
                                    rc.left + maskrect->right,
                                    rc.top + maskrect->bottom);
                            if ( DoesIntersect (&tmprc, &rcScr)) {
                                IntersectRect (&tmprc, &tmprc, &rcScr);
                                ExcludeClipRect (HDC_SCREEN_SYS, &tmprc);
                            }
                            idx = maskrect->next;
                        }
                    }
                }
                slot = nodes [slot].next;
            }

            BitBlt (HDC_SCREEN_SYS, rcOld.left, rcOld.top,
                            MIN (RECTWP (rcWin), RECTW (rcOld)),
                            MIN (RECTHP (rcWin), RECTH (rcOld)),
                            HDC_SCREEN_SYS, rcWin->left, rcWin->top, 0);
            SyncUpdateDC (HDC_SCREEN_SYS);

            /* Restore the clip region of HDC_SCREEN_SYS */
            SelectClipRect (HDC_SCREEN_SYS, &rcScr);
            EmptyClipRgn(&bblt_rgn);
#if defined (_MGRM_PROCESSES)
        }
#endif

        do_for_all_znodes (&rcOld, zi, _cb_update_znode, ZT_ALL);

        if (nodes [0].flags & ZOF_IF_REFERENCE) {
            SendMessage (HWND_DESKTOP,
                            MSG_ERASEDESKTOP, 0, (LPARAM)&rcOld);
            nodes [0].flags &= ~ZOF_IF_REFERENCE;
        }

        /* check the invalid rectangles of the being moved window */
        EmptyClipRgn (&sg_UpdateRgn);
        nInvCount = SubtractRect (rcInv, &rcOld, &rcScr);
        for (i = 0; i < nInvCount; i++) {
            AddClipRect (&sg_UpdateRgn, rcInv + i);
        }

        /* Since 5.0.0: handle levels above the current level */
        for (level = 0; level < NR_ZORDER_LEVELS; level++) {
            if (type < _zof_types_for_level[level]) {
                slot = zi->first_in_levels[level];
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE &&
                            IntersectRect (&rcInter, &rcOld, &nodes [slot].rc))
                        AddClipRect (&sg_UpdateRgn, &rcInter);
                }
            }
        }

        /* handle znodes above the current znode */
        slot = *first;
        for (; slot != idx_znode; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                        IntersectRect (&rcInter, &rcOld, &nodes [slot].rc))
                AddClipRect(&sg_UpdateRgn, &rcInter);
        }

        OffsetRegion (&sg_UpdateRgn,
                        rcWin->left - rcOld.left,
                        rcWin->top - rcOld.top);

        update_client_window_rgn (nodes [idx_znode].cli,
                nodes [idx_znode].hwnd);

        /* unlock zi for change ... */
        unlock_zi_for_change (zi);
    }
    else {
        lock_zi_for_change (zi);

        nodes [idx_znode].rc = *rcWin;
        nodes [idx_znode].age++;

        unlock_zi_for_change (zi);
    }
#else   /* defined _MGSCHEMA_COMPOSITING */
    {
        RECT org_rc;

        lock_zi_for_change (zi);

        org_rc = nodes [idx_znode].rc;
        nodes [idx_znode].rc = *rcWin;
        if (memdc != HDC_INVALID) {
            DeleteMemDC (nodes [idx_znode].mem_dc);
            nodes [idx_znode].mem_dc = memdc;
        }

        DO_COMPSOR_OP_ARGS (on_moved_win,
                get_layer_from_client (cli), idx_znode, &org_rc);

        unlock_zi_for_change (zi);
    }
#endif  /* defined _MGSCHEMA_COMPOSITING */

    return 0;
}

static int dskEnableZOrderNode (int cli, int idx_znode, int flags)
{
    ZORDERINFO* zi = get_zorder_info(cli);
    ZORDERNODE* nodes;

    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);

    /* lock zi for change */
    lock_zi_for_change (zi);

    if (flags)  /* enable window */
        nodes [idx_znode].flags &= ~ZOF_DISABLED;
    else
        nodes [idx_znode].flags |= ZOF_DISABLED;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    return 0;
}

/*======================== customize desktop =========================*/
static void
def_paint_desktop (void* context, HDC dc_desktop, const RECT* inv_rc)
{
    PBITMAP bg_bmp = NULL;
    int pic_x = 0, pic_y = 0;
    RECT rcScr = GetScreenRect ();

    if(context)
    {
        bg_bmp = ((DEF_CONTEXT *)context)->bg;
        pic_x  = ((DEF_CONTEXT *)context)->x;
        pic_y  = ((DEF_CONTEXT *)context)->y;
    }

    SetBrushColor (dc_desktop,
        GetWindowElementPixelEx (HWND_DESKTOP, HDC_SCREEN, WE_BGC_DESKTOP));

    if (inv_rc) {
        SelectClipRect (dc_desktop, inv_rc);
        FillBox (dc_desktop, inv_rc->left, inv_rc->top,
                RECTWP (inv_rc), RECTHP (inv_rc));
    }
    else {
        SelectClipRect (dc_desktop, &rcScr);
        FillBox(dc_desktop, rcScr.left, rcScr.top,
                rcScr.right, rcScr.bottom);
    }

    if (bg_bmp) {
        FillBoxWithBitmap (dc_desktop, pic_x, pic_y,
                bg_bmp->bmWidth, bg_bmp->bmHeight, bg_bmp);
    }

    SyncUpdateDC (dc_desktop);
}

static inline void def_keyboard_handler (void* context,
        int message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
    case MSG_DT_KEYLONGPRESS:
    case MSG_DT_KEYALWAYSPRESS:
    case MSG_DT_KEYDOWN:
    case MSG_DT_CHAR:
    case MSG_DT_KEYUP:
    case MSG_DT_SYSKEYDOWN:
    case MSG_DT_SYSCHAR:
    case MSG_DT_SYSKEYUP:
        break;
    }
}

#define IDC_DTI_ABOUT   (IDM_DTI_FIRST)

static void def_customize_desktop_menu (void* context,
        HMENU hmnu, int start_pos)
{
#if defined(_MGMISC_ABOUTDLG) && defined(_MGHAVE_MENU)
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = IDC_DTI_ABOUT;
    mii.typedata    = (DWORD)GetSysText(IDS_MGST_ABOUTMG);
    mii.hsubmenu    = 0;
    InsertMenuItem (hmnu, start_pos, TRUE, &mii);
#endif
}

static void def_mouse_handler(void* context, int message,
                                    WPARAM wParam, LPARAM lParam)
{
#ifdef _MGHAVE_MENU
    int x, y;
    switch (message) {
    case MSG_DT_LBUTTONDOWN:
    case MSG_DT_LBUTTONUP:
    case MSG_DT_LBUTTONDBLCLK:
    case MSG_DT_MOUSEMOVE:
    case MSG_DT_RBUTTONDOWN:
    case MSG_DT_RBUTTONDBLCLK:
        break;

    case MSG_DT_RBUTTONUP: {
        x = LOSWORD (lParam);
        y = HISWORD (lParam);

        TrackPopupMenu (sg_DesktopMenu, TPM_DEFAULT, x, y, HWND_DESKTOP);

        break;
    }

    default:
        break;
    }
#endif
}

static void def_desktop_menucmd_handler (void* context, int id)
{
#ifdef _MGMISC_ABOUTDLG
    if (id == IDC_DTI_ABOUT) {
#ifdef _MGRM_THREADS
        OpenAboutDialog ();
#else
        OpenAboutDialog (HWND_DESKTOP);
#endif
    }
#endif
}

static void def_deinit(void* context)
{
    if (context) {
        if (((DEF_CONTEXT *)context)->bg)
            UnloadBitmap (((DEF_CONTEXT *)context)->bg);

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)
        free(context);
#endif
    }

    return;
}

static int dskGetBgPicturePos (void)
{
    char szValue [21];

    if (GetMgEtcValue (__mg_def_renderer->name,
                SYSBMP_BGPICPOS, szValue, 20) < 0) {
        strcpy (szValue, "center");
    }

    if (!strcmp (szValue, "none"))
        return -1;
    if (!strcmp (szValue, "center"))
        return 0;
    if (!strcmp (szValue, "upleft"))
        return 1;
    if (!strcmp (szValue, "downleft"))
        return 2;
    if (!strcmp (szValue, "upright"))
        return 3;
    if (!strcmp (szValue, "downright"))
        return 4;
    if (!strcmp (szValue, "upcenter"))
        return 5;
    if (!strcmp (szValue, "downcenter"))
        return 6;
    if (!strcmp (szValue, "vcenterleft"))
        return 7;
    if (!strcmp (szValue, "vcenterright"))
        return 8;

    return -1;
}

static PBITMAP dskLoadBgPicture (HDC hdc)
{
    return (PBITMAP)GetSystemBitmapEx2 (hdc, __mg_def_renderer->name,
            SYSBMP_BGPICTURE);
}

static void dskGetBgPictureXY (int pos, int w, int h, int* x, int* y)
{
    switch (pos) {
        case 0: // center
            *x = (g_rcScr.right - w) >> 1;
            *y = (g_rcScr.bottom - h) >> 1;
            break;
        case 1: // upleft
            *x = 0;
            *y = 0;
            break;
        case 2: // downleft
            *x = 0;
            *y = g_rcScr.bottom - h;
            break;
        case 3: // upright
            *x = g_rcScr.right - w;
            *y = 0;
            break;
        case 4: // downright
            *x = g_rcScr.right - w;
            *y = g_rcScr.bottom - h;
            break;
        case 5: // upcenter
            *x = (g_rcScr.right - w) >> 1;
            *y = 0;
            break;
        case 6: // downcenter
            *x = (g_rcScr.right - w) >> 1;
            *y = g_rcScr.bottom - h;
            break;
        case 7: // vcenterleft
            *x = 0;
            *y = (g_rcScr.bottom - h) >> 1;
            break;
        case 8: // vcenterright
            *x = g_rcScr.right - w;
            *y = (g_rcScr.bottom - h) >> 1;
            break;
        default:
            *x = 0;
            *y = 0;
            break;
    }
}

static void* def_init (HDC dc_desktop)
{
    PBITMAP bg_bmp = NULL;
    int pic_x = 0, pic_y = 0;
    DEF_CONTEXT *con;
    int pos;

#if defined (_MGRM_THREADS) || defined (_MGRM_STANDALONE)
    con = malloc(sizeof(DEF_CONTEXT));
#else
    con = &g_def_context;
#endif

    pos = dskGetBgPicturePos ();
    if (pos < 0)
        bg_bmp = NULL;
    else
        bg_bmp = dskLoadBgPicture (dc_desktop);

    if (bg_bmp)
        dskGetBgPictureXY (pos,
                bg_bmp->bmWidth, bg_bmp->bmHeight, &pic_x, &pic_y);

    con->bg = bg_bmp;
    con->x = pic_x;
    con->y = pic_y;

    return (void *)con;
}

static DESKTOPOPS def_dsk_ops =
{
    def_init,
    def_deinit,
    def_paint_desktop,
    def_keyboard_handler,
    def_mouse_handler,
    def_customize_desktop_menu,
    def_desktop_menucmd_handler,
};

static DESKTOPOPS *dsk_ops = &def_dsk_ops;
static void *dt_context;

DESKTOPOPS* GUIAPI SetCustomDesktopOperationSet (DESKTOPOPS* usr_dsk_ops)
{
    DESKTOPOPS *tmp_ops = NULL;

    if (usr_dsk_ops == NULL) {
        return dsk_ops;
    }

    if (dsk_ops->deinit) {
        dsk_ops->deinit (dt_context);
    }

    tmp_ops = dsk_ops;
    dsk_ops = usr_dsk_ops;

    /* Since 5.0.0 */
    SendMessage (HWND_DESKTOP, MSG_REINITDESKOPS, 0, 0);
    return tmp_ops;
}

static BOOL _cb_bcast_msg (void* context,
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    PMAINWIN pWin;
    PMSG pMsg = (PMSG)context;

#if defined(_MGRM_PROCESSES)
    if (node->cli == __mg_client_id) {
#endif
        pWin = (PMAINWIN)node->hwnd;
        if (pWin && pWin->WinType != TYPE_CONTROL) {
            PostMessage ((HWND)pWin, pMsg->message, pMsg->wParam, pMsg->lParam);
            return TRUE;
        }
#if defined(_MGRM_PROCESSES)
    }
#endif

    return FALSE;
}

static int dskBroadcastMessage (PMSG pMsg)
{
    int count = 0;

    lock_zi_for_read (__mg_zorder_info);
    count = do_for_all_znodes (pMsg, __mg_zorder_info, _cb_bcast_msg, ZT_ALL);
    unlock_zi_for_read (__mg_zorder_info);

    return count;
}

static int dskSetIMEStatus (int iIMEStatusCode, int Value)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    switch (iIMEStatusCode) {
        case IME_STATUS_REGISTERED:
            return 0;

        case IME_STATUS_ENABLED:
            __mg_ime_status_info.bEnabled = Value;
            SendNotifyMessage (__mg_ime_wnd,
                    Value == 1? MSG_IME_OPEN : MSG_IME_CLOSE,
                    0, 0);
            return 0;

        case IME_STATUS_AUTOTRACK:
            __mg_ime_status_info.bAutoTrack = Value;
            return 0;

        case IME_STATUS_LANGUAGE:
        case IME_STATUS_ENCODING:
            return SendMessage (__mg_ime_wnd,
                    MSG_IME_SETSTATUS, iIMEStatusCode, Value);

        default:
            if (iIMEStatusCode >= IME_STATUS_USER_MIN
                    && iIMEStatusCode <= IME_STATUS_USER_MAX)
                return SendMessage (__mg_ime_wnd,
                        MSG_IME_SETSTATUS, iIMEStatusCode, Value);
    }

    return ERR_NO_MATCH;
}

static int dskGetIMEStatus (int iIMEStatusCode)
{
    if (__mg_ime_wnd == 0) {
        if (iIMEStatusCode == IME_STATUS_REGISTERED)
            return 0;
        else
            return ERR_IME_NOIMEWND;
    }

    switch (iIMEStatusCode) {
        case IME_STATUS_REGISTERED:
            return 1;

        case IME_STATUS_ENABLED:
            return __mg_ime_status_info.bEnabled;

        case IME_STATUS_AUTOTRACK:
            return __mg_ime_status_info.bAutoTrack;

        case IME_STATUS_LANGUAGE:
        case IME_STATUS_ENCODING:
        case IME_STATUS_VERSION:
            return SendMessage (__mg_ime_wnd,
                    MSG_IME_GETSTATUS, iIMEStatusCode, 0);

        default:
            if (iIMEStatusCode >= IME_STATUS_USER_MIN
                    && iIMEStatusCode <= IME_STATUS_USER_MAX)
            return SendMessage (__mg_ime_wnd,
                    MSG_IME_GETSTATUS, iIMEStatusCode, 0);
    }

    return ERR_NO_MATCH;
}

static int dskSetIMETargetInfo (const IME_TARGET_INFO *info)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    memcpy (&__mg_ime_status_info.TargetInfo, info, sizeof (IME_TARGET_INFO));

    if (__mg_ime_status_info.bAutoTrack) {
        int ret;
        ret = dskGetIMEStatus (IME_STATUS_VERSION);
        if (ret) {
            PostMessage (__mg_ime_wnd, MSG_IME_SET_TARGET_INFO,
                    0, (LPARAM)&__mg_ime_status_info.TargetInfo);
        } else {
            PostMessage (__mg_ime_wnd, MSG_IME_SETPOS,
                    0, (LPARAM)&__mg_ime_status_info.TargetInfo.ptCaret);
        }
    }

    return ERR_OK;
}

static int dskGetIMETargetInfo (IME_TARGET_INFO *info)
{
    if (__mg_ime_wnd == 0)
        return ERR_IME_NOIMEWND;

    *info = __mg_ime_status_info.TargetInfo;

    return ERR_OK;
}

int __kernel_get_window_region (HWND pWin, CLIPRGN* region)
{
    RECT rc;
    MASKRECT *maskrect;
    ZORDERNODE* nodes;
    ZORDERINFO* zi;
    int idx_znode, idx, nr_mask_rects;

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)
    zi = get_zorder_info (0);
#else
    zi = get_zorder_info (__mg_client_id);
#endif

    idx_znode = ((PMAINWIN)pWin)->idx_znode;
    if (IS_INVALID_ZIDX (zi, idx_znode)) {
        return -1;
    }

    EmptyClipRgn (region);

    /* lock zi for read */
    lock_zi_for_read (zi);

    nr_mask_rects = 0;
    nodes = GET_ZORDERNODE(zi);
    maskrect = GET_MASKRECT(zi);
    idx = nodes[idx_znode].idx_mask_rect;
    while (idx) {
        rc.left = maskrect->left;
        rc.top = maskrect->top;
        rc.right = maskrect->left;
        rc.bottom = maskrect->bottom;

        if (!AddClipRect (region, &rc)) {
            nr_mask_rects = -1;
            goto __mg_err_ret;
        }

        idx = maskrect->next;
        nr_mask_rects ++;
    }

    if (nr_mask_rects == 0) {
        rc = nodes [idx_znode].rc;
        /* VW: 2020-02-02: bad call to `OffsetRect (&rc, rc.left, rc.top);` */
        OffsetRect (&rc, -rc.left, -rc.top);
        if (!SetClipRgn (region, &rc))
            nr_mask_rects = -1;
    }

__mg_err_ret:
    /* unlock zi for read */
    unlock_zi_for_read (zi);

    return nr_mask_rects;
}

/*********************** Hook support ****************************************/
static HOOKINFO keyhook;
static HOOKINFO mousehook;
static HOOKINFO extrahook;

static MSGHOOK dskRegisterHookFunc (int event_type, HOOKINFO* info)
{
    MSGHOOK old_hook = NULL;
   
    switch (event_type) {
    case HOOK_EVENT_KEY:
        old_hook = keyhook.hook;
        keyhook.context = info->context;
        keyhook.hook = info->hook;
        break;

    case HOOK_EVENT_MOUSE:
        old_hook = mousehook.hook;
        mousehook.context = info->context;
        mousehook.hook = info->hook;
        break;

    case HOOK_EVENT_EXTRA:
        old_hook = extrahook.hook;
        extrahook.context = info->context;
        extrahook.hook = info->hook;
        break;

    default:
        break;
    }

    return old_hook;
}

#if 0   /* deprecated code since 5.0.0 */
static int dskHandleKeyHooks (HWND dst_wnd, UINT message,
                WPARAM wParam, LPARAM lParam)
{
    int ret = HOOK_GOON;

    if (keyhook.hook) {
        ret = keyhook.hook (keyhook.context, dst_wnd, message, wParam, lParam);
    }

    return ret;
}

static int dskHandleMouseHooks (HWND dst_wnd, UINT message,
                WPARAM wParam, LPARAM lParam)
{
    int ret = HOOK_GOON;

    if (mousehook.hook) {
        ret = mousehook.hook (mousehook.context, dst_wnd,
                message, wParam, lParam);
    }

    return ret;
}
#endif  /* deprecated code since 5.0.0 */

/* Since 5.0.0; return non zero to STOP handling the message */
static inline int
dskPreKeyMessageHandler (UINT message, WPARAM wparam, LPARAM lparam)
{
    if (__mg_check_hook_wins (HOOK_EVENT_KEY, message, wparam, lparam))
        return HOOK_STOP;

    return HOOK_GOON;
}

/* Since 5.0.0; return non zero to STOP handling the message */
static inline int
dskPreMouseMessageHandler (UINT message, WPARAM wparam, LPARAM lparam)
{
    if (__mg_check_hook_wins (HOOK_EVENT_MOUSE, message, wparam, lparam))
        return HOOK_STOP;

    return HOOK_GOON;
}

/* Since 5.0.0; return non zero to STOP handling the message */
static inline int
dskPreExtraMessageHandler (UINT message, WPARAM wparam, LPARAM lparam)
{
    if (__mg_check_hook_wins (HOOK_EVENT_EXTRA, message, wparam, lparam))
        return HOOK_STOP;

    return HOOK_GOON;
}

#include "list.h"

typedef struct _HOOKWININFO {
    struct list_head list;
    HWND hwnd;
    int flags;
    int cli;
} HOOKWININFO;

static struct list_head hook_win_list = { &hook_win_list, &hook_win_list };

static int dskRegisterHookWin (int cli, HWND hwnd, DWORD flags)
{
    struct list_head *info;
    HOOKWININFO *new_hook_info;

    list_for_each (info, &hook_win_list) {
        HOOKWININFO *hook_info = (HOOKWININFO*)info;
        if (hook_info->cli == cli && hook_info->hwnd == hwnd) {
            return -1;
        }
    }

    if ((new_hook_info = mg_slice_new (HOOKWININFO)) == NULL)
        return -1;

    new_hook_info->cli = cli;
    new_hook_info->hwnd = hwnd;
    new_hook_info->flags = (int)flags;
    list_add_tail (&new_hook_info->list, &hook_win_list);
    return 0;
}

static int dskUnregisterHookWin (int cli, HWND hwnd)
{
    struct list_head *info;
    HOOKWININFO *hook_info_got = NULL;

    list_for_each (info, &hook_win_list) {
        HOOKWININFO *hook_info = (HOOKWININFO*)info;
        if (hook_info->cli == cli && hook_info->hwnd == hwnd) {
            hook_info_got = hook_info;
            break;
        }
    }

    if (hook_info_got == NULL)
        return -1;

    list_del (&hook_info_got->list);
    mg_slice_delete (HOOKWININFO, hook_info_got);
    return 0;
}

/* this should be called before queuing the event messages to DESKTOP */
int __mg_check_hook_func (int event_type, const MSG* msg)
{
    HOOKINFO* info = NULL;
   
    switch (event_type) {
    case HOOK_EVENT_KEY:
        info = &keyhook;
        break;

    case HOOK_EVENT_MOUSE:
        info = &mousehook;
        break;

    case HOOK_EVENT_EXTRA:
        info = &extrahook;
        break;

    default:
        break;
    }

    if (info && info->hook) {
        return info->hook (info->context, msg->hwnd,
                msg->message, msg->wParam, msg->lParam);
    }

    return HOOK_GOON;
}

/* this should be called in the context of DESKTOP thread */
int __mg_check_hook_wins (int event_type,
        UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    struct list_head *info;
    int op = HOOK_GOON;

#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        return HOOK_GOON;
    }
#endif

    list_for_each (info, &hook_win_list) {
        HOOKWININFO *hook_info = (HOOKWININFO*)info;
        if ((hook_info->flags & HOOK_EVENT_MASK) == event_type) {
            op = hook_info->flags & HOOK_OP_MASK;
#ifdef _MGRM_PROCESSES
            if (hook_info->cli != 0) {
                MSG msg = { hook_info->hwnd, uMsg, wParam, lParam };
                __mg_send2client (&msg, mgClients + hook_info->cli);
            }
            else {
                PostMessage (hook_info->hwnd, uMsg, wParam, lParam);
            }
#else
            PostMessage (hook_info->hwnd, uMsg, wParam, lParam);
#endif
            if (op == HOOK_STOP)
                break;
        }
    }

    return op;
}

/* Since 5.0.0; free all if cli < 0 */
int __mg_free_hook_wins (int cli)
{
    int nr = 0;
    struct list_head *info, *tmp;

    list_for_each_safe (info, tmp, &hook_win_list) {
        HOOKWININFO *hook_info = (HOOKWININFO*)info;
        if (cli < 0 || hook_info->cli == cli) {
            list_del (&hook_info->list);
            mg_slice_delete (HOOKWININFO, hook_info);
            nr++;
        }
    }

    return nr;
}

#ifdef _MGHAVE_VIRTUAL_WINDOW
static struct list_head msg_queue_list = { &msg_queue_list, &msg_queue_list };

static inline int dskRegisterMsgQueue (MSGQUEUE* msg_queue)
{
    list_add_tail (&msg_queue->list, &msg_queue_list);
    return 0;
}

static int dskUnregisterMsgQueue (MSGQUEUE* msg_queue)
{
    struct list_head *l;
    MSGQUEUE *msg_queue_got = NULL;

    list_for_each (l, &msg_queue_list) {
        if (msg_queue == (MSGQUEUE*)l) {
            msg_queue_got = (MSGQUEUE*)l;
            break;
        }
    }

    if (msg_queue_got == NULL)
        return -1;

    list_del (&msg_queue->list);
    return 0;
}

#include "debug.h"

int __mg_join_all_message_threads (void)
{
    int nr = 0;
    struct list_head *l, *tmp;
    void* res;
    pthread_t th = pthread_self();

    list_for_each (l, &msg_queue_list) {
        MSGQUEUE *msg_queue = (MSGQUEUE*)l;
        dump_message_queue (msg_queue, __func__);
        pthread_cancel (msg_queue->th);
    }

    list_for_each_safe (l, tmp, &msg_queue_list) {
        MSGQUEUE *msg_queue = (MSGQUEUE*)l;
        if (th == msg_queue->th) {
            continue;
        }
        pthread_join (msg_queue->th, &res);
        if (res == PTHREAD_CANCELED) {
            list_del (&msg_queue->list);
            mg_DestroyMsgQueue (msg_queue);
            free (msg_queue);
            nr++;
        }
    }

    return nr;
}

static inline int post_quit_to_all_message_threads (BOOL no_self)
{
    int nr = 0;
    struct list_head *l;
    pthread_t th = pthread_self();

    list_for_each (l, &msg_queue_list) {
        MSGQUEUE *msg_queue = (MSGQUEUE*)l;
        dump_message_queue (msg_queue, __func__);
        if (no_self && th == msg_queue->th) {
            continue;
        }

        msg_queue->dwState |= QS_QUIT;
        POST_MSGQ (msg_queue);
        nr++;
    }

    return nr;
}

#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

/* Since 5.0.0 */
static int dskCalculateDefaultPosition (int cli, CALCPOSINFO* info)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    DWORD zt_type = ZOF_TYPE_NULL;
    int level, first;

    zi = get_zorder_info (cli);
    nodes = GET_ZORDERNODE(zi);

#ifdef _MGRM_PROCESSES
    if (cli == 0)
        zt_type = ZOF_TYPE_GLOBAL;
    else
#endif
    switch (info->ex_style & WS_EX_WINTYPE_MASK) {
    case WS_EX_WINTYPE_TOOLTIP:
        zt_type = ZOF_TYPE_TOOLTIP;
        break;

#ifdef _MGRM_PROCESSES
    case WS_EX_WINTYPE_GLOBAL:
        zt_type = ZOF_TYPE_GLOBAL;
        break;
#endif

    case WS_EX_WINTYPE_SCREENLOCK:
        zt_type = ZOF_TYPE_SCREENLOCK;
        break;

    case WS_EX_WINTYPE_DOCKER:
        zt_type = ZOF_TYPE_DOCKER;
        break;

    case WS_EX_WINTYPE_LAUNCHER:
        zt_type = ZOF_TYPE_LAUNCHER;
        break;

    case WS_EX_WINTYPE_HIGHER:
        zt_type = ZOF_TYPE_HIGHER;
        break;

    default:
        zt_type = ZOF_TYPE_NORMAL;
        break;
    }

#ifdef _MGRM_PROCESSES
    /* check the special types */
    switch (zt_type) {
    case ZOF_TYPE_TOOLTIP:
        if (zi->first_tooltip > 0 &&
                nodes [zi->first_tooltip].cli != cli) {
            zt_type = ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_GLOBAL:
        if (cli != 0) {
            zt_type = ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_SCREENLOCK:
        if (zi->first_screenlock > 0 &&
                nodes [zi->first_screenlock].cli != cli) {
            zt_type = ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_DOCKER:
        if (zi->first_docker > 0 &&
                nodes [zi->first_docker].cli != cli) {
            zt_type = ZOF_TYPE_HIGHER;
        }
        break;

    case ZOF_TYPE_LAUNCHER:
        if (zi->first_launcher > 0 &&
                nodes [zi->first_launcher].cli != cli) {
            zt_type = ZOF_TYPE_NORMAL;
        }
        break;
    }
#endif  /* defined _MGRM_PROCESSES */

    level = ZOF_TYPE_TO_LEVEL_IDX (zt_type);
    first = zi->first_in_levels[level];

#ifdef _MGSCHEMA_COMPOSITING   /* not defined _MGSCHEMA_COMPOSITING */
    DO_COMPSOR_OP_ARGS (calc_mainwin_pos,
            __mg_get_layer_from_zi(zi), zt_type, first, info);
#else   /* defined _MGSCHEMA_COMPOSITING */
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
        if (first == 0) {
            info->rc.left = 0;
            info->rc.top = 0;
        }
        else {
            info->rc = nodes[first].rc;
            OffsetRect (&info->rc,
                    DEF_OVERLAPPED_OFFSET_X, DEF_OVERLAPPED_OFFSET_Y);

            /* adjust to a reasonable postion */
            if (info->rc.top > (g_rcScr.bottom * 3 / 4)) {
                OffsetRect (&info->rc, 0, -info->rc.top);
            }

            if (info->rc.left > (g_rcScr.right * 3 / 4)) {
                OffsetRect (&info->rc, -info->rc.left, 0);
            }
        }
    }
#endif  /* defined _MGSCHEMA_COMPOSITING */

    return 0;
}

static int dskDumpZOrder (ZORDERINFO* zi)
{
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);
    int from = 0;
    int nr = 0;

    lock_zi_for_read (zi);

    from = __kernel_get_next_znode (zi, from);
    while (from > 0) {

#ifdef _MGRM_PROCESSES
        const char* caption = "N/A";
        if (mgIsServer) {
            caption = nodes[from].caption;
        }
        else if (nodes[from].cli == __mg_client_id) {
            caption = GetWindowCaption (nodes[from].hwnd);
        }

        _WRN_PRINTF ("ZNODE #%d (zidx: %d): hwnd (%p), "
                "caption (%s), rect (%d, %d, %d, %d), "
                "visibility (%s)\n",
                nr, from, nodes[from].hwnd,
                caption,
                nodes[from].rc.left, nodes[from].rc.top,
                nodes[from].rc.right, nodes[from].rc.bottom,
                (nodes[from].flags & ZOF_VISIBLE) ? "YES" : "NO");
#else
        _WRN_PRINTF ("ZNODE #%d (zidx: %d): hwnd (%p), "
                "caption (%s), rect (%d, %d, %d, %d), "
                "visibility (%s)\n",
                nr, from, nodes[from].hwnd,
                nodes[from].caption,
                nodes[from].rc.left, nodes[from].rc.top,
                nodes[from].rc.right, nodes[from].rc.bottom,
                (nodes[from].flags & ZOF_VISIBLE) ? "YES" : "NO");
#endif

        nr++;
        from = __kernel_get_next_znode (zi, from);
    }

    unlock_zi_for_read (zi);
    return nr;
}

static MSG sg_msgAutoRepeat;
static HWND sg_hCaretWnd;
static UINT sg_uCaretBTime;

