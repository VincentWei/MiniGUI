/**
 * \file combobox.h
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
 * $Id: combobox.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_COMBOBOX_H
#define _MGUI_CTRL_COMBOBOX_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_combobox ComboBox control
     * @{
     */

/**
 * \def CTRL_COMBOBOX
 * \brief The class name of combobox control.
 */
#define CTRL_COMBOBOX       ("combobox")

    /**
     * \defgroup ctrl_combobox_styles Styles of combobox control
     * @{
     */

/**
 * \def CBS_SIMPLE
 * \brief Displays the list box at all times.
 *
 * The current selection in the list box is displayed in the edit control.
 *
 * \note The argument \a dwAddData of \a CreateWindowEx function should be the
 * expected height of the list box.
 *
 * \code
 * int listbox_height = 100;
 *
 * CreateWindowEx (CTRL_COMBOBOX, ..., listbox_height);
 * \endcode
 */
#define CBS_SIMPLE              0x0000L

/**
 * \def CBS_AUTOSPIN
 * \brief Creates AutoSpin box.
 *
 * AutoSpin box has an input field with a spin button. The input field
 * displays an integer, and you can click spin button to increase or
 * decrease the value of the integer.
 */
#define CBS_AUTOSPIN            0x0001L

/**
 * \def CBS_SPINLIST
 * \brief Creates SpinList box.
 *
 * SpinList box has an input field with a spin button. The input field
 * displays some text, and you can click spin button to change the text.
 * The candidate text string comes from the strings you add to the box.
 */
#define CBS_SPINLIST            0x0002L

/**
 * \def CBS_DROPDOWNLIST
 * \brief Specifies a drop-down list box in the selection filed.
 *
 * \note The argument \a dwAddData of \a CreateWindowEx function should be the
 * expected height of the list box.
 *
 * \code
 * int listbox_height = 100;
 *
 * CreateWindowEx (CTRL_COMBOBOX, ..., listbox_height));
 * \endcode
 */
#define CBS_DROPDOWNLIST        0x0003L

/**
 * \def CBS_TYPEMASK
 * \brief The type mask of style of combobox.
 */
#define CBS_TYPEMASK            0x0007L

/**
 * \def CBS_NOTIFY
 * \brief Notifies the parent window.
 *
 * Causes the combo box to notify the parent window 
 * with a notification message.
 */
#define CBS_NOTIFY              0x0008L

/**
 * \def CBS_SPINARROW_TOPBOTTOM
 * \brief The spin arrow.
 */
#define CBS_SPINARROW_TOPBOTTOM 0x0010L

/**
 * \def CBS_SPINARROW_LEFTRIGHT
 * \brief The spin arrow.
 */
#define CBS_SPINARROW_LEFTRIGHT 0x0020L

/**
 * \def CBS_AUTOHSCROLL
 * \brief This style is reserved. 
 */

#define CBS_AUTOHSCROLL         0x0040L

/**
 * \def CBS_DISABLENOSCROLL
 * \brief This style is reserved. 
 */

#define CBS_DISABLENOSCROLL     0x0080L

/**
 * \def CBS_SORT
 * \brief Automatically sorts strings entered in the list box.
 */
#define CBS_SORT                0x0100L

/**
 * \def CBS_AUTOLOOP
 * \brief Loops the value automatically if the type of combobox is CBS_AUTOSPIN.
 */
#define CBS_AUTOLOOP            0x0200L

/**
 * \def CBS_EDITNOBORDER
 * \brief The edit box has no border.
 */
#define CBS_EDITNOBORDER        0x0400L

/**
 * \def CBS_EDITBASELINE
 * \brief The edit box has base line.
 */
#define CBS_EDITBASELINE        0x0800L

/**
 * \def CBS_READONLY
 * \brief The edit field is read-only.
 */
#define CBS_READONLY            0x1000L

/**
 * \def CBS_UPPERCASE
 * \brief The edit field is uppercase.
 */
#define CBS_UPPERCASE           0x2000L

/**
 * \def CBS_LOWERCASE
 * \brief The edit field is lowercase.
 */
#define CBS_LOWERCASE           0x4000L

