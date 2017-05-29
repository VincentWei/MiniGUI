/*
** $Id: embest2410.h 7335 2007-08-16 03:38:27Z xgwang $
**
** embest2410.h:. the head file of Low Level Input Engine for EMBEST ARM2410 
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Created by Cheng Jiangang
*/

#ifndef GUI_IAL_EMBEST2410_H
    #define GUI_IAL_EMBEST2410_H

#define KEY_RELEASED    0x80 

#define KEYPAD_0        0x0B
#define KEYPAD_1        0x02
#define KEYPAD_2        0x0A
#define KEYPAD_3        0x12
#define KEYPAD_4        0x01
#define KEYPAD_5        0x09
#define KEYPAD_6        0x11
#define KEYPAD_7        0x00
#define KEYPAD_8        0x08
#define KEYPAD_9        0x10
#define KEYPAD_PERIOD      0x19
#define KEYPAD_DIVIDE      0x0C
#define KEYPAD_MULTIPLY    0x14
#define KEYPAD_MINUS       0x15
#define KEYPAD_PLUS        0x18     
#define KEYPAD_ENTER       0x1A
#define KEYPAD_NUMLOCK     0x04

#define MAX_KEYPAD_CODE     0x1C

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitEMBEST2410Input (INPUT* input, const char* mdev, const char* mtype);
void    TermEMBEST2410Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_IAL_EMBEST2410_H */
