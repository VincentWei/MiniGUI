/**
 * \file scrollview.h
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
 * $Id: scrollview.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_SCROLLVIEW_H
#define _MGUI_CTRL_SCROLLVIEW_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_scrollview ScrollView control
     * @{
     *
     * Scrollview control is a scrollable window, which has a visible area and
     * normally a larger content area, user can browse content area using scrollbar.
     * Contents of the content area is totally user-defined, you can add controls in it,
     * add customed listed items, or anything you want.
     *
     * In fact, you can build a control needing scrolled window support on scrollview.
     */

/**
 * \def CTRL_SCROLLVIEW
 * \brief The class name of scrollview control, uses this name to create a scrollable
 *        window that consists of items.
 *
 */
#define CTRL_SCROLLVIEW           ("scrollview")

/**
 * \def CTRL_SCROLLWND
 * \brief The class name of scrollwnd control, uses this name to create a scrollable
 *        window to which you can add controls.
 *
 */
#define CTRL_SCROLLWND            ("scrollwnd")

/** Default container window procedure 
 * \sa DefaultDialogProc 
 */
MG_EXPORT int GUIAPI DefaultContainerProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

/** Scrollview item object, use this handle to access a scrollview item */
typedef GHANDLE HSVITEM;

/**
 * Structure which defines a container.
 */
typedef struct _CONTAINERINFO
{
#if 0
    DWORD       dwStyle;                /** style of the container */
    DWORD       dwExStyle;              /** extended style of the container */
    int         x, y, w, h;             /** position and size of the container */
#endif
    /** User-defined window procedure of the container */
    WNDPROC     user_proc;              
    /** Number of controls */
    int         controlnr;              
    /** Pointer to control array */
    PCTRLDATA   controls;               
    /** Additional data */
    DWORD       dwAddData;              
} CONTAINERINFO;
/** Data type of pointer to a CONTAINERINFO */
typedef CONTAINERINFO* PCONTAINERINFO;

/** 
 * \fn DWORD mglist_get_item_adddata (GHANDLE hi)
 * \brief Use this to get additional data from a list item in a control, such as
 *        scrollview and iconview.
 */
MG_EXPORT DWORD mglist_get_item_adddata (GHANDLE hi);

/** 
 * \fn DWORD scrollview_get_item_adddata (HSVITEM hsvi)
 * \brief Use this to get additional data from scrollview item 
 */
MG_EXPORT DWORD scrollview_get_item_adddata (HSVITEM hsvi);

/**
 * \fn int scrollview_get_item_index (HWND hWnd, HSVITEM hsvi);
 * \brief To get item index
 */ 
MG_EXPORT int scrollview_get_item_index (HWND hWnd, HSVITEM hsvi);

/**
 * \fn int scrollview_is_item_hilight (HWND hWnd, HSVITEM hsvi)
 * \brief To decide whether an item is the current hilighted item
 */
MG_EXPORT BOOL scrollview_is_item_hilight (HWND hWnd, HSVITEM hsvi);

/**
 * \fn int scrollview_is_item_selected (HSVITEM hsvi)
 * \brief To decide whether an item is a selected
 */
MG_EXPORT BOOL scrollview_is_item_selected (HSVITEM hsvi);

/**
 * \fn int scrollview_set_item_height (HWND hWnd, HSVITEM hsvi, int height)
 * \brief Sets the height of an item
 */
MG_EXPORT int scrollview_set_item_height (HWND hWnd, HSVITEM hsvi, int height);

/**
 * \var typedef void (* SVITEM_INITFUNC)(HWND hWnd, HSVITEM hsvi)
 * \brief Type of the scrollview item initialization callback procedure.
 *
 * \param hWnd Handle of the scrollview control .
 * \param hsvi Scrollview item handle.
 *
 * \return Zero on success; otherwise -1.
 */
typedef int  (*SVITEM_INITFUNC)    (HWND hWnd, HSVITEM hsvi);

/**
 * \var typedef void (* SVITEM_DESTROYFUNC)(HWND hWnd, HSVITEM hsvi)
 * \brief Type of the scrollview item destroy callback procedure.
 *
 * \param hWnd Handle of the scrollview control .
 * \param hsvi Scrollview item handle.
 */
