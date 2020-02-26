///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Or,
 *
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 *
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 *
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** desktop-sa.c: The desktop for MiniGUI-Standalone
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
PMSGQUEUE __mg_dsk_msg_queue;

#include "desktop-comm.c"
/********************* Window management support *****************************/

static BOOL InitWndManagementInfo (void)
{
    __mg_captured_wnd = 0;

    sg_ptmi = NULL;

    __mg_ime_wnd = 0;
    sg_hCaretWnd = 0;

    return TRUE;
}

BOOL mg_InitDesktop (void)
{
    int ret;
    RECT rcScr = GetScreenRect();

#if 0   /* move to init-lite.c */
    /* Since 5.0.0: allocate message queue for desktop thread */
    if (!(__mg_dsk_msg_queue = mg_AllocMsgQueueForThisThread ()) ) {
        _WRN_PRINTF ("failed to allocate message queue\n");
        return FALSE;
    }
#endif  /* moved code */

    /*
     * Init ZOrderInfo here.
     */
    ret = __kernel_alloc_z_order_info (DEF_NR_TOPMOSTS, DEF_NR_NORMALS);
    if (ret < 0) {
        _WRN_PRINTF ("KERNEL>Desktop: Can not initialize ZOrderInfo!\n");
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
    SetClipRgn (&sg_ScrGCRInfo.crgn, &rcScr);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);

    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    return TRUE;
}

void mg_TerminateDesktop (void)
{
    if (__mg_dsk_msg_queue) {
        mg_FreeMsgQueueForThisThread ();
        __mg_dsk_msg_queue = NULL;
    }

    __kernel_free_z_order_info (__mg_zorder_info);
    __mg_zorder_info = NULL;
    DestroyFreeClipRectList (&sg_FreeClipRectList);
    DestroyFreeClipRectList (&sg_FreeInvRectList);

    //mg_TerminateSystemRes ();
    //dongjunjie avoid double free
    __mg_dsk_win = 0;
}

#endif /* _MGRM_STANDALONE */
