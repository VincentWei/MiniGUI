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
**  threadx_anvil.c: A subdriver of shadow NEWGAL engine for ThreadX on VisualFone Anvil.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_SHADOW 

#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "shadow.h"

#include "tx_api.h"
#include "displaypub.h"

static int a_init (void)
{
    tp_man_lcd_init ();
    return 0;
}

static int a_getinfo (struct shadowlcd_info* lcd_info)
{
    MAN_LCD_SCREEN lcd;
    
    tp_man_lcd_get_info (&lcd);

    lcd_info->height = lcd.height;
    lcd_info->width = lcd.width;
    lcd_info->bpp = lcd.bpp;
    lcd_info->fb = lcd.fb_ptr;
    lcd_info->rlen = lcd.rlen;
    lcd_info->type = lcd.type_en;
    
    return 0;
}

static int a_release (void)
{
    tp_man_lcd_release();  
    return 0;
}

static void a_sleep (void)
{
    tx_thread_sleep (2);    // 40 ms
}

static void a_refresh (_THIS, const RECT* update)
{
    MAN_LCD_REFRESH lcd;

    lcd.startX = update->left;
    lcd.startY = update->top;
    lcd.width = RECTWP (update);
    lcd.height = RECTHP (update);

    tp_man_lcd_refresh (&lcd);
}

struct shadow_lcd_ops __mg_shadow_lcd_ops = {
    a_init,
    a_getinfo,
    a_release,
    NULL,
    a_sleep,
    a_refresh
};

#endif /* __THREADX__ && __TARGET_VFANVIL__ */

#endif /* _MGGAL_SHADOW */

