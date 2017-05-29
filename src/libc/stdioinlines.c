/*
** $Id: stdioinlines.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** vfnprintf.c: implementation of own_vfnprintf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
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

