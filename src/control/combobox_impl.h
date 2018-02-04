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
** combobox.h: the head file of ComboBox control.
**
** Originally by Wang Jian.
** Create date: 2001/08/22
*/

#ifndef GUI_COMBOBOX_IMPL_H_
#define GUI_COMBOBOX_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif


#ifdef _FLAT_WINDOW_STYLE
#define WIDTH_EDIT_BORDER       1
#else
#define WIDTH_EDIT_BORDER       2
#endif

#define MARGIN_EDIT_LEFT        1
#define MARGIN_EDIT_TOP         1
#define MARGIN_EDIT_RIGHT       2
#define MARGIN_EDIT_BOTTOM      1


typedef struct  {
    WORD    wStateFlags;    /* combobox state flags */
    int     bExtended;      /* extended UI flag */
    BOOL    bRedraw;        /* MiD - redraw flag, draw only if it's 1 */
    HWND    EditControl;    /* edit/static control hWnd */
    HWND    ListBoxControl; /* listbox control hWnd */
    const BITMAP* SpinBmp;  /* spin bitmap */
    RECT    IncSpinBox;     /* rect of inc spin (client) */
    RECT    DecSpinBox;     /* rect of dec spin (client) */
    RECT    ListBoxRect;    /* listbox rect (screen) */
    int     list_height;    /* height of listbox set by app */
    int     uHeight;        /* height of the normal state */
    int     nListItems;     /* ecw */
    int     spin_min;       /* minimal value of spin */
    int     spin_max;       /* maximal value of spin */
    int     spin_pace;      /* step value of spin */
    int     fastspin_pace;  /* step value of fast spin */
    char*   str_format;     /* string used to format spin value */
} COMBOBOXDATA;
typedef COMBOBOXDATA* PCOMBOBOXDATA;

#define CWD_LPCBDATA  0
#define CBC_EDITID    1

#define CSF_CAPTUREACTIVE   0x0001
#define CSF_LOCALBUTTONDOWN 0x0002
#define CSF_BUTTONDOWN      0x0004
#define CSF_BUTTONUP        0x0008
#define CSF_FOCUS           0x0010
#define CSF_HASDROPPED      0x0020

#define SET_STATE(lp, wMask)   (lp->wStateFlags |= (wMask))
#define CLEAR_STATE(lp, wMask) (lp->wStateFlags &= ~(wMask))
#define IS_SET(lp, wMask)      (lp->wStateFlags & (wMask))

#define BOWNERDRAW(l) ((l)->wStyle & (CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE))

BOOL RegisterComboBoxControl (void);
#ifdef __cplusplus
}
#endif

#endif // GUI_EDIT_IMPL_H_



