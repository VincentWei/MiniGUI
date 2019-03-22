/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2019, Beijing FMSoft Technologies Co., Ltd.
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
** layout-utils.c: The implementation of utilities related LAYOUTINFO
**
** Create by WEI Yongming at 2019/03/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCHARSET_UNICODE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "unicode-ops.h"
#include "layoutinfo.h"

GLYPHSTRING* __mg_glyph_string_new (void)
{
    GLYPHSTRING *string = calloc(1, sizeof(GLYPHSTRING));
    return string;
}

void __mg_glyph_string_free (GLYPHSTRING *string)
{
    if (string == NULL)
        return;

    free (string->glyphs);
    free (string->log_clusters);
    free (string);
}

void __mg_glyph_string_set_size (GLYPHSTRING* string, int new_len)
{
    if (new_len < 0)
        return;

    while (new_len > string->space) {
        if (string->space == 0) {
            string->space = 4;
        }
        else {
            const unsigned int max_space =
                MIN (INT_MAX,
                    UINT_MAX / MAX (sizeof(SHAPEDGLYPH), sizeof(int)));

            unsigned int more_space = (unsigned int)string->space * 2;

            if (more_space > max_space) {
                more_space = max_space;

                if ((unsigned int)new_len > max_space) {
                    _WRN_PRINTF("Too large glyph string of length %d", new_len);
                }
            }

            string->space = more_space;
        }
    }

    string->glyphs = realloc (string->glyphs,
            string->space * sizeof (SHAPEDGLYPH));
    string->log_clusters = realloc (string->log_clusters,
            string->space * sizeof (int));
    string->nr_glyphs = new_len;
}

int __mg_glyph_string_get_width(const GLYPHSTRING *glyphs)
{
    int i;
    int width = 0;

    for (i = 0; i < glyphs->nr_glyphs; i++)
        width += glyphs->glyphs[i].width;

    return width;
}

#define LTR(glyph_item) (((glyph_item)->trun->el & 1) == 0)

BOOL __mg_glyph_item_iter_next_cluster (GlyphItemIter *iter)
{
    int glyph_index = iter->end_glyph;
    GLYPHSTRING *glyphs = iter->glyph_item->gs;
    int cluster;
    TEXTRUN *item = iter->glyph_item->trun;

    if (LTR (iter->glyph_item)) {
        if (glyph_index == glyphs->nr_glyphs)
            return FALSE;
    }
    else {
        if (glyph_index < 0)
            return FALSE;
    }

    iter->start_glyph = iter->end_glyph;
    iter->start_index = iter->end_index;
    iter->start_char = iter->end_char;

    if (LTR (iter->glyph_item)) {
        cluster = glyphs->log_clusters[glyph_index];
        while (TRUE) {
            glyph_index++;

            if (glyph_index == glyphs->nr_glyphs) {
                iter->end_index = item->si + item->len;
                iter->end_char = item->len;
                break;
            }

            if (glyphs->log_clusters[glyph_index] > cluster) {
                iter->end_index = item->si + glyphs->log_clusters[glyph_index];
                iter->end_char += iter->end_index - iter->start_index;
                break;
            }
        }
    }
    else {  /* RTL */
        cluster = glyphs->log_clusters[glyph_index];
        while (TRUE) {
            glyph_index--;

            if (glyph_index < 0) {
                iter->end_index = item->si + item->len;
                iter->end_char = item->len;
                break;
            }

            if (glyphs->log_clusters[glyph_index] > cluster) {
                iter->end_index = item->si + glyphs->log_clusters[glyph_index];
                iter->end_char += iter->end_index - iter->start_index;
                break;
            }
        }
    }

    iter->end_glyph = glyph_index;

    assert (iter->start_char < iter->end_char);
    assert (iter->end_char <= item->len);
    return TRUE;
}

