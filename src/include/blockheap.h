/*
** $Id: blockheap.h 8944 2007-12-29 08:29:16Z xwyan $
**
** blockheap.h: the head file of heap for block data.
** 
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Create date: 2001/01/10
*/

#ifndef GUI_BLOCKHEAP_H
    #define GUI_BLOCKHEAP_H

#define BDS_FREE        0x0000
#define BDS_SPECIAL     0x0001
#define BDS_USED        0x0002

#ifndef _MGUI_GDI_H /* included in include/gdi.h */
typedef struct tagBLOCKHEAP
{
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
    size_t          bd_size;
    size_t          heap_size;
    int             free;
    void*           heap;
} BLOCKHEAP;
typedef BLOCKHEAP* PBLOCKHEAP;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Function definitions */

void InitBlockDataHeap (PBLOCKHEAP heap, size_t bd_size, size_t heap_size);
void* BlockDataAlloc (PBLOCKHEAP heap);
void BlockDataFree (PBLOCKHEAP heap, void* data);
void DestroyBlockDataHeap (PBLOCKHEAP heap);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_GDI_H */

#endif  /* GUI_BLOCKHEAP_H */


