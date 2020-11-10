//////////////////////////////////////////////////////////////////////////////
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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef _MGIAL_SINGLETOUCHKEY

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>

#include "minigui.h"
#include "misc.h"
#include "ial.h"

#include "singletouchkey.h"

#define LEN_TOUCH_DEV     127

static int sg_tp_event_fd = -1, sg_key_event_fd = -1;
static short MOUSEX = 0, MOUSEY = 0, MOUSEBUTTON = 0;

static int get_touch_data (short *x, short *y, short *button)
{
    struct input_event data;

    while (read (sg_tp_event_fd, &data, sizeof (data)) == sizeof (data)) {

        /* return for end of touch data sequece. */
        if (data.type == EV_SYN) {
            switch (data.code) {
            case SYN_REPORT:
            case SYN_MT_REPORT:
                break;
            default:
                _WRN_PRINTF ("unknow event code for EV_SYN event: 0x%x, 0x%x\n", data.code, data.value);
                /* do not update */
                return -1;
            }

            return 0;
        }

        if (data.type == EV_KEY) {
            switch (data.code) {
            case BTN_TOUCH:
                if (data.value > 0)
                    *button = IAL_MOUSE_LEFTBUTTON;
                else
                    *button = 0;
                break;

            default:
                _WRN_PRINTF ("unknow event code for EV_KEY event: 0x%x, 0x%x\n", data.code, data.value);
                return -1;
            }
        }
        else if (data.type == EV_ABS) {
            switch (data.code) {
            case ABS_X:
                *x = data.value;
                break;

            case ABS_Y:
                *y = data.value;
                break;

            case ABS_MT_POSITION_X:
                *x = data.value;
                break;

            case ABS_MT_POSITION_Y:
                *y = data.value;
                break;

            case ABS_MT_TOUCH_MAJOR:
                break;

            case ABS_MT_WIDTH_MAJOR:
                break;

            case ABS_MT_TRACKING_ID:
                break;

            default:
                _WRN_PRINTF ("unknow event code for EV_ABS event: 0x%x, 0x%x.\n", data.code, data.value);
                break;
            }
        }
    }

    return 0;
}

static int mouse_update (void)
{
    if (get_touch_data (&MOUSEX, &MOUSEY, &MOUSEBUTTON) == 0)
        return 1;

    return 0;
}

static void mouse_getxy (int *x, int* y)
{
    *x = MOUSEX;
    *y = MOUSEY;
}

static int mouse_getbutton (void)
{
    if (MOUSEBUTTON == IAL_MOUSE_LEFTBUTTON)
        return IAL_MOUSE_LEFTBUTTON;

    return 0;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int e;
    int retvalue = 0;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    FD_SET (sg_tp_event_fd, &rfds);
    if (sg_tp_event_fd > maxfd) 
        maxfd = sg_tp_event_fd;

    if (sg_key_event_fd >= 0) {
        FD_SET (sg_key_event_fd, &rfds);
        if (sg_key_event_fd > maxfd) 
            maxfd = sg_key_event_fd;
    }

    e = select (maxfd + 1, in, out, except, timeout);
    if (e > 0) {
        if (FD_ISSET (sg_tp_event_fd, &rfds)) {
            retvalue |= IAL_MOUSEEVENT;
        }
        else if (sg_key_event_fd >= 0 && FD_ISSET (sg_key_event_fd, &rfds)) {
            retvalue |= IAL_KEYEVENT;
        }
    }
    else if (e < 0) {
        retvalue = -1;
    }
    else {
        retvalue = 0;
    }

    return retvalue;
}

BOOL ial_InitSingleTouchKey (INPUT* input, const char* mdev, const char* mtype)
{
    char touch_dev[LEN_TOUCH_DEV + 1];
    if (GetMgEtcValue ("singletouchkey", "touch_dev", touch_dev, LEN_TOUCH_DEV) < 0) {
        strcpy (touch_dev, "none");
    }

    if (strcmp (touch_dev, "none") == 0) {
        _WRN_PRINTF ("no touch_dev defined.\n");
        return FALSE;
    }

    sg_tp_event_fd = open (touch_dev, O_RDONLY /*| O_NONBLOCK*/);
    if (sg_tp_event_fd < 0) {
        _WRN_PRINTF ("failed when opening touch event device: %s\n", touch_dev);
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = NULL;
    input->get_keyboard_state = NULL;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermSingleTouchKey (void)
{
    if (sg_tp_event_fd >= 0)
        close (sg_tp_event_fd);

    if (sg_key_event_fd >= 0)
        close (sg_key_event_fd);
}

#endif /* _MGIAL_SINGLETOUCHKEY */