typedef void (*SVITEM_DESTROYFUNC) (HWND hWnd, HSVITEM hsvi);

/**
 * \var typedef void (*SVITEM_DRAWFUNC)(HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
 * \brief Type of the scrollview item drawing callback procedure.
 *
 * \param hWnd Handle of the scrollview control.
 * \param hsvi Scrollview item handle.
 * \param hdc  Drawing device context.
 * \param rcDraw Drawing rect area.
 */
typedef void (*SVITEM_DRAWFUNC) (HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw);

/**
 * \var typedef int (*SVITEM_CMP) (HSVITEM hsvi1, HSVITEM hsvi2)
 * \brief Type of the scrollview item compare function
 */
typedef int (*SVITEM_CMP) (HSVITEM hsvi1, HSVITEM hsvi2);

/** Structure of item operations */
typedef struct _svitem_operations
{
    /** Called when an scrollview item is created */
    SVITEM_INITFUNC     initItem;     
    /** Called when an item is destroied */
    SVITEM_DESTROYFUNC  destroyItem;  
    /** Call this to draw an item */
    SVITEM_DRAWFUNC     drawItem;     
} SVITEMOPS;
/** Data type of pointer to a SVITEMOPS */
typedef SVITEMOPS* PSVITEMOPS;

/** Structure of the scrollview item info */
typedef struct _SCROLLVIEWITEMINFO
{
    /** Index of item */
    int        nItem;           
    /** Height of an item */
    int        nItemHeight;     
    /** Item additional data */
    DWORD      addData;         
} SVITEMINFO;
/** Data type of pointer to a SVITEMINFO */
typedef SVITEMINFO* PSVITEMINFO;

    /**
     * \defgroup ctrl_scrollview_styles Styles of scrollview control
     * @{
     */

/**
 * \def SVS_UPNOTIFY
 * \brief Sends the notification messages to parent window when the keys is up. 
 */
#define SVS_UPNOTIFY            0x0001L

/**
 * \def SVS_NOTIFY
 * \brief reserved.
 */
#define SVS_NOTIFY              0x0002L

/**
 * \def SVS_AUTOSORT
 * \brief Automatically sorts strings entered in the scrollview control. 
 */
#define SVS_AUTOSORT            0x0004L

/**
 * \def SVS_LOOP
 * \brief Loops the item automatically when user select item with down or up key.
 */
#define SVS_LOOP                0x0008L

    /** @} end of ctrl_scrollview_styles */

    /**
     * \defgroup ctrl_scrollview_msgs Messages of scrollview control
     * @{
     */

/**
 * \def SVM_ADDITEM
 * \brief Adds an item in the scrollview.
 *
 * \code
 * SVM_ADDITEM
 * SVITEMINFO svii;
 * HSVITEM *phsvi;
 *
 * wParam = (WPARAM)phsvi;
 * lParam = (LPARAM)&svii;
 * \endcode
 *
 * \param &svii Pointer to a scrollview item information structure.
 * \param phsvi Pointer to a HSVITEM var, used to store the item handle
 *              returned.
 *
 * \return Index of the scrollview item on success; otherwise -1.
 */
#define SVM_ADDITEM             0xF300

/**
 * \def SVM_DELITEM
 * \brief Deletes an item from the scrollview.
 *
 * \code
 * SVM_DELITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem Scrollview item index to delete.
 *              If hsvi is not zero, nItem will be ignored.
 * \param hsvi Scrollview item handle to delete.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_DELITEM             0xF301

/**
 * \def SVM_SETITEMDRAW
 * \brief Sets the drawing operation of an item.
 *
 * Scrollview item drawing function will be called when doing with MSG_PAINT message,
 * scrollview window should define this function if it want to draw an customed item.
 *
 * \code
 * SVM_SETITEMDRAW
 * SVITEM_DRAWFUNC pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn Scrollview item drawing function.
 *
 * \return Old drawing function pointer; otherwise 0.
 */
#define SVM_SETITEMDRAW         0xF302

