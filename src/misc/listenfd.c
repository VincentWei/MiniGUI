/*
** $Id: listenfd.c 12871 2010-05-07 06:13:42Z wanzheng $
**
** listen.c: routines for listen fd.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
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
                FD_SET (fd, &mg_rfdset);
                break;

            case POLLOUT:
                if (mg_wfdset == NULL) {
                    mg_wfdset = &_wfdset;
                    FD_ZERO (mg_wfdset);
                }
                FD_SET (fd, mg_wfdset);
                break;

            case POLLERR:
                if (mg_efdset == NULL) {
                    mg_efdset = &_efdset;
                    FD_ZERO (mg_efdset);
                }
                FD_SET (fd, mg_efdset);
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
                FD_CLR (fd, &mg_rfdset);
                break;

            case POLLOUT:
                if (mg_wfdset == NULL)
                    return FALSE;
                FD_CLR (fd, mg_wfdset);
                break;

            case POLLERR:
                if (mg_efdset == NULL)
                    return FALSE;
                FD_CLR (fd, mg_efdset);
                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}

#endif
