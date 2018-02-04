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
** textedit.c: the new textedit module.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


#ifdef __MINIGUI_LIB__
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "object.h"
#elif defined (__MGNCS_LIB__) 
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/ctrl/edit.h>
#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#endif

#if defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(_MGNCS_TEXTEDITOR)

#include "mwidget.h"
#include "mbtree.h"
#include "mtexteditbase.h"
#include "mtextedit.h"
#ifdef __MINIGUI_LIB__
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "ctrl/textedit.h"

#define NCSS_TE_READONLY  ES_READONLY
#define NCSS_TE_UPPERCASE ES_UPPERCASE
#define NCSS_TE_LOWERCASE ES_LOWERCASE
#define NCSS_TE_NOHIDESEL ES_NOHIDESEL
#define NCSS_TE_AUTOWRAP  ES_AUTOWRAP
#define NCSS_TE_BASELINE  ES_BASELINE
#define NCSS_TE_TITLE     ES_TITLE
#define NCSS_TE_AUTOVSCROLL ES_AUTOVSCROLL
#define NCSS_TE_AUTOHSCROLL ES_AUTOHSCROLL

#define NCSS_EX_UNNEEDIME  0x1

#define NCSN_TE_CONTENTCHANGED  EN_CONTCHANGED
#define NCSN_TE_SETFOCUS        EN_SETFOCUS
#define NCSN_TE_KILLFOCUS       EN_KILLFOCUS
#define NCSN_TE_MAXTEXT         EN_MAXTEXT
#define NCSN_TE_CHANGE          EN_CHANGE
#define NCSN_TE_UPDATE          EN_UPDATE
#define NCSN_TE_SELCHANGED      EN_SELCHANGED
#define NCSN_WIDGET_DBCLICKED   EN_DBLCLK
#define NCSN_WIDGET_CLICKED     EN_CLICKED

#else /* __MINIGUI_LIB__ */
#   include "mdblist.h"
#   include "mitem.h"
#   include "mscroll_widget.h"
#   include "mitem_manager.h"
#   include "mitemview.h"
#   include "mscrollview.h"
#   include "medit.h"
#endif


#define SMART_ASSIGN(lvalue, rvalue)    \
    do {                                \
        if(lvalue != NULL)              \
            *lvalue = rvalue;           \
    } while(0)                          \

static int get_first_char_len(PLOGFONT plogfont, const char* str, int len)
{
    int l;
    if(len <= 0)
        return 0;
    /*[humingming for bug 5344]: don't process like this,
     * on some borad may case some problem */
#if 0
    if(*str > 0) //is ascii code
    {
        return 1;
    }
    //it's a multi-byte character, the return value must > 1
    l = GetFirstMCharLen(plogfont, str, len);
    if(l <= 1)
        return 0;
#endif
    l = GetFirstMCharLen(plogfont, str, len);
    if (l < 0)
        return 1;
    return l;
}


// Enable this marco if support margin decorative
//#define MARGIN_UPDATE 1

static mTextRender g_textRender;
#define MAX_BLOCK 80

//mTextBuffer
static inline char* alloc_str_buffer(char* old_buff, int *plen)
{
    if(*plen <= 0)
        return old_buff;

    *plen = ((*plen + MAX_BLOCK - 1) / MAX_BLOCK ) * MAX_BLOCK;

    if(old_buff)
        return (char*)realloc(old_buff, *plen);
    return (char*)calloc(1, *plen);
}

static inline void free_str_buffer(char* text_buffer)
{
    if(text_buffer)
        free(text_buffer);
}

static inline void set_textbuffer(mTextBuffer* self, const char* title, int len)
{
    if(len <= 0 || !title)
        return;

    if(self->max_size < len){
        self->max_size = len;
        self->buffer = alloc_str_buffer(self->buffer, &self->max_size);
    }

    strncpy(self->buffer, title, len);
    self->buffer[len] = 0;
    self->char_len = len + 1;
}

static void update_observers(mTextBuffer* self, 
        int begin, int delcount, int insertcount)
{
    mTextBufferObserverNode* node = self->observers;
    while(node)
    {
        if(node->observer)
            (node->observer)->_vtable->onTextChanged(INTEFACE_ADJUST(node->observer), begin, delcount, insertcount);
        node = node->next;
    }
}

static void mTextBuffer_construct(mTextBuffer* self, va_list va)
{
    const char* title = NULL;
    int len = 0;

    Class(mObject).construct((mObject*)self, 0);
    ncsParseConstructParams(va, "si", &title, &len);

    if(title)
    {
        if(len <= 0)
            len = strlen(title);
        set_textbuffer(self, title, len);
    }
}

static void mTextBuffer_destroy(mTextBuffer* self)
{
    mTextBufferObserverNode * node;
    free_str_buffer(self->buffer);

    node = self->observers;
    while(node)
    {
        mTextBufferObserverNode* tmp = node;
        node = node->next;
        free(tmp);
    }

    Class(mObject).destroy((mObject*)self);
}

static int mTextBuffer_replace(mTextBuffer* self, int at, 
        int count, const char* str, int str_count)
{
    int i, j;
    if(at < 0 || at > self->char_len)
        at = self->char_len;
    if(str_count <= 0 && str)
        str_count = strlen(str);
    
    if(str_count < 0)
        str_count = 0;

    if(count < 0)
        count = 0;

    if(at + count > self->char_len)
        count = self->char_len - at;

    //prepare the new size
    if(count < str_count)
    {
        //alloc size and reset value
        if(self->max_size <= (self->char_len + str_count - count))
        {
            self->max_size = self->char_len + str_count - count;
            self->buffer = alloc_str_buffer(self->buffer, &self->max_size);
        }
    }
    
    if(self->char_len>(at+count) && str_count != count)
    {
        memmove(self->buffer + at + str_count, self->buffer + at + count, self->char_len - at - count);
    }

    self->char_len += (str_count - count);
    if((self->max_size - self->char_len) > (MAX_BLOCK*2))
    {
        //reduce the memory
        self->max_size = self->char_len + MAX_BLOCK*2;
        self->buffer = alloc_str_buffer(self->buffer, &self->max_size);
    }

    if(str)
    {
        i = at;
        j = 0;
        while(j < str_count)
        {
            if(str[j] == '\r')
                self->buffer[i] = '\n';
            else
                self->buffer[i] = str[j];
            i ++;
            j ++;
        }
    }
    
    //update the observer
    update_observers(self, at, count, str_count);
    
    return str_count;
}

static int mTextBuffer_getText(mTextBuffer* self, 
        int at, char* str_buffer, int str_count)
{
    int len;

    if(!str_buffer || str_count <= 0)
        return 0;

    str_buffer[0] = 0;

    if(at < 0 || at >= self->char_len)
        return 0;

    len = self->char_len - at;
    if(len > str_count)
        len = str_count;

    strncpy(str_buffer, self->buffer + at, len);
    str_buffer[len] = 0;
    return len;

}

static ITextIterator* mTextBuffer_getAt(mTextBuffer* self, int charIndex)
{
    mTextIterator* it = NEWV(mTextIterator, 2, self, charIndex);
    if(!it)
        return NULL;
    return INTERFACE_CAST(ITextIterator, it);
}

static BOOL mTextBuffer_addObserver(mTextBuffer* self, 
        ITextBufferObserver* observer)
{
    mTextBufferObserverNode * node;
    if(!observer)
        return FALSE;

    node = self->observers;
    while(node && node->observer != observer)
        node = node->next;

    if(node)
        return TRUE;

    node = 
        (mTextBufferObserverNode*)calloc(1, sizeof(mTextBufferObserverNode));
    node->next = self->observers;
    node->observer = observer;
    
    self->observers = node;

    return TRUE;
}

static BOOL mTextBuffer_removeObserver(mTextBuffer* self, 
        ITextBufferObserver* observer)
{
    mTextBufferObserverNode * node, *prev;

    if(!self->observers)
        return FALSE;
    
    node = self->observers;
    prev = NULL;
    while(node)
    {
        if(node->observer == observer)
            break;
        prev = node;
        node = node->next;
    }
    
    if(prev == NULL) {
        self->observers = node->next;
    }
    else {
        prev->next = node->next;
    }

    free(node);
    return TRUE;
}

#if 0
static int mTextBuffer_getIndexByPara(mTextBuffer* self, int line, int col)
{
    int i ;
    if(col < 0)
        col = 0;

    if(!self->buffer)
        return 0;

    i = 0;
    while(line > 0 && i < self->char_len)
    {
        if(self->buffer[i] == '\n')
            -- line;
        i ++;
    }

    if(i >= self->char_len)
        return self->char_len;

    return i + col;
}

static int mTextBuffer_getParaByIndex(mTextBuffer* self, int index, int *pcol)
{
    int i;
    int line = 0;
    int col = 0;
    int line_at = 0;

    i = 0;
    if(index > self->char_len)
        index = self->char_len;

    while(i < index)
    {
        if(self->buffer[i] == '\n')
        {
            line ++;
            line_at = i + 1;
        }
        i ++;
    }

    col = i - line_at;
    if(pcol)
        *pcol = col;
    return line;
}
#endif

static int mTextBuffer_getCount(mTextBuffer * self)
{
    return self->char_len;
}

static int findnstr(const char* text, int text_len, 
        const char* str, int str_count)
{
    int i = 0;
    while((i + str_count) < text_len)
    {
        int j;
        if(text[i] != str[0])
        {
            i ++;
            continue;
        }
        j = 1;
        while(j < str_count && text[i+j] == str[j] )
            j ++;
        if(j == str_count)
            return i;

        i += j;

    }
    return -1;
}

static int mTextBuffer_find(mTextBuffer* self, int start, 
        const char* str, int str_count)
{
    int pos = -1;
    if(str == NULL || str_count == 0)
        return -1;

    if(start < 0)
        start = 0;

    if(start >= self->char_len)
        return -1;

    if((INT_PTR)str == TI_LINERETURN || (INT_PTR)str == TI_EOF)
    {
        pos = findnstr(self->buffer + start, self->char_len - start, "\n", 1);
    }
    else
    {
        if(str_count < 0)
            str_count = strlen(str);
        pos = findnstr(self->buffer + start, self->char_len - start, str, str_count);
    }

    if(pos == -1)
        return -1;
    return start + pos;
}

static int mTextBuffer_setCase(mTextBuffer* self, int start, int len, BOOL bupper)
{
    char *str, *strend;
    char (*tocase)(char);
    if(!self || !self->buffer)
        return 0;

    if(start < 0)
        start = 0;

    if(len < 0)
        len = self->char_len - start;
    if(len <= 0)
        return 0;

    if(bupper)
        tocase = (char(*)(char))toupper;
    else
        tocase = (char(*)(char))tolower;

    str = self->buffer + start;
    strend = str + len;
    while(str < strend)
    {
        str[0] = tocase(str[0]);
        str ++;
    }

    return len;
}

BEGIN_MINI_CLASS(mTextBuffer, mObject)
    IMPLEMENT(mTextBuffer, ITextBuffer)
    CLASS_METHOD_MAP(mTextBuffer, construct)
    CLASS_METHOD_MAP(mTextBuffer, destroy)
    CLASS_METHOD_MAP(mTextBuffer, replace)
    CLASS_METHOD_MAP(mTextBuffer, getText)
    CLASS_METHOD_MAP(mTextBuffer, getCount)
    CLASS_METHOD_MAP(mTextBuffer, getAt)
    CLASS_METHOD_MAP(mTextBuffer, addObserver)
    CLASS_METHOD_MAP(mTextBuffer, removeObserver)
    CLASS_METHOD_MAP(mTextBuffer, find)
    CLASS_METHOD_MAP(mTextBuffer, setCase)
END_MINI_CLASS

/////////////////////////////////////////
//mTextIterator
static void mTextIterator_construct(mTextIterator* self, va_list va)
{
    Class(mObject).construct((mObject*)self, 0);
    ncsParseConstructParams(va, "pi", &self->buffer, &self->index); 
}

static void mTextIterator_nextChar(mTextIterator* self)
{
    if(self->buffer && self->index < self->buffer->char_len)
    {
        self->index ++;
    }
}

static void mTextIterator_prevChar(mTextIterator* self)
{
    if(self->buffer && self->index > 0)
    {
        self->index --;
    }
}

static int mTextIterator_index(mTextIterator* self)
{
    return self->index;
}

static int mTextIterator_reset(mTextIterator* self, int charIndex)
{
    if(self->buffer)
    {
        if(charIndex >= 0 && charIndex <= self->buffer->char_len)
        {
            self->index = charIndex;
        }
    }
    return self->index;
}

static BOOL mTextIterator_isEnd(mTextIterator*self)
{
    return (!self->buffer || self->index >= self->buffer->char_len );
}

static BOOL mTextIterator_isHead(mTextIterator* self)
{
    return (!self->buffer || self->index <= 0);
}

static const char* mTextIterator_getMore(mTextIterator* self, 
        int *pTextLen, int *pVisibleTextLen, BOOL bAutoSkip)
{
    const char* str;
    int len;
    int len_limit = -1;
    int left_count;
    if(!self->buffer || !self->buffer->buffer)
    {
        if(pTextLen)
            *pTextLen = 0;
        if(pVisibleTextLen)
            *pVisibleTextLen = 0;
        return NULL;
    }

    str = self->buffer->buffer;

    str += self->index;

    if(pTextLen)
        len_limit = *pTextLen;

    left_count = self->buffer->char_len - self->index;

    if (!pVisibleTextLen) {
        if (len_limit == -1 || len_limit > left_count)
            len = left_count;
        else
            len = len_limit;
    }
    else {
        len = 0;

        while(str[len] && str[len] != '\n' 
                && len != len_limit && len < left_count) {
            len ++;
        }
        
        *pVisibleTextLen = len;
        
        if(str[len] == '\n' && len != len_limit && len < left_count) {
            len ++;
        }
    }

    if(pTextLen)
        *pTextLen = len;

    if(bAutoSkip) {
        self->index += len;
    }
    return str;
}

static int mTextIterator_diff(mTextIterator* self, 
        const ITextIterator* another)
{
    mTextIterator* ti;
    if(!another)
        return 0;

    ti = INTERFACE2OBJECT(mTextIterator,another);
    return self->index - ti->index;
}

static int mTextIterator_inRange(mTextIterator* self, 
        const ITextIterator* left, const ITextIterator* right)
{
    mTextIterator* tileft, *tiright;
    if(left == NULL || right == NULL)
        return TXTITER_RANGE_INVALID;
    
    tileft = INTERFACE2OBJECT(mTextIterator,left);
    tiright = INTERFACE2OBJECT(mTextIterator,right);

    if(!tileft->buffer || !tiright->buffer 
            || tileft->buffer != self->buffer
            || tiright->buffer != self->buffer)
        return TXTITER_RANGE_INVALID;
    
    if(tileft->index > self->index)
        return TXTITER_RANGE_LEFTOUT;
    else if(tileft->index == self->index)
        return TXTITER_RANGE_LEFTEDGE;

    if(tiright->index < self->index)
        return TXTITER_RANGE_RIGHTOUT;
    else if(tiright->index == self->index)
        return TXTITER_RANGE_RIGHTEDGE;

    return TXTITER_RANGE_IN;

}

static void mTextIterator_releaseIterator(mTextIterator* self)
{
    DELETE(self);
}

static ITextIterator* mTextIterator_reference(mTextIterator* self)
{
    return INTERFACE_CAST(ITextIterator,self);
}

static ITextRender* mTextIterator_getTextRender(mTextIterator* self)
{
    return INTERFACE_CAST(ITextRender, &g_textRender);
}

static int mTextIterator_getCharType(mTextIterator* self, int offset)
{
    int idx = self->index + offset;
    if(self->buffer)
    {
        if(!self->buffer->buffer)
            return TI_OUTOFRANGE;
        if(idx < 0)
            idx = 0;
        else if(idx >= self->buffer->char_len)
            idx = self->buffer->char_len - 1;

        if(self->buffer->buffer[idx] == '\n')
            return TI_LINERETURN ;
        if(self->buffer->buffer[idx] == 0)
            return TI_EOF;

        return TI_VISIBILE_CHAR;    
    }
    return TI_OUTOFRANGE;
}

BEGIN_MINI_CLASS(mTextIterator, mObject)
    IMPLEMENT(mTextIterator, ITextIterator)
    CLASS_METHOD_MAP(mTextIterator, construct)
    CLASS_METHOD_MAP(mTextIterator, nextChar)
    CLASS_METHOD_MAP(mTextIterator, prevChar)
    CLASS_METHOD_MAP(mTextIterator, index)
    CLASS_METHOD_MAP(mTextIterator, reset)
    CLASS_METHOD_MAP(mTextIterator, isEnd)
    CLASS_METHOD_MAP(mTextIterator, isHead)
    CLASS_METHOD_MAP(mTextIterator, getMore)
    CLASS_METHOD_MAP(mTextIterator, reference)
    CLASS_METHOD_MAP(mTextIterator, diff)
    CLASS_METHOD_MAP(mTextIterator, inRange)
    CLASS_METHOD_MAP(mTextIterator, releaseIterator)
    CLASS_METHOD_MAP(mTextIterator, getTextRender)
    CLASS_METHOD_MAP(mTextIterator, getCharType)
END_MINI_CLASS

////////////////////////////////////////
//mTextLayoutNode
typedef struct _TextLayoutNodeSearchInfo {
    int type;
    union {
        unsigned int str_index;
        unsigned int text_index;
        struct {
            int x_find;
            int y_find;
        }pos;
    }key;
    unsigned int text_count;
    unsigned int str_count;
    int x_count, y_count;
}mTextLayoutNodeSearchInfo;

typedef struct _TextLayoutNodeDiff{
    int strdiff;
    int textdiff;
    int hdiff;
    int old_width; //the old_width before diff
    int new_width; //the new_width before diff
}mTextLayoutNodeDiff;

enum {
    TLNST_STR_INDEX = 0,
    TLNST_TEXT_INDEX,
    TLNST_TEXT_POS
};

static void mTextLayoutNode_resetKey(mTextLayoutNode* self)
{
    self->text_count = 0;
    self->str_count  = 0;
    self->width = 0;
    self->height = 0;
}

#define TL_ROOT(self)    ((mTextLayoutNode*)((self)->rootNode))
static inline unsigned int textlayout_get_text_count(mTextLayout* self)
{
    return TL_ROOT(self)->text_count;
}
static inline unsigned int textlayout_get_str_count(mTextLayout* self)
{
    return TL_ROOT(self)->str_count;
}

static inline unsigned int textlayout_get_total_height(mTextLayout* self)
{
    return TL_ROOT(self)->height;
}

#undef TL_ROOT

static mCommBTreeNode* _get_last_child_node(mCommBTreeNode* node, BOOL bNotNullKey)
{
    mCommBTreeNode* lastNotNullNode = node;
    if(!node)
        return node;

    while(node && (!bNotNullKey || !_c(node)->isNullKey(node)))
    {
        lastNotNullNode = node;
        node = node->next;
    }

    node = lastNotNullNode;

    if(node->level == 0 || !node->children)
        return node;
    else
        return _get_last_child_node(node->children, bNotNullKey);
}

static inline mTextLayoutNode* textlayout_get_last_node(mTextLayout* self, BOOL bNotNullKey)
{
    return (mTextLayoutNode*)_get_last_child_node((mCommBTreeNode*)self->rootNode, bNotNullKey);
}


static int mTextLayoutNode_compareNode(mTextLayoutNode* self, 
        mTextLayoutNodeSearchInfo* searchInfo)
{
    unsigned int text_count, str_count;
    int y_count;
    if(searchInfo == NULL)
        return BTREE_ERRNO_SNULL;

    text_count = self->text_count + searchInfo->text_count;
    str_count  = self->str_count + searchInfo->str_count;
    y_count    = self->height + searchInfo->y_count;

    if(searchInfo->type == TLNST_STR_INDEX)
    {
        if(searchInfo->key.str_index < str_count)
            return BTREE_ERRNO_SCHILD;
    }
    else if(searchInfo->type == TLNST_TEXT_INDEX)
    {
        if(searchInfo->key.text_index < text_count)
            return BTREE_ERRNO_SCHILD;
    }
    else
    {
        if(searchInfo->key.pos.y_find <= y_count)
            return BTREE_ERRNO_SCHILD;
    }

    searchInfo->text_count = text_count;
    searchInfo->str_count  = str_count;
    searchInfo->y_count    = y_count;
    return BTREE_ERRNO_SNEXT;
}

static void mTextLayoutNode_recompute(mTextLayoutNode* self, 
        mTextLayoutNode* child)
{
    Class(mCommBTreeNode).recompute((mCommBTreeNode*)self, (mCommBTreeNode*)child);

    if(self->level == 0)
        return;

    self->text_count += child->text_count;
    self->str_count  += child->str_count;
    if(self->width < child->width)
        self->width = child->width;

    self->height += child->height;
}

static void mTextLayoutNode_changeKey(mTextLayoutNode* self, 
        mTextLayoutNodeDiff* pdiff, int deltaChild)
{
    mTextLayoutNodeDiff diff;
    if(pdiff == NULL)
    {
        diff.textdiff = self->text_count;
        diff.strdiff  = self->str_count;
        diff.old_width = 0;
        diff.new_width = self->width;
        diff.hdiff = self->height;
        pdiff = &diff;
        
    }
    else if(pdiff == (mTextLayoutNodeDiff*)(-1))
    {
        diff.textdiff = -self->text_count;
        diff.strdiff  = -self->str_count;
        diff.old_width = self->width;
        diff.new_width = 0;
        diff.hdiff = -self->height;
        pdiff = &diff;
    }
    else 
    {
        if(self->level > 0)
        {
            self->text_count += pdiff->textdiff;
            self->str_count  += pdiff->strdiff;
            self->height += pdiff->hdiff;

            if(pdiff->new_width > pdiff->old_width) 
            {   //replace the old max width
                if(pdiff->new_width > self->width)
                    self->width = pdiff->new_width;
            }
            else if(pdiff->new_width < pdiff->old_width 
                    && pdiff->old_width == self->width) 
            {//the old width is the max width, we must refind the new max width.
                int max_width = 0;
                mTextLayoutNode* node = (mTextLayoutNode*)self->children;
                while(node)
                {
                    if(max_width < node->width)
                        max_width = node->width;
                    node = (mTextLayoutNode*)node->next;
                }
                self->width = max_width;
            }
        }
    }

     Class(mCommBTreeNode).changeKey((mCommBTreeNode*)self, pdiff, deltaChild);

}

static BOOL mTextLayoutNode_isNullKey(mTextLayoutNode* self)
{
    return self->str_count == 0;
}

BEGIN_MINI_CLASS(mTextLayoutNode, mCommBTreeNode)
    CLASS_METHOD_MAP(mTextLayoutNode, resetKey)
    CLASS_METHOD_MAP(mTextLayoutNode, compareNode)
    CLASS_METHOD_MAP(mTextLayoutNode, recompute)
    CLASS_METHOD_MAP(mTextLayoutNode, changeKey)
    CLASS_METHOD_MAP(mTextLayoutNode, isNullKey)
END_MINI_CLASS


//////////////////////////////////////////////
//get the text byte length
static inline int textlayout_get_text_byte_length(ITextIterator* txtit, 
        HDC hdc, int text_count)
{
    int str_len = 0;
    int index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit));
    while(text_count > 0)
    {
        int count = text_count;
        ITextRender* render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        int len = (render)->_vtable->getTextByteLen(INTEFACE_ADJUST(render), hdc, txtit, &count);
        str_len += len;
        text_count -= count;
        if(text_count <= 0)
            break;
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index + str_len);
    }
    //reset at begin
    if((txtit)->_vtable->index(INTEFACE_ADJUST(txtit)) != index)
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index);

    return str_len;
}

//get the text count by length
static int textlayout_get_text_count_by_length(ITextIterator* txtit, 
        HDC hdc, int str_len, int *pstr_len)
{
    ITextRender* render;
    int          index, count;
    int          old_index;
    int          text_count = 0;

    if (pstr_len)
        *pstr_len = 0;

    if(str_len <= 0)
        return 0;
    
    count = 0;
    old_index = index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit));
    while(str_len > 0)
    {
        int len = str_len;
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index);
        render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        text_count += (render)->_vtable->getTextCount(INTEFACE_ADJUST(render), hdc, txtit, &len);
        if(len == 0)
            break;

        count += len;
        str_len -= len;
        index += len;
    }

    if (pstr_len)
        *pstr_len = count;
    (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), old_index);
    return text_count;
}

//get the width of text by str index, return the width
static int textlayout_get_text_width_by_strindex(mTextLayout* self,
        mTextLayoutNode* node,
        HDC hdc, 
        ITextIterator *txtit,
        /*in out*/int *pstr_index,
        /*out*/int *ptext_index)
{
    RECT rcLimit;
    RECT rcBound;
    int  str_index = *pstr_index;
    ITextRender *render;
    int  width = 0;
    int  index;
    int  text_index = 0;

    rcLimit.left = rcLimit.top = 0;
    rcLimit.right = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);
    rcLimit.bottom = node->height;

    while((index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit))) < str_index && rcLimit.right > 0)
    {
        int len;
        int text_count;
        render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        len = (render)->_vtable->calc(INTEFACE_ADJUST(render), hdc, txtit, str_index - index, 
                &text_count, &rcLimit, &rcBound);
        if(len == 0 || RECTW(rcBound) == 0)
            break;
        text_index += text_count;
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index + len);
        width += RECTW(rcBound);
    }

    *pstr_index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit));
    if(ptext_index)
        *ptext_index = text_index;
    return width;
}

