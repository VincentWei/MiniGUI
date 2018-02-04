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
 @File   stsys.h
 @brief



*/
#ifndef __STSYS_H
#define __STSYS_H

/*
--------------------------------------------------------------------------------
Get revision of STSYS API
--------------------------------------------------------------------------------
*/
#define STSYS_GetRevision() ((ST_Revision_t) "STSYS-REL_2.0.6")

/* STFAE - No STSYS macros on linux */
#if !defined ST_OSLINUX|| (!defined(MODULE)&&defined(STAPIREF_COMPAT))
/* Under Linux and kernel space, the STSYS functions are defined into STOS vob */

/* Includes ----------------------------------------------------------------- */

#include "stddefs.h"

#if defined(ST200_OS21)
#include <os21/st200/mmap.h>
#endif /* ST200_OS21 */

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types ----------------------------------------------------------- */

#if defined(ARCHITECTURE_ST20) && ! defined(STSYS_NO_PRAGMA)
#pragma ST_device(STSYS_DU32)
#pragma ST_device(STSYS_DU16)
#pragma ST_device(STSYS_DU8)
#endif /*ifdef ARCHITECTURE_ST20*/

typedef volatile U32 STSYS_DU32;
typedef volatile U16 STSYS_DU16;
typedef volatile U8  STSYS_DU8;


typedef volatile U32 STSYS_MU32;
typedef volatile U16 STSYS_MU16;
typedef volatile U8  STSYS_MU8;


/* Exported Constants ------------------------------------------------------- */

/* Exported Variables ------------------------------------------------------- */
#if defined(ST200_OS21)
/* Variable needed to remember the offset to add to a physical address
 * to have a CPU uncached address to the memory. This is more efficient
 * than calling mmap_translate_uncached every time we make an uncached
 * access */
extern S32 STSYS_ST200UncachedOffset;
#endif /* defined(ST200_OS21) */

/* Exported Macros ---------------------------------------------------------- */


/* ########################################################################### */
/*                     DEVICE ACCESS                                           */
/* ########################################################################### */

/* ------------------------------------------------------- */
/* U8 STSYS_ReadRegDev8(void *Address_p); */
/* ------------------------------------------------------- */
#define STSYS_ReadRegDev8(Address_p) (*((STSYS_DU8 *) (Address_p)))

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev8(void *Address_p, U8 Value); */
/* ------------------------------------------------------- */
#define STSYS_WriteRegDev8(Address_p, Value)                            \
    {                                                                   \
        *((STSYS_DU8 *) (Address_p)) = (U8) (Value);                    \
    }

/* ------------------------------------------------------- */
/* U16 STSYS_ReadRegDev16BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev16BE(Address_p) ((U16) (((*(((STSYS_DU8 *) (Address_p))    )) << 8) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1))     )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegDev16BE(Address_p) (*((STSYS_DU16 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* U16 STSYS_ReadRegDev16LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev16LE(Address_p) ((U16) (((*(((STSYS_DU8 *) (Address_p))    )) ) |     \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1)) << 8)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegDev16LE(Address_p) (*((STSYS_DU16 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev16BE(void *Address_p, U16 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev16BE(Address_p, Value)                               \
    {                                                                         \
        *((STSYS_DU16 *) (Address_p)) = (U16) ((((Value) & 0xFF00) >> 8) |    \
                                               (((Value) & 0x00FF) << 8));    \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev16BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_DU16 *) (Address_p)) = (U16) (Value);                  \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev16BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value) >> 8);       \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value)     );       \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev16LE(void *Address_p, U16 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev16LE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_DU16 *) (Address_p)) = (U16) (Value);                  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev16LE(Address_p, Value)                              \
    {                                                                        \
        *((STSYS_DU16 *) (Address_p)) = (U16) ((((Value) & 0xFF00) >> 8) |   \
                                               (((Value) & 0x00FF) << 8));   \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev16LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value)     );       \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8);       \
    }
#endif /* _NO_OPTIMIZATION */


/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegDev24BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev24BE(Address_p) ((U32) (((*(((STSYS_DU8 *) (Address_p))    )) << 16) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1)) <<  8) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 2))      )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegDev24BE(Address_p) ((U32) (((*(((STSYS_DU8  *) (Address_p))    )) << 16) | \
                                                ((*(((STSYS_DU16 *) (Address_p)) + 1))      )))
