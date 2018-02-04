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
** utpmc.c: IAL Engine for UTPMC board.
** 
** Author: Zhong Shuyi (2003/12/28)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _UTPMC_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include <dm270/touchpad.h>

#define TOUCHPAD_SCALE_X      12
#define TOUCHPAD_SCALE_Y      16

#define KPD_DEVICE  "/dev/keypad"
#define TPD_DEVICE  "/dev/touchpad"

#define TPFILE      "/var/tmp/tp_cal.dat"

#ifndef _DEBUG
#define _DEBUG
#endif

#include "ial.h"
#include "utpmc.h"

/* for data reading from /dev/touchpad */
typedef struct {
    int x;
    int y;
    int status;
} TOUCHINFO;

/* for touchpad calibration */
static void set_calibration(void);

static int keypad_fd = -1;
static int touchpad_fd = -1;

static int mousex = 0;
static int mousey = 0;
static int lbutton = 0;

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
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return lbutton;
}

static unsigned char cur_key_scancode;
static unsigned char key_state [NR_KEYS];

#define MIN_KPD_SCANCODE    1
#define MAX_KPD_SCANCODE    10

static unsigned char kpd2pc_map [MAX_KPD_SCANCODE - MIN_KPD_SCANCODE + 1] =
{
    SCANCODE_CURSORBLOCKUP,
    SCANCODE_CURSORBLOCKDOWN,
    SCANCODE_CURSORBLOCKLEFT,
    SCANCODE_CURSORBLOCKRIGHT,
    SCANCODE_ENTER,
    SCANCODE_TAB,
    SCANCODE_BACKSPACE,
    SCANCODE_F1,
    SCANCODE_F2,
    SCANCODE_F3
};

static int keyboard_update(void)
{
    return NR_KEYS;
}

static const unsigned char* keyboard_getpdtate(void)
{
    return (unsigned char *)key_state;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    static unsigned int last_mouse_x;
    static unsigned int last_mouse_y;
    static BOOL is_leftbutton_down = FALSE;

    int retvalue = 0, e = 0;
    fd_set rfds;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && touchpad_fd >= 0) {
        FD_SET (touchpad_fd, in);
#ifdef _LITE_VERSION
        if (touchpad_fd > maxfd) maxfd = touchpad_fd;
#endif
    }

    if ((which & IAL_KEYEVENT) && keypad_fd >= 0){
        FD_SET (keypad_fd, in);
#ifdef _LITE_VERSION
        if (keypad_fd > maxfd) maxfd = keypad_fd;
#endif
    }

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (e > 0) { 
        if (touchpad_fd >= 0 && FD_ISSET (touchpad_fd, in)) {
            PEN_XYZ touch_info_raw;
            TOUCHINFO touch_info;

            FD_CLR (touchpad_fd, in);
            if (read (touchpad_fd, &touch_info_raw, sizeof (PEN_XYZ)) == sizeof (PEN_XYZ)) {

                touch_info.x = touch_info_raw.x;
                touch_info.y = touch_info_raw.y;
                touch_info.status= touch_info_raw.flag;

                switch (touch_info.status) {
                case 2: /* lbutton down */
                    /* mask small mouse move event */
                    if (is_leftbutton_down 
                            && (abs(last_mouse_x - touch_info.x) < PEN_X_THRESHOLD)
                            && (abs(last_mouse_y - touch_info.y) < PEN_Y_THRESHOLD))
                        break;
                    else {
                        last_mouse_x = touch_info.x;
                        last_mouse_y = touch_info.y;
                                
                        mousex = touch_info.x;
                        mousey = touch_info.y;

                        is_leftbutton_down = TRUE;

                        lbutton = IAL_MOUSE_LEFTBUTTON;
                        retvalue |= IAL_MOUSEEVENT;
                    }
                    break;

                case 4: /* lbutton up */
                   is_leftbutton_down = FALSE;
                   mousex = last_mouse_x;
                   mousey = last_mouse_y;
                   lbutton = 0;
                   retvalue |= IAL_MOUSEEVENT;
                   break;
                }
            }
        }

        if (keypad_fd >= 0 && FD_ISSET (keypad_fd, in)) {
            unsigned char kpd_scancode;

            FD_CLR (keypad_fd, in);
            read (keypad_fd, &kpd_scancode, sizeof (kpd_scancode));

            if (kpd_scancode >= MIN_KPD_SCANCODE && kpd_scancode <= MAX_KPD_SCANCODE) {
                cur_key_scancode = kpd2pc_map [kpd_scancode - MIN_KPD_SCANCODE];
                if (cur_key_scancode)
                    key_state [cur_key_scancode] = 1;
                retvalue |= IAL_KEYEVENT;
            }
            /* key up event */
            else if (kpd_scancode == 0) {
                if (cur_key_scancode) {
                    key_state [cur_key_scancode] = 0;
                    cur_key_scancode = 0;
                    retvalue |= IAL_KEYEVENT;
                }
            }
        }
    }
    else if (e < 0) {
        //perror ("Error in UTPMC IAL engine when calling select:");
        return -1;
    }

    return retvalue;
}

BOOL InitUTPMCInput (INPUT* input, const char* mdev, const char* mtype)
{
    touchpad_fd = open (TPD_DEVICE, O_RDONLY);
    if (touchpad_fd < 0) {
        fprintf (stderr, "UTPMC IAL engine: Can not open TouchPad: %s!\n", TPD_DEVICE);
        return FALSE;
    }

    keypad_fd = open (KPD_DEVICE, O_RDONLY);
    if (keypad_fd < 0 ) {
        fprintf (stderr, "UTPMC IAL engine: Can not open KeyPad: %s!\n", KPD_DEVICE);
        return FALSE;
    }

    if (input != NULL) {
        input->update_mouse = mouse_update;
        input->get_mouse_xy = mouse_getxy;
        input->set_mouse_xy = NULL;
        input->get_mouse_button = mouse_getbutton;
        input->set_mouse_range = NULL;

        input->update_keyboard = keyboard_update;
        input->get_keyboard_state = keyboard_getpdtate;
        input->set_leds = NULL;

        input->wait_event = wait_event;
    }

    set_calibration();
    
    return TRUE;
}

void TermUTPMCInput (void)
{
    if (touchpad_fd >= 0)
        close (touchpad_fd);

    if (keypad_fd >= 0)
        close (keypad_fd);
}

/*************************************************** 
 following code doing touchpad calibration 
****************************************************/
static void set_calibration(void)
{
    int fd;
    static CALIBRATION_MATRIX matrix;

    if ((fd = open (TPFILE, O_RDONLY)) < 0) {
        fprintf (stderr, "read_tp_cal_file open: cann't open file.\n");
        matrix.an = 187720;
        matrix.bn = 2180;
        matrix.cn = -11374740;
        matrix.dn = 120;
        matrix.en = 140280;
        matrix.fn = -4865400;
        matrix.divider = 526662;
    }
    
    if (read (fd, &matrix, sizeof(CALIBRATION_MATRIX)) 
            != sizeof(CALIBRATION_MATRIX)) {
        matrix.an = 187720;
        matrix.bn = 2180;
        matrix.cn = -11374740;
        matrix.dn = 120;
        matrix.en = 140280;
        matrix.fn = -4865400;
        matrix.divider = 526662;
    }

    ioctl (touchpad_fd, TOUCHPANEL_CTRL_SET_POLLING_TIMER, 20);
    ioctl (touchpad_fd, TOUCHPANEL_CTRL_SET_CALIBRATION_PARA, &matrix);
    close (fd);
}

#endif /* _UTPMC_IAL */

