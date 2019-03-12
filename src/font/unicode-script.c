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

    return UCHAR_SCRIPT_UNKNOWN;
}

int GUIAPI UCharGetScriptType (Uchar32 ch)
{
    if (ch < EASY_SCRIPTS_RANGE)
        return unicode_script_easy_table[ch];
    else
        return unichar_get_script_bsearch (ch);
}

/* http://unicode.org/iso15924/ */
static const Uint32 iso15924_tags[] =
{
#define PACK(a,b,c,d) ((Uint32)((((Uint8)(a))<<24)|(((Uint8)(b))<<16)|(((Uint8)(c))<<8)|((Uint8)(d))))

    PACK ('Z','y','y','y'), /* UCHAR_SCRIPT_COMMON */
    PACK ('Z','i','n','h'), /* UCHAR_SCRIPT_INHERITED */
    PACK ('A','r','a','b'), /* UCHAR_SCRIPT_ARABIC */
    PACK ('A','r','m','n'), /* UCHAR_SCRIPT_ARMENIAN */
    PACK ('B','e','n','g'), /* UCHAR_SCRIPT_BENGALI */
    PACK ('B','o','p','o'), /* UCHAR_SCRIPT_BOPOMOFO */
    PACK ('C','h','e','r'), /* UCHAR_SCRIPT_CHEROKEE */
    PACK ('C','o','p','t'), /* UCHAR_SCRIPT_COPTIC */
    PACK ('C','y','r','l'), /* UCHAR_SCRIPT_CYRILLIC */
    PACK ('D','s','r','t'), /* UCHAR_SCRIPT_DESERET */
    PACK ('D','e','v','a'), /* UCHAR_SCRIPT_DEVANAGARI */
    PACK ('E','t','h','i'), /* UCHAR_SCRIPT_ETHIOPIC */
    PACK ('G','e','o','r'), /* UCHAR_SCRIPT_GEORGIAN */
    PACK ('G','o','t','h'), /* UCHAR_SCRIPT_GOTHIC */
    PACK ('G','r','e','k'), /* UCHAR_SCRIPT_GREEK */
    PACK ('G','u','j','r'), /* UCHAR_SCRIPT_GUJARATI */
    PACK ('G','u','r','u'), /* UCHAR_SCRIPT_GURMUKHI */
    PACK ('H','a','n','i'), /* UCHAR_SCRIPT_HAN */
    PACK ('H','a','n','g'), /* UCHAR_SCRIPT_HANGUL */
    PACK ('H','e','b','r'), /* UCHAR_SCRIPT_HEBREW */
    PACK ('H','i','r','a'), /* UCHAR_SCRIPT_HIRAGANA */
    PACK ('K','n','d','a'), /* UCHAR_SCRIPT_KANNADA */
    PACK ('K','a','n','a'), /* UCHAR_SCRIPT_KATAKANA */
    PACK ('K','h','m','r'), /* UCHAR_SCRIPT_KHMER */
    PACK ('L','a','o','o'), /* UCHAR_SCRIPT_LAO */
    PACK ('L','a','t','n'), /* UCHAR_SCRIPT_LATIN */
    PACK ('M','l','y','m'), /* UCHAR_SCRIPT_MALAYALAM */
    PACK ('M','o','n','g'), /* UCHAR_SCRIPT_MONGOLIAN */
    PACK ('M','y','m','r'), /* UCHAR_SCRIPT_MYANMAR */
    PACK ('O','g','a','m'), /* UCHAR_SCRIPT_OGHAM */
    PACK ('I','t','a','l'), /* UCHAR_SCRIPT_OLD_ITALIC */
    PACK ('O','r','y','a'), /* UCHAR_SCRIPT_ORIYA */
    PACK ('R','u','n','r'), /* UCHAR_SCRIPT_RUNIC */
    PACK ('S','i','n','h'), /* UCHAR_SCRIPT_SINHALA */
    PACK ('S','y','r','c'), /* UCHAR_SCRIPT_SYRIAC */
    PACK ('T','a','m','l'), /* UCHAR_SCRIPT_TAMIL */
    PACK ('T','e','l','u'), /* UCHAR_SCRIPT_TELUGU */
    PACK ('T','h','a','a'), /* UCHAR_SCRIPT_THAANA */
    PACK ('T','h','a','i'), /* UCHAR_SCRIPT_THAI */
    PACK ('T','i','b','t'), /* UCHAR_SCRIPT_TIBETAN */
    PACK ('C','a','n','s'), /* UCHAR_SCRIPT_CANADIAN_ABORIGINAL */
    PACK ('Y','i','i','i'), /* UCHAR_SCRIPT_YI */
    PACK ('T','g','l','g'), /* UCHAR_SCRIPT_TAGALOG */
    PACK ('H','a','n','o'), /* UCHAR_SCRIPT_HANUNOO */
    PACK ('B','u','h','d'), /* UCHAR_SCRIPT_BUHID */
    PACK ('T','a','g','b'), /* UCHAR_SCRIPT_TAGBANWA */

    /* Unicode-4.0 additions */
    PACK ('B','r','a','i'), /* UCHAR_SCRIPT_BRAILLE */
    PACK ('C','p','r','t'), /* UCHAR_SCRIPT_CYPRIOT */
    PACK ('L','i','m','b'), /* UCHAR_SCRIPT_LIMBU */
    PACK ('O','s','m','a'), /* UCHAR_SCRIPT_OSMANYA */
    PACK ('S','h','a','w'), /* UCHAR_SCRIPT_SHAVIAN */
    PACK ('L','i','n','b'), /* UCHAR_SCRIPT_LINEAR_B */
    PACK ('T','a','l','e'), /* UCHAR_SCRIPT_TAI_LE */
    PACK ('U','g','a','r'), /* UCHAR_SCRIPT_UGARITIC */

    /* Unicode-4.1 additions */
    PACK ('T','a','l','u'), /* UCHAR_SCRIPT_NEW_TAI_LUE */
    PACK ('B','u','g','i'), /* UCHAR_SCRIPT_BUGINESE */
    PACK ('G','l','a','g'), /* UCHAR_SCRIPT_GLAGOLITIC */
    PACK ('T','f','n','g'), /* UCHAR_SCRIPT_TIFINAGH */
    PACK ('S','y','l','o'), /* UCHAR_SCRIPT_SYLOTI_NAGRI */
    PACK ('X','p','e','o'), /* UCHAR_SCRIPT_OLD_PERSIAN */
    PACK ('K','h','a','r'), /* UCHAR_SCRIPT_KHAROSHTHI */

    /* Unicode-5.0 additions */
    PACK ('Z','z','z','z'), /* UCHAR_SCRIPT_UNKNOWN */
    PACK ('B','a','l','i'), /* UCHAR_SCRIPT_BALINESE */
    PACK ('X','s','u','x'), /* UCHAR_SCRIPT_CUNEIFORM */
    PACK ('P','h','n','x'), /* UCHAR_SCRIPT_PHOENICIAN */
    PACK ('P','h','a','g'), /* UCHAR_SCRIPT_PHAGS_PA */
    PACK ('N','k','o','o'), /* UCHAR_SCRIPT_NKO */

    /* Unicode-5.1 additions */
    PACK ('K','a','l','i'), /* UCHAR_SCRIPT_KAYAH_LI */
    PACK ('L','e','p','c'), /* UCHAR_SCRIPT_LEPCHA */
    PACK ('R','j','n','g'), /* UCHAR_SCRIPT_REJANG */
    PACK ('S','u','n','d'), /* UCHAR_SCRIPT_SUNDANESE */
    PACK ('S','a','u','r'), /* UCHAR_SCRIPT_SAURASHTRA */
    PACK ('C','h','a','m'), /* UCHAR_SCRIPT_CHAM */
    PACK ('O','l','c','k'), /* UCHAR_SCRIPT_OL_CHIKI */
    PACK ('V','a','i','i'), /* UCHAR_SCRIPT_VAI */
    PACK ('C','a','r','i'), /* UCHAR_SCRIPT_CARIAN */
    PACK ('L','y','c','i'), /* UCHAR_SCRIPT_LYCIAN */
    PACK ('L','y','d','i'), /* UCHAR_SCRIPT_LYDIAN */

    /* Unicode-5.2 additions */
    PACK ('A','v','s','t'), /* UCHAR_SCRIPT_AVESTAN */
    PACK ('B','a','m','u'), /* UCHAR_SCRIPT_BAMUM */
    PACK ('E','g','y','p'), /* UCHAR_SCRIPT_EGYPTIAN_HIEROGLYPHS */
    PACK ('A','r','m','i'), /* UCHAR_SCRIPT_IMPERIAL_ARAMAIC */
    PACK ('P','h','l','i'), /* UCHAR_SCRIPT_INSCRIPTIONAL_PAHLAVI */
    PACK ('P','r','t','i'), /* UCHAR_SCRIPT_INSCRIPTIONAL_PARTHIAN */
    PACK ('J','a','v','a'), /* UCHAR_SCRIPT_JAVANESE */
    PACK ('K','t','h','i'), /* UCHAR_SCRIPT_KAITHI */
    PACK ('L','i','s','u'), /* UCHAR_SCRIPT_LISU */
    PACK ('M','t','e','i'), /* UCHAR_SCRIPT_MEETEI_MAYEK */
    PACK ('S','a','r','b'), /* UCHAR_SCRIPT_OLD_SOUTH_ARABIAN */
    PACK ('O','r','k','h'), /* UCHAR_SCRIPT_OLD_TURKIC */
    PACK ('S','a','m','r'), /* UCHAR_SCRIPT_SAMARITAN */
    PACK ('L','a','n','a'), /* UCHAR_SCRIPT_TAI_THAM */
    PACK ('T','a','v','t'), /* UCHAR_SCRIPT_TAI_VIET */

    /* Unicode-6.0 additions */
    PACK ('B','a','t','k'), /* UCHAR_SCRIPT_BATAK */
    PACK ('B','r','a','h'), /* UCHAR_SCRIPT_BRAHMI */
    PACK ('M','a','n','d'), /* UCHAR_SCRIPT_MANDAIC */

    /* Unicode-6.1 additions */
    PACK ('C','a','k','m'), /* UCHAR_SCRIPT_CHAKMA */
    PACK ('M','e','r','c'), /* UCHAR_SCRIPT_MEROITIC_CURSIVE */
    PACK ('M','e','r','o'), /* UCHAR_SCRIPT_MEROITIC_HIEROGLYPHS */
    PACK ('P','l','r','d'), /* UCHAR_SCRIPT_MIAO */
    PACK ('S','h','r','d'), /* UCHAR_SCRIPT_SHARADA */
    PACK ('S','o','r','a'), /* UCHAR_SCRIPT_SORA_SOMPENG */
    PACK ('T','a','k','r'), /* UCHAR_SCRIPT_TAKRI */

    /* Unicode 7.0 additions */
    PACK ('B','a','s','s'), /* UCHAR_SCRIPT_BASSA_VAH */
    PACK ('A','g','h','b'), /* UCHAR_SCRIPT_CAUCASIAN_ALBANIAN */
    PACK ('D','u','p','l'), /* UCHAR_SCRIPT_DUPLOYAN */
    PACK ('E','l','b','a'), /* UCHAR_SCRIPT_ELBASAN */
    PACK ('G','r','a','n'), /* UCHAR_SCRIPT_GRANTHA */
    PACK ('K','h','o','j'), /* UCHAR_SCRIPT_KHOJKI*/
    PACK ('S','i','n','d'), /* UCHAR_SCRIPT_KHUDAWADI */
    PACK ('L','i','n','a'), /* UCHAR_SCRIPT_LINEAR_A */
    PACK ('M','a','h','j'), /* UCHAR_SCRIPT_MAHAJANI */
    PACK ('M','a','n','u'), /* UCHAR_SCRIPT_MANICHAEAN */
    PACK ('M','e','n','d'), /* UCHAR_SCRIPT_MENDE_KIKAKUI */
    PACK ('M','o','d','i'), /* UCHAR_SCRIPT_MODI */
    PACK ('M','r','o','o'), /* UCHAR_SCRIPT_MRO */
    PACK ('N','b','a','t'), /* UCHAR_SCRIPT_NABATAEAN */
    PACK ('N','a','r','b'), /* UCHAR_SCRIPT_OLD_NORTH_ARABIAN */
    PACK ('P','e','r','m'), /* UCHAR_SCRIPT_OLD_PERMIC */
    PACK ('H','m','n','g'), /* UCHAR_SCRIPT_PAHAWH_HMONG */
    PACK ('P','a','l','m'), /* UCHAR_SCRIPT_PALMYRENE */
    PACK ('P','a','u','c'), /* UCHAR_SCRIPT_PAU_CIN_HAU */
    PACK ('P','h','l','p'), /* UCHAR_SCRIPT_PSALTER_PAHLAVI */
    PACK ('S','i','d','d'), /* UCHAR_SCRIPT_SIDDHAM */
    PACK ('T','i','r','h'), /* UCHAR_SCRIPT_TIRHUTA */
    PACK ('W','a','r','a'), /* UCHAR_SCRIPT_WARANG_CITI */

    /* Unicode 8.0 additions */
    PACK ('A','h','o','m'), /* UCHAR_SCRIPT_AHOM */
    PACK ('H','l','u','w'), /* UCHAR_SCRIPT_ANATOLIAN_HIEROGLYPHS */
    PACK ('H','a','t','r'), /* UCHAR_SCRIPT_HATRAN */
    PACK ('M','u','l','t'), /* UCHAR_SCRIPT_MULTANI */
    PACK ('H','u','n','g'), /* UCHAR_SCRIPT_OLD_HUNGARIAN */
    PACK ('S','g','n','w'), /* UCHAR_SCRIPT_SIGNWRITING */

    /* Unicode 9.0 additions */
    PACK ('A','d','l','m'), /* UCHAR_SCRIPT_ADLAM */
    PACK ('B','h','k','s'), /* UCHAR_SCRIPT_BHAIKSUKI */
    PACK ('M','a','r','c'), /* UCHAR_SCRIPT_MARCHEN */
    PACK ('N','e','w','a'), /* UCHAR_SCRIPT_NEWA */
    PACK ('O','s','g','e'), /* UCHAR_SCRIPT_OSAGE */
    PACK ('T','a','n','g'), /* UCHAR_SCRIPT_TANGUT */

    /* Unicode 10.0 additions */
    PACK ('G','o','n','m'), /* UCHAR_SCRIPT_MASARAM_GONDI */
    PACK ('N','s','h','u'), /* UCHAR_SCRIPT_NUSHU */
    PACK ('S','o','y','o'), /* UCHAR_SCRIPT_SOYOMBO */
    PACK ('Z','a','n','b'), /* UCHAR_SCRIPT_ZANABAZAR_SQUARE */

    /* Unicode 11.0 additions */
    PACK ('D','o','g','r'), /* UCHAR_SCRIPT_DOGRA */
    PACK ('G','o','n','g'), /* UCHAR_SCRIPT_GUNJALA_GONDI */
    PACK ('R','o','h','g'), /* UCHAR_SCRIPT_HANIFI_ROHINGYA */
    PACK ('M','a','k','a'), /* UCHAR_SCRIPT_MAKASAR */
    PACK ('M','e','d','f'), /* UCHAR_SCRIPT_MEDEFAIDRIN */
    PACK ('S','o','g','o'), /* UCHAR_SCRIPT_OLD_SOGDIAN */
    PACK ('S','o','g','d'), /* UCHAR_SCRIPT_SOGDIAN */

    /* Unicode 12.0 additions */
    PACK ('E','l','y','m'), /* UCHAR_SCRIPT_ELYMAIC */
    PACK ('N','a','n','d'), /* UCHAR_SCRIPT_NANDINAGARI */
    PACK ('H','m','n','p'), /* UCHAR_SCRIPT_NYIAKENG_PUACHUE_HMONG */
    PACK ('W','c','h','o'), /* UCHAR_SCRIPT_WANCHO */
#undef PACK
};

