/**
 * \file listbox.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: listbox.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_LISTBOX_H
#define _MGUI_CTRL_LISTBOX_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_listbox ListBox control
     * @{
     */

/**
 * \def CTRL_LISTBOX
 * \brief The class name of listbox control.
 */
#define CTRL_LISTBOX        ("listbox")

/** Listbox return value */
#define LB_OKAY                 0
/** Listbox return value */
#define LB_ERR                  (-3)
/** Listbox return value */
#define LB_ERRSPACE             (-2)

#define CMFLAG_BLANK            0x0000
#define CMFLAG_CHECKED          0x0001
#define CMFLAG_PARTCHECKED      0x0002

/**
 * \def CMFLAG_MASK
 * \brief The mask of check mark and image flag value.
 * \sa _LISTBOXITEMINFO
 */    
#define CMFLAG_MASK             0x000F

/**
 * \def IMGFLAG_BITMAP
 * \brief Listbox item style that context of itme is bitmap.
 */  
#define IMGFLAG_BITMAP          0x0010

/**
 * \def LBIS_SIGNIFICANT
 * \brief Listbox item style that item is rendered with WE_FGC_SIGNIFICANT_ITEM
 * and WE_BGC_SIGNIFICANT_ITEM color.
 */  
#define LBIS_SIGNIFICANT        0x0020

/** Structrue of the listbox item info */
typedef struct _LISTBOXITEMINFO
{
    /** Item string */
    char* string;

    /** 
     * Check mark and image flag. It can be one of the following values:
     * - CMFLAG_BLANK
     *   The item is blank.
     * - CMFLAG_CHECKED
     *   The item is checked.
     * - CMFLAG_PARTCHECKED
     *   The item is partly checked.
     *
     * If you want to significant the item which to be render, 
     * you can OR'd \a cmFlag with \a LBIS_SIGNIFICANT.
     *
     * For LBS_ICON list box, if you want to display bitmap other than icon, 
     * you can OR'd \a cmFlag with \a IMGFLAG_BITMAP.
     *
     */
    DWORD   cmFlag;         /* check mark flag */

    /** Handle to the icon (or pointer to bitmap object) of the item */
    HICON   hIcon;          /* handle to icon */
} LISTBOXITEMINFO;

/** 
 * \var typedef LISTBOXITEMINFO* PLISTBOXITEMINFO;
 * \brief Data type of the pointer to a LISTBOXITEMINFO.
 */
typedef LISTBOXITEMINFO* PLISTBOXITEMINFO;

    /**
     * \defgroup ctrl_listbox_styles Styles of listbox control
     * @{
     */

/**
 * \def LBS_NOTIFY
 * \brief Notifies the parent window.
 *
 * Causes the list box to notify the list box parent window 
 * with a notification message when the user clicks or doubleclicks an item.
 */
#define LBS_NOTIFY              0x0001L

/**
 * \def LBS_SORT
 * \brief Sorts strings alphabetically.
 *
 * Causes the list box to sort strings alphabetically that are 
 * added to the list box with an LB_ADDSTRING message.
 */
#define LBS_SORT                0x0002L

/**
 * \def LBS_MULTIPLESEL
 * \brief Causes the list box to allow the user to select multiple items.
 */
#define LBS_MULTIPLESEL         0x0008L

/**
 * \def LBS_CHECKBOX
 * \brief Displays a check box in an item.
 */
#define LBS_CHECKBOX            0x1000L

/**
 * \def LBS_USEICON
 * \brief Displays an icon or bitmap in an item.
 */
#define LBS_USEICON             0x2000L

/**
 * \def LBS_AUTOCHECK
 * \brief If the list box has LBS_CHECKBOX style, this
 *        style tell the box to auto-switch the check box between 
 *        checked or un-checked when the user click the check mark box of an item.
 */
#define LBS_AUTOCHECK           0x4000L

/**
 * \def LBS_AUTOCHECKBOX
 * \brief If the list box has LBS_AUTOCHECKBOX style, this style tell the box
 *        use LBS_CHECKBOX and BLS_AUTOCHECK style.
 */ 
#define LBS_AUTOCHECKBOX        (LBS_CHECKBOX | LBS_AUTOCHECK)

/**
 * \def LBS_SBALWAYS
 * \brief The list box with LBS_SBALWAYS style will always show vertical scrollbar.
 */
#define LBS_SBALWAYS            0x8000L

