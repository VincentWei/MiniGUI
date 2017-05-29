/*
** $Id: l7200.c 10951 2008-09-11 03:51:14Z xwyan $
**
** l7200.c: Low Level Input Engine for LinkUp System L7200 SDB
**         This driver can run on Linux.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Yao Yunyuan, 2004/06/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _L7200_IAL

#include <unistd.h>
#include <fcntl.h>

#define TS_DEVICE   "/dev/tscreen"

#include <sys/select.h>
#include <sys/poll.h>

#include "ial.h"
#include "l7200.h"

/* for data reading from /dev/l7200_ts */
typedef struct {
    short x;
    short y;
    short b;
    short pad;
} TS_EVENT;

static int mousex = 0;
static int mousey = 0;
static int stylus = 0;
static int ts = -1;

#undef _DEBUG

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

#define REP_COUNT   1

static inline int get_middle (int *aa, int count)
{
    int av = 0;
    int i;

    for (i = 0; i < count; i++) {
        av += aa[i];
    }

    return av/count;
}


static int mouse_update(void)
{
    int xa[REP_COUNT], ya[REP_COUNT];
    int count = 0;
    TS_EVENT cBuffer;
    int rsize = 0;

    while (1) {
        rsize = read (ts, &cBuffer, sizeof (TS_EVENT));
        if (rsize <= 0 || rsize != sizeof(TS_EVENT))
            return 0;

        if (cBuffer.b == 0) {
            stylus = 0;
            break;
        }

        xa[count] = cBuffer.x;
        ya[count] = cBuffer.y;

        if (count == REP_COUNT - 1)
        {
            mousex = get_middle (xa, REP_COUNT);
            mousey = get_middle (ya, REP_COUNT);;
            stylus = 1;
            count = 0;
            break;
        }
        count++;
    }

    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return stylus ? IAL_MOUSE_LEFTBUTTON : 0;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    int retvalue = 0;
    int e;
    struct timeval stimeout;

    stimeout.tv_usec = 10;
    stimeout.tv_sec = 0;

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, &stimeout);
#else
    e = select (FD_SETSIZE, in, out, except, &stimeout);
#endif

    if (e < 0) {
        return -1;
    }
    else if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        retvalue |= IAL_MOUSEEVENT;
    }

    return retvalue;
}

BOOL InitL7200Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY | O_NONBLOCK);
    if (ts < 0) {
        fprintf (stderr, "Linkup: Can not open touch screen!\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = NULL;
    input->get_keyboard_state = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;

    return TRUE;
}

void TermL7200Input (void)
{
    if (ts > 0)
        close (ts);
}
#endif
