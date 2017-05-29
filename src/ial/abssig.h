/*
** $Id: abssig.h 7335 2007-08-16 03:38:27Z xgwang $
**
** abssig.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_ABSSIG_H
    #define GUI_IAL_ABSSIG_H

/* bit 7 = state bits0-3 = key number */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <asm/dsp.h>
#include "abs_signal_driver.h"

BOOL    InitABSSIGInput (INPUT* input, const char* mdev, const char* mtype);
void    TermABSSIGInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_ABSSIG_H */


