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
** Created by ChenLei, 2009/01/16
*/

#ifndef GUI_IAL_NEXUS_H
    #define GUI_IAL_NEXUS_H

/* report key value macro define */
#define RC_UP		0x9034
#define RC_DOWN		0x8035
#define RC_LEFT		0x7036
#define RC_RIGHT	0x6037
#define RC_OK		0xE011

#define RC_0		0x0000
#define RC_1		0xF001
#define RC_2		0xE002
#define RC_3		0xD003
#define RC_4		0xC004
#define RC_5		0xB005
#define RC_6		0xA006
#define RC_7		0x9007
#define RC_8		0x8008
#define RC_9		0x7009

#define RC_CHANNEL_UP			0x500B
#define RC_CHANNEL_DOWN			0x400C
#define RC_SELECT				0xE011
#define RC_EXIT					0xD012
#define RC_VOLUME_UP			0x300D
#define RC_VOLUME_DOWN			0x200E

#define RC_ESCAPE	0xD012	

#define RC_MENU         0x6019
#define RC_GUIDE        0xd030


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitNexusInput (INPUT* input);
void    TermNexusInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_NEXUS_H */
