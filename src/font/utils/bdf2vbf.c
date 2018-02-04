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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minigui/common.h>
#include <minigui/gdi.h>

#include "../varbitmap.h"

#define VAL_DEBUG
//#define INFO_DEBUG
#include "my_debug.h"

#define SET_WEIGHT_POS     0
#define SET_SLANT_POS      1
#define SET_SETWIDTH_POS   2

#define LINE_BUF_LEN       256
#define INV_METRICS     0xFFFF
#define UPTO_MUTI_FOUR(i) (((i)+3)>>2<<2)
#define ARCHSWAP32_SELF(i) ((i) = ArchSwap32(i))


/*get one line and*/
static char* getline (char* buf, FILE* fp)
{
    int len;
    fgets(buf, LINE_BUF_LEN, fp);
    len = strlen (buf);

    if (buf[len-1] == '\n')
        buf[len-1] = '\0';

    /*void line*/
    if (!*buf)
        return NULL;

    while (*buf && !isspace(*buf))
        buf++;
    *buf++ = '\0';

    while (*buf && isspace(*buf))
        buf++;
    if (*buf == '\"')
    {
        buf++;
        *(strchr(buf, '\"')) = '\0';
    }

    return (*buf) ? buf : NULL;
}

static void set_rrncnn (char* rrncnn, int set_pos, char* value)
{
    switch (set_pos)
    {
        case SET_WEIGHT_POS:
            if (strcasecmp(value, "black") == 0)
                rrncnn[SET_WEIGHT_POS] = 'c';
            else if (strcasecmp(value, "bold") == 0)
                rrncnn[SET_WEIGHT_POS] = 'b';
            else if (strcasecmp(value, "book") == 0)
                rrncnn[SET_WEIGHT_POS] = 'k';
            else if (strcasecmp(value, "black") == 0)
                rrncnn[SET_WEIGHT_POS] = 'c';
            else if (strcasecmp(value, "demibold") == 0)
                rrncnn[SET_WEIGHT_POS] = 'd';
            else if (strcasecmp(value, "light") == 0)
                rrncnn[SET_WEIGHT_POS] = 'l';
            else if (strcasecmp(value, "medium") == 0)
                rrncnn[SET_WEIGHT_POS] = 'm';
            else if (strcasecmp(value, "regular") == 0)
                rrncnn[SET_WEIGHT_POS] = 'r';
            break;

        case SET_SLANT_POS:
            if (strcasecmp(value, "italic") == 0)
                rrncnn[SET_SLANT_POS] = 'i';
            else if (strcasecmp(value, "oblique") == 0)
                rrncnn[SET_SLANT_POS] = 'o';
            else if (strcasecmp(value, "roman") == 0)
                rrncnn[SET_SLANT_POS] = 'r';
            break;
        case SET_SETWIDTH_POS:
            if (strcasecmp(value, "bold") == 0)
                rrncnn[SET_SETWIDTH_POS] = 'b';
            else if (strcasecmp(value, "condensed") == 0)
                rrncnn[SET_SETWIDTH_POS] = 'c';
            else if (strcasecmp(value, "semicondensed") == 0)
                rrncnn[SET_SETWIDTH_POS] = 's';
            else if (strcasecmp(value, "normal") == 0)
                rrncnn[SET_SETWIDTH_POS] = 'n';
            break;
    }

}

#define FIND_LINE(line_buf, fp, header, body)\
    do \
    {\
        body = getline (line_buf, fp);\
    } while (strcmp (line_buf, header) != 0)

static void get_propt_from_name (char* name, char* family, char* charset, int* height) 
{
    int num;
    TEST_VAL (name, %s);
    num = sscanf (name, 
            "%*[-]%*[^-]" /*vender, not needed*/
            "%*[-]%[^-]" /*family*/
            "%*[-]%*[^-]" /*weight, not needed*/
            "%*[-]%*[^-]" /*slant, not needed*/
            "%*[-]%*[^-]" /*setwidth, not needed*/
            "%*[-]%d" /*height*/
            "%*[-]%*d" /*unkown*/
            "%*[-]%*d" /*unkown*/
            "%*[-]%*d" /*unkown*/
            "%*[-]%*[^-]" /*unkdown*/
            "%*[-]%*d" /*unkown*/
            "%*[-]%s",/*charset*/
            family, 
            height, 
            charset);

    TEST_VAL (family, %s);
    TEST_VAL (*height, %d);
    TEST_VAL (charset, %s);

}

