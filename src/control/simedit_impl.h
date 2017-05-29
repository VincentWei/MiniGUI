/*
** $Id: simedit_impl.h 8944 2007-12-29 08:29:16Z xwyan $
**
** simedit.h: the head file of Simple Edit Control module.
**
** Copyright (C) 2003 ~ 2007, Feynman Software.
** Copyright (C) 1999~2002, Wei Yongming.
**
** Create date: 1999/8/26
*/

#ifndef GUI_SIMEDIT_IMPL_H_
#define GUI_SIMEDIT_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LEN_SIMEDIT_BUFFER       512

typedef struct tagSIMEDITDATA
{
    DWORD   status;         // status of box

    int     bufferLen;      // length of buffer
    char*   buffer;         // buffer

    int     dataEnd;        // data end position
    int     editPos;        // current edit position
    int     caretOff;       // caret offset in box
    int     startPos;       // start display position
    
    int     charWidth;      // width of a character
    
    char    passwdChar;     // password character
    
    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin
    
    int     hardLimit;      // hard limit

} SIMEDITDATA;
typedef SIMEDITDATA* PSIMEDITDATA;
    
BOOL RegisterSIMEditControl (void);

#ifdef __cplusplus
}
#endif

#endif // GUI_SIMEDIT_IMPL_H_

