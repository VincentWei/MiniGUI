/*
** $Id$
**
** tslib.c: Low Level Input Engine for TSLIB.
** 
** Copyright (C) 2008 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_TSLIB

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <tslib.h>

#include "ial.h"
#include "tslibial.h"

static struct tsdev *ts = NULL;
static int mousex = 0;
static int mousey = 0;
static int button = 0;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    struct ts_sample samp;

    if (ts_read (ts, &samp, 1) > 0) {
        if (samp.pressure > 0) {
            mousex = samp.x;
            mousey = samp.y;
        }

        button = (samp.pressure > 0) ? IAL_MOUSE_LEFTBUTTON : 0;
        return 1;
    }else{
        return 0;
    }
}

static void mouse_getxy(int *x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return button;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    ret = 0;
    int    fd;
    int    e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    fd = ts_fd (ts);

    if ((which & IAL_MOUSEEVENT) && fd >= 0) {
        FD_SET (fd, in);
#ifndef _MGRM_THREADS
        if (fd > maxfd) maxfd = fd;
#endif
    }

#ifndef _MGRM_THREADS
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (e > 0) { 
        if (fd > 0 && FD_ISSET (fd, in)) {
            return IAL_MOUSEEVENT;
        }
    } 
    else if (e < 0) {
        return -1;
    }

    return ret;
}

BOOL InitTSLibInput(INPUT* input, const char* mdev, const char* mtype)
{
    const char* tsdevice;

    if ((tsdevice = getenv ("TSLIB_TSDEVICE")) == NULL) {
        tsdevice = mdev;
    }

    if (tsdevice == NULL) {
        _MG_PRINTF ("IAL>TSLib: Please specify the ts device\n");
        return FALSE;
    }

    ts = ts_open (tsdevice, 0);

    if (!ts) {
        _MG_PRINTF ("IAL>TSLib: can not open ts device\n");
        return FALSE;
    }

    if (ts_config (ts)) {
        _MG_PRINTF ("IAL>TSLib: can not config ts device\n");
        return FALSE;
    }
    
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    return TRUE;
}

void TermTSLibInput(void)
{
    if (ts) {
        ts_close(ts);    
        ts = NULL;
    }
}

#endif /* _MGIAL_TSLIB */
