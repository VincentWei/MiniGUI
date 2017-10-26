/**
 * \file own_stdio.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 *  ISO C standard I/O routines - with some POSIX 1003.1 extensions
 *
 \verbatim

    Copyright (C) 2002-2012 FMSoft.
    Copyright (C) 1998-2002 Wei Yongming.

    All rights reserved by FMSoft (http://www.fmsoft.cn).

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: own_stdio.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2012 FMSoft.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef OWN_STDIO_H
#define OWN_STDIO_H

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#include "endianrw.h"

#ifdef __cplusplus
extern "C" {
#endif

extern MG_RWops __mg_def_dev;

MG_EXPORT int own_printf( const char * format, ... );

MG_EXPORT int own_fprintf( FILE * stream, const char * format, ... );

MG_EXPORT int own_sprintf( char * str, const char * format, ... );

MG_EXPORT int own_fnprintf( FILE * stream, size_t length, const char * format, ... );

MG_EXPORT int own_snprintf( char * str, size_t length, const char * format, ... );

MG_EXPORT int own_vprintf( const char * format, va_list args );

MG_EXPORT int own_vfprintf( FILE * stream, const char * format, va_list args );

MG_EXPORT int own_vsprintf( char * str, const char * format, va_list args );

MG_EXPORT int own_vfnprintf( FILE * stream, size_t length, const char * format, va_list args );

MG_EXPORT int own_vsnprintf( char * str, size_t length, const char * format, va_list args );

MG_EXPORT int own_scanf( const char * format, ... );

MG_EXPORT int own_fscanf( FILE * stream, const char * format, ... );

MG_EXPORT int own_sscanf( const char * str, const char * format, ... );

MG_EXPORT int own_vscanf( const char * format, va_list args );

MG_EXPORT int own_vfscanf( FILE * stream, const char * format, va_list args );

MG_EXPORT int own_vsscanf( const char * str, const char * format, va_list args );

#ifdef __cplusplus
}  /* end of extern "C" */
#endif

#endif /* OWN_STDIO_H */

