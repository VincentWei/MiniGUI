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
** acclekey.h: the head file of accelkey.c.
**
** NOTE: Originally by Kang Xiaoning.
**
** Create date: 1999/8/28
*/

#ifndef GUI_ACCEL_H
    #define GUI_ACCEL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _ACCELITEM
{
    int                 key;            // lower 8 bit is key c
    DWORD               keymask;
    WPARAM              wParam;         // command id
    LPARAM              lParam;         // lParam to be sent to
    struct _ACCELITEM*  next;           // next item                           
} ACCELITEM;
typedef ACCELITEM* PACCELITEM;

typedef struct _ACCELTABLE
{
    short               category;       // class of data.
    HWND                hwnd;           // owner.
    PACCELITEM          head;           // head of menu item list
} ACCELTABLE;
typedef ACCELTABLE* PACCELTABLE;

#if defined (__NOUNIX__) || defined (__uClinux__)
    #define SIZE_AC_HEAP   1
    #define SIZE_AI_HEAP   4
#else
  #ifndef _MGRM_THREADS
    #define SIZE_AC_HEAP   1
    #define SIZE_AI_HEAP   4
  #else
    #define SIZE_AC_HEAP   64
    #define SIZE_AI_HEAP   512
  #endif
#endif

BOOL mg_InitAccel (void);
void mg_TerminateAccel (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_ACCEL_H

