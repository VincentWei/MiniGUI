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
** pcxvfb.c: Input Engine for PCX Virtual FrameBuffer
** 
** Created by xgwang, 2007/09/28
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#ifdef _MGGAL_PCXVFB

#ifdef __ECOS__
/* for eCos Linux Synthetic target */
#include <cyg/hal/hal_io.h>
#else

#ifndef WIN32

#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "mgsock.h"

#include "misc.h"
#define __NONEED_FD_SET_TYPE
#include "ial.h"
#include "pcxvfb.h"
#endif

#include "license.h"

#include "auto-protocol.h"

#ifdef __ECOS__

#define O_RDONLY CYG_HAL_SYS_O_RDONLY
#undef FD_SET
#define FD_SET CYG_HAL_SYS_FD_SET
#undef FD_ZERO
#define FD_ZERO CYG_HAL_SYS_FD_ZERO
#undef FD_CLR
#define FD_CLR CYG_HAL_SYS_FD_CLR
#undef FD_ISSET
#define FD_ISSET CYG_HAL_SYS_FD_ISSET
#define fd_set struct cyg_hal_sys_fd_set
#define read cyg_hal_sys_read
#define open(name, flags) cyg_hal_sys_open (name, flags, 0)
#define close cyg_hal_sys_close
#define select cyg_hal_sys__newselect
#define timeval cyg_hal_sys_timeval
#define tv_sec hal_tv_sec
#define tv_usec hal_tv_usec
#endif

#define NOBUTTON        0x0000
#define LEFTBUTTON      0x0001
#define RIGHTBUTTON     0x0002
#define MIDBUTTON       0x0004
#define MOUSEBUTTONMASK 0x00FF

#define SHIFTBUTTON     0x0100
#define CONTROLBUTTON   0x0200
#define ALTBUTTON       0x0400
#define METABUTTON      0x0800
#define KEYBUTTONMASK   0x0FFF
#define KEYPAD          0x4000

#ifdef WIN32
extern int win_PCXVFbCloseSocket (int sockfd);
#endif

static XVFBEVENT s_pcxvfb_event_mouse;
static XVFBEVENT s_pcxvfb_event_key;
static int cur_retvalue;
static unsigned char read_mouse_result = 0;

static unsigned char kbd_state [NR_KEYS];

/* for ial record */
#define IAL_RECORD_MANUAL_START

static int record_fd = -1;
static long long int record_time_start = 0;

#ifdef IAL_RECORD_MANUAL_START
#define IAL_RECORD_START_KEY	SCANCODE_F5

static BOOL record_started = FALSE;
#endif

static void (*set_ime_text)(void*,const char*);
static void *ime_text_callback_data;

static void (*exit_gui)(void);

void VFBSetIMETextCallback(void (*setIMEText)(void *, const char* text), void *user_data)
{
	set_ime_text = setIMEText;
	ime_text_callback_data = user_data;
	return;
}

void VFBSetCaption(const char* caption)
{
	int len = 0;
	char szCaption[1024];
	XVFBCaptionEventData *ced = (XVFBCaptionEventData*)szCaption;

	if(caption == NULL || (len = strlen(caption)) <= 0)
		return;

	ced->event_type = CAPTION_TYPE;
	ced->size = len;
	strcpy(ced->buff,caption);

	if(__mg_pcxvfb_client_sockfd == -1)
		return;
	/*one XVFBCaptionEventData struct costs 8 bytes,
	 * change that '8' below if the struct is changed*/
	if (mg_writesocket(__mg_pcxvfb_client_sockfd, ced, 8+len) < (8+len)) {
        _ERR_PRINTF ("IAL>PCXVFB: error occurred when calling VFBSetCaption.\n");
    }
}

void VFBOpenIME(int bOpen)
{
	XVFBIMEEventData ime={
			IME_TYPE,
			bOpen
	};

	if(__mg_pcxvfb_client_sockfd == -1)
		return;

	if (mg_writesocket(__mg_pcxvfb_client_sockfd, &ime, sizeof(ime)) < sizeof(ime)) {
        _ERR_PRINTF ("IAL>PCXVFB: error occurred when calling VFBOpenIME.\n");
    }
}

void VFBShowWindow(int show)
{
	int info[2] = {
			SHOW_HIDE_TYPE,
			show
	};
	if(__mg_pcxvfb_client_sockfd == -1)
		return;

	if (mg_writesocket(__mg_pcxvfb_client_sockfd, info, sizeof(info)) < sizeof(info)) {
        _ERR_PRINTF ("IAL>PCXVFB: error occurred when calling VFBShowWindow.\n");
    }
}

