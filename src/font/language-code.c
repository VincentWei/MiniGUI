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

#include "common.h"
#include "minigui.h"
#include "gdi.h"

#ifdef _MGCHARSET_UNICODE

static const Uint16 iso639_1_codes[] =
{
#define PACK(a,b) ((Uint16)((((Uint8)(a))<<8)|((Uint8)(b))))
    PACK('a', 'a'), //  Afar
    PACK('a', 'b'), //  Abkhazian
    PACK('a', 'f'), //  Afrikaans
    PACK('a', 'm'), //  Amharic
    PACK('a', 'r'), //  Arabic
    PACK('a', 's'), //  Assamese
    PACK('a', 'y'), //  Aymara
    PACK('a', 'z'), //  Azerbaijani
    PACK('b', 'a'), //  Bashkir
    PACK('b', 'e'), //  Byelorussian
    PACK('b', 'g'), //  Bulgarian
    PACK('b', 'h'), //  Bihari
    PACK('b', 'i'), //  Bislama
    PACK('b', 'n'), //  Bengali
    PACK('b', 'o'), //  Tibetan
    PACK('b', 'r'), //  Breton
    PACK('c', 'a'), //  Catalan
    PACK('c', 'o'), //  Corsican
    PACK('c', 's'), //  Czech
    PACK('c', 'y'), //  Welch
    PACK('d', 'a'), //  Danish
    PACK('d', 'e'), //  German
    PACK('d', 'v'), //  Divehi
    PACK('d', 'z'), //  Bhutani
    PACK('e', 'l'), //  Greek
    PACK('e', 'n'), //  English
    PACK('e', 'o'), //  Esperanto
    PACK('e', 's'), //  Spanish
    PACK('e', 't'), //  Estonian
    PACK('e', 'u'), //  Basque
    PACK('f', 'a'), //  Persian
    PACK('f', 'i'), //  Finnish
    PACK('f', 'j'), //  Fiji
    PACK('f', 'o'), //  Faeroese
    PACK('f', 'r'), //  French
    PACK('f', 'y'), //  Frisian
    PACK('g', 'a'), //  Irish
    PACK('g', 'd'), //  Scots Gaelic
    PACK('g', 'l'), //  Galician
    PACK('g', 'n'), //  Guarani
    PACK('g', 'u'), //  Gujarati
    PACK('h', 'a'), //  Hausa
    PACK('h', 'i'), //  Hindi
    PACK('h', 'e'), //  Hebrew
    PACK('h', 'r'), //  Croatian
    PACK('h', 'u'), //  Hungarian
    PACK('h', 'y'), //  Armenian
    PACK('i', 'a'), //  Interlingua
    PACK('i', 'd'), //  Indonesian
    PACK('i', 'e'), //  Interlingue
    PACK('i', 'k'), //  Inupiak
    PACK('i', 'n'), //  former Indonesian
    PACK('i', 's'), //  Icelandic
    PACK('i', 't'), //  Italian
    PACK('i', 'u'), //  Inuktitut (Eskimo)
    PACK('i', 'w'), //  former Hebrew
    PACK('j', 'a'), //  Japanese
    PACK('j', 'i'), //  former Yiddish
    PACK('j', 'w'), //  Javanese
    PACK('k', 'a'), //  Georgian
    PACK('k', 'k'), //  Kazakh
    PACK('k', 'l'), //  Greenlandic
    PACK('k', 'm'), //  Cambodian
    PACK('k', 'n'), //  Kannada
    PACK('k', 'o'), //  Korean
    PACK('k', 's'), //  Kashmiri
    PACK('k', 'u'), //  Kurdish
    PACK('k', 'y'), //  Kirghiz
    PACK('l', 'a'), //  Latin
    PACK('l', 'n'), //  Lingala
    PACK('l', 'o'), //  Laothian
    PACK('l', 't'), //  Lithuanian
    PACK('l', 'v'), //  Latvian, Lettish
    PACK('m', 'g'), //  Malagasy
    PACK('m', 'i'), //  Maori
    PACK('m', 'k'), //  Macedonian
    PACK('m', 'l'), //  Malayalam
    PACK('m', 'n'), //  Mongolian
    PACK('m', 'o'), //  Moldavian
    PACK('m', 'r'), //  Marathi
    PACK('m', 's'), //  Malay
    PACK('m', 't'), //  Maltese
    PACK('m', 'y'), //  Burmese
    PACK('n', 'a'), //  Nauru
    PACK('n', 'e'), //  Nepali
    PACK('n', 'l'), //  Dutch
    PACK('n', 'o'), //  Norwegian
    PACK('o', 'c'), //  Occitan
    PACK('o', 'm'), //  (Afan) Oromo
    PACK('o', 'r'), //  Oriya
    PACK('p', 'a'), //  Punjabi
    PACK('p', 'l'), //  Polish
    PACK('p', 's'), //  Pashto, Pushto
    PACK('p', 't'), //  Portuguese
    PACK('q', 'u'), //  Quechua
    PACK('r', 'm'), //  Rhaeto-Romance
    PACK('r', 'n'), //  Kirundi
    PACK('r', 'o'), //  Romanian
    PACK('r', 'u'), //  Russian
    PACK('r', 'w'), //  Kinyarwanda
    PACK('s', 'a'), //  Sanskrit
    PACK('s', 'd'), //  Sindhi
    PACK('s', 'g'), //  Sangro
    PACK('s', 'h'), //  Serbo-Croatian
    PACK('s', 'i'), //  Singhalese
    PACK('s', 'k'), //  Slovak
    PACK('s', 'l'), //  Slovenian
    PACK('s', 'm'), //  Samoan
    PACK('s', 'n'), //  Shona
    PACK('s', 'o'), //  Somali
    PACK('s', 'q'), //  Albanian
    PACK('s', 'r'), //  Serbian
    PACK('s', 's'), //  Siswati
    PACK('s', 't'), //  Sesotho
    PACK('s', 'u'), //  Sudanese
    PACK('s', 'v'), //  Swedish
    PACK('s', 'w'), //  Swahili
    PACK('t', 'a'), //  Tamil
    PACK('t', 'e'), //  Tegulu
    PACK('t', 'g'), //  Tajik
    PACK('t', 'h'), //  Thai
    PACK('t', 'i'), //  Tigrinya
    PACK('t', 'k'), //  Turkmen
    PACK('t', 'l'), //  Tagalog
    PACK('t', 'n'), //  Setswana
    PACK('t', 'o'), //  Tonga
    PACK('t', 'r'), //  Turkish
    PACK('t', 's'), //  Tsonga
    PACK('t', 't'), //  Tatar
    PACK('t', 'w'), //  Twi
    PACK('u', 'g'), //  Uigur
    PACK('u', 'k'), //  Ukrainian
    PACK('u', 'r'), //  Urdu
    PACK('u', 'z'), //  Uzbek
    PACK('v', 'i'), //  Vietnamese
    PACK('v', 'o'), //  Volapuk
    PACK('w', 'o'), //  Wolof
    PACK('x', 'h'), //  Xhosa
    PACK('y', 'i'), //  Yiddish
    PACK('y', 'o'), //  Yoruba
    PACK('z', 'a'), //  Zhuang
    PACK('z', 'h'), //  Chinese
    PACK('z', 'u'), //  Zulu
#undef PACK
};

