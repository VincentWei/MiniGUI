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
 @File   stgxobj.h                                                             
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/
#ifndef __STGXOBJ_H
#define __STGXOBJ_H

/* Includes ----------------------------------------------------------------- */
#include "stddefs.h"
#include "stavmem.h"

#if defined(ST_OSLINUX)
#include "stos.h"
#endif


/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants ------------------------------------------------------- */
#define STGXOBJ_DRIVER_ID     159
#define STGXOBJ_DRIVER_BASE   (STGXOBJ_DRIVER_ID << 16)

typedef enum STGXOBJ_AspectRatio_e
{
  STGXOBJ_ASPECT_RATIO_16TO9 = 1, /* 0 value is reserved for STVID STVID_Setup() API */
  STGXOBJ_ASPECT_RATIO_4TO3  = 2,
  STGXOBJ_ASPECT_RATIO_221TO1= 3,
  STGXOBJ_ASPECT_RATIO_SQUARE= 4,
  STGXOBJ_ASPECT_RATIO_14TO9 = 5
} STGXOBJ_AspectRatio_t;

typedef enum STGXOBJ_BitmapType_e
{
  STGXOBJ_BITMAP_TYPE_RASTER_PROGRESSIVE,
  STGXOBJ_BITMAP_TYPE_RASTER_TOP_BOTTOM,
  STGXOBJ_BITMAP_TYPE_MB,
  STGXOBJ_BITMAP_TYPE_MB_HDPIP,
  STGXOBJ_BITMAP_TYPE_MB_TOP_BOTTOM,
  STGXOBJ_BITMAP_TYPE_MB_RANGE_MAP,
  STGXOBJ_BITMAP_TYPE_R2B_PROGRESSIVE
}STGXOBJ_BitmapType_t;

typedef enum STGXOBJ_ColorKeyType_e
{
  STGXOBJ_COLOR_KEY_TYPE_CLUT1,
  STGXOBJ_COLOR_KEY_TYPE_CLUT8,
  STGXOBJ_COLOR_KEY_TYPE_RGB888,
  STGXOBJ_COLOR_KEY_TYPE_YCbCr888_SIGNED,
  STGXOBJ_COLOR_KEY_TYPE_YCbCr888_UNSIGNED,
  STGXOBJ_COLOR_KEY_TYPE_RGB565
} STGXOBJ_ColorKeyType_t;

typedef enum STGXOBJ_ColorSpaceConversionMode_e
{
    STGXOBJ_ITU_R_BT601,
    STGXOBJ_ITU_R_BT709,
    STGXOBJ_ITU_R_BT470_2_M,
    STGXOBJ_ITU_R_BT470_2_BG,
    STGXOBJ_SMPTE_170M,
    STGXOBJ_SMPTE_240M,
    STGXOBJ_FCC,
    STGXOBJ_CONVERSION_MODE_UNKNOWN
} STGXOBJ_ColorSpaceConversionMode_t;

typedef enum STGXOBJ_ColorType_e
{
  STGXOBJ_COLOR_TYPE_ARGB8888,
  STGXOBJ_COLOR_TYPE_RGB888,
  STGXOBJ_COLOR_TYPE_ARGB8565,
  STGXOBJ_COLOR_TYPE_RGB565,
  STGXOBJ_COLOR_TYPE_ARGB1555,
  STGXOBJ_COLOR_TYPE_ARGB4444,

  STGXOBJ_COLOR_TYPE_CLUT8,
  STGXOBJ_COLOR_TYPE_CLUT4,
  STGXOBJ_COLOR_TYPE_CLUT2,
  STGXOBJ_COLOR_TYPE_CLUT1,
  STGXOBJ_COLOR_TYPE_ACLUT88,
  STGXOBJ_COLOR_TYPE_ACLUT44,

  STGXOBJ_COLOR_TYPE_SIGNED_YCBCR888_444,
  STGXOBJ_COLOR_TYPE_UNSIGNED_YCBCR888_444,
  STGXOBJ_COLOR_TYPE_SIGNED_YCBCR888_422,
  STGXOBJ_COLOR_TYPE_UNSIGNED_YCBCR888_422,
  STGXOBJ_COLOR_TYPE_SIGNED_YCBCR888_420,
  STGXOBJ_COLOR_TYPE_UNSIGNED_YCBCR888_420,
  STGXOBJ_COLOR_TYPE_UNSIGNED_AYCBCR6888_444,
  STGXOBJ_COLOR_TYPE_SIGNED_AYCBCR8888,
  STGXOBJ_COLOR_TYPE_UNSIGNED_AYCBCR8888,

  STGXOBJ_COLOR_TYPE_ALPHA1,
  STGXOBJ_COLOR_TYPE_ALPHA4,
  STGXOBJ_COLOR_TYPE_ALPHA8,
  STGXOBJ_COLOR_TYPE_BYTE,

  STGXOBJ_COLOR_TYPE_ARGB8888_255,
  STGXOBJ_COLOR_TYPE_ARGB8565_255,
  STGXOBJ_COLOR_TYPE_ACLUT88_255,
  STGXOBJ_COLOR_TYPE_ALPHA8_255

} STGXOBJ_ColorType_t;