/**
 * \def CBS_AUTOFOCUS
 * \brief The edit field will gain the focus automatically.
 */
#define CBS_AUTOFOCUS           0x8000L

    /** @} end of ctrl_combobox_styles */

    /**
     * \defgroup ctrl_combobox_msgs Messages of combobox control
     * @{
     */

/**
 * \def CB_GETEDITSEL
 * \brief Gets the starting and ending character positions of the current 
 *        selection.
 *
 * An application sends a CB_GETEDITSEL message to get the starting and ending 
 * character positions of the current selection in the edit control of a combo box.
 *
 * \code
 * CB_GETEDITSEL
 * int start;
 * int end;
 *
 * wParam = (WPARAM)&start;
 * lParam = (LPARAM)&end;
 * \endcode
 *
 * \param start Pointer to a 32-bit value that receives the starting
 *           position of the selection.
 * \param end Pointer to a 32-bit value that receives the ending
 *           position of the selection.
 *
 * \note Not implemented yet.
 */
#define CB_GETEDITSEL               0xF140

/**
 * \def CB_LIMITTEXT
 * \brief Limits the length of text in the edit control.
 *
 * An application sends a CB_LIMITTEXT message to limit the length of the text
 * the user may type into the edit control of a combo box. 
 *
 * \code
 * CB_LIMITTEXT
 * int newLimit;
 *
 * wParam = (WPARAM)newLimit;
 * lParam = 0;
 * \endcode
 *
 * \param newLimit Specifies the maximum number of characters the user can enter.
 *
 * \return The return value is always zero.
 */
#define CB_LIMITTEXT                0xF141

/**
 * \def CB_SETEDITSEL
 * \brief Sets the starting and ending character positions of the current 
 *        selection.
 *
 * An application sends a CB_SETEDITSEL message to set the starting and ending 
 * character positions of the current selection in the edit control of a combo box.
 *
 * \code
 * CB_SETEDITSEL
 * int start;
 * int end;
 *
 * wParam = (WPARAM)start;
 * lParam = (LPARAM)end;
 * \endcode
 *
 * \param start The starting position of the selection.
 * \param end The ending position of the selection.
 *
 * \note Not implemented yet.
 */
#define CB_SETEDITSEL               0xF142

/**
 * \def CB_ADDSTRING
 * \brief Adds a string to the list box of a combo box.
 *
 * An application sends a CB_ADDSTRING message to add a string to the list box
 * of a combo box. 
 *
 * \code
 * CB_ADDSTRING
 * char* string;
 *
 * wParam = 0;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param string Pointer to the null-terminated string to be added.
 *
 * \return The index of the new item on success, else the one of
 *         the following error codes:
 *
 *         - CB_ERRSPACE    No memory can be allocated for new item.
 *         - CB_ERR         Invalid passed arguments.
 */
#define CB_ADDSTRING                0xF143

/**
 * \def CB_DELETESTRING
 * \brief Deletes a string in the list box of a combo box.
 *
 * An application sends a CB_DELETESTRING message to delete a string in the list box
 * of a combo box. 
 *
 * \code
 * CB_DELETESTRING
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index Specifies the index of the string to delete.
 *
 * \return If succeed, return zero; otherwise CB_ERR.
 */
#define CB_DELETESTRING             0xF144

#define CB_DIR                      0xF145

/**
 * \def CB_GETCOUNT
 * \brief Retreives the number of items in the list box of a combo box.
 *
 * An application sends a CB_GETCOUNT message to retreive the number of items 
 * in the list box of a combo box. 
 *
 * \code
 * CB_GETCOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The number of items in the list box.
 */
#define CB_GETCOUNT                 0xF146

/**
 * \def CB_GETCURSEL
 * \brief Retreives the index of the currently selected item in the list box.
 *
 * An application sends a CB_GETCURSEL message to retreive the index of the 
 * currently selected item in the list box of a combo box. 
 *
 * \code
 * CB_GETCURSEL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The index of currently selected item in the list box if have one;
 *         otherwise CB_ERR.
 */
#define CB_GETCURSEL                0xF147

