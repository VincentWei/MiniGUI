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
/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided ?AS IS?, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File   stddefs.h                                                             
 @brief Contains a number of generic type declarations and defines.                                                                         
                                                                               
                                                                               
                                                                               
*/
#ifndef __STDDEFS_H
#define __STDDEFS_H


/* Includes ---------------------------------------------------------------- */
#if defined(ST_OS21)
#include "stlite.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types ---------------------------------------------------------- */

/* Common unsigned types */
#ifndef DEFINED_U8
#define DEFINED_U8
typedef unsigned char  U8;
#endif

#ifndef DEFINED_U16
#define DEFINED_U16
typedef unsigned short U16;
#endif

#ifndef DEFINED_U32
#define DEFINED_U32
typedef unsigned int   U32;
#endif


/* Common signed types */
#ifndef DEFINED_S8
#define DEFINED_S8
typedef signed char  S8;
#endif

#ifndef DEFINED_S16
#define DEFINED_S16
typedef signed short S16;
#endif

#ifndef DEFINED_S32
#define DEFINED_S32
typedef signed int   S32;
#endif

#if defined(STAPIREF_COMPAT)
#else
/* STFAE - START - Include linux common by default everywhere */
/* STFAE - Please don't move this upper otherwise it creates errors in compilation */
#if defined(ST_OSLINUX)
#include "linuxcommon.h"
#endif
/* STFAE - STOP  - Include linux common by default everywhere */
#endif /* STAPIREF_COMPAT */
#include "stdevice.h"

#if defined(ST_OS21) || defined(ST_OSLINUX)
#if !defined(ST20_COMPATIBLE_64BIT_TYPES)

#ifndef DEFINED_U64
#define DEFINED_U64
typedef unsigned long long U64;
#endif

#ifndef DEFINED_S64
#define DEFINED_S64
typedef signed long long S64;
#endif



/*Value=A+B, where A & B is U64 type*/
#define I64_Add(A,B,Value)              (Value=A+B)

/*Value=A+B, where A is U64 type & B is 32-bit atmost*/
#define I64_AddLit(A,B,Value)           (Value=A+B)

/*A==B, A & B are U64 type*/
#define I64_IsEqual(A,B)                (A==B)

#define I64_GetValue(Value,Lower,Upper) ((Lower) = ((Value)&0xFFFFFFFF), (Upper) = (((Value)>>32)&0xFFFFFFFF))
#define I64_GetMSW(Value)               ((U32)(((Value)>>32)&0xFFFFFFFF))
#define I64_GetLSW(Value)               ((U32)((Value)&0xFFFFFFFF))

/*A>=B, A & B are U64 type*/
#define I64_IsGreaterOrEqual(A,B)       (A>=B)

/*A>B, A & B are U64 type*/
#define I64_IsGreaterThan(A,B)          (A>B)

/*A<B, A & B are U64 type*/
#define I64_IsLessThan(A,B)             (A<B)

/*A<=B, A & B are U64 type*/
#define I64_IsLessOrEqual(A,B)          (A<=B)

#define I64_IsNegative(A)               (((A) & ( (U64) 0x8000000000000000LL) )==((U64) 0x8000000000000000LL))

/*A==0, A is U64 type*/
#define I64_IsZero(A)                   (A==0)

/*A!=B, A & B are U64 type*/
#define I64_AreNotEqual(A,B)            (A!=B)

#define I64_SetValue(Lower,Upper,Value) ((Value) = ( ( ( (U64) (Upper) )<<32) +(Lower)) )
#define I64_SetMSW(Upper,Value)         ((Value) = ( ( ( (U64) (Upper) )<<32) +(((U64)Value)&0xFFFFFFFF)) )
#define I64_SetLSW(Lower,Value)         ((Value) = ( ( ( (U64) (Lower) ))     +(((U64)Value)&((U64)0xFFFFFFFF00000000LL))) )

/*Value=A-B, where A & B are U64 type*/
#define I64_Sub(A,B,Value)              (Value=A-B)

/*Value=A-B, where A is U64 type & B is 32-bit atmost*/
#define I64_SubLit(A,B,Value)           (Value=A-B)

/*Value=A/B, where A is U64 type & B is 32-bit atmost*/
#define I64_DivLit(A,B,Value)           (Value=A/((U32)B))

/*Value=A%B, where A is U64 type & B is 32-bit atmost*/
#define I64_ModLit(A,B,Value)           (Value=A%B)

/*Value=A*B, where A & B are U64 type*/
#define I64_Mul(A,B,Value)              (Value=A*B)

