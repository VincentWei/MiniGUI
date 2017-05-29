/*
** $Id: listview_impl.h 8810 2007-12-26 05:17:04Z xwyan $ 
**
** listview.h: header file of ListView control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
*/

#ifndef _LISTVIEW_IMPL_H_
#define _LISTVIEW_IMPL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define LV_COLW_DEF	        62  // default column width
#define COLWIDTHMIN             10  // minimum column width

#define LV_LEFTMARGIN           1
#define LV_TOPMARGIN            1
#define LV_RIGHTMARGIN          1
#define LV_BOTTOMMARGIN         1

#define LV_HDR_HEIGHT        (plvdata->nHeadHeight)

//default header height
#define LV_HDRH_DEF(hwnd)       (GetWindowFont(hwnd)->size + 6)
//default item height
#define LV_ITEMH_DEF(hwnd)      (GetWindowFont(hwnd)->size + 6)

#define LV_HDR_TOP		0   // top of the header

#define INDENT                  20  // tree node indent value

typedef enum sorttype
{
     NOTSORTED = 0,
     HISORTED, 
     LOSORTED
} SORTTYPE;

/** subitem flags */
#define LVIF_NORMAL	0x0000L
#define LVIF_BITMAP  	0x0001L
#define LVIF_ICON  	0x0002L

/** item flags */
//#define LVIF_ROOT       0x0000L
//#define LVIF_FOLD       0x0001L

typedef struct _subitemdata
{
    list_t list;
    DWORD  dwFlags;              // subitem flags
    char   *pszInfo;             // text of the subitem
    int    nTextColor;           // text color of the subitem
    DWORD  Image;                // image of the subitem
} SUBITEMDATA;
typedef SUBITEMDATA *PSUBITEMDATA;

typedef struct _itemdata
{
    SVITEMDATA svitem;

    list_t subqueue;
    /**
     * State flags of the item, can be OR'ed by the following values:
     * - LVIF_ROOT\n
     *   The item is a root item.
     * - LVIF_FOLD\n
     *   The item is folded.
     */
    DWORD dwFlags;

    /* FIXME */
    /** item height, used fold status */
    int itemh;

    /** Depth in the tree */
    int depth;

    /** Pointer to the structure of the next treeview item */
    struct _itemdata *next;
    /** Pointer to the structure of the first child item */
    struct _itemdata *child;
    /** Pointer to the parent item */
    struct _itemdata *parent;
    /** number of child items */
    int   child_nr;

    DWORD itemData;             // item additional data for user
    DWORD addData;              // internal use for scrollview item
} ITEMDATA;
typedef ITEMDATA *PITEMDATA;

/* column header struct */
typedef struct _lsthdr
{
    list_t        list;
    int           x;             // x position of the header
    int           width;         // width of the header/column/subitem
    SORTTYPE      sort;          // sort status
    char          *pTitle;       // title text of the column header
    PFNLVCOMPARE  pfnCmp;        // pointer to the application-defined or default
                                 // comparision function
    DWORD         Image;         // image of the header
    DWORD         flags;         // header and column flags
} LSTHDR;
typedef LSTHDR *PLSTHDR;

#define LVST_NORMAL	0x0000    //normal status
#define LVST_BDDRAG	0x0001    //the border is being dragged
#define LVST_HEADCLICK	0x0002    //the header is being clicked
#define LVST_INHEAD	0x0004    //mouse move in header
#define LVST_ITEMDRAG   0x0008    //mouse drag item
#define LVST_CAPTURED   0x0010    //captured status

/* this macro doesn't check the pointer, so, be careful */
#define LVSTATUS(hwnd)  ( ((PLVDATA)GetWindowAdditionalData2(hwnd))->status )

/* listview object struct */
typedef struct _lstvwdata
{
    /* scrollview object */
    SVDATA     svdata;

    int        nHeadHeight;         // header height
    int        nHeadWidth;          // header width
    int        bkc_selected;        // background color of the selected item

    int        nCols;               // current column number
    //int        nRows;               // current item number

    int        nColCurSel;	    // current column selected.

    int        nItemDraged;         // the header beging dragged
    PLSTHDR    pHdrClicked;         // the header being clicked
    DWORD      status;              // list view status: dragged, clicked

    list_t     hdrqueue;            // header list
    HWND       hWnd;                // the control handle

    STRCMP     str_cmp;             // default strcmp function

    PFNLVCOMPARE pfn_sort;          // FIXME, internal use
    int        col_sort;            // FIXME, internal use
    int        cur_col;             // FIXME, internal use 

    ITEMDATA   rootitem;               // root item, not visible

    PFN_LVHDR_BKDRAWFUNC pfn_draw_hdr_bk;       //drawing header background
    PFN_LVHDR_ITEMDRAWFUNC pfn_draw_hdr_item;   //drawing header item
} LVDATA;
typedef LVDATA *PLVDATA;


BOOL RegisterListViewControl (void);

#define plvscr          (&plvdata->svdata.scrdata)
#define plvroot         (&plvdata->rootitem)


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _LISTVIEW_IMPL_H_ */

