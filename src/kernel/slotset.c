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
** slotset.c: maintain the slot set in a byte arrary.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2005/08/16
*/

#include <stdlib.h>

size_t __mg_lookfor_unused_slot (unsigned char* bitmap, size_t len_bmp, int set)
{
    size_t unused = 0;
    size_t i, j;

    for (i = 0; i < len_bmp; i++) {
        if (*bitmap) {
            unsigned char half_byte = *bitmap & 0xF0;
            unsigned char test_byte = 0x80;

            if (half_byte) {
                for (j = 0; j < 4; j++) {
                    if (half_byte & test_byte) {
                        if (set)
                            *bitmap ^= test_byte;
                        return unused + j;
                    }

                    test_byte = test_byte >> 1;
                }
            }

            half_byte = *bitmap & 0x0F;
            test_byte = 0x08;
            for (j = 4; j < 8; j++) {
                if (half_byte & test_byte) {
                    if (set)
                        *bitmap ^= test_byte;
                    return unused + j;
                }

                test_byte = test_byte >> 1;
            }
        }

        unused += 8;
        bitmap++;
    }

    return -1;
}

#if 0 // inline since 5.0.0
void __mg_slot_set_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    *bitmap &= (~(0x80 >> (index % 8)));
}

int __mg_slot_clear_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    if (*bitmap & (0x80 >> (index % 8)))
        return 0;

    *bitmap |= (0x80 >> (index % 8));
    return 1;
}
#endif

