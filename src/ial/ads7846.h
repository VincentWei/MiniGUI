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
** ads7846.h:. the head file of Low Level Input Engine for Arca Tpanel Ads7846.
**
** Created by PengKe, 2004/05/17
*/

#ifndef GUI_IAL_ADS7846_H
    #define GUI_IAL_ADS7846_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#define CMD_SET_MINXY         1
#define CMD_SET_MAXXY         2
#define CMD_GET_VAL           3
#define CMD_SET_CAL_ENABLE    4
#define CMD_SET_CAL_DISABLE   5
#define CMD_SET_LCDXY         6
#define CMD_SET_HOLD_DELAY    7
 
#define TPANEL_DATA_IS_PENDOWN      0x0001
#define TPANEL_DATA_IS_PENUP        0x0002
#define TPANEL_DATA_IS_DOWNHOLD     0x0004
#define TPANEL_DATA_IS_UNAVAILABLE  0x0008



typedef struct {
    unsigned int status;
    unsigned int x;
    unsigned int y;
} POS;

BOOL InitAds7846Input (INPUT* input, const char* mdev, const char* mtype);
void TermAds7846Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_ADS7846_H */


