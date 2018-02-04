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
** em86.c: IAL Engine for EM86xx Remote Controller.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"

#ifdef _EM86_IAL
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ial.h"
#include "em86.h"
#include "libmc.h"

extern RECT g_rcScr;
extern BOOL __mg_with_panel;

extern int ActiveApp();
extern int DeActiveApp();
extern void UnLoadApp();
extern int OpenUrl(char *url);

static int mousex, mousey;
static int button;
static int mousestatus = 0;
static int MCIsInit = 0;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy(int *x, int *y)
{
    *x = mousex; *y = mousey;
}

static void mouse_setxy(int x, int y)
{
    mousex = x; mousey = y;
}

static inline void chmousestatus (void)
{
    mousestatus ^= 1;

	if(mousestatus)
    {
		printf("+++++++++++++ MiniGUI:Swtich to MOUSE Mode ++++++++++++++\n");
        ShowCursor(TRUE);
    }
	else
    {
		printf("+++++++++++++ MiniGUI:Back to KEYBOARD Mode ++++++++++++++\n");
        ShowCursor(FALSE);
    }
}

static int mouse_getbutton(void)
{
    return button;
}

static unsigned int cur_key;
static unsigned char state [NR_KEYS];

static struct _ir2key {
    unsigned int ir_key;
    unsigned char pc_scancode;
} ir_pckey_map [] =
{
    {KEY_0,         SCANCODE_0},
    {KEY_1,         SCANCODE_1},
    {KEY_2,         SCANCODE_2},
    {KEY_3,        	SCANCODE_3},
    {KEY_4,         SCANCODE_4},
    {KEY_5,         SCANCODE_5},
    {KEY_6,         SCANCODE_6},
    {KEY_7,        	SCANCODE_7},
    {KEY_8,        	SCANCODE_8},
    {KEY_9,         SCANCODE_9},

    {KEY_UP,        SCANCODE_CURSORBLOCKUP},
    {KEY_DOWN,		SCANCODE_CURSORBLOCKDOWN},
    {KEY_LEFT,     	SCANCODE_CURSORBLOCKLEFT},
    {KEY_RIGHT,     SCANCODE_CURSORBLOCKRIGHT},

	{KEY_PGUP,		SCANCODE_PAGEUP},
    {KEY_PGDN,     	SCANCODE_PAGEDOWN},

    {KEY_VOLINC,    SCANCODE_EQUAL},
    {KEY_VOLDEC,    SCANCODE_MINUS},

	{KEY_CAPS,		SCANCODE_CAPSLOCK},
    {KEY_TAB,       SCANCODE_TAB},
	{KEY_REFRESH,	SCANCODE_F5},
	{KEY_BACK,		SCANCODE_BACKSPACE},

    {KEY_OK,        SCANCODE_ENTER},
	{KEY_CANCEL,	SCANCODE_ESCAPE},

	{KEY_MOUSE,		SCANCODE_F8},

	{KEY_RED,		SCANCODE_F9},
	{KEY_GREEN,		SCANCODE_F10},
	{KEY_YELLOW,	SCANCODE_F11},
	{KEY_BLUE,		SCANCODE_F12}
};

