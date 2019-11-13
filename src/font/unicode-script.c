///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#ifdef _MGCHARSET_UNICODE

#include "unicode-script-table.h"

#define SCRIPT_TABLE_MIDPOINT (TABLESIZE (unicode_script_table) / 2)

static inline int unichar_get_script_bsearch (Uchar32 ch)
{
    int lower = 0;
    int upper = (int)TABLESIZE (unicode_script_table) - 1;
    static int saved_mid = SCRIPT_TABLE_MIDPOINT;
    int mid = saved_mid;

    do {
        if (ch < unicode_script_table[mid].start)
            upper = mid - 1;
        else if (ch >= unicode_script_table[mid].start + unicode_script_table[mid].chars)
            lower = mid + 1;
        else
            return unicode_script_table[saved_mid = mid].script;

        mid = (lower + upper) / 2;

    } while (lower <= upper);

    return SCRIPT_UNKNOWN;
}

ScriptType GUIAPI UCharGetScriptType (Uchar32 ch)
{
    if (ch < EASY_SCRIPTS_RANGE)
        return (ScriptType)unicode_script_easy_table[ch];
    else
        return (ScriptType)unichar_get_script_bsearch(ch);
}

/* http://unicode.org/iso15924/ */
static const Uint32 iso15924_tags[] =
{
#define PACK(a,b,c,d) ((Uint32)((((Uint8)(a))<<24)|(((Uint8)(b))<<16)|(((Uint8)(c))<<8)|((Uint8)(d))))

    PACK ('Z','y','y','y'), /* SCRIPT_COMMON */
    PACK ('Z','i','n','h'), /* SCRIPT_INHERITED */
    PACK ('A','r','a','b'), /* SCRIPT_ARABIC */
    PACK ('A','r','m','n'), /* SCRIPT_ARMENIAN */
    PACK ('B','e','n','g'), /* SCRIPT_BENGALI */
    PACK ('B','o','p','o'), /* SCRIPT_BOPOMOFO */
    PACK ('C','h','e','r'), /* SCRIPT_CHEROKEE */
    PACK ('C','o','p','t'), /* SCRIPT_COPTIC */
    PACK ('C','y','r','l'), /* SCRIPT_CYRILLIC */
    PACK ('D','s','r','t'), /* SCRIPT_DESERET */
    PACK ('D','e','v','a'), /* SCRIPT_DEVANAGARI */
    PACK ('E','t','h','i'), /* SCRIPT_ETHIOPIC */
    PACK ('G','e','o','r'), /* SCRIPT_GEORGIAN */
    PACK ('G','o','t','h'), /* SCRIPT_GOTHIC */
    PACK ('G','r','e','k'), /* SCRIPT_GREEK */
    PACK ('G','u','j','r'), /* SCRIPT_GUJARATI */
    PACK ('G','u','r','u'), /* SCRIPT_GURMUKHI */
    PACK ('H','a','n','i'), /* SCRIPT_HAN */
    PACK ('H','a','n','g'), /* SCRIPT_HANGUL */
    PACK ('H','e','b','r'), /* SCRIPT_HEBREW */
    PACK ('H','i','r','a'), /* SCRIPT_HIRAGANA */
    PACK ('K','n','d','a'), /* SCRIPT_KANNADA */
    PACK ('K','a','n','a'), /* SCRIPT_KATAKANA */
    PACK ('K','h','m','r'), /* SCRIPT_KHMER */
    PACK ('L','a','o','o'), /* SCRIPT_LAO */
    PACK ('L','a','t','n'), /* SCRIPT_LATIN */
    PACK ('M','l','y','m'), /* SCRIPT_MALAYALAM */
    PACK ('M','o','n','g'), /* SCRIPT_MONGOLIAN */
    PACK ('M','y','m','r'), /* SCRIPT_MYANMAR */
    PACK ('O','g','a','m'), /* SCRIPT_OGHAM */
    PACK ('I','t','a','l'), /* SCRIPT_OLD_ITALIC */
    PACK ('O','r','y','a'), /* SCRIPT_ORIYA */
    PACK ('R','u','n','r'), /* SCRIPT_RUNIC */
    PACK ('S','i','n','h'), /* SCRIPT_SINHALA */
    PACK ('S','y','r','c'), /* SCRIPT_SYRIAC */
    PACK ('T','a','m','l'), /* SCRIPT_TAMIL */
    PACK ('T','e','l','u'), /* SCRIPT_TELUGU */
    PACK ('T','h','a','a'), /* SCRIPT_THAANA */
    PACK ('T','h','a','i'), /* SCRIPT_THAI */
    PACK ('T','i','b','t'), /* SCRIPT_TIBETAN */
    PACK ('C','a','n','s'), /* SCRIPT_CANADIAN_ABORIGINAL */
    PACK ('Y','i','i','i'), /* SCRIPT_YI */
    PACK ('T','g','l','g'), /* SCRIPT_TAGALOG */
    PACK ('H','a','n','o'), /* SCRIPT_HANUNOO */
    PACK ('B','u','h','d'), /* SCRIPT_BUHID */
    PACK ('T','a','g','b'), /* SCRIPT_TAGBANWA */

    /* Unicode-4.0 additions */
    PACK ('B','r','a','i'), /* SCRIPT_BRAILLE */
    PACK ('C','p','r','t'), /* SCRIPT_CYPRIOT */
    PACK ('L','i','m','b'), /* SCRIPT_LIMBU */
    PACK ('O','s','m','a'), /* SCRIPT_OSMANYA */
    PACK ('S','h','a','w'), /* SCRIPT_SHAVIAN */
    PACK ('L','i','n','b'), /* SCRIPT_LINEAR_B */
    PACK ('T','a','l','e'), /* SCRIPT_TAI_LE */
    PACK ('U','g','a','r'), /* SCRIPT_UGARITIC */

    /* Unicode-4.1 additions */
    PACK ('T','a','l','u'), /* SCRIPT_NEW_TAI_LUE */
    PACK ('B','u','g','i'), /* SCRIPT_BUGINESE */
    PACK ('G','l','a','g'), /* SCRIPT_GLAGOLITIC */
    PACK ('T','f','n','g'), /* SCRIPT_TIFINAGH */
    PACK ('S','y','l','o'), /* SCRIPT_SYLOTI_NAGRI */
    PACK ('X','p','e','o'), /* SCRIPT_OLD_PERSIAN */
    PACK ('K','h','a','r'), /* SCRIPT_KHAROSHTHI */

    /* Unicode-5.0 additions */
    PACK ('Z','z','z','z'), /* SCRIPT_UNKNOWN */
    PACK ('B','a','l','i'), /* SCRIPT_BALINESE */
    PACK ('X','s','u','x'), /* SCRIPT_CUNEIFORM */
    PACK ('P','h','n','x'), /* SCRIPT_PHOENICIAN */
    PACK ('P','h','a','g'), /* SCRIPT_PHAGS_PA */
    PACK ('N','k','o','o'), /* SCRIPT_NKO */

    /* Unicode-5.1 additions */
    PACK ('K','a','l','i'), /* SCRIPT_KAYAH_LI */
    PACK ('L','e','p','c'), /* SCRIPT_LEPCHA */
    PACK ('R','j','n','g'), /* SCRIPT_REJANG */
    PACK ('S','u','n','d'), /* SCRIPT_SUNDANESE */
    PACK ('S','a','u','r'), /* SCRIPT_SAURASHTRA */
    PACK ('C','h','a','m'), /* SCRIPT_CHAM */
    PACK ('O','l','c','k'), /* SCRIPT_OL_CHIKI */
    PACK ('V','a','i','i'), /* SCRIPT_VAI */
    PACK ('C','a','r','i'), /* SCRIPT_CARIAN */
    PACK ('L','y','c','i'), /* SCRIPT_LYCIAN */
    PACK ('L','y','d','i'), /* SCRIPT_LYDIAN */

    /* Unicode-5.2 additions */
    PACK ('A','v','s','t'), /* SCRIPT_AVESTAN */
    PACK ('B','a','m','u'), /* SCRIPT_BAMUM */
    PACK ('E','g','y','p'), /* SCRIPT_EGYPTIAN_HIEROGLYPHS */
    PACK ('A','r','m','i'), /* SCRIPT_IMPERIAL_ARAMAIC */
    PACK ('P','h','l','i'), /* SCRIPT_INSCRIPTIONAL_PAHLAVI */
    PACK ('P','r','t','i'), /* SCRIPT_INSCRIPTIONAL_PARTHIAN */
    PACK ('J','a','v','a'), /* SCRIPT_JAVANESE */
    PACK ('K','t','h','i'), /* SCRIPT_KAITHI */
    PACK ('L','i','s','u'), /* SCRIPT_LISU */
    PACK ('M','t','e','i'), /* SCRIPT_MEETEI_MAYEK */
    PACK ('S','a','r','b'), /* SCRIPT_OLD_SOUTH_ARABIAN */
    PACK ('O','r','k','h'), /* SCRIPT_OLD_TURKIC */
    PACK ('S','a','m','r'), /* SCRIPT_SAMARITAN */
    PACK ('L','a','n','a'), /* SCRIPT_TAI_THAM */
    PACK ('T','a','v','t'), /* SCRIPT_TAI_VIET */

    /* Unicode-6.0 additions */
    PACK ('B','a','t','k'), /* SCRIPT_BATAK */
    PACK ('B','r','a','h'), /* SCRIPT_BRAHMI */
    PACK ('M','a','n','d'), /* SCRIPT_MANDAIC */

    /* Unicode-6.1 additions */
    PACK ('C','a','k','m'), /* SCRIPT_CHAKMA */
    PACK ('M','e','r','c'), /* SCRIPT_MEROITIC_CURSIVE */
    PACK ('M','e','r','o'), /* SCRIPT_MEROITIC_HIEROGLYPHS */
    PACK ('P','l','r','d'), /* SCRIPT_MIAO */
    PACK ('S','h','r','d'), /* SCRIPT_SHARADA */
    PACK ('S','o','r','a'), /* SCRIPT_SORA_SOMPENG */
    PACK ('T','a','k','r'), /* SCRIPT_TAKRI */

    /* Unicode 7.0 additions */
    PACK ('B','a','s','s'), /* SCRIPT_BASSA_VAH */
    PACK ('A','g','h','b'), /* SCRIPT_CAUCASIAN_ALBANIAN */
    PACK ('D','u','p','l'), /* SCRIPT_DUPLOYAN */
    PACK ('E','l','b','a'), /* SCRIPT_ELBASAN */
    PACK ('G','r','a','n'), /* SCRIPT_GRANTHA */
    PACK ('K','h','o','j'), /* SCRIPT_KHOJKI*/
    PACK ('S','i','n','d'), /* SCRIPT_KHUDAWADI */
    PACK ('L','i','n','a'), /* SCRIPT_LINEAR_A */
    PACK ('M','a','h','j'), /* SCRIPT_MAHAJANI */
    PACK ('M','a','n','u'), /* SCRIPT_MANICHAEAN */
    PACK ('M','e','n','d'), /* SCRIPT_MENDE_KIKAKUI */
    PACK ('M','o','d','i'), /* SCRIPT_MODI */
    PACK ('M','r','o','o'), /* SCRIPT_MRO */
    PACK ('N','b','a','t'), /* SCRIPT_NABATAEAN */
    PACK ('N','a','r','b'), /* SCRIPT_OLD_NORTH_ARABIAN */
    PACK ('P','e','r','m'), /* SCRIPT_OLD_PERMIC */
    PACK ('H','m','n','g'), /* SCRIPT_PAHAWH_HMONG */
    PACK ('P','a','l','m'), /* SCRIPT_PALMYRENE */
    PACK ('P','a','u','c'), /* SCRIPT_PAU_CIN_HAU */
    PACK ('P','h','l','p'), /* SCRIPT_PSALTER_PAHLAVI */
    PACK ('S','i','d','d'), /* SCRIPT_SIDDHAM */
    PACK ('T','i','r','h'), /* SCRIPT_TIRHUTA */
    PACK ('W','a','r','a'), /* SCRIPT_WARANG_CITI */

    /* Unicode 8.0 additions */
    PACK ('A','h','o','m'), /* SCRIPT_AHOM */
    PACK ('H','l','u','w'), /* SCRIPT_ANATOLIAN_HIEROGLYPHS */
    PACK ('H','a','t','r'), /* SCRIPT_HATRAN */
    PACK ('M','u','l','t'), /* SCRIPT_MULTANI */
    PACK ('H','u','n','g'), /* SCRIPT_OLD_HUNGARIAN */
    PACK ('S','g','n','w'), /* SCRIPT_SIGNWRITING */

    /* Unicode 9.0 additions */
    PACK ('A','d','l','m'), /* SCRIPT_ADLAM */
    PACK ('B','h','k','s'), /* SCRIPT_BHAIKSUKI */
    PACK ('M','a','r','c'), /* SCRIPT_MARCHEN */
    PACK ('N','e','w','a'), /* SCRIPT_NEWA */
    PACK ('O','s','g','e'), /* SCRIPT_OSAGE */
    PACK ('T','a','n','g'), /* SCRIPT_TANGUT */

    /* Unicode 10.0 additions */
    PACK ('G','o','n','m'), /* SCRIPT_MASARAM_GONDI */
    PACK ('N','s','h','u'), /* SCRIPT_NUSHU */
    PACK ('S','o','y','o'), /* SCRIPT_SOYOMBO */
    PACK ('Z','a','n','b'), /* SCRIPT_ZANABAZAR_SQUARE */

    /* Unicode 11.0 additions */
    PACK ('D','o','g','r'), /* SCRIPT_DOGRA */
    PACK ('G','o','n','g'), /* SCRIPT_GUNJALA_GONDI */
    PACK ('R','o','h','g'), /* SCRIPT_HANIFI_ROHINGYA */
    PACK ('M','a','k','a'), /* SCRIPT_MAKASAR */
    PACK ('M','e','d','f'), /* SCRIPT_MEDEFAIDRIN */
    PACK ('S','o','g','o'), /* SCRIPT_OLD_SOGDIAN */
    PACK ('S','o','g','d'), /* SCRIPT_SOGDIAN */

    /* Unicode 12.0 additions */
    PACK ('E','l','y','m'), /* SCRIPT_ELYMAIC */
    PACK ('N','a','n','d'), /* SCRIPT_NANDINAGARI */
    PACK ('H','m','n','p'), /* SCRIPT_NYIAKENG_PUACHUE_HMONG */
    PACK ('W','c','h','o'), /* SCRIPT_WANCHO */
#undef PACK
};

