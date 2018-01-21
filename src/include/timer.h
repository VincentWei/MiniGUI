/*
** $Id: timer.h 12262 2009-11-29 06:53:27Z weiym $
**
** tiemr.h: the head file of Timer module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/4/21
*/

#ifndef GUI_TIMER_H
    #define GUI_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define USEC_1S         1000000
#define USEC_10MS       10000

typedef struct _timer {
    HWND        hWnd;
    LINT        id;
    DWORD       speed;
    UINT_PTR    count;

    TIMERPROC   proc;
    UINT_PTR    tick_count;
    PMSGQUEUE   msg_queue;
} TIMER;
typedef TIMER* PTIMER;

BOOL mg_InitTimer (void);
void mg_TerminateTimer (void);
void mg_dispatch_timer_message (unsigned int inter);
void mg_remove_timers_by_msg_queue (const MSGQUEUE* msg_que);

TIMER* __mg_get_timer (int slot);
void __mg_remove_timer (TIMER* timer, int slot);

static inline HWND __mg_get_timer_hwnd (int slot)
{
    TIMER* ptimer = __mg_get_timer (slot);
    if (ptimer)
        return ptimer->hWnd;

    return HWND_NULL;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_TIMER_H */

