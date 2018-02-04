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
** monthcalendar.c: the month calendar Control module.
**
** Created by Zhong Shuyi at 2001/01/02
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

#ifdef _MGCTRL_MONTHCAL
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/monthcal.h"
#include "monthcalendar_impl.h"

#define MINWNDRECT_W    200
#define MINWNDRECT_H    120

#define MONFIELD_W       50
#define YEARFIELD_W      50
#define WEEKFIELD_W      30
#define WEEKFIELD_H(hwnd)      (GetWindowFont(hwnd)->size+4)
#define WEEK_BORDER      5
#define WEEK_VBORDER1    2
#define WEEK_VBORDER2    3
#define TEXT_BORDER      5 
#define LINE_D           2

#define ARROW_H          7
#define ARROW_W          (ARROW_H+1)/2
#define ARROWRECT_W      (ARROW_W+4)

#define HEIGHT_BORDER    6
#define MON_WIDTH        60
#define MON_HEIGHT       14
#define MON_BORDER       15
#define YEAR_WIDTH       40
#define YEAR_HEIGHT      14
#define YEAR_BORDER      15
#define ARROW_BORDER     10

// color info
#define MCCLR_DF_TITLEBK        GetWindowElementPixel(hWnd, WE_MAINC_THREED_BODY)
#define MCCLR_DF_TITLETEXT      GetWindowElementPixel(hWnd, WE_FGC_THREED_BODY)

#define MCCLR_DF_ARROW          GetWindowElementPixel(hWnd, WE_FGC_WINDOW)
#define MCCLR_DF_ARROWHIBK      GetWindowElementPixel(hWnd, WE_BGC_WINDOW)

#define MCCLR_DF_DAYBK          GetWindowElementPixel(hWnd, WE_BGC_WINDOW)
#define MCCLR_DF_DAYTEXT        GetWindowElementPixel(hWnd, WE_FGC_WINDOW)

#define MCCLR_DF_DAYHIBK        GetWindowElementPixel(hWnd, WE_BGC_HIGHLIGHT_ITEM)
#define MCCLR_DF_DAYHITEXT      GetWindowElementPixel(hWnd, WE_FGC_HIGHLIGHT_ITEM)

#define MCCLR_DF_TRAILINGTEXT   GetWindowElementPixel(hWnd, WE_FGC_DISABLED_ITEM)

#define MCCLR_DF_WEEKCAPTBK     GetWindowElementPixel(hWnd, WE_BGC_HIGHLIGHT_ITEM)
#define MCCLR_DF_WEEKCAPTTEXT   GetWindowElementPixel(hWnd, WE_FGC_HIGHLIGHT_ITEM)

static LRESULT MonthCalendarCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMonthCalendarControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName    = "monthcalendar";
    WndClass.dwStyle        = WS_NONE;
    WndClass.dwExStyle      = WS_EX_NONE;
    WndClass.hCursor        = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor       = 
        GetWindowElementPixel (HWND_DESKTOP, WE_BGC_WINDOW);
    WndClass.WinProc        = MonthCalendarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

// draw page arrow
static void mcDrawPageArrow (HWND hWnd, WINDOWINFO *info, PMONCALDDATA mc_data, 
        HDC hdc, RECT* prcArrow, BOOL bFaceR, BOOL bHilight)
{
    PMCCOLORINFO pmcci;
    Uint8 r, g, b, a;
    DWORD color;
    int pixel, status;

    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    SetPenColor (hdc, pmcci->clr_arrow);

    if (bFaceR)
        status = LFRDR_ARROW_RIGHT;
    else
        status = LFRDR_ARROW_LEFT;

    if (bHilight) {
        status |= LFRDR_BTN_STATUS_HILITE;
        pixel = pmcci->clr_arrowHibk;
    }
    else
        pixel = pmcci->clr_arrow;

    Pixel2RGBA (hdc, pixel, &r, &g, &b, &a);
    color = MakeRGBA (r, g, b, a);

    info->we_rdr->draw_arrow (hWnd, hdc, prcArrow, 
            color, LFRDR_ARROW_HAVESHELL | status);
}

// --------------------------------------------------------------------------
static void mcMonthCalendarCleanUp (MONCALDDATA* mc_data)
{
    free ((PMCCOLORINFO) mc_data->dwClrData);
    return;
}

// --------------------------------------------------------------------------

// find out whether a year is a leap year
static BOOL IsLeapYear (int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) return TRUE;
    else return FALSE;
}


// get month length from year and month
static int GetMonLen (int year, int month)
{
    int mon_len;

    if (month < 1 || month > 12) return -1;
    if ((month <= 7 && month % 2 == 1) || (month >= 8 && month % 2 == 0))
        mon_len = 31;
    else if (month == 2) 
        if (IsLeapYear (year)) mon_len = 29;
        else mon_len = 28;
    else
        mon_len = 30;
    return mon_len;
}

static int GetYearLen (int year)
{
    int i, year_len = 0;
    for (i = 1; i <= 12; i++) {
        year_len += GetMonLen (year, i);    
    }
    return year_len;
}

// get previous month length from year and month
static int GetPMonLen (int year, int month)
{
    if (month > 1 && month <= 12) return GetMonLen (year, month - 1);
    else if (month == 1) return GetMonLen (year - 1, 12);
    else return -1;
}


static int MyGetWeekDay (int year, int month, int day)
{
    // days from 2036/12/31
    int weekday, weekday1 = 3, daylen = 0, i;

    if (year < 2037) return -1;
    for (i = 2037; i < year; i++) {
        daylen += GetYearLen (i);
    }
    for (i = 1; i < month; i++)    {
        daylen += GetMonLen (year, i);
    }    
    daylen += day;
    weekday = (weekday1 + daylen) - (int)((weekday1 + daylen) / 7)*7;
    return weekday;    
}

// get weekday from date
static int GetWeekDay (int year, int month, int day)
{
    struct tm nowday;

    if (year < 1970) return -1;
    if (month < 1 || month > 12) return -1;
    if (day < 1 || day > 31) return -1;
    
    if (year >= 2037) return MyGetWeekDay (year, month, day);
    memset (&nowday, 0, sizeof (nowday));
    nowday.tm_sec = 0;
    nowday.tm_min = 0;
    nowday.tm_hour = 0;
    nowday.tm_mday = day;
    nowday.tm_mon = month-1;
    nowday.tm_year = year-1900;

    if (__mg_mktime(&nowday) == -1) {
        return -1;
    }
    else
        return nowday.tm_wday;    
}
// get line and weekday from date according to weekday1
static void mcGetCurDayPos (PMONCALDDATA mc_data, int day, int* pline, int* pWeekDay)
{
    *pWeekDay = (mc_data->WeekDay1 + day - 1) % 7;
    *pline = (day + 6 - *pWeekDay - 1)/7;
}