void VFBAtExit(void (*callback)(void))
{
    exit_gui = callback;
}

static void default_cb(void *user_data, const char* str)
{
    int len = strlen(str);
    int i=0;

    while(i<len)
    {
        WPARAM ch;
        //UTF8 to unicode
        if(((Uint8)str[i]) <= 0x7F) //ascii code
        {
            ch = (WPARAM)str[i];
            i++;
        }
        else if(((Uint8)str[i]) <= 0xBF) //
        {
            i ++;
            continue;
        }
        else if(((Uint8)str[i]) <= 0xDF) //2 code
        {
            ch = ((Uint8) str[i])|(((Uint8)str[i+1])<<8);
            i += 2;
        }
        else if(((Uint8)str[i]) <= 0xEF) //three code
        {
            ch = ((Uint8)str[i])
                | (((Uint8)str[i+1])<<8)
                | (((Uint8)str[i+2])<<16);
            i += 3;
        }
        else
        {
            i ++;
            continue;
        }

#ifdef _MGRM_PROCESSES
        Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, ch, 0);
#else
        {
            HWND hwnd;
            if ((hwnd = GetActiveWindow()) != HWND_INVALID) {
                PostMessage(hwnd, MSG_CHAR, ch, 0);
            }
        }
#endif
    }

    license_on_input();
}

/********************  Low Level Input Operations ******************/

static BOOL if_record_ial (void)
{
#ifdef WIN32
    /* TODO: */
    return FALSE;
#else
    return (record_fd >= 0);
#endif
}

static long long int getcurtime(void)
{
#ifdef WIN32
    /* TODO: */
    return 0;
#else    
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (long long int)tv.tv_sec * 1000 + (long long int)tv.tv_usec / 1000;
#endif
}

static void open_rec (const char* path)
{
#ifdef WIN32
    /* TODO: */
#else
    record_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
}

static void write_rec (const void* buf, size_t n)
{
#ifdef WIN32
    /* TODO: */
#else
    if (write(record_fd, buf, n) < n) {
        _ERR_PRINTF ("IAL>PCXVFB: error occurred when calling write_rec.\n");
    }
#endif
}

static void flush_rec (void)
{
#ifdef WIN32
    /* TODO: */
#else
    fsync(record_fd);
#endif
}


static void do_record(AUTO_IAL_INPUT_EVENT *event)
{
#define MAX_COUNT 50
#define MAX_TIME 3000
    static int record_count = 0;
    static long long int record_last_time = 0;

    if (! record_started) {
        /* adjuge if recording should be started */
        if (0 == event->type && IAL_RECORD_START_KEY == event->u.key_event.scancode) {
            /* to start recording */
            record_started = TRUE;
        }
        else {
            /* do nothing */
            return;
        }
    }

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    event->timestamp = ArchSwap64(event->timestamp);
    if (event->type == 0)
    {
        event->u.key_event.scancode = ArchSwap32(event->u.key_event.scancode);
        event->u.key_event.type = ArchSwap32(event->u.key_event.type);
    }
    else
    {
        event->u.mouse_event.x = ArchSwap32(event->u.mouse_event.x);
        event->u.mouse_event.y = ArchSwap32(event->u.mouse_event.y);
        event->u.mouse_event.buttons = ArchSwap32(event->u.mouse_event.buttons);
    }
    event->type = ArchSwap32(event->type);
#endif
    

    write_rec (event, sizeof(*event));

    if (record_count++ >= MAX_COUNT || (event->timestamp - record_last_time) >= MAX_TIME)
    {
        record_count = 0;
        record_last_time = event->timestamp;
        flush_rec ();
    }
}

/*
 * Mouse operations -- Event
 */

static void mouse_getxy (int *x, int* y)
{
    *x = s_pcxvfb_event_mouse.data.mouse.x;
    *y = s_pcxvfb_event_mouse.data.mouse.y;
}

static void mouse_setxy (int x, int y)
{
    int mouse_event[3] = {MOUSE_TYPE, x, y};

    if(__mg_pcxvfb_client_sockfd == -1)
		return;

	if (mg_writesocket (__mg_pcxvfb_client_sockfd, &mouse_event, sizeof(XVFBEVENT)) < sizeof(XVFBEVENT)) {
        _ERR_PRINTF ("IAL>PCXVFB: error occurred when setting mouse position.\n");
    }
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (s_pcxvfb_event_mouse.data.mouse.button & LEFTBUTTON)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (s_pcxvfb_event_mouse.data.mouse.button & RIGHTBUTTON)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (s_pcxvfb_event_mouse.data.mouse.button & MIDBUTTON)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;
}

