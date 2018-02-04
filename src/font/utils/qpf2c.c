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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "qpf2c.h"

#define LINE_BYTE_NUM 8
static inline long get_file_size (char* filename)
{
    FILE* fp = fopen (filename, "rb");
    if (fp == NULL)
        return 0;

    fseek (fp, 0, SEEK_END);
    return ftell (fp);
}

static inline BOOL init_array (ARRAY* array, int item_size, int item_num)
{
    array->array = malloc (item_size * item_num);
    if (! array->array)
        return FALSE;

    array->item_size = item_size;
    array->item_num = item_num;
    array->alloced_num = 0;
    return TRUE;
}

static inline BOOL reinit_array (ARRAY* array, int item_num)
{
    array->array = realloc (array->array, array->item_size * item_num);
    if (! array->array)
        return FALSE;

    array->item_num = item_num;
    return TRUE;
}

static inline void destroy_array (ARRAY* array)
{
    free (array->array);
    array->array = NULL;
    array->item_num = 0;
    array->alloced_num = 0;
}

static inline int alloc_from_array (ARRAY* array, int num)
{
    int tmp;
    if (array->alloced_num == array->item_num) {
        array->item_num = array->item_size * 2 + num;
        if (reinit_array (array, array->item_size) == FALSE)
            return -1;
    }

    tmp = array->alloced_num;
    array->alloced_num += num;
    return tmp;
}

static inline void* get_item (ARRAY* array, int index)
{
    if (index >= array->alloced_num)
        return NULL;

    return array->array + array->item_size * index;
}

static int read_node (ARRAY* a_node, ARRAY* a_glyph, uchar** data)
{
    uchar rw, cl;
    int flags;
    QPF_GLYPHTREE* root;
    int root_index;

    root_index = alloc_from_array (a_node, 1);
    root = get_item (a_node, root_index);

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    root->min = (rw << 8) | cl;

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    root->max = (rw << 8) | cl;

    root->glyph_index = alloc_from_array (a_glyph, root->max-root->min+1);

    flags = **data; (*data)++;

    if (flags & 1)
        root->less_tree_index = read_node (a_node, a_glyph, data);
    else
        root->less_tree_index = -1;

    if (flags & 2)
        root->more_tree_index = read_node (a_node, a_glyph, data);
    else
        root->more_tree_index = -1;

    return root_index;
}

static void read_metrics (ARRAY* a_node, int node_index,
       ARRAY* a_glyph, uchar** data)
{
    int i;
    int n;
    int glyph_index;
    QPF_GLYPHTREE* node;
    QPF_GLYPH* glyph;

    if (node_index == -1)
        return;

    node = (QPF_GLYPHTREE* ) get_item (a_node, node_index);
    n = node->max - node->min + 1;
    glyph_index = node->glyph_index;

    for (i = 0; i < n; i++) {
        glyph = (QPF_GLYPH*) get_item (a_glyph, glyph_index+i);
        glyph->metrics = (QPF_GLYPHMETRICS*) *data;

        *data += sizeof (QPF_GLYPHMETRICS);
    }

    read_metrics (a_node, node->less_tree_index, a_glyph, data);
    read_metrics (a_node, node->more_tree_index, a_glyph, data);
}

static void read_data (ARRAY* a_node, int node_index,
       ARRAY* a_glyph, uchar** data)
{
    int i;
    int n;
    int glyph_index;
    QPF_GLYPHTREE* node;
    QPF_GLYPH* glyph;

    if (node_index == -1)
        return;

    node = (QPF_GLYPHTREE* ) get_item (a_node, node_index);
    n = node->max - node->min + 1;
    glyph_index = node->glyph_index;

    for (i = 0; i < n; i++) {
        glyph = (QPF_GLYPH*) get_item (a_glyph, glyph_index+i);
        glyph->data =  *data;

        *data += glyph->metrics->linestep * glyph->metrics->height;
    }

    read_data (a_node, node->less_tree_index, a_glyph, data);
    read_data (a_node, node->more_tree_index, a_glyph, data);
}


static void build_glyph_tree (ARRAY* a_node, ARRAY* a_glyph, 
        unsigned char** data)
{
    read_node (a_node, a_glyph, data);
    read_metrics (a_node, 0, a_glyph, data);
    read_data (a_node, 0, a_glyph, data);
}

static void printf_data_array (FILE* fp_c, char* prefix, uchar* data, int data_size)
{
    int i;
    int j;
    int left;
    fprintf (fp_c, "static unsigned char %s_data [ ] = {\n", prefix);
    for (i=0; i<data_size; i+=LINE_BYTE_NUM)
    {
        fprintf (fp_c, "\t");
        for (j=0; j<LINE_BYTE_NUM; j++)
        {
            fprintf (fp_c, "0x%.2x, ", *data);
            data++;
        }
        fprintf (fp_c, "\n");
    }

    left = data_size % LINE_BYTE_NUM;

    fprintf (fp_c, "\t");
    for (i=0; i<left; i++)
    {
        fprintf (fp_c, "0x%x, ", *data);
        data++;
    }

    fprintf (fp_c, "};\n\n");
}

