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
** message.c: The messaging module.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "blockheap.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "timer.h"
#include "misc.h"
#include "debug.h"

#ifdef HAVE_SELECT
#include "mgsock.h"
#endif

#ifdef _MGRM_PROCESSES
#include "sharedres.h"
#endif

#include "license.h"
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/02_feiman.dat.c"
#endif

/******************************************************************************/

#if 0   /* deprecated code */
/* since 5.0.0, we use post_quit_to_all_message_threads instead */
#ifdef _MGHAVE_VIRTUAL_WINDOW

  #define SET_PADD(value) pMsg->pSyncMsg = value
  #define SYNMSGNAME pMsg->pSyncMsg?"Sync":"Normal"

  /* XXX: Need desktop thread keep alive to wakeup sem_wait()
     by delivering messages */
  #define TEST_NEED_TO_QUIT(queue)                      \
    ((__mg_quiting_stage <= _MG_QUITING_STAGE_FORCE) && \
     ((queue)!=__mg_dsk_msg_queue))
#else /* defined _MGHAVE_VIRTUAL_WINDOW */

  #define SET_PADD(value)
  #define SYNMSGNAME "Normal"
  #define TEST_NEED_TO_QUIT(queue)                     \
    (__mg_quiting_stage <= _MG_QUITING_STAGE_FORCE)

#endif /* not defined _MGHAVE_VIRTUAL_WINDOW */

#define TEST_IF_QUIT(queue, hWnd)                       \
    do {                                                \
        if (TEST_NEED_TO_QUIT(queue)) {                 \
            LOCK_MSGQ (queue);                          \
            if (!(queue->dwState & QS_QUIT)){           \
                queue->loop_depth++;                    \
                queue->dwState |= QS_QUIT;              \
                UNLOCK_MSGQ (queue);                    \
                if (IsDialog(hWnd)) {                   \
                    EndDialog (hWnd, IDCANCEL);         \
                }                                       \
                else {                                  \
                    DestroyMainWindow (hWnd);           \
                }                                       \
            } else {                                    \
                UNLOCK_MSGQ (queue);                    \
            }                                           \
        }                                               \
    } while (0)
#endif  /* deprecated code */

#ifdef _MGHAVE_VIRTUAL_WINDOW
#   define SET_PADD(value) pMsg->pSyncMsg = value
#else
#   define SET_PADD(value)
#endif

#define TEST_IF_QUIT(queue, hWnd)

#define ALLOCQMSG()         (PQMSG)BlockDataAlloc(&QMSGHeap)
#define FREEQMSG(pqmsg)     BlockDataFree(&QMSGHeap, pqmsg)
#define GETWNDPROC(hWnd)    (((PMAINWIN)hWnd)->MainWindowProc)
#define GETNOTIFPROC(hWnd)  (((PMAINWIN)hWnd)->NotifProc)

/****************************** Message Allocation ****************************/
static BLOCKHEAP QMSGHeap;

/* QMSG allocation */
BOOL mg_InitFreeQMSGList (void)
{
    InitBlockDataHeap (&QMSGHeap, sizeof (QMSG), SIZE_QMSG_HEAP);
    return TRUE;
}

void mg_DestroyFreeQMSGList (void)
{
    DestroyBlockDataHeap (&QMSGHeap);
}

/****************************** Message Queue Management ************************/
#ifdef _MGHAVE_VIRTUAL_WINDOW
pthread_key_t __mg_threadinfo_key;
#endif

MSGQUEUE* mg_AllocMsgQueueForThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    if (!(pMsgQueue = malloc (sizeof(MSGQUEUE)))) {
        return NULL;
    }

    if (!mg_InitMsgQueue (pMsgQueue, 0)) {
        free (pMsgQueue);
        return NULL;
    }

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pMsgQueue->th = pthread_self();
    pthread_setspecific (__mg_threadinfo_key, pMsgQueue);
#endif

    // register this new message queue
    SendMessage (HWND_DESKTOP, MSG_MANAGE_MSGTHREAD,
            MSGTHREAD_SIGNIN, (LPARAM)pMsgQueue);
    return pMsgQueue;
}

void mg_FreeMsgQueueForThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = getMsgQueueForThisThread ();

    if (pMsgQueue) {
        if (pMsgQueue->nrWindows > 0) {
            _WRN_PRINTF ("there are still some windows not destroyed\n");
        }

        mg_DestroyMsgQueue (pMsgQueue);
        free (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
#ifdef __VXWORKS__
        pthread_setspecific (__mg_threadinfo_key, (void*)-1);
#else
        pthread_setspecific (__mg_threadinfo_key, NULL);
#endif
#endif /* defined _MGHAVE_VIRTUAL_WINDOW */
    }
    else {
        _WRN_PRINTF ("message queue for this thread has gone\n");
    }
}

static int handle_idle_message (MSGQUEUE* msg_queue)
{
    int n = 0;

    msg_queue->idle_counter++;
    if (msg_queue->idle_counter >= MAX_IDLE_COUNTER) {
        // it is time to post MSG_IDLE to all windows in this thread.
        MSG msg = { HWND_NULL, MSG_IDLE, __mg_tick_counter, 0 };
        n = __mg_broadcast_message (msg_queue, &msg);
        msg_queue->idle_counter = 0;
    }

    return n;
}

#ifdef HAVE_SELECT
/* the idle handler for a message queue with checking listening fds */
static BOOL std_idle_handler (MSGQUEUE* msg_queue, BOOL wait)
{
    int retval, n = 0;
    fd_set rset, wset, eset;
    fd_set* rsetptr = NULL;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;
    struct timeval sel_timeout;

    if (wait) {
        sel_timeout.tv_sec = 0;
        sel_timeout.tv_usec = USEC_10MS;
    }
    else {
        sel_timeout.tv_sec = 0;
        sel_timeout.tv_usec = 0;
    }

    /* a fdset got modified each time around */
    if (msg_queue->nr_rfds) {
        rset = msg_queue->rfdset;
        rsetptr = &rset;
    }
    if (msg_queue->nr_wfds) {
        wset = msg_queue->wfdset;
        wsetptr = &wset;
    }
    if (msg_queue->nr_efds) {
        eset = msg_queue->efdset;
        esetptr = &eset;
    }

    if ((retval = select (msg_queue->maxfd + 1,
            rsetptr, wsetptr, esetptr, &sel_timeout)) < 0) {
        /* no event. */
        if (errno != EINTR) {
            _WRN_PRINTF ("unexpected error of select(): %m\n");
        }
        return FALSE;
    }

    n += __mg_check_expired_timers (msg_queue,
            __mg_tick_counter - msg_queue->old_tick_count);
    msg_queue->old_tick_count = __mg_tick_counter;

    if (rsetptr || wsetptr || esetptr) {
        n += __mg_kernel_check_listen_fds (msg_queue, rsetptr, wsetptr, esetptr);
    }

    return n > 0;
}

