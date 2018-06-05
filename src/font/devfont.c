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
** defont.c: Device fonts management.
**
** Create Date: 2000/07/07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "cursor.h"
#include "sysres.h"

#include "sysfont.h"
#include "devfont.h"
#include "charset.h"
#include "fontname.h"
#include "rawbitmap.h"
#include "varbitmap.h"
#include "misc.h"

static void font_DelDevFont (const char* font_name);

/**************************** Global data ************************************/
static DEVFONT* sb_dev_font_head = NULL;
static DEVFONT* mb_dev_font_head = NULL;
static int nr_sb_dev_fonts;
static int nr_mb_dev_fonts;


#define ADD_DEVFONT_TO_LIST(head, new)          \
{                                               \
    if (head == NULL)                           \
        head = new;                             \
    else {                                      \
        DEVFONT* tail;                          \
        tail = head;                            \
        while (tail->next) tail = tail->next;   \
        tail->next = new;                       \
    }                                           \
                                                \
    new->next = NULL;                           \
}

void font_AddSBDevFont (DEVFONT* dev_font)
{
    ADD_DEVFONT_TO_LIST (sb_dev_font_head, dev_font);
    nr_sb_dev_fonts ++;
}

void font_AddMBDevFont (DEVFONT* dev_font)
{
    ADD_DEVFONT_TO_LIST (mb_dev_font_head, dev_font);
    nr_mb_dev_fonts ++;
}

void font_ResetDevFont (void)
{
    sb_dev_font_head = mb_dev_font_head = NULL;
    nr_sb_dev_fonts = 0;
    nr_mb_dev_fonts = 0;
}

#define MATCHED_TYPE        0x01
#define MATCHED_FAMILY      0x02
#define MATCHED_CHARSET     0x04

static DEVFONT* get_matched_devfont (LOGFONT* log_font, DEVFONT* list_head, 
        int list_len, char* req_charset)
{
    int i = 0;
    BYTE* match_bits;
    int min_error;
    DEVFONT* dev_font;
    DEVFONT* matched_font;

#ifdef HAVE_ALLOCA
    match_bits = alloca (list_len);
#else
    match_bits = (BYTE *)FixStrAlloc (list_len);
#endif

    i = 0;
    dev_font = list_head;
    while (dev_font) {
        int type_req;
        char family [LEN_FONT_NAME + 1];

        /* clear match_bits first. */
        match_bits [i] = 0;

        /* does match this font type? */
        type_req = fontConvertFontType (log_font->type);
        if (type_req == FONT_TYPE_ALL)
            match_bits [i] |= MATCHED_TYPE;
        else if (type_req == fontGetFontTypeFromName (dev_font->name))
            match_bits [i] |= MATCHED_TYPE;

        /* does match this family? */
        fontGetFamilyFromName (dev_font->name, family);
        if (strcasecmp (family, log_font->family) == 0) {
            match_bits [i] |= MATCHED_FAMILY;
        }

        /* does match this charset */
        if (IsCompatibleCharset (req_charset, dev_font->charset_ops)) {
            match_bits [i] |= MATCHED_CHARSET;
        }


        /* FIXME: ignore style */

        dev_font = dev_font->next;
        i ++;
    }

    min_error = FONT_MAX_SIZE;
    matched_font = NULL;
    dev_font = list_head;
    for (i = 0; i < list_len; i++) {
        int error;
        if ((match_bits [i] & MATCHED_TYPE)
                && (match_bits [i] & MATCHED_FAMILY)
                && (match_bits [i] & MATCHED_CHARSET)) {
            error = log_font->size - 
                (*dev_font->font_ops->get_font_size) (log_font, dev_font, 
                        log_font->size);
            error = ABS (error);
            if (min_error >= error) {   /* use >=, make the later has a higher priority */
                min_error = error;
                matched_font = dev_font;
            }
        }

        dev_font = dev_font->next;
    }

    if (matched_font) {
#ifndef HAVE_ALLOCA
        FreeFixStr ((char*)match_bits);
#endif

        matched_font->font_ops->get_font_size (log_font, matched_font, 
                log_font->size);

        return matched_font;
    }

    min_error = FONT_MAX_SIZE;
    dev_font = list_head;
    for (i = 0; i < list_len; i++) {
        int error;
        if (match_bits [i] & MATCHED_CHARSET) {
            error = log_font->size - 
                (*dev_font->font_ops->get_font_size) (log_font, dev_font, log_font->size);
            error = ABS (error);
            if (min_error >= error) {   /* use >=, make the later has a higher priority */
                min_error = error;
                matched_font = dev_font;
            }
        }
        dev_font = dev_font->next;
    }

#ifndef HAVE_ALLOCA
    FreeFixStr ((char*)match_bits);
#endif

    if (matched_font)
        matched_font->font_ops->get_font_size (log_font, matched_font, log_font->size);

    return matched_font;
}

