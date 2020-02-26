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
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** linux-tty.c: handle linux tty device, e.g., VT switching.
**
** Derived from native/vt-switch.c by WEI Yongming at 2019-06-14
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"

#ifdef __LINUX__

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ial.h"
#include "linux-tty.h"

#define SIGRELVT        SIGUSR1
#define SIGACQVT        SIGUSR2

static int ttyfd = -1;
static int vtswitch_initialized = 0;
static int old_kd_mode = -1;
static struct vt_mode saved_vtmode;

static int current_vt;
static volatile int console_active = 1;            /* are we active? */
static volatile int console_should_be_active = 1;  /* should we be? */

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

    /* Check whether a switch is needed and not blocked */
    if (console_active == console_should_be_active)
        return;

    if (console_should_be_active) {
        /* Performs a switch back. */
        IAL_ResumeMouse();
        IAL_ResumeKeyboard();
        GAL_ResumeVideo();
        console_active = 1;

        if (ioctl (ttyfd, VT_RELDISP, VT_ACKACQ) == -1) {
            _ERR_PRINTF("Linux>TTY: failed to switch VT back: %m\n");
        }

    }
    else {
        /* Performs a switch away. */
        console_active = 0;
        GAL_SuspendVideo();
        IAL_SuspendKeyboard();
        IAL_SuspendMouse();

        if (ioctl (ttyfd, VT_RELDISP, 1) == -1) {
            _ERR_PRINTF("Linux>TTY: failed to switch away from VT: %m\n");
        }
    }
}

/* Init linux tty module, and returns the tty fd */
int __mg_linux_tty_init(BOOL graf_mode)
{
    const char* tty_dev;
    if (geteuid() == 0)
        tty_dev = "/dev/tty0";
    else    /* not a super user, so try to open the control terminal */
        tty_dev = "/dev/tty";

    /* open tty */
    ttyfd = open (tty_dev, O_RDWR);
    if (ttyfd < 0) {
        _ERR_PRINTF("Linux>TTY: Cannot open %s: %m\n", tty_dev);
        goto fail;
    }

    if (graf_mode) {
        if (ioctl (ttyfd, KDGETMODE, &old_kd_mode) == -1) {
            _ERR_PRINTF("Linux>TTY: Cannot get kd mode: %m\n");
            goto fail;
        }

        /* enter graphics mode */
        if (ioctl (ttyfd, KDSETMODE, KD_GRAPHICS) == -1) {
            _WRN_PRINTF("Failed when setting the terminal to graphics mode: %m.\n");
            _WRN_PRINTF("It might is not a console.\n");
        }
    }

    return ttyfd;

fail:
    if (ttyfd >= 0) {
        close(ttyfd);
        ttyfd = -1;
    }

    return -1;
}

int __mg_linux_tty_enable_vt_switch(void)
{
    struct sigaction sa;
    struct vt_mode vtm;
    struct vt_stat stat;

    if (ttyfd < 0) {
        if (__mg_linux_tty_init(FALSE) < 0)
            goto fail;
    }

    if (ioctl (ttyfd, VT_GETSTATE, &stat) == -1) {
        _ERR_PRINTF("Linux>TTY: failed to get vt state: %m.\n");
        goto fail;
    }

    current_vt = stat.v_active;
    console_active = console_should_be_active = 1;

    /* Hook the signals */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGIO); /* block async IO during the VT switch */
    sa.sa_flags = 0;
    sa.sa_handler = vt_switch_requested;
    if ((sigaction(SIGRELVT, &sa, NULL) < 0) ||
            (sigaction(SIGACQVT, &sa, NULL) < 0)) {
        _ERR_PRINTF("Linux>TTY: Unable to control VT switch.\n");
        goto fail;
    }

    /* Save old mode, take control, and arrange for the signals
     * to be raised. */
    if (ioctl(ttyfd, VT_GETMODE, &saved_vtmode) == -1) {
        _ERR_PRINTF("Linux>TTY: failed to get vt mode: %m.\n");
        goto fail;
    }

    vtm = saved_vtmode;
    vtm.mode = VT_PROCESS;
    vtm.relsig = SIGRELVT;
    vtm.acqsig = SIGACQVT;

    if (ioctl(ttyfd, VT_SETMODE, &vtm) == -1) {
        _ERR_PRINTF("Linux>TTY: failed to set vt mode: %m.\n");
        goto fail;
    }

    vtswitch_initialized = 1;
    return 0;

fail:
    return -1;
}

int __mg_linux_tty_disable_vt_switch(void)
{
    struct sigaction sa;

    if (ttyfd < 0)
        return -1;

    /* Must turn off the signals before unhooking them... */
    ioctl (ttyfd, VT_SETMODE, &saved_vtmode);

    sigemptyset (&sa.sa_mask);
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;
    sigaction (SIGRELVT, &sa, NULL);
    sigaction (SIGACQVT, &sa, NULL);

    vtswitch_initialized = 0;
    return 0;
}

int __mg_linux_tty_fini(void)
{
    if (ttyfd < 0)
        return -1;

    if (vtswitch_initialized) {
        __mg_linux_tty_disable_vt_switch();
    }

    if (old_kd_mode >= 0) {
        ioctl (ttyfd, KDSETMODE, old_kd_mode);
    }

    close (ttyfd);
    ttyfd = -1;
    return 0;
}

int __mg_linux_tty_switch_vt(int vt)
{
    if (ttyfd < 0 || !vtswitch_initialized || vt == current_vt)
        return -1;

    if (ioctl (ttyfd, VT_ACTIVATE, vt) == -1) {
        _ERR_PRINTF("Linux>TTY: failed to activiate VT (%d): %m\n", vt);
        return -1;
    }

    return 0;
}

#endif /* __LINUX__ */

