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
** $Id: tde.c 7268 2007-07-03 08:08:43Z xwyan $
**
** tde.c: TDE Function.
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hi_tde.h"
#include "tde_reg.h"

#if !defined(printx)
#define printx printf
#endif

#ifndef BOOL
#define BOOL int
#endif
#define Safe_Minus1(x) ((x)>0?((x)-1):0)

/* max FIFO Space for one accelerate process */
#define MAX_CMDFIFO_SPACE_NEEDED 11

#define TDE_MUTEXLOCK U32

#define HI_ERRNO(x) do { printf("TDE error, line:%d\n", __LINE__); return(x);}while(0)

/* noly allow one accelerate process in queue */
TDE_MUTEXLOCK tde_FIFO_lock = 1;

static TDE_Reg_Type *pTdeReg = NULL;

HI_RET TDE_Init(void* pRegisterBase)
{
    pTdeReg = pRegisterBase;
    if(NULL == pTdeReg)
    {
        HI_ERRNO(HI_FAILURE);
    }

    return HI_SUCCESS;
}

/* width and height < 0x1000 */
void TDE_SetOptSize(U16 opt_width, U16 opt_height)
{
    TDE_Reg_Size size;

    size.bits.Blt_Width = Safe_Minus1(opt_width&0x0fff);
    size.bits.Blt_Height = Safe_Minus1(opt_height&0x0fff);
    pTdeReg->size.ul32 = size.ul32;
}

void TDE_SetColorSpace(TDE_RGB ColorSpaceMax, TDE_RGB ColorSpaceMin)
{
    pTdeReg->colo_max.ul32 = (U32)ColorSpaceMax;
    pTdeReg->colo_min.ul32 = (U32)ColorSpaceMin;
}

void TDE_WaitForDone(void)
{
#if 1
    /* FIXME: maybe it's a bug of Hi3510 TDE IP */
    TDE_Reg_Status  status;

    /* wait for fifo empty, but the command perhaps do not end really */  
    pTdeReg->colo_min.ul32 = 0;    

    while(1)
    {
        status.ul32 = pTdeReg->status.ul32;
        if (status.bits.Fifo_depth >= 30)
            break;
    }
#else
    usleep(10000);
#endif
}

HI_RET TDE_MoveBlit(IN TDE_MoveBlitParam * pMBParam)
{
    TDE_Reg_Control     control;
    TDE_Reg_SrcDstStride    SrcDstStride;
    TDE_Reg_Misc            Misc;

    if(NULL == pTdeReg)
    {
        HI_ERRNO(HI_FAILURE);
    }

    if(NULL == pMBParam)
    {
        HI_ERRNO(HI_FAILURE);
    }


    control.ul32 = pTdeReg->control.ul32;

    /* in and out color RGB format */
    control.bits.in_color_format = pMBParam->InColorFormat;
    control.bits.out_color_format = pMBParam->OutColorFormat;

    control.bits.alpha = pMBParam->InternalAlpha;
    control.bits.rop_code = pMBParam->RopCode;

    /* tde_cmd */
    control.bits.tde_cmd_Cmd = TDE_CMD_MOVE_BLIT;
    control.bits.tde_cmd_Colorspace = pMBParam->ColorSpaceOpt;
    control.bits.tde_cmd_Data_Opt = pMBParam->DataOpt;
    control.bits.tde_cmd_OutAlphaFrom = pMBParam->OutAlphaFrom;

    /* set blit operation size */
    TDE_SetOptSize(pMBParam->opt_width, pMBParam->opt_height);

    /* addr */
    pTdeReg->src_addr = (U32)pMBParam->src_pixels;
    pTdeReg->dst_addr = (U32)pMBParam->dst_pixels;
    pTdeReg->out_addr = (U32)pMBParam->out_pixels;

    /* stride */
    SrcDstStride.bits.Src_Stride = pMBParam->src_stride;
    SrcDstStride.bits.Dst_Stride = pMBParam->dst_stride;
    pTdeReg->src_dst_stride.ul32 = SrcDstStride.ul32;

    /* Misc */
    Misc.ul32 = pTdeReg->misc.ul32;
    Misc.bits.Gout_Stride = pMBParam->out_stride;
    pTdeReg->misc.ul32 = Misc.ul32;

    /* set color space */
    TDE_SetColorSpace(pMBParam->ColorSpaceMax, pMBParam->ColorSpaceMin);
    
    /* start 2D */
    control.bits.tde_start = 1;
    pTdeReg->control.ul32 = control.ul32;

    TDE_WaitForDone();
    return HI_SUCCESS;
}

