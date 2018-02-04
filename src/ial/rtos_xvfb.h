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
** rtos_xvfb.h: head file of Input Engine for RTOS X Virtual FrameBuffer
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

