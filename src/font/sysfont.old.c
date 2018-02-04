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
** Current maintainer: Wei Yongming.
**
** Create date: 1999.01.03 by Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#include "sysfont.h"
#include "rawbitmap.h"
#include "charset.h"
#include "devfont.h"
#include "fontname.h"

/**************************** Global data ************************************/
LOGFONT SysLogFont;

/**************************** Static data ************************************/
static int nr_fonts;
static RBFINFO* SysRBFInfos;
static DEVFONT* SysDevFonts;

BOOL mg_InitSysFont (void)
{
    int i;
    char font_name [LEN_DEVFONT_NAME + 1];

    if (GetMgEtcIntValue ("systemfont", "font_number", 
                           &nr_fonts) < 0 )
        return FALSE;
    if ( nr_fonts < 1)
        return FALSE;

    SysRBFInfos = malloc (sizeof (RBFINFO) * nr_fonts);
    SysDevFonts = malloc (sizeof (DEVFONT) * nr_fonts);

    if (SysRBFInfos == NULL || SysDevFonts == NULL) {
        free (SysRBFInfos);
        free (SysDevFonts);
        return FALSE;
    }

    for (i = 0; i < nr_fonts; i++)
        SysRBFInfos [i].font = NULL;

    for (i = 0; i < nr_fonts; i++) {
        char key [11];
        char charset [LEN_FONT_NAME + 1];
        char file [MAX_PATH + 1];

        sprintf (key, "name%d", i);
        if (GetMgEtcValue ("systemfont", key, 
                           font_name, LEN_DEVFONT_NAME) < 0 )
            goto error_load;

        if (!fontGetCharsetFromName (font_name, charset)) {
            fprintf (stderr, "GDI: Invalid font name (charset): %s.\n", 
                    font_name);
            goto error_load;
        }

        if ((SysRBFInfos [i].charset_ops 
               = GetCharsetOps (charset)) == NULL) {
            fprintf (stderr, "GDI: Not supported charset: %s.\n", charset);
            goto error_load;
        }

        if ((SysRBFInfos [i].width = fontGetWidthFromName (font_name)) == -1) {
            fprintf (stderr, "GDI: Invalid font name (width): %s.\n", 
                    font_name);
            goto error_load;
        }
        
        if ((SysRBFInfos [i].height = fontGetHeightFromName (font_name)) == -1) {
            fprintf (stderr, "GDI: Invalid font name (height): %s.\n",
                    font_name);
            goto error_load;
        }
        
        SysRBFInfos [i].nr_chars = SysRBFInfos [i].charset_ops->nr_chars;

        sprintf (key, "fontfile%d", i);
        if (GetMgEtcValue ("systemfont", key, 
                           file, MAX_PATH) < 0)
            goto error_load;

        if (!LoadRawBitmapFont (file, SysRBFInfos + i))
            goto error_load;

        strncpy (SysDevFonts[i].name, font_name, LEN_DEVFONT_NAME);
        SysDevFonts[i].name [LEN_DEVFONT_NAME] = '\0';
        SysDevFonts[i].font_ops = &raw_bitmap_font_ops;
        SysDevFonts[i].charset_ops = SysRBFInfos [i].charset_ops;
        SysDevFonts[i].data = SysRBFInfos + i;
#if 0
        fprintf (stderr, "GDI: SysDevFont %i: %s.\n", i, SysDevFonts[i].name);
#endif
    }

    font_AddSBDevFont (SysDevFonts);
    for (i = 1; i < nr_fonts; i++) {
        font_AddMBDevFont (SysDevFonts + i);
    }

    // Init System DevFont and LogFont info
    strcpy (SysLogFont.type, "rbf");
    strcpy (SysLogFont.family, "system");
    strcpy (SysLogFont.charset, "system");
    SysLogFont.style        = 0;
    SysLogFont.size         = SysRBFInfos [0].height;
    SysLogFont.sbc_devfont  = SysDevFonts;
    if (nr_fonts > 1)
        SysLogFont.mbc_devfont = SysDevFonts + 1;
    else
        SysLogFont.mbc_devfont = NULL;

    return TRUE;

error_load:
    fprintf (stderr, "GDI: Error in loading system fonts!\n");
    for (i = 0; i < nr_fonts; i++)
        UnloadRawBitmapFont (SysRBFInfos + i);
    
    free (SysRBFInfos);
    free (SysDevFonts);
    SysRBFInfos = NULL;
    SysDevFonts = NULL;
    return FALSE;
}

