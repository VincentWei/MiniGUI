/*
** $Id: defdev.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** defdev.c: implementation of default MG_RWops object.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei Yongming
*/

#include <string.h>

#include "common.h"

#ifdef _MGUSE_OWN_STDIO

#include "endianrw.h"

#if defined (__NOUNIX__) && !defined (WIN32)

static int (*__rtos_output_char) (int ch);
static int (*__rtos_input_char) (void);

static int def_seek (MG_RWops *context, int offset, int whence)
{
    return 0;
}

static int def_read (MG_RWops *context, void *ptr, int size, int maxnum)
{
    if (__rtos_input_char) {
        int i;
        BYTE* data = ptr;

        for (i = 0; i < size*maxnum; i++) {
            *data++ = __rtos_input_char ();
        }
    }
    else {
        memset (ptr, 0, size*maxnum);
    }

	return maxnum;
}

static int def_write (MG_RWops *context, const void *ptr, int size, int num)
{
    const BYTE* data = ptr;

    if (__rtos_output_char) {
        int i;
        for (i = 0; i < size*num; i++) {
            __rtos_output_char (data [i]);
        }
    }

	return num;
}

static int def_ungetc (MG_RWops *context, unsigned char c)
{
	return EOF;
}

static int def_close (MG_RWops *context)
{
	return 0;
}

static int def_eof (MG_RWops *context)
{
    return 0;
}

MG_RWops __mg_def_dev = {def_seek, def_read, def_write, def_ungetc, 
        def_close, def_eof, RWAREA_TYPE_UNKNOWN};

int init_minigui_printf (int (*output_char) (int ch),
                int (*input_char) (void))
{
    __rtos_output_char = output_char;
    __rtos_input_char = input_char;

    return 0;
}

#else /* __NOUNIX__ && !WIN32 */

#include <string.h>
#include <stdio.h>

/* Functions to read/write stdio dev */

static int def_seek (MG_RWops *context, int offset, int whence)
{
    return 0;
}

static int def_read (MG_RWops *context, void *ptr, int size, int maxnum)
{
    fread (ptr, size, maxnum, stdin);
	return maxnum;
}

static int def_write (MG_RWops *context, const void *ptr, int size, int num)
{
    fwrite (ptr, size, num, stdout);
	return num;
}

static int def_ungetc (MG_RWops *context, unsigned char c)
{
    return ungetc (c, stdin);
}

static int def_close (MG_RWops *context)
{
	return 0;
}

static int def_eof (MG_RWops *context)
{
    return 0;
}

MG_RWops __mg_def_dev = {def_seek, def_read, def_write, def_ungetc, def_close, def_eof, RWAREA_TYPE_UNKNOWN};

#endif

#endif /* _MGUSE_OWN_STDIO */

