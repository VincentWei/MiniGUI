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
** ctrlclass.h: the head file of Control Class module.
**
** Create date: 1999/5/21
*/

#ifndef GUI_CTRLCLASS_H
    #define GUI_CTRLCLASS_H

#define MAXLEN_CLASSNAME    31

typedef struct _CTRLCLASSINFO
{
    char      name[MAXLEN_CLASSNAME + 1];
                                // class name
    /*
     * common properties of this class
     */
    DWORD     dwStyle;          // Default control styles.
    DWORD     dwExStyle;        // Default control extended styles.

    HCURSOR   hCursor;          // control cursor

    gal_pixel iFgColor;         // control foreground color.
    gal_pixel iBkColor;         // control background color.
    
    LRESULT (*ControlProc)(HWND, UINT, WPARAM, LPARAM);
                                // control procedure.

    DWORD dwAddData;            // the additional data.

    int nUseCount;              // use count.
    struct _CTRLCLASSINFO*  next;
                                // next class info
}CTRLCLASSINFO;
typedef CTRLCLASSINFO* PCTRLCLASSINFO;

typedef struct tagCONTROL
{
    /*
     * These fields are similiar with MAINWIN struct.
     */
    int left, top;          // the position of control in main window's
    int right, bottom;      // client area.

    int cl, ct;             // the positio of control client in main window's
    int cr, cb;             // client area.

    DWORD dwStyle;          // the styles of child window.
    DWORD dwExStyle;        // the extended styles of child window.

    gal_pixel iFgColor;     // control foreground color.
    gal_pixel iBkColor;		// control background color.
    HMENU hMenu;		    // handle of menu.
    HACCEL hAccel;          // handle of accelerator table.
    HCURSOR hCursor;	    // handle of cursor.
    HICON hIcon;		    // handle of icon.
    HMENU hSysMenu;         // handle of system menu.
    PLOGFONT pLogFont;      // pointer to logical font.

    char* spCaption;         // the caption of control.
    LINT  id;                // the identifier of child window.

    LFSCROLLBARINFO vscroll;   // the vertical scroll bar information.
    LFSCROLLBARINFO hscroll;   // the horizital scroll bar information.

    /** the window renderer */
    WINDOW_ELEMENT_RENDERER* we_rdr;

    HDC   privCDC;          // the private client DC.
    INVRGN InvRgn;          // the invalid region of this control.
    PGCRINFO pGCRInfo;      // pointer to global clip region info struct.

    // the Z order node, 
    // only for control with WS_EX_CTRLASMAINWIN.
    int idx_znode;

    PCARETINFO pCaretInfo;  // pointer to system caret info struct.

    DWORD dwAddData;        // the additional data.
    DWORD dwAddData2;       // the second addtional data.

    LRESULT (*ControlProc) (HWND, UINT, WPARAM, LPARAM);

    PMAINWIN pMainWin;       // the main window that contains this control.

    struct tagCONTROL* pParent;// the parent of this control.

    /*
     * Child windows.
     */
    struct tagCONTROL* children;
                             // the first child control.
    struct tagCONTROL* active;
                             // the active child control.
    struct tagCONTROL* old_under_pointer;
                             // the old under pointer child control.
    struct tagCONTROL* primitive;
                             // the primitive child of mouse event.

    NOTIFPROC notif_proc;    // the notification callback procedure.

    /*
     * window element data.
     */
    struct _wnd_element_data* wed;
    
    /*
     * some internal fields
     * VM[2018-01-18]: Move these fields from header to here to compatible with WINDOWINFO
     */
    unsigned char DataType;         // the data type
    unsigned char WinType;          // the window type
	unsigned short Flags;           // speical runtime flags, such EraseBkGnd flags

    /*
     * The following members are only implemented for control.
     */
    struct tagCONTROL* next;   // the next sibling control.
    struct tagCONTROL* prev;   // the prev sibling control.

    PCTRLCLASSINFO pcci;     // pointer to Control Class Info struct.
    MASKRECT * mask_rects;

	//if a control is has WS_EX_CTRLASMAINWIN, this proc is the next control as main window
	/*
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
	 *  	control1->next_ctrl_as_main -->
	 *  	control2-> .....
	 *
	 * THE TWO LIST USE A SAME CONTROL OBJECT, 
	 * So, When Destroy a Control: YOU MUST REMOVE 
	 * ITSELF FROM PARENT'S hFirstChildAsMainWin'S LIST
	 *
	 */
	struct tagCONTROL* next_ctrl_as_main;	

}CONTROL;
typedef CONTROL* PCONTROL;

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL mg_InitControlClass (void);
void mg_TerminateControlClass (void);

PCTRLCLASSINFO gui_GetControlClassInfo (const char* szClassName);
int gui_ControlClassDataOp (int Operation, PWNDCLASS pWndClass);

MG_EXPORT int AddNewControlClass (PWNDCLASS pWndClass);
MG_EXPORT BOOL SetWindowExStyle (HWND hWnd, DWORD dwExStyle);
int gui_DeleteControlClass (const char* szClassName);
void gui_EmptyControlClassInfoTable (void);
PCONTROL gui_Control (HWND hWnd);   // check whether hWnd is control
                                // and return pointer to control hWnd.

#ifdef _DEBUG
void DumpCtrlClassInfoTable (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CTRLCLASS_H */