/**
 * \def LBS_MOUSEFOLLOW
 * \brief The list box with LBS_MOUSEFOLLOW style will always change the 
 *        selected item following mouse.
 */
#define LBS_MOUSEFOLLOW         0x0010L
#if 0
#define LBS_OWNERDRAWFIXED      0x0010L
#define LBS_OWNERDRAWVARIABLE   0x0020L
#define LBS_USETABSTOPS         0x0080L
#define LBS_MULTICOLUMN         0x0200L
#define LBS_WANTKEYBOARDINPUT   0x0400L
#define LBS_NOREDRAW            0x0004L
#define LBS_HASSTRINGS          0x0040L
#define LBS_NOINTEGRALHEIGHT    0x0100L
#define LBS_EXTENDEDSEL         0x0800L
#endif

    /** @} end of ctrl_listbox_styles */

    /**
     * \defgroup ctrl_listbox_msgs Messages of listbox control
     * @{
     */

/**
 * \def LB_ADDSTRING
 * \brief Appends the specified string.
 *
 * An application sends an LB_ADDSTRING message to append an item
 * specified in the lParam parameter to a list box.
 *
 * For a text-only list box:
 *
 * \code
 * LB_ADDSTRING
 * const char* text;
 *
 * wParam = 0;
 * lParam = (LPARAM)text;
 * \endcode
 *
 * \param text Pointer to the string of the item to be added.
 *
 * For a list box with check box or icon 
 * (with LBS_CHECKBOX or LBS_USEICON styles):
 *
 * \code
 * LB_ADDSTRING
 * PLISTBOXITEMINFO plbii;
 *
 * wParam = 0;
 * lParam = (LPARAM)plbii;
 * \endcode
 *
 * \param plbii Pointer to the listbox item info to be added.
 *
 * \return The index of the new item on success, else the one of
 *         the following error codes:
 *
 *         - LB_ERRSPACE    No memory can be allocated for new item.
 *         - LB_ERR         Invalid passed arguments.
 *
 */
#define LB_ADDSTRING            0xF180

/**
 * \def LB_INSERTSTRING
 * \brief Inserts an item to the list box.
 *
 * An application sends an LB_INSERTSTRING message to insert an item 
 * into a list box. Unlike LB_ADDSTRING message, the LB_INSERTSTRING
 * message do not cause the list to be sorted.
 *
 * For a text-only list box:
 *
 * \code
 * LB_INSERTSTRING
 * const char* text;
 *
 * wParam = index;
 * lParam = (LPARAM)text;
 * \endcode
 *
 * \param index Specifies the index of the position at which to insert the item.
 * \param text Pointer to the string of the item to be inserted.
 *
 * For a list box with check box or icon 
 * (with LBS_CHECKBOX or LBS_USEICON styles):
 *
 * \code
 * LB_INSERTSTRING
 * int index;
 * PLISTBOXITEMINFO plbii;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)plbii;
 * \endcode
 * 
 * \param index Specifies the index of the position at which to insert the item.
 * \param plbii Pointer to the listbox item info to be inserted.
 *
 * \return The index of the new item on success, else the one of
 *         the following error codes:
 *
 *         - LB_ERRSPACE    No memory can be allocated for new item.
 *         - LB_ERR         Invalid passed arguments.
 *
 */
#define LB_INSERTSTRING         0xF181

/**
 * \def LB_DELETESTRING
 * \brief Removes an item from the list box.
 *
 * An application sends an LB_DELETESTRING message to a list box 
 * to remove from the list box.
 *
 * \code
 * LB_DELETESTRING
 * int del;
 *
 * wParam = (WPARAM)del;
 * lParam = 0;
 * \endcode
 *
 * \param del The index of the listbox item to be deleted.
 *
 * \return LB_OKAY on success, else LB_ERR to indicate you passed an invalid index.
 */
#define LB_DELETESTRING         0xF182

/**
 * \def LB_SELITEMRANGEEX
 * \brief reserved.
 */
#define LB_SELITEMRANGEEX       0xF183

/**
 * \def LB_RESETCONTENT
 * \brief Removes the contents of a list box.
 *
 * An application sends an LB_RESETCONTENT message to remove the all items
 * in a list box.
 *
 * \code
 * LB_RESETCONTENT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define LB_RESETCONTENT         0xF184

/**
 * \def LB_GETSEL
 * \brief Gets the selected state for an specified item.
 *
 * An application sends an LB_GETSEL message to a list box to get the selected 
 * state for an item specified in the wParam parameter.
 *
 * \code
 * LB_GETSEL
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the specified item.
 *
 * \return The state of the specified item:
 *         - 0\n        Not selected.
 *         - >0\n       Selected.
 *         - LB_ERR\n   Invalid index.
 */
