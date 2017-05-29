/**
 * \file menubutton.h
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
 * $Id: menubutton.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_MENUBUTTON_H
#define _MGUI_CTRL_MENUBUTTON_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_menubutton MenuButton control
     * @{
     */

/**
 * \def CTRL_MENUBTN
 * \brief The class name of menubutton control.
 */
#define CTRL_MENUBTN        ("menubutton")

/**
 * \def CTRL_MENUBUTTON
 * \brief The class name of menubutton control.
 */
#define CTRL_MENUBUTTON     ("menubutton")

/** Menu Button return value */
#define MB_OKAY                 0
/** Menu Button return value */
#define MB_ERR                  -1
/** Menu Button return value */
#define MB_INV_ITEM             -2
/** Menu Button return value */
#define MB_ERR_SPACE            -3

/** Struct used by parent to add/retrive item */
#define MB_WHICH_TEXT           0x01
/** Struct used by parent to add/retrive item */
#define MB_WHICH_BMP            0x02
/** Struct used by parent to add/retrive item */
#define MB_WHICH_ATTDATA        0x04

/** Structure of the menubutton item */
typedef struct _MENUBUTTONITEM
{
    /**
     * Which fields are valid when sets/retrives the item information 
     * (ignored when add item). It can be OR'd with the following values:
     *
     *  - MB_WHICH_TEXT The \a text field is valid.
     *  - MB_WHICH_BMP The \a bmp field is valid.
     *  - MB_WHICH_ATTDATA The \a data field is valid.
     */
    DWORD           which;
    /** Item string */
    const char*     text;
    /** Item bitmap */
    PBITMAP         bmp;
    /** Attached data */
    DWORD           data;
} MENUBUTTONITEM;

/**
 * \var typedef MENUBUTTONITEM* PMENUBUTTONITEM;
 * \brief Data type of the pointer to a MENUBUTTONITEM.
 */
typedef MENUBUTTONITEM* PMENUBUTTONITEM;

    /**
     * \defgroup ctrl_menubutton_styles Styles of menubutton control
     * @{
     */

/**
 * \def MBS_SORT
 * \brief If this bit is set, the items listed in the control 
 * are displayed in a specified order.  
 */
#define MBS_SORT                0x0001

/**
 * \def MBS_LEFTARROW
 * \brief The menu pull-down arrow will be display at the left of the text.
 */
#define MBS_LEFTARROW           0x0002

/**
 * \def MBS_NOBUTTON
 * \brief The control have not push button.
 */
#define MBS_NOBUTTON            0x0004

/**
 * \def MBS_ALIGNLEFT
 * \brief The text on menubutton is left-align (default).
 */
#define MBS_ALIGNLEFT           0x0000

/**
 * \def MBS_ALIGNRIGHT
 * \brief The text on menubutton is right-align.
 */
#define MBS_ALIGNRIGHT          0x0010

/**
 * \def MBS_ALIGNCENTER
 * \brief The text on menubutton is center-align.
 */
#define MBS_ALIGNCENTER         0x0020

/**
 * \def MBS_ALIGNMASK
 * \brief The align mask of menubutton.
 */
#define MBS_ALIGNMASK           0x00F0

    /** @} end of ctrl_menubutton_styles */

    /**
     * \defgroup ctrl_menubutton_msgs Messages of menubutton control
     * @{
     */

/**
 * \def MBM_ADDITEM
 * \brief Sends to the control to add an item to the menu list.
 *
 * \code
 * MBM_ADDITEM
 * int pos;
 * MENUBUTTONITEM newitem;
 *
 * wParam = (WPARAM)pos;
 * lParam = (LPARAM)&newitem;
 * \endcode
 *
 * \param pos The position at which to add the item. If the control
 *        was created with the style of \a MBS_SORT, this parameter
 *        will be ignored. If this parameter is less than 0, 
 *        the new item will be append to the tail of the menu list.
 * \param newitem Pointer to the menubutton item info structure.
 *
 * \return The position at which the item has been added, i.e.,
 *         the index of the added item if success. Otherwise, 
 *         the following error code will be returned:
 *
 *        - MB_ERR_SPACE\n  No memory can be allocated for new item.
 */
#define MBM_ADDITEM                 0xF200

/**
 * \def MBM_DELITEM
 * \brief Sends to the control to delete an item in the menu list.
 *
 * \code
 * MBM_DELETEITEM
 * int del;
 *
 * wParam = (WPARAM)del;
 * lParam = 0;
 * \endcode
 *
 * \param del The index of the item to be deleted.
 *
 * \return MB_OKAY if success, else MB_INV_ITEM to indicate valid index.
 */
