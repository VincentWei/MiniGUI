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
** em85.c: IAL Engine for EM85xx Front Panel and Remote Controller
**        This IAL engine is made for PLAYER_NEXTBASE.
**
** Author: Wei Yongming (2003/12/28)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _EM85_IAL
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ial.h"
#include "em85.h"

extern RECT g_rcScr;

static int fip_fd = -1;
static int mousex, mousey;
static int button;
static int mousestatus;
#define NEC_REPEAT_CODE                 (0xffffffff)

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

static unsigned int cur_key;
static unsigned char state [NR_KEYS];

static struct _ir2key {
    unsigned int ir_key;
    unsigned char pc_scancode;
} ir_pckey_map [] =
{
#if 0
	{NETSETUP_SCANCODE,		SCANCODE_F6},
	{POWER_SCANCODE,		SCANCODE_F9},
	{ONE_SCANCODE,			SCANCODE_1},
	{TWO_SCANCODE,			SCANCODE_2},
	{THREE_SCANCODE,		SCANCODE_3},
	{FOUR_SCANCODE,			SCANCODE_4},
	{FIVE_SCANCODE,			SCANCODE_5},
	{SIX_SCANCODE,			SCANCODE_6},
	{SEVEN_SCANCODE,		SCANCODE_7},
	{EIGHT_SCANCODE,		SCANCODE_8},
	{NINE_SCANCODE,			SCANCODE_9},
	{DOT_SCANCODE,			SCANCODE_PERIOD},
	{THERO_SCANCODE,		SCANCODE_0},
	{BS_SCANCODE,			SCANCODE_BACKSPACE}, //SCANCODE_REMOVE},
	{PAGEUP_SCANCODE,		SCANCODE_F3},
	{PAGEDN_SCANCODE,		SCANCODE_F4},
	{LEFT_SCANCODE,			SCANCODE_CURSORBLOCKLEFT},
	{UP_SCANCODE,			SCANCODE_CURSORBLOCKUP},
	{RIGHT_SCANCODE,		SCANCODE_CURSORBLOCKRIGHT},
	{DOWN_SCANCODE,			SCANCODE_CURSORBLOCKDOWN},
	{ENTER_SCANCODE,		SCANCODE_ENTER},
	{VOLDN_SCANCODE,		SCANCODE_MINUS},
	{VOLUP_SCANCODE,		SCANCODE_EQUAL},
	{FRW_SCANCODE,			SCANCODE_B},
	{PLAY_SCANCODE,			SCANCODE_SPACE},
	{FFW_SCANCODE,			SCANCODE_F},
	{PAUSE_SCANCODE,		SCANCODE_P},
	{STOP_SCANCODE,			SCANCODE_S},
	{MUTE_SCANCODE,			SCANCODE_M},
	{REFRESH_SCANCODE,		SCANCODE_F5},
	{HOME_SCANCODE,			SCANCODE_F1},
	{TRACK_SCANCODE,		SCANCODE_C},
	{WEB_SCANCODE,			SCANCODE_F2},
	{VK_SCANCODE,			SCANCODE_LEFTWIN}, //SCANCODE_F7},
	{OPTION_SCANCODE,		SCANCODE_F7},
#endif
//VCOM IR KEYBOARD
	{VCOM_IRKB_Esc,			SCANCODE_ESCAPE},
	{VCOM_IRKB_1,			SCANCODE_1},
	{VCOM_IRKB_2,			SCANCODE_2},
	{VCOM_IRKB_3,			SCANCODE_3},
	{VCOM_IRKB_4,			SCANCODE_4},
	{VCOM_IRKB_5,			SCANCODE_5},
	{VCOM_IRKB_6,			SCANCODE_6},
	{VCOM_IRKB_7,			SCANCODE_7},
	{VCOM_IRKB_8,			SCANCODE_8},
	{VCOM_IRKB_9,			SCANCODE_9},
	{VCOM_IRKB_0,			SCANCODE_0},
	{VCOM_IRKB_MINUS,		SCANCODE_MINUS},
	{VCOM_IRKB_EQUAL,		SCANCODE_EQUAL},
	{VCOM_IRKB_Backspace,	SCANCODE_BACKSPACE},

	{VCOM_IRKB_Tab,			SCANCODE_TAB},
	{VCOM_IRKB_Q,			SCANCODE_Q},
	{VCOM_IRKB_W,			SCANCODE_W},
	{VCOM_IRKB_E,			SCANCODE_E},
	{VCOM_IRKB_R,			SCANCODE_R},
	{VCOM_IRKB_T,			SCANCODE_T},
	{VCOM_IRKB_Y,			SCANCODE_Y},
	{VCOM_IRKB_U,			SCANCODE_U},
	{VCOM_IRKB_I,			SCANCODE_I},
	{VCOM_IRKB_O,			SCANCODE_O},
	{VCOM_IRKB_P,			SCANCODE_P},

	{VCOM_IRKB_Caps,		SCANCODE_CAPSLOCK},
	{VCOM_IRKB_A,			SCANCODE_A},
	{VCOM_IRKB_S,			SCANCODE_S},
	{VCOM_IRKB_D,			SCANCODE_D},
	{VCOM_IRKB_F,			SCANCODE_F},
	{VCOM_IRKB_G,			SCANCODE_G},
	{VCOM_IRKB_H,			SCANCODE_H},
	{VCOM_IRKB_J,			SCANCODE_J},
	{VCOM_IRKB_K,			SCANCODE_K},
	{VCOM_IRKB_L,			SCANCODE_L},
	{VCOM_IRKB_SEMICOLON,	SCANCODE_SEMICOLON},
	{VCOM_IRKB_Enter,		SCANCODE_ENTER},

	{VCOM_IRKB_Z,			SCANCODE_Z},
	{VCOM_IRKB_X,			SCANCODE_X},
	{VCOM_IRKB_C,			SCANCODE_C},
	{VCOM_IRKB_V,			SCANCODE_V},
	{VCOM_IRKB_B,			SCANCODE_B},
	{VCOM_IRKB_N,			SCANCODE_N},
	{VCOM_IRKB_M,			SCANCODE_M},
	{VCOM_IRKB_COMMA,		SCANCODE_COMMA},
	{VCOM_IRKB_PERIOD,		SCANCODE_PERIOD},
	{VCOM_IRKB_SLASH,		SCANCODE_SLASH},

	{VCOM_IRKB_shift,		SCANCODE_LEFTWIN},  // vkb //SCANCODE_LEFTSHIFT},
	{VCOM_IRKB_ctrl,		SCANCODE_RIGHTWIN}, // ime  //SCANCODE_LEFTCONTROL},
	{VCOM_IRKB_alt,			SCANCODE_MENU}, // status bar //SCANCODE_LEFTALT}, 

	{VCOM_IRKB_Space,		SCANCODE_SPACE},
	{VCOM_IRKB_Del,			SCANCODE_REMOVE},
	{VCOM_IRKB_Up,			SCANCODE_CURSORBLOCKUP},
	{VCOM_IRKB_Down,		SCANCODE_CURSORBLOCKDOWN},
	{VCOM_IRKB_Pgup,		SCANCODE_PAGEUP},
	{VCOM_IRKB_Pgdn,		SCANCODE_PAGEDOWN},
	{VCOM_IRKB_Right,		SCANCODE_CURSORBLOCKRIGHT},
	{VCOM_IRKB_Left,		SCANCODE_CURSORBLOCKLEFT},

	{VCOM_IRKB_F1,			SCANCODE_F1},
	{VCOM_IRKB_F2,			SCANCODE_F2},
	{VCOM_IRKB_F3,			SCANCODE_F3},
	{VCOM_IRKB_F4,			SCANCODE_F4},
	{VCOM_IRKB_F5,			SCANCODE_F5},
	{VCOM_IRKB_F6,			SCANCODE_F6},
	{VCOM_IRKB_F7,			SCANCODE_F7},
	{VCOM_IRKB_F8,			SCANCODE_F8},
	{VCOM_IRKB_F9,			SCANCODE_F9},
	{VCOM_IRKB_F10,			SCANCODE_F10},
	{VCOM_IRKB_F11,			SCANCODE_F11},
};

