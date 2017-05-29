/*
** $Id: treeview_impl.h 11510 2009-04-10 07:28:46Z houhuihua $
**
** treeview.h: the head file for TreeView Control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001, 2002 Zheng Yiran, Wei Yongming.
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

