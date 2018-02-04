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
** midc.h: dc operations for advanced 2D graphics APIs.
*/

#ifndef MIDC_H
#define MIDC_H 1

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void _dc_fill_spans (PDC pdc, Span* spans, int nspans, BOOL sorted);
void _dc_fill_spans_brush (PDC pdc, Span* spans, int nspans, BOOL sorted);

static inline void _dc_fill_rect (PDC pdc, int x, int y, int w, int h)
{
    GAL_Rect rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    pdc->cur_pixel = pdc->pencolor;
    pdc->cur_ban = NULL;
    pdc->step = 1;

    _dc_fillbox_clip (pdc, &rect);
}

void _dc_dump_spans (Span* spans, int nspans);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
