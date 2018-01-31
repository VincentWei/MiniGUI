/**
 * \file edit.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: edit.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_EDIT_H
#define _MGUI_CTRL_EDIT_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_edit Edit/MEdit control
     *
     * @{
     */

/**
 * \def CTRL_EDIT
 * \brief The class name of simple single-line editor box.
 *
 * This edit control uses the system default fixed logical font.
 */
#define CTRL_EDIT           ("edit")

/**
 * \def CTRL_SLEDIT
 * \brief The class name of single-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font.
 */
#define CTRL_SLEDIT         ("sledit")

/**
 * \def CTRL_BIDISLEDIT
 * \brief The class name of BIDI single-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font.
 */
#define CTRL_BIDISLEDIT     ("bidisledit")

/**
 * \def CTRL_MLEDIT
 * \brief The class name of multiple-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font.
 */
#define CTRL_MLEDIT         ("mledit")

/**
 * \def CTRL_MEDIT
 * \brief Another class name of multiple-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font.
 */
#define CTRL_MEDIT          ("medit")

    /**
     * \defgroup ctrl_edit_styles Styles of edit control
     * @{
     */

/**
 * \def ES_LEFT
 * \brief Left-aligned text.
 */
#define ES_LEFT             0x00000000L

/**
 * \def ES_CENTER
 * \brief Center-aligned text.
 */
#define ES_CENTER           0x00000001L

/**
 * \def ES_RIGHT
 * \brief Right-aligned text.
 */
#define ES_RIGHT            0x00000002L

/**
 * \def ES_MULTILINE
 * \brief Multi-line text.
 */
#define ES_MULTILINE        0x00000004L

/**
 * \def ES_UPPERCASE
 * \brief Converts all characters to uppercase as they are typed into the edit control.
 */
#define ES_UPPERCASE        0x00000008L

/**
 * \def ES_LOWERCASE
 * \brief Converts all characters to lowercase as they are typed into the edit control.
 */
#define ES_LOWERCASE        0x00000010L

/**
 * \def ES_PASSWORD
 * \brief Displays an asterisk (*) for each character typed into the edit control.
 */
#define ES_PASSWORD         0x00000020L

/**
 * \def ES_AUTOVSCROLL
 * \brief Show and hide the vertical scroll bar automatically
 */
#define ES_AUTOVSCROLL      0x00000040L

/**
 * \def ES_AUTOHSCROLL
 * \brief Show and hide the horizontal scroll bar automatically
 */
#define ES_AUTOHSCROLL      0x00000080L

/**
 * \def ES_NOHIDESEL
 * \brief Edit control with this style will remain selected when focus is lost
 */
#define ES_NOHIDESEL        0x00000100L

/**
 * \def ES_AUTOSELECT
 * \brief Selects all text when getting focus
 */
#define ES_AUTOSELECT       0x00000400L
//#define ES_OEMCONVERT       0x00000400L

/**
 * \def ES_READONLY
 * \brief Prevents the user from typing or editing text in the edit control.
 */
#define ES_READONLY         0x00000800L

/**
 * \def ES_BASELINE
 * \brief Draws base line under input area instead of frame border.
 */
#define ES_BASELINE         0x00001000L

/**
 * \def ES_AUTOWRAP
 * \brief Automatically wraps against border when inputting.
 */
#define ES_AUTOWRAP         0x00002000L

/**
 * \def ES_TITLE
 * \brief Shows specified title texts.
 */
#define ES_TITLE            0x00004000L

/**
 * \def ES_TIP
 * \brief Shows specified tip texts.
 */
#define ES_TIP              0x00008000L

    /** @} end of ctrl_edit_styles */

    /**
     * \defgroup ctrl_edit_msgs Messages of edit control
     * @{
     */

/**
 * \def EM_GETSEL
 * \brief Gets the selected string in the edit control.
 *
 * \code
 * EM_GETSEL
 *
 * char *buffer;
 * int len;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len Length of buffer.
 * \param buffer Pointer to the string buffer
 *
 * \return Length of the selected string
 */
#define EM_GETSEL               0xF0B0

/**
 * \def EM_SETSEL
 * \brief Sets the selected point in the edit control and makes
 *        the text between insertion point and selection point selected.
 *
 *        Generally, you should send EM_SETCARETPOS first to set insertion
 *        point before you use EM_SETSEL to select text.
 *
 * \code
 * EM_SETSEL
 *
 * int line_pos;
 * int char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Line position of the selection point.
 *                 For single line editor, it is always zero.
 *                 Note : For multi-line editor, "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Character(wide character) position of the selection point.
 *
 * \return Length of the selected string
 */
