/*
** $Id: hh2440.c 10951 2008-09-11 03:51:14Z xwyan $
**
** hh2440.c: Low Level Input Engine for Huaheng ARM S3C2440 Dev Board.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "common.h"

#ifdef _HH2440_IAL

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "hh2440.h"


#define DIS_NUM 4

/* for data reading from /dev/touchscreen/0raw */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;

static int ts = -1;

static int nr_events = 0;
static TS_EVENT ts_events [DIS_NUM];

static int button = 0;
static int mousex, mousey;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    int i;
    int max_x = ts_events [0].x;
    int min_x = ts_events [0].x;
    int max_event = 0, min_event = 0;
    int sum_x = 0, sum_y = 0;

    if (nr_events != DIS_NUM) {
        *x = mousex;
        *y = mousey;
#if 0
        printf ("return old mouse position: %d, %d\n", *x, *y);
#endif
        return;
    }

    for (i = 1; i < nr_events; i++) {

        if (ts_events [i].x > max_x) {
			max_x = ts_events [i].x;
            max_event = i;
		}

        if (ts_events [i].x < min_x) {
			min_x = ts_events [i].x;
            min_event = i;
		}
    }

    for (i = 0; i < nr_events; i++) {
        if (i != max_event && i != min_event) {
            sum_x += ts_events [i].x;
            sum_y += ts_events [i].y;
        }
    }

	if (max_event == min_event ) {
		*x = sum_x / (nr_events - 1);
		*y = sum_y / (nr_events - 1);
	}
	else {
		*x = sum_x / (nr_events - 2);
		*y = sum_y / (nr_events - 2);
	}

    if (ABS (mousex - *x) > 10 || ABS (mousey - *y) > 10) {
        mousex = *x;
        mousey = *y;
    }
#if 0
    else
        printf ("ignore the floating position data: %d, %d\n", *x, *y);
#endif

    *x = mousex;
    *y = mousey;
    nr_events = 0;

#if 0
    printf ("return mouse position: %d, %d\n", *x, *y);
#endif
}

static int mouse_getbutton (void)
{
    return button;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    fd_set rfds;
    int    retvalue = 0;
    int    e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        FD_SET (ts, in);
#ifdef _LITE_VERSION
        if (ts > maxfd) maxfd = ts;
#endif
    }

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        if (ts >= 0 && FD_ISSET (ts, in)) {
            TS_EVENT ts_event;

            FD_CLR (ts, in);

            read (ts, &ts_event, sizeof (TS_EVENT));
            if (ts_event.pressure <= 0) {
                nr_events = 0;
                button = 0;
                retvalue |= IAL_MOUSEEVENT;
            }
            else {
                ts_events [nr_events] = ts_event;
                nr_events ++;
                if (nr_events >= DIS_NUM) {
                    button = IAL_MOUSE_LEFTBUTTON;
                    retvalue |= IAL_MOUSEEVENT;
                }
            }
        }
    } else {
        if (e < 0)
            return -1;
    }

    return retvalue;
}

BOOL InitHH2440Input(INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (mdev, O_RDONLY| O_NONBLOCK);
    if (ts < 0) {
        fprintf (stderr, "IAL>2440: Can not open touch screen!\n");
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

void TermHH2440Input (void) 
{

    if (ts >= 0)
        close (ts);    
}

#endif /* _HH2440_IAL */