/**
 * \def SVM_ADDCTRLS
 * \brief Adds controls to the scrollview.
 *
 * \code
 * SVM_ADDCTRLS
 * int itemNr;
 * PCTRLDATA pctrls;
 *
 * wParam = (WPARAM)itemNr;
 * lParam = (LPARAM)pctrls;
 * \endcode
 *
 * \param ctrlNr Control number in the pctrls control array.
 * \param pctrls Points to a CTRLDATA array that defines controls.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_ADDCTRLS            0xF303

/**
 * \def SVM_SETCONTWIDTH
 * \brief Sets the scrollview content area (scrollable area) width.
 *
 * Scrollable area of a scrolled window is always larger than the visible area.
 *
 * \code
 * SVM_SETCONTWIDTH
 * int cont_w;
 *
 * wParam = (WPARAM)cont_w;
 * lParam = 0;
 * \endcode
 *
 * \param cont_w Scrollview content width.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETCONTWIDTH        0xF306

/**
 * \def SVM_SETCONTHEIGHT
 * \brief Sets the scrollview content area (scrollable area) height.
 *
 * Scrollable area of a scrolled window is always larger than the visible area.
 *
 * \code
 * SVM_SETCONTHEIGHT
 * int cont_h;
 *
 * wParam = (WPARAM)cont_h;
 * lParam = 0;
 * \endcode
 *
 * \param cont_h Scrollview content height.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETCONTHEIGHT       0xF307

/**
 * \def SVM_GETCTRL
 * \brief Gets the control handle in the scrollview window by control id.
 *
 * \code
 * SVM_GETCTRL
 * int id;
 *
 * wParam = (WPARAM)id;
 * lParam = 0;
 * \endcode
 *
 * \param id Control id.
 *
 * \return Control window handle on success; otherwise 0.
 */
#define SVM_GETCTRL             0xF308

/**
 * \def SVM_RESETCONTENT
 * \brief Clears all the controls and the items added to the scrollview window.
 *
 * \code
 * SVM_RESETCONTENT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_RESETCONTENT        0xF309

/**
 * \def SVM_SETITEMOPS
 * \brief Sets the item operations of the items in the scrollview.
 *
 * Normally item operations should be set before adding items.
 *
 * \code
 * SVM_SETITEMOPS
 * SVITEMOPS *iop;
 *
 * wParam = 0;
 * lParam = (LPARAM)iop;
 * \endcode
 *
 * \param iop Points to a SVITEMOPS structure that defines item operations
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETITEMOPS          0xF30a

/**
 * \def SVM_GETMARGINS
 * \brief Gets the margin values of the scrollview.
 *
 * Application should use a RECT structure to get left, top, right, and bottom margins.
 *
 * \code
 * SVM_GETMARGINS
 * RECT rcMargin;
 *
 * wParam = 0;
 * lParam = (LPARAM)&rcMargin;
 * \endcode
 *
 * \param rcMargin A RECT for storing 4 margin values.
 *
 * \return 0 on success.
 */
#define SVM_GETMARGINS            0xF30b

/**
 * \def SVM_SETMARGINS
 * \brief Sets the margin values of the scrollview.
 *
 * Application should use a RECT structure to give left, top, right, and bottom margins.
 * If you want to change a margin value, give a value large than zero, or else -1.
 *
 * \code
 * SVM_SETMARGINS
 * RECT *rcMargin;
 *
 * wParam = 0;
 * lParam = (LPARAM)rcMargin;
 * \endcode
 *
 * \param rcMargin A RECT Containing 4 margin values.
 *
 * \return 0 on success.
 */
#define SVM_SETMARGINS            0xF311

/**
 * \def SVM_GETLEFTMARGIN
 * \brief Gets the left margin value of the scrollview.
 *
 * \code
 * SVM_GETLEFTMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Left margin value on success, otherwise -1.
 */
#define SVM_GETLEFTMARGIN         0xF312

/**
 * \def SVM_GETTOPMARGIN
 * \brief Gets the top margin value of the scrollview.
 *
 * \code
 * SVM_GETTOPMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Top margin value on success, otherwise -1.
 */
#define SVM_GETTOPMARGIN          0xF313

/**
 * \def SVM_GETRIGHTMARGIN
 * \brief Gets the right margin value of the scrollview.
 *
 * \code
 * SVM_GETRIGHTMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Right margin value on success, otherwise -1.
 */
