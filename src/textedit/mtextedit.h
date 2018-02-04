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
** textedit.h: header for new text edit module.
**
** Create date: 2010/03/10
*/

#ifndef _MGUI_TEXT_EDIT_H
#define _MGUI_TEXT_EDIT_H

#if defined(_MGNCS_TEXTEDITOR) || (defined(_MGCTRL_TEXTEDIT) && defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL))

#ifdef __cplusplus
extern "C" {
#endif

/* enable ES_TITLE stle of TEXTEDIT control */
#define _MGHAVE_TEXTEDITTITLE 1

DECLARE_OBJECT(mTextBuffer)
DECLARE_OBJECT(mTextIterator)
DECLARE_OBJECT(mTextLayout)
DECLARE_OBJECT(mTextLayoutNode)
DECLARE_OBJECT(mTextRender)

///////////////////////////////
// mTextBuffer
typedef struct _TextBufferObserverNode {
    ITextBufferObserver* observer;
    struct _TextBufferObserverNode* next;
}mTextBufferObserverNode;

#define mTextBufferHeader(Clss)  \
    mObjectHeader(Clss)     \
    INTERFACE(ITextBuffer) \
    char * buffer;  \
    int max_size; \
    int char_len; \
    mTextBufferObserverNode* observers;

#define mTextBufferClassHeader(Clss, Super) \
    mObjectClassHeader(Clss, Super)  \
    ITextBufferHeader(ITextBuffer, Clss)

//mTextBuffer_construct(const char* str = NULL, const char* str_len = 0)

DEFINE_OBJECT(mTextBuffer, mObject)

//////////////////////////////////////////
// mTextIterator
#define mTextIteratorHeader(Clss)  \
    mObjectHeader(Clss)           \
    INTERFACE(ITextIterator) \
    mTextBuffer* buffer;            \
    int index; 

#define mTextIteratorClassHeader(Clss, Super) \
    mObjectClassHeader(Clss, Super)          \
    ITextIteratorHeader(ITextIterator, Clss)

//mTextIterator_construct(mTextBuffer*, int index)

DEFINE_OBJECT(mTextIterator, mObject)

//////////////////////////////////////////////
// mTextLayout
#define mTextLayoutHeader(Clss)   \
    mCommBTreeHeader(Clss)        \
    INTERFACE(ITextLayout)       \
    ITextBuffer * text_buffer;   \
    int cursor;                  \
    int cursor_x, cursor_y;      \
    int cursor_height;           \
    int text_cursor;             \
    int sel_begin;               \
    int sel_begin_x, sel_begin_y;\
    int sel_begin_height;        \
    int text_sel_begin;          \
    ITextLayoutContext* context; \
    PCLIPRGN  dirty_area;        \
    int old_height, new_height; //height changed include

#define mTextLayoutClassHeader(Clss, Super) \
    mCommBTreeClassHeader(Clss, Super)      \
    ITextLayoutHeader(ITextLayout, Clss)

DEFINE_OBJECT(mTextLayout, mCommBTree)

//mTextLayoutNode

#define mTextLayoutNodeHeader(Clss)  \
    mCommBTreeNodeHeader(Clss)       \
    int str_count;         \
    int text_count;        \
    int width, height;

#define mTextLayoutNodeClassHeader(Clss, Super)  \
    mCommBTreeNodeClassHeader(Clss, Super)

DEFINE_OBJECT(mTextLayoutNode,mCommBTreeNode)

////////////////////////////////
//mTextRender

#define  mTextRenderHeader(Clss)    \
    mObjectHeader(Clss)            \
    INTERFACE(ITextRender)

#define mTextRenderClassHeader(Clss, Super)  \
    mObjectClassHeader(Clss, Super)         \
    ITextRenderHeader(ITextRender, Clss)

DEFINE_OBJECT(mTextRender,mObject)

enum {
    NCS_TXTRDR_TEXT = 0
};

ITextRender* GetTextRender(int type);

/* mTextEditor object*/
DECLARE_OBJECT(mTextEditor)

/**
 * \defgroup Control_TextEditor mTextEditor
 * @{
 */

#ifndef __MINIGUI_LIB__
/**
 * \def NCSCTRL_TEXTEDITOR
 * \brief the name of mTextEditor
 */
#define NCSCTRL_TEXTEDITOR NCSCLASSNAME("texteditor")
#endif

/* define the flag of TextEditor */
#define NCS_TEF_REPLACE_MODE   0x010000
#define NCS_TEF_HAS_PAINTED    0x020000
#define mTextEditor_isReplaceMode(self)  \
        ((self)->flags&NCS_TEF_REPLACE_MODE)
#define mTextEditor_hasPainted(self)  \
        ((self)->flags&NCS_TEF_HAS_PAINTED)


#ifdef _MGHAVE_TEXTEDITTITLE
#define TITLE_HEADER   \
        int title_idx, title_text_idx; \
        int title_x, title_y, title_last_height; \
        int title_paracount; \
        int title_last_para_text_index; \
        int title_linecount; \
        int title_last_line_text_index;

#define TITLE_CLASS_HEADER(Clss) \
        void (*setTitle)(Clss*, const char* buff, int len); \
        int  (*getTitle)(Clss*, char* buff, int max);


#else
#define TITLE_HEADER
#define TITLE_CLASS_HEADER(Clss)
#endif

//mTextEditor_construct(HWND hwnd)
/**
 * \struct mTextEditor
 *
 * \brief The structure of mTextEditor, which derived from mWidget.
 *        It is the new multiline edit control.
 *
 *  - textBuffer\n
 *    The text buffer. 
 *
 *  - textLayout\n
 *    The text layout.
 *
 *  - visWidth, visHeight\n
 *    The text visible width and height. 
 *
 *  - offX, offY\n
 *    The text offset value.
 *
 *  - caretWidth, caretHeight\n
 *    The width and height of caret.
 *
 *  - scrollHStep, scrollVStep\n
 *    The horizontal and vertical scroll step of scrollbar.
 *
 *  - scrollBarMode\n
 *    The scroll mode of scrollbar.
 *
 */
#define mTextEditorHeader(Clss)      \
    mWidgetHeader(Clss)             \
    INTERFACE(ITextLayoutContext)   \
    mTextBuffer  *textBuffer;        \
    mTextLayout  *textLayout;        \
    int         visWidth, visHeight;\
    int         offX, offY;         \
    int         hardLimit;          \
    unsigned int flags;             \
    int         caretWidth;         \
    int         caretHeight;        \
    unsigned short scrollBarMode;   \
    unsigned int scrollHStep;       \
    unsigned int scrollVStep;       \
    RECT         margins;           \
    int  oldOffX, oldOffY;          \
    TITLE_HEADER


/** 
 * \struct mTextEditorClass
 * \brief the VTable of \a mTextEditor, derived from \ref mWidget.
 * 
 * - int (*\b getParaText)(mTextEditor*, int index, int start, int len, char* buffer);\n
 *   The function gets the specified paragraph text.
 *      \param index - The index of paragraph.
 *      \param start - The start byte index in specified paragraph.
 *      \param len   - The copied byte length of content.
 *      \param buffer- The string buffer.
 *      \return copied string length.
 *
 * - int (*\b getParaLength)(mTextEditor*, int index, int *startPos);      \n
 *   The function gets the specified paragraph length, it includes unvisible character.
 *      \param index - The index of paragraph.
 *      \param startPos(Out) - The start byte index of paragraph.
 *      \return length of specified paragraph on success, otherwise return -1.
 *
 * - int (*\b getParaNum)(mTextEditor*);                                   \n
 *   The function gets the total number of paragraphs.
 *
 * - int (*\b setText)(mTextEditor*, const char* text);\n
 *   The function sets new text content.
 *      \param text - the new text.
 *      \return length has been setted, otherwise return -1.
 *
 * - int (*\b insertText)(mTextEditor*, const char* text, int len);        \n
 *   The function inserts the specified text to current caret position.
 *
 * - int (*\b getText)(mTextEditor*, char* text, int len);                 \n
 *   The function gets the specifed length of content to specified buffer.
 *
 * - int (*\b getTextLen)(mTextEditor*);                                   \n
 *   The function gets the content length.
 *
 * - BOOL (*\b enableCaret)(mTextEditor*, BOOL enable);                    \n
 *   The function enables or disables caret.
 *
 * - int (*\b setCaretPosByPara)(mTextEditor*, int paraIdx, int offChar,
 *          BOOL isSel);                                        \n
 *   The function sets caret position by paragraph information.
 *      \param paraIdx - The index of paragraph.
 *      \param offChar - The offset of character.
 *      \param isSel - Whether is selected or not.
 *      \return the byte index of specified caret on success, otherwise return -1.
 *
 * - int (*\b getCaretParaPos)(mTextEditor*, int *paraIdx, int *offChar,
 *          int *height, BOOL isSel);                           \n
 *   The function gets the current caret paragraph information.
 *      \param paraIdx(Out) - The index of paragraph.
 *      \param offChar(Out) - The offset of character.
 *      \param height(Out) - The height of caret.
 *      \param isSel(In) - Whether is selected or not.
 *      \return the character index in current caret position on success, otherwise return -1.
 *
 * - int (*\b setCaretPosByLine)(mTextEditor*, int lineIdx, int offChar,
 *          BOOL isSel);                                        \n
 *   The function sets caret position according line information.
 *      \param lineIdx - The index of line.
 *      \param offChar - The offset of character.
 *      \param isSel - Whether is selected or not.
 *      \return the character index in current caret position on success, otherwise return -1.
 *
 * - int (*\b getLineCount)(mTextEditor*);                                 \n
 *   The function gets the number of text lines.
 *
 * - void (*\b selectAll)(mTextEditor*);                                   \n
 *   The function selects all content.
 *
 * - int (*\b getSel)(mTextEditor*, char *buffer, int len); \n
 *   The function gets selected string to user-defined text buffer.
 *      \param buffer - The pointer to the string buffer.
 *      \param len - The length of buffer.
 *      \return length has been copied to string buffer on success, otherwise return -1.
 *
 * - int (*\b setSel)(mTextEditor*, unsigned int startParaIdx, unsigned int startOffChar,    \
                unsigned int endParaIdx, unsigned int endOffChar);  \n
 *   The function sets selected region according to specified position information.
 *      \param startParaIdx The start index of paragraph.
 *      \param startOffChar The start offset of character.
 *      \param endParaIdx The end index of paragraph.
 *      \param endOffChar The end offset of character.
 *      \return zero on success, otherwise return -1.
 *
 */ 
#define mTextEditorClassHeader(Clss, Super)  \
        mWidgetClassHeader(Clss, Super)     \
        ITextLayoutContextHeader(ITextLayoutContext, Clss)          \
        int (*onChar)(Clss*, int asciiCode, DWORD keyFlags);        \
        BOOL (*onFontChanged)(Clss*);                               \
        int (*getInvalidBkgnd)(Clss*, RECT *prc);                   \
        int (*onLButtonDBClk)(Clss*, int x, int y, DWORD keyFlags); \
        void (*onCSizeChanged)(Clss*, int cliWidth, int cliHeight); \
        void (*onSetFocus)(Clss*, HWND oldActiveWnd, int lParam);   \
        void (*onKillFocus)(Clss*, HWND newActiveWnd, int lParam);  \
        void (*onHScroll)(Clss*, int code, int mouseX);             \
        void (*onVScroll)(Clss*, int code, int mouseY);             \
        \
        int (*getParaText)(Clss*, int index, int start, int len, char* buffer);       \
        int (*getLineText)(Clss*, int index, int start, int len, char* buffer);       \
        int (*getParaLength)(Clss*, int index, int *startPos);      \
        int (*getParaLengthInMChar)(Clss*, int paraNo, int *startPos);                \
        int (*getLineLength)(Clss*, int lineNo, int *startPos, BOOL bMChar);          \
        int (*getParaNum)(Clss*);                                   \
        int (*setText)(Clss*, const char* text);                    \
        int (*insertText)(Clss*, const char* text, int len);        \
        int (*getText)(Clss*, char* text, int len);                 \
        int (*getTextLen)(Clss*);                                   \
        BOOL (*enableCaret)(Clss*, BOOL enable);                    \
        int (*setCaretPosByPara)(Clss*, unsigned int paraIdx, unsigned int offChar,   \
                BOOL isSel);                                        \
        int (*getCaretPosByPara)(Clss*, int *paraIdx, int *offChar, \
                int *height, BOOL isSel);                           \
        int (*setCaretPosByLine)(Clss*, unsigned int lineIdx, unsigned int offChar,   \
                BOOL isSel);                                        \
        int (*getCaretPosByLine)(Clss*, int *lineIdx, int *offChar, \
                int *height, BOOL isSel);                           \
        int (*getLineCount)(Clss*);                                 \
        void (*selectAll)(Clss*);                                   \
        int (*setSel)(Clss*, unsigned int startParaIdx, unsigned int startOffChar,    \
                unsigned int endParaIdx, unsigned int endOffChar);  \
        int (*getSel)(Clss*, char *buffer, int len);                \
        BOOL (*needLineDecorative)(Clss*);                          \
        void (*lineDecorative)(Clss*, int x, int y, int w, int h,   \
                int lineNo, HDC hdc);                               \
        TITLE_CLASS_HEADER(Clss)

#ifndef __MINIGUI_LIB__

/**
 * \def NCSS_TE_READONLY
 * \brief The object is read-only.
 */
#define NCSS_TE_READONLY  (1<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_UPPERCASE
 * \brief The text is upper case.
 */
#define NCSS_TE_UPPERCASE (2<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_LOWERCASE
 * \brief The text is lower case.
 */
#define NCSS_TE_LOWERCASE (4<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_NOHIDESEL
 * \brief When lost the focus, it is still selected.
 */
#define NCSS_TE_NOHIDESEL (8<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_AUTOWRAP
 * \brief Wrap text automatically.
 */
#define NCSS_TE_AUTOWRAP  (0x10<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_BASELINE
 * \brief Draw the base line.
 */
#define NCSS_TE_BASELINE  (0x20<<NCSS_WIDGET_SHIFT)

/**
 * \def NCSS_TE_OWNERBKGND
 * \brief bkground is ower draw
 */
#define NCSS_TE_OWNERBKGND (0x40<<NCSS_WIDGET_SHIFT)

/*
 * \def NCSS_TE_TITLE
 * \brief make the texteditor has title
 */
#define NCSS_TE_TITLE  (0x80<<NCSS_WIDGET_SHIFT)

/*
 * \def NCSS_TE_AUTOVSCROLL
 * \brief make the texteditor has a auto vert scrollbar
 *
 * \note: NCSS_TE_AUTOVSCROLL | WS_VSCROLL == AUTO
 * !(dwStyle & NCSS_TE_AUTOVSCROLL) && (dwStyle & WS_VSCROLL)  == Always
 * !(dwStyle & (NCSS_TE_AUTOVSCROLL | WS_VSCROLL)) == Nerver
 */
#define NCSS_TE_AUTOVSCROLL (0x100<<NCSS_WIDGET_SHIFT)

/*
 * \def NCSS_TE_AUTOHSCROLL
 * \brief make the texteditor has the auto horz scrollbar
 *
 * \note: NCSS_TE_AUTOHSCROLL | WS_HSCROLL == AUTO
 * !(dwStyle & NCSS_TE_AUTOHSCROLL) && (dwStyle & WS_HSCROLL)  == Always
 * !(dwStyle & (NCSS_TE_AUTOHSCROLL | WS_HSCROLL)) == Nerver
 */
#define NCSS_TE_AUTOHSCROLL (0x200<<NCSS_WIDGET_SHIFT)

#define NCSS_TE_SHIFT  (NCSS_WIDGET_SHIFT + 10)

#endif

/**
 * \enum ncsTEScrollMode
 * \brief Scroll bar drawing mode in TextEditor.
 */
 enum ncsTEScrollMode
 {
    /** 
     * The scroll bar is never visbile.
     */
    NCS_TEF_SCROLLBAR_NEVER = 0,

    /**
     * The scroll bar is shown or hidded automatically.
     */
    NCS_TEF_SCROLLBAR_AUTO,

    /**
     * The scroll bar is always visible.
     */
    NCS_TEF_SCROLLBAR_ALWAYS,
};

/**
 * \enum mTextEditorProp
 * \brief The properties id of mTextEditor.
 */
enum mTextEditorProp 
{
    /**
     * Read-Only content.
     */
    NCSP_TE_READONLY = NCSP_WIDGET_MAX + 1,

    /**
     * The scrollbar drawing mode.
     */
    NCSP_TE_SCROLL_MODE,

    /**
     * The horizontal step value.
     */
    NCSP_TE_SCROLL_HSTEP,

    /**
     * The vertical step value.
     */
    NCSP_TE_SCROLL_VSTEP,

    /**
     * Maximum text byte length.
     */
    NCSP_TE_TEXTLIMIT,

    /**
     * Total line count. RO.
     */
    NCSP_TE_LINECOUNT,

    /**
     * The number of paragraphs. RO.
     */
    NCSP_TE_PARAGRAPHCOUNT,

    /**
     * The rectangle information of margins. param RECT*.
     */
    NCSP_TE_MARGINRECT,

    /**
     * The maximum value of mTextEditor properties id.
     */
    NCSP_TE_MAX,
};
#define NCSP_TE_NUMOFPARAGRAPHS NCSP_TE_PARAGRAPHCOUNT

#ifndef __MINIGUI_LIB__
/**
 * \enum mTextEditorNotify
 * \brief The notification code id of mTextEditor.
 */
enum mTextEditorNotify 
{
    /**
     * Notifies that content has changed when it lost focus.
     */
    NCSN_TE_CONTENTCHANGED = NCSN_WIDGET_MAX + 1,

    /**
     * Notifies the receipt of the input focus.
     */
    NCSN_TE_SETFOCUS,

    /**
     * Notifies the lost of the input focus.
     */
    NCSN_TE_KILLFOCUS,

    /**
     * Notifies reach of maximum content byte length.
     */
    NCSN_TE_MAXTEXT,

    /**
     * Notifies that the text is altered by the user.
     */
    NCSN_TE_CHANGE,

    /**
     * Notifies that the text is altered by sending setText to it.
     *
     */
    NCSN_TE_UPDATE,

    /**
     * Notifies that the selected text has been changed.
     */
    NCSN_TE_SELCHANGED,
   
    /**
     * The maximum value of mTextEditor notification code id.
     */
    NCSN_TE_MAX
};

typedef struct _mTextEditorRenderer mTextEditorRenderer;
#endif


DEFINE_OBJECT(mTextEditor, mWidget)

#ifdef __MINIGUI_LIB__
void TextEditorDrawLinesDecorative(mTextEditor * self, HDC hdc, int x, int y, int w, int h);
#define mTextEditor_drawLinesDecorative TextEditorDrawLinesDecorative
#else
void mTextEditor_drawLinesDecorative(mTextEditor * self, HDC hdc, int x, int y, int w, int h);
#endif

    /** @} end of Control_TextEditor */
#ifdef __cplusplus
}
#endif

#endif /*_MGNCS_TEXTEDITOR*/

#endif


