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
** freetype2.h: TrueType font support based on FreeType 2.
**
** Created by WEI Yongming, 2000/8/21
*/

#ifndef GUI_FREETYP2_H
    #define GUI_FREETYP2_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <ft2build.h>
#include FT_FREETYPE_H /*installation-specific macros*/
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H
#include FT_SIZES_H
#include FT_LCD_FILTER_H
#include FT_CACHE_H
#include FT_CACHE_MANAGER_H
#include FT_TRIGONOMETRY_H
#include FT_IMAGE_H

#if 0
#define _MGFONT_TTF_CACHE 1
#define _MGMAX_TTF_CACHE  20
#define _MGTTF_CACHE_SIZE 128
#endif

#ifdef _MGFONT_TTF_CACHE
typedef unsigned long HCACHE;
typedef int (* MakeHashKeyFunc)(unsigned short unicode);
#endif 

typedef struct tagFTFACEINFO {
#ifdef _MGFONT_TTF_CACHE
    /*should be MAX_PATH*/
    char        filepathname[256];
    int         face_index;
    int         cmap_index;
#else
    FT_Face     face;
#endif
    BOOL        valid;
} FTFACEINFO, *PFTFACEINFO;

typedef struct tagFTINSTANCEINFO {
    PFTFACEINFO ft_face_info;
#ifdef _MGFONT_TTF_CACHE
    FTC_ImageTypeRec  image_type;
    FT_Int      glyph_done;
    HCACHE      cache;
#else
    FT_Size     size;
#endif
    FT_Matrix   matrix;
    FT_Glyph    glyph;
    int         rotation;

    FT_Vector   advance;
    FT_BBox     bbox;

    int         max_width;
    int         ave_width;
    int         height;
    int         ascent;
    int         descent;

    FT_Bool     use_kerning; /*FT_HAS_KERNING(face)*/
    FT_Vector   delta;
    int         is_index_old;
    FT_UInt     cur_index;
    FT_UInt     prev_index;

    FT_LcdFilter    ft_lcdfilter;
} FTINSTANCEINFO, *PFTINSTANCEINFO;

#ifdef _MGFONT_TTF_CACHE

#define _TTF_HASH_NDIR   37

typedef struct tagTTFCACHEINFO {
    unsigned short unicode;
    short       glyph_code;
    FT_Vector   advance;
    FT_BBox     bbox;
    FT_Vector   delta;
    int         flag;
    int         pitch;
    void        *bitmap;
} TTFCACHEINFO, *PTTFCACHEINFO;

extern HCACHE __mg_ttc_create(char *family, char *charset, DWORD style, int size, 
      int nblk, int blksize, int ndir, MakeHashKeyFunc makeHashKey);
extern int __mg_ttc_write(HCACHE hCache, TTFCACHEINFO *data, int size);
extern void __mg_ttc_release(HCACHE hCache);
extern int __mg_ttc_sys_init(int maxCache, int cacheSize);
extern void __mg_ttc_sys_deinit(void);
extern TTFCACHEINFO *__mg_ttc_search(HCACHE hCache, 
               unsigned short unicode, int *size);
extern HCACHE __mg_ttc_is_exist(char *family, char *charset, 
               DWORD style, int size);
extern void __mg_ttc_refer(HCACHE hCache);

#endif 

#undef TTF_DBG

#ifndef TTF_DBG
#define DP(x) 
#else 
#define DP(x) do { printf x; } while (0)
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FREETYP2_H

