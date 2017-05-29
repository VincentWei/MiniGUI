/*
** $Id: psos_pprivate.c 8944 2007-12-29 08:29:16Z xwyan $
**
** psos_pprivate.c: This file contains the implementation of the POSIX 
**      pthread functions for pSOS.
**
** Copyright (C) 2007 Feynman Software.
** All rights reserved.
**
** Author: Wei Yongming
**
** Create Date: 2007-05-16
*/

#include "mgconfig.h"

#if defined (__PSOS__) && defined (_MGUSE_OWN_PTHREAD)

#include <psos.h>

int *__errno_location (void)
{
    return (int*) errno_addr();
}

#endif /* __PSOS__ && _MGUSE_OWN_PTHREAD */
