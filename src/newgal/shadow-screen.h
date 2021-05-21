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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
** The common helpers for shadow (double buffering) screen.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2020/03/11
*/

#ifndef _GAL_shadow_screen_h
#define _GAL_shadow_screen_h

#define MAGIC_SHADOW_SCREEN_HEADER  0x20100723

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef _THIS
#   define _THIS    GAL_VideoDevice *this
#endif

int shadowScreen_SetCursor (_THIS, GAL_Surface *surface, int hot_x, int hot_y);
int shadowScreen_MoveCursor (_THIS, int x, int y);
void shadowScreen_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
int shadowScreen_BlitToReal (_THIS);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_shadow_screen_h */
