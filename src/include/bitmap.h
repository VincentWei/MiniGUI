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
** readbmp.h: Low Level bitmap file read/save routines.
**
** Create date: 2001/xx/xx
*/

#ifndef GUI_GDI_BITMAP_H
    #define GUI_GDI_BITMAP_H

#include "endianrw.h"
#include "gal.h"
#include "cliprect.h"
#include "internals.h"
#include "dc.h"

struct _SCALER_INFO_FILLBMP
{
    PDC pdc;
    RECT dst_rc;
    BYTE* line_buff;
    BYTE* line_alpha_buff;
    const BITMAP* bmp;
};

typedef struct {
    struct _SCALER_INFO_FILLBMP scaler_info;
    GAL_Rect dst_rect;
    int old_bkmode;

    BYTE* decoded_buff;
    BYTE* decoded_alpha_buff;
    const BYTE* encoded_bits;
    const BYTE* encoded_alpha_mask;
} FILLINFO;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int bmp_ComputePitch (int bpp, Uint32 width, Uint32 *pitch, BOOL does_round);

PDC _begin_fill_bitmap (HDC hdc, int x, int y, int w, int h,
                const BITMAP* bmp, FILLINFO *fill_info);
void _fill_bitmap_scanline (PDC pdc, const BITMAP* bmp, 
                FILLINFO *fill_info, int y);
void _end_fill_bitmap (PDC pdc, const BITMAP* bmp, FILLINFO *fill_info);

MG_EXPORT int GUIAPI EncodeRleBitmap(HDC hdc, PBITMAP bmp);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_BITMAP_H

