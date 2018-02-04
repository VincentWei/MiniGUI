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
** keyboard.c: scancode to keycode, the new TranslateMessage implementation.
** 
** Some code from Linux Kernel.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "linux_types.h"
#include "linux_keyboard.h"
#include "linux_kd.h"

#include "keyboard.h"

ushort **__mg_key_maps;
struct kbdiacr *__mg_accent_table;
unsigned int __mg_accent_table_size;
char **__mg_func_table;

static u_short arabic_compose_ch =0xf200;
static int     arabic_compose_flag =0;

typedef void (*k_hand) (unsigned char value, key_info* kinfo);
typedef void (k_handfn) (unsigned char value, key_info* kinfo);

static k_handfn
    do_self, do_fn, do_spec, do_pad, do_dead, do_cur, do_shift,
    do_meta, do_ascii, do_dead2;

static k_hand key_handler[16] = {
    do_self, do_fn, do_spec, do_pad, do_dead, NULL, do_cur, NULL,
    do_meta, do_ascii, NULL, NULL, NULL, do_dead2,
    NULL, NULL 
};

typedef void (*void_fnp) (key_info* kinfo);
typedef void (void_fn) (key_info* kinfo);

static void_fn enter, compose;

static void_fnp spec_fn_table[] = {
    NULL,    enter,    NULL,       NULL,
    NULL,    NULL,     NULL,       NULL,
    NULL,    NULL,     NULL,       NULL,
    NULL,    NULL,     compose,    NULL,
    NULL,    NULL,     NULL,       NULL
};

static inline void put_queue (char ch, key_info* kinfo)
{
    kinfo->buff [kinfo->pos] = ch;
    kinfo->pos ++;
}

static inline void puts_queue (char* cp, key_info* kinfo)
{
    while (*cp) {
        kinfo->buff [kinfo->pos] = *cp;
        kinfo->pos ++;

        cp++;
    }
}

static void applkey (int key, char mode, key_info* kinfo)
{
    static char buf[] = { 0x1b, 'O', 0x00, 0x00 };

    buf[1] = (mode ? 'O' : '[');
    buf[2] = key;
    puts_queue (buf, kinfo);
}

/*
 * Many other routines do put_queue, but I think either
 * they produce ASCII, or they produce some user-assigned
 * string, and in both cases we might assume that it is
 * in utf-8 already.
 */
static void to_utf8 (ushort c, key_info* kinfo)
{
    if (c < 0x80)
        put_queue(c, kinfo);                /* 0*******  */
    else if (c < 0x800) {
        put_queue(0xc0 | (c >> 6), kinfo);  /*  110***** 10******  */
        put_queue(0x80 | (c & 0x3f), kinfo);
    } else {
        put_queue(0xe0 | (c >> 12), kinfo); /*  1110**** 10****** 10******  */
        put_queue(0x80 | ((c >> 6) & 0x3f), kinfo);
        put_queue(0x80 | (c & 0x3f), kinfo);
    }
    
    /* UTF-8 is defined for words of up to 31 bits,
       but we need only 16 bits here */
}

#define A_GRAVE  '`'
#define A_ACUTE  '\''
#define A_CFLEX  '^'
#define A_TILDE  '~'
#define A_DIAER  '"'
#define A_CEDIL  ','
static unsigned char ret_diacr[NR_DEAD] =
    {A_GRAVE, A_ACUTE, A_CFLEX, A_TILDE, A_DIAER, A_CEDIL };

/*
 * We have a combining character DIACR here, followed by the character CH.
 * If the combination occurs in the table, return the corresponding value.
 * Otherwise, if CH is a space or equals DIACR, return DIACR.
 * Otherwise, conclude that DIACR was not combining after all,
 * queue it and return CH.
 */
static unsigned char handle_diacr (unsigned char ch, key_info* kinfo)
{
    int d = kinfo->diacr;
    int i;

    kinfo->diacr = 0;

    for (i = 0; i < __mg_accent_table_size; i++) {
        if (__mg_accent_table[i].diacr == d && __mg_accent_table[i].base == ch)
            return __mg_accent_table[i].result;
    }

    if (ch == ' ' || ch == d)
        return d;

    put_queue (d, kinfo);
    return ch;
}

