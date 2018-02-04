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
/*        MATERIALS, which is provided “AS IS”, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File   linuxwrapper.h                                                        
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/
#ifndef __H_LINUXWRAPPER
#define __H_LINUXWRAPPER

#include "stddefs.h"

/* ***************** */
/* partial limits.h  */
/* ***************** */

/* Limits constant values */
#if !defined(CHAR_BIT)
#define CHAR_BIT 8
#endif
#if !defined(SCHAR_MAX)
#define SCHAR_MAX 127
#endif
#if !defined(UCHAR_MAX)
#define UCHAR_MAX 255
#endif

#ifdef __SIGNED_CHAR__
#if !defined(CHAR_MIN)
#define CHAR_MIN (-128)
#endif
#if !defined(CHAR_MAX)
#define CHAR_MAX 127
#endif

#else   /* __SIGNED_CHAR__ */
#if !defined(CHAR_MIN)
#define CHAR_MIN 0
#endif
#if !defined(CHAR_MAX)
#define CHAR_MAX 255
#endif

#endif  /* __SIGNED_CHAR__ */

#if !defined(MB_LEN_MAX)
#define MB_LEN_MAX 1
#endif

#if !defined(SHRT_MIN)
#define SHRT_MIN  (~0x7fff)
#endif
#if !defined(SHRT_MAX)
#define SHRT_MAX  0x7fff
#endif
#if !defined(USHRT_MAX)
#define USHRT_MAX 65535U
#endif




/* *************** */
/* Partial sttbx.h */
/* *************** */
#ifndef STTBX_INPUT
#define STTBX_INPUT
#endif

#if defined(STAPIREF_COMPAT)
#else
/* STSDK - FAE/STAPIREF_COMPAT - START - Reroute traces to FAE trace module */
#if defined(STSDK)&&!defined(MODULE)
extern ST_ErrorCode_t TRACE_Print (const char *format,...);
extern ST_ErrorCode_t TRACE_Report(U32 ReportLevel,const char *format,...);
#endif
/* STSDK - FAE/STAPIREF_COMPAT - STOP  - Reroute traces to FAE trace module */
#endif /* STAPIREF_COMPAT */
#ifdef STTBX_REPORT
#define STTBX_Report(x)     STTBX_Report_arg x

#ifdef MODULE
#define STTBX_REPORT_NL		printk("\n");
#else
#if defined(STAPIREF_COMPAT)
#define STTBX_REPORT_NL		printf("\n");
#else
/* STSDK - FAE/STAPIREF_COMPAT - START - Reroute traces to FAE trace module */
#if defined(STSDK)
#define STTBX_REPORT_NL		TRACE_Print("\n");
#else
#define STTBX_REPORT_NL		printf("\n");
#endif
/* STSDK - FAE/STAPIREF_COMPAT - STOP  - Reroute traces to FAE trace module */
#endif /* STAPIREF_COMPAT */
#endif

#else
#define STTBX_Report(x)
#define STTBX_REPORT_NL
#endif  /* STTBX_REPORT */

#ifdef STTBX_PRINT
#ifdef MODULE
#define STTBX_Print(x)		printk x
#else
#if defined(STAPIREF_COMPAT)
#define STTBX_Print(x)		printf x
#else
/* STSDK - FAE/STAPIREF_COMPAT - START - Reroute traces to FAE trace module */
#if defined(STSDK)
#define STTBX_Print(x)		TRACE_Print x
#else
#define STTBX_Print(x)		printf x
#endif
/* STSDK - FAE/STAPIREF_COMPAT - STOP  - Reroute traces to FAE trace module */
#endif /* STAPIREF_COMPAT */
#endif

#else
#define STTBX_Print(x)
#endif  /* STTBX_PRINT */



/* ************** */

