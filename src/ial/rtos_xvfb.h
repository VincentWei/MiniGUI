/*
** $Id$
**
** rtos_xvfb.h: head file of Input Engine for RTOS X Virtual FrameBuffer
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Created by Liu Peng, 2007/09/29
*/

#ifndef _IAL_RTOS_XVFB_H
#define _IAL_RTOS_XVFB_H

#include <semaphore.h>

typedef struct _XVFBKEYDATA
{
        unsigned short key_code;
        unsigned short key_state;
} XVFBKEYDATA;

typedef struct _XVFBMOUSEDATA
{
        unsigned short x;
        unsigned short y;
        unsigned int button;
} XVFBMOUSEDATA;

typedef struct _XVFBEVENT
{
        int event_type;
        union {
                XVFBKEYDATA key;
                XVFBMOUSEDATA mouse;
        } data;
} XVFBEVENT;


typedef struct _XVFBEVENTBUFFER {
        
        pthread_mutex_t lock;  /* lock */
        sem_t wait;            /* the semaphore for wait message */    
        
        XVFBEVENT *events;     /* buffer data area */
        int size;              /* buffer size */
        
        int readpos;           /* read position */
        int writepos;          /* write postion */

        BOOL buffer_released;
} XVFBEVENTBUFFER;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitRTOSXVFBInput (INPUT* input, const char* mdev, const char* mtype);
void TermRTOSXVFBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_RTOS_XVFB_H */