#define EM_SETSEL               0xF0B1

/**
 * \def EM_SETSELPOS
 * \sa EM_SETSEL
 */
#define EM_SETSELPOS            EM_SETSEL  

/**
 * \def EM_SETSELECTION
 * \sa EM_SETSEL
 */
#define EM_SETSELECTION         EM_SETSEL

/**
 * \def EM_SELECTALL
 * \brief Selects all the texts, the same meaning as ctrl+a
 *        
 * \code
 * EM_SELECTALL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define EM_SELECTALL              0xF0B2

/**
 * \def EM_GETSELPOS
 * \brief Gets the position of the selection point.
 *
 * \code
 * EM_GETSELPOS
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the selection line position.
 *                 For single line editor, it is always zero.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Pointer to a integer buffer to save the selection character position.
 *
 * \return The string length of the text from the beginning to the selection point.
 */
#define EM_GETSELPOS              0xF0B3

/**
 * \def EM_INSERTCBTEXT
 * \brief Inserts the text in the clipboard to the current caret position
 *
 * \code
 * EM_INSERTCBTEXT
 * int len;
 * const char *string;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param len Length of string
 * \param string Pointer to the text string
 */
#define EM_INSERTCBTEXT           0xF0B4

/**
 * \def EM_COPYTOCB
 * \brief Copies the currently selected text to the clipboard
 *
 * \code
 * EM_COPYTOCB
 *
 * wParam = 0;
 * lParam = 0
 * \endcode
 *
 * \return Length of the text which is really copied to clipboard.
 */
#define EM_COPYTOCB               0xF0B5

/**
 * \def EM_CUTTOCB
 * \brief Cuts the currently selected text to the clipboard
 *
 * \code
 * EM_CUTTOCB
 *
 * wParam = 0;
 * lParam = 0
 * \endcode
 *
 * \return Length of the text which is really copied to clipboard.
 */
#define EM_CUTTOCB               0xF0B6

/**
 * \def EM_SETLFDISPCHAR
 * \brief Sets the char used to represent the line seperator.
 *        
 * In default case, the line sperator will not be shown.
 * If the char used to represent the line seperator is not zero,
 *    this char will be shown in place of line seperator.
 *
 * \code
 * EM_SETLFDISPCHAR
 * unsigned char ch;
 *
 * wParam = 0;
 * lParam = ch;
 * \endcode
 *
 * \param ch The char used to represent the line seperator
 */
#define EM_SETLFDISPCHAR          0xF0B7

/**
 * \def EM_SETLINESEP
 * \brief Sets the line seperator.
 *        
 * In default case, the line sperator is Line Feed
 * characters(LF , ASCII 10, '\n').
 *
 * \code
 * EM_SETLINESEP
 * unsigned char ch;
 *
 * wParam = 0;
 * lParam = ch;
 * \endcode
 *
 * \param ch The new line seperator
 */
#define EM_SETLINESEP             0xF0B8

/* #define EM_GETRECT              0xF0B2 */
/* #define EM_SETRECT              0xF0B3 */
/* #define EM_SETRECTNP            0xF0B4 */
/* #define EM_SCROLL               0xF0B5 */

/**
 * \def EM_GETCARETPOS
 * \brief Gets the position of the caret.
 *
 * \code
 * EM_GETCARETPOS
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the caret line position.
 *                 For single line editor, it is always zero.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Pointer to a integer buffer to save the caret character position.
 *
 * \return The string length of the text from the beginning to the caret pos.
 */
#define EM_GETCARETPOS          0xF0B9

/**
 * \def EM_SETCARETPOS
 * \brief Sets the position of the caret.
 *
 * \code
 * EM_SETCARETPOS
 * int line_pos;
 * int char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos The new caret line position. For single line editor, it will be ignored.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos The new caret character position.
 *
 * \return Length of the string from the beginning to the caret position 
 *         on success, otherwise -1.
 */
#define EM_SETCARETPOS          0xF0BA

/**
 * \def EM_SETINSERTION
 * \sa EM_SETCARETPOS
 */
#define EM_SETINSERTION         EM_SETCARETPOS

