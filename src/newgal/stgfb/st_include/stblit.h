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
 @File   stblit.h
 @brief  blitter module header file



*/

/* Define to prevent recursive inclusion */
#ifndef __STBLIT_H
#define __STBLIT_H


/* Includes ----------------------------------------------------------------- */
#if !defined(MODULE)
#include <stdlib.h>
#endif

#include "stddefs.h"


#if !defined(ST_OS21) && !defined(ST_OSWINCE)
	#include "stlite.h"
#endif
#include "stevt.h"
#include "stavmem.h"
#include "stgxobj.h"

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants ------------------------------------------------------- */



#define STBLIT_DRIVER_ID    157
#define STBLIT_DRIVER_BASE  (STBLIT_DRIVER_ID << 16)

enum
{
    STBLIT_ERROR_FUNCTION_NOT_IMPLEMENTED = STBLIT_DRIVER_BASE,
    STBLIT_ERROR_MAX_JOB_NODE,
    STBLIT_ERROR_MAX_SINGLE_BLIT_NODE,
    STBLIT_ERROR_JOB_CLOSED,
    STBLIT_ERROR_MAX_JOB,
    STBLIT_ERROR_JOB_HANDLE_MISMATCH,
    STBLIT_ERROR_INVALID_JOB_HANDLE,
    STBLIT_ERROR_JOB_EMPTY,
    STBLIT_ERROR_JOB_PENDING_LOCK,
    STBLIT_ERROR_NO_AV_MEMORY,
    STBLIT_ERROR_OVERLAP_NOT_SUPPORTED,
    STBLIT_ERROR_INVALID_JOB_BLIT_HANDLE,
    STBLIT_ERROR_JOB_BLIT_HANDLE_MISMATCH,
    STBLIT_ERROR_MAX_JOB_BLIT,
    STBLIT_ERROR_WORK_BUFFER_SIZE,
#ifdef ST_OSWINCE
	STBLIT_ERROR_JOB_BLIT_DETACHED,
#endif
#ifdef ST_OSLINUX
    STBLIT_ERROR_NESTED_LOCK,
    STBLIT_ERROR_MAP_BLIT
#else /* ! ST_OSLINUX */
    STBLIT_ERROR_NESTED_LOCK,
    STBLIT_ERROR_MAP_BLIT
#endif
};

enum
{
    STBLIT_BLIT_COMPLETED_EVT = STBLIT_DRIVER_BASE,
    STBLIT_JOB_COMPLETED_EVT
#ifdef STBLIT_HW_BLITTER_RESET_DEBUG
    ,STBLIT_HARDWARE_ERROR_EVT
#endif /* STBLIT_HW_BLITTER_RESET_DEBUG */
};


/* Exported Types ----------------------------------------------------------- */

typedef enum  STBLIT_AluMode_e
{
    STBLIT_ALU_CLEAR            = 0,
    STBLIT_ALU_AND              = 1,
    STBLIT_ALU_AND_REV          = 2,
    STBLIT_ALU_COPY             = 3,
    STBLIT_ALU_AND_INVERT       = 4,
    STBLIT_ALU_NOOP             = 5,
    STBLIT_ALU_XOR              = 6,
    STBLIT_ALU_OR               = 7,
    STBLIT_ALU_NOR              = 8,
    STBLIT_ALU_EQUIV            = 9,
    STBLIT_ALU_INVERT           = 10,
    STBLIT_ALU_OR_REVERSE       = 11,
    STBLIT_ALU_COPY_INVERT      = 12,
    STBLIT_ALU_OR_INVERT        = 13,
    STBLIT_ALU_NAND             = 14,
    STBLIT_ALU_SET              = 15,
    STBLIT_ALU_ALPHA_BLEND      = 16
} STBLIT_AluMode_t;

