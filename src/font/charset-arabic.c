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
** charset.c: The charset's arabic operation set.
** 
** Created by houhuihua at 2008/01/23
*/
#ifdef _MGCHARSET_ARABIC

#include "gdi.h"
#include "bidi.h"

typedef struct _SHAPEINFO {
    unsigned short int  isolated;
    unsigned short int  final;
    unsigned short int  initial;
    unsigned short int  medial;
}SHAPEINFO;

#define MAX_GLYPH_VALUE 0x133
#define SHAPENUMBER 36
static const SHAPEINFO shape_info[SHAPENUMBER] = {
      /*        Base                 Isol    Final    Initial    Medial */
    { /* 0  0xC1 (0x0621) */ 0xC1/*0xFE80*/, 0x00,            0x00,           0x00 /* Arabic letter Hamza                 */ },
    { /* 1  0xC2 (0x0622) */ 0xC2/*0xFE81*/, 0x102/*0xFE82*/, 0x00,           0x00 /* Arabic letter Alef with Madda above */ },
    { /* 2  0xC3 (0x0623) */ 0xC3/*0xFE83*/, 0x103/*0xFE84*/, 0x00,           0x00 /* Arabic letter Alef with Hamza above */ },
    { /* 3  0xC4 (0x0624) */ 0xC4/*0xFE85*/, 0x104/*0xFE86*/, 0x00,           0x00 /* Arabic letter Waw with Hamza above  */ },
    { /* 4  0xC5 (0x0625) */ 0xC5/*0xFE87*/, 0x105/*0xFE88*/, 0x00,           0x00 /* Arabic letter Alef with Hamza below */ },
    { /* 5  0xC6 (0x0626) */ 0x9F/*0xFE89*/, 0xC6 /*0xFE8A*/, 0xC0/*0xFE8B*/, 0x106/*0xFE8C*/ /* Arabic letter Yeh with Hamza above  */ },
    { /* 6  0xC7 (0x0627) */ 0xC7/*0xFE8D*/, 0x107/*0xFE8E*/, 0x00,           0x00 /* Arabic letter Alef                             */ },
    { /* 7  0xC8 (0x0628) */ 0xC8/*0xFE8F*/, 0x108/*0xFE90*/, 0xEB/*0xFE91*/, 0x11E/*0xFE92*/ /* Arabic letter Beh                   */ },
    { /* 8  0xC9 (0x0629) */ 0xC9/*0xFE93*/, 0x8E /*0xFE94*/, 0x00,           0x00 /* Arabic letter Teh Marbuta                      */ },
    { /* 9  0xCA (0x062A) */ 0xCA/*0xFE95*/, 0x109/*0xFE96*/, 0xEC/*0xFE97*/, 0x11F/*0xFE98*/ /* Arabic letter Teh                   */ },
    { /* 10 0xCB (0x062B) */ 0xCB/*0xFE99*/, 0x10A/*0xFE9A*/, 0xED/*0xFE9B*/, 0x120/*0xFE9C*/ /* Arabic letter Theh                  */ },
    { /* 11 0xCC (0x062C) */ 0xCC/*0xFE9D*/, 0x10B/*0xFE9E*/, 0xEE/*0xFE9F*/, 0x121/*0xFEA0*/ /* Arabic letter Jeem                  */ },
    { /* 12 0xCD (0x062D) */ 0xCD/*0xFEA1*/, 0x10C/*0xFEA2*/, 0xEF/*0xFEA3*/, 0x122/*0xFEA4*/ /* Arabic letter Hah                   */ },
    { /* 13 0xCE (0x062E) */ 0xCE/*0xFEA5*/, 0x10D/*0xFEA6*/, 0xF0/*0xFEA7*/, 0x123/*0xFEA8*/ /* Arabic letter Khah                  */ },
    { /* 14 0xCF (0x062F) */ 0xCF/*0xFEA9*/, 0x10E/*0xFEAA*/, 0x00,           0x00 /* Arabic letter Dal                   */ },
    { /* 15 0xD0 (0x0630) */ 0xD0/*0xFEAB*/, 0x10F/*0xFEAC*/, 0x00,           0x00 /* Arabic letter Thal                  */ },
    { /* 16 0xD1 (0x0631) */ 0xD1/*0xFEAD*/, 0x110/*0xFEAE*/, 0x00,           0x00 /* Arabic letter Reh                   */ },
    { /* 17 0xD2 (0x0632) */ 0xD2/*0xFEAF*/, 0x111/*0xFEB0*/, 0x00,           0x00 /* Arabic letter Zain                  */ },
    { /* 18 0xD3 (0x0633) */ 0xD3/*0xFEB1*/, 0x8F /*0xFEB2*/, 0xF1/*0xFEB3*/, 0x124/*0xFEB4*/ /* Arabic letter Seen                  */ },
    { /* 19 0xD4 (0x0634) */ 0xD4/*0xFEB5*/, 0x90 /*0xFEB6*/, 0xF2/*0xFEB7*/, 0x125/*0xFEB8*/ /* Arabic letter Sheen                 */ },
    { /* 20 0xD5 (0x0635) */ 0xD5/*0xFEB9*/, 0x91 /*0xFEBA*/, 0xF3/*0xFEBB*/, 0x126/*0xFEBC*/ /* Arabic letter Sad                   */ },
    { /* 21 0xD6 (0x0636) */ 0xD6/*0xFEBD*/, 0x92 /*0xFEBE*/, 0xF4/*0xFEBF*/, 0x127/*0xFEC0*/ /* Arabic letter Dad                   */ },
    { /* 22 0xD7 (0x0637) */ 0xD7/*0xFEC1*/, 0x93 /*0xFEC2*/, 0x112/*0xFEC3*/,0x100/*0xFEC4*/ /* Arabic letter Tah                   */ },
    { /* 23 0xD8 (0x0638) */ 0xD8/*0xFEC5*/, 0x101/*0xFEC6*/, 0xD8/*0xFEC7*/, 0x94 /*0xFEC8*/ /* Arabic letter Zah                   */ },
    { /* 24 0xD9 (0x0639) */ 0xD9/*0xFEC9*/, 0x96 /*0xFECA*/, 0xF5/*0xFECB*/, 0x95 /*0xFECC*/ /* Arabic letter Ain                   */ },
    { /* 25 0xDA (0x063A) */ 0xDA/*0xFECD*/, 0x98 /*0xFECE*/, 0xF6/*0xFECF*/, 0x97 /*0xFED0*/ /* Arabic letter Ghain                 */ },
    { /* 26 0xE1 (0x0641) */ 0xE1/*0xFED1*/, 0x114/*0xFED2*/, 0xF7/*0xFED3*/, 0x99 /*0xFED4*/ /* Arabic letter Feh                   */ },
    { /* 27 0xE2 (0x0642) */ 0xE2/*0xFED5*/, 0x115/*0xFED6*/, 0xF8/*0xFED7*/, 0x9A /*0xFED8*/ /* Arabic letter Qaf                   */ },
    { /* 28 0xE3 (0x0643) */ 0xE3/*0xFED9*/, 0x116/*0xFEDA*/, 0xF9/*0xFEDB*/, 0x9B /*0xFEDC*/ /* Arabic letter Kaf                   */ },
    { /* 29 0xE4 (0x0644) */ 0xE4/*0xFEDD*/, 0x117/*0xFEDE*/, 0xFA/*0xFEDF*/, 0x128/*0xFEE0*/ /* Arabic letter Lam                   */ },
    { /* 30 0xE5 (0x0645) */ 0xE5/*0xFEE1*/, 0x118/*0xFEE2*/, 0xFB/*0xFEE3*/, 0x129/*0xFEE4*/ /* Arabic letter Meem                  */ },
    { /* 31 0xE6 (0x0646) */ 0xE6/*0xFEE5*/, 0x119/*0xFEE6*/, 0xFC/*0xFEE7*/, 0x12A/*0xFEE8*/ /* Arabic letter Noon                  */ },
    { /* 32 0xE7 (0x0647) */ 0xE7/*0xFEE9*/, 0x11A/*0xFEEA*/, 0xFD/*0xFEEB*/, 0x9C /*0xFEEC*/ /* Arabic letter Heh                   */ },
    { /* 33 0xE8 (0x0648) */ 0xE8/*0xFEED*/, 0x11B/*0xFEEE*/, 0x00,           0x00 /* Arabic letter Waw                   */ },
    { /* 34 0xE9 (0x0649) */ 0x8D/*0xFEEF*/, 0x11C/*0xFEF0*/, 0x00,           0x00 /* Arabic letter Alef Maksura          */ },
    { /* 35 0xEA (0x064A) */ 0x9E/*0xFEF1*/, 0x11D/*0xFEF2*/, 0xFE/*0xFEF3*/, 0x12B/*0xFEF4*/ /* Arabic letter Yeh                   */ },
};

