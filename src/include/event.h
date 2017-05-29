/*
** $Id: event.h 11102 2008-10-23 01:58:25Z tangjianbin $
**
** event.h: the head file of low level event handle.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/11
*/

#ifndef GUI_EVENT_H
    #define GUI_EVENT_H

typedef struct _MOUSEEVENT {
    int event;
    int x;
    int y;
    DWORD status;
}MOUSEEVENT;
typedef MOUSEEVENT* PMOUSEEVENT;

// consts for mouse event.
#define ME_MOVED                0x0000
#define ME_LEFTMASK             0x000F
#define ME_LEFTDOWN             0x0001
#define ME_LEFTUP               0x0002
#define ME_LEFTDBLCLICK         0x0003
#define ME_RIGHTMASK            0x00F0
#define ME_RIGHTDOWN            0x0010
#define ME_RIGHTUP              0x0020
#define ME_RIGHTDBLCLICK        0x0030
#define ME_MIDDLEMASK           0x0F00
#define ME_MIDDLEDOWN           0x0100
#define ME_MIDDLEUP             0x0200
#define ME_MIDDLEDBLCLICK       0x0300
#define ME_REPEATED             0xF000

typedef struct _KEYEVENT {
    int event;
    int scancode;
    DWORD status;
}KEYEVENT;
typedef KEYEVENT* PKEYEVENT;

#define KE_KEYMASK              0x000F
#define KE_KEYDOWN              0x0001
#define KE_KEYUP                0x0002
#define KE_KEYLONGPRESS         0x0004
#define KE_KEYALWAYSPRESS       0x0008
#define KE_SYSKEYMASK           0x00F0
#define KE_SYSKEYDOWN           0x0010
#define KE_SYSKEYUP             0x0020

typedef union _LWEVENTDATA {
    MOUSEEVENT me;
    KEYEVENT ke;
}LWEVENTDATA;

typedef struct _LWEVENT
{
    int type;
    int count;
    DWORD status;
    LWEVENTDATA data;
}LWEVENT;
typedef LWEVENT* PLWEVENT;

// Low level event type.
#define LWETYPE_TIMEOUT                 0
#define LWETYPE_KEY                     1
#define LWETYPE_MOUSE                   2

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

// The entrance of low level event handling thread
BOOL mg_InitLWEvent (void);
void mg_TerminateLWEvent (void);

// Low level event support
BOOL kernel_GetLWEvent (int event, PLWEVENT lwe);

// Mouse event parameters.
#define MOUSEPARA                           "mouse"
#define MOUSEPARA_DBLCLICKTIME              "dblclicktime"
#define DEF_MSEC_DBLCLICK                   300

#define EVENTPARA                           "event"
#define EVENTPARA_REPEATUSEC                "repeatusec"
#define EVENTPARA_TIMEOUTUSEC               "timeoutusec"
#define DEF_USEC_TIMEOUT                    300000
#define DEF_REPEAT_TIME                     50000

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_EVENT_H


