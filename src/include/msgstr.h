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
** msgstr.h: the text name of all messages.
**
** Create date: 1999/01/03
*/

// message string, Group 1: 0x0000 ~ 0x006F
const char *__mg_msgstr1 [] =
{
    "MSG_SYNCMSG, MSG_NULLMSG",     // 0x0000
    "MSG_LBUTTONDOWN         ",     // 0x0001
    "MSG_LBUTTONUP           ",     // 0x0002
    "MSG_LBUTTONDBLCLK       ",     // 0x0003
    "MSG_MOUSEMOVE           ",     // 0x0004
    "MSG_RBUTTONDOWN         ",     // 0x0005
    "MSG_RBUTTONUP           ",     // 0x0006
    "MSG_RBUTTONDBLCLK       ",     // 0x0007
    "MSG_NCLBUTTONDOWN       ",     // 0x0008
    "MSG_NCLBUTTONUP         ",     // 0x0009
    "MSG_NCLBUTTONDBLCLK     ",     // 0x000A
    "MSG_NCMOUSEMOVE         ",     // 0x000B
    "MSG_NCRBUTTONDOWN       ",     // 0x000C
    "MSG_NCRBUTTONUP         ",     // 0x000D
    "MSG_NCRBUTTONDBLCLK     ",     // 0x000E
    "",                             // 0x000F
    
    "MSG_KEYDOWN             ",     // 0x0010
    "MSG_CHAR                ",     // 0x0011
    "MSG_KEYUP               ",     // 0x0012
    "MSG_SYSKEYDOWN          ",     // 0x0013
    "MSG_SYSCHAR             ",     // 0x0014
    "MSG_SYSKEYUP            ",     // 0x0015
    "MSG_KEYLONGPRESS        ",     // 0x0016
    "MSG_KEYALWAYSPRESS      ",     // 0x0017
    "MSG_KEYSYM              ",     // 0x0018
    "MSG_UTF8CHAR            ",     // 0x0019
    "",                             // 0x001A
    "",                             // 0x001B
    "",                             // 0x001C
    "",                             // 0x001D
    "",                             // 0x001E
    "",                             // 0x001F
    
    
    "MSG_SETCURSOR           ",     // 0x0020
    "MSG_NCHITTEST, MSG_HITTEST",   // 0x0021
    "MSG_CHANGESIZE          ",     // 0x0022
    "MSG_QUERYNCRECT         ",     // 0x0023
    "MSG_QUERYCLIENTAREA     ",     // 0x0024
    "MSG_SIZECHANGING        ",     // 0x0025
    "MSG_SIZECHANGED         ",     // 0x0026
    "MSG_CSIZECHANGED        ",     // 0x0027
    "",                             // 0x0028
    "",                             // 0x0029
    "",                             // 0x002A
    "",                             // 0x002B
    "",                             // 0x002C
    "",                             // 0x002D
    "",                             // 0x002E
    "",                             // 0x002F
    
    "MSG_SETFOCUS            ",     // 0x0030
    "MSG_KILLFOCUS           ",     // 0x0031
    "MSG_MOUSEACTIVE         ",     // 0x0032
    "MSG_ACTIVE              ",     // 0x0033
    "MSG_CHILDHIDDEN         ",     // 0x0034
    "",                             // 0x0035
    "",                             // 0x0036
    "",                             // 0x0037
    "",                             // 0x0038
    "",                             // 0x0039
    "",                             // 0x003A
    "",                             // 0x003B
    "",                             // 0x003C
    "",                             // 0x003D
    "",                             // 0x003E
    "",                             // 0x003F
    
    "MSG_ACTIVEMENU          ",     // 0x0040
    "MSG_DEACTIVEMENU        ",     // 0x0041
    "MSG_HSCROLL             ",     // 0x0042
    "MSG_VSCROLL             ",     // 0x0043
    "MSG_NCSETCURSOR         ",     // 0x0044
    "",                             // 0x0045
    "",                             // 0x0046
    "",                             // 0x0047
    "",                             // 0x0048
    "",                             // 0x0049
    "",                             // 0x004A
    "",                             // 0x004B
    "",                             // 0x004C
    "",                             // 0x004D
    "",                             // 0x004E
    "",                             // 0x004F

    "MSG_MOUSEMOVEIN         ",     // 0x0050
    "MSG_WINDOWDROPPED       ",     // 0x0051
    "",                             // 0x0052
    "",                             // 0x0053
    "",                             // 0x0054
    "",                             // 0x0055
    "",                             // 0x0056
    "",                             // 0x0057
    "",                             // 0x0058
    "",                             // 0x0059
    "",                             // 0x005A
    "",                             // 0x005B
    "",                             // 0x005C
    "",                             // 0x005D
    "",                             // 0x005E
    "",                             // 0x005F
    
    "MSG_CREATE              ",     // 0x0060
    "MSG_NCCREATE            ",     // 0x0061
    "MSG_INITPANES           ",     // 0x0062
    "MSG_DESTROYPANES        ",     // 0x0063
    "MSG_DESTROY             ",     // 0x0064
    "MSG_NCDESTROY           ",     // 0x0065
    "MSG_CLOSE               ",     // 0x0066
    "MSG_NCCALCSIZE          ",     // 0x0067
    "MSG_MAXIMIZE            ",     // 0x0068
    "MSG_MINIMIZE            ",     // 0x0069
    "MSG_HELP                ",     // 0x006A
    "",                             // 0x006B
    "",                             // 0x006C
    "",                             // 0x006D
    "",                             // 0x006E
    ""                              // 0x006F
};