/************************* ISO8859-6 Specific Operations **********************/
static int iso8859_6_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-6"))
        return 0;

    if (strstr (name, "ARABIC"))
        return 0;

    return 1;
}

#ifdef _MGCHARSET_UNICODE
static unsigned short iso8859_68x_unicode_map [] =
{
                                    /*vowel with tadweel.*/
    0xFE70, 0xFC5E, 0xFC5F, 0xFC60, /*0x81~0x84*/
    0xFC61, 0xFC62, 0xFC63, 0xFC64, /*0x85~0x88*/
    0xFE70, 0xFCF2, 0xFCF3, 0xFCF4, /*0x89~0x8C*/

    0xFEEF, 0xFE94, 0xFEB2, 0xFEB6, /*0x8D~0x90*/
    0xFEBA, 0xFEBE, 0xFEC2, 0xFEC8, /*0x93~0x94*/
    0xFECC, 0xFECA, 0xFED0, 0xFECE,
    0xFED4, 0xFED8, 0xFEDC, 0xFEEC, 
    0xFEEA, 0xFEF1, 0xFE89, 0x0020,
                                    /*two cell ligature.*/
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, /*0xA1~0xA4*/  
    0xFFFF, 0xFFFF, 0xFFFF,         /*0xA5~0xA7*/

    0xFE70, 0xFE72, 0xFE74, 0xFE76, /*0xA8~0xAB*/
    0xFE78, 0xFE7A, 0xFE7C, 0xFE7E, /*0xAC~0xAF*/
    0x0660, 0x0661, 0x0662, 0x0663, /*0~9*/
    0x0664, 0x0665, 0x0666, 0x0667, 
    0x0668, 0x0669, 0x060C, 0x061B, 
    0xFE71, 0xFE72, 0xFE74, 0x061F, /*0xBD~0xBF*/
                                    
    0xFE8B, 0xFE80,                 /*0xC0,0xC1*/
    0xFE81, 0xFE83, 0xFE85, 0xFE87, /*0xC2~0xC5*/
    0xFE8A, 0xFE8D, 0xFE8F, 0xFE93, /*0xC6~0xC9*/
    0xFE95, 0xFE99, 0xFE9D, 0xFEA1, /*0xCA~0xCD*/
    0xFEA5, 0xFEA9, 0xFEAB, 0xFEAD, /*0xCE~0xD1*/
    0xFEAF, 0xFEB1, 0xFEB5, 0xFEB9, /*0xD2~0xD5*/

    0xFEBD, 0xFEC1, 0xFEC5, 0xFEC9, /*0xD6~0xD9*/
    0xFECD, 0xFE77, 0xFE79, 0xFE7B, /*0xDA~0xDD*/
    0xFE7D, 0xFE7F, 0x0640,
    0xFED1, 0xFED5, 0xFED9, 0xFEDD, /*0xE1~0xE4*/
    0xFEE1, 0xFEE5, 0xFEE9, 0xFEED, /*0xE5~0xE8*/
    0xFEEF, 0xFEF1, 0xFE91, 0xFE97, /*0xE9~0xEC*/

    0xFE9B, 0xFE9F, 0xFEA3, 0xFEA7, /*0xED~0xF0*/
    0xFEB3, 0xFEB7, 0xFEBB, 0xFEBF, /*0xF1~0xF4*/
    0xFECB, 0xFECF, 0xFED3, 0xFED7, /*0xF5~0xF8*/
    0xFEDB, 0xFEDF, 0xFEE3, 0xFEE7, /*0xF9~0xFC*/
    0xFEEB, 0xFEF4, 0xFFFF,         /*0xFD~0xFF*/

    0xFEC4, 0xFEC6, 0xFE82, 0xFE84, /*0x100~0x103*/
    0xFE86, 0xFE88, 0xFE8C, 0xFE8E, /*0x104~0x107*/
    0xFE90, 0xFE96, 0xFE9A, 0xFE9E, /*0x108~0x10B*/
    0xFEA2, 0xFEA6, 0xFEAA, 0xFEAC, /*0x10C~0x10F*/
                                    
    0xFEAE, 0xFEB0, 0xFEC3, 0xFEC7, /*0x110~0x113*/
    0xFED2, 0xFED6, 0xFEDA, 0xFEDE, /*0x114~0x117*/
    0xFEE2, 0xFEE6, 0xFEEA, 0xFEEE, /*0x118~0x11B*/
    0xFEF0, 0xFEF2, 0xFE92, 0xFE98, /*0x11C~0x11F*/
                                    
    0xFE9C, 0xFEA0, 0xFEA4, 0xFEA8, /*0x120~0x123*/
    0xFEB4, 0xFEB8, 0xFEBC, 0xFEC0, /*0x124~0x127*/
    0xFEE0, 0xFEE4, 0xFEE8, 0xFEF4, /*0x128~0x12B*/
                                    
    0xFEFB, 0xFEFC, 0xFEF5, 0xFEF6, /*0x12C~0x12F*/
    0xFEF7, 0xFEF8, 0xFEF9, 0xFEFA, /*0x130~0x133*/
};

