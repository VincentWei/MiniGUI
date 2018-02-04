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
** charset-bidi.h: The charset Bidirectional Algorithm.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 2008/01/23
*/

#ifndef GUI_FONT_CHARSET_BIDI_H
#define GUI_FONT_CHARSET_BIDI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

//#define ARABIC_DEBUG
#ifdef ARABIC_DEBUG
    #define DBGLOG(s)      do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG2(s, t1)  do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG3(s, t1,t2)  do { if (1) { fprintf(stderr, s, t1,t2); } } while (0)
    #define DBGLOG4(s, t1,t2,t3)  do { if (1) { fprintf(stderr, s, t1,t2,t3); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG2(s, t1)
    #define DBGLOG3(s, t1, t2)
    #define DBGLOG4(s, t1, t2, t3)
#endif

/* 
 * Define some bit masks, that character types are based on, each one has
 * only one bit on.
 */

#define BIDI_MASK_RTL       0x00000001L	/* Is right to left */
#define BIDI_MASK_ARABIC    0x00000002L	/* Is arabic */

/* Each char can be only one of the three following. */
#define BIDI_MASK_STRONG    0x00000010L	/* Is strong */
#define BIDI_MASK_WEAK	    0x00000020L	/* Is weak */
#define BIDI_MASK_NEUTRAL	0x00000040L	/* Is neutral */
#define BIDI_MASK_SENTINEL	0x00000080L	/* Is sentinel: SOT, EOT */

/* Each char can be only one of the five following. */
#define BIDI_MASK_LETTER	0x00000100L	/* Is letter: L, R, AL */
#define BIDI_MASK_NUMBER	0x00000200L	/* Is number: EN, AN */
#define BIDI_MASK_NUMSEPTER	0x00000400L	/* Is number separator or terminator: ES, ET, CS */
#define BIDI_MASK_SPACE	    0x00000800L	/* Is space: BN, BS, SS, WS */
#define BIDI_MASK_EXPLICIT	0x00001000L	/* Is expilict mark: LRE, RLE, LRO, RLO, PDF */

/* Can be on only if BIDI_MASK_SPACE is also on. */
#define BIDI_MASK_SEPARATOR	0x00002000L	/* Is test separator: BS, SS */
/* Can be on only if BIDI_MASK_EXPLICIT is also on. */
#define BIDI_MASK_OVERRIDE	0x00004000L	/* Is explicit override: LRO, RLO */

#define BIDI_MASK_ES		0x00010000L
#define BIDI_MASK_ET		0x00020000L
#define BIDI_MASK_CS		0x00040000L

#define BIDI_MASK_NSM       0x00080000L
#define BIDI_MASK_BN		0x00100000L

#define BIDI_MASK_BS		0x00200000L
#define BIDI_MASK_SS		0x00400000L
#define BIDI_MASK_WS		0x00800000L

/*
 * Define values for BidiCharType
 */

/* Strong left to right */
#define BIDI_TYPE_LTR	(BIDI_MASK_STRONG + BIDI_MASK_LETTER)

/* Right to left characters */
#define BIDI_TYPE_RTL	(BIDI_MASK_STRONG + BIDI_MASK_LETTER \
                         + BIDI_MASK_RTL)

/* Arabic characters */
#define BIDI_TYPE_AL	(BIDI_MASK_STRONG + BIDI_MASK_LETTER \
                        + BIDI_MASK_RTL  + BIDI_MASK_ARABIC)

/* Left-To-Right embedding */
#define BIDI_TYPE_LRE	(BIDI_MASK_STRONG + BIDI_MASK_EXPLICIT)

/* Right-To-Left embedding */
#define BIDI_TYPE_RLE	(BIDI_MASK_STRONG + BIDI_MASK_EXPLICIT \
                        + BIDI_MASK_RTL)

