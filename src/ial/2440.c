/*
** $Id: 2410.c 10951 2008-09-11 03:51:14Z xwyan $
**
** 2410.c: Low Level Input Engine for SMDK2410 Dev Board.
** 
** Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_2440

#include <unistd.h>
#include <fcntl.h>
//#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "2440.h"

/* for data reading from /dev/ts */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;

static unsigned char state [NR_KEYS];
static int ts = -1;
static int mousex = 0;
static int mousey = 0;
static TS_EVENT ts_event;


/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
#ifdef _DEBUG
    printf ("mousex = %d, mousey = %d\n", mousex, mousey);
#endif

    if (mousex < 0) mousex = 0;
    if (mousey < 0) mousey = 0;
    if (mousex > 799) mousex = 799;
    if (mousey > 479) mousey = 479;

    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return ts_event.pressure;
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
		    int rs;
            FD_CLR (ts, in);
            ts_event.x=0;
            ts_event.y=0;

            rs = read (ts, &ts_event, sizeof (TS_EVENT));      
            //rs = read (ts, &ts_event, sizeof (TS_EVENT));      
			if (rs != sizeof(TS_EVENT))
			    return -1;
            
            if (ts_event.pressure > 0) {
                mousex = ts_event.x;
                mousey = ts_event.y;
            }
#ifdef _DEBUG
            if (ts_event.pressure > 0) {
                printf ("mouse down: ts_event.x = %d, ts_event.y = %d\n", ts_event.x, ts_event.y);
            }
#endif
            ts_event.pressure = ( ts_event.pressure > 0 ? IAL_MOUSE_LEFTBUTTON:0);
            retvalue |= IAL_MOUSEEVENT;
        }

    } 
    else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL Init2440Input (INPUT* input, const char* mdev, const char* mtype)
{
    /* mdev should be /dev/ts */
    ts = open (mdev, O_RDONLY);
    if (ts < 0) {
        fprintf (stderr, "2410: Can not open touch screen: %s!\n", mdev);
        return FALSE;
    }

    printf ("In 2410 engine\n");

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    ts_event.x = ts_event.y = ts_event.pressure = 0;
    
    return TRUE;
}

void Term2440Input(void) 
{
    if (ts >= 0)
        close(ts);    
}

#endif /* _SMDK2410_IAL */