static KeyReg_t RegKeys[] =
{
	{KEY_0,			RC},
	{KEY_1,			RC},
	{KEY_2,			RC},
	{KEY_3,			RC},
	{KEY_4,			RC},
	{KEY_5,			RC},
	{KEY_6,			RC},
	{KEY_7,			RC},
	{KEY_8,			RC},
	{KEY_9,			RC},
	
    {KEY_UP,        RC},
    {KEY_DOWN,		RC},
    {KEY_LEFT,     	RC},
    {KEY_RIGHT,     RC},
	
	{KEY_PGUP,		RC},
    {KEY_PGDN,     	RC},
    
	{KEY_VOLINC,    RC},
    {KEY_VOLDEC,    RC},

	{KEY_CAPS,		RC},
    {KEY_TAB,       RC},
	{KEY_REFRESH,	RC},
	{KEY_BACK,		RC},
    
	{KEY_OK,       	RC},
	{KEY_CANCEL,	RC},
    
	{KEY_RED,		RC},
	{KEY_GREEN,		RC},
	{KEY_YELLOW,	RC},
	{KEY_BLUE,		RC},

	{KEY_MOUSE,		RC},

    {KEY_UP,        PN},
    {KEY_DOWN,      PN},
    {KEY_LEFT,      PN},
    {KEY_RIGHT,     PN},
    {KEY_OK,        PN},
    {KEY_MENU,      PN},
    {KEY_PLAY,      PN},
    {KEY_STOP,      PN}

};

static int 
find_ir_key(unsigned long key)
{
    int i;

    for (i = 0; i < TABLESIZE(ir_pckey_map); i++) {
        if (key == ir_pckey_map[i].ir_key) {
            return ir_pckey_map[i].pc_scancode;
        }
    }

    return -1;
}

static int keyboard_update (void)
{
    int pc_key;
    if (cur_key == 0) {
        memset (state, 0, sizeof(state));
    }
    else {
        if ((pc_key = find_ir_key(cur_key)) >= 0) {
            state [pc_key] = 1;
        }
        else {
            cur_key = 0;
            return 0;
        }
    }

    return NR_KEYS;
}

static const char * 
keyboard_get_state(void)
{
    return (char *)state;
}

static int looprcvMsg(Msg_t *pMsg, struct timeval *timeout)
{
    int ret;
    unsigned long interval;
    static struct timeval startt, curt;

    if (!timeout->tv_sec && !timeout->tv_usec)
        return rcvMcMsgNoWait(pMsg);

    gettimeofday(&startt, NULL);
    interval = 0;

    while (interval < (timeout->tv_sec*1000000 + timeout->tv_usec)) {
        ret = rcvMcMsgNoWait(pMsg);
        if (ret > 0) {
			printf("+++++++++++++++ MiniGUI:recieved Message ++++++++++++++++\n");
            return ret;
		}
		else if (ret < 0)	/* error */
			return ret;

        usleep(10000);
        gettimeofday(&curt, NULL);
        interval =
            (curt.tv_sec - startt.tv_sec)*1000000 + (curt.tv_usec - startt.tv_usec);     
	}

    return 0;	/* time out */
}