/*
// get line and weekday from date
static void mcGetDayPos (int year, int month, int day, int* pline, int* pWeekDay)
{
    *pWeekDay = GetWeekDay (year, month, day);
    *pline = (day + 6 - *pWeekDay - 1)/7;    
}
*/

// get rects of arrows
static void mcGetArrowRects (RECT* prcMonth, RECT* prcYear, 
                                RECT* prcAML, RECT* prcAMR, 
                                RECT* prcAYL, RECT* prcAYR)
{
    prcAML->right = prcMonth->left - 1;
    prcAML->left = prcAML->right - ARROWRECT_W;
    prcAML->top = prcMonth->top;
    prcAML->bottom = prcMonth->bottom;
    
    prcAMR->left = prcMonth->right + 2;    
    prcAMR->right = prcAMR->left + ARROWRECT_W;
    prcAMR->top = prcMonth->top;
    prcAMR->bottom = prcMonth->bottom;

    prcAYL->right = prcYear->left - 2;
    prcAYL->left = prcAYL->right - ARROWRECT_W;
    prcAYL->top = prcYear->top;
    prcAYL->bottom = prcYear->bottom;
    
    prcAYR->left = prcYear->right + 2;    
    prcAYR->right = prcAYR->left + ARROWRECT_W;
    prcAYR->top = prcYear->top;
    prcAYR->bottom = prcYear->bottom;
}

// get rect of month day area
static void mcGetMDayRect (HWND hwnd, RECT* prcClient, RECT* prcMDay)
{
    prcMDay->left = prcClient->left + WEEK_BORDER;
    prcMDay->right = prcClient->right - WEEK_BORDER;
    prcMDay->top = prcClient->top + 2 + DATECAP_H(hwnd) + 2*WEEK_VBORDER2 + WEEKFIELD_H(hwnd);
    prcMDay->bottom = prcClient->bottom - WEEK_VBORDER2;
}

// get rects of ...
static void mcGetRects (HWND hWnd,  RECT* prcClient,
                                    RECT* prcMonth,
                                    RECT* prcYear,
                                    RECT* prcMDay)
{
    RECT rcClient;

    GetClientRect (hWnd, &rcClient);
    rcClient.right --;
    rcClient.bottom --;

    if (prcClient) {
        *prcClient = rcClient;
    }    
    
    if (prcMonth) 
        SetRect (prcMonth, rcClient.left + MON_BORDER,
                      //rcClient.top + 4,
                      rcClient.top + 6,
                      rcClient.left + MON_BORDER + MON_WIDTH,
                      rcClient.top + 2 + DATECAP_H(hWnd) - 4);

    if (prcYear)
        SetRect (prcYear, rcClient.right - YEAR_BORDER - YEAR_WIDTH,
                      //rcClient.top + 4,
                      rcClient.top + 6,
                      rcClient.right - YEAR_BORDER,
                      rcClient.top + 2 + DATECAP_H(hWnd) - 4);

    if (prcMDay)
        mcGetMDayRect (hWnd, &rcClient, prcMDay);
}

// get item rect from the x-y mouse position
static void mcGetItemRectFromPos (PMONCALDDATA mc_data,    RECT* prcMDay, 
                            RECT* prcItem, int* pline, int* pweekday, int x, int y)
{
    *pline = (y - prcMDay->top) / mc_data->item_h;
    *pweekday = (x - prcMDay->left) / mc_data->item_w;
    prcItem->left = prcMDay->left + *pweekday*mc_data->item_w;
    prcItem->right = prcItem->left + mc_data->item_w;
    prcItem->top = prcMDay->top + *pline*mc_data->item_h;
    prcItem->bottom = prcItem->top + mc_data->item_h;
}

static void mcGetItemRectFromDay (PMONCALDDATA mc_data, RECT* prcMDay, 
                            RECT* prcItem, int day)
{
    int LineIndex, weekday;

    LineIndex = (mc_data->WeekDay1 + day - 1)/7;
    weekday = (mc_data->WeekDay1 + day - 1)%7;

    prcItem->left = prcMDay->left + weekday*mc_data->item_w;
    prcItem->right = prcItem->left + mc_data->item_w;
    prcItem->top = prcMDay->top + LineIndex*mc_data->item_h;
    prcItem->bottom = prcItem->top + mc_data->item_h;
}

// get current item rect from mc_data
static void mcGetCurRect (RECT* prcMDay, RECT* prcItem, PMONCALDDATA mc_data)
{
    prcItem->left = prcMDay->left + mc_data->cur_WeekDay*mc_data->item_w;    
    prcItem->top = prcMDay->top + mc_data->cur_line*mc_data->item_h;
    prcItem->right = prcItem->left + mc_data->item_w;
    prcItem->bottom = prcItem->top + mc_data->item_h;
}

