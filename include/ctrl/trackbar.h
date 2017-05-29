/**
 * \file trackbar.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim

    Copyright (C) 2002-2008 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: trackbar.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_TRACKBAR
#define _MGUI_CTRL_TRACKBAR
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_trackbar TrackBar control
     * @{
     */

/**
 * \def CTRL_TRACKBAR
 * \brief The class name of trackbar control.
 */
#define CTRL_TRACKBAR              ("trackbar")

/**
 * \def TBLEN_TIP
 * \brief The maximum length of the trackbar tip string.
 */
#define TBLEN_TIP                   31

    /**
     * \defgroup ctrl_trackbar_styles Styles of trackbar control
     * @{
     */

/**
 * \def TBS_NOTIFY
 * \brief Causes the trackbar to notify the parent window with a notification message 
 *         when the user clicks or doubleclicks the trackbar.
 */
#define TBS_NOTIFY                 0x0001L

/**
 * \def TBS_VERTICAL
 * \brief The trackbar control will be oriented vertically.
 */
#define TBS_VERTICAL               0x0002L

/**
 * \def TBS_BORDER
 * \brief The trackbar control will have border.
 */
#define TBS_BORDER                 0x0004L

/**
 * \def TBS_TIP
 * \brief The trackbar control will display tip string above the control.
 */
#define TBS_TIP                    0x0008L

/**
 * \def TBS_NOTICK
 * \brief The trackbar control will not display tick line beside the control.
 */
#define TBS_NOTICK                 0x0010L

/* internally used style */
#define TBS_FOCUS                  0x1000L

    /** @} end of ctrl_trackbar_styles */

    /**
     * \defgroup ctrl_trackbar_msgs Messages of trackbar control
     * @{
     */

/**
 * \def TBM_SETRANGE
 * \brief Sets the range of minimum and maximum logical positions for the 
 *         slider in a trackbar.
 *
 * \code
 * TBM_SETRANGE
 * int min;
 * int max;
 *
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * \endcode
 *
 * \param min Minimum position for the slider.
 * \param max Maximum position for the slider.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETRANGE               0xF090

/**
 * \def TBM_GETMIN
 * \brief Gets the minimum logical position for the slider.
 *
 * \code
 * TBM_GETMIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The minimum logical position for the slider.
 */
#define TBM_GETMIN                 0xF091

/**
 * \def TBM_SETPOS
 * \brief Sets the current logical position of the slider.
 *
 * \code
 * TBM_SETPOS
 * int pos;
 *
 * wParam = (WPARAM)pos;
 * lParam = 0;
 * \endcode
 *
 * \param pos New logical position of the slider.
 *
 * \return Always be zero.
 */
#define TBM_SETPOS                 0xF092

/**
 * \def TBM_GETPOS
 * \brief Gets the current logical position of the slider.
 *
 * \code
 * TBM_GETPOS
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current logical position of the slider.
 */
#define TBM_GETPOS                 0xF093

/**
 * \def TBM_SETLINESIZE
 * \brief Sets the number of logical positions moved in response to keyboard
 *     input from the arrow keys.
 *
 * Sets the number of logical positions the trackbar's slider moves in response
 * to keyboard input from the arrow keys. The logical positions are the integer
 * increments in the trackbar's range of minimum to maximum slider positions.
 *
 * \code
 * TBM_SETLINESIZE
 * int linesize;
 *
 * wParam = (WPARAM)linesize;
 * lParam = 0;
 * \endcode
 *
 * \param linesize New line size.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETLINESIZE            0xF094

/**
 * \def TBM_GETLINESIZE
 * \brief Gets the number of logical positions moved in response to keyboard
 *     input from the arrow keys.
 *
 * Gets the number of logical positions the trackbar's slider moves in response
 * to keyboard input from the arrow keys. The logical positions are the integer
 * increments in the trackbar's range of minimum to maximum slider positions.
 *
 * \code
 * TBM_GETLINESIZE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current line size.
 */
#define TBM_GETLINESIZE            0xF095

/**
 * \def TBM_SETPAGESIZE
 * \brief Sets the number of logical positions moved in response to keyboard
 *         input from page keys..
 *
 * Sets the number of logical positions the trackbar's slider moves in response
 * to keyboard input form page keys, such as PAGE DOWN or PAGE UP keys. The 
 * logical positions are the integer increments in the trackbar's range of 
 * minimum to maximum slider positions.
 *
 * \code
 * TBM_SETPAGESIZE
 * int pagesize;
 *
 * wParam = (WPARAM)pagesize;
 * lParam = 0;
 * \endcode
 *
 * \param pagesize New page size.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETPAGESIZE            0xF096

/**
 * \def TBM_GETPAGESIZE
 * \brief Gets the number of logical positions moved in response to keyboard
 *        input from page keys..
 *
 * Gets the number of logical positions the trackbar's slider moves in response
 * to keyboard input form page keys, such as PAGE DOWN or PAGE UP keys. The 
 * logical positions are the integer increments in the trackbar's range of 
 * minimum to maximum slider positions.
 *
 * \code
 * TBM_GETPAGESIZE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current page size.
 */
