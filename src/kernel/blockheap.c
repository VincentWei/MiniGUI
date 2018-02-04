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
** blockheap.c: The heap of block data.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/01/10
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "blockheap.h"

void InitBlockDataHeap (PBLOCKHEAP heap, size_t bd_size, size_t heap_size)
{
#ifdef _MGRM_THREADS
    pthread_mutex_init (&heap->lock, NULL);
#endif

    heap->heap = NULL;
    heap->bd_size = bd_size + sizeof (DWORD);
    heap->heap_size = heap_size;
}

void* BlockDataAlloc (PBLOCKHEAP heap)
{
    int i;
    char* block_data = NULL;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&heap->lock);
#endif

    if (heap->heap == NULL) {
        if (!(heap->heap = calloc (heap->heap_size, heap->bd_size)))
            goto ret;
        heap->free = 0;
    }

    block_data = (char*) heap->heap + heap->bd_size*heap->free;
    for (i = heap->free; i < heap->heap_size; i++) {

        if (*((DWORD*)block_data) == BDS_FREE) {

            heap->free = i + 1;
            *((DWORD*)block_data) = BDS_USED;

#if 0
            fprintf (stderr, "heap: %p, Allocated: %d, free: %d.\n", heap, i, heap->free);
            fprintf (stderr, "Heap: (bd_size: %d, heap_size: %d, heap: %p).\n", 
                    heap->bd_size, heap->heap_size, heap->heap);
#endif
            goto ret;
        }

        block_data += heap->bd_size;
    }

    if (!(block_data = calloc (1, heap->bd_size)))
        goto ret;

    *((DWORD*)block_data) = BDS_SPECIAL;

ret:

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&heap->lock);
#endif

    if (block_data)
        return block_data + sizeof (DWORD);
    return NULL;
}

void BlockDataFree (PBLOCKHEAP heap, void* data)
{
    int i;
    char* block_data;

#ifdef _MGRM_THREADS
    pthread_mutex_lock (&heap->lock);
#endif

    block_data = (char*) data - sizeof (DWORD);
    if (*((DWORD*)block_data) == BDS_SPECIAL)
        free (block_data);
    else if (*((DWORD*)block_data) == BDS_USED) {
        *((DWORD*)block_data) = BDS_FREE;

        i = (block_data - (char*)heap->heap)/heap->bd_size;
        if (heap->free > i)
            heap->free = i;
        
#if 0
        fprintf (stderr, "Heap: %p: Freed: %d, free: %d.\n", heap, i, heap->free);
        fprintf (stderr, "Heap: (bd_size: %d, heap_size: %d, heap: %p).\n", 
                    heap->bd_size, heap->heap_size, heap->heap);
#endif
    }

#ifdef _MGRM_THREADS
    pthread_mutex_unlock (&heap->lock);
#endif
}

void DestroyBlockDataHeap (PBLOCKHEAP heap)
{
#if 0
    fprintf (stderr, "Heap: (bd_size: %d, heap_size: %d, heap: %p).\n", 
                    heap->bd_size, heap->heap_size, heap->heap);
#endif
#ifdef _MGRM_THREADS
    pthread_mutex_destroy (&heap->lock);
#endif
    free (heap->heap);
}

