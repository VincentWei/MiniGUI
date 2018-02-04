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
** text.c: Implementation of general text API.
**
** Create date: 2000/4/19
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "drawtext.h"

int GUIAPI GetFontHeight (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    return pdc->pLogFont->size;
}

int GUIAPI GetMaxFontWidth (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    DEVFONT* sbc_devfont = pdc->pLogFont->sbc_devfont;
    DEVFONT* mbc_devfont = pdc->pLogFont->mbc_devfont;
    int sbc_max_width = (*sbc_devfont->font_ops->get_max_width) 
            (pdc->pLogFont, sbc_devfont);
    int mbc_max_width = 0;

    if (mbc_devfont)
        mbc_max_width = (*mbc_devfont->font_ops->get_max_width) 
                (pdc->pLogFont, mbc_devfont);
    
    return (sbc_max_width > mbc_max_width) ? sbc_max_width : mbc_max_width;
}

void GUIAPI GetLastTextOutPos (HDC hdc, POINT* pt)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    *pt = pdc->CurTextPos;
}