static UChar32 iso8859_6_conv_to_uc32 (Glyph32 glyph_value)
{
    if (glyph_value < 0x81)
        return (Glyph32) (glyph_value);
    else if (glyph_value <= MAX_GLYPH_VALUE )
        return (Glyph32) iso8859_68x_unicode_map[glyph_value - 0x81];
    else 
        return 0xFFFF;
}

static int iso8859_6_conv_from_uc32 (UChar32 wc, unsigned char* mchar)
{
    switch (wc) {
        case 0x060C:
            *mchar = 0xAC;
            return 1;
        case 0x061B:
            *mchar = 0xBB;
            return 1;
        case 0x061F:
            *mchar = 0xBF;
            return 1;
    }

    if (wc < 0xC1) {
        *mchar = (unsigned char) wc;
        return 1;
    }

    if (wc >= 0x0621 && wc <= (0x0621 + 0xF2 - 0xC1)) {
        *mchar = (unsigned char) (wc - 0x0621 + 0xC1);
        return 1;
    }

    return 0;
}
#endif

/* must attetion the para s is arabic glyph value.*/
static int is_arabic_glyph_vowel(Uint8 c)
{

    if ((c >= 0x81) && (c <= 0x86)) return 1;
    if ((c >= 0xa8) && (c <= 0xaf)) return 1;

    /* unicode vowel range. */
    /* if ((s >= 0x64B) && (s <= 0x655)) return 1;
       if ((s >= 0xFC5E) && (s <= 0xFC63)) return 1;
       if ((s >= 0xFE70) && (s <= 0xFE7F)) return 1;
    */
    return 0;
}

