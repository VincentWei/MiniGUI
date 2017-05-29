/*
** $Id: printf.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** printf.c: implementation of own_printf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei yongming
*/

#include <stddef.h>
#include <stdarg.h>
#include <limits.h>

#include "common.h"

#if defined (_MGUSE_OWN_STDIO) && defined (_MGUSE_OWN_PRINTF)

int own_printf( const char *format, ... )
{
    int rc;
    va_list ap;

    va_start (ap, format);

    rc = own_vfnprintf ((FILE*) &__mg_def_dev, INT_MAX, format, ap);

    va_end (ap);

    return rc;
}

#endif /* _MGUSE_OWN_STDIO */