// textout in a rect center style
static void mcTextOutCenter (HWND hWnd, HDC hdc, RECT* prcText, const char* pchText)
{

#if 0
    int ch_w, ch_h, x, y;
    int bkMode;
    
    ch_w = GetSysCharWidth ();
    ch_h = GetSysCharHeight ();
    x = (prcText->right + prcText->left)/2 - strlen(pchText)*ch_w/2;
    y = (prcText->bottom + prcText->top)/2 - ch_h/2+1;
    if (pchText) {
        bkMode = SetBkMode (hdc, BM_TRANSPARENT);
        TextOut (hdc, x, y, pchText, -1);
        SetBkMode (hdc, bkMode);
    }
#else

    if (pchText) {
        int bkMode;

        bkMode = SetBkMode (hdc, BM_TRANSPARENT);
        //SetBkColor (hdc, GetWindowBkColor(hWnd));
        //SetTextColor (hdc, pmcci->clr_weekcapttext);
        DrawText (hdc, pchText, -1, prcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        SetBkMode (hdc, bkMode);
    }
#endif
}

static void mcUnHilightRect (HWND hWnd, PMONCALDDATA mc_data, HDC hdc, RECT* prcItem, int day)
{
    int item_w, item_h;
    char daytext[3];
    PMCCOLORINFO pmcci;
    
    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    item_w = prcItem->right - prcItem->left;
    item_h = prcItem->bottom - prcItem->top; 
    SetBrushColor (hdc, pmcci->clr_daybk);
    FillBox (hdc, prcItem->left, prcItem->top, item_w, item_h);

    if (day < 10)
        sprintf (daytext, " %d", day);
    else
        sprintf (daytext, "%d", day);
    SetBkColor(hdc, pmcci->clr_dayHibk);
    //SetBkColor(hdc, pmcci->clr_dayHibk);
    SetTextColor (hdc, pmcci->clr_daytext);
    mcTextOutCenter (hWnd, hdc, prcItem, daytext);
}

static void mcHilightRect (HWND hWnd, PMONCALDDATA mc_data, HDC hdc, RECT* prcItem, int day)
{
    int item_w, item_h;
    char daytext[3];
    PMCCOLORINFO pmcci;
    
    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    SetBrushColor (hdc, pmcci->clr_dayHibk);
    item_w = prcItem->right - prcItem->left;
    item_h = prcItem->bottom - prcItem->top;
    FillBox (hdc, prcItem->left, prcItem->top, item_w, item_h);
    SetPenColor(hdc, GetWindowElementPixel (hWnd, WE_MAINC_THREED_BODY));
    Rectangle (hdc, prcItem->left, prcItem->top, prcItem->left+item_w-1, prcItem->top+item_h-1);
    if (day < 10)
        sprintf (daytext, " %d", day);
    else
        sprintf (daytext, "%d", day);
    SetTextColor (hdc, pmcci->clr_dayHitext);
    mcTextOutCenter (hWnd, hdc, prcItem, daytext);
}

/* draw customed day color */
static void mcDrawCustomedDay (HWND hWnd, HDC hdc, PMONCALDDATA mc_data, RECT* prcMDay, 
                                int newday)
{
    char daytext[3];
    gal_pixel oldcolor;
    
    if (newday < 10)
        sprintf (daytext, " %d", newday);
    else
        sprintf (daytext, "%d", newday);
    oldcolor = SetTextColor (hdc, mc_data->customed_day_color[newday]);
    mcTextOutCenter (hWnd, hdc, prcMDay, daytext);
    SetTextColor (hdc, oldcolor);
}

// draw change day
static void mcDrawDay (HWND hWnd, HDC hdc, PMONCALDDATA mc_data, RECT* prcMDay, 
                                int newday)
{
    RECT rcPItemDay, rcItemDay;
    // NUV PMCCOLORINFO pmcci;
    
    // NUV pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    mcGetCurRect (prcMDay, &rcPItemDay, mc_data);
    mcUnHilightRect (hWnd, mc_data, hdc, &rcPItemDay, mc_data->cur_day);    

    mc_data->cur_day = newday;
    mcGetCurDayPos (mc_data, mc_data->cur_day, &mc_data->cur_line, 
                        &mc_data->cur_WeekDay);    
    mcGetCurRect (prcMDay, &rcItemDay, mc_data);
    mcHilightRect (hWnd, mc_data, hdc, &rcItemDay, mc_data->cur_day);         
}

// draw month day area
static void mcDrawMonthDay (HWND hWnd, HDC hdc, RECT* prcMDay, PMONCALDDATA mc_data)
{
    int i, WeekDayPM = 0, LineIndex = 0;
    int mdaypm = 0, MonLenPM, mdaynm;
    int iWeekDay = 0;
    char chMonthDay[3];
    RECT rcMonthDay;
    PMCCOLORINFO pmcci;
    
    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    SetBkColor (hdc, pmcci->clr_daybk);    
    SetTextColor (hdc, pmcci->clr_daytext);

    for (i = 1; i <= mc_data->monlen; i++) {
        if (i < 10) 
            sprintf (chMonthDay, " %d", i);
        else
            sprintf (chMonthDay, "%d", i);
        iWeekDay = (mc_data->WeekDay1 + i - 1)%7;
        rcMonthDay.left = prcMDay->left + iWeekDay*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        LineIndex = (mc_data->WeekDay1 + i - 1)/7;
        rcMonthDay.top = prcMDay->top + mc_data->item_h*LineIndex;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        if (i == mc_data->cur_day) {
            SetTextColor (hdc, pmcci->clr_dayHitext);
            SetBkColor (hdc, pmcci->clr_dayHibk);
            SetBrushColor (hdc, pmcci->clr_dayHibk);
            FillBox (hdc, rcMonthDay.left, rcMonthDay.top, 
                    mc_data->item_w, mc_data->item_h);

            SetPenColor(hdc, GetWindowElementPixel (hWnd, WE_MAINC_THREED_BODY));
            Rectangle (hdc, rcMonthDay.left, rcMonthDay.top, 
                    rcMonthDay.left+mc_data->item_w-1 , 
                    rcMonthDay.top+mc_data->item_h-1);
        }
        else {
            SetBkColor (hdc, pmcci->clr_daybk);
            SetTextColor (hdc, pmcci->clr_daytext);
            SetBrushColor (hdc, pmcci->clr_daybk);
            FillBox (hdc, rcMonthDay.left, rcMonthDay.top, 
                    mc_data->item_w, mc_data->item_h);
        }

        if (mc_data->customed_day & (1<<i)) {
            SetTextColor (hdc, mc_data->customed_day_color[i]);

            SetBrushColor (hdc, pmcci->clr_daybk);
            FillBox (hdc, rcMonthDay.left, rcMonthDay.top, 
                    mc_data->item_w, mc_data->item_h);
        }
        mcTextOutCenter (hWnd, hdc, &rcMonthDay, chMonthDay);
    }

    SetTextColor (hdc, pmcci->clr_trailingtext);
    SetBrushColor (hdc, pmcci->clr_daybk);
    
    LineIndex += (iWeekDay+1)/7;
    iWeekDay = (iWeekDay + 1)%7;
    mdaynm = 1;
    while (LineIndex <= 5) {
        if (mdaynm < 10) 
            sprintf (chMonthDay, " %d", mdaynm);
        else
            sprintf (chMonthDay, "%d", mdaynm);
        rcMonthDay.left = prcMDay->left + iWeekDay*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        rcMonthDay.top = prcMDay->top + mc_data->item_h*LineIndex;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        FillBox (hdc, rcMonthDay.left, rcMonthDay.top, 
                mc_data->item_w, mc_data->item_h);
        mcTextOutCenter (hWnd, hdc, &rcMonthDay, chMonthDay);
        mdaynm++;
        iWeekDay++;
        if (iWeekDay == 7) {
            iWeekDay = 0;
            LineIndex++;
        }
    }
    
    WeekDayPM = mc_data->WeekDay1 - 1;
    if (WeekDayPM >= 0) {
        rcMonthDay.top = prcMDay->top;
        rcMonthDay.bottom = rcMonthDay.top + mc_data->item_h;
        rcMonthDay.left = prcMDay->left + WeekDayPM*mc_data->item_w;
        rcMonthDay.right = rcMonthDay.left + mc_data->item_w;
        MonLenPM = GetPMonLen (mc_data->cur_year, mc_data->cur_month);
        mdaypm = MonLenPM;
    }

    while (WeekDayPM >= 0) {
        sprintf (chMonthDay, "%d", mdaypm);
        FillBox (hdc, rcMonthDay.left, rcMonthDay.top, 
                mc_data->item_w, mc_data->item_h);
        mcTextOutCenter (hWnd, hdc, &rcMonthDay, chMonthDay);
        OffsetRect(&rcMonthDay, -mc_data->item_w, 0);
        mdaypm--;
        WeekDayPM--;
    }
}

// draw change month
static void mcDrawMonth (HWND hWnd, HDC hdc, PMONCALDDATA mc_data, RECT* prcMDay, 
                            RECT* prcMonth, int newmonth)
{
    if (newmonth > 12) 
        mc_data->cur_month = 12;
    else if (newmonth < 1) {
        mc_data->cur_month = 1;
    } else 
        mc_data->cur_month = newmonth; 
    mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);
    while (mc_data->cur_day > mc_data->monlen) {
        mc_data->cur_day--;
    }
    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);
    mcGetCurDayPos (mc_data, mc_data->cur_day, &mc_data->cur_line, 
                        &mc_data->cur_WeekDay);    
    InvalidateRect (hWnd, prcMonth, FALSE);
    InvalidateRect (hWnd, prcMDay, FALSE);
}

