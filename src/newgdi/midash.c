/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

/***********************************************************
Copyright 1987, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $TOG: midash.c /main/14 1998/02/09 14:46:34 kaleb $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"

#ifdef _MGHAVE_ADV_2DAPI

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

#include "mi.h"

static miDashPtr CheckDashStorage(miDashPtr* ppseg, int nseg, int* pnsegMax);

/* return a list of DashRec.  there will be an extra
entry at the end holding the last point of the polyline.
   this means that the code that actually draws dashes can
get a pair of points for every dash.  only the point in the last
dash record is useful; the other fields are not used.
   nseg is the number of segments, not the number of points.

example:

   dash1.start
   dash2.start
   dash3.start
   last-point

defines a list of segments
   (dash1.pt, dash2.pt)
   (dash2.pt, dash3.pt)
   (dash3.pt, last-point)
and nseg == 3.

NOTE:
    EVEN_DASH == ~ODD_DASH

NOTE ALSO:
    miDashLines may return 0 segments, going from pt[0] to pt[0] with one dash.
*/

enum { EVEN_DASH=0, ODD_DASH=1 };

#define sign(x) ((x)>0)?1:( ((x)<0)?-1:0 )

miDashPtr miDashLine(int npt, POINT* ppt, unsigned int nDash, unsigned char* pDash, unsigned int offset, int* pnseg)
{
    POINT pt1, pt2;
    int lenCur;                /* npt used from this dash */
    int lenMax;                /* npt in this dash */
    int iDash = 0;        /* index of current dash */
    int which;                /* EVEN_DASH or ODD_DASH */
    miDashPtr pseg;        /* list of dash segments */
    miDashPtr psegBase;        /* start of list */
    int nseg = 0;        /* number of dashes so far */
    int nsegMax = 0;        /* num segs we can fit in this list */

    int x, y, len;
    int adx, ady, signdx, signdy;
    int du, dv, e1, e2, e, base_e = 0;

    lenCur = offset;
    which = EVEN_DASH;
    while(lenCur >= pDash[iDash])
    {
        lenCur -= pDash[iDash];
        iDash++;
        if (iDash >= nDash)
            iDash = 0;
        which = ~which;
    }
    lenMax = pDash[iDash];

    psegBase = (miDashPtr)NULL;
    pt2 = ppt[0];                /* just in case there is only one point */

    while(--npt)
    {
        if (ppt[0].x == ppt[1].x && ppt[0].y == ppt[1].y)
        {
            ppt++;
            continue;                /* no duplicated points in polyline */
        }
        pt1 = *ppt++;
        pt2 = *ppt;

        adx = pt2.x - pt1.x;
        ady = pt2.y - pt1.y;
        signdx = sign(adx);
        signdy = sign(ady);
        adx = abs(adx);
        ady = abs(ady);

        if (adx > ady)
        {
            du = adx;
            dv = ady;
            len = adx;
        }
        else
        {
            du = ady;
            dv = adx;
            len = ady;
        }

        e1 = dv * 2;
        e2 = e1 - 2*du;
        e = e1 - du;
        x = pt1.x;
        y = pt1.y;

        nseg++;
        pseg = CheckDashStorage(&psegBase, nseg, &nsegMax);
        if (!pseg)
            return (miDashPtr)NULL;
        pseg->pt = pt1;
        pseg->e1 = e1;
        pseg->e2 = e2;
        base_e = pseg->e = e;
        pseg->which = which;
        pseg->newLine = 1;

        while (len--)
        {
            if (adx > ady)
            {
                /* X_AXIS */
                if (((signdx > 0) && (e < 0)) ||
                    ((signdx <=0) && (e <=0))
                   )
                {
                    e += e1;
                }
                else
                {
                    y += signdy;
                    e += e2;
                }
                x += signdx;
            }
            else
            {
                /* Y_AXIS */
                if (((signdx > 0) && (e < 0)) ||
                    ((signdx <=0) && (e <=0))
                   )
                {
                    e +=e1;
                }
                else
                {
                    x += signdx;
                    e += e2;
                }
                y += signdy;
            }

            lenCur++;
            if (lenCur >= lenMax && (len || npt <= 1))
            {
                nseg++;
                pseg = CheckDashStorage(&psegBase, nseg, &nsegMax);
                if (!pseg)
                    return (miDashPtr)NULL;
                pseg->pt.x = x;
                pseg->pt.y = y;
                pseg->e1 = e1;
                pseg->e2 = e2;
                pseg->e = e;
                which = ~which;
                pseg->which = which;
                pseg->newLine = 0;

                /* move on to next dash */
                iDash++;
                if (iDash >= nDash)
                    iDash = 0;
                lenMax = pDash[iDash];
                lenCur = 0;
            }
        } /* while len-- */
    } /* while --npt */

    if (lenCur == 0 && nseg != 0)
    {
        nseg--;
        which = ~which;
    }
    *pnseg = nseg;
    pseg = CheckDashStorage(&psegBase, nseg+1, &nsegMax);
    if (!pseg)
        return (miDashPtr)NULL;
    pseg->pt = pt2;
    pseg->e = base_e;
    pseg->which = which;
    pseg->newLine = 0;
    return psegBase;
} 


#define NSEGDELTA 16

/* returns a pointer to the pseg[nseg-1], growing the storage as
 * necessary.  this interface seems unnecessarily cumbersome.
 *
 * ppseg: base pointer
 * nseg: number of segment we want to write to
 * pnsegMax: size (in segments) of list so far
 */
static miDashPtr CheckDashStorage(miDashPtr* ppseg, int nseg, int* pnsegMax)
{
    if (nseg > *pnsegMax)
    {
        miDashPtr newppseg;

        *pnsegMax += NSEGDELTA;
        newppseg = (miDashPtr)realloc(*ppseg, (*pnsegMax)*sizeof(miDashRec));
        if (!newppseg)
        {
            free(*ppseg);
            return (miDashPtr)NULL;
        }
        *ppseg = newppseg;
    }
    return(*ppseg+(nseg-1));
}

 /*
  * dist: distance to step
  * pDashIndex: current dash
  * pDash: dash list
  * numInDashList: total length of dash list
  * pDashOffset: offset into current dash
  */
void miStepDash (int dist, int* pDashIndex, const unsigned char* pDash, int numInDashList, int* pDashOffset)
{
    int        dashIndex, dashOffset;
    int totallen;
    int        i;
    
    dashIndex = *pDashIndex;
    dashOffset = *pDashOffset;
    if (dist < pDash[dashIndex] - dashOffset)
    {
        *pDashOffset = dashOffset + dist;
        return;
    }
    dist -= pDash[dashIndex] - dashOffset;
    if (++dashIndex == numInDashList)
        dashIndex = 0;
    totallen = 0;
    for (i = 0; i < numInDashList; i++)
        totallen += pDash[i];
    if (totallen <= dist)
        dist = dist % totallen;
    while (dist >= pDash[dashIndex])
    {
        dist -= pDash[dashIndex];
        if (++dashIndex == numInDashList)
            dashIndex = 0;
    }
    *pDashIndex = dashIndex;
    *pDashOffset = dist;
}

#endif /* _MGHAVE_ADV_2DAPI */