static void do_dead (unsigned char value, key_info* kinfo)
{
    value = ret_diacr [value];
    do_dead2 (value, kinfo);
}

/*
 * Handle dead key. Note that we now may have several
 * dead keys modifying the same character. Very useful
 * for Vietnamese.
 */
static void do_dead2 (unsigned char value, key_info* kinfo)
{
    kinfo->diacr = (kinfo->diacr ? handle_diacr (value, kinfo) : value);
}

static void do_self (unsigned char value, key_info* kinfo)
{
    if (kinfo->diacr) {
        value = handle_diacr (value, kinfo);
    }

    if (kinfo->dead_key_next) {
        kinfo->dead_key_next = 0;
        kinfo->diacr = value;
        return;
    }

    if(arabic_compose_ch !=0xf200){
        unsigned char buf[3]={0};

        buf[0] =value;
        buf[1] =(arabic_compose_ch & 0xff);
        puts_queue((char*)buf,kinfo);
        arabic_compose_ch =0xf200;
    }
    else{
        put_queue (value, kinfo);
    }
}

#define SIZE(x) (sizeof(x)/sizeof((x)[0]))

static void do_fn (unsigned char value, key_info* kinfo)
{
    if (__mg_func_table [value])
        puts_queue (__mg_func_table [value], kinfo);
}

static void do_pad(unsigned char value, key_info* kinfo)
{
    static const char *pad_chars = "0123456789+-*/\015,.?()";
    static const char *app_map = "pqrstuvwxylSRQMnnmPQ";

    /* kludge... shift forces cursor/number keys */
    if ( (kinfo->kbd_mode & VC_APPLIC) && !(kinfo->shiftstate & KS_SHIFT)) {
        applkey (app_map[value], 1, kinfo);
        return;
    }

    if (!(kinfo->shiftstate & KS_NUMLOCK))
        switch (value) {
            case KVAL(K_PCOMMA):
            case KVAL(K_PDOT):
                do_fn (KVAL(K_REMOVE), kinfo);
                return;
            case KVAL(K_P0):
                do_fn (KVAL(K_INSERT), kinfo);
                return;
            case KVAL(K_P1):
                do_fn (KVAL(K_SELECT), kinfo);
                return;
            case KVAL(K_P2):
                do_cur (KVAL(K_DOWN), kinfo);
                return;
            case KVAL(K_P3):
                do_fn (KVAL(K_PGDN), kinfo);
                return;
            case KVAL(K_P4):
                do_cur (KVAL(K_LEFT), kinfo);
                return;
            case KVAL(K_P6):
                do_cur (KVAL(K_RIGHT), kinfo);
                return;
            case KVAL(K_P7):
                do_fn (KVAL(K_FIND), kinfo);
                return;
            case KVAL(K_P8):
                do_cur (KVAL(K_UP), kinfo);
                return;
            case KVAL(K_P9):
                do_fn (KVAL(K_PGUP), kinfo);
                return;
            case KVAL(K_P5):
                applkey ('G', kinfo->kbd_mode & VC_APPLIC, kinfo);
                return;
        }

    put_queue (pad_chars [value], kinfo);
    if (value == KVAL(K_PENTER) && (kinfo->kbd_mode & VC_CRLF))
        put_queue (10, kinfo);
}

static void do_cur (unsigned char value, key_info* kinfo)
{
    static const char *cur_chars = "BDCA";

    applkey (cur_chars [value], kinfo->kbd_mode & VC_CKMODE, kinfo);
}

static void do_meta (unsigned char value, key_info* kinfo)
{
    if (kinfo->kbd_mode & VC_META) {
        put_queue ('\033', kinfo);
        put_queue (value, kinfo);
    } else
        put_queue (value | 0x80, kinfo);
}

static void do_ascii (unsigned char value, key_info* kinfo)
{
    int base;

    if (value < 10)    /* decimal input of code, while Alt depressed */
        base = 10;
    else {       /* hexadecimal input of code, while AltGr depressed */
        value -= 10;
        base = 16;
    }

    if (kinfo->npadch == -1)
        kinfo->npadch = value;
    else
        kinfo->npadch = kinfo->npadch * base + value;
}

