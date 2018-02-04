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

#include <assert.h>
#include <string.h>
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"
#include "devfont.h"
#include "charset.h"
#include "fontname.h"

#ifdef _MGFONT_SEF
#include "palette.h"
#include "se_minigui.h"

static const unsigned char* PALETTE_TABLE[34] =
{
    0,
    PALETTE1A, PALETTE1B, PALETTE1C, PALETTE1D,
    PALETTE2A, PALETTE2B, PALETTE2C, PALETTE2D,
    PALETTE3A, PALETTE3B, PALETTE3C, PALETTE3D,
    PALETTE4A, PALETTE4B, PALETTE4C, PALETTE4D,
    PALETTE5A, PALETTE5B, PALETTE5C, PALETTE5D,
    PALETTE6A, PALETTE6B, PALETTE6C, PALETTE6D,
    PALETTE7A, PALETTE7B, PALETTE7C, PALETTE7D,
    PALETTE8A, PALETTE8B, PALETTE8C, PALETTE8D
};
#define PALETTE_TABLE_SIZE  (sizeof(PALETTE_TABLE) / sizeof(unsigned char*))
static int current_palette_index = 0;

#ifdef SE_PALETTE_AUTO_ADJUST
    static const unsigned char *current_palette = 0;
    #define SE_PALETTE_BW   PALETTEBW
    #define SE_PALETTE_WB       0
#endif

static DEVFONT      *se_dev_fonts = NULL;

static FONTOPS scripteasy_font_ops;

static unsigned char   is_se_initialized = 0;
static pthread_mutex_t se_mutex;

#define SE_MINIGUI_TRACE          0

static void init_lock(void)
{
    assert(!is_se_initialized);
    pthread_mutex_init(&se_mutex, NULL);
}

static void destroy_lock(void)
{
    assert(is_se_initialized);
    pthread_mutex_destroy(&se_mutex);
}

static void lock(void)
{
    assert(is_se_initialized);
    pthread_mutex_lock(&se_mutex);
}

static void unlock(void)
{
    assert(is_se_initialized);
    pthread_mutex_unlock(&se_mutex);
}

static BOOL load_fonts(void)
{
    int i;
    int font_table_size = se_font_desc.font_name_map_size;    
    BOOL result = FALSE;

#if SE_MINIGUI_TRACE
    printf("load_fonts, font_table_size: %d...\n", font_table_size);
#endif

    se_dev_fonts = calloc (font_table_size, sizeof (DEVFONT));

    for (i = 0; i < font_table_size; i++) {
        char        charset[LEN_FONT_NAME + 1];
        CHARSETOPS  *charset_ops;
        const char  *font_name;
        int         font_id;

        font_name = se_font_desc.name_map[i].font_name;
        font_id = se_font_desc.name_map[i].font_id;

        if (!fontGetCharsetFromName (font_name, charset)) {
            fprintf (stderr, "GDI: Invalid font name (charset): %s.\n",
                    font_name);
            goto exit;
        }

        if ((charset_ops = GetCharsetOps (charset)) == NULL) {
            fprintf (stderr, "GDI: Not supported charset: %s.\n", charset);
            goto exit;
        }

        strncpy (se_dev_fonts[i].name, font_name, LEN_DEVFONT_NAME);
        se_dev_fonts[i].name [LEN_DEVFONT_NAME] = '\0';
        se_dev_fonts[i].font_ops = &scripteasy_font_ops;
        se_dev_fonts[i].charset_ops = charset_ops;
        se_dev_fonts[i].data = (void*)font_id;
#if 1
        fprintf (stderr, "GDI: TTFDevFont %i: %s.\n", i, se_dev_fonts[i].name);
#endif
    }

    /* houhh 20090112, surport the other GB2312,UTF-8 such font setting.*/
    for (i = 0; i < font_table_size; i++) {
        int nr_charsets;
        char charsets [LEN_UNIDEVFONT_NAME + 1];

        if (se_dev_fonts [i].charset_ops->bytes_maxlen_char > 1) {
            font_AddMBDevFont (se_dev_fonts + i);
            font_AddSBDevFont (se_dev_fonts + i);
        }
        else
            font_AddSBDevFont (se_dev_fonts + i);

        fontGetCharsetPartFromName (se_dev_fonts[i].name, charsets);
        if ((nr_charsets = charsetGetCharsetsNumber (charsets)) > 1) {
            int j;
            for (j = 1; j < nr_charsets; j++) {
                char charset [LEN_FONT_NAME + 1];
                CHARSETOPS* charset_ops;
                DEVFONT* new_devfont;

                charsetGetSpecificCharset (charsets, j, charset);
                if ((charset_ops = GetCharsetOpsEx (charset)) == NULL)
                    continue;

                new_devfont = calloc (1, sizeof (DEVFONT));
                memcpy (new_devfont, se_dev_fonts + i, sizeof (DEVFONT));
                new_devfont->charset_ops = charset_ops;
                new_devfont->relationship = se_dev_fonts + i;
                if (new_devfont->charset_ops->bytes_maxlen_char > 1)
                    font_AddMBDevFont (new_devfont);
                else
                    font_AddSBDevFont (new_devfont);
            }
        }
    }

    result = TRUE;

exit:

#if SE_MINIGUI_TRACE
    printf("load_fonts complete, result: %s...\n", result ? "success" : "fail");
#endif

    return result;
}




