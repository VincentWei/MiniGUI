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
#endif
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

#define err_message(step, message) fprintf (stderr, "KERNEL>InitGUI (step %d): %s\n", step, message)
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
    int step = 1;

    __mg_quiting_stage = _MG_QUITING_STAGE_RUNNING;

#ifndef __NOUNIX__
        tcgetattr (0, &savedtermio);
#endif

    if (!mg_InitFixStr ()) {
        err_message (step, "Can not initialize Fixed String heap!\n");
        return step;
    }
    step++;

    if (!mg_InitMisc ()) {
        err_message (step, "Can not initialize miscellous things!");
        return step;
    }
    step++;

    /* Init GAL engine. */
    switch (mg_InitGAL ()) {
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
    if (!mg_InitScreenDC (__gal_screen)) {
        err_message (step, "Can not initialize screen DC!");
        return step;
    }
    step++;
    atexit (mg_TerminateScreenDC);

    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;

    mg_TerminateMgEtc ();
    return 0;
}

void TerminateGUI (int rcByGUI)
{
    mg_TerminateMisc ();
    mg_TerminateFixStr ();
}

#warning ExitGUISafely?

#else

#ifdef _MGRM_PROCESSES
  #include "sharedres.h"
  #include "client.h"
  #include "server.h"

  void* mgSharedRes;
  size_t mgSizeRes;

#endif

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
        fprintf (stderr, "KERNEL>InitGUI: Can not initialize mouse cursor!\n");
        goto failure;
    }
#endif

#ifdef _MGHAVE_MENU
    if (!mg_InitMenu ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Menu module failure!\n");
        goto failure;
    }
#endif

    if (!mg_InitControlClass ()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Control Class failure!\n");
        goto failure;
    }

    if (!mg_InitAccel ()) {
        fprintf(stderr, "KERNEL>InitGUI: Init Accelerator failure!\n");
        goto failure;
    }

    if (!mg_InitDesktop ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init Desktop error!\n");
        goto failure;
    }

    if (!mg_InitFreeQMSGList ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init free QMSG list error!\n");
        goto failure;
    }

    if (!InitDskMsgQueue ()) {
        fprintf (stderr, "KERNEL>InitGUI: Init MSG queue error!\n");
        goto failure;
    }

    return TRUE;

failure:
    return FALSE;
}

#define err_message(step, message) fprintf (stderr, "KERNEL>InitGUI (step %d): %s\n", step, message)

static void sig_handler (int v)
{
    if (v == SIGSEGV) {
#ifdef WIN32
		raise(SIGABRT);
#else
        kill (getpid(), SIGABRT); /* cause core dump */
#endif
    }else if (__mg_quiting_stage > 0) {
        ExitGUISafely(-1);
    }else{
        exit(1); /* force to quit */
    }
}

