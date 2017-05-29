/*
** $Id: ipaq-h3600.c 8944 2007-12-29 08:29:16Z xwyan $
**
** ipaq-h3600.c: Low Level Input Engine for iPAQ H3600/H3800
**         This driver can run on eCos and Linux.
** 
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** The device driver of iPAQ H3600 touch screen is compliant to
** the Specification for Generic Touch Screen Driver.
** Please see the file: spec_general_ts_drv.html in this directory.
**
** So this IAL engine can be a good template of your new IAL engines,
** which compliant to the specification.
**
** Created by Wei Yongming, 2001/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_IPAQ_H3600

#include <unistd.h>
#include <fcntl.h>

#ifdef __ECOS__

#define TS_DEVICE   "/dev/ts"
#define KBD_DEVICE  "/dev/kbd"

#include <sys/select.h>

#else /* LINUX */

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include <linux/h3600_ts.h>

#define TS_DEVICE   "/dev/h3600_ts"
#define KBD_DEVICE  "/dev/h3600_key"

#endif

#include "ial.h"
#include "ipaq-h3600.h"

/* for data reading from /dev/hs3600_ts */
typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

static unsigned char state [NR_KEYS];
static int ts = -1;
static int btn_fd = -1;
static unsigned char btn_state=0;
static int mousex = 0;
static int mousey = 0;
static POS pos;

#undef _DEBUG

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

    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return pos.b;
}

static int keyboard_update(void)
{
    int status;
    int key;

    //Attention!
    status = (btn_state & KEY_RELEASED)? 0 : 1;
    key = btn_state & KEY_NUM;

#ifdef _DEBUG
    fprintf(stderr, "key %d is %s", key, statinfo = (btn_state & KEY_RELEASED)? "UP":"DOWN");
#endif

    switch (key)
    {
    case 1: 
        state[SCANCODE_LEFTSHIFT] = status;
    break;
    case 2: 
        state[H3600_SCANCODE_CALENDAR] = status;
    break;
    case 3: 
        state[H3600_SCANCODE_CONTACTS] = status;
    break;
    case 4: 
        state[H3600_SCANCODE_Q] = status;
    break;
    case 5:
        state[H3600_SCANCODE_START] = status;
    break;
    case 6:
        state[H3600_SCANCODE_UP] = status;
    break;
    case 7:
        state[H3600_SCANCODE_RIGHT] = status;
    break;
    case 8:
        state[H3600_SCANCODE_LEFT] = status;
    break;
    case 9:
        state[H3600_SCANCODE_DOWN] = status;
    break;
    case 10:
        state[H3600_SCANCODE_ACTION] = status;
    break;
    case 11:
        state[H3600_SCANCODE_SUSPEND] = status;
    break;
    }

    return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
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
#ifndef _MGRM_THREADS
        if (ts > maxfd) maxfd = ts;
#endif
    }
    if ((which & IAL_KEYEVENT) && btn_fd >= 0){
        FD_SET (btn_fd, in);
#ifndef _MGRM_THREADS
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }

#ifndef _MGRM_THREADS
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        if (ts >= 0 && FD_ISSET (ts, in)) {
            short data [4];

            FD_CLR (ts, in);
            read (ts, data, sizeof (data));
            if (data[0]) {
                mousex = pos.x = data[1];
                mousey = pos.y = data[2];
            }

#ifdef _DEBUG
            if (data[0]) {
                printf ("mouse down: pos.x = %d, pos.y = %d\n", pos.x, pos.y);
            }
#endif

            pos.b = (data[0] ? IAL_MOUSE_LEFTBUTTON : 0);
            retvalue |= IAL_MOUSEEVENT;
        }

        if (btn_fd >= 0 && FD_ISSET(btn_fd, in)) {
            unsigned char key;
            FD_CLR(btn_fd, in);
            read(btn_fd, &key, sizeof(key));
            btn_state = key;
            retvalue |= IAL_KEYEVENT;
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitIPAQH3600Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY);
    if (ts < 0) {
        _MG_PRINTF ("IAL>%s: Can not open touch screen!\n", __FILE__);
        return FALSE;
    }

    btn_fd = open (KBD_DEVICE, O_RDONLY);
    if (btn_fd < 0 ) {
        _MG_PRINTF ("IAL>%s: Can not open button key!\n", __FILE__);
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    pos.x = pos.y = pos.b = 0;
    
    return TRUE;
}

void TermIPAQH3600Input (void)
{
    if (ts >= 0)
        close(ts);
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _MGIAL_IPAQ_H3600 */

