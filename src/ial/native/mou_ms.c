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
** mou_ms.c: MS Mouse Driver
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

#ifdef _MGCONSOLE_MS

#define SCALE       3    /* default scaling factor for acceleration */
#define THRESH      5    /* default threshhold for acceleration */

#define PLEN		4    /* mouse data package length */

static int  MS_Open (const char* mdev);
static void MS_Close (void);
static int  MS_GetButtonInfo (void);
static void MS_GetDefaultAccel (int *pscale, int *pthresh);
static int  MS_Read (int *dx, int *dy, int *dz, int *bp);
static void MS_Suspend (void);
static int  MS_Resume (void);

MOUSEDEVICE mousedev_MS = {
    MS_Open,
    MS_Close,
    MS_GetButtonInfo,
    MS_GetDefaultAccel,
    MS_Read,
    MS_Suspend,
    MS_Resume
};

static int mouse_fd;

/*
 * Open up the mouse device.
 * Returns the fd if successful, or negative if unsuccessful.
 */
static int MS_Open (const char* mdev)
{
    mouse_fd = open (mdev, O_RDONLY | O_NONBLOCK);
    if (mouse_fd < 0)
        return -1;
    return mouse_fd;
}

/*
 * Close the mouse device.
 */
static void MS_Close (void)
{
    if (mouse_fd > 0)
        close (mouse_fd);
    mouse_fd = -1;
}

/*
 * Get mouse buttons supported
 */
static int MS_GetButtonInfo (void)
{
    return BUTTON_L | BUTTON_M | BUTTON_R;
}

/*
 * Get default mouse acceleration settings
 */
static void MS_GetDefaultAccel (int *pscale, int *pthresh)
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
static int MS_Read(int *dx, int *dy, int *dz, int *bp)
{
   static unsigned char buf[3];
   static int buttons[4] = { 0, BUTTON_L, BUTTON_R, BUTTON_L | BUTTON_R};
   static int nbytes;
   int n;

   while ((n = read (mouse_fd, &buf[nbytes], 3 - nbytes))) {
       if(n < 0) {
           if (errno == EINTR)
               continue;
           else
               return -1;
       }

       nbytes += n;

       if (nbytes == 3) {
           /* check header */
           if ((buf[0] & 0x40) != 0x40) {
               memmove(buf, buf + 1, 2);
               nbytes = 2;
               return -1;
           }

           *bp = buttons[((buf[0] >> 4) & 0x03)];
           *dx = (signed char)((buf[0] << 6) | (buf[1] & 0x3f));
           *dy = (signed char)(((buf[0] << 4) & 0xc0) | (buf[2] & 0x3f));
           *dz = 0;
           nbytes = 0;

           return 1;
       }
   }

   return 0;
}

static void MS_Suspend (void)
{
    MS_Close();
}

static int MS_Resume(void)
{
    return MS_Open (IAL_MDev);
}

#endif /* _MGCONSOLE_MS */

