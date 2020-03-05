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
/**
 * \file common.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 *
 * \brief This file includes macro definitions and typedefs that commonly used
 *        by MiniGUI.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: common.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *      The definitions of some data types and byte order macros
 *      borrowed from LGPL'ed SDL by Sam Lantinga.
 *
 *      Fix point math routines come from Allegro (a gift software)
 *      by Shawn Hargreaves and others.
 */

#ifndef _MGUI_COMMON_H

#define _MGUI_COMMON_H

#ifndef MINIGUI_MAJOR_VERSION
#   undef PACKAGE
#   undef VERSION
#   undef PACKAGE_BUGREPORT
#   undef PACKAGE_NAME
#   undef PACKAGE_STRING
#   undef PACKAGE_TARNAME
#   undef PACKAGE_VERSION
#   ifdef __MINIGUI_LIB__
#       ifdef _FOR_DATANG
#           ifdef WIN32
#               include <config-win32/mgconfig.h>
#           elif defined (__THREADX__)
#               include "config-threadx/mgconfig.h"
#           elif defined (__NUCLEUS__)
#               include "config-nucleus/mgconfig.h"
#           endif
#       else
#           if  defined(__CMAKE_PROJECT__) || defined(WIN32)
#               include "mgconfig.h"
#           else
#               include "../mgconfig.h"
#           endif
#       endif
#   else
#       include "mgconfig.h"
#   endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>

    /**
     * \defgroup macros_types Macros and data types commonly used
     * @{
     */

    /**
     * \defgroup version_info Version information
     * @{
     */

/**
 * \def _VERSION_CODE(major, minor, micro)
 * \brief A macro that returns the version code from \a major, \a minor
 * and \a micro version number.
 *
 * MiniGUI uses this macro to evaluate the version code of current MiniGUI
 * library installed in your system, and define it to _MINIGUI_VERSION_CODE.
 *
 * \sa _MINIGUI_VERSION_CODE
 */
#define _VERSION_CODE(major, minor, micro)  \
    (((major)<<16) | ((minor)<<8) | (micro))

/**
 * \def _MINIGUI_VERSION_CODE
 * \brief Version code of MiniGUI.
 *
 * \sa _VERSION_CODE
 */
#define _MINIGUI_VERSION_CODE           \
    ((MINIGUI_MAJOR_VERSION << 16) |    \
     (MINIGUI_MINOR_VERSION << 8) |     \
     MINIGUI_MICRO_VERSION)

    /** @} end of version_info */

    /**
     * \defgroup basic_types Basic data types
     * @{
     */

/**
 * \var typedef unsigned char Uint8
 * \brief A type definition for an 8-bit unsigned character.
 */
typedef unsigned char   Uint8;
/**
 * \var typedef signed char Sint8
 * \brief A type definition for an 8-bit signed character.
 */
typedef signed char     Sint8;
/**
 * \var typedef unsigned short Uint16
 * \brief A type definition for a 16-bit unsigned integer.
 */
typedef unsigned short  Uint16;
/**
 * \var typedef signed short Sint16
 * \brief A type definition for a 16-bit signed integer.
 */
typedef signed short    Sint16;
/**
 * \var typedef unsigned int Uint32
 * \brief A type definition for a 32-bit unsigned integer.
 */
typedef unsigned int    Uint32;
/**
 * \var typedef signed int Sint32
 * \brief A type definition for a 32-bit signed integer.
 */
typedef signed int      Sint32;

/* Figure out how to support 64-bit datatypes */
#if !defined(__STRICT_ANSI__)
#   if defined(__GNUC__)
#       define MGUI_HAS_64BIT_TYPE    long long
#   endif
#   if defined(__CC_ARM)
#       define MGUI_HAS_64BIT_TYPE    long long
#   endif
#   if defined(_MSC_VER)
#       define MGUI_HAS_64BIT_TYPE __int64
#   endif
#endif /* !__STRICT_ANSI__ */

/* The 64-bit datatype isn't supported on all platforms */
#ifdef MGUI_HAS_64BIT_TYPE

/**
 * \var typedef unsigned long long Uint64
 * \brief A type definition for a 64-bit unsigned integer.
 *
 * \warning Only available under GNU C.
 */
typedef unsigned MGUI_HAS_64BIT_TYPE Uint64;
/**
 * \var typedef signed long long Sint64
 * \brief A type definition for a 64-bit signed integer.
 *
 * \warning Only available under GNU C.
 */
typedef signed MGUI_HAS_64BIT_TYPE Sint64;
#else
/* This is really just a hack to prevent the compiler from complaining */
typedef struct {
    Uint32 hi;
    Uint32 lo;
} Uint64, Sint64;
#endif

/* Make sure the types really have the right sizes */
#define MGUI_COMPILE_TIME_ASSERT(name, x)               \
       typedef int MGUI_dummy_ ## name[(x) * 2 - 1]

MGUI_COMPILE_TIME_ASSERT(uint8, sizeof(Uint8) == 1);
MGUI_COMPILE_TIME_ASSERT(sint8, sizeof(Sint8) == 1);
MGUI_COMPILE_TIME_ASSERT(uint16, sizeof(Uint16) == 2);
MGUI_COMPILE_TIME_ASSERT(sint16, sizeof(Sint16) == 2);
MGUI_COMPILE_TIME_ASSERT(uint32, sizeof(Uint32) == 4);
MGUI_COMPILE_TIME_ASSERT(sint32, sizeof(Sint32) == 4);
MGUI_COMPILE_TIME_ASSERT(uint64, sizeof(Uint64) == 8);
MGUI_COMPILE_TIME_ASSERT(sint64, sizeof(Sint64) == 8);

#undef MGUI_COMPILE_TIME_ASSERT

    /** @} end of basic_types */

/* Here we provide MG_GNUC_EXTENSION as an alias for __extension__,
 * where this is valid. This allows for warningless compilation of
 * "long long" types even in the presence of '-ansi -pedantic'.
 */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#define MG_GNUC_EXTENSION __extension__
#else
#define MG_GNUC_EXTENSION
#endif

/*
 * The MG_LIKELY and MG_UNLIKELY macros let the programmer give hints to
 * the compiler about the expected result of an expression. Some compilers
 * can use this information for optimizations.
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define _MG_BOOLEAN_EXPR(expr)                  \
 MG_GNUC_EXTENSION ({                           \
   int _g_boolean_var_;                         \
   if (expr)                                    \
      _g_boolean_var_ = 1;                      \
   else                                         \
      _g_boolean_var_ = 0;                      \
   _g_boolean_var_;                             \
})
#define MG_LIKELY(expr) (__builtin_expect (_MG_BOOLEAN_EXPR(expr), 1))
#define MG_UNLIKELY(expr) (__builtin_expect (_MG_BOOLEAN_EXPR(expr), 0))
#else
#define MG_LIKELY(expr) (expr)
#define MG_UNLIKELY(expr) (expr)
#endif

    /**
     * \defgroup endian_info Endianness information
     * @{
     */

/**
 * \def MGUI_LIL_ENDIAN
 * \brief Little endianness.
 */
#define MGUI_LIL_ENDIAN  1234
/**
 * \def MGUI_BIG_ENDIAN
 * \brief Big endianness.
 */
#define MGUI_BIG_ENDIAN  4321

/* Pardon the mess, I'm trying to determine the endianness of this host.
 *    I'm doing it by preprocessor defines rather than some sort of configure
 *    script so that application code can use this too.  The "right" way would
 *    be to dynamically generate this file on install, but that's a lot of work.
 */

/**
 * \def MGUI_BYTEORDER
 * \brief The byte order (endianness) of the target system.
 *
 * This macro will be either defined to MGUI_LIL_ENDIAN or MGUI_BIG_ENDIAN.
 * You can use the code like below
 *
 * \code
 * #if MGUI_BYTEORDER == MGUI_LIL_ENDIAN
 *     ... // code for little endian system.
 * #else
 *     ... // code for big endian system.
 * #endif
 * \endcode
 *
 * to write endianness independent code.
 */
#if defined(__i386__) || defined(__ia64__) || defined(__x86_64__) || \
        defined(__amd64) || \
        (defined(__alpha__) || defined(__alpha)) || \
        defined(__arm__) || defined(__aarch64__) || \
        defined(__riscv) || \
        (defined(__CC_ARM) && !defined(__BIG_ENDIAN)) || \
        (defined(__mips__) && defined(__MIPSEL__)) || \
        defined(__LITTLE_ENDIAN__) || \
        defined(WIN32)
#   define MGUI_BYTEORDER   MGUI_LIL_ENDIAN
#else
#   define MGUI_BYTEORDER   MGUI_BIG_ENDIAN
#endif

    /** @} end of endian_info */

    /**
     * \defgroup simple_types Simple and common types and macros
     * @{
     */


/**
 * \var PVOID
 * \brief A type definition for a pointer to any type.
 */
typedef void *PVOID;

/**
 * \var typedef int BOOL
 * \brief A type definition for boolean value.
 */
#ifndef BOOL
typedef int BOOL;
#endif

/**
 * \def FALSE
 * \brief FALSE value, defined as 0 by MiniGUI.
 */
#ifndef FALSE
    #define FALSE       0
#endif
/**
 * \def TRUE
 * \brief TRUE value, defined as 1 by MiniGUI.
 */
#ifndef TRUE
    #define TRUE        1
#endif

/**
 * \def NULL
 * \brief A value indicates null pointer.
 */
#ifndef NULL
#   ifdef __cplusplus
#       define NULL            (0)
#   else
#       define NULL            ((void *)0)
#   endif
#endif

/**
 * \def INV_PTR
 * \brief A value indicates an invalid non-null pointer.
 */
#define INV_PTR                ((void *)-1)

#define GUIAPI

#if !defined(__NODLL__) && (defined (WIN32) || defined (__NUCLEUS_MNT__))
#   if defined(__MINIGUI_LIB__)
#       define MG_EXPORT       __declspec(dllexport)
#   else
#       define MG_EXPORT       __declspec(dllimport)
#   endif
#else
#   define MG_EXPORT
#endif

    /** @} end of simple_types */

    /**
     * \defgroup handles MiniGUI handles
     * @{
     */

/**
 * \var GHANDLE
 * \brief General handle.
 */
typedef PVOID GHANDLE;
/**
 * \var HWND
 * \brief Handle to main window or control.
 */
