/*
**  $Id: ecos_generic.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  ecos_generic.c: A subdriver of CommonLCD NEWGAL engine for eCos.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
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
#include <cyg/hal/lcd_support.h>

static int a_init (void)
{
    lcd_init (16);
    return 0; 
}

static int a_getinfo (struct commlcd_info *li)
{
    lcd_getinfo ((struct lcd_info*) li);
    return 0;
}

struct commlcd_ops __mg_commlcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    NULL
};

#endif /* __ECOS__ */

#endif /* _MGGAL_COMMLCD */
