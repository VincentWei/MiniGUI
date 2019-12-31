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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** usvfbinput.c: Input Engine for UnixSocket Virtual Frame Buffer
**
** Created by Vincent Wei, 2018/07/29
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef _MGIAL_USVFB

#include "minigui.h"
#include "misc.h"
#include "ial.h"

#include "usvfbinput.h"

#define USVFB_MOUSEINPUT    0x01
#define USVFB_KBINPUT       0x02

#define USVFB_MOUSELBUTTON  0x01
#define USVFB_MOUSERBUTTON  0x04

extern int __mg_usvfb_fd; // defined in USVFB NEWGAL engine.

/* ----------------------------------------------------------------------- */
#define FT_VFBINFO      10
#define FT_PING         11
#define FT_PONG         12
#define FT_EVENT        13
#define FT_DIRTYPIXELS  14

struct _frame_header {
    int type;
    size_t payload_len;
    unsigned char payload[0];
};

#define EVENT_NULL          0
#define EVENT_MOUSEMOVE     1
#define EVENT_LBUTTONDOWN   2
#define EVENT_LBUTTONUP     3

#define EVENT_KEYDOWN       11
#define EVENT_KEYUP         12

struct _remote_event {
    int type;
    int value1;
    int value2;
};

static struct _remote_event _last_event;

/* return 0 when there is really a touch event */
static int my_ts_getdata (short *x, short *y, short *button)
{
    switch (_last_event.type) {
    case EVENT_MOUSEMOVE:
        *x = _last_event.value1;
        *y = _last_event.value2;
        break;

    case EVENT_LBUTTONDOWN:
        *x = _last_event.value1;
        *y = _last_event.value2;
        *button = USVFB_MOUSELBUTTON;
        break;

    case EVENT_LBUTTONUP:
        *x = _last_event.value1;
        *y = _last_event.value2;
        *button = 0;
        break;

    default:
        return -1;
    }

    memset (&_last_event, 0, sizeof (struct _remote_event));
    return 0;
}

/* return 0 when there is really a key event */
static int my_kb_getdata (short *key, short *status)
{
    switch (_last_event.type) {
    case EVENT_KEYDOWN:
        *key = (short)_last_event.value1;
        *status = 1;
        break;

    case EVENT_KEYUP:
        *key = (short)_last_event.value1;
        *status = 0;
        break;

    default:
        return -1;
    }

    memset (&_last_event, 0, sizeof (struct _remote_event));
    return 0;
}

static int my_wait_for_input (struct timeval *timeout)
{
    fd_set rfds;
    int retval;
    int event_flag = 0;

    FD_ZERO (&rfds);
    FD_SET (__mg_usvfb_fd, &rfds);

    retval = select (__mg_usvfb_fd + 1, &rfds, NULL, NULL, timeout);

    if (retval > 0 && FD_ISSET (__mg_usvfb_fd, &rfds)) {
        ssize_t n;
        struct _frame_header header;

        n = read (__mg_usvfb_fd, &header, sizeof (struct _frame_header));
        if (n == 0) {
            _ERR_PRINTF ("my_wait_for_input: socket closed; exit...\n");
            exit (1);
        }
        else if (n != sizeof (struct _frame_header)) {
            _ERR_PRINTF ("my_wait_for_input: socket IO error: %d\n", errno);
            return -1;
        }

        switch (header.type) {
        case FT_PING:
            header.type = FT_PONG;
            header.payload_len = 0;
            n = write (__mg_usvfb_fd, &header, sizeof (struct _frame_header));
            break;

        case FT_EVENT: {
            if (header.payload_len != sizeof (struct _remote_event)) {
                _ERR_PRINTF ("my_wait_for_input: payload length does not matched the data type: %lu.\n", header.payload_len);
                break;
            }

            n = read (__mg_usvfb_fd, &_last_event, sizeof (struct _remote_event));
            switch (_last_event.type) {
            case EVENT_MOUSEMOVE:
            case EVENT_LBUTTONDOWN:
            case EVENT_LBUTTONUP:
                event_flag |= USVFB_MOUSEINPUT;
                break;
            case EVENT_KEYDOWN:
            case EVENT_KEYUP:
                event_flag |= USVFB_KBINPUT;
                break;
            }
            break;
        }
        default:
            _ERR_PRINTF ("my_wait_for_input: FT_PING or FT_EVENT expected, but got type %d.\n", header.type);
            break;
        }
    }
    else if (retval < 0) {
        event_flag = -1;
    }

    return event_flag;
}

/* ----------------------------------------------------------------------- */
static short MOUSEX = 0, MOUSEY = 0, MOUSEBUTTON = 0;
static short KEYCODE = 0, KEYSTATUS = 0;

/************************  Low Level Input Operations **********************/
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = MOUSEX;
    *y = MOUSEY;
}

static int mouse_getbutton (void)
{
    int button = 0;
    if (MOUSEBUTTON == USVFB_MOUSELBUTTON)
        button |= IAL_MOUSE_LEFTBUTTON;
    else if (MOUSEBUTTON == USVFB_MOUSERBUTTON)
        button |= IAL_MOUSE_RIGHTBUTTON;

    return button;
}

static unsigned char kbd_state [NR_KEYS];

static int keyboard_update (void)
{
    if (KEYCODE == 0)
        return 0;

    return KEYCODE + 1;
}

static const char* keyboard_getstate (void)
{
    return (const char*)kbd_state;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int retvalue;

    retvalue = my_wait_for_input (timeout);

    if (retvalue > 0) {
        if (retvalue & USVFB_MOUSEINPUT) {
            if (my_ts_getdata (&MOUSEX, &MOUSEY, &MOUSEBUTTON) == 0)
                retvalue = IAL_MOUSEEVENT;
            else
                retvalue = -1;
        }
        else if (retvalue & USVFB_KBINPUT) {
            if (my_kb_getdata (&KEYCODE, &KEYSTATUS) == 0) {
                if (kbd_state[KEYCODE] == KEYSTATUS) {
                    retvalue = -1;
                }

                kbd_state[KEYCODE] = KEYSTATUS;
                retvalue = IAL_KEYEVENT;
            }
            else
                retvalue = -1;
        }
        else
            retvalue = -1;
    }
    else if (retvalue < 0) {
        retvalue = -1;
    }

    return retvalue;
}

BOOL InitUSVFBInput (INPUT* input, const char* mdev, const char* mtype)
{
    /* input hardware should be initialized before this function is called */

    if (__mg_usvfb_fd < 0)
        return FALSE;

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermUSVFBInput (void)
{
}

#endif /* _MGIAL_USVFB */

