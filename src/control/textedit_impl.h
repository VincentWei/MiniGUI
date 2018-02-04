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
** textedit.h: header file of TextEdit control.
**
*/

#ifndef _TEXTEDIT_IMPL_H_
#define _TEXTEDIT_IMPL_H_


#ifdef __cplusplus
extern "C"
{
#endif


/* ---------------------- configuration options ----------------------- */

/* supports text selection */
#define _SELECT_SUPPORT     1

/* supports text undo/redo operations */
#undef _UNDO_SUPPORT
/* #define _UNDO_SUPPORT       1 */

/* support title text */
#define _TITLE_SUPPORT          1

/* -------------------------------------------------------------------- */

/* default base line height */
#define DEF_LINE_BASE_H         ( (GetWindowStyle(hWnd) & TES_BASELINE) ? 2 : 0 )
/* default line above height */
#define DEF_LINE_ABOVE_H        4
/* minimal line above height */
#define MIN_LINE_ABOVE_H        1
/* default line height */
#define DEF_LINE_HEIGHT         ( GetWindowFont(hWnd)->size + DEF_LINE_BASE_H + \
                                 DEF_LINE_ABOVE_H )

/* default line buffer size */ 
#define DEF_LINE_BUFFER_SIZE    128
/* default line step block size */
#define DEF_LINE_BLOCK_SIZE     32

/* default line seperator '\n' */
#define DEF_LINESEP             '\n'

#define CH_RN                   "\r\n"

/* default title length */
#define DEF_TITLE_LEN           63


/* draw text format */
#define TE_FORMAT  (DT_LEFT | DT_TOP | DT_NOCLIP | DT_EXPANDTABS) 

#ifdef _UNDO_SUPPORT
#define DEF_UNDO_DEPTH          3

/* undo operation types */
#define UNDO_OP_EDIT            1
#define UNDO_OP_ADD             2
#define UNDO_OP_DEL             3

#endif

/* -------------------------------------------------------------------- */

/* internal status flags */
#define TEST_SELECT     0x0001
#define TEST_FOCUSED    0x0002
#define TEST_TMP        0x0004
#define TEST_REPLACE    0x0008
#define TEST_MOVE       0x0010
#define TEST_NOCARET    0x0020
#define TEST_EX_SETFOCUS   0x0040
#define TEST_EX_KILLFOCUS  0x0080

/* ------------------------------- text document/buffer ------------------------ */

/* content type, reserved */
typedef enum {
    CT_TEXT,
    CT_BMP,
    CT_ICON
} ContentType;

/* text format, reserved */
typedef enum {
    TF_PLAINTEXT,
    TF_RICHTEXT,
    TF_MEDIATEXT
} TextFormat;

/* one text line (end with '\n') is a scrollview item */ 
/* structure of text node/line */
typedef struct _textnode
{
    list_t    list;            /* list element */
    StrBuffer content;
#if 0
    ContentType type;        /* text node content type, reserved */
#endif
    DWORD     addData;         /* for storing scrollview item handle */
} TextNode;

/* structure of a text mark, recording insert position or selection range */
typedef struct _textmark
{
    int       pos_lnOff;       /* mark offset in the text node*/
    TextNode  *curNode;        /* text node containing the mark*/
} TextMark;

/* structure of text document data */
typedef struct _textdoc
{
    list_t    queue;           /* text line/node head */

    /* setup field */
    unsigned char lnsep;       /* line seperator (default is "\n") */
    int       nDefLineSize;    /* default line buffer size*/
    int       nBlockSize;      /* line buffer block size */
#if 0
    TextFormat txtFormat;    /* text format, reserved */
#endif

    /* node init function executed when creating node */
    void      (*init_fn)     (struct _textdoc *, TextNode *, TextNode *);
    /* node change function executed when changing current insertion/selection node */
    void      (*change_fn)   (struct _textdoc *, BOOL bSel);
    /* node content change function executed when string content of a node is changed */
    void      (*change_cont) (struct _textdoc *, TextNode *node);
    /* node destroy function */
    void      (*del_fn)      (struct _textdoc *, TextNode *);
    void       *fn_data;       /* data passed to init_fn */

    /* status field */
    TextMark  insert;          /* cursor/insertion mark */
#ifdef _SELECT_SUPPORT
    TextMark  selection;       /* selection mark */
#endif

} TextDoc;

/* -------------------------------------------------------------------------- */

#ifdef _UNDO_SUPPORT

#define ACTION_COUNT    5

