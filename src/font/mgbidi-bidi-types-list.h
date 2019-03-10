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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/*
** mgbidi-bidi-types-list.h:
**
** The list of BIDI types
**
** Created by WEI Yongming at 2019/03/08
**
** This implementation is based on LGPL'd FriBidi:
**
**  https://github.com/fribidi/fribidi
**
** Authors:
**   Behdad Esfahbod, 2001, 2002, 2004
**   Dov Grobgeld, 1999, 2000, 2017
**
** Copyright (C) 2004 Sharif FarsiWeb, Inc
** Copyright (C) 2001,2002 Behdad Esfahbod
** Copyright (C) 1999,2000,2017 Dov Grobgeld
*/

#ifndef _MGBIDI_ADD_TYPE
# define _MGBIDI_ADD_TYPE(x,y)
#endif
#ifndef _MGBIDI_ADD_ALIAS
# define _MGBIDI_ADD_ALIAS(x1,x2)
#endif

#if !defined(_MGBIDI_PAR_TYPES) || defined(_MGBIDI_ALL_TYPES)

_MGBIDI_ADD_TYPE (LTR, 'L')    /* Left-To-Right letter */
_MGBIDI_ADD_TYPE (RTL, 'R')    /* Right-To-Left letter */
_MGBIDI_ADD_TYPE (AL, 'A')    /* Arabic Letter */
_MGBIDI_ADD_TYPE (EN, '1')    /* European Numeral */
_MGBIDI_ADD_TYPE (AN, '9')    /* Arabic Numeral */
_MGBIDI_ADD_TYPE (ES, 'w')    /* European number Separator */
_MGBIDI_ADD_TYPE (ET, 'w')    /* European number Terminator */
_MGBIDI_ADD_TYPE (CS, 'w')    /* Common Separator */
_MGBIDI_ADD_TYPE (NSM, '`')    /* Non Spacing Mark */
_MGBIDI_ADD_TYPE (BN, 'b')    /* Boundary Neutral */
_MGBIDI_ADD_TYPE (BS, 'B')    /* Block Separator */
_MGBIDI_ADD_TYPE (SS, 'S')    /* Segment Separator */
_MGBIDI_ADD_TYPE (WS, '_')    /* WhiteSpace */
_MGBIDI_ADD_TYPE (ON, 'n')    /* Other Neutral */
_MGBIDI_ADD_TYPE (LRE, '+')    /* Left-to-Right Embedding */
_MGBIDI_ADD_TYPE (RLE, '+')    /* Right-to-Left Embedding */
_MGBIDI_ADD_TYPE (LRO, '+')    /* Left-to-Right Override */
_MGBIDI_ADD_TYPE (RLO, '+')    /* Right-to-Left Override */
_MGBIDI_ADD_TYPE (PDF, '-')    /* Pop Directional Flag */
_MGBIDI_ADD_TYPE (LRI, '+')    /* Left-to-Right Isolate */
_MGBIDI_ADD_TYPE (RLI, '+')    /* Right-to-Left Isolate */
_MGBIDI_ADD_TYPE (FSI, '+')    /* First-Strong Isolate */
_MGBIDI_ADD_TYPE (PDI, '-')    /* Pop Directional Isolate */

#if defined(_MGBIDI_ADD_ALIAS)
_MGBIDI_ADD_ALIAS (L, LTR)
_MGBIDI_ADD_ALIAS (R, RTL)
_MGBIDI_ADD_ALIAS (B, BS)
_MGBIDI_ADD_ALIAS (S, SS)
#endif /* _MGBIDI_ADD_ALIAS */

#if defined(_MGBIDI_SENTINEL_TYPE) || defined(_MGBIDI_ALL_TYPES)
_MGBIDI_ADD_TYPE (SENTINEL, '$')    /* SENTINEL */
#endif /* _MGBIDI_SENTINEL_TYPES || _MGBIDI_ALL_TYPES*/
#endif /* !_MGBIDI_PAR_TYPES || _MGBIDI_ALL_TYPES */

#if defined(_MGBIDI_PAR_TYPES) || defined(_MGBIDI_ALL_TYPES)
# if !defined(_MGBIDI_ALL_TYPES)
_MGBIDI_ADD_TYPE (LTR, 'L')    /* Left-To-Right paragraph */
_MGBIDI_ADD_TYPE (RTL, 'R')    /* Right-To-Left paragraph */
_MGBIDI_ADD_TYPE (ON, 'n')    /* directiOn-Neutral paragraph */
# endif /* !_MGBIDI_ALL_TYPES */
_MGBIDI_ADD_TYPE (WLTR, 'l')    /* Weak Left To Right paragraph */
_MGBIDI_ADD_TYPE (WRTL, 'r')    /* Weak Right To Left paragraph */
#endif /* _MGBIDI_PAR_TYPES || _MGBIDI_ALL_TYPES*/

#if defined(_MGBIDI_ENUM_TYPES)
typedef enum {
# define _MGBIDI_ADD_TYPE _MGBIDI_ENUM_ADD_TYPE
# include "mgbidi-bidi-types-list.h"
# undef _MGBIDI_ADD_TYPE
  _MGBIDI_TYPES_MAX
} _MGBIDI_ENUM_TYPES
#endif /* _MGBIDI_ENUM_TYPES */

