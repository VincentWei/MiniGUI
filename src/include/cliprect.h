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
** cliprect.h: the head file of Clip Rect module.
** 
** Create date: 1999/3/26
*/

#ifndef GUI_CLIPRECT_H
    #define GUI_CLIPRECT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct tagGCRINFO
{
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
    CLIPRGN         crgn;
    unsigned int    age;
    unsigned int    old_zi_age;
} GCRINFO;
typedef GCRINFO* PGCRINFO;

typedef struct tagINVRGN
{
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
    CLIPRGN         rgn;
    int             frozen;
} INVRGN;
typedef INVRGN* PINVRGN;

/* Function definitions */

#define MAKE_REGION_INFINITE(rgn) do { \
        RECT rc = {INT_MIN/2,INT_MIN/2,INT_MAX/2,INT_MAX/2}; \
        SetClipRgn(rgn, &rc); \
    } while (0)

#undef _REGION_DEBUG

#ifdef _REGION_DEBUG
void dbg_dumpRegion (CLIPRGN* region);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CLIPRECT_H */
