///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** cursor.h: the head file of Cursor Support Lib.
**
** Create date: 1999/01/06
*/

#ifndef GUI_CURSOR_H
    #define GUI_CURSOR_H

#include "constants.h"

/* Struct definitions */
typedef struct _CURSORDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wXhotspot;
    WORD wYhotspot;
    DWORD lBytesInRes;
    DWORD dwImageOffset;
} CURSORDIRENTRY;

typedef struct _CURSORDIR {
    WORD cdReserved;
    WORD cdType;    // must be 2.
    WORD cdCount;
} CURSORDIR;

typedef struct _CURSOR {
    int xhotspot;
    int yhotspot;
#if IS_COMPOSITING_SCHEMA
    /* Since 5.0.0; for compositing schema */
    void* surface;
#else
    int width;
    int height;
    void* AndBits;
    void* XorBits;
#endif
} CURSOR;
typedef CURSOR* PCURSOR;

#define CURSORWIDTH     32
#define CURSORHEIGHT    32
#define MONOSIZE        (CURSORWIDTH*CURSORHEIGHT/8)
#define MONOPITCH       4

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Initialization and termination. */
BOOL mg_InitCursor (void);
void mg_TerminateCursor(void);

#ifdef _MGHAVE_CURSOR

#ifndef _MGRM_THREADS

/* show cursor hidden by client GDI function */
void kernel_ReShowCursor (void);

#else /* _MGRM_THREADS */

Uint8* kernel_GetPixelUnderCursor (int x, int y, gal_pixel* pixel);

#endif /* _MGRM_THREADS */

#endif /* _MGHAVE_CURSOR */

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc);

/* Mouse event helper. */
BOOL kernel_RefreshCursor (int* x, int* y, int* button);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_CURSOR_H */
