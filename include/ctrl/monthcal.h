/**
 * \file monthcal.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 \verbatim

    Copyright (C) 2002-2008 Feynman Software
    Copyright (C) 1998-2002 Wei Yongming

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: monthcal.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef EXT_MONTHCAL_H
#define EXT_MONTHCAL_H


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_monthcal MonthCalendar control
     * @{
     */

/**
 * \def CTRL_MONTHCALENDAR
 * \brief The class name of monthcalendar control.
 */
#define CTRL_MONTHCALENDAR     ("MonthCalendar")

/** Structure of the system time */
typedef struct _SYSTEMTIME
{
    /** Year number of the date */
    int year;
    /** Month number of the date */
    int month;
    /** Day number of the date */
    int day;
    /** Weekday number of the date */
    int weekday;
} SYSTEMTIME;
/**
 * \var typedef SYSTEMTIME *PSYSTEMTIME;
 * \brief Data type of the pointer to a SYSTEMTIME.  
 */
typedef SYSTEMTIME *PSYSTEMTIME;

/** Struture of the color info of the monthcalendar control */
typedef struct _MCCOLORINFO
{
    /** Color of the titile background */
    int clr_titlebk;
    /** Color of the titile text */
    int clr_titletext;
    /** Color of the arrow */
    int clr_arrow;
    /** Color of the hilighted arrow background*/
    int clr_arrowHibk;

    /** Color of the week caption background */
    int clr_weekcaptbk;
    /** Color of the week caption text */
    int clr_weekcapttext;

    /** Color of the day text background */
    int clr_daybk;
    /** Color of the hilighted day text background */
    int clr_dayHibk;
    /** Color of the day text */
    int clr_daytext;
    /** Color of the trailing day text */
    int clr_trailingtext;
    /** Color of the hilighted day text */
    int clr_dayHitext;
} MCCOLORINFO;

/**
 * \var typedef MCCOLORINFO *PMCCOLORINFO;
 * \brief Data type of the pointer to a MCCOLORINFO.  
 */
typedef MCCOLORINFO *PMCCOLORINFO;

    /**
     * \defgroup mgext_ctrl_monthcal_styles Styles of monthcalendar control
     * @{
     */
/**
 * \def MCS_CHN
 * \brief Displays chinese label.
 */
#define MCS_CHN                 0x0001L

/**
 * \def MCS_ENG_L
 * \brief Displays long english label.
 */
#define MCS_ENG_L               0x0002L

/**
 * \def MCS_ENG_S
 * \brief Displays short english label.
 */
#define MCS_ENG_S               0x0003L

/**
 * \def MCS_NOTIFY
 * \brief Notifies the parent window when something happens.
 */
#define MCS_NOTIFY              0x0004L

/**
 * \def MCS_NOYEARMON
 * \brief Do not show year and month.
 */
#define MCS_NOYEARMON           0x0008L

    /** @} end of mgext_ctrl_monthcal_styles */

    /**
     * \defgroup mgext_ctrl_monthcal_msgs Messages of monthcalendar control
     * @{
     */

/**
 * \def MCM_GETCURDAY
 * \brief Gets the day number of the current selected date.
 *
 * \code
 * MCM_GETCURDAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The day number of the current selected date.
 */
#define MCM_GETCURDAY           0xF300

/**
 * \def MCM_GETCURMONTH
 * \brief Gets the month number of the current selected date.
 *
 * \code
 * MCM_GETCURMONTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The month number of the current selected date.
 */
#define MCM_GETCURMONTH         0xF301

/**
 * \def MCM_GETCURYEAR
 * \brief Gets the year number of the current selected date.
 *
 * \code
 * MCM_GETCURYEAR
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The year number of the current selected date.
 */
#define MCM_GETCURYEAR          0xF302

/**
 * \def MCM_GETCURWEEKDAY
 * \brief Gets the weekday number of the current selected date.
 *
 * \code
 * MCM_GETCURWEEKDAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The weekday number of the current selected date.
 */
#define MCM_GETCURWEEKDAY       0xF303

/**
 * \def MCM_GETCURMONLEN
 * \brief Gets the month length of the current selected date.
 *
 * \code
 * MCM_GETCURMONLEN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The month length of the current selected date.
 */
#define MCM_GETCURMONLEN        0xF304

/**
 * \def MCM_SETCURDAY
 * \brief Changes the day of the currently selected date.
 *
 * \code
 * MCM_SETCURDAY
 * int newday;
 *
 * wParam = (WPARAM)newday;
 * lParam = 0;
 * \endcode
 *
 * \param newday The new day number of the currently selected date.
 */
#define MCM_SETCURDAY           0xF305

/**
 * \def MCM_SETCURMONTH
 * \brief Changes the month of the currently selected date.
 *
 * \code
 * MCM_SETCURMONTH
 * int newmonth;
 *
 * wParam = (WPARAM)newmonth;
 * lParam = 0;
 * \endcode
 *
 * \param newmonth The new month number of the currently selected date.
 */
#define MCM_SETCURMONTH         0xF306

