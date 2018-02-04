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
** textedit.c: text edit control
**
** Create date: 2004/03/01
**
** Note:
**    the textedit control is written from scratch
**    to replace the buggy medit control.
**
**    The textedit control inherits scrollview.
**
** TODO:
** tab
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#if defined(_MGCTRL_TEXTEDIT) && !defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL)
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "ctrl/textedit.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "scrolled.h"
#include "scrollview_impl.h"
#include "text.h"
#include "textedit_impl.h"

#ifdef _UNDO_SUPPORT
static void teUndoBackup (TextDoc *txtoc);
#endif

#define TEXTEDIT_BUFDC 1

#undef _TEXTEDIT_DEBUG
/* #define _TEXTEDIT_DEBUG */

#ifndef TEXTEDIT_BUFDC 
#define GETDC(hWnd)           GetClientDC(hWnd)
#define RELEASEDC(hWnd, hdc)  ReleaseDC(hdc)
#else
inline HDC GETDC(HWND hWnd)
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    if (!ptedata)
        return GetClientDC(hWnd);

    SelectClipRect(ptedata->buf_dc, NULL);
    return ptedata->buf_dc;
}

inline void RELEASEDC(HWND hWnd, HDC hdc)  
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    if (ptedata && ptedata->buf_dc != hdc)
        ReleaseDC(hdc);
}
#endif
#ifdef _TEXTEDIT_DEBUG
static void dump_text (TextDoc *txtdoc, BOOL bSel)
{
    list_t *me;
    TextNode *node;
    SIZE txtsize;
    HWND hWnd = (HWND)txtdoc->fn_data;
    HDC hdc;

    printf ("\n\n\n\n");
    printf ("------------------------------------------------------\n");
    list_for_each (me, &txtdoc->queue) {
        node  = list_entry (me, TextNode, list);
        /*
        if (scrollview_is_item_selected ((HSVITEM)node->addData)) {
            printf ("sv select---<< %s\n", node->content.string);
        }
        */
#ifdef _SELECT_SUPPORT
        if (bSel && textnode_is_selected(txtdoc, node)) {
            printf ("%d:\n", node->content.txtlen);
            printf ("%s----->", node->content.string);
        }
#endif
    }
    hdc = GETDC (hWnd);
    GetTabbedTextExtent(hdc, "\t", 1, &txtsize);
    RELEASEDC (hWnd, hdc);
    printf ("tab size = %d\n", txtsize.cx);
}

static void print_selected (TextDoc *txtdoc)
{
    dump_text (txtdoc, TRUE);
}

#endif


/* ------------------------------ text document/buffer ------------------------ */

/* 
 * set_current_node : Sets a node as the current insertion/selection node,
 *                    must be called when the current node is/will be changed.
 * Params           : newnode - the new node with insertion/selection point
 *                    bSel    - insertion or selection
 *                    bChange - Whether to call change function
 * Return           : TRUE on changed, FALSE otherwise.
 */
static BOOL
set_current_node (TextDoc *txtdoc, TextNode *newnode, BOOL bSel, BOOL bChange)
{
    TextMark *mark;
    TextNode *oldnode;

    mark = GETMARK(bSel);
    oldnode = mark->curNode;

    if (newnode == oldnode)
        return FALSE;

    mark->curNode = newnode;
    mark->pos_lnOff = 0;

    /* called when the current insertion node is changed */
    if (bChange && txtdoc->change_fn)
        txtdoc->change_fn (txtdoc, bSel);

    return TRUE;
}

/* 
 * textnode_create: creat a new text node and initialize it with text
 */
static TextNode* textnode_create (TextDoc *txtdoc, const char *line, int len)
{
    TextNode *newnode;

    if ( !(newnode = textnode_alloc ()) )
        return NULL;

    /* create a new blank line */
    if (!line || len < 0) len = 0;

    if ( !(testr_alloc (&newnode->content, len, txtdoc->nBlockSize)) ) {
        textnode_free (newnode);
        return NULL;
    }

    testr_setstr (&newnode->content, line, len);
    newnode->addData = 0;

    return newnode;
}

/* 
 * textnode_destroy: destroy a text node
 */
static void textnode_destroy (TextNode *node)
{
    if (node) {
        list_del (&node->list);
        testr_free (&node->content);
        textnode_free (node);
    }
}

/* 
 * textdoc_free : free TextDoc nodes
 * Description  : only changes the status fields of a TextDoc object, does not 
 *                affect the properties.
 */
static void textdoc_free (TextDoc *txtdoc)
{
    TextNode *node;
    if (!txtdoc) return;

    while (!list_empty(&txtdoc->queue)) {
        node = list_entry (txtdoc->queue.next, TextNode, list);
        textnode_destroy (node);
    }
    txtdoc->insert.pos_lnOff = 0;
    txtdoc->insert.curNode = NULL;
#ifdef _SELECT_SUPPORT
    txtdoc->selection.curNode = 0;
    txtdoc->selection.pos_lnOff = 0;
#endif
}

/*
 * txtAddNode : add a textnode after a specified node
 * params     : node - the previous text node, if NULL, the new node will be
 *                     inserted at the tail.
 */
static TextNode*
txtAddNode (TextDoc *txtdoc, const char*pLine, int len, TextNode *node)
{
    TextNode *newnode;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2((HWND)txtdoc->fn_data);

    if ( !(newnode = textnode_create (txtdoc, pLine, len)) )
        return NULL;

    if (node)
        list_add (&newnode->list, &node->list);
    else
        list_add_tail (&newnode->list, &txtdoc->queue);

    if (txtdoc->init_fn) txtdoc->init_fn(txtdoc, newnode, node);

    if (newnode)
        ptedata->curlen += newnode->content.txtlen;

    return newnode;
}

/*
 * txtDelNode : deletes a text node
 */
static void txtDelNode (TextDoc *txtdoc, TextNode *node)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2((HWND)txtdoc->fn_data);
    ptedata->curlen -= node->content.txtlen;

    /* deletes scrollview item */
    if (txtdoc->del_fn)
        txtdoc->del_fn(txtdoc, node);

    textnode_destroy (node);
}

/*
 * textdoc_get_textlen : gets the total length of the text document
 */
static int textdoc_get_textlen (TextDoc *txtdoc)
{
    list_t *me;
    TextNode *node;
    int total_len = 0;

    list_for_each (me, &txtdoc->queue) {
        node = list_entry (me, TextNode, list);
        total_len += node->content.txtlen;
    }
    return total_len;
}

/*
 * textdoc_gettext : get text string from text document
 */
static int textdoc_gettext (TextDoc *txtdoc, int len, unsigned char *buffer)
{
    list_t *me;
    TextNode *node;
    unsigned char *pch = buffer;
    int total_len = 0, copy_len = 0;

    if (!buffer || len <= 0)
        return 0;

    list_for_each (me, &txtdoc->queue) {
        node = list_entry (me, TextNode, list);
        copy_len = MIN(node->content.txtlen, len - total_len);
        if (copy_len <= 0) break;
        memcpy (pch, node->content.string, copy_len);
        pch += copy_len;
        total_len += copy_len;
    }
    *pch = '\0';
    return total_len;
}

static int textdoc_get_paragraph_length (PLOGFONT log_font, TextDoc *txtdoc, int index)
{
    list_t *me;
    TextNode *node;
    int i = 0;
    int count;

    if (index < 0)
        return -1;

    list_for_each (me, &txtdoc->queue) {
        node = list_entry (me, TextNode, list);

        if (i++ == index) {
            return __mg_substrlen (log_font, node->content.string, 
                    node->content.txtlen, '\n', &count);
        }
    }
    return -1;
}

static int textdoc_get_paragraph_text (HWND hWnd, TextDoc *txtdoc, 
            int index, int start, int len, unsigned char *buffer)
{
    list_t *me;
    TextNode *node;
    unsigned char *pch = buffer;
    int total_len = 0, copy_len = 0;
    int count = 0, step = 0;
    int i = 0, last_pos = 0, fit;
    int *pos_chars = NULL;
    SIZE txtsize;
    HDC hdc;

    if (!buffer || index < -1 || start < 0 || len <= 0)
        return 0;

    list_for_each (me, &txtdoc->queue) {
        if (index != -1 && i++ != index)
            continue;

        node = list_entry (me, TextNode, list);

        if (count <= start && count + node->content.txtlen <= start) {
            count += node->content.txtlen;
            continue;
        }
        else if (count <= start && count + node->content.txtlen > start) {
            count += node->content.txtlen;
            step = start + node->content.txtlen - count;
        }
        else 
            step = 0;

        if (!(pos_chars = 
                calloc (1, sizeof(int) * (2 * node->content.txtlen + 1))))
            return -1;

        hdc = GETDC (hWnd);
        GetTextExtentPoint (hdc, 
            (const char*)node->content.string, 
            strlen((const char*)(node->content.string)), 0, 
            &fit,
            pos_chars,
            NULL,
            &txtsize);
        RELEASEDC (hWnd, hdc);

        if (step) {
            for (i = 0; i < fit; i++) {
               if (pos_chars[i] == step)
                   break;
               else if (pos_chars[i] < step 
                   && ((i+1) >= fit || ((i+1) < fit && pos_chars [i+1] > step))) {
                   step = pos_chars[i] > 0 ? pos_chars[i] : 0;
                   break;
               }
            } 
        }

        copy_len = MIN(node->content.txtlen-step, len - 1 - total_len);
        if (copy_len <= 0) {
            free (pos_chars);
            break;
        }

        /*It is last node copied*/
        if (copy_len < node->content.txtlen-step) {
            last_pos = copy_len + step;

            /*last character process*/
            if (pos_chars [fit-1] < last_pos) {
                if (node->content.string [node->content.txtlen] != '\n') {
                    last_pos = node->content.txtlen;
                    copy_len = last_pos - step;
                }
            } else {
                for (i = 0; i < fit; i++) {
                   if (pos_chars[i] == last_pos)
                        break;
                   else if (pos_chars[i] < last_pos && (i+1) < fit 
                       && pos_chars [i+1] > last_pos) {
                        last_pos= pos_chars[i];
                        copy_len = last_pos - step;
                   }
                }
            }
            free (pos_chars);

            memcpy (pch, node->content.string + step, copy_len);
            pch += copy_len;
            total_len += copy_len;
            break;
        }

        free (pos_chars);

        memcpy (pch, node->content.string + step, copy_len);
        pch += copy_len;
        total_len += copy_len;

        if (index != -1)
            break;
    }

    *pch = '\0';
    return total_len;
}

static int textdoc_get_num_paragraphs (TextDoc *txtdoc)
{
    list_t *me;
    int total_num = 0;

    list_for_each (me, &txtdoc->queue) {
        total_num ++;
    }

    return total_num;
}

static char* get_limited_str(HWND hWnd, const char* str, int bytes)
{
    static int pos, i = 0, eff_word;
    static int eff_chars, *pos_chars;
    static SIZE size;
    HDC hdc = GETDC (hWnd);
    char *dst = NULL;

    if ((pos_chars = malloc (bytes * sizeof(int))) == NULL) {
        return NULL;
    }

    eff_word = GetTextExtentPoint (hdc, str,
        bytes + 4, -1,
        &eff_chars, pos_chars, NULL, &size);

    if (eff_word > 0) {
        if ((dst = malloc (eff_chars)) == NULL) {
            free(pos_chars);
            return NULL;
        }
        memset (dst, 0, eff_chars);

        for (i = eff_chars - 1; i >= 0; i--) {
            if (pos_chars[i+1] <= bytes && pos_chars[i] < bytes ) {
                pos = i;
                memcpy(dst, str, pos_chars[i+1]);
                break;
            }
        }
    }

    free(pos_chars);
    RELEASEDC (hWnd, hdc);
    return dst;
}
/* 
 * textdoc_settext : setting TextDoc object using a new text content and 
 *                   free the old one
 * Params          : content - new text content, if NULL, the content of the 
 *                             TextDoc object will not be changed; if content
 *                             is a null string, txtdoc content will be cleared.
 * TODO            : for not null-terminated text
 */
static int textdoc_settext (TextDoc *txtdoc, const char*content)
{
    const char *pLine, *ptmp;
    char *str = NULL;
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);

    if (!txtdoc || !content) return -1;

    /* free the old text content */
    textdoc_free (txtdoc);
    /*now, total length should be 0*/
    ptedata->curlen = 0;

    if (ptedata->hardLimit != -1 &&  
            ptedata->hardLimit < strlen(content)) {
        str = get_limited_str(hWnd, content, ptedata->hardLimit);
    } 
    else {
        str = (char*)content;
    }

    ptmp = pLine = str;

    if (str) {
        while (*ptmp != '\0') {
            if (*ptmp == txtdoc->lnsep) {
                /* adds a new line, including the line seperator */
                txtAddNode (txtdoc, pLine, ptmp-pLine+1, NULL);
                pLine = ptmp + 1;
            }
            ptmp ++;
        }
    }
    /* adds a new blank line or the last line without a line seperator */
    txtAddNode (txtdoc, pLine, ptmp-pLine, NULL);

    set_current_node (txtdoc, FIRSTNODE(txtdoc), FALSE, TRUE);
    
    if (ptedata->hardLimit != -1 &&  
            ptedata->hardLimit < strlen(content)) {
        free(str);
    }

    return 0;
}
static void
insert_string (TextDoc *txtdoc, TextNode *curnode, int insert_pos, 
               const char *newtext, int len)
{
    StrBuffer *strbuff = &curnode->content;
    unsigned char *pLn, *pIns;
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);

    if (len > 0) {
        pLn = testr_realloc (strbuff, strbuff->txtlen + len);
        if (!pLn) return;
        pIns = pLn + insert_pos;
        memmove (pIns + len, pIns, strbuff->txtlen+1 - insert_pos);
        memcpy (pIns, newtext, len);

        if (ptedata->hardLimit !=-1 && 
            ptedata->curlen + len > ptedata->hardLimit) { 
            Ping ();
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
            return;

        }
    }
    else {
        pIns = strbuff->string + insert_pos;
        memmove (pIns + len, pIns, strbuff->txtlen+1 - insert_pos);
        pLn = testr_realloc (strbuff, strbuff->txtlen + len);
    }

    ptedata->curlen += len;
    strbuff->txtlen += len;
}

