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
** svpxx.c: ial engine for svpxx.
**
** Current maintainer: <liu jianjun>
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _SVPXX_IAL

#include "ial.h"
#include "svpxx.h"

static int mouse_x, mouse_y, mouse_button;

typedef struct tagPOS
{
    short x;
    short y;
    short b;
} POS;

#define KEYPAD_QUALITY      0x00
#define KEYPAD_PRIVACY      0x01
#define KEYPAD_PIP          0x02
#define KEYPAD_VIDEO        0x03
#define KEYPAD_SNAPSHOT     0x10
#define KEYPAD_PH_BOOK      0x11
#define KEYPAD_MENU         0x12
#define KEYPAD_UP           0x13
#define KEYPAD_CHANNEL      0x20
#define KEYPAD_TIMER        0x21
#define KEYPAD_ENTER        0x22
#define KEYPAD_DOWN         0x23
#define KEYPAD_LEFT         0x30
#define KEYPAD_RIGHT        0x31
#define KEYPAD_AUTO_ANSWER  0x32
#define KEYPAD_SPEAKER      0x33
#define KEYPAD_HISTORY      0x17
#define KEYPAD_REDIAL       0x37
#define KEYPAD_0            0x35
#define KEYPAD_1            0x04
#define KEYPAD_2            0x05
#define KEYPAD_3            0x06
#define KEYPAD_4            0x14
#define KEYPAD_5            0x15
#define KEYPAD_6            0x16
#define KEYPAD_7            0x24
#define KEYPAD_8            0x25
#define KEYPAD_9            0x26
#define KEYPAD_STAR         0x34
#define KEYPAD_POND         0x36

#define MAX_KEYPAD_CODE     0x037

static unsigned char state[NR_KEYS];

static unsigned char keycode_scancode[MAX_KEYPAD_CODE + 1];

static void init_code_map (void)
{
    keycode_scancode[KEYPAD_QUALITY] = SCANCODE_F1;
    keycode_scancode[KEYPAD_PRIVACY] = SCANCODE_F2;
    keycode_scancode[KEYPAD_PIP] = SCANCODE_F3;
    keycode_scancode[KEYPAD_VIDEO] = SCANCODE_F4;
    keycode_scancode[KEYPAD_SNAPSHOT] = SCANCODE_F5;
    keycode_scancode[KEYPAD_PH_BOOK] = SCANCODE_F6;
    keycode_scancode[KEYPAD_MENU] = SCANCODE_F7;
    keycode_scancode[KEYPAD_UP] = SCANCODE_CURSORUP;
    keycode_scancode[KEYPAD_CHANNEL] = SCANCODE_F8;
    keycode_scancode[KEYPAD_TIMER] = SCANCODE_F9;
    keycode_scancode[KEYPAD_ENTER] = SCANCODE_ENTER;
    keycode_scancode[KEYPAD_DOWN] = SCANCODE_CURSORDOWN;
    keycode_scancode[KEYPAD_LEFT] = SCANCODE_CURSORLEFT;
    keycode_scancode[KEYPAD_RIGHT] = SCANCODE_CURSORRIGHT;
    keycode_scancode[KEYPAD_AUTO_ANSWER] = SCANCODE_F10;
    keycode_scancode[KEYPAD_SPEAKER] = SCANCODE_F11;
    keycode_scancode[KEYPAD_HISTORY] = SCANCODE_F12;
    keycode_scancode[KEYPAD_REDIAL] = SCANCODE_R;
    keycode_scancode[KEYPAD_0] = SCANCODE_0;
    keycode_scancode[KEYPAD_1] = SCANCODE_1;
    keycode_scancode[KEYPAD_2] = SCANCODE_2;
    keycode_scancode[KEYPAD_3] = SCANCODE_3;
    keycode_scancode[KEYPAD_4] = SCANCODE_4;
    keycode_scancode[KEYPAD_5] = SCANCODE_5;
    keycode_scancode[KEYPAD_6] = SCANCODE_6;
    keycode_scancode[KEYPAD_7] = SCANCODE_7;
    keycode_scancode[KEYPAD_8] = SCANCODE_8;
    keycode_scancode[KEYPAD_9] = SCANCODE_9;
    keycode_scancode[KEYPAD_STAR] = SCANCODE_S;
    keycode_scancode[KEYPAD_POND] = SCANCODE_P;
}

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 0;
}

static void mouse_getxy (int *x, int *y)
{
    *x = 0;
    *y = 0;
}

static int mouse_getbutton (void)
{
    return 0;
}

static unsigned char cur_key;

static int keyboard_update (void)
{
    if (cur_key)
        state [cur_key] = 1;
    else
        memset (state, 0, sizeof(state));

    return NR_KEYS;
}

static const char * keyboard_get_state (void)
{
    return (char *) state;
}

static int wait_event (int which, fd_set * in, fd_set * out, fd_set * except,
        struct timeval *timeout)
{
    int e;

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (cur_key) {
        if (cur_key) {
            cur_key = 0;
            return IAL_KEYEVENT;
        }
    }
    else {
        unsigned char key;

        if ( PollingKeyPadIR ( &key ) ) {
            if ( key > MAX_KEYPAD_CODE ){
                printf("unknown key : %x\n",key);
                return 0;
            }
            if (keycode_scancode [key]) {
                cur_key = keycode_scancode [key];
                return IAL_KEYEVENT;
            }
        }
    }

    return 0;
}


BOOL InitSvpxxInput (INPUT * input, const char *mdev, const char *mtype)
{
    if (KeyPadIRInit ()) {
        fprintf (stderr, "SVPXX IAL ENGINE: Can not open svp keypad!\n");
        return FALSE;
    }

    printf ("SVPXX IAL ENGINE: keypad Init ok!\n");

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    mouse_x = 0;
    mouse_y = 0;
    mouse_button = 0;

    init_code_map ();

    return TRUE;
}

void TermSvpxxInput (void)
{
}

#endif /* _SVPXX_IAL */
