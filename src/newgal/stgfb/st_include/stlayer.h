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
 @File   stlayer.h
 @brief



*/
/* Define to prevent recursive inclusion */
#ifndef __STLAYER_H
#define __STLAYER_H

/* Includes ----------------------------------------------------------------- */

#include "stddefs.h"
#include "stavmem.h"
#include "stgxobj.h"
#include "stevt.h"
#include "stcommon.h"

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

/* Software workarounds for hardware bugs */
/*----------------------------------------*/

/* tunable workarounds: -D<compile-key> are set in compilation command line */
/* by the makefile and depend on env variables */
/* see './makefile' */
/* see 'build options' paragraph in release note for tunable workarounds  */
/*                                              (eq non-auto workarounds) */

/* auto workarounds: depend on chips */
#ifdef HW_7015 /* st7015 workarounds */
#define WA_GNBvd06281 /* Horizontal SRC has not been implemented ...       */
#define WA_GNBvd06293 /* In interlaced mode, if a node ... height of 1 ... */
#define WA_GNBvd06299 /* In interlaced mode, if a node ... height of 1 ... */
#define WA_GNBvd06319 /* The clut update could only be done once           */
#endif

#ifdef HW_7020 /* st7020 workarounds */
#define WA_GNBvd06293 /* In interlaced mode, if a node ... height of 1 ... */
#define WA_GNBvd06299 /* In interlaced mode, if a node ... height of 1 ... */
#endif

#ifdef HW_GX1 /* st40GX1 workarounds */
#define WA_GNBycbcr             /* load YCbCr node crashes the GDP pipeline */
#define WA_taskwait_on_os20emu  /* this WA is a software bug */
#define WA_GdpResizeOnGX1       /* resize is limited */
#endif

/* Exported constants ------------------------------------------------------- */
/* #define DEBUG_RECORD_TIME */
#ifdef DEBUG_RECORD_TIME
/* Recording time in VTG interrupt */
#define NR_TIMEVAL (1024*10) /* length of the array of time values */
#endif

#define STLAYER_DRIVER_ID     158
#define STLAYER_DRIVER_BASE   (STLAYER_DRIVER_ID << 16)


/* STLAYER return codes */
enum
{
  STLAYER_ERROR_INVALID_INPUT_RECTANGLE = STLAYER_DRIVER_BASE,
  STLAYER_ERROR_INVALID_OUTPUT_RECTANGLE,
  STLAYER_ERROR_NO_FREE_HANDLES,
  STLAYER_SUCCESS_IORECTANGLES_ADJUSTED,
  STLAYER_ERROR_IORECTANGLES_NOT_ADJUSTABLE,
  STLAYER_ERROR_INVALID_LAYER_TYPE,
  STLAYER_ERROR_USER_ALLOCATION_NOT_ALLOWED,
  STLAYER_ERROR_OVERLAP_VIEWPORT,
  STLAYER_ERROR_NO_AV_MEMORY ,
  STLAYER_ERROR_OUT_OF_LAYER,
  STLAYER_ERROR_OUT_OF_BITMAP,
  STLAYER_ERROR_INSIDE_LAYER,
  STLAYER_ERROR_EVENT_REGISTRATION
};


/*  TBD to enable multiple Flicker Filter Mode */


  typedef enum STLAYER_FlickerFilterMode_e
{
    STLAYER_FLICKER_FILTER_MODE_SIMPLE,
    STLAYER_FLICKER_FILTER_MODE_ADAPTIVE,
    STLAYER_FLICKER_FILTER_MODE_USING_BLITTER
} STLAYER_FlickerFilterMode_t;



/* STLAYER exported evt */
enum
{
    /* This event passes a (STLAYER_LayerParams_t *) as parameter */
    STLAYER_UPDATE_PARAMS_EVT = STLAYER_DRIVER_BASE,
    /* This event passes a (STLAYER_UpdateParams_t *) as parameter */
    STLAYER_UPDATE_DECIMATION_EVT,
    /* This event passes a (STLAYER_FMDResults_t *) as parameter */
    STLAYER_NEW_FMD_REPORTED_EVT,
    /* This event passes a (STLAYER_DisplaySettings_t *) as parameter */
    STLAYER_VSYNC_DISPLAY_SETTINGS_EVT
};

typedef enum STLAYER_Layer_e
{
    STLAYER_GAMMA_CURSOR,
    STLAYER_GAMMA_GDP,
    STLAYER_GAMMA_BKL,
    STLAYER_GAMMA_ALPHA,
    STLAYER_GAMMA_FILTER,
    STLAYER_OMEGA2_VIDEO1,
    STLAYER_OMEGA2_VIDEO2,
    STLAYER_7020_VIDEO1,
    STLAYER_7020_VIDEO2,
    STLAYER_OMEGA1_CURSOR,
    STLAYER_OMEGA1_VIDEO,
    STLAYER_OMEGA1_OSD,
    STLAYER_OMEGA1_STILL,
    STLAYER_SDDISPO2_VIDEO1,    /* Main SD for 5528 */
    STLAYER_SDDISPO2_VIDEO2,    /* Aux SD for 5528 */
    STLAYER_HDDISPO2_VIDEO1,    /* Main HD for 7100 */
    STLAYER_HDDISPO2_VIDEO2,    /* Aux SD for 7100/7109 */
    STLAYER_DISPLAYPIPE_VIDEO1, /* Main HD for 7109 */
    STLAYER_DISPLAYPIPE_VIDEO2, /* Main HD for 7200 */
    STLAYER_DISPLAYPIPE_VIDEO3, /* Local Aux SD (SD0) for 7200. Aux SD for 7105/7111/7141/7106 */
    STLAYER_DISPLAYPIPE_VIDEO4, /* Remote SD(SD1) for 7200 */
    STLAYER_DISPLAYPIPE_VIDEO5, /* Main HD for 7105/7111/7141/7106 */
    STLAYER_DISPLAYPIPE_VIDEO6, /* Aux HD for 7108 (With DEI) */
    STLAYER_HQVDP_VIDEO1,       /* Main HD for 7108 */
    STLAYER_VIRTUAL_VIDEOLAYER,
    STLAYER_COMPOSITOR,
    STLAYER_GAMMA_GDPVBI
} STLAYER_Layer_t;

typedef enum STLAYER_ViewPortSourceType_e
{
    STLAYER_GRAPHIC_BITMAP,
    STLAYER_STREAMING_VIDEO
} STLAYER_ViewPortSourceType_t;

typedef enum STLAYER_CompressionLevel_e
{
    STLAYER_COMPRESSION_LEVEL_NONE    = 1,
    STLAYER_COMPRESSION_LEVEL_1       = 2,
    STLAYER_COMPRESSION_LEVEL_2       = 4
} STLAYER_CompressionLevel_t;

typedef enum STLAYER_DecimationFactor_e
{
    STLAYER_DECIMATION_FACTOR_NONE    = 0,
    STLAYER_DECIMATION_FACTOR_2       = 1,
    STLAYER_DECIMATION_FACTOR_4       = 2,
    STLAYER_DECIMATION_MPEG4_P2       = 3,
    STLAYER_DECIMATION_AVS             = 4
} STLAYER_DecimationFactor_t;

typedef enum STLAYER_UpdateReason_e
{
    STLAYER_DISCONNECT_REASON           = 0x01,
    STLAYER_SCREEN_PARAMS_REASON        = 0x02,
    STLAYER_OFFSET_REASON               = 0x04,
    STLAYER_VTG_REASON                  = 0x08,
    STLAYER_CHANGE_ID_REASON            = 0x10,
    STLAYER_DISPLAY_REASON              = 0x20,
    STLAYER_LAYER_PARAMS_REASON         = 0x40,
    STLAYER_DECIMATION_NEED_REASON      = 0x80,
    STLAYER_IMPOSSIBLE_WITH_PROFILE     = 0x100,
    STLAYER_CONNECT_REASON              = 0x200,
    STLAYER_DISABLE_MIXER_REASON        = 0x400,
    STLAYER_ENABLE_MIXER_REASON         = 0x800
}STLAYER_UpdateReason_t;

typedef enum
{
    STLAYER_NORMAL_MODE,            /* Normal mode (initial one).             */
    STLAYER_SPECTACLE_MODE          /* Spectacle mode : non-linear zoom.      */
} STLAYER_OutputMode_t;

