/*
** $Id: pcxvfbial.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** pcxvfb.c: Input Engine for PCX Virtual FrameBuffer
** 
** Copyright (C) 2003 ~ 2007 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming
**
** All rights reserved by Feynman Software.
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

#else
#endif

#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "misc.h"
#define __NONEED_FD_SET_TYPE
#include "ial.h"
#include "pcxvfb.h"
#include "mgsock.h"
#endif

#include "license.h"

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
		return ;
    /*one XVFBCaptionEventData struct costs 8 bytes,
     * change that '8' below if the struct is changed*/
	FD_WRITE(__mg_pcxvfb_client_sockfd, ced, sizeof(int)*2+len);
}

void VFBOpenIME(int bOpen)
{
	XVFBIMEEventData ime={
			IME_TYPE,
			bOpen
	};

	if(__mg_pcxvfb_client_sockfd == -1)
		return ;
	FD_WRITE(__mg_pcxvfb_client_sockfd, &ime, sizeof(ime));
}

void VFBShowWindow(int show)
{
	int info[2] = {
			SHOW_HIDE_TYPE,
			show
	};
	if(__mg_pcxvfb_client_sockfd == -1)
		return ;
	FD_WRITE(__mg_pcxvfb_client_sockfd, info, sizeof(info));
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

/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    cur_retvalue = 0;
#ifndef WIN32
    return read_mouse_result;
#else
    return 1;
#endif
}

static void mouse_getxy (int *x, int* y)
{
    *x = s_pcxvfb_event_mouse.data.mouse.x;
    *y = s_pcxvfb_event_mouse.data.mouse.y;
}

static void mouse_setxy (int x, int y)
{
    int mouse_event[3] = {MOUSE_TYPE, x, y};

    if(__mg_pcxvfb_client_sockfd == -1)
		return ;
	FD_WRITE(__mg_pcxvfb_client_sockfd, &mouse_event, sizeof(XVFBEVENT));
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
    }
    else {
        scancode = unicode;
        //printf ("scancode = %d\n", scancode);
        kbd_state [scancode] = press;
        if (press)
            last = scancode;
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

    if ((ret = FD_READ (fd, &event_type, sizeof(event_type))) <= 0)
    {
#ifdef __NOUNIX__
        if (1) {
#else
        if (errno != EINTR) {
#endif
            ExitGUISafely (-1);
            return -1;
        }
    } else if(ret != sizeof(event_type)) {
        return -1;
    }

    switch (event_type) {
        case MOUSE_TYPE:
            {
                if (FD_READ (fd, &pcxvfb_event->data, sizeof(pcxvfb_event->data))
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
                if (FD_READ (fd, &pcxvfb_event->data, sizeof(pcxvfb_event->data))
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
                if(FD_READ (fd, &size, sizeof(size)) != sizeof(size)) {
                    return 0;
                }
                if(size <= 0) {
                    return 0;
                }
                if(FD_READ (fd, szBuff, size) == size) {
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
