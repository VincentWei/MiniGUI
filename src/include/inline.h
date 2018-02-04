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
** inline.h: Some inline functions.
**
** Create date: 1999/4/21
*/

#ifndef GUI_INLINE_H
    #define GUI_INLINE_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if defined(__GNUC__) && defined(i386)

#define ASM_memset2(s, c, count)  \
do {                            \
    int eax, edi, ecx;          \
    __asm__ __volatile__ (      \
            "cld\n\t"           \
            "rep\n\t"           \
            "stosw"             \
            : "=&D" (edi), "=&a" (eax), "=&c" (ecx) \
            : "0"(s), "1"(c), "2"(count)            \
            : "memory");        \
} while (0)

#define ASM_memset4(s, c, count)    \
do {                                \
    int eax, edi, ecx;              \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "rep\n\t"               \
            "stosl"                 \
            : "=&D" (edi), "=&a" (eax), "=&c" (ecx) \
            : "0"(s), "1"(c), "2"(count)    \
            : "memory");            \
} while (0)

#define ASM_memxorset(s, c, n)      \
do {                                \
    int ecx, edx, esi, edi;         \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsb\n\t"             \
            "xorb %%dl, %%al\n\t"   \
            "stosb\n\t"             \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx), "=&d" (edx)    \
            : "0" (s), "1" (s), "2" (n), "3" (c)    \
            : "al");            \
} while (0)

#define ASM_memxorset2(s, c, n)     \
do {                                \
    int ecx, edx, esi, edi;         \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsw\n\t"             \
            "xorw %%dx, %%ax\n\t"   \
            "stosw\n\t"             \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx), "=&d" (edx)    \
            : "0" (s), "1" (s), "2" (n), "3" (c)    \
            : "ax");            \
} while (0)

#define ASM_memxorset4(s, c, n)     \
do {                                \
    int ecx, edx, esi, edi;         \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "xorl %%edx, %%eax\n\t" \
            "stosl\n\t"             \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx), "=&d" (edx)    \
            : "0" (s), "1" (s), "2" (n), "3" (c)    \
            : "eax");            \
} while (0)

#define ASM_memandset4(s, c, n)     \
do {                                \
    int ecx, edx, esi, edi;         \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "andl %%edx, %%eax\n\t" \
            "stosl\n\t"             \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx), "=&d" (edx)    \
            : "0" (s), "1" (s), "2" (n), "3" (c)    \
            : "eax");            \
} while (0)

#define ASM_memorset4(s, c, n)      \
do {                                \
    int ecx, edx, esi, edi;         \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "orl %%edx, %%eax\n\t"  \
            "stosl\n\t"             \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx), "=&d" (edx)    \
            : "0" (s), "1" (s), "2" (n), "3" (c)    \
            : "eax");            \
} while (0)

#define ASM_memandcpy4(dst, src, n) \
do {                                \
    int edi, esi, ecx;              \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "andl %%eax, (%%edi)\n\t"   \
            "addl $4, %%edi\n\t"    \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx) \
            : "0" (dst), "1" (src), "2" (n) \
            : "eax");            \
} while (0)

#define ASM_memorcpy4(dst, src, n)  \
do {                                \
    int edi, esi, ecx;              \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "orl %%eax, (%%edi)\n\t"    \
            "addl $4, %%edi\n\t"    \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx) \
            : "0" (dst), "1" (src), "2" (n) \
            : "eax");            \
} while (0)

#define ASM_memxorcpy4(dst, src, n) \
do {                                \
    int edi, esi, ecx;              \
    __asm__ __volatile__ (          \
            "cld\n\t"               \
            "1:\n\t"                \
            "lodsl\n\t"             \
            "xorl %%eax, (%%edi)\n\t"   \
            "addl $4, %%edi\n\t"    \
            "loop 1b\n\t"           \
            : "=&D" (edi), "=&S" (esi), "=&c" (ecx) \
            : "0" (dst), "1" (src), "2" (n) \
            : "eax");            \
} while (0)

#if 0
static inline void *
 __memset4(void *s, unsigned c, size_t count)
{
    __asm__ __volatile__ (
            "cld\n\t"
            "rep\n\t"
            "stosl"
  : :       "a"(c), "D"(s), "c"(count)
  :         "cx", "di");
    return s;
}

static inline void *
 __memset3(void *s, int c, size_t count)