static BOOL init(
    void                *mem_pool, 
    int                 mem_pool_size, 
    void                *cache_mem_pool, 
    int                 cache_mem_pool_size)
{
    SeInitInfo  init_info;
    int         result;
    
    se_minigui_init_fonts();
    //
    // SeInitInfo initialization
    //
    memset( &init_info,0,sizeof(init_info) );
    init_info.language = SE_LANG_ZH_CN;
    init_info.mem_pool = (void*)mem_pool;
    init_info.mem_pool_size = mem_pool_size;
    init_info.font_table = se_font_desc.font_table;
    init_info.logical_font_table = se_font_desc.logical_font_table;
#ifdef SE_FONT_DYNAMIC_LOADING_SUPPORT
    init_info.font_access_callback = se_minigui_read_font;
#endif
    init_info.cache_mem_pool = (void*)cache_mem_pool;
    init_info.cache_mem_pool_size = cache_mem_pool_size;
	
    result = se_minigui_initialize( &init_info );
	if(result != SE_STATUS_OK)
        return FALSE;

    return TRUE;
}

#ifdef __NOUNIX__
typedef struct{
    unsigned int    get_imei_value;
    unsigned int    read_nvram;
    unsigned int    get_copyright;
    unsigned int    trace;
}se_ip_param;

int tp_nvram_read_u8(unsigned int offset, unsigned char *data, unsigned char as);
int app_svr_get_imei_str (char *imei);
char * GetSystemCopyRightStr (char * pBuf);

static void initialize_cert(void)
{
    se_ip_param param = {0};
    param.get_copyright = (unsigned int)GetSystemCopyRightStr;
    param.get_imei_value = (unsigned int)app_svr_get_imei_str;
    param.read_nvram = (unsigned int)tp_nvram_read_u8;

    se_minigui_set_chip_param(&param);
}
#endif

SE_EXPORT BOOL initialize_scripteasy(void)
{
#if SE_MINIGUI_TRACE
    printf("in initialize_scripteasy...\n");
#endif
    if(is_se_initialized)
        return TRUE;

    if(se_font_desc.font_table_size < 1)
        return TRUE;
    
    if(!load_fonts())
        return FALSE;

    if(!init(se_minigui_mem_pool, se_minigui_mem_pool_size, se_minigui_cache_mem_pool, se_minigui_cache_mem_pool_size))
        return FALSE;

    init_lock();
    is_se_initialized = 1;

#if SE_MINIGUI_TRACE
    printf("initialize_scripteasy complete...\n");
#endif

#ifdef __NOUNIX__
    initialize_cert();
#endif

    return TRUE;
}

SE_EXPORT void uninitialize_scripteasy(void)
{
#ifndef SE_OPT_ARM
    printf("in uninitialize_scripteasy...\n");
#endif
    if(!is_se_initialized)
        return;

    se_minigui_cleanup();
    se_minigui_destroy_fonts();

    if(se_dev_fonts != NULL)
    {
        free(se_dev_fonts);
        se_dev_fonts = NULL;
    }

    destroy_lock();
    is_se_initialized = 0;
}

static void convert_log_font(SeLogFont *selogfont, const LOGFONT *logfont, const DEVFONT *devfont)
{
    selogfont->size = logfont->size;
    selogfont->id = (unsigned char)(int)devfont->data;

    selogfont->attr = 0;

    if(selogfont->id > 0)
        selogfont->attr |= SE_LOGFONT_PREFERRED;
/*
    if(logfont->style & FS_WEIGHT_BOLD)
        selogfont->attr |= SE_LOGFONT_BOLD;

    if(logfont->style & FS_SLANT_ITALIC)
        selogfont->attr |= SE_LOGFONT_ITALIC;
*/
    if(logfont->style & FS_UNDERLINE_LINE)
        selogfont->attr |= SE_LOGFONT_UNDERLINE;

    if(logfont->style & FS_STRUCKOUT_LINE)
        selogfont->attr |= SE_LOGFONT_STRIKETHROUGH;

#if 0// SE_MINIGUI_TRACE
    printf("logfont >>> size: %d id: %d attr: 0x%02X\n", selogfont->size, selogfont->id, selogfont->attr);
#endif
}

