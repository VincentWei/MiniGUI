/**
 * \file treeview.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
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
 * $Id: treeview.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef EXT_TREEVIEW_H
#define EXT_TREEVIEW_H


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_treeview TreeView control
     *
     * \note You can pass the root item information through \a dwAddData
     * argument of CreateWindowEx function. 
     *
     * \code
     * HICON folded, unfolded;
     * TVITEMINFO tvii = {"/proc", 0, folded, unfolded};
     *
     * CreateWindowEx (CTRL_TREEVIEW, ..., (DWORD)&tvii);
     * \endcode
     * 
     * @{
     */

/**
 * \def CTRL_TREEVIEW
 * \brief The class name of treeview control.
 */
#define CTRL_TREEVIEW           ("TreeView")
#define CTRL_TREEVIEW_RDR           ("TreeView_rdr")

#define TVIF_ROOT               0x0001L
#define TVIF_FOLD               0x0004L
#define TVIF_SELECTED           0x0008L

/** TreeView item info structure */
typedef struct _TVITEMINFO
{
    /** Text of the item */
    char *text;
    /**
     * State flags of the item, can be OR'ed by the following values:
     *
     * - TVIF_ROOT\n
     *   The item is a root item.
     * - TVIF_SELECTED\n
     *   The item is a selected item.
     * - TVIF_FOLD\n
     *   The item is folded.
     *
     * When adding an item to the TreeView control, only TVIF_FOLD
     * flag is valid.
     */
    DWORD dwFlags;

    /** Handle to the icon of the folded item. */
    HICON hIconFold;
    /** Handle to the icon of the unfolded item. */
    HICON hIconUnfold;

    /** Additional data associated with the item. */
    DWORD dwAddData;
} TVITEMINFO;

/**
 * \var typedef TVITEMINFO *PTVITEMINFO;
 * \brief Data type of the pointer to a TVITEMINFO.
 */
typedef TVITEMINFO *PTVITEMINFO;

    /**
     * \defgroup mgext_ctrl_treeview_styles Styles of treeview control
     * @{
     */

/**
 * \def TVS_NOTIFY
 * \brief Sends the parent window notification messages when the user 
 *        clicks or double-clicks the control.
 */
#define TVS_NOTIFY              0x0001L

/**
 * \def TVS_WITHICON
 * \brief The treeview item will use icons to indicate folded or unfolded status.
 */
#define TVS_WITHICON            0x0002L

/**
 * \def TVS_ICONFORSELECT
 * \brief The fold icon is used to indicate selected item.
 *
 * For TreeView control with TVS_ICONFORSELECT style, the item icons
 * will be used to indicate selected or unselected status of the item.
 *
 * \note This style is obselete since MiniGUI V3.0.
 *
 */
#define TVS_ICONFORSELECT       0x0000L

/* Internally used style */
#define TVS_FOCUS               0x0008L

/**
 * \def TVS_SORT
 * \brief Enables sorting in the treeview.
 */
#define TVS_SORT                0x0010L

    /** @} end of mgext_ctrl_treeview_styles */

    /**
     * \defgroup mgext_ctrl_treeview_msgs Messages of treeview control
     * @{
     */

/**
 * \def TVM_ADDITEM
 * \brief Adds a new item in a treeview control.
 *
 * \code
 * TVM_ADDITEM
 * GHANDLE parent;
 * PTVITEMINFO newIteminfo;
 *
 * wParam = (WPARAM)parent;
 * lParam = (LPARAM)newIteminfo;
 * \endcode
 *
 * \param parent Handle to the item who is the parent of the new item.
 * \param newIteminfo Pointer to the item info structure of the new item 
 *        to be added.
 *
 * \return The handle to the new item on success, otherwise return 0.
 */
#define TVM_ADDITEM             0xF110

/**
 * \def TVM_INSERTITEM
 * \brief The same as TVM_ADDITEM message.
 */
#define TVM_INSERTITEM          0xF111

/**
 * \def TVM_GETROOT
 * \brief Gets the root item of a treeview control.
 *
 * \code
 * TVM_GETROOT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The handle to the root item.
 */
#define TVM_GETROOT             0xF112

/**
 * \def TVM_DELTREE
 * \brief Removes an item and its descendant items for the treeview control.
 *
 * \code
 * TVM_DELTREE
 * GHANDLE item;
 *
 * wParam = (WPARAM)item;
 * lParam = 0;
 * \endcode
 *
 * \return Zero on success, otherwise -1.
 */
#define TVM_DELTREE             0xF113