DEVFONT* font_GetMatchedSBDevFont (LOGFONT* log_font)
{
    DEVFONT* matched_devfont;

    if (GetCharsetOps (log_font->charset)->bytes_maxlen_char > 1) {
        /*mbc logfont --- sbc devfont*/
        char sysfont_charset [LEN_FONT_NAME + 1];
        fontGetCharsetFromName (g_SysLogFont[0]->sbc_devfont->name, sysfont_charset);
        matched_devfont = get_matched_devfont (log_font, sb_dev_font_head, 
                nr_sb_dev_fonts, sysfont_charset);
    }
    else {
        /*sbc logfont --- sbc devfont*/
        matched_devfont = get_matched_devfont (log_font, sb_dev_font_head, 
                nr_sb_dev_fonts,log_font->charset);
    }

    if (!matched_devfont && g_SysLogFont [0])
        matched_devfont = g_SysLogFont [0]->sbc_devfont;

    return matched_devfont;
}

DEVFONT* font_GetMatchedMBDevFont (LOGFONT* log_font)
{
    /*sbc logfont doesn't need mbc font*/
    if (GetCharsetOps (log_font->charset)->bytes_maxlen_char == 1)
        return NULL;
    /*mbc logfont --- mbc devfont*/
    else
        return get_matched_devfont (log_font, mb_dev_font_head, 
                nr_mb_dev_fonts, log_font->charset);
}

const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font)
{
    if (dev_font == NULL) {
        return sb_dev_font_head;
    }
    else if (dev_font->charset_ops->bytes_maxlen_char == 1) {
        dev_font = dev_font->next;
        /*if to tail of sbc list, return head of mbc list*/
        if (dev_font == NULL)
            return mb_dev_font_head;
        else
            return dev_font;
    }
    else
        return dev_font->next;
}

unsigned short font_GetBestScaleFactor (int height, int expect)
{
    int error, min_error;
    unsigned short scale = 1;
    
    min_error = height - expect;
    min_error = ABS (min_error);

    while (min_error) {
        scale ++;
        error = (height * scale) - expect;
        error = ABS (error);

        if (error == 0)
            break;

        if (error > min_error) {
            scale --;
            break;
        }
        else
            min_error = error;
    }

    return scale;
}


#ifdef _DEBUG

#define PRINT_DEVFONTS(head, devfont, count) \
{ \
    devfont = head; \
    while (devfont) { \
        _DBG_PRINTF ("  %d: %s, charsetname: %s, style: %p\n",  \
                count,  \
                devfont->name, devfont->charset_ops->name, (PVOID)devfont->style); \
            devfont = devfont->next; \
            count++; \
    } \
}

void dbg_dumpDevFonts (void)
{
    int         count = 0;
    DEVFONT*    devfont;

    _DBG_PRINTF ("============= SBDevFonts ============\n");
    PRINT_DEVFONTS (sb_dev_font_head, devfont, count);
    _DBG_PRINTF ("========== End of SBDevFonts =========\n");

    _DBG_PRINTF ("\n============= MBDevFonts ============\n");
    PRINT_DEVFONTS (mb_dev_font_head, devfont, count);
    _DBG_PRINTF ("========== End of MBDevFonts =========\n");
}
#endif



/***** Added in MiniGUI V2.2 for bitmap font *****/

/*  
 * Removes an element from single-byte devfont linked list. If two elements
 * contain the same data, only the first is removed. If none of the elements
 * contain the data, the single-byte devfont linked list is unchanged   
 */