typedef GHANDLE HWND;
/**
 * \var HDC
 * \brief Handle to device context.
 */
typedef GHANDLE HDC;
/**
 * \var HPALETTE
 * \brief Handle to a logical palette.
 */
typedef GHANDLE HPALETTE;
/**
 * \var HCURSOR
 * \brief Handle to cursor.
 */
typedef GHANDLE HCURSOR;
/**
 * \var HICON
 * \brief Handle to icon.
 */
typedef GHANDLE HICON;
/**
 * \var HMENU
 * \brief Handle to menu.
 */
typedef GHANDLE HMENU;
/**
 * \var HACCEL
 * \brief Handle to accelarator.
 */
typedef GHANDLE HACCEL;
/**
 * \var HDLG
 * \brief Handle to dialog box, same as HWND.
 */
typedef GHANDLE HDLG;
/**
 * \var HHOOK
 * \brief Handle to keyboard or mouse event hook.
 */
typedef GHANDLE HHOOK;

    /** @} end of handles */

    /**
     * \defgroup win32_types Win32-like data types and macros
     * @{
     */

/**
 * \var BYTE
 * \brief A type definition for an 8-bit unsigned character (byte).
 */
typedef unsigned char   BYTE;

/**
 * \var SBYTE
 * \brief A type definition for an 8-bit signed character.
 */
typedef signed char     SBYTE;

/**
 * \var SIZEOF_PTR
 * \brief The size of a pointer. 4 for 32-bit and 8 for 64-bit.
 */
/**
 * \var SIZEOF_HPTR
 * \brief The size of a half or pointer. 2 for 32-bit and 4 for 64-bit.
 */
#if defined(_WIN64)
#   define SIZEOF_PTR   8
#   define SIZEOF_HPTR  4
#elif defined(__LP64__)
#   define SIZEOF_PTR   8
#   define SIZEOF_HPTR  4
#else
#   define SIZEOF_PTR   4
#   define SIZEOF_HPTR  2
#endif

#if SIZEOF_PTR == 8
#   define NR_BITS_BYTE      (8)
#   define NR_BITS_WORD      (32)
#   define NR_BITS_DWORD     (64)

#   define BITMASK_BYTE      (0xFF)
#   define BITMASK_WORD      (0xFFFFFFFF)
#   define BITMASK_DWORD     (0xFFFFFFFFFFFFFFFF)

#   define INT_PTR_MIN       (-9223372036854775807L-1)
#   define INT_PTR_MAX       (9223372036854775807L)
#   define UINT_PTR_MAX      (18446744073709551615UL)
#else
#   define NR_BITS_BYTE      (8)
#   define NR_BITS_WORD      (16)
#   define NR_BITS_DWORD     (32)

#   define BITMASK_BYTE      (0xFF)
#   define BITMASK_WORD      (0xFFFF)
#   define BITMASK_DWORD     (0xFFFFFFFF)

#   define INT_PTR_MIN       (-2147483647-1)
#   define INT_PTR_MAX       (2147483647)
#   define UINT_PTR_MAX      (4294967295U)
#endif

/**
 * \var WORD_HPTR
 * \brief An unsigned int (word) type in half pointer precision.
 */
#if defined(_WIN64)
typedef unsigned int WORD_HPTR;
#elif defined(__LP64__)
typedef unsigned int WORD_HPTR;
#else
typedef unsigned short WORD_HPTR;
#endif

/**
 * \var SWORD_HPTR
 * \brief An signed int type in half pointer precision.
 */
#if defined(_WIN64)
typedef signed int SWORD_HPTR;
#elif defined(__LP64__)
typedef signed int SWORD_HPTR;
#else
typedef signed short SWORD_HPTR;
#endif

/**
 * \var WORD
 * \brief A type definition for an unsigned integer (word).
 */
typedef WORD_HPTR WORD;

/**
 * \var SWORD
 * \brief A type definition for a signed integer.
 */
typedef SWORD_HPTR SWORD;

/**
 * \var WORD16
 * \brief A type definition for a 16-bit unsigned integer (word).
 */
typedef unsigned short WORD16;

/**
 * \var SWORD16
 * \brief A type definition for a 16-bit signed integer.
 */
typedef signed short SWORD16;

/**
 * \var LONG_PTR
 * \brief A signed long type for pointer precision.
 */
#if defined(_WIN64)
typedef __int64 LONG_PTR;
#elif defined(__LP64__)
typedef long LONG_PTR;
#else
typedef long LONG_PTR;
#endif

/**
 * \var LINT
 * \brief Signed integer which has pointer precision.
 */
typedef LONG_PTR LINT;

/**
 * \var LRESULT
 * \brief Signed result of message processing.
 */
typedef LONG_PTR LRESULT;

/**
 * \var DWORD_PTR
 * \brief An unsigned long type for pointer precision.
 *
 * Commonly used for general 32-bit parameters that have been extended
 * to 64 bits in 64-bit platform.
 */
#if defined(_WIN64)
typedef unsigned __int64 DWORD_PTR;
#elif defined(__LP64__)
typedef unsigned long DWORD_PTR;
#else
typedef unsigned long DWORD_PTR;
#endif

/**
 * \var DWORD
 * \brief A unsigned long type definition for pointer precision.
 */
typedef DWORD_PTR DWORD;

/**
 * \var DWORD32
 * \brief A type definition for a 32-bit unsigned integer.
 */
typedef unsigned int DWORD32;

/**
 * \var SDWORD_PTR
 * \brief A signed long type for pointer precision.
 *
 * Commonly used for general 32-bit parameters that have been extended
 * to 64 bits in 64-bit platform.
 */
#if defined(_WIN64)
typedef signed __int64 SDWORD_PTR;
#elif defined(__LP64__)
typedef signed long SDWORD_PTR;
#else
typedef signed long SDWORD_PTR;
#endif

/**
 * \var SDWORD
 * \brief A signed long type definition for pointer precision.
 */
typedef SDWORD_PTR SDWORD;

/**
 * \var SDWORD32
 * \brief A type definition for a 32-bit signed integer.
 */
typedef signed int SDWORD32;

#if SIZEOF_PTR == 8
typedef unsigned short  QDWORD;
#define QDWORD_SHIFT    16
#else
typedef unsigned char   QDWORD;
#define QDWORD_SHIFT    8
#endif

#define MAKEDWORD(q1, q2, q3, q4)                       \
    ((DWORD)(                                           \
        (((DWORD)(QDWORD)(q1))) |                       \
        (((DWORD)((QDWORD)(q2))) << QDWORD_SHIFT) |     \
        (((DWORD)((QDWORD)(q3))) << (QDWORD_SHIFT*2)) | \
        (((DWORD)((QDWORD)(q4))) << (QDWORD_SHIFT*3))   \
    ))

#define FIRST_QDWORD(dw)    ((QDWORD)(((DWORD)(dw))))
#define SECOND_QDWORD(dw)   ((QDWORD)((((DWORD)(dw)) >> QDWORD_SHIFT)))
#define THIRD_QDWORD(dw)    ((QDWORD)((((DWORD)(dw)) >> (QDWORD_SHIFT*2))))
#define FOURTH_QDWORD(dw)   ((QDWORD)((((DWORD)(dw)) >> (QDWORD_SHIFT*3))))

/**
 * \var UINT
 * \brief A type definition for unsigned integer.
 */
typedef unsigned int UINT;

/**
 * \var INT_PTR
 * \brief A signed integer type for pointer precision.
 */
#if defined(_WIN64)
typedef __int64 INT_PTR;
#elif defined(__LP64__)
typedef long INT_PTR;
#else
typedef int INT_PTR;
#endif

/**
 * \var UINT_PTR
 * \brief A unsigned integer type for pointer precision.
 */
#if defined(_WIN64)
typedef unsigned __int64 UINT_PTR;
#elif defined(__LP64__)
typedef unsigned long UINT_PTR;
#else
typedef unsigned long UINT_PTR;
#endif

/**
 * \var typedef LONG
 * \brief A type definition for long integer.
 */
typedef long LONG;

/**
 * \var ULONG
 * \brief A type definition for unsigned long integer.
 */
typedef unsigned long ULONG;

/**
 * \var WPARAM
 * \brief A type definition for the first message paramter.
 */
typedef UINT_PTR WPARAM;

/**
 * \var LPARAM
 * \brief A type definition for the second message paramter.
 */
typedef UINT_PTR LPARAM;

/**
 * \def LOBYTE(w)
 * \brief Returns the low byte of the word \a w.
 *
 * \sa MAKEWORD
 */
#define LOBYTE(w)           ((BYTE)(w))

/**
 * \def HIBYTE(w)
 * \brief Returns the high byte of the word \a w.
 *
 * \sa MAKEWORD
 */
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> NR_BITS_BYTE) & BITMASK_BYTE))

/**
 * \def MAKEWORD(low, high)
 * \brief Makes a word from \a low byte and \a high byte.
 *
 * \sa LOBYTE, HIBYTE
 */
#define MAKEWORD(low, high)     \
    ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << NR_BITS_BYTE)))

/**
 * \def MAKEWORD16(low, high)
 * \brief Makes a 16-bit word from \a low byte and \a high byte.
 *
 * \sa LOBYTE, HIBYTE
 */
#define MAKEWORD16(low, high)   \
    ((WORD16)(((BYTE)(low)) | (((WORD16)((BYTE)(high))) << 8)))

/**
 * \def MAKEDWORD32(first, second, third, fourth)
 * \brief Makes a WORD32 value with four bytes.
 *
 * \sa MAKEWORD16, FIRSTBYTE, SECONDBYTE, THIRDBYTE, FOURTHBYTE
 */
#define MAKEDWORD32(first, second, third, fourth)   \
    ((DWORD32)(                                     \
        ((BYTE)(first)) |                           \
        (((DWORD32)((BYTE)(second))) << 8) |        \
        (((DWORD32)((BYTE)(third))) << 16) |        \
        (((DWORD32)((BYTE)(fourth))) << 24)         \
    ))

/**
 * \def MAKEWPARAM(first, second, third, fourth)
 * \brief Makes a WPARAM value with four bytes.
 *
 * \sa MAKEWORD, FIRSTBYTE, SECONDBYTE, THIRDBYTE, FOURTHBYTE
 */