static BOOL  get_font_propt(FILE* fp, char* family, char* font_rrncnn, char* charset, 
                        int* height, int* descent, int* def_glyph)
{
    char* line_buf;
    char* body;
    int ascent = 0;

    *height = 0;
    *descent = 0;

    line_buf = (char*)malloc(LINE_BUF_LEN);

#if 1
    FIND_LINE (line_buf, fp, "FONT", body);
    get_propt_from_name (body, family, charset, height);
#endif

    FIND_LINE (line_buf, fp , "FONTBOUNDINGBOX", body);
    sscanf(body, "%*d%d", height);
    TEST_VAL (*height, %d);

    FIND_LINE (line_buf, fp, "STARTPROPERTIES", body);

    while (1)
    {
        body = getline (line_buf, fp);
        TEST_VAL(line_buf, %s);
        TEST_VAL(body, %s);
        TEST_INFO("");

        if (strcmp (line_buf, "ENDPROPERTIES") == 0)
            break;

        /*FAMILY_NAME*/
        if (strcmp (line_buf, "FAMILY_NAME") == 0)
        {
            if (strlen(body) > LEN_FONT_NAME)
            {
                fprintf(stderr, "the FAMILY_NAME is too long, "
                        "please truncation it to less than %d\n", LEN_FONT_NAME-2);
                return FALSE;
            }
            strncpy (family, body, LEN_FONT_NAME);
        }
        else if (strcmp (line_buf, "WEIGHT_NAME") == 0)
            set_rrncnn (font_rrncnn, SET_WEIGHT_POS, body);

        /*FIXME SLANT_NAME? ? ?*/
        else if (strcmp (line_buf, "SLANT_NAME") == 0)
            set_rrncnn (font_rrncnn, SET_SLANT_POS, body);
        else if (strcmp (line_buf, "SETWIDTH_NAME") == 0)
            set_rrncnn (font_rrncnn, SET_SETWIDTH_POS, body);
            
        /*FIXME must inorder*/
        else if (strcmp (line_buf, "CHARSET_REGISTRY") == 0)
            strncpy (charset, body, LEN_FONT_NAME);
        else if (strcmp (line_buf, "CHARSET_ENCODING") == 0)
        {
            if (body[0] != '-')
                strcat (charset, "-");
            strcat (charset, body);
        }
        
        else if (strcmp (line_buf, "PIXEL_SIZE") == 0)
            *height = atoi (body);
        else if (strcmp (line_buf, "FONT_DESCENT") == 0)
            *descent = atoi (body);
        else if (strcmp (line_buf, "FONT_ASCENT") == 0)
            ascent = atoi (body);

        else if (strcmp (line_buf, "DEFAULT_CHAR") == 0)
        {
            *def_glyph = atoi (body);
            TEST_VAL(*def_glyph, %d);
        }
    }

    /*calc descent*/
    if (*descent == 0)
    {
        if (ascent != 0)
            *descent = *height - ascent;
        else
            *descent = 0;
    }
    else
    {
        /*adjust height*/
        if (ascent!=0 && height < descent + ascent)
            height = descent + ascent;
    }
    free(line_buf);
    return TRUE;
}

static void 
print_bitmap(FILE* file, char* bits, int width, int height)
{
    int y = 0;
    int x = 0;
    char* p_line_head;
    char* p_cur_char;
    int pitch  = (width +7) >>3;
    
    for (y = 0, p_line_head = bits; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_cur_char = (x >> 3) + p_line_head;
            if (*p_cur_char & (128 >> (x%8)))
                fprintf(file, "@ ");
            else
                fprintf(file, ". ");
        }
        fprintf(file, "\n");
        
        p_line_head += pitch;
    }
}

#define HEX_CHAR_VAL(ch)\
    do {\
        if (ch & 0x40) /*'a'--'f'*/\
            ch = (ch & 0x0F) + 9;\
        else /*'0'--'9'*/\
            ch &= 0x0F;\
    }while (0)

