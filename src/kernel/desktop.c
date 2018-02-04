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
** desktop.c: The Desktop module.
**
** Current maintainer: 
**
*/

#include "license.h"
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/00_minigui.dat.c"
#endif

RECT g_rcScr;

ZORDERINFO* __mg_zorder_info;

/* pointer to desktop window */
PMAINWIN __mg_dsk_win;

/* handle to desktop window */
HWND __mg_hwnd_desktop;

/* the capture window */
HWND __mg_capture_wnd;

/* handle to the ime window - server only. */
HWND __mg_ime_wnd;

IME_STATUS_INFO __mg_ime_status_info;

static HWND sg_hCaretWnd;
static UINT sg_uCaretBTime;
static GCRINFO sg_ScrGCRInfo;
static CLIPRGN sg_UpdateRgn;

/*default window procedure*/
WNDPROC __mg_def_proc[3];

static BLOCKHEAP sg_FreeInvRectList;
static BLOCKHEAP sg_FreeClipRectList;

#define ZT_GLOBAL       0x00000001
#define ZT_TOPMOST      0x00000002
#define ZT_NORMAL       0x00000004
#define ZT_ALL          0x0000000F

typedef BOOL (* CB_ONE_ZNODE) (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* node);

typedef struct _DEF_CONTEXT
{
    PBITMAP bg;
    int x, y;
} DEF_CONTEXT;

PGCRINFO kernel_GetGCRgnInfo (HWND hWnd)
{
    return ((PMAINWIN)hWnd)->pGCRInfo;
}

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
static DEF_CONTEXT g_def_context;
#endif

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

    return TRUE;
}

static int pt_in_maskrect (const ZORDERINFO* zi,
       const ZORDERNODE* nodes, int x, int y)
{
    int cx, cy;
    RECT tmprc;
    MASKRECT *maskrect, *firstmaskrect;
    int idx = nodes->idx_mask_rect;

    if (idx != 0 && PtInRect (&(nodes->rc), x, y)) 
    { 
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
    else
    {
        return PtInRect(&(nodes->rc), x, y);
    }
}

static int get_znode_at_point (const ZORDERINFO* zi, 
                const ZORDERNODE* nodes, int x, int y)
{
    int slot = 0;

    slot = zi->first_global;
    for (; slot > 0; slot = nodes [slot].next) {
        if (nodes [slot].flags & ZOF_VISIBLE && 
                        pt_in_maskrect(zi, &nodes[slot], x, y))
            goto ret;
    }

    slot = zi->first_topmost;
    for (; slot > 0; slot = nodes [slot].next) {
        if (nodes [slot].flags & ZOF_VISIBLE &&
                        pt_in_maskrect(zi, &nodes[slot], x, y))
            goto ret;
    }

    slot = zi->first_normal;
    for (; slot > 0; slot = nodes [slot].next) {
        if (nodes [slot].flags & ZOF_VISIBLE &&
                        pt_in_maskrect(zi, &nodes[slot], x, y)) {
            goto ret;
        }
    }

ret:
    return slot;
}

static ZORDERINFO* _get_zorder_info (int cli)
{
#if defined (_MGRM_THREADS) || defined (_MGRM_STANDALONE)
    return __mg_zorder_info;
#else
    return get_zi_from_client (cli);
#endif
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

static void clean_znode_maskrect (ZORDERINFO* zi, ZORDERNODE* nodes, int idx_znode)
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
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    if (types & ZT_GLOBAL) {
        slot = zi->first_global;
        for (; slot > 0; slot = nodes [slot].next) {
            if (cb (context, zi, nodes + slot))
                count ++;
        }
    }

    if (types & ZT_TOPMOST) {
        slot = zi->first_topmost;
        for (; slot > 0; slot = nodes [slot].next) {
            if (cb (context, zi, nodes + slot))
                count ++;
        }
    }

    if (types & ZT_NORMAL) {
        slot = zi->first_normal;
        for (; slot > 0; slot = nodes [slot].next) {
            if (cb (context, zi, nodes + slot))
                count ++;
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

    if (pWin 
            && ((pWin->WinType == TYPE_CONTROL && (pWin->dwExStyle & WS_EX_CTRLASMAINWIN))
                || pWin->WinType != TYPE_CONTROL)
            && pWin->dwStyle & WS_VISIBLE) {
        RECT invrc;
        SetRect(&invrc, left, top, right, bottom);  

        if (IsRectEmpty (&invrc)) {
            SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
            InvalidateRect ((HWND)pWin, NULL, TRUE);
        }
        else {
            RECT rcTemp, rcInv, rcWin;

            if (pWin->WinType == TYPE_CONTROL && (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)){
                dskGetWindowRectInScreen (pWin, &rcWin);
            }
            else
                GetWindowRect(hwnd, &rcWin);

            if (IntersectRect (&rcTemp, 
                        &rcWin, &invrc)) {
                dskScreenToWindow (pWin, &rcTemp, &rcInv);
                SendAsyncMessage ((HWND)pWin, 
                                MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                dskScreenToClient (pWin, &rcTemp, &rcInv);
                InvalidateRect ((HWND)pWin, &rcInv, TRUE);
            }
        }
    }
}

static int update_client_window (ZORDERNODE* znode, const RECT* rc)
{
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (!mgIsServer)
        return -1;

    if (znode->cli != 0) {
        if (rc) {

            if (IsRectEmpty (&znode->dirty_rc)){
                SetRect(&znode->dirty_rc, 
                    rc->left, rc->top, rc->right, rc->bottom);  
            }
            else{
                GetBoundRect (&znode->dirty_rc, &znode->dirty_rc, rc);    
            }
        }
        mgClients [znode->cli].has_dirty = TRUE;
    }
    else 
#endif
    {
        if (rc)
            __mg_update_window (znode->fortestinghwnd, rc->left, rc->top, 
                            rc->right, rc->bottom);
        else
            __mg_update_window (znode->fortestinghwnd, 0, 0, 0, 0);
    }

    return 0;
}


static BOOL _cb_update_znode (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* znode)
{
    const RECT* rc = (RECT*)context;

    if (znode->flags & ZOF_VISIBLE && znode->flags & ZOF_REFERENCE) {
        update_client_window (znode, rc);
        znode->flags &= ~ZOF_REFERENCE;
        return TRUE;
    }

    return FALSE;
}

static BOOL _cb_intersect_rc (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    RECT* rc = (RECT*)context;

    if (node->flags & ZOF_VISIBLE && DoesIntersect (rc, &node->rc)) {
        node->age ++;
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
        node->age ++;
        node->flags |= ZOF_REFERENCE;
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

        while(idx != 0) {
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

static void reset_window (PMAINWIN pWin, RECT* rcWin)
{
    PGCRINFO pGCRInfo;
    RECT rcTemp;

    pGCRInfo = pWin->pGCRInfo;
    IntersectRect (&rcTemp, rcWin, &g_rcScr);
    SetClipRgn (&pGCRInfo->crgn, &rcTemp);
}

#ifdef _MGRM_THREADS
#ifndef __NOUNIX__
/*for unix system, using read/write lock*/
void lock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_rwlock_wrlock(&((ZORDERINFO*)zi)->rwlock);
}

void unlock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_rwlock_unlock(&((ZORDERINFO*)zi)->rwlock);
}

void lock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_rwlock_rdlock(&((ZORDERINFO*)zi)->rwlock);
}

void unlock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_rwlock_unlock(&((ZORDERINFO*)zi)->rwlock);
}
#else
/*for non-unix system, using mutex*/
void lock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_mutex_lock(&((ZORDERINFO*)zi)->rwlock);
}

void unlock_zi_for_change (const ZORDERINFO* zi)
{
    pthread_mutex_unlock(&((ZORDERINFO*)zi)->rwlock);
}

void lock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_mutex_lock(&((ZORDERINFO*)zi)->rwlock);
}

void unlock_zi_for_read (const ZORDERINFO* zi)
{
    pthread_mutex_unlock(&((ZORDERINFO*)zi)->rwlock);
}
#endif
#elif defined(_MGRM_STANDALONE)  /*for _MGRM_STANDALONE*/
void lock_zi_for_change (const ZORDERINFO* zi) { }
void unlock_zi_for_change (const ZORDERINFO* zi) { }
void lock_zi_for_read (const ZORDERINFO* zi) { }
void unlock_zi_for_read (const ZORDERINFO* zi) { }
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
    PCLIPRECT pcrc;
    PINVRGN pInvRgn;
    PCLIPRGN prgn;

    dskClientToScreen (pWin, pswi->rc1, &rcScreen);

    //BUGFIX: if the MainWindow is AutoSecondaryDC, the secondaryDC and
    //client dc would be diffirent, so we must get the scondaryDC,
    //the update to client dc (dongjunjie 2010/7/28)
    //hdc = GetClientDC ((HWND)pWin);
    hdc = get_valid_dc(pWin, TRUE);

    pcrc = kernel_GetGCRgnInfo ((HWND)pWin)->crgn.head;
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
                    hdc, pswi->iOffx + rcMove.left, pswi->iOffy + rcMove.top, 0);
        }

        pcrc = pcrc->next;
    }
    //ReleaseDC (hdc);
    //BUGFIXED: we must update the secondaryDC to clientDC, to ensure 
    //the secondaryDC and clientDC are same (dongjunjie 2010/07/08)
    if(pWin->pMainWin->secondaryDC){
        HDC real_dc = GetClientDC((HWND)pWin->pMainWin);
        update_secondary_dc(pWin, hdc, real_dc, pswi->rc1, HT_CLIENT);
        ReleaseDC (real_dc);
    }
    release_valid_dc(pWin, hdc);

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

    prgn = CreateClipRgn();
    lock_zi_for_read (__mg_zorder_info);
    CopyRegion(prgn, &kernel_GetGCRgnInfo((HWND)pWin)->crgn);
    pcrc = prgn->head;
    unlock_zi_for_read (__mg_zorder_info);

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
        if(bNeedInvalidate)
        {
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

        if (bNeedInvalidate)
        {
			InvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }

        pcrc = pcrc->next;
    }
    DestroyClipRgn(prgn);

    if (inved) {
        PostMessage ((HWND)pWin, MSG_PAINT, 0, 0);
    }

    return 0;
}

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
    int i, slot = 0, idx_znode;
    unsigned short idx;
    CLIPRGN unvisible_rgn;

    gcrinfo = kernel_GetGCRgnInfo (pdc->hwnd);
    mainwin = (PMAINWIN)(pdc->hwnd);
    
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
        InitClipRgn (&unvisible_rgn, &sg_FreeClipRectList);
        SetClipRgn (&unvisible_rgn, &nodes [idx_znode].rc);

        subtract_rgn_by_node(&unvisible_rgn, zi, &nodes[idx_znode]);
        SubtractRegion (&gcrinfo->crgn, &gcrinfo->crgn, &unvisible_rgn);
        EmptyClipRgn(&unvisible_rgn);
    }

    /* clip by popup menus */
    for (i = 0; i < zi->nr_popupmenus; i++) {
        SubtractClipRect (&gcrinfo->crgn, &menu_nodes [i].rc);
    }

    switch (nodes[idx_znode].flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_NORMAL:
            do_for_all_znodes (gcrinfo, zi, 
                            _cb_recalc_gcrinfo, ZT_GLOBAL);
            do_for_all_znodes (gcrinfo, zi, 
                            _cb_recalc_gcrinfo, ZT_TOPMOST);
            slot = zi->first_normal;
            break;
        case ZOF_TYPE_TOPMOST:
            do_for_all_znodes (gcrinfo, zi, 
                            _cb_recalc_gcrinfo, ZT_GLOBAL);
            slot = zi->first_topmost;
            break;
        case ZOF_TYPE_GLOBAL:
            slot = zi->first_global;
            break;
        case ZOF_TYPE_DESKTOP:
            do_for_all_znodes (gcrinfo, zi, 
                            _cb_recalc_gcrinfo, ZT_ALL);
            break;
        default:
            break;
    }

    while (slot) {
        if (slot == idx_znode) {
            break;
        }

        if (nodes [slot].flags & ZOF_VISIBLE)
           subtract_rgn_by_node(&gcrinfo->crgn, zi, &nodes[slot]); 

        slot = nodes [slot].next;
    }
    gcrinfo->old_zi_age = nodes [idx_znode].age;
    gcrinfo->age ++;
}

/*
 * Init a window's global clipping region.
 */
static void dskInitGCRInfo (PMAINWIN pWin)
{
    RECT rcWin, rcTemp;

    dskGetWindowRectInScreen (pWin, &rcWin);

#ifdef _MGRM_THREADS
    pthread_mutex_init (&pWin->pGCRInfo->lock, NULL);
#endif
    pWin->pGCRInfo->age = 0;
    pWin->pGCRInfo->old_zi_age = 0;

    InitClipRgn (&pWin->pGCRInfo->crgn, &sg_FreeClipRectList);
    IntersectRect (&rcTemp, &rcWin, &g_rcScr);
    SetClipRgn (&pWin->pGCRInfo->crgn, &rcTemp);
}

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

static int get_next_visible_mainwin (const ZORDERINFO* zi, int from)
{
    int next;
    ZORDERNODE* nodes = GET_ZORDERNODE(zi);

    if (from) {
        next = nodes [from].next;

        while (next) {
            if (nodes [next].flags & ZOF_TF_MAINWIN
                    && (nodes [next].flags & ZOF_VISIBLE) 
                    && !(nodes [next].flags & ZOF_DISABLED))
                return next;

            next = nodes [next].next;
        }
    }

    next = zi->first_global;
    while (next) {
        if (nodes [next].flags & ZOF_TF_MAINWIN
                && (nodes [next].flags & ZOF_VISIBLE) 
                && !(nodes [next].flags & ZOF_DISABLED))
            return next;

        next = nodes [next].next;
    }

    next = zi->first_topmost;
    while (next) {
        if (nodes [next].flags & ZOF_TF_MAINWIN
                && (nodes [next].flags & ZOF_VISIBLE) 
                && !(nodes [next].flags & ZOF_DISABLED))
            return next;

        next = nodes [next].next;
    }

    next = zi->first_normal;
    while (next) {
        if (nodes [next].flags & ZOF_TF_MAINWIN
                && (nodes [next].flags & ZOF_VISIBLE) 
                && !(nodes [next].flags & ZOF_DISABLED))
            return next;

        next = nodes [next].next;
    }

    return 0;
}

/*
 * Add new hosted main window.
 */
static void dskAddNewHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;
    
    pHosted->pNextHosted = NULL;

    head = pHosting->pFirstHosted;
    if (head)
    {
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
void dskRemoveHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;
    
    head = pHosting->pFirstHosted;
    if (head == pHosted)
    {
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

/*
 * Moves a window to topmost.
 *
 * dskIsTopMost
 * dskMoveToTopMost
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
    BOOL ret = FALSE;

    lock_zi_for_read (__mg_zorder_info);

    if (__mg_zorder_info->first_global == pWin->idx_znode)
        ret = TRUE;
    else if (__mg_zorder_info->first_topmost == pWin->idx_znode)
        ret = TRUE;
    else if (__mg_zorder_info->first_normal == pWin->idx_znode)
        ret = TRUE;

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
    ZORDERINFO* zi = _get_zorder_info(cli);
    int i, ret = 0;
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    int cli_trackmenu;
#endif
    ZORDERNODE* menu_nodes;
    ZORDERNODE* win_nodes;
    RECT rc_bound;

    if (zi->cli_trackmenu < 0 || zi->nr_popupmenus == 0)
        return 0;

    menu_nodes = GET_MENUNODE(zi);
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;

    SetRect (&rc_bound, 0, 0, 0, 0);

    /* lock zi for change */
    lock_zi_for_change (zi);

    for (i = 0; i < zi->nr_popupmenus; i++) {
        GetBoundRect (&rc_bound, &rc_bound, &menu_nodes [i].rc);
    }

    SetClipRgn (&sg_UpdateRgn, &rc_bound);

    /* check influenced window zorder nodes */
    do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_ALL);

    if (SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
        win_nodes [0].age ++;
        win_nodes [0].flags |= ZOF_REFERENCE;
    }

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    cli_trackmenu = zi->cli_trackmenu;
#endif
    zi->cli_trackmenu = -1;
    zi->nr_popupmenus = 0;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    /* update all znode if it's dirty */
    do_for_all_znodes (&rc_bound, zi, _cb_update_znode, ZT_ALL);

    if (win_nodes [0].flags & ZOF_REFERENCE) {
        SendMessage (HWND_DESKTOP, 
                        MSG_ERASEDESKTOP, 0, (LPARAM)&rc_bound);
        win_nodes [0].flags &= ~ZOF_REFERENCE;
    }

    /* notify the client to close the menu */
    {
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
        MSG msg = {0, MSG_CLOSEMENU, 0, 0, __mg_timer_counter};

        if (cli_trackmenu)
            ret = __mg_send2client (&msg, mgClients + cli_trackmenu);
        else
#endif
            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);
    }

    return ret;
}

