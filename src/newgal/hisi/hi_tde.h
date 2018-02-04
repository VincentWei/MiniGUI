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
** $Id: hi_tde.h 7268 2007-07-03 08:08:43Z xwyan $
**
** hi_tde.h: header file. 
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/

#ifndef __TDE_H__
#define __TDE_H__

#define U32 unsigned int 
#define U8  unsigned char
#define U16 unsigned short int

#define HI_RET int
#define IN 
#define OUT
#define IO
#define HI_SUCCESS 0
#define HI_FAILURE -1
#define TRUE 1
#define FALSE 0

#define BOOL int

typedef enum
{
    TDE_CMD_NOTHING = 0,
    TDE_CMD_MOVE_BLIT = 1,
    TDE_CMD_PATTERN_CLIP = 2,
    TDE_CMD_SOLID_DRAW = 3    
}TDE_CMD;

typedef enum
{
    TDE_COLORSPACE_ENABLE = 1,
    TDE_COLORSPACE_DISABLE = 0    
}TDE_COLORSPACE;

typedef enum
{
    TDE_DATA_OPT_NONE = 0,
    TDE_DATA_OPT_ROP = 1,
    TDE_DATA_OPT_ALPHA_SRC = 2,
    TDE_DATA_OPT_ALPHA_INTERNAL = 3
}TDE_DATA_OPT;

typedef enum
{
    TDE_OUT_ALPHAFROM_SRC = 0,
    TDE_OUT_ALPHAFROM_INTERNAL = 1    
}TDE_OUT_ALPHAFROM;

typedef enum
{
    TDE_COLOR_FORMAT_RGB444     = 0,
    TDE_COLOR_FORMAT_RGB4444     = 1,
    TDE_COLOR_FORMAT_RGB555     = 2,
    TDE_COLOR_FORMAT_RGB565     = 3,
    TDE_COLOR_FORMAT_RGB1555     = 4,
    TDE_COLOR_FORMAT_RGB888     = 6,
    TDE_COLOR_FORMAT_RGB8888     = 7
}TDE_COLOR_FORMAT;


/*
** S:Source    P:Pattern    D:Destination
** in MoveBlit, PS-->S;
** in PatternFill, PS-->P
** in Solid Draw, no source and pattern, PS-->filled_color 
*/
typedef enum
{
    TDE_ROP_BLACK    = 0,    /* Blackness */
    TDE_ROP_PSDon    = 1,    /* ~(PS+D) */
    TDE_ROP_PSDna    = 2,    /* ~PS & D */
    TDE_ROP_PSn    = 3,    /* ~PS */
    TDE_ROP_DPSna    = 4,    /* PS & ~D */
    TDE_ROP_Dn        = 5,    /* ~D */
    TDE_ROP_PSDx    = 6,    /* PS^D */
    TDE_ROP_PSDan    = 7,    /* ~(PS&D) */
    TDE_ROP_PSDa    = 8,    /* PS & D */
    TDE_ROP_PSDxn    = 9,    /* ~(PS^D) */
    TDE_ROP_D        = 10,    /* D */
    TDE_ROP_PSDno    = 11,    /* ~PS + D */
    TDE_ROP_PS        = 12,    /* PS */
    TDE_ROP_DPSno    = 13,    /* PS + ~D */
    TDE_ROP_PSDo    = 14,    /* PS+D */
    TDE_ROP_WHITE    = 15    /* Whiteness */
}TDE_ROP_CODE;

typedef U32 TDE_RGB;
#define MAKERGB(r, g, b)    \
    ((((TDE_RGB)r)<<16)+(((TDE_RGB)g)<<8)+((TDE_RGB)b))

typedef U32 TDE_ARGB;
#define MAKEARGB(a, r, g, b)    \
    ((((TDE_ARGB)a)<<24)+(((TDE_ARGB)r)<<16)+(((TDE_ARGB)g)<<8)+((TDE_ARGB)b))

typedef struct tag_TDE_MoveBlitParam
{
    TDE_COLORSPACE     ColorSpaceOpt;
    TDE_DATA_OPT        DataOpt;
    TDE_OUT_ALPHAFROM    OutAlphaFrom;

    TDE_COLOR_FORMAT InColorFormat;
    TDE_COLOR_FORMAT OutColorFormat;

    /* Internal Alpha used when (OutAlphaFrom==TDE_OUT_ALPHAFROM_INTERNAL) */
    U8 InternalAlpha;

    /* RopCode used when (DataOpt==TDE_DATA_OPT_ROP) */
    TDE_ROP_CODE RopCode;

    /* ColorSpace used when (ColorSpaceOpt==TDE_COLORSPACE_ENABLE) */
    TDE_RGB ColorSpaceMin;
    TDE_RGB ColorSpaceMax;

    /* Operated Size = opt_width*opt_height */
    U16    opt_width;
    U16 opt_height;    

    /* src_surface */
    U8 *    src_pixels;
    U16     src_stride;

    /* dst_surface used when: (DataOpt!=TDE_DATA_OPT_NONE) */
    U8 *    dst_pixels;
    U16        dst_stride;

    /* out_surface */
    U8 *    out_pixels;
    U16        out_stride;
} TDE_MoveBlitParam;


