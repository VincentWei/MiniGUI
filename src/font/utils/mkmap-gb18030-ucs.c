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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct _local_to_ucs
{
    unsigned int local;
    unsigned int ucs;
} local_to_ucs [65536];

static unsigned short* gb18030_ucs_map;

static int gb18030_index (unsigned int encoding)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    if (encoding < 0xFFFF) {
        ch1 = encoding >> 8;
        ch2 = encoding;
        if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
            return ((ch1 - 0x81) * 192 + (ch2 - 0x40));
        else
            return -1;
    }
    else {
        ch1 = encoding >> 24;
        ch2 = encoding >> 16;
        ch3 = encoding >> 8;
        ch4 = encoding;
        if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 <= 0x39
                && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE) {
            return ((126 * 192) + 
                ((ch1 - 0x81) * 12600 + (ch2 - 0x30) * 1260 + 
                (ch3 - 0x81) * 10 + (ch4 - 0x30)));
        }
        else {
            fprintf (stderr, "bad char: %x, %x, %x, %x\n", ch1, ch2, ch3, ch4);
            return -1;
        }
    }
}

int main (void)
{
    int i, count;
    int max_index, index;
    FILE* fp;

    fp = fopen ("gb18030-unicode.txt", "r");
    if (fp == NULL) {
        fprintf (stderr, "mkmap-gb18030-ucs: can not open 'gb18030-unicode.txt' for reading.\n");
        return 1;
    }

    max_index = 0;
    count = 0;
    do {
        if (fscanf (fp, "%x %x\n", &local_to_ucs[count].local, &local_to_ucs[count].ucs) == EOF)
            break;

        index = gb18030_index (local_to_ucs[count].local);
        if (index < 0) {
            fprintf (stderr, "mkmap-gb18030-ucs: bad local char: %x\n", local_to_ucs[count].local);
        }

        if (index > max_index)
            max_index = index;

        count ++;
    } while (1);

    fclose (fp);

    printf ("mkmap-gb18030-ucs: read total %d local characters, max index: %d\n", count, max_index);

    gb18030_ucs_map = calloc (max_index, sizeof (unsigned short));
    if (gb18030_ucs_map == NULL) {
        fprintf (stderr, "mkmap-gb18030-ucs: insufficiant memory\n");
        return 2;
    }

    for (i = 0; i < count; i++) {
        index = gb18030_index (local_to_ucs[i].local);
        if (index >= 0) {
            if (gb18030_ucs_map [index] == 0)
                gb18030_ucs_map [index] = local_to_ucs[i].ucs;
            else
                fprintf (stderr, "mkmap-gb18030-ucs: duplicated index of local char: %x (%d)\n", 
                    local_to_ucs[i].local, index);
        }
    }

    count = 0;
    for (i = 0; i < max_index; i++) {
        if (gb18030_ucs_map [i] == 0) {
            gb18030_ucs_map [i] = 0x3000;
            count ++;
        }
    }
    printf ("mkmap-gb18030-ucs: there are %d empty slots in the map.\n", count);

    if ((fp = fopen ("gb18030unimap.c", "w+")) == NULL) {
        fprintf (stderr, "mkmap-gb18030-ucs: can not open 'gb18030unimap.c' for writing.\n");
        return 3;
    }

    fprintf (fp, "const unsigned short __mg_gb18030_0_unicode_map [] = {");

    for (i = 0; i <= max_index; i++) {
        if (i % 3 == 0)
            fprintf (fp, "\n    ");
        fprintf (fp, "0x%x, ", gb18030_ucs_map [i]);
    }

    fprintf (fp, "\n};\n");

    fclose (fp);

    free (gb18030_ucs_map);

    return 0;
}