#define LB_GETSEL               0xF187

/**
 * \def LB_SETSEL
 * \brief Selects an item in a multiple-selection list box.
 *
 * An application sends an LB_SETSEL message to select an item 
 * in a multiple-selection list box and scroll it into view if necessary.
 *
 * \code
 * LB_SETSEL
 * int index, sel
 *
 * wParam = (WPARAM)sel;
 * lParam = (LPARAM)index;
 * \endcode
 *
 * \param sel Indicates the changes to be made to the listbox item, 
 *        can be one of the following values:
 *             - -1\n      If the item has been selected, makes it unselected, vice versa.
 *             - 0\n       Makes the item unselected. 
 *             - other\n   Makes the item selected. 
 * \param index The index of the item.
 *
 * \return LB_OKAY on success, else LB_ERR to indicate you passed an invalid index
 *         or the list box has no LBS_MULTIPLESEL style.
 */
#define LB_SETSEL               0xF185

/**
 * \def LB_GETCURSEL
 * \brief Gets the index of the currently selected or highlighted item.
 *
 * An application sends an LB_GETCURSEL message to a list box to get the index of 
 * the currently selected item, if there is one, in a single-selection list box.
 * For multiple-selection list box, appliction send an LB_GETCURSEL message to a 
 * list box to get the index of the current highlighted item.
 *
 * \code
 * LB_GETCURSEL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index of the currently selected item for single-selection list box;
 *         Else the index of the highlighted item for multiple-selection list box.
 */
#define LB_GETCURSEL            0xF188

/**
 * \def LB_SETCURSEL
 * \brief Selects an item.
 *
 * An application sends an LB_SETCURSEL message to a list box to 
 * select an item and scroll it into view, if necessary.
 *
 * \code
 * LB_SETCURSEL
 * int cursel;
 *
 * wParam = (WPARAM)cursel;
 * lParam = 0;
 * \endcode
 *
 * \param cursel The index of the item to be selected and hilighted.
 *
 * \return The old index of the item selected on error, else LB_ERR to
 *         indicate an error occurred.
 */
#define LB_SETCURSEL            0xF186

/**
 * \def LB_GETTEXT
 * \brief Retrieves the text of an item in list box.
 *
 * An application sends an LB_GETTEXT message to a list box to retrieve the text
 * of an item.
 *
 * \code
 * LB_GETTEXT
 * int index;
 * char *string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the selected item.
 * \param string Pointer to the string buffer. The buffer should be large enough
 *        to contain the text of the item.
 *
 * \return One of the following values:
 *         - LB_OKAY\n  Success.
 *         - LB_ERR\n   Invalid item index.
 *
 * \sa LB_GETTEXTLEN
 */
#define LB_GETTEXT              0xF189

/**
 * \def LB_GETTEXTLEN
 * \brief Gets the length of text of item specified in a list box.
 *
 * An application sends an LB_GETTEXTLEN message to a list box to get the length 
 * of text of the item specified in the \a wParam parameter.
 *
 * \code
 * LB_GETTEXTLEN
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the specified item.
 *
 * \return The length of the strings on success, else LB_ERR to indicate invalid index.
 */
#define LB_GETTEXTLEN           0xF18A

/**
 * \def LB_GETCOUNT
 * \brief Gets the number of items in the list box.
 *
 * An application sends an LB_GETCOUNT message to a list box to get the number 
 * of items in the list box.
 *
 * \code
 * LB_GETCOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The number of items in the listbox.
 */
#define LB_GETCOUNT             0xF18B

/**
 * \def LB_SELECTSTRING
 * \brief reserved.
 */
#define LB_SELECTSTRING         0xF18C

/**
 * \def LB_DIR
 * \brief reserved.
 */
#define LB_DIR                  0xF18D

/**
 * \def LB_GETTOPINDEX
 * \brief Gets the index to the first visible item in the list box.
 *
 * An application sends an LB_GETTOPINDEX message to get the index to the first 
 * visible item in the list box. Initially, the first visible item is item 0, but 
 * this changes as the list box is scrolled. 
 *
 * \code
 * LB_GETTOPINDEX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index of the first visible item in the listbox.
 */