#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
	unsigned int ret;
    unsigned int key;
	int e;
	unsigned long usec;
	int msgtype;
	Msg_t msg;
	unsigned char *url;
    int number = 0;
	
	usec = timeout->tv_sec*1000000+timeout->tv_usec;

    if (cur_key || button) {
		usleep(usec);

        if (cur_key) {
            cur_key = 0;
            e = IAL_KEYEVENT;
        }

        if (button) {
            button = 0;
            e |= IAL_MOUSEEVENT;
        }

        return e;
    }

	if (!MCIsInit)
	{
		if (initMsgQueue(ST_BrowserControl) < 0) {
			fprintf(stderr, "Cannot Init MC MsgQueue\n");
			return -1;
		}

        if (requestKeys(__mg_with_panel ? TABLESIZE(RegKeys):(TABLESIZE(RegKeys) - 8), RegKeys) < 0) {
            fprintf(stderr, "Request Keys Error\n");
            return -1;
        }
		
		MCIsInit = 1;
	}

	ret = looprcvMsg(&msg, timeout);
	if (ret > 0) {	/* have msg */
		msgtype = getMsgId(&msg);

	    printf("@@@@@@@@@@@@@@@ MiniGUI:KEY Message msgtype (%d) \n", msgtype);
		switch(msgtype) {
		case MSG_KEY:
			printf("+++++++++++++++ MiniGUI:KEY Message +++++++++++++++++\n");
			key = *(unsigned int*)getMsgContent(&msg);
			break;
		case MSG_CMD_ACTIVATE:
			printf("MiniGUI:Receive the MSG_CMD_ACTIVATE Message =================\n");
			ActiveApp();
			acknowledgeCmd(MSG_APP_STATUS_ACTIVE);
			printf("MiniGUI:have sent the MSG_APP_STATUS_ACTIVE Message =================\n");
			return 0;
		case MSG_CMD_DEACTIVATE:
			printf("MiniGUI:Receive the MSG_CMD_DEACTIVATE Message =================\n");
			DeActiveApp();
			acknowledgeCmd(MSG_APP_STATUS_STANDBY);
			printf("MiniGUI:have sent the MSG_APP_STATUS_STANDBY Message =================\n");
			return 0;

		case MSG_CMD_UNLOAD:
			printf("MiniGUI:Receive the MSG_CMD_UNLOAD Message =================\n");
			acknowledgeCmd(MSG_APP_STATUS_UNLOAD);
			UnLoadApp();
			printf("MiniGUI:have sent the MSG_APP_STATUS_UNLOAD Message =================\n");
			return 0;

        case MSG_CMD_URL:
            url = (unsigned char*)getMsgContent(&msg);
            printf("open url: url = (%s) == \n", url);
            OpenUrl(url);
            return 0;

		default:
			printf("MiniGUI:the Message type not found!&&&&&&&&&&&&&&&\n");
			return 0;
		}
	}
	else if (ret = 0)	/* timeout */
	{
		return 0;
	}
	else	/* error */
	{
		return -1;
	}
    
    if (key == KEY_MOUSE) {
        printf("helloworld change the mouse and keyboard mode -====================================\n");
        chmousestatus();
        return 0;
    }

    if (mousestatus) {
        switch (key) {
#if 1
        case KEY_LEFT:
			printf("+++++++++++++ MiniGUI: LEFT Message ++++++++++++++\n");
            mousex -= MOUSE_MOVE_PIXEL;
            if (mousex < 0)
                mousex = 0;
            return IAL_MOUSEEVENT;
#endif

        case KEY_RIGHT:
			printf("+++++++++++++ MiniGUI: RIGHT Message ++++++++++++++\n");
            mousex += MOUSE_MOVE_PIXEL_F;
            if (mousex > MOUSE_MAX_X)
                mousex = MOUSE_MAX_X;
            return IAL_MOUSEEVENT;

        case KEY_UP:
			printf("+++++++++++++ MiniGUI: UP Message ++++++++++++++\n");
            mousey -= MOUSE_MOVE_PIXEL;
            if (mousey < 0)
                mousey = 0;
            return IAL_MOUSEEVENT;

        case KEY_DOWN:
			printf("+++++++++++++ MiniGUI: DOWN Message ++++++++++++++\n");
            mousey += MOUSE_MOVE_PIXEL_F;
            if (mousey > MOUSE_MAX_Y)
                mousey = MOUSE_MAX_Y;
            return IAL_MOUSEEVENT;

        case KEY_OK:
			printf("+++++++++++++ MiniGUI: OK Message ++++++++++++++\n");
            button = IAL_MOUSE_LEFTBUTTON;
            return IAL_MOUSEEVENT;
        }
    }

printf("&&&&&&&&&&&&&&&&&&&&In (%s) line: (%d) key: (%d)\n", __FILE__, __LINE__, key);
    if (key) {
        cur_key = key;
        return IAL_KEYEVENT;
    }

    return -1;
}

BOOL InitEm86Input (INPUT* input, const char* mdev, const char* mtype)
{

    memset (state, 0, sizeof (state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setxy;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    mouse_setxy((g_rcScr.left+g_rcScr.right)/2,
    (g_rcScr.top+g_rcScr.bottom)/2);

    return TRUE;
}

void TermEm86Input (void)
{
}

#endif /* _EM86_IAL */