static int realloc_font(char** advx, char** advy, VBF_BBOX** bbox, 
        Uint32** offsets, int char_num)
{
    int old_len_advx = UPTO_MUTI_FOUR (char_num);
    int old_len_advy = UPTO_MUTI_FOUR (char_num);
    int old_len_bbox = UPTO_MUTI_FOUR (char_num * sizeof(VBF_BBOX));
    int old_len_offsets = UPTO_MUTI_FOUR (char_num * sizeof(Uint32));

    int new_len_advx = UPTO_MUTI_FOUR (char_num<<1);
    int new_len_advy = UPTO_MUTI_FOUR (char_num<<1);
    int new_len_bbox = UPTO_MUTI_FOUR ((char_num<<1) * sizeof(VBF_BBOX));
    int new_len_offsets = UPTO_MUTI_FOUR ((char_num<<1) * sizeof(Uint32));

    *advx = (char*)realloc (*advx, new_len_advx);
    *advy = (char*)realloc (*advy, new_len_advy);
    *bbox = (VBF_BBOX*)realloc (*bbox, new_len_bbox);
    *offsets = (Uint32*) realloc (*offsets, new_len_offsets);

    memset(((char*)*advx)+old_len_advx, 0, (new_len_advx-old_len_advx));
    memset(((char*)*advy)+old_len_advy, 0, (new_len_advy-old_len_advy));
    memset(((char*)*bbox)+old_len_bbox, 0, (new_len_bbox-old_len_bbox));
    memset(((char*)*offsets)+old_len_offsets, 0, 
            (new_len_offsets-old_len_offsets));

    return char_num<<1;
}

static int get_all_glyph(FILE* fp, int glyph_num, 
        VBF_BBOX** bbox, char** advx, char** advy, Uint32** offsets, 
        char* glyph_bits, int* first_glyph, int* last_glyph, 
        int* max_width, int* ave_width, int* char_num)
{
    char* line_buf;
    char* body;

    int bbox_x = 0;
    int bbox_y = 0;
    int bbox_w = 0;
    int bbox_h = 0;
    int total_width = 0;

    int advance_x = 0;
    int advance_y = 0;

    int glyph_val = 0;

    int next_offset = 0;
    int i;

    char* cp;
    char ch_left;
    char ch_right;
    char* cur_bits = glyph_bits;
    int font_size = 0;
    int intend_char_num, glyph_bitmap_len, padding_len;
    
    *char_num = glyph_num;
    intend_char_num = glyph_num;
    *max_width = 0;

    line_buf = (char*)malloc (LINE_BUF_LEN);

    for (i=0; i<glyph_num; i++)
    {
        FIND_LINE(line_buf, fp, "STARTCHAR", body);

        while (1)
        {
            body = getline(line_buf, fp);

            if (strcmp (line_buf, "ENCODING") == 0)
            {
                glyph_val = atoi (body);

                if (glyph_val < *first_glyph)
                    *first_glyph = glyph_val;
                if (glyph_val > *last_glyph)
                    *last_glyph = glyph_val;

                if (glyph_val - *first_glyph + 1 >= *char_num)
                {
                    *char_num = glyph_val - *first_glyph + 1;
                    if (*char_num >= intend_char_num)
                        intend_char_num = realloc_font(advx, advy, 
                                bbox, offsets, intend_char_num);
                }

                (*offsets)[glyph_val-*first_glyph] = next_offset;
            }

            if (strcmp (line_buf, "DWIDTH") == 0)
            {
                sscanf (body, "%d %d", &advance_x, &advance_y);
                *max_width = (advance_x > *max_width)?
                            advance_x : *max_width;
                total_width += advance_x;
                (*advx)[glyph_val-*first_glyph] = advance_x;
                (*advy)[glyph_val-*first_glyph] = advance_y;
            }

            else if (strcmp (line_buf, "BBX") == 0)
            {
                sscanf (body, "%d %d %d %d", &bbox_w, &bbox_h,
                        &bbox_x, &bbox_y);
                bbox_y += bbox_h;
                (*bbox)[glyph_val-*first_glyph].x = bbox_x;
                (*bbox)[glyph_val-*first_glyph].y = bbox_y;
                (*bbox)[glyph_val-*first_glyph].w = bbox_w;
                (*bbox)[glyph_val-*first_glyph].h = bbox_h;
            }

            if (strcmp (line_buf, "BITMAP") == 0)
            {
                glyph_bitmap_len =0;
                while (1)
                {
                    getline (line_buf, fp);
                    if (strcmp (line_buf, "ENDCHAR") == 0)
                        goto end_one_glyph;

                    cp = line_buf;
                    TEST_VAL(cp, %s);

                    ch_left = *cp++;
                    ch_right = *cp++;

                    while(ch_left)
                    {
                        HEX_CHAR_VAL(ch_left);
                        HEX_CHAR_VAL(ch_right);
                        ch_right |= ch_left << 4;

                        *cur_bits++ = ch_right;
                        font_size ++;

                        ch_left = *cp++;
                        ch_right = *cp++;
                    }
                    next_offset += (cp - line_buf - 2) >> 1;
                    glyph_bitmap_len += (cp - line_buf - 2) >> 1;
                }
            }
        }
end_one_glyph:
        padding_len = (glyph_bitmap_len %4 ? (4 - (glyph_bitmap_len % 4)) : 0);
        next_offset += padding_len;
        while(padding_len){
            *cur_bits++ = 0x00;
            padding_len --;
        }
        TEST_INFO("");
        TEST_VAL (bbox_w, %d);
        TEST_VAL (bbox_h, %d);
        TEST_VAL (next_offset, %d);
        fprintf(stderr, "glyph_val = 0x%x\n", glyph_val);
        print_bitmap(stderr, glyph_bits+(*offsets)[glyph_val-*first_glyph], bbox_w, bbox_h);
        fprintf(stderr, "\n");
    }

    *ave_width = total_width / glyph_num;
    free (line_buf);
    return font_size;
}

