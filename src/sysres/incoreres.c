/*
** $Id: resource.c 10273 2008-06-13 05:19:30Z dongjunjie $
**
** resource.c: This file include some functions for system resource loading. 
**           some functions are from misc.c.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2003/09/06
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _MGRM_THREADS
#include <pthread.h>
#endif

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cursor.h"
#include "icon.h"
#include "sysres.h"

#ifdef _MGINCORE_RES 

#include "../font/rawbitmap.h"
#include "../font/varbitmap.h"
#include "../font/qpf.h"
#include "../font/upf.h"

#include "bmp/_bmp_inner_res.c"
#include "icon/_icon_inner_res.c"
#include "font/_font_inner_res.c"

#endif

BOOL sysres_init_inner_resource()
{
#ifdef _MGINCORE_RES
    AddInnerRes(__mgir_bmp_inner_res, TABLESIZE(__mgir_bmp_inner_res), FALSE);
    AddInnerRes(__mgir_icon_inner_res, TABLESIZE(__mgir_icon_inner_res),FALSE);
    AddInnerRes(__mgir_font_inner_res, TABLESIZE(__mgir_font_inner_res),FALSE);
#endif
    return TRUE;
}

#ifdef _MGHAVE_CURSOR
#ifdef _MGINCORE_RES
extern const int __mg_cursors_offset [];
extern const unsigned char __mg_cursors_data[];
#endif
//HCURSOR GUIAPI LoadCursorFromRes (int i)
HCURSOR mg_LoadCursorFromRes (int i)
{
    HCURSOR hCursor = 0;
    int nr_cursors = 0;

    if (GetMgEtcIntValue ("cursorinfo", "cursornumber", &nr_cursors) < 0 )
        return 0;

    if (i >= nr_cursors || i < 0)
        return 0;

#ifdef _MGINCORE_RES
    hCursor = LoadCursorFromMem (__mg_cursors_data + __mg_cursors_offset [i]);
#endif

    if (!hCursor) {
        char filename[MAX_NAME + 1];
        char path[MAX_PATH + 1];
        char key[10];

        strncpy (path, sysres_get_system_res_path(), MAX_PATH);
        strcat(path, "cursor/");

        sprintf (key, "cursor%d", i);
        if (GetMgEtcValue (CURSORSECTION, key, filename, MAX_NAME) < 0)
            return 0;

        strcat (path, filename);
        hCursor = LoadCursorFromFile (path);

    }

    return hCursor;
}
#endif


