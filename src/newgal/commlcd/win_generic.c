/*
**  $Id: win_generic.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  wvfb_generic.c: A subdriver of CommonLCD NEWGAL engine for Windows.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#if defined (WIN32) && defined (__TARGET_WVFB__)
#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

extern int wvfb_drv_lcd_init (void);
extern int wvfb_drv_lcd_getinfo (struct commlcd_info *li); 

static int wvfb_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    /* do nothing of note. */
    return 1;
}

struct commlcd_ops __mg_commlcd_ops = {
    wvfb_drv_lcd_init,
    wvfb_drv_lcd_getinfo,
    NULL,
    wvfb_drv_setclut 
};

#endif /* WIN32 && __TARGET_WVFB__ */

#endif /* _MGGAL_COMMLCD */
