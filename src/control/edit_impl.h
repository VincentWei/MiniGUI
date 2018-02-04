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
** edit_impl.h: the head file of Edit Control module.
**
** Create date: 1999/8/26
*/

#ifndef GUI_EDIT_IMPL_H_
#define GUI_EDIT_IMPL_H_


#ifdef  __cplusplus
extern  "C" {
#endif


#ifdef _FLAT_WINDOW_STYLE
#define WIDTH_EDIT_BORDER       1
#else
#define WIDTH_EDIT_BORDER       2
#endif

#define MARGIN_EDIT_LEFT        1
#define MARGIN_EDIT_TOP         1
#define MARGIN_EDIT_RIGHT       2
#define MARGIN_EDIT_BOTTOM      1

/* default line buffer size */
#define DEF_LINE_BUFFER_SIZE    128
/* default line step block size */
#define DEF_LINE_BLOCK_SIZE     32
/* default tip length */
#define DEF_TIP_LEN             63

#define LEN_SLEDIT_UNDOBUFFER   256

#define EST_FOCUSED     0x00000001L
#define EST_MODIFY      0x00000002L
#define EST_TMP         0x00000004L
#define EST_REPLACE     0x00000008L

#define EDIT_OP_NONE    0x00
#define EDIT_OP_DELETE  0x01
#define EDIT_OP_INSERT  0x02
#define EDIT_OP_REPLACE 0x03


typedef struct tagSLEDITDATA
{
    DWORD   status;         // status of box

    int     editPos;        // current edit position
    int     selStart;       // selection start position
    int     selEnd;         // selection end position
 
    int     nContX;         // content x offset position
    int     nContW;         // content width
    RECT    rcVis;          // content rect
    int     nVisW;          // visible area width
    int     startx;         // text x display position
    int     starty;         // text y display position

    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin

    int     nBlockSize;     /* string buffer block size */
    StrBuffer content;      // string text buffer
    char    *tiptext;       // tip text

    int     hardLimit;      // hard limit

    int     passwdChar;     // password character

    int     changed;        // change flag

    BOOL    bShowCaret;     // flags of caret
    
#if 0
    int     lastOp;         // last operation
    int     lastPos;        // last operation position
    int     affectedLen;    // affected len of last operation
    int     undoBufferLen;  // undo buffer len
    char    undoBuffer [LEN_SLEDIT_UNDOBUFFER];
                            // Undo buffer;
#endif

}SLEDITDATA;
typedef SLEDITDATA* PSLEDITDATA;
    

BOOL RegisterSLEditControl (void);
#ifdef __cplusplus
}
#endif

#endif /* GUI_EDIT_IMPL_H_ */

