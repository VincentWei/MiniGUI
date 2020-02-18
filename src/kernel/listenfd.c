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
/* Since 5.0.0, MiniGUI provides support for listening file descriptors
   per message thread */
LISTEN_FD mg_listen_fds[MAX_NR_LISTEN_FD];
static fd_set _wfdset, _efdset;
fd_set mg_rfdset;
fd_set* mg_wfdset = NULL;
fd_set* mg_efdset = NULL;
int mg_maxfd;
#endif /* deprecated code */

#endif /* defined HAVE_SELECT */