Uint32 GUIAPI UCharScriptTypeToISO15924 (int script)
{
    if (script == UCHAR_SCRIPT_INVALID_CODE)
        return 0;

    if (script < 0 || script >= (int) TABLESIZE (iso15924_tags))
        return 0x5A7A7A7A;

    return iso15924_tags[script];
}

int GUIAPI UCharScriptTypeFromISO15924 (Uint32 iso15924)
{
    unsigned int i;

    if (!iso15924)
        return UCHAR_SCRIPT_INVALID_CODE;

    for (i = 0; i < TABLESIZE (iso15924_tags); i++)
        if (iso15924_tags[i] == iso15924)
            return (int) i;

    return UCHAR_SCRIPT_UNKNOWN;
}

/*
 * https://www.w3.org/TR/css-text-3/#script-tagging
 */
UCharScriptType _unicode_normalize_script(LanguageCode cl,
        UCharScriptType ws)
{
    if (cl == LANGCODE_zh && ws == UCHAR_SCRIPT_INVALID_CODE) {
        return UCHAR_SCRIPT_HAN;
    }
    else if (ws == UCHAR_SCRIPT_BOPOMOFO) {
        return UCHAR_SCRIPT_HAN;
    }
    else if (cl == LANGCODE_ja && ws == UCHAR_SCRIPT_INVALID_CODE) {
        return UCHAR_SCRIPT_KATAKANA;
    }
    else if (cl == LANGCODE_ko && ws == UCHAR_SCRIPT_INVALID_CODE) {
        return UCHAR_SCRIPT_HANGUL;
    }
    else if (cl == LANGCODE_unknown || ws == UCHAR_SCRIPT_INVALID_CODE) {
        return UCHAR_SCRIPT_INVALID_CODE;
    }

    return ws;
}

#endif /* _MGCHARSET_UNICODE */

