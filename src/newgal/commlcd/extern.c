/*
**  $Id: unknown.c 7346 2007-08-16 03:58:07Z xgwang $
**  
**  extern.c: A subdriver of CommonLCD NEWGAL engine for some targets
**      which are defined by the external module.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
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
extern int __commlcd_drv_setclut (int firstcolor, 
            int ncolors, GAL_Color *colors);

struct commlcd_ops __mg_commlcd_ops = {
    __commlcd_drv_init,
    __commlcd_drv_getinfo,
    __commlcd_drv_release,
    __commlcd_drv_setclut
};

#endif

#endif /* _MGGAL_COMMLCD */

