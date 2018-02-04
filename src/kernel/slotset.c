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
** slotset.c: maintain the slot set in a byte arrary.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 2005/08/16
*/

int __mg_lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set)
{
    int unused = 0;
    int i, j;

    for (i = 0; i < len_bmp; i++) {
        for (j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j)) {
                if (set)
                    *bitmap &= (~(0x80 >> j));
                return unused + j;
            }
        }

        unused += 8;
        bitmap++;
    }

    return -1;
}

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

