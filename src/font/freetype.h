/*
** $Id: freetype.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** freetype.h: TrueType font support based on FreeType.
**
** Copyright (C) 2000 ~ 2002 Wei Yongming
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by WEI Yongming, 2000/8/21
*/

#ifndef GUI_FREETYP_H
    #define GUI_FREETYP_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <freetype/freetype.h>
#include <freetype/ftxkern.h>
#include <freetype/ftnameid.h>
#include <freetype/ftxcmap.h>
#include <freetype/ftxwidth.h>

typedef struct tagTTFGLYPHINFO {
    TT_Face     face;

    TT_Glyph    glyph;
    TT_UShort   last_glyph_index;

    TT_CharMap  char_map;
    TT_UShort   first_char;
    TT_UShort   last_char;

    BOOL        can_kern;
    TT_Kerning  directory;

    BOOL        valid;
} TTFGLYPHINFO, *PTTFGLYPHINFO;

#ifdef _MGFONT_TTF_CACHE
typedef unsigned long HCACHE;
typedef int (* MakeHashKeyFunc)(unsigned short unicode);
#endif 


typedef struct tagTTFINSTANCEINFO {

    PTTFGLYPHINFO ttf_glyph_info;

    TT_Instance instance;
    int         rotation;
    TT_Matrix   matrix;

    int         max_width;
    int         ave_width;
    int         height;
    int         ascent;
    int         descent;
    unsigned short* widths;

    TT_Outline  cur_outline;

#ifdef _MGFONT_TTF_CACHE
    unsigned short       cur_unicode;
#endif 
    short       cur_glyph_code;
    TT_Pos      cur_xmin, cur_ymin;
    TT_F26Dot6  cur_width, cur_height;
    TT_Pos      cur_advance;

#ifdef _MGFONT_TTF_CACHE    
    TT_BBox     cur_bbox;
    TT_Pos      cur_vec_x, cur_vec_y;
#endif 

    short       last_glyph_code;
    short       last_pen_pos;
    
#ifdef _MGFONT_TTF_CACHE    
    HCACHE      cache;
#endif
} TTFINSTANCEINFO, *PTTFINSTANCEINFO;


#ifdef _MGFONT_TTF_CACHE
    
    typedef struct tagTTFCACHEINFO {
        unsigned short unicode;
        short       glyph_code;
        
        TT_Pos      cur_xmin, cur_ymin;
        TT_F26Dot6  width, height;
        TT_Pos      advance;
        TT_BBox     bbox;
    
        TT_Pos      vec_x, vec_y;
        void        *bitmap;
    } TTFCACHEINFO, *PTTFCACHEINFO;

#endif 

#define TTF_INST_INFO_P(devfont) ((TTFINSTANCEINFO*)(devfont->data))
#define TTF_GLYPH_INFO_P(devfont) ((TTFGLYPHINFO*)(TTF_INST_INFO_P(devfont)->ttf_glyph_info))

#ifdef _MGFONT_TTF_CACHE

#define _TTF_HASH_NDIR   37

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

//#define TTF_DBG 1
#ifndef TTF_DBG
#define DP(x) 
#else 
#define DP(x) do { printf x; } while (0)
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FREETYP_H