/**
 * \def CB_GETLBTEXT
 * \brief Retreives the string of an item in the list box.
 *
 * An application sends a CB_GETLBTEXT message to retreive the string of 
 * a specified item in the list box of a combo box. 
 *
 * \code
 * CB_GETLBTEXT
 * int index;
 * char* string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the selected item.
 * \param string Pointer to the string buffer.
 *
 * \return One of the following values:
 *         - CB_OKAY\n  Success.
 *         - CB_ERR\n   Invalid item index.
 */
#define CB_GETLBTEXT                0xF148

/**
 * \def CB_GETLBTEXTLEN
 * \brief Gets the string length of an item in the list box.
 *
 * An application sends a CB_GETLBTEXTLEN message to get the string length
 * of a specified item in the list box of a combo box. 
 *
 * \code
 * CB_GETLBTEXTLEN
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the specified item.
 *
 * \return The length of the string on success; otherwise CB_ERR.
 */
#define CB_GETLBTEXTLEN             0xF149

/**
 * \def CB_INSERTSTRING
 * \brief Inserts a string to the list box of a combo box.
 *
 * An application sends a CB_INSERTSTRING message to insert a string to the list 
 * box of a combo box. Unlike the CB_ADDSTRING message, the CB_INSERTSTRING message
 * do not cause a list to be sorted.
 *
 * \code
 * CB_INSERTSTRING
 * int index;
 * char* string;
 *
 * wParam = (WPARAM)index;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param index The index of the position at which to insert the string.
 * \param string Pointer to the null-terminated string to be added.
 *
 * \return The index of the new item on success, else the one of
 *         the following error codes:
 *
 *         - CB_ERRSPACE    No memory can be allocated for new item.
 *         - CB_ERR         Invalid passed arguments.
 */
#define CB_INSERTSTRING             0xF14A

/**
 * \def CB_RESETCONTENT
 * \brief Removes all items from the list box and edit control.
 *
 * An application sends a CB_RESETCONTENT message remove all items from the list
 * box and edit control of a combo box. 
 *
 * \code
 * CB_RESETCONTENT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define CB_RESETCONTENT             0xF14B

/**
 * \def CB_FINDSTRING
 * \brief Searchs the list box for an item beginning with the characters in a 
 *         specified string.
 *
 * An application sends a CB_FINDSTRING message to search the list box for an 
 * item beginning with the characters in a specified string.
 *
 * \code
 * CB_FINDSTRING
 * int indexStart;
 * char* string;
 *
 * wParam = (WPARAM)indexStart;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param indexStart Index of the item preceding the first item to be searched.
 * \param string Pointer to the null-terminated string that contains the prefix
 *             to search for.
 *
 * \return The index of the matching item or CB_ERR to indicate not found.
 */
#define CB_FINDSTRING               0xF14C

#define CB_SELECTSTRING             0xF14D

/**
 * \def CB_SETCURSEL
 * \brief Selects a string in the list of a combo box.
 *
 * \code
 * CB_SETCURLSEL
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the string to select. 
 *
 * \return CB_OKAY on success; otherwise CB_ERR to indicate an invalid index.
 */
#define CB_SETCURSEL                0xF14E

#define CB_SHOWDROPDOWN             0xF14F

/**
 * \def CB_GETITEMADDDATA
 * \brief Retreives the application-supplied 32-bit value associated with the 
 *         specified item.
 *
 * An application sends an CB_GETITEMADDDATA message to retrive the 32-bit data 
 * value associated with with an item in the list box of the combo box.
 *
 * \code
 * CB_GETITEMADDDATA
 * int index;
 *
 * wParam = (WPARAM)index;
 * lParam = 0;
 * \endcode
 *
 * \param index The index of the item.
 *
 * \return The 32-bit data value associated with an item on success, otherwise
 *         CB_ERR to indicate an error.
 */
#define CB_GETITEMADDDATA           0xF150

/**
 * \def CB_SETITEMADDDATA
 * \brief Sets a 32-bit data value with the specified item.
 * 
 * An application sends an CB_SETITEMADDDATA message to associate a 32-bit data 
 * value specified in the lParam parameter with an item in the list box that 
 * is specified in the wParam parameter.
 *
 * \code
 * CB_SETITEMADDDATA
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
 *          - CB_OKAY\n         Success
 *          - CB_ERR\n          Invalid item index
 */
