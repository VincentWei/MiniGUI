/*
**  $Id: nucleus_monaco.c 11423 2009-03-26 14:23:26Z weiym $
**  
**  nucleus_monaco.c: A subdriver of shadow NEWGAL engine for Nucleus on Monaco.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
**
**  Author: Jiao Libo
**
**  Create Date: 2006-04-19
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_SHADOW 

#if defined (__NUCLEUS__) && defined (__TARGET_MONACO__)
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "shadow.h"

#include "os_api.h"
#include "display.h"

static int a_init (void)
{
    tp_man_lcd_init ();
    tp_man_lcd_set_mode(FULL);
    tp_man_lcd_backlight_start(255);
       
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
    lcd_info->type = lcd.type;
    
    return 0;
}

static int a_release (void)
{
    tp_man_lcd_release();  
    return 0;
}

static void a_sleep (void)
{
    NU_Sleep(2);
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

#endif /* __NUCLEUS__ && __TARGET_MONACO__ */

#endif /* _MGGAL_SHADOW */

