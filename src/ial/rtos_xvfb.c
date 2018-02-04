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
** rtos_xvfb.c: Input Engine for RTOS X Virtual FrameBuffer
** 
** Created by Liu Peng, 2007/09/29
*/

#include "common.h"

#ifdef _MGGAL_RTOSXVFB

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "minigui.h"
#include "misc.h"

#include "ial.h"
#include "rtos_xvfb.h"

#define NOBUTTON        0x0000
#define LEFTBUTTON      0x0001
#define RIGHTBUTTON     0x0002
#define MIDBUTTON       0x0004
#define MOUSEBUTTONMASK 0x00FF

XVFBEVENTBUFFER* __mg_rtos_xvfb_event_buffer;

static POINT mouse_pt;
static int mouse_buttons;

static XVFBKEYDATA kbd_data;
static unsigned char kbd_state [NR_KEYS];


/* Create the buffer for input events.
* Return value: NULL on error, else the pointer to a handle of xvfb event buffer.
*/
void* xVFBCreateEventBuffer (int nr_events)
{
    XVFBEVENTBUFFER* buf = malloc(sizeof(XVFBEVENTBUFFER));

    pthread_mutex_init (&buf->lock, NULL);
    sem_init (&buf->wait, 0, 0);
    buf->events = malloc (sizeof (XVFBEVENT) * nr_events);
    buf->size = nr_events;
    buf->readpos = 0;
    buf->writepos = 0;
    buf->buffer_released = FALSE;

    return buf;
    
}

void xVFBDestroyEventBuffer (void* buf)
{
    if (((XVFBEVENTBUFFER*)buf)->buffer_released == FALSE)
    {
        fprintf(stderr, "event buffer has not been released, can't destroy it \n");
        return;
    }

    free (((XVFBEVENTBUFFER*)buf)->events);
    free (buf);
}

/* Get the mouse data if there is a mouse event. 
* Return values:
*   0: ok;
*   1: the buffer full.
*   2: the buffer has be destroyed (MiniGUI terminated).
*/
int xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event)
{
    XVFBEVENTBUFFER* buf = (XVFBEVENTBUFFER*)xvfb_event_buffer;
    if (buf->buffer_released == TRUE)
        return 2;
    

    pthread_mutex_lock (&buf->lock);


    if ((buf->writepos + 1) % buf->size == buf->readpos) {
        pthread_mutex_unlock(&buf->lock); /* buf is full.*/

#ifdef _DEBUG
        fprintf (stderr, "buf is full.\n");
#endif
        return 1;
    }

    buf->events [buf->writepos] = *event;
    buf->writepos++;
    if (buf->writepos >= buf->size) 
        buf->writepos = 0;

    pthread_mutex_unlock(&buf->lock);
    sem_post (&buf->wait);

#ifdef _DEBUG
    fprintf (stderr, "event is put into buf.event_type =%d, mouse_x =%d, mouse_y =%d, mouse_btn =%d\n",
             event->event_type, event->data.mouse.x, event->data.mouse.y, event->data.mouse.button);
#endif
    
    return 0;
}


/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_pt.x;
    *y = mouse_pt.y;
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (mouse_buttons & LEFTBUTTON)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (mouse_buttons & RIGHTBUTTON)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (mouse_buttons & MIDBUTTON)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;    
}

static int keyboard_update (void)
{
    unsigned char scan_code;
    unsigned char nr_changed_keys = 0;        

    scan_code = kbd_data.key_code;
    kbd_state [scan_code] = kbd_data.key_state ? 1 : 0;

    nr_changed_keys = scan_code + 1;    
    return nr_changed_keys;
}


static const char* keyboard_getstate (void)
{
    return (char*)kbd_state;
}

static XVFBEVENT* get_event_data (XVFBEVENTBUFFER* buf)
{
    XVFBEVENT* event = NULL;
    
    sem_wait (&buf->wait);

    pthread_mutex_lock (&buf->lock);

    if (buf->readpos != buf->writepos) {
        event = buf->events + buf->readpos;
        
        buf->readpos++;
        if (buf->readpos >= buf->size) 
            buf->readpos = 0;

    }

    pthread_mutex_unlock(&buf->lock);

    return event;
    
}


static int wait_event (int which, int maxfd, fd_set *in, 
        fd_set *out, fd_set *except, struct timeval *timeout)
{
    int retvalue = 0;
    XVFBEVENT *event_data;

    event_data = get_event_data (__mg_rtos_xvfb_event_buffer);
    if (event_data == NULL)
        return -1;

#ifdef _DEBUG
    fprintf (stderr, "got event: %d \n", event_data->event_type);
#endif

    if (which & IAL_MOUSEEVENT && event_data->event_type == 0) {
        /* If data is present on the mouse, service it: */
        mouse_pt.x = event_data->data.mouse.x;
        mouse_pt.y = event_data->data.mouse.y;
        mouse_buttons = event_data->data.mouse.button;
        retvalue |= IAL_MOUSEEVENT;
    }

    if (which & IAL_KEYEVENT && event_data->event_type == 1){
        /* If data is present on the keyboard, service it: */
        kbd_data = event_data->data.key;
        retvalue |= IAL_KEYEVENT;
    }

    return retvalue;

}

BOOL InitRTOSXVFBInput (INPUT* input, const char* mdev, const char* mtype)
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

    return TRUE;
}

void TermRTOSXVFBInput (void)
{
    pthread_mutex_destroy(&__mg_rtos_xvfb_event_buffer->lock);
    __mg_rtos_xvfb_event_buffer->buffer_released = TRUE;
}

#endif /* _RTOSXVFB_ENGINE */