#ifdef _SELECT_SUPPORT
static TextMark* get_start_mark (PTEDATA ptedata)
{
    TextDoc *txtdoc = &ptedata->txtdoc;

    if (ptedata->curItemY < ptedata->selItemY ||
             (ptedata->curItemY == ptedata->selItemY && 
              txtdoc->insert.pos_lnOff < txtdoc->selection.pos_lnOff) )
        return &txtdoc->insert;
    else
        return &txtdoc->selection;
}

/* Gets the start and end selection points in a text node */
static void
get_selection_points (PTEDATA ptedata, TextNode *node, int *pos_start, int *pos_end)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextMark *markStart = get_start_mark (ptedata);
    TextMark *markEnd = (markStart == &txtdoc->insert) ? 
                          &txtdoc->selection : &txtdoc->insert;

    if (node ==  txtdoc->insert.curNode || node == txtdoc->selection.curNode) {
        if (txtdoc->insert.curNode == txtdoc->selection.curNode) {
            *pos_start = markStart->pos_lnOff;
            *pos_end = markEnd->pos_lnOff;
        }
        else if (node == markStart->curNode) {
            *pos_start = markStart->pos_lnOff;
            *pos_end = node->content.txtlen;
        }
        else {
            *pos_start = 0;
            *pos_end = markEnd->pos_lnOff;
        }
    }
    else {
        *pos_start = 0;
        *pos_end = node->content.txtlen;
    }
}

/*
 * delete_selection : deletes the selected texts
 */
static int delete_selection (TextDoc *txtdoc)
{
    int pos_start, pos_end;
    int pos_start2, pos_end2;
    TextNode *node, *startnode, *endnode;
    TextMark *markStart, *markEnd;
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 (hWnd);

    markStart = get_start_mark (ptedata);
    markEnd = (markStart == &txtdoc->insert) ? 
                          &txtdoc->selection : &txtdoc->insert;

    startnode = markStart->curNode;
    endnode = markEnd->curNode;

    get_selection_points (ptedata, endnode, &pos_start, &pos_end);
    get_selection_points (ptedata, startnode, &pos_start2, &pos_end2);

    txtdoc->selection.curNode = NULL;

    scrollview_freeze (hWnd, &ptedata->svdata, TRUE);

    insert_string (txtdoc, endnode, pos_end, NULL, pos_start-pos_end);

    if (startnode != endnode) {

        while ( (node = TXTNODE_NEXT(startnode)) != endnode ) {
            txtDelNode (txtdoc, node);
        }

        if (pos_start2 == 0) {
            txtDelNode (txtdoc, startnode);
            startnode = NULL;
            txtdoc->insert.curNode = endnode;
            txtdoc->insert.pos_lnOff = 0;
            txtdoc->change_fn (txtdoc, FALSE);
        }
        else {
            char del[1] = {127};
            textdoc_insert_string_ex (txtdoc, startnode, pos_end2-1, NULL, 
                                      pos_start2-pos_end2+1);
            textdoc_insert_string_ex_2 (txtdoc, startnode, pos_start2, del, 1);
            txtdoc->insert.curNode = startnode;
            endnode = NULL;
        }
    }

    if (txtdoc->change_cont) {
        txtdoc->change_cont (txtdoc, endnode);
        txtdoc->change_cont (txtdoc, startnode);
    }

    txtdoc->selection.curNode = NULL;
    scrollview_unselect_all (&ptedata->svdata);
    scrollview_freeze (hWnd, &ptedata->svdata, FALSE);

    return pos_start2;
}
#endif

static TextNode*
insert_ln_sep (TextDoc *txtdoc, TextNode *curnode, int insert_pos, 
                           BOOL bChRn)
{
    StrBuffer *strbuff = &curnode->content;
    TextNode *newnode;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2((HWND)txtdoc->fn_data);
    unsigned char *pIns;
    int len = bChRn ? 2 : 1;

    pIns = strbuff->string + insert_pos;

    newnode = txtAddNode ( txtdoc, (const char*)pIns, strbuff->txtlen - 
                          (pIns-strbuff->string), curnode );

    strbuff->txtlen = insert_pos + len; /* add a line sep */
    if (*pIns == '\0') {
        testr_realloc (strbuff, strbuff->txtlen);
        pIns = strbuff->string + insert_pos ;
    }

    if (bChRn)
        strncpy((char*)pIns, CH_RN, len);
    else
        *pIns = txtdoc->lnsep;
    *(pIns + len) = '\0';

    ptedata->curlen += len;
    return newnode;
}

/* 
 * textdoc_insert_string_ex : 
 *          inserts a text string(not including line seperator) into 
 *          the text buffer at the specified insertion point, or makes
 *          some special operations (inserts line sep, del, bs, ...)
 * params : enode      - the specified node to operate on
 *          insert_pos - the designed insert position
 *          newtext    - text to insert
 *          len        - len of text to insert or remove
 */
static int
textdoc_insert_string_ex (TextDoc *txtdoc, TextNode *enode, int insert_pos, 
                          const char* newtext, int len)
{
    HWND hWnd = (HWND)txtdoc->fn_data;
    TextNode *curnode, *newnode;
    StrBuffer *strbuff;
    unsigned char *pIns;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 (hWnd);

#ifdef _UNDO_SUPPORT
    /* teUndoBackup (txtdoc); */
#endif

#ifdef _SELECT_SUPPORT
    if (!enode && txtdoc->selection.curNode) {
        insert_pos = delete_selection (txtdoc);
        if (len == 1 && (*newtext == '\b' || *newtext == 127) )
            return insert_pos;
        else {
            txtdoc->insert.pos_lnOff = insert_pos;
        }
    }
#endif

    if (!enode) { /* operates on the current insertion point */
        curnode = txtdoc->insert.curNode;
        insert_pos = txtdoc->insert.pos_lnOff;
    }
    else {
        curnode = enode;
    }

    strbuff = &curnode->content;
    pIns = strbuff->string + insert_pos;

    if ( len == 1 || (len == 2 && strcmp(newtext, CH_RN) == 0) ) {
        if ( *newtext == txtdoc->lnsep || (strncmp(newtext, CH_RN, 2) == 0) ) {
            /* add a new line */
            newnode = insert_ln_sep (txtdoc, curnode, 
                                             insert_pos, len == 2);
            if (!enode) {
                txtdoc->insert.curNode = newnode;
                insert_pos = 0;
            }
            return insert_pos;
        }

        /* len == 1, other cases */
        switch (*newtext) {
        case '\b':  /* BS */
            if (pIns == strbuff->string) { /* line head */
                TextNode *node;
                int cplen;
                char *tmpstr = NULL;

                if (BE_FIRST_NODE(curnode))
                    return insert_pos;
                node = TXTNODE_PREV(curnode);

                cplen = node->content.txtlen;

                tmpstr = malloc(cplen + 1);
                if (tmpstr != NULL)
                {
                    memcpy (tmpstr, node->content.string, cplen);
                    tmpstr [cplen] = '\0';

                    /* deletes the previous node*/
                    txtDelNode (txtdoc, node);

                    /* adds the previous line at the beginning of the current line */
                    insert_pos += textdoc_insert_string_ex (txtdoc, 
                                        enode, insert_pos, 
                                       (const char*)tmpstr, cplen-1);

                    txtdoc->change_fn (txtdoc, FALSE);
                    free(tmpstr);
                }
                return insert_pos;
            }
            len = - CHLENPREV((const char *)(strbuff->string), pIns);
            break;

        case 127: /* DEL */
            if (*pIns == '\0')
                return insert_pos;
            if ( *pIns == txtdoc->lnsep || is_rn_sep(pIns) ) {
                TextNode *node = TXTNODE_NEXT(curnode);
                int oldpos = insert_pos;
                if (node->content.string[0] != txtdoc->lnsep && 
                                !is_rn_sep(node->content.string)) {
					
                    int cplen = node->content.txtlen;
                    char *tmpstr = NULL;
					
                    /*next node have info*/
                    *pIns = '\0';

                    ptedata->curlen -= 1;

                    curnode->content.txtlen = pIns - curnode->content.string;
                    tmpstr = malloc(cplen + 1);
                    if (tmpstr == NULL)
                        break;
                    memcpy (tmpstr, node->content.string, cplen);
                    tmpstr[cplen]='\0';

                    txtDelNode (txtdoc, node);
                    textdoc_insert_string_ex (txtdoc, enode, insert_pos, 
                                    (const char*)tmpstr, cplen);
                    insert_pos = oldpos;
                    free(tmpstr);
                }
                else
                    txtDelNode (txtdoc, node);

                return insert_pos;
            }
            else {
                int chlen = CHLENNEXT(pIns, (curnode->content.txtlen + 
                                             strbuff->string - pIns) );
                pIns += chlen;
                insert_pos += chlen;
                len = -chlen;
            }
            break;
        }
    }

    insert_string (txtdoc, curnode, insert_pos, newtext, len);
    insert_pos += len;

    return insert_pos;
}

/*
 * textdoc_insert_text : insert a multi-line text at the insertion point
 */
static int
textdoc_insert_text (TextDoc *txtdoc, const char* newtext, int len)
{
    const char *pLine, *ptmp;
    TextNode *node, *newnode, *nextnode;
    int leftlen = len;

    if (!txtdoc || !newtext || len < 0) return -1;

    pLine = ptmp = newtext;
    /* insert first strings */
    while (*ptmp != txtdoc->lnsep && *ptmp != '\0' && leftlen > 0) {
        ptmp ++;
        leftlen --;
    }
    textdoc_insert_string (txtdoc, pLine, ptmp-pLine);

    if (*ptmp == '\0' || leftlen <= 0)
        return 0;

    node = txtdoc->insert.curNode;

    /* make next node */
    if (ptmp > pLine  && is_rn_sep(ptmp - 1)) {
        textdoc_insert_string (txtdoc, CH_RN, 2);
    }
    else {
        textdoc_insert_string (txtdoc, (const char *)(&txtdoc->lnsep), 1);
    }
    ptmp ++;
    leftlen --;
    pLine = ptmp;
    nextnode = TXTNODE_NEXT (node);

    /* insert lines */
    newnode = node;
    while (leftlen > 0 && *ptmp != '\0') {
        if (*ptmp == txtdoc->lnsep) {
            newnode = txtAddNode (txtdoc, pLine, ptmp-pLine+1, newnode);
            pLine = ptmp + 1;
        }
        ptmp ++;
        leftlen --;
    }

#ifdef _SELECT_SUPPORT
    txtdoc->selection.curNode =NULL;
    txtdoc->selection.pos_lnOff = 0;
#endif

    /* insert last strings */
    /* FIXME */
    if (!nextnode) {
        _MG_PRINTF ("CONTROL>TextEdit: (Warning) nextnode is NULL!\n");
        /* set_current_node (txtdoc, LASTNODE(txtdoc), FALSE, TRUE); */
    }
    else {
        set_current_node (txtdoc, nextnode, FALSE, TRUE);
        textdoc_insert_string (txtdoc, pLine, ptmp-pLine);
    }

    return 0;
}

/* ---------------------------------------------------------------------------- */

#ifdef _TITLE_SUPPORT
static int teGetTitleIndent (HWND hWnd, PTEDATA ptedata, HDC hdc)
{
    SIZE txtsize = {0, 0};

    if (ptedata->title)
        GetTextExtent(hdc, ptedata->title, strlen(ptedata->title), &txtsize);
    
    return txtsize.cx;
}
#endif

/* paint textedit base lines */
static void tePaint(HWND hWnd, HDC hdc, RECT *rcDraw)
{
    RECT *rc = rcDraw;
    PTEDATA ptedata;
    int h, indent = 0;

    ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    h = ptedata->nLineHeight - 1;

    if (GetWindowStyle(hWnd) & ES_BASELINE) {
        SetPenColor (hdc, GetWindowElementPixel (hWnd, WE_FGC_WINDOW));
        while (h < RECTHP(rc)) {
#ifdef _TITLE_SUPPORT
            indent = (h == ptedata->nLineHeight - 1) ? ptedata->titleIndent : 0;
#endif
            DrawHDotLine (hdc, rc->left + indent, rc->top+h, RECTWP(rc)-indent);
            h += ptedata->nLineHeight;
        }
    }
}

static void setup_dc (HWND hWnd, HDC hdc, BOOL bSel)
{
    if (!bSel) {
        SetBkMode (hdc, BM_TRANSPARENT);
        SetBkColor (hdc, GetWindowBkColor (hWnd));

        SetTextColor (hdc, 
            GetWindowElementPixel (hWnd, GetWindowStyle(hWnd)&WS_DISABLED?
            WE_FGC_DISABLED_ITEM : WE_FGC_WINDOW));
    }
    else {
        SetBkMode (hdc, BM_OPAQUE);

        SetTextColor (hdc, 
            GetWindowElementPixel (hWnd, GetWindowStyle(hWnd)&WS_DISABLED?
            WE_FGC_DISABLED_ITEM : WE_FGC_SELECTED_ITEM));

        if (hWnd == GetFocus(GetParent(hWnd))) {
            SetBkColor (hdc, GetWindowElementPixel (hWnd, WE_BGC_SELECTED_ITEM));
        }
        else {
            SetBkColor (hdc, GetWindowElementPixel (hWnd, WE_BGC_SELECTED_LOSTFOCUS));
        }
    }
}