static void mcDrawYear (HWND hWnd, HDC hdc, PMONCALDDATA mc_data, RECT* prcMDay, 
                            RECT* prcYear, int newyear)
{
    if (newyear < 1970)    mc_data->cur_year = 1970;
    else    mc_data->cur_year = newyear;
    mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);
    while (mc_data->cur_day > mc_data->monlen) {
        mc_data->cur_day--;
    }
    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);
    mcGetCurDayPos (mc_data, mc_data->cur_day, &mc_data->cur_line, 
                        &mc_data->cur_WeekDay);    
    InvalidateRect (hWnd, prcYear, FALSE);
    InvalidateRect (hWnd, prcMDay, FALSE);
}


static const char *chMon_EL[] = {"January", "Febuary", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
static const char *chMon_ES[] = {"Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug",
                            "Sep", "Oct", "Nov", "Dec"};
static const char *chMon_C[] = {"一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", 
                            "九月", "十月", "十一月", "十二月"};
static const char *chWeek_C[] = {"日", "一", "二", "三", "四", "五", "六"}; 
static const char *chWeek_E[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *chWeek_E_S[] = {"S", "1", "2", "3", "4", "5", "6"};

// draw the whole calendar
static void mcDrawCalendar (HWND hWnd, HDC hdc, PMONCALDDATA mc_data)
{
    RECT rcClient, rcMonth, rcYear, rcMDay, rc;
    RECT rcAML, rcAMR, rcAYL, rcAYR;
    DWORD dwStyle;
    PMCCOLORINFO pmcci;
    // NUV UINT uFormat;
    WINDOWINFO *info = (WINDOWINFO*)GetWindowInfo (hWnd);
    DWORD color;

    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    dwStyle = GetWindowStyle (hWnd);
    mcGetRects (hWnd, &rcClient, &rcMonth, &rcYear, &rcMDay);

    color = GetWindowElementAttr (hWnd, WE_MAINC_THREED_BODY);
    
    /* draw client frame */
//   if(GetWindowExStyle (hWnd) & WS_EX_TRANSPARENT)
        info->we_rdr->draw_3dbox (hdc, &rcClient, color,
                    LFRDR_3DBOX_THICKFRAME| LFRDR_BTN_STATUS_PRESSED);
  /*  else
        info->we_rdr->draw_3dbox (hdc, &rcClient, color,
                    LFRDR_3DBOX_FILLED | LFRDR_3DBOX_THICKFRAME |
                    LFRDR_BTN_STATUS_PRESSED);
*/
    if (DATECAP_H(hWnd) > 0) {
        CopyRect (&rc, &rcClient);
        rc.left += 2;
        rc.top += 2;
        rc.right -= 2;
        rc.bottom = rc.top + DATECAP_H (hWnd);

        info->we_rdr->draw_3dbox (hdc, &rc, color, 
           /*LFRDR_3DBOX_FILLED |*/ LFRDR_3DBOX_THICKFRAME | LFRDR_BTN_STATUS_NORMAL);
    }

    if (DATECAP_H(hWnd) > 0) {

        mcGetArrowRects (&rcMonth, &rcYear, &rcAML, &rcAMR, &rcAYL, &rcAYR);
        mcDrawPageArrow (hWnd, info, mc_data, hdc, &rcAML, FALSE, FALSE);
        mcDrawPageArrow (hWnd, info, mc_data, hdc, &rcAMR, TRUE, FALSE);
        mcDrawPageArrow (hWnd, info, mc_data, hdc, &rcAYL, FALSE, FALSE);
        mcDrawPageArrow (hWnd, info, mc_data, hdc, &rcAYR, TRUE, FALSE);

        {
            char chYear[5];
            const char* pchMon;
            int oldcolor;

            // NUV uFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
            if ((dwStyle & MCS_LANG) == MCS_CHN)
                pchMon = chMon_C[mc_data->cur_month-1];
            else if ((dwStyle & MCS_LANG) == MCS_ENG_L)
                pchMon = chMon_EL[mc_data->cur_month-1];
            else if ((dwStyle & MCS_LANG) == MCS_ENG_S)
                pchMon = chMon_ES[mc_data->cur_month-1];
            else
                pchMon = chMon_C[mc_data->cur_month-1];

            oldcolor = SetTextColor (hdc, pmcci->clr_titletext);

            FillBox (hdc, rcMonth.left, rcMonth.top, RECTW(rcMonth), RECTH(rcMonth));
            mcTextOutCenter (hWnd, hdc, &rcMonth, pchMon);
            
            FillBox (hdc, rcYear.left, rcYear.top, RECTW(rcYear), RECTH(rcYear));
            sprintf(chYear, "%d", mc_data->cur_year);
            mcTextOutCenter (hWnd, hdc, &rcYear, chYear);
            SetTextColor (hdc, oldcolor);
        }

    } // no year mon

    // draw weekday caption
    {
        int i, weekitem_w;
        const char *pchDay;
        RECT rcWeek, rcWeekItem;
        
        rcWeek.left = rcClient.left + WEEK_BORDER;
        rcWeek.right = rcClient.right-WEEK_BORDER;
        rcWeek.top = rcClient.top + 2 + WEEK_VBORDER1 + DATECAP_H(hWnd);
        rcWeek.bottom = rcWeek.top + WEEKFIELD_H(hWnd);
        weekitem_w = (rcWeek.right - rcWeek.left)/7;
        rcWeekItem.left = rcWeek.left;
        rcWeekItem.right = rcWeekItem.left + weekitem_w;
        rcWeekItem.top = rcWeek.top;
        rcWeekItem.bottom = rcWeek.bottom;

        SetBrushColor (hdc, pmcci->clr_weekcaptbk);
        FillBox (hdc, rcWeek.left, rcWeek.top, RECTW(rcWeek), RECTH(rcWeek));

        SetBkColor (hdc, pmcci->clr_weekcaptbk);
        SetTextColor (hdc, pmcci->clr_weekcapttext);
        for (i = 0; i <= 6; i++) {
            //RECT tmp;
            if ((dwStyle & MCS_LANG) == MCS_CHN) {
                pchDay = chWeek_C[i];
            }
            else if ((dwStyle & MCS_LANG) == MCS_ENG_L)
                pchDay = chWeek_E[i];
            else if ((dwStyle & MCS_LANG) == MCS_ENG_S)
                pchDay = chWeek_E_S[i];
            else {
                pchDay = chWeek_C[i];
            }

            //tmp = rcWeekItem;
            TextOut (hdc, rcWeekItem.left+11, rcWeekItem.top+1, pchDay);
            //mcTextOutCenter (hWnd, hdc, &rcWeekItem, pchDay);
            //mcTextOutCenter (hWnd, hdc, &tmp, pchDay);
            OffsetRect (&rcWeekItem, weekitem_w, 0);
        }
    }
    // draw month day text
    {
        mcDrawMonthDay (hWnd, hdc, &rcMDay, mc_data);
    }
}

// initialize mc_data  
static BOOL mcInitMonthCalendarData (HWND hWnd, MONCALDDATA* mc_data)
{
    time_t nowtime;
    struct tm *pnt;
    RECT rcClient, rcMDay;
    PMCCOLORINFO pmcci;
    
    __mg_time(&nowtime);
    pnt = (struct tm *)__mg_localtime(&nowtime);
    mc_data->sys_year = mc_data->cur_year = pnt->tm_year + 1900;
    mc_data->sys_month = mc_data->cur_month = pnt->tm_mon + 1;
    mc_data->sys_day = mc_data->cur_day = pnt->tm_mday;
    
    mcGetRects (hWnd, &rcClient, NULL, NULL, &rcMDay);
    mc_data->item_w = (rcMDay.right - rcMDay.left) / 7;
    mc_data->item_h = (rcMDay.bottom - rcMDay.top) / 6;
    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);
    mcGetCurDayPos (mc_data, mc_data->cur_day,
                    &mc_data->cur_line, &mc_data->cur_WeekDay);    
    mc_data->sys_WeekDay = mc_data->cur_WeekDay;    
    mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);

    mc_data->state = 0;

    // color info
    pmcci = (PMCCOLORINFO) mc_data->dwClrData;
    pmcci->clr_titlebk         = MCCLR_DF_TITLEBK;
    pmcci->clr_titletext     = MCCLR_DF_TITLETEXT;
    pmcci->clr_arrow         = MCCLR_DF_ARROW;
    pmcci->clr_arrowHibk     = MCCLR_DF_ARROWHIBK;
    pmcci->clr_daybk        = MCCLR_DF_DAYBK;
    pmcci->clr_dayHibk        = MCCLR_DF_DAYHIBK;
    pmcci->clr_daytext        = MCCLR_DF_DAYTEXT;
    pmcci->clr_trailingtext    = MCCLR_DF_TRAILINGTEXT;
    pmcci->clr_dayHitext    = MCCLR_DF_DAYHITEXT;
    pmcci->clr_weekcaptbk    = MCCLR_DF_WEEKCAPTBK;
    pmcci->clr_weekcapttext = MCCLR_DF_WEEKCAPTTEXT;

    mc_data->customed_day = 0;
    return TRUE;
}

