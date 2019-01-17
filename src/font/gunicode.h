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

#ifndef __UCHAR_TYPE_H__
#define __UCHAR_TYPE_H__


/* These are the possible character classifications.
 * See http://www.unicode.org/Public/UNIDATA/UCD.html#General_Category_Values
 */
#if 0 /* VincentWei: Moved to gid.h */
typedef enum
{
  UCHAR_TYPE_CONTROL,
  UCHAR_TYPE_FORMAT,
  UCHAR_TYPE_UNASSIGNED,
  UCHAR_TYPE_PRIVATE_USE,
  UCHAR_TYPE_SURROGATE,
  UCHAR_TYPE_LOWERCASE_LETTER,
  UCHAR_TYPE_MODIFIER_LETTER,
  UCHAR_TYPE_OTHER_LETTER,
  UCHAR_TYPE_TITLECASE_LETTER,
  UCHAR_TYPE_UPPERCASE_LETTER,
  UCHAR_TYPE_COMBINING_MARK,
  UCHAR_TYPE_ENCLOSING_MARK,
  UCHAR_TYPE_NON_SPACING_MARK,
  UCHAR_TYPE_DECIMAL_NUMBER,
  UCHAR_TYPE_LETTER_NUMBER,
  UCHAR_TYPE_OTHER_NUMBER,
  UCHAR_TYPE_CONNECT_PUNCTUATION,
  UCHAR_TYPE_DASH_PUNCTUATION,
  UCHAR_TYPE_CLOSE_PUNCTUATION,
  UCHAR_TYPE_FINAL_PUNCTUATION,
  UCHAR_TYPE_INITIAL_PUNCTUATION,
  UCHAR_TYPE_OTHER_PUNCTUATION,
  UCHAR_TYPE_OPEN_PUNCTUATION,
  UCHAR_TYPE_CURRENCY_SYMBOL,
  UCHAR_TYPE_MODIFIER_SYMBOL,
  UCHAR_TYPE_MATH_SYMBOL,
  UCHAR_TYPE_OTHER_SYMBOL,
  UCHAR_TYPE_LINE_SEPARATOR,
  UCHAR_TYPE_PARAGRAPH_SEPARATOR,
  UCHAR_TYPE_SPACE_SEPARATOR
} GUnicodeType;

/* These are the possible line break classifications.
 * Note that new types may be added in the future.
 * Implementations may regard unknown values like UCHAR_BREAK_UNKNOWN
 * See http://www.unicode.org/unicode/reports/tr14/
 */
typedef enum
{
  UCHAR_BREAK_MANDATORY,
  UCHAR_BREAK_CARRIAGE_RETURN,
  UCHAR_BREAK_LINE_FEED,
  UCHAR_BREAK_COMBINING_MARK,
  UCHAR_BREAK_SURROGATE,
  UCHAR_BREAK_ZERO_WIDTH_SPACE,
  UCHAR_BREAK_INSEPARABLE,
  UCHAR_BREAK_NON_BREAKING_GLUE,
  UCHAR_BREAK_CONTINGENT,
  UCHAR_BREAK_SPACE,
  UCHAR_BREAK_AFTER,
  UCHAR_BREAK_BEFORE,
  UCHAR_BREAK_BEFORE_AND_AFTER,
  UCHAR_BREAK_HYPHEN,
  UCHAR_BREAK_NON_STARTER,
  UCHAR_BREAK_OPEN_PUNCTUATION,
  UCHAR_BREAK_CLOSE_PUNCTUATION,
  UCHAR_BREAK_QUOTATION,
  UCHAR_BREAK_EXCLAMATION,
  UCHAR_BREAK_IDEOGRAPHIC,
  UCHAR_BREAK_NUMERIC,
  UCHAR_BREAK_INFIX_SEPARATOR,
  UCHAR_BREAK_SYMBOL,
  UCHAR_BREAK_ALPHABETIC,
  UCHAR_BREAK_PREFIX,
  UCHAR_BREAK_POSTFIX,
  UCHAR_BREAK_COMPLEX_CONTEXT,
  UCHAR_BREAK_AMBIGUOUS,
  UCHAR_BREAK_UNKNOWN,
  UCHAR_BREAK_NEXT_LINE,
  UCHAR_BREAK_WORD_JOINER,
  UCHAR_BREAK_HANGUL_L_JAMO,
  UCHAR_BREAK_HANGUL_V_JAMO,
  UCHAR_BREAK_HANGUL_T_JAMO,
  UCHAR_BREAK_HANGUL_LV_SYLLABLE,
  UCHAR_BREAK_HANGUL_LVT_SYLLABLE
} GUnicodeBreakType;
#endif /* VincentWei: Moved to gid.h */