/* ISO8859-6 charset vowel relative define and judge */
#define ISARABIC_VOWEL(s) ((s >= FATHATAN) && (s <= SUKUN))
#define ISARABIC_PUNC(s) ((s == COMMA) || (s == SEMICOLON) \
        || (s == DOLLAR) || (s == QUESTION))

#define ALIF       0xC7
#define ALIFHAMZA  0xC5
#define ALIFAHAMZA 0xC3
#define ALIFMADDA  0xC2  //ARABIC LETTER ALEF WITH MADDA ABOVE
#define LAM        0xE4

/* ISO 8859-6 punctuation mark.*/
#define COMMA      0xAC
#define SEMICOLON  0xBB
#define QUESTION   0xBF
#define DOLLAR     0xA4

#define TADWEEL    0xE0
#define FATHATAN   0xEB
#define DAMMATAN   0xEC
#define KASRATAN   0xED
#define FATHA      0xEE
#define DAMMA      0xEF
#define KASRA      0xF0
#define SHADDA     0xF1
#define SUKUN      0xF2

/* this define is relative with fontset 0xa1~0xa7.
 * it it used for ligature such as LAM+ALEF, one ligature
 * have two fontset glyphs.*/
#define LAM_ALIF         0x12C //0xA1A5
#define LAM_ALIF_F       0x12D //0xA1A6

#define LAM_ALIFMADDA    0x12E //0xA2A5
#define LAM_ALIFMADDA_F  0x12F //0xA2A6