HI_RET TDE_PatternFill(IN TDE_PatternFillParam * pPFParam)
{
    TDE_Reg_Control     control;
    TDE_Reg_SrcDstStride    SrcDstStride;
    TDE_Reg_Misc            Misc;

    if(NULL == pTdeReg)
    {
        HI_ERRNO(HI_FAILURE);
    }

    if(NULL == pPFParam)
    {
        HI_ERRNO(HI_FAILURE);
    }

    //printPFParam(pPFParam);
    control.ul32 = pTdeReg->control.ul32;

    /* in and out color RGB format */
    control.bits.in_color_format = pPFParam->InColorFormat;
    control.bits.out_color_format = pPFParam->OutColorFormat;

    control.bits.alpha = pPFParam->InternalAlpha;
    control.bits.rop_code = pPFParam->RopCode;

    /* tde_cmd */
    control.bits.tde_cmd_Cmd = TDE_CMD_PATTERN_CLIP;
    control.bits.tde_cmd_Colorspace = pPFParam->ColorSpaceOpt;
    control.bits.tde_cmd_Data_Opt = pPFParam->DataOpt;
    control.bits.tde_cmd_OutAlphaFrom = pPFParam->OutAlphaFrom;

    /* set blit operation size */
    TDE_SetOptSize(pPFParam->opt_width, pPFParam->opt_height);

    /* addr */
    pTdeReg->src_addr = (U32)pPFParam->src_pixels;
    pTdeReg->dst_addr = (U32)pPFParam->dst_pixels;
    pTdeReg->out_addr = (U32)pPFParam->out_pixels;
    
    /* stride */
    SrcDstStride.bits.Src_Stride = pPFParam->src_stride;
    SrcDstStride.bits.Dst_Stride = pPFParam->dst_stride;
    pTdeReg->src_dst_stride.ul32 = SrcDstStride.ul32;
    
    /* Misc */
    Misc.ul32 = pTdeReg->misc.ul32;
    Misc.bits.Gout_Stride = pPFParam->out_stride;
    /* Pattern size */
    Misc.bits.Pattern_width = pPFParam->pattern_width;
    Misc.bits.Pattern_width = Safe_Minus1(Misc.bits.Pattern_width);
    Misc.bits.Pattern_height = pPFParam->pattern_height;
    Misc.bits.Pattern_height = Safe_Minus1(Misc.bits.Pattern_height);
    pTdeReg->misc.ul32 = Misc.ul32;

    /* set color space */
    TDE_SetColorSpace(pPFParam->ColorSpaceMax, pPFParam->ColorSpaceMin);
    
    /* start 2D */
    control.bits.tde_start = 1;
    pTdeReg->control.ul32 = control.ul32;

    TDE_WaitForDone();
    return HI_SUCCESS;
}


HI_RET TDE_SolidDraw(IN TDE_SolidDrawParam * pSDParam)
{
    TDE_Reg_Control     control;
    TDE_Reg_SrcDstStride    SrcDstStride;
    TDE_Reg_Misc            Misc;

    if(NULL == pTdeReg)
    {
        HI_ERRNO(HI_FAILURE);
    }

    if(NULL == pSDParam)
    {
        HI_ERRNO(HI_FAILURE);
    }
    
    control.ul32 = pTdeReg->control.ul32;

    /* in and out color RGB format */
    control.bits.in_color_format = pSDParam->InColorFormat;
    control.bits.out_color_format = pSDParam->OutColorFormat;

    control.bits.alpha = pSDParam->InternalAlpha;
    control.bits.rop_code = pSDParam->RopCode;

    /* tde_cmd */
    control.bits.tde_cmd_Cmd = TDE_CMD_SOLID_DRAW;
    control.bits.tde_cmd_Colorspace = pSDParam->ColorSpaceOpt;
    control.bits.tde_cmd_Data_Opt = pSDParam->DataOpt;
    control.bits.tde_cmd_OutAlphaFrom = pSDParam->OutAlphaFrom;

    /* set blit operation size */
    TDE_SetOptSize(pSDParam->opt_width, pSDParam->opt_height);

    /* addr */
    pTdeReg->dst_addr = (U32)pSDParam->dst_pixels;
    pTdeReg->out_addr = (U32)pSDParam->out_pixels;
    
    /* stride */
    SrcDstStride.bits.Dst_Stride = pSDParam->dst_stride;
    pTdeReg->src_dst_stride.ul32 = SrcDstStride.ul32;

    /* Misc */
    Misc.ul32 = pTdeReg->misc.ul32;
    Misc.bits.Gout_Stride = pSDParam->out_stride;
    pTdeReg->misc.ul32 = Misc.ul32;

    /* color filled */
    pTdeReg->color_filled.ul32 = (U32)pSDParam->sd_fill_color;

    /* start 2D */
    control.bits.tde_start = 1;
    pTdeReg->control.ul32 = control.ul32;

    TDE_WaitForDone();
    return HI_SUCCESS;
}



