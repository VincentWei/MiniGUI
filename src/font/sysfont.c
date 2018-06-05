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
** sysfont.c: Load, create and handle system font and system charset.
** 
** Create date: 1999/01/03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"

#include "devfont.h"
#include "sysfont.h"
#include "rawbitmap.h"
#include "charset.h"
#include "fontname.h"

/**************************** Global data ************************************/
PLOGFONT g_SysLogFont [NR_SYSLOGFONTS];

/**************************** Static data ************************************/

static char* sys_font_name [] =
{
    "default",
    "wchar_def",
    "fixed",
    "caption",
    "menu",
    "control"
};

BOOL mg_InitSysFont (void)
{
    int         i;
    PLOGFONT*   sys_fonts = NULL;
    int         nr_fonts;
    int         id[NR_SYSLOGFONTS];
    int         font_id;

    if (GetMgEtcIntValue ("systemfont", "font_number", &nr_fonts) < 0 )
        return FALSE;

    if (nr_fonts < 1) return TRUE;
    if (nr_fonts > NR_SYSLOGFONTS) nr_fonts = NR_SYSLOGFONTS;

    for (i = 0; i < NR_SYSLOGFONTS; i++) {

        if (GetMgEtcIntValue ("systemfont", sys_font_name [i], &font_id) < 0
                    || font_id < 0 || font_id >= nr_fonts) {
            fprintf (stderr, "FONT>SysFont: Error system logical font identifier: %s.\n", sys_font_name [i]);
            return FALSE;
        }
        id [i] = font_id;
    }

#ifdef HAVE_ALLOCA
    if ((sys_fonts = alloca (nr_fonts * sizeof (PLOGFONT))) == NULL)
#else
    if ((sys_fonts = malloc (nr_fonts * sizeof (PLOGFONT))) == NULL)
#endif
        return FALSE;

    memset (sys_fonts, 0, nr_fonts * sizeof (PLOGFONT));

    for (i = 0; i < nr_fonts; i++) {
        char key [11];
        char type [LEN_FONT_NAME + 1];
        char family [LEN_FONT_NAME + 1];
        char style [LEN_FONT_NAME + 1];
        char charset [LEN_FONT_NAME + 1];
        char font_name [LEN_DEVFONT_NAME + 1];
        int  height;
        int  j = 0, need = 0;

        while (j < NR_SYSLOGFONTS) {
            if ( i == id [j] ) {
                need = 1;
                break;
            }
            j ++;
        };

        if (i == 0 || need) {
            sprintf (key, "font%d", i);
            if (GetMgEtcValue ("systemfont", key, font_name, LEN_DEVFONT_NAME) < 0)
                goto error_load;

            if (!fontGetTypeNameFromName (font_name, type)
                    || !fontGetFamilyFromName (font_name, family)
                    || !fontCopyStyleFromName (font_name, style)
                    || !fontGetCharsetFromName (font_name, charset)
                    || ((height = fontGetHeightFromName (font_name)) == -1)) {

                fprintf (stderr, "FONT>SysFont: Invalid system logical font name: %s.\n", 
                                font_name);
                goto error_load;
            }

            _DBG_PRINTF ("FONT>SysFont: system font %d: %s-%s-%d-%s\n", 
                    i, type, family, height, charset);

            if (i == 0 && GetCharsetOps (charset)->bytes_maxlen_char > 1) {
                fprintf (stderr, "FONT>SysFont: First system font should be a single-byte charset. font_name: %s\n",
                                font_name);
                goto error_load;
            }

            if ((sys_fonts[i] = CreateLogFont (type, family, charset, 
                    style [0], style [1], style [2],
                    style [3], style [4], style [5],
                    height, 0)) == NULL) {
                goto error_load;
            }
        }

        if (i == 0) {
            g_SysLogFont [0] = sys_fonts [0];
        }
    }


    for (i = 0; i < NR_SYSLOGFONTS; i++) {
        g_SysLogFont [i] = sys_fonts[id[i]];
    }

#ifndef HAVE_ALLOCA
    free (sys_fonts);
#endif
    return TRUE;

error_load:
    fprintf (stderr, "FONT>SysFont: Error in creating system logical fonts!\n");
    for (i = 0; i < nr_fonts; i++) {
        if (sys_fonts [i])
            DestroyLogFont (sys_fonts[i]);
    }

#ifndef HAVE_ALLOCA
    free (sys_fonts);
#endif
    return FALSE;
}

static inline BOOL is_freed_font (int id)
{
    int i;

    for (i = 0; i < id; i++) {
        if (g_SysLogFont[i] == g_SysLogFont[id])
            return TRUE;
    }

    return FALSE;
}

void mg_TermSysFont (void)
{
    int i;

    for (i = 0; i < NR_SYSLOGFONTS; i++) {
        if (g_SysLogFont [i] && !is_freed_font (i))
            DestroyLogFont (g_SysLogFont [i]);
    }
}

/**************************** API: System Font Info *************************/
const char* GUIAPI GetSysCharset (BOOL wchar)
{
    if (wchar) {
        if (g_SysLogFont[1]->mbc_devfont)
            return MBC_DEVFONT_INFO_P (g_SysLogFont[1])->charset_ops->name;
        else
            return NULL;
    }
    else 
        return SBC_DEVFONT_INFO_P (g_SysLogFont[0])->charset_ops->name;
}

int GUIAPI GetSysCharWidth (void)
{
    DEVFONT* sbc_devfont = g_SysLogFont[0]->sbc_devfont;

    return sbc_devfont->font_ops->get_ave_width (g_SysLogFont[0], sbc_devfont);
}

int GUIAPI GetSysCCharWidth (void)
{
    DEVFONT* mbc_devfont = g_SysLogFont[1]->mbc_devfont;

    if (mbc_devfont)
    	return mbc_devfont->font_ops->get_ave_width (g_SysLogFont[1], mbc_devfont);
    else
        return GetSysCharWidth () * 2;
}

int GUIAPI GetSysCharHeight (void)
{
    return g_SysLogFont[0]->size;
}

int GUIAPI GetSysFontMaxWidth (int font_id)
{
    DEVFONT* sbc_devfont = g_SysLogFont[font_id]->sbc_devfont;

    return sbc_devfont->font_ops->get_max_width (g_SysLogFont[font_id], sbc_devfont);
}

int GUIAPI GetSysFontAveWidth (int font_id)
{
    DEVFONT* sbc_devfont = g_SysLogFont[font_id]->sbc_devfont;

    return sbc_devfont->font_ops->get_ave_width (g_SysLogFont[font_id], sbc_devfont);
}

int GUIAPI GetSysFontHeight (int font_id)
{
    return g_SysLogFont[font_id]->size;
}