static int mouse_update (void)
{
    cur_retvalue = 0;
#ifndef WIN32
    
    if (if_record_ial ()) {
        AUTO_IAL_INPUT_EVENT event;
        event.timestamp = getcurtime() - record_time_start;
        event.type = 1;
        mouse_getxy(&event.u.mouse_event.x, &event.u.mouse_event.y);
        event.u.mouse_event.buttons = mouse_getbutton();
        do_record(&event);
    }

    return read_mouse_result;
#else
    return 1;
#endif
}


static int keyboard_update (void)
{
    unsigned char scancode;
    static unsigned char last = 0;
    unsigned int unicode; 
    BYTE press;

	cur_retvalue = 0;

    unicode = s_pcxvfb_event_key.data.key.key_code;
    press = (s_pcxvfb_event_key.data.key.key_state) ? 1 : 0;
    
    if (!(GetShiftKeyStatus() & KS_NUMLOCK)) {
        if (unicode == SCANCODE_CURSORUP)
            unicode = SCANCODE_CURSORBLOCKUP;
        else if (unicode == SCANCODE_CURSORDOWN)
            unicode = SCANCODE_CURSORBLOCKDOWN;
        else if (unicode == SCANCODE_CURSORLEFT)
            unicode = SCANCODE_CURSORBLOCKLEFT;
        else if (unicode == SCANCODE_CURSORRIGHT)
            unicode = SCANCODE_CURSORBLOCKRIGHT;
    }

    if (unicode == 0 && !press) {
        kbd_state [last] = 0;

        if (if_record_ial ()) {
            AUTO_IAL_INPUT_EVENT event;
            event.timestamp = getcurtime() - record_time_start;
            event.type = 0;
            event.u.key_event.scancode = last;
            event.u.key_event.type = press ? 1 : 0;
            do_record(&event);
        }

    }
    else {
        scancode = unicode;
        _DBG_PRINTF ("IAL>PCXVFB: get a key state change (%s): scancode = %d\n",
                press?"pressed":"released", scancode);
        kbd_state [scancode] = press;
        if (press)
            last = scancode;

        if (if_record_ial ()) {
            AUTO_IAL_INPUT_EVENT event;
            event.timestamp = getcurtime() - record_time_start;
            event.type = 0;
            event.u.key_event.scancode = scancode;
            event.u.key_event.type = press ? 1 : 0;
            do_record(&event);
        }

    }

    return NR_KEYS;
}

static const char* keyboard_getstate (void)
{
    return (char*)kbd_state;
}

