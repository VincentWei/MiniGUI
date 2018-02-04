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
** kbd_event.c: /dev/input/eventX Keyboard Driver
*/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/kd.h>
#include <linux/input.h>

#include "common.h"
#include "ial.h"
#include "gal.h"
#include "native.h"

static const char* event_dev;

static int  EVENT_Open(const char *);
static void EVENT_Close(void);
static void EVENT_GetModifierInfo(int *modifiers);
static int  EVENT_Read(unsigned char *buf, int *modifiers);
static void EVENT_Suspend(void);
static int  EVENT_Resume(void);

KBDDEVICE kbddev_event = {
    EVENT_Open,
    EVENT_Close,
    EVENT_GetModifierInfo,
    EVENT_Read,
    EVENT_Suspend,
    EVENT_Resume
};

static int kbd_fd;        /* file descriptor for keyboard */

/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows blocking I/O, and put the terminal into
 * character mode.
 */
static int EVENT_Open (const char *device)
{
    char guess_device[PATH_MAX], *s=guess_device;
    int id=0;

    if (!device || !device[0]) { // the device is not specified, guess it
        FILE *fp;
        char tmp[1024];
        fp = fopen("/proc/bus/input/devices", "r");
        while (fgets(tmp, sizeof(tmp), fp)) {
            const char *p;
            if (! strstr(tmp, "Handlers=")) {
                continue;
            }
            if (strstr(tmp, "mouse")) { // a mouse device, not a keyboard
                continue;
            }
            if (! (p = strstr(tmp, "event"))) { // not support event interface
                continue;
            }
            p += strlen("event");
            s += sprintf(s, "/dev/input/event");
            while (*p >= '0' && *p <= '9') {
                id = id * 10 + *p - '0';
                *s++ = *p++;
            }
            *s = 0;

            mknod (guess_device, S_IFCHR | 0x660, makedev (13, 64+id));
#if 0
            {
                char cmd[1024];
                sprintf(cmd, "mknod %s c 13 %d > /dev/null 2>&1", guess_device, 64+id);
                fprintf(stderr, "%s\n", cmd);
                system(cmd);
            }
#endif
            break;
        }
        fclose(fp);

        if (s == guess_device) {
            return -1;
        }
        device = guess_device;
    }

    kbd_fd = open(device, O_RDONLY | O_NOCTTY);
    if (kbd_fd < 0)
        return -1;

    return kbd_fd;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void EVENT_Close(void)
{
    close(kbd_fd);
    kbd_fd = -1;
}

/*
 * Return the possible modifiers for the keyboard.
 */
static  void EVENT_GetModifierInfo(int *modifiers)
{
    *modifiers = 0;            /* no modifiers available */
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the mode keys (ALT, SHIFT, CTRL).  Returns -1 on error, 0 if no data
 * is ready, and 1 if data was read.  This is a non-blocking call.
 */
static int EVENT_Read(unsigned char *buf, int *modifiers)
{
    int    cc;            /* characters read */
    struct input_event buftmp;

    *modifiers = 0;            /* no modifiers yet */

    *buf = 0;

    cc=read(kbd_fd, &buftmp, sizeof(buftmp));
    if (cc < 0) {
        if ((errno != EINTR) && (errno != EAGAIN))
            return 0;
        return -1;
    }

    if(buftmp.type) {
        if(buftmp.value == 0) {
            *buf = 0x80;
        }
        *buf |= (unsigned char)(buftmp.code & 0x007f);
        // printf("keyboard: value=%d,code=%d,*buf=0x%08x\n",buftmp.value, buftmp.code, *buf);
        return 1;
    }else{
        return 0;
    }
}

/* activate_keyboard:
 *  Put keyboard into the mode we want.
 */
static void EVENT_Suspend(void)
{
    EVENT_Close();
}

/* deactivate_keyboard:
 *  Restore the original keyboard settings.
 */
static int EVENT_Resume(void)
{
    kbd_fd = open (event_dev, O_NONBLOCK);
    if (kbd_fd < 0) {
        return -1;
    }else{
        return kbd_fd;
    }
}
