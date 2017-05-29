/*
**  $Id: threadx_anvil.c 8946 2007-12-29 08:32:26Z xwyan $
**  
**  threadx_anvil.c: A subdriver of shadow NEWGAL engine for ThreadX on VisualFone Anvil.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
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