static int read_event(int fd, XVFBEVENT *pcxvfb_event)
{
    int ret;
    int event_type;

    if ((ret = mg_readsocket (fd, &event_type, sizeof(event_type))) <= 0)
    {
#ifdef __NOUNIX__
        if (1) {
#else
        if (errno != EINTR) {
#endif
            ExitGUISafely (-1);
            return -1;

#ifdef __NOUNIX__
        }
#else
        }
#endif

    } else if(ret != sizeof(event_type)) {
        return -1;
    }

    switch (event_type) {
        case MOUSE_TYPE:
            {
                if (mg_readsocket (fd, &pcxvfb_event->data, sizeof(pcxvfb_event->data))
                        != sizeof(pcxvfb_event->data)) {
                    return -1;
                }
                pcxvfb_event->event_type = event_type;

                if (pcxvfb_event->data.mouse.button < 0x08) {
                    read_mouse_result = 1;
                }
                else {
                    read_mouse_result = 0;
                }

                return IAL_MOUSEEVENT;
            } 
        case KB_TYPE:
            {
                if (mg_readsocket (fd, &pcxvfb_event->data, sizeof(pcxvfb_event->data))
                        != sizeof(pcxvfb_event->data)) {
                    return -1;
                }
                pcxvfb_event->event_type = event_type;

                return IAL_KEYEVENT;
            }
        case IME_MESSAGE_TYPE:
            {
				int size;
				unsigned char szBuff[1024];
                if(mg_readsocket (fd, &size, sizeof(size)) != sizeof(size)) {
                    return 0;
                }
                if(size <= 0) {
                    return 0;
                }
                if(mg_readsocket (fd, szBuff, size) == size) {
                    /*
					int i;
					for (i = 0; i < size; ++i)
						fprintf (stderr, "%02x ", szBuff[i]);
					fprintf (stderr, "%d\n", size);
                    */
                    szBuff[size] = '\0';
                    set_ime_text(ime_text_callback_data, (const char *)szBuff);
				} else {
				}
                return 0;
            }
        case -1:
		case XVFB_CLOSE_TYPE:
            {
				if (__mg_pcxvfb_client_sockfd >= 0) {
#ifdef WIN32
                    win_PCXVFbCloseSocket (__mg_pcxvfb_client_sockfd);
#else
                    close (__mg_pcxvfb_client_sockfd);
#endif
                    __mg_pcxvfb_client_sockfd = -1;
                }


                if (exit_gui)
                {
                    exit_gui();
                }
                ExitGUISafely(-1);
                return 0;
            }
        default:
            printf("event_type = %x\n", event_type);
            // assert(0);
            return -1;
    }
    assert(0);
    return -1;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, 
                      fd_set *except, struct timeval *timeout)
{
    int    retvalue = 0;
#ifndef	WIN32
    fd_set rfds;
#else
    _fd_set rfds;
#endif
    int    fd, e;

    if (cur_retvalue > 0)
    {
        if (in) {
            mg_fd_zero (in);
        }
        if (out) {
            mg_fd_zero (out);
        }
        if (except) {
            mg_fd_zero (except);
        }
        return cur_retvalue;
    }

    if (!in) {
        in = &rfds;
        mg_fd_zero (in);
    }

    if (which & IAL_MOUSEEVENT && __mg_pcxvfb_client_sockfd >= 0) {
        fd = __mg_pcxvfb_client_sockfd;
        mg_fd_set (fd, in);
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
        if (fd > maxfd) maxfd = fd;
#endif
    }

    if (which & IAL_KEYEVENT && __mg_pcxvfb_client_sockfd >= 0) {
        fd = __mg_pcxvfb_client_sockfd;
        mg_fd_set (fd, in);
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
        if (fd > maxfd) maxfd = fd;
#endif
    }

    /* FIXME: pass the real set size */
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    e = mg_select (maxfd + 1, in, out, except, timeout) ;
#else
    e = mg_select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        fd = __mg_pcxvfb_client_sockfd;
        /* If data is present on the mouse fd, service it: */
        if (fd >= 0 && mg_fd_isset (fd, in)) {
            static XVFBEVENT event;
            mg_fd_clr (fd, in);
            retvalue = read_event(__mg_pcxvfb_client_sockfd, &event);
            if (retvalue == IAL_MOUSEEVENT) {
                assert (event.event_type == MOUSE_TYPE);
                memcpy(&s_pcxvfb_event_mouse, &event, sizeof(event));
            }else if (retvalue == IAL_KEYEVENT) {
                assert (event.event_type == KB_TYPE);
                memcpy(&s_pcxvfb_event_key, &event, sizeof(event));
            }
            cur_retvalue = retvalue;
        }
    } else if (e < 0) {
        return -1;
    }
    return retvalue;
}

BOOL InitPCXVFBInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setxy;
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

    VFBSetIMETextCallback(default_cb, 0);

    {
        char path[PATH_MAX], *template;
        template = getenv("MG_ENV_RECORD_IAL");
        if (template)
            {
#if 0
                snprintf(path, sizeof(path), "%s%ld", template, time(NULL));
#else
                snprintf(path, sizeof(path), "%s", template);
#endif
                path[sizeof(path)/sizeof(path[0])-1] = 0;

                open_rec (path);
                record_time_start = getcurtime();
            }
    }


    return TRUE;
}

void TermPCXVFBInput (void)
{
#ifdef WIN32 /* win */
    if (__mg_pcxvfb_client_sockfd >= 0) {
        win_PCXVFbCloseSocket (__mg_pcxvfb_client_sockfd);
        __mg_pcxvfb_client_sockfd = -1;
    }
    if (__mg_pcxvfb_server_sockfd >= 0) {
        win_PCXVFbCloseSocket (__mg_pcxvfb_server_sockfd);
        __mg_pcxvfb_server_sockfd = -1;
    }
#else /* non win */
    char   socket_file [50];

    sprintf(socket_file, "/tmp/pcxvfb_socket%d", getpid());
    socket_file[49] = '\0';

    if (__mg_pcxvfb_client_sockfd >= 0) {
        close(__mg_pcxvfb_client_sockfd);
        __mg_pcxvfb_client_sockfd = -1;
    }
    if (__mg_pcxvfb_server_sockfd >= 0) {
        close(__mg_pcxvfb_server_sockfd);
        __mg_pcxvfb_server_sockfd = -1;
    }
    unlink(socket_file);
#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (mgIsServer) {
        remove("/tmp/.pcxvfb_tmp");
    }
#endif
#endif /* WIN32 */
}

#endif /* _MGGAL_PCXVFB*/