static void mcSetCurDay (HWND hWnd, PMONCALDDATA mc_data, int newday)
{
    RECT rcClient, rcMDay;
    HDC hdc;
    
    if (newday < 1) 
        newday = 1;
    else if (newday > mc_data->monlen) 
        newday = mc_data->monlen;
    mcGetRects (hWnd, &rcClient, NULL, NULL, &rcMDay);
    hdc = GetClientDC (hWnd);
    mcDrawDay (hWnd, hdc, mc_data, &rcMDay, newday);
    ReleaseDC (hdc);
}

static void
mcShiftYM (HWND hWnd, PMONCALDDATA mc_data, BOOL bMonth, BOOL bRight)
{
    if (GetWindowStyle(hWnd) & MCS_NOTIFY)
        NotifyParent (hWnd, GetDlgCtrlID (hWnd), MCN_DATECHANGE);        

    if (bMonth && bRight) {
        if (mc_data->cur_month != 12) 
            mc_data->cur_month++;
        else {
            mc_data->cur_month = 1;
            mc_data->cur_year++;
        }
    }
    else if (bMonth) {
        if (mc_data->cur_month != 1) 
            mc_data->cur_month--;
        else {
            if (mc_data->cur_year <= 1970)
                return;
            mc_data->cur_month = 12;
            mc_data->cur_year--;
        }
    }
    else if (bRight) {
        mc_data->cur_year++;
    }
    else {
        if (mc_data->cur_year <= 1970)
            return;
        mc_data->cur_year--;
    }

    mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);
    while (mc_data->cur_day > mc_data->monlen) {
        mc_data->cur_day--;
    }
    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);
    mcGetCurDayPos (mc_data, mc_data->cur_day,
        &mc_data->cur_line, &mc_data->cur_WeekDay);    
    InvalidateRect (hWnd, NULL, TRUE);
}

