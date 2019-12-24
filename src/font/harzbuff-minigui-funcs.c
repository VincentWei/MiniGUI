///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

/*
** harzbuff-minigui-funcs.c:
**
**  Functions for using HarfBuzz with the MiniGUI to provide Unicode data.
**
** Create by WEI Yongming at 2019/03/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#if defined(_MGCHARSET_UNICODE) && defined(_MGCOMPLEX_SCRIPTS)

#include "minigui.h"
#include "gdi.h"

#include <hb.h>

static hb_unicode_combining_class_t
hb_minigui_unicode_combining_class (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      unicode,
        void               *user_data)

{
    return (hb_unicode_combining_class_t) UCharCombiningClass (unicode);
}

static hb_unicode_general_category_t
hb_minigui_unicode_general_category (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      unicode,
        void               *user_data)
{
    /* hb_unicode_general_category_t and GUnicodeType are identical */
    return (hb_unicode_general_category_t) UCharGetCategory (unicode);
}

static hb_codepoint_t
hb_minigui_unicode_mirroring (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      unicode,
        void               *user_data)
{
    UCharGetMirror (unicode, &unicode);
    return unicode;
}

static hb_script_t
hb_minigui_unicode_script (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      unicode,
        void               *user_data)
{
    return ScriptTypeToISO15924 (UCharGetScriptType(unicode));
}

static hb_bool_t
hb_minigui_unicode_compose (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      a,
        hb_codepoint_t      b,
        hb_codepoint_t     *ab,
        void               *user_data)
{
    return UCharCompose (a, b, ab);
}

static hb_bool_t
hb_minigui_unicode_decompose (hb_unicode_funcs_t *ufuncs,
        hb_codepoint_t      ab,
        hb_codepoint_t     *a,
        hb_codepoint_t     *b,
        void               *user_data)
{
    return UCharDecompose(ab, a, b);
}

static hb_unicode_funcs_t *_funcs;
static hb_unicode_funcs_t *get_unicode_funcs(void)
{
    if (_funcs)
        return _funcs;

    _funcs = hb_unicode_funcs_create (NULL);

    hb_unicode_funcs_set_combining_class_func (_funcs,
            hb_minigui_unicode_combining_class, NULL, NULL);
    hb_unicode_funcs_set_general_category_func (_funcs,
            hb_minigui_unicode_general_category, NULL, NULL);
    hb_unicode_funcs_set_mirroring_func (_funcs,
            hb_minigui_unicode_mirroring, NULL, NULL);
    hb_unicode_funcs_set_script_func (_funcs,
            hb_minigui_unicode_script, NULL, NULL);
    hb_unicode_funcs_set_compose_func (_funcs,
            hb_minigui_unicode_compose, NULL, NULL);
    hb_unicode_funcs_set_decompose_func (_funcs,
            hb_minigui_unicode_decompose, NULL, NULL);

    hb_unicode_funcs_make_immutable (_funcs);

    return _funcs;
}

typedef hb_unicode_funcs_t *(*hb_get_unicode_funcs) (void);
extern hb_get_unicode_funcs __hb_extern_get_unicode_funcs;

void __mg_init_harzbuff_funcs(void)
{
    _DBG_PRINTF("%s: called\n", __FUNCTION__);
    __hb_extern_get_unicode_funcs = get_unicode_funcs;
}

void __mg_term_harzbuff_funcs(void)
{
    _DBG_PRINTF("%s: called\n", __FUNCTION__);

    if (_funcs) {
        hb_unicode_funcs_destroy(_funcs);
    }
    else {
        _ERR_PRINTF("%s: hb_unicode_funcs_t object is NULL\n",
            __FUNCTION__);
    }
}

#endif /* defined(_MGCHARSET_UNICODE) && defined(_MGCOMPLEX_SCRIPTS) */