#else   /* defined HAVE_SELECT */

static BOOL std_idle_handler (MSGQUEUE* msg_queue, BOOL wait)
{
    int n = 0;
    long timeout_ms;

    if (wait) {
        timeout_ms = 10;
    }
    else {
        timeout_ms = 0;
    }

    if (timeout_ms > 0)
        __mg_os_time_delay (timeout_ms);

    n = __mg_check_expired_timers (msg_queue,
            __mg_tick_counter - msg_queue->old_tick_count);
    msg_queue->old_tick_count = __mg_tick_counter;
    return n > 0;
}

#endif  /* not defined HAVE_SELECT */

BOOL mg_InitMsgQueue (PMSGQUEUE pMsgQueue, int iBufferLen)
{
    memset (pMsgQueue, 0, sizeof(MSGQUEUE));

    pMsgQueue->dwState = QS_EMPTY;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* since 5.0.0, we use recursive lock */
    {
        int ret;
        pthread_mutexattr_t my_attr;

        ret = pthread_mutexattr_init (&my_attr);
        if (ret) return FALSE;
        ret = pthread_mutexattr_settype (&my_attr, PTHREAD_MUTEX_RECURSIVE);
        if (ret) return FALSE;
        ret = pthread_mutex_init (&pMsgQueue->lock, &my_attr);
        if (ret) return FALSE;
        ret = pthread_mutexattr_destroy (&my_attr);
        if (ret) return FALSE;
        ret = sem_init (&pMsgQueue->wait, 0, 0);
        if (ret) return FALSE;
        //sem_init (&pMsgQueue->sync_msg, 0, 0);
    }
#endif

    if (iBufferLen <= 0)
        iBufferLen = DEF_MSGQUEUE_LEN;

    pMsgQueue->msg = malloc (sizeof (MSG) * iBufferLen);

    if (!pMsgQueue->msg) {
#ifdef _MGHAVE_VIRTUAL_WINDOW
        pthread_mutex_destroy (&pMsgQueue->lock);
        sem_destroy (&pMsgQueue->wait);
        //sem_destroy (&pMsgQueue->sync_msg);
        return FALSE;
#endif
    }

    pMsgQueue->len = iBufferLen;

    pMsgQueue->OnIdle = std_idle_handler;

    /* Since 5.0.0, MiniGUI provides support for timers per message thread */
    // pMsgQueue->first_timer_slot = 0;
    // pMsgQueue->expired_timer_mask = 0;
    // memset (pMsgQueue->timer_slots, 0, sizeof (pMsgQueue->timer_slots));
    pMsgQueue->old_tick_count = __mg_tick_counter;

#ifdef HAVE_SELECT
    /* Since 5.0.0, MiniGUI supports listening file descriptors
       per message thread */
    // pMsgQueue->nr_fd_slots = 0;
    // pMsgQueue->maxfd = 0;
    // pMsgQueue->nr_rfds = 0;
    // pMsgQueue->nr_wfds = 0;
    // pMsgQueue->nr_efds = 0;
    // pMsgQueue->fd_slots = NULL;
    mg_fd_zero (&pMsgQueue->rfdset);
    mg_fd_zero (&pMsgQueue->wfdset);
    mg_fd_zero (&pMsgQueue->efdset);
#endif

    return TRUE;
}

void mg_DestroyMsgQueue (PMSGQUEUE pMsgQueue)
{
    PQMSG head;
    PQMSG next;

    if (pMsgQueue->nrWindows > 0) {
        _WRN_PRINTF ("There are still some windows not destroyed.\n");
    }

    head = next = pMsgQueue->pFirstNotifyMsg;
    while (head) {
        next = head->next;

        FREEQMSG (head);
        head = next;
    }

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_destroy (&pMsgQueue->lock);
    sem_destroy (&pMsgQueue->wait);
    //sem_destroy (&pMsgQueue->sync_msg);
#endif

    __mg_remove_timers_by_msg_queue (pMsgQueue);

    if (pMsgQueue->msg)
        free (pMsgQueue->msg);
    pMsgQueue->msg = NULL;
}

/* post a message to a message queue */
BOOL kernel_QueueMessage (PMSGQUEUE msg_que, PMSG msg)
{
    if (msg_que == NULL || msg == NULL)
        return FALSE;

    LOCK_MSGQ(msg_que);

    msg->time = __mg_tick_counter;

    /* check for the duplicated messages */
    if (msg->message == MSG_MOUSEMOVE
            || msg->message == MSG_NCMOUSEMOVE
            || msg->message == MSG_DT_MOUSEMOVE) {
        int readpos = msg_que->readpos;
        PMSG a_msg, last_msg = NULL;

        while (readpos != msg_que->writepos) {
            a_msg = msg_que->msg + readpos;

            if (a_msg->message == msg->message
                        && a_msg->wParam == msg->wParam
                        && a_msg->hwnd == msg->hwnd) {
                last_msg = a_msg;
            }

            readpos ++;
            readpos %= msg_que->len;
        }

        if (last_msg) {
            last_msg->lParam = msg->lParam;
            last_msg->time = msg->time;
            goto ret;
        }
    }
    else if (msg->message == MSG_TIMEOUT
                || msg->message == MSG_IDLE
                || msg->message == MSG_CARETBLINK) {
        int readpos = msg_que->readpos;
        PMSG a_msg;

        while (readpos != msg_que->writepos) {
            a_msg = msg_que->msg + readpos;

            if (a_msg->message == msg->message
                    && a_msg->hwnd == msg->hwnd) {
                a_msg->wParam = msg->wParam;
                a_msg->lParam = msg->lParam;
                a_msg->time = msg->time;
                goto ret;
            }

            readpos ++;
            readpos %= msg_que->len;
        }
    }

    if ((msg_que->writepos + 1) % msg_que->len == msg_que->readpos) {
        // message queue is full.
        UNLOCK_MSGQ(msg_que);
        return FALSE;
    }

    /* Write the data and advance write pointer */
    msg_que->msg [msg_que->writepos] = *msg;
    msg_que->writepos++;
    msg_que->writepos %= msg_que->len;

ret:
    msg_que->dwState |= QS_POSTMSG;

    UNLOCK_MSGQ (msg_que);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread (msg->hwnd))
        POST_MSGQ (msg_que);
#endif

    return TRUE;
}

/******************************************************************************/