Uint32 GUIAPI ScriptTypeToISO15924 (ScriptType script)
{
    if (script == SCRIPT_INVALID_CODE)
        return 0;

    if (script < 0 || script >= (ScriptType) TABLESIZE (iso15924_tags))
        return 0x5A7A7A7A;

    return iso15924_tags[script];
}

ScriptType GUIAPI ScriptTypeFromISO15924 (Uint32 iso15924)
{
    unsigned int i;

    if (!iso15924)
        return SCRIPT_INVALID_CODE;

    for (i = 0; i < TABLESIZE (iso15924_tags); i++)
        if (iso15924_tags[i] == iso15924)
            return (ScriptType) i;

    return SCRIPT_UNKNOWN;
}

#include "script-language-table.inc"

static int comp_lang(const void *k1, const void *k2)
{
   ScriptTypeForLang *sl1 = (ScriptTypeForLang*) k1;
   ScriptTypeForLang *sl2 = (ScriptTypeForLang*) k2;
   return strcmp(sl1->lang, sl2->lang);
}

const ScriptType* GUIAPI GetScriptsForLang(const char* lang_name,
        LanguageCode* lang_code, int* nr_scripts)
{
    const ScriptType* scripts;
    const ScriptTypeForLang* matched;

    matched = bsearch(lang_name, _script_for_lang,
                TABLESIZE(_script_for_lang), sizeof (_script_for_lang[0]),
                comp_lang);

    if (matched) {
        if (lang_code)
            *lang_code = LanguageCodeFromISO639s1Code(matched->lang);
        scripts = matched->scripts;
        if (nr_scripts) {
            int i;

            for (i = 0; i < TABLESIZE(_script_for_lang->scripts); i++) {
                if (_script_for_lang->scripts[i] == 0)
                    break;
            }

            *nr_scripts = i;
        }

        return scripts;
    }

    if (lang_code)
        *lang_code = LANGCODE_unknown;

    return NULL;
}

