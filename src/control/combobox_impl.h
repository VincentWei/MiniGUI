/*
** $Id: combobox_impl.h 8944 2007-12-29 08:29:16Z xwyan $
**
** combobox.h: the head file of ComboBox control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001, 2002 Wei Yongming.
**
** NOTE: Originally by Wang Jian.
**
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



