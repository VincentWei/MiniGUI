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
** event.h: the head file of low level event handle.
**
** Create date: 1999/01/11
*/

#ifndef GUI_EVENT_H
    #define GUI_EVENT_H

typedef struct _MOUSEEVENT {
    int event;
    int x;
    int y;
    DWORD status;
}MOUSEEVENT;
typedef MOUSEEVENT* PMOUSEEVENT;

// consts for mouse event.
#define ME_MOVED                0x0000
#define ME_LEFTMASK             0x000F
#define ME_LEFTDOWN             0x0001
#define ME_LEFTUP               0x0002
#define ME_LEFTDBLCLICK         0x0003
#define ME_RIGHTMASK            0x00F0
#define ME_RIGHTDOWN            0x0010
#define ME_RIGHTUP              0x0020
#define ME_RIGHTDBLCLICK        0x0030
#define ME_MIDDLEMASK           0x0F00
#define ME_MIDDLEDOWN           0x0100
#define ME_MIDDLEUP             0x0200
#define ME_MIDDLEDBLCLICK       0x0300
#define ME_REPEATED             0xF000

typedef struct _KEYEVENT {
    int event;
    int scancode;
    DWORD status;
}KEYEVENT;
typedef KEYEVENT* PKEYEVENT;

#define KE_KEYMASK              0x000F
#define KE_KEYDOWN              0x0001
#define KE_KEYUP                0x0002
#define KE_KEYLONGPRESS         0x0004
#define KE_KEYALWAYSPRESS       0x0008
#define KE_SYSKEYMASK           0x00F0
#define KE_SYSKEYDOWN           0x0010
#define KE_SYSKEYUP             0x0020

typedef union _LWEVENTDATA {
    MOUSEEVENT me;
    KEYEVENT ke;
}LWEVENTDATA;

typedef struct _LWEVENT
{
    int type;
    int count;
    DWORD status;
    LWEVENTDATA data;
}LWEVENT;
typedef LWEVENT* PLWEVENT;

// Low level event type.
#define LWETYPE_TIMEOUT                 0
#define LWETYPE_KEY                     1
#define LWETYPE_MOUSE                   2

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

// The entrance of low level event handling thread
BOOL mg_InitLWEvent (void);
void mg_TerminateLWEvent (void);

// Low level event support
BOOL kernel_GetLWEvent (int event, PLWEVENT lwe);

// Mouse event parameters.
#define MOUSEPARA                           "mouse"
#define MOUSEPARA_DBLCLICKTIME              "dblclicktime"
#define DEF_MSEC_DBLCLICK                   300

#define EVENTPARA                           "event"
#define EVENTPARA_REPEATUSEC                "repeatusec"
#define EVENTPARA_TIMEOUTUSEC               "timeoutusec"
#define DEF_USEC_TIMEOUT                    300000
#define DEF_REPEAT_TIME                     50000

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_EVENT_H