/* backup data struct */
typedef struct _status_data
{
    DWORD     flags;        /* editor status */

    int       des_caret_x;  /* the desired caret x position, changed by mouse and <- , -> */

    int       caret_x;      /* caret x position in the virtual content window */
    int       caret_y;      /* caret y position in the virtual content window */
#ifdef _SELECT_SUPPORT
    int       sel_x;        /* selection point x position in the virtual content window */
    int       sel_y;        /* selection point y position in the virtual content window */
#endif

    TextNode  *maxNode;     /* node with the max length */
    int       maxLen;       /* max line length */
    TextNode  *secNode;     /* node with the second max length */
    int       secLen;       /* seconde max line length */

    int       curItemY;     /* y position of the current insertion node */
#ifdef _SELECT_SUPPORT
    int       selItemY;     /* y position of the selection node */
#endif
} STATDATA;
typedef STATDATA* PSTATDATA;

/* structure of backup text node/line */
typedef struct _bk_textnode
{
    list_t    list;         /* list element */
    StrBuffer content;      /* text content */
} BkNode;

typedef struct _backup_data
{
    STATDATA  statData;
    TextMark  bkIns;        /* backup insertion point */
    TextMark  bkSel;        /* backup selection point */
    int       opType;       /* operation type */
    list_t    bkQueue;      /* backup text nodes */
} BKDATA;
typedef BKDATA* PBKDATA;

#endif

/* structure of textedit control data */
typedef struct _tedata
{
    SVDATA    svdata;       /* scrollview object */
    TextDoc   txtdoc;       /* text document object */

    StrBuffer tebuff;       /* temp text content */
    int       nr_diff_line; /*first different line*/
    int       w_nochanged; /*width not changed of first different line*/
    int       w_lastline;  /* width of last line*/
    int       hardLimit;    /* hard limit */
    int       curlen;       /* current tedata's len, not include char '\n' */

    /* ----------------- properties ------------------- */

    int       nLineHeight;  /* line height */
    int       nLineAboveH;  /* height of above-line area */
    int       nLineBaseH;   /* height of base-line area */
    unsigned char lnChar;   /* the char used to represent line seperator */
    unsigned char caretShape; /* shape of the caret */

#ifdef _TITLE_SUPPORT
    char      *title;       /* title text displayed before content text */
    int       titleIndent;  /* title indent */
#endif

    /* ----------------- status ----------------------- */

    /* don't move this field */
    DWORD     flags;        /* editor status */
    DWORD     ex_flags;        /* editor extends status */
    BOOL      cont_dirty;    /*content changed flags in text field*/

    int       des_caret_x;  /* the desired caret x position, changed by mouse and <- , -> */

    int       caret_x;      /* caret x position in the virtual content window */
    int       caret_y;      /* caret y position in the virtual content window */
#ifdef _SELECT_SUPPORT
    int       sel_x;        /* selection point x position in the virtual content window */
    int       sel_y;        /* selection point y position in the virtual content window */
#endif

    TextNode  *maxNode;     /* node with the max length */
    int       maxLen;       /* max line length */
    TextNode  *secNode;     /* node with the second max length */
    int       secLen;       /* seconde max line length */

    int       curItemY;     /* y position of the current insertion node */
#ifdef _SELECT_SUPPORT
    int       selItemY;     /* y position of the selection node */
#endif

#ifdef _UNDO_SUPPORT
    BKDATA    *bkData;      /* backup status data */
    int       undo_level;   /* current undo level */
    int       cur_undo_depth; /* current undo depth */
    int       undo_depth;   /* depth of undo stack */
    int       act_count;    /* user action count */
#endif

    HDC       buf_dc;
    ED_DRAWSEL_FUNC drawSelected;
    int       (*getCaretWidth) (HWND hWnd, int def_width);
} TEDATA;
typedef TEDATA* PTEDATA;


/* -------------------------------------------------------------------------- */

/* alloc for text node */
/* TODO */
static inline TextNode* textnode_alloc (void)
{
    return malloc (sizeof(TextNode));
}

static inline void textnode_free (TextNode * node)
{
    free (node);
}

#define ptescr          (&ptedata->svdata.scrdata)

/* gets the current caret position in the virtual content window */
#define myGetCaretPos(pt)  \
            ( (pt)->x = ptedata->caret_x, \
              (pt)->y = ptedata->caret_y )

#ifdef _SELECT_SUPPORT

#define myGetSelPos(pt) \
            ( (pt)->x = ptedata->sel_x, \
              (pt)->y = ptedata->sel_y )
#define mySetSelPos(x, y) \
        ( ptedata->sel_x = x >= 0 ? x : ptedata->sel_x, \
          ptedata->sel_y = y >= 0 ? y : ptedata->sel_y )

#endif


#define BE_WRAP(hWnd)     (GetWindowStyle(hWnd) & TES_AUTOWRAP)

#define BE_FIRST_NODE(node) \
           (node->list.prev == &txtdoc->queue)

#define BE_LAST_NODE(node) \
           (node->list.next == &txtdoc->queue)

