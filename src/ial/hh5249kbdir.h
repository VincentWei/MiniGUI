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
** hh5249kbdir.h:. the head file of HH5249 PS/2 keyboard and IrDA IAL engine.
**
** Created by Wei Yongming, 2004/05/19
*/

#ifndef GUI_IAL_HH5249KBDIR_H
    #define GUI_IAL_HH5249KBDIR_H

#if 0
#define IRKEY_MUTE      0xa150  // 静音
#define IRKEY_POWEROFF  0xb140  // 关机
#define IRKEY_1         0xf000  // 1/10
#define IRKEY_2         0xe010  // 2/20
#define IRKEY_3         0xd020  // 3/30
#define IRKEY_4         0xc030  // 4
#define IRKEY_5         0xb040  // 5
#define IRKEY_6         0xa050  // 6
#define IRKEY_7         0x9060  // 7
#define IRKEY_8         0x8070  // 8
#define IRKEY_9         0x7080  // 9
#define IRKEY_0         0x6090  // 0
#define IRKEY_SND_INC   0xf100  // 声音升高
#define IRKEY_SND_DEC   0xe110  // 声音降低
#define IRKEY_CHNN_INC  0xd120  // 选台增加
#define IRKEY_CHNN_DEC  0xc130  // 选台减少
#define IRKEY_CLOCK     0x7180  // 时钟
#define IRKEY_AV        0X9160  // AV
#define IRKEY_BACK      0x51a0  // 返回
#define IRKEY_DISP_CHNN 0x6190  // 显示频道
#define IRKEY_LUM_HUE   0x11e0  // 亮度/色度选择
#define IRKEY_LH_INC    0x31c0  // 亮度/色度增加
#define IRKEY_LH_DEC    0x21d0  // 亮度/色度减少
#else
#define	IRKEY_1		0xa257
#define	IRKEY_2		0xc037
#define	IRKEY_3		0xa057
#define	IRKEY_4		0x8077
#define	IRKEY_5		0x6097
#define	IRKEY_6		0x40b7
#define	IRKEY_7		0x20d7
#define	IRKEY_8		0xf7
#define	IRKEY_9		0xe117
#define	IRKEY_10	0xc137
#define	IRKEY_11	0xe017
#define	IRKEY_12	0x8177
#define	IRKEY_13	0xab57
#define	IRKEY_14	0xc237
#define	IRKEY_15	0xc837
#define	IRKEY_16	0x6397
#define	IRKEY_17	0x22d7
#define	IRKEY_18	0xe217
#define	IRKEY_19	0x6297
#define	IRKEY_20	0x8377
#define	IRKEY_21	0xe817
#define	IRKEY_22	0x23d7
#define	IRKEY_23	0x8b77
#define	IRKEY_24	0xa357
#define	IRKEY_25	0x3f7
#define	IRKEY_26	0x48b7
#define	IRKEY_27	0x42b7
#define	IRKEY_28	0x6997
#define	IRKEY_29	0x1f7
#define	IRKEY_30	0x4ab7
#define	IRKEY_31	0xaf7
#define	IRKEY_32	0x41b7
#define	IRKEY_33	0xaa57
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitHH5249KbdIrInput (INPUT* input, const char* mdev, const char* mtype);
void    TermHH5249KbdIrInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_HH5249KBDIR_H */

