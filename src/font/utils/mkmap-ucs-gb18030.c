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

static struct _gb18030_char {
    int encoding;
} gb18030_char [USHRT_MAX + 1];

int main (int argc, char* argv[])
{
    int count, index;
    int uc16_min, uc16_max;
    FILE* fp;

    fp = fopen ("gb18030-unicode.txt", "r");
    if (fp == NULL) {
        fprintf (stderr, "mkmap-gb18030-ucs: can not open 'gb18030-unicode.txt' for reading.\n");
        return 1;
    }

    uc16_min = USHRT_MAX, uc16_max = 0;
    count = 0;
    do {
        int encoding, uc16;
        if (fscanf (fp, "%x %x\n", &encoding, &uc16) == EOF)
            break;

        gb18030_char [uc16].encoding = encoding;

        if (uc16 < uc16_min)
            uc16_min = uc16;
        if (uc16 > uc16_max)
            uc16_max = uc16;

        count ++;
    } while (1);

    fclose (fp);

    fprintf (stderr, "makeuc16-gb18030-map-li: Minimal unicode: 0x%04X, Maximal unicode: 0x%04X, count: %d.\n", 
            uc16_min, uc16_max, count);

    printf ("/* This is a machine-made file, do not modify. */\n");
    printf ("#include <stdlib.h>\n\n");

    printf ("static int ucs_min = 0x%04X;\n"
            "static int ucs_max = 0x%04X;\n", uc16_min, uc16_max);
    printf ("\n");

    /* output the gb18030 encodings */
    printf ("static const unsigned int gb18030_chars [] = {");
    for (index = uc16_min; index <= uc16_max; index ++) {
        if ((index - uc16_min) % 5 == 0) {
                printf ("\n    ");
        }
        printf ("0x%08x, ", gb18030_char [index].encoding);
    }
    printf ("\n};\n");
    printf ("\n");

    printf ("/* using linear search */\n");
    printf ("int __mg_map_ucs_to_gb18030 (int wc, unsigned char* mchar)\n");
    printf ("{\n");
    printf ("    int idx;\n");
    printf ("    unsigned int encoding;\n");
    printf ("\n");
    printf ("    if (wc < ucs_min || wc > ucs_max)\n");
    printf ("        return 0;\n");
    printf ("    \n");
    printf ("    idx = wc - ucs_min;\n");
    printf ("    encoding = gb18030_chars [idx];\n");
    printf ("\n");
    printf ("    if (encoding < 0xFFFF) {\n");
    printf ("        mchar[0] = encoding >> 8;\n");
    printf ("        mchar[1] = encoding;\n");
    printf ("        return 2;\n");
    printf ("    }\n");
    printf ("\n");
    printf ("    mchar[0] = encoding >> 24;\n");
    printf ("    mchar[1] = encoding >> 16;\n");
    printf ("    mchar[2] = encoding >> 8;\n");
    printf ("    mchar[3] = encoding;\n");
    printf ("    return 4;\n");
    printf ("}\n");

    return 0;
}

