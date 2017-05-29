/*
 ** $Id: mwidget.h 13050 2010-07-26 08:18:49Z dongjunjie $
 **
 ** mwidget.h: this file is to make same as mgncs
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
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