static int srvStartTrackPopupMenu (int cli, const RECT* rc, HWND ptmi)
{
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* menu_nodes;
    ZORDERNODE* win_nodes;

    if (zi->cli_trackmenu >= 0 && zi->cli_trackmenu != cli) {
        srvForceCloseMenu (0);
    }

    if (zi->nr_popupmenus == zi->max_nr_popupmenus)
        return -1;

    menu_nodes = GET_MENUNODE(zi);
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;
    
    /* lock zi for change */
    lock_zi_for_change (zi);

    /* check influenced window zorder nodes */
    do_for_all_znodes ((void*)rc, zi, _cb_intersect_rc, ZT_ALL);

    if (DoesIntersect (rc, &g_rcScr)) {
        win_nodes [0].age ++;
    }

    menu_nodes [zi->nr_popupmenus].rc = *rc;
    menu_nodes [zi->nr_popupmenus].fortestinghwnd = ptmi;

    if (zi->cli_trackmenu == -1)
        zi->cli_trackmenu = cli;
    zi->nr_popupmenus ++;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    return zi->nr_popupmenus - 1;
}

static int srvEndTrackPopupMenu (int cli, int idx_znode)
{
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* menu_nodes;
    ZORDERNODE* win_nodes;
    RECT rc;

    if (zi->cli_trackmenu != cli
                    || zi->nr_popupmenus != (idx_znode + 1))
        return -1;

    menu_nodes = GET_MENUNODE(zi);
    win_nodes = menu_nodes + DEF_NR_POPUPMENUS;
    
    /* lock zi for change */
    lock_zi_for_change (zi);

    rc = menu_nodes [idx_znode].rc;
    SetClipRgn (&sg_UpdateRgn, &rc);

    /* check influenced window zorder nodes */
    do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_ALL);

    if (SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
        win_nodes [0].age ++;
        win_nodes [0].flags |= ZOF_REFERENCE;
    }

    zi->nr_popupmenus --;
    if (zi->nr_popupmenus == 0)
        zi->cli_trackmenu = -1;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    /* update all znode if it's dirty */
    do_for_all_znodes (&rc, zi, _cb_update_znode, ZT_ALL);

    if (win_nodes [0].flags & ZOF_REFERENCE) {
        SendMessage (HWND_DESKTOP, 
                        MSG_ERASEDESKTOP, 0, (LPARAM)&rc);
        win_nodes [0].flags &= ~ZOF_REFERENCE;
    }

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

#endif

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
#if defined (_MGRM_PROCESSES) && !defined (_MGRM_STANDALONE)
    else {
        MSG msg = {nodes [znode].main_win, 
                message, wParam, lParam, __mg_timer_counter};

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
#if defined (_MGRM_PROCESSES) && !defined (_MGRM_STANDALONE)
    else {
        MSG msg = {znode->main_win, 
                message, wParam, lParam, __mg_timer_counter};

        ret = __mg_send2client (&msg, mgClients + znode->cli);
    }
#endif

    return ret;
}

