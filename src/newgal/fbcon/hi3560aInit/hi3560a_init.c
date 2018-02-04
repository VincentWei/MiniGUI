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
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h>

#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hifb.h"

#define SCREENWIDTH     720
#define SCREENHEIGHT    576
struct fb_bitfield stR32 = {16, 8, 0};            
struct fb_bitfield stG32 = {8, 8, 0};            
struct fb_bitfield stB32 = {0, 8, 0};            
struct fb_bitfield stA32 = {24, 8, 0};   

int main(int argc, char **argv)
{
    int fd;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    unsigned char *pShowScreen;
    HIFB_POINT_S stPoint = {0, 0};

    HI_UNF_DISP_INIT_PARA_S stPara;
    HI_HANDLE Handle;
    HIFB_FLUSHTYPE_E enFlushType;
    HI_UNF_OUTPUT_INTERFACE_S   stVO_Mode ;
    stPara.u32ID = 0;

    HI_UNF_DISP_Init();
    HI_UNF_DISP_Create(&stPara, &Handle);
    HI_UNF_DISP_SetEnable(Handle, HI_TRUE);
    stVO_Mode.enDacMode[0] = HI_UNF_DAC_MODE_PR ;
    stVO_Mode.enDacMode[1] = HI_UNF_DAC_MODE_PB ;
    stVO_Mode.enDacMode[2] = HI_UNF_DAC_MODE_Y ;
    stVO_Mode.enDacMode[3] = HI_UNF_DAC_MODE_CVBS ;
    HI_UNF_DISP_SetDacMode( Handle , &stVO_Mode);


    fd = open("/dev/fb/0", O_RDWR);
    if(fd < 0) {
        printf("open fb0 failed!\n");
        return -1;
    }

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0) {
        printf("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0) {
        printf("Get variable screen info failed!\n");
        close(fd);
        return -1;
    }

    var.xres = var.xres_virtual = SCREENWIDTH;
    var.yres = SCREENHEIGHT;
    var.yres_virtual = SCREENHEIGHT*2;
    var.transp= stA32;
    var.red = stR32;
    var.green = stG32;
    var.blue = stB32;
    var.bits_per_pixel = 32;

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0) {
        printf("Put variable screen info failed!\n");
        close(fd);
        return -1;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        printf("Get fix screen info failed!\n");
        close(fd);
        return -1;
    }

    close(fd);
    int quit = 0;
    while (!quit) {
        usleep (200000);
    }

    HI_UNF_DISP_SetEnable(Handle, HI_FALSE);
    HI_UNF_DISP_Destroy(Handle);
    HI_UNF_DISP_DeInit();

    return 0;
}