/* Left-To-Right override */
#define BIDI_TYPE_LRO	(BIDI_MASK_STRONG + BIDI_MASK_EXPLICIT \
                        + BIDI_MASK_OVERRIDE)

/* Right-To-Left override */
#define BIDI_TYPE_RLO	(BIDI_MASK_STRONG + BIDI_MASK_EXPLICIT \
                        + BIDI_MASK_RTL + BIDI_MASK_OVERRIDE)

/* Pop directional override */
#define BIDI_TYPE_PDF	(BIDI_MASK_WEAK + BIDI_MASK_EXPLICIT)

/* European digit */
#define BIDI_TYPE_EN	(BIDI_MASK_WEAK + BIDI_MASK_NUMBER)

/* Arabic digit */
#define BIDI_TYPE_AN	(BIDI_MASK_WEAK + BIDI_MASK_NUMBER \
                        + BIDI_MASK_ARABIC)

/* European number separator */
#define BIDI_TYPE_ES	(BIDI_MASK_WEAK + BIDI_MASK_NUMSEPTER \
                        + BIDI_MASK_ES)

/* European number terminator */
#define BIDI_TYPE_ET	(BIDI_MASK_WEAK + BIDI_MASK_NUMSEPTER \
                        + BIDI_MASK_ET)

/* Common Separator */
#define BIDI_TYPE_CS	(BIDI_MASK_WEAK + BIDI_MASK_NUMSEPTER \
                        + BIDI_MASK_CS)

/* Non spacing mark */
#define BIDI_TYPE_NSM	(BIDI_MASK_WEAK + BIDI_MASK_NSM)

/* Boundary neutral */
#define BIDI_TYPE_BN	(BIDI_MASK_WEAK + BIDI_MASK_SPACE \
                        + BIDI_MASK_BN)

/* Block separator */
#define BIDI_TYPE_BS	(BIDI_MASK_NEUTRAL + BIDI_MASK_SPACE \
                        + BIDI_MASK_SEPARATOR + BIDI_MASK_BS)

/* Segment separator */
#define BIDI_TYPE_SS	(BIDI_MASK_NEUTRAL + BIDI_MASK_SPACE \
                        + BIDI_MASK_SEPARATOR + BIDI_MASK_SS)
/* Whitespace */
#define BIDI_TYPE_WS	(BIDI_MASK_NEUTRAL + BIDI_MASK_SPACE \
                        + BIDI_MASK_WS)

/* Other Neutral */
#define BIDI_TYPE_ON	(BIDI_MASK_NEUTRAL)

/* The following are used to identify the paragraph direction,
   types L, R, N are not used internally anymore, and recommended to use
   LTR, RTL and ON instead, didn't removed because of compatability. */

#define BIDI_TYPE_L		(BIDI_TYPE_LTR)
#define BIDI_TYPE_R		(BIDI_TYPE_RTL)
#define BIDI_TYPE_N		(BIDI_TYPE_ON)
/* Weak left to right */
#define BIDI_TYPE_WL	(BIDI_MASK_WEAK)
/* Weak right to left */
#define BIDI_TYPE_WR	(BIDI_MASK_WEAK + BIDI_MASK_RTL)

/* The following are only used internally */

/* Start of text */
#define BIDI_TYPE_SOT	(BIDI_MASK_SENTINEL)
/* End of text */
#define BIDI_TYPE_EOT	(BIDI_MASK_SENTINEL + BIDI_MASK_RTL)

/* Is private-use value? */
#define BIDI_TYPE_PRIVATE(p)	((p) < 0)

/* Return the direction of the level number, BIDI_TYPE_LTR for even and
   BIDI_TYPE_RTL for odds. */
#define BIDI_LEVEL_TO_DIR(lev) (BIDI_TYPE_LTR | (lev & 1))

/* Return the minimum level of the direction, 0 for BIDI_TYPE_LTR and
   1 for BIDI_TYPE_RTL and BIDI_TYPE_AL. */
