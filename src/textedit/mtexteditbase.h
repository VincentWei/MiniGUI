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
** texteditbase.h: base header for the new text edit module.
**
** Create date: 2010/03/10
*/

#ifndef TEXTEDITBASE_H
#define TEXTEDITBASE_H

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE(ITextBufferObserver)
DECLARE_INTERFACE(ITextIterator)
DECLARE_INTERFACE(ITextBuffer)
DECLARE_INTERFACE(ITextRender)
DECLARE_INTERFACE(ITextLayoutContext)
DECLARE_INTERFACE(ITextLayout)

//define the interface ITextBufferObserver
#define ITextBufferObserverHeader(Interface,ClassImpl)  \
	IInterfaceHeader(Interface, ClassImpl)              \
	void (*onTextChanged)(ClassImpl*, int changedBegin, int delCount, int insertCount);

DEFINE_INTERFACE(ITextBufferObserver)


///////////////////////////////////////////////
// ITextBuffer
#define ITextBufferHeader(Interface, ClassImpl) \
	IInterfaceHeader(Interface, ClassImpl)      \
	/*replace */ \
	int (*replace)(ClassImpl *self,             \
			int at, int count,                  \
			const char* str,                    \
			int str_count/*=-1*/);              \
	int (*getText)(ClassImpl* self,             \
			int at /*=NULL*/,                   \
			char* str_buff,                     \
			int str_count);                     \
	int (*getCount)(ClassImpl* self);           \
	ITextIterator* (*getAt)(ClassImpl* self,    \
			int charIndex);                     \
	BOOL (*addObserver)(ClassImpl*, ITextBufferObserver* observer);     \
	BOOL (*removeObserver)(ClassImpl*, ITextBufferObserver* observer);  \
	int (*find)(ClassImpl*self, int start, const char* str, int str_count); \
	int (*setCase)(ClassImpl *self, int start, int len, BOOL bupper);

DEFINE_INTERFACE(ITextBuffer)

////////////////////////////////////
// ITextIterator
#define ITextIteratorHeader(Interface, ClassImpl)           \
	IInterfaceHeader(Interface, ClassImpl)                  \
	void (*nextChar)(ClassImpl* self);                      \
	void (*prevChar)(ClassImpl* self);                      \
	/*get current index of charactor*/                      \
	int  (*index)(ClassImpl* self);                         \
	/*reset the iterator*/                                  \
	int  (*reset)(ClassImpl* self, int charIndex);          \
	/*is end*/ \
	BOOL (*isEnd)(ClassImpl* self);                         \
	/*is head*/ \
	BOOL (*isHead)(ClassImpl* self);                        \
	/*get the charactor as much as more*/                   \
	const char* (*getMore)(ClassImpl*,int *pTextLen,        \
        int *pVisibleTextLen, BOOL bAutoSkip/*=TRUE*/);     \
	/*reference one*/  \
	ITextIterator* (*reference)(ClassImpl* self);           \
	/*comapre */ \
	int (*diff)(ClassImpl*, const ITextIterator* another);  \
	/*is in a range*/ \
	int (*inRange)(ClassImpl*, const ITextIterator* left,   \
            const ITextIterator* right);                    \
	/*release me*/ \
	void (*releaseIterator)(ClassImpl* self);                       \
	/*text Render*/ \
	ITextRender* (*getTextRender)(ClassImpl* self);         \
	/*getCharType*/ \
	int (*getCharType)(ClassImpl* self, int offset);

enum {
	TI_OUTOFRANGE    = -1,
	TI_VISIBILE_CHAR = 0,   /*visible char*/
	TI_LINERETURN,           /*line return*/
    TI_EOF                  /* End OF File */
};


DEFINE_INTERFACE(ITextIterator)

//define the return value of inRange
enum {
	TXTITER_RANGE_INVALID    = -1,
	TXTITER_RANGE_IN         = 0,   // left  < self  < right
	TXTITER_RANGE_LEFTEDGE   ,      // left  == self
	TXTITER_RANGE_RIGHTEDGE  ,      // right == self
	TXTITER_RANGE_LEFTOUT    ,
	TXTITER_RANGE_RIGHTOUT  
};