#endif

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegDev24LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev24LE(Address_p) ((U32) (((*(((STSYS_DU8 *) (Address_p))    ))      ) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1)) << 8 ) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 2)) << 16)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegDev24LE(Address_p) ((U32) (((*(((STSYS_DU8  *) (Address_p)) + 2)) << 16) | \
                                                ((*(((STSYS_DU16 *) (Address_p))    ))      )))
#endif


/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev24BE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev24BE(Address_p, Value)                                      \
    {                                                                                \
        *(((STSYS_DU16 *) (Address_p))    ) = (U16) ((((Value) & 0xFF0000) >> 16) |  \
                                                     (((Value) & 0x00FF00)      ));  \
        *(((STSYS_DU8  *) (Address_p)) + 2) =  (U8)   ((Value)                   );  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev24BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU16 *) (Address_p))    ) = (U16) ((Value) >> 8);     \
        *(((STSYS_DU8  *) (Address_p)) + 2) =  (U8) ((Value)     );     \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev24BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value) >> 16);      \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_DU8 *) (Address_p)) + 2) = (U8) ((Value)      );      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev24LE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev24LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU16 *) (Address_p))    ) = (U16) (Value);            \
        *(((STSYS_DU8 *)  (Address_p)) + 2) = (U8) ((Value) >> 16);     \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev24LE(Address_p, Value)                                    \
    {                                                                              \
        *(((STSYS_DU16 *) (Address_p))   ) = (U16) ((((Value) & 0xFF00) >> 8) |    \
                                                    (((Value) & 0x00FF) << 8));    \
        *(((STSYS_DU8 *) (Address_p)) + 2) =  (U8)  (((Value)         ) >> 16);    \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev24LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value)      );      \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_DU8 *) (Address_p)) + 2) = (U8) ((Value) >> 16);      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegDev32BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev32BE(Address_p) ((U32) (((*(((STSYS_DU8 *) (Address_p))    )) << 24) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1)) << 16) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 2)) <<  8) | \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 3))     )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegDev32BE(Address_p) (*((STSYS_DU32 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegDev32LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegDev32LE(Address_p) ((U32) (((*(((STSYS_DU8 *) (Address_p))    ))      ) |  \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 1)) << 8 ) |  \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 2)) << 16) |  \
                                                ((*(((STSYS_DU8 *) (Address_p)) + 3)) << 24)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegDev32LE(Address_p) (*((STSYS_DU32 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev32BE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev32BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_DU32 *) (Address_p)) = (U32) ((((Value) & 0xFF000000) >> 24) |   \
                                               (((Value) & 0x00FF0000) >> 8 ) |   \
                                               (((Value) & 0x0000FF00) << 8 ) |   \
                                               (((Value) & 0x000000FF) << 24));   \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev32BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_DU32 *) (Address_p)) = (U32) (Value);                  \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev32BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value) >> 24);      \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value) >> 16);      \
        *(((STSYS_DU8 *) (Address_p)) + 2) = (U8) ((Value) >> 8 );      \
        *(((STSYS_DU8 *) (Address_p)) + 3) = (U8) ((Value)      );      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegDev32LE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegDev32LE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_DU32 *) (Address_p)) = (U32) (Value);                  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegDev32LE(Address_p, Value)                                   \
    {                                                                             \
        *((STSYS_DU32 *) (Address_p)) = (U32) ((((Value) & 0xFF000000) >> 24) |   \
                                               (((Value) & 0x00FF0000) >> 8 ) |   \
                                               (((Value) & 0x0000FF00) << 8 ) |   \
                                               (((Value) & 0x000000FF) << 24));   \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegDev32LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_DU8 *) (Address_p))    ) = (U8) ((Value)      );      \
        *(((STSYS_DU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_DU8 *) (Address_p)) + 2) = (U8) ((Value) >> 16);      \
        *(((STSYS_DU8 *) (Address_p)) + 3) = (U8) ((Value) >> 24);      \
    }
#endif /* _NO_OPTIMIZATION */