typedef enum  STBLIT_DeviceType_e
{
    STBLIT_DEVICE_TYPE_SOFTWARE_EMULATION,
    STBLIT_DEVICE_TYPE_GAMMA_7015,
    STBLIT_DEVICE_TYPE_GAMMA_ST40GX1,
    STBLIT_DEVICE_TYPE_GAMMA_7020,
    STBLIT_DEVICE_TYPE_SOFTWARE,
    STBLIT_DEVICE_TYPE_GAMMA_5528,
    STBLIT_DEVICE_TYPE_GAMMA_7710,
    STBLIT_DEVICE_TYPE_GAMMA_7100,
    STBLIT_DEVICE_TYPE_BDISP_5100,
    STBLIT_DEVICE_TYPE_BDISP_5105,
    STBLIT_DEVICE_TYPE_BDISP_5107,
    STBLIT_DEVICE_TYPE_BDISP_5301,
    STBLIT_DEVICE_TYPE_BDISP_7109,
    STBLIT_DEVICE_TYPE_BDISP_5188,
    STBLIT_DEVICE_TYPE_BDISP_5525,
    STBLIT_DEVICE_TYPE_BDISP_7200,
    STBLIT_DEVICE_TYPE_BDISP_5162,
    STBLIT_DEVICE_TYPE_BDISP_7111,
    STBLIT_DEVICE_TYPE_BDISP_7105,
    STBLIT_DEVICE_TYPE_BDISP_7141,
	STBLIT_DEVICE_TYPE_BDISP_5197,
    STBLIT_DEVICE_TYPE_BDISP_7106,
    STBLIT_DEVICE_TYPE_BDISP_5206,
    STBLIT_DEVICE_TYPE_BDISP_7108
} STBLIT_DeviceType_t;

typedef enum STBLIT_SliceFixedRectangleType_e
{
    STBLIT_SLICE_FIXED_RECTANGLE_TYPE_SOURCE,
    STBLIT_SLICE_FIXED_RECTANGLE_TYPE_DESTINATION
} STBLIT_SliceFixedRectangleType_t;

typedef enum STBLIT_SourceType_e
{
    STBLIT_SOURCE_TYPE_COLOR,
    STBLIT_SOURCE_TYPE_BITMAP,
    STBLIT_SOURCE_TYPE_COLOR_PREMULTIPLIED
} STBLIT_SourceType_t;

typedef enum STBLIT_TriggerType_e
{
	STBLIT_TRIGGER_TYPE_TOP_LINE,
	STBLIT_TRIGGER_TYPE_BOTTOM_LINE,
	STBLIT_TRIGGER_TYPE_PROGRESSIVE_LINE,
	STBLIT_TRIGGER_TYPE_TOP_VSYNC,
	STBLIT_TRIGGER_TYPE_BOTTOM_VSYNC,
    STBLIT_TRIGGER_TYPE_PROGRESSIVE_VSYNC,
    STBLIT_TRIGGER_TYPE_TOP_CAPTURE,
    STBLIT_TRIGGER_TYPE_BOTTOM_CAPTURE,
    STBLIT_TRIGGER_TYPE_PROGRESSIVE_CAPTURE
} STBLIT_TriggerType_t;

typedef enum STBLIT_ColorKeyCopyMode_e
{
    STBLIT_COLOR_KEY_MODE_NONE,
    STBLIT_COLOR_KEY_MODE_SRC,
    STBLIT_COLOR_KEY_MODE_DST
} STBLIT_ColorKeyCopyMode_t;

typedef enum STBLIT_FlickerFilterMode_e
{
    STBLIT_FLICKER_FILTER_MODE_ADAPTIVE,
    STBLIT_FLICKER_FILTER_MODE_SIMPLE
} STBLIT_FlickerFilterMode_t;


typedef enum STBLIT_DataType_e
{
    STBLIT_DATA_TYPE_FOREGROUND,
    STBLIT_DATA_TYPE_BACKGROUND,
    STBLIT_DATA_TYPE_DESTINATION,
    STBLIT_DATA_TYPE_MASK
} STBLIT_DataType_t;

typedef enum STBLIT_SymmetryType_e
{
    STBLIT_SYMMETRY_TYPE_HORIZONTAL,
    STBLIT_SYMMETRY_TYPE_VERTICAL
} STBLIT_SymmetryType_t;


typedef enum STBLIT_ColorEffectMode_e
{
    STBLIT_COLOR_EFFECT_NONE,
    STBLIT_COLOR_EFFECT_USER_DEFINED
} STBLIT_ColorEffectMode_t;

typedef enum STBLIT_ColorEffectStage_e
{
    STBLIT_COLOR_EFFECT_INPUT_STAGE,
    STBLIT_COLOR_EFFECT_OUTPUT_STAGE
}STBLIT_ColorEffectStage_t;

typedef struct STBLIT_ColorEffectUserDefined_s
{
    U32  ColorEffectUserDefTable[4];   /* versatile matrix 4 x 32 bits used for color effects */
} STBLIT_ColorEffectUserDefined_t;

