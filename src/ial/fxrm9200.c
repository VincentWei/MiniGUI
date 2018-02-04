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
** fxrm9200.c: Low Level Input Engine for fuxu rm9200.
**         This engine runs on uClinux.
** 
** Created by pan weiguo, 2004/07/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _FXRM9200_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#define KPD_DEVICE  "/dev/keyboard"
#define TPD_DEVICE  "/dev/touchpad"

#include "ial.h"
#include "fxrm9200.h"

/* for data reading from /dev/touchpad */
typedef struct {
    int x;
    int y;
    int status;
} TOUCHINFO;

static int keypd = -1;
static int tspd = -1;

static int mouse_x = 0;
static int mouse_y = 0;
static int leftbutton = 0;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = mouse_x;
    *y = mouse_y;
}

static int mouse_getbutton(void)
{
    return leftbutton;
}

static unsigned char cur_scancode;
static unsigned char simulate_up;
static unsigned char keystate [NR_KEYS];

#define MIN_KPD_SCANCODE    0 
#define MAX_KPD_SCANCODE    15

static unsigned char keypd2pc_map [MAX_KPD_SCANCODE - MIN_KPD_SCANCODE + 1] =
{
    SCANCODE_0,
    SCANCODE_1,
    SCANCODE_2,
    SCANCODE_3,
    SCANCODE_4,
    SCANCODE_5,
    SCANCODE_6,
    SCANCODE_7,
    SCANCODE_8,
    SCANCODE_9,
    SCANCODE_TAB,
    SCANCODE_ENTER,
    SCANCODE_CURSORBLOCKUP,
    SCANCODE_CURSORBLOCKDOWN,
    SCANCODE_CURSORBLOCKLEFT,
    SCANCODE_CURSORBLOCKRIGHT,
};

static int keyboard_update(void)
{
    return NR_KEYS;
}

static const char* keyboard_getspdtate(void)
{
    return (char *)keystate;
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
    int    e = 0;

    /* simulate the release of the key */
    if (cur_scancode) {

#ifdef _LITE_VERSION
        select (maxfd + 1, in, out, except, timeout);
#else
        select (FD_SETSIZE, in, out, except, timeout);
#endif

        if (cur_scancode) {
            keystate [cur_scancode] = 0;
            cur_scancode = 0;
            e |= IAL_KEYEVENT;
        }
        if (simulate_up) {
            leftbutton = 0;
            simulate_up = 0;
            e |= IAL_MOUSEEVENT;
        }

        return e;
    }

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && tspd >= 0) {
        FD_SET (tspd, in);
#ifdef _LITE_VERSION
        if (tspd > maxfd) maxfd = tspd;
#endif
    }
    if ((which & IAL_KEYEVENT) && keypd >= 0){
        FD_SET (keypd, in);
#ifdef _LITE_VERSION
        if (keypd > maxfd) maxfd = keypd;
#endif
    }

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        if (tspd >= 0 && FD_ISSET (tspd, in)) {
            TOUCHINFO touch_info;

            FD_CLR (tspd, in);
            read (tspd, &touch_info, sizeof (TOUCHINFO));
            switch (touch_info.status) {
                case 0:
                    mouse_x = touch_info.x;
                    mouse_y = touch_info.y;
                    leftbutton = IAL_MOUSE_LEFTBUTTON;
                    simulate_up = 1;
                break;

                case 1:
                    mouse_x = touch_info.x;
                    mouse_y = touch_info.y;
                    leftbutton = IAL_MOUSE_LEFTBUTTON;
                break;

                case 2:
                    mouse_x = touch_info.x;
                    mouse_y = touch_info.y;
                    leftbutton = 0;
                break;

                default:
                    return -1;
            }

            retvalue |= IAL_MOUSEEVENT;
        }

        if (keypd >= 0 && FD_ISSET(keypd, in)) {
            unsigned char keypd_scancode;

            FD_CLR (keypd, in);
            read (keypd, &keypd_scancode, sizeof (keypd_scancode));
            if (keypd_scancode >= MIN_KPD_SCANCODE && keypd_scancode <= MAX_KPD_SCANCODE) {
                cur_scancode = keypd2pc_map [keypd_scancode - MIN_KPD_SCANCODE];
                //printf ("keypd_scancode =%d\n", keypd_scancode);
                //printf ("cur_scancode =%d\n", cur_key_scancode);
                if (cur_scancode)
                    keystate [cur_scancode] = 1;

                retvalue |= IAL_KEYEVENT;
            }

        }
    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitRm9200Input (INPUT* input, const char* mdev, const char* mtype)
{
    tspd = open (TPD_DEVICE, O_RDONLY);
    if (tspd < 0) {
        //fprintf (stderr, "UTPMC IAL engine: Can not open TouchPad: %s!\n", TPD_DEVICE);
        //return FALSE;
    }

    keypd = open (KPD_DEVICE, O_RDONLY);
    if (keypd < 0 ) {
        fprintf (stderr, "UTPMC IAL engine: Can not open KeyPad: %s!\n", KPD_DEVICE);
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getspdtate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    return TRUE;
}

void TermRm9200Input (void)
{
    if (tspd >= 0)
        close (tspd);
    if (keypd >= 0)
        close (keypd);
}

#endif /* _FXRM9200_IAL */