/* ########################################################################### */
/*                     'MEMORY' ACCESS                                         */
/* ########################################################################### */


/* ------------------------------------------------------- */
/* U8 STSYS_ReadRegMem8(void *Address_p); */
/* ------------------------------------------------------- */
#define STSYS_ReadRegMem8(Address_p) (*((STSYS_MU8 *) (Address_p)))

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem8(void *Address_p, U8 Value); */
/* ------------------------------------------------------- */
#define STSYS_WriteRegMem8(Address_p, Value)                            \
    {                                                                   \
        *((STSYS_MU8 *) (Address_p)) = (U8) (Value);                    \
    }

/* ------------------------------------------------------- */
/* U16 STSYS_ReadRegMem16BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem16BE(Address_p) ((U16) (((*(((STSYS_MU8 *) (Address_p))    )) << 8) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1))     )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegMem16BE(Address_p) (*((STSYS_MU16 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* U16 STSYS_ReadRegMem16LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem16LE(Address_p) ((U16) (((*(((STSYS_MU8 *) (Address_p))    )) ) |     \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1)) << 8)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegMem16LE(Address_p) (*((STSYS_MU16 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem16BE(void *Address_p, U16 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem16BE(Address_p, Value)                               \
    {                                                                         \
        *((STSYS_MU16 *) (Address_p)) = (U16) ((((Value) & 0xFF00) >> 8) |    \
                                               (((Value) & 0x00FF) << 8));    \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem16BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_MU16 *) (Address_p)) = (U16) (Value);                  \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem16BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value) >> 8);       \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value)     );       \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem16LE(void *Address_p, U16 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem16LE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_MU16 *) (Address_p)) = (U16) (Value);                  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem16LE(Address_p, Value)                              \
    {                                                                        \
        *((STSYS_MU16 *) (Address_p)) = (U16) ((((Value) & 0xFF00) >> 8) |   \
                                               (((Value) & 0x00FF) << 8));   \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem16LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value)     );       \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8);       \
    }
#endif /* _NO_OPTIMIZATION */


/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegMem24BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem24BE(Address_p) ((U32) (((*(((STSYS_MU8 *) (Address_p))    )) << 16) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1)) <<  8) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 2))      )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegMem24BE(Address_p) ((U32) (((*(((STSYS_MU8  *) (Address_p))    )) << 16) | \
                                                ((*(((STSYS_MU16 *) (Address_p)) + 1))      )))
#endif

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegMem24LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem24LE(Address_p) ((U32) (((*(((STSYS_MU8 *) (Address_p))    ))      ) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1)) << 8 ) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 2)) << 16)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegMem24LE(Address_p) ((U32) (((*(((STSYS_MU8  *) (Address_p)) + 2)) << 16) | \
                                                ((*(((STSYS_MU16 *) (Address_p))    ))      )))
#endif


/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem24BE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem24BE(Address_p, Value)                                      \
    {                                                                                \
        *(((STSYS_MU16 *) (Address_p))    ) = (U16) ((((Value) & 0xFF0000) >> 16) |  \
                                                     (((Value) & 0x00FF00)      ));  \
        *(((STSYS_MU8  *) (Address_p)) + 2) =  (U8)   ((Value)                   );  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem24BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU16 *) (Address_p))    ) = (U16) ((Value) >> 8);     \
        *(((STSYS_MU8  *) (Address_p)) + 2) =  (U8) ((Value)     );     \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem24BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value) >> 16);      \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_MU8 *) (Address_p)) + 2) = (U8) ((Value)      );      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem24LE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem24LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU16 *) (Address_p))    ) = (U16) (Value);            \
        *(((STSYS_MU8 *)  (Address_p)) + 2) = (U8) ((Value) >> 16);     \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem24LE(Address_p, Value)                                    \
    {                                                                              \
        *(((STSYS_MU16 *) (Address_p))   ) = (U16) ((((Value) & 0xFF00) >> 8) |    \
                                                    (((Value) & 0x00FF) << 8));    \
        *(((STSYS_MU8 *) (Address_p)) + 2) =  (U8)  (((Value)         ) >> 16);    \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem24LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value)      );      \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_MU8 *) (Address_p)) + 2) = (U8) ((Value) >> 16);      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegMem32BE(void *Address_p); */