HWND kernel_CheckInvalidRegion (PMAINWIN pWin)
{
    PCONTROL pCtrl = (PCONTROL)pWin;
    HWND hwnd;

    /* Since 5.0.0: skip virtual window */
    if (pWin->WinType == TYPE_VIRTWIN)
        return HWND_NULL;

    if (pCtrl->InvRgn.rgn.head)
        return (HWND)pCtrl;

    pCtrl = pCtrl->children;
    while (pCtrl) {

        if ((hwnd = kernel_CheckInvalidRegion ((PMAINWIN) pCtrl)))
            return hwnd;

        pCtrl = pCtrl->next;
    }

    return HWND_NULL;
}

static HWND msgCheckHostedTree (PMAINWIN pHosting)
{
    HWND hNeedPaint;
    PMAINWIN pHosted;

    /* Since 5.0.0: always skip desktop */
    if (pHosting != __mg_dsk_win &&
            (hNeedPaint = kernel_CheckInvalidRegion (pHosting)))
        return hNeedPaint;

    pHosted = pHosting->pFirstHosted;
    while (pHosted) {
        if ((hNeedPaint = msgCheckHostedTree (pHosted)))
            return hNeedPaint;

        pHosted = pHosted->pNextHosted;
    }

    return 0;
}

BOOL GUIAPI HavePendingMessageEx (HWND hWnd, BOOL bNoDeskTimer)
{
    PMSGQUEUE pMsgQueue;

    if (hWnd == HWND_NULL) {
#ifdef _MGHAVE_VIRTUAL_WINDOW
        if (!(pMsgQueue = getMsgQueueForThisThread ()))
            return FALSE;
#else
        pMsgQueue = __mg_dsk_msg_queue;
#endif
    }
    else {
        if (!(pMsgQueue = getMsgQueueIfWindowInThisThread (hWnd)))
            return FALSE;
    }

    TEST_IF_QUIT(pMsgQueue, hWnd);

    LOCK_MSGQ (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (pMsgQueue->dwState & QS_SYNCMSG) {
        if (pMsgQueue->pFirstSyncMsg) goto retok;
    }
#endif

    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
        if (pMsgQueue->pFirstNotifyMsg) goto retok;
    }

    if (pMsgQueue->dwState & QS_POSTMSG) {
        if (pMsgQueue->readpos != pMsgQueue->writepos) goto retok;
    }

    if (pMsgQueue->dwState & (QS_QUIT | QS_PAINT)) {
        goto retok;
    }

    if (pMsgQueue->expired_timer_mask)
        goto retok;

    /*
     * We do not need to check QS_DESKTIMER, because it is for the
     * desktop window, and user don't care it.
     */
    if (!bNoDeskTimer && pMsgQueue == __mg_dsk_msg_queue &&
            (pMsgQueue->dwState & QS_DESKTIMER)) {
        goto retok;
    }

    UNLOCK_MSGQ (pMsgQueue);

    /* Since 5.0.0, always call OnIdle with no wait */
    if (pMsgQueue->OnIdle)
        return pMsgQueue->OnIdle (pMsgQueue, FALSE);
    return FALSE;

retok:
    UNLOCK_MSGQ (pMsgQueue);
    return TRUE;
}

BOOL GUIAPI HavePendingMessage (HWND hWnd)
{
    return HavePendingMessageEx (hWnd, FALSE);
}

int GUIAPI BroadcastMessage (UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    MSG msg;

    msg.message = nMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;

    return SendMessage (HWND_DESKTOP, MSG_BROADCASTMSG, 0, (LPARAM)(&msg));
}

static void travel_all_hosted (PMAINWIN pWin,
    void (*cb) (PMAINWIN, void*), void* data)
{
    PMAINWIN pNext = pWin->pFirstHosted;

    while (pNext) {

        travel_all_hosted (pNext, cb, data);
        cb (pNext, data);

        pNext = pNext->pNextHosted;
    }
}

static void _my_travel_cb (PMAINWIN win, void* data)
{
    MSG* msg = (MSG*)data;

    msg->hwnd = (HWND)win;
    if (kernel_QueueMessage (win->pMsgQueue, msg)) {
        msg->time++;
    }
}

int __mg_broadcast_message (PMSGQUEUE msg_queue, MSG* msg)
{
    PMAINWIN win;

    msg->time = 0;

    if ((win = msg_queue->pRootMainWin)) {
        travel_all_hosted (win, _my_travel_cb, msg);
        _my_travel_cb (win, msg);
    }

    return (int)msg->time;
}

#ifdef _MGHAVE_VIRTUAL_WINDOW
int GUIAPI BroadcastMessageInThisThread (UINT nMsg,
        WPARAM wParam, LPARAM lParam)
{
    MSGQUEUE* pMsgQueue;
    MSG msg = { HWND_NULL, nMsg, wParam, lParam };

    pMsgQueue = (MSGQUEUE*)pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue == (void *)-1) {
        pMsgQueue = NULL;
    }
#endif

    if (pMsgQueue == NULL)
        return -1;

    return __mg_broadcast_message (pMsgQueue, &msg);
}

#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

#ifdef _MGHAVE_MSG_STRING
#include "msgstr.h"

const char* GUIAPI Message2Str (UINT message)
{
    if (message >= 0x0000 && message <= 0x006F)
        return __mg_msgstr1 [message];
    else if (message >= 0x0070 && message <= 0x009F)
        return __mg_msgstr11 [message - 0x0070];
    else if (message >= 0x00A0 && message <= 0x010F)
        return __mg_msgstr2 [message - 0x00A0];
    else if (message >= 0x0120 && message <= 0x017F)
        return __mg_msgstr3 [message - 0x0120];
    else if (message >= 0xF000)
        return "Control Messages";
    else
        return "MSG_USER";
}

void GUIAPI PrintMessage (FILE* fp, HWND hWnd,
                UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    fprintf (fp, "Message %s: hWnd: %p, wP: %p, lP: %p.\n",
             Message2Str (nMsg), hWnd, (PVOID)wParam, (PVOID)lParam);
}
#endif /* defined _MGHAVE_MSG_STRING */

static inline void CheckCapturedMouseMessage (PMSG pMsg)
{
    if (__mg_captured_wnd == pMsg->hwnd
            && pMsg->message >= MSG_FIRSTMOUSEMSG
            && pMsg->message <= MSG_LASTMOUSEMSG
            && !(pMsg->wParam & KS_CAPTURED)) {
        int x, y;
        x = LOSWORD (pMsg->lParam);
        y = HISWORD (pMsg->lParam);

        ClientToScreen (pMsg->hwnd, &x, &y);

        pMsg->lParam = MAKELONG (x, y);
        pMsg->wParam |= KS_CAPTURED;
    }
}

