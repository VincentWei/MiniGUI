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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <minigui/mgconfig.h>
#include <minigui/common.h>
#include <minigui/customial.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

typedef struct _input_info_
{
    Uint32 flag;
    Uint32 mx;
    Uint32 my;
    Uint32 MouseKey;
    Uint32 KeyInfo;
}INPUTINFO;
INPUTINFO InputInfo;

struct sockaddr_in source_addr;
int sock = -1;
socklen_t addr_len = sizeof(source_addr);

Uint32 ratio = 2;
int RealX = 0, RealY = 0, RealMouseKey = 0, RealKeyInfo = 0;
unsigned char state [NR_KEYS];

static int mouse_update(void)
{
    RealMouseKey = InputInfo.MouseKey;

    ratio = (InputInfo.flag>>8) & 0x00ff;
    RealX = ratio*InputInfo.mx;
    RealY = ratio*InputInfo.my;
    /*
    printf("RealX:%d RealY:%d ratio:%d\n", RealX, RealY, ratio);
    printf("    x:%d     y:%d\n", InputInfo.mx, InputInfo.my);
    */

    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = RealX;
    *y = RealY;
}

static int mouse_getbutton(void)
{
    return RealMouseKey;
}

static int keyboard_update (void)
{
    int keynum;

    RealKeyInfo = InputInfo.KeyInfo;
    keynum = (unsigned char)RealKeyInfo & 0x7f;
    state[keynum] = ((unsigned char)(RealKeyInfo) & 0x80) ? 1:0;

    return NR_KEYS;
}

static const char * keyboard_getstate(void)
{
    return (char *)state;
}

static int ReplayClientResolution(void)
{
    Uint32 Resolution[2];
    ssize_t n;

    /*
    Resolution[0] = g_rcScr.right;
    Resolution[1] = g_rcScr.bottom;
    */
    Resolution[0] = GetGDCapability(HDC_SCREEN, GDCAP_MAXX)+1;
    Resolution[1] = GetGDCapability(HDC_SCREEN, GDCAP_MAXY)+1;

    n = sendto(\
            sock, (void*)Resolution, sizeof(Resolution), 0, \
            (struct sockaddr *)&source_addr, \
            addr_len);

    if (n != sizeof(Resolution))
    {
        printf("Send resolution to client\n");
    }

    return 0;
}
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int e,n;
    int ret = 0;

    if (!in) 
    {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && sock >= 0) {
        FD_SET (sock, in);
#ifndef _MGRM_THREADS_
        if (sock > maxfd) maxfd = sock;
#endif
    }
#ifndef _MGRM_THREADS_
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) 
	{ 
		if (sock >= 0 && FD_ISSET (sock, in)) 
		{
            FD_CLR (sock, in);
            memset(&InputInfo, 0, sizeof(INPUTINFO));
            n = recvfrom(sock, &InputInfo, sizeof(INPUTINFO), 0,
                    (struct sockaddr *) &source_addr, &addr_len);
        
            /*
            printf("server:flag:%d mx:%d my:%d MouseKey:%d KeyInfo:%d \n",
                    InputInfo.flag, InputInfo.mx, InputInfo.my, InputInfo.MouseKey, InputInfo.KeyInfo);
                    */

            if (InputInfo.flag & IAL_MOUSEEVENT)
            {
                ret |= IAL_MOUSEEVENT;
            }

            if(InputInfo.flag & IAL_KEYEVENT)
            {
                ret |= IAL_KEYEVENT;
            }

            if ((unsigned char)InputInfo.flag == 0xff)
            {
                ReplayClientResolution();
                ret =0 ;
            }
		} 
	}
	else if (e < 0) 
	{
		return -1;
	}

	return ret;
}

#if USEDLCUSTOM
BOOL InitDLCustomInput (INPUT* input, const char* mdev, const char* mtype)
#else
BOOL InitCustomInput(INPUT* input, const char* mdev, const char* mtype)
#endif
{
    struct sockaddr_in s_addr;
    int nPort = 10086;
    char *env = getenv("MG_NETIAL_PORT");
    if(env)
    {
        nPort = atoi(env);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("socket");
        exit(errno);
    } 
    else
    {
        printf("create socket.\n");
    }

    memset(&s_addr, 0, sizeof(struct sockaddr_in));

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(nPort);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(sock, (struct sockaddr *) &s_addr, sizeof(s_addr))) == -1) 
    {
        perror("bind");
        exit(errno);
    } 
    else
    {
        printf("bind address to socket.\n");
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    
	input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;

    input->wait_event = wait_event;
    printf("hello, this is netial \n");
    
    return TRUE;
}

#if USEDLCUSTOM
void TermDLCustomInput (void)
#else
void TermCustomInput(void)
#endif
{
    if (sock>= 0)
        close(sock);    
}


