/*
** $Id: svgalib.h 7335 2007-08-16 03:38:27Z xgwang $
**
** svgalib.h: the head file of Low Level Input Engine based on SVGALib
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Created by WEI Yongming, 2000/06/13
*/

#ifndef GUI_IAL_SVGALIB_H
    #define GUI_IAL_SVGALIB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitSVGALibInput (INPUT* input, const char* mdev, const char* mtype);
void    TermSVGALibInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_SVGALIB_H */


