/**
 * \file scrollbar.h
 * \author wangjian <wangjian@minigui.org>
 * \date 2008/01/17
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
 * $Id: scrollbar.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_SCROLLBAR_H
#define _MGUI_CTRL_SCROLLBAR_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_scrollbar ScrollBar control
     *
     * \note The scrollbar is not implemented as a control in current version.
     * @{
     */

/**
 * \def CTRL_SCROLLBAR
 * \brief The class name of scrollbar control.
 */
#define CTRL_SCROLLBAR              ("scrollbar")

    /**
     * \defgroup ctrl_scrollbar_styles Styles of scrollbar control
     * @{
     */

/**
 * \def SBS_HORZ
 * \brief Create a horizontal scrollbar.
 */
#define SBS_HORZ  0x1000

/**
 * \def SBS_VERT
 * \brief Create a vertical scrollbar.
 */
#define SBS_VERT   0x2000

/**
 * \def SBS_BOTTOMALIGN
 * \brief Create a horizontal scrollbar on the bottom of the rect.
 *
 * \note This style must be used together with SBS_HORZ, and the height of the 
 * scrollbar will depend on the window elements or renderer.
 */
#define SBS_BOTTOMALIGN 0x0001

/**
 * \def SBS_TOPALIGN
 * \brief Create a horizontal scrollbar on the top of the rect.
 *
 * \note This style must be used together with SBS_HORZ, and the height of the 
 * scrollbar will depend on the window elements or renderer.
 */
#define SBS_TOPALIGN  0x0002

/**
 * \def SBS_LEFTALIGN
 * \brief Create a vertical scrollbar on the left of the rect.
 *
 * \note This style must be used together with SBS_VERT, and the width of the 
 * scrollbar will depend on the window elements or renderer.
 */
#define SBS_LEFTALIGN  0x0001

/**
 * \def SBS_RIGHTALIGN
 * \brief Create a vertical scrollbar on the right of the rect.
 *
 * \note This style must be used together with SBS_VERT, and the width of the 
 * scrollbar will depend on the window elements or renderer.
 */
#define SBS_RIGHTALIGN  0x0002

/**
 * \def SBS_NOARROW
 * \brief Create a vertical scrollbar with no arrow.
 *
 * \note This style must't be used together with SBS_NOSHAFT.
 */
#define SBS_NOARROW  0x0004

/**
 * \def SBS_NOSHAFT
 * \brief Create a vertical scrollbar with no shaft.
 *
 * \note This style must't be used together with SBS_NOARROWS.
 */
#define SBS_NOSHAFT  0x0008

/**
 * \def SBS_FIXEDBARLEN
 * \brief Create a scrollbar with fixed thumb.
 *
 */
#define SBS_FIXEDBARLEN  0x0010

/**
 * \def SBS_NOTNOTIFYPARENT
 * \brief send notification code with SendNotifyMessage instead of NotifyParent
 *
 */
#define SBS_NOTNOTIFYPARENT 0x0020

    /** @} end of ctrl_scrollbar_styles */

    /**
     * \defgroup ctrl_scrollbar_msgs Messages of scrollbar control
     * @{
     */

/**
 * \def SBM_GETSCROLLINFO
 * \brief Get the scroll information of the scrollbar.
 *
 */
#define SBM_GETSCROLLINFO           0xF0E0

/**
 * \def SBM_SETSCROLLINFO
 * \brief Set the scroll information of the scrollbar.
 */
#define SBM_SETSCROLLINFO           0xF0E1

/**
 * \def SBM_GETPOS
 * \brief Get the thumb pos of the scrollbar.
 *
 * An application sends a SBM_GETPOS message to get the start position 
 * of the scrollbar thumb. 
 *
 * \code
 * SBM_GETPOS
 * int pos;
 * 
 * pos = SendMessage (hwnd_scrollbar, SBM_GETPOS, 0, 0);
 * \endcode 
 *
 * \return the postion of the thumb.
 */