#define LB_GETTOPINDEX          0xF18E

/**
 * \def LB_FINDSTRING
 * \brief Searchs a specified string.
 *
 * An application sends an LB_FINDSTRING message to search a list box for an item 
 * that begins with the characters specified in the lParam parameter. The wParam 
 * parameter specifies the zero-based index of the item before the first item to 
 * be searched; The lParam parameter specifies a pointer to a null-terminated 
 * string that contains the prefix to search for.
 *
 * \code
 * LB_FINDSTRING
 * int index;
 * char *string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the item to be searched.
 * \param string The string of the item to be searched.
 *
 * \return The index of the matched item; LB_ERR for not found.
 */
#define LB_FINDSTRING           0xF18F

/**
 * \def LB_GETSELCOUNT
 * \brief Gets the number of selected items in a multiple-selection list box.
 *
 * An application sends an LB_GETSELCOUNT message to a list box to get the number 
 * of selected items in a multiple-selection list box.
 *
 * \code
 * LB_GETSELCOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The number of selected items in the multiple-selection listbox.
 */
#define LB_GETSELCOUNT          0xF190

/**
 * \def LB_GETSELITEMS
 * \brief Gets the numbers of selected items.
 *
 * An application sends an LB_GETSELITEMS message to a list box to fill a buffer 
 * with an array of integers that specify the item numbers of selected items in 
 * a multiple-selection list box.
 *
 * \code
 * LB_GETSELITEMS
 * int nItem;
 * int *pInt;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)pInt;
 * \endcode
 *
 * \param nItem The maximum integer numbers wanted.
 * \param pInt The buffer of an array of integers to save the indices of selected items.
 *
 * \return The number of selected items.
 */
#define LB_GETSELITEMS          0xF191

/**
 * \def LB_SELITEMRANGEEX
 * \brief reserved.
 */
#define LB_SETTABSTOPS          0xF192

/**
 * \def LB_GETHORIZONTALEXTENT
 * \brief reserved.
 */
#define LB_GETHORIZONTALEXTENT  0xF193

/**
 * \def LB_SETHORIZONTALEXTENT
 * \brief reserved.
 */
#define LB_SETHORIZONTALEXTENT  0xF194

/**
 * \def LB_SETCOLUMNWIDTH
 * \brief reserved.
 */
#define LB_SETCOLUMNWIDTH       0xF195

/**
 * \def LB_ADDFILE
 * \brief reserved.
 */
#define LB_ADDFILE              0xF196

/**
 * \def LB_SETTOPINDEX
 * \brief Ensures that a particular item in it is visible.
 *
 * An application sends an LB_SETTOPINDEX message to a list box to ensure that a 
 * particular item in it is visible. The item is specified in the wParam parameter. 
 * The list box scrolls so that either the specified item appears at the top of 
 * the list box or the maximum scroll range has been reached.
 * 
 * \code
 * LB_SETTOPINDEX
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the particular item to be set.
 *
 * \return Always be zero.
 */
#define LB_SETTOPINDEX          0xF197

/**
 * \def LB_GETITEMRECT
 * \brief Retrieves the dimensions of the rectangle.
 *
 * An application sends an LB_GETITEMRECT message to a list box to retrieve 
 * the dimensions of the rectangle that bounds an item as it is currently 
 * displayed in the list box window. The item is specified in the wParam 
 * parameter, and a pointer to a RECT structure is given in the lParam parameter.
 *
 * \code
 * LB_GETITEMRECT
 * int index;
 * RECT *rcItem;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)rcItem;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param rcItem Pointer to the buffer used for storing the item rect;
 *
 * \return LB_OKAY on success; LB_ERR on error.
 */
#define LB_GETITEMRECT          0xF198

/**
 * \def LB_GETITEMDATA
 * \brief Gets item data in a list box if the box has LBS_CHECKBOX
 *          and/or LBS_USEICON styles.
 * 
 * An application sends LB_GETITEMDATA message to a list box to retrive the
 * check box flag and the handle of icon. Note that the text of the item
 * will not be returned, i.e., the field of \a string of LISTBOXITEMINFO
 * structure will be ignored.
 *
 * \code
 * LB_GETITEMDATA
 * int index;
 * PLISTBOXITEMINFO plbii;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)plbii;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param plbii Pointer to the buffer used for storing the item data of the 
 *         specified item.
 *
 * \return LB_OKAY on success; LB_ERR on error.
 *
 * \sa LISTBOXITEMINFO
 */