/* #define EM_SCROLLCARET          0xF0B9 */
/* #define EM_GETMODIFY            0xF0BA */
/* #define EM_SETMODIFY            0xF0BB */

/**
 * \def EM_GETLINECOUNT
 * \brief Gets the line number.
 *
 * \code
 * EM_GETLINECOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Line number on success, otherwise -1.
 * \note Implemented for TextEdit control.
 */
#define EM_GETLINECOUNT         0xF0BC

/**
 * \def EM_GETLINEHEIGHT
 * \brief Gets the height of a line.
 *
 * \code
 * EM_GETLINEHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height value.
 * \note Implemented for TextEdit control.
 */
#define EM_GETLINEHEIGHT        0xF0BD

/**
 * \def EM_SETLINEHEIGHT
 * \brief Sets the height of a line.
 *
 * \code
 * EM_SETLINEHEIGHT
 *
 * wParam = (WPARAM)height;
 * lParam = 0;
 * \endcode
 *
 * \return The old height value.
 * \note Implemented for TextEdit control.
 */
#define EM_SETLINEHEIGHT        0xF0BE


/*#define EM_LINEINDEX            0xF0BD */
/*#define EM_GETTHUMB             0xF0BE */

/* internal used now */
/**
 * \def EM_LINESCROLL
 * \brief reserved. 
 *        
 */
#define EM_LINESCROLL           0xF0BF
 
/**
 * \def EM_INSERTTEXT
 * \brief Inserts the specified text to the current caret position
 *
 * Normally used to input a long string.
 *
 * \code
 * EM_INSERTTEXT
 * int len;
 * const char *string;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param len Length of string
 * \param string Pointer to the text string
 */
#define EM_INSERTTEXT           0xF0C0

/* Not use */
/**
 * \def EM_LINELENGTH
 * \brief reserved. 
 *        
 */
#define EM_LINELENGTH           0xF0C1

/* Not use */
/**
 * \def EM_REPLACESEL
 * \brief reserved. 
 *        
 */
#define EM_REPLACESEL           0xF0C2

/**
 * \def EM_GETMAXLIMIT
 * \brief Get text limit of a single-line edit control.
 */
#define EM_GETMAXLIMIT            0xF0C3

/* Not use */
/**
 * \def EM_GETLINE
 * \brief reserved. 
 *        
 */
#define EM_GETLINE              0xF0C4

/**
 * \def EM_LIMITTEXT
 * \brief Set text limit of an edit control.
 *
 * \code
 * EM_LIMITTEXT
 * int newLimit;
 *
 * wParam = (WPARAM)newLimit;
 * lParam = 0;
 * \endcode
 *
 * \param newLimit The new text limit of an edit control, by character.
 */
#define EM_LIMITTEXT            0xF0C5

/**
 * \def EM_REDO
 * \brief Redo operation.
 *
 * \code
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 */
#define EM_REDO                 0xF0C6
/*#define EM_CANUNDO              0xF0C6 */
 
/**
 * \def EM_UNDO
 * \brief Undo operation.
 *
 * \code
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 */
#define EM_UNDO                 0xF0C7

/* Not use */
/**
 * \def EM_FMTLINES
 * \brief reserved. 
 *        
 */
#define EM_FMTLINES             0xF0C8

/* Not use */
/**
 * \def EM_LINEFROMCHAR
 * \brief reserved. 
 *        
 */
#define EM_LINEFROMCHAR         0xF0C9

/* Not use */
/**
 * \def EM_SETTABSTOPS
 * \brief reserved. 
 *        
 */
#define EM_SETTABSTOPS          0xF0CB

/**
 * \def EM_SETPASSWORDCHAR
 * \brief Defines the character that edit control uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_SETPASSWORDCHAR
 * char passwdChar;
 *
 * wParam = (WPARAM)passwdChar;
 * lParam = 0;
 * \endcode
 */
#define EM_SETPASSWORDCHAR      0xF0CC

/* Not use */
#define EM_EMPTYUNDOBUFFER      0xF0CD
/* Not use */
#define EM_GETFIRSTVISIBLELINE  0xF0CE

/**
 * \def EM_SETREADONLY
 * \brief Sets or removes the read-only style (ES_READONLY) in an edit control.
 *
 * \code
 * EM_SETREADONLY
 * int readonly;
 *
 * wParam = (WPARAM)readonly;
 * lParam = 0;
 * \endcode
 *
 * \param readonly Indicates whether the edit control is read-only:
 *      - Zero\n
 *        Not read-only.
 *      - Non zero\n
 *        Read-only.
 */
