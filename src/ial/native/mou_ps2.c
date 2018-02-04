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
** mou_ps2.c: PS/2 mouse driver.
**
** Created by Song Lixin at 2000/11/17 
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

#ifdef _MGCONSOLE_PS2

#define SCALE           3    /* default scaling factor for acceleration */
#define THRESH          5    /* default threshhold for acceleration */

#define PS2_SCALE11     230    /* Set 1:1 scale factor */
#define PS2_SCALE21     231    /* Set 2:1 scale factor */
#define PS2_SETRES      232    /* Set resolution */
#define PS2_GETSCALE    233    /* Get scale factor */
#define PS2_SETSTREAM   234    /* Set stream mode */
#define PS2_SETSAMPLE   243    /* Set sample rate */
#define PS2_ENABLE      244    /* Enable PS/2 device */
#define PS2_DISABLE     245    /* Disable PS/2 device */
#define PS2_DEFAULT     246    /* Set default settings */
#define PS2_RESET       255    /* Reset PS/2 device */

static gal_uint8 initdata_ps2[] = 
/* Make sure the mouse is enabled and in a sane state */
{ PS2_DEFAULT, PS2_SCALE11, PS2_ENABLE };

static int  PS2_Open(const char* mdev);
static void PS2_Close(void);
static int  PS2_GetButtonInfo(void);
static void PS2_GetDefaultAccel(int *pscale,int *pthresh);
static int  PS2_Read(int *dx, int *dy, int *dz, int *bp);
static void PS2_Suspend(void);
static int  PS2_Resume(void);

MOUSEDEVICE mousedev_PS2 = {
    PS2_Open,
    PS2_Close,
    PS2_GetButtonInfo,
    PS2_GetDefaultAccel,
    PS2_Read,
    PS2_Suspend,
    PS2_Resume
};

static int mouse_fd;

/*
 * Open up the mouse device.
 * Returns the fd if successful, or negative if unsuccessful.
 */
static int PS2_Open(const char* mdev)
{
    mouse_fd = open (mdev, O_RDWR | O_NONBLOCK);
    if (mouse_fd < 0) {
        /* try to open read only again. */
        mouse_fd = open (mdev, O_RDONLY | O_NONBLOCK);
        if (mouse_fd < 0)
            return -1;
    }
    else {
        if (write (mouse_fd, initdata_ps2, sizeof(initdata_ps2)) < sizeof(initdata_ps2)) {
            return -1;
        }
    }

    return mouse_fd;
}

/*
 * Close the mouse device.
 */
static void PS2_Close(void)
{
    if (mouse_fd > 0)
        close(mouse_fd);
    mouse_fd = -1;
}

/*
 * Get mouse buttons supported
 */
static int PS2_GetButtonInfo(void)
{
    return BUTTON_L | BUTTON_M | BUTTON_R;
}

/*
 * Get default mouse acceleration settings
 */
static void PS2_GetDefaultAccel(int *pscale,int *pthresh)
{
    *pscale = SCALE;
    *pthresh = THRESH;
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
static int PS2_Read(int *dx, int *dy, int *dz, int *bp)
{
    static unsigned char buf[3];
    static int buttons[4] = { 0, BUTTON_L, BUTTON_R, BUTTON_L | BUTTON_R};
    static int nbytes;
    int n;

    while((n = read (mouse_fd, &buf[nbytes], 3 - nbytes))) {
        if(n < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }

        nbytes += n;

        if(nbytes == 3) {
            /* Check header byte. */
            if ((buf[0] & 0xc0) != 0) {
                buf[0] = buf[1];
                buf[1] = buf[2];
                nbytes = 2;
                return -1;
            }

            *bp = buttons[(buf[0] & 0x03)];
            *dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
            *dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];

            *dz = 0;
            nbytes = 0;
            return 1;
        }
    }
    return 0;
}

static void PS2_Suspend(void)
{
    PS2_Close();
}

static int PS2_Resume(void)
{
    return PS2_Open(IAL_MDev);
}
#endif /* _MGCONSOLE_PS2 */

