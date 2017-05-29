/*
 ** $Id: mwidget.c 13674 2010-12-06 06:45:01Z wanzheng $
 **
 ** mwidget.c: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010-07-20 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

#if defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR)
#include "object.h"
#include "mwidget.h"

static void mWidget_construct(mWidget* self, DWORD addData)
{
    Class(mObject).construct((mObject*)self, 0);
    self->hwnd = (HWND) addData;
}

static int mWidget_wndProc(mWidget* self, int message, WPARAM wParam, LPARAM lParam)
{
   switch(message)
   {
   case MSG_CREATE:
       if(!_c(self)->onCreate(self, lParam))
           return -1;
       break;
    case MSG_PAINT:
       {
            HDC hdc = BeginPaint(self->hwnd);
            _c(self)->onPaint(self, hdc, (const PCLIPRGN)lParam);
            EndPaint(self->hwnd, hdc);
            return 0;
       }
    case MSG_KEYDOWN:
       if(_c(self)->onKeyDown(self, wParam, lParam))
           break;
       return 0;
    case MSG_KEYUP:
       if(_c(self)->onKeyUp(self, wParam, lParam))
           break;
       return 0;
    case MSG_LBUTTONDOWN:
       if(_c(self)->onLButtonDown(self, LOSWORD(lParam), HISWORD(lParam), wParam))
           break;
       return 0;
    case MSG_LBUTTONUP:
       if(_c(self)->onLButtonUp(self, LOSWORD(lParam), HISWORD(lParam), wParam))
           break;
       return 0;
   case MSG_MOUSEMOVE:
       if(_c(self)->onMouseMove(self, LOSWORD(lParam), HISWORD(lParam), wParam))
           break;
       return 0;
    case MSG_SIZECHANGED:
       return _c(self)->onSizeChanged(self, (RECT*)lParam);
    case MSG_ERASEBKGND:
       return _c(self)->onEraseBkgnd(self, (HDC)wParam, (const RECT*)lParam);
	case MSG_GETDLGCODE:
			return _c(self)->dlgCode;
   }

   return DefaultControlProc(self->hwnd, message, wParam, lParam);
}

static BOOL mWidget_setProperty(mWidget* self, int id, DWORD value)
{
    return 0;
}

static DWORD mWidget_getProperty(mWidget* self, int id)
{
    return 0;
}

static BOOL mWidget_onCreate(mWidget* self, LPARAM lParam)
{
    return TRUE;
}

static void mWidget_onPaint(mWidget* self, HDC hdc, const PCLIPRGN pclip)
{
    //do nothing
}

static int mWidget_onLButtonDown(mWidget* self, int x, int y, DWORD keyFlags)
{
    return 0;
}

static int mWidget_onLButtonUp(mWidget* self, int x, int y, DWORD keyFlags)
{
    return 0;
}

static int mWidget_onMouseMove(mWidget* self, int x, int y, DWORD keyFlags)
{
    return 0;
}

static int mWidget_onKeyDown(mWidget* self, int scancode, DWORD keyFlags)
{
    return 0;
}

static int mWidget_onKeyUp(mWidget* self, int scancode, DWORD keyFlags)
{
    return 0;
}

static int mWidget_onSizeChanged(mWidget* self, RECT* rtClient)
{
    return 0;
}

static BOOL mWidget_onEraseBkgnd(mWidget* self, HDC hdc, const RECT* invRect)
{
    DefaultControlProc(self->hwnd, MSG_ERASEBKGND, (WPARAM)hdc, (LPARAM)invRect);
    return TRUE;
}


BEGIN_CMPT_CLASS(mWidget, mObject)
    CLASS_METHOD_MAP(mWidget, construct)
    CLASS_METHOD_MAP(mWidget, wndProc)
    CLASS_METHOD_MAP(mWidget, setProperty)
    CLASS_METHOD_MAP(mWidget, getProperty)
    CLASS_METHOD_MAP(mWidget, onCreate)
    CLASS_METHOD_MAP(mWidget, onPaint)
    CLASS_METHOD_MAP(mWidget, onLButtonDown)
    CLASS_METHOD_MAP(mWidget, onLButtonUp)
    CLASS_METHOD_MAP(mWidget, onMouseMove)
    CLASS_METHOD_MAP(mWidget, onKeyDown)
    CLASS_METHOD_MAP(mWidget, onKeyUp)
    CLASS_METHOD_MAP(mWidget, onSizeChanged)
    CLASS_METHOD_MAP(mWidget, onEraseBkgnd)
END_CMPT_CLASS

#endif /* defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR) */
