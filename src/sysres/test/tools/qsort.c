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
#include <unistd.h>
#include <string.h>
#include <assert.h>



#ifdef USE_BITMAP
static char *strings[] = {
    "shurufa-flat.bmp", "MiniGUI256.bmp",
    "classic_radio_button.bmp", "classic_check_button.bmp",
    "flat_radio_button.bmp", "flat_check_button.bmp",
    "fashion_radio_btn.bmp", "fashion_check_btn.bmp",
    "bg_tiny.bmp",
    "skin_bkgnd.bmp", "skin_caption.gif", "skin_cpn_btn.gif",
    "skin_sb_hshaft.bmp", "skin_sb_vshaft.bmp", "skin_sb_hthumb.bmp",
    "skin_sb_vthumb.bmp", "skin_sb_arrows.bmp", "skin_tborder.bmp",
    "skin_bborder.bmp", "skin_lborder.bmp", "skin_rborder.bmp",
    "skin_arrows.gif", "skin_arrows_shell.bmp", "skin_pushbtn.gif",
    "skin_radiobtn.bmp", "skin_checkbtn.bmp", "skin_tree.bmp",
    "skin_header.bmp", "skin_tab.bmp", "skin_tbslider_h.bmp",
    "skin_tbslider_v.bmp", "skin_tb_horz.gif", "skin_tb_vert.gif",
    "skin_pb_htrack.gif", "skin_pb_vtrack.bmp", "skin_pb_hchunk.bmp",
    "skin_pb_vchunk.bmp"
};
#else

static char *strings[] = {
"Icon_211.ico",
"excalmatory-flat.ico",
"excalmatory.ico",
"failed-flat.ico",
"failed.ico",
"fold-flat.ico",
"fold.ico",
"folder-flat.ico",
"folder.ico",
"form-flat.ico",
"form.ico",
"mg_help-flat.ico",
"mg_help.ico",
"textfile-flat.ico",
"textfile.ico",
"unfold-flat.ico",
"unfold.ico",
"w95mbx01-flat.ico",
"w95mbx01.ico",
"w95mbx02-flat.ico",
"w95mbx02.ico",
"w95mbx03-flat.ico",
"w95mbx03.ico",
"w95mbx04-flat.ico",
"w95mbx04.ico",
"warning-flat.ico",
"warning.ico"
};

#endif

#define TABLESIZE(table)    (sizeof(table)/sizeof(table[0]))


static int cmpstringp(const void *p1, const void *p2)
{
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

int main(int argc, char *argv[])
{
    int j;


    if(argc > 1)
        qsort(&argv[1], argc - 1, sizeof(char *), cmpstringp);
    else
        qsort(&strings[0], TABLESIZE (strings), sizeof(char *), cmpstringp);

    if (argc > 1)
        for (j = 1; j < argc; j++) 
            puts(argv[j]);
    else
        for (j = 0; j < TABLESIZE(strings); j++) {
            puts (strings[j]);
    }
 
    printf ("string num: %d.\n", TABLESIZE(strings));
    exit(EXIT_SUCCESS);
}


