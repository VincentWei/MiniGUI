/**
 * \file own_stdio.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 * \brief ISO C standard I/O routines - with some POSIX 1003.1 extensions
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: own_stdio.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
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

