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
** auto.c: Automatic Input Engine
** 
** Created by Wei Yongming, 2004/01/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_AUTO

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "minigui.h"
#include "misc.h"
#include "ial.h"
#include "auto-protocol.h"

static AUTO_IAL_INPUT_EVENT event, *last_event=NULL, *mouse_event=&event, *key_event=&event;
static long long int time_started = 0;
static int fd_script = -1;
static const unsigned char *s_scripts = NULL;
static int s_scripts_len = 0;

static long long int getcurtime(void)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (long long int)tv.tv_sec * 1000 + (long long int)tv.tv_usec / 1000;
}

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    if (last_event && last_event->type == 1)
    {
        mouse_event = last_event;
        last_event = NULL;
        return 1;
    }
    else
    {
        return -1;
    }
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_event->u.mouse_event.x;
    *y = mouse_event->u.mouse_event.y;
}

static int mouse_getbutton (void)
{
    return mouse_event->u.mouse_event.buttons;
}

static unsigned char kbd_state [NR_KEYS];

static int keyboard_update (void)
{
    if (last_event && last_event->type == 0)
    {
        key_event = last_event;
        last_event = NULL;
        kbd_state[key_event->u.key_event.scancode] = key_event->u.key_event.type;
        return key_event->u.key_event.scancode + 1;
    }
    else
    {
        return -1;
    }
}

static const char* keyboard_getstate (void)
{
    return ( const char*) kbd_state;
}

static int get_auto_event(void)
{
    if (s_scripts)
    {
        if (s_scripts_len >= sizeof(event))
        {
            memcpy(&event, s_scripts, sizeof(event));
            s_scripts += sizeof(event);
            s_scripts_len -= sizeof(event);
            goto got;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        int n_read = read (fd_script, &event, sizeof (event));

        if (n_read != sizeof (event)) {
            
            if (errno != EAGAIN) {
                /* reach EOF */
                
                char* loop = getenv ("MG_ENV_IAL_AUTO_REPLAY");

                if (loop) {           /* try to replay */
                    time_started = 0; /* reset time_started */
                    lseek (fd_script, 0, SEEK_SET);
                    
                    n_read = read (fd_script, &event, sizeof(event));
                    if (n_read == sizeof (event) ) {
                        goto got;
                    }
                }
            }

            if (errno != EAGAIN)
            {
                close(fd_script);
                fd_script = -1;
            }
            return -1;
        }
        else
        {
            goto got;
        }
    }
got:
    event.timestamp = ArchSwapLE64(event.timestamp);
    event.type = ArchSwapLE32(event.type);
    if (event.type == 0)
    {
        event.u.key_event.scancode = ArchSwapLE32(event.u.key_event.scancode);
        event.u.key_event.type = ArchSwapLE32(event.u.key_event.type);
    }
    else
    {
        event.u.mouse_event.x = ArchSwapLE32(event.u.mouse_event.x);
        event.u.mouse_event.y = ArchSwapLE32(event.u.mouse_event.y);
        event.u.mouse_event.buttons = ArchSwapLE32(event.u.mouse_event.buttons);
    }

    return 1;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    long long int curtime;
    long long int steptime;
    struct timeval mytimeout = { 0, 0 };

    int e;

    curtime = getcurtime();
    /* get last event */
    if (! last_event && (fd_script >= 0 || s_scripts)) {
        /* get next event */
        if (get_auto_event() > 0) {
            last_event = &event;
            if (time_started == 0) {
                time_started = curtime - last_event->timestamp;
            }
        }
    }

    if (last_event) {
        steptime = last_event->timestamp - (curtime - time_started);

        /* calculate steptime value */
        if (steptime < 0)
            steptime = 0;

        if (timeout == NULL) {
            /* modify timeout value */
            mytimeout.tv_sec = (time_t)(steptime / 1000);
            mytimeout.tv_usec = 
#ifdef HAVE_SYS_TIME_H
                (suseconds_t)
#endif
                ((steptime % 1000) * 1000);

            timeout = &mytimeout;
        } else {
            if (steptime < (timeout->tv_sec * 1000 + timeout->tv_usec / 1000)) {
                /* modify timeout value */
                timeout->tv_sec = (time_t)(steptime / 1000);
                timeout->tv_usec = 
#ifdef HAVE_SYS_TIME_H
                    (suseconds_t)
#endif
                    ((steptime % 1000) * 1000);
            }
        } /* end if */

        e = select (maxfd + 1, in, out, except, timeout);
        if (e < 0) {
            /* zero all fd sets */
            if (in) FD_ZERO (in);
            if (out) FD_ZERO (out);
            if (except) FD_ZERO (except);
        }

        curtime = getcurtime();

        if (last_event->timestamp > (curtime - time_started)) {
            return 0;
        } else {
            return (last_event->type == 0) ? IAL_KEYEVENT : IAL_MOUSEEVENT;
        }
    } else {
        e = select (maxfd + 1, in, out, except, timeout);
        if (e < 0) {
            /* zero all fd sets */
            if (in) FD_ZERO (in);
            if (out) FD_ZERO (out);
            if (except) FD_ZERO (except);

            return -1;
        } else {
            return 0;
        }
    }
}

BOOL InitAutoInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    if (! s_scripts)
    {
        char *path;
        path = getenv("MG_ENV_RECORD_IAL");
        if (! path)
        {
            path = "ial_scripts.dat";
        }
        fd_script = open(path, O_RDONLY, 0);
    }

    return TRUE;
}

void TermAutoInput (void)
{
}

void IALSetAutoScripts(const unsigned char *scripts, int scripts_len)
{
    s_scripts = scripts;
    s_scripts_len = scripts_len;
}

#endif /* _MGIAL_AUTO */