#define IS_MSG_WANTED(message) \
        ((nMsgFilterMin <= 0 && nMsgFilterMax <= 0) || \
          (nMsgFilterMin > 0 && nMsgFilterMax >= nMsgFilterMin && \
           message >= nMsgFilterMin && message <= nMsgFilterMax))

BOOL PeekMessageEx (PMSG pMsg, HWND hWnd,
        UINT nMsgFilterMin, UINT nMsgFilterMax, BOOL bWait, UINT uRemoveMsg)
{
    PMSGQUEUE pMsgQueue;
    PQMSG phead;

    if (MG_UNLIKELY (pMsg == NULL ||
                (hWnd != HWND_DESKTOP && !MG_IS_MAIN_VIRT_WINDOW (hWnd))))
        return FALSE;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!(pMsgQueue = getMsgQueueForThisThread ())) {
        _WRN_PRINTF ("not a message thread.\n");
        return FALSE;
    }
#else
    pMsgQueue = __mg_dsk_msg_queue;
#endif

    memset (pMsg, 0, sizeof(MSG));
    pMsg->time = __mg_tick_counter;

checkagain:

    TEST_CANCEL;

    TEST_IF_QUIT(pMsgQueue, hWnd);

    LOCK_MSGQ (pMsgQueue);

    if ((pMsgQueue->dwState & QS_QUIT)) {
        pMsg->hwnd = hWnd;
        pMsg->message = MSG_QUIT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;
        SET_PADD (NULL);

        if (uRemoveMsg == PM_REMOVE) {
            pMsgQueue->loop_depth --;
            if (pMsgQueue->loop_depth == 0) {
                pMsgQueue->dwState &= ~QS_QUIT;
            }
        }

        UNLOCK_MSGQ (pMsgQueue);
        return FALSE;
    }

    /* Dealing with sync messages before notify messages is better ? */
#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (pMsgQueue->dwState & QS_SYNCMSG) {
        if (pMsgQueue->pFirstSyncMsg) {
            *pMsg = pMsgQueue->pFirstSyncMsg->Msg;
            SET_PADD (pMsgQueue->pFirstSyncMsg);
            if (IS_MSG_WANTED(pMsg->message)) {
                if (uRemoveMsg == PM_REMOVE) {
                    pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;
                }

                UNLOCK_MSGQ (pMsgQueue);
                goto got_ret;
            }
        }
        else
            pMsgQueue->dwState &= ~QS_SYNCMSG;
    }
#endif

    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
        if (pMsgQueue->pFirstNotifyMsg) {
            phead = pMsgQueue->pFirstNotifyMsg;
            *pMsg = phead->Msg;
            SET_PADD (NULL);

            if (IS_MSG_WANTED(pMsg->message)) {
                if (uRemoveMsg == PM_REMOVE) {
                    pMsgQueue->pFirstNotifyMsg = phead->next;
                    if (pMsgQueue->pFirstNotifyMsg == NULL)
                        pMsgQueue->pLastNotifyMsg = NULL;
                    FREEQMSG (phead);
                }

                UNLOCK_MSGQ (pMsgQueue);
                goto got_ret;
            }
        }
        else
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
    }

    if (pMsgQueue->dwState & QS_POSTMSG) {
        if (pMsgQueue->readpos != pMsgQueue->writepos) {
            *pMsg = pMsgQueue->msg[pMsgQueue->readpos];
            SET_PADD (NULL);
            if (IS_MSG_WANTED(pMsg->message)) {
                CheckCapturedMouseMessage (pMsg);
                if (uRemoveMsg == PM_REMOVE) {
                    pMsgQueue->readpos++;
                    pMsgQueue->readpos %= pMsgQueue->len;
                }

                UNLOCK_MSGQ (pMsgQueue);
                goto got_ret;
            }
        }
        else
            pMsgQueue->dwState &= ~QS_POSTMSG;
    }

    /*
     * check invalidate region of the windows
     */
    if (pMsgQueue->dwState & QS_PAINT && IS_MSG_WANTED(MSG_PAINT)) {
        PMAINWIN pHostingRoot;
        HWND hNeedPaint;
        PMAINWIN pWin;

        pMsg->message = MSG_PAINT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;
        SET_PADD (NULL);

        pHostingRoot = pMsgQueue->pRootMainWin;

        if ((hNeedPaint = msgCheckHostedTree (pHostingRoot))) {
            pMsg->hwnd = hNeedPaint;

            //dump_window (hNeedPaint, "repainting");

            pWin = (PMAINWIN) hNeedPaint;
            pMsg->lParam = (LPARAM)(&pWin->InvRgn.rgn);
            UNLOCK_MSGQ (pMsgQueue);
            goto got_ret;
        }

        /* no paint message */
        pMsgQueue->dwState &= ~QS_PAINT;
    }

    /* handle desktop timer here */
    if (pMsgQueue == __mg_dsk_msg_queue && pMsgQueue->dwState & QS_DESKTIMER) {
        pMsg->hwnd = HWND_DESKTOP;
        pMsg->message = MSG_TIMER;
        pMsg->wParam = 0;
        pMsg->lParam = 0;

        if (uRemoveMsg == PM_REMOVE) {
            pMsgQueue->dwState &= ~QS_DESKTIMER;
        }

        UNLOCK_MSGQ (pMsgQueue);
        goto got_ret;
    }

    /* handle general timer here */
    if (pMsgQueue->expired_timer_mask && IS_MSG_WANTED(MSG_TIMER)) {
        int slot;
        TIMER* timer;

        /* get the first expired timer slot */
        slot = pMsgQueue->first_timer_slot;
        do {
            if (pMsgQueue->expired_timer_mask & (0x01UL << slot))
                break;

            slot ++;
            slot %= DEF_NR_TIMERS;
            if (slot == pMsgQueue->first_timer_slot) {
                slot = -1;
                break;
            }
        } while (TRUE);

        pMsgQueue->first_timer_slot ++;
        pMsgQueue->first_timer_slot %= DEF_NR_TIMERS;

        if ((timer = pMsgQueue->timer_slots[slot])) {
            pMsgQueue->expired_timer_mask &= ~(0x01UL << slot);

            if (timer->proc) {
                BOOL ret_timer_proc;

                /* unlock the message queue when calling timer proc */
                UNLOCK_MSGQ (pMsgQueue);

                /* calling the timer callback procedure */
                ret_timer_proc = timer->proc (timer->hWnd,
                        timer->id, timer->ticks_fired);

                /* lock the message queue again */
                LOCK_MSGQ (pMsgQueue);

                if (!ret_timer_proc) {
                    /* remove the timer */
                    __mg_remove_timer (pMsgQueue, slot);
                }
                UNLOCK_MSGQ (pMsgQueue);
                goto checkagain;
            }
            else {
                pMsg->message = MSG_TIMER;
                pMsg->hwnd = timer->hWnd;
                pMsg->wParam = timer->id;
                pMsg->lParam = timer->ticks_fired;
                SET_PADD (NULL);

                UNLOCK_MSGQ (pMsgQueue);
                goto got_ret;
            }
        }
    }

    UNLOCK_MSGQ (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (bWait) {
        /* no message, wait again. */
        if (pMsgQueue->OnIdle) {
            if (sem_trywait (&pMsgQueue->wait) == 0)
                goto checkagain;

            if (!pMsgQueue->OnIdle (pMsgQueue, TRUE)) {
                handle_idle_message (pMsgQueue);
            }
        }
        else {
            sem_wait (&pMsgQueue->wait);
        }

        goto checkagain;
    }
#else   /* defined _MGHAVE_VIRTUAL_WINDOW */
    /* no message, idle */
    if (bWait) {
        assert (pMsgQueue->OnIdle);
        if (!pMsgQueue->OnIdle (pMsgQueue, TRUE)) {
            handle_idle_message (pMsgQueue);
        }
        goto checkagain;
    }
#endif  /* not defined _MGHAVE_VIRTUAL_WINDOW */

    /* no message */
    return FALSE;

got_ret:
    if (pMsg->hwnd != HWND_DESKTOP) {
        /* since 5.0.0, reset idle counter */
        pMsgQueue->idle_counter = 0;
    }

    return TRUE;
}