typedef enum STLAYER_DeiMode_e
{
    STLAYER_DEI_MODE_OFF = 0,
    STLAYER_DEI_MODE_AUTO,                          /* FMD is tried first (if available) and then MLD mode will be tried */
    STLAYER_DEI_MODE_BYPASS,
    STLAYER_DEI_MODE_VERTICAL,
    STLAYER_DEI_MODE_DIRECTIONAL,
    STLAYER_DEI_MODE_FIELD_MERGING,                 /* OBSOLETE value not accepted anymore!!! */
    STLAYER_DEI_MODE_MEDIAN_FILTER,
    STLAYER_DEI_MODE_MLD,                           /* MLD Luma and Chroma */
    STLAYER_DEI_MODE_LMU,
    STLAYER_DEI_MODE_YMLD_CDI,                      /* MLD Luma, Directional Chroma */
    STLAYER_DEI_MODE_FIELD_MERGING_WITH_PREVIOUS,   /* Field Merging with Previous Field */
    STLAYER_DEI_MODE_FIELD_MERGING_WITH_NEXT,       /* Field Merging with Next Field */

    STLAYER_DEI_MODE_MAX                            /* This is not a valid mode */
} STLAYER_DeiMode_t;

/* Exported types ----------------------------------------------------------- */

typedef U32 STLAYER_Handle_t;
typedef U32 STLAYER_ViewPortHandle_t;

#ifdef DEBUG_RECORD_TIME
typedef struct STLAYER_Record_Time_s
{
    struct timeval  tv_data;
    int             where;  /* 0 For internal (to handler), 1 for external */
    int             irq_nb; /* identification of IRQ */
    unsigned long   data1; /* data1 */
    unsigned long   data2; /* data2 */
} STLAYER_Record_Time_t;
#endif


typedef enum
{
    STLAYER_TOP_FIELD,
    STLAYER_BOTTOM_FIELD
} STLAYER_FieldType_t;


typedef struct STLAYER_Field_s
{
    BOOL                   FieldAvailable;
    U32                    PictureIndex;
    STLAYER_FieldType_t    FieldType;
    /* With Macroblock sources:
       Data1_p = Luma Base Address (top and bottom field mixed)
       Data2_p = Chroma Base Address (top and bottom field mixed)

       With Raster sources:
       Data1_p = Picture Base Address made of a Top field (luma and chroma mixed) followed by a Bottom field (luma and chroma mixed)
       Data2_p doesn't matter */
    void *                 Data1_p;
    void *                 Data2_p;
} STLAYER_Field_t;

typedef struct STLAYER_PictureID_s
{
    S32     ID;             /* LSB for PictureID */
    U32     IDExtension;    /* MSB for PictureID, incremented at each "reset" of ID */
} STLAYER_PictureID_t;


typedef struct STLAYER_StreamingVideo_s
{
    U32                                 SourceNumber;
    /* Bitmaps params concerning Current Field. Data1_p and Data2_p are redundant with those contained in "CurrentField" */
    STGXOBJ_Bitmap_t                    BitmapParams;
    STGXOBJ_ScanType_t                  ScanType;
    U32                                 SrcFrameRate;
    BOOL                                PresentedFieldInverted;
    STLAYER_CompressionLevel_t          CompressionLevel;

    /* Available decimation factor                          */
    STLAYER_DecimationFactor_t          AvailableHorizontalDecimationFactor;
    STLAYER_DecimationFactor_t          AvailableVerticalDecimationFactor;

    /* Actual decimation factor of the picture.             */
    STLAYER_DecimationFactor_t          HorizontalDecimationFactor;
    STLAYER_DecimationFactor_t          VerticalDecimationFactor;

    BOOL                                AdvancedHDecimation;
    BOOL                                PeriodicFieldInversionDueToFRC;
    STLAYER_PictureID_t                 ExtendedPresentationOrderPictureID; /* Continuously increasing in presentation order, */
    U32                                 PictureSignature;                   /* Signature of Current picture (computed on all the frame) */
    STLAYER_Field_t                     PreviousField;
    STLAYER_Field_t                     CurrentField;
    STLAYER_Field_t                     NextField;
    BOOL                                IsAsynchronousProgramming;          /* TRUE when the display is not updated at the begining of a VSync */

#ifdef  STLAYER_USE_CRC
    BOOL                                CRCCheckMode;
    /* CRCScanType is mandatory because it is not allowed to change original*/
    /* ScanType from parsed picture */
    STGXOBJ_ScanType_t                  CRCScanType;
#endif /* STLAYER_USE_CRC */
} STLAYER_StreamingVideo_t;

typedef struct STLAYER_ViewPortSource_s
{
    STLAYER_ViewPortSourceType_t    SourceType;
    union
    {
        STGXOBJ_Bitmap_t *          BitMap_p;
        STLAYER_StreamingVideo_t *  VideoStream_p;
    }                               Data;
    STGXOBJ_Palette_t *            Palette_p;
} STLAYER_ViewPortSource_t;

typedef struct STLAYER_AllocParams_s
{
    U32     ViewPortDescriptorsBufferSize;
    BOOL    ViewPortNodesInSharedMemory;
    U32     ViewPortNodesBufferSize;
    U32     ViewPortNodesBufferAlignment;
} STLAYER_AllocParams_t;

typedef struct STLAYER_Capability_s
{
    STLAYER_Layer_t LayerType;
    U32             DeviceId;
    BOOL            MultipleViewPorts;
    BOOL            HorizontalResizing;
    BOOL            AlphaBorder;
    BOOL            GlobalAlpha;
    BOOL            ColorKeying;
    BOOL            MultipleViewPortsOnScanLineCapable;
    BOOL            PSI;
    U8              FrameBufferDisplayLatency;
    U8              FrameBufferHoldTime;
} STLAYER_Capability_t;

typedef struct STLAYER_GainParams_s
{
    U8      BlackLevel;
    U8      GainLevel;
} STLAYER_GainParams_t;

typedef struct STLAYER_GlobalAlpha_s
{
    U8 A0;
    U8 A1;
} STLAYER_GlobalAlpha_t;

typedef struct STLAYER_LayerParams_s
{
    U32                     Width;
    U32                     Height;
    STGXOBJ_AspectRatio_t   AspectRatio;
    STGXOBJ_ScanType_t      ScanType;
}STLAYER_LayerParams_t;

typedef enum
{
    FILTERSET_LEGACY=0,
    FILTERSET_SHARP,
    FILTERSET_MEDIUM,
    FILTERSET_SMOOTH,
    FILTERSET_LAST
}  STLAYER_ZoomFilterSet_t;

typedef struct STLAYER_InitParams_s
{
    STLAYER_Layer_t             LayerType;
    ST_Partition_t *            CPUPartition_p;
    BOOL                        CPUBigEndian;
    /* Base address of all the registers of the chip */
    void *                      DeviceBaseAddress_p;
    /* for Omega2 chips, BaseAddress is the base address of the plane regs */
    /* and BaseAddress2 is the base address of the video dislay registers  */
    /* (DIS_XX and DIP_XX)                                                 */
    /* for Omega1 chips, BaseAddress is the base add of the video display  */
    /* regs (VID_XX)                                                       */
    void *                      BaseAddress_p;
    void *                      BaseAddress2_p;
    void *                      SharedMemoryBaseAddress_p;
    U32                         MaxHandles;
    STAVMEM_PartitionHandle_t   AVMEM_Partition;
    ST_DeviceName_t             EventHandlerName;
    ST_DeviceName_t             InterruptEventName;
    STEVT_EventConstant_t       VideoDisplayInterrupt;
    U32                         MaxViewPorts;
    void *                      ViewPortNodeBuffer_p;
    BOOL                        NodeBufferUserAllocated;
    void *                      ViewPortBuffer_p;
    BOOL                        ViewPortBufferUserAllocated;
    STLAYER_LayerParams_t *     LayerParams_p;
} STLAYER_InitParams_t;

typedef struct STLAYER_OpenParams_s
{
    U32                 Dummy;
} STLAYER_OpenParams_t;

typedef struct STLAYER_OutputParams_s
{
    STLAYER_UpdateReason_t      UpdateReason;
    STGXOBJ_AspectRatio_t       AspectRatio;
    STGXOBJ_ScanType_t          ScanType;
    U32                         FrameRate;
    U32                         Width;
    U32                         Height;
    U32                         XStart;
    U32                         YStart;
    S8                          XOffset;
    S8                          YOffset;
    ST_DeviceName_t             VTGName;
    U32                         DeviceId;
    BOOL                        DisplayEnable;
    STLAYER_Handle_t            BackLayerHandle;
    STLAYER_Handle_t            FrontLayerHandle;
    U32                         DisplayHandle;
    U8                          FrameBufferDisplayLatency;
    U8                          FrameBufferHoldTime;
}STLAYER_OutputParams_t;

