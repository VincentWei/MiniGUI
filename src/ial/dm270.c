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
** dm270.c: Low Level Input Engine for DigiPro DM270.
** 
** Created by Yao Yunyuan, 2004/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _DM270_IAL

#include <unistd.h>
#include <fcntl.h>

#define KBD_DEVICE  "/dev/dm270keypad"

#include <sys/select.h>

#include "ial.h"
#include "dm270.h"

/* for data reading from /dev/keyboard/0raw */
static int btn_fd = -1;

static unsigned char state[NR_KEYS];
static unsigned char btn_state = 0;
static unsigned char keycode_scancode[MAX_KEYPAD_CODE + 1];
static unsigned char lastkey = 0;

#undef _DEBUG
/***************************************/

static int mouse_update (void)
{
    return 0;
}

static void mouse_getxy(int *x, int *y)
{
    *x = 0; *y = 0;
}

static int mouse_getbutton(void)
{
    return 0;
}

/************************  Low Level Input Operations **********************/
/*
 *  Mouse operations -- Event
 */
static void init_code_map(void)
{
    keycode_scancode[KEYPAD_OK] = SCANCODE_KEYPADENTER;
    keycode_scancode[KEYPAD_CANCEL] = SCANCODE_ESCAPE;
    keycode_scancode[KEYPAD_DOWN] = SCANCODE_CURSORBLOCKDOWN;
    keycode_scancode[KEYPAD_LEFT] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode[KEYPAD_RIGHT] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode[KEYPAD_UP] = SCANCODE_CURSORBLOCKUP;
}

static int keyboard_update(void)
{
    int status;
    int key;

    if (btn_state)
        status = 1;
    else {
        state[keycode_scancode[lastkey]] = 0;
        lastkey = 0;
        return NR_KEYS;
    }

    lastkey = btn_state;

    key = btn_state & (~KEY_RELEASED);
    if (key > MAX_KEYPAD_CODE) {
        printf ("unknown key : %x\n", key);
        return 0;
    }

    state[keycode_scancode[key]] = status;

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
    int    e;
    short readkey;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
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
        if (btn_fd >= 0 && FD_ISSET(btn_fd, in)) {
            FD_CLR(btn_fd, in);
            read(btn_fd, &readkey, sizeof(readkey));
            if (readkey) {
                btn_state = readkey;
                lastkey = btn_state;
                retvalue |= IAL_KEYEVENT;
            }
            else if (lastkey != 0) {
                btn_state = 0;
                retvalue |= IAL_KEYEVENT;
            }
            else
                return 0;
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitDM270Input (INPUT* input, const char* mdev, const char* mtype)
{
    btn_fd = open (KBD_DEVICE, O_RDWR | O_NONBLOCK);
    if (btn_fd < 0 ) {
        fprintf (stderr, "DM270: Can not open keyboard!\n");
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
    init_code_map();

    input->wait_event = wait_event;
    return TRUE;
}

void TermDM270Input (void)
{
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _DM270_IAL */