#if 0   /* moved code */
/* The following two functions are moved to window.h as inline functions. */
int GUIAPI GetMessage (PMSG pMsg, HWND hWnd)
{
    return PeekMessageEx (pMsg, hWnd, 0, 0, TRUE, PM_REMOVE);
}

BOOL GUIAPI PeekMessage (PMSG pMsg, HWND hWnd, UINT nMsgFilterMin,
                         UINT nMsgFilterMax, UINT uRemoveMsg)
{
    return PeekMessageEx (pMsg, hWnd, nMsgFilterMin, nMsgFilterMax,
                           FALSE, uRemoveMsg);
}
#endif  /* moved code */

/* wait for message */
BOOL GUIAPI WaitMessage (PMSG pMsg, HWND hWnd)
{
    PMSGQUEUE pMsgQueue;

    if (!pMsg)
        return FALSE;

    if (!(pMsgQueue = getMsgQueueIfWindowInThisThread(hWnd)))
        return FALSE;

    pMsgQueue->idle_counter = 0;

    memset (pMsg, 0, sizeof(MSG));

checkagain:

    TEST_CANCEL;

    TEST_IF_QUIT(pMsgQueue, hWnd);

    LOCK_MSGQ (pMsgQueue);

    if ((pMsgQueue->dwState & QS_QUIT)) {
        goto getit;
    }

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (pMsgQueue->dwState & QS_SYNCMSG) {
        if (pMsgQueue->pFirstSyncMsg) {
            goto getit;
        }
    }
#endif

    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
        if (pMsgQueue->pFirstNotifyMsg) {
            goto getit;
        }
    }

    if (pMsgQueue->dwState & QS_POSTMSG) {
        if (pMsgQueue->readpos != pMsgQueue->writepos) {
            goto getit;
        }
    }

    if (pMsgQueue->dwState & QS_PAINT) {
        goto getit;
    }

    if (pMsgQueue == __mg_dsk_msg_queue && pMsgQueue->dwState & QS_DESKTIMER) {
        goto getit;
    }

    if (pMsgQueue->expired_timer_mask) {
        goto getit;
    }

    UNLOCK_MSGQ (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* no message, try wait, if no message call idle. */
    if (pMsgQueue->OnIdle) {
        if (sem_trywait (&pMsgQueue->wait) == 0)
            goto checkagain;

        if (!pMsgQueue->OnIdle (pMsgQueue, TRUE)) {
            handle_idle_message (pMsgQueue);
        }
    }
    else {
        sem_wait (&pMsgQueue->wait);
    }
#else   /* defined _MGHAVE_VIRTUAL_WINDOW */
    /* no message, idle */
    assert (pMsgQueue->OnIdle);
    if (!pMsgQueue->OnIdle (pMsgQueue, TRUE)) {
        handle_idle_message (pMsgQueue);
    }
#endif  /* not defined _MGHAVE_VIRTUAL_WINDOW */

    goto checkagain;

getit:
    UNLOCK_MSGQ (pMsgQueue);
    return TRUE;
}

BOOL GUIAPI PeekPostMessage (PMSG pMsg, HWND hWnd, UINT nMsgFilterMin,
                        UINT nMsgFilterMax, UINT uRemoveMsg)
{
    PMSGQUEUE pMsgQueue;
    PMSG pPostMsg;

    if (!pMsg)
        return FALSE;

    if (!(pMsgQueue = getMsgQueueIfWindowInThisThread(hWnd)))
        return FALSE;

    LOCK_MSGQ (pMsgQueue);
    memset (pMsg, 0, sizeof(MSG));

    if (pMsgQueue->dwState & QS_POSTMSG) {

        if (pMsgQueue->readpos != pMsgQueue->writepos) {

            pPostMsg = pMsgQueue->msg + pMsgQueue->readpos;
            if (nMsgFilterMin == 0 && nMsgFilterMax == 0)
                *pMsg = *pPostMsg;
            else if (pPostMsg->message <= nMsgFilterMax &&
                    pPostMsg->message >= nMsgFilterMin)
                *pMsg = *pPostMsg;
            else {
                UNLOCK_MSGQ (pMsgQueue);
                return FALSE;
            }

            SET_PADD (NULL);

            if (uRemoveMsg == PM_REMOVE) {
                pMsgQueue->readpos++;
                pMsgQueue->readpos %= pMsgQueue->len;
            }

            UNLOCK_MSGQ (pMsgQueue);
            return TRUE;
        }

    }

    UNLOCK_MSGQ (pMsgQueue);
    return FALSE;
}

LRESULT GUIAPI SendMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC WndProc;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), ERR_INV_HWND);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread (hWnd))
        return SendSyncMessage (hWnd, nMsg, wParam, lParam);
#endif

    if (!(WndProc = GETWNDPROC(hWnd)))
        return ERR_INV_HWND;

    return (*WndProc)(hWnd, nMsg, wParam, lParam);

}

