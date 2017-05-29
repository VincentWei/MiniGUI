/*
** $Id: tf-stb.h 2445 2003-09-04 03:40:35Z weiym $
**
** tf-stb.h:. the head file of Low Level Input Engine for Tongfang STB.
**
** Copyright (C) 2001, RedFlag Software.
**
** Created by Wei YongMing, 2001/08/24
*/

#ifndef GUI_IAL_TFSTB_H
    #define GUI_IAL_TFSTB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitTFSTBInput (INPUT* input, const char* mdev, const char* mtype);
void    TermTFSTBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_TFSTB_H */