#define LB_GETITEMDATA          0xF199

/**
 * \def LB_SETITEMDATA
 * \brief Sets item data in a list box if the box has LBS_CHECKBOX
 *          and/or LBS_USEICON styles.
 * 
 * An application sends LB_SETITEMDATA message to a list box to set the
 * check box flag and the handle of icon. Note that the text of the item
 * will not be changed, i.e., the field of \a string of LISTBOXITEMINFO
 * structure will be ignored.
 *
 * \code
 * LB_SETITEMDATA
 * int index;
 * PLISTBOXITEMINFO plbii;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)plbii;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param plbii Pointer to the buffer used for setting the item info of the 
 *         specified item.
 *
 * \return LB_OKAY on success; LB_ERR on error.
 *
 * \sa LISTBOXITEMINFO
 */
#define LB_SETITEMDATA          0xF19A

#define LB_SELITEMRANGE         0xF19B
#define LB_SETANCHORINDEX       0xF19C
#define LB_GETANCHORINDEX       0xF19D

/**
 * \def LB_SETCARETINDEX
 * \brief Sets the focus rectangle to the item at the specified index.
 * 
 * An application sends an LB_SETCARETINDEX message to set the focus rectangle 
 * to the item at the specified index in a multiple-selection list box.
 *
 * \code
 * LB_SETCARETINDEX
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 */
#define LB_SETCARETINDEX        0xF19E

/**
 * \def LB_GETCARETINDEX
 * \brief Determines the index of the item that has the focus rectangle.
 * 
 * An application sends an LB_GETCARETINDEX message to a list box to determine 
 * the index of the item that has the focus rectangle in a multiple-selection 
 * list box.
 *
 * \code
 * LB_GETCARETINDEX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index of the item that has the focus rectangle.
 */
#define LB_GETCARETINDEX        0xF19F

/**
 * \def LB_SETITEMHEIGHT
 * \brief Sets the height of all items.
 * 
 * An application sends an LB_SETITEMHEIGHT message to set the height of 
 * all items in a list box. 
 *
 * \code
 * LB_SETITEMHEIGHT
 * int itemHeight;
 *
 * wParam = 0;
 * lParam = (LPARAM)itemHeight;
 * \endcode
 *
 * \param itemHeight New height of item.
 *
 * \return The effective height of item.
 */
#define LB_SETITEMHEIGHT        0xF1A0

/**
 * \def LB_GETITEMHEIGHT
 * \brief Gets the height in pixels of an item specified in the wParam parameter.
 * 
 * An application sends an LB_GETITEMHEIGHT message to a list box to get the 
 * height in pixels of an item specified in the wParam parameter.
 *
 * \code
 * LB_GETITEMHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The height of item in the listbox.
 */
#define LB_GETITEMHEIGHT        0xF1A1

/**
 * \def LB_FINDSTRINGEXACT
 * \brief Searchs for an item that exactly matches the characters specified.
 * 
 * An application sends an LB_FINDSTRINGEXACT message to a list box to search 
 * it for an item that exactly matches the characters specified in the lParam parameter.
 *
 * \code
 * LB_FINDSTRINGEXACT
 * int index;
 * const char *string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param string The string of the item to be searched for.
 *
 * \return The index of the found item on success, else LB_ERR.
 */
#define LB_FINDSTRINGEXACT      0xF1A2

/**
 * \def LB_SETLOCALE
 * \brief reserved.
 */
#define LB_SETLOCALE            0xF1A5
/**
 * \def LB_GETLOCALE
 * \brief reserved.
 */
#define LB_GETLOCALE            0xF1A6
/**
 * \def LB_SETCOUNT
 * \brief reserved.
 */
#define LB_SETCOUNT             0xF1A7
/**
 * \def LB_INITSTORAGE
 * \brief reserved.
 */
#define LB_INITSTORAGE          0xF1A8
/**
 * \def LB_ITEMFROMPOINT
 * \brief reserved.
 */
#define LB_ITEMFROMPOINT        0xF1A9

/**
 * \def LB_SETTEXT
 * \brief Sets text of the specified item.
 *
 * \code
 * LB_SETTEXT
 * int index;
 * const char *string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param string The string of the item to be set.
 *
 * \return One of the following values:
 *          - LB_OKAY\n Success
 *          - LB_ERR\n  Invalid item index or memory allocation error.
 */
