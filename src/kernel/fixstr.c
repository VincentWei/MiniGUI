/* 
** $Id: fixstr.c 11102 2008-10-23 01:58:25Z tangjianbin $
**
** fixstr.c: the Fixed String module for MiniGUI.
** 
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All right resered by Feynman Software.
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"

static struct FIXSTR {
    BYTE bitmap[LEN_BITMAP];
    int offset[NR_HEAP];
    char* heap[NR_HEAP + 1];
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
} FixStrHeap;

/****************** Module functions *****************************************/
BOOL mg_InitFixStr (void)
{
    int i, j, offset;
    BYTE* bitmap;
    
    // allocate memory.
    if (!(FixStrHeap.heap [0] = malloc (MAX_LEN_FIXSTR * 8 * NR_HEAP))) return FALSE;

    for (i = 1; i <= NR_HEAP; i++) {
        FixStrHeap.heap[i] = FixStrHeap.heap[0] + MAX_LEN_FIXSTR * 8 * i;
    }

    // reset bitmap
    bitmap = FixStrHeap.bitmap;
    offset = 0;
    for (i = 0; i < NR_HEAP; i++) {
        for (j = 0; j < (1<<i); j ++)
            bitmap[j] |= 0xFF;

        bitmap += 1<<i;
        
        FixStrHeap.offset[i] = offset;
        
        offset += 1<<i;
    }
    
#ifdef _MGRM_THREADS
    pthread_mutex_init (&FixStrHeap.lock, NULL);
#endif
    return TRUE;
}

void mg_TerminateFixStr (void)
{
#ifdef _MGRM_THREADS
    pthread_mutex_destroy (&FixStrHeap.lock);
#endif
    free (FixStrHeap.heap[0]);
}

static char zero_string [] = {'\0'};

char* GUIAPI FixStrAlloc (int len)
{
    UINT ulen = (UINT)len;
    int i, j, btlen, bufflen;
    char* heap;
    BYTE* bitmap;

    if (len < 0)
        return NULL;
        
    if (len == 0)
        return zero_string;
        
    if (len >= MAX_LEN_FIXSTR)
        return (char*)malloc (len + 1);
    
    // determine which heap will use.
    i = 0;
    while (ulen) {
        ulen = ulen >> 1;
        i++;
    }

    // if 2 > len >= 1, then i = 1.
    // if 4 > len >= 2, then i = 2.
    // if 8 > len >= 4, then i = 3;
    // ...
    // if 512 > len >= 256, then i = 9;
    // ...
    // if 2K > len >= 1K, then i = 11;
    if (i == 1) i = 2;
    bufflen = 1 << i;
    
    i = NR_HEAP + 1 - i;
    // i is the heap index;
    // if i == 7; then bufflen = 4
    // if i == 6; then bufflen = 8
    // ..
    // if i == 0; then bufflen = 512

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&FixStrHeap.lock);
#endif

    heap = FixStrHeap.heap[i];
    bitmap = FixStrHeap.bitmap + FixStrHeap.offset[i];
    btlen = 1 << i;
    
    for (i = 0; i < btlen; i++) {
        for(j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j)) {
                *bitmap &= (~(0x80 >> j));
#ifdef _MGRM_THREADS
                pthread_mutex_unlock (&FixStrHeap.lock);
#endif
#if 0
                printf ("FixStrAlloc, len: %d, heap: %p.\n", len, heap);
#endif
                return heap;
            }

            heap += bufflen;
        }
        
        bitmap++;
    }

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&FixStrHeap.lock);
#endif
    return (char*)malloc (len + 1);
}

void GUIAPI FreeFixStr (char* str)
{
    char* heap;
    BYTE* bitmap;
    int i;
    int bufflen;
    int stroff;
    
    if (str [0] == '\0')
        return;
        
    if (str >= FixStrHeap.heap [NR_HEAP] || str < FixStrHeap.heap [0]) {
        free (str);
        return;
    }

    for (i = 1; i <= NR_HEAP; i++) {
        if (str < FixStrHeap.heap[i])
            break;
    }
    i--;
    // i is the heap index;

    // if i == 7; then bufflen = 4
    // if i == 6; then bufflen = 8
    // ..
    // if i == 0; then bufflen = 512
    bufflen = 1 << (NR_HEAP + 1 - i);

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&FixStrHeap.lock);
#endif
    heap = FixStrHeap.heap[i];
    bitmap = FixStrHeap.bitmap + FixStrHeap.offset[i];

    // locate the bitmap and reset the bit.
    stroff = 0;
    while (str != heap) {
        heap += bufflen;
        stroff ++;
    }

#if 0
    printf ("FreeFixStr, len: %d, str: %p: heap: %p.\n", strlen (str), str, heap);
#endif

    bitmap = bitmap + (stroff>>3);
    *bitmap |= (0x80 >> (stroff%8));

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&FixStrHeap.lock);
#endif
}

char* GUIAPI FixStrDup (const char* str)
{
    char* buff;

    if (str == NULL)
        return NULL;

    buff = FixStrAlloc (strlen (str));

    if (buff) {
        strcpy (buff, str);
    }

    return buff;
}