#define EM_SETREADONLY          0xF0CF

/**
 * \var typedef int (*ED_DRAWSEL_FUNC) (HWND hWnd, HDC hdc, int startx, int starty, const char* content, int len, int selout)
 * \brief Type of the edit control callback function on drawing selected strings.
 *
 * \param hWnd Handle of the edit control.
 * \param startx x value of the beginning drawing position.
 * \param starty y value of the beginning drawing position.
 * \param content The string which will be drawed.
 * \param len Length of the string which should be drawed by this callback.
 * \param selout Length of the selected string that have been drawed before calling this callback function.
 * 
 * \return Width of the outputed strings.
 */
typedef int (*ED_DRAWSEL_FUNC) (HWND, HDC, int, int, const char*, int, int);


/**
 * \def EM_SETDRAWSELECTFUNC
 * \brief Sets the callback function on drawing selected chars
 *
 * \code
 * EM_SETDRAWSELECTFUNC
 * ED_DRAWSEL_FUNC drawsel;
 *
 * wParam = 0;
 * lParam = (LPARAM)drawsel;
 * \endcode
 *
 * \param drawsel The callback function used to draw selected strings.
 */
#define EM_SETDRAWSELECTFUNC    0xF0D0

/**
 * \def EM_SETGETCARETWIDTHFUNC
 * \brief Sets the callback function on getting caret width
 *
 * \code
 * EM_SETGETCARETWIDTHFUNC
 * int (*get_caret_width) (HWND, int);
 *
 * wParam = 0;
 * lParam = (LPARAM)get_caret_width;
 * \endcode
 *
 * \param get_caret_width The callback function used to get caret width.
 *                        The window handle and the maximum caret width are passed as arguments.
 *
 * \return The desired caret width.
 */
#define EM_SETGETCARETWIDTHFUNC 0xF0D1

/*
#define EM_SETWORDBREAKPROC     0xF0D0
#define EM_GETWORDBREAKPROC     0xF0D1
*/

/**
 * \def EM_GETPASSWORDCHAR
 * \brief Returns the character that edit controls uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_GETPASSWORDCHAR
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The currently used password character
 */
#define EM_GETPASSWORDCHAR      0xF0D2

/**
 * \def EM_SETLIMITTEXT
 * \sa EM_LIMITTEXT
 */
#define EM_SETLIMITTEXT         EM_LIMITTEXT

/**
 * \def ED_CARETSHAPE_LINE
 * \brief Line-shaped caret
 */
#define ED_CARETSHAPE_LINE      0

/**
 * \def ED_CARETSHAPE_BLOCK
 * \brief Block-shaped caret
 */
#define ED_CARETSHAPE_BLOCK     1

/**
 * \def EM_CHANGECARETSHAPE
 * \brief Changes the shape of the caret
 *
 * \code
 * EM_CHANGECARETSHAPE
 *
 * int caret_shape;
 * 
 * wParam = (WPARAM)caret_shape;
 * lParam = 0;
 * \endcode
 *
 * \param caret_shape Shape index of the caret, can be ED_CARETSHAPE_LINE or ED_CARETSHAPE_BLOCK.
 *
 * \return The old create shape
 */
#define EM_CHANGECARETSHAPE     0xF0D3

/**
 * \def EM_REFRESHCARET
 * \brief Refresh caret of the edit control
 */
#define EM_REFRESHCARET         0xF0D4

/**
 * \def EM_ENABLECARET
 * \brief To enable or disable the input caret
 *
 * \code
 * EM_ENABLECARET
 *
 * BOOL bEnable;
 *
 * wParam = (WPARAM)bEnable;
 * lParam = 0;
 * \endcode
 *
 * \param bEnable TRUE to enable caret.
 *
 * \return The previous caret enabled status.
 */
#define EM_ENABLECARET          0xF0D5

/**
 * \def EM_GETLIMITTEXT
 * \brief Get text limit value of the edit control. 
 *
 * \return -1 when user doesn't set limit value, otherwise return current 
 * limit value. 
 */
#define EM_GETLIMITTEXT         0xF0D6