static HWND dskSetActiveZOrderNode (int cli, int idx_znode)
{
    int old_active = 0;
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;
    HWND old_hwnd = HWND_NULL, new_hwnd = HWND_NULL;

    if (idx_znode > zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) {
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
            old_hwnd = nodes [old_active].fortestinghwnd;
        }
        else
            old_hwnd = HWND_OTHERPROC;
    }

    if (idx_znode) {
        if (nodes [idx_znode].cli == cli) {
            new_hwnd = nodes [idx_znode].fortestinghwnd;
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

static void GetTextCharPos (PLOGFONT log_font, const char *text, 
               int len, int fit_bytes, int *fit_chars, int *pos_chars)
{
    DEVFONT* sbc_devfont = log_font->sbc_devfont;
    DEVFONT* mbc_devfont = log_font->mbc_devfont;
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

/*get the count of idle mask rect*/
int __mg_get_idle_slot (unsigned char* bitmap, int len_bmp)
{
    int idle = 0;
    int i, j;

    for (i = 0; i < len_bmp; i++) {
        for (j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j)) 
                idle++;
        }
        bitmap++;
    }
    return idle;
}


#define do_with_round_rect(src, drc, idx) \
            if ( idx != 0 ) { \
                if ( drc[idx-1].left == src.left && \
                        RECTW(drc[idx-1]) == RECTW(src) ) { \
                    SetRect(&(drc[idx-1]),  \
                            drc[idx-1].left, \
                            drc[idx-1].top, \
                            drc[idx-1].right, \
                            src.bottom); \
                } \
                else { \
                    CopyRect(&(drc[idx]), &src); \
                    idx++; \
                } \
            } \
            else { \
                CopyRect(&(drc[idx]), &src); \
                idx++; \
            } 

int CreateNodeRoundMaskRect (ZORDERINFO* zi, ZORDERNODE* node, 
        const DWORD type, const RECT *rc)
{
    RECT* roundrc = NULL;
    RECT rect;
    int max_rect_num = 1, free_slot, rc_idx;
    int i, z, r = 10;
    unsigned short idx;
    MASKRECT *firstmaskrect;

    if (type & ZOF_TW_TROUNDCNS)
        max_rect_num += r;
    if (type & ZOF_TW_BROUNDCNS)
        max_rect_num += r;

    roundrc = calloc (1, max_rect_num * sizeof(RECT));

    if (!roundrc)
        return -1;

    /*generate region of top round corner*/
    rc_idx = 0;
    if (type & ZOF_TW_TROUNDCNS) {
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

    /*generate region of bottom round corner*/
    if (type & ZOF_TW_BROUNDCNS) {
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
    else{
        SetRect (&rect, rc->left, rc->top+RECTHP(rc)-r,
                    rc->left+RECTWP(rc), rc->top+RECTHP(rc));
        do_with_round_rect (rect, roundrc, rc_idx);
    }
  
    /* if have enough mask rect */  
    if ( rc_idx > __mg_get_idle_slot((unsigned char*)GET_MASKRECT_USAGEBMP(zi),
                zi->size_maskrect_usage_bmp))
    {
        free(roundrc);
        return -1;
    }

    firstmaskrect = GET_MASKRECT(zi);

    /* allocate space*/
    idx = 0;
    for(i = 0; i < rc_idx; i++) {
        free_slot = __mg_lookfor_unused_slot (
                            (unsigned char*)GET_MASKRECT_USAGEBMP(zi), 
                            zi->size_maskrect_usage_bmp, 1);
        if (free_slot == -1) {
            _MG_PRINTF ("KERNEL: __mg_lookfor_unused_slot failed\n");
            return -1;
        }

        (firstmaskrect+free_slot)->prev = idx;
        if (idx != 0) {
            (firstmaskrect+idx)->next = free_slot;
        }
        else {
            node->idx_mask_rect = free_slot;  
        }
        idx = free_slot;
    }

    if (idx != 0) {
        (firstmaskrect+idx)->prev = 0;
    }
 
    /*get value*/
    idx = node->idx_mask_rect;
    i=0;
    while(idx != 0) {
        (firstmaskrect+idx)->left = (unsigned short)roundrc[i].left;
        (firstmaskrect+idx)->top = (unsigned short)roundrc[i].top;
        (firstmaskrect+idx)->right = (unsigned short)roundrc[i].right;
        (firstmaskrect+idx)->bottom = (unsigned short)roundrc[i].bottom;
        idx = (firstmaskrect+idx)->next;
        i++;
    }
 
    free (roundrc);

    return 0;
}

static int AllocZOrderNode (int cli, HWND hwnd, HWND main_win, 
                DWORD flags, const RECT *rc, const char *caption)
{
    DWORD type = flags & ZOF_TYPE_MASK;
    ZORDERINFO* zi = _get_zorder_info(cli);
    int *first = NULL, *nr_nodes = NULL;
    int free_slot, slot, old_first;
    ZORDERNODE* nodes;

    switch (flags & ZOF_TYPE_MASK) {
        case ZOF_TYPE_GLOBAL:
            if (zi->nr_globals < zi->max_nr_globals) {
                first = &zi->first_global;
                nr_nodes = &zi->nr_globals;
            }
            break;
        case ZOF_TYPE_TOPMOST:
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
        default:
            break;
    }

    if (first == NULL) {
        fprintf (stderr, "Cann't find first zorder node. \n");
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0 && flags & ZOF_VISIBLE)
        srvForceCloseMenu (0);
#endif

    nodes = GET_ZORDERNODE(zi);

    /* lock zi for change */
    lock_zi_for_change (zi);

    /* the slot must be larger than zero */
    if (type == ZOF_TYPE_GLOBAL)
        free_slot = __mg_lookfor_unused_slot ((BYTE*)(zi + 1), 
                                        zi->max_nr_globals, 1);
    else {
        free_slot = __mg_lookfor_unused_slot ((BYTE*)(zi + 1) 
                        + (zi->max_nr_globals >> 3), 
                        zi->size_usage_bmp - (zi->max_nr_globals >> 3), 1);

        if (free_slot >= 0) {
            free_slot += zi->max_nr_globals;
        }
    }

    if (-1 == free_slot) {
        /* unlock zorderinfo for change. */
        unlock_zi_for_change (zi);
        fprintf (stderr, "Cann't find unused slot. \n");
        return -1;
    }

    nodes [free_slot].flags = flags;
    nodes [free_slot].rc = *rc;
    nodes [free_slot].age = 1;
    nodes [free_slot].cli = cli;
    nodes [free_slot].fortestinghwnd = hwnd;
    nodes [free_slot].main_win = main_win;
    nodes [free_slot].idx_mask_rect = 0;

    if (flags & ZOF_TW_TROUNDCNS || flags & ZOF_TW_BROUNDCNS) {
        RECT cli_rect;

        SetRect(&cli_rect, 0, 0, RECTW(nodes[free_slot].rc), 
                RECTH(nodes[free_slot].rc));
        CreateNodeRoundMaskRect (__mg_zorder_info, 
                &nodes[free_slot], flags, &cli_rect);
    }

    if (caption) {
        PLOGFONT menufont;
        int fit_chars, pos_chars[MAX_CAPTION_LEN], caplen;

        menufont = GetSystemFont (SYSLOGFONT_MENU);
        caplen = strlen(caption);

        if (caplen < 32) {
            strcpy (nodes[free_slot].caption, caption);
        } else {
            int tail_pos;
            GetTextCharPos (menufont, caption, caplen, (32 - 3), /* '...' = 3*/
                            &fit_chars, pos_chars);

            tail_pos = pos_chars[fit_chars-1];
            
            if ((tail_pos + 3) >= 32 && fit_chars > 4) {
                tail_pos = pos_chars[fit_chars-2];
                if (tail_pos + 3 >= 32) {
                    tail_pos = pos_chars[fit_chars-3];
                    if (tail_pos + 3 >= 32) {
                        tail_pos = pos_chars[fit_chars-4];
                    }
                }
            }
            memcpy(nodes[free_slot].caption, caption, tail_pos);
            strcpy ((nodes[free_slot].caption + tail_pos), "...");
            
        }
    }

    /* check influenced zorder nodes */
    if (flags & ZOF_VISIBLE) {
        if (type >= ZOF_TYPE_NORMAL) {
            slot = zi->first_normal;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE &&
                    DoesIntersect (&nodes [free_slot].rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                }
            }
        }
        if (type >= ZOF_TYPE_TOPMOST) {
            slot = zi->first_topmost;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                    DoesIntersect (&nodes [free_slot].rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                }
            }
        }
        if (type >= ZOF_TYPE_GLOBAL) {
            slot = zi->first_global;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                    DoesIntersect (&nodes [free_slot].rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                }
            }
        }
        if (DoesIntersect (&nodes [free_slot].rc, &g_rcScr)) {
            nodes [0].age ++;
        }
    }

#if defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)
    if (*first == 0 
            || (nodes [*first].flags & ZOF_TF_TOPFOREVER) != ZOF_TF_TOPFOREVER )
    {
        old_first = *first;
        nodes [old_first].prev = free_slot;
        nodes [free_slot].prev = 0;
        nodes [free_slot].next = old_first;
    }
    else {
        int pre_idx = *first;
        while(*first) {
            if ((nodes [*first].flags & ZOF_TF_TOPFOREVER) == ZOF_TF_TOPFOREVER) {
                pre_idx = *first;
                first = &nodes[*first].next;
            }
            else break;
        }
        old_first = pre_idx;
        nodes [free_slot].prev = old_first;
        nodes [free_slot].next = nodes [old_first].next;
        nodes [nodes [old_first].next].prev = free_slot;
        nodes [old_first].next = free_slot;
    }

    if (first == &zi->first_global || first == &zi->first_topmost
            || first == &zi->first_normal) {
        *first = free_slot;
    }

#else

    /* chain the new node */
    old_first = *first;
    nodes [old_first].prev = free_slot;
    nodes [free_slot].prev = 0;
    nodes [free_slot].next = old_first;
    *first = free_slot;
#endif

    *nr_nodes += 1;

    /* unlock zi for change ... */
    unlock_zi_for_change (zi);

    return free_slot;
}

static int FreeZOrderNode (int cli, int idx_znode)
{
    DWORD type;
    int *first = NULL, *nr_nodes = NULL;
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;
    int slot, old_active, next_active;
    RECT rc;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0 && nodes [idx_znode].flags & ZOF_VISIBLE)
        srvForceCloseMenu (0);
#endif

    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;

    switch (type) {
        case ZOF_TYPE_GLOBAL:
            first = &zi->first_global;
            nr_nodes = &zi->nr_globals;
            break;
        case ZOF_TYPE_TOPMOST:
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
        default:
            break;
    }

    if (first == NULL) {
        return -1;
    }

    /* please lock zi for change*/
    lock_zi_for_change (zi);

    /* Free round corners mask rect. */
    if (type & ZOF_TW_TROUNDCNS || 
            type & ZOF_TW_BROUNDCNS) {

#if 0
        int tmp, idx = nodes[idx_znode].idx_mask_rect;
        MASKRECT * first = GET_MASKRECT(zi);

        while (idx) {
            __mg_slot_clear_use ((unsigned char*)GET_MASKRECT_USAGEBMP(zi), idx);
            tmp = (first+idx)->next;
            (first+idx)->next = 0;
            (first+idx)->prev = 0;
            idx = tmp;
        }
#else
        clean_znode_maskrect (zi, nodes, idx_znode);
#endif
    }

    SetClipRgn (&sg_UpdateRgn, &nodes [idx_znode].rc);

    /* check influenced zorder nodes */
    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        rc = nodes [idx_znode].rc;

        slot = nodes [idx_znode].next;
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE &&
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                nodes [slot].age ++;
                nodes [slot].flags |= ZOF_REFERENCE;
            }
        }
        if (type > ZOF_TYPE_TOPMOST) {
            slot = zi->first_topmost;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE &&
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                    nodes [slot].age ++;
                    nodes [slot].flags |= ZOF_REFERENCE;
                }
            }
        }
        if (type > ZOF_TYPE_NORMAL) {
            slot = zi->first_normal;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                    nodes [slot].age ++;
                    nodes [slot].flags |= ZOF_REFERENCE;
                }
            }
        }

        if (SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
            nodes [0].age ++;
            nodes [0].flags |= ZOF_REFERENCE;
        }
    }

    /* unchain it */
    unchain_znode ((unsigned char*)(zi+1), nodes, idx_znode);
    nodes [idx_znode].fortestinghwnd = 0;

    if (*first == idx_znode) {
        *first = nodes [idx_znode].next;
    }
    *nr_nodes -= 1;

    old_active = zi->active_win;
    if (idx_znode == zi->active_win)
        zi->active_win = 0;

    /* unlock zi for change  */
    unlock_zi_for_change (zi);

    /* update all znode if it's dirty */
    do_for_all_znodes (&rc, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_REFERENCE) {
        SendMessage (HWND_DESKTOP, 
                        MSG_ERASEDESKTOP, 0, (WPARAM)&rc);
        nodes [0].flags &= ~ZOF_REFERENCE;
    }

    /* if active_win is this window, change it */
    if (idx_znode == old_active) {
        next_active = get_next_visible_mainwin (zi, idx_znode);
        dskSetActiveZOrderNode (nodes [next_active].cli, next_active);
    }

    return 0;
}

