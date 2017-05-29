/*
** $Id: cisco_touchpad.h  7249 2010-03-24 03:38:27Z czzhao $
**
** cisco_touchpad.h: the head file of Low Level Input Engine for 
**             Cisco touchpad 
**
** Copyright (C) 2007 ~ 2011 Feynman Software.
**
** Created by Zhao Chengzhang, 2010/03/24
*/

#ifndef _IAL_CISCO_TOUCHPAD_H
#define _IAL_CISCO_TOUCHPAD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define CMD_SET_MODE     1
#define MICE_MODE        0
#define APP_MODE         1

BOOL    InitCiscoTouchpadInput (INPUT* input, const char* mdev, const char* mtype);
void    TermCiscoTouchpadInput (void);
void    mg_ial_ioctl(unsigned int cmd, unsigned int value);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _IAL_CISCO_TOUCHPAD_H*/
