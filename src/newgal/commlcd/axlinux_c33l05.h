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
/* test c33l05dmt */

#ifndef _GPCDRV_
#define _GPCDRV_

//---------------------------------------------------------------------------------
// Constants (#define)
//---------------------------------------------------------------------------------
//DMA type
#define NORMAL_DMA            0x77000000
#define INVERSE_DMA           0x75000000
#define FIX_DMA               0x47000000

//Lcdc clock DT
#define LCLG_dt1              0x80
#define LCLG_dt2              0x81
#define LCLG_dt3              0x82
#define LCLG_dt4              0x83
#define LCLG_dt5              0x84
#define LCLG_dt6              0x85
#define LCLG_dt7              0x86
#define LCLG_dt8              0x87
#define LCLG_dt9              0x88
#define LCLG_dt10             0x89
#define LCLG_dt11             0x8a
#define LCLG_dt12             0x8b
#define LCLG_dt13             0x8c
#define LCLG_dt14             0x8d
#define LCLG_dt15             0x8e
#define LCLG_dt16             0x8f

//Panel type definition
#define MONO_4BIT             0x2200 //swinv - mono
#define MONO_8BIT             0x2600 //swinv - mono
#define COLOR_4BIT            0x4000 
#define COLOR_8BIT_F1         0x4400
#define COLOR_8BIT_F2         0x4c00
///////////////////////////////////////////////////////////////
//  Define Declare Section 
///////////////////////////////////////////////////////////////


#define	LCD_CON_ADDR				0x50
#define LCD_BPP					16

#define	VRAM_ADDR_1           0x2000000 // (0x200_0000~0x202_5800-1), range:0x25800
#define	VRAM_ADDR_2           0x2025800 // (0x202_5800~0x204_b000-1), range:0x25800
#define VIRTUAL_VRAM_ADDR_1   0x20cb000 // (0x204_b000~0x20a_2e40-1), range:0x57e40
#define VIRTUAL_VRAM_ADDR_2   0x204b000 // (0x204_b000~0x20a_2e40-1), range:0x57e40
#define VIRTUAL_SCREEN_WIDTH  320
#define VIRTUAL_SCREEN_HEIGHT 240
#define VIEW_PORT_WIDTH	      200
#define VIEW_PORT_HEIGHT      180
#define PIP_X		      5
#define PIP_Y		      40	
#define STEP_X		     (VIRTUAL_SCREEN_WIDTH-VIEW_PORT_WIDTH)/10
#define STEP_Y		     (VIRTUAL_SCREEN_HEIGHT-VIEW_PORT_HEIGHT)/10

#define COLOR_PANEL
//#define PANEL_YMIRROR	
#define SCREEN_WIDTH    	320
#define SCREEN_HEIGHT   	240
#define LCD_BPP_UNIT		2
#define LINE_SPACE		20

////////////////////////////////////////////////////////////////*Extended I/O port pins*/
#ifdef L05DMT
#define SET_P16_O           *(volatile unsigned char*) 0x402d6 |= 0x40 //set P16 as output port(LCD display switch)
#define SET_P16_H           *(volatile unsigned char*) 0x402d5 |= 0x40 //set P16 high
#define SET_P16_L           *(volatile unsigned char*) 0x402d5 &= 0xbf //set P16 low
#define SET_P15_O           *(volatile unsigned char*) 0x402d6 |= 0x20 //set P15 as output port(LCDPWR of C33L05)
#define SET_P15_H           *(volatile unsigned char*) 0x402d5 |= 0x20 //set P15 high
#define SET_P15_L           *(volatile unsigned char*) 0x402d5 &= 0xdf //set P15 low
#else
#define SET_PD7_O           *(volatile unsigned char*) 0x300f47 |= 0x80 //set PD7 as output port(LCD display switch)
#define SET_PD7_H           *(volatile unsigned char*) 0x300f46 |= 0x80 //set PD7 high
#define SET_PD7_L           *(volatile unsigned char*) 0x300f46 &= 0x7f //set PD7 low
#define SET_PD6_O           *(volatile unsigned char*) 0x300f47 |= 0x40 //set PD6 as output port(LCDPWR of C33L05)
#define SET_PD6_H           *(volatile unsigned char*) 0x300f46 |= 0x40 //set PD6 high
#define SET_PD6_L           *(volatile unsigned char*) 0x300f46 &= 0xbf //set PD6 low
#endif

