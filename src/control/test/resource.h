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
#define IDM_NEW         110
#define IDM_OPEN        120
#define IDM_SAVE        130
#define IDM_SAVEAS      140
#define IDM_CLOSE       150
#define IDM_EXIT        160

#define IDM_DIALOG1     210
#define IDM_DIALOG2     220
#define IDM_DIALOG3     230
#define IDM_DIALOG4     240
#define IDM_DIALOG5     250

#define IDM_ABOUT       310

#define IDC_MODE_MDI					101
#define IDC_MODE_APP					102
#define IDC_MODE_IIIE					103
#define IDC_MODE_SETTINGS				104
#define IDC_MODE_PLC					105
#define IDC_PROMPTINFO					111
#define IDC_PROMPTINFO2					1111
#define IDC_BOX1                        112
#define IDC_BOX2                        113
#define IDC_BOX3                        114
#define IDC_BOX4                        115

#define IDC_PROGRESS					201
#define IDC_PROGRESS2					2011

#define IDC_EDIT1                       1011
#define IDC_EDIT2                       1012
#define IDC_EDIT3                       1013
#define IDC_EDIT4                       1014
#define IDC_EDIT5                       1015
#define IDC_EDIT6                       1016
#define IDC_EDIT7                       1017
#define IDC_EDIT8                       1018
#define IDC_EDIT9                       1019
#define IDC_EDITA                       1020
#define IDC_EDITB                       1021
#define IDC_EDITC                       1022

#define IDC_NEWPASSWORD                 1050
#define IDC_PASSWORDVALID               1051

#define IDC_UTMOSTCONTENT                 1050
#define IDC_UTMOSTSTART                   1051


#define IDC_PROPSHEET                   1060
#define IDC_APPLY                       1061

#define IDC_SCROLLBAR                   1080
#define IDC_SCROLLBAR_MESSAGE_BUTTON    1081

extern DLGTEMPLATE DlgInitProgress;
extern CTRLDATA CtrlInitProgress[];

extern DLGTEMPLATE DlgStartupMode;
extern CTRLDATA CtrlStartupMode[];

extern DLGTEMPLATE DlgStructParams;
extern CTRLDATA CtrlStructParams[];

extern DLGTEMPLATE DlgPassword;
extern CTRLDATA CtrlPassword[];

extern DLGTEMPLATE DlgPropertySheet;
extern CTRLDATA CtrlPropertySheet[];

extern DLGTEMPLATE Dlgutmost;
extern CTRLDATA Ctrlutmost[];

