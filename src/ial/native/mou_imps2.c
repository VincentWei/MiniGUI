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
** mou_imps2.c: Intelligent PS/2 Mouse Driver
**
** Create Date: 2002/10/15 
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "ial.h"
#include "gal.h"
#include "native.h"

#ifdef _MGCONSOLE_IMPS2

#define SCALE           3       /* default scaling factor for acceleration */
#define THRESH          5       /* default threshhold for acceleration */

/* The intialization string of imps/2 mouse, comes from
 * xc/program/Xserver/hw/xfree86/input/mouse/mouse.c
 */
static unsigned char IMPS2_Param [] = {243,200,243,100,243,80};

static int  IMPS2_Open (const char* mdev);
static void IMPS2_Close (void);
static int  IMPS2_GetButtonInfo (void);
static void IMPS2_GetDefaultAccel (int *pscale, int *pthresh);
static int  IMPS2_Read (int *dx, int *dy, int *dz, int *bp);
static void IMPS2_Suspend (void);
static int IMPS2_Resume (void);

MOUSEDEVICE mousedev_IMPS2 = {
    IMPS2_Open,
    IMPS2_Close,
    IMPS2_GetButtonInfo,
    IMPS2_GetDefaultAccel,
    IMPS2_Read,
    IMPS2_Suspend,
    IMPS2_Resume
};

static int mouse_fd;

static int IMPS2_Open (const char* mdev)
{
    mouse_fd = open (mdev, O_RDWR | O_NONBLOCK);
    if (mouse_fd < 0) {
        mouse_fd = open (mdev, O_RDONLY | O_NONBLOCK);
        if (mouse_fd < 0)
            return -1;
    }
    else {
        if (write (mouse_fd, IMPS2_Param, sizeof (IMPS2_Param)) < sizeof (IMPS2_Param)) {
            return -1;
        }
    }

    return mouse_fd;
}

static void IMPS2_Close (void)
{
    if (mouse_fd > 0)
        close (mouse_fd);

    mouse_fd = -1;
}

static int IMPS2_GetButtonInfo(void)
{
    return BUTTON_L | BUTTON_M | BUTTON_R | WHEEL_UP | WHEEL_DOWN;
}

static void IMPS2_GetDefaultAccel(int *pscale, int *pthresh)
{
    *pscale = SCALE;
    *pthresh = THRESH;
}

static int IMPS2_Read (int *dx, int *dy, int *dz, int *bp)
{
    static unsigned char buf[5];
    static int buttons[4] = { 0, BUTTON_L, BUTTON_R, BUTTON_L | BUTTON_R};
    static int nbytes;
    int n;

    while ((n = read (mouse_fd, &buf [nbytes], 4 - nbytes))) {
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }

        nbytes += n;

        if (nbytes == 4) {
            int wheel;

            if ((buf[0] & 0xc0) != 0) {
                buf[0] = buf[1];
                buf[1] = buf[2];
                buf[2] = buf[3];
                nbytes = 3;

                return -1;
            }

            /* FORM XFree86 4.0.1 */
            *bp = buttons[(buf[0] & 0x03)];
            *dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
            *dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];

            /* Is a wheel event? */
            if ((wheel = buf[3]) != 0) {
                if(wheel > 0x7f) {
                    *bp = WHEEL_UP;
                }
                else {
                    *bp = WHEEL_DOWN;
                }
            }
             
            *dz = 0;
            nbytes = 0;
            return 1;
        }
    }
    return 0;
}

static void IMPS2_Suspend (void)
{
    IMPS2_Close();
}

static int IMPS2_Resume (void)
{
    return IMPS2_Open (IAL_MDev);
}

#endif /* _MGCONSOLE_IMPS2 */