typedef struct _FILE_HEADER {
    char ver_info [LEN_VERSION_INFO];    
    char vender_name [LEN_VENDER_INFO];  

    Uint16 len_header;                   

    Uint32 off_bboxs;      
    Uint32 len_bboxs;      

    Uint32 off_advxs;      
    Uint32 len_advxs;      

    Uint32 off_advys;      
    Uint32 len_advys;      

    Uint32 off_bit_offs;   
    Uint32 len_bit_offs;   

    Uint32 off_bits;       
    Uint32 len_bits;       

    Uint32 font_size;      
    int align[0];
} FILE_HEADER;

typedef struct _FONT_PROPT {
    char font_name [LEN_DEVFONT_NAME + 1];

    char max_width;
    char ave_width;      
    char height;         
    char descent;        

    int  first_glyph;    
    int  last_glyph;     
    int  def_glyph;      

    int align[0];
} FONT_PROPT;

char font_name [LEN_UNIDEVFONT_NAME];
char family [LEN_FONT_NAME];
char charset [LEN_FONT_NAME];
char new_file_name [LEN_UNIDEVFONT_NAME+5];

#define MAKE_HEADER(header, glyph_num)\
    do {\
        header.len_header = sizeof(FILE_HEADER);\
        header.len_advxs = UPTO_MUTI_FOUR (glyph_num);\
        header.len_advys = header.len_advxs;\
        header.len_bboxs = UPTO_MUTI_FOUR (glyph_num * sizeof(VBF_BBOX));\
        header.len_bit_offs = UPTO_MUTI_FOUR (glyph_num * sizeof(Uint32));\
    }while (0)