/* ------------------------------------------------------- */
#if (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
     (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( little endian CPU or ( big endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem32BE(Address_p) ((U32) (((*(((STSYS_MU8 *) (Address_p))    )) << 24) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1)) << 16) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 2)) <<  8) | \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 3))     )))
#else
/* ( big endian CPU and access optimized )*/
#define STSYS_ReadRegMem32BE(Address_p) (*((STSYS_MU32 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* U32 STSYS_ReadRegMem32LE(void *Address_p); */
/* ------------------------------------------------------- */
#if (defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) || \
   (!defined (STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE) && defined (STSYS_MEMORY_ACCESS_NO_OPTIMIZATION)))
/* ( big endian CPU or ( little endian CPU and access not optimized ) )*/
#define STSYS_ReadRegMem32LE(Address_p) ((U32) (((*(((STSYS_MU8 *) (Address_p))    ))      ) |  \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 1)) << 8 ) |  \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 2)) << 16) |  \
                                                ((*(((STSYS_MU8 *) (Address_p)) + 3)) << 24)))
#else
/* ( little endian CPU and access optimized )*/
#define STSYS_ReadRegMem32LE(Address_p) (*((STSYS_MU32 *) (Address_p)))
#endif

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem32BE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem32BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_MU32 *) (Address_p)) = (U32) ((((Value) & 0xFF000000) >> 24) |   \
                                               (((Value) & 0x00FF0000) >> 8 ) |   \
                                               (((Value) & 0x0000FF00) << 8 ) |   \
                                               (((Value) & 0x000000FF) << 24));   \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem32BE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_MU32 *) (Address_p)) = (U32) (Value);                  \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem32BE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value) >> 24);      \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value) >> 16);      \
        *(((STSYS_MU8 *) (Address_p)) + 2) = (U8) ((Value) >> 8 );      \
        *(((STSYS_MU8 *) (Address_p)) + 3) = (U8) ((Value)      );      \
    }
#endif /* _NO_OPTIMIZATION */

/* ------------------------------------------------------- */
/* void STSYS_WriteRegMem32LE(void *Address_p, U32 Value); */
/* ------------------------------------------------------- */
#ifndef STSYS_MEMORY_ACCESS_NO_OPTIMIZATION     /* optimized */
#ifndef STSYS_MEMORY_ACCESS_BIG_NOT_LITTLE      /* little endian CPU */
#define STSYS_WriteRegMem32LE(Address_p, Value)                         \
    {                                                                   \
        *((STSYS_MU32 *) (Address_p)) = (U32) (Value);                  \
    }
#else                                           /* big endian CPU */
#define STSYS_WriteRegMem32LE(Address_p, Value)                                   \
    {                                                                             \
        *((STSYS_MU32 *) (Address_p)) = (U32) ((((Value) & 0xFF000000) >> 24) |   \
                                               (((Value) & 0x00FF0000) >> 8 ) |   \
                                               (((Value) & 0x0000FF00) << 8 ) |   \
                                               (((Value) & 0x000000FF) << 24));   \
    }
#endif /* _BIG_NOT_LITTLE */
#else                                           /* not optimized */
#define STSYS_WriteRegMem32LE(Address_p, Value)                         \
    {                                                                   \
        *(((STSYS_MU8 *) (Address_p))    ) = (U8) ((Value)      );      \
        *(((STSYS_MU8 *) (Address_p)) + 1) = (U8) ((Value) >> 8 );      \
        *(((STSYS_MU8 *) (Address_p)) + 2) = (U8) ((Value) >> 16);      \
        *(((STSYS_MU8 *) (Address_p)) + 3) = (U8) ((Value) >> 24);      \
    }
#endif /* _NO_OPTIMIZATION */



/* Exported Functions ------------------------------------------------------- */


/* C++ support */
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef ST_OSLINUX */