static DWORD get_znode_flags_from_style (PMAINWIN pWin)
{
    DWORD zt_type = 0;

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE) 
    if (mgIsServer) {
       zt_type |= ZOF_TYPE_GLOBAL;
    } else 
#endif
    {
        if (pWin->dwExStyle & WS_EX_TOPMOST)
            zt_type |= ZOF_TYPE_TOPMOST;
        else if (pWin->WinType == TYPE_CONTROL && 
                    (pWin->pMainWin->dwExStyle & WS_EX_TOPMOST))
            zt_type |= ZOF_TYPE_TOPMOST;
        else
            zt_type |= ZOF_TYPE_NORMAL;
    }

    if (pWin->dwStyle & WS_VISIBLE)
        zt_type |= ZOF_VISIBLE;
    if (pWin->dwStyle & WS_DISABLED)
        zt_type |= ZOF_DISABLED;

    if (pWin->WinType == TYPE_MAINWIN)
        zt_type |= ZOF_TF_MAINWIN;

    if (pWin->dwExStyle & WS_EX_TOOLWINDOW)
        zt_type |= ZOF_TF_TOOLWIN;

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
    ZORDERINFO* zi = _get_zorder_info(cli);
    int free_slot, i, cur_idx, idx, old_num = 0;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
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
        int idle = __mg_get_idle_slot((unsigned char*)GET_MASKRECT_USAGEBMP(zi), 
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
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);

    /* lock zi for change */
    lock_zi_for_change (zi);

#if 0
    int idx, tmp;
    MASKRECT *first;
    first = GET_MASKRECT(zi);
    idx = nodes [idx_znode].idx_mask_rect;

    while(idx) {
       __mg_slot_clear_use((unsigned char*)GET_MASKRECT_USAGEBMP(zi), idx);
       (first+idx)->prev = 0;
       tmp = (first+idx)->next;
       (first+idx)->next = 0;
       idx = tmp;
    }

    nodes[idx_znode].idx_mask_rect = 0;
#else
    clean_znode_maskrect (zi, nodes, idx_znode);
#endif

    /* unlock zi for change  */
    unlock_zi_for_change (zi);
    return 0;
}

static int update_client_window_rgn (int cli, HWND hwnd);

#if defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)
static int dskMove2Top (int cli, int idx_znode);
static int dskShowWindow (int cli, int idx_znode);
static int dskHideWindow (int cli, int idx_znode);
static ZORDERINFO* _get_zorder_info (int cli);

void dskRefreshAllClient (const RECT* invrc)
{
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (mgIsServer) {
        ZORDERINFO* zi = _get_zorder_info (0);
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

int dskCreateTopZOrderNode (int cli, const RECT *rc)
{
    int idx_znode = 0;
    int zt_type = ZOF_VISIBLE | ZOF_TF_MAINWIN;
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (mgIsServer) {
        zt_type |= ZOF_TYPE_GLOBAL;
    } else
#endif
        zt_type |= ZOF_TYPE_TOPMOST;

    idx_znode = AllocZOrderNode (0, 0,
            (HWND)0,
            zt_type,
            rc, "");
    dskShowWindow (cli, idx_znode);
    return idx_znode;
}

int dskDestroyTopZOrderNode (int cli, int idx_znode)
{
        return FreeZOrderNode (cli, idx_znode);
}

int dskSetTopForEver(int cli, int idx_znode, BOOL show)
{
    // NUV DWORD type;
    ZORDERINFO* zi = _get_zorder_info (cli);
    ZORDERNODE* nodes;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nodes = GET_ZORDERNODE(zi);
    // NUV type = nodes [idx_znode].flags & ZOF_TYPE_MASK;

    /* lock zi for change */
    lock_zi_for_change (zi);

    nodes[idx_znode].flags |= ZOF_TF_TOPFOREVER;  

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    if (show) {
        dskShowWindow (cli, idx_znode);
        return dskMove2Top (cli, idx_znode);
    }
    else {
        return dskHideWindow (cli, idx_znode);
    }
}
#endif /* _MG_ENABLE_SCREENSAVER || _MG_ENABLE_WATERMARK */

static int dskMove2Top (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = _get_zorder_info (cli);
    int *first = NULL;
    ZORDERNODE* nodes;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nodes = GET_ZORDERNODE(zi);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;

    switch (type) {
        case ZOF_TYPE_GLOBAL:
            first = &zi->first_global;
            break;
        case ZOF_TYPE_TOPMOST:
            first = &zi->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = &zi->first_normal;
            break;
        default:
            break;
    }

    if (first == NULL || *first == idx_znode)
        return -1;

    EmptyClipRgn (&sg_UpdateRgn);

    /* lock zi for change */
    lock_zi_for_change (zi);

    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int slot;
        RECT rc = nodes [idx_znode].rc;

        if (type == ZOF_TYPE_NORMAL) {
            slot = zi->first_normal;
            for (; slot != idx_znode; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE &&
                                DoesIntersect (&rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                    AddClipRect(&sg_UpdateRgn, &nodes [slot].rc);
                }
            }
        }
        if (type == ZOF_TYPE_TOPMOST) {
            slot = zi->first_topmost;
            for (; slot != idx_znode; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                                DoesIntersect (&rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                    AddClipRect (&sg_UpdateRgn, &nodes [slot].rc);
                }
            }
        }
        if (type == ZOF_TYPE_GLOBAL) {
            slot = zi->first_global;
            for (; slot != idx_znode; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                                DoesIntersect (&rc, &nodes [slot].rc)) {
                    nodes [slot].age ++;
                    AddClipRect (&sg_UpdateRgn, &nodes [slot].rc);
                }
            }
        }
    }

    /* unchain it and move to top */
    if (nodes [idx_znode].prev) {
        nodes [nodes [idx_znode].prev].next = nodes [idx_znode].next;
    }
    if (nodes [idx_znode].next) {
        nodes [nodes [idx_znode].next].prev = nodes [idx_znode].prev;
    }

#if defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)
    if (!(*first) || ((nodes [*first].flags & ZOF_TF_TOPFOREVER) != ZOF_TF_TOPFOREVER))
    {
        nodes [idx_znode].prev = nodes[*first].prev;
        nodes [idx_znode].next = *first;
        nodes [*first].prev = idx_znode;
    }
    else {
        int pre_idx = *first;
        while(*first) {
            if ((nodes [*first].flags & ZOF_TF_TOPFOREVER) == ZOF_TF_TOPFOREVER) {
                pre_idx = *first;
                first = &nodes[*first].next;
            }
            else break;
        }
        nodes [idx_znode].prev = pre_idx;
        nodes [idx_znode].next = nodes [pre_idx].next;
        nodes [nodes[pre_idx].next].prev = idx_znode;
        nodes [pre_idx].next = idx_znode;
    }

    if (first == &zi->first_global || first == &zi->first_topmost
            || first == &zi->first_normal) {
        *first = idx_znode;
    }
#else
    nodes [idx_znode].prev = 0;
    nodes [idx_znode].next = *first;
    nodes [*first].prev = idx_znode;
    *first = idx_znode;
#endif

    nodes [idx_znode].age ++;

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    if ((nodes [idx_znode].flags & ZOF_VISIBLE) && nodes [idx_znode].fortestinghwnd) {
        update_client_window_rgn (nodes [idx_znode].cli, 
                        nodes [idx_znode].fortestinghwnd);

    }

    return 0;
}