/* Since 5.0.0, delcare as a GUIAPI */
/* houhh 20090619, send a prior notify message. */
int GUIAPI SendPriorNotifyMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    PQMSG pqmsg;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), ERR_INV_HWND);

    if (!(pMsgQueue = getMsgQueue(hWnd)))
        return ERR_INV_HWND;

    pqmsg = ALLOCQMSG();

    LOCK_MSGQ (pMsgQueue);

    /* queue the notification message. */
    pqmsg->Msg.hwnd = hWnd;
    pqmsg->Msg.message = nMsg;
    pqmsg->Msg.wParam = wParam;
    pqmsg->Msg.lParam = lParam;
    pqmsg->Msg.time = __mg_tick_counter;
    pqmsg->next = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL) {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else {
        pqmsg->next = pMsgQueue->pFirstNotifyMsg;
        pMsgQueue->pFirstNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;

    UNLOCK_MSGQ (pMsgQueue);
#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread(hWnd))
        POST_MSGQ(pMsgQueue);
#endif

    return ERR_OK;
}

int GUIAPI SendNotifyMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    PQMSG pqmsg;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), ERR_INV_HWND);

    if (!(pMsgQueue = getMsgQueue(hWnd)))
        return ERR_INV_HWND;

    pqmsg = ALLOCQMSG();

    LOCK_MSGQ (pMsgQueue);

    /* queue the notification message. */
    pqmsg->Msg.hwnd = hWnd;
    pqmsg->Msg.message = nMsg;
    pqmsg->Msg.wParam = wParam;
    pqmsg->Msg.lParam = lParam;
    pqmsg->Msg.time = __mg_tick_counter;
    pqmsg->next = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL) {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else {
        assert (pMsgQueue->pLastNotifyMsg);
        pMsgQueue->pLastNotifyMsg->next = pqmsg;
        pMsgQueue->pLastNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;

    UNLOCK_MSGQ (pMsgQueue);
#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread(hWnd))
        POST_MSGQ(pMsgQueue);
#endif

    return ERR_OK;
}

int GUIAPI NotifyWindow (HWND hWnd, LINT id, int code, DWORD dwAddData)
{
    PMSGQUEUE pMsgQueue;
    PQMSG pqmsg;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), ERR_INV_HWND);

    if (!(pMsgQueue = getMsgQueue(hWnd)))
        return ERR_INV_HWND;

    pqmsg = ALLOCQMSG();

    LOCK_MSGQ (pMsgQueue);

    /* queue the notification message. */
    pqmsg->Msg.hwnd = hWnd;
    pqmsg->Msg.message = MSG_NOTIFICATION;
    pqmsg->Msg.wParam = (WPARAM)id;
    pqmsg->Msg.lParam = (LPARAM)code;
    pqmsg->Msg.time = dwAddData;
    pqmsg->next = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL) {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else {
        assert (pMsgQueue->pLastNotifyMsg);
        pMsgQueue->pLastNotifyMsg->next = pqmsg;
        pMsgQueue->pLastNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;
    UNLOCK_MSGQ (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread(hWnd))
        POST_MSGQ(pMsgQueue);
#endif

    return ERR_OK;
}

int GUIAPI PostMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    MSG msg;

    if (!(pMsgQueue = getMsgQueue(hWnd)))
        return ERR_INV_HWND;

    if (nMsg == MSG_PAINT) {
        LOCK_MSGQ (pMsgQueue);
        pMsgQueue->dwState |= QS_PAINT;
        UNLOCK_MSGQ (pMsgQueue);
#ifdef _MGHAVE_VIRTUAL_WINDOW
        if (!getMainWinIfWindowInThisThread(hWnd))
            POST_MSGQ(pMsgQueue);
#endif
        return ERR_OK;
    }

    msg.hwnd = hWnd;
    msg.message = nMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;

    if (!kernel_QueueMessage(pMsgQueue, &msg))
        return ERR_QUEUE_FULL;

    return ERR_OK;
}

int GUIAPI PostQuitMessage (HWND hWnd)
{
    PMSGQUEUE pMsgQueue = NULL;

    if (hWnd && hWnd != HWND_INVALID) {
        PMAINWIN pWin = (PMAINWIN)hWnd;
        if (pWin->DataType == TYPE_HWND || pWin->DataType == TYPE_WINTODEL) {
            pMsgQueue = pWin->pMainWin->pMsgQueue;
        }
    }

    if (pMsgQueue == NULL)
        return ERR_INV_HWND;

    LOCK_MSGQ (pMsgQueue);
    if (!(pMsgQueue->dwState & QS_QUIT)){
        pMsgQueue->loop_depth ++;
        pMsgQueue->dwState |= QS_QUIT;
    }
    UNLOCK_MSGQ (pMsgQueue);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!getMainWinIfWindowInThisThread (hWnd))
        POST_MSGQ (pMsgQueue);
#endif

    return ERR_OK;
}

#include "license.h"