typedef struct tag_TDE_PatternFillParam
{
    TDE_COLORSPACE     ColorSpaceOpt;
    TDE_DATA_OPT        DataOpt;
    TDE_OUT_ALPHAFROM    OutAlphaFrom;

    TDE_COLOR_FORMAT InColorFormat;
    TDE_COLOR_FORMAT OutColorFormat;

    /* Internal Alpha used when (OutAlphaFrom==TDE_OUT_ALPHAFROM_INTERNAL) */
    U8 InternalAlpha;

    /* RopCode used when (DataOpt==TDE_DATA_OPT_ROP) */
    TDE_ROP_CODE RopCode;

    /* ColorSpace used when (ColorSpaceOpt==TDE_COLORSPACE_ENABLE) */
    TDE_RGB ColorSpaceMin;
    TDE_RGB ColorSpaceMax;

    /* Operated Size = opt_width*opt_height */
    U16 opt_width;
    U16 opt_height;    

    /* src_surface */
    U8 *    src_pixels;
    U16        src_stride;

    /* dst_surface used when: (DataOpt!=TDE_DATA_OPT_NONE)    */
    U8 *    dst_pixels;
    U16        dst_stride;

    /* out_surface */
    U8 *    out_pixels;
    U16        out_stride;

    U8    pattern_width;
    U8    pattern_height;
} TDE_PatternFillParam;

typedef struct tag_TDE_SolidDrawParam
{
    TDE_COLORSPACE     ColorSpaceOpt;
    TDE_DATA_OPT        DataOpt;
    TDE_OUT_ALPHAFROM    OutAlphaFrom;

    TDE_COLOR_FORMAT InColorFormat;
    TDE_COLOR_FORMAT OutColorFormat;

    /* Internal Alpha used when (OutAlphaFrom==TDE_OUT_ALPHAFROM_INTERNAL) */
    U8 InternalAlpha;

    /* RopCode used when (DataOpt==TDE_DATA_OPT_ROP) */
    TDE_ROP_CODE RopCode;

    /* Operated Size = opt_width*opt_height */
    U16 opt_width;
    U16 opt_height;

    /* Operated area filled color */
    TDE_RGB    sd_fill_color;

    /* dst_surface used when: (DataOpt!=TDE_DATA_OPT_NONE)    */
    U8 *    dst_pixels;
    U16        dst_stride;

    /* out_surface */
    U8 *    out_pixels;
    U16        out_stride;
} TDE_SolidDrawParam;

extern HI_RET TDE_Init(void* pRegisterBas);
extern HI_RET TDE_MoveBlit(IN TDE_MoveBlitParam * pMBParam);
extern HI_RET TDE_PatternFill(IN TDE_PatternFillParam * pPFParam);
extern HI_RET TDE_SolidDraw(IN TDE_SolidDrawParam * pSDParam);

extern void printMBParam(IN TDE_MoveBlitParam *pParam);
extern void printPFParam(IN TDE_PatternFillParam *pParam);
extern void printSDParam(IN TDE_SolidDrawParam *pParam);
extern void TDE_AntiFlicker(U32 src1_phy_addr,
                               U32 src2_phy_addr,
                               U32 dst_phy_addr,
                               U32 img_width,
                               U32 img_heigth,
                               U32 img_stride,
                               U32 coff, 
                               U32 flag,
                               U32 offset);
void MoveVOAddr(U32 Addr);
void TDE_WaitForDone(void);

/* extend functions */
HI_RET TDE_wmemcpy(
    U32* dest, 
    const U32* src, 
    size_t wcount);


HI_RET TDE_wmemset( 
    U32* dest,
    U32 w, 
    size_t wcount);


HI_RET TDE_ImageMerge(
    U32 bgImageAddr, U32 bgImageWidth, U32 bgImageHeight, U32 bgImageStride,
    U32 stickImageAddr, U32 stickImageWidth, U32 stickImageHeight, U32 stickImageStride,
    U32 stickX, U32 stickY, U32 stickTransparency,
    U32 mergeImageAddr, U32 mergeImageStride);



#endif  /* __TDE_H__ */

