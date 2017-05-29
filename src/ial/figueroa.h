/*
 ** $Id: figueroa.h 7114 2007-03-05 01:46:42Z lbjiao $
 **
 ** figueroa.h:. the head file of Low Level Input Engine for FiguerOA
 **
 ** Copyright (C) 2003 ~ 2007 Feynman Software.
 **
 ** Created by Libo Jiao, 2007/03/04
 */

#ifndef GUI_IAL_FIGUEROA_H
    #define GUI_IAL_FIGUEROA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    BOOL    InitFiguerOAInput (INPUT* input, const char* mdev, const char* mtype); 
    void    TermFiguerOAInput (void); 

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_FiguerOA_H */