BOOL __mg_language_includes_script(LanguageCode lc, ScriptType script)
{
    const ScriptType *scripts;
    int num_scripts, j;

    /* copied from the one in pango-script.c */
#define REAL_SCRIPT(script) \
    ((script) > SCRIPT_INHERITED && (script) != SCRIPT_UNKNOWN)

    if (!REAL_SCRIPT (script))
        return TRUE;

#undef REAL_SCRIPT

    scripts = GetScriptsForLang (LanguageCodeToISO639s1(lc), &lc, &num_scripts);
    if (!scripts)
        return TRUE;

    for (j = 0; j < num_scripts; j++)
        if (scripts[j] == script)
            return TRUE;

    return FALSE;
}

/*
 * https://www.w3.org/TR/css-text-3/#script-tagging
 */
ScriptType GUIAPI NormalizeScriptType(LanguageCode cl,
        ScriptType ws)
{
    if (ws == SCRIPT_BOPOMOFO) {
        return SCRIPT_HAN;
    }

    if (ws != SCRIPT_INVALID_CODE)
        return ws;

    switch (cl) {
    case LANGCODE_zh:
        return SCRIPT_HAN;
    case LANGCODE_ja:
        return SCRIPT_HAN;
    case LANGCODE_ko:
        return SCRIPT_HANGUL;
    case LANGCODE_unknown:
        return SCRIPT_COMMON;
    default:
        break;
    }

    return ws;
}

