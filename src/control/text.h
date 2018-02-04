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
** text.h: header file of text operations.
*/

#ifndef _MG_TEXT_H
#define _MG_TEXT_H


#ifdef __cplusplus
extern "C"
{
#endif


/* structure to represent a text string */
typedef struct strbuffer_s
{
    unsigned char *string;     /* text string content */
    int           txtlen;      /* text len or buffer used size */
    int           blocksize;   /* block size, a block is an allocate unit */
    int           buffsize;    /* buffer size */
} StrBuffer;

/* ------------------------- memory and struct alloc ------------------------ */

/* text string content buffer allocation and free */
#if 1
  #define te_alloc                         malloc
  #define te_free                          free
static inline void* te_realloc (void* ptr, size_t size, size_t cpysize) {
    return realloc (ptr, size);
}
#else
  #define te_alloc                         FixStrAlloc
  #define te_free                          FixStrFree
static inline void* te_realloc (void* ptr, size_t size, size_t cpysize)
{
    void* newptr;

    if ( !(newptr = FixStrAlloc (size)) )
        return NULL;

    memcpy (newptr, ptr, MIN(cpysize,size));
    FixStrFree (ptr);

    return newptr;
}
#endif

/* -------------------------------------------------------------------------- */

/* alloc with a unit of blocksize */
static inline void*
testr_alloc (StrBuffer *txtbuff, size_t len, size_t blocksize)
{
    txtbuff->buffsize = (len + 1) + (blocksize - (len + 1)%blocksize);
    txtbuff->string = te_alloc (txtbuff->buffsize);
    txtbuff->blocksize = blocksize;
    //txtbuff->txtlen = len;
    return txtbuff->string;
}

static inline void*
testr_realloc (StrBuffer *txtbuff, size_t len)
{
    if (len + 1 > txtbuff->buffsize || 
             len + 1 < txtbuff->buffsize - txtbuff->blocksize) {
        /* really realloc */
        size_t old_size;
        old_size  = txtbuff->buffsize;
        txtbuff->buffsize = (len + 1) + (txtbuff->blocksize - 
                            (len + 1)%txtbuff->blocksize);
        txtbuff->string = te_realloc (txtbuff->string, txtbuff->buffsize, old_size);
    }

    return txtbuff->string;
}

static inline void testr_free (StrBuffer *txtbuff)
{
    if (txtbuff) {
        te_free (txtbuff->string);
    }
}

static inline void
testr_setstr (StrBuffer *txtbuff, const char *newstring, int len)
{
    int datalen;

    if (len < 0 && !newstring) return;

    datalen = MIN(len, txtbuff->buffsize-1);
    memcpy (txtbuff->string, newstring, datalen);
    txtbuff->string[datalen] = '\0';
    txtbuff->txtlen = datalen;
}

/* copy a string buffer */
static inline StrBuffer* testr_copy (StrBuffer *des, StrBuffer *src)
{
    if (!des || !src)
        return NULL;

    testr_free (des);
    testr_alloc (des, src->txtlen, des->blocksize);
    testr_setstr (des, (const char *)src->string, src->txtlen);
    return des;
}

/* duplicates a string buffer */
static inline StrBuffer* testr_dup (StrBuffer *s1)
{
    StrBuffer *s2;

    if (!s1)
        return NULL;

    s2 = malloc (sizeof(s1));
    if (s2) {
        testr_alloc (s2, s1->txtlen, s1->blocksize);
        testr_setstr (s2, (const char *)s1->string, s1->txtlen);
    }
    return s2;
}

/* duplicates a string buffer */
static inline StrBuffer* testr_dup2 (StrBuffer *s2, StrBuffer *s1)
{
    if (!s1 || !s2)
        return NULL;

    testr_alloc (s2, s1->txtlen, s1->blocksize);
    testr_setstr (s2, (const char *)s1->string, s1->txtlen);
    return s2;
}


/* FIXME */
int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);

#define CHLENPREV(pStr, pIns) \
        ( GetLastMCharLen (GetWindowFont(hWnd), (const char *)pStr, (const char *)pIns - (const char *)pStr) )

#define CHLENNEXT(pIns, len) \
        ( GetFirstMCharLen (GetWindowFont(hWnd), (const char *)pIns, len) )


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* _MG_TEXT_H */

