/*
** $Id: main.c 11102 2008-10-23 01:58:25Z tangjianbin $
**
** main.c: The main function wrapper.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gal.h"
#include "ial.h"


#ifndef _MGRM_THREADS
BOOL mgIsServer = FALSE;
#endif

void GUIAPI MiniGUIPanic (int exitcode)
{
    exitcode = 1;
#ifndef _MGRM_THREADS
    if (mgIsServer)
#endif
    {
        mg_TerminateIAL ();
    } 

    mg_TerminateGAL ();

#ifndef __NOUNIX__
    _exit (exitcode);
#endif
}