static int dskShowWindow (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = _get_zorder_info(cli); 
    ZORDERNODE* nodes;
    int *first = NULL;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) ||
            idx_znode <= 0) {
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nodes = GET_ZORDERNODE(__mg_zorder_info);
    
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    switch (type) {
        case ZOF_TYPE_GLOBAL:
            first = &zi->first_global;
            break;
        case ZOF_TYPE_TOPMOST:
            first = &zi->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = &zi->first_normal;
            break;
        default:
            break;
    }

    if (first == NULL)
        return -1;

    /* lock zi for change */
    lock_zi_for_change (zi);

    {
        int slot;
        RECT rc = nodes [idx_znode].rc;

        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_NORMAL);
        }
        if (type > ZOF_TYPE_TOPMOST) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_TOPMOST);
        }
        if (type > ZOF_TYPE_GLOBAL) {
            do_for_all_znodes (&rc, zi, _cb_intersect_rc, ZT_GLOBAL);
        }

        slot = nodes [idx_znode].next;
        for (; slot != 0; slot = nodes [slot].next)
        {
            if (nodes [slot].flags & ZOF_VISIBLE && 
                            DoesIntersect (&rc, &nodes [slot].rc)) {
                nodes [slot].age ++;
            }
        }

        if (DoesIntersect (&rc, &g_rcScr)) {
            nodes [0].age ++;
        }
        nodes [idx_znode].age ++;
        nodes [idx_znode].flags |= ZOF_VISIBLE;
    }

    /* unlock zi for change ... */
    unlock_zi_for_change (zi);

    return 0;
}

static int dskHideWindow (int cli, int idx_znode)
{
    DWORD type;
    ZORDERINFO* zi = _get_zorder_info(cli);
    int *first = NULL;
    ZORDERNODE* nodes;

    if (idx_znode > zi->max_nr_globals 
            + zi->max_nr_topmosts + zi->max_nr_normals) {
        return -1;
    }

#ifdef _MGHAVE_MENU
    if (zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nodes = GET_ZORDERNODE(zi);
    
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;
    switch (type) {
        case ZOF_TYPE_GLOBAL:
            first = &zi->first_global;
            break;
        case ZOF_TYPE_TOPMOST:
            first = &zi->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = &zi->first_normal;
            break;
        default:
            break;
    }

    if (first == NULL)
        return -1;

    /* lock zi for change */
    lock_zi_for_change (zi);

    /* check influenced zorder nodes */
    SetClipRgn (&sg_UpdateRgn, &nodes [idx_znode].rc);
    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        int slot;

        slot = nodes [idx_znode].next;
        for (; slot > 0; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE && 
                    subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                nodes [slot].age ++;
                nodes [slot].flags |= ZOF_REFERENCE;
            }
        }
        if (type > ZOF_TYPE_TOPMOST) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_TOPMOST);
        }
        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes (&sg_UpdateRgn, zi, _cb_update_rc, ZT_NORMAL);
        }
        if (SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
            nodes [0].age ++;
            nodes [0].flags |= ZOF_REFERENCE;
        }
    }

    if (idx_znode && (nodes [idx_znode].flags & ZOF_TF_MAINWIN
         && (nodes [idx_znode].flags & ZOF_VISIBLE))) {
        post_msg_by_znode_p (zi, nodes + idx_znode, 
                        MSG_NCACTIVATE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode, 
                        MSG_ACTIVE, FALSE, 0);
        post_msg_by_znode_p (zi, nodes + idx_znode, 
                        MSG_KILLFOCUS, 0, 0);
    }

    nodes [idx_znode].flags &= ~ZOF_VISIBLE;
    /*
     * do not reset the age to zero.
     * nodes [idx_znode].age = 0;
     */

    /* unlock zi for change */
    unlock_zi_for_change (zi);

    /* update all znode if it's dirty */
    do_for_all_znodes (&nodes [idx_znode].rc, zi, _cb_update_znode, ZT_ALL);

    if (nodes [0].flags & ZOF_REFERENCE) {
        SendMessage (HWND_DESKTOP, 
                        MSG_ERASEDESKTOP, 0, (WPARAM)&nodes [idx_znode].rc);
        nodes [0].flags &= ~ZOF_REFERENCE;
    }

    return 0;
}

static int dskMoveWindow (int cli, int idx_znode, const RECT* rcWin)
{
    DWORD type;
    int *first = NULL;
    ZORDERNODE* nodes;
    RECT rcInv[4], rcOld, rcInter, tmprc;
    int i, slot, nInvCount;
    unsigned short idx; 
    CLIPRGN bblt_rgn;
    MASKRECT *firstmaskrect, *maskrect;
    ZORDERINFO* zi = _get_zorder_info(cli);
    
    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) ||
            idx_znode < 0) {
        return -1;
    }

    nodes = GET_ZORDERNODE(zi);
    type = nodes [idx_znode].flags & ZOF_TYPE_MASK;

    switch (type) {
        case ZOF_TYPE_GLOBAL:
            first = &zi->first_global;
            break;
        case ZOF_TYPE_TOPMOST:
            first = &zi->first_topmost;
            break;
        case ZOF_TYPE_NORMAL:
            first = &zi->first_normal;
            break;
        default:
            break;
    }

    if (first == NULL)
        return -1;

    if (memcmp (&nodes [idx_znode].rc, rcWin, sizeof (RECT)) == 0)
        return 0;

    if ( (RECTW(nodes[idx_znode].rc) != RECTWP(rcWin)  || 
            RECTH(nodes[idx_znode].rc) != RECTHP(rcWin)) && 
            (nodes[idx_znode].flags & ZOF_TW_TROUNDCNS || 
             nodes[idx_znode].flags & ZOF_TW_BROUNDCNS) ) 
    {
        RECT cli_rect;
        SetRect(&cli_rect, 0, 0, RECTWP(rcWin), RECTHP(rcWin));

        lock_zi_for_change(zi);

        if (nodes[idx_znode].idx_mask_rect != 0) {

            idx = nodes[idx_znode].idx_mask_rect; 
            firstmaskrect = GET_MASKRECT(zi);

            while (idx) {
                __mg_slot_clear_use((unsigned char*)GET_MASKRECT_USAGEBMP(zi), idx);
                idx = ((MASKRECT *)(firstmaskrect+idx))->next;
            }
        }

        CreateNodeRoundMaskRect (zi, &nodes[idx_znode], 
                nodes[idx_znode].flags, &cli_rect);

        unlock_zi_for_change(zi);
    }

#ifdef _MGHAVE_MENU
    if (nodes [idx_znode].flags & ZOF_VISIBLE && zi->cli_trackmenu >= 0)
        srvForceCloseMenu (0);