#ifdef _KEY_OP_SUPPORT
static void mcKeyOperations (HWND hWnd, PMONCALDDATA mc_data, WPARAM wParam)
{
    int day = -1;

    switch (wParam) {

    case SCANCODE_CURSORBLOCKLEFT:
    case SCANCODE_CURSORBLOCKUP:
        if (CURFOCUS == MST_YEAR_FOCUS) {
            mcShiftYM (hWnd, mc_data, FALSE, FALSE);
            return;
        }
        else if (CURFOCUS == MST_MONTH_FOCUS) {
            mcShiftYM (hWnd, mc_data, TRUE, FALSE);
            return;
        }
        else
            day = mc_data->cur_day - (wParam == SCANCODE_CURSORBLOCKLEFT ? 1 : 7);
        break;
    case SCANCODE_CURSORBLOCKRIGHT:
    case SCANCODE_CURSORBLOCKDOWN:
        if (CURFOCUS == MST_YEAR_FOCUS) {
            mcShiftYM (hWnd, mc_data, FALSE, TRUE);
            return;
        }
        else if (CURFOCUS == MST_MONTH_FOCUS) {
            mcShiftYM (hWnd, mc_data, TRUE, TRUE);
            return;
        }
        else
            day = mc_data->cur_day + (wParam == SCANCODE_CURSORBLOCKRIGHT ? 1 : 7);
        break;

    default:
        return;
    }

    if (day > mc_data->monlen) {
        mc_data->cur_day = day - mc_data->monlen;
        mcShiftYM (hWnd, mc_data, TRUE, TRUE);
    }
    else if (day < 1) {
        mcShiftYM (hWnd, mc_data, TRUE, FALSE);
        mcSetCurDay (hWnd, mc_data, mc_data->monlen + day);
    }
    else {
        mcSetCurDay (hWnd, mc_data, day);
    }
}

#endif

/********************************************************************************/

