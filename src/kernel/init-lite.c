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
** init-lite.c: The Initialization/Termination routines for
** MiniGUI-Processes and MiniGUI-Standalone.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifndef _MGRM_THREADS

#ifndef WIN32
#include <unistd.h>
#include <sys/termios.h>
#endif /* WIN32 */

#include <signal.h>

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "cursor.h"
#include "event.h"
#include "misc.h"
#include "menu.h"
#include "timer.h"
#include "accelkey.h"
#include "license.h"

/* Minimal graphics support by humingming 2010.7.8 */
#ifdef _MG_MINIMALGDI
static struct termios savedtermio;

#define err_message(step, message) _ERR_PRINTF ("KERNEL>InitGUI (step %d): %s\n", step, message)
static void sig_handler (int v)
{
    ;
}

/* for pc_xvfb test by humingming 2010.7.9 */
#include "dc.h"
void kernel_ShowCursorForGDI(BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (fShow)
        GAL_UpdateRect (cur_pdc->surface,
                        prc->left, prc->top, RECTWP(prc), RECTHP(prc));
}

int InitGUI (int argc, const char* agr[])
{
    char engine [LEN_ENGINE_NAME + 1];
    char mode [LEN_VIDEO_MODE + 1];
    int step = 1;

#ifndef __NOUNIX__
        tcgetattr (0, &savedtermio);
#endif

    step++;
    if (!mg_InitSliceAllocator ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to initialize slice allocator!\n");
        return step;
    }
    atexit (mg_TerminateSliceAllocator);

    if (!mg_InitFixStr ()) {
        err_message (step, "Can not initialize Fixed String heap!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateFixStr);

    if (!mg_InitMisc ()) {
        err_message (step, "Can not initialize miscellous things!");
        return step;
    }
    step++;
    atexit (mg_TerminateMisc);

    /* Init GAL engine. */
    switch (mg_InitGAL (engine, mode)) {
    case ERR_CONFIG_FILE:
        err_message (step, "Reading configuration failure!");
        return step;

    case ERR_NO_ENGINE:
        err_message (step, "No graphics engine defined!");
        return step;

    case ERR_NO_MATCH:
        err_message (step, "Can not get graphics engine information!");
        return step;

    case ERR_GFX_ENGINE:
        err_message (step, "Can not initialize graphics engine!");
        return step;
    }
    step++;
    atexit (mg_TerminateGAL);

    /* Init Master Screen DC here */
    if (!mg_InitScreenDC ()) {
        err_message (step, "Can not initialize screen DC!");
        return step;
    }
    step++;
    atexit (mg_TerminateScreenDC);

#if 0
    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;
#endif

    mg_TerminateMgEtc ();
    return 0;
}

void TerminateGUI (int rcByGUI)
{
}

#warning ExitGUISafely?

#else /* _MG_MINIMALGDI */

#ifdef _MGRM_PROCESSES
#include "sharedres.h"
#include "client.h"
#include "server.h"

void* mgSharedRes;
size_t mgSizeRes;
#endif /* _MGRM_PROCESSES */

BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
{
    return SendMessage (HWND_DESKTOP, MSG_REINITSESSION, init_sys_text, 0) == 0;
}

#ifndef WIN32
static struct termios savedtermio;
#endif

void* GUIAPI GetOriginalTermIO (void)
{
#ifdef _MGRM_STANDALONE
#ifdef WIN32
    return NULL;
#else
    return &savedtermio;
#endif
#else
    return &SHAREDRES_TERMIOS;
#endif
}

static BOOL InitResource (void)
{
#ifdef _MGHAVE_CURSOR
    if (!mg_InitCursor ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Can not initialize mouse cursor!\n");
        goto failure;
    }
#endif

#ifdef _MGHAVE_MENU
    if (!mg_InitMenu ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Init Menu module failure!\n");
        goto failure;
    }
#endif

    if (!mg_InitControlClass ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Init Control Class failure!\n");
        goto failure;
    }

    if (!mg_InitAccel ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Init Accelerator failure!\n");
        goto failure;
    }

    if (!mg_InitDesktop ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Init Desktop error!\n");
        goto failure;
    }

    if (!mg_InitFreeQMSGList ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Init free QMSG list error!\n");
        goto failure;
    }

    return TRUE;

failure:
    return FALSE;
}

#define err_message(step, message)      \
    _ERR_PRINTF ("KERNEL>InitGUI (step %d): %s\n", step, message)

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
        ExitGUISafely (-1);
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

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef _MGRM_PROCESSES
static BOOL _is_server = FALSE;

BOOL IsServer(void)
{
    return _is_server;
}

#endif

int InitGUI (int argc, const char* agr[])
{
    char engine [LEN_ENGINE_NAME + 1];
    char mode [LEN_VIDEO_MODE + 1];
    int step = 1;

#ifdef _MGRM_PROCESSES
    const char* name;

    if ((name = strrchr (agr [0], '/')) == NULL)
        name = agr [0];
    else
        name ++;

    if (!strcmp (name, "mginit"))
        _is_server = TRUE;
#endif

#ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "C");
#endif

    /* Save original termio */
#ifdef _MGRM_PROCESSES
    if (_is_server)
#endif
#ifndef __NOUNIX__
        tcgetattr (0, &savedtermio);
#endif

    /*Initialize default window process*/
    __mg_def_proc[0] = PreDefMainWinProc;
    __mg_def_proc[1] = PreDefDialogProc;
    __mg_def_proc[2] = PreDefControlProc;
#ifdef _MGHAVE_VIRTUAL_WINDOW
    __mg_def_proc[3] = PreDefVirtualWinProc;
#endif

    if (!mg_InitSliceAllocator ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: failed to initialize slice allocator!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateSliceAllocator);

    if (!mg_InitFixStr ()) {
        err_message (step, "Can not initialize Fixed String heap!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateFixStr);

    if (!mg_InitMisc ()) {
        err_message (step, "Can not initialize miscellous things!");
        return step;
    }
    step++;
    atexit (mg_TerminateMisc);

#ifdef _MGRM_PROCESSES
    if (_is_server && !kernel_IsOnlyMe ()) {
        err_message (step, "There is already an instance of 'mginit'!");
        return step;
    }
    step++;
#endif

#ifdef _MGHAVE_VIRTUAL_WINDOW
    if (!createThreadInfoKey ()) {
        err_message (step, "Failed to init pthread key.\n");
        return step;
    }
    step++;
#endif

    /* Since 5.0.0
       allocate message queue for desktop thread before starting client. */
    if (!(__mg_dsk_msg_queue = mg_AllocMsgQueueForThisThread ()) ) {
        err_message (step, "Failed to allocate message queue!");
        return step;
    }
    step++;

#ifdef _MGRM_PROCESSES
    if (!_is_server) {
        if (!client_ClientStartup ()) {
            err_message (step, "Can not start client (Please run mginit first)!");
            return step;
        }
        step++;

        if ((mgSharedRes = kernel_AttachSharedResource ()) == NULL) {
            err_message (step, "Can not attach shared resource (Please run mginit first)!");
            return step;
        }
        atexit (kernel_UnattachSharedResource);
    }
#endif

    /* Init GAL engine. */
    switch (mg_InitGAL (engine, mode)) {
    case ERR_CONFIG_FILE:
        err_message (step, "Reading configuration failure!");
        return step;

    case ERR_NO_ENGINE:
        err_message (step, "No graphics engine defined!");
        return step;

    case ERR_NO_MATCH:
        err_message (step, "Can not get graphics engine information!");
        return step;

    case ERR_GFX_ENGINE:
        err_message (step, "Can not initialize graphics engine!");
        return step;
    }
    step++;
    atexit (mg_TerminateGAL);

    /* install signal handlers */
#ifdef _MGRM_PROCESSES
    if (_is_server)
#endif
        InstallSEGVHandler ();

    if (!mg_InitGDI ()) {
        err_message (step, "Initialization of GDI resource failure!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateGDI);

    /* Init Master Screen DC here */
    if (!mg_InitScreenDC ()) {
        err_message (step, "Can not initialize screen DC!");
        return step;
    }
    step++;
    atexit (mg_TerminateScreenDC);

    /** initialize icon bitmap resource.*/
    if (mg_InitSystemRes () == FALSE) {
        _ERR_PRINTF ("KERNEL>IniGUI: init system res error!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateSystemRes);

#if 0
    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;
#endif

#ifdef _MGRM_PROCESSES
    if (_is_server) {
        /* Load shared resource and create shared memory. */
        if ((mgSharedRes = kernel_LoadSharedResource ()) == NULL) {
            err_message (step, "Can not load shared resource!");
            return step;
        }
        atexit (kernel_UnloadSharedResource);
        SHAREDRES_TERMIOS = savedtermio;

        /* Since 5.0.0
         * Copy the video engine information to the shared resource segement
         */
        strncpy (SHAREDRES_VIDEO_ENGINE, engine, LEN_ENGINE_NAME);
        strncpy (SHAREDRES_VIDEO_MODE, mode, LEN_VIDEO_MODE);
#if 0   /* deprecated code */
        // these fileds will be initialized by DRM engine.
        if (GetMgEtcValue (engine, "device",
                    SHAREDRES_VIDEO_DEVICE, LEN_DEVICE_NAME) < 0)
            *SHAREDRES_VIDEO_DEVICE = 0;
        if (GetMgEtcValue (engine, "pixelformat",
                    SHAREDRES_VIDEO_FOURCC, LEN_FOURCC_FORMAT) < 0)
            *SHAREDRES_VIDEO_FOURCC = 0;
        if (GetMgEtcValue (engine, "exdriver",
                    SHAREDRES_VIDEO_EXDRIVER, LEN_EXDRIVER_NAME) < 0)
            *SHAREDRES_VIDEO_EXDRIVER = 0;
#endif  /* deprecated code */

        SHAREDRES_VIDEO_DPI   = __gal_screen->dpi;
        SHAREDRES_VIDEO_HRES  = __gal_screen->w;
        SHAREDRES_VIDEO_VRES  = __gal_screen->h;
        SHAREDRES_VIDEO_DEPTH = __gal_screen->format->BitsPerPixel;
        SHAREDRES_VIDEO_RMASK = __gal_screen->format->Rmask;
        SHAREDRES_VIDEO_GMASK = __gal_screen->format->Gmask;
        SHAREDRES_VIDEO_BMASK = __gal_screen->format->Bmask;
        SHAREDRES_VIDEO_AMASK = __gal_screen->format->Amask;

        GAL_CopyVideoInfoToSharedRes();
    }
    else {
        _DBG_PRINTF("Engien info from shared resource: %s %s %d\n",
                SHAREDRES_VIDEO_ENGINE,
                SHAREDRES_VIDEO_MODE, SHAREDRES_VIDEO_DPI);
    }

    step++;
#endif /* _MGRM_PROCESSES */

    if (!mg_InitLFManager ()) {
        err_message (step, "Can not initialize LFManager!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateLFManager);


#ifdef _MGRM_PROCESSES
    if (_is_server)
#endif
    {
        __mg_license_create();
        __mg_splash_draw_framework();
        __mg_splash_delay();
    }

    /* Initialize resource */
    if (!InitResource ()) {
        err_message (step, "Can not initialize resource!");
        return step;
    }
    step++;

#ifdef _MGRM_STANDALONE
    /* Init IAL engine.. */
    if (!mg_InitLWEvent ()) {
        err_message (step, "Can not initialize low level event!");
        return step;
    }
    step++;
    atexit (mg_TerminateLWEvent);

    SetDskIdleHandler (salone_IdleHandler4StandAlone);
    if (!salone_StandAloneStartup ()) {
        _ERR_PRINTF ("KERNEL>InitGUI: Can not start MiniGUI-StandAlone version.\n");
        return step;
    }

#else   /* not defined _MGRM_STANDALONE */
    if (_is_server) {
        /* Init IAL engine.. */
        if (!mg_InitLWEvent ()) {
            err_message (step, "Can not initialize low level event!");
            return step;
        }
        step++;

        atexit (mg_TerminateLWEvent);

        SetDskIdleHandler (server_IdleHandler4Server);
    }
    else {
        SetDskIdleHandler (client_IdleHandler4Client);
    }
#endif   /* not defined _MGRM_STANDALONE */

    SetKeyboardLayout ("default");

    mg_TerminateMgEtc ();

    return 0;
}

void TerminateGUI (int rcByGUI)
{
#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* Since 5.0.0 */
    __mg_join_all_message_threads ();
#endif

    mg_TerminateAccel ();
    mg_TerminateControlClass ();

#ifdef _MGHAVE_MENU
    mg_TerminateMenu ();
#endif
#ifdef _MGHAVE_CURSOR
    mg_TerminateCursor ();
#endif

#ifdef _MGRM_PROCESSES
    if (_is_server)
#endif
    {
        __mg_license_destroy();
    }

#ifdef _MGRM_STANDALONE
    mg_TerminateDesktop ();

    salone_StandAloneCleanup ();
#else
    if (_is_server) {
        mg_TerminateDesktop ();

        /* Cleanup UNIX domain socket and other IPC objects. */
        server_ServerCleanup ();
    }
    else {
        client_ClientCleanup ();
    }
#endif

    mg_DestroyFreeQMSGList ();

#ifdef _MGHAVE_ADV_2DAPI
    extern void miFreeArcCache (void);
    miFreeArcCache ();
#endif

#ifdef _MGHAVE_VIRTUAL_WINDOW
    deleteThreadInfoKey();
#endif

}
#endif /* ifndef _MG_MINIMALGDI */

/* XXX: We need a better policy to exit MiniGUI safely by giving a chance
   for all windows to save data. */
void GUIAPI ExitGUISafely (int exitcode)
{
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
    {
/* see src/kernel/desktop-*.c */
#   define IDM_CLOSEALLWIN (MINID_RESERVED + 1) 

        SendNotifyMessage (HWND_DESKTOP, MSG_COMMAND, IDM_CLOSEALLWIN, 0);
#ifdef _MGHAVE_VIRTUAL_WINDOW
        __mg_join_all_message_threads ();
        SendMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
        pthread_join (__mg_dsk_msg_queue->th, NULL);
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */
    }
}

#endif /* !_MGRM_THREADS */