static void print_ColorSpace(IN TDE_COLORSPACE cs)
{
#ifdef _DEBUG
    printx("ColorSpaceOpt = ");
    switch(cs)
    {
        case TDE_COLORSPACE_DISABLE:
            printx("TDE_COLORSPACE_DISABLE\n");
            break;
        case TDE_COLORSPACE_ENABLE:
            printx("TDE_COLORSPACE_ENABLE\n");
            break;
        default:
            printx("Unknown %d\n", cs);
            break;
    }
#endif
}

void print_DataOpt(TDE_DATA_OPT    dp)
{
#ifdef _DEBUG
    printx("DataOpt = ");
    switch(dp)
    {
        case TDE_DATA_OPT_NONE:
            printx("TDE_DATA_OPT_NONE\n");
            break;
        case TDE_DATA_OPT_ROP:
            printx("TDE_DATA_OPT_ROP\n");
            break;
        case TDE_DATA_OPT_ALPHA_INTERNAL:
            printx("TDE_DATA_OPT_ALPHA_INTERNAL\n");
            break;
        case TDE_DATA_OPT_ALPHA_SRC:
            printx("TDE_DATA_OPT_ALPHA_SRC\n");
            break;
        default:
            printx("Unknown %d\n", dp);
            break;
    }
#endif
}

void print_OutAlphaFrom(TDE_OUT_ALPHAFROM oaf)
{
#ifdef _DEBUG
    printx("OutAlphaFrom = ");
    switch(oaf)
    {
        case TDE_OUT_ALPHAFROM_INTERNAL:
            printx("TDE_OUT_ALPHAFROM_INTERNAL\n");
            break;
        case TDE_OUT_ALPHAFROM_SRC:
            printx("TDE_OUT_ALPHAFROM_SRC\n");
            break;
        default:
            printx("Unknown %d\n", oaf);
            break;
    }
#endif
}

void print_ColorFormat(TDE_COLOR_FORMAT cf)
{
#ifdef _DEBUG
    switch(cf)
    {
        case TDE_COLOR_FORMAT_RGB1555:
            printx("TDE_COLOR_FORMAT_RGB1555\n");
            break;
        case TDE_COLOR_FORMAT_RGB444:
            printx("TDE_COLOR_FORMAT_RGB444\n");
            break;
        case TDE_COLOR_FORMAT_RGB4444:
            printx("TDE_COLOR_FORMAT_RGB4444\n");
            break;
        case TDE_COLOR_FORMAT_RGB555:
            printx("TDE_COLOR_FORMAT_RGB555\n");
            break;
        case TDE_COLOR_FORMAT_RGB565:
            printx("TDE_COLOR_FORMAT_RGB565\n");
            break;
        case TDE_COLOR_FORMAT_RGB888:
            printx("TDE_COLOR_FORMAT_RGB888\n");
            break;
        case TDE_COLOR_FORMAT_RGB8888:
            printx("TDE_COLOR_FORMAT_RGB8888\n");
            break;
        default:
            printx("Unknown %d\n", cf);
            break;
    }
#endif
}

void print_RopCode(TDE_ROP_CODE rc)
{
#ifdef _DEBUG
    printx("RopCode = %d :");
    switch(rc)
    {
        case TDE_ROP_BLACK: /* 0 */
            printx("TDE_ROP_BLACK\n");
            break;
        case TDE_ROP_PSDon:
            printx("TDE_ROP_PSDon\n");
            break;
        case TDE_ROP_PSDna:
            printx("TDE_ROP_PSDna\n");
            break;
        case TDE_ROP_PSn:
            printx("TDE_ROP_PSn\n");
            break;
        case TDE_ROP_DPSna:
            printx("TDE_ROP_DPSna\n");
            break;
        case TDE_ROP_Dn: /* 5 */
            printx("TDE_ROP_Dn\n");
            break;
        case TDE_ROP_PSDx:
            printx("TDE_ROP_PSDx\n");
            break;
        case TDE_ROP_PSDan:
            printx("TDE_ROP_PSDan\n");
            break;
        case TDE_ROP_PSDa:
            printx("TDE_ROP_PSDa\n");
            break;
        case TDE_ROP_PSDxn:
            printx("TDE_ROP_PSDxn\n");
            break;
        case TDE_ROP_D: /* 10 */
            printx("TDE_ROP_D\n");
            break;
        case TDE_ROP_PSDno:
            printx("TDE_ROP_PSDno\n");
            break;
        case TDE_ROP_PS:
            printx("TDE_ROP_PS\n");
            break;
        case TDE_ROP_DPSno:
            printx("TDE_ROP_DPSno\n");
            break;
        case TDE_ROP_PSDo:
            printx("TDE_ROP_PSDo\n");
            break;
        case TDE_ROP_WHITE:
            printx("TDE_ROP_WHITE\n");
            break;
        default:
            printx("Unknown\n");
            break;
    }
#endif
}

