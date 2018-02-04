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
** mapunitogb.c: Map UCS-2 to GB2312.
** 
** Create date: 2005/05/18
*/

#include "common.h"

#ifdef _MGCHARSET_GB

#ifdef _MGCHARSET_UNICODE

/* This is a machine-made file, do not modify. */
#include <stdlib.h>

static unsigned short uc16_min = 0x00A4;
static unsigned short uc16_max = 0xFFE5;

static const unsigned char gb_chars_ban0 [] = {
    0xa1, 0xe8, 
};
static const unsigned char gb_chars_ban1 [] = {
    0xa1, 0xec, 0xa1, 0xa7, 
};
static const unsigned char gb_chars_ban2 [] = {
    0xa1, 0xe3, 0xa1, 0xc0, 
};
static const unsigned char gb_chars_ban3 [] = {
    0xa1, 0xc1, 
};
static const unsigned char gb_chars_ban4 [] = {
    0xa8, 0xa4, 0xa8, 0xa2, 
};
static const unsigned char gb_chars_ban5 [] = {
    0xa8, 0xa8, 0xa8, 0xa6, 0xa8, 0xba, 
};
static const unsigned char gb_chars_ban6 [] = {
    0xa8, 0xac, 0xa8, 0xaa, 
};
static const unsigned char gb_chars_ban7 [] = {
    0xa8, 0xb0, 0xa8, 0xae, 
};
static const unsigned char gb_chars_ban8 [] = {
    0xa1, 0xc2, 
};
static const unsigned char gb_chars_ban9 [] = {
    0xa8, 0xb4, 0xa8, 0xb2, 
};
static const unsigned char gb_chars_ban10 [] = {
    0xa8, 0xb9, 
};
static const unsigned char gb_chars_ban11 [] = {
    0xa8, 0xa1, 
};
static const unsigned char gb_chars_ban12 [] = {
    0xa8, 0xa5, 
};
static const unsigned char gb_chars_ban13 [] = {
    0xa8, 0xa7, 
};
static const unsigned char gb_chars_ban14 [] = {
    0xa8, 0xa9, 
};
static const unsigned char gb_chars_ban15 [] = {
    0xa8, 0xad, 
};
static const unsigned char gb_chars_ban16 [] = {
    0xa8, 0xb1, 
};
static const unsigned char gb_chars_ban17 [] = {
    0xa8, 0xa3, 
};
static const unsigned char gb_chars_ban18 [] = {
    0xa8, 0xab, 
};
static const unsigned char gb_chars_ban19 [] = {
    0xa8, 0xaf, 
};
static const unsigned char gb_chars_ban20 [] = {
    0xa8, 0xb3, 
};
static const unsigned char gb_chars_ban21 [] = {
    0xa8, 0xb5, 
};
static const unsigned char gb_chars_ban22 [] = {
    0xa8, 0xb6, 
};
static const unsigned char gb_chars_ban23 [] = {
    0xa8, 0xb7, 
};
static const unsigned char gb_chars_ban24 [] = {
    0xa8, 0xb8, 
};
static const unsigned char gb_chars_ban25 [] = {
    0xa1, 0xa6, 
};
static const unsigned char gb_chars_ban26 [] = {
    0xa1, 0xa5, 
};
static const unsigned char gb_chars_ban27 [] = {
    0xa6, 0xa1, 0xa6, 0xa2, 0xa6, 0xa3, 0xa6, 0xa4, 0xa6, 0xa5, 
    0xa6, 0xa6, 0xa6, 0xa7, 0xa6, 0xa8, 0xa6, 0xa9, 0xa6, 0xaa, 
    0xa6, 0xab, 0xa6, 0xac, 0xa6, 0xad, 0xa6, 0xae, 0xa6, 0xaf, 
    0xa6, 0xb0, 0xa6, 0xb1, 
};
static const unsigned char gb_chars_ban28 [] = {
    0xa6, 0xb2, 0xa6, 0xb3, 0xa6, 0xb4, 0xa6, 0xb5, 0xa6, 0xb6, 
    0xa6, 0xb7, 0xa6, 0xb8, 
};
static const unsigned char gb_chars_ban29 [] = {
    0xa6, 0xc1, 0xa6, 0xc2, 0xa6, 0xc3, 0xa6, 0xc4, 0xa6, 0xc5, 
    0xa6, 0xc6, 0xa6, 0xc7, 0xa6, 0xc8, 0xa6, 0xc9, 0xa6, 0xca, 
    0xa6, 0xcb, 0xa6, 0xcc, 0xa6, 0xcd, 0xa6, 0xce, 0xa6, 0xcf, 
    0xa6, 0xd0, 0xa6, 0xd1, 
};
static const unsigned char gb_chars_ban30 [] = {
    0xa6, 0xd2, 0xa6, 0xd3, 0xa6, 0xd4, 0xa6, 0xd5, 0xa6, 0xd6, 
    0xa6, 0xd7, 0xa6, 0xd8, 
};
static const unsigned char gb_chars_ban31 [] = {
    0xa7, 0xa7, 
};
static const unsigned char gb_chars_ban32 [] = {
    0xa7, 0xa1, 0xa7, 0xa2, 0xa7, 0xa3, 0xa7, 0xa4, 0xa7, 0xa5, 
    0xa7, 0xa6, 0xa7, 0xa8, 0xa7, 0xa9, 0xa7, 0xaa, 0xa7, 0xab, 
    0xa7, 0xac, 0xa7, 0xad, 0xa7, 0xae, 0xa7, 0xaf, 0xa7, 0xb0, 
    0xa7, 0xb1, 0xa7, 0xb2, 0xa7, 0xb3, 0xa7, 0xb4, 0xa7, 0xb5, 
    0xa7, 0xb6, 0xa7, 0xb7, 0xa7, 0xb8, 0xa7, 0xb9, 0xa7, 0xba, 
    0xa7, 0xbb, 0xa7, 0xbc, 0xa7, 0xbd, 0xa7, 0xbe, 0xa7, 0xbf, 
    0xa7, 0xc0, 0xa7, 0xc1, 0xa7, 0xd1, 0xa7, 0xd2, 0xa7, 0xd3, 
    0xa7, 0xd4, 0xa7, 0xd5, 0xa7, 0xd6, 0xa7, 0xd8, 0xa7, 0xd9, 
    0xa7, 0xda, 0xa7, 0xdb, 0xa7, 0xdc, 0xa7, 0xdd, 0xa7, 0xde, 
    0xa7, 0xdf, 0xa7, 0xe0, 0xa7, 0xe1, 0xa7, 0xe2, 0xa7, 0xe3, 
    0xa7, 0xe4, 0xa7, 0xe5, 0xa7, 0xe6, 0xa7, 0xe7, 0xa7, 0xe8, 
    0xa7, 0xe9, 0xa7, 0xea, 0xa7, 0xeb, 0xa7, 0xec, 0xa7, 0xed, 
    0xa7, 0xee, 0xa7, 0xef, 0xa7, 0xf0, 0xa7, 0xf1, 
};
static const unsigned char gb_chars_ban33 [] = {
    0xa7, 0xd7, 
};
static const unsigned char gb_chars_ban34 [] = {
    0xa1, 0xaa, 
};
static const unsigned char gb_chars_ban35 [] = {
    0xa1, 0xae, 0xa1, 0xaf, 
};
static const unsigned char gb_chars_ban36 [] = {
    0xa1, 0xb0, 0xa1, 0xb1, 
};
static const unsigned char gb_chars_ban37 [] = {
    0xa1, 0xad, 
};
static const unsigned char gb_chars_ban38 [] = {
    0xa1, 0xeb, 
};
static const unsigned char gb_chars_ban39 [] = {
    0xa1, 0xe4, 0xa1, 0xe5, 
};
static const unsigned char gb_chars_ban40 [] = {
    0xa1, 0xf9, 
};
static const unsigned char gb_chars_ban41 [] = {
    0xa1, 0xe6, 
};
static const unsigned char gb_chars_ban42 [] = {
    0xa1, 0xed, 
};
static const unsigned char gb_chars_ban43 [] = {
    0xa2, 0xf1, 0xa2, 0xf2, 0xa2, 0xf3, 0xa2, 0xf4, 0xa2, 0xf5, 
    0xa2, 0xf6, 0xa2, 0xf7, 0xa2, 0xf8, 0xa2, 0xf9, 0xa2, 0xfa, 
    0xa2, 0xfb, 0xa2, 0xfc, 
};
static const unsigned char gb_chars_ban44 [] = {
    0xa1, 0xfb, 0xa1, 0xfc, 0xa1, 0xfa, 0xa1, 0xfd, 
};
static const unsigned char gb_chars_ban45 [] = {
    0xa1, 0xca, 
};
static const unsigned char gb_chars_ban46 [] = {
    0xa1, 0xc7, 
};
static const unsigned char gb_chars_ban47 [] = {
    0xa1, 0xc6, 
};
static const unsigned char gb_chars_ban48 [] = {
    0xa1, 0xcc, 
};
static const unsigned char gb_chars_ban49 [] = {
    0xa1, 0xd8, 0xa1, 0xde, 
};
static const unsigned char gb_chars_ban50 [] = {
    0xa1, 0xcf, 
};
static const unsigned char gb_chars_ban51 [] = {
    0xa1, 0xce, 
};
static const unsigned char gb_chars_ban52 [] = {
    0xa1, 0xc4, 0xa1, 0xc5, 0xa1, 0xc9, 0xa1, 0xc8, 0xa1, 0xd2, 
};
static const unsigned char gb_chars_ban53 [] = {
    0xa1, 0xd3, 
};
static const unsigned char gb_chars_ban54 [] = {
    0xa1, 0xe0, 0xa1, 0xdf, 0xa1, 0xc3, 0xa1, 0xcb, 
};
static const unsigned char gb_chars_ban55 [] = {
    0xa1, 0xd7, 
};
static const unsigned char gb_chars_ban56 [] = {
    0xa1, 0xd6, 
};
static const unsigned char gb_chars_ban57 [] = {
    0xa1, 0xd5, 
};
static const unsigned char gb_chars_ban58 [] = {
    0xa1, 0xd9, 0xa1, 0xd4, 
};
static const unsigned char gb_chars_ban59 [] = {
    0xa1, 0xdc, 0xa1, 0xdd, 
};
static const unsigned char gb_chars_ban60 [] = {
    0xa1, 0xda, 0xa1, 0xdb, 
};
static const unsigned char gb_chars_ban61 [] = {
    0xa1, 0xd1, 
};
static const unsigned char gb_chars_ban62 [] = {
    0xa1, 0xcd, 
};
static const unsigned char gb_chars_ban63 [] = {
    0xa1, 0xd0, 
};
static const unsigned char gb_chars_ban64 [] = {
    0xa2, 0xd9, 0xa2, 0xda, 0xa2, 0xdb, 0xa2, 0xdc, 0xa2, 0xdd, 
    0xa2, 0xde, 0xa2, 0xdf, 0xa2, 0xe0, 0xa2, 0xe1, 0xa2, 0xe2, 
};
static const unsigned char gb_chars_ban65 [] = {
    0xa2, 0xc5, 0xa2, 0xc6, 0xa2, 0xc7, 0xa2, 0xc8, 0xa2, 0xc9, 
    0xa2, 0xca, 0xa2, 0xcb, 0xa2, 0xcc, 0xa2, 0xcd, 0xa2, 0xce, 
    0xa2, 0xcf, 0xa2, 0xd0, 0xa2, 0xd1, 0xa2, 0xd2, 0xa2, 0xd3, 
    0xa2, 0xd4, 0xa2, 0xd5, 0xa2, 0xd6, 0xa2, 0xd7, 0xa2, 0xd8, 
    0xa2, 0xb1, 0xa2, 0xb2, 0xa2, 0xb3, 0xa2, 0xb4, 0xa2, 0xb5, 
    0xa2, 0xb6, 0xa2, 0xb7, 0xa2, 0xb8, 0xa2, 0xb9, 0xa2, 0xba, 
    0xa2, 0xbb, 0xa2, 0xbc, 0xa2, 0xbd, 0xa2, 0xbe, 0xa2, 0xbf, 
    0xa2, 0xc0, 0xa2, 0xc1, 0xa2, 0xc2, 0xa2, 0xc3, 0xa2, 0xc4, 
};
static const unsigned char gb_chars_ban66 [] = {
    0xa9, 0xa4, 0xa9, 0xa5, 0xa9, 0xa6, 0xa9, 0xa7, 0xa9, 0xa8, 
    0xa9, 0xa9, 0xa9, 0xaa, 0xa9, 0xab, 0xa9, 0xac, 0xa9, 0xad, 
    0xa9, 0xae, 0xa9, 0xaf, 0xa9, 0xb0, 0xa9, 0xb1, 0xa9, 0xb2, 
    0xa9, 0xb3, 0xa9, 0xb4, 0xa9, 0xb5, 0xa9, 0xb6, 0xa9, 0xb7, 
    0xa9, 0xb8, 0xa9, 0xb9, 0xa9, 0xba, 0xa9, 0xbb, 0xa9, 0xbc, 
    0xa9, 0xbd, 0xa9, 0xbe, 0xa9, 0xbf, 0xa9, 0xc0, 0xa9, 0xc1, 
    0xa9, 0xc2, 0xa9, 0xc3, 0xa9, 0xc4, 0xa9, 0xc5, 0xa9, 0xc6, 
    0xa9, 0xc7, 0xa9, 0xc8, 0xa9, 0xc9, 0xa9, 0xca, 0xa9, 0xcb, 
    0xa9, 0xcc, 0xa9, 0xcd, 0xa9, 0xce, 0xa9, 0xcf, 0xa9, 0xd0, 
    0xa9, 0xd1, 0xa9, 0xd2, 0xa9, 0xd3, 0xa9, 0xd4, 0xa9, 0xd5, 
    0xa9, 0xd6, 0xa9, 0xd7, 0xa9, 0xd8, 0xa9, 0xd9, 0xa9, 0xda, 
    0xa9, 0xdb, 0xa9, 0xdc, 0xa9, 0xdd, 0xa9, 0xde, 0xa9, 0xdf, 
    0xa9, 0xe0, 0xa9, 0xe1, 0xa9, 0xe2, 0xa9, 0xe3, 0xa9, 0xe4, 
    0xa9, 0xe5, 0xa9, 0xe6, 0xa9, 0xe7, 0xa9, 0xe8, 0xa9, 0xe9, 
    0xa9, 0xea, 0xa9, 0xeb, 0xa9, 0xec, 0xa9, 0xed, 0xa9, 0xee, 
    0xa9, 0xef, 
};
static const unsigned char gb_chars_ban67 [] = {
    0xa1, 0xf6, 0xa1, 0xf5, 
};
static const unsigned char gb_chars_ban68 [] = {
    0xa1, 0xf8, 0xa1, 0xf7, 
};
static const unsigned char gb_chars_ban69 [] = {
    0xa1, 0xf4, 0xa1, 0xf3, 
};
static const unsigned char gb_chars_ban70 [] = {
    0xa1, 0xf0, 
};
static const unsigned char gb_chars_ban71 [] = {
    0xa1, 0xf2, 0xa1, 0xf1, 
};
static const unsigned char gb_chars_ban72 [] = {
    0xa1, 0xef, 0xa1, 0xee, 
};
static const unsigned char gb_chars_ban73 [] = {
    0xa1, 0xe2, 
};
static const unsigned char gb_chars_ban74 [] = {
    0xa1, 0xe1, 
};
static const unsigned char gb_chars_ban75 [] = {
    0xd7, 0xfe, 0xa1, 0xa2, 0xa1, 0xa3, 0xa1, 0xa8, 
};
static const unsigned char gb_chars_ban76 [] = {
    0xa1, 0xa9, 
};
static const unsigned char gb_chars_ban77 [] = {
    0xa1, 0xb4, 0xa1, 0xb5, 0xa1, 0xb6, 0xa1, 0xb7, 0xa1, 0xb8, 
    0xa1, 0xb9, 0xa1, 0xba, 0xa1, 0xbb, 0xa1, 0xbe, 0xa1, 0xbf, 
};
static const unsigned char gb_chars_ban78 [] = {
    0xa1, 0xfe, 0xa1, 0xb2, 0xa1, 0xb3, 0xa1, 0xbc, 0xa1, 0xbd, 
};
static const unsigned char gb_chars_ban79 [] = {
    0xa4, 0xa1, 0xa4, 0xa2, 0xa4, 0xa3, 0xa4, 0xa4, 0xa4, 0xa5, 
    0xa4, 0xa6, 0xa4, 0xa7, 0xa4, 0xa8, 0xa4, 0xa9, 0xa4, 0xaa, 
    0xa4, 0xab, 0xa4, 0xac, 0xa4, 0xad, 0xa4, 0xae, 0xa4, 0xaf, 
    0xa4, 0xb0, 0xa4, 0xb1, 0xa4, 0xb2, 0xa4, 0xb3, 0xa4, 0xb4, 
    0xa4, 0xb5, 0xa4, 0xb6, 0xa4, 0xb7, 0xa4, 0xb8, 0xa4, 0xb9, 
    0xa4, 0xba, 0xa4, 0xbb, 0xa4, 0xbc, 0xa4, 0xbd, 0xa4, 0xbe, 
    0xa4, 0xbf, 0xa4, 0xc0, 0xa4, 0xc1, 0xa4, 0xc2, 0xa4, 0xc3, 
    0xa4, 0xc4, 0xa4, 0xc5, 0xa4, 0xc6, 0xa4, 0xc7, 0xa4, 0xc8, 
    0xa4, 0xc9, 0xa4, 0xca, 0xa4, 0xcb, 0xa4, 0xcc, 0xa4, 0xcd, 
    0xa4, 0xce, 0xa4, 0xcf, 0xa4, 0xd0, 0xa4, 0xd1, 0xa4, 0xd2, 
    0xa4, 0xd3, 0xa4, 0xd4, 0xa4, 0xd5, 0xa4, 0xd6, 0xa4, 0xd7, 
    0xa4, 0xd8, 0xa4, 0xd9, 0xa4, 0xda, 0xa4, 0xdb, 0xa4, 0xdc, 
    0xa4, 0xdd, 0xa4, 0xde, 0xa4, 0xdf, 0xa4, 0xe0, 0xa4, 0xe1, 
    0xa4, 0xe2, 0xa4, 0xe3, 0xa4, 0xe4, 0xa4, 0xe5, 0xa4, 0xe6, 
    0xa4, 0xe7, 0xa4, 0xe8, 0xa4, 0xe9, 0xa4, 0xea, 0xa4, 0xeb, 
    0xa4, 0xec, 0xa4, 0xed, 0xa4, 0xee, 0xa4, 0xef, 0xa4, 0xf0, 
    0xa4, 0xf1, 0xa4, 0xf2, 0xa4, 0xf3, 
};
static const unsigned char gb_chars_ban80 [] = {
    0xa5, 0xa1, 0xa5, 0xa2, 0xa5, 0xa3, 0xa5, 0xa4, 0xa5, 0xa5, 
    0xa5, 0xa6, 0xa5, 0xa7, 0xa5, 0xa8, 0xa5, 0xa9, 0xa5, 0xaa, 
    0xa5, 0xab, 0xa5, 0xac, 0xa5, 0xad, 0xa5, 0xae, 0xa5, 0xaf, 
    0xa5, 0xb0, 0xa5, 0xb1, 0xa5, 0xb2, 0xa5, 0xb3, 0xa5, 0xb4, 
    0xa5, 0xb5, 0xa5, 0xb6, 0xa5, 0xb7, 0xa5, 0xb8, 0xa5, 0xb9, 
    0xa5, 0xba, 0xa5, 0xbb, 0xa5, 0xbc, 0xa5, 0xbd, 0xa5, 0xbe, 
    0xa5, 0xbf, 0xa5, 0xc0, 0xa5, 0xc1, 0xa5, 0xc2, 0xa5, 0xc3, 
    0xa5, 0xc4, 0xa5, 0xc5, 0xa5, 0xc6, 0xa5, 0xc7, 0xa5, 0xc8, 
    0xa5, 0xc9, 0xa5, 0xca, 0xa5, 0xcb, 0xa5, 0xcc, 0xa5, 0xcd, 
    0xa5, 0xce, 0xa5, 0xcf, 0xa5, 0xd0, 0xa5, 0xd1, 0xa5, 0xd2, 
    0xa5, 0xd3, 0xa5, 0xd4, 0xa5, 0xd5, 0xa5, 0xd6, 0xa5, 0xd7, 
    0xa5, 0xd8, 0xa5, 0xd9, 0xa5, 0xda, 0xa5, 0xdb, 0xa5, 0xdc, 
    0xa5, 0xdd, 0xa5, 0xde, 0xa5, 0xdf, 0xa5, 0xe0, 0xa5, 0xe1, 
    0xa5, 0xe2, 0xa5, 0xe3, 0xa5, 0xe4, 0xa5, 0xe5, 0xa5, 0xe6, 
    0xa5, 0xe7, 0xa5, 0xe8, 0xa5, 0xe9, 0xa5, 0xea, 0xa5, 0xeb, 
    0xa5, 0xec, 0xa5, 0xed, 0xa5, 0xee, 0xa5, 0xef, 0xa5, 0xf0, 
    0xa5, 0xf1, 0xa5, 0xf2, 0xa5, 0xf3, 0xa5, 0xf4, 0xa5, 0xf5, 
    0xa5, 0xf6, 
};
static const unsigned char gb_chars_ban81 [] = {
    0xa1, 0xa4, 
};
static const unsigned char gb_chars_ban82 [] = {
    0xa8, 0xc5, 0xa8, 0xc6, 0xa8, 0xc7, 0xa8, 0xc8, 0xa8, 0xc9, 
    0xa8, 0xca, 0xa8, 0xcb, 0xa8, 0xcc, 0xa8, 0xcd, 0xa8, 0xce, 
    0xa8, 0xcf, 0xa8, 0xd0, 0xa8, 0xd1, 0xa8, 0xd2, 0xa8, 0xd3, 
    0xa8, 0xd4, 0xa8, 0xd5, 0xa8, 0xd6, 0xa8, 0xd7, 0xa8, 0xd8, 
    0xa8, 0xd9, 0xa8, 0xda, 0xa8, 0xdb, 0xa8, 0xdc, 0xa8, 0xdd, 
    0xa8, 0xde, 0xa8, 0xdf, 0xa8, 0xe0, 0xa8, 0xe1, 0xa8, 0xe2, 
    0xa8, 0xe3, 0xa8, 0xe4, 0xa8, 0xe5, 0xa8, 0xe6, 0xa8, 0xe7, 
    0xa8, 0xe8, 0xa8, 0xe9, 
};
static const unsigned char gb_chars_ban83 [] = {
    0xa2, 0xe5, 0xa2, 0xe6, 0xa2, 0xe7, 0xa2, 0xe8, 0xa2, 0xe9, 
    0xa2, 0xea, 0xa2, 0xeb, 0xa2, 0xec, 0xa2, 0xed, 0xa2, 0xee, 
};
static const unsigned char gb_chars_ban84 [] = {
    0xd2, 0xbb, 0xb6, 0xa1, 
};
static const unsigned char gb_chars_ban85 [] = {
    0xc6, 0xdf, 
};
static const unsigned char gb_chars_ban86 [] = {
    0xcd, 0xf2, 0xd5, 0xc9, 0xc8, 0xfd, 0xc9, 0xcf, 0xcf, 0xc2, 
    0xd8, 0xa2, 0xb2, 0xbb, 0xd3, 0xeb, 
};
static const unsigned char gb_chars_ban87 [] = {
    0xd8, 0xa4, 0xb3, 0xf3, 
};
static const unsigned char gb_chars_ban88 [] = {
    0xd7, 0xa8, 0xc7, 0xd2, 0xd8, 0xa7, 0xca, 0xc0, 
};
static const unsigned char gb_chars_ban89 [] = {
    0xc7, 0xf0, 0xb1, 0xfb, 0xd2, 0xb5, 0xb4, 0xd4, 0xb6, 0xab, 
    0xcb, 0xbf, 0xd8, 0xa9, 
};
static const unsigned char gb_chars_ban90 [] = {
    0xb6, 0xaa, 
};
static const unsigned char gb_chars_ban91 [] = {
    0xc1, 0xbd, 0xd1, 0xcf, 
};
static const unsigned char gb_chars_ban92 [] = {
    0xc9, 0xa5, 0xd8, 0xad, 
};
static const unsigned char gb_chars_ban93 [] = {
    0xb8, 0xf6, 0xd1, 0xbe, 0xe3, 0xdc, 0xd6, 0xd0, 
};
static const unsigned char gb_chars_ban94 [] = {
    0xb7, 0xe1, 
};
static const unsigned char gb_chars_ban95 [] = {
    0xb4, 0xae, 
};
static const unsigned char gb_chars_ban96 [] = {
    0xc1, 0xd9, 
};
static const unsigned char gb_chars_ban97 [] = {
    0xd8, 0xbc, 
};
static const unsigned char gb_chars_ban98 [] = {
    0xcd, 0xe8, 0xb5, 0xa4, 0xce, 0xaa, 0xd6, 0xf7, 
};
static const unsigned char gb_chars_ban99 [] = {
    0xc0, 0xf6, 0xbe, 0xd9, 0xd8, 0xaf, 
};
static const unsigned char gb_chars_ban100 [] = {
    0xc4, 0xcb, 
};
static const unsigned char gb_chars_ban101 [] = {
    0xbe, 0xc3, 
};
static const unsigned char gb_chars_ban102 [] = {
    0xd8, 0xb1, 0xc3, 0xb4, 0xd2, 0xe5, 
};
static const unsigned char gb_chars_ban103 [] = {
    0xd6, 0xae, 0xce, 0xda, 0xd5, 0xa7, 0xba, 0xf5, 0xb7, 0xa6, 
    0xc0, 0xd6, 
};
static const unsigned char gb_chars_ban104 [] = {
    0xc6, 0xb9, 0xc5, 0xd2, 0xc7, 0xc7, 
};
static const unsigned char gb_chars_ban105 [] = {
    0xb9, 0xd4, 
};
static const unsigned char gb_chars_ban106 [] = {
    0xb3, 0xcb, 0xd2, 0xd2, 
};
static const unsigned char gb_chars_ban107 [] = {
    0xd8, 0xbf, 0xbe, 0xc5, 0xc6, 0xf2, 0xd2, 0xb2, 0xcf, 0xb0, 
    0xcf, 0xe7, 
};
static const unsigned char gb_chars_ban108 [] = {
    0xca, 0xe9, 
};
static const unsigned char gb_chars_ban109 [] = {
    0xd8, 0xc0, 
};
static const unsigned char gb_chars_ban110 [] = {
    0xc2, 0xf2, 0xc2, 0xd2, 
};
static const unsigned char gb_chars_ban111 [] = {
    0xc8, 0xe9, 
};
static const unsigned char gb_chars_ban112 [] = {
    0xc7, 0xac, 
};
static const unsigned char gb_chars_ban113 [] = {
    0xc1, 0xcb, 
};
static const unsigned char gb_chars_ban114 [] = {
    0xd3, 0xe8, 0xd5, 0xf9, 
};
static const unsigned char gb_chars_ban115 [] = {
    0xca, 0xc2, 0xb6, 0xfe, 0xd8, 0xa1, 0xd3, 0xda, 0xbf, 0xf7, 
};
static const unsigned char gb_chars_ban116 [] = {
    0xd4, 0xc6, 0xbb, 0xa5, 0xd8, 0xc1, 0xce, 0xe5, 0xbe, 0xae, 
};
static const unsigned char gb_chars_ban117 [] = {
    0xd8, 0xa8, 
};
static const unsigned char gb_chars_ban118 [] = {
    0xd1, 0xc7, 0xd0, 0xa9, 
};
static const unsigned char gb_chars_ban119 [] = {
    0xd8, 0xbd, 0xd9, 0xef, 0xcd, 0xf6, 0xbf, 0xba, 
};
static const unsigned char gb_chars_ban120 [] = {
    0xbd, 0xbb, 0xba, 0xa5, 0xd2, 0xe0, 0xb2, 0xfa, 0xba, 0xe0, 
    0xc4, 0xb6, 
};
static const unsigned char gb_chars_ban121 [] = {
    0xcf, 0xed, 0xbe, 0xa9, 0xcd, 0xa4, 0xc1, 0xc1, 
};
static const unsigned char gb_chars_ban122 [] = {
    0xc7, 0xd7, 0xd9, 0xf1, 
};
static const unsigned char gb_chars_ban123 [] = {
    0xd9, 0xf4, 
};
static const unsigned char gb_chars_ban124 [] = {
    0xc8, 0xcb, 0xd8, 0xe9, 
};
static const unsigned char gb_chars_ban125 [] = {
    0xd2, 0xda, 0xca, 0xb2, 0xc8, 0xca, 0xd8, 0xec, 0xd8, 0xea, 
    0xd8, 0xc6, 0xbd, 0xf6, 0xc6, 0xcd, 0xb3, 0xf0, 
};
static const unsigned char gb_chars_ban126 [] = {
    0xd8, 0xeb, 0xbd, 0xf1, 0xbd, 0xe9, 
};
static const unsigned char gb_chars_ban127 [] = {
    0xc8, 0xd4, 0xb4, 0xd3, 
};
static const unsigned char gb_chars_ban128 [] = {
    0xc2, 0xd8, 
};
static const unsigned char gb_chars_ban129 [] = {
    0xb2, 0xd6, 0xd7, 0xd0, 0xca, 0xcb, 0xcb, 0xfb, 0xd5, 0xcc, 
    0xb8, 0xb6, 0xcf, 0xc9, 
};
static const unsigned char gb_chars_ban130 [] = {
    0xd9, 0xda, 0xd8, 0xf0, 0xc7, 0xaa, 
};
static const unsigned char gb_chars_ban131 [] = {
    0xd8, 0xee, 
};
static const unsigned char gb_chars_ban132 [] = {
    0xb4, 0xfa, 0xc1, 0xee, 0xd2, 0xd4, 
};
static const unsigned char gb_chars_ban133 [] = {
    0xd8, 0xed, 
};
static const unsigned char gb_chars_ban134 [] = {
    0xd2, 0xc7, 0xd8, 0xef, 0xc3, 0xc7, 
};
static const unsigned char gb_chars_ban135 [] = {
    0xd1, 0xf6, 
};
static const unsigned char gb_chars_ban136 [] = {
    0xd6, 0xd9, 0xd8, 0xf2, 
};
static const unsigned char gb_chars_ban137 [] = {
    0xd8, 0xf5, 0xbc, 0xfe, 0xbc, 0xdb, 
};
static const unsigned char gb_chars_ban138 [] = {
    0xc8, 0xce, 
};
static const unsigned char gb_chars_ban139 [] = {
    0xb7, 0xdd, 
};
static const unsigned char gb_chars_ban140 [] = {
    0xb7, 0xc2, 
};
static const unsigned char gb_chars_ban141 [] = {
    0xc6, 0xf3, 
};
static const unsigned char gb_chars_ban142 [] = {
    0xd8, 0xf8, 0xd2, 0xc1, 
};
static const unsigned char gb_chars_ban143 [] = {
    0xce, 0xe9, 0xbc, 0xbf, 0xb7, 0xfc, 0xb7, 0xa5, 0xd0, 0xdd, 
};
static const unsigned char gb_chars_ban144 [] = {
    0xd6, 0xda, 0xd3, 0xc5, 0xbb, 0xef, 0xbb, 0xe1, 0xd8, 0xf1, 
};
static const unsigned char gb_chars_ban145 [] = {
    0xc9, 0xa1, 0xce, 0xb0, 0xb4, 0xab, 
};
static const unsigned char gb_chars_ban146 [] = {
    0xd8, 0xf3, 
};
static const unsigned char gb_chars_ban147 [] = {
    0xc9, 0xcb, 0xd8, 0xf6, 0xc2, 0xd7, 0xd8, 0xf7, 
};
static const unsigned char gb_chars_ban148 [] = {
    0xce, 0xb1, 0xd8, 0xf9, 
};
static const unsigned char gb_chars_ban149 [] = {
    0xb2, 0xae, 0xb9, 0xc0, 
};
static const unsigned char gb_chars_ban150 [] = {
    0xd9, 0xa3, 
};
static const unsigned char gb_chars_ban151 [] = {
    0xb0, 0xe9, 
};
static const unsigned char gb_chars_ban152 [] = {
    0xc1, 0xe6, 
};
static const unsigned char gb_chars_ban153 [] = {
    0xc9, 0xec, 
};
static const unsigned char gb_chars_ban154 [] = {
    0xcb, 0xc5, 
};
static const unsigned char gb_chars_ban155 [] = {
    0xcb, 0xc6, 0xd9, 0xa4, 
};
static const unsigned char gb_chars_ban156 [] = {
    0xb5, 0xe8, 
};
static const unsigned char gb_chars_ban157 [] = {
    0xb5, 0xab, 
};
static const unsigned char gb_chars_ban158 [] = {
    0xce, 0xbb, 0xb5, 0xcd, 0xd7, 0xa1, 0xd7, 0xf4, 0xd3, 0xd3, 
};
static const unsigned char gb_chars_ban159 [] = {
    0xcc, 0xe5, 
};
static const unsigned char gb_chars_ban160 [] = {
    0xba, 0xce, 
};
static const unsigned char gb_chars_ban161 [] = {
    0xd9, 0xa2, 0xd9, 0xdc, 0xd3, 0xe0, 0xd8, 0xfd, 0xb7, 0xf0, 
    0xd7, 0xf7, 0xd8, 0xfe, 0xd8, 0xfa, 0xd9, 0xa1, 0xc4, 0xe3, 
};
static const unsigned char gb_chars_ban162 [] = {
    0xd3, 0xb6, 0xd8, 0xf4, 0xd9, 0xdd, 
};
static const unsigned char gb_chars_ban163 [] = {
    0xd8, 0xfb, 
};
static const unsigned char gb_chars_ban164 [] = {
    0xc5, 0xe5, 
};
static const unsigned char gb_chars_ban165 [] = {
    0xc0, 0xd0, 
};
static const unsigned char gb_chars_ban166 [] = {
    0xd1, 0xf0, 0xb0, 0xdb, 
};
static const unsigned char gb_chars_ban167 [] = {
    0xbc, 0xd1, 0xd9, 0xa6, 
};
static const unsigned char gb_chars_ban168 [] = {
    0xd9, 0xa5, 
};
static const unsigned char gb_chars_ban169 [] = {
    0xd9, 0xac, 0xd9, 0xae, 
};
static const unsigned char gb_chars_ban170 [] = {
    0xd9, 0xab, 0xca, 0xb9, 
};
static const unsigned char gb_chars_ban171 [] = {
    0xd9, 0xa9, 0xd6, 0xb6, 
};
static const unsigned char gb_chars_ban172 [] = {
    0xb3, 0xde, 0xd9, 0xa8, 
};
static const unsigned char gb_chars_ban173 [] = {
    0xc0, 0xfd, 
};
static const unsigned char gb_chars_ban174 [] = {
    0xca, 0xcc, 
};
static const unsigned char gb_chars_ban175 [] = {
    0xd9, 0xaa, 
};
static const unsigned char gb_chars_ban176 [] = {
    0xd9, 0xa7, 
};
static const unsigned char gb_chars_ban177 [] = {
    0xd9, 0xb0, 
};
static const unsigned char gb_chars_ban178 [] = {
    0xb6, 0xb1, 
};
static const unsigned char gb_chars_ban179 [] = {
    0xb9, 0xa9, 
};
static const unsigned char gb_chars_ban180 [] = {
    0xd2, 0xc0, 
};
static const unsigned char gb_chars_ban181 [] = {
    0xcf, 0xc0, 
};
static const unsigned char gb_chars_ban182 [] = {
    0xc2, 0xc2, 
};
static const unsigned char gb_chars_ban183 [] = {
    0xbd, 0xc4, 0xd5, 0xec, 0xb2, 0xe0, 0xc7, 0xc8, 0xbf, 0xeb, 
    0xd9, 0xad, 
};
static const unsigned char gb_chars_ban184 [] = {
    0xd9, 0xaf, 
};
static const unsigned char gb_chars_ban185 [] = {
    0xce, 0xea, 0xba, 0xee, 
};
static const unsigned char gb_chars_ban186 [] = {
    0xc7, 0xd6, 
};
static const unsigned char gb_chars_ban187 [] = {
    0xb1, 0xe3, 
};
static const unsigned char gb_chars_ban188 [] = {
    0xb4, 0xd9, 0xb6, 0xed, 0xd9, 0xb4, 
};
static const unsigned char gb_chars_ban189 [] = {
    0xbf, 0xa1, 
};
static const unsigned char gb_chars_ban190 [] = {
    0xd9, 0xde, 0xc7, 0xce, 0xc0, 0xfe, 0xd9, 0xb8, 
};
static const unsigned char gb_chars_ban191 [] = {
    0xcb, 0xd7, 0xb7, 0xfd, 
};
static const unsigned char gb_chars_ban192 [] = {
    0xd9, 0xb5, 
};
static const unsigned char gb_chars_ban193 [] = {
    0xd9, 0xb7, 0xb1, 0xa3, 0xd3, 0xe1, 0xd9, 0xb9, 
};
static const unsigned char gb_chars_ban194 [] = {
    0xd0, 0xc5, 
};
static const unsigned char gb_chars_ban195 [] = {
    0xd9, 0xb6, 
};
static const unsigned char gb_chars_ban196 [] = {
    0xd9, 0xb1, 
};
static const unsigned char gb_chars_ban197 [] = {
    0xd9, 0xb2, 0xc1, 0xa9, 0xd9, 0xb3, 
};
static const unsigned char gb_chars_ban198 [] = {
    0xbc, 0xf3, 0xd0, 0xde, 0xb8, 0xa9, 
};
static const unsigned char gb_chars_ban199 [] = {
    0xbe, 0xe3, 
};
static const unsigned char gb_chars_ban200 [] = {
    0xd9, 0xbd, 
};
static const unsigned char gb_chars_ban201 [] = {
    0xd9, 0xba, 
};
static const unsigned char gb_chars_ban202 [] = {
    0xb0, 0xb3, 
};
static const unsigned char gb_chars_ban203 [] = {
    0xd9, 0xc2, 
};
static const unsigned char gb_chars_ban204 [] = {
    0xd9, 0xc4, 0xb1, 0xb6, 
};
static const unsigned char gb_chars_ban205 [] = {
    0xd9, 0xbf, 
};
static const unsigned char gb_chars_ban206 [] = {
    0xb5, 0xb9, 
};
static const unsigned char gb_chars_ban207 [] = {
    0xbe, 0xf3, 
};
static const unsigned char gb_chars_ban208 [] = {
    0xcc, 0xc8, 0xba, 0xf2, 0xd2, 0xd0, 
};
static const unsigned char gb_chars_ban209 [] = {
    0xd9, 0xc3, 
};
static const unsigned char gb_chars_ban210 [] = {
    0xbd, 0xe8, 
};
static const unsigned char gb_chars_ban211 [] = {
    0xb3, 0xab, 
};
static const unsigned char gb_chars_ban212 [] = {
    0xd9, 0xc5, 0xbe, 0xeb, 
};
static const unsigned char gb_chars_ban213 [] = {
    0xd9, 0xc6, 0xd9, 0xbb, 0xc4, 0xdf, 
};
static const unsigned char gb_chars_ban214 [] = {
    0xd9, 0xbe, 0xd9, 0xc1, 0xd9, 0xc0, 
};
static const unsigned char gb_chars_ban215 [] = {
    0xd5, 0xae, 
};
static const unsigned char gb_chars_ban216 [] = {
    0xd6, 0xb5, 
};
static const unsigned char gb_chars_ban217 [] = {
    0xc7, 0xe3, 
};
static const unsigned char gb_chars_ban218 [] = {
    0xd9, 0xc8, 
};
static const unsigned char gb_chars_ban219 [] = {
    0xbc, 0xd9, 0xd9, 0xca, 
};
static const unsigned char gb_chars_ban220 [] = {
    0xd9, 0xbc, 
};
static const unsigned char gb_chars_ban221 [] = {
    0xd9, 0xcb, 0xc6, 0xab, 
};
static const unsigned char gb_chars_ban222 [] = {
    0xd9, 0xc9, 
};
static const unsigned char gb_chars_ban223 [] = {
    0xd7, 0xf6, 
};
static const unsigned char gb_chars_ban224 [] = {
    0xcd, 0xa3, 
};
static const unsigned char gb_chars_ban225 [] = {
    0xbd, 0xa1, 
};
static const unsigned char gb_chars_ban226 [] = {
    0xd9, 0xcc, 
};
static const unsigned char gb_chars_ban227 [] = {
    0xc5, 0xbc, 0xcd, 0xb5, 
};
static const unsigned char gb_chars_ban228 [] = {
    0xd9, 0xcd, 
};
static const unsigned char gb_chars_ban229 [] = {
    0xd9, 0xc7, 0xb3, 0xa5, 0xbf, 0xfe, 
};
static const unsigned char gb_chars_ban230 [] = {
    0xb8, 0xb5, 
};
static const unsigned char gb_chars_ban231 [] = {
    0xc0, 0xfc, 
};
static const unsigned char gb_chars_ban232 [] = {
    0xb0, 0xf8, 
};
static const unsigned char gb_chars_ban233 [] = {
    0xb4, 0xf6, 
};
static const unsigned char gb_chars_ban234 [] = {
    0xd9, 0xce, 
};
static const unsigned char gb_chars_ban235 [] = {
    0xd9, 0xcf, 0xb4, 0xa2, 0xd9, 0xd0, 
};
static const unsigned char gb_chars_ban236 [] = {
    0xb4, 0xdf, 
};
static const unsigned char gb_chars_ban237 [] = {
    0xb0, 0xc1, 
};
static const unsigned char gb_chars_ban238 [] = {
    0xd9, 0xd1, 0xc9, 0xb5, 
};
static const unsigned char gb_chars_ban239 [] = {
    0xcf, 0xf1, 
};
static const unsigned char gb_chars_ban240 [] = {
    0xd9, 0xd2, 
};
static const unsigned char gb_chars_ban241 [] = {
    0xc1, 0xc5, 
};
static const unsigned char gb_chars_ban242 [] = {
    0xd9, 0xd6, 0xc9, 0xae, 
};
static const unsigned char gb_chars_ban243 [] = {
    0xd9, 0xd5, 0xd9, 0xd4, 0xd9, 0xd7, 
};
static const unsigned char gb_chars_ban244 [] = {
    0xcb, 0xdb, 
};
static const unsigned char gb_chars_ban245 [] = {
    0xbd, 0xa9, 
};
static const unsigned char gb_chars_ban246 [] = {
    0xc6, 0xa7, 
};
static const unsigned char gb_chars_ban247 [] = {
    0xd9, 0xd3, 0xd9, 0xd8, 
};
static const unsigned char gb_chars_ban248 [] = {
    0xd9, 0xd9, 
};
static const unsigned char gb_chars_ban249 [] = {
    0xc8, 0xe5, 
};
static const unsigned char gb_chars_ban250 [] = {
    0xc0, 0xdc, 
};
static const unsigned char gb_chars_ban251 [] = {
    0xb6, 0xf9, 0xd8, 0xa3, 0xd4, 0xca, 
};
static const unsigned char gb_chars_ban252 [] = {
    0xd4, 0xaa, 0xd0, 0xd6, 0xb3, 0xe4, 0xd5, 0xd7, 
};
static const unsigned char gb_chars_ban253 [] = {
    0xcf, 0xc8, 0xb9, 0xe2, 
};
static const unsigned char gb_chars_ban254 [] = {
    0xbf, 0xcb, 
};
static const unsigned char gb_chars_ban255 [] = {
    0xc3, 0xe2, 
};
static const unsigned char gb_chars_ban256 [] = {
    0xb6, 0xd2, 
};
static const unsigned char gb_chars_ban257 [] = {
    0xcd, 0xc3, 0xd9, 0xee, 0xd9, 0xf0, 
};
static const unsigned char gb_chars_ban258 [] = {
    0xb5, 0xb3, 
};
static const unsigned char gb_chars_ban259 [] = {
    0xb6, 0xb5, 
};
static const unsigned char gb_chars_ban260 [] = {
    0xbe, 0xa4, 
};
static const unsigned char gb_chars_ban261 [] = {
    0xc8, 0xeb, 
};
static const unsigned char gb_chars_ban262 [] = {
    0xc8, 0xab, 
};
static const unsigned char gb_chars_ban263 [] = {
    0xb0, 0xcb, 0xb9, 0xab, 0xc1, 0xf9, 0xd9, 0xe2, 
};
static const unsigned char gb_chars_ban264 [] = {
    0xc0, 0xbc, 0xb9, 0xb2, 
};
static const unsigned char gb_chars_ban265 [] = {
    0xb9, 0xd8, 0xd0, 0xcb, 0xb1, 0xf8, 0xc6, 0xe4, 0xbe, 0xdf, 
    0xb5, 0xe4, 0xd7, 0xc8, 
};
static const unsigned char gb_chars_ban266 [] = {
    0xd1, 0xf8, 0xbc, 0xe6, 0xca, 0xde, 
};
static const unsigned char gb_chars_ban267 [] = {
    0xbc, 0xbd, 0xd9, 0xe6, 0xd8, 0xe7, 
};
static const unsigned char gb_chars_ban268 [] = {
    0xc4, 0xda, 
};
static const unsigned char gb_chars_ban269 [] = {
    0xb8, 0xd4, 0xc8, 0xbd, 
};
static const unsigned char gb_chars_ban270 [] = {
    0xb2, 0xe1, 0xd4, 0xd9, 
};
static const unsigned char gb_chars_ban271 [] = {
    0xc3, 0xb0, 
};
static const unsigned char gb_chars_ban272 [] = {
    0xc3, 0xe1, 0xda, 0xa2, 0xc8, 0xdf, 
};
static const unsigned char gb_chars_ban273 [] = {
    0xd0, 0xb4, 
};
static const unsigned char gb_chars_ban274 [] = {
    0xbe, 0xfc, 0xc5, 0xa9, 
};
static const unsigned char gb_chars_ban275 [] = {
    0xb9, 0xda, 
};
static const unsigned char gb_chars_ban276 [] = {
    0xda, 0xa3, 
};
static const unsigned char gb_chars_ban277 [] = {
    0xd4, 0xa9, 0xda, 0xa4, 
};
static const unsigned char gb_chars_ban278 [] = {
    0xd9, 0xfb, 0xb6, 0xac, 
};
static const unsigned char gb_chars_ban279 [] = {
    0xb7, 0xeb, 0xb1, 0xf9, 0xd9, 0xfc, 0xb3, 0xe5, 0xbe, 0xf6, 
};
static const unsigned char gb_chars_ban280 [] = {
    0xbf, 0xf6, 0xd2, 0xb1, 0xc0, 0xe4, 
};
static const unsigned char gb_chars_ban281 [] = {
    0xb6, 0xb3, 0xd9, 0xfe, 0xd9, 0xfd, 
};
static const unsigned char gb_chars_ban282 [] = {
    0xbe, 0xbb, 
};
static const unsigned char gb_chars_ban283 [] = {
    0xc6, 0xe0, 
};
static const unsigned char gb_chars_ban284 [] = {
    0xd7, 0xbc, 0xda, 0xa1, 
};
static const unsigned char gb_chars_ban285 [] = {
    0xc1, 0xb9, 
};
static const unsigned char gb_chars_ban286 [] = {
    0xb5, 0xf2, 0xc1, 0xe8, 
};
static const unsigned char gb_chars_ban287 [] = {
    0xbc, 0xf5, 
};
static const unsigned char gb_chars_ban288 [] = {
    0xb4, 0xd5, 
};
static const unsigned char gb_chars_ban289 [] = {
    0xc1, 0xdd, 
};
static const unsigned char gb_chars_ban290 [] = {
    0xc4, 0xfd, 
};
static const unsigned char gb_chars_ban291 [] = {
    0xbc, 0xb8, 0xb7, 0xb2, 
};
static const unsigned char gb_chars_ban292 [] = {
    0xb7, 0xef, 
};
static const unsigned char gb_chars_ban293 [] = {
    0xd9, 0xec, 
};
static const unsigned char gb_chars_ban294 [] = {
    0xc6, 0xbe, 
};
static const unsigned char gb_chars_ban295 [] = {
    0xbf, 0xad, 0xbb, 0xcb, 
};
static const unsigned char gb_chars_ban296 [] = {
    0xb5, 0xca, 
};
static const unsigned char gb_chars_ban297 [] = {
    0xdb, 0xc9, 0xd0, 0xd7, 
};
static const unsigned char gb_chars_ban298 [] = {
    0xcd, 0xb9, 0xb0, 0xbc, 0xb3, 0xf6, 0xbb, 0xf7, 0xdb, 0xca, 
    0xba, 0xaf, 
};
static const unsigned char gb_chars_ban299 [] = {
    0xd4, 0xe4, 0xb5, 0xb6, 0xb5, 0xf3, 0xd8, 0xd6, 0xc8, 0xd0, 
};
static const unsigned char gb_chars_ban300 [] = {
    0xb7, 0xd6, 0xc7, 0xd0, 0xd8, 0xd7, 
};
static const unsigned char gb_chars_ban301 [] = {
    0xbf, 0xaf, 
};
static const unsigned char gb_chars_ban302 [] = {
    0xdb, 0xbb, 0xd8, 0xd8, 
};
static const unsigned char gb_chars_ban303 [] = {
    0xd0, 0xcc, 0xbb, 0xae, 
};
static const unsigned char gb_chars_ban304 [] = {
    0xeb, 0xbe, 0xc1, 0xd0, 0xc1, 0xf5, 0xd4, 0xf2, 0xb8, 0xd5, 
    0xb4, 0xb4, 
};
static const unsigned char gb_chars_ban305 [] = {
    0xb3, 0xf5, 
};
static const unsigned char gb_chars_ban306 [] = {
    0xc9, 0xbe, 
};
static const unsigned char gb_chars_ban307 [] = {
    0xc5, 0xd0, 
};
static const unsigned char gb_chars_ban308 [] = {
    0xc5, 0xd9, 0xc0, 0xfb, 
};
static const unsigned char gb_chars_ban309 [] = {
    0xb1, 0xf0, 
};
static const unsigned char gb_chars_ban310 [] = {
    0xd8, 0xd9, 0xb9, 0xce, 
};
static const unsigned char gb_chars_ban311 [] = {
    0xb5, 0xbd, 
};
static const unsigned char gb_chars_ban312 [] = {
    0xd8, 0xda, 
};
static const unsigned char gb_chars_ban313 [] = {
    0xd6, 0xc6, 0xcb, 0xa2, 0xc8, 0xaf, 0xc9, 0xb2, 0xb4, 0xcc, 
    0xbf, 0xcc, 
};
static const unsigned char gb_chars_ban314 [] = {
    0xb9, 0xf4, 
};
static const unsigned char gb_chars_ban315 [] = {
    0xd8, 0xdb, 0xd8, 0xdc, 0xb6, 0xe7, 0xbc, 0xc1, 0xcc, 0xea, 
};
static const unsigned char gb_chars_ban316 [] = {
    0xcf, 0xf7, 
};
static const unsigned char gb_chars_ban317 [] = {
    0xd8, 0xdd, 0xc7, 0xb0, 
};
static const unsigned char gb_chars_ban318 [] = {
    0xb9, 0xd0, 0xbd, 0xa3, 
};
static const unsigned char gb_chars_ban319 [] = {
    0xcc, 0xde, 
};
static const unsigned char gb_chars_ban320 [] = {
    0xc6, 0xca, 
};
static const unsigned char gb_chars_ban321 [] = {
    0xd8, 0xe0, 
};
static const unsigned char gb_chars_ban322 [] = {
    0xd8, 0xde, 
};
static const unsigned char gb_chars_ban323 [] = {
    0xd8, 0xdf, 
};
static const unsigned char gb_chars_ban324 [] = {
    0xb0, 0xfe, 
};
static const unsigned char gb_chars_ban325 [] = {
    0xbe, 0xe7, 
};
static const unsigned char gb_chars_ban326 [] = {
    0xca, 0xa3, 0xbc, 0xf4, 
};
static const unsigned char gb_chars_ban327 [] = {
    0xb8, 0xb1, 
};
static const unsigned char gb_chars_ban328 [] = {
    0xb8, 0xee, 
};
static const unsigned char gb_chars_ban329 [] = {
    0xd8, 0xe2, 
};
static const unsigned char gb_chars_ban330 [] = {
    0xbd, 0xcb, 
};
static const unsigned char gb_chars_ban331 [] = {
    0xd8, 0xe4, 0xd8, 0xe3, 
};
static const unsigned char gb_chars_ban332 [] = {
    0xc5, 0xfc, 
};
static const unsigned char gb_chars_ban333 [] = {
    0xd8, 0xe5, 
};
static const unsigned char gb_chars_ban334 [] = {
    0xd8, 0xe6, 
};
static const unsigned char gb_chars_ban335 [] = {
    0xc1, 0xa6, 
};
static const unsigned char gb_chars_ban336 [] = {
    0xc8, 0xb0, 0xb0, 0xec, 0xb9, 0xa6, 0xbc, 0xd3, 0xce, 0xf1, 
    0xdb, 0xbd, 0xc1, 0xd3, 
};
static const unsigned char gb_chars_ban337 [] = {
    0xb6, 0xaf, 0xd6, 0xfa, 0xc5, 0xac, 0xbd, 0xd9, 0xdb, 0xbe, 
    0xdb, 0xbf, 
};
static const unsigned char gb_chars_ban338 [] = {
    0xc0, 0xf8, 0xbe, 0xa2, 0xc0, 0xcd, 
};
static const unsigned char gb_chars_ban339 [] = {
    0xdb, 0xc0, 0xca, 0xc6, 
};
static const unsigned char gb_chars_ban340 [] = {
    0xb2, 0xaa, 
};
static const unsigned char gb_chars_ban341 [] = {
    0xd3, 0xc2, 
};
static const unsigned char gb_chars_ban342 [] = {
    0xc3, 0xe3, 
};
static const unsigned char gb_chars_ban343 [] = {
    0xd1, 0xab, 
};
static const unsigned char gb_chars_ban344 [] = {
    0xdb, 0xc2, 
};
static const unsigned char gb_chars_ban345 [] = {
    0xc0, 0xd5, 
};
static const unsigned char gb_chars_ban346 [] = {
    0xdb, 0xc3, 
};
static const unsigned char gb_chars_ban347 [] = {
    0xbf, 0xb1, 
};
static const unsigned char gb_chars_ban348 [] = {
    0xc4, 0xbc, 
};
static const unsigned char gb_chars_ban349 [] = {
    0xc7, 0xda, 
};
static const unsigned char gb_chars_ban350 [] = {
    0xdb, 0xc4, 
};
static const unsigned char gb_chars_ban351 [] = {
    0xd9, 0xe8, 0xc9, 0xd7, 
};
static const unsigned char gb_chars_ban352 [] = {
    0xb9, 0xb4, 0xce, 0xf0, 0xd4, 0xc8, 
};
static const unsigned char gb_chars_ban353 [] = {
    0xb0, 0xfc, 0xb4, 0xd2, 
};
static const unsigned char gb_chars_ban354 [] = {
    0xd0, 0xd9, 
};
static const unsigned char gb_chars_ban355 [] = {
    0xd9, 0xe9, 
};
static const unsigned char gb_chars_ban356 [] = {
    0xde, 0xcb, 0xd9, 0xeb, 
};
static const unsigned char gb_chars_ban357 [] = {
    0xd8, 0xb0, 0xbb, 0xaf, 0xb1, 0xb1, 
};
static const unsigned char gb_chars_ban358 [] = {
    0xb3, 0xd7, 0xd8, 0xce, 
};
static const unsigned char gb_chars_ban359 [] = {
    0xd4, 0xd1, 
};
static const unsigned char gb_chars_ban360 [] = {
    0xbd, 0xb3, 0xbf, 0xef, 
};
static const unsigned char gb_chars_ban361 [] = {
    0xcf, 0xbb, 
};
static const unsigned char gb_chars_ban362 [] = {
    0xd8, 0xd0, 
};
static const unsigned char gb_chars_ban363 [] = {
    0xb7, 0xcb, 
};
static const unsigned char gb_chars_ban364 [] = {
    0xd8, 0xd1, 
};
static const unsigned char gb_chars_ban365 [] = {
    0xc6, 0xa5, 0xc7, 0xf8, 0xd2, 0xbd, 
};
static const unsigned char gb_chars_ban366 [] = {
    0xd8, 0xd2, 0xc4, 0xe4, 
};
static const unsigned char gb_chars_ban367 [] = {
    0xca, 0xae, 
};
static const unsigned char gb_chars_ban368 [] = {
    0xc7, 0xa7, 
};
static const unsigned char gb_chars_ban369 [] = {
    0xd8, 0xa6, 
};
static const unsigned char gb_chars_ban370 [] = {
    0xc9, 0xfd, 0xce, 0xe7, 0xbb, 0xdc, 0xb0, 0xeb, 
};
static const unsigned char gb_chars_ban371 [] = {
    0xbb, 0xaa, 0xd0, 0xad, 
};
static const unsigned char gb_chars_ban372 [] = {
    0xb1, 0xb0, 0xd7, 0xe4, 0xd7, 0xbf, 
};
static const unsigned char gb_chars_ban373 [] = {
    0xb5, 0xa5, 0xc2, 0xf4, 0xc4, 0xcf, 
};
static const unsigned char gb_chars_ban374 [] = {
    0xb2, 0xa9, 
};
static const unsigned char gb_chars_ban375 [] = {
    0xb2, 0xb7, 
};
static const unsigned char gb_chars_ban376 [] = {
    0xb1, 0xe5, 0xdf, 0xb2, 0xd5, 0xbc, 0xbf, 0xa8, 0xc2, 0xac, 
    0xd8, 0xd5, 0xc2, 0xb1, 
};
static const unsigned char gb_chars_ban377 [] = {
    0xd8, 0xd4, 0xce, 0xd4, 
};
static const unsigned char gb_chars_ban378 [] = {
    0xda, 0xe0, 
};
static const unsigned char gb_chars_ban379 [] = {
    0xce, 0xc0, 
};
static const unsigned char gb_chars_ban380 [] = {
    0xd8, 0xb4, 0xc3, 0xae, 0xd3, 0xa1, 0xce, 0xa3, 
};
static const unsigned char gb_chars_ban381 [] = {
    0xbc, 0xb4, 0xc8, 0xb4, 0xc2, 0xd1, 
};
static const unsigned char gb_chars_ban382 [] = {
    0xbe, 0xed, 0xd0, 0xb6, 
};
static const unsigned char gb_chars_ban383 [] = {
    0xda, 0xe1, 
};
static const unsigned char gb_chars_ban384 [] = {
    0xc7, 0xe4, 
};
static const unsigned char gb_chars_ban385 [] = {
    0xb3, 0xa7, 
};
static const unsigned char gb_chars_ban386 [] = {
    0xb6, 0xf2, 0xcc, 0xfc, 0xc0, 0xfa, 
};
static const unsigned char gb_chars_ban387 [] = {
    0xc0, 0xf7, 
};
static const unsigned char gb_chars_ban388 [] = {
    0xd1, 0xb9, 0xd1, 0xe1, 0xd8, 0xc7, 
};
static const unsigned char gb_chars_ban389 [] = {
    0xb2, 0xde, 
};
static const unsigned char gb_chars_ban390 [] = {
    0xc0, 0xe5, 
};
static const unsigned char gb_chars_ban391 [] = {
    0xba, 0xf1, 
};
static const unsigned char gb_chars_ban392 [] = {
    0xd8, 0xc8, 
};
static const unsigned char gb_chars_ban393 [] = {
    0xd4, 0xad, 
};
static const unsigned char gb_chars_ban394 [] = {
    0xcf, 0xe1, 0xd8, 0xc9, 
};
static const unsigned char gb_chars_ban395 [] = {
    0xd8, 0xca, 0xcf, 0xc3, 
};
static const unsigned char gb_chars_ban396 [] = {
    0xb3, 0xf8, 0xbe, 0xc7, 
};
static const unsigned char gb_chars_ban397 [] = {
    0xd8, 0xcb, 
};
static const unsigned char gb_chars_ban398 [] = {
    0xdb, 0xcc, 
};
static const unsigned char gb_chars_ban399 [] = {
    0xc8, 0xa5, 
};
static const unsigned char gb_chars_ban400 [] = {
    0xcf, 0xd8, 
};
static const unsigned char gb_chars_ban401 [] = {
    0xc8, 0xfe, 0xb2, 0xce, 
};
static const unsigned char gb_chars_ban402 [] = {
    0xd3, 0xd6, 0xb2, 0xe6, 0xbc, 0xb0, 0xd3, 0xd1, 0xcb, 0xab, 
    0xb7, 0xb4, 
};
static const unsigned char gb_chars_ban403 [] = {
    0xb7, 0xa2, 
};
static const unsigned char gb_chars_ban404 [] = {
    0xca, 0xe5, 
};
static const unsigned char gb_chars_ban405 [] = {
    0xc8, 0xa1, 0xca, 0xdc, 0xb1, 0xe4, 0xd0, 0xf0, 
};
static const unsigned char gb_chars_ban406 [] = {
    0xc5, 0xd1, 
};
static const unsigned char gb_chars_ban407 [] = {
    0xdb, 0xc5, 0xb5, 0xfe, 
};
static const unsigned char gb_chars_ban408 [] = {
    0xbf, 0xda, 0xb9, 0xc5, 0xbe, 0xe4, 0xc1, 0xed, 
};
static const unsigned char gb_chars_ban409 [] = {
    0xdf, 0xb6, 0xdf, 0xb5, 0xd6, 0xbb, 0xbd, 0xd0, 0xd5, 0xd9, 
    0xb0, 0xc8, 0xb6, 0xa3, 0xbf, 0xc9, 0xcc, 0xa8, 0xdf, 0xb3, 
    0xca, 0xb7, 0xd3, 0xd2, 
};
static const unsigned char gb_chars_ban410 [] = {
    0xd8, 0xcf, 0xd2, 0xb6, 0xba, 0xc5, 0xcb, 0xbe, 0xcc, 0xbe, 
};
static const unsigned char gb_chars_ban411 [] = {
    0xdf, 0xb7, 0xb5, 0xf0, 0xdf, 0xb4, 
};
static const unsigned char gb_chars_ban412 [] = {
    0xd3, 0xf5, 
};
static const unsigned char gb_chars_ban413 [] = {
    0xb3, 0xd4, 0xb8, 0xf7, 
};
static const unsigned char gb_chars_ban414 [] = {
    0xdf, 0xba, 
};
static const unsigned char gb_chars_ban415 [] = {
    0xba, 0xcf, 0xbc, 0xaa, 0xb5, 0xf5, 
};
static const unsigned char gb_chars_ban416 [] = {
    0xcd, 0xac, 0xc3, 0xfb, 0xba, 0xf3, 0xc0, 0xf4, 0xcd, 0xc2, 
    0xcf, 0xf2, 0xdf, 0xb8, 0xcf, 0xc5, 
};
static const unsigned char gb_chars_ban417 [] = {
    0xc2, 0xc0, 0xdf, 0xb9, 0xc2, 0xf0, 
};
static const unsigned char gb_chars_ban418 [] = {
    0xbe, 0xfd, 
};
static const unsigned char gb_chars_ban419 [] = {
    0xc1, 0xdf, 0xcd, 0xcc, 0xd2, 0xf7, 0xb7, 0xcd, 0xdf, 0xc1, 
};
static const unsigned char gb_chars_ban420 [] = {
    0xdf, 0xc4, 
};
static const unsigned char gb_chars_ban421 [] = {
    0xb7, 0xf1, 0xb0, 0xc9, 0xb6, 0xd6, 0xb7, 0xd4, 
};
static const unsigned char gb_chars_ban422 [] = {
    0xba, 0xac, 0xcc, 0xfd, 0xbf, 0xd4, 0xcb, 0xb1, 0xc6, 0xf4, 
};
static const unsigned char gb_chars_ban423 [] = {
    0xd6, 0xa8, 0xdf, 0xc5, 
};
static const unsigned char gb_chars_ban424 [] = {
    0xce, 0xe2, 0xb3, 0xb3, 
};
static const unsigned char gb_chars_ban425 [] = {
    0xce, 0xfc, 0xb4, 0xb5, 
};
static const unsigned char gb_chars_ban426 [] = {
    0xce, 0xc7, 0xba, 0xf0, 
};
static const unsigned char gb_chars_ban427 [] = {
    0xce, 0xe1, 
};
static const unsigned char gb_chars_ban428 [] = {
    0xd1, 0xbd, 
};
static const unsigned char gb_chars_ban429 [] = {
    0xdf, 0xc0, 
};
static const unsigned char gb_chars_ban430 [] = {
    0xb4, 0xf4, 
};
static const unsigned char gb_chars_ban431 [] = {
    0xb3, 0xca, 
};
static const unsigned char gb_chars_ban432 [] = {
    0xb8, 0xe6, 0xdf, 0xbb, 
};
static const unsigned char gb_chars_ban433 [] = {
    0xc4, 0xc5, 
};
static const unsigned char gb_chars_ban434 [] = {
    0xdf, 0xbc, 0xdf, 0xbd, 0xdf, 0xbe, 0xc5, 0xbb, 0xdf, 0xbf, 
    0xdf, 0xc2, 0xd4, 0xb1, 0xdf, 0xc3, 
};
static const unsigned char gb_chars_ban435 [] = {
    0xc7, 0xba, 0xce, 0xd8, 
};
static const unsigned char gb_chars_ban436 [] = {
    0xc4, 0xd8, 
};
static const unsigned char gb_chars_ban437 [] = {
    0xdf, 0xca, 
};
static const unsigned char gb_chars_ban438 [] = {
    0xdf, 0xcf, 
};
static const unsigned char gb_chars_ban439 [] = {
    0xd6, 0xdc, 
};
static const unsigned char gb_chars_ban440 [] = {
    0xdf, 0xc9, 0xdf, 0xda, 0xce, 0xb6, 
};
static const unsigned char gb_chars_ban441 [] = {
    0xba, 0xc7, 0xdf, 0xce, 0xdf, 0xc8, 0xc5, 0xde, 
};
static const unsigned char gb_chars_ban442 [] = {
    0xc9, 0xeb, 0xba, 0xf4, 0xc3, 0xfc, 
};
static const unsigned char gb_chars_ban443 [] = {
    0xbe, 0xd7, 
};
static const unsigned char gb_chars_ban444 [] = {
    0xdf, 0xc6, 
};
static const unsigned char gb_chars_ban445 [] = {
    0xdf, 0xcd, 
};
static const unsigned char gb_chars_ban446 [] = {
    0xc5, 0xd8, 
};
static const unsigned char gb_chars_ban447 [] = {
    0xd5, 0xa6, 0xba, 0xcd, 
};
static const unsigned char gb_chars_ban448 [] = {
    0xbe, 0xcc, 0xd3, 0xbd, 0xb8, 0xc0, 
};
static const unsigned char gb_chars_ban449 [] = {
    0xd6, 0xe4, 
};
static const unsigned char gb_chars_ban450 [] = {
    0xdf, 0xc7, 0xb9, 0xbe, 0xbf, 0xa7, 
};
static const unsigned char gb_chars_ban451 [] = {
    0xc1, 0xfc, 0xdf, 0xcb, 0xdf, 0xcc, 
};
static const unsigned char gb_chars_ban452 [] = {
    0xdf, 0xd0, 
};
static const unsigned char gb_chars_ban453 [] = {
    0xdf, 0xdb, 0xdf, 0xe5, 
};
static const unsigned char gb_chars_ban454 [] = {
    0xdf, 0xd7, 0xdf, 0xd6, 0xd7, 0xc9, 0xdf, 0xe3, 0xdf, 0xe4, 
    0xe5, 0xeb, 0xd2, 0xa7, 0xdf, 0xd2, 
};
static const unsigned char gb_chars_ban455 [] = {
    0xbf, 0xa9, 
};
static const unsigned char gb_chars_ban456 [] = {
    0xd4, 0xdb, 
};
static const unsigned char gb_chars_ban457 [] = {
    0xbf, 0xc8, 0xdf, 0xd4, 
};
static const unsigned char gb_chars_ban458 [] = {
    0xcf, 0xcc, 
};
static const unsigned char gb_chars_ban459 [] = {
    0xdf, 0xdd, 
};
static const unsigned char gb_chars_ban460 [] = {
    0xd1, 0xca, 
};
static const unsigned char gb_chars_ban461 [] = {
    0xdf, 0xde, 0xb0, 0xa7, 0xc6, 0xb7, 0xdf, 0xd3, 
};
static const unsigned char gb_chars_ban462 [] = {
    0xba, 0xe5, 
};
static const unsigned char gb_chars_ban463 [] = {
    0xb6, 0xdf, 0xcd, 0xdb, 0xb9, 0xfe, 0xd4, 0xd5, 
};
static const unsigned char gb_chars_ban464 [] = {
    0xdf, 0xdf, 0xcf, 0xec, 0xb0, 0xa5, 0xdf, 0xe7, 0xdf, 0xd1, 
    0xd1, 0xc6, 0xdf, 0xd5, 0xdf, 0xd8, 0xdf, 0xd9, 0xdf, 0xdc, 
};
static const unsigned char gb_chars_ban465 [] = {
    0xbb, 0xa9, 
};
static const unsigned char gb_chars_ban466 [] = {
    0xdf, 0xe0, 0xdf, 0xe1, 
};
static const unsigned char gb_chars_ban467 [] = {
    0xdf, 0xe2, 0xdf, 0xe6, 0xdf, 0xe8, 0xd3, 0xb4, 
};
static const unsigned char gb_chars_ban468 [] = {
    0xb8, 0xe7, 0xc5, 0xb6, 0xdf, 0xea, 0xc9, 0xda, 0xc1, 0xa8, 
    0xc4, 0xc4, 
};
static const unsigned char gb_chars_ban469 [] = {
    0xbf, 0xde, 0xcf, 0xf8, 
};
static const unsigned char gb_chars_ban470 [] = {
    0xd5, 0xdc, 0xdf, 0xee, 
};
static const unsigned char gb_chars_ban471 [] = {
    0xb2, 0xb8, 
};
static const unsigned char gb_chars_ban472 [] = {
    0xba, 0xdf, 0xdf, 0xec, 
};
static const unsigned char gb_chars_ban473 [] = {
    0xdb, 0xc1, 
};
static const unsigned char gb_chars_ban474 [] = {
    0xd1, 0xe4, 
};
static const unsigned char gb_chars_ban475 [] = {
    0xcb, 0xf4, 0xb4, 0xbd, 
};
static const unsigned char gb_chars_ban476 [] = {
    0xb0, 0xa6, 
};
static const unsigned char gb_chars_ban477 [] = {
    0xdf, 0xf1, 0xcc, 0xc6, 0xdf, 0xf2, 
};
static const unsigned char gb_chars_ban478 [] = {
    0xdf, 0xed, 
};
static const unsigned char gb_chars_ban479 [] = {
    0xdf, 0xe9, 
};
static const unsigned char gb_chars_ban480 [] = {
    0xdf, 0xeb, 
};
static const unsigned char gb_chars_ban481 [] = {
    0xdf, 0xef, 0xdf, 0xf0, 0xbb, 0xbd, 
};
static const unsigned char gb_chars_ban482 [] = {
    0xdf, 0xf3, 
};
static const unsigned char gb_chars_ban483 [] = {
    0xdf, 0xf4, 
};
static const unsigned char gb_chars_ban484 [] = {
    0xbb, 0xa3, 
};
static const unsigned char gb_chars_ban485 [] = {
    0xca, 0xdb, 0xce, 0xa8, 0xe0, 0xa7, 0xb3, 0xaa, 
};
static const unsigned char gb_chars_ban486 [] = {
    0xe0, 0xa6, 
};
static const unsigned char gb_chars_ban487 [] = {
    0xe0, 0xa1, 
};
static const unsigned char gb_chars_ban488 [] = {
    0xdf, 0xfe, 
};
static const unsigned char gb_chars_ban489 [] = {
    0xcd, 0xd9, 0xdf, 0xfc, 
};
static const unsigned char gb_chars_ban490 [] = {
    0xdf, 0xfa, 
};
static const unsigned char gb_chars_ban491 [] = {
    0xbf, 0xd0, 0xd7, 0xc4, 
};
static const unsigned char gb_chars_ban492 [] = {
    0xc9, 0xcc, 
};
static const unsigned char gb_chars_ban493 [] = {
    0xdf, 0xf8, 0xb0, 0xa1, 
};
static const unsigned char gb_chars_ban494 [] = {
    0xdf, 0xfd, 
};
static const unsigned char gb_chars_ban495 [] = {
    0xdf, 0xfb, 0xe0, 0xa2, 
};
static const unsigned char gb_chars_ban496 [] = {
    0xe0, 0xa8, 
};
static const unsigned char gb_chars_ban497 [] = {
    0xb7, 0xc8, 
};
static const unsigned char gb_chars_ban498 [] = {
    0xc6, 0xa1, 0xc9, 0xb6, 0xc0, 0xb2, 0xdf, 0xf5, 
};
static const unsigned char gb_chars_ban499 [] = {
    0xc5, 0xbe, 
};
static const unsigned char gb_chars_ban500 [] = {
    0xd8, 0xc4, 0xdf, 0xf9, 0xc4, 0xf6, 
};
static const unsigned char gb_chars_ban501 [] = {
    0xe0, 0xa3, 0xe0, 0xa4, 0xe0, 0xa5, 0xd0, 0xa5, 
};
static const unsigned char gb_chars_ban502 [] = {
    0xe0, 0xb4, 0xcc, 0xe4, 
};
static const unsigned char gb_chars_ban503 [] = {
    0xe0, 0xb1, 
};
static const unsigned char gb_chars_ban504 [] = {
    0xbf, 0xa6, 0xe0, 0xaf, 0xce, 0xb9, 0xe0, 0xab, 0xc9, 0xc6, 
};
static const unsigned char gb_chars_ban505 [] = {
    0xc0, 0xae, 0xe0, 0xae, 0xba, 0xed, 0xba, 0xb0, 0xe0, 0xa9, 
};
static const unsigned char gb_chars_ban506 [] = {
    0xdf, 0xf6, 
};
static const unsigned char gb_chars_ban507 [] = {
    0xe0, 0xb3, 
};
static const unsigned char gb_chars_ban508 [] = {
    0xe0, 0xb8, 
};
static const unsigned char gb_chars_ban509 [] = {
    0xb4, 0xad, 0xe0, 0xb9, 
};
static const unsigned char gb_chars_ban510 [] = {
    0xcf, 0xb2, 0xba, 0xc8, 
};
static const unsigned char gb_chars_ban511 [] = {
    0xe0, 0xb0, 
};
static const unsigned char gb_chars_ban512 [] = {
    0xd0, 0xfa, 
};
static const unsigned char gb_chars_ban513 [] = {
    0xe0, 0xac, 
};
static const unsigned char gb_chars_ban514 [] = {
    0xd4, 0xfb, 
};
static const unsigned char gb_chars_ban515 [] = {
    0xdf, 0xf7, 
};
static const unsigned char gb_chars_ban516 [] = {
    0xc5, 0xe7, 
};
static const unsigned char gb_chars_ban517 [] = {
    0xe0, 0xad, 
};
static const unsigned char gb_chars_ban518 [] = {
    0xd3, 0xf7, 
};
static const unsigned char gb_chars_ban519 [] = {
    0xe0, 0xb6, 0xe0, 0xb7, 
};
static const unsigned char gb_chars_ban520 [] = {
    0xe0, 0xc4, 0xd0, 0xe1, 
};
static const unsigned char gb_chars_ban521 [] = {
    0xe0, 0xbc, 
};
static const unsigned char gb_chars_ban522 [] = {
    0xe0, 0xc9, 0xe0, 0xca, 
};
static const unsigned char gb_chars_ban523 [] = {
    0xe0, 0xbe, 0xe0, 0xaa, 0xc9, 0xa4, 0xe0, 0xc1, 
};
static const unsigned char gb_chars_ban524 [] = {
    0xe0, 0xb2, 
};
static const unsigned char gb_chars_ban525 [] = {
    0xca, 0xc8, 0xe0, 0xc3, 
};
static const unsigned char gb_chars_ban526 [] = {
    0xe0, 0xb5, 
};
static const unsigned char gb_chars_ban527 [] = {
    0xce, 0xcb, 
};
static const unsigned char gb_chars_ban528 [] = {
    0xcb, 0xc3, 0xe0, 0xcd, 0xe0, 0xc6, 0xe0, 0xc2, 
};
static const unsigned char gb_chars_ban529 [] = {
    0xe0, 0xcb, 
};
static const unsigned char gb_chars_ban530 [] = {
    0xe0, 0xba, 0xe0, 0xbf, 0xe0, 0xc0, 
};
static const unsigned char gb_chars_ban531 [] = {
    0xe0, 0xc5, 
};
static const unsigned char gb_chars_ban532 [] = {
    0xe0, 0xc7, 0xe0, 0xc8, 
};
static const unsigned char gb_chars_ban533 [] = {
    0xe0, 0xcc, 
};
static const unsigned char gb_chars_ban534 [] = {
    0xe0, 0xbb, 
};
static const unsigned char gb_chars_ban535 [] = {
    0xcb, 0xd4, 0xe0, 0xd5, 
};
static const unsigned char gb_chars_ban536 [] = {
    0xe0, 0xd6, 0xe0, 0xd2, 
};
static const unsigned char gb_chars_ban537 [] = {
    0xe0, 0xd0, 0xbc, 0xce, 
};
static const unsigned char gb_chars_ban538 [] = {
    0xe0, 0xd1, 
};
static const unsigned char gb_chars_ban539 [] = {
    0xb8, 0xc2, 0xd8, 0xc5, 
};
static const unsigned char gb_chars_ban540 [] = {
    0xd0, 0xea, 
};
static const unsigned char gb_chars_ban541 [] = {
    0xc2, 0xef, 
};
static const unsigned char gb_chars_ban542 [] = {
    0xe0, 0xcf, 0xe0, 0xbd, 
};
static const unsigned char gb_chars_ban543 [] = {
    0xe0, 0xd4, 0xe0, 0xd3, 
};
static const unsigned char gb_chars_ban544 [] = {
    0xe0, 0xd7, 
};
static const unsigned char gb_chars_ban545 [] = {
    0xe0, 0xdc, 0xe0, 0xd8, 
};
static const unsigned char gb_chars_ban546 [] = {
    0xd6, 0xf6, 0xb3, 0xb0, 
};
static const unsigned char gb_chars_ban547 [] = {
    0xd7, 0xec, 
};
static const unsigned char gb_chars_ban548 [] = {
    0xcb, 0xbb, 
};
static const unsigned char gb_chars_ban549 [] = {
    0xe0, 0xda, 
};
static const unsigned char gb_chars_ban550 [] = {
    0xce, 0xfb, 
};
static const unsigned char gb_chars_ban551 [] = {
    0xba, 0xd9, 
};
static const unsigned char gb_chars_ban552 [] = {
    0xe0, 0xe1, 0xe0, 0xdd, 0xd2, 0xad, 
};
static const unsigned char gb_chars_ban553 [] = {
    0xe0, 0xe2, 
};
static const unsigned char gb_chars_ban554 [] = {
    0xe0, 0xdb, 0xe0, 0xd9, 0xe0, 0xdf, 
};
static const unsigned char gb_chars_ban555 [] = {
    0xe0, 0xe0, 
};
static const unsigned char gb_chars_ban556 [] = {
    0xe0, 0xde, 
};
static const unsigned char gb_chars_ban557 [] = {
    0xe0, 0xe4, 
};
static const unsigned char gb_chars_ban558 [] = {
    0xc6, 0xf7, 0xd8, 0xac, 0xd4, 0xeb, 0xe0, 0xe6, 0xca, 0xc9, 
};
static const unsigned char gb_chars_ban559 [] = {
    0xe0, 0xe5, 
};
static const unsigned char gb_chars_ban560 [] = {
    0xb8, 0xc1, 
};
static const unsigned char gb_chars_ban561 [] = {
    0xe0, 0xe7, 0xe0, 0xe8, 
};
static const unsigned char gb_chars_ban562 [] = {
    0xe0, 0xe9, 0xe0, 0xe3, 
};
static const unsigned char gb_chars_ban563 [] = {
    0xba, 0xbf, 0xcc, 0xe7, 
};
static const unsigned char gb_chars_ban564 [] = {
    0xe0, 0xea, 
};
static const unsigned char gb_chars_ban565 [] = {
    0xcf, 0xf9, 
};
static const unsigned char gb_chars_ban566 [] = {
    0xe0, 0xeb, 
};
static const unsigned char gb_chars_ban567 [] = {
    0xc8, 0xc2, 
};
static const unsigned char gb_chars_ban568 [] = {
    0xbd, 0xc0, 
};
static const unsigned char gb_chars_ban569 [] = {
    0xc4, 0xd2, 
};
static const unsigned char gb_chars_ban570 [] = {
    0xe0, 0xec, 
};
static const unsigned char gb_chars_ban571 [] = {
    0xe0, 0xed, 
};
static const unsigned char gb_chars_ban572 [] = {
    0xc7, 0xf4, 0xcb, 0xc4, 
};
static const unsigned char gb_chars_ban573 [] = {
    0xe0, 0xee, 0xbb, 0xd8, 0xd8, 0xb6, 0xd2, 0xf2, 0xe0, 0xef, 
    0xcd, 0xc5, 
};
static const unsigned char gb_chars_ban574 [] = {
    0xb6, 0xda, 
};
static const unsigned char gb_chars_ban575 [] = {
    0xe0, 0xf1, 
};
static const unsigned char gb_chars_ban576 [] = {
    0xd4, 0xb0, 
};
static const unsigned char gb_chars_ban577 [] = {
    0xc0, 0xa7, 0xb4, 0xd1, 
};
static const unsigned char gb_chars_ban578 [] = {
    0xce, 0xa7, 0xe0, 0xf0, 
};
static const unsigned char gb_chars_ban579 [] = {
    0xe0, 0xf2, 0xb9, 0xcc, 
};
static const unsigned char gb_chars_ban580 [] = {
    0xb9, 0xfa, 0xcd, 0xbc, 0xe0, 0xf3, 
};
static const unsigned char gb_chars_ban581 [] = {
    0xc6, 0xd4, 0xe0, 0xf4, 
};
static const unsigned char gb_chars_ban582 [] = {
    0xd4, 0xb2, 
};
static const unsigned char gb_chars_ban583 [] = {
    0xc8, 0xa6, 0xe0, 0xf6, 0xe0, 0xf5, 
};
static const unsigned char gb_chars_ban584 [] = {
    0xe0, 0xf7, 
};
static const unsigned char gb_chars_ban585 [] = {
    0xcd, 0xc1, 
};
static const unsigned char gb_chars_ban586 [] = {
    0xca, 0xa5, 
};
static const unsigned char gb_chars_ban587 [] = {
    0xd4, 0xda, 0xdb, 0xd7, 0xdb, 0xd9, 
};
static const unsigned char gb_chars_ban588 [] = {
    0xdb, 0xd8, 0xb9, 0xe7, 0xdb, 0xdc, 0xdb, 0xdd, 0xb5, 0xd8, 
};
static const unsigned char gb_chars_ban589 [] = {
    0xdb, 0xda, 
};
static const unsigned char gb_chars_ban590 [] = {
    0xdb, 0xdb, 0xb3, 0xa1, 0xdb, 0xdf, 
};
static const unsigned char gb_chars_ban591 [] = {
    0xbb, 0xf8, 
};
static const unsigned char gb_chars_ban592 [] = {
    0xd6, 0xb7, 
};
static const unsigned char gb_chars_ban593 [] = {
    0xdb, 0xe0, 
};
static const unsigned char gb_chars_ban594 [] = {
    0xbe, 0xf9, 
};
static const unsigned char gb_chars_ban595 [] = {
    0xb7, 0xbb, 
};
static const unsigned char gb_chars_ban596 [] = {
    0xdb, 0xd0, 0xcc, 0xae, 0xbf, 0xb2, 0xbb, 0xb5, 0xd7, 0xf8, 
    0xbf, 0xd3, 
};
static const unsigned char gb_chars_ban597 [] = {
    0xbf, 0xe9, 
};
static const unsigned char gb_chars_ban598 [] = {
    0xbc, 0xe1, 0xcc, 0xb3, 0xdb, 0xde, 0xb0, 0xd3, 0xce, 0xeb, 
    0xb7, 0xd8, 0xd7, 0xb9, 0xc6, 0xc2, 
};
static const unsigned char gb_chars_ban599 [] = {
    0xc0, 0xa4, 
};
static const unsigned char gb_chars_ban600 [] = {
    0xcc, 0xb9, 
};
static const unsigned char gb_chars_ban601 [] = {
    0xdb, 0xe7, 0xdb, 0xe1, 0xc6, 0xba, 0xdb, 0xe3, 
};
static const unsigned char gb_chars_ban602 [] = {
    0xdb, 0xe8, 
};
static const unsigned char gb_chars_ban603 [] = {
    0xc5, 0xf7, 
};
static const unsigned char gb_chars_ban604 [] = {
    0xdb, 0xea, 
};
static const unsigned char gb_chars_ban605 [] = {
    0xdb, 0xe9, 0xbf, 0xc0, 
};
static const unsigned char gb_chars_ban606 [] = {
    0xdb, 0xe6, 0xdb, 0xe5, 
};
static const unsigned char gb_chars_ban607 [] = {
    0xb4, 0xb9, 0xc0, 0xac, 0xc2, 0xa2, 0xdb, 0xe2, 0xdb, 0xe4, 
};
static const unsigned char gb_chars_ban608 [] = {
    0xd0, 0xcd, 0xdb, 0xed, 
};
static const unsigned char gb_chars_ban609 [] = {
    0xc0, 0xdd, 0xdb, 0xf2, 
};
static const unsigned char gb_chars_ban610 [] = {
    0xb6, 0xe2, 
};
static const unsigned char gb_chars_ban611 [] = {
    0xdb, 0xf3, 0xdb, 0xd2, 0xb9, 0xb8, 0xd4, 0xab, 0xdb, 0xec, 
};
static const unsigned char gb_chars_ban612 [] = {
    0xbf, 0xd1, 0xdb, 0xf0, 
};
static const unsigned char gb_chars_ban613 [] = {
    0xdb, 0xd1, 
};
static const unsigned char gb_chars_ban614 [] = {
    0xb5, 0xe6, 
};
static const unsigned char gb_chars_ban615 [] = {
    0xdb, 0xeb, 0xbf, 0xe5, 
};
static const unsigned char gb_chars_ban616 [] = {
    0xdb, 0xee, 
};
static const unsigned char gb_chars_ban617 [] = {
    0xdb, 0xf1, 
};
static const unsigned char gb_chars_ban618 [] = {
    0xdb, 0xf9, 
};
static const unsigned char gb_chars_ban619 [] = {
    0xb9, 0xa1, 0xb0, 0xa3, 
};
static const unsigned char gb_chars_ban620 [] = {
    0xc2, 0xf1, 
};
static const unsigned char gb_chars_ban621 [] = {
    0xb3, 0xc7, 0xdb, 0xef, 
};
static const unsigned char gb_chars_ban622 [] = {
    0xdb, 0xf8, 
};
static const unsigned char gb_chars_ban623 [] = {
    0xc6, 0xd2, 0xdb, 0xf4, 
};
static const unsigned char gb_chars_ban624 [] = {
    0xdb, 0xf5, 0xdb, 0xf7, 0xdb, 0xf6, 
};
static const unsigned char gb_chars_ban625 [] = {
    0xdb, 0xfe, 
};
static const unsigned char gb_chars_ban626 [] = {
    0xd3, 0xf2, 0xb2, 0xba, 
};
static const unsigned char gb_chars_ban627 [] = {
    0xdb, 0xfd, 
};
static const unsigned char gb_chars_ban628 [] = {
    0xdc, 0xa4, 
};
static const unsigned char gb_chars_ban629 [] = {
    0xdb, 0xfb, 
};
static const unsigned char gb_chars_ban630 [] = {
    0xdb, 0xfa, 
};
static const unsigned char gb_chars_ban631 [] = {
    0xdb, 0xfc, 0xc5, 0xe0, 0xbb, 0xf9, 
};
static const unsigned char gb_chars_ban632 [] = {
    0xdc, 0xa3, 
};
static const unsigned char gb_chars_ban633 [] = {
    0xdc, 0xa5, 
};
static const unsigned char gb_chars_ban634 [] = {
    0xcc, 0xc3, 
};
static const unsigned char gb_chars_ban635 [] = {
    0xb6, 0xd1, 0xdd, 0xc0, 
};
static const unsigned char gb_chars_ban636 [] = {
    0xdc, 0xa1, 
};
static const unsigned char gb_chars_ban637 [] = {
    0xdc, 0xa2, 
};
static const unsigned char gb_chars_ban638 [] = {
    0xc7, 0xb5, 
};
static const unsigned char gb_chars_ban639 [] = {
    0xb6, 0xe9, 
};
static const unsigned char gb_chars_ban640 [] = {
    0xdc, 0xa7, 
};
static const unsigned char gb_chars_ban641 [] = {
    0xdc, 0xa6, 
};
static const unsigned char gb_chars_ban642 [] = {
    0xdc, 0xa9, 0xb1, 0xa4, 
};
static const unsigned char gb_chars_ban643 [] = {
    0xb5, 0xcc, 
};
static const unsigned char gb_chars_ban644 [] = {
    0xbf, 0xb0, 
};
static const unsigned char gb_chars_ban645 [] = {
    0xd1, 0xdf, 
};
static const unsigned char gb_chars_ban646 [] = {
    0xb6, 0xc2, 
};
static const unsigned char gb_chars_ban647 [] = {
    0xdc, 0xa8, 
};
static const unsigned char gb_chars_ban648 [] = {
    0xcb, 0xfa, 0xeb, 0xf3, 
};
static const unsigned char gb_chars_ban649 [] = {
    0xcb, 0xdc, 
};
static const unsigned char gb_chars_ban650 [] = {
    0xcb, 0xfe, 
};
static const unsigned char gb_chars_ban651 [] = {
    0xcc, 0xc1, 
};
static const unsigned char gb_chars_ban652 [] = {
    0xc8, 0xfb, 
};
static const unsigned char gb_chars_ban653 [] = {
    0xdc, 0xaa, 
};
static const unsigned char gb_chars_ban654 [] = {
    0xcc, 0xee, 0xdc, 0xab, 
};
static const unsigned char gb_chars_ban655 [] = {
    0xdb, 0xd3, 
};
static const unsigned char gb_chars_ban656 [] = {
    0xdc, 0xaf, 0xdc, 0xac, 
};
static const unsigned char gb_chars_ban657 [] = {
    0xbe, 0xb3, 
};
static const unsigned char gb_chars_ban658 [] = {
    0xca, 0xfb, 
};
static const unsigned char gb_chars_ban659 [] = {
    0xdc, 0xad, 
};
static const unsigned char gb_chars_ban660 [] = {
    0xc9, 0xca, 0xc4, 0xb9, 
};
static const unsigned char gb_chars_ban661 [] = {
    0xc7, 0xbd, 0xdc, 0xae, 
};
static const unsigned char gb_chars_ban662 [] = {
    0xd4, 0xf6, 0xd0, 0xe6, 
};
static const unsigned char gb_chars_ban663 [] = {
    0xc4, 0xab, 0xb6, 0xd5, 
};
static const unsigned char gb_chars_ban664 [] = {
    0xdb, 0xd4, 
};
static const unsigned char gb_chars_ban665 [] = {
    0xb1, 0xda, 
};
static const unsigned char gb_chars_ban666 [] = {
    0xdb, 0xd5, 
};
static const unsigned char gb_chars_ban667 [] = {
    0xdb, 0xd6, 
};
static const unsigned char gb_chars_ban668 [] = {
    0xba, 0xbe, 
};
static const unsigned char gb_chars_ban669 [] = {
    0xc8, 0xc0, 
};
static const unsigned char gb_chars_ban670 [] = {
    0xca, 0xbf, 0xc8, 0xc9, 
};
static const unsigned char gb_chars_ban671 [] = {
    0xd7, 0xb3, 
};
static const unsigned char gb_chars_ban672 [] = {
    0xc9, 0xf9, 
};
static const unsigned char gb_chars_ban673 [] = {
    0xbf, 0xc7, 
};
static const unsigned char gb_chars_ban674 [] = {
    0xba, 0xf8, 
};
static const unsigned char gb_chars_ban675 [] = {
    0xd2, 0xbc, 
};
static const unsigned char gb_chars_ban676 [] = {
    0xe2, 0xba, 
};
static const unsigned char gb_chars_ban677 [] = {
    0xb4, 0xa6, 
};
static const unsigned char gb_chars_ban678 [] = {
    0xb1, 0xb8, 
};
static const unsigned char gb_chars_ban679 [] = {
    0xb8, 0xb4, 
};
static const unsigned char gb_chars_ban680 [] = {
    0xcf, 0xc4, 
};
static const unsigned char gb_chars_ban681 [] = {
    0xd9, 0xe7, 0xcf, 0xa6, 0xcd, 0xe2, 
};
static const unsigned char gb_chars_ban682 [] = {
    0xd9, 0xed, 0xb6, 0xe0, 
};
static const unsigned char gb_chars_ban683 [] = {
    0xd2, 0xb9, 
};
static const unsigned char gb_chars_ban684 [] = {
    0xb9, 0xbb, 
};
static const unsigned char gb_chars_ban685 [] = {
    0xe2, 0xb9, 0xe2, 0xb7, 
};
static const unsigned char gb_chars_ban686 [] = {
    0xb4, 0xf3, 
};
static const unsigned char gb_chars_ban687 [] = {
    0xcc, 0xec, 0xcc, 0xab, 0xb7, 0xf2, 
};
static const unsigned char gb_chars_ban688 [] = {
    0xd8, 0xb2, 0xd1, 0xeb, 0xba, 0xbb, 
};
static const unsigned char gb_chars_ban689 [] = {
    0xca, 0xa7, 
};
static const unsigned char gb_chars_ban690 [] = {
    0xcd, 0xb7, 
};
static const unsigned char gb_chars_ban691 [] = {
    0xd2, 0xc4, 0xbf, 0xe4, 0xbc, 0xd0, 0xb6, 0xe1, 
};
static const unsigned char gb_chars_ban692 [] = {
    0xde, 0xc5, 
};
static const unsigned char gb_chars_ban693 [] = {
    0xde, 0xc6, 0xdb, 0xbc, 
};
static const unsigned char gb_chars_ban694 [] = {
    0xd1, 0xd9, 
};
static const unsigned char gb_chars_ban695 [] = {
    0xc6, 0xe6, 0xc4, 0xce, 0xb7, 0xee, 
};
static const unsigned char gb_chars_ban696 [] = {
    0xb7, 0xdc, 
};
static const unsigned char gb_chars_ban697 [] = {
    0xbf, 0xfc, 0xd7, 0xe0, 
};
static const unsigned char gb_chars_ban698 [] = {
    0xc6, 0xf5, 
};
static const unsigned char gb_chars_ban699 [] = {
    0xb1, 0xbc, 0xde, 0xc8, 0xbd, 0xb1, 0xcc, 0xd7, 0xde, 0xca, 
};
static const unsigned char gb_chars_ban700 [] = {
    0xde, 0xc9, 
};
static const unsigned char gb_chars_ban701 [] = {
    0xb5, 0xec, 
};
static const unsigned char gb_chars_ban702 [] = {
    0xc9, 0xdd, 
};
static const unsigned char gb_chars_ban703 [] = {
    0xb0, 0xc2, 
};
static const unsigned char gb_chars_ban704 [] = {
    0xc5, 0xae, 0xc5, 0xab, 
};
static const unsigned char gb_chars_ban705 [] = {
    0xc4, 0xcc, 
};
static const unsigned char gb_chars_ban706 [] = {
    0xbc, 0xe9, 0xcb, 0xfd, 
};
static const unsigned char gb_chars_ban707 [] = {
    0xba, 0xc3, 
};
static const unsigned char gb_chars_ban708 [] = {
    0xe5, 0xf9, 0xc8, 0xe7, 0xe5, 0xfa, 0xcd, 0xfd, 
};
static const unsigned char gb_chars_ban709 [] = {
    0xd7, 0xb1, 0xb8, 0xbe, 0xc2, 0xe8, 
};
static const unsigned char gb_chars_ban710 [] = {
    0xc8, 0xd1, 
};
static const unsigned char gb_chars_ban711 [] = {
    0xe5, 0xfb, 
};
static const unsigned char gb_chars_ban712 [] = {
    0xb6, 0xca, 0xbc, 0xcb, 
};
static const unsigned char gb_chars_ban713 [] = {
    0xd1, 0xfd, 0xe6, 0xa1, 
};
static const unsigned char gb_chars_ban714 [] = {
    0xc3, 0xee, 
};
static const unsigned char gb_chars_ban715 [] = {
    0xe6, 0xa4, 
};
static const unsigned char gb_chars_ban716 [] = {
    0xe5, 0xfe, 0xe6, 0xa5, 0xcd, 0xd7, 
};
static const unsigned char gb_chars_ban717 [] = {
    0xb7, 0xc1, 0xe5, 0xfc, 0xe5, 0xfd, 0xe6, 0xa3, 
};
static const unsigned char gb_chars_ban718 [] = {
    0xc4, 0xdd, 0xe6, 0xa8, 
};
static const unsigned char gb_chars_ban719 [] = {
    0xe6, 0xa7, 
};
static const unsigned char gb_chars_ban720 [] = {
    0xc3, 0xc3, 
};
static const unsigned char gb_chars_ban721 [] = {
    0xc6, 0xde, 
};
static const unsigned char gb_chars_ban722 [] = {
    0xe6, 0xaa, 
};
static const unsigned char gb_chars_ban723 [] = {
    0xc4, 0xb7, 
};
static const unsigned char gb_chars_ban724 [] = {
    0xe6, 0xa2, 0xca, 0xbc, 
};
static const unsigned char gb_chars_ban725 [] = {
    0xbd, 0xe3, 0xb9, 0xc3, 0xe6, 0xa6, 0xd0, 0xd5, 0xce, 0xaf, 
};
static const unsigned char gb_chars_ban726 [] = {
    0xe6, 0xa9, 0xe6, 0xb0, 
};
static const unsigned char gb_chars_ban727 [] = {
    0xd2, 0xa6, 
};
static const unsigned char gb_chars_ban728 [] = {
    0xbd, 0xaa, 0xe6, 0xad, 
};
static const unsigned char gb_chars_ban729 [] = {
    0xe6, 0xaf, 
};
static const unsigned char gb_chars_ban730 [] = {
    0xc0, 0xd1, 
};
static const unsigned char gb_chars_ban731 [] = {
    0xd2, 0xcc, 
};
static const unsigned char gb_chars_ban732 [] = {
    0xbc, 0xa7, 
};
static const unsigned char gb_chars_ban733 [] = {
    0xe6, 0xb1, 
};
static const unsigned char gb_chars_ban734 [] = {
    0xd2, 0xf6, 
};
static const unsigned char gb_chars_ban735 [] = {
    0xd7, 0xcb, 
};
static const unsigned char gb_chars_ban736 [] = {
    0xcd, 0xfe, 
};
static const unsigned char gb_chars_ban737 [] = {
    0xcd, 0xde, 0xc2, 0xa6, 0xe6, 0xab, 0xe6, 0xac, 0xbd, 0xbf, 
    0xe6, 0xae, 0xe6, 0xb3, 
};
static const unsigned char gb_chars_ban738 [] = {
    0xe6, 0xb2, 
};
static const unsigned char gb_chars_ban739 [] = {
    0xe6, 0xb6, 
};
static const unsigned char gb_chars_ban740 [] = {
    0xe6, 0xb8, 
};
static const unsigned char gb_chars_ban741 [] = {
    0xc4, 0xef, 
};
static const unsigned char gb_chars_ban742 [] = {
    0xc4, 0xc8, 
};
static const unsigned char gb_chars_ban743 [] = {
    0xbe, 0xea, 0xc9, 0xef, 
};
static const unsigned char gb_chars_ban744 [] = {
    0xe6, 0xb7, 
};
static const unsigned char gb_chars_ban745 [] = {
    0xb6, 0xf0, 
};
static const unsigned char gb_chars_ban746 [] = {
    0xc3, 0xe4, 
};
static const unsigned char gb_chars_ban747 [] = {
    0xd3, 0xe9, 0xe6, 0xb4, 
};
static const unsigned char gb_chars_ban748 [] = {
    0xe6, 0xb5, 
};
static const unsigned char gb_chars_ban749 [] = {
    0xc8, 0xa2, 
};
static const unsigned char gb_chars_ban750 [] = {
    0xe6, 0xbd, 
};
static const unsigned char gb_chars_ban751 [] = {
    0xe6, 0xb9, 
};
static const unsigned char gb_chars_ban752 [] = {
    0xc6, 0xc5, 
};
static const unsigned char gb_chars_ban753 [] = {
    0xcd, 0xf1, 0xe6, 0xbb, 
};
static const unsigned char gb_chars_ban754 [] = {
    0xe6, 0xbc, 
};
static const unsigned char gb_chars_ban755 [] = {
    0xbb, 0xe9, 
};
static const unsigned char gb_chars_ban756 [] = {
    0xe6, 0xbe, 
};
static const unsigned char gb_chars_ban757 [] = {
    0xe6, 0xba, 
};
static const unsigned char gb_chars_ban758 [] = {
    0xc0, 0xb7, 
};
static const unsigned char gb_chars_ban759 [] = {
    0xd3, 0xa4, 0xe6, 0xbf, 0xc9, 0xf4, 0xe6, 0xc3, 
};
static const unsigned char gb_chars_ban760 [] = {
    0xe6, 0xc4, 
};
static const unsigned char gb_chars_ban761 [] = {
    0xd0, 0xf6, 
};
static const unsigned char gb_chars_ban762 [] = {
    0xc3, 0xbd, 
};
static const unsigned char gb_chars_ban763 [] = {
    0xc3, 0xc4, 0xe6, 0xc2, 
};
static const unsigned char gb_chars_ban764 [] = {
    0xe6, 0xc1, 
};
static const unsigned char gb_chars_ban765 [] = {
    0xe6, 0xc7, 0xcf, 0xb1, 
};
static const unsigned char gb_chars_ban766 [] = {
    0xeb, 0xf4, 
};
static const unsigned char gb_chars_ban767 [] = {
    0xe6, 0xca, 
};
static const unsigned char gb_chars_ban768 [] = {
    0xe6, 0xc5, 
};
static const unsigned char gb_chars_ban769 [] = {
    0xbc, 0xde, 0xc9, 0xa9, 
};
static const unsigned char gb_chars_ban770 [] = {
    0xbc, 0xb5, 
};
static const unsigned char gb_chars_ban771 [] = {
    0xcf, 0xd3, 
};
static const unsigned char gb_chars_ban772 [] = {
    0xe6, 0xc8, 
};
static const unsigned char gb_chars_ban773 [] = {
    0xe6, 0xc9, 
};
static const unsigned char gb_chars_ban774 [] = {
    0xe6, 0xce, 
};
static const unsigned char gb_chars_ban775 [] = {
    0xe6, 0xd0, 
};
static const unsigned char gb_chars_ban776 [] = {
    0xe6, 0xd1, 
};
static const unsigned char gb_chars_ban777 [] = {
    0xe6, 0xcb, 0xb5, 0xd5, 
};
static const unsigned char gb_chars_ban778 [] = {
    0xe6, 0xcc, 
};
static const unsigned char gb_chars_ban779 [] = {
    0xe6, 0xcf, 
};
static const unsigned char gb_chars_ban780 [] = {
    0xc4, 0xdb, 
};
static const unsigned char gb_chars_ban781 [] = {
    0xe6, 0xc6, 
};
static const unsigned char gb_chars_ban782 [] = {
    0xe6, 0xcd, 
};
static const unsigned char gb_chars_ban783 [] = {
    0xe6, 0xd2, 
};
static const unsigned char gb_chars_ban784 [] = {
    0xe6, 0xd4, 0xe6, 0xd3, 
};
static const unsigned char gb_chars_ban785 [] = {
    0xe6, 0xd5, 
};
static const unsigned char gb_chars_ban786 [] = {
    0xd9, 0xf8, 
};
static const unsigned char gb_chars_ban787 [] = {
    0xe6, 0xd6, 
};
static const unsigned char gb_chars_ban788 [] = {
    0xe6, 0xd7, 
};
static const unsigned char gb_chars_ban789 [] = {
    0xd7, 0xd3, 0xe6, 0xdd, 
};
static const unsigned char gb_chars_ban790 [] = {
    0xe6, 0xde, 0xbf, 0xd7, 0xd4, 0xd0, 
};
static const unsigned char gb_chars_ban791 [] = {
    0xd7, 0xd6, 0xb4, 0xe6, 0xcb, 0xef, 0xe6, 0xda, 0xd8, 0xc3, 
    0xd7, 0xce, 0xd0, 0xa2, 
};
static const unsigned char gb_chars_ban792 [] = {
    0xc3, 0xcf, 
};
static const unsigned char gb_chars_ban793 [] = {
    0xe6, 0xdf, 0xbc, 0xbe, 0xb9, 0xc2, 0xe6, 0xdb, 0xd1, 0xa7, 
};
static const unsigned char gb_chars_ban794 [] = {
    0xba, 0xa2, 0xc2, 0xcf, 
};
static const unsigned char gb_chars_ban795 [] = {
    0xd8, 0xab, 
};
static const unsigned char gb_chars_ban796 [] = {
    0xca, 0xeb, 0xe5, 0xee, 
};
static const unsigned char gb_chars_ban797 [] = {
    0xe6, 0xdc, 
};
static const unsigned char gb_chars_ban798 [] = {
    0xb7, 0xf5, 
};
static const unsigned char gb_chars_ban799 [] = {
    0xc8, 0xe6, 
};
static const unsigned char gb_chars_ban800 [] = {
    0xc4, 0xf5, 
};
static const unsigned char gb_chars_ban801 [] = {
    0xe5, 0xb2, 0xc4, 0xfe, 
};
static const unsigned char gb_chars_ban802 [] = {
    0xcb, 0xfc, 0xe5, 0xb3, 0xd5, 0xac, 
};
static const unsigned char gb_chars_ban803 [] = {
    0xd3, 0xee, 0xca, 0xd8, 0xb0, 0xb2, 
};
static const unsigned char gb_chars_ban804 [] = {
    0xcb, 0xce, 0xcd, 0xea, 
};
static const unsigned char gb_chars_ban805 [] = {
    0xba, 0xea, 
};
static const unsigned char gb_chars_ban806 [] = {
    0xe5, 0xb5, 
};
static const unsigned char gb_chars_ban807 [] = {
    0xe5, 0xb4, 
};
static const unsigned char gb_chars_ban808 [] = {
    0xd7, 0xda, 0xb9, 0xd9, 0xd6, 0xe6, 0xb6, 0xa8, 0xcd, 0xf0, 
    0xd2, 0xcb, 0xb1, 0xa6, 0xca, 0xb5, 
};
static const unsigned char gb_chars_ban809 [] = {
    0xb3, 0xe8, 0xc9, 0xf3, 0xbf, 0xcd, 0xd0, 0xfb, 0xca, 0xd2, 
    0xe5, 0xb6, 0xbb, 0xc2, 
};
static const unsigned char gb_chars_ban810 [] = {
    0xcf, 0xdc, 0xb9, 0xac, 
};
static const unsigned char gb_chars_ban811 [] = {
    0xd4, 0xd7, 
};
static const unsigned char gb_chars_ban812 [] = {
    0xba, 0xa6, 0xd1, 0xe7, 0xcf, 0xfc, 0xbc, 0xd2, 
};
static const unsigned char gb_chars_ban813 [] = {
    0xe5, 0xb7, 0xc8, 0xdd, 
};
static const unsigned char gb_chars_ban814 [] = {
    0xbf, 0xed, 0xb1, 0xf6, 0xcb, 0xde, 
};
static const unsigned char gb_chars_ban815 [] = {
    0xbc, 0xc5, 
};
static const unsigned char gb_chars_ban816 [] = {
    0xbc, 0xc4, 0xd2, 0xfa, 0xc3, 0xdc, 0xbf, 0xdc, 
};
static const unsigned char gb_chars_ban817 [] = {
    0xb8, 0xbb, 
};
static const unsigned char gb_chars_ban818 [] = {
    0xc3, 0xc2, 
};
static const unsigned char gb_chars_ban819 [] = {
    0xba, 0xae, 0xd4, 0xa2, 
};
static const unsigned char gb_chars_ban820 [] = {
    0xc7, 0xde, 0xc4, 0xaf, 0xb2, 0xec, 
};
static const unsigned char gb_chars_ban821 [] = {
    0xb9, 0xd1, 
};
static const unsigned char gb_chars_ban822 [] = {
    0xe5, 0xbb, 0xc1, 0xc8, 
};
static const unsigned char gb_chars_ban823 [] = {
    0xd5, 0xaf, 
};
static const unsigned char gb_chars_ban824 [] = {
    0xe5, 0xbc, 
};
static const unsigned char gb_chars_ban825 [] = {
    0xe5, 0xbe, 
};
static const unsigned char gb_chars_ban826 [] = {
    0xb4, 0xe7, 0xb6, 0xd4, 0xcb, 0xc2, 0xd1, 0xb0, 0xb5, 0xbc, 
};
static const unsigned char gb_chars_ban827 [] = {
    0xca, 0xd9, 
};
static const unsigned char gb_chars_ban828 [] = {
    0xb7, 0xe2, 
};
static const unsigned char gb_chars_ban829 [] = {
    0xc9, 0xe4, 
};
static const unsigned char gb_chars_ban830 [] = {
    0xbd, 0xab, 
};
static const unsigned char gb_chars_ban831 [] = {
    0xce, 0xbe, 0xd7, 0xf0, 
};
static const unsigned char gb_chars_ban832 [] = {
    0xd0, 0xa1, 
};
static const unsigned char gb_chars_ban833 [] = {
    0xc9, 0xd9, 
};
static const unsigned char gb_chars_ban834 [] = {
    0xb6, 0xfb, 0xe6, 0xd8, 0xbc, 0xe2, 
};
static const unsigned char gb_chars_ban835 [] = {
    0xb3, 0xbe, 
};
static const unsigned char gb_chars_ban836 [] = {
    0xc9, 0xd0, 
};
static const unsigned char gb_chars_ban837 [] = {
    0xe6, 0xd9, 0xb3, 0xa2, 
};
static const unsigned char gb_chars_ban838 [] = {
    0xde, 0xcc, 
};
static const unsigned char gb_chars_ban839 [] = {
    0xd3, 0xc8, 0xde, 0xcd, 
};
static const unsigned char gb_chars_ban840 [] = {
    0xd2, 0xa2, 
};
static const unsigned char gb_chars_ban841 [] = {
    0xde, 0xce, 
};
static const unsigned char gb_chars_ban842 [] = {
    0xbe, 0xcd, 
};
static const unsigned char gb_chars_ban843 [] = {
    0xde, 0xcf, 
};
static const unsigned char gb_chars_ban844 [] = {
    0xca, 0xac, 0xd2, 0xfc, 0xb3, 0xdf, 0xe5, 0xea, 0xc4, 0xe1, 
    0xbe, 0xa1, 0xce, 0xb2, 0xc4, 0xf2, 0xbe, 0xd6, 0xc6, 0xa8, 
    0xb2, 0xe3, 
};
static const unsigned char gb_chars_ban845 [] = {
    0xbe, 0xd3, 
};
static const unsigned char gb_chars_ban846 [] = {
    0xc7, 0xfc, 0xcc, 0xeb, 0xbd, 0xec, 0xce, 0xdd, 
};
static const unsigned char gb_chars_ban847 [] = {
    0xca, 0xba, 0xc6, 0xc1, 0xe5, 0xec, 0xd0, 0xbc, 
};
static const unsigned char gb_chars_ban848 [] = {
    0xd5, 0xb9, 
};
static const unsigned char gb_chars_ban849 [] = {
    0xe5, 0xed, 
};
static const unsigned char gb_chars_ban850 [] = {
    0xca, 0xf4, 
};
static const unsigned char gb_chars_ban851 [] = {
    0xcd, 0xc0, 0xc2, 0xc5, 
};
static const unsigned char gb_chars_ban852 [] = {
    0xe5, 0xef, 
};
static const unsigned char gb_chars_ban853 [] = {
    0xc2, 0xc4, 0xe5, 0xf0, 
};
static const unsigned char gb_chars_ban854 [] = {
    0xe5, 0xf8, 0xcd, 0xcd, 
};
static const unsigned char gb_chars_ban855 [] = {
    0xc9, 0xbd, 
};
static const unsigned char gb_chars_ban856 [] = {
    0xd2, 0xd9, 0xe1, 0xa8, 
};
static const unsigned char gb_chars_ban857 [] = {
    0xd3, 0xec, 
};
static const unsigned char gb_chars_ban858 [] = {
    0xcb, 0xea, 0xc6, 0xf1, 
};
static const unsigned char gb_chars_ban859 [] = {
    0xe1, 0xac, 
};
static const unsigned char gb_chars_ban860 [] = {
    0xe1, 0xa7, 0xe1, 0xa9, 
};
static const unsigned char gb_chars_ban861 [] = {
    0xe1, 0xaa, 0xe1, 0xaf, 
};
static const unsigned char gb_chars_ban862 [] = {
    0xb2, 0xed, 
};
static const unsigned char gb_chars_ban863 [] = {
    0xe1, 0xab, 0xb8, 0xda, 0xe1, 0xad, 0xe1, 0xae, 0xe1, 0xb0, 
    0xb5, 0xba, 0xe1, 0xb1, 
};
static const unsigned char gb_chars_ban864 [] = {
    0xe1, 0xb3, 0xe1, 0xb8, 
};
static const unsigned char gb_chars_ban865 [] = {
    0xd1, 0xd2, 
};
static const unsigned char gb_chars_ban866 [] = {
    0xe1, 0xb6, 0xe1, 0xb5, 0xc1, 0xeb, 
};
static const unsigned char gb_chars_ban867 [] = {
    0xe1, 0xb7, 
};
static const unsigned char gb_chars_ban868 [] = {
    0xd4, 0xc0, 
};
static const unsigned char gb_chars_ban869 [] = {
    0xe1, 0xb2, 
};
static const unsigned char gb_chars_ban870 [] = {
    0xe1, 0xba, 0xb0, 0xb6, 
};
static const unsigned char gb_chars_ban871 [] = {
    0xe1, 0xb4, 
};
static const unsigned char gb_chars_ban872 [] = {
    0xbf, 0xf9, 
};
static const unsigned char gb_chars_ban873 [] = {
    0xe1, 0xb9, 
};
static const unsigned char gb_chars_ban874 [] = {
    0xe1, 0xbb, 
};
static const unsigned char gb_chars_ban875 [] = {
    0xe1, 0xbe, 
};
static const unsigned char gb_chars_ban876 [] = {
    0xe1, 0xbc, 
};
static const unsigned char gb_chars_ban877 [] = {
    0xd6, 0xc5, 
};
static const unsigned char gb_chars_ban878 [] = {
    0xcf, 0xbf, 
};
static const unsigned char gb_chars_ban879 [] = {
    0xe1, 0xbd, 0xe1, 0xbf, 0xc2, 0xcd, 
};
static const unsigned char gb_chars_ban880 [] = {
    0xb6, 0xeb, 
};
static const unsigned char gb_chars_ban881 [] = {
    0xd3, 0xf8, 
};
static const unsigned char gb_chars_ban882 [] = {
    0xc7, 0xcd, 
};
static const unsigned char gb_chars_ban883 [] = {
    0xb7, 0xe5, 
};
static const unsigned char gb_chars_ban884 [] = {
    0xbe, 0xfe, 
};
static const unsigned char gb_chars_ban885 [] = {
    0xe1, 0xc0, 0xe1, 0xc1, 
};
static const unsigned char gb_chars_ban886 [] = {
    0xe1, 0xc7, 0xb3, 0xe7, 
};
static const unsigned char gb_chars_ban887 [] = {
    0xc6, 0xe9, 
};
static const unsigned char gb_chars_ban888 [] = {
    0xb4, 0xde, 
};
static const unsigned char gb_chars_ban889 [] = {
    0xd1, 0xc2, 
};
static const unsigned char gb_chars_ban890 [] = {
    0xe1, 0xc8, 
};
static const unsigned char gb_chars_ban891 [] = {
    0xe1, 0xc6, 
};
static const unsigned char gb_chars_ban892 [] = {
    0xe1, 0xc5, 
};
static const unsigned char gb_chars_ban893 [] = {
    0xe1, 0xc3, 0xe1, 0xc2, 
};
static const unsigned char gb_chars_ban894 [] = {
    0xb1, 0xc0, 
};
static const unsigned char gb_chars_ban895 [] = {
    0xd5, 0xb8, 0xe1, 0xc4, 
};
static const unsigned char gb_chars_ban896 [] = {
    0xe1, 0xcb, 
};
static const unsigned char gb_chars_ban897 [] = {
    0xe1, 0xcc, 0xe1, 0xca, 
};
static const unsigned char gb_chars_ban898 [] = {
    0xef, 0xfa, 
};
static const unsigned char gb_chars_ban899 [] = {
    0xe1, 0xd3, 0xe1, 0xd2, 0xc7, 0xb6, 
};
static const unsigned char gb_chars_ban900 [] = {
    0xe1, 0xc9, 
};
static const unsigned char gb_chars_ban901 [] = {
    0xe1, 0xce, 
};
static const unsigned char gb_chars_ban902 [] = {
    0xe1, 0xd0, 
};
static const unsigned char gb_chars_ban903 [] = {
    0xe1, 0xd4, 
};
static const unsigned char gb_chars_ban904 [] = {
    0xe1, 0xd1, 0xe1, 0xcd, 
};
static const unsigned char gb_chars_ban905 [] = {
    0xe1, 0xcf, 
};
static const unsigned char gb_chars_ban906 [] = {
    0xe1, 0xd5, 
};
static const unsigned char gb_chars_ban907 [] = {
    0xe1, 0xd6, 
};
static const unsigned char gb_chars_ban908 [] = {
    0xe1, 0xd7, 
};
static const unsigned char gb_chars_ban909 [] = {
    0xe1, 0xd8, 
};
static const unsigned char gb_chars_ban910 [] = {
    0xe1, 0xda, 
};
static const unsigned char gb_chars_ban911 [] = {
    0xe1, 0xdb, 
};
static const unsigned char gb_chars_ban912 [] = {
    0xce, 0xa1, 
};
static const unsigned char gb_chars_ban913 [] = {
    0xe7, 0xdd, 
};
static const unsigned char gb_chars_ban914 [] = {
    0xb4, 0xa8, 0xd6, 0xdd, 
};
static const unsigned char gb_chars_ban915 [] = {
    0xd1, 0xb2, 0xb3, 0xb2, 
};
static const unsigned char gb_chars_ban916 [] = {
    0xb9, 0xa4, 0xd7, 0xf3, 0xc7, 0xc9, 0xbe, 0xde, 0xb9, 0xae, 
};
static const unsigned char gb_chars_ban917 [] = {
    0xce, 0xd7, 
};
static const unsigned char gb_chars_ban918 [] = {
    0xb2, 0xee, 0xdb, 0xcf, 
};
static const unsigned char gb_chars_ban919 [] = {
    0xbc, 0xba, 0xd2, 0xd1, 0xcb, 0xc8, 0xb0, 0xcd, 
};
static const unsigned char gb_chars_ban920 [] = {
    0xcf, 0xef, 
};
static const unsigned char gb_chars_ban921 [] = {
    0xd9, 0xe3, 0xbd, 0xed, 
};
static const unsigned char gb_chars_ban922 [] = {
    0xb1, 0xd2, 0xca, 0xd0, 0xb2, 0xbc, 
};
static const unsigned char gb_chars_ban923 [] = {
    0xcb, 0xa7, 0xb7, 0xab, 
};
static const unsigned char gb_chars_ban924 [] = {
    0xca, 0xa6, 
};
static const unsigned char gb_chars_ban925 [] = {
    0xcf, 0xa3, 
};
static const unsigned char gb_chars_ban926 [] = {
    0xe0, 0xf8, 0xd5, 0xca, 0xe0, 0xfb, 
};
static const unsigned char gb_chars_ban927 [] = {
    0xe0, 0xfa, 0xc5, 0xc1, 0xcc, 0xfb, 
};
static const unsigned char gb_chars_ban928 [] = {
    0xc1, 0xb1, 0xe0, 0xf9, 0xd6, 0xe3, 0xb2, 0xaf, 0xd6, 0xc4, 
    0xb5, 0xdb, 
};
static const unsigned char gb_chars_ban929 [] = {
    0xb4, 0xf8, 0xd6, 0xa1, 
};
static const unsigned char gb_chars_ban930 [] = {
    0xcf, 0xaf, 0xb0, 0xef, 
};
static const unsigned char gb_chars_ban931 [] = {
    0xe0, 0xfc, 
};
static const unsigned char gb_chars_ban932 [] = {
    0xe1, 0xa1, 0xb3, 0xa3, 
};
static const unsigned char gb_chars_ban933 [] = {
    0xe0, 0xfd, 0xe0, 0xfe, 0xc3, 0xb1, 
};
static const unsigned char gb_chars_ban934 [] = {
    0xc3, 0xdd, 
};
static const unsigned char gb_chars_ban935 [] = {
    0xe1, 0xa2, 0xb7, 0xf9, 
};
static const unsigned char gb_chars_ban936 [] = {
    0xbb, 0xcf, 
};
static const unsigned char gb_chars_ban937 [] = {
    0xe1, 0xa3, 0xc4, 0xbb, 
};
static const unsigned char gb_chars_ban938 [] = {
    0xe1, 0xa4, 
};
static const unsigned char gb_chars_ban939 [] = {
    0xe1, 0xa5, 
};
static const unsigned char gb_chars_ban940 [] = {
    0xe1, 0xa6, 0xb4, 0xb1, 
};
static const unsigned char gb_chars_ban941 [] = {
    0xb8, 0xc9, 0xc6, 0xbd, 0xc4, 0xea, 
};
static const unsigned char gb_chars_ban942 [] = {
    0xb2, 0xa2, 
};
static const unsigned char gb_chars_ban943 [] = {
    0xd0, 0xd2, 
};
static const unsigned char gb_chars_ban944 [] = {
    0xe7, 0xdb, 0xbb, 0xc3, 0xd3, 0xd7, 0xd3, 0xc4, 
};
static const unsigned char gb_chars_ban945 [] = {
    0xb9, 0xe3, 0xe2, 0xcf, 
};
static const unsigned char gb_chars_ban946 [] = {
    0xd7, 0xaf, 
};
static const unsigned char gb_chars_ban947 [] = {
    0xc7, 0xec, 0xb1, 0xd3, 
};
static const unsigned char gb_chars_ban948 [] = {
    0xb4, 0xb2, 0xe2, 0xd1, 
};
static const unsigned char gb_chars_ban949 [] = {
    0xd0, 0xf2, 0xc2, 0xae, 0xe2, 0xd0, 
};
static const unsigned char gb_chars_ban950 [] = {
    0xbf, 0xe2, 0xd3, 0xa6, 0xb5, 0xd7, 0xe2, 0xd2, 0xb5, 0xea, 
};
static const unsigned char gb_chars_ban951 [] = {
    0xc3, 0xed, 0xb8, 0xfd, 
};
static const unsigned char gb_chars_ban952 [] = {
    0xb8, 0xae, 
};
static const unsigned char gb_chars_ban953 [] = {
    0xc5, 0xd3, 0xb7, 0xcf, 0xe2, 0xd4, 
};
static const unsigned char gb_chars_ban954 [] = {
    0xe2, 0xd3, 0xb6, 0xc8, 0xd7, 0xf9, 
};
static const unsigned char gb_chars_ban955 [] = {
    0xcd, 0xa5, 
};
static const unsigned char gb_chars_ban956 [] = {
    0xe2, 0xd8, 
};
static const unsigned char gb_chars_ban957 [] = {
    0xe2, 0xd6, 0xca, 0xfc, 0xbf, 0xb5, 0xd3, 0xb9, 0xe2, 0xd5, 
};
static const unsigned char gb_chars_ban958 [] = {
    0xe2, 0xd7, 
};
static const unsigned char gb_chars_ban959 [] = {
    0xc1, 0xae, 0xc0, 0xc8, 
};
static const unsigned char gb_chars_ban960 [] = {
    0xe2, 0xdb, 0xe2, 0xda, 0xc0, 0xaa, 
};
static const unsigned char gb_chars_ban961 [] = {
    0xc1, 0xce, 
};
static const unsigned char gb_chars_ban962 [] = {
    0xe2, 0xdc, 
};
static const unsigned char gb_chars_ban963 [] = {
    0xe2, 0xdd, 
};
static const unsigned char gb_chars_ban964 [] = {
    0xe2, 0xde, 
};
static const unsigned char gb_chars_ban965 [] = {
    0xdb, 0xc8, 
};
static const unsigned char gb_chars_ban966 [] = {
    0xd1, 0xd3, 0xcd, 0xa2, 
};
static const unsigned char gb_chars_ban967 [] = {
    0xbd, 0xa8, 
};
static const unsigned char gb_chars_ban968 [] = {
    0xde, 0xc3, 0xd8, 0xa5, 0xbf, 0xaa, 0xdb, 0xcd, 0xd2, 0xec, 
    0xc6, 0xfa, 0xc5, 0xaa, 
};
static const unsigned char gb_chars_ban969 [] = {
    0xde, 0xc4, 
};
static const unsigned char gb_chars_ban970 [] = {
    0xb1, 0xd7, 0xdf, 0xae, 
};
static const unsigned char gb_chars_ban971 [] = {
    0xca, 0xbd, 
};
static const unsigned char gb_chars_ban972 [] = {
    0xdf, 0xb1, 
};
static const unsigned char gb_chars_ban973 [] = {
    0xb9, 0xad, 
};
static const unsigned char gb_chars_ban974 [] = {
    0xd2, 0xfd, 
};
static const unsigned char gb_chars_ban975 [] = {
    0xb8, 0xa5, 0xba, 0xeb, 
};
static const unsigned char gb_chars_ban976 [] = {
    0xb3, 0xda, 
};
static const unsigned char gb_chars_ban977 [] = {
    0xb5, 0xdc, 0xd5, 0xc5, 
};
static const unsigned char gb_chars_ban978 [] = {
    0xc3, 0xd6, 0xcf, 0xd2, 0xbb, 0xa1, 
};
static const unsigned char gb_chars_ban979 [] = {
    0xe5, 0xf3, 0xe5, 0xf2, 
};
static const unsigned char gb_chars_ban980 [] = {
    0xe5, 0xf4, 
};
static const unsigned char gb_chars_ban981 [] = {
    0xcd, 0xe4, 
};
static const unsigned char gb_chars_ban982 [] = {
    0xc8, 0xf5, 
};
static const unsigned char gb_chars_ban983 [] = {
    0xb5, 0xaf, 0xc7, 0xbf, 
};
static const unsigned char gb_chars_ban984 [] = {
    0xe5, 0xf6, 
};
static const unsigned char gb_chars_ban985 [] = {
    0xec, 0xb0, 
};
static const unsigned char gb_chars_ban986 [] = {
    0xe5, 0xe6, 
};
static const unsigned char gb_chars_ban987 [] = {
    0xb9, 0xe9, 0xb5, 0xb1, 
};
static const unsigned char gb_chars_ban988 [] = {
    0xc2, 0xbc, 0xe5, 0xe8, 0xe5, 0xe7, 0xe5, 0xe9, 
};
static const unsigned char gb_chars_ban989 [] = {
    0xd2, 0xcd, 
};
static const unsigned char gb_chars_ban990 [] = {
    0xe1, 0xea, 0xd0, 0xce, 
};
static const unsigned char gb_chars_ban991 [] = {
    0xcd, 0xae, 
};
static const unsigned char gb_chars_ban992 [] = {
    0xd1, 0xe5, 
};
static const unsigned char gb_chars_ban993 [] = {
    0xb2, 0xca, 0xb1, 0xeb, 
};
static const unsigned char gb_chars_ban994 [] = {
    0xb1, 0xf2, 0xc5, 0xed, 
};
static const unsigned char gb_chars_ban995 [] = {
    0xd5, 0xc3, 0xd3, 0xb0, 
};
static const unsigned char gb_chars_ban996 [] = {
    0xe1, 0xdc, 
};
static const unsigned char gb_chars_ban997 [] = {
    0xe1, 0xdd, 
};
static const unsigned char gb_chars_ban998 [] = {
    0xd2, 0xdb, 
};
static const unsigned char gb_chars_ban999 [] = {
    0xb3, 0xb9, 0xb1, 0xcb, 
};
static const unsigned char gb_chars_ban1000 [] = {
    0xcd, 0xf9, 0xd5, 0xf7, 0xe1, 0xde, 
};
static const unsigned char gb_chars_ban1001 [] = {
    0xbe, 0xb6, 0xb4, 0xfd, 
};
static const unsigned char gb_chars_ban1002 [] = {
    0xe1, 0xdf, 0xba, 0xdc, 0xe1, 0xe0, 0xbb, 0xb2, 0xc2, 0xc9, 
    0xe1, 0xe1, 
};
static const unsigned char gb_chars_ban1003 [] = {
    0xd0, 0xec, 
};
static const unsigned char gb_chars_ban1004 [] = {
    0xcd, 0xbd, 
};
static const unsigned char gb_chars_ban1005 [] = {
    0xe1, 0xe2, 
};
static const unsigned char gb_chars_ban1006 [] = {
    0xb5, 0xc3, 0xc5, 0xc7, 0xe1, 0xe3, 
};
static const unsigned char gb_chars_ban1007 [] = {
    0xe1, 0xe4, 
};
static const unsigned char gb_chars_ban1008 [] = {
    0xd3, 0xf9, 
};
static const unsigned char gb_chars_ban1009 [] = {
    0xe1, 0xe5, 
};
static const unsigned char gb_chars_ban1010 [] = {
    0xd1, 0xad, 
};
static const unsigned char gb_chars_ban1011 [] = {
    0xe1, 0xe6, 0xce, 0xa2, 
};
static const unsigned char gb_chars_ban1012 [] = {
    0xe1, 0xe7, 
};
static const unsigned char gb_chars_ban1013 [] = {
    0xb5, 0xc2, 
};
static const unsigned char gb_chars_ban1014 [] = {
    0xe1, 0xe8, 0xbb, 0xd5, 
};
static const unsigned char gb_chars_ban1015 [] = {
    0xd0, 0xc4, 0xe2, 0xe0, 0xb1, 0xd8, 0xd2, 0xe4, 
};
static const unsigned char gb_chars_ban1016 [] = {
    0xe2, 0xe1, 
};
static const unsigned char gb_chars_ban1017 [] = {
    0xbc, 0xc9, 0xc8, 0xcc, 
};
static const unsigned char gb_chars_ban1018 [] = {
    0xe2, 0xe3, 0xec, 0xfe, 0xec, 0xfd, 0xdf, 0xaf, 
};
static const unsigned char gb_chars_ban1019 [] = {
    0xe2, 0xe2, 0xd6, 0xbe, 0xcd, 0xfc, 0xc3, 0xa6, 
};
static const unsigned char gb_chars_ban1020 [] = {
    0xe3, 0xc3, 
};
static const unsigned char gb_chars_ban1021 [] = {
    0xd6, 0xd2, 0xe2, 0xe7, 
};
static const unsigned char gb_chars_ban1022 [] = {
    0xe2, 0xe8, 
};
static const unsigned char gb_chars_ban1023 [] = {
    0xd3, 0xc7, 
};
static const unsigned char gb_chars_ban1024 [] = {
    0xe2, 0xec, 0xbf, 0xec, 
};
static const unsigned char gb_chars_ban1025 [] = {
    0xe2, 0xed, 0xe2, 0xe5, 
};
static const unsigned char gb_chars_ban1026 [] = {
    0xb3, 0xc0, 
};
static const unsigned char gb_chars_ban1027 [] = {
    0xc4, 0xee, 
};
static const unsigned char gb_chars_ban1028 [] = {
    0xe2, 0xee, 
};
static const unsigned char gb_chars_ban1029 [] = {
    0xd0, 0xc3, 
};
static const unsigned char gb_chars_ban1030 [] = {
    0xba, 0xf6, 0xe2, 0xe9, 0xb7, 0xde, 0xbb, 0xb3, 0xcc, 0xac, 
    0xcb, 0xcb, 0xe2, 0xe4, 0xe2, 0xe6, 0xe2, 0xea, 0xe2, 0xeb, 
};
static const unsigned char gb_chars_ban1031 [] = {
    0xe2, 0xf7, 
};
static const unsigned char gb_chars_ban1032 [] = {
    0xe2, 0xf4, 0xd4, 0xf5, 0xe2, 0xf3, 
};
static const unsigned char gb_chars_ban1033 [] = {
    0xc5, 0xad, 
};
static const unsigned char gb_chars_ban1034 [] = {
    0xd5, 0xfa, 0xc5, 0xc2, 0xb2, 0xc0, 
};
static const unsigned char gb_chars_ban1035 [] = {
    0xe2, 0xef, 
};
static const unsigned char gb_chars_ban1036 [] = {
    0xe2, 0xf2, 0xc1, 0xaf, 0xcb, 0xbc, 
};
static const unsigned char gb_chars_ban1037 [] = {
    0xb5, 0xa1, 0xe2, 0xf9, 
};
static const unsigned char gb_chars_ban1038 [] = {
    0xbc, 0xb1, 0xe2, 0xf1, 0xd0, 0xd4, 0xd4, 0xb9, 0xe2, 0xf5, 
    0xb9, 0xd6, 0xe2, 0xf6, 
};
static const unsigned char gb_chars_ban1039 [] = {
    0xc7, 0xd3, 
};
static const unsigned char gb_chars_ban1040 [] = {
    0xe2, 0xf0, 
};
static const unsigned char gb_chars_ban1041 [] = {
    0xd7, 0xdc, 0xed, 0xa1, 
};
static const unsigned char gb_chars_ban1042 [] = {
    0xe2, 0xf8, 
};
static const unsigned char gb_chars_ban1043 [] = {
    0xed, 0xa5, 0xe2, 0xfe, 0xca, 0xd1, 
};
static const unsigned char gb_chars_ban1044 [] = {
    0xc1, 0xb5, 
};
static const unsigned char gb_chars_ban1045 [] = {
    0xbb, 0xd0, 
};
static const unsigned char gb_chars_ban1046 [] = {
    0xbf, 0xd6, 
};
static const unsigned char gb_chars_ban1047 [] = {
    0xba, 0xe3, 
};
static const unsigned char gb_chars_ban1048 [] = {
    0xcb, 0xa1, 
};
static const unsigned char gb_chars_ban1049 [] = {
    0xed, 0xa6, 0xed, 0xa3, 
};
static const unsigned char gb_chars_ban1050 [] = {
    0xed, 0xa2, 
};
static const unsigned char gb_chars_ban1051 [] = {
    0xbb, 0xd6, 0xed, 0xa7, 0xd0, 0xf4, 
};
static const unsigned char gb_chars_ban1052 [] = {
    0xed, 0xa4, 0xba, 0xde, 0xb6, 0xf7, 0xe3, 0xa1, 0xb6, 0xb2, 
    0xcc, 0xf1, 0xb9, 0xa7, 
};
static const unsigned char gb_chars_ban1053 [] = {
    0xcf, 0xa2, 0xc7, 0xa1, 
};
static const unsigned char gb_chars_ban1054 [] = {
    0xbf, 0xd2, 
};
static const unsigned char gb_chars_ban1055 [] = {
    0xb6, 0xf1, 
};
static const unsigned char gb_chars_ban1056 [] = {
    0xe2, 0xfa, 0xe2, 0xfb, 0xe2, 0xfd, 0xe2, 0xfc, 0xc4, 0xd5, 
    0xe3, 0xa2, 
};
static const unsigned char gb_chars_ban1057 [] = {
    0xd3, 0xc1, 
};
static const unsigned char gb_chars_ban1058 [] = {
    0xe3, 0xa7, 0xc7, 0xc4, 
};
static const unsigned char gb_chars_ban1059 [] = {
    0xcf, 0xa4, 
};
static const unsigned char gb_chars_ban1060 [] = {
    0xe3, 0xa9, 0xba, 0xb7, 
};
static const unsigned char gb_chars_ban1061 [] = {
    0xe3, 0xa8, 
};
static const unsigned char gb_chars_ban1062 [] = {
    0xbb, 0xda, 
};
static const unsigned char gb_chars_ban1063 [] = {
    0xe3, 0xa3, 
};
static const unsigned char gb_chars_ban1064 [] = {
    0xe3, 0xa4, 0xe3, 0xaa, 
};
static const unsigned char gb_chars_ban1065 [] = {
    0xe3, 0xa6, 
};
static const unsigned char gb_chars_ban1066 [] = {
    0xce, 0xf2, 0xd3, 0xc6, 
};
static const unsigned char gb_chars_ban1067 [] = {
    0xbb, 0xbc, 
};
static const unsigned char gb_chars_ban1068 [] = {
    0xd4, 0xc3, 
};
static const unsigned char gb_chars_ban1069 [] = {
    0xc4, 0xfa, 
};
static const unsigned char gb_chars_ban1070 [] = {
    0xed, 0xa8, 0xd0, 0xfc, 0xe3, 0xa5, 
};
static const unsigned char gb_chars_ban1071 [] = {
    0xc3, 0xf5, 
};
static const unsigned char gb_chars_ban1072 [] = {
    0xe3, 0xad, 0xb1, 0xaf, 
};
static const unsigned char gb_chars_ban1073 [] = {
    0xe3, 0xb2, 
};
static const unsigned char gb_chars_ban1074 [] = {
    0xbc, 0xc2, 
};
static const unsigned char gb_chars_ban1075 [] = {
    0xe3, 0xac, 0xb5, 0xbf, 
};
static const unsigned char gb_chars_ban1076 [] = {
    0xc7, 0xe9, 0xe3, 0xb0, 
};
static const unsigned char gb_chars_ban1077 [] = {
    0xbe, 0xaa, 0xcd, 0xef, 
};
static const unsigned char gb_chars_ban1078 [] = {
    0xbb, 0xf3, 
};
static const unsigned char gb_chars_ban1079 [] = {
    0xcc, 0xe8, 
};
static const unsigned char gb_chars_ban1080 [] = {
    0xe3, 0xaf, 
};
static const unsigned char gb_chars_ban1081 [] = {
    0xe3, 0xb1, 
};
static const unsigned char gb_chars_ban1082 [] = {
    0xcf, 0xa7, 0xe3, 0xae, 
};
static const unsigned char gb_chars_ban1083 [] = {
    0xce, 0xa9, 0xbb, 0xdd, 
};
static const unsigned char gb_chars_ban1084 [] = {
    0xb5, 0xeb, 0xbe, 0xe5, 0xb2, 0xd2, 0xb3, 0xcd, 
};
static const unsigned char gb_chars_ban1085 [] = {
    0xb1, 0xb9, 0xe3, 0xab, 0xb2, 0xd1, 0xb5, 0xac, 0xb9, 0xdf, 
    0xb6, 0xe8, 
};
static const unsigned char gb_chars_ban1086 [] = {
    0xcf, 0xeb, 0xe3, 0xb7, 
};
static const unsigned char gb_chars_ban1087 [] = {
    0xbb, 0xcc, 
};
static const unsigned char gb_chars_ban1088 [] = {
    0xc8, 0xc7, 0xd0, 0xca, 
};
static const unsigned char gb_chars_ban1089 [] = {
    0xe3, 0xb8, 0xb3, 0xee, 
};
static const unsigned char gb_chars_ban1090 [] = {
    0xed, 0xa9, 
};
static const unsigned char gb_chars_ban1091 [] = {
    0xd3, 0xfa, 0xd3, 0xe4, 
};
static const unsigned char gb_chars_ban1092 [] = {
    0xed, 0xaa, 0xe3, 0xb9, 0xd2, 0xe2, 
};
static const unsigned char gb_chars_ban1093 [] = {
    0xe3, 0xb5, 
};
static const unsigned char gb_chars_ban1094 [] = {
    0xd3, 0xde, 
};
static const unsigned char gb_chars_ban1095 [] = {
    0xb8, 0xd0, 0xe3, 0xb3, 
};
static const unsigned char gb_chars_ban1096 [] = {
    0xe3, 0xb6, 0xb7, 0xdf, 
};
static const unsigned char gb_chars_ban1097 [] = {
    0xe3, 0xb4, 0xc0, 0xa2, 
};
static const unsigned char gb_chars_ban1098 [] = {
    0xe3, 0xba, 
};
static const unsigned char gb_chars_ban1099 [] = {
    0xd4, 0xb8, 
};
static const unsigned char gb_chars_ban1100 [] = {
    0xb4, 0xc8, 
};
static const unsigned char gb_chars_ban1101 [] = {
    0xe3, 0xbb, 
};
static const unsigned char gb_chars_ban1102 [] = {
    0xbb, 0xc5, 
};
static const unsigned char gb_chars_ban1103 [] = {
    0xc9, 0xf7, 
};
static const unsigned char gb_chars_ban1104 [] = {
    0xc9, 0xe5, 
};
static const unsigned char gb_chars_ban1105 [] = {
    0xc4, 0xbd, 
};
static const unsigned char gb_chars_ban1106 [] = {
    0xed, 0xab, 
};
static const unsigned char gb_chars_ban1107 [] = {
    0xc2, 0xfd, 
};
static const unsigned char gb_chars_ban1108 [] = {
    0xbb, 0xdb, 0xbf, 0xae, 
};
static const unsigned char gb_chars_ban1109 [] = {
    0xce, 0xbf, 
};
static const unsigned char gb_chars_ban1110 [] = {
    0xe3, 0xbc, 
};
static const unsigned char gb_chars_ban1111 [] = {
    0xbf, 0xb6, 
};
static const unsigned char gb_chars_ban1112 [] = {
    0xb1, 0xef, 
};
static const unsigned char gb_chars_ban1113 [] = {
    0xd4, 0xf7, 
};
static const unsigned char gb_chars_ban1114 [] = {
    0xe3, 0xbe, 
};
static const unsigned char gb_chars_ban1115 [] = {
    0xed, 0xad, 
};
static const unsigned char gb_chars_ban1116 [] = {
    0xe3, 0xbf, 0xba, 0xa9, 0xed, 0xac, 
};
static const unsigned char gb_chars_ban1117 [] = {
    0xe3, 0xbd, 
};
static const unsigned char gb_chars_ban1118 [] = {
    0xe3, 0xc0, 
};
static const unsigned char gb_chars_ban1119 [] = {
    0xba, 0xb6, 
};
static const unsigned char gb_chars_ban1120 [] = {
    0xb6, 0xae, 
};
static const unsigned char gb_chars_ban1121 [] = {
    0xd0, 0xb8, 
};
static const unsigned char gb_chars_ban1122 [] = {
    0xb0, 0xc3, 0xed, 0xae, 
};
static const unsigned char gb_chars_ban1123 [] = {
    0xed, 0xaf, 0xc0, 0xc1, 
};
static const unsigned char gb_chars_ban1124 [] = {
    0xe3, 0xc1, 
};
static const unsigned char gb_chars_ban1125 [] = {
    0xc5, 0xb3, 
};
static const unsigned char gb_chars_ban1126 [] = {
    0xe3, 0xc2, 
};
static const unsigned char gb_chars_ban1127 [] = {
    0xdc, 0xb2, 
};
static const unsigned char gb_chars_ban1128 [] = {
    0xed, 0xb0, 
};
static const unsigned char gb_chars_ban1129 [] = {
    0xb8, 0xea, 
};
static const unsigned char gb_chars_ban1130 [] = {
    0xce, 0xec, 0xea, 0xa7, 0xd0, 0xe7, 0xca, 0xf9, 0xc8, 0xd6, 
    0xcf, 0xb7, 0xb3, 0xc9, 0xce, 0xd2, 0xbd, 0xe4, 
};
static const unsigned char gb_chars_ban1131 [] = {
    0xe3, 0xde, 0xbb, 0xf2, 0xea, 0xa8, 0xd5, 0xbd, 
};
static const unsigned char gb_chars_ban1132 [] = {
    0xc6, 0xdd, 0xea, 0xa9, 
};
static const unsigned char gb_chars_ban1133 [] = {
    0xea, 0xaa, 
};
static const unsigned char gb_chars_ban1134 [] = {
    0xea, 0xac, 0xea, 0xab, 
};
static const unsigned char gb_chars_ban1135 [] = {
    0xea, 0xae, 0xea, 0xad, 
};
static const unsigned char gb_chars_ban1136 [] = {
    0xbd, 0xd8, 
};
static const unsigned char gb_chars_ban1137 [] = {
    0xea, 0xaf, 
};
static const unsigned char gb_chars_ban1138 [] = {
    0xc2, 0xbe, 
};
static const unsigned char gb_chars_ban1139 [] = {
    0xb4, 0xc1, 0xb4, 0xf7, 
};
static const unsigned char gb_chars_ban1140 [] = {
    0xbb, 0xa7, 
};
static const unsigned char gb_chars_ban1141 [] = {
    0xec, 0xe6, 0xec, 0xe5, 0xb7, 0xbf, 0xcb, 0xf9, 0xb1, 0xe2, 
};
static const unsigned char gb_chars_ban1142 [] = {
    0xec, 0xe7, 
};
static const unsigned char gb_chars_ban1143 [] = {
    0xc9, 0xc8, 0xec, 0xe8, 0xec, 0xe9, 
};
static const unsigned char gb_chars_ban1144 [] = {
    0xca, 0xd6, 0xde, 0xd0, 0xb2, 0xc5, 0xd4, 0xfa, 
};
static const unsigned char gb_chars_ban1145 [] = {
    0xc6, 0xcb, 0xb0, 0xc7, 0xb4, 0xf2, 0xc8, 0xd3, 
};
static const unsigned char gb_chars_ban1146 [] = {
    0xcd, 0xd0, 
};
static const unsigned char gb_chars_ban1147 [] = {
    0xbf, 0xb8, 
};
static const unsigned char gb_chars_ban1148 [] = {
    0xbf, 0xdb, 
};
static const unsigned char gb_chars_ban1149 [] = {
    0xc7, 0xa4, 0xd6, 0xb4, 
};
static const unsigned char gb_chars_ban1150 [] = {
    0xc0, 0xa9, 0xde, 0xd1, 0xc9, 0xa8, 0xd1, 0xef, 0xc5, 0xa4, 
    0xb0, 0xe7, 0xb3, 0xb6, 0xc8, 0xc5, 
};
static const unsigned char gb_chars_ban1151 [] = {
    0xb0, 0xe2, 
};
static const unsigned char gb_chars_ban1152 [] = {
    0xb7, 0xf6, 
};
static const unsigned char gb_chars_ban1153 [] = {
    0xc5, 0xfa, 
};
static const unsigned char gb_chars_ban1154 [] = {
    0xb6, 0xf3, 
};
static const unsigned char gb_chars_ban1155 [] = {
    0xd5, 0xd2, 0xb3, 0xd0, 0xbc, 0xbc, 
};
static const unsigned char gb_chars_ban1156 [] = {
    0xb3, 0xad, 
};
static const unsigned char gb_chars_ban1157 [] = {
    0xbe, 0xf1, 0xb0, 0xd1, 
};
static const unsigned char gb_chars_ban1158 [] = {
    0xd2, 0xd6, 0xca, 0xe3, 0xd7, 0xa5, 
};
static const unsigned char gb_chars_ban1159 [] = {
    0xcd, 0xb6, 0xb6, 0xb6, 0xbf, 0xb9, 0xd5, 0xdb, 
};
static const unsigned char gb_chars_ban1160 [] = {
    0xb8, 0xa7, 0xc5, 0xd7, 
};
static const unsigned char gb_chars_ban1161 [] = {
    0xde, 0xd2, 0xbf, 0xd9, 0xc2, 0xd5, 0xc7, 0xc0, 
};
static const unsigned char gb_chars_ban1162 [] = {
    0xbb, 0xa4, 0xb1, 0xa8, 
};
static const unsigned char gb_chars_ban1163 [] = {
    0xc5, 0xea, 
};
static const unsigned char gb_chars_ban1164 [] = {
    0xc5, 0xfb, 0xcc, 0xa7, 
};
static const unsigned char gb_chars_ban1165 [] = {
    0xb1, 0xa7, 
};
static const unsigned char gb_chars_ban1166 [] = {
    0xb5, 0xd6, 
};
static const unsigned char gb_chars_ban1167 [] = {
    0xc4, 0xa8, 
};
static const unsigned char gb_chars_ban1168 [] = {
    0xde, 0xd3, 0xd1, 0xba, 0xb3, 0xe9, 
};
static const unsigned char gb_chars_ban1169 [] = {
    0xc3, 0xf2, 
};
static const unsigned char gb_chars_ban1170 [] = {
    0xb7, 0xf7, 
};
static const unsigned char gb_chars_ban1171 [] = {
    0xd6, 0xf4, 0xb5, 0xa3, 0xb2, 0xf0, 0xc4, 0xb4, 0xc4, 0xe9, 
    0xc0, 0xad, 0xde, 0xd4, 
};
static const unsigned char gb_chars_ban1172 [] = {
    0xb0, 0xe8, 0xc5, 0xc4, 0xc1, 0xe0, 
};
static const unsigned char gb_chars_ban1173 [] = {
    0xb9, 0xd5, 
};
static const unsigned char gb_chars_ban1174 [] = {
    0xbe, 0xdc, 0xcd, 0xd8, 0xb0, 0xce, 
};
static const unsigned char gb_chars_ban1175 [] = {
    0xcd, 0xcf, 0xde, 0xd6, 0xbe, 0xd0, 0xd7, 0xbe, 0xde, 0xd5, 
    0xd5, 0xd0, 0xb0, 0xdd, 
};
static const unsigned char gb_chars_ban1176 [] = {
    0xc4, 0xe2, 
};
static const unsigned char gb_chars_ban1177 [] = {
    0xc2, 0xa3, 0xbc, 0xf0, 
};
static const unsigned char gb_chars_ban1178 [] = {
    0xd3, 0xb5, 0xc0, 0xb9, 0xc5, 0xa1, 0xb2, 0xa6, 0xd4, 0xf1, 
};
static const unsigned char gb_chars_ban1179 [] = {
    0xc0, 0xa8, 0xca, 0xc3, 0xde, 0xd7, 0xd5, 0xfc, 
};
static const unsigned char gb_chars_ban1180 [] = {
    0xb9, 0xb0, 
};
static const unsigned char gb_chars_ban1181 [] = {
    0xc8, 0xad, 0xcb, 0xa9, 
};
static const unsigned char gb_chars_ban1182 [] = {
    0xde, 0xd9, 0xbf, 0xbd, 
};
static const unsigned char gb_chars_ban1183 [] = {
    0xc6, 0xb4, 0xd7, 0xa7, 0xca, 0xb0, 0xc4, 0xc3, 
};
static const unsigned char gb_chars_ban1184 [] = {
    0xb3, 0xd6, 0xb9, 0xd2, 
};
static const unsigned char gb_chars_ban1185 [] = {
    0xd6, 0xb8, 0xea, 0xfc, 0xb0, 0xb4, 
};
static const unsigned char gb_chars_ban1186 [] = {
    0xbf, 0xe6, 
};
static const unsigned char gb_chars_ban1187 [] = {
    0xcc, 0xf4, 
};
static const unsigned char gb_chars_ban1188 [] = {
    0xcd, 0xda, 
};
static const unsigned char gb_chars_ban1189 [] = {
    0xd6, 0xbf, 0xc2, 0xce, 
};
static const unsigned char gb_chars_ban1190 [] = {
    0xce, 0xce, 0xcc, 0xa2, 0xd0, 0xae, 0xc4, 0xd3, 0xb5, 0xb2, 
    0xde, 0xd8, 0xd5, 0xf5, 0xbc, 0xb7, 0xbb, 0xd3, 
};
static const unsigned char gb_chars_ban1191 [] = {
    0xb0, 0xa4, 
};
static const unsigned char gb_chars_ban1192 [] = {
    0xc5, 0xb2, 0xb4, 0xec, 
};
static const unsigned char gb_chars_ban1193 [] = {
    0xd5, 0xf1, 
};
static const unsigned char gb_chars_ban1194 [] = {
    0xea, 0xfd, 
};
static const unsigned char gb_chars_ban1195 [] = {
    0xde, 0xda, 0xcd, 0xa6, 
};
static const unsigned char gb_chars_ban1196 [] = {
    0xcd, 0xec, 
};
static const unsigned char gb_chars_ban1197 [] = {
    0xce, 0xe6, 0xde, 0xdc, 
};
static const unsigned char gb_chars_ban1198 [] = {
    0xcd, 0xb1, 0xc0, 0xa6, 
};
static const unsigned char gb_chars_ban1199 [] = {
    0xd7, 0xbd, 
};
static const unsigned char gb_chars_ban1200 [] = {
    0xde, 0xdb, 0xb0, 0xc6, 0xba, 0xb4, 0xc9, 0xd3, 0xc4, 0xf3, 
    0xbe, 0xe8, 
};
static const unsigned char gb_chars_ban1201 [] = {
    0xb2, 0xb6, 
};
static const unsigned char gb_chars_ban1202 [] = {
    0xc0, 0xcc, 0xcb, 0xf0, 
};
static const unsigned char gb_chars_ban1203 [] = {
    0xbc, 0xf1, 0xbb, 0xbb, 0xb5, 0xb7, 
};
static const unsigned char gb_chars_ban1204 [] = {
    0xc5, 0xf5, 
};
static const unsigned char gb_chars_ban1205 [] = {
    0xde, 0xe6, 
};
static const unsigned char gb_chars_ban1206 [] = {
    0xde, 0xe3, 0xbe, 0xdd, 
};
static const unsigned char gb_chars_ban1207 [] = {
    0xde, 0xdf, 
};
static const unsigned char gb_chars_ban1208 [] = {
    0xb4, 0xb7, 0xbd, 0xdd, 
};
static const unsigned char gb_chars_ban1209 [] = {
    0xde, 0xe0, 0xc4, 0xed, 
};
static const unsigned char gb_chars_ban1210 [] = {
    0xcf, 0xc6, 
};
static const unsigned char gb_chars_ban1211 [] = {
    0xb5, 0xe0, 
};
static const unsigned char gb_chars_ban1212 [] = {
    0xb6, 0xde, 0xca, 0xda, 0xb5, 0xf4, 0xde, 0xe5, 
};
static const unsigned char gb_chars_ban1213 [] = {
    0xd5, 0xc6, 
};
static const unsigned char gb_chars_ban1214 [] = {
    0xde, 0xe1, 0xcc, 0xcd, 0xc6, 0xfe, 
};
static const unsigned char gb_chars_ban1215 [] = {
    0xc5, 0xc5, 
};
static const unsigned char gb_chars_ban1216 [] = {
    0xd2, 0xb4, 
};
static const unsigned char gb_chars_ban1217 [] = {
    0xbe, 0xf2, 
};
static const unsigned char gb_chars_ban1218 [] = {
    0xc2, 0xd3, 
};
static const unsigned char gb_chars_ban1219 [] = {
    0xcc, 0xbd, 0xb3, 0xb8, 
};
static const unsigned char gb_chars_ban1220 [] = {
    0xbd, 0xd3, 
};
static const unsigned char gb_chars_ban1221 [] = {
    0xbf, 0xd8, 0xcd, 0xc6, 0xd1, 0xda, 0xb4, 0xeb, 
};
static const unsigned char gb_chars_ban1222 [] = {
    0xde, 0xe4, 0xde, 0xdd, 0xde, 0xe7, 
};
static const unsigned char gb_chars_ban1223 [] = {
    0xea, 0xfe, 
};
static const unsigned char gb_chars_ban1224 [] = {
    0xc2, 0xb0, 0xde, 0xe2, 
};
static const unsigned char gb_chars_ban1225 [] = {
    0xd6, 0xc0, 0xb5, 0xa7, 
};
static const unsigned char gb_chars_ban1226 [] = {
    0xb2, 0xf4, 
};
static const unsigned char gb_chars_ban1227 [] = {
    0xde, 0xe8, 
};
static const unsigned char gb_chars_ban1228 [] = {
    0xde, 0xf2, 
};
static const unsigned char gb_chars_ban1229 [] = {
    0xde, 0xed, 
};
static const unsigned char gb_chars_ban1230 [] = {
    0xde, 0xf1, 
};
static const unsigned char gb_chars_ban1231 [] = {
    0xc8, 0xe0, 
};
static const unsigned char gb_chars_ban1232 [] = {
    0xd7, 0xe1, 0xde, 0xef, 0xc3, 0xe8, 0xcc, 0xe1, 
};
static const unsigned char gb_chars_ban1233 [] = {
    0xb2, 0xe5, 
};
static const unsigned char gb_chars_ban1234 [] = {
    0xd2, 0xbe, 
};
static const unsigned char gb_chars_ban1235 [] = {
    0xde, 0xee, 
};
static const unsigned char gb_chars_ban1236 [] = {
    0xde, 0xeb, 0xce, 0xd5, 
};
static const unsigned char gb_chars_ban1237 [] = {
    0xb4, 0xa7, 
};
static const unsigned char gb_chars_ban1238 [] = {
    0xbf, 0xab, 0xbe, 0xbe, 
};
static const unsigned char gb_chars_ban1239 [] = {
    0xbd, 0xd2, 
};
static const unsigned char gb_chars_ban1240 [] = {
    0xde, 0xe9, 
};
static const unsigned char gb_chars_ban1241 [] = {
    0xd4, 0xae, 
};
static const unsigned char gb_chars_ban1242 [] = {
    0xde, 0xde, 
};
static const unsigned char gb_chars_ban1243 [] = {
    0xde, 0xea, 
};
static const unsigned char gb_chars_ban1244 [] = {
    0xc0, 0xbf, 
};
static const unsigned char gb_chars_ban1245 [] = {
    0xde, 0xec, 0xb2, 0xf3, 0xb8, 0xe9, 0xc2, 0xa7, 
};
static const unsigned char gb_chars_ban1246 [] = {
    0xbd, 0xc1, 
};
static const unsigned char gb_chars_ban1247 [] = {
    0xde, 0xf5, 0xde, 0xf8, 
};
static const unsigned char gb_chars_ban1248 [] = {
    0xb2, 0xab, 0xb4, 0xa4, 
};
static const unsigned char gb_chars_ban1249 [] = {
    0xb4, 0xea, 0xc9, 0xa6, 
};
static const unsigned char gb_chars_ban1250 [] = {
    0xde, 0xf6, 0xcb, 0xd1, 
};
static const unsigned char gb_chars_ban1251 [] = {
    0xb8, 0xe3, 
};
static const unsigned char gb_chars_ban1252 [] = {
    0xde, 0xf7, 0xde, 0xfa, 
};
static const unsigned char gb_chars_ban1253 [] = {
    0xde, 0xf9, 
};
static const unsigned char gb_chars_ban1254 [] = {
    0xcc, 0xc2, 
};
static const unsigned char gb_chars_ban1255 [] = {
    0xb0, 0xe1, 0xb4, 0xee, 
};
static const unsigned char gb_chars_ban1256 [] = {
    0xe5, 0xba, 
};
static const unsigned char gb_chars_ban1257 [] = {
    0xd0, 0xaf, 
};
static const unsigned char gb_chars_ban1258 [] = {
    0xb2, 0xeb, 
};
static const unsigned char gb_chars_ban1259 [] = {
    0xeb, 0xa1, 
};
static const unsigned char gb_chars_ban1260 [] = {
    0xde, 0xf4, 
};
static const unsigned char gb_chars_ban1261 [] = {
    0xc9, 0xe3, 0xde, 0xf3, 0xb0, 0xda, 0xd2, 0xa1, 0xb1, 0xf7, 
};
static const unsigned char gb_chars_ban1262 [] = {
    0xcc, 0xaf, 
};
static const unsigned char gb_chars_ban1263 [] = {
    0xde, 0xf0, 
};
static const unsigned char gb_chars_ban1264 [] = {
    0xcb, 0xa4, 
};
static const unsigned char gb_chars_ban1265 [] = {
    0xd5, 0xaa, 
};
static const unsigned char gb_chars_ban1266 [] = {
    0xde, 0xfb, 
};
static const unsigned char gb_chars_ban1267 [] = {
    0xb4, 0xdd, 
};
static const unsigned char gb_chars_ban1268 [] = {
    0xc4, 0xa6, 
};
static const unsigned char gb_chars_ban1269 [] = {
    0xde, 0xfd, 
};
static const unsigned char gb_chars_ban1270 [] = {
    0xc3, 0xfe, 0xc4, 0xa1, 0xdf, 0xa1, 
};
static const unsigned char gb_chars_ban1271 [] = {
    0xc1, 0xcc, 
};
static const unsigned char gb_chars_ban1272 [] = {
    0xde, 0xfc, 0xbe, 0xef, 
};
static const unsigned char gb_chars_ban1273 [] = {
    0xc6, 0xb2, 
};
static const unsigned char gb_chars_ban1274 [] = {
    0xb3, 0xc5, 0xc8, 0xf6, 
};
static const unsigned char gb_chars_ban1275 [] = {
    0xcb, 0xba, 0xde, 0xfe, 
};
static const unsigned char gb_chars_ban1276 [] = {
    0xdf, 0xa4, 
};
static const unsigned char gb_chars_ban1277 [] = {
    0xd7, 0xb2, 
};
static const unsigned char gb_chars_ban1278 [] = {
    0xb3, 0xb7, 
};
static const unsigned char gb_chars_ban1279 [] = {
    0xc1, 0xc3, 
};
static const unsigned char gb_chars_ban1280 [] = {
    0xc7, 0xcb, 0xb2, 0xa5, 0xb4, 0xe9, 
};
static const unsigned char gb_chars_ban1281 [] = {
    0xd7, 0xab, 
};
static const unsigned char gb_chars_ban1282 [] = {
    0xc4, 0xec, 
};
static const unsigned char gb_chars_ban1283 [] = {
    0xdf, 0xa2, 0xdf, 0xa3, 
};
static const unsigned char gb_chars_ban1284 [] = {
    0xdf, 0xa5, 
};
static const unsigned char gb_chars_ban1285 [] = {
    0xba, 0xb3, 
};
static const unsigned char gb_chars_ban1286 [] = {
    0xdf, 0xa6, 
};
static const unsigned char gb_chars_ban1287 [] = {
    0xc0, 0xde, 
};
static const unsigned char gb_chars_ban1288 [] = {
    0xc9, 0xc3, 
};
static const unsigned char gb_chars_ban1289 [] = {
    0xb2, 0xd9, 0xc7, 0xe6, 
};
static const unsigned char gb_chars_ban1290 [] = {
    0xdf, 0xa7, 
};
static const unsigned char gb_chars_ban1291 [] = {
    0xc7, 0xdc, 
};
static const unsigned char gb_chars_ban1292 [] = {
    0xdf, 0xa8, 0xeb, 0xa2, 
};
static const unsigned char gb_chars_ban1293 [] = {
    0xcb, 0xd3, 
};
static const unsigned char gb_chars_ban1294 [] = {
    0xdf, 0xaa, 
};
static const unsigned char gb_chars_ban1295 [] = {
    0xdf, 0xa9, 
};
static const unsigned char gb_chars_ban1296 [] = {
    0xb2, 0xc1, 
};
static const unsigned char gb_chars_ban1297 [] = {
    0xc5, 0xca, 
};
static const unsigned char gb_chars_ban1298 [] = {
    0xdf, 0xab, 
};
static const unsigned char gb_chars_ban1299 [] = {
    0xd4, 0xdc, 
};
static const unsigned char gb_chars_ban1300 [] = {
    0xc8, 0xc1, 
};
static const unsigned char gb_chars_ban1301 [] = {
    0xdf, 0xac, 
};
static const unsigned char gb_chars_ban1302 [] = {
    0xbe, 0xf0, 
};
static const unsigned char gb_chars_ban1303 [] = {
    0xdf, 0xad, 0xd6, 0xa7, 
};
static const unsigned char gb_chars_ban1304 [] = {
    0xea, 0xb7, 0xeb, 0xb6, 0xca, 0xd5, 
};
static const unsigned char gb_chars_ban1305 [] = {
    0xd8, 0xfc, 0xb8, 0xc4, 
};
static const unsigned char gb_chars_ban1306 [] = {
    0xb9, 0xa5, 
};
static const unsigned char gb_chars_ban1307 [] = {
    0xb7, 0xc5, 0xd5, 0xfe, 
};
static const unsigned char gb_chars_ban1308 [] = {
    0xb9, 0xca, 
};
static const unsigned char gb_chars_ban1309 [] = {
    0xd0, 0xa7, 0xf4, 0xcd, 
};
static const unsigned char gb_chars_ban1310 [] = {
    0xb5, 0xd0, 
};
static const unsigned char gb_chars_ban1311 [] = {
    0xc3, 0xf4, 
};
static const unsigned char gb_chars_ban1312 [] = {
    0xbe, 0xc8, 
};
static const unsigned char gb_chars_ban1313 [] = {
    0xeb, 0xb7, 0xb0, 0xbd, 
};
static const unsigned char gb_chars_ban1314 [] = {
    0xbd, 0xcc, 
};
static const unsigned char gb_chars_ban1315 [] = {
    0xc1, 0xb2, 
};
static const unsigned char gb_chars_ban1316 [] = {
    0xb1, 0xd6, 0xb3, 0xa8, 
};
static const unsigned char gb_chars_ban1317 [] = {
    0xb8, 0xd2, 0xc9, 0xa2, 
};
static const unsigned char gb_chars_ban1318 [] = {
    0xb6, 0xd8, 
};
static const unsigned char gb_chars_ban1319 [] = {
    0xeb, 0xb8, 0xbe, 0xb4, 
};
static const unsigned char gb_chars_ban1320 [] = {
    0xca, 0xfd, 
};
static const unsigned char gb_chars_ban1321 [] = {
    0xc7, 0xc3, 
};
static const unsigned char gb_chars_ban1322 [] = {
    0xd5, 0xfb, 
};
static const unsigned char gb_chars_ban1323 [] = {
    0xb7, 0xf3, 
};
static const unsigned char gb_chars_ban1324 [] = {
    0xce, 0xc4, 
};
static const unsigned char gb_chars_ban1325 [] = {
    0xd5, 0xab, 0xb1, 0xf3, 
};
static const unsigned char gb_chars_ban1326 [] = {
    0xec, 0xb3, 0xb0, 0xdf, 
};
static const unsigned char gb_chars_ban1327 [] = {
    0xec, 0xb5, 
};
static const unsigned char gb_chars_ban1328 [] = {
    0xb6, 0xb7, 
};
static const unsigned char gb_chars_ban1329 [] = {
    0xc1, 0xcf, 
};
static const unsigned char gb_chars_ban1330 [] = {
    0xf5, 0xfa, 0xd0, 0xb1, 
};
static const unsigned char gb_chars_ban1331 [] = {
    0xd5, 0xe5, 
};
static const unsigned char gb_chars_ban1332 [] = {
    0xce, 0xd3, 
};
static const unsigned char gb_chars_ban1333 [] = {
    0xbd, 0xef, 0xb3, 0xe2, 
};
static const unsigned char gb_chars_ban1334 [] = {
    0xb8, 0xab, 
};
static const unsigned char gb_chars_ban1335 [] = {
    0xd5, 0xb6, 
};
static const unsigned char gb_chars_ban1336 [] = {
    0xed, 0xbd, 
};
static const unsigned char gb_chars_ban1337 [] = {
    0xb6, 0xcf, 
};
static const unsigned char gb_chars_ban1338 [] = {
    0xcb, 0xb9, 0xd0, 0xc2, 
};
static const unsigned char gb_chars_ban1339 [] = {
    0xb7, 0xbd, 
};
static const unsigned char gb_chars_ban1340 [] = {
    0xec, 0xb6, 0xca, 0xa9, 
};
static const unsigned char gb_chars_ban1341 [] = {
    0xc5, 0xd4, 
};
static const unsigned char gb_chars_ban1342 [] = {
    0xec, 0xb9, 0xec, 0xb8, 0xc2, 0xc3, 0xec, 0xb7, 
};
static const unsigned char gb_chars_ban1343 [] = {
    0xd0, 0xfd, 0xec, 0xba, 
};
static const unsigned char gb_chars_ban1344 [] = {
    0xec, 0xbb, 0xd7, 0xe5, 
};
static const unsigned char gb_chars_ban1345 [] = {
    0xec, 0xbc, 
};
static const unsigned char gb_chars_ban1346 [] = {
    0xec, 0xbd, 0xc6, 0xec, 
};
static const unsigned char gb_chars_ban1347 [] = {
    0xce, 0xde, 
};
static const unsigned char gb_chars_ban1348 [] = {
    0xbc, 0xc8, 
};
static const unsigned char gb_chars_ban1349 [] = {
    0xc8, 0xd5, 0xb5, 0xa9, 0xbe, 0xc9, 0xd6, 0xbc, 0xd4, 0xe7, 
};
static const unsigned char gb_chars_ban1350 [] = {
    0xd1, 0xae, 0xd0, 0xf1, 0xea, 0xb8, 0xea, 0xb9, 0xea, 0xba, 
    0xba, 0xb5, 
};
static const unsigned char gb_chars_ban1351 [] = {
    0xca, 0xb1, 0xbf, 0xf5, 
};
static const unsigned char gb_chars_ban1352 [] = {
    0xcd, 0xfa, 
};
static const unsigned char gb_chars_ban1353 [] = {
    0xea, 0xc0, 
};
static const unsigned char gb_chars_ban1354 [] = {
    0xb0, 0xba, 0xea, 0xbe, 
};
static const unsigned char gb_chars_ban1355 [] = {
    0xc0, 0xa5, 
};
static const unsigned char gb_chars_ban1356 [] = {
    0xea, 0xbb, 
};
static const unsigned char gb_chars_ban1357 [] = {
    0xb2, 0xfd, 
};
static const unsigned char gb_chars_ban1358 [] = {
    0xc3, 0xf7, 0xbb, 0xe8, 
};
static const unsigned char gb_chars_ban1359 [] = {
    0xd2, 0xd7, 0xce, 0xf4, 0xea, 0xbf, 
};
static const unsigned char gb_chars_ban1360 [] = {
    0xea, 0xbc, 
};
static const unsigned char gb_chars_ban1361 [] = {
    0xea, 0xc3, 
};
static const unsigned char gb_chars_ban1362 [] = {
    0xd0, 0xc7, 0xd3, 0xb3, 
};
static const unsigned char gb_chars_ban1363 [] = {
    0xb4, 0xba, 
};
static const unsigned char gb_chars_ban1364 [] = {
    0xc3, 0xc1, 0xd7, 0xf2, 
};
static const unsigned char gb_chars_ban1365 [] = {
    0xd5, 0xd1, 
};
static const unsigned char gb_chars_ban1366 [] = {
    0xca, 0xc7, 
};
static const unsigned char gb_chars_ban1367 [] = {
    0xea, 0xc5, 
};
static const unsigned char gb_chars_ban1368 [] = {
    0xea, 0xc4, 0xea, 0xc7, 0xea, 0xc6, 
};
static const unsigned char gb_chars_ban1369 [] = {
    0xd6, 0xe7, 
};
static const unsigned char gb_chars_ban1370 [] = {
    0xcf, 0xd4, 
};
static const unsigned char gb_chars_ban1371 [] = {
    0xea, 0xcb, 
};
static const unsigned char gb_chars_ban1372 [] = {
    0xbb, 0xce, 
};
static const unsigned char gb_chars_ban1373 [] = {
    0xbd, 0xfa, 0xc9, 0xce, 
};
static const unsigned char gb_chars_ban1374 [] = {
    0xea, 0xcc, 
};
static const unsigned char gb_chars_ban1375 [] = {
    0xc9, 0xb9, 0xcf, 0xfe, 0xea, 0xca, 0xd4, 0xce, 0xea, 0xcd, 
    0xea, 0xcf, 
};
static const unsigned char gb_chars_ban1376 [] = {
    0xcd, 0xed, 
};
static const unsigned char gb_chars_ban1377 [] = {
    0xea, 0xc9, 
};
static const unsigned char gb_chars_ban1378 [] = {
    0xea, 0xce, 
};
static const unsigned char gb_chars_ban1379 [] = {
    0xce, 0xee, 
};
static const unsigned char gb_chars_ban1380 [] = {
    0xbb, 0xde, 
};
static const unsigned char gb_chars_ban1381 [] = {
    0xb3, 0xbf, 
};
static const unsigned char gb_chars_ban1382 [] = {
    0xc6, 0xd5, 0xbe, 0xb0, 0xce, 0xfa, 
};
static const unsigned char gb_chars_ban1383 [] = {
    0xc7, 0xe7, 
};
static const unsigned char gb_chars_ban1384 [] = {
    0xbe, 0xa7, 0xea, 0xd0, 
};
static const unsigned char gb_chars_ban1385 [] = {
    0xd6, 0xc7, 
};
static const unsigned char gb_chars_ban1386 [] = {
    0xc1, 0xc0, 
};
static const unsigned char gb_chars_ban1387 [] = {
    0xd4, 0xdd, 
};
static const unsigned char gb_chars_ban1388 [] = {
    0xea, 0xd1, 
};
static const unsigned char gb_chars_ban1389 [] = {
    0xcf, 0xbe, 
};
static const unsigned char gb_chars_ban1390 [] = {
    0xea, 0xd2, 
};
static const unsigned char gb_chars_ban1391 [] = {
    0xca, 0xee, 
};
static const unsigned char gb_chars_ban1392 [] = {
    0xc5, 0xaf, 0xb0, 0xb5, 
};
static const unsigned char gb_chars_ban1393 [] = {
    0xea, 0xd4, 
};
static const unsigned char gb_chars_ban1394 [] = {
    0xea, 0xd3, 0xf4, 0xdf, 
};
static const unsigned char gb_chars_ban1395 [] = {
    0xc4, 0xba, 
};
static const unsigned char gb_chars_ban1396 [] = {
    0xb1, 0xa9, 
};
static const unsigned char gb_chars_ban1397 [] = {
    0xe5, 0xdf, 
};
static const unsigned char gb_chars_ban1398 [] = {
    0xea, 0xd5, 
};
static const unsigned char gb_chars_ban1399 [] = {
    0xca, 0xef, 
};
static const unsigned char gb_chars_ban1400 [] = {
    0xea, 0xd6, 0xea, 0xd7, 0xc6, 0xd8, 
};
static const unsigned char gb_chars_ban1401 [] = {
    0xea, 0xd8, 
};
static const unsigned char gb_chars_ban1402 [] = {
    0xea, 0xd9, 
};
static const unsigned char gb_chars_ban1403 [] = {
    0xd4, 0xbb, 
};
static const unsigned char gb_chars_ban1404 [] = {
    0xc7, 0xfa, 0xd2, 0xb7, 0xb8, 0xfc, 
};
static const unsigned char gb_chars_ban1405 [] = {
    0xea, 0xc2, 
};
static const unsigned char gb_chars_ban1406 [] = {
    0xb2, 0xdc, 
};
static const unsigned char gb_chars_ban1407 [] = {
    0xc2, 0xfc, 
};
static const unsigned char gb_chars_ban1408 [] = {
    0xd4, 0xf8, 0xcc, 0xe6, 0xd7, 0xee, 
};
static const unsigned char gb_chars_ban1409 [] = {
    0xd4, 0xc2, 0xd3, 0xd0, 0xeb, 0xc3, 0xc5, 0xf3, 
};
static const unsigned char gb_chars_ban1410 [] = {
    0xb7, 0xfe, 
};
static const unsigned char gb_chars_ban1411 [] = {
    0xeb, 0xd4, 
};
static const unsigned char gb_chars_ban1412 [] = {
    0xcb, 0xb7, 0xeb, 0xde, 
};
static const unsigned char gb_chars_ban1413 [] = {
    0xc0, 0xca, 
};
static const unsigned char gb_chars_ban1414 [] = {
    0xcd, 0xfb, 
};
static const unsigned char gb_chars_ban1415 [] = {
    0xb3, 0xaf, 
};
static const unsigned char gb_chars_ban1416 [] = {
    0xc6, 0xda, 
};
static const unsigned char gb_chars_ban1417 [] = {
    0xeb, 0xfc, 
};
static const unsigned char gb_chars_ban1418 [] = {
    0xc4, 0xbe, 
};
static const unsigned char gb_chars_ban1419 [] = {
    0xce, 0xb4, 0xc4, 0xa9, 0xb1, 0xbe, 0xd4, 0xfd, 
};
static const unsigned char gb_chars_ban1420 [] = {
    0xca, 0xf5, 
};
static const unsigned char gb_chars_ban1421 [] = {
    0xd6, 0xec, 
};
static const unsigned char gb_chars_ban1422 [] = {
    0xc6, 0xd3, 0xb6, 0xe4, 
};
static const unsigned char gb_chars_ban1423 [] = {
    0xbb, 0xfa, 
};
static const unsigned char gb_chars_ban1424 [] = {
    0xd0, 0xe0, 
};
static const unsigned char gb_chars_ban1425 [] = {
    0xc9, 0xb1, 
};
static const unsigned char gb_chars_ban1426 [] = {
    0xd4, 0xd3, 0xc8, 0xa8, 
};
static const unsigned char gb_chars_ban1427 [] = {
    0xb8, 0xcb, 
};
static const unsigned char gb_chars_ban1428 [] = {
    0xe8, 0xbe, 0xc9, 0xbc, 
};
static const unsigned char gb_chars_ban1429 [] = {
    0xe8, 0xbb, 
};
static const unsigned char gb_chars_ban1430 [] = {
    0xc0, 0xee, 0xd0, 0xd3, 0xb2, 0xc4, 0xb4, 0xe5, 
};
static const unsigned char gb_chars_ban1431 [] = {
    0xe8, 0xbc, 
};
static const unsigned char gb_chars_ban1432 [] = {
    0xd5, 0xc8, 
};
static const unsigned char gb_chars_ban1433 [] = {
    0xb6, 0xc5, 
};
static const unsigned char gb_chars_ban1434 [] = {
    0xe8, 0xbd, 0xca, 0xf8, 0xb8, 0xdc, 0xcc, 0xf5, 
};
static const unsigned char gb_chars_ban1435 [] = {
    0xc0, 0xb4, 
};
static const unsigned char gb_chars_ban1436 [] = {
    0xd1, 0xee, 0xe8, 0xbf, 0xe8, 0xc2, 
};
static const unsigned char gb_chars_ban1437 [] = {
    0xba, 0xbc, 
};
static const unsigned char gb_chars_ban1438 [] = {
    0xb1, 0xad, 0xbd, 0xdc, 
};
static const unsigned char gb_chars_ban1439 [] = {
    0xea, 0xbd, 0xe8, 0xc3, 
};
static const unsigned char gb_chars_ban1440 [] = {
    0xe8, 0xc6, 
};
static const unsigned char gb_chars_ban1441 [] = {
    0xe8, 0xcb, 
};
static const unsigned char gb_chars_ban1442 [] = {
    0xe8, 0xcc, 
};
static const unsigned char gb_chars_ban1443 [] = {
    0xcb, 0xc9, 0xb0, 0xe5, 
};
static const unsigned char gb_chars_ban1444 [] = {
    0xbc, 0xab, 
};
static const unsigned char gb_chars_ban1445 [] = {
    0xb9, 0xb9, 
};
static const unsigned char gb_chars_ban1446 [] = {
    0xe8, 0xc1, 
};
static const unsigned char gb_chars_ban1447 [] = {
    0xcd, 0xf7, 
};
static const unsigned char gb_chars_ban1448 [] = {
    0xe8, 0xca, 
};
static const unsigned char gb_chars_ban1449 [] = {
    0xce, 0xf6, 
};
static const unsigned char gb_chars_ban1450 [] = {
    0xd5, 0xed, 
};
static const unsigned char gb_chars_ban1451 [] = {
    0xc1, 0xd6, 0xe8, 0xc4, 
};
static const unsigned char gb_chars_ban1452 [] = {
    0xc3, 0xb6, 
};
static const unsigned char gb_chars_ban1453 [] = {
    0xb9, 0xfb, 0xd6, 0xa6, 0xe8, 0xc8, 
};
static const unsigned char gb_chars_ban1454 [] = {
    0xca, 0xe0, 0xd4, 0xe6, 
};
static const unsigned char gb_chars_ban1455 [] = {
    0xe8, 0xc0, 
};
static const unsigned char gb_chars_ban1456 [] = {
    0xe8, 0xc5, 0xe8, 0xc7, 
};
static const unsigned char gb_chars_ban1457 [] = {
    0xc7, 0xb9, 0xb7, 0xe3, 
};
static const unsigned char gb_chars_ban1458 [] = {
    0xe8, 0xc9, 
};
static const unsigned char gb_chars_ban1459 [] = {
    0xbf, 0xdd, 0xe8, 0xd2, 
};
static const unsigned char gb_chars_ban1460 [] = {
    0xe8, 0xd7, 
};
static const unsigned char gb_chars_ban1461 [] = {
    0xe8, 0xd5, 0xbc, 0xdc, 0xbc, 0xcf, 0xe8, 0xdb, 
};
static const unsigned char gb_chars_ban1462 [] = {
    0xe8, 0xde, 
};
static const unsigned char gb_chars_ban1463 [] = {
    0xe8, 0xda, 0xb1, 0xfa, 
};
static const unsigned char gb_chars_ban1464 [] = {
    0xb0, 0xd8, 0xc4, 0xb3, 0xb8, 0xcc, 0xc6, 0xe2, 0xc8, 0xbe, 
    0xc8, 0xe1, 
};
static const unsigned char gb_chars_ban1465 [] = {
    0xe8, 0xcf, 0xe8, 0xd4, 0xe8, 0xd6, 
};
static const unsigned char gb_chars_ban1466 [] = {
    0xb9, 0xf1, 0xe8, 0xd8, 0xd7, 0xf5, 
};
static const unsigned char gb_chars_ban1467 [] = {
    0xc4, 0xfb, 
};
static const unsigned char gb_chars_ban1468 [] = {
    0xe8, 0xdc, 
};
static const unsigned char gb_chars_ban1469 [] = {
    0xb2, 0xe9, 
};
static const unsigned char gb_chars_ban1470 [] = {
    0xe8, 0xd1, 
};
static const unsigned char gb_chars_ban1471 [] = {
    0xbc, 0xed, 
};
static const unsigned char gb_chars_ban1472 [] = {
    0xbf, 0xc2, 0xe8, 0xcd, 0xd6, 0xf9, 
};
static const unsigned char gb_chars_ban1473 [] = {
    0xc1, 0xf8, 0xb2, 0xf1, 
};
static const unsigned char gb_chars_ban1474 [] = {
    0xe8, 0xdf, 
};
static const unsigned char gb_chars_ban1475 [] = {
    0xca, 0xc1, 0xe8, 0xd9, 
};
static const unsigned char gb_chars_ban1476 [] = {
    0xd5, 0xa4, 
};
static const unsigned char gb_chars_ban1477 [] = {
    0xb1, 0xea, 0xd5, 0xbb, 0xe8, 0xce, 0xe8, 0xd0, 0xb6, 0xb0, 
    0xe8, 0xd3, 
};
static const unsigned char gb_chars_ban1478 [] = {
    0xe8, 0xdd, 0xc0, 0xb8, 
};
static const unsigned char gb_chars_ban1479 [] = {
    0xca, 0xf7, 
};
static const unsigned char gb_chars_ban1480 [] = {
    0xcb, 0xa8, 
};
static const unsigned char gb_chars_ban1481 [] = {
    0xc6, 0xdc, 0xc0, 0xf5, 
};
static const unsigned char gb_chars_ban1482 [] = {
    0xe8, 0xe9, 
};
static const unsigned char gb_chars_ban1483 [] = {
    0xd0, 0xa3, 
};
static const unsigned char gb_chars_ban1484 [] = {
    0xe8, 0xf2, 0xd6, 0xea, 
};
static const unsigned char gb_chars_ban1485 [] = {
    0xe8, 0xe0, 0xe8, 0xe1, 
};
static const unsigned char gb_chars_ban1486 [] = {
    0xd1, 0xf9, 0xba, 0xcb, 0xb8, 0xf9, 
};
static const unsigned char gb_chars_ban1487 [] = {
    0xb8, 0xf1, 0xd4, 0xd4, 0xe8, 0xef, 
};
static const unsigned char gb_chars_ban1488 [] = {
    0xe8, 0xee, 0xe8, 0xec, 0xb9, 0xf0, 0xcc, 0xd2, 0xe8, 0xe6, 
    0xce, 0xa6, 0xbf, 0xf2, 
};
static const unsigned char gb_chars_ban1489 [] = {
    0xb0, 0xb8, 0xe8, 0xf1, 0xe8, 0xf0, 
};
static const unsigned char gb_chars_ban1490 [] = {
    0xd7, 0xc0, 
};
static const unsigned char gb_chars_ban1491 [] = {
    0xe8, 0xe4, 
};
static const unsigned char gb_chars_ban1492 [] = {
    0xcd, 0xa9, 0xc9, 0xa3, 
};
static const unsigned char gb_chars_ban1493 [] = {
    0xbb, 0xb8, 0xbd, 0xdb, 0xe8, 0xea, 
};
static const unsigned char gb_chars_ban1494 [] = {
    0xe8, 0xe2, 0xe8, 0xe3, 0xe8, 0xe5, 0xb5, 0xb5, 0xe8, 0xe7, 
    0xc7, 0xc5, 0xe8, 0xeb, 0xe8, 0xed, 0xbd, 0xb0, 0xd7, 0xae, 
};
static const unsigned char gb_chars_ban1495 [] = {
    0xe8, 0xf8, 
};
static const unsigned char gb_chars_ban1496 [] = {
    0xe8, 0xf5, 
};
static const unsigned char gb_chars_ban1497 [] = {
    0xcd, 0xb0, 0xe8, 0xf6, 
};
static const unsigned char gb_chars_ban1498 [] = {
    0xc1, 0xba, 
};
static const unsigned char gb_chars_ban1499 [] = {
    0xe8, 0xe8, 
};
static const unsigned char gb_chars_ban1500 [] = {
    0xc3, 0xb7, 0xb0, 0xf0, 
};
static const unsigned char gb_chars_ban1501 [] = {
    0xe8, 0xf4, 
};
static const unsigned char gb_chars_ban1502 [] = {
    0xe8, 0xf7, 
};
static const unsigned char gb_chars_ban1503 [] = {
    0xb9, 0xa3, 
};
static const unsigned char gb_chars_ban1504 [] = {
    0xc9, 0xd2, 
};
static const unsigned char gb_chars_ban1505 [] = {
    0xc3, 0xce, 0xce, 0xe0, 0xc0, 0xe6, 
};
static const unsigned char gb_chars_ban1506 [] = {
    0xcb, 0xf3, 
};
static const unsigned char gb_chars_ban1507 [] = {
    0xcc, 0xdd, 0xd0, 0xb5, 
};
static const unsigned char gb_chars_ban1508 [] = {
    0xca, 0xe1, 
};
static const unsigned char gb_chars_ban1509 [] = {
    0xe8, 0xf3, 
};
static const unsigned char gb_chars_ban1510 [] = {
    0xbc, 0xec, 
};
static const unsigned char gb_chars_ban1511 [] = {
    0xe8, 0xf9, 
};
static const unsigned char gb_chars_ban1512 [] = {
    0xc3, 0xde, 
};
static const unsigned char gb_chars_ban1513 [] = {
    0xc6, 0xe5, 
};
static const unsigned char gb_chars_ban1514 [] = {
    0xb9, 0xf7, 
};
static const unsigned char gb_chars_ban1515 [] = {
    0xb0, 0xf4, 
};
static const unsigned char gb_chars_ban1516 [] = {
    0xd7, 0xd8, 
};
static const unsigned char gb_chars_ban1517 [] = {
    0xbc, 0xac, 
};
static const unsigned char gb_chars_ban1518 [] = {
    0xc5, 0xef, 
};
static const unsigned char gb_chars_ban1519 [] = {
    0xcc, 0xc4, 
};
static const unsigned char gb_chars_ban1520 [] = {
    0xe9, 0xa6, 
};
static const unsigned char gb_chars_ban1521 [] = {
    0xc9, 0xad, 
};
static const unsigned char gb_chars_ban1522 [] = {
    0xe9, 0xa2, 0xc0, 0xe2, 
};
static const unsigned char gb_chars_ban1523 [] = {
    0xbf, 0xc3, 
};
static const unsigned char gb_chars_ban1524 [] = {
    0xe8, 0xfe, 0xb9, 0xd7, 
};
static const unsigned char gb_chars_ban1525 [] = {
    0xe8, 0xfb, 
};
static const unsigned char gb_chars_ban1526 [] = {
    0xe9, 0xa4, 
};
static const unsigned char gb_chars_ban1527 [] = {
    0xd2, 0xce, 
};
static const unsigned char gb_chars_ban1528 [] = {
    0xe9, 0xa3, 
};
static const unsigned char gb_chars_ban1529 [] = {
    0xd6, 0xb2, 0xd7, 0xb5, 
};
static const unsigned char gb_chars_ban1530 [] = {
    0xe9, 0xa7, 
};
static const unsigned char gb_chars_ban1531 [] = {
    0xbd, 0xb7, 
};
static const unsigned char gb_chars_ban1532 [] = {
    0xe8, 0xfc, 0xe8, 0xfd, 
};
static const unsigned char gb_chars_ban1533 [] = {
    0xe9, 0xa1, 
};
static const unsigned char gb_chars_ban1534 [] = {
    0xcd, 0xd6, 
};
static const unsigned char gb_chars_ban1535 [] = {
    0xd2, 0xac, 
};
static const unsigned char gb_chars_ban1536 [] = {
    0xe9, 0xb2, 
};
static const unsigned char gb_chars_ban1537 [] = {
    0xe9, 0xa9, 
};
static const unsigned char gb_chars_ban1538 [] = {
    0xb4, 0xaa, 
};
static const unsigned char gb_chars_ban1539 [] = {
    0xb4, 0xbb, 
};
static const unsigned char gb_chars_ban1540 [] = {
    0xe9, 0xab, 
};
static const unsigned char gb_chars_ban1541 [] = {
    0xd0, 0xa8, 
};
static const unsigned char gb_chars_ban1542 [] = {
    0xe9, 0xa5, 
};
static const unsigned char gb_chars_ban1543 [] = {
    0xb3, 0xfe, 
};
static const unsigned char gb_chars_ban1544 [] = {
    0xe9, 0xac, 0xc0, 0xe3, 
};
static const unsigned char gb_chars_ban1545 [] = {
    0xe9, 0xaa, 
};
static const unsigned char gb_chars_ban1546 [] = {
    0xe9, 0xb9, 
};
static const unsigned char gb_chars_ban1547 [] = {
    0xe9, 0xb8, 
};
static const unsigned char gb_chars_ban1548 [] = {
    0xe9, 0xae, 
};
static const unsigned char gb_chars_ban1549 [] = {
    0xe8, 0xfa, 
};
static const unsigned char gb_chars_ban1550 [] = {
    0xe9, 0xa8, 
};
static const unsigned char gb_chars_ban1551 [] = {
    0xbf, 0xac, 0xe9, 0xb1, 0xe9, 0xba, 
};
static const unsigned char gb_chars_ban1552 [] = {
    0xc2, 0xa5, 
};
static const unsigned char gb_chars_ban1553 [] = {
    0xe9, 0xaf, 
};
static const unsigned char gb_chars_ban1554 [] = {
    0xb8, 0xc5, 
};
static const unsigned char gb_chars_ban1555 [] = {
    0xe9, 0xad, 
};
static const unsigned char gb_chars_ban1556 [] = {
    0xd3, 0xdc, 0xe9, 0xb4, 0xe9, 0xb5, 0xe9, 0xb7, 
};
static const unsigned char gb_chars_ban1557 [] = {
    0xe9, 0xc7, 
};
static const unsigned char gb_chars_ban1558 [] = {
    0xc0, 0xc6, 0xe9, 0xc5, 
};
static const unsigned char gb_chars_ban1559 [] = {
    0xe9, 0xb0, 
};
static const unsigned char gb_chars_ban1560 [] = {
    0xe9, 0xbb, 0xb0, 0xf1, 
};
static const unsigned char gb_chars_ban1561 [] = {
    0xe9, 0xbc, 0xd5, 0xa5, 
};
static const unsigned char gb_chars_ban1562 [] = {
    0xe9, 0xbe, 
};
static const unsigned char gb_chars_ban1563 [] = {
    0xe9, 0xbf, 
};
static const unsigned char gb_chars_ban1564 [] = {
    0xe9, 0xc1, 
};
static const unsigned char gb_chars_ban1565 [] = {
    0xc1, 0xf1, 
};
static const unsigned char gb_chars_ban1566 [] = {
    0xc8, 0xb6, 
};
static const unsigned char gb_chars_ban1567 [] = {
    0xe9, 0xbd, 
};
static const unsigned char gb_chars_ban1568 [] = {
    0xe9, 0xc2, 
};
static const unsigned char gb_chars_ban1569 [] = {
    0xe9, 0xc3, 
};
static const unsigned char gb_chars_ban1570 [] = {
    0xe9, 0xb3, 
};
static const unsigned char gb_chars_ban1571 [] = {
    0xe9, 0xb6, 
};
static const unsigned char gb_chars_ban1572 [] = {
    0xbb, 0xb1, 
};
static const unsigned char gb_chars_ban1573 [] = {
    0xe9, 0xc0, 
};
static const unsigned char gb_chars_ban1574 [] = {
    0xbc, 0xf7, 
};
static const unsigned char gb_chars_ban1575 [] = {
    0xe9, 0xc4, 0xe9, 0xc6, 
};
static const unsigned char gb_chars_ban1576 [] = {
    0xe9, 0xca, 
};
static const unsigned char gb_chars_ban1577 [] = {
    0xe9, 0xce, 
};
static const unsigned char gb_chars_ban1578 [] = {
    0xb2, 0xdb, 
};
static const unsigned char gb_chars_ban1579 [] = {
    0xe9, 0xc8, 
};
static const unsigned char gb_chars_ban1580 [] = {
    0xb7, 0xae, 
};
static const unsigned char gb_chars_ban1581 [] = {
    0xe9, 0xcb, 0xe9, 0xcc, 
};
static const unsigned char gb_chars_ban1582 [] = {
    0xd5, 0xc1, 
};
static const unsigned char gb_chars_ban1583 [] = {
    0xc4, 0xa3, 
};
static const unsigned char gb_chars_ban1584 [] = {
    0xe9, 0xd8, 
};
static const unsigned char gb_chars_ban1585 [] = {
    0xba, 0xe1, 
};
static const unsigned char gb_chars_ban1586 [] = {
    0xe9, 0xc9, 
};
static const unsigned char gb_chars_ban1587 [] = {
    0xd3, 0xa3, 
};
static const unsigned char gb_chars_ban1588 [] = {
    0xe9, 0xd4, 
};
static const unsigned char gb_chars_ban1589 [] = {
    0xe9, 0xd7, 0xe9, 0xd0, 
};
static const unsigned char gb_chars_ban1590 [] = {
    0xe9, 0xcf, 
};
static const unsigned char gb_chars_ban1591 [] = {
    0xc7, 0xc1, 
};
static const unsigned char gb_chars_ban1592 [] = {
    0xe9, 0xd2, 
};
static const unsigned char gb_chars_ban1593 [] = {
    0xe9, 0xd9, 0xb3, 0xc8, 
};
static const unsigned char gb_chars_ban1594 [] = {
    0xe9, 0xd3, 
};
static const unsigned char gb_chars_ban1595 [] = {
    0xcf, 0xf0, 
};
static const unsigned char gb_chars_ban1596 [] = {
    0xe9, 0xcd, 
};
static const unsigned char gb_chars_ban1597 [] = {
    0xb3, 0xf7, 
};
static const unsigned char gb_chars_ban1598 [] = {
    0xe9, 0xd6, 
};
static const unsigned char gb_chars_ban1599 [] = {
    0xe9, 0xda, 
};
static const unsigned char gb_chars_ban1600 [] = {
    0xcc, 0xb4, 
};
static const unsigned char gb_chars_ban1601 [] = {
    0xcf, 0xad, 
};
static const unsigned char gb_chars_ban1602 [] = {
    0xe9, 0xd5, 
};
static const unsigned char gb_chars_ban1603 [] = {
    0xe9, 0xdc, 0xe9, 0xdb, 
};
static const unsigned char gb_chars_ban1604 [] = {
    0xe9, 0xde, 
};
static const unsigned char gb_chars_ban1605 [] = {
    0xe9, 0xd1, 
};
static const unsigned char gb_chars_ban1606 [] = {
    0xe9, 0xdd, 
};
static const unsigned char gb_chars_ban1607 [] = {
    0xe9, 0xdf, 0xc3, 0xca, 
};
static const unsigned char gb_chars_ban1608 [] = {
    0xc7, 0xb7, 0xb4, 0xce, 0xbb, 0xb6, 0xd0, 0xc0, 0xec, 0xa3, 
};
static const unsigned char gb_chars_ban1609 [] = {
    0xc5, 0xb7, 
};
static const unsigned char gb_chars_ban1610 [] = {
    0xd3, 0xfb, 
};
static const unsigned char gb_chars_ban1611 [] = {
    0xec, 0xa4, 
};
static const unsigned char gb_chars_ban1612 [] = {
    0xec, 0xa5, 0xc6, 0xdb, 
};
static const unsigned char gb_chars_ban1613 [] = {
    0xbf, 0xee, 
};
static const unsigned char gb_chars_ban1614 [] = {
    0xec, 0xa6, 
};
static const unsigned char gb_chars_ban1615 [] = {
    0xec, 0xa7, 0xd0, 0xaa, 
};
static const unsigned char gb_chars_ban1616 [] = {
    0xc7, 0xb8, 
};
static const unsigned char gb_chars_ban1617 [] = {
    0xb8, 0xe8, 
};
static const unsigned char gb_chars_ban1618 [] = {
    0xec, 0xa8, 
};
static const unsigned char gb_chars_ban1619 [] = {
    0xd6, 0xb9, 0xd5, 0xfd, 0xb4, 0xcb, 0xb2, 0xbd, 0xce, 0xe4, 
    0xc6, 0xe7, 
};
static const unsigned char gb_chars_ban1620 [] = {
    0xcd, 0xe1, 
};
static const unsigned char gb_chars_ban1621 [] = {
    0xb4, 0xf5, 
};
static const unsigned char gb_chars_ban1622 [] = {
    0xcb, 0xc0, 0xbc, 0xdf, 
};
static const unsigned char gb_chars_ban1623 [] = {
    0xe9, 0xe2, 0xe9, 0xe3, 0xd1, 0xea, 0xe9, 0xe5, 
};
static const unsigned char gb_chars_ban1624 [] = {
    0xb4, 0xf9, 0xe9, 0xe4, 
};
static const unsigned char gb_chars_ban1625 [] = {
    0xd1, 0xb3, 0xca, 0xe2, 0xb2, 0xd0, 
};
static const unsigned char gb_chars_ban1626 [] = {
    0xe9, 0xe8, 
};
static const unsigned char gb_chars_ban1627 [] = {
    0xe9, 0xe6, 0xe9, 0xe7, 
};
static const unsigned char gb_chars_ban1628 [] = {
    0xd6, 0xb3, 
};
static const unsigned char gb_chars_ban1629 [] = {
    0xe9, 0xe9, 0xe9, 0xea, 
};
static const unsigned char gb_chars_ban1630 [] = {
    0xe9, 0xeb, 
};
static const unsigned char gb_chars_ban1631 [] = {
    0xe9, 0xec, 
};
static const unsigned char gb_chars_ban1632 [] = {
    0xec, 0xaf, 0xc5, 0xb9, 0xb6, 0xce, 
};
static const unsigned char gb_chars_ban1633 [] = {
    0xd2, 0xf3, 
};
static const unsigned char gb_chars_ban1634 [] = {
    0xb5, 0xee, 
};
static const unsigned char gb_chars_ban1635 [] = {
    0xbb, 0xd9, 0xec, 0xb1, 
};
static const unsigned char gb_chars_ban1636 [] = {
    0xd2, 0xe3, 
};
static const unsigned char gb_chars_ban1637 [] = {
    0xce, 0xe3, 
};
static const unsigned char gb_chars_ban1638 [] = {
    0xc4, 0xb8, 
};
static const unsigned char gb_chars_ban1639 [] = {
    0xc3, 0xbf, 
};
static const unsigned char gb_chars_ban1640 [] = {
    0xb6, 0xbe, 0xd8, 0xb9, 0xb1, 0xc8, 0xb1, 0xcf, 0xb1, 0xd1, 
    0xc5, 0xfe, 
};
static const unsigned char gb_chars_ban1641 [] = {
    0xb1, 0xd0, 
};
static const unsigned char gb_chars_ban1642 [] = {
    0xc3, 0xab, 
};
static const unsigned char gb_chars_ban1643 [] = {
    0xd5, 0xb1, 
};
static const unsigned char gb_chars_ban1644 [] = {
    0xeb, 0xa4, 0xba, 0xc1, 
};
static const unsigned char gb_chars_ban1645 [] = {
    0xcc, 0xba, 
};
static const unsigned char gb_chars_ban1646 [] = {
    0xeb, 0xa5, 
};
static const unsigned char gb_chars_ban1647 [] = {
    0xeb, 0xa7, 
};
static const unsigned char gb_chars_ban1648 [] = {
    0xeb, 0xa8, 
};
static const unsigned char gb_chars_ban1649 [] = {
    0xeb, 0xa6, 
};
static const unsigned char gb_chars_ban1650 [] = {
    0xeb, 0xa9, 0xeb, 0xab, 0xeb, 0xaa, 
};
static const unsigned char gb_chars_ban1651 [] = {
    0xeb, 0xac, 
};
static const unsigned char gb_chars_ban1652 [] = {
    0xca, 0xcf, 0xd8, 0xb5, 0xc3, 0xf1, 
};
static const unsigned char gb_chars_ban1653 [] = {
    0xc3, 0xa5, 0xc6, 0xf8, 0xeb, 0xad, 0xc4, 0xca, 
};
static const unsigned char gb_chars_ban1654 [] = {
    0xeb, 0xae, 0xeb, 0xaf, 0xeb, 0xb0, 0xb7, 0xd5, 
};
static const unsigned char gb_chars_ban1655 [] = {
    0xb7, 0xfa, 
};
static const unsigned char gb_chars_ban1656 [] = {
    0xeb, 0xb1, 0xc7, 0xe2, 
};
static const unsigned char gb_chars_ban1657 [] = {
    0xeb, 0xb3, 
};
static const unsigned char gb_chars_ban1658 [] = {
    0xba, 0xa4, 0xd1, 0xf5, 0xb0, 0xb1, 0xeb, 0xb2, 0xeb, 0xb4, 
};
static const unsigned char gb_chars_ban1659 [] = {
    0xb5, 0xaa, 0xc2, 0xc8, 0xc7, 0xe8, 
};
static const unsigned char gb_chars_ban1660 [] = {
    0xeb, 0xb5, 
};
static const unsigned char gb_chars_ban1661 [] = {
    0xcb, 0xae, 0xe3, 0xdf, 
};
static const unsigned char gb_chars_ban1662 [] = {
    0xd3, 0xc0, 
};
static const unsigned char gb_chars_ban1663 [] = {
    0xd9, 0xdb, 
};
static const unsigned char gb_chars_ban1664 [] = {
    0xcd, 0xa1, 0xd6, 0xad, 0xc7, 0xf3, 
};
static const unsigned char gb_chars_ban1665 [] = {
    0xd9, 0xe0, 0xbb, 0xe3, 
};
static const unsigned char gb_chars_ban1666 [] = {
    0xba, 0xba, 0xe3, 0xe2, 
};
static const unsigned char gb_chars_ban1667 [] = {
    0xcf, 0xab, 
};
static const unsigned char gb_chars_ban1668 [] = {
    0xe3, 0xe0, 0xc9, 0xc7, 
};
static const unsigned char gb_chars_ban1669 [] = {
    0xba, 0xb9, 
};
static const unsigned char gb_chars_ban1670 [] = {
    0xd1, 0xb4, 0xe3, 0xe1, 0xc8, 0xea, 0xb9, 0xaf, 0xbd, 0xad, 
    0xb3, 0xd8, 0xce, 0xdb, 
};
static const unsigned char gb_chars_ban1671 [] = {
    0xcc, 0xc0, 
};
static const unsigned char gb_chars_ban1672 [] = {
    0xe3, 0xe8, 0xe3, 0xe9, 0xcd, 0xf4, 
};
static const unsigned char gb_chars_ban1673 [] = {
    0xcc, 0xad, 
};
static const unsigned char gb_chars_ban1674 [] = {
    0xbc, 0xb3, 
};
static const unsigned char gb_chars_ban1675 [] = {
    0xe3, 0xea, 
};
static const unsigned char gb_chars_ban1676 [] = {
    0xe3, 0xeb, 
};
static const unsigned char gb_chars_ban1677 [] = {
    0xd0, 0xda, 
};
static const unsigned char gb_chars_ban1678 [] = {
    0xc6, 0xfb, 0xb7, 0xda, 
};
static const unsigned char gb_chars_ban1679 [] = {
    0xc7, 0xdf, 0xd2, 0xca, 0xce, 0xd6, 
};
static const unsigned char gb_chars_ban1680 [] = {
    0xe3, 0xe4, 0xe3, 0xec, 
};
static const unsigned char gb_chars_ban1681 [] = {
    0xc9, 0xf2, 0xb3, 0xc1, 
};
static const unsigned char gb_chars_ban1682 [] = {
    0xe3, 0xe7, 
};
static const unsigned char gb_chars_ban1683 [] = {
    0xc6, 0xe3, 0xe3, 0xe5, 
};
static const unsigned char gb_chars_ban1684 [] = {
    0xed, 0xb3, 0xe3, 0xe6, 
};
static const unsigned char gb_chars_ban1685 [] = {
    0xc9, 0xb3, 
};
static const unsigned char gb_chars_ban1686 [] = {
    0xc5, 0xe6, 
};
static const unsigned char gb_chars_ban1687 [] = {
    0xb9, 0xb5, 
};
static const unsigned char gb_chars_ban1688 [] = {
    0xc3, 0xbb, 
};
static const unsigned char gb_chars_ban1689 [] = {
    0xe3, 0xe3, 0xc5, 0xbd, 0xc1, 0xa4, 0xc2, 0xd9, 0xb2, 0xd7, 
};
static const unsigned char gb_chars_ban1690 [] = {
    0xe3, 0xed, 0xbb, 0xa6, 0xc4, 0xad, 
};
static const unsigned char gb_chars_ban1691 [] = {
    0xe3, 0xf0, 0xbe, 0xda, 
};
static const unsigned char gb_chars_ban1692 [] = {
    0xe3, 0xfb, 0xe3, 0xf5, 0xba, 0xd3, 
};
static const unsigned char gb_chars_ban1693 [] = {
    0xb7, 0xd0, 0xd3, 0xcd, 
};
static const unsigned char gb_chars_ban1694 [] = {
    0xd6, 0xce, 0xd5, 0xd3, 0xb9, 0xc1, 0xd5, 0xb4, 0xd1, 0xd8, 
};
static const unsigned char gb_chars_ban1695 [] = {
    0xd0, 0xb9, 0xc7, 0xf6, 
};
static const unsigned char gb_chars_ban1696 [] = {
    0xc8, 0xaa, 0xb2, 0xb4, 
};
static const unsigned char gb_chars_ban1697 [] = {
    0xc3, 0xda, 
};
static const unsigned char gb_chars_ban1698 [] = {
    0xe3, 0xee, 
};
static const unsigned char gb_chars_ban1699 [] = {
    0xe3, 0xfc, 0xe3, 0xef, 0xb7, 0xa8, 0xe3, 0xf7, 0xe3, 0xf4, 
};
static const unsigned char gb_chars_ban1700 [] = {
    0xb7, 0xba, 
};
static const unsigned char gb_chars_ban1701 [] = {
    0xc5, 0xa2, 
};
static const unsigned char gb_chars_ban1702 [] = {
    0xe3, 0xf6, 0xc5, 0xdd, 0xb2, 0xa8, 0xc6, 0xfc, 
};
static const unsigned char gb_chars_ban1703 [] = {
    0xc4, 0xe0, 
};
static const unsigned char gb_chars_ban1704 [] = {
    0xd7, 0xa2, 
};
static const unsigned char gb_chars_ban1705 [] = {
    0xc0, 0xe1, 0xe3, 0xf9, 
};
static const unsigned char gb_chars_ban1706 [] = {
    0xe3, 0xfa, 0xe3, 0xfd, 0xcc, 0xa9, 0xe3, 0xf3, 
};
static const unsigned char gb_chars_ban1707 [] = {
    0xd3, 0xbe, 
};
static const unsigned char gb_chars_ban1708 [] = {
    0xb1, 0xc3, 0xed, 0xb4, 0xe3, 0xf1, 0xe3, 0xf2, 
};
static const unsigned char gb_chars_ban1709 [] = {
    0xe3, 0xf8, 0xd0, 0xba, 0xc6, 0xc3, 0xd4, 0xf3, 0xe3, 0xfe, 
};
static const unsigned char gb_chars_ban1710 [] = {
    0xbd, 0xe0, 
};
static const unsigned char gb_chars_ban1711 [] = {
    0xe4, 0xa7, 
};
static const unsigned char gb_chars_ban1712 [] = {
    0xe4, 0xa6, 
};
static const unsigned char gb_chars_ban1713 [] = {
    0xd1, 0xf3, 0xe4, 0xa3, 
};
static const unsigned char gb_chars_ban1714 [] = {
    0xe4, 0xa9, 
};
static const unsigned char gb_chars_ban1715 [] = {
    0xc8, 0xf7, 
};
static const unsigned char gb_chars_ban1716 [] = {
    0xcf, 0xb4, 
};
static const unsigned char gb_chars_ban1717 [] = {
    0xe4, 0xa8, 0xe4, 0xae, 0xc2, 0xe5, 
};
static const unsigned char gb_chars_ban1718 [] = {
    0xb6, 0xb4, 
};
static const unsigned char gb_chars_ban1719 [] = {
    0xbd, 0xf2, 
};
static const unsigned char gb_chars_ban1720 [] = {
    0xe4, 0xa2, 
};
static const unsigned char gb_chars_ban1721 [] = {
    0xba, 0xe9, 0xe4, 0xaa, 
};
static const unsigned char gb_chars_ban1722 [] = {
    0xe4, 0xac, 
};
static const unsigned char gb_chars_ban1723 [] = {
    0xb6, 0xfd, 0xd6, 0xde, 0xe4, 0xb2, 
};
static const unsigned char gb_chars_ban1724 [] = {
    0xe4, 0xad, 
};
static const unsigned char gb_chars_ban1725 [] = {
    0xe4, 0xa1, 
};
static const unsigned char gb_chars_ban1726 [] = {
    0xbb, 0xee, 0xcd, 0xdd, 0xc7, 0xa2, 0xc5, 0xc9, 
};
static const unsigned char gb_chars_ban1727 [] = {
    0xc1, 0xf7, 
};
static const unsigned char gb_chars_ban1728 [] = {
    0xe4, 0xa4, 
};
static const unsigned char gb_chars_ban1729 [] = {
    0xc7, 0xb3, 0xbd, 0xac, 0xbd, 0xbd, 0xe4, 0xa5, 
};
static const unsigned char gb_chars_ban1730 [] = {
    0xd7, 0xc7, 0xb2, 0xe2, 
};
static const unsigned char gb_chars_ban1731 [] = {
    0xe4, 0xab, 0xbc, 0xc3, 0xe4, 0xaf, 
};
static const unsigned char gb_chars_ban1732 [] = {
    0xbb, 0xeb, 0xe4, 0xb0, 0xc5, 0xa8, 0xe4, 0xb1, 
};
static const unsigned char gb_chars_ban1733 [] = {
    0xd5, 0xe3, 0xbf, 0xa3, 
};
static const unsigned char gb_chars_ban1734 [] = {
    0xe4, 0xba, 
};
static const unsigned char gb_chars_ban1735 [] = {
    0xe4, 0xb7, 
};
static const unsigned char gb_chars_ban1736 [] = {
    0xe4, 0xbb, 
};
static const unsigned char gb_chars_ban1737 [] = {
    0xe4, 0xbd, 
};
static const unsigned char gb_chars_ban1738 [] = {
    0xc6, 0xd6, 
};
static const unsigned char gb_chars_ban1739 [] = {
    0xba, 0xc6, 0xc0, 0xcb, 
};
static const unsigned char gb_chars_ban1740 [] = {
    0xb8, 0xa1, 0xe4, 0xb4, 
};
static const unsigned char gb_chars_ban1741 [] = {
    0xd4, 0xa1, 
};
static const unsigned char gb_chars_ban1742 [] = {
    0xba, 0xa3, 0xbd, 0xfe, 
};
static const unsigned char gb_chars_ban1743 [] = {
    0xe4, 0xbc, 
};
static const unsigned char gb_chars_ban1744 [] = {
    0xcd, 0xbf, 
};
static const unsigned char gb_chars_ban1745 [] = {
    0xc4, 0xf9, 
};
static const unsigned char gb_chars_ban1746 [] = {
    0xcf, 0xfb, 0xc9, 0xe6, 
};
static const unsigned char gb_chars_ban1747 [] = {
    0xd3, 0xbf, 
};
static const unsigned char gb_chars_ban1748 [] = {
    0xcf, 0xd1, 
};
static const unsigned char gb_chars_ban1749 [] = {
    0xe4, 0xb3, 
};
static const unsigned char gb_chars_ban1750 [] = {
    0xe4, 0xb8, 0xe4, 0xb9, 0xcc, 0xe9, 
};
static const unsigned char gb_chars_ban1751 [] = {
    0xcc, 0xce, 
};
static const unsigned char gb_chars_ban1752 [] = {
    0xc0, 0xd4, 0xe4, 0xb5, 0xc1, 0xb0, 0xe4, 0xb6, 0xce, 0xd0, 
};
static const unsigned char gb_chars_ban1753 [] = {
    0xbb, 0xc1, 0xb5, 0xd3, 
};
static const unsigned char gb_chars_ban1754 [] = {
    0xc8, 0xf3, 0xbd, 0xa7, 0xd5, 0xc7, 0xc9, 0xac, 0xb8, 0xa2, 
    0xe4, 0xca, 
};
static const unsigned char gb_chars_ban1755 [] = {
    0xe4, 0xcc, 0xd1, 0xc4, 
};
static const unsigned char gb_chars_ban1756 [] = {
    0xd2, 0xba, 
};
static const unsigned char gb_chars_ban1757 [] = {
    0xba, 0xad, 
};
static const unsigned char gb_chars_ban1758 [] = {
    0xba, 0xd4, 
};
static const unsigned char gb_chars_ban1759 [] = {
    0xe4, 0xc3, 0xb5, 0xed, 
};
static const unsigned char gb_chars_ban1760 [] = {
    0xd7, 0xcd, 0xe4, 0xc0, 0xcf, 0xfd, 0xe4, 0xbf, 
};
static const unsigned char gb_chars_ban1761 [] = {
    0xc1, 0xdc, 0xcc, 0xca, 
};
static const unsigned char gb_chars_ban1762 [] = {
    0xca, 0xe7, 
};
static const unsigned char gb_chars_ban1763 [] = {
    0xc4, 0xd7, 
};
static const unsigned char gb_chars_ban1764 [] = {
    0xcc, 0xd4, 0xe4, 0xc8, 
};
static const unsigned char gb_chars_ban1765 [] = {
    0xe4, 0xc7, 0xe4, 0xc1, 
};
static const unsigned char gb_chars_ban1766 [] = {
    0xe4, 0xc4, 0xb5, 0xad, 
};
static const unsigned char gb_chars_ban1767 [] = {
    0xd3, 0xd9, 
};
static const unsigned char gb_chars_ban1768 [] = {
    0xe4, 0xc6, 
};
static const unsigned char gb_chars_ban1769 [] = {
    0xd2, 0xf9, 0xb4, 0xe3, 
};
static const unsigned char gb_chars_ban1770 [] = {
    0xbb, 0xb4, 
};
static const unsigned char gb_chars_ban1771 [] = {
    0xc9, 0xee, 
};
static const unsigned char gb_chars_ban1772 [] = {
    0xb4, 0xbe, 
};
static const unsigned char gb_chars_ban1773 [] = {
    0xbb, 0xec, 
};
static const unsigned char gb_chars_ban1774 [] = {
    0xd1, 0xcd, 
};
static const unsigned char gb_chars_ban1775 [] = {
    0xcc, 0xed, 0xed, 0xb5, 
};
static const unsigned char gb_chars_ban1776 [] = {
    0xc7, 0xe5, 
};
static const unsigned char gb_chars_ban1777 [] = {
    0xd4, 0xa8, 
};
static const unsigned char gb_chars_ban1778 [] = {
    0xe4, 0xcb, 0xd7, 0xd5, 0xe4, 0xc2, 
};
static const unsigned char gb_chars_ban1779 [] = {
    0xbd, 0xa5, 0xe4, 0xc5, 
};
static const unsigned char gb_chars_ban1780 [] = {
    0xd3, 0xe6, 
};
static const unsigned char gb_chars_ban1781 [] = {
    0xe4, 0xc9, 0xc9, 0xf8, 
};
static const unsigned char gb_chars_ban1782 [] = {
    0xe4, 0xbe, 
};
static const unsigned char gb_chars_ban1783 [] = {
    0xd3, 0xe5, 
};
static const unsigned char gb_chars_ban1784 [] = {
    0xc7, 0xfe, 0xb6, 0xc9, 
};
static const unsigned char gb_chars_ban1785 [] = {
    0xd4, 0xfc, 0xb2, 0xb3, 0xe4, 0xd7, 
};
static const unsigned char gb_chars_ban1786 [] = {
    0xce, 0xc2, 
};
static const unsigned char gb_chars_ban1787 [] = {
    0xe4, 0xcd, 
};
static const unsigned char gb_chars_ban1788 [] = {
    0xce, 0xbc, 
};
static const unsigned char gb_chars_ban1789 [] = {
    0xb8, 0xdb, 
};
static const unsigned char gb_chars_ban1790 [] = {
    0xe4, 0xd6, 
};
static const unsigned char gb_chars_ban1791 [] = {
    0xbf, 0xca, 
};
static const unsigned char gb_chars_ban1792 [] = {
    0xd3, 0xce, 
};
static const unsigned char gb_chars_ban1793 [] = {
    0xc3, 0xec, 
};
static const unsigned char gb_chars_ban1794 [] = {
    0xc5, 0xc8, 0xe4, 0xd8, 
};
static const unsigned char gb_chars_ban1795 [] = {
    0xcd, 0xc4, 0xe4, 0xcf, 
};
static const unsigned char gb_chars_ban1796 [] = {
    0xe4, 0xd4, 0xe4, 0xd5, 
};
static const unsigned char gb_chars_ban1797 [] = {
    0xba, 0xfe, 
};
static const unsigned char gb_chars_ban1798 [] = {
    0xcf, 0xe6, 
};
static const unsigned char gb_chars_ban1799 [] = {
    0xd5, 0xbf, 
};
static const unsigned char gb_chars_ban1800 [] = {
    0xe4, 0xd2, 
};
static const unsigned char gb_chars_ban1801 [] = {
    0xe4, 0xd0, 
};
static const unsigned char gb_chars_ban1802 [] = {
    0xe4, 0xce, 
};
static const unsigned char gb_chars_ban1803 [] = {
    0xcd, 0xe5, 0xca, 0xaa, 
};
static const unsigned char gb_chars_ban1804 [] = {
    0xc0, 0xa3, 
};
static const unsigned char gb_chars_ban1805 [] = {
    0xbd, 0xa6, 0xe4, 0xd3, 
};
static const unsigned char gb_chars_ban1806 [] = {
    0xb8, 0xc8, 
};
static const unsigned char gb_chars_ban1807 [] = {
    0xe4, 0xe7, 0xd4, 0xb4, 
};
static const unsigned char gb_chars_ban1808 [] = {
    0xe4, 0xdb, 
};
static const unsigned char gb_chars_ban1809 [] = {
    0xc1, 0xef, 
};
static const unsigned char gb_chars_ban1810 [] = {
    0xe4, 0xe9, 
};
static const unsigned char gb_chars_ban1811 [] = {
    0xd2, 0xe7, 
};
static const unsigned char gb_chars_ban1812 [] = {
    0xe4, 0xdf, 
};
static const unsigned char gb_chars_ban1813 [] = {
    0xe4, 0xe0, 
};
static const unsigned char gb_chars_ban1814 [] = {
    0xcf, 0xaa, 
};
static const unsigned char gb_chars_ban1815 [] = {
    0xcb, 0xdd, 
};
static const unsigned char gb_chars_ban1816 [] = {
    0xe4, 0xda, 0xe4, 0xd1, 
};
static const unsigned char gb_chars_ban1817 [] = {
    0xe4, 0xe5, 
};
static const unsigned char gb_chars_ban1818 [] = {
    0xc8, 0xdc, 0xe4, 0xe3, 
};
static const unsigned char gb_chars_ban1819 [] = {
    0xc4, 0xe7, 0xe4, 0xe2, 
};
static const unsigned char gb_chars_ban1820 [] = {
    0xe4, 0xe1, 
};
static const unsigned char gb_chars_ban1821 [] = {
    0xb3, 0xfc, 0xe4, 0xe8, 
};
static const unsigned char gb_chars_ban1822 [] = {
    0xb5, 0xe1, 
};
static const unsigned char gb_chars_ban1823 [] = {
    0xd7, 0xcc, 
};
static const unsigned char gb_chars_ban1824 [] = {
    0xe4, 0xe6, 
};
static const unsigned char gb_chars_ban1825 [] = {
    0xbb, 0xac, 
};
static const unsigned char gb_chars_ban1826 [] = {
    0xd7, 0xd2, 0xcc, 0xcf, 0xeb, 0xf8, 
};
static const unsigned char gb_chars_ban1827 [] = {
    0xe4, 0xe4, 
};
static const unsigned char gb_chars_ban1828 [] = {
    0xb9, 0xf6, 
};
static const unsigned char gb_chars_ban1829 [] = {
    0xd6, 0xcd, 0xe4, 0xd9, 0xe4, 0xdc, 0xc2, 0xfa, 0xe4, 0xde, 
};
static const unsigned char gb_chars_ban1830 [] = {
    0xc2, 0xcb, 0xc0, 0xc4, 0xc2, 0xd0, 
};
static const unsigned char gb_chars_ban1831 [] = {
    0xb1, 0xf5, 0xcc, 0xb2, 
};
static const unsigned char gb_chars_ban1832 [] = {
    0xb5, 0xce, 
};
static const unsigned char gb_chars_ban1833 [] = {
    0xe4, 0xef, 
};
static const unsigned char gb_chars_ban1834 [] = {
    0xc6, 0xaf, 
};
static const unsigned char gb_chars_ban1835 [] = {
    0xc6, 0xe1, 
};
static const unsigned char gb_chars_ban1836 [] = {
    0xe4, 0xf5, 
};
static const unsigned char gb_chars_ban1837 [] = {
    0xc2, 0xa9, 
};
static const unsigned char gb_chars_ban1838 [] = {
    0xc0, 0xec, 0xd1, 0xdd, 0xe4, 0xee, 
};
static const unsigned char gb_chars_ban1839 [] = {
    0xc4, 0xae, 
};
static const unsigned char gb_chars_ban1840 [] = {
    0xe4, 0xed, 
};
static const unsigned char gb_chars_ban1841 [] = {
    0xe4, 0xf6, 0xe4, 0xf4, 0xc2, 0xfe, 
};
static const unsigned char gb_chars_ban1842 [] = {
    0xe4, 0xdd, 
};
static const unsigned char gb_chars_ban1843 [] = {
    0xe4, 0xf0, 
};
static const unsigned char gb_chars_ban1844 [] = {
    0xca, 0xfe, 
};
static const unsigned char gb_chars_ban1845 [] = {
    0xd5, 0xc4, 
};
static const unsigned char gb_chars_ban1846 [] = {
    0xe4, 0xf1, 
};
static const unsigned char gb_chars_ban1847 [] = {
    0xd1, 0xfa, 
};
static const unsigned char gb_chars_ban1848 [] = {
    0xe4, 0xeb, 0xe4, 0xec, 
};
static const unsigned char gb_chars_ban1849 [] = {
    0xe4, 0xf2, 
};
static const unsigned char gb_chars_ban1850 [] = {
    0xce, 0xab, 
};
static const unsigned char gb_chars_ban1851 [] = {
    0xc5, 0xcb, 
};
static const unsigned char gb_chars_ban1852 [] = {
    0xc7, 0xb1, 
};
static const unsigned char gb_chars_ban1853 [] = {
    0xc2, 0xba, 
};
static const unsigned char gb_chars_ban1854 [] = {
    0xe4, 0xea, 
};
static const unsigned char gb_chars_ban1855 [] = {
    0xc1, 0xca, 
};
static const unsigned char gb_chars_ban1856 [] = {
    0xcc, 0xb6, 0xb3, 0xb1, 
};
static const unsigned char gb_chars_ban1857 [] = {
    0xe4, 0xfb, 
};
static const unsigned char gb_chars_ban1858 [] = {
    0xe4, 0xf3, 
};
static const unsigned char gb_chars_ban1859 [] = {
    0xe4, 0xfa, 
};
static const unsigned char gb_chars_ban1860 [] = {
    0xe4, 0xfd, 
};
static const unsigned char gb_chars_ban1861 [] = {
    0xe4, 0xfc, 
};
static const unsigned char gb_chars_ban1862 [] = {
    0xb3, 0xce, 
};
static const unsigned char gb_chars_ban1863 [] = {
    0xb3, 0xba, 0xe4, 0xf7, 
};
static const unsigned char gb_chars_ban1864 [] = {
    0xe4, 0xf9, 0xe4, 0xf8, 0xc5, 0xec, 
};
static const unsigned char gb_chars_ban1865 [] = {
    0xc0, 0xbd, 
};
static const unsigned char gb_chars_ban1866 [] = {
    0xd4, 0xe8, 
};
static const unsigned char gb_chars_ban1867 [] = {
    0xe5, 0xa2, 
};
static const unsigned char gb_chars_ban1868 [] = {
    0xb0, 0xc4, 
};
static const unsigned char gb_chars_ban1869 [] = {
    0xe5, 0xa4, 
};
static const unsigned char gb_chars_ban1870 [] = {
    0xe5, 0xa3, 
};
static const unsigned char gb_chars_ban1871 [] = {
    0xbc, 0xa4, 
};
static const unsigned char gb_chars_ban1872 [] = {
    0xe5, 0xa5, 
};
static const unsigned char gb_chars_ban1873 [] = {
    0xe5, 0xa1, 
};
static const unsigned char gb_chars_ban1874 [] = {
    0xe4, 0xfe, 0xb1, 0xf4, 
};
static const unsigned char gb_chars_ban1875 [] = {
    0xe5, 0xa8, 
};
static const unsigned char gb_chars_ban1876 [] = {
    0xe5, 0xa9, 0xe5, 0xa6, 
};
static const unsigned char gb_chars_ban1877 [] = {
    0xe5, 0xa7, 0xe5, 0xaa, 
};
static const unsigned char gb_chars_ban1878 [] = {
    0xc6, 0xd9, 
};
static const unsigned char gb_chars_ban1879 [] = {
    0xe5, 0xab, 0xe5, 0xad, 
};
static const unsigned char gb_chars_ban1880 [] = {
    0xe5, 0xac, 
};
static const unsigned char gb_chars_ban1881 [] = {
    0xe5, 0xaf, 
};
static const unsigned char gb_chars_ban1882 [] = {
    0xe5, 0xae, 
};
static const unsigned char gb_chars_ban1883 [] = {
    0xb9, 0xe0, 
};
static const unsigned char gb_chars_ban1884 [] = {
    0xe5, 0xb0, 
};
static const unsigned char gb_chars_ban1885 [] = {
    0xe5, 0xb1, 
};
static const unsigned char gb_chars_ban1886 [] = {
    0xbb, 0xf0, 0xec, 0xe1, 0xc3, 0xf0, 
};
static const unsigned char gb_chars_ban1887 [] = {
    0xb5, 0xc6, 0xbb, 0xd2, 
};
static const unsigned char gb_chars_ban1888 [] = {
    0xc1, 0xe9, 0xd4, 0xee, 
};
static const unsigned char gb_chars_ban1889 [] = {
    0xbe, 0xc4, 
};
static const unsigned char gb_chars_ban1890 [] = {
    0xd7, 0xc6, 
};
static const unsigned char gb_chars_ban1891 [] = {
    0xd4, 0xd6, 0xb2, 0xd3, 0xec, 0xbe, 
};
static const unsigned char gb_chars_ban1892 [] = {
    0xea, 0xc1, 
};
static const unsigned char gb_chars_ban1893 [] = {
    0xc2, 0xaf, 0xb4, 0xb6, 
};
static const unsigned char gb_chars_ban1894 [] = {
    0xd1, 0xd7, 
};
static const unsigned char gb_chars_ban1895 [] = {
    0xb3, 0xb4, 
};
static const unsigned char gb_chars_ban1896 [] = {
    0xc8, 0xb2, 0xbf, 0xbb, 0xec, 0xc0, 
};
static const unsigned char gb_chars_ban1897 [] = {
    0xd6, 0xcb, 
};
static const unsigned char gb_chars_ban1898 [] = {
    0xec, 0xbf, 0xec, 0xc1, 
};
static const unsigned char gb_chars_ban1899 [] = {
    0xec, 0xc5, 0xbe, 0xe6, 0xcc, 0xbf, 0xc5, 0xda, 0xbe, 0xbc, 
};
static const unsigned char gb_chars_ban1900 [] = {
    0xec, 0xc6, 
};
static const unsigned char gb_chars_ban1901 [] = {
    0xb1, 0xfe, 
};
static const unsigned char gb_chars_ban1902 [] = {
    0xec, 0xc4, 0xd5, 0xa8, 0xb5, 0xe3, 
};
static const unsigned char gb_chars_ban1903 [] = {
    0xec, 0xc2, 0xc1, 0xb6, 0xb3, 0xe3, 
};
static const unsigned char gb_chars_ban1904 [] = {
    0xec, 0xc3, 0xcb, 0xb8, 0xc0, 0xc3, 0xcc, 0xfe, 
};
static const unsigned char gb_chars_ban1905 [] = {
    0xc1, 0xd2, 
};
static const unsigned char gb_chars_ban1906 [] = {
    0xec, 0xc8, 
};
static const unsigned char gb_chars_ban1907 [] = {
    0xba, 0xe6, 0xc0, 0xd3, 
};
static const unsigned char gb_chars_ban1908 [] = {
    0xd6, 0xf2, 
};
static const unsigned char gb_chars_ban1909 [] = {
    0xd1, 0xcc, 
};
static const unsigned char gb_chars_ban1910 [] = {
    0xbf, 0xbe, 
};
static const unsigned char gb_chars_ban1911 [] = {
    0xb7, 0xb3, 0xc9, 0xd5, 0xec, 0xc7, 0xbb, 0xe2, 
};
static const unsigned char gb_chars_ban1912 [] = {
    0xcc, 0xcc, 0xbd, 0xfd, 0xc8, 0xc8, 
};
static const unsigned char gb_chars_ban1913 [] = {
    0xcf, 0xa9, 
};
static const unsigned char gb_chars_ban1914 [] = {
    0xcd, 0xe9, 
};
static const unsigned char gb_chars_ban1915 [] = {
    0xc5, 0xeb, 
};
static const unsigned char gb_chars_ban1916 [] = {
    0xb7, 0xe9, 
};
static const unsigned char gb_chars_ban1917 [] = {
    0xd1, 0xc9, 0xba, 0xb8, 
};
static const unsigned char gb_chars_ban1918 [] = {
    0xec, 0xc9, 
};
static const unsigned char gb_chars_ban1919 [] = {
    0xec, 0xca, 
};
static const unsigned char gb_chars_ban1920 [] = {
    0xbb, 0xc0, 0xec, 0xcb, 
};
static const unsigned char gb_chars_ban1921 [] = {
    0xec, 0xe2, 0xb1, 0xba, 0xb7, 0xd9, 
};
static const unsigned char gb_chars_ban1922 [] = {
    0xbd, 0xb9, 
};
static const unsigned char gb_chars_ban1923 [] = {
    0xec, 0xcc, 0xd1, 0xe6, 0xec, 0xcd, 
};
static const unsigned char gb_chars_ban1924 [] = {
    0xc8, 0xbb, 
};
static const unsigned char gb_chars_ban1925 [] = {
    0xec, 0xd1, 
};
static const unsigned char gb_chars_ban1926 [] = {
    0xec, 0xd3, 
};
static const unsigned char gb_chars_ban1927 [] = {
    0xbb, 0xcd, 
};
static const unsigned char gb_chars_ban1928 [] = {
    0xbc, 0xe5, 
};
static const unsigned char gb_chars_ban1929 [] = {
    0xec, 0xcf, 
};
static const unsigned char gb_chars_ban1930 [] = {
    0xc9, 0xb7, 
};
static const unsigned char gb_chars_ban1931 [] = {
    0xc3, 0xba, 
};
static const unsigned char gb_chars_ban1932 [] = {
    0xec, 0xe3, 0xd5, 0xd5, 0xec, 0xd0, 
};
static const unsigned char gb_chars_ban1933 [] = {
    0xd6, 0xf3, 
};
static const unsigned char gb_chars_ban1934 [] = {
    0xec, 0xd2, 0xec, 0xce, 
};
static const unsigned char gb_chars_ban1935 [] = {
    0xec, 0xd4, 
};
static const unsigned char gb_chars_ban1936 [] = {
    0xec, 0xd5, 
};
static const unsigned char gb_chars_ban1937 [] = {
    0xc9, 0xbf, 
};
static const unsigned char gb_chars_ban1938 [] = {
    0xcf, 0xa8, 
};
static const unsigned char gb_chars_ban1939 [] = {
    0xd0, 0xdc, 
};
static const unsigned char gb_chars_ban1940 [] = {
    0xd1, 0xac, 
};
static const unsigned char gb_chars_ban1941 [] = {
    0xc8, 0xdb, 
};
static const unsigned char gb_chars_ban1942 [] = {
    0xec, 0xd6, 0xce, 0xf5, 
};
static const unsigned char gb_chars_ban1943 [] = {
    0xca, 0xec, 0xec, 0xda, 
};
static const unsigned char gb_chars_ban1944 [] = {
    0xec, 0xd9, 
};
static const unsigned char gb_chars_ban1945 [] = {
    0xb0, 0xbe, 
};
static const unsigned char gb_chars_ban1946 [] = {
    0xec, 0xd7, 
};
static const unsigned char gb_chars_ban1947 [] = {
    0xec, 0xd8, 
};
static const unsigned char gb_chars_ban1948 [] = {
    0xec, 0xe4, 
};
static const unsigned char gb_chars_ban1949 [] = {
    0xc8, 0xbc, 
};
static const unsigned char gb_chars_ban1950 [] = {
    0xc1, 0xc7, 
};
static const unsigned char gb_chars_ban1951 [] = {
    0xec, 0xdc, 0xd1, 0xe0, 
};
static const unsigned char gb_chars_ban1952 [] = {
    0xec, 0xdb, 
};
static const unsigned char gb_chars_ban1953 [] = {
    0xd4, 0xef, 
};
static const unsigned char gb_chars_ban1954 [] = {
    0xec, 0xdd, 
};
static const unsigned char gb_chars_ban1955 [] = {
    0xdb, 0xc6, 
};
static const unsigned char gb_chars_ban1956 [] = {
    0xec, 0xde, 
};
static const unsigned char gb_chars_ban1957 [] = {
    0xb1, 0xac, 
};
static const unsigned char gb_chars_ban1958 [] = {
    0xec, 0xdf, 
};
static const unsigned char gb_chars_ban1959 [] = {
    0xec, 0xe0, 
};
static const unsigned char gb_chars_ban1960 [] = {
    0xd7, 0xa6, 
};
static const unsigned char gb_chars_ban1961 [] = {
    0xc5, 0xc0, 
};
static const unsigned char gb_chars_ban1962 [] = {
    0xeb, 0xbc, 0xb0, 0xae, 
};
static const unsigned char gb_chars_ban1963 [] = {
    0xbe, 0xf4, 0xb8, 0xb8, 0xd2, 0xaf, 0xb0, 0xd6, 0xb5, 0xf9, 
};
static const unsigned char gb_chars_ban1964 [] = {
    0xd8, 0xb3, 
};
static const unsigned char gb_chars_ban1965 [] = {
    0xcb, 0xac, 
};
static const unsigned char gb_chars_ban1966 [] = {
    0xe3, 0xdd, 
};
static const unsigned char gb_chars_ban1967 [] = {
    0xc6, 0xac, 0xb0, 0xe6, 
};
static const unsigned char gb_chars_ban1968 [] = {
    0xc5, 0xc6, 0xeb, 0xb9, 
};
static const unsigned char gb_chars_ban1969 [] = {
    0xeb, 0xba, 
};
static const unsigned char gb_chars_ban1970 [] = {
    0xeb, 0xbb, 
};
static const unsigned char gb_chars_ban1971 [] = {
    0xd1, 0xc0, 
};
static const unsigned char gb_chars_ban1972 [] = {
    0xc5, 0xa3, 
};
static const unsigned char gb_chars_ban1973 [] = {
    0xea, 0xf2, 
};
static const unsigned char gb_chars_ban1974 [] = {
    0xc4, 0xb2, 
};
static const unsigned char gb_chars_ban1975 [] = {
    0xc4, 0xb5, 0xc0, 0xce, 
};
static const unsigned char gb_chars_ban1976 [] = {
    0xea, 0xf3, 0xc4, 0xc1, 
};
static const unsigned char gb_chars_ban1977 [] = {
    0xce, 0xef, 
};
static const unsigned char gb_chars_ban1978 [] = {
    0xea, 0xf0, 0xea, 0xf4, 
};
static const unsigned char gb_chars_ban1979 [] = {
    0xc9, 0xfc, 
};
static const unsigned char gb_chars_ban1980 [] = {
    0xc7, 0xa3, 
};
static const unsigned char gb_chars_ban1981 [] = {
    0xcc, 0xd8, 0xce, 0xfe, 
};
static const unsigned char gb_chars_ban1982 [] = {
    0xea, 0xf5, 0xea, 0xf6, 0xcf, 0xac, 0xc0, 0xe7, 
};
static const unsigned char gb_chars_ban1983 [] = {
    0xea, 0xf7, 
};
static const unsigned char gb_chars_ban1984 [] = {
    0xb6, 0xbf, 0xea, 0xf8, 
};
static const unsigned char gb_chars_ban1985 [] = {
    0xea, 0xf9, 
};
static const unsigned char gb_chars_ban1986 [] = {
    0xea, 0xfa, 
};
static const unsigned char gb_chars_ban1987 [] = {
    0xea, 0xfb, 
};
static const unsigned char gb_chars_ban1988 [] = {
    0xea, 0xf1, 
};
static const unsigned char gb_chars_ban1989 [] = {
    0xc8, 0xae, 0xe1, 0xeb, 
};
static const unsigned char gb_chars_ban1990 [] = {
    0xb7, 0xb8, 0xe1, 0xec, 
};
static const unsigned char gb_chars_ban1991 [] = {
    0xe1, 0xed, 
};
static const unsigned char gb_chars_ban1992 [] = {
    0xd7, 0xb4, 0xe1, 0xee, 0xe1, 0xef, 0xd3, 0xcc, 
};
static const unsigned char gb_chars_ban1993 [] = {
    0xe1, 0xf1, 0xbf, 0xf1, 0xe1, 0xf0, 0xb5, 0xd2, 
};
static const unsigned char gb_chars_ban1994 [] = {
    0xb1, 0xb7, 
};
static const unsigned char gb_chars_ban1995 [] = {
    0xe1, 0xf3, 0xe1, 0xf2, 
};
static const unsigned char gb_chars_ban1996 [] = {
    0xba, 0xfc, 
};
static const unsigned char gb_chars_ban1997 [] = {
    0xe1, 0xf4, 
};
static const unsigned char gb_chars_ban1998 [] = {
    0xb9, 0xb7, 
};
static const unsigned char gb_chars_ban1999 [] = {
    0xbe, 0xd1, 
};
static const unsigned char gb_chars_ban2000 [] = {
    0xc4, 0xfc, 
};
static const unsigned char gb_chars_ban2001 [] = {
    0xba, 0xdd, 0xbd, 0xc6, 
};
static const unsigned char gb_chars_ban2002 [] = {
    0xe1, 0xf5, 0xe1, 0xf7, 
};
static const unsigned char gb_chars_ban2003 [] = {
    0xb6, 0xc0, 0xcf, 0xc1, 0xca, 0xa8, 0xe1, 0xf6, 0xd5, 0xf8, 
    0xd3, 0xfc, 0xe1, 0xf8, 0xe1, 0xfc, 0xe1, 0xf9, 
};
static const unsigned char gb_chars_ban2004 [] = {
    0xe1, 0xfa, 0xc0, 0xea, 
};
static const unsigned char gb_chars_ban2005 [] = {
    0xe1, 0xfe, 0xe2, 0xa1, 0xc0, 0xc7, 
};
static const unsigned char gb_chars_ban2006 [] = {
    0xe1, 0xfb, 
};
static const unsigned char gb_chars_ban2007 [] = {
    0xe1, 0xfd, 
};
static const unsigned char gb_chars_ban2008 [] = {
    0xe2, 0xa5, 
};
static const unsigned char gb_chars_ban2009 [] = {
    0xc1, 0xd4, 
};
static const unsigned char gb_chars_ban2010 [] = {
    0xe2, 0xa3, 
};
static const unsigned char gb_chars_ban2011 [] = {
    0xe2, 0xa8, 0xb2, 0xfe, 0xe2, 0xa2, 
};
static const unsigned char gb_chars_ban2012 [] = {
    0xc3, 0xcd, 0xb2, 0xc2, 0xe2, 0xa7, 0xe2, 0xa6, 
};
static const unsigned char gb_chars_ban2013 [] = {
    0xe2, 0xa4, 0xe2, 0xa9, 
};
static const unsigned char gb_chars_ban2014 [] = {
    0xe2, 0xab, 
};
static const unsigned char gb_chars_ban2015 [] = {
    0xd0, 0xc9, 0xd6, 0xed, 0xc3, 0xa8, 0xe2, 0xac, 
};
static const unsigned char gb_chars_ban2016 [] = {
    0xcf, 0xd7, 
};
static const unsigned char gb_chars_ban2017 [] = {
    0xe2, 0xae, 
};
static const unsigned char gb_chars_ban2018 [] = {
    0xba, 0xef, 
};
static const unsigned char gb_chars_ban2019 [] = {
    0xe9, 0xe0, 0xe2, 0xad, 0xe2, 0xaa, 
};
static const unsigned char gb_chars_ban2020 [] = {
    0xbb, 0xab, 0xd4, 0xb3, 
};
static const unsigned char gb_chars_ban2021 [] = {
    0xe2, 0xb0, 
};
static const unsigned char gb_chars_ban2022 [] = {
    0xe2, 0xaf, 
};
static const unsigned char gb_chars_ban2023 [] = {
    0xe9, 0xe1, 
};
static const unsigned char gb_chars_ban2024 [] = {
    0xe2, 0xb1, 
};
static const unsigned char gb_chars_ban2025 [] = {
    0xe2, 0xb2, 
};
static const unsigned char gb_chars_ban2026 [] = {
    0xe2, 0xb3, 0xcc, 0xa1, 
};
static const unsigned char gb_chars_ban2027 [] = {
    0xe2, 0xb4, 
};
static const unsigned char gb_chars_ban2028 [] = {
    0xe2, 0xb5, 
};
static const unsigned char gb_chars_ban2029 [] = {
    0xd0, 0xfe, 
};
static const unsigned char gb_chars_ban2030 [] = {
    0xc2, 0xca, 
};
static const unsigned char gb_chars_ban2031 [] = {
    0xd3, 0xf1, 
};
static const unsigned char gb_chars_ban2032 [] = {
    0xcd, 0xf5, 
};
static const unsigned char gb_chars_ban2033 [] = {
    0xe7, 0xe0, 
};
static const unsigned char gb_chars_ban2034 [] = {
    0xe7, 0xe1, 
};
static const unsigned char gb_chars_ban2035 [] = {
    0xbe, 0xc1, 
};
static const unsigned char gb_chars_ban2036 [] = {
    0xc2, 0xea, 
};
static const unsigned char gb_chars_ban2037 [] = {
    0xe7, 0xe4, 
};
static const unsigned char gb_chars_ban2038 [] = {
    0xe7, 0xe3, 
};
static const unsigned char gb_chars_ban2039 [] = {
    0xcd, 0xe6, 
};
static const unsigned char gb_chars_ban2040 [] = {
    0xc3, 0xb5, 
};
static const unsigned char gb_chars_ban2041 [] = {
    0xe7, 0xe2, 0xbb, 0xb7, 0xcf, 0xd6, 
};
static const unsigned char gb_chars_ban2042 [] = {
    0xc1, 0xe1, 0xe7, 0xe9, 
};
static const unsigned char gb_chars_ban2043 [] = {
    0xe7, 0xe8, 
};
static const unsigned char gb_chars_ban2044 [] = {
    0xe7, 0xf4, 0xb2, 0xa3, 
};
static const unsigned char gb_chars_ban2045 [] = {
    0xe7, 0xea, 
};
static const unsigned char gb_chars_ban2046 [] = {
    0xe7, 0xe6, 
};
static const unsigned char gb_chars_ban2047 [] = {
    0xe7, 0xec, 0xe7, 0xeb, 0xc9, 0xba, 
};
static const unsigned char gb_chars_ban2048 [] = {
    0xd5, 0xe4, 
};
static const unsigned char gb_chars_ban2049 [] = {
    0xe7, 0xe5, 0xb7, 0xa9, 0xe7, 0xe7, 
};
static const unsigned char gb_chars_ban2050 [] = {
    0xe7, 0xee, 
};
static const unsigned char gb_chars_ban2051 [] = {
    0xe7, 0xf3, 
};
static const unsigned char gb_chars_ban2052 [] = {
    0xd6, 0xe9, 
};
static const unsigned char gb_chars_ban2053 [] = {
    0xe7, 0xed, 
};
static const unsigned char gb_chars_ban2054 [] = {
    0xe7, 0xf2, 
};
static const unsigned char gb_chars_ban2055 [] = {
    0xe7, 0xf1, 
};
static const unsigned char gb_chars_ban2056 [] = {
    0xb0, 0xe0, 
};
static const unsigned char gb_chars_ban2057 [] = {
    0xe7, 0xf5, 
};
static const unsigned char gb_chars_ban2058 [] = {
    0xc7, 0xf2, 
};
static const unsigned char gb_chars_ban2059 [] = {
    0xc0, 0xc5, 0xc0, 0xed, 
};
static const unsigned char gb_chars_ban2060 [] = {
    0xc1, 0xf0, 0xe7, 0xf0, 
};
static const unsigned char gb_chars_ban2061 [] = {
    0xe7, 0xf6, 0xcb, 0xf6, 
};
static const unsigned char gb_chars_ban2062 [] = {
    0xe8, 0xa2, 0xe8, 0xa1, 
};
static const unsigned char gb_chars_ban2063 [] = {
    0xd7, 0xc1, 
};
static const unsigned char gb_chars_ban2064 [] = {
    0xe7, 0xfa, 0xe7, 0xf9, 
};
static const unsigned char gb_chars_ban2065 [] = {
    0xe7, 0xfb, 
};
static const unsigned char gb_chars_ban2066 [] = {
    0xe7, 0xf7, 
};
static const unsigned char gb_chars_ban2067 [] = {
    0xe7, 0xfe, 
};
static const unsigned char gb_chars_ban2068 [] = {
    0xe7, 0xfd, 
};
static const unsigned char gb_chars_ban2069 [] = {
    0xe7, 0xfc, 
};
static const unsigned char gb_chars_ban2070 [] = {
    0xc1, 0xd5, 0xc7, 0xd9, 0xc5, 0xfd, 0xc5, 0xc3, 
};
static const unsigned char gb_chars_ban2071 [] = {
    0xc7, 0xed, 
};
static const unsigned char gb_chars_ban2072 [] = {
    0xe8, 0xa3, 
};
static const unsigned char gb_chars_ban2073 [] = {
    0xe8, 0xa6, 
};
static const unsigned char gb_chars_ban2074 [] = {
    0xe8, 0xa5, 
};
static const unsigned char gb_chars_ban2075 [] = {
    0xe8, 0xa7, 0xba, 0xf7, 0xe7, 0xf8, 0xe8, 0xa4, 
};
static const unsigned char gb_chars_ban2076 [] = {
    0xc8, 0xf0, 0xc9, 0xaa, 
};
static const unsigned char gb_chars_ban2077 [] = {
    0xe8, 0xa9, 
};
static const unsigned char gb_chars_ban2078 [] = {
    0xb9, 0xe5, 
};
static const unsigned char gb_chars_ban2079 [] = {
    0xd1, 0xfe, 0xe8, 0xa8, 
};
static const unsigned char gb_chars_ban2080 [] = {
    0xe8, 0xaa, 
};
static const unsigned char gb_chars_ban2081 [] = {
    0xe8, 0xad, 0xe8, 0xae, 
};
static const unsigned char gb_chars_ban2082 [] = {
    0xc1, 0xa7, 
};
static const unsigned char gb_chars_ban2083 [] = {
    0xe8, 0xaf, 
};
static const unsigned char gb_chars_ban2084 [] = {
    0xe8, 0xb0, 
};
static const unsigned char gb_chars_ban2085 [] = {
    0xe8, 0xac, 
};
static const unsigned char gb_chars_ban2086 [] = {
    0xe8, 0xb4, 
};
static const unsigned char gb_chars_ban2087 [] = {
    0xe8, 0xab, 
};
static const unsigned char gb_chars_ban2088 [] = {
    0xe8, 0xb1, 
};
static const unsigned char gb_chars_ban2089 [] = {
    0xe8, 0xb5, 0xe8, 0xb2, 0xe8, 0xb3, 
};
static const unsigned char gb_chars_ban2090 [] = {
    0xe8, 0xb7, 
};
static const unsigned char gb_chars_ban2091 [] = {
    0xe8, 0xb6, 
};
static const unsigned char gb_chars_ban2092 [] = {
    0xb9, 0xcf, 
};
static const unsigned char gb_chars_ban2093 [] = {
    0xf0, 0xac, 
};
static const unsigned char gb_chars_ban2094 [] = {
    0xf0, 0xad, 
};
static const unsigned char gb_chars_ban2095 [] = {
    0xc6, 0xb0, 0xb0, 0xea, 0xc8, 0xbf, 
};
static const unsigned char gb_chars_ban2096 [] = {
    0xcd, 0xdf, 
};
static const unsigned char gb_chars_ban2097 [] = {
    0xce, 0xcd, 0xea, 0xb1, 
};
static const unsigned char gb_chars_ban2098 [] = {
    0xea, 0xb2, 
};
static const unsigned char gb_chars_ban2099 [] = {
    0xc6, 0xbf, 0xb4, 0xc9, 
};
static const unsigned char gb_chars_ban2100 [] = {
    0xea, 0xb3, 
};
static const unsigned char gb_chars_ban2101 [] = {
    0xd5, 0xe7, 
};
static const unsigned char gb_chars_ban2102 [] = {
    0xdd, 0xf9, 
};
static const unsigned char gb_chars_ban2103 [] = {
    0xea, 0xb4, 
};
static const unsigned char gb_chars_ban2104 [] = {
    0xea, 0xb5, 
};
static const unsigned char gb_chars_ban2105 [] = {
    0xea, 0xb6, 
};
static const unsigned char gb_chars_ban2106 [] = {
    0xb8, 0xca, 0xdf, 0xb0, 0xc9, 0xf5, 
};
static const unsigned char gb_chars_ban2107 [] = {
    0xcc, 0xf0, 
};
static const unsigned char gb_chars_ban2108 [] = {
    0xc9, 0xfa, 
};
static const unsigned char gb_chars_ban2109 [] = {
    0xc9, 0xfb, 
};
static const unsigned char gb_chars_ban2110 [] = {
    0xd3, 0xc3, 0xcb, 0xa6, 
};
static const unsigned char gb_chars_ban2111 [] = {
    0xb8, 0xa6, 0xf0, 0xae, 0xb1, 0xc2, 
};
static const unsigned char gb_chars_ban2112 [] = {
    0xe5, 0xb8, 0xcc, 0xef, 0xd3, 0xc9, 0xbc, 0xd7, 0xc9, 0xea, 
};
static const unsigned char gb_chars_ban2113 [] = {
    0xb5, 0xe7, 
};
static const unsigned char gb_chars_ban2114 [] = {
    0xc4, 0xd0, 0xb5, 0xe9, 
};
static const unsigned char gb_chars_ban2115 [] = {
    0xee, 0xae, 0xbb, 0xad, 
};
static const unsigned char gb_chars_ban2116 [] = {
    0xe7, 0xde, 
};
static const unsigned char gb_chars_ban2117 [] = {
    0xee, 0xaf, 
};
static const unsigned char gb_chars_ban2118 [] = {
    0xb3, 0xa9, 
};
static const unsigned char gb_chars_ban2119 [] = {
    0xee, 0xb2, 
};
static const unsigned char gb_chars_ban2120 [] = {
    0xee, 0xb1, 0xbd, 0xe7, 
};
static const unsigned char gb_chars_ban2121 [] = {
    0xee, 0xb0, 0xce, 0xb7, 
};
static const unsigned char gb_chars_ban2122 [] = {
    0xc5, 0xcf, 
};
static const unsigned char gb_chars_ban2123 [] = {
    0xc1, 0xf4, 0xdb, 0xce, 0xee, 0xb3, 0xd0, 0xf3, 
};
static const unsigned char gb_chars_ban2124 [] = {
    0xc2, 0xd4, 0xc6, 0xe8, 
};
static const unsigned char gb_chars_ban2125 [] = {
    0xb7, 0xac, 
};
static const unsigned char gb_chars_ban2126 [] = {
    0xee, 0xb4, 
};
static const unsigned char gb_chars_ban2127 [] = {
    0xb3, 0xeb, 
};
static const unsigned char gb_chars_ban2128 [] = {
    0xbb, 0xfb, 0xee, 0xb5, 
};
static const unsigned char gb_chars_ban2129 [] = {
    0xe7, 0xdc, 
};
static const unsigned char gb_chars_ban2130 [] = {
    0xee, 0xb6, 
};
static const unsigned char gb_chars_ban2131 [] = {
    0xbd, 0xae, 
};
static const unsigned char gb_chars_ban2132 [] = {
    0xf1, 0xe2, 
};
static const unsigned char gb_chars_ban2133 [] = {
    0xca, 0xe8, 
};
static const unsigned char gb_chars_ban2134 [] = {
    0xd2, 0xc9, 0xf0, 0xda, 
};
static const unsigned char gb_chars_ban2135 [] = {
    0xf0, 0xdb, 
};
static const unsigned char gb_chars_ban2136 [] = {
    0xf0, 0xdc, 0xc1, 0xc6, 
};
static const unsigned char gb_chars_ban2137 [] = {
    0xb8, 0xed, 0xbe, 0xce, 
};
static const unsigned char gb_chars_ban2138 [] = {
    0xf0, 0xde, 
};
static const unsigned char gb_chars_ban2139 [] = {
    0xc5, 0xb1, 0xf0, 0xdd, 0xd1, 0xf1, 
};
static const unsigned char gb_chars_ban2140 [] = {
    0xf0, 0xe0, 0xb0, 0xcc, 0xbd, 0xea, 
};
static const unsigned char gb_chars_ban2141 [] = {
    0xd2, 0xdf, 0xf0, 0xdf, 
};
static const unsigned char gb_chars_ban2142 [] = {
    0xb4, 0xaf, 0xb7, 0xe8, 0xf0, 0xe6, 0xf0, 0xe5, 0xc6, 0xa3, 
    0xf0, 0xe1, 0xf0, 0xe2, 0xb4, 0xc3, 
};
static const unsigned char gb_chars_ban2143 [] = {
    0xf0, 0xe3, 0xd5, 0xee, 
};
static const unsigned char gb_chars_ban2144 [] = {
    0xcc, 0xdb, 0xbe, 0xd2, 0xbc, 0xb2, 
};
static const unsigned char gb_chars_ban2145 [] = {
    0xf0, 0xe8, 0xf0, 0xe7, 0xf0, 0xe4, 0xb2, 0xa1, 
};
static const unsigned char gb_chars_ban2146 [] = {
    0xd6, 0xa2, 0xd3, 0xb8, 0xbe, 0xb7, 0xc8, 0xac, 
};
static const unsigned char gb_chars_ban2147 [] = {
    0xf0, 0xea, 
};
static const unsigned char gb_chars_ban2148 [] = {
    0xd1, 0xf7, 
};
static const unsigned char gb_chars_ban2149 [] = {
    0xd6, 0xcc, 0xba, 0xdb, 0xf0, 0xe9, 
};
static const unsigned char gb_chars_ban2150 [] = {
    0xb6, 0xbb, 
};
static const unsigned char gb_chars_ban2151 [] = {
    0xcd, 0xb4, 
};
static const unsigned char gb_chars_ban2152 [] = {
    0xc6, 0xa6, 
};
static const unsigned char gb_chars_ban2153 [] = {
    0xc1, 0xa1, 0xf0, 0xeb, 0xf0, 0xee, 
};
static const unsigned char gb_chars_ban2154 [] = {
    0xf0, 0xed, 0xf0, 0xf0, 0xf0, 0xec, 
};
static const unsigned char gb_chars_ban2155 [] = {
    0xbb, 0xbe, 0xf0, 0xef, 
};
static const unsigned char gb_chars_ban2156 [] = {
    0xcc, 0xb5, 0xf0, 0xf2, 
};
static const unsigned char gb_chars_ban2157 [] = {
    0xb3, 0xd5, 
};
static const unsigned char gb_chars_ban2158 [] = {
    0xb1, 0xd4, 
};
static const unsigned char gb_chars_ban2159 [] = {
    0xf0, 0xf3, 
};
static const unsigned char gb_chars_ban2160 [] = {
    0xf0, 0xf4, 0xf0, 0xf6, 0xb4, 0xe1, 
};
static const unsigned char gb_chars_ban2161 [] = {
    0xf0, 0xf1, 
};
static const unsigned char gb_chars_ban2162 [] = {
    0xf0, 0xf7, 
};
static const unsigned char gb_chars_ban2163 [] = {
    0xf0, 0xfa, 
};
static const unsigned char gb_chars_ban2164 [] = {
    0xf0, 0xf8, 
};
static const unsigned char gb_chars_ban2165 [] = {
    0xf0, 0xf5, 
};
static const unsigned char gb_chars_ban2166 [] = {
    0xf0, 0xfd, 
};
static const unsigned char gb_chars_ban2167 [] = {
    0xf0, 0xf9, 0xf0, 0xfc, 0xf0, 0xfe, 
};
static const unsigned char gb_chars_ban2168 [] = {
    0xf1, 0xa1, 
};
static const unsigned char gb_chars_ban2169 [] = {
    0xce, 0xc1, 0xf1, 0xa4, 
};
static const unsigned char gb_chars_ban2170 [] = {
    0xf1, 0xa3, 
};
static const unsigned char gb_chars_ban2171 [] = {
    0xc1, 0xf6, 0xf0, 0xfb, 0xca, 0xdd, 
};
static const unsigned char gb_chars_ban2172 [] = {
    0xb4, 0xf1, 0xb1, 0xf1, 0xcc, 0xb1, 
};
static const unsigned char gb_chars_ban2173 [] = {
    0xf1, 0xa6, 
};
static const unsigned char gb_chars_ban2174 [] = {
    0xf1, 0xa7, 
};
static const unsigned char gb_chars_ban2175 [] = {
    0xf1, 0xac, 0xd5, 0xce, 0xf1, 0xa9, 
};
static const unsigned char gb_chars_ban2176 [] = {
    0xc8, 0xb3, 
};
static const unsigned char gb_chars_ban2177 [] = {
    0xf1, 0xa2, 
};
static const unsigned char gb_chars_ban2178 [] = {
    0xf1, 0xab, 0xf1, 0xa8, 0xf1, 0xa5, 
};
static const unsigned char gb_chars_ban2179 [] = {
    0xf1, 0xaa, 
};
static const unsigned char gb_chars_ban2180 [] = {
    0xb0, 0xa9, 0xf1, 0xad, 
};
static const unsigned char gb_chars_ban2181 [] = {
    0xf1, 0xaf, 
};
static const unsigned char gb_chars_ban2182 [] = {
    0xf1, 0xb1, 
};
static const unsigned char gb_chars_ban2183 [] = {
    0xf1, 0xb0, 
};
static const unsigned char gb_chars_ban2184 [] = {
    0xf1, 0xae, 
};
static const unsigned char gb_chars_ban2185 [] = {
    0xd1, 0xa2, 
};
static const unsigned char gb_chars_ban2186 [] = {
    0xf1, 0xb2, 
};
static const unsigned char gb_chars_ban2187 [] = {
    0xf1, 0xb3, 
};
static const unsigned char gb_chars_ban2188 [] = {
    0xb9, 0xef, 
};
static const unsigned char gb_chars_ban2189 [] = {
    0xb5, 0xc7, 
};
static const unsigned char gb_chars_ban2190 [] = {
    0xb0, 0xd7, 0xb0, 0xd9, 
};
static const unsigned char gb_chars_ban2191 [] = {
    0xd4, 0xed, 
};
static const unsigned char gb_chars_ban2192 [] = {
    0xb5, 0xc4, 
};
static const unsigned char gb_chars_ban2193 [] = {
    0xbd, 0xd4, 0xbb, 0xca, 0xf0, 0xa7, 
};
static const unsigned char gb_chars_ban2194 [] = {
    0xb8, 0xde, 
};
static const unsigned char gb_chars_ban2195 [] = {
    0xf0, 0xa8, 
};
static const unsigned char gb_chars_ban2196 [] = {
    0xb0, 0xa8, 
};
static const unsigned char gb_chars_ban2197 [] = {
    0xf0, 0xa9, 
};
static const unsigned char gb_chars_ban2198 [] = {
    0xcd, 0xee, 
};
static const unsigned char gb_chars_ban2199 [] = {
    0xf0, 0xaa, 
};
static const unsigned char gb_chars_ban2200 [] = {
    0xf0, 0xab, 
};
static const unsigned char gb_chars_ban2201 [] = {
    0xc6, 0xa4, 
};
static const unsigned char gb_chars_ban2202 [] = {
    0xd6, 0xe5, 0xf1, 0xe4, 
};
static const unsigned char gb_chars_ban2203 [] = {
    0xf1, 0xe5, 
};
static const unsigned char gb_chars_ban2204 [] = {
    0xc3, 0xf3, 
};
static const unsigned char gb_chars_ban2205 [] = {
    0xd3, 0xdb, 
};
static const unsigned char gb_chars_ban2206 [] = {
    0xd6, 0xd1, 0xc5, 0xe8, 
};
static const unsigned char gb_chars_ban2207 [] = {
    0xd3, 0xaf, 
};
static const unsigned char gb_chars_ban2208 [] = {
    0xd2, 0xe6, 
};
static const unsigned char gb_chars_ban2209 [] = {
    0xee, 0xc1, 0xb0, 0xbb, 0xd5, 0xb5, 0xd1, 0xce, 0xbc, 0xe0, 
    0xba, 0xd0, 
};
static const unsigned char gb_chars_ban2210 [] = {
    0xbf, 0xf8, 
};
static const unsigned char gb_chars_ban2211 [] = {
    0xb8, 0xc7, 0xb5, 0xc1, 0xc5, 0xcc, 
};
static const unsigned char gb_chars_ban2212 [] = {
    0xca, 0xa2, 
};
static const unsigned char gb_chars_ban2213 [] = {
    0xc3, 0xcb, 
};
static const unsigned char gb_chars_ban2214 [] = {
    0xee, 0xc2, 
};
static const unsigned char gb_chars_ban2215 [] = {
    0xc4, 0xbf, 0xb6, 0xa2, 
};
static const unsigned char gb_chars_ban2216 [] = {
    0xed, 0xec, 0xc3, 0xa4, 
};
static const unsigned char gb_chars_ban2217 [] = {
    0xd6, 0xb1, 
};
static const unsigned char gb_chars_ban2218 [] = {
    0xcf, 0xe0, 0xed, 0xef, 
};
static const unsigned char gb_chars_ban2219 [] = {
    0xc5, 0xce, 
};
static const unsigned char gb_chars_ban2220 [] = {
    0xb6, 0xdc, 
};
static const unsigned char gb_chars_ban2221 [] = {
    0xca, 0xa1, 
};
static const unsigned char gb_chars_ban2222 [] = {
    0xed, 0xed, 
};
static const unsigned char gb_chars_ban2223 [] = {
    0xed, 0xf0, 0xed, 0xf1, 0xc3, 0xbc, 
};
static const unsigned char gb_chars_ban2224 [] = {
    0xbf, 0xb4, 
};
static const unsigned char gb_chars_ban2225 [] = {
    0xed, 0xee, 
};
static const unsigned char gb_chars_ban2226 [] = {
    0xed, 0xf4, 0xed, 0xf2, 
};
static const unsigned char gb_chars_ban2227 [] = {
    0xd5, 0xe6, 0xc3, 0xdf, 
};
static const unsigned char gb_chars_ban2228 [] = {
    0xed, 0xf3, 
};
static const unsigned char gb_chars_ban2229 [] = {
    0xed, 0xf6, 
};
static const unsigned char gb_chars_ban2230 [] = {
    0xd5, 0xa3, 0xd1, 0xa3, 
};
static const unsigned char gb_chars_ban2231 [] = {
    0xed, 0xf5, 
};
static const unsigned char gb_chars_ban2232 [] = {
    0xc3, 0xd0, 
};
static const unsigned char gb_chars_ban2233 [] = {
    0xed, 0xf7, 0xbf, 0xf4, 0xbe, 0xec, 0xed, 0xf8, 
};
static const unsigned char gb_chars_ban2234 [] = {
    0xcc, 0xf7, 
};
static const unsigned char gb_chars_ban2235 [] = {
    0xd1, 0xdb, 
};
static const unsigned char gb_chars_ban2236 [] = {
    0xd7, 0xc5, 0xd5, 0xf6, 
};
static const unsigned char gb_chars_ban2237 [] = {
    0xed, 0xfc, 
};
static const unsigned char gb_chars_ban2238 [] = {
    0xed, 0xfb, 
};
static const unsigned char gb_chars_ban2239 [] = {
    0xed, 0xf9, 0xed, 0xfa, 
};
static const unsigned char gb_chars_ban2240 [] = {
    0xed, 0xfd, 0xbe, 0xa6, 
};
static const unsigned char gb_chars_ban2241 [] = {
    0xcb, 0xaf, 0xee, 0xa1, 0xb6, 0xbd, 
};
static const unsigned char gb_chars_ban2242 [] = {
    0xee, 0xa2, 0xc4, 0xc0, 
};
static const unsigned char gb_chars_ban2243 [] = {
    0xed, 0xfe, 
};
static const unsigned char gb_chars_ban2244 [] = {
    0xbd, 0xde, 0xb2, 0xc7, 
};
static const unsigned char gb_chars_ban2245 [] = {
    0xb6, 0xc3, 
};
static const unsigned char gb_chars_ban2246 [] = {
    0xee, 0xa5, 0xd8, 0xba, 0xee, 0xa3, 0xee, 0xa6, 
};
static const unsigned char gb_chars_ban2247 [] = {
    0xc3, 0xe9, 0xb3, 0xf2, 
};
static const unsigned char gb_chars_ban2248 [] = {
    0xee, 0xa7, 0xee, 0xa4, 0xcf, 0xb9, 
};
static const unsigned char gb_chars_ban2249 [] = {
    0xee, 0xa8, 0xc2, 0xf7, 
};
static const unsigned char gb_chars_ban2250 [] = {
    0xee, 0xa9, 0xee, 0xaa, 
};
static const unsigned char gb_chars_ban2251 [] = {
    0xde, 0xab, 
};
static const unsigned char gb_chars_ban2252 [] = {
    0xc6, 0xb3, 
};
static const unsigned char gb_chars_ban2253 [] = {
    0xc7, 0xc6, 
};
static const unsigned char gb_chars_ban2254 [] = {
    0xd6, 0xf5, 0xb5, 0xc9, 
};
static const unsigned char gb_chars_ban2255 [] = {
    0xcb, 0xb2, 
};
static const unsigned char gb_chars_ban2256 [] = {
    0xee, 0xab, 
};
static const unsigned char gb_chars_ban2257 [] = {
    0xcd, 0xab, 
};
static const unsigned char gb_chars_ban2258 [] = {
    0xee, 0xac, 
};
static const unsigned char gb_chars_ban2259 [] = {
    0xd5, 0xb0, 
};
static const unsigned char gb_chars_ban2260 [] = {
    0xee, 0xad, 
};
static const unsigned char gb_chars_ban2261 [] = {
    0xf6, 0xc4, 
};
static const unsigned char gb_chars_ban2262 [] = {
    0xdb, 0xc7, 
};
static const unsigned char gb_chars_ban2263 [] = {
    0xb4, 0xa3, 
};
static const unsigned char gb_chars_ban2264 [] = {
    0xc3, 0xac, 0xf1, 0xe6, 
};
static const unsigned char gb_chars_ban2265 [] = {
    0xca, 0xb8, 0xd2, 0xd3, 
};
static const unsigned char gb_chars_ban2266 [] = {
    0xd6, 0xaa, 
};
static const unsigned char gb_chars_ban2267 [] = {
    0xef, 0xf2, 
};
static const unsigned char gb_chars_ban2268 [] = {
    0xbe, 0xd8, 
};
static const unsigned char gb_chars_ban2269 [] = {
    0xbd, 0xc3, 0xef, 0xf3, 0xb6, 0xcc, 0xb0, 0xab, 
};
static const unsigned char gb_chars_ban2270 [] = {
    0xca, 0xaf, 
};
static const unsigned char gb_chars_ban2271 [] = {
    0xed, 0xb6, 
};
static const unsigned char gb_chars_ban2272 [] = {
    0xed, 0xb7, 
};
static const unsigned char gb_chars_ban2273 [] = {
    0xce, 0xf9, 0xb7, 0xaf, 0xbf, 0xf3, 0xed, 0xb8, 0xc2, 0xeb, 
    0xc9, 0xb0, 
};
static const unsigned char gb_chars_ban2274 [] = {
    0xed, 0xb9, 
};
static const unsigned char gb_chars_ban2275 [] = {
    0xc6, 0xf6, 0xbf, 0xb3, 
};
static const unsigned char gb_chars_ban2276 [] = {
    0xed, 0xbc, 0xc5, 0xf8, 
};
static const unsigned char gb_chars_ban2277 [] = {
    0xd1, 0xd0, 
};
static const unsigned char gb_chars_ban2278 [] = {
    0xd7, 0xa9, 0xed, 0xba, 0xed, 0xbb, 
};
static const unsigned char gb_chars_ban2279 [] = {
    0xd1, 0xe2, 
};
static const unsigned char gb_chars_ban2280 [] = {
    0xed, 0xbf, 0xed, 0xc0, 
};
static const unsigned char gb_chars_ban2281 [] = {
    0xed, 0xc4, 
};
static const unsigned char gb_chars_ban2282 [] = {
    0xed, 0xc8, 
};
static const unsigned char gb_chars_ban2283 [] = {
    0xed, 0xc6, 0xed, 0xce, 0xd5, 0xe8, 
};
static const unsigned char gb_chars_ban2284 [] = {
    0xed, 0xc9, 
};
static const unsigned char gb_chars_ban2285 [] = {
    0xed, 0xc7, 0xed, 0xbe, 
};
static const unsigned char gb_chars_ban2286 [] = {
    0xc5, 0xe9, 
};
static const unsigned char gb_chars_ban2287 [] = {
    0xc6, 0xc6, 
};
static const unsigned char gb_chars_ban2288 [] = {
    0xc9, 0xe9, 0xd4, 0xd2, 0xed, 0xc1, 0xed, 0xc2, 0xed, 0xc3, 
    0xed, 0xc5, 
};
static const unsigned char gb_chars_ban2289 [] = {
    0xc0, 0xf9, 
};
static const unsigned char gb_chars_ban2290 [] = {
    0xb4, 0xa1, 
};
static const unsigned char gb_chars_ban2291 [] = {
    0xb9, 0xe8, 
};
static const unsigned char gb_chars_ban2292 [] = {
    0xed, 0xd0, 
};
static const unsigned char gb_chars_ban2293 [] = {
    0xed, 0xd1, 
};
static const unsigned char gb_chars_ban2294 [] = {
    0xed, 0xca, 
};
static const unsigned char gb_chars_ban2295 [] = {
    0xed, 0xcf, 
};
static const unsigned char gb_chars_ban2296 [] = {
    0xce, 0xf8, 
};
static const unsigned char gb_chars_ban2297 [] = {
    0xcb, 0xb6, 0xed, 0xcc, 0xed, 0xcd, 
};
static const unsigned char gb_chars_ban2298 [] = {
    0xcf, 0xf5, 
};
static const unsigned char gb_chars_ban2299 [] = {
    0xed, 0xd2, 0xc1, 0xf2, 0xd3, 0xb2, 0xed, 0xcb, 0xc8, 0xb7, 
};
static const unsigned char gb_chars_ban2300 [] = {
    0xbc, 0xef, 
};
static const unsigned char gb_chars_ban2301 [] = {
    0xc5, 0xf0, 
};
static const unsigned char gb_chars_ban2302 [] = {
    0xed, 0xd6, 
};
static const unsigned char gb_chars_ban2303 [] = {
    0xb5, 0xef, 
};
static const unsigned char gb_chars_ban2304 [] = {
    0xc2, 0xb5, 0xb0, 0xad, 0xcb, 0xe9, 
};
static const unsigned char gb_chars_ban2305 [] = {
    0xb1, 0xae, 
};
static const unsigned char gb_chars_ban2306 [] = {
    0xed, 0xd4, 
};
static const unsigned char gb_chars_ban2307 [] = {
    0xcd, 0xeb, 0xb5, 0xe2, 
};
static const unsigned char gb_chars_ban2308 [] = {
    0xed, 0xd5, 0xed, 0xd3, 0xed, 0xd7, 
};
static const unsigned char gb_chars_ban2309 [] = {
    0xb5, 0xfa, 
};
static const unsigned char gb_chars_ban2310 [] = {
    0xed, 0xd8, 
};
static const unsigned char gb_chars_ban2311 [] = {
    0xed, 0xd9, 
};
static const unsigned char gb_chars_ban2312 [] = {
    0xed, 0xdc, 
};
static const unsigned char gb_chars_ban2313 [] = {
    0xb1, 0xcc, 
};
static const unsigned char gb_chars_ban2314 [] = {
    0xc5, 0xf6, 0xbc, 0xee, 0xed, 0xda, 0xcc, 0xbc, 0xb2, 0xea, 
};
static const unsigned char gb_chars_ban2315 [] = {
    0xed, 0xdb, 
};
static const unsigned char gb_chars_ban2316 [] = {
    0xc4, 0xeb, 
};
static const unsigned char gb_chars_ban2317 [] = {
    0xb4, 0xc5, 
};
static const unsigned char gb_chars_ban2318 [] = {
    0xb0, 0xf5, 
};
static const unsigned char gb_chars_ban2319 [] = {
    0xed, 0xdf, 0xc0, 0xda, 0xb4, 0xe8, 
};
static const unsigned char gb_chars_ban2320 [] = {
    0xc5, 0xcd, 
};
static const unsigned char gb_chars_ban2321 [] = {
    0xed, 0xdd, 0xbf, 0xc4, 
};
static const unsigned char gb_chars_ban2322 [] = {
    0xed, 0xde, 
};
static const unsigned char gb_chars_ban2323 [] = {
    0xc4, 0xa5, 
};
static const unsigned char gb_chars_ban2324 [] = {
    0xed, 0xe0, 
};
static const unsigned char gb_chars_ban2325 [] = {
    0xed, 0xe1, 
};
static const unsigned char gb_chars_ban2326 [] = {
    0xed, 0xe3, 
};
static const unsigned char gb_chars_ban2327 [] = {
    0xc1, 0xd7, 
};
static const unsigned char gb_chars_ban2328 [] = {
    0xbb, 0xc7, 
};
static const unsigned char gb_chars_ban2329 [] = {
    0xbd, 0xb8, 
};
static const unsigned char gb_chars_ban2330 [] = {
    0xed, 0xe2, 
};
static const unsigned char gb_chars_ban2331 [] = {
    0xed, 0xe4, 
};
static const unsigned char gb_chars_ban2332 [] = {
    0xed, 0xe6, 
};
static const unsigned char gb_chars_ban2333 [] = {
    0xed, 0xe5, 
};
static const unsigned char gb_chars_ban2334 [] = {
    0xed, 0xe7, 
};
static const unsigned char gb_chars_ban2335 [] = {
    0xca, 0xbe, 0xec, 0xea, 0xc0, 0xf1, 
};
static const unsigned char gb_chars_ban2336 [] = {
    0xc9, 0xe7, 
};
static const unsigned char gb_chars_ban2337 [] = {
    0xec, 0xeb, 0xc6, 0xee, 
};
static const unsigned char gb_chars_ban2338 [] = {
    0xec, 0xec, 
};
static const unsigned char gb_chars_ban2339 [] = {
    0xc6, 0xed, 0xec, 0xed, 
};
static const unsigned char gb_chars_ban2340 [] = {
    0xec, 0xf0, 
};
static const unsigned char gb_chars_ban2341 [] = {
    0xd7, 0xe6, 0xec, 0xf3, 
};
static const unsigned char gb_chars_ban2342 [] = {
    0xec, 0xf1, 0xec, 0xee, 0xec, 0xef, 0xd7, 0xa3, 0xc9, 0xf1, 
    0xcb, 0xee, 0xec, 0xf4, 
};
static const unsigned char gb_chars_ban2343 [] = {
    0xec, 0xf2, 
};
static const unsigned char gb_chars_ban2344 [] = {
    0xcf, 0xe9, 
};
static const unsigned char gb_chars_ban2345 [] = {
    0xec, 0xf6, 0xc6, 0xb1, 
};
static const unsigned char gb_chars_ban2346 [] = {
    0xbc, 0xc0, 
};
static const unsigned char gb_chars_ban2347 [] = {
    0xec, 0xf5, 
};
static const unsigned char gb_chars_ban2348 [] = {
    0xb5, 0xbb, 0xbb, 0xf6, 
};
static const unsigned char gb_chars_ban2349 [] = {
    0xec, 0xf7, 
};
static const unsigned char gb_chars_ban2350 [] = {
    0xd9, 0xf7, 0xbd, 0xfb, 
};
static const unsigned char gb_chars_ban2351 [] = {
    0xc2, 0xbb, 0xec, 0xf8, 
};
static const unsigned char gb_chars_ban2352 [] = {
    0xec, 0xf9, 
};
static const unsigned char gb_chars_ban2353 [] = {
    0xb8, 0xa3, 
};
static const unsigned char gb_chars_ban2354 [] = {
    0xec, 0xfa, 
};
static const unsigned char gb_chars_ban2355 [] = {
    0xec, 0xfb, 
};
static const unsigned char gb_chars_ban2356 [] = {
    0xec, 0xfc, 
};
static const unsigned char gb_chars_ban2357 [] = {
    0xd3, 0xed, 0xd8, 0xae, 0xc0, 0xeb, 
};
static const unsigned char gb_chars_ban2358 [] = {
    0xc7, 0xdd, 0xba, 0xcc, 
};
static const unsigned char gb_chars_ban2359 [] = {
    0xd0, 0xe3, 0xcb, 0xbd, 
};
static const unsigned char gb_chars_ban2360 [] = {
    0xcd, 0xba, 
};
static const unsigned char gb_chars_ban2361 [] = {
    0xb8, 0xd1, 
};
static const unsigned char gb_chars_ban2362 [] = {
    0xb1, 0xfc, 
};
static const unsigned char gb_chars_ban2363 [] = {
    0xc7, 0xef, 
};
static const unsigned char gb_chars_ban2364 [] = {
    0xd6, 0xd6, 
};
static const unsigned char gb_chars_ban2365 [] = {
    0xbf, 0xc6, 0xc3, 0xeb, 
};
static const unsigned char gb_chars_ban2366 [] = {
    0xef, 0xf5, 
};
static const unsigned char gb_chars_ban2367 [] = {
    0xc3, 0xd8, 
};
static const unsigned char gb_chars_ban2368 [] = {
    0xd7, 0xe2, 
};
static const unsigned char gb_chars_ban2369 [] = {
    0xef, 0xf7, 0xb3, 0xd3, 
};
static const unsigned char gb_chars_ban2370 [] = {
    0xc7, 0xd8, 0xd1, 0xed, 
};
static const unsigned char gb_chars_ban2371 [] = {
    0xd6, 0xc8, 
};
static const unsigned char gb_chars_ban2372 [] = {
    0xef, 0xf8, 
};
static const unsigned char gb_chars_ban2373 [] = {
    0xef, 0xf6, 
};
static const unsigned char gb_chars_ban2374 [] = {
    0xbb, 0xfd, 0xb3, 0xc6, 
};
static const unsigned char gb_chars_ban2375 [] = {
    0xbd, 0xd5, 
};
static const unsigned char gb_chars_ban2376 [] = {
    0xd2, 0xc6, 
};
static const unsigned char gb_chars_ban2377 [] = {
    0xbb, 0xe0, 
};
static const unsigned char gb_chars_ban2378 [] = {
    0xcf, 0xa1, 
};
static const unsigned char gb_chars_ban2379 [] = {
    0xef, 0xfc, 0xef, 0xfb, 
};
static const unsigned char gb_chars_ban2380 [] = {
    0xef, 0xf9, 
};
static const unsigned char gb_chars_ban2381 [] = {
    0xb3, 0xcc, 
};
static const unsigned char gb_chars_ban2382 [] = {
    0xc9, 0xd4, 0xcb, 0xb0, 
};
static const unsigned char gb_chars_ban2383 [] = {
    0xef, 0xfe, 
};
static const unsigned char gb_chars_ban2384 [] = {
    0xb0, 0xde, 
};
static const unsigned char gb_chars_ban2385 [] = {
    0xd6, 0xc9, 
};
static const unsigned char gb_chars_ban2386 [] = {
    0xef, 0xfd, 
};
static const unsigned char gb_chars_ban2387 [] = {
    0xb3, 0xed, 
};
static const unsigned char gb_chars_ban2388 [] = {
    0xf6, 0xd5, 
};
static const unsigned char gb_chars_ban2389 [] = {
    0xce, 0xc8, 
};
static const unsigned char gb_chars_ban2390 [] = {
    0xf0, 0xa2, 
};
static const unsigned char gb_chars_ban2391 [] = {
    0xf0, 0xa1, 
};
static const unsigned char gb_chars_ban2392 [] = {
    0xb5, 0xbe, 0xbc, 0xda, 0xbb, 0xfc, 
};
static const unsigned char gb_chars_ban2393 [] = {
    0xb8, 0xe5, 
};
static const unsigned char gb_chars_ban2394 [] = {
    0xc4, 0xc2, 
};
static const unsigned char gb_chars_ban2395 [] = {
    0xf0, 0xa3, 
};
static const unsigned char gb_chars_ban2396 [] = {
    0xcb, 0xeb, 
};
static const unsigned char gb_chars_ban2397 [] = {
    0xf0, 0xa6, 
};
static const unsigned char gb_chars_ban2398 [] = {
    0xd1, 0xa8, 
};
static const unsigned char gb_chars_ban2399 [] = {
    0xbe, 0xbf, 0xc7, 0xee, 0xf1, 0xb6, 0xf1, 0xb7, 0xbf, 0xd5, 
};
static const unsigned char gb_chars_ban2400 [] = {
    0xb4, 0xa9, 0xf1, 0xb8, 0xcd, 0xbb, 
};
static const unsigned char gb_chars_ban2401 [] = {
    0xc7, 0xd4, 0xd5, 0xad, 
};
static const unsigned char gb_chars_ban2402 [] = {
    0xf1, 0xb9, 
};
static const unsigned char gb_chars_ban2403 [] = {
    0xf1, 0xba, 
};
static const unsigned char gb_chars_ban2404 [] = {
    0xc7, 0xcf, 
};
static const unsigned char gb_chars_ban2405 [] = {
    0xd2, 0xa4, 0xd6, 0xcf, 
};
static const unsigned char gb_chars_ban2406 [] = {
    0xf1, 0xbb, 0xbd, 0xd1, 0xb4, 0xb0, 0xbe, 0xbd, 
};
static const unsigned char gb_chars_ban2407 [] = {
    0xb4, 0xdc, 0xce, 0xd1, 
};
static const unsigned char gb_chars_ban2408 [] = {
    0xbf, 0xdf, 0xf1, 0xbd, 
};
static const unsigned char gb_chars_ban2409 [] = {
    0xbf, 0xfa, 0xf1, 0xbc, 
};
static const unsigned char gb_chars_ban2410 [] = {
    0xf1, 0xbf, 
};
static const unsigned char gb_chars_ban2411 [] = {
    0xf1, 0xbe, 0xf1, 0xc0, 
};
static const unsigned char gb_chars_ban2412 [] = {
    0xf1, 0xc1, 
};
static const unsigned char gb_chars_ban2413 [] = {
    0xc1, 0xfe, 
};
static const unsigned char gb_chars_ban2414 [] = {
    0xc1, 0xa2, 
};
static const unsigned char gb_chars_ban2415 [] = {
    0xca, 0xfa, 
};
static const unsigned char gb_chars_ban2416 [] = {
    0xd5, 0xbe, 
};
static const unsigned char gb_chars_ban2417 [] = {
    0xbe, 0xba, 0xbe, 0xb9, 0xd5, 0xc2, 
};
static const unsigned char gb_chars_ban2418 [] = {
    0xbf, 0xa2, 
};
static const unsigned char gb_chars_ban2419 [] = {
    0xcd, 0xaf, 0xf1, 0xb5, 
};
static const unsigned char gb_chars_ban2420 [] = {
    0xbd, 0xdf, 
};
static const unsigned char gb_chars_ban2421 [] = {
    0xb6, 0xcb, 
};
static const unsigned char gb_chars_ban2422 [] = {
    0xd6, 0xf1, 0xf3, 0xc3, 
};
static const unsigned char gb_chars_ban2423 [] = {
    0xf3, 0xc4, 
};
static const unsigned char gb_chars_ban2424 [] = {
    0xb8, 0xcd, 
};
static const unsigned char gb_chars_ban2425 [] = {
    0xf3, 0xc6, 0xf3, 0xc7, 
};
static const unsigned char gb_chars_ban2426 [] = {
    0xb0, 0xca, 
};
static const unsigned char gb_chars_ban2427 [] = {
    0xf3, 0xc5, 
};
static const unsigned char gb_chars_ban2428 [] = {
    0xf3, 0xc9, 0xcb, 0xf1, 
};
static const unsigned char gb_chars_ban2429 [] = {
    0xf3, 0xcb, 
};
static const unsigned char gb_chars_ban2430 [] = {
    0xd0, 0xa6, 
};
static const unsigned char gb_chars_ban2431 [] = {
    0xb1, 0xca, 0xf3, 0xc8, 
};
static const unsigned char gb_chars_ban2432 [] = {
    0xf3, 0xcf, 
};
static const unsigned char gb_chars_ban2433 [] = {
    0xb5, 0xd1, 
};
static const unsigned char gb_chars_ban2434 [] = {
    0xf3, 0xd7, 
};
static const unsigned char gb_chars_ban2435 [] = {
    0xf3, 0xd2, 
};
static const unsigned char gb_chars_ban2436 [] = {
    0xf3, 0xd4, 0xf3, 0xd3, 0xb7, 0xfb, 
};
static const unsigned char gb_chars_ban2437 [] = {
    0xb1, 0xbf, 
};
static const unsigned char gb_chars_ban2438 [] = {
    0xf3, 0xce, 0xf3, 0xca, 0xb5, 0xda, 
};
static const unsigned char gb_chars_ban2439 [] = {
    0xf3, 0xd0, 
};
static const unsigned char gb_chars_ban2440 [] = {
    0xf3, 0xd1, 
};
static const unsigned char gb_chars_ban2441 [] = {
    0xf3, 0xd5, 
};
static const unsigned char gb_chars_ban2442 [] = {
    0xf3, 0xcd, 
};
static const unsigned char gb_chars_ban2443 [] = {
    0xbc, 0xe3, 
};
static const unsigned char gb_chars_ban2444 [] = {
    0xc1, 0xfd, 
};
static const unsigned char gb_chars_ban2445 [] = {
    0xf3, 0xd6, 
};
static const unsigned char gb_chars_ban2446 [] = {
    0xf3, 0xda, 
};
static const unsigned char gb_chars_ban2447 [] = {
    0xf3, 0xcc, 
};
static const unsigned char gb_chars_ban2448 [] = {
    0xb5, 0xc8, 
};
static const unsigned char gb_chars_ban2449 [] = {
    0xbd, 0xee, 0xf3, 0xdc, 
};
static const unsigned char gb_chars_ban2450 [] = {
    0xb7, 0xa4, 0xbf, 0xf0, 0xd6, 0xfe, 0xcd, 0xb2, 
};
static const unsigned char gb_chars_ban2451 [] = {
    0xb4, 0xf0, 
};
static const unsigned char gb_chars_ban2452 [] = {
    0xb2, 0xdf, 
};
static const unsigned char gb_chars_ban2453 [] = {
    0xf3, 0xd8, 
};
static const unsigned char gb_chars_ban2454 [] = {
    0xf3, 0xd9, 0xc9, 0xb8, 
};
static const unsigned char gb_chars_ban2455 [] = {
    0xf3, 0xdd, 
};
static const unsigned char gb_chars_ban2456 [] = {
    0xf3, 0xde, 
};
static const unsigned char gb_chars_ban2457 [] = {
    0xf3, 0xe1, 
};
static const unsigned char gb_chars_ban2458 [] = {
    0xf3, 0xdf, 
};
static const unsigned char gb_chars_ban2459 [] = {
    0xf3, 0xe3, 0xf3, 0xe2, 
};
static const unsigned char gb_chars_ban2460 [] = {
    0xf3, 0xdb, 
};
static const unsigned char gb_chars_ban2461 [] = {
    0xbf, 0xea, 
};
static const unsigned char gb_chars_ban2462 [] = {
    0xb3, 0xef, 
};
static const unsigned char gb_chars_ban2463 [] = {
    0xf3, 0xe0, 
};
static const unsigned char gb_chars_ban2464 [] = {
    0xc7, 0xa9, 
};
static const unsigned char gb_chars_ban2465 [] = {
    0xbc, 0xf2, 
};
static const unsigned char gb_chars_ban2466 [] = {
    0xf3, 0xeb, 
};
static const unsigned char gb_chars_ban2467 [] = {
    0xb9, 0xbf, 
};
static const unsigned char gb_chars_ban2468 [] = {
    0xf3, 0xe4, 
};
static const unsigned char gb_chars_ban2469 [] = {
    0xb2, 0xad, 0xbb, 0xfe, 
};
static const unsigned char gb_chars_ban2470 [] = {
    0xcb, 0xe3, 
};
static const unsigned char gb_chars_ban2471 [] = {
    0xf3, 0xed, 0xf3, 0xe9, 
};
static const unsigned char gb_chars_ban2472 [] = {
    0xb9, 0xdc, 0xf3, 0xee, 
};
static const unsigned char gb_chars_ban2473 [] = {
    0xf3, 0xe5, 0xf3, 0xe6, 0xf3, 0xea, 0xc2, 0xe1, 0xf3, 0xec, 
    0xf3, 0xef, 0xf3, 0xe8, 0xbc, 0xfd, 
};
static const unsigned char gb_chars_ban2474 [] = {
    0xcf, 0xe4, 
};
static const unsigned char gb_chars_ban2475 [] = {
    0xf3, 0xf0, 
};
static const unsigned char gb_chars_ban2476 [] = {
    0xf3, 0xe7, 
};
static const unsigned char gb_chars_ban2477 [] = {
    0xf3, 0xf2, 
};
static const unsigned char gb_chars_ban2478 [] = {
    0xd7, 0xad, 0xc6, 0xaa, 
};
static const unsigned char gb_chars_ban2479 [] = {
    0xf3, 0xf3, 
};
static const unsigned char gb_chars_ban2480 [] = {
    0xf3, 0xf1, 
};
static const unsigned char gb_chars_ban2481 [] = {
    0xc2, 0xa8, 
};
static const unsigned char gb_chars_ban2482 [] = {
    0xb8, 0xdd, 0xf3, 0xf5, 
};
static const unsigned char gb_chars_ban2483 [] = {
    0xf3, 0xf4, 
};
static const unsigned char gb_chars_ban2484 [] = {
    0xb4, 0xdb, 
};
static const unsigned char gb_chars_ban2485 [] = {
    0xf3, 0xf6, 0xf3, 0xf7, 
};
static const unsigned char gb_chars_ban2486 [] = {
    0xf3, 0xf8, 
};
static const unsigned char gb_chars_ban2487 [] = {
    0xc0, 0xba, 
};
static const unsigned char gb_chars_ban2488 [] = {
    0xc0, 0xe9, 
};
static const unsigned char gb_chars_ban2489 [] = {
    0xc5, 0xf1, 
};
static const unsigned char gb_chars_ban2490 [] = {
    0xf3, 0xfb, 
};
static const unsigned char gb_chars_ban2491 [] = {
    0xf3, 0xfa, 
};
static const unsigned char gb_chars_ban2492 [] = {
    0xb4, 0xd8, 
};
static const unsigned char gb_chars_ban2493 [] = {
    0xf3, 0xfe, 0xf3, 0xf9, 
};
static const unsigned char gb_chars_ban2494 [] = {
    0xf3, 0xfc, 
};
static const unsigned char gb_chars_ban2495 [] = {
    0xf3, 0xfd, 
};
static const unsigned char gb_chars_ban2496 [] = {
    0xf4, 0xa1, 
};
static const unsigned char gb_chars_ban2497 [] = {
    0xf4, 0xa3, 0xbb, 0xc9, 
};
static const unsigned char gb_chars_ban2498 [] = {
    0xf4, 0xa2, 
};
static const unsigned char gb_chars_ban2499 [] = {
    0xf4, 0xa4, 
};
static const unsigned char gb_chars_ban2500 [] = {
    0xb2, 0xbe, 0xf4, 0xa6, 0xf4, 0xa5, 
};
static const unsigned char gb_chars_ban2501 [] = {
    0xbc, 0xae, 
};
static const unsigned char gb_chars_ban2502 [] = {
    0xc3, 0xd7, 0xd9, 0xe1, 
};
static const unsigned char gb_chars_ban2503 [] = {
    0xc0, 0xe0, 0xf4, 0xcc, 0xd7, 0xd1, 
};
static const unsigned char gb_chars_ban2504 [] = {
    0xb7, 0xdb, 
};
static const unsigned char gb_chars_ban2505 [] = {
    0xf4, 0xce, 0xc1, 0xa3, 
};
static const unsigned char gb_chars_ban2506 [] = {
    0xc6, 0xc9, 
};
static const unsigned char gb_chars_ban2507 [] = {
    0xb4, 0xd6, 0xd5, 0xb3, 
};
static const unsigned char gb_chars_ban2508 [] = {
    0xf4, 0xd0, 0xf4, 0xcf, 0xf4, 0xd1, 0xcb, 0xda, 
};
static const unsigned char gb_chars_ban2509 [] = {
    0xf4, 0xd2, 
};
static const unsigned char gb_chars_ban2510 [] = {
    0xd4, 0xc1, 0xd6, 0xe0, 
};
static const unsigned char gb_chars_ban2511 [] = {
    0xb7, 0xe0, 
};
static const unsigned char gb_chars_ban2512 [] = {
    0xc1, 0xb8, 
};
static const unsigned char gb_chars_ban2513 [] = {
    0xc1, 0xbb, 0xf4, 0xd3, 0xbe, 0xac, 
};
static const unsigned char gb_chars_ban2514 [] = {
    0xb4, 0xe2, 
};
static const unsigned char gb_chars_ban2515 [] = {
    0xf4, 0xd4, 0xf4, 0xd5, 0xbe, 0xab, 
};
static const unsigned char gb_chars_ban2516 [] = {
    0xf4, 0xd6, 
};
static const unsigned char gb_chars_ban2517 [] = {
    0xf4, 0xdb, 
};
static const unsigned char gb_chars_ban2518 [] = {
    0xf4, 0xd7, 0xf4, 0xda, 
};
static const unsigned char gb_chars_ban2519 [] = {
    0xba, 0xfd, 
};
static const unsigned char gb_chars_ban2520 [] = {
    0xf4, 0xd8, 0xf4, 0xd9, 
};
static const unsigned char gb_chars_ban2521 [] = {
    0xb8, 0xe2, 0xcc, 0xc7, 0xf4, 0xdc, 
};
static const unsigned char gb_chars_ban2522 [] = {
    0xb2, 0xda, 
};
static const unsigned char gb_chars_ban2523 [] = {
    0xc3, 0xd3, 
};
static const unsigned char gb_chars_ban2524 [] = {
    0xd4, 0xe3, 0xbf, 0xb7, 
};
static const unsigned char gb_chars_ban2525 [] = {
    0xf4, 0xdd, 
};
static const unsigned char gb_chars_ban2526 [] = {
    0xc5, 0xb4, 
};
static const unsigned char gb_chars_ban2527 [] = {
    0xf4, 0xe9, 
};
static const unsigned char gb_chars_ban2528 [] = {
    0xcf, 0xb5, 
};
static const unsigned char gb_chars_ban2529 [] = {
    0xce, 0xc9, 
};
static const unsigned char gb_chars_ban2530 [] = {
    0xcb, 0xd8, 
};
static const unsigned char gb_chars_ban2531 [] = {
    0xcb, 0xf7, 
};
static const unsigned char gb_chars_ban2532 [] = {
    0xbd, 0xf4, 
};
static const unsigned char gb_chars_ban2533 [] = {
    0xd7, 0xcf, 
};
static const unsigned char gb_chars_ban2534 [] = {
    0xc0, 0xdb, 
};
static const unsigned char gb_chars_ban2535 [] = {
    0xd0, 0xf5, 
};
static const unsigned char gb_chars_ban2536 [] = {
    0xf4, 0xea, 
};
static const unsigned char gb_chars_ban2537 [] = {
    0xf4, 0xeb, 
};
static const unsigned char gb_chars_ban2538 [] = {
    0xf4, 0xec, 
};
static const unsigned char gb_chars_ban2539 [] = {
    0xf7, 0xe3, 
};
static const unsigned char gb_chars_ban2540 [] = {
    0xb7, 0xb1, 
};
static const unsigned char gb_chars_ban2541 [] = {
    0xf4, 0xed, 
};
static const unsigned char gb_chars_ban2542 [] = {
    0xd7, 0xeb, 
};
static const unsigned char gb_chars_ban2543 [] = {
    0xf4, 0xee, 
};
static const unsigned char gb_chars_ban2544 [] = {
    0xe6, 0xf9, 0xbe, 0xc0, 0xe6, 0xfa, 0xba, 0xec, 0xe6, 0xfb, 
    0xcf, 0xcb, 0xe6, 0xfc, 0xd4, 0xbc, 0xbc, 0xb6, 0xe6, 0xfd, 
    0xe6, 0xfe, 0xbc, 0xcd, 0xc8, 0xd2, 0xce, 0xb3, 0xe7, 0xa1, 
};
static const unsigned char gb_chars_ban2545 [] = {
    0xb4, 0xbf, 0xe7, 0xa2, 0xc9, 0xb4, 0xb8, 0xd9, 0xc4, 0xc9, 
};
static const unsigned char gb_chars_ban2546 [] = {
    0xd7, 0xdd, 0xc2, 0xda, 0xb7, 0xd7, 0xd6, 0xbd, 0xce, 0xc6, 
    0xb7, 0xc4, 
};
static const unsigned char gb_chars_ban2547 [] = {
    0xc5, 0xa6, 0xe7, 0xa3, 0xcf, 0xdf, 0xe7, 0xa4, 0xe7, 0xa5, 
    0xe7, 0xa6, 0xc1, 0xb7, 0xd7, 0xe9, 0xc9, 0xf0, 0xcf, 0xb8, 
    0xd6, 0xaf, 0xd6, 0xd5, 0xe7, 0xa7, 0xb0, 0xed, 0xe7, 0xa8, 
    0xe7, 0xa9, 0xc9, 0xdc, 0xd2, 0xef, 0xbe, 0xad, 0xe7, 0xaa, 
    0xb0, 0xf3, 0xc8, 0xde, 0xbd, 0xe1, 0xe7, 0xab, 0xc8, 0xc6, 
};
static const unsigned char gb_chars_ban2548 [] = {
    0xe7, 0xac, 0xbb, 0xe6, 0xb8, 0xf8, 0xd1, 0xa4, 0xe7, 0xad, 
    0xc2, 0xe7, 0xbe, 0xf8, 0xbd, 0xca, 0xcd, 0xb3, 0xe7, 0xae, 
    0xe7, 0xaf, 0xbe, 0xee, 0xd0, 0xe5, 
};
static const unsigned char gb_chars_ban2549 [] = {
    0xcb, 0xe7, 0xcc, 0xd0, 0xbc, 0xcc, 0xe7, 0xb0, 0xbc, 0xa8, 
    0xd0, 0xf7, 0xe7, 0xb1, 
};
static const unsigned char gb_chars_ban2550 [] = {
    0xd0, 0xf8, 0xe7, 0xb2, 0xe7, 0xb3, 0xb4, 0xc2, 0xe7, 0xb4, 
    0xe7, 0xb5, 0xc9, 0xfe, 0xce, 0xac, 0xc3, 0xe0, 0xe7, 0xb7, 
    0xb1, 0xc1, 0xb3, 0xf1, 
};
static const unsigned char gb_chars_ban2551 [] = {
    0xe7, 0xb8, 0xe7, 0xb9, 0xd7, 0xdb, 0xd5, 0xc0, 0xe7, 0xba, 
    0xc2, 0xcc, 0xd7, 0xba, 0xe7, 0xbb, 0xe7, 0xbc, 0xe7, 0xbd, 
    0xbc, 0xea, 0xc3, 0xe5, 0xc0, 0xc2, 0xe7, 0xbe, 0xe7, 0xbf, 
    0xbc, 0xa9, 
};
static const unsigned char gb_chars_ban2552 [] = {
    0xe7, 0xc0, 0xe7, 0xc1, 0xe7, 0xb6, 0xb6, 0xd0, 0xe7, 0xc2, 
};
static const unsigned char gb_chars_ban2553 [] = {
    0xe7, 0xc3, 0xe7, 0xc4, 0xbb, 0xba, 0xb5, 0xde, 0xc2, 0xc6, 
    0xb1, 0xe0, 0xe7, 0xc5, 0xd4, 0xb5, 0xe7, 0xc6, 0xb8, 0xbf, 
    0xe7, 0xc8, 0xe7, 0xc7, 0xb7, 0xec, 
};
static const unsigned char gb_chars_ban2554 [] = {
    0xe7, 0xc9, 0xb2, 0xf8, 0xe7, 0xca, 0xe7, 0xcb, 0xe7, 0xcc, 
    0xe7, 0xcd, 0xe7, 0xce, 0xe7, 0xcf, 0xe7, 0xd0, 0xd3, 0xa7, 
    0xcb, 0xf5, 0xe7, 0xd1, 0xe7, 0xd2, 0xe7, 0xd3, 0xe7, 0xd4, 
    0xc9, 0xc9, 0xe7, 0xd5, 0xe7, 0xd6, 0xe7, 0xd7, 0xe7, 0xd8, 
    0xe7, 0xd9, 0xbd, 0xc9, 0xe7, 0xda, 0xf3, 0xbe, 
};
static const unsigned char gb_chars_ban2555 [] = {
    0xb8, 0xd7, 
};
static const unsigned char gb_chars_ban2556 [] = {
    0xc8, 0xb1, 
};
static const unsigned char gb_chars_ban2557 [] = {
    0xf3, 0xbf, 
};
static const unsigned char gb_chars_ban2558 [] = {
    0xf3, 0xc0, 0xf3, 0xc1, 
};
static const unsigned char gb_chars_ban2559 [] = {
    0xb9, 0xde, 0xcd, 0xf8, 
};
static const unsigned char gb_chars_ban2560 [] = {
    0xd8, 0xe8, 0xba, 0xb1, 
};
static const unsigned char gb_chars_ban2561 [] = {
    0xc2, 0xde, 0xee, 0xb7, 
};
static const unsigned char gb_chars_ban2562 [] = {
    0xb7, 0xa3, 
};
static const unsigned char gb_chars_ban2563 [] = {
    0xee, 0xb9, 
};
static const unsigned char gb_chars_ban2564 [] = {
    0xee, 0xb8, 0xb0, 0xd5, 
};
static const unsigned char gb_chars_ban2565 [] = {
    0xee, 0xbb, 0xd5, 0xd6, 0xd7, 0xef, 
};
static const unsigned char gb_chars_ban2566 [] = {
    0xd6, 0xc3, 
};
static const unsigned char gb_chars_ban2567 [] = {
    0xee, 0xbd, 0xca, 0xf0, 
};
static const unsigned char gb_chars_ban2568 [] = {
    0xee, 0xbc, 
};
static const unsigned char gb_chars_ban2569 [] = {
    0xee, 0xbe, 
};
static const unsigned char gb_chars_ban2570 [] = {
    0xee, 0xc0, 
};
static const unsigned char gb_chars_ban2571 [] = {
    0xee, 0xbf, 
};
static const unsigned char gb_chars_ban2572 [] = {
    0xd1, 0xf2, 
};
static const unsigned char gb_chars_ban2573 [] = {
    0xc7, 0xbc, 
};
static const unsigned char gb_chars_ban2574 [] = {
    0xc3, 0xc0, 
};
static const unsigned char gb_chars_ban2575 [] = {
    0xb8, 0xe1, 
};
static const unsigned char gb_chars_ban2576 [] = {
    0xc1, 0xe7, 
};
static const unsigned char gb_chars_ban2577 [] = {
    0xf4, 0xc6, 0xd0, 0xdf, 0xf4, 0xc7, 
};
static const unsigned char gb_chars_ban2578 [] = {
    0xcf, 0xdb, 
};
static const unsigned char gb_chars_ban2579 [] = {
    0xc8, 0xba, 
};
static const unsigned char gb_chars_ban2580 [] = {
    0xf4, 0xc8, 
};
static const unsigned char gb_chars_ban2581 [] = {
    0xf4, 0xc9, 0xf4, 0xca, 
};
static const unsigned char gb_chars_ban2582 [] = {
    0xf4, 0xcb, 
};
static const unsigned char gb_chars_ban2583 [] = {
    0xd9, 0xfa, 0xb8, 0xfe, 
};
static const unsigned char gb_chars_ban2584 [] = {
    0xe5, 0xf1, 0xd3, 0xf0, 
};
static const unsigned char gb_chars_ban2585 [] = {
    0xf4, 0xe0, 
};
static const unsigned char gb_chars_ban2586 [] = {
    0xce, 0xcc, 
};
static const unsigned char gb_chars_ban2587 [] = {
    0xb3, 0xe1, 
};
static const unsigned char gb_chars_ban2588 [] = {
    0xf1, 0xb4, 
};
static const unsigned char gb_chars_ban2589 [] = {
    0xd2, 0xee, 
};
static const unsigned char gb_chars_ban2590 [] = {
    0xf4, 0xe1, 
};
static const unsigned char gb_chars_ban2591 [] = {
    0xcf, 0xe8, 0xf4, 0xe2, 
};
static const unsigned char gb_chars_ban2592 [] = {
    0xc7, 0xcc, 
};
static const unsigned char gb_chars_ban2593 [] = {
    0xb5, 0xd4, 0xb4, 0xe4, 0xf4, 0xe4, 
};
static const unsigned char gb_chars_ban2594 [] = {
    0xf4, 0xe3, 0xf4, 0xe5, 
};
static const unsigned char gb_chars_ban2595 [] = {
    0xf4, 0xe6, 
};
static const unsigned char gb_chars_ban2596 [] = {
    0xf4, 0xe7, 
};
static const unsigned char gb_chars_ban2597 [] = {
    0xba, 0xb2, 0xb0, 0xbf, 
};
static const unsigned char gb_chars_ban2598 [] = {
    0xf4, 0xe8, 
};
static const unsigned char gb_chars_ban2599 [] = {
    0xb7, 0xad, 0xd2, 0xed, 
};
static const unsigned char gb_chars_ban2600 [] = {
    0xd2, 0xab, 0xc0, 0xcf, 
};
static const unsigned char gb_chars_ban2601 [] = {
    0xbf, 0xbc, 0xeb, 0xa3, 0xd5, 0xdf, 0xea, 0xc8, 
};
static const unsigned char gb_chars_ban2602 [] = {
    0xf1, 0xf3, 0xb6, 0xf8, 0xcb, 0xa3, 
};
static const unsigned char gb_chars_ban2603 [] = {
    0xc4, 0xcd, 
};
static const unsigned char gb_chars_ban2604 [] = {
    0xf1, 0xe7, 
};
static const unsigned char gb_chars_ban2605 [] = {
    0xf1, 0xe8, 0xb8, 0xfb, 0xf1, 0xe9, 0xba, 0xc4, 0xd4, 0xc5, 
    0xb0, 0xd2, 
};
static const unsigned char gb_chars_ban2606 [] = {
    0xf1, 0xea, 
};
static const unsigned char gb_chars_ban2607 [] = {
    0xf1, 0xeb, 
};
static const unsigned char gb_chars_ban2608 [] = {
    0xf1, 0xec, 
};
static const unsigned char gb_chars_ban2609 [] = {
    0xf1, 0xed, 0xf1, 0xee, 0xf1, 0xef, 0xf1, 0xf1, 0xf1, 0xf0, 
    0xc5, 0xd5, 
};
static const unsigned char gb_chars_ban2610 [] = {
    0xf1, 0xf2, 
};
static const unsigned char gb_chars_ban2611 [] = {
    0xb6, 0xfa, 
};
static const unsigned char gb_chars_ban2612 [] = {
    0xf1, 0xf4, 0xd2, 0xae, 0xde, 0xc7, 0xcb, 0xca, 
};
static const unsigned char gb_chars_ban2613 [] = {
    0xb3, 0xdc, 
};
static const unsigned char gb_chars_ban2614 [] = {
    0xb5, 0xa2, 
};
static const unsigned char gb_chars_ban2615 [] = {
    0xb9, 0xa2, 
};
static const unsigned char gb_chars_ban2616 [] = {
    0xc4, 0xf4, 0xf1, 0xf5, 
};
static const unsigned char gb_chars_ban2617 [] = {
    0xf1, 0xf6, 
};
static const unsigned char gb_chars_ban2618 [] = {
    0xc1, 0xc4, 0xc1, 0xfb, 0xd6, 0xb0, 0xf1, 0xf7, 
};
static const unsigned char gb_chars_ban2619 [] = {
    0xf1, 0xf8, 
};
static const unsigned char gb_chars_ban2620 [] = {
    0xc1, 0xaa, 
};
static const unsigned char gb_chars_ban2621 [] = {
    0xc6, 0xb8, 
};
static const unsigned char gb_chars_ban2622 [] = {
    0xbe, 0xdb, 
};
static const unsigned char gb_chars_ban2623 [] = {
    0xf1, 0xf9, 0xb4, 0xcf, 
};
static const unsigned char gb_chars_ban2624 [] = {
    0xf1, 0xfa, 
};
static const unsigned char gb_chars_ban2625 [] = {
    0xed, 0xb2, 0xed, 0xb1, 
};
static const unsigned char gb_chars_ban2626 [] = {
    0xcb, 0xe0, 0xd2, 0xde, 
};
static const unsigned char gb_chars_ban2627 [] = {
    0xcb, 0xc1, 0xd5, 0xd8, 
};
static const unsigned char gb_chars_ban2628 [] = {
    0xc8, 0xe2, 
};
static const unsigned char gb_chars_ban2629 [] = {
    0xc0, 0xdf, 0xbc, 0xa1, 
};
static const unsigned char gb_chars_ban2630 [] = {
    0xeb, 0xc1, 
};
static const unsigned char gb_chars_ban2631 [] = {
    0xd0, 0xa4, 
};
static const unsigned char gb_chars_ban2632 [] = {
    0xd6, 0xe2, 
};
static const unsigned char gb_chars_ban2633 [] = {
    0xb6, 0xc7, 0xb8, 0xd8, 0xeb, 0xc0, 0xb8, 0xce, 
};
static const unsigned char gb_chars_ban2634 [] = {
    0xeb, 0xbf, 0xb3, 0xa6, 0xb9, 0xc9, 0xd6, 0xab, 
};
static const unsigned char gb_chars_ban2635 [] = {
    0xb7, 0xf4, 0xb7, 0xca, 
};
static const unsigned char gb_chars_ban2636 [] = {
    0xbc, 0xe7, 0xb7, 0xbe, 0xeb, 0xc6, 
};
static const unsigned char gb_chars_ban2637 [] = {
    0xeb, 0xc7, 0xb0, 0xb9, 0xbf, 0xcf, 
};
static const unsigned char gb_chars_ban2638 [] = {
    0xeb, 0xc5, 0xd3, 0xfd, 
};
static const unsigned char gb_chars_ban2639 [] = {
    0xeb, 0xc8, 
};
static const unsigned char gb_chars_ban2640 [] = {
    0xeb, 0xc9, 
};
static const unsigned char gb_chars_ban2641 [] = {
    0xb7, 0xce, 
};
static const unsigned char gb_chars_ban2642 [] = {
    0xeb, 0xc2, 0xeb, 0xc4, 0xc9, 0xf6, 0xd6, 0xd7, 0xd5, 0xcd, 
    0xd0, 0xb2, 0xeb, 0xcf, 0xce, 0xb8, 0xeb, 0xd0, 
};
static const unsigned char gb_chars_ban2643 [] = {
    0xb5, 0xa8, 
};
static const unsigned char gb_chars_ban2644 [] = {
    0xb1, 0xb3, 0xeb, 0xd2, 0xcc, 0xa5, 
};
static const unsigned char gb_chars_ban2645 [] = {
    0xc5, 0xd6, 0xeb, 0xd3, 
};
static const unsigned char gb_chars_ban2646 [] = {
    0xeb, 0xd1, 0xc5, 0xdf, 0xeb, 0xce, 0xca, 0xa4, 0xeb, 0xd5, 
    0xb0, 0xfb, 
};
static const unsigned char gb_chars_ban2647 [] = {
    0xba, 0xfa, 
};
static const unsigned char gb_chars_ban2648 [] = {
    0xd8, 0xb7, 0xf1, 0xe3, 
};
static const unsigned char gb_chars_ban2649 [] = {
    0xeb, 0xca, 0xeb, 0xcb, 0xeb, 0xcc, 0xeb, 0xcd, 0xeb, 0xd6, 
    0xe6, 0xc0, 0xeb, 0xd9, 
};
static const unsigned char gb_chars_ban2650 [] = {
    0xbf, 0xe8, 0xd2, 0xc8, 0xeb, 0xd7, 0xeb, 0xdc, 0xb8, 0xec, 
    0xeb, 0xd8, 
};
static const unsigned char gb_chars_ban2651 [] = {
    0xbd, 0xba, 
};
static const unsigned char gb_chars_ban2652 [] = {
    0xd0, 0xd8, 
};
static const unsigned char gb_chars_ban2653 [] = {
    0xb0, 0xb7, 
};
static const unsigned char gb_chars_ban2654 [] = {
    0xeb, 0xdd, 0xc4, 0xdc, 
};
static const unsigned char gb_chars_ban2655 [] = {
    0xd6, 0xac, 
};
static const unsigned char gb_chars_ban2656 [] = {
    0xb4, 0xe0, 
};
static const unsigned char gb_chars_ban2657 [] = {
    0xc2, 0xf6, 0xbc, 0xb9, 
};
static const unsigned char gb_chars_ban2658 [] = {
    0xeb, 0xda, 0xeb, 0xdb, 0xd4, 0xe0, 0xc6, 0xea, 0xc4, 0xd4, 
    0xeb, 0xdf, 0xc5, 0xa7, 0xd9, 0xf5, 
};
static const unsigned char gb_chars_ban2659 [] = {
    0xb2, 0xb1, 
};
static const unsigned char gb_chars_ban2660 [] = {
    0xeb, 0xe4, 
};
static const unsigned char gb_chars_ban2661 [] = {
    0xbd, 0xc5, 
};
static const unsigned char gb_chars_ban2662 [] = {
    0xeb, 0xe2, 
};
static const unsigned char gb_chars_ban2663 [] = {
    0xeb, 0xe3, 
};
static const unsigned char gb_chars_ban2664 [] = {
    0xb8, 0xac, 
};
static const unsigned char gb_chars_ban2665 [] = {
    0xcd, 0xd1, 0xeb, 0xe5, 
};
static const unsigned char gb_chars_ban2666 [] = {
    0xeb, 0xe1, 
};
static const unsigned char gb_chars_ban2667 [] = {
    0xc1, 0xb3, 
};
static const unsigned char gb_chars_ban2668 [] = {
    0xc6, 0xa2, 
};
static const unsigned char gb_chars_ban2669 [] = {
    0xcc, 0xf3, 
};
static const unsigned char gb_chars_ban2670 [] = {
    0xeb, 0xe6, 
};
static const unsigned char gb_chars_ban2671 [] = {
    0xc0, 0xb0, 0xd2, 0xb8, 0xeb, 0xe7, 
};
static const unsigned char gb_chars_ban2672 [] = {
    0xb8, 0xaf, 0xb8, 0xad, 
};
static const unsigned char gb_chars_ban2673 [] = {
    0xeb, 0xe8, 0xc7, 0xbb, 0xcd, 0xf3, 
};
static const unsigned char gb_chars_ban2674 [] = {
    0xeb, 0xea, 0xeb, 0xeb, 
};
static const unsigned char gb_chars_ban2675 [] = {
    0xeb, 0xed, 
};
static const unsigned char gb_chars_ban2676 [] = {
    0xd0, 0xc8, 
};
static const unsigned char gb_chars_ban2677 [] = {
    0xeb, 0xf2, 
};
static const unsigned char gb_chars_ban2678 [] = {
    0xeb, 0xee, 
};
static const unsigned char gb_chars_ban2679 [] = {
    0xeb, 0xf1, 0xc8, 0xf9, 
};
static const unsigned char gb_chars_ban2680 [] = {
    0xd1, 0xfc, 0xeb, 0xec, 
};
static const unsigned char gb_chars_ban2681 [] = {
    0xeb, 0xe9, 
};
static const unsigned char gb_chars_ban2682 [] = {
    0xb8, 0xb9, 0xcf, 0xd9, 0xc4, 0xe5, 0xeb, 0xef, 0xeb, 0xf0, 
    0xcc, 0xda, 0xcd, 0xc8, 0xb0, 0xf2, 
};
static const unsigned char gb_chars_ban2683 [] = {
    0xeb, 0xf6, 
};
static const unsigned char gb_chars_ban2684 [] = {
    0xeb, 0xf5, 
};
static const unsigned char gb_chars_ban2685 [] = {
    0xb2, 0xb2, 
};
static const unsigned char gb_chars_ban2686 [] = {
    0xb8, 0xe0, 
};
static const unsigned char gb_chars_ban2687 [] = {
    0xeb, 0xf7, 
};
static const unsigned char gb_chars_ban2688 [] = {
    0xb1, 0xec, 
};
static const unsigned char gb_chars_ban2689 [] = {
    0xcc, 0xc5, 0xc4, 0xa4, 0xcf, 0xa5, 
};
static const unsigned char gb_chars_ban2690 [] = {
    0xeb, 0xf9, 
};
static const unsigned char gb_chars_ban2691 [] = {
    0xec, 0xa2, 
};
static const unsigned char gb_chars_ban2692 [] = {
    0xc5, 0xf2, 
};
static const unsigned char gb_chars_ban2693 [] = {
    0xeb, 0xfa, 
};
static const unsigned char gb_chars_ban2694 [] = {
    0xc9, 0xc5, 
};
static const unsigned char gb_chars_ban2695 [] = {
    0xe2, 0xdf, 0xeb, 0xfe, 
};
static const unsigned char gb_chars_ban2696 [] = {
    0xcd, 0xce, 0xec, 0xa1, 0xb1, 0xdb, 0xd3, 0xb7, 
};
static const unsigned char gb_chars_ban2697 [] = {
    0xd2, 0xdc, 
};
static const unsigned char gb_chars_ban2698 [] = {
    0xeb, 0xfd, 
};
static const unsigned char gb_chars_ban2699 [] = {
    0xeb, 0xfb, 
};
static const unsigned char gb_chars_ban2700 [] = {
    0xb3, 0xbc, 
};
static const unsigned char gb_chars_ban2701 [] = {
    0xea, 0xb0, 
};
static const unsigned char gb_chars_ban2702 [] = {
    0xd7, 0xd4, 
};
static const unsigned char gb_chars_ban2703 [] = {
    0xf4, 0xab, 0xb3, 0xf4, 
};
static const unsigned char gb_chars_ban2704 [] = {
    0xd6, 0xc1, 0xd6, 0xc2, 
};
static const unsigned char gb_chars_ban2705 [] = {
    0xd5, 0xe9, 0xbe, 0xca, 
};
static const unsigned char gb_chars_ban2706 [] = {
    0xf4, 0xa7, 
};
static const unsigned char gb_chars_ban2707 [] = {
    0xd2, 0xa8, 0xf4, 0xa8, 0xf4, 0xa9, 
};
static const unsigned char gb_chars_ban2708 [] = {
    0xf4, 0xaa, 0xbe, 0xcb, 0xd3, 0xdf, 
};
static const unsigned char gb_chars_ban2709 [] = {
    0xc9, 0xe0, 0xc9, 0xe1, 
};
static const unsigned char gb_chars_ban2710 [] = {
    0xf3, 0xc2, 
};
static const unsigned char gb_chars_ban2711 [] = {
    0xca, 0xe6, 
};
static const unsigned char gb_chars_ban2712 [] = {
    0xcc, 0xf2, 
};
static const unsigned char gb_chars_ban2713 [] = {
    0xe2, 0xb6, 0xcb, 0xb4, 
};
static const unsigned char gb_chars_ban2714 [] = {
    0xce, 0xe8, 0xd6, 0xdb, 
};
static const unsigned char gb_chars_ban2715 [] = {
    0xf4, 0xad, 0xf4, 0xae, 0xf4, 0xaf, 
};
static const unsigned char gb_chars_ban2716 [] = {
    0xf4, 0xb2, 
};
static const unsigned char gb_chars_ban2717 [] = {
    0xba, 0xbd, 0xf4, 0xb3, 0xb0, 0xe3, 0xf4, 0xb0, 
};
static const unsigned char gb_chars_ban2718 [] = {
    0xf4, 0xb1, 0xbd, 0xa2, 0xb2, 0xd5, 
};
static const unsigned char gb_chars_ban2719 [] = {
    0xf4, 0xb6, 0xf4, 0xb7, 0xb6, 0xe6, 0xb2, 0xb0, 0xcf, 0xcf, 
    0xf4, 0xb4, 0xb4, 0xac, 
};
static const unsigned char gb_chars_ban2720 [] = {
    0xf4, 0xb5, 
};
static const unsigned char gb_chars_ban2721 [] = {
    0xf4, 0xb8, 
};
static const unsigned char gb_chars_ban2722 [] = {
    0xf4, 0xb9, 
};
static const unsigned char gb_chars_ban2723 [] = {
    0xcd, 0xa7, 
};
static const unsigned char gb_chars_ban2724 [] = {
    0xf4, 0xba, 
};
static const unsigned char gb_chars_ban2725 [] = {
    0xf4, 0xbb, 
};
static const unsigned char gb_chars_ban2726 [] = {
    0xf4, 0xbc, 
};
static const unsigned char gb_chars_ban2727 [] = {
    0xcb, 0xd2, 
};
static const unsigned char gb_chars_ban2728 [] = {
    0xf4, 0xbd, 
};
static const unsigned char gb_chars_ban2729 [] = {
    0xf4, 0xbe, 
};
static const unsigned char gb_chars_ban2730 [] = {
    0xf4, 0xbf, 
};
static const unsigned char gb_chars_ban2731 [] = {
    0xf4, 0xde, 0xc1, 0xbc, 0xbc, 0xe8, 
};
static const unsigned char gb_chars_ban2732 [] = {
    0xc9, 0xab, 0xd1, 0xde, 0xe5, 0xf5, 
};
static const unsigned char gb_chars_ban2733 [] = {
    0xdc, 0xb3, 0xd2, 0xd5, 
};
static const unsigned char gb_chars_ban2734 [] = {
    0xdc, 0xb4, 0xb0, 0xac, 0xdc, 0xb5, 
};
static const unsigned char gb_chars_ban2735 [] = {
    0xbd, 0xda, 
};
static const unsigned char gb_chars_ban2736 [] = {
    0xdc, 0xb9, 
};
static const unsigned char gb_chars_ban2737 [] = {
    0xd8, 0xc2, 
};
static const unsigned char gb_chars_ban2738 [] = {
    0xdc, 0xb7, 0xd3, 0xf3, 
};
static const unsigned char gb_chars_ban2739 [] = {
    0xc9, 0xd6, 0xdc, 0xba, 0xdc, 0xb6, 
};
static const unsigned char gb_chars_ban2740 [] = {
    0xdc, 0xbb, 0xc3, 0xa2, 
};
static const unsigned char gb_chars_ban2741 [] = {
    0xdc, 0xbc, 0xdc, 0xc5, 0xdc, 0xbd, 
};
static const unsigned char gb_chars_ban2742 [] = {
    0xce, 0xdf, 0xd6, 0xa5, 
};
static const unsigned char gb_chars_ban2743 [] = {
    0xdc, 0xcf, 
};
static const unsigned char gb_chars_ban2744 [] = {
    0xdc, 0xcd, 
};
static const unsigned char gb_chars_ban2745 [] = {
    0xdc, 0xd2, 0xbd, 0xe6, 0xc2, 0xab, 
};
static const unsigned char gb_chars_ban2746 [] = {
    0xdc, 0xb8, 0xdc, 0xcb, 0xdc, 0xce, 0xdc, 0xbe, 0xb7, 0xd2, 
    0xb0, 0xc5, 0xdc, 0xc7, 0xd0, 0xbe, 0xdc, 0xc1, 0xbb, 0xa8, 
};
static const unsigned char gb_chars_ban2747 [] = {
    0xb7, 0xbc, 0xdc, 0xcc, 
};
static const unsigned char gb_chars_ban2748 [] = {
    0xdc, 0xc6, 0xdc, 0xbf, 0xc7, 0xdb, 
};
static const unsigned char gb_chars_ban2749 [] = {
    0xd1, 0xbf, 0xdc, 0xc0, 
};
static const unsigned char gb_chars_ban2750 [] = {
    0xdc, 0xca, 
};
static const unsigned char gb_chars_ban2751 [] = {
    0xdc, 0xd0, 
};
static const unsigned char gb_chars_ban2752 [] = {
    0xce, 0xad, 0xdc, 0xc2, 
};
static const unsigned char gb_chars_ban2753 [] = {
    0xdc, 0xc3, 0xdc, 0xc8, 0xdc, 0xc9, 0xb2, 0xd4, 0xdc, 0xd1, 
    0xcb, 0xd5, 
};
static const unsigned char gb_chars_ban2754 [] = {
    0xd4, 0xb7, 0xdc, 0xdb, 0xdc, 0xdf, 0xcc, 0xa6, 0xdc, 0xe6, 
};
static const unsigned char gb_chars_ban2755 [] = {
    0xc3, 0xe7, 0xdc, 0xdc, 
};
static const unsigned char gb_chars_ban2756 [] = {
    0xbf, 0xc1, 0xdc, 0xd9, 
};
static const unsigned char gb_chars_ban2757 [] = {
    0xb0, 0xfa, 0xb9, 0xb6, 0xdc, 0xe5, 0xdc, 0xd3, 
};
static const unsigned char gb_chars_ban2758 [] = {
    0xdc, 0xc4, 0xdc, 0xd6, 0xc8, 0xf4, 0xbf, 0xe0, 
};
static const unsigned char gb_chars_ban2759 [] = {
    0xc9, 0xbb, 
};
static const unsigned char gb_chars_ban2760 [] = {
    0xb1, 0xbd, 
};
static const unsigned char gb_chars_ban2761 [] = {
    0xd3, 0xa2, 
};
static const unsigned char gb_chars_ban2762 [] = {
    0xdc, 0xda, 
};
static const unsigned char gb_chars_ban2763 [] = {
    0xdc, 0xd5, 
};
static const unsigned char gb_chars_ban2764 [] = {
    0xc6, 0xbb, 
};
static const unsigned char gb_chars_ban2765 [] = {
    0xdc, 0xde, 
};
static const unsigned char gb_chars_ban2766 [] = {
    0xd7, 0xc2, 0xc3, 0xaf, 0xb7, 0xb6, 0xc7, 0xd1, 0xc3, 0xa9, 
    0xdc, 0xe2, 0xdc, 0xd8, 0xdc, 0xeb, 0xdc, 0xd4, 
};
static const unsigned char gb_chars_ban2767 [] = {
    0xdc, 0xdd, 
};
static const unsigned char gb_chars_ban2768 [] = {
    0xbe, 0xa5, 0xdc, 0xd7, 
};
static const unsigned char gb_chars_ban2769 [] = {
    0xdc, 0xe0, 
};
static const unsigned char gb_chars_ban2770 [] = {
    0xdc, 0xe3, 0xdc, 0xe4, 
};
static const unsigned char gb_chars_ban2771 [] = {
    0xdc, 0xf8, 
};
static const unsigned char gb_chars_ban2772 [] = {
    0xdc, 0xe1, 0xdd, 0xa2, 0xdc, 0xe7, 
};
static const unsigned char gb_chars_ban2773 [] = {
    0xbc, 0xeb, 0xb4, 0xc4, 
};
static const unsigned char gb_chars_ban2774 [] = {
    0xc3, 0xa3, 0xb2, 0xe7, 0xdc, 0xfa, 
};
static const unsigned char gb_chars_ban2775 [] = {
    0xdc, 0xf2, 
};
static const unsigned char gb_chars_ban2776 [] = {
    0xdc, 0xef, 
};
static const unsigned char gb_chars_ban2777 [] = {
    0xdc, 0xfc, 0xdc, 0xee, 0xd2, 0xf0, 0xb2, 0xe8, 
};
static const unsigned char gb_chars_ban2778 [] = {
    0xc8, 0xd7, 0xc8, 0xe3, 0xdc, 0xfb, 
};
static const unsigned char gb_chars_ban2779 [] = {
    0xdc, 0xed, 
};
static const unsigned char gb_chars_ban2780 [] = {
    0xdc, 0xf7, 
};
static const unsigned char gb_chars_ban2781 [] = {
    0xdc, 0xf5, 
};
static const unsigned char gb_chars_ban2782 [] = {
    0xbe, 0xa3, 0xdc, 0xf4, 
};
static const unsigned char gb_chars_ban2783 [] = {
    0xb2, 0xdd, 
};
static const unsigned char gb_chars_ban2784 [] = {
    0xdc, 0xf3, 0xbc, 0xf6, 0xdc, 0xe8, 0xbb, 0xc4, 
};
static const unsigned char gb_chars_ban2785 [] = {
    0xc0, 0xf3, 
};
static const unsigned char gb_chars_ban2786 [] = {
    0xbc, 0xd4, 0xdc, 0xe9, 0xdc, 0xea, 
};
static const unsigned char gb_chars_ban2787 [] = {
    0xdc, 0xf1, 0xdc, 0xf6, 0xdc, 0xf9, 0xb5, 0xb4, 
};
static const unsigned char gb_chars_ban2788 [] = {
    0xc8, 0xd9, 0xbb, 0xe7, 0xdc, 0xfe, 0xdc, 0xfd, 0xd3, 0xab, 
    0xdd, 0xa1, 0xdd, 0xa3, 0xdd, 0xa5, 0xd2, 0xf1, 0xdd, 0xa4, 
    0xdd, 0xa6, 0xdd, 0xa7, 0xd2, 0xa9, 
};
static const unsigned char gb_chars_ban2789 [] = {
    0xba, 0xc9, 0xdd, 0xa9, 
};
static const unsigned char gb_chars_ban2790 [] = {
    0xdd, 0xb6, 0xdd, 0xb1, 0xdd, 0xb4, 
};
static const unsigned char gb_chars_ban2791 [] = {
    0xdd, 0xb0, 0xc6, 0xce, 
};
static const unsigned char gb_chars_ban2792 [] = {
    0xc0, 0xf2, 
};
static const unsigned char gb_chars_ban2793 [] = {
    0xc9, 0xaf, 
};
static const unsigned char gb_chars_ban2794 [] = {
    0xdc, 0xec, 0xdd, 0xae, 
};
static const unsigned char gb_chars_ban2795 [] = {
    0xdd, 0xb7, 
};
static const unsigned char gb_chars_ban2796 [] = {
    0xdc, 0xf0, 0xdd, 0xaf, 
};
static const unsigned char gb_chars_ban2797 [] = {
    0xdd, 0xb8, 
};
static const unsigned char gb_chars_ban2798 [] = {
    0xdd, 0xac, 
};
static const unsigned char gb_chars_ban2799 [] = {
    0xdd, 0xb9, 0xdd, 0xb3, 0xdd, 0xad, 0xc4, 0xaa, 
};
static const unsigned char gb_chars_ban2800 [] = {
    0xdd, 0xa8, 0xc0, 0xb3, 0xc1, 0xab, 0xdd, 0xaa, 0xdd, 0xab, 
};
static const unsigned char gb_chars_ban2801 [] = {
    0xdd, 0xb2, 0xbb, 0xf1, 0xdd, 0xb5, 0xd3, 0xa8, 0xdd, 0xba, 
};
static const unsigned char gb_chars_ban2802 [] = {
    0xdd, 0xbb, 0xc3, 0xa7, 
};
static const unsigned char gb_chars_ban2803 [] = {
    0xdd, 0xd2, 0xdd, 0xbc, 
};
static const unsigned char gb_chars_ban2804 [] = {
    0xdd, 0xd1, 
};
static const unsigned char gb_chars_ban2805 [] = {
    0xb9, 0xbd, 
};
static const unsigned char gb_chars_ban2806 [] = {
    0xbe, 0xd5, 
};
static const unsigned char gb_chars_ban2807 [] = {
    0xbe, 0xfa, 
};
static const unsigned char gb_chars_ban2808 [] = {
    0xba, 0xca, 
};
static const unsigned char gb_chars_ban2809 [] = {
    0xdd, 0xca, 
};
static const unsigned char gb_chars_ban2810 [] = {
    0xdd, 0xc5, 
};
static const unsigned char gb_chars_ban2811 [] = {
    0xdd, 0xbf, 
};
static const unsigned char gb_chars_ban2812 [] = {
    0xb2, 0xcb, 0xdd, 0xc3, 
};
static const unsigned char gb_chars_ban2813 [] = {
    0xdd, 0xcb, 0xb2, 0xa4, 0xdd, 0xd5, 
};
static const unsigned char gb_chars_ban2814 [] = {
    0xdd, 0xbe, 
};
static const unsigned char gb_chars_ban2815 [] = {
    0xc6, 0xd0, 0xdd, 0xd0, 
};
static const unsigned char gb_chars_ban2816 [] = {
    0xdd, 0xd4, 0xc1, 0xe2, 0xb7, 0xc6, 
};
static const unsigned char gb_chars_ban2817 [] = {
    0xdd, 0xce, 0xdd, 0xcf, 
};
static const unsigned char gb_chars_ban2818 [] = {
    0xdd, 0xc4, 
};
static const unsigned char gb_chars_ban2819 [] = {
    0xdd, 0xbd, 
};
static const unsigned char gb_chars_ban2820 [] = {
    0xdd, 0xcd, 0xcc, 0xd1, 
};
static const unsigned char gb_chars_ban2821 [] = {
    0xdd, 0xc9, 
};
static const unsigned char gb_chars_ban2822 [] = {
    0xdd, 0xc2, 0xc3, 0xc8, 0xc6, 0xbc, 0xce, 0xae, 0xdd, 0xcc, 
};
static const unsigned char gb_chars_ban2823 [] = {
    0xdd, 0xc8, 
};
static const unsigned char gb_chars_ban2824 [] = {
    0xdd, 0xc1, 
};
static const unsigned char gb_chars_ban2825 [] = {
    0xdd, 0xc6, 0xc2, 0xdc, 
};
static const unsigned char gb_chars_ban2826 [] = {
    0xd3, 0xa9, 0xd3, 0xaa, 0xdd, 0xd3, 0xcf, 0xf4, 0xc8, 0xf8, 
};
static const unsigned char gb_chars_ban2827 [] = {
    0xdd, 0xe6, 
};
static const unsigned char gb_chars_ban2828 [] = {
    0xdd, 0xc7, 
};
static const unsigned char gb_chars_ban2829 [] = {
    0xdd, 0xe0, 0xc2, 0xe4, 
};
static const unsigned char gb_chars_ban2830 [] = {
    0xdd, 0xe1, 
};
static const unsigned char gb_chars_ban2831 [] = {
    0xdd, 0xd7, 
};
static const unsigned char gb_chars_ban2832 [] = {
    0xd6, 0xf8, 
};
static const unsigned char gb_chars_ban2833 [] = {
    0xdd, 0xd9, 0xdd, 0xd8, 0xb8, 0xf0, 0xdd, 0xd6, 
};
static const unsigned char gb_chars_ban2834 [] = {
    0xc6, 0xcf, 
};
static const unsigned char gb_chars_ban2835 [] = {
    0xb6, 0xad, 
};
static const unsigned char gb_chars_ban2836 [] = {
    0xdd, 0xe2, 
};
static const unsigned char gb_chars_ban2837 [] = {
    0xba, 0xf9, 0xd4, 0xe1, 0xdd, 0xe7, 
};
static const unsigned char gb_chars_ban2838 [] = {
    0xb4, 0xd0, 
};
static const unsigned char gb_chars_ban2839 [] = {
    0xdd, 0xda, 
};
static const unsigned char gb_chars_ban2840 [] = {
    0xbf, 0xfb, 0xdd, 0xe3, 
};
static const unsigned char gb_chars_ban2841 [] = {
    0xdd, 0xdf, 
};
static const unsigned char gb_chars_ban2842 [] = {
    0xdd, 0xdd, 
};
static const unsigned char gb_chars_ban2843 [] = {
    0xb5, 0xd9, 
};
static const unsigned char gb_chars_ban2844 [] = {
    0xdd, 0xdb, 0xdd, 0xdc, 0xdd, 0xde, 
};
static const unsigned char gb_chars_ban2845 [] = {
    0xbd, 0xaf, 0xdd, 0xe4, 
};
static const unsigned char gb_chars_ban2846 [] = {
    0xdd, 0xe5, 
};
static const unsigned char gb_chars_ban2847 [] = {
    0xdd, 0xf5, 
};
static const unsigned char gb_chars_ban2848 [] = {
    0xc3, 0xc9, 
};
static const unsigned char gb_chars_ban2849 [] = {
    0xcb, 0xe2, 
};
static const unsigned char gb_chars_ban2850 [] = {
    0xdd, 0xf2, 
};
static const unsigned char gb_chars_ban2851 [] = {
    0xd8, 0xe1, 
};
static const unsigned char gb_chars_ban2852 [] = {
    0xc6, 0xd1, 
};
static const unsigned char gb_chars_ban2853 [] = {
    0xdd, 0xf4, 
};
static const unsigned char gb_chars_ban2854 [] = {
    0xd5, 0xf4, 0xdd, 0xf3, 0xdd, 0xf0, 
};
static const unsigned char gb_chars_ban2855 [] = {
    0xdd, 0xec, 
};
static const unsigned char gb_chars_ban2856 [] = {
    0xdd, 0xef, 
};
static const unsigned char gb_chars_ban2857 [] = {
    0xdd, 0xe8, 
};
static const unsigned char gb_chars_ban2858 [] = {
    0xd0, 0xee, 
};
static const unsigned char gb_chars_ban2859 [] = {
    0xc8, 0xd8, 0xdd, 0xee, 
};
static const unsigned char gb_chars_ban2860 [] = {
    0xdd, 0xe9, 
};
static const unsigned char gb_chars_ban2861 [] = {
    0xdd, 0xea, 0xcb, 0xf2, 
};
static const unsigned char gb_chars_ban2862 [] = {
    0xdd, 0xed, 
};
static const unsigned char gb_chars_ban2863 [] = {
    0xb1, 0xcd, 
};
static const unsigned char gb_chars_ban2864 [] = {
    0xc0, 0xb6, 
};
static const unsigned char gb_chars_ban2865 [] = {
    0xbc, 0xbb, 0xdd, 0xf1, 
};
static const unsigned char gb_chars_ban2866 [] = {
    0xdd, 0xf7, 
};
static const unsigned char gb_chars_ban2867 [] = {
    0xdd, 0xf6, 0xdd, 0xeb, 
};
static const unsigned char gb_chars_ban2868 [] = {
    0xc5, 0xee, 
};
static const unsigned char gb_chars_ban2869 [] = {
    0xdd, 0xfb, 
};
static const unsigned char gb_chars_ban2870 [] = {
    0xde, 0xa4, 
};
static const unsigned char gb_chars_ban2871 [] = {
    0xde, 0xa3, 
};
static const unsigned char gb_chars_ban2872 [] = {
    0xdd, 0xf8, 
};
static const unsigned char gb_chars_ban2873 [] = {
    0xc3, 0xef, 
};
static const unsigned char gb_chars_ban2874 [] = {
    0xc2, 0xfb, 
};
static const unsigned char gb_chars_ban2875 [] = {
    0xd5, 0xe1, 
};
static const unsigned char gb_chars_ban2876 [] = {
    0xce, 0xb5, 
};
static const unsigned char gb_chars_ban2877 [] = {
    0xdd, 0xfd, 
};
static const unsigned char gb_chars_ban2878 [] = {
    0xb2, 0xcc, 
};
static const unsigned char gb_chars_ban2879 [] = {
    0xc4, 0xe8, 0xca, 0xdf, 
};
static const unsigned char gb_chars_ban2880 [] = {
    0xc7, 0xbe, 0xdd, 0xfa, 0xdd, 0xfc, 0xdd, 0xfe, 0xde, 0xa2, 
    0xb0, 0xaa, 0xb1, 0xce, 
};
static const unsigned char gb_chars_ban2881 [] = {
    0xde, 0xac, 
};
static const unsigned char gb_chars_ban2882 [] = {
    0xde, 0xa6, 0xbd, 0xb6, 0xc8, 0xef, 
};
static const unsigned char gb_chars_ban2883 [] = {
    0xde, 0xa1, 
};
static const unsigned char gb_chars_ban2884 [] = {
    0xde, 0xa5, 
};
static const unsigned char gb_chars_ban2885 [] = {
    0xde, 0xa9, 
};
static const unsigned char gb_chars_ban2886 [] = {
    0xde, 0xa8, 
};
static const unsigned char gb_chars_ban2887 [] = {
    0xde, 0xa7, 
};
static const unsigned char gb_chars_ban2888 [] = {
    0xde, 0xad, 
};
static const unsigned char gb_chars_ban2889 [] = {
    0xd4, 0xcc, 
};
static const unsigned char gb_chars_ban2890 [] = {
    0xde, 0xb3, 0xde, 0xaa, 0xde, 0xae, 
};
static const unsigned char gb_chars_ban2891 [] = {
    0xc0, 0xd9, 
};
static const unsigned char gb_chars_ban2892 [] = {
    0xb1, 0xa1, 0xde, 0xb6, 
};
static const unsigned char gb_chars_ban2893 [] = {
    0xde, 0xb1, 
};
static const unsigned char gb_chars_ban2894 [] = {
    0xde, 0xb2, 
};
static const unsigned char gb_chars_ban2895 [] = {
    0xd1, 0xa6, 0xde, 0xb5, 
};
static const unsigned char gb_chars_ban2896 [] = {
    0xde, 0xaf, 
};
static const unsigned char gb_chars_ban2897 [] = {
    0xde, 0xb0, 
};
static const unsigned char gb_chars_ban2898 [] = {
    0xd0, 0xbd, 
};
static const unsigned char gb_chars_ban2899 [] = {
    0xde, 0xb4, 0xca, 0xed, 0xde, 0xb9, 
};
static const unsigned char gb_chars_ban2900 [] = {
    0xde, 0xb8, 
};
static const unsigned char gb_chars_ban2901 [] = {
    0xde, 0xb7, 
};
static const unsigned char gb_chars_ban2902 [] = {
    0xde, 0xbb, 
};
static const unsigned char gb_chars_ban2903 [] = {
    0xbd, 0xe5, 
};
static const unsigned char gb_chars_ban2904 [] = {
    0xb2, 0xd8, 0xc3, 0xea, 
};
static const unsigned char gb_chars_ban2905 [] = {
    0xde, 0xba, 
};
static const unsigned char gb_chars_ban2906 [] = {
    0xc5, 0xba, 
};
static const unsigned char gb_chars_ban2907 [] = {
    0xde, 0xbc, 
};
static const unsigned char gb_chars_ban2908 [] = {
    0xcc, 0xd9, 
};
static const unsigned char gb_chars_ban2909 [] = {
    0xb7, 0xaa, 
};
static const unsigned char gb_chars_ban2910 [] = {
    0xd4, 0xe5, 
};
static const unsigned char gb_chars_ban2911 [] = {
    0xde, 0xbd, 
};
static const unsigned char gb_chars_ban2912 [] = {
    0xde, 0xbf, 
};
static const unsigned char gb_chars_ban2913 [] = {
    0xc4, 0xa2, 
};
static const unsigned char gb_chars_ban2914 [] = {
    0xde, 0xc1, 
};
static const unsigned char gb_chars_ban2915 [] = {
    0xde, 0xbe, 
};
static const unsigned char gb_chars_ban2916 [] = {
    0xde, 0xc0, 
};
static const unsigned char gb_chars_ban2917 [] = {
    0xd5, 0xba, 
};
static const unsigned char gb_chars_ban2918 [] = {
    0xde, 0xc2, 
};
static const unsigned char gb_chars_ban2919 [] = {
    0xf2, 0xae, 0xbb, 0xa2, 0xc2, 0xb2, 0xc5, 0xb0, 0xc2, 0xc7, 
};
static const unsigned char gb_chars_ban2920 [] = {
    0xf2, 0xaf, 
};
static const unsigned char gb_chars_ban2921 [] = {
    0xd0, 0xe9, 
};
static const unsigned char gb_chars_ban2922 [] = {
    0xd3, 0xdd, 
};
static const unsigned char gb_chars_ban2923 [] = {
    0xeb, 0xbd, 
};
static const unsigned char gb_chars_ban2924 [] = {
    0xb3, 0xe6, 0xf2, 0xb0, 
};
static const unsigned char gb_chars_ban2925 [] = {
    0xf2, 0xb1, 
};
static const unsigned char gb_chars_ban2926 [] = {
    0xca, 0xad, 
};
static const unsigned char gb_chars_ban2927 [] = {
    0xba, 0xe7, 0xf2, 0xb3, 0xf2, 0xb5, 0xf2, 0xb4, 0xcb, 0xe4, 
    0xcf, 0xba, 0xf2, 0xb2, 0xca, 0xb4, 0xd2, 0xcf, 0xc2, 0xec, 
};
static const unsigned char gb_chars_ban2928 [] = {
    0xce, 0xc3, 0xf2, 0xb8, 0xb0, 0xf6, 0xf2, 0xb7, 
};
static const unsigned char gb_chars_ban2929 [] = {
    0xf2, 0xbe, 
};
static const unsigned char gb_chars_ban2930 [] = {
    0xb2, 0xcf, 
};
static const unsigned char gb_chars_ban2931 [] = {
    0xd1, 0xc1, 0xf2, 0xba, 
};
static const unsigned char gb_chars_ban2932 [] = {
    0xf2, 0xbc, 0xd4, 0xe9, 
};
static const unsigned char gb_chars_ban2933 [] = {
    0xf2, 0xbb, 0xf2, 0xb6, 0xf2, 0xbf, 0xf2, 0xbd, 
};
static const unsigned char gb_chars_ban2934 [] = {
    0xf2, 0xb9, 
};
static const unsigned char gb_chars_ban2935 [] = {
    0xf2, 0xc7, 0xf2, 0xc4, 0xf2, 0xc6, 
};
static const unsigned char gb_chars_ban2936 [] = {
    0xf2, 0xca, 0xf2, 0xc2, 0xf2, 0xc0, 
};
static const unsigned char gb_chars_ban2937 [] = {
    0xf2, 0xc5, 
};
static const unsigned char gb_chars_ban2938 [] = {
    0xd6, 0xfb, 
};
static const unsigned char gb_chars_ban2939 [] = {
    0xf2, 0xc1, 
};
static const unsigned char gb_chars_ban2940 [] = {
    0xc7, 0xf9, 0xc9, 0xdf, 
};
static const unsigned char gb_chars_ban2941 [] = {
    0xf2, 0xc8, 0xb9, 0xc6, 0xb5, 0xb0, 
};
static const unsigned char gb_chars_ban2942 [] = {
    0xf2, 0xc3, 0xf2, 0xc9, 0xf2, 0xd0, 0xf2, 0xd6, 
};
static const unsigned char gb_chars_ban2943 [] = {
    0xbb, 0xd7, 
};
static const unsigned char gb_chars_ban2944 [] = {
    0xf2, 0xd5, 0xcd, 0xdc, 
};
static const unsigned char gb_chars_ban2945 [] = {
    0xd6, 0xeb, 
};
static const unsigned char gb_chars_ban2946 [] = {
    0xf2, 0xd2, 0xf2, 0xd4, 
};
static const unsigned char gb_chars_ban2947 [] = {
    0xb8, 0xf2, 
};
static const unsigned char gb_chars_ban2948 [] = {
    0xf2, 0xcb, 
};
static const unsigned char gb_chars_ban2949 [] = {
    0xf2, 0xce, 0xc2, 0xf9, 
};
static const unsigned char gb_chars_ban2950 [] = {
    0xd5, 0xdd, 0xf2, 0xcc, 0xf2, 0xcd, 0xf2, 0xcf, 0xf2, 0xd3, 
};
static const unsigned char gb_chars_ban2951 [] = {
    0xf2, 0xd9, 0xd3, 0xbc, 
};
static const unsigned char gb_chars_ban2952 [] = {
    0xb6, 0xea, 
};
static const unsigned char gb_chars_ban2953 [] = {
    0xca, 0xf1, 
};
static const unsigned char gb_chars_ban2954 [] = {
    0xb7, 0xe4, 0xf2, 0xd7, 
};
static const unsigned char gb_chars_ban2955 [] = {
    0xf2, 0xd8, 0xf2, 0xda, 0xf2, 0xdd, 0xf2, 0xdb, 
};
static const unsigned char gb_chars_ban2956 [] = {
    0xf2, 0xdc, 
};
static const unsigned char gb_chars_ban2957 [] = {
    0xd1, 0xd1, 0xf2, 0xd1, 
};
static const unsigned char gb_chars_ban2958 [] = {
    0xcd, 0xc9, 
};
static const unsigned char gb_chars_ban2959 [] = {
    0xce, 0xcf, 0xd6, 0xa9, 
};
static const unsigned char gb_chars_ban2960 [] = {
    0xf2, 0xe3, 
};
static const unsigned char gb_chars_ban2961 [] = {
    0xc3, 0xdb, 
};
static const unsigned char gb_chars_ban2962 [] = {
    0xf2, 0xe0, 
};
static const unsigned char gb_chars_ban2963 [] = {
    0xc0, 0xaf, 0xf2, 0xec, 0xf2, 0xde, 
};
static const unsigned char gb_chars_ban2964 [] = {
    0xf2, 0xe1, 
};
static const unsigned char gb_chars_ban2965 [] = {
    0xf2, 0xe8, 
};
static const unsigned char gb_chars_ban2966 [] = {
    0xf2, 0xe2, 
};
static const unsigned char gb_chars_ban2967 [] = {
    0xf2, 0xe7, 
};
static const unsigned char gb_chars_ban2968 [] = {
    0xf2, 0xe6, 
};
static const unsigned char gb_chars_ban2969 [] = {
    0xf2, 0xe9, 
};
static const unsigned char gb_chars_ban2970 [] = {
    0xf2, 0xdf, 
};
static const unsigned char gb_chars_ban2971 [] = {
    0xf2, 0xe4, 0xf2, 0xea, 
};
static const unsigned char gb_chars_ban2972 [] = {
    0xd3, 0xac, 0xf2, 0xe5, 0xb2, 0xf5, 
};
static const unsigned char gb_chars_ban2973 [] = {
    0xf2, 0xf2, 
};
static const unsigned char gb_chars_ban2974 [] = {
    0xd0, 0xab, 
};
static const unsigned char gb_chars_ban2975 [] = {
    0xf2, 0xf5, 
};
static const unsigned char gb_chars_ban2976 [] = {
    0xbb, 0xc8, 
};
static const unsigned char gb_chars_ban2977 [] = {
    0xf2, 0xf9, 
};
static const unsigned char gb_chars_ban2978 [] = {
    0xf2, 0xf0, 
};
static const unsigned char gb_chars_ban2979 [] = {
    0xf2, 0xf6, 0xf2, 0xf8, 0xf2, 0xfa, 
};
static const unsigned char gb_chars_ban2980 [] = {
    0xf2, 0xf3, 
};
static const unsigned char gb_chars_ban2981 [] = {
    0xf2, 0xf1, 
};
static const unsigned char gb_chars_ban2982 [] = {
    0xba, 0xfb, 
};
static const unsigned char gb_chars_ban2983 [] = {
    0xb5, 0xfb, 
};
static const unsigned char gb_chars_ban2984 [] = {
    0xf2, 0xef, 0xf2, 0xf7, 0xf2, 0xed, 0xf2, 0xee, 
};
static const unsigned char gb_chars_ban2985 [] = {
    0xf2, 0xeb, 0xf3, 0xa6, 
};
static const unsigned char gb_chars_ban2986 [] = {
    0xf3, 0xa3, 
};
static const unsigned char gb_chars_ban2987 [] = {
    0xf3, 0xa2, 
};
static const unsigned char gb_chars_ban2988 [] = {
    0xf2, 0xf4, 
};
static const unsigned char gb_chars_ban2989 [] = {
    0xc8, 0xda, 
};
static const unsigned char gb_chars_ban2990 [] = {
    0xf2, 0xfb, 
};
static const unsigned char gb_chars_ban2991 [] = {
    0xf3, 0xa5, 
};
static const unsigned char gb_chars_ban2992 [] = {
    0xc3, 0xf8, 
};
static const unsigned char gb_chars_ban2993 [] = {
    0xf2, 0xfd, 
};
static const unsigned char gb_chars_ban2994 [] = {
    0xf3, 0xa7, 0xf3, 0xa9, 0xf3, 0xa4, 
};
static const unsigned char gb_chars_ban2995 [] = {
    0xf2, 0xfc, 
};
static const unsigned char gb_chars_ban2996 [] = {
    0xf3, 0xab, 
};
static const unsigned char gb_chars_ban2997 [] = {
    0xf3, 0xaa, 
};
static const unsigned char gb_chars_ban2998 [] = {
    0xc2, 0xdd, 
};
static const unsigned char gb_chars_ban2999 [] = {
    0xf3, 0xae, 
};
static const unsigned char gb_chars_ban3000 [] = {
    0xf3, 0xb0, 
};
static const unsigned char gb_chars_ban3001 [] = {
    0xf3, 0xa1, 
};
static const unsigned char gb_chars_ban3002 [] = {
    0xf3, 0xb1, 0xf3, 0xac, 
};
static const unsigned char gb_chars_ban3003 [] = {
    0xf3, 0xaf, 0xf2, 0xfe, 0xf3, 0xad, 
};
static const unsigned char gb_chars_ban3004 [] = {
    0xf3, 0xb2, 
};
static const unsigned char gb_chars_ban3005 [] = {
    0xf3, 0xb4, 
};
static const unsigned char gb_chars_ban3006 [] = {
    0xf3, 0xa8, 
};
static const unsigned char gb_chars_ban3007 [] = {
    0xf3, 0xb3, 
};
static const unsigned char gb_chars_ban3008 [] = {
    0xf3, 0xb5, 
};
static const unsigned char gb_chars_ban3009 [] = {
    0xd0, 0xb7, 
};
static const unsigned char gb_chars_ban3010 [] = {
    0xf3, 0xb8, 
};
static const unsigned char gb_chars_ban3011 [] = {
    0xd9, 0xf9, 
};
static const unsigned char gb_chars_ban3012 [] = {
    0xf3, 0xb9, 
};
static const unsigned char gb_chars_ban3013 [] = {
    0xf3, 0xb7, 
};
static const unsigned char gb_chars_ban3014 [] = {
    0xc8, 0xe4, 0xf3, 0xb6, 
};
static const unsigned char gb_chars_ban3015 [] = {
    0xf3, 0xba, 
};
static const unsigned char gb_chars_ban3016 [] = {
    0xf3, 0xbb, 0xb4, 0xc0, 
};
static const unsigned char gb_chars_ban3017 [] = {
    0xee, 0xc3, 
};
static const unsigned char gb_chars_ban3018 [] = {
    0xf3, 0xbc, 
};
static const unsigned char gb_chars_ban3019 [] = {
    0xf3, 0xbd, 
};
static const unsigned char gb_chars_ban3020 [] = {
    0xd1, 0xaa, 
};
static const unsigned char gb_chars_ban3021 [] = {
    0xf4, 0xac, 0xd0, 0xc6, 
};
static const unsigned char gb_chars_ban3022 [] = {
    0xd0, 0xd0, 0xd1, 0xdc, 
};
static const unsigned char gb_chars_ban3023 [] = {
    0xcf, 0xce, 
};
static const unsigned char gb_chars_ban3024 [] = {
    0xbd, 0xd6, 
};
static const unsigned char gb_chars_ban3025 [] = {
    0xd1, 0xc3, 
};
static const unsigned char gb_chars_ban3026 [] = {
    0xba, 0xe2, 0xe1, 0xe9, 0xd2, 0xc2, 0xf1, 0xc2, 0xb2, 0xb9, 
};
static const unsigned char gb_chars_ban3027 [] = {
    0xb1, 0xed, 0xf1, 0xc3, 
};
static const unsigned char gb_chars_ban3028 [] = {
    0xc9, 0xc0, 0xb3, 0xc4, 
};
static const unsigned char gb_chars_ban3029 [] = {
    0xd9, 0xf2, 
};
static const unsigned char gb_chars_ban3030 [] = {
    0xcb, 0xa5, 
};
static const unsigned char gb_chars_ban3031 [] = {
    0xf1, 0xc4, 
};
static const unsigned char gb_chars_ban3032 [] = {
    0xd6, 0xd4, 
};
static const unsigned char gb_chars_ban3033 [] = {
    0xf1, 0xc5, 0xf4, 0xc0, 0xf1, 0xc6, 
};
static const unsigned char gb_chars_ban3034 [] = {
    0xd4, 0xac, 0xf1, 0xc7, 
};
static const unsigned char gb_chars_ban3035 [] = {
    0xb0, 0xc0, 0xf4, 0xc1, 
};
static const unsigned char gb_chars_ban3036 [] = {
    0xf4, 0xc2, 
};
static const unsigned char gb_chars_ban3037 [] = {
    0xb4, 0xfc, 
};
static const unsigned char gb_chars_ban3038 [] = {
    0xc5, 0xdb, 
};
static const unsigned char gb_chars_ban3039 [] = {
    0xcc, 0xbb, 
};
static const unsigned char gb_chars_ban3040 [] = {
    0xd0, 0xe4, 
};
static const unsigned char gb_chars_ban3041 [] = {
    0xcd, 0xe0, 
};
static const unsigned char gb_chars_ban3042 [] = {
    0xf1, 0xc8, 
};
static const unsigned char gb_chars_ban3043 [] = {
    0xd9, 0xf3, 
};
static const unsigned char gb_chars_ban3044 [] = {
    0xb1, 0xbb, 
};
static const unsigned char gb_chars_ban3045 [] = {
    0xcf, 0xae, 
};
static const unsigned char gb_chars_ban3046 [] = {
    0xb8, 0xa4, 
};
static const unsigned char gb_chars_ban3047 [] = {
    0xf1, 0xca, 
};
static const unsigned char gb_chars_ban3048 [] = {
    0xf1, 0xcb, 
};
static const unsigned char gb_chars_ban3049 [] = {
    0xb2, 0xc3, 0xc1, 0xd1, 
};
static const unsigned char gb_chars_ban3050 [] = {
    0xd7, 0xb0, 0xf1, 0xc9, 
};
static const unsigned char gb_chars_ban3051 [] = {
    0xf1, 0xcc, 
};
static const unsigned char gb_chars_ban3052 [] = {
    0xf1, 0xce, 
};
static const unsigned char gb_chars_ban3053 [] = {
    0xd9, 0xf6, 
};
static const unsigned char gb_chars_ban3054 [] = {
    0xd2, 0xe1, 0xd4, 0xa3, 
};
static const unsigned char gb_chars_ban3055 [] = {
    0xf4, 0xc3, 0xc8, 0xb9, 
};
static const unsigned char gb_chars_ban3056 [] = {
    0xf4, 0xc4, 
};
static const unsigned char gb_chars_ban3057 [] = {
    0xf1, 0xcd, 0xf1, 0xcf, 0xbf, 0xe3, 0xf1, 0xd0, 
};
static const unsigned char gb_chars_ban3058 [] = {
    0xf1, 0xd4, 
};
static const unsigned char gb_chars_ban3059 [] = {
    0xf1, 0xd6, 0xf1, 0xd1, 
};
static const unsigned char gb_chars_ban3060 [] = {
    0xc9, 0xd1, 0xc5, 0xe1, 
};
static const unsigned char gb_chars_ban3061 [] = {
    0xc2, 0xe3, 0xb9, 0xfc, 
};
static const unsigned char gb_chars_ban3062 [] = {
    0xf1, 0xd3, 
};
static const unsigned char gb_chars_ban3063 [] = {
    0xf1, 0xd5, 
};
static const unsigned char gb_chars_ban3064 [] = {
    0xb9, 0xd3, 
};
static const unsigned char gb_chars_ban3065 [] = {
    0xf1, 0xdb, 
};
static const unsigned char gb_chars_ban3066 [] = {
    0xba, 0xd6, 
};
static const unsigned char gb_chars_ban3067 [] = {
    0xb0, 0xfd, 0xf1, 0xd9, 
};
static const unsigned char gb_chars_ban3068 [] = {
    0xf1, 0xd8, 0xf1, 0xd2, 0xf1, 0xda, 
};
static const unsigned char gb_chars_ban3069 [] = {
    0xf1, 0xd7, 
};
static const unsigned char gb_chars_ban3070 [] = {
    0xc8, 0xec, 
};
static const unsigned char gb_chars_ban3071 [] = {
    0xcd, 0xca, 0xf1, 0xdd, 
};
static const unsigned char gb_chars_ban3072 [] = {
    0xe5, 0xbd, 
};
static const unsigned char gb_chars_ban3073 [] = {
    0xf1, 0xdc, 
};
static const unsigned char gb_chars_ban3074 [] = {
    0xf1, 0xde, 
};
static const unsigned char gb_chars_ban3075 [] = {
    0xf1, 0xdf, 
};
static const unsigned char gb_chars_ban3076 [] = {
    0xcf, 0xe5, 
};
static const unsigned char gb_chars_ban3077 [] = {
    0xf4, 0xc5, 0xbd, 0xf3, 
};
static const unsigned char gb_chars_ban3078 [] = {
    0xf1, 0xe0, 
};
static const unsigned char gb_chars_ban3079 [] = {
    0xf1, 0xe1, 
};
static const unsigned char gb_chars_ban3080 [] = {
    0xce, 0xf7, 
};
static const unsigned char gb_chars_ban3081 [] = {
    0xd2, 0xaa, 
};
static const unsigned char gb_chars_ban3082 [] = {
    0xf1, 0xfb, 
};
static const unsigned char gb_chars_ban3083 [] = {
    0xb8, 0xb2, 
};
static const unsigned char gb_chars_ban3084 [] = {
    0xbc, 0xfb, 0xb9, 0xdb, 
};
static const unsigned char gb_chars_ban3085 [] = {
    0xb9, 0xe6, 0xc3, 0xd9, 0xca, 0xd3, 0xea, 0xe8, 0xc0, 0xc0, 
    0xbe, 0xf5, 0xea, 0xe9, 0xea, 0xea, 0xea, 0xeb, 
};
static const unsigned char gb_chars_ban3086 [] = {
    0xea, 0xec, 0xea, 0xed, 0xea, 0xee, 0xea, 0xef, 0xbd, 0xc7, 
};
static const unsigned char gb_chars_ban3087 [] = {
    0xf5, 0xfb, 
};
static const unsigned char gb_chars_ban3088 [] = {
    0xf5, 0xfd, 
};
static const unsigned char gb_chars_ban3089 [] = {
    0xf5, 0xfe, 
};
static const unsigned char gb_chars_ban3090 [] = {
    0xf5, 0xfc, 
};
static const unsigned char gb_chars_ban3091 [] = {
    0xbd, 0xe2, 
};
static const unsigned char gb_chars_ban3092 [] = {
    0xf6, 0xa1, 0xb4, 0xa5, 
};
static const unsigned char gb_chars_ban3093 [] = {
    0xf6, 0xa2, 
};
static const unsigned char gb_chars_ban3094 [] = {
    0xf6, 0xa3, 
};
static const unsigned char gb_chars_ban3095 [] = {
    0xec, 0xb2, 
};
static const unsigned char gb_chars_ban3096 [] = {
    0xd1, 0xd4, 
};
static const unsigned char gb_chars_ban3097 [] = {
    0xd9, 0xea, 
};
static const unsigned char gb_chars_ban3098 [] = {
    0xf6, 0xa4, 
};
static const unsigned char gb_chars_ban3099 [] = {
    0xee, 0xba, 
};
static const unsigned char gb_chars_ban3100 [] = {
    0xd5, 0xb2, 
};
static const unsigned char gb_chars_ban3101 [] = {
    0xd3, 0xfe, 0xcc, 0xdc, 
};
static const unsigned char gb_chars_ban3102 [] = {
    0xca, 0xc4, 
};
static const unsigned char gb_chars_ban3103 [] = {
    0xe5, 0xc0, 
};
static const unsigned char gb_chars_ban3104 [] = {
    0xf6, 0xa5, 
};
static const unsigned char gb_chars_ban3105 [] = {
    0xbe, 0xaf, 
};
static const unsigned char gb_chars_ban3106 [] = {
    0xc6, 0xa9, 
};
static const unsigned char gb_chars_ban3107 [] = {
    0xda, 0xa5, 0xbc, 0xc6, 0xb6, 0xa9, 0xb8, 0xbc, 0xc8, 0xcf, 
    0xbc, 0xa5, 0xda, 0xa6, 0xda, 0xa7, 0xcc, 0xd6, 0xc8, 0xc3, 
    0xda, 0xa8, 0xc6, 0xfd, 
};
static const unsigned char gb_chars_ban3108 [] = {
    0xd1, 0xb5, 0xd2, 0xe9, 0xd1, 0xb6, 0xbc, 0xc7, 
};
static const unsigned char gb_chars_ban3109 [] = {
    0xbd, 0xb2, 0xbb, 0xe4, 0xda, 0xa9, 0xda, 0xaa, 0xd1, 0xc8, 
    0xda, 0xab, 0xd0, 0xed, 0xb6, 0xef, 0xc2, 0xdb, 
};
static const unsigned char gb_chars_ban3110 [] = {
    0xcb, 0xcf, 0xb7, 0xed, 0xc9, 0xe8, 0xb7, 0xc3, 0xbe, 0xf7, 
    0xd6, 0xa4, 0xda, 0xac, 0xda, 0xad, 0xc6, 0xc0, 0xd7, 0xe7, 
    0xca, 0xb6, 
};
static const unsigned char gb_chars_ban3111 [] = {
    0xd5, 0xa9, 0xcb, 0xdf, 0xd5, 0xef, 0xda, 0xae, 0xd6, 0xdf, 
    0xb4, 0xca, 0xda, 0xb0, 0xda, 0xaf, 
};
static const unsigned char gb_chars_ban3112 [] = {
    0xd2, 0xeb, 0xda, 0xb1, 0xda, 0xb2, 0xda, 0xb3, 0xca, 0xd4, 
    0xda, 0xb4, 0xca, 0xab, 0xda, 0xb5, 0xda, 0xb6, 0xb3, 0xcf, 
    0xd6, 0xef, 0xda, 0xb7, 0xbb, 0xb0, 0xb5, 0xae, 0xda, 0xb8, 
    0xda, 0xb9, 0xb9, 0xee, 0xd1, 0xaf, 0xd2, 0xe8, 0xda, 0xba, 
    0xb8, 0xc3, 0xcf, 0xea, 0xb2, 0xef, 0xda, 0xbb, 0xda, 0xbc, 
};
static const unsigned char gb_chars_ban3113 [] = {
    0xbd, 0xeb, 0xce, 0xdc, 0xd3, 0xef, 0xda, 0xbd, 0xce, 0xf3, 
    0xda, 0xbe, 0xd3, 0xd5, 0xbb, 0xe5, 0xda, 0xbf, 0xcb, 0xb5, 
    0xcb, 0xd0, 0xda, 0xc0, 0xc7, 0xeb, 0xd6, 0xee, 0xda, 0xc1, 
    0xc5, 0xb5, 0xb6, 0xc1, 0xda, 0xc2, 0xb7, 0xcc, 0xbf, 0xce, 
    0xda, 0xc3, 0xda, 0xc4, 0xcb, 0xad, 0xda, 0xc5, 0xb5, 0xf7, 
    0xda, 0xc6, 0xc1, 0xc2, 0xd7, 0xbb, 0xda, 0xc7, 0xcc, 0xb8, 
};
static const unsigned char gb_chars_ban3114 [] = {
    0xd2, 0xea, 0xc4, 0xb1, 0xda, 0xc8, 0xb5, 0xfd, 0xbb, 0xd1, 
    0xda, 0xc9, 0xd0, 0xb3, 0xda, 0xca, 0xda, 0xcb, 0xce, 0xbd, 
    0xda, 0xcc, 0xda, 0xcd, 0xda, 0xce, 0xb2, 0xf7, 0xda, 0xd1, 
    0xda, 0xcf, 0xd1, 0xe8, 0xda, 0xd0, 0xc3, 0xd5, 0xda, 0xd2, 
};
static const unsigned char gb_chars_ban3115 [] = {
    0xda, 0xd3, 0xda, 0xd4, 0xda, 0xd5, 0xd0, 0xbb, 0xd2, 0xa5, 
    0xb0, 0xf9, 0xda, 0xd6, 0xc7, 0xab, 0xda, 0xd7, 0xbd, 0xf7, 
    0xc3, 0xa1, 0xda, 0xd8, 0xda, 0xd9, 0xc3, 0xfd, 0xcc, 0xb7, 
    0xda, 0xda, 0xda, 0xdb, 0xc0, 0xbe, 0xc6, 0xd7, 0xda, 0xdc, 
    0xda, 0xdd, 0xc7, 0xb4, 0xda, 0xde, 0xda, 0xdf, 0xb9, 0xc8, 
};
static const unsigned char gb_chars_ban3116 [] = {
    0xbb, 0xed, 
};
static const unsigned char gb_chars_ban3117 [] = {
    0xb6, 0xb9, 0xf4, 0xf8, 
};
static const unsigned char gb_chars_ban3118 [] = {
    0xf4, 0xf9, 
};
static const unsigned char gb_chars_ban3119 [] = {
    0xcd, 0xe3, 
};
static const unsigned char gb_chars_ban3120 [] = {
    0xf5, 0xb9, 
};
static const unsigned char gb_chars_ban3121 [] = {
    0xeb, 0xe0, 
};
static const unsigned char gb_chars_ban3122 [] = {
    0xcf, 0xf3, 0xbb, 0xbf, 
};
static const unsigned char gb_chars_ban3123 [] = {
    0xba, 0xc0, 0xd4, 0xa5, 
};
static const unsigned char gb_chars_ban3124 [] = {
    0xe1, 0xd9, 
};
static const unsigned char gb_chars_ban3125 [] = {
    0xf5, 0xf4, 0xb1, 0xaa, 0xb2, 0xf2, 
};
static const unsigned char gb_chars_ban3126 [] = {
    0xf5, 0xf5, 
};
static const unsigned char gb_chars_ban3127 [] = {
    0xf5, 0xf7, 
};
static const unsigned char gb_chars_ban3128 [] = {
    0xba, 0xd1, 0xf5, 0xf6, 
};
static const unsigned char gb_chars_ban3129 [] = {
    0xc3, 0xb2, 
};
static const unsigned char gb_chars_ban3130 [] = {
    0xf5, 0xf9, 
};
static const unsigned char gb_chars_ban3131 [] = {
    0xf5, 0xf8, 
};
static const unsigned char gb_chars_ban3132 [] = {
    0xb1, 0xb4, 0xd5, 0xea, 0xb8, 0xba, 
};
static const unsigned char gb_chars_ban3133 [] = {
    0xb9, 0xb1, 0xb2, 0xc6, 0xd4, 0xf0, 0xcf, 0xcd, 0xb0, 0xdc, 
    0xd5, 0xcb, 0xbb, 0xf5, 0xd6, 0xca, 0xb7, 0xb7, 0xcc, 0xb0, 
    0xc6, 0xb6, 0xb1, 0xe1, 0xb9, 0xba, 0xd6, 0xfc, 0xb9, 0xe1, 
    0xb7, 0xa1, 0xbc, 0xfa, 0xea, 0xda, 0xea, 0xdb, 0xcc, 0xf9, 
    0xb9, 0xf3, 0xea, 0xdc, 0xb4, 0xfb, 0xc3, 0xb3, 0xb7, 0xd1, 
    0xba, 0xd8, 0xea, 0xdd, 0xd4, 0xf4, 0xea, 0xde, 0xbc, 0xd6, 
    0xbb, 0xdf, 0xea, 0xdf, 0xc1, 0xde, 0xc2, 0xb8, 0xd4, 0xdf, 
    0xd7, 0xca, 0xea, 0xe0, 0xea, 0xe1, 0xea, 0xe4, 0xea, 0xe2, 
    0xea, 0xe3, 0xc9, 0xde, 0xb8, 0xb3, 0xb6, 0xc4, 0xea, 0xe5, 
    0xca, 0xea, 0xc9, 0xcd, 0xb4, 0xcd, 
};
static const unsigned char gb_chars_ban3134 [] = {
    0xe2, 0xd9, 0xc5, 0xe2, 0xea, 0xe6, 0xc0, 0xb5, 
};
static const unsigned char gb_chars_ban3135 [] = {
    0xd7, 0xb8, 0xea, 0xe7, 0xd7, 0xac, 0xc8, 0xfc, 0xd8, 0xd3, 
    0xd8, 0xcd, 0xd4, 0xde, 
};
static const unsigned char gb_chars_ban3136 [] = {
    0xd4, 0xf9, 0xc9, 0xc4, 0xd3, 0xae, 0xb8, 0xd3, 0xb3, 0xe0, 
};
static const unsigned char gb_chars_ban3137 [] = {
    0xc9, 0xe2, 0xf4, 0xf6, 
};
static const unsigned char gb_chars_ban3138 [] = {
    0xba, 0xd5, 
};
static const unsigned char gb_chars_ban3139 [] = {
    0xf4, 0xf7, 
};
static const unsigned char gb_chars_ban3140 [] = {
    0xd7, 0xdf, 
};
static const unsigned char gb_chars_ban3141 [] = {
    0xf4, 0xf1, 0xb8, 0xb0, 0xd5, 0xd4, 0xb8, 0xcf, 0xc6, 0xf0, 
};
static const unsigned char gb_chars_ban3142 [] = {
    0xb3, 0xc3, 
};
static const unsigned char gb_chars_ban3143 [] = {
    0xf4, 0xf2, 0xb3, 0xac, 
};
static const unsigned char gb_chars_ban3144 [] = {
    0xd4, 0xbd, 0xc7, 0xf7, 
};
static const unsigned char gb_chars_ban3145 [] = {
    0xf4, 0xf4, 
};
static const unsigned char gb_chars_ban3146 [] = {
    0xf4, 0xf3, 
};
static const unsigned char gb_chars_ban3147 [] = {
    0xcc, 0xcb, 
};
static const unsigned char gb_chars_ban3148 [] = {
    0xc8, 0xa4, 
};
static const unsigned char gb_chars_ban3149 [] = {
    0xf4, 0xf5, 
};
static const unsigned char gb_chars_ban3150 [] = {
    0xd7, 0xe3, 0xc5, 0xbf, 0xf5, 0xc0, 
};
static const unsigned char gb_chars_ban3151 [] = {
    0xf5, 0xbb, 
};
static const unsigned char gb_chars_ban3152 [] = {
    0xf5, 0xc3, 
};
static const unsigned char gb_chars_ban3153 [] = {
    0xf5, 0xc2, 
};
static const unsigned char gb_chars_ban3154 [] = {
    0xd6, 0xba, 0xf5, 0xc1, 
};
static const unsigned char gb_chars_ban3155 [] = {
    0xd4, 0xbe, 0xf5, 0xc4, 
};
static const unsigned char gb_chars_ban3156 [] = {
    0xf5, 0xcc, 
};
static const unsigned char gb_chars_ban3157 [] = {
    0xb0, 0xcf, 0xb5, 0xf8, 
};
static const unsigned char gb_chars_ban3158 [] = {
    0xf5, 0xc9, 0xf5, 0xca, 
};
static const unsigned char gb_chars_ban3159 [] = {
    0xc5, 0xdc, 
};
static const unsigned char gb_chars_ban3160 [] = {
    0xf5, 0xc5, 0xf5, 0xc6, 
};
static const unsigned char gb_chars_ban3161 [] = {
    0xf5, 0xc7, 0xf5, 0xcb, 
};
static const unsigned char gb_chars_ban3162 [] = {
    0xbe, 0xe0, 0xf5, 0xc8, 0xb8, 0xfa, 
};
static const unsigned char gb_chars_ban3163 [] = {
    0xf5, 0xd0, 0xf5, 0xd3, 
};
static const unsigned char gb_chars_ban3164 [] = {
    0xbf, 0xe7, 
};
static const unsigned char gb_chars_ban3165 [] = {
    0xb9, 0xf2, 0xf5, 0xbc, 0xf5, 0xcd, 
};
static const unsigned char gb_chars_ban3166 [] = {
    0xc2, 0xb7, 
};
static const unsigned char gb_chars_ban3167 [] = {
    0xcc, 0xf8, 
};
static const unsigned char gb_chars_ban3168 [] = {
    0xbc, 0xf9, 
};
static const unsigned char gb_chars_ban3169 [] = {
    0xf5, 0xce, 0xf5, 0xcf, 0xf5, 0xd1, 0xb6, 0xe5, 0xf5, 0xd2, 
};
static const unsigned char gb_chars_ban3170 [] = {
    0xf5, 0xd5, 
};
static const unsigned char gb_chars_ban3171 [] = {
    0xf5, 0xbd, 
};
static const unsigned char gb_chars_ban3172 [] = {
    0xf5, 0xd4, 0xd3, 0xbb, 
};
static const unsigned char gb_chars_ban3173 [] = {
    0xb3, 0xec, 
};
static const unsigned char gb_chars_ban3174 [] = {
    0xcc, 0xa4, 
};
static const unsigned char gb_chars_ban3175 [] = {
    0xf5, 0xd6, 
};
static const unsigned char gb_chars_ban3176 [] = {
    0xf5, 0xd7, 0xbe, 0xe1, 0xf5, 0xd8, 
};
static const unsigned char gb_chars_ban3177 [] = {
    0xcc, 0xdf, 0xf5, 0xdb, 
};
static const unsigned char gb_chars_ban3178 [] = {
    0xb2, 0xc8, 0xd7, 0xd9, 
};
static const unsigned char gb_chars_ban3179 [] = {
    0xf5, 0xd9, 
};
static const unsigned char gb_chars_ban3180 [] = {
    0xf5, 0xda, 0xf5, 0xdc, 
};
static const unsigned char gb_chars_ban3181 [] = {
    0xf5, 0xe2, 
};
static const unsigned char gb_chars_ban3182 [] = {
    0xf5, 0xe0, 
};
static const unsigned char gb_chars_ban3183 [] = {
    0xf5, 0xdf, 0xf5, 0xdd, 
};
static const unsigned char gb_chars_ban3184 [] = {
    0xf5, 0xe1, 
};
static const unsigned char gb_chars_ban3185 [] = {
    0xf5, 0xde, 0xf5, 0xe4, 0xf5, 0xe5, 
};
static const unsigned char gb_chars_ban3186 [] = {
    0xcc, 0xe3, 
};
static const unsigned char gb_chars_ban3187 [] = {
    0xe5, 0xbf, 0xb5, 0xb8, 0xf5, 0xe3, 0xf5, 0xe8, 0xcc, 0xa3, 
};
static const unsigned char gb_chars_ban3188 [] = {
    0xf5, 0xe6, 0xf5, 0xe7, 
};
static const unsigned char gb_chars_ban3189 [] = {
    0xf5, 0xbe, 
};
static const unsigned char gb_chars_ban3190 [] = {
    0xb1, 0xc4, 
};
static const unsigned char gb_chars_ban3191 [] = {
    0xf5, 0xbf, 
};
static const unsigned char gb_chars_ban3192 [] = {
    0xb5, 0xc5, 0xb2, 0xe4, 
};
static const unsigned char gb_chars_ban3193 [] = {
    0xf5, 0xec, 0xf5, 0xe9, 
};
static const unsigned char gb_chars_ban3194 [] = {
    0xb6, 0xd7, 
};
static const unsigned char gb_chars_ban3195 [] = {
    0xf5, 0xed, 
};
static const unsigned char gb_chars_ban3196 [] = {
    0xf5, 0xea, 
};
static const unsigned char gb_chars_ban3197 [] = {
    0xf5, 0xeb, 
};
static const unsigned char gb_chars_ban3198 [] = {
    0xb4, 0xda, 
};
static const unsigned char gb_chars_ban3199 [] = {
    0xd4, 0xea, 
};
static const unsigned char gb_chars_ban3200 [] = {
    0xf5, 0xee, 
};
static const unsigned char gb_chars_ban3201 [] = {
    0xb3, 0xf9, 
};
static const unsigned char gb_chars_ban3202 [] = {
    0xf5, 0xef, 0xf5, 0xf1, 
};
static const unsigned char gb_chars_ban3203 [] = {
    0xf5, 0xf0, 
};
static const unsigned char gb_chars_ban3204 [] = {
    0xf5, 0xf2, 
};
static const unsigned char gb_chars_ban3205 [] = {
    0xf5, 0xf3, 
};
static const unsigned char gb_chars_ban3206 [] = {
    0xc9, 0xed, 0xb9, 0xaa, 
};
static const unsigned char gb_chars_ban3207 [] = {
    0xc7, 0xfb, 
};
static const unsigned char gb_chars_ban3208 [] = {
    0xb6, 0xe3, 
};
static const unsigned char gb_chars_ban3209 [] = {
    0xcc, 0xc9, 
};
static const unsigned char gb_chars_ban3210 [] = {
    0xea, 0xa6, 
};
static const unsigned char gb_chars_ban3211 [] = {
    0xb3, 0xb5, 0xd4, 0xfe, 0xb9, 0xec, 0xd0, 0xf9, 
};
static const unsigned char gb_chars_ban3212 [] = {
    0xe9, 0xed, 0xd7, 0xaa, 0xe9, 0xee, 0xc2, 0xd6, 0xc8, 0xed, 
    0xba, 0xe4, 0xe9, 0xef, 0xe9, 0xf0, 0xe9, 0xf1, 0xd6, 0xe1, 
    0xe9, 0xf2, 0xe9, 0xf3, 0xe9, 0xf5, 0xe9, 0xf4, 0xe9, 0xf6, 
    0xe9, 0xf7, 0xc7, 0xe1, 0xe9, 0xf8, 0xd4, 0xd8, 0xe9, 0xf9, 
    0xbd, 0xce, 
};
static const unsigned char gb_chars_ban3213 [] = {
    0xe9, 0xfa, 0xe9, 0xfb, 0xbd, 0xcf, 0xe9, 0xfc, 0xb8, 0xa8, 
    0xc1, 0xbe, 0xe9, 0xfd, 0xb1, 0xb2, 0xbb, 0xd4, 0xb9, 0xf5, 
    0xe9, 0xfe, 
};
static const unsigned char gb_chars_ban3214 [] = {
    0xea, 0xa1, 0xea, 0xa2, 0xea, 0xa3, 0xb7, 0xf8, 0xbc, 0xad, 
};
static const unsigned char gb_chars_ban3215 [] = {
    0xca, 0xe4, 0xe0, 0xce, 0xd4, 0xaf, 0xcf, 0xbd, 0xd5, 0xb7, 
    0xea, 0xa4, 0xd5, 0xde, 0xea, 0xa5, 0xd0, 0xc1, 0xb9, 0xbc, 
};
static const unsigned char gb_chars_ban3216 [] = {
    0xb4, 0xc7, 0xb1, 0xd9, 
};
static const unsigned char gb_chars_ban3217 [] = {
    0xc0, 0xb1, 
};
static const unsigned char gb_chars_ban3218 [] = {
    0xb1, 0xe6, 0xb1, 0xe7, 
};
static const unsigned char gb_chars_ban3219 [] = {
    0xb1, 0xe8, 
};
static const unsigned char gb_chars_ban3220 [] = {
    0xb3, 0xbd, 0xc8, 0xe8, 
};
static const unsigned char gb_chars_ban3221 [] = {
    0xe5, 0xc1, 
};
static const unsigned char gb_chars_ban3222 [] = {
    0xb1, 0xdf, 
};
static const unsigned char gb_chars_ban3223 [] = {
    0xc1, 0xc9, 0xb4, 0xef, 
};
static const unsigned char gb_chars_ban3224 [] = {
    0xc7, 0xa8, 0xd3, 0xd8, 
};
static const unsigned char gb_chars_ban3225 [] = {
    0xc6, 0xf9, 0xd1, 0xb8, 
};
static const unsigned char gb_chars_ban3226 [] = {
    0xb9, 0xfd, 0xc2, 0xf5, 
};
static const unsigned char gb_chars_ban3227 [] = {
    0xd3, 0xad, 
};
static const unsigned char gb_chars_ban3228 [] = {
    0xd4, 0xcb, 0xbd, 0xfc, 
};
static const unsigned char gb_chars_ban3229 [] = {
    0xe5, 0xc2, 0xb7, 0xb5, 0xe5, 0xc3, 
};
static const unsigned char gb_chars_ban3230 [] = {
    0xbb, 0xb9, 0xd5, 0xe2, 
};
static const unsigned char gb_chars_ban3231 [] = {
    0xbd, 0xf8, 0xd4, 0xb6, 0xce, 0xa5, 0xc1, 0xac, 0xb3, 0xd9, 
};
static const unsigned char gb_chars_ban3232 [] = {
    0xcc, 0xf6, 
};
static const unsigned char gb_chars_ban3233 [] = {
    0xe5, 0xc6, 0xe5, 0xc4, 0xe5, 0xc8, 
};
static const unsigned char gb_chars_ban3234 [] = {
    0xe5, 0xca, 0xe5, 0xc7, 0xb5, 0xcf, 0xc6, 0xc8, 
};
static const unsigned char gb_chars_ban3235 [] = {
    0xb5, 0xfc, 0xe5, 0xc5, 
};
static const unsigned char gb_chars_ban3236 [] = {
    0xca, 0xf6, 
};
static const unsigned char gb_chars_ban3237 [] = {
    0xe5, 0xc9, 
};
static const unsigned char gb_chars_ban3238 [] = {
    0xc3, 0xd4, 0xb1, 0xc5, 0xbc, 0xa3, 
};
static const unsigned char gb_chars_ban3239 [] = {
    0xd7, 0xb7, 
};
static const unsigned char gb_chars_ban3240 [] = {
    0xcd, 0xcb, 0xcb, 0xcd, 0xca, 0xca, 0xcc, 0xd3, 0xe5, 0xcc, 
    0xe5, 0xcb, 0xc4, 0xe6, 
};
static const unsigned char gb_chars_ban3241 [] = {
    0xd1, 0xa1, 0xd1, 0xb7, 0xe5, 0xcd, 
};
static const unsigned char gb_chars_ban3242 [] = {
    0xe5, 0xd0, 
};
static const unsigned char gb_chars_ban3243 [] = {
    0xcd, 0xb8, 0xd6, 0xf0, 0xe5, 0xcf, 0xb5, 0xdd, 
};
static const unsigned char gb_chars_ban3244 [] = {
    0xcd, 0xbe, 
};
static const unsigned char gb_chars_ban3245 [] = {
    0xe5, 0xd1, 0xb6, 0xba, 
};
static const unsigned char gb_chars_ban3246 [] = {
    0xcd, 0xa8, 0xb9, 0xe4, 
};
static const unsigned char gb_chars_ban3247 [] = {
    0xca, 0xc5, 0xb3, 0xd1, 0xcb, 0xd9, 0xd4, 0xec, 0xe5, 0xd2, 
    0xb7, 0xea, 
};
static const unsigned char gb_chars_ban3248 [] = {
    0xe5, 0xce, 
};
static const unsigned char gb_chars_ban3249 [] = {
    0xe5, 0xd5, 0xb4, 0xfe, 0xe5, 0xd6, 
};
static const unsigned char gb_chars_ban3250 [] = {
    0xe5, 0xd3, 0xe5, 0xd4, 
};
static const unsigned char gb_chars_ban3251 [] = {
    0xd2, 0xdd, 
};
static const unsigned char gb_chars_ban3252 [] = {
    0xc2, 0xdf, 0xb1, 0xc6, 
};
static const unsigned char gb_chars_ban3253 [] = {
    0xd3, 0xe2, 
};
static const unsigned char gb_chars_ban3254 [] = {
    0xb6, 0xdd, 0xcb, 0xec, 
};
static const unsigned char gb_chars_ban3255 [] = {
    0xe5, 0xd7, 
};
static const unsigned char gb_chars_ban3256 [] = {
    0xd3, 0xf6, 
};
static const unsigned char gb_chars_ban3257 [] = {
    0xb1, 0xe9, 
};
static const unsigned char gb_chars_ban3258 [] = {
    0xb6, 0xf4, 0xe5, 0xda, 0xe5, 0xd8, 0xe5, 0xd9, 0xb5, 0xc0, 
};
static const unsigned char gb_chars_ban3259 [] = {
    0xd2, 0xc5, 0xe5, 0xdc, 
};
static const unsigned char gb_chars_ban3260 [] = {
    0xe5, 0xde, 
};
static const unsigned char gb_chars_ban3261 [] = {
    0xe5, 0xdd, 0xc7, 0xb2, 
};
static const unsigned char gb_chars_ban3262 [] = {
    0xd2, 0xa3, 
};
static const unsigned char gb_chars_ban3263 [] = {
    0xe5, 0xdb, 
};
static const unsigned char gb_chars_ban3264 [] = {
    0xd4, 0xe2, 0xd5, 0xda, 
};
static const unsigned char gb_chars_ban3265 [] = {
    0xe5, 0xe0, 0xd7, 0xf1, 
};
static const unsigned char gb_chars_ban3266 [] = {
    0xe5, 0xe1, 
};
static const unsigned char gb_chars_ban3267 [] = {
    0xb1, 0xdc, 0xd1, 0xfb, 
};
static const unsigned char gb_chars_ban3268 [] = {
    0xe5, 0xe2, 0xe5, 0xe4, 
};
static const unsigned char gb_chars_ban3269 [] = {
    0xe5, 0xe3, 
};
static const unsigned char gb_chars_ban3270 [] = {
    0xe5, 0xe5, 
};
static const unsigned char gb_chars_ban3271 [] = {
    0xd2, 0xd8, 
};
static const unsigned char gb_chars_ban3272 [] = {
    0xb5, 0xcb, 
};
static const unsigned char gb_chars_ban3273 [] = {
    0xe7, 0xdf, 
};
static const unsigned char gb_chars_ban3274 [] = {
    0xda, 0xf5, 
};
static const unsigned char gb_chars_ban3275 [] = {
    0xda, 0xf8, 
};
static const unsigned char gb_chars_ban3276 [] = {
    0xda, 0xf6, 
};
static const unsigned char gb_chars_ban3277 [] = {
    0xda, 0xf7, 
};
static const unsigned char gb_chars_ban3278 [] = {
    0xda, 0xfa, 0xd0, 0xcf, 0xc4, 0xc7, 
};
static const unsigned char gb_chars_ban3279 [] = {
    0xb0, 0xee, 
};
static const unsigned char gb_chars_ban3280 [] = {
    0xd0, 0xb0, 
};
static const unsigned char gb_chars_ban3281 [] = {
    0xda, 0xf9, 
};
static const unsigned char gb_chars_ban3282 [] = {
    0xd3, 0xca, 0xba, 0xaa, 0xdb, 0xa2, 0xc7, 0xf1, 
};
static const unsigned char gb_chars_ban3283 [] = {
    0xda, 0xfc, 0xda, 0xfb, 0xc9, 0xdb, 0xda, 0xfd, 
};
static const unsigned char gb_chars_ban3284 [] = {
    0xdb, 0xa1, 0xd7, 0xde, 0xda, 0xfe, 0xc1, 0xda, 
};
static const unsigned char gb_chars_ban3285 [] = {
    0xdb, 0xa5, 
};
static const unsigned char gb_chars_ban3286 [] = {
    0xd3, 0xf4, 
};
static const unsigned char gb_chars_ban3287 [] = {
    0xdb, 0xa7, 0xdb, 0xa4, 
};
static const unsigned char gb_chars_ban3288 [] = {
    0xdb, 0xa8, 
};
static const unsigned char gb_chars_ban3289 [] = {
    0xbd, 0xbc, 
};
static const unsigned char gb_chars_ban3290 [] = {
    0xc0, 0xc9, 0xdb, 0xa3, 0xdb, 0xa6, 0xd6, 0xa3, 
};
static const unsigned char gb_chars_ban3291 [] = {
    0xdb, 0xa9, 
};
static const unsigned char gb_chars_ban3292 [] = {
    0xdb, 0xad, 
};
static const unsigned char gb_chars_ban3293 [] = {
    0xdb, 0xae, 0xdb, 0xac, 0xba, 0xc2, 
};
static const unsigned char gb_chars_ban3294 [] = {
    0xbf, 0xa4, 0xdb, 0xab, 
};
static const unsigned char gb_chars_ban3295 [] = {
    0xdb, 0xaa, 0xd4, 0xc7, 0xb2, 0xbf, 
};
static const unsigned char gb_chars_ban3296 [] = {
    0xdb, 0xaf, 
};
static const unsigned char gb_chars_ban3297 [] = {
    0xb9, 0xf9, 
};
static const unsigned char gb_chars_ban3298 [] = {
    0xdb, 0xb0, 
};
static const unsigned char gb_chars_ban3299 [] = {
    0xb3, 0xbb, 
};
static const unsigned char gb_chars_ban3300 [] = {
    0xb5, 0xa6, 
};
static const unsigned char gb_chars_ban3301 [] = {
    0xb6, 0xbc, 0xdb, 0xb1, 
};
static const unsigned char gb_chars_ban3302 [] = {
    0xb6, 0xf5, 
};
static const unsigned char gb_chars_ban3303 [] = {
    0xdb, 0xb2, 
};
static const unsigned char gb_chars_ban3304 [] = {
    0xb1, 0xc9, 
};
static const unsigned char gb_chars_ban3305 [] = {
    0xdb, 0xb4, 
};
static const unsigned char gb_chars_ban3306 [] = {
    0xdb, 0xb3, 0xdb, 0xb5, 
};
static const unsigned char gb_chars_ban3307 [] = {
    0xdb, 0xb7, 
};
static const unsigned char gb_chars_ban3308 [] = {
    0xdb, 0xb6, 
};
static const unsigned char gb_chars_ban3309 [] = {
    0xdb, 0xb8, 
};
static const unsigned char gb_chars_ban3310 [] = {
    0xdb, 0xb9, 
};
static const unsigned char gb_chars_ban3311 [] = {
    0xdb, 0xba, 
};
static const unsigned char gb_chars_ban3312 [] = {
    0xd3, 0xcf, 0xf4, 0xfa, 0xc7, 0xf5, 0xd7, 0xc3, 0xc5, 0xe4, 
    0xf4, 0xfc, 0xf4, 0xfd, 0xf4, 0xfb, 
};
static const unsigned char gb_chars_ban3313 [] = {
    0xbe, 0xc6, 
};
static const unsigned char gb_chars_ban3314 [] = {
    0xd0, 0xef, 
};
static const unsigned char gb_chars_ban3315 [] = {
    0xb7, 0xd3, 
};
static const unsigned char gb_chars_ban3316 [] = {
    0xd4, 0xcd, 0xcc, 0xaa, 
};
static const unsigned char gb_chars_ban3317 [] = {
    0xf5, 0xa2, 0xf5, 0xa1, 0xba, 0xa8, 0xf4, 0xfe, 0xcb, 0xd6, 
};
static const unsigned char gb_chars_ban3318 [] = {
    0xf5, 0xa4, 0xc0, 0xd2, 
};
static const unsigned char gb_chars_ban3319 [] = {
    0xb3, 0xea, 
};
static const unsigned char gb_chars_ban3320 [] = {
    0xcd, 0xaa, 0xf5, 0xa5, 0xf5, 0xa3, 0xbd, 0xb4, 0xf5, 0xa8, 
};
static const unsigned char gb_chars_ban3321 [] = {
    0xf5, 0xa9, 0xbd, 0xcd, 0xc3, 0xb8, 0xbf, 0xe1, 0xcb, 0xe1, 
    0xf5, 0xaa, 
};
static const unsigned char gb_chars_ban3322 [] = {
    0xf5, 0xa6, 0xf5, 0xa7, 0xc4, 0xf0, 
};
static const unsigned char gb_chars_ban3323 [] = {
    0xf5, 0xac, 
};
static const unsigned char gb_chars_ban3324 [] = {
    0xb4, 0xbc, 
};
static const unsigned char gb_chars_ban3325 [] = {
    0xd7, 0xed, 
};
static const unsigned char gb_chars_ban3326 [] = {
    0xb4, 0xd7, 0xf5, 0xab, 0xf5, 0xae, 
};
static const unsigned char gb_chars_ban3327 [] = {
    0xf5, 0xad, 0xf5, 0xaf, 0xd0, 0xd1, 
};
static const unsigned char gb_chars_ban3328 [] = {
    0xc3, 0xd1, 0xc8, 0xa9, 
};
static const unsigned char gb_chars_ban3329 [] = {
    0xf5, 0xb0, 0xf5, 0xb1, 
};
static const unsigned char gb_chars_ban3330 [] = {
    0xf5, 0xb2, 
};
static const unsigned char gb_chars_ban3331 [] = {
    0xf5, 0xb3, 0xf5, 0xb4, 0xf5, 0xb5, 
};
static const unsigned char gb_chars_ban3332 [] = {
    0xf5, 0xb7, 0xf5, 0xb6, 
};
static const unsigned char gb_chars_ban3333 [] = {
    0xf5, 0xb8, 
};
static const unsigned char gb_chars_ban3334 [] = {
    0xb2, 0xc9, 
};
static const unsigned char gb_chars_ban3335 [] = {
    0xd3, 0xd4, 0xca, 0xcd, 
};
static const unsigned char gb_chars_ban3336 [] = {
    0xc0, 0xef, 0xd6, 0xd8, 0xd2, 0xb0, 0xc1, 0xbf, 
};
static const unsigned char gb_chars_ban3337 [] = {
    0xbd, 0xf0, 
};
static const unsigned char gb_chars_ban3338 [] = {
    0xb8, 0xaa, 
};
static const unsigned char gb_chars_ban3339 [] = {
    0xbc, 0xf8, 
};
static const unsigned char gb_chars_ban3340 [] = {
    0xf6, 0xc6, 
};
static const unsigned char gb_chars_ban3341 [] = {
    0xf6, 0xc7, 
};
static const unsigned char gb_chars_ban3342 [] = {
    0xf6, 0xc8, 
};
static const unsigned char gb_chars_ban3343 [] = {
    0xf6, 0xc9, 
};
static const unsigned char gb_chars_ban3344 [] = {
    0xf6, 0xca, 
};
static const unsigned char gb_chars_ban3345 [] = {
    0xf6, 0xcc, 
};
static const unsigned char gb_chars_ban3346 [] = {
    0xf6, 0xcb, 
};
static const unsigned char gb_chars_ban3347 [] = {
    0xf7, 0xe9, 
};
static const unsigned char gb_chars_ban3348 [] = {
    0xf6, 0xcd, 
};
static const unsigned char gb_chars_ban3349 [] = {
    0xf6, 0xce, 
};
static const unsigned char gb_chars_ban3350 [] = {
    0xee, 0xc4, 0xee, 0xc5, 0xee, 0xc6, 0xd5, 0xeb, 0xb6, 0xa4, 
    0xee, 0xc8, 0xee, 0xc7, 0xee, 0xc9, 0xee, 0xca, 0xc7, 0xa5, 
    0xee, 0xcb, 0xee, 0xcc, 
};
static const unsigned char gb_chars_ban3351 [] = {
    0xb7, 0xb0, 0xb5, 0xf6, 0xee, 0xcd, 0xee, 0xcf, 
};
static const unsigned char gb_chars_ban3352 [] = {
    0xee, 0xce, 
};
static const unsigned char gb_chars_ban3353 [] = {
    0xb8, 0xc6, 0xee, 0xd0, 0xee, 0xd1, 0xee, 0xd2, 0xb6, 0xdb, 
    0xb3, 0xae, 0xd6, 0xd3, 0xc4, 0xc6, 0xb1, 0xb5, 0xb8, 0xd6, 
    0xee, 0xd3, 0xee, 0xd4, 0xd4, 0xbf, 0xc7, 0xd5, 0xbe, 0xfb, 
    0xce, 0xd9, 0xb9, 0xb3, 0xee, 0xd6, 0xee, 0xd5, 0xee, 0xd8, 
    0xee, 0xd7, 0xc5, 0xa5, 0xee, 0xd9, 0xee, 0xda, 0xc7, 0xae, 
    0xee, 0xdb, 0xc7, 0xaf, 0xee, 0xdc, 0xb2, 0xa7, 0xee, 0xdd, 
    0xee, 0xde, 0xee, 0xdf, 0xee, 0xe0, 0xee, 0xe1, 0xd7, 0xea, 
    0xee, 0xe2, 0xee, 0xe3, 0xbc, 0xd8, 0xee, 0xe4, 0xd3, 0xcb, 
    0xcc, 0xfa, 0xb2, 0xac, 0xc1, 0xe5, 0xee, 0xe5, 0xc7, 0xa6, 
    0xc3, 0xad, 
};
static const unsigned char gb_chars_ban3354 [] = {
    0xee, 0xe6, 0xee, 0xe7, 0xee, 0xe8, 0xee, 0xe9, 0xee, 0xea, 
    0xee, 0xeb, 0xee, 0xec, 
};
static const unsigned char gb_chars_ban3355 [] = {
    0xee, 0xed, 0xee, 0xee, 0xee, 0xef, 
};
static const unsigned char gb_chars_ban3356 [] = {
    0xee, 0xf0, 0xee, 0xf1, 0xee, 0xf2, 0xee, 0xf4, 0xee, 0xf3, 
};
static const unsigned char gb_chars_ban3357 [] = {
    0xee, 0xf5, 0xcd, 0xad, 0xc2, 0xc1, 0xee, 0xf6, 0xee, 0xf7, 
    0xee, 0xf8, 0xd5, 0xa1, 0xee, 0xf9, 0xcf, 0xb3, 0xee, 0xfa, 
    0xee, 0xfb, 
};
static const unsigned char gb_chars_ban3358 [] = {
    0xee, 0xfc, 0xee, 0xfd, 0xef, 0xa1, 0xee, 0xfe, 0xef, 0xa2, 
    0xb8, 0xf5, 0xc3, 0xfa, 0xef, 0xa3, 0xef, 0xa4, 0xbd, 0xc2, 
    0xd2, 0xbf, 0xb2, 0xf9, 0xef, 0xa5, 0xef, 0xa6, 0xef, 0xa7, 
    0xd2, 0xf8, 0xef, 0xa8, 0xd6, 0xfd, 0xef, 0xa9, 0xc6, 0xcc, 
};
static const unsigned char gb_chars_ban3359 [] = {
    0xef, 0xaa, 0xef, 0xab, 0xc1, 0xb4, 0xef, 0xac, 0xcf, 0xfa, 
    0xcb, 0xf8, 0xef, 0xae, 0xef, 0xad, 0xb3, 0xfa, 0xb9, 0xf8, 
    0xef, 0xaf, 0xef, 0xb0, 0xd0, 0xe2, 0xef, 0xb1, 0xef, 0xb2, 
    0xb7, 0xe6, 0xd0, 0xbf, 0xef, 0xb3, 0xef, 0xb4, 0xef, 0xb5, 
    0xc8, 0xf1, 0xcc, 0xe0, 0xef, 0xb6, 0xef, 0xb7, 0xef, 0xb8, 
    0xef, 0xb9, 0xef, 0xba, 0xd5, 0xe0, 0xef, 0xbb, 0xb4, 0xed, 
    0xc3, 0xaa, 0xef, 0xbc, 
};
static const unsigned char gb_chars_ban3360 [] = {
    0xef, 0xbd, 0xef, 0xbe, 0xef, 0xbf, 
};
static const unsigned char gb_chars_ban3361 [] = {
    0xce, 0xfd, 0xef, 0xc0, 0xc2, 0xe0, 0xb4, 0xb8, 0xd7, 0xb6, 
    0xbd, 0xf5, 
};
static const unsigned char gb_chars_ban3362 [] = {
    0xcf, 0xc7, 0xef, 0xc3, 0xef, 0xc1, 0xef, 0xc2, 0xef, 0xc4, 
    0xb6, 0xa7, 0xbc, 0xfc, 0xbe, 0xe2, 0xc3, 0xcc, 0xef, 0xc5, 
    0xef, 0xc6, 
};
static const unsigned char gb_chars_ban3363 [] = {
    0xef, 0xc7, 0xef, 0xcf, 0xef, 0xc8, 0xef, 0xc9, 0xef, 0xca, 
    0xc7, 0xc2, 0xef, 0xf1, 0xb6, 0xcd, 0xef, 0xcb, 
};
static const unsigned char gb_chars_ban3364 [] = {
    0xef, 0xcc, 0xef, 0xcd, 0xb6, 0xc6, 0xc3, 0xbe, 0xef, 0xce, 
};
static const unsigned char gb_chars_ban3365 [] = {
    0xef, 0xd0, 0xef, 0xd1, 0xef, 0xd2, 0xd5, 0xf2, 
};
static const unsigned char gb_chars_ban3366 [] = {
    0xef, 0xd3, 0xc4, 0xf7, 
};
static const unsigned char gb_chars_ban3367 [] = {
    0xef, 0xd4, 0xc4, 0xf8, 0xef, 0xd5, 0xef, 0xd6, 0xb8, 0xe4, 
    0xb0, 0xf7, 0xef, 0xd7, 0xef, 0xd8, 0xef, 0xd9, 
};
static const unsigned char gb_chars_ban3368 [] = {
    0xef, 0xda, 0xef, 0xdb, 0xef, 0xdc, 0xef, 0xdd, 
};
static const unsigned char gb_chars_ban3369 [] = {
    0xef, 0xde, 0xbe, 0xb5, 0xef, 0xe1, 0xef, 0xdf, 0xef, 0xe0, 
};
static const unsigned char gb_chars_ban3370 [] = {
    0xef, 0xe2, 0xef, 0xe3, 0xc1, 0xcd, 0xef, 0xe4, 0xef, 0xe5, 
    0xef, 0xe6, 0xef, 0xe7, 0xef, 0xe8, 0xef, 0xe9, 0xef, 0xea, 
    0xef, 0xeb, 0xef, 0xec, 0xc0, 0xd8, 
};
static const unsigned char gb_chars_ban3371 [] = {
    0xef, 0xed, 0xc1, 0xad, 0xef, 0xee, 0xef, 0xef, 0xef, 0xf0, 
};
static const unsigned char gb_chars_ban3372 [] = {
    0xcf, 0xe2, 
};
static const unsigned char gb_chars_ban3373 [] = {
    0xb3, 0xa4, 
};
static const unsigned char gb_chars_ban3374 [] = {
    0xc3, 0xc5, 0xe3, 0xc5, 0xc9, 0xc1, 0xe3, 0xc6, 
};
static const unsigned char gb_chars_ban3375 [] = {
    0xb1, 0xd5, 0xce, 0xca, 0xb4, 0xb3, 0xc8, 0xf2, 0xe3, 0xc7, 
    0xcf, 0xd0, 0xe3, 0xc8, 0xbc, 0xe4, 0xe3, 0xc9, 0xe3, 0xca, 
    0xc3, 0xc6, 0xd5, 0xa2, 0xc4, 0xd6, 0xb9, 0xeb, 0xce, 0xc5, 
    0xe3, 0xcb, 0xc3, 0xf6, 0xe3, 0xcc, 
};
static const unsigned char gb_chars_ban3376 [] = {
    0xb7, 0xa7, 0xb8, 0xf3, 0xba, 0xd2, 0xe3, 0xcd, 0xe3, 0xce, 
    0xd4, 0xc4, 0xe3, 0xcf, 
};
static const unsigned char gb_chars_ban3377 [] = {
    0xe3, 0xd0, 0xd1, 0xcb, 0xe3, 0xd1, 0xe3, 0xd2, 0xe3, 0xd3, 
    0xe3, 0xd4, 0xd1, 0xd6, 0xe3, 0xd5, 0xb2, 0xfb, 0xc0, 0xbb, 
    0xe3, 0xd6, 
};
static const unsigned char gb_chars_ban3378 [] = {
    0xc0, 0xab, 0xe3, 0xd7, 0xe3, 0xd8, 0xe3, 0xd9, 
};
static const unsigned char gb_chars_ban3379 [] = {
    0xe3, 0xda, 0xe3, 0xdb, 
};
static const unsigned char gb_chars_ban3380 [] = {
    0xb8, 0xb7, 0xda, 0xe2, 
};
static const unsigned char gb_chars_ban3381 [] = {
    0xb6, 0xd3, 
};
static const unsigned char gb_chars_ban3382 [] = {
    0xda, 0xe4, 0xda, 0xe3, 
};
static const unsigned char gb_chars_ban3383 [] = {
    0xda, 0xe6, 
};
static const unsigned char gb_chars_ban3384 [] = {
    0xc8, 0xee, 
};
static const unsigned char gb_chars_ban3385 [] = {
    0xda, 0xe5, 0xb7, 0xc0, 0xd1, 0xf4, 0xd2, 0xf5, 0xd5, 0xf3, 
    0xbd, 0xd7, 
};
static const unsigned char gb_chars_ban3386 [] = {
    0xd7, 0xe8, 0xda, 0xe8, 0xda, 0xe7, 
};
static const unsigned char gb_chars_ban3387 [] = {
    0xb0, 0xa2, 0xcd, 0xd3, 
};
static const unsigned char gb_chars_ban3388 [] = {
    0xda, 0xe9, 
};
static const unsigned char gb_chars_ban3389 [] = {
    0xb8, 0xbd, 0xbc, 0xca, 0xc2, 0xbd, 0xc2, 0xa4, 0xb3, 0xc2, 
    0xda, 0xea, 
};
static const unsigned char gb_chars_ban3390 [] = {
    0xc2, 0xaa, 0xc4, 0xb0, 0xbd, 0xb5, 
};
static const unsigned char gb_chars_ban3391 [] = {
    0xcf, 0xde, 
};
static const unsigned char gb_chars_ban3392 [] = {
    0xda, 0xeb, 0xc9, 0xc2, 
};
static const unsigned char gb_chars_ban3393 [] = {
    0xb1, 0xdd, 
};
static const unsigned char gb_chars_ban3394 [] = {
    0xda, 0xec, 
};
static const unsigned char gb_chars_ban3395 [] = {
    0xb6, 0xb8, 0xd4, 0xba, 
};
static const unsigned char gb_chars_ban3396 [] = {
    0xb3, 0xfd, 
};
static const unsigned char gb_chars_ban3397 [] = {
    0xda, 0xed, 0xd4, 0xc9, 0xcf, 0xd5, 0xc5, 0xe3, 
};
static const unsigned char gb_chars_ban3398 [] = {
    0xda, 0xee, 
};
static const unsigned char gb_chars_ban3399 [] = {
    0xda, 0xef, 
};
static const unsigned char gb_chars_ban3400 [] = {
    0xda, 0xf0, 0xc1, 0xea, 0xcc, 0xd5, 0xcf, 0xdd, 
};
static const unsigned char gb_chars_ban3401 [] = {
    0xd3, 0xe7, 0xc2, 0xa1, 
};
static const unsigned char gb_chars_ban3402 [] = {
    0xda, 0xf1, 
};
static const unsigned char gb_chars_ban3403 [] = {
    0xcb, 0xe5, 
};
static const unsigned char gb_chars_ban3404 [] = {
    0xda, 0xf2, 
};
static const unsigned char gb_chars_ban3405 [] = {
    0xcb, 0xe6, 0xd2, 0xfe, 
};
static const unsigned char gb_chars_ban3406 [] = {
    0xb8, 0xf4, 
};
static const unsigned char gb_chars_ban3407 [] = {
    0xda, 0xf3, 0xb0, 0xaf, 0xcf, 0xb6, 
};
static const unsigned char gb_chars_ban3408 [] = {
    0xd5, 0xcf, 
};
static const unsigned char gb_chars_ban3409 [] = {
    0xcb, 0xed, 
};
static const unsigned char gb_chars_ban3410 [] = {
    0xda, 0xf4, 
};
static const unsigned char gb_chars_ban3411 [] = {
    0xe3, 0xc4, 
};
static const unsigned char gb_chars_ban3412 [] = {
    0xc1, 0xa5, 
};
static const unsigned char gb_chars_ban3413 [] = {
    0xf6, 0xbf, 
};
static const unsigned char gb_chars_ban3414 [] = {
    0xf6, 0xc0, 0xf6, 0xc1, 0xc4, 0xd1, 
};
static const unsigned char gb_chars_ban3415 [] = {
    0xc8, 0xb8, 0xd1, 0xe3, 
};
static const unsigned char gb_chars_ban3416 [] = {
    0xd0, 0xdb, 0xd1, 0xc5, 0xbc, 0xaf, 0xb9, 0xcd, 
};
static const unsigned char gb_chars_ban3417 [] = {
    0xef, 0xf4, 
};
static const unsigned char gb_chars_ban3418 [] = {
    0xb4, 0xc6, 0xd3, 0xba, 0xf6, 0xc2, 0xb3, 0xfb, 
};
static const unsigned char gb_chars_ban3419 [] = {
    0xf6, 0xc3, 
};
static const unsigned char gb_chars_ban3420 [] = {
    0xb5, 0xf1, 
};
static const unsigned char gb_chars_ban3421 [] = {
    0xf6, 0xc5, 
};
static const unsigned char gb_chars_ban3422 [] = {
    0xd3, 0xea, 0xf6, 0xa7, 0xd1, 0xa9, 
};
static const unsigned char gb_chars_ban3423 [] = {
    0xf6, 0xa9, 
};
static const unsigned char gb_chars_ban3424 [] = {
    0xf6, 0xa8, 
};
static const unsigned char gb_chars_ban3425 [] = {
    0xc1, 0xe3, 0xc0, 0xd7, 
};
static const unsigned char gb_chars_ban3426 [] = {
    0xb1, 0xa2, 
};
static const unsigned char gb_chars_ban3427 [] = {
    0xce, 0xed, 
};
static const unsigned char gb_chars_ban3428 [] = {
    0xd0, 0xe8, 0xf6, 0xab, 
};
static const unsigned char gb_chars_ban3429 [] = {
    0xcf, 0xf6, 
};
static const unsigned char gb_chars_ban3430 [] = {
    0xf6, 0xaa, 0xd5, 0xf0, 0xf6, 0xac, 0xc3, 0xb9, 
};
static const unsigned char gb_chars_ban3431 [] = {
    0xbb, 0xf4, 0xf6, 0xae, 0xf6, 0xad, 
};
static const unsigned char gb_chars_ban3432 [] = {
    0xc4, 0xde, 
};
static const unsigned char gb_chars_ban3433 [] = {
    0xc1, 0xd8, 
};
static const unsigned char gb_chars_ban3434 [] = {
    0xcb, 0xaa, 
};
static const unsigned char gb_chars_ban3435 [] = {
    0xcf, 0xbc, 
};
static const unsigned char gb_chars_ban3436 [] = {
    0xf6, 0xaf, 
};
static const unsigned char gb_chars_ban3437 [] = {
    0xf6, 0xb0, 
};
static const unsigned char gb_chars_ban3438 [] = {
    0xf6, 0xb1, 
};
static const unsigned char gb_chars_ban3439 [] = {
    0xc2, 0xb6, 
};
static const unsigned char gb_chars_ban3440 [] = {
    0xb0, 0xd4, 0xc5, 0xf9, 
};
static const unsigned char gb_chars_ban3441 [] = {
    0xf6, 0xb2, 
};
static const unsigned char gb_chars_ban3442 [] = {
    0xc7, 0xe0, 0xf6, 0xa6, 
};
static const unsigned char gb_chars_ban3443 [] = {
    0xbe, 0xb8, 
};
static const unsigned char gb_chars_ban3444 [] = {
    0xbe, 0xb2, 
};
static const unsigned char gb_chars_ban3445 [] = {
    0xb5, 0xe5, 
};
static const unsigned char gb_chars_ban3446 [] = {
    0xb7, 0xc7, 
};
static const unsigned char gb_chars_ban3447 [] = {
    0xbf, 0xbf, 0xc3, 0xd2, 0xc3, 0xe6, 
};
static const unsigned char gb_chars_ban3448 [] = {
    0xd8, 0xcc, 
};
static const unsigned char gb_chars_ban3449 [] = {
    0xb8, 0xef, 
};
static const unsigned char gb_chars_ban3450 [] = {
    0xbd, 0xf9, 0xd1, 0xa5, 
};
static const unsigned char gb_chars_ban3451 [] = {
    0xb0, 0xd0, 
};
static const unsigned char gb_chars_ban3452 [] = {
    0xf7, 0xb0, 
};
static const unsigned char gb_chars_ban3453 [] = {
    0xf7, 0xb1, 
};
static const unsigned char gb_chars_ban3454 [] = {
    0xd0, 0xac, 
};
static const unsigned char gb_chars_ban3455 [] = {
    0xb0, 0xb0, 
};
static const unsigned char gb_chars_ban3456 [] = {
    0xf7, 0xb2, 0xf7, 0xb3, 
};
static const unsigned char gb_chars_ban3457 [] = {
    0xf7, 0xb4, 
};
static const unsigned char gb_chars_ban3458 [] = {
    0xc7, 0xca, 
};
static const unsigned char gb_chars_ban3459 [] = {
    0xbe, 0xcf, 
};
static const unsigned char gb_chars_ban3460 [] = {
    0xf7, 0xb7, 
};
static const unsigned char gb_chars_ban3461 [] = {
    0xf7, 0xb6, 
};
static const unsigned char gb_chars_ban3462 [] = {
    0xb1, 0xde, 
};
static const unsigned char gb_chars_ban3463 [] = {
    0xf7, 0xb5, 
};
static const unsigned char gb_chars_ban3464 [] = {
    0xf7, 0xb8, 
};
static const unsigned char gb_chars_ban3465 [] = {
    0xf7, 0xb9, 
};
static const unsigned char gb_chars_ban3466 [] = {
    0xce, 0xa4, 0xc8, 0xcd, 
};
static const unsigned char gb_chars_ban3467 [] = {
    0xba, 0xab, 0xe8, 0xb8, 0xe8, 0xb9, 0xe8, 0xba, 0xbe, 0xc2, 
};
static const unsigned char gb_chars_ban3468 [] = {
    0xd2, 0xf4, 
};
static const unsigned char gb_chars_ban3469 [] = {
    0xd4, 0xcf, 0xc9, 0xd8, 
};
static const unsigned char gb_chars_ban3470 [] = {
    0xd2, 0xb3, 0xb6, 0xa5, 0xc7, 0xea, 0xf1, 0xfc, 0xcf, 0xee, 
    0xcb, 0xb3, 0xd0, 0xeb, 0xe7, 0xef, 0xcd, 0xe7, 0xb9, 0xcb, 
    0xb6, 0xd9, 0xf1, 0xfd, 0xb0, 0xe4, 0xcb, 0xcc, 0xf1, 0xfe, 
    0xd4, 0xa4, 0xc2, 0xad, 0xc1, 0xec, 0xc6, 0xc4, 0xbe, 0xb1, 
    0xf2, 0xa1, 0xbc, 0xd5, 
};
static const unsigned char gb_chars_ban3471 [] = {
    0xf2, 0xa2, 0xf2, 0xa3, 
};
static const unsigned char gb_chars_ban3472 [] = {
    0xf2, 0xa4, 0xd2, 0xc3, 0xc6, 0xb5, 
};
static const unsigned char gb_chars_ban3473 [] = {
    0xcd, 0xc7, 0xf2, 0xa5, 
};
static const unsigned char gb_chars_ban3474 [] = {
    0xd3, 0xb1, 0xbf, 0xc5, 0xcc, 0xe2, 
};
static const unsigned char gb_chars_ban3475 [] = {
    0xf2, 0xa6, 0xf2, 0xa7, 0xd1, 0xd5, 0xb6, 0xee, 0xf2, 0xa8, 
    0xf2, 0xa9, 0xb5, 0xdf, 0xf2, 0xaa, 0xf2, 0xab, 
};
static const unsigned char gb_chars_ban3476 [] = {
    0xb2, 0xfc, 0xf2, 0xac, 0xf2, 0xad, 0xc8, 0xa7, 
};
static const unsigned char gb_chars_ban3477 [] = {
    0xb7, 0xe7, 
};
static const unsigned char gb_chars_ban3478 [] = {
    0xec, 0xa9, 0xec, 0xaa, 0xec, 0xab, 
};
static const unsigned char gb_chars_ban3479 [] = {
    0xec, 0xac, 
};
static const unsigned char gb_chars_ban3480 [] = {
    0xc6, 0xae, 0xec, 0xad, 0xec, 0xae, 
};
static const unsigned char gb_chars_ban3481 [] = {
    0xb7, 0xc9, 0xca, 0xb3, 
};
static const unsigned char gb_chars_ban3482 [] = {
    0xe2, 0xb8, 0xf7, 0xcf, 
};
static const unsigned char gb_chars_ban3483 [] = {
    0xf7, 0xd0, 
};
static const unsigned char gb_chars_ban3484 [] = {
    0xb2, 0xcd, 
};
static const unsigned char gb_chars_ban3485 [] = {
    0xf7, 0xd1, 
};
static const unsigned char gb_chars_ban3486 [] = {
    0xf7, 0xd3, 0xf7, 0xd2, 
};
static const unsigned char gb_chars_ban3487 [] = {
    0xe2, 0xbb, 
};
static const unsigned char gb_chars_ban3488 [] = {
    0xbc, 0xa2, 
};
static const unsigned char gb_chars_ban3489 [] = {
    0xe2, 0xbc, 0xe2, 0xbd, 0xe2, 0xbe, 0xe2, 0xbf, 0xe2, 0xc0, 
    0xe2, 0xc1, 0xb7, 0xb9, 0xd2, 0xfb, 0xbd, 0xa4, 0xca, 0xce, 
    0xb1, 0xa5, 0xcb, 0xc7, 
};
static const unsigned char gb_chars_ban3490 [] = {
    0xe2, 0xc2, 0xb6, 0xfc, 0xc8, 0xc4, 0xe2, 0xc3, 
};
static const unsigned char gb_chars_ban3491 [] = {
    0xbd, 0xc8, 
};
static const unsigned char gb_chars_ban3492 [] = {
    0xb1, 0xfd, 0xe2, 0xc4, 
};
static const unsigned char gb_chars_ban3493 [] = {
    0xb6, 0xf6, 0xe2, 0xc5, 0xc4, 0xd9, 
};
static const unsigned char gb_chars_ban3494 [] = {
    0xe2, 0xc6, 0xcf, 0xda, 0xb9, 0xdd, 0xe2, 0xc7, 0xc0, 0xa1, 
};
static const unsigned char gb_chars_ban3495 [] = {
    0xe2, 0xc8, 0xb2, 0xf6, 
};
static const unsigned char gb_chars_ban3496 [] = {
    0xe2, 0xc9, 
};
static const unsigned char gb_chars_ban3497 [] = {
    0xc1, 0xf3, 0xe2, 0xca, 0xe2, 0xcb, 0xc2, 0xf8, 0xe2, 0xcc, 
    0xe2, 0xcd, 0xe2, 0xce, 0xca, 0xd7, 0xd8, 0xb8, 0xd9, 0xe5, 
    0xcf, 0xe3, 
};
static const unsigned char gb_chars_ban3498 [] = {
    0xf0, 0xa5, 
};
static const unsigned char gb_chars_ban3499 [] = {
    0xdc, 0xb0, 
};
static const unsigned char gb_chars_ban3500 [] = {
    0xc2, 0xed, 0xd4, 0xa6, 0xcd, 0xd4, 0xd1, 0xb1, 0xb3, 0xdb, 
    0xc7, 0xfd, 
};
static const unsigned char gb_chars_ban3501 [] = {
    0xb2, 0xb5, 0xc2, 0xbf, 0xe6, 0xe0, 0xca, 0xbb, 0xe6, 0xe1, 
    0xe6, 0xe2, 0xbe, 0xd4, 0xe6, 0xe3, 0xd7, 0xa4, 0xcd, 0xd5, 
    0xe6, 0xe5, 0xbc, 0xdd, 0xe6, 0xe4, 0xe6, 0xe6, 0xe6, 0xe7, 
    0xc2, 0xee, 
};
static const unsigned char gb_chars_ban3502 [] = {
    0xbd, 0xbe, 0xe6, 0xe8, 0xc2, 0xe6, 0xba, 0xa7, 0xe6, 0xe9, 
};
static const unsigned char gb_chars_ban3503 [] = {
    0xe6, 0xea, 0xb3, 0xd2, 0xd1, 0xe9, 
};
static const unsigned char gb_chars_ban3504 [] = {
    0xbf, 0xa5, 0xe6, 0xeb, 0xc6, 0xef, 0xe6, 0xec, 0xe6, 0xed, 
};
static const unsigned char gb_chars_ban3505 [] = {
    0xe6, 0xee, 0xc6, 0xad, 0xe6, 0xef, 
};
static const unsigned char gb_chars_ban3506 [] = {
    0xc9, 0xa7, 0xe6, 0xf0, 0xe6, 0xf1, 0xe6, 0xf2, 0xe5, 0xb9, 
    0xe6, 0xf3, 0xe6, 0xf4, 0xc2, 0xe2, 0xe6, 0xf5, 0xe6, 0xf6, 
    0xd6, 0xe8, 0xe6, 0xf7, 
};
static const unsigned char gb_chars_ban3507 [] = {
    0xe6, 0xf8, 0xb9, 0xc7, 
};
static const unsigned char gb_chars_ban3508 [] = {
    0xf7, 0xbb, 0xf7, 0xba, 
};
static const unsigned char gb_chars_ban3509 [] = {
    0xf7, 0xbe, 0xf7, 0xbc, 0xba, 0xa1, 
};
static const unsigned char gb_chars_ban3510 [] = {
    0xf7, 0xbf, 
};
static const unsigned char gb_chars_ban3511 [] = {
    0xf7, 0xc0, 
};
static const unsigned char gb_chars_ban3512 [] = {
    0xf7, 0xc2, 0xf7, 0xc1, 0xf7, 0xc4, 
};
static const unsigned char gb_chars_ban3513 [] = {
    0xf7, 0xc3, 
};
static const unsigned char gb_chars_ban3514 [] = {
    0xf7, 0xc5, 0xf7, 0xc6, 
};
static const unsigned char gb_chars_ban3515 [] = {
    0xf7, 0xc7, 
};
static const unsigned char gb_chars_ban3516 [] = {
    0xcb, 0xe8, 
};
static const unsigned char gb_chars_ban3517 [] = {
    0xb8, 0xdf, 
};
static const unsigned char gb_chars_ban3518 [] = {
    0xf7, 0xd4, 
};
static const unsigned char gb_chars_ban3519 [] = {
    0xf7, 0xd5, 
};
static const unsigned char gb_chars_ban3520 [] = {
    0xf7, 0xd6, 
};
static const unsigned char gb_chars_ban3521 [] = {
    0xf7, 0xd8, 
};
static const unsigned char gb_chars_ban3522 [] = {
    0xf7, 0xda, 
};
static const unsigned char gb_chars_ban3523 [] = {
    0xf7, 0xd7, 
};
static const unsigned char gb_chars_ban3524 [] = {
    0xf7, 0xdb, 
};
static const unsigned char gb_chars_ban3525 [] = {
    0xf7, 0xd9, 
};
static const unsigned char gb_chars_ban3526 [] = {
    0xd7, 0xd7, 
};
static const unsigned char gb_chars_ban3527 [] = {
    0xf7, 0xdc, 
};
static const unsigned char gb_chars_ban3528 [] = {
    0xf7, 0xdd, 
};
static const unsigned char gb_chars_ban3529 [] = {
    0xf7, 0xde, 
};
static const unsigned char gb_chars_ban3530 [] = {
    0xf7, 0xdf, 
};
static const unsigned char gb_chars_ban3531 [] = {
    0xf7, 0xe0, 
};
static const unsigned char gb_chars_ban3532 [] = {
    0xdb, 0xcb, 
};
static const unsigned char gb_chars_ban3533 [] = {
    0xd8, 0xaa, 
};
static const unsigned char gb_chars_ban3534 [] = {
    0xe5, 0xf7, 0xb9, 0xed, 
};
static const unsigned char gb_chars_ban3535 [] = {
    0xbf, 0xfd, 0xbb, 0xea, 0xf7, 0xc9, 0xc6, 0xc7, 0xf7, 0xc8, 
};
static const unsigned char gb_chars_ban3536 [] = {
    0xf7, 0xca, 0xf7, 0xcc, 0xf7, 0xcb, 
};
static const unsigned char gb_chars_ban3537 [] = {
    0xf7, 0xcd, 
};
static const unsigned char gb_chars_ban3538 [] = {
    0xce, 0xba, 
};
static const unsigned char gb_chars_ban3539 [] = {
    0xf7, 0xce, 
};
static const unsigned char gb_chars_ban3540 [] = {
    0xc4, 0xa7, 
};
static const unsigned char gb_chars_ban3541 [] = {
    0xd3, 0xe3, 
};
static const unsigned char gb_chars_ban3542 [] = {
    0xf6, 0xcf, 
};
static const unsigned char gb_chars_ban3543 [] = {
    0xc2, 0xb3, 0xf6, 0xd0, 
};
static const unsigned char gb_chars_ban3544 [] = {
    0xf6, 0xd1, 0xf6, 0xd2, 0xf6, 0xd3, 0xf6, 0xd4, 
};
static const unsigned char gb_chars_ban3545 [] = {
    0xf6, 0xd6, 
};
static const unsigned char gb_chars_ban3546 [] = {
    0xb1, 0xab, 0xf6, 0xd7, 
};
static const unsigned char gb_chars_ban3547 [] = {
    0xf6, 0xd8, 0xf6, 0xd9, 0xf6, 0xda, 
};
static const unsigned char gb_chars_ban3548 [] = {
    0xf6, 0xdb, 0xf6, 0xdc, 
};
static const unsigned char gb_chars_ban3549 [] = {
    0xf6, 0xdd, 0xf6, 0xde, 0xcf, 0xca, 
};
static const unsigned char gb_chars_ban3550 [] = {
    0xf6, 0xdf, 0xf6, 0xe0, 0xf6, 0xe1, 0xf6, 0xe2, 0xf6, 0xe3, 
    0xf6, 0xe4, 0xc0, 0xf0, 0xf6, 0xe5, 0xf6, 0xe6, 0xf6, 0xe7, 
    0xf6, 0xe8, 0xf6, 0xe9, 
};
static const unsigned char gb_chars_ban3551 [] = {
    0xf6, 0xea, 
};
static const unsigned char gb_chars_ban3552 [] = {
    0xf6, 0xeb, 0xf6, 0xec, 
};
static const unsigned char gb_chars_ban3553 [] = {
    0xf6, 0xed, 0xf6, 0xee, 0xf6, 0xef, 0xf6, 0xf0, 0xf6, 0xf1, 
    0xf6, 0xf2, 0xf6, 0xf3, 0xf6, 0xf4, 0xbe, 0xa8, 
};
static const unsigned char gb_chars_ban3554 [] = {
    0xf6, 0xf5, 0xf6, 0xf6, 0xf6, 0xf7, 0xf6, 0xf8, 
};
static const unsigned char gb_chars_ban3555 [] = {
    0xc8, 0xfa, 0xf6, 0xf9, 0xf6, 0xfa, 0xf6, 0xfb, 0xf6, 0xfc, 
};
static const unsigned char gb_chars_ban3556 [] = {
    0xf6, 0xfd, 0xf6, 0xfe, 0xf7, 0xa1, 0xf7, 0xa2, 0xf7, 0xa3, 
    0xf7, 0xa4, 0xf7, 0xa5, 
};
static const unsigned char gb_chars_ban3557 [] = {
    0xf7, 0xa6, 0xf7, 0xa7, 0xf7, 0xa8, 0xb1, 0xee, 0xf7, 0xa9, 
    0xf7, 0xaa, 0xf7, 0xab, 
};
static const unsigned char gb_chars_ban3558 [] = {
    0xf7, 0xac, 0xf7, 0xad, 0xc1, 0xdb, 0xf7, 0xae, 
};
static const unsigned char gb_chars_ban3559 [] = {
    0xf7, 0xaf, 
};
static const unsigned char gb_chars_ban3560 [] = {
    0xc4, 0xf1, 0xf0, 0xaf, 0xbc, 0xa6, 0xf0, 0xb0, 0xc3, 0xf9, 
};
static const unsigned char gb_chars_ban3561 [] = {
    0xc5, 0xb8, 0xd1, 0xbb, 
};
static const unsigned char gb_chars_ban3562 [] = {
    0xf0, 0xb1, 0xf0, 0xb2, 0xf0, 0xb3, 0xf0, 0xb4, 0xf0, 0xb5, 
    0xd1, 0xbc, 
};
static const unsigned char gb_chars_ban3563 [] = {
    0xd1, 0xec, 
};
static const unsigned char gb_chars_ban3564 [] = {
    0xf0, 0xb7, 0xf0, 0xb6, 0xd4, 0xa7, 
};
static const unsigned char gb_chars_ban3565 [] = {
    0xcd, 0xd2, 0xf0, 0xb8, 0xf0, 0xba, 0xf0, 0xb9, 0xf0, 0xbb, 
    0xf0, 0xbc, 
};
static const unsigned char gb_chars_ban3566 [] = {
    0xb8, 0xeb, 0xf0, 0xbd, 0xba, 0xe8, 
};
static const unsigned char gb_chars_ban3567 [] = {
    0xf0, 0xbe, 0xf0, 0xbf, 0xbe, 0xe9, 0xf0, 0xc0, 0xb6, 0xec, 
    0xf0, 0xc1, 0xf0, 0xc2, 0xf0, 0xc3, 0xf0, 0xc4, 0xc8, 0xb5, 
    0xf0, 0xc5, 0xf0, 0xc6, 
};
static const unsigned char gb_chars_ban3568 [] = {
    0xf0, 0xc7, 0xc5, 0xf4, 
};
static const unsigned char gb_chars_ban3569 [] = {
    0xf0, 0xc8, 
};
static const unsigned char gb_chars_ban3570 [] = {
    0xf0, 0xc9, 
};
static const unsigned char gb_chars_ban3571 [] = {
    0xf0, 0xca, 0xf7, 0xbd, 
};
static const unsigned char gb_chars_ban3572 [] = {
    0xf0, 0xcb, 0xf0, 0xcc, 0xf0, 0xcd, 
};
static const unsigned char gb_chars_ban3573 [] = {
    0xf0, 0xce, 
};
static const unsigned char gb_chars_ban3574 [] = {
    0xf0, 0xcf, 0xba, 0xd7, 
};
static const unsigned char gb_chars_ban3575 [] = {
    0xf0, 0xd0, 0xf0, 0xd1, 0xf0, 0xd2, 0xf0, 0xd3, 0xf0, 0xd4, 
    0xf0, 0xd5, 0xf0, 0xd6, 0xf0, 0xd8, 
};
static const unsigned char gb_chars_ban3576 [] = {
    0xd3, 0xa5, 0xf0, 0xd7, 
};
static const unsigned char gb_chars_ban3577 [] = {
    0xf0, 0xd9, 
};
static const unsigned char gb_chars_ban3578 [] = {
    0xf5, 0xba, 0xc2, 0xb9, 
};
static const unsigned char gb_chars_ban3579 [] = {
    0xf7, 0xe4, 
};
static const unsigned char gb_chars_ban3580 [] = {
    0xf7, 0xe5, 0xf7, 0xe6, 
};
static const unsigned char gb_chars_ban3581 [] = {
    0xf7, 0xe7, 
};
static const unsigned char gb_chars_ban3582 [] = {
    0xf7, 0xe8, 0xc2, 0xb4, 
};
static const unsigned char gb_chars_ban3583 [] = {
    0xf7, 0xea, 
};
static const unsigned char gb_chars_ban3584 [] = {
    0xf7, 0xeb, 
};
static const unsigned char gb_chars_ban3585 [] = {
    0xc2, 0xf3, 
};
static const unsigned char gb_chars_ban3586 [] = {
    0xf4, 0xf0, 
};
static const unsigned char gb_chars_ban3587 [] = {
    0xf4, 0xef, 
};
static const unsigned char gb_chars_ban3588 [] = {
    0xc2, 0xe9, 
};
static const unsigned char gb_chars_ban3589 [] = {
    0xf7, 0xe1, 0xf7, 0xe2, 
};
static const unsigned char gb_chars_ban3590 [] = {
    0xbb, 0xc6, 
};
static const unsigned char gb_chars_ban3591 [] = {
    0xd9, 0xe4, 
};
static const unsigned char gb_chars_ban3592 [] = {
    0xca, 0xf2, 0xc0, 0xe8, 0xf0, 0xa4, 
};
static const unsigned char gb_chars_ban3593 [] = {
    0xba, 0xda, 
};
static const unsigned char gb_chars_ban3594 [] = {
    0xc7, 0xad, 
};
static const unsigned char gb_chars_ban3595 [] = {
    0xc4, 0xac, 
};
static const unsigned char gb_chars_ban3596 [] = {
    0xf7, 0xec, 0xf7, 0xed, 0xf7, 0xee, 
};
static const unsigned char gb_chars_ban3597 [] = {
    0xf7, 0xf0, 0xf7, 0xef, 
};
static const unsigned char gb_chars_ban3598 [] = {
    0xf7, 0xf1, 
};
static const unsigned char gb_chars_ban3599 [] = {
    0xf7, 0xf4, 
};
static const unsigned char gb_chars_ban3600 [] = {
    0xf7, 0xf3, 
};
static const unsigned char gb_chars_ban3601 [] = {
    0xf7, 0xf2, 0xf7, 0xf5, 
};
static const unsigned char gb_chars_ban3602 [] = {
    0xf7, 0xf6, 
};
static const unsigned char gb_chars_ban3603 [] = {
    0xed, 0xe9, 
};
static const unsigned char gb_chars_ban3604 [] = {
    0xed, 0xea, 0xed, 0xeb, 
};
static const unsigned char gb_chars_ban3605 [] = {
    0xf6, 0xbc, 
};
static const unsigned char gb_chars_ban3606 [] = {
    0xf6, 0xbd, 
};
static const unsigned char gb_chars_ban3607 [] = {
    0xf6, 0xbe, 0xb6, 0xa6, 
};
static const unsigned char gb_chars_ban3608 [] = {
    0xd8, 0xbe, 
};
static const unsigned char gb_chars_ban3609 [] = {
    0xb9, 0xc4, 
};
static const unsigned char gb_chars_ban3610 [] = {
    0xd8, 0xbb, 
};
static const unsigned char gb_chars_ban3611 [] = {
    0xdc, 0xb1, 
};
static const unsigned char gb_chars_ban3612 [] = {
    0xca, 0xf3, 
};
static const unsigned char gb_chars_ban3613 [] = {
    0xf7, 0xf7, 
};
static const unsigned char gb_chars_ban3614 [] = {
    0xf7, 0xf8, 
};
static const unsigned char gb_chars_ban3615 [] = {
    0xf7, 0xf9, 
};
static const unsigned char gb_chars_ban3616 [] = {
    0xf7, 0xfb, 
};
static const unsigned char gb_chars_ban3617 [] = {
    0xf7, 0xfa, 
};
static const unsigned char gb_chars_ban3618 [] = {
    0xb1, 0xc7, 
};
static const unsigned char gb_chars_ban3619 [] = {
    0xf7, 0xfc, 0xf7, 0xfd, 
};
static const unsigned char gb_chars_ban3620 [] = {
    0xf7, 0xfe, 
};
static const unsigned char gb_chars_ban3621 [] = {
    0xc6, 0xeb, 0xec, 0xb4, 
};
static const unsigned char gb_chars_ban3622 [] = {
    0xb3, 0xdd, 0xf6, 0xb3, 
};
static const unsigned char gb_chars_ban3623 [] = {
    0xf6, 0xb4, 0xc1, 0xe4, 0xf6, 0xb5, 0xf6, 0xb6, 0xf6, 0xb7, 
    0xf6, 0xb8, 0xf6, 0xb9, 0xf6, 0xba, 0xc8, 0xa3, 0xf6, 0xbb, 
};
static const unsigned char gb_chars_ban3624 [] = {
    0xc1, 0xfa, 0xb9, 0xa8, 0xed, 0xe8, 
};
static const unsigned char gb_chars_ban3625 [] = {
    0xb9, 0xea, 0xd9, 0xdf, 
};
static const unsigned char gb_chars_ban3626 [] = {
    0xa3, 0xa1, 0xa3, 0xa2, 0xa3, 0xa3, 0xa1, 0xe7, 0xa3, 0xa5, 
    0xa3, 0xa6, 0xa3, 0xa7, 0xa3, 0xa8, 0xa3, 0xa9, 0xa3, 0xaa, 
    0xa3, 0xab, 0xa3, 0xac, 0xa3, 0xad, 0xa3, 0xae, 0xa3, 0xaf, 
    0xa3, 0xb0, 0xa3, 0xb1, 0xa3, 0xb2, 0xa3, 0xb3, 0xa3, 0xb4, 
    0xa3, 0xb5, 0xa3, 0xb6, 0xa3, 0xb7, 0xa3, 0xb8, 0xa3, 0xb9, 
    0xa3, 0xba, 0xa3, 0xbb, 0xa3, 0xbc, 0xa3, 0xbd, 0xa3, 0xbe, 
    0xa3, 0xbf, 0xa3, 0xc0, 0xa3, 0xc1, 0xa3, 0xc2, 0xa3, 0xc3, 
    0xa3, 0xc4, 0xa3, 0xc5, 0xa3, 0xc6, 0xa3, 0xc7, 0xa3, 0xc8, 
    0xa3, 0xc9, 0xa3, 0xca, 0xa3, 0xcb, 0xa3, 0xcc, 0xa3, 0xcd, 
    0xa3, 0xce, 0xa3, 0xcf, 0xa3, 0xd0, 0xa3, 0xd1, 0xa3, 0xd2, 
    0xa3, 0xd3, 0xa3, 0xd4, 0xa3, 0xd5, 0xa3, 0xd6, 0xa3, 0xd7, 
    0xa3, 0xd8, 0xa3, 0xd9, 0xa3, 0xda, 0xa3, 0xdb, 0xa3, 0xdc, 
    0xa3, 0xdd, 0xa3, 0xde, 0xa3, 0xdf, 0xa3, 0xe0, 0xa3, 0xe1, 
    0xa3, 0xe2, 0xa3, 0xe3, 0xa3, 0xe4, 0xa3, 0xe5, 0xa3, 0xe6, 
    0xa3, 0xe7, 0xa3, 0xe8, 0xa3, 0xe9, 0xa3, 0xea, 0xa3, 0xeb, 
    0xa3, 0xec, 0xa3, 0xed, 0xa3, 0xee, 0xa3, 0xef, 0xa3, 0xf0, 
    0xa3, 0xf1, 0xa3, 0xf2, 0xa3, 0xf3, 0xa3, 0xf4, 0xa3, 0xf5, 
    0xa3, 0xf6, 0xa3, 0xf7, 0xa3, 0xf8, 0xa3, 0xf9, 0xa3, 0xfa, 
    0xa3, 0xfb, 0xa3, 0xfc, 0xa3, 0xfd, 0xa1, 0xab, 
};
static const unsigned char gb_chars_ban3627 [] = {
    0xa1, 0xe9, 0xa1, 0xea, 
};
static const unsigned char gb_chars_ban3628 [] = {
    0xa3, 0xfe, 
};
static const unsigned char gb_chars_ban3629 [] = {
    0xa3, 0xa4, 
};

static struct _uc16_bans {
    unsigned short start, end;
    const unsigned char* gb_chars;
} uc16_bans [] = {
    {0x00A4, 0x00A4, gb_chars_ban0}, 
    {0x00A7, 0x00A8, gb_chars_ban1}, 
    {0x00B0, 0x00B1, gb_chars_ban2}, 
    {0x00D7, 0x00D7, gb_chars_ban3}, 
    {0x00E0, 0x00E1, gb_chars_ban4}, 
    {0x00E8, 0x00EA, gb_chars_ban5}, 
    {0x00EC, 0x00ED, gb_chars_ban6}, 
    {0x00F2, 0x00F3, gb_chars_ban7}, 
    {0x00F7, 0x00F7, gb_chars_ban8}, 
    {0x00F9, 0x00FA, gb_chars_ban9}, 
    {0x00FC, 0x00FC, gb_chars_ban10}, 
    {0x0101, 0x0101, gb_chars_ban11}, 
    {0x0113, 0x0113, gb_chars_ban12}, 
    {0x011B, 0x011B, gb_chars_ban13}, 
    {0x012B, 0x012B, gb_chars_ban14}, 
    {0x014D, 0x014D, gb_chars_ban15}, 
    {0x016B, 0x016B, gb_chars_ban16}, 
    {0x01CE, 0x01CE, gb_chars_ban17}, 
    {0x01D0, 0x01D0, gb_chars_ban18}, 
    {0x01D2, 0x01D2, gb_chars_ban19}, 
    {0x01D4, 0x01D4, gb_chars_ban20}, 
    {0x01D6, 0x01D6, gb_chars_ban21}, 
    {0x01D8, 0x01D8, gb_chars_ban22}, 
    {0x01DA, 0x01DA, gb_chars_ban23}, 
    {0x01DC, 0x01DC, gb_chars_ban24}, 
    {0x02C7, 0x02C7, gb_chars_ban25}, 
    {0x02C9, 0x02C9, gb_chars_ban26}, 
    {0x0391, 0x03A1, gb_chars_ban27}, 
    {0x03A3, 0x03A9, gb_chars_ban28}, 
    {0x03B1, 0x03C1, gb_chars_ban29}, 
    {0x03C3, 0x03C9, gb_chars_ban30}, 
    {0x0401, 0x0401, gb_chars_ban31}, 
    {0x0410, 0x044F, gb_chars_ban32}, 
    {0x0451, 0x0451, gb_chars_ban33}, 
    {0x2015, 0x2015, gb_chars_ban34}, 
    {0x2018, 0x2019, gb_chars_ban35}, 
    {0x201C, 0x201D, gb_chars_ban36}, 
    {0x2026, 0x2026, gb_chars_ban37}, 
    {0x2030, 0x2030, gb_chars_ban38}, 
    {0x2032, 0x2033, gb_chars_ban39}, 
    {0x203B, 0x203B, gb_chars_ban40}, 
    {0x2103, 0x2103, gb_chars_ban41}, 
    {0x2116, 0x2116, gb_chars_ban42}, 
    {0x2160, 0x216B, gb_chars_ban43}, 
    {0x2190, 0x2193, gb_chars_ban44}, 
    {0x2208, 0x2208, gb_chars_ban45}, 
    {0x220F, 0x220F, gb_chars_ban46}, 
    {0x2211, 0x2211, gb_chars_ban47}, 
    {0x221A, 0x221A, gb_chars_ban48}, 
    {0x221D, 0x221E, gb_chars_ban49}, 
    {0x2220, 0x2220, gb_chars_ban50}, 
    {0x2225, 0x2225, gb_chars_ban51}, 
    {0x2227, 0x222B, gb_chars_ban52}, 
    {0x222E, 0x222E, gb_chars_ban53}, 
    {0x2234, 0x2237, gb_chars_ban54}, 
    {0x223D, 0x223D, gb_chars_ban55}, 
    {0x2248, 0x2248, gb_chars_ban56}, 
    {0x224C, 0x224C, gb_chars_ban57}, 
    {0x2260, 0x2261, gb_chars_ban58}, 
    {0x2264, 0x2265, gb_chars_ban59}, 
    {0x226E, 0x226F, gb_chars_ban60}, 
    {0x2299, 0x2299, gb_chars_ban61}, 
    {0x22A5, 0x22A5, gb_chars_ban62}, 
    {0x2312, 0x2312, gb_chars_ban63}, 
    {0x2460, 0x2469, gb_chars_ban64}, 
    {0x2474, 0x249B, gb_chars_ban65}, 
    {0x2500, 0x254B, gb_chars_ban66}, 
    {0x25A0, 0x25A1, gb_chars_ban67}, 
    {0x25B2, 0x25B3, gb_chars_ban68}, 
    {0x25C6, 0x25C7, gb_chars_ban69}, 
    {0x25CB, 0x25CB, gb_chars_ban70}, 
    {0x25CE, 0x25CF, gb_chars_ban71}, 
    {0x2605, 0x2606, gb_chars_ban72}, 
    {0x2640, 0x2640, gb_chars_ban73}, 
    {0x2642, 0x2642, gb_chars_ban74}, 
    {0x3000, 0x3003, gb_chars_ban75}, 
    {0x3005, 0x3005, gb_chars_ban76}, 
    {0x3008, 0x3011, gb_chars_ban77}, 
    {0x3013, 0x3017, gb_chars_ban78}, 
    {0x3041, 0x3093, gb_chars_ban79}, 
    {0x30A1, 0x30F6, gb_chars_ban80}, 
    {0x30FB, 0x30FB, gb_chars_ban81}, 
    {0x3105, 0x3129, gb_chars_ban82}, 
    {0x3220, 0x3229, gb_chars_ban83}, 
    {0x4E00, 0x4E01, gb_chars_ban84}, 
    {0x4E03, 0x4E03, gb_chars_ban85}, 
    {0x4E07, 0x4E0E, gb_chars_ban86}, 
    {0x4E10, 0x4E11, gb_chars_ban87}, 
    {0x4E13, 0x4E16, gb_chars_ban88}, 
    {0x4E18, 0x4E1E, gb_chars_ban89}, 
    {0x4E22, 0x4E22, gb_chars_ban90}, 
    {0x4E24, 0x4E25, gb_chars_ban91}, 
    {0x4E27, 0x4E28, gb_chars_ban92}, 
    {0x4E2A, 0x4E2D, gb_chars_ban93}, 
    {0x4E30, 0x4E30, gb_chars_ban94}, 
    {0x4E32, 0x4E32, gb_chars_ban95}, 
    {0x4E34, 0x4E34, gb_chars_ban96}, 
    {0x4E36, 0x4E36, gb_chars_ban97}, 
    {0x4E38, 0x4E3B, gb_chars_ban98}, 
    {0x4E3D, 0x4E3F, gb_chars_ban99}, 
    {0x4E43, 0x4E43, gb_chars_ban100}, 
    {0x4E45, 0x4E45, gb_chars_ban101}, 
    {0x4E47, 0x4E49, gb_chars_ban102}, 
    {0x4E4B, 0x4E50, gb_chars_ban103}, 
    {0x4E52, 0x4E54, gb_chars_ban104}, 
    {0x4E56, 0x4E56, gb_chars_ban105}, 
    {0x4E58, 0x4E59, gb_chars_ban106}, 
    {0x4E5C, 0x4E61, gb_chars_ban107}, 
    {0x4E66, 0x4E66, gb_chars_ban108}, 
    {0x4E69, 0x4E69, gb_chars_ban109}, 
    {0x4E70, 0x4E71, gb_chars_ban110}, 
    {0x4E73, 0x4E73, gb_chars_ban111}, 
    {0x4E7E, 0x4E7E, gb_chars_ban112}, 
    {0x4E86, 0x4E86, gb_chars_ban113}, 
    {0x4E88, 0x4E89, gb_chars_ban114}, 
    {0x4E8B, 0x4E8F, gb_chars_ban115}, 
    {0x4E91, 0x4E95, gb_chars_ban116}, 
    {0x4E98, 0x4E98, gb_chars_ban117}, 
    {0x4E9A, 0x4E9B, gb_chars_ban118}, 
    {0x4E9F, 0x4EA2, gb_chars_ban119}, 
    {0x4EA4, 0x4EA9, gb_chars_ban120}, 
    {0x4EAB, 0x4EAE, gb_chars_ban121}, 
    {0x4EB2, 0x4EB3, gb_chars_ban122}, 
    {0x4EB5, 0x4EB5, gb_chars_ban123}, 
    {0x4EBA, 0x4EBB, gb_chars_ban124}, 
    {0x4EBF, 0x4EC7, gb_chars_ban125}, 
    {0x4EC9, 0x4ECB, gb_chars_ban126}, 
    {0x4ECD, 0x4ECE, gb_chars_ban127}, 
    {0x4ED1, 0x4ED1, gb_chars_ban128}, 
    {0x4ED3, 0x4ED9, gb_chars_ban129}, 
    {0x4EDD, 0x4EDF, gb_chars_ban130}, 
    {0x4EE1, 0x4EE1, gb_chars_ban131}, 
    {0x4EE3, 0x4EE5, gb_chars_ban132}, 
    {0x4EE8, 0x4EE8, gb_chars_ban133}, 
    {0x4EEA, 0x4EEC, gb_chars_ban134}, 
    {0x4EF0, 0x4EF0, gb_chars_ban135}, 
    {0x4EF2, 0x4EF3, gb_chars_ban136}, 
    {0x4EF5, 0x4EF7, gb_chars_ban137}, 
    {0x4EFB, 0x4EFB, gb_chars_ban138}, 
    {0x4EFD, 0x4EFD, gb_chars_ban139}, 
    {0x4EFF, 0x4EFF, gb_chars_ban140}, 
    {0x4F01, 0x4F01, gb_chars_ban141}, 
    {0x4F09, 0x4F0A, gb_chars_ban142}, 
    {0x4F0D, 0x4F11, gb_chars_ban143}, 
    {0x4F17, 0x4F1B, gb_chars_ban144}, 
    {0x4F1E, 0x4F20, gb_chars_ban145}, 
    {0x4F22, 0x4F22, gb_chars_ban146}, 
    {0x4F24, 0x4F27, gb_chars_ban147}, 
    {0x4F2A, 0x4F2B, gb_chars_ban148}, 
    {0x4F2F, 0x4F30, gb_chars_ban149}, 
    {0x4F32, 0x4F32, gb_chars_ban150}, 
    {0x4F34, 0x4F34, gb_chars_ban151}, 
    {0x4F36, 0x4F36, gb_chars_ban152}, 
    {0x4F38, 0x4F38, gb_chars_ban153}, 
    {0x4F3A, 0x4F3A, gb_chars_ban154}, 
    {0x4F3C, 0x4F3D, gb_chars_ban155}, 
    {0x4F43, 0x4F43, gb_chars_ban156}, 
    {0x4F46, 0x4F46, gb_chars_ban157}, 
    {0x4F4D, 0x4F51, gb_chars_ban158}, 
    {0x4F53, 0x4F53, gb_chars_ban159}, 
    {0x4F55, 0x4F55, gb_chars_ban160}, 
    {0x4F57, 0x4F60, gb_chars_ban161}, 
    {0x4F63, 0x4F65, gb_chars_ban162}, 
    {0x4F67, 0x4F67, gb_chars_ban163}, 
    {0x4F69, 0x4F69, gb_chars_ban164}, 
    {0x4F6C, 0x4F6C, gb_chars_ban165}, 
    {0x4F6F, 0x4F70, gb_chars_ban166}, 
    {0x4F73, 0x4F74, gb_chars_ban167}, 
    {0x4F76, 0x4F76, gb_chars_ban168}, 
    {0x4F7B, 0x4F7C, gb_chars_ban169}, 
    {0x4F7E, 0x4F7F, gb_chars_ban170}, 
    {0x4F83, 0x4F84, gb_chars_ban171}, 
    {0x4F88, 0x4F89, gb_chars_ban172}, 
    {0x4F8B, 0x4F8B, gb_chars_ban173}, 
    {0x4F8D, 0x4F8D, gb_chars_ban174}, 
    {0x4F8F, 0x4F8F, gb_chars_ban175}, 
    {0x4F91, 0x4F91, gb_chars_ban176}, 
    {0x4F94, 0x4F94, gb_chars_ban177}, 
    {0x4F97, 0x4F97, gb_chars_ban178}, 
    {0x4F9B, 0x4F9B, gb_chars_ban179}, 
    {0x4F9D, 0x4F9D, gb_chars_ban180}, 
    {0x4FA0, 0x4FA0, gb_chars_ban181}, 
    {0x4FA3, 0x4FA3, gb_chars_ban182}, 
    {0x4FA5, 0x4FAA, gb_chars_ban183}, 
    {0x4FAC, 0x4FAC, gb_chars_ban184}, 
    {0x4FAE, 0x4FAF, gb_chars_ban185}, 
    {0x4FB5, 0x4FB5, gb_chars_ban186}, 
    {0x4FBF, 0x4FBF, gb_chars_ban187}, 
    {0x4FC3, 0x4FC5, gb_chars_ban188}, 
    {0x4FCA, 0x4FCA, gb_chars_ban189}, 
    {0x4FCE, 0x4FD1, gb_chars_ban190}, 
    {0x4FD7, 0x4FD8, gb_chars_ban191}, 
    {0x4FDA, 0x4FDA, gb_chars_ban192}, 
    {0x4FDC, 0x4FDF, gb_chars_ban193}, 
    {0x4FE1, 0x4FE1, gb_chars_ban194}, 
    {0x4FE3, 0x4FE3, gb_chars_ban195}, 
    {0x4FE6, 0x4FE6, gb_chars_ban196}, 
    {0x4FE8, 0x4FEA, gb_chars_ban197}, 
    {0x4FED, 0x4FEF, gb_chars_ban198}, 
    {0x4FF1, 0x4FF1, gb_chars_ban199}, 
    {0x4FF3, 0x4FF3, gb_chars_ban200}, 
    {0x4FF8, 0x4FF8, gb_chars_ban201}, 
    {0x4FFA, 0x4FFA, gb_chars_ban202}, 
    {0x4FFE, 0x4FFE, gb_chars_ban203}, 
    {0x500C, 0x500D, gb_chars_ban204}, 
    {0x500F, 0x500F, gb_chars_ban205}, 
    {0x5012, 0x5012, gb_chars_ban206}, 
    {0x5014, 0x5014, gb_chars_ban207}, 
    {0x5018, 0x501A, gb_chars_ban208}, 
    {0x501C, 0x501C, gb_chars_ban209}, 
    {0x501F, 0x501F, gb_chars_ban210}, 
    {0x5021, 0x5021, gb_chars_ban211}, 
    {0x5025, 0x5026, gb_chars_ban212}, 
    {0x5028, 0x502A, gb_chars_ban213}, 
    {0x502C, 0x502E, gb_chars_ban214}, 
    {0x503A, 0x503A, gb_chars_ban215}, 
    {0x503C, 0x503C, gb_chars_ban216}, 
    {0x503E, 0x503E, gb_chars_ban217}, 
    {0x5043, 0x5043, gb_chars_ban218}, 
    {0x5047, 0x5048, gb_chars_ban219}, 
    {0x504C, 0x504C, gb_chars_ban220}, 
    {0x504E, 0x504F, gb_chars_ban221}, 
    {0x5055, 0x5055, gb_chars_ban222}, 
    {0x505A, 0x505A, gb_chars_ban223}, 
    {0x505C, 0x505C, gb_chars_ban224}, 
    {0x5065, 0x5065, gb_chars_ban225}, 
    {0x506C, 0x506C, gb_chars_ban226}, 
    {0x5076, 0x5077, gb_chars_ban227}, 
    {0x507B, 0x507B, gb_chars_ban228}, 
    {0x507E, 0x5080, gb_chars_ban229}, 
    {0x5085, 0x5085, gb_chars_ban230}, 
    {0x5088, 0x5088, gb_chars_ban231}, 
    {0x508D, 0x508D, gb_chars_ban232}, 
    {0x50A3, 0x50A3, gb_chars_ban233}, 
    {0x50A5, 0x50A5, gb_chars_ban234}, 
    {0x50A7, 0x50A9, gb_chars_ban235}, 
    {0x50AC, 0x50AC, gb_chars_ban236}, 
    {0x50B2, 0x50B2, gb_chars_ban237}, 
    {0x50BA, 0x50BB, gb_chars_ban238}, 
    {0x50CF, 0x50CF, gb_chars_ban239}, 
    {0x50D6, 0x50D6, gb_chars_ban240}, 
    {0x50DA, 0x50DA, gb_chars_ban241}, 
    {0x50E6, 0x50E7, gb_chars_ban242}, 
    {0x50EC, 0x50EE, gb_chars_ban243}, 
    {0x50F3, 0x50F3, gb_chars_ban244}, 
    {0x50F5, 0x50F5, gb_chars_ban245}, 
    {0x50FB, 0x50FB, gb_chars_ban246}, 
    {0x5106, 0x5107, gb_chars_ban247}, 
    {0x510B, 0x510B, gb_chars_ban248}, 
    {0x5112, 0x5112, gb_chars_ban249}, 
    {0x5121, 0x5121, gb_chars_ban250}, 
    {0x513F, 0x5141, gb_chars_ban251}, 
    {0x5143, 0x5146, gb_chars_ban252}, 
    {0x5148, 0x5149, gb_chars_ban253}, 
    {0x514B, 0x514B, gb_chars_ban254}, 
    {0x514D, 0x514D, gb_chars_ban255}, 
    {0x5151, 0x5151, gb_chars_ban256}, 
    {0x5154, 0x5156, gb_chars_ban257}, 
    {0x515A, 0x515A, gb_chars_ban258}, 
    {0x515C, 0x515C, gb_chars_ban259}, 
    {0x5162, 0x5162, gb_chars_ban260}, 
    {0x5165, 0x5165, gb_chars_ban261}, 
    {0x5168, 0x5168, gb_chars_ban262}, 
    {0x516B, 0x516E, gb_chars_ban263}, 
    {0x5170, 0x5171, gb_chars_ban264}, 
    {0x5173, 0x5179, gb_chars_ban265}, 
    {0x517B, 0x517D, gb_chars_ban266}, 
    {0x5180, 0x5182, gb_chars_ban267}, 
    {0x5185, 0x5185, gb_chars_ban268}, 
    {0x5188, 0x5189, gb_chars_ban269}, 
    {0x518C, 0x518D, gb_chars_ban270}, 
    {0x5192, 0x5192, gb_chars_ban271}, 
    {0x5195, 0x5197, gb_chars_ban272}, 
    {0x5199, 0x5199, gb_chars_ban273}, 
    {0x519B, 0x519C, gb_chars_ban274}, 
    {0x51A0, 0x51A0, gb_chars_ban275}, 
    {0x51A2, 0x51A2, gb_chars_ban276}, 
    {0x51A4, 0x51A5, gb_chars_ban277}, 
    {0x51AB, 0x51AC, gb_chars_ban278}, 
    {0x51AF, 0x51B3, gb_chars_ban279}, 
    {0x51B5, 0x51B7, gb_chars_ban280}, 
    {0x51BB, 0x51BD, gb_chars_ban281}, 
    {0x51C0, 0x51C0, gb_chars_ban282}, 
    {0x51C4, 0x51C4, gb_chars_ban283}, 
    {0x51C6, 0x51C7, gb_chars_ban284}, 
    {0x51C9, 0x51C9, gb_chars_ban285}, 
    {0x51CB, 0x51CC, gb_chars_ban286}, 
    {0x51CF, 0x51CF, gb_chars_ban287}, 
    {0x51D1, 0x51D1, gb_chars_ban288}, 
    {0x51DB, 0x51DB, gb_chars_ban289}, 
    {0x51DD, 0x51DD, gb_chars_ban290}, 
    {0x51E0, 0x51E1, gb_chars_ban291}, 
    {0x51E4, 0x51E4, gb_chars_ban292}, 
    {0x51EB, 0x51EB, gb_chars_ban293}, 
    {0x51ED, 0x51ED, gb_chars_ban294}, 
    {0x51EF, 0x51F0, gb_chars_ban295}, 
    {0x51F3, 0x51F3, gb_chars_ban296}, 
    {0x51F5, 0x51F6, gb_chars_ban297}, 
    {0x51F8, 0x51FD, gb_chars_ban298}, 
    {0x51FF, 0x5203, gb_chars_ban299}, 
    {0x5206, 0x5208, gb_chars_ban300}, 
    {0x520A, 0x520A, gb_chars_ban301}, 
    {0x520D, 0x520E, gb_chars_ban302}, 
    {0x5211, 0x5212, gb_chars_ban303}, 
    {0x5216, 0x521B, gb_chars_ban304}, 
    {0x521D, 0x521D, gb_chars_ban305}, 
    {0x5220, 0x5220, gb_chars_ban306}, 
    {0x5224, 0x5224, gb_chars_ban307}, 
    {0x5228, 0x5229, gb_chars_ban308}, 
    {0x522B, 0x522B, gb_chars_ban309}, 
    {0x522D, 0x522E, gb_chars_ban310}, 
    {0x5230, 0x5230, gb_chars_ban311}, 
    {0x5233, 0x5233, gb_chars_ban312}, 
    {0x5236, 0x523B, gb_chars_ban313}, 
    {0x523D, 0x523D, gb_chars_ban314}, 
    {0x523F, 0x5243, gb_chars_ban315}, 
    {0x524A, 0x524A, gb_chars_ban316}, 
    {0x524C, 0x524D, gb_chars_ban317}, 
    {0x5250, 0x5251, gb_chars_ban318}, 
    {0x5254, 0x5254, gb_chars_ban319}, 
    {0x5256, 0x5256, gb_chars_ban320}, 
    {0x525C, 0x525C, gb_chars_ban321}, 
    {0x525E, 0x525E, gb_chars_ban322}, 
    {0x5261, 0x5261, gb_chars_ban323}, 
    {0x5265, 0x5265, gb_chars_ban324}, 
    {0x5267, 0x5267, gb_chars_ban325}, 
    {0x5269, 0x526A, gb_chars_ban326}, 
    {0x526F, 0x526F, gb_chars_ban327}, 
    {0x5272, 0x5272, gb_chars_ban328}, 
    {0x527D, 0x527D, gb_chars_ban329}, 
    {0x527F, 0x527F, gb_chars_ban330}, 
    {0x5281, 0x5282, gb_chars_ban331}, 
    {0x5288, 0x5288, gb_chars_ban332}, 
    {0x5290, 0x5290, gb_chars_ban333}, 
    {0x5293, 0x5293, gb_chars_ban334}, 
    {0x529B, 0x529B, gb_chars_ban335}, 
    {0x529D, 0x52A3, gb_chars_ban336}, 
    {0x52A8, 0x52AD, gb_chars_ban337}, 
    {0x52B1, 0x52B3, gb_chars_ban338}, 
    {0x52BE, 0x52BF, gb_chars_ban339}, 
    {0x52C3, 0x52C3, gb_chars_ban340}, 
    {0x52C7, 0x52C7, gb_chars_ban341}, 
    {0x52C9, 0x52C9, gb_chars_ban342}, 
    {0x52CB, 0x52CB, gb_chars_ban343}, 
    {0x52D0, 0x52D0, gb_chars_ban344}, 
    {0x52D2, 0x52D2, gb_chars_ban345}, 
    {0x52D6, 0x52D6, gb_chars_ban346}, 
    {0x52D8, 0x52D8, gb_chars_ban347}, 
    {0x52DF, 0x52DF, gb_chars_ban348}, 
    {0x52E4, 0x52E4, gb_chars_ban349}, 
    {0x52F0, 0x52F0, gb_chars_ban350}, 
    {0x52F9, 0x52FA, gb_chars_ban351}, 
    {0x52FE, 0x5300, gb_chars_ban352}, 
    {0x5305, 0x5306, gb_chars_ban353}, 
    {0x5308, 0x5308, gb_chars_ban354}, 
    {0x530D, 0x530D, gb_chars_ban355}, 
    {0x530F, 0x5310, gb_chars_ban356}, 
    {0x5315, 0x5317, gb_chars_ban357}, 
    {0x5319, 0x531A, gb_chars_ban358}, 
    {0x531D, 0x531D, gb_chars_ban359}, 
    {0x5320, 0x5321, gb_chars_ban360}, 
    {0x5323, 0x5323, gb_chars_ban361}, 
    {0x5326, 0x5326, gb_chars_ban362}, 
    {0x532A, 0x532A, gb_chars_ban363}, 
    {0x532E, 0x532E, gb_chars_ban364}, 
    {0x5339, 0x533B, gb_chars_ban365}, 
    {0x533E, 0x533F, gb_chars_ban366}, 
    {0x5341, 0x5341, gb_chars_ban367}, 
    {0x5343, 0x5343, gb_chars_ban368}, 
    {0x5345, 0x5345, gb_chars_ban369}, 
    {0x5347, 0x534A, gb_chars_ban370}, 
    {0x534E, 0x534F, gb_chars_ban371}, 
    {0x5351, 0x5353, gb_chars_ban372}, 
    {0x5355, 0x5357, gb_chars_ban373}, 
    {0x535A, 0x535A, gb_chars_ban374}, 
    {0x535C, 0x535C, gb_chars_ban375}, 
    {0x535E, 0x5364, gb_chars_ban376}, 
    {0x5366, 0x5367, gb_chars_ban377}, 
    {0x5369, 0x5369, gb_chars_ban378}, 
    {0x536B, 0x536B, gb_chars_ban379}, 
    {0x536E, 0x5371, gb_chars_ban380}, 
    {0x5373, 0x5375, gb_chars_ban381}, 
    {0x5377, 0x5378, gb_chars_ban382}, 
    {0x537A, 0x537A, gb_chars_ban383}, 
    {0x537F, 0x537F, gb_chars_ban384}, 
    {0x5382, 0x5382, gb_chars_ban385}, 
    {0x5384, 0x5386, gb_chars_ban386}, 
    {0x5389, 0x5389, gb_chars_ban387}, 
    {0x538B, 0x538D, gb_chars_ban388}, 
    {0x5395, 0x5395, gb_chars_ban389}, 
    {0x5398, 0x5398, gb_chars_ban390}, 
    {0x539A, 0x539A, gb_chars_ban391}, 
    {0x539D, 0x539D, gb_chars_ban392}, 
    {0x539F, 0x539F, gb_chars_ban393}, 
    {0x53A2, 0x53A3, gb_chars_ban394}, 
    {0x53A5, 0x53A6, gb_chars_ban395}, 
    {0x53A8, 0x53A9, gb_chars_ban396}, 
    {0x53AE, 0x53AE, gb_chars_ban397}, 
    {0x53B6, 0x53B6, gb_chars_ban398}, 
    {0x53BB, 0x53BB, gb_chars_ban399}, 
    {0x53BF, 0x53BF, gb_chars_ban400}, 
    {0x53C1, 0x53C2, gb_chars_ban401}, 
    {0x53C8, 0x53CD, gb_chars_ban402}, 
    {0x53D1, 0x53D1, gb_chars_ban403}, 
    {0x53D4, 0x53D4, gb_chars_ban404}, 
    {0x53D6, 0x53D9, gb_chars_ban405}, 
    {0x53DB, 0x53DB, gb_chars_ban406}, 
    {0x53DF, 0x53E0, gb_chars_ban407}, 
    {0x53E3, 0x53E6, gb_chars_ban408}, 
    {0x53E8, 0x53F3, gb_chars_ban409}, 
    {0x53F5, 0x53F9, gb_chars_ban410}, 
    {0x53FB, 0x53FD, gb_chars_ban411}, 
    {0x5401, 0x5401, gb_chars_ban412}, 
    {0x5403, 0x5404, gb_chars_ban413}, 
    {0x5406, 0x5406, gb_chars_ban414}, 
    {0x5408, 0x540A, gb_chars_ban415}, 
    {0x540C, 0x5413, gb_chars_ban416}, 
    {0x5415, 0x5417, gb_chars_ban417}, 
    {0x541B, 0x541B, gb_chars_ban418}, 
    {0x541D, 0x5421, gb_chars_ban419}, 
    {0x5423, 0x5423, gb_chars_ban420}, 
    {0x5426, 0x5429, gb_chars_ban421}, 
    {0x542B, 0x542F, gb_chars_ban422}, 
    {0x5431, 0x5432, gb_chars_ban423}, 
    {0x5434, 0x5435, gb_chars_ban424}, 
    {0x5438, 0x5439, gb_chars_ban425}, 
    {0x543B, 0x543C, gb_chars_ban426}, 
    {0x543E, 0x543E, gb_chars_ban427}, 
    {0x5440, 0x5440, gb_chars_ban428}, 
    {0x5443, 0x5443, gb_chars_ban429}, 
    {0x5446, 0x5446, gb_chars_ban430}, 
    {0x5448, 0x5448, gb_chars_ban431}, 
    {0x544A, 0x544B, gb_chars_ban432}, 
    {0x5450, 0x5450, gb_chars_ban433}, 
    {0x5452, 0x5459, gb_chars_ban434}, 
    {0x545B, 0x545C, gb_chars_ban435}, 
    {0x5462, 0x5462, gb_chars_ban436}, 
    {0x5464, 0x5464, gb_chars_ban437}, 
    {0x5466, 0x5466, gb_chars_ban438}, 
    {0x5468, 0x5468, gb_chars_ban439}, 
    {0x5471, 0x5473, gb_chars_ban440}, 
    {0x5475, 0x5478, gb_chars_ban441}, 
    {0x547B, 0x547D, gb_chars_ban442}, 
    {0x5480, 0x5480, gb_chars_ban443}, 
    {0x5482, 0x5482, gb_chars_ban444}, 
    {0x5484, 0x5484, gb_chars_ban445}, 
    {0x5486, 0x5486, gb_chars_ban446}, 
    {0x548B, 0x548C, gb_chars_ban447}, 
    {0x548E, 0x5490, gb_chars_ban448}, 
    {0x5492, 0x5492, gb_chars_ban449}, 
    {0x5494, 0x5496, gb_chars_ban450}, 
    {0x5499, 0x549B, gb_chars_ban451}, 
    {0x549D, 0x549D, gb_chars_ban452}, 
    {0x54A3, 0x54A4, gb_chars_ban453}, 
    {0x54A6, 0x54AD, gb_chars_ban454}, 
    {0x54AF, 0x54AF, gb_chars_ban455}, 
    {0x54B1, 0x54B1, gb_chars_ban456}, 
    {0x54B3, 0x54B4, gb_chars_ban457}, 
    {0x54B8, 0x54B8, gb_chars_ban458}, 
    {0x54BB, 0x54BB, gb_chars_ban459}, 
    {0x54BD, 0x54BD, gb_chars_ban460}, 
    {0x54BF, 0x54C2, gb_chars_ban461}, 
    {0x54C4, 0x54C4, gb_chars_ban462}, 
    {0x54C6, 0x54C9, gb_chars_ban463}, 
    {0x54CC, 0x54D5, gb_chars_ban464}, 
    {0x54D7, 0x54D7, gb_chars_ban465}, 
    {0x54D9, 0x54DA, gb_chars_ban466}, 
    {0x54DC, 0x54DF, gb_chars_ban467}, 
    {0x54E5, 0x54EA, gb_chars_ban468}, 
    {0x54ED, 0x54EE, gb_chars_ban469}, 
    {0x54F2, 0x54F3, gb_chars_ban470}, 
    {0x54FA, 0x54FA, gb_chars_ban471}, 
    {0x54FC, 0x54FD, gb_chars_ban472}, 
    {0x54FF, 0x54FF, gb_chars_ban473}, 
    {0x5501, 0x5501, gb_chars_ban474}, 
    {0x5506, 0x5507, gb_chars_ban475}, 
    {0x5509, 0x5509, gb_chars_ban476}, 
    {0x550F, 0x5511, gb_chars_ban477}, 
    {0x5514, 0x5514, gb_chars_ban478}, 
    {0x551B, 0x551B, gb_chars_ban479}, 
    {0x5520, 0x5520, gb_chars_ban480}, 
    {0x5522, 0x5524, gb_chars_ban481}, 
    {0x5527, 0x5527, gb_chars_ban482}, 
    {0x552A, 0x552A, gb_chars_ban483}, 
    {0x552C, 0x552C, gb_chars_ban484}, 
    {0x552E, 0x5531, gb_chars_ban485}, 
    {0x5533, 0x5533, gb_chars_ban486}, 
    {0x5537, 0x5537, gb_chars_ban487}, 
    {0x553C, 0x553C, gb_chars_ban488}, 
    {0x553E, 0x553F, gb_chars_ban489}, 
    {0x5541, 0x5541, gb_chars_ban490}, 
    {0x5543, 0x5544, gb_chars_ban491}, 
    {0x5546, 0x5546, gb_chars_ban492}, 
    {0x5549, 0x554A, gb_chars_ban493}, 
    {0x5550, 0x5550, gb_chars_ban494}, 
    {0x5555, 0x5556, gb_chars_ban495}, 
    {0x555C, 0x555C, gb_chars_ban496}, 
    {0x5561, 0x5561, gb_chars_ban497}, 
    {0x5564, 0x5567, gb_chars_ban498}, 
    {0x556A, 0x556A, gb_chars_ban499}, 
    {0x556C, 0x556E, gb_chars_ban500}, 
    {0x5575, 0x5578, gb_chars_ban501}, 
    {0x557B, 0x557C, gb_chars_ban502}, 
    {0x557E, 0x557E, gb_chars_ban503}, 
    {0x5580, 0x5584, gb_chars_ban504}, 
    {0x5587, 0x558B, gb_chars_ban505}, 
    {0x558F, 0x558F, gb_chars_ban506}, 
    {0x5591, 0x5591, gb_chars_ban507}, 
    {0x5594, 0x5594, gb_chars_ban508}, 
    {0x5598, 0x5599, gb_chars_ban509}, 
    {0x559C, 0x559D, gb_chars_ban510}, 
    {0x559F, 0x559F, gb_chars_ban511}, 
    {0x55A7, 0x55A7, gb_chars_ban512}, 
    {0x55B1, 0x55B1, gb_chars_ban513}, 
    {0x55B3, 0x55B3, gb_chars_ban514}, 
    {0x55B5, 0x55B5, gb_chars_ban515}, 
    {0x55B7, 0x55B7, gb_chars_ban516}, 
    {0x55B9, 0x55B9, gb_chars_ban517}, 
    {0x55BB, 0x55BB, gb_chars_ban518}, 
    {0x55BD, 0x55BE, gb_chars_ban519}, 
    {0x55C4, 0x55C5, gb_chars_ban520}, 
    {0x55C9, 0x55C9, gb_chars_ban521}, 
    {0x55CC, 0x55CD, gb_chars_ban522}, 
    {0x55D1, 0x55D4, gb_chars_ban523}, 
    {0x55D6, 0x55D6, gb_chars_ban524}, 
    {0x55DC, 0x55DD, gb_chars_ban525}, 
    {0x55DF, 0x55DF, gb_chars_ban526}, 
    {0x55E1, 0x55E1, gb_chars_ban527}, 
    {0x55E3, 0x55E6, gb_chars_ban528}, 
    {0x55E8, 0x55E8, gb_chars_ban529}, 
    {0x55EA, 0x55EC, gb_chars_ban530}, 
    {0x55EF, 0x55EF, gb_chars_ban531}, 
    {0x55F2, 0x55F3, gb_chars_ban532}, 
    {0x55F5, 0x55F5, gb_chars_ban533}, 
    {0x55F7, 0x55F7, gb_chars_ban534}, 
    {0x55FD, 0x55FE, gb_chars_ban535}, 
    {0x5600, 0x5601, gb_chars_ban536}, 
    {0x5608, 0x5609, gb_chars_ban537}, 
    {0x560C, 0x560C, gb_chars_ban538}, 
    {0x560E, 0x560F, gb_chars_ban539}, 
    {0x5618, 0x5618, gb_chars_ban540}, 
    {0x561B, 0x561B, gb_chars_ban541}, 
    {0x561E, 0x561F, gb_chars_ban542}, 
    {0x5623, 0x5624, gb_chars_ban543}, 
    {0x5627, 0x5627, gb_chars_ban544}, 
    {0x562C, 0x562D, gb_chars_ban545}, 
    {0x5631, 0x5632, gb_chars_ban546}, 
    {0x5634, 0x5634, gb_chars_ban547}, 
    {0x5636, 0x5636, gb_chars_ban548}, 
    {0x5639, 0x5639, gb_chars_ban549}, 
    {0x563B, 0x563B, gb_chars_ban550}, 
    {0x563F, 0x563F, gb_chars_ban551}, 
    {0x564C, 0x564E, gb_chars_ban552}, 
    {0x5654, 0x5654, gb_chars_ban553}, 
    {0x5657, 0x5659, gb_chars_ban554}, 
    {0x565C, 0x565C, gb_chars_ban555}, 
    {0x5662, 0x5662, gb_chars_ban556}, 
    {0x5664, 0x5664, gb_chars_ban557}, 
    {0x5668, 0x566C, gb_chars_ban558}, 
    {0x5671, 0x5671, gb_chars_ban559}, 
    {0x5676, 0x5676, gb_chars_ban560}, 
    {0x567B, 0x567C, gb_chars_ban561}, 
    {0x5685, 0x5686, gb_chars_ban562}, 
    {0x568E, 0x568F, gb_chars_ban563}, 
    {0x5693, 0x5693, gb_chars_ban564}, 
    {0x56A3, 0x56A3, gb_chars_ban565}, 
    {0x56AF, 0x56AF, gb_chars_ban566}, 
    {0x56B7, 0x56B7, gb_chars_ban567}, 
    {0x56BC, 0x56BC, gb_chars_ban568}, 
    {0x56CA, 0x56CA, gb_chars_ban569}, 
    {0x56D4, 0x56D4, gb_chars_ban570}, 
    {0x56D7, 0x56D7, gb_chars_ban571}, 
    {0x56DA, 0x56DB, gb_chars_ban572}, 
    {0x56DD, 0x56E2, gb_chars_ban573}, 
    {0x56E4, 0x56E4, gb_chars_ban574}, 
    {0x56EB, 0x56EB, gb_chars_ban575}, 
    {0x56ED, 0x56ED, gb_chars_ban576}, 
    {0x56F0, 0x56F1, gb_chars_ban577}, 
    {0x56F4, 0x56F5, gb_chars_ban578}, 
    {0x56F9, 0x56FA, gb_chars_ban579}, 
    {0x56FD, 0x56FF, gb_chars_ban580}, 
    {0x5703, 0x5704, gb_chars_ban581}, 
    {0x5706, 0x5706, gb_chars_ban582}, 
    {0x5708, 0x570A, gb_chars_ban583}, 
    {0x571C, 0x571C, gb_chars_ban584}, 
    {0x571F, 0x571F, gb_chars_ban585}, 
    {0x5723, 0x5723, gb_chars_ban586}, 
    {0x5728, 0x572A, gb_chars_ban587}, 
    {0x572C, 0x5730, gb_chars_ban588}, 
    {0x5733, 0x5733, gb_chars_ban589}, 
    {0x5739, 0x573B, gb_chars_ban590}, 
    {0x573E, 0x573E, gb_chars_ban591}, 
    {0x5740, 0x5740, gb_chars_ban592}, 
    {0x5742, 0x5742, gb_chars_ban593}, 
    {0x5747, 0x5747, gb_chars_ban594}, 
    {0x574A, 0x574A, gb_chars_ban595}, 
    {0x574C, 0x5751, gb_chars_ban596}, 
    {0x5757, 0x5757, gb_chars_ban597}, 
    {0x575A, 0x5761, gb_chars_ban598}, 
    {0x5764, 0x5764, gb_chars_ban599}, 
    {0x5766, 0x5766, gb_chars_ban600}, 
    {0x5768, 0x576B, gb_chars_ban601}, 
    {0x576D, 0x576D, gb_chars_ban602}, 
    {0x576F, 0x576F, gb_chars_ban603}, 
    {0x5773, 0x5773, gb_chars_ban604}, 
    {0x5776, 0x5777, gb_chars_ban605}, 
    {0x577B, 0x577C, gb_chars_ban606}, 
    {0x5782, 0x5786, gb_chars_ban607}, 
    {0x578B, 0x578C, gb_chars_ban608}, 
    {0x5792, 0x5793, gb_chars_ban609}, 
    {0x579B, 0x579B, gb_chars_ban610}, 
    {0x57A0, 0x57A4, gb_chars_ban611}, 
    {0x57A6, 0x57A7, gb_chars_ban612}, 
    {0x57A9, 0x57A9, gb_chars_ban613}, 
    {0x57AB, 0x57AB, gb_chars_ban614}, 
    {0x57AD, 0x57AE, gb_chars_ban615}, 
    {0x57B2, 0x57B2, gb_chars_ban616}, 
    {0x57B4, 0x57B4, gb_chars_ban617}, 
    {0x57B8, 0x57B8, gb_chars_ban618}, 
    {0x57C2, 0x57C3, gb_chars_ban619}, 
    {0x57CB, 0x57CB, gb_chars_ban620}, 
    {0x57CE, 0x57CF, gb_chars_ban621}, 
    {0x57D2, 0x57D2, gb_chars_ban622}, 
    {0x57D4, 0x57D5, gb_chars_ban623}, 
    {0x57D8, 0x57DA, gb_chars_ban624}, 
    {0x57DD, 0x57DD, gb_chars_ban625}, 
    {0x57DF, 0x57E0, gb_chars_ban626}, 
    {0x57E4, 0x57E4, gb_chars_ban627}, 
    {0x57ED, 0x57ED, gb_chars_ban628}, 
    {0x57EF, 0x57EF, gb_chars_ban629}, 
    {0x57F4, 0x57F4, gb_chars_ban630}, 
    {0x57F8, 0x57FA, gb_chars_ban631}, 
    {0x57FD, 0x57FD, gb_chars_ban632}, 
    {0x5800, 0x5800, gb_chars_ban633}, 
    {0x5802, 0x5802, gb_chars_ban634}, 
    {0x5806, 0x5807, gb_chars_ban635}, 
    {0x580B, 0x580B, gb_chars_ban636}, 
    {0x580D, 0x580D, gb_chars_ban637}, 
    {0x5811, 0x5811, gb_chars_ban638}, 
    {0x5815, 0x5815, gb_chars_ban639}, 
    {0x5819, 0x5819, gb_chars_ban640}, 
    {0x581E, 0x581E, gb_chars_ban641}, 
    {0x5820, 0x5821, gb_chars_ban642}, 
    {0x5824, 0x5824, gb_chars_ban643}, 
    {0x582A, 0x582A, gb_chars_ban644}, 
    {0x5830, 0x5830, gb_chars_ban645}, 
    {0x5835, 0x5835, gb_chars_ban646}, 
    {0x5844, 0x5844, gb_chars_ban647}, 
    {0x584C, 0x584D, gb_chars_ban648}, 
    {0x5851, 0x5851, gb_chars_ban649}, 
    {0x5854, 0x5854, gb_chars_ban650}, 
    {0x5858, 0x5858, gb_chars_ban651}, 
    {0x585E, 0x585E, gb_chars_ban652}, 
    {0x5865, 0x5865, gb_chars_ban653}, 
    {0x586B, 0x586C, gb_chars_ban654}, 
    {0x587E, 0x587E, gb_chars_ban655}, 
    {0x5880, 0x5881, gb_chars_ban656}, 
    {0x5883, 0x5883, gb_chars_ban657}, 
    {0x5885, 0x5885, gb_chars_ban658}, 
    {0x5889, 0x5889, gb_chars_ban659}, 
    {0x5892, 0x5893, gb_chars_ban660}, 
    {0x5899, 0x589A, gb_chars_ban661}, 
    {0x589E, 0x589F, gb_chars_ban662}, 
    {0x58A8, 0x58A9, gb_chars_ban663}, 
    {0x58BC, 0x58BC, gb_chars_ban664}, 
    {0x58C1, 0x58C1, gb_chars_ban665}, 
    {0x58C5, 0x58C5, gb_chars_ban666}, 
    {0x58D1, 0x58D1, gb_chars_ban667}, 
    {0x58D5, 0x58D5, gb_chars_ban668}, 
    {0x58E4, 0x58E4, gb_chars_ban669}, 
    {0x58EB, 0x58EC, gb_chars_ban670}, 
    {0x58EE, 0x58EE, gb_chars_ban671}, 
    {0x58F0, 0x58F0, gb_chars_ban672}, 
    {0x58F3, 0x58F3, gb_chars_ban673}, 
    {0x58F6, 0x58F6, gb_chars_ban674}, 
    {0x58F9, 0x58F9, gb_chars_ban675}, 
    {0x5902, 0x5902, gb_chars_ban676}, 
    {0x5904, 0x5904, gb_chars_ban677}, 
    {0x5907, 0x5907, gb_chars_ban678}, 
    {0x590D, 0x590D, gb_chars_ban679}, 
    {0x590F, 0x590F, gb_chars_ban680}, 
    {0x5914, 0x5916, gb_chars_ban681}, 
    {0x5919, 0x591A, gb_chars_ban682}, 
    {0x591C, 0x591C, gb_chars_ban683}, 
    {0x591F, 0x591F, gb_chars_ban684}, 
    {0x5924, 0x5925, gb_chars_ban685}, 
    {0x5927, 0x5927, gb_chars_ban686}, 
    {0x5929, 0x592B, gb_chars_ban687}, 
    {0x592D, 0x592F, gb_chars_ban688}, 
    {0x5931, 0x5931, gb_chars_ban689}, 
    {0x5934, 0x5934, gb_chars_ban690}, 
    {0x5937, 0x593A, gb_chars_ban691}, 
    {0x593C, 0x593C, gb_chars_ban692}, 
    {0x5941, 0x5942, gb_chars_ban693}, 
    {0x5944, 0x5944, gb_chars_ban694}, 
    {0x5947, 0x5949, gb_chars_ban695}, 
    {0x594B, 0x594B, gb_chars_ban696}, 
    {0x594E, 0x594F, gb_chars_ban697}, 
    {0x5951, 0x5951, gb_chars_ban698}, 
    {0x5954, 0x5958, gb_chars_ban699}, 
    {0x595A, 0x595A, gb_chars_ban700}, 
    {0x5960, 0x5960, gb_chars_ban701}, 
    {0x5962, 0x5962, gb_chars_ban702}, 
    {0x5965, 0x5965, gb_chars_ban703}, 
    {0x5973, 0x5974, gb_chars_ban704}, 
    {0x5976, 0x5976, gb_chars_ban705}, 
    {0x5978, 0x5979, gb_chars_ban706}, 
    {0x597D, 0x597D, gb_chars_ban707}, 
    {0x5981, 0x5984, gb_chars_ban708}, 
    {0x5986, 0x5988, gb_chars_ban709}, 
    {0x598A, 0x598A, gb_chars_ban710}, 
    {0x598D, 0x598D, gb_chars_ban711}, 
    {0x5992, 0x5993, gb_chars_ban712}, 
    {0x5996, 0x5997, gb_chars_ban713}, 
    {0x5999, 0x5999, gb_chars_ban714}, 
    {0x599E, 0x599E, gb_chars_ban715}, 
    {0x59A3, 0x59A5, gb_chars_ban716}, 
    {0x59A8, 0x59AB, gb_chars_ban717}, 
    {0x59AE, 0x59AF, gb_chars_ban718}, 
    {0x59B2, 0x59B2, gb_chars_ban719}, 
    {0x59B9, 0x59B9, gb_chars_ban720}, 
    {0x59BB, 0x59BB, gb_chars_ban721}, 
    {0x59BE, 0x59BE, gb_chars_ban722}, 
    {0x59C6, 0x59C6, gb_chars_ban723}, 
    {0x59CA, 0x59CB, gb_chars_ban724}, 
    {0x59D0, 0x59D4, gb_chars_ban725}, 
    {0x59D7, 0x59D8, gb_chars_ban726}, 
    {0x59DA, 0x59DA, gb_chars_ban727}, 
    {0x59DC, 0x59DD, gb_chars_ban728}, 
    {0x59E3, 0x59E3, gb_chars_ban729}, 
    {0x59E5, 0x59E5, gb_chars_ban730}, 
    {0x59E8, 0x59E8, gb_chars_ban731}, 
    {0x59EC, 0x59EC, gb_chars_ban732}, 
    {0x59F9, 0x59F9, gb_chars_ban733}, 
    {0x59FB, 0x59FB, gb_chars_ban734}, 
    {0x59FF, 0x59FF, gb_chars_ban735}, 
    {0x5A01, 0x5A01, gb_chars_ban736}, 
    {0x5A03, 0x5A09, gb_chars_ban737}, 
    {0x5A0C, 0x5A0C, gb_chars_ban738}, 
    {0x5A11, 0x5A11, gb_chars_ban739}, 
    {0x5A13, 0x5A13, gb_chars_ban740}, 
    {0x5A18, 0x5A18, gb_chars_ban741}, 
    {0x5A1C, 0x5A1C, gb_chars_ban742}, 
    {0x5A1F, 0x5A20, gb_chars_ban743}, 
    {0x5A23, 0x5A23, gb_chars_ban744}, 
    {0x5A25, 0x5A25, gb_chars_ban745}, 
    {0x5A29, 0x5A29, gb_chars_ban746}, 
    {0x5A31, 0x5A32, gb_chars_ban747}, 
    {0x5A34, 0x5A34, gb_chars_ban748}, 
    {0x5A36, 0x5A36, gb_chars_ban749}, 
    {0x5A3C, 0x5A3C, gb_chars_ban750}, 
    {0x5A40, 0x5A40, gb_chars_ban751}, 
    {0x5A46, 0x5A46, gb_chars_ban752}, 
    {0x5A49, 0x5A4A, gb_chars_ban753}, 
    {0x5A55, 0x5A55, gb_chars_ban754}, 
    {0x5A5A, 0x5A5A, gb_chars_ban755}, 
    {0x5A62, 0x5A62, gb_chars_ban756}, 
    {0x5A67, 0x5A67, gb_chars_ban757}, 
    {0x5A6A, 0x5A6A, gb_chars_ban758}, 
    {0x5A74, 0x5A77, gb_chars_ban759}, 
    {0x5A7A, 0x5A7A, gb_chars_ban760}, 
    {0x5A7F, 0x5A7F, gb_chars_ban761}, 
    {0x5A92, 0x5A92, gb_chars_ban762}, 
    {0x5A9A, 0x5A9B, gb_chars_ban763}, 
    {0x5AAA, 0x5AAA, gb_chars_ban764}, 
    {0x5AB2, 0x5AB3, gb_chars_ban765}, 
    {0x5AB5, 0x5AB5, gb_chars_ban766}, 
    {0x5AB8, 0x5AB8, gb_chars_ban767}, 
    {0x5ABE, 0x5ABE, gb_chars_ban768}, 
    {0x5AC1, 0x5AC2, gb_chars_ban769}, 
    {0x5AC9, 0x5AC9, gb_chars_ban770}, 
    {0x5ACC, 0x5ACC, gb_chars_ban771}, 
    {0x5AD2, 0x5AD2, gb_chars_ban772}, 
    {0x5AD4, 0x5AD4, gb_chars_ban773}, 
    {0x5AD6, 0x5AD6, gb_chars_ban774}, 
    {0x5AD8, 0x5AD8, gb_chars_ban775}, 
    {0x5ADC, 0x5ADC, gb_chars_ban776}, 
    {0x5AE0, 0x5AE1, gb_chars_ban777}, 
    {0x5AE3, 0x5AE3, gb_chars_ban778}, 
    {0x5AE6, 0x5AE6, gb_chars_ban779}, 
    {0x5AE9, 0x5AE9, gb_chars_ban780}, 
    {0x5AEB, 0x5AEB, gb_chars_ban781}, 
    {0x5AF1, 0x5AF1, gb_chars_ban782}, 
    {0x5B09, 0x5B09, gb_chars_ban783}, 
    {0x5B16, 0x5B17, gb_chars_ban784}, 
    {0x5B32, 0x5B32, gb_chars_ban785}, 
    {0x5B34, 0x5B34, gb_chars_ban786}, 
    {0x5B37, 0x5B37, gb_chars_ban787}, 
    {0x5B40, 0x5B40, gb_chars_ban788}, 
    {0x5B50, 0x5B51, gb_chars_ban789}, 
    {0x5B53, 0x5B55, gb_chars_ban790}, 
    {0x5B57, 0x5B5D, gb_chars_ban791}, 
    {0x5B5F, 0x5B5F, gb_chars_ban792}, 
    {0x5B62, 0x5B66, gb_chars_ban793}, 
    {0x5B69, 0x5B6A, gb_chars_ban794}, 
    {0x5B6C, 0x5B6C, gb_chars_ban795}, 
    {0x5B70, 0x5B71, gb_chars_ban796}, 
    {0x5B73, 0x5B73, gb_chars_ban797}, 
    {0x5B75, 0x5B75, gb_chars_ban798}, 
    {0x5B7A, 0x5B7A, gb_chars_ban799}, 
    {0x5B7D, 0x5B7D, gb_chars_ban800}, 
    {0x5B80, 0x5B81, gb_chars_ban801}, 
    {0x5B83, 0x5B85, gb_chars_ban802}, 
    {0x5B87, 0x5B89, gb_chars_ban803}, 
    {0x5B8B, 0x5B8C, gb_chars_ban804}, 
    {0x5B8F, 0x5B8F, gb_chars_ban805}, 
    {0x5B93, 0x5B93, gb_chars_ban806}, 
    {0x5B95, 0x5B95, gb_chars_ban807}, 
    {0x5B97, 0x5B9E, gb_chars_ban808}, 
    {0x5BA0, 0x5BA6, gb_chars_ban809}, 
    {0x5BAA, 0x5BAB, gb_chars_ban810}, 
    {0x5BB0, 0x5BB0, gb_chars_ban811}, 
    {0x5BB3, 0x5BB6, gb_chars_ban812}, 
    {0x5BB8, 0x5BB9, gb_chars_ban813}, 
    {0x5BBD, 0x5BBF, gb_chars_ban814}, 
    {0x5BC2, 0x5BC2, gb_chars_ban815}, 
    {0x5BC4, 0x5BC7, gb_chars_ban816}, 
    {0x5BCC, 0x5BCC, gb_chars_ban817}, 
    {0x5BD0, 0x5BD0, gb_chars_ban818}, 
    {0x5BD2, 0x5BD3, gb_chars_ban819}, 
    {0x5BDD, 0x5BDF, gb_chars_ban820}, 
    {0x5BE1, 0x5BE1, gb_chars_ban821}, 
    {0x5BE4, 0x5BE5, gb_chars_ban822}, 
    {0x5BE8, 0x5BE8, gb_chars_ban823}, 
    {0x5BEE, 0x5BEE, gb_chars_ban824}, 
    {0x5BF0, 0x5BF0, gb_chars_ban825}, 
    {0x5BF8, 0x5BFC, gb_chars_ban826}, 
    {0x5BFF, 0x5BFF, gb_chars_ban827}, 
    {0x5C01, 0x5C01, gb_chars_ban828}, 
    {0x5C04, 0x5C04, gb_chars_ban829}, 
    {0x5C06, 0x5C06, gb_chars_ban830}, 
    {0x5C09, 0x5C0A, gb_chars_ban831}, 
    {0x5C0F, 0x5C0F, gb_chars_ban832}, 
    {0x5C11, 0x5C11, gb_chars_ban833}, 
    {0x5C14, 0x5C16, gb_chars_ban834}, 
    {0x5C18, 0x5C18, gb_chars_ban835}, 
    {0x5C1A, 0x5C1A, gb_chars_ban836}, 
    {0x5C1C, 0x5C1D, gb_chars_ban837}, 
    {0x5C22, 0x5C22, gb_chars_ban838}, 
    {0x5C24, 0x5C25, gb_chars_ban839}, 
    {0x5C27, 0x5C27, gb_chars_ban840}, 
    {0x5C2C, 0x5C2C, gb_chars_ban841}, 
    {0x5C31, 0x5C31, gb_chars_ban842}, 
    {0x5C34, 0x5C34, gb_chars_ban843}, 
    {0x5C38, 0x5C42, gb_chars_ban844}, 
    {0x5C45, 0x5C45, gb_chars_ban845}, 
    {0x5C48, 0x5C4B, gb_chars_ban846}, 
    {0x5C4E, 0x5C51, gb_chars_ban847}, 
    {0x5C55, 0x5C55, gb_chars_ban848}, 
    {0x5C59, 0x5C59, gb_chars_ban849}, 
    {0x5C5E, 0x5C5E, gb_chars_ban850}, 
    {0x5C60, 0x5C61, gb_chars_ban851}, 
    {0x5C63, 0x5C63, gb_chars_ban852}, 
    {0x5C65, 0x5C66, gb_chars_ban853}, 
    {0x5C6E, 0x5C6F, gb_chars_ban854}, 
    {0x5C71, 0x5C71, gb_chars_ban855}, 
    {0x5C79, 0x5C7A, gb_chars_ban856}, 
    {0x5C7F, 0x5C7F, gb_chars_ban857}, 
    {0x5C81, 0x5C82, gb_chars_ban858}, 
    {0x5C88, 0x5C88, gb_chars_ban859}, 
    {0x5C8C, 0x5C8D, gb_chars_ban860}, 
    {0x5C90, 0x5C91, gb_chars_ban861}, 
    {0x5C94, 0x5C94, gb_chars_ban862}, 
    {0x5C96, 0x5C9C, gb_chars_ban863}, 
    {0x5CA2, 0x5CA3, gb_chars_ban864}, 
    {0x5CA9, 0x5CA9, gb_chars_ban865}, 
    {0x5CAB, 0x5CAD, gb_chars_ban866}, 
    {0x5CB1, 0x5CB1, gb_chars_ban867}, 
    {0x5CB3, 0x5CB3, gb_chars_ban868}, 
    {0x5CB5, 0x5CB5, gb_chars_ban869}, 
    {0x5CB7, 0x5CB8, gb_chars_ban870}, 
    {0x5CBD, 0x5CBD, gb_chars_ban871}, 
    {0x5CBF, 0x5CBF, gb_chars_ban872}, 
    {0x5CC1, 0x5CC1, gb_chars_ban873}, 
    {0x5CC4, 0x5CC4, gb_chars_ban874}, 
    {0x5CCB, 0x5CCB, gb_chars_ban875}, 
    {0x5CD2, 0x5CD2, gb_chars_ban876}, 
    {0x5CD9, 0x5CD9, gb_chars_ban877}, 
    {0x5CE1, 0x5CE1, gb_chars_ban878}, 
    {0x5CE4, 0x5CE6, gb_chars_ban879}, 
    {0x5CE8, 0x5CE8, gb_chars_ban880}, 
    {0x5CEA, 0x5CEA, gb_chars_ban881}, 
    {0x5CED, 0x5CED, gb_chars_ban882}, 
    {0x5CF0, 0x5CF0, gb_chars_ban883}, 
    {0x5CFB, 0x5CFB, gb_chars_ban884}, 
    {0x5D02, 0x5D03, gb_chars_ban885}, 
    {0x5D06, 0x5D07, gb_chars_ban886}, 
    {0x5D0E, 0x5D0E, gb_chars_ban887}, 
    {0x5D14, 0x5D14, gb_chars_ban888}, 
    {0x5D16, 0x5D16, gb_chars_ban889}, 
    {0x5D1B, 0x5D1B, gb_chars_ban890}, 
    {0x5D1E, 0x5D1E, gb_chars_ban891}, 
    {0x5D24, 0x5D24, gb_chars_ban892}, 
    {0x5D26, 0x5D27, gb_chars_ban893}, 
    {0x5D29, 0x5D29, gb_chars_ban894}, 
    {0x5D2D, 0x5D2E, gb_chars_ban895}, 
    {0x5D34, 0x5D34, gb_chars_ban896}, 
    {0x5D3D, 0x5D3E, gb_chars_ban897}, 
    {0x5D47, 0x5D47, gb_chars_ban898}, 
    {0x5D4A, 0x5D4C, gb_chars_ban899}, 
    {0x5D58, 0x5D58, gb_chars_ban900}, 
    {0x5D5B, 0x5D5B, gb_chars_ban901}, 
    {0x5D5D, 0x5D5D, gb_chars_ban902}, 
    {0x5D69, 0x5D69, gb_chars_ban903}, 
    {0x5D6B, 0x5D6C, gb_chars_ban904}, 
    {0x5D6F, 0x5D6F, gb_chars_ban905}, 
    {0x5D74, 0x5D74, gb_chars_ban906}, 
    {0x5D82, 0x5D82, gb_chars_ban907}, 
    {0x5D99, 0x5D99, gb_chars_ban908}, 
    {0x5D9D, 0x5D9D, gb_chars_ban909}, 
    {0x5DB7, 0x5DB7, gb_chars_ban910}, 
    {0x5DC5, 0x5DC5, gb_chars_ban911}, 
    {0x5DCD, 0x5DCD, gb_chars_ban912}, 
    {0x5DDB, 0x5DDB, gb_chars_ban913}, 
    {0x5DDD, 0x5DDE, gb_chars_ban914}, 
    {0x5DE1, 0x5DE2, gb_chars_ban915}, 
    {0x5DE5, 0x5DE9, gb_chars_ban916}, 
    {0x5DEB, 0x5DEB, gb_chars_ban917}, 
    {0x5DEE, 0x5DEF, gb_chars_ban918}, 
    {0x5DF1, 0x5DF4, gb_chars_ban919}, 
    {0x5DF7, 0x5DF7, gb_chars_ban920}, 
    {0x5DFD, 0x5DFE, gb_chars_ban921}, 
    {0x5E01, 0x5E03, gb_chars_ban922}, 
    {0x5E05, 0x5E06, gb_chars_ban923}, 
    {0x5E08, 0x5E08, gb_chars_ban924}, 
    {0x5E0C, 0x5E0C, gb_chars_ban925}, 
    {0x5E0F, 0x5E11, gb_chars_ban926}, 
    {0x5E14, 0x5E16, gb_chars_ban927}, 
    {0x5E18, 0x5E1D, gb_chars_ban928}, 
    {0x5E26, 0x5E27, gb_chars_ban929}, 
    {0x5E2D, 0x5E2E, gb_chars_ban930}, 
    {0x5E31, 0x5E31, gb_chars_ban931}, 
    {0x5E37, 0x5E38, gb_chars_ban932}, 
    {0x5E3B, 0x5E3D, gb_chars_ban933}, 
    {0x5E42, 0x5E42, gb_chars_ban934}, 
    {0x5E44, 0x5E45, gb_chars_ban935}, 
    {0x5E4C, 0x5E4C, gb_chars_ban936}, 
    {0x5E54, 0x5E55, gb_chars_ban937}, 
    {0x5E5B, 0x5E5B, gb_chars_ban938}, 
    {0x5E5E, 0x5E5E, gb_chars_ban939}, 
    {0x5E61, 0x5E62, gb_chars_ban940}, 
    {0x5E72, 0x5E74, gb_chars_ban941}, 
    {0x5E76, 0x5E76, gb_chars_ban942}, 
    {0x5E78, 0x5E78, gb_chars_ban943}, 
    {0x5E7A, 0x5E7D, gb_chars_ban944}, 
    {0x5E7F, 0x5E80, gb_chars_ban945}, 
    {0x5E84, 0x5E84, gb_chars_ban946}, 
    {0x5E86, 0x5E87, gb_chars_ban947}, 
    {0x5E8A, 0x5E8B, gb_chars_ban948}, 
    {0x5E8F, 0x5E91, gb_chars_ban949}, 
    {0x5E93, 0x5E97, gb_chars_ban950}, 
    {0x5E99, 0x5E9A, gb_chars_ban951}, 
    {0x5E9C, 0x5E9C, gb_chars_ban952}, 
    {0x5E9E, 0x5EA0, gb_chars_ban953}, 
    {0x5EA5, 0x5EA7, gb_chars_ban954}, 
    {0x5EAD, 0x5EAD, gb_chars_ban955}, 
    {0x5EB3, 0x5EB3, gb_chars_ban956}, 
    {0x5EB5, 0x5EB9, gb_chars_ban957}, 
    {0x5EBE, 0x5EBE, gb_chars_ban958}, 
    {0x5EC9, 0x5ECA, gb_chars_ban959}, 
    {0x5ED1, 0x5ED3, gb_chars_ban960}, 
    {0x5ED6, 0x5ED6, gb_chars_ban961}, 
    {0x5EDB, 0x5EDB, gb_chars_ban962}, 
    {0x5EE8, 0x5EE8, gb_chars_ban963}, 
    {0x5EEA, 0x5EEA, gb_chars_ban964}, 
    {0x5EF4, 0x5EF4, gb_chars_ban965}, 
    {0x5EF6, 0x5EF7, gb_chars_ban966}, 
    {0x5EFA, 0x5EFA, gb_chars_ban967}, 
    {0x5EFE, 0x5F04, gb_chars_ban968}, 
    {0x5F08, 0x5F08, gb_chars_ban969}, 
    {0x5F0A, 0x5F0B, gb_chars_ban970}, 
    {0x5F0F, 0x5F0F, gb_chars_ban971}, 
    {0x5F11, 0x5F11, gb_chars_ban972}, 
    {0x5F13, 0x5F13, gb_chars_ban973}, 
    {0x5F15, 0x5F15, gb_chars_ban974}, 
    {0x5F17, 0x5F18, gb_chars_ban975}, 
    {0x5F1B, 0x5F1B, gb_chars_ban976}, 
    {0x5F1F, 0x5F20, gb_chars_ban977}, 
    {0x5F25, 0x5F27, gb_chars_ban978}, 
    {0x5F29, 0x5F2A, gb_chars_ban979}, 
    {0x5F2D, 0x5F2D, gb_chars_ban980}, 
    {0x5F2F, 0x5F2F, gb_chars_ban981}, 
    {0x5F31, 0x5F31, gb_chars_ban982}, 
    {0x5F39, 0x5F3A, gb_chars_ban983}, 
    {0x5F3C, 0x5F3C, gb_chars_ban984}, 
    {0x5F40, 0x5F40, gb_chars_ban985}, 
    {0x5F50, 0x5F50, gb_chars_ban986}, 
    {0x5F52, 0x5F53, gb_chars_ban987}, 
    {0x5F55, 0x5F58, gb_chars_ban988}, 
    {0x5F5D, 0x5F5D, gb_chars_ban989}, 
    {0x5F61, 0x5F62, gb_chars_ban990}, 
    {0x5F64, 0x5F64, gb_chars_ban991}, 
    {0x5F66, 0x5F66, gb_chars_ban992}, 
    {0x5F69, 0x5F6A, gb_chars_ban993}, 
    {0x5F6C, 0x5F6D, gb_chars_ban994}, 
    {0x5F70, 0x5F71, gb_chars_ban995}, 
    {0x5F73, 0x5F73, gb_chars_ban996}, 
    {0x5F77, 0x5F77, gb_chars_ban997}, 
    {0x5F79, 0x5F79, gb_chars_ban998}, 
    {0x5F7B, 0x5F7C, gb_chars_ban999}, 
    {0x5F80, 0x5F82, gb_chars_ban1000}, 
    {0x5F84, 0x5F85, gb_chars_ban1001}, 
    {0x5F87, 0x5F8C, gb_chars_ban1002}, 
    {0x5F90, 0x5F90, gb_chars_ban1003}, 
    {0x5F92, 0x5F92, gb_chars_ban1004}, 
    {0x5F95, 0x5F95, gb_chars_ban1005}, 
    {0x5F97, 0x5F99, gb_chars_ban1006}, 
    {0x5F9C, 0x5F9C, gb_chars_ban1007}, 
    {0x5FA1, 0x5FA1, gb_chars_ban1008}, 
    {0x5FA8, 0x5FA8, gb_chars_ban1009}, 
    {0x5FAA, 0x5FAA, gb_chars_ban1010}, 
    {0x5FAD, 0x5FAE, gb_chars_ban1011}, 
    {0x5FB5, 0x5FB5, gb_chars_ban1012}, 
    {0x5FB7, 0x5FB7, gb_chars_ban1013}, 
    {0x5FBC, 0x5FBD, gb_chars_ban1014}, 
    {0x5FC3, 0x5FC6, gb_chars_ban1015}, 
    {0x5FC9, 0x5FC9, gb_chars_ban1016}, 
    {0x5FCC, 0x5FCD, gb_chars_ban1017}, 
    {0x5FCF, 0x5FD2, gb_chars_ban1018}, 
    {0x5FD6, 0x5FD9, gb_chars_ban1019}, 
    {0x5FDD, 0x5FDD, gb_chars_ban1020}, 
    {0x5FE0, 0x5FE1, gb_chars_ban1021}, 
    {0x5FE4, 0x5FE4, gb_chars_ban1022}, 
    {0x5FE7, 0x5FE7, gb_chars_ban1023}, 
    {0x5FEA, 0x5FEB, gb_chars_ban1024}, 
    {0x5FED, 0x5FEE, gb_chars_ban1025}, 
    {0x5FF1, 0x5FF1, gb_chars_ban1026}, 
    {0x5FF5, 0x5FF5, gb_chars_ban1027}, 
    {0x5FF8, 0x5FF8, gb_chars_ban1028}, 
    {0x5FFB, 0x5FFB, gb_chars_ban1029}, 
    {0x5FFD, 0x6006, gb_chars_ban1030}, 
    {0x600A, 0x600A, gb_chars_ban1031}, 
    {0x600D, 0x600F, gb_chars_ban1032}, 
    {0x6012, 0x6012, gb_chars_ban1033}, 
    {0x6014, 0x6016, gb_chars_ban1034}, 
    {0x6019, 0x6019, gb_chars_ban1035}, 
    {0x601B, 0x601D, gb_chars_ban1036}, 
    {0x6020, 0x6021, gb_chars_ban1037}, 
    {0x6025, 0x602B, gb_chars_ban1038}, 
    {0x602F, 0x602F, gb_chars_ban1039}, 
    {0x6035, 0x6035, gb_chars_ban1040}, 
    {0x603B, 0x603C, gb_chars_ban1041}, 
    {0x603F, 0x603F, gb_chars_ban1042}, 
    {0x6041, 0x6043, gb_chars_ban1043}, 
    {0x604B, 0x604B, gb_chars_ban1044}, 
    {0x604D, 0x604D, gb_chars_ban1045}, 
    {0x6050, 0x6050, gb_chars_ban1046}, 
    {0x6052, 0x6052, gb_chars_ban1047}, 
    {0x6055, 0x6055, gb_chars_ban1048}, 
    {0x6059, 0x605A, gb_chars_ban1049}, 
    {0x605D, 0x605D, gb_chars_ban1050}, 
    {0x6062, 0x6064, gb_chars_ban1051}, 
    {0x6067, 0x606D, gb_chars_ban1052}, 
    {0x606F, 0x6070, gb_chars_ban1053}, 
    {0x6073, 0x6073, gb_chars_ban1054}, 
    {0x6076, 0x6076, gb_chars_ban1055}, 
    {0x6078, 0x607D, gb_chars_ban1056}, 
    {0x607F, 0x607F, gb_chars_ban1057}, 
    {0x6083, 0x6084, gb_chars_ban1058}, 
    {0x6089, 0x6089, gb_chars_ban1059}, 
    {0x608C, 0x608D, gb_chars_ban1060}, 
    {0x6092, 0x6092, gb_chars_ban1061}, 
    {0x6094, 0x6094, gb_chars_ban1062}, 
    {0x6096, 0x6096, gb_chars_ban1063}, 
    {0x609A, 0x609B, gb_chars_ban1064}, 
    {0x609D, 0x609D, gb_chars_ban1065}, 
    {0x609F, 0x60A0, gb_chars_ban1066}, 
    {0x60A3, 0x60A3, gb_chars_ban1067}, 
    {0x60A6, 0x60A6, gb_chars_ban1068}, 
    {0x60A8, 0x60A8, gb_chars_ban1069}, 
    {0x60AB, 0x60AD, gb_chars_ban1070}, 
    {0x60AF, 0x60AF, gb_chars_ban1071}, 
    {0x60B1, 0x60B2, gb_chars_ban1072}, 
    {0x60B4, 0x60B4, gb_chars_ban1073}, 
    {0x60B8, 0x60B8, gb_chars_ban1074}, 
    {0x60BB, 0x60BC, gb_chars_ban1075}, 
    {0x60C5, 0x60C6, gb_chars_ban1076}, 
    {0x60CA, 0x60CB, gb_chars_ban1077}, 
    {0x60D1, 0x60D1, gb_chars_ban1078}, 
    {0x60D5, 0x60D5, gb_chars_ban1079}, 
    {0x60D8, 0x60D8, gb_chars_ban1080}, 
    {0x60DA, 0x60DA, gb_chars_ban1081}, 
    {0x60DC, 0x60DD, gb_chars_ban1082}, 
    {0x60DF, 0x60E0, gb_chars_ban1083}, 
    {0x60E6, 0x60E9, gb_chars_ban1084}, 
    {0x60EB, 0x60F0, gb_chars_ban1085}, 
    {0x60F3, 0x60F4, gb_chars_ban1086}, 
    {0x60F6, 0x60F6, gb_chars_ban1087}, 
    {0x60F9, 0x60FA, gb_chars_ban1088}, 
    {0x6100, 0x6101, gb_chars_ban1089}, 
    {0x6106, 0x6106, gb_chars_ban1090}, 
    {0x6108, 0x6109, gb_chars_ban1091}, 
    {0x610D, 0x610F, gb_chars_ban1092}, 
    {0x6115, 0x6115, gb_chars_ban1093}, 
    {0x611A, 0x611A, gb_chars_ban1094}, 
    {0x611F, 0x6120, gb_chars_ban1095}, 
    {0x6123, 0x6124, gb_chars_ban1096}, 
    {0x6126, 0x6127, gb_chars_ban1097}, 
    {0x612B, 0x612B, gb_chars_ban1098}, 
    {0x613F, 0x613F, gb_chars_ban1099}, 
    {0x6148, 0x6148, gb_chars_ban1100}, 
    {0x614A, 0x614A, gb_chars_ban1101}, 
    {0x614C, 0x614C, gb_chars_ban1102}, 
    {0x614E, 0x614E, gb_chars_ban1103}, 
    {0x6151, 0x6151, gb_chars_ban1104}, 
    {0x6155, 0x6155, gb_chars_ban1105}, 
    {0x615D, 0x615D, gb_chars_ban1106}, 
    {0x6162, 0x6162, gb_chars_ban1107}, 
    {0x6167, 0x6168, gb_chars_ban1108}, 
    {0x6170, 0x6170, gb_chars_ban1109}, 
    {0x6175, 0x6175, gb_chars_ban1110}, 
    {0x6177, 0x6177, gb_chars_ban1111}, 
    {0x618B, 0x618B, gb_chars_ban1112}, 
    {0x618E, 0x618E, gb_chars_ban1113}, 
    {0x6194, 0x6194, gb_chars_ban1114}, 
    {0x619D, 0x619D, gb_chars_ban1115}, 
    {0x61A7, 0x61A9, gb_chars_ban1116}, 
    {0x61AC, 0x61AC, gb_chars_ban1117}, 
    {0x61B7, 0x61B7, gb_chars_ban1118}, 
    {0x61BE, 0x61BE, gb_chars_ban1119}, 
    {0x61C2, 0x61C2, gb_chars_ban1120}, 
    {0x61C8, 0x61C8, gb_chars_ban1121}, 
    {0x61CA, 0x61CB, gb_chars_ban1122}, 
    {0x61D1, 0x61D2, gb_chars_ban1123}, 
    {0x61D4, 0x61D4, gb_chars_ban1124}, 
    {0x61E6, 0x61E6, gb_chars_ban1125}, 
    {0x61F5, 0x61F5, gb_chars_ban1126}, 
    {0x61FF, 0x61FF, gb_chars_ban1127}, 
    {0x6206, 0x6206, gb_chars_ban1128}, 
    {0x6208, 0x6208, gb_chars_ban1129}, 
    {0x620A, 0x6212, gb_chars_ban1130}, 
    {0x6215, 0x6218, gb_chars_ban1131}, 
    {0x621A, 0x621B, gb_chars_ban1132}, 
    {0x621F, 0x621F, gb_chars_ban1133}, 
    {0x6221, 0x6222, gb_chars_ban1134}, 
    {0x6224, 0x6225, gb_chars_ban1135}, 
    {0x622A, 0x622A, gb_chars_ban1136}, 
    {0x622C, 0x622C, gb_chars_ban1137}, 
    {0x622E, 0x622E, gb_chars_ban1138}, 
    {0x6233, 0x6234, gb_chars_ban1139}, 
    {0x6237, 0x6237, gb_chars_ban1140}, 
    {0x623D, 0x6241, gb_chars_ban1141}, 
    {0x6243, 0x6243, gb_chars_ban1142}, 
    {0x6247, 0x6249, gb_chars_ban1143}, 
    {0x624B, 0x624E, gb_chars_ban1144}, 
    {0x6251, 0x6254, gb_chars_ban1145}, 
    {0x6258, 0x6258, gb_chars_ban1146}, 
    {0x625B, 0x625B, gb_chars_ban1147}, 
    {0x6263, 0x6263, gb_chars_ban1148}, 
    {0x6266, 0x6267, gb_chars_ban1149}, 
    {0x6269, 0x6270, gb_chars_ban1150}, 
    {0x6273, 0x6273, gb_chars_ban1151}, 
    {0x6276, 0x6276, gb_chars_ban1152}, 
    {0x6279, 0x6279, gb_chars_ban1153}, 
    {0x627C, 0x627C, gb_chars_ban1154}, 
    {0x627E, 0x6280, gb_chars_ban1155}, 
    {0x6284, 0x6284, gb_chars_ban1156}, 
    {0x6289, 0x628A, gb_chars_ban1157}, 
    {0x6291, 0x6293, gb_chars_ban1158}, 
    {0x6295, 0x6298, gb_chars_ban1159}, 
    {0x629A, 0x629B, gb_chars_ban1160}, 
    {0x629F, 0x62A2, gb_chars_ban1161}, 
    {0x62A4, 0x62A5, gb_chars_ban1162}, 
    {0x62A8, 0x62A8, gb_chars_ban1163}, 
    {0x62AB, 0x62AC, gb_chars_ban1164}, 
    {0x62B1, 0x62B1, gb_chars_ban1165}, 
    {0x62B5, 0x62B5, gb_chars_ban1166}, 
    {0x62B9, 0x62B9, gb_chars_ban1167}, 
    {0x62BB, 0x62BD, gb_chars_ban1168}, 
    {0x62BF, 0x62BF, gb_chars_ban1169}, 
    {0x62C2, 0x62C2, gb_chars_ban1170}, 
    {0x62C4, 0x62CA, gb_chars_ban1171}, 
    {0x62CC, 0x62CE, gb_chars_ban1172}, 
    {0x62D0, 0x62D0, gb_chars_ban1173}, 
    {0x62D2, 0x62D4, gb_chars_ban1174}, 
    {0x62D6, 0x62DC, gb_chars_ban1175}, 
    {0x62DF, 0x62DF, gb_chars_ban1176}, 
    {0x62E2, 0x62E3, gb_chars_ban1177}, 
    {0x62E5, 0x62E9, gb_chars_ban1178}, 
    {0x62EC, 0x62EF, gb_chars_ban1179}, 
    {0x62F1, 0x62F1, gb_chars_ban1180}, 
    {0x62F3, 0x62F4, gb_chars_ban1181}, 
    {0x62F6, 0x62F7, gb_chars_ban1182}, 
    {0x62FC, 0x62FF, gb_chars_ban1183}, 
    {0x6301, 0x6302, gb_chars_ban1184}, 
    {0x6307, 0x6309, gb_chars_ban1185}, 
    {0x630E, 0x630E, gb_chars_ban1186}, 
    {0x6311, 0x6311, gb_chars_ban1187}, 
    {0x6316, 0x6316, gb_chars_ban1188}, 
    {0x631A, 0x631B, gb_chars_ban1189}, 
    {0x631D, 0x6325, gb_chars_ban1190}, 
    {0x6328, 0x6328, gb_chars_ban1191}, 
    {0x632A, 0x632B, gb_chars_ban1192}, 
    {0x632F, 0x632F, gb_chars_ban1193}, 
    {0x6332, 0x6332, gb_chars_ban1194}, 
    {0x6339, 0x633A, gb_chars_ban1195}, 
    {0x633D, 0x633D, gb_chars_ban1196}, 
    {0x6342, 0x6343, gb_chars_ban1197}, 
    {0x6345, 0x6346, gb_chars_ban1198}, 
    {0x6349, 0x6349, gb_chars_ban1199}, 
    {0x634B, 0x6350, gb_chars_ban1200}, 
    {0x6355, 0x6355, gb_chars_ban1201}, 
    {0x635E, 0x635F, gb_chars_ban1202}, 
    {0x6361, 0x6363, gb_chars_ban1203}, 
    {0x6367, 0x6367, gb_chars_ban1204}, 
    {0x6369, 0x6369, gb_chars_ban1205}, 
    {0x636D, 0x636E, gb_chars_ban1206}, 
    {0x6371, 0x6371, gb_chars_ban1207}, 
    {0x6376, 0x6377, gb_chars_ban1208}, 
    {0x637A, 0x637B, gb_chars_ban1209}, 
    {0x6380, 0x6380, gb_chars_ban1210}, 
    {0x6382, 0x6382, gb_chars_ban1211}, 
    {0x6387, 0x638A, gb_chars_ban1212}, 
    {0x638C, 0x638C, gb_chars_ban1213}, 
    {0x638E, 0x6390, gb_chars_ban1214}, 
    {0x6392, 0x6392, gb_chars_ban1215}, 
    {0x6396, 0x6396, gb_chars_ban1216}, 
    {0x6398, 0x6398, gb_chars_ban1217}, 
    {0x63A0, 0x63A0, gb_chars_ban1218}, 
    {0x63A2, 0x63A3, gb_chars_ban1219}, 
    {0x63A5, 0x63A5, gb_chars_ban1220}, 
    {0x63A7, 0x63AA, gb_chars_ban1221}, 
    {0x63AC, 0x63AE, gb_chars_ban1222}, 
    {0x63B0, 0x63B0, gb_chars_ban1223}, 
    {0x63B3, 0x63B4, gb_chars_ban1224}, 
    {0x63B7, 0x63B8, gb_chars_ban1225}, 
    {0x63BA, 0x63BA, gb_chars_ban1226}, 
    {0x63BC, 0x63BC, gb_chars_ban1227}, 
    {0x63BE, 0x63BE, gb_chars_ban1228}, 
    {0x63C4, 0x63C4, gb_chars_ban1229}, 
    {0x63C6, 0x63C6, gb_chars_ban1230}, 
    {0x63C9, 0x63C9, gb_chars_ban1231}, 
    {0x63CD, 0x63D0, gb_chars_ban1232}, 
    {0x63D2, 0x63D2, gb_chars_ban1233}, 
    {0x63D6, 0x63D6, gb_chars_ban1234}, 
    {0x63DE, 0x63DE, gb_chars_ban1235}, 
    {0x63E0, 0x63E1, gb_chars_ban1236}, 
    {0x63E3, 0x63E3, gb_chars_ban1237}, 
    {0x63E9, 0x63EA, gb_chars_ban1238}, 
    {0x63ED, 0x63ED, gb_chars_ban1239}, 
    {0x63F2, 0x63F2, gb_chars_ban1240}, 
    {0x63F4, 0x63F4, gb_chars_ban1241}, 
    {0x63F6, 0x63F6, gb_chars_ban1242}, 
    {0x63F8, 0x63F8, gb_chars_ban1243}, 
    {0x63FD, 0x63FD, gb_chars_ban1244}, 
    {0x63FF, 0x6402, gb_chars_ban1245}, 
    {0x6405, 0x6405, gb_chars_ban1246}, 
    {0x640B, 0x640C, gb_chars_ban1247}, 
    {0x640F, 0x6410, gb_chars_ban1248}, 
    {0x6413, 0x6414, gb_chars_ban1249}, 
    {0x641B, 0x641C, gb_chars_ban1250}, 
    {0x641E, 0x641E, gb_chars_ban1251}, 
    {0x6420, 0x6421, gb_chars_ban1252}, 
    {0x6426, 0x6426, gb_chars_ban1253}, 
    {0x642A, 0x642A, gb_chars_ban1254}, 
    {0x642C, 0x642D, gb_chars_ban1255}, 
    {0x6434, 0x6434, gb_chars_ban1256}, 
    {0x643A, 0x643A, gb_chars_ban1257}, 
    {0x643D, 0x643D, gb_chars_ban1258}, 
    {0x643F, 0x643F, gb_chars_ban1259}, 
    {0x6441, 0x6441, gb_chars_ban1260}, 
    {0x6444, 0x6448, gb_chars_ban1261}, 
    {0x644A, 0x644A, gb_chars_ban1262}, 
    {0x6452, 0x6452, gb_chars_ban1263}, 
    {0x6454, 0x6454, gb_chars_ban1264}, 
    {0x6458, 0x6458, gb_chars_ban1265}, 
    {0x645E, 0x645E, gb_chars_ban1266}, 
    {0x6467, 0x6467, gb_chars_ban1267}, 
    {0x6469, 0x6469, gb_chars_ban1268}, 
    {0x646D, 0x646D, gb_chars_ban1269}, 
    {0x6478, 0x647A, gb_chars_ban1270}, 
    {0x6482, 0x6482, gb_chars_ban1271}, 
    {0x6484, 0x6485, gb_chars_ban1272}, 
    {0x6487, 0x6487, gb_chars_ban1273}, 
    {0x6491, 0x6492, gb_chars_ban1274}, 
    {0x6495, 0x6496, gb_chars_ban1275}, 
    {0x6499, 0x6499, gb_chars_ban1276}, 
    {0x649E, 0x649E, gb_chars_ban1277}, 
    {0x64A4, 0x64A4, gb_chars_ban1278}, 
    {0x64A9, 0x64A9, gb_chars_ban1279}, 
    {0x64AC, 0x64AE, gb_chars_ban1280}, 
    {0x64B0, 0x64B0, gb_chars_ban1281}, 
    {0x64B5, 0x64B5, gb_chars_ban1282}, 
    {0x64B7, 0x64B8, gb_chars_ban1283}, 
    {0x64BA, 0x64BA, gb_chars_ban1284}, 
    {0x64BC, 0x64BC, gb_chars_ban1285}, 
    {0x64C0, 0x64C0, gb_chars_ban1286}, 
    {0x64C2, 0x64C2, gb_chars_ban1287}, 
    {0x64C5, 0x64C5, gb_chars_ban1288}, 
    {0x64CD, 0x64CE, gb_chars_ban1289}, 
    {0x64D0, 0x64D0, gb_chars_ban1290}, 
    {0x64D2, 0x64D2, gb_chars_ban1291}, 
    {0x64D7, 0x64D8, gb_chars_ban1292}, 
    {0x64DE, 0x64DE, gb_chars_ban1293}, 
    {0x64E2, 0x64E2, gb_chars_ban1294}, 
    {0x64E4, 0x64E4, gb_chars_ban1295}, 
    {0x64E6, 0x64E6, gb_chars_ban1296}, 
    {0x6500, 0x6500, gb_chars_ban1297}, 
    {0x6509, 0x6509, gb_chars_ban1298}, 
    {0x6512, 0x6512, gb_chars_ban1299}, 
    {0x6518, 0x6518, gb_chars_ban1300}, 
    {0x6525, 0x6525, gb_chars_ban1301}, 
    {0x652B, 0x652B, gb_chars_ban1302}, 
    {0x652E, 0x652F, gb_chars_ban1303}, 
    {0x6534, 0x6536, gb_chars_ban1304}, 
    {0x6538, 0x6539, gb_chars_ban1305}, 
    {0x653B, 0x653B, gb_chars_ban1306}, 
    {0x653E, 0x653F, gb_chars_ban1307}, 
    {0x6545, 0x6545, gb_chars_ban1308}, 
    {0x6548, 0x6549, gb_chars_ban1309}, 
    {0x654C, 0x654C, gb_chars_ban1310}, 
    {0x654F, 0x654F, gb_chars_ban1311}, 
    {0x6551, 0x6551, gb_chars_ban1312}, 
    {0x6555, 0x6556, gb_chars_ban1313}, 
    {0x6559, 0x6559, gb_chars_ban1314}, 
    {0x655B, 0x655B, gb_chars_ban1315}, 
    {0x655D, 0x655E, gb_chars_ban1316}, 
    {0x6562, 0x6563, gb_chars_ban1317}, 
    {0x6566, 0x6566, gb_chars_ban1318}, 
    {0x656B, 0x656C, gb_chars_ban1319}, 
    {0x6570, 0x6570, gb_chars_ban1320}, 
    {0x6572, 0x6572, gb_chars_ban1321}, 
    {0x6574, 0x6574, gb_chars_ban1322}, 
    {0x6577, 0x6577, gb_chars_ban1323}, 
    {0x6587, 0x6587, gb_chars_ban1324}, 
    {0x658B, 0x658C, gb_chars_ban1325}, 
    {0x6590, 0x6591, gb_chars_ban1326}, 
    {0x6593, 0x6593, gb_chars_ban1327}, 
    {0x6597, 0x6597, gb_chars_ban1328}, 
    {0x6599, 0x6599, gb_chars_ban1329}, 
    {0x659B, 0x659C, gb_chars_ban1330}, 
    {0x659F, 0x659F, gb_chars_ban1331}, 
    {0x65A1, 0x65A1, gb_chars_ban1332}, 
    {0x65A4, 0x65A5, gb_chars_ban1333}, 
    {0x65A7, 0x65A7, gb_chars_ban1334}, 
    {0x65A9, 0x65A9, gb_chars_ban1335}, 
    {0x65AB, 0x65AB, gb_chars_ban1336}, 
    {0x65AD, 0x65AD, gb_chars_ban1337}, 
    {0x65AF, 0x65B0, gb_chars_ban1338}, 
    {0x65B9, 0x65B9, gb_chars_ban1339}, 
    {0x65BC, 0x65BD, gb_chars_ban1340}, 
    {0x65C1, 0x65C1, gb_chars_ban1341}, 
    {0x65C3, 0x65C6, gb_chars_ban1342}, 
    {0x65CB, 0x65CC, gb_chars_ban1343}, 
    {0x65CE, 0x65CF, gb_chars_ban1344}, 
    {0x65D2, 0x65D2, gb_chars_ban1345}, 
    {0x65D6, 0x65D7, gb_chars_ban1346}, 
    {0x65E0, 0x65E0, gb_chars_ban1347}, 
    {0x65E2, 0x65E2, gb_chars_ban1348}, 
    {0x65E5, 0x65E9, gb_chars_ban1349}, 
    {0x65EC, 0x65F1, gb_chars_ban1350}, 
    {0x65F6, 0x65F7, gb_chars_ban1351}, 
    {0x65FA, 0x65FA, gb_chars_ban1352}, 
    {0x6600, 0x6600, gb_chars_ban1353}, 
    {0x6602, 0x6603, gb_chars_ban1354}, 
    {0x6606, 0x6606, gb_chars_ban1355}, 
    {0x660A, 0x660A, gb_chars_ban1356}, 
    {0x660C, 0x660C, gb_chars_ban1357}, 
    {0x660E, 0x660F, gb_chars_ban1358}, 
    {0x6613, 0x6615, gb_chars_ban1359}, 
    {0x6619, 0x6619, gb_chars_ban1360}, 
    {0x661D, 0x661D, gb_chars_ban1361}, 
    {0x661F, 0x6620, gb_chars_ban1362}, 
    {0x6625, 0x6625, gb_chars_ban1363}, 
    {0x6627, 0x6628, gb_chars_ban1364}, 
    {0x662D, 0x662D, gb_chars_ban1365}, 
    {0x662F, 0x662F, gb_chars_ban1366}, 
    {0x6631, 0x6631, gb_chars_ban1367}, 
    {0x6634, 0x6636, gb_chars_ban1368}, 
    {0x663C, 0x663C, gb_chars_ban1369}, 
    {0x663E, 0x663E, gb_chars_ban1370}, 
    {0x6641, 0x6641, gb_chars_ban1371}, 
    {0x6643, 0x6643, gb_chars_ban1372}, 
    {0x664B, 0x664C, gb_chars_ban1373}, 
    {0x664F, 0x664F, gb_chars_ban1374}, 
    {0x6652, 0x6657, gb_chars_ban1375}, 
    {0x665A, 0x665A, gb_chars_ban1376}, 
    {0x665F, 0x665F, gb_chars_ban1377}, 
    {0x6661, 0x6661, gb_chars_ban1378}, 
    {0x6664, 0x6664, gb_chars_ban1379}, 
    {0x6666, 0x6666, gb_chars_ban1380}, 
    {0x6668, 0x6668, gb_chars_ban1381}, 
    {0x666E, 0x6670, gb_chars_ban1382}, 
    {0x6674, 0x6674, gb_chars_ban1383}, 
    {0x6676, 0x6677, gb_chars_ban1384}, 
    {0x667A, 0x667A, gb_chars_ban1385}, 
    {0x667E, 0x667E, gb_chars_ban1386}, 
    {0x6682, 0x6682, gb_chars_ban1387}, 
    {0x6684, 0x6684, gb_chars_ban1388}, 
    {0x6687, 0x6687, gb_chars_ban1389}, 
    {0x668C, 0x668C, gb_chars_ban1390}, 
    {0x6691, 0x6691, gb_chars_ban1391}, 
    {0x6696, 0x6697, gb_chars_ban1392}, 
    {0x669D, 0x669D, gb_chars_ban1393}, 
    {0x66A7, 0x66A8, gb_chars_ban1394}, 
    {0x66AE, 0x66AE, gb_chars_ban1395}, 
    {0x66B4, 0x66B4, gb_chars_ban1396}, 
    {0x66B9, 0x66B9, gb_chars_ban1397}, 
    {0x66BE, 0x66BE, gb_chars_ban1398}, 
    {0x66D9, 0x66D9, gb_chars_ban1399}, 
    {0x66DB, 0x66DD, gb_chars_ban1400}, 
    {0x66E6, 0x66E6, gb_chars_ban1401}, 
    {0x66E9, 0x66E9, gb_chars_ban1402}, 
    {0x66F0, 0x66F0, gb_chars_ban1403}, 
    {0x66F2, 0x66F4, gb_chars_ban1404}, 
    {0x66F7, 0x66F7, gb_chars_ban1405}, 
    {0x66F9, 0x66F9, gb_chars_ban1406}, 
    {0x66FC, 0x66FC, gb_chars_ban1407}, 
    {0x66FE, 0x6700, gb_chars_ban1408}, 
    {0x6708, 0x670B, gb_chars_ban1409}, 
    {0x670D, 0x670D, gb_chars_ban1410}, 
    {0x6710, 0x6710, gb_chars_ban1411}, 
    {0x6714, 0x6715, gb_chars_ban1412}, 
    {0x6717, 0x6717, gb_chars_ban1413}, 
    {0x671B, 0x671B, gb_chars_ban1414}, 
    {0x671D, 0x671D, gb_chars_ban1415}, 
    {0x671F, 0x671F, gb_chars_ban1416}, 
    {0x6726, 0x6726, gb_chars_ban1417}, 
    {0x6728, 0x6728, gb_chars_ban1418}, 
    {0x672A, 0x672D, gb_chars_ban1419}, 
    {0x672F, 0x672F, gb_chars_ban1420}, 
    {0x6731, 0x6731, gb_chars_ban1421}, 
    {0x6734, 0x6735, gb_chars_ban1422}, 
    {0x673A, 0x673A, gb_chars_ban1423}, 
    {0x673D, 0x673D, gb_chars_ban1424}, 
    {0x6740, 0x6740, gb_chars_ban1425}, 
    {0x6742, 0x6743, gb_chars_ban1426}, 
    {0x6746, 0x6746, gb_chars_ban1427}, 
    {0x6748, 0x6749, gb_chars_ban1428}, 
    {0x674C, 0x674C, gb_chars_ban1429}, 
    {0x674E, 0x6751, gb_chars_ban1430}, 
    {0x6753, 0x6753, gb_chars_ban1431}, 
    {0x6756, 0x6756, gb_chars_ban1432}, 
    {0x675C, 0x675C, gb_chars_ban1433}, 
    {0x675E, 0x6761, gb_chars_ban1434}, 
    {0x6765, 0x6765, gb_chars_ban1435}, 
    {0x6768, 0x676A, gb_chars_ban1436}, 
    {0x676D, 0x676D, gb_chars_ban1437}, 
    {0x676F, 0x6770, gb_chars_ban1438}, 
    {0x6772, 0x6773, gb_chars_ban1439}, 
    {0x6775, 0x6775, gb_chars_ban1440}, 
    {0x6777, 0x6777, gb_chars_ban1441}, 
    {0x677C, 0x677C, gb_chars_ban1442}, 
    {0x677E, 0x677F, gb_chars_ban1443}, 
    {0x6781, 0x6781, gb_chars_ban1444}, 
    {0x6784, 0x6784, gb_chars_ban1445}, 
    {0x6787, 0x6787, gb_chars_ban1446}, 
    {0x6789, 0x6789, gb_chars_ban1447}, 
    {0x678B, 0x678B, gb_chars_ban1448}, 
    {0x6790, 0x6790, gb_chars_ban1449}, 
    {0x6795, 0x6795, gb_chars_ban1450}, 
    {0x6797, 0x6798, gb_chars_ban1451}, 
    {0x679A, 0x679A, gb_chars_ban1452}, 
    {0x679C, 0x679E, gb_chars_ban1453}, 
    {0x67A2, 0x67A3, gb_chars_ban1454}, 
    {0x67A5, 0x67A5, gb_chars_ban1455}, 
    {0x67A7, 0x67A8, gb_chars_ban1456}, 
    {0x67AA, 0x67AB, gb_chars_ban1457}, 
    {0x67AD, 0x67AD, gb_chars_ban1458}, 
    {0x67AF, 0x67B0, gb_chars_ban1459}, 
    {0x67B3, 0x67B3, gb_chars_ban1460}, 
    {0x67B5, 0x67B8, gb_chars_ban1461}, 
    {0x67C1, 0x67C1, gb_chars_ban1462}, 
    {0x67C3, 0x67C4, gb_chars_ban1463}, 
    {0x67CF, 0x67D4, gb_chars_ban1464}, 
    {0x67D8, 0x67DA, gb_chars_ban1465}, 
    {0x67DC, 0x67DE, gb_chars_ban1466}, 
    {0x67E0, 0x67E0, gb_chars_ban1467}, 
    {0x67E2, 0x67E2, gb_chars_ban1468}, 
    {0x67E5, 0x67E5, gb_chars_ban1469}, 
    {0x67E9, 0x67E9, gb_chars_ban1470}, 
    {0x67EC, 0x67EC, gb_chars_ban1471}, 
    {0x67EF, 0x67F1, gb_chars_ban1472}, 
    {0x67F3, 0x67F4, gb_chars_ban1473}, 
    {0x67FD, 0x67FD, gb_chars_ban1474}, 
    {0x67FF, 0x6800, gb_chars_ban1475}, 
    {0x6805, 0x6805, gb_chars_ban1476}, 
    {0x6807, 0x680C, gb_chars_ban1477}, 
    {0x680E, 0x680F, gb_chars_ban1478}, 
    {0x6811, 0x6811, gb_chars_ban1479}, 
    {0x6813, 0x6813, gb_chars_ban1480}, 
    {0x6816, 0x6817, gb_chars_ban1481}, 
    {0x681D, 0x681D, gb_chars_ban1482}, 
    {0x6821, 0x6821, gb_chars_ban1483}, 
    {0x6829, 0x682A, gb_chars_ban1484}, 
    {0x6832, 0x6833, gb_chars_ban1485}, 
    {0x6837, 0x6839, gb_chars_ban1486}, 
    {0x683C, 0x683E, gb_chars_ban1487}, 
    {0x6840, 0x6846, gb_chars_ban1488}, 
    {0x6848, 0x684A, gb_chars_ban1489}, 
    {0x684C, 0x684C, gb_chars_ban1490}, 
    {0x684E, 0x684E, gb_chars_ban1491}, 
    {0x6850, 0x6851, gb_chars_ban1492}, 
    {0x6853, 0x6855, gb_chars_ban1493}, 
    {0x6860, 0x6869, gb_chars_ban1494}, 
    {0x686B, 0x686B, gb_chars_ban1495}, 
    {0x6874, 0x6874, gb_chars_ban1496}, 
    {0x6876, 0x6877, gb_chars_ban1497}, 
    {0x6881, 0x6881, gb_chars_ban1498}, 
    {0x6883, 0x6883, gb_chars_ban1499}, 
    {0x6885, 0x6886, gb_chars_ban1500}, 
    {0x688F, 0x688F, gb_chars_ban1501}, 
    {0x6893, 0x6893, gb_chars_ban1502}, 
    {0x6897, 0x6897, gb_chars_ban1503}, 
    {0x68A2, 0x68A2, gb_chars_ban1504}, 
    {0x68A6, 0x68A8, gb_chars_ban1505}, 
    {0x68AD, 0x68AD, gb_chars_ban1506}, 
    {0x68AF, 0x68B0, gb_chars_ban1507}, 
    {0x68B3, 0x68B3, gb_chars_ban1508}, 
    {0x68B5, 0x68B5, gb_chars_ban1509}, 
    {0x68C0, 0x68C0, gb_chars_ban1510}, 
    {0x68C2, 0x68C2, gb_chars_ban1511}, 
    {0x68C9, 0x68C9, gb_chars_ban1512}, 
    {0x68CB, 0x68CB, gb_chars_ban1513}, 
    {0x68CD, 0x68CD, gb_chars_ban1514}, 
    {0x68D2, 0x68D2, gb_chars_ban1515}, 
    {0x68D5, 0x68D5, gb_chars_ban1516}, 
    {0x68D8, 0x68D8, gb_chars_ban1517}, 
    {0x68DA, 0x68DA, gb_chars_ban1518}, 
    {0x68E0, 0x68E0, gb_chars_ban1519}, 
    {0x68E3, 0x68E3, gb_chars_ban1520}, 
    {0x68EE, 0x68EE, gb_chars_ban1521}, 
    {0x68F0, 0x68F1, gb_chars_ban1522}, 
    {0x68F5, 0x68F5, gb_chars_ban1523}, 
    {0x68F9, 0x68FA, gb_chars_ban1524}, 
    {0x68FC, 0x68FC, gb_chars_ban1525}, 
    {0x6901, 0x6901, gb_chars_ban1526}, 
    {0x6905, 0x6905, gb_chars_ban1527}, 
    {0x690B, 0x690B, gb_chars_ban1528}, 
    {0x690D, 0x690E, gb_chars_ban1529}, 
    {0x6910, 0x6910, gb_chars_ban1530}, 
    {0x6912, 0x6912, gb_chars_ban1531}, 
    {0x691F, 0x6920, gb_chars_ban1532}, 
    {0x6924, 0x6924, gb_chars_ban1533}, 
    {0x692D, 0x692D, gb_chars_ban1534}, 
    {0x6930, 0x6930, gb_chars_ban1535}, 
    {0x6934, 0x6934, gb_chars_ban1536}, 
    {0x6939, 0x6939, gb_chars_ban1537}, 
    {0x693D, 0x693D, gb_chars_ban1538}, 
    {0x693F, 0x693F, gb_chars_ban1539}, 
    {0x6942, 0x6942, gb_chars_ban1540}, 
    {0x6954, 0x6954, gb_chars_ban1541}, 
    {0x6957, 0x6957, gb_chars_ban1542}, 
    {0x695A, 0x695A, gb_chars_ban1543}, 
    {0x695D, 0x695E, gb_chars_ban1544}, 
    {0x6960, 0x6960, gb_chars_ban1545}, 
    {0x6963, 0x6963, gb_chars_ban1546}, 
    {0x6966, 0x6966, gb_chars_ban1547}, 
    {0x696B, 0x696B, gb_chars_ban1548}, 
    {0x696E, 0x696E, gb_chars_ban1549}, 
    {0x6971, 0x6971, gb_chars_ban1550}, 
    {0x6977, 0x6979, gb_chars_ban1551}, 
    {0x697C, 0x697C, gb_chars_ban1552}, 
    {0x6980, 0x6980, gb_chars_ban1553}, 
    {0x6982, 0x6982, gb_chars_ban1554}, 
    {0x6984, 0x6984, gb_chars_ban1555}, 
    {0x6986, 0x6989, gb_chars_ban1556}, 
    {0x698D, 0x698D, gb_chars_ban1557}, 
    {0x6994, 0x6995, gb_chars_ban1558}, 
    {0x6998, 0x6998, gb_chars_ban1559}, 
    {0x699B, 0x699C, gb_chars_ban1560}, 
    {0x69A7, 0x69A8, gb_chars_ban1561}, 
    {0x69AB, 0x69AB, gb_chars_ban1562}, 
    {0x69AD, 0x69AD, gb_chars_ban1563}, 
    {0x69B1, 0x69B1, gb_chars_ban1564}, 
    {0x69B4, 0x69B4, gb_chars_ban1565}, 
    {0x69B7, 0x69B7, gb_chars_ban1566}, 
    {0x69BB, 0x69BB, gb_chars_ban1567}, 
    {0x69C1, 0x69C1, gb_chars_ban1568}, 
    {0x69CA, 0x69CA, gb_chars_ban1569}, 
    {0x69CC, 0x69CC, gb_chars_ban1570}, 
    {0x69CE, 0x69CE, gb_chars_ban1571}, 
    {0x69D0, 0x69D0, gb_chars_ban1572}, 
    {0x69D4, 0x69D4, gb_chars_ban1573}, 
    {0x69DB, 0x69DB, gb_chars_ban1574}, 
    {0x69DF, 0x69E0, gb_chars_ban1575}, 
    {0x69ED, 0x69ED, gb_chars_ban1576}, 
    {0x69F2, 0x69F2, gb_chars_ban1577}, 
    {0x69FD, 0x69FD, gb_chars_ban1578}, 
    {0x69FF, 0x69FF, gb_chars_ban1579}, 
    {0x6A0A, 0x6A0A, gb_chars_ban1580}, 
    {0x6A17, 0x6A18, gb_chars_ban1581}, 
    {0x6A1F, 0x6A1F, gb_chars_ban1582}, 
    {0x6A21, 0x6A21, gb_chars_ban1583}, 
    {0x6A28, 0x6A28, gb_chars_ban1584}, 
    {0x6A2A, 0x6A2A, gb_chars_ban1585}, 
    {0x6A2F, 0x6A2F, gb_chars_ban1586}, 
    {0x6A31, 0x6A31, gb_chars_ban1587}, 
    {0x6A35, 0x6A35, gb_chars_ban1588}, 
    {0x6A3D, 0x6A3E, gb_chars_ban1589}, 
    {0x6A44, 0x6A44, gb_chars_ban1590}, 
    {0x6A47, 0x6A47, gb_chars_ban1591}, 
    {0x6A50, 0x6A50, gb_chars_ban1592}, 
    {0x6A58, 0x6A59, gb_chars_ban1593}, 
    {0x6A5B, 0x6A5B, gb_chars_ban1594}, 
    {0x6A61, 0x6A61, gb_chars_ban1595}, 
    {0x6A65, 0x6A65, gb_chars_ban1596}, 
    {0x6A71, 0x6A71, gb_chars_ban1597}, 
    {0x6A79, 0x6A79, gb_chars_ban1598}, 
    {0x6A7C, 0x6A7C, gb_chars_ban1599}, 
    {0x6A80, 0x6A80, gb_chars_ban1600}, 
    {0x6A84, 0x6A84, gb_chars_ban1601}, 
    {0x6A8E, 0x6A8E, gb_chars_ban1602}, 
    {0x6A90, 0x6A91, gb_chars_ban1603}, 
    {0x6A97, 0x6A97, gb_chars_ban1604}, 
    {0x6AA0, 0x6AA0, gb_chars_ban1605}, 
    {0x6AA9, 0x6AA9, gb_chars_ban1606}, 
    {0x6AAB, 0x6AAC, gb_chars_ban1607}, 
    {0x6B20, 0x6B24, gb_chars_ban1608}, 
    {0x6B27, 0x6B27, gb_chars_ban1609}, 
    {0x6B32, 0x6B32, gb_chars_ban1610}, 
    {0x6B37, 0x6B37, gb_chars_ban1611}, 
    {0x6B39, 0x6B3A, gb_chars_ban1612}, 
    {0x6B3E, 0x6B3E, gb_chars_ban1613}, 
    {0x6B43, 0x6B43, gb_chars_ban1614}, 
    {0x6B46, 0x6B47, gb_chars_ban1615}, 
    {0x6B49, 0x6B49, gb_chars_ban1616}, 
    {0x6B4C, 0x6B4C, gb_chars_ban1617}, 
    {0x6B59, 0x6B59, gb_chars_ban1618}, 
    {0x6B62, 0x6B67, gb_chars_ban1619}, 
    {0x6B6A, 0x6B6A, gb_chars_ban1620}, 
    {0x6B79, 0x6B79, gb_chars_ban1621}, 
    {0x6B7B, 0x6B7C, gb_chars_ban1622}, 
    {0x6B81, 0x6B84, gb_chars_ban1623}, 
    {0x6B86, 0x6B87, gb_chars_ban1624}, 
    {0x6B89, 0x6B8B, gb_chars_ban1625}, 
    {0x6B8D, 0x6B8D, gb_chars_ban1626}, 
    {0x6B92, 0x6B93, gb_chars_ban1627}, 
    {0x6B96, 0x6B96, gb_chars_ban1628}, 
    {0x6B9A, 0x6B9B, gb_chars_ban1629}, 
    {0x6BA1, 0x6BA1, gb_chars_ban1630}, 
    {0x6BAA, 0x6BAA, gb_chars_ban1631}, 
    {0x6BB3, 0x6BB5, gb_chars_ban1632}, 
    {0x6BB7, 0x6BB7, gb_chars_ban1633}, 
    {0x6BBF, 0x6BBF, gb_chars_ban1634}, 
    {0x6BC1, 0x6BC2, gb_chars_ban1635}, 
    {0x6BC5, 0x6BC5, gb_chars_ban1636}, 
    {0x6BCB, 0x6BCB, gb_chars_ban1637}, 
    {0x6BCD, 0x6BCD, gb_chars_ban1638}, 
    {0x6BCF, 0x6BCF, gb_chars_ban1639}, 
    {0x6BD2, 0x6BD7, gb_chars_ban1640}, 
    {0x6BD9, 0x6BD9, gb_chars_ban1641}, 
    {0x6BDB, 0x6BDB, gb_chars_ban1642}, 
    {0x6BE1, 0x6BE1, gb_chars_ban1643}, 
    {0x6BEA, 0x6BEB, gb_chars_ban1644}, 
    {0x6BEF, 0x6BEF, gb_chars_ban1645}, 
    {0x6BF3, 0x6BF3, gb_chars_ban1646}, 
    {0x6BF5, 0x6BF5, gb_chars_ban1647}, 
    {0x6BF9, 0x6BF9, gb_chars_ban1648}, 
    {0x6BFD, 0x6BFD, gb_chars_ban1649}, 
    {0x6C05, 0x6C07, gb_chars_ban1650}, 
    {0x6C0D, 0x6C0D, gb_chars_ban1651}, 
    {0x6C0F, 0x6C11, gb_chars_ban1652}, 
    {0x6C13, 0x6C16, gb_chars_ban1653}, 
    {0x6C18, 0x6C1B, gb_chars_ban1654}, 
    {0x6C1F, 0x6C1F, gb_chars_ban1655}, 
    {0x6C21, 0x6C22, gb_chars_ban1656}, 
    {0x6C24, 0x6C24, gb_chars_ban1657}, 
    {0x6C26, 0x6C2A, gb_chars_ban1658}, 
    {0x6C2E, 0x6C30, gb_chars_ban1659}, 
    {0x6C32, 0x6C32, gb_chars_ban1660}, 
    {0x6C34, 0x6C35, gb_chars_ban1661}, 
    {0x6C38, 0x6C38, gb_chars_ban1662}, 
    {0x6C3D, 0x6C3D, gb_chars_ban1663}, 
    {0x6C40, 0x6C42, gb_chars_ban1664}, 
    {0x6C46, 0x6C47, gb_chars_ban1665}, 
    {0x6C49, 0x6C4A, gb_chars_ban1666}, 
    {0x6C50, 0x6C50, gb_chars_ban1667}, 
    {0x6C54, 0x6C55, gb_chars_ban1668}, 
    {0x6C57, 0x6C57, gb_chars_ban1669}, 
    {0x6C5B, 0x6C61, gb_chars_ban1670}, 
    {0x6C64, 0x6C64, gb_chars_ban1671}, 
    {0x6C68, 0x6C6A, gb_chars_ban1672}, 
    {0x6C70, 0x6C70, gb_chars_ban1673}, 
    {0x6C72, 0x6C72, gb_chars_ban1674}, 
    {0x6C74, 0x6C74, gb_chars_ban1675}, 
    {0x6C76, 0x6C76, gb_chars_ban1676}, 
    {0x6C79, 0x6C79, gb_chars_ban1677}, 
    {0x6C7D, 0x6C7E, gb_chars_ban1678}, 
    {0x6C81, 0x6C83, gb_chars_ban1679}, 
    {0x6C85, 0x6C86, gb_chars_ban1680}, 
    {0x6C88, 0x6C89, gb_chars_ban1681}, 
    {0x6C8C, 0x6C8C, gb_chars_ban1682}, 
    {0x6C8F, 0x6C90, gb_chars_ban1683}, 
    {0x6C93, 0x6C94, gb_chars_ban1684}, 
    {0x6C99, 0x6C99, gb_chars_ban1685}, 
    {0x6C9B, 0x6C9B, gb_chars_ban1686}, 
    {0x6C9F, 0x6C9F, gb_chars_ban1687}, 
    {0x6CA1, 0x6CA1, gb_chars_ban1688}, 
    {0x6CA3, 0x6CA7, gb_chars_ban1689}, 
    {0x6CA9, 0x6CAB, gb_chars_ban1690}, 
    {0x6CAD, 0x6CAE, gb_chars_ban1691}, 
    {0x6CB1, 0x6CB3, gb_chars_ban1692}, 
    {0x6CB8, 0x6CB9, gb_chars_ban1693}, 
    {0x6CBB, 0x6CBF, gb_chars_ban1694}, 
    {0x6CC4, 0x6CC5, gb_chars_ban1695}, 
    {0x6CC9, 0x6CCA, gb_chars_ban1696}, 
    {0x6CCC, 0x6CCC, gb_chars_ban1697}, 
    {0x6CD0, 0x6CD0, gb_chars_ban1698}, 
    {0x6CD3, 0x6CD7, gb_chars_ban1699}, 
    {0x6CDB, 0x6CDB, gb_chars_ban1700}, 
    {0x6CDE, 0x6CDE, gb_chars_ban1701}, 
    {0x6CE0, 0x6CE3, gb_chars_ban1702}, 
    {0x6CE5, 0x6CE5, gb_chars_ban1703}, 
    {0x6CE8, 0x6CE8, gb_chars_ban1704}, 
    {0x6CEA, 0x6CEB, gb_chars_ban1705}, 
    {0x6CEE, 0x6CF1, gb_chars_ban1706}, 
    {0x6CF3, 0x6CF3, gb_chars_ban1707}, 
    {0x6CF5, 0x6CF8, gb_chars_ban1708}, 
    {0x6CFA, 0x6CFE, gb_chars_ban1709}, 
    {0x6D01, 0x6D01, gb_chars_ban1710}, 
    {0x6D04, 0x6D04, gb_chars_ban1711}, 
    {0x6D07, 0x6D07, gb_chars_ban1712}, 
    {0x6D0B, 0x6D0C, gb_chars_ban1713}, 
    {0x6D0E, 0x6D0E, gb_chars_ban1714}, 
    {0x6D12, 0x6D12, gb_chars_ban1715}, 
    {0x6D17, 0x6D17, gb_chars_ban1716}, 
    {0x6D19, 0x6D1B, gb_chars_ban1717}, 
    {0x6D1E, 0x6D1E, gb_chars_ban1718}, 
    {0x6D25, 0x6D25, gb_chars_ban1719}, 
    {0x6D27, 0x6D27, gb_chars_ban1720}, 
    {0x6D2A, 0x6D2B, gb_chars_ban1721}, 
    {0x6D2E, 0x6D2E, gb_chars_ban1722}, 
    {0x6D31, 0x6D33, gb_chars_ban1723}, 
    {0x6D35, 0x6D35, gb_chars_ban1724}, 
    {0x6D39, 0x6D39, gb_chars_ban1725}, 
    {0x6D3B, 0x6D3E, gb_chars_ban1726}, 
    {0x6D41, 0x6D41, gb_chars_ban1727}, 
    {0x6D43, 0x6D43, gb_chars_ban1728}, 
    {0x6D45, 0x6D48, gb_chars_ban1729}, 
    {0x6D4A, 0x6D4B, gb_chars_ban1730}, 
    {0x6D4D, 0x6D4F, gb_chars_ban1731}, 
    {0x6D51, 0x6D54, gb_chars_ban1732}, 
    {0x6D59, 0x6D5A, gb_chars_ban1733}, 
    {0x6D5C, 0x6D5C, gb_chars_ban1734}, 
    {0x6D5E, 0x6D5E, gb_chars_ban1735}, 
    {0x6D60, 0x6D60, gb_chars_ban1736}, 
    {0x6D63, 0x6D63, gb_chars_ban1737}, 
    {0x6D66, 0x6D66, gb_chars_ban1738}, 
    {0x6D69, 0x6D6A, gb_chars_ban1739}, 
    {0x6D6E, 0x6D6F, gb_chars_ban1740}, 
    {0x6D74, 0x6D74, gb_chars_ban1741}, 
    {0x6D77, 0x6D78, gb_chars_ban1742}, 
    {0x6D7C, 0x6D7C, gb_chars_ban1743}, 
    {0x6D82, 0x6D82, gb_chars_ban1744}, 
    {0x6D85, 0x6D85, gb_chars_ban1745}, 
    {0x6D88, 0x6D89, gb_chars_ban1746}, 
    {0x6D8C, 0x6D8C, gb_chars_ban1747}, 
    {0x6D8E, 0x6D8E, gb_chars_ban1748}, 
    {0x6D91, 0x6D91, gb_chars_ban1749}, 
    {0x6D93, 0x6D95, gb_chars_ban1750}, 
    {0x6D9B, 0x6D9B, gb_chars_ban1751}, 
    {0x6D9D, 0x6DA1, gb_chars_ban1752}, 
    {0x6DA3, 0x6DA4, gb_chars_ban1753}, 
    {0x6DA6, 0x6DAB, gb_chars_ban1754}, 
    {0x6DAE, 0x6DAF, gb_chars_ban1755}, 
    {0x6DB2, 0x6DB2, gb_chars_ban1756}, 
    {0x6DB5, 0x6DB5, gb_chars_ban1757}, 
    {0x6DB8, 0x6DB8, gb_chars_ban1758}, 
    {0x6DBF, 0x6DC0, gb_chars_ban1759}, 
    {0x6DC4, 0x6DC7, gb_chars_ban1760}, 
    {0x6DCB, 0x6DCC, gb_chars_ban1761}, 
    {0x6DD1, 0x6DD1, gb_chars_ban1762}, 
    {0x6DD6, 0x6DD6, gb_chars_ban1763}, 
    {0x6DD8, 0x6DD9, gb_chars_ban1764}, 
    {0x6DDD, 0x6DDE, gb_chars_ban1765}, 
    {0x6DE0, 0x6DE1, gb_chars_ban1766}, 
    {0x6DE4, 0x6DE4, gb_chars_ban1767}, 
    {0x6DE6, 0x6DE6, gb_chars_ban1768}, 
    {0x6DEB, 0x6DEC, gb_chars_ban1769}, 
    {0x6DEE, 0x6DEE, gb_chars_ban1770}, 
    {0x6DF1, 0x6DF1, gb_chars_ban1771}, 
    {0x6DF3, 0x6DF3, gb_chars_ban1772}, 
    {0x6DF7, 0x6DF7, gb_chars_ban1773}, 
    {0x6DF9, 0x6DF9, gb_chars_ban1774}, 
    {0x6DFB, 0x6DFC, gb_chars_ban1775}, 
    {0x6E05, 0x6E05, gb_chars_ban1776}, 
    {0x6E0A, 0x6E0A, gb_chars_ban1777}, 
    {0x6E0C, 0x6E0E, gb_chars_ban1778}, 
    {0x6E10, 0x6E11, gb_chars_ban1779}, 
    {0x6E14, 0x6E14, gb_chars_ban1780}, 
    {0x6E16, 0x6E17, gb_chars_ban1781}, 
    {0x6E1A, 0x6E1A, gb_chars_ban1782}, 
    {0x6E1D, 0x6E1D, gb_chars_ban1783}, 
    {0x6E20, 0x6E21, gb_chars_ban1784}, 
    {0x6E23, 0x6E25, gb_chars_ban1785}, 
    {0x6E29, 0x6E29, gb_chars_ban1786}, 
    {0x6E2B, 0x6E2B, gb_chars_ban1787}, 
    {0x6E2D, 0x6E2D, gb_chars_ban1788}, 
    {0x6E2F, 0x6E2F, gb_chars_ban1789}, 
    {0x6E32, 0x6E32, gb_chars_ban1790}, 
    {0x6E34, 0x6E34, gb_chars_ban1791}, 
    {0x6E38, 0x6E38, gb_chars_ban1792}, 
    {0x6E3A, 0x6E3A, gb_chars_ban1793}, 
    {0x6E43, 0x6E44, gb_chars_ban1794}, 
    {0x6E4D, 0x6E4E, gb_chars_ban1795}, 
    {0x6E53, 0x6E54, gb_chars_ban1796}, 
    {0x6E56, 0x6E56, gb_chars_ban1797}, 
    {0x6E58, 0x6E58, gb_chars_ban1798}, 
    {0x6E5B, 0x6E5B, gb_chars_ban1799}, 
    {0x6E5F, 0x6E5F, gb_chars_ban1800}, 
    {0x6E6B, 0x6E6B, gb_chars_ban1801}, 
    {0x6E6E, 0x6E6E, gb_chars_ban1802}, 
    {0x6E7E, 0x6E7F, gb_chars_ban1803}, 
    {0x6E83, 0x6E83, gb_chars_ban1804}, 
    {0x6E85, 0x6E86, gb_chars_ban1805}, 
    {0x6E89, 0x6E89, gb_chars_ban1806}, 
    {0x6E8F, 0x6E90, gb_chars_ban1807}, 
    {0x6E98, 0x6E98, gb_chars_ban1808}, 
    {0x6E9C, 0x6E9C, gb_chars_ban1809}, 
    {0x6E9F, 0x6E9F, gb_chars_ban1810}, 
    {0x6EA2, 0x6EA2, gb_chars_ban1811}, 
    {0x6EA5, 0x6EA5, gb_chars_ban1812}, 
    {0x6EA7, 0x6EA7, gb_chars_ban1813}, 
    {0x6EAA, 0x6EAA, gb_chars_ban1814}, 
    {0x6EAF, 0x6EAF, gb_chars_ban1815}, 
    {0x6EB1, 0x6EB2, gb_chars_ban1816}, 
    {0x6EB4, 0x6EB4, gb_chars_ban1817}, 
    {0x6EB6, 0x6EB7, gb_chars_ban1818}, 
    {0x6EBA, 0x6EBB, gb_chars_ban1819}, 
    {0x6EBD, 0x6EBD, gb_chars_ban1820}, 
    {0x6EC1, 0x6EC2, gb_chars_ban1821}, 
    {0x6EC7, 0x6EC7, gb_chars_ban1822}, 
    {0x6ECB, 0x6ECB, gb_chars_ban1823}, 
    {0x6ECF, 0x6ECF, gb_chars_ban1824}, 
    {0x6ED1, 0x6ED1, gb_chars_ban1825}, 
    {0x6ED3, 0x6ED5, gb_chars_ban1826}, 
    {0x6ED7, 0x6ED7, gb_chars_ban1827}, 
    {0x6EDA, 0x6EDA, gb_chars_ban1828}, 
    {0x6EDE, 0x6EE2, gb_chars_ban1829}, 
    {0x6EE4, 0x6EE6, gb_chars_ban1830}, 
    {0x6EE8, 0x6EE9, gb_chars_ban1831}, 
    {0x6EF4, 0x6EF4, gb_chars_ban1832}, 
    {0x6EF9, 0x6EF9, gb_chars_ban1833}, 
    {0x6F02, 0x6F02, gb_chars_ban1834}, 
    {0x6F06, 0x6F06, gb_chars_ban1835}, 
    {0x6F09, 0x6F09, gb_chars_ban1836}, 
    {0x6F0F, 0x6F0F, gb_chars_ban1837}, 
    {0x6F13, 0x6F15, gb_chars_ban1838}, 
    {0x6F20, 0x6F20, gb_chars_ban1839}, 
    {0x6F24, 0x6F24, gb_chars_ban1840}, 
    {0x6F29, 0x6F2B, gb_chars_ban1841}, 
    {0x6F2D, 0x6F2D, gb_chars_ban1842}, 
    {0x6F2F, 0x6F2F, gb_chars_ban1843}, 
    {0x6F31, 0x6F31, gb_chars_ban1844}, 
    {0x6F33, 0x6F33, gb_chars_ban1845}, 
    {0x6F36, 0x6F36, gb_chars_ban1846}, 
    {0x6F3E, 0x6F3E, gb_chars_ban1847}, 
    {0x6F46, 0x6F47, gb_chars_ban1848}, 
    {0x6F4B, 0x6F4B, gb_chars_ban1849}, 
    {0x6F4D, 0x6F4D, gb_chars_ban1850}, 
    {0x6F58, 0x6F58, gb_chars_ban1851}, 
    {0x6F5C, 0x6F5C, gb_chars_ban1852}, 
    {0x6F5E, 0x6F5E, gb_chars_ban1853}, 
    {0x6F62, 0x6F62, gb_chars_ban1854}, 
    {0x6F66, 0x6F66, gb_chars_ban1855}, 
    {0x6F6D, 0x6F6E, gb_chars_ban1856}, 
    {0x6F72, 0x6F72, gb_chars_ban1857}, 
    {0x6F74, 0x6F74, gb_chars_ban1858}, 
    {0x6F78, 0x6F78, gb_chars_ban1859}, 
    {0x6F7A, 0x6F7A, gb_chars_ban1860}, 
    {0x6F7C, 0x6F7C, gb_chars_ban1861}, 
    {0x6F84, 0x6F84, gb_chars_ban1862}, 
    {0x6F88, 0x6F89, gb_chars_ban1863}, 
    {0x6F8C, 0x6F8E, gb_chars_ban1864}, 
    {0x6F9C, 0x6F9C, gb_chars_ban1865}, 
    {0x6FA1, 0x6FA1, gb_chars_ban1866}, 
    {0x6FA7, 0x6FA7, gb_chars_ban1867}, 
    {0x6FB3, 0x6FB3, gb_chars_ban1868}, 
    {0x6FB6, 0x6FB6, gb_chars_ban1869}, 
    {0x6FB9, 0x6FB9, gb_chars_ban1870}, 
    {0x6FC0, 0x6FC0, gb_chars_ban1871}, 
    {0x6FC2, 0x6FC2, gb_chars_ban1872}, 
    {0x6FC9, 0x6FC9, gb_chars_ban1873}, 
    {0x6FD1, 0x6FD2, gb_chars_ban1874}, 
    {0x6FDE, 0x6FDE, gb_chars_ban1875}, 
    {0x6FE0, 0x6FE1, gb_chars_ban1876}, 
    {0x6FEE, 0x6FEF, gb_chars_ban1877}, 
    {0x7011, 0x7011, gb_chars_ban1878}, 
    {0x701A, 0x701B, gb_chars_ban1879}, 
    {0x7023, 0x7023, gb_chars_ban1880}, 
    {0x7035, 0x7035, gb_chars_ban1881}, 
    {0x7039, 0x7039, gb_chars_ban1882}, 
    {0x704C, 0x704C, gb_chars_ban1883}, 
    {0x704F, 0x704F, gb_chars_ban1884}, 
    {0x705E, 0x705E, gb_chars_ban1885}, 
    {0x706B, 0x706D, gb_chars_ban1886}, 
    {0x706F, 0x7070, gb_chars_ban1887}, 
    {0x7075, 0x7076, gb_chars_ban1888}, 
    {0x7078, 0x7078, gb_chars_ban1889}, 
    {0x707C, 0x707C, gb_chars_ban1890}, 
    {0x707E, 0x7080, gb_chars_ban1891}, 
    {0x7085, 0x7085, gb_chars_ban1892}, 
    {0x7089, 0x708A, gb_chars_ban1893}, 
    {0x708E, 0x708E, gb_chars_ban1894}, 
    {0x7092, 0x7092, gb_chars_ban1895}, 
    {0x7094, 0x7096, gb_chars_ban1896}, 
    {0x7099, 0x7099, gb_chars_ban1897}, 
    {0x709C, 0x709D, gb_chars_ban1898}, 
    {0x70AB, 0x70AF, gb_chars_ban1899}, 
    {0x70B1, 0x70B1, gb_chars_ban1900}, 
    {0x70B3, 0x70B3, gb_chars_ban1901}, 
    {0x70B7, 0x70B9, gb_chars_ban1902}, 
    {0x70BB, 0x70BD, gb_chars_ban1903}, 
    {0x70C0, 0x70C3, gb_chars_ban1904}, 
    {0x70C8, 0x70C8, gb_chars_ban1905}, 
    {0x70CA, 0x70CA, gb_chars_ban1906}, 
    {0x70D8, 0x70D9, gb_chars_ban1907}, 
    {0x70DB, 0x70DB, gb_chars_ban1908}, 
    {0x70DF, 0x70DF, gb_chars_ban1909}, 
    {0x70E4, 0x70E4, gb_chars_ban1910}, 
    {0x70E6, 0x70E9, gb_chars_ban1911}, 
    {0x70EB, 0x70ED, gb_chars_ban1912}, 
    {0x70EF, 0x70EF, gb_chars_ban1913}, 
    {0x70F7, 0x70F7, gb_chars_ban1914}, 
    {0x70F9, 0x70F9, gb_chars_ban1915}, 
    {0x70FD, 0x70FD, gb_chars_ban1916}, 
    {0x7109, 0x710A, gb_chars_ban1917}, 
    {0x7110, 0x7110, gb_chars_ban1918}, 
    {0x7113, 0x7113, gb_chars_ban1919}, 
    {0x7115, 0x7116, gb_chars_ban1920}, 
    {0x7118, 0x711A, gb_chars_ban1921}, 
    {0x7126, 0x7126, gb_chars_ban1922}, 
    {0x712F, 0x7131, gb_chars_ban1923}, 
    {0x7136, 0x7136, gb_chars_ban1924}, 
    {0x7145, 0x7145, gb_chars_ban1925}, 
    {0x714A, 0x714A, gb_chars_ban1926}, 
    {0x714C, 0x714C, gb_chars_ban1927}, 
    {0x714E, 0x714E, gb_chars_ban1928}, 
    {0x715C, 0x715C, gb_chars_ban1929}, 
    {0x715E, 0x715E, gb_chars_ban1930}, 
    {0x7164, 0x7164, gb_chars_ban1931}, 
    {0x7166, 0x7168, gb_chars_ban1932}, 
    {0x716E, 0x716E, gb_chars_ban1933}, 
    {0x7172, 0x7173, gb_chars_ban1934}, 
    {0x7178, 0x7178, gb_chars_ban1935}, 
    {0x717A, 0x717A, gb_chars_ban1936}, 
    {0x717D, 0x717D, gb_chars_ban1937}, 
    {0x7184, 0x7184, gb_chars_ban1938}, 
    {0x718A, 0x718A, gb_chars_ban1939}, 
    {0x718F, 0x718F, gb_chars_ban1940}, 
    {0x7194, 0x7194, gb_chars_ban1941}, 
    {0x7198, 0x7199, gb_chars_ban1942}, 
    {0x719F, 0x71A0, gb_chars_ban1943}, 
    {0x71A8, 0x71A8, gb_chars_ban1944}, 
    {0x71AC, 0x71AC, gb_chars_ban1945}, 
    {0x71B3, 0x71B3, gb_chars_ban1946}, 
    {0x71B5, 0x71B5, gb_chars_ban1947}, 
    {0x71B9, 0x71B9, gb_chars_ban1948}, 
    {0x71C3, 0x71C3, gb_chars_ban1949}, 
    {0x71CE, 0x71CE, gb_chars_ban1950}, 
    {0x71D4, 0x71D5, gb_chars_ban1951}, 
    {0x71E0, 0x71E0, gb_chars_ban1952}, 
    {0x71E5, 0x71E5, gb_chars_ban1953}, 
    {0x71E7, 0x71E7, gb_chars_ban1954}, 
    {0x71EE, 0x71EE, gb_chars_ban1955}, 
    {0x71F9, 0x71F9, gb_chars_ban1956}, 
    {0x7206, 0x7206, gb_chars_ban1957}, 
    {0x721D, 0x721D, gb_chars_ban1958}, 
    {0x7228, 0x7228, gb_chars_ban1959}, 
    {0x722A, 0x722A, gb_chars_ban1960}, 
    {0x722C, 0x722C, gb_chars_ban1961}, 
    {0x7230, 0x7231, gb_chars_ban1962}, 
    {0x7235, 0x7239, gb_chars_ban1963}, 
    {0x723B, 0x723B, gb_chars_ban1964}, 
    {0x723D, 0x723D, gb_chars_ban1965}, 
    {0x723F, 0x723F, gb_chars_ban1966}, 
    {0x7247, 0x7248, gb_chars_ban1967}, 
    {0x724C, 0x724D, gb_chars_ban1968}, 
    {0x7252, 0x7252, gb_chars_ban1969}, 
    {0x7256, 0x7256, gb_chars_ban1970}, 
    {0x7259, 0x7259, gb_chars_ban1971}, 
    {0x725B, 0x725B, gb_chars_ban1972}, 
    {0x725D, 0x725D, gb_chars_ban1973}, 
    {0x725F, 0x725F, gb_chars_ban1974}, 
    {0x7261, 0x7262, gb_chars_ban1975}, 
    {0x7266, 0x7267, gb_chars_ban1976}, 
    {0x7269, 0x7269, gb_chars_ban1977}, 
    {0x726E, 0x726F, gb_chars_ban1978}, 
    {0x7272, 0x7272, gb_chars_ban1979}, 
    {0x7275, 0x7275, gb_chars_ban1980}, 
    {0x7279, 0x727A, gb_chars_ban1981}, 
    {0x727E, 0x7281, gb_chars_ban1982}, 
    {0x7284, 0x7284, gb_chars_ban1983}, 
    {0x728A, 0x728B, gb_chars_ban1984}, 
    {0x728D, 0x728D, gb_chars_ban1985}, 
    {0x728F, 0x728F, gb_chars_ban1986}, 
    {0x7292, 0x7292, gb_chars_ban1987}, 
    {0x729F, 0x729F, gb_chars_ban1988}, 
    {0x72AC, 0x72AD, gb_chars_ban1989}, 
    {0x72AF, 0x72B0, gb_chars_ban1990}, 
    {0x72B4, 0x72B4, gb_chars_ban1991}, 
    {0x72B6, 0x72B9, gb_chars_ban1992}, 
    {0x72C1, 0x72C4, gb_chars_ban1993}, 
    {0x72C8, 0x72C8, gb_chars_ban1994}, 
    {0x72CD, 0x72CE, gb_chars_ban1995}, 
    {0x72D0, 0x72D0, gb_chars_ban1996}, 
    {0x72D2, 0x72D2, gb_chars_ban1997}, 
    {0x72D7, 0x72D7, gb_chars_ban1998}, 
    {0x72D9, 0x72D9, gb_chars_ban1999}, 
    {0x72DE, 0x72DE, gb_chars_ban2000}, 
    {0x72E0, 0x72E1, gb_chars_ban2001}, 
    {0x72E8, 0x72E9, gb_chars_ban2002}, 
    {0x72EC, 0x72F4, gb_chars_ban2003}, 
    {0x72F7, 0x72F8, gb_chars_ban2004}, 
    {0x72FA, 0x72FC, gb_chars_ban2005}, 
    {0x7301, 0x7301, gb_chars_ban2006}, 
    {0x7303, 0x7303, gb_chars_ban2007}, 
    {0x730A, 0x730A, gb_chars_ban2008}, 
    {0x730E, 0x730E, gb_chars_ban2009}, 
    {0x7313, 0x7313, gb_chars_ban2010}, 
    {0x7315, 0x7317, gb_chars_ban2011}, 
    {0x731B, 0x731E, gb_chars_ban2012}, 
    {0x7321, 0x7322, gb_chars_ban2013}, 
    {0x7325, 0x7325, gb_chars_ban2014}, 
    {0x7329, 0x732C, gb_chars_ban2015}, 
    {0x732E, 0x732E, gb_chars_ban2016}, 
    {0x7331, 0x7331, gb_chars_ban2017}, 
    {0x7334, 0x7334, gb_chars_ban2018}, 
    {0x7337, 0x7339, gb_chars_ban2019}, 
    {0x733E, 0x733F, gb_chars_ban2020}, 
    {0x734D, 0x734D, gb_chars_ban2021}, 
    {0x7350, 0x7350, gb_chars_ban2022}, 
    {0x7352, 0x7352, gb_chars_ban2023}, 
    {0x7357, 0x7357, gb_chars_ban2024}, 
    {0x7360, 0x7360, gb_chars_ban2025}, 
    {0x736C, 0x736D, gb_chars_ban2026}, 
    {0x736F, 0x736F, gb_chars_ban2027}, 
    {0x737E, 0x737E, gb_chars_ban2028}, 
    {0x7384, 0x7384, gb_chars_ban2029}, 
    {0x7387, 0x7387, gb_chars_ban2030}, 
    {0x7389, 0x7389, gb_chars_ban2031}, 
    {0x738B, 0x738B, gb_chars_ban2032}, 
    {0x738E, 0x738E, gb_chars_ban2033}, 
    {0x7391, 0x7391, gb_chars_ban2034}, 
    {0x7396, 0x7396, gb_chars_ban2035}, 
    {0x739B, 0x739B, gb_chars_ban2036}, 
    {0x739F, 0x739F, gb_chars_ban2037}, 
    {0x73A2, 0x73A2, gb_chars_ban2038}, 
    {0x73A9, 0x73A9, gb_chars_ban2039}, 
    {0x73AB, 0x73AB, gb_chars_ban2040}, 
    {0x73AE, 0x73B0, gb_chars_ban2041}, 
    {0x73B2, 0x73B3, gb_chars_ban2042}, 
    {0x73B7, 0x73B7, gb_chars_ban2043}, 
    {0x73BA, 0x73BB, gb_chars_ban2044}, 
    {0x73C0, 0x73C0, gb_chars_ban2045}, 
    {0x73C2, 0x73C2, gb_chars_ban2046}, 
    {0x73C8, 0x73CA, gb_chars_ban2047}, 
    {0x73CD, 0x73CD, gb_chars_ban2048}, 
    {0x73CF, 0x73D1, gb_chars_ban2049}, 
    {0x73D9, 0x73D9, gb_chars_ban2050}, 
    {0x73DE, 0x73DE, gb_chars_ban2051}, 
    {0x73E0, 0x73E0, gb_chars_ban2052}, 
    {0x73E5, 0x73E5, gb_chars_ban2053}, 
    {0x73E7, 0x73E7, gb_chars_ban2054}, 
    {0x73E9, 0x73E9, gb_chars_ban2055}, 
    {0x73ED, 0x73ED, gb_chars_ban2056}, 
    {0x73F2, 0x73F2, gb_chars_ban2057}, 
    {0x7403, 0x7403, gb_chars_ban2058}, 
    {0x7405, 0x7406, gb_chars_ban2059}, 
    {0x7409, 0x740A, gb_chars_ban2060}, 
    {0x740F, 0x7410, gb_chars_ban2061}, 
    {0x741A, 0x741B, gb_chars_ban2062}, 
    {0x7422, 0x7422, gb_chars_ban2063}, 
    {0x7425, 0x7426, gb_chars_ban2064}, 
    {0x7428, 0x7428, gb_chars_ban2065}, 
    {0x742A, 0x742A, gb_chars_ban2066}, 
    {0x742C, 0x742C, gb_chars_ban2067}, 
    {0x742E, 0x742E, gb_chars_ban2068}, 
    {0x7430, 0x7430, gb_chars_ban2069}, 
    {0x7433, 0x7436, gb_chars_ban2070}, 
    {0x743C, 0x743C, gb_chars_ban2071}, 
    {0x7441, 0x7441, gb_chars_ban2072}, 
    {0x7455, 0x7455, gb_chars_ban2073}, 
    {0x7457, 0x7457, gb_chars_ban2074}, 
    {0x7459, 0x745C, gb_chars_ban2075}, 
    {0x745E, 0x745F, gb_chars_ban2076}, 
    {0x746D, 0x746D, gb_chars_ban2077}, 
    {0x7470, 0x7470, gb_chars_ban2078}, 
    {0x7476, 0x7477, gb_chars_ban2079}, 
    {0x747E, 0x747E, gb_chars_ban2080}, 
    {0x7480, 0x7481, gb_chars_ban2081}, 
    {0x7483, 0x7483, gb_chars_ban2082}, 
    {0x7487, 0x7487, gb_chars_ban2083}, 
    {0x748B, 0x748B, gb_chars_ban2084}, 
    {0x748E, 0x748E, gb_chars_ban2085}, 
    {0x7490, 0x7490, gb_chars_ban2086}, 
    {0x749C, 0x749C, gb_chars_ban2087}, 
    {0x749E, 0x749E, gb_chars_ban2088}, 
    {0x74A7, 0x74A9, gb_chars_ban2089}, 
    {0x74BA, 0x74BA, gb_chars_ban2090}, 
    {0x74D2, 0x74D2, gb_chars_ban2091}, 
    {0x74DC, 0x74DC, gb_chars_ban2092}, 
    {0x74DE, 0x74DE, gb_chars_ban2093}, 
    {0x74E0, 0x74E0, gb_chars_ban2094}, 
    {0x74E2, 0x74E4, gb_chars_ban2095}, 
    {0x74E6, 0x74E6, gb_chars_ban2096}, 
    {0x74EE, 0x74EF, gb_chars_ban2097}, 
    {0x74F4, 0x74F4, gb_chars_ban2098}, 
    {0x74F6, 0x74F7, gb_chars_ban2099}, 
    {0x74FF, 0x74FF, gb_chars_ban2100}, 
    {0x7504, 0x7504, gb_chars_ban2101}, 
    {0x750D, 0x750D, gb_chars_ban2102}, 
    {0x750F, 0x750F, gb_chars_ban2103}, 
    {0x7511, 0x7511, gb_chars_ban2104}, 
    {0x7513, 0x7513, gb_chars_ban2105}, 
    {0x7518, 0x751A, gb_chars_ban2106}, 
    {0x751C, 0x751C, gb_chars_ban2107}, 
    {0x751F, 0x751F, gb_chars_ban2108}, 
    {0x7525, 0x7525, gb_chars_ban2109}, 
    {0x7528, 0x7529, gb_chars_ban2110}, 
    {0x752B, 0x752D, gb_chars_ban2111}, 
    {0x752F, 0x7533, gb_chars_ban2112}, 
    {0x7535, 0x7535, gb_chars_ban2113}, 
    {0x7537, 0x7538, gb_chars_ban2114}, 
    {0x753A, 0x753B, gb_chars_ban2115}, 
    {0x753E, 0x753E, gb_chars_ban2116}, 
    {0x7540, 0x7540, gb_chars_ban2117}, 
    {0x7545, 0x7545, gb_chars_ban2118}, 
    {0x7548, 0x7548, gb_chars_ban2119}, 
    {0x754B, 0x754C, gb_chars_ban2120}, 
    {0x754E, 0x754F, gb_chars_ban2121}, 
    {0x7554, 0x7554, gb_chars_ban2122}, 
    {0x7559, 0x755C, gb_chars_ban2123}, 
    {0x7565, 0x7566, gb_chars_ban2124}, 
    {0x756A, 0x756A, gb_chars_ban2125}, 
    {0x7572, 0x7572, gb_chars_ban2126}, 
    {0x7574, 0x7574, gb_chars_ban2127}, 
    {0x7578, 0x7579, gb_chars_ban2128}, 
    {0x757F, 0x757F, gb_chars_ban2129}, 
    {0x7583, 0x7583, gb_chars_ban2130}, 
    {0x7586, 0x7586, gb_chars_ban2131}, 
    {0x758B, 0x758B, gb_chars_ban2132}, 
    {0x758F, 0x758F, gb_chars_ban2133}, 
    {0x7591, 0x7592, gb_chars_ban2134}, 
    {0x7594, 0x7594, gb_chars_ban2135}, 
    {0x7596, 0x7597, gb_chars_ban2136}, 
    {0x7599, 0x759A, gb_chars_ban2137}, 
    {0x759D, 0x759D, gb_chars_ban2138}, 
    {0x759F, 0x75A1, gb_chars_ban2139}, 
    {0x75A3, 0x75A5, gb_chars_ban2140}, 
    {0x75AB, 0x75AC, gb_chars_ban2141}, 
    {0x75AE, 0x75B5, gb_chars_ban2142}, 
    {0x75B8, 0x75B9, gb_chars_ban2143}, 
    {0x75BC, 0x75BE, gb_chars_ban2144}, 
    {0x75C2, 0x75C5, gb_chars_ban2145}, 
    {0x75C7, 0x75CA, gb_chars_ban2146}, 
    {0x75CD, 0x75CD, gb_chars_ban2147}, 
    {0x75D2, 0x75D2, gb_chars_ban2148}, 
    {0x75D4, 0x75D6, gb_chars_ban2149}, 
    {0x75D8, 0x75D8, gb_chars_ban2150}, 
    {0x75DB, 0x75DB, gb_chars_ban2151}, 
    {0x75DE, 0x75DE, gb_chars_ban2152}, 
    {0x75E2, 0x75E4, gb_chars_ban2153}, 
    {0x75E6, 0x75E8, gb_chars_ban2154}, 
    {0x75EA, 0x75EB, gb_chars_ban2155}, 
    {0x75F0, 0x75F1, gb_chars_ban2156}, 
    {0x75F4, 0x75F4, gb_chars_ban2157}, 
    {0x75F9, 0x75F9, gb_chars_ban2158}, 
    {0x75FC, 0x75FC, gb_chars_ban2159}, 
    {0x75FF, 0x7601, gb_chars_ban2160}, 
    {0x7603, 0x7603, gb_chars_ban2161}, 
    {0x7605, 0x7605, gb_chars_ban2162}, 
    {0x760A, 0x760A, gb_chars_ban2163}, 
    {0x760C, 0x760C, gb_chars_ban2164}, 
    {0x7610, 0x7610, gb_chars_ban2165}, 
    {0x7615, 0x7615, gb_chars_ban2166}, 
    {0x7617, 0x7619, gb_chars_ban2167}, 
    {0x761B, 0x761B, gb_chars_ban2168}, 
    {0x761F, 0x7620, gb_chars_ban2169}, 
    {0x7622, 0x7622, gb_chars_ban2170}, 
    {0x7624, 0x7626, gb_chars_ban2171}, 
    {0x7629, 0x762B, gb_chars_ban2172}, 
    {0x762D, 0x762D, gb_chars_ban2173}, 
    {0x7630, 0x7630, gb_chars_ban2174}, 
    {0x7633, 0x7635, gb_chars_ban2175}, 
    {0x7638, 0x7638, gb_chars_ban2176}, 
    {0x763C, 0x763C, gb_chars_ban2177}, 
    {0x763E, 0x7640, gb_chars_ban2178}, 
    {0x7643, 0x7643, gb_chars_ban2179}, 
    {0x764C, 0x764D, gb_chars_ban2180}, 
    {0x7654, 0x7654, gb_chars_ban2181}, 
    {0x7656, 0x7656, gb_chars_ban2182}, 
    {0x765C, 0x765C, gb_chars_ban2183}, 
    {0x765E, 0x765E, gb_chars_ban2184}, 
    {0x7663, 0x7663, gb_chars_ban2185}, 
    {0x766B, 0x766B, gb_chars_ban2186}, 
    {0x766F, 0x766F, gb_chars_ban2187}, 
    {0x7678, 0x7678, gb_chars_ban2188}, 
    {0x767B, 0x767B, gb_chars_ban2189}, 
    {0x767D, 0x767E, gb_chars_ban2190}, 
    {0x7682, 0x7682, gb_chars_ban2191}, 
    {0x7684, 0x7684, gb_chars_ban2192}, 
    {0x7686, 0x7688, gb_chars_ban2193}, 
    {0x768B, 0x768B, gb_chars_ban2194}, 
    {0x768E, 0x768E, gb_chars_ban2195}, 
    {0x7691, 0x7691, gb_chars_ban2196}, 
    {0x7693, 0x7693, gb_chars_ban2197}, 
    {0x7696, 0x7696, gb_chars_ban2198}, 
    {0x7699, 0x7699, gb_chars_ban2199}, 
    {0x76A4, 0x76A4, gb_chars_ban2200}, 
    {0x76AE, 0x76AE, gb_chars_ban2201}, 
    {0x76B1, 0x76B2, gb_chars_ban2202}, 
    {0x76B4, 0x76B4, gb_chars_ban2203}, 
    {0x76BF, 0x76BF, gb_chars_ban2204}, 
    {0x76C2, 0x76C2, gb_chars_ban2205}, 
    {0x76C5, 0x76C6, gb_chars_ban2206}, 
    {0x76C8, 0x76C8, gb_chars_ban2207}, 
    {0x76CA, 0x76CA, gb_chars_ban2208}, 
    {0x76CD, 0x76D2, gb_chars_ban2209}, 
    {0x76D4, 0x76D4, gb_chars_ban2210}, 
    {0x76D6, 0x76D8, gb_chars_ban2211}, 
    {0x76DB, 0x76DB, gb_chars_ban2212}, 
    {0x76DF, 0x76DF, gb_chars_ban2213}, 
    {0x76E5, 0x76E5, gb_chars_ban2214}, 
    {0x76EE, 0x76EF, gb_chars_ban2215}, 
    {0x76F1, 0x76F2, gb_chars_ban2216}, 
    {0x76F4, 0x76F4, gb_chars_ban2217}, 
    {0x76F8, 0x76F9, gb_chars_ban2218}, 
    {0x76FC, 0x76FC, gb_chars_ban2219}, 
    {0x76FE, 0x76FE, gb_chars_ban2220}, 
    {0x7701, 0x7701, gb_chars_ban2221}, 
    {0x7704, 0x7704, gb_chars_ban2222}, 
    {0x7707, 0x7709, gb_chars_ban2223}, 
    {0x770B, 0x770B, gb_chars_ban2224}, 
    {0x770D, 0x770D, gb_chars_ban2225}, 
    {0x7719, 0x771A, gb_chars_ban2226}, 
    {0x771F, 0x7720, gb_chars_ban2227}, 
    {0x7722, 0x7722, gb_chars_ban2228}, 
    {0x7726, 0x7726, gb_chars_ban2229}, 
    {0x7728, 0x7729, gb_chars_ban2230}, 
    {0x772D, 0x772D, gb_chars_ban2231}, 
    {0x772F, 0x772F, gb_chars_ban2232}, 
    {0x7735, 0x7738, gb_chars_ban2233}, 
    {0x773A, 0x773A, gb_chars_ban2234}, 
    {0x773C, 0x773C, gb_chars_ban2235}, 
    {0x7740, 0x7741, gb_chars_ban2236}, 
    {0x7743, 0x7743, gb_chars_ban2237}, 
    {0x7747, 0x7747, gb_chars_ban2238}, 
    {0x7750, 0x7751, gb_chars_ban2239}, 
    {0x775A, 0x775B, gb_chars_ban2240}, 
    {0x7761, 0x7763, gb_chars_ban2241}, 
    {0x7765, 0x7766, gb_chars_ban2242}, 
    {0x7768, 0x7768, gb_chars_ban2243}, 
    {0x776B, 0x776C, gb_chars_ban2244}, 
    {0x7779, 0x7779, gb_chars_ban2245}, 
    {0x777D, 0x7780, gb_chars_ban2246}, 
    {0x7784, 0x7785, gb_chars_ban2247}, 
    {0x778C, 0x778E, gb_chars_ban2248}, 
    {0x7791, 0x7792, gb_chars_ban2249}, 
    {0x779F, 0x77A0, gb_chars_ban2250}, 
    {0x77A2, 0x77A2, gb_chars_ban2251}, 
    {0x77A5, 0x77A5, gb_chars_ban2252}, 
    {0x77A7, 0x77A7, gb_chars_ban2253}, 
    {0x77A9, 0x77AA, gb_chars_ban2254}, 
    {0x77AC, 0x77AC, gb_chars_ban2255}, 
    {0x77B0, 0x77B0, gb_chars_ban2256}, 
    {0x77B3, 0x77B3, gb_chars_ban2257}, 
    {0x77B5, 0x77B5, gb_chars_ban2258}, 
    {0x77BB, 0x77BB, gb_chars_ban2259}, 
    {0x77BD, 0x77BD, gb_chars_ban2260}, 
    {0x77BF, 0x77BF, gb_chars_ban2261}, 
    {0x77CD, 0x77CD, gb_chars_ban2262}, 
    {0x77D7, 0x77D7, gb_chars_ban2263}, 
    {0x77DB, 0x77DC, gb_chars_ban2264}, 
    {0x77E2, 0x77E3, gb_chars_ban2265}, 
    {0x77E5, 0x77E5, gb_chars_ban2266}, 
    {0x77E7, 0x77E7, gb_chars_ban2267}, 
    {0x77E9, 0x77E9, gb_chars_ban2268}, 
    {0x77EB, 0x77EE, gb_chars_ban2269}, 
    {0x77F3, 0x77F3, gb_chars_ban2270}, 
    {0x77F6, 0x77F6, gb_chars_ban2271}, 
    {0x77F8, 0x77F8, gb_chars_ban2272}, 
    {0x77FD, 0x7802, gb_chars_ban2273}, 
    {0x7809, 0x7809, gb_chars_ban2274}, 
    {0x780C, 0x780D, gb_chars_ban2275}, 
    {0x7811, 0x7812, gb_chars_ban2276}, 
    {0x7814, 0x7814, gb_chars_ban2277}, 
    {0x7816, 0x7818, gb_chars_ban2278}, 
    {0x781A, 0x781A, gb_chars_ban2279}, 
    {0x781C, 0x781D, gb_chars_ban2280}, 
    {0x781F, 0x781F, gb_chars_ban2281}, 
    {0x7823, 0x7823, gb_chars_ban2282}, 
    {0x7825, 0x7827, gb_chars_ban2283}, 
    {0x7829, 0x7829, gb_chars_ban2284}, 
    {0x782C, 0x782D, gb_chars_ban2285}, 
    {0x7830, 0x7830, gb_chars_ban2286}, 
    {0x7834, 0x7834, gb_chars_ban2287}, 
    {0x7837, 0x783C, gb_chars_ban2288}, 
    {0x783E, 0x783E, gb_chars_ban2289}, 
    {0x7840, 0x7840, gb_chars_ban2290}, 
    {0x7845, 0x7845, gb_chars_ban2291}, 
    {0x7847, 0x7847, gb_chars_ban2292}, 
    {0x784C, 0x784C, gb_chars_ban2293}, 
    {0x784E, 0x784E, gb_chars_ban2294}, 
    {0x7850, 0x7850, gb_chars_ban2295}, 
    {0x7852, 0x7852, gb_chars_ban2296}, 
    {0x7855, 0x7857, gb_chars_ban2297}, 
    {0x785D, 0x785D, gb_chars_ban2298}, 
    {0x786A, 0x786E, gb_chars_ban2299}, 
    {0x7877, 0x7877, gb_chars_ban2300}, 
    {0x787C, 0x787C, gb_chars_ban2301}, 
    {0x7887, 0x7887, gb_chars_ban2302}, 
    {0x7889, 0x7889, gb_chars_ban2303}, 
    {0x788C, 0x788E, gb_chars_ban2304}, 
    {0x7891, 0x7891, gb_chars_ban2305}, 
    {0x7893, 0x7893, gb_chars_ban2306}, 
    {0x7897, 0x7898, gb_chars_ban2307}, 
    {0x789A, 0x789C, gb_chars_ban2308}, 
    {0x789F, 0x789F, gb_chars_ban2309}, 
    {0x78A1, 0x78A1, gb_chars_ban2310}, 
    {0x78A3, 0x78A3, gb_chars_ban2311}, 
    {0x78A5, 0x78A5, gb_chars_ban2312}, 
    {0x78A7, 0x78A7, gb_chars_ban2313}, 
    {0x78B0, 0x78B4, gb_chars_ban2314}, 
    {0x78B9, 0x78B9, gb_chars_ban2315}, 
    {0x78BE, 0x78BE, gb_chars_ban2316}, 
    {0x78C1, 0x78C1, gb_chars_ban2317}, 
    {0x78C5, 0x78C5, gb_chars_ban2318}, 
    {0x78C9, 0x78CB, gb_chars_ban2319}, 
    {0x78D0, 0x78D0, gb_chars_ban2320}, 
    {0x78D4, 0x78D5, gb_chars_ban2321}, 
    {0x78D9, 0x78D9, gb_chars_ban2322}, 
    {0x78E8, 0x78E8, gb_chars_ban2323}, 
    {0x78EC, 0x78EC, gb_chars_ban2324}, 
    {0x78F2, 0x78F2, gb_chars_ban2325}, 
    {0x78F4, 0x78F4, gb_chars_ban2326}, 
    {0x78F7, 0x78F7, gb_chars_ban2327}, 
    {0x78FA, 0x78FA, gb_chars_ban2328}, 
    {0x7901, 0x7901, gb_chars_ban2329}, 
    {0x7905, 0x7905, gb_chars_ban2330}, 
    {0x7913, 0x7913, gb_chars_ban2331}, 
    {0x791E, 0x791E, gb_chars_ban2332}, 
    {0x7924, 0x7924, gb_chars_ban2333}, 
    {0x7934, 0x7934, gb_chars_ban2334}, 
    {0x793A, 0x793C, gb_chars_ban2335}, 
    {0x793E, 0x793E, gb_chars_ban2336}, 
    {0x7940, 0x7941, gb_chars_ban2337}, 
    {0x7946, 0x7946, gb_chars_ban2338}, 
    {0x7948, 0x7949, gb_chars_ban2339}, 
    {0x7953, 0x7953, gb_chars_ban2340}, 
    {0x7956, 0x7957, gb_chars_ban2341}, 
    {0x795A, 0x7960, gb_chars_ban2342}, 
    {0x7962, 0x7962, gb_chars_ban2343}, 
    {0x7965, 0x7965, gb_chars_ban2344}, 
    {0x7967, 0x7968, gb_chars_ban2345}, 
    {0x796D, 0x796D, gb_chars_ban2346}, 
    {0x796F, 0x796F, gb_chars_ban2347}, 
    {0x7977, 0x7978, gb_chars_ban2348}, 
    {0x797A, 0x797A, gb_chars_ban2349}, 
    {0x7980, 0x7981, gb_chars_ban2350}, 
    {0x7984, 0x7985, gb_chars_ban2351}, 
    {0x798A, 0x798A, gb_chars_ban2352}, 
    {0x798F, 0x798F, gb_chars_ban2353}, 
    {0x799A, 0x799A, gb_chars_ban2354}, 
    {0x79A7, 0x79A7, gb_chars_ban2355}, 
    {0x79B3, 0x79B3, gb_chars_ban2356}, 
    {0x79B9, 0x79BB, gb_chars_ban2357}, 
    {0x79BD, 0x79BE, gb_chars_ban2358}, 
    {0x79C0, 0x79C1, gb_chars_ban2359}, 
    {0x79C3, 0x79C3, gb_chars_ban2360}, 
    {0x79C6, 0x79C6, gb_chars_ban2361}, 
    {0x79C9, 0x79C9, gb_chars_ban2362}, 
    {0x79CB, 0x79CB, gb_chars_ban2363}, 
    {0x79CD, 0x79CD, gb_chars_ban2364}, 
    {0x79D1, 0x79D2, gb_chars_ban2365}, 
    {0x79D5, 0x79D5, gb_chars_ban2366}, 
    {0x79D8, 0x79D8, gb_chars_ban2367}, 
    {0x79DF, 0x79DF, gb_chars_ban2368}, 
    {0x79E3, 0x79E4, gb_chars_ban2369}, 
    {0x79E6, 0x79E7, gb_chars_ban2370}, 
    {0x79E9, 0x79E9, gb_chars_ban2371}, 
    {0x79EB, 0x79EB, gb_chars_ban2372}, 
    {0x79ED, 0x79ED, gb_chars_ban2373}, 
    {0x79EF, 0x79F0, gb_chars_ban2374}, 
    {0x79F8, 0x79F8, gb_chars_ban2375}, 
    {0x79FB, 0x79FB, gb_chars_ban2376}, 
    {0x79FD, 0x79FD, gb_chars_ban2377}, 
    {0x7A00, 0x7A00, gb_chars_ban2378}, 
    {0x7A02, 0x7A03, gb_chars_ban2379}, 
    {0x7A06, 0x7A06, gb_chars_ban2380}, 
    {0x7A0B, 0x7A0B, gb_chars_ban2381}, 
    {0x7A0D, 0x7A0E, gb_chars_ban2382}, 
    {0x7A14, 0x7A14, gb_chars_ban2383}, 
    {0x7A17, 0x7A17, gb_chars_ban2384}, 
    {0x7A1A, 0x7A1A, gb_chars_ban2385}, 
    {0x7A1E, 0x7A1E, gb_chars_ban2386}, 
    {0x7A20, 0x7A20, gb_chars_ban2387}, 
    {0x7A23, 0x7A23, gb_chars_ban2388}, 
    {0x7A33, 0x7A33, gb_chars_ban2389}, 
    {0x7A37, 0x7A37, gb_chars_ban2390}, 
    {0x7A39, 0x7A39, gb_chars_ban2391}, 
    {0x7A3B, 0x7A3D, gb_chars_ban2392}, 
    {0x7A3F, 0x7A3F, gb_chars_ban2393}, 
    {0x7A46, 0x7A46, gb_chars_ban2394}, 
    {0x7A51, 0x7A51, gb_chars_ban2395}, 
    {0x7A57, 0x7A57, gb_chars_ban2396}, 
    {0x7A70, 0x7A70, gb_chars_ban2397}, 
    {0x7A74, 0x7A74, gb_chars_ban2398}, 
    {0x7A76, 0x7A7A, gb_chars_ban2399}, 
    {0x7A7F, 0x7A81, gb_chars_ban2400}, 
    {0x7A83, 0x7A84, gb_chars_ban2401}, 
    {0x7A86, 0x7A86, gb_chars_ban2402}, 
    {0x7A88, 0x7A88, gb_chars_ban2403}, 
    {0x7A8D, 0x7A8D, gb_chars_ban2404}, 
    {0x7A91, 0x7A92, gb_chars_ban2405}, 
    {0x7A95, 0x7A98, gb_chars_ban2406}, 
    {0x7A9C, 0x7A9D, gb_chars_ban2407}, 
    {0x7A9F, 0x7AA0, gb_chars_ban2408}, 
    {0x7AA5, 0x7AA6, gb_chars_ban2409}, 
    {0x7AA8, 0x7AA8, gb_chars_ban2410}, 
    {0x7AAC, 0x7AAD, gb_chars_ban2411}, 
    {0x7AB3, 0x7AB3, gb_chars_ban2412}, 
    {0x7ABF, 0x7ABF, gb_chars_ban2413}, 
    {0x7ACB, 0x7ACB, gb_chars_ban2414}, 
    {0x7AD6, 0x7AD6, gb_chars_ban2415}, 
    {0x7AD9, 0x7AD9, gb_chars_ban2416}, 
    {0x7ADE, 0x7AE0, gb_chars_ban2417}, 
    {0x7AE3, 0x7AE3, gb_chars_ban2418}, 
    {0x7AE5, 0x7AE6, gb_chars_ban2419}, 
    {0x7AED, 0x7AED, gb_chars_ban2420}, 
    {0x7AEF, 0x7AEF, gb_chars_ban2421}, 
    {0x7AF9, 0x7AFA, gb_chars_ban2422}, 
    {0x7AFD, 0x7AFD, gb_chars_ban2423}, 
    {0x7AFF, 0x7AFF, gb_chars_ban2424}, 
    {0x7B03, 0x7B04, gb_chars_ban2425}, 
    {0x7B06, 0x7B06, gb_chars_ban2426}, 
    {0x7B08, 0x7B08, gb_chars_ban2427}, 
    {0x7B0A, 0x7B0B, gb_chars_ban2428}, 
    {0x7B0F, 0x7B0F, gb_chars_ban2429}, 
    {0x7B11, 0x7B11, gb_chars_ban2430}, 
    {0x7B14, 0x7B15, gb_chars_ban2431}, 
    {0x7B19, 0x7B19, gb_chars_ban2432}, 
    {0x7B1B, 0x7B1B, gb_chars_ban2433}, 
    {0x7B1E, 0x7B1E, gb_chars_ban2434}, 
    {0x7B20, 0x7B20, gb_chars_ban2435}, 
    {0x7B24, 0x7B26, gb_chars_ban2436}, 
    {0x7B28, 0x7B28, gb_chars_ban2437}, 
    {0x7B2A, 0x7B2C, gb_chars_ban2438}, 
    {0x7B2E, 0x7B2E, gb_chars_ban2439}, 
    {0x7B31, 0x7B31, gb_chars_ban2440}, 
    {0x7B33, 0x7B33, gb_chars_ban2441}, 
    {0x7B38, 0x7B38, gb_chars_ban2442}, 
    {0x7B3A, 0x7B3A, gb_chars_ban2443}, 
    {0x7B3C, 0x7B3C, gb_chars_ban2444}, 
    {0x7B3E, 0x7B3E, gb_chars_ban2445}, 
    {0x7B45, 0x7B45, gb_chars_ban2446}, 
    {0x7B47, 0x7B47, gb_chars_ban2447}, 
    {0x7B49, 0x7B49, gb_chars_ban2448}, 
    {0x7B4B, 0x7B4C, gb_chars_ban2449}, 
    {0x7B4F, 0x7B52, gb_chars_ban2450}, 
    {0x7B54, 0x7B54, gb_chars_ban2451}, 
    {0x7B56, 0x7B56, gb_chars_ban2452}, 
    {0x7B58, 0x7B58, gb_chars_ban2453}, 
    {0x7B5A, 0x7B5B, gb_chars_ban2454}, 
    {0x7B5D, 0x7B5D, gb_chars_ban2455}, 
    {0x7B60, 0x7B60, gb_chars_ban2456}, 
    {0x7B62, 0x7B62, gb_chars_ban2457}, 
    {0x7B6E, 0x7B6E, gb_chars_ban2458}, 
    {0x7B71, 0x7B72, gb_chars_ban2459}, 
    {0x7B75, 0x7B75, gb_chars_ban2460}, 
    {0x7B77, 0x7B77, gb_chars_ban2461}, 
    {0x7B79, 0x7B79, gb_chars_ban2462}, 
    {0x7B7B, 0x7B7B, gb_chars_ban2463}, 
    {0x7B7E, 0x7B7E, gb_chars_ban2464}, 
    {0x7B80, 0x7B80, gb_chars_ban2465}, 
    {0x7B85, 0x7B85, gb_chars_ban2466}, 
    {0x7B8D, 0x7B8D, gb_chars_ban2467}, 
    {0x7B90, 0x7B90, gb_chars_ban2468}, 
    {0x7B94, 0x7B95, gb_chars_ban2469}, 
    {0x7B97, 0x7B97, gb_chars_ban2470}, 
    {0x7B9C, 0x7B9D, gb_chars_ban2471}, 
    {0x7BA1, 0x7BA2, gb_chars_ban2472}, 
    {0x7BA6, 0x7BAD, gb_chars_ban2473}, 
    {0x7BB1, 0x7BB1, gb_chars_ban2474}, 
    {0x7BB4, 0x7BB4, gb_chars_ban2475}, 
    {0x7BB8, 0x7BB8, gb_chars_ban2476}, 
    {0x7BC1, 0x7BC1, gb_chars_ban2477}, 
    {0x7BC6, 0x7BC7, gb_chars_ban2478}, 
    {0x7BCC, 0x7BCC, gb_chars_ban2479}, 
    {0x7BD1, 0x7BD1, gb_chars_ban2480}, 
    {0x7BD3, 0x7BD3, gb_chars_ban2481}, 
    {0x7BD9, 0x7BDA, gb_chars_ban2482}, 
    {0x7BDD, 0x7BDD, gb_chars_ban2483}, 
    {0x7BE1, 0x7BE1, gb_chars_ban2484}, 
    {0x7BE5, 0x7BE6, gb_chars_ban2485}, 
    {0x7BEA, 0x7BEA, gb_chars_ban2486}, 
    {0x7BEE, 0x7BEE, gb_chars_ban2487}, 
    {0x7BF1, 0x7BF1, gb_chars_ban2488}, 
    {0x7BF7, 0x7BF7, gb_chars_ban2489}, 
    {0x7BFC, 0x7BFC, gb_chars_ban2490}, 
    {0x7BFE, 0x7BFE, gb_chars_ban2491}, 
    {0x7C07, 0x7C07, gb_chars_ban2492}, 
    {0x7C0B, 0x7C0C, gb_chars_ban2493}, 
    {0x7C0F, 0x7C0F, gb_chars_ban2494}, 
    {0x7C16, 0x7C16, gb_chars_ban2495}, 
    {0x7C1F, 0x7C1F, gb_chars_ban2496}, 
    {0x7C26, 0x7C27, gb_chars_ban2497}, 
    {0x7C2A, 0x7C2A, gb_chars_ban2498}, 
    {0x7C38, 0x7C38, gb_chars_ban2499}, 
    {0x7C3F, 0x7C41, gb_chars_ban2500}, 
    {0x7C4D, 0x7C4D, gb_chars_ban2501}, 
    {0x7C73, 0x7C74, gb_chars_ban2502}, 
    {0x7C7B, 0x7C7D, gb_chars_ban2503}, 
    {0x7C89, 0x7C89, gb_chars_ban2504}, 
    {0x7C91, 0x7C92, gb_chars_ban2505}, 
    {0x7C95, 0x7C95, gb_chars_ban2506}, 
    {0x7C97, 0x7C98, gb_chars_ban2507}, 
    {0x7C9C, 0x7C9F, gb_chars_ban2508}, 
    {0x7CA2, 0x7CA2, gb_chars_ban2509}, 
    {0x7CA4, 0x7CA5, gb_chars_ban2510}, 
    {0x7CAA, 0x7CAA, gb_chars_ban2511}, 
    {0x7CAE, 0x7CAE, gb_chars_ban2512}, 
    {0x7CB1, 0x7CB3, gb_chars_ban2513}, 
    {0x7CB9, 0x7CB9, gb_chars_ban2514}, 
    {0x7CBC, 0x7CBE, gb_chars_ban2515}, 
    {0x7CC1, 0x7CC1, gb_chars_ban2516}, 
    {0x7CC5, 0x7CC5, gb_chars_ban2517}, 
    {0x7CC7, 0x7CC8, gb_chars_ban2518}, 
    {0x7CCA, 0x7CCA, gb_chars_ban2519}, 
    {0x7CCC, 0x7CCD, gb_chars_ban2520}, 
    {0x7CD5, 0x7CD7, gb_chars_ban2521}, 
    {0x7CD9, 0x7CD9, gb_chars_ban2522}, 
    {0x7CDC, 0x7CDC, gb_chars_ban2523}, 
    {0x7CDF, 0x7CE0, gb_chars_ban2524}, 
    {0x7CE8, 0x7CE8, gb_chars_ban2525}, 
    {0x7CEF, 0x7CEF, gb_chars_ban2526}, 
    {0x7CF8, 0x7CF8, gb_chars_ban2527}, 
    {0x7CFB, 0x7CFB, gb_chars_ban2528}, 
    {0x7D0A, 0x7D0A, gb_chars_ban2529}, 
    {0x7D20, 0x7D20, gb_chars_ban2530}, 
    {0x7D22, 0x7D22, gb_chars_ban2531}, 
    {0x7D27, 0x7D27, gb_chars_ban2532}, 
    {0x7D2B, 0x7D2B, gb_chars_ban2533}, 
    {0x7D2F, 0x7D2F, gb_chars_ban2534}, 
    {0x7D6E, 0x7D6E, gb_chars_ban2535}, 
    {0x7D77, 0x7D77, gb_chars_ban2536}, 
    {0x7DA6, 0x7DA6, gb_chars_ban2537}, 
    {0x7DAE, 0x7DAE, gb_chars_ban2538}, 
    {0x7E3B, 0x7E3B, gb_chars_ban2539}, 
    {0x7E41, 0x7E41, gb_chars_ban2540}, 
    {0x7E47, 0x7E47, gb_chars_ban2541}, 
    {0x7E82, 0x7E82, gb_chars_ban2542}, 
    {0x7E9B, 0x7E9B, gb_chars_ban2543}, 
    {0x7E9F, 0x7EAD, gb_chars_ban2544}, 
    {0x7EAF, 0x7EB3, gb_chars_ban2545}, 
    {0x7EB5, 0x7EBA, gb_chars_ban2546}, 
    {0x7EBD, 0x7ED5, gb_chars_ban2547}, 
    {0x7ED7, 0x7EE3, gb_chars_ban2548}, 
    {0x7EE5, 0x7EEB, gb_chars_ban2549}, 
    {0x7EED, 0x7EF8, gb_chars_ban2550}, 
    {0x7EFA, 0x7F09, gb_chars_ban2551}, 
    {0x7F0B, 0x7F0F, gb_chars_ban2552}, 
    {0x7F11, 0x7F1D, gb_chars_ban2553}, 
    {0x7F1F, 0x7F36, gb_chars_ban2554}, 
    {0x7F38, 0x7F38, gb_chars_ban2555}, 
    {0x7F3A, 0x7F3A, gb_chars_ban2556}, 
    {0x7F42, 0x7F42, gb_chars_ban2557}, 
    {0x7F44, 0x7F45, gb_chars_ban2558}, 
    {0x7F50, 0x7F51, gb_chars_ban2559}, 
    {0x7F54, 0x7F55, gb_chars_ban2560}, 
    {0x7F57, 0x7F58, gb_chars_ban2561}, 
    {0x7F5A, 0x7F5A, gb_chars_ban2562}, 
    {0x7F5F, 0x7F5F, gb_chars_ban2563}, 
    {0x7F61, 0x7F62, gb_chars_ban2564}, 
    {0x7F68, 0x7F6A, gb_chars_ban2565}, 
    {0x7F6E, 0x7F6E, gb_chars_ban2566}, 
    {0x7F71, 0x7F72, gb_chars_ban2567}, 
    {0x7F74, 0x7F74, gb_chars_ban2568}, 
    {0x7F79, 0x7F79, gb_chars_ban2569}, 
    {0x7F7E, 0x7F7E, gb_chars_ban2570}, 
    {0x7F81, 0x7F81, gb_chars_ban2571}, 
    {0x7F8A, 0x7F8A, gb_chars_ban2572}, 
    {0x7F8C, 0x7F8C, gb_chars_ban2573}, 
    {0x7F8E, 0x7F8E, gb_chars_ban2574}, 
    {0x7F94, 0x7F94, gb_chars_ban2575}, 
    {0x7F9A, 0x7F9A, gb_chars_ban2576}, 
    {0x7F9D, 0x7F9F, gb_chars_ban2577}, 
    {0x7FA1, 0x7FA1, gb_chars_ban2578}, 
    {0x7FA4, 0x7FA4, gb_chars_ban2579}, 
    {0x7FA7, 0x7FA7, gb_chars_ban2580}, 
    {0x7FAF, 0x7FB0, gb_chars_ban2581}, 
    {0x7FB2, 0x7FB2, gb_chars_ban2582}, 
    {0x7FB8, 0x7FB9, gb_chars_ban2583}, 
    {0x7FBC, 0x7FBD, gb_chars_ban2584}, 
    {0x7FBF, 0x7FBF, gb_chars_ban2585}, 
    {0x7FC1, 0x7FC1, gb_chars_ban2586}, 
    {0x7FC5, 0x7FC5, gb_chars_ban2587}, 
    {0x7FCA, 0x7FCA, gb_chars_ban2588}, 
    {0x7FCC, 0x7FCC, gb_chars_ban2589}, 
    {0x7FCE, 0x7FCE, gb_chars_ban2590}, 
    {0x7FD4, 0x7FD5, gb_chars_ban2591}, 
    {0x7FD8, 0x7FD8, gb_chars_ban2592}, 
    {0x7FDF, 0x7FE1, gb_chars_ban2593}, 
    {0x7FE5, 0x7FE6, gb_chars_ban2594}, 
    {0x7FE9, 0x7FE9, gb_chars_ban2595}, 
    {0x7FEE, 0x7FEE, gb_chars_ban2596}, 
    {0x7FF0, 0x7FF1, gb_chars_ban2597}, 
    {0x7FF3, 0x7FF3, gb_chars_ban2598}, 
    {0x7FFB, 0x7FFC, gb_chars_ban2599}, 
    {0x8000, 0x8001, gb_chars_ban2600}, 
    {0x8003, 0x8006, gb_chars_ban2601}, 
    {0x800B, 0x800D, gb_chars_ban2602}, 
    {0x8010, 0x8010, gb_chars_ban2603}, 
    {0x8012, 0x8012, gb_chars_ban2604}, 
    {0x8014, 0x8019, gb_chars_ban2605}, 
    {0x801C, 0x801C, gb_chars_ban2606}, 
    {0x8020, 0x8020, gb_chars_ban2607}, 
    {0x8022, 0x8022, gb_chars_ban2608}, 
    {0x8025, 0x802A, gb_chars_ban2609}, 
    {0x8031, 0x8031, gb_chars_ban2610}, 
    {0x8033, 0x8033, gb_chars_ban2611}, 
    {0x8035, 0x8038, gb_chars_ban2612}, 
    {0x803B, 0x803B, gb_chars_ban2613}, 
    {0x803D, 0x803D, gb_chars_ban2614}, 
    {0x803F, 0x803F, gb_chars_ban2615}, 
    {0x8042, 0x8043, gb_chars_ban2616}, 
    {0x8046, 0x8046, gb_chars_ban2617}, 
    {0x804A, 0x804D, gb_chars_ban2618}, 
    {0x8052, 0x8052, gb_chars_ban2619}, 
    {0x8054, 0x8054, gb_chars_ban2620}, 
    {0x8058, 0x8058, gb_chars_ban2621}, 
    {0x805A, 0x805A, gb_chars_ban2622}, 
    {0x8069, 0x806A, gb_chars_ban2623}, 
    {0x8071, 0x8071, gb_chars_ban2624}, 
    {0x807F, 0x8080, gb_chars_ban2625}, 
    {0x8083, 0x8084, gb_chars_ban2626}, 
    {0x8086, 0x8087, gb_chars_ban2627}, 
    {0x8089, 0x8089, gb_chars_ban2628}, 
    {0x808B, 0x808C, gb_chars_ban2629}, 
    {0x8093, 0x8093, gb_chars_ban2630}, 
    {0x8096, 0x8096, gb_chars_ban2631}, 
    {0x8098, 0x8098, gb_chars_ban2632}, 
    {0x809A, 0x809D, gb_chars_ban2633}, 
    {0x809F, 0x80A2, gb_chars_ban2634}, 
    {0x80A4, 0x80A5, gb_chars_ban2635}, 
    {0x80A9, 0x80AB, gb_chars_ban2636}, 
    {0x80AD, 0x80AF, gb_chars_ban2637}, 
    {0x80B1, 0x80B2, gb_chars_ban2638}, 
    {0x80B4, 0x80B4, gb_chars_ban2639}, 
    {0x80B7, 0x80B7, gb_chars_ban2640}, 
    {0x80BA, 0x80BA, gb_chars_ban2641}, 
    {0x80BC, 0x80C4, gb_chars_ban2642}, 
    {0x80C6, 0x80C6, gb_chars_ban2643}, 
    {0x80CC, 0x80CE, gb_chars_ban2644}, 
    {0x80D6, 0x80D7, gb_chars_ban2645}, 
    {0x80D9, 0x80DE, gb_chars_ban2646}, 
    {0x80E1, 0x80E1, gb_chars_ban2647}, 
    {0x80E4, 0x80E5, gb_chars_ban2648}, 
    {0x80E7, 0x80ED, gb_chars_ban2649}, 
    {0x80EF, 0x80F4, gb_chars_ban2650}, 
    {0x80F6, 0x80F6, gb_chars_ban2651}, 
    {0x80F8, 0x80F8, gb_chars_ban2652}, 
    {0x80FA, 0x80FA, gb_chars_ban2653}, 
    {0x80FC, 0x80FD, gb_chars_ban2654}, 
    {0x8102, 0x8102, gb_chars_ban2655}, 
    {0x8106, 0x8106, gb_chars_ban2656}, 
    {0x8109, 0x810A, gb_chars_ban2657}, 
    {0x810D, 0x8114, gb_chars_ban2658}, 
    {0x8116, 0x8116, gb_chars_ban2659}, 
    {0x8118, 0x8118, gb_chars_ban2660}, 
    {0x811A, 0x811A, gb_chars_ban2661}, 
    {0x811E, 0x811E, gb_chars_ban2662}, 
    {0x812C, 0x812C, gb_chars_ban2663}, 
    {0x812F, 0x812F, gb_chars_ban2664}, 
    {0x8131, 0x8132, gb_chars_ban2665}, 
    {0x8136, 0x8136, gb_chars_ban2666}, 
    {0x8138, 0x8138, gb_chars_ban2667}, 
    {0x813E, 0x813E, gb_chars_ban2668}, 
    {0x8146, 0x8146, gb_chars_ban2669}, 
    {0x8148, 0x8148, gb_chars_ban2670}, 
    {0x814A, 0x814C, gb_chars_ban2671}, 
    {0x8150, 0x8151, gb_chars_ban2672}, 
    {0x8153, 0x8155, gb_chars_ban2673}, 
    {0x8159, 0x815A, gb_chars_ban2674}, 
    {0x8160, 0x8160, gb_chars_ban2675}, 
    {0x8165, 0x8165, gb_chars_ban2676}, 
    {0x8167, 0x8167, gb_chars_ban2677}, 
    {0x8169, 0x8169, gb_chars_ban2678}, 
    {0x816D, 0x816E, gb_chars_ban2679}, 
    {0x8170, 0x8171, gb_chars_ban2680}, 
    {0x8174, 0x8174, gb_chars_ban2681}, 
    {0x8179, 0x8180, gb_chars_ban2682}, 
    {0x8182, 0x8182, gb_chars_ban2683}, 
    {0x8188, 0x8188, gb_chars_ban2684}, 
    {0x818A, 0x818A, gb_chars_ban2685}, 
    {0x818F, 0x818F, gb_chars_ban2686}, 
    {0x8191, 0x8191, gb_chars_ban2687}, 
    {0x8198, 0x8198, gb_chars_ban2688}, 
    {0x819B, 0x819D, gb_chars_ban2689}, 
    {0x81A3, 0x81A3, gb_chars_ban2690}, 
    {0x81A6, 0x81A6, gb_chars_ban2691}, 
    {0x81A8, 0x81A8, gb_chars_ban2692}, 
    {0x81AA, 0x81AA, gb_chars_ban2693}, 
    {0x81B3, 0x81B3, gb_chars_ban2694}, 
    {0x81BA, 0x81BB, gb_chars_ban2695}, 
    {0x81C0, 0x81C3, gb_chars_ban2696}, 
    {0x81C6, 0x81C6, gb_chars_ban2697}, 
    {0x81CA, 0x81CA, gb_chars_ban2698}, 
    {0x81CC, 0x81CC, gb_chars_ban2699}, 
    {0x81E3, 0x81E3, gb_chars_ban2700}, 
    {0x81E7, 0x81E7, gb_chars_ban2701}, 
    {0x81EA, 0x81EA, gb_chars_ban2702}, 
    {0x81EC, 0x81ED, gb_chars_ban2703}, 
    {0x81F3, 0x81F4, gb_chars_ban2704}, 
    {0x81FB, 0x81FC, gb_chars_ban2705}, 
    {0x81FE, 0x81FE, gb_chars_ban2706}, 
    {0x8200, 0x8202, gb_chars_ban2707}, 
    {0x8204, 0x8206, gb_chars_ban2708}, 
    {0x820C, 0x820D, gb_chars_ban2709}, 
    {0x8210, 0x8210, gb_chars_ban2710}, 
    {0x8212, 0x8212, gb_chars_ban2711}, 
    {0x8214, 0x8214, gb_chars_ban2712}, 
    {0x821B, 0x821C, gb_chars_ban2713}, 
    {0x821E, 0x821F, gb_chars_ban2714}, 
    {0x8221, 0x8223, gb_chars_ban2715}, 
    {0x8228, 0x8228, gb_chars_ban2716}, 
    {0x822A, 0x822D, gb_chars_ban2717}, 
    {0x822F, 0x8231, gb_chars_ban2718}, 
    {0x8233, 0x8239, gb_chars_ban2719}, 
    {0x823B, 0x823B, gb_chars_ban2720}, 
    {0x823E, 0x823E, gb_chars_ban2721}, 
    {0x8244, 0x8244, gb_chars_ban2722}, 
    {0x8247, 0x8247, gb_chars_ban2723}, 
    {0x8249, 0x8249, gb_chars_ban2724}, 
    {0x824B, 0x824B, gb_chars_ban2725}, 
    {0x824F, 0x824F, gb_chars_ban2726}, 
    {0x8258, 0x8258, gb_chars_ban2727}, 
    {0x825A, 0x825A, gb_chars_ban2728}, 
    {0x825F, 0x825F, gb_chars_ban2729}, 
    {0x8268, 0x8268, gb_chars_ban2730}, 
    {0x826E, 0x8270, gb_chars_ban2731}, 
    {0x8272, 0x8274, gb_chars_ban2732}, 
    {0x8279, 0x827A, gb_chars_ban2733}, 
    {0x827D, 0x827F, gb_chars_ban2734}, 
    {0x8282, 0x8282, gb_chars_ban2735}, 
    {0x8284, 0x8284, gb_chars_ban2736}, 
    {0x8288, 0x8288, gb_chars_ban2737}, 
    {0x828A, 0x828B, gb_chars_ban2738}, 
    {0x828D, 0x828F, gb_chars_ban2739}, 
    {0x8291, 0x8292, gb_chars_ban2740}, 
    {0x8297, 0x8299, gb_chars_ban2741}, 
    {0x829C, 0x829D, gb_chars_ban2742}, 
    {0x829F, 0x829F, gb_chars_ban2743}, 
    {0x82A1, 0x82A1, gb_chars_ban2744}, 
    {0x82A4, 0x82A6, gb_chars_ban2745}, 
    {0x82A8, 0x82B1, gb_chars_ban2746}, 
    {0x82B3, 0x82B4, gb_chars_ban2747}, 
    {0x82B7, 0x82B9, gb_chars_ban2748}, 
    {0x82BD, 0x82BE, gb_chars_ban2749}, 
    {0x82C1, 0x82C1, gb_chars_ban2750}, 
    {0x82C4, 0x82C4, gb_chars_ban2751}, 
    {0x82C7, 0x82C8, gb_chars_ban2752}, 
    {0x82CA, 0x82CF, gb_chars_ban2753}, 
    {0x82D1, 0x82D5, gb_chars_ban2754}, 
    {0x82D7, 0x82D8, gb_chars_ban2755}, 
    {0x82DB, 0x82DC, gb_chars_ban2756}, 
    {0x82DE, 0x82E1, gb_chars_ban2757}, 
    {0x82E3, 0x82E6, gb_chars_ban2758}, 
    {0x82EB, 0x82EB, gb_chars_ban2759}, 
    {0x82EF, 0x82EF, gb_chars_ban2760}, 
    {0x82F1, 0x82F1, gb_chars_ban2761}, 
    {0x82F4, 0x82F4, gb_chars_ban2762}, 
    {0x82F7, 0x82F7, gb_chars_ban2763}, 
    {0x82F9, 0x82F9, gb_chars_ban2764}, 
    {0x82FB, 0x82FB, gb_chars_ban2765}, 
    {0x8301, 0x8309, gb_chars_ban2766}, 
    {0x830C, 0x830C, gb_chars_ban2767}, 
    {0x830E, 0x830F, gb_chars_ban2768}, 
    {0x8311, 0x8311, gb_chars_ban2769}, 
    {0x8314, 0x8315, gb_chars_ban2770}, 
    {0x8317, 0x8317, gb_chars_ban2771}, 
    {0x831A, 0x831C, gb_chars_ban2772}, 
    {0x8327, 0x8328, gb_chars_ban2773}, 
    {0x832B, 0x832D, gb_chars_ban2774}, 
    {0x832F, 0x832F, gb_chars_ban2775}, 
    {0x8331, 0x8331, gb_chars_ban2776}, 
    {0x8333, 0x8336, gb_chars_ban2777}, 
    {0x8338, 0x833A, gb_chars_ban2778}, 
    {0x833C, 0x833C, gb_chars_ban2779}, 
    {0x8340, 0x8340, gb_chars_ban2780}, 
    {0x8343, 0x8343, gb_chars_ban2781}, 
    {0x8346, 0x8347, gb_chars_ban2782}, 
    {0x8349, 0x8349, gb_chars_ban2783}, 
    {0x834F, 0x8352, gb_chars_ban2784}, 
    {0x8354, 0x8354, gb_chars_ban2785}, 
    {0x835A, 0x835C, gb_chars_ban2786}, 
    {0x835E, 0x8361, gb_chars_ban2787}, 
    {0x8363, 0x836F, gb_chars_ban2788}, 
    {0x8377, 0x8378, gb_chars_ban2789}, 
    {0x837B, 0x837D, gb_chars_ban2790}, 
    {0x8385, 0x8386, gb_chars_ban2791}, 
    {0x8389, 0x8389, gb_chars_ban2792}, 
    {0x838E, 0x838E, gb_chars_ban2793}, 
    {0x8392, 0x8393, gb_chars_ban2794}, 
    {0x8398, 0x8398, gb_chars_ban2795}, 
    {0x839B, 0x839C, gb_chars_ban2796}, 
    {0x839E, 0x839E, gb_chars_ban2797}, 
    {0x83A0, 0x83A0, gb_chars_ban2798}, 
    {0x83A8, 0x83AB, gb_chars_ban2799}, 
    {0x83B0, 0x83B4, gb_chars_ban2800}, 
    {0x83B6, 0x83BA, gb_chars_ban2801}, 
    {0x83BC, 0x83BD, gb_chars_ban2802}, 
    {0x83C0, 0x83C1, gb_chars_ban2803}, 
    {0x83C5, 0x83C5, gb_chars_ban2804}, 
    {0x83C7, 0x83C7, gb_chars_ban2805}, 
    {0x83CA, 0x83CA, gb_chars_ban2806}, 
    {0x83CC, 0x83CC, gb_chars_ban2807}, 
    {0x83CF, 0x83CF, gb_chars_ban2808}, 
    {0x83D4, 0x83D4, gb_chars_ban2809}, 
    {0x83D6, 0x83D6, gb_chars_ban2810}, 
    {0x83D8, 0x83D8, gb_chars_ban2811}, 
    {0x83DC, 0x83DD, gb_chars_ban2812}, 
    {0x83DF, 0x83E1, gb_chars_ban2813}, 
    {0x83E5, 0x83E5, gb_chars_ban2814}, 
    {0x83E9, 0x83EA, gb_chars_ban2815}, 
    {0x83F0, 0x83F2, gb_chars_ban2816}, 
    {0x83F8, 0x83F9, gb_chars_ban2817}, 
    {0x83FD, 0x83FD, gb_chars_ban2818}, 
    {0x8401, 0x8401, gb_chars_ban2819}, 
    {0x8403, 0x8404, gb_chars_ban2820}, 
    {0x8406, 0x8406, gb_chars_ban2821}, 
    {0x840B, 0x840F, gb_chars_ban2822}, 
    {0x8411, 0x8411, gb_chars_ban2823}, 
    {0x8418, 0x8418, gb_chars_ban2824}, 
    {0x841C, 0x841D, gb_chars_ban2825}, 
    {0x8424, 0x8428, gb_chars_ban2826}, 
    {0x8431, 0x8431, gb_chars_ban2827}, 
    {0x8438, 0x8438, gb_chars_ban2828}, 
    {0x843C, 0x843D, gb_chars_ban2829}, 
    {0x8446, 0x8446, gb_chars_ban2830}, 
    {0x8451, 0x8451, gb_chars_ban2831}, 
    {0x8457, 0x8457, gb_chars_ban2832}, 
    {0x8459, 0x845C, gb_chars_ban2833}, 
    {0x8461, 0x8461, gb_chars_ban2834}, 
    {0x8463, 0x8463, gb_chars_ban2835}, 
    {0x8469, 0x8469, gb_chars_ban2836}, 
    {0x846B, 0x846D, gb_chars_ban2837}, 
    {0x8471, 0x8471, gb_chars_ban2838}, 
    {0x8473, 0x8473, gb_chars_ban2839}, 
    {0x8475, 0x8476, gb_chars_ban2840}, 
    {0x8478, 0x8478, gb_chars_ban2841}, 
    {0x847A, 0x847A, gb_chars_ban2842}, 
    {0x8482, 0x8482, gb_chars_ban2843}, 
    {0x8487, 0x8489, gb_chars_ban2844}, 
    {0x848B, 0x848C, gb_chars_ban2845}, 
    {0x848E, 0x848E, gb_chars_ban2846}, 
    {0x8497, 0x8497, gb_chars_ban2847}, 
    {0x8499, 0x8499, gb_chars_ban2848}, 
    {0x849C, 0x849C, gb_chars_ban2849}, 
    {0x84A1, 0x84A1, gb_chars_ban2850}, 
    {0x84AF, 0x84AF, gb_chars_ban2851}, 
    {0x84B2, 0x84B2, gb_chars_ban2852}, 
    {0x84B4, 0x84B4, gb_chars_ban2853}, 
    {0x84B8, 0x84BA, gb_chars_ban2854}, 
    {0x84BD, 0x84BD, gb_chars_ban2855}, 
    {0x84BF, 0x84BF, gb_chars_ban2856}, 
    {0x84C1, 0x84C1, gb_chars_ban2857}, 
    {0x84C4, 0x84C4, gb_chars_ban2858}, 
    {0x84C9, 0x84CA, gb_chars_ban2859}, 
    {0x84CD, 0x84CD, gb_chars_ban2860}, 
    {0x84D0, 0x84D1, gb_chars_ban2861}, 
    {0x84D3, 0x84D3, gb_chars_ban2862}, 
    {0x84D6, 0x84D6, gb_chars_ban2863}, 
    {0x84DD, 0x84DD, gb_chars_ban2864}, 
    {0x84DF, 0x84E0, gb_chars_ban2865}, 
    {0x84E3, 0x84E3, gb_chars_ban2866}, 
    {0x84E5, 0x84E6, gb_chars_ban2867}, 
    {0x84EC, 0x84EC, gb_chars_ban2868}, 
    {0x84F0, 0x84F0, gb_chars_ban2869}, 
    {0x84FC, 0x84FC, gb_chars_ban2870}, 
    {0x84FF, 0x84FF, gb_chars_ban2871}, 
    {0x850C, 0x850C, gb_chars_ban2872}, 
    {0x8511, 0x8511, gb_chars_ban2873}, 
    {0x8513, 0x8513, gb_chars_ban2874}, 
    {0x8517, 0x8517, gb_chars_ban2875}, 
    {0x851A, 0x851A, gb_chars_ban2876}, 
    {0x851F, 0x851F, gb_chars_ban2877}, 
    {0x8521, 0x8521, gb_chars_ban2878}, 
    {0x852B, 0x852C, gb_chars_ban2879}, 
    {0x8537, 0x853D, gb_chars_ban2880}, 
    {0x8543, 0x8543, gb_chars_ban2881}, 
    {0x8548, 0x854A, gb_chars_ban2882}, 
    {0x8556, 0x8556, gb_chars_ban2883}, 
    {0x8559, 0x8559, gb_chars_ban2884}, 
    {0x855E, 0x855E, gb_chars_ban2885}, 
    {0x8564, 0x8564, gb_chars_ban2886}, 
    {0x8568, 0x8568, gb_chars_ban2887}, 
    {0x8572, 0x8572, gb_chars_ban2888}, 
    {0x8574, 0x8574, gb_chars_ban2889}, 
    {0x8579, 0x857B, gb_chars_ban2890}, 
    {0x857E, 0x857E, gb_chars_ban2891}, 
    {0x8584, 0x8585, gb_chars_ban2892}, 
    {0x8587, 0x8587, gb_chars_ban2893}, 
    {0x858F, 0x858F, gb_chars_ban2894}, 
    {0x859B, 0x859C, gb_chars_ban2895}, 
    {0x85A4, 0x85A4, gb_chars_ban2896}, 
    {0x85A8, 0x85A8, gb_chars_ban2897}, 
    {0x85AA, 0x85AA, gb_chars_ban2898}, 
    {0x85AE, 0x85B0, gb_chars_ban2899}, 
    {0x85B7, 0x85B7, gb_chars_ban2900}, 
    {0x85B9, 0x85B9, gb_chars_ban2901}, 
    {0x85C1, 0x85C1, gb_chars_ban2902}, 
    {0x85C9, 0x85C9, gb_chars_ban2903}, 
    {0x85CF, 0x85D0, gb_chars_ban2904}, 
    {0x85D3, 0x85D3, gb_chars_ban2905}, 
    {0x85D5, 0x85D5, gb_chars_ban2906}, 
    {0x85DC, 0x85DC, gb_chars_ban2907}, 
    {0x85E4, 0x85E4, gb_chars_ban2908}, 
    {0x85E9, 0x85E9, gb_chars_ban2909}, 
    {0x85FB, 0x85FB, gb_chars_ban2910}, 
    {0x85FF, 0x85FF, gb_chars_ban2911}, 
    {0x8605, 0x8605, gb_chars_ban2912}, 
    {0x8611, 0x8611, gb_chars_ban2913}, 
    {0x8616, 0x8616, gb_chars_ban2914}, 
    {0x8627, 0x8627, gb_chars_ban2915}, 
    {0x8629, 0x8629, gb_chars_ban2916}, 
    {0x8638, 0x8638, gb_chars_ban2917}, 
    {0x863C, 0x863C, gb_chars_ban2918}, 
    {0x864D, 0x8651, gb_chars_ban2919}, 
    {0x8654, 0x8654, gb_chars_ban2920}, 
    {0x865A, 0x865A, gb_chars_ban2921}, 
    {0x865E, 0x865E, gb_chars_ban2922}, 
    {0x8662, 0x8662, gb_chars_ban2923}, 
    {0x866B, 0x866C, gb_chars_ban2924}, 
    {0x866E, 0x866E, gb_chars_ban2925}, 
    {0x8671, 0x8671, gb_chars_ban2926}, 
    {0x8679, 0x8682, gb_chars_ban2927}, 
    {0x868A, 0x868D, gb_chars_ban2928}, 
    {0x8693, 0x8693, gb_chars_ban2929}, 
    {0x8695, 0x8695, gb_chars_ban2930}, 
    {0x869C, 0x869D, gb_chars_ban2931}, 
    {0x86A3, 0x86A4, gb_chars_ban2932}, 
    {0x86A7, 0x86AA, gb_chars_ban2933}, 
    {0x86AC, 0x86AC, gb_chars_ban2934}, 
    {0x86AF, 0x86B1, gb_chars_ban2935}, 
    {0x86B4, 0x86B6, gb_chars_ban2936}, 
    {0x86BA, 0x86BA, gb_chars_ban2937}, 
    {0x86C0, 0x86C0, gb_chars_ban2938}, 
    {0x86C4, 0x86C4, gb_chars_ban2939}, 
    {0x86C6, 0x86C7, gb_chars_ban2940}, 
    {0x86C9, 0x86CB, gb_chars_ban2941}, 
    {0x86CE, 0x86D1, gb_chars_ban2942}, 
    {0x86D4, 0x86D4, gb_chars_ban2943}, 
    {0x86D8, 0x86D9, gb_chars_ban2944}, 
    {0x86DB, 0x86DB, gb_chars_ban2945}, 
    {0x86DE, 0x86DF, gb_chars_ban2946}, 
    {0x86E4, 0x86E4, gb_chars_ban2947}, 
    {0x86E9, 0x86E9, gb_chars_ban2948}, 
    {0x86ED, 0x86EE, gb_chars_ban2949}, 
    {0x86F0, 0x86F4, gb_chars_ban2950}, 
    {0x86F8, 0x86F9, gb_chars_ban2951}, 
    {0x86FE, 0x86FE, gb_chars_ban2952}, 
    {0x8700, 0x8700, gb_chars_ban2953}, 
    {0x8702, 0x8703, gb_chars_ban2954}, 
    {0x8707, 0x870A, gb_chars_ban2955}, 
    {0x870D, 0x870D, gb_chars_ban2956}, 
    {0x8712, 0x8713, gb_chars_ban2957}, 
    {0x8715, 0x8715, gb_chars_ban2958}, 
    {0x8717, 0x8718, gb_chars_ban2959}, 
    {0x871A, 0x871A, gb_chars_ban2960}, 
    {0x871C, 0x871C, gb_chars_ban2961}, 
    {0x871E, 0x871E, gb_chars_ban2962}, 
    {0x8721, 0x8723, gb_chars_ban2963}, 
    {0x8725, 0x8725, gb_chars_ban2964}, 
    {0x8729, 0x8729, gb_chars_ban2965}, 
    {0x872E, 0x872E, gb_chars_ban2966}, 
    {0x8731, 0x8731, gb_chars_ban2967}, 
    {0x8734, 0x8734, gb_chars_ban2968}, 
    {0x8737, 0x8737, gb_chars_ban2969}, 
    {0x873B, 0x873B, gb_chars_ban2970}, 
    {0x873E, 0x873F, gb_chars_ban2971}, 
    {0x8747, 0x8749, gb_chars_ban2972}, 
    {0x874C, 0x874C, gb_chars_ban2973}, 
    {0x874E, 0x874E, gb_chars_ban2974}, 
    {0x8753, 0x8753, gb_chars_ban2975}, 
    {0x8757, 0x8757, gb_chars_ban2976}, 
    {0x8759, 0x8759, gb_chars_ban2977}, 
    {0x8760, 0x8760, gb_chars_ban2978}, 
    {0x8763, 0x8765, gb_chars_ban2979}, 
    {0x876E, 0x876E, gb_chars_ban2980}, 
    {0x8770, 0x8770, gb_chars_ban2981}, 
    {0x8774, 0x8774, gb_chars_ban2982}, 
    {0x8776, 0x8776, gb_chars_ban2983}, 
    {0x877B, 0x877E, gb_chars_ban2984}, 
    {0x8782, 0x8783, gb_chars_ban2985}, 
    {0x8785, 0x8785, gb_chars_ban2986}, 
    {0x8788, 0x8788, gb_chars_ban2987}, 
    {0x878B, 0x878B, gb_chars_ban2988}, 
    {0x878D, 0x878D, gb_chars_ban2989}, 
    {0x8793, 0x8793, gb_chars_ban2990}, 
    {0x8797, 0x8797, gb_chars_ban2991}, 
    {0x879F, 0x879F, gb_chars_ban2992}, 
    {0x87A8, 0x87A8, gb_chars_ban2993}, 
    {0x87AB, 0x87AD, gb_chars_ban2994}, 
    {0x87AF, 0x87AF, gb_chars_ban2995}, 
    {0x87B3, 0x87B3, gb_chars_ban2996}, 
    {0x87B5, 0x87B5, gb_chars_ban2997}, 
    {0x87BA, 0x87BA, gb_chars_ban2998}, 
    {0x87BD, 0x87BD, gb_chars_ban2999}, 
    {0x87C0, 0x87C0, gb_chars_ban3000}, 
    {0x87C6, 0x87C6, gb_chars_ban3001}, 
    {0x87CA, 0x87CB, gb_chars_ban3002}, 
    {0x87D1, 0x87D3, gb_chars_ban3003}, 
    {0x87DB, 0x87DB, gb_chars_ban3004}, 
    {0x87E0, 0x87E0, gb_chars_ban3005}, 
    {0x87E5, 0x87E5, gb_chars_ban3006}, 
    {0x87EA, 0x87EA, gb_chars_ban3007}, 
    {0x87EE, 0x87EE, gb_chars_ban3008}, 
    {0x87F9, 0x87F9, gb_chars_ban3009}, 
    {0x87FE, 0x87FE, gb_chars_ban3010}, 
    {0x8803, 0x8803, gb_chars_ban3011}, 
    {0x880A, 0x880A, gb_chars_ban3012}, 
    {0x8813, 0x8813, gb_chars_ban3013}, 
    {0x8815, 0x8816, gb_chars_ban3014}, 
    {0x881B, 0x881B, gb_chars_ban3015}, 
    {0x8821, 0x8822, gb_chars_ban3016}, 
    {0x8832, 0x8832, gb_chars_ban3017}, 
    {0x8839, 0x8839, gb_chars_ban3018}, 
    {0x883C, 0x883C, gb_chars_ban3019}, 
    {0x8840, 0x8840, gb_chars_ban3020}, 
    {0x8844, 0x8845, gb_chars_ban3021}, 
    {0x884C, 0x884D, gb_chars_ban3022}, 
    {0x8854, 0x8854, gb_chars_ban3023}, 
    {0x8857, 0x8857, gb_chars_ban3024}, 
    {0x8859, 0x8859, gb_chars_ban3025}, 
    {0x8861, 0x8865, gb_chars_ban3026}, 
    {0x8868, 0x8869, gb_chars_ban3027}, 
    {0x886B, 0x886C, gb_chars_ban3028}, 
    {0x886E, 0x886E, gb_chars_ban3029}, 
    {0x8870, 0x8870, gb_chars_ban3030}, 
    {0x8872, 0x8872, gb_chars_ban3031}, 
    {0x8877, 0x8877, gb_chars_ban3032}, 
    {0x887D, 0x887F, gb_chars_ban3033}, 
    {0x8881, 0x8882, gb_chars_ban3034}, 
    {0x8884, 0x8885, gb_chars_ban3035}, 
    {0x8888, 0x8888, gb_chars_ban3036}, 
    {0x888B, 0x888B, gb_chars_ban3037}, 
    {0x888D, 0x888D, gb_chars_ban3038}, 
    {0x8892, 0x8892, gb_chars_ban3039}, 
    {0x8896, 0x8896, gb_chars_ban3040}, 
    {0x889C, 0x889C, gb_chars_ban3041}, 
    {0x88A2, 0x88A2, gb_chars_ban3042}, 
    {0x88A4, 0x88A4, gb_chars_ban3043}, 
    {0x88AB, 0x88AB, gb_chars_ban3044}, 
    {0x88AD, 0x88AD, gb_chars_ban3045}, 
    {0x88B1, 0x88B1, gb_chars_ban3046}, 
    {0x88B7, 0x88B7, gb_chars_ban3047}, 
    {0x88BC, 0x88BC, gb_chars_ban3048}, 
    {0x88C1, 0x88C2, gb_chars_ban3049}, 
    {0x88C5, 0x88C6, gb_chars_ban3050}, 
    {0x88C9, 0x88C9, gb_chars_ban3051}, 
    {0x88CE, 0x88CE, gb_chars_ban3052}, 
    {0x88D2, 0x88D2, gb_chars_ban3053}, 
    {0x88D4, 0x88D5, gb_chars_ban3054}, 
    {0x88D8, 0x88D9, gb_chars_ban3055}, 
    {0x88DF, 0x88DF, gb_chars_ban3056}, 
    {0x88E2, 0x88E5, gb_chars_ban3057}, 
    {0x88E8, 0x88E8, gb_chars_ban3058}, 
    {0x88F0, 0x88F1, gb_chars_ban3059}, 
    {0x88F3, 0x88F4, gb_chars_ban3060}, 
    {0x88F8, 0x88F9, gb_chars_ban3061}, 
    {0x88FC, 0x88FC, gb_chars_ban3062}, 
    {0x88FE, 0x88FE, gb_chars_ban3063}, 
    {0x8902, 0x8902, gb_chars_ban3064}, 
    {0x890A, 0x890A, gb_chars_ban3065}, 
    {0x8910, 0x8910, gb_chars_ban3066}, 
    {0x8912, 0x8913, gb_chars_ban3067}, 
    {0x8919, 0x891B, gb_chars_ban3068}, 
    {0x8921, 0x8921, gb_chars_ban3069}, 
    {0x8925, 0x8925, gb_chars_ban3070}, 
    {0x892A, 0x892B, gb_chars_ban3071}, 
    {0x8930, 0x8930, gb_chars_ban3072}, 
    {0x8934, 0x8934, gb_chars_ban3073}, 
    {0x8936, 0x8936, gb_chars_ban3074}, 
    {0x8941, 0x8941, gb_chars_ban3075}, 
    {0x8944, 0x8944, gb_chars_ban3076}, 
    {0x895E, 0x895F, gb_chars_ban3077}, 
    {0x8966, 0x8966, gb_chars_ban3078}, 
    {0x897B, 0x897B, gb_chars_ban3079}, 
    {0x897F, 0x897F, gb_chars_ban3080}, 
    {0x8981, 0x8981, gb_chars_ban3081}, 
    {0x8983, 0x8983, gb_chars_ban3082}, 
    {0x8986, 0x8986, gb_chars_ban3083}, 
    {0x89C1, 0x89C2, gb_chars_ban3084}, 
    {0x89C4, 0x89CC, gb_chars_ban3085}, 
    {0x89CE, 0x89D2, gb_chars_ban3086}, 
    {0x89D6, 0x89D6, gb_chars_ban3087}, 
    {0x89DA, 0x89DA, gb_chars_ban3088}, 
    {0x89DC, 0x89DC, gb_chars_ban3089}, 
    {0x89DE, 0x89DE, gb_chars_ban3090}, 
    {0x89E3, 0x89E3, gb_chars_ban3091}, 
    {0x89E5, 0x89E6, gb_chars_ban3092}, 
    {0x89EB, 0x89EB, gb_chars_ban3093}, 
    {0x89EF, 0x89EF, gb_chars_ban3094}, 
    {0x89F3, 0x89F3, gb_chars_ban3095}, 
    {0x8A00, 0x8A00, gb_chars_ban3096}, 
    {0x8A07, 0x8A07, gb_chars_ban3097}, 
    {0x8A3E, 0x8A3E, gb_chars_ban3098}, 
    {0x8A48, 0x8A48, gb_chars_ban3099}, 
    {0x8A79, 0x8A79, gb_chars_ban3100}, 
    {0x8A89, 0x8A8A, gb_chars_ban3101}, 
    {0x8A93, 0x8A93, gb_chars_ban3102}, 
    {0x8B07, 0x8B07, gb_chars_ban3103}, 
    {0x8B26, 0x8B26, gb_chars_ban3104}, 
    {0x8B66, 0x8B66, gb_chars_ban3105}, 
    {0x8B6C, 0x8B6C, gb_chars_ban3106}, 
    {0x8BA0, 0x8BAB, gb_chars_ban3107}, 
    {0x8BAD, 0x8BB0, gb_chars_ban3108}, 
    {0x8BB2, 0x8BBA, gb_chars_ban3109}, 
    {0x8BBC, 0x8BC6, gb_chars_ban3110}, 
    {0x8BC8, 0x8BCF, gb_chars_ban3111}, 
    {0x8BD1, 0x8BE9, gb_chars_ban3112}, 
    {0x8BEB, 0x8C08, gb_chars_ban3113}, 
    {0x8C0A, 0x8C1D, gb_chars_ban3114}, 
    {0x8C1F, 0x8C37, gb_chars_ban3115}, 
    {0x8C41, 0x8C41, gb_chars_ban3116}, 
    {0x8C46, 0x8C47, gb_chars_ban3117}, 
    {0x8C49, 0x8C49, gb_chars_ban3118}, 
    {0x8C4C, 0x8C4C, gb_chars_ban3119}, 
    {0x8C55, 0x8C55, gb_chars_ban3120}, 
    {0x8C5A, 0x8C5A, gb_chars_ban3121}, 
    {0x8C61, 0x8C62, gb_chars_ban3122}, 
    {0x8C6A, 0x8C6B, gb_chars_ban3123}, 
    {0x8C73, 0x8C73, gb_chars_ban3124}, 
    {0x8C78, 0x8C7A, gb_chars_ban3125}, 
    {0x8C82, 0x8C82, gb_chars_ban3126}, 
    {0x8C85, 0x8C85, gb_chars_ban3127}, 
    {0x8C89, 0x8C8A, gb_chars_ban3128}, 
    {0x8C8C, 0x8C8C, gb_chars_ban3129}, 
    {0x8C94, 0x8C94, gb_chars_ban3130}, 
    {0x8C98, 0x8C98, gb_chars_ban3131}, 
    {0x8D1D, 0x8D1F, gb_chars_ban3132}, 
    {0x8D21, 0x8D50, gb_chars_ban3133}, 
    {0x8D53, 0x8D56, gb_chars_ban3134}, 
    {0x8D58, 0x8D5E, gb_chars_ban3135}, 
    {0x8D60, 0x8D64, gb_chars_ban3136}, 
    {0x8D66, 0x8D67, gb_chars_ban3137}, 
    {0x8D6B, 0x8D6B, gb_chars_ban3138}, 
    {0x8D6D, 0x8D6D, gb_chars_ban3139}, 
    {0x8D70, 0x8D70, gb_chars_ban3140}, 
    {0x8D73, 0x8D77, gb_chars_ban3141}, 
    {0x8D81, 0x8D81, gb_chars_ban3142}, 
    {0x8D84, 0x8D85, gb_chars_ban3143}, 
    {0x8D8A, 0x8D8B, gb_chars_ban3144}, 
    {0x8D91, 0x8D91, gb_chars_ban3145}, 
    {0x8D94, 0x8D94, gb_chars_ban3146}, 
    {0x8D9F, 0x8D9F, gb_chars_ban3147}, 
    {0x8DA3, 0x8DA3, gb_chars_ban3148}, 
    {0x8DB1, 0x8DB1, gb_chars_ban3149}, 
    {0x8DB3, 0x8DB5, gb_chars_ban3150}, 
    {0x8DB8, 0x8DB8, gb_chars_ban3151}, 
    {0x8DBA, 0x8DBA, gb_chars_ban3152}, 
    {0x8DBC, 0x8DBC, gb_chars_ban3153}, 
    {0x8DBE, 0x8DBF, gb_chars_ban3154}, 
    {0x8DC3, 0x8DC4, gb_chars_ban3155}, 
    {0x8DC6, 0x8DC6, gb_chars_ban3156}, 
    {0x8DCB, 0x8DCC, gb_chars_ban3157}, 
    {0x8DCE, 0x8DCF, gb_chars_ban3158}, 
    {0x8DD1, 0x8DD1, gb_chars_ban3159}, 
    {0x8DD6, 0x8DD7, gb_chars_ban3160}, 
    {0x8DDA, 0x8DDB, gb_chars_ban3161}, 
    {0x8DDD, 0x8DDF, gb_chars_ban3162}, 
    {0x8DE3, 0x8DE4, gb_chars_ban3163}, 
    {0x8DE8, 0x8DE8, gb_chars_ban3164}, 
    {0x8DEA, 0x8DEC, gb_chars_ban3165}, 
    {0x8DEF, 0x8DEF, gb_chars_ban3166}, 
    {0x8DF3, 0x8DF3, gb_chars_ban3167}, 
    {0x8DF5, 0x8DF5, gb_chars_ban3168}, 
    {0x8DF7, 0x8DFB, gb_chars_ban3169}, 
    {0x8DFD, 0x8DFD, gb_chars_ban3170}, 
    {0x8E05, 0x8E05, gb_chars_ban3171}, 
    {0x8E09, 0x8E0A, gb_chars_ban3172}, 
    {0x8E0C, 0x8E0C, gb_chars_ban3173}, 
    {0x8E0F, 0x8E0F, gb_chars_ban3174}, 
    {0x8E14, 0x8E14, gb_chars_ban3175}, 
    {0x8E1D, 0x8E1F, gb_chars_ban3176}, 
    {0x8E22, 0x8E23, gb_chars_ban3177}, 
    {0x8E29, 0x8E2A, gb_chars_ban3178}, 
    {0x8E2C, 0x8E2C, gb_chars_ban3179}, 
    {0x8E2E, 0x8E2F, gb_chars_ban3180}, 
    {0x8E31, 0x8E31, gb_chars_ban3181}, 
    {0x8E35, 0x8E35, gb_chars_ban3182}, 
    {0x8E39, 0x8E3A, gb_chars_ban3183}, 
    {0x8E3D, 0x8E3D, gb_chars_ban3184}, 
    {0x8E40, 0x8E42, gb_chars_ban3185}, 
    {0x8E44, 0x8E44, gb_chars_ban3186}, 
    {0x8E47, 0x8E4B, gb_chars_ban3187}, 
    {0x8E51, 0x8E52, gb_chars_ban3188}, 
    {0x8E59, 0x8E59, gb_chars_ban3189}, 
    {0x8E66, 0x8E66, gb_chars_ban3190}, 
    {0x8E69, 0x8E69, gb_chars_ban3191}, 
    {0x8E6C, 0x8E6D, gb_chars_ban3192}, 
    {0x8E6F, 0x8E70, gb_chars_ban3193}, 
    {0x8E72, 0x8E72, gb_chars_ban3194}, 
    {0x8E74, 0x8E74, gb_chars_ban3195}, 
    {0x8E76, 0x8E76, gb_chars_ban3196}, 
    {0x8E7C, 0x8E7C, gb_chars_ban3197}, 
    {0x8E7F, 0x8E7F, gb_chars_ban3198}, 
    {0x8E81, 0x8E81, gb_chars_ban3199}, 
    {0x8E85, 0x8E85, gb_chars_ban3200}, 
    {0x8E87, 0x8E87, gb_chars_ban3201}, 
    {0x8E8F, 0x8E90, gb_chars_ban3202}, 
    {0x8E94, 0x8E94, gb_chars_ban3203}, 
    {0x8E9C, 0x8E9C, gb_chars_ban3204}, 
    {0x8E9E, 0x8E9E, gb_chars_ban3205}, 
    {0x8EAB, 0x8EAC, gb_chars_ban3206}, 
    {0x8EAF, 0x8EAF, gb_chars_ban3207}, 
    {0x8EB2, 0x8EB2, gb_chars_ban3208}, 
    {0x8EBA, 0x8EBA, gb_chars_ban3209}, 
    {0x8ECE, 0x8ECE, gb_chars_ban3210}, 
    {0x8F66, 0x8F69, gb_chars_ban3211}, 
    {0x8F6B, 0x8F7F, gb_chars_ban3212}, 
    {0x8F81, 0x8F8B, gb_chars_ban3213}, 
    {0x8F8D, 0x8F91, gb_chars_ban3214}, 
    {0x8F93, 0x8F9C, gb_chars_ban3215}, 
    {0x8F9E, 0x8F9F, gb_chars_ban3216}, 
    {0x8FA3, 0x8FA3, gb_chars_ban3217}, 
    {0x8FA8, 0x8FA9, gb_chars_ban3218}, 
    {0x8FAB, 0x8FAB, gb_chars_ban3219}, 
    {0x8FB0, 0x8FB1, gb_chars_ban3220}, 
    {0x8FB6, 0x8FB6, gb_chars_ban3221}, 
    {0x8FB9, 0x8FB9, gb_chars_ban3222}, 
    {0x8FBD, 0x8FBE, gb_chars_ban3223}, 
    {0x8FC1, 0x8FC2, gb_chars_ban3224}, 
    {0x8FC4, 0x8FC5, gb_chars_ban3225}, 
    {0x8FC7, 0x8FC8, gb_chars_ban3226}, 
    {0x8FCE, 0x8FCE, gb_chars_ban3227}, 
    {0x8FD0, 0x8FD1, gb_chars_ban3228}, 
    {0x8FD3, 0x8FD5, gb_chars_ban3229}, 
    {0x8FD8, 0x8FD9, gb_chars_ban3230}, 
    {0x8FDB, 0x8FDF, gb_chars_ban3231}, 
    {0x8FE2, 0x8FE2, gb_chars_ban3232}, 
    {0x8FE4, 0x8FE6, gb_chars_ban3233}, 
    {0x8FE8, 0x8FEB, gb_chars_ban3234}, 
    {0x8FED, 0x8FEE, gb_chars_ban3235}, 
    {0x8FF0, 0x8FF0, gb_chars_ban3236}, 
    {0x8FF3, 0x8FF3, gb_chars_ban3237}, 
    {0x8FF7, 0x8FF9, gb_chars_ban3238}, 
    {0x8FFD, 0x8FFD, gb_chars_ban3239}, 
    {0x9000, 0x9006, gb_chars_ban3240}, 
    {0x9009, 0x900B, gb_chars_ban3241}, 
    {0x900D, 0x900D, gb_chars_ban3242}, 
    {0x900F, 0x9012, gb_chars_ban3243}, 
    {0x9014, 0x9014, gb_chars_ban3244}, 
    {0x9016, 0x9017, gb_chars_ban3245}, 
    {0x901A, 0x901B, gb_chars_ban3246}, 
    {0x901D, 0x9022, gb_chars_ban3247}, 
    {0x9026, 0x9026, gb_chars_ban3248}, 
    {0x902D, 0x902F, gb_chars_ban3249}, 
    {0x9035, 0x9036, gb_chars_ban3250}, 
    {0x9038, 0x9038, gb_chars_ban3251}, 
    {0x903B, 0x903C, gb_chars_ban3252}, 
    {0x903E, 0x903E, gb_chars_ban3253}, 
    {0x9041, 0x9042, gb_chars_ban3254}, 
    {0x9044, 0x9044, gb_chars_ban3255}, 
    {0x9047, 0x9047, gb_chars_ban3256}, 
    {0x904D, 0x904D, gb_chars_ban3257}, 
    {0x904F, 0x9053, gb_chars_ban3258}, 
    {0x9057, 0x9058, gb_chars_ban3259}, 
    {0x905B, 0x905B, gb_chars_ban3260}, 
    {0x9062, 0x9063, gb_chars_ban3261}, 
    {0x9065, 0x9065, gb_chars_ban3262}, 
    {0x9068, 0x9068, gb_chars_ban3263}, 
    {0x906D, 0x906E, gb_chars_ban3264}, 
    {0x9074, 0x9075, gb_chars_ban3265}, 
    {0x907D, 0x907D, gb_chars_ban3266}, 
    {0x907F, 0x9080, gb_chars_ban3267}, 
    {0x9082, 0x9083, gb_chars_ban3268}, 
    {0x9088, 0x9088, gb_chars_ban3269}, 
    {0x908B, 0x908B, gb_chars_ban3270}, 
    {0x9091, 0x9091, gb_chars_ban3271}, 
    {0x9093, 0x9093, gb_chars_ban3272}, 
    {0x9095, 0x9095, gb_chars_ban3273}, 
    {0x9097, 0x9097, gb_chars_ban3274}, 
    {0x9099, 0x9099, gb_chars_ban3275}, 
    {0x909B, 0x909B, gb_chars_ban3276}, 
    {0x909D, 0x909D, gb_chars_ban3277}, 
    {0x90A1, 0x90A3, gb_chars_ban3278}, 
    {0x90A6, 0x90A6, gb_chars_ban3279}, 
    {0x90AA, 0x90AA, gb_chars_ban3280}, 
    {0x90AC, 0x90AC, gb_chars_ban3281}, 
    {0x90AE, 0x90B1, gb_chars_ban3282}, 
    {0x90B3, 0x90B6, gb_chars_ban3283}, 
    {0x90B8, 0x90BB, gb_chars_ban3284}, 
    {0x90BE, 0x90BE, gb_chars_ban3285}, 
    {0x90C1, 0x90C1, gb_chars_ban3286}, 
    {0x90C4, 0x90C5, gb_chars_ban3287}, 
    {0x90C7, 0x90C7, gb_chars_ban3288}, 
    {0x90CA, 0x90CA, gb_chars_ban3289}, 
    {0x90CE, 0x90D1, gb_chars_ban3290}, 
    {0x90D3, 0x90D3, gb_chars_ban3291}, 
    {0x90D7, 0x90D7, gb_chars_ban3292}, 
    {0x90DB, 0x90DD, gb_chars_ban3293}, 
    {0x90E1, 0x90E2, gb_chars_ban3294}, 
    {0x90E6, 0x90E8, gb_chars_ban3295}, 
    {0x90EB, 0x90EB, gb_chars_ban3296}, 
    {0x90ED, 0x90ED, gb_chars_ban3297}, 
    {0x90EF, 0x90EF, gb_chars_ban3298}, 
    {0x90F4, 0x90F4, gb_chars_ban3299}, 
    {0x90F8, 0x90F8, gb_chars_ban3300}, 
    {0x90FD, 0x90FE, gb_chars_ban3301}, 
    {0x9102, 0x9102, gb_chars_ban3302}, 
    {0x9104, 0x9104, gb_chars_ban3303}, 
    {0x9119, 0x9119, gb_chars_ban3304}, 
    {0x911E, 0x911E, gb_chars_ban3305}, 
    {0x9122, 0x9123, gb_chars_ban3306}, 
    {0x912F, 0x912F, gb_chars_ban3307}, 
    {0x9131, 0x9131, gb_chars_ban3308}, 
    {0x9139, 0x9139, gb_chars_ban3309}, 
    {0x9143, 0x9143, gb_chars_ban3310}, 
    {0x9146, 0x9146, gb_chars_ban3311}, 
    {0x9149, 0x9150, gb_chars_ban3312}, 
    {0x9152, 0x9152, gb_chars_ban3313}, 
    {0x9157, 0x9157, gb_chars_ban3314}, 
    {0x915A, 0x915A, gb_chars_ban3315}, 
    {0x915D, 0x915E, gb_chars_ban3316}, 
    {0x9161, 0x9165, gb_chars_ban3317}, 
    {0x9169, 0x916A, gb_chars_ban3318}, 
    {0x916C, 0x916C, gb_chars_ban3319}, 
    {0x916E, 0x9172, gb_chars_ban3320}, 
    {0x9174, 0x9179, gb_chars_ban3321}, 
    {0x917D, 0x917F, gb_chars_ban3322}, 
    {0x9185, 0x9185, gb_chars_ban3323}, 
    {0x9187, 0x9187, gb_chars_ban3324}, 
    {0x9189, 0x9189, gb_chars_ban3325}, 
    {0x918B, 0x918D, gb_chars_ban3326}, 
    {0x9190, 0x9192, gb_chars_ban3327}, 
    {0x919A, 0x919B, gb_chars_ban3328}, 
    {0x91A2, 0x91A3, gb_chars_ban3329}, 
    {0x91AA, 0x91AA, gb_chars_ban3330}, 
    {0x91AD, 0x91AF, gb_chars_ban3331}, 
    {0x91B4, 0x91B5, gb_chars_ban3332}, 
    {0x91BA, 0x91BA, gb_chars_ban3333}, 
    {0x91C7, 0x91C7, gb_chars_ban3334}, 
    {0x91C9, 0x91CA, gb_chars_ban3335}, 
    {0x91CC, 0x91CF, gb_chars_ban3336}, 
    {0x91D1, 0x91D1, gb_chars_ban3337}, 
    {0x91DC, 0x91DC, gb_chars_ban3338}, 
    {0x9274, 0x9274, gb_chars_ban3339}, 
    {0x928E, 0x928E, gb_chars_ban3340}, 
    {0x92AE, 0x92AE, gb_chars_ban3341}, 
    {0x92C8, 0x92C8, gb_chars_ban3342}, 
    {0x933E, 0x933E, gb_chars_ban3343}, 
    {0x936A, 0x936A, gb_chars_ban3344}, 
    {0x938F, 0x938F, gb_chars_ban3345}, 
    {0x93CA, 0x93CA, gb_chars_ban3346}, 
    {0x93D6, 0x93D6, gb_chars_ban3347}, 
    {0x943E, 0x943E, gb_chars_ban3348}, 
    {0x946B, 0x946B, gb_chars_ban3349}, 
    {0x9485, 0x9490, gb_chars_ban3350}, 
    {0x9492, 0x9495, gb_chars_ban3351}, 
    {0x9497, 0x9497, gb_chars_ban3352}, 
    {0x9499, 0x94C6, gb_chars_ban3353}, 
    {0x94C8, 0x94CE, gb_chars_ban3354}, 
    {0x94D0, 0x94D2, gb_chars_ban3355}, 
    {0x94D5, 0x94D9, gb_chars_ban3356}, 
    {0x94DB, 0x94E5, gb_chars_ban3357}, 
    {0x94E7, 0x94FA, gb_chars_ban3358}, 
    {0x94FC, 0x951B, gb_chars_ban3359}, 
    {0x951D, 0x951F, gb_chars_ban3360}, 
    {0x9521, 0x9526, gb_chars_ban3361}, 
    {0x9528, 0x9532, gb_chars_ban3362}, 
    {0x9534, 0x953C, gb_chars_ban3363}, 
    {0x953E, 0x9542, gb_chars_ban3364}, 
    {0x9544, 0x9547, gb_chars_ban3365}, 
    {0x9549, 0x954A, gb_chars_ban3366}, 
    {0x954C, 0x9554, gb_chars_ban3367}, 
    {0x9556, 0x9559, gb_chars_ban3368}, 
    {0x955B, 0x955F, gb_chars_ban3369}, 
    {0x9561, 0x956D, gb_chars_ban3370}, 
    {0x956F, 0x9573, gb_chars_ban3371}, 
    {0x9576, 0x9576, gb_chars_ban3372}, 
    {0x957F, 0x957F, gb_chars_ban3373}, 
    {0x95E8, 0x95EB, gb_chars_ban3374}, 
    {0x95ED, 0x95FE, gb_chars_ban3375}, 
    {0x9600, 0x9606, gb_chars_ban3376}, 
    {0x9608, 0x9612, gb_chars_ban3377}, 
    {0x9614, 0x9617, gb_chars_ban3378}, 
    {0x9619, 0x961A, gb_chars_ban3379}, 
    {0x961C, 0x961D, gb_chars_ban3380}, 
    {0x961F, 0x961F, gb_chars_ban3381}, 
    {0x9621, 0x9622, gb_chars_ban3382}, 
    {0x962A, 0x962A, gb_chars_ban3383}, 
    {0x962E, 0x962E, gb_chars_ban3384}, 
    {0x9631, 0x9636, gb_chars_ban3385}, 
    {0x963B, 0x963D, gb_chars_ban3386}, 
    {0x963F, 0x9640, gb_chars_ban3387}, 
    {0x9642, 0x9642, gb_chars_ban3388}, 
    {0x9644, 0x9649, gb_chars_ban3389}, 
    {0x964B, 0x964D, gb_chars_ban3390}, 
    {0x9650, 0x9650, gb_chars_ban3391}, 
    {0x9654, 0x9655, gb_chars_ban3392}, 
    {0x965B, 0x965B, gb_chars_ban3393}, 
    {0x965F, 0x965F, gb_chars_ban3394}, 
    {0x9661, 0x9662, gb_chars_ban3395}, 
    {0x9664, 0x9664, gb_chars_ban3396}, 
    {0x9667, 0x966A, gb_chars_ban3397}, 
    {0x966C, 0x966C, gb_chars_ban3398}, 
    {0x9672, 0x9672, gb_chars_ban3399}, 
    {0x9674, 0x9677, gb_chars_ban3400}, 
    {0x9685, 0x9686, gb_chars_ban3401}, 
    {0x9688, 0x9688, gb_chars_ban3402}, 
    {0x968B, 0x968B, gb_chars_ban3403}, 
    {0x968D, 0x968D, gb_chars_ban3404}, 
    {0x968F, 0x9690, gb_chars_ban3405}, 
    {0x9694, 0x9694, gb_chars_ban3406}, 
    {0x9697, 0x9699, gb_chars_ban3407}, 
    {0x969C, 0x969C, gb_chars_ban3408}, 
    {0x96A7, 0x96A7, gb_chars_ban3409}, 
    {0x96B0, 0x96B0, gb_chars_ban3410}, 
    {0x96B3, 0x96B3, gb_chars_ban3411}, 
    {0x96B6, 0x96B6, gb_chars_ban3412}, 
    {0x96B9, 0x96B9, gb_chars_ban3413}, 
    {0x96BC, 0x96BE, gb_chars_ban3414}, 
    {0x96C0, 0x96C1, gb_chars_ban3415}, 
    {0x96C4, 0x96C7, gb_chars_ban3416}, 
    {0x96C9, 0x96C9, gb_chars_ban3417}, 
    {0x96CC, 0x96CF, gb_chars_ban3418}, 
    {0x96D2, 0x96D2, gb_chars_ban3419}, 
    {0x96D5, 0x96D5, gb_chars_ban3420}, 
    {0x96E0, 0x96E0, gb_chars_ban3421}, 
    {0x96E8, 0x96EA, gb_chars_ban3422}, 
    {0x96EF, 0x96EF, gb_chars_ban3423}, 
    {0x96F3, 0x96F3, gb_chars_ban3424}, 
    {0x96F6, 0x96F7, gb_chars_ban3425}, 
    {0x96F9, 0x96F9, gb_chars_ban3426}, 
    {0x96FE, 0x96FE, gb_chars_ban3427}, 
    {0x9700, 0x9701, gb_chars_ban3428}, 
    {0x9704, 0x9704, gb_chars_ban3429}, 
    {0x9706, 0x9709, gb_chars_ban3430}, 
    {0x970D, 0x970F, gb_chars_ban3431}, 
    {0x9713, 0x9713, gb_chars_ban3432}, 
    {0x9716, 0x9716, gb_chars_ban3433}, 
    {0x971C, 0x971C, gb_chars_ban3434}, 
    {0x971E, 0x971E, gb_chars_ban3435}, 
    {0x972A, 0x972A, gb_chars_ban3436}, 
    {0x972D, 0x972D, gb_chars_ban3437}, 
    {0x9730, 0x9730, gb_chars_ban3438}, 
    {0x9732, 0x9732, gb_chars_ban3439}, 
    {0x9738, 0x9739, gb_chars_ban3440}, 
    {0x973E, 0x973E, gb_chars_ban3441}, 
    {0x9752, 0x9753, gb_chars_ban3442}, 
    {0x9756, 0x9756, gb_chars_ban3443}, 
    {0x9759, 0x9759, gb_chars_ban3444}, 
    {0x975B, 0x975B, gb_chars_ban3445}, 
    {0x975E, 0x975E, gb_chars_ban3446}, 
    {0x9760, 0x9762, gb_chars_ban3447}, 
    {0x9765, 0x9765, gb_chars_ban3448}, 
    {0x9769, 0x9769, gb_chars_ban3449}, 
    {0x9773, 0x9774, gb_chars_ban3450}, 
    {0x9776, 0x9776, gb_chars_ban3451}, 
    {0x977C, 0x977C, gb_chars_ban3452}, 
    {0x9785, 0x9785, gb_chars_ban3453}, 
    {0x978B, 0x978B, gb_chars_ban3454}, 
    {0x978D, 0x978D, gb_chars_ban3455}, 
    {0x9791, 0x9792, gb_chars_ban3456}, 
    {0x9794, 0x9794, gb_chars_ban3457}, 
    {0x9798, 0x9798, gb_chars_ban3458}, 
    {0x97A0, 0x97A0, gb_chars_ban3459}, 
    {0x97A3, 0x97A3, gb_chars_ban3460}, 
    {0x97AB, 0x97AB, gb_chars_ban3461}, 
    {0x97AD, 0x97AD, gb_chars_ban3462}, 
    {0x97AF, 0x97AF, gb_chars_ban3463}, 
    {0x97B2, 0x97B2, gb_chars_ban3464}, 
    {0x97B4, 0x97B4, gb_chars_ban3465}, 
    {0x97E6, 0x97E7, gb_chars_ban3466}, 
    {0x97E9, 0x97ED, gb_chars_ban3467}, 
    {0x97F3, 0x97F3, gb_chars_ban3468}, 
    {0x97F5, 0x97F6, gb_chars_ban3469}, 
    {0x9875, 0x988A, gb_chars_ban3470}, 
    {0x988C, 0x988D, gb_chars_ban3471}, 
    {0x988F, 0x9891, gb_chars_ban3472}, 
    {0x9893, 0x9894, gb_chars_ban3473}, 
    {0x9896, 0x9898, gb_chars_ban3474}, 
    {0x989A, 0x98A2, gb_chars_ban3475}, 
    {0x98A4, 0x98A7, gb_chars_ban3476}, 
    {0x98CE, 0x98CE, gb_chars_ban3477}, 
    {0x98D1, 0x98D3, gb_chars_ban3478}, 
    {0x98D5, 0x98D5, gb_chars_ban3479}, 
    {0x98D8, 0x98DA, gb_chars_ban3480}, 
    {0x98DE, 0x98DF, gb_chars_ban3481}, 
    {0x98E7, 0x98E8, gb_chars_ban3482}, 
    {0x990D, 0x990D, gb_chars_ban3483}, 
    {0x9910, 0x9910, gb_chars_ban3484}, 
    {0x992E, 0x992E, gb_chars_ban3485}, 
    {0x9954, 0x9955, gb_chars_ban3486}, 
    {0x9963, 0x9963, gb_chars_ban3487}, 
    {0x9965, 0x9965, gb_chars_ban3488}, 
    {0x9967, 0x9972, gb_chars_ban3489}, 
    {0x9974, 0x9977, gb_chars_ban3490}, 
    {0x997A, 0x997A, gb_chars_ban3491}, 
    {0x997C, 0x997D, gb_chars_ban3492}, 
    {0x997F, 0x9981, gb_chars_ban3493}, 
    {0x9984, 0x9988, gb_chars_ban3494}, 
    {0x998A, 0x998B, gb_chars_ban3495}, 
    {0x998D, 0x998D, gb_chars_ban3496}, 
    {0x998F, 0x9999, gb_chars_ban3497}, 
    {0x99A5, 0x99A5, gb_chars_ban3498}, 
    {0x99A8, 0x99A8, gb_chars_ban3499}, 
    {0x9A6C, 0x9A71, gb_chars_ban3500}, 
    {0x9A73, 0x9A82, gb_chars_ban3501}, 
    {0x9A84, 0x9A88, gb_chars_ban3502}, 
    {0x9A8A, 0x9A8C, gb_chars_ban3503}, 
    {0x9A8F, 0x9A93, gb_chars_ban3504}, 
    {0x9A96, 0x9A98, gb_chars_ban3505}, 
    {0x9A9A, 0x9AA5, gb_chars_ban3506}, 
    {0x9AA7, 0x9AA8, gb_chars_ban3507}, 
    {0x9AB0, 0x9AB1, gb_chars_ban3508}, 
    {0x9AB6, 0x9AB8, gb_chars_ban3509}, 
    {0x9ABA, 0x9ABA, gb_chars_ban3510}, 
    {0x9ABC, 0x9ABC, gb_chars_ban3511}, 
    {0x9AC0, 0x9AC2, gb_chars_ban3512}, 
    {0x9AC5, 0x9AC5, gb_chars_ban3513}, 
    {0x9ACB, 0x9ACC, gb_chars_ban3514}, 
    {0x9AD1, 0x9AD1, gb_chars_ban3515}, 
    {0x9AD3, 0x9AD3, gb_chars_ban3516}, 
    {0x9AD8, 0x9AD8, gb_chars_ban3517}, 
    {0x9ADF, 0x9ADF, gb_chars_ban3518}, 
    {0x9AE1, 0x9AE1, gb_chars_ban3519}, 
    {0x9AE6, 0x9AE6, gb_chars_ban3520}, 
    {0x9AEB, 0x9AEB, gb_chars_ban3521}, 
    {0x9AED, 0x9AED, gb_chars_ban3522}, 
    {0x9AEF, 0x9AEF, gb_chars_ban3523}, 
    {0x9AF9, 0x9AF9, gb_chars_ban3524}, 
    {0x9AFB, 0x9AFB, gb_chars_ban3525}, 
    {0x9B03, 0x9B03, gb_chars_ban3526}, 
    {0x9B08, 0x9B08, gb_chars_ban3527}, 
    {0x9B0F, 0x9B0F, gb_chars_ban3528}, 
    {0x9B13, 0x9B13, gb_chars_ban3529}, 
    {0x9B1F, 0x9B1F, gb_chars_ban3530}, 
    {0x9B23, 0x9B23, gb_chars_ban3531}, 
    {0x9B2F, 0x9B2F, gb_chars_ban3532}, 
    {0x9B32, 0x9B32, gb_chars_ban3533}, 
    {0x9B3B, 0x9B3C, gb_chars_ban3534}, 
    {0x9B41, 0x9B45, gb_chars_ban3535}, 
    {0x9B47, 0x9B49, gb_chars_ban3536}, 
    {0x9B4D, 0x9B4D, gb_chars_ban3537}, 
    {0x9B4F, 0x9B4F, gb_chars_ban3538}, 
    {0x9B51, 0x9B51, gb_chars_ban3539}, 
    {0x9B54, 0x9B54, gb_chars_ban3540}, 
    {0x9C7C, 0x9C7C, gb_chars_ban3541}, 
    {0x9C7F, 0x9C7F, gb_chars_ban3542}, 
    {0x9C81, 0x9C82, gb_chars_ban3543}, 
    {0x9C85, 0x9C88, gb_chars_ban3544}, 
    {0x9C8B, 0x9C8B, gb_chars_ban3545}, 
    {0x9C8D, 0x9C8E, gb_chars_ban3546}, 
    {0x9C90, 0x9C92, gb_chars_ban3547}, 
    {0x9C94, 0x9C95, gb_chars_ban3548}, 
    {0x9C9A, 0x9C9C, gb_chars_ban3549}, 
    {0x9C9E, 0x9CA9, gb_chars_ban3550}, 
    {0x9CAB, 0x9CAB, gb_chars_ban3551}, 
    {0x9CAD, 0x9CAE, gb_chars_ban3552}, 
    {0x9CB0, 0x9CB8, gb_chars_ban3553}, 
    {0x9CBA, 0x9CBD, gb_chars_ban3554}, 
    {0x9CC3, 0x9CC7, gb_chars_ban3555}, 
    {0x9CCA, 0x9CD0, gb_chars_ban3556}, 
    {0x9CD3, 0x9CD9, gb_chars_ban3557}, 
    {0x9CDC, 0x9CDF, gb_chars_ban3558}, 
    {0x9CE2, 0x9CE2, gb_chars_ban3559}, 
    {0x9E1F, 0x9E23, gb_chars_ban3560}, 
    {0x9E25, 0x9E26, gb_chars_ban3561}, 
    {0x9E28, 0x9E2D, gb_chars_ban3562}, 
    {0x9E2F, 0x9E2F, gb_chars_ban3563}, 
    {0x9E31, 0x9E33, gb_chars_ban3564}, 
    {0x9E35, 0x9E3A, gb_chars_ban3565}, 
    {0x9E3D, 0x9E3F, gb_chars_ban3566}, 
    {0x9E41, 0x9E4C, gb_chars_ban3567}, 
    {0x9E4E, 0x9E4F, gb_chars_ban3568}, 
    {0x9E51, 0x9E51, gb_chars_ban3569}, 
    {0x9E55, 0x9E55, gb_chars_ban3570}, 
    {0x9E57, 0x9E58, gb_chars_ban3571}, 
    {0x9E5A, 0x9E5C, gb_chars_ban3572}, 
    {0x9E5E, 0x9E5E, gb_chars_ban3573}, 
    {0x9E63, 0x9E64, gb_chars_ban3574}, 
    {0x9E66, 0x9E6D, gb_chars_ban3575}, 
    {0x9E70, 0x9E71, gb_chars_ban3576}, 
    {0x9E73, 0x9E73, gb_chars_ban3577}, 
    {0x9E7E, 0x9E7F, gb_chars_ban3578}, 
    {0x9E82, 0x9E82, gb_chars_ban3579}, 
    {0x9E87, 0x9E88, gb_chars_ban3580}, 
    {0x9E8B, 0x9E8B, gb_chars_ban3581}, 
    {0x9E92, 0x9E93, gb_chars_ban3582}, 
    {0x9E9D, 0x9E9D, gb_chars_ban3583}, 
    {0x9E9F, 0x9E9F, gb_chars_ban3584}, 
    {0x9EA6, 0x9EA6, gb_chars_ban3585}, 
    {0x9EB4, 0x9EB4, gb_chars_ban3586}, 
    {0x9EB8, 0x9EB8, gb_chars_ban3587}, 
    {0x9EBB, 0x9EBB, gb_chars_ban3588}, 
    {0x9EBD, 0x9EBE, gb_chars_ban3589}, 
    {0x9EC4, 0x9EC4, gb_chars_ban3590}, 
    {0x9EC9, 0x9EC9, gb_chars_ban3591}, 
    {0x9ECD, 0x9ECF, gb_chars_ban3592}, 
    {0x9ED1, 0x9ED1, gb_chars_ban3593}, 
    {0x9ED4, 0x9ED4, gb_chars_ban3594}, 
    {0x9ED8, 0x9ED8, gb_chars_ban3595}, 
    {0x9EDB, 0x9EDD, gb_chars_ban3596}, 
    {0x9EDF, 0x9EE0, gb_chars_ban3597}, 
    {0x9EE2, 0x9EE2, gb_chars_ban3598}, 
    {0x9EE5, 0x9EE5, gb_chars_ban3599}, 
    {0x9EE7, 0x9EE7, gb_chars_ban3600}, 
    {0x9EE9, 0x9EEA, gb_chars_ban3601}, 
    {0x9EEF, 0x9EEF, gb_chars_ban3602}, 
    {0x9EF9, 0x9EF9, gb_chars_ban3603}, 
    {0x9EFB, 0x9EFC, gb_chars_ban3604}, 
    {0x9EFE, 0x9EFE, gb_chars_ban3605}, 
    {0x9F0B, 0x9F0B, gb_chars_ban3606}, 
    {0x9F0D, 0x9F0E, gb_chars_ban3607}, 
    {0x9F10, 0x9F10, gb_chars_ban3608}, 
    {0x9F13, 0x9F13, gb_chars_ban3609}, 
    {0x9F17, 0x9F17, gb_chars_ban3610}, 
    {0x9F19, 0x9F19, gb_chars_ban3611}, 
    {0x9F20, 0x9F20, gb_chars_ban3612}, 
    {0x9F22, 0x9F22, gb_chars_ban3613}, 
    {0x9F2C, 0x9F2C, gb_chars_ban3614}, 
    {0x9F2F, 0x9F2F, gb_chars_ban3615}, 
    {0x9F37, 0x9F37, gb_chars_ban3616}, 
    {0x9F39, 0x9F39, gb_chars_ban3617}, 
    {0x9F3B, 0x9F3B, gb_chars_ban3618}, 
    {0x9F3D, 0x9F3E, gb_chars_ban3619}, 
    {0x9F44, 0x9F44, gb_chars_ban3620}, 
    {0x9F50, 0x9F51, gb_chars_ban3621}, 
    {0x9F7F, 0x9F80, gb_chars_ban3622}, 
    {0x9F83, 0x9F8C, gb_chars_ban3623}, 
    {0x9F99, 0x9F9B, gb_chars_ban3624}, 
    {0x9F9F, 0x9FA0, gb_chars_ban3625}, 
    {0xFF01, 0xFF5E, gb_chars_ban3626}, 
    {0xFFE0, 0xFFE1, gb_chars_ban3627}, 
    {0xFFE3, 0xFFE3, gb_chars_ban3628}, 
    {0xFFE5, 0xFFE5, gb_chars_ban3629}, 
};
#define NR_BYTES_PER_CHAR 2

/* using binary search */
const unsigned char* __mg_map_uc16_to_gb (unsigned short uc16)
{
    int low, high, mid;

    if (uc16 < uc16_min || uc16 > uc16_max)
        return NULL;

    low = 0;
    high = sizeof(uc16_bans)/sizeof(uc16_bans[0]) - 1;

    while (low <= high) {
        unsigned short start, end;

        mid = (low + high) / 2;

        start = uc16_bans [mid].start;
        end = uc16_bans [mid].end;

        if (uc16 >= start && uc16 <= end) {
                return uc16_bans [mid].gb_chars + ((uc16 - start) * NR_BYTES_PER_CHAR);
        }
        else if (uc16 < start) {
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }

    return NULL;
}

#endif /* _MGCHARSET_UNICODE */

#endif /* _MGCHARSET_GB */

