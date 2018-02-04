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
#include <ctype.h>
#include <string.h>

#include <minigui/common.h>

#define MAX_NR_PINYIN   10

typedef struct _gbhz_pinyin
{
    unsigned short encoding;
    unsigned long pinyin [MAX_NR_PINYIN];
} GBHZ_PINYIN;

#define MAX_LEN_PINYIN  10

typedef struct _pinyin
{
    char pinyin [MAX_LEN_PINYIN];
} PINYIN;

#define NR_GBHZ     ((0xFF-0xB0) * (0xFF-0xA1))

static int max_index;
static GBHZ_PINYIN gbhz_py_tab [NR_GBHZ];

static int nr_py;
static PINYIN py_tab [1024];

static void output_py_tab (void)
{
    int i;

    printf ("static int nr_pinyin = %d;\n"
            "static const char* py_tab [] = {\n", nr_py);

    for (i = 0; i < nr_py; i++) {
        printf ("\t\"%s\",\n", py_tab[i].pinyin);
    }

    printf ("};\n\n");
}

static void output_gbhz_py_tab (void)
{
    int i;

    printf ("#define MAX_NR_PINYIN\t10\n\n");

    printf ("typedef struct _gbhz_pinyin\n");
    printf ("{\n");
    printf ("\tunsigned short encoding;\n");
    printf ("\tunsigned short def_py;\n");
    printf ("\tunsigned long pinyin [MAX_NR_PINYIN];\n");
    printf ("} GBHZ_PINYIN;\n\n");

    printf ("static int max_index = %d;\n", max_index);
    printf ("static GBHZ_PINYIN gbhz_py_tab [NR_GBHZ] = {\n");

    for (i = 0; i <= max_index; i++) {
        int j;
        GBHZ_PINYIN* p = gbhz_py_tab + i;

        printf ("\t{0x%04X /* %c%c */, 0, {", p->encoding, HIBYTE_WORD16 (p->encoding), LOBYTE_WORD16 (p->encoding));
        for (j = 0; j < MAX_NR_PINYIN; j++) {
            int no_py, tone;
            if (p->pinyin [j] == 0)
                break;
            printf ("%lu", (p->pinyin [j] << 16) | p->encoding);
            no_py = p->pinyin [j] >> 6;
            tone = p->pinyin [j] & 0x003F;
            printf (" /* %s%d */, ", py_tab [no_py].pinyin, tone);
        }
        printf ("}},\n");
    }

    printf ("};\n\n");
}

static void output_funcs (void)
{
    printf ("\n");

    printf ("#define NR_GBHZ     ((0xFF-0xB0) * (0xFF-0xA1))\n");
    printf ("#define PINYIN_NO(pinyin)   (pinyin >> 6)\n");
    printf ("#define PINYIN_TONE(pinyin) (pinyin & 0x003F)\n");
    printf ("\n");

    printf ("static int gbhz_index (unsigned char ch1, unsigned char ch2)\n");
    printf ("{\n");
    printf ("\treturn ((ch1 - 0xB0) * 94 + ch2 - 0xA1);\n");
    printf ("}\n\n");
}

static int gbhz_index (unsigned char ch1, unsigned char ch2)
{
    return ((ch1 - 0xB0) * 94 + ch2 - 0xA1);
}

static int set_gbhz_pinyin (unsigned char ch1, unsigned char ch2, unsigned short no_py, int tone)
{
    int i, index;
    GBHZ_PINYIN* p;

    index = gbhz_index (ch1, ch2);

    if (index < 0 || index >= NR_GBHZ)
        return -1;

    if (index > max_index)
        max_index = index;

    p = gbhz_py_tab + index;

    p->encoding = MAKEWORD16 (ch2, ch1);
    for (i = 0; i < MAX_NR_PINYIN; i++) {
        if (p->pinyin [i] == 0) {
            p->pinyin [i] = no_py;
            p->pinyin [i] <<= 6;
            p->pinyin [i] |= (tone & 0x03F);
            break;
        }
    }

    return 0;
}

int main (int argc, char* argv[])
{
    FILE* fp;
    char c, *tmp;
    char line [1024];

    if ((fp = fopen ("pinyin.gb", "r")) == NULL)
        return 1;

    while (feof (fp) == 0) {
        fscanf (fp, "%c", &c);
        if (isalpha (c)) {
            fgets (line + 1, 1023, fp);
            line [0] = c;

            tmp = index (line, ',');
            *tmp = '\0';
            strcpy (py_tab [nr_py].pinyin, line);
            nr_py ++;
        }
        else if (isdigit (c)) {
            fgets (line, 1023, fp);

            tmp = line;
            while (*tmp && *tmp != '\n') {
                int tone = c - '0';

                set_gbhz_pinyin (tmp [0], tmp[1], nr_py - 1, tone);

                tmp += 2;
            }
        }
        else 
            break;
    }

    output_funcs ();
    output_py_tab ();
    output_gbhz_py_tab ();

    fclose (fp);
    return 0;
}