/*Value=A*B, where A is U64 type & B is 32-bit atmost*/
#define I64_MulLit(A,B,Value)           (Value=A*B)

/*Value=Value<<Shift, where Value is U64 type*/
#define I64_ShiftLeft(Shift,Value)      (Value=Value<<Shift)

/*Value=Value>>Shift, where Value is U64 type*/
#define I64_ShiftRight(Shift,Value)     (Value=Value>>Shift)

#else /*#if !defined(ST20_COMPATIBLE_64BIT_TYPES)*/

#ifndef DEFINED_U64
#define DEFINED_U64
typedef struct U64_s
{
    unsigned int LSW;
    unsigned int MSW;
}
U64;
#endif

/*The following S64 type is not truly a Signed number. You would have to use additional
  logic to use it alongwith the macro I64_IsNegative(A) provided below*/
#ifndef DEFINED_S64
#define DEFINED_S64
typedef U64 S64;
#endif

/*Value=A+B, where A & B is U64 type*/
#define I64_Add(A,B,Value)      { register long long T1,T2,Val; \
                                T1  = (long long)(A).MSW << 32 | (A).LSW; \
                                T2  = (long long)(B).MSW << 32 | (B).LSW; \
                                Val = T1 + T2; \
                                (Value).MSW = Val >> 32; \
                                (Value).LSW = (U32)Val; \
                                }

/*Value=A+B, where A is U64 type & B is 32-bit atmost*/
#define I64_AddLit(A,B,Value)   { register long long T1,Val; \
                                T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                Val=T1+(B); \
                                (Value).MSW = Val >> 32; \
                                (Value).LSW = (U32)Val; \
                                }

/*A==B, A & B are U64 type*/
#define I64_IsEqual(A,B)                (((A).LSW == (B).LSW) && ((A).MSW == (B).MSW))

#define I64_GetValue(Value,Lower,Upper) ((Lower) = (Value).LSW, (Upper) = (Value).MSW)
#define I64_GetMSW(Value)               ((Value).MSW)
#define I64_GetLSW(Value)               ((Value).LSW)

/*A>=B, A & B are U64 type*/
#define I64_IsGreaterOrEqual(A,B)       ( ((A).MSW >  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW >= (B).LSW)))

/*A>B, A & B are U64 type*/
#define I64_IsGreaterThan(A,B)          ( ((A).MSW >  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW > (B).LSW)))

/*A<B, A & B are U64 type*/
#define I64_IsLessThan(A,B)             ( ((A).MSW <  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW < (B).LSW)))

/*A<=B, A & B are U64 type*/
#define I64_IsLessOrEqual(A,B)          ( ((A).MSW <  (B).MSW) || \
                                         (((A).MSW == (B).MSW) && ((A).LSW <= (B).LSW)))

#define I64_IsNegative(A)               ((((A).MSW & 0X80000000))==0x80000000)

/*A==0, A is U64 type*/
#define I64_IsZero(A)                   (((A).LSW == 0) && ((A).MSW == 0))

/*A!=B, A & B are U64 type*/
#define I64_AreNotEqual(A,B)            (((A).LSW != (B).LSW) || ((A).MSW != (B).MSW))

#define I64_SetValue(Lower,Upper,Value) ((Value).LSW = (Lower), (Value).MSW = (Upper))
#define I64_SetMSW(Upper,Value)         ((Value).MSW = (Upper))
#define I64_SetLSW(Lower,Value)         ((Value).LSW = (Lower))

/*Value=A-B, where A & B are U64 type*/
#define I64_Sub(A,B,Value)              ((Value).MSW  = (A).MSW - (B).MSW - (((A).LSW < (B).LSW)?1:0), \
                                         (Value).LSW  = (A).LSW - (B).LSW)

/*Value=A-B, where A is U64 type & B is 32-bit atmost*/
#define I64_SubLit(A,B,Value)           ((Value).MSW  = (A).MSW - (((A).LSW < (B))?1:0), \
                                         (Value).LSW  = (A).LSW - (B))

/*Value=A/B, where A is U64 type & B is 32-bit atmost*/
#define I64_DivLit(A,B,Value)           { register long long T1, Val;\
                                        T1 = (long long)(A).MSW << 32 | (A).LSW;\
                                        Val = T1/(B);\
                                        (Value).MSW = Val >> 32;\
                                        (Value).LSW = (U32)Val;\
                                        }

/*Value=A%B, where A is U64 type & B is 32-bit atmost*/
#define I64_ModLit(A,B,Value)           { register long long T1, Val;\
                                        T1 = (long long)(A).MSW << 32 | (A).LSW;\
                                        Val=T1%(B);\
                                        (Value).MSW = Val >> 32;\
                                        (Value).LSW = (U32)Val;\
                                        }