#define TBM_GETPAGESIZE            0xF097

/**
 * \def TBM_SETTIP
 * \brief Sets the start and end tip strings.
 *
 * \code
 * TBM_SETTIP
 * char* starttip;
 * char* endtip;
 *
 * wParam = (WPARAM)starttip;
 * lParam = (LPARAM)endtip;
 * \endcode
 *
 * \param starttip New start tip string.
 * \param endtip New end tip tip string.
 *
 * \return Always be zero.
 */
#define TBM_SETTIP            0xF098

/**
 * \def TBM_GETTIP
 * \brief Gets the start and end tip strings.
 *
 * \code
 * TBM_GETTIP
 * char starttip [TBLEN_TIP + 1];
 * char endtip [TBLEN_TIP + 1];
 *
 * wParam = (WPARAM)starttip;
 * lParam = (LPARAM)endtip;
 * \endcode
 *
 * \param starttip Buffer receives the start tip string. It should
 *        be length enough to save (TBLEN_TIP + 1) characters.
 * \param endtip Buffer receives the end tip string. It should
 *        be length enough to save (TBLEN_TIP + 1) characters.
 *
 * \return Always be zero.
 */
#define TBM_GETTIP                 0xF09A

/**
 * \def TBM_SETTICKFREQ
 * \brief Sets the interval frequency for tick marks in a trackbar.
 *
 * \code
 * TBM_SETTICKFREQ
 * int tickfreq;
 *
 * wParam = (WPARAM)tickfreq;
 * lParam = 0;
 * \endcode
 *
 * \param tickfreq New interval frequency for tick marks in a trackbar.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETTICKFREQ            0xF09B

/**
 * \def TBM_GETTICKFREQ
 * \brief Gets the interval frequency for tick marks in a trackbar.
 *
 * \code
 * TBM_GETTICKFREQ
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current interval frequency for tick marks in a trackbar.
 */
#define TBM_GETTICKFREQ            0xF09C

/**
 * \def TBM_SETMIN
 * \brief Sets the minimum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_SETMIN
 * int min;
 *
 * wParam = (WPARAM)min;
 * lParam = 0;
 * \endcode
 *
 * \param min The new minimum logical position for the slider in a trackbar.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETMIN                 0xF09D

/**
 * \def TBM_SETMAX
 * \brief Sets the maximum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_SETMAX
 * int max;
 *
 * wParam = (WPARAM)max;
 * lParam = 0;
 * \endcode
 *
 * \param min The new maximum logical position for the slider in a trackbar.
 *
 * \return Zero on success; otherwise -1.
 */
#define TBM_SETMAX                 0xF09E

/**
 * \def TBM_GETMAX
 * \brief Gets the maximum logical position for the slider in a trackbar.
 *
 * \code
 * TBM_GETMAX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current maximum logical position for the slider in a trackbar.
 */
#define TBM_GETMAX                 0xF09F

#define TBR_MSGMAX                 0xF010

    /** @} end of ctrl_trackbar_msgs */

    /**
     * \defgroup ctrl_trackbar_ncs Notification codes of trackbar control
     * @{
     */

/**
 * \def TBN_REACHMIN
 * \brief Notifies that the slider has reached the minimum position.
 */
#define TBN_REACHMIN                1

/**
 * \def TBN_REACHMAX
 * \brief Notifies that the slider has reached the maximum position.
 */
#define TBN_REACHMAX                2

/**
 * \def TBN_CHANGE
 * \brief Notifies that the position of the slider has changed.
 */
#define TBN_CHANGE                  3

/**
 * \def TBN_STARTDRAG
 * \brief Notifies that the user start to drag the slider.
 */
#define TBN_STARTDRAG               4

/**
 * \def TBN_STOPDRAG
 * \brief Notifies that the user stop to drag the slider.
 */
#define TBN_STOPDRAG                5

    /** @} end of ctrl_trackbar_ncs */

    /**
     * \defgroup ctrl_trackbar_render_state state for renderer
     * @{
     */
/**
 * \def LFRDR_TBS_PRESSED
 * \brief indicate left button down
 */ 
#define LFRDR_TBS_PRESSED        0x0100L

/**
 * \def LFRDR_TBS_HILITE
 * \brief indicate mouse is in region of thumb of trackbar
 */
#define LFRDR_TBS_HILITE         0x0200L

    /** @} end of ctrl_trackbar_render_state */

    /** @} end of ctrl_trackbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_TRACKBAR */

