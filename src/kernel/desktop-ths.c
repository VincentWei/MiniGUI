/* 
** $Id: desktop-ths.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** desktop-ths.c: The desktop for MiniGUI-Threads.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/04/19
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

#ifdef _MGRM_THREADS

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
#include "misc.h"
#include "devfont.h"

/******************************* global data *********************************/
/* system threads */
pthread_t __mg_desktop, __mg_parsor, __mg_timer;

/* pointer to desktop message queue */
PMSGQUEUE __mg_dsk_msg_queue;

/********************* Window management support *****************************/
#include "desktop-comm.c"

static BOOL InitWndManagementInfo (void)
{
    __mg_capture_wnd = 0;

#ifdef _MGHAVE_MENU
    sg_ptmi = NULL;
#endif

    __mg_ime_wnd = 0;
    sg_hCaretWnd = 0;

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcScr);
    pthread_mutex_init (&sg_ScrGCRInfo.lock, NULL);
    sg_ScrGCRInfo.age = 0;
    sg_ScrGCRInfo.old_zi_age = 0;

    InitClipRgn (&sg_UpdateRgn, &sg_FreeClipRectList);
    MAKE_REGION_INFINITE(&sg_UpdateRgn);

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
    if (!InitWndManagementInfo()) {
        _MG_PRINTF ("KERNEL>Desktop: Can not initialize window management information!\n");
        return FALSE;
    }

    return TRUE;
}

void* DesktopMain (void* data)
{
    MSG Msg;

    /* init message queue of desktop thread */
    if (!(__mg_dsk_msg_queue = mg_InitMsgQueueThisThread ()) ) {
        _MG_PRINTF ("KERNEL>Desktop: mg_InitMsgQueueThisThread failure!\n");
        return NULL;
    }

    /* init desktop window */
    init_desktop_win();
    __mg_dsk_win->th = pthread_self ();
    __mg_dsk_msg_queue->pRootMainWin = __mg_dsk_win;

    DesktopWinProc (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    PostMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

    /* desktop thread is ready now */
    sem_post ((sem_t*)data);

    /* process messages of desktop thread */
    while (GetMessage(&Msg, HWND_DESKTOP)) {
        LRESULT lRet = 0;

#ifdef _MGHAVE_TRACE_MSG
        fprintf (stderr, "Message, %s: hWnd: %p, wP: %p, lP: %p. %s\n",
            Message2Str (Msg.message),
            Msg.hwnd,
            (PVOID)Msg.wParam,
            (PVOID)Msg.lParam,
            Msg.pAdd?"Sync":"Normal");
#endif

        lRet = DesktopWinProc (HWND_DESKTOP, 
                        Msg.message, Msg.wParam, Msg.lParam);

        if (Msg.pAdd) /* this is a sync message. */
        {
            PSYNCMSG pSyncMsg = (PSYNCMSG)(Msg.pAdd);
            pSyncMsg->retval = lRet;
            sem_post(pSyncMsg->sem_handle);
        }

#ifdef _MGHAVE_TRACE_MSG
        fprintf (stderr, "Message, %s done, return value: %p\n",
            Message2Str (Msg.message), (PVOID)lRet);
#endif
    }

    /* printf("Quit from DesktopMain()\n"); */

    __mg_quiting_stage = _MG_QUITING_STAGE_EVENT;

    return NULL;
}

pthread_t GUIAPI GetMainWinThread(HWND hMainWnd)
{ 
#ifdef WIN32
    pthread_t ret;
    memset(&ret, 0, sizeof(pthread_t));
    MG_CHECK_RET (MG_IS_WINDOW(hMainWnd), ret);
#else
    MG_CHECK_RET (MG_IS_WINDOW(hMainWnd), 0);
#endif
    
    return ((PMAINWIN)hMainWnd)->th;
}

#endif /* _MGRM_THREADS */

