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
** skyeye-ep7312.c: Low Level Input Engine for touch screen of 
**      SkyEye EP7312 simulation
** 
** Created by Wei Yongming, 2005/03/10
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _SKYEYE_EP7312_IAL

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "misc.h"
#include "ial.h"
#include "skyeye-ep7312.h"

/* file descriptor for touch panel */
static int ts = -1;
static int pen_down = 0; /* pen down */

static int mousex = 79, mousey = 79;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    struct ts_pen_info pen_info;
    int bytes_read;

    if (ts < 0)
        return 0;

    bytes_read = read (ts, &pen_info, sizeof (pen_info));

    if (bytes_read != sizeof (pen_info)) {
        return 0;
    }

    switch(pen_info.event) {
    case EV_PEN_UP:
        pen_down = 0;
        mousex = pen_info.x;
        mousey = pen_info.y;
        break;

    case EV_PEN_DOWN:
        pen_down = IAL_MOUSE_LEFTBUTTON;
        mousex = pen_info.x;
        mousey = pen_info.y;
        break;

    case EV_PEN_MOVE:
        pen_down = IAL_MOUSE_LEFTBUTTON;
        mousex = pen_info.x;
        mousey = pen_info.y;
        break;
    }

    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton (void)
{
    return pen_down;
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
            FD_CLR (ts, in);
            retvalue |= IAL_MOUSEEVENT;
        }
    }
    else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitSkyEyeEP7312Input (INPUT* input, const char* mdev, const char* mtype)
{
    int err;
    struct ts_drv_params  drv_params;
    int mx1, mx2, my1, my2;
    int ux1, ux2, uy1, uy2;

     /*
     * open up the touch-panel device.
     * Return the fd if successful, or negative if unsuccessful.
     */

    ts = open("/dev/ts", O_NONBLOCK | O_RDWR);
    if (ts < 0) {
        fprintf (stderr, "Error %d opening touch panel\n", errno);
        return FALSE;
    }
    
	// ywc 2004-07-05
    err = ioctl (ts, TS_PARAMS_GET, &drv_params);
    if (err == -1) {
        close (ts);
        return FALSE;
    }

    drv_params.version_req    = EP7312TS_VERSION;
    drv_params.event_queue_on = 1;
    drv_params.sample_ms      = 10;
    drv_params.deglitch_on    = 1;
    drv_params.follow_thrs    = 0;
    drv_params.mv_thrs        = 2;
    drv_params.y_max          = 159 + 66;  // to allow scribble area
    drv_params.y_min          = 0;
    drv_params.x_max          = 159;
    drv_params.x_min          = 0;
    drv_params.xy_swap        = 0;

    /*
     * according to mc68328digi.h 'How to calculate the parameters', 
     * we have measured:
     */
    mx1 = 508; ux1 =   0;
    my1 = 508; uy1 =   0;
    mx2 = 188; ux2 = 159;
    my2 = 188; uy2 = 159;

    /*
     * now calculate the params:
     */
    drv_params.x_ratio_num    = ux1 - ux2;
    drv_params.x_ratio_den    = mx1 - mx2;
    drv_params.x_offset       =
    ux1 - mx1 * drv_params.x_ratio_num / drv_params.x_ratio_den;

    drv_params.y_ratio_num    = uy1 - uy2;
    drv_params.y_ratio_den    = my1 - my2;
    drv_params.y_offset       =
    uy1 - my1 * drv_params.y_ratio_num / drv_params.y_ratio_den;
   	//ywc 2004-07-05
    err = ioctl (ts, TS_PARAMS_SET, &drv_params);
    if (err == -1) {
        close (ts);
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = NULL;
    input->get_keyboard_state = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    return TRUE;
}

void TermSkyEyeEP7312Input (void)
{
    if (ts >= 0)
        close (ts);
    ts = -1;
}

#endif /* _SKYEYE_EP7312_IAL */