/* Video filter enum access (way to access to sepcific video filter)         */
typedef enum STLAYER_VideoFiltering_s
{
    /* Please keep enum order.    */
    STLAYER_VIDEO_CHROMA_AUTOFLESH,
    STLAYER_VIDEO_CHROMA_GREENBOOST,
    STLAYER_VIDEO_CHROMA_TINT,
    STLAYER_VIDEO_CHROMA_SAT,

    STLAYER_VIDEO_LUMA_EDGE_REPLACEMENT,
    STLAYER_VIDEO_LUMA_PEAKING,
    STLAYER_VIDEO_LUMA_DCI,
    STLAYER_VIDEO_LUMA_BC,

    STLAYER_ZOOM_FILTERSET,

    STLAYER_VIDEO_IQI_LTI,
    STLAYER_VIDEO_IQI_CTI,
    STLAYER_VIDEO_IQI_PEAKING,
    STLAYER_VIDEO_IQI_LUMA_ENHANCER,
    STLAYER_VIDEO_IQI_DEMO_MODE,

    /* Insert new filter names here ...... */

    STLAYER_MAX_VIDEO_FILTER_POSITION /* Last position of filter enum.       */
                                    /* Please insert new one above this line.*/

} STLAYER_VideoFiltering_t;

/* Video filter enum control (action to perform to specific video filter)    */
typedef enum STLAYER_VideoFilteringControl_s
{
    /* Please keep enum order.    */

    STLAYER_DISABLE,              /* Filter is disabled.                     */
                                  /* default values will be loaded !!!       */
    STLAYER_ENABLE_AUTO_MODE1,    /* Filter is enable with auto parameters 1 */
    STLAYER_ENABLE_AUTO_MODE2,    /* Filter is enable with auto parameters 2 */
    STLAYER_ENABLE_AUTO_MODE3,    /* Filter is enable with auto parameters 3 */

    STLAYER_ENABLE_MANUAL         /* Filter is enable with parameters        */
                                  /* set by API.                             */

} STLAYER_VideoFilteringControl_t;

typedef enum
{
    STLAYER_COMPOSITION_RECURRENCE_EVERY_VSYNC,                       /* Normal mode (initial one).                                   */
    STLAYER_COMPOSITION_RECURRENCE_MANUAL_OR_VIEWPORT_PARAMS_CHANGES  /* Spectacle mode : compose viewport only when content changed. */
} STLAYER_CompositionRecurrence_t;

/* Video filters individual manual parameters.      */
typedef struct STLAYER_AutoFleshParameters_s {
    U32 AutoFleshControl;   /* percent unit of maximum effect */
    enum {
        LARGE_WIDTH,
        MEDIUM_WIDTH,
        SMALL_WIDTH
    } QuadratureFleshWidthControl;
    enum {
        AXIS_116_6,         /* 116.60 */
        AXIS_121_0,         /* 121.00 */
        AXIS_125_5,         /* 125.50 */
        AXIS_130_2          /* 130.20 */
    } AutoFleshAxisControl;
} STLAYER_AutoFleshParameters_t;

typedef struct STLAYER_GreenBoostParameters_s {
    S32 GreenBoostControl;  /* percent unit of maximum effect */
                            /* Allowed : -100% to 100%        */
} STLAYER_GreenBoostParameters_t;

typedef struct STLAYER_TintRotationControl_s {
    S32 TintRotationControl;/* percent unit of maximum effect */
                            /* Allowed : -100% to 100%        */
} STLAYER_TintParameters_t;

typedef struct STLAYER_SatParameters_s {
    S32 SaturationGainControl;  /* 1100% of gain to apply to chroma     */
                                /* -100% = Max decrease of saturation.  */
                                /*    0% = No effect.                   */
                                /*  100% = Max Increase of saturation.  */
} STLAYER_SatParameters_t;

typedef struct STLAYER_EdgeReplacementParameters_s {
    U32 GainControl;        /* percent unit of maximum effect */
    enum {
        HIGH_FREQ_FILTER = 1,
        MEDIUM_FREQ_FILTER,
        LOW_FREQ_FILTER
    } FrequencyControl;
} STLAYER_EdgeReplacementParameters_t;

typedef struct STLAYER_PeakingParameters_s {
    S32 VerticalPeakingGainControl; /* percent unit of maximum effect        */
                                    /* Allowed : -100% to 100%               */
    U32 CoringForVerticalPeaking;   /* percent unit of maximum effect        */
    S32 HorizontalPeakingGainControl;   /* percent unit of maximum effect    */
                                        /* Allowed : -100% to 100%           */
    U32 CoringForHorizontalPeaking; /* percent unit of maximum effect        */
    U32 HorizontalPeakingFilterSelection;   /* Ratio (%) = 100 * Fs/Fc       */
                                            /* with Fs : Output sample freq. */
                                            /* with Fs : Centered filter freq*/
    BOOL SINECompensationEnable;    /* Sinx/x compensation enable/disable    */
} STLAYER_PeakingParameters_t;

typedef struct STLAYER_DCIParameters{
    U32 CoringLevelGainControl;     /* percent unit of maximum effect       */
    U32 FirstPixelAnalysisWindow;
    U32 LastPixelAnalysisWindow;
    U32 FirstLineAnalysisWindow;
    U32 LastLineAnalysisWindow;
} STLAYER_DCIParameters_t;

typedef struct STLAYER_BrightnessContrastParameters_s {
    S32 BrightnessGainControl;  /*       adjust luminance intensity     */
                                /* percent unit of maximum effect    */
                                /* Allowed : -100% to 100%           */
    U32 ContrastGainControl;    /*       gain to apply to chroma     */
                                /* percent unit of maximum effect   */
                                /* Allowed : 0% to 100%          */
} STLAYER_BrightnessContrastParameters_t;

typedef struct STLAYER_ZoomFilterSetParameters_s {
    STLAYER_ZoomFilterSet_t ZoomFilterSet;  /* Filter set to use when VHSRC zooming occurs */
} STLAYER_ZoomFilterSetParameters_t;


/*********************************************************************************/
/* IQI parameters */

#define STLAYER_IQI_LE_LUT_SIZE             (128)                           /* Define Maximum size of IQI LUT for Luma enhancer */
#define STLAYER_IQI_LE_LUT_SIZE_IN_BYTES    (256)                           /* We are storing 16 bits values because LU is on 10 bits */

#define  STLAYER_IQI_SOFT_PRESET STLAYER_ENABLE_AUTO_MODE1
#define  STLAYER_IQI_MEDIUM_PRESET STLAYER_ENABLE_AUTO_MODE2
#define  STLAYER_IQI_STRONG_PRESET STLAYER_ENABLE_AUTO_MODE3

typedef enum
{
    /* for these frequencies extended mode is used  */
    STLAYER_IQI_PeakingFreq_0_15_FsDiv2 = 0,    /* Correspond to 1.0MHZ   for 1H signal   (1.0/6.75 = 0.15) */
    STLAYER_IQI_PeakingFreq_0_18_FsDiv2,        /* Correspond to 1.25MHZ  for 1H signal   (1.25/6.75 = 0.18) */
    STLAYER_IQI_PeakingFreq_0_22_FsDiv2,        /* Correspond to 1.5MHZ   for 1H signal   (1.5/6.75 = 0.22) */
    STLAYER_IQI_PeakingFreq_0_26_FsDiv2,        /* Correspond to 1.75MHZ  for 1H signal   (1.75/6.75 = 0.26) */
    /* for these frequencies extended mode is NOT set */
    STLAYER_IQI_PeakingFreq_0_30_FsDiv2,        /* Correspond to 2.0MHZ   for 1H signal   (2.0/6.75 = 0.30) */
    STLAYER_IQI_PeakingFreq_0_34_FsDiv2,        /* Correspond to 2.25MHZ  for 1H signal   (2.25/6.75 = 0.33) */
    STLAYER_IQI_PeakingFreq_0_37_FsDiv2,        /* Correspond to 2.5MHZ   for 1H signal   (2.5/6.75 = 0.37)*/
    STLAYER_IQI_PeakingFreq_0_40_FsDiv2,        /* Correspond to 2.75MHZ  for 1H signal   (2.75/6.75 = 0.40) */
    STLAYER_IQI_PeakingFreq_0_44_FsDiv2,        /* Correspond to 3.0MHZ   for 1H signal   (3.0/6.75 = 0.44)*/
    STLAYER_IQI_PeakingFreq_0_48_FsDiv2,        /* Correspond to 3.25MHZ  for 1H signal   (3.25/6.75 = 0.48) */
    STLAYER_IQI_PeakingFreq_0_51_FsDiv2,        /* Correspond to 3.5MHZ   for 1H signal   (3.5/6.75 = 0.51)*/
    STLAYER_IQI_PeakingFreq_0_55_FsDiv2,        /* Correspond to 3.75MHZ  for 1H signal   (3.75/6.75 = 0.55) */
    STLAYER_IQI_PeakingFreq_0_58_FsDiv2,        /* Correspond to 4.0MHZ   for 1H signal   (4.0/6.75 = 0.58)*/
    STLAYER_IQI_PeakingFreq_0_63_FsDiv2,        /* Correspond to 4.25MHZ  for 1H signal   (4.25/6.75 = 0.63)*/
    STLAYER_IQI_PeakingFreq_Nb                  /* for array usage */
} STLAYER_IQI_PeakingFrequency_t;