void mg_TermSysFont (void)
{
    int i;

    for (i = 0; i < nr_fonts; i++)
        UnloadRawBitmapFont (SysRBFInfos + i);
    
    free (SysRBFInfos);
    free (SysDevFonts);

    SysRBFInfos = NULL;
    SysDevFonts = NULL;
}

/**************************** API: System Font Info *************************/
const char* GUIAPI GetSysCharset (void)
{
    if (SysLogFont.mbc_devfont == NULL)
        return SBC_DEVFONT_INFO (SysLogFont)->charset_ops->name;
    else 
        return MBC_DEVFONT_INFO (SysLogFont)->charset_ops->name;
}

BOOL GUIAPI DoesSupportCharset (const char* charset)
{
    int i;

    for (i = 0; i < nr_fonts; i++) {
        if ((*SysRBFInfos [i].charset_ops->is_this_charset) (charset) == 0)
            return TRUE;
    }

    return FALSE;
}

int GUIAPI SetSysCharset (const char* charset)
{
    int i;

    for (i = 0; i < nr_fonts; i++) {
        if ((*SysRBFInfos [i].charset_ops->is_this_charset) (charset) == 0)
            break;
    }

    if (i == nr_fonts)
        return -1;

    SysLogFont.sbc_devfont = SysDevFonts;
    if (i == 0)
        SysLogFont.mbc_devfont = NULL;
    else 
        SysLogFont.mbc_devfont = SysDevFonts + i;

    return 0;
}

int GUIAPI GetSysCharWidth (void)
{
    return SysRBFInfos[0].width;
}

int GUIAPI GetSysCCharWidth (void)
{
    if (nr_fonts > 1)
        return SysRBFInfos[1].width;
    else
        return 0;
}

int GUIAPI GetSysCharHeight (void)
{
    return SysRBFInfos[0].height;
}

#define TEXT_TABSIZE    8
#define DEF_TABSTOPS    TEXT_TABSIZE

void GUIAPI GetSysTabbedTextExtent (const char* spText, SIZE* pSize)
{
    int x = 0;
    int y = 0;

    pSize->cx = 0;
    pSize->cy = 0; 

    for (; *spText; spText++) {
        switch (*spText) {
        case '\a':              /* badly implemented */
            break;

        case '\b':
            x -= GetSysCharWidth();
            if(x < 0)
               x = 0;
            break;

        case '\n':
            y += GetSysCharHeight();
            if(y > pSize->cy) pSize->cy = y;

        case '\r':
            x = 0;
            break;

        case '\t':
            x += (TEXT_TABSIZE - (x / GetSysCharWidth()) % TEXT_TABSIZE)
                 * GetSysCharWidth();
            if(x > pSize->cx) pSize->cx = x;
            break;

        case '\v':
            y += GetSysCharHeight();
            if(y > pSize->cy) pSize->cy = y;
            break;

        default:
            if( ((unsigned char)(*spText) & 0x80)
                && ((unsigned char)(*(spText + 1)) & 0x80)) {
                spText++;
                x += GetSysCCharWidth();
                if(x > pSize->cx) pSize->cx = x;
            }
            else {
                x += GetSysCharWidth();
                if(x > pSize->cx) pSize->cx = x;
            }
        }
    }
}

