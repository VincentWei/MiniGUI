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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

static BITMAP bmp1 , bmp2;
static BITMAP dst;
static unsigned char small_buf[66*76*4];

pthread_t update_th;
HDC slave_hdc;
static HWND _g_hWnd;
int w, h;

static void* do_update (void* data)
{
    int i = 0;
    int timer = 0;
    int screen_w, screen_h;
    
    screen_w =g_rcDesktop.right - g_rcDesktop.left;
    screen_h =g_rcDesktop.bottom - g_rcDesktop.top;
    
    while (1) {
        usleep (10000);
        if (timer == 20)
            break;
        timer++;
        if (*((int*)data) == 1){
            mlsSetSlaveScreenInfo(slave_hdc, MLS_INFOMASK_BLEND, 0, 0, 
                    MLS_BLENDMODE_ALPHA, 0xff, 256/20*timer, 0);						
            mlsSetSlaveScreenInfo(slave_hdc, MLS_INFOMASK_OFFSET, screen_w/20*timer, screen_h/20*timer, 
                    0, 0, 0, 0);
        }else if (*((int*)data) == 0){
            mlsSetSlaveScreenInfo(slave_hdc, MLS_INFOMASK_BLEND, 0, 0, 
                    MLS_BLENDMODE_ALPHA, 0xff, 256 - 256/20*timer, 0);						
            mlsSetSlaveScreenInfo(slave_hdc, MLS_INFOMASK_OFFSET, screen_w-screen_w/20*timer, 
                            screen_h-screen_h/20*timer, 0, 0, 0, 0);
        }
    }
    mlsEnableSlaveScreen(slave_hdc, FALSE);
    if (*((int*)data) == 1)
        MoveWindow(_g_hWnd, screen_w-60, screen_h-20, 60, 20, FALSE);
    else if (*((int*)data) == 0)
        MoveWindow(_g_hWnd, 0, 0, screen_w, screen_h, FALSE);
    
    return NULL;
}

static int LoadBmpWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;
    char mode_string[20];
    volatile int i;
    int base_coor;
    base_coor = 20;
    i = 0;
    _g_hWnd = hWnd;
    int mini_flags;

    switch (message) {
        case MSG_CREATE:
#if 1
            GetWindowRect (hWnd, &rect);
            w = rect.right - rect.left;
            h = rect.bottom - rect.top;
            
            mini_flags = -1;

            sprintf(&mode_string, "%dx%d-%dbpp", w, h, 16);
            slave_hdc = InitSlaveScreen("mlshadow", &mode_string);
            {
                int ret;
                pthread_attr_t new_attr;
                pthread_attr_init(&new_attr);
#ifndef __LINUX__
                pthread_attr_setstacksize (&new_attr, 1024);
#endif
                pthread_attr_setdetachstate (&new_attr, 2);//PTHREAD_CREATE_DETACHED);
                ret = pthread_create(&update_th, &new_attr, do_update, &mini_flags);
                pthread_attr_destroy(&new_attr);
            }
#endif
            return 0;

        case MSG_PAINT:
			hdc = BeginPaint (hWnd);
            if (LoadBitmapFromFile (hdc, &bmp1, "mlshadow_test.jpg"))
				return -1;
            FillBoxWithBitmap(hdc, 0, 0, 480, 457, &bmp1);
            
            //BitBlt(hdc, 0, 0, w, h, slave_hdc, 0, 0, 0);

            EndPaint (hWnd, hdc);
            return 0;

        case MSG_MAXIMIZE:
            mini_flags = 0;
            mlsEnableSlaveScreen(slave_hdc, TRUE);
            do_update(&mini_flags);
            break;

        case MSG_MINIMIZE:
            mini_flags = 1;
            mlsEnableSlaveScreen(slave_hdc, TRUE);
            do_update(&mini_flags);
            break;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}
#if 0
#ifdef _MGRM_PROCESSES
static pthread_mutex_t mutex;

static int __mg_lock_cli_req(void)
{
	return pthread_mutex_lock (&mutex); 
}

static int __mg_trylock_cli_req(void)
{
	return pthread_mutex_trylock (&mutex); 
}

static void __mg_unlock_cli_req(void)
{
	pthread_mutex_unlock (&mutex);
}
#endif
#endif
int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
#ifdef _MGRM_PROCESSES
#if 0   
	pthread_mutex_init (&mutex, NULL);

	OnLockClientReq = __mg_lock_cli_req;
	OnTrylockClientReq = __mg_trylock_cli_req;
	OnUnlockClientReq = __mg_unlock_cli_req;
#endif
	JoinLayer(NAME_DEF_LAYER , "loadbmp" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Load and display a bitmap";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = LoadBmpWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 430;
    CreateInfo.by = 420;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);
    return 0;
}


