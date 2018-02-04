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
#include <minigui/control.h>

static DLGTEMPLATE DlgBoxInputChar =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 400+400, 230, 
    "bidiedit input (press F1 to change keyboard layout!)",
    0, 0,
    6, NULL,
    0
};

#define IDC_CHAR        100
#define IDC_CHARS       110
#define IDC_BIDICHAR    120
#define IDC_CHARNORMAL  130
#define IDC_STATIC2     140
#define IDC_BIDICHAR_R  150

static CTRLDATA CtrlInputChar [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE,
        10, 10, 380, 18, 
        IDC_STATIC, 
        "bidiedit input ",
        0
    },
    {
        CTRL_BIDISLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_RIGHT,
        10, 40, 80+600, 25,
        IDC_BIDICHAR,
        NULL,
        //"BIDI:this is only a test.",
        0
    },
    {
        CTRL_BIDISLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
        10, 40+30, 80+600, 25,
        IDC_BIDICHAR_R,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE,
        10, 40+60, 380, 18, 
        IDC_STATIC2, 
        "normal sledit input:",
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
        10, 40+60+30, 80+600, 25,
        IDC_CHARNORMAL,
        "Normal:this is only a test.",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        170-80, 160, 60+100, 25,
        IDOK, 
        "Exit",
        0
    }
#if 0
    {
        CTRL_MLEDIT,
        WS_VISIBLE | WS_BORDER | WS_VSCROLL 
            | ES_BASELINE | ES_AUTOWRAP | ES_NOHIDESEL | ES_NOHIDESEL,//| WS_DISABLED,
        10, 80, 380, 70, 
        IDC_CHARS, 
        NULL,
        0 
    },
#endif
};

static void my_notif_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    char buff [256] = {0};
/*
    if (id == IDC_CHAR && nc == EN_CHANGE) {

        GetWindowText (hwnd, buff, 4);
        SendMessage (hwnd, EM_SETCARETPOS, 0, 0);
        SendMessage (GetDlgItem (GetParent (hwnd), IDC_CHARS), MSG_CHAR, buff[0], 0L);
    }
    else if (id == IDC_CHARS && nc == EN_CHANGE) {
        GetWindowText (hwnd, buff, 255);
    }
*/
}       

#define BUF_LEN    20
//static char buf[BUF_LEN] = {0xD0, 0xD1, 0xD2, 0xD3};
//static char* buf = "this is only a test.";
//static char* buf = "this is only a test."
//                   "\xe4\xd8\xc2\xd1\xd5\xe4\xc2\xe4\xc7\x0";
static char* buf = "this is only a test."
                   "\xe4\xd8\xc2\xd1\x30\x31\x32\xd5\xe4\xc2\xe4\xc7\x0";
//static char* buf = "test"
//                   "\xe4\xd8";

static char* buf_rtol = "\xe4\xd8\xc2\xd1\xd5\xe4\xc2\xe4\xc7"
                   "this is only a test.";

//static char* buf = "\xe4\xd8""a";


static char tip[100] = "this is only a test.";

static PLOGFONT  arabicfont;
static BOOL arabic_input = FALSE;
static HWND hwnd_edit;
static HWND hwnd_focus_edit;
static HWND hwnd_static;

static FILE *fp = NULL;
char linebuf[4096];

static int reset_fp_pos()
{
    if(!fp){
        fp = fopen("bidi_text.txt", "rb");
        if (!fp) {
            fprintf(stderr, "open file error!\n");
            return 0;
        }
    }
    if(fp){
        fseek(fp, 0, SEEK_SET);
    }
    return 1;
}

static char* read_one_line(BOOL* read_over)
{
    int i = 0;
    char c = 0;
    if(fp == NULL){
        *read_over = TRUE;
        return NULL;
    }
    memset(linebuf, 0, 4096);
    while(!(*read_over = feof(fp))) {
        c = fgetc(fp);
        if (c == 0xa || c == 0xd) {
            break;
        }
        linebuf[i] = c;
        i++;
    }
    if(i > 0) {
        return linebuf;
    }
    else{
        return NULL;
    }
}

