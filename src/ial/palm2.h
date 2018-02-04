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
** palm2.h: the head file of Low Level Input Engine for OKWAP Palm2. 
**
** Created by Wei YongMing, 2004/01/29
*/

#ifndef _IAL_PALMII_H
    #define _IAL_PALMII_H

/* bit 7 = state bits0-3 = key number */
#define KEY_RELEASED    0x80
#define KEY_NUM	        0x0f
#define MAX_KEY_EVENTS  4

#define H3600_SCANCODE_RECORD   59
#define H3600_SCANCODE_CALENDAR 60
#define H3600_SCANCODE_CONTACTS 61
#define H3600_SCANCODE_Q        62
#define H3600_SCANCODE_START    63
#define H3600_SCANCODE_UP       103
#define H3600_SCANCODE_RIGHT    106
#define H3600_SCANCODE_LEFT     105
#define H3600_SCANCODE_DOWN     108
#define H3600_SCANCODE_ACTION   64
#define H3600_SCANCODE_SUSPEND  65

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitPALMIIInput (INPUT* input, const char* mdev, const char* mtype);
void TermPALMIIInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _IAL_PALMII_H */