#define LAM_ALIFAHAMZA   0x130 //0xA3A5
#define LAM_ALIFAHAMZA_F 0x131 //0xA3A6  //ARABIC LETTER ALEF WITH HAMZA BELOW
#define LAM_ALIFHAMZA    0x132 //0xA4A5
#define LAM_ALIFHAMZA_F  0x133 //0xA4A6  //ARABIC LETTER ALEF WITH HAMZA ABOVE final.


/* Because the get_ligature is close relative with fontset 6.8x, so
 * do it in the follow five functions, if the fontset is change, you only
 * need to implement follow five interface.
 * 1. get_vowel_glyph().
 * 2. get_twovowel_glyph().
 * 3. get_tadweel_glyph().
 * 4. get_ligature_glyph().
 * 5. get_punpoint_glyph().
 * houhh 20080128.
 * */

static int fontset_68x_get_punpoint_glyph(Uint8 c)
{
    int ligature = -1;
    switch(c){
        case  COMMA:     ligature = 0xba; break;
        case  SEMICOLON: ligature = 0xbb; break;
        case  QUESTION:  ligature = 0xbf; break;
        case  DOLLAR:    ligature = 0x24; break;
        default:         ligature = -1;   break; // this will not happen.
    }
    return ligature;
}

static int fontset_68x_get_vowel_glyph(Uint8 c)
{
    int ligature = -1;
    switch(c){
        case  FATHATAN: ligature = 0xa8; break;
        case  DAMMATAN: ligature = 0xa9; break;
        case  KASRATAN: ligature = 0xaa; break;
        case  FATHA:    ligature = 0xab; break;
        case  DAMMA:    ligature = 0xac; break;
        case  KASRA:    ligature = 0xad; break;
        case  SHADDA:   ligature = 0xae; break;
        case  SUKUN:    ligature = 0xaf; break;
        default:        ligature = -1;   break; // this will not happen.
    }
    return ligature;
}

static int fontset_68x_get_twovowel_glyph(unsigned char c, unsigned char next, int* ignore)
{
    int ligature = -1;
    if(c == SHADDA){
        switch(next){
            case  FATHATAN: *ignore = 1; ligature = 0x81; break;
            case  DAMMATAN: *ignore = 1; ligature = 0x82; break;
            case  KASRATAN: *ignore = 1; ligature = 0x83; break;
            case  FATHA:    *ignore = 1; ligature = 0x84; break;
            case  DAMMA:    *ignore = 1; ligature = 0x85; break;
            case  KASRA:    *ignore = 1; ligature = 0x86; break;
            default:        *ignore = 0; ligature = 0xae; break; //FIX BUG, only SHADDA.
        }
    }
    else {
        *ignore = 0;
        ligature = fontset_68x_get_vowel_glyph(c);
    }
    return ligature;
}

static int fontset_68x_get_ligature_glyph(unsigned char c, BOOL prev_affects_joining, int* ignore)
{
    int ligature = -1;
    if(prev_affects_joining){
        switch (c){
            case ALIF:       *ignore = 1; ligature = LAM_ALIF_F; break;
            case ALIFHAMZA:  *ignore = 1; ligature = LAM_ALIFHAMZA_F; break;
            case ALIFAHAMZA: *ignore = 1; ligature = LAM_ALIFAHAMZA_F;break;
            case ALIFMADDA:  *ignore = 1; ligature = LAM_ALIFMADDA_F; break;
            default:         *ignore = 0; ligature = -1; break; // FIX BUG, later do the shape continue.
        }
    }
    else{
        switch (c){
            case ALIF:       *ignore = 1; ligature = LAM_ALIF; break;
            case ALIFHAMZA:  *ignore = 1; ligature = LAM_ALIFHAMZA; break;
            case ALIFAHAMZA: *ignore = 1; ligature = LAM_ALIFAHAMZA;break;
            case ALIFMADDA:  *ignore = 1; ligature = LAM_ALIFMADDA; break;
            default:         *ignore = 0; ligature = -1; break;
        }
    }
    return ligature;
}