//get the text index by width, return text_index
static int textlayout_get_text_index_by_width(mTextLayout* self,
        mTextLayoutNode* node,
        /*in out*/int* pwidth,
        HDC hdc /*= 0*/,
        ITextIterator* txtit,
        int *pstr_index)
{
    BOOL         bGetDC = (hdc == 0);
    RECT         rcLimit, rcBound;
    int          str_index = 0;
    int          text_index = 0;
    ITextRender *render;
    int          width = *pwidth;
    int          str_len = node->str_count;

    if(width >= node->width)
    {
        *pwidth = node->width;
        text_index = node->text_count;

        if((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), str_len - 1) == TI_LINERETURN)
        {
            str_len --;
            text_index --;
        }

        if(pstr_index)
            *pstr_index = str_len;
        return text_index;
    }


    rcLimit.left = rcLimit.top = 0;
    rcLimit.bottom = node->height;

    if(bGetDC)
        hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));

    while(width > 0)
    {
        int len;
        int text_count;
        int index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit));
        rcLimit.right = width;
        render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        len = (render)->_vtable->calc(INTEFACE_ADJUST(render), hdc, txtit, str_len, 
                &text_count, &rcLimit, &rcBound);
        if(len >0 && (txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), len - 1) == TI_LINERETURN)
        {
            int visibile_len;
            (void)(txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), &len, &visibile_len, FALSE);
            width -= RECTW(rcBound);
            str_index += visibile_len;
            text_index += text_count;
            break;
        }

        if(len == 0 || RECTW(rcBound) == 0)
            break;

        width      -= RECTW(rcBound);
        str_index  += len;
        text_index += text_count;
        str_len    -= len;
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index + len);
    }

    if(bGetDC)
        (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);

    if(pstr_index)
        *pstr_index = str_index;
    *pwidth -= width;
    return text_index;
}


//////////////////////////////////////////
static inline void append_updaterc(mTextLayout* self, RECT *prcupate)
{
    if(self->dirty_area == NULL)
        self->dirty_area = CreateClipRgn();
    AddClipRect(self->dirty_area, prcupate);
}

static void textlayout_update_selection_ex(mTextLayout* self, 
        int *begin, int *end)
{
    RECT rcUpdate;      
        
    rcUpdate.left = begin[0]; //begin.x
    rcUpdate.top  = begin[1]; //begin.y
    //printf("--- update selection from : begin(%d,%d)-end(%d,%d)\n", begin[0],begin[1],end[0],end[1]);

    //the same line
    if(begin[1] == end[1])
    {
        rcUpdate.right = end[0]; //end.x
        rcUpdate.bottom = rcUpdate.top + begin[2]; //begin.height
        //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), &rcUpdate);
        append_updaterc(self, &rcUpdate);
    }
    else
    {
        //multiple lines, first update the above line
        rcUpdate.right = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);
        rcUpdate.bottom = rcUpdate.top + begin[2]; //begin.height
        //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), &rcUpdate);
        append_updaterc(self, &rcUpdate);

        //second: update middle line(s) 
        rcUpdate.top  = begin[1] + begin[2]; //begin.y + begin.height
        if(rcUpdate.top < end[1]/*end.y*/) 
        {
            //int top = rcUpdate.top;
            rcUpdate.left = 0;
            rcUpdate.right = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);
            rcUpdate.bottom = end[1]; //end.y
            //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), &rcUpdate);
            append_updaterc(self, &rcUpdate);
        }

        rcUpdate.left = 0;
        rcUpdate.right = end[0]; //end.x
        rcUpdate.top = end[1]; //end.y
        rcUpdate.bottom = end[1] + end[2]; //end.y + end.height
        //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), &rcUpdate);
        append_updaterc(self, &rcUpdate);
    }

    //Don't call update with TRUE. must call it when the scrollbar has changed
    //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), TRUE);
}

static void textlayout_updateselection(mTextLayout* self)
{
    int *begin; //begin[0] : begin.x, begin[1]: begin.y, begin[2]:begin.height
    int *end;

    if(self->sel_begin == self->cursor)
        return;


    if(self->sel_begin > self->cursor) {
        begin = &self->cursor_x;
        end   = &self->sel_begin_x;
    }
    else {
        begin = &self->sel_begin_x;
        end   = &self->cursor_x;
    }

    if(self->old_height != -1
            && self->old_height != self->new_height)
    {
        int max;
        if(self->old_height > self->new_height)
        {
            max = self->old_height;
        }else{
            max = self->new_height;
        }

        if(begin[1] >= max)
        {
            begin[1] = begin[1] + self->new_height - self->old_height;
            end[1] = end[1] + self->new_height - self->old_height;
        }
        else if(end[1] >= max)
        {
            end[1] = end[1] + self->new_height - self->old_height;
        }
 
    }

    textlayout_update_selection_ex(self, begin, end);
}

static void textlayout_resetsel(mTextLayout* self)
{
    self->sel_begin = self->cursor;
    self->sel_begin_x = self->cursor_x;
    self->sel_begin_y = self->cursor_y; 
    self->sel_begin_height = self->cursor_height;
    self->text_sel_begin = self->text_cursor;
}

static void textlayout_update_new_selection(mTextLayout* self, 
        int new_index, int x_sel, int y_sel, int height)
{
    //get the max update selection
    int sel_info[] = {new_index, x_sel, y_sel, height};
    int *pa, *pc;
    int *pbegin, *pend;
    pa = sel_info;
    pc = &self->sel_begin;

    //not process selection between min and cursor.
    pbegin = *pa < *pc ? pa : pc;
    pend = *pa < *pc ? pc : pa;

    textlayout_update_selection_ex(self, pbegin + 1, pend + 1);
}

#define TEXTLAYOUT_ISEMPTY(self)   \
    (((mTextLayoutNode*)(self)->rootNode)->str_count == 0)

static inline mTextLayoutNode* _get_first_child(mTextLayoutNode*node)
{
    if(node->level == 0 || !node->children)
        return node;
    return _get_first_child((mTextLayoutNode*)node->children);
}

static inline mTextLayoutNode* textlayout_get_first_node(mTextLayout* self)
{
    return _get_first_child((mTextLayoutNode*)self->rootNode);
}

static inline mTextLayoutNode* 
search_textlayout_node_by_textindex(mTextLayout* self, 
        mTextLayoutNodeSearchInfo* searchInfo,
        int index, int search_flags)
{
    memset(searchInfo, 0, sizeof(mTextLayoutNodeSearchInfo));
    searchInfo->type = TLNST_TEXT_INDEX;
    if(TEXTLAYOUT_ISEMPTY(self))
        return textlayout_get_first_node(self);
    else if(index > textlayout_get_text_count(self))
        index = textlayout_get_text_count(self);
    if(index < 0)
        index = 0;
    searchInfo->key.text_index = index;
    return (mTextLayoutNode*)_c(self)->search(self, (void*)searchInfo, search_flags);
}

static inline mTextLayoutNode* 
search_textlayout_node_by_strindex(mTextLayout* self, 
        mTextLayoutNodeSearchInfo* searchInfo,
        int index, int search_flags)
{
    memset(searchInfo, 0, sizeof(mTextLayoutNodeSearchInfo));
    searchInfo->type = TLNST_STR_INDEX;
    if(TEXTLAYOUT_ISEMPTY(self))
        return textlayout_get_first_node(self);
    else if(index > textlayout_get_str_count(self))
        index = textlayout_get_str_count(self);
    searchInfo->key.str_index = index;
    return (mTextLayoutNode*)_c(self)->search(self, (void*)searchInfo, search_flags);
}

static inline mTextLayoutNode* 
search_textlayout_node_by_pos(mTextLayout* self, 
        mTextLayoutNodeSearchInfo* searchInfo,
        int x, int y, int search_flags)
{
    memset(searchInfo, 0, sizeof(mTextLayoutNodeSearchInfo));
    searchInfo->type = TLNST_TEXT_POS;
    searchInfo->key.pos.x_find = x;
    searchInfo->key.pos.y_find = y;
    if(TEXTLAYOUT_ISEMPTY(self))
        return textlayout_get_first_node(self);
    return (mTextLayoutNode*)_c(self)->search(self, (void*)searchInfo, search_flags);
}

static inline int calc_line(ITextIterator* txtit, HDC hdc, 
        int str_len, /*out*/ int *ptext_count, 
        const RECT* prcLimit, RECT *prcBound)
{
    int             text_count;
    ITextRender    *render;
    int             len = 0;

    if(ptext_count)
        *ptext_count = 0;

    if(str_len <= 0)
        str_len = 0x7FFFFFFF;

    memset(prcBound, 0, sizeof(RECT));

    while(len != str_len)
    {
        RECT rcBound;
        int  tlen;
        int  index = (txtit)->_vtable->index(INTEFACE_ADJUST(txtit));
        int  all_len = str_len - len;

        render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        (void)(txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), &all_len, NULL, FALSE);
        tlen = (render)->_vtable->calc(INTEFACE_ADJUST(render), hdc,txtit,  
                str_len - len, &text_count, prcLimit, &rcBound);

        //attempt to output a character
        if(tlen == 0) {
            //get byte length with a character
            int byte_len = textlayout_get_text_byte_length(txtit, hdc, 1);
            tlen = (render)->_vtable->calc(INTEFACE_ADJUST(render), hdc, txtit,
                    byte_len, &text_count, NULL, &rcBound);
        }

        if (tlen == 0)
            break;

        len += tlen;
        prcBound->right += RECTW(rcBound);
        if(prcBound->bottom < RECTH(rcBound))
            prcBound->bottom = RECTH(rcBound);  
        if(ptext_count)
            *ptext_count += text_count;

        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), index + tlen);
        if(all_len >= tlen || (txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), -1) == TI_LINERETURN)
            break;
    }

    return len;
}

static int calc_node(mTextLayout* self, 
        mTextLayoutNode* node, 
        ITextIterator*  txtit,
        HDC             hdc,
        const RECT     *rcLimit, 
        int             linespace)
{
    RECT rcBound;
    int old_count, old_height, old_len;
    int text_count, str_len = 0x7FFFFFFF;
    mTextLayoutNodeDiff diff;

    old_count = node->text_count;
    old_len   = node->str_count;
    old_height = node->height;

    diff.old_width = node->width;

    str_len = calc_line(txtit, hdc, str_len, &text_count, rcLimit, &rcBound);

    node->text_count = text_count;
    node->str_count  = str_len;

    if(old_len <= 0 && str_len <= 0)
        return -old_len;
    //If node is changed from non-null to null, we should differ its information.
    //important: don't reset the width and height value. 
    //because we would use them to calc the update area.
    node->width = RECTW(rcBound);
    node->height = RECTH(rcBound) + linespace;
    
    diff.textdiff = node->text_count - old_count;
    diff.strdiff  = node->str_count - old_len;
    diff.new_width = node->width;
    diff.hdiff = node->height - old_height;
    if(node->parent) //change parent's key
        _c(node->parent)->changeKey(node->parent, &diff, 0);

    return old_len - str_len;
}

static void textlayout_relayout(mTextLayout* self, int begin, 
        int delCount, int insertCount, BOOL bAutoUpdate)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode           *begin_node=NULL, *end_node = NULL;
    int                       begin_index;
    ITextIterator*            txtit;
    mCommBTreeLeafIterator    node_it;
    BOOL                      bHeightChanged = FALSE; /* Very important for unequal height*/
    int maxwidth;
    int linespace;
    int total_height;
    //int old_heights = 0, new_heihts = 0; //TODO total height which is changed of textedit
    HDC hdc;
    RECT rcLimit;
    RECT rcUpdate = {0, 0, 0, 0};
    int count = (self->text_buffer)->_vtable->getCount(INTEFACE_ADJUST(self->text_buffer));
    count -= (insertCount - delCount);

    //1. find the begin node

    //if begin is equal with the character number of old contents, 
    //then we need to use begin-1 to get last valid node.
    begin_node = search_textlayout_node_by_strindex(self, 
                    &searchInfo, begin >= count ? begin - 1: begin, 
                    BTSF_RETLAST_IF_OUTOFRANGE);

    // get the end not
    if(begin_node == NULL)
    {
        end_node = begin_node = textlayout_get_last_node(self, FALSE);
        begin_index = textlayout_get_str_count(self);
    }
    else
    {
        begin_index = searchInfo.str_count;
    }

    //include the height change info
    //if(self->old_height == -1)
    //    self->old_height = searchInfo.y_count;
    self->old_height = self->new_height = searchInfo.y_count;
    //printf("--- textrelayout: old_height=%d\n", self->old_height);

    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), begin_index);

    delCount    += (begin - begin_index);
    insertCount += (begin - begin_index);
  

    linespace = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_LINESPACE);
    maxwidth  = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);
    rcLimit.left = rcLimit.top = 0;
    rcLimit.right = maxwidth;
    rcLimit.bottom = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXHEIGHT);

    rcUpdate.top = searchInfo.y_count;
    rcUpdate.bottom = rcUpdate.top;


    total_height = textlayout_get_total_height(self);

    hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));

    //exclude the area before  begin_index
    if(begin > begin_index)
    {
        //recalc the start left
        RECT rcBound;
        ITextRender *textRender = _I(txtit)->getTextRender(INTEFACE_ADJUST(txtit));
        _I(textRender)->calc(INTEFACE_ADJUST(textRender), hdc, txtit, 
                begin - begin_index, NULL, &rcLimit, &rcBound);
        rcUpdate.left = RECTW(rcBound);
    }

    //(void)INIT_OBJV(mCommBTreeLeafIterator, &node_it, 2, self, begin_node);
    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, begin_node);

    if( end_node == NULL) //get the end node
    {
        int left_del_count = delCount;
        int index = 0;
        mTextLayoutNode* tnode = NULL;
        while(node_it.current)
        {
            tnode = (mTextLayoutNode*)(node_it.current);
            if(tnode->str_count == 0)
                break;
            index += tnode->str_count;
            left_del_count -= tnode->str_count;
            if((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), index) == TI_LINERETURN)
            {
                if(left_del_count == 0) {
                    _c(&node_it)->next(&node_it);
                    tnode = (mTextLayoutNode*)(node_it.current);
                    break;
                }
                else if(left_del_count < 0) {
                    break;
                }
            }
            _c(&node_it)->next(&node_it);
        }
        end_node = tnode;
        if(!end_node)
            end_node = textlayout_get_last_node(self, FALSE);

        if(node_it.current != (mCommBTreeNode*)begin_node)
            (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, begin_node);

        delCount -= left_del_count;
        if(left_del_count < 0)
            insertCount -= left_del_count;
    }

    //replace: remove and insert
    while(1)
    {
        int height = 0;
        mTextLayoutNode* tnode = (mTextLayoutNode*)(node_it.current);

        BOOL bEnd = node_it.current == (mCommBTreeNode*)end_node;
        /** TODO:
         *  old_heights += tnode->height;
         */
        if(!bHeightChanged) //if height changed, we must update all area
        {
            rcUpdate.right = tnode->width;
            height = tnode->height;
        }
        if(tnode->str_count > 0)
            self->old_height += tnode->height;
        
        delCount -= tnode->str_count; //old count
        calc_node(self, tnode, txtit, hdc, &rcLimit, linespace);
        insertCount -= tnode->str_count;
        /**TODO:
         *  new_heihts += tnode->height;
         **/
        if(tnode->str_count == 0) {
            //remove all, the height must changed
            rcUpdate.right = maxwidth;
            rcUpdate.bottom = total_height;

            while(node_it.current) {
                if(((mTextLayoutNode*)(node_it.current))->str_count > 0)
                    self->old_height += ((mTextLayoutNode*)(node_it.current))->height;
                _c(&node_it)->remove(&node_it);
            }
            bHeightChanged = TRUE;
            goto END;
        }

        
        if(!bHeightChanged)
        {
            if(height != tnode->height)
            {
                bHeightChanged = TRUE;
                height = self->new_height; //store the start changed of height
            }
            else
            {
            //    if(rcUpdate.right < tnode->width)
            //        rcUpdate.right = tnode->width;
                //use max width
                rcUpdate.right = maxwidth;
                rcUpdate.bottom = rcUpdate.top + height;
                //add region 
                append_updaterc(self, &rcUpdate);
                rcUpdate.left = 0; //new line
                rcUpdate.top = rcUpdate.bottom;
            }
        }

        if(tnode->str_count > 0)
            self->new_height += tnode->height;

        if(delCount < 0)
            insertCount -= delCount;
        else if(insertCount < 0)
            delCount -= insertCount;

        if(bEnd && (delCount == 0 || insertCount == 0))
            break;

        _c(&node_it)->next(&node_it);

        if(delCount <= 0 || insertCount <= 0)
            break;
    }

    if(insertCount > 0)
    {
        //add new lines and set update
        mTextLayoutNode* node = NULL;

        if(node_it.current == NULL)
            node_it.current = (mCommBTreeNode*)textlayout_get_last_node(self, FALSE);

        node = (mTextLayoutNode*)node_it.current;
        if(node->str_count == 0) //a null inline, we should add it
        {
            calc_node(self, node, txtit, hdc, &rcLimit, linespace);
            insertCount -= node->str_count;
            self->new_height += node->height;
        }

        while(insertCount > 0)
        {
            int text_count;
            int str_len ;
            RECT rcBound;
            str_len = calc_line(txtit, hdc, insertCount, 
                    &text_count, &rcLimit, &rcBound);
            if(str_len <= 0)
                break;
            node = (mTextLayoutNode*)_c(self)->newLeaf(self);
            node->str_count = str_len;
            node->text_count = text_count;
            node->width  = RECTW(rcBound);
            node->height = RECTH(rcBound) + linespace;

            _c(&node_it)->insert(&node_it, (mCommBTreeNode*)node);


            insertCount -= str_len;

            //include new height;
            self->new_height += node->height;
        }

        bHeightChanged = TRUE;
    }
    else if(node_it.current)
    {
        //delete the lines update all the area of line

        if (delCount == 0) {
            (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), textlayout_get_str_count(self));

            //whether delete last null line
            if((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), - 1) != TI_LINERETURN) {
                mTextLayoutNode* node = textlayout_get_last_node(self, FALSE);
                if (node->str_count <= 0) {
                    self->old_height += node->height;
                    _c(self)->deleteNode(self, (mCommBTreeNode*)node);
                }
            }
        }
        if(delCount > 0)
        {
            //need change the height
            bHeightChanged = TRUE;
        }
        while(delCount > 0 && node_it.current)
        {
            mTextLayoutNode* tnode = (mTextLayoutNode*)(node_it.current);
            delCount -= tnode->str_count;
            
            if(tnode->str_count > 0)
                 self->old_height += tnode->height;
            _c(&node_it)->remove(&node_it);

            if(tnode == end_node)
                break;
        }
    }


END:
    //if last node is ended with '\n', we should add a new empty line 
    //to make sure root node's height correct.
    end_node = textlayout_get_last_node(self, FALSE);
    //printf("---- end_node=%p, text count = %d\n", end_node, end_node?end_node->text_count:0);
    if(end_node)
    {
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), textlayout_get_str_count(self));
        //the last line is not null, and end with \n, we must ensure the last line is a null line
        if ((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), - 1) == TI_LINERETURN) {
            if(end_node->text_count > 0) //insert a new null line
            {
                //note:use the above end_node's height
                int height = _I(self->context)->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_DEFLINEHEIGHT);
                node_it.current = (mCommBTreeNode*)end_node;
                end_node = (mTextLayoutNode*)_c(self)->newLeaf(self);
                end_node->str_count = 0;
                end_node->text_count = 0;
                end_node->width = 0;
                end_node->height = height;
                _c(&node_it)->insert(&node_it, (mCommBTreeNode*)end_node);
            }
            else if(end_node->text_count <= 0 && end_node->height <= 0)
            {
                //make sure the end_node->height > 0
                int height = end_node->height;
                mTextLayoutNodeDiff diff;
                memset(&diff, 0, sizeof(diff));
                end_node->height = _I(self->context)->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_DEFLINEHEIGHT);
                diff.hdiff = end_node->height - height;
                if(end_node->parent)
                    _c(end_node->parent)->changeKey(end_node->parent, &diff, 0);
            }
            //printf("--- insert null end node = %p, height=%d\n", end_node,height);
            bHeightChanged = TRUE;
        }
    }


    (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    //printf("--- old height=%d, new height=%d\n",self->old_height, self->new_height);
    
    if(bHeightChanged)
    {
        int maxheight = self->new_height;
        if(maxheight < self->old_height)
            maxheight = self->old_height;
        rcUpdate.left = 0;
        rcUpdate.right = maxwidth;
        rcUpdate.bottom = maxheight;
        append_updaterc(self, &rcUpdate);
        //rcUpdate.bottom = rcUpdate.top + (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXHEIGHT);
    }
    else
    {
        self->old_height = -1; //no height changed
    }
    (self->context)->_vtable->update(INTEFACE_ADJUST(self->context), bAutoUpdate);

}

static void mTextLayout_reset(mTextLayout* self)
{
    int count = (self->text_buffer)->_vtable->getCount(INTEFACE_ADJUST(self->text_buffer));

    textlayout_relayout(self, 0, count, count, FALSE);

    //recalc cursor and sel_begin position
    self->text_cursor = _c(self)->getCharPos(self, self->cursor, 
            &self->cursor_x, &self->cursor_y, &self->cursor_height, FALSE);

    if (self->sel_begin != self->cursor) {
        self->text_sel_begin = _c(self)->getCharPos(self, self->sel_begin, 
                &self->sel_begin_x, &self->sel_begin_y, 
                &self->sel_begin_height, FALSE);
    }
    else
        textlayout_resetsel(self);

    //(self->context)->_vtable->update(INTEFACE_ADJUST(self->context), &rcUpdate);
    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
}


static void mTextLayout_construct(mTextLayout* self, va_list va)
{
    Class(mCommBTree).construct((mCommBTree*)self, 0);
    
    ncsParseConstructParams(va, "pp", &self->text_buffer, &self->context);

    self->old_height = -1;
}

static void mTextLayout_destroy(mTextLayout* self)
{
    if(self->dirty_area)
    {
        DestroyClipRgn(self->dirty_area);
    }

    Class(mCommBTree).destroy((mCommBTree*)self);
}

#if 0
//get the para pos by text
static int textlayout_get_text_para_pos(mTextLayout* self, int text_index, int *offChar)
{
    mTextLayoutNode* node = textlayout_get_first_node(self);
    mCommBTreeLeafIterator    node_it;
    int text_idx = 0;
    int para_idx = 0;
    if(!node)
        return 0;

    if(text_index <= 0)
    {
        if(offChar)
            *offChar = 0;
        return 0;
    }

    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, node);

    while(node_it.current && (text_idx + node->text_count) <= text_index)
    {
        node = (mTextLayoutNode*)(node_it.current);
        text_idx += node->text_count;
        _c(&node_it)->next(&node_it);
    }


    if(offChar)
    {
        if(text_idx < text_index)
            _c(self)->textCount2Len(self, text_idx, text_index - text_idx, offChar);
        else
            *offChar = 0;
    }
    
    return para_idx;
}
#endif

#define GETCHARINDEX_RET(x, y, height, str_index, text_index)  do { \
    *px = (x);      \
    *py = (y);      \
    if(plineHeight) *plineHeight = (height);    \
    if(pstr_index)  *pstr_index  = (str_index); \
    return (text_index);  }while(0)

static int mTextLayout_getCharIndex(mTextLayout* self, int* px, int* py, 
        int *plineHeight, int *pstr_index)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode  *node;
    ITextIterator   *txtit;
    int             text_index=0,str_index=0;
    int             x;
    int             y;
    int             width;

    if(!self->text_buffer || !px || !py)
        return -1;

    if ((self->text_buffer)->_vtable->getCount(INTEFACE_ADJUST(self->text_buffer)) <= 0) {
        *px = 0;
        *py = 0;
        return 0;
    }

    x = *px;
    y = *py;

    node = search_textlayout_node_by_pos(self, &searchInfo, x, y,0);

    if(node == NULL)
    {
        // FALSE can get the last empty node, if have.
        //node = textlayout_get_last_node(self, TRUE);
        node = textlayout_get_last_node(self, FALSE);
        GETCHARINDEX_RET(node->width, 
                textlayout_get_total_height(self) - node->height, 
                node->height,
                textlayout_get_str_count(self),
                textlayout_get_text_count(self));
    }

    if(searchInfo.x_count == x)
    {
        GETCHARINDEX_RET(0, searchInfo.y_count, node->height, 
                searchInfo.str_count, searchInfo.text_count);
    }
    else if(node->width <= x)
    {
        int reduce = 0;
        if(node->str_count > 0)
        {
            txtit = _I(self->text_buffer)->getAt(INTEFACE_ADJUST(self->text_buffer), searchInfo.str_count);
            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), node->str_count -1) == TI_LINERETURN)
                reduce = 1;
            _I(txtit)->releaseIterator(INTEFACE_ADJUST(txtit));
        }
        //printf("-- reduce=%d\n",reduce);

        GETCHARINDEX_RET(node->width, searchInfo.y_count, node->height,
                searchInfo.str_count + node->str_count - reduce , 
                searchInfo.text_count + node->text_count - reduce);
    }

    //recalc the left size
    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), searchInfo.str_count);
    width = x - searchInfo.x_count;
    text_index = textlayout_get_text_index_by_width(self, 
            node,
            &width,
            0,
            txtit,
            &str_index) + searchInfo.text_count;

    GETCHARINDEX_RET(width, 
            searchInfo.y_count, 
            node->height,
            str_index + searchInfo.str_count,
            text_index);
}
#undef GETCHARINDEX_RET

