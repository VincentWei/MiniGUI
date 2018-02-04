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
#include "common.h"
#include "minigui.h"
#include "gdi.h"

typedef struct {
        char* type;
        FONTOPS* ops;
}FONTOPS_INFO;

#ifdef _MGFONT_RBF
extern FONTOPS __mg_rbf_ops;
#endif

#ifdef _MGFONT_VBF
extern FONTOPS __mg_vbf_ops;
#endif

#ifdef _MGFONT_QPF
extern FONTOPS __mg_qpf_ops;
#endif

#ifdef _MGFONT_UPF
extern FONTOPS __mg_upf_ops;
#endif

#if defined _MGFONT_FT2 || defined _MGFONT_TTF
extern FONTOPS __mg_ttf_ops;
#endif

FONTOPS_INFO __mg_fontops_infos[] = {
#ifdef _MGFONT_RBF
    {FONT_TYPE_NAME_BITMAP_RAW, &__mg_rbf_ops},
#endif
#ifdef _MGFONT_VBF
    {FONT_TYPE_NAME_BITMAP_VAR, &__mg_vbf_ops},
#endif
#ifdef _MGFONT_QPF
    {FONT_TYPE_NAME_BITMAP_QPF, &__mg_qpf_ops},
#endif
#ifdef _MGFONT_UPF
    {FONT_TYPE_NAME_BITMAP_UPF, &__mg_upf_ops},
#endif
#if defined _MGFONT_FT2 || defined _MGFONT_TTF
    {FONT_TYPE_NAME_SCALE_TTF, &__mg_ttf_ops},
#endif
    {NULL, NULL},
};