typedef enum {
    HORIZONTAL_DIRECTION_LTR,
    HORIZONTAL_DIRECTION_RTL,
    HORIZONTAL_DIRECTION_WEAK,
} HorizontalDirection;

typedef enum {
    VERTICAL_DIRECTION_NONE,
    VERTICAL_DIRECTION_TTB,
    VERTICAL_DIRECTION_BTT,
} VerticalDirection;

typedef struct {
    /* GlyphRunDir */
    Uint8 horiz_dir;        /* Orientation in horizontal context */

    /* VerticalDirection */
    Uint8 vert_dir;         /* Orientation in vertical context */

    /* GlyphGravity */
    Uint8 preferred_gravity; /* Preferred context gravity */

    /* BOOL */
    Uint8 wide;             /* Whether script is mostly wide.
                             * Wide characters are upright (ie.
                             * not rotated) in foreign context */
} ScriptTypeProperties;

#define NONE VERTICAL_DIRECTION_NONE
#define TTB  VERTICAL_DIRECTION_TTB
#define BTT  VERTICAL_DIRECTION_BTT

#define LTR  HORIZONTAL_DIRECTION_LTR
#define RTL  HORIZONTAL_DIRECTION_RTL
#define WEAK HORIZONTAL_DIRECTION_WEAK