/*
 * \def EM_SETMARGINS
 * \brief Set the margins of edit control
 * 
 * \code
 * EM_SETMARGINS
 *
 * const RECT *prcMargins
 *
 * lParam = (LPARAM)prcMargins
 * \endcode
 * 
 * \param prcMargins : the margins info:
 *    - top : the top margin value
 *    - left: the left margin value
 *    - right: the right margin value
 *    - bottom: the bottom margin value
 *
 * \return ignored
 *
 * \sa EM_GETMARGINS
 */
#define EM_SETMARGINS           0xF0D7

/*
 * \def EM_GETMARGINS
 * \brief Get the margins of content
 *
 * \code
 * EM_GETMARGINS
 *
 * RECT *prcMargins
 *
 * lParam = (LPARAM)prcMargins
 * \endcode
 * 
 * \param prcMargins : the margins info:
 *    - top : the top margin value
 *    - left: the left margin value
 *    - right: the right margin value
 *    - bottom: the bottom margin value
 *
 * \return TRUE / FALSE
 *
 * \sa EM_SETMARGINS
 */
#define EM_GETMARGINS           0xF0D8
/*
#define EM_POSFROMCHAR          0xF0D9
#define EM_CHARFROMPOS          0xF0DA
#define EM_SETIMESTATUS         0xF0DB
#define EM_GETIMESTATUS         0xF0DC

#define MEM_SCROLLCHANGE        0xF0DD
*/

/* Not use */
#define MED_STATE_YES           0x0
/* Not use */
#define MED_STATE_NOUP          0x1
/* Not use */
#define MED_STATE_NODN          0x2
/* Not use */
#define MED_STATE_NO            0x3

/**
 * \def EM_SETTITLETEXT
 * \brief Sets the title text displayed before content text. 
 *
 * \code
 * EM_SETTITLETEXT
 * const char *title;
 * int len;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)title;
 * \endcode
 *
 * \note Implemented for TextEdit control.
 */
#define EM_SETTITLETEXT         0xF0DC

/**
 * \def EM_GETTITLETEXT
 * \brief Gets the title text displayed before content text. 
 *
 * \code
 * EM_GETTITLETEXT
 * const char *buffer;
 * int len;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len Should be length of buffer minus 1, left space for '\\0'
 * \param buffer String buffer
 *
 *
 * \return Length of title
 * \note Implemented for TextEdit control.
 */
#define EM_GETTITLETEXT         0xF0DD

/**
 * \def EM_SETTIPTEXT
 * \brief Sets the tip text displayed when content is empty. 
 *
 * \code
 * EM_SETTIPTEXT
 * const char *buffer;
 * int len;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 */
#define EM_SETTIPTEXT           0xF0DE

/**
 * \def EM_GETTIPTEXT
 * \brief Gets the tip text displayed when content is empty. 
 *
 * \code
 * EM_GETTIPTEXT
 * const char *buffer;
 * int len;
 *
 * wParam = (WPARAM)len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len Should be length of buffer minus 1, left space for '\\0'
 * \param buffer String buffer
 *
 * \return Length of tip text
 */
#define EM_GETTIPTEXT           0xF0DF

/**
 * Structure defines text position information.
 */
typedef struct _TEXTPOSINFO {
    /** The index of paragraph, if value is -1, 
     *it will take effect on the whole text.*/
    int paragraph_index;
    /** The index of line, if value is -1, 
     *it will take effect on the whole text.*/
    int line_index;
     /** The beginning byte position can be copied to the buffer.*/
    int start_pos;
     /** The maximal number of bytes can be copied to the buffer.
      * It includes terminate character 0x0.*/
    int copy_len;
    /** The pointer to a buffer receives the text. 
     *Please make sure buffer size is more than the value of copy_len.*/
    char *buff;
}TEXTPOSINFO;

/**
 * \def EM_GETNUMOFPARAGRAPHS
 * \brief Gets the number of paragraphs in textedit control. 
 *
 * \return The number of paragraphs.
 */
#define EM_GETNUMOFPARAGRAPHS   0xF0E0
/**
 * \def EM_GETPARAGRAPHLENGTH
 * \brief Gets the specified paragraph length in textedit control. 
 *
 * \code
 * EM_GETPARAGRAPHLENGTH
 * wParam = (WPARAM) index;
 * \endcode
 *
 * \return The length of text.
 */