/*Value=A*B, where A & B are U64 type*/
#define I64_Mul(A,B,Value)              { register long long T1, T2, Val; \
                                        T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                        T2 = (long long)(B).MSW << 32 | (B).LSW; \
                                        Val=T1*T2; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=A*B, where A is U64 type & B is 32-bit atmost*/
#define I64_MulLit(A,B,Value)           { register long long T1,Val; \
                                        T1 = (long long)(A).MSW << 32 | (A).LSW; \
                                        Val=T1*(B); \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=Value<<Shift, where Value is U64 type*/
#define I64_ShiftLeft(Shift,Value)      { register long long T1, T2, Val; \
                                        T1 = (long long)(Value).MSW << 32 | (Value).LSW; \
                                        Val=T1 << Shift; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

/*Value=Value>>Shift, where Value is U64 type*/
#define I64_ShiftRight(Shift,Value)     { register long long T1, T2, Val; \
                                        T1 = (long long)(Value).MSW << 32 | (Value).LSW; \
                                        Val=T1 >> Shift; \
                                        (Value).MSW = Val >> 32; \
                                        (Value).LSW = (U32)Val; \
                                        }

#endif /*#if !defined(ST20_COMPATIBLE_64BIT_TYPES)*/
#endif /*#if defined(ST_OS21) || defined(ST_OSLINUX)*/

/* Boolean type (values should be among TRUE and FALSE constants only) */
#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif


/* General purpose string type */
typedef char* ST_String_t;

/* Function return error code */
typedef U32 ST_ErrorCode_t;

/* Revision structure */
typedef const char * ST_Revision_t;

/* This is the driver identification number */
typedef U16 ST_DriverId_t;

/* This is a CPU identifier */
typedef U8 ST_CPU_t;
#define NULL_CPU ((ST_CPU_t) -1)

#define INCLUDE_GetRevision() ((ST_Revision_t) "INCLUDE-REL_2.1.24")

/* Device name type */
#define ST_MAX_DEVICE_NAME 16  /* 15 characters plus '\0' */
typedef char ST_DeviceName_t[ST_MAX_DEVICE_NAME];

/* Generic partition type */
#ifdef ST_OSLINUX
#ifndef PARTITION_T
#define PARTITION_T
typedef int partition_t;
#endif
#ifndef ST_PARTITION_T
#define ST_PARTITION_T
typedef partition_t ST_Partition_t;
#endif
#endif /*#ifdef ST_OSLINUX*/

#if defined(ST_OS21)
typedef partition_t ST_Partition_t;
#endif


/* Exported Constants ------------------------------------------------------ */

/* BOOL type constant values */
#ifndef TRUE
    #define TRUE (1 == 1)
#endif
#ifndef FALSE
    #define FALSE (!TRUE)
#endif

/* Common driver error constants */
#define ST_DRIVER_ID   0
#define ST_DRIVER_BASE (ST_DRIVER_ID << 16)
enum
{
    ST_NO_ERROR = ST_DRIVER_BASE,       /* 0 */
    ST_ERROR_BAD_PARAMETER,             /* 1 Bad parameter passed       */
    ST_ERROR_NO_MEMORY,                 /* 2 Memory allocation failed   */
    ST_ERROR_UNKNOWN_DEVICE,            /* 3 Unknown device name        */
    ST_ERROR_ALREADY_INITIALIZED,       /* 4 Device already initialized */
    ST_ERROR_NO_FREE_HANDLES,           /* 5 Cannot open device again   */
    ST_ERROR_OPEN_HANDLE,               /* 6 At least one open handle   */
    ST_ERROR_INVALID_HANDLE,            /* 7 Handle is not valid        */
    ST_ERROR_FEATURE_NOT_SUPPORTED,     /* 8 Feature unavailable        */
    ST_ERROR_INTERRUPT_INSTALL,         /* 9 Interrupt install failed   */
    ST_ERROR_INTERRUPT_UNINSTALL,       /* 10 Interrupt uninstall failed */
    ST_ERROR_TIMEOUT,                   /* 11 Timeout occured            */
    ST_ERROR_DEVICE_BUSY,               /* 12 Device is currently busy   */
    ST_ERROR_SUSPENDED                  /* 13 Device is in D1 or D2 state */
};

/* Exported Variables ------------------------------------------------------ */

/* Exported Macros --------------------------------------------------------- */
#define UNUSED_PARAMETER(x)    (void)(x)

/* Exported Functions ------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __STDDEFS_H */

/* End of stddefs.h  ------------------------------------------------------- */


