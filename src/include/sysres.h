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
#ifndef GUI_SYSRES_H
#define GUI_SYSRES_H

#ifndef CURSORSECTION
#define CURSORSECTION    "cursorinfo"
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define RegisterResFromRes(hdc, file)   \
        (LoadResource(file,RES_TYPE_IMAGE, NULL) != NULL)

/*Load system cursor */
#ifdef _MGHAVE_CURSOR
HCURSOR __mg_load_cursor_from_res (int i);

static inline PCURSOR sysres_load_system_cursor (int i) {
    HCURSOR hCursor;
    hCursor = __mg_load_cursor_from_res (i);
    return (PCURSOR)hCursor;
}
#else
static inline PCURSOR sysres_load_system_cursor (int i) {
    return ((PCURSOR)(NULL));
}
#endif /* _MGHAVE_CURSOR */

BOOL __sysres_init_inner_resource(void);
const char* __sysres_get_system_res_path(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_SYSRES_H */
