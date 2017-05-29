/*
** $Id: snprintf.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** snprintf.c: implementation of own_snprintf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei yongming
*/

#include <stddef.h>

#include "common.h"

#if defined (_MGUSE_OWN_STDIO) && defined (_MGUSE_OWN_SNPRINTF)

int own_snprintf( char *s, size_t size, const char *format, ... )
{
    int rc;
    va_list ap;

    va_start (ap, format);

    rc = own_vsnprintf (s, size, format, ap);

    va_end (ap);

    return rc;
}

#endif /* _MGUSE_OWN_STDIO */

