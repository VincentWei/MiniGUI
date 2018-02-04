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
** arm3000.c: Low Level Input Engine for UP-NETARM3000
** 
** Created by Yao Yunyuan, 2004/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _ARM3000_IAL

#include <unistd.h>
#include <fcntl.h>

#define TS_DEVICE   "/dev/touchscreen/0raw"
#define KBD_DEVICE  "/dev/keyboard/0raw"

#include <sys/select.h>

#include "ial.h"
#include "arm3000.h"

/* for data reading from /dev/keyboard/0raw */
typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

static int ts = -1;
static int btn_fd = -1;

static unsigned char state[NR_KEYS];
static unsigned char btn_state=0;
static unsigned char keycode_scancode[MAX_KEYPAD_CODE + 1];
static int numlock = 0;

static int mousex = 0;
static int mousey = 0;
static POS pos;

#undef _DEBUG

/************************  Low Level Input Operations **********************/
/*
 *  Mouse operations -- Event
 */

static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return pos.b;
}

static void init_code_map_nonumlock(void)
{
    keycode_scancode[KEYPAD_MINUS] = SCANCODE_KEYPADMINUS;
    keycode_scancode[KEYPAD_ENTER] = SCANCODE_KEYPADENTER;
    keycode_scancode[KEYPAD_NUMLOCK] = SCANCODE_NUMLOCK;
    keycode_scancode[KEYPAD_DIVIDE] = SCANCODE_KEYPADDIVIDE;
    keycode_scancode[KEYPAD_PLUS] = SCANCODE_KEYPADPLUS;
    keycode_scancode[KEYPAD_MULTIPLY] = SCANCODE_KEYPADMULTIPLY;
    keycode_scancode[KEYPAD_PERIOD] = SCANCODE_KEYPADPERIOD;
    keycode_scancode[KEYPAD_0] = SCANCODE_INSERT;
    keycode_scancode[KEYPAD_1] = SCANCODE_END;
    keycode_scancode[KEYPAD_2] = SCANCODE_CURSORBLOCKDOWN;
    keycode_scancode[KEYPAD_3] = SCANCODE_PAGEDOWN;
    keycode_scancode[KEYPAD_4] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode[KEYPAD_5] = SCANCODE_KEYPAD5;
    keycode_scancode[KEYPAD_6] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode[KEYPAD_7] = SCANCODE_HOME;
    keycode_scancode[KEYPAD_8] = SCANCODE_CURSORBLOCKUP;
    keycode_scancode[KEYPAD_9] = SCANCODE_PAGEUP;
}

static void init_code_map_numlock(void)
{
    keycode_scancode[KEYPAD_MINUS] = SCANCODE_KEYPADMINUS;
    keycode_scancode[KEYPAD_ENTER] = SCANCODE_KEYPADENTER;
    keycode_scancode[KEYPAD_NUMLOCK] = SCANCODE_NUMLOCK;
    keycode_scancode[KEYPAD_DIVIDE] = SCANCODE_KEYPADDIVIDE;
    keycode_scancode[KEYPAD_PLUS] = SCANCODE_KEYPADPLUS;
    keycode_scancode[KEYPAD_MULTIPLY] = SCANCODE_KEYPADMULTIPLY;
    keycode_scancode[KEYPAD_PERIOD] = SCANCODE_KEYPADPERIOD;
    keycode_scancode[KEYPAD_0] = SCANCODE_KEYPAD0;
    keycode_scancode[KEYPAD_1] = SCANCODE_KEYPAD1;
    keycode_scancode[KEYPAD_2] = SCANCODE_KEYPAD2;
    keycode_scancode[KEYPAD_3] = SCANCODE_KEYPAD3;
    keycode_scancode[KEYPAD_4] = SCANCODE_KEYPAD4;
    keycode_scancode[KEYPAD_5] = SCANCODE_KEYPAD5;
    keycode_scancode[KEYPAD_6] = SCANCODE_KEYPAD6;
    keycode_scancode[KEYPAD_7] = SCANCODE_KEYPAD7;
    keycode_scancode[KEYPAD_8] = SCANCODE_KEYPAD8;
    keycode_scancode[KEYPAD_9] = SCANCODE_KEYPAD9;
}

static int keyboard_update(void)
{
    int status;
    int key;

    status = (btn_state & KEY_RELEASED)? 1 : 0;

    key = btn_state & (~KEY_RELEASED);
    if (key > MAX_KEYPAD_CODE) {
        printf ("unknown key : %x\n", key);
        return 0;
    }

    if (key == 0x04 && status == 1)
        numlock = (numlock + 1) % 2;

//    fprintf(stderr, "key %d is %s\n", key, (btn_state & KEY_RELEASED)? "DOWN":"UP");
    if (numlock == 1)
        init_code_map_numlock();
    else
        init_code_map_nonumlock();

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
                pos.x = data[1];
                pos.y = data[2];
//printf ("posx = %d, posy = %d\n", pos.x, pos.y);
                mousex = (pos.x - 240) * 320 / (1728 - 240);
                mousey = (pos.y - 1869) * 240 / (211 - 1869);
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

BOOL InitARM3000Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY | O_NONBLOCK);
    if (ts < 0) {
        fprintf (stderr, "ARM3000: Can not open touch screen!\n");
        return FALSE;
    }

    btn_fd = open (KBD_DEVICE, O_RDONLY | O_NONBLOCK);
    if (btn_fd < 0 ) {
        fprintf (stderr, "ARM3000: Can not open keyboard!\n");
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
    init_code_map_nonumlock();

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    pos.x = pos.y = pos.b = 0;
    
    return TRUE;
}

void TermARM3000Input (void)
{
    if (ts >= 0)
        close(ts);
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _ARM3000_IAL */