static void do_shift (unsigned char value, key_info* kinfo)
{
    /* kludge */
    if ((kinfo->shiftstate != kinfo->oldstate) && (kinfo->npadch != -1)) {
        if (kinfo->kbd_mode & VC_UNICODE)
            to_utf8 (kinfo->npadch & 0xffff, kinfo);
        else
            put_queue (kinfo->npadch & 0xff, kinfo);
        kinfo->npadch = -1;
    }
}

static void do_spec (unsigned char value, key_info* kinfo)
{
    if (value >= SIZE(spec_fn_table))
        return;

    if (spec_fn_table [value])
        spec_fn_table [value] (kinfo);
}

static void enter (key_info* kinfo)
{
    if (kinfo->diacr) {
        put_queue (kinfo->diacr, kinfo);
        kinfo->diacr = 0;
    }
    put_queue (13, kinfo);
    
    if (kinfo->kbd_mode & VC_CRLF)
        put_queue (10, kinfo);
}

static void compose (key_info* kinfo)
{
    kinfo->dead_key_next = 1;
}

static int compute_shiftstate (DWORD shiftstate)
{
    int shift_final = 0;

    if (__mg_key_maps [1]) {
        if (shiftstate & KS_SHIFT) {
            shift_final += 1 << KG_SHIFT;
        }
    }
    else {
        if (shiftstate & KS_LEFTSHIFT){
            shift_final += 1 << KG_SHIFTL;
        }
        if (shiftstate & KS_RIGHTSHIFT){
            shift_final += 1 << KG_SHIFTR;
        }
    }

    if (__mg_key_maps [4]) {
        if (shiftstate & KS_CTRL) {
            shift_final += 1 << KG_CTRL;
        }
    }
    else {
        if (shiftstate & KS_LEFTCTRL)
            shift_final += 1 << KG_CTRLL;
        if (shiftstate & KS_RIGHTCTRL)
            shift_final += 1 << KG_CTRLR;
    }

    if (shiftstate & KS_LEFTALT)
        shift_final += 1 << KG_ALT;
    if (shiftstate & KS_RIGHTALT)
        shift_final += 1 << KG_ALTGR;

    return shift_final;
}


/* Get the compose character of a arabic symbol 
 * int shift_final : the state of the shift key. 0 key up   1 key down 
 * int scancode : the scan code of the key pressed down 
 * ushort * key_map : keyboard value table in which a keyboard value can be found acoording to 
 *                    the scancode*/
static inline void get_arabic_compose_character(int shift_final, int scancode, ushort *key_map)
{
    if(!shift_final){
        if(__mg_key_maps[3]){
            arabic_compose_ch =__mg_key_maps[3][scancode];
            arabic_compose_flag =1;
        }
        else{
            arabic_compose_ch =0xf200;
            arabic_compose_flag =0;
        }
    }
    else if(1 ==shift_final){
        arabic_compose_ch = key_map[scancode +1];
        arabic_compose_flag =1;
    }
}

/*
 * Translation of escaped scancodes to keycodes.
 */
static void handle_scancode_on_keydown (int scancode, key_info* kinfo)
{
    u_short keysym;
    int shift_final;
    ushort *key_map;
    
    shift_final = compute_shiftstate (kinfo->shiftstate);

    key_map = __mg_key_maps [shift_final];
    if (key_map != NULL) {
        keysym = key_map [scancode];
        kinfo->type = HIBYTE (keysym);

        if (kinfo->type >= 0xf0) {
            kinfo->type -= 0xf0;
            if (kinfo->type == KT_LETTER) {
                if (kinfo->shiftstate & KS_CAPSLOCK) {
                    key_map = __mg_key_maps [shift_final ^ (1<<KG_SHIFT)];
                    if (key_map)
                      keysym = key_map [scancode];

                    kinfo->type = HIBYTE (keysym);
                    /*Now, the kinfo->type maybe changed to KT_ARABIC_COMPOSE.
                     * So we have to deal with this situation */
                    if(kinfo->type >= 0xf0){
                        kinfo->type -= 0xf0;
                        if(kinfo->type ==KT_ARABIC_COMPOSE){
                            get_arabic_compose_character(shift_final ^ (1<<KG_SHIFT),scancode,key_map);
                        }
                    }
                }
                kinfo->type =KT_LATIN;
            }
            else if(kinfo->type == KT_ARABIC_COMPOSE){
                if(kinfo->shiftstate & KS_CAPSLOCK){
                    key_map = __mg_key_maps[shift_final ^(1<<KG_SHIFT)];
                    if (key_map)
                      keysym = key_map [scancode];
                    kinfo->type = HIBYTE (keysym);

                    if(kinfo->type >= 0xf0){
                        kinfo->type -= 0xf0;
                        if(kinfo->type ==KT_ARABIC_COMPOSE){
                            get_arabic_compose_character(shift_final ^ (1<<KG_SHIFT),scancode,key_map);
                        }
                    }
                }
                else{
                    get_arabic_compose_character(shift_final,scancode,key_map);
                }
                kinfo->type =KT_LATIN;
            }
            if (key_handler [kinfo->type])
                (*key_handler [kinfo->type]) (keysym & 0xff, kinfo);
        }
        else {
            to_utf8 (keysym, kinfo);
        }
    }
}

