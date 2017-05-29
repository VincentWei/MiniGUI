/**
 * \file propsheet.h
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
 * $Id: propsheet.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_PROPSHEET_H
#define _MGUI_CTRL_PROPSHEET_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_propsheet PropertySheet control
     * @{
     */

/**
 * \def CTRL_PROPSHEET
 * \brief The class name of propsheet control.
 */
#define CTRL_PROPSHEET       ("propsheet")

    /**
     * \defgroup ctrl_propsheet_styles Styles of propertysheet control
     * @{
     */

/**
 * \def PSS_SIMPLE
 * \brief A simple property sheet control. All tabs of the control
 *        will have the same width.
 */
#define PSS_SIMPLE                0x0000L

/**
 * \def PSS_COMPACTTAB
 * \brief Compact tab style. The width of a tab is adaptive to the tab title.
 */
#define PSS_COMPACTTAB            0x0001L

/**
 * \def PSS_SCROLLABLE
 * \brief Tabs can be scrolled by scroll button.
 */
#define PSS_SCROLLABLE            0x0002L

/**
 * \def PSS_BOTTOM
 * \brief Bottom tab style. Tabs will be located at the bottom of 
 * the property sheet.
 */
#define PSS_BOTTOM                0x0010L

    /** @} end of ctrl_propsheet_styles */

    /**
     * \defgroup ctrl_propsheet_msgs Messages of propertysheet control
     * @{
     */
/** Propsheet return value */
#define PS_OKAY         0
/** Propsheet return value */
#define PS_ERR          (-1)

/**
 * \def PSM_GETACTIVEPAGE
 * \brief Gets the handle of current active page.
 *
 * Sends this message to retreive the propsheet window's active page.
 *
 * \code
 * PSM_GETACTIVEPAGE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The handle to the active page; HWND_INVALID if no such a page.
 */
#define PSM_GETACTIVEPAGE         0xF1C0L

/**
 * \def PSM_SETACTIVEINDEX
 * \brief Changes the active page by index.
 *
 * Sends this message to change the propsheet window's active page.
 *
 * \code
 * PSM_SETACTIVEINDEX
 * int page;
 *
 * wParam = (WPARAM)page;
 * lParam = 0;
 * \endcode
 *
 * \param page Index of the page to set.
 *
 * \return PS_OKAY on success, otherwise PS_ERR.
 */
#define PSM_SETACTIVEINDEX         0xF1C2L

/**
 * \def PSM_GETPAGE
 * \brief Gets the handle of a page by index.
 *
 * Sends this message to retreive the handle to a page by index.
 *
 * \code
 * PSM_GETPAGE
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the page.
 *
 * \return The handle to the page; HWND_INVALID if no such a page.
 */
#define PSM_GETPAGE               0xF1C3L

/**
 * \def PSM_GETACTIVEINDEX
 * \brief Gets the index of the current active page.
 *
 * Sends this message to retreive the index of the propsheet window's active page.
 *
 * \code
 * PSM_GETACTIVEINDEX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index number of the active page.
 */
#define PSM_GETACTIVEINDEX        0xF1C4L

/**
 * \def PSM_GETPAGEINDEX
 * \brief Gets the index of a page by handle.
 *
 * Sends this message to retreive the index to a page by handle.
 *
 * \code
 * PSM_GETPAGEINDEX
 * HWND hwnd;
 *
 * wParam = hwnd;
 * lParam = 0;
 * \endcode
 *
 * \param hwnd The handle of the page.
 *
 * \return The index of the page; PS_ERR if no such a page.
 */
#define PSM_GETPAGEINDEX          0xF1C5L

/**
 * \def PSM_GETPAGECOUNT
 * \brief Gets the number of pages of the propsheet.
 *
 * Sends this message to retreive the number of pages currently in the propsheet.
 *
 * \code
 * PSM_GETPAGECOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The number of pages in the propsheet.
 */
#define PSM_GETPAGECOUNT          0xF1C6L

/**
 * \def PSM_GETTITLELENGTH
 * \brief Gets the length of a page title.
 *
 * Sends this message to retreive the title length of a page.
 *
 * \code
 * PSM_GETTITLELENGTH
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index number of the page in the propsheet.
 *
 * \return The length of the page in the propsheet; PS_ERR if no such a page.
 */
