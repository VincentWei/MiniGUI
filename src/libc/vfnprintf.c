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
** vfnprintf.c: implementation of own_vfnprintf function.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02 by Wei yongming
*/

/*
** This code is based on original code with the following copyright:
*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdarg.h>               // Variable argument definitions
#include <stdio.h>                // Standard header for all stdio files
#include <string.h>               // memchr() and strlen() functions

#include "common.h"

#if defined (_MGUSE_OWN_STDIO) && defined (_MGUSE_OWN_VFNPRINTF) 

#include "endianrw.h"

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#ifdef _PRINTF_FLOATING_POINT

#include <float.h>
#include <math.h>
#include "ieeefp.h"

#ifdef __VXWORKS__
#include <private/mathP.h>
#define isnan  isNan
#define finite(z) !isInf(z)
#endif

#define MAXFRACT    DBL_DIG
#define MAXEXP      DBL_MAX_10_EXP

#define BUF         (MAXEXP+MAXFRACT+1)     /* + decimal point */
#define DEFPREC     6

static int cvt (double, int, int, char *, int, char *, char * );

#else

#define BUF         40

#endif /* _PRINTF_FLOATING_POINT */

/*
 * Actual printf innards.
 *
 * This code is large and complicated...
 */

#ifdef _I18N_MB_REQUIRED
typedef int (*mbtowc_fn_type)(wchar_t *, const char *, size_t, int *);
mbtowc_fn_type __get_current_locale_mbtowc_fn();
#endif

/*
 * Macros for converting digits to letters and vice versa
 */
#define to_digit(c)     ((c) - '0')
#define is_digit(c)     ((unsigned)to_digit(c) <= 9)
#define to_char(n)      ((n) + '0')

/*
 * Flags used during conversion.
 */
#define ALT             0x001           /* alternate form */
#define HEXPREFIX       0x002           /* add 0x or 0X prefix */
#define LADJUST         0x004           /* left adjustment */
#define LONGDBL         0x008           /* long double; unimplemented */
#define LONGINT         0x010           /* long integer */
#define QUADINT         0x020           /* quad integer */
#define SHORTINT        0x040           /* short integer */
#define ZEROPAD         0x080           /* zero (as opposed to blank) pad */
#define FPT             0x100           /* Floating point number */
#define SIZET           0x200           /* size_t */

