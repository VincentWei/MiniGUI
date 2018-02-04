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
 * Usage:
 *  1) Define your VBFINFO name as extern_vbfinfo in a C file.
 *  2) Please link this file with the C file defining your VBF file to compile an executable.
 *  3) Run the executable.
 *
 * Limitation: 
 *  Only VBF V1.0 is supported.
 */

#include <stdio.h>
#include <string.h>

#include "vbf.h"

#define LEN_VERSION_INFO    10

#define VBF_VERSION         "vbf-1.0**"

typedef int BOOL;
#define TRUE    1
#define FALSE   0

#define LEN_FONT_NAME               23
#define LEN_DEVFONT_NAME            79

extern VBFINFO extern_vbfinfo;

#define vbf_to_dump extern_vbfinfo

BOOL dumpVBF (VBFINFO* vbf, char* file)
{
    FILE* fp;
    char family [LEN_FONT_NAME + 1];
    char style [LEN_FONT_NAME + 1];
    int len_offsets;
    int len_widths;
    int len_header;
    int len_bits = vbf->len_bits;
    int len_file;
    int num_chars = (vbf->last_char - vbf->first_char + 1);

    len_header = 4 + 2 + 8 + 3 + 12 + 4 + LEN_VERSION_INFO;
    len_offsets = num_chars * sizeof (unsigned short);
    len_widths = num_chars * sizeof (unsigned char);

    len_file = len_header + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + len_bits;
    sprintf (file, "%s.vbf", vbf->name);

    if ((fp = fopen (file, "w+")) == NULL)
        return FALSE;

    fwrite (VBF_VERSION, sizeof (char), LEN_VERSION_INFO, fp);
    fwrite (&len_header, sizeof (int), 1, fp);
    fwrite (&vbf->max_width, sizeof (char), 1, fp);
    fwrite (&vbf->ave_width, sizeof (char), 1, fp);
    fwrite (&vbf->height, sizeof (int), 1, fp);
    fwrite (&vbf->descent, sizeof (int), 1, fp);
    fwrite (&vbf->first_char, sizeof (unsigned char), 1, fp);
    fwrite (&vbf->last_char, sizeof (unsigned char), 1, fp);
    fwrite (&vbf->def_char, sizeof (unsigned char), 1, fp);

    fwrite (&len_offsets, sizeof (int), 1, fp);
    fwrite (&len_widths, sizeof (int), 1, fp);
    fwrite (&len_bits, sizeof (int), 1, fp);
    fwrite (&len_file, sizeof (int), 1, fp);

    fwrite (vbf->name, sizeof (char), LEN_DEVFONT_NAME + 1, fp);
    fwrite (vbf->offset, sizeof (unsigned short), num_chars, fp);
    fwrite (vbf->width, sizeof (unsigned char), num_chars, fp);

    fwrite (vbf->bits, sizeof (unsigned char), len_bits, fp);

    fclose (fp);
    return TRUE;
}

int main (void)
{
    char file [256];

    dumpVBF (&vbf_to_dump, file);
    printf ("The vbf font name: '%s'\n", vbf_to_dump.name);
    printf ("The VBF dump to file: '%s'\n", file);
    return 0;
}

