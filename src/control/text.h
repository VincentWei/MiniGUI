/*
** $Id: text.h 8944 2007-12-29 08:29:16Z xwyan $ 
**
** text.h: header file of text operations.
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
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