#define PB_FUNC1_LCDC       *(volatile unsigned char*) 0x300f62 = 0x55 //set PB function:FPDAT3--0
#define PB_FUNC2_LCDC       *(volatile unsigned char*) 0x300f63 = 0x55 //set PB function:FPDAT7--4
#define PC_FUNC_LCDC        *(volatile unsigned char*) 0x300f64 = 0x55 //set PC function:DRDY, FPSHIFT, FPLINE, FPFRAME

/*Misc register initialize*/

#define CLR_MISC_PROTECT    *(volatile unsigned char*)  0x300f2f = 0x96 //remove write protection
#define MISC_LCLG_EN_DT     *(volatile unsigned char*)  0x300f34 = LCLG_dt4 //set LCDC CLK = subsys_clk/(LCLGDT[3:0]+1)
#define MISC_LCLG_EN        *(volatile unsigned char*)  0x300f34|=0x80 //enable lcdc clock
#define MISC_LCLG_DN        *(volatile unsigned char*)  0x300f34&=0x7f //disable lcdc clock

/*HSDMA*/
#define HSDMA_SET2ADDRDUAL  *(volatile unsigned short*) 0x48242|=0x8000 //set ch.2 address mode:dual addr
#define HSDMA_SET2EN        *(volatile unsigned short*) 0x4824c|=0x0001 //enable HSDMA ch.2
#define HSDMA_SET2DN        *(volatile unsigned short*) 0x4824c&=0xfffe //disable HSDMA ch.2
#define HSDMA_SET2SWTRI_EN  *(volatile unsigned char*)  0x4029a|=0x04 //enable ch.2 software trigger

/*LCDC register initialize*/

#define EN_FRAME_INT        *(volatile unsigned short*) 0x380010=0x8000 //enable lcdc frame interrupt
#define DN_FRAME_INT        *(volatile unsigned short*) 0x380010=0x0 //disable lcdc frame interrupt
#define CHECK_INT_FLAG      (*(volatile unsigned short*) 0x380014 & 0x8000) //if frame interrupt is generated
#define RESET_INT_FLAG      *(volatile unsigned short*) 0x380014 |= 0x8000 //reset frame interrupt flag
#define CHECK_TIMEOUT_FLAG  (*(volatile unsigned short*) 0x380014 & 0x0020) //if timeout error is generated

#define POWER_SAVE          *(volatile unsigned short*) 0x380014&=0xfffc //set power save mode
#define POWER_NORMAL        *(volatile unsigned short*) 0x380014|=0x0003 //set power normal mode

//frame rate= 3MHz / (320+72) / (240+6) = 31 Hz
#define SET_CHAR_COUNT      *(volatile unsigned short*) 0x380042 = 0x27 //set horizontal panel size = 320
#define SET_LINE_COUNT      *(volatile unsigned short*) 0x38004c = 0xef //set vertical panel size = 240

#define SET_CTRL_MODE0      *(volatile unsigned short*) 0x380200 = 0x55 //set mode0:16bpp, lut bypass, dither mode

#define SET_COLOR           *(volatile unsigned short*) 0x380202|=0x4000 //set color mode
#define SET_MONO            *(volatile unsigned short*) 0x380202&=0xbfff //set mono mode
#define SET_FPSHIFT_MASK    *(volatile unsigned short*) 0x380202|=0x2000 //set fpshift mask
#define CLR_FPSHIFT_MASK    *(volatile unsigned short*) 0x380202&=0xdfff //clear fpshift mask
#define SET_SWDT_INV        *(volatile unsigned short*) 0x380202|=0x0200 //set swdt invert
#define CLR_SWDT_INV        *(volatile unsigned short*) 0x380202&=0xfdff //clear swdt invert
#define SET_DISP_BLANK      *(volatile unsigned short*) 0x380202|=0x0100 //set disp blank
#define CLR_DISP_BLANK      *(volatile unsigned short*) 0x380202&=0xfeff //clear disp blank

#define SET_STADDR_1        *(volatile unsigned short*) 0x380210 = 0x8000 //set screen1 start address (setting is 0x8000)
#define SET_STADDR_2        *(volatile unsigned short*) 0x380210 = 0x92c0 //set screen2 start address (setting is 0x92c0)
#define SET_VRAM_EX         *(volatile unsigned short*) 0x380210|=0x8000 //set external sdram as vram
#define SET_VRAM_IN         *(volatile unsigned short*) 0x380210&=0x7fff //set internal vram as vra

#endif // _GPCDRV_ 