#define FIRSTNODE(txtdoc) \
           (list_entry((txtdoc)->queue.next, TextNode, list))
#define LASTNODE(txtdoc) \
           (list_entry((txtdoc)->queue.prev, TextNode, list))

#define NODE_LINENR(node) \
        ( scrollview_get_item_height((HSVITEM)(node)->addData) / ptedata->nLineHeight )

#define NODE_HEIGHT(node) \
        (scrollview_get_item_height((HSVITEM)(node)->addData))

#define NODE_INDEX(node) \
        (scrollview_get_item_index(hWnd, (HSVITEM)(node)->addData))

static inline int teGetLineIndent (PTEDATA ptedata, TextNode *node)
{
#ifdef _TITLE_SUPPORT
    if (node && node->list.prev == &ptedata->txtdoc.queue) {
        return ptedata->titleIndent;
    }
#endif
    return 0;
}

#define TXTNODE_NEXT(node) \
            ( ((node)->list.next == &txtdoc->queue) ? NULL : \
                list_entry((node)->list.next, TextNode, list) )

#define TXTNODE_PREV(node) \
            ( ((node)->list.prev == &txtdoc->queue) ? NULL : \
                list_entry((node)->list.prev, TextNode, list) )

 
#ifdef _SELECT_SUPPORT
static BOOL textnode_is_selected (TextDoc *txtdoc, TextNode *node)
{
    if (!txtdoc->selection.curNode)
        return FALSE;

    return scrollview_is_item_selected ((HSVITEM)node->addData);
}

#define NODE_IS_SELECTED(node) \
            scrollview_is_item_selected ((HSVITEM)node->addData)

#define SELECT_NODE(node, bsel) \
            scrollview_select_item (&ptedata->svdata, (HSVITEM)(node)->addData, bsel)

#endif


#define is_rn_sep(pIns) (txtdoc->lnsep == '\n' && *(pIns) == '\r' && *(pIns+1) == '\n')

#define check_caret() \
            if (txtdoc->selection.curNode) { \
                if (txtdoc->selection.curNode == txtdoc->insert.curNode && \
                      txtdoc->selection.pos_lnOff == txtdoc->insert.pos_lnOff) { \
                    ShowCaret (hWnd); \
                    txtdoc->selection.curNode = NULL; \
                } \
                else \
                    HideCaret (hWnd); \
            } \
            else \
                ShowCaret (hWnd);

#define REFRESH_NODE_EX(node, rcInv) \
            scrollview_refresh_item_ex(&ptedata->svdata, \
                    (HSVITEM)(node)->addData, rcInv)

#define REFRESH_NODE(node) \
            scrollview_refresh_item(&ptedata->svdata, (HSVITEM)(node)->addData)

static int
textdoc_insert_string_ex (TextDoc *txtdoc, TextNode *enode, int insert_pos,
                          const char* newtext, int len);

static inline int
textdoc_insert_string_ex_2 (TextDoc *txtdoc, TextNode *enode, int insert_pos,
                          const char* newtext, int len)
{
    TextNode *node=NULL;
    BOOL bDel;
    int ret;

    bDel = (txtdoc->selection.curNode && !enode) ? TRUE : FALSE;

    /* must use enode here */
    ret = textdoc_insert_string_ex (txtdoc, enode, insert_pos, newtext, len);
    if (!enode)
        txtdoc->insert.pos_lnOff = ret;

    /*when newtext is '\b' and all text is selected ,
     * txtdoc->insert.curNode change after call 
     * funciton textdoc_insert_string_ex*/
    node = enode ? enode : txtdoc->insert.curNode;

    if (txtdoc->change_cont && node) {
        txtdoc->change_cont (txtdoc, node);
    }

    return !bDel;
}

#define textdoc_insert_string(txtdoc, newtext, len)  \
             textdoc_insert_string_ex_2(txtdoc, NULL, 0, newtext, len)


#ifdef _SELECT_SUPPORT
    #define GETMARK(cond)     ( (cond) ? (&txtdoc->selection) : \
                                      (&txtdoc->insert) )
    #define SETITEMY(cond, item_y)    \
                if (cond) \
                    ptedata->selItemY = item_y; \
                else \
                    ptedata->curItemY = item_y;

#else
    #define GETMARK(cond)     ((cond)? NULL : &txtdoc->insert)
    #define SETITEMY(cond, item_y)    \
                if (!cond) \
                    ptedata->curItemY = item_y;
#endif

#ifdef _UNDO_SUPPORT
    #define BACKUP_STDATA(stdata) \
                memcpy (stdata, (void*)&ptedata->flags, sizeof(STATDATA));
    #define UNDO_STDATA(stdata) \
                memcpy ((void*)&ptedata->flags, stdata, sizeof(STATDATA));
#endif


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TEXTEDIT_IMPL_H_ */