// message string, Group 2: 0x00A0 ~ 0x010F
char * __mg_msgstr2 [] =
{
    "MSG_SHOWWINDOW          ",     // 0x00A0
    "",                             // 0x00A1
    "",                             // 0x00A2
    "",                             // 0x00A3
    "",                             // 0x00A4
    "",                             // 0x00A5
    "",                             // 0x00A6
    "",                             // 0x00A7
    "",                             // 0x00A8
    "",                             // 0x00A9
    "",                             // 0x00AA
    "",                             // 0x00AB
    "",                             // 0x00AC
    "",                             // 0x00AD
    "",                             // 0x00AE
    "",                             // 0x00AF
    
    "MSG_ERASEBKGND          ",     // 0x00B0       // this is an async message.
    "MSG_PAINT               ",     // 0x00B1
    "MSG_NCPAINT             ",     // 0x00B2
    "MSG_NCACTIVATE          ",     // 0x00B3
    "MSG_SYNCPAINT           ",     // 0x00B4
    "",                             // 0x00B5
    "",                             // 0x00B6
    "",                             // 0x00B7
    "",                             // 0x00B8
    "",                             // 0x00B9
    "",                             // 0x00BA
    "",                             // 0x00BB
    "",                             // 0x00BC
    "",                             // 0x00BD
    "",                             // 0x00BE
    "",                             // 0x00BF
    "",                             // 0x00C0
    "",                             // 0x00C1
    "",                             // 0x00C2
    "",                             // 0x00C3
    "",                             // 0x00C4
    "",                             // 0x00C5
    "",                             // 0x00C6
    "",                             // 0x00C7
    "",                             // 0x00C8
    "",                             // 0x00C9
    "",                             // 0x00CA
    "",                             // 0x00CB
    "",                             // 0x00CC
    "",                             // 0x00CD
    "",                             // 0x00CE
    "",                             // 0x00CF
    
    "MSG_STARTSESSION        ",     // 0x00D0
    "MSG_QUERYENDSESSION     ",     // 0x00D1
    "MSG_ENDSESSION          ",     // 0x00D2
    "MSG_REINITSESSION       ",     // 0x00D3
    "",                             // 0x00D4
    "",                             // 0x00D5
    "",                             // 0x00D6
    "",                             // 0x00D7
    "",                             // 0x00D8
    "",                             // 0x00D9
    "",                             // 0x00DA
    "",                             // 0x00DB
    "",                             // 0x00DC
    "",                             // 0x00DD
    "MSG_ERASEDESKTOP        ",     // 0x00DE
    "MSG_PAINTDESKTOP        ",     // 0x00DF

    "MSG_DT_LBUTTONDOWN      ",     // 0x00E0
    "MSG_DT_LBUTTONUP        ",     // 0x00E1
    "MSG_DT_LBUTTONDBLCLK    ",     // 0x00E2
    "MSG_DT_MOUSEMOVE        ",     // 0x00E3
    "MSG_DT_RBUTTONDOWN      ",     // 0x00E4
    "MSG_DT_RBUTTONUP        ",     // 0x00E5
    "MSG_DT_RBUTTONDBLCLK    ",     // 0x00E6
    "",                             // 0x00E7
    "MSG_DT_KEYDOWN          ",     // 0x00E8
    "MSG_DT_CHAR             ",     // 0x00E9
    "MSG_DT_KEYUP            ",     // 0x00EA
    "MSG_DT_SYSKEYDOWN       ",     // 0x00EB
    "MSG_DT_SYSCHAR          ",     // 0x00EC
    "MSG_DT_SYSKEYUP         ",     // 0x00ED
    "MSG_DT_KEYLONGPRESS     ",     // 0x00EE
    "MSG_DT_KEYALWAYSPRESS   ",     // 0x00EF

    "MSG_ADDNEWMAINWIN       ",     // 0x00F0
    "MSG_REMOVEMAINWIN       ",     // 0x00F1
    "MSG_MOVETOTOPMOST       ",     // 0x00F2 
    "MSG_SETACTIVEMAIN       ",     // 0x00F3
    "MSG_GETACTIVEMAIN       ",     // 0x00F4
    "MSG_SHOWMAINWIN         ",     // 0x00F5
    "MSG_HIDEMAINWIN         ",     // 0x00F6
    "MSG_MOVEMAINWIN         ",     // 0x00F7
    "MSG_SETCAPTURE          ",     // 0x00F8
    "MSG_GETCAPTURE          ",     // 0x00F9
    "MSG_ENDTRACKMENU        ",     // 0x00FA
    "MSG_TRACKPOPUPMENU      ",     // 0x00FB
    "MSG_CLOSEMENU           ",     // 0x00FC
    "MSG_SCROLLMAINWIN       ",     // 0x00FD
    "MSG_CARET_CREATE        ",     // 0x00FE
    "MSG_CARET_DESTROY       ",     // 0x00FF

    "MSG_ENABLEMAINWIN       ",     // 0x0100
    "MSG_ISENABLED           ",     // 0x0101
    "MSG_SETWINCURSOR",             // 0x0102
    "MSG_GETNEXTMAINWIN",           // 0x0103
    "",                             // 0x0104
    "",                             // 0x0105
    "",                             // 0x0106
    "",                             // 0x0107
    "",                             // 0x0108
    "",                             // 0x0109
    "MSG_SHOWGLOBALCTRL",           // 0x010A
    "MSG_HIDEGLOBALCTRL",           // 0x010B
    "",                             // 0x010C
    "",                             // 0x010D
    "",                             // 0x010E
    ""                              // 0x010F
};

