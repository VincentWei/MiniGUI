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
** em8620.c: IAL Engine for EM8620L 
** 
** Author: Zhang Xinhua (2005/10/xx)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _EM8620_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ial.h"
#include "em8620.h"

extern RECT g_rcScr;

static int fip_fd = -1;
static int mousex, mousey;
static int button = 0;
static int mousestatus;

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

static void mouse_setxy(int x, int y)
{
    mousex = x; mousey = y;
}

static inline void chmousestatus (void)
{
    mousestatus ^= 1;
}

static int mouse_getbutton(void)
{
    return button;
}

static int cur_key = -1;
static unsigned char state[NR_KEYS];
static int ir_pckey_map[NR_KEYS + 1];

static void init_keymap(void)
{
    int i;

    for (i = 0; i < NR_KEYS + 1; i++)
        ir_pckey_map[i] = -1;

    ir_pckey_map[RM_ENTER] =                SCANCODE_ENTER;
    ir_pckey_map[RM_HW_ON_OFF] =            SCANCODE_ESCAPE;
    ir_pckey_map[RM_HW_PROGRAM] =           -1;
    ir_pckey_map[RM_HW_UP] =                SCANCODE_CURSORBLOCKUP;
    ir_pckey_map[RM_HW_DOWN] =              SCANCODE_CURSORBLOCKDOWN;
    ir_pckey_map[RM_HW_LEFT] =              SCANCODE_CURSORBLOCKLEFT;
    ir_pckey_map[RM_HW_RIGHT] =             SCANCODE_CURSORBLOCKRIGHT;
    ir_pckey_map[RM_HW_SELECT] =            -1;
    ir_pckey_map[RM_HW_MENU] =              -1;
    ir_pckey_map[RM_HW_TITLE] =             -1;
    ir_pckey_map[RM_HW_OSD] =               -1;
    ir_pckey_map[RM_HW_LANGUAGE] =          -1;
    ir_pckey_map[RM_HW_ANGLE] =             -1;
    ir_pckey_map[RM_HW_SUB_TITLE] =         -1;
    ir_pckey_map[RM_HW_PAUSE] =             -1;
    ir_pckey_map[RM_HW_STOP] =              -1;
    ir_pckey_map[RM_HW_PAUSE_PLAY] =        -1;
    ir_pckey_map[RM_HW_EJECT] =             SCANCODE_F1;
    ir_pckey_map[RM_HW_FAST_REWIND] =       -1;
    ir_pckey_map[RM_HW_FAST_FORWARD] =      -1;
    ir_pckey_map[RM_HW_SLOW_REVERSE] =      -1;
    ir_pckey_map[RM_HW_SLOW_FORWARD] =      -1;
    ir_pckey_map[RM_HW_REPEAT] =            -1;
    ir_pckey_map[RM_HW_AB_REPEAT] =         -1;
    ir_pckey_map[RM_HW_PREV_TRACK] =        SCANCODE_PAGEUP;
    ir_pckey_map[RM_HW_NEXT_TRACK] =        SCANCODE_PAGEDOWN;
    ir_pckey_map[RM_HW_VOL_PLUS] =          -1;
    ir_pckey_map[RM_HW_VOL_MINUS] =         -1;
    ir_pckey_map[RM_HW_MUTE]  =             -1;
    ir_pckey_map[RM_HW_KEY_1] =             SCANCODE_1;
    ir_pckey_map[RM_HW_KEY_2] =             SCANCODE_2;
    ir_pckey_map[RM_HW_KEY_3] =             SCANCODE_3;
    ir_pckey_map[RM_HW_KEY_4] =             SCANCODE_4;
    ir_pckey_map[RM_HW_KEY_5] =             SCANCODE_5;
    ir_pckey_map[RM_HW_KEY_6] =             SCANCODE_6;
    ir_pckey_map[RM_HW_KEY_7] =             SCANCODE_7;
    ir_pckey_map[RM_HW_KEY_8] =             SCANCODE_8;
    ir_pckey_map[RM_HW_KEY_9] =             SCANCODE_9;
    ir_pckey_map[RM_HW_KEY_0] =             SCANCODE_0;
    ir_pckey_map[RM_HW_ENT] =               -1;
    ir_pckey_map[RM_HW_SETUP] =             -1;
    ir_pckey_map[RM_HW_CLEAR] =             -1;
    ir_pckey_map[RM_HW_TVMODE] =            -1; 
    ir_pckey_map[RM_HW_PBC] =               -1;
    ir_pckey_map[RM_HW_RETURN] =            -1;
    ir_pckey_map[RM_HW_SHUFFLE] =           -1;
    ir_pckey_map[RM_HW_SEARCH] =            -1;
    ir_pckey_map[RM_HW_ZOOM] =              SCANCODE_EQUAL;
}

