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
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include "netial_ctrlwin.h"

typedef struct _input_info_
{
    Uint32 flag;
    Uint32 mx;
    Uint32 my;
    Uint32 MouseKey;
    Uint32 KeyInfo;
}INPUTINFO;

Uint32 ratio = 1;
struct sockaddr_in ServerAddr;
int sock;

static int SendData(void *data, int len)
{
    int hadSend= 0;
    int counter = 0,n = 0;

    while(n < len)
    {
        hadSend = sendto(\
                sock, (void*)data + n, len, 0, \
                (struct sockaddr *)&ServerAddr, \
                (int)sizeof(struct sockaddr_in));
        n += hadSend;
        if(counter < 10)
        {
            counter++;
        }
        else
        {
            printf("Network busy? check it!\n");
            break;
        }
    }

    return 0;
}

static int HandleKeyInfo(INPUTINFO *InputInfo, int message, WPARAM  wParam, LPARAM lParam)
{
    int ret = 0;

    /*when 'ctrl' pressed, click 'e' will return 1, then wil exit ctrl page*/
    if ((wParam == SCANCODE_E) && (lParam & KS_LEFTCTRL))
    {
        printf("Exit Ctrl page...\n");
        InputInfo->KeyInfo = SCANCODE_LEFTCONTROL;
        InputInfo->KeyInfo &= 0x7f;

        ret = 1;
        goto _exit_win_;
    }

    InputInfo->KeyInfo = wParam;
    if (message == MSG_KEYDOWN)
    {
        InputInfo->KeyInfo |= 0x80;
    }
    else if (message == MSG_KEYUP)
    {
        InputInfo->KeyInfo &= 0x7f;
    }
 
_exit_win_:
    InputInfo->flag |= IAL_KEYEVENT;
    return ret;
}

static int HandleMouseInfo(INPUTINFO *InputInfo, int message, WPARAM  wParam, LPARAM lParam)
{
    InputInfo->mx = LOWORD (lParam);
    InputInfo->my = HIWORD (lParam);

    switch (message)
    {
        case MSG_LBUTTONUP:
            InputInfo->MouseKey = 0x0;
            break;
        case MSG_LBUTTONDOWN:
            InputInfo->MouseKey = 0x01;
            break;
        case MSG_RBUTTONDOWN:
            InputInfo->MouseKey = 0x02;
            break;
        case MSG_RBUTTONUP:
            InputInfo->MouseKey = 0x03;
            break;
    }

    InputInfo->flag |= (ratio << 8);
    InputInfo->flag |= IAL_MOUSEEVENT;

    return 0;
}

static int InitNetwork(char *pURL, int nPort)
{
    int sock_opt = 1;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(errno);
    }
    else
    {
        printf("create socket success\n");
    }

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(nPort);
    ServerAddr.sin_addr.s_addr = inet_addr(pURL);
    
    return 0;
}

static int UnInitNetwork(void)
{
    if(sock>0)  
    {  
        close(sock);  
    } 
    return 0;
}

static int CloseCtrlWin(HWND hWnd)
{
    KillTimer (hWnd, 100);
    DestroyMainWindow (hWnd);
    UnInitNetwork();

    return 0;
}

char *str="This is netial control area,\nyour mouse don't leave this area!\n";
static int NetIalCtrlWin(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    INPUTINFO InputInfo;
    char SendFlag = 0;
    static HWND hMainST;
    switch(message)
    {
        case MSG_CREATE:
            hMainST = CreateWindow (CTRL_STATIC, 
                    str,
                    WS_VISIBLE,
                    1, 
                    100, 100, 200, 30, hWnd, 0);

            break;
        case MSG_MOUSEMOVE:
        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
        case MSG_LBUTTONUP:
        case MSG_RBUTTONUP:
            HandleMouseInfo(&InputInfo, message, wParam, lParam);
            SendFlag = 1;
            break;
        case MSG_KEYDOWN:
            if (HandleKeyInfo(&InputInfo, message, wParam, lParam) == 1)
            {
                SendData((void*)&InputInfo, sizeof(INPUTINFO));
                CloseCtrlWin(hWnd);
                return 0;
            }
            SendFlag = 1;
            break;
        case MSG_KEYUP:
            HandleKeyInfo(&InputInfo, message, wParam, lParam);
            SendFlag = 1;
            break;
        case MSG_CLOSE:
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            break;
    }

    if(SendFlag)
    {
        SendData((void*)&InputInfo, sizeof(INPUTINFO));
        InputInfo.flag = 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

#define WIDE 400
int GetServerResolution (int *wide, int *height)
{
    ssize_t n;
    Uint32 tmp[3] = {0,0,0};
    int w,h,i;
    int ret = 1;
    INPUTINFO SendReq;

    memset(&SendReq, 0xff, sizeof(INPUTINFO));
    SendData((void *)&SendReq, sizeof(INPUTINFO));
    for (i=0; i<9; i++)
    {
        n = recvfrom(sock, tmp, sizeof(tmp), 0,
                NULL , NULL);
        if (n > 0)
            break;
        usleep(50000);
    }
    //    n = recv (sock, tmp, sizeof(tmp), 0);

    if((n > 0) && \
            (0 < tmp[0] && tmp[0] < 1920) && \
            (0 < tmp[1] && tmp[1] < 1080))
    {
        w = tmp[0]; h = tmp[1];
        printf("Target Resolution: %dx%d\n", w, h);
    }
    else
    {
        *wide = 400; *height = 300;
        printf("Get target resolution error, default it 400x300!\n");
        return 1;
    }

    i = 1;
    while(i < 500)
    {
        if(!(w%i || h%i))
        {
            *wide = w/i;
            *height = h/i;
            ratio = i;
        }

        if (h/i < 400 || w/i < 400)
        {
            ret = 0;
            break;
        }
        i++;
    }
    printf("Create window with %dx%d ratio:%d\n", *wide, *height, ratio);

    return ret;
}

int CreateCtrlWin(char *pURL, int nPort)
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    int right, buttom;

    InitNetwork(pURL, nPort);
    GetServerResolution(&right, &buttom);

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "netial", 0, 0);
#endif
    CreateInfo.dwStyle = WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "CtrlPage";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NetIalCtrlWin;
    CreateInfo.lx = g_rcScr.left+50;
    CreateInfo.ty = g_rcScr.top+50;
    CreateInfo.rx = right;
    CreateInfo.by = buttom;
    /*
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    */
    CreateInfo.iBkColor = COLOR_lightgray;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
     
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}



