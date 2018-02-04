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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
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

int __mg_quiting_stage;

#ifdef _MGRM_THREADS
int __mg_enter_terminategui;

/******************************* extern data *********************************/
extern void* DesktopMain (void* data);

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

    Msg.time = __mg_timer_counter;

    if (lwe->type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe->count;
        Msg.lParam = 0;
        QueueDeskMessage (&Msg);
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
        }

        if (me->event != ME_MOVED && (mouse_x != me->x || mouse_y != me->y)) {
            int old = Msg.message;

            Msg.message = MSG_MOUSEMOVE;
            QueueDeskMessage (&Msg);
            Msg.message = old;

            mouse_x = me->x; mouse_y = me->y;
        }

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

    while (__mg_quiting_stage > _MG_QUITING_STAGE_EVENT) {
        event = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT, 0,
                        NULL, NULL, NULL, (void*)&__mg_event_timeout);
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

        if (event == 0 && kernel_GetLWEvent (0, &lwe))
            ParseEvent (&lwe);
    }

    /* printf("Quit from EventLoop()\n"); */
    return NULL;
}

/************************** Thread Information  ******************************/

pthread_key_t __mg_threadinfo_key;

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

MSGQUEUE* mg_InitMsgQueueThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    if (!(pMsgQueue = malloc(sizeof(MSGQUEUE))) ) {
        return NULL;
    }

    if (!mg_InitMsgQueue(pMsgQueue, 0)) {
        free (pMsgQueue);
        return NULL;
    }

    pthread_setspecific (__mg_threadinfo_key, pMsgQueue);
    return pMsgQueue;
}

