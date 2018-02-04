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
** treeview.h: the head file for TreeView Control.
**
** Create date: 2000/12/01
*/

#ifndef __TREEVIEW_IMPL_H_
#define __TREEVIEW_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#ifndef __MINIGUI_LIB__

/****** TreeView Control *************************************************/

#define CTRL_TREEVIEW            ("treeview")

#endif /* !__MINIGUI_LIB__ */

/** TreeView item structure */
typedef struct _TVITEM
{
    /** text of the item */
    char *text;

    /**
     * State flags of the item, can be OR'ed by the following values:
     * - TVIF_ROOT\n
     *   The item is a root item.
     * - TVIF_SELECTED\n
     *   The item is a selected item.
     * - TVIF_FOLD\n
     *   The item is folded.
     */
    DWORD dwFlags;

    /** Handle to the icon of the folded item. */
    HICON hIconFold;
    /** Handle to the icon of the unfolded item. */
    HICON hIconUnfold;

    /** Additional data associated with the item. */
    DWORD dwAddData;

    /** Depth in the tree */
    int depth;

    /** Pointer to the structure of the next treeview item */
    struct _TVITEM *next;
    /** Pointer to the structure of the first child item */
    struct _TVITEM *child;
    /** Pointer to the parent item */
    struct _TVITEM *parent;

    /** Structure of the text extension */
    SIZE text_ext;
} TVITEM;
/**
 * \var typedef TVITEM *PTVITEM;
 * \brief Data type of the pointer to a TVITEM.
 */
typedef TVITEM *PTVITEM;

typedef struct tagTVDATA
{
    /* draw icon or not. */
    DWORD            dwStyle;

    /* identifier. */
    int              id;

    /* STRCMP function */
    STRCMP           str_cmp;

    /* how many items can be showed in client area. */
    unsigned int     nVisCount;

    /* the height of one item. */
    unsigned int     nItemHeight;

    /* Number of all items in treeview. */
    unsigned int     nItemCount;

    /*
     * Number of items which can (not should) be drawed
     * in client area. That means excluding children of fold items.
     */
    unsigned int     nVisItemCount;

    /* how many items are above the client area and should not be drawed. */
    unsigned int     nItemTop;

    /* how many pixels that treeview has moved toward left. */
    unsigned int     nLeft;

    /* the width of treeview. */
    unsigned int     nWidth;

    /* how many pixels can be showed in client area. */
    unsigned int     nVisWidth;

    /* this item is selected. */
    PTVITEM          pItemSelected;

    /* the root item of treeview. */
    PTVITEM          root;
} TVDATA;
typedef TVDATA* PTVDATA;

BOOL RegisterTreeViewControl (void);

#ifdef  __cplusplus
}
#endif

#endif /* __TREEVIEW_IMPL_H_ */

