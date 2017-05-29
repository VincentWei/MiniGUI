/*
** $Id: edit_impl.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** edit_impl.h: the head file of Edit Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
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

