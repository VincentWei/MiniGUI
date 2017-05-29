/*
** $Id: mc68x328.h 7335 2007-08-16 03:38:27Z xgwang $
**
** mc68x328.h: the head file of Low Level Input Engine for 
**              uClinux touch screen palm/mc68ez328
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Wei Yongming, 2003/08/06
*/

#ifndef GUI_IAL_MC68X328_H
    #define GUI_IAL_MC68X328_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitMC68X328Input (INPUT* input, const char* mdev, const char* mtype);
void    TermMC68X328Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_MC68X328_H */


