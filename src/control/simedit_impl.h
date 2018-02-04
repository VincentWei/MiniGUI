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
** simedit.h: the head file of Simple Edit Control module.
**
** Create date: 1999/8/26
*/

#ifndef GUI_SIMEDIT_IMPL_H_
#define GUI_SIMEDIT_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LEN_SIMEDIT_BUFFER       512

typedef struct tagSIMEDITDATA
{
    DWORD   status;         // status of box

    int     bufferLen;      // length of buffer
    char*   buffer;         // buffer

    int     dataEnd;        // data end position
    int     editPos;        // current edit position
    int     caretOff;       // caret offset in box
    int     startPos;       // start display position
    
    int     charWidth;      // width of a character
    
    char    passwdChar;     // password character
    
    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin
    
    int     hardLimit;      // hard limit

} SIMEDITDATA;
typedef SIMEDITDATA* PSIMEDITDATA;
    
BOOL RegisterSIMEditControl (void);

#ifdef __cplusplus
}
#endif

#endif // GUI_SIMEDIT_IMPL_H_

