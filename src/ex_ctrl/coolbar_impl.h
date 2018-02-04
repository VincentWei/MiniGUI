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
** Create date: 2000/9/20
*/


#ifndef __COOLBAR_IMPL_H_
#define __COOLBAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LEN_HINT    50
#define LEN_TITLE   10

typedef struct coolbarCTRL
{
    int    	nCount;
    int 	ItemWidth;
    int		ItemHeight;
    PBITMAP BackBmp;
    struct 	coolbarItemData* head;
    struct 	coolbarItemData* tail;
    struct 	coolbarCTRL* nline;     // control linked list next
    int 	iSel; 				    // selected selected pic's insPos
    int 	iMvOver;
    BOOL    ShowHint;
    HWND    hToolTip;
	// add end
 }COOLBARCTRL;
typedef COOLBARCTRL* PCOOLBARCTRL;

typedef struct coolbarItemData
{
      RECT      RcTitle;            // title and where clicked
      int       hintx, hinty;       // position of hint box
      int       id;                 // id
      int       insPos;
      int	    ItemType;	
      BOOL	    Disable;
      PBITMAP   Bmp;
      char      Hint [LEN_HINT + 1];
      char      Caption[LEN_TITLE + 1];
      HMENU	    ItemMenu; 
      struct    coolbarItemData* next; //page linked list next
} COOLBARITEMDATA;
typedef  COOLBARITEMDATA* PCOOLBARITEMDATA;

BOOL RegisterCoolBarControl (void);
#ifdef  __cplusplus
}
#endif

#endif // GUI_COOBAR_IMPL_H_