static void handle_scancode_on_keyup (int scancode, key_info* kinfo)
{
    u_short keysym;
    int shift_final;
    ushort *key_map;
    
    shift_final = compute_shiftstate (kinfo->shiftstate);

    key_map = __mg_key_maps [shift_final];
    if (key_map != NULL) {
        keysym = key_map [scancode];
        kinfo->type = HIBYTE (keysym);

        if (kinfo->type >= 0xf0) {
            kinfo->type -= 0xf0;
            if (kinfo->type == KT_SHIFT)
                do_shift (keysym & 0xff, kinfo);
        }
    }
}

kbd_layout_info __mg_layouts [] =
{
    {KBD_LAYOUT_DEFAULT, init_default_kbd_layout},
#ifdef _MGKBDLAYOUT_HEBREWPC
    {KBD_LAYOUT_HEBREW, init_hebrew_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_ARABICPC
    {KBD_LAYOUT_ARABIC, init_arabic_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_FRPC
    {KBD_LAYOUT_FRPC, init_frpc_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_FR
    {KBD_LAYOUT_FR, init_fr_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_DE
    {KBD_LAYOUT_DE, init_de_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_DELATIN1
    {KBD_LAYOUT_DELATIN1, init_delatin1_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_IT
    {KBD_LAYOUT_IT, init_it_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_ES
    {KBD_LAYOUT_ES, init_es_kbd_layout},
#endif
#ifdef _MGKBDLAYOUT_ESCP850
    {KBD_LAYOUT_ESCP850, init_escp850_kbd_layout}
#endif

};

/* [humingming/2010/10/19]: bug 5157: the __mg_kinfo use the global variable
 * will cause multi-thread problem when in thread mode and create mgi window. */
//static key_info __mg_kinfo = {VC_XLATE, 0, 0, -1};
static DWORD __mg_kinfo_kbd_mode = VC_XLATE;
static int __mg_kinfo_npadch = -1;

BOOL GUIAPI SetKeyboardLayout (const char* kbd_layout)
{
    int i;

    if (!kbd_layout)
        return FALSE;
    
    for (i = 0; i < TABLESIZE(__mg_layouts); i++) {
        if (strcmp (__mg_layouts[i].name, kbd_layout) == 0) {
            __mg_layouts [i].init (&__mg_key_maps, &__mg_accent_table, &__mg_accent_table_size, &__mg_func_table);
            //memset (&__mg_kinfo, 0, sizeof (key_info));
            //__mg_kinfo.kbd_mode = VC_XLATE;
            //__mg_kinfo.npadch = -1;
            __mg_kinfo_kbd_mode = VC_XLATE;
            __mg_kinfo_npadch = -1;
            return TRUE;
        }
    }

    fprintf (stderr,"GUI: can not find the layout name:%s\n", kbd_layout);

    return FALSE;
}

#if 0
BOOL GUIAPI TranslateMessage (PMSG pMsg)
{
    int i;

    __mg_kinfo.pos = 0;

    if ((pMsg->hwnd != HWND_DESKTOP)) {
        if ((pMsg->message == MSG_KEYDOWN || pMsg->message == MSG_SYSKEYDOWN) &&
                pMsg->wParam < SCANCODE_USER) {
            __mg_kinfo.shiftstate = pMsg->lParam;
            handle_scancode_on_keydown (pMsg->wParam, &__mg_kinfo);
            __mg_kinfo.oldstate = pMsg->lParam;
        }
        else if ((pMsg->message == MSG_KEYLONGPRESS 
                                || pMsg->message == MSG_KEYALWAYSPRESS 
                                || pMsg->message == MSG_KEYUP 
                                || pMsg->message == MSG_SYSKEYUP) 
                        && pMsg->wParam < SCANCODE_USER) {
            __mg_kinfo.shiftstate = pMsg->lParam;
            handle_scancode_on_keyup (pMsg->wParam, &__mg_kinfo);
            __mg_kinfo.oldstate = pMsg->lParam;
        }
    }

    if (__mg_kinfo.pos == 1) {
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, __mg_kinfo.buff[0], pMsg->lParam);
    }
    else if (__mg_kinfo.pos == 2 && arabic_compose_flag) {
#if 0
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, 
                MAKEWPARAM (__mg_kinfo.buff[0], __mg_kinfo.buff[1], 0, 0), pMsg->lParam);
#else
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, __mg_kinfo.buff[0], pMsg->lParam);
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, __mg_kinfo.buff[1], pMsg->lParam);
#endif
        arabic_compose_flag =0;
    }
    else {
        for (i = 0; i < __mg_kinfo.pos; i++)
            SendNotifyMessage (pMsg->hwnd, MSG_KEYSYM, 
                        MAKEWORD (__mg_kinfo.buff[i], i), pMsg->lParam);
    }

    return FALSE; 
}
#endif

BOOL GUIAPI TranslateMessage (PMSG pMsg)
{
    int i;
    key_info kinfo;

    memset (&kinfo, 0, sizeof (key_info));
    kinfo.kbd_mode = __mg_kinfo_kbd_mode;
    kinfo.npadch = __mg_kinfo_npadch;
    kinfo.pos = 0;

    if ((pMsg->hwnd != HWND_DESKTOP)) {
        if ((pMsg->message == MSG_KEYDOWN || pMsg->message == MSG_SYSKEYDOWN) &&
                pMsg->wParam < SCANCODE_USER) {
            kinfo.shiftstate = pMsg->lParam;
            handle_scancode_on_keydown (pMsg->wParam, &kinfo);
            kinfo.oldstate = pMsg->lParam;
        }
        else if ((pMsg->message == MSG_KEYLONGPRESS 
                                || pMsg->message == MSG_KEYALWAYSPRESS 
                                || pMsg->message == MSG_KEYUP 
                                || pMsg->message == MSG_SYSKEYUP) 
                        && pMsg->wParam < SCANCODE_USER) {
            kinfo.shiftstate = pMsg->lParam;
            handle_scancode_on_keyup (pMsg->wParam, &kinfo);
            kinfo.oldstate = pMsg->lParam;
        }
    }

    if (kinfo.pos == 1) {
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, kinfo.buff[0], pMsg->lParam);
    }
    else if (kinfo.pos == 2 && arabic_compose_flag) {
#if 0
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, 
                MAKEWPARAM (__mg_kinfo.buff[0], __mg_kinfo.buff[1], 0, 0), pMsg->lParam);
#else
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, kinfo.buff[0], pMsg->lParam);
        SendNotifyMessage (pMsg->hwnd, MSG_CHAR, kinfo.buff[1], pMsg->lParam);
#endif
        arabic_compose_flag =0;
    }
    else {
        for (i = 0; i < kinfo.pos; i++)
            SendNotifyMessage (pMsg->hwnd, MSG_KEYSYM, 
                        MAKEWORD (kinfo.buff[i], i), pMsg->lParam);
    }

    return FALSE; 
}