#define GETCHARPOS_RET(x, y, index)  do{ \
    if(px) *px = (x);                     \
    if(py) *py = (y);                     \
    return (index); } while(0)

static int _get_str_width(mTextLayout *self, 
        mTextLayoutNode *node, int str_index, int len)
{
    ITextIterator   * txtit;
    HDC     hdc;
    int     width;

    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), str_index);
    hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));

    str_index += len;
    width = textlayout_get_text_width_by_strindex(self, node, 
                        hdc, txtit, &str_index, NULL);
    (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    return width;
}

static int mTextLayout_getCharPos(mTextLayout* self, int index, 
        int *px, int *py, int *plineHeight, BOOL bAsText)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode  *node;
    HDC             hdc;
    int text_index, str_index, width, count;
    ITextIterator   *txtit;

    if(!self->text_buffer)
        return -1;

    if (index < 0)
        index = 0;

    count = bAsText ? textlayout_get_text_count(self) 
                    : textlayout_get_str_count(self);
    if (count <= 0)
        GETCHARPOS_RET(0, 0, 0);

    if (index > count)
        index = count;

    if(bAsText) {
        //only for last text, need to use last index to get correct node.
        text_index = index >= count ? count - 1 : index;
        node = search_textlayout_node_by_textindex(self, &searchInfo, 
                text_index, BTSF_RETLAST_IF_OUTOFRANGE);
        if(!node)
            return -1;

    
        if(plineHeight)
            *plineHeight = node->height;

        if(text_index == searchInfo.text_count) {
            if (index != text_index) {
                //for last text, need add the last index's length and width
                int width, len, str_index;
                _c(self)->textCount2Len(self, text_index, 1, &len);
                str_index = textlayout_get_str_count(self) - len;

                if (len == 1) {
                    BOOL isLnSep;
                    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), str_index);
                    isLnSep = (txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), str_index) == TI_LINERETURN;
                    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
                    //for null line, should skip to next line's start position.
                    if (isLnSep) {
                        GETCHARPOS_RET(0, searchInfo.y_count + node->height, 
                                searchInfo.str_count + len);
                    }
                }

                width = _get_str_width(self, node, str_index, len);
                GETCHARPOS_RET(width, searchInfo.y_count, searchInfo.str_count + len);
            }
            else {
                GETCHARPOS_RET(0, searchInfo.y_count, searchInfo.str_count);
            }
        }
        text_index = index;
    }
    else
    {
        //only for last char, need to use last index to get correct node.
        str_index = index >= count ? count -1 : index;
        node = search_textlayout_node_by_strindex(self, 
                &searchInfo, str_index, BTSF_RETLAST_IF_OUTOFRANGE);
        if(!node)
            return -1;

        if(plineHeight)
            *plineHeight = node->height;

        if(str_index == searchInfo.str_count) {
            if (index != str_index) {
                BOOL isLnSep;
                txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), str_index);
                isLnSep = (txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), str_index) == TI_LINERETURN;
                (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
                //for last char, need add the last index's length and width.
                // and for null line, should skip to next line's start position.
                if(isLnSep) {
                    GETCHARPOS_RET(0, searchInfo.y_count + node->height, 
                            searchInfo.text_count + 1);
                }
                else {
                    GETCHARPOS_RET(_get_str_width(self, node, str_index, 1), 
                        searchInfo.y_count, searchInfo.text_count + 1);
                }
            }
            else {
                GETCHARPOS_RET(0, searchInfo.y_count, searchInfo.text_count);
            }
        }
        if (index != str_index)
            str_index = index;
    }

    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), searchInfo.str_count);

    if((bAsText && text_index >= (searchInfo.text_count + node->text_count))
        || (!bAsText && str_index >= (searchInfo.str_count + node->str_count)))
    {
        int xtmp , ytmp = searchInfo.y_count;
        if((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), node->str_count-1) == TI_LINERETURN)
        {
            xtmp = 0;
            ytmp += node->height;
        }
        else
        {
            xtmp = node->width;
        }

        (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
        GETCHARPOS_RET(xtmp, ytmp, 
                bAsText?(searchInfo.str_count + node->str_count)
                    :(searchInfo.text_count + node->text_count));
    }

    hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));

    if(bAsText)
        str_index = searchInfo.str_count + 
            textlayout_get_text_byte_length(txtit, hdc, 
                    text_index - searchInfo.text_count);

    width = textlayout_get_text_width_by_strindex(self, 
                    node, hdc, txtit, &str_index, &text_index);
    text_index += searchInfo.text_count;


    (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    GETCHARPOS_RET(width, searchInfo.y_count, bAsText?str_index:text_index);

}
#undef GETCHARPOS_RET

static int mTextLayout_setSelBeginPos(mTextLayout* self, 
        int x, int y, BOOL bUpdate)
{
    int x_sel, y_sel, height;
    int str_index;
    int text_index;
    if(x == self->sel_begin_x && y == self->sel_begin_y)
        return self->sel_begin;

    if(y < 0)
        y = 0;
    else if( y > (int)((mTextLayoutNode*)self->rootNode)->height)
        y = ((mTextLayoutNode*)self->rootNode)->height - 1;

    if(x < 0)
        x = 0;
    else if(x > (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH))
        x = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);

    x_sel = x;
    y_sel = y;

    text_index = _c(self)->getCharIndex(self, &x_sel, &y_sel, &height, &str_index);

    if(str_index < 0)
        return self->sel_begin;

    //printf("---- x_sel=%d,    y_sel=%d,    str_index=%d, text_index=%d\n", x_sel, y_sel, str_index, text_index);
    //printf("---- cursor_x=%d, cursor_y=%d, cursor=%d,    text_cursor=%d\n", self->cursor_x, self->cursor_y, self->cursor, self->text_cursor); 
    if(bUpdate) 
        textlayout_update_new_selection(self, str_index, x_sel, y_sel, height);

    self->sel_begin        = str_index;
    self->sel_begin_x      = x_sel;
    self->sel_begin_y      = y_sel;
    self->sel_begin_height = height;
    self->text_sel_begin   = text_index;

    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
    return text_index;  
}

static BOOL mTextLayout_setSelBegin(mTextLayout* self, 
        int text_index, BOOL bUpdate)
{
    int x_sel, y_sel;
    int height = 0;
    int str_index;

    if(text_index < 0)
        text_index = 0;
    else {
        int count = textlayout_get_text_count(self); //(self->text_buffer)->_vtable->getCount(INTEFACE_ADJUST(self->text_buffer));
        if(text_index >= count)
            text_index = count;
    }

    if(text_index == self->text_cursor)
    {
        if(bUpdate)
        {
            textlayout_updateselection(self);
        }
        textlayout_resetsel(self);
        return TRUE;
    }
    
    str_index = _c(self)->getCharPos(self, text_index, &x_sel, &y_sel,&height, TRUE);

    if(bUpdate) 
        textlayout_update_new_selection(self, str_index, x_sel, y_sel, height);

    self->sel_begin        = str_index;
    self->sel_begin_x      = x_sel;
    self->sel_begin_y      = y_sel;
    self->sel_begin_height = height;
    self->text_sel_begin   = text_index;
    
    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
    return TRUE;
}

static int mTextLayout_getSelBegin(mTextLayout* self, int* px, int *py,
        int *pHeight)
{
    if(px)
        *px = self->sel_begin_x;
    if(py)
        *py = self->sel_begin_y;

    if (pHeight)
        *pHeight = self->sel_begin_height;

    return self->text_sel_begin;
}

static BOOL mTextLayout_setCursor(mTextLayout* self, int text_index, BOOL isSel)
{
    if (isSel) {
        return mTextLayout_setSelBegin(self, text_index, TRUE);
    }

    if(self->sel_begin != self->cursor) {
        textlayout_updateselection(self);
    }

    if (text_index < 0)
        self->text_cursor = 0;
    else {
        int count = textlayout_get_text_count(self);
        if (text_index >= count)
            self->text_cursor = count;
        else
            self->text_cursor = text_index;
    }
    
    self->cursor = _c(self)->getCharPos(self, self->text_cursor, &self->cursor_x, 
            &self->cursor_y, &self->cursor_height, TRUE);

    //printf("===== cursor_x=%d, cursor_y=%d, cursor=%d,    text_cursor=%d\n", self->cursor_x, self->cursor_y, self->cursor, self->text_cursor); 

    textlayout_resetsel(self);

    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
    return TRUE;
}

static int mTextLayout_setCursorPos(mTextLayout* self, int x, int y, BOOL isSel)
{
    if (isSel) {
        return mTextLayout_setSelBeginPos(self, x, y, TRUE);
    }

    if(x == self->cursor_x && y == self->cursor_y)
        return self->text_cursor;

    if( y > ((mTextLayoutNode*)self->rootNode)->height)
        return self->text_cursor;

    if(self->sel_begin != self->cursor) {
        textlayout_updateselection(self);
    }

    self->cursor_x = x;
    self->cursor_y = y;
    self->text_cursor = _c(self)->getCharIndex(self, &self->cursor_x, 
            &self->cursor_y, &self->cursor_height, &self->cursor);
    //printf("++++ cursor_x=%d, cursor_y=%d, cursor=%d,    text_cursor=%d\n", self->cursor_x, self->cursor_y, self->cursor, self->text_cursor); 
    
    textlayout_resetsel(self);

    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
    return self->text_cursor;
}

static int mTextLayout_getCursor(mTextLayout* self, int *px, int *py, 
        int *pcursorHeight, BOOL isSel)
{
    if (isSel) {
        return mTextLayout_getSelBegin(self, px, py, pcursorHeight);
    }

    if(px)
        *px = self->cursor_x;
    if(py)
        *py = self->cursor_y;
    if(pcursorHeight)
        *pcursorHeight = self->cursor_height;
    return self->text_cursor;
}

static mTextLayoutNode* get_begin_line(mTextLayout* self, 
        int* px, int* py, int *pindex)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode* node;
    node = search_textlayout_node_by_pos(self, &searchInfo, *px, *py, 0);
    if(!node)
        return node;
    if(pindex)
        *pindex = searchInfo.str_count;
    
    if(py)
        *py = searchInfo.y_count;
    if(px)
        *px = searchInfo.x_count;
    return node;
}

static int textlayout_drawline(mTextLayout* self,
        HDC hdc, 
        int* px, int y, 
        mTextLayoutNode* node, 
        int *pcount_left, ITextIterator* it, 
        int max_selection, BOOL bsel)
{
    int index = (it)->_vtable->index(INTEFACE_ADJUST(it));  
    int total_count = *pcount_left;
    int index_begin = index;
    int draw_count;
    ITextRender* textRender;
    //RECT rcbg;
    //BOOL bRecalcWidth = FALSE;
    //BOOL drawLeftBg;

    //if is in selection and the whole line is selected, we should draw whole line
    //drawLeftBg = (bsel &&  (total_count == max_selection - index));
    //printf("--- bsel =%d, total_count=%d, max_selection=%d, index=%d\n",bsel, total_count, max_selection, index);

    if(max_selection >0 && total_count > max_selection - index)
    {
        //the left chars from max_selection to (index + total_count) is selected or unslected,
        //so, we cannot draw the whole line, so we must recalc the width
        total_count = max_selection - index;
        //bRecalcWidth = TRUE;
    }

    textRender = (it)->_vtable->getTextRender(INTEFACE_ADJUST(it));
#if 0
    rcbg.left = *px;
    rcbg.bottom = y + node->height;
    rcbg.top  =  y;

    //printf("---- rcbg=%d,%d,%d,%d\n",rcbg.left, rcbg.right, rcbg.top, rcbg.bottom);
    if(rcbg.bottom > rcbg.top)
    {
        rcbg.right = (self->context)->_vtable->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_MAXWIDTH);
        if(bRecalcWidth && !drawLeftBg) //if drawLeftBg is true, mean we should draw whole line
        {
            RECT rcBounds;
            (textRender)->_vtable->calc(INTEFACE_ADJUST(textRender), hdc, 
                    it, total_count, NULL, &rcbg, &rcBounds);
            rcbg.right = rcbg.left + RECTW(rcBounds);
        }
        //printf("---- draw : rcbg=%d,%d,%d,%d\n",rcbg.left, rcbg.right, rcbg.top, rcbg.bottom);
        (self->context)->_vtable->drawBkgnd(INTEFACE_ADJUST(self->context), hdc, &rcbg, bsel);
    }
#endif

    while(total_count > 0 && !(it)->_vtable->isEnd(INTEFACE_ADJUST(it)))
    {
        int old_index = index;
        int tx = *px, ty = y;
        _I(self->context)->contentPosToDCPos(INTEFACE_ADJUST(self->context), hdc, &tx, &ty);
        *px += (textRender)->_vtable->draw(INTEFACE_ADJUST(textRender), hdc, tx, ty, it, total_count);
        index = (it)->_vtable->index(INTEFACE_ADJUST(it));
        if(index == old_index)
        {
            (it)->_vtable->nextChar(INTEFACE_ADJUST(it)); //goto next charactor and draw it
            index ++;
        }
        total_count -= (index - old_index);
    }
    
    draw_count = index - index_begin;
    *pcount_left -= draw_count;
    return draw_count;
}

//return the x pos of last line
static int textlayout_drawlines(mTextLayout* self, HDC hdc, 
                int x_begin,int x, int *py, 
                mCommBTreeLeafIterator* pnode_it, ITextIterator* it,
                int first_line_left_count, int *plast_line_left_count, 
                int max_selection, int bottom, BOOL bsel)
{
    int left_count = first_line_left_count;
    if (max_selection == 0) {
        *plast_line_left_count = first_line_left_count;
        return x_begin;
    }

    if(!pnode_it->current)
    {
        *plast_line_left_count = 0;
        return x_begin;
    }

    while(*py < bottom && !(it)->_vtable->isEnd(INTEFACE_ADJUST(it)))
    {
        if (left_count == 0 && !_c(pnode_it)->hasNext(pnode_it) 
                && pnode_it->current 
                && ((mTextLayoutNode*)pnode_it->current)->height > 0) {
            //only last node is empty node possiblely, 
            //we should draw it's background.
            textlayout_drawline(self, hdc, &x, *py, 
                    (mTextLayoutNode*)pnode_it->current, 
                    &left_count, it, max_selection, bsel);
        }

        while(left_count > 0 && !(it)->_vtable->isEnd(INTEFACE_ADJUST(it)))
        {
            textlayout_drawline(self, hdc, &x, *py, 
                    (mTextLayoutNode*)pnode_it->current, 
                    &left_count, it, max_selection, bsel);
            if(max_selection > 0 && (it)->_vtable->index(INTEFACE_ADJUST(it)) >= max_selection)
            {
                if(left_count <= 0)
                {
                    *py += ((mTextLayoutNode*)pnode_it->current)->height;
                    _c(pnode_it)->next(pnode_it);
                    if(pnode_it->current)
                        *plast_line_left_count = 
                            ((mTextLayoutNode*)pnode_it->current)->str_count;
                    else
                        *plast_line_left_count = 0;
                    return x_begin;
                }
                *plast_line_left_count = left_count;
                return x;
            }
        }
        *py += ((mTextLayoutNode*)pnode_it->current)->height;
        x = x_begin;
        _c(pnode_it)->next(pnode_it);
        if(!pnode_it->current)
            break;
        left_count = ((mTextLayoutNode*)pnode_it->current)->str_count;
    }

    *plast_line_left_count = left_count;
    return x;
}


static void mTextLayout_draw(mTextLayout* self, 
        HDC hdc, int x, int y, int width, int height)
{
    mTextLayoutNode* node;
    int index;
    ITextIterator * it;
    mCommBTreeLeafIterator  node_it;
    int x_off, y_off;
    int last_left_count;

    x_off = -x;
    y_off = -y;
    
    node = get_begin_line(self, &x, &y, &index);
    if(!node)
        return;

    (void)INIT_OBJV(mCommBTreeLeafIterator, &node_it,2,self, node);

    
    it = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), index);

    if(self->sel_begin == self->cursor)
    {
        x += x_off;
        y += y_off;
        (self->context)->_vtable->setupDC(INTEFACE_ADJUST(self->context), hdc);
        textlayout_drawlines(self, hdc, x, x, &y, &node_it, it, 
                node->str_count, &last_left_count, -1, height,FALSE);
    }
    else
    {
        int sel_begin, sel_end;
        int x_sel_begin;
        x += x_off;
        y += y_off;
        if(self->sel_begin < self->cursor)
        {
            sel_begin = self->sel_begin;
            sel_end   = self->cursor;
        }
        else
        {
            sel_begin = self->cursor;
            sel_end   = self->sel_begin;
        }
        //printf("--- sel_begin=%d, sel_end=%d\n", sel_begin, sel_end);

        (self->context)->_vtable->setupDC(INTEFACE_ADJUST(self->context), hdc);
        x_sel_begin = textlayout_drawlines(self, hdc, x, x, &y, &node_it, 
                        it, node->str_count, &last_left_count, 
                        sel_begin, height, FALSE);
        if(y >= height)
            goto END;

        (self->context)->_vtable->beginSelection(INTEFACE_ADJUST(self->context), hdc);
        x_sel_begin = textlayout_drawlines(self, hdc, x, x_sel_begin, &y, 
                &node_it, it, last_left_count, &last_left_count, 
                sel_end, height, TRUE);
        (self->context)->_vtable->endSelection(INTEFACE_ADJUST(self->context), hdc);
        if(y >= height)
            goto END;

        textlayout_drawlines(self, hdc, x, x_sel_begin, &y, &node_it, it, 
                last_left_count, &last_left_count, -1, height,FALSE);

    }

END:
    (it)->_vtable->releaseIterator(INTEFACE_ADJUST(it));
}

static void 
textlayout_te_set_cursor_by_strindex(mTextLayout* self, int str_index)
{
    if(self->sel_begin != self->cursor)
    {
        textlayout_updateselection(self);
    }

    if(str_index < 0)
        self->cursor = 0;
    else {
        int count = textlayout_get_str_count(self);
        if(str_index > count)
            self->cursor = count;
        else
            self->cursor = str_index;
    }

    self->text_cursor = _c(self)->getCharPos(self, self->cursor, 
            &self->cursor_x, &self->cursor_y, &self->cursor_height, FALSE);
    
    textlayout_resetsel(self);

    (self->context)->_vtable->updateCaret(INTEFACE_ADJUST(self->context));
}

static void mTextLayout_onTextChanged(mTextLayout* self, 
        int begin, int delCount, int insertCount)
{
    textlayout_relayout(self, begin, delCount, insertCount, FALSE);
    textlayout_te_set_cursor_by_strindex(self, begin+insertCount);
}

static mCommBTreeNode* mTextLayout_newLeaf(mTextLayout *self)
{
    return (mCommBTreeNode*)NEW(mTextLayoutNode);
}

static mCommBTreeNode* mTextLayout_newNode(mTextLayout *self)
{
    return (mCommBTreeNode*)NEW(mTextLayoutNode);
}

static int mTextLayout_getLineHeight(mTextLayout* self, int textindex)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode* node;
    if(textindex < 0)
        textindex = 0;
    else if(textindex > textlayout_get_text_count(self))
        textindex = textlayout_get_text_count(self);
    
    node = search_textlayout_node_by_textindex(self, &searchInfo, 
            textindex, BTSF_RETLAST_IF_OUTOFRANGE);

    if(!node)
        return 0;

    return node->height;
}

static BOOL mTextLayout_getTextBoundSize(mTextLayout* self, int *cx, int *cy)
{
    if(cx)
        *cx = ((mTextLayoutNode*)self->rootNode)->width;
    if(cy)
        *cy = ((mTextLayoutNode*)self->rootNode)->height;
    return TRUE;
}

static int mTextLayout_textCount2Len(mTextLayout* self, int text_index, 
        int text_count, int *plen)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode *node;
    HDC hdc;
    int str_index;
    BOOL bcalcIndex = FALSE;
    ITextIterator* txtit;

    if(plen) *plen = 0;

    if(text_index <= 0)
    {
        if(!plen || text_count <= 0)
        {
            return 0;
        }
        text_index = 0;
        str_index = 0;
        goto CALC;
    }
    else
    {
        int count = textlayout_get_text_count(self);
        if(text_index >= count)
        {
            return textlayout_get_str_count(self);
        }
    }

    node = search_textlayout_node_by_textindex(self, 
            &searchInfo, text_index, BTSF_RETLAST_IF_OUTOFRANGE);

    if(!node)
        return -1;

    if(text_index == searchInfo.text_count)
    {
        if(!plen || text_count <= 0)
            return searchInfo.str_count;
        str_index = searchInfo.str_count;
    }

    if(text_index == (searchInfo.text_count + node->text_count))
    {
        if(!plen || text_count <= 0)
            return searchInfo.str_count + node->str_count;
        str_index = searchInfo.str_count + node->str_count;
    }

    if(node->str_count == node->text_count)
    {
        if(!plen 
            ||text_count<=(node->text_count+searchInfo.text_count-text_index))
        {
            if(plen)
                *plen = text_count;
            return searchInfo.str_count + (text_index - searchInfo.text_count);
        }
        str_index = searchInfo.str_count + (text_index - searchInfo.text_count);
    }
    else
    {
        str_index = searchInfo.str_count;
        bcalcIndex = TRUE;
    }

CALC:

    hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));

    txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), str_index);

    if(bcalcIndex)
    {
        str_index += textlayout_get_text_byte_length(txtit, hdc, 
                text_index - searchInfo.text_count);
        (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), str_index);
    }

    if(plen && text_count > 0)
        *plen = textlayout_get_text_byte_length(txtit, hdc, text_count);

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);
    return str_index;
}

static int mTextLayout_textLen2Count(mTextLayout* self, int str_index, 
        int str_len, int *ptext_count, int *poutstr_index, int *poutstr_count)
{
    mTextLayoutNodeSearchInfo searchInfo;
    mTextLayoutNode *node;
    HDC hdc;
    int text_index;
    BOOL bcalcIndex = FALSE;
    ITextIterator* txtit;

    if(ptext_count) *ptext_count = 0;
    if (poutstr_index) *poutstr_index = str_index;
    if (poutstr_count) *poutstr_count = 0;

    if(str_index <= 0)
    {
        if(!(ptext_count || poutstr_count) || str_len <= 0)
            return 0;
        str_index = 0;
        if (poutstr_index) *poutstr_index = 0;
        text_index = 0;
        goto CALC;
    }
    else
    {
        int count = textlayout_get_str_count(self);
        if(str_index >= count) {
            if (poutstr_index) *poutstr_index = count;
            return textlayout_get_text_count(self);
        }
    }

    node = search_textlayout_node_by_strindex(self, &searchInfo, 
                        str_index, BTSF_RETLAST_IF_OUTOFRANGE);

    if(!node)
        return -1;

    if(str_index == searchInfo.str_count)
    {
        if(!(ptext_count || poutstr_count) || str_len <= 0)
            return searchInfo.text_count;
        text_index = searchInfo.text_count;
    }

    if(str_index == (searchInfo.str_count + node->str_count))
    {
        if(!(ptext_count || poutstr_count) || str_len <= 0)
            return searchInfo.text_count + node->text_count;

        text_index = searchInfo.text_count + node->text_count;
    }

    if(node->str_count == node->text_count) {
        //no need output length information or information in this range.
        if(!(ptext_count || poutstr_count) 
            || str_len <= (searchInfo.str_count + node->str_count - str_index ))
        {
            if(ptext_count) *ptext_count = str_len;
            if (poutstr_count) *poutstr_count = str_len;
            return searchInfo.text_count + (str_index - searchInfo.str_count);
        }

        text_index = searchInfo.text_count + (str_index - searchInfo.str_count);
    }
    else {
        text_index = searchInfo.text_count;
        bcalcIndex = TRUE;
    }

CALC:

    hdc = (self->context)->_vtable->getDC(INTEFACE_ADJUST(self->context));
    if(bcalcIndex) {
        int len;
        txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), searchInfo.str_count);
        text_index += textlayout_get_text_count_by_length(txtit, hdc, 
                            str_index - searchInfo.str_count, &len);

        if (poutstr_index) *poutstr_index = searchInfo.str_count + len;
    }
    else {
        txtit = (self->text_buffer)->_vtable->getAt(INTEFACE_ADJUST(self->text_buffer), str_index);
    }

    //calc text count
    if((ptext_count || poutstr_count) && str_len > 0) {
        int count = textlayout_get_text_count_by_length(txtit, 
                        hdc, str_len, poutstr_count);
        if (ptext_count) *ptext_count = count;
    }

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    (self->context)->_vtable->releaseDC(INTEFACE_ADJUST(self->context), hdc);
    return text_index;
}