#define SVM_GETRIGHTMARGIN        0xF314

/**
 * \def SVM_GETBOTTOMMARGIN
 * \brief Gets the bottom margin value of the scrollview.
 *
 * \code
 * SVM_GETBOTTOMMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Bottom margin value on success, otherwise -1.
 */
#define SVM_GETBOTTOMMARGIN       0xF315

/**
 * \def SVM_GETVISIBLEWIDTH
 * \brief Gets the width of the visible content area.
 *
 * \code
 * SVM_GETVISIBLEWIDTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Width of the visible content area on success, otherwise -1.
 */
#define SVM_GETVISIBLEWIDTH       0xF316

/**
 * \def SVM_GETVISIBLEHEIGHT
 * \brief Gets the height of the visible content area.
 *
 * \code
 * SVM_GETVISIBLEHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height of the visible content area on success, otherwise -1.
 */
#define SVM_GETVISIBLEHEIGHT      0xF317

/**
 * \def SVM_GETCONTWIDTH
 * \brief Gets the width of the content area.
 *
 * \code
 * SVM_GETCONTWIDTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Width of the content area on success, otherwise -1.
 */
#define SVM_GETCONTWIDTH          0xF318

/**
 * \def SVM_GETCONTHEIGHT
 * \brief Gets the height of the content area.
 *
 * \code
 * SVM_GETCONTHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height of the content area on success, otherwise -1.
 */
#define SVM_GETCONTHEIGHT         0xF319

/**
 * \def SVM_SETCONTRANGE
 * \brief Sets the width and height of the content area.
 *
 * If you want to change width and height, give a value large than zero.
 * If you give -1, this value will remain untouched.
 *
 * \code
 * SVM_SETCONTRANGE
 * int cont_w, cont_h;
 *
 * wParam = (WPARAM)cont_w;
 * lParam = (LPARAM)cont_h;
 * \endcode
 *
 * \param cont_w Width of the content area.
 * \param cont_h Height of the content area.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SETCONTRANGE          0xF31a

/**
 * \def SVM_GETCONTENTX
 * \brief Gets the content x offset in the viewport.
 *
 * \code
 * SVM_GETCONTENTX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Content x coordinate in the viewport on success, otherwise -1.
 */
#define SVM_GETCONTENTX           0xF31b

/**
 * \def SVM_GETCONTENTY
 * \brief Gets the content y offset in the viewport.
 *
 * \code
 * SVM_GETCONTENTY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Content y coordinate in the viewport on success, otherwise -1.
 */
#define SVM_GETCONTENTY           0xF31c

/**
 * \def SVM_SETCONTPOS
 * \brief Sets the content offset position in the viewport.
 *
 * \code
 * SVM_SETCONTPOS
 * int cont_x, cont_y;
 *
 * wParam = (WPARAM)cont_x;
 * lParam = (LPARAM)cont_y;
 * \endcode
 *
 * \param cont_x New content x offset
 * \param cont_y New content y offset
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SETCONTPOS            0xF31d

/**
 * \def SVM_GETCURSEL
 * \brief Gets the index of the current hilighted scrollview item.
 *
 * An application sends an this message to a scrollview window to get the index of 
 * the currently selected item, if there is one, in a single-selection scrollview.
 * For multiple-selection case, appliction send this message to a scrollview
 * to get the index of the current highlighted item.
 *
 * \code
 * SVM_GETCURSEL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Index of current hilighted item.
 */
#define SVM_GETCURSEL             0xF31e