#define MAKEWPARAM(first, second, third, fourth)    \
    ((WPARAM)(                                      \
        ((BYTE)(first)) |                           \
        (((WPARAM)((BYTE)(second))) << 8) |         \
        (((WPARAM)((BYTE)(third))) << 16) |         \
        (((WPARAM)((BYTE)(fourth))) << 24)          \
    ))

/**
 * \def FIRSTBYTE(w)
 * \brief Returns the first byte of the WPARAM \a w.
 *
 * \sa MAKEWPARAM
 */
#define FIRSTBYTE(w)        ((BYTE)(w))

/**
 * \def SECONDBYTE(w)
 * \brief Returns the second byte of the WPARAM \a w.
 *
 * \sa MAKEWPARAM
 */
#define SECONDBYTE(w)        ((BYTE)(((DWORD32)(w)) >> 8))

/**
 * \def THIRDBYTE(w)
 * \brief Returns the third byte of the WPARAM \a w.
 *
 * \sa MAKEWPARAM
 */
#define THIRDBYTE(w)        ((BYTE)(((DWORD32)(w)) >> 16))

/**
 * \def FOURTHBYTE(w)
 * \brief Returns the fourth byte of the WPARAM \a w.
 *
 * \sa MAKEWPARAM
 */
#define FOURTHBYTE(w)        ((BYTE)(((DWORD32)(w)) >> 24))

/**
 * \def LOWORD(l)
 * \brief Returns the low word of the double word \a l
 *
 * \sa MAKELONG
 */
#define LOWORD(l)           ((WORD)(DWORD)(l))
/**
 * \def HIWORD(l)
 * \brief Returns the high word of the double word \a l
 *
 * \sa MAKELONG
 */
#define HIWORD(l)           \
    ((WORD)((((DWORD)(l)) >> NR_BITS_WORD) & BITMASK_WORD))

/**
 * \def LOSWORD(l)
 * \brief Returns the low signed word of the double word \a l
 *
 * \sa MAKELONG
 */
#define LOSWORD(l)          ((SWORD)(DWORD)(l))
/**
 * \def HISWORD(l)
 * \brief Returns the high signed word of the double word \a l
 *
 * \sa MAKELONG
 */
#define HISWORD(l)          \
    ((SWORD)((((DWORD)(l)) >> NR_BITS_WORD) & BITMASK_WORD))

/**
 * \def MAKELONG32(low, high)
 * \brief Makes a 32-bit double word from \a low word and \a high word which are both in 16-bit.
 * \sa MAKELONG
 */
#define MAKELONG32(low, high)   \
    ((DWORD32)(((WORD16)(low)) | (((DWORD32)((WORD16)(high))) << 16)))

/**
 * \def MAKELONG(low, high)
 * \brief Makes a double word with pointer precision from \a low word and \a high word.
 * \sa MAKELONG32
 */
#define MAKELONG(low, high)     \
    ((DWORD)(((WORD)(low)) | (((DWORD)((WORD)(high))) << NR_BITS_WORD)))

/**
 * \var typedef DWORD32 RGBCOLOR
 * \brief A type definition for a RGB color value.
 */
typedef DWORD32 RGBCOLOR;

/**
 * \def GetRValue(rgba)
 * \brief Get the red component from a RGBA quadruple value \a rgba.
 *
 * You can make a RGBA triple by using MakeRGBA.
 *
 * \sa MakeRGBA, MakeRGB
 */
#define GetRValue(rgba)      ((BYTE)(rgba))
/**
 * \def GetGValue(rgba)
 * \brief Get the green component from a RGBA quadruple value \a rgba.
 *
 * You can make a RGBA triple by using MakeRGBA.
 *
 * \sa MakeRGBA, MakeRGB
 */
#define GetGValue(rgba)      ((BYTE)(((DWORD32)(rgba)) >> 8))
/**
 * \def GetBValue(rgba)
 * \brief Get the blue component from a RGBA quadruple value \a rgba.
 *
 * You can make a RGBA triple by using MakeRGBA.
 *
 * \sa MakeRGBA, MakeRGB
 */
#define GetBValue(rgba)      ((BYTE)((DWORD32)(rgba) >> 16))
/**
 * \def GetAValue(rgba)
 * \brief Get the alpha component from a RGBA quadruple value \a rgba.
 *
 * You can make a RGBA triple by using MakeRGBA.
 *
 * \sa MakeRGBA, MakeRGB
 */
#define GetAValue(rgba)      ((BYTE)((DWORD32)(rgba) >> 24))

/**
 * \def MakeRGBA(r, g, b, a)
 * \brief Makes a RGBA quadruple value from red \a r, green \a g,
 *        blue \a b and alpha \a components.
 *
 * \note The red, green, blue, and alpha components are all ranged
 *      from 0 to 255, and the returned value will be a 32-bit double word.
 *
 * \sa GetRValue, GetGValue, GetBValue, GetAValue
 */
#define MakeRGBA(r, g, b, a)                    \
                (((DWORD32)((BYTE)(r)))         \
                | ((DWORD32)((BYTE)(g)) << 8)   \
                | ((DWORD32)((BYTE)(b)) << 16)  \
                | ((DWORD32)((BYTE)(a)) << 24))

/**
 * \def MakeRGB(r, g, b)
 * \brief Makes a RGB triple value from red \a r, green \a g,
 *  and blue \a b components.
 *
 * \note The red, green, and blue components are all ranged from 0 to 255,
 * and the returned value will be a 32-bit double word.
 *
 * \sa GetRValue, GetGValue, GetBValue, GetAValue
 */
#define MakeRGB(r, g, b) MakeRGBA((r), (g), (b), 255)

/**
 * A rectangle defined by coordinates of corners.
 *
 * \note The lower-right corner does not belong to the rectangle,
 * i.e. the bottom horizontal line and the right vertical line are excluded
 * from the retangle.
 *
 * \sa PRECT, GAL_Rect
 */
typedef struct _RECT {
    /**
     * The x coordinate of the upper-left corner of the rectangle.
     */
    int left;
    /**
     * The y coordinate of the upper-left corner of the rectangle.
     */
    int top;
    /**
     * The x coordinate of the lower-right corner of the rectangle.
     */
    int right;
    /**
     * The y coordinate of the lower-right corner of the rectangle.
     */
    int bottom;
} RECT;
/**
 * \var typedef RECT* PRECT
 * \brief Data type of the pointer to a RECT.
 *
 * \sa RECT
 */
typedef RECT* PRECT;

/**
 * Point structure.
 * \sa PPOINT
 */
typedef struct _POINT
{
    /**
     * The x coordinate of the point.
     */
    int x;
    /**
     * The y coordinate of the point.
     */
    int y;
} POINT;
/**
 * \var typedef POINT* PPOINT
 * \brief Data type of the pointer to a POINT.
 *
 * \sa POINT
 */
typedef POINT* PPOINT;

/**
 * Size structure of a 2-dimension object.
 * \sa PSIZE
 */
typedef struct _SIZE
{
    /**
     * The extent in x coordinate of a 2D object.
     */
    int cx;
    /**
     * The extent in y coordinate of a 2D object.
     */
    int cy;
} SIZE;
/**
 * \var typedef SIZE* PSIZE
 * \brief Data type of the pointer to a SIZE.
 *
 * \sa SIZE
 */
typedef SIZE* PSIZE;

/**
 * RGB triple structure.
 * \sa PRGB, GAL_Color
 */
typedef struct _RGB
{
    /**
     * The red component of a RGB triple.
     */
    BYTE r;
    /**
     * The green component of a RGB triple.
     */
    BYTE g;
    /**
     * The blue component of a RGB triple.
     */
    BYTE b;
    /**
     * Reserved for alignment; or the alpha component of a RGBA quadruple.
     */
    BYTE a;
} RGB;

/**
 * \var typedef RGB* PRGB
 * \brief Data type of the pointer to a RGB.
 *
 * \sa RGB
 */
typedef RGB* PRGB;

    /** @} end of win32_types */

    /**
     * \defgroup gdi_types Data types for GDI
     * @{
     */
/**
 * \var typedef Sint8 gal_sint8
 * \brief Data type of 8-bit signed integer.
 *
 * \sa Sint8
 */
typedef Sint8       gal_sint8;
/**
 * \var typedef Uint8 gal_uint8
 * \brief Data type of 8-bit unsigned integer.
 *
 * \sa Uint8
 */
typedef Uint8       gal_uint8;

/**
 * \var typedef Sint16 gal_sint16
 * \brief Data type of 16-bit signed integer.
 *
 * \sa Sint16
 */
typedef Sint16      gal_sint16;
/**
 * \var typedef Uint16 gal_uint16
 * \brief Data type of 16-bit unsigned integer.
 *
 * \sa Uint16
 */
typedef Uint16      gal_uint16;

/**
 * \var typedef Sint32 gal_sint32
 * \brief Data type of 32-bit signed integer.
 *
 * \sa Sint32
 */
typedef Sint32      gal_sint32;
/**
 * \var typedef Uint32 gal_uint32
 * \brief Data type of 32-bit unsigned integer.
 *
 * \sa Uint32
 */
typedef Uint32      gal_uint32;

/**
 * \var typedef signed int gal_sint
 * \brief Data type of signed integer.
 */
typedef signed int      gal_sint;
/**
 * \var typedef unsigned int gal_uint
 * \brief Data type of unsigned integer.
 */
typedef unsigned int    gal_uint;

/**
 * \var typedef Uint32 gal_pixel
 * \brief Data type of pixel value
 */
typedef Uint32          gal_pixel;
/**
 * \var typedef Uint32 gal_attr
 * \brief Data type of attribute value
 */
typedef Uint32          gal_attr;

/**
 * \var typedef long fixed.
 * \brief Data type of fixed point.
 */
typedef long fixed;

/**
 * RGBA quarter structure.
 * \sa RGB
 */
typedef struct _GAL_Color
{
    /**
     * The red component of a RGBA quarter.
     */
    gal_uint8 r;
    /**
     * The green component of a RGBA quarter.
     */
    gal_uint8 g;
    /**
     * The blue component of a RGBA quarter.
     */
    gal_uint8 b;
    /**
     * The alpha component of a RGBA quarter.
     */
    gal_uint8 a;
} GAL_Color;

/**
 * Palette structure.
 * \sa GAL_Color
 */
