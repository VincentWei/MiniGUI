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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
** init.c: The Initialization/Termination routines for MiniGUI-Threads.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"

#ifndef __NOUNIX__
#include <unistd.h>
#include <signal.h>
#include <sys/termios.h>
#endif

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "ial.h"
#include "internals.h"
#include "ctrlclass.h"
#include "cursor.h"
#include "event.h"
#include "misc.h"
#include "menu.h"
#include "timer.h"
#include "accelkey.h"
#include "clipboard.h"
#include "license.h"

#ifdef _MGRM_THREADS
// deprecated since 5.0.0
// int __mg_enter_terminategui;

static int _is_minigui_running;

static pthread_t _th_parsor;

/******************************* extern data *********************************/
extern void* __kernel_desktop_main (void* data);

/************************* Entry of the thread of parsor *********************/
static void ParseEvent (PLWEVENT lwe)
{
    PMOUSEEVENT me;
    PKEYEVENT ke;
    MSG Msg;
    static int mouse_x = 0, mouse_y = 0;

    ke = &(lwe->data.ke);
    me = &(lwe->data.me);
    Msg.hwnd = HWND_DESKTOP;
    Msg.wParam = 0;
    Msg.lParam = 0;

    Msg.time = __mg_tick_counter;

    if (lwe->type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe->count;
        Msg.lParam = 0;
        // Since 5.0.0, we do not genenrate MSG_TIMEOUT message any more.
        // QueueDeskMessage (&Msg);
    }
    else if (lwe->type == LWETYPE_KEY) {
        Msg.wParam = ke->scancode;
        Msg.lParam = ke->status;
        if(ke->event == KE_KEYDOWN){
            Msg.message = MSG_KEYDOWN;
        }
        else if (ke->event == KE_KEYUP) {
            Msg.message = MSG_KEYUP;
        }
        else if (ke->event == KE_KEYLONGPRESS) {
            Msg.message = MSG_KEYLONGPRESS;
        }
        else if (ke->event == KE_KEYALWAYSPRESS) {
            Msg.message = MSG_KEYALWAYSPRESS;
        }

        if (__mg_check_hook_func (HOOK_EVENT_KEY, &Msg) == HOOK_GOON)
            QueueDeskMessage (&Msg);
    }
    else if(lwe->type == LWETYPE_MOUSE) {
        Msg.wParam = me->status;
        Msg.lParam = MAKELONG (me->x, me->y);

        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
            break;
        case ME_LEFTDOWN:
            Msg.message = MSG_LBUTTONDOWN;
            break;
        case ME_LEFTUP:
            Msg.message = MSG_LBUTTONUP;
            break;
        case ME_LEFTDBLCLICK:
            Msg.message = MSG_LBUTTONDBLCLK;
            break;
        case ME_RIGHTDOWN:
            Msg.message = MSG_RBUTTONDOWN;
            break;
        case ME_RIGHTUP:
            Msg.message = MSG_RBUTTONUP;
            break;
        case ME_RIGHTDBLCLICK:
            Msg.message = MSG_RBUTTONDBLCLK;
            break;
        case ME_MIDDLEDOWN:
            Msg.message = MSG_MBUTTONDOWN;
            break;
        case ME_MIDDLEUP:
            Msg.message = MSG_MBUTTONUP;
            break;
        case ME_MIDDLEDBLCLICK:
            Msg.message = MSG_MBUTTONDBLCLK;
            break;
        }

        if (me->event != ME_MOVED && (mouse_x != me->x || mouse_y != me->y)) {
            int old = Msg.message;

            Msg.message = MSG_MOUSEMOVE;
            if (__mg_check_hook_func (HOOK_EVENT_MOUSE, &Msg) == HOOK_GOON)
                QueueDeskMessage (&Msg);
            Msg.message = old;

            mouse_x = me->x; mouse_y = me->y;
        }

        if (__mg_check_hook_func (HOOK_EVENT_MOUSE, &Msg) == HOOK_GOON)
            QueueDeskMessage (&Msg);
    }
}


extern struct timeval __mg_event_timeout;

