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

//#define VAL_DEBUG
//#define INFO_DEBUG
//#define FUN_DEBUG
#include "my_debug.h"

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

#define MIN_FONT_SIZE 8
#define MAX_FONT_SIZE 72


#define ID_FAMILY   100
#define ID_CHARSET  101
#define ID_WEIGHT   102
#define ID_SLANT    103
#define ID_FLIP     104
#define ID_SIZE     105
#define ID_DISPLAY  106

#define ID_SHOW_FAMILY   107
#define ID_SHOW_CHARSET  108
#define ID_SHOW_WEIGHT   109
#define ID_SHOW_SLANT    110
#define ID_SHOW_FLIP     111
#define ID_SHOW_SIZE     112
#define ID_SHOW_DISPLAY  113

typedef struct _CHARSET
{
    char name [LEN_FONT_NAME + 1];
    struct _CHARSET* next;
}CHARSET;

typedef struct _FAMILY
{
    char name [LEN_FONT_NAME + 1];
    CHARSET chset_head;
    struct _FAMILY* next;
}FAMILY;

static FAMILY* find_insert_family (FAMILY* head, const char* fami_name)
{
    FAMILY* cur = NULL;
    FAMILY* pre = NULL;

    cur = head->next;
    pre = head;

    while (cur)
    {
        if (strcasecmp(cur->name, fami_name) == 0)
            return cur;
        pre = cur;
        cur = cur->next;
    }

    cur = (FAMILY*)malloc(sizeof(*cur));
    strncpy (cur->name, fami_name, LEN_FONT_NAME);
    cur->name [LEN_FONT_NAME] = '\0';
    cur->chset_head.next = NULL;
    cur->next = NULL;

    pre->next = cur;
    return cur;
}