void font_DelSBDevFont (DEVFONT* dev_font)
{
  DEVFONT *tmp, *prev = NULL;

  tmp = sb_dev_font_head;
  while (tmp != NULL) {
      if (tmp == dev_font) {
	  if (prev != NULL)
              prev->next = tmp->next;
	  else
              sb_dev_font_head = tmp->next;

          nr_sb_dev_fonts --;
          
	  break;
      }
      
      prev = tmp;
      tmp = prev->next;
  }
  
  return;
}

/*  
 * Removes an element from multiple-byte devfont linked list. If two elements
 * contain the same data, only the first is removed. If none of the elements
 * contain the data, the multiple-byte devfont linked list is unchanged   
 */
void font_DelMBDevFont (DEVFONT* dev_font)
{
  DEVFONT *tmp, *prev = NULL;

  tmp = mb_dev_font_head;
  while (tmp != NULL) {
      if (tmp == dev_font) {
          if (prev != NULL)
              prev->next = tmp->next;
          else
              mb_dev_font_head = tmp->next;

          nr_mb_dev_fonts --;
          
	  break;
      }
      prev = tmp;
      tmp = prev->next;
  }
  
  return;
}

//////////////////////////////////////////////////////////////////////
static DEVFONT* make_devfont (const char* font_name, void* data, BOOL is_filename)
{
    FONTOPS_INFO* fontops_info = __mg_fontops_infos;
    DEVFONT* devfont;
    CHARSETOPS* charset_ops;
    char charset [LEN_FONT_NAME + 1];
	
    /*find the FONTOPS and LOADER*/
    while (fontops_info->type) {
        if (strncmp (font_name, fontops_info->type, FONT_TYPE_NAME_LEN) == 0)
            break;
        fontops_info++;
    }
	
    if (fontops_info->type == NULL) {
        _MG_PRINTF ("FONT>DevFont: invalid font type: %s.\n", font_name);
        return NULL;
    }

    /*find the first charset*/
    if (!fontGetCharsetFromName (font_name, charset)) {
        _MG_PRINTF ("FONT>DevFont: Invalid charset name: %s.\n", font_name);
        return NULL;
    }

    if ((charset_ops = GetCharsetOpsEx (charset)) == NULL) {
        _MG_PRINTF ("FONT>DevFont: Invalid charset name %s of font %s.\n", 
                charset, font_name);
        return NULL;
    }

    devfont = calloc (1, sizeof (DEVFONT));

    devfont->font_ops = fontops_info->fontops;
    devfont->style = fontGetStyleFromName (font_name);
    devfont->charset_ops = charset_ops;

    if (is_filename) {
        devfont->data = devfont->font_ops->load_font_data (font_name, data);

        if (devfont->data == NULL) {
            _MG_PRINTF ("FONT>DevFont: error in loading font %s from %s file.\n", 
                font_name, (const char*) data);
            free (devfont);
            return NULL;
        }
        devfont->need_unload = TRUE;
    }
    else {
        devfont->data = data;
        devfont->need_unload = FALSE;
    }

    strncpy (devfont->name, font_name, LEN_UNIDEVFONT_NAME);

    return devfont;
}

#define ADD_DEVFONT_TO_LINKLIST(devfont) \
{ \
    if (devfont->charset_ops->bytes_maxlen_char > 1) \
        font_AddMBDevFont (devfont); \
    else \
        font_AddSBDevFont (devfont); \
}

static void add_relating_devfonts_to_list (DEVFONT* related_devfont)
{
    DEVFONT* sub_devfont;

    char charset [LEN_FONT_NAME + 1];
    char charsets [LEN_UNIDEVFONT_NAME + 1];
    int nr_charsets;

    fontGetCharsetPartFromName (related_devfont->name, charsets);

    if ((nr_charsets = charsetGetCharsetsNumber (charsets)) > 1) {
        int j;

        for (j = 1; j < nr_charsets; j++) {
            CHARSETOPS* charset_ops;
            charsetGetSpecificCharset (charsets, j, charset);
            if ((charset_ops = GetCharsetOpsEx (charset)) == NULL)
                continue;

            sub_devfont = calloc (1, sizeof (DEVFONT));
            memcpy (sub_devfont, related_devfont, sizeof (DEVFONT));
            sub_devfont->charset_ops = charset_ops;
            sub_devfont->relationship = related_devfont;
            ADD_DEVFONT_TO_LINKLIST (sub_devfont);
        }
    }
}