typedef enum
{
    STLAYER_IQI_PeakingNoclipping=0,           /* No clipping  */
    STLAYER_IQI_PeakingSoftClipping1=1,        /* -3dB soft clipping */
    STLAYER_IQI_PeakingSoftClipping2=2,        /* -6dB soft clipping  */
    STLAYER_IQI_PeakingClippingCurvesNumber=3  /* Number of clipping curves   */
} STLAYER_IQI_PeakingClippingParameters_t;

 typedef enum
{
    STLAYER_IQI_PeakingGain_6_0db_neg=0,       /* Correspond to -6.0 db  */
    STLAYER_IQI_PeakingGain_5_5db_neg=1,       /* Correspond to -5.5 db  */
    STLAYER_IQI_PeakingGain_5_0db_neg=2,       /* Correspond to -5.0 db  */
    STLAYER_IQI_PeakingGain_4_5db_neg=3,       /* Correspond to -4.5 db  */
    STLAYER_IQI_PeakingGain_4_0db_neg=4,       /* Correspond to -4.0 db  */
    STLAYER_IQI_PeakingGain_3_5db_neg=5,       /* Correspond to -3.5 db  */
    STLAYER_IQI_PeakingGain_3_0db_neg=6,       /* Correspond to -3.0 db  */
    STLAYER_IQI_PeakingGain_2_5db_neg=7,       /* Correspond to -2.5 db  */
    STLAYER_IQI_PeakingGain_2_0db_neg=8,       /* Correspond to -2.0 db  */
    STLAYER_IQI_PeakingGain_1_5db_neg=9,       /* Correspond to -1.5 db  */
    STLAYER_IQI_PeakingGain_1_0db_neg=10,      /* Correspond to -1.0 db  */
    STLAYER_IQI_PeakingGain_0_5db_neg=11,      /* Correspond to -0.5 db  */
    STLAYER_IQI_PeakingGain_0_0db_pos=12,      /* Correspond to +0.0 db  */
    STLAYER_IQI_PeakingGain_0_5db_pos=13,      /* Correspond to +0.5 db  */
    STLAYER_IQI_PeakingGain_1_0db_pos=14,      /* Correspond to +1.0 db  */
    STLAYER_IQI_PeakingGain_1_5db_pos=15,      /* Correspond to +1.5 db  */
    STLAYER_IQI_PeakingGain_2_0db_pos=16,      /* Correspond to +2.0 db  */
    STLAYER_IQI_PeakingGain_2_5db_pos=17,      /* Correspond to +2.5 db  */
    STLAYER_IQI_PeakingGain_3_0db_pos=18,      /* Correspond to +3.0 db  */
    STLAYER_IQI_PeakingGain_3_5db_pos=19,      /* Correspond to +3.5 db  */
    STLAYER_IQI_PeakingGain_4_0db_pos=20,      /* Correspond to +4.0 db  */
    STLAYER_IQI_PeakingGain_4_5db_pos=21,      /* Correspond to +4.5 db  */
    STLAYER_IQI_PeakingGain_5_0db_pos=22,      /* Correspond to +5.0 db  */
    STLAYER_IQI_PeakingGain_5_5db_pos=23,      /* Correspond to +5.5 db  */
    STLAYER_IQI_PeakingGain_6_0db_pos=24,      /* Correspond to +6.0 db  */
    STLAYER_IQI_PeakingGain_6_5db_pos=25,      /* Correspond to +6.5 db  */
    STLAYER_IQI_PeakingGain_7_0db_pos=26,      /* Correspond to +7.0 db  */
    STLAYER_IQI_PeakingGain_7_5db_pos=27,      /* Correspond to +7.5 db  */
    STLAYER_IQI_PeakingGain_8_0db_pos=28,      /* Correspond to +8.0 db  */
    STLAYER_IQI_PeakingGain_8_5db_pos=29,      /* Correspond to +8.5 db  */
    STLAYER_IQI_PeakingGain_9_0db_pos=30,      /* Correspond to +9.0 db  */
    STLAYER_IQI_PeakingGain_9_5db_pos=31,      /* Correspond to +9.5 db  */
    STLAYER_IQI_PeakingGain_10_0db_pos=32,     /* Correspond to +10.0 db  */
    STLAYER_IQI_PeakingGain_10_5db_pos=33,     /* Correspond to +10.5 db  */
    STLAYER_IQI_PeakingGain_11_0db_pos=34,     /* Correspond to +11.0 db  */
    STLAYER_IQI_PeakingGain_11_5db_pos=35,     /* Correspond to +11.5 db  */
    STLAYER_IQI_PeakingGain_12_0db_pos=36,     /* Correspond to +12.0 db  */
    STLAYER_IQI_PeakingGainMax
} STLAYER_IQI_PeakingGain_t;

typedef enum
{
    STLAYER_IQI_PeakingFactor_1_00=0,          /* Correspond to 1.0  (Over and Under shoot factor) */
    STLAYER_IQI_PeakingFactor_0_75=1,          /* Correspond to 0.75 (Over and Under shoot factor) */
    STLAYER_IQI_PeakingFactor_0_50=2,          /* Correspond to 0.50 (Over and Under shoot factor) */
    STLAYER_IQI_PeakingFactor_0_25=3           /* Correspond to 0.25 (Over and Under shoot factor) */
} STLAYER_IQI_PeakingFactor_t;


typedef enum
{
    STLAYER_IQI_LtiSelectiveEdgeOff=0,         /* selective Edge is off */
    STLAYER_IQI_LtiSelectiveEdgeMin=1,         /* selective Edge set to min */
    STLAYER_IQI_LtiSelectiveEdgeMed=2,         /* selective Edge set to Medium */
    STLAYER_IQI_LtiSelectiveEdgeMax=3          /* selective Edge set to max */
} STLAYER_IQI_LtiSelectiveEdge_t;

typedef enum
{
    STLAYER_IQI_LtiPreFilterOff=0,             /* HMMS (Horizontal Min-Max Search) prefilter is off */
    STLAYER_IQI_LtiWeakPreFiltering=1,         /* HMMS Prefilter is in Weak mode */
    STLAYER_IQI_LtiStrongPreFilter=2           /* HMMS Prefilter is in strong mode */
} STLAYER_IQI_LtiEdgePrefilter_t;

typedef struct STLAYER_IQI_DemoParameters_s
{
    BOOL IsLTIDemoEnabled;         /* If TRUE the Luma Transient Improvement Demo feature is enabled*/
    BOOL IsCTIDemoEnabled;         /* If TRUE the Chroma Enhancement Demo feature is enabled*/
    BOOL IsLEDemoEnabled;          /* If TRUE the Luma Enhancement Demo feature is enabled*/
    BOOL IsPeakingDemoEnabled;     /* If TRUE the peaking Demo feature is enabled*/
    U16  DemoWindowStartPercent;   /* First active pixel in the demo window */
    U16  DemoWindowEndPercent;     /* Last active pixel in the demo window */
} STLAYER_IQI_DemoParameters_t;

typedef struct STLAYER_IQI_PeakingParameters_s
{
    /* HV STLAYER_IQI_Peaking */
    STLAYER_IQI_PeakingGain_t HorGainBandPass; /* Horizontal gain in dB in the range [-6, +12 ] for the bandpass filter step is 0.5 db*/
    STLAYER_IQI_PeakingGain_t HorGainHighPass; /* Horizontal gain in dB in the range [-6, +12 ] for the highpass filter step is 0.5 db*/
    STLAYER_IQI_PeakingGain_t VerticalGain;    /* Vertical gain in dB in the range [-2.5, +7.5]  */
    BOOL  IsVerticalPeakingEnabled;            /* Vertical peaking enabled(1)/disabled(0) */
    U8    CoringLevel;                         /* peaking coring value : applies to both V&H. 0 to 63*/
    BOOL  IsCoringModeChromaAdaptative;        /* If TRUE the peaking is done in chroma adaptive coring mode else it's done in manual coring mode */
    STLAYER_IQI_PeakingFactor_t    Overshoot;  /* Overshoot factor  */
    STLAYER_IQI_PeakingFactor_t    Undershoot; /* Undershoot factor */
    STLAYER_IQI_PeakingClippingParameters_t    ClippingMode;         /* Iqi peaking clipping mode */
    STLAYER_IQI_PeakingFrequency_t    BandPassFilterCenterFrequency; /* peaking filter center frequency */
    STLAYER_IQI_PeakingFrequency_t    HighPassFilterCutoffFrequency; /* peaking filter cutoff frequency */
} STLAYER_IQI_PeakingParameters_t;

