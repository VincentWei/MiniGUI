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
** listen.c: routines for listen fd.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/03/28
*/

#include <stdio.h>
#include <string.h>

#include "common.h"

#ifdef HAVE_SELECT

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "internals.h"
#include "timer.h"
#include "mgsock.h"

#ifdef _MGRM_PROCESSES
#include "sharedres.h"
#endif

int __mg_kernel_check_listen_fds (MSGQUEUE* msg_queue,
        fd_set* rsetptr, fd_set* wsetptr, fd_set* esetptr)
{
    int i;
    int nr = 0;
    MSG Msg;

    Msg.message = MSG_FDEVENT;

    /* go through registered listening fds */
    for (i = 0; i < msg_queue->nr_fd_slots; i++) {
        if (msg_queue->fd_slots[i]) {
            int type_set = msg_queue->fd_slots[i]->type;
            int type_got = 0;

            if (type_set & POLLIN) {
                if (rsetptr && FD_ISSET (msg_queue->fd_slots[i]->fd, rsetptr)) {
                    type_got |= POLLIN;
                }
            }
            else if (type_set & POLLOUT) {
                if (wsetptr && FD_ISSET (msg_queue->fd_slots[i]->fd, wsetptr)) {
                    type_got |= POLLOUT;
                }
            }
            else if (type_set & POLLERR) {
                if (esetptr && FD_ISSET (msg_queue->fd_slots[i]->fd, esetptr)) {
                    type_got |= POLLERR;
                }
            }

            if (type_got) {
                Msg.hwnd = (HWND)msg_queue->fd_slots[i]->hwnd;

                // XXX: Here the fd was assumend as a unsigned short integer
                Msg.wParam = MAKELONG (msg_queue->fd_slots[i]->fd, type_got);
                Msg.lParam = (LPARAM)msg_queue->fd_slots[i]->context;
                kernel_QueueMessage (msg_queue, &Msg);
                nr++;
            }
        }
    }

    return nr;
}

