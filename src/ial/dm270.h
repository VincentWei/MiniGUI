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
** dm270.h:. the head file of Low Level Input Engine for dm270
**
** Created by Yao Yunyuan, 2004/07/23
*/

#ifndef GUI_IAL_DM270_H
    #define GUI_IAL_DM270_H

#define KEY_RELEASED    0x80 

#define KEYPAD_UP       0x01
#define KEYPAD_DOWN     0x02
#define KEYPAD_LEFT     0x04
#define KEYPAD_RIGHT    0x08
#define KEYPAD_OK       0x10
#define KEYPAD_CANCEL   0x20

#define MAX_KEYPAD_CODE     0x21

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDM270Input (INPUT* input, const char* mdev, const char* mtype);
void    TermDM270Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_IAL_DM270_H */