#if 0
static int get_glyph_width (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    seunichar16 wc;
    SeLogFont   selogfont;
    int         width;

    if (devfont->charset_ops->conv_to_uc32)
        wc = (*devfont->charset_ops->conv_to_uc32)(glyph_value);
    else
        wc = glyph_value;

#if SE_MINIGUI_TRACE
    printf("in get_glyph_width...\n");
#endif

    convert_log_font(&selogfont, logfont, devfont);

    lock();

    width = se_minigui_get_char_width(&selogfont, wc);

    unlock();

#if SE_MINIGUI_TRACE
    printf("get_glyph_width complete...\n");
#endif

    return width;
}
#endif

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return logfont->size;
}

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    if (logfont->style & FS_WEIGHT_BOOK)
        return DEVFONTGLYPHTYPE_GREYBMP;

    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return logfont->size;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return logfont->size;
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    return logfont->size;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    SeLogFont   selogfont;
    int ascent;

    convert_log_font(&selogfont, logfont, devfont);

    lock();

//    ascent = se_minigui_get_font_ascent(&selogfont);
    ascent = selogfont.size - selogfont.size / 4;

    unlock();

#if SE_MINIGUI_TRACE
    printf("get_font_ascent complete. ascent: %d...\n", ascent);
#endif

    return ascent;
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    SeLogFont   selogfont;
    int descent;

    convert_log_font(&selogfont, logfont, devfont);

    lock();

//    descent = se_minigui_get_font_descent(&selogfont);
    descent = selogfont.size / 4;
    unlock();

#if SE_MINIGUI_TRACE
    printf("get_font_descent complete. descent: %d...\n", descent);
#endif

    return descent;
    //return logfont->size - get_font_ascent (logfont, devfont);
}

static const void* get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    seunichar16 wc;
    SeLogFont   selogfont;
    const void *result;
    
#if SE_MINIGUI_TRACE
    printf("in get_glyph_greybitmap....\n");
#endif

    if (devfont->charset_ops->conv_to_uc32)
        wc = (*devfont->charset_ops->conv_to_uc32)(glyph_value);
    else
        wc = glyph_value;

    convert_log_font(&selogfont, logfont, devfont);
    
    lock();
    result = se_minigui_get_char_pixmap(&selogfont, wc, pitch);
    unlock();    

#if SE_MINIGUI_TRACE
    printf("[GET_CHAR_PIXMAP_RESULT] wc: 0x%04X size: %d attr: %d id: %d pitch: %d\n", 
        wc,
        selogfont.size,
        selogfont.attr,
        selogfont.id,
        pitch);
#endif
    if(scale)
        *scale = 1;

    return result;
}

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    seunichar16 wc;
    SeLogFont   selogfont;
    const void *result = NULL;

#if SE_MINIGUI_TRACE
    printf("in get_char_bitmap....\n");
#endif

    if (devfont->charset_ops->conv_to_uc32)
        wc = (*devfont->charset_ops->conv_to_uc32)(glyph_value);
    else
        wc = glyph_value;

    convert_log_font(&selogfont, logfont, devfont);
    selogfont.attr |= SE_LOGFONT_MONO;
    
    lock();
    result = se_minigui_get_char_pixmap(&selogfont, wc, pitch);
    unlock();

    if(scale)
        *scale = 1;

    return result;
}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py, 
                int* pwidth, int* pheight)
{
    seunichar16 wc;
    SeLogFont   selogfont;
    int result = 0;

    if(px == 0 && py == 0 && pwidth == 0 && pheight == 0)
        return 1;

    if (devfont->charset_ops->conv_to_uc32)
        wc = (*devfont->charset_ops->conv_to_uc32)(glyph_value);
    else
        wc = glyph_value;

    convert_log_font(&selogfont, logfont, devfont);

    lock();

    result = se_minigui_get_char_bbox(&selogfont, wc, px, py, pwidth, pheight);

    unlock();

#if SE_MINIGUI_TRACE
    printf("get_char_bbox result. wc: 0x%04X x: %d y: %d w: %d h: %d\n", wc, px ? *px : 0, py ? *py : 0,
              pwidth ? *pwidth : 0, pheight ? *pheight : 0);
#endif

    return result;
}