typedef struct STBLIT_ColorEffectNone_s
{
	U32  Dummy;
} STBLIT_ColorEffectNone_t;

typedef union STBLIT_ColorEffectData_u
{
    STBLIT_ColorEffectUserDefined_t ColorEffectUserDefined;
    STBLIT_ColorEffectNone_t        ColorEffectNone;
} STBLIT_ColorEffectData_t;

typedef U32 STBLIT_JobHandle_t;
#define STBLIT_NO_JOB_HANDLE ((STBLIT_JobHandle_t) NULL)

typedef U32 STBLIT_JobBlitHandle_t;

typedef struct STBLIT_AllocParams_s
{
    U32     SingleBlitNodeSize;
    BOOL    SingleBlitNodeInSharedMemory;
    U32     SingleBlitNodeAlignment;

    U32     JobBlitNodeSize;
    BOOL    JobBlitNodeInSharedMemory;
    U32     JobBlitNodeAlignment;

    U32     JobBlitSize;
    BOOL    JobBlitInSharedMemory;
    U32     JobBlitAlignment;

    U32     JobSize;
    BOOL    JobInSharedMemory;
    U32     JobAlignment;

    U32     MinWorkBufferSize;
    BOOL    WorkBufferInSharedMemory;
    U32     WorkBufferAlignment;

/*    U32     MinFilterBufferSize;*/
/*    BOOL    FilterBufferInSharedMemory;*/
/*    U32     FilterBufferAlignment;*/

} STBLIT_AllocParams_t;

typedef struct STBLIT_Trigger_s
{
    BOOL                    EnableTrigger;
    STBLIT_TriggerType_t    Type;
    U8                      VTGId;
    U16                     ScanLine;
} STBLIT_Trigger_t;

typedef struct STBLIT_SliceRectangle_s
{
    STGXOBJ_Rectangle_t     SrcRectangle;
    STGXOBJ_Rectangle_t     DstRectangle;
    STGXOBJ_Rectangle_t     ClipRectangle;
    U16                     Reserved1;
    U16                     Reserved2;
    U16                     Reserved3;
    U16                     Reserved4;
} STBLIT_SliceRectangle_t;

typedef struct STBLIT_SliceRectangleBuffer_s
{
    STBLIT_SliceFixedRectangleType_t    FixedRectangleType;
    STGXOBJ_Rectangle_t                 FixedRectangle;
    STGXOBJ_Rectangle_t                 FullSizeSrcRectangle;
    STGXOBJ_Rectangle_t                 FullSizeDstRectangle;
    U32                                 HorizontalSlicingNumber;
    U32                                 VerticalSlicingNumber;
    STBLIT_SliceRectangle_t*            SliceRectangleArray;
} STBLIT_SliceRectangleBuffer_t;

typedef struct STBLIT_SliceData_s
{
    BOOL                           UseSliceRectanglePosition;
    STGXOBJ_Bitmap_t*              Bitmap_p;
    STGXOBJ_Palette_t*             Palette_p;
} STBLIT_SliceData_t;

typedef struct STBLIT_BlitContext_s
{
    STBLIT_ColorKeyCopyMode_t       ColorKeyCopyMode;
    STGXOBJ_ColorKey_t              ColorKey;
    STBLIT_AluMode_t                AluMode;
    BOOL                            EnableMaskWord;
    U32                             MaskWord;
    BOOL                            EnableMaskBitmap;
    STGXOBJ_Bitmap_t*               MaskBitmap_p;
    STGXOBJ_Rectangle_t             MaskRectangle;
    void*                           WorkBuffer_p;
    BOOL                            EnableColorCorrection;
    STGXOBJ_Palette_t*              ColorCorrectionTable_p;
    STBLIT_Trigger_t                Trigger;
    U8                              GlobalAlpha;
    BOOL                            EnableClipRectangle;
    STGXOBJ_Rectangle_t             ClipRectangle;
    BOOL                            WriteInsideClipRectangle;
    BOOL                            EnableFlickerFilter;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
    BOOL                            EnableResizeFilter;
#endif
    STBLIT_JobHandle_t              JobHandle;
    void*                           UserTag_p;
    BOOL                            NotifyBlitCompletion;
    STEVT_SubscriberID_t            EventSubscriberID;
} STBLIT_BlitContext_t;

