/*
** $Id: evmv10.c 10951 2008-09-11 03:51:14Z xwyan $
**
** evmv10e.c: Low Level Input Engine for DigiPro xScale EVMV 1.0.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Yao Yunyuan, 2004/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#ifdef _EVMV10_IAL

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <termios.h>

#include "ial.h"
#include "evmv10.h" 

#define _DEBUG_EVMV10    0

/* ------------------------------------------------------------------------- */

#ifndef KEY_RELEASED
#define KEY_RELEASED    0x80
#endif

#ifndef KEY_NUM
#define KEY_NUM        0x7F
#endif

/* for data reading from /dev/tpanel */
typedef struct {
 short header;
 short x;
 short y;
 short pad;
 struct timeval  stamp;

} POS;

static int ts = -1;
static int kb_fd = -1;

static unsigned char state [NR_KEYS];
static unsigned char btn_state=0;

static POS pos;

/************************  Low Level Input Operations **********************/

static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = pos.x;
    *y = pos.y;
    
}

static int mouse_getbutton(void)
{
//    printf("pos.header=%d\n",pos.header);
    return pos.header;

}

static int keyboard_update(void)
{
    int status;
    int key;

    status = (btn_state & KEY_RELEASED)? 0: 1;
    key = btn_state & KEY_NUM;

    if (key == SCANCODE_CURSORLEFT)
    key = SCANCODE_CURSORBLOCKLEFT;
    else if (key == SCANCODE_CURSORRIGHT)
    key = SCANCODE_CURSORBLOCKRIGHT;
    else if (key == SCANCODE_CURSORUP)
    key = SCANCODE_CURSORBLOCKUP;
    else if (key == SCANCODE_CURSORDOWN)
    key = SCANCODE_CURSORBLOCKDOWN;

    state[key] = status;

    return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
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
    if ((which & IAL_KEYEVENT) && kb_fd >= 0){
        FD_SET (kb_fd, in);
#ifdef _LITE_VERSION
        if(kb_fd > maxfd) maxfd = kb_fd;
#endif
    }

#ifdef _LITE_VERSION
    if ( select (maxfd + 1, in, out, except, timeout) < 0 )
#else
    if ( select (FD_SETSIZE, in, out, except, timeout) < 0 )
#endif
    return -1;
   
    if (ts >= 0 && FD_ISSET (ts, in))
    {
        FD_CLR (ts, in);
        if ( read (ts, &pos, sizeof (POS)) == sizeof(POS) ) {
            pos.header = ( pos.header > 0 ? IAL_MOUSE_LEFTBUTTON:0 );
            retvalue |= IAL_MOUSEEVENT;
        }
        else {
            fprintf (stderr, "read pos data error!\n");
            return -1;
        }
    }
 

    if (kb_fd >= 0 && FD_ISSET(kb_fd, in))
    {
        unsigned char scankey;

        FD_CLR(kb_fd, in);

        if (read(kb_fd, &scankey, sizeof(scankey)) == sizeof(scankey)) {
#if _DEBUG_PX255B
        printf ("scankey = %d\n", scankey);
#endif
        /* 224 where from ? */
        if (scankey == 224) {
        return -1;
        }

            btn_state = scankey;
            retvalue |= IAL_KEYEVENT;
        }
        else {
            fprintf (stderr, "read pos data error!\n");
            return -1;
        }
    }

    return retvalue;
}

BOOL InitXscaleEVMV10Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open ("/dev/tsc2101/touchscreen", O_RDWR);
    if (ts < 0) {
        fprintf (stderr, "EVMV10: Can not open touch screen!\n");
        return FALSE;
    }

#if 0
    kb_fd = open ("/dev/kb_7289", O_RDONLY);
    if (kb_fd < 0 ) {
        fprintf (stderr, "PX255B: Can not open key board!\n");
        return FALSE;
    }
    if (ioctl(kb_fd, KDSKBMODE, K_MEDIUMRAW) < 0) {
        fprintf (stderr, "PX255B: key board ioctl error!\n");
        return FALSE;
    }
#endif

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    pos.x = pos.y = pos.header = 0;

    return TRUE;
}

void TermXscaleEVMV10Input (void)
{
    if (ts >= 0)
        close(ts);
    if (kb_fd >= 0)
        close(kb_fd);
}

#endif /* _EVMV10_IAL */