/* 
 * teDrawItem : draw text node/item, including multi lines in wrap mode
 */
static void teDrawItem (HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
{
    RECT rcTxt = *rcDraw;
    TextNode *node;
    char *content;
    int txtlen, outlen, indent;
    UINT format;
    PTEDATA ptedata;
    TextDoc *txtdoc;
#ifdef _SELECT_SUPPORT
    int i, pos_start, pos_end, outchars, line_nr, outw, selout = 0;
    DTFIRSTLINE fl;
#endif
    unsigned char chln = 0;

    ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    txtdoc = &ptedata->txtdoc;

    node =  (TextNode*) scrollview_get_item_adddata(hsvi);
    if (!node)
        return;

    content = (char*)node->content.string;
    txtlen = node->content.txtlen;
    indent = teGetLineIndent (ptedata, node);

    if (txtlen <= 0 && indent <= 0)
        return;

    setup_dc (hWnd, hdc, FALSE);

    format = TE_FORMAT;
    if (BE_WRAP(hWnd))
        format |= DT_WORDBREAK;
    SetTextAboveLineExtra (hdc, ptedata->nLineAboveH);
    SetTextBellowLineExtra (hdc, ptedata->nLineBaseH);

#ifdef _TITLE_SUPPORT
    /* draw title */
    if (ptedata->title && indent > 0)
        DrawText (hdc, ptedata->title, strlen(ptedata->title), &rcTxt, format);
#endif

    outlen = txtlen;
    if (outlen > 0 && content[outlen-1] == txtdoc->lnsep) {
        outlen --;
        if (txtdoc->lnsep == '\n' && 
            (outlen > 0 && content[outlen-1] == '\r'))
            outlen --;
        if (ptedata->lnChar) {
            outlen ++;
            chln = content[outlen-1];
            content[outlen-1] = ptedata->lnChar;
        }
    }

    if (outlen <= 0)
        return;
    /* draw not selected node line text */
#ifdef _SELECT_SUPPORT
    if (!textnode_is_selected(txtdoc, node)) {
        DrawTextEx (hdc, content, outlen, &rcTxt, indent, format);
        if (chln)
            content[outlen-1] = chln;
        return;
    }

    /* draw selected node */
    get_selection_points (ptedata, node, &pos_start, &pos_end);
    line_nr = RECTH(rcTxt) / ptedata->nLineHeight;
    for (i = 0; i < line_nr; i++) {
        int startx, starty;

        if (i > 0) indent = 0;
        /* calc line info */
        DrawTextEx2(hdc, content, outlen, &rcTxt, indent, format, &fl);
        startx = rcTxt.left;
        starty = rcTxt.top;
        outw = indent;
        outchars = 0;
        if (pos_start > 0) { /* first: not selected section */
            setup_dc (hWnd, hdc, FALSE);
            outchars = MIN(pos_start,fl.nr_chars);
            outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
            content += outchars;
        }
        if (pos_start < fl.nr_chars && pos_end > 0) { /* second: selected section */
            outchars = MIN(pos_end, fl.nr_chars) -  MAX(pos_start,0);
            if (!ptedata->drawSelected) {
                setup_dc (hWnd, hdc, TRUE);
                outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
                if (!(ptedata->ex_flags & TEST_EX_SETFOCUS) 
                        && !(ptedata->ex_flags & TEST_EX_KILLFOCUS)) {
                    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SELCHANGED);
                }
            }
            else {
                SetBkMode (hdc, BM_OPAQUE);
                outw += ptedata->drawSelected(hWnd, hdc, startx+outw, starty, 
                        content, outchars, selout);
            }
            content += outchars;
            selout += outchars;
        }
        if (pos_end < fl.nr_chars) { /* third: not selected section */
            setup_dc (hWnd, hdc, FALSE);
            outchars = fl.nr_chars - MAX(pos_end, 0);
            outw += TabbedTextOutLen (hdc, startx + outw, starty, content, outchars);
            content += outchars;
        }
        pos_start -= fl.nr_chars;
        pos_end -= fl.nr_chars;
        outlen -= fl.nr_chars;
        rcTxt.top += ptedata->nLineHeight;
    }
#else
    DrawTextEx (hdc, content, outlen, &rcTxt, indent, format);
#endif
    if (chln)
        content[outlen-1] = chln;
}

/*
static void teDestroyItem (HWND hWnd, HSVITEM hsvi)
{
    TextNode *node = (TextNode*)scrollview_get_item_adddata(hsvi);
    textnode_destroy (node);
}
*/

/* --------------------------- size and position calculation ---------------- */

static int get_caret_width (HWND hWnd, PTEDATA ptedata)
{
    if (ptedata->caretShape == ED_CARETSHAPE_LINE) {
        return 1;
    }
    else if (ptedata->caretShape == ED_CARETSHAPE_BLOCK) {
        HDC hdc = GETDC (hWnd);
        TextDoc *txtdoc = &ptedata->txtdoc;
        TextNode *node = txtdoc->insert.curNode;
        char *pIns = ( char*)(node->content.string) + txtdoc->insert.pos_lnOff;
        int chlen = CHLENNEXT(pIns, node->content.txtlen - txtdoc->insert.pos_lnOff);
        int width = GetMaxFontWidth (hdc);

        if (chlen == 0) {
            if (ptedata->getCaretWidth) {
                int cw = ptedata->getCaretWidth (hWnd, width);
                if (cw > 0) width = cw;
            }
        }
        else {
            SIZE chsize = {0, 0};
            GetTextExtent (hdc, pIns, chlen, &chsize);

            if (chsize.cx > 0)
                width = chsize.cx;
        }

        RELEASEDC (hWnd, hdc);
        return width;
    }

    return 1;
}

/* sets the current caret position in the virtual content window */
static void mySetCaretPos (HWND hWnd, int x, int y)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 (hWnd);

   /*
    * no change, should return?
    if ( (x < 0 || x == ptedata->caret_x) && (y < 0 || y == ptedata->caret_y) )
        return;
    */

    if (x >= 0)
       ptedata->caret_x = x;
    else
       x = ptedata->caret_x;

    if (y >= 0)
       ptedata->caret_y = y;
    else
       y = ptedata->caret_y;

    scrolled_content_to_visible (ptescr, &x, &y);
    scrolled_visible_to_window (ptescr, &x, &y);
    SetCaretPos (hWnd, x, y);
    ChangeCaretSize (hWnd, get_caret_width(hWnd, ptedata), 
            ptedata->nLineHeight - ptedata->nLineAboveH- ptedata->nLineBaseH);
    if ( (ptedata->flags & TEST_FOCUSED) &&
             !(ptedata->flags & TEST_NOCARET) 
#ifdef _SELECT_SUPPORT
             && !(ptedata->txtdoc.selection.curNode)
#endif
        ) {
        ActiveCaret (hWnd);
        ShowCaret (hWnd);
    }
}

/*
 * teSetCaretPos : sets the caret position according to the current insertion 
 *                 point or sets the selection caret position according to the
 *                 current selection point of the text doc.
 * Description   : should be called after the current insert point or the 
 *                 selection point of the text document is changed
 */
static void teSetCaretPos (HWND hWnd, PTEDATA ptedata)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node;
    TextMark *mark;
    int h, indent, endh;
    HDC hdc;

    mark = GETMARK(txtdoc->selection.curNode);
    node = mark->curNode;

    if (!node) return;

    hdc = GETDC (hWnd); 

    SelectFont (hdc, GetWindowFont(hWnd));

#ifdef _SELECT_SUPPORT
    if (txtdoc->selection.curNode)
        h = ptedata->selItemY;
    else
#endif
        h = ptedata->curItemY;

    indent = teGetLineIndent (ptedata, node);

    if (BE_WRAP(hWnd)) {
        RECT rc;
        int w;
        SIZE txtsize = {0, 0};
        int txtlen, linenr, lineidx;
        const unsigned char *pLine, *pIns;
        DTFIRSTLINE fl;

        rc.left = 0;
        rc.top = 0;
        rc.right = ptedata->svdata.scrdata.nContWidth;
        rc.bottom = GetWindowFont(hWnd)->size;

        linenr = NODE_LINENR(node);
        pLine = node->content.string;
        pIns = pLine + mark->pos_lnOff;
        txtlen = node->content.txtlen;
        /*
        if (pLine[txtlen-1] == txtdoc->lnsep)
            txtlen --;
        */
        lineidx = 0;
        while (txtlen > 0) {
            DrawTextEx2 (hdc, (const char*)pLine, txtlen, &rc, indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &fl);
            if (pLine + fl.nr_chars > pIns)
                break;
            else if (pLine + fl.nr_chars == pIns) {
#if 0
                if (lineidx < linenr-1) {
                    pLine += fl.nr_chars;
                    lineidx ++;
                    indent = 0;
                }
#endif
                break;
            }
            pLine += fl.nr_chars;
            txtlen -= fl.nr_chars;
            rc.top += ptedata->nLineHeight;
            lineidx ++;
            indent = 0;
        }

        GetTabbedTextExtent(hdc, (const char*)pLine, pIns - pLine, &txtsize);

        w = indent + txtsize.cx;
        h += ptedata->nLineHeight * lineidx;

        ptedata->des_caret_x = w;
        endh = (h < ptescr->nContY) ? h : h + ptedata->nLineHeight;
        scrolled_make_pos_visible (hWnd, ptescr, -1, endh > 0 ? endh : 0);
#ifdef _SELECT_SUPPORT
        if (txtdoc->selection.curNode)
            mySetSelPos (w, h + ptedata->nLineAboveH);
        else
#endif
        {
            /* FIXME */
            if ( ptedata->caretShape == ED_CARETSHAPE_BLOCK && 
                     RECTW(rc) - w <= (GetWindowFont(hWnd)->size)/2 ) {
                scrolled_make_pos_visible (hWnd, ptescr, -1, 
                        endh + ptedata->nLineHeight);
                mySetCaretPos (hWnd, indent, 
                        h + ptedata->nLineAboveH + ptedata->nLineHeight);
            }
            else {
                mySetCaretPos (hWnd, w, h + ptedata->nLineAboveH);
            }
        }
    }
    else {
        SIZE txtsize;

        GetTabbedTextExtent(hdc, (const char*)(node->content.string), 
                mark->pos_lnOff, &txtsize);
        ptedata->des_caret_x = txtsize.cx + indent;
        endh = (h < ptescr->nContY) ? h : h + ptedata->nLineHeight;
        scrolled_make_pos_visible (hWnd, ptescr, 
                        txtsize.cx + indent, endh > 0 ? endh : 0);
#ifdef _SELECT_SUPPORT
        if (txtdoc->selection.curNode)
            mySetSelPos (txtsize.cx + indent, h + ptedata->nLineAboveH);
        else
#endif
            mySetCaretPos (hWnd, txtsize.cx + indent, h + ptedata->nLineAboveH);
    }

    RELEASEDC (hWnd, hdc);
}

static void textedit_set_svlist (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 (hWnd);
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node = FIRSTNODE(txtdoc);

    /* reset caret pos in the real window */
    if (ptedata->flags & TEST_FOCUSED)
        mySetCaretPos (hWnd, -1, -1);

    while (node) {
    	txtdoc->change_cont (txtdoc, node);
        node = TXTNODE_NEXT(node);
    }

    scrollview_set_svlist (hWnd, pscrdata, visChanged);
}

/*
 * set_caret_pos : Sets the caret/selection position in a node according to
 *                 x,y values
 */
static int
set_caret_pos (HWND hWnd, PTEDATA ptedata, TextNode *node, int x, int y, BOOL bSel)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    const unsigned char *string = node->content.string;
    const unsigned char *pLine = string;
    int txtlen = node->content.txtlen;
    int line, indent, h = 0, out_chars, ln_chars;
    RECT rc;
    SIZE txtsize;
    DTFIRSTLINE fl;
    HDC hdc;

    indent = teGetLineIndent (ptedata, node);
    line = y / ptedata->nLineHeight;
    if (!bSel)
        h = ptedata->curItemY + line * ptedata->nLineHeight;
#ifdef _SELECT_SUPPORT
    else
        h = ptedata->selItemY + line * ptedata->nLineHeight;
#endif

    if (txtlen == 0 || string[0] == txtdoc->lnsep || is_rn_sep(string)) {
        if (!bSel) {
            txtdoc->insert.pos_lnOff = 0;
            mySetCaretPos (hWnd, 0 + indent, h + ptedata->nLineAboveH);
        }
#ifdef _SELECT_SUPPORT
        else {
            txtdoc->selection.pos_lnOff = 0;
            mySetSelPos (0 + indent, h + ptedata->nLineAboveH);
        }
#endif
        return 0;
    }

    /* FIXME */
    hdc = GETDC (hWnd);

    if (NODE_LINENR(node) > 1) {  /* multi line case */
        int i = 0;

        rc.left = 0;
        rc.top = 0;
        rc.right = scrolled_get_contwidth(ptescr);
        rc.bottom = ptedata->nLineHeight;

        i = 0;
        while (1) {
            if (i > 0) indent = 0;
            DrawTextEx2 (hdc, (const char*)pLine, txtlen, &rc, indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &fl);
            if (i == line)
                break;
            pLine += fl.nr_chars;
            txtlen -= fl.nr_chars;
            i++;
        }
        ln_chars = fl.nr_chars;
    }
    else {
        pLine = string;
        ln_chars = txtlen;
    }


    if (pLine[ln_chars-1] == txtdoc->lnsep) {
        /* add caret before line seperator */
        ln_chars --;
        if (ln_chars > 0 && is_rn_sep((pLine + ln_chars - 1)) )
            ln_chars --;
    }

    if (x - indent <= 0) {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else
        out_chars = GetTabbedTextExtentPoint (hdc, (const char*)pLine, ln_chars, 
                        x - indent, NULL, NULL, NULL, &txtsize);

    RELEASEDC (hWnd, hdc);

    if (!bSel) {
        txtdoc->insert.pos_lnOff = pLine - string + out_chars;
        mySetCaretPos (hWnd, txtsize.cx + indent, h + ptedata->nLineAboveH);
    }
#ifdef _SELECT_SUPPORT
    else {
        txtdoc->selection.pos_lnOff = pLine - string + out_chars;
        mySetSelPos (txtsize.cx + indent, h + ptedata->nLineAboveH);
    }
#endif

    return 0;
}

