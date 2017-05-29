/*
**  $Id: unknown.c 8946 2007-12-29 08:32:26Z xwyan $
**  
**  unknown.c: A subdriver of shadow NEWGAL engine for unknown target.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_SHADOW 

#ifdef __TARGET_UNKNOWN__

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "shadow.h"

static int a_init (void)
{
    return 0;
}

static int a_getinfo (struct shadowlcd_info* lcd_info)
{
    lcd_info->height = 240;
    lcd_info->width = 320;
    lcd_info->bpp = 1;
    lcd_info->fb = NULL;
    lcd_info->rlen = 0;
    lcd_info->type = 0;
    
    return 0;
}

static void a_sleep (void)
{
}

static void a_refresh (_THIS, const RECT* update)
{
}

struct shadow_lcd_ops __mg_shadow_lcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    NULL,
    a_sleep,
    a_refresh
};

#endif /* __TARGET_UNKNOWN__ */

#endif /* _MGGAL_SHADOW */