static void mTextLayout_foreachLine(mTextLayout* self, int x, int y, int maxWidth, int maxHeight, 
        CB_LINE_PROC proc, mObject* proc_owern, void* user_param)
{
    mTextLayoutNode* node;
    mCommBTreeLeafIterator  node_it;
    int  xline, yline;
    int  wline, hline;
    int  bottom;
    int  index = 0;
    int lineNo = 0;
    int cx, cy;
    if(!proc)
        return ;

    xline = x;
    yline = y;

    bottom = y + maxHeight;

    node = get_begin_line(self, &xline, &yline, &index);
    _c(self)->getTextBoundSize(self, &cx, &cy);
    //printf("--- x=%d, y=%d, xline=%d, yline=%d, cx=%d,cy=%d\n",x, y,xline, yline,cx,cy);
    lineNo = _c(self)->getParaByIndex(self, index, NULL, FALSE);
    if(!node)
    {
        node = textlayout_get_last_node(self, FALSE);
    }

    if(!node || node->height <= 0) //
    {
        xline = 0;
        wline = maxWidth;
        hline = _I(self->context)->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_DEFLINEHEIGHT);
        yline = (y / hline) * hline;
        lineNo = -1; //no lines

        goto DEFAULT_LINE;
    }

    
    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, node);

    xline = 0;

    while(node_it.current && yline < bottom)
    {
        node = (mTextLayoutNode*)(node_it.current);
        proc(proc_owern, xline, yline, node->width,  node->height, lineNo, user_param);
        yline += node->height;
        lineNo ++;
    }

    if(yline >= bottom)
        return;

    index = -1;

    wline = maxWidth;
    hline = _I(self->context)->getMetrics(INTEFACE_ADJUST(self->context), TEXTMETRICS_DEFLINEHEIGHT);

DEFAULT_LINE:

    while( yline < bottom)
    {
        proc(proc_owern, xline, yline, wline, hline, -1, user_param);
        yline += hline;
    }
}

static void mTextLayout_travelNode(mTextLayout* self, CB_NODE_PROC proc, mObject* proc_owern, void* user_param)
{
}

static PLOGFONT mTextLayout_getReleatedFont(mTextLayout* self, int idx, BOOL isText)
{
    return NULL;
}

static int mTextLayout_getParaByIndex(mTextLayout* self, int index, int *pcol, BOOL bText)
{
    int idx = 0;
    int line_idx = 0;
    int para_idx = 0;

    mTextLayoutNode* node = textlayout_get_first_node(self);
    mCommBTreeLeafIterator node_it;
    ITextIterator *txtit;

    if(!node)
        return 0;

    if(index <= 0)
    {
        if(pcol)
            *pcol = 0;
        return 0;
    }

    txtit = _I(self->text_buffer)->getAt(INTEFACE_ADJUST(self->text_buffer), 0);

    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, node);

    if(bText)
    {
        int str_idx = 0;
        while((node = (mTextLayoutNode*)node_it.current) && (idx + node->text_count) < index)
        {
            idx += node->text_count;
            str_idx += node->str_count;
            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx-1) == TI_LINERETURN)
            {
                para_idx ++;
                line_idx = idx;
            }
            _c(&node_it)->next(&node_it);
        }

        if(node && (idx + node->text_count) == index
                && (_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx + node->str_count - 1) == TI_LINERETURN))
        {
            idx += node->text_count;
            line_idx = idx;
            para_idx ++;
        }

    }
    else
    {
        while((node = (mTextLayoutNode*)node_it.current) && (idx + node->str_count) < index)
        {
            node = (mTextLayoutNode*)(node_it.current);
            idx += node->str_count;
            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), idx - 1) == TI_LINERETURN)
            {
                para_idx ++;
                line_idx = idx;
            }
            _c(&node_it)->next(&node_it);
        }

        if(node && (idx + node->str_count) == index
                && (_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), idx + node->str_count - 1) == TI_LINERETURN))
        {
            idx += node->str_count;
            line_idx = idx;
            para_idx ++;
        }

    }

    if(pcol)
        *pcol = index - line_idx;

    _I(txtit)->releaseIterator(INTEFACE_ADJUST(txtit));

    return para_idx;
}

/* Internal interface: the index is contain the title context. */
static int mTextLayout_getLineByIndex(mTextLayout* self, int index, int *pcol, BOOL bText)
{
    int idx = 0;
    int line_idx = 0;
    int str_idx = 0;
    int count = 0;

    mTextLayoutNode* node = textlayout_get_first_node(self);
    mCommBTreeLeafIterator node_it;
    ITextIterator *txtit;
    //mTextEditor* obj_texteditor = (mTextEditor*)(INTEFACE_ADJUST(self->context));
    
    if(!node)
        return 0;

    if(index <= 0) 
    {
        if(pcol)
            *pcol = 0;
        return 0;
    }

    txtit = _I(self->text_buffer)->getAt(INTEFACE_ADJUST(self->text_buffer), 0);

    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, node);

    while(1) 
    {
        node = (mTextLayoutNode*)node_it.current;
        if(!node)
            break;

        if(bText)
           count = node->text_count;
        else
           count = node->str_count; 

        if(idx + count >= index)
            break;

        str_idx += node->str_count;
        idx += count;
        line_idx ++;

        _c(&node_it)->next(&node_it);
    }

    if(node && (idx + count) == index
            && (_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx + node->str_count - 1) == TI_LINERETURN))
    {
        idx += count;
        line_idx ++;
    }

    if(pcol)
        *pcol = index - idx;

    _I(txtit)->releaseIterator(INTEFACE_ADJUST(txtit));
    
    return line_idx;
}

static int mTextLayout_getIndexByPara(mTextLayout* self, int paraNo, int col, BOOL* outOfRange, BOOL bText)
{
    int para_idx = 0, str_idx = 0;
    int idx = 0, count = 0;
    int col_idx = 0, col_para_idx = 0, col_str_idx = 0;

    mTextLayoutNode* node;
    mCommBTreeLeafIterator node_it;
    ITextIterator *txtit;

    if(paraNo <= 0 && col <= 0)
    {
        if (paraNo < 0 || col < 0)
            SMART_ASSIGN(outOfRange, TRUE);
        return 0;
    }

    node = textlayout_get_first_node(self);

    txtit = _I(self->text_buffer)->getAt(INTEFACE_ADJUST(self->text_buffer), 0);

    (void)initObjectArgs((mObject*)((void*)(&node_it)), (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, self, node);

    /* IMPORTANT: make the node has the correct value when we jump out the while loop.
     * more loop once to calc the para length to compare the col and the para length */
    while((node = (mTextLayoutNode*)node_it.current) && col_para_idx < paraNo + 1)
    {
        if (para_idx < paraNo)
        {
            str_idx += node->str_count;
            idx += node->text_count;
            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx - 1) == TI_LINERETURN)
                para_idx ++;
        }

        col_str_idx += node->str_count;
        col_idx += node->text_count;
        if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), col_str_idx - 1) == TI_LINERETURN)
            col_para_idx ++;
        
        _c(&node_it)->next(&node_it);
    }

    if(para_idx < paraNo)
    {
        SMART_ASSIGN(outOfRange, TRUE);
        idx = _I(self->text_buffer)->getCount(INTEFACE_ADJUST(self->text_buffer));
    }
    else
    {
        if (bText)
            count = col_idx - idx;
        else
            count = col_str_idx - str_idx;

        if (col >= count)
        {
            idx = col_idx;
            str_idx = col_str_idx;

            if (col > count)
                SMART_ASSIGN(outOfRange, TRUE);

            // ignore the '\n' 
            if (_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx - 1) == TI_LINERETURN)
            {
                SMART_ASSIGN(outOfRange, TRUE);
                idx -= 1;
                str_idx -= 1;
            }
        }
        else
        {
            idx += col;
            str_idx += col;
        }
    }

    _I(txtit)->releaseIterator(INTEFACE_ADJUST(txtit));

    if (bText)
        return idx;
    else
        return str_idx;
}

/* Internal interface: the index is contain the title context. */
static int mTextLayout_getIndexByLine(mTextLayout* self, int lineIdx, int col, BOOL* outOfRange, BOOL bText)
{
    int index = 0, str_idx = 0; 
    int i = 0, count = 0;
    BOOL found = FALSE;

    mCommBTreeLeafIterator iter;
    mTextLayoutNode *node = NULL;
    mTextEditor* obj_texteditor = (mTextEditor*)(INTEFACE_ADJUST(self->context));

    (void)INIT_OBJV(mCommBTreeLeafIterator, &iter, 2, self, node);
    node = (mTextLayoutNode*)_c(&iter)->getCurrent(&iter);

    if (lineIdx <= 0 && col <= 0)
    {
        if (lineIdx < 0 || col < 0)
            SMART_ASSIGN(outOfRange, TRUE);
        return 0;
    }

    while (node)
    {
        if (lineIdx == i) 
        {
            found = TRUE;
            break; 
        }

        index += node->text_count;
        str_idx += node->str_count;
        i++;

        node = (mTextLayoutNode*)_c(&iter)->next(&iter);
    }

    if (found) 
    {
        ITextIterator* txtit = _c(obj_texteditor->textBuffer)->getAt(obj_texteditor->textBuffer, 0);

        if (bText)
            count = node->text_count;
        else
            count = node->str_count;

        // the charPos has more than the total char of the line
        if (col >= count)
        {
            index += node->text_count;
            str_idx += node->str_count;

            if (col > count)
                SMART_ASSIGN(outOfRange, TRUE);

            // ignore the '\n' 
            if (_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx - 1) == TI_LINERETURN)
            {
                SMART_ASSIGN(outOfRange, TRUE);
                index -= 1;
                str_idx -= 1;
            }
        }
        else
        {
            index += col;
            str_idx += col;
        }

        _I(txtit)->releaseIterator(INTEFACE_ADJUST(txtit));
    }
    else // the lineIdx has more than the total line number
    {
        SMART_ASSIGN(outOfRange, TRUE);
        index = _c(obj_texteditor->textBuffer)->getCount(obj_texteditor->textBuffer);
        str_idx = index;
    }

    if (bText)
        return index;
    else
        return str_idx;
}

BEGIN_MINI_CLASS(mTextLayout, mCommBTree)
    IMPLEMENT(mTextLayout, ITextLayout)
    CLASS_METHOD_MAP(mTextLayout, construct)
    CLASS_METHOD_MAP(mTextLayout, destroy)
    CLASS_METHOD_MAP(mTextLayout, getCharIndex)
    CLASS_METHOD_MAP(mTextLayout, getCharPos)
    CLASS_METHOD_MAP(mTextLayout, setCursor)
    CLASS_METHOD_MAP(mTextLayout, getCursor)
    CLASS_METHOD_MAP(mTextLayout, setCursorPos)
    CLASS_METHOD_MAP(mTextLayout, getLineHeight)
    CLASS_METHOD_MAP(mTextLayout, draw)
    CLASS_METHOD_MAP(mTextLayout, newNode)
    CLASS_METHOD_MAP(mTextLayout, newLeaf)
    CLASS_METHOD_MAP(mTextLayout, onTextChanged)
    CLASS_METHOD_MAP(mTextLayout, getTextBoundSize)
    CLASS_METHOD_MAP(mTextLayout, reset)
    CLASS_METHOD_MAP(mTextLayout, textLen2Count)
    CLASS_METHOD_MAP(mTextLayout, textCount2Len)
    CLASS_METHOD_MAP(mTextLayout, foreachLine)
    CLASS_METHOD_MAP(mTextLayout, travelNode)
    CLASS_METHOD_MAP(mTextLayout, getReleatedFont)
    CLASS_METHOD_MAP(mTextLayout, getParaByIndex)
    CLASS_METHOD_MAP(mTextLayout, getLineByIndex)
    CLASS_METHOD_MAP(mTextLayout, getIndexByPara)
    CLASS_METHOD_MAP(mTextLayout, getIndexByLine)
END_MINI_CLASS


/////////////////////////////////////////////////////////
//mTextRender
static int mTextRender_calc(mTextRender* self, HDC hdc, 
        ITextIterator* begin, int str_count, int *ptext_count, 
        const RECT* maxBound, RECT *bound)
{
    int rel_count;
    int visible_str_count;
    SIZE size;
    const char* text;
    if(!begin)
        return 0;
    
    text = (begin)->_vtable->getMore(INTEFACE_ADJUST(begin), &str_count, &visible_str_count, FALSE);
    rel_count = GetTabbedTextExtentPoint(hdc, text, visible_str_count,
            maxBound ? RECTWP(maxBound) : 0,
            ptext_count,
            NULL, NULL,
            &size);
    if(bound)
    {
        bound->left = 0;
        bound->right = size.cx;
        bound->top = 0;
        bound->bottom = size.cy;
    }
    if(rel_count == visible_str_count)
    {
        rel_count = str_count;
        if(ptext_count && visible_str_count < str_count)
            *ptext_count += (str_count - visible_str_count);
    }
    return rel_count;
}

static int mTextRender_draw(mTextRender* self, HDC hdc, 
        int x, int y, ITextIterator* text, int str_count)
{
    int visible_str_count;
    const char* str_text = 
        (text)->_vtable->getMore(INTEFACE_ADJUST(text), &str_count, &visible_str_count, TRUE);
    //usleep(100000); //to make it slowed
    return TabbedTextOutLen(hdc, x, y, str_text, visible_str_count);
}

static int mTextRender_getTextByteLen(mTextRender* self, 
        HDC hdc, ITextIterator* it, int *ptext_count)
{
    int text_count;
    int total_len = -1;
    int len = 0;
    const char* str;
    PLOGFONT plogfont;
    if(!ptext_count || !it || hdc == (HDC)0)
        return 0;

    text_count = *ptext_count;

    str = (it)->_vtable->getMore(INTEFACE_ADJUST(it), &total_len, NULL, FALSE);

    if(!str)
        return 0;
    
    plogfont = GetCurFont(hdc);

    while(text_count > 0)
    {
        len += get_first_char_len(plogfont, str + len, total_len - len);
        text_count --;
    }

    return len;

}

static int mTextRender_getTextCount(mTextRender* self, 
        HDC hdc, ITextIterator* it, int *pstr_len)
{
    int     text_count = 0;
    int     all_len;
    int     first_len = 0, len = 0;
    const char  *str;
    PLOGFONT    plogfont;

    if(!pstr_len || !it || hdc == (HDC)0)
        return 0;

    all_len = *pstr_len;
    str = (it)->_vtable->getMore(INTEFACE_ADJUST(it), &all_len, NULL, FALSE);

    if(!str)
        return 0;

    plogfont = GetCurFont(hdc);
    
    while (len < all_len) {
        first_len = get_first_char_len(plogfont, str + len, 4);

        //if failure or overflow, we should delete current character.
        if (first_len <= 0 || len + first_len > all_len) 
            break;

        text_count ++;
        len += first_len;
    }
    *pstr_len = len;
    return text_count;
}

static int mTextRender_getCaretSize(mTextRender* self, HDC hdc, ITextIterator* it, int *pwidth, int *pheight)
{
    int len = 0;
    int char_type;
    PLOGFONT     plogfont ;
    FONTMETRICS  font_metrics;
    if(!pwidth && !pheight)
        return 1; //have caret size

    char_type = (it)->_vtable->getCharType(INTEFACE_ADJUST(it), 0);
    if(char_type == TI_VISIBILE_CHAR)
    {
        int txt_count = 1;
        len = _c(self)->getTextByteLen(self, hdc, it, &txt_count);
    }

    plogfont = GetCurFont(hdc);
    GetFontMetrics(plogfont, &font_metrics);
    if(pwidth) 
        *pwidth = font_metrics.ave_width;
    if(pheight)
        *pheight = font_metrics.font_height;

    if(len != 0)
    {
        int visible_len;
        const char* text = (it)->_vtable->getMore(INTEFACE_ADJUST(it), &len, &visible_len, FALSE);
        if(text && visible_len > 0)
        {
            SIZE size;
            len = GetTabbedTextExtentPoint(hdc, text, visible_len, 0x7FFFFFFF, NULL, NULL,NULL, &size);
            if(len > 0)
            {
                if(pwidth)
                    *pwidth = size.cx;
                if(pheight)
                {
                    if(*pheight < size.cy)
                        *pheight = size.cy;
                }
            }
        }
        else
            len = 0;
    }


    return 1;
}

BEGIN_MINI_CLASS(mTextRender, mObject)
    IMPLEMENT(mTextRender, ITextRender)
    CLASS_METHOD_MAP(mTextRender, calc)
    CLASS_METHOD_MAP(mTextRender, draw)
    CLASS_METHOD_MAP(mTextRender, getTextByteLen)
    CLASS_METHOD_MAP(mTextRender, getTextCount)
    CLASS_METHOD_MAP(mTextRender, getCaretSize)

    //init the g_textRender
    (void)INIT_OBJ(mTextRender, &g_textRender);
END_MINI_CLASS

//////////////////////////////////////////////////////////////
/*internal flags for textedit*/
enum {
    NCS_TEF_NOCARET = 0x0001,
    NCS_TEF_FOCUS   = 0x0002,
    NCS_TEF_CONTCHANGED = 0x0004,
    NCS_TEF_CURSORPAGEMOVE = 0x8,
};

#define pTextBuffer self->textBuffer
#define pTextLayout self->textLayout

#define TE_VALID_OBJ(self)  (self && pTextLayout && pTextBuffer)
#define TE_IS_FOCUS(self)  (self->flags & NCS_TEF_FOCUS)
#define TE_IS_CURSORE_PAGE_MOVE(self)   (self->flags & NCS_TEF_CURSORPAGEMOVE)
#define TE_SET_CURSORE_PAGE_MOVE(self)  (self->flags |= NCS_TEF_CURSORPAGEMOVE)
#define TE_CLEAR_CUSROR_PAGE_MOVE(self) (self->flags &= ~NCS_TEF_CURSORPAGEMOVE)

//TE_SEL_CHANGING and TE_SEL_CHANGED is used simuteously.
#define TE_SEL_CHANGING(self)\
    int old_txt_selidx, old_txt_cursoridx;\
    old_txt_selidx = _c(pTextLayout)->getCursor(pTextLayout, NULL, NULL, NULL, TRUE);\
    old_txt_cursoridx = _c(pTextLayout)->getCursor(pTextLayout, NULL, NULL, NULL, FALSE);    

#define TE_SEL_CHANGED(self, isSel)\
do{ \
    if (!isSel) {\
        if (old_txt_selidx != old_txt_cursoridx)/*cancel selection*/\
            ncsNotifyParent (self, NCSN_TE_SELCHANGED);\
    }\
    else {\
        int new_txt_selidx = _c(pTextLayout)->getCursor(pTextLayout, NULL, NULL, NULL,TRUE);\
        if (old_txt_selidx != new_txt_selidx)   /*begin or change seleciton*/  \
            ncsNotifyParent (self, NCSN_TE_SELCHANGED);\
    }\
}while(0)

/* update title */
#ifdef _MGHAVE_TEXTEDITTITLE
static void texteditor_update_title_line(mTextEditor* self)
{
    int i;
    mCommBTreeLeafIterator node_it;
    mTextLayoutNode* node;

    self->title_linecount = _c(pTextLayout)->getLineByIndex(pTextLayout, 
            self->title_text_idx, &self->title_last_line_text_index, TRUE);
    
    /* FixMe: this determine depend text.
     * deal with when the title last line just autowrap. */
    if(self->title_last_line_text_index != 0)
    {
        node = textlayout_get_first_node(pTextLayout);
        (void)initObjectArgs((mObject*)((void*)(&node_it)), 
                (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, pTextLayout, node);
        
        for (i = 0; i <= self->title_linecount; i++)
        {
            node = (mTextLayoutNode*)node_it.current;
            _c(&node_it)->next(&node_it);
        }
        
        if(node->text_count == self->title_last_line_text_index) 
        {
            self->title_linecount ++;
            self->title_last_line_text_index = 0;
        }
    }
}

static void texteditor_update_title(mTextEditor* self)
{
    if((GetWindowStyle(self->hwnd)& NCSS_TE_TITLE) && self->title_idx > 0)
    {
        self->title_text_idx = _c(pTextLayout)->getCharPos(pTextLayout, self->title_idx,
            &self->title_x, &self->title_y, &self->title_last_height, FALSE);
        self->title_paracount = _c(pTextLayout)->getParaByIndex(pTextLayout, self->title_text_idx, &self->title_last_para_text_index, TRUE);

        /* when have any text, the title line content can't calc correct.
         * call this function to recalc title line content correct, when you need the title line content. */
        texteditor_update_title_line(self);
    }
    else{
        self->title_text_idx = -1;
        self->title_x = self->title_y = self->title_last_height = -1;
        self->title_paracount = self->title_last_para_text_index = -1;
        self->title_linecount = self->title_last_line_text_index = -1;
    }
}
#else
#define texteditor_update_title(self)
#define texteditor_update_title_line(mTextEditor* self) 
#endif


//margins
static inline void texteditor_get_view_rect(mTextEditor* self, RECT *prc)
{
    if(!prc)
        return ;
    
    GetClientRect(self->hwnd, prc);
    
    prc->left  += self->margins.left;
    prc->right -= (self->margins.right/*- (self->margins.right>0?1:0)*/);
    prc->top   += self->margins.top;
    prc->bottom -= (self->margins.bottom/*-(self->margins.bottom>0?1:0)*/);

}

static inline void texteditor_view_to_client(mTextEditor* self, int *px, int *py)
{
    if(px)
        *px += self->margins.left;
    if(py)
        *py += self->margins.top;
}

static inline void texteditor_client_to_view(mTextEditor *self, int *px, int *py)
{
    if(px)
        *px -= self->margins.left;
    if(py)
        *py -= self->margins.top;
}

static inline void texteditor_rect_view_to_client(mTextEditor* self, RECT *prc)
{
    OffsetRect(prc, self->margins.left, self->margins.top);
}

static inline void texteditor_rect_client_to_view(mTextEditor* self, RECT *prc)
{
    OffsetRect(prc, -self->margins.left, self->margins.top);
}

#ifdef _MGHAVE_TEXTEDITTITLE
static void texteditor_add_title_content(mTextEditor* self, int* idx, int* offset, BOOL bLine)
{
    if (bLine)
    {
        texteditor_update_title_line(self);

        if (self->title_idx > 0)
        {
            if (0 == *idx)
                *offset += self->title_last_line_text_index;
            *idx += self->title_linecount;
        }
    }
    else
    {
        if (self->title_idx > 0)
        {
            if (0 == *idx)
                *offset += self->title_last_para_text_index;
            *idx += self->title_paracount;
        }
    }
}
#else
#define texteditor_add_title_content(mTextEditor* self, int* idx, int* offset, BOOL bLine) 
#endif

static  void _update_scrollbar(mTextEditor *self)
{
    SCROLLINFO si;
    int cx, cy;
    DWORD style;
    if (!TE_VALID_OBJ(self))
        return;

    if (self->visWidth <= 0 || self->visHeight <= 0)
        return ;

    style = GetWindowStyle(self->hwnd);
    if (self->scrollBarMode == NCS_TEF_SCROLLBAR_NEVER) {
        ShowScrollBar(self->hwnd, SB_VERT, FALSE);
        ShowScrollBar(self->hwnd, SB_HORZ, FALSE);
        return;
    }

    _c(pTextLayout)->getTextBoundSize(pTextLayout, &cx, &cy);

    //vertical scrollbar
    if (cy <= self->visHeight) {
        SetScrollPos(self->hwnd, SB_VERT, 0);
        if (self->scrollBarMode == NCS_TEF_SCROLLBAR_AUTO) { /* Auto */
            ShowScrollBar (self->hwnd, SB_VERT, FALSE);
        } else if (self->scrollBarMode == NCS_TEF_SCROLLBAR_ALWAYS) { /* Always */
            EnableScrollBar(self->hwnd, SB_VERT, TRUE);

            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMax = 1;
            si.nMin = 0;
            si.nPage = 0;
            SetScrollInfo (self->hwnd, SB_VERT, &si, TRUE);
            ShowScrollBar (self->hwnd, SB_VERT, TRUE);
        }
    }
    else {
        IncludeWindowStyle(self->hwnd, WS_VSCROLL);

        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = cy - 1;
        si.nPage = self->visHeight;
        si.nPos = self->offY;

        SetScrollInfo (self->hwnd, SB_VERT, &si, TRUE);

        EnableScrollBar(self->hwnd, SB_VERT, TRUE);
        ShowScrollBar (self->hwnd, SB_VERT, TRUE);
    }

    //horz scrollbar
    if(style & NCSS_TE_AUTOWRAP)
    {
        SetScrollPos(self->hwnd, SB_HORZ, 0);
        if (self->scrollBarMode == NCS_TEF_SCROLLBAR_AUTO) { /* Auto */
            ShowScrollBar (self->hwnd, SB_HORZ, FALSE);
        } else if (self->scrollBarMode == NCS_TEF_SCROLLBAR_ALWAYS) { /* Always */
            EnableScrollBar(self->hwnd, SB_HORZ, FALSE);

            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMax = 1;
            si.nMin = 0;
            si.nPage = 0;
            SetScrollInfo (self->hwnd, SB_HORZ, &si, TRUE);
            ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
        }
    }
    else
    {
        if (cx <= self->visWidth) {
            SetScrollPos(self->hwnd, SB_HORZ, 0);
            if (self->scrollBarMode == NCS_TEF_SCROLLBAR_AUTO) { /* Auto */
                ShowScrollBar (self->hwnd, SB_HORZ, FALSE);
            } else if (self->scrollBarMode == NCS_TEF_SCROLLBAR_ALWAYS) { /* Always */
                EnableScrollBar(self->hwnd, SB_HORZ, TRUE);

                si.fMask = SIF_RANGE | SIF_PAGE;
                si.nMax = 1;
                si.nMin = 0;
                si.nPage = 0;
                SetScrollInfo (self->hwnd, SB_HORZ, &si, TRUE);
                ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
            }
        }
        else {
            IncludeWindowStyle(self->hwnd, WS_HSCROLL);

            si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
            si.nMin = 0;
            si.nMax = cx - 1;
            si.nPage = self->visWidth;
            si.nPos = self->offX;

            SetScrollInfo (self->hwnd, SB_HORZ, &si, TRUE);

            EnableScrollBar(self->hwnd, SB_HORZ, TRUE);
            ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
        }
    }
}

static BOOL texteditor_must_update_bkgnd(mTextEditor* self)
{
    return (GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT) /*|| use bitmap as bkgound  */
#ifdef __MGNCS_LIB__
        || (GetWindowStyle(self->hwnd) & NCSS_TE_OWNERBKGND)
        || self->bkimg.img.pbmp
        || ncsGetComponentHandler((mComponent*)self, MSG_ERASEBKGND)
#else
        || SendMessage(self->hwnd, EM_MUSTUPDATEBKGND, 0, 0)
#endif
        ;
}

static void _scroll_invalidate_window(mTextEditor* self, int offX, int offY)
{
    RECT rcScrolled;

    //make the the progress goto onPaint
    if(texteditor_must_update_bkgnd(self))
    {
        if(offX != self->offX && offY != self->offY)
        {
            InvalidateRect(self->hwnd, NULL, TRUE);
            return ;
        }
        else 
        {
            RECT rc;
            texteditor_get_view_rect(self, &rc);
            InvalidateRect(self->hwnd, &rc, TRUE);
        }
    }
    else
    {
        //update the area must be update

        texteditor_get_view_rect(self, &rcScrolled);

#if 1
        //printf("---- update scrollbar:self->offX=%d,offX=%d,self->offY=%d,offY=%d\n", self->offX, offX, self->offY, offY);
        ScrollWindow(self->hwnd, self->offX - offX, self->offY - offY, &rcScrolled, NULL);
#else
        rc = rcScrolled;
        if(offX != self->offX)
        {
            if(offX > self->offX)
                rcScrolled.left = rcScrolled.right - offX + self->offX - 1;
            else
                rcScrolled.right = self->offX - offX + rcScrolled.left + 1;
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);

        }
        
        if(offY != self->offY)
        {
            rcScrolled = rc;
            if(offY > self->offY)
                rcScrolled.top = rcScrolled.bottom - offY + self->offY - 1;
            else
                rcScrolled.bottom = self->offY - offY + rcScrolled.top + 1;
            //printf("--- rcScrollend = %d,%d,%d,%d\n", rcScrolled.left, rcScrolled.top, rcScrolled.right, rcScrolled.bottom);
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);
        }
#endif
    }

}