/* calculates line number of a text node, for wrap mode */
static int get_line_nr (HWND hWnd, PTEDATA ptedata, TextNode *node, int indent)
{
    RECT rc;
    PLOGFONT logfont = GetWindowFont(hWnd);
    HDC hdc;

    if (!node) {
        _MG_PRINTF ("CONTROL>TextEdit: (Warning) pass NULL text node to get_line_nr\n");
        return 0;
    }

    if (!BE_WRAP(hWnd))
        return 1;

    hdc = GETDC (hWnd);

    SelectFont (hdc, logfont);

    rc.left = 0;
    rc.top = 0;
    rc.right = scrolled_get_contwidth(ptescr);
    rc.bottom = logfont->size;

    if (ptedata->lnChar) {
        char *content, chln = 0;
        int outlen;
        content = (char*)node->content.string;
        outlen = node->content.txtlen;
        if (outlen > 0 && content[outlen-1] == ptedata->txtdoc.lnsep) {
            outlen --;
            if (ptedata->txtdoc.lnsep == '\n' && content[outlen-1] == '\r')
                outlen --;
            outlen ++;
            chln = content[outlen-1];
            content[outlen-1] = ptedata->lnChar;
        }
        DrawTextEx (hdc, content, outlen, &rc, indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT);
        if (chln)
            content[outlen-1] = chln;
    }
    else {
        DrawTextEx (hdc, (const char*)(node->content.string), node->content.txtlen, &rc, indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT);
    }

    RELEASEDC (hWnd, hdc);

    return (RECTH(rc)/logfont->size);
}

/*
 * TODO
 */
static int recalc_max_len (HWND hWnd ,PTEDATA ptedata)
{
    list_t *me;
    TextDoc *txtdoc = &ptedata->txtdoc;
    SIZE txtsize;
    TextNode *node;
    HDC hdc;

    ptedata->maxLen = 0;
    ptedata->maxNode = NULL;
    ptedata->secLen = 0;
    ptedata->secNode = NULL;

    hdc = GETDC (hWnd);
    //SelectFont (hdc, GetWindowFont(hWnd));
    list_for_each (me, &txtdoc->queue) {
        node = list_entry (me, TextNode, list);
        GetTabbedTextExtent(hdc, (const char*)(node->content.string), 
                        node->content.txtlen, &txtsize);
        if (txtsize.cx > ptedata->maxLen || ptedata->maxLen == 0) {
            ptedata->maxLen = txtsize.cx;
            ptedata->maxNode = node;
        }
        else if (txtsize.cx >= ptedata->secLen) {
            ptedata->secLen = txtsize.cx;
            ptedata->secNode = node;
        }
    }

    RELEASEDC (hWnd, hdc);
    return 0;
}

static int
revise_max_len (HWND hWnd, PTEDATA ptedata, TextNode *node, int textlen)
{
    if (textlen > ptedata->maxLen) {
        if (node != ptedata->maxNode) {
            ptedata->secLen = ptedata->maxLen;
            ptedata->secNode = ptedata->maxNode;
        }
        ptedata->maxLen = textlen;
        ptedata->maxNode = node;
    }
    else if (textlen > ptedata->secLen) {
        if (node != ptedata->maxNode) {
            ptedata->secLen = textlen;
            ptedata->secNode = node;
            return 0; /* no need to reset content width */
        }
        else {
            ptedata->maxLen = textlen;
        }
    }
    else { /* FIXME */
        if (node == ptedata->maxNode) {
            recalc_max_len (hWnd, ptedata);
            return 1;
        }
        else if (node == ptedata->secNode) {
            recalc_max_len (hWnd, ptedata);
            return 0;
        }
        return 0;
    }

    /* need to reset content width */
    return 1;
}

/* set text node line width, for non wrap mode */
static int 
set_line_width (HWND hWnd, PTEDATA ptedata, TextNode *node, int indent)
{
    SIZE txtsize = {0, 0};
    HDC hdc;

    if (!node) {
        _MG_PRINTF ("CONTROL>TextEdit: (Warning) pass NULL text node to set_line_width\n");
        return -1;
    }

    if (BE_WRAP(hWnd))
        return -1;

    hdc = GETDC (hWnd);
    SelectFont (hdc, GetWindowFont(hWnd));
    GetTabbedTextExtent(hdc, (const char*)(node->content.string), 
                        node->content.txtlen, &txtsize);
    RELEASEDC (hWnd, hdc);

    if (revise_max_len (hWnd, ptedata, node, txtsize.cx + indent) > 0)
        scrolled_set_cont_width (hWnd, ptescr, ptedata->maxLen + 1);

    return 0;
}

/*
static int set_line_size (HWND hWnd, PTEDATA ptedata, TextNode *node)
{
    int linenr = 1, indent;

    if (!node)
        return 0;

    indent = teGetLineIndent (ptedata, node);

    linenr = get_line_nr(hWnd, ptedata, node, indent);
    set_line_width (hWnd, ptedata, node, indent);

    return scrollview_set_item_height (hWnd, (HSVITEM)node->addData, 
                        ptedata->nLineHeight*linenr);
}
*/

/*
 * get_node_by_idx : Get node and y position by node index
 */
static TextNode* get_node_by_idx (PTEDATA ptedata, int line, int *item_y)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node;
    int nr = 0, ypos = 0;

    if (line < 0)
        return NULL;

    node = FIRSTNODE(txtdoc);
    while (node) {
        if (nr == line)
            break;
        nr ++;
        ypos += NODE_HEIGHT(node);
        node = TXTNODE_NEXT(node);
    }

    if (nr != line)
        return NULL;

    if (item_y)
        *item_y = ypos;
    return node;
}

static void teNodeInit (TextDoc *txtdoc, TextNode* node, TextNode *prenode)
{
    SVITEMINFO svii;
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    int linenr = 1, indent;
    HSVITEM preitem = 0;

    if (!node) return;

    indent = teGetLineIndent (ptedata, node);

    linenr = get_line_nr(hWnd, ptedata, node, indent);
    set_line_width (hWnd, ptedata, node, indent);

    svii.nItemHeight = ptedata->nLineHeight * linenr;
    svii.addData = (DWORD)node;
    svii.nItem = -1;

    if (prenode)
        preitem = (HSVITEM)prenode->addData;
    node->addData = (DWORD) scrollview_add_item (hWnd, &ptedata->svdata, 
                                preitem, &svii, NULL);
}

static void teNodeDel (TextDoc *txtdoc, TextNode *node)
{
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    HSVITEM hsvi = (HSVITEM) node->addData; 

    node->addData = 0;
    scrollview_del_item (hWnd, &ptedata->svdata, 0, hsvi);
}

static void teNodeChange (TextDoc *txtdoc, BOOL bSel)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2((HWND)txtdoc->fn_data);

    if (!bSel) {
        ptedata->curItemY = scrollview_get_item_ypos (&ptedata->svdata, 
                                     (HSVITEM)txtdoc->insert.curNode->addData);
    }
#ifdef _SELECT_SUPPORT        
    else {
        ptedata->selItemY = scrollview_get_item_ypos (&ptedata->svdata, 
                                     (HSVITEM)txtdoc->selection.curNode->addData);
    }
#endif        
}

static void teChangeCont (TextDoc *txtdoc, TextNode *node)
{
    HWND hWnd = (HWND)txtdoc->fn_data;
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    int indent, linenr;

    if (!node)
        return;

    indent = teGetLineIndent (ptedata, node);
    linenr = get_line_nr(hWnd, ptedata, node, indent);
    scrollview_set_item_height (hWnd, (HSVITEM)node->addData, 
                        ptedata->nLineHeight*linenr);
    set_line_width (hWnd, ptedata, node, indent);
}

/*
 * textdoc_reset : resets or initializes the properties of a textdoc to 
 *                 default values
 */
static int textdoc_reset (HWND hWnd, TextDoc *txtdoc)
{
    txtdoc->lnsep = DEF_LINESEP;
    txtdoc->nDefLineSize = DEF_LINE_BUFFER_SIZE;
    txtdoc->nBlockSize = DEF_LINE_BLOCK_SIZE;

    txtdoc->init_fn = teNodeInit;
    txtdoc->change_fn = teNodeChange;
    txtdoc->change_cont = teChangeCont;
    txtdoc->del_fn = teNodeDel;
    txtdoc->fn_data = (void*)hWnd;
    return 0;
}

/*
 * teResetData : resets status data
 */
static void teResetData (PTEDATA ptedata)
{
    ptedata->des_caret_x = 0;

    ptedata->curItemY = 0;
#ifdef _SELECT_SUPPORT
    ptedata->selItemY = 0;
#endif

    ptedata->maxNode = NULL;
    ptedata->maxLen = 0;
    ptedata->secNode = NULL;
    ptedata->secLen = 0;

    ptedata->flags = 0;
}

/* -------------------------------------------------------------------------- */

#if defined(_MGHAVE_CLIPBOARD) && defined(_SELECT_SUPPORT)
int textdoc_copy_to_cb (PTEDATA ptedata, char *buffer, int len, BOOL bCB)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    int pos_start, pos_end;
    TextNode *node, *endnode;
    TextMark *markStart, *markEnd;
    int org_len = len;

    if (!txtdoc->selection.curNode)
        return 0;

    markStart = get_start_mark (ptedata);
    markEnd = (markStart == &txtdoc->insert) ? 
                          &txtdoc->selection : &txtdoc->insert;

    node = markStart->curNode;
    endnode = markEnd->curNode;

    /* clear clipboard */
    if (bCB)
        SetClipBoardData (CBNAME_TEXT, NULL, 0, CBOP_NORMAL);
    while (1) {
        get_selection_points (ptedata, node, &pos_start, &pos_end);
        if (bCB) {
            SetClipBoardData (CBNAME_TEXT, node->content.string + pos_start,
                        pos_end - pos_start, CBOP_APPEND);
        }
        else if (buffer) {
            int output;
            output = MIN(len, pos_end - pos_start);
            strncpy (buffer, (const char*)(node->content.string) + pos_start, output);
            buffer += output;
            len -= output;
        }
        else {
            len += pos_end - pos_start;
        }

        if(node != endnode)
            node = TXTNODE_NEXT (node);
        else
            break;
    }

    if (!bCB && !buffer && org_len == 0)
        return len;

    return org_len - len;
}
#endif

static void te_make_caret_visible (HWND hWnd, PTEDATA ptedata, BOOL bSel)
{
    POINT pt;

    if (!bSel)
        myGetCaretPos (&pt);
#ifdef _SELECT_SUPPORT
    else
        myGetSelPos (&pt);
#endif

    if (pt.y > ptescr->nContY)
        pt.y = (pt.y / ptedata->nLineHeight + 1) * ptedata->nLineHeight;
    else
        pt.y = (pt.y / ptedata->nLineHeight) * ptedata->nLineHeight;
    scrolled_make_pos_visible (hWnd, ptescr, pt.x, pt.y);

    if (!bSel) {
        mySetCaretPos (hWnd, -1, -1);

    /*
     * FIXME
     * ShowCaret (hWnd);
     */
    }
}


static void teSetCaretPosOnChar (HWND hWnd, PTEDATA ptedata, int old_caret_pos)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node;
    TextMark *mark;
    int h, indent, endh;
    HDC hdc;

    /*initialize line info*/
    ptedata->nr_diff_line = -1;
    ptedata->w_nochanged = 0;
    ptedata->w_lastline = 0;

    mark = GETMARK(txtdoc->selection.curNode);
    node = mark->curNode;

    if (!node) return;

    hdc = GETDC (hWnd);
    SelectFont (hdc, GetWindowFont(hWnd));

#ifdef _SELECT_SUPPORT
    if (txtdoc->selection.curNode)
        h = ptedata->selItemY;
    else