LRESULT GUIAPI DispatchMessage (PMSG pMsg)
{
    WNDPROC WndProc;
#ifdef _MGHAVE_VIRTUAL_WINDOW
    PSYNCMSG pSyncMsg;
#endif
    LRESULT lRet;

    LICENSE_MODIFY_MESSAGE(pMsg);

#ifdef _MGHAVE_TRACE_MSG
    if (pMsg->message != MSG_TIMEOUT && pMsg->message != MSG_CARETBLINK) {
        dump_message (pMsg, __func__);
    }
#endif

    if (pMsg->hwnd == HWND_INVALID) {
#ifdef _MGHAVE_VIRTUAL_WINDOW
        if (pMsg->pSyncMsg) {
            pSyncMsg = (PSYNCMSG)pMsg->pSyncMsg;
            pSyncMsg->retval = ERR_MSG_CANCELED;
            sem_post (pSyncMsg->sem_handle);
        }
#endif

#ifdef _MGHAVE_TRACE_MSG
        if (pMsg->message != MSG_TIMEOUT && pMsg->message != MSG_CARETBLINK) {
            dump_message (pMsg, "thrown away");
        }
#endif
        return -1;
    }

    if (pMsg->hwnd == 0)
        return -1;

    if (!(WndProc = GETWNDPROC (pMsg->hwnd)))
        return -1;

    /* Since 5.0.0 */
    if (pMsg->message == MSG_NOTIFICATION) {
        NOTIFPROC NotifProc = GETNOTIFPROC (pMsg->hwnd);
        if (NotifProc) {
            NotifProc (pMsg->hwnd, (LINT)pMsg->wParam, (int)pMsg->lParam,
                    pMsg->time);
            lRet = 0;
        }
        else {
            lRet = WndProc (pMsg->hwnd, MSG_COMMAND,
                    MAKELONG (pMsg->wParam, pMsg->lParam), pMsg->time);
        }
    }
    else {
        lRet = WndProc (pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
    }


#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* this is a sync message. */
    if (pMsg->pSyncMsg) {
        pSyncMsg = (PSYNCMSG)pMsg->pSyncMsg;
        pSyncMsg->retval = lRet;
        sem_post (pSyncMsg->sem_handle);
    }
#endif

#ifdef _MGHAVE_TRACE_MSG
    if (pMsg->message != MSG_TIMEOUT && pMsg->message != MSG_CARETBLINK) {
        dump_message_with_retval (pMsg, lRet, __func__);
    }
#endif

    return lRet;
}

/* Throw away messages in the message queue for the specified window.
   If the specified window is a main window, all messages sent
   to the controls of the main window will be thrown away as well. */
int __mg_throw_away_messages (PMSGQUEUE pMsgQueue, HWND hWnd)
{
    PMAINWIN    pMainWin = (PMAINWIN)hWnd;
    PMSG        pMsg;
    PQMSG       pQMsg;
    int         nCountN = 0;
    int         nCountS = 0;
    int         nCountP = 0;
    int         readpos;
    int         slot;

    LOCK_MSGQ (pMsgQueue);

    /* for virtual window and main window, use pMainWin to check whether we
       should throw away a message for controls of a main window.
       checkAndGetMainWindowIfControl returns NULL for non control. */
    if (pMainWin != HWND_NULL && pMainWin != HWND_INVALID &&
            (pMainWin->WinType == TYPE_MAINWIN ||
             pMainWin->WinType == TYPE_VIRTWIN))
        pMainWin = (PMAINWIN)hWnd;
    else
        pMainWin = NULL;

    if (pMsgQueue->pFirstNotifyMsg) {
        PQMSG pPrev = NULL, pNext;
        pQMsg = pMsgQueue->pFirstNotifyMsg;

        while (pQMsg) {
            pMsg = &pQMsg->Msg;

            if (pMsg->hwnd == hWnd ||
                    checkAndGetMainWindowIfControl (pMsg->hwnd) == pMainWin) {
#if 0   /* deprecated code */
                pMsg->hwnd = HWND_INVALID;
#else   /* deprecated code */
                // since 5.0.0, we free the msg structure for notification.
                if (pPrev) {
                    pPrev->next = pQMsg->next;
                }
                else {
                    pMsgQueue->pFirstNotifyMsg = pQMsg->next;
                }

                if (pMsgQueue->pLastNotifyMsg == pQMsg) {
                    if (pPrev)
                        pMsgQueue->pLastNotifyMsg = pPrev;
                    else
                        pMsgQueue->pLastNotifyMsg = pMsgQueue->pFirstNotifyMsg;
                }

                /* keep pPrev unchanged */
                pNext = pQMsg->next;
                FREEQMSG (pQMsg);
                pQMsg = pNext;
#endif
                nCountN++;
            }
            else {
                pPrev = pQMsg;
                pQMsg = pQMsg->next;
            }
        }
    }

    _DBG_PRINTF ("%d notification messages thrown for window: %p\n",
            nCountN, hWnd);

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (pMsgQueue->pFirstSyncMsg) {
        PSYNCMSG pSyncMsg, pSyncPrev = NULL;
        pSyncMsg = pMsgQueue->pFirstSyncMsg;

        while (pSyncMsg) {
            pMsg = &pSyncMsg->Msg;

            if (pMsg->hwnd == hWnd ||
                    checkAndGetMainWindowIfControl (pMsg->hwnd) == pMainWin) {
                pMsg->hwnd = HWND_INVALID;
                nCountS ++;

                // notify the waiting thread and remove the node from msg queue
                pSyncMsg->retval = ERR_MSG_CANCELED;
                if (pSyncPrev) {
                    pSyncPrev->pNext = pSyncMsg->pNext;
                }
                else {
                    pSyncPrev = pSyncMsg;
                    pSyncMsg = pSyncMsg->pNext;
                    pMsgQueue->pFirstSyncMsg = pSyncMsg;
                    sem_post (pSyncPrev->sem_handle);
                    pSyncPrev = NULL;
                    continue;
                }
                sem_post (pSyncMsg->sem_handle);
            }

            pSyncPrev = pSyncMsg;
            pSyncMsg = pSyncMsg->pNext;
        }
    }

    _DBG_PRINTF ("%d sync messages thrown for window %p, pMainWin (%p)\n",
            nCountS, hWnd, pMainWin);
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

    readpos = pMsgQueue->readpos;
    while (readpos != pMsgQueue->writepos) {
        pMsg = pMsgQueue->msg + readpos;

        if (pMsg->hwnd == hWnd ||
                checkAndGetMainWindowIfControl (pMsg->hwnd) == pMainWin) {

            nCountP++;
            pMsg->hwnd = HWND_INVALID;

            int pos = readpos;
            int old_pos = readpos;

            do {
                pos++;
                pos %= pMsgQueue->len;

                if (pos != pMsgQueue->writepos) {
                    pMsgQueue->msg[old_pos] = pMsgQueue->msg[pos];
                    old_pos = pos;
                }
                else
                    break;

            } while (1);

            if (pMsgQueue->writepos > 0)
                pMsgQueue->writepos--;
            else
                pMsgQueue->writepos = pMsgQueue->len - 1;
        }
        else {
            readpos++;
            readpos %= pMsgQueue->len;
        }
    }

    _DBG_PRINTF ("%d post messages thrown for window %p\n", nCountP, hWnd);

    /* clear timer message flags of this window */
    for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
        if (pMsgQueue->expired_timer_mask & (0x01UL << slot)) {
            HWND timer_wnd = pMsgQueue->timer_slots [slot]->hWnd;
            if (timer_wnd == hWnd ||
                     checkAndGetMainWindowIfControl (timer_wnd) == pMainWin) {
                pMsgQueue->expired_timer_mask &= ~(0x01UL << slot);
            }
        }
    }

    UNLOCK_MSGQ (pMsgQueue);

    return nCountN + nCountS + nCountP;
}

int GUIAPI ThrowAwayMessages (HWND hWnd)
{
    PMSGQUEUE pMsgQueue = NULL;

    if (hWnd && hWnd != HWND_INVALID) {
        PMAINWIN pWin = (PMAINWIN)hWnd;
        if (pWin->DataType == TYPE_HWND || pWin->DataType == TYPE_WINTODEL) {
            pMsgQueue = pWin->pMainWin->pMsgQueue;
        }
    }

    if (pMsgQueue == NULL)
        return ERR_INV_HWND;

    return __mg_throw_away_messages (pMsgQueue, hWnd);
}

