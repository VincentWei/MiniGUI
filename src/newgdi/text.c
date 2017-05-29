/*
** $Id: text.c 9892 2008-03-20 02:50:02Z xwyan $
**
** text.c: Implementation of general text API.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
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

