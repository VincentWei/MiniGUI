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
#include <limits.h>

#include "../gbunimap.c"

#define IS_GB2312_CHAR(ch1, ch2) \
        if (((ch1 >= 0xA1 && ch1 <= 0xA9) || (ch1 >= 0xB0 && ch1 <= 0xF7)) \
                        && ch2 >= 0xA1 && ch2 <= 0xFE)

static int gb_index (unsigned char ch1, unsigned char ch2)
{
    int area;

    area = ch1 - 0xA1;
    IS_GB2312_CHAR(ch1, ch2) {
        if (area < 9) {
            return (area * 94 + ch2 - 0xA1);
        }
        else if (area >= 15)
            return ((area - 6)* 94 + ch2 - 0xA1);
    }

    return -1;
}

struct _gb_char {
    unsigned char ch1, ch2;
} gb_char [USHRT_MAX + 1];

struct _uc16_bans {
    unsigned short start, end;
} uc16_bans [USHRT_MAX + 1];

int main (int argc, char* argv[])
{
    unsigned char ch1, ch2;
    int index;
    unsigned short uc16, ban, uc16_min = USHRT_MAX, uc16_max = 0;

    for (ch1 = 0xA1; ch1 < 0xF8; ch1++) {
        for (ch2 = 0xA1; ch2 < 0xFF; ch2++) {
            index = gb_index (ch1, ch2);
            if (index >= 0) {
                uc16 = gbunicode_map [index];

                if (gb_char [uc16].ch1 || gb_char [uc16].ch2) {
                    fprintf (stderr, "WARNING: duplicated value 0x%04X.\n", uc16);
                }

                gb_char [uc16].ch1 = ch1;
                gb_char [uc16].ch2 = ch2;

                if (uc16 < uc16_min)
                    uc16_min = uc16;
                if (uc16 > uc16_max)
                    uc16_max = uc16;
            }
        }
    }

    fprintf (stderr, "Minimal unicode: 0x%04X, Maximal unicode: 0x%04X.\n", uc16_min, uc16_max);

    printf ("/* This is a machine-made file, do not modify. */\n");
    printf ("#include <stdlib.h>\n\n");

    printf ("static unsigned short uc16_min = 0x%04X;\n"
            "static unsigned short uc16_max = 0x%04X;\n", uc16_min, uc16_max);
    printf ("\n");

    ban = 0;
    for (index = 0; index <= USHRT_MAX; index++) {
        if (gb_char [index].ch1 && gb_char [index].ch2) {
            if (uc16_bans [ban].start) {
                uc16_bans [ban].end = index;
            }
            else {
                uc16_bans [ban].start = index;
                uc16_bans [ban].end = index;
            }
        }
        else {
            if (uc16_bans [ban].start) {
                fprintf (stderr, "uc16_ban [%d]: (0x%04X, 0x%04X).\n", ban, uc16_bans [ban].start, uc16_bans [ban].end);
                ban++;
            }
        }
    }

    /* output the gb chars of bans */
    for (index = 0; index < ban; index ++) {
        printf ("static const unsigned char gb_chars_ban%d [] = {", index);
        for (uc16 = uc16_bans [index].start; uc16 <= uc16_bans [index].end; uc16++) {
            if ((uc16 - uc16_bans [index].start) % 5 == 0) {
                printf ("\n    ");
            }
            printf ("0x%02x, 0x%02x, ", gb_char [uc16].ch1, gb_char [uc16].ch2);
        }
        printf ("\n};\n", index);
    }
    printf ("\n");

    /* output the uc16 bans */
    printf ("static struct _uc16_bans {\n");
    printf ("    unsigned short start, end;\n");
    printf ("    const unsigned char* gb_chars;\n");
    printf ("} uc16_bans [] = {\n");

    for (index = 0; index < ban; index ++) {
        printf ("    {0x%04X, 0x%04X, gb_chars_ban%d}, \n",
                uc16_bans [index].start, uc16_bans [index].end, index);
    }
    printf ("};\n");

    printf ("#define NR_BYTES_PER_CHAR %d\n", sizeof (struct _gb_char));
    printf ("\n");
    printf ("/* using binary search */\n");
    printf ("const unsigned char* __mg_map_uc16_to_gb (unsigned short uc16)\n");
    printf ("{\n");
    printf ("    int low, high, mid;\n");
    printf ("\n");
    printf ("    if (uc16 < uc16_min || uc16 > uc16_max)\n");
    printf ("        return NULL;\n");
    printf ("\n");
    printf ("    low = 0;\n");
    printf ("    high = sizeof(uc16_bans)/sizeof(uc16_bans[0]) - 1;\n");
    printf ("\n");
    printf ("    while (low <= high) {\n");
    printf ("        unsigned short start, end;\n");
    printf ("\n");
    printf ("        mid = (low + high) / 2;\n");
    printf ("\n");
    printf ("        start = uc16_bans [mid].start;\n");
    printf ("        end = uc16_bans [mid].end;\n");
    printf ("\n");
    printf ("        if (uc16 >= start && uc16 <= end) {\n");
    printf ("                return uc16_bans [mid].gb_chars + ((uc16 - start) * NR_BYTES_PER_CHAR);\n");
    printf ("        }\n");
    printf ("        else if (uc16 < uc16_bans [mid].start) {\n");
    printf ("            high = mid - 1;\n");
    printf ("        }\n");
    printf ("        else {\n");
    printf ("            low = mid + 1;\n");
    printf ("        }\n");
    printf ("    }\n");
    printf ("\n");
    printf ("    return NULL;\n");
    printf ("}\n\n");

    return 0;
}