int own_vfnprintf ( FILE *stream, size_t n, const char *format, va_list arg)
{
        char *fmt;     /* format string */
        int ch;        /* character from fmt */
        int x, y;      /* handy integers (short term usage) */
        char *cp;      /* handy char pointer (short term usage) */
        int flags;     /* flags as above */

#ifdef _I18N_MB_REQUIRED
        int state = 0; /* state for mbtowc conversion */
        mbtowc_fn_type mbtowc_fn;
#endif

        int ret;                /* return value accumulator */
        int width;              /* width from format (%8d), or 0 */
        int prec;               /* precision from format (%.3d), or -1 */
        char sign;              /* sign prefix (' ', '+', '-', or \0) */
        wchar_t wc;

#define quad_t    Sint64
#define u_quad_t  Uint64

        u_quad_t _uquad;        /* integer arguments %[diouxX] */
        enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
        int dprec;              /* a copy of prec if [diouxX], 0 otherwise */
        int fieldsz;            /* field size expanded by sign, etc */
        int realsz;             /* field size expanded by dprec */
        int size;               /* size of converted field or string */
        char *xdigs;            /* digits for [xX] conversion */
#define NIOV 8
        char buf[BUF];          /* space for %c, %[diouxX], %[eEfgG] */
        char ox[2];             /* space for 0x hex-prefix */
#ifdef _PRINTF_FLOATING_POINT
        char softsign;          /* temporary negative sign for floats */
        double _double;         /* double precision arguments %[eEfgG] */
        int fpprec;             /* `extra' floating precision in [eEfgG] */
#endif

        /*
         * Choose PADSIZE to trade efficiency vs. size.  If larger printf
         * fields occur frequently, increase PADSIZE and make the initialisers
         * below longer.
         */
#define PADSIZE 16              /* pad chunk size */
        static char blanks[PADSIZE] =
         {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        static char zeroes[PADSIZE] =
         {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

        /*
         * BEWARE, these `goto error' on error, and PAD uses `n'.
         */
#define PRINT(ptr, len)                                                      \
_MACRO_START                                                                 \
    Uint32 length = MIN( (Uint32) len, n - ret - 1);                         \
    Uint32 written;                                                          \
    if ((written = ((MG_RWops*)stream)->write( (MG_RWops*)stream,            \
                        (const Uint8 *)ptr, 1, length)) != length)           \
        goto error;                                                          \
    if (written < (Uint32)len) {                                             \
        ret += written;                                                      \
        goto done;                                                           \
    }                                                                        \
_MACRO_END


#define PAD(howmany, with)                                                   \
_MACRO_START                                                                 \
    if ((x = (howmany)) > 0) {                                               \
        while (x > PADSIZE) {                                                \
            PRINT(with, PADSIZE);                                            \
            x -= PADSIZE;                                                    \
        }                                                                    \
        PRINT(with, x);                                                      \
    }                                                                        \
_MACRO_END

        /*
         * To extend shorts properly, we need both signed and unsigned
         * argument extraction methods.
         */

#define UARG() \
        (flags&QUADINT ? va_arg(arg, Uint64) : \
            flags&LONGINT ? va_arg(arg, unsigned long) : \
            flags&SHORTINT ? (unsigned long)(unsigned short)va_arg(arg, int) : \
            flags&SIZET ? va_arg(arg, size_t) : \
            (unsigned long)va_arg(arg, unsigned int))


        xdigs = NULL;  // stop compiler whinging
        fmt = (char *)format;
        ret = 0;
#ifdef _I18N_MB_REQUIRED
        mbtowc_fn = __get_current_locale_mbtowc_fn();
#endif

        /*
         * Scan the format for conversions (`%' character).
         */
        for (;;) {
                cp = (char *)fmt;
#ifndef _I18N_MB_REQUIRED
                while ((x = ((wc = *fmt) != 0))) {
#else
                while ((x = mbtowc_fn (&wc, fmt, MB_CUR_MAX, &state)) > 0) {
#endif
                        fmt += x;
                        if (wc == '%') {
                                fmt--;
                                break;
                        }
                }
                if ((y = fmt - cp) != 0) {
                        PRINT(cp, y);
                        ret += y;
                }
                if ((x <= 0) || (ret >= (int)n))  // @@@ this check with n isn't good enough
                        goto done;
                fmt++;          /* skip over '%' */

                flags = 0;
                dprec = 0;
#ifdef _PRINTF_FLOATING_POINT
                fpprec = 0;
#endif
                width = 0;
                prec = -1;
                sign = '\0';

rflag:          ch = *fmt++;
reswitch:       switch (ch) {
                case ' ':
                        /*
                         * ``If the space and + flags both appear, the space
                         * flag will be ignored.''
                         *      -- ANSI X3J11
                         */
                        if (!sign)
                                sign = ' ';
                        goto rflag;
                case '#':
                        flags |= ALT;
                        goto rflag;
                case '*':
                        /*
                         * ``A negative field width argument is taken as a
                         * - flag followed by a positive field width.''
                         *      -- ANSI X3J11
                         * They don't exclude field widths read from args.
                         */
                        if ((width = va_arg(arg, int)) >= 0)
                                goto rflag;
                        width = -width;
                        /* FALLTHROUGH */
                case '-':
                        flags |= LADJUST;
                        goto rflag;
                case '+':
                        sign = '+';
                        goto rflag;
                case '.':
                        if ((ch = *fmt++) == '*') {
                                x = va_arg(arg, int);
                                prec = x < 0 ? -1 : x;
                                goto rflag;
                        }
                        x = 0;
                        while (is_digit(ch)) {
                                x = 10 * x + to_digit(ch);
                                ch = *fmt++;
                        }
                        prec = x < 0 ? -1 : x;
                        goto reswitch;
                case '0':
                        /*
                         * ``Note that 0 is taken as a flag, not as the
                         * beginning of a field width.''
                         *      -- ANSI X3J11
                         */
                        flags |= ZEROPAD;
                        goto rflag;
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        x = 0;
                        do {
                                x = 10 * x + to_digit(ch);
                                ch = *fmt++;
                        } while (is_digit(ch));
                        width = x;
                        goto reswitch;
#ifdef _PRINTF_FLOATING_POINT
                case 'L':
                        flags |= LONGDBL;
                        goto rflag;
#endif
                case 'h':
                        flags |= SHORTINT;
                        goto rflag;
                case 'l':
                        if (*fmt == 'l') {
                                fmt++;
                                flags |= QUADINT;
                        } else {
                                flags |= LONGINT;
                        }
                        goto rflag;
                case 'q':
                        flags |= QUADINT;
                        goto rflag;
                case 'c':
                        *(cp = buf) = va_arg(arg, int);
                        size = 1;
                        sign = '\0';
                        break;
                case 'D':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'd':
                case 'i':
                    if (flags&QUADINT)
                        _uquad = va_arg(arg, Sint64);
                    else if (flags&LONGINT)
                        _uquad = va_arg(arg, long);
                    else if (flags&SHORTINT)
                        _uquad = (long)(short)va_arg(arg, int);
                    else if (flags&SIZET)
                        _uquad = va_arg(arg, size_t);
                    else
                        _uquad = (long)va_arg(arg, int);
#ifndef _NO_LONGLONG
                        if ((quad_t)_uquad < 0)
#else
                        if ((long) _uquad < 0)
#endif
                        {

                                _uquad = -_uquad;
                                sign = '-';
                        }
                        base = DEC;
                        goto number;

#ifdef _PRINTF_FLOATING_POINT
                case 'e':
                case 'E':
                case 'f':
                case 'g':
                case 'G':
                        _double = va_arg(arg, double);
                        /*
                         * don't do unrealistic precision; just pad it with
                         * zeroes later, so buffer size stays rational.
                         */
                        if (prec > MAXFRACT) {
                                if ((ch != 'g' && ch != 'G') || (flags&ALT))
                                        fpprec = prec - MAXFRACT;
                                prec = MAXFRACT;
                        } else if (prec == -1)
                                prec = DEFPREC;
                        /*
                         * cvt may have to round up before the "start" of
                         * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
                         * if the first character is still NUL, it did.
                         * softsign avoids negative 0 if _double < 0 but
                         * no significant digits will be shown.
                         */
                        cp = buf;
                        *cp = '\0';
                        size = cvt(_double, prec, flags, &softsign, ch,
                            cp, buf + sizeof(buf));
                        if (softsign)
                                sign = '-';
                        if (*cp == '\0')
                                cp++;
                        break;
#else
                case 'e':
                case 'E':
                case 'f':
                case 'g':
                case 'G':
                        // Output nothing at all
                        (void) va_arg(arg, double); // take off arg anyway
                        cp = "";
                        size = 0;
                        sign = '\0';
                        break;
                        
                         
#endif /* _PRINTF_FLOATING_POINT */

                case 'n':
#ifndef _NO_LONGLONG
                        if (flags & QUADINT)
                                *va_arg(arg, quad_t *) = ret;
                        else 
#endif
                        if (flags & LONGINT)
                                *va_arg(arg, long *) = ret;
                        else if (flags & SHORTINT)
                                *va_arg(arg, short *) = ret;
                        else if (flags & SIZET)
                                *va_arg(arg, size_t *) = ret;
                        else
                                *va_arg(arg, int *) = ret;
                        continue;       /* no output */
                case 'O':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'o':
                        _uquad = UARG();
                        base = OCT;
                        goto nosign;
                case 'p':
                        /*
                         * ``The argument shall be a pointer to void.  The
                         * value of the pointer is converted to a sequence
                         * of printable characters, in an implementation-
                         * defined manner.''
                         *      -- ANSI X3J11
                         */
                        /* NOSTRICT */
                        _uquad = (unsigned long)va_arg(arg, void *);
                        base = HEX;
                        xdigs = "0123456789abcdef";
                        flags |= HEXPREFIX;
                        ch = 'x';
                        goto nosign;
                case 's':
                        if ((cp = va_arg(arg, char *)) == NULL)
                                cp = "(null)";
                        if (prec >= 0) {
                                /*
                                 * can't use strlen; can only look for the
                                 * NUL in the first `prec' characters, and
                                 * strlen() will go further.
                                 */
                                char *p = (char *)memchr(cp, 0, prec);

                                if (p != NULL) {
                                        size = p - cp;
                                        if (size > prec)
                                                size = prec;
                                } else
                                        size = prec;
                        } else
                                size = strlen(cp);
                        sign = '\0';
                        break;
                case 'U':
                        flags |= LONGINT;
                        /*FALLTHROUGH*/
                case 'u':
                        _uquad = UARG();
                        base = DEC;
                        goto nosign;
                case 'X':
                        xdigs = "0123456789ABCDEF";
                        goto hex;
                case 'x':
                        xdigs = "0123456789abcdef";
hex:                    _uquad = UARG();
                        base = HEX;
                        /* leading 0x/X only if non-zero */
                        if (flags & ALT && _uquad != 0)
                                flags |= HEXPREFIX;

                        /* unsigned conversions */
nosign:                 sign = '\0';
                        /*
                         * ``... diouXx conversions ... if a precision is
                         * specified, the 0 flag will be ignored.''
                         *      -- ANSI X3J11
                         */
number:                 if ((dprec = prec) >= 0)
                                flags &= ~ZEROPAD;

                        /*
                         * ``The result of converting a zero value with an
                         * explicit precision of zero is no characters.''
                         *      -- ANSI X3J11
                         */
                        cp = buf + BUF;
                        if (_uquad != 0 || prec != 0) {
                                /*
                                 * Unsigned mod is hard, and unsigned mod
                                 * by a constant is easier than that by
                                 * a variable; hence this switch.
                                 */
                                switch (base) {
                                case OCT:
                                        do {
                                                *--cp = to_char(_uquad & 7);
                                                _uquad >>= 3;
                                        } while (_uquad);
                                        /* handle octal leading 0 */
                                        if (flags & ALT && *cp != '0')
                                                *--cp = '0';
                                        break;

                                case DEC:
                                        /* many numbers are 1 digit */
                                        while (_uquad >= 10) {
                                                *--cp = to_char(_uquad % 10);
                                                _uquad /= 10;
                                        }
                                        *--cp = to_char(_uquad);
                                        break;

                                case HEX:
                                        do {
                                                *--cp = xdigs[_uquad & 15];
                                                _uquad >>= 4;
                                        } while (_uquad);
                                        break;

                                default:
                                        cp = "bug in vfprintf: bad base";
                                        size = strlen(cp);
                                        goto skipsize;
                                }
                        }
                        size = buf + BUF - cp;
                skipsize:
                        break;
                case 'z':
                        flags |= SIZET;
                        goto rflag;
                default:        /* "%?" prints ?, unless ? is NUL */
                        if (ch == '\0')
                                goto done;
                        /* pretend it was %c with argument ch */
                        cp = buf;
                        *cp = ch;
                        size = 1;
                        sign = '\0';
                        break;
                }

                /*
                 * All reasonable formats wind up here.  At this point, `cp'
                 * points to a string which (if not flags&LADJUST) should be
                 * padded out to `width' places.  If flags&ZEROPAD, it should
                 * first be prefixed by any sign or other prefix; otherwise,
                 * it should be blank padded before the prefix is emitted.
                 * After any left-hand padding and prefixing, emit zeroes
                 * required by a decimal [diouxX] precision, then print the
                 * string proper, then emit zeroes required by any leftover
                 * floating precision; finally, if LADJUST, pad with blanks.
                 *
                 * Compute actual size, so we know how much to pad.
                 * fieldsz excludes decimal prec; realsz includes it.
                 */
#ifdef _PRINTF_FLOATING_POINT
                fieldsz = size + fpprec;
#else
                fieldsz = size;
#endif
                if (sign)
                        fieldsz++;
                else if (flags & HEXPREFIX)
                        fieldsz+= 2;
                realsz = dprec > fieldsz ? dprec : fieldsz;

                /* right-adjusting blank padding */
                if ((flags & (LADJUST|ZEROPAD)) == 0) {
                    if (width - realsz > 0) {
                        PAD(width - realsz, blanks);
                        ret += width - realsz;
                    }
                }

                /* prefix */
                if (sign) {
                        PRINT(&sign, 1);
                        ret++;
                } else if (flags & HEXPREFIX) {
                        ox[0] = '0';
                        ox[1] = ch;
                        PRINT(ox, 2);
                        ret += 2;
                }

                /* right-adjusting zero padding */
                if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD) {
                    if (width - realsz > 0) {
                        PAD(width - realsz, zeroes);
                        ret += width - realsz;
                    }
                }

                if (dprec - fieldsz > 0) {
                    /* leading zeroes from decimal precision */
                    PAD(dprec - fieldsz, zeroes);
                    ret += dprec - fieldsz;
                }

                /* the string or number proper */
                PRINT(cp, size);
                ret += size;

#ifdef _PRINTF_FLOATING_POINT
                /* trailing f.p. zeroes */
                PAD(fpprec, zeroes);
                ret += fpprec;
#endif

                /* left-adjusting padding (always blank) */
                if (flags & LADJUST) {
                    if (width - realsz > 0) {
                        PAD(width - realsz, blanks);
                        ret += width - realsz;
                    }
                }

        }
done:
error:
        return (((MG_RWops*)stream)->eof((MG_RWops*)stream) ? EOF : ret);
        /* NOTREACHED */
}


#ifdef _PRINTF_FLOATING_POINT

static char *
_round(double fract, int *exp, char *start, char *end, char ch, char *signp)
{
        double tmp;

        if (fract)
        (void)modf(fract * 10, &tmp);
        else
                tmp = to_digit(ch);
        if (tmp > 4)
                for (;; --end) {
                        if (*end == '.')
                                --end;
                        if (++*end <= '9')
                                break;
                        *end = '0';
                        if (end == start) {
                                if (exp) {      /* e/E; increment exponent */
                                        *end = '1';
                                        ++*exp;
                                }
                                else {          /* f; add extra digit */
                                *--end = '1';
                                --start;
                                }
                                break;
                        }
                }
        /* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
        else if (*signp == '-')
                for (;; --end) {
                        if (*end == '.')
                                --end;
                        if (*end != '0')
                                break;
                        if (end == start)
                                *signp = 0;
                }
        return (start);
} // _round()


static char *
exponent(char *p, int exp, int fmtch)
{
        char *t;
        char expbuf[MAXEXP];

        *p++ = fmtch;
        if (exp < 0) {
                exp = -exp;
                *p++ = '-';
        }
        else
                *p++ = '+';
        t = expbuf + MAXEXP;
        if (exp > 9) {
                do {
                        *--t = to_char(exp % 10);
                } while ((exp /= 10) > 9);
                *--t = to_char(exp);
                for (; t < expbuf + MAXEXP; *p++ = *t++);
        }
        else {
                *p++ = '0';
                *p++ = to_char(exp);
        }
        return (p);
} // exponent()

static int cvt(double number, int prec, int flags, char *signp, int fmtch, char *startp,
    char *endp)
{
    _libm_ieee_double_shape_type ieeefp;
    char *t = startp;

    ieeefp.value = number;
    *signp = 0;
    if ( ieeefp.number.sign ){  // this checks for <0.0 and -0.0
        number = -number;
        *signp = '-';
    }

    if (finite(number)) {
        char *p;
        double fract;
        int dotrim, expcnt, gformat;
        double integer, tmp;

        dotrim = expcnt = gformat = 0;
        fract = modf(number, &integer);

        /* get an extra slot for rounding. */
        t = ++startp;

        /*
         * get integer portion of number; put into the end of the buffer; the
         * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
         */
        for (p = endp - 1; integer; ++expcnt) {
                tmp = modf(integer / 10, &integer);
                *p-- = to_char((int)((tmp + .01) * 10));
        }
        switch (fmtch) {
        case 'f':
                /* reverse integer into beginning of buffer */
                if (expcnt)
                        for (; ++p < endp; *t++ = *p);
                else
                        *t++ = '0';
                /*
                 * if precision required or alternate flag set, add in a
                 * decimal point.
                 */
                if (prec || flags&ALT)
                        *t++ = '.';
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec)
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while (--prec && fract);
                        if (fract)
                                startp = _round(fract, (int *)NULL, startp,
                                    t - 1, (char)0, signp);
                }
                for (; prec--; *t++ = '0');
                break;
        case 'e':
        case 'E':
eformat:        if (expcnt) {
                        *t++ = *++p;
                        if (prec || flags&ALT)
                                *t++ = '.';
                        /* if requires more precision and some integer left */
                        for (; prec && ++p < endp; --prec)
                                *t++ = *p;
                        /*
                         * if done precision and more of the integer component,
                         * round using it; adjust fract so we don't re-round
                         * later.
                         */
                        if (!prec && ++p < endp) {
                                fract = 0;
                                startp = _round((double)0, &expcnt, startp,
                                    t - 1, *p, signp);
                        }
                        /* adjust expcnt for digit in front of decimal */
                        --expcnt;
                }
                /* until first fractional digit, decrement exponent */
                else if (fract) {
                        /* adjust expcnt for digit in front of decimal */
                        for (expcnt = -1;; --expcnt) {
                                fract = modf(fract * 10, &tmp);
                                if (tmp)
                                        break;
                        }
                        *t++ = to_char((int)tmp);
                        if (prec || flags&ALT)
                                *t++ = '.';
                }
                else {
                        *t++ = '0';
                        if (prec || flags&ALT)
                                *t++ = '.';
                }
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec)
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while (--prec && fract);
                        if (fract)
                                startp = _round(fract, &expcnt, startp,
                                    t - 1, (char)0, signp);
                }
                /* if requires more precision */
                for (; prec--; *t++ = '0');

                /* unless alternate flag, trim any g/G format trailing 0's */
                if (gformat && !(flags&ALT)) {
                        while (t > startp && *--t == '0');
                        if (*t == '.')
                                --t;
                        ++t;
                }
                t = exponent(t, expcnt, fmtch);
                break;
        case 'g':
        case 'G':
                /* a precision of 0 is treated as a precision of 1. */
                if (!prec)
                        ++prec;
                /*
                 * ``The style used depends on the value converted; style e
                 * will be used only if the exponent resulting from the
                 * conversion is less than -4 or greater than the precision.''
                 *      -- ANSI X3J11
                 */
                if (expcnt > prec || (!expcnt && fract && fract < .0001)) {
                        /*
                         * g/G format counts "significant digits, not digits of
                         * precision; for the e/E format, this just causes an
                         * off-by-one problem, i.e. g/G considers the digit
                         * before the decimal point significant and e/E doesn't
                         * count it as precision.
                         */
                        --prec;
                        fmtch -= 2;             /* G->E, g->e */
                        gformat = 1;
                        goto eformat;
                }
                /*
                 * reverse integer into beginning of buffer,
                 * note, decrement precision
                 */
                if (expcnt)
                        for (; ++p < endp; *t++ = *p, --prec);
                else
                        *t++ = '0';
                /*
                 * if precision required or alternate flag set, add in a
                 * decimal point.  If no digits yet, add in leading 0.
                 */
                if (prec || flags&ALT) {
                        dotrim = 1;
                        *t++ = '.';
                }
                else
                        dotrim = 0;
                /* if requires more precision and some fraction left */
                if (fract) {
                        if (prec) {
                                do {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                } while(!tmp);
                                while (--prec && fract) {
                                        fract = modf(fract * 10, &tmp);
                                        *t++ = to_char((int)tmp);
                                }
                        }
                        if (fract)
                                startp = _round(fract, (int *)NULL, startp,
                                    t - 1, (char)0, signp);
                }
                /* alternate format, adds 0's for precision, else trim 0's */
                if (flags&ALT)
                        for (; prec--; *t++ = '0');
                else if (dotrim) {
                        while (t > startp && *--t == '0');
                        if (*t != '.')
                                ++t;
                }
        }
    } else {
        unsigned case_adj;
        switch (fmtch) {
        case 'f':
        case 'g':
        case 'e':
            case_adj = 'a' - 'A';
            break;
        default:
            case_adj = 0;
        }
        if (isnan(number)) {
            *t++ = 'N' + case_adj;
            *t++ = 'A' + case_adj;
            *t++ = 'N' + case_adj;
        } else { // infinite
            *t++ = 'I' + case_adj;
            *t++ = 'N' + case_adj;
            *t++ = 'F' + case_adj;
        }
    }
    return (t - startp);
}

#endif /* _PRINTF_FLOATING_POINT */

#endif /* _MGUSE_OWN_STDIO */

