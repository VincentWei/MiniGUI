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
** embest2410.c: Low Level Input Engine for EMBEST ARM2410
** 
** Created by Cheng Jiangang, 2004/09/22
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _EMBEST2410_IAL

#include <unistd.h>
#include <fcntl.h>

#define TS_DEVICE   "/dev/touchscreen/0raw"

#include <sys/select.h>

#include "ial.h"
#include "embest2410.h"

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
printf ("posx = %d, posy = %d\n", pos.x, pos.y);
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

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitEMBEST2410Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY | O_NONBLOCK);
    if (ts < 0) {
        fprintf (stderr, "EMBEST2410: Can not open touch screen!\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    pos.x = pos.y = pos.b = 0;
    
    return TRUE;
}

void TermEMBEST2410Input (void)
{
    if (ts >= 0)
        close(ts);
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif /* _EMBEST2410_IAL */
