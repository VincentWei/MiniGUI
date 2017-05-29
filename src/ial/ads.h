/*
** $Id: ads.h 2445 2003-09-04 03:40:35Z weiym $
**
** ads.h:. the head file of Low Level Input Engine for ADS Graphics Client.
**
** Copyright (C) 2000, Wei YongMing 
** Copyright (C) 2000, BluePoint Software.
**
** Created by Wei YongMing, 2000/09/24
*/

#ifndef GUI_IAL_ADS_H
    #define GUI_IAL_ADS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitADSInput (INPUT* input, const char* mdev, const char* mtype);
void    TermADSInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_ADS_H */


