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
#include "event.h"

/******************************* global data *********************************/
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
    ret = __kernel_alloc_z_order_info (DEF_NR_TOPMOSTS, DEF_NR_NORMALS,
            TRUE);
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
        _ERR_PRINTF ("KERNEL>Desktop: failed to init window manager!\n");
        return FALSE;
    }

    return TRUE;
}

#include "debug.h"

static IDLEHANDLER std_idle_handler;

static BOOL idle_handler_for_desktop_thread (MSGQUEUE *msg_queue, BOOL wait)
{
    __mg_update_tick_count (msg_queue);
    return std_idle_handler (msg_queue, wait);
}

void* __kernel_desktop_main (void* data)
{
    MSG Msg;

    /* init message queue of desktop thread */
    if (!(__mg_dsk_msg_queue = mg_AllocMsgQueueForThisThread (TRUE)) ) {
        _ERR_PRINTF ("failed to allocate message queue\n");
        sem_post ((sem_t*)data);
        return NULL;
    }

    /* For bug reported in Issue #116, under threads mode, the idle handler
       for the desktop thread should call __mg_update_tick_count () */
    std_idle_handler = __mg_dsk_msg_queue->OnIdle;
    __mg_dsk_msg_queue->OnIdle = idle_handler_for_desktop_thread;

    /* init desktop window */
    init_desktop_win ();

    DesktopWinProc (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    /* desktop thread is ready now */
    sem_post ((sem_t*)data);

    /* process messages of desktop thread */
    while (GetMessage (&Msg, HWND_DESKTOP)) {
        LRESULT lRet = 0;

#ifdef _MGHAVE_TRACE_MSG
        if (Msg.message != MSG_TIMEOUT && Msg.message != MSG_TIMER) {
            dump_message (&Msg, __func__);
        }
#endif

        lRet = DesktopWinProc (HWND_DESKTOP,
                        Msg.message, Msg.wParam, Msg.lParam);

        if (Msg.pSyncMsg) {
            /* this is a sync message. */
            PSYNCMSG pSyncMsg = (PSYNCMSG)(Msg.pSyncMsg);
            pSyncMsg->retval = lRet;
            if (pSyncMsg->sem_handle)
                sem_post(pSyncMsg->sem_handle);
        }

#ifdef _MGHAVE_TRACE_MSG
        if (Msg.message != MSG_TIMEOUT && Msg.message != MSG_TIMER) {
            dump_message_with_retval (&Msg, lRet, __func__);
        }
#endif
    }

    mg_FreeMsgQueueForThisThread (TRUE);
    __mg_dsk_msg_queue = NULL;
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

    // mg_TerminateSystemRes ();
    // dongjunjie avoid double free
    __mg_dsk_win = 0;
}

#endif /* _MGRM_THREADS */