#define LB_SETTEXT              0xF1AA

/**
 * \def LB_GETCHECKMARK
 * \brief Gets check mark status of an item.
 *
 * \code
 * LB_GETCHECKMARK
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the specified item.
 *
 * \return The check mark status of specified item on success; LB_ERR on error.
 *
 * \retval LB_ERR               Invalid item index or the list box have no LBS_CHECKBOX style.
 * \retval CMFLAG_CHECKED       The item is checked.
 * \retval CMFLAG_PARTCHECKED   The item is partly checked.
 * \retval CMFLAG_BLANK         The item is not checked.
 */
#define LB_GETCHECKMARK         0xF1AB

/**
 * \def LB_SETCHECKMARK
 * \brief Sets check mark status of an item.
 *
 * \code
 * LB_SETCHECKMARK
 * int index, status;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)status;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param status The new check mark status, can be one of the followings.
 *             - CMFLAG_CHECKED\n The item is checked.
 *             - CMFLAG_PARTCHECKED\n The item is partly checked.
 *
 * \return One of the following values:
 *          - LB_OKAY\n
 *              Success
 *          - LB_ERR\n
 *              Invalid item index or this list box have no LBS_CHECKBOX style.
 */
#define LB_SETCHECKMARK         0xF1AC

/**
 * \def LB_GETITEMADDDATA
 * \brief Gets the 32-bit data value associated with an item.
 * 
 * An application sends an LB_GETITEMADDDATA message to a list box to get the 
 * 32-bit data value the list box has stored for the item with index of 
 * \a wParam; By default this is zero. An application must set the 
 * item data value by sending an LB_SETITEMADDDATA message to the list box for 
 * this value to have meaning.
 *
 * \code
 * LB_GETITEMADDDATA
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the specified item.
 *
 * \return The 32-bit data value associated with an item on success, otherwise
 *         LB_ERR to indicate an error.
 */
#define LB_GETITEMADDDATA       0xF1AD

/**
 * \def LB_SETITEMADDDATA
 * \brief Associates a 32-bit data value with an item.
 *
 * An application sends an LB_SETITEMADDDATA message to associate a 32-bit data 
 * value specified in the \a lParam parameter with an item in the list box.
 *
 * \code
 * LB_SETITEMADDDATA
 * int index;
 * DWORD addData;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)addData;
 * \endcode
 *
 * \param index The index of the specified item.
 * \param addData the 32-bit data value which will associated with the item.
 *
 * \return One of the following values:
 *          - LB_OKAY\n         Success
 *          - LB_ERR\n          Invalid item index
 */
#define LB_SETITEMADDDATA       0xF1AE

/**
 * \def LB_SETSTRCMPFUNC
 * \brief Sets the STRCMP function used to sort items.
 *
 * An application sends an LB_SETSTRCMPFUNC message to set a 
 * new STRCMP function to sort items.
 *
 * Note that you should send this message before adding 
 * any item to the list box control.
 *
 * \code
 * static int my_strcmp (const char* s1, const char* s2, size_t n)
 * {
 *      ...
 *      return 0;
 * }
 *
 * LB_SETSTRCMPFUNC
 *
 * wParam = 0;
 * lParam = (LPARAM)my_strcmp;
 * \endcode
 *
 * \param my_strcmp Your own function to compare two strings.
 *
 * \return One of the following values:
 *          - LB_OKAY\n         Success
 *          - LB_ERR\n          Not an empty list box.
 */
#define LB_SETSTRCMPFUNC        0xF1AF

/**
 * \def LB_SETITEMDISABLE
 * \brief Sets the item disable.
 *
 * An application sends an LB_SETITEMDISABLE message to set a
 * item to disable.
 *
 * \code
 * wParam = index;
 * lParam = (LPARAM)disable;
 * \endcode
 *
 * \return One of the following values:
 *          - LB_OKAY\n         Success
 *          - LB_ERR\n          index of item is not exist.
 */
#define LB_SETITEMDISABLE       0xF1B0

/**
 * \def LB_GETITEMDISABLE
 * \brief Get the item disable status.
 *
 * An application sends an LB_SETITEMDISABLE message to get a 
 * item to disable status.
 *
 * \code
 * wParam = index;
 * lParam = 0;
 * \endcode
 *
 * \return One of the following values:
 *          - TRUE\n         Item is disable
 *          - FLASE\n        Item is enable
 */