BOOL GUIAPI EmptyMessageQueue (HWND hWnd)
{
    PMSGQUEUE   pMsgQueue;
    PQMSG       pQMsg, temp;

    if (!(pMsgQueue = getMsgQueueIfWindowInThisThread(hWnd)))
        return FALSE;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* Since 5.0.0, we cancel all sync messages */
    if (pMsgQueue->pFirstSyncMsg) {
        PSYNCMSG pSyncMsg, pSyncPrev = NULL;
        pSyncMsg = pMsgQueue->pFirstSyncMsg;

        while (pSyncMsg) {
            pSyncMsg->Msg.hwnd = HWND_INVALID;

                // notify the waiting thread and remove the node from msg queue
            pSyncMsg->retval = ERR_MSG_CANCELED;
            if (pSyncPrev) {
                pSyncPrev->pNext = pSyncMsg->pNext;
            }
            else {
                pSyncPrev = pSyncMsg;
                pSyncMsg = pSyncMsg->pNext;
                pMsgQueue->pFirstSyncMsg = pSyncMsg;
                sem_post (pSyncPrev->sem_handle);
                pSyncPrev = NULL;
                continue;
            }
            sem_post (pSyncMsg->sem_handle);
            pSyncPrev = pSyncMsg;
            pSyncMsg = pSyncMsg->pNext;
        }
    }
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

    if (pMsgQueue->pFirstNotifyMsg) {
        pQMsg = pMsgQueue->pFirstNotifyMsg;
        while (pQMsg) {
            temp = pQMsg->next;
            FREEQMSG (pQMsg);
            pQMsg = temp;
        }
    }

    pMsgQueue->pFirstNotifyMsg = NULL;
    pMsgQueue->pLastNotifyMsg = NULL;

    pMsgQueue->readpos = 0;
    pMsgQueue->writepos = 0;

    pMsgQueue->dwState = QS_EMPTY;
    pMsgQueue->expired_timer_mask = 0;

    return TRUE;
}

#ifdef _MGHAVE_VIRTUAL_WINDOW

/* send a synchronous message to a window in a different thread */
LRESULT SendSyncMessage (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue, thinfo = NULL;
    SYNCMSG SyncMsg;
    sem_t sync_msg;

    if (!(pMsgQueue = getMsgQueue(hWnd)))
        return ERR_INV_HWND;

#define _SYNC_NEED_REENTERABLE
#ifndef _SYNC_NEED_REENTERABLE
    if ((thinfo = getMsgQueueForThisThread ())) {
        /* avoid to create a new semaphore object, Note: it's not reenterable.*/
        SyncMsg.sem_handle = &thinfo->sync_msg;
    }
    else {
        /* this is not a message thread */
        sem_init (&sync_msg, 0, 0);
        SyncMsg.sem_handle = &sync_msg;
    }
#else   /* not defined _SYNC_NEED_REENTERABLE */
    /* For reentrantable implementation, we use a unique semaphore which is
       created in the stack. */
    thinfo = getMsgQueueForThisThread ();
    sem_init (&sync_msg, 0, 0);
    SyncMsg.sem_handle = &sync_msg;
#endif  /* defined _SYNC_NEED_REENTERABLE */

    /* queue the sync message. */
    SyncMsg.Msg.hwnd = hWnd;
    SyncMsg.Msg.message = msg;
    SyncMsg.Msg.wParam = wParam;
    SyncMsg.Msg.lParam = lParam;
    SyncMsg.retval = ERR_MSG_CANCELED;
    SyncMsg.pNext = NULL;

    /* add by houhh 20081030, deal with itself's SyncMsg first before
     * SendSyncMessage to other thread, because other thread maybe wait
     * for SyncMsg for you.*/
    if (thinfo) {
#if 0   /* deprecated code */
        MSG msg;
        PMSGQUEUE pMsgQueue = thinfo;

        while (TRUE) {
            LOCK_MSGQ (pMsgQueue);
            if (pMsgQueue->dwState & QS_SYNCMSG) {
                if (pMsgQueue->pFirstSyncMsg) {
                    msg = pMsgQueue->pFirstSyncMsg->Msg;
                    msg.pSyncMsg = (pMsgQueue->pFirstSyncMsg);
                    pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;
                    UNLOCK_MSGQ (pMsgQueue);
                    TranslateMessage (&msg);
                    DispatchMessage (&msg);
                }
                else {
                    UNLOCK_MSGQ (pMsgQueue);
                    pMsgQueue->dwState &= ~QS_SYNCMSG;
                    break;
                }
            }
            else {
                UNLOCK_MSGQ (pMsgQueue);
                break;
            }
        }
#else   /* deprecated code */
        /* tuned since 5.0.0 */
        MSG msg;
        PMSGQUEUE msg_queue = thinfo;

        LOCK_MSGQ (msg_queue);
        while (msg_queue->dwState & QS_SYNCMSG) {
            if (msg_queue->pFirstSyncMsg) {
                msg = msg_queue->pFirstSyncMsg->Msg;
                msg.pSyncMsg = (msg_queue->pFirstSyncMsg);
                msg_queue->pFirstSyncMsg = msg_queue->pFirstSyncMsg->pNext;
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
            else {
                msg_queue->dwState &= ~QS_SYNCMSG;
            }
        }

        UNLOCK_MSGQ (msg_queue);
#endif  /* tuned code */
    }

    LOCK_MSGQ (pMsgQueue);

    if (pMsgQueue->pFirstSyncMsg == NULL) {
        pMsgQueue->pFirstSyncMsg = pMsgQueue->pLastSyncMsg = &SyncMsg;
    }
    else {
        pMsgQueue->pLastSyncMsg->pNext = &SyncMsg;
        pMsgQueue->pLastSyncMsg = &SyncMsg;
    }

    pMsgQueue->dwState |= QS_SYNCMSG;

    UNLOCK_MSGQ (pMsgQueue);
    POST_MSGQ (pMsgQueue);

    /* suspend until the message has been handled. */
    if (sem_wait (SyncMsg.sem_handle) < 0) {
        _ERR_PRINTF ("SendSyncMessage: thread is interrupted abnormally!\n");
    }

#ifndef _SYNC_NEED_REENTERABLE
    if (thinfo == NULL)
        sem_destroy (&sync_msg);
#else
        sem_destroy (&sync_msg);
#endif

#undef _SYNC_NEED_REENTERABLE
    return SyncMsg.retval;
}

LRESULT GUIAPI PostSyncMessage (HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam)
{
    MG_CHECK_RET (MG_IS_WINDOW(hWnd), -1);
    if (getMainWinIfWindowInThisThread(hWnd))
        return -1;

    return SendSyncMessage (hWnd, msg, wParam, lParam);
}

LRESULT GUIAPI SendAsyncMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam)
{
    WNDPROC WndProc;

    MG_CHECK_RET (MG_IS_WINDOW(hWnd), -1);

    if (!(WndProc = GETWNDPROC(hWnd)))
        return -1;

    return WndProc (hWnd, nMsg, wParam, lParam);
}

#endif /* defined _MGHAVE_VIRTUAL_WINDOW */

