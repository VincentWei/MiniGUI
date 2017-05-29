/*
** $Id: palm2.c 10951 2008-09-11 03:51:14Z xwyan $
**
** palm2.c: Low Level Input Engine for OKWAP PalmII.
**          This engine runs on eCos.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Wei Yongming, 2004/01/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _PALMII_IAL

#include <unistd.h>
#include <fcntl.h>

#define TS_DEVICE   "/dev/ts"
#define KBD_DEVICE  "/dev/kbd"

#include "ial.h"
#include "palm2.h"

/* for data reading from /dev/ts */
typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

static unsigned char state [NR_KEYS];
static int ts = -1;
static int btn_fd = -1;
static unsigned char cur_key=111;
static unsigned char prev_key = 0;
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

static int key2scancode [30] =
{
    SCANCODE_1,
    SCANCODE_2,
    SCANCODE_3,
    SCANCODE_4,
    SCANCODE_5,
    SCANCODE_6,
    SCANCODE_7,
    SCANCODE_8,
    SCANCODE_9,
    SCANCODE_ENTER,
    SCANCODE_0,
    SCANCODE_ENTER,
    SCANCODE_F1,
    SCANCODE_F2,
    SCANCODE_F3,
    SCANCODE_F4,
    SCANCODE_F5,
    SCANCODE_F6,
    SCANCODE_F7,
    SCANCODE_F8,
    SCANCODE_F9,
    SCANCODE_F10,
    SCANCODE_F11,
    SCANCODE_BACKSPACE,
    SCANCODE_CURSORBLOCKUP,
    SCANCODE_CURSORBLOCKDOWN,
    SCANCODE_CURSORBLOCKLEFT,
    SCANCODE_CURSORBLOCKRIGHT,
    SCANCODE_ENTER,
    SCANCODE_ESCAPE,
};

static int keyboard_update(void)
{
/*    static unsigned char prev_key = 0;

    if (cur_key < 30 && cur_key >= 0) {
        state[key2scancode[cur_key]] = 1;
        prev_key = cur_key;
    }
    else if (cur_key >= 0x80 && cur_key < (0x80 | 30))
        state[key2scancode[prev_key]] = 0;
*/
    return NR_KEYS;

    /* all key simulates the left shift key */
    /*
    if (cur_key) {
        state [SCANCODE_LEFTSHIFT] = 1;
    }
    else
        state [SCANCODE_LEFTSHIFT] = 0;

    return SCANCODE_LEFTSHIFT + 1;
    */

#if 0
    static unsigned char last_key=0;

    int status;
    int key;

    status = (last_key & KEY_RELEASED)? 0 : 1;
    key = last_key & KEY_NUM;
#ifdef _DEBUG
    fprintf(stderr, "key %d is %s", key, (last_key & KEY_RELEASED)? "UP":"DOWN");
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
#endif
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
    if ((which & IAL_KEYEVENT) && btn_fd >= 0){
        FD_SET (btn_fd, in);
#ifdef _LITE_VERSION
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }

#ifdef _LITE_VERSION
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
			int ret = 0;
            FD_CLR (btn_fd, in);
            ret = read (btn_fd, &cur_key, sizeof (cur_key));
			if (ret != sizeof (cur_key)) {
				printf ("%s:IAL_ENGINE,read error!!!!:ret:%i\n", __FUNCTION__, ret);
				return -1;
			}
			printf ("%s:IAL_ENGINE:cur_key:%i\n", __FUNCTION__, cur_key);
			if (cur_key < 30 && cur_key >= 0) {
				state[key2scancode[cur_key]] = 1;
				prev_key = cur_key;
				retvalue |= IAL_KEYEVENT;
			}
			else if (cur_key >= 0x80 && cur_key < (0x80 | 30)) {
				state[key2scancode[prev_key]] = 0;
				retvalue |= IAL_KEYEVENT;
			}
		}

    }
    else if (e == 0) {
        /* simulate the key up when time out */
        //if (cur_key >= 0 && cur_key < 30) {
          //  cur_key = 111;
			if (state[key2scancode[prev_key]] == 1) {
            	return IAL_KEYEVENT;
			}
    }
    else {
        return -1;
    }

    return retvalue;
}

BOOL InitPALMIIInput (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY);
    if (ts < 0) {
        fprintf (stderr, "PALMII: Can not open touch screen!\n");
        return FALSE;
    }

    btn_fd = open (KBD_DEVICE, O_RDONLY);
    if (btn_fd < 0 ) {
        fprintf (stderr, "PALMII: Can not open button key!\n");
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

void TermPALMIIInput (void)
{
    if (ts >= 0)
        close(ts);
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _PALMII_IAL */