typedef struct STBLIT_Destination_s
{
    STGXOBJ_Bitmap_t*      Bitmap_p;
    STGXOBJ_Rectangle_t    Rectangle;
    STGXOBJ_Palette_t*     Palette_p;
} STBLIT_Destination_t;

typedef struct STBLIT_JobParams_s
{
    BOOL                  NotifyJobCompletion;
    STEVT_SubscriberID_t  EventSubscriberID;
/*    pthread_attr_t      EVENT_TaskAttribute;*/
} STBLIT_JobParams_t;

typedef struct STBLIT_Source_s
{
    STBLIT_SourceType_t     Type;
    union {
            STGXOBJ_Color_t*   Color_p;
            STGXOBJ_Bitmap_t*  Bitmap_p;
    } Data;
    STGXOBJ_Rectangle_t    Rectangle;
    STGXOBJ_Palette_t*     Palette_p;
} STBLIT_Source_t;


typedef struct STBLIT_Capability_s
{
    U32 dummy ;           /* To Be defined */
} STBLIT_Capability_t;


typedef U32 STBLIT_Handle_t;

typedef struct STBLIT_InitParams_s
{
    STBLIT_DeviceType_t         DeviceType;
    ST_Partition_t*             CPUPartition_p;
    void*                       BaseAddress_p;
    STAVMEM_PartitionHandle_t   AVMEMPartition;
    void*                       SharedMemoryBaseAddress_p;
    ST_DeviceName_t             EventHandlerName;
    ST_DeviceName_t             BlitInterruptEventName;
    STEVT_EventConstant_t       BlitInterruptEvent;
    U32                         BlitInterruptNumber;
    U32                         BlitInterruptLevel;
    U32                         MaxHandles;
    BOOL                        SingleBlitNodeBufferUserAllocated;
    U32                         SingleBlitNodeMaxNumber;
    void*                       SingleBlitNodeBuffer_p;
    BOOL                        JobBlitNodeBufferUserAllocated;
    U32                         JobBlitNodeMaxNumber;
    void*                       JobBlitNodeBuffer_p;
    BOOL                        JobBlitBufferUserAllocated;
    U32                         JobBlitMaxNumber;
    void*                       JobBlitBuffer_p;
    BOOL                        JobBufferUserAllocated;
    U32                         JobMaxNumber;
    void*                       JobBuffer_p;
    BOOL                        WorkBufferUserAllocated;
    U32                         WorkBufferSize;
    void*                       WorkBuffer_p;
    BOOL                        BigNotLittle;

} STBLIT_InitParams_t;

typedef struct STBLIT_OpenParams_s
{
    U32 Dummy;             /* To Be defined */
} STBLIT_OpenParams_t;

typedef struct STBLIT_TermParams_s
{
    BOOL ForceTerminate;
} STBLIT_TermParams_t;



typedef struct STBLIT_XY_s
{
    S32 PositionX;
    S32 PositionY;
    U32 Reserved1;
    U32 Reserved2;
} STBLIT_XY_t;

typedef struct STBLIT_XYC_s
{
    S32 PositionX;
    S32 PositionY;
    U32 Reserved1;
    U32 Reserved2;
} STBLIT_XYC_t;

typedef struct STBLIT_XYL_s
{
    S32 PositionX;
    S32 PositionY;
    U32 Length;
    U32 Reserved;
} STBLIT_XYL_t;


#define STBLIT_NUM_AQS 1



/* Exported Variables ------------------------------------------------------- */

#ifdef STBLIT_TEST_FRONTEND
extern U32 STBLIT_FirstNodeAddress;
#endif

/* Exported Macros ---------------------------------------------------------- */


/* Exported Functions ------------------------------------------------------- */

ST_ErrorCode_t STBLIT_Blit( STBLIT_Handle_t             Handle,
                            STBLIT_Source_t*            Src1_p,
                            STBLIT_Source_t*            Src2_p,
                            STBLIT_Destination_t*       Dst_p,
                            STBLIT_BlitContext_t*       BlitContext_p );

ST_ErrorCode_t STBLIT_Close(STBLIT_Handle_t  Handle);

