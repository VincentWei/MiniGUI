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
** mgbidi-bidi-types-list.inc:
**
** The list of BIDI types
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
#ifndef _UNIBIDI_ADD_ALIAS
# define _UNIBIDI_ADD_ALIAS(x1,x2)
#endif

#if !defined(_UNIBIDI_PAR_TYPES) || defined(_UNIBIDI_ALL_TYPES)

_UNIBIDI_ADD_TYPE (LTR, 'L')    /* Left-To-Right letter */
_UNIBIDI_ADD_TYPE (RTL, 'R')    /* Right-To-Left letter */
_UNIBIDI_ADD_TYPE (AL, 'A')    /* Arabic Letter */
_UNIBIDI_ADD_TYPE (EN, '1')    /* European Numeral */
_UNIBIDI_ADD_TYPE (AN, '9')    /* Arabic Numeral */
_UNIBIDI_ADD_TYPE (ES, 'w')    /* European number Separator */
_UNIBIDI_ADD_TYPE (ET, 'w')    /* European number Terminator */
_UNIBIDI_ADD_TYPE (CS, 'w')    /* Common Separator */
_UNIBIDI_ADD_TYPE (NSM, '`')    /* Non Spacing Mark */
_UNIBIDI_ADD_TYPE (BN, 'b')    /* Boundary Neutral */
_UNIBIDI_ADD_TYPE (BS, 'B')    /* Block Separator */
_UNIBIDI_ADD_TYPE (SS, 'S')    /* Segment Separator */
_UNIBIDI_ADD_TYPE (WS, '_')    /* WhiteSpace */
_UNIBIDI_ADD_TYPE (ON, 'n')    /* Other Neutral */
_UNIBIDI_ADD_TYPE (LRE, '+')    /* Left-to-Right Embedding */
_UNIBIDI_ADD_TYPE (RLE, '+')    /* Right-to-Left Embedding */
_UNIBIDI_ADD_TYPE (LRO, '+')    /* Left-to-Right Override */
_UNIBIDI_ADD_TYPE (RLO, '+')    /* Right-to-Left Override */
_UNIBIDI_ADD_TYPE (PDF, '-')    /* Pop Directional Flag */
_UNIBIDI_ADD_TYPE (LRI, '+')    /* Left-to-Right Isolate */
_UNIBIDI_ADD_TYPE (RLI, '+')    /* Right-to-Left Isolate */
_UNIBIDI_ADD_TYPE (FSI, '+')    /* First-Strong Isolate */
_UNIBIDI_ADD_TYPE (PDI, '-')    /* Pop Directional Isolate */

#if defined(_UNIBIDI_ADD_ALIAS)
_UNIBIDI_ADD_ALIAS (L, LTR)
_UNIBIDI_ADD_ALIAS (R, RTL)
_UNIBIDI_ADD_ALIAS (B, BS)
_UNIBIDI_ADD_ALIAS (S, SS)
#endif /* _UNIBIDI_ADD_ALIAS */

#if defined(_UNIBIDI_SENTINEL_TYPE) || defined(_UNIBIDI_ALL_TYPES)
_UNIBIDI_ADD_TYPE (SENTINEL, '$')    /* SENTINEL */
#endif /* _UNIBIDI_SENTINEL_TYPES || _UNIBIDI_ALL_TYPES*/
#endif /* !_UNIBIDI_PAR_TYPES || _UNIBIDI_ALL_TYPES */

#if defined(_UNIBIDI_PAR_TYPES) || defined(_UNIBIDI_ALL_TYPES)
# if !defined(_UNIBIDI_ALL_TYPES)
_UNIBIDI_ADD_TYPE (LTR, 'L')    /* Left-To-Right paragraph */
_UNIBIDI_ADD_TYPE (RTL, 'R')    /* Right-To-Left paragraph */
_UNIBIDI_ADD_TYPE (ON, 'n')    /* directiOn-Neutral paragraph */
# endif /* !_UNIBIDI_ALL_TYPES */
_UNIBIDI_ADD_TYPE (WLTR, 'l')    /* Weak Left To Right paragraph */
_UNIBIDI_ADD_TYPE (WRTL, 'r')    /* Weak Right To Left paragraph */
#endif /* _UNIBIDI_PAR_TYPES || _UNIBIDI_ALL_TYPES*/

#if defined(_UNIBIDI_ENUM_TYPES)
typedef enum {
# define _UNIBIDI_ADD_TYPE _UNIBIDI_ENUM_ADD_TYPE
# include "mgbidi-bidi-types-list.h"
# undef _UNIBIDI_ADD_TYPE
  _UNIBIDI_TYPES_MAX
} _UNIBIDI_ENUM_TYPES
#endif /* _UNIBIDI_ENUM_TYPES */

