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
** hh44b0.c: Low Level Input Engine for Embest ARM44B0
** 
** Created by Zhang Xinhua, 2005/06/xx.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _EMBEST44B0_IAL

#include <unistd.h>
#include <linux/i2c.h>
#include <fcntl.h>

#define KBD_DEVICE  "/dev/i2c0"

#include <sys/select.h>

#include "ial.h"
#include "hh44b0.h"

#define I2C_SET_DATA_ADDR	0x0601
#define I2C_SET_BUS_CLOCK	0x0602

/* for data reading from /dev/keyboard/0raw */
static int btn_fd = -1;

static unsigned char state[NR_KEYS];
static unsigned char prevkey = 0;
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
    keycode_scancode[KEYPAD_ENTER] =    SCANCODE_ENTER;
    keycode_scancode[KEYPAD_ESCAPE] =   SCANCODE_ESCAPE;
    keycode_scancode[KEYPAD_UP] =       SCANCODE_CURSORBLOCKUP;
    keycode_scancode[KEYPAD_DOWN] =     SCANCODE_CURSORBLOCKDOWN;
    keycode_scancode[KEYPAD_LEFT] =     SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode[KEYPAD_RIGHT] =    SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode[KEYPAD_F1] =    SCANCODE_F1;
    keycode_scancode[KEYPAD_F2] =    SCANCODE_F2;
    keycode_scancode[KEYPAD_F3] =    SCANCODE_F3;
    keycode_scancode[KEYPAD_F4] =    SCANCODE_F4;
    keycode_scancode[KEYPAD_F5] =    SCANCODE_F5;
    keycode_scancode[KEYPAD_F6] =    SCANCODE_F6;
    keycode_scancode[KEYPAD_F7] =    SCANCODE_F7;
    keycode_scancode[KEYPAD_F8] =    SCANCODE_F8;
    keycode_scancode[KEYPAD_F9] =    SCANCODE_F9;
}

static int keyboard_update(void)
{
    if (lastkey) {
        if (lastkey > MAX_KEYPAD_CODE) {
            prevkey = 0;
            printf ("unknown key : %x\n", lastkey);
            return 0;
        }
        state[keycode_scancode[lastkey]] = 1;
    }
    else if (prevkey)
        state[keycode_scancode[prevkey]] = 0;
    else
        return 0;

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
    char key;
    short readkey;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_KEYEVENT) && btn_fd >= 0) {
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
            ioctl(btn_fd, I2C_SET_DATA_ADDR, REG_Sys);
            read(btn_fd, &key, 1);
            if (key & 0x01) {
                ioctl(btn_fd, I2C_SET_DATA_ADDR, REG_Key);
                read(btn_fd, &key, 1);
                if (key > 0) {
                        lastkey = prevkey = key;
                }
                else if (!key) {
                        lastkey = 0;
                }

                retvalue |= IAL_KEYEVENT;
            }
            else
                return 0;
        }
    }
    else if (e < 0)
        return -1;

    return retvalue;
}

BOOL InitEMBEST44b0Input(INPUT * input, const char * mdev, const char * mtype)
{
    btn_fd = open(KBD_DEVICE, O_RDWR | O_NONBLOCK);
    if (btn_fd < 0 ) {
        fprintf (stderr, "EMBEST44B0: Can not open keyboard!\n");
        return FALSE;
    }

    ioctl(btn_fd, I2C_SLAVE_FORCE, ZLG_SLAVE_ADDR);
    ioctl(btn_fd, I2C_SET_BUS_CLOCK, 16*1000);

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

void TermEMBEST44b0Input (void)
{
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _EMBEST44B0_IAL */