// message string, Group 3: 0x00120 ~ 0x017F
char * __mg_msgstr3 [] =
{
    "MSG_COMMAND             ",     // 0x0120
    "MSG_SYSCOMMAND          ",     // 0x0121
    "MSG_GETDLGCODE          ",     // 0x0122
    "MSG_INITDIALOG          ",     // 0x0123
    "MSG_NEXTDLGCTRL         ",     // 0x0124
    "MSG_ENTERIDLE           ",     // 0x0125
    "MSG_DLG_GETDEFID        ",     // 0x0126
    "MSG_DLG_SETDEFID        ",     // 0x0127
    "MSG_ISDIALOG            ",     // 0x0128
    "MSG_INITPAGE            ",     // 0x0129
    "MSG_SHOWPAGE            ",     // 0x012A
    "MSG_SHEETCMD            ",     // 0x012B
    "",                             // 0x012C
    "",                             // 0x012D
    "",                             // 0x012E
    "",                             // 0x012F
    
    "MSG_FONTCHANGING        ",     // 0x0130
    "MSG_FONTCHANGED         ",     // 0x0131
    "MSG_GETTEXTLENGTH       ",     // 0x0132
    "MSG_GETTEXT             ",     // 0x0133
    "MSG_SETTEXT             ",     // 0x0134
    "MSG_ENABLE              ",     // 0x0135
    "",                             // 0x0136
    "",                             // 0x0137
    "",                             // 0x0138
    "",                             // 0x0139
    "",                             // 0x013A
    "",                             // 0x013B
    "",                             // 0x013C
    "",                             // 0x013D
    "",                             // 0x013E
    "",                             // 0x013F
    
    "MSG_QUIT                ",     // 0x0140
    "",                             // 0x0141
    "MSG_IDLE                ",     // 0x0142
    "MSG_TIMEOUT             ",     // 0x0143
    "MSG_TIMER               ",     // 0x0144
    "MSG_CARETBLINK          ",     // 0x0145
    "MSG_FDEVENT             ",     // 0x0146
    "",                             // 0x0147
    "",                             // 0x0148
    "",                             // 0x0149
    "",                             // 0x014A
    "",                             // 0x014B
    "",                             // 0x014C
    "",                             // 0x014D
    "",                             // 0x014E
    "",                             // 0x014F
    
    "MSG_DOESNEEDIME         ",     // 0x0150
    "MSG_IME_REGISTER        ",     // 0x0151
    "MSG_IME_UNREGISTER      ",     // 0x0152
    "MSG_IME_OPEN            ",     // 0x0153
    "MSG_IME_CLOSE           ",     // 0x0154
    "",                             // 0x0155
    "MSG_IME_SETSTATUS       ",     // 0x0156
    "MSG_IME_GETSTATUS       ",     // 0x0157
    "MSG_IME_SETTARGET       ",     // 0x0158
    "MSG_IME_GETTARGET       ",     // 0x0159
    "",                             // 0x015A
    "",                             // 0x015B
    "",                             // 0x015C
    "",                             // 0x015D
    "",                             // 0x015E
    "",                             // 0x015F
    
    "MSG_SHOWMENU            ",     // 0x0160
    "MSG_HIDEMENU            ",     // 0x0161
    "MSG_ADDTIMER            ",     // 0x0162
    "MSG_REMOVETIMER         ",     // 0x0163
    "MSG_RESETTIMER          ",     // 0x0164
    "MSG_WINDOWCHANGED       ",     // 0x0165
    "MSG_BROADCASTMSG        ",     // 0x0166
    "MSG_REGISTERWNDCLASS    ",     // 0x0167
    "MSG_UNREGISTERWNDCLASS  ",     // 0x0168
    "MSG_NEWCTRLINSTANCE     ",     // 0x0169
    "MSG_REMOVECTRLINSTANCE  ",     // 0x016A
    "MSG_GETCTRLCLASSINFO    ",     // 0x016B
    "MSG_CTRLCLASSDATAOP     ",     // 0x016C
    "MSG_REGISTERKEYHOOK     ",     // 0x016D
    "MSG_REGISTERMOUSEHOOK   ",     // 0x016E
    "",                             // 0x016F
    
    "MSG_INITMENU            ",     // 0x0170
    "MSG_INITMENUPOPUP       ",     // 0x0171
    "MSG_MENUSELECT          ",     // 0x0172
    "MSG_MENUCHAR            ",     // 0x0173
    "MSG_ENTERMENULOOP       ",     // 0x0174
    "MSG_EXITMENULOOP        ",     // 0x0175
    "MSG_CONTEXTMENU         ",     // 0x0176
    "MSG_NEXTMENU            ",     // 0x0177
    "",                             // 0x0178
    "",                             // 0x0179
    "",                             // 0x017A
    "",                             // 0x017B
    "",                             // 0x017C
    "",                             // 0x017D
    "",                             // 0x017E
    ""                              // 0x017F
};

