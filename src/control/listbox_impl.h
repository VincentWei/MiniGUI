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
** listbox.h: the head file of ListBox control.
**
** Create date: 1999/8/31
*/

#ifndef __LISTBOX_IMPL_H_
#define __LISTBOX_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LBIF_NORMAL         0x0000L
#define LBIF_SELECTED       0x0001L
#define LBIF_USEBITMAP      0x0002L

#define LBIF_DISABLE        0x0004L
#define LBIF_BOLDSTYLE      0x0008L

#define LBIF_BLANK          0x0000L
#define LBIF_CHECKED        0x0010L
#define LBIF_PARTCHECKED    0x0020L
#define LBIF_SIGNIFICANT    0x0040L
#define LBIF_CHECKMARKMASK  0x00F0L

typedef struct _LISTBOXITEM 
{
    char*   key;                // item sort key
    DWORD   dwFlags;            // item flags
    DWORD   dwImage;            // item image
    DWORD   dwAddData;          // item additional data
    struct  _LISTBOXITEM* next; // next item
} LISTBOXITEM;
typedef LISTBOXITEM* PLISTBOXITEM;

#ifndef _MGRM_THREADS
    #define DEF_LB_BUFFER_LEN       8
#else
    #define DEF_LB_BUFFER_LEN       16
#endif

#define LBF_FOCUS               0x0001

typedef struct tagLISTBOXDATA
{
    DWORD dwFlags;          // listbox flags

    STRCMP str_cmp;         // function to compare two string

    int itemLeft;           // display left of item
    int itemWidth;          // display width of item 
    int itemMaxWidth;       // max width of all items
    
    int itemCount;          // items count
    int itemTop;            // start display item
    int itemVisibles;       // number of visible items

    int itemHilighted;      // current hilighted item
    int itemHeight;         // item height

    int selItem;            // record last selected item when use LBS_MOUSEFOLLOW style 
    LISTBOXITEM* head;      // items linked list head

    int buffLen;            // buffer length
    LISTBOXITEM* buffStart; // buffer start
    LISTBOXITEM* buffEnd;   // buffer end
    LISTBOXITEM* freeList;  // free list in buffer 
    SBPolicyType sbPolicy;  // scrollbar policy type
    
    PLOGFONT lst_font;       // logic font for bold display 
} LISTBOXDATA;
typedef LISTBOXDATA* PLISTBOXDATA;   

BOOL RegisterListboxControl (void);

#ifdef  __cplusplus
}
#endif

#endif  // __LISTBOX_IMPL_H__