static int fontset_68x_get_tadweel_glyph(unsigned char c, unsigned char next, int* ignore)
{
    int ligature = -1;
    if(c == SHADDA){
        /* TADWEEL combine with two vowel except SUKUN 
         * can not combine with SHADDA.*/
        switch(next){
            case  FATHATAN: *ignore = 2; ligature = 0x87; break;
            case  DAMMATAN: *ignore = 2; ligature = 0x88; break;
            case  KASRATAN: *ignore = 2; ligature = 0x89; break;
            case  FATHA:    *ignore = 2; ligature = 0x8a; break;
            case  DAMMA:    *ignore = 2; ligature = 0x8b; break;
            case  KASRA:    *ignore = 2; ligature = 0x8c; break;
            default:        *ignore = 1; ligature = 0xde; break; // FIX BUG combine of TADWELL SHADDA.
        }
    }
    else {
        /* TADWEEL combine withe one vowel. */
        switch(c){
            case  FATHATAN: *ignore = 1; ligature = 0xbc; break;
            case  DAMMATAN: *ignore = 1; ligature = 0xbd; break;
            case  KASRATAN: *ignore = 1; ligature = 0xbe; break;
            case  FATHA:    *ignore = 1; ligature = 0xdb; break;
            case  DAMMA:    *ignore = 1; ligature = 0xdc; break;
            case  KASRA:    *ignore = 1; ligature = 0xdd; break;
            case  SHADDA:   *ignore = 1; ligature = 0xde; break;
            case  SUKUN:    *ignore = 1; ligature = 0xdf; break;
            default:        *ignore = 0; ligature = TADWEEL; break; // FIX BUG of only TADWEEL.
        }
    }
    return ligature;
}

#ifndef _DEBUG
static 
#endif
int get_ligature(const unsigned char* mchar, int len, BOOL prev_affects_joining, int* ignore)
{
    int ligature = -1;
    Uint8 cur_char, next, next_next;

    if(ignore) *ignore = 0;

    cur_char = *mchar;
    
    ligature = fontset_68x_get_punpoint_glyph(cur_char);
    if(ligature > 0) return ligature;
    
    if(len == 1){
        if (ISARABIC_VOWEL(cur_char)){
            ligature = fontset_68x_get_vowel_glyph(cur_char);
        }
        else if(cur_char == TADWEEL){
            ligature = TADWEEL;
        }
    }
    else if(len > 1){
        next = *(mchar+1);
        if (ISARABIC_VOWEL(cur_char)){ /* two VOWEL, one must be SHADDA first. */
            ligature = fontset_68x_get_twovowel_glyph(cur_char, next, ignore);
        }
        else if (cur_char == LAM) {    /* LAM+ALEF+HAMAZ+MADDA ligature. */
            ligature = fontset_68x_get_ligature_glyph(next, prev_affects_joining, ignore);
        } 
        else if(cur_char == TADWEEL){  /* TADWEEL combine with VOWEL*/
            if(len > 2) next_next = *(mchar+2);
            else next_next = 0;

            ligature = fontset_68x_get_tadweel_glyph(next, next_next, ignore);
            if(ligature == -1 && cur_char == TADWEEL){
                ligature = TADWEEL;
            }
        }
    }
    else{
        *ignore = 0;
    }

    return ligature;
}

static int iso8859_6_len_first_char (const unsigned char* mstr, int len)
{
    int ignore = 0;

    /* if ligature, ligature will have two or three bytes.*/
    get_ligature (mstr, len, FALSE, &ignore);

    if (ignore == 1)
        return 2;
    else if (ignore == 2)
        return 3;
    else 
        return 1;

}

#define ISARABIC_LIG_HALF(s) ((s == 0xa5) || (s == 0xa6))

static unsigned int iso8859_6_glyph_type (Glyph32 glyph_value)
{
    unsigned int ch_type = MCHAR_TYPE_UNKNOWN;

    if (is_arabic_glyph_vowel (glyph_value)){  /* is vowel */
        ch_type = MCHAR_TYPE_VOWEL;
    }
    else{
        ch_type = sb_glyph_type (glyph_value);
    }

    return ch_type;
}

static unsigned int iso8859_6_bidi_glyph_type (Glyph32 glyph_value)
{
    unsigned int ch_type = BIDI_TYPE_AL;

    ch_type = bidi_glyph_type(FONT_CHARSET_ISO8859_6, glyph_value);

    return ch_type;
}

