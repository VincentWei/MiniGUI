/*
** $Id: ads7846.h 7335 2007-08-16 03:38:27Z xgwang $
**
** ads7846.h:. the head file of Low Level Input Engine for Arca Tpanel Ads7846.
**
** Copyright (C) 2004 ~ 2007, Feynman Software.
**
** Created by PengKe, 2004/05/17
*/

#ifndef GUI_IAL_ADS7846_H
    #define GUI_IAL_ADS7846_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define CMD_SET_MINXY         1
#define CMD_SET_MAXXY         2
#define CMD_GET_VAL           3
#define CMD_SET_CAL_ENABLE    4
#define CMD_SET_CAL_DISABLE   5
#define CMD_SET_LCDXY         6
#define CMD_SET_HOLD_DELAY    7
 
#define TPANEL_DATA_IS_PENDOWN      0x0001
#define TPANEL_DATA_IS_PENUP        0x0002
#define TPANEL_DATA_IS_DOWNHOLD     0x0004
#define TPANEL_DATA_IS_UNAVAILABLE  0x0008



typedef struct {
    unsigned int status;
    unsigned int x;
    unsigned int y;
} POS;

BOOL InitAds7846Input (INPUT* input, const char* mdev, const char* mtype);
void TermAds7846Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_ADS7846_H */


