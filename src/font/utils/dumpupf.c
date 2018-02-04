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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "../../include/devfont.h"

#define PATH_LEN    128

static  LOGFONT *_logfont;

#ifdef  _DEF_DYNAMIC_LOAD_DEVFONT_
static  DEVFONT  *_devfont;
#endif

const char * _usage =
        "\n"
        "Usage: dumpupf <dev-font-name> <font-file-name> "
        "<small_unicode> [big_unicode]\n"
        "       Dump font data between small unicode and big unicode.\n" 
        "       The font data list below.\n"
        "       ---font property \n"
        "               ave width \n"
        "               max width \n"
        "               font heigh \n"
        "               font size \n"
        "               font ascent \n"
        "               font descent \n"
        "       ---glyph property \n"
        "               glyph exist \n"
        "               glyph bbox \n"
        "               glyph advance \n"
        "               glyph monobitmap \n"
        "               glyph greybitmap \n"
        "\n"
        "Example: \n"
        " $ ./dumpupf qpf-unifont-rrncnn-16-16-ISO8859-1 unifont_160_50.qpf 0 1618\n"
        " $ ./dumpupf qpf-micro-rrncnn-4-4-ISO8859-1 micro_40_50.qpf 100 > tmp\n\n";

void dumpupf (DEVFONT * devfont, int glyph)
{
    int index;
    int bbox_x, bbox_y, bbox_w, bbox_h;
    int advance_x, advance_y;
    unsigned short scale;
    unsigned short scale_greybmp, scale_monobmp;
    int pitch;
    unsigned char * monobitmap;
    unsigned char * bmp_grey;
    unsigned char * bmp_mono;
    BITMAP bmp_glyph = {0};

    printf ("------------- glyph : %i ------------- \n", glyph);

    if (devfont->font_ops->is_glyph_existed(_logfont, devfont, glyph))
    {
        printf ("glyph exist      = TRUE\n");
    }
    else
    {
        printf ("glyph exist      = FALSE\n");
    }

    if (devfont->font_ops->get_glyph_bbox)
    {
        devfont->font_ops->get_glyph_bbox 
            (_logfont, devfont, glyph, &bbox_x, &bbox_y, &bbox_w, &bbox_h);
        printf ("glyph bbox       = (%i, %i, %i , %i)\n",
                bbox_x, bbox_y, bbox_w, bbox_h);
    }
    else
    {
        printf ("glyph bbox       = NULL function\n");
    }

    if (devfont->font_ops->get_glyph_advance)
    {
    
        devfont->font_ops->get_glyph_advance 
            (_logfont, devfont, glyph, &advance_x, &advance_y);

        printf ("glyph advance    = (%i, %i)\n", 
                advance_x, advance_y);
    }
    else
    {
        printf ("glyph advance    = NULL function\n"); 
    }

    if (devfont->font_ops->get_glyph_monobitmap)
    {
        bmp_grey = (unsigned char *)devfont->font_ops->get_glyph_monobitmap 
            (_logfont, devfont, glyph, &scale_greybmp);

        printf ("glyph monobitmap = (scale=%i; ", scale_greybmp);
        if (!bmp_grey)
        {
            printf ("bmp = NULL)\n");
        }
        else
        {
            printf ("bmp = {");

            printf ("0x%x", bmp_grey [0]);

            for (index = 1;
                index < sizeof (bmp_grey) / sizeof (unsigned char);
                ++index)
            {
                printf (", 0x%x", bmp_grey [index]);
            }

            printf ("} )\n");
        }

    }
    else
    {
        printf ("glyph monobitmap = NULL function\n"); 
    }

    if (devfont->font_ops->get_glyph_greybitmap)
    {
        bmp_mono = (unsigned char *)devfont->font_ops->get_glyph_greybitmap 
            (_logfont, devfont, glyph, &pitch, &scale_monobmp);

        printf ("glyph greybitmap = (scale=%i, pitch=%i, ", 
                scale_monobmp, pitch);

        if (!bmp_mono)
        {
            printf ("bmp=NULL)\n");
        }
        else
        {
            printf ("glyph greybitmap = (bmp = {");

            printf ("0x%x", bmp_mono [0]);

            for (index = 1; 
                index < sizeof (bmp_mono) / sizeof (unsigned char) - 1; 
                ++index)
            {
                printf (", 0x%x", bmp_mono [index]);
            }

            printf ("} )\n");
        }
    }
    else
    {
        printf ("glyph greybitmap = NULL function\n"); 
    }
  
}

void dump_common_data (DEVFONT * devfont)
{
    int ave_width;
    int max_width;
    int font_height;
    int font_size;
    int font_ascent;
    int font_descent;

    ave_width = devfont->font_ops->get_ave_width(_logfont, devfont);
    max_width = devfont->font_ops->get_max_width(_logfont, devfont);
    font_height = devfont->font_ops->get_font_height(_logfont, devfont);
    font_size = devfont->font_ops->get_font_size(_logfont, devfont, 1);
    font_ascent = devfont->font_ops->get_font_ascent(_logfont, devfont);
    font_descent = devfont->font_ops->get_font_descent(_logfont, devfont);

    printf ( 
            "\n           font name : [%s] \n \n"
            "======================================\n",
            devfont->name);

    printf (
            "ave_width     = %i\n"
            "max_width     = %i\n"
            "font_height   = %i\n"
            "font_size     = %i\n"
            "font_ascent   = %i\n"
            "font_descent  = %i\n" 
            "======================================\n",
            ave_width, max_width, font_height, 
            font_size, font_ascent, font_descent);
}

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "dumpupf", 0, 0);
#endif

    int i;
    int unicode_value_min;
    int unicode_value_max;

    char  font_file[PATH_LEN] = {0};

    DEVFONT *mydevfont;

    if (argc != 4 && argc != 5)
    {
        printf (_usage);
        exit (0);
    }

    /** get unicode value */
    unicode_value_min = atoi (argv[3]);
    if (argc == 5)
    {
        unicode_value_max = atoi (argv[4]);
        if (unicode_value_max < unicode_value_min)
        {
            printf ( _usage);
            exit (0);
        }
    }
    else
    {
        unicode_value_max = unicode_value_min;
    }

    /** get font file name */
    strcpy (font_file, "/usr/local/lib/minigui/res/font/");
    strcat (font_file, argv[2]);
    font_file[PATH_LEN] = '\0';

#ifdef _DEF_DYNAMIC_LOAD_DEVFONT_
    _devfont == LoadDevFontFromFile (argv[1], font_file);
    if (!_devfont)
    {
        printf ("ERROR: LoadDevFontFromFile fail. \n");
        exit (0);
    }
#endif

    /** create logfont according device font name */
    _logfont = CreateLogFontByName (argv[1]);
    if (!_logfont)
    {
        printf ("ERROR: CreateLogFontByName fail. \n");
        exit (0);
    }

#ifdef _DEBUG
    dbg_dumpDevFonts();
#endif
    
    if (_logfont->sbc_devfont)
        mydevfont = _logfont->sbc_devfont;
    else
        mydevfont = _logfont->mbc_devfont;

    if (!mydevfont)
    {
        printf ("ERROR: Invalid LogFont.\n");
        exit (0);
    }

    dump_common_data (mydevfont);

    for (i = unicode_value_min; i <= unicode_value_max; ++i)
    {
        dumpupf(mydevfont, i);
    }

#ifdef _DEF_DYNAMIC_LOAD_DEVFONT_
    DestroyDynamicDevFont (_devfont);
#endif

    exit (0);
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