LanguageCode GUIAPI LanguageCodeFromISO639s1 (Uint16 iso639_1)
{
    unsigned int lower = 0;
    unsigned int upper = TABLESIZE (iso639_1_codes) - 1;
    unsigned int mid = TABLESIZE (iso639_1_codes) / 2;

    if (!iso639_1)
        return LANGCODE_unknown;

    do {
        if (iso639_1 < iso639_1_codes[mid])
            upper = mid - 1;
        else if (iso639_1 > iso639_1_codes[mid])
            lower = mid + 1;
        else
            return (LanguageCode)mid;

        mid = (lower + upper) / 2;
    } while (lower <= upper);

    return LANGCODE_unknown;
}

static const char* iso639_1_codes_str[] =
{
    "aa", //  Afar
    "ab", //  Abkhazian
    "af", //  Afrikaans
    "am", //  Amharic
    "ar", //  Arabic
    "as", //  Assamese
    "ay", //  Aymara
    "az", //  Azerbaijani
    "ba", //  Bashkir
    "be", //  Byelorussian
    "bg", //  Bulgarian
    "bh", //  Bihari
    "bi", //  Bislama
    "bn", //  Bengali
    "bo", //  Tibetan
    "br", //  Breton
    "ca", //  Catalan
    "co", //  Corsican
    "cs", //  Czech
    "cy", //  Welch
    "da", //  Danish
    "de", //  German
    "dv", //  Divehi
    "dz", //  Bhutani
    "el", //  Greek
    "en", //  English
    "eo", //  Esperanto
    "es", //  Spanish
    "et", //  Estonian
    "eu", //  Basque
    "fa", //  Persian
    "fi", //  Finnish
    "fj", //  Fiji
    "fo", //  Faeroese
    "fr", //  French
    "fy", //  Frisian
    "ga", //  Irish
    "gd", //  Scots Gaelic
    "gl", //  Galician
    "gn", //  Guarani
    "gu", //  Gujarati
    "ha", //  Hausa
    "hi", //  Hindi
    "he", //  Hebrew
    "hr", //  Croatian
    "hu", //  Hungarian
    "hy", //  Armenian
    "ia", //  Interlingua
    "id", //  Indonesian
    "ie", //  Interlingue
    "ik", //  Inupiak
    "in", //  former Indonesian
    "is", //  Icelandic
    "it", //  Italian
    "iu", //  Inuktitut (Eskimo)
    "iw", //  former Hebrew
    "ja", //  Japanese
    "ji", //  former Yiddish
    "jw", //  Javanese
    "ka", //  Georgian
    "kk", //  Kazakh
    "kl", //  Greenlandic
    "km", //  Cambodian
    "kn", //  Kannada
    "ko", //  Korean
    "ks", //  Kashmiri
    "ku", //  Kurdish
    "ky", //  Kirghiz
    "la", //  Latin
    "ln", //  Lingala
    "lo", //  Laothian
    "lt", //  Lithuanian
    "lv", //  Latvian, Lettish
    "mg", //  Malagasy
    "mi", //  Maori
    "mk", //  Macedonian
    "ml", //  Malayalam
    "mn", //  Mongolian
    "mo", //  Moldavian
    "mr", //  Marathi
    "ms", //  Malay
    "mt", //  Maltese
    "my", //  Burmese
    "na", //  Nauru
    "ne", //  Nepali
    "nl", //  Dutch
    "no", //  Norwegian
    "oc", //  Occitan
    "om", //  (Afan) Oromo
    "or", //  Oriya
    "pa", //  Punjabi
    "pl", //  Polish
    "ps", //  Pashto, Pushto
    "pt", //  Portuguese
    "qu", //  Quechua
    "rm", //  Rhaeto-Romance
    "rn", //  Kirundi
    "ro", //  Romanian
    "ru", //  Russian
    "rw", //  Kinyarwanda
    "sa", //  Sanskrit
    "sd", //  Sindhi
    "sg", //  Sangro
    "sh", //  Serbo-Croatian
    "si", //  Singhalese
    "sk", //  Slovak
    "sl", //  Slovenian
    "sm", //  Samoan
    "sn", //  Shona
    "so", //  Somali
    "sq", //  Albanian
    "sr", //  Serbian
    "ss", //  Siswati
    "st", //  Sesotho
    "su", //  Sudanese
    "sv", //  Swedish
    "sw", //  Swahili
    "ta", //  Tamil
    "te", //  Tegulu
    "tg", //  Tajik
    "th", //  Thai
    "ti", //  Tigrinya
    "tk", //  Turkmen
    "tl", //  Tagalog
    "tn", //  Setswana
    "to", //  Tonga
    "tr", //  Turkish
    "ts", //  Tsonga
    "tt", //  Tatar
    "tw", //  Twi
    "ug", //  Uigur
    "uk", //  Ukrainian
    "ur", //  Urdu
    "uz", //  Uzbek
    "vi", //  Vietnamese
    "vo", //  Volapuk
    "wo", //  Wolof
    "xh", //  Xhosa
    "yi", //  Yiddish
    "yo", //  Yoruba
    "za", //  Zhuang
    "zh", //  Chinese
    "zu", //  Zulu
};