static int get_table_index(Uint8 c)
{
    /* is arabic letter range. */
    if (c < 0xc0 || c == 0xe0) {
        return -1;
    }
    if (c > 0xea) {
        return -1;
    }
    /* first continue char range.*/
    if ((c >= 0xc1) && (c <= 0xda)) {
        return c - 0xc1;
    }
    /* second continue char shape range.*/
    if ((c > 0xe0) && (c <= 0xea)) {
        return (c - 0xe0) + (0xda- 0xc1);
    }
    return -1;
}

/* must attetion the para s is arabic letter value.*/
static int is_arabic_letter_vowel(Uint8 s)
{
    if ((s >= 0xeb) && (s <= 0xf2)) return 1;
    return 0;
}

/* arabic letter will affect shape, ascii or space will not. */
static int is_char_transparent(Uint8 c)
{
    BOOL is_ascii = (c < 0x7f) ? 1 : 0;
    BOOL is_space  = ((c == 0x20) || (c == 0xa0)) ? 1 : 0;
    BOOL is_punctuation = ((c == 0xac) || (c == 0xbb)
            || (c == 0xbf) || (c == DOLLAR)) ? 1 : 0;
    BOOL is_vowel = is_arabic_letter_vowel(c);
    if(is_ascii || is_space || is_punctuation || is_vowel)
        return 0;
    else return 1;
}

static int get_next_char(const unsigned char* mchar, int len)
{
    int next_char = *mchar;
    int left_bytes = len, len_cur_char = 0;

    /* skip all vowel, get the next_char. */
    if(len <= 1) return 0;
    while (left_bytes > 0) {
        len_cur_char = iso8859_6_len_first_char(mchar, left_bytes);
        if (len_cur_char > 0 && is_arabic_letter_vowel(*(mchar+len_cur_char))) {
            left_bytes -= len_cur_char;
            mchar += len_cur_char;
        }
        else{
            next_char = *(mchar+len_cur_char);
            break;
        }
    }
    return next_char;
}

Glyph32 iso8859_6_char_glyph_value (const unsigned char* prev_mchar, int prev_len, const unsigned char* mchar, int len)
{
    BOOL next_affects_joining = FALSE, prev_affects_joining = FALSE;
    int char_index, prev_index;
    int final, initial, medial, ligature;
    int ignore;
    char next_char = 0; // NUV , prev_char = 0;

    /*ascii*/
    if(*mchar < 0x7f) return *mchar;

    char_index = get_table_index(*mchar);

    /*current glyph has no transfiguration and has no ligature*/
    if (char_index < 0 && !ISARABIC_PUNC(*mchar) 
        && !ISARABIC_VOWEL(*mchar) && !(*mchar == TADWEEL)) {
        return *mchar;
    }

    if(prev_mchar){
        prev_index = get_table_index(*prev_mchar);
        prev_affects_joining = ( prev_index >= 0 || is_char_transparent(*prev_mchar)) && (shape_info[prev_index].medial);
        // NUV prev_char = *prev_mchar;
        /*houhh 20080505, check if ligature. */
        if(prev_affects_joining) {
            ligature = get_ligature(prev_mchar, prev_len, 1, &ignore);
            if(ligature > 0) 
                prev_affects_joining = 0; 
        }
    }

    /* processing ligature first.*/
    ligature = get_ligature(mchar, len, prev_affects_joining, &ignore);

    if (ligature > 0) 
        return ligature;

    /* if not ligature char, get it's relative shape from shape_info table.*/
    next_char = get_next_char(mchar, len);
    next_affects_joining = ((get_table_index(next_char) > 0 || is_char_transparent(next_char)) && (shape_info[char_index].medial));

    /* 1.prev and next char not affect, return isoliate */
    if ((!prev_affects_joining) && (!next_affects_joining)) {
        return shape_info[char_index].isolated;
    }
    /* 2.only next char affect，if has Initial，return Initial; else return Isolated */
    else if ((!prev_affects_joining) && (next_affects_joining)) { 
        initial = shape_info[char_index].initial;
        if (initial) 
            return initial;
        else 
            return shape_info[char_index].isolated;  
    } 
    /* 3.prev and next char affect all，if has Medial， return Medial; else Isolated */
    else if ((prev_affects_joining) && (next_affects_joining)) { 
        medial = shape_info[char_index].medial;
        if (medial) 
            return medial;
        else 
            return shape_info[char_index].isolated;
    } 
    /* 4.only prev char affect，if has Final, return Final; else Isolated */
    else if ((prev_affects_joining) && (!next_affects_joining)) { 
        final = shape_info[char_index].final;
        if (final)
            return final;
        else 
            return shape_info[char_index].isolated;
    }

    return *mchar;
}