typedef struct _GAL_Palette
{
    /**
     * The number of palette items.
     */
    int        ncolors;
    /**
     * The pointer to the array of palette items.
     */
    GAL_Color* colors;
} GAL_Palette;

/**
 * A rectangle defined by upper-left coordinates and width/height.
 * \sa RECT
 */
typedef struct _GAL_Rect {
    /**
     * The coordinates of the upper-left corner of the rectangle.
     */
    Sint32      x, y;
    /**
     * The width and height of the rectangle.
     */
    Sint32      w, h;
} GAL_Rect;

    /** @} end of gdi_types */

    /**
     * \defgroup key_defs Macros for key codes and shift status
     * @{
     */

/**
 * \def MGUI_NR_KEYS
 * \brief Number of MiniGUI keys.
 *
 * The number of MiniGUI keys is defined to 255 by default. This means that
 * MiniGUI can destinguish 255 different keys with each has an unique scan code.
 * The scan codes below 251 are defined for Linux keyboard keys by default.
 * If your system has a large amount of keys, you can define the scan code of
 * keys ranged from 1 to 255 in your IAL engine. And your application will
 * receive a MSG_KEYDOWN and MSG_KEYUP messages when a key pressed and
 * released, and the wParam of the messages will be defined to be equal to
 * the scan code of the key.
 *
 * \sa NR_KEYS, SCANCODE_USER
 */
#define MGUI_NR_KEYS                    255

/**
 * \def NR_KEYS
 * \brief The number of keys defined by Linux operating system.
 *
 * For a Linux box, NR_KEYS is defined to 250 by default. You can define
 * some input events from an input device other than keyboard, e.g.
 * your remote controller, as key events with different scan codes from
 * those of PC's. MiniGUI can support 255 keys, and the constant
 * is defined by MGUI_NR_KEYS.
 *
 * \sa MGUI_NR_KEYS
 */
#ifndef NR_KEYS
#define NR_KEYS                         250
#endif

/**
 * \def SCANCODE_USER
 * \brief The first key scan code different from OS defined ones.
 *
 * You can define your special key scan codes like below
 *
 * \code
 * #define SCANCODE_PLAY    (SCANCODE_USER)
 * #define SCANCODE_STOP    (SCANCODE_USER + 1)
 * #define SCANCODE_PAUSE   (SCANCODE_USER + 2)
 * \endcode
 *
 * to distinguish the keys on your remote controller.
 *
 * \sa MGUI_NR_KEYS, NR_KEYS
 */
#define SCANCODE_USER                   (NR_KEYS + 1)

#define SCANCODE_RESERVED               0
#define SCANCODE_ESCAPE                 1
    #define SCANCODE_ESC                    SCANCODE_ESCAPE

#define SCANCODE_1                      2
#define SCANCODE_2                      3
#define SCANCODE_3                      4
#define SCANCODE_4                      5
#define SCANCODE_5                      6
#define SCANCODE_6                      7
#define SCANCODE_7                      8
#define SCANCODE_8                      9
#define SCANCODE_9                      10
#define SCANCODE_0                      11

#define SCANCODE_MINUS                  12
#define SCANCODE_EQUAL                  13

#define SCANCODE_BACKSPACE              14
#define SCANCODE_TAB                    15

#define SCANCODE_Q                      16
#define SCANCODE_W                      17
#define SCANCODE_E                      18
#define SCANCODE_R                      19
#define SCANCODE_T                      20
#define SCANCODE_Y                      21
#define SCANCODE_U                      22
#define SCANCODE_I                      23
#define SCANCODE_O                      24
#define SCANCODE_P                      25
#define SCANCODE_BRACKET_LEFT           26
    #define SCANCODE_LEFTBRACE              SCANCODE_BRACKET_LEFT
#define SCANCODE_BRACKET_RIGHT          27
    #define SCANCODE_RIGHTBRACE             SCANCODE_BRACKET_RIGHT

#define SCANCODE_ENTER                  28

#define SCANCODE_LEFTCONTROL            29
    #define SCANCODE_LEFTCTRL               SCANCODE_LEFTCONTROL

#define SCANCODE_A                      30
#define SCANCODE_S                      31
#define SCANCODE_D                      32
#define SCANCODE_F                      33
#define SCANCODE_G                      34
#define SCANCODE_H                      35
#define SCANCODE_J                      36
#define SCANCODE_K                      37
#define SCANCODE_L                      38
#define SCANCODE_SEMICOLON              39
#define SCANCODE_APOSTROPHE             40
#define SCANCODE_GRAVE                  41
#define SCANCODE_LEFTSHIFT              42
#define SCANCODE_BACKSLASH              43

#define SCANCODE_Z                      44
#define SCANCODE_X                      45
#define SCANCODE_C                      46
#define SCANCODE_V                      47
#define SCANCODE_B                      48
#define SCANCODE_N                      49
#define SCANCODE_M                      50
#define SCANCODE_COMMA                  51
#define SCANCODE_PERIOD                 52
    #define SCANCODE_DOT                    SCANCODE_PERIOD
#define SCANCODE_SLASH                  53
#define SCANCODE_RIGHTSHIFT             54
#define SCANCODE_KEYPADMULTIPLY         55
    #define SCANCODE_KPASTERISK             SCANCODE_KEYPADMULTIPLY
#define SCANCODE_LEFTALT                56
#define SCANCODE_SPACE                  57
#define SCANCODE_CAPSLOCK               58

#define SCANCODE_F1                     59
#define SCANCODE_F2                     60
#define SCANCODE_F3                     61
#define SCANCODE_F4                     62
#define SCANCODE_F5                     63
#define SCANCODE_F6                     64
#define SCANCODE_F7                     65
#define SCANCODE_F8                     66
#define SCANCODE_F9                     67
#define SCANCODE_F10                    68

#define SCANCODE_NUMLOCK                69
#define SCANCODE_SCROLLLOCK             70

#define SCANCODE_KEYPAD7                71
    #define SCANCODE_KP7                    SCANCODE_KEYPAD7
#define SCANCODE_CURSORUPLEFT           71
#define SCANCODE_KEYPAD8                72
    #define SCANCODE_KP8                    SCANCODE_KEYPAD8
#define SCANCODE_CURSORUP               72
#define SCANCODE_KEYPAD9                73
    #define SCANCODE_KP9                    SCANCODE_KEYPAD9
#define SCANCODE_CURSORUPRIGHT          73
#define SCANCODE_KEYPADMINUS            74
    #define SCANCODE_KPMINUS                SCANCODE_KEYPADMINUS
#define SCANCODE_KEYPAD4                75
    #define SCANCODE_KP4                    SCANCODE_KEYPAD4
#define SCANCODE_CURSORLEFT             75
#define SCANCODE_KEYPAD5                76
    #define SCANCODE_KP5                    SCANCODE_KEYPAD5
#define SCANCODE_KEYPAD6                77
    #define SCANCODE_KP6                    SCANCODE_KEYPAD6
#define SCANCODE_CURSORRIGHT            77
#define SCANCODE_KEYPADPLUS             78
    #define SCANCODE_KPPLUS                 SCANCODE_KEYPADPLUS
#define SCANCODE_KEYPAD1                79
    #define SCANCODE_KP1                    SCANCODE_KEYPAD1
#define SCANCODE_CURSORDOWNLEFT         79
#define SCANCODE_KEYPAD2                80
    #define SCANCODE_KP2                    SCANCODE_KEYPAD2
#define SCANCODE_CURSORDOWN             80
#define SCANCODE_KEYPAD3                81
    #define SCANCODE_KP3                    SCANCODE_KEYPAD3
#define SCANCODE_CURSORDOWNRIGHT        81
#define SCANCODE_KEYPAD0                82
    #define SCANCODE_KP0                    SCANCODE_KEYPAD0
#define SCANCODE_KEYPADPERIOD           83
    #define SCANCODE_KPDOT                  SCANCODE_KEYPADPERIOD

#define SCANCODE_ZENKAKUHANKAKU         85

#define SCANCODE_LESS                   86
#define SCANCODE_102ND                      SCANCODE_LESS

#define SCANCODE_F11                    87
#define SCANCODE_F12                    88

#define SCANCODE_RO                     89
#define SCANCODE_KATAKANA               90
#define SCANCODE_HIRAGANA               91
#define SCANCODE_HENKAN                 92
#define SCANCODE_KATAKANAHIRAGANA       93
#define SCANCODE_MUHENKAN               94
#define SCANCODE_KPJPCOMMA              95

#define SCANCODE_KEYPADENTER            96
    #define SCANCODE_KPENTER                SCANCODE_KEYPADENTER
#define SCANCODE_RIGHTCONTROL           97
    #define SCANCODE_RIGHTCTRL              SCANCODE_RIGHTCONTROL
#define SCANCODE_CONTROL                97
#define SCANCODE_KEYPADDIVIDE           98
    #define SCANCODE_KPSLASH                SCANCODE_KEYPADDIVIDE
#define SCANCODE_PRINTSCREEN            99
    #define SCANCODE_SYSRQ                  SCANCODE_PRINTSCREEN
#define SCANCODE_RIGHTALT               100
#define SCANCODE_LINEFEED               101

#define SCANCODE_HOME                   102
#define SCANCODE_CURSORBLOCKUP          103    /* Cursor key block */
    #define SCANCODE_UP                     SCANCODE_CURSORBLOCKUP
#define SCANCODE_PAGEUP                 104
#define SCANCODE_CURSORBLOCKLEFT        105    /* Cursor key block */
    #define SCANCODE_LEFT                   SCANCODE_CURSORBLOCKLEFT
#define SCANCODE_CURSORBLOCKRIGHT       106    /* Cursor key block */
    #define SCANCODE_RIGHT                  SCANCODE_CURSORBLOCKRIGHT
#define SCANCODE_END                    107
#define SCANCODE_CURSORBLOCKDOWN        108    /* Cursor key block */
    #define SCANCODE_DOWN                   SCANCODE_CURSORBLOCKDOWN
#define SCANCODE_PAGEDOWN               109
#define SCANCODE_INSERT                 110
#define SCANCODE_REMOVE                 111
    #define SCANCODE_DELETE                 SCANCODE_REMOVE

