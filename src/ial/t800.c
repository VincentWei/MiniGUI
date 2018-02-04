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
** t800.c: The input engine for MT T800 device.
**
** Author: Wang Jianwei (MT).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _T800_IAL

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/poll.h>

#include "misc.h"
#include "ial.h"
#include "t800.h"

#define KEYBOARD  "/dev/w83c43"

static int mouse_fd = -1, kbd_fd = -1;
static int mouse_x, mouse_y, mouse_button;
static unsigned char state [NR_KEYS];

/************************  Low Level Input Operations **********************/
static int mouse_update (void)
{
    return 0;
}

static void mouse_getxy (int* x, int* y)
{
    *x = mouse_x;
    *y = mouse_y;
}

static int mouse_getbutton (void)
{
    return mouse_button;
}

static int t800_kbd_read (unsigned char *buf, int *modifiers)
{
    int    cc;            /* characters read */
    unsigned char buf1;

    *modifiers = 0;            /* no modifiers yet */
    cc = read (kbd_fd, &buf1, 1);
    
    *buf = buf1;
    if (cc > 0) {
        return 1;
    }

    if ((cc < 0) && (errno != EINTR) && (errno != EAGAIN))
        return -1;

    return 0;
}

static int keyboard_update (void)
{
    unsigned char buf;
    int modifier;
    int ch;
    int is_pressed;
    int retvalue;

    retvalue = t800_kbd_read (&buf, &modifier); 

    if ((retvalue == -1) || (retvalue == 0))
        return 0;
    else {

        is_pressed = !(buf & 0x80);
        ch         = buf & 0x7f;
        if (is_pressed)
            state[ch] = 1;
        else 
            state[ch] = 0;
    }

    return NR_KEYS;
}

static const char* keyboard_getstate (void)
{
    return state;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    fd_set    rfds;
    int    retvalue = 0;
    int    fd, e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_MOUSEEVENT && mouse_fd >= 0) {
        fd = mouse_fd;          /* FIXME: mouse fd may be changed in vt switch! */
        FD_SET (fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }

    if (which & IAL_KEYEVENT && kbd_fd >= 0){
        fd = kbd_fd;          /* FIXME: keyboard fd may be changed in vt switch! */
        FD_SET (kbd_fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }

    /* FIXME: pass the real set size */
#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        fd = mouse_fd;
        /* If data is present on the mouse fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_MOUSEEVENT;
        }

        fd = kbd_fd;
        /* If data is present on the keyboard fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_KEYEVENT;
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitT800Input (INPUT* input, const char* mdev, const char* mtype)
{
    kbd_fd = open(KEYBOARD, O_RDONLY | O_NOCTTY);
    
    if (kbd_fd < 0)
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

    mouse_x = 0;
    mouse_y = 0;
    mouse_button= 0;

    return TRUE;
}

void TermT800Input (void)
{
    if (kbd_fd >= 0) {
        close (kbd_fd);
        kbd_fd = -1;
    }
}

#endif /* _T800_IAL */

