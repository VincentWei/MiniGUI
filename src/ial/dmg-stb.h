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
** tf-stb.h:. the head file of Intel DMG GEN10 STB IAL engine.
**
** Created by Wei Yongming, 2003/12/14
*/

#ifndef GUI_IAL_DMGSTB_H
    #define GUI_IAL_DMGSTB_H

#define DMG_IR_BACK         0x0004

#define DMG_IR_UP           0x0010
#define DMG_IR_DOWN         0x0020
#define DMG_IR_LEFT         0x0040
#define DMG_IR_RIGHT        0x0080

#define DMG_IR_PLAY         0x0100
#define DMG_IR_STOP         0x0200

#define DMG_IR_ENTER        0x4000

#define DMG_KB_UP           SCANCODE_CURSORBLOCKUP      /* ,< */
#define DMG_KB_DOWN         SCANCODE_CURSORBLOCKDOWN    /* .> */
#define DMG_KB_LEFT         SCANCODE_CURSORBLOCKLEFT    /* [{ */
#define DMG_KB_RIGHT        SCANCODE_CURSORBLOCKRIGHT   /* ]} */
#define DMG_KB_ENTER        SCANCODE_ENTER              /* Enter */

#define DMG_KB_STOP         SCANCODE_Q              /* 'q' */
#define DMG_KB_PLAY         SCANCODE_P              /* 'p' */
#define DMG_KB_BACK         SCANCODE_R              /* 'r' */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDMGSTBInput (INPUT* input, const char* mdev, const char* mtype);
void    TermDMGSTBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DMGSTB_H */