BOOL AddDevFont (const char* font_name, void* data, BOOL is_filename)
{
    DEVFONT* devfont = make_devfont (font_name, data, is_filename);
    if (devfont == NULL) {
        return FALSE;
    }

    ADD_DEVFONT_TO_LINKLIST (devfont);
    add_relating_devfonts_to_list (devfont);
    return TRUE;
}

#ifdef _MGFONT_RBF

#include "rawbitmap.h"

#ifdef _MGINCORERBF_LATIN1_VGAOEM
extern INCORE_RBFINFO __mgif_rbf_vgaoem_8x8;
#endif
#ifdef _MGINCORERBF_LATIN1_TERMINAL
extern INCORE_RBFINFO __mgif_rbf_terminal_8x12;
#endif
#ifdef _MGINCORERBF_LATIN1_FIXEDSYS
extern INCORE_RBFINFO __mgif_rbf_fixedsys_8x15;
#endif

static INCORE_RBFINFO* incore_rbfonts [] = {
#ifdef _MGINCORERBF_LATIN1_VGAOEM
    &__mgif_rbf_vgaoem_8x8,
#endif
#ifdef _MGINCORERBF_LATIN1_TERMINAL
    &__mgif_rbf_terminal_8x12,
#endif
#ifdef _MGINCORERBF_LATIN1_FIXEDSYS
    &__mgif_rbf_fixedsys_8x15,
#endif
};

#define NR_RBFONTS  (sizeof (incore_rbfonts) /sizeof (INCORE_RBFINFO*))

#endif /* _MGFONT_RBF */

#ifdef _MGFONT_VBF

#include "varbitmap.h"

#ifdef _MGINCOREFONT_SANSSERIF
extern VBFINFO __mgif_vbf_SansSerif11x13;
#endif

#ifdef _MGINCOREFONT_COURIER
extern VBFINFO __mgif_vbf_Courier8x13;
#endif

#ifdef _MGINCOREFONT_SYSTEM
extern VBFINFO __mgif_vbf_System14x16;
#endif

static VBFINFO* incore_vbfonts [] = {
#ifdef _MGINCOREFONT_SANSSERIF
    &__mgif_vbf_SansSerif11x13,
#endif
#ifdef _MGINCOREFONT_COURIER
    &__mgif_vbf_Courier8x13,
#endif
#ifdef _MGINCOREFONT_SYSTEM
    &__mgif_vbf_System14x16,
#endif
};

#define NR_VBFONTS  (sizeof (incore_vbfonts) / sizeof (VBFINFO*))

#endif /* _MGFONT_VBF */

#ifdef _MGFONT_UPF

#include "upf.h"

#ifdef _MGINCOREFONT_TIMES
extern UPFINFO __mgif_upf_times_12x10;
extern UPFINFO __mgif_upf_times_17x14;
#endif

static UPFINFO* incore_upfonts [] = {
#ifdef _MGINCOREFONT_SMOOTHTIMES
    &__mgif_upf_times_12x10,
    &__mgif_upf_times_17x14,
#endif
	NULL
};

#define NR_UPFONTS  (sizeof (incore_upfonts) / sizeof (UPFINFO*))

#endif /* _MGFONT_UPF */