#endif
        h = ptedata->curItemY;

    indent = teGetLineIndent (ptedata, node);
    old_caret_pos = MIN(old_caret_pos, mark->pos_lnOff);

    if (BE_WRAP(hWnd)) {
        RECT rc;
        int w;
        SIZE txtsize = {0, 0};
        int oldtxtlen, newtxtlen, linenr, lineidx;
        const unsigned char *newLine, *oldLine, *pIns, *oldIns;
        DTFIRSTLINE oldfl, newfl;

        rc.left = 0;
        rc.top = 0;
        rc.right = ptedata->svdata.scrdata.nContWidth;
        rc.bottom = GetWindowFont(hWnd)->size;

        oldLine = ptedata->tebuff.string;
        oldtxtlen = ptedata->tebuff.txtlen;

        linenr = NODE_LINENR(node);
        newLine = node->content.string;
        newtxtlen = node->content.txtlen;
        pIns = newLine + mark->pos_lnOff;
        oldIns = oldLine + old_caret_pos;
        lineidx = 0;

        newfl.width = 0;
        while (old_caret_pos > 0) {
            DrawTextEx2 (hdc, (const char*)oldLine, oldtxtlen, &rc, 
                    lineidx ? 0: indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &oldfl);

            DrawTextEx2 (hdc, (const char*)newLine, newtxtlen, &rc, 
                    lineidx ? 0 : indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &newfl);

            /*compare two string */
            if (oldfl.nr_chars != newfl.nr_chars)
            {
                ptedata->nr_diff_line = lineidx;

                if (oldfl.nr_chars < newfl.nr_chars)
                    ptedata->w_nochanged = oldfl.width;
                else
                    ptedata->w_nochanged = newfl.width;
            }

            if (oldLine + oldfl.nr_chars > oldIns) {
                ptedata->nr_diff_line = lineidx;
                GetTabbedTextExtent(hdc, (const char*)oldLine, 
                   oldIns - oldLine, &txtsize);
		if (txtsize.cx < ptedata->w_nochanged)
                   ptedata->w_nochanged = txtsize.cx;
            }

            if (ptedata->nr_diff_line != -1)
                break;

            newLine += newfl.nr_chars;
            oldLine += oldfl.nr_chars; 
            oldtxtlen -= oldfl.nr_chars;
            newtxtlen -= newfl.nr_chars;
            lineidx ++;
            old_caret_pos -= oldfl.nr_chars;
        }

        if (ptedata->nr_diff_line == -1) {
            ptedata->nr_diff_line = lineidx;
            GetTabbedTextExtent(hdc, (const char*)oldLine, 
               old_caret_pos, &txtsize);
            ptedata->w_nochanged = txtsize.cx;
        }
        if (lineidx == 0)
            ptedata->w_nochanged += indent;

        newLine = node->content.string;
        newtxtlen = node->content.txtlen;
        lineidx = 0;

        while (newtxtlen > 0) {
            DrawTextEx2 (hdc, (const char*)newLine, newtxtlen, &rc, indent, 
                    TE_FORMAT | DT_WORDBREAK | DT_CALCRECT, &newfl);
            if (newLine + newfl.nr_chars > pIns)
                break;
            else if (newLine + newfl.nr_chars == pIns) {
#if 0
                if (lineidx < linenr-1) {
                    newLine += newfl.nr_chars;
                    lineidx ++;
                    indent = 0;
                }
#endif
                break;
            }
            newLine += newfl.nr_chars;
            newtxtlen -= newfl.nr_chars;
            rc.top += ptedata->nLineHeight;
            lineidx ++;
            indent = 0;
        }

        GetTabbedTextExtent(hdc, (const char*)newLine, pIns - newLine, &txtsize);

        w = indent + txtsize.cx;
        h += ptedata->nLineHeight * lineidx;

        ptedata->des_caret_x = w;
        endh = (h < ptescr->nContY) ? h : h + ptedata->nLineHeight;

        scrolled_make_pos_visible (hWnd, ptescr, -1, endh > 0 ? endh : 0);
#ifdef _SELECT_SUPPORT
        if (txtdoc->selection.curNode) {
            mySetSelPos (w, h + ptedata->nLineAboveH);
        }
        else
#endif
        {
            /* FIXME */
            if ( ptedata->caretShape == ED_CARETSHAPE_BLOCK && 
                     RECTW(rc) - w <= (GetWindowFont(hWnd)->size)/2 ) {
                scrolled_make_pos_visible (hWnd, ptescr, -1, 
                        endh + ptedata->nLineHeight);
                mySetCaretPos (hWnd, indent, 
                        h + ptedata->nLineAboveH + ptedata->nLineHeight);
            }
            else {
                mySetCaretPos (hWnd, w, h + ptedata->nLineAboveH);
            }
        }
    }
    else {
        SIZE txtsize;

        ptedata->nr_diff_line = 0;

        GetTabbedTextExtent(hdc, 
                (const char*)(node->content.string), old_caret_pos, &txtsize);
        ptedata->w_nochanged = indent + txtsize.cx;

        GetTabbedTextExtent(hdc, (const char*)(node->content.string), 
                mark->pos_lnOff, &txtsize);
        ptedata->des_caret_x = txtsize.cx + indent;

        endh = (h < ptescr->nContY) ? h : h + ptedata->nLineHeight;
        scrolled_make_pos_visible (hWnd, ptescr, 
                        txtsize.cx + indent, endh > 0 ? endh : 0);
#ifdef _SELECT_SUPPORT
        if (txtdoc->selection.curNode) {
            mySetSelPos (txtsize.cx + indent, h + ptedata->nLineAboveH);
        }
        else
#endif
            mySetCaretPos (hWnd, txtsize.cx + indent, h + ptedata->nLineAboveH);
    }

    RELEASEDC (hWnd, hdc);
}

static void teOnChar (HWND hWnd, PTEDATA ptedata, WPARAM wParam)
{
    int chlen;
    unsigned char ch[4];
    TextNode *node;
    TextMark *mark;
    TextDoc *txtdoc;
    DWORD dwStyle = GetWindowStyle(hWnd);
    int old_caret_pos;
    RECT rcInv;
    int oldlinenr, newlinenr, i;

    if (dwStyle & ES_READONLY) {
        Ping();
        return;
    }

    ch [0] = FIRSTBYTE (wParam);
    ch [1] = SECONDBYTE (wParam);
    ch [2] = THIRDBYTE (wParam);
    ch [3] = FOURTHBYTE (wParam);

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

        if ( ch[0] < 0x20 && ch[0] != '\b' && ch[0] != '\t'
                && ch[0] != '\n' && ch[0] != '\r' ) {
            return;
        }

        if (dwStyle & ES_UPPERCASE) {
            ch [0] = toupper (ch[0]);
        }
        else if (dwStyle & ES_LOWERCASE) {
            ch [0] = tolower (ch[0]);
        }

        /* ENTER */
        if (ch [0] == '\r') {
            ch [0] = ptedata->txtdoc.lnsep;
        }
    }

    node = ptedata->txtdoc.insert.curNode;
    txtdoc = &(ptedata->txtdoc);
    mark = GETMARK(txtdoc->selection.curNode);
    old_caret_pos = mark->pos_lnOff;

    oldlinenr = NODE_LINENR(node);
    testr_copy(&(ptedata->tebuff), &(node->content));

    if (ptedata->hardLimit == -1 || (ptedata->curlen + chlen <= ptedata->hardLimit) 
            || (chlen == 1 && (ch[0] == '\b' || ch[0] == 127))) {
        if (textdoc_insert_string (&ptedata->txtdoc, (const char*)ch, chlen)) {
            teSetCaretPosOnChar (hWnd, ptedata, old_caret_pos);

            rcInv.left = ptedata->w_nochanged;
            rcInv.top = ptedata->nr_diff_line * ptedata->nLineHeight;
            rcInv.right = ptedata->svdata.scrdata.nContWidth;
            rcInv.bottom = rcInv.top + ptedata->nLineHeight - ptedata->nLineBaseH; 
            REFRESH_NODE_EX(node, &rcInv);
           // UpdateWindow(hWnd, FALSE); 

            newlinenr = NODE_LINENR(node);
            rcInv.left = 0;
            for (i = ptedata->nr_diff_line + 1; i < MAX(oldlinenr, newlinenr); i++) {
                rcInv.top += ptedata->nLineHeight;
                rcInv.bottom = rcInv.top + ptedata->nLineHeight  - ptedata->nLineBaseH; 
                REFRESH_NODE_EX(node, &rcInv);
             //   UpdateWindow(hWnd, FALSE); 
            }
            /* houhh 20090814, UpdataWindow will invalidate all edit area, but
             * erase area is not same with msg_paint area.*/
            UpdateWindow(hWnd, FALSE); 
            //SendMessage(hWnd, MSG_PAINT, 0, 0);
        }
    }
    else {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
    }

    if (node != ptedata->txtdoc.insert.curNode) {
        node = ptedata->txtdoc.insert.curNode;
        /* FIXME, optimize */
        teNodeChange (&ptedata->txtdoc, FALSE);
        teSetCaretPos (hWnd, ptedata);
        REFRESH_NODE(node);
    }
#ifdef _SELECT_SUPPORT
    teSetCaretPos (hWnd, ptedata);
#endif
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    ptedata->cont_dirty = TRUE;

}

static void
teOnMouseDown (HWND hWnd, TextNode *node, POINT *pt, int item_y, BOOL bShift)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    TextDoc *txtdoc = &ptedata->txtdoc;

    if (!node) return;

    if (!bShift) {
        txtdoc->insert.curNode = node;
        ptedata->curItemY = item_y;
    }
#ifdef _SELECT_SUPPORT
    else {
        txtdoc->selection.curNode = node;
        ptedata->selItemY = item_y;
    }
#endif

    set_caret_pos (hWnd, ptedata, node, pt->x, pt->y, bShift);
}

#ifdef _SELECT_SUPPORT

static void get_selected_rect (PTEDATA ptedata, RECT *rcSel)
{
    TextDoc *txtdoc = &ptedata->txtdoc;

    rcSel->left = 0;
    rcSel->right = 0;
    if (ptedata->curItemY < ptedata->selItemY) {
        rcSel->top = ptedata->curItemY;
        rcSel->bottom = ptedata->selItemY + NODE_HEIGHT(txtdoc->selection.curNode);
    }
    else {
        rcSel->top = ptedata->selItemY;
        rcSel->bottom = ptedata->curItemY + NODE_HEIGHT(txtdoc->insert.curNode);
    }

}

/*
 * te_unselect_all : unselects the selected parts
 * Params          : bChange - whether to change the insertion point to the
 *                             position of the selection point
 */
static void te_unselect_all (HWND hWnd, PTEDATA ptedata, BOOL bChange)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    RECT rcSel;
    POINT pt;

    pt.x = 0;
    pt.y = 0;

    if (txtdoc->selection.curNode) {
        get_selected_rect (ptedata, &rcSel);

        if (bChange) {
            txtdoc->insert = txtdoc->selection;
            ptedata->curItemY = ptedata->selItemY;
            myGetSelPos (&pt);
        }

        txtdoc->selection.curNode = NULL;
        scrollview_unselect_all (&ptedata->svdata);
        scrolled_refresh_rect (ptescr, &rcSel);

        if (bChange) {
            mySetCaretPos (hWnd, pt.x, pt.y);
            ptedata->des_caret_x = pt.x;
        }
    }
}

static int
te_set_selection (HWND hWnd, PTEDATA ptedata)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextMark *markStart, *markEnd;
    TextNode *node;
    RECT rcSel;

    markStart = get_start_mark (ptedata);
    markEnd = (markStart == &txtdoc->insert) ? 
                              &txtdoc->selection : &txtdoc->insert;

    node = markStart->curNode;
    while (node) {
        SELECT_NODE (node, TRUE);
        if (node == markEnd->curNode)
            break;
        node = TXTNODE_NEXT (node);
    }

    get_selected_rect (ptedata, &rcSel);
    scrolled_refresh_rect (ptescr, &rcSel);
    check_caret ();

    return 0;
}

static int te_select_all (HWND hWnd, PTEDATA ptedata)
{
    TextMark *selection = &ptedata->txtdoc.selection;
    TextMark *insertion = &ptedata->txtdoc.insert;
    TextNode *last;

    if (selection->curNode) {
        te_unselect_all (hWnd, ptedata, TRUE);
    }

    insertion->pos_lnOff = 0;
    insertion->curNode = FIRSTNODE(&ptedata->txtdoc);
    ptedata->curItemY = 0;
    teSetCaretPos (hWnd, ptedata);

    last = LASTNODE(&ptedata->txtdoc);
    selection->curNode = last;
    selection->pos_lnOff = last->content.txtlen;
    ptedata->selItemY = scrollview_get_total_item_height (&ptedata->svdata) 
                        - NODE_HEIGHT(last);
    /* FIXME, optimize */
    teSetCaretPos (hWnd, ptedata);

    return te_set_selection (hWnd, ptedata);
}

#endif

/*
 * te_set_position : Sets insertion or selection position
 * Params          : mark - insertion or selection point
 */
static TextNode*
te_set_position (HWND hWnd, PTEDATA ptedata, TextMark *mark, 
                 int line, int char_pos, int *item_y, int *newpos)
{
    TextNode *markNode;
    int *pos_chars;
    int nr_chars;

    if (char_pos < 0)
        return NULL;
    if ( !(markNode = get_node_by_idx (ptedata, line, item_y)) )
        return NULL;
    if (char_pos > markNode->content.txtlen)
        return NULL;

    pos_chars = ALLOCATE_LOCAL (markNode->content.txtlen * sizeof(int));
    nr_chars = GetTextMCharInfo (GetWindowFont (hWnd),
                    (const char*)(markNode->content.string), 
                    markNode->content.txtlen, pos_chars);

    if (char_pos > nr_chars) {
        DEALLOCATE_LOCAL (pos_chars);
        return NULL;
    }

    if (char_pos == nr_chars)
        char_pos = markNode->content.txtlen;
    else
        char_pos = pos_chars[char_pos];

    DEALLOCATE_LOCAL (pos_chars);

    if (newpos)
        *newpos = char_pos;
    return markNode;
}

static int
TE_OnMouseDown (HWND hWnd, HSVITEM hsvi, POINT* pt, int item_y, BOOL bShift)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2(hWnd);
    TextNode *node = NULL;
#ifdef _SELECT_SUPPORT
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextMark *selection = &txtdoc->selection;
#endif

    /* note that hsvi may be null */
    if (hsvi)
        node = (TextNode*)scrollview_get_item_adddata (hsvi);

#ifdef _SELECT_SUPPORT
    if (selection->curNode) {
        te_unselect_all (hWnd, ptedata, !bShift);
    }
#endif

    if (!bShift) {
        if (node)
            ptedata->des_caret_x = pt->x;
        teOnMouseDown (hWnd, node, pt, item_y, bShift);

        /*
	 * FIXME
         * ShowCaret (hWnd);
	 */
        /*
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CLICKED);
        */
        /*
	 * FIXME
         * NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
	 */
    }
