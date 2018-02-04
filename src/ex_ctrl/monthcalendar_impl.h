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
** monthcalendar.h: the header for MonthCalendar control.
**
** Create date: 2001/01/03
*/

#ifndef __MONTHCALENDAR_IMPL_H_
#define __MONTHCALENDAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define _KEY_OP_SUPPORT      1


#ifdef _KEY_OP_SUPPORT

#define MST_FOCUS_MASK       0x00000003
#define FOCUS_NR             3

#define MST_DAY_FOCUS        0
#define MST_MONTH_FOCUS      1
#define MST_YEAR_FOCUS       2

#define CURFOCUS             (mc_data->state & MST_FOCUS_MASK)
#define SETFOCUS(i)          (mc_data->state = (mc_data->state & \
                                  (~MST_FOCUS_MASK)) | (i & MST_FOCUS_MASK) )
#endif


#define MCS_LANG 0x0003L

static inline int DATECAP_H(HWND hwnd)
{
    if (GetWindowStyle(hwnd)&MCS_NOYEARMON)
        return 0;
    return (GetWindowFont(hwnd)->size+8);
}

// structs 
typedef struct tagMonthCalendarData
{
    int     sys_month;
    int     sys_year;
    int     sys_day;
    int     sys_WeekDay;

    int     cur_month;
    int     cur_year;
    int     cur_day;

    int     cur_line;
    int     cur_WeekDay;
    int     item_w;
    int     item_h;
    int     WeekDay1;
    int     monlen;

    DWORD   dwClrData;

    UINT    state;

    UINT    customed_day;
    gal_pixel customed_day_color[32];

} MONCALDDATA;
typedef MONCALDDATA* PMONCALDDATA;

BOOL RegisterMonthCalendarControl (void);

time_t __mg_time (time_t * timer);
time_t __mg_mktime (struct tm * timeptr);
struct tm *__mg_localtime (const time_t * timer);

#ifdef  __cplusplus
}
#endif

#endif    // __MONTHCALENDAR_IMPL_H__