////////////////////////////////////////////////////////////////////////////////
// ITextRender
#define ITextRenderHeader(Interface, ClassImpl)         \
	IInterfaceHeader(Interface, ClassImpl)              \
	int  (*calc)(ClassImpl* self, HDC hdc,              \
            ITextIterator* begin,                       \
			int str_count, int* ptext_count,            \
			const RECT* maxBound, RECT *bounds);        \
	int (*draw)(ClassImpl* self, HDC hdc, int x, int y, \
            ITextIterator* begin, int str_count);       \
	int (*getTextByteLen)(ClassImpl* self, HDC hdc,     \
        ITextIterator* it, /*in out*/int *ptext_index); \
	int (*getTextCount)(ClassImpl* self, HDC hdc,       \
            ITextIterator* it, /*in out*/int* pstr_len);\
	int (*getCaretSize)(ClassImpl* self, HDC hdc,       \
			ITextIterator* it, int *pWidth, int *pHeight);

DEFINE_INTERFACE(ITextRender)


////////////////////////////////////////////////////////////////
//Interface for ITextLayout

#define ITextLayoutContextHeader(Interface, ClassImpl)  \
	IInterfaceHeader(Interface, ClassImpl)              \
	int (*getMetrics)(ClassImpl*,int type);             \
	void (*setupDC)(ClassImpl*, HDC hdc);               \
	void (*beginSelection)(ClassImpl*,HDC hdc);         \
	void (*endSelection)(ClassImpl*, HDC hdc);          \
	HDC  (*getDC)(ClassImpl*);                          \
    void (*contentPosToDCPos)(ClassImpl*, HDC hdc,      \
            int *px, int *py);                          \
	void (*releaseDC)(ClassImpl*, HDC hdc);             \
    void (*updateCaret)(ClassImpl*);                    \
	void (*update)(ClassImpl*, BOOL);

enum {
	TEXTMETRICS_MAXWIDTH,
	TEXTMETRICS_MAXHEIGHT,
	TEXTMETRICS_LINESPACE,
    TEXTMETRICS_DEFLINEHEIGHT
};

DEFINE_INTERFACE(ITextLayoutContext)
//////////////////////////////////////////////////////////////////
//ITextLayout
typedef void (*CB_LINE_PROC)(mObject* self, int x, int y, int width, int height, int lineNo, void* user_param);
typedef BOOL (*CB_NODE_PROC)(mObject* self, mObject* cur_node, void* user_param);

#define ITextLayoutHeader(Interface, ClassImpl)         \
	ITextBufferObserverHeader(Interface, ClassImpl)     \
	/*get the text iterator by pos*/ \
	int (*getCharIndex)(ClassImpl*, int *px, int *py,   \
        int *plineHeight/*=NULL*/, int *ptext_index);   \
	/*get the pos by text iterator */ \
	int (*getCharPos)(ClassImpl*, int index, int *px,   \
            int *py, int *plineHeight, BOOL bAsText);   \
	/*get the size of the text */ \
	BOOL (*getTextBoundSize)(ClassImpl* self, int *cx, int *cy);                \
	BOOL (*setCursor)(ClassImpl* self, int index, BOOL isSel);                  \
	int (*setCursorPos)(ClassImpl* self, int x, int y, BOOL isSel);             \
	int (*getCursor)(ClassImpl*, int *px, int *py, int *cursor_h, BOOL isSel);  \
	void (*draw)(ClassImpl* self, HDC hdc, int x, int y, int width, int height);\
	int (*getLineHeight)(ClassImpl* self, int index);                           \
    void (*reset)(ClassImpl *self);                                             \
	int (*textCount2Len)(ClassImpl*, int text_index, int text_count, int *plen);\
	int (*textLen2Count)(ClassImpl*, int str_index, int str_len, int*text_count,\
            int *poutstr_index, int *poutstr_count);                            \
    void (*foreachLine)(ClassImpl*, int x, int y, int maxWidth, int maxHeight,  \
            CB_LINE_PROC proc, mObject* proc_owner, void* user_param);          \
    void (*travelNode)(ClassImpl*, CB_NODE_PROC proc, mObject* proc_owner, void* user_param); \
    PLOGFONT (*getReleatedFont)(ClassImpl*, int idx, BOOL isText);              \
    int (*getParaByIndex)(ClassImpl* self, int index, int *pcol, BOOL bText);   \
    int (*getLineByIndex)(ClassImpl* self, int index, int* pcol, BOOL bText);   \
    int (*getIndexByPara)(ClassImpl* self, int paraNo, int col, BOOL* outOfRange, BOOL bText); \
    int (*getIndexByLine)(ClassImpl* self, int lineIdx, int col, BOOL* outOfRange, BOOL bText);

DEFINE_INTERFACE(ITextLayout)

#ifdef __cplusplus
}
#endif

#endif