/**
 * \def SVM_SELECTITEM
 * \brief Selects or unselects an item in the scrollview item.
 *
 * \code
 * SVM_SELECTITEM
 * int nItem;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)bSel;
 * \endcode
 *
 * \param nItem Index of the item to select or unselect.
 * \param bSel To select or unselect.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SELECTITEM            0xF31f

/**
 * \def SVM_SHOWITEM
 * \brief Makes an item visible in the scrollview item.
 *
 * \code
 * SVM_SHOWITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem Item index. Be useful when hsvi is zero.
 * \param hsvi Item handle.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SHOWITEM              0xF320

/**
 * \def SVM_CHOOSEITEM
 * \brief Makes an item selected and visible in the scrollview item.
 *
 * \code
 * SVM_CHOOSEITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem Item index. Be useful when hsvi is zero.
 * \param hsvi Item handle.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_CHOOSEITEM            0xF321

/**
 * \def SVM_SETCURSEL
 * \brief Makes an item as the current hilighted item in the scrollview item.
 *
 * \code
 * SVM_SETCURSEL
 * int nItem;
 * BOOL bVisible;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)bVisible;
 * \endcode
 *
 * \param nitem Item index.
 * \param bVisible If bVisible is TRUE, this item wille be made visible.
 *                 
 * \return old hilighted item index on success, otherwise -1.
 */
#define SVM_SETCURSEL             0xF322

/**
 * \def SVM_SETITEMINIT
 * \brief Sets the init operation of the items in the scrollview.
 *
 * Normally item operations should be set before adding items.
 * The initialization callback function will be called when adding an item.
 *
 * \code
 * SVM_SETITEMINIT
 * SVITEM_INITFUNC *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn Item init operation function
 *
 * \return Old function on success; otherwise NULL.
 */
#define SVM_SETITEMINIT            0xF323

/**
 * \def SVM_SETITEMDESTROY
 * \brief Sets the destroy operation of the items in the scrollview.
 *
 * Normally item operations should be set before adding items.
 * The destroy callback function will be called when deleting an item
 *
 * \code
 * SVM_SETITEMDESTROY
 * SVITEM_DESTROYFUNC *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn Item destroy operation function
 *
 * \return Old function on success; otherwise NULL.
 */
#define SVM_SETITEMDESTROY        0xF324

/**
 * \def SVM_SETITEMCMP
 * \brief Sets the item compare function.
 *
 * \code
 * SVM_SETITEMCMP
 * SVITEM_CMP *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn New item compare function
 *
 * \return Old function on success; otherwise NULL.
 */
#define SVM_SETITEMCMP            0xF327

/**
 * \def SVM_MAKEPOSVISIBLE
 * \brief Makes a position in the content area visible.
 *
 * \code
 * SVM_MAKEPOSVISIBLE
 * int pos_x, pos_y;
 *
 * wParam = (WPARAM)pos_x;
 * lParam = (LPARAM)pos_y;
 * \endcode
 *
 * \param pos_x X coordinate of the position to be made visible
 * \param pos_y Y coordinate of the position to be made visible
 */
#define SVM_MAKEPOSVISIBLE        0xF328

/**
 * \def SVM_SETCONTAINERPROC
 * \brief Sets the window procedure of the container window in the scrollview.
 *
 * \code
 * SVM_SETCONTAINERPROC
 * WNDPROC pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn New container procedure function.
 *
 * \return Old container procedure.
 */
#define SVM_SETCONTAINERPROC       0xF329

/**
 * \def SVM_GETFOCUSCHILD
 * \brief Gets the focus control in the scrollview.
 *
 * \code
 * SVM_GETFOCUSCHILD
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Handle of the focus control.
 */
#define SVM_GETFOCUSCHILD          0xF32a

/**
 * \def SVM_GETHSCROLLVAL
 * \brief Gets the horizontal scroll value.
 *
 * \code
 * SVM_GETHSCROLLVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current horizontal scroll value.
 */
#define SVM_GETHSCROLLVAL           0xF32b

/**
 * \def SVM_GETVSCROLLVAL
 * \brief Gets the vertical scroll value.
 *
 * \code
 * SVM_GETVSCROLLVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current vertical scroll value.
 */
#define SVM_GETVSCROLLVAL           0xF32c

/**
 * \def SVM_GETHSCROLLPAGEVAL
 * \brief Gets the horizontal page scroll value.
 *
 * \code
 * SVM_GETHSCROLLPAGEVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current horizontal page scroll value.
 */
#define SVM_GETHSCROLLPAGEVAL       0xF32d

/**
 * \def SVM_GETVSCROLLPAGEVAL
 * \brief Gets the vertical page scroll value.
 *
 * \code
 * SVM_GETVSCROLLPAGEVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current vertical page scroll value.
 */