#define SCANCODE_MACRO                  112
#define SCANCODE_MUTE                   113
#define SCANCODE_VOLUMEDOWN             114
#define SCANCODE_VOLUMEUP               115
#define SCANCODE_POWER                  116    /* SC System Power Down */
#define SCANCODE_KPEQUAL                117
#define SCANCODE_KPPLUSMINUS            118
#define SCANCODE_BREAK                  119
    #define SCANCODE_BREAK_ALTERNATIVE      SCANCODE_BREAK
    #define SCANCODE_PAUSE                  SCANCODE_BREAK

#define SCANCODE_SCALE                  120    /* AL Compiz Scale (Expose) */
#define SCANCODE_KPCOMMA                121
#define SCANCODE_HANGEUL                122
#define SCANCODE_HANJA                  123
#define SCANCODE_YEN                    124
#define SCANCODE_LEFTWIN                125
    #define SCANCODE_LEFTMETA               SCANCODE_LEFTWIN
#define SCANCODE_RIGHTWIN               126
    #define SCANCODE_RIGHTMETA              SCANCODE_RIGHTWIN
#define SCANCODE_COMPOSE                127
#define SCANCODE_STOP                   128    /* AC Stop */
#define SCANCODE_AGAIN                  129
#define SCANCODE_PROPS                  130    /* AC Properties */
#define SCANCODE_UNDO                   131    /* AC Undo */
#define SCANCODE_FRONT                  132
#define SCANCODE_COPY                   133    /* AC Copy */
#define SCANCODE_OPEN                   134    /* AC Open */
#define SCANCODE_PASTE                  135    /* AC Paste */
#define SCANCODE_FIND                   136    /* AC Search */
#define SCANCODE_CUT                    137    /* AC Cut */
#define SCANCODE_HELP                   138    /* AL Integrated Help Center */
#define SCANCODE_MENU                   139    /* Menu (show menu) */
#define SCANCODE_CALC                   140    /* AL Calculator */
#define SCANCODE_SETUP                  141
#define SCANCODE_SLEEP                  142    /* SC System Sleep */
#define SCANCODE_WAKEUP                 143    /* System Wake Up */
#define SCANCODE_FILE                   144    /* AL Local Machine Browser */
#define SCANCODE_SENDFILE               145
#define SCANCODE_DELETEFILE             146
#define SCANCODE_XFER                   147
#define SCANCODE_PROG1                  148
#define SCANCODE_PROG2                  149
#define SCANCODE_WWW                    150    /* AL Internet Browser */
#define SCANCODE_MSDOS                  151
#define SCANCODE_COFFEE                 152    /* AL Terminal Lock/Screensaver */
    #define SCANCODE_SCREENLOCK             SCANCODE_COFFEE
#define SCANCODE_ROTATE_DISPLAY         153    /* Display orientation for e.g. tablets */
    #define SCANCODE_DIRECTION              SCANCODE_ROTATE_DISPLAY
#define SCANCODE_CYCLEWINDOWS           154
#define SCANCODE_MAIL                   155
#define SCANCODE_BOOKMARKS              156    /* AC Bookmarks */
#define SCANCODE_COMPUTER               157
#define SCANCODE_BACK                   158    /* AC Back */
#define SCANCODE_FORWARD                159    /* AC Forward */
#define SCANCODE_CLOSECD                160
#define SCANCODE_EJECTCD                161
#define SCANCODE_EJECTCLOSECD           162
#define SCANCODE_NEXTSONG               163
#define SCANCODE_PLAYPAUSE              164
#define SCANCODE_PREVIOUSSONG           165
#define SCANCODE_STOPCD                 166
#define SCANCODE_RECORD                 167
#define SCANCODE_REWIND                 168
#define SCANCODE_PHONE                  169    /* Media Select Telephone */
#define SCANCODE_ISO                    170
#define SCANCODE_CONFIG                 171    /* AL Consumer Control Configuration */
#define SCANCODE_HOMEPAGE               172    /* AC Home */
#define SCANCODE_REFRESH                173    /* AC Refresh */
#define SCANCODE_EXIT                   174    /* AC Exit */
#define SCANCODE_MOVE                   175
#define SCANCODE_EDIT                   176
#define SCANCODE_SCROLLUP               177
#define SCANCODE_SCROLLDOWN             178
#define SCANCODE_KPLEFTPAREN            179
#define SCANCODE_KPRIGHTPAREN           180
#define SCANCODE_NEW                    181    /* AC New */
#define SCANCODE_REDO                   182    /* AC Redo/Repeat */
#define SCANCODE_F13                    183
#define SCANCODE_F14                    184
#define SCANCODE_F15                    185
#define SCANCODE_F16                    186
#define SCANCODE_F17                    187
#define SCANCODE_F18                    188
#define SCANCODE_F19                    189
#define SCANCODE_F20                    190
#define SCANCODE_F21                    191
#define SCANCODE_F22                    192
#define SCANCODE_F23                    193
#define SCANCODE_F24                    194

#define SCANCODE_PLAYCD                 200
#define SCANCODE_PAUSECD                201
#define SCANCODE_PROG3                  202
#define SCANCODE_PROG4                  203
#define SCANCODE_DASHBOARD              204    /* AL Dashboard */
#define SCANCODE_SUSPEND                205
#define SCANCODE_CLOSE                  206    /* AC Close */
#define SCANCODE_PLAY                   207
#define SCANCODE_FASTFORWARD            208
#define SCANCODE_BASSBOOST              209
#define SCANCODE_PRINT                  210    /* AC Print */
#define SCANCODE_HP                     211
#define SCANCODE_CAMERA                 212
#define SCANCODE_SOUND                  213
#define SCANCODE_QUESTION               214
#define SCANCODE_EMAIL                  215
#define SCANCODE_CHAT                   216
#define SCANCODE_SEARCH                 217
#define SCANCODE_CONNECT                218
#define SCANCODE_FINANCE                219    /* AL Checkbook/Finance */
#define SCANCODE_SPORT                  220
#define SCANCODE_SHOP                   221
#define SCANCODE_ALTERASE               222
#define SCANCODE_CANCEL                 223    /* AC Cancel */
#define SCANCODE_BRIGHTNESSDOWN         224
#define SCANCODE_BRIGHTNESSUP           225
#define SCANCODE_MEDIA                  226
#define SCANCODE_SWITCHVIDEOMODE        227    /* Cycle between available video outputs (Monitor/LCD/TV-out/etc) */
#define SCANCODE_KBDILLUMTOGGLE         228
#define SCANCODE_KBDILLUMDOWN           229
#define SCANCODE_KBDILLUMUP             230
#define SCANCODE_SEND                   231    /* AC Send */
#define SCANCODE_REPLY                  232    /* AC Reply */
#define SCANCODE_FORWARDMAIL            233    /* AC Forward Msg */
#define SCANCODE_SAVE                   234    /* AC Save */
#define SCANCODE_DOCUMENTS              235
#define SCANCODE_BATTERY                236
#define SCANCODE_BLUETOOTH              237
#define SCANCODE_WLAN                   238
#define SCANCODE_UWB                    239
#define SCANCODE_UNKNOWN                240
#define SCANCODE_VIDEO_NEXT             241    /* drive next video source */
#define SCANCODE_VIDEO_PREV             242    /* drive previous video source */
#define SCANCODE_BRIGHTNESS_CYCLE       243    /* brightness up, after max is min */
#define SCANCODE_BRIGHTNESS_AUTO        244    /* Set Auto Brightness: manual brightness control is off, rely on ambient */
    #define SCANCODE_BRIGHTNESS_ZERO        SCANCODE_BRIGHTNESS_AUTO
#define SCANCODE_DISPLAY_OFF            245    /* display device to off state */
#define SCANCODE_WWAN                   246    /* Wireless WAN (LTE, UMTS, GSM, etc.) */
    #define SCANCODE_WIMAX                  SCANCODE_WWAN
#define SCANCODE_RFKILL                 247    /* Key that controls all radios */
#define SCANCODE_MICMUTE                248    /* Mute / unmute the microphone */

#define SCANCODE_LEFTBUTTON             0x1000
#define SCANCODE_RIGHTBUTTON            0x2000
#define SCANCODE_MIDDLBUTTON            0x4000

/**
 * \def KS_LEFTMETA
 * \brief This status indicate that left-Meta key was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 *
 * Since 4.0.0
 */
#define KS_LEFTMETA                    0x00002000

/**
 * \def KS_RIGHTMETA
 * \brief This status indicate that right-Meta key was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 *
 * Since 4.0.0
 */
#define KS_RIGHTMETA                   0x00001000

/**
 * \def KS_META
 * \brief This status indicate that either the left-Meta key or the right-Meta key
 * was pressed when the key or mouse message posted to the window.
 *
 * \sa key_msgs
 *
 * Since 4.0.0
 */
#define KS_META                        0x00003000

/**
 * \def KS_REPEATED
 * \brief This status indicate that the key down message is an
 *        auto-repeated one.
 *
 * You can test the status by AND'ed with lParam of the message, like below:
 *
 * \code
 *      switch (message) {
 *      case MSG_KEYDOWN:
 *          if (lParam & KS_REPEATED) {
 *              // the key down messsage is auto-repeated.
 *              ...
 *          }
 *          break;
 *      ...
 * \endcode
 *
 * \sa key_msgs
 */
#define KS_REPEATED                     0x00000800

/**
 * \def KS_CAPTURED
 * \brief This status indicate that the mouse is captured by a window when
 * the mouse message posted.
 *
 * You can test the status by AND'ed with wParam of the message, like below:
 *
 * \code
 *      switch (message) {
 *      case MSG_MOUSEMOVE:
 *          if (wParam & KS_CAPTURED) {
 *              // the mouse is captured by this window.
 *              ...
 *          }
 *          break;
 *      ...
 * \endcode
 *
 * \sa mouse_msgs
 */
#define KS_CAPTURED                     0x00000400

/**
 * \def KS_IMEPOST
 * \brief This status indicate that the key message is posted by the IME window.
 *
 * \sa key_msgs
 */
#define KS_IMEPOST                      0x00000200

/**
 * \def KS_CAPSLOCK
 * \brief This status indicate that the CapsLock key was locked when
 * the key or mouse message posted to the window.
 *
 * You can test the status by AND'ed with lParam of the message, like below
 *
 * \code
 *      switch (message) {
 *      case MSG_KEYDOWN:
 *          if (lParam & KS_CAPSLOCK) {
 *              // the CapsLock key is locked.
 *              ...
 *          }
 *          break;
 *      ...
 * \endcode
 *
 * \sa key_msgs
 */