#define BIDI_DIR_TO_LEVEL(dir) ((BidiLevel)(dir & 1))

/* Is right to left? */
#define BIDI_IS_RTL(p)      ((p) & BIDI_MASK_RTL)
/* Is arabic? */
#define BIDI_IS_ARABIC(p)   ((p) & BIDI_MASK_ARABIC)

/* Is strong? */
#define BIDI_IS_STRONG(p)   ((p) & BIDI_MASK_STRONG)
/* Is weak? */
#define BIDI_IS_WEAK(p)     ((p) & BIDI_MASK_WEAK)
/* Is neutral? */
#define BIDI_IS_NEUTRAL(p)  ((p) & BIDI_MASK_NEUTRAL)
/* Is sentinel? */
#define BIDI_IS_SENTINEL(p) ((p) & BIDI_MASK_SENTINEL)

/* Is letter: L, R, AL? */
#define BIDI_IS_LETTER(p)   ((p) & BIDI_MASK_LETTER)
/* Is number: EN, AN? */
#define BIDI_IS_NUMBER(p)   ((p) & BIDI_MASK_NUMBER)
/* Is number separator or terminator: ES, ET, CS? */
#define BIDI_IS_NUMBER_SEPARATOR_OR_TERMINATOR(p) \
	((p) & BIDI_MASK_NUMSEPTER)

/* Is space: BN, BS, SS, WS? */
#define BIDI_IS_SPACE(p)    ((p) & BIDI_MASK_SPACE)
/* Is explicit mark: LRE, RLE, LRO, RLO, PDF? */
#define BIDI_IS_EXPLICIT(p) ((p) & BIDI_MASK_EXPLICIT)

/* Is test separator: BS, SS? */
#define BIDI_IS_SEPARATOR(p) ((p) & BIDI_MASK_SEPARATOR)

/* Is explicit override: LRO, RLO? */
#define BIDI_IS_OVERRIDE(p) ((p) & BIDI_MASK_OVERRIDE)

/* Some more: */

/* Is left to right letter: LTR? */
#define BIDI_IS_LTR_LETTER(p) \
	((p) & (BIDI_MASK_LETTER | BIDI_MASK_RTL) == BIDI_MASK_LETTER)

/* Is right to left letter: RTL, AL? */
#define BIDI_IS_RTL_LETTER(p) \
	((p) & (BIDI_MASK_LETTER | BIDI_MASK_RTL) \
	== (BIDI_MASK_LETTER | BIDI_MASK_RTL))

/* Is ES or CS: ES, CS? */
#define BIDI_IS_ES_OR_CS(p) \
	((p) & (BIDI_MASK_ES | BIDI_MASK_CS))

/* Change numbers: EN, AN to RTL. */
#define BIDI_NUMBER_TO_RTL(p) \
        (BIDI_IS_NUMBER(p) ? BIDI_TYPE_RTL : (p))

#ifdef MAIN_CAP_TEST
typedef char    BidiChar;
#else
typedef int     BidiChar;
#endif
typedef int     BidiCharType;
typedef char    BidiLevel;
typedef int     BidiStrIndex;
typedef struct  _TYPERUN  TYPERUN;

typedef void (*CB_DO_REORDER) (void* context, int len, int pos);

int bidi_glyph_type(const char* charset_name, Glyph32 glyph_value);

Glyph32* bidi_str_reorder (const char* charset_name, Glyph32* glyphs,
        int len);

void bidi_get_embeddlevels(const char* charset_name, Glyph32* glyphs, 
        BidiStrIndex len, Uint8* embedding_level_list, Uint8 type);

Glyph32* bidi_index_reorder (const char* charset_name, Glyph32* glyphs, 
        int len, int* index_map);

Glyph32* bidi_map_reorder (const char* charset_name, Glyph32* glyphs, 
        int len, GLYPHMAPINFO* map);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_CHARSET_BIDI_H