const char* GUIAPI LanguageCodeToISO639s1 (LanguageCode lc)
{
    switch (lc) {
    case LANGCODE_unknown:
        return "";
    default:
        return iso639_1_codes_str[lc];
    }

    return "";
}

static const LanguageCode sample_language_of_script[] = {
    LANGCODE_unknown,     /* SCRIPT_COMMON */
    LANGCODE_unknown,     /* SCRIPT_INHERITED */
    LANGCODE_ar,   /* SCRIPT_ARABIC */
    LANGCODE_hy,   /* SCRIPT_ARMENIAN */
    LANGCODE_bn,   /* SCRIPT_BENGALI */
    LANGCODE_zh,   /* SCRIPT_BOPOMOFO: Used primarily in Taiwan, but not part of the standard zh-tw orthography */
    LANGCODE_unknown,     /* SCRIPT_CHEROKEE; no ISO639-1 code; ISO639-2 code: chr  */
    LANGCODE_unknown,     /* SCRIPT_COPTIC; no ISO639-1 code; ISO639-2 code: cop */
    LANGCODE_ru,   /* SCRIPT_CYRILLIC */
    LANGCODE_unknown,     /* SCRIPT_DESERET: Deseret was used to write English */
    LANGCODE_hi,   /* SCRIPT_DEVANAGARI */
    LANGCODE_am,   /* SCRIPT_ETHIOPIC */
    LANGCODE_ka,   /* SCRIPT_GEORGIAN */
    LANGCODE_unknown,     /* SCRIPT_GOTHIC */
    LANGCODE_el,   /* SCRIPT_GREEK */
    LANGCODE_gu,   /* SCRIPT_GUJARATI */
    LANGCODE_pa,   /* SCRIPT_GURMUKHI */
    LANGCODE_unknown,     /* SCRIPT_HAN */
    LANGCODE_ko,   /* SCRIPT_HANGUL */
    LANGCODE_he,   /* SCRIPT_HEBREW */
    LANGCODE_ja,   /* SCRIPT_HIRAGANA */
    LANGCODE_kn,   /* SCRIPT_KANNADA */
    LANGCODE_ja,   /* SCRIPT_KATAKANA */
    LANGCODE_km,   /* SCRIPT_KHMER */
    LANGCODE_lo,   /* SCRIPT_LAO */
    LANGCODE_en,   /* SCRIPT_LATIN */
    LANGCODE_ml,   /* SCRIPT_MALAYALAM */
    LANGCODE_mn,   /* SCRIPT_MONGOLIAN */
    LANGCODE_my,   /* SCRIPT_MYANMAR */
    LANGCODE_unknown,     /* SCRIPT_OGHAM; Ogham was used to write old Irish */
    LANGCODE_unknown,     /* SCRIPT_OLD_ITALIC */
    LANGCODE_or,   /* SCRIPT_ORIYA */
    LANGCODE_unknown,     /* SCRIPT_RUNIC */
    LANGCODE_si,   /* SCRIPT_SINHALA */
    LANGCODE_unknown,     /* SCRIPT_SYRIAC; no ISO639-1 code; ISO639-2 code: syr */
    LANGCODE_ta,   /* SCRIPT_TAMIL */
    LANGCODE_te,   /* SCRIPT_TELUGU */
    LANGCODE_dv,   /* SCRIPT_THAANA */
    LANGCODE_th,   /* SCRIPT_THAI */
    LANGCODE_bo,   /* SCRIPT_TIBETAN */
    LANGCODE_iu,   /* SCRIPT_CANADIAN_ABORIGINAL */
    LANGCODE_unknown,     /* SCRIPT_YI */
    LANGCODE_tl,   /* SCRIPT_TAGALOG */

    /* Phillipino languages/scripts */
    LANGCODE_unknown,     /* SCRIPT_HANUNOO; no ISO639-1 code and ISO639-2 code; hnn? */
    LANGCODE_unknown,     /* SCRIPT_BUHID; no ISO639-1 code and ISO639-2 code; bku? */
    LANGCODE_unknown,     /* SCRIPT_TAGBANWA; no ISO639-1 code and ISO639-2 code; tbw? */

    LANGCODE_unknown,     /* SCRIPT_BRAILLE */
    LANGCODE_unknown,     /* SCRIPT_CYPRIOT */
    LANGCODE_unknown,     /* SCRIPT_LIMBU */
    LANGCODE_unknown,     /* SCRIPT_OSMANYA; Used for Somali (so) in the past */
    LANGCODE_unknown,     /* SCRIPT_SHAVIAN; The Shavian alphabet was designed for English */
    LANGCODE_unknown,     /* SCRIPT_LINEAR_B */
    LANGCODE_unknown,     /* SCRIPT_TAI_LE */
    LANGCODE_unknown,     /* SCRIPT_UGARITIC; no ISO639-1 code; ISO639-2 code: uga */

    LANGCODE_unknown,     /* SCRIPT_NEW_TAI_LUE */
    LANGCODE_unknown,     /* SCRIPT_BUGINESE; no ISO639-1 code; ISO639-2 code: bug */
    LANGCODE_unknown,     /* SCRIPT_GLAGOLITIC: The original script for Old Church Slavonic (chu), later written with Cyrillic */
    LANGCODE_unknown,     /* SCRIPT_TIFINAGH: Used for for Berber (ber), but Arabic script is more common */
    LANGCODE_unknown,     /* SCRIPT_SYLOTI_NAGRI; no ISO639-1 code and ISO639-2 code; syl? */
    LANGCODE_unknown,     /* SCRIPT_OLD_PERSIAN; no ISO639-1 code; ISO639-2 code: peo */
    LANGCODE_unknown,     /* SCRIPT_KHAROSHTHI */

    LANGCODE_unknown,     /* SCRIPT_UNKNOWN */
    LANGCODE_unknown,     /* SCRIPT_BALINESE */
    LANGCODE_unknown,     /* SCRIPT_CUNEIFORM */
    LANGCODE_unknown,     /* SCRIPT_PHOENICIAN */
    LANGCODE_unknown,     /* SCRIPT_PHAGS_PA */
    LANGCODE_unknown,     /* SCRIPT_NKO; no ISO639-1 code; ISO639-2 code: nqo */

    /* Unicode-5.1 additions */
    LANGCODE_unknown,     /* SCRIPT_KAYAH_LI */
    LANGCODE_unknown,     /* SCRIPT_LEPCHA */
    LANGCODE_unknown,     /* SCRIPT_REJANG */
    LANGCODE_unknown,     /* SCRIPT_SUNDANESE */
    LANGCODE_unknown,     /* SCRIPT_SAURASHTRA */
    LANGCODE_unknown,     /* SCRIPT_CHAM */
    LANGCODE_unknown,     /* SCRIPT_OL_CHIKI */
    LANGCODE_unknown,     /* SCRIPT_VAI */
    LANGCODE_unknown,     /* SCRIPT_CARIAN */
    LANGCODE_unknown,     /* SCRIPT_LYCIAN */
    LANGCODE_unknown,     /* SCRIPT_LYDIAN */

    /* Unicode-6.0 additions */
    LANGCODE_unknown,     /* SCRIPT_BATAK */
    LANGCODE_unknown,     /* SCRIPT_BRAHMI */
    LANGCODE_unknown,     /* SCRIPT_MANDAIC */

    /* Unicode-6.1 additions */
    LANGCODE_unknown,     /* SCRIPT_CHAKMA */
    LANGCODE_unknown,     /* SCRIPT_MEROITIC_CURSIVE */
    LANGCODE_unknown,     /* SCRIPT_MEROITIC_HIEROGLYPHS */
    LANGCODE_unknown,     /* SCRIPT_MIAO */
    LANGCODE_unknown,     /* SCRIPT_SHARADA */
    LANGCODE_unknown,     /* SCRIPT_SORA_SOMPENG */
    LANGCODE_unknown,     /* SCRIPT_TAKRI */

    /* Unicode 7.0 additions */
    LANGCODE_unknown,     /* SCRIPT_BASSA_VAH, Bass */
    LANGCODE_unknown,     /* SCRIPT_CAUCASIAN_ALBANIAN, Aghb */
    LANGCODE_unknown,     /* SCRIPT_DUPLOYAN, Dupl */
    LANGCODE_unknown,     /* SCRIPT_ELBASAN, Elba */
    LANGCODE_unknown,     /* SCRIPT_GRANTHA, Gran */
    LANGCODE_unknown,     /* SCRIPT_KHOJKI, Khoj */
    LANGCODE_unknown,     /* SCRIPT_KHUDAWADI, Sind */
    LANGCODE_unknown,     /* SCRIPT_LINEAR_A, Lina */
    LANGCODE_unknown,     /* SCRIPT_MAHAJANI, Mahj */
    LANGCODE_unknown,     /* SCRIPT_MANICHAEAN, Manu */
    LANGCODE_unknown,     /* SCRIPT_MENDE_KIKAKUI, Mend */
    LANGCODE_unknown,     /* SCRIPT_MODI, Modi */
    LANGCODE_unknown,     /* SCRIPT_MRO, Mroo */
    LANGCODE_unknown,     /* SCRIPT_NABATAEAN, Nbat */
    LANGCODE_unknown,     /* SCRIPT_OLD_NORTH_ARABIAN, Narb */
    LANGCODE_unknown,     /* SCRIPT_OLD_PERMIC, Perm */
    LANGCODE_unknown,     /* SCRIPT_PAHAWH_HMONG, Hmng */
    LANGCODE_unknown,     /* SCRIPT_PALMYRENE, Palm */
    LANGCODE_unknown,     /* SCRIPT_PAU_CIN_HAU, Pauc */
    LANGCODE_unknown,     /* SCRIPT_PSALTER_PAHLAVI, Phlp */
    LANGCODE_unknown,     /* SCRIPT_SIDDHAM, Sidd */
    LANGCODE_unknown,     /* SCRIPT_TIRHUTA, Tirh */
    LANGCODE_unknown,     /* SCRIPT_WARANG_CITI, Wara */

    /* Unicode 8.0 additions */
    LANGCODE_unknown,     /* SCRIPT_AHOM, Ahom */
    LANGCODE_unknown,     /* SCRIPT_ANATOLIAN_HIEROGLYPHS, Hluw */
    LANGCODE_unknown,     /* SCRIPT_HATRAN, Hatr */
    LANGCODE_unknown,     /* SCRIPT_MULTANI, Mult */
    LANGCODE_unknown,     /* SCRIPT_OLD_HUNGARIAN, Hung */
    LANGCODE_unknown,     /* SCRIPT_SIGNWRITING, Sgnw */

    /* Unicode 9.0 additions */
    LANGCODE_unknown,     /* SCRIPT_ADLAM, Adlm */
    LANGCODE_unknown,     /* SCRIPT_BHAIKSUKI, Bhks */
    LANGCODE_unknown,     /* SCRIPT_MARCHEN, Marc */
    LANGCODE_unknown,     /* SCRIPT_NEWA, Newa */
    LANGCODE_unknown,     /* SCRIPT_OSAGE, Osge */
    LANGCODE_unknown,     /* SCRIPT_TANGUT, Tang */

    /* Unicode 10.0 additions */
    LANGCODE_unknown,     /* SCRIPT_MASARAM_GONDI, Gonm */
    LANGCODE_unknown,     /* SCRIPT_NUSHU, Nshu */
    LANGCODE_unknown,     /* SCRIPT_SOYOMBO, Soyo */
    LANGCODE_unknown,     /* SCRIPT_ZANABAZAR_SQUARE, Zanb */

    /* Unicode 11.0 additions */
    LANGCODE_unknown,     /* SCRIPT_DOGRA, Dogr */
    LANGCODE_unknown,     /* SCRIPT_GUNJALA_GONDI, Gong */
    LANGCODE_unknown,     /* SCRIPT_HANIFI_ROHINGYA, Rohg */
    LANGCODE_unknown,     /* SCRIPT_MAKASAR, Maka */
    LANGCODE_unknown,     /* SCRIPT_MEDEFAIDRIN, Medf */
    LANGCODE_unknown,     /* SCRIPT_OLD_SOGDIAN, Sogo */
    LANGCODE_unknown,     /* SCRIPT_SOGDIAN, Sogd */

    /* Unicode 12.0 additions */
    LANGCODE_unknown,     /* SCRIPT_ELYMAIC, Elymaic */
    LANGCODE_unknown,     /* SCRIPT_NANDINAGARI, Nandinagari */
    LANGCODE_unknown,     /* SCRIPT_NYIAKENG_PUACHUE_HMONG, Nyiakeng Puachue Hmong */
    LANGCODE_unknown,     /* SCRIPT_WANCHO, Wancho */
};

LanguageCode GetSampleLanguageForScript(ScriptType st)
{
    switch (st) {
    case SCRIPT_INVALID_CODE:
        return LANGCODE_unknown;
    default:
        return sample_language_of_script[st];
    }

    return LANGCODE_unknown;
}

#endif /* _MGCHARSET_UNICODE */