#if defined(STAPIREF_COMPAT)
#else
/* STFAE - START - Create function for STSYS access in user mode */
#if defined(ST_OSLINUX)&&!defined(MODULE)
#include "stddefs.h"
/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif
void STSYS_WriteRegDev8(U32 Address,U8 Value);
U8   STSYS_ReadRegDev8(U32 Address);
void STSYS_WriteRegDev16LE(U32 Address,U16 Value);
U16  STSYS_ReadRegDev16LE(U32 Address);
void STSYS_WriteRegDev32LE(U32 Address,U32 Value);
U32  STSYS_ReadRegDev32LE(U32 Address);
void STSYS_WriteRegDev16BE(U32 Address,U16 Value);
U16  STSYS_ReadRegDev16BE(U32 Address);
void STSYS_WriteRegDev32BE(U32 Address,U32 Value);
U32  STSYS_ReadRegDev32BE(U32 Address);
void STSYS_WriteRegMem8(U32 Address,U8 Value);
U8   STSYS_ReadRegMem8(U32 Address);
void STSYS_WriteRegMem16LE(U32 Address,U16 Value);
U16  STSYS_ReadRegMem16LE(U32 Address);
void STSYS_WriteRegMem32LE(U32 Address,U32 Value);
U32  STSYS_ReadRegMem32LE(U32 Address);
void STSYS_WriteRegMem16BE(U32 Address,U16 Value);
U16  STSYS_ReadRegMem16BE(U32 Address);
void STSYS_WriteRegMem32BE(U32 Address,U32 Value);
U32  STSYS_ReadRegMem32BE(U32 Address);
U32  STSYS_Memory_Allocate(U32 Size);
U32  STSYS_Memory_UserToKernel(U32 Buffer);
U32  STSYS_Memory_KernelToUser(U32 Buffer);
#if defined(STAPIREF_COMPAT)
#else
/* STFAE - START - Add functions to get physical addresses */
U32  STSYS_Memory_DeviceToUser(U32 Buffer);
U32  STSYS_Memory_UserToDevice(U32 Buffer);
U32  STSYS_MapRegisters(U32 pAddr,U32 Length,char *Name);
void STSYS_UnmapRegisters(U32 vAddr,U32 Length);
/* STFAE - STOP  - Add functions to get physical addresses */
#endif /* STAPIREF_COMPAT */
void STSYS_Memory_Deallocate(U32 Buffer);
/* C++ support */
#ifdef __cplusplus
}
#endif
#endif
/* STFAE - STOP  - Create function for STSYS access in user mode */