BOOL font_InitIncoreFonts (void)
{
    int i;

#ifdef _MGFONT_RBF
    for (i = 0; i < NR_RBFONTS; i++) {
        if (!AddDevFont (incore_rbfonts [i]->name, incore_rbfonts [i]->data, FALSE)) {
            _MG_PRINTF ("FONT>DevFont: can not init incore font: %s\n", incore_rbfonts [i]->name);
            return FALSE;
        }
    }
#endif

#ifdef _MGFONT_VBF
    for (i = 0; i < NR_VBFONTS && incore_vbfonts[i]; i++) {
        if (!AddDevFont (incore_vbfonts [i]->name, incore_vbfonts [i], FALSE)) {
            _MG_PRINTF ("FONT>DevFont: can not init incore font: %s\n", incore_vbfonts [i]->name);
            return FALSE;
        }
    }
#endif

#ifdef _MGFONT_UPF
    for (i = 0; i < NR_UPFONTS && incore_upfonts[i]; i++) {
        const char* name = ((UPFV1_FILE_HEADER*)(incore_upfonts [i]->root_dir))->font_name;
        if (!AddDevFont (name, incore_upfonts [i], FALSE)) {
            _MG_PRINTF ("FONT>DevFont: can not init incore font: %s\n", name);
            return FALSE;
        }
    }
#endif

#ifdef _MGFONT_SEF
    if(!initialize_scripteasy()) {
        _MG_PRINTF (stderr, "GDI: Can not initialize ScriptEasy fonts!\n");
        return FALSE;
    }
#endif

    return TRUE;
}

BOOL font_TerminateIncoreFonts (void)
{
    int i;

#ifdef _MGFONT_RBF
    for (i = 0; i < NR_RBFONTS && incore_rbfonts[i]; i++) {
        font_DelDevFont (incore_rbfonts [i]->name);
    }
#endif

#ifdef _MGFONT_VBF
    for (i = 0; i < NR_VBFONTS && incore_vbfonts[i]; i++) {
        font_DelDevFont (incore_vbfonts [i]->name);
    }
#endif

#ifdef _MGFONT_UPF
    for (i = 0; i < NR_UPFONTS && incore_upfonts[i]; i++) {
        const char* name = ((UPFV1_FILE_HEADER*)(incore_upfonts [i]->root_dir))->font_name;
        font_DelDevFont (name);
    }
#endif

#ifdef _MGFONT_SEF
    uninitialize_scripteasy();
#endif

    return TRUE;
}

/* if font_name == NULL, delete all devfont */
/*
static void del_all_devfonts (void)
{
    DEVFONT* tmp;
    DEVFONT* cur;
    DEVFONT* head;

    head = sb_dev_font_head; 
one_list:
    cur = head;
    while (cur) {
        tmp = cur;
        cur = tmp->next;

        if (!tmp->relationship && tmp->need_unload)
            tmp->font_ops->unload_font_data (tmp->data);
        free (tmp);
    }

    if (head == sb_dev_font_head) {
        head = mb_dev_font_head;
        goto one_list;
    }

    sb_dev_font_head = NULL;
    mb_dev_font_head = NULL;
    nr_sb_dev_fonts = 0;
    nr_mb_dev_fonts = 0;
}
*/


static inline void del_devfont_from_list(const char* font_name, BOOL is_mbc_list) 
{ 
    DEVFONT* head;
    DEVFONT* cur;
    DEVFONT* prev;

    if (is_mbc_list) {
        head = mb_dev_font_head;
        cur = head;
        prev = NULL;
    }
    else {
        head = sb_dev_font_head;
        cur = head;
        prev = NULL;
    }

    while (cur) { 
        if (strcmp (cur->name, font_name) == 0) { 
            /*unload cur->data*/ 
            if (!cur->relationship && cur->need_unload) 
                cur->font_ops->unload_font_data (cur->data); 

            if (cur == head) {
                cur = cur->next;
                free (head);
                head = cur;
                prev = NULL;
            }
            else {
                prev->next = cur->next;
                free (cur); 
                cur = prev->next; 
            }

            if (is_mbc_list)
                nr_mb_dev_fonts--;
            else
                nr_sb_dev_fonts--;
        } 
        else { 
            prev = cur;
            cur = cur->next;
        } 
    } 

    if (is_mbc_list)
        mb_dev_font_head = head;
    else
        sb_dev_font_head = head;
}

static void font_DelDevFont (const char* font_name)
{
    if (font_name == NULL) {
        /*if font_name == NULL, delete all devfont*/
        /*del_all_devfonts ();*/
        return;
    }

    del_devfont_from_list(font_name, TRUE);
    del_devfont_from_list(font_name, FALSE);
}