#endif

    nInvCount = SubtractRect (rcInv, &nodes [idx_znode].rc, rcWin);

    if (nodes [idx_znode].flags & ZOF_VISIBLE) {
        /* lock zi for change */
        lock_zi_for_change (zi);

        if (type > ZOF_TYPE_NORMAL) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_NORMAL);
        }
        if (type > ZOF_TYPE_TOPMOST) {
            do_for_all_znodes ((void*)rcWin, zi, _cb_intersect_rc, ZT_TOPMOST);
        }

        slot = nodes [idx_znode].next;
        for (; slot != 0; slot = nodes [slot].next)
        {
            if (nodes [slot].flags & ZOF_VISIBLE && 
                            DoesIntersect (rcWin, &nodes [slot].rc)) {
                nodes [slot].age ++;
            }
        }

        if (DoesIntersect (rcWin, &g_rcScr)) {
            nodes [0].age ++;
        }

        /* check influenced zorder nodes */
       // for (i = 0; i < nInvCount; i++) {
            //SetClipRgn (&sg_UpdateRgn, rcInv + i);
            SetClipRgn (&sg_UpdateRgn, &(nodes [idx_znode].rc));

            slot = nodes [idx_znode].next;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                        !(nodes [slot].flags & ZOF_REFERENCE) &&
                        subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                    nodes [slot].age ++;
                    nodes [slot].flags |= ZOF_REFERENCE;
                }
            }

            if (type > ZOF_TYPE_TOPMOST) {
                slot = zi->first_topmost;
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE && 
                            !(nodes [slot].flags & ZOF_REFERENCE) &&
                            subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                        nodes [slot].age ++;
                        nodes [slot].flags |= ZOF_REFERENCE;
                    }
                }
            }

            if (type > ZOF_TYPE_NORMAL) {
                slot = zi->first_normal;
                for (; slot > 0; slot = nodes [slot].next) {
                    if (nodes [slot].flags & ZOF_VISIBLE && 
                            !(nodes [slot].flags & ZOF_REFERENCE) &&
                            subtract_rgn_by_node(&sg_UpdateRgn, zi, &nodes[slot])) {
                        nodes [slot].age ++;
                        nodes [slot].flags |= ZOF_REFERENCE;
                    }
                }
            }

            if (!(nodes [0].flags & ZOF_REFERENCE) &&
                            SubtractClipRect (&sg_UpdateRgn, &g_rcScr)) {
                nodes [0].age ++;
                nodes [0].flags |= ZOF_REFERENCE;
            }
       // }

        rcOld = nodes [idx_znode].rc;
        nodes [idx_znode].rc = *rcWin;
        nodes [idx_znode].age ++;

#if defined (_MGRM_PROCESSES) && !defined (_MGRM_STANDALONE)
        if (cli == 0 || mgClients [cli].layer == SHAREDRES_TOPMOST_LAYER) {
#endif
            /* Copy window content to new postion */
            InitClipRgn (&bblt_rgn, &sg_FreeClipRectList);
            if (nodes[idx_znode].idx_mask_rect == 0){
                SelectClipRect (HDC_SCREEN_SYS, rcWin);
            } else {
                firstmaskrect = GET_MASKRECT(zi);
                idx = nodes [idx_znode].idx_mask_rect;

                while (idx) {
                    maskrect = firstmaskrect + idx;
                    SetRect (&tmprc, rcWin->left + maskrect->left, 
                            rcWin->top + maskrect->top, 
                            rcWin->left + maskrect->right, 
                            rcWin->top + maskrect->bottom);
                    if ( DoesIntersect (&tmprc, &g_rcScr)) {
                        IntersectRect (&tmprc, &tmprc, &g_rcScr);
                        AddClipRect (&bblt_rgn, &tmprc);
                    }
                    idx = maskrect->next;
                }
                SelectClipRegion(HDC_SCREEN_SYS, &bblt_rgn);
            }
            
            slot = 0;
            switch (type) {
            case ZOF_TYPE_NORMAL:
                do_for_all_znodes (NULL, zi, 
                                _cb_exclude_rc, ZT_GLOBAL);
                do_for_all_znodes (NULL, zi, 
                                _cb_exclude_rc, ZT_TOPMOST);
                slot = zi->first_normal;
                break;
            case ZOF_TYPE_TOPMOST:
                do_for_all_znodes (NULL, zi, 
                                _cb_exclude_rc, ZT_GLOBAL);
                slot = zi->first_topmost;
                break;
            case ZOF_TYPE_GLOBAL:
                slot = zi->first_global;
                break;
            case ZOF_TYPE_DESKTOP:
                do_for_all_znodes (NULL, zi, 
                                _cb_exclude_rc, ZT_ALL);
                break;
            default:
                break;
            }

            while (slot) {
                if (slot == idx_znode) {
                    break;
                }
#if 0
                if (nodes [slot].flags & ZOF_VISIBLE) {
                    ExcludeClipRect (HDC_SCREEN_SYS, &nodes [slot].rc);
                }
#endif
                /* houhh 20090730, if slot wind is no regular.*/
                if (nodes [slot].flags & ZOF_VISIBLE) {
                    if (nodes[slot].idx_mask_rect == 0){
                        ExcludeClipRect (HDC_SCREEN_SYS, &nodes [slot].rc);
                    } else {
                        RECT rc;
                        GetWindowRect(nodes [slot].fortestinghwnd, &rc);
                        firstmaskrect = GET_MASKRECT(zi);
                        idx = nodes [slot].idx_mask_rect;
                        while (idx) {
                            maskrect = firstmaskrect + idx;
                            SetRect (&tmprc, rc.left + maskrect->left, 
                                    rc.top + maskrect->top, 
                                    rc.left + maskrect->right, 
                                    rc.top + maskrect->bottom);
                            if ( DoesIntersect (&tmprc, &g_rcScr)) {
                                IntersectRect (&tmprc, &tmprc, &g_rcScr);
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
            
            /* Restore the clip region of HDC_SCREEN_SYS */
            SelectClipRect (HDC_SCREEN_SYS, &g_rcScr);
            EmptyClipRgn(&bblt_rgn);
#if defined (_MGRM_PROCESSES) && !defined (_MGRM_STANDALONE)
        }
#endif

        /* unlock zi for change ... */
        unlock_zi_for_change (zi);

        /* check the invalid rect of the window */
        EmptyClipRgn (&sg_UpdateRgn);
        nInvCount = SubtractRect (rcInv, &rcOld, &g_rcScr);
        for (i = 0; i < nInvCount; i++) {
            AddClipRect (&sg_UpdateRgn, rcInv + i);
        }

        if (type < ZOF_TYPE_GLOBAL) {
            slot = zi->first_global;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                            IntersectRect (&rcInter, &rcOld, &nodes [slot].rc))
                    AddClipRect(&sg_UpdateRgn, &rcInter);
            }
        }

        if (type < ZOF_TYPE_TOPMOST) {
            slot = zi->first_topmost;
            for (; slot > 0; slot = nodes [slot].next) {
                if (nodes [slot].flags & ZOF_VISIBLE && 
                            IntersectRect (&rcInter, &rcOld, &nodes [slot].rc))
                    AddClipRect(&sg_UpdateRgn, &rcInter);
            }
        }

        slot = *first;
        for (; slot != idx_znode; slot = nodes [slot].next) {
            if (nodes [slot].flags & ZOF_VISIBLE && 
                        IntersectRect (&rcInter, &rcOld, &nodes [slot].rc))
                AddClipRect(&sg_UpdateRgn, &rcInter);
        }
        do_for_all_znodes (&rcOld, zi, _cb_update_znode, ZT_ALL);

        if (nodes [0].flags & ZOF_REFERENCE) {
            SendMessage (HWND_DESKTOP, 
                            MSG_ERASEDESKTOP, 0, (LPARAM)&rcOld); 
            nodes [0].flags &= ~ZOF_REFERENCE;
        }

        OffsetRegion (&sg_UpdateRgn, 
                        rcWin->left - rcOld.left, 
                        rcWin->top - rcOld.top);

        update_client_window_rgn (nodes [idx_znode].cli, 
                nodes [idx_znode].fortestinghwnd);
    }
    else {
        lock_zi_for_change (zi);

        nodes [idx_znode].rc = *rcWin;
        nodes [idx_znode].age ++;

        unlock_zi_for_change (zi);
    }

    return 0;
}

static int dskEnableZOrderNode (int cli, int idx_znode, int flags)
{
    ZORDERINFO* zi = _get_zorder_info(cli);
    ZORDERNODE* nodes;

    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals) ||
            idx_znode < 0) {
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
    
    if(context)
    {
        bg_bmp = ((DEF_CONTEXT *)context)->bg;
        pic_x  = ((DEF_CONTEXT *)context)->x;
        pic_y  = ((DEF_CONTEXT *)context)->y;
    }

    SetBrushColor (dc_desktop, 
        GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP));

    if (inv_rc) {
        SelectClipRect (dc_desktop, inv_rc);
        FillBox (dc_desktop, inv_rc->left, inv_rc->top, 
                RECTWP (inv_rc), RECTHP (inv_rc));
    }
    else {
        SelectClipRect (dc_desktop, &g_rcDesktop);
        FillBox(dc_desktop, g_rcDesktop.left, g_rcDesktop.top,
                g_rcDesktop.right,
                g_rcDesktop.bottom);
    }

    if (bg_bmp) {
        FillBoxWithBitmap (dc_desktop, pic_x, pic_y,
                bg_bmp->bmWidth, bg_bmp->bmHeight, bg_bmp);
    }

}