static const unsigned char* iso8859_6_get_next_word (const unsigned char* mstr,
        int mstrlen, WORDINFO* word_info)
{
    int i;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    if (mstrlen == 0) return NULL;

    for (i = 0; i < mstrlen; i++) {
        switch (mstr[i]) {
            case 0xa0:
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if (word_info->delimiter == '\0') {
                    word_info->delimiter = mstr[i];
                    word_info->nr_delimiters ++;
                }
                else if (word_info->delimiter == mstr[i])
                    word_info->nr_delimiters ++;
                else
                    return mstr + word_info->len + word_info->nr_delimiters;
                break;

            default:
                if (word_info->delimiter != '\0')
                    break;

                word_info->len++;
        }
    }
            
    return mstr + word_info->len + word_info->nr_delimiters;

    //return sb_get_next_word(mstr, mstrlen, word_info);
}

static Glyph32* iso8859_6_bidi_str_reorder (Glyph32* glyphs, int len)
{

    return bidi_str_reorder (FONT_CHARSET_ISO8859_6, glyphs, len);
}


/*if cur_len>1, search ligature shape, 
 * else search letter or phonetic symbol shape*/
static Glyph32 iso8859_6_glyph_shape (const unsigned char* cur_mchar, 
        int cur_len, int shape_type)
{
    Glyph32 glyph_value = -1;
    int ignore;
    int index;

    if (cur_len > 1)
    {
        if (shape_type == GLYPH_ISOLATED)
            glyph_value = get_ligature (cur_mchar, cur_len, FALSE, &ignore);
        else if (shape_type != GLYPH_INITIAL) {
            /* LAM+ALEF+HAMAZ+MADDA ligature. */
            if (*cur_mchar == LAM)
                glyph_value = fontset_68x_get_ligature_glyph(*(cur_mchar+1), TRUE, &ignore);
        }
    }

    if (glyph_value == -1 || glyph_value == 0) {
        if (ISARABIC_VOWEL(*cur_mchar)){
            glyph_value = fontset_68x_get_vowel_glyph(*cur_mchar);
        }

        if (glyph_value == 0 || glyph_value == -1) {
            index = get_table_index(*cur_mchar);
            if (index >=0)
            {
                switch (shape_type) {
                    case GLYPH_ISOLATED:
                        glyph_value = shape_info[index].isolated;
                        break;
                    case GLYPH_FINAL:
                        glyph_value = shape_info[index].final;
                        break;
                    case GLYPH_INITIAL:
                        glyph_value = shape_info[index].initial;
                        break;
                    case GLYPH_MEDIAL:
                        glyph_value = shape_info[index].medial;
                        break;
                }
            }
            else
                glyph_value = *cur_mchar;
        }
    }

    if (glyph_value == 0) 
        glyph_value =-1;

    return glyph_value;
}

static CHARSETOPS CharsetOps_iso8859_6 = {
    256,
    3,
    FONT_CHARSET_ISO8859_6,
    0,
    iso8859_6_len_first_char,
    iso8859_6_char_glyph_value,
    iso8859_6_glyph_shape,
    iso8859_6_bidi_glyph_type,
    iso8859_6_glyph_type,
    sb_nr_chars_in_str,
    iso8859_6_is_this_charset,
    sb_len_first_substr,
    iso8859_6_get_next_word,
    sb_pos_first_char,
    iso8859_6_bidi_str_reorder, 
#ifdef _MGCHARSET_UNICODE
    iso8859_6_conv_to_uc32,
    iso8859_6_conv_from_uc32
#endif
};

#ifdef _DEBUG
int test_glyph_value (int char_index, Glyph32 glyph_value)
{
    int i = 0;
    for (i = 0; i<SHAPENUMBER; i++) {
        if (shape_info[i].isolated == glyph_value ||
            shape_info[i].initial == glyph_value ||
            shape_info[i].medial == glyph_value ||
            shape_info[i].final == glyph_value )
            if( char_index == i)
                return 1;
    }
    return 0;
}
#endif


#endif /* _ARABIC */
