/*
** $Id: mi.h 7359 2007-08-16 05:08:40Z xgwang $
**
** mi.h: low level advanced two dimensions graphics interfaces.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
*/

#ifndef MI_H
#define MI_H 1

#define PixmapBytePad(w, d) (w)
#define BitmapBytePad(w) (w)

#define SCRRIGHT(x, n) ((x)>>(n))

#include "mistruct.h"
#include "mifpoly.h"
#include "mifillarc.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

miDashPtr miDashLine (int npt, POINT* ppt, unsigned int nDash, unsigned char* pDash, unsigned int offset, int* pnseg);
void miStepDash (int dist, int *pDashIndex, const unsigned char *pDash, int numInDashList, int *pDashOffset);

void miWideDash (PDC pdc, register int npt, register POINT *pPts);
void miWideLine (PDC pdc, register int npt, register POINT *pPts);

void miPolyArc (PDC pdc, int narcs, miArc *parcs);
void miPolyFillArc (PDC pdc, int narcs, miArc *parcs);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

