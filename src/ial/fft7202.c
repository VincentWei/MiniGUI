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
** fft7202.c: Low Level Input Engine for FFT 7202
** 
** The device driver of fft 7202 touch screen is compliant to
** the Specification for Generic Touch Screen Driver.
** Please see the file: spec_general_ts_drv.html in this directory.
**
** So this IAL engine can be a good template of your new IAL engines,
** which compliant to the specification.
**
** Created by Yao yunyuan, 2001/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _FFT7202_IAL

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#define TS_DEVICE   "/dev/ts"

#include "ial.h"
#include "fft7202.h"

/* for data reading from /dev/ts */
typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

static int ts = -1;
static int mousex = 0;
static int mousey = 0;
static POS pos;

#undef _DEBUG

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
#ifdef _DEBUG
    printf ("mousex = %d, mousey = %d\n", mousex, mousey);
#endif

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
            }

            if (data[0]) {
                printf ("mouse down: pos.x = %d, pos.y = %d\n", pos.x, pos.y);
            }

            pos.b = (data[0] ? IAL_MOUSE_LEFTBUTTON : 0);
            retvalue |= IAL_MOUSEEVENT;
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitFFTInput (INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (TS_DEVICE, O_RDONLY);
    if (ts < 0) {
        fprintf (stderr, "IPAQ: Can not open touch screen!\n");
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

void TermFFTInput (void)
{
    if (ts >= 0)
        close(ts);
}

#endif /* _FFT7202_IAL */

