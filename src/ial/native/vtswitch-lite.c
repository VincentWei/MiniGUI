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
** vtswitch-lite.c: VT switching.
**
** Created by Song Lixin, 2000/10/17
** Clean code for MiniGUI 1.1.x by Wei Yongming, 20001/11/28
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/vt.h>

#include "common.h"
#include "minigui.h"

#ifndef _MGRM_THREADS

#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "ial.h"
#include "native.h"

static int vtswitch_initialised = 0;
static struct vt_mode startup_vtmode;

volatile static int console_active = 1;            /* are we active? */
volatile static int console_should_be_active = 1;  /* should we be? */

static int ttyfd = -1;
static int current_vt;

static void vt_switch_requested(int signo);
static void poll_console_switch (void);
static void come_back(void);
static void go_away(void);

int init_vtswitch (int kbd_fd) 
{
    struct sigaction sa;
    struct vt_mode vtm;
    struct vt_stat stat;

    if (vtswitch_initialised)
        return 0;  /* shouldn't happen */

    if (kbd_fd >= 0)
        ttyfd = kbd_fd;
    else {
        ttyfd = open("/dev/tty0", O_RDONLY);
        if (ttyfd == -1) {
            fprintf (stderr, "Error can't open tty: %m.\n");
            return -1;
        }
    }

    ioctl (ttyfd, VT_GETSTATE, &stat);
    current_vt = stat.v_active;
    console_active = console_should_be_active = 1;

    /* Hook the signals */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGIO); /* block async IO during the VT switch */
    sa.sa_flags = 0;
    sa.sa_handler = vt_switch_requested;
    if ((sigaction(SIGRELVT, &sa, NULL) < 0) || (sigaction(SIGACQVT, &sa, NULL) < 0)) {
        fprintf (stderr, "init_vtswitch: Unable to control VT switch.\n");
        return -1;
    }

    /* Save old mode, take control, and arrange for the signals
     * to be raised. */
    ioctl(ttyfd, VT_GETMODE, &startup_vtmode);

    vtm = startup_vtmode;

    vtm.mode = VT_PROCESS;
    vtm.relsig = SIGRELVT;
    vtm.acqsig = SIGACQVT;

    ioctl(ttyfd, VT_SETMODE, &vtm);

    vtswitch_initialised = 1;

    return 0;
}

/* vt_switch_requested:
 *  This is our signal handler; it gets called whenever a switch is
 *  requested, because either SIGRELVT or SIGACQVT is raised.
 */
static void vt_switch_requested(int signo)
{
    switch (signo) {
        case SIGRELVT:
            console_should_be_active = 0;
            break;
        case SIGACQVT:
            console_should_be_active = 1;
            break;
        default:
            return;
    }
    poll_console_switch();
}

/* poll_console_switch:
 *  Checks whether a switch is needed and not blocked; if so,
 *  makes the switch.
 */
static void poll_console_switch (void)
{
    if (console_active == console_should_be_active) return;

    if (console_should_be_active)
        come_back();
    else
        go_away();
}

/* come_back:
 *  Performs a switch back.
 */
static void come_back (void)
{
    int new_fd;

    IAL_ResumeMouse();
    if ((new_fd = IAL_ResumeKeyboard()) >= 0)
        ttyfd = new_fd;

    ioctl (ttyfd, VT_RELDISP, VT_ACKACQ);

    console_active = 1;
    __mg_switch_away = FALSE;

#ifdef _MGRM_PROCESSES
    UpdateTopmostLayer (NULL);
#else
    SendNotifyMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
#endif
}

/* go_away:
 *  Performs a switch away.
 */
static void go_away (void)
{
#ifdef _MGRM_PROCESSES
    DisableClientsOutput ();
#endif

    if (ioctl (ttyfd, VT_RELDISP, 1) == -1) {
        fprintf (stderr, "Error can't switch away from VT: %m\n");
        return;
    }

    console_active = 0;
    __mg_switch_away = TRUE;
    IAL_SuspendKeyboard();
    IAL_SuspendMouse();
}

/*  done_vtswitch:
 *  Undoes the effect of `init_vtswitch'.
 */
int done_vtswitch (int kbd_fd)
{
    struct sigaction sa;

    if (!vtswitch_initialised) return 0;  /* shouldn't really happen either */

    /* !trout gfoot.  Must turn off the signals before unhooking them... */
    ioctl (ttyfd, VT_SETMODE, &startup_vtmode);

    sigemptyset (&sa.sa_mask);
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;
    sigaction (SIGRELVT, &sa, NULL);
    sigaction (SIGACQVT, &sa, NULL);

    vtswitch_initialised = 0;

    if (ttyfd != kbd_fd)
        close (ttyfd);

    return 0;
}

int vtswitch_try(int vt) 
{
    if (!vtswitch_initialised)
        return 1;

    if(vt == current_vt) 
        return 1;    
    ioctl(ttyfd, VT_ACTIVATE, vt);
    return 0;
}

#endif /* !_MGRM_THREADS */