static void _update_scrolled_margins(mTextEditor* self, int offX, int offY)
{
    RECT rcScrolled;
    RECT rc;

    if(texteditor_must_update_bkgnd(self))
        return ;

    GetClientRect(self->hwnd, &rc);
    //make the margins invalidate, because i don't known what and how it will draw
    if(offX != self->offX)
    {
        rcScrolled = rc;
        //update the top && bottom margins too
        if(self->margins.top > 0)
        {
            rcScrolled.top = rc.top;
            rcScrolled.bottom = self->margins.top;
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);
        }
        if(self->margins.bottom > 0)
        {
            rcScrolled.top = rc.bottom - self->margins.bottom;
            rcScrolled.bottom = rc.bottom;
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);
        }
    }
    
    if(offY != self->offY)
    {
        rcScrolled = rc;
       //update the left && right margins
        if(self->margins.left > 0)
        {
            rcScrolled.left = rc.left;
            rcScrolled.right = self->margins.left;
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);
        }
        if(self->margins.right > 0)
        {
            rcScrolled.left = rc.right - self->margins.right;
            rcScrolled.right = rc.right;
            InvalidateRect(self->hwnd, &rcScrolled, TRUE);
        }
    }
}

static void _update_content_view(mTextEditor* self, int width, int height, BOOL bresetContent)
{
    FONTMETRICS fm;

    if (!TE_VALID_OBJ(self))
        return;

    if(width == -1 || height == -1)
    {
        RECT rc;
        GetClientRect(self->hwnd, &rc);
        width  = RECTW(rc);
        height = RECTH(rc);
    }

    GetFontMetrics(GetWindowFont(self->hwnd), &fm);

    width -= (self->margins.left + self->margins.right);
    height -= (self->margins.top + self->margins.bottom);

    if(width <= fm.max_width)
        width = fm.max_width + 1;
    if(height <= fm.font_height)
        height = fm.font_height + 1;

    //printf("--- update content view: width=%d,height=%d, self->visWidth=%d, self->visHeight=%d\n", width, height, self->visWidth, self->visHeight);
    if(!bresetContent && self->visWidth == width && self->visHeight == height)
        return;

    self->visWidth = width;
    self->visHeight = height;

    if ((bresetContent || (GetWindowStyle(self->hwnd)&NCSS_TE_AUTOWRAP) ) && _c(pTextBuffer)->getCount(pTextBuffer) > 0 ) {
        //printf("--- update content view\n");
        _c(pTextLayout)->reset(pTextLayout);

        texteditor_update_title(self);
    }
    else
    {
        _update_scrollbar(self);
    }

    _c(self)->updateCaret(self);

}


static void _update_caret_size(mTextEditor* self);
static void _change_caret_size(mTextEditor* self, BOOL show_caret);

static void _update_height_changed(mTextEditor* self, int offX, int offY)
{
    //ScrollWindow may offset the invalidate area of window, so, we must call it firstly

    if(self->textLayout->old_height != -1 
            && self->textLayout->old_height != self->textLayout->new_height)
    {
        RECT rc;
        if(texteditor_must_update_bkgnd(self)) //text layout's height changed, 
        {
            int top;
            //invalidate All changed part
            GetClientRect(self->hwnd, &rc);
            top = self->textLayout->new_height - offY;
            rc.top = top;
            InvalidateRect(self->hwnd, &rc, TRUE);
            self->textLayout->old_height = -1;
        }
        else 
        {
#if 1
            //scroll the area
            texteditor_get_view_rect(self, &rc);
            if(self->textLayout->old_height < self->textLayout->new_height)
                rc.top = self->textLayout->old_height - offY + self->margins.top;
            else
                rc.top = self->textLayout->new_height - offY + self->margins.top;
            //printf("--- scrollwindow: self->new_height=%d, self->old_height=%d\n", self->textLayout->new_height, self->textLayout->old_height);
            //printf("--- scrollwindow: rc=%d,%d,%d,%d, offx=%d, offy=%d\n",rc.left, rc.top, rc.right, rc.bottom, 0, self->textLayout->new_height - self->textLayout->old_height);
            ScrollWindow(self->hwnd, 0, self->textLayout->new_height - self->textLayout->old_height, &rc, NULL);

#endif
        }
    }

}

static void _update_height_changed_margins(mTextEditor* self)
{
    RECT rc;
    int mintop;
    int right;

    if(texteditor_must_update_bkgnd(self)) //text layout's height changed, 
        return;

    if(!(self->textLayout->old_height != -1 
            && self->textLayout->old_height != self->textLayout->new_height))
        return;

    mintop = self->textLayout->old_height;
    if(self->textLayout->old_height > self->textLayout->new_height)  //only update the area when scroll up
    {
        texteditor_get_view_rect(self, &rc);
        //update the buttom area which is not updated and margin area of same part
        rc.top = rc.bottom - self->textLayout->old_height + self->textLayout->new_height - 1;
        InvalidateRect(self->hwnd, &rc, TRUE);
        mintop = self->textLayout->new_height;
    }
    //update the left and  right margins area
    GetClientRect(self->hwnd, &rc);
    rc.top = mintop;
    right = rc.right;
    if(self->margins.left > 0)
    {
        rc.right = rc.left + self->margins.left;
        InvalidateRect(self->hwnd, &rc, TRUE);
    }
    if(self->margins.right > 0)
    {
        rc.right = right;
        rc.left = right - self->margins.right;
        InvalidateRect(self->hwnd, &rc, TRUE);
    }
}

static void _update_content_dirty_region(mTextEditor* self)
{
    RECT rc;
    PCLIPRGN prcRgn;
    if (!self || !self->textLayout)
        return;


    prcRgn = self->textLayout->dirty_area;
    if(prcRgn && !IsEmptyClipRgn(prcRgn))
    {
        PCLIPRECT prc;
        RECT rcView;
        prc = prcRgn->head;

        texteditor_get_view_rect(self,&rcView);
        while(prc)
        {
            rc = prc->rc;
            OffsetRect(&rc, -self->offX, -self->offY);
            texteditor_rect_view_to_client(self, &rc);
            if(IntersectRect(&rc, &rc, &rcView))
            {
                InvalidateRect(self->hwnd, &rc, TRUE);
            }
            prc = prc->next;
            
        }
        EmptyClipRgn(self->textLayout->dirty_area);
    }


}

static int texteditor_get_text_count(mTextEditor* self, int start_with, BOOL bText, const char* str, int len)
{
    PLOGFONT plogfont;
    int idx = 0;
    int l;
    if(!self || !str || len <= 0)
        return 0;

    plogfont = _c(pTextLayout)->getReleatedFont(pTextLayout, start_with, bText);
    if(!plogfont)
    {
        plogfont = GetWindowFont(self->hwnd);
        if(!plogfont)
            return len;
    }

    if(plogfont->mbc_devfont == NULL)
        return len;

    while(idx < len)
    {
        l = get_first_char_len(plogfont, str + idx, len - idx);
        if(l <= 0)
            break;
        idx += l;
    }
    return idx;
}

static int texteditor_get_text_length(mTextEditor* self,int start_with, BOOL bText, const char* str, int count, int total_len)
{
    PLOGFONT plogfont;
    int idx = 0;
    int l;
    if(!self || !str || total_len <= 0 || count <= 0)
        return 0;

    plogfont = _c(pTextLayout)->getReleatedFont(pTextLayout, start_with, bText);
    if(!plogfont)
    {
        plogfont = GetWindowFont(self->hwnd);
        if(!plogfont)
            return count;
    }

    while(count > 0 && total_len > 0)
    {
        l = get_first_char_len(plogfont, str + idx, total_len);
        if(l == 0)
            break;
        idx += l;
        count --;
        total_len -= l;
    }

    return idx;
}

//get the validate length of text. if the text include half-bytes of mutil-code character, we must exclude them
//return the validate length of text
static int texteditor_get_validate_text_length(mTextEditor* self, int start_with, BOOL start_with_is_text, const char* text, int len)
{
    PLOGFONT plogfont;
    int validate_len = 0;
    if(!text||len <= 1)
        return len < 0 ? 0 : len;

    plogfont = _c(pTextLayout)->getReleatedFont(pTextLayout, start_with, start_with_is_text);
    if(!plogfont)
    {
        plogfont = GetWindowFont(self->hwnd);
        if(!plogfont)
            return len;
    }

    while(validate_len < len)
    {
        int l = get_first_char_len(plogfont, text + validate_len, len - validate_len);
        if(l == 0)
            break;
        validate_len += l;
    }

    return validate_len;
}

static void str_upper_copy(char* str, const char* src, int len) {
    int i;
    if(!src || !str || len <= 0)
        return;
    for(i=0; i < len; i ++) {
        str[i] = (src[i] >= 'a' && src[i] <= 'z') ? (src[i] - 'a' + 'A') : src[i];
    }
}
static void str_lower_copy(char* str, const char* src, int len) {
    int i;
    if(!src || !str || len <= 0)
        return;
    for(i=0; i < len; i ++) {
        str[i] = (src[i] >= 'A' && src[i] <= 'Z') ? (src[i] - 'A' + 'a') : src[i];
    }
}
static char * texteditor_case_copy(mTextEditor* self, const char* text, int len)
{
    DWORD bCase;
    if(!text)
        return NULL;
    if(len <= 0)
        len = strlen(text);
    if(len == 0)
        return NULL;

    bCase = GetWindowStyle(self->hwnd) & (NCSS_TE_LOWERCASE | NCSS_TE_UPPERCASE);

    if(bCase)
    {
        char* str = FixStrAlloc(len);
        if(bCase & NCSS_TE_UPPERCASE)
            str_upper_copy(str, text, len);
        else
            str_lower_copy(str, text, len);
        return str;
    }
    else
        return (char*)text;

}


static void mTextEditor_construct(mTextEditor* self, DWORD addData)
{
    DWORD style;
    Class(mWidget).construct((mWidget*)self, addData);
    self->hardLimit     = -1;
    self->textBuffer    = NEW(mTextBuffer);
    self->scrollHStep   = 18;
    self->scrollVStep   = 18;
    self->textLayout    = NEWV(mTextLayout, 2,
                            INTERFACE_CAST(ITextBuffer, self->textBuffer),
                            INTERFACE_CAST(ITextLayoutContext, self)); 

#ifdef _MGHAVE_TEXTEDITTITLE
    self->title_idx = -1;
    self->title_text_idx = -1;
    self->title_x = self->title_y = self->title_last_height = -1;
#endif
    
    style = GetWindowStyle(self->hwnd);
    if(style & (NCSS_TE_AUTOHSCROLL | NCSS_TE_AUTOVSCROLL))
        self->scrollBarMode = NCS_TEF_SCROLLBAR_AUTO;
    else if(style & (WS_VSCROLL | WS_HSCROLL))
        self->scrollBarMode = NCS_TEF_SCROLLBAR_ALWAYS;
    else
        self->scrollBarMode = NCS_TEF_SCROLLBAR_NEVER;

    TE_CLEAR_CUSROR_PAGE_MOVE(self);

    _c(pTextBuffer)->addObserver(pTextBuffer, 
       (ITextBufferObserver*)INTERFACE_CAST(ITextLayout, pTextLayout));
}

static BOOL mTextEditor_onCreate(mTextEditor* self, LPARAM lParam)
{
    /* create caret */
    CreateCaret(self->hwnd, NULL, 1, GetWindowFont(self->hwnd)->size);
    _c(self)->setText(self, GetWindowCaption(self->hwnd));
    return TRUE;
}

static void mTextEditor_destroy(mTextEditor* self)
{
    if (pTextBuffer)
        DELETE(pTextBuffer);

    if (pTextLayout)
        DELETE(pTextLayout);

    DestroyCaret(self->hwnd);
    Class(mWidget).destroy((mWidget*)self);
}

static int mTextEditor_getMetrics(mTextEditor *self,int type)            
{
    if (self) {
        switch (type) {
            case TEXTMETRICS_MAXWIDTH:
                if(GetWindowStyle(self->hwnd)&NCSS_TE_AUTOWRAP)
                    return self->visWidth;
                else {
                    //max value, to void the signal int overflow, 
                    //we only give the half of max integer
                    return 0x7FFFFFFF/2;
                }

            case TEXTMETRICS_MAXHEIGHT:
                return self->visHeight;

            case TEXTMETRICS_LINESPACE:
                return 2;

            case TEXTMETRICS_DEFLINEHEIGHT:
                return GetWindowFont(self->hwnd)->size + 2;
        }
    }

    return 0;
}

static void mTextEditor_beginSelection(mTextEditor *self, HDC hdc)
{
    if (!self)
        return;

    SetBkMode(hdc, BM_OPAQUE);

    SetTextColor (hdc, 
        GetWindowElementPixel (self->hwnd, 
            GetWindowStyle(self->hwnd)&WS_DISABLED?
                WE_FGC_DISABLED_ITEM : WE_FGC_SELECTED_ITEM));

    if (TE_IS_FOCUS(self)) {
        SetBkColor (hdc, 
            GetWindowElementPixel(self->hwnd, WE_BGC_SELECTED_ITEM));
    }
    else {
        SetBkColor (hdc, 
            GetWindowElementPixel (self->hwnd, WE_BGC_SELECTED_LOSTFOCUS));
    }
}

static void mTextEditor_setupDC(mTextEditor *self, HDC hdc)
{
    if (self) {
        SetBkMode (hdc, BM_TRANSPARENT);
        SetBkColor (hdc, GetWindowBkColor (self->hwnd));

        SetTextColor (hdc, 
            GetWindowElementPixel (self->hwnd, 
                GetWindowStyle(self->hwnd)&WS_DISABLED?
                    WE_FGC_DISABLED_ITEM : WE_FGC_WINDOW));
    }
}

static void mTextEditor_contentPosToDCPos(mTextEditor* self, HDC hdc, int *px, int *py)
{
    texteditor_view_to_client(self, px, py);
}

static void mTextEditor_endSelection(mTextEditor *self, HDC hdc)
{
    if (self) _c(self)->setupDC(self, hdc);
}

static HDC mTextEditor_getDC(mTextEditor *self)                         
{
    return self ? GetSecondaryClientDC(self->hwnd) : HDC_INVALID; 
}

static void mTextEditor_releaseDC(mTextEditor *self, HDC hdc)            
{
    if (self) ReleaseSecondaryDC(self->hwnd, hdc);
}

static void mTextEditor_update(mTextEditor *self, BOOL bUpdateImdiate)
{
    if (!self )
        return;

    _update_scrollbar(self);

    if(bUpdateImdiate)
    {
        _update_content_dirty_region(self);
    }


#if 0
    if (prcUpdate) {
        RECT updateRC = *prcUpdate;
        //printf("++ update:%d,%d,%d,%d\n", prcUpdate->left, prcUpdate->right, prcUpdate->top, prcUpdate->bottom);

        if (self->offX) {
            updateRC.left     -= self->offX;
            updateRC.right    -= self->offX;
        }
        if (self->offY) {
            updateRC.top      -= self->offY;
            updateRC.bottom   -= self->offY;
        }

        //printf("-- update:%d,%d,%d,%d,self->offY=%d\n", prcUpdate->left, prcUpdate->right, prcUpdate->top, prcUpdate->bottom, self->offY);
        texteditor_rect_view_to_client(self, &updateRC);
        InvalidateRect(self->hwnd, &updateRC, TRUE);

    }
    else {
        InvalidateRect(self->hwnd, NULL, TRUE);
    }
#endif
}

static int mTextEditor_getInvalidBkgnd(mTextEditor *self, RECT *prc)
{
    int cx, cy;
    if (!prc || !TE_VALID_OBJ(self)) {
        if (prc) memset(prc, 0, sizeof(RECT));
        return 0;
    }

    _c(pTextLayout)->getTextBoundSize(pTextLayout, &cx, &cy);   
    GetClientRect(self->hwnd, prc);

    if (RECTWP(prc) != self->visWidth)
        prc->right = prc->left + self->visWidth;
    if (RECTHP(prc) != self->visHeight)
        prc->bottom = prc->top + self->visHeight;

    if((cy - self->offY) >= self->visHeight)
    {
        memset(prc, 0, sizeof(RECT));
        return 0;
    }
    
    prc->top = cy - self->offY;
    return 1;
}

#define INIT_SELECT_BRUSH(self, hdc, oldColor) \
    if (TE_IS_FOCUS(self)) {\
        oldColor = SetBrushColor (hdc, GetWindowElementPixel(self->hwnd, \
                    WE_BGC_SELECTED_ITEM));    \
    }\
    else {\
        oldColor = SetBrushColor (hdc, GetWindowElementPixel(self->hwnd, \
                    WE_BGC_SELECTED_LOSTFOCUS));\
    }

#define INIT_NORMAL_BRUSH(self, hdc, oldColor) \
    oldColor = SetBrushColor(hdc, GetWindowBkColor(self->hwnd));

static inline void _fill_bkgnd(mTextEditor *self, 
        HDC hdc, const RECT *prcBk, BOOL isSel)
{
    gal_pixel oldColor; 
    RECT rc;


    if (isSel) {
        INIT_SELECT_BRUSH(self, hdc, oldColor);
    }
    else {
        INIT_NORMAL_BRUSH(self, hdc, oldColor);
    }

    if(!prcBk)
    {
        GetClientRect(self->hwnd, &rc);
        prcBk = &rc;
    }


    FillBox(hdc, prcBk->left, prcBk->top, RECTWP(prcBk), RECTHP(prcBk));

    SetBrushColor(hdc, oldColor);



}

static BOOL 
mTextEditor_onEraseBkgnd(mTextEditor* self, HDC hdc, const PRECT pClip)
{
    //do nothing, we clear the bkground in onPaint
    //return TRUE;
    
#ifdef __MINIGUI_LIB__
    if (!self || (GetWindowExStyle(self->hwnd) && WS_EX_TRANSPARENT))
        return TRUE;

    _fill_bkgnd(self, hdc, pClip, FALSE);
    return TRUE;
#else
    return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pClip);
#endif
}

#if 0
static void mTextEditor_drawBkgnd(mTextEditor* self, HDC hdc, 
        const RECT *prcBk, BOOL isSel)
{
/*  RECT rcClient;
    int  width, height;

    if (!self)
        return;

    GetClientRect(self->hwnd, &rcClient);
    width  = RECTW(rcClient);
    height = RECTH(rcClient);

    rcClient = *prcBk;
    OffsetRect(&rcClient, self->offX, 0);
    if(RECTW(rcClient) > width)
        rcClient.right = rcClient.left + width;

    if(RECTH(rcClient) > height)
        rcClient.bottom = rcClient.top + height;

    if (isSel) {
        _fill_bkgnd(self, hdc, &rcClient, TRUE);
    }
    else
        //SendMessage(self->hwnd, MSG_ERASEBKGND, hdc, (LPARAM)&rcClient);          
        _c(self)->onEraseBkgnd(self, hdc, &rcClient);*/
}
#endif

static BOOL _read_only(mTextEditor *self)
{
    if (GetWindowStyle(self->hwnd) & NCSS_TE_READONLY) {
        Ping();
        return TRUE;
    }
    return FALSE;
}

static inline void min_swap(int *pa, int *pb)
{
    if(*pa > *pb) {
        int tmp = *pa;
        *pa = *pb;
        *pb = tmp;  
    }
}

static inline void _remove_one_char(mTextEditor* self, int textPos)
{
    int strPos, strLen = 0;
    if (textPos < 0)
        return;

    strPos = _c(pTextLayout)->textCount2Len(pTextLayout, textPos, 1, &strLen);
    _c(pTextBuffer)->replace(pTextBuffer, strPos, strLen, NULL, -1);
}

static inline void _remove_chars(mTextEditor* self, BOOL bback)
{
    int selBegin, selEnd;

    if(_c(pTextBuffer)->getCount(pTextBuffer) <= 0)
        return ;

    selBegin = pTextLayout->sel_begin;
    selEnd = pTextLayout->cursor;
    
    min_swap(&selBegin, &selEnd);

#ifdef _MGHAVE_TEXTEDITTITLE
    if(self->title_idx > 0)
    {
        if(selBegin < self->title_idx || (bback && selBegin == selEnd && selBegin == self->title_idx))
            return;
    }
#endif


    if(selBegin == selEnd) {
        _remove_one_char(self, bback ? (pTextLayout->text_cursor - 1 ): pTextLayout->text_cursor);
    }
    else
    {
#ifdef _MGHAVE_TEXTEDITTITLE
        _c(pTextBuffer)->replace(pTextBuffer, selBegin, selEnd - selBegin, NULL, -1);
#endif
    }
    ncsNotifyParent(self, NCSN_TE_CHANGE);
}

static inline void _set_cont_changed(mTextEditor *self, BOOL changed)
{
    if (self) {
        if(changed)
            self->flags |= NCS_TEF_CONTCHANGED;
        else 
            self->flags &= ~NCS_TEF_CONTCHANGED;
    }
}

static int texteditor_get_text_char_size(mTextEditor* self, int begin)
{
    int char_size = 1;
    //replace mode
    ITextIterator *txtit = _c(pTextBuffer)->getAt(pTextBuffer, begin);
    if((txtit)->_vtable->getCharType(INTEFACE_ADJUST(txtit), 0) == TI_VISIBILE_CHAR)
    {
        int txt_idx = 1;
        ITextRender *render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
        HDC hdc = GetClientDC(self->hwnd);
        char_size = (render)->_vtable->getTextByteLen(INTEFACE_ADJUST(render), hdc, txtit, &txt_idx);
        ReleaseDC(hdc);
    }
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    return char_size;
}

static int mTextEditor_onChar(mTextEditor *self, WPARAM eucCode, DWORD keyFlags)
{
    int selBegin, selEnd, chlen;
    unsigned char ch[4];
    int replace_char_size = 0;

    if (!TE_VALID_OBJ(self) || _read_only(self) || (keyFlags & KS_CTRL))
        return 0;

    if(eucCode == 127 || eucCode == '\b') {
        _remove_chars(self, TRUE);
        return 0;
    }
  
    ch [0] = FIRSTBYTE (eucCode);
    ch [1] = SECONDBYTE (eucCode);
    ch [2] = THIRDBYTE (eucCode);
    ch [3] = FOURTHBYTE (eucCode);
    if (ch[3]) {
        chlen = 4;
    }
    else if (ch[2]) {
        chlen = 3;
    }
    else if (ch[1]) {
        chlen = 2;
    }
    else {
        chlen = 1;
    }

    selBegin = pTextLayout->sel_begin;
    selEnd = pTextLayout->cursor;

    if (self->hardLimit != -1) //limit text
    {
        int newCount = textlayout_get_text_count(pTextLayout) ;
        if(selBegin == selEnd)
        {
            if(!mTextEditor_isReplaceMode(self) || 
                    pTextLayout->text_sel_begin >= newCount)
                newCount ++;
        }

#ifdef _MGHAVE_TEXTEDITTITLE
        if(self->title_text_idx > 0)
            newCount -= self->title_text_idx ;
#endif
        if(newCount > self->hardLimit)
        {
            Ping ();
            ncsNotifyParent (self, NCSN_TE_MAXTEXT);
            return 0;
        }
    }

    min_swap(&selBegin, &selEnd);

    if (chlen == 1) {
        if (GetWindowStyle(self->hwnd) & NCSS_TE_UPPERCASE) {
            eucCode = toupper(eucCode);
        }
        else if (GetWindowStyle(self->hwnd) & NCSS_TE_LOWERCASE) {
            eucCode = tolower(eucCode);
        }
    }

    if(mTextEditor_isReplaceMode(self) && selBegin == selEnd
        && selBegin < _c(pTextBuffer)->getCount(pTextBuffer))
    {
        if(replace_char_size == 0)
            replace_char_size = texteditor_get_text_char_size(self, selBegin);
        selEnd = selBegin + replace_char_size ;
    }


    _c(self->textBuffer)->replace(self->textBuffer,
            selBegin, selEnd - selBegin, (char*)&eucCode, -1);

    ncsNotifyParent (self, NCSN_TE_CHANGE);
    _set_cont_changed(self, TRUE);
    return 0;
}

