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
**  ecos_mv6600.c: A subdriver of CommonLCD NEWGAL engine for eCos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#ifdef __ECOS__

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#include <cyg/hal/drv_api.h>
#include <cyg/io/io.h>
//#include <cyg/hal/lcd_support.h>
//#include <L1Common_trace.h>

typedef unsigned long   UINT32;
typedef signed long		INT32;

extern void toppoly_init(void);
extern INT32 lcd_graphic_overlay_display(UINT32 image_buf_addr, UINT32 image_width, UINT32 image_height, UINT32 X_pos, UINT32 Y_pos, UINT32 skip_pixels);

static int a_init (void)
{
 //   lcd_init (16);
//    MV_TRACE(FLASH, "FILE=%s.........LINE=%d\n", __FILE__, __LINE__);
    toppoly_init();
    lcd_graphic_overlay_display(0xe40000, 320, 240, 0, 0, 0);
    //MV_TRACE(FLASH, "FILE=%s.........LINE=%d\n", __FILE__, __LINE__);
    return 0; 
}

static int a_getinfo (struct commlcd_info *li)
{
   // lcd_getinfo ((struct lcd_info*) li);
  //  MV_TRACE(FLASH, "FILE=%s.........LINE=%d\n", __FILE__, __LINE__);
   li->height = 240;
   li->width = 320;
   li->bpp = 32;
   li->rlen = 320 * 32 / 8;
   li->fb = 0xe40000;
   li->type = COMMLCD_TRUE_RGB0888;
    return 0;
}

struct commlcd_ops __mg_commlcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    NULL,
    NULL
};

#endif /* __ECOS__ */

#endif /* _MGGAL_COMMLCD */