/* STFAE - START - Create function for STSYS access in kernel mode */
#if defined(ST_OSLINUX)&&defined(MODULE)
#include "stddefs.h"
/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif
#define STSYS_ReadRegDev8(Address_p)                     readb(Address_p)
#define STSYS_ReadRegMem8(Address_p)                     readb(Address_p)
#define STSYS_ReadRegDev16LE(address)                    readw(address)
#define STSYS_ReadRegMem16LE(Address_p)                  readw(Address_p)
#define STSYS_ReadRegDev16BE(Address_p)                  (U32)(((U32)(readb((U32)(Address_p)))<<8)|((U32)(readb((U32)(Address_p+1)))))
#define STSYS_ReadRegMem16BE(Address_p)                  STSYS_ReadRegDev16BE(Address_p)
#define STSYS_ReadRegDev24LE(Address_p)                  ((U32)((readb((U32)(Address_p)+2))<<16|(readw(Address_p))))
#define STSYS_ReadRegMem24LE(Address_p)                  STSYS_ReadRegDev24LE(Address_p)
#define STSYS_ReadRegDev24BE(Address_p)                  (U32)(((U32)(readb((U32)(Address_p)))<<16)|((U32)(readb((U32)(Address_p+1)))<<8)|((U32)(readb((U32)(Address_p+2)))))
#define STSYS_ReadRegMem24BE(Address_p)                  STSYS_ReadRegDev24BE(Address_p)
#define STSYS_ReadRegDev32LE(address)                    readl(address)
#define STSYS_ReadRegMem32LE(address)                    readl(address)
#define STSYS_ReadRegDev32BE(Address_p)                  (U32)(((U32)(readb((U32)(Address_p)))<<24)|((U32)(readb((U32)(Address_p+1)))<<16)|((U32)(readb((U32)(Address_p+2)))<<8)|((U32)(readb((U32)(Address_p+3)))))
#define STSYS_ReadRegMem32BE(Address_p)                  STSYS_ReadRegDev32BE(Address_p)
#define STSYS_WriteRegDev8(Address_p, value)             writeb((u8)value, (void*)Address_p)
#define STSYS_WriteRegMem8(Address, value)               writeb((u8)value, (u32)Address)
#define STSYS_WriteRegDev16LE(Address_p, value)          writew((u16)value, (void*)Address_p)
#define STSYS_WriteRegMem16LE(Address_p, value)          writew((u16)value, (void*)Address_p)
#define STSYS_WriteRegDev32LE(Address_p, value)          writel((u32)value, (void*)Address_p)
#define STSYS_WriteRegMem32LE(Address, value)            writel((u32)value, (u32)Address)
#define STSYS_SetRegMask32LE(address, mask)              STSYS_WriteRegDev32LE(address, STSYS_ReadRegDev32LE(address) | (mask) )
#define STSYS_ClearRegMask32LE(address, mask)            STSYS_WriteRegDev32LE(address, STSYS_ReadRegDev32LE(address) & ~(mask) )
#define STSYS_ReadRegMemUncached8(Address_p)             STSYS_ReadRegDev8((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000))
#define STSYS_WriteRegMemUncached8(Address_p, Value)     STSYS_WriteRegDev8((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000), (Value))
#define STSYS_ReadRegMemUncached16LE(Address_p)          STSYS_ReadRegDev16LE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000))
#define STSYS_WriteRegMemUncached16LE(Address_p, Value)  STSYS_WriteRegDev16LE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000), (Value))
#define STSYS_ReadRegMemUncached16BE(Address_p)          STSYS_ReadRegDev16BE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000))
#define STSYS_WriteRegMemUncached16BE(Address_p, Value)  STSYS_WriteRegDev16BE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000), (Value))
#define STSYS_ReadRegMemUncached32LE(Address_p)          STSYS_ReadRegDev32LE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000))
#define STSYS_WriteRegMemUncached32LE(Address_p, Value)  STSYS_WriteRegDev32LE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000), (Value))
#define STSYS_ReadRegMemUncached32BE(Address_p)          STSYS_ReadRegDev32BE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000))
#define STSYS_WriteRegMemUncached32BE(Address_p, Value)  STSYS_WriteRegDev32BE((void *)(((U32)(Address_p) & 0x1FFFFFFF) | 0xA0000000), (Value))
#define STSYS_WriteRegDev16BE(Address_p, Value)          {U16 Reg16BEToLE=(U16)((((Value)&0xFF00)>>8)|(((Value)&0x00FF)<<8)); writew(Reg16BEToLE,(void*)Address_p); }
#define STSYS_WriteRegDev24BE(Address_p, Value)          {U16 Reg24BEToLE=(U16)((((Value)&0xFF0000)>>16)|(((Value)&0x00FF00))); writew(Reg24BEToLE,(void*)Address_p); writeb((U8)(Value),(void*)((U32)(Address_p)+2)); }
#define STSYS_WriteRegDev24LE(Address_p, Value)          {writew((U16)(Value),(void*)Address_p); writeb((U8)(((Value)&0xFF0000)>>16),(void*)((U32)(Address_p)+2)); }
#define STSYS_WriteRegDev32BE(Address_p, Value)          {U32 Reg32BEToLE=(U32)((((Value)&0xFF000000)>>24)|(((Value)&0x00FF0000)>>8)|(((Value)&0x0000FF00)<<8)|(((Value)&0x000000FF)<<24)); writel(Reg32BEToLE,(void*)Address_p); }
#define STSYS_WriteRegMem16BE(Address_p, Value)          STSYS_WriteRegDev16BE(Address_p,Value)
#define STSYS_WriteRegMem24BE(Address_p, Value)          STSYS_WriteRegDev24BE(Address_p,Value)
#define STSYS_WriteRegMem24LE(Address_p, Value)          STSYS_WriteRegDev24LE(Address_p,Value)
#define STSYS_WriteRegMem32BE(Address_p, Value)          STSYS_WriteRegDev32BE(Address_p,Value)
/* C++ support */
#ifdef __cplusplus
}
#endif
#endif
/* STFAE - STOP - Create function for STSYS access in kernel mode */
#endif /* STAPIREF_COMPAT */
#endif /* #ifndef __STSYS_H */

/* End of stsys.h */