static LRESULT MonthCalendarCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PMONCALDDATA    mc_data = NULL;
    DWORD dwStyle = GetWindowStyle (hWnd);
    
    if (message != MSG_CREATE)
        mc_data = (PMONCALDDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {
        case MSG_CREATE:
        {
            RECT rcWnd;
            int rc_w, rc_h;
            PMCCOLORINFO pmcci;
            
            GetWindowRect (hWnd, &rcWnd);
            rc_w = rcWnd.right - rcWnd.left;
            rc_h = rcWnd.bottom - rcWnd.top;
            if (rc_w < MINWNDRECT_W) {
                rc_w = MINWNDRECT_W;
                MoveWindow (hWnd, rcWnd.left, rcWnd.top, rc_w, 
                                rc_h, FALSE);
            }
            if (rc_h < MINWNDRECT_H) {
                rc_h = MINWNDRECT_H;
                MoveWindow (hWnd, rcWnd.left, rcWnd.top, rc_w, 
                                rc_h, FALSE);
            }
            
            if ((mc_data = (MONCALDDATA *) malloc (sizeof(MONCALDDATA))) == NULL)
                return -1;
            SetWindowAdditionalData2 (hWnd, (DWORD) mc_data);
            if ((pmcci = (MCCOLORINFO*) malloc (sizeof(MCCOLORINFO))) == NULL) {
                free (mc_data);
                return -1;
            }
            mc_data->dwClrData = (DWORD) pmcci;
            
            if (!mcInitMonthCalendarData (hWnd, mc_data)) {
                free (pmcci);
                free (mc_data);
                return -1;
            }
        }
            break;
            
        case MCM_GETCURDAY:
        {
            return mc_data->cur_day;
        }
        case MCM_GETCURWEEKDAY:
        {
            return mc_data->cur_WeekDay;
        }
            
        case MCM_GETCURMONTH:
            return mc_data->cur_month;
            
        case MCM_GETCURYEAR:
            return mc_data->cur_year;
            
        case MCM_GETCURMONLEN:
            return mc_data->monlen;
        
        case MCM_SETCURDAY:
        {
            mcSetCurDay (hWnd, mc_data, wParam);
            return 0;
        }
            
        case MCM_SETCURMONTH:
        {
            int newmonth = (int) wParam;
            RECT rcClient, rcMonth, rcMDay;
            
            if (newmonth > 12)
                newmonth = 12;
            else if (newmonth < 1) 
                newmonth = 1;

            if (DATECAP_H(hWnd) == 0)
                break;

            mcGetRects (hWnd, &rcClient, &rcMonth, NULL, &rcMDay);
            hdc = GetClientDC (hWnd);
            mcDrawMonth (hWnd, hdc, mc_data, &rcMDay, &rcMonth, newmonth);
            ReleaseDC (hdc);
            return 0;
        }
            
        case MCM_SETCURYEAR:
        {
            int newyear = (int) wParam;
            RECT rcClient, rcYear, rcMDay;

            if (newyear < 1970)    newyear = 1970;

            if (DATECAP_H(hWnd) == 0)
                break;

            mcGetRects (hWnd, &rcClient, NULL, &rcYear, &rcMDay);
            hdc = GetClientDC (hWnd);
            mcDrawYear (hWnd, hdc, mc_data, &rcMDay, &rcYear, newyear);
            ReleaseDC (hdc);
            return 0;
        }
        
        case MCM_SETTODAY:
        {
            RECT rcClient, rcYear, rcMonth, rcMDay;
            
            mcGetRects (hWnd, &rcClient, &rcMonth, &rcYear, &rcMDay);
            mc_data->cur_day = mc_data->sys_day;
            mc_data->cur_month = mc_data->sys_month;
            mc_data->cur_year = mc_data->sys_year;
            mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, mc_data->cur_month, 1);
            mcGetCurDayPos (mc_data, mc_data->cur_day,
                        &mc_data->cur_line, &mc_data->cur_WeekDay);    
            mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);

            InvalidateRect (hWnd, &rcYear, FALSE);
            InvalidateRect (hWnd, &rcMonth, FALSE);
            InvalidateRect (hWnd, &rcMDay, FALSE);
            return 0;
        }

        case MCM_SETCURDATE:
        {
            RECT rcClient, rcYear, rcMonth, rcMDay;
            PSYSTEMTIME pcurdate = NULL;

            mcGetRects (hWnd, &rcClient, &rcMonth, &rcYear, &rcMDay);
            pcurdate = (PSYSTEMTIME) lParam;
            mc_data->cur_year = MAX(pcurdate->year, 1970);
                      
            if (pcurdate->month > 12) {
                mc_data->cur_month = 12;
            } else {
                mc_data->cur_month = (pcurdate->month < 1) ? 1 : pcurdate->month;
            }
            
            mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);
            if (pcurdate->day < 1) {
                mc_data->cur_day = 1;
            } else {
                mc_data->cur_day = (pcurdate->day > mc_data->monlen) ? mc_data->monlen : pcurdate->day; 
            }
            
            mc_data->WeekDay1 = GetWeekDay (mc_data->cur_year, mc_data->cur_month, 1);
            mcGetCurDayPos (mc_data, mc_data->cur_day, 
                            &mc_data->cur_line, &mc_data->cur_WeekDay);
            //mc_data->monlen = GetMonLen (mc_data->cur_year, mc_data->cur_month);
            
            InvalidateRect (hWnd, &rcYear, FALSE);
            InvalidateRect (hWnd, &rcMonth, FALSE);
            InvalidateRect (hWnd, &rcMDay, FALSE);
            return 0;
        }

        case MCM_SETDAYCOLOR:
        {
            HDC hdc;
            RECT rcMDay, rcDay;

            if (wParam <= 0 || wParam > 31)
                return 0;

            mc_data->customed_day |= (1<<wParam);
            mc_data->customed_day_color[wParam] = lParam;

            mcGetRects (hWnd, NULL, NULL, NULL, &rcMDay);
            hdc = GetClientDC (hWnd);
            mcGetItemRectFromDay (mc_data, &rcMDay, &rcDay, wParam);
            mcDrawCustomedDay (hWnd, hdc, mc_data, &rcDay, wParam);
            ReleaseDC (hdc);

            return 0;
        }

        case MCM_CLEARDAYCOLOR:
            mc_data->customed_day = 0;
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;

        case MCM_GETFIRSTWEEKDAY:
            return mc_data->WeekDay1;
            
        case MCM_GETCURDATE:
        {
            PSYSTEMTIME pcurdate = NULL;

            pcurdate = (PSYSTEMTIME) lParam;
            pcurdate->year = mc_data->cur_year;
            pcurdate->month = mc_data->cur_month;
            pcurdate->day = mc_data->cur_day;
            pcurdate->weekday = mc_data->cur_WeekDay;
            return 0;
        }
            
        case MCM_GETTODAY:
        {
            PSYSTEMTIME ptoday = NULL;

            ptoday = (PSYSTEMTIME) lParam;
            ptoday->year = mc_data->sys_year;
            ptoday->month = mc_data->sys_month;
            ptoday->day = mc_data->sys_day;
            ptoday->weekday = mc_data->sys_WeekDay;
            return 0;
        }
            
        case MCM_GETMINREQRECTW:
            return MINWNDRECT_W;    
            
        case MCM_GETMINREQRECTH:
            return MINWNDRECT_H;
            
        case MCM_GETCOLOR:
        {
            PMCCOLORINFO pmcci, pmccinfo;

            pmccinfo = (PMCCOLORINFO) lParam;
            pmcci = (PMCCOLORINFO) mc_data->dwClrData;
            
            pmccinfo->clr_titlebk         = pmcci->clr_titlebk;
            pmccinfo->clr_titletext     = pmcci->clr_titletext;
            pmccinfo->clr_arrow         = pmcci->clr_arrow;
            pmccinfo->clr_arrowHibk     = pmcci->clr_arrowHibk;
            pmccinfo->clr_daybk            = pmcci->clr_daybk;
            pmccinfo->clr_dayHibk        = pmcci->clr_dayHibk;
            pmccinfo->clr_daytext        = pmcci->clr_daytext;
            pmccinfo->clr_trailingtext    = pmcci->clr_trailingtext;
            pmccinfo->clr_dayHitext        = pmcci->clr_dayHitext;
            pmccinfo->clr_weekcaptbk    = pmcci->clr_weekcaptbk;
            pmccinfo->clr_weekcapttext     = pmcci->clr_weekcapttext;
            return 0;
        }
            
        case MCM_SETCOLOR:
        {
            PMCCOLORINFO pmcci, pmcci_new;
            
            pmcci = (PMCCOLORINFO) mc_data->dwClrData;
            pmcci_new = (PMCCOLORINFO) lParam;
            
            if (pmcci_new->clr_titlebk >= 0) {
                pmcci->clr_titlebk = pmcci_new->clr_titlebk;
            }
            if (pmcci_new->clr_titletext >= 0) {
                pmcci->clr_titletext = pmcci_new->clr_titletext;
            }
            if (pmcci->clr_arrow >= 0) {
                pmcci->clr_arrow = pmcci_new->clr_arrow;
            }
            if (pmcci->clr_arrowHibk >= 0) {
                pmcci->clr_arrowHibk = pmcci_new->clr_arrowHibk;
            }
            if (pmcci->clr_daybk >= 0)
                pmcci->clr_daybk        = pmcci_new->clr_daybk;
            if (pmcci->clr_dayHibk >= 0)
                pmcci->clr_dayHibk        = pmcci_new->clr_dayHibk;
            if (pmcci->clr_daytext >= 0)
                pmcci->clr_daytext        = pmcci_new->clr_daytext;
            if (pmcci->clr_trailingtext >= 0)
                pmcci->clr_trailingtext    = pmcci_new->clr_trailingtext;
            if (pmcci->clr_dayHitext >= 0)
                pmcci->clr_dayHitext    = pmcci_new->clr_dayHitext;
            if (pmcci->clr_weekcaptbk >= 0)
                pmcci->clr_weekcaptbk    = pmcci_new->clr_weekcaptbk;
            if (pmcci->clr_weekcapttext >= 0) 
                pmcci->clr_weekcapttext = pmcci_new->clr_weekcapttext;
            
            InvalidateRect (hWnd, NULL, FALSE);    
            return 0;
        }
            
        case MSG_DESTROY:
            mcMonthCalendarCleanUp (mc_data);
            free (mc_data);    
            break;
            
        case MSG_LBUTTONDOWN:
        {
            int x, y;
            RECT rcClient, rcMonth, rcYear, rcMDay;
            RECT rcAML, rcAMR, rcAYL, rcAYR;
            
            x = LOSWORD (lParam);
            y = HISWORD (lParam);
            mcGetRects (hWnd, &rcClient, &rcMonth, &rcYear, &rcMDay);
            mcGetArrowRects (&rcMonth, &rcYear, &rcAML, &rcAMR, &rcAYL, &rcAYR);

            InflateRect (&rcAML, 6, 2);
            InflateRect (&rcAMR, 6, 2);
            InflateRect (&rcAYR, 6, 2);
            InflateRect (&rcAYL, 6, 2);

            if (PtInRect (&rcAMR, x, y)) {
#ifdef _KEY_OP_SUPPORT
                SETFOCUS(MST_MONTH_FOCUS);
#endif
                mcShiftYM (hWnd, mc_data, TRUE, TRUE);
            }
            else if (PtInRect (&rcAML, x, y)) {
#ifdef _KEY_OP_SUPPORT
                SETFOCUS(MST_MONTH_FOCUS);
#endif
                mcShiftYM (hWnd, mc_data, TRUE, FALSE);
            }
            else if (PtInRect (&rcAYR, x, y)) {
#ifdef _KEY_OP_SUPPORT
                SETFOCUS(MST_YEAR_FOCUS);
#endif
                mcShiftYM (hWnd, mc_data, FALSE, TRUE);
            }
            else if (PtInRect (&rcAYL, x, y)) {
#ifdef _KEY_OP_SUPPORT
                SETFOCUS(MST_YEAR_FOCUS);
#endif
                mcShiftYM (hWnd, mc_data, FALSE, FALSE);
            }
                
            else if (PtInRect (&rcMDay, x, y)) {
                int line, weekday, pline, pweekday, posindex;
                RECT rcItemDay; //rcPItemDay;
                
#ifdef _KEY_OP_SUPPORT
                SETFOCUS(MST_DAY_FOCUS);
#endif
                
                pline = mc_data->cur_line;
                pweekday = mc_data->cur_WeekDay;
                mcGetItemRectFromPos (mc_data, &rcMDay, 
                                        &rcItemDay, &line, &weekday, x, y);
                posindex = line * 7 + weekday + 1;
                if (posindex < mc_data->WeekDay1+1) {
                    if (dwStyle & MCS_NOTIFY)
                        NotifyParent (hWnd, GetDlgCtrlID (hWnd), MCN_DATECHANGE);    
                    if (mc_data->cur_month != 1) 
                        mc_data->cur_month--;
                    else {
                        if (mc_data->cur_year <= 1970)
                            break;
                        mc_data->cur_month = 12;
                        mc_data->cur_year--;
                        InvalidateRect (hWnd, &rcYear, FALSE);
                    }
                    mc_data->monlen = GetMonLen (mc_data->cur_year, 
                                                 mc_data->cur_month);
                    while (mc_data->cur_day > mc_data->monlen) {
                        mc_data->cur_day--;
                    }
                    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, 
                                                   mc_data->cur_month, 1);
                    mcGetCurDayPos (mc_data, mc_data->cur_day,
                        &mc_data->cur_line, &mc_data->cur_WeekDay);    
                    InvalidateRect (hWnd, &rcMonth, FALSE);
                    InvalidateRect (hWnd, &rcMDay, FALSE);
                }
                else if (posindex >mc_data->monlen + mc_data->WeekDay1) {
                    if (dwStyle & MCS_NOTIFY)
                        NotifyParent (hWnd, GetDlgCtrlID (hWnd), MCN_DATECHANGE);    
                    if (mc_data->cur_month != 12) 
                        mc_data->cur_month++;
                    else {
                        mc_data->cur_month = 1; 
                        mc_data->cur_year++;
                        InvalidateRect (hWnd, &rcYear, FALSE);
                    }
                    mc_data->monlen = GetMonLen (mc_data->cur_year, 
                                                 mc_data->cur_month);
                    while (mc_data->cur_day > mc_data->monlen) {
                        mc_data->cur_day--;
                    }
                    mc_data->WeekDay1 = GetWeekDay(mc_data->cur_year, 
                                                   mc_data->cur_month, 1);
                    mcGetCurDayPos (mc_data, mc_data->cur_day,
                            &mc_data->cur_line, &mc_data->cur_WeekDay);    
                    InvalidateRect (hWnd, &rcMonth, FALSE);
                    InvalidateRect (hWnd, &rcMDay, FALSE);
                }
                else {
                  if (line != pline || weekday != pweekday) {
                    int newday;

                    if (dwStyle & MCS_NOTIFY)
                        NotifyParent (hWnd, GetDlgCtrlID (hWnd), MCN_DATECHANGE);
                    hdc = GetClientDC (hWnd);
                    newday = (line*7+weekday+1) - (mc_data->WeekDay1+1) + 1;
                    mcDrawDay (hWnd, hdc, mc_data, &rcMDay, newday);
                    ReleaseDC (hdc);
                  }
                  else {
                    if (dwStyle & MCS_NOTIFY)
                        NotifyParent (hWnd, GetDlgCtrlID (hWnd), MCN_DATECLK);        
                  }
                }
            }
        }    
            break;
            
        case MSG_LBUTTONUP:
            break;

#ifdef _KEY_OP_SUPPORT
        case MSG_KEYDOWN:
            if (wParam == SCANCODE_TAB) {
                int focus = (CURFOCUS+1) % FOCUS_NR;
                SETFOCUS(focus);
            }
            else
                mcKeyOperations (hWnd, mc_data, wParam);
            break;

        case MSG_GETDLGCODE:
            return DLGC_WANTTAB | DLGC_WANTARROWS;
#endif

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            mcDrawCalendar (hWnd, hdc, mc_data);
            EndPaint (hWnd, hdc);
            return 0;
            
        default:
            break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}
#endif /* _MGCTRL_MONTHCAL */