typedef struct STLAYER_IQI_LEFixedCurve_s
{
    U16 BlackStretchInflexionPoint;  /* Black Stretch Inflexion Point (10 bits : 0..1023)*/
    U8  BlackStretchGain;            /* Black Stretch Gain            (% : 0..100) */
    U16 BlackStretchLimitPoint;      /* Black Stretch Limit Point (10 bits : 0..1023) */
    U16 WhiteStretchInflexionPoint;  /* White Stretch Inflexion Point (10 bits : 0..1023)*/
    U8  WhiteStretchGain;            /* White Stretch Gain            (% : 0..100)*/
    U16 WhiteStretchLimitPoint;      /* White Stretch Limit Point (10 bits : 0..1023) */
} STLAYER_IQI_LEFixedCurve_t;

typedef struct STLAYER_IQI_LEParameters_s
{
    U8    CSCGain;                 /* Chroma Saturation Compensation Gain (From 0 to 31) */
    BOOL  IsFixedCurveEnabled;     /* If TRUE the contrast Enhancer Fixed Curve  is enabled */
    STLAYER_IQI_LEFixedCurve_t  FixedCurveParams; /* Fixed Curve params for Contrast Enhancer */
    BOOL  IsCustomCurveEnabled;    /* If TRUE the contrast Enhancer custom curve  is enabled */
    const S16  *CustomCurveData_p; /* Custom Curve data pointer for Contrast Enhancer. Size of data is 128 elements. */
} STLAYER_IQI_LEParameters_t;

typedef struct STLAYER_IQI_LTIParameters_s
{
    STLAYER_IQI_LtiSelectiveEdge_t SelectiveEdgeMode; /* Selective edge Mode*/
    BOOL      IsAntiAliasingEnabled; /* Anti aliasing enabled(1)/disabled(0) */
    STLAYER_IQI_LtiEdgePrefilter_t EdgePrefilterMode; /* Pre filter Mode*/
    BOOL      IsVerticalLtiEnabled;  /* Vertical lti enabled(1)/disabled(0) */
    U8        HorLtiStrength;        /* Horizontal lti strength [0, 31] */
    U8        VerLtiStrength;        /* Vertical lti strength [0, 15]*/
} STLAYER_IQI_LTIParameters_t;

typedef struct STLAYER_IQI_CTIParameters_s
{
    U8    CtiStrength1;      /* Value of cti strength1 [0, 3] */
    U8    CtiStrength2;      /* Value of cti strength1 [0, 3] */
} STLAYER_IQI_CTIParameters_t;

/*****************************End of IQI*********************************************/

/* Video filters overall manual parameters.         */
typedef union STLAYER_VideoFilteringParameters_s
{
    STLAYER_AutoFleshParameters_t       AutoFleshParameters;
    STLAYER_GreenBoostParameters_t      GreenBoostParameters;
    STLAYER_TintParameters_t            TintParameters;
    STLAYER_SatParameters_t             SatParameters;
    STLAYER_EdgeReplacementParameters_t EdgeReplacementParameters;
    STLAYER_PeakingParameters_t         PeakingParameters;
    STLAYER_DCIParameters_t             DCIParameters;
    STLAYER_BrightnessContrastParameters_t              BCParameters;
    STLAYER_ZoomFilterSetParameters_t   ZoomFilterSetParameters;

    STLAYER_IQI_LTIParameters_t         LTIParameters;
    STLAYER_IQI_CTIParameters_t         CTIParameters;
    STLAYER_IQI_LEParameters_t          LEParameters;
    STLAYER_IQI_PeakingParameters_t     IQIPeakingParameters;
    STLAYER_IQI_DemoParameters_t        IQIDemoParameters;

} STLAYER_VideoFilteringParameters_t;

typedef struct STLAYER_PSI_s
{
  STLAYER_VideoFiltering_t              VideoFiltering;
  STLAYER_VideoFilteringControl_t       VideoFilteringControl;
  STLAYER_VideoFilteringParameters_t    VideoFilteringParameters;

} STLAYER_PSI_t;

typedef struct
{
    U16     NumberOfZone;           /* Number of zone to consider (according  */
                                    /* to hardware constraints).              */
    U16 *   SizeOfZone_p;           /* Size of zone in percent unit of output */
                                    /* window.                                */
    U16     EffectControl;          /* In percent unit of max possible effect.*/

} STLAYER_SpectacleModeParams_t;

typedef union
{
    STLAYER_SpectacleModeParams_t   SpectacleModeParams;    /* CF above.      */
} STLAYER_OutputWindowSpecialModeParams_t;


typedef struct STLAYER_TermParams_s
{
    BOOL ForceTerminate;
} STLAYER_TermParams_t;

typedef struct STLAYER_QualityOptimizations_s
{
    BOOL DoForceStartOnEvenLine;
    BOOL DoNotRescaleForZoomCloseToUnity;
} STLAYER_QualityOptimizations_t;


typedef struct STLAYER_ViewPortParams_s
{
    STLAYER_ViewPortSource_t *  Source_p;
    STGXOBJ_Rectangle_t         InputRectangle;
    STGXOBJ_Rectangle_t         OutputRectangle;
} STLAYER_ViewPortParams_t;


typedef struct STLAYER_ViewPortLayerInfo_s
{
    STLAYER_Layer_t             LayerType;
    ST_DeviceName_t             LayerName;
} STLAYER_ViewPortLayerInfo_t;


typedef struct STLAYER_FMDParams_s
{
    U32 SceneChangeDetectionThreshold; /* SCD threshold used to define SceneCount. This is a threshold of blocks pixels differences sum. */
                                       /* Above this threshold, a block is considered as moving*/
    U32 RepeatFieldDetectionThreshold; /* RFD threshold used to define RepeatStatus. This is a threshold of blocks pixels differences sum.*/
                                       /* Above this threshold, a block is considered as moving*/
    U32 PixelMoveDetectionThreshold; /* Pixels are said moving pixels when their inter-frame (|A-D|) difference is superior to PixelMoveDetectionThreshold.*/
    U32 BlockMoveDetectionThreshold; /* A block is said moving if it contains more than BlockMoveDetectionThreshold moving pixels. */
                                     /* MoveStatus bit is set if at least one block is moving block.*/
    U32 CFDThreshold;   /* CFD pixels differences that are inferior to CFDThreshold are considered as noise so not taken into account. */
} STLAYER_FMDParams_t;

typedef struct STLAYER_FMDResults_s
{
    U32 PictureIndex;   /* Index of the current displayed picture. Note that it's not the picture number found during decode */
    U32 FieldRepeatCount; /* Gives the number of repeated fields since previous FMD event */
    U32 FieldSum;       /* Gives the motion between current and previous same polarity field (N-2 field). Inter-frame motion */
    U32 CFDSum;         /* Gives the motion between current and previous Fields. Inter-field motion  */
    U32  SceneCount;    /* Number of blocks that have their inter-frame pixels differences |A-D| sum superior to threshold SceneChangeDetectionThreshold */
    BOOL MoveStatus;    /* This bit is set if at least one block is a moving block regarding BlockMoveDetectionThreshold.*/
    BOOL RepeatStatus;  /* This bit is set if there is no moving block regarding RepeatFieldDetectionThreshold */
} STLAYER_FMDResults_t;

typedef struct STLAYER_VTGParams_s
{
    ST_DeviceName_t             VTGName;
    /* ... Valid if UpdateReason has STLAYER_VTG_REASON set */
    U32                         VTGFrameRate;
    /* ... Valid if UpdateReason has STLAYER_VTG_REASON set */
} STLAYER_VTGParams_t;

