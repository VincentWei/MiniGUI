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
** defdev.c: implementation of default MG_RWops object.
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

