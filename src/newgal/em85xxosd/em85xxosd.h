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
**  $Id: em85xxosd.h 7349 2007-08-16 04:54:21Z xgwang $
**  
**  Copyright (C) 2003 ~ 2007 Feynman Software.
*/


#ifndef _GAL_em85xxosd_h
#define _GAL_em85xxosd_h

#include "sysvideo.h"

#define BYTE        RMuint8
#define LONG        RMint32
#define ULONG       RMuint32
#define ULONGLONG   RMuint64
#define BOOL        RMbool
#define BOOLEAN     RMbool
#define HANDLE      void *
#define UCHAR       RMuint8
#define USHORT      RMuint16
#define PULONG      RMuint32 *
#define PVOID       void *

/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this

/* Private display data */

struct GAL_PrivateVideoData {
    int status;      /* 0=never inited, 1=once inited, 2=inited. */
    RUA_handle handle;
    Uint8* osd_buffer;

    int w, h, pitch;
    void* fb;
    BOOL dirty;
};

#endif /* _GAL_em85xxosd_h */

