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
** kbd_tty.c: /dev/tty TTY Keyboard Driver
*/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "common.h"
#include "ial.h"
#include "gal.h"
#include "native.h"

static const char* tty_dev;

static int  TTY_Open(const char *);
static void TTY_Close(void);
static void TTY_GetModifierInfo(int *modifiers);
static int  TTY_Read(unsigned char *buf, int *modifiers);
static void TTY_Suspend(void);
static int  TTY_Resume(void);

KBDDEVICE kbddev_tty = {
    TTY_Open,
    TTY_Close,
    TTY_GetModifierInfo,
    TTY_Read,
    TTY_Suspend,
    TTY_Resume
};

static int kbd_fd;        /* file descriptor for keyboard */
static    struct termios    startup_termios;        /* original terminal modes */
static    int    startup_kbdmode;
struct termios    work_termios;    /* startup_termios terminal modes */

/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows blocking I/O, and put the terminal into
 * character mode.
 */
static int TTY_Open (const char *unused)
{
    if (geteuid() == 0) /* is a super user, try to open the active console */
        tty_dev = "/dev/tty0";
    else    /* not a super user, so try to open the control terminal */
        tty_dev = "/dev/tty";

    kbd_fd = open(tty_dev, O_RDONLY | O_NOCTTY);
    if (kbd_fd < 0)
        return -1;

    if (tcgetattr(kbd_fd, &startup_termios) < 0)
        goto err;

    work_termios = startup_termios;

    work_termios.c_lflag &= ~(ICANON | ECHO  | ISIG);
    work_termios.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
    work_termios.c_iflag |= IGNBRK;
    work_termios.c_cc[VMIN] = 0;
    work_termios.c_cc[VTIME] = 0;

    if(tcsetattr(kbd_fd, TCSAFLUSH, &work_termios) < 0)
        goto err;

    /* Put the keyboard into MEDIUMRAW mode.  Despite the name, this
     * is really "mostly raw", with the kernel just folding long
     * scancode sequences (e.g. E0 XX) onto single keycodes.
     */
    ioctl (kbd_fd, KDGKBMODE, &startup_kbdmode);
    if (ioctl(kbd_fd, KDSKBMODE, K_MEDIUMRAW) < 0) 
        goto err;
    return kbd_fd;

err:
    close(kbd_fd);
    kbd_fd = 0;
    return -1;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void TTY_Close(void)
{
    ioctl(kbd_fd, KDSKBMODE, startup_kbdmode);
    tcsetattr(kbd_fd, TCSANOW, &startup_termios);
    close(kbd_fd);
    kbd_fd = -1;
}

/*
 * Return the possible modifiers for the keyboard.
 */
static  void TTY_GetModifierInfo(int *modifiers)
{
    *modifiers = 0;            /* no modifiers available */
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the mode keys (ALT, SHIFT, CTRL).  Returns -1 on error, 0 if no data
 * is ready, and 1 if data was read.  This is a non-blocking call.
 */
static int TTY_Read(unsigned char *buf, int *modifiers)
{
    int    cc;            /* characters read */
    unsigned char buf1;

    *modifiers = 0;            /* no modifiers yet */
    cc = read(kbd_fd, &buf1, 1);
    *buf = buf1;
    if (cc > 0) {
        return 1;
    }
    if ((cc < 0) && (errno != EINTR) && (errno != EAGAIN))
        return -1;
    return 0;
}

/* activate_keyboard:
 *  Put keyboard into the mode we want.
 */
static void TTY_Suspend(void)
{
    TTY_Close();
}

/* deactivate_keyboard:
 *  Restore the original keyboard settings.
 */
static int TTY_Resume(void)
{
    kbd_fd = open (tty_dev, O_NONBLOCK);
    if (kbd_fd < 0)
        return -1;

    if(tcsetattr(kbd_fd, TCSANOW, &work_termios) < 0)
        goto err;

    /* Put the keyboard into MEDIUMRAW mode.  Despite the name, this
     * is really "mostly raw", with the kernel just folding long
     * scancode sequences (e.g. E0 XX) onto single keycodes.
     */
    if (ioctl(kbd_fd, KDSKBMODE, K_MEDIUMRAW) < 0) 
        goto err;
    return kbd_fd;

err:
    close(kbd_fd);
    kbd_fd = 0;
    return -1;
}