#define S GLYPH_GRAVITY_SOUTH
#define E GLYPH_GRAVITY_EAST
#define W GLYPH_GRAVITY_WEST

#define UNKNOWN_SCRIPT_PROPERTY \
    {LTR, NONE, S, FALSE}

const ScriptTypeProperties script_properties[] =
{
    {LTR, NONE, S, FALSE},    /* Zyyy */
    {LTR, NONE, S, FALSE},    /* Qaai */
    {RTL, NONE, S, FALSE},    /* Arab */
    {LTR, NONE, S, FALSE},    /* Armn */
    {LTR, NONE, S, FALSE},    /* Beng */
    {LTR, TTB,  E, TRUE },    /* Bopo */
    {LTR, NONE, S, FALSE},    /* Cher */
    {LTR, NONE, S, FALSE},    /* Qaac */
    {LTR, NONE, S, FALSE},    /* Cyrl (Cyrs) */
    {LTR, NONE, S, FALSE},    /* Dsrt */
    {LTR, NONE, S, FALSE},    /* Deva */
    {LTR, NONE, S, FALSE},    /* Ethi */
    {LTR, NONE, S, FALSE},    /* Geor (Geon, Geoa) */
    {LTR, NONE, S, FALSE},    /* Goth */
    {LTR, NONE, S, FALSE},    /* Grek */
    {LTR, NONE, S, FALSE},    /* Gujr */
    {LTR, NONE, S, FALSE},    /* Guru */
    {LTR, TTB,  E, TRUE },    /* Hani */
    {LTR, TTB,  E, TRUE },    /* Hang */
    {RTL, NONE, S, FALSE},    /* Hebr */
    {LTR, TTB,  E, TRUE },    /* Hira */
    {LTR, NONE, S, FALSE},    /* Knda */
    {LTR, TTB,  E, TRUE },    /* Kana */
    {LTR, NONE, S, FALSE},    /* Khmr */
    {LTR, NONE, S, FALSE},    /* Laoo */
    {LTR, NONE, S, FALSE},    /* Latn (Latf, Latg) */
    {LTR, NONE, S, FALSE},    /* Mlym */
    {WEAK,TTB,  W, FALSE},    /* Mong */
    {LTR, NONE, S, FALSE},    /* Mymr */
    {LTR, BTT,  W, FALSE},    /* Ogam */
    {LTR, NONE, S, FALSE},    /* Ital */
    {LTR, NONE, S, FALSE},    /* Orya */
    {LTR, NONE, S, FALSE},    /* Runr */
    {LTR, NONE, S, FALSE},    /* Sinh */
    {RTL, NONE, S, FALSE},    /* Syrc (Syrj, Syrn, Syre) */
    {LTR, NONE, S, FALSE},    /* Taml */
    {LTR, NONE, S, FALSE},    /* Telu */
    {RTL, NONE, S, FALSE},    /* Thaa */
    {LTR, NONE, S, FALSE},    /* Thai */
    {LTR, NONE, S, FALSE},    /* Tibt */
    {LTR, NONE, S, FALSE},    /* Cans */
    {LTR, TTB,  S, TRUE },    /* Yiii */
    {LTR, NONE, S, FALSE},    /* Tglg */
    {LTR, NONE, S, FALSE},    /* Hano */
    {LTR, NONE, S, FALSE},    /* Buhd */
    {LTR, NONE, S, FALSE},    /* Tagb */

    /* Unicode-4.0 additions */
    {LTR, NONE, S, FALSE},    /* Brai */
    {RTL, NONE, S, FALSE},    /* Cprt */
    {LTR, NONE, S, FALSE},    /* Limb */
    {LTR, NONE, S, FALSE},    /* Osma */
    {LTR, NONE, S, FALSE},    /* Shaw */
    {LTR, NONE, S, FALSE},    /* Linb */
    {LTR, NONE, S, FALSE},    /* Tale */
    {LTR, NONE, S, FALSE},    /* Ugar */

    /* Unicode-4.1 additions */
    {LTR, NONE, S, FALSE},    /* Talu */
    {LTR, NONE, S, FALSE},    /* Bugi */
    {LTR, NONE, S, FALSE},    /* Glag */
    {LTR, NONE, S, FALSE},    /* Tfng */
    {LTR, NONE, S, FALSE},    /* Sylo */
    {LTR, NONE, S, FALSE},    /* Xpeo */
    {LTR, NONE, S, FALSE},    /* Khar */

    /* Unicode-5.0 additions */
    {LTR, NONE, S, FALSE},    /* Zzzz */
    {LTR, NONE, S, FALSE},    /* Bali */
    {LTR, NONE, S, FALSE},    /* Xsux */
    {RTL, NONE, S, FALSE},    /* Phnx */
    {LTR, NONE, S, FALSE},    /* Phag */
    {RTL, NONE, S, FALSE},    /* Nkoo */

    /* Unicode-5.1 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Kali */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lepc */
    UNKNOWN_SCRIPT_PROPERTY,  /* Rjng */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sund */
    UNKNOWN_SCRIPT_PROPERTY,  /* Saur */
    UNKNOWN_SCRIPT_PROPERTY,  /* Cham */
    UNKNOWN_SCRIPT_PROPERTY,  /* Olck */
    UNKNOWN_SCRIPT_PROPERTY,  /* Vaii */
    UNKNOWN_SCRIPT_PROPERTY,  /* Cari */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lyci */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lydi */

    /* Unicode-5.2 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Avst */
    UNKNOWN_SCRIPT_PROPERTY,  /* Bamu */
    UNKNOWN_SCRIPT_PROPERTY,  /* Egyp */
    UNKNOWN_SCRIPT_PROPERTY,  /* Armi */
    UNKNOWN_SCRIPT_PROPERTY,  /* Phli */
    UNKNOWN_SCRIPT_PROPERTY,  /* Prti */
    UNKNOWN_SCRIPT_PROPERTY,  /* Java */
    UNKNOWN_SCRIPT_PROPERTY,  /* Kthi */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lisu */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mtei */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sarb */
    UNKNOWN_SCRIPT_PROPERTY,  /* Orkh */
    UNKNOWN_SCRIPT_PROPERTY,  /* Samr */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lana */
    UNKNOWN_SCRIPT_PROPERTY,  /* Tavt */

    /* Unicode-6.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Batk */
    UNKNOWN_SCRIPT_PROPERTY,  /* Brah */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mand */

    /* Unicode-6.1 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Cakm */
    UNKNOWN_SCRIPT_PROPERTY,  /* Merc */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mero */
    UNKNOWN_SCRIPT_PROPERTY,  /* Plrd */
    UNKNOWN_SCRIPT_PROPERTY,  /* Shrd */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sora */
    UNKNOWN_SCRIPT_PROPERTY,  /* Takr */

    /* Unicode 7.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Bass */
    UNKNOWN_SCRIPT_PROPERTY,  /* Aghb */
    UNKNOWN_SCRIPT_PROPERTY,  /* Dupl */
    UNKNOWN_SCRIPT_PROPERTY,  /* Elba */
    UNKNOWN_SCRIPT_PROPERTY,  /* Gran */
    UNKNOWN_SCRIPT_PROPERTY,  /* Khoj */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sind */
    UNKNOWN_SCRIPT_PROPERTY,  /* Lina */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mahj */
    UNKNOWN_SCRIPT_PROPERTY,  /* Manu */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mend */
    UNKNOWN_SCRIPT_PROPERTY,  /* Modi */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mroo */
    UNKNOWN_SCRIPT_PROPERTY,  /* Nbat */
    UNKNOWN_SCRIPT_PROPERTY,  /* Narb */
    UNKNOWN_SCRIPT_PROPERTY,  /* Perm */
    UNKNOWN_SCRIPT_PROPERTY,  /* Hmng */
    UNKNOWN_SCRIPT_PROPERTY,  /* Palm */
    UNKNOWN_SCRIPT_PROPERTY,  /* Pauc */
    UNKNOWN_SCRIPT_PROPERTY,  /* Phlp */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sidd */
    UNKNOWN_SCRIPT_PROPERTY,  /* Tirh */
    UNKNOWN_SCRIPT_PROPERTY,  /* Wara */

    /* Unicode 8.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Ahom */
    UNKNOWN_SCRIPT_PROPERTY,  /* Hluw */
    UNKNOWN_SCRIPT_PROPERTY,  /* Hatr */
    UNKNOWN_SCRIPT_PROPERTY,  /* Mult */
    UNKNOWN_SCRIPT_PROPERTY,  /* Hung */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sgnw */

    /* Unicode 9.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Adlm */
    UNKNOWN_SCRIPT_PROPERTY,  /* Bhks */
    UNKNOWN_SCRIPT_PROPERTY,  /* Marc */
    UNKNOWN_SCRIPT_PROPERTY,  /* Newa */
    UNKNOWN_SCRIPT_PROPERTY,  /* Osge */
    UNKNOWN_SCRIPT_PROPERTY,  /* Tang */

    /* Unicode 10.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Gonm */
    UNKNOWN_SCRIPT_PROPERTY,  /* Nshu */
    UNKNOWN_SCRIPT_PROPERTY,  /* Soyo */
    UNKNOWN_SCRIPT_PROPERTY,  /* Zanb */

    /* Unicode 11.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Dogr */
    UNKNOWN_SCRIPT_PROPERTY,  /* Gong */
    UNKNOWN_SCRIPT_PROPERTY,  /* Rohg */
    UNKNOWN_SCRIPT_PROPERTY,  /* Maka */
    UNKNOWN_SCRIPT_PROPERTY,  /* Medf */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sogo */
    UNKNOWN_SCRIPT_PROPERTY,  /* Sogd */

    /* Unicode 12.0 additions */
    UNKNOWN_SCRIPT_PROPERTY,  /* Elymaic */
    UNKNOWN_SCRIPT_PROPERTY,  /* Nandinagari */
    UNKNOWN_SCRIPT_PROPERTY,  /* Nyiakeng Puachue Hmong */
    UNKNOWN_SCRIPT_PROPERTY,  /* Wancho */
};

