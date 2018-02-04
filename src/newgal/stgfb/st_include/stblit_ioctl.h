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
 @File   stblit_ioctl.h
 @brief



*/
#ifndef STBLIT_IOCTL_H
#define STBLIT_IOCTL_H

#include <linux/ioctl.h>   /* Defines macros for ioctl numbers */



#include "stblit.h"

#define STBLIT_IOCTL_LINUX_DEVICE_NAME       "stblit_ioctl"
/*** IOCtl defines ***/
/* Magic number used to differenciate STBLIT IOCTL with other drivers' */
#if defined(STAPIREF_COMPAT)
#define STBLIT_MAGIC_NUMBER      30
#else
/* STFAE - Define a global magic number */
#ifndef STBLIT_IOCTL_MAGIC_NUMBER
#define STBLIT_MAGIC_NUMBER      30
#else
#define STBLIT_MAGIC_NUMBER      STBLIT_IOCTL_MAGIC_NUMBER
#endif
#endif /* STAPIREF_COMPAT */

#define STBLIT_IOC_INIT                     _IO(STBLIT_MAGIC_NUMBER, 0)
#define STBLIT_IOC_TERM                     _IO(STBLIT_MAGIC_NUMBER, 1)
#define STBLIT_IOC_OPEN                     _IO(STBLIT_MAGIC_NUMBER, 2)
#define STBLIT_IOC_CLOSE                    _IO(STBLIT_MAGIC_NUMBER, 3)
#define STBLIT_IOC_GETCAPABILITY            _IO(STBLIT_MAGIC_NUMBER, 4)
#define STBLIT_IOC_WAITEVENT                _IO(STBLIT_MAGIC_NUMBER, 5)
#define STBLIT_IOC_FILLRECTANGLE            _IO(STBLIT_MAGIC_NUMBER, 6)
#define STBLIT_IOC_BLIT                     _IO(STBLIT_MAGIC_NUMBER, 7)
#define STBLIT_IOC_COPYRECTANGLE            _IO(STBLIT_MAGIC_NUMBER, 8)
#define STBLIT_IOC_DRAWHLINE                _IO(STBLIT_MAGIC_NUMBER, 9)
#define STBLIT_IOC_DRAWVLINE                _IO(STBLIT_MAGIC_NUMBER, 10)
#define STBLIT_IOC_SETPIXEL                 _IO(STBLIT_MAGIC_NUMBER, 11)
#define STBLIT_IOC_GETPIXEL                 _IO(STBLIT_MAGIC_NUMBER, 12)
#define STBLIT_IOC_DRAWXYARRAY              _IO(STBLIT_MAGIC_NUMBER, 13)
#define STBLIT_IOC_DRAWXYCARRAY             _IO(STBLIT_MAGIC_NUMBER, 14)
#define STBLIT_IOC_DRAWXYLARRAY             _IO(STBLIT_MAGIC_NUMBER, 15)
#define STBLIT_IOC_DRAWXYLCARRAY            _IO(STBLIT_MAGIC_NUMBER, 16)
#define STBLIT_IOC_CONCAT                   _IO(STBLIT_MAGIC_NUMBER, 17)
#define STBLIT_IOC_GETOPTIMALRECTANGLE      _IO(STBLIT_MAGIC_NUMBER, 18)
#define STBLIT_IOC_GETBITMAPALLOCPARAMS     _IO(STBLIT_MAGIC_NUMBER, 19)
#define STBLIT_IOC_GETINITALLOCPARAMS       _IO(STBLIT_MAGIC_NUMBER, 20)
#define STBLIT_IOC_GETPALETTEALLOCPARAMS    _IO(STBLIT_MAGIC_NUMBER, 21)
#define STBLIT_IOC_GETBITMAPHEADERSIZE      _IO(STBLIT_MAGIC_NUMBER, 22)
#define STBLIT_IOC_SUBMITJOBFRONT           _IO(STBLIT_MAGIC_NUMBER, 23)
#define STBLIT_IOC_SUBMITJOBBACK            _IO(STBLIT_MAGIC_NUMBER, 24)
#define STBLIT_IOC_LOCK                     _IO(STBLIT_MAGIC_NUMBER, 25)
#define STBLIT_IOC_UNLOCK                   _IO(STBLIT_MAGIC_NUMBER, 26)
#define STBLIT_IOC_GETJOBBLITHANDLE         _IO(STBLIT_MAGIC_NUMBER, 27)
#define STBLIT_IOC_SETJOBBLITRECTANGLE      _IO(STBLIT_MAGIC_NUMBER, 28)
#define STBLIT_IOC_BLITSLICERECTANGLE       _IO(STBLIT_MAGIC_NUMBER, 29)
#define STBLIT_IOC_GETSLICERECTANGLENUMBER  _IO(STBLIT_MAGIC_NUMBER, 30)
#define STBLIT_IOC_GETSLICERECTANGLEBUFFER  _IO(STBLIT_MAGIC_NUMBER, 31)
#define STBLIT_IOC_SETJOBBLITBITMAP         _IO(STBLIT_MAGIC_NUMBER, 32)
#define STBLIT_IOC_SETJOBBLITCOLORFILL      _IO(STBLIT_MAGIC_NUMBER, 33)
#define STBLIT_IOC_SETJOBBLITCLIPRECTANGLE  _IO(STBLIT_MAGIC_NUMBER, 34)
#define STBLIT_IOC_SETJOBBLITMASKWORD       _IO(STBLIT_MAGIC_NUMBER, 35)
#define STBLIT_IOC_SETJOBBLITCOLORKEY       _IO(STBLIT_MAGIC_NUMBER, 36)
#define STBLIT_IOC_CREATEJOB                _IO(STBLIT_MAGIC_NUMBER, 37)
#define STBLIT_IOC_DELETEJOB                _IO(STBLIT_MAGIC_NUMBER, 38)
#define STBLIT_IOC_GETFLICKERFILTERMODE     _IO(STBLIT_MAGIC_NUMBER, 39)
#define STBLIT_IOC_SETFLICKERFILTERMODE     _IO(STBLIT_MAGIC_NUMBER, 40)
#define STBLIT_IOC_ALLOCDATA                _IO(STBLIT_MAGIC_NUMBER, 43)
#define STBLIT_IOC_FREEDATA                 _IO(STBLIT_MAGIC_NUMBER, 44)
#define STBLIT_IOC_ALLOCDATASECURE          _IO(STBLIT_MAGIC_NUMBER, 45)
#ifdef STBLIT_DEBUG_GET_STATISTICS
#define STBLIT_IOC_GETSTATISTICS            _IO(STBLIT_MAGIC_NUMBER, 46)
#define STBLIT_IOC_RESETSTATISTICS          _IO(STBLIT_MAGIC_NUMBER, 47)
#endif /* STBLIT_DEBUG_GET_STATISTICS */

