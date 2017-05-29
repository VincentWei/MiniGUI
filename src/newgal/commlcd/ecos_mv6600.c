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
    NULL
};

#endif /* __ECOS__ */

#endif /* _MGGAL_COMMLCD */