BOOL GUIAPI TranslateKeyMsgToChar (int message, WPARAM wParam, LPARAM lParam, WORD *ch)
{
    key_info ki = {VC_XLATE, 0, 0, -1};

    ki.pos = 0;

    if ((message == MSG_KEYDOWN || message == MSG_SYSKEYDOWN) &&
            wParam < SCANCODE_USER) {
        ki.shiftstate = lParam;
        handle_scancode_on_keydown (wParam, &ki);
        ki.oldstate = lParam;
    }
    else if ((message == MSG_KEYUP || message == MSG_SYSKEYUP) &&
            wParam < SCANCODE_USER) {
        ki.shiftstate = lParam;
        handle_scancode_on_keyup (wParam, &ki);
        ki.oldstate = lParam;
    }

    if (ki.pos == 1) {
        *ch = ki.buff[0];
        return TRUE;
    }

    return FALSE; 
}

#ifdef _MGCHARSET_UNICODE

static UChar32 utf8_to_wc (const unsigned char* mstr, int* utf8_len)
{
    UChar32 wc = *((unsigned char *)(mstr++));
    int n, t;

    *utf8_len = 1;
    if (wc & 0x80) {
        n = 1;
        while (wc & (0x80 >> n))
            n++;

        wc &= (1 << (8-n)) - 1;
        while (--n > 0) {
            t = *((unsigned char *)(mstr++));
            (*utf8_len)++;

            wc = (wc << 6) | (t & 0x3F);
        }
    }

    return wc;
}