/* count is in 24-bit pixels (3 bytes per pixel) */
{
    __asm__ __volatile__ (
            "cmpl $8,%%edx\n\t"
    /* "jmp 2f\n\t" *//* debug */
            "jl 2f\n\t"

            "movl %%eax,%%ebx\n\t"   /* eax = (low) BGR0 (high) */
            "shll $24,%%ebx\n\t"     /* ebx = 000B */
            "orl %%ebx,%%eax\n\t"    /* eax = BGRB */

            "movl %%eax,%%ebx\n\t"
            "shrl $8,%%ebx\n\t"      /* ebx = GRB0 */
            "movl %%ebx,%%ecx\n\t"
            "shll $24,%%ecx\n\t"     /* ecx = 000G */
            "orl %%ecx,%%ebx\n\t"    /* ebx = GRBG */

            "movl %%eax,%%ecx\n\t"
            "shll $8,%%ecx\n\t"      /* ecx = 0BGR */
            "movb %%bh,%%cl\n\t"     /* ecx = RBGR */
            "cmpl $16,%%edx\n\t"
            "jl 1f\n\t"
            "jmp 5f\n\t"
            ".align 4,0x90\n\t"

            "5:\n\t"         /* loop unrolling */
            "movl %%eax,(%%edi)\n\t"         /* write BGRB */
            "movl %%ebx,4(%%edi)\n\t"        /* write GRBG */
            "movl %%ecx,8(%%edi)\n\t"        /* write RBGR */
            "movl %%eax,12(%%edi)\n\t"
            "movl %%ebx,16(%%edi)\n\t"
            "movl %%ecx,20(%%edi)\n\t"
            "movl %%eax,24(%%edi)\n\t"
            "movl %%ebx,28(%%edi)\n\t"
            "movl %%ecx,32(%%edi)\n\t"
            "movl %%eax,36(%%edi)\n\t"
            "subl $16,%%edx\n\t"     /* blend end-of-loop instr. */
            "movl %%ebx,40(%%edi)\n\t"
            "movl %%ecx,44(%%edi)\n\t"
            "addl $48,%%edi\n\t"
            "cmpl $16,%%edx\n\t"
            "jge 5b\n\t"
            "andl %%edx,%%edx\n\t"
            "jz 4f\n\t"      /* finished */
            "cmpl $4,%%edx\n\t"
            "jl 2f\n\t"      /* less than 4 pixels left */
            "jmp 1f\n\t"
            ".align 4,0x90\n\t"

            "1:\n\t"
            "movl %%eax,(%%edi)\n\t"         /* write BGRB */
            "movl %%ebx,4(%%edi)\n\t"        /* write GRBG */
            "movl %%ecx,8(%%edi)\n\t"        /* write RBGR */
            "addl $12,%%edi\n\t"
            "subl $4,%%edx\n\t"
            "cmpl $4,%%edx\n\t"
            "jge 1b\n\t"

            "2:\n\t"
            "cmpl $0,%%edx\n\t"      /* none left? */
            "jle 4f\n\t"     /* finished */

            "mov %%eax,%%ecx\n\t"
            "shrl $16,%%ecx\n\t"     /* B in cl */

            "3:\n\t"         /* write last few pixels */
            "movw %%ax,(%%edi)\n\t"  /* write RG */
            "movb %%cl,2(%%edi)\n\t"         /* write B */
            "addl $3,%%edi\n\t"
            "decl %%edx\n\t"
            "jnz 3b\n\t"

            "4:\n\t"
  : :       "a"(c), "D"(s), "d"(count)
  :         "ax", "bx", "cx", "dx", "di");
    return s;
}

static inline void *
 __memset(void *s, char c, size_t count)
{
   int d0;
    __asm__ (
       "cld\n\t"
       "cmpl $12,%%edx\n\t"
       "jl 1f\n\t" /* if (count >= 12) */

       "movzbl %%al,%%ax\n\t"
       "movl %%eax,%%ecx\n\t"
       "shll $8,%%ecx\n\t" /* c |= c << 8 */
       "orl %%ecx,%%eax\n\t"
       "movl %%eax,%%ecx\n\t"
       "shll $16,%%ecx\n\t" /* c |= c << 16 */
       "orl %%ecx,%%eax\n\t"

       "movl %%edx,%%ecx\n\t"

       "negl %%ecx\n\t"
       "andl $3,%%ecx\n\t" /* (-s % 4) */
       "subl %%ecx,%%edx\n\t" /* count -= (-s % 4) */
       "rep ; stosb\n\t" /* align to longword boundary */

       "movl %%edx,%%ecx\n\t"
       "shrl $2,%%ecx\n\t"
       "rep ; stosl\n\t" /* fill longwords */

       "andl $3,%%edx\n" /* fill last few bytes */
       "1:\tmovl %%edx,%%ecx\n\t" /* <= 12 entry point */
       "rep ; stosb\n\t"
  :    "=&a"(d0)
  :    "a"(c), "D"(s), "d"(count)
  :    "eax", "ecx", "edx", "edi", "esi");
    return s;
}