#ifdef _SELECT_SUPPORT
    else {
        teOnMouseDown (hWnd, node, pt, item_y, bShift);
        te_set_selection (hWnd, ptedata);
    }
#endif

    return 0;
}

#ifdef _SELECT_SUPPORT
static void begin_selection (PTEDATA ptedata)
{
    ptedata->txtdoc.selection = ptedata->txtdoc.insert;
    ptedata->selItemY = ptedata->curItemY;
    ptedata->sel_x = ptedata->caret_x;
    ptedata->sel_y = ptedata->caret_y;
    SELECT_NODE(ptedata->txtdoc.selection.curNode, TRUE);
}
#endif

static void
te_cursor_updown (HWND hWnd, PTEDATA ptedata, int len, BOOL bShift)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node;
    TextMark *mark;
    int item_y;
    POINT pt;

#ifdef _SELECT_SUPPORT
    TextNode *org_node;

    if (!bShift && txtdoc->selection.curNode) {
        te_unselect_all (hWnd, ptedata, TRUE);
    }

    mark = bShift ? &txtdoc->selection : &txtdoc->insert;

    if (bShift && !mark->curNode) {
        begin_selection (ptedata);
    }
    node = mark->curNode;
    org_node = mark->curNode;
    item_y = bShift ? ptedata->selItemY : ptedata->curItemY;
#else
    mark = &txtdoc->insert;
    node = mark->curNode;
    item_y = ptedata->curItemY;
#endif

#ifdef _SELECT_SUPPORT
    if (bShift) {
        myGetSelPos (&pt);
    }
    else
#endif
        myGetCaretPos (&pt);

    pt.y += len * ptedata->nLineHeight;
    pt.x = ptedata->des_caret_x;

    if (len == 1) {  /* go to next line */
        if (pt.y >= item_y + NODE_HEIGHT(node)) {
            /* go to next node */
            if (BE_LAST_NODE(node))
                return;
            item_y += NODE_HEIGHT(node);
            node = TXTNODE_NEXT(node);
        }
    }
    else if (len == -1) {
        if (pt.y < item_y) {
            /* go to previous node */
            if (BE_FIRST_NODE(node))
                return;
            node = TXTNODE_PREV(node);
            item_y -= NODE_HEIGHT(node);
        }
    }
    else {
        /* TODO */
        return;
    }
    pt.y -= item_y;

    teOnMouseDown (hWnd, node, &pt, item_y, bShift);
    te_make_caret_visible (hWnd, ptedata, bShift);

#ifdef _SELECT_SUPPORT
    if (bShift) {
        if(NODE_IS_SELECTED(node)) {
            if (node != org_node)
                SELECT_NODE(org_node, FALSE);
        }
        else {
            SELECT_NODE(node, TRUE);
        }
        if (node != org_node)
            REFRESH_NODE(org_node);
        REFRESH_NODE(node);
        check_caret();
    }
#endif
}

/*
 * te_cursor_shift : process shift keys, including left and right arrow, Home,
 *                   End keys.
 * Params          : bShift - Whether shift key is in pressed status
 */ 
static void
te_cursor_shift (HWND hWnd, PTEDATA ptedata, int kcode, BOOL bShift)
{
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextNode *node, *curnode;
    unsigned char *pIns;
    int len;
    POINT pt;
    TextMark *mark;

#ifdef _SELECT_SUPPORT
    /* unselect the selected first */
    if (!bShift && txtdoc->selection.curNode) {
        te_unselect_all (hWnd, ptedata, TRUE);
    }

    mark = bShift ? &txtdoc->selection : &txtdoc->insert;

    if (bShift && !mark->curNode) {
        begin_selection (ptedata);
    }
#else
    mark = &txtdoc->insert;
#endif    

    node = mark->curNode;
    pIns = node->content.string + mark->pos_lnOff;

    if (kcode == SCANCODE_CURSORBLOCKRIGHT) {
        if (*pIns == '\0')
            return;
        else if (*pIns == txtdoc->lnsep || is_rn_sep(pIns)) {
            curnode = TXTNODE_NEXT(node);
            set_current_node (txtdoc, curnode, bShift, FALSE);
#ifdef _SELECT_SUPPORT
            if (bShift) {
                SELECT_NODE(node, (NODE_IS_SELECTED(curnode) ? FALSE : TRUE) );
                SELECT_NODE(curnode, TRUE);
                ptedata->selItemY += NODE_HEIGHT(node);
            }
            else
#endif
                ptedata->curItemY += NODE_HEIGHT(node);
        }
        else {
            mark->pos_lnOff += CHLENNEXT(pIns, (node->content.string + 
                                                node->content.txtlen - pIns));
        }
    }
    else if (kcode == SCANCODE_CURSORBLOCKLEFT) {
        len = -1;
        if (pIns == node->content.string) {
            if (BE_FIRST_NODE(node))
                return;
            else {
                curnode = TXTNODE_PREV(node);
                set_current_node (txtdoc, curnode, bShift, FALSE);
#ifdef _SELECT_SUPPORT
                if (bShift) {
                    SELECT_NODE(node, (NODE_IS_SELECTED(curnode) ? FALSE : TRUE) );
                    SELECT_NODE(curnode, TRUE);
                    ptedata->selItemY -= NODE_HEIGHT(TXTNODE_PREV(node));
                }
                else
#endif
                    ptedata->curItemY -= NODE_HEIGHT(TXTNODE_PREV(node));
                mark->pos_lnOff = TXTNODE_PREV(node)->content.txtlen - 1;
            }
        }
        else {
            mark->pos_lnOff -= CHLENPREV((const char *)(node->content.string), pIns);
        }
    }
    else if (kcode == SCANCODE_HOME || kcode == SCANCODE_END) {
        myGetCaretPos (&pt);
        pt.y -= ptedata->curItemY;
        pt.x = (kcode == SCANCODE_HOME)? 0 : scrolled_get_contwidth (ptescr) - 1;
        if (node)
            ptedata->des_caret_x = pt.x;
        teOnMouseDown (hWnd, node, &pt, ptedata->curItemY, bShift);
        te_make_caret_visible (hWnd, ptedata, bShift);
        goto SFRETURN;
    }

    teSetCaretPos (hWnd, ptedata);

SFRETURN:
#ifdef _SELECT_SUPPORT
    if (bShift) {
        REFRESH_NODE(mark->curNode);
        check_caret ();
    }
#endif
}

#ifdef _SELECT_SUPPORT

static void te_cursor_move (HWND hWnd, PTEDATA ptedata, POINT *pt)
{
    int item_h;
    TextDoc *txtdoc = &ptedata->txtdoc;
    TextMark *selection = &txtdoc->selection;
    TextNode *curnode;
    POINT oldpt, newpt;
    RECT rcInv;

    /* begin to select */
    if (!selection->curNode) {
        begin_selection (ptedata);
        return;
    }

    curnode = selection->curNode;

    item_h = scrollview_get_item_height (curnode->addData);
    if (pt->y >= ptedata->selItemY + item_h) {
        TextNode *next = TXTNODE_NEXT (curnode);
        if (next) {
            selection->curNode = next;
            ptedata->selItemY += item_h;
        }
        else {
            if (selection->pos_lnOff != curnode->content.txtlen) {
                selection->pos_lnOff = curnode->content.txtlen;
                REFRESH_NODE(curnode);
                check_caret ();
            }
            return;
        }
        /* FIXME */
        pt->y = ptedata->selItemY + item_h + ptedata->nLineHeight/2;
    }
    else if (pt->y < ptedata->selItemY) {
        TextNode *prev = TXTNODE_PREV (curnode);
        if (prev) {
            selection->curNode = prev;
            item_h = scrollview_get_item_height ((HSVITEM)prev->addData);
            ptedata->selItemY -= item_h;
        }
        else {
            if (selection->pos_lnOff != 0) {
                selection->pos_lnOff = 0;
                REFRESH_NODE(curnode);
                check_caret ();
            }
            return;
        }
        pt->y = ptedata->selItemY - ptedata->nLineHeight/2;
    }

    if (ptedata->flags & TEST_MOVE) {
        /*
        int step;
        step = pt->y - ptescr->nContY - ptescr->visibleHeight;
        step = step > ptedata->nLineHeight ? 6 : 3;
        */
        scrolled_make_pos_visible (hWnd, ptescr, pt->x, pt->y);
    }
    pt->y -= ptedata->selItemY;

    /* moves to a new line */
    if (curnode && selection->curNode != curnode) {
        /* moves off the current selected line */
        if (NODE_IS_SELECTED(selection->curNode))
            SELECT_NODE(curnode, FALSE);
        else
            SELECT_NODE(selection->curNode, TRUE);
        REFRESH_NODE(curnode);
    }

    myGetSelPos (&oldpt);
    set_caret_pos (hWnd, ptedata, selection->curNode, pt->x, pt->y, TRUE);
    myGetSelPos (&newpt);
#if 0
    if (oldpt.x != newpt.x || oldpt.y != newpt.y)
        REFRESH_NODE(selection->curNode);
#else
    /* for bug 2763 (wangjian 2008-05-20) */
    /* for bug 3104, houhh 20081029, rcInv is relative to current item rect.*/
    if (oldpt.x != newpt.x && oldpt.y == newpt.y)
    {
        rcInv.left   = newpt.x > oldpt.x ? oldpt.x : newpt.x;
        rcInv.top    = 0; 
        rcInv.right  = oldpt.x > newpt.x ? oldpt.x : newpt.x;
        rcInv.bottom = rcInv.top + ptedata->nLineHeight - ptedata->nLineBaseH;     
        
        REFRESH_NODE_EX(selection->curNode, &rcInv);
    }else if (oldpt.y != newpt.y) { 
        rcInv.left   = 0;
        rcInv.top    = 0;
        rcInv.right  = rcInv.left + ptedata->svdata.scrdata.nContWidth;
        //rcInv.bottom = (newpt.y > oldpt.y ? newpt.y : oldpt.y) 
        rcInv.bottom = abs(newpt.y - oldpt.y) 
            + ptedata->nLineHeight - ptedata->nLineBaseH;    
        
        REFRESH_NODE_EX(selection->curNode, &rcInv);
    }
#endif
    check_caret ();
}
#endif

static SVITEMOPS textedit_iops =
{
    NULL, NULL, teDrawItem
};

void textedit_set_draw_selected (HWND hWnd, ED_DRAWSEL_FUNC drawSel)
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    ptedata->drawSelected = drawSel;
    InvalidateRect (hWnd, NULL, TRUE);
}

void textedit_set_caret_func (HWND hWnd, int (*getcaretwidth) (HWND, int) )
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    ptedata->getCaretWidth = getcaretwidth;
}

void textedit_refresh_caret (HWND hWnd)
{
    mySetCaretPos (hWnd, -1, -1);
}

/* --------------------------------------------------------------------------------- */

static int textedit_get_pos_ex (HWND hWnd, int *line_pos, int *char_pos, BOOL bSel)
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    TextDoc *txtdoc = &ptedata->txtdoc;
    int nr_chars = 0;
    TextMark *mark;

    mark = GETMARK(bSel);
    if (!mark->curNode)
        return -1;

    if (line_pos) {
        *line_pos = NODE_INDEX(mark->curNode);
    }
    if (char_pos) {
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd), 
                      (const char*)mark->curNode->content.string, mark->pos_lnOff, NULL);
        *char_pos = nr_chars;
    }

    return mark->pos_lnOff;
}

int textedit_get_caretpos (HWND hWnd, int *line_pos, int *char_pos)
{
    return textedit_get_pos_ex (hWnd, line_pos, char_pos, FALSE);
}

int textedit_get_selpos (HWND hWnd, int *line_pos, int *char_pos)
{
    return textedit_get_pos_ex (hWnd, line_pos, char_pos, TRUE);
}

static int textedit_set_pos_ex (HWND hWnd, int line_pos, int char_pos, BOOL bSel)
{
    PTEDATA ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);
    TextDoc *txtdoc = &ptedata->txtdoc;
    int item_y, newpos;
    TextMark *mark;
    TextNode *node;

    if ( !(mark = GETMARK(bSel)) )
        return -1;

    if ( !(node = te_set_position (hWnd, ptedata, mark, 
                                   line_pos, char_pos, &item_y, &newpos)) )
        return -1;

#ifdef _SELECT_SUPPORT
    /* unselect the former seleted before mark is changed */
    if (txtdoc->selection.curNode) {
        te_unselect_all (hWnd, ptedata, TRUE);
    }
#endif
    SETITEMY(bSel, item_y);

    mark->pos_lnOff = newpos;
    mark->curNode = node;

#ifdef _SELECT_SUPPORT
    if (bSel)
        te_set_selection (hWnd, ptedata);
    else
#endif
        teSetCaretPos (hWnd, ptedata);
    return mark->pos_lnOff;
}

#ifdef _TITLE_SUPPORT
static int 
textedit_get_titletext (HWND hWnd, PTEDATA ptedata, int tlen, char *buffer)
{
    int len, title_len;

    if (!ptedata->title)
        return -1;

    title_len = strlen (ptedata->title);

    if (!buffer)
        return title_len;

    if (tlen >= 0)
        len = (tlen > DEF_TITLE_LEN) ? DEF_TITLE_LEN : tlen;
    else
        len = DEF_TITLE_LEN;

    strncpy (buffer, ptedata->title, len);
    buffer[len] = '\0';

    return title_len;
}