#define PSM_GETTITLELENGTH        0xF1C7L

/**
 * \def PSM_GETTITLE
 * \brief Gets a page title.
 *
 * Sends this message to retreive the title of a page.
 *
 * \code
 * PSM_GETTITLE
 * int index;
 * char *buffer;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param index The index number of the page in the propsheet.
 * \param buffer The buffer storing the title string.
 *
 * \return PS_OKAY on success; PS_ERR if no such a page.
 */
#define PSM_GETTITLE              0xF1C8L

/**
 * \def PSM_SETTITLE
 * \brief Sets a page title.
 *
 * Sends this message to specify the title of a page.
 *
 * \code
 * PSM_SETTITLE
 * int index;
 * char *buffer;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param index The index number of the page in the propsheet.
 * \param buffer The string buffer storing the title.
 *
 * \return PS_OKAY on success; PS_ERR if no such a page.
 */
#define PSM_SETTITLE              0xF1C9L

/**
 * \def PSM_ADDPAGE
 * \brief Adds a page to the propsheet.
 *
 * Sends this message to add a page to the propsheet.
 *
 * \code
 * PSM_ADDPAGE
 * DLGTEMPLATE *dlg_tmpl;
 * WNDPROC proc;
 *
 * wParam = (WPARAM)dlg_tmpl;
 * lParam = (LPARAM)proc;
 * \endcode
 *
 * \param hdlg The handle of the page window to be added in the propsheet.
 * \param proc The window callback procedure of the page window.
 *        Note that the procedure should call DefaultPageProc function
 *        by default.
 *
 * \return The index of the page added on success; PS_ERR on error.
 */
#define PSM_ADDPAGE               0xF1D0L

/**
 * \def PSM_REMOVEPAGE
 * \brief Removes a page from the propsheet.
 *
 * Sends this message to remove a page from the propsheet and destroys the 
 * associated controls.
 *
 * \code
 * PSM_REMOVEPAGE
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index number of the page to be removed from the propsheet.
 *
 * \return If success, return PS_OKAY, otherwise PS_ERR.
 */
#define PSM_REMOVEPAGE            0xF1D1L

/**
 * \def PSM_SHEETCMD
 * \brief Sends a MSG_SHEETCMD message to all pages in the propsheet.
 *
 * If you send MSG_SHEETCMD message to the propsheet control, the control
 * will broadcast the message to all pages it contains. The page callback 
 * procedure will receive the message and handle it. If one page return non-zero
 * value, the broadcast will be broken and the message will return a value 
 * indicating which page returned error. The value will be equal to the page index 
 * plus one.
 *
 * The PSM_SHEETCMD can be used by property sheet window, i.e., the container
 * of the property pages. The sheet can create three buttons, like
 * "Ok", "Cancel", and "Apply". When the user clicked the "Apply" or "Ok"
 * button, it can send a PSM_SHEETCMD message to the propsheet control, the
 * control will then send the message to all pages to notify pages to apply
 * the changes made by the user. If there are some errors, the page can return 
 * a non-zero value to indicate an invalid chage so that the sheet can stop
 * to close the sheet window. You can tell the pages which action should
 * be taken by passing a value through the WPARAM parameter of the message.
 * 
 * \code
 * PSM_SHEETCMD
 * WPARAM wParam;
 * LPARAM lParam;
 *
 * wParam = (WPARAM)wParam;
 * lParam = (LPARAM)lParam;
 * \endcode
 *
 * \param wParam The WPARAM parameter of the MSG_SHEETCMD message.
 * \param lParam The LPARAM parameter of the MSG_SHEETCMD message.
 *
 * \return The message has been broken by a page, the value will be
 *         (page_index + 1); Zero indicates no page asserts an error.
 */
#define PSM_SHEETCMD              0xF1D2L

#define PSM_MSGMAX              0xF1E0L

    /** @} end of ctrl_propsheet_msgs */

    /**
     * \defgroup ctrl_propsheet_ncs Notification codes of propertysheet control
     * @{
     */

/**
 * \def PSN_ACTIVE_CHANGED
 * \brief Notifies the parent window that the active page of 
 *        the propsheet has been changed.
 */
#define PSN_ACTIVE_CHANGED        0x01

    /** @} end of ctrl_propsheet_ncs */

    /** @} end of ctrl_propsheet */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_PROPSHEET_H */

