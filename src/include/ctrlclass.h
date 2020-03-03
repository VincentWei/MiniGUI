///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** ctrlclass.h: the head file of Control Class module.
**
** Create date: 1999/5/21
*/

#ifndef GUI_CTRLCLASS_H
    #define GUI_CTRLCLASS_H

#define MAXLEN_CLASSNAME    31

typedef struct _CTRLCLASSINFO
{
    char      name[MAXLEN_CLASSNAME + 1];   // class name

    /* common properties of this class */
    DWORD     dwStyle;          // Default control styles.
    DWORD     dwExStyle;        // Default control extended styles.

    HCURSOR   hCursor;          // control cursor

#ifdef _MGSCHEMA_COMPOSITING
    DWORD     dwBkColor;        // control background color.
#else
    // gal_pixel iFgColor;      // control foreground color; removed since 5.0.0.
    gal_pixel iBkColor;         // control background color (pixel value)
#endif

    WNDPROC ControlProc;        // window procedure for this control class.

    DWORD dwAddData;            // the additional data.

    int nUseCount;              // use count.

    struct _CTRLCLASSINFO*  next;   // next class info
} CTRLCLASSINFO;
typedef CTRLCLASSINFO* PCTRLCLASSINFO;

typedef struct _CONTROL
{
    /*
     * These following fields are similiar with MAINWIN struct.
     */

    /*
     * Fields for data type
     * VM[2020-02-14]: Move these fields back to header.
     * VM[2018-01-18]: Move these fields from header to here to compatible with
     *      WINDOWINFO
     */
    unsigned char DataType; // the data type
    unsigned char WinType;  // the window type
    unsigned short Flags;   // speical runtime flags, such EraseBkGnd flags

    /*
     * Common fields for both control, virtual window, and main window.
     * VM[2020-02-14]: Move these fields to here to support virtual window.
     */
    PCTRLCLASSINFO pcci;    // pointer to Control Class Info struct.

    char* spCaption;        // the caption of control.
    LINT  id;               // the identifier of child window.

    WNDPROC ControlProc;    // the window procedure of this control.
    NOTIFPROC NotifProc;    // the notification callback procedure.

    DWORD dwAddData;        // the additional data.
    DWORD dwAddData2;       // the second addtional data.

    map_t* mapLocalData;    // the map of local data.

    /*
     * The following members are only implemented for control.
     */
    PMAINWIN pMainWin;      // the main window that contains this control.

    struct _CONTROL* next;  // the next sibling control.
    struct _CONTROL* prev;  // the prev sibling control.
    MASKRECT * mask_rects;  // mask rectangles for the control.

    /*
     * Fields for appearance of this control.
     */
    int left, top;          // the position of control in main window's
    int right, bottom;      // client area.

    int cl, ct;             // the positio of control client in main window's
    int cr, cb;             // client area.

    DWORD dwStyle;          // the styles of child window.
    DWORD dwExStyle;        // the extended styles of child window.

    int idx_znode;          // the z-node; only works for control as main window.
    //gal_pixel iFgColor;   // control foreground color (pixel value).
    gal_pixel iBkColor;     // control background color (pixel value)

    HMENU hMenu;            // handle of menu.
    HACCEL hAccel;          // handle of accelerator table.
    HCURSOR hCursor;        // handle of cursor.
    HICON hIcon;            // handle of icon.
    HMENU hSysMenu;         // handle of system menu.
    PLOGFONT pLogFont;      // pointer to logical font.

    LFSCROLLBARINFO vscroll;   // the vertical scroll bar information.
    LFSCROLLBARINFO hscroll;   // the horizital scroll bar information.

    /*
     * Fields for window element data.
     */
    WINDOW_ELEMENT_RENDERER* we_rdr;    // the window renderer
    struct _wnd_element_data* wed;      // the data of window elements

    HDC   privCDC;          // the private client DC.
    INVRGN InvRgn;          // the invalid region of this control.
#ifdef _MGSCHEMA_COMPOSITING
    struct GAL_Surface* surf;  // the shared surface of the main window.
#else
    PGCRINFO pGCRInfo;      // pointer to global clip region info struct.
#endif

    PCARETINFO pCaretInfo;  // pointer to system caret info struct.

    /*
     * Fields to manage the relationship among main windows and controls.
     */
    struct _CONTROL* pParent;   // the parent of this control.


    struct _CONTROL* children;            // the first child control.
    struct _CONTROL* active;              // the active child control.
    struct _CONTROL* old_under_pointer;   // the old child under pointer.
    struct _CONTROL* primitive;           // the primitive child of mouse event.

    /*
     * The following members are only implemented for control.
     */
    /*
     * if a control is has WS_EX_CTRLASMAINWIN,
     * this field is the next control as main window
     * MainWindow->hFirstChildAsMainWin --->
     *   control1->next_ctrl_as_main ---->
     *   control2->next_ctrl_as_main ---->
     *   .....
     *
     * Control have to list, one for browsers, the other for as main cotrol list:
     *
     *  hFirstChild :
     *     control1->next-->
     *     control2->next-->
     *     control3->next-->
     *     ....
     *
     *  hFirstChildAsMainWin:
     *      control1->next_ctrl_as_main -->
     *      control2-> .....
     *
     * THE TWO LIST USE A SAME CONTROL OBJECT,
     * So, When Destroy a Control: YOU MUST REMOVE
     * ITSELF FROM PARENT'S hFirstChildAsMainWin'S LIST
     *
     */
    struct _CONTROL* next_ctrl_as_main;
} CONTROL;
typedef CONTROL* PCONTROL;

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* check whether hWnd is controland return pointer to control hWnd. */
static inline PCONTROL gui_Control (HWND hWnd)
{
    if (hWnd && hWnd != HWND_INVALID &&
            ((PCONTROL)hWnd)->WinType == TYPE_CONTROL)
        return (PCONTROL)hWnd;

    return NULL;
}

BOOL mg_InitControlClass (void);
void mg_TerminateControlClass (void);

PCTRLCLASSINFO gui_GetControlClassInfo (const char* szClassName);
int gui_ControlClassDataOp (int Operation, PWNDCLASS pWndClass);

int gui_DeleteControlClass (const char* szClassName);
void gui_EmptyControlClassInfoTable (void);

int gui_AddNewControlClass (PWNDCLASS pWndClass);
BOOL gui_SetWindowExStyle (HWND hWnd, DWORD dwExStyle);

#ifdef _DEBUG_CTRL
void dbg_DumpCtrlClassInfoTable (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CTRLCLASS_H */

