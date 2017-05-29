/*
**  $Id: vxworks_ppc.c 7035 2006-12-15 03:32:23Z xwyan $
**  
**  vxworks_ppc.c: A subdriver of CommonLCD NEWGAL engine for VxWorks on i386.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

/*WINML2.0 reference*/
/* This implementation is based on WinML2.0 & for nari */
#if defined (__VXWORKS__) && defined (__TARGET_PPC__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#include <ugl/uglugi.h>
#include <ugl/uglinfo.h>
#include <ugl/ugl.h>

static UGL_DEVICE_ID devId;
UGL_GC_ID gc;
    
static int a_init (void)
{
    if (uglInitialize() == UGL_STATUS_ERROR)
        return 1;
    
    return 0; 
}

static int a_getinfo (struct lcd_info *li)
{
    UGL_MODE_INFO modeInfo;

    
    /* Obtain display device identifier */
    uglDriverFind (UGL_DISPLAY_TYPE, 0, (UGL_UINT32 *)&devId);
   
    /* Create a graphics context */
    gc = uglGcCreate (devId);
    
    uglInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);

    li->height = modeInfo.height;
    li->width = modeInfo.width;
    li->fb = modeInfo.fbAddress;
    //li->bpp = modeInfo.colorDepth;
    li->bpp = 32;
    printf ("Current color depth is %d \n", modeInfo.colorDepth);
    li->rlen = (li->bpp*1024 + 7) / 8;
    return 0;
}

static int a_release(void)
{
    uglGcDestroy (gc); 
    uglDeinitialize();
    return 0;
}

struct lcd_ops __mg_commlcd_ops = {
        a_init,
        a_getinfo,
        a_release,
        NULL
};

#endif /* __VXWORKS__ && __TARGET_PPC__ */

#endif /* _MGGAL_COMMLCD */