typedef struct STLAYER_UpdateParams_s
{
    STLAYER_UpdateReason_t      UpdateReason;
    STLAYER_LayerParams_t *     LayerParams_p;
    /* ... Valid if UpdateReason has STLAYER_LAYER_PARAMS_REASON set */
    STLAYER_VTGParams_t         VTGParams;
    /* ... Valid if UpdateReason has STLAYER_VTG_REASON set */
    ST_DeviceName_t             VTG_Name;
    /* ... REDUNDANT, kept only for backwards compatibility...  */
    /* ... Valid if UpdateReason has STLAYER_VTG_REASON set     */
    BOOL                        IsDecimationNeeded;
    /* ... Valid if UpdateReason has STLAYER_DECIMATION_NEED_REASON set */
    STLAYER_DecimationFactor_t  RecommendedVerticalDecimation;
    /* ... Valid if UpdateReason has STLAYER_DECIMATION_NEED_REASON set */
    STLAYER_DecimationFactor_t  RecommendedHorizontalDecimation;
    /* ... Valid if UpdateReason has STLAYER_DECIMATION_NEED_REASON set */
    U32                         StreamWidth;
    /* ... Valid if UpdateReason has STLAYER_DECIMATION_NEED_REASON set */
    U32                         StreamHeight;
    /* ... Valid if UpdateReason has STLAYER_DECIMATION_NEED_REASON set */
    struct
    {
        U32                      FrameBufferDisplayLatency; /* Number of VSyncs between video commit and framebuffer display */
        U32                      FrameBufferHoldTime;       /* Number of VSyncs that the display needs the framebuffer
                                                             * is kept unchanged for nodes generation and composition */
    } DisplayParamsForVideo;
    /* ... Valid if UpdateReason has STLAYER_CONNECT_REASON set */
} STLAYER_UpdateParams_t;

#ifdef ST_OSLINUX

#define LAYER_PARTITION_AVMEM         0         /* Which partition to use for allocating in AVMEM memory */
#define LAYER_SECURED_PARTITION_AVMEM           2         /* Which partition to use for allocating in SECURED AVMEM memory */


typedef struct STLAYER_AllocDataParams_s
{
    U32     Size;
    U32     Alignment;
} STLAYER_AllocDataParams_t;

ST_ErrorCode_t STLAYER_MapAddress( void *KernelAddress_p, void **UserAddress_p );
ST_ErrorCode_t STLAYER_AllocData( STLAYER_Handle_t  LayerHandle, STLAYER_AllocDataParams_t *Params_p, void **Address_p );
ST_ErrorCode_t STLAYER_AllocDataSecure( STLAYER_Handle_t  LayerHandle, STLAYER_AllocDataParams_t *Params_p, void **Address_p );
ST_ErrorCode_t STLAYER_FreeData( STLAYER_Handle_t  LayerHandle, void *Address_p );

U32 *STLAYER_UserToKernel( U32 VirtUserAddress_p );
U32 *STLAYER_KernelToUser( U32 VirtKernelAddress_p );

#endif /* ST_OSLINUX */

/* Exported Functions ------------------------------------------------------- */

/******************************************************************************/
/* STLAYER Public API ------------------------------------------------------- */
/******************************************************************************/

ST_ErrorCode_t STLAYER_GetCapability(const ST_DeviceName_t DeviceName,
                                     STLAYER_Capability_t * const Capability);

ST_ErrorCode_t STLAYER_GetInitAllocParams(STLAYER_Layer_t LayerType,
                                          U32             ViewPortsNumber,
                                          STLAYER_AllocParams_t * Params);

ST_Revision_t  STLAYER_GetRevision(void);

ST_ErrorCode_t STLAYER_Init(const ST_DeviceName_t        DeviceName,
                            const STLAYER_InitParams_t * const InitParams_p);

ST_ErrorCode_t STLAYER_Term(const ST_DeviceName_t        DeviceName,
                            const STLAYER_TermParams_t * const TermParams_p);

ST_ErrorCode_t STLAYER_Open(const ST_DeviceName_t       DeviceName,
                            const STLAYER_OpenParams_t * const Params,
                            STLAYER_Handle_t *    Handle);

ST_ErrorCode_t STLAYER_Close(STLAYER_Handle_t  Handle);

ST_ErrorCode_t STLAYER_GetLayerParams(STLAYER_Handle_t  Handle,
        STLAYER_LayerParams_t *  LayerParams_p);

ST_ErrorCode_t STLAYER_SetLayerParams(STLAYER_Handle_t  Handle,
        STLAYER_LayerParams_t *  LayerParams_p);

/* ViewPort functions */
ST_ErrorCode_t STLAYER_OpenViewPort(
  STLAYER_Handle_t            LayerHandle,
  STLAYER_ViewPortParams_t*   Params,
  STLAYER_ViewPortHandle_t*   VPHandle);

