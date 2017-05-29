/*
** $Id: vr4181.h 7335 2007-08-16 03:38:27Z xgwang $
**
** vr4181.h:. the head file of Low Level Input Engine for NEC VR4181 debug board.
**
** Copyright (C) 2001 ~ 2007 RedFlag Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_VR4181_H
    #define GUI_IAL_VR4181_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitVR4181Input (INPUT* input, const char* mdev, const char* mtype);
void TermVR4181Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_VR4181_H */