static void printf_glyph_array (FILE* fp_c, char* prefix, ARRAY* a_glyph, uchar* data)
{
    int i;
    QPF_GLYPH* glyph;

    fprintf (fp_c, "static QPF_GLYPH %s_glyph [ ] = {\n", prefix);
    for (i=0; i<a_glyph->alloced_num; i++)
    {
        glyph = (QPF_GLYPH*) get_item (a_glyph, i);

        fprintf (fp_c, "\t{(QPF_GLYPHMETRICS*)(%s_data + %d), %s_data + %d},\n", 
                prefix, ((uchar*)glyph->metrics) - data,
                prefix, glyph->data - data);

    }
    fprintf (fp_c, "};\n\n");
}

static void printf_node_array (FILE* fp_c, char* prefix, ARRAY* a_node)
{
    int i;
    QPF_GLYPHTREE* node;
    fprintf (fp_c, "static QPF_GLYPHTREE %s_tree [ ] = {\n", prefix);

    for (i=0; i<a_node->alloced_num; i++)
    {
        node = (QPF_GLYPHTREE*) get_item (a_node, i);

        fprintf (fp_c, "\t{%d, %d, ", node->min, node->max);

        if (node->less_tree_index == -1 )
            fprintf (fp_c, "NULL, ");
        else
            fprintf (fp_c, "%s_tree + %d, ", prefix, node->less_tree_index);

        if (node->more_tree_index == -1 )
            fprintf (fp_c, "NULL, ");
        else
            fprintf (fp_c, "%s_tree + %d, ", prefix, node->more_tree_index);

        fprintf (fp_c, "%s_glyph + %d},\n", prefix, node->glyph_index);
    }

    fprintf (fp_c, "};\n\n");
}

static void printf_qpfinfo (FILE* fp_c, char* prefix, int height, int width)
{
    fprintf (fp_c, "static QPFINFO %s_info = {\n", prefix);
    fprintf (fp_c, "\t%d,\n"
            "\t%d,\n"
            "\t%d,\n"
            "\t(QPFMETRICS*) %s_data,\n"
            "\t%s_tree,\n",
            width, height, 0,
            prefix, prefix);
    fprintf (fp_c, "};\n");
    
}

static BOOL get_c_file_and_array_prefix (char* file_name, 
        char* c_file_name, char* prefix)
{
    char* cp;
    char* point;

    point = strrchr (file_name, '.');
    if (!point || strcmp (point+1, "qpf") != 0) {
        fprintf (stderr, "%s --not qpf font\n", file_name);
        return FALSE;
    }

    strcpy (prefix, "__mgif_qpf_");
    strncpy (prefix+strlen(prefix), file_name, point-file_name);

    for (cp=prefix; *cp; cp++)
    {
        if (isalnum(*cp) || *cp == '_')
            continue;

        *cp = '_';
    }

    strcpy (c_file_name, "qpf_");
    strncpy (c_file_name+strlen(c_file_name), file_name, point-file_name);
    strcat (c_file_name, ".c");

    return TRUE;
}

BOOL static qpf2c_for_minigui_v3 (char* file_name)
{
    int file_size;
    FILE* fp;
    FILE* fp_c;
    ARRAY a_glyph;
    ARRAY a_node;
    unsigned char* data;
    unsigned char* real_data;
    QPFMETRICS* font_metrics;
    static char prefix [128];
    static char c_file_name [128];

    bzero (prefix, 128);
    bzero (c_file_name, 128);
    if (get_c_file_and_array_prefix (file_name, c_file_name, prefix)
            == FALSE)
        return FALSE;

    file_size = get_file_size (file_name);
    if (file_size <=0)
        return FALSE;


    data = (unsigned char*) malloc (file_size);
    init_array (&a_glyph, sizeof(QPF_GLYPH), file_size/sizeof(QPF_GLYPH));
    init_array (&a_node, sizeof(QPF_GLYPHTREE), file_size/sizeof(QPF_GLYPHTREE));

    fp = fopen (file_name, "rb");
    fp_c = fopen (c_file_name, "w+");

    fread (data, 1, file_size, fp);
    font_metrics = (QPFMETRICS*)data;
    real_data = data + sizeof(QPFMETRICS);

    build_glyph_tree (&a_node, &a_glyph, &real_data);

    printf_data_array (fp_c, prefix, data, file_size);

    printf_glyph_array (fp_c, prefix, &a_glyph, data);
    printf_node_array (fp_c, prefix, &a_node);
    printf_qpfinfo (fp_c, prefix, font_metrics->maxwidth, 
            font_metrics->ascent + font_metrics->descent);
    fclose (fp_c);
    printf ("%s -----> %s\n", file_name, c_file_name);

    return TRUE;
}

int main (int argc, char* argv[])
{
    int i = 1;
    for (i=1; i<argc; i++)
        qpf2c_for_minigui_v3 (argv[i]);

    return 0;
}