int GUIAPI ToUnicode (UINT keycode, const BYTE* kbd_state, 
        wchar_t* wcs, int wcs_len, const char* kbd_layout)
{
    DWORD shiftstate = 0;
    key_info ki = {VC_XLATE | VC_UNICODE, 0, 0, -1};
    const unsigned char* utf8_str;
    int utf8_str_len, nr_wcs;

    ki.pos = 0;

    if (kbd_state [SCANCODE_LEFTSHIFT])
        shiftstate |= KS_LEFTSHIFT;
    if (kbd_state [SCANCODE_RIGHTSHIFT])
        shiftstate |= KS_RIGHTSHIFT;
    if (kbd_state [SCANCODE_LEFTCONTROL])
        shiftstate |= KS_LEFTCTRL;
    if (kbd_state [SCANCODE_RIGHTCONTROL])
        shiftstate |= KS_RIGHTCTRL;
    if (kbd_state [SCANCODE_LEFTALT])
        shiftstate |= KS_LEFTALT;
    if (kbd_state [SCANCODE_RIGHTALT])
        shiftstate |= KS_RIGHTALT;

    ki.shiftstate = shiftstate;
    handle_scancode_on_keydown (keycode, &ki);
    ki.oldstate = shiftstate;

    ki.shiftstate = shiftstate;
    handle_scancode_on_keyup (keycode, &ki);
    ki.oldstate = shiftstate;

    utf8_str = ki.buff;
    utf8_str_len = ki.pos;
    nr_wcs = 0;
    while (utf8_str_len > 0 && wcs_len > nr_wcs) {
        int utf8_char_len;
        wcs [nr_wcs] = utf8_to_wc (utf8_str, &utf8_char_len);

        utf8_str += utf8_char_len;
        utf8_str_len -= utf8_char_len; 
        nr_wcs ++;
    }
    
    return nr_wcs;
}

int GUIAPI ToAscii (UINT keycode, const BYTE* kbd_state, 
        WORD* ch, const char* kbd_layout)
{
    DWORD shiftstate = 0;
    key_info ki = {VC_XLATE, 0, 0, -1};

    ki.pos = 0;

    if (kbd_state [SCANCODE_LEFTSHIFT])
        shiftstate |= KS_LEFTSHIFT;
    if (kbd_state [SCANCODE_RIGHTSHIFT])
        shiftstate |= KS_RIGHTSHIFT;
    if (kbd_state [SCANCODE_LEFTCONTROL])
        shiftstate |= KS_LEFTCTRL;
    if (kbd_state [SCANCODE_RIGHTCONTROL])
        shiftstate |= KS_RIGHTCTRL;
    if (kbd_state [SCANCODE_LEFTALT])
        shiftstate |= KS_LEFTALT;
    if (kbd_state [SCANCODE_RIGHTALT])
        shiftstate |= KS_RIGHTALT;

    ki.shiftstate = shiftstate;
    handle_scancode_on_keydown (keycode, &ki);
    ki.oldstate = shiftstate;

    ki.shiftstate = shiftstate;
    handle_scancode_on_keyup (keycode, &ki);
    ki.oldstate = shiftstate;

    if (ki.pos == 1) {
        *ch = ki.buff[0];
        return 1;
    }

    return 0;
}

#endif