/* the idle handler for a message queue with checking listening fds */
BOOL __mg_idle_handler_with_fds (MSGQUEUE* msg_queue, BOOL wait)
{
    int n;
    fd_set rset, wset, eset;
    fd_set* rsetptr = NULL;
    fd_set* wsetptr = NULL;
    fd_set* esetptr = NULL;
    struct timeval sel_timeout;

    if (wait) {
        if (msg_queue->nr_timers > 0) {
            sel_timeout.tv_sec = 0;
            sel_timeout.tv_usec = USEC_10MS;
        }
        else {
            sel_timeout.tv_sec = 0;
            sel_timeout.tv_usec = USEC_TIMEOUT;
        }
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

    if ((n = select (msg_queue->maxfd + 1,
            rsetptr, wsetptr, esetptr, &sel_timeout)) < 0) {
        if (errno == EINTR) {
            /* it is time to check message again. */
            return FALSE;
        }
        _WRN_PRINTF ("failed to call select\n");
    }
    else if (n == 0) {
#ifdef _MGRM_PROCESSES
        if (MG_UNLIKELY (msg_queue->old_tick_count == 0))
            msg_queue->old_tick_count = SHAREDRES_TIMER_COUNTER;

        n = __mg_check_expired_timers (msg_queue,
                SHAREDRES_TIMER_COUNTER - msg_queue->old_tick_count);
        msg_queue->old_tick_count = SHAREDRES_TIMER_COUNTER;
#else
        if (MG_UNLIKELY (sg_old_counter == 0))
            msg_queue->old_tick_count = __mg_timer_counter;
        n = __mg_check_expired_timers (msg_queue,
                __mg_timer_counter - msg_queue->old_tick_count);
        msg_queue->old_tick_count = __mg_timer_counter;
#endif
    }

    if (rsetptr || wsetptr || esetptr) {
        n += __mg_kernel_check_listen_fds (msg_queue, rsetptr, wsetptr, esetptr);
    }

    if (wait && n == 0) {
        // no MSG_TIMER or MSG_FDEVENT message...
        // it time to post MSG_IDLE to all windows in this thread.
#ifdef _MGRM_PROCESSES
        MSG msg = { HWND_NULL, MSG_IDLE, SHAREDRES_TIMER_COUNTER, 0 };
#else
        MSG msg = { HWND_NULL, MSG_IDLE, __mg_timer_counter, 0 };
#endif
        n = __mg_broadcast_message (msg_queue, &msg);
    }

    return n > 0;
}

/*
 * Register/Unregister a listen fd to MiniGUI.
 * When there is a read event on this fd, MiniGUI
 * will post a MSG_FDEVENT message with wParam being equal to
 * MAKELONG (fd, type) to target window.
 */

/* Return TRUE if all OK, and FALSE on error. */
static BOOL allocate_more_fd_slots (MSGQUEUE* msg_queue)
{
    void* new_array;

    new_array = realloc (msg_queue->fd_slots,
                sizeof (LISTEN_FD*) * (msg_queue->nr_fd_slots +
                    MAX_NR_LISTEN_FD));

    if (new_array) {
        int i;

        msg_queue->fd_slots = (LISTEN_FD**)new_array;
        for (i = msg_queue->nr_fd_slots;
                i < msg_queue->nr_fd_slots + MAX_NR_LISTEN_FD; i++) {
            msg_queue->fd_slots[i] = NULL;
        }
        msg_queue->nr_fd_slots += MAX_NR_LISTEN_FD;
    }
    
    return FALSE;
}

BOOL GUIAPI RegisterListenFD (int fd, int type, HWND hwnd, void* context)
{
    MSGQUEUE* msg_queue;
    LISTEN_FD* fd_slot = NULL;

    msg_queue = getMsgQueueForThisThread ();
    if (msg_queue) {
        int i = 0;
        BOOL ok = FALSE;

        if (msg_queue->fd_slots == NULL) {
            assert (msg_queue->nr_fd_slots == 0);
            if (!allocate_more_fd_slots (msg_queue))
                goto failed;
        }

        for (i = 0; i < msg_queue->nr_fd_slots; i++) {
            if (msg_queue->fd_slots[i] == NULL) {
                msg_queue->fd_slots[i] = mg_slice_new (LISTEN_FD);
                if (msg_queue->fd_slots[i] == NULL)
                    goto failed;

                fd_slot = msg_queue->fd_slots[i];
                break;
            }
        }

        if (fd_slot == NULL) {
            int free_slot;

            if (!allocate_more_fd_slots (msg_queue))
                goto failed;

            free_slot = msg_queue->nr_fd_slots - MAX_NR_LISTEN_FD;
            msg_queue->fd_slots[free_slot] = mg_slice_new (LISTEN_FD);
            if (msg_queue->fd_slots[free_slot] == NULL)
                goto failed;

            fd_slot = msg_queue->fd_slots[free_slot];
        }

        fd_slot->fd = fd;
        fd_slot->hwnd = hwnd;
        fd_slot->type = type;
        fd_slot->context = context;

        if (type & POLLIN) {
            if (!mg_fd_isset (fd, &msg_queue->rfdset)) {
                mg_fd_set (fd, &msg_queue->rfdset);
                msg_queue->nr_rfds++;
                ok = TRUE;
            }
        }
        else if (type & POLLOUT) {
            if (!mg_fd_isset (fd, &msg_queue->wfdset)) {
                mg_fd_set (fd, &msg_queue->wfdset);
                msg_queue->nr_wfds++;
                ok = TRUE;
            }
        }
        else if (type & POLLERR) {
            if (!mg_fd_isset (fd, &msg_queue->wfdset)) {
                mg_fd_set (fd, &msg_queue->efdset);
                msg_queue->nr_efds++;
                ok = TRUE;
            }
        }

        if (ok && msg_queue->maxfd < fd) {
            msg_queue->maxfd = fd;
            return TRUE;
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

failed:
    return FALSE;
}

/* Return TRUE if all OK, and FALSE on error. */
BOOL GUIAPI UnregisterListenFD (int fd)
{
    MSGQUEUE* msg_queue;

    msg_queue = getMsgQueueForThisThread ();
    if (msg_queue) {
        int i = 0;

        for (i = 0; i < msg_queue->nr_fd_slots; i++) {
            if (msg_queue->fd_slots[i] && msg_queue->fd_slots[i]->fd == fd) {
                if (msg_queue->fd_slots[i]->type & POLLIN) {
                    mg_fd_clr (fd, &msg_queue->rfdset);
                    msg_queue->nr_rfds--;
                }
                else if (msg_queue->fd_slots[i]->type & POLLOUT) {
                    mg_fd_clr (fd, &msg_queue->wfdset);
                    msg_queue->nr_wfds--;
                }
                else if (msg_queue->fd_slots[i]->type & POLLERR) {
                    mg_fd_clr (fd, &msg_queue->efdset);
                    msg_queue->nr_efds--;
                }

                mg_slice_delete (LISTEN_FD, msg_queue->fd_slots[i]);
                msg_queue->fd_slots[i] = NULL;
                return TRUE;
            }
        }
    }
    else {
        _WRN_PRINTF ("called for non message thread\n");
    }

    return FALSE;
}

#if 0
/* Since 4.2.0, MiniGUI provides support for listening file descriptors
   per message thread */
LISTEN_FD mg_listen_fds[MAX_NR_LISTEN_FD];
static fd_set _wfdset, _efdset;
fd_set mg_rfdset;
fd_set* mg_wfdset = NULL;
fd_set* mg_efdset = NULL;
int mg_maxfd;
#endif /* deprecated code */

#endif /* defined HAVE_SELECT */
