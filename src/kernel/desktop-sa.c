/*
** $Id: desktop-sa.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** desktop-sa.c: The desktop for MiniGUI-Standalone
**
** Copyright (C) 2002 ~ 2008 Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Derived from desktop-lite.c (2005/08/15)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

#ifdef _MGRM_STANDALONE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "menu.h"
#include "timer.h"
#include "dc.h"
#include "icon.h"
#include "dc.h"
#include "misc.h"
#include "devfont.h"

#ifndef WIN32
#include <sys/termios.h>
#endif

/******************************* global data *********************************/
MSGQUEUE __mg_desktop_msg_queue;
PMSGQUEUE __mg_dsk_msg_queue = &__mg_desktop_msg_queue;

BOOL __mg_switch_away; // always be zero for clients.

#include "desktop-comm.c"
/********************* Window management support *****************************/

static BOOL InitWndManagementInfo (void)
{
    __mg_capture_wnd = 0;

    sg_ptmi = NULL;

    __mg_ime_wnd = 0;
    sg_hCaretWnd = 0;

    return TRUE;
}

BOOL mg_InitDesktop (void)
{
    int ret = 0; 
    /*
     * Init ZOrderInfo here.
     */
    ret = kernel_alloc_z_order_info (DEF_NR_TOPMOSTS, DEF_NR_NORMALS);
    if (ret < 0) {
        _MG_PRINTF ("KERNEL>Desktop: Can not initialize ZOrderInfo!\n");
        return FALSE;
    }
    
    /*
     * Init heap of clipping rects.
     */
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);

    /*
     * Init heap of invalid rects.
     */
    InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);

    // Init Window Management information.
    if (!InitWndManagementInfo ())
        return FALSE;

    init_desktop_win ();

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcScr);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);

    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    return TRUE;
}

#endif /* _MGRM_STANDALONE */