void printMBParam(TDE_MoveBlitParam *pParam)
{
#ifdef _DEBUG
    printx("\nMoveBlit Param print info:\n");

    print_ColorSpace(pParam->ColorSpaceOpt);
    print_DataOpt(pParam->DataOpt);
    print_OutAlphaFrom(pParam->OutAlphaFrom);

    printx("InColorFormat = ");
    print_ColorFormat(pParam->InColorFormat);

    printx("OutColorFormat = ");
    print_ColorFormat(pParam->OutColorFormat);

    printx("InternalAlpha = 0x%x\n", pParam->InternalAlpha);

    print_RopCode(pParam->RopCode);

    printx("ColorSpaceMin = %#x\n", pParam->ColorSpaceMin);
    printx("ColorSpaceMax = %#x\n", pParam->ColorSpaceMax);

    printx("opt_width = %d\n", pParam->opt_width);
    printx("opt_height = %d\n", pParam->opt_height);

    printx("src_pixels = %#x\n", (U32)pParam->src_pixels);
    printx("src_stride = %d\n", pParam->src_stride);

    printx("dst_pixels = %#x\n", (U32)pParam->dst_pixels);
    printx("dst_stride = %d\n", pParam->dst_stride);
    
    printx("out_pixels = %#x\n", (U32)pParam->out_pixels);
    printx("out_stride = %d\n", pParam->out_stride);
#endif
}

void printPFParam(TDE_PatternFillParam *pParam)
{
#ifdef _DEBUG
    printx("\nPatternFill Param print info:\n");

    print_ColorSpace(pParam->ColorSpaceOpt);
    print_DataOpt(pParam->DataOpt);
    print_OutAlphaFrom(pParam->OutAlphaFrom);

    printx("InColorFormat = ");
    print_ColorFormat(pParam->InColorFormat);

    printx("OutColorFormat = ");
    print_ColorFormat(pParam->OutColorFormat);

    printx("InternalAlpha = 0x%x\n", pParam->InternalAlpha);

    print_RopCode(pParam->RopCode);

    printx("ColorSpaceMin = %#x\n", pParam->ColorSpaceMin);
    printx("ColorSpaceMax = %#x\n", pParam->ColorSpaceMax);

    printx("opt_width = %d\n", pParam->opt_width);
    printx("opt_height= %d\n", pParam->opt_height);

    printx("src_pixels = %#x\n", (U32)pParam->src_pixels);
    printx("src_stride = %d\n", pParam->src_stride);

    printx("dst_pixels = %#x\n", (U32)pParam->dst_pixels);
    printx("dst_stride = %d\n", pParam->dst_stride);
    
    printx("out_pixels = %#x\n", (U32)pParam->out_pixels);
    printx("out_stride = %d\n", pParam->out_stride);

    printx("pattern_width = %d\n", pParam->pattern_width);
    printx("pattern_height = %d\n", pParam->pattern_height);    
#endif
}

void printSDParam(TDE_SolidDrawParam *pParam)
{
#ifdef _DEBUG
    printx("\nSolidDraw Param print info:\n");

    print_ColorSpace(pParam->ColorSpaceOpt);
    print_DataOpt(pParam->DataOpt);
    print_OutAlphaFrom(pParam->OutAlphaFrom);

    printx("InColorFormat = ");
    print_ColorFormat(pParam->InColorFormat);

    printx("OutColorFormat = ");
    print_ColorFormat(pParam->OutColorFormat);

    printx("InternalAlpha = 0x%x", pParam->InternalAlpha);

    print_RopCode(pParam->RopCode);

    printx("opt_width = %d\n", pParam->opt_width);
    printx("opt_height = 0x%x\n", pParam->opt_height);

    printx("sd_fill_color = 0x%x\n", pParam->sd_fill_color);

    printx("dst_pixels = 0x%x\n", (U32)pParam->dst_pixels);
    printx("dst_stride = %d\n", pParam->dst_stride);
    
    printx("out_stride = 0x%x\n", (U32)pParam->out_pixels);
    printx("out_stride = %d\n", pParam->out_stride);
#endif
}