static FAMILY* find_family (FAMILY* head, const char* fami_name)
{
    FAMILY* cur = head->next;
    while (cur)
    {
        if (strcasecmp(cur->name, fami_name) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void insert_chset (FAMILY* fami, const char* chset_name)
{
    CHARSET* cur = fami->chset_head.next;
    CHARSET* pre = &(fami->chset_head);

    while (cur)
    {
        if (strcasecmp(cur->name, chset_name) == 0)
            return;
        pre = cur;
        cur = cur->next;
    }

    cur = (CHARSET*)malloc(sizeof(*cur));
    strncpy (cur->name, chset_name, LEN_FONT_NAME);
    cur->name [LEN_FONT_NAME] = '\0';
    cur->next = NULL;

    pre->next = cur;
}

static FAMILY* create_font_attrlist (void)
{
    const DEVFONT* font = NULL; 
    char* font_name;

    char* fm_name_start;
    char* fm_name_end;

    char* cs_name_start;
    char* cs_name_sep;

    int i;

    FAMILY* fami_head;
    FAMILY* fami_cur = NULL;

    font_name = malloc (LEN_UNIDEVFONT_NAME + 2);

    fami_head = (FAMILY*)malloc(sizeof(*fami_head));
    fami_head->next = NULL;
    fami_head->chset_head.next = NULL;

    font = GetNextDevFont (NULL);
    while (font)
    {
        /*find family name*/
        strncpy(font_name, font->name, LEN_DEVFONT_NAME);

        i = strlen(font_name);
        font_name[i] = ',';
        font_name[i+1] = '\0';

        fm_name_start = strchr (font_name, '-') + 1;
        fm_name_end = strchr (fm_name_start, '-');
        *fm_name_end = '\0';

        /*find one or more charset names*/
        cs_name_start = fm_name_end + 1;
        for (i=0; i<3; i++)
            cs_name_start = strchr (cs_name_start, '-') + 1;

        fami_cur = find_insert_family(fami_head, fm_name_start);

        while (*cs_name_start) 
        {
            /*find a charset */
            cs_name_sep = strchr (cs_name_start, ',');
            *cs_name_sep = '\0';
            insert_chset (fami_cur, cs_name_start);

            cs_name_start = cs_name_sep + 1;
        };

        font = GetNextDevFont (font);
    }

    free (font_name);
    return fami_head;
}

static void free_font_attrlist (FAMILY* fami_head)
{
    FAMILY* fami_cur = fami_head->next;
    FAMILY* fami_tmp;

    CHARSET* chset_cur;
    CHARSET* chset_tmp;

    while (fami_cur)
    {
        /*delete all charset node of current family*/
        chset_cur = fami_cur->chset_head.next;

        while (chset_cur)
        {
            chset_tmp = chset_cur->next;
            free (chset_cur);
            chset_cur = chset_tmp;
        }
        
        /*delete current family node*/
        fami_tmp = fami_cur->next;
        free (fami_cur);
        fami_cur = fami_tmp;
    }
}

static CTRLDATA ctrls [] =
{
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        10, 10, 80, 10,
        ID_SHOW_FAMILY,
        "family", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|LBS_SORT|WS_VSCROLL|WS_BORDER,
        10, 30, 80, 80,
        ID_FAMILY,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        100, 10, 80, 10,
        ID_SHOW_CHARSET,
        "charset", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|LBS_SORT|WS_VSCROLL|WS_BORDER,
        100, 30, 80, 80,
        ID_CHARSET,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        10, 120, 50, 10,
        ID_SHOW_WEIGHT,
        "weight", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
        10, 140, 50, 50,
        ID_WEIGHT,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        70, 120, 50, 10,
        ID_SHOW_SLANT,
        "slant", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
        70, 140, 50, 50,
        ID_SLANT,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        130, 120, 50, 10,
        ID_SHOW_FLIP,
        "flip", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
        130, 140, 50, 50,
        ID_FLIP,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        190, 10, 20, 10,
        ID_SHOW_SIZE,
        "size", 0
    },
    {
        CTRL_LISTBOX,
        WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_BORDER,
        190, 30, 20, 160,
        ID_SIZE,
        "", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD,
        10, 200, 80, 10,
        ID_SHOW_DISPLAY,
        "preview", 0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        10, 220, 200, 30,
        ID_DISPLAY,
        "font in minigui", 0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE|WS_CHILD|WS_BORDER|BS_PUSHBUTTON|BS_CENTER|BS_VCENTER,
        10, 260, 30, 10,
        IDCANCEL,
        "CANCEL", 0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE|WS_CHILD|WS_BORDER|BS_PUSHBUTTON|BS_CENTER|BS_VCENTER,
        180, 260, 30, 10,
        IDOK,
        "OK", 0
    },
};

static DLGTEMPLATE dlg_temp = 
{
    WS_BORDER|WS_CAPTION|WS_VISIBLE,
    WS_EX_NONE,
    20, 20, 220, 300,
    "font selecting dialog",
    0, 0,
    ARRAY_LEN(ctrls), ctrls,
    0,
};

static char* weight_str [] =
{
    "black",
    "bold",
    "book",
    "demibold",
    "light",
    "medium",
    "regular",
};
static char weight_arg [] = 
{
    FONT_WEIGHT_BLACK,
    FONT_WEIGHT_BOLD,
    FONT_WEIGHT_BOOK,
    FONT_WEIGHT_DEMIBOLD,
    FONT_WEIGHT_LIGHT,
    FONT_WEIGHT_MEDIUM,
    FONT_WEIGHT_REGULAR,
};

static char* slant_str [] = 
{
    "italic",
    "oblique",
    "roman",
};
static char slant_arg [] =
{
    FONT_SLANT_ITALIC,
    FONT_SLANT_OBLIQUE,
    FONT_SLANT_ROMAN,
};

static char* flip_str [] =
{
    "nil",
    "horz",
    "vert",
    "horzvert",
};

static char flip_arg [] = 
{
    FONT_FLIP_NIL,
    FONT_FLIP_HORZ,
    FONT_FLIP_VERT,
    FONT_FLIP_HORZVERT,
};

static void insert_strings (HWND hWnd, char** strs, int str_num)
{
    int i;
    for (i=0; i<str_num; i++)
    {
        SendMessage (hWnd, LB_INSERTSTRING, i, (LPARAM)strs[i]);
    }
    SendMessage (hWnd, LB_SETCURSEL, 0, 0);
}

static int fresh_chset_list (HWND hWnd)
{
    char str_buffer [LEN_FONT_NAME + 1];
    HWND fami_lb;
    HWND chset_lb;
    int cur_sel;

    FAMILY* fami_head;
    FAMILY* fami_cur;
    CHARSET* chset_cur;

    fami_lb = GetDlgItem (hWnd, ID_FAMILY);
    chset_lb = GetDlgItem (hWnd, ID_CHARSET);    

    cur_sel = SendMessage (fami_lb, LB_GETCURSEL, 0, 0);
    SendMessage (fami_lb, LB_GETTEXT, cur_sel, (LPARAM)str_buffer);
    
    fami_head = (FAMILY*)GetWindowAdditionalData (hWnd);
    fami_cur = find_family (fami_head, str_buffer);

    if (!fami_cur)
        return -1;

    chset_cur = fami_cur->chset_head.next;

    SendMessage (chset_lb, LB_RESETCONTENT, 0, (LPARAM)chset_cur->name);

    /*insert charset listbox*/
    while (chset_cur)
    {
        SendMessage (chset_lb, LB_ADDSTRING, 0, (LPARAM)chset_cur->name);
        chset_cur = chset_cur->next;
    }
    SendMessage (chset_lb, LB_SETCURSEL, 0, 0);
    return 0;

}

static int insert_listboxs (HWND hWnd)
{
    FAMILY* fami_cur = ((FAMILY*)GetWindowAdditionalData(hWnd))->next;
    char str_buffer [LEN_FONT_NAME + 1];
    int i;
    HWND hctrl;

    /*insert family listbox*/
    hctrl = GetDlgItem (hWnd, ID_FAMILY);
    while (fami_cur)
    {
        SendMessage (hctrl, LB_ADDSTRING, 0, (LPARAM)fami_cur->name);
        fami_cur = fami_cur->next;
    }

    SendMessage (hctrl, LB_SETCURSEL, 0, 0);
    SendMessage (hctrl, LB_GETTEXT, 0, (LPARAM)str_buffer);

    UpdateWindow (hctrl, TRUE);
    fresh_chset_list (hWnd);

    /*insert weight listbox*/
    hctrl = GetDlgItem (hWnd, ID_WEIGHT);
    insert_strings (hctrl, weight_str, ARRAY_LEN(weight_str));

    /*insert slant listbox*/
    hctrl = GetDlgItem (hWnd, ID_SLANT);
    insert_strings (hctrl, slant_str, ARRAY_LEN(slant_str));

    /*insert flip listbox*/
    hctrl = GetDlgItem (hWnd, ID_FLIP);
    insert_strings (hctrl, flip_str, ARRAY_LEN(flip_str));

    hctrl = GetDlgItem (hWnd, ID_SIZE);
    for (i=MIN_FONT_SIZE; i<=MAX_FONT_SIZE; i++)
    {
        snprintf(str_buffer, 4, "%d", i);
        SendMessage (hctrl, LB_ADDSTRING, 0, (LPARAM)str_buffer);
    }
    return 0;
}

static LOGFONT* create_sel_font(HWND hWnd)
{
    char family [LEN_FONT_NAME +1];
    char chset [LEN_FONT_NAME +1];
    char weight;
    char slant;
    char flip;
    int sel;
    HWND hctrl;
    
    hctrl = GetDlgItem (hWnd, ID_FAMILY);
    sel = SendMessage (hctrl, LB_GETCURSEL, 0, 0);
    SendMessage (hctrl, LB_GETTEXT, sel, (LPARAM)family);

    hctrl = GetDlgItem (hWnd, ID_CHARSET);
    sel = SendMessage (hctrl, LB_GETCURSEL, 0, 0);
    SendMessage (hctrl, LB_GETTEXT, sel, (LPARAM)chset);

    sel = SendMessage (GetDlgItem (hWnd, ID_WEIGHT), LB_GETCURSEL, 0, 0);
    weight = weight_arg [sel];

    sel = SendMessage (GetDlgItem (hWnd, ID_SLANT), LB_GETCURSEL, 0, 0);
    slant = slant_arg [sel];

    sel = SendMessage (GetDlgItem (hWnd, ID_FLIP), LB_GETCURSEL, 0, 0);
    flip = flip_arg [sel];

    sel = SendMessage (GetDlgItem (hWnd, ID_SIZE), LB_GETCURSEL, 0, 0);

    return CreateLogFont (FONT_TYPE_NAME_ALL, family, 
                chset, weight, slant, flip, 
                FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, 
                FONT_STRUCKOUT_NONE , sel+MIN_FONT_SIZE, 0);

}

static int dialog_proc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    LOGFONT* font;
    LOGFONT* old_font;
    HWND hwnd_display;
    switch (message)
    {
        case MSG_INITDIALOG:
            SetWindowAdditionalData (hWnd, lParam);
            insert_listboxs (hWnd);

            hwnd_display = GetDlgItem(hWnd, ID_DISPLAY);
            /*FIXME needed ? save window default font*/
            old_font = GetWindowFont (hwnd_display);
            SetWindowAdditionalData (hwnd_display, (DWORD)old_font);

            font = create_sel_font(hWnd);
            if (font)
            {
                    SetWindowFont(hwnd_display, font);
                    UpdateWindow(hwnd_display, TRUE);
            }
            break;

        case MSG_COMMAND:
            hwnd_display = GetDlgItem(hWnd, ID_DISPLAY);
            switch (LOWORD (wParam))
            {
                case ID_FAMILY:
                    fresh_chset_list (hWnd);

                case ID_CHARSET:
                case ID_WEIGHT:
                case ID_SLANT:
                case ID_FLIP:
                case ID_SIZE:
                    if (HIWORD (wParam) == LBN_SELCHANGE)
                    {
                        font = create_sel_font(hWnd);
                        if (font)
                        {
                            old_font = SetWindowFont(hwnd_display, font);
                            UpdateWindow(hwnd_display, TRUE);
                            DestroyLogFont(old_font);
                        }
                    }
                    break;

                case IDOK:
                    EndDialog (hWnd, (DWORD)GetWindowFont (hwnd_display));
                    //EndDialog (hWnd, 0);
                        break;

                case IDCANCEL:
                    font = GetWindowFont (hwnd_display);
                    SetWindowFont (hwnd_display,
                            (PLOGFONT)GetWindowAdditionalData(hwnd_display));
                    DestroyLogFont (font);
                    EndDialog (hWnd, 0);
                    break;
                default:
                    break;
            }
            break;
    }
    return DefaultDialogProc (hWnd, message, wParam, lParam);
}

/*listbox --  family, charset, weight, slant, tcip
 * static box -- display
 * button OK CANCEL*/

LOGFONT* font_sel_dialog (HWND hWnd)
{
    FAMILY* fami_head;
    LOGFONT* font;
    int i;

    dlg_temp.w <<= 1;
    dlg_temp.h <<= 1;

    for (i=0; i<ARRAY_LEN(ctrls); i++)
    {
        ctrls[i].x <<= 1;
        ctrls[i].y <<= 1;
        ctrls[i].w <<= 1;
        ctrls[i].h <<= 1;
    }

    fami_head = create_font_attrlist ();
    font = (LOGFONT*)DialogBoxIndirectParam(&dlg_temp, hWnd, 
            dialog_proc, (LPARAM)fami_head);

    free_font_attrlist (fami_head);

    dlg_temp.w >>= 1;
    dlg_temp.h >>= 1;

    for (i=0; i<ARRAY_LEN(ctrls); i++)
    {
        ctrls[i].x >>= 1;
        ctrls[i].y >>= 1;
        ctrls[i].w >>= 1;
        ctrls[i].h >>= 1;
    }

    return font;
}