#define SBM_GETPOS                  0xF0E2

/**
 * \def SBM_SETPOS
 * \brief Set the thumb pos of the scrollbar.
 *
 * An application sends a SBM_SETPOS message to set the start position 
 * of the scrollbar thumb. 
 *
 * \code
 * SBM_SETPOS
 * int pos = 10;
 * BOOL redraw = TRUE;
 * 
 * wParam = (WPARAM)pos;
 * lParam = (LPARAM)redraw;
 * SendMessage (hwnd_scrollbar, SBM_SETPOS, wParam, lParam);
 * \endcode
 *
 * \param pos The new positon of the thumb to set. 
 * \param redraw Whether to repaint the control, TRUE for repaint, FALSE for not.
 *
 * \return the old postion of the thumb.
 */
#define SBM_SETPOS                  0xF0E3

/**
 * \def SBM_GETRANGE
 * \brief Get the range of the scrollbar.
 *
 * An application sends a SBM_GETRANGE message to get the move 
 * range of the scrollbar. 
 *
 * \code
 * SBM_GETRANGE
 * int range;
 * 
 * range = SendMessage (hwnd_scrollbar, SBM_GETRANGE, 0, 0);
 * \endcode 
 *
 * \return the range of the scrollbar.
 */
#define SBM_GETRANGE                0xF0E4

/**
 * \def SBM_SETRANGE
 * \brief Set the range of the scrollbar.
 *
 * An application sends a SBM_SETRANGE message to set the move 
 * range of the scrollbar. 
 *
 * \code
 * SBM_SETRANGE
 * int min = 0;
 * int max = 100;
 * 
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * SendMessage (hwnd_scrollbar, SBM_SETRANGE, wParam, lParam);
 * \endcode
 *
 * \param min The minimum value of the range to set.
 * \param max The maximum value of the range to set.
 *
 * \note this message will not redraw the control.
 */
#define SBM_SETRANGE                0xF0E5

/**
 * \def SBM_SETRANGEREDRAW
 * \brief Set the range of the scrollbar.
 *
 * An application sends a SBM_SETRANGEREDRAW message to set the move 
 * range of the scrollbar and redraw the control. 
 *
 * \code
 * SBM_SETRANGEREDRAW
 * int min = 0;
 * int max = 100;
 * 
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * SendMessage (hwnd_scrollbar, SBM_SETRANGEREDRAW, wParam, lParam);
 * \endcode
 *
 * \param min The minimum value of the range to set.
 * \param max The maximum value of the range to set.
 *
 * \note this message will redraw the control.
 */
#define SBM_SETRANGEREDRAW          0xF0E6

/**
 * \def SBM_ENABLE_ARROW
 * \brief Enable or disable the arrow of the scrollbar.
 *
 * An application sends a SBM_ENABLE_ARROW message to enable or disable
 * the arrow button of the scrollbar. 
 *
 * \code
 * SBM_ENABLE_ARROW
 * int which = SB_ARROW_LTUP;
 * BOOL is_active;
 * 
 * wParam = (WPARAM)which;
 * lParam = (LPARAM)is_active;
 * SendMessage (hwnd_scrollbar, SBM_ENABLE_ARROW, wParam, lParam);
 * \endcode
 *
 * \param which the part of the scrollbar to enable or disable,
 * can be one of the follow value:
 *      - SB_ARROW_LTUP\n
 *        the left or up arrow button.
 *      - SB_ARROW_BTDN\n
 *        the right or down arrow button.
 *      - SB_ARROW_BOTH\n
 *        all the arrow button.
 * \param is_active TRUE for enable ,FALSE for disable.
 *
 */
#define SBM_ENABLE_ARROW            0xF0E7

    /** @} end of ctrl_scrollbar_msgs */

    /** @} end of ctrl_scrollbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCROLLBAR_H */

