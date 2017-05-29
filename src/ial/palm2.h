/*
** $Id: palm2.h 7335 2007-08-16 03:38:27Z xgwang $
**
** palm2.h: the head file of Low Level Input Engine for OKWAP Palm2. 
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Wei YongMing, 2004/01/29
*/

#ifndef _IAL_PALMII_H
    #define _IAL_PALMII_H

/* bit 7 = state bits0-3 = key number */
#define KEY_RELEASED    0x80
#define KEY_NUM	        0x0f
#define MAX_KEY_EVENTS  4

#define H3600_SCANCODE_RECORD   59
#define H3600_SCANCODE_CALENDAR 60
#define H3600_SCANCODE_CONTACTS 61
#define H3600_SCANCODE_Q        62
#define H3600_SCANCODE_START    63
#define H3600_SCANCODE_UP       103
#define H3600_SCANCODE_RIGHT    106
#define H3600_SCANCODE_LEFT     105
#define H3600_SCANCODE_DOWN     108
#define H3600_SCANCODE_ACTION   64
#define H3600_SCANCODE_SUSPEND  65

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitPALMIIInput (INPUT* input, const char* mdev, const char* mtype);
void TermPALMIIInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _IAL_PALMII_H */