BOOL __mg_glyph_item_iter_prev_cluster (GlyphItemIter *iter)
{
    int glyph_index = iter->start_glyph;
    GLYPHSTRING *glyphs = iter->glyph_item->gs;
    int cluster;
    TEXTRUN *item = iter->glyph_item->trun;

    if (LTR (iter->glyph_item)) {
        if (glyph_index == 0)
            return FALSE;
    }
    else {
        if (glyph_index == glyphs->nr_glyphs - 1)
            return FALSE;
    }

    iter->end_glyph = iter->start_glyph;
    iter->end_index = iter->start_index;
    iter->end_char = iter->start_char;

    if (LTR (iter->glyph_item)) {

        cluster = glyphs->log_clusters[glyph_index - 1];
        while (TRUE) {
            if (glyph_index == 0) {
                iter->start_index = item->si;
                iter->start_char = 0;
                break;
            }

            glyph_index--;

            if (glyphs->log_clusters[glyph_index] < cluster) {
                glyph_index++;
                iter->start_index = item->si + glyphs->log_clusters[glyph_index];
                iter->start_char -= iter->end_index - iter->start_index;
                break;
            }
        }
    }
    else { /* RTL */

        cluster = glyphs->log_clusters[glyph_index + 1];
        while (TRUE) {
            if (glyph_index == glyphs->nr_glyphs - 1) {
                iter->start_index = item->si;
                iter->start_char = 0;
                break;
            }

            glyph_index++;

            if (glyphs->log_clusters[glyph_index] < cluster) {
                glyph_index--;
                iter->start_index = item->si + glyphs->log_clusters[glyph_index];
                iter->start_char -= iter->end_index - iter->start_index;
                break;
            }
        }
    }

    iter->start_glyph = glyph_index;

    assert (iter->start_char < iter->end_char);
    assert (0 <= iter->start_char);

    return TRUE;
}

BOOL __mg_glyph_item_iter_init_start (GlyphItemIter  *iter,
        const GlyphItem *glyph_item, const Uchar32 *text)
{
    iter->glyph_item = glyph_item;
    iter->text = text;

    if (LTR (glyph_item))
        iter->end_glyph = 0;
    else
        iter->end_glyph = glyph_item->gs->nr_glyphs - 1;

    iter->end_index = glyph_item->trun->si;
    iter->end_char = 0;

    iter->start_glyph = iter->end_glyph;
    iter->start_index = iter->end_index;
    iter->start_char = iter->end_char;

    /* Advance onto the first cluster of the glyph item */
    return __mg_glyph_item_iter_next_cluster (iter);
}

BOOL __mg_glyph_item_iter_init_end (GlyphItemIter *iter,
        const GlyphItem *glyph_item, const Uchar32 *text)
{
    iter->glyph_item = glyph_item;
    iter->text = text;

    if (LTR (glyph_item))
        iter->start_glyph = glyph_item->gs->nr_glyphs;
    else
        iter->start_glyph = -1;

    iter->start_index = glyph_item->trun->si + glyph_item->trun->len;
    iter->start_char = glyph_item->trun->len;

    iter->end_glyph = iter->start_glyph;
    iter->end_index = iter->start_index;
    iter->end_char = iter->start_char;

    /* Advance onto the first cluster of the glyph item */
    return __mg_glyph_item_iter_prev_cluster (iter);
}

void __mg_glyph_item_get_logical_widths (const GlyphItem *glyph_item,
        const Uchar32 *text, int *logical_widths)
{
    GlyphItemIter iter;
    BOOL has_cluster;
    int dir;

    dir = glyph_item->trun->el % 2 == 0 ? +1 : -1;
    for (has_cluster = __mg_glyph_item_iter_init_start (&iter, glyph_item, text);
            has_cluster;
            has_cluster = __mg_glyph_item_iter_next_cluster (&iter))
    {
        int glyph_index, char_index, num_chars, cluster_width = 0, char_width;

        for (glyph_index  = iter.start_glyph;
                glyph_index != iter.end_glyph;
                glyph_index += dir) {
            cluster_width += glyph_item->gs->glyphs[glyph_index].width;
        }

        num_chars = iter.end_char - iter.start_char;
        if (num_chars) /* pedantic */
        {
            char_width = cluster_width / num_chars;

            for (char_index = iter.start_char;
                    char_index < iter.end_char;
                    char_index++)
            {
                logical_widths[char_index] = char_width;
            }

            /* add any residues to the first char */
            logical_widths[iter.start_char] += cluster_width - (char_width * num_chars);
        }
    }
}

void __mg_shape_utf8 (const char* text, int len,
        const TEXTRUN* trun, GLYPHSTRING* gs)
{
}

void __mg_text_run_free(TEXTRUN* trun)
{
}

void __mg_glyph_item_letter_space (const GlyphItem* glyph_item,
        const Uchar32* ucs, const BreakOppo* bos, int letter_spacing)
{
}


#endif /*  _MGCHARSET_UNICODE */