#define CB_SETITEMADDDATA              0xF151

/**
 * \def CB_GETDROPPEDCONTROLRECT
 * \brief Retreives the screen coordinates of the dropdown list box of a combo box. 
 *
 * \code
 * CB_GETDROPPEDCONTROLRECT
 * RECT *rect;
 *
 * wParam = 0;
 * lParam = (LPARAM)rect;
 * \endcode
 *
 * \param rect Pointer to the RECT structure used to save the coordinates.
 */
#define CB_GETDROPPEDCONTROLRECT    0xF152

/**
 * \def CB_SETITEMHEIGHT
 * \brief Sets the height of all items of the list box in a combo box.
 * 
 * An application sends an CB_SETITEMHEIGHT message to set the height of 
 * all items of the list box in a combo box.
 *
 * \code
 * CB_SETITEMHEIGHT
 * int itemHeight;
 *
 * wParam = 0;
 * lParam = (LPARAM)itemHeight;
 * \endcode
 *
 * \param itemHeight New height of item of the list box.
 *
 * \return The effective height of item of the list box.
 */
#define CB_SETITEMHEIGHT            0xF153

/**
 * \def CB_GETITEMHEIGHT
 * \brief Gets the height of items of the list box in a combo box.
 *
 * \code
 * CB_GETITEMHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The height of item in the list box.
 */
#define CB_GETITEMHEIGHT            0xF154

#define CB_SETEXTENDEDUI            0xF155
#define CB_GETEXTENDEDUI            0xF156

/**
 * \def CB_GETDROPPEDSTATE
 * \brief Determines whether the list box of a combo box is dropped down.
 *
 * \code
 * CB_GETIDROPSTATE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return If the list box is visible, the return value is TRUE; 
 *         otherwise, it is FALSE.
 */
#define CB_GETDROPPEDSTATE          0xF157

/**
 * \def CB_FINDSTRINGEXACT
 * \brief Searchs the list box for an item that matches the specified string. 
 *
 * An application sends a CB_FINDSTRINGEXACT message to search the list box for an 
 * item that matches a specified string.
 *
 * \code
 * CB_FINDSTRINGEXACT
 * int indexStart;
 * char* string;
 *
 * wParam = (WPARAM)indexStart;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param indexStart Index of the item preceding the first item to be searched.
 * \param string Pointer to the null-terminated string to search for.
 *
 * \return The index of the matching item; otherwise CB_ERR to indicate not found.
 */
#define CB_FINDSTRINGEXACT          0xF158

#define CB_SETLOCALE                0xF159
#define CB_GETLOCALE                0xF15A
#define CB_GETTOPINDEX              0xF15b
#define CB_SETTOPINDEX              0xF15c
#define CB_GETHORIZONTALEXTENT      0xF15d
#define CB_SETHORIZONTALEXTENT      0xF15e
#define CB_GETDROPPEDWIDTH          0xF15f
#define CB_SETDROPPEDWIDTH          0xF160
#define CB_INITSTORAGE              0xF161

/**
 * \def CB_SETSPINFORMAT
 * \brief Sets the format string of value for CBS_AUTOSPIN type.
 *
 * \code
 * CB_SETSPINFORMAT
 * const char* format;
 *
 * wParam = 0;
 * lParam = (LPARAM)format;
 * \endcode
 *
 * \param format A format string can be used by \a sprintf function
 *        to format an interger.
 *
 * \return CB_OKAY on success; otherwise CB_ERR.
 */
#define CB_SETSPINFORMAT             0xF162

/**
 * \def CB_SETSPINRANGE
 * \brief Determines the range of the spin box in a combo box.
 *
 * \code
 * CB_SETSPINRANGE
 * int new_min;
 * int new_max;
 *
 * wParam = (WPARAM)new_min;
 * lParam = (LPARAM)new_max;
 * \endcode
 *
 * \param new_min The new minimum value of the spin box.
 * \param new_max The new maximum value of the spin box.
 *
 * \return CB_OKAY on success; otherwise CB_ERR to indicate invalid parameters.
 */