static void* EventLoop (void* data)
{
    LWEVENT lwe;
    int event;

    lwe.data.me.x = 0; lwe.data.me.y = 0;

    sem_post ((sem_t*)data);

    while (_is_minigui_running) {
        EXTRA_INPUT_EVENT extra;

#if 0   /* deprecated code */
#ifndef __USE_TIMER_THREAD
        /* VM: Since 4.0.0, update timer counter in event loop */
        if (__mg_quiting_stage > _MG_QUITING_STAGE_TIMER) {
            extern void __mg_timer_action (void *data);
            __mg_timer_action (NULL);
        }
#endif
#endif  /* deprecated code */

        extra.params_mask = 0;
        event = IAL_WaitEvent (0, NULL, NULL, NULL, &__mg_event_timeout,
                &extra);
        if (event < 0)
            continue;

        lwe.status = 0L;
        lwe.data.me.status = 0;
        if (event & IAL_MOUSEEVENT && kernel_GetLWEvent (IAL_MOUSEEVENT, &lwe))
            ParseEvent (&lwe);

        lwe.status = 0L;
        lwe.data.ke.status = 0;
        if (event & IAL_KEYEVENT && kernel_GetLWEvent (IAL_KEYEVENT, &lwe))
            ParseEvent (&lwe);

        // for extra input events; since 4.0.0
        if (event & IAL_EVENT_EXTRA) {
            MSG msg;
            msg.hwnd = HWND_DESKTOP;
            msg.message = extra.event;
            msg.wParam = extra.wparam;
            msg.lParam = extra.lparam;
            msg.time = __mg_tick_counter;
            if (extra.params_mask) {
                // packed multiple sub events
                int i, n = 0;
                for (i = 0; i < NR_PACKED_SUB_EVENTS; i++) {
                    if (extra.params_mask & (1 << i)) {
                        msg.wParam = extra.wparams[i];
                        msg.lParam = extra.lparams[i];
                        if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) ==
                                HOOK_GOON)
                        QueueDeskMessage (&msg);
                        n++;
                    }
                }

                if (n > 0) {
                    msg.message = MSG_EXIN_END_CHANGES;
                    msg.wParam = n;
                    msg.lParam = 0;
                    if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) ==
                            HOOK_GOON)
                        QueueDeskMessage (&msg);
                }
            }
            else {
                if (__mg_check_hook_func (HOOK_EVENT_EXTRA, &msg) == HOOK_GOON)
                    QueueDeskMessage (&msg);
            }
        }
        else if (event == 0 && kernel_GetLWEvent (0, &lwe))
            ParseEvent (&lwe);
    }

    return NULL;
}

/* The following functions were moved to src/include/internals.h as inline.
static inline BOOL createThreadInfoKey (void)
{
    if (pthread_key_create (&__mg_threadinfo_key, NULL))
        return FALSE;
    return TRUE;
}

static inline void deleteThreadInfoKey (void)
{
    pthread_key_delete (__mg_threadinfo_key);
}
*/

/************************** System Initialization ****************************/
static BOOL SystemThreads(void)
{
    pthread_t th;
    sem_t wait;

    sem_init (&wait, 0, 0);

    // this is the thread for desktop window.
    // this thread should have a normal priority same as
    // other main window threads.
    // if there is no main window can receive the low level events,
    // this desktop window is the only one can receive the events.
    // to close a MiniGUI application, we should close the desktop
    // window.
#ifdef __NOUNIX__
    {
        pthread_attr_t new_attr;
        pthread_attr_init (&new_attr);
        pthread_attr_setstacksize (&new_attr, 16 * 1024);
        pthread_create (&th, &new_attr, __kernel_desktop_main, &wait);
        pthread_attr_destroy (&new_attr);
    }
#else
    pthread_create (&th, NULL, __kernel_desktop_main, &wait);
#endif

    sem_wait (&wait);

    if (__mg_dsk_msg_queue == NULL)
        return FALSE;

#if 0   /* deprecated code */
    /* Since 5.0.0, we no longer use the timer thread */
    __mg_timer_init ();
#endif  /* deprecated code */

    // this thread collect low level event from outside,
    // if there is no event, this thread will suspend to wait a event.
    // the events maybe mouse event, keyboard event, or timeout event.
    //
    // this thread also parse low level event and translate it to message,
    // then post the message to the approriate message queue.
    // this thread should also have a higher priority.
    pthread_create (&_th_parsor, NULL, EventLoop, &wait);
    // XXX: Since 5.0.0, event parsor should be joinable
    //pthread_detach (_th_parsor);

    sem_wait (&wait);

    sem_destroy (&wait);
    return TRUE;
}

BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
{
    return SendMessage (HWND_DESKTOP, MSG_REINITSESSION, init_sys_text, 0) == 0;
}

#ifndef __NOUNIX__
static struct termios savedtermio;

void* GUIAPI GetOriginalTermIO (void)
{
    return &savedtermio;
}

static void sig_handler (int v)
{
    if (v == SIGSEGV) {
        kill (getpid(), SIGABRT); /* cause core dump */
    }
    else if (v == SIGINT) {
        _exit(1); /* force to quit */
    }
#if 0   /* since 5.0.0, deprecated code */
    else if (v == SIGTERM) {
        ExitGUISafely(-1);
    }
#endif  /* since 5.0.0, deprecated code */
    else {
        exit(1); /* force to quit */
    }
}

static BOOL InstallSEGVHandler (void)
{
    struct sigaction siga;

    siga.sa_handler = sig_handler;
    siga.sa_flags = 0;

    memset (&siga.sa_mask, 0, sizeof (sigset_t));
    sigaction (SIGSEGV, &siga, NULL);
    sigaction (SIGTERM, &siga, NULL);
    sigaction (SIGINT, &siga, NULL);
    sigaction (SIGPIPE, &siga, NULL);

    return TRUE;
}
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

static IDLEHANDLER std_idle_handler;

static BOOL idle_handler_for_main_thread (MSGQUEUE *msg_queue, BOOL wait)
{
    __mg_update_tick_count (NULL);
    return std_idle_handler (msg_queue, wait);
}

int GUIAPI InitGUI (int args, const char *agr[])
{
    char engine [LEN_ENGINE_NAME + 1];
    char mode [LEN_VIDEO_MODE + 1];
    int step = 0;
    MSGQUEUE* msg_queue;

#ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "C");
#endif

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)
    step++;
    if (_sysvipc_mutex_sem_init ())
        return step;
#endif

#ifndef __NOUNIX__
    // Save original termio
    tcgetattr (0, &savedtermio);
#endif

    /*init default window process*/
    __mg_def_proc[0] = PreDefMainWinProc;
    __mg_def_proc[1] = PreDefDialogProc;
    __mg_def_proc[2] = PreDefControlProc;
#ifdef _MGHAVE_VIRTUAL_WINDOW
    __mg_def_proc[3] = PreDefVirtualWinProc;
#endif

    step++;
    if (!mg_InitSliceAllocator ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init slice allocator!\n");
        return step;
    }

    step++;
    if (!mg_InitFixStr ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init fixed string heap!\n");
        return step;
    }

    step++;
    /* Init miscelleous*/
    if (!mg_InitMisc ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init misc module!\n");
        return step;
    }

    step++;
    switch (mg_InitGAL (engine, mode)) {
    case ERR_CONFIG_FILE:
        _ERR_PRINTF ("KERNEL>InitGUI: Reading configuration failure!\n");
        return step;

    case ERR_NO_ENGINE:
        _ERR_PRINTF ("KERNEL>InitGUI: No graphics engine defined!\n");
        return step;

    case ERR_NO_MATCH:
        _ERR_PRINTF ("KERNEL>InitGUI: Cannot get graphics engine info!\n");
        return step;

    case ERR_GFX_ENGINE:
        _ERR_PRINTF ("KERNEL>InitGUI: Cannot init graphics engine!\n");
        return step;
    }

#ifndef __NOUNIX__
    InstallSEGVHandler ();
#endif

    /* Init GDI. */
    step++;
    if(!mg_InitGDI()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Initialization of GDI failure!\n");
        goto failure1;
    }

    /* Init Master Screen DC here */
    step++;
    if (!mg_InitScreenDC ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Can not init screen DC!\n");
        goto failure1;
    }

    /*
     * Load system resource here.
     */
    step++;
    if (!mg_InitSystemRes ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Can not init system resource!\n");
        goto failure1;
    }

    __mg_license_create();
    __mg_splash_draw_framework();

    /* Init mouse cursor. */
    step++;
    if( !mg_InitCursor() ) {
        _ERR_PRINTF ("KERNEL>InitGUI: Count not init mouse cursor!\n");
        goto failure1;
    }
    __mg_splash_progress();

    /* Init low level event */
    step++;
    if(!mg_InitLWEvent()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init low level event!\n");
        goto failure1;
    }
    __mg_splash_progress();

    /** Init LF Manager */
    step++;
    if (!mg_InitLFManager ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init LF Manager!\n");
        goto failure;
    }
    __mg_splash_progress();

