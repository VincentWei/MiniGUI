/*
** $Id: propsheet_impl.h 8944 2007-12-29 08:29:16Z xwyan $
**
** propsheet.h: the head file of PropSheet control.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming and others.
**
** NOTE: Originally by Wang Jian and Jiang Jun.
**
** Create date: 2001/11/19
*/

#ifndef GUI_PROPSHEET_IMPL_H_
#define GUI_PROPSHEET_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct tagPROPPAGE
{
    char* title;                /* title of page */
    HICON icon;                 /* icon of page */
    int  width;                 /* width of title */
    HWND hwnd;                  /* container control */
    WNDPROC proc;               /* callback of page */

    struct tagPROPPAGE* next;   /* next page */
} PROPPAGE;
typedef PROPPAGE* PPROPPAGE;

typedef struct tagPROPSHEETDATA
{
    RECT     head_rc;            /* height of page title */
    int      head_width;         /* the effective width of head */
    int      page_count;         /* the number of pages */
    PROPPAGE *active;            /* index of active page */
    PROPPAGE *first_display_page;/* index of active page */
    PROPPAGE *head;              /* head of page list */
    BOOL     overload;           /* if too much head and 
                                    too long for display, 
                                    head_overload = TRUE */
} PROPSHEETDATA;
typedef PROPSHEETDATA* PPROPSHEETDATA;

BOOL RegisterPropSheetControl (void);

#ifdef __cplusplus
}
#endif

#endif // GUI_PROPSHEET_IMPL_H_


