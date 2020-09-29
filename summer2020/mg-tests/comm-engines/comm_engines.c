///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
** commlcd.c: Sample program for CommLCD NEWGAL engine.
**
** Copyright (C) 2003 ~ 2018 FMSoft (http://www.fmsoft.cn).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_LAMIAN              101
#define IDC_CHOUDOUFU           102
#define IDC_JIANBING            103
#define IDC_MAHUA               104
#define IDC_SHUIJIAO            105

#define IDC_XIAN                110
#define IDC_LA                  111

#define IDC_PROMPT              200

#include "text_en.h"

static DLGTEMPLATE DlgYourTaste =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    0, 0, 370, 300,
    what_flavor_snack_do_you_like,
    0, 0,
    12, NULL,
    0
};

static CTRLDATA CtrlYourTaste[] =
{
    {
        "static",
        WS_VISIBLE | SS_GROUPBOX,
        16, 10, 230, 160,
        IDC_STATIC,
        optional_snack,
        0,
        WS_EX_TRANSPARENT
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP,
        36, 38, 200, 20,
        IDC_LAMIAN,
        northwest_pulled_noodle,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 64, 200, 20,
        IDC_CHOUDOUFU,
        chang_sha_bad_smelling_bean_curd,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DISABLED,
        36, 90, 200, 20,
        IDC_JIANBING,
        shan_dong_thini_pancake,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 116, 200, 20,
        IDC_MAHUA,
        tianjin_fired_dough_twist,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTORADIOBUTTON,
        36, 142, 200, 20,
        IDC_SHUIJIAO,
        chengdu_red_oil_boiled_dumpling,
        0
    },
    {
        "static",
        WS_VISIBLE | SS_GROUPBOX | WS_GROUP,
        250, 10, 100, 160,
        IDC_STATIC,
        flavor,
        0,
        WS_EX_TRANSPARENT
    },
    {
        "button",
        WS_VISIBLE | BS_AUTOCHECKBOX,
        260, 38, 80, 20,
        IDC_XIAN,
        partial_salty,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKED,
        260, 64, 80, 20,
        IDC_LA,
        partial_spicy,
        0
    },
    {
        "static",
        WS_VISIBLE | SS_LEFT | WS_GROUP,
        16, 180, 360, 40,
        IDC_PROMPT,
        northwest_pulled_noodle_is_competitive_product_in_the_wheaten_food,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
        70, 230, 70, 30,
        IDOK,
        OK,
        0
    },
    {
        "button",
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        200, 230, 70, 30,
        IDCANCEL,
        Cancel,
        0
    },
};


static void my_notif_proc (HWND hwnd, LINT id, int nc, DWORD add_data)
{
    if (nc == BN_CLICKED) {
        SetWindowText (GetDlgItem (GetParent (hwnd), IDC_PROMPT), prompts [id - IDC_LAMIAN]);
    }
}

static LRESULT DialogBoxProc2 (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
        {
            int i;
            for (i = IDC_LAMIAN; i <= IDC_SHUIJIAO; i++)
                SetNotificationCallback (GetDlgItem (hDlg, i), my_notif_proc);
        }
        return 1;

    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
#include <wiringPi.h>
int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "button" , 0 , 0);
#endif

    DlgYourTaste.controls = CtrlYourTaste;

    DialogBoxIndirectParam (&DlgYourTaste, HWND_DESKTOP, DialogBoxProc2, 0L);

    return 0;
}


#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