#define SVM_GETVSCROLLPAGEVAL       0xF32e

/**
 * \def SVM_SETSCROLLVAL
 * \brief Sets the horizontal and vertical scroll value.
 *
 * \code
 * SVM_SETSCROLLVAL
 * int hval, vval;
 *
 * wParam = (WPARAM)hval;
 * lParam = (LPARAM)vval;
 * \endcode
 *
 * \param hval New horizontal scroll value.
 * \param vval New vertical scroll value.
 *
 * \return Zero.
 */
#define SVM_SETSCROLLVAL           0xF32f

/**
 * \def SVM_SETSCROLLPAGEVAL
 * \brief Sets the horizontal and vertical page value.
 *
 * \code
 * SVM_SETSCROLLPAGEVAL
 * int hval, vval;
 *
 * wParam = (WPARAM)hval;
 * lParam = (WPARAM)vval;
 * \endcode
 *
 * \param hval New horizontal page value.
 * \param vval New vertical page value.
 *
 * \return Zero.
 */
#define SVM_SETSCROLLPAGEVAL       0xF330

/**
 * \def SVM_SORTITEMS
 * \brief Sorts the items according to a specified comparision function.
 *
 * \code
 * SVM_SORTITEMS
 * SVITEM_CMP pfn;
 *
 * lParam = pfn;
 * \endcode
 *
 * \param pfn Comparision function.
 *
 * \return Zero.
 */
#define SVM_SORTITEMS              0xF331

/**
 * \def SVM_GETITEMCOUNT
 * \brief Gets the total number of the items.
 *
 * \return Number of the items.
 */
#define SVM_GETITEMCOUNT           0xF332

/**
 * \def SVM_GETITEMADDDATA
 * \brief Gets the additional data of the item
 *
 * \code
 * SVM_GETITEMADDDATA
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem Scrollview item index to access.
 *              If hsvi is not zero, nItem will be ignored.
 * \param hsvi Scrollview item handle to access.
 *
 * \return Item additional data.
 */
#define SVM_GETITEMADDDATA         0xF333

/**
 * \def SVM_SETITEMADDDATA
 * \brief Sets the additional data of the item
 *
 * \code
 * SVM_SETITEMADDDATA
 * int nitem;
 * int addData;
 *
 * wParam = (WPARAM)nitem;
 * lParam = (LPARAM)addData;
 * \endcode
 *
 * \param nitem Item index.
 * \param addData Additional data
 *
 * \return Old item additional data.
 */
#define SVM_SETITEMADDDATA         0xF334

/**
 * \def SVM_REFRESHITEM
 * \brief Refresh the item
 *
 * \code
 * SVM_REFRESHITEM
 * int nitem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nitem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nitem Item index. If hsvi is not zero, nItem will be ignored.
 * \param hsvi Item handle.
 *
 * \return Returns 0.
 */
#define SVM_REFRESHITEM            0xF335

/**
 * \def SVM_SETITEMHEIGHT
 * \brief Sets the height of an item
 *
 * \code
 * SVM_SETITEMHEIGHT
 * int nitem;
 * int height;
 *
 * wParam = (WPARAM)nitem;
 * lParam = (LPARAM)height;
 * \endcode
 *
 * \param nitem Item index.
 * \param height New height of the item
 *
 * \return Old item height.
 */
#define SVM_SETITEMHEIGHT         0xF336

/**
 * \def SVM_GETFIRSTVISIBLEITEM
 * \brief Gets the index of the first visible item
 *
 * \code
 * SVM_GETFIRSTVISIBLEITEM
 * \endcode
 *
 * \return Item index.
 */
#define SVM_GETFIRSTVISIBLEITEM   0xF337


    /** @} end of ctrl_scrollview_msgs */

    /**
     * \defgroup ctrl_scrollview_ncs Notification codes of scrollview control
     * @{
     */

/** Indicates clicking on the item */
#define SVN_CLICKED             1

/** Indicates the hilighted item changed */
#define SVN_SELCHANGED          2

/** Indicates the hilighted item changing */
#define SVN_SELCHANGING			4

    /** @} end of ctrl_scrollview_ncs */

    /** @} end of ctrl_scrollview */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCROLLVIEW_H */