static int 
textedit_set_titletext (HWND hWnd, PTEDATA ptedata, int tlen, const char *newtitle)
{
    int len;
    HDC hdc;

    if (!ptedata->title || !newtitle)
        return -1;

    if (tlen >= 0)
        len = (tlen > DEF_TITLE_LEN) ? DEF_TITLE_LEN : tlen;
    else
        len = DEF_TITLE_LEN;
    strncpy (ptedata->title, newtitle, len);
    ptedata->title[len] = '\0';

    hdc = GETDC (hWnd);
    SelectFont (hdc, GetWindowFont(hWnd));
    ptedata->titleIndent = teGetTitleIndent (hWnd, ptedata, hdc);
    RELEASEDC (hWnd, hdc);
    mySetCaretPos (hWnd, ptedata->titleIndent, 0 + ptedata->nLineAboveH);

    return strlen (ptedata->title);
}
#endif

static void
textedit_reset_content (HWND hWnd, PTEDATA ptedata, const char *newtext, BOOL bLast)
{
    TextNode *node=NULL;
    TextDoc *txtdoc = &ptedata->txtdoc;

    node = FIRSTNODE(&ptedata->txtdoc);
    if (node)
        node->addData = 0;

    scrollview_reset_content (hWnd, &ptedata->svdata);
    teResetData (ptedata);

    scrollview_freeze (hWnd, &ptedata->svdata, TRUE);
    textdoc_settext (txtdoc, newtext);
    scrollview_freeze (hWnd, &ptedata->svdata, FALSE);

    if ( bLast && !(GetWindowStyle(hWnd) & ES_READONLY) ) {
        TextNode *node = LASTNODE(txtdoc);
        txtdoc->insert.curNode = node;
        txtdoc->insert.pos_lnOff = node->content.txtlen;
        teNodeChange (txtdoc, FALSE);
        teSetCaretPos (hWnd, ptedata);
    }
    else {
        mySetCaretPos (hWnd, teGetLineIndent(ptedata, FIRSTNODE(txtdoc)), 
                    0 + ptedata->nLineAboveH);
    }

    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_UPDATE);
    ptedata->cont_dirty = TRUE;
}

int textedit_insert_text (HWND hWnd, const char* text, int len)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 (hWnd);
    TextDoc *txtdoc = &ptedata->txtdoc;
    int ret;
    /*
    TextNode *node = txtdoc->insert.curNode;
    TextNode *nextnode = NULL;
    
    if (!BE_LAST_NODE(node))
        nextnode = TXTNODE_NEXT (node);
    */

    if (!text || len <= 0)
        return -1;

    ret = textdoc_insert_text (txtdoc, text, len);

    teNodeChange (txtdoc, FALSE);
    teSetCaretPos (hWnd, ptedata);

    /* FIXME */
    REFRESH_NODE(txtdoc->insert.curNode);

    /*
    if (set_line_size (hWnd, ptedata, node) < 0)
        REFRESH_NODE(node);

    if (nextnode)
        set_line_size (hWnd, ptedata, TXTNODE_PREV(nextnode));
    else
        set_line_size (hWnd, ptedata, LASTNODE(txtdoc));
    */

    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    ptedata->cont_dirty = TRUE;
    return ret;
}

#ifdef _MGHAVE_CLIPBOARD
/*
 * textedit_insert_cbtext : inserts clipboard texts into the current insertion point
 */
static int textedit_insert_cbtext (HWND hWnd, PTEDATA ptedata)
{
    int  inserting;
    unsigned char *txtBuffer;

    inserting = GetClipBoardDataLen (CBNAME_TEXT);
    txtBuffer = (unsigned char *)FixStrAlloc (inserting);
    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);

    textedit_insert_text (hWnd, (const char*)txtBuffer, inserting);

    FreeFixStr((char*)txtBuffer);

    return inserting;
}
#endif

/*
 * initialize text edit object structure
 */
static int teInitData (HWND hWnd, PTEDATA ptedata)
{
    int pageval;
    TextDoc *txtdoc = &ptedata->txtdoc;

    ptedata->hardLimit = -1;
    ptedata->curlen = 0;

    /* init scrollview object */
    scrollview_init (hWnd, &ptedata->svdata);
    
    /* change default move_content function */
    ptedata->svdata.scrdata.move_content = textedit_set_svlist;

    ptedata->nLineHeight = DEF_LINE_HEIGHT;
    ptedata->nLineAboveH = DEF_LINE_ABOVE_H;
    ptedata->nLineBaseH  = DEF_LINE_BASE_H;

#ifdef _TITLE_SUPPORT
    if (GetWindowStyle(hWnd) & ES_TITLE) {
        ptedata->title = FixStrAlloc (DEF_TITLE_LEN+1);
        ptedata->title[0] = '\0';
        ptedata->titleIndent = 0;
    }
    else {
        ptedata->title = NULL;
        ptedata->titleIndent = 0;
    }
#endif

    scrolled_set_scrollval (ptescr, 0, ptedata->nLineHeight);
    pageval = (ptescr->visibleHeight / ptedata->nLineHeight ) *
                    ptedata->nLineHeight;
    scrolled_set_scrollpageval (ptescr, 0, pageval);

    ptedata->lnChar = 0;
    ptedata->caretShape = ED_CARETSHAPE_LINE;
    teResetData (ptedata);

    scrollview_set_itemops (&ptedata->svdata, (SVITEMOPS*)&textedit_iops);
    ptedata->drawSelected = NULL;
    ptedata->getCaretWidth = NULL;

    /* init text document object */
    INIT_LIST_HEAD(&txtdoc->queue);
    memset (&txtdoc->insert, 0, sizeof(txtdoc->insert));
#ifdef _SELECT_SUPPORT
    txtdoc->selection.curNode = NULL;
#endif

    textdoc_reset (hWnd, txtdoc);

    /* FIXME, where to put? */
    SetWindowAdditionalData2 (hWnd, (DWORD) ptedata);

    scrollview_freeze (hWnd, &ptedata->svdata, TRUE);
    textdoc_settext (txtdoc, GetWindowCaption(hWnd));
    scrollview_freeze (hWnd, &ptedata->svdata, FALSE);
    /*Init temp StrBuffer*/
    testr_alloc (&(ptedata->tebuff), 128, 256);

    return 0;
}

#ifdef _UNDO_SUPPORT

/*
static void teUndoBackup (TextDoc *txtdoc)
{
    PTEDATA ptedata = (PTEDATA)GetWindowAdditionalData2 ((HWND)txtdoc->fn_data);
    TextNode *curnode = txtdoc->insert.curNode;

    if ( (ptedata->act_count %= ACTION_COUNT) == 0 ) {
        BACKUP_DATA();
        ptedata->bkIns = txtdoc->insert;
        ptedata->bkSel = txtdoc->selection;
        testr_free (ptedata->bkBuff);
        free (ptedata->bkBuff);
        ptedata->bkBuff = testr_dup (&curnode->content);
        printf ("back up : %s\n", ptedata->bkBuff->string);
        ptedata->act_count = 0;
    }
    ptedata->act_count ++;
}
*/

static int te_init_undo (HWND hWnd, PTEDATA ptedata)
{
    ptedata->undo_depth = DEF_UNDO_DEPTH;

    ptedata->bkData = calloc (1, sizeof(BKDATA)*ptedata->undo_depth);
    if (!ptedata->bkData)
        return -1;

    ptedata->undo_level = -1;
    ptedata->act_count = 0;

    return 0;
}

#endif

static int textedit_init (HWND hWnd, PTEDATA ptedata)
{
	HDC hdc ;
    if (!ptedata)
        return -1;

    teInitData (hWnd, ptedata);

    if (BE_WRAP(hWnd))
        ShowScrollBar (hWnd, SB_HORZ, FALSE);

    // create caret with the max font width, so that the
    // caret bitmap memery is enought for caret shape changed.
    hdc = GETDC (hWnd);
    CreateCaret (hWnd, NULL, GetMaxFontWidth(hdc), 
            ptedata->nLineHeight - ptedata->nLineAboveH- ptedata->nLineBaseH);
    RELEASEDC (hWnd, hdc);
    /* SetFocus (hWnd); */

    mySetCaretPos (hWnd, ptedata->des_caret_x, 0 + ptedata->nLineAboveH);

#ifdef _UNDO_SUPPORT
    if (te_init_undo (hWnd, ptedata) < 0)
        return -1;
#endif
    return 0;
}

/*
 * destroy a textedit
 */
static int textedit_destroy (HWND hWnd, PTEDATA ptedata)
{
#ifdef _TITLE_SUPPORT
    if (GetWindowStyle(hWnd) & ES_TITLE) {
        FreeFixStr (ptedata->title);
    }
#endif
    DestroyCaret (hWnd);
    scrollview_destroy (&ptedata->svdata);
    textdoc_free (&ptedata->txtdoc);
    testr_free (&(ptedata->tebuff));
    ptedata->hardLimit = -1;
    ptedata->curlen = 0;
    return 0;
}

/* --------------------------------------------------------------------------------- */

#define RETURN_DIRECT   return DefaultControlProc(hWnd, message, wParam, lParam)

static void init_bufdc (HWND hWnd, PTEDATA ptedata)
{
    RECT rc;
    HDC hdc;
    GetClientRect(hWnd, &rc);
    if (ptedata->buf_dc != 0) {
        int w = GetGDCapability(ptedata->buf_dc, GDCAP_MAXX) + 1;
        int h = GetGDCapability(ptedata->buf_dc, GDCAP_MAXY) + 1;
        if (w == RECTW(rc) && h == RECTH(rc)) {
            return;
        }
        DeleteCompatibleDC(ptedata->buf_dc);
    }
    hdc = GetClientDC(hWnd);
    ptedata->buf_dc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
    if (ptedata->buf_dc == HDC_INVALID) {
        fprintf(stderr, "create textedit buf_dc error!\n");
    }
    DefaultControlProc (hWnd, MSG_ERASEBKGND, (WPARAM)ptedata->buf_dc, 0);
    ReleaseDC(hdc);
}

static LRESULT TextEditCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PTEDATA ptedata = NULL;

    if (message != MSG_CREATE)
        ptedata = (PTEDATA) GetWindowAdditionalData2 (hWnd);


    switch (message) {

        case MSG_CREATE:
            {
                if (!(ptedata = (PTEDATA) calloc (1, sizeof (TEDATA))))
                    return -1;
                textedit_init (hWnd, ptedata);
                ptedata->caretShape = 1;
                RETURN_DIRECT;
            }

        case MSG_DESTROY:
            {
                textedit_destroy (hWnd, ptedata);
                free (ptedata);
                DeleteCompatibleDC(ptedata->buf_dc);
                RETURN_DIRECT;
            }

        case MSG_SETCURSOR:
            if (GetWindowStyle(hWnd) & WS_DISABLED) {
                SetCursor (GetSystemCursor (IDC_ARROW));
                return 0;
            }
            break;

        case MSG_SETFOCUS:
            ptedata->flags |= TEST_FOCUSED;
            ptedata->cont_dirty = FALSE;
            mySetCaretPos (hWnd, -1, -1);
            ptedata->ex_flags |= TEST_EX_SETFOCUS;
            ptedata->ex_flags |= ~TEST_EX_KILLFOCUS;
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SETFOCUS);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_KILLFOCUS:
            {
                DWORD dwStyle = GetWindowStyle (hWnd);
                ptedata->flags &= ~TEST_FOCUSED;
                HideCaret (hWnd);
                if (ptedata->cont_dirty == TRUE) {
                    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CONTCHANGED);
                    ptedata->cont_dirty = FALSE;
                }
                ptedata->ex_flags &= ~TEST_EX_SETFOCUS;
                ptedata->ex_flags |= TEST_EX_KILLFOCUS;
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_KILLFOCUS);
                if (!(dwStyle & ES_NOHIDESEL)) {
                    te_unselect_all (hWnd, ptedata, FALSE);
                }
                InvalidateRect (hWnd, NULL, TRUE);
                break;
            }

        case MSG_DOESNEEDIME:
            if (GetWindowStyle(hWnd) & ES_READONLY)
                return IME_WINDOW_TYPE_READONLY;
            else
                return IME_WINDOW_TYPE_EDITABLE;

        case MSG_GETDLGCODE:
            return DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS | DLGC_WANTENTER;

        case MSG_LBUTTONDBLCLK:
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), EN_DBLCLK);
            break;
#ifdef TEXTEDIT_BUFDC
        case MSG_ERASEBKGND:
            SelectClipRect(ptedata->buf_dc, NULL);
            DefaultControlProc (hWnd, MSG_ERASEBKGND, (WPARAM)ptedata->buf_dc, lParam);
            return 0;
        case MSG_CHANGESIZE:
            DefaultControlProc (hWnd, MSG_CHANGESIZE, wParam, lParam);
            init_bufdc (hWnd, ptedata);
            return 0;
#endif
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hWnd);
                RECT rcDraw, rcVis;

                scrolled_get_content_rect (ptescr, &rcDraw);
                scrolled_visible_to_window (ptescr, &rcDraw.left, &rcDraw.top);
                scrolled_visible_to_window (ptescr, &rcDraw.right, &rcDraw.bottom);

#ifndef TEXTEDIT_BUFDC
                scrolled_get_visible_rect (ptescr, &rcVis);
                ClipRectIntersect (hdc, &rcVis);
                tePaint (hWnd, hdc, &rcDraw);
                scrollview_draw (hWnd, hdc, &ptedata->svdata);
#else
                scrolled_get_visible_rect (ptescr, &rcVis);
                SelectClipRect(ptedata->buf_dc, NULL);
                ClipRectIntersect (ptedata->buf_dc, &rcVis);
                tePaint (hWnd, ptedata->buf_dc, &rcDraw);
                scrollview_draw (hWnd, ptedata->buf_dc, &ptedata->svdata);
                BitBlt(ptedata->buf_dc, 0, 0, 0, 0, hdc, 0, 0, 0);