typedef enum STGXOBJ_PaletteType_e
{
  STGXOBJ_PALETTE_TYPE_DEVICE_INDEPENDENT,
  STGXOBJ_PALETTE_TYPE_DEVICE_DEPENDENT
}STGXOBJ_PaletteType_t;

typedef enum STGXOBJ_ScanType_e
{
    STGXOBJ_PROGRESSIVE_SCAN,
    STGXOBJ_INTERLACED_SCAN
} STGXOBJ_ScanType_t;

typedef enum STGXOBJ_SubByteFormat_e
{
    STGXOBJ_SUBBYTE_FORMAT_RPIX_MSB = 0,
    STGXOBJ_SUBBYTE_FORMAT_RPIX_LSB = 1
}STGXOBJ_SubByteFormat_t;

typedef enum STGXOBJ_HardUse_e
{
    STGXOBJ_NO_HARD              = 0x00000000,
    STGXOBJ_SUBPIC_CELL          = 0x00000001,
    STGXOBJ_STILLPIC_CELL        = 0x00000002,
    STGXOBJ_OSDFRAME_CELL        = 0x00000004,
    STGXOBJ_OSDTOP_BOT_CELL      = 0x00000008,
    STGXOBJ_GAMMA_BLITTER        = 0x00000010,
    STGXOBJ_SOFT_EMU_BLITTER     = 0x00000020,
    STGXOBJ_GAMMA_GDP_PIPELINE   = 0x00000040,
    STGXOBJ_GAMMA_BKL_PIPELINE   = 0x00000080,
    STGXOBJ_GAMMA_CURS_PIPELINE  = 0x00000100
}STGXOBJ_HardUse_t;

typedef enum STGXOBJ_MPEGFrame_e
{
    STGXOBJ_MPEG_FRAME_I      = 1,
    STGXOBJ_MPEG_FRAME_P      = 2,
    STGXOBJ_MPEG_FRAME_B      = 4
} STGXOBJ_MPEGFrame_t;

typedef struct STGXOBJ_PictureInfos_s
{
    struct
    {
        STGXOBJ_MPEGFrame_t MPEGFrame;
    } VideoParams;
} STGXOBJ_PictureInfos_t;

/* Exported Types ----------------------------------------------------------- */
/* YUV_Scaling_t value define if a specific scaling factor has been applied to the values of luma and chroma of the bitmap
 in such a case the display module will be in charge to retrieve the correct value of luma and chroma before displaying the bitmap */
typedef enum YUV_Scaling_s
{
    YUV_NO_RESCALE = 1,   /* default value to be used in standard cases*/
    YUV_HALF_VALUE,
    YUV_RANGE_MAP_0,
    YUV_RANGE_MAP_1,
    YUV_RANGE_MAP_2,
    YUV_RANGE_MAP_3,
    YUV_RANGE_MAP_4,
    YUV_RANGE_MAP_5,
    YUV_RANGE_MAP_6,
    YUV_RANGE_MAP_7

} YUV_Scaling_t;


typedef struct YUV_ScalingFactor_s
{
    YUV_Scaling_t ScalingFactorY;
    YUV_Scaling_t ScalingFactorUV;

} YUV_ScalingFactor_t;



typedef struct STGXOBJ_Bitmap_s
{
  STGXOBJ_ColorType_t                   ColorType;
  STGXOBJ_BitmapType_t                  BitmapType;
  BOOL                                  PreMultipliedColor;
  STGXOBJ_ColorSpaceConversionMode_t    ColorSpaceConversion;
  STGXOBJ_AspectRatio_t                 AspectRatio;
  U32                                   Width;
  U32                                   Height;
  U32                                   Pitch;
  U32                                   Offset;
  void*                                 Data1_p;
  void*                                 Data1_Cp;
  void*                                 Data1_NCp;
  U32                                   Size1;
  void*                                 Data2_p;
  void*                                 Data2_Cp;
  void*                                 Data2_NCp;
  U32                                   Size2;
  STGXOBJ_SubByteFormat_t               SubByteFormat;
  BOOL                                  BigNotLittle;
  U32                                   Pitch2;
  U32                                   Offset2;
  YUV_ScalingFactor_t                   YUVScaling;
} STGXOBJ_Bitmap_t;