static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    seunichar16 wc;
    SeLogFont   selogfont;
    int x = *px;

    if (devfont->charset_ops->conv_to_uc32)
        wc = (*devfont->charset_ops->conv_to_uc32)(glyph_value);
    else
        wc = glyph_value;

    convert_log_font(&selogfont, logfont, devfont);

    lock();

    se_minigui_get_char_advance(&selogfont, wc, px, py);

    unlock();

#if SE_MINIGUI_TRACE
    printf("get_glyph_advance result. wc: 0x%04X x: %d y: %d\n", wc, px ? *px : 0, py ? *py : 0);
#endif
   return *px - x; 
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    int uni_char;
    int x, y, width, height;
    
    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;
    
    if (!get_glyph_bbox (logfont, devfont, 
                glyph_value, &x, &y, &width, &height))
        return FALSE;
    else
        return TRUE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return rot_desired;
}

/**************************** Global data ************************************/
static FONTOPS scripteasy_font_ops = {
    get_glyph_type,
    get_ave_width,
    get_max_width,
    get_font_height,
    get_font_size,
    get_font_ascent,
    get_font_descent,

    is_glyph_existed,
    get_glyph_advance,
    get_glyph_bbox,

    get_glyph_monobitmap,
    get_glyph_greybitmap,

    NULL,
    NULL,
    NULL,
    NULL,
    is_rotatable,
    NULL,
    NULL
};

/***********************************Pallete Processing*************************/

SE_EXPORT int se_set_palette_index(int index)
{
    if(index >= 0 && index <= PALETTE_TABLE_SIZE)
    {
        current_palette_index = index;
    }

    return current_palette_index;
}

SE_EXPORT int se_next_palette(void)
{
    current_palette_index++;
    if(current_palette_index >= PALETTE_TABLE_SIZE)
        current_palette_index = 0;

    return current_palette_index;
}

SE_EXPORT int se_previous_palette(void)
{
    current_palette_index--;
    if(current_palette_index < 0)
        current_palette_index = PALETTE_TABLE_SIZE - 1;

    return current_palette_index;
}

SE_EXPORT int se_get_current_palette_index(void)
{
    return current_palette_index;
}

SE_EXPORT const unsigned char * se_get_current_palette(void)
{
#ifdef SE_PALETTE_AUTO_ADJUST
    return current_palette;
#else
    return PALETTE_TABLE[current_palette_index];
#endif
}

/* Palette auto adjust routines */

#define YUV_Y(r, g, b) ((299 * (r) + 587 * (g) + 114 * (b)) >> 10)
//extern gal_pixel SeGetPixel(HDC hdc, int x, int y);
#define SeGetPixel(hdc, x, y) GetPixel(hdc, x, y)
void se_switch_palette(
    HDC             hdc,
    int             x,
    int             y,
    int             w,
    int             h)
{
#ifdef SE_PALETTE_AUTO_ADJUST
    int         bkmode;
    gal_pixel   bg_color;
    gal_pixel   fg_color;
    int         yuv1, yuv2;
    int         r = 0, g = 0, b = 0;
    
    bkmode = GetBkMode(hdc);
    fg_color = GetTextColor(hdc);

    if (bkmode == BM_TRANSPARENT)
    {
        gal_pixel   colors[4];
        int         i = 0;
        Uint8       rr, gg, bb;
        colors[0] = SeGetPixel(hdc, x + 1, y + 1);
        colors[1] = SeGetPixel(hdc, x + w - 1, y + 1);
        colors[2] = SeGetPixel(hdc, x + 1, y + h - 1);
        colors[3] = SeGetPixel(hdc, x + w - 1, y + h -1);
        for ( ; i < 4; ++i)
        {
            Pixel2RGB(hdc, colors[i], &rr, &gg, &bb);
            r += rr;
            g += gg;
            b += bb;
        }
        
        r >>= 2;
        g >>= 2;
        b >>= 2;
    }
    else
    {
        bg_color = GetBkColor(hdc);
        Pixel2RGB(hdc, bg_color, (Uint8*)&r, (Uint8*)&g, (Uint8*)&b);
    }
    
    yuv2 = YUV_Y(r, g, b);
    
    Pixel2RGB(hdc, fg_color, (Uint8*)&r, (Uint8*)&g, (Uint8*)&b);
    yuv1 = YUV_Y(r, g, b);

    if(yuv2 < yuv1)
    {
        current_palette = SE_PALETTE_BW;
    }
    else
    {
        current_palette = SE_PALETTE_WB;
    }

#endif
}
#endif
