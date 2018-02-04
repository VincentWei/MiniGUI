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
** fip.c: IAL Engine for EM85xx Front Panel and Remote Controller
**        This IAL engine is made for PLAYER_NEXTBASE.
** 
** Author: Wei Yongming (2003/12/28)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _FIP_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ial.h"
#include "fip.h"

static int fip_fd = -1;
static int mousex, mousey;
static int button;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy(int *x, int *y)
{
    *x = mousex; *y = mousey;
}

static int mouse_getbutton(void)
{
    return button;
}

static unsigned int cur_key;
static unsigned char state [NR_KEYS];

static struct _ir2key {
    unsigned int ir_key;
    unsigned char pc_scancode;
} ir_pckey_map [] =
{
    {PAGEUP_SCANCODE,       SCANCODE_PAGEUP},
    {DOWN_SCANCODE,         SCANCODE_CURSORBLOCKDOWN},
    {ZERO_SCANCODE,         SCANCODE_0},
    {THREED_SCANCODE,       SCANCODE_F1},
    {THREE_SCANCODE,        SCANCODE_3},
    {SEVEN_SCANCODE,        SCANCODE_7},
    {TITLE_SCANCODE,        SCANCODE_F2},
    {TWO_SCANCODE,          SCANCODE_2},
    {ONE_SCANCODE,          SCANCODE_1},
    {LEFT_SCANCODE,         SCANCODE_CURSORBLOCKLEFT},
    {FIVE_SCANCODE,         SCANCODE_5},
    {UP_SCANCODE,           SCANCODE_CURSORBLOCKUP},
    {MUTE_SCANCODE,         SCANCODE_F3},
    {NINE_SCANCODE,         SCANCODE_9},
    {PAUSESTEP_SCANCODE,    SCANCODE_F4},
    {PAGEDOWN_SCANCODE,     SCANCODE_PAGEDOWN},
    {FOUR_SCANCODE,         SCANCODE_4},
    {TAB_SCANCODE,          SCANCODE_TAB},
    {BACKSPACE_SCANCODE,    SCANCODE_BACKSPACE},
    {STOP_SCANCODE,         SCANCODE_F5},
    {EIGHT_SCANCODE,        SCANCODE_8},
    {PLAY_SCANCODE,         SCANCODE_F6},
    {ENTER_SCANCODE,        SCANCODE_ENTER},
    {SIX_SCANCODE,          SCANCODE_6},
    {RIGHT_SCANCODE,        SCANCODE_CURSORBLOCKRIGHT},
    {POWER_SCANCODE,        SCANCODE_F7},
    {ZOOM_SCANCODE,         SCANCODE_F8},
    {FWD_SCANCODE,          SCANCODE_F9},
    {CAPSNUM_SCANCODE,      SCANCODE_F10},
    {NEXT_SCANCODE,         SCANCODE_F11},
    {PREV_SCANCODE,         SCANCODE_F12},
};

static int find_ir_key (unsigned long key)
{
    int a0, a1, a;

    a0 = 0;
    a1 = TABLESIZE (ir_pckey_map) - 1;

    /*
     * binary search for the closest ir key.
     */
    while (a1 - a0 > 1) {
        a = (a0 + a1) / 2;
        if (key > ir_pckey_map [a].ir_key)
            a0 = a;
        else
            a1 = a;
    }

    if (ir_pckey_map [a1].ir_key == key)
        return ir_pckey_map [a1].pc_scancode;
    if (ir_pckey_map [a0].ir_key == key)
        return ir_pckey_map [a0].pc_scancode;

    return -1;
}