static void _te_set_cursor_pos(mTextEditor *self, int x, int y, BOOL isSel)
{
    if (TE_VALID_OBJ(self)) {
        int title_x = 0;
        int title_y = 0;
        int title_last_height = 0;
        int cx, cy;
        int total_height;

#ifdef _MGHAVE_TEXTEDITTITLE
        if(self->title_x >= 0) title_x = self->title_x;
        if(self->title_y >= 0) title_y = self->title_y;
        if(self->title_last_height >= 0) title_last_height = self->title_last_height;
#endif

        if(y < 0) y = 0;
        if(x < 0) x = 0;

        total_height = textlayout_get_total_height(pTextLayout);

        // text bound maybe bigger than visible rect, when not auotwrap.
        _c(pTextLayout)->getTextBoundSize(pTextLayout, &cx, &cy);

        //printf("--- x=%d, y=%d, title:(x=%d,y=%d, height=%d, idx=%d, text_idx=%d\n", x, y, self->title_x, self->title_y, self->title_last_height, self->title_idx, self->title_text_idx);

        // click at title rect
        if(y <= title_y 
                || (y >= title_y  && y <= (title_y + title_last_height) && x < title_x))
        {
            x = title_x;
            y = title_y + title_last_height;
        }
        // click at text rect outside
        else if(y > total_height)
        {
            y = total_height;
            if( x < title_x && y == (title_y + title_last_height))
                x = self->title_x;
            else
                x = cx;
        }

        //printf("---- new x=%d, y=%d total_height=%d\n", x, y,total_height);
        { /* add {} for avoid compile error in Visual C. */
            TE_SEL_CHANGING(self);
            _c(pTextLayout)->setCursorPos(pTextLayout, x, y, isSel);
            TE_SEL_CHANGED(self, isSel);
        }
    }
}

/* Note: the pos is in the char not in byte. */
static void _te_set_cursor(mTextEditor *self, int pos, BOOL isSel)
{
    if (TE_VALID_OBJ(self)) {
#ifdef _MGHAVE_TEXTEDITTITLE
        if(pos < self->title_text_idx)
            pos = self->title_text_idx;
        //printf ("_te_set_cursor: pos:%d, text_idx:%d\n", pos, self->title_text_idx);
#endif
        { /* add {} for avoid compile error in Visual C. */
            TE_SEL_CHANGING(self);
            _c(pTextLayout)->setCursor(pTextLayout, pos, isSel);
            TE_SEL_CHANGED(self, isSel);
        }
    }
}

static int 
mTextEditor_onKeyDown(mTextEditor *self, int scancode, DWORD keyFlags)
{
    BOOL bShift = keyFlags & KS_SHIFT;
    BOOL bCtrl  = keyFlags & KS_CTRL;

    if (!TE_VALID_OBJ(self)) return 1;

    switch (scancode) {
        /*case SCANCODE_BACKSPACE:
            if (!_read_only(self))
                _remove_chars(self,TRUE);
            return 0;*/

        case SCANCODE_INSERT:
            if(mTextEditor_isReplaceMode(self))
            {
                self->flags &= ~NCS_TEF_REPLACE_MODE;
            }
            else
            {
                self->flags |= NCS_TEF_REPLACE_MODE;
            }
            _update_caret_size(self);
            _change_caret_size(self, TRUE);
            return 0;

        case SCANCODE_REMOVE:
            if (!_read_only(self))
                _remove_chars(self,FALSE);
            return 0;

        case SCANCODE_CURSORBLOCKLEFT:
            _te_set_cursor(self, 
                    (bShift ? pTextLayout->text_sel_begin 
                            : pTextLayout->text_cursor) - 1, 
                    bShift);
            return 0;

        case SCANCODE_CURSORBLOCKRIGHT:
            _te_set_cursor(self, 
                    (bShift ? pTextLayout->text_sel_begin 
                            : pTextLayout->text_cursor) + 1, 
                    bShift);
            return 0;

        case SCANCODE_HOME:
        case SCANCODE_END:
        {
            int caretX, caretY;
            _c(pTextLayout)->getCursor(pTextLayout, &caretX, &caretY, NULL, bShift);
            if(scancode == SCANCODE_HOME)
                caretX = 0;
            else
                caretX = _c(self)->getMetrics(self, TEXTMETRICS_MAXWIDTH);
            _te_set_cursor_pos(self, caretX, caretY + 1, bShift);
            return 0;
        }

        case SCANCODE_PAGEDOWN:
        case SCANCODE_PAGEUP:
        {
            TE_SET_CURSORE_PAGE_MOVE(self);
            _c(self)->onVScroll(self, 
                scancode == SCANCODE_PAGEUP ?  SB_PAGEUP : SB_PAGEDOWN, 0);
            return 0;
        }

        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKDOWN:
        {
            int caretX, caretY;
            int height;
            
            _c(pTextLayout)->getCursor(pTextLayout, &caretX, &caretY, &height, bShift);
            if(scancode == SCANCODE_CURSORBLOCKUP && caretY <= 0)
                break;
        
            if(scancode == SCANCODE_CURSORBLOCKDOWN)
                caretY += (height+1);
            else 
                caretY -= (height-1);
            _te_set_cursor_pos(self, caretX, caretY, bShift);

            return 0;
        }
        case SCANCODE_A:
        {
            if (bCtrl) {
                _c(self)->selectAll(self);
            }
            return 0;
        }

        case SCANCODE_X:
        case SCANCODE_C:
        {
            if (bCtrl) {
                int selBegin, selEnd;
                selBegin = pTextLayout->sel_begin;
                selEnd = pTextLayout->cursor;

                if (selBegin == selEnd)
                   return 0; 

                min_swap(&selBegin, &selEnd);

                //clear clipboard
                SetClipBoardData (CBNAME_TEXT, NULL, 0, CBOP_NORMAL);

                SetClipBoardData(CBNAME_TEXT, 
                        pTextBuffer->buffer + selBegin, 
                        selEnd - selBegin, CBOP_APPEND);

                //for SCANCODE_X: need delete selected text
                if (scancode == SCANCODE_X && !_read_only(self)) {
                    _c(pTextBuffer)->replace(pTextBuffer, 
                            selBegin, selEnd - selBegin, NULL, -1);
                    ncsNotifyParent(self, NCSN_TE_CHANGE);
                }

                return selEnd - selBegin;
            }
            return 0;
        }

        case SCANCODE_V:
        {
            if (GetWindowStyle(self->hwnd) & NCSS_TE_READONLY)
                return 0;

            if (bCtrl) {
                int inserting;
                int begin_pos;
                char *txtBuffer;
                DWORD dwStyle = GetWindowStyle(self->hwnd);

                inserting = GetClipBoardDataLen (CBNAME_TEXT);
                if (inserting > 0) {
                    txtBuffer = FixStrAlloc (inserting);
                    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);

                    if(pTextLayout->sel_begin > pTextLayout->cursor)
                        begin_pos = pTextLayout->cursor;
                    else 
                        begin_pos = pTextLayout->sel_begin;

                    _c(self)->insertText(self, txtBuffer, inserting);

                    if(dwStyle & NCSS_TE_UPPERCASE)
                        _c(pTextBuffer)->setCase(pTextBuffer, begin_pos, begin_pos+inserting, TRUE);
                    else if(dwStyle & NCSS_TE_LOWERCASE)
                        _c(pTextBuffer)->setCase(pTextBuffer, begin_pos, begin_pos+inserting, FALSE);

                    FreeFixStr(txtBuffer);
                }
                return inserting;
            }
            return 0;
        }
    }
    return 1;
}

static int mTextEditor_getSel(mTextEditor *self, char *buffer, int len)
{
    int selBegin, selEnd;

    if (!TE_VALID_OBJ(self)) {
        return -1;
    }

    selBegin = pTextLayout->sel_begin;
    selEnd = pTextLayout->cursor;


    if (selBegin == selEnd)
        return -1;

    min_swap(&selBegin, &selEnd);

    if(!buffer || len <= 0)
        return selEnd - selBegin; //return the length of sel

    len = MIN(len, selEnd - selBegin);

    buffer[0] = 0;

    //strncpy(buffer, pTextBuffer->buffer + selBegin, len);
    _c(pTextBuffer)->getText(pTextBuffer, selBegin, buffer, len);
    buffer[len] = 0;
    return len;
}

static int mTextEditor_setText(mTextEditor *self, const char* text)
{
    int len = -1;
    int at = 0;
    char * str;

    if (!TE_VALID_OBJ(self))
        return -1;

    if(text)
        len = strlen(text);

#ifdef _MGHAVE_TEXTEDITTITLE
    at = self->title_idx;
    if(at < 0)
        at = 0;
#endif

    if (len > 0 && self->hardLimit != -1) {
        int text_count;
        text_count = texteditor_get_text_count(self, at, FALSE, text, len);
        
        if(text_count > self->hardLimit)
            len = texteditor_get_text_length(self, at, FALSE, text, self->hardLimit, len);
    }
    else {
        len = texteditor_get_validate_text_length(self, 0, FALSE, text, len);
    }

    str = texteditor_case_copy(self, text, len);

    len = _c(pTextBuffer)->replace(pTextBuffer, at, 
                _c(pTextBuffer)->getCount(pTextBuffer) - at, str, len);

    if(str && str != text)
        FreeFixStr(str);

    _te_set_cursor(self, 0, FALSE);

    ncsNotifyParent (self, NCSN_TE_UPDATE);
    _set_cont_changed(self, TRUE);
    return len;
}

static int mTextEditor_insertText(mTextEditor *self, const char* text, int len)
{
    int length = -1;
    int begin_pos;
    int replace_len;
    char * str;

    if (!TE_VALID_OBJ(self))
        return -1;

    if(!text && len < 0)
        len = 0;
    else if(text && len < 0)
        len = strlen(text);

    if(pTextLayout->sel_begin > pTextLayout->cursor)
    {
        begin_pos = pTextLayout->cursor;
        replace_len = pTextLayout->sel_begin - pTextLayout->cursor;
    }
    else {
        begin_pos = pTextLayout->sel_begin;
        replace_len = pTextLayout->cursor - pTextLayout->sel_begin;
    }


    if(len>0 && self->hardLimit != -1) //limit text
    {
        int selBegin, selEnd;
        int newCount;
        int text_len;
        
        text_len = texteditor_get_text_count(self, begin_pos,  FALSE, text, len); 

        selBegin = pTextLayout->text_sel_begin;
        selEnd = pTextLayout->text_cursor;
        min_swap(&selBegin, &selEnd);

       newCount = textlayout_get_text_count(pTextLayout);
     
       if(text_len != (selEnd - selBegin))
       {
           if(mTextEditor_isReplaceMode(self))
           {
               if(text_len > (selEnd - selBegin))
               {
                    int insert = text_len - selEnd + selBegin;
                    if(selEnd  + insert > newCount)
                        newCount = selEnd + insert;
               }
               else
                   newCount += (text_len - selEnd + selBegin);
           }
           else
               newCount += (text_len - (selEnd - selBegin)); 
       }

#ifdef _MGHAVE_TEXTEDITTITLE
       if(self->title_idx > 0)
           newCount -= self->title_text_idx;
#endif
       if(newCount > self->hardLimit)
       {
           text_len += (self->hardLimit - newCount);
           if(text_len <= 0)
           {
                Ping();
                ncsNotifyParent(self, NCSN_TE_MAXTEXT);
                return 0;
           }
           //get the real len
           len = texteditor_get_text_length(self, begin_pos, FALSE, text, text_len, len);
       }
    }
    else
    {
        len = texteditor_get_validate_text_length(self, begin_pos,FALSE, text, len);
    }

    str = texteditor_case_copy(self, text, len);

    length = _c(pTextBuffer)->replace(pTextBuffer, begin_pos, replace_len, str, len);
    
    if(str && str != text)
        FreeFixStr(str);

    ncsNotifyParent (self, NCSN_TE_CHANGE);
    _set_cont_changed(self, TRUE);

    return length;
}

static int mTextEditor_getText(mTextEditor *self, char* text, int len)
{
    if (TE_VALID_OBJ(self)) 
    {
#ifdef _MGHAVE_TEXTEDITTITLE
        int at = self->title_idx;
        if(at < 0) 
            at = 0;
#else
        int at = 0;
#endif
        return _c(pTextBuffer)->getText(pTextBuffer, at, text, len);
    }
    return 0;
}

static int mTextEditor_getTextLen(mTextEditor *self)
{
    if (TE_VALID_OBJ(self)) {
        return _c(pTextBuffer)->getCount(pTextBuffer)
#ifdef _MGHAVE_TEXTEDITTITLE
            - (self->title_idx > 0 ? self->title_idx : 0)
#endif
            ;
    }
    return 0;
}

static void mTextEditor_onSetFocus(mTextEditor *self, HWND oldActiveWnd, int lParam)
{
    int text_len = 0;

    if (!self || self->hwnd == oldActiveWnd)
        return;

    self->flags |= NCS_TEF_FOCUS;

    ncsNotifyParent (self, NCSN_TE_SETFOCUS);
    InvalidateRect (self->hwnd, NULL, TRUE);

#ifdef _MGHAVE_TEXTEDITTITLE
    text_len = self->title_idx <= -1 ? 0: self->title_idx;
#endif

    /*[bug5291]: when get focus don't auto move caret. 
     * only there is empty text, call it to show the caret. */
    if (self->textBuffer->char_len == text_len)
        _c(self)->updateCaret(self);
}

static void mTextEditor_onKillFocus(mTextEditor *self, HWND newActiveWnd, int lParam)
{
    if (!self)
        return;

    self->flags &= ~NCS_TEF_FOCUS;

    //unselect all
    if (!(GetWindowStyle(self->hwnd) & NCSS_TE_NOHIDESEL)) {
        int cursor = _c(pTextLayout)->getCursor(pTextLayout, NULL, NULL, NULL, FALSE);
        _c(pTextLayout)->setCursor(pTextLayout, cursor, FALSE);
    }

    HideCaret(self->hwnd);

    if (self->flags & NCS_TEF_CONTCHANGED)
        ncsNotifyParent (self, NCSN_TE_CONTENTCHANGED);
    ncsNotifyParent (self, NCSN_TE_KILLFOCUS);

    InvalidateRect (self->hwnd, NULL, TRUE);
}

static BOOL mTextEditor_onFontChanged(mTextEditor *self)
{
    if (!TE_VALID_OBJ(self))
        return FALSE;


    DestroyCaret (self->hwnd);
    CreateCaret(self->hwnd, NULL, 1, GetWindowFont(self->hwnd)->size);

    _update_content_view(self, -1, -1, TRUE);

    InvalidateRect(self->hwnd, NULL, TRUE);

    return TRUE;
}

static void mTextEditor_onPaint(mTextEditor *self, HDC hdc, const PCLIPRGN pClip)
{
    self->flags |= NCS_TEF_HAS_PAINTED;

    if (TE_VALID_OBJ(self)) {
        RECT rcView;

        texteditor_get_view_rect(self, &rcView);
        //printf("-- rcView = %d,%d,%d,%d\n", rcView.left, rcView.top, rcView.right, rcView.bottom);

#if 0
        if(!texteditor_must_update_bkgnd(self) && (
                    (self->oldOffX != self->offX || self->oldOffY != self->offY )
                        || (self->textLayout->old_height != -1
                    && self->textLayout->old_height != self->textLayout->new_height)
                    ))
        {
            //we scroll the view area
            //use bitblt to scroll the area 
            int offx = self->oldOffX - self->offX;
            int offy = self->oldOffY - self->offY; 
            int sx, sy, sw, sh, dx, dy;
            RECT rc;
            PCLIPRGN prgn = CreateClipRgn();
        
            //save clipRgn
            GetClipRegion(hdc,prgn);

            //set ClipRgn make the scrolled area enabled
           
            if(self->oldOffX != self->offX || self->oldOffY != self->offY)
            {
                if(offx == 0) {
                    sx = dx = rcView.left;
                    sw = RECTW(rcView);
                }
                else if(offx > 0)
                {
                    sx = rcView.left;
                    dx = rcView.left + offx;
                    sw = RECTW(rcView) - offx;
                }
                else {
                    sx = rcView.left - offx;
                    dx = rcView.left;
                    sw = RECTW(rcView) + offx;
                }

                if(offy == 0){
                    sy = dy = rcView.top;
                    sh = RECTH(rcView);
                }
                else if(offy > 0){
                    sy = rcView.top;
                    dy = rcView.top + offy;
                    sh = RECTH(rcView) - offy;
                }
                else {
                    sy = rcView.top - offy;
                    dy = rcView.top;
                    sh = RECTH(rcView) + offy;
                }
                //sw --;
                //sh --;

                rc.left = dx;
                rc.top  = dy;
                rc.right = dx + sw;
                rc.bottom = dy + sh;

                SelectClipRect(hdc, &rc);

                //printf("--- oldOffX=%d, Y=%d, offX=%d, y=%d\n",self->oldOffX, self->oldOffY, self->offX, self->offY);
                //printf("--- bitblt:sx=%d,sy=%d,sw=%d,sh=%d, dx=%d,dy=%d\n", sx,sy, sw, sh, dx,dy);
                BitBlt(hdc, sx, sy, sw, sh, hdc, dx, dy, 0);
            }


            //bitblt the area the height changed
            if(self->textLayout->old_height != -1
                    && self->textLayout->old_height != self->textLayout->new_height)
            {
                //printf("---- old_height=%d, new_height=%d\n", self->textLayout->old_height, self->textLayout->new_height);
                sx = rcView.left;
                sw = RECTW(rcView);
                sy = self->textLayout->old_height - self->offY + self->margins.top;
                sh = rcView.bottom - sy;
                dy = self->textLayout->new_height - self->offY + self->margins.top;
                if( (dy + sh) > rcView.bottom)
                    sh = rcView.bottom - dy;
                dx = sx;

                rc.left = dx;
                rc.top  = dy;
                rc.right = dx + sw;
                rc.bottom = dy + sh;

                SelectClipRect(hdc, &rc);

                //printf("--- bitblt height:sx=%d,sy=%d,sw=%d,sh=%d, dx=%d,dy=%d\n", sx,sy, sw, sh, dx,dy);
                BitBlt(hdc, sx, sy, sw, sh, hdc, dx, dy, 0);
                self->textLayout->old_height = -1;
            }
            
            SelectClipRegion(hdc, prgn);
            DestroyClipRgn(prgn);
        }

        //clear the scroll info
        self->oldOffX = self->offX;
        self->oldOffY = self->offY;
#endif

/*     if(!(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT))
            _fill_bkgnd(self, hdc, NULL, FALSE);
*/
        SetBkMode(hdc, BM_TRANSPARENT);

        //usleep(50000);

        //TODO draw margins Decorative hear, e.g frameset

        //draw linedectors
        mTextEditor_drawLinesDecorative(self, hdc, self->offX, self->offY, RECTW(rcView), RECTH(rcView));
        //usleep(50000);

        ClipRectIntersect(hdc, &rcView);
        _c(pTextLayout)->draw(pTextLayout, hdc, self->offX, 
                self->offY, self->visWidth, self->visHeight);

        //draw margin
/*        SelectClipRect(hdc, NULL);
        SetPenColor(hdc, COLOR_red);
        RECT myrc;
        GetClientRect(self->hwnd, &myrc);
        Rectangle(hdc, self->margins.left-1, self->margins.top-1,myrc.right - self->margins.right-1, myrc.bottom - self->margins.bottom-1);
        SetPenColor(hdc, COLOR_blue);
        Rectangle(hdc, 0, 0, myrc.right ,self->margins.top);
        Rectangle(hdc, 0, self->margins.top, self->margins.left, myrc.bottom - self->margins.bottom);
        Rectangle(hdc, 0, myrc.bottom - self->margins.bottom, myrc.right, myrc.bottom);
        Rectangle(hdc, myrc.right - self->margins.right, 0, myrc.right, myrc.bottom);*/
    }
}

static int mTextEditor_onLButtonDown(mTextEditor *self, int x, int y, DWORD keyFlags)
{
    if (!GetCapture()) {
        SetCapture(self->hwnd);
    }
    texteditor_client_to_view(self, &x, &y);
    _te_set_cursor_pos(self,  x + self->offX, y + self->offY, FALSE);
    return 0;
}

static int mTextEditor_onLButtonDBClk(mTextEditor *self, int x, int y, DWORD keyFlags)
{
    ncsNotifyParent (self, NCSN_WIDGET_DBCLICKED);
    return 0;
}

static int mTextEditor_onLButtonUp(mTextEditor *self, int x, int y, DWORD keyFlags)
{
    if(GetCapture() == self->hwnd)
        ReleaseCapture();
    ncsNotifyParent (self, NCSN_WIDGET_CLICKED);
    return 0;
}

static int mTextEditor_onMouseMove(mTextEditor *self, int x, int y, DWORD keyFlags)
{
    if(GetCapture() == self->hwnd)
    {
        ScreenToClient(self->hwnd, &x, &y);
        texteditor_client_to_view(self, &x, &y);
        _te_set_cursor_pos(self,  x + self->offX, y + self->offY, TRUE);
    }
    return 0;
}


static void mTextEditor_onCSizeChanged(mTextEditor *self, int visWidth, int visHeight)
{
    //-- WARNING:  don't use visWidth and visHeight, because they would be different with ClientRect
    //MSG_CSIZECHANGED is sent as Notify Message, the visWidth and visHeight is the client width and height
    //before seconds
    _update_content_view(self, -1, -1,FALSE);
    //need update the margin
    InvalidateRect(self->hwnd, NULL, TRUE);
}

/**
 * index:   paragraph index (not contain title context)
 * start:   start position (in byte)
 * len:     copied string length in specified paragraph
 * buffer:  string buffer
 */
static int mTextEditor_getParaText(mTextEditor *self, int index,
                        int start, int len, char* buffer)
{
    int char_start = 0, count = 0;
    int copy_len = 0;
    ITextIterator * txtit = NULL;

    if (!TE_VALID_OBJ(self) || index < -1 || start < 0 || len <= 0)
        return 0;

    // get whole text
    if(index == -1) 
        return _c(self)->getText(self, buffer, len);

    count = _c(pTextBuffer)->getCount(pTextBuffer);

#ifdef _MGHAVE_TEXTEDITTITLE
    if(self->title_idx > 0)
    {
        if(index == 0)
        {
            _c(pTextLayout)->textCount2Len(pTextLayout, 
                    self->title_text_idx - self->title_last_para_text_index,
                    self->title_last_para_text_index, &char_start);
        }
        else
            char_start = 0;
        index += self->title_paracount;
    }
#else
    char_start = 0;
#endif
    
    if(index > 0)
        char_start = _c(pTextLayout)->getIndexByPara(pTextLayout, index, char_start, NULL, FALSE);

    if(char_start >= count)
        goto RETURN_0;

    txtit = _c(pTextBuffer)->getAt(pTextBuffer, char_start);

    (void) (txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), &len, &copy_len, FALSE);

    if(start >= copy_len)
        goto RETURN_0;

    /* set start
     * don't need to judge the copy_len whether overrange the line. 
     * because mTextIterator_getMore will stop when have '\n'. */
    (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), char_start + start);
    copy_len -= start;
    strncpy(buffer, (txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), NULL, NULL, FALSE), copy_len);

    buffer[copy_len] = 0;
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    return copy_len;
RETURN_0:
    buffer[0] = 0;
    if(txtit)
        (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    return 0;
}