/**
 * \def MCM_SETCURYEAR
 * \brief Changes the year of the currently selected date.
 *
 * \code
 * MCM_SETCURYEAR
 * int newyear;
 *
 * wParam = (WPARAM)newyear;
 * lParam = 0;
 * \endcode
 *
 * \param newyear The new year number of the currently selected date.
 */
#define MCM_SETCURYEAR          0xF307

/**
 * \def MCM_SETTODAY
 * \brief Sets the currently selected date as the date of "today".
 *
 * \code
 * MCM_SETTODAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define MCM_SETTODAY            0xF308

/**
 * \def MCM_GETFIRSTWEEKDAY
 * \brief Gets the weekday of the first day of this month.
 *
 * \code
 * MCM_GETFIRSTWEEKDAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The weekday of the first day.
 */
#define MCM_GETFIRSTWEEKDAY     0xF309

/**
 * \def MCM_GETCURDATE
 * \brief Gets the currently selected date.
 *
 * \code
 * MCM_GETCURDATE
 * PSYSTEMTIME pcurdate;
 *
 * wParam = 0;
 * lParam = (LPARAM)pcurdate;
 * \endcode
 *
 * \param pcurdate Pointer to the SYSTEMTIME structure to retreive the date.
 */
#define MCM_GETCURDATE          0xF310

/**
 * \def MCM_GETTODAY
 * \brief Gets the date of "today".
 *
 * \code
 * MCM_GETTODAY
 * PSYSTEMTIME pcurdate;
 *
 * wParam = 0;
 * lParam = (LPARAM)pcurdate;
 * \endcode
 *
 * \param pcurdate Pointer to the SYSTEMTIME structure to retreive the 
 *             date of "today".
 */
#define MCM_GETTODAY            0xF311

/**
 * \def MCM_GETMINREQRECTW
 * \brief Gets the minimum width required to display a full month in a month 
 *         calendar control.
 *
 * \code
 * MCM_GETMINREQRECTW
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum width required to dispaly a full month.
 */
#define MCM_GETMINREQRECTW      0xF312

/**
 * \def MCM_GETMINREQRECTH
 * \brief Gets the minimum height required to display a full month in a month 
 *         calendar control.
 *
 * \code
 * MCM_GETMINREQRECTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum height required to dispaly a full month.
 */
#define MCM_GETMINREQRECTH      0xF313

/**
 * \def MCM_SETCURDATE
 * \brief Sets the currently selected date.
 *
 * \code
 * MCM_SETCURDATE
 * PSYSTEMTIME pcurdate;
 *
 * wParam = 0;
 * lParam = (LPARAM)pcurdate;
 * \endcode
 *
 * \param pcurdate Pointer to the SYSTEMTIME structure storing the values of 
 *             the current selected date.
 */
#define MCM_SETCURDATE          0xF314

/**
 * \def MCM_SETCOLOR
 * \brief Sets the color of the monthcalendar control.
 *
 * \code
 * MCM_SETCOLOR
 * PMCCOLORINFO newcolor;
 *
 * wParam = 0;
 * lParam = (LPARAM)newcolor;
 * \endcode
 *
 * \param newcolor Pointer to the MCCOLORINFO structure storing the vaules of 
 *             the new color info.
 */
#define MCM_SETCOLOR            0xF315

/**
 * \def MCM_GETCOLOR
 * \brief Gets the color of the monthcalendar control.
 *
 * \code
 * MCM_GETCOLOR
 * PMCCOLORINFO color;
 *
 * wParam = 0;
 * lParam = (LPARAM)color;
 * \endcode
 *
 * \param color Pointer to the MCCOLORINFO structure retreiving the color info.
 */
#define MCM_GETCOLOR            0xF316

/**
 * \def MCM_SETDAYCOLOR
 * \brief Sets the color of a specified date .
 *
 * \code
 * MCM_SETDAYCOLOR
 * int day;
 * int color;
 *
 * wParam = day;
 * lParam = color;
 * \endcode
 *
 * \param day Month day to set color
 * \param color Pixel color.
 */
#define MCM_SETDAYCOLOR          0xF317

/**
 * \def MCM_CLEARDAYCOLOR
 * \brief Clears the color of a specified month view .
 *
 * \code
 * MCM_CLEARDAYCOLOR
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define MCM_CLEARDAYCOLOR        0xF318


    /** @} end of mgext_ctrl_monthcal_msgs */

    /**
     * \defgroup mgext_ctrl_monthcal_ncs Notification codes of monthcalendar control
     * @{
     */

/**
 * \def MCN_DATECHANGE
 * \brief Notifies that the currently selected date is changed.
 */
#define MCN_DATECHANGE          1
/*#define MCN_DAYCHANGE           2 */
#define MCN_DATECLK             3

    /** @} end of mgext_ctrl_monthcal_ncs */

    /** @} end of mgext_ctrl_monthcal */

    /** @} end of controls */
#ifdef  __cplusplus
}
#endif

#endif /* EXT_MONTHCAL_H */

