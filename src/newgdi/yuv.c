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
/*
** yuv.c: YUV overlay support.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/11/06
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"

/****************************** YUV overlay support **************************/
/* Create a YUV overlay */
GAL_Overlay* GUIAPI CreateYUVOverlay (int width, int height, Uint32 format, HDC hdc)
{
    PDC pdc = dc_HDC2PDC (hdc);

    return GAL_CreateYUVOverlay (width, height, format, pdc->surface);
}

/* Display a YUV overlay */
void GUIAPI DisplayYUVOverlay (GAL_Overlay* overlay, const RECT* dstrect)
{
    GAL_Rect dst;
    PDC pdc;
    RECT rcOutput = {0};

    rcOutput.right = WIDTHOFPHYGC;
    rcOutput.bottom = HEIGHTOFPHYGC;

    if (!(pdc = __mg_check_ecrgn (HDC_SCREEN)))
        return;

    if (dstrect) {
        rcOutput = *dstrect;
        dst.x = dstrect->left;
        dst.y = dstrect->top;
        dst.w = RECTWP(dstrect);
        dst.h = RECTHP(dstrect);
    }
    else {
        dst.x = 0;
        dst.y = 0;
        dst.w = WIDTHOFPHYGC;
        dst.h = HEIGHTOFPHYGC;
    }

    pdc->rc_output = rcOutput;
    ENTER_DRAWING (pdc);
    GAL_DisplayYUVOverlay (overlay, &dst);
    LEAVE_DRAWING (pdc);
    UNLOCK_GCRINFO (pdc);
}

