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
 ** mwidget.h: this file is to make same as mgncs
 **
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010-07-20 
 */

#ifndef __MGUI_WIDGET_H
#define __MGUI_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

#define NCSCLASSNAME(name)     name _MGNCS_CLASS_SUFFIX

DECLARE_OBJECT(mWidget)
#define mWidgetHeader(clsName)                  \
	mObjectHeader(clsName)                   \
	HWND hwnd;

#define mWidgetClassHeader(clsName, parentClass) \
	mObjectClassHeader(clsName, parentClass)                                     \
    DWORD dlgCode;                                                               \
    BOOL (*setProperty)(clsName*, int id, DWORD value);                             \
    DWORD (*getProperty)(clsName*, int id);                                         \
	int (*wndProc)(clsName* , int, WPARAM, LPARAM);                                 \
	BOOL (*onCreate)(clsName*, LPARAM);                                             \
	void (*onPaint)(clsName*, HDC, const PCLIPRGN pClip);                           \
	int (*onLButtonDown)(clsName*, int x, int y, DWORD keyFlags);                  \
	int (*onLButtonUp)(clsName*, int x, int y, DWORD keyFlags);                    \
	int (*onMouseMove)(clsName*, int x, int y, DWORD keyFlags);                    \
	int (*onKeyDown)(clsName*, int scancode, DWORD keyFlags);                      \
	int (*onKeyUp)(clsName*, int scancode, DWORD keyFlags);                        \
	int (*onSizeChanged)(clsName*, RECT* rtClient);                                 \
	BOOL (*onEraseBkgnd)(clsName*, HDC hdc, const RECT* invRect); 

DEFINE_OBJECT(mWidget, mObject)

#define BEGIN_CMPT_CLASS BEGIN_MINI_CLASS

#define END_CMPT_CLASS END_MINI_CLASS

#define ncsNotifyParent(self, nc)  NotifyParent(self->hwnd, GetDlgCtrlID(self->hwnd), nc)

#define SET_DLGCODE(code)  _class->dlgCode = (code);

#define NCSP_WIDGET_MAX 0

#ifdef __cplusplus
}
#endif


#endif

