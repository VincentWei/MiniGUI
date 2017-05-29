/*
** $Id: ucb1x00.h 7335 2007-08-16 03:38:27Z xgwang $
**
** Copyright (C) 2003 ~ 2007, Feynman Software.
**
** ucb1x00.h: Head file for IAL Engine for UCB1x00 board.
** 
** Author: Zhong Shuyi (2003/12/28)
*/

#ifndef GUI_IAL_UCB1X00_H
    #define GUI_IAL_UCB1X00_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitUCB1X00Input (INPUT* input, const char* mdev, const char* mtype);
void    TermUCB1X00Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_UCB1X00_H */