typedef enum 
{                         /* ISO 15924 code */
  UCHAR_TYPE_SCRIPT_INVALID_CODE = -1,
  UCHAR_TYPE_SCRIPT_COMMON       = 0,   /* Zyyy */
  UCHAR_TYPE_SCRIPT_INHERITED,          /* Qaai */
  UCHAR_TYPE_SCRIPT_ARABIC,             /* Arab */
  UCHAR_TYPE_SCRIPT_ARMENIAN,           /* Armn */
  UCHAR_TYPE_SCRIPT_BENGALI,            /* Beng */
  UCHAR_TYPE_SCRIPT_BOPOMOFO,           /* Bopo */
  UCHAR_TYPE_SCRIPT_CHEROKEE,           /* Cher */
  UCHAR_TYPE_SCRIPT_COPTIC,             /* Qaac */
  UCHAR_TYPE_SCRIPT_CYRILLIC,           /* Cyrl (Cyrs) */
  UCHAR_TYPE_SCRIPT_DESERET,            /* Dsrt */
  UCHAR_TYPE_SCRIPT_DEVANAGARI,         /* Deva */
  UCHAR_TYPE_SCRIPT_ETHIOPIC,           /* Ethi */
  UCHAR_TYPE_SCRIPT_GEORGIAN,           /* Geor (Geon, Geoa) */
  UCHAR_TYPE_SCRIPT_GOTHIC,             /* Goth */
  UCHAR_TYPE_SCRIPT_GREEK,              /* Grek */
  UCHAR_TYPE_SCRIPT_GUJARATI,           /* Gujr */
  UCHAR_TYPE_SCRIPT_GURMUKHI,           /* Guru */
  UCHAR_TYPE_SCRIPT_HAN,                /* Hani */
  UCHAR_TYPE_SCRIPT_HANGUL,             /* Hang */
  UCHAR_TYPE_SCRIPT_HEBREW,             /* Hebr */
  UCHAR_TYPE_SCRIPT_HIRAGANA,           /* Hira */
  UCHAR_TYPE_SCRIPT_KANNADA,            /* Knda */
  UCHAR_TYPE_SCRIPT_KATAKANA,           /* Kana */
  UCHAR_TYPE_SCRIPT_KHMER,              /* Khmr */
  UCHAR_TYPE_SCRIPT_LAO,                /* Laoo */
  UCHAR_TYPE_SCRIPT_LATIN,              /* Latn (Latf, Latg) */
  UCHAR_TYPE_SCRIPT_MALAYALAM,          /* Mlym */
  UCHAR_TYPE_SCRIPT_MONGOLIAN,          /* Mong */
  UCHAR_TYPE_SCRIPT_MYANMAR,            /* Mymr */
  UCHAR_TYPE_SCRIPT_OGHAM,              /* Ogam */
  UCHAR_TYPE_SCRIPT_OLD_ITALIC,         /* Ital */
  UCHAR_TYPE_SCRIPT_ORIYA,              /* Orya */
  UCHAR_TYPE_SCRIPT_RUNIC,              /* Runr */
  UCHAR_TYPE_SCRIPT_SINHALA,            /* Sinh */
  UCHAR_TYPE_SCRIPT_SYRIAC,             /* Syrc (Syrj, Syrn, Syre) */
  UCHAR_TYPE_SCRIPT_TAMIL,              /* Taml */
  UCHAR_TYPE_SCRIPT_TELUGU,             /* Telu */
  UCHAR_TYPE_SCRIPT_THAANA,             /* Thaa */
  UCHAR_TYPE_SCRIPT_THAI,               /* Thai */
  UCHAR_TYPE_SCRIPT_TIBETAN,            /* Tibt */
  UCHAR_TYPE_SCRIPT_CANADIAN_ABORIGINAL, /* Cans */
  UCHAR_TYPE_SCRIPT_YI,                 /* Yiii */
  UCHAR_TYPE_SCRIPT_TAGALOG,            /* Tglg */
  UCHAR_TYPE_SCRIPT_HANUNOO,            /* Hano */
  UCHAR_TYPE_SCRIPT_BUHID,              /* Buhd */
  UCHAR_TYPE_SCRIPT_TAGBANWA,           /* Tagb */

  /* Unicode-4.0 additions */
  UCHAR_TYPE_SCRIPT_BRAILLE,            /* Brai */
  UCHAR_TYPE_SCRIPT_CYPRIOT,            /* Cprt */
  UCHAR_TYPE_SCRIPT_LIMBU,              /* Limb */
  UCHAR_TYPE_SCRIPT_OSMANYA,            /* Osma */
  UCHAR_TYPE_SCRIPT_SHAVIAN,            /* Shaw */
  UCHAR_TYPE_SCRIPT_LINEAR_B,           /* Linb */
  UCHAR_TYPE_SCRIPT_TAI_LE,             /* Tale */
  UCHAR_TYPE_SCRIPT_UGARITIC,           /* Ugar */

  /* Unicode-4.1 additions */
  UCHAR_TYPE_SCRIPT_NEW_TAI_LUE,        /* Talu */
  UCHAR_TYPE_SCRIPT_BUGINESE,           /* Bugi */
  UCHAR_TYPE_SCRIPT_GLAGOLITIC,         /* Glag */
  UCHAR_TYPE_SCRIPT_TIFINAGH,           /* Tfng */
  UCHAR_TYPE_SCRIPT_SYLOTI_NAGRI,       /* Sylo */
  UCHAR_TYPE_SCRIPT_OLD_PERSIAN,        /* Xpeo */
  UCHAR_TYPE_SCRIPT_KHAROSHTHI,         /* Khar */

  /* Unicode-5.0 additions */
  UCHAR_TYPE_SCRIPT_UNKNOWN,            /* Zzzz */
  UCHAR_TYPE_SCRIPT_BALINESE,           /* Bali */
  UCHAR_TYPE_SCRIPT_CUNEIFORM,          /* Xsux */
  UCHAR_TYPE_SCRIPT_PHOENICIAN,         /* Phnx */
  UCHAR_TYPE_SCRIPT_PHAGS_PA,           /* Phag */
  UCHAR_TYPE_SCRIPT_NKO                 /* Nkoo */
} GUnicodeScript;

#endif /* __UCHAR_TYPE_H__ */