#define KS_CAPSLOCK                     0x00000100

/**
 * \def KS_NUMLOCK
 * \brief This status indicate that the NumLock key was locked when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_NUMLOCK                      0x00000080

/**
 * \def KS_SCROLLLOCK
 * \brief This status indicate that the ScrollLock key was locked when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_SCROLLLOCK                   0x00000040

/**
 * \def KS_LEFTCTRL
 * \brief This status indicate that the left-Ctrl key was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_LEFTCTRL                     0x00000020

/**
 * \def KS_RIGHTCTRL
 * \brief This status indicate that the right-Ctrl key was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_RIGHTCTRL                    0x00000010

/**
 * \def KS_CTRL
 * \brief This status indicate that either the left-Ctrl key or the
 * right-Ctrl key was pressed when the key or mouse message posted
 * to the window.
 *
 * \sa key_msgs
 */
#define KS_CTRL                         0x00000030

/**
 * \def KS_LEFTALT
 * \brief This status indicate that left-Alt key was pressed when
 * the key  or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_LEFTALT                      0x00000008

/**
 * \def KS_RIGHTALT
 * \brief This status indicate that right-Alt key was pressed when
 * the key  or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_RIGHTALT                     0x00000004

/**
 * \def KS_ALT
 * \brief This status indicate that either the left-Alt key or the right-Alt key
 * was pressed when the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_ALT                          0x0000000C

/**
 * \def KS_LEFTSHIFT
 * \brief This status indicate that left-Shift key was pressed when
 * the key  or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_LEFTSHIFT                    0x00000002

/**
 * \def KS_RIGHTSHIFT
 * \brief This status indicate that right-Shift key was pressed when
 * the key  or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_RIGHTSHIFT                   0x00000001

/**
 * \def KS_SHIFT
 * \brief This status indicate that either the left-Shift key or
 * the right-Shift key was pressed when the key or mouse message
 * posted to the window.
 *
 * \sa key_msgs
 */
#define KS_SHIFT                        0x00000003

/**
 * \def MASK_KS_SHIFTKEYS
 * \brief The mask of key status.
 */
#define MASK_KS_SHIFTKEYS               0x0000FFFF

/**
 * \def KS_LEFTBUTTON
 * \brief This status indicate that left button was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_LEFTBUTTON                   0x00010000

/**
 * \def KS_RIGHTBUTTON
 * \brief This status indicate that right button was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_RIGHTBUTTON                  0x00020000

/**
 * \def KS_MIDDLEBUTTON
 * \brief This status indicate that middle button was pressed when
 * the key or mouse message posted to the window.
 *
 * \sa key_msgs
 */
#define KS_MIDDLEBUTTON                 0x00040000

/**
 * \def MASK_KS_BUTTONS
 * \brief The mask of mouse button status.
 */
#define MASK_KS_BUTTONS                 0x000F0000

    /** @} end of key_defs */

    /**
     * \defgroup err_codes Error codes
     * @{
     */
/**
 * \def ERR_OK
 * \brief Return value error ok.
 */
#define ERR_OK                   0

/**
 * \def ERR_QUEUE_FULL
 * \brief Return value queue is full error.
 */
#define ERR_QUEUE_FULL          -1

/**
 * \def ERR_INVALID_HANDLE
 * \brief Return value invalid handle error.
 */
#define ERR_INVALID_HANDLE      -2

/**
 * \def ERR_INV_HWND
 * \brief Return value invalid window handle error.
 */
#define ERR_INV_HWND            ERR_INVALID_HANDLE

/**
 * \def ERR_INVALID_HMENU
 * \brief Invalid menu handle.
 */
#define ERR_INVALID_HMENU       ERR_INVALID_HANDLE

/**
 * \def ERR_MSG_CANCELED
 * \brief The message handling was canceled.
 */
#define ERR_MSG_CANCELED        -3

/**
 * \def ERR_INVALID_POS
 * \brief Invalid postion.
 */
#define ERR_INVALID_POS         -5

/**
 * \def ERR_INVALID_ID
 * \brief Invalid identifier.
 */
#define ERR_INVALID_ID          -6

/**
 * \def ERR_RES_ALLOCATION
 * \brief Failed to allocate memory.
 */
#define ERR_RES_ALLOCATION      -7

/**
 * \def ERR_CTRLCLASS_INVNAME
 * \brief Return value invalid name of control class error.
 */
#define ERR_CTRLCLASS_INVNAME   -8

/**
 * \def ERR_CTRLCLASS_INVLEN
 * \brief Return value control class invalid length.
 */
#define ERR_CTRLCLASS_INVLEN    -9

/**
 * \def ERR_CTRLCLASS_MEM
 * \brief Return value control class memory error.
 */
#define ERR_CTRLCLASS_MEM       -10

/**
 * \def ERR_CTRLCLASS_INUSE
 * \brief Return value inuse control class error.
 */
#define ERR_CTRLCLASS_INUSE     -11

/**
 * \def ERR_ALREADY_EXIST
 * \brief Return value already exit error.
 */
#define ERR_ALREADY_EXIST       -12

/**
 * \def ERR_NO_MATCH
 * \brief Return no match error.
 */
#define ERR_NO_MATCH            -13

/**
 * \def ERR_BAD_OWNER
 * \brief Return bad owner error.
 */
#define ERR_BAD_OWNER           -14

/**
 * \def ERR_IME_TOOMUCHIMEWND
 * \brief Return value too much IME window error.
 */
#define ERR_IME_TOOMUCHIMEWND   -15

/**
 * \def ERR_IME_NOSUCHIMEWND
 * \brief Return value no such IME wondow error.
 */
#define ERR_IME_NOSUCHIMEWND    -16

/**
 * \def ERR_IME_NOIMEWND
 * \brief Return value no IME wondow error.
 */
#define ERR_IME_NOIMEWND        -17

/**
 * \def ERR_CONFIG_FILE
 * \brief Return value configure file error.
 */
#define ERR_CONFIG_FILE         -18

/**
 * \def ERR_FILE_IO
 * \brief Return value file I/O error.
 */
#define ERR_FILE_IO             -19

/**
 * \def ERR_GFX_ENGINE
 * \brief Return value GFX engine error.
 */
#define ERR_GFX_ENGINE          -20

/**
 * \def ERR_INPUT_ENGINE
 * \brief Return value input engine error.
 */
#define ERR_INPUT_ENGINE        -21

/**
 * \def ERR_NO_ENGINE
 * \brief Return value no engine error.
 */
#define ERR_NO_ENGINE           -22

/**
 * \def ERR_INVALID_ARGS
 * \brief Return value invalid arguments.
 */
#define ERR_INVALID_ARGS        -23

    /** @} end of err_codes */

    /**
     * \defgroup misc_macros Miscellaneous macros
     * @{
     */
/**
 * \def TABLESIZE(table)
 * \brief A macro returns the number of elements in a \a table.
 */
#define TABLESIZE(table)    (sizeof(table)/sizeof(table[0]))

/* MAX/MIN/ABS macors */
/**
 * \def MAX(x, y)
 * \brief A macro returns the maximum of \a x and \a y.
 */
#ifndef MAX
#define MAX(x, y)           (((x) > (y))?(x):(y))
#endif
/**
 * \def MIN(x, y)
 * \brief A macro returns the minimum of \a x and \a y.
 */
#ifndef MIN
#define MIN(x, y)           (((x) < (y))?(x):(y))
#endif
/**
 * \def ABS(x)
 * \brief A macro returns the absolute value of \a x.
 */
#ifndef ABS
#define ABS(x)              (((x)<0) ? -(x) : (x))
#endif

/* Commonly used definitions */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef PATH_MAX
#   define PATH_MAX    256
#endif

#ifndef NAME_MAX
#   define NAME_MAX    64
#endif


/**
 * \def MAX_PATH
 * \brief The possible maximal length of a path name.
 * \note This definition is an alias of PATH_MAX
 */
#ifndef MAX_PATH
#   define MAX_PATH        PATH_MAX
#endif

/**
 * \def MAX_NAME
 * \brief The possible maximal length of a file name.
 * \note This definition is an alias of NAME_MAX
 */
#ifndef MAX_NAME
#   define MAX_NAME        NAME_MAX
#endif

    /** @} end of misc_macros */

    /** @} end of macros_types */

#ifdef HAVE_TIME
#include <time.h>
#else
typedef unsigned long time_t;

