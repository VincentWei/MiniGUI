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
/*! ============================================================================
 * @file lf_common.c 
 * @Synopsis Common functions for Look&Feel render. 
 * @version 1.0
 * @date 2009-12-25
 */

#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "cursor.h"
#include "internals.h"
#include "inline.h"
#include "memops.h"
#include "ctrlclass.h"
#include "dc.h"

char* gui_GetIconFile(const char* rdr_name, char* file, char* _szValue)
{
    char szValue[MAX_NAME + 1];
    char *iconname;

    strcpy(szValue, "icon/");
    iconname = szValue + strlen(szValue);

    if (GetMgEtcValue (rdr_name, file,
                iconname, sizeof(szValue)-(iconname-szValue)) < 0 ) {
        _MG_PRINTF ("SYSRES: can't get %s's value from section %s in etc.\n",
                file, rdr_name);
        return NULL;
    }
    strcpy(_szValue, szValue);
    return _szValue;
}

BOOL gui_LoadIconRes(HDC hdc, const char* rdr_name, char* file) 
{
    char szValue[MAX_NAME + 1];

    gui_GetIconFile(rdr_name, file, szValue);

    if (LoadResource(szValue, RES_TYPE_ICON, (DWORD)hdc) != NULL)
        return TRUE;

    _MG_PRINTF ("SYSRES: can't get %s's value from section %s in etc.\n",
            file, rdr_name);

    return FALSE;
}

/* DK: [12/23/09] #Bug4289
 * Fill target DC area with a part of resource BITMAP as much as possible, 
 * except the resource BITMAP is not comatible with target DC, in the
 * later case, the BITMAP will converted to DC for do StretchBlt.*/
BOOL gui_fill_box_with_bitmap_part_except_incompatible (HDC hdc,
    int x, int y, int w, int h, int bw, int bh, const BITMAP * 	bmp, int xo, int yo)
{
    if (NULL == bmp) {
        return FALSE;
    }

    if (IsCompatibleDC(HDC_SCREEN, hdc))
    {
        return FillBoxWithBitmapPart(hdc, x, y, w, h, bw, bh, bmp, xo, yo);
    }
    else
    {
        HDC bmpToDC = CreateMemDCFromBitmap(HDC_SCREEN, bmp);
        if (HDC_INVALID != bmpToDC)
        {
            int w_source = 0 == bw ? bmp->bmWidth : bw;
            int h_source = 0 == bh ? bmp->bmHeight : bh;

            if (w_source != bmp->bmWidth || h_source != bmp->bmHeight)
            {
                ConvertMemDC(bmpToDC, hdc, MEMDC_FLAG_SWSURFACE);
                StretchBlt(bmpToDC, xo, yo, w, h, 
                    hdc, x, y, w, h, 0);
            }
            else
            {
                BitBlt(bmpToDC, xo, yo, w, h, 
                    hdc, x, y, 0);
            }
            DeleteMemDC(bmpToDC);
        }
    }
    return TRUE;
}

/* humingming: [2010.8.4] #Bug4860
 * if the bitmap is not compatible with the out dc,
 * convert it to memdc, than find the colorkey postion and 
 * reset the colorkey with the postion after convert.
 * last use FillBoxWithBitmap, because it deal with colorkey and alpha. */
BOOL gui_fill_box_with_bitmap_except_incompatible (HDC hdc,
    int x, int y, int w, int h, const BITMAP *  bmp)
{
    if (NULL == bmp) {
        return FALSE;
    }

    if (!IsCompatibleDC(HDC_SCREEN, hdc)) {
        BOOL ret;
        HDC bmpToDC;
        BITMAP bmpTmp;
        PDC pdc = dc_HDC2PDC(hdc);

        /* copy original bmp information */
        memcpy(&bmpTmp, bmp, sizeof(BITMAP));

        bmpToDC = CreateMemDCFromBitmap(HDC_SCREEN, bmp);
        ret = ConvertMemDC(bmpToDC, hdc, MEMDC_FLAG_SWSURFACE);
        if (FALSE == ret) {
            DeleteMemDC(bmpToDC);
            return ret;
        }

        /* find the colorkey pixel position */
        if (bmpTmp.bmType & BMP_TYPE_COLORKEY) {
            int i, j;
            BOOL isFind = FALSE;

            for (i = 0; i < bmp->bmHeight; i++) {
                for (j = 0; j < bmp->bmWidth; j++) {
                    if (bmp->bmColorKey == GetPixelInBitmap(bmp, j, i)) {
                        bmpTmp.bmColorKey = GetPixel(bmpToDC, j, i);
                        isFind = TRUE;
                        break;
                    }
                }

                if (TRUE == isFind)
                    break;
            }
        }

        bmpTmp.bmBits = dc_HDC2PDC(bmpToDC)->surface->pixels;
        bmpTmp.bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
        bmpTmp.bmBytesPerPixel = pdc->surface->format->BytesPerPixel;
        GAL_GetBoxSize(pdc->surface, bmpTmp.bmWidth, bmpTmp.bmHeight, &(bmpTmp.bmPitch));

        ret = FillBoxWithBitmap(hdc, x, y, w, h, &bmpTmp);
        DeleteMemDC(bmpToDC);

        return ret;
    }
    else {
        return FillBoxWithBitmap(hdc, x, y, w, h, bmp);
    }
}