static void def_keyboard_handler(void* context, int message, 
		                               WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
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

static void def_customize_desktop_menu (void* context, HMENU hmnu, int start_pos)
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
    switch(message)
    {
        case MSG_DT_LBUTTONDOWN:
        case MSG_DT_LBUTTONUP:
        case MSG_DT_LBUTTONDBLCLK:
        case MSG_DT_MOUSEMOVE:
        case MSG_DT_RBUTTONDOWN:
        case MSG_DT_RBUTTONDBLCLK:
			break;

        case MSG_DT_RBUTTONUP:
			{
				x = LOSWORD (lParam);
				y = HISWORD (lParam);

				TrackPopupMenu (sg_DesktopMenu, TPM_DEFAULT, x, y, HWND_DESKTOP);

				break;
			}
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
	if(context) {
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

    if(GetMgEtcValue (__mg_def_renderer->name, 
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

static PBITMAP dskLoadBgPicture (void)
{
    return (PBITMAP)GetSystemBitmapEx (__mg_def_renderer->name, 
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

static void* def_init(void)
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
        bg_bmp = dskLoadBgPicture ();

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
		dsk_ops->deinit(dt_context);
	}

	if (usr_dsk_ops->init) {
		dt_context = usr_dsk_ops->init();
	}

	tmp_ops = dsk_ops;
	dsk_ops = usr_dsk_ops;

    SendMessage(HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    return tmp_ops;
}

static BOOL _cb_bcast_msg (void* context, 
                const ZORDERINFO* zi, ZORDERNODE* node)
{
    PMAINWIN pWin;
    PMSG pMsg = (PMSG)context;

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (node->cli == __mg_client_id) {
#endif
        pWin = (PMAINWIN)node->fortestinghwnd;
        if (pWin && pWin->WinType != TYPE_CONTROL) {
            PostMessage ((HWND)pWin, pMsg->message, pMsg->wParam, pMsg->lParam);
            return TRUE;
        }
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
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

#ifdef _DEB
void GUIAPI DumpWindow (FILE* fp, HWND hWnd)
{
    PMAINWIN pWin = (PMAINWIN)hWnd;
    PCONTROL pCtrl;

    if (pWin->DataType != TYPE_HWND) {
        fprintf (fp, "DumpWindow: Invalid DataType!\n");
        return;
    }

    if (pWin->WinType == TYPE_MAINWIN) {
        fprintf (fp, "=============== Main Window %#x==================\n", 
                        hWnd);

        fprintf (fp, "Rect        -- (%d, %d, %d, %d)\n", 
                        pWin->left, pWin->top, pWin->right, pWin->bottom);
        fprintf (fp, "Client      -- (%d, %d, %d, %d)\n", 
                        pWin->cl, pWin->ct, pWin->cr, pWin->cb);

        fprintf (fp, "Style       -- %lx\n", pWin->dwStyle);
        fprintf (fp, "ExStyle     -- %lx\n", pWin->dwExStyle);

        //fprintf (fp, "PrivDC     -- %#x\n", pWin->privDC);

        fprintf (fp, "AddData     -- %lx\n", pWin->dwAddData);
        fprintf (fp, "AddData2    -- %lx\n", pWin->dwAddData2);

        fprintf (fp, "WinProc     -- %p\n", pWin->MainWindowProc);

        fprintf (fp, "Caption     -- %s\n", pWin->spCaption);
        fprintf (fp, "ID          -- %d\n", pWin->id);

        fprintf (fp, "FirstChild  -- %#x\n", pWin->hFirstChild);
        pCtrl = (PCONTROL)pWin->hFirstChild;
        while (pCtrl) {
            fprintf (fp, "    Child   -- %p(%d), %s(%d)\n", pCtrl, pCtrl->id, 
                                pCtrl->pcci->name, pCtrl->pcci->nUseCount);
            pCtrl = pCtrl->next;
        }
        fprintf (fp, "ActiveChild -- %#x\n", pWin->hActiveChild);

        fprintf (fp, "Hosting     -- %p\n", pWin->pHosting);
        fprintf (fp, "FirstHosted -- %p\n", pWin->pFirstHosted);
        fprintf (fp, "NextHosted  -- %p\n", pWin->pNextHosted);
        fprintf (fp, "BkColor     -- %d\n",  pWin->iBkColor);
        fprintf (fp, "Menu        -- %#x\n", pWin->hMenu);
        fprintf (fp, "Accel       -- %#x\n", pWin->hAccel);
        fprintf (fp, "Cursor      -- %#x\n", pWin->hCursor);
        fprintf (fp, "Icon        -- %#x\n", pWin->hIcon);
        fprintf (fp, "SysMenu     -- %#x\n", pWin->hSysMenu);
        fprintf (fp, "MsgQueue    -- %p\n", pWin->pMessages);
    }
    else {
        fprintf (fp, "=============== Control %#x==================\n", hWnd);
        pCtrl = (PCONTROL)hWnd;

        fprintf (fp, "Rect        -- (%d, %d, %d, %d)\n", 
                        pCtrl->left, pCtrl->top, pCtrl->right, pCtrl->bottom);
        fprintf (fp, "Client      -- (%d, %d, %d, %d)\n", 
                        pCtrl->cl, pCtrl->ct, pCtrl->cr, pCtrl->cb);

        fprintf (fp, "Style       -- %lx\n", pCtrl->dwStyle);
        fprintf (fp, "ExStyle     -- %lx\n", pCtrl->dwExStyle);

        fprintf (fp, "PrivCDC     -- %#x\n", pCtrl->privCDC);

        fprintf (fp, "AddData     -- %lx\n", pCtrl->dwAddData);
        fprintf (fp, "AddData2    -- %lx\n", pCtrl->dwAddData2);

        fprintf (fp, "WinProc     -- %p\n", pCtrl->ControlProc);

        fprintf (fp, "Caption     -- %s\n", pCtrl->spCaption);
        fprintf (fp, "ID          -- %d\n", pCtrl->id);

        fprintf (fp, "FirstChild  -- %p\n", pCtrl->children);
        fprintf (fp, "ActiveChild -- %p\n", pCtrl->active);
        fprintf (fp, "Parent      -- %p\n", pCtrl->pParent);
        fprintf (fp, "Next        -- %p\n", pCtrl->next);

        pCtrl = (PCONTROL)pCtrl->children;
        while (pCtrl) {
            fprintf (fp, "    Child   -- %p(%d), %s(%d)\n", pCtrl, pCtrl->id, 
                                pCtrl->pcci->name, pCtrl->pcci->nUseCount);
            pCtrl = pCtrl->next;
        }
    }

    fprintf (fp, "=================== End ==================\n");
    return; 
}
#endif /* _DEBUG */

int kernel_get_window_region (HWND pWin, CLIPRGN* region)
{
    RECT rc;
    MASKRECT *maskrect;
    ZORDERNODE* nodes;
    ZORDERINFO* zi;
    int idx_znode, idx, nr_mask_rects;

#if defined(_MGRM_THREADS) || defined(_MGRM_STANDALONE)
    zi = _get_zorder_info (0);
#else
    zi = _get_zorder_info (__mg_client_id);
#endif

    idx_znode = ((PMAINWIN)pWin)->idx_znode;
    if (idx_znode > (zi->max_nr_globals 
                    + zi->max_nr_topmosts + zi->max_nr_normals)
            || idx_znode <= 0) {
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
            goto err_ret;
        }

        idx = maskrect->next;
        nr_mask_rects ++;
    }

    if (nr_mask_rects == 0) {
        rc = nodes[idx_znode].rc;
        OffsetRect (&rc, rc.left, rc.top);
        if (!SetClipRgn (region, &rc))
            nr_mask_rects = -1;
    }

err_ret:
    /* unlock zi for read */
    unlock_zi_for_read (zi);

    return nr_mask_rects;
}