typedef struct STGXOBJ_BitmapAllocParams_s
{
  STAVMEM_AllocBlockParams_t    AllocBlockParams;
  U32                           Pitch;
  U32                           Offset;
} STGXOBJ_BitmapAllocParams_t;

typedef struct STGXOBJ_Capability_s
{
  char TBD;
} STGXOBJ_Capability_t;

typedef struct STGXOBJ_ColorACLUT_s
{
  U8 Alpha;
  U8 PaletteEntry;
} STGXOBJ_ColorACLUT_t;

typedef struct STGXOBJ_ColorARGB_s
{
  U8 Alpha;
  U8 R;
  U8 G;
  U8 B;
} STGXOBJ_ColorARGB_t;

typedef struct STGXOBJ_ColorKeyCLUT_s
{
  U8      PaletteEntryMin;
  U8      PaletteEntryMax;
  BOOL    PaletteEntryOut;
  BOOL    PaletteEntryEnable;
} STGXOBJ_ColorKeyCLUT_t;

typedef struct STGXOBJ_ColorKeyRGB_s
{
  U8      RMin;
  U8      RMax;
  BOOL    ROut;
  BOOL    REnable;

  U8      GMin;
  U8      GMax;
  BOOL    GOut;
  BOOL    GEnable;

  U8      BMin;
  U8      BMax;
  BOOL    BOut;
  BOOL    BEnable;
} STGXOBJ_ColorKeyRGB_t;

typedef struct STGXOBJ_ColorKeySignedYCbCr_s
{
  U8      YMin;
  U8      YMax;
  BOOL    YOut;
  BOOL    YEnable;

  S8      CbMin;
  S8      CbMax;
  BOOL    CbOut;
  BOOL    CbEnable;

  S8      CrMin;
  S8      CrMax ;
  BOOL    CrOut;
  BOOL    CrEnable;
} STGXOBJ_ColorKeySignedYCbCr_t;

typedef struct STGXOBJ_ColorKeyUnsignedYCbCr_s
{
  U8      YMin;
  U8      YMax;
  BOOL    YOut;
  BOOL    YEnable;

  U8      CbMin;
  U8      CbMax;
  BOOL    CbOut;
  BOOL    CbEnable;

  U8      CrMin;
  U8      CrMax ;
  BOOL    CrOut;
  BOOL    CrEnable;
} STGXOBJ_ColorKeyUnsignedYCbCr_t;

typedef union STGXOBJ_ColorKeyValue_u
{
  STGXOBJ_ColorKeyCLUT_t           CLUT1;
  STGXOBJ_ColorKeyCLUT_t           CLUT8;
  STGXOBJ_ColorKeyRGB_t            RGB888;
  STGXOBJ_ColorKeySignedYCbCr_t    SignedYCbCr888;
  STGXOBJ_ColorKeyUnsignedYCbCr_t  UnsignedYCbCr888;
  STGXOBJ_ColorKeyRGB_t            RGB565;
} STGXOBJ_ColorKeyValue_t;

typedef struct STGXOBJ_ColorKey_s
{
  STGXOBJ_ColorKeyType_t           Type;
  STGXOBJ_ColorKeyValue_t          Value;
} STGXOBJ_ColorKey_t;

typedef struct STGXOBJ_ColorRGB_s
{
  U8 R;
  U8 G;
  U8 B;
} STGXOBJ_ColorRGB_t;

typedef struct STGXOBJ_ColorSignedYCbCr_s
{
  U8 Y;
  S8 Cb;
  S8 Cr;
} STGXOBJ_ColorSignedYCbCr_t;

typedef struct STGXOBJ_ColorUnsignedYCbCr_s
{
  U8 Y;
  U8 Cb;
  U8 Cr;
} STGXOBJ_ColorUnsignedYCbCr_t;

typedef struct STGXOBJ_ColorSignedAYCbCr_s
{
  U8 Alpha;
  U8 Y;
  S8 Cb;
  S8 Cr;
} STGXOBJ_ColorSignedAYCbCr_t;