/* C++ support */
#if defined __cplusplus
    extern "C" {
#endif


/* *************** */

/* Input functions */
#if defined(STAPIREF_COMPAT)
#if !defined(MODULE)
void STTBX_InputChar(char * result);
BOOL STTBX_InputPollChar(char* buffer);
int STTBX_InputStr( char* answer, int size );
#endif  /* !MODULE */
#else
/* STFAE - No STTBX in STOS,it should be done in STTBX driver !!! */
#endif /* STAPIREF_COMPAT */

/* Defines the levels of the report (type of message) */
typedef enum STTBX_ReportLevel_e
{                                   /* Use the given level to indicate: */
    STTBX_REPORT_LEVEL_FATAL = 0,   /* Imminent non recoverable system failure */
    STTBX_REPORT_LEVEL_ERROR,       /* Serious error, though recoverable */
    STTBX_REPORT_LEVEL_WARNING,     /* Warnings of unusual occurences */
    STTBX_REPORT_LEVEL_ENTER_LEAVE_FN, /* Entrance or exit of a function, and parameter display */
    STTBX_REPORT_LEVEL_INFO,        /* Status and other information - normal though */
    STTBX_REPORT_LEVEL_USER1,       /* User specific */
    STTBX_REPORT_LEVEL_USER2,       /* User specific */

    /* Keep as last one (Internal use only) */
    STTBX_NB_OF_REPORT_LEVEL        /* Last element has a value corresponding to the number of elements by default ! */
} STTBX_ReportLevel_t;

void STTBX_Report_arg(const STTBX_ReportLevel_t ReportLevel, const char *const Format_p, ...);

#ifndef STTBX_REPORT_LEVEL_MAX
#define STTBX_REPORT_LEVEL_MAX      STTBX_REPORT_LEVEL_ENTER_LEAVE_FN
#endif



/* *************** */
/*
#ifndef PARTITION_T
#define PARTITION_T
typedef int partition_t;
#endif
*/

#ifndef ST_PARTITION_T
#define ST_PARTITION_T
typedef int ST_Partition_t;
#endif

typedef BOOL boolean;
#if !defined false
#define false	FALSE
#endif
#if !defined true
#define true	TRUE
#endif

#define MILLISECOND_TO_TICKS(v)          (((unsigned int) STLINUX_GetClocksPerSecond()*((unsigned int)(v)))/1000)

/* STSYS functions */
#if defined(STAPIREF_COMPAT)
#ifdef MODULE
U16 STSYS_ReadRegDev16BE(U32 Address_p);
U32 STSYS_ReadRegDev24BE(U32 Address_p);
U32 STSYS_ReadRegDev32BE(U32 Address_p);

#define STSYS_ReadRegDev8(Address_p)        readb(Address_p)
#define STSYS_ReadRegMem8(Address_p)        readb(Address_p)
#define STSYS_ReadRegDev16LE(address)       readw(address)
#define STSYS_ReadRegMem16LE(Address_p)     readw(Address_p)
#define STSYS_ReadRegMem16BE(Address_p)     STSYS_ReadRegDev16BE(Address_p)
#define STSYS_ReadRegDev24LE(Address_p)     ((U32) ((readb((U32)(Address_p) + 2)) << 16 | (readw(Address_p))))
#define STSYS_ReadRegMem24LE(Address_p)     STSYS_ReadRegDev24LE(Address_p)
#define STSYS_ReadRegMem24BE(Address_p)     STSYS_ReadRegDev24BE(Address_p)
#define STSYS_ReadRegDev32LE(address)       readl(address)
#define STSYS_ReadRegMem32LE(address)       readl(address)
#define STSYS_ReadRegMem32BE(Address_p)     STSYS_ReadRegDev32BE(Address_p)

#define STSYS_WriteRegDev8(Address_p, value)        writeb((u8)value, (void*)Address_p)
#define STSYS_WriteRegMem8(Address, value)          writeb((u8)value, (u32)Address)
#define STSYS_WriteRegDev16LE(Address_p, value)     writew((u16)value, (void*)Address_p)
#define STSYS_WriteRegMem16LE(Address_p, value)     writew((u16)value, (void*)Address_p)
#define STSYS_WriteRegDev32LE(Address_p, value)     writel((u32)value, (void*)Address_p)
#define STSYS_WriteRegMem32LE(Address, value)       writel((u32)value, (u32)Address)

#define STSYS_WriteRegDev16BE(Address_p, Value)                     \
    do {   U16 Reg16BEToLE =  (U16) ((((Value) & 0xFF00) >> 8) |    \
                                     (((Value) & 0x00FF) << 8));    \
           writew(Reg16BEToLE, (void*)Address_p);                   \
    } while (0)

#define STSYS_WriteRegDev24BE(Address_p, Value)                     \
    do {   U16 Reg24BEToLE = (U16) ((((Value) & 0xFF0000) >> 16) |  \
                                    (((Value) & 0x00FF00)      ));  \
            writew(Reg24BEToLE, (void*)Address_p);                  \
            writeb((U8)(Value), (void*)((U32)(Address_p) + 2));     \
    } while (0)

#define STSYS_WriteRegDev24LE(Address_p, Value)                                             \
    do {    writew((U16)(Value), (void*)Address_p);                                         \
            writeb((U8)(((Value) & 0xFF0000) >> 16), (void*)((U32)(Address_p) + 2));        \
    } while (0)

#define STSYS_WriteRegDev32BE(Address_p, Value)                         \
    do {    U32 Reg32BEToLE = (U32) ((((Value) & 0xFF000000) >> 24) |   \
                                     (((Value) & 0x00FF0000) >>  8) |   \
                                     (((Value) & 0x0000FF00) <<  8) |   \
                                     (((Value) & 0x000000FF) << 24));   \
            writel(Reg32BEToLE, (void*)Address_p);                      \
    } while (0)

#define STSYS_WriteRegMem16BE(Address_p, Value)     STSYS_WriteRegDev16BE(Address_p, Value)
#define STSYS_WriteRegMem24BE(Address_p, Value)     STSYS_WriteRegDev24BE(Address_p, Value)
#define STSYS_WriteRegMem24LE(Address_p, Value)     STSYS_WriteRegDev24LE(Address_p, Value)
#define STSYS_WriteRegMem32BE(Address_p, Value)     STSYS_WriteRegDev32BE(Address_p, Value)


#define STSYS_SetRegMask32LE(address, mask) \
    STSYS_WriteRegDev32LE(address, STSYS_ReadRegDev32LE(address) | (mask) )

#define STSYS_ClearRegMask32LE(address, mask) \
    STSYS_WriteRegDev32LE(address, STSYS_ReadRegDev32LE(address) & ~(mask) )

#define STSYS_memcpy_fromio(dest, source, num)    \
do {   \
    memcpy_fromio((dest), (u32)(source), (num));    \
    rmb();                               \
} while(0)

#define STSYS_ReadRegMemUncached8(Address_p)             STSYS_ReadRegDev8((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2))
#define STSYS_WriteRegMemUncached8(Address_p, Value)     STSYS_WriteRegDev8((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2), (Value))
#define STSYS_ReadRegMemUncached16LE(Address_p)          STSYS_ReadRegDev16LE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2))
#define STSYS_WriteRegMemUncached16LE(Address_p, Value)  STSYS_WriteRegDev16LE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2), (Value))
#define STSYS_ReadRegMemUncached16BE(Address_p)          STSYS_ReadRegDev16BE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2))
#define STSYS_WriteRegMemUncached16BE(Address_p, Value)  STSYS_WriteRegDev16BE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2), (Value))
#define STSYS_ReadRegMemUncached32LE(Address_p)          STSYS_ReadRegDev32LE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2))
#define STSYS_WriteRegMemUncached32LE(Address_p, Value)  STSYS_WriteRegDev32LE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2), (Value))
#define STSYS_ReadRegMemUncached32BE(Address_p)          STSYS_ReadRegDev32BE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2))
#define STSYS_WriteRegMemUncached32BE(Address_p, Value)  STSYS_WriteRegDev32BE((void *)(((U32)(Address_p) & MASK_STBUS_FROM_ST40) | REGION2), (Value))

#endif
#else
/* STFAE - No STSYS in STOS,it should be done in STSYS driver !!! */
#endif /* STAPIREF_COMPAT */

/* C++ support */
#if defined __cplusplus
}
#endif


#endif  /* __H_LINUXWRAPPER */


/* End of File */


