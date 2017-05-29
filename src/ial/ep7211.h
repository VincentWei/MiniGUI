/*
** $Id: ep7211.h 2445 2003-09-04 03:40:35Z weiym $
**
** ep7211.h:. the head file of Low Level Input Engine for EP7211
**
** Copyright (C) 2000, Wei YongMing 
** Copyright (C) 2000, BluePoint Software.
**
** Created by Wei YongMing, 2000/09/24
*/

#ifndef GUI_IAL_EP7211_H
    #define GUI_IAL_EP7211_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitEP7211Input (INPUT* input, const char* mdev, const char* mtype);
void    TermEP7211Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_EP7211_H */


