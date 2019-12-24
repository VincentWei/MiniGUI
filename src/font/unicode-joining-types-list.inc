/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** mgbidi-joining-types-list.h:
**
** The list of joining types
**
** Created by WEI Yongming at 2019/03/08
**
** This implementation is based on LGPL'd FriBidi:
**
**  https://github.com/fribidi/fribidi
*/

#ifndef _UNIBIDI_ADD_TYPE
# define _UNIBIDI_ADD_TYPE(x,y)
#endif

_UNIBIDI_ADD_TYPE (U, '|')	/* nUn-joining, e.g. Full Stop */
_UNIBIDI_ADD_TYPE (R, '<')	/* Right-joining, e.g. Arabic Letter Dal */
_UNIBIDI_ADD_TYPE (D, '+')	/* Dual-joining, e.g. Arabic Letter Ain */
_UNIBIDI_ADD_TYPE (C, '-')	/* join-Causing, e.g. Tatweel, ZWJ */
_UNIBIDI_ADD_TYPE (T, '^')	/* Transparent, e.g. Arabic Fatha */
_UNIBIDI_ADD_TYPE (L, '>')	/* Left-joining, i.e. fictional */
_UNIBIDI_ADD_TYPE (G, '~')	/* iGnored, e.g. LRE, RLE, ZWNBSP */
