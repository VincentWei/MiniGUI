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
**  extern.c: A subdriver of CommonLCD NEWGAL engine for some targets
**      which are defined by the external module.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#if defined (__VXWORKS__) || defined (__TARGET_UNKNOWN__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

extern int __commlcd_drv_init (void);
extern int __commlcd_drv_getinfo (struct commlcd_info *li);
extern int __commlcd_drv_release (void);
extern int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors);
extern int __commlcd_drv_update (const RECT* rc_dirty);

struct commlcd_ops __mg_commlcd_ops = {
    __commlcd_drv_init,
    __commlcd_drv_getinfo,
    __commlcd_drv_release,
    __commlcd_drv_setclut,
    __commlcd_drv_update
};

#endif

#endif /* _MGGAL_COMMLCD */

