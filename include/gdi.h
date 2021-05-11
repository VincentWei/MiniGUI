///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/**
 * \file gdi.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 *
 * \brief This file includes graphics device interfaces (GDI) of MiniGUI.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: gdi.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_GDI_H
    #define _MGUI_GDI_H

#include "common.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "endianrw.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup global_vars Global variables
     * @{
     */

    /**
     * \defgroup color_vars System colors and pixel values.
     * @{
     */

/**
 * \var gal_pixel SysPixelIndex []
 * \brief The pre-defined system pixel values.
 *
 * MiniGUI defines some system pixel values when initializing
 * the graphics sub-system. You can access the arrary to get the
 * system pixel values, or just use the following macros:
 *
 *  - PIXEL_black\n
 *    black
 *  - PIXEL_darkred\n
 *    dark red
 *  - PIXEL_darkgreen\n
 *    dark green
 *  - PIXEL_darkyellow\n
 *    dark yellow
 *  - PIXEL_darkblue\n
 *    dark blue
 *  - PIXEL_darkmagenta\n
 *    dark magenta
 *  - PIXEL_darkcyan\n
 *    dark cyan
 *  - PIXEL_lightgray\n
 *    light gray
 *  - PIXEL_darkgray\n
 *    dark gray
 *  - PIXEL_red\n
 *    red
 *  - PIXEL_green\n
 *    green
 *  - PIXEL_yellow\n
 *    yellow
 *  - PIXEL_blue\n
 *    blue
 *  - PIXEL_magenta\n
 *    magenta
 *  - PIXEL_cyan\n
 *    cyan
 *  - PIXEL_lightwhite\n
 *    light white
 *
 * \note These pixel values are complient to pixel format of \a HDC_SCREEN.
 *  Since 5.0.0, you should use the value returned by \a MakeRGBA macro
 *  for the background color of a window.
 */
extern MG_EXPORT gal_pixel SysPixelIndex [];

/**
 * \var RGB SysPixelColor []
 * \brief The pre-defined system RGB colors.
 *
 * The elements in this array are system colors in RGBA quadruple.
 */
extern const MG_EXPORT RGB SysPixelColor [];

/** Indexes for system standard colors. */
enum {
    /** Index for transparent color. */
    IDX_COLOR_transparent = 0,
    /** Index for dark blue color. */
    IDX_COLOR_darkblue,
    /** Index for dark green color. */
    IDX_COLOR_darkgreen,
    /** Index for dark cyan color. */
    IDX_COLOR_darkcyan,
    /** Index for dark red color. */
    IDX_COLOR_darkred,
    /** Index for draw magenta color. */
    IDX_COLOR_darkmagenta,
    /** Index for dark yellow color. */
    IDX_COLOR_darkyellow,
    /** Index for dark gray color. */
    IDX_COLOR_darkgray,
    /** Index for light gray color. */
    IDX_COLOR_lightgray,
    /** Index for blue color. */
    IDX_COLOR_blue,
    /** Index for green color. */
    IDX_COLOR_green,
    /** Index for cyan color. */
    IDX_COLOR_cyan,
    /** Index for red color. */
    IDX_COLOR_red,
    /** Index for magenta color. */
    IDX_COLOR_magenta,
    /** Index for yellow color. */
    IDX_COLOR_yellow,
    /** Index for light white color. */
    IDX_COLOR_lightwhite,
    /** Index for black color. */
    IDX_COLOR_black,
};

/**
 * \def PIXEL_invalid
 * Compatiblity definition; deprecated.
 * \sa SysPixelIndex
 */
#define PIXEL_invalid       0

/**
 * \def PIXEL_transparent
 * \brief Transparent
 * \sa SysPixelIndex
 */
#define PIXEL_transparent   SysPixelIndex[0]

/**
 * \def PIXEL_darkblue
 * \brief Dark blue.
 * \sa SysPixelIndex
 */
#define PIXEL_darkblue      SysPixelIndex[1]

/**
 * \def PIXEL_darkgreen
 * \brief Dark green.
 * \sa SysPixelIndex
 */
#define PIXEL_darkgreen     SysPixelIndex[2]

/**
 * \def PIXEL_darkcyan
 * \brief Dark cyan.
 * \sa SysPixelIndex
 */
#define PIXEL_darkcyan      SysPixelIndex[3]

/**
 * \def PIXEL_darkred
 * \brief Dark red.
 * \sa SysPixelIndex
 */
#define PIXEL_darkred       SysPixelIndex[4]

/**
 * \def PIXEL_darkmagenta
 * \brief Dark magenta.
 * \sa SysPixelIndex
 */
#define PIXEL_darkmagenta   SysPixelIndex[5]

/**
 * \def PIXEL_darkyellow
 * \brief Dark yellow.
 * \sa SysPixelIndex
 */
#define PIXEL_darkyellow    SysPixelIndex[6]

/**
 * \def PIXEL_darkgray
 * \brief Dark gray.
 * \sa SysPixelIndex
 */
#define PIXEL_darkgray      SysPixelIndex[7]

/**
 * \def PIXEL_lightgray
 * \brief Light gray.
 * \sa SysPixelIndex
 */
#define PIXEL_lightgray     SysPixelIndex[8]

/**
 * \def PIXEL_blue
 * \brief Blue.
 * \sa SysPixelIndex
 */
#define PIXEL_blue          SysPixelIndex[9]

/**
 * \def PIXEL_green
 * \brief Green.
 * \sa SysPixelIndex
 */
#define PIXEL_green         SysPixelIndex[10]

/**
 * \def PIXEL_cyan
 * \brief Cyan.
 * \sa SysPixelIndex
 */
#define PIXEL_cyan          SysPixelIndex[11]

/**
 * \def PIXEL_red
 * \brief Red.
 * \sa SysPixelIndex
 */
#define PIXEL_red           SysPixelIndex[12]

/**
 * \def PIXEL_magenta
 * \brief Magenta.
 * \sa SysPixelIndex
 */
#define PIXEL_magenta       SysPixelIndex[13]

/**
 * \def PIXEL_yellow
 * \brief Yellow.
 * \sa SysPixelIndex
 */
#define PIXEL_yellow        SysPixelIndex[14]

/**
 * \def PIXEL_lightwhite
 * \brief Light white.
 * \sa SysPixelIndex
 */
#define PIXEL_lightwhite    SysPixelIndex[15]

/**
 * \def PIXEL_black
 * \brief Black.
 * \sa SysPixelIndex
 */
#define PIXEL_black         SysPixelIndex[16]

/**
 * \def COLOR_invalid
 * Compatiblity definition; deprecated.
 */
#define COLOR_invalid       PIXEL_invalid

/**
 * \def COLOR_transparent
 * Same as PIXEL_transparent; deprecated.
 */
#define COLOR_transparent   PIXEL_transparent

/**
 * \def COLOR_darkred
 * Same as PIXEL_darkred; deprecated.
 */
#define COLOR_darkred       PIXEL_darkred

/**
 * \def COLOR_darkgreen
 * Same as PIXEL_darkgreen; deprecated.
 */
#define COLOR_darkgreen     PIXEL_darkgreen

/**
 * \def COLOR_darkyellow
 * Same as PIXEL_darkyellow; deprecated.
 */
#define COLOR_darkyellow    PIXEL_darkyellow

/**
 * \def COLOR_darkblue
 * Same as PIXEL_darkblue; deprecated.
 */
#define COLOR_darkblue      PIXEL_darkblue

/**
 * \def COLOR_darkmagenta
 * Same as PIXEL_darkmagenta; deprecated.
 */
#define COLOR_darkmagenta   PIXEL_darkmagenta

/**
 * \def COLOR_darkcyan
 * Same as PIXEL_darkcyan; deprecated.
 */
#define COLOR_darkcyan      PIXEL_darkcyan

/**
 * \def COLOR_lightgray
 * Same as PIXEL_lightgray; deprecated.
 */
#define COLOR_lightgray     PIXEL_lightgray

/**
 * \def COLOR_darkgray
 * Same as PIXEL_darkgray; deprecated.
 */
#define COLOR_darkgray      PIXEL_darkgray

/**
 * \def COLOR_red
 * Same as PIXEL_red; deprecated.
 */
#define COLOR_red           PIXEL_red

/**
 * \def COLOR_green
 * Same as PIXEL_green; deprecated.
 */
#define COLOR_green         PIXEL_green

/**
 * \def COLOR_yellow
 * Same as PIXEL_yellow; deprecated.
 */
#define COLOR_yellow        PIXEL_yellow

/**
 * \def COLOR_blue
 * Same as PIXEL_blue; deprecated.
 */
#define COLOR_blue          PIXEL_blue

/**
 * \def COLOR_magenta
 * Same as PIXEL_magenta; deprecated.
 */
#define COLOR_magenta       PIXEL_magenta

/**
 * \def COLOR_cyan
 * Same as PIXEL_cyan; deprecated.
 */
#define COLOR_cyan          PIXEL_cyan

/**
 * \def COLOR_lightwhite
 * Same as PIXEL_lightwhite; deprecated.
 */
#define COLOR_lightwhite    PIXEL_lightwhite

/**
 * \def COLOR_black
 * Same as \a PIXEL_black; deprecated.
 */
#define COLOR_black         PIXEL_black

/**
 * \def RGBA_transparent
 * The RGBA quadruple for transparent color.
 */
#define RGBA_transparent                                \
    (MakeRGBA(SysPixelColor[0].r, SysPixelColor[0].g,   \
              SysPixelColor[0].b, SysPixelColor[0].a))

/**
 * \def RGBA_darkblue
 * The RGBA quadruple for dark blue color.
 */
#define RGBA_darkblue                                   \
    (MakeRGBA(SysPixelColor[1].r, SysPixelColor[1].g,   \
              SysPixelColor[1].b, SysPixelColor[1].a))

/**
 * \def RGBA_darkgreen
 * The RGBA quadruple for dark green color.
 */
#define RGBA_darkgreen                                  \
    (MakeRGBA(SysPixelColor[2].r, SysPixelColor[2].g,   \
              SysPixelColor[2].b, SysPixelColor[2].a))

/**
 * \def RGBA_darkcyan
 * The RGBA quadruple for dark cyan color.
 */
#define RGBA_darkcyan                                   \
    (MakeRGBA(SysPixelColor[3].r, SysPixelColor[3].g,   \
              SysPixelColor[3].b, SysPixelColor[3].a))

/**
 * \def RGBA_darkred
 * The RGBA quadruple for dark red color.
 */
#define RGBA_darkred                                    \
    (MakeRGBA(SysPixelColor[4].r, SysPixelColor[4].g,   \
              SysPixelColor[4].b, SysPixelColor[4].a))

/**
 * \def RGBA_darkmagenta
 * The RGBA quadruple for dark magenta color.
 */
#define RGBA_darkmagenta                                \
    (MakeRGBA(SysPixelColor[5].r, SysPixelColor[5].g,   \
              SysPixelColor[5].b, SysPixelColor[5].a))

/**
 * \def RGBA_darkyellow
 * The RGBA quadruple for dark yellow color.
 */
#define RGBA_darkyellow                                 \
    (MakeRGBA(SysPixelColor[6].r, SysPixelColor[6].g,   \
              SysPixelColor[6].b, SysPixelColor[6].a))

/**
 * \def RGBA_darkgray
 * The RGBA quadruple for dark gray color.
 */
#define RGBA_darkgray                                   \
    (MakeRGBA(SysPixelColor[7].r, SysPixelColor[7].g,   \
              SysPixelColor[7].b, SysPixelColor[7].a))

/**
 * \def RGBA_lightgray
 * The RGBA quadruple for light gray color.
 */
#define RGBA_lightgray                                  \
    (MakeRGBA(SysPixelColor[8].r, SysPixelColor[8].g,   \
              SysPixelColor[8].b, SysPixelColor[8].a))

/**
 * \def RGBA_blue
 * The RGBA quadruple for blue color.
 */
#define RGBA_blue                                       \
    (MakeRGBA(SysPixelColor[9].r, SysPixelColor[9].g,   \
              SysPixelColor[9].b, SysPixelColor[9].a))

/**
 * \def RGBA_green
 * The RGBA quadruple for green color.
 */
#define RGBA_green                                      \
    (MakeRGBA(SysPixelColor[10].r, SysPixelColor[10].g, \
              SysPixelColor[10].b, SysPixelColor[10].a))

/**
 * \def RGBA_cyan
 * The RGBA quadruple for cyan color.
 */
#define RGBA_cyan                                       \
    (MakeRGBA(SysPixelColor[11].r, SysPixelColor[11].g, \
              SysPixelColor[11].b, SysPixelColor[11].a))

/**
 * \def RGBA_red
 * The RGBA quadruple for red color.
 */
#define RGBA_red                                        \
    (MakeRGBA(SysPixelColor[12].r, SysPixelColor[12].g, \
              SysPixelColor[12].b, SysPixelColor[12].a))

/**
 * \def RGBA_magenta
 * The RGBA quadruple for magenta color.
 */
#define RGBA_magenta                                    \
    (MakeRGBA(SysPixelColor[13].r, SysPixelColor[13].g, \
              SysPixelColor[13].b, SysPixelColor[13].a))

/**
 * \def RGBA_yellow
 * The RGBA quadruple for yellow color.
 */
#define RGBA_yellow                                     \
    (MakeRGBA(SysPixelColor[14].r, SysPixelColor[14].g, \
              SysPixelColor[14].b, SysPixelColor[14].a))

/**
 * \def RGBA_lightwhite
 * The RGBA quadruple for light white color.
 */
#define RGBA_lightwhite                                 \
    (MakeRGBA(SysPixelColor[15].r, SysPixelColor[15].g, \
              SysPixelColor[15].b, SysPixelColor[15].a))

/**
 * \def RGBA_black
 * The RGBA quadruple for black color.
 */
#define RGBA_black                                      \
    (MakeRGBA(SysPixelColor[16].r, SysPixelColor[16].g, \
              SysPixelColor[16].b, SysPixelColor[16].a))

/**
 * \def SysColorIndex
 * \sa SysPixelColor
 */
#define SysColorIndex       SysPixelColor

    /** @} end of color_vars */

    /** @} end of global_vars */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup block_heap_fns Block data heap operations
     *
     * MiniGUI maintains some private block data heaps to allocate
     * data blocks which have fixed size, such as the clipping rectangles
     * in a region. By using the private heap, we can avoiding calling
     * \a malloc and \a free function frequently, so will have a slight
     * performance enhancement.
     *
     * @{
     */

/**
 * MiniGUI's private block data heap.
 *
 * \sa InitBlockDataHeap, DestroyBlockDataHeap
 */
typedef struct _BLOCKHEAP {
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
    /** Size of one block element in bytes. */
    size_t          sz_block;
    /** Size of the heap in blocks. */
    size_t          sz_heap;
    /** The number of blocks extra allocated. */
    size_t          nr_alloc;
    /** The size of usage bitmap in bytes. */
    size_t          sz_usage_bmp;

    /** The pointer to the pre-allocated heap. */
    unsigned char*  heap;
    /** The pointer to the usage bitmap. */
    unsigned char*  usage_bmp;
} BLOCKHEAP;

/**
 * \var typedef BLOCKHEAP* PBLOCKHEAP
 * \brief Data type of the pointer to a BLOCKHEAP.
 *
 * \sa BLOCKHEAP
 */
typedef BLOCKHEAP* PBLOCKHEAP;

/**
 * \fn BOOL InitBlockDataHeap (PBLOCKHEAP heap,
                size_t bd_size, size_t heap_size)
 * \brief Initializes a private block data heap.
 *
 * This function initializes a block data heap pointed to by \a heap.
 * It will allocate the buffer used by the heap from system heap by using
 * \a malloc(3). Initially, the heap has \a heap_size blocks free, and each
 * is \a bd_size bytes long.
 *
 * \param heap The pointer to the heap structure.
 * \param bd_size The size of one block in bytes.
 * \param heap_size The size of the heap in blocks.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa BLOCKHEAP
 */
MG_EXPORT BOOL InitBlockDataHeap (PBLOCKHEAP heap,
                size_t bd_size, size_t heap_size);

/**
 * \fn void* BlockDataAlloc (PBLOCKHEAP heap)
 * \brief Allocates a data block from private heap.
 *
 * This function allocates a data block from an initialized
 * block data heap. The allocated block will have the size of \a heap->bd_size.
 * If there is no free block in the heap, this function will try to allocate
 * the block from the system heap by using \a malloc(3) function.
 *
 * \param heap The pointer to the initialized heap.
 * \return Pointer to the allocated data block, NULL on error.
 *
 * \sa InitBlockDataHeap, BlockDataFree
 */

MG_EXPORT void* BlockDataAlloc (PBLOCKHEAP heap);

/**
 * \fn void BlockDataFree (PBLOCKHEAP heap, void* data)
 * \brief Frees an allocated data block.
 *
 * This function frees the specified data block pointed to by \a data to
 * the heap \a heap. If the block was allocated by using \a malloc function,
 * this function will free the element by using \a free(3) function.
 *
 * \param heap The pointer to the heap.
 * \param data The pointer to the element to be freed.
 *
 * \sa InitBlockDataHeap, BlockDataAlloc
 */
MG_EXPORT void BlockDataFree (PBLOCKHEAP heap, void* data);

/**
 * \fn void DestroyBlockDataHeap (PBLOCKHEAP heap)
 * \brief Destroys a private block data heap.
 *
 * \param heap The pointer to the heap to be destroied.
 *
 * \sa InitBlockDataHeap, BLOCKHEAP
 */
MG_EXPORT void DestroyBlockDataHeap (PBLOCKHEAP heap);

    /** @} end of block_heap_fns */

    /** @} end of global_fns */

    /**
     * \defgroup gdi_fns GDI functions
     * @{
     */

struct _BITMAP;

/**
 * \var typedef struct _BITMAP BITMAP
 * \sa _BITMAP
 */
typedef struct _BITMAP BITMAP;

/**
 * \var typedef BITMAP* PBITMAP
 * \brief Data type of pointer to a BITMAP.
 * \sa BITMAP _BITMAP
 */
typedef BITMAP* PBITMAP;

struct _MYBITMAP;

/**
 * \var typedef struct _MYBITMAP MYBITMAP
 * \sa _MYBITMAP
 */
typedef struct _MYBITMAP MYBITMAP;

/**
 * \var typedef struct _MYBITMAP* PMYBITMAP
 * \brief Data type of pointer to a struct _MYBITMAP
 * \sa MYBITMAP _MYBITMAP
 */
typedef struct _MYBITMAP* PMYBITMAP;


   /**
    * \defgroup region_fns Region operations
    *
    * A Region is simply an area, as the name implies, and is implemented as
    * a "y-x-banded" array of rectangles. To explain: Each Region is made up
    * of a certain number of rectangles sorted by y coordinate first,
    * and then by x coordinate.
    *
    * Furthermore, the rectangles are banded such that every rectangle with a
    * given upper-left y coordinate (y1) will have the same lower-right y
    * coordinate (y2) and vice versa. If a rectangle has scanlines in a band, it
    * will span the entire vertical distance of the band. This means that some
    * areas that could be merged into a taller rectangle will be represented as
    * several shorter rectangles to account for shorter rectangles to its left
    * or right but within its "vertical scope".
    *
    * An added constraint on the rectangles is that they must cover as much
    * horizontal area as possible. E.g. no two rectangles in a band are allowed
    * to touch.
    *
    * Whenever possible, bands will be merged together to cover a greater
    * vertical distance (and thus reduce the number of rectangles). Two bands
    * can be merged only if the bottom of one touches the top of the other and
    * they have rectangles in the same places (of the same width, of course).
    * This maintains the y-x-banding that's so nice to have...
    *
    * Example:
    *
    * \include region.c
    *
    * @{
    */

/**
 * Clipping rectangle structure.
 */
typedef struct _CLIPRECT
{
    /**
     * The clipping rectangle itself.
     */
    RECT rc;
    /**
     * The next clipping rectangle.
     */
    struct _CLIPRECT* next;
    /**
     * The previous clipping rectangle.
     */
    struct _CLIPRECT* prev;
} CLIPRECT;
typedef CLIPRECT* PCLIPRECT;

/* Clipping Region */
#define NULLREGION      0x00
#define SIMPLEREGION    0x01
#define COMPLEXREGION   0x02

/**
 * Clipping region structure, alos used for general regions.
 */
typedef struct _CLIPRGN
{
   /**
    * Type of the region, can be one of the following value:
    *   - NULLREGION\n
    *     A null region.
    *   - SIMPLEREGION\n
    *     A simple region.
    *   - COMPLEXREGION\n
    *     A complex region.
    */
    BYTE            type;
   /**
    * Reserved for alignment.
    */
    BYTE            reserved[3];
   /**
    * The bounding rect of the region.
    */
    RECT            rcBound;
   /**
    * Head of the clipping rectangle list.
    */
    PCLIPRECT       head;
   /**
    * Tail of the clipping rectangle list.
    */
    PCLIPRECT       tail;
   /**
    * The private block data heap used to allocate clipping rectangles.
    * \sa BLOCKHEAP
    */
    PBLOCKHEAP      heap;
} CLIPRGN;

/**
 * \var typedef CLIPRGN* PCLIPRGN
 * \brief Data type of the pointer to a CLIPRGN.
 *
 * \sa CLIPRGN
 */
typedef CLIPRGN* PCLIPRGN;

/**
 * \def InitFreeClipRectList(heap, size)
 * \brief Initializes the private block data heap used to allocate
 * clipping rectangles.
 *
 * \param heap The pointer to a BLOCKHEAP structure.
 * \param size The size of the heap.
 *
 * \note This macro is defined to call \a InitBlockDataHeap function
 *       with \a bd_size set to \a sizeof(CLIPRECT).
 *
 * \sa InitBlockDataHeap
 */
#define InitFreeClipRectList(heap, size)    \
                InitBlockDataHeap (heap, sizeof (CLIPRECT), size)

/**
 * \def ClipRectAlloc(heap)
 * \brief Allocates a clipping rectangles from the private block data heap.
 *
 * \param heap The pointer to the initialized BLOCKHEAP structure.
 *
 * \note This macro is defined to call \a BlockDataAlloc function.
 *
 * \sa BlockDataAlloc
 */
#define ClipRectAlloc(heap)     BlockDataAlloc (heap)

/**
 * \def FreeClipRect(heap, cr)
 * \brief Frees a clipping rectangle which is allocated from the private
 *        block data heap.
 *
 * \param heap The pointer to the initialized BLOCKHEAP structure.
 * \param cr The pointer to the clipping rectangle to be freed.
 *
 * \note This macro is defined to call \a BlockDataFree function.
 *
 * \sa BlockDataFree
 */
#define FreeClipRect(heap, cr)  BlockDataFree (heap, cr);

/**
 * \def DestroyFreeClipRectList(heap)
 * \brief Destroys the private block data heap used to allocate clipping
 *        rectangles.
 *
 * \param heap The pointer to the BLOCKHEAP structure.
 *
 * \note This macro is defined to call \a DestroyBlockDataHeap function.
 *
 * \sa DestroyBlockDataHeap
 */
#define DestroyFreeClipRectList(heap)   DestroyBlockDataHeap (heap);

/**
 * \fn void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP pFreeList)
 * \brief Initializes a clipping region.
 *
 * Before intializing a clipping region, you should initialize a private
 * block data heap first. The region operations, such as \a UnionRegion
 * function, will allocate/free the clipping rectangles from/to the heap.
 * This function will set the \a heap field of \a pRgn to be \a pFreeList,
 * and empty the region.
 *
 * \param pRgn The pointer to the CLIPRGN structure to be initialized.
 * \param pFreeList The pointer to the initialized private block data heap.
 *
 * \sa InitFreeClipRectList, EmptyClipRgn.
 *
 * Example:
 *
 * \include initcliprgn.c
 */

MG_EXPORT void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP pFreeList);

/**
 * \fn void GUIAPI EmptyClipRgn (PCLIPRGN pRgn)
 * \brief Empties a clipping region.
 *
 * This function empties a clipping region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 *
 * \sa InitClipRgn
 */
MG_EXPORT void GUIAPI EmptyClipRgn (PCLIPRGN pRgn);

/**
 * \fn PCLIPRGN GUIAPI CreateClipRgn (void)
 * \brief Creates a clipping region.
 *
 * \return The pointer to the clip region.
 *
 * \sa InitClipRgn, EmptyClipRgn, DestroyClipRgn.
 */

MG_EXPORT PCLIPRGN GUIAPI CreateClipRgn (void);

/**
 * \fn void GUIAPI DestroyClipRgn (PCLIPRGN pRgn)
 * \brief Empties and destroys a clipping region.
 *
 * This function empties and destroys a clipping region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 *
 * \sa InitClipRgn, CreateClipRgn
 */
MG_EXPORT void GUIAPI DestroyClipRgn (PCLIPRGN pRgn);

/**
 * \fn BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn)
 * \brief Copies one region to another.
 *
 * This function copies the region pointed to by \a pSrcRgn to the region
 * pointed to by \a pDstRgn.
 *
 * \param pDstRgn The destination region.
 * \param pSrcRgn The source region.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function will empty the region \a pDstRgn first.
 *
 * \sa EmptyClipRgn, ClipRgnIntersect, UnionRegion, SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn);

/**
 * \fn BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn, \
                const CLIPRGN* pRgn1, const CLIPRGN* pRgn2)
 * \brief Intersects two region.
 *
 * This function gets the intersection of two regions pointed to by \a pRgn1
 * and \a pRgn2 respectively and puts the result to the region pointed to
 * by \a pRstRgn.
 *
 * \param pRstRgn The intersected result region.
 * \param pRgn1 The first region.
 * \param pRgn2 The second region.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note If \a pRgn1 does not intersected with \a pRgn2, the result region
 *       will be an emgty region.
 *
 * \sa EmptyClipRgn, ClipRgnCopy, UnionRegion, SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn,
                       const CLIPRGN* pRgn1, const CLIPRGN* pRgn2);

/**
 * \fn void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect)
 * \brief Get the bounding rectangle of a region.
 *
 * This function gets the bounding rect of the region pointed to by \a pRgn,
 * and returns the rect in the rect pointed to by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the result rect.
 *
 * \sa IsEmptyClipRgn
 */
MG_EXPORT void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect);

/**
 * \fn BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Set a region to contain only one rect.
 *
 * This function sets the region \a pRgn to contain only a rect pointed to
 * by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rect.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function will empty the region \a pRgn first.
 *
 * \sa EmptyClipRgn
 */
MG_EXPORT BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn)
 * \brief Determine whether a region is an empty region.
 *
 * This function determines whether the region pointed to by \a pRgn is
 * an empty region.
 *
 * \param pRgn The pointer to the region.
 *
 * \return TRUE for empty one, else for not empty region.
 *
 * \sa EmptyClipRgn
 */
MG_EXPORT BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn);

/**
 * \fn BOOL GUIAPI AddClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Unions one rectangle to a region.
 *
 * This function unions a rectangle to the region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rectangle.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IntersectClipRect, SubtractClipRect
 */
MG_EXPORT BOOL GUIAPI AddClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI IntersectClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Intersects a rectangle with a region.
 *
 * This function intersects the region pointed to by \a pRgn with a
 * rect pointed to by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rectangle.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa AddClipRect, SubtractClipRect
 */
MG_EXPORT BOOL GUIAPI IntersectClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI SubtractClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Subtracts a rectangle from a region.
 *
 * This function subtracts a rect pointed to by \a pRect from the region
 * pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rect.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa AddClipRect, IntersectClipRect
 */
MG_EXPORT BOOL GUIAPI SubtractClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y)
 * \brief Determine whether a point is in a region.
 *
 * This function determines whether a point \a (x,y) is in the region pointed
 * to by \a region.
 *
 * \param region The pointer to the region.
 * \param x x,y: The point.
 * \param y x,y: The point.
 *
 * \return TRUE for in the region, otherwise FALSE.
 *
 * \sa RectInRegion
 */
MG_EXPORT BOOL GUIAPI PtInRegion (const PCLIPRGN region, int x, int y);

/**
 * \fn BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect)
 * \brief Determine whether a rectangle is intersected with a region.
 *
 * This function determines whether the rect \a rect is intersected with
 * the region pointed to by \a region.
 *
 * \param region The pointer to the region.
 * \param rect The pointer to the rect.
 *
 * \return TRUE for in the region, otherwise FALSE.
 *
 * \sa PtInRegion
 */
MG_EXPORT BOOL GUIAPI RectInRegion (const PCLIPRGN region, const RECT* rect);

/**
 * \fn BOOL GUIAPI AreRegionsIntersected (const PCLIPRGN s1, const PCLIPRGN s2)
 * \brief Determine whether two regions are intersected.
 *
 * This function determines whether the region \a s1 and the region \a s2
 * are intersected.
 *
 * \param s1 The pointer to the first region.
 * \param s2 The pointer to the second region.
 *
 * \return TRUE if intersected, otherwise FALSE.
 *
 * \sa RectInRegion
 */
static inline
BOOL GUIAPI AreRegionsIntersected (const PCLIPRGN s1, const PCLIPRGN s2)
{
    PCLIPRECT crc = s1->head;
    while (crc) {
        if (RectInRegion (s2, &crc->rc))
            return TRUE;
        crc = crc->next;
    }

    return FALSE;
}

/**
 * \fn void GUIAPI OffsetRegionEx (PCLIPRGN region, const RECT *rcClient, \
                            const RECT *rcScroll, int x, int y)
 * \brief Offsets the region in the specified window's scroll area.
 *
 * This function offsets a given region pointed to by region in the specified
 * window's scroll area.
 *
 * \param region The pointer to the region.
 * \param rcClient The client area which the region belongs to.
 * \param rcScroll The rectangle of the area in which the region will be offset.
 * \param x x,y: Offsets on x and y coodinates.
 * \param y x,y: Offsets on x and y coodinates.
 *
 */
MG_EXPORT void GUIAPI OffsetRegionEx (PCLIPRGN region, const RECT *rcClient,
                            const RECT *rcScroll, int x, int y);

/**
 * \fn void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y)
 * \brief Offsets the region.
 *
 * This function offsets a given region pointed to by \a region.
 *
 * \param region The pointer to the region.
 * \param x x,y: Offsets on x and y coodinates.
 * \param y x,y: Offsets on x and y coodinates.
 *
 */
MG_EXPORT void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y);

/**
 * \fn BOOL GUIAPI UnionRegion (PCLIPRGN dst, \
                const CLIPRGN* src1, const CLIPRGN* src2)
 * \brief Unions two regions.
 *
 * This function unions two regions pointed to by \a src1 and \a src2
 * respectively and puts the result to the region pointed to by \a dst.
 *
 * \param dst The pointer to the result region.
 * \param src1 src1,src2: Two regions will be unioned.
 * \param src2 src1,src2: Two regions will be unioned.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI UnionRegion (PCLIPRGN dst,
                const CLIPRGN* src1, const CLIPRGN* src2);

/**
 * \fn BOOL GUIAPI SubtractRegion (CLIPRGN* rgnD, \
                const CLIPRGN* rgnM, const CLIPRGN* rgnS)
 * \brief Substrcts a region from another.
 *
 * This function subtracts \a rgnS from \a rgnM and leave the result in \a rgnD.
 *
 * \param rgnD The pointer to the difference region.
 * \param rgnM The pointer to the minuend region.
 * \param rgnS The pointer to the subtrahend region.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa UnionRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI SubtractRegion (CLIPRGN* rgnD,
                const CLIPRGN* rgnM, const CLIPRGN* rgnS);

/**
 * \fn BOOL GUIAPI XorRegion (CLIPRGN *dst, \
                const CLIPRGN *src1, const CLIPRGN *src2)
 * \brief Does the XOR operation between two regions.
 *
 * This function does the XOR operation between two regions pointed to by
 * \a src1 and \a src2 and puts the result to the region pointed to by \a dst.
 *
 * \param dst The pointer to the result region.
 * \param src1 src1,src2: Two regions will be xor'ed.
 * \param src2 src1,src2: Two regions will be xor'ed.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa UnionRegion, SubtractRegion
 */
MG_EXPORT BOOL GUIAPI XorRegion (CLIPRGN *dst,
                const CLIPRGN *src1, const CLIPRGN *src2);

/**
 * \def UnionRectWithRegion
 * \brief Is an alias of \a AddClipRect
 * \sa AddClipRect
 */
#define UnionRectWithRegion     AddClipRect

/**
 * \def CopyRegion
 * \brief Is an alias of \a ClipRgnCopy
 * \sa ClipRgnCopy
 */
#define CopyRegion              ClipRgnCopy

/**
 * \def IntersectRegion
 * \brief Is an alias of \a ClipRgnIntersect
 * \sa ClipRgnIntersect
 */
#define IntersectRegion         ClipRgnIntersect

/**
 * \fn BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r)
 * \brief Initializes a region to be an enclosed circle.
 *
 * \param dst The pointer to the region to be initialized.
 * \param x x,y: The center of the circle.
 * \param y x,y: The center of the circle.
 * \param r The radius of the circle.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa InitEllipseRegion, InitPolygonRegion
 */
MG_EXPORT BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r);

/**
 * \fn BOOL GUIAPI InitEllipseRegion (PCLIPRGN dst, \
                int x, int y, int rx, int ry)
 * \brief Initializes a region to be an enclosed ellipse.
 *
 * \param dst The pointer to the region to be initialized.
 * \param x x,y: The center of the ellipse.
 * \param y x,y: The center of the ellipse.
 * \param rx The x-radius of the ellipse.
 * \param ry The y-radius of the ellipse.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa InitCircleRegion, InitPolygonRegion
 */
MG_EXPORT BOOL GUIAPI InitEllipseRegion (PCLIPRGN dst,
                int x, int y, int rx, int ry);

/**
 * \fn BOOL GUIAPI InitPolygonRegion (PCLIPRGN dst, \
                const POINT* pts, int vertices)
 * \brief Initializes a region to be an enclosed polygon.
 *
 * \param dst The pointer to the region to be initialized.
 * \param pts The vertex array of the polygon.
 * \param vertices The number of the vertices.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa InitCircleRegion, InitEllipseRegion
 */
MG_EXPORT BOOL GUIAPI InitPolygonRegion (PCLIPRGN dst,
                const POINT* pts, int vertices);

    /** @} end of region_fns */

    /**
     * \defgroup dc_fns General DC operations
     *
     * DC means Device Context, just like Graphics Context (GC) of X Lib.
     * DC represents a rectangle area on the actual screen or a virtual screen
     * created in memory. We call the rectangle area as "surface" of the DC.
     *
     * You can call a GDI function and pass a DC to the function to
     * draw lines, circles, or text. Commonly, you should call a function
     * like \a GetClientDC or \a CreateCompatibleDC to get or create a
     * DC, then call GDI functions to draw objects, e.g. \a MoveTo and
     * \a LineTo. After finishing drawing, you should call \a ReleaseDC or
     * \a DeleteMemDC function to release or destroy the DC.
     *
     * MiniGUI reserved an global DC called \a HDC_SCREEN. You can
     * use this DC directly without getting/creating or releasing/destroying.
     *
     * For main windows or controls, MiniGUI will send a MSG_PAINT message to
     * the window when the whole or part of window area have been invalidated.
     * You should call \a BegainPaint function to get the DC, then repaint
     * the window, and call \a EndPaint function to release the DC at the last.
     *
     * Example:
     *
     * \include msg_paint.c
     *
     * @{
     */

/**
 * \def HDC_SCREEN
 * \brief Handle to the device context of the whole screen or the fake screen
 * when MiniGUI is running under MiniGUI-Processes with compositing schema.
 *
 * This DC is a special one. Under MiniGUI-Standalone runmode, MiniGUI-Threads
 * runmode, and MiniGUI-Processes runmode with shared frame buffer schema,
 * MiniGUI uses it to draw popup menus and other global objects. You can also
 * use this DC to draw lines or text on the screen directly, and there is no
 * need to get or release it.
 *
 * If you do not want to create any main window, but you want to draw on
 * the screen, you can use this DC. Note that MiniGUI does not do any
 * clipping operation for this DC, so drawing something on this DC may
 * make a mess of other windows.
 *
 * However, under MiniGUI-Processes with compositing schema, HDC_SCREEN
 * stands for a global shared surface for wallpaper pattern. This surface
 * is the ONLY surface that can be accessed by all processes (including
 * the server and all clients) under compositing schema.
 *
 * This surface will have the same pixel format as the real screen.
 * Therefore, one app can still use HDC_SCREEN to create a compatible
 * memory DC, load bitmaps, or draw something to the surface. However,
 * the content in the wallpaper surface may not be reflected to
 * the whole screen; the compositor decides how to display the contents
 * in it.
 *
 * On the other hand, you can configure MiniGUI to create a smaller
 * surface than the whole screen as the underlying surface of HDC_SCREEN,
 * and the compositor may use it as a pattern to tile the content
 * to the whole wallpaper. You can use the key
 * `compositing_schema.wallpaper_pattern_size` to specify the pattern size,
 * i.e., the size of HDC_SCREEN, in runtime configuration.
 *
 * Because of the change of HDC_SCREEN's connotation, you should avoid
 * to use \a GetGDCapability to determine the screen resolution. Instead,
 * you use the global variable/macro \a g_rcScr or \a the function
 * GetScreenRect();
 *
 * \sa HDC_SCREEN_SYS, GetScreenRect
 */
#define HDC_SCREEN          ((HDC)0)

/*
 * \def HDC_SCREEN_SYS
 * \brief This is a system screen DC created for internal use.
 *
 * Under compositing schema, this DC stands for the surface of the
 * real screen. The compositor running in the server will
 * use this DC to compositing the surfaces created and rendered by
 * the server and the clients to the screen. For clients, this DC
 * essentially is HDC_SCREEN.
 *
 * Under MiniGUI-Standalone runmode, MiniGUI-Threads
 * runmode, and MiniGUI-Processes runmode with shared frame buffer schema,
 * MiniGUI uses it to draw popup menus and other global objects.
 *
 * \sa HDC_SCREEN
 */
#define HDC_SCREEN_SYS      ((HDC)1)

/**
 * \def HDC_INVALID
 * \brief Indicates an invalid handle to device context.
 */
#define HDC_INVALID         ((HDC)-1)

#define GDCAP_COLORNUM      0
#define GDCAP_HPIXEL        1
#define GDCAP_VPIXEL        2
#define GDCAP_MAXX          3
#define GDCAP_MAXY          4
#define GDCAP_DEPTH         5
#define GDCAP_BPP           6
#define GDCAP_BITSPP        7
#define GDCAP_RMASK         8
#define GDCAP_GMASK         9
#define GDCAP_BMASK         10
#define GDCAP_AMASK         11
#define GDCAP_PITCH         12
#define GDCAP_DPI           13
    #define GDCAP_DPI_DEFAULT   96
    #define GDCAP_DPI_MINIMAL   36

/**
 * \fn Uint32 GUIAPI GetGDCapability (HDC hdc, int iItem)
 * \brief Returns a capability of a DC.
 *
 * This function returns the capability of the specified item \a iItem of
 * the DC \a hdc.
 *
 * \param hdc The handle to the DC.
 * \param iItem An integer presents the capablity, can be one of the
 *        following values:
 *
 *  - GDCAP_COLORNUM\n
 *    Tell \a GetGDCapability to return the colors number of the DC.
 *    Note the for a DC with 32-bit depth, the function will return
 *    0xFFFFFFFF, not 0x100000000.
 *  - GDCAP_HPIXEL\n
 *    Tell \a GetGCapability to return the horizontal resolution of the DC.
 *  - GDCAP_VPIXEL\n
 *    Tell \a GetGDCapability to return the vertical resolution of the DC.
 *  - GDCAP_MAXX\n
 *    Tell \a GetGDCapability to return the maximal visible x value of the DC.
 *  - GDCAP_MAXY\n
 *    Tell \a GetGDCapability to return the maximal visible y value of the DC.
 *  - GDCAP_DEPTH\n
 *    Tell \a GetGDCapability to return the color depth of the DC.
 *    The returned value can be 1, 4, 8, 15, 16, 24, or 32.
 *  - GDCAP_BITSPP\n
 *    Tell \a GetGDCapability to return the bits number for storing a pixle
 *    in the DC.
 *  - GDCAP_BPP\n
 *    Tell \a GetGDCapability to return the bytes number for storing a pixle
 *    in the DC.
 *  - GDCAP_RMASK\n
 *    Tell \a GetGDCapability to return the pixel red color mask for the DC.
 *  - GDCAP_GMASK\n
 *    Tell \a GetGDCapability to return the pixel green color mask for the DC.
 *  - GDCAP_BMASK\n
 *    Tell \a GetGDCapability to return the pixel blue color mask for the DC.
 *  - GDCAP_AMASK\n
 *    Tell \a GetGDCapability to return the pixel alpha color mask for the DC.
 *  - GDCAP_PITCH\n
 *    Tell \a GetGDCapability to return the pitch (the bytes of one scan line) of the DC.
 *  - GDCAP_DPI\n
 *    Tell \a GetGDCapability to return the DPI (the dots per inch) of the DC.
 *
 * \return The capbility.
 */
MG_EXPORT Uint32 GUIAPI GetGDCapability (HDC hdc, int iItem);

/**
 * \fn HDC GUIAPI GetDCEx (HWND hWnd, BOOL bClient)
 * \brief Get a window or client DC of a window.
 *
 * This function gets a window or a client DC of the specified \a hwnd, and
 * returns the handle to the DC. MiniGUI will try to return an unused DC from
 * the internal DC pool, rather than allocating a new one from the system heap.
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * \param hWnd The handle to the window.
 * \param bClient Whether to initialize as a window or a client DC;
 *  TRUE for client DC, FALSE for window DC.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetClientDC, ReleaseDC
 *
 * Since 5.0.0
 */
MG_EXPORT HDC GUIAPI GetDCEx (HWND hWnd, BOOL bClient);

/**
 * \fn HDC GUIAPI GetDC (HWND hWnd)
 * \brief Get a window DC of a window.
 *
 * This function gets a window DC of the specified \a hwnd, and returns
 * the handle to the DC. MiniGUI will try to return an unused DC from the
 * internal DC pool, rather than allocating a new one from the system heap.
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * \param hWnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetClientDC, ReleaseDC
 */
static inline HDC GUIAPI GetDC (HWND hWnd)
{
    return GetDCEx (hWnd, FALSE);
}

/**
 * \fn HDC GUIAPI GetClientDC (HWND hWnd)
 * \brief Get a client DC of a window.
 *
 * This function gets a client DC of the specified \a hwnd, and returns the
 * handle to the DC. MiniGUI will try to return an unused DC from the
 * internal DC pool, rather than allocating a new one from the system heap.
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * \param hWnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetDC, ReleaseDC
 */
static inline HDC GUIAPI GetClientDC (HWND hWnd)
{
    return GetDCEx (hWnd, TRUE);
}

/**
 * \fn HDC GUIAPI GetSubDC (HDC hdc, int off_x, int off_y, \
                int width, int height)
 * \brief This function gets a sub DC which is compliant to the specified
 *        client DC.
 *
 * This function creates a sub client DC, which is restricted in the specified
 * rectangle and compliant to the specified client DC \a hdc. Note that you
 * should release the returned dc by calling \a ReleaseDC.
 *
 * \param hdc The handle to the parent client DC.
 * \param off_x The x-coordinate of the sub DC in the parent DC (in device
 *              coordinate system).
 * \param off_y The y-coordinate of the sub DC in the parent DC (in device
 *              coordinate system).
 * \param width The expected width of the sub DC.
 * \param height The expected height of the sub DC.
 *
 * \return The handle to the new sub DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetDC, GetClientDC, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetSubDC (HDC hdc, int off_x, int off_y, int width, int height);

/**
 * \fn void GUIAPI ReleaseDC (HDC hdc)
 * \brief Releases a DC from DC pool.
 *
 * This function releases the DC returned by \a GetDC or \a GetClientDC.
 *
 * \param hdc The handle to the DC.
 *
 * \sa GetDC, GetClientDC, GetSubDC
 */
MG_EXPORT void GUIAPI ReleaseDC (HDC hdc);

/**
 * \fn HWND GUIAPI WindowFromDC (HDC hdc)
 * \brief Get the window handle from DC.
 *
 * This function returns the handle to a window according to the handle
 * to DC of the window.
 *
 * \param hdc The handle to the DC.
 *
 * \note Do not call this function on memory DC.
 *
 * \sa GetDC.
 */
MG_EXPORT HWND GUIAPI WindowFromDC (HDC hdc);

/**
 * \fn int GUIAPI SaveDC (HDC hdc)
 * \brief Saves the current state of a device context.
 *
 * This function saves the current state of the specified device context (DC)
 * by copying data describing selected objects and graphic modes (such as
 * pen, brush, palette, font, pen, region, drawing mode, and mapping mode)
 * to a context stack.
 *
 * \param hdc The handle to the DC whose state is to be saved.
 *
 * \return If the function succeeds, the return value identifies the saved
 *         state. If the function fails, the return value is zero.
 *
 * \note This function can be used any number of times to save any number
 *       of instances of the DC state. A saved state can be restored by using
 *       the \a RestoreDC function.
 *
 * \sa RestoreDC
 */
MG_EXPORT int GUIAPI SaveDC (HDC hdc);

/**
 * \fn BOOL GUIAPI RestoreDC (HDC hdc, int saved_dc)
 * \brief Restores a device context (DC) to the specified state.
 *
 * This function restores a device context (DC) to the specified state.
 * The DC is restored by popping state information off a stack created
 * by earlier calls to the \a SaveDC function.
 *
 * \param hdc The handle to the DC.
 * \param saved_dc Specifies the saved state to be restored. If this
 *        parameter is positive, saved_dc represents a specific instance
 *        of the state to be restored. If this parameter is negative,
 *        saved_dc represents an instance relative to the current state.
 *        For example, -1 restores the most recently saved state.
 *
 * \return If the function succeeds, the return value is nonzero. If the
 *         function fails, the return value is zero.
 *
 * \note The stack can contain the state information for several instances
 *       of the DC. If the state specified by the specified parameter is
 *       not at the top of the stack, RestoreDC deletes all state information
 *       between the top of the stack and the specified instance.
 *
 * \sa SaveDC
 */
MG_EXPORT BOOL GUIAPI RestoreDC (HDC hdc, int saved_dc);

#define MEMDC_FLAG_NONE             0x00000000  /* None. */
#define MEMDC_FLAG_SWSURFACE        0x00000000  /* DC is in system memory */
#define MEMDC_FLAG_HWSURFACE        0x00000001  /* DC is in video memory */
#define MEMDC_FLAG_SRCCOLORKEY      0x00001000  /* Blit uses a source color key */
#define MEMDC_FLAG_SRCALPHA         0x00010000  /* Blit uses source alpha blending*/
#define MEMDC_FLAG_SRCPIXELALPHA    0x00020000  /* Blit uses source per-pixel alpha blending*/
#define MEMDC_FLAG_RLEACCEL         0x00004000  /* Surface is RLE encoded */

/**
 * \fn HDC GUIAPI CreateCompatibleDCEx (HDC hdc, int width, int height)
 * \brief Creates a memory DC which is compatible with the specified
 *        reference DC.
 *
 * This function creates a memory DC which have the same pixel format as the
 * specified reference DC \a hdc. The same pixel format means that the memory
 * DC will have the same pixel depth, the same RGB composition, or the same
 * palette as the reference DC. Note that the memdc will have the same DC
 * attributes as the reference DC.
 *
 * \param hdc The handle to the reference DC.
 * \param width The expected width of the result memory DC. If it is zero,
 *        the width will be equal to the width of the reference DC.
 * \param height The expected height of the result memory DC. If it is zero,
 *        the height will be equal to the height of the reference DC.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \sa CreateCompatibleDC
 */
MG_EXPORT HDC GUIAPI CreateCompatibleDCEx (HDC hdc, int width, int height);

/**
 * \fn BOOL GUIAPI IsCompatibleDC (HDC hdc1, HDC hdc2)
 * \brief Check whether a given DC is compliant to a specific DC.
 *
 * This function checks whether a give DC \a hdc2 is compliant to the specific
 * DC \a hdc1.
 *
 * \param hdc1 The handle to a DC.
 * \param hdc2 The handle to another DC.
 *
 * \return TRUE when hdc2 is compiliant to hdc1, otherwise FALSE.
 *
 * \sa CreateCompatibleDCEx
 */
MG_EXPORT BOOL GUIAPI IsCompatibleDC (HDC hdc1, HDC hdc2);

/**
 * \fn BOOL GUIAPI IsMemDC (HDC hdc)
 * \brief Check whether a given DC is a memory DC.
 *
 * This function checks whether a give DC \a hdc is a memory DC.
 *
 * \param hdc The handle to the DC.
 *
 * \return TRUE for memory DC, otherwise FALSE.
 *
 * \sa CreateMemDCEx
 */
MG_EXPORT BOOL GUIAPI IsMemDC (HDC hdc);

/**
 * \fn BOOL GUIAPI IsScreenDC (HDC hdc)
 * \brief Check whether a given DC is a screen DC.
 *
 * This function checks whether a give DC \a hdc is a screen DC.
 *
 * \param hdc The handle to the DC.
 *
 * \return TRUE for screen DC, otherwise FALSE.
 *
 * \sa CreateMemDCEx
 */
MG_EXPORT BOOL GUIAPI IsScreenDC (HDC hdc);

/**
 * \fn BOOL GUIAPI IsWindowDC (HDC hdc)
 * \brief Check whether a given DC is a window DC.
 *
 * This function checks whether a give DC \a hdc is a window DC.
 *
 * \param hdc The handle to the DC.
 *
 * \return TRUE for window DC, otherwise FALSE.
 *
 * \sa CreateMemDCEx
 */
MG_EXPORT BOOL GUIAPI IsWindowDC (HDC hdc);

/**
 * \fn HDC GUIAPI CreateMemDCEx (int width, int height, int depth, DWORD flags, \
 Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask,
 void* bits, int pitch)
 * \brief Creates a memory DC.
 *
 * This function creates a memory DC which have the specified flags and
 * pixel format, and uses the pre-allocated buffer as the surface if \a bits
 * is not NULL.
 *
 * \param width The expected width of the result memory DC.
 * \param height The expected height of the result memory DC.
 * \param depth The expected color depth of the memory DC.
 * \param flags The memory DC flags, can be or'ed values of the following flags:
 *
 *   - MEMDC_FLAG_SWSURFACE\n
 *     Creates the surface of memory DC in the system memory.
 *   - MEMDC_FLAG_HWSURFACE\n
 *     Creates the surface of memory DC in the video memory.
 *   - MEMDC_FLAG_SRCCOLORKEY\n
 *     The created memory DC will use a source color key to blit to other DC.
 *   - MEMDC_FLAG_SRCALPHA\n
 *     The created memory DC will use a source alpha blending to blit to
 *     other DC.
 *   - MEMDC_FLAG_RLEACCEL\n
 *     The memory DC will be RLE encoded.
 *
 * \param Rmask The bit-masks of the red components in a pixel value.
 * \param Gmask The bit-masks of the green components in a pixel value.
 * \param Bmask The bit-masks of the blue components in a pixel value.
 * \param Amask The bit-masks of the alpha components in a pixel value.
 * \param bits The pointer to the pre-allocated surface. If this is NULL,
 *        MiniGUI will try to allocate the surface from video memory or
 *        system memory.
 * \param pitch The pitch (the number of bytes of one scan line) of the surface.
 *
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note If you pass a pre-allocated surface buffer to this function, MiniGUI
 *       will not free the buffer when you delete the memdc by calling DeleteMemDC.
 *
 * \sa CreateMemDC, CreateMemDCFromBitmap, CreateMemDCFromMyBitmap, CreateCompatibleDCEx
 */
MG_EXPORT HDC GUIAPI CreateMemDCEx (int width, int height, int depth, DWORD flags,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask,
        void* bits, int pitch);

/**
 * \fn HDC GUIAPI CreateMemDC (int width, int height, int depth, DWORD flags, \
 Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
 * \brief Creates a memory DC.
 *
 * This function creates a memory DC which have the specified flags and
 * pixel format.
 *
 * \param width The expected width of the result memory DC.
 * \param height The expected height of the result memory DC.
 * \param depth The expected color depth of the memory DC.
 * \param flags The memory DC flags, can be or'ed values of the following flags:
 *
 *   - MEMDC_FLAG_SWSURFACE\n
 *     Creates the surface of memory DC in the system memory.
 *   - MEMDC_FLAG_HWSURFACE\n
 *     Creates the surface of memory DC in the video memory.
 *   - MEMDC_FLAG_SRCCOLORKEY\n
 *     The created memory DC will use a source color key to blit to other DC.
 *   - MEMDC_FLAG_SRCALPHA\n
 *     The created memory DC will use a source alpha blending to blit to
 *     other DC.
 *   - MEMDC_FLAG_RLEACCEL\n
 *     The memory DC will be RLE encoded.
 *
 * \param Rmask The bit-masks of the red components in a pixel value.
 * \param Gmask The bit-masks of the green components in a pixel value.
 * \param Bmask The bit-masks of the blue components in a pixel value.
 * \param Amask The bit-masks of the alpha components in a pixel value.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \sa CreateMemDCEx, CreateMemDCFromBitmap, CreateMemDCFromMyBitmap, CreateCompatibleDCEx
 */
static inline HDC GUIAPI CreateMemDC (int width, int height, int depth, DWORD flags,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    return CreateMemDCEx (width, height, depth, flags, Rmask, Gmask, Bmask, Amask,
            NULL, 0);
}

/**
 * \fn HDC GUIAPI CreateSubMemDC (HDC parent, int off_x, int off_y, \
                int width, int height, BOOL comp_to_parent)
 * \brief Creates a sub memory DC in the given memory DC.
 *
 * This function creates a sub memory DC in the given memory DC (the parent
 * memory DC). Note that the new sub memdc will have the same DC attributes
 * as the parent memdc.
 *
 * \param parent The handle to the parent memory DC.
 * \param off_x The x-coordinate of the sub memory DC in the parent DC (in
 *        device coordinate system).
 * \param off_y The y-coordinate of the sub memory DC in the parent DC (in
 *        device coordinate system).
 * \param width The expected width of the sub memory DC.
 * \param height The expected height of the sub memory DC.
 * \param comp_to_parent The flag indicates whether the sub memdc is compliant
 *        to the parent dc.
 * \return The handle to the memory DC; HDC_INVALID indicates an error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDC
 */
MG_EXPORT HDC GUIAPI CreateSubMemDC (HDC parent, int off_x, int off_y,
                int width, int height, BOOL comp_to_parent);

/**
 * \fn HDC GUIAPI CreateMemDCFromBitmap (HDC hdc, const BITMAP* bmp)
 * \brief Creates a memory DC from a reference DC and a BITMAP object.
 *
 * This function creates a memory DC compatible with the specified DC,
 * and use the bits of the BITMAP object as the surface of the memory DC.
 * The created memory DC will have the same geometry as the BITMAP object.
 *
 * \param hdc The reference DC.
 * \param bmp The BITMAP object.
 *
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \sa CreateMemDCFromMyBitmap, DeleteMemDC, BITMAP
 */
MG_EXPORT HDC GUIAPI CreateMemDCFromBitmap (HDC hdc, const BITMAP* bmp);

/**
 * \fn HDC GUIAPI CreateMemDCFromMyBitmap (const MYBITMAP* my_bmp, const RGB* pal)
 * \brief Creates a memory DC from a device independent MYBITMAP object.
 *
 * This function creates a memory DC which have the same
 * pixel format as the MYBITMAP object \a my_bmp, and use the bits of
 * the MYBITMAP object as the surface of the memory DC. The created memory DC
 * will have the same geometry as the MYBITMAP object. If the depth of \a my_bmp
 * is 8-bit, the function will use \a pal to initialize the palette of
 * the memory DC.
 *
 * Note that the color depth of a memory DC can not be less than 8.
 * Thefore, if the color depth of the MYBITMAP object is less than 8,
 * this function will return HDC_INVALID.
 *
 * Note that if the MYBITMAP object has the flag MYBMP_FLOW_UP, this
 * function will return HDC_INVALID as well.
 *
 * \param my_bmp The device independent MYBITMAP object.
 * \param pal The palette of the MYBITMAP object. If the depth of \a my_bmp
 *        is larger than 8-bit, this argument can be NULL.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \sa CreateMemDCFromBitmap, DeleteMemDC, MYBITMAP
 */
MG_EXPORT HDC GUIAPI CreateMemDCFromMyBitmap (const MYBITMAP* my_bmp, const RGB* pal);

/**
 * \fn BOOL GUIAPI ConvertMemDC (HDC mem_dc, HDC ref_dc, DWORD flags)
 * \brief Converts a memory DC to have a same format as a reference DC.
 *
 * This function converts a memory DC \a mem_dc in order to
 * let it have the same pixel format as the reference DC \a ref_dc.
 * This function will try to create a new surface for \a mem_dc,
 * and then copies and maps the surface of \a ref_dc to it so the blit of
 * the converted memory DC will be as fast as possible.
 *
 * The \a flags parameter has the same semantics as \a CreateMemDC.
 * You can also pass MEMDC_FLAG_RLEACCEL in the flags parameter and
 * MiniGUI will try to RLE accelerate colorkey and alpha blits in the
 * resulting memory DC.
 *
 * \param mem_dc The device context to be converted.
 * \param ref_dc The reference device context.
 * \param flags The memory DC flags, has the same semantics as \a CreateMemDC.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateMemDC, DeleteMemDC
 */
MG_EXPORT BOOL GUIAPI ConvertMemDC (HDC mem_dc, HDC ref_dc, DWORD flags);

/**
 * \fn BOOL GUIAPI SetMemDCAlpha (HDC mem_dc, DWORD flags, Uint8 alpha)
 * \brief Set the alpha value for the entire surface of a DC, as opposed to
 *        using the alpha component of each pixel.
 *
 * This function sets the alpha value for the entire surface of the DC
 * \a mem_dc, as opposed to using the alpha component of each pixel.
 * This value measures the range of transparency of the surface, 0 being
 * completely transparent to 255 being completely opaque. An \a alpha value
 * of 255 causes blits to be opaque, the source pixels copied to the
 * destination (the default). Note that per-surface alpha can be combined
 * with colorkey transparency.
 *
 * If \a flags is 0, alpha blending is disabled for the surface.
 * If \a flags is MEMDC_FLAG_SRCALPHA, alpha blending is enabled for the
 * surface. OR'ing the flag with MEMDC_FLAG_RLEACCEL requests RLE acceleration
 * for the surface; if MEMDC_FLAG_RLEACCEL is not specified, the RLE
 * acceleration will be removed.
 * If \a flags is MEMDC_FLAG_SRCPIXELALPHA, per-pixel alpha blending is enabled
 * for the surface.
 *
 * \param mem_dc The device context.
 * \param flags The alpha value specific memory DC flags.
 * \param alpha the alpha value for the entire surface of the DC \a mem_dc.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetMemDCColorKey
 */
MG_EXPORT BOOL GUIAPI SetMemDCAlpha (HDC mem_dc, DWORD flags, Uint8 alpha);

/**
 * \fn BOOL GUIAPI SetMemDCColorKey (HDC mem_dc, DWORD flags, Uint32 color_key)
 * \brief Set the color key (transparent pixel) of a memory DC.
 *
 * This function sets the color key (transparent pixel) of the memory DC
 * \a mem_dc. If \a flags is MEMDC_FLAG_SRCCOLORKEY (optionally OR'ed with
 * MEMDC_FLAG_RLEACCEL), \a color_key will be the transparent pixel in the
 * source DC of a blit. MEMDC_FLAG_RLEACCEL requests RLE acceleration for
 * the source of the DC if present, and removes RLE acceleration if absent.
 * If \a flags is 0, this function clears any current color key.
 *
 * \param mem_dc The device context.
 * \param flags The color key specific memory DC flags.
 * \param color_key The color_key of the memory DC.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetMemDCAlpha
 */
MG_EXPORT BOOL GUIAPI SetMemDCColorKey (HDC mem_dc, DWORD flags,
                Uint32 color_key);

/**
 * \fn void GUIAPI DeleteMemDC (HDC mem_dc)
 * \brief Deletes a memory DC.
 *
 * This function deletes the memory DC \a mem_dc, and frees the surface of
 * the DC. For the memory DC created from BITMAP object or MYBITMAP object,
 * the bits used by the surface of the DC will be reserved.
 *
 * \param mem_dc The device context to be deleted.
 *
 * \sa CreateMemDC, CreateMemDCFromBitmap, CreateMemDCFromMyBitmap
 */
MG_EXPORT void GUIAPI DeleteMemDC (HDC mem_dc);

/**
 * \def CreateCompatibleDC(hdc)
 * \brief Creates a memory DC which is compatible with a given DC.
 *
 * This function creates a memory DC fully compatible with the reference
 * DC \a hdc, including pixel format and geomatry.
 *
 * \param hdc The reference DC.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \sa CreateCompatibleDCEx, DeleteMemDC
 */
#define CreateCompatibleDC(hdc) CreateCompatibleDCEx(hdc, 0, 0)

/**
 * \def DeleteCompatibleDC(hdc)
 * \brief Deletes a memory DC.
 *
 * This function deletes a memory DC created by \a CreateCompatibleDC.
 *
 * \param hdc The device context to be deleted.
 *
 * \sa CreateCompatibleDC, DeleteMemDC
 */
#define DeleteCompatibleDC(hdc) DeleteMemDC(hdc)

/**
 * \fn Uint8* GUIAPI LockDC (HDC hdc, const RECT* rw_rc, \
                int* width, int* height, int* pitch)
 * \brief Locks a dc to get direct access to pixels in the DC.
 *
 * Calling this function will try to lock the DC \a hdc to directly access
 * the pixels of the DC. You should tell this function the rectangle to be
 * accessed, and the function will return the requested \a width, \a height
 * and \a pitch of the DC. The access beyond requested width and height
 * will be invalid.
 *
 * Locking a DC which uses screen surface will lock some global objects,
 * such as mouse cursor, and so on. All GDI calls of other threads
 * (in MiniGUI-Threads) or other process (in MiniGUI-Processes) will be blocked
 * as well. So you should call \a UnlockDC to unlock the DC as soon as
 * possible, and should not call any system function in the duration of
 * locking the DC.
 *
 * \param hdc The handle to the device context.
 * \param rw_rc The rectangle in device coordinate system to be accessed
 *        in the DC.
 * \param width The width of the effective rectangle can access will be
 *        returned through this pointer.
 * \param height The height of the effective rectangle can access will be
 *        returned through this pointer.
 * \param pitch The pitch of the scan line of the DC will be returned
 *        through this pointer. Pitch means the length of the scan line
 *        in bytes.
 * \return The bits pointer to the upper-left corner of the requested
 *        rectangle, NULL on error.
 *
 * \sa UnlockDC, LockDCEx
 *
 * Example:
 *
 * \include lockdc.c
 */
MG_EXPORT Uint8* GUIAPI LockDC (HDC hdc, const RECT* rw_rc,
                int* width, int* height, int* pitch);

/**
 * \var typedef void (* CB_DIRECT_DRAW_RECT) (HDC hdc, Uint8 * pixels, \
                int pitch, int bytesPerPixel, const RECT* rc, void* context);
 *
 * \brief Type of directly access pixels hook function.
 *
 * This function is invoked by LockDCEx, application should implement this
 * function in which could directly read and write the pixels in a DC.
 *
 * \param hdc the device context.
 * \param pixels the pixels addr point to the rc's left-top in DC.
 * \param pitch (the bytes of one scan line) of the DC.
 * \param bytesPerPixel bytes per pixel.
 * \param rc the rectangle to draw, the field left of which is mapped to pixels.
 * \param context the context provided by application.
 */
typedef void (* CB_DIRECT_DRAW_RECT) (HDC hdc, Uint8 * pixels,
        int pitch, int bytesPerPixel, const RECT* rc, void* context);

/**
 * \fn  BOOL GUIAPI LockDCEx (HDC hdc, const PCLIPRGN region, \
 *      void* context, CB_DIRECT_DRAW_RECT cb)
 *
 * \brief directly access the pixels in a DC.
 *
 * This function fill \a region by function \a cb that directly accesses
 * pixels in a DC.
 * This function is used to draw region more effiently. The function that
 * lock __mg_gdilock should not be invoked any more after invoking this
 * function, or else it will cause deadlock. UnlockDC should be invoked
 * after invoking this function that will call back CB_DIRECT_DRAW_RECT,
 * other process could not draw any more until the DC has been unlocked.
 *
 * \param hdc the device context.
 * \param region the visible region to draw.
 * \param context the context provided by application, which is as a
 *           parameter of \a cb.
 * \param cb the direct draw function that directly accesses pixels
 *           which should be implemented by application. if cb is NULL,
 *           the region will be filled by current brush color.
 *
 * \return if LockDCEx success return TRUE, else return FALSE;
 *
 * \sa UnlockDC, LockDC
 */
MG_EXPORT BOOL GUIAPI LockDCEx (HDC hdc, const PCLIPRGN region,
        void* context, CB_DIRECT_DRAW_RECT cb);

/**
 * \fn void GUIAPI UnlockDC (HDC hdc)
 * \brief Unlocks a locked DC.
 *
 * \param hdc The locked DC.
 *
 * \sa LockDC, LockDCEx
 */
MG_EXPORT void GUIAPI UnlockDC (HDC hdc);

/**
 * \fn HDC GUIAPI CreatePrivateDC (HWND hwnd)
 * \brief Creates a private window DC of a window.
 *
 * This function creates a private window DC of the window \a hwnd and
 * returns the handle to the DC.
 *
 * When you calling \a CreatePrivateDC function to create a private DC,
 * MiniGUI will create the DC in the system heap, rather than allocating one
 * from the DC pool. Thus, you can keep up the private DC in the life cycle
 * of the window, and are not needed to release it for using by other windows.
 *
 * Since 5.0.0, if the main window which contains the window has secondary
 * DC, this function will make the private data using the memory surface
 * for the secondary DC.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa DeletePrivateDC
 */
MG_EXPORT HDC GUIAPI CreatePrivateDC (HWND hwnd);

/**
 * \fn HDC GUIAPI CreatePrivateClientDC (HWND hwnd)
 * \brief Creates a private client DC of a window.
 *
 * This function creates a private client DC of the window \a hwnd
 * and returns the handle to the DC.
 *
 * When you calling \a CreatePrivateClientDC function to create a private
 * client DC, MiniGUI will create the DC in the system heap, rather than
 * allocating one from the DC pool. Thus, you can keep up the DC in the life
 * cycle of the window, and are not needed to release it for using by
 * other windows.
 *
 * Since 5.0.0, if the main window which contains the window has secondary
 * DC, this function will make the private data using the memory surface
 * for the secondary DC.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa DeletePrivateDC
 */
MG_EXPORT HDC GUIAPI CreatePrivateClientDC (HWND hwnd);

/**
 * \fn HDC GUIAPI CreatePrivateSubDC (HDC hdc, int off_x, int off_y, \
 *         int width, int height)
 * \brief Creates a private SubDC of a window.
 *
 * This function creates a private sub DC of the DC and returns the handle
 * of the SubDC.
 *
 * When you calling \a CreatePrivateSubDC function to create a private
 * a sub DC, MiniGUI will create the DC in the system heap, rather than
 * allocating one from the DC pool. Thus, you can keep up the DC in the life
 * cycle of the window, and are not needed to release it for using by
 * other windows.
 *
 * \param hdc   The handle of the DC.
 * \param off_x The x-coordinate of the sub DC in the parent DC (in device
 *              coordinate system).
 * \param off_y The y-coordinate of the sub DC in the parent DC (in device
 *              coordinate system).
 * \param width The expected width of the sub DC.
 * \param height The expected height of the sub DC.
 *
 * \return The handle to SubDC, HDC_INVALID indicates an error.
 *
 * \sa DeletePrivateDC
 */
HDC GUIAPI CreatePrivateSubDC(HDC hdc, int off_x, int off_y,
        int width, int height);

/**
 * \fn HDC GUIAPI GetPrivateClientDC (HWND hwnd)
 * \brief Returns the private client DC of a window.
 *
 * This function returns the private client DC of the window \a hwnd which
 * have extended style \a WS_EX_USEPRIVATECDC.
 *
 * When a main window have the extended style \a WS_EX_USEPRIVATECDC, or a
 * control class have the style \a CS_OWNDC, MiniGUI will create a private
 * client DC for this window in the creation progress of the window, and
 * destroy the DC when you destroy the window, and use this private client DC
 * in default message handlers. So there will be some improvments on
 * drawing/repaint performance. You can alos call this function to get the
 * private client DC, and use it to draw anything in your window.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the private client DC, HDC_INVALID indicates an error.
 *
 * \sa CreatePrivateClientDC
 */
MG_EXPORT HDC GUIAPI GetPrivateClientDC (HWND hwnd);

/**
 * \fn void GUIAPI DeletePrivateDC (HDC hdc)
 * \brief Deletes the DC returned by \a CreatePrivateDC or
 *        \a CreatePrivateClientDC or \a CreatePrivateSubDC.
 *
 * \param hdc The handle to the DC.
 *
 * \sa CreatePrivateDC, CreatePrivateClientDC
 */
MG_EXPORT void GUIAPI DeletePrivateDC (HDC hdc);

/**
 * \fn HDC GUIAPI CreateSecondaryDC (HWND hwnd)
 * \brief Creates a secondary DC for a main window.
 *
 * This function creates a secondary DC for the main window \a hwnd and
 * returns the handle to the secondary DC.
 *
 * When you calling \a CreateSecondaryDC function, MiniGUI will create a
 * memory DC which is compatible with HDC_SCREEN.
 *
 * When a main window has set a secondary DC, MiniGUI will use the
 * off-screen surface of the secondary DC to render the content of the main
 * window and its descendants.
 *
 * When a main window have the extended style \a WS_EX_AUTOSECONDARYDC,
 * MiniGUI will create a secondary DC for this main window in the creation
 * progress of the main window, and destroy the DC when you destroy the
 * window.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa DeleteSecondaryDC
 */
MG_EXPORT HDC GUIAPI CreateSecondaryDC (HWND hwnd);

/**
 * \var typedef int (* ON_UPDATE_SECONDARYDC)(HWND hwnd, HDC secondary_dc, \
                HDC real_dc, const RECT* secondary_rc, const RECT* real_rc, \
                const RECT* main_update_rc);
 * \brief The callback type of on updating secondary DC.
 *
 * \param hwnd           The handle to the updated window.
 * \param secondary_dc   The update src dc.
 * \param real_dc        The update dst dc.
 * \param secondary_rc   The rectangle relative to secondary_dc.
 * \param real_rc        The rectangle relative to real_dc.
 * \param main_update_dc The rectangle relative to main window.
 *
 * \return The update result.
 */
typedef int (* ON_UPDATE_SECONDARYDC)(HWND hwnd, HDC secondary_dc,
        HDC real_dc, const RECT* secondary_rc, const RECT* real_rc,
        const RECT* main_update_rc);

#define ON_UPDSECDC_DEFAULT     ((ON_UPDATE_SECONDARYDC)0)
#define ON_UPDSECDC_DONOTHING   ((ON_UPDATE_SECONDARYDC)-1)

/**
 * \fn HDC GUIAPI SetSecondaryDC (HWND hwnd, HDC secondary_dc, \
                   ON_UPDATE_SECONDARYDC on_update_secondarydc)
 * \brief Set a window's secondary DC and the callback procedure for the secondary
 *        DC update.
 *
 * This function set the secondary DC of the main window \a hwnd and returns
 * the handle to the old seconadary DC.
 *
 * \param hwnd The handle to the main window.
 * \param secondary_dc The secondary DC which will be the new secondary DC of the
 *        main window.
 * \param on_update_secondarydc The callback procedure for the secondary DC update.
 *        You can pass one of two sepcial values:
 *          - ON_UPDSECDC_DEFAULT\n
 *            The default operation to update the secondary DC to the real DC
 *            of the main window.
 *          - ON_UPDSECDC_DONOTHING\n
 *            No any operation occures.
 *
 * \return The handle to the old secondary DC, HDC_SCREEN indicates the main
 *         window has no secondary DC before calling this function.
 *
 * \sa DeleteSecondaryDC
 */
MG_EXPORT HDC GUIAPI SetSecondaryDC (HWND hwnd, HDC secondary_dc,
        ON_UPDATE_SECONDARYDC on_update_secondarydc);

/**
 * \fn HDC GUIAPI GetSecondaryDC (HWND hwnd)
 * \brief Retrieve and return the secondary DC of a specific window.
 *
 * This function retrieves and returns the secondary DC of the main window
 * which contains the specified window \a hwnd.
 *
 * When a main window has been set the secondary DC, MiniGUI will use the
 * off-screen surface of the secondary DC to render the content of the main
 * window and its descendants.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the secondary DC; HDC_SCREEN indicates that
 *         the window has no secondary DC.
 *
 * \sa CreateSecondaryDC, SetSecondaryDC
 */
MG_EXPORT HDC GUIAPI GetSecondaryDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetDCInSecondarySurface (HWND hWnd, BOOL bClient)
 * \brief Get a window or client DC by using the secondary surface if possible.
 *
 * This function gets a window DC or a client DC by using the surface of
 * the secondary DC created by the main window which contains the specified
 * window \a hWnd. MiniGUI will try to return an unused DC from
 * the internal DC pool, rather than allocating a new one from the system heap.
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * If the main window which contains this window does not have secondary DC,
 * this function acts as \a GetDCEx.
 *
 * \param hWnd The handle to the main window.
 * \param bClient Whether to initialize as a window or client DC.
 *  TRUE for client DC, FALSE for window DC.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa GetDCEx, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetDCInSecondarySurface (HWND hWnd, BOOL bClient);

/**
 * \fn HDC GUIAPI GetSecondaryClientDC (HWND hwnd)
 * \brief Get a client DC of a window by using the secondary surface if possible.
 *
 * This function gets a client DC by using the surface of the secondary DC
 * created by the main window which contains the specified window \a hWnd.
 * MiniGUI will try to return an unused DC from the internal DC pool,
 * rather than allocating a new one from the system heap.
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * If the main window which contains this window (or itself) does not have a
 * secondary DC, this function acts as \a GetClientDC.
 *
 * \param hwnd The handle to a main window or a control.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa GetDCInSecondarySurface
 */
static inline HDC GUIAPI GetSecondaryClientDC (HWND hwnd)
{
    return GetDCInSecondarySurface (hwnd, TRUE);
}

/**
 * \fn void GUIAPI ReleaseSecondaryDC (HWND hwnd, HDC hdc)
 * \brief Release the DC returned by GetSecondaryClientDC.
 *
 * \param hwnd The handle to the window.
 * \param hdc  The handle to the secondary DC.
 *
 * \note Deprecated; use ReleaseDC instead.
 *
 * \sa ReleaseDC
 */
static inline void GUIAPI ReleaseSecondaryDC (HWND hwnd, HDC hdc)
{
    ReleaseDC (hdc);
}

/**
 * \fn void GUIAPI DeleteSecondaryDC (HWND hwnd)
 * \brief Deletes the secondary DC of the window.
 *
 * \param hwnd The handle to the window.
 *
 * \sa CreateSecondaryDC
 */
MG_EXPORT void GUIAPI DeleteSecondaryDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetEffectiveCDC (HWND hwnd)
 * \brief Get the effective client DC of a window.
 *
 * This function returns an effective client DC of the specified window by
 * using the following rules:
 *
 *  - If the window has private client DC, returns it.
 *  - If the main window contains the window has a secondary DC, returns a
 *    client DC by calling GetDCInSecondarySurface.
 *  - Returns a client DC by calling GetClientDC.
 *
 * You should release the client DC as soon as possible when you are done
 * with it.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa GetPrivateClientDC, GetDCInSecondarySurface, GetClientDC, ReleaseDC
 *
 * Since 5.0.0
 */
MG_EXPORT HDC GUIAPI GetEffectiveCDC (HWND hwnd);

/**
 * \fn BOOL GUIAPI SyncUpdateDC (HDC hdc)
 * \brief Synchronize the update rectangles of the surface corresponding to
 *      a DC to screen.
 *
 * This function synchronizes the update rectangles of the surface
 * corresponding to the DC specified by \a hdc to screen.
 *
 * \param hdc The handle to the DC.
 *
 * \return TRUE when there are rectangles to be synchronized and updated;
 *      FALSE when it is not necessory to update.
 *
 */
MG_EXPORT BOOL GUIAPI SyncUpdateDC (HDC hdc);

/**
 * \fn BOOL GUIAPI SyncUpdateSurface (HWND hwnd)
 * \brief Synchronize the update rectangles of the backing surface of
 *      a window to screen.
 *
 * This function synchronizes the update rectangles of the backing surface
 * of the window specified by \a hwnd to screen.
 *
 * \param hwnd The handle to the window.
 *
 * \return TRUE when there are rectangles to be synchronized and updated;
 *      FALSE when hwnd is invalid, or it is not necessory to update.
 */
MG_EXPORT BOOL GUIAPI SyncUpdateSurface (HWND hwnd);

    /** @} end of dc_fns */

    /**
     * \defgroup dc_attrs DC attribute operations
     * @{
     */

#define BM_TRANSPARENT          1
#define BM_OPAQUE               0

#define DC_ATTR_BK_COLOR        0
#define DC_ATTR_PEN_COLOR       1
#define DC_ATTR_BRUSH_COLOR     2
#define DC_ATTR_TEXT_COLOR      3

#define DC_ATTR_BK_MODE         4

#define DC_ATTR_TAB_STOP        5
#define DC_ATTR_CHAR_EXTRA      6
#define DC_ATTR_WORD_EXTRA      7
#define DC_ATTR_ALINE_EXTRA     8
#define DC_ATTR_BLINE_EXTRA     9

#define DC_ATTR_MAP_MODE        10

#define DC_ATTR_TEXT_ALIGN      11

#define DC_ATTR_BIDI_FLAGS      12

#ifdef _MGHAVE_ADV_2DAPI
# define DC_ATTR_PEN_TYPE        13
# define DC_ATTR_PEN_CAP_STYLE   14
# define DC_ATTR_PEN_JOIN_STYLE  15
# define DC_ATTR_PEN_WIDTH       16
# define DC_ATTR_BRUSH_TYPE      17
# define NR_DC_ATTRS             18
#else   /* _MGHAVE_ADV_2DAPI */
# define NR_DC_ATTRS             13
#endif  /* !_MGHAVE_ADV_2DAPI */


/**
 * \fn Uint32 GUIAPI GetDCAttr (HDC hdc, int attr)
 * \brief Get a specified attribute value of a DC.
 *
 * This function retrieves a specified attribute value of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param attr The attribute to be retrieved, can be one of the following values:
 *
 *      - DC_ATTR_BK_COLOR\n
 *        Background color.
 *      - DC_ATTR_BK_MODE\n
 *        Background mode.
 *      - DC_ATTR_PEN_TYPE\n
 *        Pen type.
 *      - DC_ATTR_PEN_CAP_STYLE\n
 *        Cap style of pen.
 *      - DC_ATTR_PEN_JOIN_STYLE\n
 *        Join style of pen.
 *      - DC_ATTR_PEN_COLOR\n
 *        Pen color.
 *      - DC_ATTR_BRUSH_TYPE\n
 *        Brush type.
 *      - DC_ATTR_BRUSH_COLOR\n
 *        Brush color.
 *      - DC_ATTR_TEXT_COLOR\n
 *        Text color.
 *      - DC_ATTR_TAB_STOP\n
 *        Tabstop width.
 *      - DC_ATTR_CHAR_EXTRA\n
 *        Inter-character spacing for the DC.
 *      - DC_ATTR_WORD_EXTRA\n
 *        Inter-word spacing for the DC.
 *      - DC_ATTR_ALINE_EXTRA\n
 *        Spacing above line for the DC.
 *      - DC_ATTR_BLINE_EXTRA\n
 *        Spacing bellow line for the DC.
 *      - DC_ATTR_MAP_MODE\n
 *        mode of a DC.
 *      - DC_ATTR_TEXT_ALIGN\n
 *        Text-alignment flags of a DC.
 *      - DC_ATTR_BIDI_FLAGS\n
 *        BIDI flags.
 * \return The attribute value.
 *
 * \sa SetDCAttr
 */
MG_EXPORT Uint32 GUIAPI GetDCAttr (HDC hdc, int attr);

/**
 * \fn Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value)
 * \brief Set a specified attribute value of a DC.
 *
 * This function sets a specified attribute value of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param attr The attribute to be set.
 * \param value The attribute value.
 * \return The old attribute value.
 *
 * \sa GetDCAttr
 */
MG_EXPORT Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value);

/**
 * \def GetBkColor(hdc)
 * \brief Get the background color of a DC.
 *
 * \param hdc The device context.
 * \return The background pixel value of the DC \a hdc.
 *
 * \sa GetDCAttr, SetBkColor
 */
#define GetBkColor(hdc)     GetDCAttr (hdc, DC_ATTR_BK_COLOR)

/**
 * \def GetBkMode(hdc)
 * \brief Get the background mode of a DC.
 *
 * \param hdc The device context.
 * \return The background mode of the DC \a hdc.
 *
 * \retval BM_TRANSPARENT
 *         Indicate that reserve the background untouched when drawing text.
 * \retval BM_OPAQUE
 *         Indicate that erase the background with background color when
 *         drawing text.
 *
 * \sa GetDCAttr, SetBkMode
 */
#define GetBkMode(hdc)      GetDCAttr (hdc, DC_ATTR_BK_MODE)

/**
 * \def GetPenColor(hdc)
 * \brief Get the pen color of a DC.
 *
 * \param hdc The device context.
 * \return The pen color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetPenColor
 */
#define GetPenColor(hdc)    GetDCAttr (hdc, DC_ATTR_PEN_COLOR)

/**
 * \def GetBrushColor(hdc)
 * \brief Get the brush color of a DC.
 *
 * \param hdc The device context.
 * \return The brush color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetBrushColor
 */
#define GetBrushColor(hdc)  GetDCAttr (hdc, DC_ATTR_BRUSH_COLOR)

/**
 * \def GetTextColor(hdc)
 * \brief Get the text color of a DC.
 *
 * \param hdc The device context.
 * \return The text color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetTextColor
 */
#define GetTextColor(hdc)   GetDCAttr (hdc, DC_ATTR_TEXT_COLOR)

/**
 * \def GetTabStop(hdc)
 * \brief Get the tabstop value of a DC.
 *
 * \param hdc The device context.
 * \return The tabstop value of the DC \a hdc.
 *
 * \sa GetDCAttr, SetTabStop
 */
#define GetTabStop(hdc)     GetDCAttr (hdc, DC_ATTR_TAB_STOP)

/**
 * \def SetBkColor(hdc, color)
 * \brief Set the background color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new background color (pixel value).
 * \return The old background pixel value of the DC \a hdc.
 *
 * \sa SetDCAttr, GetBkColor
 */
#define SetBkColor(hdc, color)  \
                SetDCAttr (hdc, DC_ATTR_BK_COLOR, (DWORD) color)

/**
 * \def SetBkMode(hdc, mode)
 * \brief Set the background color of a DC to a new mode.
 *
 * \param hdc The device context.
 * \param mode The new background mode, be can one of the following values:
 *
 *      - BM_TRANSPARENT\n
 *        Indicate that reserve the background untouched when drawing text.
 *      - BM_OPAQUE\n
 *        Indicate that erase the background with background color when
 *        drawing text.
 *
 * \return The old background mode of the DC \a hdc.
 *
 * \sa SetDCAttr, GetBkMode
 */
#define SetBkMode(hdc, mode)    \
                SetDCAttr (hdc, DC_ATTR_BK_MODE, (DWORD) mode)

/**
 * \def SetPenColor(hdc, color)
 * \brief Set the pen color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new pen color (pixel value).
 * \return The old pen pixel value of the DC \a hdc.
 *
 * \sa SetDCAttr, GetPenColor
 */
#define SetPenColor(hdc, color) \
                SetDCAttr (hdc, DC_ATTR_PEN_COLOR, (DWORD) color)

/**
 * \def SetBrushColor(hdc, color)
 * \brief Set the brush color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new brush color (pixel value).
 * \return The old brush pixel value of the DC \a hdc.
 *
 * \sa SetDCAttr, GetBrushColor
 */
#define SetBrushColor(hdc, color)   \
                SetDCAttr (hdc, DC_ATTR_BRUSH_COLOR, (DWORD) color)

/**
 * \def SetTextColor(hdc, color)
 * \brief Set the text color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new text color (pixel value).
 * \return The old text color (pixel value) of the DC \a hdc.
 *
 * \sa SetDCAttr, GetTextColor
 */
#define SetTextColor(hdc, color)    \
                SetDCAttr (hdc, DC_ATTR_TEXT_COLOR, (DWORD) color)

/**
 * \def SetTabStop(hdc, value)
 * \brief Set the tabstop of a DC to a new value.
 *
 * \param hdc The device context.
 * \param value The new tabstop value in pixels.
 * \return The old tabstop value in pixels of the DC \a hdc.
 *
 * \sa SetDCAttr, GetTabStop
 */
#define SetTabStop(hdc, value)  \
                SetDCAttr (hdc, DC_ATTR_TAB_STOP, (DWORD) value)

#define ROP_SET         0
#define ROP_AND         1
#define ROP_OR          2
#define ROP_XOR         3
#define ROP_COMPOSITE   4
#define ROP_COMP_USER   0x0100
/* Reserved for future
 * #define ROP_COMP_XXX    0x01XX
 */

/**
 * \fn int GUIAPI GetRasterOperation (HDC hdc)
 * \brief Get the raster operation of a DC.
 *
 * This function gets the raster operation of the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The current raster operation of the DC \a hdc.
 *
 * \retval ROP_SET
 *         Set to the new pixel value, erase original pixel on the surface.
 * \retval ROP_AND
 *         AND'd the new pixel value with the original pixel on the surface.
 * \retval ROP_OR
 *         OR'd the new pixel value with the original pixel on the surface.
 * \retval ROP_XOR
 *         XOR'd the new pixel value with the original pixel on the surface.
 * \retval ROP_COMPOSITE
 *         call default color composition the new pixel value with the original pixel on the surface.
 * \retval ROP_COMP_USER
 *         User defined color composition operations.
 *
 * \sa SetRasterOperation, SetUserCompositionOps
 */
MG_EXPORT int GUIAPI GetRasterOperation (HDC hdc);

/**
 * \fn int GUIAPI SetRasterOperation (HDC hdc, int rop)
 * \brief Set the raster operation of a DC to a new value.
 *
 * This function sets the raster operation of the DC \a hdc to the new
 * value \a rop.
 *
 * \param hdc The device context.
 * \param rop The new raster operation, can be one of the following values:
 *
 *      - ROP_SET\n
 *        Set to the new pixel value, erase original pixel on the surface.
 *      - ROP_AND\n
 *        AND'd the new pixel value with the original pixel on the surface.
 *      - ROP_OR\n
 *        OR'd the new pixel value with the original pixel on the surface.
 *      - ROP_XOR\n
 *        XOR'd the new pixel value with the original pixel on the surface.
 *      - ROP_COMPOSITE\n
 *        call default color composition operators the new pixel value with the original pixel on the surface.
 *      - ROP_COMP_USER\n
 *        call user defined color composition operators the new pixel value with the original pixel on the surface.
 *
 * \return The old raster operation of the DC \a hdc.
 *
 * \sa GetRasterOperation, SetUserCompositionOps
 */
MG_EXPORT int GUIAPI SetRasterOperation (HDC hdc, int rop);

    /** @} end of dc_attrs */

    /**
     * \defgroup pal_fns Palette operations
     * @{
     */

/**
 * \fn int GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
 * \brief Get palette entries of a DC.
 *
 * This function gets some palette entries of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param start The start entry of palette to be retrieved.
 * \param len The length of entries to be retrieved.
 * \param cmap The buffer receives the palette entries.
 *
 * \return The number of entries retrieved from the logical palette.
 *
 * \sa SetPalette
 */
MG_EXPORT int GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap);

/**
 * \fn BOOL GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
 * \brief Set palette entries of a DC.
 *
 * This function sets some palette entries of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param start The start entry of palette to be set.
 * \param len The length of entries to be set.
 * \param cmap Pointer to the palette entries.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetPalette
 */
MG_EXPORT BOOL GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap);

/**
 * \fn BOOL GUIAPI SetColorfulPalette (HDC hdc)
 * \brief Set a DC with colorful palette.
 *
 * This function sets the DC specified by \a hdc with colorful palette.
 *
 * \param hdc The device context.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetPalette
 */
MG_EXPORT BOOL GUIAPI SetColorfulPalette (HDC hdc);

/**
 * \fn HPALETTE GUIAPI CreatePalette (GAL_Palette* pal)
 * \brief Creates a logical palette.
 *
 * This function creates a logical palette from a palette entry \a pal.
 *
 * \param pal The Pointer to a GAL_Palette structure that contains information
 *        about the colors in the logical palette..
 *
 * \return Handle to the logical palette on success, otherwise 0.
 *
 * \sa GetPalette
 */
MG_EXPORT HPALETTE GUIAPI CreatePalette (GAL_Palette* pal);

/**
 * \fn BOOL GUIAPI SelectPalette (HDC hdc, HPALETTE hpal, BOOL reserved)
 *
 * \brief Selects a palette for a DC.
 *
 * This function the palette \a hpal to be the palette of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param hpal Handle to the palette.
 * \param reserved No use, reserved.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreatePalette
 */
MG_EXPORT BOOL GUIAPI SelectPalette (HDC hdc, HPALETTE hpal, BOOL reserved);

/**
 * \fn BOOL GUIAPI RealizePalette (HDC hdc)
 * \brief This function maps palette entries from the current
 *        logical palette to the system palette.
 *
 * This function modifies the palette for the device associated
 * with the specified device context.
 *
 * If the device context is a display DC, the physical palette
 * for that device is modified. This function will return
 * FALSE if the hdc does not have a settable palette.
 *
 * If the device context is a memory DC, this function will return
 * FALSE and do nothing.
 *
 * \param hdc Handle to the device context into which a logical
 *        palette has been selected.
 *
 * \return TRUE on success, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI RealizePalette (HDC hdc);

/**
 * \fn BOOL GUIAPI ResizePalette (HPALETTE hpal, int len)
 * \brief Resizes a logical palette.
 *
 * This function resizes a logical palette.
 *
 * \param hpal The handle to the logical palette.
 * \param len The new size of the logical palette.
 *
 * \return TRUE for success, else returns FALSE.
 *
 * \sa CreatePalette
 */
MG_EXPORT BOOL GUIAPI ResizePalette (HPALETTE hpal, int len);

/**
 * \fn void GUIAPI DestroyPalette (HPALETTE pal)
 * \brief Destroy a logical palette.
 *
 * This function destroyes a logical palette \a pal.
 *
 * \param pal The handle to the logical palette.
 *
 * \sa CreatePalette
 */
MG_EXPORT void GUIAPI DestroyPalette (HPALETTE hpal);

/**
 * \fn HPALETTE GUIAPI GetDefaultPalette (void);
 * \brief Get the system default logical palette.
 *
 * \return Handle to the system default logical palette on success, otherwise 0.
 *
 * \sa GetPalette
 */
MG_EXPORT HPALETTE GUIAPI GetDefaultPalette (void);

/**
 * \fn int GUIAPI GetPaletteEntries (HPALETTE hpal, \
                int start, int len, GAL_Color* cmap);
 * \brief Get palette entries of a logical palette.
 *
 * This function gets some palette entries of the logical palette \a hpal.
 *
 * \param hpal The logical palette.
 * \param start The start entry of palette to be retrieved.
 * \param len The length of entries to be retrieved.
 * \param cmap The buffer receives the palette entries.
 *
 * \return The number of entries retrieved from the logical palette.
 *
 * \sa GetPalette
 */
MG_EXPORT int GUIAPI GetPaletteEntries (HPALETTE hpal,
                int start, int len, GAL_Color* cmap);

/**
 * \fn int GUIAPI SetPaletteEntries (HPALETTE hpal, \
                int start, int len, GAL_Color* cmap);
 * \brief Set palette entries of a logical palette.
 *
 * This function sets some palette entries of the logical palette \a hpal.
 *
 * \param hpal The logical palette.
 * \param start The start entry of palette to be set.
 * \param len The length of entries to be set.
 * \param cmap The buffer contains the palette entries.
 *
 * \return The number of entries set for the logical palette.
 *
 * \sa SetPalette
 */
MG_EXPORT int GUIAPI SetPaletteEntries (HPALETTE hpal,
                int start, int len, GAL_Color* cmap);

/**
 * \fn UINT GUIAPI GetNearestPaletteIndex (HPALETTE hpal,
                Uint8 red, Uint8 green, Uint8 blue)
 * \brief Get the nearest palette index in the logical palette for a
 *        spefici color.
 *
 * This function gets the nearest palette index in the logical palette \a hpal
 * for the color (red, green, blue).
 *
 * \param hpal The logical palette.
 * \param red The red component value of the color.
 * \param green The green component value of the color.
 * \param blue The blue component value of the color.
 *
 * \return The index of palette entry in the logical palette.
 *
 * \sa GetNearestColor
 */
MG_EXPORT UINT GUIAPI GetNearestPaletteIndex (HPALETTE hpal,
                Uint8 red, Uint8 green, Uint8 blue);

/**
 * \fn RGBCOLOR GUIAPI GetNearestColor (HDC hdc,
                Uint8 red, Uint8 green, Uint8 blue)
 * \brief Get the nearest color compliant to a DC for a spefici color.
 *
 * This function gets the nearest color compliant to the DC \a hdc
 * for the specific color (red, green, blue).
 *
 * \param hdc The handle to the device context.
 * \param red The red component value of the color.
 * \param green The green component value of the color.
 * \param blue The blue component value of the color.
 *
 * \return The color of the nearest color.
 *
 * \sa GetNearestPaletteIndex
 */
MG_EXPORT RGBCOLOR GUIAPI GetNearestColor (HDC hdc,
                Uint8 red, Uint8 green, Uint8 blue);

    /** @} end of pal_fns */

    /**
     * \defgroup draw_fns General drawing functions
     * @{
     */

/**
 * \fn void GUIAPI SetPixel (HDC hdc, int x, int y, gal_pixel pixel)
 * \brief Set the pixel with a new pixel value at the specified position
 *        on a DC.
 *
 * This function sets the pixel with a pixel value \a pixel at the specified
 * position \a (x,y) on the DC \a hdc. You can the pre-defined standard system
 * pixel values.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param pixel The pixel value.
 *
 * \sa GetPixel, color_vars
 */
MG_EXPORT void GUIAPI SetPixel (HDC hdc, int x, int y, gal_pixel pixel);

/**
 * \fn gal_pixel GUIAPI SetPixelRGB (HDC hdc, int x, int y, \
                Uint8 r, Uint8 g, Uint8 b)
 * \brief Set the pixel by a RGB triple at the specified position on a DC.
 *
 * This function sets the pixel with a RGB triple \a (r,g,b) at the specified
 * position \a (x,y) on the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \return The pixel value of the RGB triple.
 *
 * \sa GetPixel, RGB2Pixel
 */
MG_EXPORT gal_pixel GUIAPI SetPixelRGB (HDC hdc, int x, int y,
                Uint8 r, Uint8 g, Uint8 b);

/**
 * \fn gal_pixel GUIAPI SetPixelRGBA (HDC hdc, int x, int y, \
                Uint8 r, Uint8 g, Uint8 b, Uint8 a)
 * \brief Set the pixel by a RGBA quarter at the specified position on a DC.
 *
 * This function sets the pixel with a RGBA quarter \a (r,g,b,a) at the
 * specified position \a (x,y) on the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \param a The alpha component of a color.
 *
 * \return The pixel value of the RGBA quarter.
 *
 * \sa GetPixel, RGBA2Pixel
 */
MG_EXPORT gal_pixel GUIAPI SetPixelRGBA (HDC hdc, int x, int y,
                Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/**
 * \fn gal_pixel GUIAPI GetPixel (HDC hdc, int x, int y)
 * \brief Get the pixel value at the specified position on a DC.
 *
 * This function gets the pixel value at the specified position \a (x,y) on
 * the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 *
 * \return The pixel value.
 *
 * \sa SetPixel
 */
MG_EXPORT gal_pixel GUIAPI GetPixel (HDC hdc, int x, int y);

/**
 * \fn gal_pixel GUIAPI GetPixelRGB (HDC hdc, int x, int y, \
                Uint8* r, Uint8* g, Uint8* b)
 * \brief Get the pixel value at the specified position on a DC in RGB triple.
 *
 * This function gets the pixel value at the specified position \a (x,y)
 * on the DC \a hdc in RGB triple.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of the pixel will be returned through
 *          this pointer.
 * \param g The green component of the pixel will be returned through
 *          this pointer.
 * \param b The blue component of the pixel will be returned through
 *          this pointer.
 * \return The pixel value.
 *
 * \sa SetPixelRGB
 */
MG_EXPORT gal_pixel GUIAPI GetPixelRGB (HDC hdc, int x, int y,
                Uint8* r, Uint8* g, Uint8* b);

/**
 * \fn gal_pixel GUIAPI GetPixelRGBA (HDC hdc, int x, int y, \
                Uint8* r, Uint8* g, Uint8* b, Uint8* a)
 * \brief Get the pixel value at the specified position on a DC in
 *        RGBA quarter.
 *
 * This function gets the pixel value at the specified position \a (x,y) on
 * the DC \a hdc in RGBA quarter.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of the pixel will be returned through
 *        this pointers.
 * \param g The green component of the pixel will be returned through
 *        this pointers.
 * \param b The blue component of the pixel will be returned through
 *        this pointers.
 * \param a The alpha component of the pixel will be returned through
 *        this pointers.
 *
 * \return The pixel value.
 *
 * \sa SetPixelRGBA
 */
MG_EXPORT gal_pixel GUIAPI GetPixelRGBA (HDC hdc, int x, int y,
                Uint8* r, Uint8* g, Uint8* b, Uint8* a);

/**
 * \fn void GUIAPI RGBA2Pixels (HDC hdc, const RGB* rgbs, gal_pixel* pixels, int count)
 * \brief Get the pixel values from a color array in RGBA quarter under a DC.
 *
 * This function gets the pixel values from the RGB quarter array \a rgbs
 * under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param rgbs The array of RGBA quarters to be converted.
 * \param pixels The array returns the converted pixel values.
 * \param count The number of RGBA quarters.
 *
 * \sa Pixel2RGBAs
 */
MG_EXPORT void GUIAPI RGBA2Pixels (HDC hdc, const RGB* rgbs,
                gal_pixel* pixels, int count);

/**
 * \fn gal_pixel GUIAPI RGBA2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
 * \brief Get the pixel value from a color in RGBA quarter under a DC.
 *
 * This function gets the pixel value from the RGB quarter \a (r,g,b,a)
 * under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \param a The alpha component of a color.
 * \return The pixel value.
 *
 * \note This function is implemented as an inline to RGBA2Pixels.
 *
 * \sa RGBA2Pixels, Pixel2RGBA
 */
static inline gal_pixel RGBA2Pixel (HDC hdc,
                Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    RGB rgb = {r, g, b, a};
    gal_pixel pixel;

    RGBA2Pixels (hdc, &rgb, &pixel, 1);
    return pixel;
}

/**
 * \fn void GUIAPI RGB2Pixels (HDC hdc, const RGB* rgbs, gal_pixel* pixels, int count)
 * \brief Get the pixel values from a color array in RGB triple under a DC.
 *
 * This function gets the pixel values from the RGB triple array \a rgbs under
 * the DC \a hdc.
 *
 * \param hdc The device context.
 * \param rgbs The array of RGB triples to be converted.
 * \param pixels The array returns the converted pixel values.
 * \param count The number of RGB triples.
 *
 * \sa Pixel2RGBs
 */
MG_EXPORT void GUIAPI RGB2Pixels (HDC hdc, const RGB* rgbs,
                gal_pixel* pixels, int count);

/**
 * \fn gal_pixel GUIAPI RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b)
 * \brief Get the pixel value from a color in RGB triple under a DC.
 *
 * This function gets the pixel value from the RGB triple \a (r,g,b) under
 * the DC \a hdc.
 *
 * \param hdc The device context.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \return The pixel value.
 *
 * \note This function is implemented as an inline to RGB2Pixels.
 *
 * \sa RGB2Pixels, Pixel2RGB
 */
static inline gal_pixel RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b)
{
    RGB rgb = {r, g, b, 0xFF};
    gal_pixel pixel;

    RGB2Pixels (hdc, &rgb, &pixel, 1);
    return pixel;
}

/**
 * \fn void GUIAPI Pixel2RGBs (HDC hdc, const gal_pixel* pixels, RGB* rgbs, int count)
 * \brief Get the colors in RGB triple from a pixel value array under a DC.
 *
 * This function gets the colors in RGB triple from the pixel value array
 * \a pixels under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixels The array of pixel values.
 * \param rgbs The array returns the converted RGB triples.
 * \param count The number of pixels.
 *
 * \sa RGB2Pixels
 */
MG_EXPORT void GUIAPI Pixel2RGBs (HDC hdc, const gal_pixel* pixels,
                RGB* rgbs, int count);

/**
 * \fn void GUIAPI Pixel2RGB (HDC hdc, gal_pixel pixel, \
                Uint8* r, Uint8* g, Uint8* b)
 * \brief Get the color in RGB triple from a pixel value under a DC.
 *
 * This function gets the color in RGB triple from the pixel value \a pixel
 * under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixel The pixel value.
 * \param r The red component of the pixel will be returned through
 *        this pointer.
 * \param g The green component of the pixel will be returned through
 *        this pointer.
 * \param b The blue component of the pixel will be returned through
 *        this pointer.
 *
 * \note This function is implemented as an inline to RGB2Pixels.
 *
 * \sa Pixel2RGBs, RGB2Pixel
 */
static inline void Pixel2RGB (HDC hdc, gal_pixel pixel,
                Uint8* r, Uint8* g, Uint8* b)
{
    RGB rgb;
    Pixel2RGBs (hdc, &pixel, &rgb, 1);
    *r = rgb.r; *g = rgb.g, *b = rgb.b;
}

/**
 * \fn void GUIAPI Pixel2RGBAs (HDC hdc, const gal_pixel* pixels, RGB* rgbs, int count)
 * \brief Get the colors in RGBA quarter from a array of pixel values under a DC.
 *
 * This function gets the colors in RGBA quarter from the pixel value array \a
 * \a pixels under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixels The array of pixel values.
 * \param rgbs The array returns the converted RGB quarters.
 * \param count The number of pixels.
 *
 * \sa RGB2Pixels
 */
MG_EXPORT void GUIAPI Pixel2RGBAs (HDC hdc, const gal_pixel* pixels,
                RGB* rgbs, int count);

/**
 * \fn void GUIAPI Pixel2RGBA (HDC hdc, gal_pixel pixel, \
                Uint8* r, Uint8* g, Uint8* b, Uint8* a)
 * \brief Get the color in RGBA quarter from a pixel value under a DC.
 *
 * This function gets the color in RGBA quarter from the pixel value \a pixel
 * under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixel The pixel value.
 * \param r The red component of the pixel will be returned through
 *        this pointer.
 * \param g The green component of the pixel will be returned through
 *        this pointer.
 * \param b The blue component of the pixel will be returned through
 *        this pointer.
 * \param a The alpha component of the pixel will be returned through
 *        this pointer.
 *
 * \note This function is implemented as an inline to RGBA2Pixels.
 *
 * \sa RGBA2Pixels, RGB2Pixel
 */
static inline void Pixel2RGBA (HDC hdc, gal_pixel pixel,
                Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    RGB rgb;
    Pixel2RGBAs (hdc, &pixel, &rgb, 1);
    *r = rgb.r; *g = rgb.g, *b = rgb.b; *a = rgb.a;
}

/**
 * \fn DWORD GUIAPI Pixel2DWORD (HDC hdc, gal_pixel pixel)
 * \brief An inline function to convert pixel value to DWORD color.
 *
 * This function converts a pixel value to color in DWORD.
 *
 * \param hdc The device context.
 * \param pixel The pixel value to be converted.
 * \return The converted color value in DWORD.
 */
static inline DWORD Pixel2DWORD (HDC hdc, gal_pixel pixel)
{
    Uint8 r,g,b,a;
    Pixel2RGBA (hdc, pixel, &r, &g, &b, &a);
    return MakeRGBA (r, g, b, a);
}

#define PIXEL2DWORD(hdc, pixel) Pixel2DWORD(hdc, pixel)

/**
 * \fn gal_pixel DWORD2Pixel (HDC hdc, DWORD dword)
 * \brief An inline function to convert DWORD color to gal_pixel.
 *
 * This function converts a color in DWORD to pixel value.
 *
 * \param hdc The device context.
 * \param dword The color value in DWORD.
 * \return The converted pixel value.
 */
static inline gal_pixel DWORD2Pixel (HDC hdc, DWORD dword)
{
    return RGBA2Pixel (hdc,
            GetRValue(dword), GetGValue(dword),
            GetBValue(dword), GetAValue(dword));
}

/**
 * \fn void GUIAPI FocusRect (HDC hdc, int x0, int y0, int x1, int y1)
 * \brief Draws a focus rectangle.
 *
 * This function uses XOR mode to draw the focus rectangle on the device
 * context, i.e. calling this function then calling this function with same
 * arguments again will erase the focus rectangle and restore the pixels
 * before the first call.
 *
 * \param hdc The device context.
 * \param x0 x0,y0: The coordinates of upper-left corner of the rectangle.
 * \param y0 x0,y0: The coordinates of upper-left corner of the rectangle.
 * \param x1 x1,y1: The corrdinates of lower-right corner of the rectangle.
 * \param y1 x1,y1: The corrdinates of lower-right corner of the rectangle.
 *
 * \sa DrawHVDotLine
 */
MG_EXPORT void GUIAPI FocusRect (HDC hdc, int x0, int y0, int x1, int y1);

/**
 * \fn void GUIAPI DrawHVDotLine (HDC hdc, int x, int y, int w_h, BOOL H_V)
 * \brief Draws a horizontal or vertical dot dash line.
 *
 * This function draws a horizontal or vertical dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param w_h The width of the line or horizontal dot line, or the height
 *        if you want to draw a vertical dot line.
 * \param H_V Specify whether you want to draw a horizontal or vertical line.
 *        TRUE for horizontal, FALSE for vertical.
 *
 * \sa FocusRect
 */
MG_EXPORT void GUIAPI DrawHVDotLine (HDC hdc, int x, int y, int w_h, BOOL H_V);

/**
 * \def DrawHDotLine(hdc, x, y, w)
 * \brief Draws a horizontal dot dash line.
 *
 * This function draws a horizontal dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param w The width of the horizontal dot line.
 *
 * \note Defined as a macro calling DrawHVDotLine.
 *
 * \sa DrawVDotLine, DrawHVDotLine
 */
#define DrawHDotLine(hdc, x, y, w) DrawHVDotLine (hdc, x, y, w, TRUE);

/**
 * \def DrawVDotLine(hdc, x, y, h)
 * \brief Draws a vertical dot line.
 *
 * This function draws a vertical dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param h The height of the horizontal dot line.
 *
 * \note Defined as a macro calling DrawHVDotLine.
 *
 * \sa DrawHDotLine, DrawHVDotLine
 */
#define DrawVDotLine(hdc, x, y, h) DrawHVDotLine (hdc, x, y, h, FALSE);

/**
 * \fn BOOL GUIAPI LineClipper (const RECT* cliprc, \
                int *_x0, int *_y0, int *_x1, int *_y1)
 * \brief The line clipper using Cohen-Sutherland algorithm.
 *
 * This function clips a line from \a (*_x0,*_y0) to \a (*_x1, *_y1)
 * with the specified clipping rectangle pointed to by \a cliprc.
 * Then return the clipped line throught the pointers \a (_x0, _y0, _x1, _y1).
 *
 * It is modified to do pixel-perfect clipping. This means that it
 * will generate the same endpoints that would be drawn if an ordinary
 * Bresenham line-drawer where used and only visible pixels drawn.
 *
 * \param cliprc The Pointer to the clipping rectangle.
 * \param _x0 _x0,_y0: The pointers contains the start point of the line.
 *        The start point after clipping will be returned through them
 *        as well as.
 * \param _y0 _x0,_y0: The pointers contains the start point of the line.
 *        The start point after clipping will be returned through them
 *        as well as.
 * \param _x1 _x1,_y1: The pointers contains the end point of the line.
 *        The end point after clipping will be returned through them as well as.
 * \param _y1 _x1,_y1: The pointers contains the end point of the line.
 *        The end point after clipping will be returned through them as well as.
 *
 * \return TRUE for clipped by the clipping rectangle, FALSE for not clipped.
 *
 * \sa LineGenerator
 */
MG_EXPORT BOOL GUIAPI LineClipper (const RECT* cliprc,
                int *_x0, int *_y0, int *_x1, int *_y1);

/**
 * \var typedef void (* CB_LINE) (void* context, int stepx, int stepy)
 * \brief The type of line generator callback.
 */
typedef void (* CB_LINE) (void* context, int stepx, int stepy);

/**
 * \fn void GUIAPI LineGenerator (void* context, \
                int x1, int y1, int x2, int y2, CB_LINE cb)
 * \brief A line generator based-on Breshenham algorithm.
 *
 * This is a Breshenham line generator. When it generates a new point on the
 * line, it will call the callback \a cb and pass the context \a context and
 * the step values on x-coordinate and y-coodinate since the last point.
 * The first generated point always be the start point you passed to this
 * function. Thus, the first step values passed to \a cb will be (0,0).
 *
 * MiniGUI implements \a LineTo function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param x1 x1,y1: The start point of the line.
 * \param y1 x1,y1: The start point of the line.
 * \param x2 x2,y2: The end point of the line.
 * \param y2 x2,y2: The end point of the line.
 * \param cb The line generator callback.
 *
 * \sa LineTo, LineClipper, CB_LINE
 */
MG_EXPORT void GUIAPI LineGenerator (void* context,
                int x1, int y1, int x2, int y2, CB_LINE cb);

/**
 * \var typedef void (* CB_CIRCLE) (void* context, int x1, int x2, int y)
 * \brief The type of circle generator callback.
 */
typedef void (* CB_CIRCLE) (void* context, int x1, int x2, int y);

/**
 * \fn void GUIAPI CircleGenerator (void* context, \
                int sx, int sy, int r, CB_CIRCLE cb)
 * \brief A circle generator.
 *
 * This is a general circle generator, it will generate two points on the
 * same horizontal scan line at every turn. When it generates the points on
 * the circle, it will call the callback \a cb and pass the context \a context,
 * the y-coordinate of the scan line, the x-coordinate of the left point on
 * the scan line, and the x-coordinate of the right point. Note that in some
 * cases, the left point and the right point will be the same.
 *
 * MiniGUI implements \a Circle, \a FillCircle and \a InitCircleRegion
 * functions by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 * \param cb The circle generator callback.
 *
 * \sa Circle, FillCircle, InitCircleRegion, CB_CIRCLE
 *
 * Example:
 *
 * \include circlegenerator.c
 */
MG_EXPORT void GUIAPI CircleGenerator (void* context,
                int sx, int sy, int r, CB_CIRCLE cb);

/**
 * \var typedef void (* CB_ELLIPSE) (void* context, int x1, int x2, int y)
 * \brief The type of ellipse generator callback.
 */
typedef void (* CB_ELLIPSE) (void* context, int x1, int x2, int y);

/**
 * \fn void GUIAPI EllipseGenerator (void* context, \
                int sx, int sy, int rx, int ry, CB_ELLIPSE cb)
 * \brief An ellipse generator.
 *
 * This is a general ellipse generator, it will generate two points on the same
 * horizontal scan line at every turn. When it generates the points on the
 * ellipse, it will call the callback \a cb and pass the context \a context,
 * the y-coordinate of the scan line, the x-coordinate of the left point on
 * the scan line, and the x-coordinate of the right point. Note that in some
 * cases, the left point and the right point will be the same.
 *
 * MiniGUI implements \a Ellipse, \a FillEllipse, and \a InitEllipseRegion
 * functions by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the ellipse.
 * \param sy sx,sy: The center of the ellipse.
 * \param rx The x-radius of the ellipse.
 * \param ry The y-radius of the ellipse.
 * \param cb The ellipse generator callback.
 *
 * \sa Ellipse, FillEllipse, InitEllipseRegion, CB_CIRCLE
 */
MG_EXPORT void GUIAPI EllipseGenerator (void* context,
                int sx, int sy, int rx, int ry, CB_ELLIPSE cb);

/**
 * \var typedef void (* CB_ARC) (void* context, int x, int y)
 * \brief The type of arc generator callback.
 */
typedef void (* CB_ARC) (void* context, int x, int y);

/**
 * \fn void GUIAPI CircleArcGenerator (void* context, \
                int sx, int sy, int r, int ang1, int ang2, CB_ARC cb)
 * \brief An arc generator.
 *
 * This is a general arc generator. When it generates a point on the arc,
 * it will call the callback \a cb and pass the context \a context, the
 * coordinates of the point.
 *
 * MiniGUI implements \a CircleArc function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the arc.
 * \param sy sx,sy: The center of the arc.
 * \param r The radius of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position,
 *        counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1, in 1/64ths of
 *        a degree.
 * \param cb The arc generator callback.
 *
 * \sa CircleArc, CB_ARC
 */
MG_EXPORT void GUIAPI CircleArcGenerator (void* context,
                int sx, int sy, int r, int ang1, int ang2, CB_ARC cb);

/**
 * \var typedef void (* CB_POLYGON) (void* context, int x1, int x2, int y)
 * \brief The type of polygon generator callback.
 */
typedef void (* CB_POLYGON) (void* context, int x1, int x2, int y);

/**
 * \fn BOOL GUIAPI MonotoneVerticalPolygonGenerator (void* context, \
                const POINT* pts, int vertices, CB_POLYGON cb)
 * \brief A monotone vertical polygon generator.
 *
 * This is a monotone vertical polygon generator.
 *
 * "Monoton vertical" means "monotone with respect to a vertical line";
 * that is, every horizontal line drawn through the polygon at any point
 * would cross exactly two active edges (neither horizontal lines
 * nor zero-length edges count as active edges; both are acceptable
 * anywhere in the polygon). Right & left edges may cross (polygons may
 * be nonsimple).  Polygons that are not convex according to this definition
 * won't be drawn properly.
 *
 * You can call \a PolygonIsMonotoneVertical function to check one polygon is
 * monotone vertical or not.
 *
 * This function will generate two points on the same horizontal scan line at
 * every turn.  When it generates the points on the polygon, it will call
 * the callback \a cb and pass the context \a context, the y-coordinate of
 * the scan line, the x-coordinate of the left point on the scan line, and
 * the x-coordinate of the right point. Note that in some cases, the left
 * point and the right point will be the same.
 *
 * MiniGUI implements \a FillPolygon and \a InitPolygonRegion functions
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex
 *        array.
 * \param cb The polygon generator callback.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa FillPolygon, InitPolygonRegion, PolygonIsMonotoneVertical, CB_CIRCLE
 */
MG_EXPORT BOOL GUIAPI MonotoneVerticalPolygonGenerator (void* context,
                const POINT* pts, int vertices, CB_POLYGON cb);

/**
 * \fn BOOL GUIAPI PolygonIsMonotoneVertical (const POINT* pts, int vertices)
 * \brief Check a polygon is monotone vertical or not.
 *
 * This function checks if the given polygon is monotone vertical.
 *
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex
 *        array.
 *
 * \return TRUE if it is monotonoe vertical, otherwise FALSE.
 *
 * \note To make sure a polygon is monotone vertical, the polygon must be a
 *       closed one, that is, the pts[0] must be equal to the pts[vertices-1].
 *
 * \sa MonotoneVerticalPolygonGenerator
 */
MG_EXPORT BOOL GUIAPI PolygonIsMonotoneVertical (const POINT* pts,
                int vertices);

/**
 * \fn BOOL GUIAPI PolygonGeneratorEx (void* context, \
                const POINT* pts, int vertices, CB_POLYGON cb, RECT* rc_output)
 * \brief A general polygon generator.
 *
 * This is a general polygon generator.
 *
 * This function will generate two points on the same horizontal scan line
 * at every turn.  When it generates the points on the polygon, it will
 * call the callback \a cb and pass the context \a context, the y-coordinate
 * of the scan line, the x-coordinate of the left point on the scan line,
 * and the x-coordinate of the right point. Note that in some cases, the
 * left point and the right point will be the same.
 *
 * MiniGUI implements \a FillPolygon and \a InitPolygonRegion functions
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the
 *        vertex array.
 * \param cb The polygon generator callback.
 * \param rc_output The limit RECT for Polygon ScanLines ranges .
 *
 * \sa FillPolygon, InitPolygonRegion, CB_CIRCLE
 */
MG_EXPORT BOOL GUIAPI PolygonGeneratorEx (void* context,
                const POINT* pts, int vertices, CB_POLYGON cb, RECT* rc_output);
/**
 * \def PolygonGenerator(context, pts, vertices, cb)
 * \brief This is a general polygon generator.
 *
 * Defined as a macro calling \a PolygonGeneratorEx passing
 * and \a rc_output as NULL.
 *
 * \sa PolygonGeneratorEx
 */
#define PolygonGenerator(context, pts, vertices, cb)          \
                PolygonGeneratorEx(context, pts, vertices, cb, NULL)

/**
 * \var typedef BOOL (* CB_EQUAL_PIXEL) (void* context, int x, int y)
 * \brief Flodd filling generator's equation callback.
 */
typedef BOOL (* CB_EQUAL_PIXEL) (void* context, int x, int y);

/**
 * \var typedef void (* CB_FLOOD_FILL) (void* context, int x1, int x2, int y)
 * \brief Flodd filling generator's scan line callback.
 */
typedef void (* CB_FLOOD_FILL) (void* context, int x1, int x2, int y);

/**
 * \fn BOOL GUIAPI FloodFillGenerator (void* context, \
            const RECT* src_rc, int x, int y, \
            CB_EQUAL_PIXEL cb_equal_pixel, CB_FLOOD_FILL cb_flood_fill)
 * \brief A flood filling generator.
 *
 * This function is a general flood filling generator.
 *
 * MiniGUI implements \a FloodFill function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param src_rc The filling bounding rectangle.
 * \param x x,y: The start filling point.
 * \param y x,y: The start filling point.
 * \param cb_equal_pixel The callback to check the pixel is equal with
 *        the start point or not.
 * \param cb_flood_fill The callback to fill a scan line.
 *
 * \sa FloodFill, CB_EQUAL_PIXEL, CB_FLOOD_FILL
 */
MG_EXPORT BOOL GUIAPI FloodFillGenerator (void* context,
                const RECT* src_rc, int x, int y,
                CB_EQUAL_PIXEL cb_equal_pixel, CB_FLOOD_FILL cb_flood_fill);

/* DDA algorithm */
#define BITMAP_SCALER_DDA       0
/* Bilinear interpolation algorithm */
#define BITMAP_SCALER_BILINEAR  1

/**
 * \fn int GUIAPI SetBitmapScalerType (HDC hdc, int scaler_type);
 * \brief Set bitmap scaler algorithm callback of DC according by scaler_type.
 *
 * This function sets the bitmap scaler with DDA or bilinear interpolation
 * algorithm. MiniGUI implements \a StretchBlt functions by using this scaler
 * (when not using Pixman).
 *
 * \param hdc The device context.
 * \param scaler_type The type of scaler algorithm, use BITMAP_SCALER_DDA
 *        or BITMAP_SCALER_BILINEAR.
 *
 * \sa BitmapDDAScaler, BitmapBinearScaler
 */

MG_EXPORT int GUIAPI SetBitmapScalerType (HDC hdc, int scaler_type);

/**
 * \fn void GUIAPI MoveTo (HDC hdc, int x, int y)
 * \brief Moves the current zero pen position.
 *
 * This function moves the current pen position of the DC \a hdc to \a (x,y).
 * The next line or other pen drawing operation will start from the position.
 *
 * \param hdc The device context.
 * \param x x,y: The new pen position.
 * \param y x,y: The new pen position.
 *
 * \sa LineTo
 */
MG_EXPORT void GUIAPI MoveTo (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI LineTo (HDC hdc, int x, int y)
 * \brief Draws a zero line to a position.
 *
 * This function draws a line from the current zero pen position to \a (x,y),
 * and then moves the zero pen position to \a (x,y) by using the zero pen.
 * The next zero line drawing operation will start from the position.
 *
 * \param hdc The device context.
 * \param x x,y: The end point of the line.
 * \param y x,y: The end point of the line.
 *
 * \sa MoveTo, LineGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI LineTo (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1)
 * \brief Draws a rectangle.
 *
 * This function draws a rectangle by using \a MoveTo and \a LineTo.
 * The current zero pen position after calling this function will be \a (x1,y1).
 *
 * \param hdc The device context.
 * \param x0 x0,y0: The coordinates of the upper-left corner of the rectangle.
 * \param y0 x0,y0: The coordinates of the upper-left corner of the rectangle.
 * \param x1 x1,y1: The coordinates of the lower-right corner of the rectangle.
 * \param y1 x1,y1: The coordinates of the lower-right corner of the rectangle.
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1);

/**
 * \fn void GUIAPI PolyLineTo (HDC hdc, const POINT* pts, int vertices)
 * \brief Draws a polyline.
 *
 * This function draws a polyline by using \a MoveTo and \a LineTo.
 * The current zero pen position after calling this function will be the
 * last vertex of the polyline.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polyline.
 * \param vertices The number of the vertices.
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI PolyLineTo (HDC hdc, const POINT* pts, int vertices);

/**
 * \fn void GUIAPI SplineTo (HDC hdc, const POINT* pts)
 * \brief Draws a bezier spline.
 *
 * This function draws a bezier spline by using \a MoveTo and \a LineTo.
 * The current pen position after calling this function will be the
 * last control point of the spline. Note that the number of the
 * control points should always be 4.
 *
 * \param hdc The device context.
 * \param pts The pointer to the control point array of the spline.
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI SplineTo (HDC hdc, const POINT* pts);

/**
 * \fn void GUIAPI Circle (HDC hdc, int sx, int sy, int r)
 * \brief Draws a circle.
 *
 * This function draws a circle with the zero pen on the DC \a hdc.
 * The center of the circle is at \a (sx, sy), and the radius is \a r.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 *
 * \sa CircleGenerator
 */
MG_EXPORT void GUIAPI Circle (HDC hdc, int sx, int sy, int r);

/**
 * \fn void GUIAPI Ellipse (HDC hdc, int sx, int sy, int rx, int ry)
 * \brief Draws a ellipse.
 *
 * This function draws an ellipse with the zero pen on the DC \a hdc.
 * The center of the ellipse is at \a (sx, sy), the x-coordinate radius is
 * \a rx, and the y-coordinate radius is \a ry.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the ellipse.
 * \param sy sx,sy: The center of the ellipse.
 * \param rx The x-coordinate radius of the ellipse.
 * \param ry The y-coordinate radius of the ellipse.
 *
 * \sa EllipseGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI Ellipse (HDC hdc, int sx, int sy, int rx, int ry);

/**
 * \fn void GUIAPI CircleArc (HDC hdc, int sx, int sy, int r, \
                int ang1, int ang2)
 * \brief Draws an arc.
 *
 * This function draws an arc with the zero pen on the DC \a hdc.
 * The center of the arc is at \a (sx, sy), the radius is \a r,
 * and the radians of start angle and end angle are \a ang1 and \a ang2
 * respectively.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the arc.
 * \param sy sx,sy: The center of the arc.
 * \param r The radius of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position,
 *        counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1,
 *        in 1/64ths of a degree.
 *
 * \sa CircleArcGenerator, fixed_math_fns, dc_attrs
 */
MG_EXPORT void GUIAPI CircleArc (HDC hdc, int sx, int sy, int r,
                int ang1, int ang2);

/**
 * \fn void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h)
 * \brief Fills a rectangle box.
 *
 * This function fills a box with the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The coorinates of the upper-left corner of the box.
 * \param y x,y: The coorinates of the upper-left corner of the box.
 * \param w The width of the box.
 * \param h The height of the box.
 *
 * \sa dc_attrs
 */
MG_EXPORT void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h);

/**
 * \fn void GUIAPI FillCircle (HDC hdc, int sx, int sy, int r)
 * \brief Fills a circle.
 *
 * This function fills a circle with the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 *
 * \sa CircleGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI FillCircle (HDC hdc, int sx, int sy, int r);

/**
 * \fn void GUIAPI FillEllipse (HDC hdc, int sx, int sy, int rx, int ry)
 * \brief Fills an ellipse.
 *
 * This function fills an ellipse with the current brush in the DC \a hdc.
 * Note that MiniGUI only defined the color property for the brush objects
 * so far.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param rx The x-coordinate radius of the ellipse.
 * \param ry The y-coordinate radius of the ellipse.
 *
 * \sa EllipseGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI FillEllipse (HDC hdc, int sx, int sy, int rx, int ry);

/**
 * \fn BOOL GUIAPI FillPolygon (HDC hdc, const POINT* pts, int vertices)
 * \brief Fills an polygon.
 *
 * This function fills a polygon with the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the
 *        vertex array.
 *
 * \sa PolygonGenerator, dc_attrs
 */
MG_EXPORT BOOL GUIAPI FillPolygon (HDC hdc, const POINT* pts, int vertices);

/**
 * \fn BOOL GUIAPI FloodFill (HDC hdc, int x, int y)
 * \brief Fills an enclosed area starting at point \a (x,y).
 *
 * This function fills an enclosed area staring at point \a (x,y), and
 * stops when encountering a pixel different from the start point.
 *
 * \param hdc The device context.
 * \param x x,y: The start point.
 * \param y x,y: The start point.
 *
 * \sa FloodFillGenerator, dc_attrs
 */
MG_EXPORT BOOL GUIAPI FloodFill (HDC hdc, int x, int y);

#ifdef _MGHAVE_ADV_2DAPI

    /**
     * \addtogroup draw_adv_2d_fns Advanced 2-Dimension graphics functions.
     * @{
     */

/**
 * DC Line styles
 */
typedef enum
{
  /**
   * The solid pen.
   * We call the solid pen with zero wide as "zero pen".
   */
  PT_SOLID,
  /**
   * The on/off dash pen, even segments are drawn;
   * odd segments are not drawn.
   */
  PT_ON_OFF_DASH,
  /**
   * The double dash pen, even segments are normally.
   * Odd segments are drawn in the brush color if
   * the brush type is BT_SOLID, or in the brush color
   * masked by the stipple if the brush type is BT_STIPPLED.
   */
  PT_DOUBLE_DASH,
} PenType;

/**
 * \def GetPenType(hdc)
 * \brief Get the pen type of a DC.
 *
 * \param hdc The device context.
 * \return The pen type of the DC \a hdc.
 *
 * \sa PenType, GetDCAttr, SetPenType
 */
#define GetPenType(hdc)                 \
                GetDCAttr (hdc, DC_ATTR_PEN_TYPE)

/**
 * \def SetPenType(hdc, type)
 * \brief Set the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param type The new pen type.
 * \return The old pen type of the DC \a hdc.
 *
 * \sa PenType, SetDCAttr, GetPenType
 */
#define SetPenType(hdc, type)           \
                SetDCAttr (hdc, DC_ATTR_PEN_TYPE, (DWORD) type)

/**
 * \fn void SetPenDashes (HDC hdc, int dash_offset, \
                const unsigned char* dash_list, int n)
 * \brief Set the way dashed-lines are drawn.
 *
 * Set the way dashed-lines are drawn. Lines will be drawn with
 * alternating on and off segments of the lengths specified in dash_list.
 * The manner in which the on and off segments are drawn is determined by
 * the pen type of the DC.  (This can be changed with SetPenType function.)
 *
 * \param hdc The device context.
 * \param dash_offset The offset in the dash list.
 * \param dash_list The dash list.
 * \param n The lenght of the dash list.
 *
 * \return The old pen type of the DC \a hdc.
 *
 * \sa SetPenType
 */
MG_EXPORT void GUIAPI SetPenDashes (HDC hdc, int dash_offset,
                const unsigned char* dash_list, int n);

/**
 * DC Line cap styles
 */
typedef enum
{
  /**
   * The ends of the lines are drawn squared off
   * and extending to the coordinates of the end point.
   */
  PT_CAP_BUTT,
  /**
   * The ends of the lines are drawn as semicircles with
   * the diameter equal to the line width and centered at the end point.
   */
  PT_CAP_ROUND,
  /**
   * The ends of the lines are drawn squared off and
   * extending half the width of the line beyond the end point.
   */
  PT_CAP_PROJECTING
} PTCapStyle;

/**
 * \def GetPenCapStyle(hdc)
 * \brief Get the pen cap style of a DC.
 *
 * \param hdc The device context.
 * \return The pen cap style of the DC \a hdc.
 *
 * \sa PTCapStyle, GetDCAttr, SetPenCapStyle
 */
#define GetPenCapStyle(hdc)                 \
                GetDCAttr (hdc, DC_ATTR_PEN_CAP_STYLE)

/**
 * \def SetPenCapStyle(hdc, style)
 * \brief Set the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param style The new pen cap style.
 * \return The old pen cap style of the DC \a hdc.
 *
 * \sa PTCapStyle, SetDCAttr, GetPenCapStyle
 */
#define SetPenCapStyle(hdc, style)          \
            SetDCAttr (hdc, DC_ATTR_PEN_CAP_STYLE, (DWORD) style)

/**
 * DC Line join styles
 */
typedef enum
{
  /**
   * The sides of each line are extended to meet at an angle.
   */
  PT_JOIN_MITER,
  /**
   * The sides of the two lines are joined by a circular arc.
   */
  PT_JOIN_ROUND,
  /**
   * The sides of the two lines are joined by a straight line
   * which makes an equal angle with each line.
   */
  PT_JOIN_BEVEL
} PTJoinStyle;

/**
 * \def GetPenJoinStyle(hdc)
 * \brief Get the pen join style of a DC.
 *
 * \param hdc The device context.
 * \return The pen join style of the DC \a hdc.
 *
 * \sa PTJoinStyle, GetDCAttr, SetPenJoinStyle
 */
#define GetPenJoinStyle(hdc)           GetDCAttr (hdc, DC_ATTR_PEN_JOIN_STYLE)

/**
 * \def SetPenJoinStyle(hdc, style)
 * \brief Set the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param style The new pen join style.
 * \return The old pen join style of the DC \a hdc.
 *
 * \sa PTJoinStyle, SetDCAttr, GetPenJoinStyle
 */
#define SetPenJoinStyle(hdc, style)         \
            SetDCAttr (hdc, DC_ATTR_PEN_JOIN_STYLE, (DWORD) style)

/**
 * \def GetPenWidth(hdc)
 * \brief Get the pen width of a DC.
 *
 * \param hdc The device context.
 * \return The width of the current pen in the DC \a hdc.
 *
 * \sa GetDCAttr, SetPenWidth
 */
#define GetPenWidth(hdc)                    \
                GetDCAttr (hdc, DC_ATTR_PEN_WIDTH)

/**
 * \def SetPenWidth(hdc, width)
 * \brief Set the pen width of a DC to a new width.
 *
 * \param hdc The device context.
 * \param width The new pen width.
 * \return The old pen width of the DC \a hdc.
 *
 * \sa SetDCAttr, GetPenWidth
 */
#define SetPenWidth(hdc, width)             \
                SetDCAttr (hdc, DC_ATTR_PEN_WIDTH, (DWORD) width)

/**
 * DC brush types.
 *  Solid:
 *  Tiled:
 *  Stippled:
 *  OpaqueStippled:
 */
typedef enum {
  /**
   * Draw with the current brush color.
   */
  BT_SOLID,
  /**
   * Draw with a tiled bitmap.
   */
  BT_TILED,
  /**
   * Draw using the stipple bitmap.
   * Pixels corresponding to bits in the stipple bitmap that are set
   * will be drawn in the brush color; pixels corresponding to bits
   * that are not set will be left untouched.
   */
  BT_STIPPLED,
  /**
   * Draw using the stipple bitmap.
   * Pixels corresponding to bits in the stipple bitmap that are set
   * will be drawn in the brush color; pixels corresponding to bits
   * that are not set will be drawn with the background color.
   */
  BT_OPAQUE_STIPPLED
} BrushType;

/**
 * \def GetBrushType(hdc)
 * \brief Get the brush type of a DC.
 *
 * \param hdc The device context.
 * \return The brush type of the DC \a hdc.
 *
 * \sa BrushType, GetDCAttr, SetBrushType
 */
#define GetBrushType(hdc)               \
                GetDCAttr (hdc, DC_ATTR_BRUSH_TYPE)

/**
 * \def SetBrushType(hdc, type)
 * \brief Set the brush type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param type The new brush type.
 * \return The old brush type of the DC \a hdc.
 *
 * \sa BrushType, SetDCAttr, GetBrushType
 */
#define SetBrushType(hdc, type)         \
                SetDCAttr (hdc, DC_ATTR_BRUSH_TYPE, (DWORD) type)

/** The stipple bitmap structure. */
typedef struct _STIPPLE
{
    /** The width of the stipple bitmap. */
    int width;
    /** The height of the stipple bitmap. */
    int height;

    /** The pitch of the stipple bitmap. */
    int pitch;

    /** The size of the stipple bitmap. */
    size_t size;

    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;
} STIPPLE;

/**
 * \fn void GUIAPI SetBrushInfo (HDC hdc, \
                const BITMAP* tile, const STIPPLE* stipple)
 * \brief Set the tile or stipple with the DC.
 *
 * \param hdc The device context.
 * \param tile The tile bitmap.
 * \param stipple The stipple bitmap.
 *
 * \sa SetBrushInfo
 */
MG_EXPORT void GUIAPI SetBrushInfo (HDC hdc,
                const BITMAP* tile, const STIPPLE* stipple);

/**
 * \fn void GUIAPI SetBrushOrigin (HDC hdc, int x, int y)
 * \brief Set the origin when using tiles or stipples with the DC.
 *
 * Set the origin when using tiles or stipples with the DC.
 * The tile or stipple will be aligned such that the upper left corner
 * of the tile or stipple will coincide with this point.
 *
 * \param hdc The device context.
 * \param x The x-coordinate of the origin.
 * \param y The y-coordinate of the origin.
 *
 * \sa SetBrushInfo
 */
MG_EXPORT void GUIAPI SetBrushOrigin (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI LineEx (HDC hdc, int x1, int y1, int x2, int y2)
 * \brief Draws a line with the current pen in the DC \a hdc.
 *
 * This function draws a line with the current pen in the DC \a hdc.
 * The line draws from \a (x1, y1) to \a (x2, y2).
 *
 * \param hdc The device context.
 * \param x1 x1,y1: The start point of the line.
 * \param y1 x1,y1: The start point of the line.
 * \param x2 x2,y2: The end point of the line.
 * \param y2 x2,y2: The end point of the line.
 *
 * \note This function does not change the pen position.
 *
 * \sa LineTo, dc_attrs
 */
MG_EXPORT void GUIAPI LineEx (HDC hdc, int x1, int y1, int x2, int y2);

/**
 * \fn void GUIAPI ArcEx (HDC hdc, int x, int y, int width, int height, \
                int ang1, int ang2)
 * \brief Draws an arc with the current pen in a DC.
 *
 * This function draws an arc with the current pen in the DC \a hdc.
 * The coordinates of the upper-left corner of the bounding rectanglecenter of
 * the arc is \a (x, y), the bounding box of the arc is \a width wide
 * and \a height high, and the degree of start angle and end angle are \a ang1
 * and \a ang2 respectively.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the left edge of the bounding rectangle.
 * \param y The y coordinate of the left edge of the bounding rectangle.
 * \param width The width of the bounding box of the arc.
 * \param height The height of the bounding box of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position,
 *        counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1,
 *        in 1/64ths of a degree.
 *
 * \sa dc_attrs
 */
MG_EXPORT void GUIAPI ArcEx (HDC hdc, int x, int y, int width, int height,
                int ang1, int ang2);

/**
 * \fn void GUIAPI FillArcEx (HDC hdc, int x, int y, int width, int height, \
                int ang1, int ang2)
 * \brief Fills an arc with the current brush in a DC.
 *
 * This function fills an arc with the current brush in the DC \a hdc.
 * The coordinates of the upper-left corner of the bounding rectanglecenter of
 * the arc is \a (x, y), the bounding box of the arc is \a width wide
 * and \a height high, and the degree of start angle and end angle are \a ang1
 * and \a ang2 respectively.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the left edge of the bounding rectangle.
 * \param y The y coordinate of the left edge of the bounding rectangle.
 * \param width The width of the bounding box of the arc.
 * \param height The height of the bounding box of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position,
 *        counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1,
 *        in 1/64ths of a degree.
 *
 * \sa ArcEx, dc_attrs
 */
MG_EXPORT void GUIAPI FillArcEx (HDC hdc, int x, int y, int width, int height,
                int ang1, int ang2);

/**
 * \fn void GUIAPI PolyLineEx (HDC hdc, const POINT* pts, int nr_pts)
 * \brief Draws a polyline with the current pen in a DC.
 *
 * This function draws a polyline with the current pen in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polyline.
 * \param nr_pts The number of the vertices.
 *
 * \sa LineEx
 */
MG_EXPORT void GUIAPI PolyLineEx (HDC hdc, const POINT* pts, int nr_pts);

/**
 * Arc structure.
 */
typedef struct _ARC
{
    /** The x coordinate of the left edge of the bounding rectangle. */
    int x;
    /** The y coordinate of the left edge of the bounding rectangle. */
    int y;

    /** The width of the bounding box of the arc. */
    int width;
    /** The height of the bounding box of the arc. */
    int height;

    /**
     * The start angle of the arc, relative to the 3 o'clock position,
     * counter-clockwise, in 1/64ths of a degree.
     */
    int angle1;
    /**
     * The end angle of the arc, relative to angle1, in 1/64ths of a degree.
     */
    int angle2;
} ARC;

/**
 * \fn void GUIAPI PolyArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
 * \brief Draws a polyarc with the current pen in a DC.
 *
 * This function draws a polyarc with the current pen in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param arcs The pointer to an ARC array which defines the arcs.
 * \param nr_arcs The number of the arcs.
 *
 * \sa ArcEx
 */
MG_EXPORT void GUIAPI PolyArcEx (HDC hdc, const ARC* arcs, int nr_arcs);

/**
 * \fn void GUIAPI PolyFillArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
 * \brief Fill a polyarc with the current brush in a DC.
 *
 * This function fill a polyarc with the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param arcs The pointer to an ARC array which defines the arcs.
 * \param nr_arcs The number of the arcs.
 *
 * \sa ArcEx
 */
MG_EXPORT void GUIAPI PolyFillArcEx (HDC hdc, const ARC* arcs, int nr_arcs);

/**
 * \fn BOOL GUIAPI RoundRect (HDC hdc, int x0, int y0, int x1, int y1, int rw, int rh)
 * \brief Draw and fill a rectangle with rounded corners in a DC.
 *
 * This function draws a rectangle with rounded corners. The rectangle is outlined by using
 * the current pen and filled by using the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x0 The x-coordinate of the upper-left corner of the rectangle.
 * \param y0 The y-coordinate of the upper-left corner of the rectangle.
 * \param x1 The x-coordinate of the lower-right corner of the rectangle.
 * \param y1 The y-coordinate of the lower-right corner of the rectangle.
 * \param rw The x-radius of the rounded corners.
 * \param rh The y-radius of the rounded corners.
 * \return TRUE for visible, else for not.
 *
 * \sa ArcEx, LineEx, FillArcEx, FillBox
 */
MG_EXPORT BOOL GUIAPI RoundRect (HDC hdc, int x0, int y0, int x1, int y1, int cw, int ch);

    /**
     * @} end of draw_adv_2d_fns
     */
#endif

    /** @} end of draw_fns */

    /**
     * \defgroup map_fns Mapping Operations
     *
     * The mapping mode defines the unit of measure used to transform
     * page-space units into device-space units, and also defines
     * the orientation of the device's x and y axes.
     *
     * So far, MiniGUI support only two mapping mode:
     *
     *      - MM_TEXT\n
     *        Each logical unit is mapped to on device pixel.
     *        Positive x is to the right; positive y is down.
     *      - MM_ANISOTROPIC\n
     *        Logical units are mapped to arbitrary units with arbitrarily
     *        scaled axes; Use \a SetWindowExt and \a SetViewportExt functions
     *        to specify the units, orientation, and scaling required.
     *
     * The following formula shows the math involved in converting a point
     * from page space to device space:
     *
     * \code
     *      Dx = ((Lx - WOx) * VEx / WEx) + VOx
     * \endcode
     *
     * The following variables are involved:
     *
     *      - Dx    x value in device units
     *      - Lx    x value in logical units (also known as page space units)
     *      - WO    window x origin
     *      - VO    viewport x origin
     *      - WE    window x-extent
     *      - VE    viewport x-extent
     *
     * The same equation with y replacing x transforms the y component of
     * a point. The formula first offsets the point from its coordinate origin.
     * This value, no longer biased by the  origin, is then scaled into
     * the destination coordinate system by the ratio of the extents.
     * Finally, the scaled value is offset by the destination origin to
     * its final mapping.
     *
     * @{
     */

#define MM_TEXT               0
#define MM_ANISOTROPIC        1

/**
 * \def GetMapMode(hdc)
 * \brief Retrieve the current mapping mode of a DC.
 *
 * This function retrieves the current mapping mode of the DC \a hdc.
 *
 * So far, MiniGUI support two mapping modes: MM_TEXT and MM_ANISOTROPIC.
 *
 * \param hdc The device context.
 *
 * \return The current mapping mode, can be either \a MM_TEXT or
 *         \a MM_ANISOTROPIC.
 *
 * \sa SetWindowExt, SetViewportExt, SetMapMode
 */
#define GetMapMode(hdc)         GetDCAttr (hdc, DC_ATTR_MAP_MODE)

/**
 * \def SetMapMode(hdc, mapmode)
 * \brief Set the mapping mode of a display context.
 *
 * This function sets the mapping mode of the specified display context \a hdc.
 *
 * So far, MiniGUI support two mapping modes: MM_TEXT and MM_ANISOTROPIC.
 *
 * \param hdc The device context.
 * \param mapmode The new mapping mode, should be either \a MM_TEXT or
 *        \a MM_ANISOTROPIC.
 * \return The old mapping mode, either \a MM_TEXT or \a MM_ANISOTROPIC.
 *
 * \sa SetWindowExt, SetViewportExt, SetMapMode
 */
#define SetMapMode(hdc, mapmode)        \
                SetDCAttr (hdc, DC_ATTR_MAP_MODE, (DWORD)mapmode)

/**
 * \fn void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt)
 * \brief Retrieve mapping parameters of a device context.
 *
 * This function retrieves mapping paramters of the specified device context
 * \a hdc when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrieve, can be one of
 *        the following values:
 *
 *      - DC_LCS_VORG\n
 *        Retrieve the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Retrieve the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Retrieve the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Retrieve the x-extents and y-extents of the window.
 *
 * \param pt The coordinates or extents will be returned through this buffer.
 *
 * \sa GetMapMode, SetMapMode, SetDCLCS
 */
MG_EXPORT void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt);

/**
 * \fn void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt)
 * \brief Set mapping parameters of a device context.
 *
 * This function sets mapping paramters of the specified device context \a hdc
 * when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrieve, can be one of
 *        the following values:
 *
 *      - DC_LCS_VORG\n
 *        Set the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Set the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Set the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Set the x-extents and y-extents of the window.
 *
 * \param pt The coordinates or extents will be set.
 *
 * \sa GetMapMode, SetMapMode, GetDCLCS
 */
MG_EXPORT void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt);

#define DC_LCS_VORG     0
#define DC_LCS_VEXT     1
#define DC_LCS_WORG     2
#define DC_LCS_WEXT     3
#define NR_DC_LCS_PTS   4

/**
 * \def GetViewportOrg(hdc, pPt)
 * \brief Retrieve the x-coordinates and y-coordinates of the viewport
 *        origin for a device context.
 *
 * This function retrieves the x-coordinates and y-coordinates of
 * the viewport origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport origin will be returned through this buffer.
 *
 * \sa GetMapMode, SetViewportOrg
 */
#define GetViewportOrg(hdc, pPt)    GetDCLCS(hdc, DC_LCS_VORG, pPt)

/**
 * \def GetViewportExt(hdc, pPt)
 * \brief Retrieve the x-extents and y-extents of the current viewport for
 *        a device context.
 *
 * This function retrieves the x-extents and y-extens of the current viewport of
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport extents will be returned through this buffer.
 *
 * \sa GetMapMode, SetViewportExt
 */
#define GetViewportExt(hdc, pPt)    GetDCLCS(hdc, DC_LCS_VEXT, pPt)

/**
 * \def GetWindowOrg(hdc, pPt)
 * \brief Retrieve the x-coordinates and y-coordinates of the window for
 *        a device context.
 *
 * This function retrieves the x-coordinates and y-coordinates of
 * the window origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window origin will be returned through this buffer.
 *
 * \sa GetMapMode, SetWindowOrg
 */
#define GetWindowOrg(hdc, pPt)      GetDCLCS(hdc, DC_LCS_WORG, pPt)

/**
 * \def GetWindowExt(hdc, pPt)
 * \brief Retrieve the x-extents and y-extents of the current window for
 *        a device context.
 *
 * This function retrieves the x-extents and y-extens of the current window of
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window extents will be returned through this buffer.
 *
 * \sa GetMapMode, SetWindowExt
 */
#define GetWindowExt(hdc, pPt)      GetDCLCS(hdc, DC_LCS_WEXT, pPt)

/**
 * \def SetViewportOrg(hdc, pPt)
 * \brief Set the x-coordinates and y-coordinates of the viewport origin for
 *        a device context.
 *
 * This function sets the x-coordinates and y-coordinates of
 * the viewport origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport origin will be set.
 *
 * \sa GetMapMode, GetViewportOrg
 */
#define SetViewportOrg(hdc, pPt)    SetDCLCS(hdc, DC_LCS_VORG, pPt)

/**
 * \def SetViewportExt(hdc, pPt)
 * \brief Set the x-extents and y-extents of the current viewport for
 *        a device context.
 *
 * This function sets the x-extents and y-extens of the current viewport of
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport extents will be set.
 *
 * \sa GetMapMode, GetViewportExt
 */
#define SetViewportExt(hdc, pPt)    SetDCLCS(hdc, DC_LCS_VEXT, pPt)

/**
 * \def SetWindowOrg(hdc, pPt)
 * \brief Set the x-coordinates and y-coordinates of the window for
 *        a device context.
 *
 * This function sets the x-coordinates and y-coordinates of
 * the window origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window origin will be set.
 *
 * \sa GetMapMode, GetWindowOrg
 */
#define SetWindowOrg(hdc, pPt)      SetDCLCS(hdc, DC_LCS_WORG, pPt)

/**
 * \def SetWindowExt(hdc, pPt)
 * \brief Set the x-extents and y-extents of the current window for
 *        a device context.
 *
 * This function sets the x-extents and y-extens of the current window of
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window extents will be set.
 *
 * \sa GetMapMode, GetWindowExt
 */
#define SetWindowExt(hdc, pPt)      SetDCLCS(hdc, DC_LCS_WEXT, pPt)

/**
 * \fn void GUIAPI DPtoLP (HDC hdc, POINT* pPt)
 * \brief Converts device coordinates into logical coordinates.
 *
 * This function converts device coordinates into logical coordinates
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context,
 * the settings of the origins and extents for the window and viewport.
 * The x-coordinate and y-coordinate contained in struct \a pPt will be
 * transformed.
 *
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa LPtoDP
 */
MG_EXPORT void GUIAPI DPtoLP (HDC hdc, POINT* pPt);


/**
 * \fn void GUIAPI LPtoDP (HDC hdc, POINT* pPt)
 * \brief Converts logical coordinates into device coordinates.
 *
 * This function converts logical coordinates into device coordinates
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context,
 * the settings of the origins and extents for the window and viewport.
 * The x-coordinate and y-coordinate contained in struct \a pPt will be
 * transformed.
 *
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa DPtoLP
 */
MG_EXPORT void GUIAPI LPtoDP (HDC hdc, POINT* pPt);

/**
 * \fn void GUIAPI SPtoLP (HDC hdc, POINT* pPt)
 * \brief Converts screen coordinates into logical coordinates.
 *
 * This function converts screen coordinates into logical coordinates
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context,
 * the settings of the origins and extents for the window and viewport.
 * The x-coordinate and y-coordinate contained in struct \a pPt will be
 * transformed.
 *
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa LPtoSP
 */
MG_EXPORT void GUIAPI SPtoLP(HDC hdc, POINT* pPt);


/**
 * \fn void GUIAPI LPtoSP (HDC hdc, POINT* pPt)
 * \brief Converts logical coordinates into screen coordinates.
 *
 * This function converts logical coordinates into screen coordinates
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context,
 * the settings of the origins and extents for the window and viewport.
 * The x-coordinate and y-coordinate contained in struct \a pPt will be
 * transformed.
 *
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa SPtoLP
 */
MG_EXPORT void GUIAPI LPtoSP(HDC hdc, POINT* pPt);

    /** @} end of map_fns */

    /**
     * \defgroup clip_fns Clipping operations
     * @{
     */

/**
 * \fn void GUIAPI ExcludeClipRect (HDC hdc, const RECT* prc)
 * \brief Excludes the specified rectangle from the current visible region of
 *        a DC.
 *
 * This function excludes the specified rect \a prc from the current visible
 * region of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc The rectangle to be excluded.
 *
 * \sa IncludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI ExcludeClipRect (HDC hdc, const RECT* prc);

/**
 * \fn void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc)
 * \brief Includes the specified rectangle to the current visible region of
 *        a DC.
 *
 * This function includes the specified rectangle \a prc to the current
 * visible region of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc The rectangle to be included.
 *
 * \sa ExcludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc);

/**
 * \fn BOOL GUIAPI PtVisible (HDC hdc, int x, int y)
 * \brief Check whether a point is visible.
 *
 * This function checks whether the point specified by \a (x,y) is visible, i.e.
 * it is within the current visible clipping region of the device context
 * \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The coordinates of the point.
 * \param y x,y: The coordinates of the point.
 * \return TRUE for visible, else for not.
 *
 * \sa RectVisible, region_fns
 */
MG_EXPORT BOOL GUIAPI PtVisible (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI ClipRectIntersect (HDC hdc, const RECT* prc)
 * \brief Intersects the specified rectangle with the visible region of the DC.
 *
 * This function intersects the specified rectangle \a prc with the visible
 * region of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc Pointer to the rectangle.
 *
 * \sa IncludeClipRect, ExcludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI ClipRectIntersect (HDC hdc, const RECT* prc);

/**
 * \fn void GUIAPI SelectClipRect (HDC hdc, const RECT* prc)
 * \brief Set the visible region of a DC to be a rectangle.
 *
 * This function sets the visible region of the device context \a hdc
 * to the rectangle pointed to by \a prc.
 *
 * \param hdc The device context.
 * \param prc Pointer to the rectangle.
 *
 * \sa SelectClipRegion, region_fns
 */
MG_EXPORT void GUIAPI SelectClipRect (HDC hdc, const RECT* prc);

#define RGN_COPY        0x00
#define RGN_DIFF        0x01
#define RGN_AND         0x02
#define RGN_OR          0x03
#define RGN_XOR         0x04

/**
 * \fn int GUIAPI SelectClipRegionEx (HDC hdc, const CLIPRGN* pRgn, int fnMode)
 * \brief Combines the specified region with the current clipping
 *        region using the specified mode.
 *
 * This function combines the specified region with the current
 * clipping region using the specified mode.
 *
 * If an error occurs when this function is called, the previous
 * clipping region for the specified device context is not affected.
 *
 * The SelectClipRegionEx function assumes that the coordinates
 * for the specified region are specified in device units.
 *
 * Only a copy of the region identified by the pRgn parameter is used.
 * The region itself can be reused after this call or it can be deleted.
 *
 * \param hdc Handle to the device context.
 * \param pRgn Pointer to the region to be selected. This handle can
 *        only be NULL when the RGN_COPY mode is specified.
 * \param fnMode Specifies the operation to be performed. It must be one of
 *        the following values:
 *
 *        - RGN_AND\n
 *          The new clipping region combines the overlapping areas of
 *          the current clipping region and the region identified by pRgn.
 *        - RGN_COPY\n
 *          The new clipping region is a copy of the region identified
 *          by pRgn. This is identical to SelectClipRegion. If the
 *          region identified by pRgn is NULL, the new clipping region
 *          is the default clipping region (the default clipping region
 *          is a null region).
 *        - RGN_DIFF\n
 *          The new clipping region combines the areas of the current
 *          clipping region with those areas excluded from the region
 *          identified by hrgn.
 *        - RGN_OR\n
 *          The new clipping region combines the current clipping region
 *          and the region identified by hrgn.
 *        - RGN_XOR\n
 *          The new clipping region combines the current clipping region
 *          and the region identified by hrgn but excludes any overlapping areas.
 *
 * \return The return value specifies the new clipping region's complexity;
 *         it can be one of the following values.
 *
 * \retval NULLREGION     Region is empty.
 * \retval SIMPLEREGION   Region is a single rectangle.
 * \retval COMPLEXREGION  Region is more than one rectangle.
 * \retval -1             An error occurred.
 *
 * \sa SelectClipRect, SelectClipRegion, GetClipRegion, region_fns
 */
MG_EXPORT int GUIAPI SelectClipRegionEx (HDC hdc, const CLIPRGN* pRgn,
                int fnMode);

/**
 * \fn void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn)
 * \brief Set the visible region of a DC to be a region.
 *
 * This function sets the visible region of the device context \a hdc
 * to the region pointed to by \a pRgn.
 *
 * \param hdc The device context.
 * \param pRgn Pointer to the region.
 *
 * \sa SelectClipRect, GetClipRegion, region_fns
 */
MG_EXPORT void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn);

/**
 * \fn int GUIAPI OffsetClipRegion (HDC hdc, int off_x, int off_y)
 * \brief Moves the clipping region of a device context by the
 *        specified offsets.
 *
 * This function moves the clipping region of a device context \a hdc by
 * the specified offsets (off_x, off_y).
 *
 * \param hdc Handle to the device context.
 * \param off_x Specifies the number of device units to move left or right.
 * \param off_y pecifies the number of device units to move up or down.
 *
 * \return The return value specifies the new clipping region's complexity;
 *         it can be one of the following values.
 *
 * \retval NULLREGION     Region is empty.
 * \retval SIMPLEREGION   Region is a single rectangle.
 * \retval COMPLEXREGION  Region is more than one rectangle.
 * \retval -1             An error occurred.
 *
 * \sa SelectClipRegion
 */
MG_EXPORT int GUIAPI OffsetClipRegion (HDC hdc, int nXOffset, int nYOffset);

/**
 * \fn void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect)
 * \brief Retrieve the bounding rectangle of the current visible region of a DC.
 *
 * This function retrieves the bounding rectangle of the current visible region
 * of the specified device context \a hdc, and returned through \a pRect.
 *
 * \param hdc The device context.
 * \param pRect The bounding rectangle will be returned through this buffer.
 *
 * \sa region_fns
 */
MG_EXPORT void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect);

/**
 * \fn BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect)
 * \brief Check whether the specified rectangle is visible.
 *
 * This function checks whether the rectangle pointed to by \a pRect is
 * visible, i.e. it is intersected with the current visible region of the
 * device context \a hdc.
 *
 * \param hdc The device context.
 * \param pRect Pointer to the rectangle.
 *
 * \sa PtVisible, region_fns
 *
 * Example:
 *
 * \include rectvisible.c
 */
MG_EXPORT BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect);

/**
 * \fn int GetClipBox (HDC hdc, RECT* clipbox)
 * \brief Retrieve the bounding rectangle of the current clipping region of a DC.
 *
 * This function retrieves the bounding rectangle of the current clipping region
 * of the specified device context \a hdc, and returned through \a clipbox.
 *
 * \param hdc The device context.
 * \param clipbox The bounding rectangle will be returned through this buffer.
 *
 * \return -1 for error, else the type of clipping region, can be one of
 *        the following values:
 *   - NULLREGION\n     A null region.
 *   - SIMPLEREGION\n   A simple region.
 *   - COMPLEXREGION\n  A complex region.
 *
 * \sa GetBoundsRect
 */
MG_EXPORT int GUIAPI GetClipBox (HDC hdc, RECT* clipbox);

/**
 * \fn int GetClipRegion (HDC hdc, CLIPRGN* cliprgn)
 * \brief Get the current clipping region of a DC.
 *
 * This function gets the current clipping region
 * of the specified device context \a hdc, and returned through \a cliprgn.
 *
 * \param hdc The device context.
 * \param cliprgn The clipping region will be returned through this buffer.
 *
 * \return -1 for error, else the type of clipping region, can be one of
 *        the following values:
 *   - NULLREGION\n     A null region.
 *   - SIMPLEREGION\n   A simple region.
 *   - COMPLEXREGION\n  A complex region.
 *
 * \sa GetClipBox, SelectClipRegion
 */
MG_EXPORT int GUIAPI GetClipRegion (HDC hdc, CLIPRGN* cliprgn);

    /** @} end of clip_fns */

    /**
     * \defgroup bmp_fns BITMAP and blitting operations
     *
     * Example:
     *
     * \include blitting.c
     *
     * @{
     */

/**
 * \fn BOOL GUIAPI GetBitmapFromDC (HDC hdc, \
                int x, int y, int w, int h, BITMAP* bmp)
 * \brief Get image box on a DC and saves it into a BITMAP object.
 *
 * This function gets image box on the specified device context \a hdc,
 * and saves the image bits into the BITMAP object pointed to by \a bmp.
 * The image box begins at \a (x,y), and is \a w wide and \a h high.
 * You must make sure that \a bmp->bits is big enough to store the image.
 *
 * \param hdc The device context.
 * \param x The x coordinate in pixels of upper-left corner of the image box.
 * \param y The y coordinate in pixels of upper-left corner of the image box.
 * \param w The width of the image box.
 * \param h The height of the image box.
 * \param bmp The pointer to the BITMAP object.
 *
 * \note If BITMAP's bmPitch is zero or bmBits is NULL, it will be malloc bmBits
 * internal, you need init the BITMAP surely(such as \a InitBitmap) or
 * memset(bmp, 0, sizeof(BITMAP)).
 *
 * \sa FillBoxWithBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI GetBitmapFromDC (HDC hdc,
                int x, int y, int w, int h, BITMAP* bmp);

/**
 * \fn BOOL GUIAPI FillBoxWithBitmap (HDC hdc, \
                int x, int y, int w, int h, const BITMAP *bmp)
 * \brief Fills a box with a BITMAP object.
 *
 * This function fills a box with a BITMAP object pointed to by \a bmp.
 * \a (x,y) is the upper-left corner of the box, and \a w, \a h are
 * the width and the height of the box respectively.
 *
 * This function will scale the bitmap when necessary; that is, when
 * the width or the height of the box is not equal to the with or
 * the height of the BITMAP object.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box. Can be zero, means the width
 *        or the height will be equal to the width or the height of
 *        the BITMAP object.
 * \param h The height of the box. Can be zero, means the width
 *        or the height will be equal to the width or the height of
 *        the BITMAP object.
 * \param bmp The pointer to the BITMAP object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note You can specify the alpha value or the color key of the BITMAP object,
 *       and the current raster operation was set by \a SetRasterOperation for
 *       the DC will override the alpha value of color key if ROP is not ROP_SET.
 *
 * \note If the bitmap has BMP_TYPE_RLE, the target \a w and \a h will
 *       be ignored. That is, the scale of the bitmap is not supported
 *       for RLE encoded bitmap.
 *
 * \sa FillBoxWithBitmapPart, GetBitmapFromDC, bmp_load_fns
 */
MG_EXPORT BOOL GUIAPI FillBoxWithBitmap (HDC hdc, int x, int y, int w, int h,
                const BITMAP *bmp);

/**
 * \fn BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc, \
                int x, int y, int w, int h, \
                int bw, int bh, const BITMAP* bmp, int xo, int yo)
 * \brief Fills a box with a part of a bitmap oject.
 *
 * This function fills a box with a part of a bitmap object pointed to by
 * \a bmp. \a (x,y) is the upper-left corner of the box, and \a w, \a h are
 * the width and the height of the box respectively. \a (xo, yo) is the
 * start position of the part box in the bitmap relative to upper-left
 * corner of the bitmap, and \a bw, \a bh are the width and the height of
 * the full bitmap expected.
 *
 * If \a bw or \a bh is less than or equal to zero, this function will use
 * the original width and height of the bitmap, else it will scale
 * the BITMAP object when necessary; that is, when bw or bh
 * is not equal to the width or the height of the BITMAP object.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box.
 * \param h The height of the box.
 * \param bw The width of the full bitmap expected.
 *        Can be zero, means the width or the height will be equal to
 *        the width or the height of the BITMAP object.
 * \param bh The height of the full bitmap expected.
 *        Can be zero, means the width or the height will be equal to
 *        the width or the height of the BITMAP object.
 * \param xo xo,yo: The start position of the part box in the bitmap
 *        relative to upper-left corner of the BITMAP object.
 * \param yo xo,yo: The start position of the part box in the bitmap
 *        relative to upper-left corner of the BITMAP object.
 * \param bmp The pointer to the BITMAP object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note 1: You can specify the alpha value or the color key of the BITMAP
 *          object, and the current raster operation was set by
 *          \a SetRasterOperation for the DC will override the alpha
 *          value or the color key if ROP is not ROP_SET.
 * \note 2: xo/yo must be greater zero and (xo + w) < bw, (yo + h) < bh.
 *          else with fill nothing.
 *
 * \note 3: xo/yo must be multiply scale factor when bitmap scaled.
 *
 * \note 4: if bw or bh is not equal to he width or the height of the
 *          BITMAP object, it will be scale bitmap, but fill box is
 *          also (0, 0, w,h).
 *
 * \note The RLE encoded bitmap is not supported by this function so far.
 *
 * \sa FillBoxWithBitmap, GetBitmapFromDC, bmp_struct
 */
MG_EXPORT BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc,
                int x, int y, int w, int h,
                int bw, int bh, const BITMAP* bmp, int xo, int yo);

/**
 * \fn BOOL FillBitmapPartInBox (HDC hdc, int box_x, int box_y, \
                int box_w, int box_h, const BITMAP* pbmp, int bmp_x, int bmp_y, \
                int bmp_w, int bmp_h)
 * \brief Fills a part of bitmap into a box, and the parts bitmap will be scaled if needed.
 *
 * This function gives an intuitionistic way to use function FillBoxWidthBitmapPart.
 *
 * \param hdc The target DC to show bitmap
 * \param box_x The left of target box
 * \param box_y The top of target box
 * \param box_w The width of target box
 * \param box_h The height of target box
 * \param pbmp The bitmap which will be showed in the hdc
 * \param bmp_x The visible part's left of bitmap
 * \param bmp_y The visible part's top of bitmap
 * \param bmp_w The visible part's width of bitmap
 * \param bmp_h The visible part's height of bitmap
 *
 * \sa FillBoxWidthBitmapPart
 */

MG_EXPORT BOOL GUIAPI FillBitmapPartInBox (HDC hdc, int box_x, int box_y,
                int box_w, int box_h, const BITMAP* pbmp, int bmp_x, int bmp_y,
                int bmp_w, int bmp_h);

/**
 * The color blend mothed.
 *
 * See [Compositing and Blending Level 1](https://www.w3.org/TR/compositing-1/)
 * Note: we define the same values with Pixman.
 */
typedef enum {
    COLOR_BLEND_LEGACY      = 0,
    COLOR_BLEND_FLAGS_MASK  = 0x00FF,

    /** Porter Duff rule: clear */
    COLOR_BLEND_PD_CLEAR    = 0x0100,   // PIXMAN_OP_CLEAR          = 0x00
    /** Porter Duff rule: source */
    COLOR_BLEND_PD_SRC      = 0x0101,   // PIXMAN_OP_SRC            = 0x01
    /** Porter Duff rule: destination */
    COLOR_BLEND_PD_DST      = 0x0102,   // PIXMAN_OP_DST            = 0x02
    /** Porter Duff rule: source over destination */
    COLOR_BLEND_PD_SRC_OVER = 0x0103,   // PIXMAN_OP_OVER           = 0x03
    /** Porter Duff rule: destination over source */
    COLOR_BLEND_PD_DST_OVER = 0x0104,   // PIXMAN_OP_OVER_REVERSE   = 0x04
    /** Porter Duff rule: souorce in destination */
    COLOR_BLEND_PD_SRC_IN   = 0x0105,   // PIXMAN_OP_IN             = 0x05
    /** Porter Duff rule: destination in souorce */
    COLOR_BLEND_PD_DST_IN   = 0x0106,   // PIXMAN_OP_IN_REVERSE     = 0x06
    /** Porter Duff rule: source held out by destination */
    COLOR_BLEND_PD_SRC_OUT  = 0x0107,   // PIXMAN_OP_OUT            = 0x07
    /** Porter Duff rule: destination held out by source */
    COLOR_BLEND_PD_DST_OUT  = 0x0108,   // PIXMAN_OP_OUT_REVERSE    = 0x08
    /** Porter Duff rule: source atop destination */
    COLOR_BLEND_PD_SRC_ATOP = 0x0109,   // PIXMAN_OP_ATOP           = 0x09
    /** Porter Duff rule: destination atop source */
    COLOR_BLEND_PD_DST_ATOP = 0x010a,   // PIXMAN_OP_ATOP_REVERSE   = 0x0a
    /** Porter Duff rule: source xor destination */
    COLOR_BLEND_PD_XOR      = 0x010b,   // PIXMAN_OP_XOR            = 0x0b
    /** Porter Duff rule: plus */
    COLOR_BLEND_PD_PLUS     = 0x010c,   // PIXMAN_OP_ADD            = 0x0c
    /** Porter Duff rule: modulate */
    COLOR_BLEND_PD_MODULATE = 0x010d,   // PIXMAN_OP_SATURATE       = 0x0d

    COLOR_BLEND_PD_FLAG     = 0x0100,
    COLOR_BLEND_PD_FIRST    = COLOR_BLEND_PD_CLEAR,
    COLOR_BLEND_PD_LAST     = COLOR_BLEND_PD_MODULATE,

    /**
     * Separable blend mode: normal
     * The blending formula simply selects the source color.
     */
    COLOR_BLEND_SP_NORMAL       = 0x0801,
    /**
     * Separable blend mode: multiply
     * Darkens by multiplying colors: Sc·Dc.
     */
    COLOR_BLEND_SP_MULTIPLY     = 0x0830,   // PIXMAN_OP_MULTIPLY       = 0x30
    /**
     * Separable blend mode: screen
     * Complements product of complements: Sc + Dc - Sc·Dc.
     */
    COLOR_BLEND_SP_SCREEN       = 0x0831,   // PIXMAN_OP_SCREEN         = 0x31
    /**
     * Separable blend mode: overlay
     * Inverse of hard-light.
     */
    COLOR_BLEND_SP_OVERLAY      = 0x0832,   // PIXMAN_OP_OVERLAY        = 0x32
    /**
     * Separable blend mode: darken
     * Minimum of colors: min(Sc, Dc).
     */
    COLOR_BLEND_SP_DARKEN       = 0x0833,   // PIXMAN_OP_DARKEN         = 0x33
    /**
     * Separable blend mode: lighten
     * Maximum of colors: max(Sc, Dc).
     */
    COLOR_BLEND_SP_LIGHTEN      = 0x0834,   // PIXMAN_OP_LIGHTEN        = 0x34
    /**
     * Separable blend mode: color-dodge
     * Brightens destination based on source.
     */
    COLOR_BLEND_SP_COLOR_DODGE  = 0x0835,    // PIXMAN_OP_COLOR_DODGE   = 0x35,
    /**
     * Separable blend mode: color-burn
     * Darkens destination based on source.
     */
    COLOR_BLEND_SP_COLOR_BURN   = 0x0836,   // PIXMAN_OP_COLOR_BURN     = 0x36,
    /**
     * Separable blend mode: hard-light
     * Similar to effect of harsh spotlight.
     */
    COLOR_BLEND_SP_HARD_LIGHT   = 0x0837,   // PIXMAN_OP_HARD_LIGHT     = 0x37,
    /**
     * Separable blend mode: soft-light
     * Similar to effect of soft spotlight.
     */
    COLOR_BLEND_SP_SOFT_LIGHT   = 0x0838,   // PIXMAN_OP_SOFT_LIGHT     = 0x38
    /**
     * Separable blend mode: difference
     * Subtracts the darker from the lighter: Abs(Dc - Sc).
     */
    COLOR_BLEND_SP_DIFFERENCE   = 0x0839,   // PIXMAN_OP_DIFFERENCE     = 0x39
    /**
     * Separable blend mode: exclusion
     * Similar to Difference but lower contrast.
     */
    COLOR_BLEND_SP_EXCLUSION    = 0x083a,   // PIXMAN_OP_EXCLUSION      = 0x3a

    COLOR_BLEND_SP_FLAG         = 0x0800,
    COLOR_BLEND_SP_FIRST        = COLOR_BLEND_SP_NORMAL,
    COLOR_BLEND_SP_LAST         = COLOR_BLEND_SP_EXCLUSION,

    /**
     * Non-Separable blend mode: hue
     * Creates a color with the hue of the source color and
     * the saturation and luminosity of the backdrop color.
     */
    COLOR_BLEND_NS_HUE          = 0x103b,   // PIXMAN_OP_HSL_HUE        = 0x3b
    /**
     * Non-Separable blend mode: saturation
     * Creates a color with the saturation of the source color and
     * the hue and luminosity of the backdrop color.
     */
    COLOR_BLEND_NS_SATURATION   = 0x103c,   // PIXMAN_OP_HSL_SATURATION = 0x3c
    /**
     * Non-Separable blend mode: color
     * Creates a color with the hue and saturation of the source color
     * and the luminosity of the backdrop color.
     */
    COLOR_BLEND_NS_COLOR        = 0x103d,   // PIXMAN_OP_HSL_COLOR      = 0x3d

    /**
     * Non-Separable blend mode: luminosity
     * Creates a color with the luminosity of the source color and
     * the hue and saturation of the backdrop color.
     */
    COLOR_BLEND_NS_LUMINOSITY   = 0x103e,  // PIXMAN_OP_HSL_LUMINOSITY  = 0x3e

    COLOR_BLEND_NS_FLAG         = 0x1000,
    COLOR_BLEND_NS_FIRST        = COLOR_BLEND_NS_HUE,
    COLOR_BLEND_NS_LAST         = COLOR_BLEND_NS_LUMINOSITY,

    COLOR_BLEND_FIRST        = COLOR_BLEND_PD_CLEAR,
    COLOR_BLEND_LAST         = COLOR_BLEND_NS_LAST,
} ColorBlendMethod;

/**
 * The color logical operations.
 */
typedef enum {
   COLOR_LOGICOP_CLEAR = 0,
   COLOR_LOGICOP_NOR,
   COLOR_LOGICOP_AND_INVERTED,
   COLOR_LOGICOP_COPY_INVERTED,
   COLOR_LOGICOP_AND_REVERSE,
   COLOR_LOGICOP_INVERT,
   COLOR_LOGICOP_XOR,
   COLOR_LOGICOP_NAND,
   COLOR_LOGICOP_AND,
   COLOR_LOGICOP_EQUIV,
   COLOR_LOGICOP_NOOP0,
   COLOR_LOGICOP_OR_INVERTED1,
   COLOR_LOGICOP_COPY,
   COLOR_LOGICOP_OR_REVERSE,
   COLOR_LOGICOP_OR,
   COLOR_LOGICOP_SET,
} ColorLogicalOp;

/**
 * \fn void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh, \
                HDC hddc, int dx, int dy, DWORD dwRop)
 * \brief Performs a bit-block transfer from a device context into
 *        another device context.
 *
 * This function performs a bit-block transfer of the color data cooresponding
 * to a rectangle of pixels from the specified source device context \a hsdc
 * into a destination device context \a hddc. \a (sx,sy,sw,sh) specifies the
 * rectangle in the source DC, and \a (dx,dy) specifies the position of the
 * rectangle in the destination DC. Note that the size of the two rectangles
 * are identical.
 *
 * Note that all coordinates should be in the device space.
 *
 * \param hsdc The source device context.
 * \param sx The x coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sy The y coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sw The width of the source rectangle.
 * \param sh The height of the source rectangle.
 * \param hddc The destination device context \a hddc.
 * \param dx The x coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dy The y coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dwRop The color blending method, see \a ColorBlendMethod.
 *        This argument is only valid when Pixman is used.
 *
 * \note When the source color key is specified for the blitting operation,
 *      or the formats of the device contexts are not supported by Pixman,
 *      this function will use the legacy implementation. In this situation,
 *      the color blending method will be ignored.
 *
 * \sa StretchBlt, SetMemDCAlpha, SetMemDCColorKey, ColorBlendMethod
 */
MG_EXPORT void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh,
                HDC hddc, int dx, int dy, DWORD dwRop);

/**
 * The scaling filter for StretchBlt.
 */
typedef enum {
    /** The fast filter (DDA scaler) */
    SCALING_FILTER_FAST     = 0x00000000,
    /** The good filter */
    SCALING_FILTER_GOOD     = 0x00010000,
    /** The best filter */
    SCALING_FILTER_BEST     = 0x00020000,
    /** The filter using nearest algorithm */
    SCALING_FILTER_NEAREST  = 0x00020000,
    /** The filter using bi-linear algorithm */
    SCALING_FILTER_BILINEAR = 0x00040000,
    /** The filter using convolution algorithm */
    SCALING_FILTER_CONVOLUTION = 0x00050000,
    SCALING_FILTER_SHIFT    = 16,
} ScalingFilter;

/**
 * The stretch extra information for \a StretchBltEx.
 *
 * \sa StretchBltEx
 */
typedef struct _STRETCH_EXTRA_INFO {
    /** The version code; reserved for future, must set zero */
    int version;

    /** The rotation of the source bitmap in 1/64ths of a degree. */
    int rotation;

    /** The x-coordinate of the rotation center in the destination rectangle,
      * relative to the uppper-left corner of the rectangle. */

    int cx;
    /** The y-coordinate of the rotation center in the destination rectangle,
      * relative to the uppper-left corner of the rectangle. */
    int cy;
} STRETCH_EXTRA_INFO;

/**
 * \fn BOOL GUIAPI StretchBltEx (HDC hsdc, int sx, int sy, int sw, int sh,
 *      HDC hddc, int dx, int dy, int dw, int dh, int rotation, DWORD dwRop)
 * \brief Copies a bitmap from a source rectangle into a destination
 *        rectangle, streches and rotates the bitmap if necessary.
 *
 * This function copies a bitmap from a source rectangle into a destination
 * rectangle, streching or compressing the bitmap to fit the dimension of
 * the destination rectangle, if necessary. This function is similar with
 * \sa BitBlt function except the former scaling the bitmap. \a (dw,dh)
 * specifies the size of the destination rectangle.
 *
 * \param hsdc The source device context.
 * \param sx The x coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sy The y coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sw The width of the source rectangle.
 * \param sh The height of the source rectangle.
 * \param hddc The destination device context \a hddc.
 * \param dx The x coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dy The y coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dw The width of the destination rectangle.
 * \param dh The height of the destination rectangle.
 * \param sei The pointer to a stretch extra information strucure;
 *        can be NULL. Note this only works when Pixman is used.
 * \param dwRop The color blending method, see \a ColorBlendMethod, OR'd
 *        with a fiter of the scaling, see \a ScalingFilter.
 *        This argument only works when Pixman is used.
 *
 * \return TRUE for success, FALSE for bad arguments and there is no any
 *      drawing occurred.
 *
 * \note When the source color key is specified for the blitting operation,
 *      or the formats of the device contexts are not supported by Pixman,
 *      this function will use the legacy implementation. In this situation,
 *      the color blending method, the scaling filter, and the rotation
 *      will be ignored.
 *
 * \sa BitBlt, SetMemDCAlpha, SetMemDCColorKey, STRETCH_EXTRA_INFO,
 *     ColorBlendMethod, ScalingFilter
 */
MG_EXPORT BOOL GUIAPI StretchBltEx (HDC hsdc, int sx, int sy, int sw, int sh,
        HDC hddc, int dx, int dy, int dw, int dh,
        const STRETCH_EXTRA_INFO *sei, DWORD dwRop);
 
/**
 * \fn void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh, \
                HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
 * \brief Copies a bitmap from a source rectangle into a destination
 *        rectangle, streches the bitmap if necessary.
 *
 * This function copies a bitmap from a source rectangle into a destination
 * rectangle, streching or compressing the bitmap to fit the dimension of
 * the destination rectangle, if necessary. This function is similar with
 * \sa BitBlt function except the former scaling the bitmap. \a (dw,dh)
 * specifies the size of the destination rectangle.
 *
 * \param hsdc The source device context.
 * \param sx The x coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sy The y coordinate of the upper-left corner of the rectangle
 *        in the source DC.
 * \param sw The width of the source rectangle.
 * \param sh The height of the source rectangle.
 * \param hddc The destination device context \a hddc.
 * \param dx The x coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dy The y coordinate of the upper-left corner of the rectangle
 *        in the destination DC.
 * \param dw The width of the destination rectangle.
 * \param dh The height of the destination rectangle.
 * \param dwRop The color blending method, see \a ColorBlendMethod.
 *        This argument is only valid when Pixman is used.
 *
 * \note When color key is specified for the blitting operation, or the
 *      formats of the device contexts are not supported by Pixman,
 *      this function will use the legacy implementation. In this situation,
 *      the color blending method and the scaling filter will be ignored.
 *
 * \sa BitBlt, SetMemDCAlpha, SetMemDCColorKey, ColorBlendMethod
 */
static inline void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh,
        HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
{
    StretchBltEx (hsdc, sx, sy, sw, sh, hddc, dx, dy, dw, dh, NULL, dwRop);
}

/* Use this if you want to have the legacy manner of StretchBlt */
MG_EXPORT void GUIAPI StretchBltLegacy (HDC hsdc, int sx, int sy, int sw, int sh,
                       HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop);

/**
 * \fn BOOL GUIAPI ScaleBitmapEx (BITMAP* dst, const BITMAP* src, \
 *             HDC ref_dc)
 * \brief Scales a BITMAP object into another BITMAP object by specify
 *        algorithm.
 *
 * This function scales a BITMAP object \a src into another BITMAO object \a dst
 * by specify algorithm. The source rectangle and the destination rectangle
 * both are defined in the BITMAP objects.
 *
 * \param dst The destination BITMAP object.
 * \param src The srouce BITMAP object.
 * \param ref_dc The device context of Bitmap's relative device dc.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note you should init the dst Bitmap first, such as bmPitch and bmBits.
 *
 * \sa FillBoxWithBitmap, StretchBlt, bmp_struct
 */
MG_EXPORT BOOL GUIAPI ScaleBitmapEx (BITMAP* dst, const BITMAP* src,
        HDC ref_dc);

#define ScaleBitmap(dst, src) ScaleBitmapEx(dst, src, HDC_SCREEN)

/**
 * \fn gal_pixel GUIAPI GetPixelInBitmapEx (const BITMAP* bmp, int x, int y, Uint8* alpha)
 * \brief Returns the pixel value in a BITMAP object.
 *
 * This function returns the pixel value and alpha at the position \a (x,y) in
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param alpha The point to alpha of position (x, y).
 * \return The pixel value, if the position is out of the bitmap,
 *         zero returned.
 *
 * \note if the bitmap with BMP_TYPE_ALPHA_MASK flag, the alpha value will
 *       be read from bmp->AlphaMask(Bitmap Alpha Mask array).
 *
 * \sa SetPixelInBitmapEx, bmp_struct
 */
MG_EXPORT gal_pixel GUIAPI GetPixelInBitmapEx (const BITMAP* bmp, int x, int y, Uint8* alpha);

/**
 * \fn static inline gal_pixel GUIAPI GetPixelInBitmap (const BITMAP* bmp, int x, int y)
 * \brief Returns the pixel value in a BITMAP object.
 *
 * This function returns the pixel value at the position \a (x,y) in
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \return The pixel value, if the position is out of the bitmap,
 *         zero returned.
 *
 * \sa SetPixelInBitmap, bmp_struct
 */
static inline gal_pixel GUIAPI GetPixelInBitmap (const BITMAP* bmp, int x, int y)
{
    return GetPixelInBitmapEx (bmp, x, y, NULL);
}

/**
 * \fn BOOL GUIAPI SetPixelInBitmapEx (const BITMAP* bmp, \
                int x, int y, gal_pixel pixel, Uint8* alpha)
 * \brief Set pixel and alpha value in a BITMAP object.
 *
 * This function sets the pixel and alpha value  at the position \a (x,y) in
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param pixel The pixel value.
 * \param alpha The point to alpha of position (x, y).
 *
 * \return TRUE on success. If the position is out of the bitmap,
 *         FALSE returned.
 *
 * \note if the bitmap with BMP_TYPE_ALPHA_MASK flag, the alpha value will
 *       be write to bmp->AlphaMask(Bitmap Alpha Mask array).
 *
 * \sa GetPixelInBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI SetPixelInBitmapEx (const BITMAP* bmp,
                int x, int y, gal_pixel pixel, const Uint8* alpha);

/**
 * \fn static inline BOOL GUIAPI SetPixelInBitmap (const BITMAP* bmp, \
                int x, int y, gal_pixel pixel)
 * \brief Set pixel value in a BITMAP object.
 *
 * This function sets the pixel value at the position \a (x,y) in
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to
 *        the upper-left corner of the bitmap.
 * \param pixel The pixel value.
 *
 * \return TRUE on success. If the position is out of the bitmap,
 *         FALSE returned.
 *
 * \sa GetPixelInBitmap, bmp_struct
 */
static inline BOOL GUIAPI SetPixelInBitmap (const BITMAP* bmp,
        int x, int y, gal_pixel pixel)
{
    return SetPixelInBitmapEx (bmp, x, y, pixel, NULL);
}

#ifdef _MGMISC_SAVESCREEN

/**
 * \fn BOOL GUIAPI SaveScreenRectContent (const RECT* rc, \
                const char* filename)
 * \brief Saves content of a rectangle in the screen to a file.
 *
 * This function saves the content of the rect \a rc to the image
 * file \a filename. MiniGUI uses the extension name of the file to
 * determine the format of the image file.
 *
 * \param rc The RECT object defined the rectangle in the screen.
 * \param filename The name of the image file.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _MGMISC_SAVESCREEN.
 *
 * \sa bmp_load_fns
 */
MG_EXPORT BOOL GUIAPI SaveScreenRectContent (const RECT* rcWin,
                const char* filename);

/**
 * \fn BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename)
 * \brief Saves content of a main window to a file.
 *
 * This function saves the content of the main window \a hWnd to the image
 * file \a filename. MiniGUI uses the extension name of the file to
 * determine the format of the image file.
 *
 * \param hWnd Handle to the main window.
 * \param filename The name of the image file.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _MGMISC_SAVESCREEN.
 *
 * \sa bmp_load_fns
 */
MG_EXPORT BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename);
#endif

    /** @} end of bmp_fns */

    /**
     * \defgroup icon_fns Icon operations
     * @{
     */

/**
 * \fn HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename, int which)
 * \brief Load an icon from a Windows ICO file.
 *
 * This function loads an icon from a Windows ICO file named \a filename
 * and creates an icon object. This function can load monochrome, 16-color,
 * and 256-color icons. Some Windows ICO file contain two icons in different
 * sizes. You can tell this function to load which icon though \a which,
 * 0 for the first icon, and 1 for the second icon. Generally, the later
 * icon is the larger icon.
 *
 * \param hdc The device context.
 * \param filename The file name of the ICO file.
 * \param which Tell the function to load which icon.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa CreateIconEx
 */
MG_EXPORT HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename,
                int which);

/**
 * \fn HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which)
 * \brief Load an icon from a memory area.
 *
 * This function loads an icon from a memroy area pointed to by \a area.
 * The memory area has the same layout as the M$ Windows ICO file.
 * This function can load monochrome, 16-color, and 256-color icons.
 * Some Windows ICO file contain two icons in different sizes. You can tell
 * this function to load which icon though \a which, 0 for the first icon,
 * and 1 for the second icon. Generally, the later icon is the larger icon.
 *
 * \param hdc The device context.
 * \param area The pointer to the memory area.
 * \param which Tell the function to load which icon.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa CreateIconEx
 */
MG_EXPORT HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which);

/**
 * \fn HICON GUIAPI CreateIconEx (HDC hdc, int w, int h, \
                const BYTE* AndBits, const BYTE* XorBits, int colornum, \
                const RGB* pal)
 * \brief Creates an icon object from the memory.
 *
 * This function creates an icon from memory data rather than an icon file.
 * \a w and \a h are the width and the height of the icon respectively.
 * \a pANDBits and \a pXORBits are AND bitmask and XOR bitmask of the icon.
 * MiniGUI currently support mono-color cursor 256-color icon and 16-color icon,
 * \a colornum specifies the cursor's color depth. For mono-color, it should
 * be 1, and for 16-color cursor, it should be 4.
 *
 * \param hdc The device context.
 * \param w The width of the icon.
 * \param h The height of the icon.
 * \param AndBits The pointer to the AND bits of the icon.
 * \param XorBits The pointer to the XOR bits of the icon.
 * \param colornum The bit-per-pixel of XOR bits.
 * \param pal The palette of icon.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa LoadIconFromFile
 */
MG_EXPORT HICON GUIAPI CreateIconEx (HDC hdc, int w, int h,
                const BYTE* AndBits, const BYTE* XorBits, int colornum,
                const RGB* pal);
/**
 * \def CreateIcon
 * \sa CreateIconEx
 */
#define CreateIcon(hdc, w, h, AndBits, XorBits, colornum) \
        CreateIconEx(hdc, w, h, AndBits, XorBits, colornum, NULL)

/**
 * \fn HICON GUIAPI LoadBitmapIconEx (MG_RWops* area, const char* ext)
 * \brief Load an icon from a general bitmap data source.
 *
 * This function loads an icon from the data source \a area, which contains
 * the data of a bitmap in a specific type specified by \a ext.
 *
 * \param hdc The device context, currently ignored.
 * \param area The data source.
 * \param ext The type of the bitmap (extension of a bitmap file).
 *
 * \return 0 on failure, otherwise success.
 *
 * \sa LoadBitmapIconFromFile, LoadBitmapIconFromMem
 */
MG_EXPORT HICON GUIAPI LoadBitmapIconEx (HDC hdc, MG_RWops* area,
        const char* ext);

/**
 * \fn HICON GUIAPI LoadBitmapIconFromFile (HDC hdc, const char* file_name)
 * \brief Load an icon from a bitmap file.
 *
 * This function loads an icon from a bitmap file specified by \a file_name.
 *
 * \param hdc The device context, currently ignored.
 * \param file_name The file name.
 *
 * \return 0 on failure, otherwise success.
 *
 * \sa LoadBitmapIconEx
 */
MG_EXPORT HICON GUIAPI LoadBitmapIconFromFile (HDC hdc,
        const char* file_name);

/**
 * \fn HICON GUIAPI LoadBitmapIconFromMem (HDC hdc, const void* mem, size_t size,
 *      const char* ext)
 * \brief Load an icon from memory.
 *
 * This function loads an icon from a memory zone specified by \a mem
 * and \a size.
 *
 * \param hdc The device context, currently ignored.
 * \param mem The pointer to the memory zone.
 * \param size The size of the memory zone in bytes.
 * \param ext The the bitmap type (extension of a bitmap file).
 *
 * \return 0 on failure, otherwise success.
 *
 * \sa LoadBitmapIconEx
 */
MG_EXPORT HICON GUIAPI LoadBitmapIconFromMem (HDC hdc,
        const void* mem, size_t size, const char* ext);

/**
 * \fn BOOL GUIAPI DestroyIcon (HICON hicon)
 * \brief Destroys an icon object.
 *
 * This function destroys the icon object \a hicon.
 *
 * \param hicon The icon object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateIconEx, LoadIconFromFile
 */
MG_EXPORT BOOL GUIAPI DestroyIcon (HICON hicon);

/**
 * \fn BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h)
 * \brief Get the size of an icon object.
 *
 * This function gets the size of the icon object \a hicon.
 *
 * \param hicon The icon object.
 * \param w The width of the icon will be returned throught this buffer.
 * \param h The height of the icon will be returned throught this buffer.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateIconEx, LoadIconFromFile
 */
MG_EXPORT BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h);

/**
 * \fn void GUIAPI DrawIcon (HDC hdc, \
                int x, int y, int w, int h, HICON hicon)
 * \brief Draws an icon into a box.
 *
 * This function draws an icon object \a hicon into a box specified by
 * \a (x,y,w,h).
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box.
 * \param h The height of the box.
 * \param hicon The icon object.
 *
 * \sa CreateIconEx, LoadIconFromFile
 */
MG_EXPORT void GUIAPI DrawIcon (HDC hdc,
                int x, int y, int w, int h, HICON hicon);

    /** @} end of icon_fns */

    /**
     * \defgroup rect_fns Rectangle operations
     * @{
     */

/**
 * \fn void SetRect (RECT* prc, int left, int top, int right, int bottom)
 * \brief Set a rectangle.
 *
 * This function sets the rectangle with specified values.
 *
 * \param prc The pointer to the rectangle.
 * \param left The x coordinate of the upper-left corner of the rectangle.
 * \param top The y coordinate of the upper-left corner of the rectangle.
 * \param right The x coordinate of the lower-right corner of the rectangle.
 * \param bottom The y coordinate of the lower-right corner of the rectangle.
 *
 * \sa SetRectEmpty
 */
static inline void SetRect (RECT* prc, int left, int top, int right, int bottom)
{
    (prc)->left = left; (prc)->top = top;
    (prc)->right = right; (prc)->bottom = bottom;
}

/**
 * \fn void SetRectEmpty (RECT* prc)
 * \brief Empties a rectangle.
 *
 * This function empties the rectangle pointed to by \a prc.
 * An empty rectangle in MiniGUI is a rectangle whose width and height both
 * are zero. This function will sets all coordinates of the rectangle to
 * be zero.
 *
 * \param prc The pointer to the rectangle.
 *
 * \sa SetRect
 */
static inline void SetRectEmpty (RECT* prc)
{
    (prc)->left = (prc)->top = (prc)->right = (prc)->bottom = 0;
}

/**
 * \fn void CopyRect (RECT* pdrc, const RECT* psrc)
 * \brief Copies one rectangle to another.
 *
 * This function copies the coordinates of the source rectangle
 * pointed to by \a psrc to the destination rectangle pointed to by \a pdrc.
 *
 * \param pdrc The pointer to the destination rectangle.
 * \param psrc The pointer to the source rectangle.
 *
 * \sa SetRect
 */
static inline void CopyRect (RECT* pdrc, const RECT* psrc)
{
    (pdrc)->left = (psrc)->left; (pdrc)->top = (psrc)->top;
    (pdrc)->right = (psrc)->right; (pdrc)->bottom = (psrc)->bottom;
}

/**
 * \fn void OffsetRect (RECT* prc, int x, int y)
 * \brief Moves a rectangle by offsets.
 *
 * This function moves the specified rectangle by the specified offsets.
 * \a x and \a y specify the amount to move the rectangle left/right or up/down
 * respectively. \a x must be a negative value to move the rectangle to
 * the left, and \a y must be a negative value to move the rectangle up.
 *
 * \param prc The pointer to the rectangle.
 * \param x The x offset.
 * \param y The y offset.
 *
 * \sa InflateRect
 */
static inline void OffsetRect (RECT* prc, int x, int y)
{
    (prc)->left += x; (prc)->top += y; (prc)->right += x; (prc)->bottom += y;
}

/**
 * \fn void InflateRect (RECT* prc, int cx, int cy)
 * \brief Increases or decreases the width and height of an rectangle.
 *
 * This function increases or decreases the width and height of
 * the specified rectangle \a prc. This function adds \a cx units
 * to the left and right ends of the rectangle and \a cy units to the
 * top and bottom. the \a cx and \a cy are signed values; positive values
 * increases the width and height, and negative values decreases them.
 *
 * \param prc The pointer to the rectangle.
 * \param cx The inflating x value.
 * \param cy The inflating y value.
 *
 * \sa InflateRectToPt
 */
static inline void InflateRect (RECT* prc, int cx, int cy)
{
    (prc)->left -= cx; (prc)->top -= cy;
    (prc)->right += cx; (prc)->bottom += cy;
}

/**
 * \fn void InflateRectToPt (RECT* prc, int x, int y)
 * \brief Inflates a rectangle to contain a point.
 *
 * This function inflates the rectangle \a prc to contain the specified
 * point \a (x,y).
 *
 * \param prc The pointer to the rectangle.
 * \param x x,y: The point.
 * \param y x,y: The point.
 *
 * \sa InflateRect
 */
static inline void InflateRectToPt (RECT* prc, int x, int y)
{
    if ((x) < (prc)->left) (prc)->left = (x);
    if ((y) < (prc)->top) (prc)->top = (y);
    if ((x) > (prc)->right) (prc)->right = (x);
    if ((y) > (prc)->bottom) (prc)->bottom = (y);
}

/**
 * \fn BOOL PtInRect(const RECT* prc, int x, int y)
 * \brief Determine whether a point lies within an rectangle.
 *
 * This function determines whether the specified point \a (x,y) lies within
 * the specified rectangle \a prc.
 *
 * A point is within a rectangle if it lies on the left or top side or is
 * within all four sides. A point on the right or bottom side is considered
 * outside the rectangle.
 *
 * \param prc The pointer to the rectangle.
 * \param x x,y: The point.
 * \param y x,y: The point.
 */
static inline BOOL PtInRect(const RECT* prc, int x, int y)
{
    if (x >= prc->left && x < prc->right && y >= prc->top && y < prc->bottom)
        return TRUE;
    return FALSE;
}

/**
 * \fn BOOL GUIAPI IsRectEmpty (const RECT* prc)
 * \brief Determine whether an rectangle is empty.
 *
 * This function determines whether the specified rectangle \a prc is empty.
 * An empty rectangle is one that has no area; that is, the coordinates
 * of the right side is equal to the coordinate of the left side, or the
 * coordinates of the bottom side is equal to the coordinate of the top side.
 *
 * \param prc The pointer to the rectangle.
 * \return TRUE for empty, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI IsRectEmpty (const RECT* prc);

/**
 * \fn BOOL GUIAPI EqualRect (const RECT* prc1, const RECT* prc2)
 * \brief Determine whether two rectangles are equal.
 *
 * This function determines whether the two specified rectangles
 * (\a prc1 and \a prc2) are equal by comparing the coordinates of
 * the upper-left and lower-right corners.
 *
 * \param prc1 The pointers to the first rectangles.
 * \param prc2 The pointers to the second rectangles.
 * \return TRUE for equal, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI EqualRect (const RECT* prc1, const RECT* prc2);

/**
 * \fn void GUIAPI NormalizeRect (RECT* pRect)
 * \brief Normalizes a rectangle.
 *
 * This function normalizes the rectangle pointed to by \a prc
 * so that both the height and width are positive.
 *
 * \param pRect The pointer to the rectangle.
 */
MG_EXPORT void GUIAPI NormalizeRect (RECT* pRect);

/**
 * \fn BOOL GUIAPI IntersectRect (RECT* pdrc, \
                const RECT* psrc1, const RECT* psrc2)
 * \brief Calculates the intersection of two rectangles.
 *
 * This function calculates the intersection of two source rectangles
 * (\a psrc1 and \a psrc2) and places the coordinates of the intersection
 * rectangle into the destination rectangle pointed to by \a pdrc.
 * If the source rectangles do not intersect, and empty rectangle
 * (in which all coordinates are set to zero) is placed into the destination
 * rectangle.
 *
 * \param pdrc The pointer to the destination rectangle.
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 *
 * \return TRUE if the source rectangles intersect, otherwise FALSE.
 *
 * \sa DoesIntersect, IsCovered
 */
MG_EXPORT BOOL GUIAPI IntersectRect (RECT* pdrc,
                const RECT* psrc1, const RECT* psrc2);

/**
 * \fn BOOL GUIAPI IsCovered (const RECT* prc1, const RECT* prc2)
 * \brief Determine whether one rectangle is covered by another.
 *
 * This function determines whether one rectangle (\a prc1)
 * is covered by another rectangle (\a prc2).
 *
 * \param prc1 The first rectangles.
 * \param prc2 The second rectangles.
 *
 * \return TRUE if the first rectangle is covered by the second,
 *         otherwise FALSE.
 *
 * \sa DoesIntersect
 */
MG_EXPORT BOOL GUIAPI IsCovered (const RECT* prc1, const RECT* prc2);

/**
 * \fn BOOL GUIAPI DoesIntersect (const RECT* psrc1, const RECT* psrc2)
 * \brief Determine whether two rectangles intersect.
 *
 * This function determines whether two rectangles (\a psrc1 and \a psrc2)
 * intersect.
 *
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 * \return TRUE if the source rectangles intersect, otherwise FALSE.
 *
 * \sa IntersectRect
 */
MG_EXPORT BOOL GUIAPI DoesIntersect (const RECT* psrc1, const RECT* psrc2);

/**
 * \fn BOOL GUIAPI UnionRect (RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
 * \brief Unions two source rectangles.
 *
 * This function creates the union (\a pdrc) of two rectangles
 * (\a psrc1 and \a psrc2), if the source rectangles are border upon and
 * not stagger.
 *
 * \param pdrc The unioned rectangle.
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 *
 * \return TRUE if the source rectangles are border upon and not stagger,
 *         otherwise FALSE.
 *
 * \sa GetBoundRect
 */
MG_EXPORT BOOL GUIAPI UnionRect (RECT* pdrc,
                const RECT* psrc1, const RECT* psrc2);

/**
 * \fn void GUIAPI GetBoundRect (PRECT pdrc, \
                const RECT* psrc1, const RECT* psrc2)
 * \brief Get the bound rectangle of two source rectangles.
 *
 * This function creates the bound rect (\a pdrc) of two rectangles
 * (\a psrc1 and \a prsrc2). The bound rect is the smallest rectangle
 * that contains both source rectangles.
 *
 * \param pdrc The destination rectangle.
 * \param psrc1 The first source rectangle.
 * \param psrc2 The second source rectangle.
 *
 * \sa UnionRect
 */
MG_EXPORT void GUIAPI GetBoundRect (PRECT pdrc,
                const RECT* psrc1, const RECT* psrc2);

/**
 * \fn int GUIAPI SubtractRect (RECT* rc, const RECT* psrc1, const RECT* psrc2)
 * \brief Obtains the rectangles when substracting one rectangle from another.
 *
 * This function obtains the rectangles substracting the rectangle \a psrc1
 * from the other \a psrc2. \a rc should be an array of RECT struct, and
 * may contain at most four rectangles. This function returns
 * the number of result rectangles.
 *
 * \param rc The pointer to the resule rectangle array.
 * \param psrc1 The pointer to the minuend rectangle.
 * \param psrc2 The pointer to the subtrahend rectangle.
 * \return The number of result rectangles.
 *
 * \sa UnionRect
 */
MG_EXPORT int GUIAPI SubtractRect (RECT* rc, const RECT* psrc1, const RECT* psrc2);

/**
 * \def RECTWP(prc)
 * \brief Get the width of a RECT object by using the pointer to it.
 */
#define RECTWP(prc)  ((prc)->right - (prc)->left)
/**
 * \def RECTHP(prc)
 * \brief Get the height of a RECT object by using the pointer to it.
 */
#define RECTHP(prc)  ((prc)->bottom - (prc)->top)
/**
 * \def RECTW(rc)
 * \brief Get the width of a RECT object.
 */
#define RECTW(rc)    ((rc).right - (rc).left)
/**
 * \def RECTH(rc)
 * \brief Get the height of a RECT object.
 */
#define RECTH(rc)    ((rc).bottom - (rc).top)

    /** @} end of rect_fns */

    /**
     * \defgroup font_fns Logical font operations
     *
     * MiniGUI uses logical font to render text in a DC. You can
     * create a logical font by using \a CreateLogFont and select it
     * into a DC by using \a SelectFont, then you can use this logical
     * font to render text by using \a TextOutLen or \a DrawTextEx.
     *
     * @{
     */

/* Font-related structures */
#define LEN_LOGFONT_NAME_FIELD      31
#define LEN_LOGFONT_FAMILY_FIELD    (LEN_LOGFONT_NAME_FIELD*7+6)
#define LEN_LOGFONT_NAME_FULL       (LEN_LOGFONT_FAMILY_FIELD + 32)
#define LEN_UNIDEVFONT_NAME         255

#define LEN_FONT_NAME               LEN_LOGFONT_NAME_FIELD
#define LEN_DEVFONT_NAME            LEN_UNIDEVFONT_NAME

#define FONT_WEIGHT_NIL             '\0'
#define FONT_WEIGHT_ANY             '*'
#define FONT_WEIGHT_THIN            't'
#define FONT_WEIGHT_EXTRA_LIGHT     'e'
#define FONT_WEIGHT_LIGHT           'l'
#define FONT_WEIGHT_NORMAL          'n'
#define FONT_WEIGHT_REGULAR         'r'
#define FONT_WEIGHT_MEDIUM          'm'
#define FONT_WEIGHT_DEMIBOLD        'd'
#define FONT_WEIGHT_BOLD            'b'
#define FONT_WEIGHT_EXTRA_BOLD      'x'
#define FONT_WEIGHT_BLACK           'c'

/* Backward compatiblilty definitions.
 * deprecated since v4.0.0, use FS_RENDER_XXX/FONT_RENDER_XXX instead
 */
#define FONT_WEIGHT_SUBPIXEL        's'
#define FONT_WEIGHT_BOOK            'k'

#define FS_WEIGHT_MASK              0x000000FF
#define FS_WEIGHT_ANY               0
#define FS_WEIGHT_THIN              10
#define FS_WEIGHT_EXTRA_LIGHT       20
#define FS_WEIGHT_LIGHT             30
#define FS_WEIGHT_NORMAL            35
#define FS_WEIGHT_REGULAR           40
#define FS_WEIGHT_MEDIUM            50
#define FS_WEIGHT_DEMIBOLD          60
#define FS_WEIGHT_EXTRA_BOLD        70
#define FS_WEIGHT_BOLD              80
#define FS_WEIGHT_BLACK             90

#define FONT_SLANT_NIL              '\0'
#define FONT_SLANT_ANY              '*'
#define FONT_SLANT_ROMAN            'r'
#define FONT_SLANT_ITALIC           'i'
#define FONT_SLANT_OBLIQUE          'o'

#define FS_SLANT_MASK               0x00000F00
#define FS_SLANT_ANY                0x00000000
#define FS_SLANT_ROMAN              0x00000100
#define FS_SLANT_ITALIC             0x00000200
#define FS_SLANT_OBLIQUE            0x00000300

#define FONT_FLIP_NONE              'n'
#define FONT_FLIP_HORZ              'h'
#define FONT_FLIP_VERT              'v'
#define FONT_FLIP_HORZVERT          't'
#define FONT_FLIP_NIL               FONT_FLIP_NONE

#define FS_FLIP_MASK                0x0000F000
#define FS_FLIP_NONE                0x00000000
#define FS_FLIP_HORZ                0x00001000
#define FS_FLIP_VERT                0x00002000
#define FS_FLIP_HORZVERT            0x00003000 /* HORZ | VERT */

#define FONT_OTHER_NONE             'n'
#define FONT_OTHER_AUTOSCALE        's'
#define FONT_OTHER_TTFNOCACHE       'c'
#define FONT_OTHER_TTFKERN          'k'
#define FONT_OTHER_TTFNOCACHEKERN   'r'
#define FONT_OTHER_NIL              FONT_OTHER_NONE

#define FS_OTHER_MASK               0x00F00000
#define FS_OTHER_NONE               0x00000000
#define FS_OTHER_AUTOSCALE          0x00100000
#define FS_OTHER_TTFNOCACHE         0x00200000
#define FS_OTHER_TTFKERN            0x00400000
#define FS_OTHER_TTFNOCACHEKERN     0x00600000 /* KERN | NOCACHE */

#define FONT_DECORATE_NIL           '\0'
#define FONT_DECORATE_NONE          'n'
#define FONT_DECORATE_UNDERLINE     'u'
#define FONT_DECORATE_STRUCKOUT     's'
#define FONT_DECORATE_REVERSE       'r'
#define FONT_DECORATE_OUTLINE       'o'
#define FONT_DECORATE_US            'U' /* UNDERLINE | STRUCKOUT */

#define FS_DECORATE_MASK            0x0F000000
#define FS_DECORATE_NONE            0x00000000
#define FS_DECORATE_UNDERLINE       0x01000000
#define FS_DECORATE_STRUCKOUT       0x02000000
#define FS_DECORATE_REVERSE         0x04000000
#define FS_DECORATE_OUTLINE         0x08000000
#define FS_DECORATE_US              (FS_DECORATE_UNDERLINE | FS_DECORATE_STRUCKOUT)

#define FONT_RENDER_NIL             '\0'
#define FONT_RENDER_ANY             '*'
#define FONT_RENDER_MONO            'n'
#define FONT_RENDER_GREY            'g'
#define FONT_RENDER_SUBPIXEL        's'

#define FS_RENDER_MASK              0xF0000000
#define FS_RENDER_MONO              0x00000000
#define FS_RENDER_GREY              0x10000000
#define FS_RENDER_SUBPIXEL          0x20000000
#define FS_RENDER_ANY               FS_RENDER_MONO

/* Backward compatiblilty definitions; deprecated since v4.0.0 */
#define FONT_OTHER_LCDPORTRAIT      'p'
#define FONT_OTHER_LCDPORTRAITKERN  'q'

#define FONT_WEIGHT_ALL             FONT_WEIGHT_ANY
#define FONT_SLANT_ALL              FONT_SLANT_ANY
#define FS_WEIGHT_ALL               FS_WEIGHT_ANY
#define FS_SLANT_ALL                FS_SLANT_ANY

/* Backward compatiblilty definitions; deprecated since v4.0.0
 * All FONT_SETWIDTH_* and FONT_SPACING_* types will
 * be treated as FONT_FLIP_NIL and FONT_OTHER_NIL respectively.
 */
#define FONT_SETWIDTH_NIL           '\0'
#define FONT_SETWIDTH_ALL           '*'
#define FONT_SETWIDTH_BOLD          'b'
#define FONT_SETWIDTH_CONDENSED     'c'
#define FONT_SETWIDTH_SEMICONDENSED 's'
#define FONT_SETWIDTH_NORMAL        'n'
#define FONT_SETWIDTH_TTFNOCACHE    'w'

#define FONT_SPACING_NIL            '\0'
#define FONT_SPACING_ALL            '*'
#define FONT_SPACING_MONOSPACING    'm'
#define FONT_SPACING_PROPORTIONAL   'p'
#define FONT_SPACING_CHARCELL       'c'

#define FONT_UNDERLINE_NIL          '\0'
#define FONT_UNDERLINE_ALL          '*'
#define FONT_UNDERLINE_LINE         'u'
#define FONT_UNDERLINE_NONE         'n'

#define FONT_STRUCKOUT_NIL          '\0'
#define FONT_STRUCKOUT_ALL          '*'
#define FONT_STRUCKOUT_LINE         's'
#define FONT_STRUCKOUT_NONE         'n'
/******/

#define FONT_TYPE_NAME_LEN          3
#define FONT_TYPE_NAME_BITMAP_NUF   "nuf"
#define FONT_TYPE_NAME_BITMAP_RAW   "rbf"
#define FONT_TYPE_NAME_BITMAP_VAR   "vbf"
#define FONT_TYPE_NAME_BITMAP_QPF   "qpf"
#define FONT_TYPE_NAME_BITMAP_UPF   "upf"
#define FONT_TYPE_NAME_BITMAP_BMP   "bmp"
#define FONT_TYPE_NAME_SCALE_TTF    "ttf"
#define FONT_TYPE_NAME_SCALE_T1F    "t1f"
#define FONT_TYPE_NAME_ANY          "*"
#define FONT_TYPE_NAME_ALL          FONT_TYPE_NAME_ANY

#define FONT_ETC_SECTION_NAME_VBF   "varbitmapfonts"
#define FONT_ETC_SECTION_NAME_RBF   "rawbitmapfonts"
#define FONT_ETC_SECTION_NAME_UPF   "upf"
#define FONT_ETC_SECTION_NAME_QPF   "qpf"
#define FONT_ETC_SECTION_NAME_TTF   "truetypefonts"

#define FONT_TYPE_BITMAP_RAW        0x0001  /* "rbf" */
#define FONT_TYPE_BITMAP_VAR        0x0002  /* "vbf" */
#define FONT_TYPE_BITMAP_QPF        0x0003  /* "qpf" */
#define FONT_TYPE_BITMAP_BMP        0x0004  /* "bmp" */
#define FONT_TYPE_BITMAP_UPF        0x0005  /* "upf" */
#define FONT_TYPE_SCALE_TTF         0x0010  /* "ttf" */
#define FONT_TYPE_SCALE_T1F         0x0011  /* "t1f" */
#define FONT_TYPE_ANY               0x0000  /* "any" */
#define FONT_TYPE_ALL               FONT_TYPE_ANY

#define FONT_CHARSET_US_ASCII       "US-ASCII"

#define FONT_CHARSET_ISO8859_1      "ISO8859-1"
#define FONT_CHARSET_ISO8859_2      "ISO8859-2"
#define FONT_CHARSET_ISO8859_3      "ISO8859-3"
#define FONT_CHARSET_ISO8859_4      "ISO8859-4"
#define FONT_CHARSET_ISO8859_5      "ISO8859-5"
#define FONT_CHARSET_ISO8859_6      "ISO8859-6"
#define FONT_CHARSET_ISO8859_7      "ISO8859-7"
#define FONT_CHARSET_ISO8859_8      "ISO8859-8"
#define FONT_CHARSET_ISO8859_9      "ISO8859-9"
#define FONT_CHARSET_ISO8859_10     "ISO8859-10"
#define FONT_CHARSET_ISO8859_11     "ISO8859-11"
#define FONT_CHARSET_ISO8859_12     "ISO8859-12"
#define FONT_CHARSET_ISO8859_13     "ISO8859-13"
#define FONT_CHARSET_ISO8859_14     "ISO8859-14"
#define FONT_CHARSET_ISO8859_15     "ISO8859-15"
#define FONT_CHARSET_ISO8859_16     "ISO8859-16"

#define FONT_CHARSET_EUC_CN         "EUC-CN"

/**
 * \def FONT_CHARSET_GB1988_0
 * \brief EUC encoding of GB1988 charset, treat as ISO8859-1.
 */
#define FONT_CHARSET_GB1988_0       "GB1988-0"

/**
 * \def FONT_CHARSET_GB2312_0
 * \brief EUC encoding of GB2312 charset.
 */
#define FONT_CHARSET_GB2312_0       "GB2312-0"
#define FONT_CHARSET_GBK            "GBK"

/**
 * \def FONT_CHARSET_GB18030_0
 * \brief EUC encoding of GB18030 charset.
 */
#define FONT_CHARSET_GB18030_0      "GB18030-0"
#define FONT_CHARSET_BIG5           "BIG5"

/**
 * \def FONT_CHARSET_EUCTW
 * \brief EUC encoding of CNS11643 charset, not supported.
 */
#define FONT_CHARSET_EUCTW          "EUC-TW"
#define FONT_CHARSET_EUCKR          "EUC-KR"

/**
 * \def FONT_CHARSET_KSC5636_0
 * \brief EUC encoding of KSC5636 charset, treat as ISO8859-1.
 */
#define FONT_CHARSET_KSC5636_0      "KSC5636-0"

/**
 * \def FONT_CHARSET_KSC5601_0
 * \brief EUC encoding of KSC5601 charset.
 */
#define FONT_CHARSET_KSC5601_0      "KSC5601-0"
#define FONT_CHARSET_EUCJP          "EUC-JP"

/**
 * \def FONT_CHARSET_JISX0201_0
 * \brief EUC encoding of JISX0201 charset.
 */
#define FONT_CHARSET_JISX0201_0     "JISX0201-0"

/**
 * \def FONT_CHARSET_JISX0208_0
 * \brief EUC encoding of JISX0208 charset.
 */
#define FONT_CHARSET_JISX0208_0     "JISX0208-0"
#define FONT_CHARSET_SHIFTJIS       "SHIFT-JIS"

/**
 * \def FONT_CHARSET_JISX0201_1
 * \brief Shift-JIS encoding of JISX0201 charset
 */
#define FONT_CHARSET_JISX0201_1     "JISX0201-1"

/**
 * \def FONT_CHARSET_JISX0208_1
 * \brief Shift-JIS encoding of JISX0208 charset.
 */
#define FONT_CHARSET_JISX0208_1     "JISX0208-1"

/**
 * \def FONT_CHARSET_ISO_10646_1
 * \brief UCS-2 encoding of UNICODE.
 */
#define FONT_CHARSET_ISO_10646_1    "ISO-10646-1"

/**
 * \def FONT_CHARSET_UTF8
 * \brief UTF-8 encoding of UNICODE.
 */
#define FONT_CHARSET_UTF8           "UTF-8"

/**
 * \def FONT_CHARSET_UTF16LE
 * \brief UTF-16 encoding (Little-Endian) of UNICODE.
 */
#define FONT_CHARSET_UTF16LE        "UTF-16LE"

/**
 * \def FONT_CHARSET_UTF16BE
 * \brief UTF-16 encoding (Big-Endian) of UNICODE.
 */
#define FONT_CHARSET_UTF16BE        "UTF-16BE"

#define FONT_MAX_SIZE               256
#define FONT_MIN_SIZE               1
#define MAXNR_DEVFONTS              8

struct _DEVFONT;
typedef struct _DEVFONT DEVFONT;

/**
  * The logical font structure.
  * \note All fields are read-only.
  */
typedef struct _LOGFONT {
    /** The type of the logical font. */
    char* type;
    /** The family name of the logical font. */
    char* family;
    /** The charset of the logical font. */
    char* charset;
    /** The styles of the logical font. */
    DWORD32 style;
    /** The size of the logical font. */
    int  size;
    /** The rotation angle of the logical font. */
    int  rotation;
    /** The ascent of the logical font. */
    int  ascent;
    /** The descent of the logical font. */
    int  descent;
    /** The size requested initially. */
    int  size_request;

    /*
     * The following fields are internally used.
     * They may be changed in the future.
     */

    // The scale factors of devfonts
    unsigned short  scales[MAXNR_DEVFONTS];
    // The devfonts for the logfont
    DEVFONT*        devfonts[MAXNR_DEVFONTS];
} LOGFONT;

/**
 * \var typedef LOGFONT* PLOGFONT
 * \brief Data type of pointer to a LOGFONT.
 */
typedef LOGFONT* PLOGFONT;

/**
 * \var typedef LOGFONT* CPLOGFONT
 * \brief Data type of pointer to a LOGFONT.
 */
typedef const LOGFONT* CPLOGFONT;

struct _WORDINFO;

/**
 * \var typedef struct _WORDINFO WORDINFO
 * \brief Date type of _WORDINFO.
 */
typedef struct _WORDINFO WORDINFO;

#define MAX_LEN_MCHAR               6

typedef Uint16 Uchar16;
typedef Uint32 Uchar32;

    /**
     * \defgroup achar_types Abstract Character Types
     *
     * MiniGUI uses a Uint32 value to store the type information of a character.
     *
     * MiniGUI uses the lower 16-bit integer for the basic type of
     * an abstract character. The definition macros have ACHAR_BASIC_
     * as the prefix.
     *
     * For a UNICODE character, MiniGUI uses the high 16-bit for the
     * general UNICODE character categories and the possible line break
     * classifications. The low byte for the general category, and
     * the high byte for the break class.
     *
     * @{
     */

#define ACHARTYPE_BASIC_MASK        0x0000FFFF
#define ACHARTYPE_CATEGORY_MASK     0x00FF0000
#define ACHARTYPE_BREAK_MASK        0xFF000000

/**
 * \def ACHARTYPE_EXTRACT_BASIC
 * \brief Extract the basic type from an abstract character type.
 */
#define ACHARTYPE_EXTRACT_BASIC(t) \
        (((t) & ACHARTYPE_BASIC_MASK))

/**
 * \def ACHARTYPE_EXTRACT_CATEGORY
 * \brief Extract the UNICODE general category from an abstract character type.
 */
#define ACHARTYPE_EXTRACT_CATEGORY(t) \
        (((t) & ACHARTYPE_CATEGORY_MASK) >> 16)

/**
 * \def ACHARTYPE_EXTRACT_BREAK
 * \brief Extract the UNICODE break class from an abstract character type.
 */
#define ACHARTYPE_EXTRACT_BREAK(t) \
        (((t) & ACHARTYPE_BREAK_MASK) >> 24)

/**
 * \def ACHARTYPE_UNKNOWN
 * \brief Unknown abstract character type.
 */
#define ACHARTYPE_UNKNOWN           0

#define ACHAR_BASIC_NOSPACING_MARK   0x0001
#define ACHAR_BASIC_ABOVE_MARK       0x0002
#define ACHAR_BASIC_BELLOW_MARK      0x0004

#define ACHAR_BASIC_UNKNOWN          0x0000
#define ACHAR_BASIC_NUL              (0x0000 | ACHAR_BASIC_NOSPACING_MARK)
/** Control character (group 1) */
#define ACHAR_BASIC_CTRL1            0x0100
/** Bell */
#define ACHAR_BASIC_BEL              0x0200
/** Backspace */
#define ACHAR_BASIC_BS               0x0300
/** Horinzontal Tab */
#define ACHAR_BASIC_HT               0x0400
/** Line Feed */
#define ACHAR_BASIC_LF               0x0500
/** Vertical Tab */
#define ACHAR_BASIC_VT               0x0600
/** Form Feed */
#define ACHAR_BASIC_FF               0x0700
/** Carriage Return */
#define ACHAR_BASIC_CR               0x0800
/** Control character (group 2) */
#define ACHAR_BASIC_CTRL2            0x0900
/** Space */
#define ACHAR_BASIC_SPACE            0x0A00
/** Zero width character */
#define ACHAR_BASIC_ZEROWIDTH        0x0B00
#define ACHAR_BASIC_GENERIC          0x0C00

#define ACHAR_BASIC_LIGATURE         (0x0D00 | ACHAR_BASIC_NOSPACING_MARK)
#define ACHAR_BASIC_VOWEL            (0x0E00 | ACHAR_BASIC_NOSPACING_MARK)
#define ACHAR_BASIC_VOWEL_ABOVE      (ACHAR_BASIC_VOWEL | ACHAR_BASIC_ABOVE_MARK)
#define ACHAR_BASIC_VOWEL_BELLOW     (ACHAR_BASIC_VOWEL | ACHAR_BASIC_BELLOW_MARK)

/* Compatibility definitions */
#define MCHAR_TYPE_UNKNOWN          ACHAR_BASIC_UNKNOWN
#define MCHAR_TYPE_NUL              ACHAR_BASIC_NUL
#define MCHAR_TYPE_CTRL1            ACHAR_BASIC_CTRL1
#define MCHAR_TYPE_BEL              ACHAR_BASIC_BEL
#define MCHAR_TYPE_BS               ACHAR_BASIC_BS
#define MCHAR_TYPE_HT               ACHAR_BASIC_HT
#define MCHAR_TYPE_LF               ACHAR_BASIC_LF
#define MCHAR_TYPE_VT               ACHAR_BASIC_VT
#define MCHAR_TYPE_FF               ACHAR_BASIC_FF
#define MCHAR_TYPE_CR               ACHAR_BASIC_CR
#define MCHAR_TYPE_CTRL2            ACHAR_BASIC_CTRL2
#define MCHAR_TYPE_SPACE            ACHAR_BASIC_SPACE
#define MCHAR_TYPE_ZEROWIDTH        ACHAR_BASIC_ZEROWIDTH
#define MCHAR_TYPE_GENERIC          ACHAR_BASIC_GENERIC
#define MCHAR_TYPE_NOSPACING_MARK   ACHAR_BASIC_NOSPACING_MARK
#define MCHAR_TYPE_LIGATURE         ACHAR_BASIC_LIGATURE
#define MCHAR_TYPE_VOWEL            ACHAR_BASIC_VOWEL
#define MCHAR_TYPE_VOWEL_ABOVE      ACHAR_BASIC_VOWEL_ABOVE
#define MCHAR_TYPE_VOWEL_BELLOW     ACHAR_BASIC_VOWEL_BELLOW

#if 0
#define MCHAR_TYPE_ASCII            0x0080
#define MCHAR_TYPE_LATIN1           0x0081
#define MCHAR_TYPE_LATIN2           0x0082
#define MCHAR_TYPE_LATIN3           0x0083
#define MCHAR_TYPE_LATIN4           0x0084
#define MCHAR_TYPE_LATIN5           0x0085
#define MCHAR_TYPE_LATIN6           0x0086
#define MCHAR_TYPE_LATIN7           0x0087
#define MCHAR_TYPE_LATIN8           0x0088
#define MCHAR_TYPE_LATIN9           0x0089
#define MCHAR_TYPE_LATIN10          0x008A
#define MCHAR_TYPE_CYRILLIC         0x008B
#define MCHAR_TYPE_ARABIC           0x008C
#define MCHAR_TYPE_GREEK            0x008D
#define MCHAR_TYPE_HEBREW           0x008E
#define MCHAR_TYPE_THAI             0x008F

#define MCHAR_TYPE_DIGIT            0x0091
#define MCHAR_TYPE_SYMBOL_PUNCT     0x0092
#define MCHAR_TYPE_SYMBOL_MATH      0x0093
#define MCHAR_TYPE_SYMBOL_OTHER     0x0094
#define MCHAR_TYPE_FW_DIGIT         0x0095
#define MCHAR_TYPE_FW_SYM_PUNCT     0x0096
#define MCHAR_TYPE_FW_SYM_MATH      0x0097
#define MCHAR_TYPE_FW_SYM_OTHER     0x0098

#define MCHAR_TYPE_HANGUL           0x00A1
#define MCHAR_TYPE_KATAKANA         0x00A2
#define MCHAR_TYPE_CJK              0x00A3
#endif

/**
 * UCharGeneralCategory: General category of a UNICODE character.
 *
 * - UCHAR_CATEGORY_CONTROL: General category "Other, Control" (Cc)
 * - UCHAR_CATEGORY_FORMAT: General category "Other, Format" (Cf)
 * - UCHAR_CATEGORY_UNASSIGNED: General category "Other, Not Assigned" (Cn)
 * - UCHAR_CATEGORY_PRIVATE_USE: General category "Other, Private Use" (Co)
 * - UCHAR_CATEGORY_SURROGATE: General category "Other, Surrogate" (Cs)
 * - UCHAR_CATEGORY_LOWERCASE_LETTER: General category "Letter, Lowercase" (Ll)
 * - UCHAR_CATEGORY_MODIFIER_LETTER: General category "Letter, Modifier" (Lm)
 * - UCHAR_CATEGORY_OTHER_LETTER: General category "Letter, Other" (Lo)
 * - UCHAR_CATEGORY_TITLECASE_LETTER: General category "Letter, Titlecase" (Lt)
 * - UCHAR_CATEGORY_UPPERCASE_LETTER: General category "Letter, Uppercase" (Lu)
 * - UCHAR_CATEGORY_SPACING_MARK: General category "Mark, Spacing" (Mc)
 * - UCHAR_CATEGORY_ENCLOSING_MARK: General category "Mark, Enclosing" (Me)
 * - UCHAR_CATEGORY_NON_SPACING_MARK: General category "Mark, Nonspacing" (Mn)
 * - UCHAR_CATEGORY_DECIMAL_NUMBER: General category "Number, Decimal Digit" (Nd)
 * - UCHAR_CATEGORY_LETTER_NUMBER: General category "Number, Letter" (Nl)
 * - UCHAR_CATEGORY_OTHER_NUMBER: General category "Number, Other" (No)
 * - UCHAR_CATEGORY_CONNECT_PUNCTUATION: General category "Punctuation, Connector" (Pc)
 * - UCHAR_CATEGORY_DASH_PUNCTUATION: General category "Punctuation, Dash" (Pd)
 * - UCHAR_CATEGORY_CLOSE_PUNCTUATION: General category "Punctuation, Close" (Pe)
 * - UCHAR_CATEGORY_FINAL_PUNCTUATION: General category "Punctuation, Final quote" (Pf)
 * - UCHAR_CATEGORY_INITIAL_PUNCTUATION: General category "Punctuation, Initial quote" (Pi)
 * - UCHAR_CATEGORY_OTHER_PUNCTUATION: General category "Punctuation, Other" (Po)
 * - UCHAR_CATEGORY_OPEN_PUNCTUATION: General category "Punctuation, Open" (Ps)
 * - UCHAR_CATEGORY_CURRENCY_SYMBOL: General category "Symbol, Currency" (Sc)
 * - UCHAR_CATEGORY_MODIFIER_SYMBOL: General category "Symbol, Modifier" (Sk)
 * - UCHAR_CATEGORY_MATH_SYMBOL: General category "Symbol, Math" (Sm)
 * - UCHAR_CATEGORY_OTHER_SYMBOL: General category "Symbol, Other" (So)
 * - UCHAR_CATEGORY_LINE_SEPARATOR: General category "Separator, Line" (Zl)
 * - UCHAR_CATEGORY_PARAGRAPH_SEPARATOR: General category "Separator, Paragraph" (Zp)
 * - UCHAR_CATEGORY_SPACE_SEPARATOR: General category "Separator, Space" (Zs)
 *
 * These are the possible character classifications from the
 * Unicode specification.
 * See [Unicode Character Database](https://www.unicode.org/reports/tr44/#General_Category_Values).
 */
typedef enum {
    UCHAR_CATEGORY_CONTROL,
    UCHAR_CATEGORY_FORMAT,
    UCHAR_CATEGORY_UNASSIGNED,
    UCHAR_CATEGORY_PRIVATE_USE,
    UCHAR_CATEGORY_SURROGATE,
    UCHAR_CATEGORY_LOWERCASE_LETTER,
    UCHAR_CATEGORY_MODIFIER_LETTER,
    UCHAR_CATEGORY_OTHER_LETTER,
    UCHAR_CATEGORY_TITLECASE_LETTER,
    UCHAR_CATEGORY_UPPERCASE_LETTER,
    UCHAR_CATEGORY_SPACING_MARK,
    UCHAR_CATEGORY_ENCLOSING_MARK,
    UCHAR_CATEGORY_NON_SPACING_MARK,
    UCHAR_CATEGORY_DECIMAL_NUMBER,
    UCHAR_CATEGORY_LETTER_NUMBER,
    UCHAR_CATEGORY_OTHER_NUMBER,
    UCHAR_CATEGORY_CONNECT_PUNCTUATION,
    UCHAR_CATEGORY_DASH_PUNCTUATION,
    UCHAR_CATEGORY_CLOSE_PUNCTUATION,
    UCHAR_CATEGORY_FINAL_PUNCTUATION,
    UCHAR_CATEGORY_INITIAL_PUNCTUATION,
    UCHAR_CATEGORY_OTHER_PUNCTUATION,
    UCHAR_CATEGORY_OPEN_PUNCTUATION,
    UCHAR_CATEGORY_CURRENCY_SYMBOL,
    UCHAR_CATEGORY_MODIFIER_SYMBOL,
    UCHAR_CATEGORY_MATH_SYMBOL,
    UCHAR_CATEGORY_OTHER_SYMBOL,
    UCHAR_CATEGORY_LINE_SEPARATOR,
    UCHAR_CATEGORY_PARAGRAPH_SEPARATOR,
    UCHAR_CATEGORY_SPACE_SEPARATOR
} UCharGeneralCategory;

/**
 * UCharBreakType: The break type of a UNICODE character.
 *
 * - UCHAR_BREAK_MANDATORY: Mandatory Break (BK)
 * - UCHAR_BREAK_CARRIAGE_RETURN: Carriage Return (CR)
 * - UCHAR_BREAK_LINE_FEED: Line Feed (LF)
 * - UCHAR_BREAK_COMBINING_MARK: Attached Characters and Combining Marks (CM)
 * - UCHAR_BREAK_SURROGATE: Surrogates (SG)
 * - UCHAR_BREAK_ZERO_WIDTH_SPACE: Zero Width Space (ZW)
 * - UCHAR_BREAK_INSEPARABLE: Inseparable (IN)
 * - UCHAR_BREAK_NON_BREAKING_GLUE: Non-breaking ("Glue") (GL)
 * - UCHAR_BREAK_CONTINGENT: Contingent Break Opportunity (CB)
 * - UCHAR_BREAK_SPACE: Space (SP)
 * - UCHAR_BREAK_AFTER: Break Opportunity After (BA)
 * - UCHAR_BREAK_BEFORE: Break Opportunity Before (BB)
 * - UCHAR_BREAK_BEFORE_AND_AFTER: Break Opportunity Before and After (B2)
 * - UCHAR_BREAK_HYPHEN: Hyphen (HY)
 * - UCHAR_BREAK_NON_STARTER: Nonstarter (NS)
 * - UCHAR_BREAK_OPEN_PUNCTUATION: Opening Punctuation (OP)
 * - UCHAR_BREAK_CLOSE_PUNCTUATION: Closing Punctuation (CL)
 * - UCHAR_BREAK_QUOTATION: Ambiguous Quotation (QU)
 * - UCHAR_BREAK_EXCLAMATION: Exclamation/Interrogation (EX)
 * - UCHAR_BREAK_IDEOGRAPHIC: Ideographic (ID)
 * - UCHAR_BREAK_NUMERIC: Numeric (NU)
 * - UCHAR_BREAK_INFIX_SEPARATOR: Infix Separator (Numeric) (IS)
 * - UCHAR_BREAK_SYMBOL: Symbols Allowing Break After (SY)
 * - UCHAR_BREAK_ALPHABETIC: Ordinary Alphabetic and Symbol Characters (AL)
 * - UCHAR_BREAK_PREFIX: Prefix (Numeric) (PR)
 * - UCHAR_BREAK_POSTFIX: Postfix (Numeric) (PO)
 * - UCHAR_BREAK_COMPLEX_CONTEXT: Complex Content Dependent (South East Asian) (SA)
 * - UCHAR_BREAK_AMBIGUOUS: Ambiguous (Alphabetic or Ideographic) (AI)
 * - UCHAR_BREAK_UNKNOWN: Unknown (XX)
 * - UCHAR_BREAK_NEXT_LINE: Next Line (NL)
 * - UCHAR_BREAK_WORD_JOINER: Word Joiner (WJ)
 * - UCHAR_BREAK_HANGUL_L_JAMO: Hangul L Jamo (JL)
 * - UCHAR_BREAK_HANGUL_V_JAMO: Hangul V Jamo (JV)
 * - UCHAR_BREAK_HANGUL_T_JAMO: Hangul T Jamo (JT)
 * - UCHAR_BREAK_HANGUL_LV_SYLLABLE: Hangul LV Syllable (H2)
 * - UCHAR_BREAK_HANGUL_LVT_SYLLABLE: Hangul LVT Syllable (H3)
 * - UCHAR_BREAK_CLOSE_PARANTHESIS: Closing Parenthesis (CP).
 * - UCHAR_BREAK_CONDITIONAL_JAPANESE_STARTER: Conditional Japanese Starter (CJ).
 * - UCHAR_BREAK_HEBREW_LETTER: Hebrew Letter (HL).
 * - UCHAR_BREAK_REGIONAL_INDICATOR: Regional Indicator (RI).
 * - UCHAR_BREAK_EMOJI_BASE: Emoji Base (EB).
 * - UCHAR_BREAK_EMOJI_MODIFIER: Emoji Modifier (EM).
 * - UCHAR_BREAK_ZERO_WIDTH_JOINER: Zero Width Joiner (ZWJ).
 *
 * These are the possible line break classifications.
 *
 * Since new unicode versions may add new types here, applications should be
 * ready to handle unknown values. They may be regarded as %UCHAR_BREAK_UNKNOWN.
 *
 * See [Unicode Line Breaking Algorithm](https://www.unicode.org/unicode/reports/tr14/).
 */
typedef enum {
    UCHAR_BREAK_MANDATORY,
    UCHAR_BREAK_CARRIAGE_RETURN,
    UCHAR_BREAK_LINE_FEED,
    UCHAR_BREAK_COMBINING_MARK,
    UCHAR_BREAK_SURROGATE,
    UCHAR_BREAK_ZERO_WIDTH_SPACE,
    UCHAR_BREAK_INSEPARABLE,
    UCHAR_BREAK_NON_BREAKING_GLUE,
    UCHAR_BREAK_CONTINGENT,
    UCHAR_BREAK_SPACE,
    UCHAR_BREAK_AFTER,
    UCHAR_BREAK_BEFORE,
    UCHAR_BREAK_BEFORE_AND_AFTER,
    UCHAR_BREAK_HYPHEN,
    UCHAR_BREAK_NON_STARTER,
    UCHAR_BREAK_OPEN_PUNCTUATION,
    UCHAR_BREAK_CLOSE_PUNCTUATION,
    UCHAR_BREAK_QUOTATION,
    UCHAR_BREAK_EXCLAMATION,
    UCHAR_BREAK_IDEOGRAPHIC,
    UCHAR_BREAK_NUMERIC,
    UCHAR_BREAK_INFIX_SEPARATOR,
    UCHAR_BREAK_SYMBOL,
    UCHAR_BREAK_ALPHABETIC,
    UCHAR_BREAK_PREFIX,
    UCHAR_BREAK_POSTFIX,
    UCHAR_BREAK_COMPLEX_CONTEXT,
    UCHAR_BREAK_AMBIGUOUS,
    UCHAR_BREAK_UNKNOWN,
    UCHAR_BREAK_NEXT_LINE,
    UCHAR_BREAK_WORD_JOINER,
    UCHAR_BREAK_HANGUL_L_JAMO,
    UCHAR_BREAK_HANGUL_V_JAMO,
    UCHAR_BREAK_HANGUL_T_JAMO,
    UCHAR_BREAK_HANGUL_LV_SYLLABLE,
    UCHAR_BREAK_HANGUL_LVT_SYLLABLE,
    UCHAR_BREAK_CLOSE_PARANTHESIS,
    UCHAR_BREAK_CONDITIONAL_JAPANESE_STARTER,
    UCHAR_BREAK_HEBREW_LETTER,
    UCHAR_BREAK_REGIONAL_INDICATOR,
    UCHAR_BREAK_EMOJI_BASE,
    UCHAR_BREAK_EMOJI_MODIFIER,
    UCHAR_BREAK_ZERO_WIDTH_JOINER
} UCharBreakType;

/**
 * ScriptType: The script type of a UNICODE character.
 *
 * - SCRIPT_INVALID_CODE:
 *                      a value never returned from UCharGetScriptType()
 * - SCRIPT_COMMON:     a character used by multiple different scripts
 * - SCRIPT_INHERITED:  a mark glyph that takes its script from the
 *                           base glyph to which it is attached
 * - SCRIPT_ARABIC:     Arabic
 * - SCRIPT_ARMENIAN:   Armenian
 * - SCRIPT_BENGALI:    Bengali
 * - SCRIPT_BOPOMOFO:   Bopomofo
 * - SCRIPT_CHEROKEE:   Cherokee
 * - SCRIPT_COPTIC:     Coptic
 * - SCRIPT_CYRILLIC:   Cyrillic
 * - SCRIPT_DESERET:    Deseret
 * - SCRIPT_DEVANAGARI: Devanagari
 * - SCRIPT_ETHIOPIC:   Ethiopic
 * - SCRIPT_GEORGIAN:   Georgian
 * - SCRIPT_GOTHIC:     Gothic
 * - SCRIPT_GREEK:      Greek
 * - SCRIPT_GUJARATI:   Gujarati
 * - SCRIPT_GURMUKHI:   Gurmukhi
 * - SCRIPT_HAN:        Han
 * - SCRIPT_HANGUL:     Hangul
 * - SCRIPT_HEBREW:     Hebrew
 * - SCRIPT_HIRAGANA:   Hiragana
 * - SCRIPT_KANNADA:    Kannada
 * - SCRIPT_KATAKANA:   Katakana
 * - SCRIPT_KHMER:      Khmer
 * - SCRIPT_LAO:        Lao
 * - SCRIPT_LATIN:      Latin
 * - SCRIPT_MALAYALAM:  Malayalam
 * - SCRIPT_MONGOLIAN:  Mongolian
 * - SCRIPT_MYANMAR:    Myanmar
 * - SCRIPT_OGHAM:      Ogham
 * - SCRIPT_OLD_ITALIC: Old Italic
 * - SCRIPT_ORIYA:      Oriya
 * - SCRIPT_RUNIC:      Runic
 * - SCRIPT_SINHALA:    Sinhala
 * - SCRIPT_SYRIAC:     Syriac
 * - SCRIPT_TAMIL:      Tamil
 * - SCRIPT_TELUGU:     Telugu
 * - SCRIPT_THAANA:     Thaana
 * - SCRIPT_THAI:       Thai
 * - SCRIPT_TIBETAN:    Tibetan
 * - SCRIPT_CANADIAN_ABORIGINAL:
 *                            Canadian Aboriginal
 * - SCRIPT_YI:         Yi
 * - SCRIPT_TAGALOG:    Tagalog
 * - SCRIPT_HANUNOO:    Hanunoo
 * - SCRIPT_BUHID:      Buhid
 * - SCRIPT_TAGBANWA:   Tagbanwa
 * - SCRIPT_BRAILLE:    Braille
 * - SCRIPT_CYPRIOT:    Cypriot
 * - SCRIPT_LIMBU:      Limbu
 * - SCRIPT_OSMANYA:    Osmanya
 * - SCRIPT_SHAVIAN:    Shavian
 * - SCRIPT_LINEAR_B:   Linear B
 * - SCRIPT_TAI_LE:     Tai Le
 * - SCRIPT_UGARITIC:   Ugaritic
 * - SCRIPT_NEW_TAI_LUE:
 *                            New Tai Lue
 * - SCRIPT_BUGINESE:   Buginese
 * - SCRIPT_GLAGOLITIC: Glagolitic
 * - SCRIPT_TIFINAGH:   Tifinagh
 * - SCRIPT_SYLOTI_NAGRI:
 *                            Syloti Nagri
 * - SCRIPT_OLD_PERSIAN:
 *                            Old Persian
 * - SCRIPT_KHAROSHTHI: Kharoshthi
 * - SCRIPT_UNKNOWN:    an unassigned code point
 * - SCRIPT_BALINESE:   Balinese
 * - SCRIPT_CUNEIFORM:  Cuneiform
 * - SCRIPT_PHOENICIAN: Phoenician
 * - SCRIPT_PHAGS_PA:   Phags-pa
 * - SCRIPT_NKO:        N'Ko
 * - SCRIPT_KAYAH_LI:   Kayah Li.
 * - SCRIPT_LEPCHA:     Lepcha.
 * - SCRIPT_REJANG:     Rejang.
 * - SCRIPT_SUNDANESE:  Sundanese.
 * - SCRIPT_SAURASHTRA: Saurashtra.
 * - SCRIPT_CHAM:       Cham.
 * - SCRIPT_OL_CHIKI:   Ol Chiki.
 * - SCRIPT_VAI:        Vai.
 * - SCRIPT_CARIAN:     Carian.
 * - SCRIPT_LYCIAN:     Lycian.
 * - SCRIPT_LYDIAN:     Lydian.
 * - SCRIPT_AVESTAN:    Avestan.
 * - SCRIPT_BAMUM:      Bamum.
 * - SCRIPT_EGYPTIAN_HIEROGLYPHS: Egyptian Hieroglpyhs.
 * - SCRIPT_IMPERIAL_ARAMAIC: Imperial Aramaic.
 * - SCRIPT_INSCRIPTIONAL_PAHLAVI: Inscriptional Pahlavi.
 * - SCRIPT_INSCRIPTIONAL_PARTHIAN: Inscriptional Parthian.
 * - SCRIPT_JAVANESE:   Javanese.
 * - SCRIPT_KAITHI:     Kaithi.
 * - SCRIPT_LISU:       Lisu.
 * - SCRIPT_MEETEI_MAYEK: Meetei Mayek.
 * - SCRIPT_OLD_SOUTH_ARABIAN: Old South Arabian.
 * - SCRIPT_OLD_TURKIC: Old Turkic.
 * - SCRIPT_SAMARITAN:  Samaritan.
 * - SCRIPT_TAI_THAM:   Tai Tham.
 * - SCRIPT_TAI_VIET:   Tai Viet.
 * - SCRIPT_BATAK:      Batak.
 * - SCRIPT_BRAHMI:     Brahmi.
 * - SCRIPT_MANDAIC:    Mandaic.
 * - SCRIPT_CHAKMA:     Chakma.
 * - SCRIPT_MEROITIC_CURSIVE:     Meroitic Cursive.
 * - SCRIPT_MEROITIC_HIEROGLYPHS: Meroitic Hieroglyphs.
 * - SCRIPT_MIAO:                 Miao.
 * - SCRIPT_SHARADA:              Sharada.
 * - SCRIPT_SORA_SOMPENG:         Sora Sompeng.
 * - SCRIPT_TAKRI:                Takri.
 * - SCRIPT_BASSA_VAH:            Bassa.
 * - SCRIPT_CAUCASIAN_ALBANIAN:   Caucasian Albanian.
 * - SCRIPT_DUPLOYAN:             Duployan.
 * - SCRIPT_ELBASAN:              Elbasan.
 * - SCRIPT_GRANTHA:              Grantha.
 * - SCRIPT_KHOJKI:               Kjohki.
 * - SCRIPT_KHUDAWADI:            Khudawadi, Sindhi.
 * - SCRIPT_LINEAR_A:             Linear A.
 * - SCRIPT_MAHAJANI:             Mahajani.
 * - SCRIPT_MANICHAEAN:           Manichaean.
 * - SCRIPT_MENDE_KIKAKUI:        Mende Kikakui.
 * - SCRIPT_MODI:                 Modi.
 * - SCRIPT_MRO:                  Mro.
 * - SCRIPT_NABATAEAN:            Nabataean.
 * - SCRIPT_OLD_NORTH_ARABIAN:    Old North Arabian.
 * - SCRIPT_OLD_PERMIC:           Old Permic.
 * - SCRIPT_PAHAWH_HMONG:         Pahawh Hmong.
 * - SCRIPT_PALMYRENE:            Palmyrene.
 * - SCRIPT_PAU_CIN_HAU:          Pau Cin Hau.
 * - SCRIPT_PSALTER_PAHLAVI:      Psalter Pahlavi.
 * - SCRIPT_SIDDHAM:              Siddham.
 * - SCRIPT_TIRHUTA:              Tirhuta.
 * - SCRIPT_WARANG_CITI:          Warang Citi.
 * - SCRIPT_AHOM:                 Ahom.
 * - SCRIPT_ANATOLIAN_HIEROGLYPHS: Anatolian Hieroglyphs.
 * - SCRIPT_HATRAN:               Hatran.
 * - SCRIPT_MULTANI:              Multani.
 * - SCRIPT_OLD_HUNGARIAN:        Old Hungarian.
 * - SCRIPT_SIGNWRITING:          Signwriting.
 * - SCRIPT_ADLAM:                Adlam.
 * - SCRIPT_BHAIKSUKI:            Bhaiksuki.
 * - SCRIPT_MARCHEN:              Marchen.
 * - SCRIPT_NEWA:                 Newa.
 * - SCRIPT_OSAGE:                Osage.
 * - SCRIPT_TANGUT:               Tangut.
 * - SCRIPT_MASARAM_GONDI:        Masaram Gondi.
 * - SCRIPT_NUSHU:                Nushu.
 * - SCRIPT_SOYOMBO:              Soyombo.
 * - SCRIPT_ZANABAZAR_SQUARE:     Zanabazar Square.
 * - SCRIPT_DOGRA:                Dogra.
 * - SCRIPT_GUNJALA_GONDI:        Gunjala Gondi.
 * - SCRIPT_HANIFI_ROHINGYA:      Hanifi Rohingya.
 * - SCRIPT_MAKASAR:              Makasar.
 * - SCRIPT_MEDEFAIDRIN:          Medefaidrin.
 * - SCRIPT_OLD_SOGDIAN:          Old Sogdian.
 * - SCRIPT_SOGDIAN:              Sogdian.
 * - SCRIPT_ELYMAIC:              Elymaic.
 * - SCRIPT_NANDINAGARI:          Nandinagari.
 * - SCRIPT_NYIAKENG_PUACHUE_HMONGN: Nyiakeng Puachue Hmong.
 * - SCRIPT_WANCHO:               Wancho.
 *
 * This enumeration identifies different writing
 * systems. The values correspond to the names as defined in the
 * Unicode standard.
 *
 * Note that new types may be added in the future. Applications
 * should be ready to handle unknown values.
 * See [Unicode Standard Annex #24: Script names](https://www.unicode.org/reports/tr24/).
 */
typedef enum {
    SCRIPT_INVALID_CODE = 0xFF,
    SCRIPT_COMMON       = 0,   /* Zyyy */
    SCRIPT_INHERITED,          /* Zinh (Qaai) */
    SCRIPT_ARABIC,             /* Arab */
    SCRIPT_ARMENIAN,           /* Armn */
    SCRIPT_BENGALI,            /* Beng */
    SCRIPT_BOPOMOFO,           /* Bopo */
    SCRIPT_CHEROKEE,           /* Cher */
    SCRIPT_COPTIC,             /* Copt (Qaac) */
    SCRIPT_CYRILLIC,           /* Cyrl (Cyrs) */
    SCRIPT_DESERET,            /* Dsrt */
    SCRIPT_DEVANAGARI,         /* Deva */
    SCRIPT_ETHIOPIC,           /* Ethi */
    SCRIPT_GEORGIAN,           /* Geor (Geon, Geoa) */
    SCRIPT_GOTHIC,             /* Goth */
    SCRIPT_GREEK,              /* Grek */
    SCRIPT_GUJARATI,           /* Gujr */
    SCRIPT_GURMUKHI,           /* Guru */
    SCRIPT_HAN,                /* Hani */
    SCRIPT_HANGUL,             /* Hang */
    SCRIPT_HEBREW,             /* Hebr */
    SCRIPT_HIRAGANA,           /* Hira */
    SCRIPT_KANNADA,            /* Knda */
    SCRIPT_KATAKANA,           /* Kana */
    SCRIPT_KHMER,              /* Khmr */
    SCRIPT_LAO,                /* Laoo */
    SCRIPT_LATIN,              /* Latn (Latf, Latg) */
    SCRIPT_MALAYALAM,          /* Mlym */
    SCRIPT_MONGOLIAN,          /* Mong */
    SCRIPT_MYANMAR,            /* Mymr */
    SCRIPT_OGHAM,              /* Ogam */
    SCRIPT_OLD_ITALIC,         /* Ital */
    SCRIPT_ORIYA,              /* Orya */
    SCRIPT_RUNIC,              /* Runr */
    SCRIPT_SINHALA,            /* Sinh */
    SCRIPT_SYRIAC,             /* Syrc (Syrj, Syrn, Syre) */
    SCRIPT_TAMIL,              /* Taml */
    SCRIPT_TELUGU,             /* Telu */
    SCRIPT_THAANA,             /* Thaa */
    SCRIPT_THAI,               /* Thai */
    SCRIPT_TIBETAN,            /* Tibt */
    SCRIPT_CANADIAN_ABORIGINAL, /* Cans */
    SCRIPT_YI,                 /* Yiii */
    SCRIPT_TAGALOG,            /* Tglg */
    SCRIPT_HANUNOO,            /* Hano */
    SCRIPT_BUHID,              /* Buhd */
    SCRIPT_TAGBANWA,           /* Tagb */

    /* Unicode-4.0 additions */
    SCRIPT_BRAILLE,            /* Brai */
    SCRIPT_CYPRIOT,            /* Cprt */
    SCRIPT_LIMBU,              /* Limb */
    SCRIPT_OSMANYA,            /* Osma */
    SCRIPT_SHAVIAN,            /* Shaw */
    SCRIPT_LINEAR_B,           /* Linb */
    SCRIPT_TAI_LE,             /* Tale */
    SCRIPT_UGARITIC,           /* Ugar */

    /* Unicode-4.1 additions */
    SCRIPT_NEW_TAI_LUE,        /* Talu */
    SCRIPT_BUGINESE,           /* Bugi */
    SCRIPT_GLAGOLITIC,         /* Glag */
    SCRIPT_TIFINAGH,           /* Tfng */
    SCRIPT_SYLOTI_NAGRI,       /* Sylo */
    SCRIPT_OLD_PERSIAN,        /* Xpeo */
    SCRIPT_KHAROSHTHI,         /* Khar */

    /* Unicode-5.0 additions */
    SCRIPT_UNKNOWN,            /* Zzzz */
    SCRIPT_BALINESE,           /* Bali */
    SCRIPT_CUNEIFORM,          /* Xsux */
    SCRIPT_PHOENICIAN,         /* Phnx */
    SCRIPT_PHAGS_PA,           /* Phag */
    SCRIPT_NKO,                /* Nkoo */

    /* Unicode-5.1 additions */
    SCRIPT_KAYAH_LI,           /* Kali */
    SCRIPT_LEPCHA,             /* Lepc */
    SCRIPT_REJANG,             /* Rjng */
    SCRIPT_SUNDANESE,          /* Sund */
    SCRIPT_SAURASHTRA,         /* Saur */
    SCRIPT_CHAM,               /* Cham */
    SCRIPT_OL_CHIKI,           /* Olck */
    SCRIPT_VAI,                /* Vaii */
    SCRIPT_CARIAN,             /* Cari */
    SCRIPT_LYCIAN,             /* Lyci */
    SCRIPT_LYDIAN,             /* Lydi */

    /* Unicode-5.2 additions */
    SCRIPT_AVESTAN,                /* Avst */
    SCRIPT_BAMUM,                  /* Bamu */
    SCRIPT_EGYPTIAN_HIEROGLYPHS,   /* Egyp */
    SCRIPT_IMPERIAL_ARAMAIC,       /* Armi */
    SCRIPT_INSCRIPTIONAL_PAHLAVI,  /* Phli */
    SCRIPT_INSCRIPTIONAL_PARTHIAN, /* Prti */
    SCRIPT_JAVANESE,               /* Java */
    SCRIPT_KAITHI,                 /* Kthi */
    SCRIPT_LISU,                   /* Lisu */
    SCRIPT_MEETEI_MAYEK,           /* Mtei */
    SCRIPT_OLD_SOUTH_ARABIAN,      /* Sarb */
    SCRIPT_OLD_TURKIC,             /* Orkh */
    SCRIPT_SAMARITAN,              /* Samr */
    SCRIPT_TAI_THAM,               /* Lana */
    SCRIPT_TAI_VIET,               /* Tavt */

    /* Unicode-6.0 additions */
    SCRIPT_BATAK,                  /* Batk */
    SCRIPT_BRAHMI,                 /* Brah */
    SCRIPT_MANDAIC,                /* Mand */

    /* Unicode-6.1 additions */
    SCRIPT_CHAKMA,                 /* Cakm */
    SCRIPT_MEROITIC_CURSIVE,       /* Merc */
    SCRIPT_MEROITIC_HIEROGLYPHS,   /* Mero */
    SCRIPT_MIAO,                   /* Plrd */
    SCRIPT_SHARADA,                /* Shrd */
    SCRIPT_SORA_SOMPENG,           /* Sora */
    SCRIPT_TAKRI,                  /* Takr */

    /* Unicode 7.0 additions */
    SCRIPT_BASSA_VAH,              /* Bass */
    SCRIPT_CAUCASIAN_ALBANIAN,     /* Aghb */
    SCRIPT_DUPLOYAN,               /* Dupl */
    SCRIPT_ELBASAN,                /* Elba */
    SCRIPT_GRANTHA,                /* Gran */
    SCRIPT_KHOJKI,                 /* Khoj */
    SCRIPT_KHUDAWADI,              /* Sind */
    SCRIPT_LINEAR_A,               /* Lina */
    SCRIPT_MAHAJANI,               /* Mahj */
    SCRIPT_MANICHAEAN,             /* Manu */
    SCRIPT_MENDE_KIKAKUI,          /* Mend */
    SCRIPT_MODI,                   /* Modi */
    SCRIPT_MRO,                    /* Mroo */
    SCRIPT_NABATAEAN,              /* Nbat */
    SCRIPT_OLD_NORTH_ARABIAN,      /* Narb */
    SCRIPT_OLD_PERMIC,             /* Perm */
    SCRIPT_PAHAWH_HMONG,           /* Hmng */
    SCRIPT_PALMYRENE,              /* Palm */
    SCRIPT_PAU_CIN_HAU,            /* Pauc */
    SCRIPT_PSALTER_PAHLAVI,        /* Phlp */
    SCRIPT_SIDDHAM,                /* Sidd */
    SCRIPT_TIRHUTA,                /* Tirh */
    SCRIPT_WARANG_CITI,            /* Wara */

    /* Unicode 8.0 additions */
    SCRIPT_AHOM,                   /* Ahom */
    SCRIPT_ANATOLIAN_HIEROGLYPHS,  /* Hluw */
    SCRIPT_HATRAN,                 /* Hatr */
    SCRIPT_MULTANI,                /* Mult */
    SCRIPT_OLD_HUNGARIAN,          /* Hung */
    SCRIPT_SIGNWRITING,            /* Sgnw */

    /* Unicode 9.0 additions */
    SCRIPT_ADLAM,                  /* Adlm */
    SCRIPT_BHAIKSUKI,              /* Bhks */
    SCRIPT_MARCHEN,                /* Marc */
    SCRIPT_NEWA,                   /* Newa */
    SCRIPT_OSAGE,                  /* Osge */
    SCRIPT_TANGUT,                 /* Tang */

    /* Unicode 10.0 additions */
    SCRIPT_MASARAM_GONDI,          /* Gonm */
    SCRIPT_NUSHU,                  /* Nshu */
    SCRIPT_SOYOMBO,                /* Soyo */
    SCRIPT_ZANABAZAR_SQUARE,       /* Zanb */

    /* Unicode 11.0 additions */
    SCRIPT_DOGRA,                  /* Dogr */
    SCRIPT_GUNJALA_GONDI,          /* Gong */
    SCRIPT_HANIFI_ROHINGYA,        /* Rohg */
    SCRIPT_MAKASAR,                /* Maka */
    SCRIPT_MEDEFAIDRIN,            /* Medf */
    SCRIPT_OLD_SOGDIAN,            /* Sogo */
    SCRIPT_SOGDIAN,                /* Sogd */

    /* Unicode 12.0 additions */
    SCRIPT_ELYMAIC,                /* Elymaic */
    SCRIPT_NANDINAGARI,            /* Nandinagari */
    SCRIPT_NYIAKENG_PUACHUE_HMONG, /* Nyiakeng Puachue Hmong */
    SCRIPT_WANCHO,                 /* Wancho */
} ScriptType;

    /** @} end of achar_types */

    /**
     * \defgroup language_code Language Code
     *
     * The language code specifies the content language.
     *
     * @{
     */

/**
 * The language code.
 */
typedef enum {
    /** Unknown language code */
    LANGCODE_unknown = 0xFF,
    /** Language code for Afar */
    LANGCODE_aa = 0,
    /** Language code for Abkhazian */
    LANGCODE_ab,
    /** Language code for Afrikaans */
    LANGCODE_af,
    /** Language code for Amharic */
    LANGCODE_am,
    /** Language code for Arabic */
    LANGCODE_ar,
    /** Language code for Assamese */
    LANGCODE_as,
    /** Language code for Aymara */
    LANGCODE_ay,
    /** Language code for Azerbaijani */
    LANGCODE_az,
    /** Language code for Bashkir */
    LANGCODE_ba,
    /** Language code for Byelorussian */
    LANGCODE_be,
    /** Language code for Bulgarian */
    LANGCODE_bg,
    /** Language code for Bihari */
    LANGCODE_bh,
    /** Language code for Bislama */
    LANGCODE_bi,
    /** Language code for Bengali */
    LANGCODE_bn,
    /** Language code for Tibetan */
    LANGCODE_bo,
    /** Language code for Breton */
    LANGCODE_br,
    /** Language code for Catalan */
    LANGCODE_ca,
    /** Language code for Corsican */
    LANGCODE_co,
    /** Language code for Czech */
    LANGCODE_cs,
    /** Language code for Welch */
    LANGCODE_cy,
    /** Language code for Danish */
    LANGCODE_da,
    /** Language code for German */
    LANGCODE_de,
    /** Language code for Divehi */
    LANGCODE_dv,
    /** Language code for Bhutani */
    LANGCODE_dz,
    /** Language code for Greek */
    LANGCODE_el,
    /** Language code for English */
    LANGCODE_en,
    /** Language code for Esperanto */
    LANGCODE_eo,
    /** Language code for Spanish */
    LANGCODE_es,
    /** Language code for Estonian */
    LANGCODE_et,
    /** Language code for Basque */
    LANGCODE_eu,
    /** Language code for Persian */
    LANGCODE_fa,
    /** Language code for Finnish */
    LANGCODE_fi,
    /** Language code for Fiji */
    LANGCODE_fj,
    /** Language code for Faeroese */
    LANGCODE_fo,
    /** Language code for French */
    LANGCODE_fr,
    /** Language code for Frisian */
    LANGCODE_fy,
    /** Language code for Irish */
    LANGCODE_ga,
    /** Language code for Scots Gaelic */
    LANGCODE_gd,
    /** Language code for Galician */
    LANGCODE_gl,
    /** Language code for Guarani */
    LANGCODE_gn,
    /** Language code for Gujarati */
    LANGCODE_gu,
    /** Language code for Hausa */
    LANGCODE_ha,
    /** Language code for Hindi */
    LANGCODE_hi,
    /** Language code for Hebrew */
    LANGCODE_he,
    /** Language code for Croatian */
    LANGCODE_hr,
    /** Language code for Hungarian */
    LANGCODE_hu,
    /** Language code for Armenian */
    LANGCODE_hy,
    /** Language code for Interlingua */
    LANGCODE_ia,
    /** Language code for Indonesian */
    LANGCODE_id,
    /** Language code for Interlingue */
    LANGCODE_ie,
    /** Language code for Inupiak */
    LANGCODE_ik,
    /** Language code for former Indonesian */
    LANGCODE_in,
    /** Language code for Icelandic */
    LANGCODE_is,
    /** Language code for Italian */
    LANGCODE_it,
    /** Language code for Inuktitut (Eskimo) */
    LANGCODE_iu,
    /** Language code for former Hebrew */
    LANGCODE_iw,
    /** Language code for Japanese */
    LANGCODE_ja,
    /** Language code for former Yiddish */
    LANGCODE_ji,
    /** Language code for Javanese */
    LANGCODE_jw,
    /** Language code for Georgian */
    LANGCODE_ka,
    /** Language code for Kazakh */
    LANGCODE_kk,
    /** Language code for Greenlandic */
    LANGCODE_kl,
    /** Language code for Cambodian */
    LANGCODE_km,
    /** Language code for Kannada */
    LANGCODE_kn,
    /** Language code for Korean */
    LANGCODE_ko,
    /** Language code for Kashmiri */
    LANGCODE_ks,
    /** Language code for Kurdish */
    LANGCODE_ku,
    /** Language code for Kirghiz */
    LANGCODE_ky,
    /** Language code for Latin */
    LANGCODE_la,
    /** Language code for Lingala */
    LANGCODE_ln,
    /** Language code for Laothian */
    LANGCODE_lo,
    /** Language code for Lithuanian */
    LANGCODE_lt,
    /** Language code for Latvian, Lettish */
    LANGCODE_lv,
    /** Language code for Malagasy */
    LANGCODE_mg,
    /** Language code for Maori */
    LANGCODE_mi,
    /** Language code for Macedonian */
    LANGCODE_mk,
    /** Language code for Malayalam */
    LANGCODE_ml,
    /** Language code for Mongolian */
    LANGCODE_mn,
    /** Language code for Moldavian */
    LANGCODE_mo,
    /** Language code for Marathi */
    LANGCODE_mr,
    /** Language code for Malay */
    LANGCODE_ms,
    /** Language code for Maltese */
    LANGCODE_mt,
    /** Language code for Burmese */
    LANGCODE_my,
    /** Language code for Nauru */
    LANGCODE_na,
    /** Language code for Nepali */
    LANGCODE_ne,
    /** Language code for Dutch */
    LANGCODE_nl,
    /** Language code for Norwegian */
    LANGCODE_no,
    /** Language code for Occitan */
    LANGCODE_oc,
    /** Language code for (Afan) Oromo */
    LANGCODE_om,
    /** Language code for Oriya */
    LANGCODE_or,
    /** Language code for Punjabi */
    LANGCODE_pa,
    /** Language code for Polish */
    LANGCODE_pl,
    /** Language code for Pashto, Pushto */
    LANGCODE_ps,
    /** Language code for Portuguese */
    LANGCODE_pt,
    /** Language code for Quechua */
    LANGCODE_qu,
    /** Language code for Rhaeto-Romance */
    LANGCODE_rm,
    /** Language code for Kirundi */
    LANGCODE_rn,
    /** Language code for Romanian */
    LANGCODE_ro,
    /** Language code for Russian */
    LANGCODE_ru,
    /** Language code for Kinyarwanda */
    LANGCODE_rw,
    /** Language code for Sanskrit */
    LANGCODE_sa,
    /** Language code for Sindhi */
    LANGCODE_sd,
    /** Language code for Sangro */
    LANGCODE_sg,
    /** Language code for Serbo-Croatian */
    LANGCODE_sh,
    /** Language code for Singhalese */
    LANGCODE_si,
    /** Language code for Slovak */
    LANGCODE_sk,
    /** Language code for Slovenian */
    LANGCODE_sl,
    /** Language code for Samoan */
    LANGCODE_sm,
    /** Language code for Shona */
    LANGCODE_sn,
    /** Language code for Somali */
    LANGCODE_so,
    /** Language code for Albanian */
    LANGCODE_sq,
    /** Language code for Serbian */
    LANGCODE_sr,
    /** Language code for Siswati */
    LANGCODE_ss,
    /** Language code for Sesotho */
    LANGCODE_st,
    /** Language code for Sudanese */
    LANGCODE_su,
    /** Language code for Swedish */
    LANGCODE_sv,
    /** Language code for Swahili */
    LANGCODE_sw,
    /** Language code for Tamil */
    LANGCODE_ta,
    /** Language code for Tegulu */
    LANGCODE_te,
    /** Language code for Tajik */
    LANGCODE_tg,
    /** Language code for Thai */
    LANGCODE_th,
    /** Language code for Tigrinya */
    LANGCODE_ti,
    /** Language code for Turkmen */
    LANGCODE_tk,
    /** Language code for Tagalog */
    LANGCODE_tl,
    /** Language code for Setswana */
    LANGCODE_tn,
    /** Language code for Tonga */
    LANGCODE_to,
    /** Language code for Turkish */
    LANGCODE_tr,
    /** Language code for Tsonga */
    LANGCODE_ts,
    /** Language code for Tatar */
    LANGCODE_tt,
    /** Language code for Twi */
    LANGCODE_tw,
    /** Language code for Uigur */
    LANGCODE_ug,
    /** Language code for Ukrainian */
    LANGCODE_uk,
    /** Language code for Urdu */
    LANGCODE_ur,
    /** Language code for Uzbek */
    LANGCODE_uz,
    /** Language code for Vietnamese */
    LANGCODE_vi,
    /** Language code for Volapuk */
    LANGCODE_vo,
    /** Language code for Wolof */
    LANGCODE_wo,
    /** Language code for Xhosa */
    LANGCODE_xh,
    /** Language code for Yiddish */
    LANGCODE_yi,
    /** Language code for Yoruba */
    LANGCODE_yo,
    /** Language code for Zhuang */
    LANGCODE_za,
    /** Language code for Chinese */
    LANGCODE_zh,
    /** Language code for Zulu */
    LANGCODE_zu,
} LanguageCode;

/**
 * \fn LanguageCode GUIAPI LanguageCodeFromISO639s1 (Uint16 iso639_1)
 * \brief Get MiniGUI language code from an encoded ISO639-1 language code.
 *
 * Looks up the language code for ISO639-1. ISO639-1 assigns two-letter
 * codes to languages.  For example, the code for Arabic is 'ar'.
 * This function accepts two letter codes encoded as a Uint16 in a
 * big-endian fashion.  That is, the code expected for Arabic is
 * 0x6172 (0x61 is ASCII code for 'a', 0x72 is ASCII code for 'r', etc).
 *
 * \param iso639_1 An encoded language code in ISO639-1.
 * \return The language code for \a iso639_1, or \a LANGCODE_unknown if
 *          \a iso639_1 is invalid.
 *
 * Since: 4.0.0
 */
MG_EXPORT LanguageCode GUIAPI LanguageCodeFromISO639s1 (Uint16 iso639_1);

/**
 * \fn LanguageCode GUIAPI LanguageCodeFromISO639s1Code(const char* iso639_1)
 * \brief Get MiniGUI language code from an ISO639-1 language code string.
 *
 * Looks up the language code for ISO639-1. ISO639-1 assigns two-letter
 * codes to languages. For example, the code for Arabic is 'ar'.
 *
 * \param iso639_1 A language code in ISO639-1.
 * \return The language code for \a iso639_1, or \a LANGCODE_unknown if
 *          \a iso639_1 is invalid.
 *
 * Since: 4.0.0
 */
static inline LanguageCode GUIAPI LanguageCodeFromISO639s1Code(const char* iso639_1)
{
    return LanguageCodeFromISO639s1(MAKEWORD16(iso639_1[1],
            iso639_1[0]));
}

/** Get ISO639-1 language code from the native language code. */
MG_EXPORT const char* GUIAPI LanguageCodeToISO639s1(LanguageCode lc);

/** Get the sample language code (ISO639-1) from the specific script type */
MG_EXPORT LanguageCode GUIAPI GetSampleLanguageForScript(ScriptType st);

/** Get language code and scripts for ISO639 language name. */
MG_EXPORT const ScriptType* GUIAPI GetScriptsForLang(const char* lang_name,
        LanguageCode* lang_code, int* nr_scripts);

/** Normalize script type according to language code */
MG_EXPORT ScriptType GUIAPI NormalizeScriptType(LanguageCode cl,
    ScriptType ws);

    /** @} end of language_code */

struct _FONTOPS;
struct _CHARSETOPS;

typedef struct _FONTOPS FONTOPS;
typedef struct _CHARSETOPS CHARSETOPS;

/**
 * The device font structure.
 * \note All fields are read-only.
 */
struct _DEVFONT {
    /**
      * The device font name.
      * The family name supports aliases since 4.0.0:
      *
      *     <fonttype>-<family[,aliase]*>-<styles>-<width>-<height>-<charset[,charset]*>
      *
      * for example:
      *
      *     ttf-courier,monospace-rrncnn-0-0-ISO8859-1,UTF-8
      */
    char             name [LEN_UNIDEVFONT_NAME + 1];

    /** The styles of the device font. */
    DWORD32          style;

    /*
     * The following fields are internally used.
     * They may changed in the future.
     */
    // indicating if the data need to be unloaded before delete a devfont
    BOOL             need_unload;

    // The pointer to font operation structure.
    FONTOPS*         font_ops;

    // The pointer to character set operation structure.
    CHARSETOPS*      charset_ops;

    // The pointer to next device font.
    struct _DEVFONT* next;

    // The device font used data.
    void*            data;

    // The device font used relationship.
    void*            relationship;
};

/**
 * \def INV_LOGFONT
 * \brief Invalid logfont.
 */
#define INV_LOGFONT     0

/** The font metrics structure. */
typedef struct _FONTMETRICS {
    /** The height of the logical font. */
    int font_height;
    /** The ascent of the logical font. */
    int ascent;
    /** The descent of the logical font. */
    int descent;

    /** The maximal width of the logical font. */
    int max_width;
    /** The average width of the logical font. */
    int ave_width;
} FONTMETRICS;

/**
 * \fn void GUIAPI GetFontMetrics (LOGFONT* log_font, \
                FONTMETRICS* font_metrics)
 * \brief Get metrics information of a logical font.
 *
 * This function returns the font metrics information of the specified
 * logical font \a log_font.
 *
 * \param log_font The pointer to the logical font.
 * \param font_metrics The buffer receives the metrics information.
 * \return None.
 *
 * \sa GetGlyphInfo, FONTMETRICS
 */
MG_EXPORT void GUIAPI GetFontMetrics (LOGFONT* log_font,
                FONTMETRICS* font_metrics);

/** The glyph bitmap structure. */
typedef struct _GLYPHBITMAP {
    /** The bounding box of the glyph. */
    int bbox_x, bbox_y, bbox_w, bbox_h;
    /** The advance value of the glyph. */
    int advance_x, advance_y;

    /** The size of the glyph bitmap. */
    size_t bmp_size;
    /** The pitch of the glyph bitmap. */
    int bmp_pitch;
    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;
} GLYPHBITMAP;

/**
 * \fn void GUIAPI GetGlyphBitmap (LOGFONT* log_font, \
                const char* mchar, int mchar_len, \
                GLYPHBITMAP* glyph_bitmap)
 * \brief Get the glyph bitmap information when uses a logical font to
 *        output a multi-byte character.
 *
 * This function gets the glyph bitmap of one multi-byte character
 * (specified by \a mchar and \a mchar_len) and returns the bitmap information
 * through \a font_bitmap when using \a log_font to render the character.
 *
 * \param log_font The logical font used to render the character.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of the multi-byte character.
 * \param glyph_bitmap The buffer receives the glyph bitmap information.
 * \return None.
 *
 * Example:
 * \code
 *      GLYPHBITMAP glyph_bitmap = {0};
 *
 *      GetGlyphBitmap (log_font, "A", 1, &glyph_bitmap);
 * \endcode
 *
 * \note Deprecated. This function is a legacy implementation.
 *      New applications should use \a GetGlyphInfo instead.
 *
 * \sa GetFontMetrics, GLYPHBITMAP, GetGlyphInfo
 */
MG_EXPORT void GUIAPI GetGlyphBitmap (LOGFONT* log_font,
                const char* mchar, int mchar_len,
                GLYPHBITMAP* glyph_bitmap);

/**
 * \fn BOOL GUIAPI InitVectorialFonts (void)
 * \brief Initializes vectorial font renderer.
 *
 * Before 5.0.0, for the performance reason, under MiniGUI-Processes and
 * MiniGUI-Standalone runtime modes, MiniGUI does not load vector fonts,
 * such as TrueType or Adobe Type 1, at startup automatically.
 * This function initializes the vector font engines for MiniGUI-Processes
 * and MiniGUI-Standalone apps.  If you want to render text in vector fonts,
 * you must call this function to initialize the vector font engines.
 *
 * Since 5.0.0, MiniGUI always initialize the vector font engines at startup
 * if it was configured for all runtime modes.
 *
 * \return Always return TRUE.
 *
 * \sa TermVectorialFonts
 */
static inline BOOL GUIAPI InitVectorialFonts (void)
{
    _WRN_PRINTF("deprecated\n");
    return TRUE;
}

/**
 * \fn void GUIAPI TermVectorialFonts (void)
 * \brief Terminates vectorial font renderer.
 *
 * Before 5.0.0, this function terminates the vector font engines.
 * When you are done with the vector fonts, you should call this function
 * to unload the vector fonts to save memory.
 *
 * Since 5.0.0, MiniGUI always initialize the FreeType fonts at startup
 * if it was configured for all runtime modes. Therefore, this function
 * does nothing.
 *
 * \sa InitVectorialFonts
 */
static inline void GUIAPI TermVectorialFonts (void)
{
    _WRN_PRINTF("deprecated\n");
}

/**
 * \fn PLOGFONT GUIAPI CreateLogFont (const char* type, \
                const char* family, const char* charset, char weight, \
                char slant, char flip, char other, char underline, \
                char struckout, int size, int rotation)
 * \brief Creates a logical font.
 *
 * This function creates a logical font.
 *
 * \param type The type of the logical font, can be one of the values:
 *      - FONT_TYPE_NAME_BITMAP_RAW\n
 *        Creates a logical font by using raw bitmap device font, i.e.
 *        mono-space bitmap font.
 *      - FONT_TYPE_NAME_BITMAP_VAR\n
 *        Creates a logical font by using var-width bitmap device font.
 *      - FONT_TYPE_NAME_BITMAP_QPF\n
 *        Creates a logical font by using qpf device font.
 *      - FONT_TYPE_NAME_BITMAP_BMP\n
 *        Creates a logical font by using bitmap font.
 *      - FONT_TYPE_NAME_SCALE_TTF\n
 *        Creates a logical font by using scalable TrueType device font.
 *      - FONT_TYPE_NAME_SCALE_T1F\n
 *        Creates a logical font by using scalable Adobe Type1 device font.
 *      - FONT_TYPE_NAME_ANY\n
 *        Creates a logical font by using any type device font.
 * \param family The family of the logical font, such as "Courier",
 *        "Helvetica", and so on. Since version 4.0.0, you can specify
 *        up to 7 family names separated by comma, e.g.,
 *        "Helvetica,黑体,Naskh,SansSerif". Note that a family name should
 *        be encoded in UTF-8 and without special characters (middle spaces
 *        are allowed).
 * \param charset The charset of the logical font. You can specify a
 *        sigle-byte charset like "ISO8859-1", or a multi-byte charset
 *        like "UTF-8", or "GB2312-0".
 * \param weight The weight of the logical font, can be one of the values:
 *      - FONT_WEIGHT_ANY\n
 *        Any one.
 *      - FONT_WEIGHT_THIN\n
 *        Thin.
 *      - FONT_WEIGHT_EXTRA_LIGHT\n
 *        Extra light (Ultra light).
 *      - FONT_WEIGHT_LIGHT\n
 *        Light.
 *      - FONT_WEIGHT_NORMAL\n
 *        Normal.
 *      - FONT_WEIGHT_REGULAR\n
 *        Regular.
 *      - FONT_WEIGHT_MEDIUM\n
 *        Medium.
 *      - FONT_WEIGHT_DEMIBOLD\n
 *        Semi Bold (Demi Bold).
 *      - FONT_WEIGHT_BOLD\n
 *        Bold.
 *      - FONT_WEIGHT_EXTRA_BOLD\n
 *        Extra bold (Ultra Bold).
 *      - FONT_WEIGHT_BLACK\n
 *        Black (Heavy).
 *      - FONT_WEIGHT_BOOK\n
 *        For backforward compatibility only, same as FONT_WEIGHT_REGULAR and FONT_RENDER_GREY.
 *      - FONT_WEIGHT_SUBPIXEL\n
 *        For backforward compatibility only, same as FONT_WEIGHT_REGULAR and FONT_RENDER_SUBPIXEL.
 * \param slant The slant of the logical font, can be one of the values:
 *      - FONT_SLANT_ANY\n
 *        Any one.
 *      - FONT_SLANT_ROMAN\n
 *        Regular font.
 *      - FONT_SLANT_OBLIQUE\n
 *        Oblique font.
 * \param flip Does flip the glyph of the font, can be one of the following values:
 *      - FONT_FLIP_NONE\n
 *        Do not flip the glyph.
 *      - FONT_FLIP_HORZ\n
 *        Flip the glyph horizontally .
 *      - FONT_FLIP_VERT\n
 *        Flip the glyph vertically.
 *      - FONT_FLIP_HORZVERT\n
 *        Flip the glyph horizontally and vertically.
 * \param other Other rendering features, can be one of the following values:
 *      - FONT_OTHER_NONE\n
 *        Not specified.
 *      - FONT_OTHER_AUTOSCALE\n
 *        Auto scale the bitmap glyph to match the desired font size.
 *      - FONT_OTHER_TTFNOCACHE\n
 *        Do not use cache when using TrueType font.
 *      - FONT_OTHER_TTFKERN\n
 *        Kern the glyph when using TrueType font.
 *      - FONT_OTHER_TTFNOCACHEKERN\n
 *        When using TrueType font, kern the glyph and do not use cache.
 *      - FONT_OTHER_LCDPORTRAIT\n
 *        When using TrueType font and sub-pixels smoothing strategy,
 *        set LCD portrait and do not kern the glyph.
 *      - FONT_OTHER_LCDPORTRAITKERN\n
 *        When using TrueType font and sub-pixels smoothing strategy,
 *        set LCD portrait and kern the glyph.
 * \param underline The underline of the logical font, can be one of the values:
 *      - FONT_UNDERLINE_ALL\n
 *        Any one.
 *      - FONT_UNDERLINE_LINE\n
 *        With underline.
 *      - FONT_UNDERLINE_NONE\n
 *        Without underline.
 * \param struckout The struckout line of the logical font, can be one of
 *        the values:
 *      - FONT_STRUCKOUT_ALL\n
 *        Any one.
 *      - FONT_STRUCKOUT_LINE\n
 *        With struckout line.
 *      - FONT_STRUCKOUT_NONE\n
 *        Without struckout line.
 * \param size The size, i.e. the height, of the logical font. Note that
 *        the size of the created logical font may be different from the
 *        size expected.
 * \param rotation The rotation of the logical font, it is in units of
 *        tenth degrees. Note that you can specify rotation only for
 *        vector fonts (use FreeType 2 font engine).
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFontIndirect, CreateLogFontByName, SelectFont
 *
 * Example:
 *
 * \include createlogfont.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family,
        const char* charset, char weight, char slant, char flip,
        char other, char underline, char struckout,
        int size, int rotation);

/**
 * \fn PLOGFONT GUIAPI CreateLogFontEx (const char* type, \
                const char* family, const char* charset, char weight, \
                char slant, char flip, char other, char decoration, \
                char rendering, int size, int rotation)
 * \brief Creates a logical font.
 *
 * This function creates a logical font.
 *
 * \param type The type of the logical font, can be one of the values:
 *      - FONT_TYPE_NAME_BITMAP_RAW\n
 *        Creates a logical font by using raw bitmap device font, i.e.
 *        mono-space bitmap font.
 *      - FONT_TYPE_NAME_BITMAP_VAR\n
 *        Creates a logical font by using var-width bitmap device font.
 *      - FONT_TYPE_NAME_BITMAP_QPF\n
 *        Creates a logical font by using qpf device font.
 *      - FONT_TYPE_NAME_BITMAP_BMP\n
 *        Creates a logical font by using bitmap font.
 *      - FONT_TYPE_NAME_SCALE_TTF\n
 *        Creates a logical font by using scalable TrueType device font.
 *      - FONT_TYPE_NAME_SCALE_T1F\n
 *        Creates a logical font by using scalable Adobe Type1 device font.
 *      - FONT_TYPE_NAME_ANY\n
 *        Creates a logical font by using any type device font.
 * \param family The family of the logical font, such as "Courier",
 *        "Helvetica", and so on. Since version 4.0.0, you can specify
 *        up to 7 family names separated by comma, e.g.,
 *        "Helvetica,黑体,Naskh,SansSerif". Note that a family name should
 *        be encoded in UTF-8 and without special characters (middle spaces
 *        are allowed).
 * \param charset The charset of the logical font. You can specify a
 *        sigle-byte charset like "ISO8859-1", or a multi-byte charset
 *        like "GB2312-0" and "UTF-8".
 * \param weight The weight of the logical font, can be one of the values:
 *      - FONT_WEIGHT_ANY\n
 *        Any one.
 *      - FONT_WEIGHT_THIN\n
 *        Thin.
 *      - FONT_WEIGHT_EXTRA_LIGHT\n
 *        Extra light (Ultra light).
 *      - FONT_WEIGHT_LIGHT\n
 *        Light.
 *      - FONT_WEIGHT_NORMAL\n
 *        Normal.
 *      - FONT_WEIGHT_REGULAR\n
 *        Regular.
 *      - FONT_WEIGHT_MEDIUM\n
 *        Medium.
 *      - FONT_WEIGHT_DEMIBOLD\n
 *        Semi Bold (Demi Bold).
 *      - FONT_WEIGHT_BOLD\n
 *        Bold.
 *      - FONT_WEIGHT_EXTRA_BOLD\n
 *        Extra bold (Ultra Bold).
 *      - FONT_WEIGHT_BLACK\n
 *        Black (Heavy).
 * \param slant The slant of the logical font, can be one of the values:
 *      - FONT_SLANT_ANY\n
 *        Any one.
 *      - FONT_SLANT_ROMAN\n
 *        Regular font.
 *      - FONT_SLANT_ITALIC\n
 *        Italic font.
 *      - FONT_SLANT_OB\n
 *        Italic font.
 * \param flip Does flip the glyph of the font, can be one of the following values:
 *      - FONT_FLIP_NONE\n
 *        Do not flip the glyph.
 *      - FONT_FLIP_HORZ\n
 *        Flip the glyph horizontally .
 *      - FONT_FLIP_VERT\n
 *        Flip the glyph vertically.
 *      - FONT_FLIP_HORZVERT\n
 *        Flip the glyph horizontally and vertically.
 * \param other Other rendering features, can be one of the following values:
 *      - FONT_OTHER_NONE\n
 *        Not specified.
 *      - FONT_OTHER_AUTOSCALE\n
 *        Auto scale the bitmap glyph to match the desired font size.
 *      - FONT_OTHER_TTFNOCACHE\n
 *        Do not use cache when using TrueType font.
 *      - FONT_OTHER_TTFKERN\n
 *        Kern the glyph when using TrueType font.
 *      - FONT_OTHER_TTFNOCACHEKERN\n
 *        When using TrueType font, kern the glyph and do not use cache.
 *      - FONT_OTHER_LCDPORTRAIT\n
 *        When using TrueType font and sub-pixels smoothing strategy,
 *        set lcd portrait and do not use kern the glyph.
 *      - FONT_OTHER_LCDPORTRAITKERN\n
 *        When using TrueType font and sub-pixels smoothing strategy,
 *        set lcd portrait and use kern the glyph.
 * \param decoration The decoration (underline and/or struckout line)
          of the logical font, can be one of the values:
 *      - FONT_DECORATE_NONE\n
 *        Without underline and struckout line.
 *      - FONT_DECORATE_UNDERLINE\n
 *        With underline.
 *      - FONT_DECORATE_STRUCKOUT\n
 *        With struckout line.
 *      - FONT_DECORATE_BOTH\n
 *        With both underline and struckout line.
 * \param rendering The rendering type of the logical font, can be one of
 *        the values:
 *      - FONT_RENDER_ANY\n
 *        Any one (not specified).
 *      - FONT_RENDER_MONO\n
 *        Use a monochromatic bitmap.
 *      - FONT_RENDER_OUTLINE\n
 *        Render the glyph with an outline.
 *      - FONT_RENDER_GREY\n
 *        Render the glyph by using a gray bitmap.
 *      - FONT_RENDER_SUBPIXEL\n
 *        Use subpixel rendering method (TrueType only).
 * \param size The size, i.e. the height, of the logical font. Note that
 *        the size of the created logical font may be different from the
 *        size expected.
 * \param rotation The rotation of the logical font, it is in units of
 *        tenth degrees. Note that you can specify rotation only for
 *        TrueType and Adobe Type1 fonts.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFontIndirect, CreateLogFontByName, SelectFont
 *
 * Example:
 *
 * \include createlogfontex.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontEx (const char* type, const char* family,
        const char* charset, char weight, char slant, char flip,
        char other, char decoration, char rendering,
        int size, int rotation);

/**
 * \fn PLOGFONT GUIAPI CreateLogFontByName (const char* font_name)
 * \brief Creates a logical font by a font name.
 *
 * This function creates a logical font by a font name specified by
 * \a font_name. Note that since version 4.0.0, you can specify up
 * to 7 family names in the LOGFONT name, such as:
 *
 *      ttf-Courier,宋体,Naskh,SansSerif-rrncns-U-16-UTF-8
 *
 * In this way, you can specify a logfont to use multiple devfonts
 * to render a complex text. This is useful when different glyphs are
 * contained in different font files. It is well known that, a font is
 * often designed for a particular language/script or a few similar
 * languages/scripts.
 *
 * Since 4.0.0, the previous width field of a logfont name is used for
 * the glyph orientation:
 *
 *  - 'U': Glyphs stand upright (default).
 *  - 'S': Glyphs are rotated 90 degrees clockwise (sideways).
 *  - 'D': Glyphs are upside-down.
 *  - 'L': Glyphs are rotated 90 degrees counter-clockwise (sideways left).
 *
 * \param font_name The name of the logfont.
 *
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFont, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontByName(const char* font_name);

#ifdef _MGCHARSET_UNICODE
/**
 * \fn PLOGFONT GUIAPI CreateLogFontForMChar2UChar(const char* charset)
 * \brief Create a logical font for conversion from multi-byte character
 *      string to Uchar32 string.
 *
 * This function creates a logical font in order to convert a multi-byte
 * character string to Uchar32 string. You can use this logfont for
 * \a GetUCharsUntilParagraphBoundary.
 *
 * \param charset The charset name of the multi-byte character string.
 *
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa DestroyLogFont, GetUCharsUntilParagraphBoundary
 *
 * Since 4.0.0
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontForMChar2UChar(const char* charset);
#endif /* _MGCHARSET_UNICODE */

/**
 * \fn PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT* logfont)
 * \brief Creates a logical font indirectly from a LOGFONT structure.
 *
 * This function creates a logical font from information in a LOGFONT object.
 *
 * \param logfont The pointer to the reference logical font structure.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFont, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT* logfont);

/**
 * \fn PLOGFONT GUIAPI CreateLogFontIndirectEx (LOGFONT* logfont, int rotation)
 * \brief Creates a new logical font indirectly from a LOGFONT structure with
 * a rotation degrees.
 *
 * This function creates a new logical font from information in the LOGFONT object
 * \a logfont and the new rotation value \a rotation.
 *
 * Since 4.0.0.
 *
 * \param logfont The pointer to the reference logical font structure.
 * \param rotation The rotation of the logical font, it is in units of
 *        tenth degrees. Note that you can specify rotation only for
 *        vector fonts (use FreeType2 font engine).
 *
 * \return The pointer to the logical font created, NULL on error.
 *        If \a rotation is not zero and the devfonts of the \a logfont
 *        does not support rotation, it returns NULL.
 *
 * \sa CreateLogFont, CreateLogFontIndirect, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontIndirectEx (LOGFONT* logfont,
        int rotation);

/**
 * \fn void GUIAPI DestroyLogFont (PLOGFONT log_font)
 * \brief Destroys a logical font.
 *
 * This function destroys the specified logical font \a log_font.
 *
 * \param log_font The logical font will be destroied.
 *
 * \sa CreateLogFont, CreateLogFontIndirect
 */
MG_EXPORT void GUIAPI DestroyLogFont (PLOGFONT log_font);

/**
 * \fn void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font)
 * \brief Get logical font information of a DC.
 *
 * This function gets the logical font information of the specified DC \a hdc,
 * and copies to the LOGFONT structure pointed to by \a log_font.
 *
 * \param hdc The device context.
 * \param log_font The pointer to the LOGFONT structure to save the logical
 *        font info.
 *
 * \sa GetCurFont
 */
MG_EXPORT void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font);

/**
 * \fn PLOGFONT GUIAPI GetCurFont (HDC hdc)
 * \brief Get the pointer to the current logical font of a DC.
 *
 * This function returns the pointer to the current logical font selected to
 * the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The pointer to the current logical font.
 *
 * \sa GetLogFontInfo, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI GetCurFont (HDC hdc);

/**
 * \fn PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font)
 * \brief Selects a logical font into a DC.
 *
 * This function selects the specified logical font \a log_font into the
 * device context \a hdc.
 *
 * \param hdc The device context.
 * \param log_font The pointer to the logical font to be selected into the DC.
 * \return The old logical font of the DC.
 *
 * \sa GetCurFont
 *
 * Example:
 *
 * \include drawtext.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font);

/**
 * \fn const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font)
 * \brief Get next devfont information.
 *
 * This function can be used to enumerate all device fonts loaded by MiniGUI.
 *
 * \param dev_font The pointer to the current device font, can be NULL to get
 *                 the first device font.
 * \return The next device font.
 */
MG_EXPORT const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font);

#ifdef _MGFONT_FT2

/**
 * List of values to identify various types of LCD filters,
 * Note a freetype2 specific definition.
 */
typedef enum {
    /** equal to FT_LCD_FILTER_NONE    */
    MG_SMOOTH_NONE = 0,
    /** equal to FT_LCD_FILTER_DEFAULT */
    MG_SMOOTH_DEFAULT = 1,
    /** equal to FT_LCD_FILTER_LIGHT */
    MG_SMOOTH_LIGHT = 2,
    /** equal to FT_LCD_FILTER_LEGACY */
    MG_SMOOTH_LEGACY = 16,
    /** equal to FT_LCD_FILTER_MAX */
    MG_SMOOTH_MAX    /*do not remove*/
} FT2LCDFilter;

/**
 * \fn BOOL ft2SetLcdFilter(LOGFONT* logfont, FT2LCDFilter filter)
 *
 * \brief Set freetype2 smooth mode.
 *
 * This function can be used to set freetype2 library handle font smooth mode.
 *
 * \param logfont The logical font.
 *
 * \param filter The handle font smooth mode, it must be a value of FT2LCDFilter.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa FT2LCDFilter
 */
MG_EXPORT BOOL GUIAPI ft2SetLcdFilter (LOGFONT* logfont, FT2LCDFilter filter);

#endif

/**
 * \fn DEVFONT* GUIAPI LoadDevFontFromIncoreData (const char* devfont_name, \
 *        const void* data)
 * \brief Load device font from incore data.
 *
 * This function can be used to load device font from incore data.
 *
 * \param devfont_name The device font name.
 * \param data The pointer to the incore font data.
 *
 * \return the pointer to the new device font on success, NULL on error.
 *
 * \sa GetNextDevFont, DestroyDynamicDevFont
 */
MG_EXPORT DEVFONT* GUIAPI LoadDevFontFromIncoreData (const char *devfont_name,
        const void *data);

/**
 * \fn DEVFONT* GUIAPI LoadDevFontFromFile (const char* devfont_name, \
 *        const char* file_name)
 * \brief Load device font from font file.
 *
 * This function can be used to load device font from font file.Now,it support
 * free type font file and qpf font file.
 *
 * \param devfont_name The device font name.
 *
 * \param file_name The font file name.
 *
 * \return the pointer to the new device font on success, NULL on error.
 *
 * \sa GetNextDevFont, DestroyDynamicDevFont
 */
MG_EXPORT DEVFONT* GUIAPI LoadDevFontFromFile (const char *devfont_name,
        const char *file_name);

/**
 * \fn void GUIAPI DestroyDynamicDevFont (DEVFONT **devfont)
 * \brief Destroy device font loaded dynamically from font file.
 *
 * \param devfont The double pointer to DEVFONT structure. Note that the pointer
 *        must be the double pointer to device font loaded by LoadDevFontFromFile.
 *
 * \sa LoadDevFontFromFile
 */
MG_EXPORT void GUIAPI DestroyDynamicDevFont (DEVFONT **devfont);

    /** @} end of font_fns */

    /**
     * \defgroup system_font System charset and font operations
     *
     * MiniGUI creates a few system fonts to draw menu text, window caption, or
     * other general items. MiniGUI at least creates two system fonts: one
     * mono-space logical font for single-byte charset, and one mono-space
     * logical font for multi-byte charset. For the multi-byte charset,
     * the width of one multi-byte character should be equal to the width of
     * two single-byte characters.
     *
     * @{
     */
/**
 * \def SYSLOGFONT_DEFAULT
 * \sa GetSystemFont
 */
#define SYSLOGFONT_DEFAULT          0

/**
 * \def SYSLOGFONT_WCHAR_DEF
 * \sa GetSystemFont
 */
#define SYSLOGFONT_WCHAR_DEF        1

/**
 * \def SYSLOGFONT_FIXED
 * \sa GetSystemFont
 */
#define SYSLOGFONT_FIXED            2

/**
 * \def SYSLOGFONT_CAPTION
 * \sa GetSystemFont
 */
#define SYSLOGFONT_CAPTION          3

/**
 * \def SYSLOGFONT_MENU
 * \sa GetSystemFont
 */
#define SYSLOGFONT_MENU             4

/**
 * \def SYSLOGFONT_CONTROL
 * \sa GetSystemFont
 */
#define SYSLOGFONT_CONTROL          5

#define NR_SYSLOGFONTS              6


extern MG_EXPORT PLOGFONT g_SysLogFont [];

/**
 * \fn PLOGFONT GUIAPI GetSystemFont (int font_id)
 * \brief Get the system logical font through an font identifier.
 *
 * This function returns the system logical font through the font
 * identifier \a font_id.
 *
 * \param font_id The identifier of a system font, can be one of
 *        the following values:
 *      - SYSLOGFONT_DEFAULT\n
 *        The default system logical font in single-byte charset, must be rbf.
 *      - SYSLOGFONT_WCHAR_DEF\n
 *        The default system logical font in multi-byte charset, must be rbf.
 *      - SYSLOGFONT_FIXED\n
 *        The fixed space system logical font.
 *      - SYSLOGFONT_CAPTION\n
 *        The system logical font used to draw caption text.
 *      - SYSLOGFONT_MENU\n
 *        The system logical font used to draw menu items.
 *      - SYSLOGFONT_CONTROL\n
 *        The system logical font used to draw controls.
 * \return The pointer to the system logical font. NULL on error.
 */
static inline PLOGFONT GUIAPI GetSystemFont (int font_id)
{
    if (font_id >= 0 && font_id < NR_SYSLOGFONTS)
        return g_SysLogFont [font_id];
    else
        return (PLOGFONT)NULL;
}

/**
 * \fn int GUIAPI GetSysFontMaxWidth (int font_id)
 * \brief Get the maximal width of a single-byte character of a system font.
 *
 * This function returns the maximal width of a single-byte character of
 * one system font.
 *
 * \param font_id The identifier of a system font.
 *
 * \return The maximal width of single-byte character of the default
 *         system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontMaxWidth (int font_id);

/**
 * \fn int GUIAPI GetSysFontAveWidth (int font_id)
 * \brief Get the average width of a single-byte character of a system font.
 *
 * This function returns the average width of a single-byte character of
 * one system font.
 *
 * \param font_id The identifier of a system font.
 *
 * \return The average width of single-byte character of the default
 *         system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontAveWidth (int font_id);

/**
 * \fn int GUIAPI GetSysFontHeight (int font_id)
 * \brief Get the height of a single-byte character of a system font.
 *
 * This function returns the height of a single-byte character of one
 * system font.
 *
 * \param font_id The identifier of a system font.
 *
 * \return The height of single-byte character of the default system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontHeight (int font_id);

/**
 * \fn const char* GUIAPI GetSysCharset (BOOL wchar)
 * \brief Get the current system charset.
 *
 * This function gets the current system charset and returns the charset name.
 * By default, the system charset is ISO8859-1 (for single-byte charset) or
 * GB2312.1980-0 (for wide charset), but you can change it by modifying
 * \a MiniGUI.cfg.
 *
 * \param wchar Whether to retrieve the wide charset supported currently.
 * \return The read-only buffer of charset name. If you pass \a wchar TRUE,
 *         This function may return NULL, if there is not any wide
 *         charset supported.
 */
MG_EXPORT const char* GUIAPI GetSysCharset (BOOL wchar);

/**
 * \fn int GUIAPI GetSysCharHeight (void)
 * \brief Get the height of a character of the default system font.
 *
 * This function returns the height of a character of the system default font.
 * MiniGUI uses mono-space font as the default system font.
 *
 * Normally, the width of one multi-byte character is equal to
 * the width of two single-byte character when using the default system font.
 *
 * \return The character height of the default system font.
 *
 * \sa GetSysCharWidth, GetSysCCharWidth
 */
MG_EXPORT int GUIAPI GetSysCharHeight (void);

/**
 * \fn int GUIAPI GetSysCharWidth (void)
 * \brief Get the width of a single-byte character of the default system font.
 *
 * This function returns the width of a single-byte character of the
 * default system font. MiniGUI uses mono-space font as the default
 * system font, but you can specify a different font to output text
 * in windows of MiniGUI.
 *
 * \return The width of single-byte character of the default system font.
 *
 * \sa GetSysCharHeight, GetSysCCharWidth, SelectFont, CreateLogFont
 */
MG_EXPORT int GUIAPI GetSysCharWidth (void);

/**
 * \fn int GUIAPI GetSysCCharWidth (void)
 * \brief Get the width of a multi-byte character of the default system font.
 *
 * This function returns the width of a multi-byte character of the default
 * system font. MiniGUI uses mono-space font as the system default font.
 *
 * \return The width of multi-byte character of the default system font.
 *
 * \sa GetSysCharHeight, GetSysCharWidth
 */
MG_EXPORT int GUIAPI GetSysCCharWidth (void);

    /** @} end of system_font */

    /**
     * \defgroup text_parse_fns Text parsing functions
     *
     * MiniGUI uses logical font to output text, also uses
     * logical font to parsing the multi-byte characters and
     * words in the text.
     *
     * @{
     */

/** Word information structure */
struct _WORDINFO
{
    /** The length of the word. */
    int len;
    /** The delimiter after the word. */
    unsigned char delimiter;
    /** The number of delimiters after the word. */
    int nr_delimiters;
};

/**
 * \fn int GUIAPI GetTextMCharInfo (PLOGFONT log_font, \
                const char* mstr, int len, int* pos_chars)
 * \brief Retrieve positions of multi-byte characters in a string.
 *
 * This function retrieves position of multi-byte characters in
 * the string \a mstr which is \a len bytes long. It returns the positions
 * of characters in the string in an integer array pointed to by \a pos_chars.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param pos_chars The position of characters will be returned through
 *        this array if it is not NULL.
 *
 * \return The number of characters in the multi-byte string.
 *
 * \sa GetTextWordInfo
 */
MG_EXPORT int GUIAPI GetTextMCharInfo (PLOGFONT log_font,
                const char* mstr, int len, int* pos_chars);

/**
 * \fn int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr, \
                int len, int* pos_words, WORDINFO* info_words)
 * \brief Retrieve information of multi-byte words in a string.
 *
 * This function retrieves information of multi-byte words in the string
 * \a mstr which is \a len bytes long. It returns the positions of words
 * in the string in an integer array pointed to by \a pos_words if the
 * pointer is not NULL. This function also returns the information of words
 * in a WORDINFO struct array pointed to by \a info_words when the pointer
 * is not NULL.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param pos_words The position of words will be returned through
 *        this array if it is not NULL.
 * \param info_words The words' information will be returned through
 *        this array if it is not NULL.
 * \return The number of words in the multi-byte string.
 *
 * \sa GetTextMCharInfo, WORDINFO
 */
MG_EXPORT int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr,
                int len, int* pos_words, WORDINFO* info_words);

/**
 * \fn int GUIAPI GetFirstMCharLen (PLOGFONT log_font, \
                const char* mstr, int len)
 * \brief Retrieve the length of the first multi-byte character in a string.
 *
 * This function retrieves and returns the length of the first multi-byte
 * character in the string \a mstr which is \a len bytes long.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \return The length of the first multi-byte character.
 *
 * \sa GetFirstWord
 */
MG_EXPORT int GUIAPI GetFirstMCharLen (PLOGFONT log_font,
                const char* mstr, int len);

/**
 * \fn int GUIAPI GetFirstWord (PLOGFONT log_font, \
                const char* mstr, int len, WORDINFO* word_info)
 * \brief Retrieve the length and info of the first multi-byte word in a string.
 *
 * This function retrieves the information of the first multi-byte character
 * in the string \a mstr which is \a len bytes long, and returns it through
 * \a word_info. It also returns the full length of the word including
 * the delimiters after the word.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param word_info The pointer to a WORDINFO structure used to return
 *        the word information.
 *
 * \return The length of the first multi-byte word.
 *
 * \sa GetFirstMCharLen, WORDINFO
 */
MG_EXPORT int GUIAPI GetFirstWord (PLOGFONT log_font,
                const char* mstr, int len, WORDINFO* word_info);

/**
 * \fn int GUIAPI GetTextExtentPoint (HDC hdc, const char* text, int len, \
                int max_extent, int* fit_chars, int* pos_chars,  \
                int* dx_chars, SIZE* size)
 * \brief Computes the extent of a string when output the string in a
 *        limited space.
 *
 * This function computes the extent of the specified string of text \a text
 * which is \a len bytes long when output the text in a limited space
 * (\a max_extent wide). If \a pos_chars and \a dx_chars are not NULL,
 * this function will return the positions of each character in the text,
 * and the output position of each character.  This function returns the
 * text extent in a SIZE struct pointed to by \a size, and the width of
 * text as return value.
 *
 * \param hdc The device context.
 * \param text The multi-byte string.
 * \param len The length of the string.
 * \param max_extent The width of the limited space.
 * \param fit_chars The number of the characters actually outputed.
 * \param pos_chars The positions of each character in the text will be
 *        returned through this pointer.
 * \param dx_chars The output positions of each character in the text will be
 *        returned through this pointer.
 * \param size The output extent of the text in the limited space will be
 *        returned through this pointer.
 *
 * \return The number of the characters which can be fit to the limited space.
 *
 * \sa GetFirstMCharLen, GetFirstWord
 */
MG_EXPORT int GUIAPI GetTextExtentPoint (HDC hdc, const char* text, int len,
                int max_extent, int* fit_chars, int* pos_chars,
                int* dx_chars, SIZE* size);

/**
 * \fn int GUIAPI GetTabbedTextExtentPoint (HDC hdc, \
                const char* text, int len, int max_extent, \
                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
 *
 * \brief Computes the extent of a string when output the formatted string
 *        in a limited space.
 */
MG_EXPORT int GUIAPI GetTabbedTextExtentPoint (HDC hdc,
                const char* text, int len, int max_extent,
                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size);

#ifdef _MGCHARSET_UNICODE

/**
 * \fn int GUIAPI MB2WCEx (PLOGFONT log_font, void* dest, BOOL wc32, \
 *              const unsigned char* mstr, int n)
 *
 * \brief Converts a multibyte character to a wide character in UCS
 *        according to the charset/encoding of the logical font.
 *
 * \param log_font The logical font.
 * \param dest The buffer used to store the wide character; can be NULL.
 * \param wc32 Whether the wide char is 32-bit long. TRUE for yes, FALSE
 *        for 16-bit long.
 * \param mstr The pointer to the multi-byte character.
 * \param n The length of the multi-byte character.
 *
 * \return If mchar is not NULL, the function returns the number of consumed
 *        bytes starting at mchar, or 0 if s points to a null byte,
 *        or -1 upon  failure.
 *
 * \sa WC2MBEx, mbtowc
 */
MG_EXPORT int GUIAPI MB2WCEx (PLOGFONT log_font, void* dest, BOOL wc32,
                const unsigned char* mstr, int n);

/**
 * \def MB2WC(log_font, dest, mstr, n)
 * \brief The backward compatibility version of MB2WCEx.
 *
 * \sa MB2WCEx
 */
#define MB2WC(log_font, dest, mstr, n) \
                   MB2WCEx (log_font, dest, sizeof(wchar_t) == 4, mstr, n)

/**
 * \fn int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, Uchar32 wc)
 *
 * \brief Converts a wide character in UCS to a multibyte character
 *        according to the charset/encoding of the logical font.
 *
 * \param log_font The logical font.
 * \param s The buffer used to store the converted multibyte characters.
 * \param wc The wide character.
 *
 * \return If s is not NULL, the function returns the number of bytes that
 *        have been written to the byte array at s. If wc can not be
 *        represented as a multibyte sequence (according to
 *        the current logfont),  -1 is returned.
 *
 * \sa WC2MB, mbtowc
 */
MG_EXPORT int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, Uchar32 wc);

/**
 * \def WC2MB(log_font, dest, mstr, n)
 * \brief The backward compatibility version of WC2MBEx.
 *
 * \sa WC2MBEx
 */
#define WC2MB(log_font, s, wc) \
                   WC2MBEx (log_font, s, (Uchar32)wc)

/**
 * \fn int GUIAPI MBS2WCSEx (PLOGFONT log_font, void* dest, BOOL wc32, \
 *               const unsigned char* mstr, int mstr_len, int n, \
 *               int* conved_mstr_len)
 *
 * \brief Converts a multibyte string to a wide character string in
 *        UC16 according to the charset/encoding of the logical font.
 *
 * This function is a MiniGUI version of ISO/ANSI mbstowcs function.
 * It converts a multibyte string to a wide character string in UC16.
 * The behaviour of mbstowcs depends on the LC_CTYPE category of the
 * current locale, while MBS2WCS depends on the charset/encoding of
 * MiniGUI logical font.
 *
 * \param log_font The logical font.
 * \param dest The buffer used to store the converted wide character string.
 * \param wc32 Whether the unicode char is 32-bit long. TRUE for yes, FALSE
 *        for 16-bit long.
 * \param mstr The pointer to multibyte string.
 * \param mstr_len The length of the multibyte string in bytes.
 * \param n The length of the buffer in wide characters.
 * \param conved_mstr_len The length of the multibyte string which are
 *        converted correctly in bytes will be returned through this pointer,
 *        can be NULL.
 *
 * \return The function returns the number of wide characters
 *         that make up the converted part of the wide character string,
 *         not including the terminating null wide character.
 *         If an invalid multibyte sequence was encountered, -1 is returned.
 *
 * \sa WCS2MBSEx, mbstowcs, mbsrtowcs
 */
MG_EXPORT int GUIAPI MBS2WCSEx (PLOGFONT log_font, void* dest, BOOL wc32,
                const unsigned char* mstr, int mstr_len, int n,
                int* conved_mstr_len);

/**
 * \def MBS2WCS(log_font, dest, mstr, mstr_len, n)
 * \brief The backward compatibility version of MBS2WCSEx.
 *
 * \sa MBS2WCSEx
 */
#define MBS2WCS(log_font, dest, mstr, mstr_len, n) \
            MBS2WCSEx(log_font, dest, sizeof (wchar_t) == 4, mstr, \
            mstr_len, n, NULL)

/**
 * \fn int GUIAPI WCS2MBSEx (PLOGFONT log_font, unsigned char* dest, \
 *               const void *wcs, int wcs_len, BOOL wc32, int n, \
 *               int* conved_wcs_len)
 *
 * \brief Converts a wide character string in UC16 to a multibyte
 *        string according to the charset/encoding of the logical font.
 *
 * This function is a MiniGUI version of ISO/ANSI wcstombs function.
 * It converts a wide character string in UC16 to a multibyte string.
 * The behaviour of wcstombs depends on the LC_CTYPE category of the
 * current locale, while WCS2MBS depends on the charset/encoding of
 * MiniGUI logical font.
 *
 * \param log_font The logical font.
 * \param dest The buffer used to store the converted multibyte string.
 * \param wcs The pointer to the wide character string.
 * \param wcs_len The number of the wide characters in wcs.
 * \param wc32 Whether the wide char is 32-bit long. TRUE for yes, FALSE
 *        for 16-bit long.
 * \param n The length of the dest in bytes.
 * \param conved_wcs_len The number of the wide characters which are
 *        converted correctly will be returned through this pointer, can
 *        be NULL.
 *
 * \return The function returns the number of bytes that make up the
 *         converted part of multibyte sequence, not including the
 *         terminating null byte. If a wide character was encountered
 *         which could not be converted, -1 is returned.
 *
 * \sa MBS2WCSEx, wcstombs, wcsrtombs
 */
MG_EXPORT int GUIAPI WCS2MBSEx (PLOGFONT log_font, unsigned char* dest,
                const void *wcs, int wcs_len, BOOL wc32, int n,
                int* conved_wcs_len);

/**
 * \def WCS2MBS(log_font, dest, wcs, wcs_len, n)
 * \brief The backward compatibility version of WCS2MBSEx.
 *
 * \sa WCS2MBSEx
 */
#define WCS2MBS(log_font, dest, wcs, wcs_len, n) \
            WCS2MBSEx (log_font, dest, wcs, wcs_len, sizeof (wchar_t) == 4, \
            n, NULL)

#endif /* _MGCHARSET_UNICODE */

    /** @} end of text_parse_fns */

    /**
     * \defgroup bidi_types BIDI types
     *
     * Values for BIDI types.
     *
     * @{
     */

/**
 * The type for bidirection embedding level (Sint8).
 */
typedef Sint8   BidiLevel;
/**
 * The type for bidirection type (Uint16).
 */
typedef Uint16  BidiType;
/**
 * The type for bidirection bracket type (Uint32).
 */
typedef Uint32  BidiBracketType;

/**
 * The type for bidirection joining type (Uint8).
 */
typedef Uint8   BidiJoiningType;
/**
 * The type for bidirection Arabic property (Uint8).
 */
typedef Uint8   BidiArabicProp;

/**
 * The type for paragraph direction (Uint16).
 * Only can be one of the following values:
 *
 * - BIDI_PGDIR_LTR: Left to right
 * - BIDI_PGDIR_RTL: Right to left
 * - BIDI_PGDIR_WLTR: Weak left to right
 * - BIDI_PGDIR_WRTL: Weak right to left
 * - BIDI_PGDIR_ON: Neutral
 */
typedef Uint16  ParagraphDir;

#define BIDI_FLAG_SHAPE_MIRRORING       0x00000001
#define BIDI_FLAG_REORDER_NSM           0x00000002
#define BIDI_FLAG_SHAPE_ARAB_PRES       0x00000100
#define BIDI_FLAG_SHAPE_ARAB_LIGA       0x00000200
#define BIDI_FLAG_SHAPE_ARAB_CONSOLE    0x00000400

#define BIDI_FLAGS_DEFAULT  \
    (BIDI_FLAG_REORDER_NSM | BIDI_FLAG_REMOVE_SPECIALS)

#define BIDI_FLAGS_ARABIC  \
    (BIDI_FLAG_SHAPE_ARAB_PRES | BIDI_FLAG_SHAPE_ARAB_LIGA)

#define BIDI_FLAG_REMOVE_BIDI           0x00010000
#define BIDI_FLAG_REMOVE_JOINING        0x00020000
#define BIDI_FLAG_REMOVE_SPECIALS       0x00040000

#define BIDI_BRACKET_NONE           0
#define BIDI_BRACKET_OPEN_MASK      0x80000000
#define BIDI_BRACKET_CHAR_MASK      0X7FFFFFFF
#define BIDI_IS_BRACKET_OPEN(bt)    ((bt & BIDI_BRACKET_OPEN_MASK)>0)
#define BIDI_BRACKET_CHAR(bt)       ((bt & BIDI_BRACKET_CHAR_MASK))

/*
 * Define some bit masks, that character types are based on, each one has
 * only one bit on.
 */

#define BIDI_TYPE_INVALID       0x0000

#define BIDI_MASK_RTL           0x0001    /* Is right to left */
#define BIDI_MASK_ARABIC        0x0002    /* Is arabic */
#define BIDI_MASK_FIRST         0x0004    /* Is direction is determined by first strong */
#define BIDI_MASK_SEPARATOR     0x0008    /* Is separator: BS, SS */

#define BIDI_MASK_STRONG        0x0010    /* Is strong */
#define BIDI_MASK_WEAK          0x0020    /* Is weak */
#define BIDI_MASK_NEUTRAL       0x0040    /* Is neutral */
#define BIDI_MASK_SENTINEL      0x0080    /* Is sentinel: SOT, EOT */

/* Each char can be only one of the seven following: */
#define BIDI_MASK_LETTER        0x0100    /* Is letter: L, R, AL */
#define BIDI_MASK_NUMBER        0x0200    /* Is number: EN, AN */
#define BIDI_MASK_NUMSEPTER     0x0300    /* Is number separator or terminator: ES, ET, CS */
#define BIDI_MASK_SPACE         0x0400    /* Is space: BN, BS, SS, WS */
#define BIDI_MASK_EXPLICIT      0x0500    /* Is expilict mark: LRE, RLE, LRO, RLO, PDF */
#define BIDI_MASK_ISOLATE       0x0600    /* Is isolate mark: LRI, RLI, FSI, PDI */
#define BIDI_MASK_NSM           0x0700    /* Is non spacing mark: NSM */
#   define BIDI_TYPE_MASK          0x0700

#define BIDI_MASK_OVERRIDE      0x8000    /* Is explicit override: LRO, RLO */

/* Each char can be only one of the seven following: */
#define BIDI_MASK_ES            0x1000
#define BIDI_MASK_CS            0x2000
#define BIDI_MASK_ET            0x3000
#define BIDI_MASK_BS            0x4000
#define BIDI_MASK_SS            0x5000
#define BIDI_MASK_BN            0x6000
#define BIDI_MASK_WS            0x7000
#   define BIDI_MISC_MASK          0x7000

/* Reserved for private use */
#define BIDI_MASK_PRIVATE       0x8000

/**
 * \def BIDI_TYPE_LTR
 * \brief Strong left to right
 */
#define BIDI_TYPE_LTR \
    (BIDI_MASK_STRONG | BIDI_MASK_LETTER)

/**
 * \def BIDI_TYPE_RTL
 * \brief Right to left characters
 */
#define BIDI_TYPE_RTL \
    (BIDI_MASK_STRONG | BIDI_MASK_LETTER | BIDI_MASK_RTL)

/**
 * \def BIDI_TYPE_AL
 * \brief Arabic characters
 */
#define BIDI_TYPE_AL \
    (BIDI_MASK_STRONG | BIDI_MASK_LETTER | BIDI_MASK_RTL  | BIDI_MASK_ARABIC)

/**
 * \def BIDI_TYPE_LRE
 * \brief Left-To-Right embedding
 */
#define BIDI_TYPE_LRE \
    (BIDI_MASK_STRONG | BIDI_MASK_EXPLICIT)

/**
 * \def BIDI_TYPE_RLE
 * \brief Right-To-Left embedding
 */
#define BIDI_TYPE_RLE \
    (BIDI_MASK_STRONG | BIDI_MASK_EXPLICIT | BIDI_MASK_RTL)

/**
 * \def BIDI_TYPE_LRO
 * \brief Left-To-Right override
 */
#define BIDI_TYPE_LRO \
    (BIDI_MASK_STRONG | BIDI_MASK_EXPLICIT | BIDI_MASK_OVERRIDE)

/**
 * \def BIDI_TYPE_RLO
 * \brief Right-To-Left override
 */
#define BIDI_TYPE_RLO \
    (BIDI_MASK_STRONG | BIDI_MASK_EXPLICIT | BIDI_MASK_RTL | BIDI_MASK_OVERRIDE)

/**
 * \def BIDI_TYPE_PDF
 * \brief Pop directional override
 */
#define BIDI_TYPE_PDF \
    (BIDI_MASK_WEAK | BIDI_MASK_EXPLICIT)

/**
 * \def BIDI_TYPE_EN
 * \brief European digit
 */
#define BIDI_TYPE_EN \
    (BIDI_MASK_WEAK | BIDI_MASK_NUMBER)

/**
 * \def BIDI_TYPE_AN
 * \brief Arabic digit
 */
#define BIDI_TYPE_AN \
    (BIDI_MASK_WEAK | BIDI_MASK_NUMBER | BIDI_MASK_ARABIC)

/**
 * \def BIDI_TYPE_ES
 * \brief European number separator
 */
#define BIDI_TYPE_ES \
    (BIDI_MASK_WEAK | BIDI_MASK_NUMSEPTER | BIDI_MASK_ES)

/**
 * \def BIDI_TYPE_ET
 * \brief European number terminator
 */
#define BIDI_TYPE_ET \
    (BIDI_MASK_WEAK | BIDI_MASK_NUMSEPTER | BIDI_MASK_ET)

/**
 * \def BIDI_TYPE_CS
 * \brief Common Separator
 */
#define BIDI_TYPE_CS \
    (BIDI_MASK_WEAK | BIDI_MASK_NUMSEPTER | BIDI_MASK_CS)

/**
 * \def BIDI_TYPE_NSM
 * \brief Non spacing mark
 */
#define BIDI_TYPE_NSM \
    (BIDI_MASK_WEAK | BIDI_MASK_NSM)

/**
 * \def BIDI_TYPE_BN
 * \brief Boundary neutral
 */
#define BIDI_TYPE_BN \
    (BIDI_MASK_WEAK | BIDI_MASK_SPACE | BIDI_MASK_BN)

/**
 * \def BIDI_TYPE_BS
 * \brief Block separator (Paragraph separator)
 */
#define BIDI_TYPE_BS \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_SPACE | BIDI_MASK_SEPARATOR | BIDI_MASK_BS)

/**
 * \def BIDI_TYPE_SS
 * \brief Segment separator
 */
#define BIDI_TYPE_SS \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_SPACE | BIDI_MASK_SEPARATOR | BIDI_MASK_SS)

/**
 * \def BIDI_TYPE_WS
 * \brief Whitespace
 */
#define BIDI_TYPE_WS \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_SPACE | BIDI_MASK_WS)

/**
 * \def BIDI_TYPE_ON
 * \brief Other Neutral
 */
#define BIDI_TYPE_ON \
    (BIDI_MASK_NEUTRAL)

/**
 * \def BIDI_TYPE_LRI
 * \brief Left-to-Right Isolate
 */
#define BIDI_TYPE_LRI \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_ISOLATE)

/**
 * \def BIDI_TYPE_RLI
 * \brief Right-to-Left Isolate
 */
#define BIDI_TYPE_RLI \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_ISOLATE | BIDI_MASK_RTL)

/**
 * \def BIDI_TYPE_FSI
 * \brief First Strong Isolate
 */
#define BIDI_TYPE_FSI \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_ISOLATE | BIDI_MASK_FIRST)

/**
 * \def BIDI_TYPE_PDI
 * \brief Pop Directional Isolate
 */
#define BIDI_TYPE_PDI \
    (BIDI_MASK_NEUTRAL | BIDI_MASK_WEAK | BIDI_MASK_ISOLATE)

#define BIDI_TYPE_SENTINEL    (BIDI_MASK_SENTINEL)

/* The following are only used internally */

/* Weak Left-To-Right */
#define BIDI_TYPE_WLTR      (BIDI_MASK_WEAK)
/* Weak Right-To-Left */
#define BIDI_TYPE_WRTL      (BIDI_MASK_WEAK | BIDI_MASK_RTL)

/* Start of text */
#define BIDI_TYPE_SOT       (BIDI_MASK_SENTINEL)
/* End of text */
#define BIDI_TYPE_EOT       (BIDI_MASK_SENTINEL | BIDI_MASK_RTL)

#define BIDI_TYPE_PRIVATE   (BIDI_MASK_PRIVATE)

/* Is private-use value? */
#define BIDI_IS_PRIVATE(p)  ((p) & BIDI_MASK_PRIVATE)

/* Is right to left? */
#define BIDI_IS_RTL(p)      ((p) & BIDI_MASK_RTL)

/* Is arabic? */
#define BIDI_IS_ARABIC(p)   ((p) & BIDI_MASK_ARABIC)

/* Is right-to-left level? */
#define BIDI_LEVEL_IS_RTL(lev) ((lev) & 1)

/* Return the bidi type corresponding to the direction of the level number,
   BIDI_TYPE_LTR for evens and BIDI_TYPE_RTL for odds. */
#define BIDI_LEVEL_TO_DIR(lev) \
    (BIDI_LEVEL_IS_RTL(lev) ? BIDI_TYPE_RTL : BIDI_TYPE_LTR)

/* Return the minimum level of the direction, 0 for BIDI_TYPE_LTR and
   1 for BIDI_TYPE_RTL and BIDI_TYPE_AL. */
#define BIDI_DIR_TO_LEVEL(dir) \
    ((BidiLevel) (BIDI_IS_RTL(dir) ? 1 : 0))

/* Is strong? */
#define BIDI_IS_STRONG(p)   ((p) & BIDI_MASK_STRONG)
/* Is weak? */
#define BIDI_IS_WEAK(p)     ((p) & BIDI_MASK_WEAK)
/* Is neutral? */
#define BIDI_IS_NEUTRAL(p)  ((p) & BIDI_MASK_NEUTRAL)
/* Is sentinel? */
#define BIDI_IS_SENTINEL(p) ((p) & BIDI_MASK_SENTINEL)

/* Is letter: L, R, AL? */
#define BIDI_IS_LETTER(p)   (((p) & BIDI_TYPE_MASK) == BIDI_MASK_LETTER)

/* Is number: EN, AN? */
#define BIDI_IS_NUMBER(p)   (((p) & BIDI_TYPE_MASK) == BIDI_MASK_NUMBER)

/* Is number separator or terminator: ES, ET, CS? */
#define BIDI_IS_NUMBER_SEPARATOR_OR_TERMINATOR(p) \
    (((p) & BIDI_TYPE_MASK) == BIDI_MASK_NUMSEPTER)

/* Is space: BN, BS, SS, WS? */
#define BIDI_IS_SPACE(p)    (((p) & BIDI_TYPE_MASK) == BIDI_MASK_SPACE)
/* Is explicit mark: LRE, RLE, LRO, RLO, PDF? */
#define BIDI_IS_EXPLICIT(p) (((p) & BIDI_TYPE_MASK) == BIDI_MASK_EXPLICIT)

/* Is test separator: BS, SS? */
#define BIDI_IS_SEPARATOR(p) ((p) & BIDI_MASK_SEPARATOR)

/* Is explicit override: LRO, RLO? */
#define BIDI_IS_OVERRIDE(p) ((p) & BIDI_MASK_OVERRIDE)

/* Is isolote: LRO, RLO? */
#define BIDI_IS_ISOLATE(p) (((p) & BIDI_TYPE_MASK) == BIDI_MASK_ISOLATE)

/* Is left to right letter: LTR? */
#define BIDI_IS_LTR_LETTER(p) \
    ((p) & (BIDI_MASK_LETTER | BIDI_MASK_RTL) == BIDI_MASK_LETTER)

/* Is right to left letter: RTL, AL? */
#define BIDI_IS_RTL_LETTER(p) \
    ((p) & (BIDI_MASK_LETTER | BIDI_MASK_RTL) \
    == (BIDI_MASK_LETTER | BIDI_MASK_RTL))

/* Is ES or CS: ES, CS? */
#define BIDI_IS_ES_OR_CS(p) \
    (((p) & BIDI_MISC_MASK) == BIDI_MASK_ES || \
     ((p) & BIDI_MISC_MASK) == BIDI_MASK_CS)

/* Change numbers: EN, AN to RTL. */
#define BIDI_NUMBER_TO_RTL(p) \
    (BIDI_IS_NUMBER(p) ? BIDI_TYPE_RTL : (p))

/* Is explicit or BN: LRE, RLE, LRO, RLO, PDF, BN? */
#define BIDI_IS_EXPLICIT_OR_BN(p) \
    ((((p) & BIDI_TYPE_MASK) == BIDI_MASK_EXPLICIT) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_BN))

/* Is explicit or BN or WS: LRE, RLE, LRO, RLO, PDF, BN, WS? */
#define BIDI_IS_EXPLICIT_OR_BN_OR_WS(p) \
    ((((p) & BIDI_TYPE_MASK) == BIDI_MASK_EXPLICIT) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_BN) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_WS))

/* Is explicit or separator or BN or WS: LRE, RLE, LRO, RLO, PDF, BS, SS, BN, WS? */
#define BIDI_IS_EXPLICIT_OR_SEPARATOR_OR_BN_OR_WS(p) \
    ((((p) & BIDI_TYPE_MASK) == BIDI_MASK_EXPLICIT) || \
        ((p) & BIDI_MASK_SEPARATOR) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_BN) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_WS))

/* Is explicit or BN or NSM: LRE, RLE, LRO, RLO, PDF, BN, NSM? */
#define BIDI_IS_EXPLICIT_OR_BN_OR_NSM(p) \
    ((((p) & BIDI_TYPE_MASK) == BIDI_MASK_EXPLICIT) || \
        (((p) & BIDI_TYPE_MASK) == BIDI_MASK_NSM) || \
        (((p) & BIDI_MISC_MASK) == BIDI_MASK_BN))

/* Override status of an explicit mark:
 * LRO,LRE->LTR, RLO,RLE->RTL, otherwise->ON. */
#define BIDI_EXPLICIT_TO_OVERRIDE_DIR(p) \
    (BIDI_IS_OVERRIDE(p) ? BIDI_LEVEL_TO_DIR(BIDI_DIR_TO_LEVEL(p)) \
        : BIDI_TYPE_ON)

/* Change numbers to RTL: EN,AN -> RTL. */
#define BIDI_CHANGE_NUMBER_TO_RTL(p) \
    (BIDI_IS_NUMBER(p) ? BIDI_TYPE_RTL : (p))

#define BIDI_PGDIR_LTR  BIDI_TYPE_LTR
#define BIDI_PGDIR_RTL  BIDI_TYPE_RTL
#define BIDI_PGDIR_WLTR BIDI_TYPE_WLTR
#define BIDI_PGDIR_WRTL BIDI_TYPE_WRTL
#define BIDI_PGDIR_ON   BIDI_TYPE_ON

/*
 * Define bit masks that joining types are based on, each mask has
 * only one bit set.
 */
#define BIDI_MASK_JOINS_RIGHT       0x01    /* May join to right */
#define BIDI_MASK_JOINS_LEFT        0x02    /* May join to right */
#define BIDI_MASK_ARAB_SHAPES       0x04    /* May Arabic shape */
#define BIDI_MASK_TRANSPARENT       0x08    /* Is transparent */
#define BIDI_MASK_IGNORED           0x10    /* Is ignored */
#define BIDI_MASK_LIGATURED         0x20    /* Is ligatured */

/*
 * Define values for BidiJoiningType
 */

/* nUn-joining */
#define BIDI_JOINING_TYPE_U_VAL    ( 0 )

/* Right-joining */
#define BIDI_JOINING_TYPE_R_VAL    \
    ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_ARAB_SHAPES )

/* Dual-joining */
#define BIDI_JOINING_TYPE_D_VAL    \
    ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT \
    | BIDI_MASK_ARAB_SHAPES )

/* join-Causing */
#define BIDI_JOINING_TYPE_C_VAL    \
    ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT )

/* Left-joining */
#define BIDI_JOINING_TYPE_L_VAL    \
    ( BIDI_MASK_JOINS_LEFT | BIDI_MASK_ARAB_SHAPES )

/* Transparent */
#define BIDI_JOINING_TYPE_T_VAL    \
    ( BIDI_MASK_TRANSPARENT | BIDI_MASK_ARAB_SHAPES )

/* iGnored */
#define BIDI_JOINING_TYPE_G_VAL    ( BIDI_MASK_IGNORED )

/*
 * The equivalent of JoiningType values for ArabicProp
 */

/* Primary Arabic Joining Classes (Table 8-2) */

/* nUn-joining */
#define BIDI_IS_JOINING_TYPE_U(p)    \
    ( 0 == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT ) ) )

/* Right-joining */
#define BIDI_IS_JOINING_TYPE_R(p)    \
    ( BIDI_MASK_JOINS_RIGHT == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT ) ) )

/* Dual-joining */
#define BIDI_IS_JOINING_TYPE_D(p)    \
    ( ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT    \
      | BIDI_MASK_ARAB_SHAPES ) == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT    \
        | BIDI_MASK_ARAB_SHAPES ) ) )

/* join-Causing */
#define BIDI_IS_JOINING_TYPE_C(p)    \
    ( ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT ) == ( (p) & \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT    \
        | BIDI_MASK_ARAB_SHAPES ) ) )

/* Left-joining */
#define BIDI_IS_JOINING_TYPE_L(p)    \
    ( BIDI_MASK_JOINS_LEFT == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT ) ) )

/* Transparent */
#define BIDI_IS_JOINING_TYPE_T(p)    \
    ( BIDI_MASK_TRANSPARENT == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED ) ) )

/* iGnored */
#define BIDI_IS_JOINING_TYPE_G(p)    \
    ( BIDI_MASK_IGNORED == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED ) ) )

/* and for Derived Arabic Joining Classes (Table 8-3) */

/* Right join-Causing */
#define BIDI_IS_JOINING_TYPE_RC(p)    \
    ( BIDI_MASK_JOINS_RIGHT == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_RIGHT ) ) )

/* Left join-Causing */
#define BIDI_IS_JOINING_TYPE_LC(p)    \
    ( BIDI_MASK_JOINS_LEFT == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_JOINS_LEFT ) ) )

/*
 * Defining macros for needed queries, It is fully dependent on the
 * implementation of BidiJoiningType.
 */

/* Joins to right: R, D, C? */
#define BIDI_JOINS_RIGHT(p)    ((p) & BIDI_MASK_JOINS_RIGHT)

/* Joins to left: L, D, C? */
#define BIDI_JOINS_LEFT(p)    ((p) & BIDI_MASK_JOINS_LEFT)

/* May shape: R, D, L, T? */
#define BIDI_ARAB_SHAPES(p)    ((p) & BIDI_MASK_ARAB_SHAPES)

/* Is skipped in joining: T, G? */
#define BIDI_IS_JOIN_SKIPPED(p)    \
    ((p) & (BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED))

/* Is base that will be shaped: R, D, L? */
#define BIDI_IS_JOIN_BASE_SHAPES(p)    \
    ( BIDI_MASK_ARAB_SHAPES == ( (p) &    \
        ( BIDI_MASK_TRANSPARENT | BIDI_MASK_IGNORED    \
        | BIDI_MASK_ARAB_SHAPES ) ) )

#define BIDI_JOINS_PRECEDING_MASK(level)    \
    (BIDI_LEVEL_IS_RTL (level) ? BIDI_MASK_JOINS_RIGHT    \
                      : BIDI_MASK_JOINS_LEFT)

#define BIDI_JOINS_FOLLOWING_MASK(level)    \
    (BIDI_LEVEL_IS_RTL (level) ? BIDI_MASK_JOINS_LEFT    \
                      : BIDI_MASK_JOINS_RIGHT)

#define BIDI_JOIN_SHAPE(p)    \
    ((p) & ( BIDI_MASK_JOINS_RIGHT | BIDI_MASK_JOINS_LEFT ))

    /** @} end of bidi_types */

#ifdef _MGCHARSET_UNICODE

    /**
     * \defgroup unicode_ops Operators for Unicode character and string
     *
     * @{
     */

/**
 * \fn UCharGeneralCategory GUIAPI UCharGetCategory(Uchar32 uc)
 * \brief Get the general category of a Unicode character.
 *
 * The function determines the general category (basic type) of
 * a UNICODE character \a uc.
 *
 * \param uc The Uchar32 value of the Unicode character.
 *
 * \return The general category of the Unicode character.
 *
 * \sa UCharGeneralCategory
 *
 * Since: 4.0.0
 */
MG_EXPORT UCharGeneralCategory GUIAPI UCharGetCategory(Uchar32 uc);

/**
 * \fn UCharBreakType GUIAPI UCharGetBreakType(Uchar32 uc)
 * \brief Get the break property of a Unicode character.
 *
 * The function determines the break property of a UNICODE character
 * \a uc.
 *
 * \param uc The Uchar32 value of the Unicode character.
 *
 * \return The breaking type value of the Unicode character.
 *
 * \sa UCharBreakType
 *
 * Since: 4.0.0
 */
MG_EXPORT UCharBreakType GUIAPI UCharGetBreakType(Uchar32 uc);

/**
 * \fn BidiType GUIAPI UCharGetBidiType(Uchar32 uc)
 * \brief Get bidi type of a Unicode character.
 *
 * This function returns the bidi type of a Unicode character as defined in
 * Table 3.7 Bidirectional Character Types of the
 * Unicode Bidirectional Algorithm available at
 *
 * https://www.unicode.org/reports/tr9/#Bidirectional_Character_Types
 *
 * \param uc The Uchar32 character.
 *
 * \return The bidi type.
 *
 * \sa UStrGetBidiTypes, bidi_types
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiType GUIAPI UCharGetBidiType(Uchar32 uc);

/**
 * \fn void GUIAPI UStrGetBidiTypes(const Uchar32* ucs, int nr_ucs,
 *      BidiType* bdts);
 * \brief Get bidi types for an string of Unicode characters.
 *
 * This function finds the bidi types of an string of Unicode characters.
 *
 * \param ucs The pointer to the Uchar32 string.
 * \param nr_ucs The number of Unicode characters in the string.
 * \param bdts The pointer to a buffer which will be used to store
 *      the bidi types of the characters in \a ucs.
 *
 * \sa UCharGetBidiType
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UStrGetBidiTypes(const Uchar32* ucs, int nr_ucs,
        BidiType* bdts);

/**
 * \fn BidiBracketType GUIAPI UCharGetBracketType(Uchar32 ch)
 * \brief Get bracketed character
 *
 * This function finds the bracketed equivalent of a character as defined in
 * the file BidiBrackets.txt of the Unicode Character Database available at
 *
 * https://www.unicode.org/Public/UNIDATA/BidiBrackets.txt.
 *
 * If the input character is a declared as a brackets character in the
 * Unicode standard and has a bracketed equivalent.  The matching bracketed
 * character is put in the output, otherwise the input character itself is
 * put.
 *
 * \param ch input character
 *
 * \return The bracket type of the character. Use the
 *      BIDI_IS_BRACKET(BidiBracketType) to test if it is a valid
 *      property. Use BCHI_BRACKET_CHAR(BidiBracketType) to get
 *      the bracketed character value.
 *
 * \sa UStrGetBracketTypes
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiBracketType GUIAPI UCharGetBracketType(Uchar32 ch);

/**
 * \fn void UStrGetBracketTypes(const Uchar32 *ucs,
 *      const BidiType *bidi_types, int nr_ucs,
 *      BidiBracketType *bracket_types)
 * \brief Get bracketed characters of a Uchar32 string.
 *
 * This function finds the bracketed characters of a string of Unicode
 * characters. See UCharGetBracketType() for more information about the
 * bracketed characters returned by this function.
 *
 * \param ucs The input Uchar32 string.
 * \param nr_ucs The length of string.
 * \param bidi_types The bidi types (an array of BidiType) of the string.
 * \param bracket_types The pointer to a BidiBracketType array storing
 *      the bracketed characters.
 *
 * \sa UCharGetBracketType
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UStrGetBracketTypes(const Uchar32 *ucs,
        const BidiType *bidi_types, int len_ucs,
        BidiBracketType *bracket_types);

/**
 * \fn BOOL GUIAPI UCharGetMirror(Uchar32 uc, Uchar32* mirrored)
 * \brief Get mirrored character.
 *
 * This function finds the mirrored equivalent of a Unicode character as
 * defined in the file BidiMirroring.txt of the Unicode Character Database
 * available at
 *
 * https://www.unicode.org/Public/UNIDATA/BidiMirroring.txt.
 *
 * \param uc The Uchar32 character.
 * \param mirrored A pointer to a Uchar32 buffer to return the mirroed
 *      character. If the input character \a uc is a declared as a
 *      mirroring character in the Unicode standard and has a mirrored
 *      equivalent, the matching mirrored  character was put in this buffer,
 *      otherwise the input character itself was put.
 *
 * \return A BOOL value indicates if the character has a mirroring equivalent
 *      or not.
 *
 * Since: 4.0.0
 */
MG_EXPORT BOOL GUIAPI UCharGetMirror(Uchar32 uc, Uchar32* mirrored);

/**
 * \fn BidiJoiningType GUIAPI UCharGetJoiningType(Uchar32 uc)
 * \brief Get character joining type.
 *
 * This function returns the joining type of a character as defined in Table
 * 8-2 Primary Arabic Joining Classes of the Unicode standard available at
 *
 * https://www.unicode.org/versions/Unicode12.0.0/ch08.pdf#G7462
 *
 * using data provided in file ArabicShaping.txt and UnicodeData.txt of
 * the Unicode Character Database available at
 *
 * https://www.unicode.org/Public/UNIDATA/ArabicShaping.txt
 *
 * and
 *
 * https://www.unicode.org/Public/UNIDATA/UnicodeData.txt.
 *
 * \param uc The Uchar32 character.
 *
 * \return The joining type of the specified Uchar32 character.
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiJoiningType GUIAPI UCharGetJoiningType(Uchar32 uc);

/**
 * \fn void GUIAPI UStrGetJoiningTypes(const Uchar32 *ucs, int nr_ucs,
 *      BidiJoiningType *joining_types)
 * \brief Get joining types for a string of Unicode characters
 *
 * This function finds the joining types of an string of characters.
 * See \a UCharGetJoiningType for more information about the
 * joining types returned by this function.
 *
 * \param ucs The input Uchar32 string.
 * \param nr_ucs The number of Uchar32 characters.
 * \param joining_types The pointer to a BidiJoiningType array storing
 *      the joining types of the characters.
 *
 * \sa UCharGetJoiningType
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UStrGetJoiningTypes(const Uchar32 *ucs, int nr_ucs,
        BidiJoiningType *joining_types);

/**
 * \fn BidiType GUIAPI UBidiGetParagraphDir(const BidiType *bidi_types, int len)
 * \brief Get the base paragraph direction.
 *
 * This function finds the base direction of a single paragraph,
 * as defined by
 * [rule P2 of the Unicode Bidirectional Algorithm](https://www.unicode.org/reports/tr9/#P2).
 *
 * You typically do not need this function as
 * \a UBidiGetParagraphEmbeddingLevels knows how to compute base direction
 * itself, but you may need this to implement a more sophisticated paragraph
 * direction handling.  Note that you can pass more than a paragraph to this
 * function and the direction of the first non-neutral paragraph is returned,
 * which is a very good heuristic to set direction of the neutral paragraphs
 * at the beginning of text.  For other neutral paragraphs, you better use the
 * direction of the previous paragraph.
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      UStrGetBidiTypes().
 * \param len The length of bidi types array.
 *
 * \return Base pargraph direction. No weak paragraph direction is returned,
 * only BIDI_PGDIR_LTR, BIDI_PGDIR_RTL, or BIDI_PGDIR_ON.
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiType GUIAPI UBidiGetParagraphDir(const BidiType *bidi_types, int len);

/**
 * \fn BidiLevel GUIAPI UBidiGetParagraphEmbeddingLevels(
 *      const BidiType *bidi_types,
 *      const BidiBracketType* bracket_types, int len,
 *      ParagraphDir *paragraph_dir, BidiLevel *embedding_levels);
 * \brief Get bidi embedding levels of a paragraph.
 *
 * This function finds the bidi embedding levels of a single paragraph,
 * as defined by
 * [the Unicode Bidirectional Algorithm](https://www.unicode.org/reports/tr9/).
 *
 * This function implements rules P2 to I1 inclusive, and parts 1 to 3 of L1.
 * Part 4 of L1 is implemented in \a UBidiReorderLine().
 *
 * \param bidi_types the pointer to the BidiType array as returned by
 *      \a UStrGetBidiTypes.
 * \param bracket_types The pointer to a Uint8 which contains the
        bracket types as returned by \a UStrGetBracketTypes.
 * \param len The length of the list.
 * \param paragraph_dir requested and resolved paragraph base direction. You
 *      can pass the following values for base direction:
 *          - BIDI_PGDIR_LTR\n
 *              Explicit left to right.
 *          - BIDI_PGDIR_RTL\n
 *              Explicit right to left.
 *          - other values\n
 *              The base direction will be resolved by applying the
 *              rules P2 and P3, and returned via this parameter (one
 *              of BIDI_PGDIR_LTR or BIDI_PGDIR_RTL) .
 * \param embedding_levels The pointer to a buffer which will restore
 *      the embedding levels.
 *
 * \return The Maximum level found plus one, or zero if any error occurred
 * (memory allocation failure most probably).
 *
 * \sa UStrGetBidiTypes, UStrGetBracketTypes
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiLevel GUIAPI UBidiGetParagraphEmbeddingLevels(
        const BidiType *bidi_types,
        const BidiBracketType* bracket_types, int len,
        ParagraphDir *paragraph_dir, BidiLevel *embedding_levels);

/**
 * \brief An alternative of \a UBidiGetParagraphEmbeddingLevels().
 *
 * This function finds the bidi embedding levels of a single paragraph,
 * as defined by
 * [the Unicode Bidirectional Algorithm](https://www.unicode.org/reports/tr9/).
 *
 * This function implements rules P2 to I1 inclusive, and parts 1 to 3 of L1.
 * Part 4 of L1 is implemented in \a UBidiReorderLine().
 *
 * Different from \a UBidiGetParagraphEmbeddingLevels(), this function finds
 * the bidi embedding levels directly from the Uchar32 string.
 *
 * \param ucs The Unicode character string.
 * \param nr_ucs The length of the string.
 * \param paragraph_dir requested and resolved paragraph base direction. You
 *      can pass the following values for base direction:
 *          - BIDI_PGDIR_LTR\n
 *              Explicit left to right.
 *          - BIDI_PGDIR_RTL\n
 *              Explicit right to left.
 *          - other values\n
 *              The base direction will be resolved by applying the
 *              rules P2 and P3, and returned via this parameter (one
 *              of BIDI_PGDIR_LTR or BIDI_PGDIR_RTL) .
 * \param embedding_levels The pointer to a buffer which will restore
 *      the embedding levels.
 *
 * \return The Maximum level found plus one, or zero if any error occurred
 * (memory allocation failure most probably).
 *
 * \sa UBidiGetParagraphEmbeddingLevels
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiLevel GUIAPI UBidiGetParagraphEmbeddingLevelsAlt(
        const Uchar32* ucs, int nr_ucs,
        ParagraphDir *paragraph_dir, BidiLevel *embedding_levels);

/*
 * \var typedef void (*CB_REVERSE_ARRAY) (void* extra, int len, int pos)
 * \brief The prototype of the user defined function to reverse an array.
 *
 * The function reverse an array pointed by \a extra from the position
 * specified by \a pos for the length specified by \a len.
 *
 * \sa UBidiReorderLine, BIDILogAChars2VisACharsEx
 */
typedef void (*CB_REVERSE_ARRAY) (void* extra, int len, int pos);

/**
 * \fn BidiLevel GUIAPI UBidiReorderLine(Uint32 bidi_flags,
 *      const BidiType *bidi_types, int len, int off,
 *      ParagraphDir paragraph_dir, BidiLevel *embedding_levels,
 *      Uchar32 *visual_str, int *indices_map,
 *      void* extra, CB_REVERSE_ARRAY cb_reverse_extra)
 * \brief Reorder a line of logical string to visual string.
 *
 * This function reorders the characters in a line of text from logical to
 * final visual order.
 *
 * This function implements part 4 of rule L1, and rules
 * L2 and L3 of the Unicode Bidirectional Algorithm available at
 *
 * https://www.unicode.org/reports/tr9/#Reordering_Resolved_Levels.
 *
 * As a side effect it also sets position maps if not NULL.
 *
 * You should provide the resolved paragraph direction and embedding levels as
 * set by UBidiGetParagraphEmbeddingLevels(). Also note that the embedding
 * levels may change a bit.  To be exact, the embedding level of any sequence
 * of white space at the end of line is reset to the paragraph embedding level
 * (That is part 4 of rule L1).
 *
 * Note that the bidi types and embedding levels are not reordered.
 * You can reorder these (or any other) arrays using the map later.
 * The user is responsible to initialize map to something sensible,
 * like an identity mapping, or pass NULL if no map is needed.
 *
 * There is an optional part to this function, which is whether non-spacing
 * marks for right-to-left parts of the text should be reordered to come after
 * their base characters in the visual string or not.
 *
 * Most rendering engines expect this behavior, but console-based systems
 * for example do not like it. This is controlled by the
 * BIDI_FLAG_REORDER_NSM flag. The flag is on in BIDI_FLAGS_DEFAULT.
 *
 * \param bidi_flags The reorder flags.
 * \param bidi_types the pointer to the BidiType array as returned by
 *      UStrGetBidiTypes()
 * \param len The length of the list.
 * \param off The input offset of the beginning of the line in the paragraph.
 * \param paragraph_dir The resolved paragraph base direction.
 * \param embedding_levels The embedding levels, as returned by
        UBidiGetParagraphEmbeddingLevels()
 * \param visual_str The Uchar32 string will be reordered.
 * \param indices_map A map of string indices which is reordered to reflect
 *      where each character ends up.
 * \param extra The pointer to the extra array to reorder; can be NULL.
 * \param cb_reverse_extra The callback function to reverse the extra array.
 *
 * \return Maximum level found in this line plus one, or zero if any error
 * occurred (memory allocation failure most probably).
 *
 * \sa UStrGetBidiTypes, UStrGetBracketTypes,
 *      UBidiGetParagraphEmbeddingLevels
 *
 * Since: 4.0.0
 */
MG_EXPORT BidiLevel GUIAPI UBidiReorderLine(Uint32 bidi_flags,
        const BidiType *bidi_types, int len, int off,
        ParagraphDir paragraph_dir, BidiLevel *embedding_levels,
        Uchar32 *visual_str, int *indices_map,
        void* extra, CB_REVERSE_ARRAY cb_reverse_extra);

/**
 * \fn void GUIAPI UBidiShapeMirroring(const BidiLevel *embedding_levels,
 *      int len, Uchar32 *ucs)
 * \brief Do mirroring shaping
 *
 * This functions replaces mirroring characters on right-to-left embeddings in
 * string with their mirrored equivalent as returned by UCharGetMirror().
 *
 * This function implements rule L4 of the Unicode Bidirectional Algorithm
 * available at
 *
 * https://www.unicode.org/reports/tr9/#L4.
 *
 * \param embedding_levels input list of embedding levels, as returned by
 *      UBidiGetParagraphEmbeddingLevels().
 * \param len The input string length.
 * \param ucs The Uchar32 string to shape.
 *
 * \sa UBidiGetParagraphEmbeddingLevels
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UBidiShapeMirroring(const BidiLevel *embedding_levels,
        int len, Uchar32* ucs);

/**
 * \fn void GUIAPI UBidiJoinArabic(const BidiType *bidi_types,
 *      const BidiLevel *embedding_levels, int len,
 *      BidiArabicProp *ar_props);
 * \brief Do Arabic joining.
 *
 * This function does the Arabic joining algorithm.  Means, given Arabic
 * joining types of the characters in ar_props (note that
 * BidiJoiningType can be casted to BidiArabicProp automagically), this
 * function modifies this properties to grasp the effect of neighboring
 * characters. You probably need this information later to do Arabic shaping.
 *
 * This function implements rules R1 to R7 inclusive (all rules) of the Arabic
 * Cursive Joining algorithm of the Unicode standard as available at
 *
 * https://www.unicode.org/versions/Unicode12.0.0/ch08.pdf#G7462.
 *
 * It also interacts correctly with the bidirection algorithm as defined in
 * Section 3.5 Shaping of the Unicode Bidirectional Algorithm available at
 *
 * https://www.unicode.org/reports/tr9/#Shaping.
 *
 * \param bidi_types The list of bidi types as returned by UStrGetBidiTypes().
 * \param embedding_levels input list of embedding levels, as returned by
 *      UBidiGetParagraphEmbeddingLevels().
 * \param len The input string length.
 * \param ar_props Arabic properties to analyze, initialized by joining types,
 *      as returned by UStrGetJoiningTypes().
 *
 * \sa UBidiGetParagraphEmbeddingLevels, UStrGetJoiningTypes
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UBidiJoinArabic(const BidiType *bidi_types,
        const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props);

/**
 * \fn void GUIAPI UBidiShapeArabic(Uint32 shaping_flags,
 *      const BidiLevel *embedding_levels, int len,
 *      BidiArabicProp *ar_props, Uchar32 *ucs)
 * \brief Do Arabic shaping.
 *
 * The actual shaping that is done depends on the flags set. The following
 * flags affect this function:
 *
 *  - BIDI_FLAG_SHAPE_MIRRORING\n
 *      Do mirroring.
 *  - BIDI_FLAG_SHAPE_ARAB_PRES\n
 *      Shape Arabic characters to their presentation form glyphs.
 *  - BIDI_FLAG_SHAPE_ARAB_LIGA\n
 *      Form mandatory Arabic ligatures.
 *  - BIDI_FLAG_SHAPE_ARAB_CONSOLE\n
 *      Perform additional Arabic shaping
 *      suitable for text rendered on grid terminals with no mark
 *      rendering capabilities.
 *
 * Of the above, BIDI_FLAG_SHAPE_ARAB_CONSOLE is only used in special
 * cases, but the rest are recommended in any environment that doesn't have
 * other means for doing Arabic shaping.  The set of extra flags that enable
 * this level of Arabic support has a shortcut named BIDI_FLAGS_ARABIC.
 *
 * \param shaping_flags shaping flags.
 * \param embedding_levels input list of embedding levels, as returned by
 *      UBidiGetParagraphEmbeddingLevels().
 * \param len The length of the string.
 * \param ar_props The Arabic character properties as computed by
 *      UBidiJoinArabic().
 * \param ucs The Uchar32 string to shape.
 *
 * \sa UBidiGetParagraphEmbeddingLevels, UBidiJoinArabic
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UBidiShapeArabic(Uint32 shaping_flags,
        const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props, Uchar32* ucs);

/**
 * \fn void GUIAPI UBidiShape(Uint32 shaping_flags,
 *      const BidiLevel *embedding_levels, int len,
 *      BidiArabicProp *ar_props, Uchar32* ucs)
 * \brief Do bidi-aware shaping.
 *
 * This function does all shaping work that depends on the resolved embedding
 * levels of the characters.  Currently it does mirroring and Arabic shaping,
 * but the list may grow in the future.  This function is a wrapper around
 * UBidiShapeMirroring and UBidiShapeArabic.
 *
 * The flags parameter specifies which shapings are applied. The only flags
 * affecting the functionality of this function are those beginning with
 * BIDI_FLAG_SHAPE_.  Of these, only BIDI_FLAG_SHAPE_MIRRORING is on
 * in BIDI_FLAGS_DEFAULT.  For details of the Arabic-specific flags see
 * UBidiShapeArabic. If ar_props is NULL, no Arabic shaping is performed.
 *
 * Feel free to do your own shaping before or after calling this function,
 * but you should take care of embedding levels yourself then.
 *
 * \param shaping_flags shaping flags
 * \param embedding_levels input list of embedding levels, as returned by
 *      UBidiGetParagraphEmbeddingLevels().
 * \param len The length of the string.
 * \param ar_props The Arabic character properties as computed by
 *      UBidiJoinArabic().
 * \param ucs The Uchar32 string to shape.
 *
 * \sa UBidiShapeArabic, UBidiShapeMirroring
 *
 * Since: 4.0.0
 */
MG_EXPORT void GUIAPI UBidiShape(Uint32 shaping_flags,
        const BidiLevel *embedding_levels, int len,
        BidiArabicProp *ar_props, Uchar32* ucs);

    /**
     * \defgroup char_transform_rules Character Transformation Rules
     *
     *  The character transformation rule indicates how \a UStrGetBreaks
     *  transforms text for styling purposes; can be
     *
     *      - CTR_NONE,
     *      - or one of CTR_CAPITALIZE, CTR_UPPERCASE, and CTR_LOWERCASE,
     *      - and OR'ed with CTR_FULL_WIDTH and/or CTR_FULL_SIZE_KANA
     *
     * @{
     */

/**
 * \def CTR_NONE
 *
 * \brief No effects.
 */
#define CTR_NONE            0x00

#define CTR_CASE_MASK       0x0F

/**
 * \def CTR_CAPITALIZE
 *
 * \brief Puts the first typographic letter unit of each word,
 * if lowercase, in titlecase; other characters are unaffected.
 */
#define CTR_CAPITALIZE      0x01

/**
 * \def CTR_UPPERCASE
 *
 * \brief Puts all letters in uppercase.
 */
#define CTR_UPPERCASE       0x02

/**
 * \def CTR_LOWERCASE
 *
 * \brief Puts all letters in lowercase.
 */
#define CTR_LOWERCASE       0x03

/**
 * \def CTR_FULL_WIDTH
 *
 * \brief Puts all typographic character units in fullwidth form.
 * If a character does not have a corresponding fullwidth form,
 * it is left as is. This value is typically used to typeset
 * Latin letters and digits as if they were ideographic characters.
 */
#define CTR_FULL_WIDTH      0x10

/**
 * \def CTR_FULL_SIZE_KANA
 *
 * \brief Converts all small Kana characters to the equivalent
 * full-size Kana. This value is typically used for ruby annotation
 * text, where authors may want all small Kana to be drawn as large
 * Kana to compensate for legibility issues at the small font sizes
 * typically used in ruby.
 */
#define CTR_FULL_SIZE_KANA  0x20

    /** @} end of char_transform_rules */

    /**
     * \defgroup word_break_rules Word Breaking Rules
     *
     *  The word breaking rule indicates how \a UStrGetBreaks
     *  creates soft wrap opportunities between letters.
     *
     * @{
     */

/**
 * \def WBR_NORMAL
 *
 * \brief Words break according to their customary rules, as defined
 * by UNICODE LINE BREAKING ALGORITHM.
 */
#define WBR_NORMAL          0x00

/**
 * \def WBR_BREAK_ALL
 *
 * \brief Breaking is allowed within “words”.
 */
#define WBR_BREAK_ALL       0x01

/**
 * \def WBR_KEEP_ALL
 *
 * \brief Breaking is forbidden within “words”.
 */
#define WBR_KEEP_ALL        0x02

    /** @} end of word_break_rules */

    /**
     * \defgroup line_break_policies Line Breaking Policies
     *
     * The line breaking policy specifies the strictness
     * of line-breaking rules applied within an element:
     * especially how wrapping interacts with punctuation and symbols.
     *
     * @{
     */

/**
 * \def LBP_NORMAL
 *
 * \brief Breaks text using the most common set of line-breaking rules.
 */
#define LBP_NORMAL          0x00

/**
 * \def LBP_LOOSE
 *
 * \brief Breaks text using the least restrictive set of line-breaking rules.
 * Typically used for short lines, such as in newspapers.
 */
#define LBP_LOOSE           0x01

/**
 * \def LBP_STRICT
 *
 * \brief Breaks text using the most stringent set of line-breaking rules.
 */
#define LBP_STRICT          0x02

/**
 * \def LBP_ANYWHERE
 *
 * \brief There is a soft wrap opportunity around every typographic character
 * unit, including around any punctuation character or preserved spaces,
 * or in the middle of words, disregarding any prohibition against line
 * breaks, even those introduced by characters with the GL, WJ, or ZWJ
 * breaking class or mandated by the word breaking rule. The different wrapping
 * opportunities must not be prioritized. Hyphenation is not applied.
 */
#define LBP_ANYWHERE        0x03

    /** @} end of line_break_policies */

    /**
     * \defgroup breaking_opportunities The breaking opportunity code
     *
     * MiniGUI uses a 16-bits word to represent the character, word, and line
     * breaking opportunities of a character in a context.
     *
     * Please see UAX#29 and UAX#14 for more information:
     *
     * https://www.unicode.org/reports/tr29/tr29-33.html
     * https://www.unicode.org/reports/tr14/tr14-41.html
     *
     * @{
     */
/**
 * Unknown breaking code.
 */
#define BOV_UNKNOWN                 0x0000
/**
 * If set, the character is a whitespace character.
 */
#define BOV_WHITESPACE              0x8000
/**
 * If set, the character is an expandable space.
 */
#define BOV_EXPANDABLE_SPACE        0x0800

/**
 * If set, the character has zero width.
 */
#define BOV_ZERO_WIDTH              0x0080

#define BOV_GB_MASK                 0x7000
/**
 * If set, can break at the character when doing character wrap.
 */
#define BOV_GB_CHAR_BREAK           0x1000
/**
 * If set, cursor can appear in front of the character
 * (i.e. this is a grapheme boundary, or the first character in the text).
 */
#define BOV_GB_CURSOR_POS           0x2000
/**
 * If set, backspace deletes one character rather than
 * the entire grapheme cluster.
 */
#define BOV_GB_BACKSPACE_DEL_CH     0x4000

#define BOV_WB_MASK                 0x0700
/**
 * If set, the glyph is the word boundary as defined by UAX#29.
 */
#define BOV_WB_WORD_BOUNDARY        0x0100
/**
 * If set, the glyph is the first character in a word.
 */
#define BOV_WB_WORD_START           0x0200
/**
 * If set, the glyph is the first non-word character after a word.
 */
#define BOV_WB_WORD_END             0x0400

#define BOV_SB_MASK                 0x0070
/**
 * If set, the glyph is the sentence boundary as defined by UAX#29.
 */
#define BOV_SB_SENTENCE_BOUNDARY    0x0010
/**
 * If set, the glyph is the first character in a sentence.
 */
#define BOV_SB_SENTENCE_START       0x0020
/**
 * If set, the glyph is the first non-sentence character after a sentence.
 */
#define BOV_SB_SENTENCE_END         0x0040

#define BOV_LB_MASK                 0x000F
#define BOV_LB_BREAK_FLAG           0x0004
#define BOV_LB_MANDATORY_FLAG       0x0008
/**
 * The line can break after the character.
 */
#define BOV_LB_ALLOWED              (BOV_LB_BREAK_FLAG | 0x0001)
/**
 * The line must break after the character.
 */
#define BOV_LB_MANDATORY            (BOV_LB_BREAK_FLAG | BOV_LB_MANDATORY_FLAG | 0x0002)
/**
 * The line break is not allowed after the character.
 */
#define BOV_LB_NOTALLOWED           0x0003

/**
 * The type for breaking opportunity (Uint16).
 */
typedef Uint16 BreakOppo;

    /** @} end of breaking_opportunities */

/**
 * \fn int GUIAPI UStrGetBreaks(LanguageCode lang_code,
 *          Uint8 ctr, Uint8 wbr, Uint8 lbp,
 *          Uchar32* ucs, int nr_ucs, BreakOppo** break_oppos);
 * \brief Calculate the breaking opportunities of a Uchar32 string under
 *      the specified rules and line breaking policy.
 *
 * This function calculates the breaking opportunities of the Unicode characters
 * under the specified the writing system \a writing_system, the word breaking
 * rule \a wbr, and the line breaking policy \a lbp. This function also
 * transforms the character according to the text transformation rule \a ctr.
 *
 * The implementation of this function conforms to UNICODE LINE BREAKING
 * ALGORITHM:
 *
 * https://www.unicode.org/reports/tr14/tr14-39.html
 *
 * and UNICODE TEXT SEGMENTATION:
 *
 * https://www.unicode.org/reports/tr29/tr29-33.html
 *
 * and the CSS Text Module Level 3:
 *
 * https://www.w3.org/TR/css-text-3/
 *
 * The function will return if it encounters the end of the text.
 *
 * Note that you are responsible for freeing the break opportunities array
 * allocated by this function if it allocates the buffer.
 *
 * \param lang_code The language code; not used so far, reserved for future.
 * \param ctr The character transformation rule; see \a char_transform_rules.
 * \param wbr The word breaking rule; see \a word_break_rules.
 * \param lbp The line breaking policy; see \a line_break_policies.
 * \param ucs The Uchar32 string.
 * \param nr_ucs The length of the Uchar32 string.
 * \param break_oppos The pointer to a buffer to store the address of a
 *        Uint16 array which will return the break opportunities of the uchars.
 *        If the buffer contains a NULL value, this function will try to
 *        allocate a new space for the break opportunities.
 *        Note that the length of this array is always one longer than
 *        the Unicode array. The first unit of the array stores the
 *        break opportunity before the first uchar, and the others store
 *        the break opportunities after other gyphs.
 *        The break opportunity can be one of the following values:
 *          - BOV_LB_MANDATORY\n
 *            The mandatory breaking.
 *          - BOV_LB_NOTALLOWED\n
 *            No breaking allowed after the uchar definitely.
 *          - BOV_LB_ALLOWED\n
 *            Breaking allowed after the uchar.
 *
 * \return The length of break oppoortunities array; zero on error.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa DrawGlyphStringEx, word_break_rules, char_transform_rules,
 *      line_break_policies
 */
MG_EXPORT int GUIAPI UStrGetBreaks(LanguageCode lang_code,
        Uint8 ctr, Uint8 wbr, Uint8 lbp,
        Uchar32* ucs, int nr_ucs, BreakOppo** break_oppos);

MG_EXPORT void GUIAPI UStrTailorBreaks(ScriptType writing_system,
        const Uchar32* ucs, int nr_ucs, BreakOppo* break_oppos);

/** The function determines whether a character is alphanumeric. */
MG_EXPORT BOOL GUIAPI IsUCharAlnum(Uchar32 uc);

/** The function determines whether a character is alphabetic (i.e. a letter). */
MG_EXPORT BOOL GUIAPI IsUCharAlpha(Uchar32 uc);

/** The function determines whether a character is a control character. */
MG_EXPORT BOOL GUIAPI IsUCharControl(Uchar32 uc);

/** The function determines whether a character is numeric (i.e. a digit). */
MG_EXPORT BOOL GUIAPI IsUCharDigit(Uchar32 uc);

/** The function determines whether a character is printable and not a space */
MG_EXPORT BOOL GUIAPI IsUCharGraph(Uchar32 uc);

/** The function determines whether a character is a lowercase letter. */
MG_EXPORT BOOL GUIAPI IsUCharLowercase(Uchar32 uc);

/** The function determines whether a character is printable. */
MG_EXPORT BOOL GUIAPI IsUCharPrint(Uchar32 uc);

/** The function determines whether a character is a uppercase letter. */
MG_EXPORT BOOL GUIAPI IsUCharUppercase(Uchar32 uc);

/** The function determines whether a character is punctuation or a symbol. */
MG_EXPORT BOOL GUIAPI IsUCharPunct(Uchar32 uc);

/** The function determines whether a character is a space, tab, or line separator
 * (newline, carriage return, etc.). */
MG_EXPORT BOOL GUIAPI IsUCharSpace(Uchar32 uc);

/** The function determines whether a character is a mark (non-spacing mark,
 * combining mark, or enclosing mark in Unicode speak). */
MG_EXPORT BOOL GUIAPI IsUCharMark(Uchar32 uc);

/**
 * The function determines if a character is titlecase. Some characters in
 * Unicode which are composites, such as the DZ digraph
 * have three case variants instead of just two. The titlecase
 * form is used at the beginning of a word where only the
 * first letter is capitalized. The titlecase form of the DZ
 * digraph is U+01F2 LATIN CAPITAL LETTTER D WITH SMALL LETTER Z.
 */
MG_EXPORT BOOL GUIAPI IsUCharTitle(Uchar32 uc);

/** The function determines if a character is a hexidecimal digit. */
MG_EXPORT BOOL GUIAPI IsUCharXDigit(Uchar32 uc);

/** The function determines if a given character is assigned in the Unicode standard. */
MG_EXPORT BOOL GUIAPI IsUCharDefined(Uchar32 uc);

/** The function determines if a given character typically takes zero width when rendered. */
MG_EXPORT BOOL GUIAPI IsUCharZeroWidth(Uchar32 uc);

/** The function determines if a character is typically rendered in a double-width cell. */
MG_EXPORT BOOL GUIAPI IsUCharWide(Uchar32 uc);

/**
 * The function determines if a character is typically rendered in a double-width
 * cell under legacy East Asian locales.  If a character is wide according to
 * g_unichar_iswide(), then it is also reported wide with this function, but
 * the converse is not necessarily true. See the
 * [Unicode Standard Annex #11](https://www.unicode.org/reports/tr11/)
 * for details.
 */
MG_EXPORT BOOL GUIAPI IsUCharWideCJK(Uchar32 uc);

/** Converts a character to uppercase.  */
MG_EXPORT Uchar32 UCharToUpper (Uchar32 uc);

/** Converts a character to lower case. */
MG_EXPORT Uchar32 GUIAPI UCharToLower (Uchar32 uc);

/** Converts a glyph to the titlecase. */
MG_EXPORT Uchar32 GUIAPI UCharToTitle (Uchar32 uc);

/** Converts a glyph to full-width. */
MG_EXPORT Uchar32 GUIAPI UCharToFullWidth (Uchar32 uc);

/** Converts a glyph to single-width. */
MG_EXPORT Uchar32 GUIAPI UCharToSingleWidth (Uchar32 uc);

/** Converts a glyph to full-size Kana. */
MG_EXPORT Uchar32 GUIAPI UCharToFullSizeKana (Uchar32 uc);

/** Converts a glyph to small Kana. */
MG_EXPORT Uchar32 GUIAPI UCharToSmallKana (Uchar32 uc);

/** Determine is the given Unicode character an Arabic vowel. */
MG_EXPORT BOOL GUIAPI UCharIsArabicVowel(Uchar32 uc);

/** Determine the canonical combining class of a Unicode character.*/
MG_EXPORT int GUIAPI UCharCombiningClass (Uchar32 uc);

/**
 * Computes the canonical ordering of a string in-place.
 * This rearranges decomposed characters in the string
 * according to their combining classes.
 * See the Unicode manual for more information.
 */
MG_EXPORT void GUIAPI UCharCanonicalOrdering (Uchar32 *string, int len);

/** Performs a single decomposition step of the Unicode canonical decomposition algorithm. */
MG_EXPORT BOOL GUIAPI UCharCompose (Uchar32 a, Uchar32 b, Uchar32 *ch);

/** Performs a single composition step of the Unicode canonical composition algorithm. */
MG_EXPORT BOOL GUIAPI UCharDecompose (Uchar32 ch, Uchar32 *a, Uchar32 *b);

/**
 * \fn int GUIAPI UCharFullyDecompose (Uchar32 ch, BOOL compat,
 *      Uchar32 *result, int result_len)
 *
 * Computes the canonical or compatibility decomposition of a
 * Unicode character.
 *
 * For compatibility decomposition, pass TRUE for \a compat;
 * for canonical decomposition pass FALSE for \a compat.
 */
MG_EXPORT int GUIAPI UCharFullyDecompose (Uchar32 ch, BOOL compat,
        Uchar32 *result, int result_len);

/**
 * \fn ScriptType GUIAPI UCharGetScriptType (Uchar32 ch)
 *
 * Looks up the script code for a particular character (as defined
 * by Unicode Standard Annex #24). No check is made for \a ch being a
 * valid Unicode character; if you pass in invalid character, the
 * result is undefined.
 *
 * \param ch a Unicode character
 *
 * \return The script type code for the UNICODE character.
 *
 * \sa ScriptType
 *
 * Since: 4.0.0
 */
MG_EXPORT ScriptType GUIAPI UCharGetScriptType (Uchar32 ch);

/**
 * \fn Uint32 ScriptTypeToISO15924 (ScriptType script)
 *
 * Looks up the ISO15924 code for the specific ScriptType \a script.
 * ISO15924 assigns four-letter codes to scripts. For example,
 * the code for Arabic is 'Arab'.  The four letter codes are encoded
 * as a Uint32 by this function in a big-endian fashion.
 * That is, the code returned for Arabic is 0x41726162 (0x41 is ASCII
 * code for 'A', 0x72 is ASCII code for 'r', etc).
 *
 * See
 * [Codes for the representation of names of scripts](https://www.unicode.org/iso15924/codelists.html)
 * for details.
 *
 * \param script a Unicode script
 *
 * \return the ISO 15924 code for \a script, encoded as an integer,
 *   of zero if script is SCRIPT_INVALID_CODE or
 *   ISO 15924 code 'Zzzz' (script code for UNKNOWN) if \a script
 *   is not understood.
 *
 * \sa ScriptTypeFromISO15924
 *
 * Since: 4.0.0
 */
MG_EXPORT Uint32 GUIAPI ScriptTypeToISO15924 (ScriptType script);

/**
 * \fn ScriptType ScriptTypeFromISO15924 (Uint32 iso15924)
 * \brief Get the ScriptType value from an encoded ISO15924 script code.
 *
 * Looks up the Unicode script type for the specified encoded ISO15924
 * script code \a iso15924. ISO 15924 assigns four-letter
 * codes to scripts. For example, the code for Arabic is 'Arab'.
 * This function accepts four letter codes encoded as a Uint32 in a
 * big-endian fashion. That is, the code expected for Arabic is
 * 0x41726162 (0x41 is ASCII code for 'A', 0x72 is ASCII code for 'r', etc).
 *
 * See
 * [Codes for the representation of names of scripts](https://www.unicode.org/iso15924/codelists.html)
 * for details.
 *
 * \param iso15924 an encoded Unicode script code in ISO15924.
 *
 * \return the Unicode script for \a iso15924, or
 *   of \a SCRIPT_INVALID_CODE if \a iso15924 is zero and
 *   \a SCRIPT_UNKNOWN if \a iso15924 is unknown.
 *
 * \sa ScriptTypeFromISO15924Code
 *
 * Since: 4.0.0
 */
MG_EXPORT ScriptType GUIAPI ScriptTypeFromISO15924 (Uint32 iso15924);

/**
 * \fn ScriptType ScriptTypeFromISO15924Code (const char* iso15924)
 *
 * Looks up the Unicode script type for the specified ISO15924 script code
 * \a iso15924. ISO15924 assigns four-letter codes to scripts.
 * For example, the code for Arabic is 'Arab'.
 *
 * See
 * [Codes for the representation of names of scripts](https://www.unicode.org/iso15924/codelists.html)
 * for details.
 *
 * \param iso15924 a Unicode script type code in ISO15924.
 *
 * \return the Unicode script for \a iso15924, or
 *   of \a SCRIPT_INVALID_CODE if \a iso15924 is zero and
 *   \a SCRIPT_UNKNOWN if \a iso15924 is unknown.
 *
 * \sa ScriptTypeFromISO15924
 *
 * Since: 4.0.0
 */
static inline ScriptType GUIAPI ScriptTypeFromISO15924Code (const char* iso15924)
{
    return ScriptTypeFromISO15924(MAKEDWORD32(iso15924[3],
            iso15924[2], iso15924[1], iso15924[0]));
}

typedef enum _UVerticalOrient {
    UCHAR_VOP_U = 0,
    UCHAR_VOP_R,
    UCHAR_VOP_TU,
    UCHAR_VOP_TR,
} UVerticalOrient;

typedef enum {
    GLYPH_RUN_DIR_LTR = 0,
    GLYPH_RUN_DIR_RTL,
    GLYPH_RUN_DIR_TTB,
    GLYPH_RUN_DIR_BTT,
} GlyphRunDir;

typedef enum {
    GLYPH_GRAVITY_SOUTH = 0,
    GLYPH_GRAVITY_EAST,
    GLYPH_GRAVITY_NORTH,
    GLYPH_GRAVITY_WEST,
    GLYPH_GRAVITY_AUTO,
} GlyphGravity;

typedef enum {
    GLYPH_GRAVITY_POLICY_NATURAL,
    GLYPH_GRAVITY_POLICY_STRONG,
    GLYPH_GRAVITY_POLICY_LINE,
    GLYPH_GRAVITY_POLICY_MIXED,
} GlyphGravityPolicy;

/** Get the vertical orientation property of a Unicode character */
MG_EXPORT UVerticalOrient GUIAPI UCharGetVerticalOrientation(Uchar32 uc);

/**
 * \fn GlyphGravity GUIAPI ScriptGetGlyphGravity(ScriptType script,
 *      GlyphGravity base_gravity, GlyphGravityPolicy policy)
 * \brief Based on the script, base gravity, and policy, returns actual gravity
 *      to use in laying out a single glyph run.
 *
 * If \a base_orient is \a GLYPH_GRAVITY_AUTO, it is first replaced with the
 * preferred orientation of \a script. To get the preferred orientation of a script,
 * pass \a GLYPH_GRAVITY_AUTO and \a GLYPH_GRAVITY_POLICY_STRONG in.
 *
 * \param script The script type to query
 * \param vertical TRUE for vertical layout
 * \param base_gravity The base gravity of the layout
 * \param policy The gravity policy
 *
 * \return The resolved gravity suitable to use for a layout run of text
 * with \a script.
 *
 * Since: 4.0.0
 */
MG_EXPORT GlyphGravity GUIAPI ScriptGetGlyphGravity(ScriptType script,
        BOOL vertical, GlyphGravity base_gravity, GlyphGravityPolicy policy);

/**
 * \fn GlyphGravity GUIAPI ScriptGetGlyphGravityForWide (ScriptType script,
 *      BOOL wide, GlyphGravity base_gravity, GlyphGravityPolicy policy)
 * \brief Based on the script, East Asian width, base gravity, and policy,
 *      returns the actual gravity to use in laying out a single character
 *      or a run of text.
 *
 * This function is similar to ScriptGetGlyphGravity() except
 * that this function makes a distinction between narrow/half-width and
 * wide/full-width characters also.  Wide/full-width characters always
 * stand upright, that is, they always take the
 * base gravity, whereas narrow/half-width characters are always
 * rotated in vertical context.
 *
 * If \a base_orient is \a GLYPH_GRAVITY_AUTO, it is first replaced with the
 * preferred gravity of \a script.
 *
 * \param script The script type to query
 * \param vertical TRUE for vertical layout.
 * \param wide TRUE for wide characters as returned by IsUCharWide()
 * \param base_gravity The base gravity of the paragraph
 * \param policy The gravity policy
 *
 * \return The resolved gravity suitable to use for a run of text
 * with \a script and \a wide.
 *
 * Since: 4.0.0
 */
GlyphGravity ScriptGetGlyphGravityForWide (ScriptType script,
        BOOL vertical, BOOL wide,
        GlyphGravity base_gravity, GlyphGravityPolicy policy);

    /** @} end of unicode_ops */

#endif /* _MGCHARSET_UNICODE */

    /**
     * \defgroup text_output_fns Text output functions
     * @{
     */

/**
 * \fn int GUIAPI GetFontHeight (HDC hdc)
 * \brief Retrieve the height of the current logical font in a DC.
 *
 * This function retrieves the height of the current logical font in
 * the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The height of the current logical font.
 *
 * \sa GetMaxFontWidth
 */
MG_EXPORT int GUIAPI GetFontHeight (HDC hdc);

/**
 * \fn int GUIAPI GetMaxFontWidth (HDC hdc)
 * \brief Retrieve the maximal character width of the current logical font
 *        in a DC.
 *
 * This function retrieves the maximal character width of the current
 * logical font in the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The maximal character width of the current logical font.
 *
 * \sa GetFontHeight
 */
MG_EXPORT int GUIAPI GetMaxFontWidth (HDC hdc);


/**
 * \fn int GUIAPI GetTextExtent (HDC hdc, \
                const char* spText, int len, SIZE* pSize)
 * \brief Computes the output extent of a string of text.
 *
 * This function computes the output extent of the text (\a spText) with
 * length of \a len. This function returns the extent in a SIZE struct
 * pointed to by \a pSize and the width of text as the return value.
 *
 * \param hdc The device context.
 * \param spText The pointer to the string of text.
 * \param len The length of the text.
 * \param pSize The output extent will be returned through this buffer.
 *
 * \return The width of the text in baseline direction.
 *
 * \sa GetTabbedTextExtent
 */
MG_EXPORT int GUIAPI GetTextExtent (HDC hdc,
                const char* spText, int len, SIZE* pSize);

/**
 * \fn int GUIAPI GetTabbedTextExtent (HDC hdc, \
                const char* spText, int len, SIZE* pSize)
 * \brief Computes the output extent of a formatted text.
 *
 * This function computes the output extent of the formatted text (\a spText)
 * with length of \a len. This function returns the extent in a SIZE struct
 * pointed to by \a pSize and the width of text as the return value.
 * The output extent will be affected by the default tab size if there are
 * some TAB characters in the text.
 *
 * \param hdc The device context.
 * \param spText The pointer to the string of text.
 * \param len The length of the text.
 * \param pSize The output extent will be returned through this buffer.
 *
 * \return The width of the text in baseline direction.
 *
 * \sa GetTextExtent
 */
MG_EXPORT int GUIAPI GetTabbedTextExtent (HDC hdc,
                const char* spText, int len, SIZE* pSize);

/**
 * \def GetTextCharacterExtra(hdc)
 * \brief Retrieve the current inter-character spacing for the DC.
 *
 * \sa SetTextCharacterExtra
 */
#define GetTextCharacterExtra(hdc)  GetDCAttr (hdc, DC_ATTR_CHAR_EXTRA)

/**
 * \def GetTextWordExtra(hdc)
 * \brief Retrieve the current inter-word spacing for the DC.
 *
 * \sa SetTextWordExtra
 */
#define GetTextWordExtra(hdc)  GetDCAttr (hdc, DC_ATTR_WORD_EXTRA)

/**
 * \def GetTextAboveLineExtra(hdc)
 * \brief Retrieve the current spacing above line for the DC.
 *
 * \sa SetTextAboveLineExtra
 */
#define GetTextAboveLineExtra(hdc)  GetDCAttr (hdc, DC_ATTR_ALINE_EXTRA)

/**
 * \def GetTextBellowLineExtra(hdc)
 * \brief Retrieve the current spacing bellow line for the DC.
 *
 * \sa SetTextBellowLineExtra
 */
#define GetTextBellowLineExtra(hdc) GetDCAttr (hdc, DC_ATTR_BLINE_EXTRA)

/**
 * \def SetTextCharacterExtra(hdc, extra)
 * \brief Set the inter-character spacing for the DC and returns
 *        the old spacing value.
 *
 * \sa GetTextCharacterExtra
 */
#define SetTextCharacterExtra(hdc, extra)       \
                SetDCAttr (hdc, DC_ATTR_CHAR_EXTRA, (DWORD) extra)

/**
 * \def SetTextWordExtra(hdc, extra)
 * \brief Set the inter-word spacing for the DC and returns
 *        the old spacing value.
 *
 * \sa GetTextWordExtra
 */
#define SetTextWordExtra(hdc, extra)       \
                SetDCAttr (hdc, DC_ATTR_WORD_EXTRA, (DWORD) extra)

/**
 * \def SetTextAboveLineExtra(hdc, extra)
 * \brief Set the spacing above line for the DC and returns the old value.
 *
 * \sa GetTextAboveLineExtra
 */
#define SetTextAboveLineExtra(hdc, extra)       \
                SetDCAttr (hdc, DC_ATTR_ALINE_EXTRA, (DWORD) extra)

/**
 * \def SetTextBellowLineExtra(hdc, extra)
 * \brief Set the spacing bellow line for the DC and returns the old value.
 *
 * \sa GetTextBellowLineExtra
 */
#define SetTextBellowLineExtra(hdc, extra)      \
                SetDCAttr (hdc, DC_ATTR_BLINE_EXTRA, (DWORD) extra)

#define TA_LEFT         0x0000
#define TA_RIGHT        0x0001
#define TA_CENTER       0x0002
#define TA_X_MASK       0x000F

#define TA_TOP          0x0000
#define TA_BASELINE     0x0010
#define TA_BOTTOM       0x0020
#define TA_Y_MASK       0x00F0

#define TA_NOUPDATECP   0x0000
#define TA_UPDATECP     0x0100
#define TA_CP_MASK      0x0F00

/**
 * \def GetTextAlign(hdc)
 * \brief Retrieve the current text-alignment flags of a DC.
 *
 * \param hdc The device context.
 *
 * \return The return value is one or more of the following values:
 *
 *  - TA_TOP\n
 *      Specifies alignment of the y-axis and the top of the bounding
 *      rectangle.
 *  - TA_BASELINE\n
 *      Specifies alignment of the y-axis and the baseline of the chosen font
 *      within the bounding rectangle.
 *  - TA_BOTTOM\n
 *      Specifies alignment of the y-axis and the bottom of the bounding
 *      rectangle.
 *  - TA_LEFT\n
 *      Specifies alignment of the x-axis and the left side of the bounding
 *      rectangle.
 *  - TA_RIGHT\n
 *      Specifies alignment of the x-axis and the right side of the bounding
 *      rectangle.
 *  - TA_CENTER\n
 *      Specifies alignment of the x-axis and the center of the bounding
 *      rectangle. Note that MiniGUI does not support TA_CENTER so far.
 *  - TA_NOUPDATECP\n
 *      Specifies that the current position is not updated.
 *  - TA_UPDATECP\n
 *      Specifies that the current position is updated.
 *
 * \note The text-alignment flags determine how the TextOut, TabbedTextOut,
 * and TabbedTextOutEx functions align a string of text in relation to
 * the string's starting point. The text-alignment flags are not
 * necessarily single-bit flags and may be equal to 0. To test whether a
 * flag is set, an application should follow these steps:
 *
 * 1. Apply the bitwise OR operator to the flag and its related flags,
 *    grouped as follows:
 *
 *  - TA_LEFT, TA_RIGHT, TA_CENTER (TA_X_MASK) \n
 *  - TA_BASELINE, TA_BOTTOM, and TA_TOP (TA_Y_MASK) \n
 *  - TA_NOUPDATECP and TA_UPDATECP (TA_CP_MASK) \n
 *
 * 2. Apply the bitwise-AND operator to the result and the return value of
 *    GetTextAlign.
 *
 * 3. Test for the equality of this result and the flag.
 *
 * \sa SetTextAlign
 */
#define GetTextAlign(hdc)         GetDCAttr (hdc, DC_ATTR_TEXT_ALIGN)

/**
 * \def SetTextAlign(hdc, ta_flags)
 * \brief Set text-alignment flags of a DC.
 *
 * \param hdc The device context.
 * \param ta_flags The flags specify the relationship between a point and a
 * rectangle that bounds the text. The point can be either the current
 * position or coordinates specified by a text-output function. The rectangle
 * that bounds the text is defined by the adjacent character cells in the text
 * string. The ta_flags parameter can be one or more flags from the following
 * three categories. Choose only one flag from each category.
 *
 * The first category affects text alignment in the x-direction:
 *  - TA_LEFT\n
 *      Aligns the point with the left side of the bounding rectangle.
 *      This is the default setting.
 *  - TA_RIGHT\n
 *      Aligns the point with the right side of the bounding rectangle.
 *  - TA_CENTER\n
 *      Aligns the point with the horizontal center of the bounding rectangle.
 *      Note that MiniGUI does not support TA_CENTER so far.
 *
 * The second category affects text alignment in the y-direction:
 *  - TA_BASELINE\n
 *      Aligns the point with the base line of the chosen font.
 *  - TA_BOTTOM\n
 *      Aligns the point with the bottom of the bounding rectangle.
 *  - TA_TOP\n
 *      Aligns the point with the top of the bounding rectangle.
 *      This is the default setting.
 *
 * The third category determines whether the current position is
 * updated when text is written:
 *  - TA_NOUPDATECP\n
 *      Does not update the current position after each call to a
 *      text-output function. This is the default setting.
 *  - TA_UPDATECP\n
 *      Updates the current x-position after each call to a text-output
 *      function. The new position is at the right side of the bounding
 *      rectangle for the text.  When this flag is set, the coordinates
 *      specified in calls to the TextOut function are ignored.
 *
 * \return The previous text-alignment setting, if successful.
 *      The low-order word contains the horizontal setting and the
 *      high-order word contains the vertical setting; otherwise 0.
 *
 * \note The TextOut, TabbedTextOut, and TabbedTextOutEx functions
 * use these flags when positioning a string of text on a DC.
 * The flags specify the relationship between a specific point
 * and a rectangle that bounds the text. The coordinates of this point
 * are passed as parameters to the TextOut function. The rectangle that
 * bounds the text is formed by the adjacent character cells in the
 * text string.
 *
 * \sa GetTextAlign
 */
#define SetTextAlign(hdc, ta_flags)         \
            SetDCAttr (hdc, DC_ATTR_TEXT_ALIGN, (DWORD)ta_flags)

#define BIDI_FLAG_LTR       0x00
#define BIDI_FLAG_RTL       0x10

#define GetBIDIFlags(hdc)                   \
            GetDCAttr (hdc, DC_ATTR_BIDI_FLAGS)

#define SetBIDIFlags(hdc, bidi_flags)       \
            SetDCAttr (hdc, DC_ATTR_BIDI_FLAGS, (DWORD)bidi_flags)

/**
 * \fn int GUIAPI TextOutLen (HDC hdc, int x, int y, \
                const char* spText, int len)
 * \brief Outputs a string of text.
 *
 * This function outputs the text \a spText with length of \a len at a (x,y).
 * This function returns the width of text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The string of the text.
 * \param len The length of the text. If it is less than 0, MiniGUI will
 *        treat it as a null-terminated string.
 * \return The output width of the text.
 *
 * \sa TabbedTextOutLen
 */
MG_EXPORT int GUIAPI TextOutLen (HDC hdc, int x, int y,
                const char* spText, int len);


/**
 * \fn int GUIAPI TextOutOmitted (HDC hdc, int x, int y, \
                const char *mtext, int len, int max_extent)
 * \brief Outputs a string of text with omitted format.
 *
 * This function outputs the text \a mtext with length of \a len at a (x,y) with
 * omitted format, if width of the text more than \a max_extent.This function
 * returns the output width of the text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param mtext The string of the text.
 * \param len The length of the text. If it is less than 0, MiniGUI will
 *        treat it as a null-terminated string.
 * \param max_extent The max extent of output text.
 *
 * \return The output width of the text.
 *
 * \sa TextOutLen
 */
MG_EXPORT int GUIAPI TextOutOmitted (HDC hdc, int x, int y,
                const char *mtext, int len, int max_extent);

/**
 * \fn int GUIAPI TabbedTextOutLen (HDC hdc, int x, int y, \
                const char* spText, int len)
 * \brief Outputs a formatted text.
 *
 * This function outputs the formatted text \a spText with length of \a len
 * at \a (x,y). This function returns the width of text. The output will start
 * a new line if there is a line feed character '\n' in the text. The output
 * will be affected by the default tab size if there are some TAB characters
 * in the text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The formatted text.
 * \param len The length of the text. If it is less than 0, MiniGUI will
 *        treat it as a null-terminated string.
 * \return The output width of the text.
 *
 * \sa TextOutLen
 */
MG_EXPORT int GUIAPI TabbedTextOutLen (HDC hdc, int x, int y,
                const char* spText, int len);

/**
 * \fn int GUIAPI TabbedTextOutEx (HDC hdc, int x, int y, \
                const char* spText, int nCount, int nTabPositions, \
                int *pTabPositions, int nTabOrigin)
 * \brief Writes a character string at a specified location, expanding tabs
 *        to the values specified in an anrry of tab-stop positions.
 *
 * This function writes the string \a spText with length of \a nCount at
 * a specified locations, expanding tabs to the value spcified in the
 * array \a pTabPositions of tab-stop positions. The output will start
 * a new line if there is a line feed character '\n' in the text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The formatted text.
 * \param nCount The length of the text. If it is less than 0, MiniGUI will
 *        treat it as a null-terminated string.
 * \param nTabPositions The length of the array of the tab-stop positions.
 * \param pTabPositions The array of the tab-stop positions.
 * \param nTabOrigin The origin of the tab-stops.
 *
 * \return The output width of the text.
 *
 * \sa TabbedTextOutLen
 */
MG_EXPORT int GUIAPI TabbedTextOutEx (HDC hdc, int x, int y,
                const char* spText, int nCount,
                int nTabPositions, int *pTabPositions, int nTabOrigin);

/**
 * \fn void GUIAPI GetLastTextOutPos (HDC hdc, POINT* pt)
 * \brief Retrieve the last text output position.
 *
 * \param hdc The device context.
 * \param pt The last text output position will be returned through
 *        this pointer.
 *
 */
MG_EXPORT void GUIAPI GetLastTextOutPos (HDC hdc, POINT* pt);

/**
 * \def TextOut(hdc, x, y, text)
 * \brief Outputs text.
 *
 * Defined as a macro calling \a TextOutLen passing \a len as -1.
 *
 * \sa TextOutLen
 */
#define TextOut(hdc, x, y, text)    TextOutLen (hdc, x, y, text, -1)

/**
 * \def TabbedTextOut(hdc, x, y, text)
 * \brief Outputs formatted text.
 *
 * Defined as a macro calling \a TabbedTextOutLen passing \a len as -1.
 *
 * \sa TextOutLen
 */
#define TabbedTextOut(hdc, x, y, text)  TabbedTextOutLen (hdc, x, y, text, -1)

#define DT_TOP              0x00000000
#define DT_LEFT             0x00000000
#define DT_CENTER           0x00000001
#define DT_RIGHT            0x00000002
#define DT_VCENTER          0x00000004
#define DT_BOTTOM           0x00000008
#define DT_WORDBREAK        0x00000010
#define DT_SINGLELINE       0x00000020
#define DT_EXPANDTABS       0x00000040
#define DT_TABSTOP          0x00000080
#define DT_NOCLIP           0x00000100
#define DT_EXTERNALLPADING  0x00000200
#define DT_CALCRECT         0x00000400
#define DT_NOPREFIX         0x00000800
#define DT_INTERNAL         0x00001000
#define DT_CHARBREAK        0x00002000

/**
 * First line information of DrawTextEx2
 */
typedef struct _DTFIRSTLINE
{
    /** The number of the characters of first line in bytes. */
    int nr_chars;
    /** The output x-coordinate of the fist line. */
    int startx;
    /** The output y-coordinate of the fist line. */
    int starty;
    /** The output width of the fist line. */
    int width;
    /** The output height of the fist line. */
    int height;
} DTFIRSTLINE;

/**
 * \fn int GUIAPI DrawTextEx2 (HDC hdc, const char* pText, int nCount, \
                RECT* pRect, int nIndent, UINT nFormat, \
                DTFIRSTLINE* firstline)
 * \brief Draws a formatted text in a rectangle.
 *
 * This function draws formatted text (\a pText) in the specified rectangle
 * (\a pRect). It formats the text according to the specified method
 * (through \a nFormat, including expanding tabs, justifying characters,
 * breaking lines, and so forth).
 *
 * \param hdc The device context.
 * \param pText The formatted text.
 * \param nCount The length of the text. If it is less than 0, MiniGUI will
 *        treat it as a null-terminated string.
 * \param pRect The output rectangle.
 * \param nIndent The indent value of the first line.
 * \param nFormat The methods used to format the text. MiniGUI support
 *        the following method to format text so far:
 *
 *  - DT_TOP\n
 *    Top-justifies text (single line only).
 *  - DT_LEFT\n
 *    Aligns text to the left.
 *  - DT_CENTER\n
 *    Centers text horizontally in the rectangle.
 *  - DT_RIGHT\n
 *    Aligns text to the right.
 *  - DT_VCENTER\n
 *    Centers text vertically (single line only).
 *  - DT_BOTTOM\n
 *    Justify the text to the bottom of the rectangle. This value must be
 *    combined with DT_SINGLELINE.
 *  - DT_WORDBREAK\n
 *    Break words. Lines are automatically broken between words if a word
 *    would extend past the edge of the rectangle specified by the the pRect
 *    parameter. A carriage return or linefeed also breaks the line.
 *  - DT_CHARBREAK\n
 *    Break characters. Lines are automatically broken between characters
 *    if a character would extend past the edge of the rectangle specified
 *    by the the pRect parameter. A carriage return or linefeed also
 *    breaks the line. DT_CHARBREAK will override DT_WORDBREAK.
 *  - DT_SINGLELINE\n
 *    Display text on the single line only. Carriage returns and linefeeds
 *    do not break the line.
 *  - DT_EXPANDTABS\n
 *    Expands tab characters. The default number of character per tab is eight.
 *  - DT_TABSTOP\n
 *    Set tab stops. Bits 15-8 (high-order byte of the low-order word) of
 *    the \a uForma parameter specify the number of characters for each tab.
 *    The default number of characters per tab is eight.
 *  - DT_NOCLIP\n
 *    Draws without clipping. \a DrawText is somewhat faster when DT_NOCLIP is
 *    used.
 *  - DT_CALCRECT\n
 *    Determine the width and the height of the rectangle. If there are
 *    multiple lines of text, \a DrawText uses the width of the rectangle
 *    pointed to by the \a lpRect parameter and extends the base of the
 *    rectangle to bound the last line of text. If there is only one line of
 *    text, \a DrawText modifies the right side of the rectangle so that it
 *    bounds the last character in the line. In either case, \a DrawText
 *    returns the height of the formatted text but does not draw the text.
 *
 * \param firstline If not null, DrawTextEx2 will only calculate the first
 *        line will be output in the rectangle, and return the number of
 *        the characters and the output width.
 *
 * \return The output height of the formatted text.
 *
 * \sa DrawText
 *
 * Example:
 *
 * \include drawtext.c
 */
MG_EXPORT int GUIAPI DrawTextEx2 (HDC hdc, const char* pText, int nCount,
                RECT* pRect, int nIndent, UINT nFormat, DTFIRSTLINE *firstline);

/**
 * \def DrawText(hdc, text, n, rc, format)
 * \brief Draws a formatted text in a rectangle.
 *
 * Defined as a macro calling \a DrawTextEx2 passing \a nIndent as 0
 * and \a firstline as NULL.
 *
 * \sa DrawTextEx2
 */
#define DrawText(hdc, text, n, rc, format)          \
                DrawTextEx2 (hdc, text, n, rc, 0, format, NULL)

/**
 * \def DrawTextEx(hdc, text, n, rc, indent, format)
 * \brief Draws a formatted text in a rectangle.
 *
 * Defined as a macro calling \a DrawTextEx2 passing \a firstline as NULL.
 *
 * \sa DrawTextEx2
 */
#define DrawTextEx(hdc, text, n, rc, indent, format)    \
                DrawTextEx2 (hdc, text, n, rc, indent, format, NULL)
    /** @} end of text_output_fns */

    /**
     * \defgroup bmp_struct Bitmap structure
     *
     * MiniGUI uses a MYBITMAP structure to represent a device-independent
     * bitmap, and BITMAP structure to represent a device-dependent bitmap.
     *
     * @{
     */

#define MYBMP_TYPE_NORMAL       0x00000000
#define MYBMP_TYPE_RLE4         0x00000001
#define MYBMP_TYPE_RLE8         0x00000002
#define MYBMP_TYPE_RGB          0x00000003
#define MYBMP_TYPE_BGR          0x00000004
#define MYBMP_TYPE_RGBA         0x00000005
#define MYBMP_TYPE_MASK         0x0000000F

#define MYBMP_FLOW_DOWN         0x00000010
#define MYBMP_FLOW_UP           0x00000020
#define MYBMP_FLOW_MASK         0x000000F0

#define MYBMP_TRANSPARENT       0x00000100
#define MYBMP_ALPHACHANNEL      0x00000200
#define MYBMP_ALPHA             0x00000400

#define MYBMP_RGBSIZE_3         0x00001000
#define MYBMP_RGBSIZE_4         0x00002000

#define MYBMP_LOAD_GRAYSCALE    0x00010000
#define MYBMP_LOAD_ALLOCATE_ONE 0x00020000
#define MYBMP_LOAD_NONE         0x00000000

/** Device-independent bitmap structure. */
struct _MYBITMAP
{
    /**
     * Flags of the bitmap, can be OR'ed by the following values:
     *  - MYBMP_TYPE_NORMAL\n
     *    A normal palette bitmap.
     *  - MYBMP_TYPE_RGB\n
     *    A RGB bitmap.
     *  - MYBMP_TYPE_BGR\n
     *    A BGR bitmap.
     *  - MYBMP_TYPE_RGBA\n
     *    A RGBA bitmap.
     *  - MYBMP_FLOW_DOWN\n
     *    The scanline flows from top to bottom.
     *  - MYBMP_FLOW_UP\n
     *    The scanline flows from bottom to top.
     *  - MYBMP_TRANSPARENT\n
     *    Have a trasparent value.
     *  - MYBMP_ALPHACHANNEL\n
     *    Have a alpha channel.
     *  - MYBMP_ALPHA\n
     *    Have a per-pixel alpha value.
     *  - MYBMP_RGBSIZE_3\n
     *    Size of each RGB triple is 3 bytes.
     *  - MYBMP_RGBSIZE_4\n
     *    Size of each RGB triple is 4 bytes.
     *  - MYBMP_LOAD_GRAYSCALE\n
     *    Tell bitmap loader to load a grayscale bitmap.
     *  - MYBMP_LOAD_ALLOCATE_ONE\n
     *    Tell bitmap loader to allocate space for only one scanline.
     */
    DWORD flags;
    /** The number of the frames. */
    int   frames;
    /** The pixel depth. */
    Uint8 depth;
    /** The alpha channel value. */
    Uint8 alpha;
    Uint8 reserved [2];
    /** The transparent pixel. */
    Uint32 transparent;

    /** The width of the bitmap. */
    Uint32 w;
    /** The height of the bitmap. */
    Uint32 h;
    /** The pitch of the bitmap. */
    Uint32 pitch;
    /** The size of the bits of the bitmap. */
    Uint32 size;

    /** The pointer to the bits of the bitmap. */
    BYTE* bits;
};

#define BMP_TYPE_NORMAL         0x00
#define BMP_TYPE_RLE            0x01
#define BMP_TYPE_ALPHA          0x02
#define BMP_TYPE_ALPHACHANNEL   0x04
#define BMP_TYPE_COLORKEY       0x10
#define BMP_TYPE_ALPHA_MASK     0x20
#define BMP_TYPE_REPLACEKEY     0x40

/** Expanded device-dependent bitmap structure. */
struct _BITMAP
{
    /**
     * Bitmap types, can be OR'ed by the following values:
     *  - BMP_TYPE_NORMAL\n
     *    A nomal bitmap, without alpha and color key.
     *  - BMP_TYPE_RLE\n
     *    A RLE (run-length-encode) encoded bitmap.
     *    The encoding translates the pixel data to a stream of segments of
     *    the form
     *
     *      \<skip\> \<run\> \<data\>
     *
     *    where \<skip\> is the number of transparent pixels to skip,
     *          \<run\>  is the number of opaque pixels to blit,
     *    and   \<data\> are the pixels themselves.
     *
     *    Encoded pixels always have the same format as the BITMAP.
     *    \<skip\> and \<run\> are unsigned 8 bit integers, except for 32 bit depth
     *    where they are 16 bit. This makes the pixel data aligned at all times.
     *    Segments never wrap around from one scan line to the next.
     *    The end of one segment is marked by a zero \<skip\>,\<run\> pair.
     *  - BMP_TYPE_ALPHA\n
     *    Per-pixel alpha in the bitmap.
     *  - BMP_TYPE_ALPHACHANNEL\n
     *    The \a bmAlpha is a valid alpha channel value.
     *  - BMP_TYPE_COLORKEY\n
     *    The \a bmColorKey is a valid color key value.
     *  - BMP_TYPE_ALPHA_MASK\n
     *    The \a bmAlphaMask and \a bmAlphaPitch are valid.
     *  - BMP_TYPE_REPLACEKEY\n
     *    The \a bmColorRep is valid. Any pixel which is equal to \a bmColorKey will
     *    be replaced by \a bmColorRep.
     */
    Uint8   bmType;
    /** The bits per piexel. */
    Uint8   bmBitsPerPixel;
    /** The bytes per piexel. */
    Uint8   bmBytesPerPixel;
    /** The alpha channel value. */
    Uint8   bmAlpha;
    /** The color key value. */
    Uint32  bmColorKey;
    /** The pixel value used to replace the color key. */
    Uint32  bmColorRep;

    /** The width of the bitmap */
    Uint32  bmWidth;
    /** The height of the bitmap */
    Uint32  bmHeight;
    /** The pitch of the bitmap */
    Uint32  bmPitch;
    /** The bits of the bitmap */
    Uint8*  bmBits;

    /* Obsolte.
     The private pixel format
    void*   bmAlphaPixelFormat; */

    /** The Alpha mask of the bitmap */
    Uint8*  bmAlphaMask;

    /** The pitch of the Alpha mask */
    Uint32  bmAlphaPitch;
};

    /** @} end of bmp_struct */

    /**
     * \defgroup bmp_load_fns Bitmap file load/save operations
     * @{
     *
     * Example:
     *
     * \include bitmap.c
     */

/**
 * \var typedef void (* CB_ALLOC_BITMAP_BUFF) (void* context, BITMAP* bmp)
 * \brief The type of callback to allocate pixel buffer for BITMAP object.
 *
 * Return TRUE for success, otherwise FALSE.
 */
typedef BOOL (* CB_ALLOC_BITMAP_BUFF) (void* context, BITMAP* bmp);

/**
 * \var typedef void (* CB_ONE_SCANLINE) (void* context, MYBITMAP* my_bmp, int y)
 * \brief The type of scanline loaded callback.
 */
typedef void (* CB_ONE_SCANLINE) (void* context, MYBITMAP* my_bmp, int y);

/**
 * \fn BOOL GUIAPI RegisterBitmapFileType (const char *ext, \
            void* (*init) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal), \
            int (*load) (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp, \
                    CB_ONE_SCANLINE cb, void* context), \
            void (*cleanup) (void* init_info), \
            int (*save) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal), \
            BOOL (*check) (MG_RWops* fp))
 * \brief Registers a bitmap file loader, saver, and checker.
 *
 * This function registers a new bitmap file loader, saver, and checker.
 * You should pass the extension of the bitmap files, the functions to
 * init, load, and cleanup this type of bitmap file, the function to save,
 * and the function to check the type.
 *
 * \param ext The extension name of the type of bitmap file, like "jpg"
 *        or "gif".
 * \param init The routine to init the MYBITMAP object (bmp).
 *        This routine fills the MYBITMAP structure and get the pallete
 *        if needed. It will return the init_info for the following load
 *        routine.
 * \param load The routine to load the scanlines of the bitmap file.
 *        This routine will load the initialized MYBITMAP object (bmp) from the
 *        data source (fp). It will call the scanline loaded callback (cb) by
 *        passing through the context (context), the MYBITMAP object (bmp),
 *        and the index of the scanline.
 * \param cleanup The cleanup routine.
 * \param save The saver of the bitmap file, can be NULL.
 * \param check The checker of the bitmap file.
 * \return TRUE on success, FALSE on error.
 *
 * \sa CheckBitmapType, InitMyBitmapSL, LoadMyBitmapSL, CleanupMyBitmapSL,
 *     general_rw_fns
 */
MG_EXPORT BOOL GUIAPI RegisterBitmapFileType (const char *ext,
            void* (*init) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            int (*load) (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp,
            CB_ONE_SCANLINE cb, void* context),
            void (*cleanup) (void* init_info),
            int (*save) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            BOOL (*check) (MG_RWops* fp));

/**
 * \fn const char* GUIAPI CheckBitmapType (MG_RWops* fp)
 * \brief Check the type of the bitmap in a data source.
 *
 * This function checks the type of the bitmap in the data source \a fp,
 * and returns the extension of this type of bitmap file.
 *
 * \param fp The pointer to the data source.
 *
 * \return The extension of the type of bitmap file.
 *         NULL for not recongnized bitmap type.
 *
 * \sa RegisterBitmapFileType
 */

MG_EXPORT const char* GUIAPI CheckBitmapType (MG_RWops* fp);

/**
 * This function is used to set attributes of MLShadow slave screen.
 * After creating a slave screen by  \a InitSlaveScreen successfully, you
 * should call this function to set its attributes. When blend_flags is
 * MLS_INFOMASK_ALL, it will set all attributes of slave screen.
 *
 * \param dc_mls The handle to the slave screen.
 * \param mask The attributes mask, can be or'ed values of the following mask:
 *
 *   - MLS_INFOMASK_OFFSET\n
 *     Set x and y offset value of slave screen.
 *
 *   - MLS_INFOMASK_BLEND\n
 *     Set blend mode of slave screen.
 *
 *   - MLS_INFOMASK_ZORDER\n
 *     Set z_order value of slave screen.
 *
 *   - MLS_INFOMASK_ALL\n
 *     Set all attribute value of slave screen.
 *
 * \param offset_x The x offset value of slave screen relative to masterscreen.
 * \param offset_y The y offset value of slave screen relative to masterscreen.
 * \param blend_flags The blend flags, can be one of the following values:
 *
 *   - MLS_BLENDMODE_COLORKEY\n
 *     Set color_key value of slave screen.
 *
 *   - MLS_BLENDMODE_ALPHA\n
 *     Set alpha value of slave screen.
 *
 *   - MLS_BLENDMODE_NONE\n
 *     The value of alpha and color_key is invalid.
 *
 * \param color_key The colorkey value of MLShadow slave screen.
 * \param alpha The alpha value of MLShadow slave screen.
 * \param z_order The zorder of MLShadow slave screen.
 *
 * \return TRUE on success, FALSE on error.
 */

#define MLS_BLENDMODE_NONE           0x00
#define MLS_BLENDMODE_COLORKEY       0x01
#define MLS_BLENDMODE_ALPHA          0x02

#define MLS_INFOMASK_OFFSET          0x01
#define MLS_INFOMASK_BLEND           0x02
#define MLS_INFOMASK_ZORDER          0x04
#define MLS_INFOMASK_ALL             0x07
#define MLS_INFOMASK_ENABLE          0x08

MG_EXPORT BOOL GUIAPI mlsSetSlaveScreenInfo (HDC dc_mls, DWORD mask, int offset_x, int offset_y,
        DWORD blend_flags, gal_pixel color_key, int alpha, int z_order);

MG_EXPORT BOOL GUIAPI mlsGetSlaveScreenInfo (HDC dc_mls, DWORD mask, int* offset_x, int* offset_y,
                        DWORD blend_flags, gal_pixel* color_key, int* alpha, int* z_order);
/**
 * \fn BOOL GUIAPI mlsEnableSlaveScreen (HDC dc_mls, BOOL enable)
 * \brief Enable or Disable a MLShadow slave screen.
 *
 * This function is used to enable or diable a MLShadow slave screen.
 * After creating a slave screen by  \a InitSlaveScreen successfully, its
 * default status is disabled.
 *
 * \param dc_mls The handle of MLShadow slave screen.
 * \param enable Whether to enable the MLShadow slave screen.
 *
 * \return TRUE on success, FALSE on error.
 */
MG_EXPORT BOOL GUIAPI mlsEnableSlaveScreen (HDC dc_mls, BOOL enable);

#define ERR_BMP_OK              0
#define ERR_BMP_IMAGE_TYPE      -1
#define ERR_BMP_UNKNOWN_TYPE    -2
#define ERR_BMP_CANT_READ       -3
#define ERR_BMP_CANT_SAVE       -4
#define ERR_BMP_NOT_SUPPORTED   -5
#define ERR_BMP_MEM             -6
#define ERR_BMP_LOAD            -7
#define ERR_BMP_FILEIO          -8
#define ERR_BMP_OTHER           -9
#define ERR_BMP_ERROR_SOURCE    -10

/**
 * \fn int GUIAPI LoadBitmapEx2 (HDC hdc, PBITMAP pBitmap,
                MG_RWops* area, const char* ext,
                CB_ALLOC_BITMAP_BUFF cb_alloc_buff, void* context)
 * \brief Load a device-dependent bitmap from a general data source.
 *
 * This function loads a device-dependent bitmap from the data source \a area.
 * This function gives a chance to the caller to allocate the pixel buffer
 * of the BITMAP object in a different way.
 *
 * \param hdc The device context.
 * \param pBitmap The pointer to the BITMAP object.
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 * \param cb_alloc_buff The callback to allocate the buffer for the pixels.
 *      If it is NULL, the function will allocate the buffer by calling malloc.
 * \param context The context will be passed to the \a cb_alloc_buff.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \retval ERR_BMP_OK Loading successfully
 * \retval ERR_BMP_IMAGE_TYPE Not a valid bitmap.
 * \retval ERR_BMP_UNKNOWN_TYPE Not recongnized bitmap type.
 * \retval ERR_BMP_CANT_READ Read error.
 * \retval ERR_BMP_CANT_SAVE Save error.
 * \retval ERR_BMP_NOT_SUPPORTED Not supported bitmap type.
 * \retval ERR_BMP_MEM Memory allocation error.
 * \retval ERR_BMP_LOAD Loading error.
 * \retval ERR_BMP_FILEIO I/O failed.
 * \retval ERR_BMP_OTHER Other error.
 * \retval ERR_BMP_ERROR_SOURCE A error data source.
 *
 * \sa LoadBitmapFromFile, LoadBitmapFromMem, CB_ALLOC_BITMAP_BUFF
 */
MG_EXPORT int GUIAPI LoadBitmapEx2 (HDC hdc, PBITMAP pBitmap,
                MG_RWops* area, const char* ext,
                CB_ALLOC_BITMAP_BUFF cb_alloc_buff, void* context);

/**
 * \fn int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP pBitmap, \
                MG_RWops* area, const char* ext)
 * \brief Load a device-dependent bitmap from a general data source.
 *
 * This function loads a device-dependent bitmap from the data source \a area.
 *
 * \param hdc The device context.
 * \param pBitmap The pointer to the BITMAP object.
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 * \return 0 on success, less than 0 on error.
 *
 * \retval ERR_BMP_OK Loading successfully
 * \retval ERR_BMP_IMAGE_TYPE Not a valid bitmap.
 * \retval ERR_BMP_UNKNOWN_TYPE Not recongnized bitmap type.
 * \retval ERR_BMP_CANT_READ Read error.
 * \retval ERR_BMP_CANT_SAVE Save error.
 * \retval ERR_BMP_NOT_SUPPORTED Not supported bitmap type.
 * \retval ERR_BMP_MEM Memory allocation error.
 * \retval ERR_BMP_LOAD Loading error.
 * \retval ERR_BMP_FILEIO I/O failed.
 * \retval ERR_BMP_OTHER Other error.
 * \retval ERR_BMP_ERROR_SOURCE A error data source.
 *
 * \sa LoadBitmapFromFile, LoadBitmapFromMem
 */
static inline int LoadBitmapEx (HDC hdc, PBITMAP pBitmap,
                MG_RWops* area, const char* ext) {
    return LoadBitmapEx2 (hdc, pBitmap, area, ext, NULL, NULL);
}

/**
 * \fn int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap, \
                const char* spFileName)
 * \brief Load a device-dependent bitmap from a file.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap,
                const char* spFileName);

/**
 * \def LoadBitmap
 * \brief An alias of \a LoadBitmapFromFile.
 *
 * \sa LoadBitmapFromFile
 */
#define LoadBitmap  LoadBitmapFromFile

/**
 * \fn int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap, \
                const void* mem, size_t size, const char* ext)
 * \brief Load a device-dependent bitmap from memory.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap,
                const void* mem, size_t size, const char* ext);

/**
 * \fn void GUIAPI UnloadBitmap (PBITMAP pBitmap)
 * \brief Unloads a bitmap.
 *
 * This function unloads the specified bitmap \a pBitmap.
 * It will free the private pixel format and the bits of the bitmap.
 *
 * \param pBitmap The BITMAP object.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT void GUIAPI UnloadBitmap (PBITMAP pBitmap);

/**
 * \fn int GUIAPI SetBitmapKeyColor (HDC hdc, PBITMAP bmp,
 *              Uint8 r, Uint8 g, Uint8 b)
 * \brief Set the bitmap color key.
 *
 * This function set the bitmap member bmColorKey with the param r,g,b and
 * set the BMP_TYPE_COLORKEY flag.
 *
 * \param hdc The device context.
 * \param bmp Point of the bitmap which to set color key.
 * \param r The red componets of RGB color.
 * \param g The green componets of RGB color.
 * \param b The blue componets of RGB color.
 *
 * \return ERR_BMP_OK for success, ERR_BMP_UNKNOWN_TYPE for error.
 *
 * /sa SetMemDCColorKey
 */
MG_EXPORT int GUIAPI SetBitmapKeyColor (HDC hdc, PBITMAP bmp, Uint8 r, Uint8 g, Uint8 b);

/**
 * \fn void GUIAPI ReplaceBitmapColor (HDC hdc, PBITMAP pBitmap, \
                gal_pixel iOColor, gal_pixel iNColor)
 * \brief Replaces a specific pixels in a bitmap with another pixel.
 *
 * This function replaces the specific pixels with value \a iOColor with
 * the other pixel value \a iNcolor in the bitmap \a pBitmap.
 *
 * \param hdc The device context.
 * \param pBitmap The BITMAP object.
 * \param iOColor The pixel value of the color will be replaced.
 * \param iNColor The pixel value of the new color.
 *
 */
MG_EXPORT void GUIAPI ReplaceBitmapColor (HDC hdc, PBITMAP pBitmap,
                gal_pixel iOColor, gal_pixel iNColor);

/**
 * \fn void void GUIAPI HFlipBitmap (BITMAP* bmp, unsigned char* inter_buff)
 *
 * \brief Horizontal Flip the special Bitmap object.
 *
 * \param bmp The BITMAP object.
 * \param inter_buff The one scan line pre-alloc buffer of bitmap.
 *
 */
MG_EXPORT void GUIAPI HFlipBitmap (BITMAP* bmp, unsigned char* inter_buff);

/**
 * \fn void GUIAPI VFlipBitmap (BITMAP* bmp, unsigned char* inter_buff)
 *
 * \brief Vertical Flip the special Bitmap object.
 *
 * \param bmp The BITMAP object.
 * \param inter_buff The one scan line  pre-alloc buffer of bitmap.
 *
 */
MG_EXPORT void GUIAPI VFlipBitmap (BITMAP* bmp, unsigned char* inter_buff);

/**
 * \fn void* GUIAPI InitMyBitmapSL (MG_RWops* area, \
                const char* ext, MYBITMAP* my_bmp, RGB* pal)
 * \brief Initializes scanline loader of the MYBITMAP object from a data source.
 *
 * This function initializes scanline loader of the MYBITMAP object from
 * a data source.
 *
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param pal The palette will be returned.
 *
 * \return The initialized information which should be passed to
 *         LoadMyBitmapSL function, and NULL on error.
 *
 * \sa LoadMyBitmapEx, LoadMyBitmapSL, CleanupMyBitmapSL
 */
MG_EXPORT void* GUIAPI InitMyBitmapSL (MG_RWops* area,
                const char* ext, MYBITMAP* my_bmp, RGB* pal);

/**
 * \fn int GUIAPI LoadMyBitmapSL (MG_RWops* area, void* load_info, \
                MYBITMAP* my_bmp, CB_ONE_SCANLINE cb, void* context)
 * \brief Load MYBITMAP scanlines from a data source one by one.
 *
 * This function loads MYBITMAP scanlines from the data source \a area
 * one by one.
 *
 * \param area The data source.
 * \param load_info The initialized information retured by
 *        InitMyBitmapSL function.
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param cb The callback to inform one scanline loaded. It can be NULL.
 * \param context The context information passed to the callback.
 * \return 0 on success, less than 0 on error.
 *
 * \sa LoadMyBitmapEx, InitMyBitmapSL, CleanupMyBitmapSL
 */
MG_EXPORT int GUIAPI LoadMyBitmapSL (MG_RWops* area, void* load_info,
                MYBITMAP* my_bmp, CB_ONE_SCANLINE cb, void* context);

/**
 * \fn int GUIAPI CleanupMyBitmapSL (MYBITMAP* my_bmp, void* load_info);
 * \brief Cleanups the scanline loader.
 *
 * This function cleanups the scanline loader.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param load_info The initialized information retured by InitMyBitmapSL
 *        function.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa LoadMyBitmapEx, InitMyBitmapSL, LoadMyBitmapSL
 */
MG_EXPORT int GUIAPI CleanupMyBitmapSL (MYBITMAP* my_bmp, void* load_info);

/**
 * \fn int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, \
                MG_RWops* area, const char* ext)
 * \brief Load a MYBITMAP object from a data source.
 *
 * This function loads a MYBITMAP object from the data source \a area.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param area The data source.
 * \param pal The palette will be returned.
 * \param ext The extension of the type of this bitmap.
 * \return 0 on success, less than 0 on error.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal,
                MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal, \
                const char* file_name)
 * \brief Load a MYBITMAP object from a file.
 *
 * \sa LoadMyBitmapEx
 */
MG_EXPORT int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal,
                const char* file_name);

/**
 * \def LoadMyBitmap
 * \brief Alias of \a LoadMyBitmapFromFile.
 *
 * \sa LoadMyBitmapFromFile
 */
#define LoadMyBitmap    LoadMyBitmapFromFile

/**
 * \fn int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal, \
                const void* mem, size_t size, const char* ext)
 * \brief Load a MYBITMAP object from memory.
 *
 * This function loads a MYBITMAP object from memory.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param pal The palette will be retruned through this pointer.
 * \param mem The pointer to the memory area.
 * \param size The size of the memory area.
 * \param ext The extension name used to determine the type of the bitmap.
 *
 * \sa LoadMyBitmapEx, MYBITMAP
 */
MG_EXPORT int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal,
                const void* mem, size_t size, const char* ext);

/**
 * \fn void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp)
 * \brief Unloads a bitmap.
 *
 * This function unloads the specified MYBITMAP object \a my_bmp.
 * It will free the bits of the bitmap.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 *
 * \sa LoadMyBitmapEx
 */
MG_EXPORT void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp);

#ifdef _MGMISC_SAVEBITMAP

/**
 * \fn int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal, \
                const char* spFileName)
 * \brief Saves a MYBITMAP object to a bitmap file.
 *
 * This function saves the MYBITMAP object \a my_bmp to the bitmap file
 * named \a spFileName.
 *
 * \param my_bmp The MYBITMAP object.
 * \param pal The palette.
 * \param spFileName The file name.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa SaveBitmapToFile
 */
MG_EXPORT int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal,
                const char* spFileName);

/**
 * \fn int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP pBitmap, \
                const char* spFileName)
 * \brief Saves a BITMAP object to a bitmap file.
 *
 * This function saves the BITMAP object \a pBitmap to the bitmap file
 * named \a spFileName.
 *
 * \param hdc The device context.
 * \param pBitmap The BITMAP object.
 * \param spFileName The file name.
 * \return 0 on success, less than 0 on error.
 *
 * \sa SaveMyBitmapToFile
 */
MG_EXPORT int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP pBitmap,
                const char* spFileName);
#define SaveBitmap  SaveBitmapToFile
#endif

/**
 * \fn BOOL GUIAPI InitBitmap (HDC hdc, Uint32 w, Uint32 h, Uint32 pitch, \
                BYTE* bits, PBITMAP bmp)
 * \brief Initializes a BITMAP object as a normal bitmap.
 *
 * This function initializes the bitmap pointed to by \a bmp as a normal bitmap.
 * It sets the bitmap structure fields, and allocates the bits if \a bits
 * is NULL.
 *
 * \param hdc The device context.
 * \param w The width of the bitmap.
 * \param h The height of the bitmap.
 * \param pitch The pitch of the bitmap.
 * \param bits The bits of the bitmap.
 * \param bmp The BITMAP object to be initialized.
 * \return TRUE on success, FALSE on error.
 *
 * \note \a LoadBitmapEx will initialize the BITMAP object itself.
 *
 * \sa InitBitmapPixelFormat, UnloadBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI InitBitmap (HDC hdc, Uint32 w, Uint32 h, Uint32 pitch,
                BYTE* bits, PBITMAP bmp);

/**
 * \fn HDC GUIAPI InitSlaveScreenEx (const char* name, const char* mode, int dpi)
 * \brief Initializes slave screen.
 *
 * \param name The gal engine name.
 * \param mode The display mode. For example : 640x480-16bpp.
 * \param dpi  The resolution of screen, should be a value larger than GDCAP_DPI_MINIMAL (36).
 *
 * \return Valid handle on success, HDC_INVALID on failure.
 *
 */
MG_EXPORT HDC GUIAPI InitSlaveScreenEx (const char* name, const char* mode, int dpi);

/**
 * \fn HDC GUIAPI InitSlaveScreen (const char* name, const char* mode)
 * \brief Initializes slave screen.
 *
 * \param name The gal engine name.
 * \param mode The display mode. For example : 640x480-16bpp.
 *
 * \return Valid handle on success, HDC_INVALID on failure.
 */
static inline HDC InitSlaveScreen (const char* name, const char* mode)
{
    return InitSlaveScreenEx(name, mode, GDCAP_DPI_DEFAULT);
}

/**
 * \fn void TerminateSlaveScreen (HDC hdc)
 * \brief Terminates slave screen.
 *
 * \param hdc The slave device context.
 *
 */
MG_EXPORT void GUIAPI TerminateSlaveScreen (HDC hdc);
/**
 * \fn BOOL GUIAPI InitBitmapPixelFormat (HDC hdc, PBITMAP bmp)
 * \brief Initializes the bitmap pixel format information of a BITMAP object.
 *
 * This function initializes the bitmap pixel format information of the
 * BITMAP object pointed to by \a bmp. This includes \a bmBitsPerPixel
 * and \a bmBytesPerPixel fields, and the private pixel format if the
 * bitmap is a bitmap with alpha.
 *
 * \param hdc The device context.
 * \param bmp The BITMAP object to be initialized.
 * \return TRUE on success, FALSE on error.
 *
 * \sa InitBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI InitBitmapPixelFormat (HDC hdc, PBITMAP bmp);

/**
 * \fn void GUIAPI DeleteBitmapAlphaPixel (PBITMAP bmp)
 * \brief Deletes the bitmap alpha pixel format information of a BITMAP object.
 *
 * This function deletes the bitmap alpha pixel format information of
 * the BITMAP object \a bmp.
 *
 * \sa InitBitmapPixelFormat
 */
MG_EXPORT void GUIAPI DeleteBitmapAlphaPixel (PBITMAP bmp);

/* Everything in the pixel format structure is read-only */
typedef struct GAL_PixelFormat {
    GAL_Palette *palette;

    Uint8  BitsPerPixel;
    Uint8  BytesPerPixel;
    /* The flag indicating dithered palette */
    Uint8  DitheredPalette;
    /* The flag indicating the Most Significant Bits (MSB)
     * is left when depth is less than 8. */
    Uint8  MSBLeft;

    Uint8  Rloss;
    Uint8  Gloss;
    Uint8  Bloss;
    Uint8  Aloss;
    Uint8  Rshift;
    Uint8  Gshift;
    Uint8  Bshift;
    Uint8  Ashift;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    /* RGB color key information */
    gal_pixel colorkey;
    /* Alpha value information (per-surface alpha) */
    gal_uint8 alpha;
} GAL_PixelFormat;

#define _FILL_MYBITMAP

#ifndef MYBITMAP_CONTXT
typedef struct _MYBITMAP_CONTXT{
    Uint32 colorKey;
    Uint32 pitch;
    GAL_PixelFormat* AlphaPixelFormat;
    MYBITMAP*  mybmp;
}MYBITMAP_CONTXT;
#endif

typedef BYTE* (* CB_DRAW_PIXEL) (HDC hdc, MYBITMAP_CONTXT* mybmp, Uint32 pixel, BYTE* dest);

/**
 * \fn int GUIAPI ExpandMyBitmap (HDC hdc, PBITMAP bmp, \
                const MYBITMAP* my_bmp, const RGB* pal, int frame)
 * \brief Expands a MYBITMAP object to a BITMAP object.
 *
 * This function expands the MYBITMAP object pointed to by \a my_bmp
 * to a BITMAP object (\a bmp).
 *
 * \param hdc The device context.
 * \param bmp The expanded BITMAP object.
 * \param my_bmp The MYBITMAP object to be expanded.
 * \param pal The palette of the MYBITMAP object.
 * \param frame The frame of the MYBITMAP object.
 * \return 0 on success, non-zero on error.
 */
MG_EXPORT int GUIAPI ExpandMyBitmap (HDC hdc, PBITMAP bmp,
                const MYBITMAP* my_bmp, const RGB* pal, int frame);

/**
 * \fn void GUIAPI ExpandMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
 *               const BYTE* my_bits, Uint32 my_pitch,
 *               Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg)
 *
 * \brief Convert a mono color MYBITMAP pixel data to a BITMAP pixel data.
 *
 * This function compile a MYBITMAP pixel data to a BITMAP pixel data.
 *
 * \param hdc The device context.
 * \param bits The BITMAP pixel data(RGBA etc.).
 * \param pitch The pitch of the BITMAP object.
 * \param my_bits The MYBITMAP pixel data to be compile.
 * \param my_pitch The pitch of the MYBITMAP object.
 * \param w The width of the MYBITMAP object.
 * \param h The height of the MYBITMAP object.
 * \param flags The flags of the MYBITMAP object.
 * \param bg The foreground color of the BITMAP object.
 * \param fg The foreground color of the BITMAP object.
 *
 * \sa Expand16CBitmap, Expand256CBitmap
 */
MG_EXPORT void GUIAPI ExpandMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg);

MG_EXPORT void GUIAPI Expand16CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags,
                const RGB* pal, BYTE use_pal_alpha, BYTE alpha);

/**
 * \fn void GUIAPI Expand16CBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
 *               const BYTE* my_bits, Uint32 my_pitch,
 *               Uint32 w, Uint32 h, DWORD flags, const RGB* pal)
 *
 * \brief Convert a 16 color MYBITMAP pixel data to a BITMAP pixel data.
 *
 * This function compile a 16 color  MYBITMAP pixel data to a BITMAP pixel data.
 *
 * \param hdc The device context.
 * \param bits The BITMAP pixel data(RGBA etc.).
 * \param pitch The pitch of the BITMAP object.
 * \param my_bits The MYBITMAP pixel data to be compile.
 * \param my_pitch The pitch of the MYBITMAP object.
 * \param w The width of the MYBITMAP object.
 * \param h The height of the MYBITMAP object.
 * \param flags The flags of the MYBITMAP object.
 * \param pal The Palette of the MYBITMAP object.
 *
 * \sa ExpandMonoBitmap, Expand256CBitmap
 */
static inline void GUIAPI Expand16CBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, const RGB* pal)
{
    Expand16CBitmapEx (hdc, bits, pitch, my_bits, my_pitch,
                w, h, flags, pal, FALSE, 0xFF);
}

MG_EXPORT void GUIAPI Expand256CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags,
                const RGB* pal, BYTE use_pal_alpha, BYTE alpha,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);
/**
 * \fn void GUIAPI Expand256CBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
 *               const BYTE* my_bits, Uint32 my_pitch,
 *               Uint32 w, Uint32 h, DWORD flags, const RGB* pal,
 *               CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);
 *
 * \brief Convert a 256 color MYBITMAP pixel data to a BITMAP pixel data.
 *
 * This function compile a 256 color MYBITMAP pixel data to a BITMAP pixel data.
 *
 * \param hdc The device context.
 * \param bits The BITMAP pixel data(RGBA etc.).
 * \param pitch The pitch of the BITMAP object.
 * \param my_bits The MYBITMAP pixel data to be compile.
 * \param my_pitch The pitch of the MYBITMAP object.
 * \param w The width of the MYBITMAP object.
 * \param h The height of the MYBITMAP object.
 * \param flags The flags of the MYBITMAP object.
 * \param pal The Palette of the MYBITMAP object.
 * \param cb_draw The draw callback function.
 * \param mybmp The context of cb_draw.
 *
 * \sa ExpandMonoBitmap, Expand16CBitmap
 */
static inline void GUIAPI Expand256CBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, const RGB* pal,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    Expand256CBitmapEx (hdc, bits, pitch, my_bits, my_pitch,
                w, h, flags, pal, FALSE, 0xFF, cb_draw, mybmp);
}

/**
 * \fn void GUIAPI CompileRGBABitmap (HDC hdc, BYTE* bits, Uint32 pitch, \
 *              const BYTE* my_bits, Uint32 my_pitch, \
 *              Uint32 w, Uint32 h, DWORD flags, void* pixel_format, \
 *              CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);
 * \brief Convert a MYBITMAP pixel data to a BITMAP pixel data.
 *
 * This function Convert a MYBITMAP pixel data to a BITMAP pixel data.
 *
 * \param hdc The device context.
 * \param bits The BITMAP pixel data(RGBA etc.).
 * \param pitch The pitch of the BITMAP object.
 * \param my_bits The MYBITMAP pixel data to be compile.
 * \param my_pitch The pitch of the MYBITMAP object.
 * \param w The width of the MYBITMAP object.
 * \param h The height of the MYBITMAP object.
 * \param flags The flags of the MYBITMAP object.
 * \param pixel_format The pixel format of the MYBITMAP object.
 * \param cb_draw The draw callback function.
 * \param mybmp The context of cb_draw.
 *
 * \sa CompileRGBABitmapEx
 */
MG_EXPORT void GUIAPI CompileRGBABitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, void* pixel_format,
                CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);

/**
 * \fn void GUIAPI CompileRGBABitmapEx (HDC hdc, BYTE* bits, Uint32 pitch, \
 *              const BYTE* my_bits, Uint32 my_pitch, \
 *              Uint32 w, Uint32 h, DWORD flags, void* pixel_format, \
 *              CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp,
 *              BYTE* alpha_mask);
 * \brief Compile a MYBITMAP pixel data to a BITMAP pixel data.
 *
 * This function compile a MYBITMAP pixel data to a BITMAP pixel data.
 *
 * \sa CompileRGBABitmap
 */
MG_EXPORT void GUIAPI CompileRGBABitmapEx (HDC hdc, BYTE* bits, Uint32 pitch,
                        const BYTE* my_bits, Uint32 my_pitch,
                        Uint32 w, Uint32 h, DWORD flags, void* pixel_format,
                        CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp,
                        BYTE* alpha_mask);

#ifdef _FILL_MYBITMAP
MG_EXPORT int GUIAPI FillBoxWithMyBitmap (HDC hdc, int x, int y, MYBITMAP* mybmp, RGB* pal);

MG_EXPORT void GUIAPI ExpandPartMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg,
                int stepx, CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);

MG_EXPORT void GUIAPI ExpandPart16CBitmapEx (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags,
                const RGB* pal, BYTE use_pal_alpha, BYTE alpha,
                int stepx, CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp);

static inline void GUIAPI ExpandPart16CBitmap (HDC hdc, BYTE* bits, Uint32 pitch,
                const BYTE* my_bits, Uint32 my_pitch,
                Uint32 w, Uint32 h, DWORD flags, const RGB* pal,
                int stepx, CB_DRAW_PIXEL cb_draw, MYBITMAP_CONTXT* mybmp)
{
    ExpandPart16CBitmapEx (hdc, bits, pitch, my_bits, my_pitch,
                w, h, flags, pal, FALSE, 0xFF, stepx, cb_draw, mybmp);
}

#endif

#define CompileRGBBitmap CompileRGBABitmap

/**
 * \fn int GUIAPI PaintImageEx (HDC hdc, int x, int y, \
                MG_RWops* area, const char* ext);
 * \brief Paints an image from data source on device directly.
 *
 * This function paints an image from data source onto device directly.
 *
 * \param hdc The device context.
 * \param x (x,y), the paint position on device.
 * \param y (x,y), the paint position on device.
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \retval ERR_BMP_OK Painted successfully
 * \retval ERR_BMP_IMAGE_TYPE Not a valid image type.
 * \retval ERR_BMP_UNKNOWN_TYPE Not recongnized bitmap type.
 * \retval ERR_BMP_CANT_READ Read error.
 * \retval ERR_BMP_NOT_SUPPORTED Not supported bitmap type.
 * \retval ERR_BMP_MEM Memory allocation error.
 * \retval ERR_BMP_LOAD Loading error.
 * \retval ERR_BMP_FILEIO I/O failed.
 * \retval ERR_BMP_OTHER Other error.
 * \retval ERR_BMP_ERROR_SOURCE A error data source.
 *
 * \sa PaintImageFromFile, PaintImageFromMem
 */
MG_EXPORT int GUIAPI PaintImageEx (HDC hdc, int x, int y,
                MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI PaintImageFromFile (HDC hdc, int x, int y, \
                const char* spFileName);
 * \brief Paints an image from file on device directly.
 *
 * \param hdc The device context.
 * \param x (x,y), the paint position on device.
 * \param y (x,y), the paint position on device.
 * \param spFileName The file name of the image file.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa PaintImageEx
 */
MG_EXPORT int GUIAPI PaintImageFromFile (HDC hdc, int x, int y,
                const char* spFileName);

/**
 * \fn int GUIAPI PaintImageFromMem (HDC hdc, int x, int y, \
                const void* mem, size_t size, const char* ext);
 * \brief Paints an image from memory on device directly.
 *
 * \param hdc The device context.
 * \param x (x,y), the paint position on device.
 * \param y (x,y), the paint position on device.
 * \param mem The pointer to memory containing image data.
 * \param size The size of the image data.
 * \param ext The name of the image which indicates the type of the image.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa PaintImageEx
 */
MG_EXPORT int GUIAPI PaintImageFromMem (HDC hdc, int x, int y,
                const void* mem, size_t size, const char* ext);

/**
 * \fn int GUIAPI StretchPaintImageEx (HDC hdc, int x, int y, int w, int h, \
                MG_RWops* area, const char* ext);
 * \brief Paints an image from data source on device directly.
 *
 * This function paints an image from data source onto device directly with
 * stretch.
 *
 * \param hdc The device context.
 * \param x (x,y), the paint position on device.
 * \param y (x,y), the paint position on device.
 * \param w the width of the stretched bitmap.
 * \param h the height of the stretched bitmap.
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \retval ERR_BMP_OK Painted successfully
 * \retval ERR_BMP_IMAGE_TYPE Not a valid image type.
 * \retval ERR_BMP_UNKNOWN_TYPE Not recongnized bitmap type.
 * \retval ERR_BMP_CANT_READ Read error.
 * \retval ERR_BMP_NOT_SUPPORTED Not supported bitmap type.
 * \retval ERR_BMP_MEM Memory allocation error.
 * \retval ERR_BMP_LOAD Loading error.
 * \retval ERR_BMP_FILEIO I/O failed.
 * \retval ERR_BMP_OTHER Other error.
 * \retval ERR_BMP_ERROR_SOURCE A error data source.
 *
 * \sa PaintImageFromFile, PaintImageFromMem
 */
MG_EXPORT int GUIAPI StretchPaintImageEx (HDC hdc, int x, int y, int w, int h,
                MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI StretchPaintImageFromFile (HDC hdc, int x, int y, \
 *              int w, int h, const char* spFileName);
 * \brief Paints an image from file on device directly.
 *
 * \param hdc The device context.
 * \param x (x,y), the paint position on device.
 * \param y (x,y), the paint position on device.
 * \param w The width of the stretched bitmap.
 * \param h The height of the stretched bitmap.
 * \param spFileName the file name of the image file.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa StretchPaintImageEx
 */
MG_EXPORT int GUIAPI StretchPaintImageFromFile (HDC hdc, int x, int y,
                int w, int h, const char* spFileName);

/**
 * \fn int GUIAPI StretchPaintImageFromMem (HDC hdc, int x, int y, \
 *               int w, int h, const void* mem, size_t size, const char* ext)
 * \brief Paints an image from memory on device directly.
 *
 * \param hdc The device context.
 * \param x (x,y) the paint position on device.
 * \param y (x,y) the paint position on device.
 * \param w The width of the stretched bitmap.
 * \param h The height of the stretched bitmap.
 * \param mem The pointer to memory containing image data.
 * \param size The size of the image data.
 * \param ext The name of the image which indicates the type of the image.
 *
 * \return 0 on success, less than 0 on error.
 *
 * \sa StretchPaintImageEx
 */
MG_EXPORT int GUIAPI StretchPaintImageFromMem (HDC hdc, int x, int y,
                int w, int h, const void* mem, size_t size, const char* ext);

#ifdef _MGHAVE_FIXED_MATH

/**
 * \fn void GUIAPI RotateBitmap (HDC hdc, const BITMAP *bmp, \
 *               int lx, int ty, int angle)
 *
 * \brief Rotate a bitmap object.
 *
 * \param hdc The device context.
 * \param bmp The pointer of Bitmap object.
 * \param lx (lx,ty), the x coordinate of top left corner.
 * \param ty (lx,ty), the y coordinate of top left corner.
 * \param angle The specified rotated angle around its center.
 *              It must be in 1/64ths of a degree.
 *
 * \sa PivotScaledBitmapFlip
 */
MG_EXPORT void GUIAPI RotateBitmap (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle);

/**
 * \fn void GUIAPI  RotateBitmapVFlip (HDC hdc, const BITMAP *bmp, \
                int lx, int ty, int angle)
 *
 * \brief Flips vertically and rotates a bitmap object.
 *
 * This function flips vertically before rotating the bitmap pointed to \a bmp.
 *
 * \sa RotateBitmap
 */
MG_EXPORT void GUIAPI RotateBitmapVFlip (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle);

/**
 * \fn void GUIAPI  RotateBitmapHFlip (HDC hdc, const BITMAP *bmp, int lx, \
               int ty, int angle)
 *
 * \brief Flips horizontally and rotates a bitmap object.
 *
 * \sa RotateBitmap
 */
MG_EXPORT void GUIAPI RotateBitmapHFlip (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle);

/**
 * \fn void GUIAPI  RotateScaledBitmap (HDC hdc, const BITMAP *bmp, int lx, \
 *                  int ty, int angle, int w, int h)
 *
 * \brief Stretches or shrinks a bitmap object at the same as rotating it.
 *
 * \sa RotateBitmap
 */
MG_EXPORT void GUIAPI RotateScaledBitmap (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle, int w, int h);

/**
 * \fn void GUIAPI  RotateScaledBitmapVFlip (HDC hdc, const BITMAP *bmp, \
                int lx, int ty, int angle, int w, int h)

 * \brief Flip vertically, rotates, stretch or shrinks a bitmap object.
 *
 * This function is similar to RotateScaledBitmap() expect that it flips the
 *        bitmap vertically first.
 *
 * \sa RotateScaledBitmap()
 */
MG_EXPORT void GUIAPI RotateScaledBitmapVFlip (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle, int w, int h);

/**
 * \fn void GUIAPI  RotateScaledBitmapHFlip (HDC hdc, const BITMAP *bmp, \
                int lx, int ty, int angle, int w, int h)
 *
 * \brief Flip horizontaly, rotates, stretch or shrinks a bitmap object.
 *
 * This function is similar to RotateScaledBitmap() expect that it flips the
 *        bitmap horizontally first.
 *
 * \sa RotateScaledBitmap()
 */
MG_EXPORT void GUIAPI RotateScaledBitmapHFlip (HDC hdc, const BITMAP *bmp,
                int lx, int ty, int angle, int w, int h);

/**
 * \fn void GUIAPI PivotBitmap(HDC hdc, const BITMAP *bmp, \
            int x, int y, int cx, int cy, int angle)
 *
 * \brief Pivot a bitmap object.
 *
 * This function aligns the point in the bitmap given by (cx, cy) to (x, y)
 * in device context, then rotates around this point.
 *
 * \sa PivotScaledBitmapFlip
 */
MG_EXPORT void GUIAPI PivotBitmap(HDC hdc, const BITMAP *bmp,
                int lx, int ty, int cx, int cy, int angle);

/**
 * \fn void GUIAPI PivotScaledBitmapFlip (HDC hdc, const BITMAP *bmp, \
 *               fixed x, fixed y, fixed cx, fixed cy, int angle, fixed scale_x, \
 *               fixed scale_y, BOOL h_flip, BOOL v_flip)
 * \brief Rotates, stretches or shrinks, flips a bitmap object.
 *
 *  This function flips the bitmap vertically if \a v_flip is TRUE, flips the
 *  bitmap horizontally if \a h_flip is TRUE first. Then stretches or shrinks
 *  the bitmap according to \a scale and  aligns the point in the bitmap given
 *  by (cx, cy) to (x, y) in device context, Finally rotates specified angle
 *  pointed to \a angle in 1/64ths of a degree around this point (cx, cy).
 *
 * \param hdc The device context.
 * \param bmp The pointer of BITMAP object.
 * \param x (x,y) The x coordinate of a point in fixed point on dc.
 * \param y (x,y) The y coordinate of a point in fixed point on dc.
 * \param cx (cx,cy) The x coordinate of a point in fixed point on the bitmap.
 * \param cy (cx,cy) The y coordinate of a point in fixed point on the bitmap.
 * \param angle The specified rotated angle around its center.
 * \param scale_x The ratio of width of stretching or shrinking the bitmap
 *                 in fixed point.
 * \param scale_y The ratio of height of stretching or shrinking the bitmap
 *                 in fixed point.
 * \param h_flip The flags of fliping horizontally.
 * \param v_flip The flags of fliping vertically.
 */
MG_EXPORT void GUIAPI PivotScaledBitmapFlip (HDC hdc, const BITMAP *bmp,
                fixed x, fixed y, fixed cx, fixed cy, int angle, fixed scale_x,
                fixed scale_y, BOOL h_flip, BOOL v_flip);

#endif /*_MGHAVE_FIXED_MATH*/

    /** @} end of bmp_load_fns */

    /** @} end of gdi_fns */

    /** @} end of fns */

#ifdef _MGFONT_BMPF

/**
 * \fn DEVFONT* GUIAPI CreateBMPDevFont (const char *bmpfont_name, \
                const BITMAP* glyph_bmp, const char* start_mchar, \
                int nr_glyphs, int glyph_width)
 * \brief Create a bitmap device font.
 *
 * This function create a bitmap device font and return the pointer to
 * the new device font.
 *
 * \param bmpfont_name The font name.
 * \param glyph_bmp The pointer to the bitmap object.
 * \param start_mchar The pointer to the first char of segment.
 * \param nr_glyphs The character count of the segment.
 * \param glyph_width The character width.
 *
 * \return The pointer to the new device font on sucess, otherwise NULL.
 */
MG_EXPORT DEVFONT* GUIAPI CreateBMPDevFont (const char *bmpfont_name,
                const BITMAP* glyph_bmp, const char* start_mchar, int nr_glyphs,
                int glyph_width);

/**
 * \fn BOOL GUIAPI AddGlyphsToBMPFont (DEVFONT* dev_font, BITMAP* glyph_bmp, \
                const char* start_mchar, int nr_glyphs, int glyph_width);
 * \brief Add a new segment in device font.
 *
 * This function add a new segment to the device font pointed by \a dev_font.
 *
 * \param dev_font The pointer to the device font.
 * \param glyph_bmp The pointer to the segment glyph bitmap object.
 * \param start_mchar The pointer to the first char of the segment.
 * \param nr_glyphs The character count of the segment.
 * \param glyph_width The character width in the segment.
 *
 * \return TRUE if success, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI AddGlyphsToBMPFont (DEVFONT* dev_font, BITMAP* glyph_bmp,
                const char* start_mchar, int nr_glyphs, int glyph_width);

/**
 * \fn void GUIAPI DestroyBMPFont (DEVFONT* dev_font)
 * \brief Destroy the bitmap device font.
 *
 * This function destroy the bitmap device font pointed by \a dev_font.
 *
 * \param dev_font The pointer to the device font which will be destroyed.
 *
 * \return TRUE if success, otherwise FALSE.
 */
MG_EXPORT void GUIAPI DestroyBMPFont (DEVFONT* dev_font);

#endif /* end of _MGFONT_BMPF */

     /**
     * \addtogroup gdi_fns GDI functions
     *
     * @{
     */

     /**
      * \defgroup complex_scripts Rendering Text in Complex/Mixed Scripts
      *
      * To lay out, shape, and render a text in mixed scripts, you should call
      * \a GetUCharsUntilParagraphBoundary function first to convert
      * a multi-byte string to a Unicode string under the specified white space
      * rule, breaking rule, and transformation rule. For example, converting a
      * general C string in UTF-8 or GB18030 to a Uchar32 string by calling this
      * function. You can call \a CreateLogFontForMChar2UChar function to create
      * a dummy logfont object for this purpose in order to expense a minimal memory.
      *
      * If the text is in simple scripts, like Latin or Chinese, you can call
      * \a GetGlyphsExtentFromUChars function to lay out the paragraph. This function
      * returns a glyph string which can fit in a line with the specified
      * maximal extent and rendering flags. After this, you call
      * \a DrawGlyphStringEx function to draw the glyph string to the
      * specific position of a DC.
      *
      * If the text is in complex and/or mixed scripts, like Arabic, Thai,
      * and Indic, you should create a TEXTRUNS object first by calling
      * \a CreateTextRuns function, then initialize the shaping engine for
      * laying out the text.
      *
      * MiniGUI provides two types of shaping engine. One is the basic
      * shaping engine. The corresponding function is \a InitBasicShapingEngine.
      * The other is called complex shaping engine, which is based on HarfBuzz.
      * The corresponding function is \a InitComplexShapingEngine. The latter
      * one can give you a better shaping result.
      *
      * After this, you should call \a CreateLayout to create a layout object
      * for laying out the text, then call \a LayoutNextLine to lay out the lines
      * one by one.
      *
      * You can render the laid out lines by calling \a DrawLayoutLine function.
      *
      * Finally, you call \a DestroyLayout and \a DestroyTextRuns to destroy
      * the layout object and text runs object.
      *
      * Before rendering the glyphs laid out, you can also call \a GetLayoutLineRect
      * to get the line rectangle, or call \a CalcLayoutBoundingRect to get
      * the bounding rectangle of one paragraph.
      *
      * These new APIs provide a very flexible implementation for your apps
      * to process the complex scripts.
      *
      * Note that MiniGUI uses Achar32 type for an abstract character index value
      * under a certain charset/encoding, and uses Glyph32 type
      * for the glyph index value in a device font.
      *
      * Under Unicode charset or encodings, the abstract character index value
      * will be identical to the Unicode code point, i.e., Achar32
      * is equivalent to Uchar32 under this situation.
      *
      * @{
      */

typedef Uint32  Achar32;
typedef Uint32  Glyph32;

/**
 * \def INV_ACHAR_VALUE
 */
#define INV_ACHAR_VALUE             0xFFFFFFFF

/**
 * \def INV_GLYPH_VALUE
 */
#define INV_GLYPH_VALUE             0xFFFFFFFF

/**
 * \fn Achar32 GUIAPI GetACharValue (LOGFONT* logfont, const char* mchar, \
 *         int mchar_len, const char* pre_mchar, int pre_len)
 * \brief Get the character value of a multi-byte character.
 *
 * \param logfont The logical font.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of \a mchar in bytes.
 * \param pre_mchar The pointer to the multi-byte character before \a mchar.
 * \param pre_len The length of \a per_mchar in bytes.
 *
 * \return The abstract character value of the multi-byte character.
 */
MG_EXPORT Achar32 GUIAPI GetACharValue (LOGFONT* logfont, const char* mchar,
        int mchar_len, const char* pre_mchar, int pre_len);

#ifdef _MGCHARSET_UNICODE

/**
 * \fn int GUIAPI GetNextUChar (LOGFONT* logfont, const char* mchar,
 *         int mchar_len, Uchar32* uc)
 * \brief Get the Uchar32 value (Unicode code point) of a multi-byte character
 *      in specified LOGFONT object.
 *
 * This function get the Uchar32 value of a mutil-byte character in specified
 * LOGFONT object \a logfont, which is under speicific charset/encoding.
 *
 * \param logfont The logical font.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of \a mchar in bytes.
 * \param uc The buffer to receive the Uchar32 value.
 *
 * \return The number of bytes consumed, i.e., the lenght of the
 *      multi-byte character.
 */
MG_EXPORT int GUIAPI GetNextUChar(LOGFONT* logfont, const char* mchar,
        int mchar_len, Uchar32* uc);

#endif /* _MGCHARSET_UNICODE */

/**
 * \fn Uint32 GUIAPI GetACharType (LOGFONT* logfont, Achar32 chv)
 * \brief Retrieve the basic type, the general cateory of Unicode, and
 *      the break class of Unicode of an abstract character.
 *
 * This function retrieves the basic type, the general category defined by
 * Unicode terms, and the break class defined by Unicode of an abstract
 * character.
 *
 * \param logfont The logical font.
 * \param chv An Achar32 value.
 *
 * \return The type of the specified Achar32 value. You can use the following
 *      macros to extract the basic type, the break type, and the bidi type
 *      respectively:
 *      - ACHARTYPE_EXTRACT_BASIC(type)\n
 *      - ACHARTYPE_EXTRACT_CATEGORY(type)\n
 *      - ACHARTYPE_EXTRACT_BREAK(type)\n
 *
 * \note The basic type is defined by MiniGUI for basic rendering for some
 *      complex shaping glyphs.
 *
 * \note The general category and the break class are only available when
 *      the support for Unicode is enabled.
 *
 * \sa GetACharBidiType, achar_types
 */
MG_EXPORT Uint32 GUIAPI GetACharType (LOGFONT* logfont, Achar32 chv);

/**
 * \var typedef enum ACHARSHAPETYPE
 * \brief Achar32 shape type.
 */
typedef enum {
    ACHAR_ISOLATED,
    ACHAR_FINAL,
    ACHAR_INITIAL,
    ACHAR_MEDIAL
} ACHARSHAPETYPE;

/**
 * \fn Achar32 GUIAPI GetShapedAChar (LOGFONT* logfont, const char* mchar, \
 *         int mchar_len, ACHARSHAPETYPE shape_type)
 * \brief Get the glyph shape of a character.
 *
 * \param logfont The logical font.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of \a mchar in bytes.
 * \param shape_type The requested shape type.
 *
 * \return The shaped character value.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT Achar32 GUIAPI GetShapedAChar (LOGFONT* logfont, const char* mchar,
        int mchar_len, ACHARSHAPETYPE shape_type);

/**
 * \fn BOOL GUIAPI GetMirrorAChar (LOGFONT* logfont, Achar32 chv,
 *      Achar32* mirrored)
 * \brief Get the mirrored abstract character if possible.
 *
 * \param logfont The logical font.
 * \param chv The abstract character value.
 * \param mirrored The buffer to store the mirrored Achar32 value if
 *      the multi-byte character has a mirrored character.
 *
 * \return TRUE if success, FALSE on failure.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT BOOL GUIAPI GetMirrorAChar (LOGFONT* logfont, Achar32 chv,
        Achar32* mirrored);

/**
 * \fn BidiType GUIAPI GetACharBidiType (LOGFONT* logfont, Achar32 chv)
 * \brief Retrieve the BIDI type of an abstract character.
 *
 * This function retrieves the BIDI type of an abstract character.
 *
 * \param logfont The logical font.
 * \param chv The abstract character value.
 *
 * \return The BIDI type of \a chv; BIDI_TYPE_INVALID on failure.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops, GetACharType, bidi_types
 */
MG_EXPORT BidiType GUIAPI GetACharBidiType (LOGFONT* log_font, Achar32 chv);

/**
 * \var typedef struct  _ACHARMAPINFO ACHARMAPINFO
 * \brief Data type of struct _ACHARMAPINFO.
 */
typedef struct _ACHARMAPINFO {
    /** The index of the character in the text string. */
    int byte_index;
    /** The length of the character in bytes. */
    int char_len;
    /** The direction of the character; TRUE for RTL, FALSE for LTR. */
    BOOL is_rtol;
} ACHARMAPINFO;

/**
 * \fn int GUIAPI BIDIGetTextLogicalAChars(LOGFONT* log_font,
 *      const char* text, int text_len, Achar32** achars,
 *      ACHARMAPINFO** achars_map)
 * \brief Get logical achars string of the text.
 *
 * \param log_font The logical font.
 * \param text The logical text string.
 * \param text_len The lenght of the logical text string in bytes.
 * \param achars The pointer to the logical glyph string.
 * \param achars_map The position map from the logical achars string to
 *        the logical text.
 *
 * \return The length of the logical glyph string.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops, ACHARMAPINFO
 */
MG_EXPORT int GUIAPI BIDIGetTextLogicalAChars (LOGFONT* log_font,
        const char* text, int text_len, Achar32** achars,
        ACHARMAPINFO** achars_map);

/**
 * \fn void GUIAPI BIDIGetTextRangesLog2Vis (LOGFONT* log_font,
 *      const char* text, int text_len, int start_index, int end_index,
 *      int** ranges, int* nr_ranges)
 * \brief Get a list of visual ranges corresponding to a given logical range.
 *
 * \param log_font The logical font.
 * \param text The pointer to the logical text string.
 * \param text_len The length of the logical text string in bytes.
 * \param start_index The start index of the logical range.
 * \param end_index The end index of the logical range.
 * \param ranges The pointer to store a pointer to an array of arranges.
 * \param nr_ranges The number of ranges stored in \a ranges.
 *
 * \return None.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT void GUIAPI BIDIGetTextRangesLog2Vis (LOGFONT* log_font,
        const char* text, int text_len, int start_index, int end_index,
        int** ranges, int* nr_ranges);

/** \fn int GUIAPI BIDIGetTextVisualAChars (LOGFONT* log_font,
 *      const char* text, int text_len, Achar32** achars,
 *      ACHARMAPINFO** achars_map)
 * \brief Get visual achars and glyph_map info relative with logical
 *        string byte index.
 *
 * \param log_font The logical font.
 * \param text The logical text string.
 * \param text_len The length of the logical text string in bytes.
 * \param achars The pointer to the visual glyph string.
 * \param achars_map The position map from visual achars string to
 *        the logical text.
 *
 * \return The length of the visual glyph string.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT int GUIAPI BIDIGetTextVisualAChars (LOGFONT* log_font,
        const char* text, int text_len, Achar32** achars,
        ACHARMAPINFO** achars_map);

/** \fn BOOL GUIAPI BIDILogAChars2VisACharsEx (LOGFONT* log_font,
 *      Achar32* achars, int nr_achars, int pel,
 *      void* extra, CB_REVERSE_ARRAY cb_reverse_extra)
 * \brief Reorder the specified logical glyph string in visual order and
 * reorder an extra array to reflect the visule order of the achars.
 *
 * This function reorders the logical glyph string in place to visual order.
 * If \a extra and \a cb_reverse_extra are both not NULL, it also reorders
 * the array pointed by \a extra by calling the callback function
 * \a cb_reverse_extra.
 *
 * \param log_font The logical font.
 * \param achars The pointer to the glyph string.
 * \param nr_achars The length of the glyph string.
 * \param pel The paragraph embedding level, can be one of the following values:
 *          - 0: Level 0 (left to right)
 *          - 1: Level 1 (right to left)
 *          - others: Determine according to the heuristic given in
 *            steps P2 and P3 of the Unicode bidirectional algorithm.
 * \param extra The pointer to the extra array to reorder; can be NULL.
 * \param cb_reverse_extra The callback function to reverse the extra array.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT BOOL GUIAPI BIDILogAChars2VisACharsEx (LOGFONT* log_font,
        Achar32* achars, int nr_achars, int pel,
        void* extra, CB_REVERSE_ARRAY cb_reverse_extra);

/** \fn BOOL GUIAPI BIDILogAChars2VisAChars (LOGFONT* log_font,
 *      Achar32* achars, int nr_achars, ACHARMAPINFO* achars_map)
 * \brief Reorder the specified logical glyph string in visual order and
 * reorder glyph map if specified.
 *
 * This function reorders the logical glyph string in place to visual order.
 * If \a achars_map is not NULL, it also reorders
 * the map to reflect the visual order.
 *
 * \param log_font The logical font.
 * \param achars The pointer to the glyph string.
 * \param nr_achars The length of the glyph string.
 * \param achars_map The position map returned by \a BIDIGetTextLogicalAChars;
 *          can be NULL.
 *
 * \return The pointer to the visual achars; NULL when error.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT Achar32* GUIAPI BIDILogAChars2VisAChars (LOGFONT* log_font,
        Achar32* achars, int nr_achars, ACHARMAPINFO* achars_map);

/**
 * \fn void GUIAPI BIDIGetLogicalEmbedLevelsEx (LOGFONT* log_font, \
        const Achar32* achars, int nr_achars, int pel, Uint8** embedding_levels)
 * \brief Get the logical embedding levels for the logical glyph string
 *        and generate runs by embedding levels, the for reorder to get
 *        visual glyph string.
 *
 * \param log_font The logical font.
 * \param achars The pointer to the logical glyph string.
 * \param nr_achars The length of the glyph string.
 * \param pel The paragraph embedding level, can be one of the following values:
 *          - 0: Level 0 (left to right)
 *          - 1: Level 1 (right to left)
 *          - others: Determine according to the heuristic given in
 *            steps P2 and P3 of the Unicode bidirectional algorithm.
 * \param embedding_levels The logical embedding level.
 *
 * \return None.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (for Arabic and Hebrew languages respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */
MG_EXPORT void GUIAPI BIDIGetLogicalEmbedLevelsEx (LOGFONT* log_font,
        Achar32* achars, int nr_achars, int pel, Uint8** embedding_levels);

static inline void BIDIGetLogicalEmbeddLevels (LOGFONT* log_font,
        Achar32* achars, int nr_achars, Uint8** embedding_levels)
{
    BIDIGetLogicalEmbedLevelsEx (log_font, achars, nr_achars, -1,
            embedding_levels);
}

/**
 * \fn void GUIAPI BIDIGetVisualEmbedLevelsEx (LOGFONT* log_font,
        const Achar32* achars, int nr_achars, int pel, Uint8** embedding_levels)
 * \brief Get the visual embedding levels for the given logical glyph
 *        string, then you can get the edge for visual achars.
 *
 * \param log_font The logical font.
 * \param achars The pointer to the logical glyph string.
 * \param nr_achars The length of the glyph string.
 * \param pel The paragraph embedding level, can be one of the following values:
 *          - 0\n
 *            Level 0 (left to right)
 *          - 1\n
 *            Level 1 (right to left)
 *          - others\n
 *            Determine according to the heuristic given in
 *            steps P2 and P3 of the Unicode bidirectional algorithm.
 * \param embedding_levels The embedding level logical to visual.
 *
 * \return none.
 *
 * \note This is an API of the legacy implementation of bidirectional algorithm.
 *      It is used to support text in ISO8859-6 and ISO8859-8 charsets
 *      (Arabic and Hebrew respectively).
 *      New apps should enable the support for Unicode charset, and
 *      use the new implementation of Unicode Bidirectional Algorithm (UBA).
 *
 * \sa UBidiReorderLine, GetShapedGlyphsBasic, GetShapedGlyphsComplex,
 *      unicode_ops
 */

MG_EXPORT void GUIAPI BIDIGetVisualEmbedLevelsEx (LOGFONT* log_font,
        Achar32* achars, int nr_achars, int pel, Uint8** embedding_levels);

static inline void BIDIGetVisualEmbeddLevels (LOGFONT* log_font,
        Achar32* achars, int nr_achars, Uint8** embedding_levels)
{
    BIDIGetVisualEmbedLevelsEx (log_font, achars, nr_achars, -1,
            embedding_levels);
}

/*
 * \fn int GUIAPI DrawACharString (HDC hdc, int x, int y,
 *      Achar32* achars, int nr_achars, int* adv_x, int* adv_y);
 *
 * \brief Draw an abstract character string.
 *
 * This function draws an abstract character string to the specific
 * postion of a DC. Note that this function will ignore all breaks
 * in the Achar32 string.
 *
 * \param hdc The device context.
 * \param x The output start x position.
 * \param y The output start y position.
 * \param achars The pointer to the glyph string.
 * \param nr_achars The number of achars which will be draw.
 * \param adv_x The pointer used to return the advance in x-coordinate of
 *        the glyph string, can be NULL.
 * \param adv_y The pointer used to return the advance in y-coordinate of
 *        the glyph string, can be NULL.
 *
 * \return The advance on baseline.
 */
MG_EXPORT int GUIAPI DrawACharString (HDC hdc, int x, int y, Achar32* achars,
        int nr_achars, int* adv_x, int* adv_y);

/**
 * \fn int GUIAPI GetACharsExtent(HDC hdc, Achar32* achars, int nr_achars, \
 *         SIZE* size)
 * \brief Get visual extent value of an achar string.
 *
 * This function gets the extent value of an achar string on a DC. Note that
 * this function will ignore all breaks in the achar string.
 *
 * \param hdc The device context.
 * \param achars The pointer to the achar string.
 * \param nr_achars The length of the achar string.
 * \param size The buffer restoring the extents of the achar strings.
 *
 * \return The extent of the achar string.
 */
MG_EXPORT int GUIAPI GetACharsExtent (HDC hdc, Achar32* achars, int nr_achars,
        SIZE* size);

/**
 * \fn int GUIAPI GetACharsExtentPoint (HDC hdc, Achar32* achars, \
 *         int nr_achars, int max_extent, SIZE* size)
 * \brief Get the visual extent value of an achar string.
 *
 * This function gets the visual extent value of a glpyh string.
 * Note that this function ignore all breaks in the achar string.
 *
 * \param hdc The device context.
 * \param achars The pointer to the achar string.
 * \param nr_achars The length of the achar string len.
 * \param max_extent The maximal output extent value.
 * \param size The real extent of all visual achars in the achar string.
 *
 * \return The the index of the last achar which can be fit to the extent.
 *
 * \sa GetACharsExtentPointEx
 */
MG_EXPORT int GUIAPI GetACharsExtentPoint (HDC hdc, Achar32* achars,
        int nr_achars, int max_extent, SIZE* size);

/**
 * \fn Glyph32 GUIAPI GetGlyphValue (LOGFONT* logfont, const char* mchar, \
 *         int mchar_len, const char* pre_mchar, int pre_len)
 * \brief Get the glyph value of a multi-byte character.
 *
 * \param logfont The logical font.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of \a mchar in bytes.
 * \param pre_mchar The pointer to the multi-byte character before \a mchar.
 * \param pre_len The length of \a per_mchar in bytes.
 *
 * \return The glyph value of the multi-byte character.
 */
MG_EXPORT Glyph32 GUIAPI GetGlyphValue (LOGFONT* logfont, const char* mchar,
        int mchar_len, const char* pre_mchar, int pre_len);

/**
 * \fn Glyph32 GUIAPI GetGlyphValueAlt (LOGFONT* logfont, Achar32 chv)
 * \brief Get the LOGFONT glyph value of an abstract character.
 *
 * \param logfont The logical font.
 * \param chv The value of the abstract character.
 *
 * \return The glyph value of the abstract character;
 *      INV_GLYPH_VALUE on failure.
 */
MG_EXPORT Glyph32 GUIAPI GetGlyphValueAlt(LOGFONT* logfont, Achar32 chv);

/**
 * \fn int GUIAPI DrawGlyph (HDC hdc, int x, int y, Glyph32 glyph_value, \
 *         int* adv_x, int* adv_y)
 * \brief Draw a glyph.
 *
 * This function draws a glyph to the specific postion of a DC.
 *
 * \param hdc The device context.
 * \param x The output start x position.
 * \param y The output start y position.
 * \param glyph_value The glyph value.
 * \param adv_x The pointer used to return the advance in x-coordinate of
 *        the glyph, can be NULL.
 * \param adv_y The pointer used to return the advance in y-coordinate of
 *        the glyph, can be NULL.
 *
 * \return The advance on baseline.
 */
MG_EXPORT int GUIAPI DrawGlyph (HDC hdc, int x, int y, Glyph32 glyph_value,
        int* adv_x, int* adv_y);

/*
 * \fn int GUIAPI DrawGlyphString (HDC hdc, Glyph32* glyphs, int nr_glyphs,
 *      const POINT* pts);
 *
 * \brief Draw a glyph string at specified positions.
 *
 * This function draws a glyph string to the specific postions of a DC.
 * Note that this function will ignore all special type (such as zero-width)
 * in the glyph string.
 *
 * \param hdc The device context.
 * \param glyphs The pointer to the glyph string.
 * \param nr_glyphs The number of glyphs which will be draw.
 * \param pts The positions of every glyphs.
 *
 * \return The number of glyphs drawn.
 *
 * \sa DrawGlyphStringEx
 */
MG_EXPORT int GUIAPI DrawGlyphString (HDC hdc, Glyph32* glyphs, int nr_glyphs,
        const POINT* pts);

/**
 * \fn int GUIAPI GetGlyphsExtent(HDC hdc, Glyph32* glyphs, int nr_glyphs,
 *         SIZE* size)
 * \brief Get visual extent value of a glyph string.
 *
 * This function gets the extent value of a glyph string on a DC. Note that
 * this function will ignore all special types in the glyph string.
 *
 * \param hdc The device context.
 * \param glyphs The pointer to the glyph string.
 * \param nr_glyphs The length of the glyph string.
 * \param size The buffer restoring the extents of the glyph string.
 *
 * \return The extent of the glyph string.
 *
 * \sa GetGlyphsExtentFromUChars
 */
MG_EXPORT int GUIAPI GetGlyphsExtent (HDC hdc, Glyph32* glyphs, int nr_glyphs,
        SIZE* size);

/**
 * \fn int GUIAPI GetGlyphsExtentPoint (HDC hdc, Glyph32* glyphs,
 *         int nr_glyphs, int max_extent, SIZE* size)
 * \brief Get the visual extent value of an glyph string.
 *
 * This function gets the visual extent value of a glpyh string.
 * Note that this function ignore all special types in the glyph string.
 *
 * \param hdc The device context.
 * \param glyphs The pointer to the glyph string.
 * \param nr_glyphs The length of the glyph string len.
 * \param max_extent The maximal output extent value.
 * \param size The real extent of all visual glyphs in the glyph string.
 *
 * \return The the index of the last glyph which can be fit to the extent.
 *
 * \sa GetGlyphsExtentFromUChars
 */
MG_EXPORT int GUIAPI GetGlyphsExtentPoint (HDC hdc, Glyph32* glyphs,
        int nr_glyphs, int max_extent, SIZE* size);

#define GLYPH_INFO_METRICS      0x01
#define GLYPH_INFO_BMP          0x02
#define GLYPH_INFO_FACE         0x04

/** The type of glyph bitmap: monochrome */
#define GLYPHBMP_TYPE_MONO      0x00
/** The type of glyph bitmap: grey (8-bit) */
#define GLYPHBMP_TYPE_GREY      0x01
/** The type of glyph bitmap: grey (4-bit) */
#define GLYPHBMP_TYPE_GREY4b    0x02
/** The type of glyph bitmap: subpixel */
#define GLYPHBMP_TYPE_SUBPIXEL  0x03
/** The type of glyph bitmap: pre-rendered BITMAP object */
#define GLYPHBMP_TYPE_PRERENDER 0x04

/**
 * \var typedef struct  _GLYPHINFO GLYPHINFO
 * \brief Data type of struct _GLYPHINFO.
 */
typedef struct _GLYPHINFO {
    /**
     * The mask indicates if you want to get glyph metrics,
     * or bitmap infomation. Or'ed with the following values:
     * - GLYPH_INFO_METRICS
     * - GLYPH_INFO_BMP
     * - GLYPH_INFO_FACE
     */
    Uint32 mask;

    /** The bounding box of the glyph. */
    int bbox_x, bbox_y, bbox_w, bbox_h;
    /** The advance measure of the glyph. */
    int advance_x, advance_y;

    /**
     * The type of glyph bitmap, one of the following values:
     * - GLYPHBMP_TYPE_MONO
     * - GLYPHBMP_TYPE_PRERENDER
     * - GLYPHBMP_TYPE_SUBPIXEL
     * - GLYPHBMP_TYPE_PRERENDER
     */
    int bmp_type;

    /** The width of the glyph bitmap. */
    int bmp_width;
    /** The height of the glyph bitmap. */
    int bmp_height;
    /* The pitch of the glyph bitmap. */
    int bmp_pitch;

    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;

    /**
     * The prerender bitmap object.
     * It is only valid if bmp_type is GLYPHBMP_TYPE_PRERENDER
     */
    BITMAP prbitmap;

    /**
     * The FreeType face, font file path, and the glyph index
     * (when mask has GLYPH_INFO_FACE set).
     * You can use these information to render the glyph by yourself.
     * Only available if the device font is a TrueType/OpenType font.
     */
    void* ft_face;
    const char* file_path;
    Uint32 index;
} GLYPHINFO;

/**
 * \fn int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value, \
 *         GLYPHINFO* glyph_info)
 * \brief Retrieve the information of a glyph.
 *
 * This function retrieves the information of a glyph.
 *
 * \param logfont The logical font.
 * \param glyph_value The glyph value.
 * \param glyph_info The glyph information will be returned through this struct.
 *
 * \note You must set glyph_info->mask to indicate what you want.
 *
 * \return 0 if success, otherwise on failure.
 *
 * \sa GLYPHINFO
 */
MG_EXPORT int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value,
        GLYPHINFO* glyph_info);

#ifdef _MGCHARSET_UNICODE

    /**
     * \defgroup white_space_rules White Space Rules
     *
     *  The white space rule indicates \a GetUCharsUntilParagraphBoundary.
     *
     *      - whether and how white space inside the string is collapsed.
     *      - whether lines may wrap at unforced soft wrap opportunities.
     * @{
     */

/**
 * \def WSR_NORMAL
 *
 * \brief This value directs \a GetUCharsUntilParagraphBoundary
 * collapses sequences of white space into a single character.
 * Lines may wrap at allowed soft wrap opportunities.
 */
#define WSR_NORMAL          0x00

/**
 * \def WSR_PRE
 *
 * \brief This value prevents \a GetUCharsUntilParagraphBoundary from collapsing
 * sequences of white space. Segment breaks such as line feeds are
 * preserved as forced line breaks. Lines only break at forced line breaks;
 * content that does not fit within the specified extent overflows it.
 */
#define WSR_PRE             0x01

/**
 * \def WSR_NOWRAP
 *
 * \brief Like \a WSR_NORMAL, this value collapses white spaces; but like
 * \a WSR_PRE, it does not allow wrapping.
 */
#define WSR_NOWRAP          0x02

/**
 * \def WSR_PRE_WRAP
 *
 * \brief Like \a WSR_PRE, this value preserves white space; but like
 * \a WSR_NORMAL, it allows wrapping.
 */
#define WSR_PRE_WRAP        0x03

/**
 * \def WSR_BREAK_SPACES
 *
 * \brief
 * The behavior is identical to that of \a WSR_PRE_WRAP, except that:
 *  - Any sequence of preserved white space always takes up space,
 *      including at the end of the line.
 *  - A line breaking opportunity exists after every preserved
 *      white space glyph, including between white space characters.
 *
 * When white space rule is specified to be WSR_BREAK_SPACES, the manner
 * of \a GetUCharsUntilParagraphBoundary will conform
 * to CSS Text Module Level 3.
 */
#define WSR_BREAK_SPACES    0x04

/**
 * \def WSR_PRE_LINE
 *
 * \brief Like WS_NORMAL, this value collapses consecutive spaces and
 * allows wrapping, but preserves segment breaks in the source
 * as forced line breaks.
 */
#define WSR_PRE_LINE        0x05

    /** @} end of white_space_rules */

/**
 * \fn int GUIAPI GetUCharsUntilParagraphBoundary(LOGFONT* logfont,
 *          const char* mstr, int mstr_len, Uint8 wsr,
 *          Uchar32** uchars, int* nr_uchars);
 * \brief Convert a multi-byte character string to a Unicode character
 *      (Uchar32) string until the end of text (null character) or an
 *      explicit paragraph boundary encountered.
 *
 * This function calculates and allocates the Uchar32 string from a multi-byte
 * string until it encounters the end of text (null character) or an explicit
 * paragraph boundary. It also processes the text according to the specified
 * white space rule \a wsr and the text transformation rule \a ctr.
 *
 * The implementation of this function conforms to the CSS Text Module Level 3:
 *
 * https://www.w3.org/TR/css-text-3/
 *
 * Note that you are responsible for freeing the Uchar32 string allocated
 * by this function.
 *
 * \param logfont The logfont used to parse the string.
 * \param mstr The pointer to the multi-byte string.
 * \param mstr_len The length of \a mstr in bytes.
 * \param wsr The white space rule; see \a white_space_rules.
 * \param uchars The pointer to a buffer to store the address of the
 *      Uchar32 array which contains the Unicode character values.
 * \param nr_uchars The buffer to store the number of the allocated
 *      Uchar32 array.
 *
 * \return The number of the bytes consumed in \a mstr; zero on error.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa DrawGlyphStringEx, white_space_rules, char_transform_rules
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI GetUCharsUntilParagraphBoundary(LOGFONT* logfont,
        const char* mstr, int mstr_len, Uint8 wsr,
        Uchar32** uchars, int* nr_uchars);

/**
 * \fn AChar2UChar(LOGFONT* logfont, Achar32 chv)
 * \brief Get Uchar32 value (Unicode wide character value) from
 * a LOGFONT abstract character value.
 *
 * Only valid for UNICODE.
 *
 * \param logfont The LOGFONT object
 * \param chv The LOGFONT character value.
 *
 * \return The Uchar32 value (Unicode code point) of the abstract character.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * Since 4.0.0
 */
MG_EXPORT Uchar32 GUIAPI AChar2UChar(LOGFONT* logfont, Achar32 chv);

/**
 * \fn int AChars2UChars(LOGFONT* logfont, const Achar32* achs,
 *      Uchar32* ucs, int n)
 * \brief Convert an Achar32 array to Unicode character array.
 *
 * Only valid for UNICODE.
 *
 * \param logfont The LOGFONT object
 * \param achs The array of the abstract character values.
 * \param ucs The buffer to store the converted Uchar32 characters.
 * \param n The length of the Achar32 array.
 *
 * \return The number of characters converted successfully.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI AChars2UChars(LOGFONT* logfont, const Achar32* chs,
        Uchar32* ucs, int n);

/**
 * \def UCHAR2ACHAR(uc)
 * \brief The macro to convert a Uchar32 value to Achar32 value.
 *
 * \note The converted Achar32 value should be used only with LOGFONT objects
 *      in Unicode charset (encodings like UTF-8, UTF-16LE, and UTF-16BE).
 *
 * \sa UChar2AChar, UChars2AChars
 *
 * Since 4.0.0
 */
#define UCHAR2ACHAR(uc) ((uc) | 0x80000000)

/**
 * \fn BOOL UChar2AChar(LOGFONT* logfont, Uchar32 uc, Achar32* ac)
 * \brief Get the LOGFONT Achar32 value from a Unicode character value.
 *
 * This function converts a Unicode character value \a uc to the abstract
 * character value which is defined by the charset of the LOGFONT object
 * \a logfont.
 *
 * \param logfont The LOGFONT object
 * \param uc The Unicode character value.
 * \param ac The buffer to store the converted Achar32 value.
 *
 * \return TRUE for success, otherwise FALSE. When the return value
 *      is FALSE, it means that the charset of the LOGFONT object
 *      does not contain a code point for the Unicode character.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa UChars2AChars, UCHAR2ACHAR
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI UChar2AChar(LOGFONT* logfont, Uchar32 uc, Achar32* ac);

/**
 * \fn int UChars2AChars(LOGFONT* logfont, const Uchar32* ucs,
 *      Achar32* acs, int n)
 * \brief Convert an Uchar32 array to Unicode character array.
 *
 * Only valid for UNICODE.
 *
 * \param logfont The LOGFONT object
 * \param ucs The array of the Unicode characters.
 * \param acs The buffer to store the converted abstract characters.
 * \param n The length of the Uchar32 array.
 *
 * \return The number of characters converted successfully.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa UChar2AChar, UCHAR2ACHAR
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI UChars2AChars(LOGFONT* logfont, const Uchar32* ucs,
        Achar32* acs, int n);

    /**
     * \defgroup glyph_render_flags Glyph Rendering Flags
     *
     * The glyph rendering flags indicate \a GetGlyphsExtentFromUChars and
     * \a CreateLayout how to lay out the text:
     *      - The writing mode (horizontal or vertical) and the glyph orientation;
     *      - The indentation mode (none, first line or hanging);
     *      - Whether and how to break if the line overflows the max extent;
     *      - Whether and how to ellipsize if the line overflows the max extent;
     *      - The alignment of lines;
     *      - Whether and how to adjust the glyph position for alignment of justify;
     *      - The hanging punctation method;
     *      - Remove or hange the spaces at the start and/or end of the line.
     * @{
     */

#define GRF_WRITING_MODE_MASK               0xF0000000
#define GRF_WRITING_MODE_VERTICAL_FLAG      0x20000000
/**
 * Top-to-bottom horizontal direction.
 * Both the writing mode and the typographic mode are horizontal.
 */
#define GRF_WRITING_MODE_HORIZONTAL_TB      0x00000000
/**
 * Bottom-to-top horizontal direction.
 * Both the writing mode and the typographic mode are horizontal,
 * but lines are generated from bottom to top.
 */
#define GRF_WRITING_MODE_HORIZONTAL_BT      0x10000000
/**
 * Right-to-left vertical direction.
 * Both the writing mode and the typographic mode are vertical,
 * but the lines are generated from right to left.
 */
#define GRF_WRITING_MODE_VERTICAL_RL        0x20000000
/**
 * Left-to-right vertical direction.
 * Both the writing mode and the typographic mode are vertical.
 * but the lines are generated from left to right.
 */
#define GRF_WRITING_MODE_VERTICAL_LR        0x30000000

#define GRF_TEXT_ORIENTATION_MASK           0x0F000000
/**
 * The glyphs are individually typeset upright in
 * vertical lines with vertical font metrics.
 */
#define GRF_TEXT_ORIENTATION_UPRIGHT        0x00000000
/**
 * The glyphs typeset a run rotated 90° clockwise
 * from their upright orientation.
 */
#define GRF_TEXT_ORIENTATION_SIDEWAYS       0x01000000
/**
 * The glyphs are individually typeset upside down in
 * vertical lines with vertical font metrics.
 */
#define GRF_TEXT_ORIENTATION_UPSIDE_DOWN    0x02000000
/**
 * The glyphs typeset a run rotated 90° counter-clockwise
 * from their upright orientation.
 */
#define GRF_TEXT_ORIENTATION_SIDEWAYS_LEFT  0x03000000
/**
 * In vertical writing modes, all typographic character units
 * keep in their intrinsic orientation.
 */
#define GRF_TEXT_ORIENTATION_AUTO           0x04000000
/**
 * In vertical writing modes, typographic character units from
 * horizontal-only scripts are typeset sideways, i.e. 90° clockwise
 * from their standard orientation in horizontal text.
 * Typographic character units from vertical scripts are
 * typeset with their intrinsic orientation.
 */
#define GRF_TEXT_ORIENTATION_MIXED          0x05000000

#define GRF_TEXT_ORIENTATION_LINE           0x06000000

#define GRF_LINE_EXTENT_MASK                0x00C00000
/**
 * The maximal line extent is fixed.
 * The maximal line extent value you passed to \a LayoutNextLine
 * will be ignored.
 */
#define GRF_LINE_EXTENT_FIXED               0x00000000
/**
 * The maximal line extent is variable. You should pass the desired
 * maximal line extent value for a new line when calling
 * \a LayoutNextLine. The intent mode will be ignored as well.
 */
#define GRF_LINE_EXTENT_VARIABLE            0x00400000

#define GRF_INDENT_MASK                     0x00300000
/**
 * No indentation.
 */
#define GRF_INDENT_NONE                     0x00000000
/**
 * The first line is indented.
 */
#define GRF_INDENT_FIRST_LINE               0x00100000
/**
 * Indent all the lines of a paragraph except the first line.
 */
#define GRF_INDENT_HANGING                  0x00200000

#define GRF_OVERFLOW_WRAP_MASK              0x000C0000
/**
 * Lines may break only at allowed break points.
 */
#define GRF_OVERFLOW_WRAP_NORMAL            0x00000000
/**
 * Lines may break only at word seperators.
 */
#define GRF_OVERFLOW_WRAP_BREAK_WORD        0x00040000
/**
 * An otherwise unbreakable sequence of characters may be broken
 * at an arbitrary point if there are no otherwise-acceptable
 * break points in the line.
 */
#define GRF_OVERFLOW_WRAP_ANYWHERE          0x00080000

#define GRF_OVERFLOW_ELLIPSIZE_MASK         0x00030000
/**
 * No ellipsization
 */
#define GRF_OVERFLOW_ELLIPSIZE_NONE         0x00000000
/**
 * Omit characters at the start of the text
 */
#define GRF_OVERFLOW_ELLIPSIZE_START        0x00010000
/**
 * Omit characters in the middle of the text
 */
#define GRF_OVERFLOW_ELLIPSIZE_MIDDLE       0x00020000
/**
 * Omit characters at the end of the text
 */
#define GRF_OVERFLOW_ELLIPSIZE_END          0x00030000

#define GRF_ALIGN_MASK                      0x0000F000
/**
 * Text content is aligned to the start edge of the line box.
 */
#define GRF_ALIGN_START                     0x00000000
/**
 * Text content is aligned to the end edge of the line box.
 */
#define GRF_ALIGN_END                       0x00001000
/**
 * Text content is aligned to the line left edge of the line box.
 * In vertical writing modes, this will be the physical top edge.
 */
#define GRF_ALIGN_LEFT                      0x00002000
/**
 * Text content is aligned to the line right edge of the line box.
 * In vertical writing modes, this will be the physical bottom edge.
 */
#define GRF_ALIGN_RIGHT                     0x00003000
/**
 * Text content is centered within the line box.
 */
#define GRF_ALIGN_CENTER                    0x00004000
/**
 * All lines will be justified according to the method specified by
 * GRF_TEXT_JUSTIFY_XXX, in order to exactly fill the line box.
 *
 * If you specify only a valid justification method (not GRF_TEXT_JUSTIFY_NONE)
 * without GRF_ALIGN_JUSTIFY, the last line will not be justified.
 */
#define GRF_ALIGN_JUSTIFY                   0x00005000

#define GRF_TEXT_JUSTIFY_MASK               0x00000F00
/**
 * Do not justify.
 */
#define GRF_TEXT_JUSTIFY_NONE               0x00000000
/**
 * Justification adjusts primarily the spacing at word separators
 * and between CJK typographic letter units along with secondarily
 * between Southeast Asian typographic letter units.
 */
#define GRF_TEXT_JUSTIFY_AUTO               0x00000100
/**
 * Justification adjusts spacing at word separators only.
 */
#define GRF_TEXT_JUSTIFY_INTER_WORD         0x00000200
/**
 * Justification adjusts spacing between each pair of adjacent
 * typographic character units.
 */
#define GRF_TEXT_JUSTIFY_INTER_CHAR         0x00000300

#define GRF_HANGING_PUNC_MASK               0x000000F0
/**
 * No character hangs.
 */
#define GRF_HANGING_PUNC_NONE               0x00000000
/**
 * A stop or comma at the end of a line hangs.
 */
#define GRF_HANGING_PUNC_FORCE_END          0x00000010
/**
 * A stop or comma at the end of a line hangs
 * if it does not otherwise fit prior to justification.
 */
#define GRF_HANGING_PUNC_ALLOW_END          0x00000020
/**
 * An opening bracket or quote at the start of the line hangs.
 */
#define GRF_HANGING_PUNC_OPEN               0x00000040
/**
 * An closing bracket or quote at the end of the line hangs.
 */
#define GRF_HANGING_PUNC_CLOSE              0x00000080

#define GRF_SPACES_MASK                     0x0000000F
/**
 * All spaces are kept.
 */
#define GRF_SPACES_KEEP                     0x00000000
/**
 * A sequence of spaces at the start of a line is removed.
 */
#define GRF_SPACES_REMOVE_START             0x00000001
/**
 * A sequence of spaces at the end of a line is removed.
 */
#define GRF_SPACES_REMOVE_END               0x00000002
/**
 * A sequence of spaces at the end of a line hangs.
 */
#define GRF_SPACES_HANGE_END                0x00000004

    /** @} end of glyph_render_flags */

typedef enum {
    GLYPH_ORIENT_UPRIGHT        = GLYPH_GRAVITY_SOUTH,
    GLYPH_ORIENT_SIDEWAYS       = GLYPH_GRAVITY_EAST,
    GLYPH_ORIENT_UPSIDE_DOWN    = GLYPH_GRAVITY_NORTH,
    GLYPH_ORIENT_SIDEWAYS_LEFT  = GLYPH_GRAVITY_WEST,
} GlyphOrient;

typedef enum {
    GLYPH_HANGED_NONE = 0,
    GLYPH_HANGED_START,
    GLYPH_HANGED_END,
} GlyphHanged;

/**
 * The glyph extent information.
 */
typedef struct _GLYPHEXTINFO {
    /** The bounding box of the glyph. */
    int bbox_x, bbox_y, bbox_w, bbox_h;
    /** The advance values of the glyph. */
    int adv_x, adv_y;
    /** The extra spacing values of the glyph. */
    int extra_x, extra_y;
    /** The advance value of the glyph along the line direction. */
    int line_adv;
    /**
     * Whether suppress the glyph.
     */
    Uint8 suppressed:1;
    /**
     * Whether is a whitespace glyph.
     */
    Uint8 whitespace:1;
    /**
     * The orientation of the glyph; can be one of the following values:
     *  - GLYPH_ORIENT_UPRIGHT\n
     *      the glyph is in the standard horizontal orientation.
     *  - GLYPH_ORIENT_SIDEWAYS\n
     *      the glyph rotates 90° clockwise from horizontal.
     *  - GLYPH_ORIENT_SIDEWAYS_LEFT\n
     *      the glyph rotates 90° counter-clockwise from horizontal.
     *  - GLYPH_ORIENT_UPSIDE_DOWN\n
     *      the glyph is in the inverted horizontal orientation.
     */
    Uint8 orientation:2;
} GLYPHEXTINFO;

/**
 * The glyph position information.
 */
typedef struct _GLYPHPOS {
    /**
     * The x coordinate of the glyph position.
     */
    int x;
    /**
     * The y coordinate of the glyph position.
     */
    int y;
    /**
     * The x offset of the glyph position.
     */
    int x_off;
    /**
     * The y offset of the glyph position.
     */
    int y_off;
    /**
     * The line advance of the glyph.
     */
    int advance;
    /**
     * Whether suppress the glyph.
     */
    Uint8 suppressed:1;
    /**
     * Whether is a whitespace glyph.
     */
    Uint8 whitespace:1;
    /**
     * Whether is an ellipsized glyph.
     */
    Uint8 ellipsis:1;
    /**
     * The orientation of the glyph; can be one of the following values:
     *  - GLYPH_ORIENT_UPRIGHT\n
     *      the glyph is in the standard horizontal orientation.
     *  - GLYPH_ORIENT_SIDEWAYS\n
     *      the glyph rotates 90° clockwise from horizontal.
     *  - GLYPH_ORIENT_SIDEWAYS_LEFT\n
     *      the glyph rotates 90° counter-clockwise from horizontal.
     *  - GLYPH_ORIENT_UPSIDE_DOWN\n
     *      the glyph is upside down.
     */
    Uint8 orientation:2;
    /**
     * Whether hanged the glyph; can be one of the following values:
     *  - GLYPH_HANGED_NONE\n
     *      the glyph is not hanged.
     *  - GLYPH_HANGED_START\n
     *      the glyph is hanged at the start of the line.
     *  - GLYPH_HANGED_END\n
     *      the glyph is hanged at the end of the line.
     */
    Uint8 hanged:2;
} GLYPHPOS;

/**
 * \fn int GUIAPI GetGlyphsExtentFromUChars(LOGFONT* logfont_upright,
 *          const Uchar32* uchars, int nr_uchars, const BreakOppo* break_oppos,
 *          Uint32 render_flags, int x, int y,
 *          int letter_spacing, int word_spacing, int tab_size, int max_extent,
 *          SIZE* line_size, Glyph32* glyphs, GLYPHEXTINFO* glyph_ext_info,
 *          GLYPHPOS* glyph_pos, LOGFONT** logfont_sideways)
 * \brief Get the visual extent info of all glyphs fitting in the specified
 *      maximal output extent.
 *
 * This function gets the visual extent information of a glyph string which can
 * fit a line with the specified maximal extent.
 *
 * \param logfont_upright The logfont used to render the uchar string. Note
 *      that the charset/encoding of this logfont should be Unicode, such as
 *      UTF-8, UTF-16LE, and UTF-16BE.
 * \param uchars The pointer to the achar string.
 * \param nr_uchars The number of the glyphs.
 * \param break_oppos The pointer to the break opportunities array of the glyphs.
 *      It should be returned by \a UStrGetBreaks. However, the caller
 *      should skip the first unit (the break opportunity before the first glyph)
 *      when passing the pointer to this function.
 * \param render_flags The render flags; see \a glyph_render_flags.
 * \param x The x-position of first glyph.
 * \param y The y-position of first glyph.
 * \param letter_spacing This parameter specifies additional spacing
 *      (commonly called tracking) between adjacent glyphs.
 * \param word_spacing This parameter specifies the additional spacing between
 *      words.
 * \param tab_size The tab size used to render preserved tab characters.
 * \param max_extent The maximal output extent value. No limit when it is < 0.
 * \param line_size The buffer to store the line extent info; can be NULL.
 * \param glyphs The buffer to store the glyphs, which can fit in
 *      the max extent; cannot be NULL.
 * \param glyph_ext_info The buffer to store the extent info of all glyphs
 *      which can fit in the max extent; can be NULL.
 * \param glyph_pos The buffer to store the positions and orientations of
 *      all glyphs which can fit in the max extent; cannot be NULL.
 * \param logfont_sideways The buffer to store the LOGFONT object created
 *      by this function for sideways glyphs if text orientation specified
 *      in \a render_flags is mixed (GRF_TEXT_ORIENTATION_MIXED) or
 *      sideways (GRF_TEXT_ORIENTATION_SIDEWAYS). If *logfont_sidways is
 *      not NULL, this function will try to use this LOGFONT object for
 *      sideways glyphs.
 *
 * \return The number of achars which can be fit to the maximal extent.
 *      The glyphs and the extent info of every glyphs which are fit in
 *      the maximal extent will be returned through \a glyphs and
 *      \a glyph_ext_info (if it was not NULL), and the
 *      line extent info will be returned through \a line_size
 *      if it was not NULL. Note the function will return immediately if
 *      it encounters a mandatory breaking.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \note This function ignore the special types (such as diacritic mark,
 *      vowel, contextual form, ligature, and so on) of the Unicode characters.
 *      Please see \a GetShapedGlyphsBasic and \a GetShapedGlyphsComplex
 *      for the purpose of shaping glyphs according to the language, the
 *      script (writing system), and the contextual shaping features.
 *
 * \note Any invisible format character including SOFT HYPHEN (U+00AD)
 *      will be ignored (suppressed).
 *
 * \note The LOGFONT object \a logfont_upright should have the rotation
 *      be 0° for upright glyphs and \a logfont_sideways will have the
 *      rotation be 90° for sideways glyphs.
 *
 * \note The position coordinates of the first glyph are
 *      with respect to the top-left corner of the output rectangle
 *      if the writing mode is GRF_WRITING_MODE_HORIZONTAL_TB or
 *      GRF_WRITING_MODE_VERTICAL_LR, otherwise they are with respect
 *      to the top-right corner of the output rectangle. However,
 *      the positions contained in \a glyph_pos are always with respect to
 *      the top-left corner of the resulting output line rectangle.
 *
 * \sa UStrGetBreaks, DrawGlyphStringEx, GLYPHEXTINFO, glyph_render_flags
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI GetGlyphsExtentFromUChars(LOGFONT* logfont_upright,
        const Achar32* uchars, int nr_uchars, const BreakOppo* break_oppos,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        SIZE* line_size, Glyph32* glyphs, GLYPHEXTINFO* glyph_ext_info,
        GLYPHPOS* glyph_pos, LOGFONT** logfont_sideways);

/*
 * \fn int GUIAPI DrawGlyphStringEx (HDC hdc,
 *      LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
 *      const Glyph32* glyphs, const GLYPHPOS* glyph_pos, int nr_glyphs)
 * \brief Draw a glyph string at the specified positions and text orientations.
 *
 * This function draws a glyph string to the specific positions and
 * orientations on a DC \a hdc with the logfonts specified by
 * \a logfont_upright and \a logfont_sideways.
 *
 * \param hdc The device context.
 * \param logfont_upright The LOGFONT object used for upright glyphs.
 * \param logfont_sideways The LOGFONT object used for sideways glyphs.
 * \param glyphs The pointer to the glyph string
 * \param glyph_pos The buffer holds the position information
 *      of every glyph.
 * \param nr_glyphs The number of the glyphs should be drawn.
 *
 * \return The number of glyphs really drawn.
 *
 * \note The positions contained in \a glyph_pos are always aligned to
 *      the top-left corner of the output rectangle.
 *
 * \sa GetGlyphsExtentFromUChars
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI DrawGlyphStringEx (HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const Glyph32* glyphs, const GLYPHPOS* glyph_pos,
        int nr_glyphs);

/**
 * The type for a text runs object.
 * The fields in this structure are invisible to users.
 */
typedef struct _TEXTRUNS TEXTRUNS;

/**
 * \fn TEXTRUNS* GUIAPI CreateTextRuns(const Uchar32* ucs, int nr_ucs,
 *      LanguageCode lang_code, ParagraphDir base_dir,
 *      const char* logfont_name, RGBCOLOR color, RGBCOLOR bg_color,
 *      BreakOppo* break_oppos)
 *
 * \brief Split a Uchar32 paragraph string in mixed scripts into text runs.
 *
 * This function splits the Uchar32 paragraph \a ucs in mixed scripts
 * into text runs, and returns a TEXTRUNS object.
 *
 * \param ucs The Uchar32 string returned by \a GetUCharsUntilParagraphBoundary.
 * \param nr_ucs The length of the Uchar32 string.
 * \param lang_code The language code.
 * \param base_dir The base direction of the paragraph.
 * \param logfont_name The default logfont name. You can change the font
 *      of some text in the paragraph by calling \a SetFontNameInTextRuns.
 * \param color The default text color. You can change the text color
 *      of some text in the paragraph by calling \a SetTextColorInTextRuns.
 * \param bg_color The default background color. You can change the background
 *      color of some text in the paragraph by calling
 *      \a SetBackgroundColorInTextRuns. If the background is transparent,
 *      please pass MakeRGBA(0,0,0,0) for this argument.
 * \param break_oppos If not NULL, the break opportunities will be tailored
 *      according to the script type of every text run. Please skip the first
 *      entry when you pass the pointer.
 *
 * \return The TEXTRUNS object create; NULL for failure.
 *
 * \note This function assumes that you passed one paragraph of
 *      the logical Unicode string. Therefore, you'd better to call this
 *      function after calling \a GetUCharsUntilParagraphBoundary.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa GetUCharsUntilParagraphBoundary, UStrGetBreaks,
 *      SetFontNameInTextRuns, SetTextColorInTextRuns
 *
 * Since 4.0.0
 */
MG_EXPORT TEXTRUNS* GUIAPI CreateTextRuns(
        const Uchar32* ucs, int nr_ucs,
        LanguageCode lang_code, ParagraphDir base_dir,
        const char* logfont_name, RGBCOLOR color, RGBCOLOR bg_color,
        BreakOppo* break_oppos);

/**
 * \brief Set logfont name of text runs.
 *
 * This function set a new LOGFONT name for all text or a part of the text
 * in a TEXTRUNS object.
 *
 * \param truns The TEXTRUNS object.
 * \param start_index The start index of the text which will use the
 *      new font name.
 * \param length The length of the text which will use the
 *      new font name.
 * \param logfont_name The new logfont name.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI SetFontNameInTextRuns(TEXTRUNS* truns,
        int start_index, int length, const char* logfont_name);

/**
 * Get the font name of a specific character in a TEXTRUNS object.
 *
 * Since 4.0.0
 */
MG_EXPORT const char* GUIAPI GetFontNameInTextRuns(
        const TEXTRUNS* truns, int index);

/**
 * \brief Set text color in a TEXTRUNS object.
 *
 * This function set a new text color for all text or a part of the text
 * in a TEXTRUNS object \a truns.
 *
 * \param truns The TEXTRUNS object.
 * \param start_index The start index of the text which will use the
 *      new font name.
 * \param length The length of the text which will use the
 *      new font name.
 * \param color The new text color.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI SetTextColorInTextRuns(TEXTRUNS* truns,
        int start_index, int length, RGBCOLOR color);

/**
 * Get the text color of a specific character in a TEXTRUNS object.
 *
 * Since 4.0.0
 */
MG_EXPORT RGBCOLOR GUIAPI GetTextColorInTextRuns(
        const TEXTRUNS* truns, int index);

/**
 * \brief Set background color in a TEXTRUNS object.
 *
 * This function set a new background color for all text or a part of
 * the text in a TEXTRUNS object \a truns.
 *
 * \param truns The TEXTRUNS object.
 * \param start_index The start index of the text which will use the
 *      new font name.
 * \param length The length of the text which will use the
 *      new font name.
 * \param color The new background color.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 * \note If you use the value returned by \a MakeRGBA(0,0,0,0),
 *      the background will be transparent.
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI SetBackgroundColorInTextRuns(TEXTRUNS* truns,
    int start_index, int length, RGBCOLOR color);

/**
 * Get the background color of a specific character in a TEXTRUNS object.
 *
 * Since 4.0.0
 */
MG_EXPORT RGBCOLOR GUIAPI GetBackgroundColorInTextRuns(
        const TEXTRUNS* truns, int index);

/**
 * \fn BOOL GUIAPI DestroyTextRuns(TEXTRUNS* truns)
 *
 * \brief Destroy the glyph run info object. It also frees all data allocated
 *      for shapping and layouting the glyphs.
 *
 * This function destroies the specific TEXTRUNS object \a truns.
 *
 * \param truns The TEXTRUNS object.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa CreateTextRuns
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI DestroyTextRuns(TEXTRUNS* truns);

/**
 * \fn BOOL GUIAPI InitBasicShapingEngine(TEXTRUNS* truns)
 * \brief Initialize the base shaping engine for a TEXTRUNS object.
 *
 * This function initializes the base shaping engine for the specific
 * TEXTRUNS object \a truns.
 *
 * The basic shaping engine performs the basic shaping process
 * according to the Unicode character properties if the script type
 * (writing system) is Arabic. This includes the mirroring/brackets
 * subsititions, and the mandatory ligatures.
 *
 * For the better shaping glyphs, you can call \a InitComplexShapingEngine
 * to perform the shaping process based on the data contained in
 * the OpenType Layout tables in the underlaying OpenType fonts or TrueType
 * fonts. MiniGUI uses LGPL'd HarfBuzz to implement the complex
 * shaping engine.
 *
 * After initializing the shapping engine, you can call \a CreateLayout
 * to layout the Uchar32 paragraph string.
 *
 * \param truns The TEXTRUNS object.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \note Only available when support for UNICODE is enabled.
 *
 * \sa InitComplexShapingEngine, CreateLayout
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI InitBasicShapingEngine(TEXTRUNS* truns);

#ifdef _MGCOMPLEX_SCRIPTS

/**
 * \fn BOOL GUIAPI InitComplexShapingEngine(TEXTRUNS* truns)
 * \brief Initialize the complex shaping engine for a TEXTRUNS object.
 *
 * This function initializes the complex shaping engine for the specific
 * TEXTRUNS object \a truns.
 *
 * The complex shaping engine performs the complex shaping process
 * according to the data contained in the OpenType Layout tables
 * (GSUB, GPOS, and so on) contained in a OpenType font. The complex
 * shaping engine is implemented based on HarfBuzz.
 *
 * You can call \a InitBasicShapingEngine to initialize the basic shaping
 * process which shapes the glyphs based on the Unicode character properties
 * instead.
 *
 * After initializing the shapping engine, you can call \a CreateLayout
 * to layout the Uchar32 paragraph string.
 *
 * \param truns The TEXTRUNS object.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \note Only available when the support for UNICODE (_MGCHARSET_UNICODE)
 *      and the support for complex scripts (_MGCOMPLEX_SCRIPTS) are enabled.
 *
 * \note You must use TrueType/OpenType vector fonts for the complex shaping
 *      engine.
 *
 * \sa InitBasicShapingEngine, CreateLayout
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI InitComplexShapingEngine(TEXTRUNS* truns);

#endif /* _MGCOMPLEX_SCRIPTS */

/**
 * The type for a layout object.
 * The fields in this structure are invisible to users.
 */
typedef struct _LAYOUT LAYOUT;

/**
 * The type for a layout line object.
 * The fields in this structure are invisible to users.
 */
typedef struct _LAYOUTLINE LAYOUTLINE;

/**
 * \fn LAYOUT* GUIAPI CreateLayout(
 *      const TEXTRUNS* truns, Uint32 render_flags,
 *      const BreakOppo* break_oppos, BOOL persist_lines,
 *      int max_line_extent, int indent,
 *      int letter_spacing, int word_spacing, int tab_size,
 *      int* tabs, int nr_tabs)
 * \brief Create layout information structure for laying out a paragraph.
 *
 * This function creates a LAYOUT object for laying out a TEXTRUNS
 * object \a truns, which represents a Uchar32 paragraph in mixed scripts
 * ready to lay out.
 *
 * \param truns The TEXTRUNS object.
 * \param render_flags The render flags; see \a glyph_render_flags.
 * \param break_oppos The breaking opportunities of the paragraph.
 * \param persist_lines Whether to persist the lines laid out.
 * \param max_line_extent The fixed maximal line extent.
 * \param indent The indentation value.
 * \param letter_spacing This parameter specifies additional spacing
 *      (commonly called tracking) between adjacent glyphs.
 * \param word_spacing This parameter specifies the additional spacing between
 *      words.
 * \param tab_size The tab size used to render preserved tab characters.
 * \param tabs The array of the tab stops; can be NULL.
 * \param nr_tabs The length of the tab stops array.
 *
 * \return The LAYOUT object; NULL for error.
 *
 * \sa CreateTextRuns, InitBasicShapingEngine, InitComplexShapingEngine,
 *      UStrGetBreaks, LayoutNextLine
 *
 * Since 4.0.0
 */
MG_EXPORT LAYOUT* GUIAPI CreateLayout(
        const TEXTRUNS* truns, Uint32 render_flags,
        const BreakOppo* break_oppos, BOOL persist_lines,
        int max_line_extent, int indent,
        int letter_spacing, int word_spacing, int tab_size,
        int* tabs, int nr_tabs);

/**
 * \fn BOOL GUIAPI DestroyLayout(LAYOUT* layout)
 * \brief Destroy the specified layout information structure.
 *
 * This function destroy the specific layout information object \a layout.
 *
 * \param layout The LAYOUT object.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \sa CreateLayout
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI DestroyLayout(LAYOUT* layout);

/*
 * \var typedef struct _RENDERDATA RENDERDATA
 * \brief The extra rendering data of the shaped glyph.
 */
typedef struct _RENDERDATA {
    /**
     * The text runs object.
     */
    const TEXTRUNS*     truns;

    /**
     * The layout object in which the glyph is contained.
     */
    const LAYOUT*       layout;

    /**
     * The layout line object in which the glyph is located.
     */
    const LAYOUTLINE*   line;

    /**
     * The logfont object should be used to render the glyph.
     */
    LOGFONT*            logfont;

    /**
     * The text alignment for rendering the glyph
     */
    Uint32              ta;

    /**
     * The Unicode character corresponding to the glyph.
     */
    Uchar32             uc;

    /**
     * The index of the Unicode character corresponding to
     * the glyph in the text runs object.
     */
    int                 uc_index;
} RENDERDATA;

/**
 * \var typedef BOOL (*CB_GLYPH_LAID_OUT) (GHANDLE ctxt,
        Glyph32 glyph_value, const GLYPHPOS* glyph_pos,
        const RENDERDATA* render_data)
 * \brief The prototype of callback function for LayoutNextLine.
 *
 * This callback function will be called by \a LayoutNextLine when
 * a glyph is laid out.
 *
 * \param ctxt The context value passed to LayoutNextLine.
 * \param glyph_value The glyph value.
 * \param glyph_pos The glyph position and orientation information.
 * \param render_data The shaping data of the glyph.
 *
 * \sa LayoutNextLine, GLYPHPOS, RENDERDATA
 *
 * Since 4.0.0
 */
typedef BOOL (*CB_GLYPH_LAID_OUT) (GHANDLE ctxt,
        Glyph32 glyph_value, const GLYPHPOS* glyph_pos,
        const RENDERDATA* render_data);

/**
 * \fn LAYOUTLINE* GUIAPI LayoutNextLine(LAYOUT* layout,
 *      LAYOUTLINE* prev_line, int max_extent, BOOL last_line,
 *      CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt)
 * \brief Layout the next line of a paragraph according to the layout
 *      information.
 *
 * This function lays out the next line of in a LAYOUTLINE object
 * \a layout. If \a prev_line is NULL, the function will returns
 * the first line.
 *
 * You can pass the maximal extent of the next line via \a max_extent to
 * control the output extent of every line. The function will wrap or
 * ellipsize the line according to the rendering flags of the LAYOUT
 * object. You can also control whether to render the next line as
 * the last line of the LAYOUT object via \a last_line parameter.
 *
 * The line size will be returned through \a line_size if it is not NULL.
 * When there is a glyph positioned, the function will call \a cb_laid_out
 * with the context \a ctxt. You can draw the glyph to a DC or do anything
 * you want.
 *
 * The previous line will be release if the LAYOUTLINE object is not
 * persisted. In this way, you can save memory use of the LAYOUTLINE object.
 *
 * \param layout The LAYOUT object.
 * \param prev_line NULL or the previous line object returned by this
 *      function.
 * \param max_extent The maximal extent of the next line; No limit if
 *      it is less than 0. This parameter is only effective when the
 *      line extent mode of the layout object is variable.
 * \param last_line Whether try to lay out all left characters in one line.
 * \param cb_laid_out The callback for one laid out glyph.
 * \param ctxt The context will be passed to \a cb_laid_out.
 *      This parameter is only effective when cb_laid_out is not NULL.
 *
 * \return NULL for no line, otherwise the next line object.
 *
 * \sa CreateLayout, DestroyLayout, CreateTextRuns
 *
 * Since 4.0.0
 */
MG_EXPORT LAYOUTLINE* GUIAPI LayoutNextLine(LAYOUT* layout,
        LAYOUTLINE* prev_line, int max_extent, BOOL last_line,
        CB_GLYPH_LAID_OUT cb_laid_out, GHANDLE ctxt);

/**
 * \fn BOOL GUIAPI GetLayoutLineSize(const LAYOUTLINE* line,
 *      SIZE* line_size)
 * \brief Get the size of a layout line.
 *
 * This function gets the size of the specific layout line object \a line.
 *
 * \param line The layout line object.
 * \param line_size The buffer to store the result.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \sa CreateLayout, DestroyLayout, LayoutNextLine
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI GetLayoutLineSize(const LAYOUTLINE* line,
        SIZE* line_size);

/**
 * \fn BOOL GUIAPI GetLayoutLineRect(LAYOUTLINE* line,
 *      int* x, int* y, int line_height, RECT* line_rc)
 * \brief Get the bouding rectangle of a layout line.
 *
 * This function gets the bouding rectangle of the specific
 * layout line object \a line.
 *
 * \param line The layout line object.
 * \param x The buffer contained the initial x-position of the line,
 *      and returns the x-position of the next line.
 * \param x The buffer contained the initial y-position of the line,
 *      and returns the y-position of the next line.
 * \param line_height The line height.
 * \param line_rc The buffer to store the result.
 *
 * \return TRUE for success, FALSE otherwise.
 *
 * \note The position coordinates of the line are
 *      with respect to the top-left corner of the output rectangle
 *      if the writing mode is GRF_WRITING_MODE_HORIZONTAL_TB or
 *      GRF_WRITING_MODE_VERTICAL_LR, the bottom-left corner if
 *      the writing mode is GRF_WRITING_MODE_HORIZONTAL_BT,
 *      the top-right corner if the writing mode is
 *      GRF_WRITING_MODE_VERTICAL_RL.
 *
 * \sa CreateLayout, DestroyLayout, LayoutNextLine,
 *  GetLayoutLineSize
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL GUIAPI GetLayoutLineRect(const LAYOUTLINE* line,
        int* x, int* y, int line_height, RECT* line_rc);

/**
 * \fn int GUIAPI CalcLayoutBoundingRect(LAYOUT* layout,
 *      int max_line_extent, int max_height, int line_height,
 *      int x, int y, RECT* bounding)
 * \brief Calculate the bounding rectangle of a layout.
 *
 * This function calculates the bounding rectangle of the specific
 * layout object \a layout.
 *
 * \param layout The LAYOUT object.
 * \param max_line_extent The maximal line extent; only effective if
 *      the line extent mode of the layout object is variable.
 * \param max_height The maximal height of the lines; no limit if
 *      it is less than 0.
 * \param line_height The line height. If it is less than 0, this function
 *      will try to determine the line height automatically.
 * \param x The x-position of first line.
 * \param y The y-position of first line.
 * \param bounding The buffer to receive the result.
 *
 * \return The number of lines laid out.
 *
 * \note The position coordinates of the first line are
 *      with respect to the top-left corner of the output rectangle
 *      if the writing mode is GRF_WRITING_MODE_HORIZONTAL_TB or
 *      GRF_WRITING_MODE_VERTICAL_LR, the bottom-left corner if
 *      the writing mode is GRF_WRITING_MODE_HORIZONTAL_BT,
 *      the top-right corner if the writing mode is
 *      GRF_WRITING_MODE_VERTICAL_RL.
 *
 * \sa CreateLayout, DestroyLayout, LayoutNextLine
 *  GetLayoutLineRect
 *
 * Since 4.0.0
 */
MG_EXPORT int GUIAPI CalcLayoutBoundingRect(LAYOUT* layout,
        int max_line_extent, int max_height, int line_height,
        int x, int y, RECT* bounding);

/**
 * \fn BOOL DrawShapedGlyph(HDC hdc,
 *      Glyph32 glyph_value, const GLYPHPOS* glyph_pos,
 *      const RENDERDATA render_data)
 * \brief Draw a laid out glyph.
 *
 * This function draws a laied out glyph.
 * You can pass this function to \a LayoutNextLine as the callback
 * to draw a laid out glyph on the DC \a hdc.
 *
 * Note that the function will call DrawGlyph to draw the glyphs
 * and always draw the first glyph at the origin of the device context.
 * You may call \a SetMapMode and \a SetViewportOrg
 * before calling \a LayoutNextLine to change the logical coordinates
 * of the DC in order to refect the real device corrdinates.
 *
 * However, we strongly recommend that you use \a DrawLayoutLine to
 * output the glyphs for a better performance.
 *
 * \param hdc The device context.
 * \param glyph_value The glyph value.
 * \param glyph_pos The glyph position and orientation information.
 * \param render_data The rendering data of the glyph.
 *
 * \sa CreateLayout, DestroyLayout, LayoutNextLine,
 *      DrawLayoutLine, GLYPHPOS, RENDERDATA
 *
 * Since 4.0.0
 */
MG_EXPORT BOOL DrawShapedGlyph(HDC hdc,
        Glyph32 glyph_value, const GLYPHPOS* glyph_pos,
        const RENDERDATA* render_data);

/**
 * \fn DrawLayoutLine(HDC hdc, const LAYOUTLINE* line,
 *      int x, int y)
 * \brief Draw a laid out line at the specific position.
 *
 * This function draws a laied out line at the specified position.
 *
 * \param hdc The device context.
 * \param line The laid out LAYOUTLINE object. It should be one returned
 *      by \a LayoutNextLine.
 * \param x The buffer contained the x-corrdinate of the output position
 *      of the first glyph in the line. The x-corrdinate of the next line
 *      will be returned through this buffer as well.
 * \param y The buffer contained the y-corrdinate of the output position
 *      of the first glyph in the line. The y-corrdinate of the next line
 *      will be returned through this buffer as well.
 *
 * \return The number of glyphs drawn.
 *
 * \note The position coordinates of the first line are
 *      with respect to the top-left corner of the output rectangle
 *      if the writing mode is GRF_WRITING_MODE_HORIZONTAL_TB or
 *      GRF_WRITING_MODE_VERTICAL_LR, the bottom-left corner if
 *      the writing mode is GRF_WRITING_MODE_HORIZONTAL_BT,
 *      the top-right corner if the writing mode is
 *      GRF_WRITING_MODE_VERTICAL_RL.
 *
 * \sa CreateLayout, DestroyLayout, LayoutNextLine
 *
 * Since 4.0.0
 */
MG_EXPORT int DrawLayoutLine(HDC hdc, const LAYOUTLINE* line,
        int x, int y);

#ifdef _MGDEVEL_MODE
typedef struct _TextRun TEXTRUN;

MG_EXPORT TEXTRUN* GetNextTextRunInfo(TEXTRUNS* runinfo,
        TEXTRUN* prev,
        const char** fontname, int* start_index, int* length,
        LanguageCode* lang_code, ScriptType* script,
        BidiLevel* embedding_level, Uint8* flags);

/* Get layout line information */
MG_EXPORT BOOL GUIAPI GetLayoutLineInfo(LAYOUTLINE* line,
        int* max_extent, int* nr_chars, int* nr_glyphs,
        int** log_widths, int* width, int* height,
        BOOL* is_ellipsized, BOOL* is_wrapped);
#endif

#endif /* _MGCHARSET_UNICODE */

    /** @} end of complex_scripts */

/*
 * \var typedef struct _COMP_CTXT COMP_CTXT
 * \brief The context information of user defined color composition operators.
 *
 * \sa SetUserCompositionOps
 */
typedef struct _COMP_CTXT {
    /** the pointer to the destination */
    gal_uint8* cur_dst;

    /** The user context passed to SetUserCompositionOps */
    void* user_comp_ctxt;

    /** the pixel value should be skipped (the color key) */
    gal_pixel skip_pixel;

    /** the current pixel value for setpixel and setpixels operation */
    gal_pixel cur_pixel;

    /** the step of current pixel operations. */
    int step;
} COMP_CTXT;

/*
 * \var typedef struct _SPAN_CTXT SPAN_CTXT
 * \brief span context
 */
typedef struct _SPAN_CTXT {
    GAL_PixelFormat* dst_format; /* The pixel format of the destination pixels */
    GAL_PixelFormat* src_format; /* The pixel format of the source pixels */
    void * user_context; /* The user context passed to SetColorCompositeFunc */
} SPAN_CTXT;

/*
 * \var typedef struct _COMPOSITE_CTXT COMPOSITE_CTXT
 * \brief composite context
 */
typedef struct _COMPOSITE_CTXT {
    HDC dst_dc;
    int comp_mode;
    const int *far_bkmode;
    const int *far_step;
    const gal_pixel *far_skip_pixel;
} COMPOSITE_CTXT;

/*
 * \var typedef void (*CB_COMP_SETPIXEL) (COMP_CTXT* comp_ctxt)
 * \brief The prototype of the user defined color composition setpixel.
 *
 * This operator sets a pixel on the \a cur_dst defined in \a comp_ctxt with
 * the cur_pixel defined in \a comp_ctxt.
 *
 * \sa SetUserCompositionOps, COMP_CTXT
 */
typedef void (*CB_COMP_SETPIXEL) (COMP_CTXT* comp_ctxt);

/*
 * \var typedef void (*CB_COMP_SETHLINE) (COMP_CTXT* comp_ctxt, int w)
 * \brief The prototype of the user defined color composition sethline.
 *
 * This operator sets a horizital line on the \a cur_dst defined in \a comp_ctxt
 * with the \a cur_pixel defined in \a comp_ctxt, totally \a w pixels.
 *
 * \sa SetUserCompositionOps, COMP_CTXT
 */
typedef void (*CB_COMP_SETHLINE) (COMP_CTXT* comp_ctxt, int w);

/*
 * \var typedef void (*CB_COMP_PUTHLINE) (COMP_CTXT* comp_ctxt, gal_uint8* src, int w)
 * \brief The prototype of the user defined color composition puthline.
 *
 * This operator puts the pixels in \a src to a horizital line on the \a cur_dst
 * defined in \a comp_ctxt, totally \a w pixels.
 *
 * \sa SetUserCompositionOps, COMP_CTXT
 */
typedef void (*CB_COMP_PUTHLINE) (COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w);

/**
 * \fn int GUIAPI SetUserCompositionOps (HDC hdc, CB_COMP_SETPIXEL comp_setpixel,
 * CB_COMP_SETPIXELS comp_setpixels, CB_COMP_PUTLINE comp_putline, void* user_ctxt)
 * \brief Set user defined color composition operators.
 *
 * This function sets the user defined color composition operators of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param comp_setpixel The user defined setpixel operator, which will be called
 *        when MiniGUI sets a pixel in a DC.
 * \param comp_setpixels The user defined setpixels operator, which will be called
 *        when MiniGUI sets a consecutive pixels in one scanline of the DC.
 * \param comp_comp_putline The user defined putline operator, which will be called
 *        when MiniGUI sets a consecutive pixels in one scanline of the DC by
 *        using an array of source pixels.
 * \param user_ctxt The context which will be passed to the user defined operators.
 *
 * \return The old raster operation of the DC \a hdc when success, otherwise -1.
 *
 * \sa GetRasterOperation, SetRasterOperation, COMP_CTXT
 * */
MG_EXPORT int GUIAPI SetUserCompositionOps (HDC hdc,
        CB_COMP_SETPIXEL comp_setpixel,
        CB_COMP_SETHLINE comp_sethline,
        CB_COMP_PUTHLINE comp_puthline,
        void* user_comp_ctxt);

/**
 * This function gets the video handle which corresponds to the given
 * device context.
 *
 * \param hdc The device context.
 *
 * \return The handle to the video; NULL on error.
 *
 * Since 4.0.4
 */
MG_EXPORT GHANDLE GetVideoHandle (HDC hdc);

#ifdef _MGGAL_DRM

/**
  * \defgroup gdi_drm_fns Functions for Linux DRM integration
  *
  * These functions can be used to get the DRM device file descriptor and the
  * information of the hardware surface corresponding to the DC which is
  * created by the Linux DRM engine. By using the information, we can
  * integrate MiniGUI with other graphics libraries, such as Cairo
  * (vector graphics) and Mesa (3D graphics).
  *
  * \note Only available when support for Linuxe DRM NEWGAL engine
  * (`_MGGAL_DRM`) is enabled.
  *
  * Since 4.0.4
  *
  * @{
  */

/**
 * This function gets the file descriptor opened by the Linux DRM engine.
 *
 * \param video The video handle returned by \a GetVideoHandle.
 *
 * \return The DRM device file descriptor opened by the Linux DRM engine;
 *      >= 0 for success and < 0 on error. If \a surface is not a hardware
 *      surface created by the DRM engine, this function returns -1.
 */
MG_EXPORT int drmGetDeviceFD (GHANDLE video);

/**
 * THe struct type defines the DRM surface information.
 */
typedef struct _DrmSurfaceInfo {
    /** The local handle of the buffer object. */
    uint32_t handle;
    /** The prime fd of the buffer object. If it was no exported as a global
     buffer object, it has the value -1. */
    int prime_fd;
    /** The global name of the buffer object. It has the value 0 when it was
        not expored as global buffer object. */
    uint32_t name;
    /** The frame buffer identifier. If the buffer was not added as
        a frame buffer to the system, it has the value 0. */
    uint32_t fb_id;

    /** The width of the surface. */
    uint32_t width;
    /** The height of the surface. */
    uint32_t height;
    /** The row stride of the surface. */
    uint32_t pitch;
    /** The DRM pixel format. */
    uint32_t drm_format;
    /** Size in bytes of the buffer object. */
    size_t size;
    /** The offset from the buffer start to the real pixel data. */
    off_t offset;
} DrmSurfaceInfo;

/**
 * This function gets the DRM surface information from specified device context.
 *
 * \param video The handle to the video.
 * \param hdc The handle to the device context.
 * \param info The pointer to a DrmSurfaceInfo structure to hold
 *      the surface information.
 *
 * \return TRUE for success, FALSE for failure.
 */
MG_EXPORT BOOL drmGetSurfaceInfo (GHANDLE video, HDC hdc, DrmSurfaceInfo* info);

/**
 * This function creates a memory DC with a DRM surface which is created by
 * a foreign process and identified by a global name handle.
 *
 * \param video The video handle.
 * \param name The name handle of the DRM surface.
 * \param drm_format The DRM pixel format.
 * \param offset The offset from the buffer start to the real pixel data.
 * \param width The width of the DRM surface.
 * \param height The height of the DRM surface.
 * \param pitch The pitch (row stride) of the DRM surface.
 *
 * \return The handle to the memory DC for success, HDC_INVALID for failure.
 */
MG_EXPORT HDC drmCreateDCFromNameEx (GHANDLE video,
        uint32_t name, uint32_t drm_format, off_t offset,
        uint32_t width, uint32_t height, uint32_t pitch);

static inline HDC drmCreateDCFromName (GHANDLE video,
        uint32_t name, uint32_t drm_format,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    return drmCreateDCFromNameEx (video,
        name, drm_format, 0, width, height, pitch);
}

/**
 * This function creates a memory DC with a DRM surface which is created by
 * a foreign process and identified by a PRIME file descriptor.
 *
 * \param video The video handle.
 * \param prime_fd The PRIME file descriptor.
 * \param size The size of the DRM surface in bytes.
 * \param drm_format The DRM pixel format.
 * \param offset The offset from the buffer start to the real pixel data.
 * \param width The width of the DRM surface.
 * \param height The height of the DRM surface.
 * \param pitch The pitch (row stride) of the DRM surface.
 *
 * \return The handle to the memory DC for success, HDC_INVALID for failure.
 */
MG_EXPORT HDC drmCreateDCFromPrimeFdEx (GHANDLE video,
        int prime_fd, size_t size, uint32_t drm_format, off_t offset,
        uint32_t width, uint32_t height, uint32_t pitch);

static inline HDC drmCreateDCFromPrimeFd (GHANDLE video,
        int prime_fd, size_t size, uint32_t drm_format,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    return drmCreateDCFromPrimeFdEx (video,
        prime_fd, size, drm_format, 0, width, height, pitch);
}

/**
 * This function creates a memory DC with a DRM surface which is created by
 * a foreign graphics component.
 *
 * \param video The video handle.
 * \param handle The handle of the DRM surface.
 * \param size The size of the DRM surface in bytes.
 * \param drm_format The DRM pixel format.
 * \param offset The offset from the buffer start to the real pixel data.
 * \param width The width of the DRM surface.
 * \param height The height of the DRM surface.
 * \param pitch The pitch (row stride) of the DRM surface.
 *
 * \return The handle to the memory DC for success, HDC_INVALID for failure.
 */
MG_EXPORT HDC drmCreateDCFromHandleEx (GHANDLE video,
        uint32_t handle, size_t size, uint32_t drm_format, off_t offset,
        uint32_t width, uint32_t height, uint32_t pitch);

static inline HDC drmCreateDCFromHandle (GHANDLE video,
        uint32_t handle, size_t size, uint32_t drm_format,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    return drmCreateDCFromHandleEx (video,
        handle, size, drm_format, 0, width, height, pitch);
}

/** @} end of gdi_drm_fns */

#endif /* _MGGAL_DRM */

    /** @} end of gdi_fns */

#ifdef _MGGAL_HI3560

/* screen attribute type */
#define SCREEN_ATTR_ALPHA_CHANNEL    0x01      // alpha channel
#define SCREEN_ATTR_COLORKEY         0x02      // colorkey
#define SCREEN_ATTR_COLORSPACE       0x03      // colorspace
#define SCREEN_ATTR_ALPHA            0x04
#define SCREEN_NO_EXIST               -99      // screen don't exist

/* API speicific to Hi3560 GAL engines */
MG_EXPORT int hi3560GetVideoFD (void);
MG_EXPORT void* hi3560GetFBAddress (void);
MG_EXPORT int hi3560SetScreenAttr (Uint8 siAttr, void* pValue);

#endif /* _MGGAL_HI3560 */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _MGUI_GDI_H */