/**
 * \def TVM_SEARCHITEM
 * \brief Searches an item matching a specified string in a subtree rooted by a specific item.
 *
 * \note This is a depth first search.
 *
 * \code
 * TVM_SEARCHITEM
 * GHNADLE item;
 * char* string;
 *
 * wParam = (WPARAM)item;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param found The handle to the item which is the root item of the subtree.
 *        Zero means the root item.
 * \param string The specified string.
 *
 * \return The handle to the item whose text matches the specified string
 *         on success, otherwise zero for not found or invalid parameters.
 */
#define TVM_SEARCHITEM            0xF114

/**
 * \def TVM_FINDCHILD
 * \brief Finds a child item matching a specified string in children of a specific item.
 *
 * \code
 * TVM_FINDCHILD
 * GHNADLE item;
 * char* string;
 *
 * wParam = (WPARAM)item;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param item The handle to the item. Zero means the root item.
 * \param string The specified string.
 *
 * \return The handle to the item whose text matches the specified string
 *         on success, otherwise zero for not found or invalid parameters.
 */
#define TVM_FINDCHILD           0xF115

/**
 * \def TVM_GETSELITEM
 * \brief Gets the selected item.
 *
 * \code
 * TVM_GETSELITEM
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Handle to the selected item.
 */
#define TVM_GETSELITEM          0xF116

/**
 * \def TVM_SETSELITEM
 * \brief Sets the selected item.
 *
 * \code
 * TVM_SETSELITEM
 * GHANDLE item;
 *
 * wParam = (WPARAM)item;
 * lParam = 0;
 * \endcode
 *
 * \param item The handle to candicate item.
 *
 * \return The handle to the old selected item on success, otherwise -1.
 */
#define TVM_SETSELITEM          0xF117

/**
 * \def TVM_GETITEMTEXTLEN
 * \brief Retrives the text length of the specified item in TreeView control.
 *
 * \code
 * TVM_GETITEMTEXTLEN
 *
 * GHANDLE item;
 *
 * wParam = (WPARAM)item;
 * lParam = (LPARAM)0;
 * \endcode
 *
 * \param item The handle to the item which we want to retrive.
 *
 * \return The length of the item text on success, otherwise -1.
 */
#define TVM_GETITEMTEXTLEN      0xF118

/**
 * \def TVM_GETITEMTEXT
 * \brief Retrives the text of the specified item in TreeView control.
 *
 * \code
 * TVM_GETITEMTEXT
 *
 * GHANDLE item;
 * char* buffer;
 *
 * wParam = (WPARAM)item;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param item The handle to the item which we want to retrive.
 * \param buffer The pointer to a buffer to save the text of the item.
 *
 * \return The length of the item text on success, otherwise -1.
 */
#define TVM_GETITEMTEXT         0xF119

/**
 * \def TVM_GETITEMINFO
 * \brief Retrives the information of an item in TreeView control.
 *
 * \code
 * TVM_GETITEMINFO
 *
 * GHANDLE item;
 * TVITEMINFO* tvii;
 *
 * wParam = (WPARAM)item;
 * lParam = (LPARAM)tvii;
 * \endcode
 *
 * \param item The handle to the item which we want to retrive.
 * \param tvii A pointer to a TVITEMINFO structure to receive the item information.
 *        Note that the \a text field of this structure should be large enough to 
 *        get the item text.
 *
 * \return Zero on success, otherwise -1.
 *
 * \sa TVM_GETITEMTEXTLEN
 */
#define TVM_GETITEMINFO         0xF11A

/**
 * \def TVM_SETITEMINFO
 * \brief Changes the information of an item.
 *
 * \code
 * TVM_SETITEMTEXT
 * GHANDLE item;
 * TVITEMINFO* tvii;
 *
 * wParam = (WPARAM)id;
 * lParam = (LPARAM)tvii;
 * \endcode
 *
 * \param item The handle of the item.
 * \param newText Pointer to a TVITEMINFO structure which include the new information
 *        of the item.
 *
 * \return Zero on success, otherwise -1.
 */
#define TVM_SETITEMINFO         0xF11B

#define TVIR_PARENT             1
#define TVIR_FIRSTCHILD         2
#define TVIR_NEXTSIBLING        3
#define TVIR_PREVSIBLING        4