#define MBM_DELITEM                 0xF201

/**
 * \def MBM_RESETCTRL
 * \brief Sends to the control to remove all items in the menu list.
 *
 * \code
 * MBM_RESETCTRL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define MBM_RESETCTRL               0xF202

/**
 * \def MBM_SETITEMDATA
 * \brief Sends to the control to set the data of a specific item.
 *
 * \code
 * MBM_SETITEMDATA
 * int index;
 * PMENUBUTTONITEM pmbi;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)pmbi;
 * \endcode
 *
 * \param index The index of the item to be altered.
 * \param pmbi Pointer to the MENUBUTTONITEM structure that stores the new 
 *         menubutton item data.
 *
 * \return MB_OKAY if success, otherwise will be one of the following error codes:
 *
 *          - MB_INV_ITEM\n     Indicate that the index you passed is valid.
 *          - MB_ERR_SPACE\n    No memory can be allocated for new item data.
 */
#define MBM_SETITEMDATA             0xF203

/**
 * \def MBM_GETITEMDATA
 * \brief Sends to the control to retrive the data of a specific item.
 *
 * \code
 * MBM_GETITEMDATA
 * int index;
 * PMENUBUTTONITEM pmbi;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)pmbi;
 * \endcode
 *
 * \param index The index of the specific item.
 * \param pmbi Pointer to the MENUBUTTONITEM structure for storing the 
 *         menubutton item data.
 *
 * \return MB_OKAY if success, otherwise MB_INV_ITEM to indicate invalid index.
 */
#define MBM_GETITEMDATA             0xF204

/**
 * \def MBM_GETCURITEM
 * \brief Sends to get the index of the current selected item.
 *
 * \code
 * MBM_GETCURITEM
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index of the current selected item. If there is no seleted
 *         item, MB_INV_ITEM will be returned.
 */
#define MBM_GETCURITEM              0xF206

/**
 * \def MBM_SETCURITEM
 * \brief Sends to set the current selected item based on index.
 *
 * \code
 * MBM_SETCURITEM
 * int cur;
 *
 * wParam = (WPARAM)cur;
 * lParam = 0;
 * \endcode
 *
 * \param cur The index to be the current item.
 *
 * \return The index of the old selected item.
 */
#define MBM_SETCURITEM              0xF207

/**
 * \def MBM_SETSTRCMPFUNC
 * \brief Sets the STRCMP function used to sort items.
 *
 * An application sends a MBM_SETSTRCMPFUNC message to set a 
 * new STRCMP function to sort items in the menubutton.
 *
 * Note that you should send this message before adding 
 * any item to the menubutton control.
 *
 * \code
 * static int my_strcmp (const char* s1, const char* s2, size_t n)
 * {
 *      ...
 *      return 0;
 * }
 *
 * MBM_SETSTRCMPFUNC
 *
 * wParam = 0;
 * lParam = (LPARAM) my_strcmp;
 * \endcode
 *
 * \param my_strcmp Your own function to compare two strings.
 *
 * \return One of the following values:
 *          - MB_OKAY\n     Success
 *          - MB_ERR\n      Not an empty menubutton
 */
#define MBM_SETSTRCMPFUNC           0xF208

/* Internal */
#define MBM_MSGMAX                  0xF210

    /** @} end of ctrl_menubutton_msgs */

    /**
     * \defgroup ctrl_menubutton_ncs Notification codes of menubutton control
     * @{
     */

/**
 * \def MBN_ERRSPACE
 * \brief Sends when memory space error occures.
 */
#define MBN_ERRSPACE            255

/**
 * \def MBN_CHANGED
 * \brief Sends when selected item changes.
 */
#define MBN_CHANGED             1

/**
 * \def MBN_SELECTED
 * \brief Sends when an item is selected.
 */
#define MBN_SELECTED            2

/**
 * \def MBN_STARTMENU
 * \brief Sends when starting tracking popup menu.
 */
#define MBN_STARTMENU           4

/**
 * \def MBN_ENDMENU
 * \brief Sends when ending tracking popup menu.
 */
#define MBN_ENDMENU             5

/**
 * \def MBN_CLICKED
 * \brief Sends when the user clicked the menubutton but not active the menu.
 */
#define MBN_CLICKED             6

    /** @} end of ctrl_menubutton_ncs */

    /** @} end of ctrl_menubutton */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_MENUBUTTON_H */

