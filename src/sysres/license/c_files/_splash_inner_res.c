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
//inner list file
//create by inner-trans

#ifdef _MG_ENABLE_LICENSE

//include files
extern unsigned char __mg_splash_dat_00_minigui_data[]; //00_minigui.dat.c 
extern unsigned char __mg_splash_dat_01_fmsoft_data[]; //01_fmsoft.dat.c 
extern unsigned char __mg_splash_dat_02_feiman_data[]; //02_feiman.dat.c 
extern unsigned char __mg_splash_dat_03_progressbar_data[]; //03_progressbar.dat.c 
extern unsigned char __mg_splash_dat_04_progressbar_bk_data[]; //04_progressbar-bk.dat.c 

//declear arrays
static INNER_RES __mg_splash_splash_inner_res[]={
	{ 0x1562B2, (void*)__mg_splash_dat_00_minigui_data, 18031, "dat"}, //00_minigui.dat.c
	{ 0xCD582, (void*)__mg_splash_dat_01_fmsoft_data, 3798, "dat"}, //01_fmsoft.dat.c
	{ 0xC8712, (void*)__mg_splash_dat_02_feiman_data, 10012, "dat"}, //02_feiman.dat.c
	{ 0x536B0E, (void*)__mg_splash_dat_03_progressbar_data, 2857, "dat"}, //03_progressbar.dat.c
	{ 0xC97F8E, (void*)__mg_splash_dat_04_progressbar_bk_data, 2926, "dat"}, //04_progressbar-bk.dat.c
};

#endif // _MG_ENABLE_LICENSE