static void set_edit_font(HWND hDlg)
{
    char* pline = NULL;
    BOOL read_over = FALSE;
    hwnd_edit = GetDlgItem (hDlg, IDC_BIDICHAR);

#if 1
    	arabicfont = CreateLogFont("vbf", "naskhi", "ISO8859-6", FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
			FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
			FONT_STRUCKOUT_NONE, 30, 0);
#else
    	arabicfont = CreateLogFont ("ttf", "DejaVuSans", "ISO8859-6", 
        		FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, 
            		FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL, 
            		FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
            		18, 0);
#endif

    if(arabicfont ==INV_LOGFONT)
        printf("arabicfont create error!\n");
    else
	printf("the font used %s\n", arabicfont->mbc_devfont->name);
    
    reset_fp_pos();
    pline = read_one_line(&read_over);
    SetWindowFont(hwnd_edit, arabicfont);
    //SendMessage (hwnd_edit, MSG_SETTEXT, strlen(pline), (LPARAM)pline);
    //SendMessage (hwnd_edit, MSG_SETTEXT, strlen(buf_rtol), (LPARAM)buf_rtol);

    /* set the right style bidiedit. */
    hwnd_edit = GetDlgItem (hDlg, IDC_BIDICHAR_R);
    SetWindowFont(hwnd_edit, arabicfont);
    //SendMessage (hwnd_edit, MSG_SETTEXT, strlen(pline), (LPARAM)pline);
    //SendMessage (hwnd_edit, MSG_SETTEXT, strlen(buf), (LPARAM)buf);
}


static void set_edit_focus(HWND hDlg)
{
    /* set the right style bidiedit. */
    hwnd_focus_edit = GetFocusChild(hDlg);

}
static void set_keyboard_layout(HWND hDlg)
{
    hwnd_focus_edit = GetFocusChild(hDlg);
    if (!arabic_input) {
        arabic_input = TRUE;
        strcpy(tip, "Arabic Input:");
        SetKeyboardLayout (KBD_LAYOUT_ARABIC);
    }
    else {
        arabic_input = FALSE;
        strcpy(tip, "English Input:");
        SetKeyboardLayout ("default");
    }
    hwnd_static = GetDlgItem (hDlg, IDC_STATIC);
    //SetWindowBkColor(hwnd_static, PIXEL_blue);
    SendMessage (hwnd_static, MSG_SETTEXT, strlen(tip), (LPARAM)tip);
    SetFocusChild(hwnd_focus_edit);
}

#if 0
    if (cur_type == GLYPH_TYPE_RTL) {
        if (l_glyph_type == GLYPH_TYPE_RTL)
            return INSERTPOS_BEFORE (sled, l_glyph_index);
        else if (r_glyph_type == GLYPH_TYPE_RTL)
            return INSERTPOS_AFTER (sled, r_glyph_index);
        else
        {
            if (l_glyph_type == GLYPH_TYPE_LTR)
                return INSERTPOS_AFTER (sled, l_glyph_index);
            else if (l_glyph_type == GLYPH_TYPE_LTR)
                return INSERTPOS_BEFORE (sled, r_glyph_index);
            else
                return 0;
        }
    }
    else {
        if (l_glyph_type == GLYPH_TYPE_LTR)
            return INSERTPOS_AFTER (sled, l_glyph_index);
        else if (r_glyph_type == GLYPH_TYPE_LTR)
            return INSERTPOS_BEFORE (sled, r_glyph_index);
        else {
            if (l_glyph_type == GLYPH_TYPE_RTL)
                return INSERTPOS_BEFORE (sled, l_glyph_index);
            else if (l_glyph_type == GLYPH_TYPE_RTL)
                return INSERTPOS_AFTER (sled, r_glyph_index);
            else
                return 0;
        }
    }
#endif

static int InputCharDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG: 
            set_keyboard_layout(hDlg);
            set_edit_font(hDlg);
            break;
        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            break;
        case MSG_KEYDOWN:
            if (LOWORD(wParam)==SCANCODE_F1)
                set_keyboard_layout(hDlg);
            break;
        case MSG_COMMAND:
            switch (wParam) {
                case IDOK:
                    EndDialog (hDlg, wParam);
                    break;
                case IDCANCEL:
                    EndDialog (hDlg, wParam);
                    break;
            }
            break;
    }
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "edit" , 0 , 0);
#endif

#ifndef _MGRM_THREADS
    if (!InitVectorialFonts ()) {
        printf ("InitVectorialFonts: error.\n");
        return 1;
    }
#endif

    DlgBoxInputChar.controls = CtrlInputChar;
    DialogBoxIndirectParam (&DlgBoxInputChar, HWND_DESKTOP, InputCharDialogBoxProc, 0L);

#ifndef _MGRM_THREADS
    TermVectorialFonts ();
#endif
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