static BOOL InstallSEGVHandler (void)
{
#ifdef WIN32
	signal(SIGSEGV, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
#else
    struct sigaction siga;
    
    siga.sa_handler = sig_handler;
    siga.sa_flags = 0;
    
    memset (&siga.sa_mask, 0, sizeof (sigset_t));
    sigaction (SIGSEGV, &siga, NULL);
    sigaction (SIGTERM, &siga, NULL);
    sigaction (SIGINT, &siga, NULL);
    sigaction (SIGPIPE, &siga, NULL);
#endif
    return TRUE;
}

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

int InitGUI (int argc, const char* agr[])
{
    int step = 1;

    __mg_quiting_stage = _MG_QUITING_STAGE_RUNNING;

#ifdef _MGRM_PROCESSES
    const char* name;

    if ((name = strrchr (agr [0], '/')) == NULL)
        name = agr [0];
    else
        name ++;

    if (!strcmp (name, "mginit"))
        mgIsServer = TRUE;
#endif

#ifdef HAVE_SETLOCALE
    setlocale (LC_ALL, "C");
#endif

    /* Save original termio */
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
//#ifndef WIN32
#ifndef __NOUNIX__
        tcgetattr (0, &savedtermio);
#endif

    /*Initialize default window process*/
    __mg_def_proc[0] = PreDefMainWinProc;
    __mg_def_proc[1] = PreDefDialogProc;
    __mg_def_proc[2] = PreDefControlProc;

    if (!mg_InitFixStr ()) {
        err_message (step, "Can not initialize Fixed String heap!\n");
        return step;
    }
    step++;

    if (!mg_InitMisc ()) {
        err_message (step, "Can not initialize miscellous things!");
        return step;
    }
    step++;

#ifdef _MGRM_PROCESSES
    if (mgIsServer && !kernel_IsOnlyMe ()) {
        err_message (step, "There is already an instance of 'mginit'!");
        return step;
    }
    step++;
#endif

#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
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
    switch (mg_InitGAL ()) {
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
    if (mgIsServer)
#endif
        InstallSEGVHandler ();

    /** initialize icon bitmap resource.*/
    if (mg_InitSystemRes () == FALSE) {
        fprintf (stderr, "KERNEL>IniGUI: init system res error!\n");
        return step;
    }
    step++;
    /* DK[11/29/10]: For fix bug 4440, avoid double free. */
/*     atexit (mg_TerminateSystemRes);
 */

    if (!mg_InitGDI ()) {
        err_message (step, "Initialization of GDI resource failure!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateGDI);

    /* Init Master Screen DC here */
    if (!mg_InitScreenDC (__gal_screen)) {
        err_message (step, "Can not initialize screen DC!");
        return step;
    }
    step++;
    atexit (mg_TerminateScreenDC);

    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN_SYS, GDCAP_MAXY) + 1;

#ifdef _MGRM_PROCESSES
    if (mgIsServer) {
        /* Load shared resource and create shared memory. */
        if ((mgSharedRes = kernel_LoadSharedResource ()) == NULL) {
            err_message (step, "Can not load shared resource!");
            return step;
        }
        atexit (kernel_UnloadSharedResource);
        SHAREDRES_TERMIOS = savedtermio;
    }

    step++;
#endif

    if (!mg_InitLFManager ()) {
        err_message (step, "Can not initialize LFManager!\n");
        return step;
    }
    step++;
    atexit (mg_TerminateLFManager);


#ifdef _MGRM_PROCESSES
    if (mgIsServer) 
#endif
    {
        license_create();
        splash_draw_framework(); 
        splash_delay();
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
        fprintf (stderr, "KERNEL>InitGUI: Can not start MiniGUI-StandAlone version.\n");
        return step;
    }

#else

    if (mgIsServer) {
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
#endif

    SetKeyboardLayout ("default");

    mg_TerminateMgEtc ();
    return 0;
}

void TerminateGUI (int rcByGUI)
{
    /* printf("Quit from MiniGUIMain()\n"); */

    DestroyDskMsgQueue ();
    mg_DestroyFreeQMSGList ();
    mg_TerminateAccel ();
    mg_TerminateControlClass ();

#ifdef _MGHAVE_MENU
    mg_TerminateMenu ();
#endif
#ifdef _MGHAVE_CURSOR
    mg_TerminateCursor ();
#endif
    mg_TerminateMisc ();
    mg_TerminateFixStr ();

#ifdef _MGRM_PROCESSES
    if (mgIsServer) 
#endif
    {
        license_destroy();
    }

#ifdef _MGRM_STANDALONE
    mg_TerminateDesktop ();

    salone_StandAloneCleanup ();
#else
    if (mgIsServer) {
        mg_TerminateDesktop ();

        /* Cleanup UNIX domain socket and other IPC objects. */
        server_ServerCleanup ();
    }
    else {
        client_ClientCleanup ();
    }
#endif
}
#endif /* ifdef _MG_MINIMALGDI */

#endif /* !_MGRM_THREADS */