struct tm {
    int    tm_sec;   /* seconds [0,61] */
    int    tm_min;   /* minutes [0,59] */
    int    tm_hour;  /* hour [0,23] */
    int    tm_mday;  /* day of month [1,31] */
    int    tm_mon;   /* month of year [0,11] */
    int    tm_year;  /* years since 1900 */
    int    tm_wday;  /* day of week [0,6] (Sunday = 0) */
    int    tm_yday;  /* day of year [0,365] */
    int    tm_isdst; /* daylight savings flag */
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
#include  "fx_api.h"
#include  "tx_api.h"
#include  "os_type.h"
#include  "os_file_api.h"

#define fopen       tp_fopen
#define fclose      tp_fclose
#define fwrite      tp_fwrite
#define fread       tp_fread
#define fseek       tp_fseek
#define feof        tp_feof

#undef assert
#define _HAVE_ASSERT 1

#define assert(e)   \
    do {            \
        e;          \
    } while (0)

#undef stdin
#undef stdout
#undef stderr

#define stdin ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)
void Comm_Lock_Screen (void);
void Comm_Unlock_Screen (void);

#endif

#ifdef __UCOSII__

/* use our own implementation of strdup */
#undef HAVE_STRDUP
#undef strdup
#define strdup own_strdup

#endif /* __UCOSII__ */

#ifndef HAVE_STRDUP
MG_EXPORT char *strdup(const char *s);
#endif

#ifndef HAVE_STRCASECMP
MG_EXPORT int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef HAVE_STRNCASECMP
MG_EXPORT int strncasecmp(const char *s1, const char *s2, unsigned int n);
#endif

#ifdef _MGUSE_OWN_MALLOC

/**
 * \fn int init_minigui_malloc (unsigned char* heap, unsigned int heap_size,
                int (*lock_heap) (void), int (*unlock_heap) (void))
 * \brief Initialize MiniGUI's own heap system.
 *
 * MiniGUI implements a heap system which is compatbile with ANIS C. When the
 * RTOS does not provide a re-entriable malloc implementation, you can use
 * MiniGUI's heap system.
 *
 * Before using MiniGUI's own heap system, you should provide a memory range
 * to be the heap, and provided two functions to lock the heap and unlock
 * the heap.
 *
 * \param heap The pointer to the heap bottom.
 * \param heap_size The size of the heap in bytes.
 * \param lock_heap A function will be called by MiniGUI's own heap system
 *      to lock the heap.
 * \param lock_heap A function will be called by MiniGUI's own heap system
 *      to unlock the heap.
 *
 * \return Zero on success, non-zero on error.
 */
int init_minigui_malloc (unsigned char* heap, unsigned int heap_size,
                int (*lock_heap) (void), int (*unlock_heap) (void));

#define USE_DL_PREFIX

#include "own_malloc.h"

/* wrappers for malloc functions */
#define calloc      dlcalloc
#define free        dlfree
#define malloc      dlmalloc
#define memalign    dlmemalign
#define realloc     dlrealloc
#define valloc      dlvalloc

#endif

/* Do not use the alloca of ARMCC */
#if defined(__CC_ARM)
#   undef HAVE_ALLOCA
#   undef HAVE_ALLOCA_H
#endif

#ifdef HAVE_ALLOCA_H
#   include <alloca.h>
#   define ALLOCATE_LOCAL(size)    alloca((int)(size))
#   define DEALLOCATE_LOCAL(ptr)   /* as nothing */
#else
#   define ALLOCATE_LOCAL(size)    malloc((int)(size))
#   define DEALLOCATE_LOCAL(ptr)   free(ptr)
#endif

#ifdef _MGUSE_OWN_STDIO

/**
 * \fn int init_minigui_printf (int (*output_char) (int ch), int (*input_char) (void))
 * \brief Initiliaze MiniGUI's own printf implementation.
 *
 * This function will initialize MiniGUI's own printf implementation. This implementation
 * provides the interface compatible with ANSI C's standard io interface.
 *
 * \param output_char A callback function to output one character.
 * \param input_char A callback function to input one character.
 *
 * \return Zero no success, non-zero on error.
 */
int init_minigui_printf (int (*output_char) (int ch),
                int (*input_char) (void));

#if defined (__UCOSII__) || defined (__VXWORKS__) || defined (__NUCLEUS__) || defined (__PSOS__)
#   undef _PRINTF_FLOATING_POINT
#   undef _SCANF_FLOATING_POINT
#else
#   ifdef HAVE_MATH_H
#       define _PRINTF_FLOATING_POINT      1
#       define _SCANF_FLOATING_POINT       1
#   endif
#endif

#ifdef WIN32
#   include <float.h>
#   define isnan    _isnan
#   define finite   _finite
#endif

#undef _I18N_MB_REQUIRED

#if defined(__VXWORKS__) || defined(WIN32) || defined (__NUCLEUS_MNT__) || defined (__PSOS__)
  #define _MGUSE_OWN_SNPRINTF
  #define _MGUSE_OWN_VSNPRINTF
  #define _MGUSE_OWN_VFNPRINTF
#else
  #define _MGUSE_OWN_PRINTF
  #define _MGUSE_OWN_FPRINTF
  #define _MGUSE_OWN_SPRINTF
  #define _MGUSE_OWN_FNPRINTF
  #define _MGUSE_OWN_SNPRINTF
  #define _MGUSE_OWN_VPRINTF
  #define _MGUSE_OWN_VFPRINTF
  #define _MGUSE_OWN_VSPRINTF
  #define _MGUSE_OWN_VFNPRINTF
  #define _MGUSE_OWN_VSNPRINTF
  #define _MGUSE_OWN_SCANF
  #define _MGUSE_OWN_FSCANF
  #define _MGUSE_OWN_SSCANF
  #define _MGUSE_OWN_VSCANF
  #define _MGUSE_OWN_VFSCANF
  #define _MGUSE_OWN_VSSCANF
#endif

#include "own_stdio.h"

/* wrappers for stdio functions */
#ifdef _MGUSE_OWN_PRINTF
  #define printf      own_printf
#endif

#ifdef _MGUSE_OWN_FPRINTF
  #define fprintf     own_fprintf
#endif

#ifdef _MGUSE_OWN_SPRINTF
  #define sprintf     own_sprintf
#endif

#ifdef _MGUSE_OWN_FNPRINTF
  #define fnprintf    own_fnprintf
#endif

#ifdef _MGUSE_OWN_SNPRINTF
  #define snprintf    own_snprintf
#endif

#ifdef _MGUSE_OWN_VPRINTF
  #define vprintf     own_vprintf
#endif

#ifdef _MGUSE_OWN_VFPRINTF
  #define vfprintf    own_vfprintf
#endif

#ifdef _MGUSE_OWN_VSPRINTF
  #define vsprintf    own_vsprintf
#endif

#ifdef _MGUSE_OWN_VFNPRINTF
  #define vfnprintf   own_vfnprintf
#endif

#ifdef _MGUSE_OWN_VSNPRINTF
  #define vsnprintf   own_vsnprintf
#endif

#ifdef _MGUSE_OWN_SCANF
  #define scanf       own_scanf
#endif

#ifdef _MGUSE_OWN_FSCANF
  #define fscanf      own_fscanf
#endif

#ifdef _MGUSE_OWN_SSCANF
  #define sscanf      own_sscanf
#endif

#ifdef _MGUSE_OWN_VSCANF
  #define vscanf      own_vscanf
#endif

#ifdef _MGUSE_OWN_VFSCANF
  #define vfscanf     own_vfscanf
#endif

#ifdef _MGUSE_OWN_VSSCANF
  #define vsscanf     own_vsscanf
#endif

#endif /* _MGUSE_OWN_STDIO */

#ifdef __LINUX__
    #define TCS_NONE(fp)            fprintf (fp, "\e[0m")
    #define TCS_BLACK(fp)           fprintf (fp, "\e[0;30m")
    #define TCS_BOLD_BLACK(fp)      fprintf (fp, "\e[1;30m")
    #define TCS_RED(fp)             fprintf (fp, "\e[0;31m")
    #define TCS_BOLD_RED(fp)        fprintf (fp, "\e[1;31m")
    #define TCS_GREEN(fp)           fprintf (fp, "\e[0;32m")
    #define TCS_BOLD_GREEN(fp)      fprintf (fp, "\e[1;32m")
    #define TCS_BROWN(fp)           fprintf (fp, "\e[0;33m")
    #define TCS_YELLOW(fp)          fprintf (fp, "\e[1;33m")
    #define TCS_BLUE(fp)            fprintf (fp, "\e[0;34m")
    #define TCS_BOLD_BLUE(fp)       fprintf (fp, "\e[1;34m")
    #define TCS_PURPLE(fp)          fprintf (fp, "\e[0;35m")
    #define TCS_BOLD_PURPLE(fp)     fprintf (fp, "\e[1;35m")
    #define TCS_CYAN(fp)            fprintf (fp, "\e[0;36m")
    #define TCS_BOLD_CYAN(fp)       fprintf (fp, "\e[1;36m")
    #define TCS_GRAY(fp)            fprintf (fp, "\e[0;37m")
    #define TCS_WHITE(fp)           fprintf (fp, "\e[1;37m")
    #define TCS_BOLD(fp)            fprintf (fp, "\e[1m")
    #define TCS_UNDERLINE(fp)       fprintf (fp, "\e[4m")
    #define TCS_BLINK(fp)           fprintf (fp, "\e[5m")
    #define TCS_REVERSE(fp)         fprintf (fp, "\e[7m")
    #define TCS_HIDE(fp)            fprintf (fp, "\e[8m")
    #define TCS_CLEAR(fp)           fprintf (fp, "\e[2J")
    #define TCS_CLRLINE(fp)         fprintf (fp, "\e[1K\r")
#else
    #define TCS_NONE(fp)
    #define TCS_BLACK(fp)
    #define TCS_BOLD_BLACK(fp)
    #define TCS_RED(fp)
    #define TCS_BOLD_RED(fp)
    #define TCS_GREEN(fp)
    #define TCS_BOLD_GREEN(fp)
    #define TCS_BROWN(fp)
    #define TCS_YELLOW(fp)
    #define TCS_BLUE(fp)
    #define TCS_BOLD_BLUE(fp)
    #define TCS_PURPLE(fp)
    #define TCS_BOLD_PURPLE(fp)
    #define TCS_CYAN(fp)
    #define TCS_BOLD_CYAN(fp)
    #define TCS_GRAY(fp)
    #define TCS_WHITE(fp)
    #define TCS_BOLD(fp)
    #define TCS_UNDERLINE(fp)
    #define TCS_BLINK(fp)
    #define TCS_REVERSE(fp)
    #define TCS_HIDE(fp)
    #define TCS_CLEAR(fp)
    #define TCS_CLRLINE(fp)
#endif

#define _MG_PRINTF(fmt, ...)                    \
    do {                                        \
        TCS_GREEN (stdout);                     \
        fprintf (stdout, fmt, ##__VA_ARGS__);   \
        TCS_NONE (stdout);                      \
    } while (0)

#define _WRN_PRINTF(fmt, ...)                   \
    do {                                        \
        TCS_BROWN (stderr);                     \
        fprintf (stderr, "%s: ", __FUNCTION__); \
        fprintf (stderr, fmt, ##__VA_ARGS__);   \
        TCS_NONE (stderr);                      \
    } while (0)

#define _ERR_PRINTF(fmt, ...)                   \
    do {                                        \
        TCS_RED (stderr);                       \
        fprintf (stderr, fmt, ##__VA_ARGS__);   \
        TCS_NONE (stderr);                      \
    } while (0)

#if defined(_DEBUG)
#   define _DBG_PRINTF(fmt, ...)                \
    do {                                        \
        TCS_YELLOW (stderr);                    \
        fprintf (stderr, "%s: ", __FUNCTION__); \
        fprintf (stderr, fmt, ##__VA_ARGS__);   \
        TCS_NONE (stderr);                      \
    } while (0)
# else
#   define _DBG_PRINTF(fmt, ...) do { } while (0)
#endif

#ifdef _MGRM_THREADS

#ifdef _MGUSE_OWN_PTHREAD

#define MAIN_PTH_MIN_STACK_SIZE (1024)
#define MAIN_PTH_DEF_STACK_SIZE (1024*4)

/**
 * \fn int start_minigui_pthread (int (* pth_entry) (int argc,
                const char* argv []),
                int argc, const char* argv[],
                char* stack_base, unsigned int stack_size)
 * \brief Initialize MiniGUI's own POSIX Threads system and start the main thread.
 *
 * This function initializes MiniGUI's own POSIX Threads system.
 * If pth_entry is not NULL, this function will create a detached pthread
 * as the main pthread, and call pth_entry with the arguments passed to
 * this function.
 *
 * Note for pSOS, this function will ignore the stack_base, cause pSOS kernel
 * will allocate the stack for the main thread. Therefore, please pass
 * stack_base to NULL for pSOS.
 *
 * \param pth_entry The entry function of the main thread.
 * \param argc The first argument will be passed to pth_entry.
 * \param argv The seconde argument will be passed to pth_entry.
 * \param stack_base The base address of the stack for the main thread.
 * \param stack_size The size of the stack for the main thread in bytes.
 *
 * \return Zero on success, non-zero on error.
 */
int start_minigui_pthread (int (* pth_entry) (int argc, const char* argv []),
                int argc, const char* argv[],
                char* stack_base, unsigned int stack_size);

#ifndef ESRCH
#   define ESRCH      3
#endif

#ifndef EAGAIN
#   define EAGAIN     11
#endif

#ifndef ENOMEM
#   define ENOMEM     12
#endif

#ifndef EBUSY
#   define EBUSY      16
#endif

#ifndef EINVAL
#   define EINVAL     22
#endif

#ifndef EDEADLK
#   define EDEADLK    35
#endif

#ifndef ENOSYS
#   define ENOSYS     38
#endif

#ifndef ENOTSUP
#   define ENOTSUP    95
#endif

#ifdef __VXWORKS__

#define pthread_create vxworks_pthread_create
#define pthread_self   vxworks_pthread_self
#define pthread_equal  vxworks_pthread_equal
#define pthread_exit   vxworks_pthread_exit
#define pthread_join   vxworks_pthread_join
#define pthread_detach vxworks_pthread_detach
#define pthread_cancel vxworks_pthread_cancel
#define pthread_once   vxworks_pthread_once
#define pthread_key_create vxworks_pthread_key_create
#define pthread_key_delete vxworks_pthread_key_delete
#define pthread_setspecific vxworks_pthread_setspecific
#define pthread_getspecific vxworks_pthread_getspecific
#define pthread_setcancelstate vxworks_pthread_setcancelstate
#define pthread_setcanceltype vxworks_pthread_setcanceltype
#define pthread_testcancel vxworks_pthread_testcancel

#define pthread_attr_init    vxworks_pthread_attr_init
#define pthread_attr_destroy vxworks_pthread_attr_destroy
#define pthread_attr_setdetachstate vxworks_pthread_attr_setdetachstate
#define pthread_attr_getdetachstate vxworks_pthread_attr_getdetachstate
#define pthread_attr_setscope vxworks_pthread_attr_setscope
#define pthread_attr_getscope vxworks_pthread_attr_getscope
#define pthread_attr_setinheritsched vxworks_pthread_attr_setinheritsched
#define pthread_attr_getinheritsched vxworks_pthread_attr_getinheritsched
#define pthread_attr_setschedpolicy vxworks_pthread_attr_setschedpolicy
#define pthread_attr_getschedpolicy vxworks_pthread_attr_getschedpolicy
#define pthread_attr_setschedparam vxworks_pthread_attr_setschedparam
#define pthread_attr_getschedparam vxworks_pthread_attr_getschedparam
#define pthread_attr_setstackaddr vxworks_pthread_attr_setstackaddr
#define pthread_attr_getstackaddr vxworks_pthread_attr_getstackaddr
#define pthread_attr_setstacksize vxworks_pthread_attr_setstacksize
#define pthread_attr_getstacksize vxworks_pthread_attr_getstacksize
#define pthread_setschedparam vxworks_pthread_setschedparam
#define pthread_getschedparam vxworks_pthread_getschedparam

#define pthread_mutex_init  vxworks_pthread_mutex_init
#define pthread_mutex_destroy vxworks_pthread_mutex_destroy
#define pthread_mutex_lock  vxworks_pthread_mutex_lock
#define pthread_mutex_unlock vxworks_pthread_mutex_unlock
#define pthread_mutex_trylock vxworks_pthread_mutex_trylock
#define pthread_mutexattr_init vxworks_pthread_mutexattr_init
#define pthread_mutexattr_destroy vxworks_pthread_mutexattr_destroy
#define pthread_mutexattr_setpriorityinherit vxworks_pthread_mutexattr_setpriorityinherit
#define pthread_mutexattr_getpriorityinherit vxworks_pthread_mutexattr_getpriorityinherit

#ifdef _MGUSE_OWN_SEMAPHORE

#define sem_t vxworks_sem_t

#define sem_init vxworks_sem_init
#define sem_destroy vxworks_sem_destroy
#define sem_wait vxworks_sem_wait
#define sem_trywait vxworks_sem_trywait
#define sem_post vxworks_sem_post
#define sem_getvalue vxworks_sem_getvalue

#endif /* _MGUSE_OWN_SEMAPHORE */

#endif /* __VXWORKS__ */

#ifdef __MINIGUI_LIB__

#ifdef __UCOSII__
#   include "ucos2_pthread.h"
#   include "ucos2_semaphore.h"
#elif defined (__THREADX__)
#   include "threadx_pthread.h"
#   include "threadx_semaphore.h"
#elif defined (__NUCLEUS__)
#   include "nucleus_pthread.h"
#   include "nucleus_semaphore.h"
#elif defined (__VXWORKS__)
#   include "vxworks_pthread.h"
#   ifdef _MGUSE_OWN_SEMAPHORE
#       include "vxworks_semaphore.h"
#   else
#       include "semaphore.h"
#   endif
#elif defined (__OSE__)
#   include "pthread.h"
#   include "ose_semaphore.h"
#elif defined (__PSOS__)
#   include "psos_pthread.h"
#   include "psos_semaphore.h"
#elif defined (WIN32)
#   include "win32_pthread.h"
#   include "win32_semaphore.h"
#else
#   error No own pthread implementation for this OS!
#endif

#else

#   include <pthread.h>
#   include <semaphore.h>

#endif /* __MINIGUI_LIB__ */

#else /* _MGUSE_OWN_PTHREAD */

#include <pthread.h>
#include <semaphore.h>

#endif /* !_MGUSE_OWN_PTHREAD */

#ifdef __DARWIN__

#define _USE_SEM_ON_SYSVIPC     1

#endif

#if defined (_USE_MUTEX_ON_SYSVIPC) || defined (_USE_SEM_ON_SYSVIPC)

int _sysvipc_mutex_sem_init (void);
int _sysvipc_mutex_sem_term (void);

#endif

#ifdef _USE_MUTEX_ON_SYSVIPC

#define pthread_mutex_t _sysvipc_pthread_mutex_t
#define pthread_mutexattr_t _sysvipc_pthread_mutexattr_t

#define pthread_mutexattr_init _sysvipc_pthread_mutexattr_init
#define pthread_mutexattr_destroy _sysvipc_pthread_mutexattr_destroy
#define pthread_mutexattr_settype _sysvipc_pthread_mutexattr_settype
#define pthread_mutexattr_gettype _sysvipc_pthread_mutexattr_gettype

#define pthread_init _sysvipc_pthread_init
#define pthread_mutex_init _sysvipc_pthread_mutex_init
#define pthread_mutex_destroy _sysvipc_pthread_mutex_destroy
#define pthread_mutex_lock _sysvipc_pthread_mutex_lock
#define pthread_mutex_trylock _sysvipc_pthread_mutex_trylock
#define pthread_mutex_unlock _sysvipc_pthread_mutex_unlock

typedef struct
{
    int kind;
    int semid;
    int sem_num;
    int locked_times;
} pthread_mutex_t;

#define SYSVIPC_PTHREAD_MUTEX_FAST_NP         0
#define SYSVIPC_PTHREAD_MUTEX_RECURSIVE_NP    1
#define SYSVIPC_PTHREAD_MUTEX_ERRORCHECK_NP   2

typedef struct
{
    int kind;
} pthread_mutexattr_t;

int pthread_mutexattr_init (pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy (pthread_mutexattr_t *attr);
int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_gettype (const pthread_mutexattr_t *attr, int* type);

int pthread_mutex_init (pthread_mutex_t *mutex,
                                const pthread_mutexattr_t *mutex_attr);

int pthread_mutex_destroy (pthread_mutex_t *mutex);

int pthread_mutex_lock (pthread_mutex_t *mutex);
int pthread_mutex_trylock (pthread_mutex_t *mutex);
int pthread_mutex_unlock (pthread_mutex_t *mutex);

#endif /* _USE_MUTEX_ON_SYSVIPC */

#ifdef _USE_SEM_ON_SYSVIPC

#define sem_t _sysvipc_sem_t

#define sem_init _sysvipc_sem_init
#define sem_destroy _sysvipc_sem_destroy
#define sem_wait _sysvipc_sem_wait
#define sem_trywait _sysvipc_sem_trywait
#define sem_post _sysvipc_sem_post
#define sem_getvalue _sysvipc_sem_getvalue

#define SYSVIPC_SEM_VALUE_MAX   USHRT_MAX

/*-----------------------------------------------------------------------------
** Semaphore object definition
*/

typedef struct
{
    int semid;
    int sem_num;
} sem_t;

int sem_init (sem_t *sem, int pshared, unsigned int value);
int sem_destroy (sem_t *sem);
int sem_wait (sem_t *sem);
int sem_trywait (sem_t *sem);
int sem_post (sem_t *sem);
int sem_getvalue (sem_t *sem, int *sval);

#endif /* _USE_SEM_ON_SYSVIPC */

#endif /* _MGRM_THREADS */

#ifdef __cplusplus
}  /* end of extern "C" */
#endif

#endif /* _MGUI_COMMON_H */