/* External interface: the index is not contain the title context. */
static int mTextEditor_getLineText(mTextEditor *self, int index,
                        int start, int len, char* buffer)
{
    int char_start = 0, count = 0, title_linecount = 0;
    int col = 0, copy_len = 0, line_len = 0;
    char* str = NULL;
    ITextIterator* txtit = NULL;

    if (!TE_VALID_OBJ(self) || index < -1 || start < 0 || len <= 0)
        return 0;

    // get whole text
    if(index == -1) 
        return _c(self)->getText(self, buffer, len);

    count = _c(pTextBuffer)->getCount(pTextBuffer);

    // col must be byte 
    // texteditor_add_title_content(self, &index, &col, TRUE);
#ifdef _MGHAVE_TEXTEDITTITLE
    texteditor_update_title_line(self);
    title_linecount = self->title_linecount;
    if (title_linecount < 0)
        title_linecount = 0;

    if (self->title_idx > 0)
    {
        if (0 == index)
        {
            _c(pTextLayout)->textCount2Len(pTextLayout, 
                    self->title_text_idx - self->title_last_line_text_index, 
                    self->title_last_line_text_index, &col);
        }
        index += self->title_linecount;
    }
#endif
    
    if(index > 0)
        char_start = _c(pTextLayout)->getIndexByLine(pTextLayout, index, col, NULL, FALSE);

    if(char_start >= count)
        return 0;

    txtit = _c(pTextBuffer)->getAt(pTextBuffer, char_start);
    str = (char*)(txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), &len, &copy_len, FALSE);

    if(start >= copy_len)
    {
        (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
        return 0;
    }

    // set start
    (txtit)->_vtable->reset(INTEFACE_ADJUST(txtit), char_start + start);

    /* note: the index must subtract the title context,
     * because mTextLayout_getLineLength param of index is not contain title context.
     * It's a external interface. All external interface(mTextEditor) param of index not contain title context,
     * and all internal interface(mTextLayout) param of index contain title context. */
    line_len = _c(self)->getLineLength(self, index - title_linecount, NULL, FALSE);

    copy_len -= start;
    line_len -= start;
    copy_len = MIN(copy_len, line_len);
    if(copy_len < 0)
        copy_len = 0;

    if(str)
    {
        strncpy(buffer, str, copy_len);
    }
    else
    {
        (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
        return 0;
    }

    buffer[copy_len] = 0;
    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));
    return copy_len;
}

static int mTextEditor_getParaNum(mTextEditor *self)
{
    if (TE_VALID_OBJ(self)) { 
        int count = _c(pTextBuffer)->getCount(pTextBuffer);
#ifdef _MGHAVE_TEXTEDITTITLE
        if(count == self->title_idx)
            return 0;
#endif
        return count ? 
            (_c(pTextLayout)->getParaByIndex(pTextLayout, count, NULL, FALSE) + 1
#ifdef _MGHAVE_TEXTEDITTITLE
             - (self->title_idx > 0 ? self->title_paracount : 0)
#endif
            ) 
            : 0;
    }
        
    return 0;
}

// index: begin 0 
static int mTextEditor_getParaLength(mTextEditor *self, int index, int *startPos)
{
    int start = 0, count;
    ITextIterator * txtit;
    int visible_chars = 0;

    if (index < 0 || !TE_VALID_OBJ(self))
        return -1;

    count = _c(pTextBuffer)->getCount(pTextBuffer);

#ifdef _MGHAVE_TEXTEDITTITLE
    if(self->title_idx > 0)
    {
        if(index == 0)
        {
            _c(pTextLayout)->textCount2Len(pTextLayout, 
                    self->title_text_idx - self->title_last_para_text_index, 
                    self->title_last_para_text_index, &start);
        }
        index += self->title_paracount;
    }
#endif

    start = _c(pTextLayout)->getIndexByPara(pTextLayout, index, start, NULL, FALSE);
    if(start >= count)
    {
        start = count;
        if(startPos)
            *startPos = start;
        return 0;
    }

    txtit = _c(pTextBuffer)->getAt(pTextBuffer, start);

    if(!txtit)
    {
        if(startPos)
            *startPos = start;
        return 0;
    }

    while(1)
    {
        int len;
        (void)(txtit)->_vtable->getMore(INTEFACE_ADJUST(txtit), NULL, &len, TRUE);
        visible_chars += len;
        if(_I(txtit)->index(INTEFACE_ADJUST(txtit)) >= (count -1))
            break;
        if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), -1) == TI_LINERETURN)
            break;
    }

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    if(startPos)
        *startPos = start;

    return visible_chars;
}

static int mTextEditor_getParaLengthInMChar(mTextEditor *self, int paraNo, int *startPos)
{
    int total;
    int len = 0, col = 0;
    int str_idx = 0, para_idx = 0;

    mTextLayoutNode* node;
    ITextIterator * txtit;
    mCommBTreeLeafIterator node_it;

    if (paraNo < 0 || !TE_VALID_OBJ(self)) 
        return -1;

    total = _c(pTextBuffer)->getCount(pTextBuffer);

    texteditor_add_title_content(self, &paraNo, &col, FALSE);

    node = textlayout_get_first_node(pTextLayout);

    txtit = _c(pTextBuffer)->getAt(pTextBuffer, 0);

    (void)initObjectArgs((mObject*)((void*)(&node_it)), 
            (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, pTextLayout, node);

    while((node = (mTextLayoutNode*)node_it.current))
    {
        if (para_idx >= paraNo)
        {
            len += node->text_count;
            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx + node->str_count - 1) == TI_LINERETURN)
            {
                // ignore the '\n'
                if (len > 0)
                    len -= 1;
                break;
            }
        }

        str_idx += node->str_count;
        if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx - 1) == TI_LINERETURN)
            para_idx ++;
        _c(&node_it)->next(&node_it);
    }

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    if (str_idx >= total)
    {
        SMART_ASSIGN(startPos, total); 
        return 0;
    }

#ifdef _MGHAVE_TEXTEDITTITLE
    if (paraNo == self->title_paracount)
        len -= self->title_last_para_text_index;
#endif

    SMART_ASSIGN(startPos, str_idx);
    return len;
}

