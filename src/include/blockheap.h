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
** blockheap.h: the head file of heap for block data.
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