#define STBLIT_IOC_ENABLEANTIFLUTTER        _IO(STBLIT_MAGIC_NUMBER, 48)
#define STBLIT_IOC_DISABLEANTIFLUTTER       _IO(STBLIT_MAGIC_NUMBER, 49)
#define STBLIT_IOC_ENABLEANTIALIASING       _IO(STBLIT_MAGIC_NUMBER, 50)
#define STBLIT_IOC_DISABLEANTIALIASING      _IO(STBLIT_MAGIC_NUMBER, 51)
#define STBLIT_IOC_ENABLEDEI                _IO(STBLIT_MAGIC_NUMBER, 52)
#define STBLIT_IOC_DISABLEDEI               _IO(STBLIT_MAGIC_NUMBER, 53)
#define STBLIT_IOC_WAITALLBLITSCOMPLETED    _IO(STBLIT_MAGIC_NUMBER, 54)
#define STBLIT_IOC_ENABLESYMMETRY           _IO(STBLIT_MAGIC_NUMBER, 55)
#define STBLIT_IOC_DISABLESYMMETRY          _IO(STBLIT_MAGIC_NUMBER, 56)
#define STBLIT_IOC_SETCOLOREFFECT           _IO(STBLIT_MAGIC_NUMBER, 57)



#define  BLIT_PARTITION_AVMEM  0
#define  BLIT_SECURED_PARTITION_AVMEM  2