#define EM_GETPARAGRAPHLENGTH  0xF0E1
/**
 * \def EM_GETPARAGRAPHTEXT
 * \brief Gets the specified paragraph text from textedit control. 
 *
 * \code
 * EM_GETPARAGRAPHTEXT
 * const char buffer[BUF_SIZE];
 * TEXTPOSINFO info;
 *
 * info.start_pos = 0;
 * info.copy_len = BUF_SIZE;
 * info.buff = buffer;
 * info.paragraph_index = -1;
 *
 * wParam = (WPARAM)&info;
 * \endcode
 *
 * \return The copied length of text which doesn't include terminate 
 * character 0x0.
 */
#define EM_GETPARAGRAPHTEXT    0xF0E2

/*
 * \def EM_MUSTUPDATEBKGND
 * \brief Test if the bkgournd must be update
 *
 * \return BOOL. TRUE -- must update the bkground, texteditor would not use ScrollWindow
 *  FALSE -- bkground does not need to update, textditor use Scrollwindow
 *
 *  if you want paint a image on bkground, please make this message return TRUE
 *
 */
#define EM_MUSTUPDATEBKGND     0xF0E3

/**
 * \def EM_SETCARETPOSBYLINE
 * \brief Sets the selected point in the edit control and makes
 *        the text between insertion point and selection point selected.
 *        This is set by really line. That means if you set the TES_AUTOWRAP,
 *        the line number you set, the caret pos line number you see. This is
 *        different from EM_SETCARETPOS.
 *
 *
 * \code
 * EM_SETCARETPOSBYLINE
 *
 * int line_pos;
 * int char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Line position of the selection point.
 *                 For single line editor, it is always zero.
 *                 Note : For multi-line editor, "line" means a text string ended with a line
 *                 seperator, in wrap mode there also is the really line number. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Character(wide character) position of the selection point.
 *
 * \return Length of the selected string
 */
#define EM_SETCARETPOSBYLINE    0xF0D9

/**
 * \def EM_GETCARETPOSBYLINE
 * \brief Gets the position of the caret.
 *
 * \code
 * EM_GETCARETPOSBYLINE
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the caret line position.
 *                 For single line editor, it is always zero.
 * \param char_pos Pointer to a integer buffer to save the caret character position.
 *
 * \return The string length of the text from the beginning to the caret pos.
 */

#define EM_GETCARETPOSBYLINE    0xF0DA

/**
 * \def EM_SETSELPOSBYLINE
 * \sa EM_SETSELBYLINE
 */
#define EM_SETSELPOSBYLINE    0xF0DB

/**
 * \def EM_GETSELPOSBYLINE
 * \brief Gets the position of the selection point.
 *
 * \code
 * EM_GETSELPOSBYLINE
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the selection line position.
 *                 For single line editor, it is always zero.
 * \param char_pos Pointer to a integer buffer to save the selection character position.
 *
 * \return The string length of the text from the beginning to the selection point.
 */
#define EM_GETSELPOSBYLINE    0xF0E4

/**
 * \def EM_GETPARAGRAPHLENGTHINMCHAR
 * \brief Gets the specified paragraph length in textedit control.
 * in text not charactors. 
 *
 * \code
 * EM_GETPARAGRAPHLENGTHINCHAR
 * wParam = (WPARAM) index;
 * \endcode
 *
 * \return The length of text.
 */
#define EM_GETPARAGRAPHLENGTHINMCHAR  0xF0E5

/**
 * \def EM_GETLINELENGTHINMCHAR
 * \brief Gets the specified line length in textedit control.
 * in charactors not byte. 
 *
 * \code
 * EM_GETLINELENGTHINMCHAR
 * wParam = (WPARAM) index;
 * \endcode
 *
 * \return The length of text.
 */
#define EM_GETLINELENGTHINMCHAR  0xF0E6

/**
 * \def EM_GETLINELENGTH
 * \brief Gets the specified line length in textedit control.
 *
 * \code
 * EM_GETLINELENGTH
 * wParam = (WPARAM) index;
 * \endcode
 *
 * \return The length of text.
 */
#define EM_GETLINELENGTH  0xF0E7

/**
 * \def EM_GETLINETEXT
 * \brief Gets the specified line text from textedit control. 
 *
 * \code
 * EM_GETLINETEXT
 * const char buffer[BUF_SIZE];
 * TEXTPOSINFO info;
 *
 * info.start_pos = 0;
 * info.copy_len = BUF_SIZE;
 * info.buff = buffer;
 * info.line_index = -1;
 *
 * wParam = (WPARAM)&info;
 * \endcode
 *
 * \return The copied length of text which doesn't include terminate 
 * character 0x0.
 */
