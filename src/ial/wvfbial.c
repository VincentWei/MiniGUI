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
** wvfb.c: Input Engine for Windows Virtual FrameBuffer
** 
** Created by snig, 2004/6/22
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "misc.h"
#include "ial.h"


#ifdef _MGIAL_WVFB

#include "wvfb.h"
#include "winial.h"

int win_IALInit (void);
void win_IALClose (void);

//int win_EventDataAvailable (void);
int win_GetEventData (void* data, size_t len);

#define NOBUTTON        0x0000
#define LEFTBUTTON      0x0001
#define RIGHTBUTTON     0x0002
#define MIDBUTTON       0x0004
#define MOUSEBUTTONMASK 0x00FF

#define SHIFTBUTTON     0x0100
#define CONTROLBUTTON   0x0200
#define ALTBUTTON       0x0400
#define METABUTTON      0x0800
#define KEYBUTTONMASK   0x0FFF
#define KEYPAD          0x4000

static POINT mouse_pt;
static int mouse_buttons;
static WVFbKeyData kbd_data;

static unsigned char kbd_state [NR_KEYS];

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_pt.x;
    *y = mouse_pt.y;
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (mouse_buttons & LEFTBUTTON)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (mouse_buttons & RIGHTBUTTON)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (mouse_buttons & MIDBUTTON)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;
}

/*
 * Key operations -- Event
 */

static unsigned char keycode_to_scancode (unsigned char keycode)
{
    if (keycode == 72)
        keycode = SCANCODE_CURSORBLOCKUP;
    else if (keycode == 80)
        keycode = SCANCODE_CURSORBLOCKDOWN;
    else if (keycode == 75)
        keycode = SCANCODE_CURSORBLOCKLEFT;
    else if (keycode == 77)
        keycode = SCANCODE_CURSORBLOCKRIGHT;
    return keycode;
}

static int keyboard_update (void)
{
    unsigned char scancode;
    static unsigned char last = 0;

#if 0
    printf ("key info: (0x%x(0x%x), %x, %s, %s)\n", kbd_data.keycode, 
                    HIWORD(kbd_data.keycode) & 0x00FF, kbd_data.keystate, 
                    kbd_data.press?"Pressed":"Released",
                    kbd_data.repeat?"repeat":"no repete");
#endif

    if (kbd_data.repeat > 1) {
        fprintf (stderr, "key repeat too much.\n");
        return 0;
    }

    if (kbd_data.keycode == 0 && !kbd_data.press) {
        kbd_state [last] = 0;
    }
    else {
        scancode = keycode_to_scancode (kbd_data.keycode);
        //printf ("scancode = %d\n", scancode);
        kbd_state [scancode] = kbd_data.press;
        if (kbd_data.press)
            last = scancode;
    }

    return NR_KEYS;
}

static const char* keyboard_getstate (void)
{
    return kbd_state;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int retvalue = 0;
    WVFbEventData event_data;

    if (win_GetEventData (&event_data, sizeof(event_data)) <= 0)
        return 0;

    if (which & IAL_MOUSEEVENT && event_data.event_type == 0) {
        /* If data is present on the mouse, service it: */
        mouse_pt.x = event_data.mousedata.x;
        mouse_pt.y = event_data.mousedata.y;
        mouse_buttons = event_data.mousedata.btn;
        retvalue |= IAL_MOUSEEVENT;
    }

    if (which & IAL_KEYEVENT && event_data.event_type == 1){
        /* If data is present on the keyboard, service it: */
        kbd_data = event_data.kbdata;
        retvalue |= IAL_KEYEVENT;
    }

    return retvalue;
}

BOOL InitWVFBInput (INPUT* input, const char* mdev, const char* mtype)
{
    if (win_IALInit () < 0) {
        fprintf (stderr, "WVFB IAL engine: can not open pipe.\n");
        return FALSE;
    }

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

void TermWVFBInput (void)
{
    win_IALClose ();
}

#endif /* _MGIAL_WVFB */