ST_ErrorCode_t STLAYER_CloseViewPort(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_EnableViewPort(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_DisableViewPort(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_AdjustViewPortParams(
  STLAYER_Handle_t LayerHandle,
  STLAYER_ViewPortParams_t*   Params_p
);

ST_ErrorCode_t STLAYER_GetViewPortLayerInformation(
  const STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_ViewPortLayerInfo_t *   ViewPortLayerInfo_p);

ST_ErrorCode_t STLAYER_SetViewPortParams(
  const STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_ViewPortParams_t*       Params_p
);

ST_ErrorCode_t STLAYER_GetViewPortParams(
  const STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_ViewPortParams_t*       Params_p
);

ST_ErrorCode_t STLAYER_SetViewPortSource(
  STLAYER_ViewPortHandle_t    VPHandle,
  STLAYER_ViewPortSource_t*   VPSource
);

ST_ErrorCode_t STLAYER_GetViewPortSource(
  STLAYER_ViewPortHandle_t    VPHandle,
  STLAYER_ViewPortSource_t*   VPSource
);

ST_ErrorCode_t STLAYER_SetViewPortIORectangle(
  STLAYER_ViewPortHandle_t    VPHandle,
  STGXOBJ_Rectangle_t*        InputRectangle,
  STGXOBJ_Rectangle_t*        OutputRectangle
);

ST_ErrorCode_t STLAYER_AdjustIORectangle(
  STLAYER_Handle_t       Handle,
  STGXOBJ_Rectangle_t*   InputRectangle,
  STGXOBJ_Rectangle_t*   OutputRectangle
);

ST_ErrorCode_t STLAYER_GetViewPortIORectangle(
  STLAYER_ViewPortHandle_t  VPHandle,
  STGXOBJ_Rectangle_t*      InputRectangle,
  STGXOBJ_Rectangle_t*      OutputRectangle
);

ST_ErrorCode_t STLAYER_SetViewPortPosition(
  STLAYER_ViewPortHandle_t VPHandle,
  S32                      XPosition,
  S32                      YPosition
);

ST_ErrorCode_t STLAYER_GetViewPortPosition(
  STLAYER_ViewPortHandle_t VPHandle,
  S32*                     XPosition,
  S32*                     YPosition
);

ST_ErrorCode_t STLAYER_DisableColorKey(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_EnableColorKey(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_SetViewPortColorKey(
  STLAYER_ViewPortHandle_t    VPHandle,
  STGXOBJ_ColorKey_t*         ColorKey
);

ST_ErrorCode_t STLAYER_GetViewPortColorKey(
  STLAYER_ViewPortHandle_t    VPHandle,
  STGXOBJ_ColorKey_t*         ColorKey
);

ST_ErrorCode_t STLAYER_DisableBorderAlpha(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_EnableBorderAlpha(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_GetViewPortAlpha(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_GlobalAlpha_t*    Alpha
);

ST_ErrorCode_t STLAYER_SetViewPortAlpha(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_GlobalAlpha_t*    Alpha
);

ST_ErrorCode_t STLAYER_SetViewPortGain(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_GainParams_t*     Params
);

ST_ErrorCode_t STLAYER_GetViewPortGain(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_GainParams_t*     Params
);

#ifdef ST_VALIDATION
ST_ErrorCode_t STLAYER_EnablePremultipliedFormat(
    STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_DisablePremultipliedFormat(
    STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_SetViewPortEnableForceIgnore(
    STLAYER_ViewPortHandle_t VPHandle,
    BOOL ForceON,
    BOOL IgnoreON,
    BOOL Mixer
);

ST_ErrorCode_t STLAYER_SetViewPortDisableForceIgnore(
    STLAYER_ViewPortHandle_t VPHandle,
    BOOL Mixer
);

#endif

ST_ErrorCode_t STLAYER_SetViewPortRecordable(STLAYER_ViewPortHandle_t  VPHandle,
                                             BOOL Recordable);

ST_ErrorCode_t STLAYER_GetViewPortRecordable(STLAYER_ViewPortHandle_t  VPHandle,
                                             BOOL  * Recordable_p);

ST_ErrorCode_t STLAYER_GetBitmapAllocParams(
  STLAYER_Handle_t             LayerHandle,
  STGXOBJ_Bitmap_t*            Bitmap_p,
  STGXOBJ_BitmapAllocParams_t* Params1_p,
  STGXOBJ_BitmapAllocParams_t* Params2_p
);

ST_ErrorCode_t STLAYER_GetBitmapHeaderSize(
  STLAYER_Handle_t             LayerHandle,
  STGXOBJ_Bitmap_t*            Bitmap_p,
  U32 *                        HeaderSize_p
);

ST_ErrorCode_t STLAYER_GetPaletteAllocParams(
  STLAYER_Handle_t               LayerHandle,
  STGXOBJ_Palette_t*             Palette_p,
  STGXOBJ_PaletteAllocParams_t*  Params_p
);

ST_ErrorCode_t STLAYER_GetVTGName(
  STLAYER_Handle_t               LayerHandle,
  ST_DeviceName_t * const        VTGName_p
);

ST_ErrorCode_t STLAYER_DisableViewPortFilter(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_EnableViewPortFilter(STLAYER_ViewPortHandle_t VPHandle,
                                            STLAYER_Handle_t FilterHandle);

ST_ErrorCode_t STLAYER_AttachAlphaViewPort(STLAYER_ViewPortHandle_t VPHandle,
                                           STLAYER_Handle_t MaskedLayer);

ST_ErrorCode_t STLAYER_GetViewPortFlickerFilterMode(
    STLAYER_ViewPortHandle_t          VPHandle,
    STLAYER_FlickerFilterMode_t*      FlickerFilterMode_p);

ST_ErrorCode_t STLAYER_SetViewPortFlickerFilterMode(
    STLAYER_ViewPortHandle_t          VPHandle,
    STLAYER_FlickerFilterMode_t       FlickerFilterMode);

ST_ErrorCode_t STLAYER_DisableViewportColorFill(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_EnableViewportColorFill(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_SetViewportColorFill(
  STLAYER_ViewPortHandle_t    VPHandle,
  STGXOBJ_ColorARGB_t*         ColorFill
);

ST_ErrorCode_t STLAYER_GetViewportColorFill(
  STLAYER_ViewPortHandle_t    VPHandle,
  STGXOBJ_ColorARGB_t*         ColorFill
);



/*******************************************************************************/
/* Internal functions used only by other STAPI Drivers (STVID, STVMIX...) ---- */
/*******************************************************************************/

ST_ErrorCode_t STLAYER_UpdateFromMixer(
  STLAYER_Handle_t              LayerHandle,
  STLAYER_OutputParams_t *      OutputParams_p
);

ST_ErrorCode_t STLAYER_SetViewPortCompositionRecurrence(STLAYER_ViewPortHandle_t        VPHandle,
                                                  const STLAYER_CompositionRecurrence_t CompositionRecurrence);

ST_ErrorCode_t STLAYER_PerformViewPortComposition(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_InformPictureToBeDecoded(
  const STLAYER_ViewPortHandle_t        VPHandle,
  STGXOBJ_PictureInfos_t*               PictureInfos_p
);

ST_ErrorCode_t STLAYER_CommitViewPortParams(STLAYER_ViewPortHandle_t VPHandle);

ST_ErrorCode_t STLAYER_GetVTGParams(
  STLAYER_Handle_t               LayerHandle,
  STLAYER_VTGParams_t * const    VTGParams_p
);

/* Access to PSI functions should only be done through STVID */
ST_ErrorCode_t STLAYER_SetViewPortPSI(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_PSI_t*            VPPSI_p
);
ST_ErrorCode_t STLAYER_GetViewPortPSI(
  STLAYER_ViewPortHandle_t  VPHandle,
  STLAYER_PSI_t*            VPPSI_p
);

/* Access to ViewPortSpecialMode functions should only be done through STVID */
ST_ErrorCode_t STLAYER_SetViewPortSpecialMode (
  const STLAYER_ViewPortHandle_t                  VPHandle,
  const STLAYER_OutputMode_t                      OuputMode,
  const STLAYER_OutputWindowSpecialModeParams_t * const Params_p);
ST_ErrorCode_t STLAYER_GetViewPortSpecialMode (
  const STLAYER_ViewPortHandle_t                  VPHandle,
  STLAYER_OutputMode_t                    * const OuputMode_p,
  STLAYER_OutputWindowSpecialModeParams_t * const Params_p);


/******************************************************************************
 * Un-supported extensions (debug only) ---------------------------------------

   WARNING (do not remove)
   =======
   Every user of this code must be made aware of the following limitations.
   This section's API's are extensions to the STLAYER API, for debugging and testing
   purposes only. They are not part of the STLAYER product delivery, they don't
   contain driver features used in production. They are not compiled by default.
   They may change without notice. They may contain errors, are not tested. They
   may be not supported on some platforms, may change the driver normal behaviour.
   Application code such as reference software or customer application may use
   them at their own risks and not for production. There will be no support.
 ******************************************************************************/

typedef enum
{
    VHSRC_FILTER_NONE,
    VHSRC_FILTER_A,
    VHSRC_FILTER_B,
    VHSRC_FILTER_C,
    VHSRC_FILTER_D,
    VHSRC_FILTER_E,
    VHSRC_FILTER_F,
    VHSRC_FILTER_G,
    VHSRC_FILTER_H,
    VHSRC_FILTER_I,
    VHSRC_FILTER_J,
    VHSRC_FILTER_K,
    VHSRC_FILTER_L,
    VHSRC_FILTER_CHRX2
} STLAYER_VHSRCFilter_t;


typedef struct STLAYER_VHSRCHorizontalParams_s
{
    U32 Increment;
    U32 InitialPhase;
    U32 PixelRepeat;
} STLAYER_VHSRCHorizontalParams_t;


typedef struct STLAYER_VHSRCVerticalParams_s
{
    U32 Increment;
    U32 InitialPhase;
    U32 LineRepeat;
} STLAYER_VHSRCVerticalParams_t;


typedef struct STLAYER_VHSRCParams_s
{
   STLAYER_VHSRCHorizontalParams_t LumaHorizontal;
   STLAYER_VHSRCHorizontalParams_t ChromaHorizontal;
   STLAYER_VHSRCVerticalParams_t   LumaTop;
   STLAYER_VHSRCVerticalParams_t   ChromaTop;
   STLAYER_VHSRCVerticalParams_t   LumaBot;
   STLAYER_VHSRCVerticalParams_t   ChromaBot;
} STLAYER_VHSRCParams_t;


typedef struct STLAYER_VHSRCVideoDisplayFilters_s
{
    STLAYER_ZoomFilterSet_t        CurrentFilterSet;
    STLAYER_VHSRCFilter_t          LumaHorizontalCoefType;
    STLAYER_VHSRCFilter_t          ChromaHorizontalCoefType;
    STLAYER_VHSRCFilter_t          LumaVerticalCoefType;
    STLAYER_VHSRCFilter_t          ChromaVerticalCoefType;
} STLAYER_VHSRCVideoDisplayFilters_t;

typedef struct STLAYER_MemoryStacks_s
{
    /* Luma stacks */
    U32 YF_STACK_l0;
    U32 YF_STACK_l1;
    U32 YF_STACK_l2;
    U32 YF_STACK_l3;
    U32 YF_STACK_l4;
    U32 YF_STACK_p0;
    U32 YF_STACK_p1;
    U32 YF_STACK_p2;
    /* Chroma stacks */
    U32 CF_STACK_l0;
    U32 CF_STACK_l1;
    U32 CF_STACK_l2;
    U32 CF_STACK_l3;
    U32 CF_STACK_l4;
    U32 CF_STACK_p0;
    U32 CF_STACK_p1;
    U32 CF_STACK_p2;
    /* Motion Buffers stacks */
    U32 MF_STACK_l0;
    U32 MF_STACK_p0;
} STLAYER_MemoryStacks_t;

typedef struct STLAYER_FmdData_s
{
    U32 FieldSum;       /* Motion between previous and next Fields. Inter-frame motion */
    U32 CFDSum;         /* Motion between current and next Fields. Inter-field motion  */
} STLAYER_FmdData_t;

typedef enum STLAYER_DisplayParamsMode_e
{
    STLAYER_DISPLAYPARAMS_AUTO,
    STLAYER_DISPLAYPARAMS_STATIC_MANUAL,  /* Static params are specified. Dynamic parameters are still automatically computed (ex: video buffer addresses)*/
    STLAYER_DISPLAYPARAMS_FULL_MANUAL   /* All params are manually specified even buffer pointers */
} STLAYER_DisplayParamsMode_t;

typedef enum STLAYER_DisplayType_e
{
    STLAYER_SDDISPO2_DISPLAY,
    STLAYER_VDP_DISPLAY,
    STLAYER_VIRTUAL_DISPLAY,
    STLAYER_HQVDP_DISPLAY
} STLAYER_DisplayType_t;

typedef struct STLAYER_SDDISPO2_VideoDisplayParams_s
{
    STLAYER_VHSRCParams_t                   VHSRCparams;
    STLAYER_VHSRCVideoDisplayFilters_t      VHSRCVideoDisplayFilters;
} STLAYER_SDDISPO2_VideoDisplayParams_t;

typedef struct STLAYER_VDP_VideoDisplayParams_s
{
    STLAYER_VHSRCParams_t                   VHSRCparams;
    STLAYER_VHSRCVideoDisplayFilters_t      VHSRCVideoDisplayFilters;
    STLAYER_MemoryStacks_t                  Stacks;
    STLAYER_FmdData_t                       FmdData;
} STLAYER_VDP_VideoDisplayParams_t;

typedef struct STLAYER_HQVDP_VideoDisplayParams_s
{
    struct
    {
        U32 Config;             /* Only TOP_NOT_BOT bit is dynamic */
        U32 MemFormat;          /* Dynamic */
        U32 CurrentLuma;        /* Dynamic */
        U32 CurrentChroma;      /* Dynamic */
        U32 OutputData;         /* Dynamic */
        U32 LumaSrcPitch;       /* Dynamic */
        U32 ChromaSrcPitch;     /* Dynamic */
        U32 ProcessedPitch;     /* Dynamic */
        U32 InputFrameSize;     /* Dynamic */
        U32 InputViewportOri;   /* Dynamic */
        U32 InputViewportSize;  /* Dynamic */
        U32 CrcResetCtrl;       /* Dynamic */
    } Top;
    struct
    {
        U32 CtrlPrvCsdi;
        U32 CtrlCurCsdi;
        U32 CtrlNxtCsdi;
        U32 CtrlCurFmd;
        U32 CtrlNxtFmd;
    } Vc1re;
    struct
    {
        U32 Config;
        U32 ViewportOri;        /* Dynamic */
        U32 ViewportSize;       /* Dynamic */
        U32 NextNextLuma;       /* Dynamic */
        U32 NextNextNextLuma;   /* Dynamic */
        U32 ThresholdScd;
        U32 ThresholdRfd;
        U32 ThresholdMove;
        U32 ThresholdCfd;
    } Fmd;
    struct
    {
        U32 Config;
        U32 PrevLuma;           /* Dynamic */
        U32 NextLuma;           /* Dynamic */
        U32 PrevChroma;         /* Dynamic */
        U32 NextChroma;         /* Dynamic */
        U32 PrevMotion;         /* Dynamic */
        U32 CurMotion;          /* Dynamic */
        U32 NextMotion;         /* Dynamic */
        U32 LfmMap;             /* Dynamic */
    } Csdi;
    struct
    {
        U32 HorPanoramicCtrl;
        U32 OutputPictureSize;
        U32 InitPhaseCtrl;
        U32 FilteringCtrl;
        U32 ParamCtrl;
    } Hqr;
    struct
    {
        U32 Config;
        U32 DemoWindSize;
        U32 PkConfig;
        U32 Coeff0Coeff1;
        U32 Coeff2Coeff3;
        U32 Coeff4;
        U32 PkLut;
        U32 PkGain;
        U32 PkCoringLevel;
        U32 LtiConfig;
        U32 LtiThreshold;
        U32 LtiDeltaSlope;
        U32 CtiConfig;
        U32 LeConfig;
        U32 LeLut[64];          /* Dynamic */
        U32 ConBri;
        U32 SatGain;
        U32 PxfConf;
        U32 DefaultColor;
    } Iqi;

    /* Status (read only) */
    struct
    {
        U32 InputYCrc;
        U32 InputUvCrc;
    } TopStatus;
    struct
    {
        U32 FmdRepeatMoveStatus;
        U32 FmdSceneCountStatus;
        U32 CfdSum;
        U32 FieldSum;
        U32 NextYFmdCrc;
        U32 NextNextYFmdCrc;
        U32 NextNextNextYFmdCrc;
    } FmdStatus;
    struct
    {
        U32 PrevYCsdiCrc;
        U32 CurYCsdiCrc;
        U32 NextYCsdiCrc;
        U32 PrevUvCsdiCrc;
        U32 CurUvCsdiCrc;
        U32 NextUvCsdiCrc;
        U32 YCsdiCrc;
        U32 UvCsdiCrc;
        U32 UvCupCrc;
        U32 MotCsdiCrc;
    } CsdiStatus;
    struct
    {
        U32 YHqrCrc;
        U32 UHqrCrc;
        U32 VHqrCrc;
    } HqrStatus;
    struct
    {
        U32 PxfItStatus;
        U32 YIqiCrc;
        U32 UIqiCrc;
        U32 VIqiCrc;
    } IqiStatus;
} STLAYER_HQVDP_VideoDisplayParams_t;

typedef struct STLAYER_VideoDisplayParams_s
{
    STGXOBJ_Rectangle_t                     InputRectangle;
    STGXOBJ_Rectangle_t                     OutputRectangle;
    STLAYER_DecimationFactor_t              VerticalDecimFactor;
    STLAYER_DecimationFactor_t              HorizontalDecimFactor;
    STLAYER_DeiMode_t                       DeiMode;
    STGXOBJ_ScanType_t                      SourceScanType;

    STLAYER_DisplayParamsMode_t           DisplayParamsMode;
    STLAYER_DisplayType_t                   DisplayType;
    union
    {
        STLAYER_SDDISPO2_VideoDisplayParams_t  SDDispo2Params;
        STLAYER_VDP_VideoDisplayParams_t       VDPParams;
        STLAYER_HQVDP_VideoDisplayParams_t     HQVDPParams;
   } TypeData;
} STLAYER_VideoDisplayParams_t;

typedef struct STLAYER_DisplaySettings_s
{
    STLAYER_Layer_t              LayerType;
    BOOL                         IsLayerOnMainDisplay;
    STLAYER_Field_t              PresentedField;
    BOOL                         IsVSyncTop;
    BOOL                         DoesFieldPolarityMatch;
    STLAYER_VideoDisplayParams_t VideoDisplayParams;
    STLAYER_PictureID_t          PictureID;                 /* Continuously increasing in presentation order, */
    U32                          PictureIndex;              /* Index of decoded picture since init */
    U32                          PictureSignature;          /* Signature of Current picture (computed on all the frame) */
    BOOL                         IsNewPicturePresented;     /* True if PictureID is different from previous one */
} STLAYER_DisplaySettings_t;


#ifdef STVID_DEBUG_GET_DISPLAY_PARAMS
ST_ErrorCode_t STLAYER_GetVideoDisplayParams(STLAYER_ViewPortHandle_t VPHandle,
                                    STLAYER_VideoDisplayParams_t * Params_p);
ST_ErrorCode_t STLAYER_SetVideoDisplayParams(STLAYER_ViewPortHandle_t VPHandle,
                                    const STLAYER_VideoDisplayParams_t * Params_p);
#endif /* STVID_DEBUG_GET_DISPLAY_PARAMS  */


#ifdef VIDEO_DEBUG_DEINTERLACING_MODE
ST_ErrorCode_t STLAYER_GetRequestedDeinterlacingMode(STLAYER_ViewPortHandle_t VPHandle,
                                               STLAYER_DeiMode_t * RequestedMode_p);
ST_ErrorCode_t STLAYER_SetRequestedDeinterlacingMode(STLAYER_ViewPortHandle_t VPHandle,
                                               STLAYER_DeiMode_t RequestedMode);
#endif /* VIDEO_DEBUG_DEINTERLACING_MODE */


#ifdef STLAYER_USE_FMD_REPORTING
ST_ErrorCode_t STLAYER_EnableFMDReporting(STLAYER_ViewPortHandle_t VPHandle);
ST_ErrorCode_t STLAYER_DisableFMDReporting(STLAYER_ViewPortHandle_t VPHandle);
ST_ErrorCode_t STLAYER_GetFMDParams(STLAYER_ViewPortHandle_t VPHandle,
                                    STLAYER_FMDParams_t * Params_p);
ST_ErrorCode_t STLAYER_SetFMDParams(STLAYER_ViewPortHandle_t VPHandle,
                                    const STLAYER_FMDParams_t * Params_p);
#endif /* STLAYER_USE_FMD_REPORTING */


/* C++ support */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __STLAYER_H */

/* End of stlayer.h */