#define EM_GETLINETEXT    0xF0E8

#define EM_MSGMAX               0xF0EA

    /** @} end of ctrl_edit_msgs */

    /**
     * \defgroup ctrl_edit_ncs Notification codes of edit control
     * @{
     */

#define EN_ERRSPACE         255

/**
 * \def EN_CLICKED
 * \brief Notifies a click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user clicks
 * in an edit control.
 */
#define EN_CLICKED          0x0001

/**
 * \def EN_DBLCLK
 * \brief Notifies a double click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user 
 * double clicks in an edit control.
 */
#define EN_DBLCLK           0x0002

/**
 * \def EN_SETFOCUS
 * \brief Notifies the receipt of the input focus.
 *
 * The EN_SETFOCUS notification code is sent when an edit control receives 
 * the input focus.
 */
#define EN_SETFOCUS         0x0100

/**
 * \def EN_KILLFOCUS
 * \brief Notifies the lost of the input focus.
 *
 * The EN_KILLFOCUS notification code is sent when an edit control loses 
 * the input focus.
 */
#define EN_KILLFOCUS        0x0200

/**
 * \def EN_CHANGE
 * \brief Notifies that the text is altered by the user.
 *
 * An edit control sends the EN_CHANGE notification code when the user takes 
 * an action that may have altered text in an edit control.
 */
#define EN_CHANGE           0x0300

/**
 * \def EN_UPDATE
 * \brief Notifies that the text is altered by sending MSG_SETTEXT
 *        TEM_RESETCONTENT, or EM_SETLINEHEIGHT message to it.
 *
 * An edit control sends the EN_UPDATE notification code when the control
 * received MSG_SETTEXT, TEM_RESETCONTENT, or EM_SETLINEHEIGHT message.
 *
 * Note that this notification will occured when you call SetWindowText or
 * SetDlgItemText function on the control.
 */
#define EN_UPDATE           0x0400

/**
 * \def EN_MAXTEXT
 * \brief Notifies reach of maximum text limitation.
 *
 * The EN_MAXTEXT notification message is sent when the current text 
 * insertion has exceeded the specified number of characters for the edit control.
 */
#define EN_MAXTEXT          0x0501

/* Not use */
#define EN_HSCROLL          0x0601
/* Not use */
#define EN_VSCROLL          0x0602


/**
 * \def EN_SELCHANGED
 * \brief Notifies that the current selection is changed in the text field.
 */
#define EN_SELCHANGED       0x0603

/**
 * \def EN_CONTCHANGED
 * \brief Notifies that the current content is changed in the text field
 * when text edit losts focus.
 */
#define EN_CONTCHANGED     0x0604

/**
 * \def EN_ENTER
 * \brief Notifies the user has type the ENTER key in a single-line edit control.
 */
#define EN_ENTER            0x0700

    /** @} end of ctrl_edit_ncs */

/* Edit control EM_SETMARGIN parameters */
/**
 * \def EC_LEFTMARGIN
 * \brief Value of wParam. Specifies the left margins to set.
 */
#define EC_LEFTMARGIN       0x0001
/**
 * \def EC_RIGHTMARGIN
 * \brief Value of wParam. Specifies the right margins to set.
 */
#define EC_RIGHTMARGIN      0x0002
/**
 * \def EC_USEFONTINFO
 * \brief Value of wParam. Specifies the user font info to set.
 */
#define EC_USEFONTINFO      0xffff

/* wParam of EM_GET/SETIMESTATUS  */
/**
 * \def EMSIS_COMPOSITIONSTRING
 * \brief Indicates the type of status to retrieve.
 */
#define EMSIS_COMPOSITIONSTRING        0x0001

/* lParam for EMSIS_COMPOSITIONSTRING  */
/**
 * \def EIMES_GETCOMPSTRATONCE
 * \brief lParam for EMSIS_COMPOSITIONSTRING(reserved).
 */
#define EIMES_GETCOMPSTRATONCE         0x0001
/**
 * \def EIMES_CANCELCOMPSTRINFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING(reserved).
 */
#define EIMES_CANCELCOMPSTRINFOCUS     0x0002
/**
 * \def EIMES_COMPLETECOMPSTRKILLFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING(reserved).
 */
#define EIMES_COMPLETECOMPSTRKILLFOCUS 0x0004

    /** @} end of ctrl_edit */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_EDIT_H */