#define LB_GETITEMDISABLE       0xF1B1

/**
 * \def LB_SETITEMBOLD
 * \brief Sets the item display use bold font.
 *
 * An application sends an LB_SETITEMBOLD message to set a 
 * item display use bold font.
 *
 * \code
 * wParam = index;
 * lParam = (LPARAM)bold;
 * \endcode
 *
 * \return One of the following values:
 *          - LB_OKAY\n         Success
 *          - LB_ERR\n          index of item is not exist.
 */
#define LB_SETITEMBOLD          0xF1B2

/**
 * \def LB_MULTIADDITEM
 * \brief Appends the multi strings.
 *
 * An application sends an LB_MULTIADDITEM message to append item array
 * specified in the lParam parameter to a list box.
 *
 * For a text-only list box:
 *
 * \code
 * LB_MULTIADDITEM
 * int num;
 * const char text[num][];
 *
 * wParam = num;
 * lParam = (LPARAM)text;
 * \endcode
 *
 * \param num is count of text array
 * \param text pointer to the string of the item array to be added.
 *
 * For a list box with check box or icon 
 * (with LBS_CHECKBOX or LBS_USEICON styles):
 *
 * \code
 * LB_MULTIADDITEM
 * LISTBOXITEMINFO plbii[num];
 *
 * wParam = num;
 * lParam = (LPARAM)plbii;
 * \endcode
 *
 * \param num is count of plbii array
 * \param plbii pointer to the listbox item info to be added.
 *
 * \return The index of the new item on success, else the one of
 *         the following error codes:
 *
 *         - LB_ERRSPACE    No memory can be allocated for new item.
 *         - LB_ERR         Invalid passed arguments.
 *
 */
#define LB_MULTIADDITEM         0xF1B3

#define LB_MSGMAX               0xF1B4

    /** @} end of ctrl_listbox_msgs */

    /**
     * \defgroup ctrl_listbox_ncs Notification codes of listbox control
     * @{
     */

/**
 * \def LBN_ERRSPACE
 * \brief Indicates that memory is not enough.
 * 
 * A list box sends an LBN_ERRSPACE notification message to its parent window 
 * when it cannot allocate enough memory to complete the current operation.
 */
#define LBN_ERRSPACE            255

/**
 * \def LBN_SELCHANGE
 * \brief Indicates change due to mouse or keyboard user input.
 * 
 * A list box created with the LBS_NOTIFY style sends an LBN_SELCHANGE 
 * notification message to its parent window when the selection is about to 
 * change due to mouse or keyboard user input.
 */
#define LBN_SELCHANGE           1

/**
 * \def LBN_DBLCLK
 * \brief Indicates double click on an item.
 * 
 * A list box created with the LBS_NOTIFY style sends an LBN_DBLCLK notification 
 * message to its parent window when the user double-clicks a string in it.
 */
#define LBN_DBLCLK              2

/**
 * \def LBN_SELCANCEL
 * \brief Indicates cancel of the selection in the list box.
 * 
 * A list box created with the LBS_NOTIFY style sends an LBN_SELCANCEL 
 * notification message to its parent window when the user cancels the selection 
 * in the list box.
 */
#define LBN_SELCANCEL           3

/**
 * \def LBN_SETFOCUS
 * \brief Indicates gain of input focus.
 * 
 * A list box sends an LBN_SETFOCUS notification message to its parent window 
 * when the list box gains the input focus.
 */
#define LBN_SETFOCUS            4

/**
 * \def LBN_KILLFOCUS
 * \brief Indicates loss of input focus.
 * 
 * A list box sends an LBN_KILLFOCUS notification message to its parent window 
 * when the list box loses the input focus.
 */
#define LBN_KILLFOCUS           5

/**
 * \def LBN_CLICKCHECKMARK
 * \brief Indicates click on the check mark.
 */
#define LBN_CLICKCHECKMARK      6

/**
 * \def LBN_CLICKED
 * \brief Indicates click on the string.
 */
#define LBN_CLICKED             7

/**
 * \def LBN_ENTER
 * \brief Indicates the user has pressed the ENTER key.
 */
#define LBN_ENTER               8

    /** @} end of ctrl_listbox_ncs */

    /** @} end of ctrl_listbox */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_LISTBOX_H */

