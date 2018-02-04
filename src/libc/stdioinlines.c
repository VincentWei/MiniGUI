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
** vfnprintf.c: implementation of own_vfnprintf function.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei yongming
*/

#include <stdarg.h>
#include <limits.h>

#include "common.h"

#ifdef _MGUSE_OWN_STDIO

int own_vfprintf( FILE *stream, const char *format, va_list arg )
{
    return own_vfnprintf(stream, INT_MAX, format, arg);
}

int own_vprintf( const char *format, va_list arg)
{
    return own_vfnprintf( stdout, INT_MAX, format, arg );
}

int own_vsprintf( char *s, const char *format, va_list arg )
{
    return own_vsnprintf(s, INT_MAX, format, arg);
}

FILE *own_fopen(const char *FILE, const char *MODE)
{
    return NULL;
}

int own_fclose(FILE *FP)
{
    return 0;
}

size_t own_fread(void *BUF, size_t SIZE, size_t COUNT,
    FILE *FP)
{
    return 0;
}

size_t own_fwrite(const void *BUF, size_t SIZE,
    size_t COUNT, FILE *FP)
{
    return 0;
}

int own_fflush(FILE *FP)
{
    return 0;
}
#endif /* _MGUSE_OWN_STDIO */

