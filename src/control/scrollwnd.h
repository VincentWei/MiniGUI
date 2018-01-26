/*
** $Id: scrollwnd.h 8944 2007-12-29 08:29:16Z xwyan $ 
**
** scrollwnd.h: header file of ScrollView control.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
*/

#ifndef _SCROLLWND_H
#define _SCROLLWND_H


#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define CTRL_CONTAINER   "container"


/* default size and position values */
#define HSCROLL                 5   // h scroll value
#define VSCROLL                 15  // v scroll value

typedef struct _containerdata
{
    /* scroll view window */
    HWND hSV;
    /* container flags, status ... */
    DWORD flags;

} CONTDATA;
typedef CONTDATA* PCONTDATA;

typedef struct _swdata
{
    SCRDATA scrdata;

    /* handle of static container */
    HWND hContainer;
    /* private data of container */
    PCONTDATA pContdata;

    /* scroll view flags, status ... */
    DWORD flags;

} SWDATA;
typedef SWDATA *PSWDATA;

int     scrollwnd_init (HWND hWnd, PSWDATA psv);
void    scrollwnd_destroy (PSWDATA pswdata);
void    scrollwnd_hscroll (HWND hWnd, PSWDATA pswdata, WPARAM wParam, LPARAM lParam);
void    scrollwnd_vscroll (HWND hWnd, PSWDATA pswdata, WPARAM wParam, LPARAM lParam);
void    scrollwnd_reset_content (HWND hWnd, PSWDATA pswdata);
int     scrollwnd_SetContWidth (HWND hWnd, PSWDATA pswdata, int cont_w);
void    scrollwnd_set_cont_pos (HWND hWnd, PSWDATA pswdata, int cont_x, int cont_y);
void    scrollwnd_make_pos_visible (HWND hWnd, PSWDATA pswdata, int pos_x, int pos_y);
void    scrollwnd_refresh_container (HWND hWnd, PSWDATA pswdata);
void    scrollwnd_set_scrollval (PSWDATA pswdata, int hval, int vval);
void    scrollwnd_set_scrollpageval (PSWDATA pswdata, int hval, int vval);


static inline int scrollwnd_get_contwidth (PSWDATA pswdata)
{
    return pswdata->scrdata.nContWidth;
}

static inline WNDPROC scrollwnd_set_container_proc (PSWDATA pswdata, WNDPROC newproc)
{
    return SetWindowCallbackProc (pswdata->hContainer, newproc);
}

static inline HWND scrollwnd_get_window (HWND hcontainer)
{
    return GetParent (GetParent (hcontainer));
}

static inline void scrollwnd_refresh_content (PSWDATA pswdata)
{
    InvalidateRect (pswdata->hContainer, NULL, TRUE);
}


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SCROLLWND_H */

