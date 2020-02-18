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
** desktop-ths.c: The desktop for MiniGUI-Threads.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/04/19
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

#ifdef _MGRM_THREADS

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "menu.h"
#include "timer.h"
#include "dc.h"
#include "misc.h"
#include "devfont.h"

/******************************* global data *********************************/
/* system threads */
pthread_t __mg_desktop, __mg_parsor, __mg_timer;

/* pointer to desktop message queue */
PMSGQUEUE __mg_dsk_msg_queue;

/********************* Window management support *****************************/
#include "desktop-comm.c"

static BOOL InitWndManagementInfo (void)
{
    RECT rcScr = GetScreenRect();

    __mg_captured_wnd = 0;

#ifdef _MGHAVE_MENU
    sg_ptmi = NULL;
#endif

    __mg_ime_wnd = 0;
    sg_hCaretWnd = 0;

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &rcScr);
    pthread_mutex_init (&sg_ScrGCRInfo.lock, NULL);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);

    return TRUE;
}

BOOL mg_InitDesktop (void)
{
    int ret;

    /*
     * Init ZOrderInfo here.
     */
    ret = __kernel_alloc_z_order_info (DEF_NR_TOPMOSTS, DEF_NR_NORMALS);
    if (ret < 0) {
        _WRN_PRINTF ("KERNEL>Desktop: Can not initialize ZOrderInfo!\n");
        return FALSE;
    }

    /*
     * Init heap of clipping rects.
     */
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);

    /*
     * Init heap of invalid rects.
     */
    InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);

    // Init Window Management information.
    if (!InitWndManagementInfo()) {
        _WRN_PRINTF ("KERNEL>Desktop: Can not initialize window management information!\n");
        return FALSE;
    }

    return TRUE;
}

void* __kernel_desktop_main (void* data)
{
    MSG Msg;

    /* init message queue of desktop thread */
    if (!(__mg_dsk_msg_queue = mg_AllocMsgQueueForThisThread ()) ) {
        _WRN_PRINTF ("failed to allocate message queue\n");
        return NULL;
    }

    /* for threads mode, the idle handler for desktop thread is NULL */
    __mg_dsk_msg_queue->OnIdle = NULL;

    /* init desktop window */
    init_desktop_win ();

    DesktopWinProc (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    /* desktop thread is ready now */
    sem_post ((sem_t*)data);

    /* process messages of desktop thread */
    while (GetMessage(&Msg, HWND_DESKTOP)) {
        LRESULT lRet = 0;

#ifdef _MGHAVE_TRACE_MSG
        if (Msg.message != MSG_TIMEOUT && Msg.message != MSG_TIMER) {
            fprintf (stderr, "Message %u (%s): hWnd: HWND_DESKTOP, wP: %p, lP: %p, tick: %u; %s\n",
                Msg.message, Message2Str (Msg.message),
                (PVOID)Msg.wParam, (PVOID)Msg.lParam, (UINT)Msg.time,
                Msg.pAdd?"Sync":"Normal");
        }
#endif

        lRet = DesktopWinProc (HWND_DESKTOP,
                        Msg.message, Msg.wParam, Msg.lParam);

        if (Msg.pAdd) /* this is a sync message. */
        {
            PSYNCMSG pSyncMsg = (PSYNCMSG)(Msg.pAdd);
            pSyncMsg->retval = lRet;
            if(pSyncMsg->sem_handle)
                sem_post(pSyncMsg->sem_handle);
        }

#ifdef _MGHAVE_TRACE_MSG
        if (Msg.message != MSG_TIMEOUT && Msg.message != MSG_TIMER) {
            fprintf (stderr, "Message %u (%s) done, return value: %p\n",
                Msg.message, Message2Str (Msg.message), (PVOID)lRet);
        }
#endif
    }

    mg_FreeMsgQueueForThisThread ();
    __mg_dsk_msg_queue = NULL;

    __mg_quiting_stage = _MG_QUITING_STAGE_EVENT;

    return NULL;
}

void mg_TerminateDesktop (void)
{
    pthread_mutex_destroy(&sg_ScrGCRInfo.lock);

    __kernel_free_z_order_info (__mg_zorder_info);
    __mg_zorder_info = NULL;
    DestroyFreeClipRectList (&sg_FreeClipRectList);
    DestroyFreeClipRectList (&sg_FreeInvRectList);

    // Since 5.0.0
    __mg_free_hook_wins (0);

    mg_TerminateSystemRes ();
    // dongjunjie avoid double free
    __mg_dsk_win = 0;
}

#endif /* _MGRM_THREADS */