static int irvalue_key(unsigned int ir)
{
    switch (ir) {
        case 0xea15e608:
			return RM_ENTER;

		case 0xbc43e608:
		case 0xED12FF00:
			return RM_HW_ON_OFF;

		case 0xa15ee608:
		case 0xB649FF00:
			return RM_HW_PROGRAM;

		case 0xeb14e608:
		case 0xFE01FF00:
			return RM_HW_UP;

		case 0xe916e608:
		case 0xF609FF00:
			return RM_HW_DOWN;

		case 0xe21de608:
		case 0xE619FF00:
			return RM_HW_LEFT;

		case 0xee11e608:
		case 0xEE11FF00:
			return RM_HW_RIGHT;

		//case 0xea15e608:
		case 0xBF40FF00:
			return RM_HW_SELECT;

		case 0xed12e608:
		case 0xE01FFF00:
			return RM_HW_MENU;

		case 0xa55ae608:
		case 0xE11EFF00:
			return RM_HW_TITLE;

		case 0xa25de608:
		case 0xE718FF00:
			return RM_HW_OSD;

		case 0xad52e608:
		case 0xFC03FF00:
			return RM_HW_LANGUAGE;

		case 0xa45be608:
		case 0xEB14FF00:
			return RM_HW_ANGLE;

		case 0xa956e608:
		case 0xE21DFF00:
			return RM_HW_SUB_TITLE;

		case 0xe817e608:
		case 0xE817FF00:
			return RM_HW_PAUSE;

		case 0xab54e608:
		case 0xFB04FF00:
			return RM_HW_STOP;

		case 0xaa55e608:
		case 0xF807FF00:
			return RM_HW_PAUSE_PLAY;

		case 0xb847e608:
		case 0xE916FF00:
			return RM_HW_EJECT;

		case 0xa758e608:
		case 0xF00FFF00:
			return RM_HW_FAST_REWIND;

		case 0xaf50e608:
		case 0xF10EFF00:
			return RM_HW_FAST_FORWARD;

		case 0xa659e608:
			return RM_HW_SLOW_REVERSE;

		case 0xae51e608:
			return RM_HW_SLOW_FORWARD;

		case 0xa35ce608:
			return RM_HW_REPEAT;

		case 0xe01fe608:
			return RM_HW_AB_REPEAT;

		case 0xe41be608:
		case 0xFD02FF00:
			return RM_HW_PREV_TRACK;

		case 0xec13e608:
		case 0xF50AFF00:
			return RM_HW_NEXT_TRACK;

		case 0xe718e608:
		case 0xF20DFF00:
			return RM_HW_VOL_PLUS;

		case 0xef10e608:
		case 0xEC13FF00:
			return RM_HW_VOL_MINUS;

		case 0xe619e608:
		case 0xE31CFF00:
			return RM_HW_MUTE;

		case 0xbd42e608:
		case 0xBE41FF00:
			return RM_HW_KEY_1;

		case 0xbe41e608:
		case 0xB24DFF00:
			return RM_HW_KEY_2;

		case 0xbf40e608:
		case 0xBA45FF00:
			return RM_HW_KEY_3;

		case 0xb946e608:
		case 0xBD42FF00:
			return RM_HW_KEY_4;

		case 0xba45e608:
		case 0xB14EFF00:
			return RM_HW_KEY_5;

		case 0xbb44e608:
		case 0xB946FF00:
			return RM_HW_KEY_6;

		case 0xb54ae608:
		case 0xBC43FF00:
			return RM_HW_KEY_7;

		case 0xb649e608:
		case 0xB04FFF00:
			return RM_HW_KEY_8;

		case 0xb748e608:
		case 0xB847FF00:
			return RM_HW_KEY_9;

		case 0xb24de608:
		case 0xFF00FF00:
			return RM_HW_KEY_0;

		case 0xb34ce608:
		case 0xF30CFF00:
			return RM_HW_ENT;

		case 0xe51ae608:
			return RM_HW_SETUP;

		case 0xb14ee608:
			return RM_HW_CLEAR;

		case 0xb44be608:
			return RM_HW_TVMODE;

		case 0xe31ce608:
			return RM_HW_PBC;

		case 0xe11ee608:
			return RM_HW_RETURN;

		case 0xa05fe608:
			return RM_HW_SHUFFLE;

		case 0xa857e608:
			return RM_HW_SEARCH;

		case 0xac53e608:
			return RM_HW_ZOOM;

        case 0x0:
        case 0x10:
        case NEC_REPEAT_CODE:
            return -1;

		default: 
			return RM_HW_TIMEOUT;
		}
}

