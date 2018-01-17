/*
** $Id: element.h 10734 2008-08-21 07:58:37Z weiym $
**
** element.h: the head file of window element data.
** 
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Create date: 2004/05/10
*/

#ifndef GUI_ELEMENT_H
    #define GUI_ELEMENT_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* window element data struct */
typedef struct _wnd_element_data
{
    list_t      list;       /* list pointer */
    UINT        id;         /* the item type */
    DWORD       data;       /* the data of the item */
} WND_ELEMENT_DATA;

#define WED_OK              0
#define WED_NEW_DATA        1
#define WED_NOT_CHANGED     2

#define WED_INVARG          -1
#define WED_NODEFINED       -2
#define WED_NOTFOUND        -3
#define WED_MEMERR          -4

extern int free_window_element_data (HWND hwnd);
extern int append_window_element_data (WND_ELEMENT_DATA** wed, Uint32 id, DWORD data);
extern int set_window_element_data (HWND hwnd, Uint32 id, DWORD new_data, DWORD* old_data);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_ELEMENT_H */


