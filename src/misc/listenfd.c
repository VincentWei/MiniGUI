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
** listen.c: routines for listen fd.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/03/28
*/

#include <stdio.h>
#include <string.h>

#include "common.h"

#ifndef _MGRM_THREADS

#ifndef WIN32
#include <sys/poll.h>
#endif

#include "minigui.h"
#include "ourhdr.h"
#include "mgsock.h"

LISTEN_FD mg_listen_fds [MAX_NR_LISTEN_FD];
static fd_set _wfdset, _efdset;
fd_set mg_rfdset;
fd_set* mg_wfdset = NULL;
fd_set* mg_efdset = NULL;
int mg_maxfd;

/* Register/Unregister a listen fd to MiniGUI.
 * When there is a read event on this fd, MiniGUI
 * will post a MSG_FDEVENT message with wParam being equal to
 * MAKELONG (fd, type) to target window.
 */

/* Return TRUE if all OK, and FALSE on error. */
BOOL GUIAPI RegisterListenFD (int fd, int type, HWND hwnd, void* context)
{
    int i = 0;
    for (i = 0; i < MAX_NR_LISTEN_FD; i++) {
        if (mg_listen_fds [i].fd == 0) {
            mg_listen_fds [i].fd = fd;
            mg_listen_fds [i].hwnd = hwnd;
            mg_listen_fds [i].type = type;
            mg_listen_fds [i].context = context;
            switch (type) {
            case POLLIN:
                mg_fd_set (fd, &mg_rfdset);
                break;

            case POLLOUT:
                if (mg_wfdset == NULL) {
                    mg_wfdset = &_wfdset;
                    mg_fd_zero (mg_wfdset);
                }
                mg_fd_set (fd, mg_wfdset);
                break;

            case POLLERR:
                if (mg_efdset == NULL) {
                    mg_efdset = &_efdset;
                    mg_fd_zero (mg_efdset);
                }
                mg_fd_set (fd, mg_efdset);
                break;
            }

            if (mg_maxfd < fd)
                mg_maxfd = fd;
            return TRUE;
        }
    }

    return FALSE;
}

/* Return TRUE if all OK, and FALSE on error. */
BOOL GUIAPI UnregisterListenFD (int fd)
{
    int i = 0;
    for (i = 0; i < MAX_NR_LISTEN_FD; i++) {
        if (mg_listen_fds [i].fd == fd) {
            mg_listen_fds [i].fd = 0;
            switch (mg_listen_fds [i].type) {
            case POLLIN:
                mg_fd_clr (fd, &mg_rfdset);
                break;

            case POLLOUT:
                if (mg_wfdset == NULL)
                    return FALSE;
                mg_fd_clr (fd, mg_wfdset);
                break;

            case POLLERR:
                if (mg_efdset == NULL)
                    return FALSE;
                mg_fd_clr (fd, mg_efdset);
                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}

#endif