static inline void *
 __memset2(void *s, short c, size_t count)
/* count is in 16-bit pixels */
/* s is assumed to be 16-bit aligned */
{
    int eax, ecx, edi, edx;
    __asm__ __volatile__ (
            "cld\n\t"
            "cmpl $12,%%edx\n\t"
            "jl 1f\n\t"      /* if (count >= 12) */

            "movzwl %%ax,%%eax\n\t"
            "movl %%eax,%%ecx\n\t"
            "shll $16,%%ecx\n\t"     /* c |= c << 16 */
            "orl %%ecx,%%eax\n\t"

            "movl %%edi,%%ecx\n\t"
            "andl $2,%%ecx\n\t"      /* s & 2 */
            "jz 2f\n\t"
            "decl %%edx\n\t" /* count -= 1 */
            "stosw\n\t"      /* align to longword boundary */

            "2:\n\t"
            "movl %%edx,%%ecx\n\t"
            "shrl $1,%%ecx\n\t"
            "rep ; stosl\n\t"        /* fill longwords */

            "andl $1,%%edx\n"        /* one 16-bit word left? */
            "jz 3f\n\t"      /* no, finished */
            "1:\tmovl %%edx,%%ecx\n\t"       /* <= 12 entry point */
            "rep ; stosw\n\t"
            "3:\n\t"
            : "=&D" (edi), "=&a" (eax), "=&c" (ecx), "=&d" (edx)
            : "0" (s), "1" (c), "2" (count)
            : "memory");
    return s;
}

static inline void *
 __memcpy (void *to, const void *from, size_t n)
{
   int d0;
   __asm__ (
            "cld\n\t"
            "movl %%edi,%%ecx\n\t"
            "andl $1,%%ecx\n\t"
            "subl %%ecx,%%edx\n\t"
            "rep ; movsb\n\t"   /* 16-bit align destination */
            "movl %%edx,%%ecx\n\t"
            "shrl $2,%%ecx\n\t"
            "rep ; movsl\n\t"
            "testb $1,%%dl\n\t"
            "je 1f\n\t"
            "movsb\n"
            "1:\ttestb $2,%%dl\n\t"
            "je 2f\n\t"
            "movsw\n"
            "2:\n"
  :         "=&S"(d0)
  :         "d"(n), "D"((long) to), "S"((long) from)
  :         "eax", "ecx", "edi", "esi");
    return (to);
}

static inline void *__memcpyf ( void * dest, void * src, long size )
{
 __asm__ __volatile__ (
        "movl    %%ecx, %%eax\n\t"
        "andl    $0xFFFFFFFC, %%ecx\n\t"
        "shr     $2, %%ecx\n\t"
        "rep;    movsl\n\t"
        "movl    %%eax, %%ecx\n\t"
        "andl    $3, %%ecx\n\t"
        "rep;    movsb\n"
        : : "S" ((unsigned long)src), "D" ((unsigned long)dest), "c" (size)
        : "esi", "edi", "ecx", "eax" );
 return dest;
}

static inline void *__memsetf ( void * dest, char c, long size )
{
 __asm__ __volatile__ (
        "movl    %%ecx, %%edx\n\t"
        "andl    $0xFFFFFFFC, %%ecx\n\t"
        "shr     $2, %%ecx\n\t"
        "rep;    stosl\n\t"
        "movl    %%edx, %%ecx\n\t"
        "andl    $3, %%ecx\n\t"
        "rep;    stosb\n"
        : : "D" ((unsigned long)dest), "b" ((unsigned long)c), "c" (size)
        : "edi", "ecx", "eax", "edx" );
 return dest;
}

#endif

#endif /* defined(__GNUC__) && defined(i386) */

#ifndef ASM_memcpy
#define ASM_memcpy  memcpy
#endif

#ifndef ASM_memset
#define ASM_memset  memset
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_INLINE_H */