#endif

                if ((ptedata->ex_flags & TEST_EX_SETFOCUS)){
                    ptedata->ex_flags &= ~TEST_EX_SETFOCUS;
                }
                else if (ptedata->ex_flags & TEST_EX_KILLFOCUS) {
                    ptedata->ex_flags &= ~TEST_EX_KILLFOCUS;
                }
                EndPaint (hWnd, hdc);
                return 0;
            }

        case MSG_KEYDOWN:
            {
#ifdef _TEXTEDIT_DEBUG
                if (wParam == SCANCODE_F1) {
                    /* print_selected (&ptedata->txtdoc); */
                    printf ("current node item y = %d\n", ptedata->curItemY);
                    printf ("current total item h = %d\n", 
                            ptedata->svdata.svlist.nTotalItemH);
                }
#endif
                switch (wParam) {
                    case SCANCODE_REMOVE:
                        teOnChar (hWnd, ptedata, 127);
                        return 0;
                    case SCANCODE_CURSORBLOCKLEFT:
                    case SCANCODE_CURSORBLOCKRIGHT:
                    case SCANCODE_HOME:
                    case SCANCODE_END:
                        te_cursor_shift (hWnd, ptedata, wParam, lParam & KS_SHIFT);
                        return 0;
                    case SCANCODE_CURSORBLOCKUP:
                        te_cursor_updown (hWnd, ptedata, -1, lParam & KS_SHIFT);
                        return 0;
                    case SCANCODE_CURSORBLOCKDOWN:
                        te_cursor_updown (hWnd, ptedata, 1, lParam & KS_SHIFT);
                        return 0;
#if defined(_MGHAVE_CLIPBOARD)
#if defined(_SELECT_SUPPORT)
                    case SCANCODE_C:
                        if (lParam & KS_CTRL) {
                            textdoc_copy_to_cb (ptedata, NULL, 0, TRUE);
                        }
                        return 0;
#endif
                    case SCANCODE_V:
                        if (lParam & KS_CTRL) {
                            return textedit_insert_cbtext (hWnd, ptedata);
                        }
                        return 0;
#endif
#ifdef _SELECT_SUPPORT
                    case SCANCODE_A:
                        if (lParam & KS_CTRL) {
                            te_select_all (hWnd, ptedata);
                        }
                        return 0;
#ifdef _MGHAVE_CLIPBOARD
                    case SCANCODE_X:
                        if (lParam & KS_CTRL && ptedata->txtdoc.selection.curNode) {
                            textdoc_copy_to_cb (ptedata, NULL, 0, TRUE);
                            teOnChar (hWnd, ptedata, 127);
                        }
                        return 0;
#endif
#endif
#ifdef _UNDO_SUPPORT
                        /*
                           case SCANCODE_Z:
                           if (lParam & KS_CTRL)
                           teUndo (hWnd, ptedata);
                           return 0;
                           case SCANCODE_R:
                           if (lParam & KS_CTRL)
                           teRedo (hWnd, ptedata);
                           return 0;
                           */
#endif
                } /* end switch keydown */
                break;
            }

        case MSG_UTF8CHAR:
            _ERR_PRINTF ("CONTROL>EDIT: MSG_UTF8CHAR is not implemented.\n");
            break;

        case MSG_CHAR:
            if (wParam == 127) /* BS */
                wParam = '\b';
            teOnChar (hWnd, ptedata, wParam);
            return 0;

        case MSG_LBUTTONUP:
#ifdef _SELECT_SUPPORT
            if (GetCapture() == hWnd)
                ReleaseCapture();
            if (ptedata->flags & TEST_SELECT)
                ptedata->flags &= ~TEST_SELECT;
            if (ptedata->flags & TEST_MOVE)
                ptedata->flags &= ~TEST_MOVE;
#endif
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CLICKED);
            break;

        case MSG_LBUTTONDOWN:
            {
                int mouseX = LOSWORD (lParam);
                int mouseY = HISWORD (lParam);
                HSVITEM hsvi = 0;
                int nItem, item_y = 0;
                POINT pt;

#ifdef _SELECT_SUPPORT
                if (!GetCapture()) {
                    SetCapture(hWnd);
                    ptedata->flags |= TEST_SELECT;
                }
#endif

                scrolled_window_to_visible (ptescr, &mouseX, &mouseY);
                scrolled_visible_to_content (ptescr, &mouseX, &mouseY);
                pt.x = mouseX;
                pt.y = mouseY;
                nItem = scrollview_is_in_item (&ptedata->svdata, mouseY, &hsvi, &item_y);
                if (nItem >= 0) {
                    scrolled_make_pos_visible (hWnd, ptescr, pt.x, pt.y);
                    pt.y -= item_y;
                }

                TE_OnMouseDown (hWnd, hsvi, &pt, item_y, wParam & KS_SHIFT);
                return 0;
            }

#ifdef _SELECT_SUPPORT
        case MSG_MOUSEMOVE:
            {
                int mouseX = LOSWORD (lParam);
                int mouseY = HISWORD (lParam);
                POINT pt;
                RECT rcVis;

                if (GetCapture() != hWnd || !(ptedata->flags & TEST_SELECT))
                    break;

                ScreenToClient (hWnd, &mouseX, &mouseY);

                scrolled_get_visible_rect (ptescr, &rcVis);

                if (!PtInRect (&rcVis, mouseX, mouseY)) {
                    if ( !(ptedata->flags & TEST_MOVE) ) {
                        ptedata->flags |= TEST_MOVE;
                        SetAutoRepeatMessage (hWnd, MSG_MOUSEMOVE, 0, lParam);
                    }
                }
                else {
                    if ( ptedata->flags & TEST_MOVE ) {
                        ptedata->flags &= ~TEST_MOVE;
                        /* cancel repeat message */
                        SetAutoRepeatMessage (0, 0, 0, 0);
                    }
                }

                scrolled_window_to_visible (ptescr, &mouseX, &mouseY);
                scrolled_visible_to_content (ptescr, &mouseX, &mouseY);

                pt.x = mouseX;
                pt.y = mouseY;

                te_cursor_move (hWnd, ptedata, &pt);
                break;
            }
#endif

        case MSG_GETTEXTLENGTH:
            return textdoc_get_textlen(&ptedata->txtdoc);

        case MSG_GETTEXT:
            return textdoc_gettext (&ptedata->txtdoc, wParam, (unsigned char*)lParam);

        case MSG_SETTEXT:
            textedit_reset_content (hWnd, ptedata, (const char*)lParam, wParam);
            return 0;

            /*
               case MSG_FREEZECTRL:
               scrollview_freeze (hWnd, &ptedata->svdata, wParam);
               return 0;
               */

            /*
             * FIXME,TODO
             */
        case MSG_FONTCHANGED:
            {
                HDC hdc = GETDC (hWnd);
                TextDoc *txtdoc = &ptedata->txtdoc;
                TextNode *node = FIRSTNODE(txtdoc);

                DestroyCaret (hWnd);
                ptedata->nLineHeight = GetWindowFont(hWnd)->size + 
                    ptedata->nLineAboveH + ptedata->nLineBaseH;

                // create caret with the max font width, so that the
                // caret bitmap memery is enought for caret shape changed.
                CreateCaret (hWnd, NULL, GetMaxFontWidth(hdc), 
                        ptedata->nLineHeight - ptedata->nLineAboveH- ptedata->nLineBaseH);

                while (node) {
                    txtdoc->change_cont (txtdoc, node);
                    node = TXTNODE_NEXT(node);
                }
                teSetCaretPos (hWnd, ptedata);
                SelectFont(hdc, GetWindowFont(hWnd));
                RELEASEDC (hWnd, hdc);
                if (ptedata->title)
                    textedit_set_titletext (hWnd, ptedata, strlen(ptedata->title), ptedata->title);

                InvalidateRect (hWnd, NULL, TRUE);
                return 0;
            }

        case TEM_RESETCONTENT:
            textedit_reset_content (hWnd, ptedata, "", FALSE);
            return 0;

#ifdef _TITLE_SUPPORT
        case EM_GETTITLETEXT:
            return textedit_get_titletext (hWnd, ptedata, wParam, (char *)lParam);

        case EM_SETTITLETEXT:
            return textedit_set_titletext (hWnd, ptedata, wParam, (const char *)lParam);
#endif

        case EM_GETLINECOUNT:
            return scrollview_get_total_item_height(&ptedata->svdata) / ptedata->nLineHeight;

        case EM_GETLINEHEIGHT:
            return ptedata->nLineHeight;

        case EM_SETLINEHEIGHT:
            {
                int old_h = ptedata->nLineHeight, height = (int)wParam;
                int new_h;

                if (height <= 0)
                    return -1;

                new_h = GetWindowFont(hWnd)->size + DEF_LINE_BASE_H + MIN_LINE_ABOVE_H;
                new_h = MAX (new_h, height);

                if (new_h == old_h)
                    return -1;

                ptedata->nLineHeight = new_h;
                ptedata->nLineAboveH = new_h - GetWindowFont(hWnd)->size - ptedata->nLineBaseH;

                textedit_reset_content (hWnd, ptedata, "", FALSE);

                return old_h;
            }

        case EM_SETREADONLY:
            {
                if (wParam)
                    IncludeWindowStyle (hWnd, ES_READONLY);
                else
                    ExcludeWindowStyle (hWnd, ES_READONLY);
                return 0;
            }
        case EM_LIMITTEXT:
            {
                int newLimit = (int)wParam;

                if (newLimit >= 0) {
                    if (ptedata->curlen > newLimit) {
                        Ping ();
                        return -1;
                    }
                    else {
                        ptedata->hardLimit = newLimit;
                        return 0;
                    }
                }
                return -1;
            }

        case EM_GETLIMITTEXT:
            {
                return ptedata->hardLimit;
            }
#ifdef _SELECT_SUPPORT
#ifdef _MGHAVE_CLIPBOARD
        case EM_GETSEL:
            {
                char *buffer = (char *)lParam;
                int len = (int)wParam;

                if (!buffer && len != 0)
                    return 0;
                return textdoc_copy_to_cb (ptedata, buffer, len, FALSE);
            }
#endif
        case EM_SELECTALL:
            return te_select_all (hWnd, ptedata);

#endif

        case EM_GETSELPOS:
        case EM_GETCARETPOS:
            {
                return textedit_get_pos_ex (hWnd, (int *)wParam, (int *)lParam,
                        message==EM_GETSELPOS);
            }

        case EM_SETCARETPOS:
        case EM_SETSEL:
            {
                return textedit_set_pos_ex (hWnd, wParam, lParam, message == EM_SETSEL);
            }

        case EM_INSERTTEXT:
            {
                return textedit_insert_text (hWnd, (const char*)lParam, wParam);
            }

#ifdef _MGHAVE_CLIPBOARD
        case EM_INSERTCBTEXT:
            {
                if (GetWindowStyle(hWnd) & ES_READONLY)
                    return 0;
                return textedit_insert_cbtext (hWnd, ptedata);
            }
#ifdef _SELECT_SUPPORT
        case EM_COPYTOCB:
        case EM_CUTTOCB:
            {
                int len;
                len = textdoc_copy_to_cb (ptedata, NULL, 0, TRUE);
                if (message == EM_CUTTOCB)
                    teOnChar (hWnd, ptedata, 127);
                return len;
            }
#endif
#endif

        case EM_SETLFDISPCHAR:
            ptedata->lnChar = lParam;
            scrollview_refresh_content (&ptedata->svdata);
            return 0;

            /* must be setted before set text */
        case EM_SETLINESEP:
            ptedata->txtdoc.lnsep = lParam;
            return 0;

        case EM_CHANGECARETSHAPE:
            {
                int old = ptedata->caretShape;
                ptedata->caretShape = wParam;
                textedit_refresh_caret (hWnd);
                return old;
            }

        case EM_SETDRAWSELECTFUNC:
            textedit_set_draw_selected(hWnd, (ED_DRAWSEL_FUNC)lParam);
            return 0;

        case EM_SETGETCARETWIDTHFUNC:
            textedit_set_caret_func (hWnd, (int(*)(HWND, int))lParam);
            return 0;

        case EM_REFRESHCARET:
            textedit_refresh_caret (hWnd);
            return 0;

        case EM_ENABLECARET:
            {
                if (wParam && (ptedata->flags & TEST_NOCARET)) {
                    ptedata->flags &= ~TEST_NOCARET;
                    if (lParam)
                        teSetCaretPos (hWnd, ptedata);
                }
                else if (!wParam && !(ptedata->flags & TEST_NOCARET)) {
                    ptedata->flags |= TEST_NOCARET;
                    HideCaret (hWnd);
                }
                return 0;
            }

        case EM_GETNUMOFPARAGRAPHS:
            return textdoc_get_num_paragraphs (&ptedata->txtdoc);

        case EM_GETPARAGRAPHTEXT:
            {
                TEXTPOSINFO *info = (TEXTPOSINFO*)wParam;
                int start = info->start_pos;
                int len = info->copy_len;
                int index = info->paragraph_index;
                unsigned char* buff = (unsigned char*)(info->buff);

                return textdoc_get_paragraph_text (hWnd, &ptedata->txtdoc, 
                        index, start, len, buff);
            }

        case EM_GETPARAGRAPHLENGTH:
            return textdoc_get_paragraph_length (GetWindowFont(hWnd), &ptedata->txtdoc, wParam);

        case EM_UNDO:
            /* teUndo (hWnd, ptedata); */
            return 0;
        case EM_REDO:
            /* teRedo (hWnd, ptedata); */
            return 0;

    }/* end switch */

    return ScrollViewCtrlProc (hWnd, message, wParam, lParam);
}

BOOL RegisterTextEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_TEXTEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = TextEditCtrlProc;

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    WndClass.spClassName = CTRL_MLEDIT;

    if (AddNewControlClass (&WndClass) != ERR_OK)
        return FALSE;

    WndClass.spClassName = CTRL_MEDIT;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif /* _MGCTRL_TEXTEDIT */

