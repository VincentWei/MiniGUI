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
** $Id$
**
** combiningtest.c: Unit test for arabic combining.
**
** Copyright (C) 2003 ~ 2008 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: MaLin.
**
** Create date: 2008/01/24
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#define TATWEEL  0xe0
#define SHADDA   0xf1
#define SUKUN    0xf2

#define PHONETICSNUM  6
static FILE *fp = NULL;
char filename[256];
char combuf[4096];


static Uint8 phonetics_tab [PHONETICSNUM] = {
    0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0
};


void phoneticstest (unsigned char *mchar, int result)
{
    int i;
    unsigned char code;
    unsigned char cur_char, next, next_next;

    cur_char = *mchar;

    next = *(mchar + 1);
    next_next = *(mchar + 2);

    for (i = 0; i < PHONETICSNUM; i ++)
    {
        code = phonetics_tab[i];

        //音标组合测试
        if (cur_char == SHADDA && next == code) 
        {
            printf ("This is a Phonetics SHADDA + standard:\n");
            printf ("0x%02x + 0x%02x = 0x%02x \n", cur_char, next, result);
        }
        if (cur_char == TATWEEL)
        {
            if (next == code)
            {
                printf ("This is a Phonetics TATWEEL + standard:\n");
                printf ("0x%02x + 0x%02x = 0x%02x\n", cur_char, next, result);
            }else if (next == SUKUN)
            {
                printf ("This is a Phonetics TATWEEL + SUKUN:\n");
                printf ("0x%02x + 0x%02x = 0x%02x\n", cur_char, next, result);
                break;
            }else if (next == SHADDA)
            {
                printf ("This is a Phonetics TATWEEL + SHADDA:\n");
                printf ("0x%02x + 0x%02x = 0x%02x\n", cur_char, next, result);
                break;
            }else if (next == SHADDA && next_next == code)
            {
                printf ("This is a Phonetics TATWEEL + SHADDA + standard:\n");
                printf ("0x%02x + 0x%02x + 0x%02x = 0x%02x\n", cur_char, next, next_next, result);
            }
        }
//        printf ("cur_char = 0x%02x, next = 0x%02x, next_next = 0x%02x \n", cur_char, next, next_next);
    }
}

int cominingtest()
{
    int i = 0;
    int result = -1, ignore;
    if (fp == NULL)
    {
        fp = fopen (filename, "rb");
        if (!fp)
        {
            fprintf (stderr, "cannot open %s\n", filename);
            return -1;
        }
    }
    memset (combuf, 0, 4096);
    while (!feof(fp))
    {
        combuf[i] = fgetc(fp);
        /*if (combuf[i] == 0xa || combuf[i] == 0xd)
        {
            break;
        }*/
        i ++;
    }
    if (i > 0)
    {
        int m = 0;
        for (m = 0; m < i; m ++)
        {
            //printf ("0x%02x  ", (unsigned char)combuf[m]);
            result = get_ligature(combuf+m, i, 1, &ignore);
            if (result != -1)
            {
                phoneticstest ((unsigned char*) combuf+m, result);
                printf ("\n");
            }
        }
        printf ("\n");
        return 1;
    }else{
        fseek (fp, 0, SEEK_SET);
        return -1;
    }



}

int main (int argc, const char* argv[])
{
    if (argc >= 2)
        strcpy (filename, argv[1]);
    cominingtest();
}