#ifdef _MGHAVE_MENU
    /* Init menu */
    step++;
    if (!mg_InitMenu ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init menu module!\n");
        goto failure;
    }
#endif

    /* Init control class */
    step++;
    if(!mg_InitControlClass()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init control classes!\n");
        goto failure;
    }
    __mg_splash_progress();

    /* Init accelerator */
    step++;
    if(!mg_InitAccel()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init accelerator!\n");
        goto failure;
    }
    __mg_splash_progress();

    step++;
    if (!mg_InitDesktop ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init desktop!\n");
        goto failure;
    }
    __mg_splash_progress();

    step++;
    if (!mg_InitFreeQMSGList ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init free QMSG heap!\n");
        goto failure;
    }
    __mg_splash_progress();

    step++;
    if (!createThreadInfoKey ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init thread key!\n");
        goto failure;
    }

    __mg_splash_delay();

    _is_minigui_running = 1;

    step++;
    if (!SystemThreads()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to init system threads!\n");
        goto failure;
    }

    /* init message queue of main GUI thread */
    step++;
    if (!(msg_queue = mg_AllocMsgQueueForThisThread ())) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to allocate message queue!\n");
        goto failure;
    }

    /* for threads mode, the idle handler for the main threads should
       call __mg_update_tick_count () */
    std_idle_handler = msg_queue->OnIdle;
    msg_queue->OnIdle = idle_handler_for_main_thread;

    /* init timer for tick counter */
    step++;
    if (!mg_InitTimer (FALSE)) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to start time counter\n");
        goto failure;
    }

    SetKeyboardLayout ("default");

    SetCursor (GetSystemCursor (IDC_ARROW));

    SetCursorPos (g_rcScr.right >> 1, g_rcScr.bottom >> 1);

    mg_TerminateMgEtc ();
    return 0;

failure:
    mg_TerminateLWEvent ();
failure1:
    mg_TerminateGAL ();
    _ERR_PRINTF ("KERNEL>InitGUI: initialization failed; please check "
            "your MiniGUI configuration or resource.\n");
    return step;
}

void GUIAPI TerminateGUI (int not_used)
{
    mg_TerminateTimer (FALSE);

    /* Since 5.0.0 */
    SendNotifyMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    pthread_join (__mg_dsk_msg_queue->th, NULL);

    /* Tell event parsor quit */
    _is_minigui_running = 0;
    pthread_cancel (_th_parsor);
    pthread_join (_th_parsor, NULL);

    __mg_license_destroy();

    mg_TerminateDesktop ();

    mg_DestroyFreeQMSGList ();
    mg_TerminateAccel ();
    mg_TerminateControlClass ();
#ifdef _MGHAVE_MENU
    mg_TerminateMenu ();
#endif
    mg_TerminateMisc ();
    mg_TerminateFixStr ();

#ifdef _MGHAVE_CURSOR
    mg_TerminateCursor ();
#endif
    mg_TerminateLWEvent ();

    mg_TerminateScreenDC ();
    mg_TerminateLFManager ();
    mg_TerminateSystemRes ();
    mg_TerminateGDI ();
    mg_TerminateGAL ();

#ifdef _MGHAVE_ADV_2DAPI
    extern void miFreeArcCache (void);
    miFreeArcCache ();
#endif

    mg_FreeMsgQueueForThisThread ();

    deleteThreadInfoKey ();

    mg_TerminateSliceAllocator();

    /*
     * Restore original termio
     *tcsetattr (0, TCSAFLUSH, &savedtermio);
     */

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)
    _sysvipc_mutex_sem_term ();
#endif
}

/* XXX: We need a better policy to exit MiniGUI safely by giving a chance
   for all windows to save data. */

/* see src/kernel/desktop-*.c */
#define IDM_CLOSEALLWIN (MINID_RESERVED + 1)

void GUIAPI ExitGUISafely (int exitcode)
{
    SendNotifyMessage (HWND_DESKTOP, MSG_COMMAND, IDM_CLOSEALLWIN, 0);
    __mg_join_all_message_threads ();
    SendMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    pthread_join (__mg_dsk_msg_queue->th, NULL);

    /* Tell event parsor quit */
    _is_minigui_running = 0;
    pthread_cancel (_th_parsor);
    pthread_join (_th_parsor, NULL);
}

#endif /* _MGRM_THREADS */