#define SRC1_ENABLED                               1
#define SRC2_ENABLED                               2
#define SRC1_SRC2_ENABLED                          3


typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STBLIT_InitParams_t     InitParams;
} STBLIT_Ioctl_Init_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STBLIT_TermParams_t     TermParams;
} STBLIT_Ioctl_Term_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STBLIT_OpenParams_t     OpenParams;
    STBLIT_Handle_t         Handle;
} STBLIT_Ioctl_Open_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
} STBLIT_Ioctl_Close_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
     ST_DeviceName_t          DeviceName ;
     STBLIT_Capability_t     Capability ;
} STBLIT_Ioctl_GetCapability_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STGXOBJ_Rectangle_t     Rectangle;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_FillRectangle_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STBLIT_Source_t         Src1;
    STBLIT_Source_t         Src2;
    STBLIT_Destination_t    Dst;
    STBLIT_BlitContext_t    BlitContext;
    U32                     SrcUsed; /*the SrcUsed is even: 0x01:means src1 is used, 0x10:src2 is used, 0x11:both src1 and src2 are used*/
} STBLIT_Ioctl_Blit_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        SrcBitmap;
    STGXOBJ_Rectangle_t     SrcRectangle;
    STGXOBJ_Bitmap_t        DstBitmap;
    S32                     DstPositionX;
    S32                     DstPositionY;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_CopyRectangle_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    S32                     PositionX;
    S32                     PositionY;
    U32                     Length;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawHLine_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    S32                     PositionX;
    S32                     PositionY;
    U32                     Length;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawVLine_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    S32                     PositionX;
    S32                     PositionY;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_SetPixel_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    S32                     PositionX;
    S32                     PositionY;
    STGXOBJ_Color_t         Color;
} STBLIT_Ioctl_GetPixel_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STBLIT_XY_t             XYArray;
    U32                     PixelsNumber;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawXYArray_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STBLIT_XYC_t            XYArray;
    U32                     PixelsNumber;
    STGXOBJ_Color_t         ColorArray;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawXYCArray_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STBLIT_XYL_t            XYLArray;
    U32                     SegmentsNumber;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawXYLArray_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STBLIT_XYL_t            XYLArray;
    U32                     SegmentsNumber;
    STGXOBJ_Color_t         ColorArray;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_DrawXYLCArray_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        SrcAlphaBitmap;
    STGXOBJ_Rectangle_t     SrcAlphaRectangle;
    STGXOBJ_Bitmap_t        SrcColorBitmap;
    STGXOBJ_Rectangle_t     SrcColorRectangle;
    STGXOBJ_Bitmap_t        DstBitmap;
    STGXOBJ_Rectangle_t     DstRectangle;
    STBLIT_BlitContext_t    BlitContext;
} STBLIT_Ioctl_Concat_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        Bitmap;
    STGXOBJ_Rectangle_t     Rectangle;
} STBLIT_Ioctl_GetOptimalRectangle_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STGXOBJ_Bitmap_t                Bitmap;
    STGXOBJ_BitmapAllocParams_t     Params1;
    STGXOBJ_BitmapAllocParams_t     Params2;
} STBLIT_Ioctl_GetBitmapAllocParams_t;



typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_AllocParams_t Params;
} STBLIT_Ioctl_GetInitAllocParams_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STGXOBJ_Palette_t               Palette;
    STGXOBJ_PaletteAllocParams_t    Params;
} STBLIT_Ioctl_GetPaletteAllocParams_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t     Handle;
    STGXOBJ_Bitmap_t    Bitmap;
    U32                 HeaderSize;
} STBLIT_Ioctl_GetBitmapHeaderSize_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t     Handle;
    STBLIT_JobHandle_t  JobHandle;
} STBLIT_Ioctl_JobStr_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t             Handle;
    STBLIT_JobHandle_t          JobHandle;
    STBLIT_JobBlitHandle_t      JobBlitHandle;
} STBLIT_Ioctl_GetJobBlitHandle_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t         Handle;
    STBLIT_JobBlitHandle_t  JobBlitHandle;
    STBLIT_DataType_t       DataType;
    STGXOBJ_Rectangle_t     Rectangle;
} STBLIT_Ioctl_SetJobBlitRectangle_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t           Handle;
    STBLIT_SliceData_t        Src1;
    STBLIT_SliceData_t        Src2;
    STBLIT_SliceData_t        Dst;
    STBLIT_SliceRectangle_t   SliceRectangle;
    STBLIT_BlitContext_t      BlitContext;
} STBLIT_Ioctl_BlitSliceRectangle_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_SliceRectangleBuffer_t   Buffer;
} STBLIT_Ioctl_GetSliceRectangle_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobBlitHandle_t          JobBlitHandle;
    STBLIT_DataType_t               DataType;
    STGXOBJ_Bitmap_t                Bitmap;
} STBLIT_Ioctl_SetJobBlitBitmap_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobBlitHandle_t          JobBlitHandle;
    STBLIT_DataType_t               DataType;
    STGXOBJ_Color_t                 Color;
} STBLIT_Ioctl_SetJobBlitColorFill_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobBlitHandle_t          JobBlitHandle;
    BOOL                            WriteInsideClipRectangle;
    STGXOBJ_Rectangle_t             ClipRectangle;
} STBLIT_Ioctl_SetJobBlitClipRectangle_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobBlitHandle_t          JobBlitHandle;
    U32                             MaskWord;
} STBLIT_Ioctl_SetJobBlitMaskWord_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobBlitHandle_t          JobBlitHandle;
    STBLIT_ColorKeyCopyMode_t       ColorKeyCopyMode;
    STGXOBJ_ColorKey_t              ColorKey;
} STBLIT_Ioctl_SetJobBlitColorKey_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_JobParams_t              Params;
    STBLIT_JobHandle_t              JobHandle;
} STBLIT_Ioctl_CreateJob_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_FlickerFilterMode_t      FlickerFilterMode;
} STBLIT_Ioctl_GetFlickerFilterMode_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_FlickerFilterMode_t      FlickerFilterMode;
} STBLIT_Ioctl_SetFlickerFilterMode_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_EnableAntiFlutter_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_DisableAntiFlutter_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_EnableAntiAliasing_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_DisableAntiAliasing_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    BOOL                            TopField;
} STBLIT_Ioctl_EnableDEI_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_ColorEffectMode_t        ColorEffectMode;
    STBLIT_ColorEffectData_t        ColorEffectData;
    STBLIT_ColorEffectStage_t       ColorEffectStage;
} STBLIT_Ioctl_SetColorEffect_t;


typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_DisableDEI_t;

typedef struct{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t          ErrorCode;

    /* Parameters to the function */
    STBLIT_Handle_t          BlitHandle;
    void                     *Address_p;
} STBLIT_Ioctl_FreeData_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_WaitAllBlitsCompleted_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_SymmetryType_t 			SymmetryType;
} STBLIT_Ioctl_EnableSymmetry_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_DisableSymmetry_t;

#ifdef STBLIT_DEBUG_GET_STATISTICS
typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
    STBLIT_Statistics_t             Statistics;
} STBLIT_Ioctl_GetStatistics_t;

typedef struct
{
    /* Error code retrieved by STAPI function */
     ST_ErrorCode_t  ErrorCode;
    /* Parameters to the function */
    STBLIT_Handle_t                 Handle;
} STBLIT_Ioctl_ResetStatistics_t;

#endif /* STBLIT_DEBUG_GET_STATISTICS */


#endif