DEVFONT* GUIAPI LoadDevFontFromFile 
    (const char *devfont_name, const char *file_name)
{
    DEVFONT* devfont = make_devfont (devfont_name, (char*)file_name, TRUE);
    if (devfont == NULL)
        return NULL;

    ADD_DEVFONT_TO_LINKLIST (devfont);
    add_relating_devfonts_to_list (devfont);
    return devfont;
}

void GUIAPI DestroyDynamicDevFont (DEVFONT** devfont)
{
    font_DelDevFont ((*devfont)->name);
    *devfont = NULL;
}

static BOOL init_or_term_specifical_fonts (char* etc_section, BOOL is_unload)
{
    int font_num;
    int i;
    char font_name [LEN_UNIDEVFONT_NAME + 1];
    char font_file [MAX_PATH+1];
    char font_path [MAX_PATH+1];
	char *font_file_name = NULL;
	MEM_RES* memres = NULL;

    char key[12];
    int added_num = 0;

	font_file_name = font_file;

    /*get font number in minigui etc*/
    if (GetMgEtcIntValue (etc_section, "font_number", &font_num) < 0 ) {
        _MG_PRINTF ("FONT>DevFont: can't find font_number in section %s.\n",
                etc_section);
        return FALSE;
    }

    /*add every font*/
    for (i=0; i<font_num; i++) {
        snprintf (key, sizeof(key)-1, "name%d", i);
        if (GetMgEtcValue (etc_section, key, font_name, LEN_UNIDEVFONT_NAME) < 0) {
            _MG_PRINTF ("FONT>DevFont: can't get name of key %s in section %s.\n", 
                    key, etc_section);
            continue;
        }

        snprintf (key, sizeof(key)-1, "fontfile%d", i);
        if (GetMgEtcValue (etc_section, key, font_file_name, MAX_PATH) < 0) {
            _MG_PRINTF ("FONT>DevFont: can't get font_file of key %s in section %s.\n", 
                    key, etc_section);
            continue;
        }

        /*unload devfont*/
        if (is_unload) {
            font_DelDevFont (font_name);
            continue;
        }

        /*add devfont*/
        if ((memres = (MEM_RES*)LoadResource (font_file, RES_TYPE_MEM_RES, 0))) {
            AddDevFont (font_name, memres->data, FALSE);
            added_num ++;
        }
        else {
            /* [DK] Fix Bug #4801, which introduce a absolute path check error in Windows,
             * first to load from sytem res path, else load it directly(relative or absolute path).*/
            if ((0 == mg_path_joint(font_path, MAX_PATH + 1, sysres_get_system_res_path(), font_file)) 
                    && ((AddDevFont (font_name, font_path, TRUE)) == TRUE))
                added_num++;
            else if ((AddDevFont (font_name, font_file, TRUE)) == TRUE)
                added_num++;
        }
    }

    if (is_unload || added_num > 0 || added_num == font_num)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GUIAPI font_InitSpecificalFonts (char* etc_section)
{
    return init_or_term_specifical_fonts (etc_section, FALSE);
}

void GUIAPI font_TermSpecificalFonts (char* etc_section)
{
    init_or_term_specifical_fonts (etc_section, TRUE);
}

#ifndef _MGRM_THREADS

static int init_count = 0;
BOOL GUIAPI InitVectorialFonts (void)
{
    if (init_count == 0)
    {
#if defined (_MGFONT_TTF) || defined (_MGFONT_FT2)
        if (!font_InitFreetypeLibrary ()) {
            _ERR_PRINTF ("FONT>DevFont: Can not init freetype library!\n");
            return FALSE;
        }

        if (!font_InitSpecificalFonts (FONT_ETC_SECTION_NAME_TTF)) {
            _ERR_PRINTF ("FONT>DevFont: Can not init TrueType fonts!\n");
            return FALSE;
        }
#endif
    }

    init_count ++;
    return TRUE;
}

void GUIAPI TermVectorialFonts (void)
{
    if (init_count == 0)
        return;

    init_count --;
    if (init_count > 0)
        return;

#if defined (_MGFONT_TTF) || defined (_MGFONT_FT2)
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_TTF);
    font_TermFreetypeLibrary ();
#endif
}

#endif /* !_MGRM_THREADS */

