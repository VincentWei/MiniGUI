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
#ifndef _SYSRES_H
#define _SYSRES_H

#ifndef CURSORSECTION
#define CURSORSECTION    "cursorinfo"
#endif

//BOOL GUIAPI RegisterResFromRes (HDC hdc, const char* filename);
#define RegisterResFromRes(hdc, file)  (LoadResource(file,RES_TYPE_IMAGE, NULL) != NULL)


/*Load system cursor */
#ifdef _MGHAVE_CURSOR
PCURSOR sysres_load_system_cursor (int i);
#else
#define sysres_load_system_cursor(i)  ((PCURSOR)(NULL))
#endif

BOOL sysres_init_inner_resource(void);

const char* sysres_get_system_res_path(void);


#endif /* _SYSRES_H */