#if 0
static struct _fip2key {
    unsigned int fip_key;
    unsigned char pc_scancode;
} fip_pckey_map [] =
{
    {SC_PANEL_RELEASE,      SCANCODE_F1},
    {SC_PANEL_REV,          SCANCODE_F2},
    {SC_PANEL_FWD,          SCANCODE_F3},
    {SC_PANEL_PREV,         SCANCODE_F4},
    {SC_PANEL_NEXT,         SCANCODE_F5},
    {SC_PANEL_EJECT,        SCANCODE_F6},
    {SC_PANEL_PLAY,         SCANCODE_F7},
    {SC_PANEL_STOP,         SCANCODE_F8},
};

static int find_fip_key (unsigned long key)
{
    int i;

    for (i = 0; i < TABLESIZE (fip_pckey_map); i++) {
        if (fip_pckey_map [i].fip_key == key)
            return fip_pckey_map [i].pc_scancode;
    }

    return -1;
}
#endif

static int keyboard_update (void)
{
    int pc_key;

    if (cur_key == 0) {
        memset (state, 0, sizeof(state));
    }
#if 0
    else {
        if (cur_key == CMD_EOF_SCANCODE)
            state [SCANCODE_TAB] = 1;
        else if (cur_key == INVALID_SCANCODE)
            state [SCANCODE_BACKSPACE] = 1;
        else if (cur_key & 0x807f0000) {
            if ((pc_key = find_ir_key (cur_key)) >= 0)
                state [pc_key] = 1;
            else
                return 0;
        }
        else
            if ((pc_key = find_fip_key (cur_key)) >= 0)
                state [pc_key] = 1;
            else
                return 0;
    }
#else
    else {
        if ((pc_key = find_ir_key (cur_key)) >= 0) {
            state [pc_key] = 1;
        }
        else {
            cur_key = 0;
            return 0;
        }
    }
#endif

    return NR_KEYS;
}

static const char * keyboard_get_state(void)
{
    return (char *)state;
}

#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    unsigned int key;

    if (cur_key || button) {
        int e = 0;

#ifdef _LITE_VERSION
        select (maxfd + 1, in, out, except, timeout);
#else
        select (FD_SETSIZE, in, out, except, timeout);
#endif
        if (cur_key) {
            cur_key = 0;
            e = IAL_KEYEVENT;
        }

        if (button) {
            button = 0;
            e |= IAL_MOUSEEVENT;
        }

        return e;
    }

    key = (unsigned int) ioctl (fip_fd, FIP_IOCTL_READ_KEYS, 1000);
    switch (key) {
        case LEFT_SCANCODE:
            mousex -= MOUSE_MOVE_PIXEL;
            if (mousex < 0)
                mousex = 0;
            return IAL_MOUSEEVENT;

        case RIGHT_SCANCODE:
            mousex += MOUSE_MOVE_PIXEL;
            if (mousex > MOUSE_MAX_X)
                mousex = MOUSE_MAX_X;
            return IAL_MOUSEEVENT;

        case UP_SCANCODE:
            mousey -= MOUSE_MOVE_PIXEL;
            if (mousey < 0)
                mousey = 0;
            return IAL_MOUSEEVENT;

        case DOWN_SCANCODE:
            mousey += MOUSE_MOVE_PIXEL;
            if (mousey > MOUSE_MAX_Y)
                mousey = MOUSE_MAX_Y;
            return IAL_MOUSEEVENT;

        case ENTER_SCANCODE:
            button = IAL_MOUSE_LEFTBUTTON;
            return IAL_MOUSEEVENT;

        default:
            if (key) {
                cur_key = key;
                return IAL_KEYEVENT;
            }
            break;
    }

    return 0;
}

BOOL InitFIPInput (INPUT* input, const char* mdev, const char* mtype)
{
    if ((fip_fd = open("/dev/fip", O_RDONLY | O_NONBLOCK)) < 0) {
        printf( "Cannot open /dev/fip (%s)", strerror (errno));
        return FALSE;
    }

    memset (state, 0, sizeof (state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    return TRUE;
}

void TermFIPInput (void)
{
    if (fip_fd >= 0)
        close (fip_fd);
}

#endif /* _FIP_IAL */