typedef struct STGXOBJ_ColorUnsignedAYCbCr_s
{
  U8 Alpha;
  U8 Y;
  U8 Cb;
  U8 Cr;
} STGXOBJ_ColorUnsignedAYCbCr_t;

typedef union STGXOBJ_ColorValue_u
{
  STGXOBJ_ColorARGB_t           ARGB8888;
  STGXOBJ_ColorRGB_t            RGB888;
  STGXOBJ_ColorARGB_t           ARGB8565;
  STGXOBJ_ColorRGB_t            RGB565;
  STGXOBJ_ColorARGB_t           ARGB1555;
  STGXOBJ_ColorARGB_t           ARGB4444;

  U8                            CLUT8;
  U8                            CLUT4;
  U8                            CLUT2;
  U8                            CLUT1;
  STGXOBJ_ColorACLUT_t          ACLUT88 ;
  STGXOBJ_ColorACLUT_t          ACLUT44 ;

  STGXOBJ_ColorSignedYCbCr_t    SignedYCbCr888_444;
  STGXOBJ_ColorUnsignedYCbCr_t  UnsignedYCbCr888_444;
  STGXOBJ_ColorSignedYCbCr_t    SignedYCbCr888_422;
  STGXOBJ_ColorUnsignedYCbCr_t  UnsignedYCbCr888_422;
  STGXOBJ_ColorSignedYCbCr_t    SignedYCbCr888_420;
  STGXOBJ_ColorUnsignedYCbCr_t  UnsignedYCbCr888_420;
  STGXOBJ_ColorUnsignedAYCbCr_t UnsignedAYCbCr6888_444;
  STGXOBJ_ColorSignedAYCbCr_t   SignedAYCbCr8888;
  STGXOBJ_ColorUnsignedAYCbCr_t UnsignedAYCbCr8888;

  U8                            ALPHA1;
  U8                            ALPHA4;
  U8                            ALPHA8;
  U8                            Byte;

} STGXOBJ_ColorValue_t;

typedef struct STGXOBJ_Color_s
{
  STGXOBJ_ColorType_t            Type;
  STGXOBJ_ColorValue_t           Value;
} STGXOBJ_Color_t;

typedef struct STGXOBJ_Palette_s
{
  STGXOBJ_ColorType_t       ColorType;
  STGXOBJ_PaletteType_t     PaletteType;
  U8                        ColorDepth;
  void*                     Data_p;
  void*                     Data_Cp;
  void*                     Data_NCp;
} STGXOBJ_Palette_t;

typedef struct STGXOBJ_PaletteAllocParams_s
{
  STAVMEM_AllocBlockParams_t  AllocBlockParams;
} STGXOBJ_PaletteAllocParams_t;

typedef struct STGXOBJ_Rectangle_s
{
  S32 PositionX;
  S32 PositionY;
  U32 Width;
  U32 Height;
} STGXOBJ_Rectangle_t;



/* Exported Functions ------------------------------------------------------- */

ST_ErrorCode_t STGXOBJ_ConvertPalette(
  STGXOBJ_Palette_t*           SrcPalette_p,
  STGXOBJ_Palette_t*           DstPalette_p,
  STGXOBJ_ColorSpaceConversionMode_t ConvMode
);

ST_ErrorCode_t STGXOBJ_GetPaletteColor(
  STGXOBJ_Palette_t*    Palette_p,
  U8                    PaletteIndex,
  STGXOBJ_Color_t*      Color_p
);


ST_ErrorCode_t STGXOBJ_SetPaletteColor(
  STGXOBJ_Palette_t*    Palette_p,
  U8                    PaletteIndex,
  STGXOBJ_Color_t*      Color_p
);


ST_ErrorCode_t STGXOBJ_GetBitmapAllocParams(STGXOBJ_Bitmap_t*     Bitmap_p,
                                    STGXOBJ_HardUse_t             HardUse,
                                    STGXOBJ_BitmapAllocParams_t*  Params1_p,
                                    STGXOBJ_BitmapAllocParams_t*  Params2_p);

ST_ErrorCode_t STGXOBJ_GetPaletteAllocParams(STGXOBJ_Palette_t*     Palette_p,
                                        STGXOBJ_HardUse_t             HardUse,
                                        STGXOBJ_PaletteAllocParams_t* Params_p);

ST_Revision_t STGXOBJ_GetRevision(void);

/* C++ support */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __STGXOBJ_H */

/* End of stgxobj.h */