#if 0
static int print_register(void)
{
    if(NULL == pTdeReg)
    {
        HI_ERRNO(HI_FAILURE);
    }
    
    printf("control:%d!\n", pTdeReg->control.ul32);
    printf("size:%d!\n", pTdeReg->size.ul32);
    printf("src_addr:%d!\n", pTdeReg->src_addr);
    printf("dst_addr:%d!\n", pTdeReg->dst_addr);
    printf("out_addr:%d!\n", pTdeReg->out_addr);
    printf("src_dst_stride:%d!\n", pTdeReg->src_dst_stride.ul32);
    printf("misc:%d!\n", pTdeReg->misc.ul32);
    printf("color_filled:%d!\n", pTdeReg->color_filled.ul32);
    printf("colo_max:%d!\n", pTdeReg->colo_max.ul32);
    printf("colo_min:%d!\n", pTdeReg->colo_min.ul32);
    printf("status:%d!\n", pTdeReg->status.ul32);
    printf("fifo:%d!\n", pTdeReg->status.bits.Fifo_depth);
    return 0;
}
#endif

HI_RET TDE_wmemcpy(U32* dest, const U32* src, size_t wcount)
{
    #define CPY_STRIDE    2048
    TDE_MoveBlitParam    mbParam;

    if( (NULL == dest) || (NULL == src) )
    {
        HI_ERRNO(HI_FAILURE);
    }

    mbParam.ColorSpaceOpt = TDE_COLORSPACE_DISABLE;
    mbParam.DataOpt = TDE_DATA_OPT_NONE;

    if(mbParam.DataOpt == TDE_DATA_OPT_ALPHA_INTERNAL)
        mbParam.OutAlphaFrom = TDE_OUT_ALPHAFROM_INTERNAL;
    else
        mbParam.OutAlphaFrom = TDE_OUT_ALPHAFROM_SRC;

    mbParam.InColorFormat = TDE_COLOR_FORMAT_RGB8888;
    mbParam.OutColorFormat = mbParam.InColorFormat;

    mbParam.opt_width = CPY_STRIDE/4;
    mbParam.opt_height = wcount*4/CPY_STRIDE;

    /* source image */
    mbParam.src_pixels = (U8 *)src;
    mbParam.src_stride = CPY_STRIDE;

    mbParam.out_pixels = (U8 *)dest;
    mbParam.out_stride = CPY_STRIDE;

    if(HI_SUCCESS == TDE_MoveBlit(&mbParam))
    {
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}

void TDE_AntiFlicker(
    U32 src1_phy_addr,
    U32 src2_phy_addr,
    U32 dst_phy_addr,
    U32 img_width,
    U32 img_heigth,
    U32 img_stride,
    U32 coff, 
    U32 flag,
    U32 offset)
{
	TDE_MoveBlitParam    mbParam;
	U32 src_alpha;
	if((0 == src1_phy_addr) 
	   || (0 == src2_phy_addr) 
	   || (0 == dst_phy_addr)  
	   || (0 == img_width) 
	   || (1 > img_heigth) 
	   || (0 == img_stride))
	{
		return;
	}

	src_alpha = 0x80*coff/100;

	mbParam.ColorSpaceOpt = TDE_COLORSPACE_DISABLE;
	mbParam.DataOpt = TDE_DATA_OPT_ALPHA_INTERNAL;

	if(mbParam.DataOpt == TDE_DATA_OPT_ALPHA_INTERNAL)
	{
	mbParam.OutAlphaFrom = TDE_OUT_ALPHAFROM_INTERNAL;
	mbParam.InternalAlpha = (U8)src_alpha;
	}
	else
	{
		mbParam.OutAlphaFrom = TDE_OUT_ALPHAFROM_SRC;
	}

	mbParam.InColorFormat = TDE_COLOR_FORMAT_RGB1555;
	mbParam.OutColorFormat = mbParam.InColorFormat;

	mbParam.opt_width = img_width;
	mbParam.opt_height = img_heigth;

	/* source1 image */
	mbParam.src_pixels = (U8 *)src1_phy_addr;
	mbParam.src_stride = img_stride;

	/*source2 image*/
	mbParam.dst_pixels = (U8 *)src2_phy_addr;
	mbParam.dst_stride = img_stride;

    /*out image, attention offset*/
    if (flag != 1)
        mbParam.out_pixels = (U8 *)dst_phy_addr;
    else
        mbParam.out_pixels = (U8 *)(dst_phy_addr + offset);
	mbParam.out_stride = img_stride;

	TDE_MoveBlit(&mbParam);
    
	return ;
}