static int
find_ir_key(unsigned long key)
{
    int i;

    for (i = 0; i < TABLESIZE(ir_pckey_map); i++) {
        if (key == ir_pckey_map[i].ir_key) {
            return ir_pckey_map[i].pc_scancode;
        }
    }

    return -1;
}

static int keyboard_update (void)
{
    int pc_key;
    if (cur_key == 0) {
        memset (state, 0, sizeof(state));
    }
    else {
        if ((pc_key = find_ir_key(cur_key)) >= 0) {
            state [pc_key] = 1;
        }
        else {
            cur_key = 0;
            return 0;
        }
    }

    return NR_KEYS;
}

static const char *
keyboard_get_state(void)
{
    return (char *)state;
}

static int gettime_ms (void)
{
    struct timeval tv1;
    struct timezone tz;
    gettimeofday (&tv1, &tz);
    return tv1.tv_sec * 1000 + tv1.tv_usec/1000;
}

#define LIMIT_KEYSTROKE 0

#if LIMI_KEYSTROKE
static unsigned long g_last_key_time = 0;
#endif

#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    unsigned int key = 0;
    int e = 0;
    fd_set rfds;

    timeout->tv_sec = 0;
    timeout->tv_usec = 10;
    usleep(20000);

    if (cur_key || button) {
#ifdef _LITE_VERSION
        select(maxfd + 1, in, out, except, timeout);
#else
        select(FD_SETSIZE, in, out, except, timeout);
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

    if (!in) {
        in = &rfds;
        FD_ZERO(in);
    }
    FD_SET(fip_fd, in);

#if 1
#ifdef _LITE_VERSION
    if (fip_fd > maxfd)
        maxfd = fip_fd;

    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif
#else
	e = select(fip_fd + 1, in, NULL, NULL, timeout);
#endif

    if (e > 0 && fip_fd >= 0 && FD_ISSET (fip_fd, in)) {
        FD_CLR (fip_fd, in);

        ioctl(fip_fd, IR_IOCTL_READ_KEY, &key);
        //read(fip_fd, &key, 4);
		//printf("got a key under minigui:%d\n", key);
    }
    if (key == NEC_REPEAT_CODE || key == 0)
	return -1;
/*    else
	return 0;

	if (!key)
		return -1;

	printf("..........key: %x\n", key);

    if (key == OPENCLOSE_SCANCODE) {
        chmousestatus();
        return 0;
    }
*/
    if (mousestatus) {
        switch (key) {
        case LEFT_SCANCODE:
        chmousestatus();
            mousex -= MOUSE_MOVE_PIXEL_F;
            if (mousex < 0)
                mousex = 0;
            return IAL_MOUSEEVENT;

        case RIGHT_SCANCODE:
            mousex += MOUSE_MOVE_PIXEL_F;
            if (mousex > MOUSE_MAX_X)
                mousex = MOUSE_MAX_X;
            return IAL_MOUSEEVENT;

        case UP_SCANCODE:
            mousey -= MOUSE_MOVE_PIXEL_F;
            if (mousey < 0)
                mousey = 0;
            return IAL_MOUSEEVENT;

        case DOWN_SCANCODE:
            mousey += MOUSE_MOVE_PIXEL_F;
            if (mousey > MOUSE_MAX_Y)
                mousey = MOUSE_MAX_Y;
            return IAL_MOUSEEVENT;

        case ENTER_SCANCODE:
            button = IAL_MOUSE_LEFTBUTTON;
            return IAL_MOUSEEVENT;
        }
    }
    if (key && key != 0xffffffff) {
		//限制按键频率在300毫秒以上
#if LIMI_KEYSTROKE
		if (g_last_key_time && ((unsigned long)gettime_ms() - g_last_key_time) <= 300){
			printf("minigui lib:key press too quick, ignore\n");
			return -1;
		}
		g_last_key_time = gettime_ms();
#endif
		cur_key = key;
        return IAL_KEYEVENT;
    }

    return -1;
}

BOOL InitEm85Input (INPUT* input, const char* mdev, const char* mtype)
{
    if ((fip_fd = open("/dev/ir", O_RDWR | O_NONBLOCK)) < 0) {
        printf( "Cannot open /dev/ir (%s)", strerror (errno));
        return FALSE;
    }

    printf("InitEm85Input : **************************************\n");
    memset (state, 0, sizeof (state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setxy;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    mouse_setxy((g_rcScr.left+g_rcScr.right)/2,
    (g_rcScr.top+g_rcScr.bottom)/2);

    return TRUE;
}

void TermEm85Input (void)
{
    if (fip_fd >= 0)
        close (fip_fd);
}

#endif /* _EM86_IAL */

