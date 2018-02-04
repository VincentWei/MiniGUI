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
** mou_gpm.c: GPM Mouse Driver
**
** Create by Song Lixin at 2000/10/17.
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "ial.h"
#include "gal.h"
#include "native.h"

#ifdef _MGCONSOLE_GPM

#define SCALE           3    /* default scaling factor for acceleration */
#define THRESH          5    /* default threshhold for acceleration */

static int  GPM_Open(const char* mdev);
static void GPM_Close(void);
static int  GPM_GetButtonInfo(void);
static void GPM_GetDefaultAccel(int *pscale,int *pthresh);
static int  GPM_Read(int *dx, int *dy, int *dz, int *bp);
static void GPM_Suspend(void);
static int  GPM_Resume(void);

MOUSEDEVICE mousedev_GPM = {
    GPM_Open,
    GPM_Close,
    GPM_GetButtonInfo,
    GPM_GetDefaultAccel,
    GPM_Read,
    GPM_Suspend,
    GPM_Resume
};

static int mouse_fd;

/*
 * Open up the mouse device.
 * Returns the fd if successful, or negative if unsuccessful.
 */
static int GPM_Open (const char* mdev)
{
    mouse_fd = open (mdev, O_RDONLY | O_NONBLOCK);
    if (mouse_fd < 0)
        return -1;
    return mouse_fd;
}

/*
 * Close the mouse device.
 */
static void GPM_Close(void)
{
    if (mouse_fd > 0)
        close(mouse_fd);
    mouse_fd = -1;
}

/*
 * Get mouse buttons supported
 */
static int GPM_GetButtonInfo(void)
{
    return BUTTON_L | BUTTON_M | BUTTON_R;
}

/*
 * Get default mouse acceleration settings
 */
static void GPM_GetDefaultAccel(int *pscale,int *pthresh)
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
static int GPM_Read(int *dx, int *dy, int *dz, int *bp)
{
    static unsigned char buf[5];
    static int nbytes;
    int n;

    while((n = read (mouse_fd, &buf[nbytes], 5 - nbytes))) {
        if(n < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }

        nbytes += n;

        if(nbytes == 5) {
            /* check header */
            if ((buf[0] & 0xf8) != 0x80) {
                memmove(buf, buf + 1, 4);
                nbytes = 4;
                return -1;
            }
            *bp = buf[0] & 0x07; /* FIXME */
            
            *dx = (signed char)(buf[1]) + (signed char)(buf[3]);
            *dy = -((signed char)(buf[2]) + (signed char)(buf[4]));
            *dz = 0;
            nbytes = 0;
            return 1;
        }
    }
    return 0;
}

static void GPM_Suspend(void)
{
    GPM_Close();
}

static int GPM_Resume(void)
{
    return GPM_Open (IAL_MDev);
}

#endif /* _MGCONSOLE_GPM */

