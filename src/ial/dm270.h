/*
** $Id: dm270.h 7335 2007-08-16 03:38:27Z xgwang $
**
** dm270.h:. the head file of Low Level Input Engine for dm270
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Yao Yunyuan, 2004/07/23
*/

#ifndef GUI_IAL_DM270_H
    #define GUI_IAL_DM270_H

#define KEY_RELEASED    0x80 

#define KEYPAD_UP       0x01
#define KEYPAD_DOWN     0x02
#define KEYPAD_LEFT     0x04
#define KEYPAD_RIGHT    0x08
#define KEYPAD_OK       0x10
#define KEYPAD_CANCEL   0x20

#define MAX_KEYPAD_CODE     0x21

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDM270Input (INPUT* input, const char* mdev, const char* mtype);
void    TermDM270Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_IAL_DM270_H */
