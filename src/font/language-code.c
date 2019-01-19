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

int GUIAPI LanguageCodeFromISO639s1 (Uint16 iso639_1)
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
            return (int)mid;

        mid = (lower + upper) / 2;
    } while (lower <= upper);

    return LANGCODE_unknown;
}

#endif /* _MGCHARSET_UNICODE */

