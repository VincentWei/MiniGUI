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
**  vxworks_i386.c: A subdriver of CommonLCD NEWGAL engine for VxWorks on i386.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#if defined (__OSE__) && defined (__TARGET_MX21__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#include "fbdev.sig"

union SIGNAL
{
    SIGSELECT            sig_no;
    struct DevInterface  intf;
    struct DevOpen       open;
    struct DevClose      close;
    struct FbdevQuery    query;
};

static const SIGSELECT any_sig[] = {0};
static PROCESS fbdev_pid;

static int a_init (void)
{
    union SIGNAL *lcdsig;
    if (!hunt("ose_fbdev", 0, &fbdev_pid, 0))
    {
	      fprintf(stderr, "No framebuffer device found\n");
	      return 1;
    }
    
    /* Open framebuffer */
    lcdsig = alloc(sizeof(struct DevOpen), DEV_OPEN_REQUEST);
    send(&lcdsig, fbdev_pid);
    lcdsig = receive(any_sig);
    if (lcdsig->open.status != 0)
    {
	      printf("ose_fbdev: %s\n", strerror(lcdsig->open.status));
    }


    return 0; 
}

static int a_getinfo (struct commlcd_info *li)
{
    struct FbdevQuery *fbinfo;
    union SIGNAL *lcdsig = alloc(sizeof(struct FbdevQuery), FBDEV_QUERY_REQUEST);
    send(&lcdsig, fbdev_pid);
    lcdsig = receive(any_sig);
    fbinfo = &lcdsig->query;

    /*set default RGB*/
    li->type = COMMLCD_TRUE_RGB565;
    li->height = fbinfo->height;
    li->width = fbinfo->width;
    li->fb = fbinfo->fb_ptr;
    li->bpp = fbinfo->depth;
    li->rlen = (li->bpp*li->width + 7) / 8;
    /*li->rlen = fbinfo->fb_size;*/
    return 0;
}

struct commlcd_ops __mg_commlcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    NULL,
    NULL
};

#endif /* __OSE__ && __TARGET_MX21__ */

#endif /* _MGGAL_COMMLCD */

