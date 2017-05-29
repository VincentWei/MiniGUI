/*
** $Id: minigui-win32.c 10660 2008-08-14 09:30:39Z weiym $
**
** minigui-win32.c: Defines the entry point for the DLL application.
**
** Copyright (C) 2004 ~ 2008 Feynman Software.
*/

#ifdef WIN32

#include "windows.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif
