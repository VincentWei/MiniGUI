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

#ifdef _MGINCORE_RES

//include files
extern unsigned char _mgir_bmp_classic_check_button_data[]; //bmp/classic_check_button.bmp.c 
extern unsigned char _mgir_bmp_classic_radio_button_data[]; //bmp/classic_radio_button.bmp.c 

#ifdef _MGLF_RDR_FLAT
extern unsigned char _mgir_bmp_flat_check_button_data[]; //bmp/flat_check_button.bmp.c 
extern unsigned char _mgir_bmp_flat_radio_button_data[]; //bmp/flat_radio_button.bmp.c 
#endif

#ifdef _MGLF_RDR_SKIN
extern unsigned char _mgir_gif_skin_arrows_data[]; //bmp/skin_arrows.gif.c 
extern unsigned char _mgir_bmp_skin_arrows_shell_data[]; //bmp/skin_arrows_shell.bmp.c 
extern unsigned char _mgir_bmp_skin_bborder_data[]; //bmp/skin_bborder.bmp.c 
extern unsigned char _mgir_bmp_skin_bkgnd_data[]; //bmp/skin_bkgnd.bmp.c 
extern unsigned char _mgir_gif_skin_caption_data[]; //bmp/skin_caption.gif.c 
extern unsigned char _mgir_bmp_skin_checkbtn_data[]; //bmp/skin_checkbtn.bmp.c 
extern unsigned char _mgir_gif_skin_cpn_btn_data[]; //bmp/skin_cpn_btn.gif.c 
extern unsigned char _mgir_bmp_skin_header_data[]; //bmp/skin_header.bmp.c 
extern unsigned char _mgir_bmp_skin_lborder_data[]; //bmp/skin_lborder.bmp.c 
extern unsigned char _mgir_gif_skin_pb_htrack_data[]; //bmp/skin_pb_htrack.gif.c 
extern unsigned char _mgir_bmp_skin_pb_htruck_data[]; //bmp/skin_pb_htruck.bmp.c 
extern unsigned char _mgir_gif_skin_pb_vtrack_data[]; //bmp/skin_pb_vtrack.gif.c 
extern unsigned char _mgir_bmp_skin_pb_vtruck_data[]; //bmp/skin_pb_vtruck.bmp.c 
extern unsigned char _mgir_gif_skin_pushbtn_data[]; //bmp/skin_pushbtn.gif.c 
extern unsigned char _mgir_gif_skin_radiobtn_data[]; //bmp/skin_radiobtn.gif.c 
extern unsigned char _mgir_bmp_skin_rborder_data[]; //bmp/skin_rborder.bmp.c 
extern unsigned char _mgir_bmp_skin_sb_arrows_data[]; //bmp/skin_sb_arrows.bmp.c 
extern unsigned char _mgir_bmp_skin_sb_hshaft_data[]; //bmp/skin_sb_hshaft.bmp.c 
extern unsigned char _mgir_bmp_skin_sb_hthumb_data[]; //bmp/skin_sb_hthumb.bmp.c 
extern unsigned char _mgir_bmp_skin_sb_vshaft_data[]; //bmp/skin_sb_vshaft.bmp.c 
extern unsigned char _mgir_bmp_skin_sb_vthumb_data[]; //bmp/skin_sb_vthumb.bmp.c 
extern unsigned char _mgir_gif_skin_tab_data[]; //bmp/skin_tab.gif.c 
extern unsigned char _mgir_gif_skin_tb_horz_data[]; //bmp/skin_tb_horz.gif.c 
extern unsigned char _mgir_bmp_skin_tborder_data[]; //bmp/skin_tborder.bmp.c 
extern unsigned char _mgir_gif_skin_tbslider_h_data[]; //bmp/skin_tbslider_h.gif.c 
extern unsigned char _mgir_gif_skin_tbslider_v_data[]; //bmp/skin_tbslider_v.gif.c 
extern unsigned char _mgir_gif_skin_tb_vert_data[]; //bmp/skin_tb_vert.gif.c 
extern unsigned char _mgir_bmp_skin_tree_data[]; //bmp/skin_tree.bmp.c 
#endif

//declear arrays
static INNER_RES __mgir_bmp_inner_res[]={
	{ 0xA1C8F16, (void*)_mgir_bmp_classic_check_button_data, 4110, "bmp"}, //bmp/classic_check_button.bmp.c
	{ 0xA1BCFD6, (void*)_mgir_bmp_classic_radio_button_data, 3510, "bmp"}, //bmp/classic_radio_button.bmp.c


#ifdef _MGLF_RDR_FLAT
	{ 0x35C7BA2, (void*)_mgir_bmp_flat_check_button_data, 4110, "bmp"}, //bmp/flat_check_button.bmp.c
	{ 0x35C4802, (void*)_mgir_bmp_flat_radio_button_data, 4110, "bmp"}, //bmp/flat_radio_button.bmp.c
#endif


#ifdef _MGLF_RDR_SKIN
	{ 0x69FFF6, (void*)_mgir_gif_skin_arrows_data, 170, "gif"}, //bmp/skin_arrows.gif.c
	{ 0x35DCDF6, (void*)_mgir_bmp_skin_arrows_shell_data, 3128, "bmp"}, //bmp/skin_arrows_shell.bmp.c
	{ 0xABFEB6, (void*)_mgir_bmp_skin_bborder_data, 1600, "bmp"}, //bmp/skin_bborder.bmp.c
	{ 0x504416, (void*)_mgir_bmp_skin_bkgnd_data, 230456, "bmp"}, //bmp/skin_bkgnd.bmp.c
	{ 0x8C4256, (void*)_mgir_gif_skin_caption_data, 1272, "gif"}, //bmp/skin_caption.gif.c
	{ 0xDDF636, (void*)_mgir_bmp_skin_checkbtn_data, 4216, "bmp"}, //bmp/skin_checkbtn.bmp.c
	{ 0x8D4736, (void*)_mgir_gif_skin_cpn_btn_data, 4199, "gif"}, //bmp/skin_cpn_btn.gif.c
	{ 0x6BA656, (void*)_mgir_bmp_skin_header_data, 4088, "bmp"}, //bmp/skin_header.bmp.c
	{ 0xAB1EB6, (void*)_mgir_bmp_skin_lborder_data, 1576, "bmp"}, //bmp/skin_lborder.bmp.c
	{ 0x11F3CB6, (void*)_mgir_gif_skin_pb_htrack_data, 318, "gif"}, //bmp/skin_pb_htrack.gif.c
	{ 0x1472CB6, (void*)_mgir_bmp_skin_pb_htruck_data, 440, "bmp"}, //bmp/skin_pb_htruck.bmp.c
	{ 0x11F3B36, (void*)_mgir_gif_skin_pb_vtrack_data, 310, "gif"}, //bmp/skin_pb_vtrack.gif.c
	{ 0x1472B36, (void*)_mgir_bmp_skin_pb_vtruck_data, 416, "bmp"}, //bmp/skin_pb_vtruck.bmp.c
	{ 0x8FDA56, (void*)_mgir_gif_skin_pushbtn_data, 1891, "gif"}, //bmp/skin_pushbtn.gif.c
	{ 0xD59796, (void*)_mgir_gif_skin_radiobtn_data, 1857, "gif"}, //bmp/skin_radiobtn.gif.c
	{ 0xAAFEB6, (void*)_mgir_bmp_skin_rborder_data, 1576, "bmp"}, //bmp/skin_rborder.bmp.c
	{ 0x1538AF6, (void*)_mgir_bmp_skin_sb_arrows_data, 14200, "bmp"}, //bmp/skin_sb_arrows.bmp.c
	{ 0x153D4B6, (void*)_mgir_bmp_skin_sb_hshaft_data, 2440, "bmp"}, //bmp/skin_sb_hshaft.bmp.c
	{ 0x14E1FB6, (void*)_mgir_bmp_skin_sb_hthumb_data, 2520, "bmp"}, //bmp/skin_sb_hthumb.bmp.c
	{ 0x153D336, (void*)_mgir_bmp_skin_sb_vshaft_data, 2440, "bmp"}, //bmp/skin_sb_vshaft.bmp.c
	{ 0x14E1836, (void*)_mgir_bmp_skin_sb_vthumb_data, 4280, "bmp"}, //bmp/skin_sb_vthumb.bmp.c
	{ 0x231BD6, (void*)_mgir_gif_skin_tab_data, 957, "gif"}, //bmp/skin_tab.gif.c
	{ 0x835BB6, (void*)_mgir_gif_skin_tb_horz_data, 1362, "gif"}, //bmp/skin_tb_horz.gif.c
	{ 0xAA9EB6, (void*)_mgir_bmp_skin_tborder_data, 1600, "bmp"}, //bmp/skin_tborder.bmp.c
	{ 0x18623B6, (void*)_mgir_gif_skin_tbslider_h_data, 168, "gif"}, //bmp/skin_tbslider_h.gif.c
	{ 0x1861FB6, (void*)_mgir_gif_skin_tbslider_v_data, 167, "gif"}, //bmp/skin_tbslider_v.gif.c
	{ 0x86DC36, (void*)_mgir_gif_skin_tb_vert_data, 1451, "gif"}, //bmp/skin_tb_vert.gif.c
	{ 0x345EF6, (void*)_mgir_bmp_skin_tree_data, 560, "bmp"}, //bmp/skin_tree.bmp.c
#endif
};

#endif // _MGINCORE_RES