#define CB_SETSPINRANGE             0xF163

/**
 * \def CB_GETSPINRANGE
 * \brief Gets the range of the spin box in a combo box.
 *
 * \code
 * CB_GETSPINRANGE
 * int *spin_min;
 * int *spin_max;
 *
 * wParam = (WPARAM)spin_min;
 * lParam = (LPARAM)spin_max;
 * \endcode
 *
 * \param spin_min The minimum value of the spin box.
 * \param spin_max The maximum value of the spin box.
 *
 * \return Always be CB_OKAY.
 */
#define CB_GETSPINRANGE             0xF164

/**
 * \def CB_SETSPINVALUE
 * \brief Determines the value of the spin box in a combo box.
 *
 * \code
 * CB_SETSPINVALUE
 * int new_value;
 *
 * wParam = (WPARAM)new_value;
 * lParam = 0;
 * \endcode
 *
 * \param new_value The new value of the spin box.
 *
 * \return CB_OKAY on success; otherwise CB_ERR to indicate invalid parameters.
 */
#define CB_SETSPINVALUE             0xF165

/**
 * \def CB_GETSPINVALUE
 * \brief Gets the current value of the spin box in a combo box.
 *
 * \code
 * CB_GETSPINVALUE
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current value of the spin box.
 */
#define CB_GETSPINVALUE             0xF166

/**
 * \def CB_SETSPINPACE
 * \brief Determines the pace and the fast pace of the spin box in a combo box.
 *
 * \code
 * CB_SETSPINPACE
 * int new_pace;
 * int new_fastpace;
 *
 * wParam = (WPARAM)new_pace;
 * lParam = (LPARAM)new_fastpace;
 * \endcode
 *
 * \param new_pace The new pace value of the spin box.
 * \param new_fastpace The new fast pace value of the spin box.
 *
 * \return Always be CB_OKAY.
 */
#define CB_SETSPINPACE              0xF167

/**
 * \def CB_GETSPINPACE
 * \brief Gets the pace and the fast pace of the spin box in a combo box.
 *
 * \code
 * CB_GETSPINPACE
 * int *spin_pace;
 * int *spin_fastpace;
 *
 * wParam = (WPARAM)spin_pace;
 * lParam = (LPARAM)spin_fastpace;
 * \endcode
 *
 * \param spin_pace Pointer to the data to retreive the new pace value of the spin box.
 * \param spin_fastpace Pointer to the data to retreive the new fast pace value of the spin box.
 *
 * \return Always be CB_OKAY.
 */
#define CB_GETSPINPACE              0xF168

/**
 * \def CB_SPIN
 * \brief Spins the value of the spin box or auto spin box.
 *
 * \code
 * CB_SPIN
 *
 * int direct;
 *
 * wParam = (WPARAM)direct;
 * lParam = 0;
 * \endcode
 *
 * \param direct Indicats the direct of the spin. Zero means up, non-zero down.
 *
 * \return Always be CB_OKAY.
 */
#define CB_SPIN                     0xF170

/**
 * \def CB_FASTSPIN
 * \brief Fast spins the value of the spin box or auto spin box.
 *
 * \code
 * CB_FASTSPIN
 *
 * int direct
 *
 * wParam = (WPARAM)direct;
 * lParam = 0;
 * \endcode
 *
 * \param direct Indicats the direct of the spin. Zero means up, non-zero down.
 *
 * \return Always be CB_OKAY.
 */
#define CB_FASTSPIN                 0xF171

/**
 * \def CB_SETSTRCMPFUNC
 * \brief Sets the STRCMP function used to sort items.
 *
 * An application sends a CB_SETSTRCMPFUNC message to set a 
 * new STRCMP function to sort items in the combo-box.
 *
 * Note that you should send this message before adding 
 * any item to the combo-box control.
 *
 * \code
 * static int my_strcmp (const char* s1, const char* s2, size_t n)
 * {
 *      ...
 *      return 0;
 * }
 *
 * CB_SETSTRCMPFUNC
 *
 * wParam = 0;
 * lParam = (LPARAM) my_strcmp;
 * \endcode
 *
 * \param my_strcmp Your own function to compare two strings.
 *
 * \return One of the following values:
 *          - CB_OKAY\n
 *              Success
 *          - CB_ERR\n
 *              This combobox has no list box or it is not an empty list box.
 */
