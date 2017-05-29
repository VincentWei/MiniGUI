/*
** $Id: vsnprintf.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** sprintf.c: implementation of own_sprintf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei yongming
*/

#include <stddef.h>
#include <limits.h>

#include "common.h"

#if defined(_MGUSE_OWN_STDIO) && defined(_MGUSE_OWN_VSNPRINTF)

#include "endianrw.h"

int own_vsnprintf( char *s, size_t size, const char *format, va_list arg )
{
    int rc;
    MG_RWops area;

    MGUI_InitMemRW (&area, s, size);
    
    rc = own_vfnprintf ((FILE *)&area, size, format, arg);

    if( rc > 0 )
        s[rc] = '\0';

    return rc;
}

#endif /* _MGUSE_OWN_STDIO */

