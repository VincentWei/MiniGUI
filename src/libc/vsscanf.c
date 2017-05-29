/*
** $Id: vsscanf.c 8944 2007-12-29 08:29:16Z xwyan $
**
** scanf.c: implementation of own_scanf function.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-03 by Wei yongming
*/

#include <stddef.h>
#include <limits.h>

#include "common.h"

#if defined (_MGUSE_OWN_STDIO) && defined(_MGUSE_OWN_VSSCANF)

int own_vsscanf( const char *s, const char *format, va_list arg )
{
    int rc;
    MG_RWops area;
    
    MGUI_InitMemRW (&area, (char*)s, INT_MAX);
    
    rc = own_vfscanf( (FILE *)&area, format, arg );

    return rc;
}

#endif /* _MGUSE_OWN_STDIO */