void mg_FreeMsgQueueThisThread (void)
{
    MSGQUEUE* pMsgQueue;

    pMsgQueue = pthread_getspecific (__mg_threadinfo_key);
#ifdef __VXWORKS__
    if (pMsgQueue != (void *)0 && pMsgQueue != (void *)-1) {
#else
    if (pMsgQueue) {
#endif
        mg_DestroyMsgQueue (pMsgQueue);
        free (pMsgQueue);
#ifdef __VXWORKS__
        pthread_setspecific (__mg_threadinfo_key, (void*)-1);
#else
        pthread_setspecific (__mg_threadinfo_key, NULL);
#endif
    }
}

/*
The following function is moved to src/include/internals.h as an inline 
function.
MSGQUEUE* GetMsgQueueThisThread (void)
{
    return (MSGQUEUE*) pthread_getspecific (__mg_threadinfo_key);
}
*/

/************************** System Initialization ****************************/

int __mg_timer_init (void);

static BOOL SystemThreads(void)
{
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
        pthread_create (&__mg_desktop, &new_attr, DesktopMain, &wait);
        pthread_attr_destroy (&new_attr);
    }
#else
    pthread_create (&__mg_desktop, NULL, DesktopMain, &wait);
#endif

    sem_wait (&wait);

    __mg_timer_init ();

    // this thread collect low level event from outside,
    // if there is no event, this thread will suspend to wait a event.
    // the events maybe mouse event, keyboard event, or timeout event.
    //
    // this thread also parse low level event and translate it to message,
    // then post the message to the approriate message queue.
    // this thread should also have a higher priority.
    pthread_create (&__mg_parsor, NULL, EventLoop, &wait);
    pthread_detach (__mg_parsor);
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
    }else if (__mg_quiting_stage > 0) {
        ExitGUISafely(-1);
    }else{
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

int GUIAPI InitGUI (int args, const char *agr[])
{
    int step = 0;

    __mg_quiting_stage = _MG_QUITING_STAGE_RUNNING;
    __mg_enter_terminategui = 0;

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

    /*initialize default window process*/
    __mg_def_proc[0] = PreDefMainWinProc;
    __mg_def_proc[1] = PreDefDialogProc;
    __mg_def_proc[2] = PreDefControlProc;

    step++;
    if (!mg_InitFixStr ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Fixed String module failure!\n");
        return step;
    }
    
    step++;
    /* Init miscelleous*/
    if (!mg_InitMisc ()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of misc things failure!\n");
        return step;
    }

    step++;
    switch (mg_InitGAL ()) {
    case ERR_CONFIG_FILE:
        fprintf (stderr, 
            "KERNEL>InitGUI: Reading configuration failure!\n");
        return step;

    case ERR_NO_ENGINE:
        fprintf (stderr, 
            "KERNEL>InitGUI: No graphics engine defined!\n");
        return step;

    case ERR_NO_MATCH:
        fprintf (stderr, 
            "KERNEL>InitGUI: Can not get graphics engine information!\n");
        return step;

    case ERR_GFX_ENGINE:
        fprintf (stderr, 
            "KERNEL>InitGUI: Can not initialize graphics engine!\n");
        return step;
    }

#ifndef __NOUNIX__
    InstallSEGVHandler ();
#endif

    /*
     * Load system resource here.
     */
    step++;
    if (!mg_InitSystemRes ()) {
        fprintf (stderr, "KERNEL>InitGUI: Can not initialize system resource!\n");
        goto failure1;
    }

    /* Init GDI. */
    step++;
    if(!mg_InitGDI()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of GDI failure!\n");
        goto failure1;
    }

    /* Init Master Screen DC here */
    step++;
    if (!mg_InitScreenDC (__gal_screen)) {
        fprintf (stderr, "KERNEL>InitGUI: Can not initialize screen DC!\n");
        goto failure1;
    }

    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;

    license_create();
    splash_draw_framework(); 

    /* Init mouse cursor. */
    step++;
    if( !mg_InitCursor() ) {
        fprintf (stderr, "KERNEL>InitGUI: Count not init mouse cursor!\n");
        goto failure1;
    }
    splash_progress();

    /* Init low level event */
    step++;
    if(!mg_InitLWEvent()) {
        fprintf(stderr, "KERNEL>InitGUI: Low level event initialization failure!\n");
        goto failure1;
    }
    splash_progress();

    /** Init LF Manager */
    step++;
    if (!mg_InitLFManager ()) {
        fprintf (stderr, "KERNEL>InitGUI: Initialization of LF Manager failure!\n");
        goto failure;
    }
    splash_progress();

#ifdef _MGHAVE_MENU
    /* Init menu */
    step++;
    if (!mg_InitMenu ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Menu module failure!\n");
        goto failure;
    }
#endif

    /* Init control class */
    step++;
    if(!mg_InitControlClass()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Control Class failure!\n");
        goto failure;
    }
    splash_progress();

    /* Init accelerator */
    step++;
    if(!mg_InitAccel()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Accelerator failure!\n");
        goto failure;
    }
    splash_progress();

    step++;
    if (!mg_InitDesktop ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Desktop failure!\n");
        goto failure;
    }
    splash_progress();
   
    step++;
    if (!mg_InitFreeQMSGList ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init free QMSG list failure!\n");
        goto failure;
    }
    splash_progress();

    step++;
    if (!createThreadInfoKey ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init thread hash table failure!\n");
        goto failure;
    }

    splash_delay();

    step++;
    if (!SystemThreads()) {
        fprintf (stderr, "KERNEL>InitGUI: Init system threads failure!\n");
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
    fprintf (stderr, "KERNEL>InitGUI: Init failure, please check "
            "your MiniGUI configuration or resource.\n");
    return step;
}

void GUIAPI TerminateGUI (int not_used)
{
    /* printf("Quit from MiniGUIMain()\n"); */

    __mg_enter_terminategui = 1;

    pthread_join (__mg_desktop, NULL);

    /* DesktopProc() will set __mg_quiting_stage to _MG_QUITING_STAGE_EVENT */
    pthread_join (__mg_parsor, NULL);

    deleteThreadInfoKey ();
    license_destroy();

    __mg_quiting_stage = _MG_QUITING_STAGE_TIMER;
    mg_TerminateTimer ();

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
    mg_TerminateGDI ();
    mg_TerminateLFManager ();
    mg_TerminateGAL ();

#ifdef _MGHAVE_ADV_2DAPI
    extern void mg_miFreeArcCache (void);
    mg_miFreeArcCache ();
#endif

    /* 
     * Restore original termio
     *tcsetattr (0, TCSAFLUSH, &savedtermio);
     */

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)
    _sysvipc_mutex_sem_term ();
#endif
}

#endif /* _MGRM_THREADS */

void GUIAPI ExitGUISafely (int exitcode)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
#   define IDM_CLOSEALLWIN (MINID_RESERVED + 1) /* see src/kernel/desktop-*.c */
        SendNotifyMessage(HWND_DESKTOP, MSG_COMMAND, IDM_CLOSEALLWIN, 0);
        SendNotifyMessage(HWND_DESKTOP, MSG_ENDSESSION, 0, 0);

        if (__mg_quiting_stage > 0) {
            __mg_quiting_stage = _MG_QUITING_STAGE_START;
        }
    }
}