int bdf2vbf(char* file_name)
{
    char font_rrncnn [7] = "rrncnn";

    char* advx;
    char* advy;
    VBF_BBOX* bbox;
    Uint32* offsets;
    char* glyph_bits;
    
    int max_width;
    int ave_width;

    int height;
    int descent;

    int first_glyph = 0x7FFFFFFF;
    int last_glyph = -1;
    int def_glyph = 0;
    int glyph_num;
    
    char* line_buf;
    char* body;
    FILE* fp;
    int file_size;
    int char_num;
    int i;

    FILE_HEADER header;
    FONT_PROPT font_propt;

    fp = fopen (file_name, "rb");

    fseek(fp, 0, SEEK_END);
    file_size = ftell (fp);
    fseek(fp, 0, SEEK_SET);

    if (!fp) 
    {
        fprintf(stderr, "open bdf file error\n");
        return -1;
    }

    line_buf = (char*)malloc(LINE_BUF_LEN);
    if (!get_font_propt(fp, family, font_rrncnn, charset, 
                        &height, &descent, &def_glyph))
        return -1;

    if (height > 128)
    {
        fprintf(stderr, "error: %s is too big %d,\n vbf height is less than 128\n", file_name, height);
        return -1;
    }

    FIND_LINE (line_buf, fp, "CHARS", body);
    glyph_num = atoi (body);
    font_rrncnn[6] = '\0';

    TEST_VAL (family, %s);
    TEST_VAL (font_rrncnn, %s);
    TEST_VAL (charset, %s);

    TEST_VAL (height, %d);
    TEST_VAL (descent, %d);

    TEST_VAL (glyph_num, %d);

    strcpy (header.ver_info, "vbf-3.0**");
    strcpy (header.vender_name, "FMSoft");

    MAKE_HEADER (header, glyph_num);

    advx = (char*)malloc (header.len_advxs);
    advy = (char*)malloc (header.len_advys);
    bbox = (VBF_BBOX*)malloc (header.len_bboxs);
    offsets = (Uint32*) malloc (header.len_bit_offs);

    memset (advx, 0, header.len_advxs);
    memset (advy, 0, header.len_advys);
    memset (bbox, 0, header.len_bboxs);
    memset (offsets, 0, header.len_bit_offs);

    /*size of bdf is larger than sizeof glyph_bits*/
    glyph_bits = (char*) malloc (file_size);

    header.len_bits = get_all_glyph(fp, glyph_num, &bbox, &advx, &advy, &offsets, 
            glyph_bits, &first_glyph, &last_glyph, 
            &max_width, &ave_width, &char_num);

    for (i=0; i<char_num; i++)
    {
        if (advx[i] == 0)
        {
            advx[i] = advx[def_glyph];
            advy[i] = advy[def_glyph];
            bbox[i] = bbox[def_glyph];
            offsets[i] = offsets[def_glyph];
        }

    }

    MAKE_HEADER (header, char_num);

    /*def_glyph may be a offset to first_glyph*/
    if (def_glyph < first_glyph)
        def_glyph += first_glyph;

    sprintf(font_name, "vbf-%s-%s-%d-%d-%s", family, font_rrncnn, 
            max_width, height, charset);
    printf("%s\n", font_name);

    strncpy (font_propt.font_name, font_name, LEN_DEVFONT_NAME);
    font_propt.max_width = max_width;
    font_propt.ave_width = ave_width;
    font_propt.height = height;
    font_propt.descent = descent;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    font_propt.first_glyph = ArchSwap32 (first_glyph);
    font_propt.last_glyph = ArchSwap32 (last_glyph);
    font_propt.def_glyph = ArchSwap32 (def_glyph);
#else
    font_propt.first_glyph = first_glyph;
    font_propt.last_glyph = last_glyph;
    font_propt.def_glyph = def_glyph;
#endif

    fclose(fp);

    strcpy (new_file_name, font_name);
    strcat (new_file_name, ".vbf");
    fp = fopen(new_file_name, "wb");
    fwrite (&header, sizeof(header), 1, fp);
    fwrite (&font_propt, sizeof(font_propt), 1, fp);

    header.off_bboxs = ftell (fp);
    fwrite (bbox, header.len_bboxs, 1, fp);

    header.off_advxs = ftell (fp);
    fwrite (advx, header.len_advxs, 1, fp);

    header.off_advys = ftell (fp);
    fwrite (advx, header.len_advys, 1, fp);

    header.off_bit_offs = ftell (fp);
    fwrite (offsets, header.len_bit_offs, 1, fp);

    header.off_bits = ftell (fp);
    fwrite (glyph_bits, header.len_bits, 1, fp);
    header.font_size = ftell (fp);
    TEST_VAL(header.font_size, %d);

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    header.len_header = ArchSwap16 (header.len_header);

    ARCHSWAP32_SELF (header.off_bboxs);
    ARCHSWAP32_SELF (header.len_bboxs);

    ARCHSWAP32_SELF (header.off_advxs);
    ARCHSWAP32_SELF (header.len_advxs);

    ARCHSWAP32_SELF (header.off_advys);
    ARCHSWAP32_SELF (header.len_advys);

    ARCHSWAP32_SELF (header.off_bit_offs);
    ARCHSWAP32_SELF (header.len_bit_offs);

    ARCHSWAP32_SELF (header.off_bits);
    ARCHSWAP32_SELF (header.len_bits);

    ARCHSWAP32_SELF (header.font_size);
#endif
    fseek (fp, 0, SEEK_SET);
    fwrite (&header, sizeof(header), 1, fp);
    fclose (fp);

    free(line_buf);
    free (advx);
    free (advy);
    free (bbox);
    free (offsets);
    free (glyph_bits);

    return 0;
}

int main (int argc, char* argv[])
{
    int i;
#if 1
    for (i=1; i<argc; i++)
        bdf2vbf (argv[i]);
#else
        bdf2vbf ("bdf.bdf");
#endif
    return 0;
}