/**
 * \def TVM_GETRELATEDITEM
 * \brief Retrives related item of specific item.
 *
 * \code
 * TVM_GETRELATEDITEM
 * int related;
 * GHANDLE item;
 *
 * wParam = (WPARAM)related;
 * lParam = (LPARAM)item;
 * \endcode
 *
 * \param related A integer which indicates the relationship between 
 *        the item to retrive and the specified item, can be one of the following values:
 *          - TVIR_PARENT\n
 *              To retrive the parent item of the specified item.
 *          - TVIR_FIRSTCHILD\n
 *              To retrive the first child item of the specified item.
 *          - TVIR_NEXTSIBLING\n
 *              To retrive the next sibling item of the specified item.
 *          - TVIR_PREVSIBLING\n
 *              To retrive the previous sibling item of the specified item.
 *
 * \param item The handle to the known item.
 *
 * \return The handle to the related item on success, otherwise 0.
 */
#define TVM_GETRELATEDITEM      0xF11C

/**
 * \def TVM_SETSTRCMPFUNC
 * \brief Sets the STRCMP function used to sort items.
 *
 * An application sends a TVM_SETSTRCMPFUNC message to set a 
 * new STRCMP function to sort items in the TreeView control.
 *
 * Note that you should send this message before adding 
 * any item to the TreeView control.
 *
 * \code
 * static int my_strcmp (const char* s1, const char* s2, size_t n)
 * {
 *      ...
 *      return 0;
 * }
 *
 * TVM_SETSTRCMPFUNC
 *
 * wParam = 0;
 * lParam = (LPARAM) my_strcmp;
 * \endcode
 *
 * \param my_strcmp Your own function to compare two strings.
 *
 * \return One of the following values:
 *          - 0\n     Success
 *          - -1\n    Not an empty TreeView control
 */
#define TVM_SETSTRCMPFUNC       0xF11D

#define TVM_MSGMAX              0xF120

    /** @} end of mgext_ctrl_treeview_msgs */

    /**
     * \defgroup mgext_ctrl_treeview_ncs Notification codes of treeview control
     * @{
     */

/**
 * \def TVN_ERRSPACE
 * \brief Indicates that memory is not enough.
 * 
 * A list box sends an TVN_ERRSPACE notification message to its parent window 
 * when it cannot allocate enough memory to complete the current operation.
 */
#define TVN_ERRSPACE            255

/**
 * \def TVN_SELCHANGE
 * \brief Notifies the change of selection.
 */
#define TVN_SELCHANGE           1

/**
 * \def TVN_DBLCLK
 * \brief Notifies the user has double-clicked an item.
 */
#define TVN_DBLCLK              2

/**
 * \def TVN_SETFOCUS
 * \brief Indicates gain of input focus.
 * 
 * A TreeView control sends an TVN_SETFOCUS notification message to its parent window 
 * when the list box gains the input focus.
 */
#define TVN_SETFOCUS            4

/**
 * \def TVN_KILLFOCUS
 * \brief Indicates loss of input focus.
 * 
 * A TreeView control sends an TVN_KILLFOCUS notification message to its parent window 
 * when the list box loses the input focus.
 */
#define TVN_KILLFOCUS           5

/**
 * \def TVN_CLICKED
 * \brief Indicates the user has clicked an item.
 */
#define TVN_CLICKED             7

/**
 * \def TVN_ENTER
 * \brief Indicates the user has pressed the ENTER key.
 */
#define TVN_ENTER               8

/**
 * \def TVN_FOLDED
 * \brief Indicates a sub-tree has folded.
 *
 * \note This is an extended notification, the control calls \a NotifyParentEx function
 *       to notify the parent, and passes the handle to the folded item through
 *       \a add_data argument of \a NotifyParentEx function. You should define and
 *       set Notificaton Callback Procedure for the control in order to
 *       get the handle to the folded item.
 *
 * \sa SetNotificationCallback, NotifyParentEx
 */
#define TVN_FOLDED              9

/**
 * \def TVN_UNFOLDED
 * \brief Indicates a sub-tree has unfolded.
 *
 * \note This is an extended notification, the control calls \a NotifyParentEx function
 *       to nofity the parent, and passes the handle to the unfolded item through
 *       \a add_data argument of \a NotifyParentEx function. You should define and
 *       set Notificaton Callback Procedure for the control in order to
 *       get the handle to the unfolded item.
 * 
 * \sa SetNotificationCallback, NotifyParentEx
 */
#define TVN_UNFOLDED              10

    /** @} end of mgext_ctrl_treeview_ncs */

    /** @} end of mgext_ctrl_treeview */

    /** @} end of controls */

#ifdef  __cplusplus
}
#endif

#endif /* EXT_TREEVIEW_H */