#undef NONE
#undef TTB
#undef BTT

#undef LTR
#undef RTL
#undef WEAK

#undef S
#undef E
#undef N
#undef W

static inline ScriptTypeProperties get_script_properties (ScriptType script)
{
    if (script < 0 || (size_t)script >= TABLESIZE (script_properties))
        return script_properties[0];

    return script_properties[script];
}

GlyphGravity ScriptGetGlyphGravity (ScriptType script, BOOL vertical,
        GlyphGravity base_gravity, GlyphGravityPolicy hint)
{
    ScriptTypeProperties props = get_script_properties (script);

    if (base_gravity == GLYPH_GRAVITY_AUTO)
        base_gravity = props.preferred_gravity;

    return ScriptGetGlyphGravityForWide (script, vertical, props.wide,
            base_gravity, hint);
}

GlyphGravity ScriptGetGlyphGravityForWide (ScriptType script, BOOL vertical,
        BOOL wide, GlyphGravity base_gravity, GlyphGravityPolicy hint)
{
    ScriptTypeProperties props = get_script_properties (script);

    if (base_gravity == GLYPH_GRAVITY_AUTO)
        base_gravity = props.preferred_gravity;

    /* Everything is designed such that a system with no vertical support
     * renders everything correctly horizontally.  So, if not in a vertical
     * gravity, base and resolved gravities are always the same.
     *
     * Wide characters are always upright.
     */
    if (!vertical || wide) {
        return base_gravity;
    }

    /* If here, we have a narrow character in a vertical gravity setting.
     * Resolve depending on the hint.
     */
    switch (hint) {
    default:
    case GLYPH_GRAVITY_POLICY_NATURAL:
        if (props.vert_dir == VERTICAL_DIRECTION_NONE) {
            return GLYPH_GRAVITY_SOUTH;
        }
        if (((base_gravity   == GLYPH_GRAVITY_EAST) ^
                    (props.vert_dir == VERTICAL_DIRECTION_BTT))
                || props.wide)
            return GLYPH_GRAVITY_SOUTH;
        else {
            return GLYPH_GRAVITY_NORTH;
        }

    case GLYPH_GRAVITY_POLICY_STRONG:
        return base_gravity;

    case GLYPH_GRAVITY_POLICY_LINE:
        if ((base_gravity    == GLYPH_GRAVITY_EAST) ^
                (props.horiz_dir == HORIZONTAL_DIRECTION_RTL))
            return GLYPH_GRAVITY_SOUTH;
        else
            return GLYPH_GRAVITY_NORTH;

    case GLYPH_GRAVITY_POLICY_MIXED:
        // CSS3: horizontal-only scripts are typeset sideways
        if (props.vert_dir != VERTICAL_DIRECTION_NONE)
            return GLYPH_GRAVITY_SOUTH;
        else
            return GLYPH_GRAVITY_EAST;
    }
}
#endif /* _MGCHARSET_UNICODE */

