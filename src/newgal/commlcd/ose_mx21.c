/*
**  $Id: ose_mx21.c 8944 2007-12-29 08:29:16Z xwyan $
**  
**  vxworks_i386.c: A subdriver of CommonLCD NEWGAL engine for VxWorks on i386.
**
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#if defined (__OSE__) && defined (__TARGET_MX21__)

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#include "fbdev.sig"

union SIGNAL
{
    SIGSELECT            sig_no;
    struct DevInterface  intf;
    struct DevOpen       open;
    struct DevClose      close;
    struct FbdevQuery    query;
};

static const SIGSELECT any_sig[] = {0};
static PROCESS fbdev_pid;

static int a_init (void)
{
    union SIGNAL *lcdsig;
    if (!hunt("ose_fbdev", 0, &fbdev_pid, 0))
    {
	      fprintf(stderr, "No framebuffer device found\n");
	      return 1;
    }
    
    /* Open framebuffer */
    lcdsig = alloc(sizeof(struct DevOpen), DEV_OPEN_REQUEST);
    send(&lcdsig, fbdev_pid);
    lcdsig = receive(any_sig);
    if (lcdsig->open.status != 0)
    {
	      printf("ose_fbdev: %s\n", strerror(lcdsig->open.status));
    }


    return 0; 
}

static int a_getinfo (struct commlcd_info *li)
{
    struct FbdevQuery *fbinfo;
    union SIGNAL *lcdsig = alloc(sizeof(struct FbdevQuery), FBDEV_QUERY_REQUEST);
    send(&lcdsig, fbdev_pid);
    lcdsig = receive(any_sig);
    fbinfo = &lcdsig->query;

    /*set default RGB*/
    li->type = COMMLCD_TRUE_RGB565;
    li->height = fbinfo->height;
    li->width = fbinfo->width;
    li->fb = fbinfo->fb_ptr;
    li->bpp = fbinfo->depth;
    li->rlen = (li->bpp*li->width + 7) / 8;
    /*li->rlen = fbinfo->fb_size;*/
    return 0;
}

struct commlcd_ops __mg_commlcd_ops = {
    a_init,
    a_getinfo,
    NULL,
    NULL
};

#endif /* __OSE__ && __TARGET_MX21__ */

#endif /* _MGGAL_COMMLCD */