static int keyboard_update(void)
{
    if (cur_key < 0)
        memset(state, 0, sizeof(state));
    else
        state[cur_key] = 1;

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
    unsigned int fkey;
    int key, e = 0;
    fd_set rfds;

    if (cur_key >= 0 || button) {
#ifdef _LITE_VERSION
        select(maxfd + 1, in, out, except, timeout);
#else
        select(FD_SETSIZE, in, out, except, timeout);
#endif
        if (cur_key >= 0) {
            cur_key = -1;
            e = IAL_KEYEVENT;
        }

        if (button) {
            button = 0;
            e |= IAL_MOUSEEVENT;
        }

        return e;
    }

    if (!in) {
        in = &rfds;
        FD_ZERO(in);
    }
    FD_SET(fip_fd, in);

#ifdef _LITE_VERSION
    if (fip_fd > maxfd)
        maxfd = fip_fd;

    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (e > 0 && fip_fd >= 0 && FD_ISSET(fip_fd, in)) {
        FD_CLR (fip_fd, in);
        read(fip_fd, &fkey, 4);
    }

    key = irvalue_key(fkey);
    if (key < 0) {
        cur_key = -1;
        return -1;
    }
    else if (key == RM_HW_RETURN) {
        chmousestatus();
        return 0;
    }
    else if (key == RM_HW_TIMEOUT)
        return 0;

    cur_key = ir_pckey_map[key];
    if (cur_key < 0) {
        cur_key = -1;
        return -1;
    }

    if (mousestatus) {
        switch (cur_key) {
        case SCANCODE_CURSORBLOCKLEFT:
            mousex -= MOUSE_MOVE_PIXEL_F;
            if (mousex < 0)
                mousex = 0;
            goto mouse_event;

        case SCANCODE_CURSORBLOCKRIGHT:
            mousex += MOUSE_MOVE_PIXEL_F;
            if (mousex > MOUSE_MAX_X)
                mousex = MOUSE_MAX_X;
            goto mouse_event;

        case SCANCODE_CURSORBLOCKUP:
            mousey -= MOUSE_MOVE_PIXEL_F;
            if (mousey < 0)
                mousey = 0;
            goto mouse_event;

        case SCANCODE_CURSORBLOCKDOWN:
            mousey += MOUSE_MOVE_PIXEL_F;
            if (mousey > MOUSE_MAX_Y)
                mousey = MOUSE_MAX_Y;
            goto mouse_event;

        case SCANCODE_ENTER:
            button = IAL_MOUSE_LEFTBUTTON;
            goto mouse_event;
        }
    }

    return IAL_KEYEVENT;

mouse_event:
    cur_key = -1;
    return IAL_MOUSEEVENT;
}

BOOL InitEm8620Input(INPUT * input, const char * mdev, const char * mtype)
{
    if ((fip_fd = open("/dev/ir", O_RDONLY | O_NONBLOCK)) < 0) {
        printf( "Cannot open /dev/fip (%s)", strerror (errno));
        return FALSE;
    }

    memset(state, 0, sizeof(state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setxy;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    mouse_setxy( (g_rcScr.left + g_rcScr.right) / 2,
            (g_rcScr.top + g_rcScr.bottom) / 2);

    init_keymap();
    

    return TRUE;
}

void TermEm8620Input (void)
{
    if (fip_fd >= 0)
        close (fip_fd);
}

#endif /* _EM8620_IAL */