/* External interface: the index is not contain the title context. */
static int mTextEditor_getLineLength(mTextEditor *self, int lineNo, int *startPos, BOOL bMChar)
{
    int total;
    int col = 0, str_idx = 0;
    int str_len = 0, text_len = 0, line_idx = 0;

    mTextLayoutNode* node;
    ITextIterator * txtit;
    mCommBTreeLeafIterator node_it;

    if (lineNo < 0 || !TE_VALID_OBJ(self)) 
        return -1;

    total = _c(pTextBuffer)->getCount(pTextBuffer);

    texteditor_add_title_content(self, &lineNo, &col, TRUE);

    node = textlayout_get_first_node(pTextLayout);

    (void)initObjectArgs((mObject*)((void*)(&node_it)), 
            (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, pTextLayout, node);

    txtit = _c(pTextBuffer)->getAt(pTextBuffer, 0);

    while((node = (mTextLayoutNode*)node_it.current))
    {
        if (line_idx >= lineNo)
        {
            text_len = node->text_count;
            str_len = node->str_count;

            if(_I(txtit)->getCharType(INTEFACE_ADJUST(txtit), str_idx + node->str_count - 1) == TI_LINERETURN)
            {
                // ignore the '\n'
                if (text_len > 0) text_len -= 1;
                if (str_len > 0) str_len -= 1;
            }

            break;
        }

        str_idx += node->str_count;
        line_idx ++;
        _c(&node_it)->next(&node_it);
    }

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

    if (str_idx >= total)
    {
        SMART_ASSIGN(startPos, total); 
        return 0;
    }

#ifdef _MGHAVE_TEXTEDITTITLE
    if (lineNo == self->title_linecount)
    {
        text_len -= self->title_last_line_text_index;

        if (!bMChar)
        {
            _c(pTextLayout)->textCount2Len(pTextLayout, 
                    self->title_text_idx - self->title_last_line_text_index, 
                    self->title_last_line_text_index, &total);
            str_len -= total;
        }
    }
#endif

    SMART_ASSIGN(startPos, str_idx);
    if (bMChar)
        return text_len;
    else
        return str_len;
}

static BOOL mTextEditor_setProperty(mTextEditor *self, int id, DWORD value)
{
    if (!self || id >= NCSP_TE_MAX)
        return FALSE;

    switch (id) {
        case NCSP_TE_SCROLL_HSTEP:
            self->scrollHStep = value;
            return TRUE;

        case NCSP_TE_SCROLL_VSTEP:
            self->scrollVStep = value;
            return TRUE;

        case NCSP_TE_SCROLL_MODE:
        {
            if (value == self->scrollBarMode)
                return FALSE;

            self->scrollBarMode = value;
            switch (value) {
                case NCS_TEF_SCROLLBAR_NEVER:
                {
                    //previous:scrollbar shown
                    ExcludeWindowStyle(self->hwnd, NCSS_TE_AUTOVSCROLL | NCSS_TE_AUTOHSCROLL);
                    break;
                }

                case NCS_TEF_SCROLLBAR_AUTO:
                {
                    IncludeWindowStyle(self->hwnd, NCSS_TE_AUTOVSCROLL | NCSS_TE_AUTOHSCROLL | WS_HSCROLL | WS_VSCROLL);
                    break;
                }

                case NCS_TEF_SCROLLBAR_ALWAYS:
                {
                    IncludeWindowStyle(self->hwnd, WS_HSCROLL | WS_VSCROLL );
                    ExcludeWindowStyle(self->hwnd, NCSS_TE_AUTOHSCROLL | NCSS_TE_AUTOVSCROLL);
                    break;
                }
                default:
                    return FALSE;
            }
            _update_scrollbar(self);

            return TRUE;
        }

        case NCSP_TE_READONLY:
        {
            if (self && self->hwnd) {
                if (value)
                    IncludeWindowStyle (self->hwnd, NCSS_TE_READONLY);
                else
                    ExcludeWindowStyle (self->hwnd, NCSS_TE_READONLY);
                return TRUE;
            }
            break;
        }
        case NCSP_TE_MARGINRECT:
        {
            const RECT * margins = (const RECT*)value;
            if(margins)
            {
                RECT rc;
                GetClientRect(self->hwnd, &rc);

                self->margins = *margins;

                if(self->margins.left >= RECTW(rc)/2)
                    self->margins.left = RECTW(rc) / 2 - 5;
                if(self->margins.left < 0)
                    self->margins.left = 0;

                if(self->margins.top >= RECTH(rc) / 2)
                    self->margins.top = RECTH(rc) / 2 - 5;
                if(self->margins.top < 0)
                    self->margins.top = 0;

                if(self->margins.right >= RECTW(rc)/2)
                    self->margins.right = RECTW(rc) / 2 - 5;
                if(self->margins.right < 0)
                    self->margins.right = 0;
                if(self->margins.bottom >= RECTH(rc)/2)
                    self->margins.bottom = RECTH(rc)/2 - 5;
                if(self->margins.bottom < 0)
                    self->margins.bottom = 0;

                //update client area
                _update_content_view(self, -1, -1, FALSE);
                InvalidateRect(self->hwnd, NULL, TRUE);
                return TRUE;
            }
            return FALSE;
        }
        case NCSP_TE_TEXTLIMIT:
        {
            int newLimit = (int)value;
            if(newLimit >= 0){
                int count = textlayout_get_text_count(pTextLayout);
#ifdef _MGHAVE_TEXTEDITTITLE
                if(self->title_text_idx > 0)
                    count -= self->title_text_idx;
#endif
                if(count > newLimit) {
                    Ping();
                    return FALSE;
                }
                else 
                    self->hardLimit = newLimit;
            }
            else
                self->hardLimit = -1; //cancel limit
            return TRUE;
        }
        
        default:
            break;
    }

    return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mTextEditor_getProperty(mTextEditor *self, int id)
{
    if (!self || id >= NCSP_TE_MAX)
        return -1;

    switch (id) {
#if 0
        case NCSP_TE_SCROLL_MODE:
            return self->scrollBarMode;
#endif

        case NCSP_TE_SCROLL_HSTEP:
            return self->scrollHStep;

        case NCSP_TE_SCROLL_VSTEP:
            return self->scrollVStep;

        case NCSP_TE_READONLY:
        {
            if (self && self->hwnd) {
                return GetWindowStyle(self->hwnd) & NCSS_TE_READONLY;
            }
            break;
        }
        case NCSP_TE_MARGINRECT:
        {
            return (DWORD)(&self->margins);
        }
        case NCSP_TE_TEXTLIMIT:
            return (DWORD)(self->hardLimit);
        case NCSP_TE_LINECOUNT:
            return (DWORD)(_c(self)->getLineCount(self));
        case NCSP_TE_PARAGRAPHCOUNT:
            return (DWORD)(_c(self)->getParaNum(self));
        default:
            break;
    }
    return Class(mWidget).getProperty((mWidget*)self,id) ;
}

static BOOL mTextEditor_enableCaret(mTextEditor *self, BOOL enable)
{
    if (!TE_VALID_OBJ(self))
        return FALSE;

    if (enable && (self->flags & NCS_TEF_NOCARET)) {
        self->flags &= ~NCS_TEF_NOCARET;
        _c(self)->updateCaret(self);
        return TRUE;
    }
    else if (!enable && !(self->flags & NCS_TEF_NOCARET)) {
        self->flags |= NCS_TEF_NOCARET;
        HideCaret(self->hwnd);
        return TRUE;
    }
    return FALSE;
}

static int mTextEditor_setCaretPosByPara(mTextEditor *self, int paraIdx, 
        int charPos, BOOL isSel)
{
    int index = 0, cursor = 0; 
    BOOL out_of_range = FALSE;

    if (!TE_VALID_OBJ(self) || charPos < 0 || paraIdx < 0)
        return -1;

    texteditor_add_title_content(self, &paraIdx, &charPos, FALSE);

    index = _c(pTextLayout)->getIndexByPara(pTextLayout, paraIdx, charPos, &out_of_range, TRUE);

    _te_set_cursor(self, index, isSel);

    cursor = isSel ? pTextLayout->text_sel_begin : pTextLayout->text_cursor;

    if (out_of_range)
        return -1;
    else
        return cursor
#ifdef _MGHAVE_TEXTEDITTITLE
                - (self->title_text_idx > 0 ? self->title_text_idx : 0)
#endif
            ;
}

static int mTextEditor_setCaretPosByLine(mTextEditor *self, int lineIdx, 
        int charPos, BOOL isSel)
{
    int index = 0, cursor = 0;
    BOOL out_of_range = FALSE;

    if (!TE_VALID_OBJ(self) || charPos < 0 || lineIdx < 0)
        return -1;

#ifdef _MGHAVE_TEXTEDITTITLE
    {
        int total = _c(self->textBuffer)->getCount(self->textBuffer);
        /* when have any text, the texteditor_update_title_line can't calc correct. 
         * so don't call it in this situation. */
        if (total <= self->title_idx)
        {
            _te_set_cursor(self, index, isSel);
            if (lineIdx > 0 || charPos > 0)
                return -1;
            else
                return 0;
        }
    }
#endif

    texteditor_add_title_content(self, &lineIdx, &charPos, TRUE);

    index = _c(pTextLayout)->getIndexByLine(pTextLayout, lineIdx, charPos, &out_of_range, TRUE);

    _te_set_cursor(self, index, isSel);

    cursor = isSel ? pTextLayout->text_sel_begin : pTextLayout->text_cursor;

    if (out_of_range)
        return -1;
    else
        return cursor
#ifdef _MGHAVE_TEXTEDITTITLE
                - (self->title_text_idx > 0 ? self->title_text_idx : 0)
#endif
            ;
}

static int mTextEditor_setSel(mTextEditor *self, 
        unsigned int startParaIdx, unsigned int startOffChar, 
        unsigned int endParaIdx, unsigned int endOffChar)
{
    if (_c(self)->setCaretPosByPara(self, startParaIdx, startOffChar, FALSE) != -1
        && _c(self)->setCaretPosByPara(self, endParaIdx, endOffChar, TRUE) != -1)
        return 0;

    return -1;
}

static int mTextEditor_getLineCount(mTextEditor *self)
{
    mCommBTreeLeafIterator iter;
    mTextLayoutNode *node;
    int line = 0;
#ifdef _MGHAVE_TEXTEDITTITLE
    int str_count = 0;
    
    /* if last node is ended with '\n', it will add a new empty line.
     * so when the title last is '\n', and there has any real text,
     * it will make line count overrange 1. */
    if (_c(pTextBuffer)->getCount(pTextBuffer) == self->title_idx)
        return 0;
#endif

    if (!TE_VALID_OBJ(self))
        return -1;

    (void)INIT_OBJV(mCommBTreeLeafIterator, &iter, 2, pTextLayout, NULL);
    node = (mTextLayoutNode*)_c(&iter)->getCurrent(&iter);
#ifdef _MGHAVE_TEXTEDITTITLE
    if(self->title_idx > 0)
    {
        while(node && node->str_count > 0){
            str_count += node->str_count;
            if(str_count > self->title_idx)
                break;
            node = (mTextLayoutNode*)_c(&iter)->next(&iter);
        }
    }
#endif

    while (node && node->height > 0){
        //the line is valid.
        line++;

        node = (mTextLayoutNode*)_c(&iter)->next(&iter);
    };

    return line;
}

static int mTextEditor_getCaretPosByPara(mTextEditor *self, 
        int *paraIdx, int *offChar, int *height, BOOL isSel)
{
    int cursor, paraNo;

    if (!TE_VALID_OBJ(self) || !(paraIdx || offChar || height))
        return -1;

    //cursor is the text not charactors
    //cursor = _c(pTextLayout)->getCursor(pTextLayout, NULL, NULL, height, isSel); 
    //FIXEDME: add Get string cursor pos interface
    //cursor = isSel? pTextLayout->sel_begin : pTextLayout->cursor;
    cursor = isSel ? pTextLayout->text_sel_begin : pTextLayout->text_cursor;
    if (height)
        *height = isSel ? pTextLayout->sel_begin_height : pTextLayout->cursor_height;
    paraNo = _c(pTextLayout)->getParaByIndex(pTextLayout, cursor, offChar, TRUE);

#ifdef _MGHAVE_TEXTEDITTITLE
    if (self->title_idx > 0)
    {
        if(offChar && paraNo == self->title_paracount)
            *offChar -= self->title_last_para_text_index;

        paraNo -= self->title_paracount;
    }
#endif

    if (paraIdx)
        *paraIdx = paraNo;

    return cursor
#ifdef _MGHAVE_TEXTEDITTITLE
        - (self->title_text_idx > 0 ? self->title_text_idx : 0);
#endif
        ;
}

static int mTextEditor_getCaretPosByLine(mTextEditor *self, 
        int *lineIdx, int *offChar, int *height, BOOL isSel)
{
    int cursor, lineNo, i;
    int idx = 0;
    int h = 0;

    mCommBTreeLeafIterator node_it;
    mTextLayoutNode* node;

    if (!TE_VALID_OBJ(self) || !(lineIdx || offChar || height))
        return -1;

#ifdef _MGHAVE_TEXTEDITTITLE
    {
        int total = _c(self->textBuffer)->getCount(self->textBuffer);
        if (total <= self->title_idx)
        {
            *lineIdx = 0;
            *offChar = 0;
            return 0;
        }
    }
#endif

    // cursor is the text not charactors
    cursor = isSel ? pTextLayout->text_sel_begin : pTextLayout->text_cursor;
    if (height)
        *height = isSel ? pTextLayout->sel_begin_height : pTextLayout->cursor_height;
    lineNo = _c(pTextLayout)->getLineByIndex(pTextLayout, cursor, offChar, TRUE);

    texteditor_update_title_line(self);

#ifdef _MGHAVE_TEXTEDITTITLE
    if (self->title_idx > 0)
    {
        if(offChar && lineNo == self->title_linecount)
            *offChar -= self->title_last_line_text_index;
    }
#endif

    // deal with when the cursor in the autowrap line tail or line head
    if (*offChar != 0)
    {
        node = textlayout_get_first_node(pTextLayout);
        (void)initObjectArgs((mObject*)((void*)(&node_it)), 
                (mObjectClass*)((void*)(&Class(mCommBTreeLeafIterator))), 2, pTextLayout, node);

        for (i = 0; i <= lineNo; i++)
        {
            node = (mTextLayoutNode*)node_it.current;
            idx += node->text_count;
            h += node->height;
            _c(&node_it)->next(&node_it);
        }

        if(node && (idx == cursor) 
                && (pTextLayout->cursor_y >= h)) 
        {
            lineNo ++;
            *offChar = 0;
        }
    }

#ifdef _MGHAVE_TEXTEDITTITLE
    if (self->title_idx > 0)
        lineNo -= self->title_linecount;
#endif

    if (lineIdx) 
        *lineIdx = lineNo;

    return cursor
#ifdef _MGHAVE_TEXTEDITTITLE
        - (self->title_text_idx > 0 ? self->title_text_idx : 0);
#endif
        ;
}

static void mTextEditor_selectAll(mTextEditor *self)
{
    if (TE_VALID_OBJ(self)) {
        _te_set_cursor(self, 0, FALSE);
        _te_set_cursor(self, _c(pTextBuffer)->getCount(pTextBuffer), TRUE);
    }
}

static inline BOOL _needCaretWidth(mTextEditor* self)
{
    return (mTextEditor_isReplaceMode(self) &&
            pTextLayout->cursor == pTextLayout->sel_begin);
}

static void _change_caret_size(mTextEditor* self, BOOL show_caret)
{
    if(!ChangeCaretSize(self->hwnd, self->caretWidth, self->caretHeight))
    {
        //recerate one
        DestroyCaret(self->hwnd);
        CreateCaret(self->hwnd, NULL, self->caretWidth, self->caretHeight);
    }

    if (show_caret && !(self->flags & NCS_TEF_NOCARET) && (self->flags & NCS_TEF_FOCUS)) {
        ActiveCaret(self->hwnd);
        ShowCaret(self->hwnd);
    }

}   

static void _update_caret_size(mTextEditor* self)
{
    HDC hdc;
    ITextIterator* txtit = _c(pTextBuffer)->getAt(pTextBuffer, pTextLayout->cursor);
    int width, height;
    ITextRender * render = (txtit)->_vtable->getTextRender(INTEFACE_ADJUST(txtit));
    hdc = GetClientDC(self->hwnd);
    if(!(render)->_vtable->getCaretSize(INTEFACE_ADJUST(render), hdc, txtit, &width, &height)) //the text cannot show an relpace size, can only show as default
    {
        width = 1;
        height = self->caretHeight;
    }
    else if(!_needCaretWidth(self)) //cannot show as replace size
    {
        width = 1; //change width as 1
    }
    ReleaseDC(hdc);

    self->caretWidth = width;
    self->caretHeight = height;

    (txtit)->_vtable->releaseIterator(INTEFACE_ADJUST(txtit));

}



static void _make_caret_visible(mTextEditor *self, 
        int caretX, int caretY, int caretW, int caretH)
{
    int cx, cy;
    int offX, offY;
    BOOL refresh = FALSE;

    if (! mTextEditor_hasPainted(self)) {
        pTextLayout->old_height = -1; /* Fix bug #5606 */
        return;
    }

    //make cursor visible
    _c(pTextLayout)->getTextBoundSize(pTextLayout, &cx, &cy);
    offX = self->offX;
    offY = self->offY;

    if(cy >= self->visHeight) {
        if(caretY < offY || caretH >= self->visHeight)
            offY = caretY;
        else if((caretY+caretH) > (offY + self->visHeight))
            offY = caretY + caretH - self->visHeight;

        if (offY > cy - self->visHeight)
            offY = cy - self->visHeight;
    }
    else {
        offY = 0;
    }
    
    if(cx >= self->visWidth) {
        if(caretX < offX)
            offX = caretX;
        else if((caretX + 1) > (offX + self->visWidth))
            offX = caretX + 1- self->visWidth;

        if (offX > (cx - self->visWidth + 1))
            offX = cx - self->visWidth + 1;
    }
    else {
        offX = 0;
    }

    if (offX != self->offX) {
        int maxx = (cx > self->visWidth ? cx - self->visWidth : 0) + 1;
        if (offX > maxx)
            offX = maxx;

        if (offX >= 0 && offX <= maxx && offX != self->offX) {
            refresh = TRUE;
        }
    }

    if (offY != self->offY) {
        int maxy = cy > self->visHeight ? cy - self->visHeight: 0;
        if (offY > maxy)
            offY = maxy;

        if (offY >= 0 && offY <= maxy && offY != self->offY) {
            refresh = TRUE;
        }
    }

    if (refresh) {
        _scroll_invalidate_window(self, offX, offY);
        _update_height_changed(self, offX, offY);
        _update_scrolled_margins(self, offX, offY);

        self->offX = offX;
        self->offY = offY;
        
    }
    else {
        _update_height_changed(self, self->offX, self->offY);
    }

    _update_scrollbar(self);
    _update_height_changed_margins(self);
    pTextLayout->old_height = -1;
    _update_content_dirty_region(self);
}

static void _update_caret(mTextEditor *self, 
        void (*cb)(mTextEditor*, int, int,int, int))
{
    int posx, posy, height, cursor;

    //update caret size
    _update_caret_size(self);

    //make selected position visible
    cursor = _c(pTextLayout)->getCursor(pTextLayout, &posx, &posy, &height, TRUE);

    if (cb)
        cb(self, posx, posy, self->caretWidth, height);

    //get cursor position
    cursor = _c(pTextLayout)->getCursor(pTextLayout, &posx, &posy, &height, FALSE);

    if (cursor < 0)
        cursor = 0;
    {
        RECT rcView;
        posx -= self->offX;
        posy -= self->offY;
        texteditor_view_to_client(self, &posx, &posy);
        texteditor_get_view_rect(self, &rcView);
        if( posx >= rcView.left && (posx + self->caretWidth) <= rcView.right
                && posy >= rcView.top && (posy + self->caretHeight) <= rcView.bottom)
        {
            //if(self->flags & NCS_TEF_FOCUS)
            //    ShowCaret(self->hwnd);
            SetCaretPos(self->hwnd, posx, posy);
        }
        else
        {
            HideCaret(self->hwnd);
            return ;
        }

    }

    // ChangeCaretSize(self->hwnd, 1, height);
    _change_caret_size(self, TRUE);
}


static void mTextEditor_updateCaret(mTextEditor *self)
{
    if (TE_VALID_OBJ(self))
        _update_caret(self, _make_caret_visible);
}

#define EQUAL_SEL_TO_CURSOR(layout)                        \
    do {                                                   \
        layout->sel_begin_x = layout->cursor_x;            \
        layout->sel_begin_y = layout->cursor_y;            \
        layout->sel_begin_height = layout->cursor_height;  \
        layout->sel_begin = layout->cursor;                \
        layout->text_sel_begin = layout->text_cursor;      \
    } while(0)      

#ifdef _MGHAVE_TEXTEDITTITLE
#define OUT_CURSOR_FORM_TITLE(self)                           \
    do {                                                      \
        if (pTextLayout->cursor_y <= self->title_y) {         \
            pTextLayout->cursor_x = self->title_x;            \
            pTextLayout->cursor_y = self->title_y;            \
            pTextLayout->cursor = self->title_idx;            \
            pTextLayout->text_cursor = self->title_text_idx;  \
        }                                                     \
    } while(0) 
#else
#define OUT_CURSOR_FORM_TITLE(self)     
#endif

static void _update_scroll_region(mTextEditor* self, 
        int scrollVal, int newPos, int code, BOOL bHScroll)
{
    int scrollBoundMax, scrollBoundMin, scroll = 0;
    int nOffset, cx, cy;
    BOOL bScroll = FALSE;

    _c(pTextLayout)->getTextBoundSize(pTextLayout, &cx, &cy);
    
    if (bHScroll) {
        scrollBoundMax = cx - self->visWidth;
        nOffset = self->offX;
    }
    else {
        scrollBoundMax = cy - self->visHeight;
        nOffset = self->offY;
    }
    scrollBoundMin = 0;

    if (newPos >= 0) { 
        scrollVal = newPos - nOffset;
    }

    scroll = (scrollVal > 0 ? scrollVal : -scrollVal);

    if (scrollVal > 0 && nOffset < scrollBoundMax) {
        if ((nOffset + scroll) > scrollBoundMax)
            nOffset = scrollBoundMax;
        else
            nOffset += scroll;
        bScroll = TRUE;
    }
    else if ( scrollVal < 0 && nOffset > scrollBoundMin) {
        if ((nOffset - scroll) < scrollBoundMin)
            nOffset = scrollBoundMin;
        else
            nOffset -= scroll;
        bScroll = TRUE;
    }

    if (bScroll) {
        if (bHScroll) {
            int maxContX = cx - self->visWidth;
            int x = nOffset > maxContX ? maxContX: nOffset;
            if (x >= 0 && self->offX != x) {
                SetScrollPos (self->hwnd, SB_HORZ, x);

                _scroll_invalidate_window(self, x, self->offY);
                _update_height_changed(self, x, self->offY);
                _update_scrolled_margins(self, x, self->offY);
           
                self->offX = x;
                _update_caret(self, NULL);
                
                _update_height_changed_margins(self);
                pTextLayout->old_height = -1;
                _update_content_dirty_region(self);
            }
        }
        else {
            int maxContY = cy - self->visHeight;
            int y = nOffset > maxContY ? maxContY: nOffset;
            if( self->offY != y)
            {
                SetScrollPos (self->hwnd, SB_VERT, y);

                _scroll_invalidate_window(self, self->offX, y);
                _update_height_changed(self, self->offX, y);
                
                _update_scrolled_margins(self, self->offX, y);

                self->offY = y;

                // PAGEUP or PAGEDOWN will move the cursor
                if (TE_IS_CURSORE_PAGE_MOVE(self) && (code == SB_PAGEUP || code == SB_PAGEDOWN)) {
                    pTextLayout->cursor_y += scrollVal; 
                    pTextLayout->text_cursor = _c(pTextLayout)->getCharIndex(pTextLayout, &pTextLayout->cursor_x, 
                            &pTextLayout->cursor_y, &pTextLayout->cursor_height, &pTextLayout->cursor);

                    // cursor can't move to title
                    OUT_CURSOR_FORM_TITLE(self);
                    
                    // move cursor, so cancel selection
                    EQUAL_SEL_TO_CURSOR(pTextLayout);
                    _update_caret(self, _make_caret_visible);
                }
                else {
                    _update_caret(self, NULL);
                    _update_height_changed_margins(self);
                    pTextLayout->old_height = -1;
                    _update_content_dirty_region(self);
                }
            }
        }
    }
    else {
        if ((!bHScroll) && (code == SB_PAGEUP || code == SB_PAGEDOWN)) {
                pTextLayout->cursor_y += scrollVal; 
                pTextLayout->text_cursor = _c(pTextLayout)->getCharIndex(pTextLayout, &pTextLayout->cursor_x, 
                        &pTextLayout->cursor_y, &pTextLayout->cursor_height, &pTextLayout->cursor);
                OUT_CURSOR_FORM_TITLE(self);
                EQUAL_SEL_TO_CURSOR(pTextLayout);
                _update_caret(self, _make_caret_visible);
        }
    }

    TE_CLEAR_CUSROR_PAGE_MOVE(self);
}

static void mTextEditor_onHScroll(mTextEditor *self, int code, int mouseX)
{
    int hScroll = 0, newPos = -1;

    if (!TE_VALID_OBJ(self))
        return;

    switch (code) {
        case SB_LINERIGHT:
            hScroll = self->scrollHStep;
            break;
        case SB_LINELEFT:
            hScroll = -self->scrollHStep;
            break;
        case SB_PAGERIGHT:
            hScroll = self->visWidth - self->scrollHStep;
            break;
        case SB_PAGELEFT:
            hScroll = -(self->visWidth - self->scrollHStep);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            newPos = mouseX;
            break;
    }
    
    _update_scroll_region(self, hScroll, newPos, code, TRUE);
}

static void mTextEditor_onVScroll(mTextEditor *self, int code, int mouseY)
{
    int hScroll = 0, newPos = -1;

    if (!TE_VALID_OBJ(self))
        return;

    switch (code) {
        case SB_LINEDOWN:
            hScroll = self->scrollVStep;
            break;
        case SB_LINEUP:
            hScroll = -self->scrollVStep;
            break;
        case SB_PAGEDOWN:
            hScroll = self->visHeight - self->scrollVStep;
            break;
        case SB_PAGEUP:
            hScroll = -(self->visHeight - self->scrollVStep);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            newPos = mouseY;
            break;
    }
    
    _update_scroll_region(self, hScroll, newPos, code, FALSE);
}

static void mTextEditor_lineDecorative(mTextEditor* self, int x, int y, int w, int h, int lineNo, HDC hdc)
{
    RECT rcview;
#ifdef _MGHAVE_TEXTEDITTITLE
    //TODO FIXEDME lineNo would be not correct if we set the title
    //printf("--linedecorative x=%d, y=%d, title:(x=%d,y=%d, height=%d, idx=%d, text_idx=%d\n", x, y, self->title_x, self->title_y, self->title_last_height, self->title_idx, self->title_text_idx);
    if( y < self->title_y)
        return;
    else if( y >= self->title_y && y < (self->title_y + self->title_last_height))
    {
        x = self->title_x;
        x -= self->offX;
        if(x < 0) x = 0;
    }
    else
        x = 0;
    //printf("--linedecorative x=%d\n", x);
#else
    x = 0;
#endif
    texteditor_get_view_rect(self, &rcview);
    y = y - self->offY + self->margins.top + h - 1;
    //printf("---- print at y=%d<self->offY=%d, self->marigns.top=%d, h=%d, self->visHeight=%d>\n", y, self->offY, self->margins.top, h,self->visHeight);
    if(y > rcview.bottom || y < rcview.top)
        return ;
    //printf("-- lineNo=%d\n", lineNo);
    //TODO hear, draw some information, e.g line No 
#if MARGIN_UPDATE 
    //implement it if need
    if(lineNo >= 0)
    {
        char szbuff[20];
        RECT rcLineNo;
        sprintf(szbuff, "%d", lineNo+1);
        rcLineNo.left = 0;
        rcLineNo.right = self->margins.left - 1;
        rcLineNo.top  = y - h;
        rcLineNo.bottom = y;
        DrawText(hdc, szbuff, -1, &rcLineNo, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

    }
#endif

#if 1
    //TODO : use the user defined handler to draw the lineDecorative
    /*
     if ( self->userLineDecorative )
        self->userLineDecorative( hdc, self->margins.left, y, self->visWidth - 1)
    else
     */
    DrawHDotLine(hdc, self->margins.left + x + (self->offX % 2) ,
            y,
            self->visWidth - x);  
#else
    LineEx(hdc, self->margins.left + x, y, self->margins.left + self->visWidth, y);
#endif
}

static BOOL mTextEditor_needLineDecorative(mTextEditor* self)
{
    return GetWindowStyle(self->hwnd) & NCSS_TE_BASELINE;
}

#ifdef __MINIGUI_LIB__
void TextEditorDrawLinesDecorative(mTextEditor * self, HDC hdc, int x, int y, int w, int h)
#else
void mTextEditor_drawLinesDecorative(mTextEditor * self, HDC hdc, int x, int y, int w, int h)
#endif
{
    mTextLayout * txtLayout = self->textLayout;

    if(!_c(self)->needLineDecorative(self))
        return ;

    if(w <= 0 || h <= 0)
    {
        w = self->visWidth;
        h = self->visHeight;
    }

    _c(txtLayout)->foreachLine(txtLayout, 0, y, w, h, (CB_LINE_PROC)_c(self)->lineDecorative, (mObject*)self, (void*)hdc);

}

static int mTextEditor_wndProc(mTextEditor *self, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_GETTEXTLENGTH:
            return _c(self)->getTextLen(self);

        case MSG_GETTEXT:
            //lParam:string; wParam:len
            return _c(self)->getText(self, (char*)lParam, wParam);

        case MSG_SETTEXT:
            //lParam:string; wParam:meaningless
            return _c(self)->setText(self, (const char*)lParam);

        case MSG_SETCURSOR:
            if (GetWindowStyle(self->hwnd) & WS_DISABLED) {
                SetCursor (GetSystemCursor (IDC_ARROW));
                return 0;
            }
            break;

        case MSG_SETFOCUS:
            _c(self)->onSetFocus(self, (HWND)wParam, lParam);
            break;

        case MSG_KILLFOCUS:
            _c(self)->onKillFocus(self, (HWND)wParam, lParam);
            break;

        case MSG_DOESNEEDIME:
            if ((GetWindowStyle(self->hwnd) & NCSS_TE_READONLY) || (GetWindowExStyle(self->hwnd)&NCSS_EX_UNNEEDIME))
                return FALSE;
            return TRUE;

        case MSG_LBUTTONDBLCLK:
            _c(self)->onLButtonDBClk(self, LOSWORD(lParam), HISWORD(lParam), wParam);
            break;

        case MSG_SIZECHANGED:
            {
                RECT* rc = (RECT*)lParam;
                if (rc)
                    _c(self)->onCSizeChanged(self, RECTWP(rc), RECTHP(rc));
            }
            return 0;
#if 0
        case MSG_CSIZECHANGED:
            _c(self)->onCSizeChanged(self, wParam, lParam);
            return 0;
#endif

        case MSG_CHAR:
            return _c(self)->onChar(self, wParam, lParam);

        case MSG_UTF8CHAR:
            _ERR_PRINTF ("CONTROL>EDIT: MSG_UTF8CHAR is not implemented.\n");
            break;

        case MSG_FONTCHANGED:
            _c(self)->onFontChanged(self);
            return 0;

        case MSG_HSCROLL:
            _c(self)->onHScroll(self, wParam, lParam);
            break;

        case MSG_VSCROLL:
            _c(self)->onVScroll(self, wParam, lParam);
            break;

    }


    return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

#ifdef _MGHAVE_TEXTEDITTITLE
static void mTextEditor_setTitle(mTextEditor* self, const char* buff, int len)
{
    //repalce
    int slen ;

    if(!(GetWindowStyle(self->hwnd) & NCSS_TE_TITLE))
        return ;

    slen = buff ? strlen(buff) : -1;
    if(len <= 0 || len > slen)
        len = slen;
    if(len <= 0)
        len = -1;

    _c(pTextBuffer)->replace(pTextBuffer, 0, self->title_idx, buff, len);
    if(buff == NULL || len <= 0)
    {
        self->title_idx = -1;

    }
    else{
        self->title_idx = len;
    } 

    texteditor_update_title(self);

}

static int mTextEditor_getTitle(mTextEditor* self, char* buff, int max)
{
    if(GetWindowStyle(self->hwnd) & NCSS_TE_TITLE)
        return _c(pTextBuffer)->getText(pTextBuffer, 0, buff, self->title_idx > max ? max : self->title_idx); 
    else
    {
        if(buff)
            buff[0] = 0;
        return 0;
    }
}
#endif

#if defined(__MGNCS_LIB__) && defined(_MGNCS_GUIBUILDER_SUPPORT)
static BOOL mTextEditor_refresh(mTextEditor* self)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    int   scrollmode;
    if(dwStyle & NCSS_TE_UPPERCASE)
        _c(self->textBuffer)->setCase(self->textBuffer, 0,-1, TRUE);
    else if(dwStyle & NCSS_TE_LOWERCASE)
        _c(self->textBuffer)->setCase(self->textBuffer, 0, -1, FALSE);

    //reset the scrollmode
    scrollmode = self->scrollBarMode;
    self->scrollBarMode = -1;
    _c(self)->setProperty(self, NCSP_TE_SCROLL_MODE, scrollmode);

    if(dwStyle &(NCSS_TE_LOWERCASE|NCSS_TE_LOWERCASE))
        InvalidateRect(self->hwnd, NULL, TRUE);

    return TRUE;
}
#endif

BEGIN_CMPT_CLASS(mTextEditor, mWidget)
    IMPLEMENT(mTextEditor, ITextLayoutContext)
    CLASS_METHOD_MAP(mTextEditor, construct)
    CLASS_METHOD_MAP(mTextEditor, destroy)
    CLASS_METHOD_MAP(mTextEditor, wndProc)
    CLASS_METHOD_MAP(mTextEditor, onCreate)
    CLASS_METHOD_MAP(mTextEditor, setProperty)
    CLASS_METHOD_MAP(mTextEditor, getProperty)
    CLASS_METHOD_MAP(mTextEditor, getMetrics)
    CLASS_METHOD_MAP(mTextEditor, beginSelection)
    CLASS_METHOD_MAP(mTextEditor, endSelection)
    CLASS_METHOD_MAP(mTextEditor, setupDC)
    CLASS_METHOD_MAP(mTextEditor, getDC)
    CLASS_METHOD_MAP(mTextEditor, releaseDC)
    CLASS_METHOD_MAP(mTextEditor, contentPosToDCPos)
    CLASS_METHOD_MAP(mTextEditor, update)
    CLASS_METHOD_MAP(mTextEditor, updateCaret)
    
    //message
    CLASS_METHOD_MAP(mTextEditor, onChar)
    CLASS_METHOD_MAP(mTextEditor, onKeyDown)
    CLASS_METHOD_MAP(mTextEditor, onSetFocus)
    CLASS_METHOD_MAP(mTextEditor, onKillFocus)
    CLASS_METHOD_MAP(mTextEditor, onPaint)
    CLASS_METHOD_MAP(mTextEditor, onEraseBkgnd)
    CLASS_METHOD_MAP(mTextEditor, getInvalidBkgnd)
    CLASS_METHOD_MAP(mTextEditor, onFontChanged)
    CLASS_METHOD_MAP(mTextEditor, onLButtonDown)
    CLASS_METHOD_MAP(mTextEditor, onLButtonUp)
    CLASS_METHOD_MAP(mTextEditor, onLButtonDBClk)
    CLASS_METHOD_MAP(mTextEditor, onMouseMove)
    CLASS_METHOD_MAP(mTextEditor, onCSizeChanged)
    CLASS_METHOD_MAP(mTextEditor, onVScroll)
    CLASS_METHOD_MAP(mTextEditor, onHScroll)
    CLASS_METHOD_MAP(mTextEditor, setCaretPosByLine)
    CLASS_METHOD_MAP(mTextEditor, setCaretPosByPara)
    CLASS_METHOD_MAP(mTextEditor, getCaretPosByPara)
    CLASS_METHOD_MAP(mTextEditor, getCaretPosByLine)
    //CLASS_METHOD_MAP(mTextEditor, drawBkgnd)
    CLASS_METHOD_MAP(mTextEditor, getSel)
    CLASS_METHOD_MAP(mTextEditor, setSel)
    SET_DLGCODE(DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS | DLGC_WANTENTER);

    CLASS_METHOD_MAP(mTextEditor, setText)
    CLASS_METHOD_MAP(mTextEditor, getText)
    CLASS_METHOD_MAP(mTextEditor, getTextLen)
    CLASS_METHOD_MAP(mTextEditor, insertText)
    CLASS_METHOD_MAP(mTextEditor, getParaText)
    CLASS_METHOD_MAP(mTextEditor, getLineText)
    CLASS_METHOD_MAP(mTextEditor, getParaLength)
    CLASS_METHOD_MAP(mTextEditor, getParaLengthInMChar)
    CLASS_METHOD_MAP(mTextEditor, getLineLength)
    CLASS_METHOD_MAP(mTextEditor, getParaNum)
    CLASS_METHOD_MAP(mTextEditor, enableCaret)
    CLASS_METHOD_MAP(mTextEditor, getLineCount)
    CLASS_METHOD_MAP(mTextEditor, selectAll)
    CLASS_METHOD_MAP(mTextEditor, needLineDecorative)
    CLASS_METHOD_MAP(mTextEditor, lineDecorative)

#ifdef _MGHAVE_TEXTEDITTITLE
    CLASS_METHOD_MAP(mTextEditor, setTitle)
    CLASS_METHOD_MAP(mTextEditor, getTitle)
#endif

#if defined(__MGNCS_LIB__) && defined(_MGNCS_GUIBUILDER_SUPPORT)
    CLASS_METHOD_MAP(mTextEditor, refresh)
#endif
END_CMPT_CLASS

///////////////////////////////////////////
#ifdef __MINIGUI_LIB__
static void init_textedit(void)
{
    MGNCS_INIT_CLASS(mObject);
    MGNCS_INIT_CLASS(mWidget);
    MGNCS_INIT_CLASS(mCommBTreeNode);
    MGNCS_INIT_CLASS(mCommBTree);
    MGNCS_INIT_CLASS(mCommBTreeLeafIterator);
    MGNCS_INIT_CLASS(mTextBuffer);
    MGNCS_INIT_CLASS(mTextIterator);
    MGNCS_INIT_CLASS(mTextLayout);
    MGNCS_INIT_CLASS(mTextLayoutNode);
    MGNCS_INIT_CLASS(mTextRender);

    MGNCS_INIT_CLASS(mTextEditor);
}

/*======== The following is textedit control implementation. =========*/
static LRESULT mTextEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    mTextEditor *self = NULL;

    if (message != MSG_CREATE)
        self = (mTextEditor*) GetWindowAdditionalData2 (hWnd);

    switch(message)
    {
        case MSG_CREATE:
            if (!(self = NEWEX(mTextEditor, (DWORD)hWnd)))
                return -1;

            SetWindowAdditionalData2 (hWnd, (DWORD)self);
            break;

        case MSG_DESTROY:
            if (self)
                DELETE(self);
            SetWindowAdditionalData2 (hWnd, 0);
            return 0;

        case EM_INSERTTEXT:
            //wParam:len; lParam:string
            return _c(self)->insertText(self, (const char*)lParam, wParam);

        case EM_SETMARGINS:
            {
                return _c(self)->setProperty(self, NCSP_TE_MARGINRECT, (DWORD)lParam);
            }
        case EM_GETMARGINS:
            {
                RECT *prcMargins = (RECT*)lParam;
                if(!prcMargins)
                    return 0;
                *prcMargins = self->margins;
                return 0;
            }

        case TEM_GETBKGNDINVALID:
            return _c(self)->getInvalidBkgnd(self, (RECT*)lParam);


        case TEM_RESETCONTENT:
            return _c(self)->setText(self, "");

        case EM_GETLINECOUNT:
            return _c(self)->getLineCount(self);

        case EM_SETREADONLY:
            return _c(self)->setProperty(self, NCSP_TE_READONLY, wParam);

        case EM_INSERTCBTEXT:
        {
            if (GetWindowStyle(hWnd) & NCSS_TE_READONLY)
                return 0;
            return _c(self)->onKeyDown(self, SCANCODE_V, KS_CTRL);
        }
        case EM_COPYTOCB:
        case EM_CUTTOCB:
            return _c(self)->onKeyDown(self, message == EM_CUTTOCB ? SCANCODE_X : SCANCODE_C, KS_CTRL);

        case EM_GETSEL:
        {
            char *buffer = (char*)lParam;
            int len = (int)wParam;
            return _c(self)->getSel(self, buffer, len);
        }

        case EM_GETSELPOS:
        case EM_GETCARETPOS:
            return _c(self)->getCaretPosByPara(self, (int*)wParam, (int*)lParam, 
                    NULL, message == EM_GETSELPOS);

        case EM_SETCARETPOS:
        case EM_SETSELPOS:
            return _c(self)->setCaretPosByPara(self, wParam, lParam, (EM_SETSELPOS == message));

        case EM_SETCARETPOSBYLINE:
        case EM_SETSELPOSBYLINE:
            return _c(self)->setCaretPosByLine(self, wParam, lParam, (EM_SETSELPOSBYLINE == message));

        case EM_GETCARETPOSBYLINE:
        case EM_GETSELPOSBYLINE:
            return _c(self)->getCaretPosByLine(self, (int*)wParam, (int*)lParam, 
                    NULL, message == EM_GETSELPOSBYLINE);

        case EM_REFRESHCARET:
            SetCaretPos(self->hwnd, pTextLayout->cursor_x, pTextLayout->cursor_y);
            return 0;

        case EM_LIMITTEXT:
            return _c(self)->setProperty(self, NCSP_TE_TEXTLIMIT, wParam) ? 0 : -1;

        case EM_GETLIMITTEXT:
            return self->hardLimit;

        case EM_GETLINEHEIGHT:
            return _c(pTextLayout)->getLineHeight(pTextLayout, 0);

        case EM_GETTITLETEXT:
#ifdef _MGHAVE_TEXTEDITTITLE
            return _c(self)->getTitle(self, (char*)lParam, (int)wParam);
#endif

        case EM_SETTITLETEXT:
#ifdef _MGHAVE_TEXTEDITTITLE
            _c(self)->setTitle(self, (const char*)lParam, (int)wParam);
#endif
            return 0;

        case EM_CHANGECARETSHAPE:
            if(wParam == ED_CARETSHAPE_LINE && mTextEditor_isReplaceMode(self))
            {
                self->flags &= ~NCS_TEF_REPLACE_MODE;
                _update_caret_size(self);
            }
            else if(wParam == ED_CARETSHAPE_BLOCK && !mTextEditor_isReplaceMode(self))
            {
                self->flags |= NCS_TEF_REPLACE_MODE;
                _update_caret_size(self);
            }
            return 0;

        case EM_ENABLECARET:
            _c(self)->enableCaret(self, wParam);
            return 0;

        case EM_GETNUMOFPARAGRAPHS:
            return _c(self)->getParaNum(self); 

        case EM_GETPARAGRAPHTEXT:
        {
            //wParam: TEXTPOSINFO
            TEXTPOSINFO *info = (TEXTPOSINFO*)wParam;
            if (info)
                return _c(self)->getParaText(self, info->paragraph_index,
                        info->start_pos, info->copy_len, info->buff);
            return 0;
        }

        case EM_GETLINETEXT:
        {
            //wParam: TEXTPOSINFO
            TEXTPOSINFO *info = (TEXTPOSINFO*)wParam;
            if (info)
                return _c(self)->getLineText(self, info->line_index,
                        info->start_pos, info->copy_len, info->buff);
            return 0;
        }

        case EM_GETPARAGRAPHLENGTH:
            //wParam:paragraph index
            return _c(self)->getParaLength(self, wParam, NULL);

        case EM_GETPARAGRAPHLENGTHINMCHAR:
            //wParam:paragraph index
            return _c(self)->getParaLengthInMChar(self, wParam, NULL);

        case EM_GETLINELENGTH:
        case EM_GETLINELENGTHINMCHAR:
            //wParam:line index
            return _c(self)->getLineLength(self, wParam, NULL, message == EM_GETLINELENGTHINMCHAR);

        case EM_SELECTALL:
            _c(self)->selectAll(self);
            return 0;

        //TODO: unsupport message
        case EM_SETLINEHEIGHT:
        case EM_SETLFDISPCHAR:
        case EM_SETLINESEP:
        case EM_SETDRAWSELECTFUNC:
        case EM_SETGETCARETWIDTHFUNC:
            break;

        case EM_UNDO:
        case EM_REDO:
            return 0;
    }

    if(self)
        return _c(self)->wndProc(self, message, wParam, lParam);
    else
        return DefaultControlProc (hWnd, message, wParam, lParam);
}

extern int AddNewControlClass (PWNDCLASS pWndClass);
BOOL RegisterTextEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_TEXTEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = mTextEditCtrlProc;

    init_textedit();

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    WndClass.spClassName = CTRL_MLEDIT;

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    WndClass.spClassName = CTRL_MEDIT;

    return AddNewControlClass (&WndClass) == ERR_OK;
}
#endif /* __MINIGUI_LIB__ */
#endif /* defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(_MGNCS_TEXTEDITOR) */
