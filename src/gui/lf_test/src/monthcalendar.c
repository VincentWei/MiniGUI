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
** $Id: monthcalendar.c,v 1.11 2007-12-06 02:25:00 xwyan Exp $
**
** monthcalendar.c: The MonthCalendar control demo program.
**
** Copyright (C) 2001 ~ 2002 Zhong Shuyi.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Create date: 2001/11/01
*/

/*
**  This source is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public
**  License as published by the Free Software Foundation; either
**  version 2 of the License, or (at your option) any later version.
**
**  This software is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
**  MA 02111-1307, USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>

#define IDC_CTRL1 	10
#define IDC_CTRL2 	20
#define IDC_CTRL3 	30
#define IDC_CTRL4 	40
#define IDC_CTRL5 	50
#define IDC_CTRL6 	60
#define IDC_CTRL7 	70
#define IDC_CTRL8	80
#define IDC_CTRL9	90
#define IDC_CTRL10	100
#define IDC_CTRL11	110
#define IDC_CTRL12	120
#define IDC_CTRL13	130
#define IDC_CTRL14 	140
#define IDC_CTRL15	150
#define IDC_CTRL16	160
#define IDC_CTRL17	170

static HWND hMainWnd = HWND_INVALID;

static int ControlTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static HWND hChildWnd1, hMeditWnd1, hBtnWnd1, hBtnWnd2, hBtnWnd3, hBtnWnd4, 
			hBtnWnd5, hBtnWnd6, hBtnWnd7, hBtnWnd8, hBtnWnd9, hBtnWnd10,
			hBtnWnd11, hBtnWnd12, hMeditWnd2, hBtnWnd13, hBtnWnd14;

    switch (message) {
        case MSG_CREATE:
            hChildWnd1 = CreateWindow ("monthcalendar", 
                                        "monthcalendar", 
                                        WS_CHILD |
                                        WS_VISIBLE |
										MCS_ENG_L |
										MCS_NOTIFY, 
                                        IDC_CTRL1, 
                                        50, 20, 250, 180, hWnd, 0);
			hBtnWnd1 = CreateWindow ("button", 
									 "get current day",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL2,
									 140, 220, 160, 20, hWnd, 0);
			hMeditWnd1 = CreateWindow ("medit", 
									  NULL,
									  WS_CHILD | WS_BORDER |
									  WS_VISIBLE,
									  IDC_CTRL3,
									  350, 20, 230, 100, hWnd, 0);
		
			hBtnWnd2 = CreateWindow ("button", 
									 "get current month",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL4,
									 140, 260, 160, 20, hWnd, 0);
			hBtnWnd3 = CreateWindow ("button", 
									 "get current year",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL5,
									 140, 300, 160, 20, hWnd, 0);
			hBtnWnd4 = CreateWindow ("button", 
									 "set current day",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL6,
									 140, 340, 160, 20, hWnd, 0);
			hBtnWnd5 = CreateWindow ("button", 
									 "set current month",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL7,
									 140, 380, 160, 20, hWnd, 0);
			hBtnWnd6 = CreateWindow ("button", 
									 "set current year",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL8,
									 140, 420, 160, 20, hWnd, 0);
			hBtnWnd7 = CreateWindow ("button", 
									 "set current date as today",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL9,
									 140, 460, 160, 20, hWnd, 0);
			hBtnWnd8 = CreateWindow ("button", 
									 "get first day of week",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL10,
									 140, 500, 160, 20, hWnd, 0);
			hBtnWnd9 = CreateWindow ("button", 
									 "get current date",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL11,
									 350, 220, 160, 20, hWnd, 0);
			hBtnWnd10 = CreateWindow ("button", 
									 "get today",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL12,
									 350, 260, 160, 20, hWnd, 0);
			hBtnWnd11 = CreateWindow ("button", 
									 "get minreqrectsize",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL13,
									 350, 300, 160, 20, hWnd, 0);
			hBtnWnd12 = CreateWindow ("button", 
									 "set current date",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL14,
									 350, 340, 160, 20, hWnd, 0);
			hMeditWnd2 = CreateWindow ("medit", 
									  NULL,
									  WS_CHILD | WS_BORDER |
									  WS_VISIBLE,
									  IDC_CTRL15,
									  350, 150, 230, 50, hWnd, 0);
			hBtnWnd13 = CreateWindow ("button", 
									 "set color",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL16,
									 350, 380, 160, 20, hWnd, 0);
			hBtnWnd14 = CreateWindow ("button", 
									 "get color",
									 WS_CHILD 
									 | BS_PUSHBUTTON 
									 | WS_VISIBLE, 
									 IDC_CTRL17,
									 350, 420, 160, 20, hWnd, 0);

        break;

        case MSG_COMMAND:
        {
            int id   = LOWORD(wParam);
            int code = HIWORD(wParam);

            switch (id) {
            	case IDC_CTRL2:
				{
					int day;
					char chday[50];
					day = SendMessage (hChildWnd1, MCM_GETCURDAY, 0, 0);
					sprintf (chday, "Current day=%d\n", day);
					SetWindowText (hMeditWnd1, chday);
				}
           	 	break;
				
				case IDC_CTRL4:
				{
					int month;
					char chmon[50];
					month = SendMessage (hChildWnd1, MCM_GETCURMONTH, 0, 0);
					sprintf (chmon, "Current month=%d\n", month);
					SetWindowText (hMeditWnd1, chmon);
				}
				break;

				case IDC_CTRL5:
				{
					int year;
					char chyear[50];
					year = SendMessage (hChildWnd1, MCM_GETCURYEAR, 0, 0);
					sprintf (chyear, "Current year=%d\n", year);
					SetWindowText (hMeditWnd1, chyear);
				}
				break;
			
				case IDC_CTRL6:
				{
					int newday = 31;
					SendMessage (hChildWnd1, MCM_SETCURDAY, newday, 0);
				}
				break;
				
				case IDC_CTRL7:
				{
					int newmonth = 2;
					SendMessage (hChildWnd1, MCM_SETCURMONTH, newmonth, 0);
				}
				break;
				
				case IDC_CTRL8:
				{
					int newyear = 1997;
					SendMessage (hChildWnd1, MCM_SETCURYEAR, newyear, 0);
				}
				break;
				
				case IDC_CTRL9:
				{
					SendMessage (hChildWnd1, MCM_SETTODAY, 0, 0);
				}
				break;
				
				case IDC_CTRL10:
				{
					int weekday1;
					char buffer[50];
					weekday1 = SendMessage (hChildWnd1, MCM_GETFIRSTWEEKDAY, 0, 0);
					sprintf (buffer, "first day of week =%d\n", weekday1);
					SetWindowText (hMeditWnd1, buffer);
				}
				break;
				
				case IDC_CTRL11:
				{
					SYSTEMTIME curdate;
					char buffer[300];
					SendMessage (hChildWnd1, MCM_GETCURDATE, 0, (LPARAM) &curdate);
					sprintf (buffer, "current year = %d\ncurrent month = %d\ncurrent day = %d\ncurrent day of week = %d\n",	curdate.year, curdate.month, curdate.day, curdate.weekday);
					SetWindowText (hMeditWnd1, buffer);
				}
				break;
				
				case IDC_CTRL12:
				{
					SYSTEMTIME today;
					char buffer[300];

					SendMessage (hChildWnd1, MCM_GETTODAY, 0, (LPARAM) &today);
					sprintf (buffer, "today is \nyear %d\nmonth %d\nday %d\nday of week %d\n",
									today.year, today.month, today.day, today.weekday);
					SetWindowText (hMeditWnd1, buffer);
				}
				break;
				
				case IDC_CTRL13:
				{
					int minw, minh;
					char buffer[100];

					minw = SendMessage (hChildWnd1, MCM_GETMINREQRECTW, 0, 0);
					minh = SendMessage (hChildWnd1, MCM_GETMINREQRECTH, 0, 0);
					sprintf (buffer, "the minimum required window width = %d\nthe minimum required window height =%d\n", minw, minh);
					SetWindowText (hMeditWnd1, buffer);
				}
				break;
				
				case IDC_CTRL14:
				{
					SYSTEMTIME curdate;
				
					curdate.year = 1997;
					curdate.month = 7;
					curdate.day = 1;
					SendMessage (hChildWnd1, MCM_SETCURDATE, 0, (LPARAM) &curdate);	
				}
				break;
			
				case IDC_CTRL1:
					switch (code) {
						case MCN_DATECHANGE:
							SetWindowText (hMeditWnd2, "the current date is changed\n");
						break;
					
					}
				break;
				
				case IDC_CTRL16:
				{
					MCCOLORINFO mcci;
					
					memset (&mcci, -1, 11*sizeof(int));
					//mcci.clr_titlebk 		= COLOR_darkgray;
					//mcci.clr_titletext 		= PIXEL_lightwhite;
					mcci.clr_arrow 			= PIXEL_darkblue;
					mcci.clr_arrowHibk 		= COLOR_yellow;
					mcci.clr_daybk			= COLOR_yellow;
					mcci.clr_dayHibk		= COLOR_magenta;
					mcci.clr_daytext		= PIXEL_darkblue;
					mcci.clr_trailingtext	= COLOR_green;
					mcci.clr_dayHitext		= COLOR_yellow;
					mcci.clr_weekcaptbk		= COLOR_darkgreen;
					mcci.clr_weekcapttext 	= COLOR_yellow;
				
					SendMessage (hChildWnd1, MCM_SETCOLOR, 0, (LPARAM) &mcci);
				}
				break;
				
				case IDC_CTRL17:
				{
					char buffer[300];
					MCCOLORINFO mcci;
					
					SendMessage (hChildWnd1, MCM_GETCOLOR, 0, (LPARAM) &mcci);
					sprintf (buffer, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", 
							mcci.clr_titlebk, mcci.clr_titletext, 
							mcci.clr_arrow, mcci.clr_arrowHibk,
							mcci.clr_daybk, mcci.clr_dayHibk,
							mcci.clr_daytext, mcci.clr_trailingtext,
							mcci.clr_dayHitext, mcci.clr_weekcaptbk,
							mcci.clr_weekcapttext);
					SetWindowText (hMeditWnd1, buffer);
				}
				break;
				
            	default:
            	break;
            }

        }
        break;
       
	case MSG_DESTROY:
            DestroyAllControls (hWnd);
            hMainWnd = HWND_INVALID;
	    return 0;

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            MainWindowCleanup (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX 
                                      | WS_MAXIMIZEBOX
                                      | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "MonthCalendar control" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(1);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlTestWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 640;
    pCreateInfo->by = 480;
    pCreateInfo->iBkColor = GetWindowElementColor (WE_MAINC_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void monthcalendar_demo (HWND hwnd)
{
	MAINWINCREATE CreateInfo;

    if (hMainWnd != HWND_INVALID) {
        ShowWindow (hMainWnd, SW_SHOWNORMAL);
        return;
    }

	InitCreateInfo(&CreateInfo);
    CreateInfo.hHosting = hwnd;

	hMainWnd = CreateMainWindow(&CreateInfo);

}