#define CB_SETSTRCMPFUNC            0xF172

/**
 * \def CB_GETCHILDREN
 * \brief Gets the handles to the children of a ComboBox control.
 *
 * An application sends a CB_GETCHILDREN message to get the handles 
 * to the children of a ComboBox control.
 *
 * \code
 * CB_GETCHILDREN
 *
 * HWND *wnd_edit, *wnd_listbox;
 *
 * wParam = (WPARAM)wnd_edit;
 * lParam = (LPARAM)wnd_listbox;
 * \endcode
 *
 * \param wnd_edit The buffer saving the handle to the edit box of the ComboBox control.
 * \param wnd_list The buffer saving the handle to the list box of the ComboBox control.
 *        If the ComboBox have type of CBS_AUTOSPIN, handle to the list box will be 0.
 *
 * \return Always be CB_OKAY.
 */
#define CB_GETCHILDREN              0xF173

#define CB_MSGMAX                   0xF180

    /** @} end of ctrl_combobox_msgs */

/** Combo Box return ok value */
#define CB_OKAY                 LB_OKAY
/** Combo Box return error value */
#define CB_ERR                  LB_ERR
/** Combo Box return space error value */
#define CB_ERRSPACE             LB_ERRSPACE

    /**
     * \defgroup ctrl_combobox_ncs Notification codes of combobox control
     * @{
     */

#define CBN_ERRSPACE            255

/**
 * \def CBN_SELCHANGE
 * \brief Notifies the change of the current selection.
 *
 * The CBN_SELCHANGE notification code is sent when the user changes the current
 * selection in the list box of a combo box.
 */
#define CBN_SELCHANGE           1

/**
 * \def CBN_DBLCLK
 * \brief Notifies the user has double clicked an item.
 *
 * A combo box created with the CBS_NOTIFY style sends an CBN_DBLCLK notification 
 * message to its parent window when the user double-clicks a string in its listbox.
 */
#define CBN_DBLCLK              2

/**
 * \def CBN_SETFOCUS
 * \brief Notifies the box has gained the input focus.
 */
#define CBN_SETFOCUS            3

/**
 * \def CBN_KILLFOCUS
 * \brief Notifies the box has lost the input focus.
 */
#define CBN_KILLFOCUS           4

/**
 * \def CBN_EDITCHANGE
 * \brief Notifies the change of the text in the edit control.
 *
 * The CBN_EDITCHANGE notification code is sent when the user has taken an action
 * that may have altered the text in the edit control portion of a combo box.
 */
#define CBN_EDITCHANGE          5

#define CBN_EDITUPDATE          6

/**
 * \def CBN_DROPDOWN
 * \brief Notifies the list box has been dropped down.
 */
#define CBN_DROPDOWN            7

/**
 * \def CBN_CLOSEUP
 * \brief Notifies the list box has been closed up.
 */
#define CBN_CLOSEUP             8

/**
 * \def CBN_SELECTOK
 * \brief Notifies the selection of a list item.
 *
 * The CBN_SELECTOK notification code is sent when the user has 
 * selected a list item.
 */
#define CBN_SELECTOK            9

/**
 * \def CBN_SELECTCANCEL
 * \brief Notifies that the selection of a list item is ignored.
 *
 * The CBN_SELECTCANCEL notification code is sent when the user has selected a list
 * item but then selects another control or closes the dialog box.
 */
#define CBN_SELECTCANCEL        10

/**
 * \def CBN_CLICKED
 * \brief Notifies that the user has clicked combo box.
 *
 * The CBN_CLICKED notification code is sent when the user has clicked combo box.
 */
#define CBN_CLICKED        11
    
    /** @} end of ctrl_combobox_ncs */

    /** @} end of ctrl_combobox */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_COMBOBOX_H */

