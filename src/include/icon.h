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
** icon.h: the head file of Icon Support Lib.
**
** Create date: 1999/01/06
*/

#ifndef GUI_ICON_H
    #define GUI_ICON_H

/* Struct definitions */
typedef struct _ICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
}ICONDIRENTRY;

#define SIZEOF_ICONDIRENTRY 16
typedef struct _ICONDIR {
    WORD cdReserved;
    WORD cdType;    // must be 1.
    WORD cdCount;
}ICONDIR;

typedef struct _ICON {
    Uint32  width;
    Uint32  height;
    Uint32  pitch;
    BYTE*   AndBits;
    BYTE*   XorBits;
}ICON;

typedef ICON* PICON;

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Initialization and termination. */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_ICON_H