ST_ErrorCode_t STBLIT_CopyRectangle( STBLIT_Handle_t       Handle,
                                     STGXOBJ_Bitmap_t*        SrcBitmap_p,
                                     STGXOBJ_Rectangle_t*     SrcRectangle_p,
                                     STGXOBJ_Bitmap_t*        DstBitmap_p,
                                     S32                   DstPositionX,
                                     S32                   DstPositionY,
                                     STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_FillRectangle( STBLIT_Handle_t       Handle,
                                     STGXOBJ_Bitmap_t*        Bitmap_p,
                                     STGXOBJ_Rectangle_t*     Rectangle_p,
                                     STGXOBJ_Color_t*         Color_p,
                                     STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_CreateJob( STBLIT_Handle_t         Handle,
                                 STBLIT_JobParams_t*     Params_p,
                                 STBLIT_JobHandle_t*     JobHandle_p);

ST_ErrorCode_t STBLIT_DeleteJob( STBLIT_Handle_t        Handle,
                                 STBLIT_JobHandle_t     JobHandle);

ST_ErrorCode_t STBLIT_DrawHLine( STBLIT_Handle_t       Handle,
                                 STGXOBJ_Bitmap_t*     Bitmap_p,
                                 S32                   PositionX,
                                 S32                   PositionY,
                                 U32                   Length,
                                 STGXOBJ_Color_t*         Color_p,
                                 STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_SetPixel(  STBLIT_Handle_t       Handle,
                                 STGXOBJ_Bitmap_t*        Bitmap_p,
                                 S32                   PositionX,
                                 S32                   PositionY,
                                 STGXOBJ_Color_t*         Color_p,
                                 STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_DrawVLine( STBLIT_Handle_t       Handle,
                                 STGXOBJ_Bitmap_t*        Bitmap_p,
                                 S32                   PositionX,
                                 S32                   PositionY,
                                 U32                   Length,
                                 STGXOBJ_Color_t*         Color_p,
                                 STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_DrawXYArray( STBLIT_Handle_t       Handle,
                                   STGXOBJ_Bitmap_t*        Bitmap_p,
                                   STBLIT_XY_t*          XYArray_p,
                                   U32                   PixelsNumber,
                                   STGXOBJ_Color_t*         Color_p,
                                   STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_DrawXYCArray( STBLIT_Handle_t       Handle,
                                    STGXOBJ_Bitmap_t*        Bitmap_p,
                                    STBLIT_XYC_t*         XYArray_p,
                                    U32                   PixelsNumber,
                                    STGXOBJ_Color_t*         ColorArray_p,
                                    STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_DrawXYLArray( STBLIT_Handle_t       Handle,
                                    STGXOBJ_Bitmap_t*        Bitmap_p,
                                    STBLIT_XYL_t*         XYLArray_p,
                                    U32                   SegmentsNumber,
                                    STGXOBJ_Color_t*         Color_p,
                                    STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_DrawXYLCArray( STBLIT_Handle_t       Handle,
                                     STGXOBJ_Bitmap_t*        Bitmap_p,
                                     STBLIT_XYL_t*         XYLArray_p,
                                     U32                   SegmentsNumber,
                                     STGXOBJ_Color_t*         ColorArray_p,
                                     STBLIT_BlitContext_t* BlitContext_p );

ST_ErrorCode_t STBLIT_GetCapability(const ST_DeviceName_t DeviceName,
                                    STBLIT_Capability_t * const Capability_p);

ST_ErrorCode_t STBLIT_GetBitmapAllocParams(STBLIT_Handle_t                 Handle,
                                           STGXOBJ_Bitmap_t*               Bitmap_p,
                                           STGXOBJ_BitmapAllocParams_t*    Params1_p,
                                           STGXOBJ_BitmapAllocParams_t*    Params2_p);

ST_ErrorCode_t STBLIT_GetBitmapHeaderSize (STBLIT_Handle_t      Handle,
                                           STGXOBJ_Bitmap_t*    Bitmap_p,
                                           U32*                 HeaderSize_p);

ST_ErrorCode_t STBLIT_GetInitAllocParams( STBLIT_AllocParams_t* Params_p);

ST_ErrorCode_t STBLIT_GetOptimalRectangle( STBLIT_Handle_t      Handle,
                                           STGXOBJ_Bitmap_t*       Bitmap_p,
                                           STGXOBJ_Rectangle_t*    Rectangle_p);

ST_ErrorCode_t STBLIT_GetPaletteAllocParams(STBLIT_Handle_t                 Handle,
                                            STGXOBJ_Palette_t*              Palette_p,
                                            STGXOBJ_PaletteAllocParams_t*   Params_p);

ST_ErrorCode_t STBLIT_GetPixel(  STBLIT_Handle_t       Handle,
                                 STGXOBJ_Bitmap_t*        Bitmap_p,
                                 S32                   PositionX,
                                 S32                   PositionY,
                                 STGXOBJ_Color_t*         Color_p);

ST_Revision_t STBLIT_GetRevision(void);


ST_ErrorCode_t STBLIT_Init(const ST_DeviceName_t DeviceName,
                           const STBLIT_InitParams_t * const InitParams_p);

ST_ErrorCode_t STBLIT_Lock(STBLIT_Handle_t  Handle, STBLIT_JobHandle_t JobHandle);

ST_ErrorCode_t STBLIT_Open(const ST_DeviceName_t      DeviceName,
                           const STBLIT_OpenParams_t* const Params_p,
                           STBLIT_Handle_t*           Handle_p);

ST_ErrorCode_t STBLIT_SubmitJobFront( STBLIT_Handle_t        Handle,
                                      STBLIT_JobHandle_t     JobHandle);

ST_ErrorCode_t STBLIT_SubmitJobBack( STBLIT_Handle_t        Handle,
                                     STBLIT_JobHandle_t     JobHandle);

ST_ErrorCode_t STBLIT_Term(const ST_DeviceName_t DeviceName,
                       const STBLIT_TermParams_t * const TermParams_p);

ST_ErrorCode_t STBLIT_Unlock(STBLIT_Handle_t  Handle, STBLIT_JobHandle_t JobHandle);

/* Slice functions */
ST_ErrorCode_t STBLIT_BlitSliceRectangle(
    STBLIT_Handle_t           Handle,
    STBLIT_SliceData_t*       Src1_p,
    STBLIT_SliceData_t*       Src2_p,
    STBLIT_SliceData_t*       Dst_p,
    STBLIT_SliceRectangle_t*  SliceRectangle_p,
    STBLIT_BlitContext_t*     BlitContext_p
);

ST_ErrorCode_t STBLIT_GetSliceRectangleNumber(
    STBLIT_Handle_t                  Handle,
    STBLIT_SliceRectangleBuffer_t*   Buffer_p
);

ST_ErrorCode_t STBLIT_GetSliceRectangleBuffer(
    STBLIT_Handle_t                  Handle,
    STBLIT_SliceRectangleBuffer_t*   Buffer_p
);

ST_ErrorCode_t STBLIT_GetJobBlitHandle(
    STBLIT_Handle_t                Handle,
    STBLIT_JobHandle_t             JobHandle,
    STBLIT_JobBlitHandle_t*        JobBlitHandle_p
);

#ifdef ST_OSWINCE
ST_ErrorCode_t STBLIT_RemoveJobBlit(
	STBLIT_Handle_t Handle,
    STBLIT_JobBlitHandle_t JobBlitHandle
);

ST_ErrorCode_t STBLIT_RemoveAllJobBlits(
	STBLIT_Handle_t Handle,
    STBLIT_JobHandle_t JobHandle
);

ST_ErrorCode_t STBLIT_InsertJobBlitBefore(
	STBLIT_Handle_t Handle,
	STBLIT_JobHandle_t JobHandle,
    STBLIT_JobBlitHandle_t insertBeforeThisJobBlit,
    STBLIT_JobBlitHandle_t insertThisJobBlit
);

ST_ErrorCode_t STBLIT_InsertJobBlitAfter(
	STBLIT_Handle_t Handle,
	STBLIT_JobHandle_t JobHandle,
    STBLIT_JobBlitHandle_t insertAfterThisJobBlit,
    STBLIT_JobBlitHandle_t insertThisJobBlit
);

ST_ErrorCode_t STBLIT_InsertJobBlitAtEnd(
	STBLIT_Handle_t Handle,
	STBLIT_JobHandle_t JobHandle,
    STBLIT_JobBlitHandle_t insertThisJobBlit
);

ST_ErrorCode_t STBLIT_InsertJobBlitCopy(
	STBLIT_Handle_t Handle,
	STBLIT_JobHandle_t JobHandle,
    STBLIT_JobBlitHandle_t sourceJobBlit
);


ST_ErrorCode_t STBLIT_DeleteJobBlit(
	STBLIT_Handle_t Handle,
    STBLIT_JobBlitHandle_t JobBlitHandle
);
#endif


ST_ErrorCode_t STBLIT_SetJobBlitRectangle(STBLIT_Handle_t               Handle,
                                          STBLIT_JobBlitHandle_t        JobBlitHandle,
                                          STBLIT_DataType_t             DataType,
                                          STGXOBJ_Rectangle_t*          Rectangle_p
);

ST_ErrorCode_t STBLIT_SetJobBlitBitmap(STBLIT_Handle_t        Handle,
                                       STBLIT_JobBlitHandle_t JobBlitHandle,
                                       STBLIT_DataType_t      DataType,
                                       STGXOBJ_Bitmap_t*      Bitmap_p
);

#ifdef ST_OSWINCE
ST_ErrorCode_t STBLIT_SetJobBlitBitmapAndRectangle(STBLIT_Handle_t        Handle,
                                                   STBLIT_JobBlitHandle_t JobBlitHandle,
                                                   STBLIT_DataType_t      DataType,
                                                   STGXOBJ_Bitmap_t*      Bitmap_p,
                                                   STGXOBJ_Rectangle_t*   Rectangle_p
);
#endif

ST_ErrorCode_t STBLIT_SetJobBlitColorFill(STBLIT_Handle_t         Handle,
                                          STBLIT_JobBlitHandle_t  JobBlitHandle,
                                          STBLIT_DataType_t   DataType,
                                          STGXOBJ_Color_t*        Color_p);

ST_ErrorCode_t STBLIT_SetJobBlitClipRectangle(STBLIT_Handle_t          Handle,
                                              STBLIT_JobBlitHandle_t   JobBlitHandle,
                                              BOOL                     WriteInsideClipRectangle,
                                              STGXOBJ_Rectangle_t*     ClipRectangle_p);

ST_ErrorCode_t STBLIT_SetJobBlitMaskWord(STBLIT_Handle_t          Handle,
                                         STBLIT_JobBlitHandle_t   JobBlitHandle,
                                         U32                      MaskWord);

ST_ErrorCode_t STBLIT_SetJobBlitColorKey(STBLIT_Handle_t          Handle,
                                         STBLIT_JobBlitHandle_t   JobBlitHandle,
                                         STBLIT_ColorKeyCopyMode_t ColorKeyCopyMode,
                                         STGXOBJ_ColorKey_t*       ColorKey_p);
#ifdef ST_OSWINCE
ST_ErrorCode_t STBLIT_SetJobBlitGlobalAlpha(STBLIT_Handle_t          Handle,
                                            STBLIT_JobBlitHandle_t   JobBlitHandle,
											U8                       GlobalAlpha);
#endif

ST_ErrorCode_t STBLIT_GetFlickerFilterMode(STBLIT_Handle_t                  Handle,
                                           STBLIT_FlickerFilterMode_t*      FlickerFilterMode_p);

ST_ErrorCode_t STBLIT_SetFlickerFilterMode(STBLIT_Handle_t                  Handle,
                                           STBLIT_FlickerFilterMode_t       FlickerFilterMode);

ST_ErrorCode_t STBLIT_DisableAntiFlutter(STBLIT_Handle_t    Handle);
ST_ErrorCode_t STBLIT_EnableAntiFlutter(STBLIT_Handle_t    Handle);

ST_ErrorCode_t STBLIT_DisableAntiAliasing(STBLIT_Handle_t    Handle);
ST_ErrorCode_t STBLIT_EnableAntiAliasing(STBLIT_Handle_t    Handle);

ST_ErrorCode_t STBLIT_Concat( STBLIT_Handle_t       Handle,
                              STGXOBJ_Bitmap_t*     SrcAlphaBitmap_p,
                              STGXOBJ_Rectangle_t*  SrcAlphaRectangle_p,
                              STGXOBJ_Bitmap_t*     SrcColorBitmap_p,
                              STGXOBJ_Rectangle_t*  SrcColorRectangle_p,
                              STGXOBJ_Bitmap_t*     DstBitmap_p,
                              STGXOBJ_Rectangle_t*  DstRectangle_p,
                              STBLIT_BlitContext_t* BlitContext_p);


ST_ErrorCode_t STBLIT_WaitAllBlitsCompleted(STBLIT_Handle_t  Handle);

ST_ErrorCode_t STBLIT_EnableDeInterlacing(STBLIT_Handle_t Handle, BOOL TopField);

ST_ErrorCode_t STBLIT_DisableDeInterlacing(STBLIT_Handle_t Handle);

ST_ErrorCode_t STBLIT_EnableSymmetry(STBLIT_Handle_t Handle, STBLIT_SymmetryType_t SymmetryType);

ST_ErrorCode_t STBLIT_DisableSymmetry(STBLIT_Handle_t Handle);

ST_ErrorCode_t STBLIT_SetColorEffect(STBLIT_Handle_t            Handle,
                                     STBLIT_ColorEffectMode_t   ColorEffectMode,
                                     STBLIT_ColorEffectData_t   ColorEffectData,
                                     STBLIT_ColorEffectStage_t  ColorEffectStage);

/* Un-supported extensions (debug only) ------------------------------------- */

/*******************************************************************************
WARNING (do not remove)
=======
Every user of this code must be made aware of the following limitations.
This section's API's are extensions to the STBLIT API, for debugging and testing
purposes only. They are not part of the STBLIT product delivery, they don't
contain driver features used in production. They are not compiled by default.
They may change without notice. They may contain errors, are not tested. They
may be not supported on some platforms, may change the driver normal behaviour.
Application code such as reference software or customer application may use
them at their own risks and not for production. There will be no support.
*******************************************************************************/
#ifdef STBLIT_DEBUG_GET_STATISTICS
typedef struct STBLIT_Statistics_s
{
	/* General information */
    U32 Submissions;                           /* Number of STBLIT APIs calls */
    U32 SubmissionsAtTheFront;                 /* Number of submissions at the front */
    U32 SubmissionsAtTheEnd;                   /* Number of submissions at the end */
    U32 BlitCompletionInterruptions;           /* Number of received completion interruptions */
    U32 CorrectTimeStatisticsValues;           /* Number of STBLIT APIs calls */

    /* Single Blit Nodes related*/
    U32 MinSingleBlitNode;                     /* Min Single Blit Nodes number */
    U32 MaxSingleBlitNode;                     /* Max Single Blit Nodes number */
    U32 LatestSingleBlitNode;                  /* Latest STBLIT call Single Blit Nodes number */

    /* Job Blit Nodes related*/
    U32 MinJobBlitNode;                        /* Min job Blit Nodes number */
    U32 MaxJobBlitNode;                        /* Max job Blit Nodes number */
    U32 LatestJobBlitNode;                     /* Latest STBLIT call job Blit Nodes number */

	/* Generation time information (Milli seconds) */
    U32 MinGenerationTime;                     /* Min node generation time */
    U32 AverageGenerationTime;                 /* Average node generation time */
    U32 MaxGenerationTime;                     /* Max node generation time */
    U32 LatestBlitGenerationTime;              /* Latest STBLIT call node generation time */

	/* Execution time information (Milli seconds) */
    U32 MinExecutionTime;                      /* Min blitter execution time */
    U32 AverageExecutionTime;                  /* Average blitter execution time */
    U32 MaxExecutionTime;                      /* Max blitter execution time */
    U32 LatestBlitExecutionTime;               /* Latest STBLIT call blitter execution time */

    /* Processing time (Generation + Execution) information (Milli seconds) */
    U32 MinProcessingTime;                      /* Min blitter execution time */
    U32 AverageProcessingTime;                  /* Average blitter execution time */
    U32 MaxProcessingTime;                      /* Max blitter execution time */
    U32 LatestBlitProcessingTime;               /* Latest STBLIT call blitter execution time */

	/* Execution rate information (MegaPixels/Second) */
    U32 MinExecutionRate;                      /* Min execution rate time */
    U32 AverageExecutionRate;                  /* Average execution rate time */
    U32 MaxExecutionRate;                      /* Max execution rate time */
    U32 LatestBlitExecutionRate;               /* Latest STBLIT call execution rate time */
} STBLIT_Statistics_t;

ST_ErrorCode_t STBLIT_GetStatistics(STBLIT_Handle_t  Handle, STBLIT_Statistics_t  * const Statistics_p);
ST_ErrorCode_t STBLIT_ResetStatistics(STBLIT_Handle_t  Handle);
#endif /* STBLIT_DEBUG_GET_STATISTICS */

#ifdef STBLIT_USE_MEMORY_TRACE
ST_ErrorCode_t STBLIT_MemoryTraceDump(void);
#endif /* STBLIT_USE_MEMORY_TRACE */



/* C++ support */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __STBLIT_H  */

/* End of stblit.h */




