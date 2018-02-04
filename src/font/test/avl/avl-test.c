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
** avl-test.c: Unit test for avl tree of bitmap font.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/22
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

#include "bmpfonttest.h"

char *pgm_name;

/* Print helpful syntax message and exit. */
static void usage (void)
{
  static const char *help[] =
    {
      "bmpfont-test, unit test for MiniGUI2.2 bitmap font .\n\n",
      "Usage: %s [insert order] [charset] ...\n\n",
      "insert order          ORDER is one of:\n",
      "                      random      random permutation (default)\n",
      "                      ascending   ascending order 0...n-1\n",
      "                      custom      custom, read from stdin\n",
      "charset               Charset is one of:\n",
      "                      ISO8859   ISO8859-1\n",
      "                      GB2312    GB2312-0\n\n",
      NULL,
    };

  const char **p;
  for (p = help; *p != NULL; p++)
    printf (*p, pgm_name);

  exit (EXIT_SUCCESS);
}

/* Parses command-line arguments from null-terminated array |args|.
   Sets up |options| appropriately to correspond. */
static void
parse_command_line (char **args, struct test_options *options)
{
    
    options->insert_order = INS_ASCENDING;
    options->charset = ISO8859;
    options->node_cnt = 3;

    if (strcmp (args[1], "random") == 0)
        options->insert_order = INS_RANDOM;
    else if (strcmp (args[1], "ascending") == 0)                       
        options->insert_order = INS_ASCENDING;
    else if (strcmp (args[1], "descending") == 0)                          
        options->insert_order = INS_DESCENDING;
    else if (strcmp (args[1], "custom") == 0)                          
        options->insert_order = INS_CUSTOM;

    if (strcmp (args[2], "ISO8859") == 0)
        options->charset = ISO8859;
    else if (strcmp (args[2], "GB2312") == 0)                       
        options->charset = GB2312;

    options->node_cnt = atoi(args[3]);

}

int main (int argc, char *argv[])
{

    struct test_options opts;        /* Command-line options. */
    ELEMENT *insert;
    char font_name [50];

    pgm_name = argv[0];

    if (argc < 4 || argc > 4)
        usage ();

    parse_command_line (argv, &opts);
    insert = malloc (sizeof(*insert)*opts.node_cnt);
   
    //opts.seed = (int)time(0); 
    srand ((int)time(0));
    gen_insertions (opts.insert_order, opts.charset, insert, opts.node_cnt);

    strncpy (font_name, "bmp-led-rrncnn-8-2-", 19);
    switch(opts.charset) {
        case ISO8859:
            strcat (font_name, "ISO8859-1");
            break;
            
        case GB2312:
            strcat (font_name, "GB2312-0");
            break;
    }

    fprintf (stderr, "font_name = %s \n", font_name);

    test_correctness (font_name, insert, opts.node_cnt);
   
    free (insert);
}


