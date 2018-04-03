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
**  unknown.c: A subdriver of CommonLCD NEWGAL engine for EPSON C33L05 running axLinux.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGGAL_COMMLCD

#ifdef __TARGET_C33L05__

#include "minigui.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "commlcd.h"

#undef L05DMT

#include "axlinux_c33l05.h"

static void LCD_initial (unsigned char lcdclkdt)
{
    int             i;
    unsigned char   dt;

#ifdef L05DMT
    SET_P15_H;
    SET_P16_H;
    //set P15, P16 as GPIO
    *(volatile unsigned short*) 0x402d4&= 0x9f;

    //set P15 as output port (as LCDPWR of C33L05)
    SET_P15_O;
    //set P16 as output port (as display on)
    SET_P16_O;
#else
    SET_PD6_H;
    SET_PD7_H;
    //set P15, P16 as GPIO
    //*(volatile unsigned short*) 0x402d4&= 0x9f;

    //set PD6 as output port (as LCDPWR of C33L05)
    SET_PD6_O;
    //set PD7 as output port (as display on)
    SET_PD7_O;
#endif

    //share pins of LCD : PB function select:FPDAT7--0
    PB_FUNC1_LCDC;
    PB_FUNC2_LCDC;

    //share pins of LCD : PC function select:DRDY, FPSHIFT, FPLINE, FPFRAME
    PC_FUNC_LCDC;


    //2) LCDC CLG enable: D7=1
    //LCDC clock divider ratio:(D3--0)=fosc3/16=pclk=3MHz
    for (i=1; i<16; i*=2)
    {
        dt = lcdclkdt & i;
        dt = ((*(volatile unsigned char*) 0x300f34 ) & ~i) | dt;
        *(volatile unsigned char*)  0x300f2f = 0x96; //remove write protection
        *(volatile unsigned char*)  0x300f34 = dt;
     };

     *(volatile unsigned char*)  0x300f2f = 0x96; //remove write protection
     *(volatile unsigned char*)  0x300f34 = lcdclkdt;

    /* ------ LCDC initialization ------- */

    //3) 16bpp, lut bypass, dither mode
    *(volatile unsigned short*) 0x380200 = 0x55; //set mode0:16bpp, lut bypass, dither mode

    //3) mono, 4bit
    #ifdef COLOR_PANEL
        // SET_CTRL_MODE1 = color type
        *(volatile unsigned short*)0x380202 = COLOR_8BIT_F2; //set mode1
    #else
        // SET_CTRL_MODE1 = mono type
        *(volatile unsigned short*)0x380202 = MONO_4BIT; //set mode1
    #endif

    //set hndp, hdp, vndp, vdp
    //SET_HNDP_COUNT;
    *(volatile unsigned short*) 0x380040 = 0x05; //set hndp count : (hndp+4)*8=72

    //SET_VNDP_COUNT;
    *(volatile unsigned short*) 0x38004a = 0x06; //set vndp count : vndp = 6

//    SET_CHAR_COUNT;
    *(volatile unsigned short*) 0x380042 = SCREEN_WIDTH / 8 - 1; //set horizontal panel size = 320
//    SET_LINE_COUNT;
    *(volatile unsigned short*) 0x38004c = SCREEN_HEIGHT - 1; //set vertical panel size = 240


    // SET VRAM_ADDR_1 as current display memory
    *(volatile unsigned short*) 0x380210 = (((unsigned long)VRAM_ADDR_1 - 0x2000000) / 32) | 0x8000;


    //7) PS (Normal)
    POWER_NORMAL;

    //Disable HSDMA ch.1
    *(volatile unsigned char*) 0x4823c=0x0;
    
    //9) delay
    for (i=100000; i>0; i--);

    //10) set P15 high(power on)
#ifdef L05DMT
    SET_P15_L;
#else
    SET_PD6_L;
#endif

    //delay
    for (i=100000; i>0; i--);

    //set P16 high(display on)
#ifdef L05DMT
    SET_P16_L;
#else
    SET_PD7_L;
#endif
}

static void LCD_Display_on (void)
{
    //*(volatile unsigned char *)LCD_Command = 0xAF;    // Display ON
}

static void LCD_Display_off (void)
{
    //*(volatile unsigned char *)LCD_Command = 0xAE;    // Display OFF
}

static void waitms (int ms)
{
    unsigned long i,j;
    if (ms<=0)
        return;

    for (i=0;i<ms;i++) {
        // 10MHz: 10000 Clocks
        // 20MHz: 20000 Clocks
        // 40MHz: 40000 Clocks
        for(j=0;j<1000;j++) {
            // 20 Clocks every circle, with 1 wait, and "-O" parameter in GCC33 make file
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        }
    }
}

static int L2F50113T00_open (void)
{
    LCD_initial (LCLG_dt4);
    LCD_Display_on ();
    waitms(1000);
    return 0;
}

static int L2F50113T00_getscreeninfo (struct commlcd_info *li)
{
    li->type = COMMLCD_TRUE_RGB565;
    li->height = SCREEN_HEIGHT;
    li->width = SCREEN_WIDTH;
    li->fb = (void*)VRAM_ADDR_1;
    li->bpp = LCD_BPP;
    li->rlen = SCREEN_WIDTH * LCD_BPP_UNIT;

    return 0;
}

static int L2F50113T00_close (void)
{
    LCD_Display_off ();
    return 0;
}

static int L2F50113T00_setpalette (int first,int count, GAL_Color *colors)
{
    /* no palette available*/
    return 0;
}

struct commlcd_ops __mg_commlcd_ops = {
    L2F50113T00_open,
    L2F50113T00_getscreeninfo,
    L2F50113T00_close,
    L2F50113T00_setpalette,
    NULL
};

#endif /* __TARGET_C33L05__ */

#endif /* _MGGAL_COMMLCD */

