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

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
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
    <http://www.minigui.com/en/about/licensing-policy/>.

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
 * graphics sub-system. You can access the arrary to get the
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
 */
extern MG_EXPORT gal_pixel SysPixelIndex [];

/**
 * \var RGB SysPixelColor []
 * \brief The pre-defined system RGB colors.
 *
 * The elements in this array are system colors in RGB triples.
 */
extern const MG_EXPORT RGB SysPixelColor [];

/**
 * \def PIXEL_invalid
 * \brief Invalid pixel.
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

/* Compatiblity definitions */
/**
 * \def COLOR_invalid
 * \sa PIXEL_invalid
 */
#define COLOR_invalid       PIXEL_invalid

/**
 * \def COLOR_transparent
 * \sa PIXEL_transparent
 */
#define COLOR_transparent   PIXEL_transparent

/**
 * \def COLOR_darkred
 * \sa PIXEL_darkred
 */
#define COLOR_darkred       PIXEL_darkred

/**
 * \def COLOR_darkgreen
 * \sa PIXEL_darkgreen
 */
#define COLOR_darkgreen     PIXEL_darkgreen

/**
 * \def COLOR_darkyellow
 * \sa PIXEL_darkyellow
 */
#define COLOR_darkyellow    PIXEL_darkyellow

/**
 * \def COLOR_darkblue
 * \sa PIXEL_darkblue
 */
#define COLOR_darkblue      PIXEL_darkblue

/**
 * \def COLOR_darkmagenta
 * \sa PIXEL_darkmagenta
 */
#define COLOR_darkmagenta   PIXEL_darkmagenta

/**
 * \def COLOR_darkcyan
 * \sa PIXEL_darkcyan
 */
#define COLOR_darkcyan      PIXEL_darkcyan

/**
 * \def COLOR_lightgray
 * \sa PIXEL_lightgray
 */
#define COLOR_lightgray     PIXEL_lightgray

/**
 * \def COLOR_darkgray
 * \sa PIXEL_darkgray
 */
#define COLOR_darkgray      PIXEL_darkgray

/**
 * \def COLOR_red
 * \sa PIXEL_red
 */
#define COLOR_red           PIXEL_red

/**
 * \def COLOR_green
 * \sa PIXEL_green
 */
#define COLOR_green         PIXEL_green

/**
 * \def COLOR_yellow
 * \sa PIXEL_yellow
 */
#define COLOR_yellow        PIXEL_yellow

/**
 * \def COLOR_blue
 * \sa PIXEL_blue
 */
#define COLOR_blue          PIXEL_blue

/**
 * \def COLOR_magenta
 * \sa PIXEL_magenta
 */
#define COLOR_magenta       PIXEL_magenta

/**
 * \def COLOR_cyan
 * \sa PIXEL_cyan
 */
#define COLOR_cyan          PIXEL_cyan

/**
 * \def COLOR_lightwhite
 * \sa PIXEL_lightwhite
 */
#define COLOR_lightwhite    PIXEL_lightwhite

/**
 * \def COLOR_black
 * \sa PIXEL_black
 */
#define COLOR_black         PIXEL_black

/**
 * \def SysColorIndex
 * \sa SysPixelIndex
 */
#define SysColorIndex       SysPixelIndex

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
typedef struct _BLOCKHEAP
{
#ifdef _MGRM_THREADS
    pthread_mutex_t lock;
#endif
    /**
     * Size of one block element.
     */
    size_t          bd_size;
    /**
     * Size of the heap in blocks.
     */
    size_t          heap_size;
    /**
     * The first free element in the heap.
     */
    int             free;
    /**
     * Pointer to the pre-allocated heap.
     */
    void*           heap;
} BLOCKHEAP;

/**
 * \var typedef BLOCKHEAP* PBLOCKHEAP
 * \brief Data type of the pointer to a BLOCKHEAP.
 *
 * \sa BLOCKHEAP
 */
typedef BLOCKHEAP* PBLOCKHEAP;

/**
 * \fn void InitBlockDataHeap (PBLOCKHEAP heap, \
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
 * \return \a heap->heap will contains a valid pointer on success, 
 *         NULL on error.
 *
 * \note This function does not return anything. You should check the \a heap
 *       field of the \a heap structure.
 *
 * \sa BLOCKHEAP
 */
MG_EXPORT extern void InitBlockDataHeap (PBLOCKHEAP heap, 
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

MG_EXPORT extern void* BlockDataAlloc (PBLOCKHEAP heap);

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
MG_EXPORT extern void BlockDataFree (PBLOCKHEAP heap, void* data);

/**
 * \fn void DestroyBlockDataHeap (PBLOCKHEAP heap)
 * \brief Destroys a private block data heap.
 *
 * \param heap The pointer to the heap to be destroied.
 *
 * \sa InitBlockDataHeap, BLOCKHEAP
 */
MG_EXPORT extern void DestroyBlockDataHeap (PBLOCKHEAP heap);

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
    * Type of the region, can be one of the following:
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
 *       will be a emgty region.
 *
 * \sa EmptyClipRgn, ClipRgnCopy, UnionRegion, SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn,
                       const CLIPRGN* pRgn1, const CLIPRGN* pRgn2);

/**
 * \fn void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect)
 * \brief Gets the bounding rectangle of a region.
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
 * \brief Sets a region to contain only one rect.
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
 * \brief Determines whether a region is an empty region.
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
 * \brief Determines whether a point is in a region.
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
MG_EXPORT BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y);

/**
 * \fn BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect)
 * \brief Determines whether a rectangle is intersected with a region.
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
MG_EXPORT BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect);

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
 * \brief Handle to the device context of the whole screen.
 *
 * This DC is a special one. MiniGUI uses it to draw popup menus and 
 * other global objects. You can also use this DC to draw lines or text on 
 * the screen directly, and there is no need to get or release it.
 *
 * If you do not want to create any main window, but you want to draw on
 * the screen, you can use this DC.
 *
 * \note MiniGUI does not do any clipping operation for this DC, 
 *       so use this DC may make a mess of other windows. 
 */
#define HDC_SCREEN          ((HDC)0)

/* 
 * This is a system screen DC create for MiniGUI internal usage, for example,
 * menu and z-order operations
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

/**
 * \fn unsigned int GUIAPI GetGDCapability (HDC hdc, int iItem)
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
 *
 * \return The capbility.
 */
MG_EXPORT unsigned int GUIAPI GetGDCapability (HDC hdc, int iItem);

/**
 * \fn HDC GUIAPI GetDC (HWND hwnd)
 * \brief Gets a window DC of a window.
 *
 * This function gets a window DC of the specified \a hwnd, and returns 
 * the handle to the DC. MiniGUI will try to return an unused DC from the 
 * internal DC pool, rather than allocate a new one from the system heap. 
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetClientDC, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetClientDC (HWND hwnd)
 * \brief Gets a client DC of a window.
 *
 * This function gets a client DC of the specified \a hwnd, and returns the 
 * handle to the DC. MiniGUI will try to return an unused DC from the 
 * internal DC pool, rather than allocate a new one from the system heap. 
 * Thus, you should release the DC when you finish drawing as soon as possible.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetDC, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetClientDC (HWND hwnd);

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
 * \brief Sets the alpha value for the entire surface of a DC, as opposed to
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
 * \brief Sets the color key (transparent pixel) of a memory DC.
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
 * MiniGUI will create the DC in the system heap, rather than allocate one 
 * from the DC pool. Thus, you can keep up the private DC in the life cycle 
 * of the window, and are not needed to release it for using by other windows.
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
 * allocate one from the DC pool. Thus, you can keep up the DC in the life 
 * cycle of the window, and are not needed to release it for using by 
 * other windows.
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
 * This function creates a private SubDC of the DC and returns the handle 
 * of the SubDC.
 *
 * When you calling \a CreatePrivateSubDC function to create a private 
 * Sub DC, MiniGUI will create the DC in the system heap, rather than 
 * allocate one from the DC pool. Thus, you can keep up the DC in the life 
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
 * \brief Creates a secondary window DC of a window.
 *
 * This function creates a secondary DC for the main window \a hwnd and 
 * returns the handle to the secondary DC.
 *
 * When you calling \a CreateSecondaryDC function, MiniGUI will create a 
 * memory DC which is compatible with HDC_SCREEN.
 * 
 * When a main window have the extended style \a WS_EX_AUTOSECONDARYDC, 
 * MiniGUI will create a Secondary DC for this main window in the creation 
 * progress of the main window, and destroy the DC when you destroy the 
 * window. MiniGUI will use this Secondary DC and its sub DCs to render 
 * the window content and childen.
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
#define ON_UPDSECDC_DONOTHING   ((ON_UPDATE_SECONDARYDC)0xFFFFFFFF)

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
 * \brief Retrives and returns the secondary DC of a specific window.
 *
 * This function retrives and returns the secondary DC of the window \a hwnd.
 * 
 * When a main window have the secondary DC, MiniGUI will use this secondary DC and 
 * its sub DCs to render the content of the window and its children.
 *
 * \param hwnd The handle to the window.
 *
 * \return The handle to the secondary DC, HDC_SCREEN indicates that 
 *         the window has no secondary DC.
 *
 * \sa ReleaseSecondaryDC, SetSecondaryDC
 */
MG_EXPORT HDC GUIAPI GetSecondaryDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetSecondaryClientDC (HWND hwnd)
 * \brief Retrives and returns the client secondary DC of a specific window.
 *
 * This function retrives and returns the client secondary DC of the main window \a hwnd.
 * 
 * When a main window have the secondary DC, MiniGUI will use this secondary DC and 
 * its sub DCs to render the content of the window and its children.
 *
 * \param hwnd The handle to the main window.
 *
 * \return The handle to the client secondary DC, HDC_SCREEN indicates that 
 *         the main window has no secondary DC.
 *
 * \sa ReleaseSecondaryDC, SetSecondaryDC
 */
MG_EXPORT HDC GUIAPI GetSecondaryClientDC (HWND hwnd);

/**
 * \fn void GUIAPI ReleaseSecondaryDC (HWND hwnd, HDC hdc)
 * \brief Release the DC returned by \a GetSecondaryDC or \a GetSecondaryClientDC.
 *
 * \param hwnd The handle to the window.
 * \param hdc  The handle to the secondary DC.
 *
 * \return void
 *
 * \sa GetSecondaryDC, GetSecondaryClientDC
 */
MG_EXPORT void GUIAPI ReleaseSecondaryDC (HWND hwnd, HDC hdc);

/**
 * \fn void GUIAPI DeleteSecondaryDC (HWND hwnd)
 * \brief Deletes the secondary DC of the window.
 *
 * \param hwnd The handle to the window.
 *
 * \sa CreateSecondaryDC
 */
MG_EXPORT void GUIAPI DeleteSecondaryDC (HWND hwnd);

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
#define DC_ATTR_ALINE_EXTRA     7
#define DC_ATTR_BLINE_EXTRA     8

#define DC_ATTR_MAP_MODE        9

#define DC_ATTR_TEXT_ALIGN      10

#ifdef _MGHAVE_ADV_2DAPI

#define DC_ATTR_PEN_TYPE        11
#define DC_ATTR_PEN_CAP_STYLE   12
#define DC_ATTR_PEN_JOIN_STYLE  13
#define DC_ATTR_PEN_WIDTH       14

#define DC_ATTR_BRUSH_TYPE      15

#define NR_DC_ATTRS             16

#else

#define NR_DC_ATTRS             11

#endif


/**
 * \fn Uint32 GUIAPI GetDCAttr (HDC hdc, int attr)
 * \brief Gets a specified attribute value of a DC.
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
 *        Intercharacter spacing for the DC.
 *      - DC_ATTR_ALINE_EXTRA\n
 *        Spacing above line for the DC.
 *      - DC_ATTR_BLINE_EXTRA\n
 *        Spacing bellow line for the DC.
 *      - DC_ATTR_MAP_MODE\n
 *        mode of a DC.
  *      - DC_ATTR_TEXT_ALIGN\n
 *        Text-alignment flags of a DC.
 *
 * \return The attribute value.
 *
 * \sa SetDCAttr
 */
MG_EXPORT Uint32 GUIAPI GetDCAttr (HDC hdc, int attr);

/**
 * \fn Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value)
 * \brief Sets a specified attribute value of a DC.
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
 * \brief Gets the background color of a DC.
 *
 * \param hdc The device context.
 * \return The background pixel value of the DC \a hdc.
 *
 * \sa GetDCAttr, SetBkColor
 */
#define GetBkColor(hdc)     GetDCAttr (hdc, DC_ATTR_BK_COLOR)

/**
 * \def GetBkMode(hdc)
 * \brief Gets the background mode of a DC.
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
 * \brief Gets the pen color of a DC.
 *
 * \param hdc The device context.
 * \return The pen color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetPenColor
 */
#define GetPenColor(hdc)    GetDCAttr (hdc, DC_ATTR_PEN_COLOR)

/**
 * \def GetBrushColor(hdc)
 * \brief Gets the brush color of a DC.
 *
 * \param hdc The device context.
 * \return The brush color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetBrushColor
 */
#define GetBrushColor(hdc)  GetDCAttr (hdc, DC_ATTR_BRUSH_COLOR)

/**
 * \def GetTextColor(hdc)
 * \brief Gets the text color of a DC.
 *
 * \param hdc The device context.
 * \return The text color (pixel value) of the DC \a hdc.
 *
 * \sa GetDCAttr, SetTextColor
 */
#define GetTextColor(hdc)   GetDCAttr (hdc, DC_ATTR_TEXT_COLOR)

/**
 * \def GetTabStop(hdc)
 * \brief Gets the tabstop value of a DC.
 *
 * \param hdc The device context.
 * \return The tabstop value of the DC \a hdc.
 *
 * \sa GetDCAttr, SetTabStop
 */
#define GetTabStop(hdc)     GetDCAttr (hdc, DC_ATTR_TAB_STOP)

/**
 * \def SetBkColor(hdc, color)
 * \brief Sets the background color of a DC to a new value.
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
 * \brief Sets the background color of a DC to a new mode.
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
 * \brief Sets the pen color of a DC to a new value.
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
 * \brief Sets the brush color of a DC to a new value.
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
 * \brief Sets the text color of a DC to a new value.
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
 * \brief Sets the tabstop of a DC to a new value.
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
 * \brief Gets the raster operation of a DC.
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
 * \brief Sets the raster operation of a DC to a new value.
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
 * \brief Gets palette entries of a DC.
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
 * \brief Sets palette entries of a DC.
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
 * \brief Sets a DC with colorfule palette.
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
 *
 * \brief This function maps palette entries from the current 
 *        logical palette to the system palette.
 *
 * RealizePalette modifies the palette for the device associated 
 * with the specified device context.
 *
 * If the device context is a display DC, the physical palette 
 * for that device is modified.  RealizePalette will return 
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
 * \brief Gets the system default logical palette.
 *
 * \return Handle to the system default logical palette on success, otherwise 0.
 *
 * \sa GetPalette
 */
MG_EXPORT HPALETTE GUIAPI GetDefaultPalette (void);

/**
 * \fn int GUIAPI GetPaletteEntries (HPALETTE hpal, \
                int start, int len, GAL_Color* cmap);
 * \brief Gets palette entries of a logical palette.
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
 * \brief Sets palette entries of a logical palette.
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
 * \brief Gets the nearest palette index in the logical palette for a
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
 * \brief Gets the nearest color compliant to a DC for a spefici color.
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
 * \brief Sets the pixel with a new pixel value at the specified position 
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
 * \brief Sets the pixel by a RGB triple at the specified position on a DC.
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
 * \brief Sets the pixel by a RGBA quarter at the specified position on a DC.
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
 * \brief Gets the pixel value at the specified position on a DC.
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
 * \brief Gets the pixel value at the specified position on a DC in RGB triple.
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
 * \brief Gets the pixel value at the specified position on a DC in 
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
 * \brief Gets the pixel values from a color array in RGBA quarter under a DC.
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
 * \brief Gets the pixel value from a color in RGBA quarter under a DC.
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
 * \brief Gets the pixel values from a color array in RGB triple under a DC.
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
 * \brief Gets the pixel value from a color in RGB triple under a DC.
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
    RGB rgb = {r, g, b, 0};
    gal_pixel pixel;

    RGB2Pixels (hdc, &rgb, &pixel, 1);
    return pixel;
}

/**
 * \fn void GUIAPI Pixel2RGBs (HDC hdc, const gal_pixel* pixels, RGB* rgbs, int count)
 * \brief Gets the colors in RGB triple from a pixel value array under a DC.
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
 * \brief Gets the color in RGB triple from a pixel value under a DC.
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
 * \brief Gets the colors in RGBA quarter from a array of pixel values under a DC.
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
 * \brief Gets the color in RGBA quarter from a pixel value under a DC.
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
 * \brief Checks a polygon is monotone vertical or not.
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
 *
 * \brief set bitmap scaler algorithm callback of DC according by scaler_type.
 *
 * This function is a set general bitmap scaler type that is DDA or Bilinear 
 interpolation algorithm. 
 * MiniGUI implements StretchBlt functions by using this scaler.
 *
 * \param hdc The device context.
 * \param scaler_type The type of scaler algorithm, BITMAP_SCALER_DDA 
 *                    or BITMAP_SCALER_BILINEAR.
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
 * \brief Gets the pen type of a DC.
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
 * \brief Sets the pen type of a DC to a new type.
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
 * \brief Sets the way dashed-lines are drawn. 
 *
 * Sets the way dashed-lines are drawn. Lines will be drawn with 
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
 * \brief Gets the pen cap style of a DC.
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
 * \brief Sets the pen type of a DC to a new type.
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
 * \brief Gets the pen join style of a DC.
 *
 * \param hdc The device context.
 * \return The pen join style of the DC \a hdc.
 *
 * \sa PTJoinStyle, GetDCAttr, SetPenJoinStyle
 */
#define GetPenJoinStyle(hdc)           GetDCAttr (hdc, DC_ATTR_PEN_JOIN_STYLE)

/**
 * \def SetPenJoinStyle(hdc, style)
 * \brief Sets the pen type of a DC to a new type.
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
 * \brief Gets the pen width of a DC.
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
 * \brief Sets the pen width of a DC to a new width.
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
typedef enum
{
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
 * \brief Gets the brush type of a DC.
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
 * \brief Sets the brush type of a DC to a new type.
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
 * \brief Retrieves the current mapping mode of a DC.
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
 * \brief Sets the mapping mode of a display context.
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
 * \brief Retrieves mapping parameters of a device context.
 *
 * This function retrieves mapping paramters of the specified device context 
 * \a hdc when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrieve, can be one of 
 *        the following values:
 *
 *      - DC_LCS_VORG\n
 *        Retrieves the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Retrieves the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Retrieves the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Retrieves the x-extents and y-extents of the window.
 *        
 * \param pt The coordinates or extents will be returned through this buffer.
 * 
 * \sa GetMapMode, SetMapMode, SetDCLCS
 */
MG_EXPORT void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt);

/**
 * \fn void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt)
 * \brief Sets mapping parameters of a device context.
 *
 * This function sets mapping paramters of the specified device context \a hdc
 * when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrieve, can be one of 
 *        the following values:
 *
 *      - DC_LCS_VORG\n
 *        Sets the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Sets the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Sets the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Sets the x-extents and y-extents of the window.
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
 * \brief Retrieves the x-coordinates and y-coordinates of the viewport 
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
 * \brief Retrieves the x-extents and y-extents of the current viewport for 
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
 * \brief Retrieves the x-coordinates and y-coordinates of the window for 
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
 * \brief Retrieves the x-extents and y-extents of the current window for 
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
 * \brief Sets the x-coordinates and y-coordinates of the viewport origin for 
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
 * \brief Sets the x-extents and y-extents of the current viewport for 
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
 * \brief Sets the x-coordinates and y-coordinates of the window for 
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
 * \brief Sets the x-extents and y-extents of the current window for 
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
 * \brief Checks whether a point is visible.
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
 * \brief Sets the visible region of a DC to be a rectangle. 
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
 * \brief Sets the visible region of a DC to be a region. 
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
 * \brief Retrieves the bounding rectangle of the current visible region of a DC.
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
 * \brief Checks whether the specified rectangle is visible.
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
 * \brief Retrieves the bounding rectangle of the current clipping region of a DC.
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
 * \brief Gets the current clipping region of a DC.
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
 * \brief Gets image box on a DC and saves it into a BITMAP object. 
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
 * \param dwRop The raster operation, currently ignored.
 *
 * \note The alpha and color key settings of the source DC will come into play.
 *
 * \sa StretchBlt, SetMemDCAlpha, SetMemDCColorKey
 */
MG_EXPORT void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh, 
                HDC hddc, int dx, int dy, DWORD dwRop);

/**
 * \fn void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh, \
                HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
 * \brief Copies a bitmap from a source rectangle into a destination 
 *        rectangle, streches the bitmap if necessary.
 *
 * This function copies a bitmap from a source rectangle into a destination 
 * rectangle, streching or compressing the bitmap to fit the dimension of 
 * the destination rectangle, if necessary. This function is similar with 
 * \sa BitBlt function except the former scaling the bitmap.  \a (dw,dh) 
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
 * \param dwRop The raster operation, currently ignored.
 *
 * \note The source rect should be contained in the device space entirely,
 *       and all coordinates should be in the device space.
 *
 * \note The source DC and dest DC must compatible, else will do nothing.
 *
 * \note The alpha and color key settings of the source DC will not come 
 *       into play.
 * 
 * \sa BitBlt, SetMemDCAlpha, SetMemDCColorKey
 */
MG_EXPORT void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh, 
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
 * \brief Sets pixel and alpha value in a BITMAP object.
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
 * \brief Sets pixel value in a BITMAP object.
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
 * \brief Loads an icon from a Windows ICO file.
 *
 * This function loads an icon from a Windows ICO file named \a filename 
 * and creates an icon object. This function can load mono-,16-color and 
 * 256-color icons.Some Windows ICO file contain two icons in different 
 * sizes. You can tell this function to load which icon though \a which, 
 * 0 for the first icon, and 1 for the second icon. Generally, the later 
 * icon is the larger icon.
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
 * \brief Loads an icon from a memory area.
 *
 * This function loads an icon from a memroy area pointed to by \a area.
 * The memory area has the same layout as the M$ Windows ICO file.
 * This function can load mono- ,16-color and 256-color icons. 
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
 * This function creates an icon from memory data rather than icon file. 
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
 * \brief Gets the size of an icon object.
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
 * \brief Sets a rectangle.
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
 * \brief Determines whether a point lies within an rectangle.
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
 * \brief Determines whether an rectangle is empty.
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
 * \brief Determines whether two rectangles are equal.
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
 * \brief Determines whether one rectangle is covered by another.
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
 * \brief Determines whether two rectangles intersect.
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
 * \brief Gets the bound rectangle of two source rectangles.
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
 * \brief Gets the width of a RECT object by using the pointer to it.
 */
#define RECTWP(prc)  ((prc)->right - (prc)->left)
/**
 * \def RECTHP(prc)
 * \brief Gets the height of a RECT object by using the pointer to it.
 */
#define RECTHP(prc)  ((prc)->bottom - (prc)->top)
/**
 * \def RECTW(rc)
 * \brief Gets the width of a RECT object.
 */
#define RECTW(rc)    ((rc).right - (rc).left)
/**
 * \def RECTH(rc)
 * \brief Gets the height of a RECT object.
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
#define LEN_FONT_NAME               31
#define LEN_DEVFONT_NAME            79
#define LEN_UNIDEVFONT_NAME         127

#define FONT_WEIGHT_NIL             '\0'
#define FONT_WEIGHT_ALL             '*'
#define FONT_WEIGHT_BLACK           'c'
#define FONT_WEIGHT_BOLD            'b'
#define FONT_WEIGHT_BOOK            'k'
#define FONT_WEIGHT_DEMIBOLD        'd'
#define FONT_WEIGHT_LIGHT           'l'
#define FONT_WEIGHT_MEDIUM          'm'
#define FONT_WEIGHT_REGULAR         'r'
#define FONT_WEIGHT_SUBPIXEL        's'

#define FS_WEIGHT_MASK              0x000000FF
#define FS_WEIGHT_BLACK             0x00000001
#define FS_WEIGHT_BOLD              0x00000002
#define FS_WEIGHT_BOOK              0x00000004
#define FS_WEIGHT_DEMIBOLD          0x00000006 /* BOOK | BOLD */
#define FS_WEIGHT_LIGHT             0x00000010
#define FS_WEIGHT_MEDIUM            0x00000020
#define FS_WEIGHT_SUBPIXEL          0x00000040
#define FS_WEIGHT_REGULAR           0x00000000

#define FONT_SLANT_NIL              '\0'
#define FONT_SLANT_ALL              '*'
#define FONT_SLANT_ITALIC           'i'
#define FONT_SLANT_OBLIQUE          'o'
#define FONT_SLANT_ROMAN            'r'

#define FS_SLANT_MASK               0x00000F00
#define FS_SLANT_ITALIC             0x00000100
#define FS_SLANT_OBLIQUE            0x00000200
#define FS_SLANT_ROMAN              0x00000000

#define FONT_FLIP_NIL               '\0'
#define FONT_FLIP_HORZ              'H'
#define FONT_FLIP_VERT              'V'
#define FONT_FLIP_HORZVERT          'T'

#define FS_FLIP_MASK                0x0000F000
#define FS_FLIP_HORZ                0x00001000
#define FS_FLIP_VERT                0x00002000
#define FS_FLIP_HORZVERT            0x00003000 /* HORZ | VERT */

#define FONT_OTHER_NIL              '\0'
#define FONT_OTHER_AUTOSCALE        'S'
#define FONT_OTHER_TTFNOCACHE       'N'
#define FONT_OTHER_TTFKERN          'K'
#define FONT_OTHER_TTFNOCACHEKERN   'R'
#define FONT_OTHER_LCDPORTRAIT      'P'
#define FONT_OTHER_LCDPORTRAITKERN  'Q'

#define FS_OTHER_MASK               0x00FF0000
#define FS_OTHER_AUTOSCALE          0x00010000
#define FS_OTHER_TTFNOCACHE         0x00020000
#define FS_OTHER_TTFKERN            0x00040000
#define FS_OTHER_TTFNOCACHEKERN     0x00060000 /* KERN | NOCACHE */
#define FS_OTHER_LCDPORTRAIT        0x00100000
#define FS_OTHER_LCDPORTRAITKERN    0x00140000

/* 
 * Backward compatiblilty definitions. 
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

/*
#define FS_SETWIDTH_MASK            0x0000F000
#define FS_SETWIDTH_BOLD            0x00001000
#define FS_SETWIDTH_CONDENSED       0x00002000
#define FS_SETWIDTH_SEMICONDENSED   0x00004000
#define FS_SETWIDTH_TTFNOCACHE      0x00008000
#define FS_SETWIDTH_NORMAL          0x00000000
*/

#define FONT_SPACING_NIL            '\0'
#define FONT_SPACING_ALL            '*'
#define FONT_SPACING_MONOSPACING    'm'
#define FONT_SPACING_PROPORTIONAL   'p'
#define FONT_SPACING_CHARCELL       'c'

/*
#define FS_SPACING_MASK             0x000F0000
#define FS_SPACING_MONOSPACING      0x00010000
#define FS_SPACING_PROPORTIONAL     0x00020000
#define FS_SPACING_CHARCELL         0x00000000
*/

#define FONT_UNDERLINE_NIL          '\0'
#define FONT_UNDERLINE_ALL          '*'
#define FONT_UNDERLINE_LINE         'u'
#define FONT_UNDERLINE_NONE         'n'

#define FS_UNDERLINE_MASK           0x00F00000
#define FS_UNDERLINE_LINE           0x00100000
#define FS_UNDERLINE_NONE           0x00000000

#define FONT_STRUCKOUT_NIL          '\0'
#define FONT_STRUCKOUT_ALL          '*'
#define FONT_STRUCKOUT_LINE         's'
#define FONT_STRUCKOUT_NONE         'n'

#define FS_STRUCKOUT_MASK           0x0F000000
#define FS_STRUCKOUT_LINE           0x01000000
#define FS_STRUCKOUT_NONE           0x00000000

#define FONT_TYPE_NAME_LEN          3
#define FONT_TYPE_NAME_BITMAP_RAW   "rbf"
#define FONT_TYPE_NAME_BITMAP_VAR   "vbf"
#define FONT_TYPE_NAME_BITMAP_QPF   "qpf"
#define FONT_TYPE_NAME_BITMAP_UPF   "upf"
#define FONT_TYPE_NAME_BITMAP_BMP   "bmp"
#define FONT_TYPE_NAME_SCALE_TTF    "ttf"
#define FONT_TYPE_NAME_SCALE_T1F    "t1f"
#define FONT_TYPE_NAME_ALL          "*"

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
#define FONT_TYPE_ALL               0x00FF  /* "all" */

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
#define FONT_MIN_SIZE               4

struct _DEVFONT;
typedef struct _DEVFONT DEVFONT;

/** The logical font structure. */
typedef struct _LOGFONT {
    /** The type of the logical font. */
    char type [LEN_FONT_NAME + 1];
    /** The family name of the logical font. */
    char family [LEN_FONT_NAME + 1];
    /** The charset of the logical font. */
    char charset [LEN_FONT_NAME + 1];
    /** The styles of the logical font. */
    DWORD style;
    /** The size of the logical font. */
    int size;
    /** The rotation angle of the logical font. */
    int rotation;
    /** The ascent of the logical font. */
    int ascent;
    /** The descent of the logical font. */
    int descent;
    /** The scale factor of sbc device font. */
    unsigned short sbc_scale;
    /** The scale factor of mbc device font. */
    unsigned short mbc_scale;
    /** Device font in single charset set */
    DEVFONT* sbc_devfont;
    /** Device font in multiply charset set */
    DEVFONT* mbc_devfont;
} LOGFONT;
/**
 * \var typedef LOGFONT* PLOGFONT
 * \brief Data type of pointer to a LOGFONT.
 */
typedef LOGFONT*    PLOGFONT;
/**
 * \var typedef const LOGFONT* CPLOGFONT
 * \brief Data type of pointer to a const LOGFONT.
 */
typedef const LOGFONT*    CPLOGFONT;

struct _WORDINFO;
/**
 * \var typedef struct _WORDINFO WORDINFO
 * \brief Date type of _WORDINFO.
 */
typedef struct _WORDINFO WORDINFO;

#define MAX_LEN_MCHAR               6

typedef unsigned short  UChar16;
typedef int    UChar32;

#define MCHAR_TYPE_UNKNOWN          0xFFFF
#define MCHAR_TYPE_NUL              0x0000
#define MCHAR_TYPE_CTRL1            0x0100
#define MCHAR_TYPE_BEL              0x0200
#define MCHAR_TYPE_BS               0x0300
#define MCHAR_TYPE_HT               0x0400
#define MCHAR_TYPE_LF               0x0500
#define MCHAR_TYPE_VT               0x0600
#define MCHAR_TYPE_FF               0x0700
#define MCHAR_TYPE_CR               0x0800
#define MCHAR_TYPE_CTRL2            0x0900
#define MCHAR_TYPE_SPACE            0x0A00
#define MCHAR_TYPE_ZEROWIDTH        0x0B00
#define MCHAR_TYPE_GENERIC          0x0C00
#define MCHAR_TYPE_NOSPACING_MARK   0x0001
#define MCHAR_TYPE_LIGATURE         (MCHAR_TYPE_NOSPACING_MARK |0x0D00)
#define MCHAR_TYPE_VOWEL            (MCHAR_TYPE_NOSPACING_MARK |0x0E00)
#define MCHAR_TYPE_VOWEL_ABOVE      (MCHAR_TYPE_VOWEL |0x0002)
#define MCHAR_TYPE_VOWEL_BELLOW     (MCHAR_TYPE_VOWEL |0x0004)

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

struct _FONTOPS;
struct _CHARSETOPS;

typedef struct _FONTOPS FONTOPS;
typedef struct _CHARSETOPS CHARSETOPS;

/* charops fontops devont structure is  here. */
/** The device font structure. */
struct _DEVFONT
{
    /** The device font name. */
    char             name [LEN_UNIDEVFONT_NAME + 1];

    /** The styles of the device font. */
    DWORD            style;

    /** The pointer to font operation structure. */
    FONTOPS*         font_ops;

    /** The pointer to character set operation structure. */
    CHARSETOPS*      charset_ops;

    /** The pointer to next device font. */
    struct _DEVFONT* next;

    /** The device font used data. */
    void*            data;

    /** The device font used relationship. */
    void*            relationship;

    /** indicating if the data need to be unloaded before delete a devfont*/
    BOOL             need_unload;
};

#define SBC_DEVFONT_INFO(logfont) (logfont.sbc_devfont)
#define MBC_DEVFONT_INFO(logfont) (logfont.mbc_devfont)

#define SBC_DEVFONT_INFO_P(logfont) (logfont->sbc_devfont)
#define MBC_DEVFONT_INFO_P(logfont) (logfont->mbc_devfont)

/**
 * \def INV_LOGFONT
 * \brief Invalid logfont.
 */
#define INV_LOGFONT     0

/** The font metrics structure. */
typedef struct _FONTMETRICS
{
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
 * \brief Gets metrics information of a logical font.
 *
 * This function returns the font metrics information of the specified
 * logical font \a log_font.
 *
 * \param log_font The pointer to the logical font.
 * \param font_metrics The buffer receives the metrics information.
 * \return None.
 *
 * \sa GetGlyphBitmap, FONTMETRICS
 */
MG_EXPORT void GUIAPI GetFontMetrics (LOGFONT* log_font, 
                FONTMETRICS* font_metrics);

/** The glyph bitmap structure. */
typedef struct _GLYPHBITMAP
{
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
 * \brief Gets the glyph bitmap information when uses a logical font to 
 *        output a character.
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
 * \sa GetFontMetrics, GLYPHBITMAP
 */
MG_EXPORT void GUIAPI GetGlyphBitmap (LOGFONT* log_font, 
                const char* mchar, int mchar_len, 
                GLYPHBITMAP* glyph_bitmap);

#ifndef _MGRM_THREADS

/**
 * \fn BOOL GUIAPI InitVectorialFonts (void)
 * \brief Initializes vectorial font renderer.
 *
 * This function initializes vectorial font renderer for MiniGUI-Processes 
 * application. For the performance reason, MiniGUI-Processes does not load 
 * vetorical fonts, such as TrueType or Adobe Type1, at startup. If you 
 * want to render text in vectorial fonts, you must call this function 
 * to initialize TrueType and Type1 font renderer.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note Only defined for non-threads runmode. If your MiniGUI configured as 
 * MiniGUI-Threads, no need to initialize TrueType and Type1 font 
 * renderer explicitly.
 *
 * \sa TermVectorialFonts
 */
MG_EXPORT BOOL GUIAPI InitVectorialFonts (void);


/**
 * \fn void GUIAPI TermVectorialFonts (void)
 * \brief Terminates vectorial font renderer.
 *
 * This function terminates the vectorial font renderer.
 * When you are done with vectorial fonts, you should call this function to 
 * unload the vectorial fonts to save memory.
 *
 * \note Only defined for non-threads runmode. 
 *
 * \sa InitVectorialFonts
 */
MG_EXPORT void GUIAPI TermVectorialFonts (void);

#endif

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
 *      - FONT_TYPE_NAME_ALL\n
 *        Creates a logical font by using any type device font.
 * \param family The family of the logical font, such as "Courier", 
 *        "Helvetica", and so on.
 * \param charset The charset of the logical font. You can specify a 
 *        sigle-byte charset like "ISO8859-1", or a multi-byte charset 
 *        like "GB2312-0".
 * \param weight The weight of the logical font, can be one of the values:
 *      - FONT_WEIGHT_ALL\n
 *        Any one.
 *      - FONT_WEIGHT_REGULAR\n
 *        Regular font.
 *      - FONT_WEIGHT_BOLD\n
 *        Bold font.
 *      - FONT_WEIGHT_BOOK\n
 *        Anti-aliase font.
 *      - FONT_WEIGHT_LIGHT\n
 *        Draw the glyph border with the background color.
 * \param slant The slant of the logical font, can be one of the values:
 *      - FONT_SLANT_ALL\n
 *        Any one.
 *      - FONT_SLANT_ROMAN\n
 *        Regular font.
 *      - FONT_SLANT_ITALIC\n
 *        Italic font.
 * \param flip Does flip the glyph of the font, can be one of the following values:
 *      - FONT_FLIP_NIL\n
 *        Do not flip the glyph.
 *      - FONT_FLIP_HORZ\n
 *        Flip the glyph horizontally .
 *      - FONT_FLIP_VERT\n
 *        Flip the glyph vertically.
 *      - FONT_FLIP_HORZVERT\n
 *        Flip the glyph horizontally and vertically.
 * \param other Other rendering features, can be one of the following values:
 *      - FONT_OTHER_NIL\n
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
 *        TrueType and Adobe Type1 fonts.
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
 * \fn PLOGFONT GUIAPI CreateLogFontByName (const char* font_name)
 * \brief Creates a logical font by a font name.
 *
 * This function creates a logical font by a font name specified by 
 * \a font_name.
 *
 * \param font_name The name of the font.
 *
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFont, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontByName (const char* font_name);

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
 * \brief Gets logical font information of a DC.
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
 * \brief Gets the pointer to the current logical font of a DC.
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
typedef enum
{
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
} mg_FT_LcdFilter;

/**
 * \fn BOOL ft2SetLcdFilter(LOGFONT* logfont, mg_FT_LcdFilter filter) 
 *
 * \brief Set freetype2 smooth mode.
 *
 * This function can be used to set freetype2 library handle font smooth mode.
 *
 * \param logfont The logical font.
 *
 * \param filter The handle font smooth mode, it must be a value of mg_FT_LcdFilter.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa mg_FT_LcdFilter 
 */
MG_EXPORT BOOL GUIAPI ft2SetLcdFilter (LOGFONT* logfont, mg_FT_LcdFilter filter);

#endif

#if defined(_MGFONT_QPF) || defined(_MGFONT_FT2) || defined(_MGFONT_TTF) || defined(_MGFONT_UPF)
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
#endif

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
 * \brief Gets the system logical font through an font identifier.
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
 * \brief Gets the maximal width of a single-byte character of a system font.
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
 * \brief Gets the average width of a single-byte character of a system font.
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
 * \brief Gets the height of a single-byte character of a system font.
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
 * \brief Gets the current system charset.
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
 * \brief Gets the height of a character of the default system font.
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
 * \brief Gets the width of a single-byte character of the default system font.
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
 * \brief Gets the width of a multi-byte character of the default system font.
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
 * \brief Retrieves positions of multi-byte characters in a string.
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
 * \brief Retrieves information of multi-byte words in a string.
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
 * \brief Retrieves the length of the first multi-byte character in a string.
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
 * \brief Retrieves the length and info of the first multi-byte word in a string.
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

#ifdef _MGCHARSET_UNICODE

#include <stddef.h>
#include <stdlib.h>

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
 * \fn int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, UChar32 wc)
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
MG_EXPORT int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, UChar32 wc);

/**
 * \def WC2MB(log_font, dest, mstr, n)
 * \brief The backward compatibility version of WC2MBEx.
 *
 * \sa WC2MBEx
 */
#define WC2MB(log_font, s, wc) \
                   WC2MBEx (log_font, s, (UChar32)wc)

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

    /** @} end of text_parse_fns */

    /**
     * \defgroup text_output_fns Text output functions
     * @{
     */

/**
 * \fn int GUIAPI GetFontHeight (HDC hdc)
 * \brief Retrieves the height of the current logical font in a DC.
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
 * \brief Retrieves the maximal character width of the current logical font 
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
 * \brief Retrieves the current intercharacter spacing for the DC.
 * 
 * \sa SetTextCharacterExtra
 */
#define GetTextCharacterExtra(hdc)  GetDCAttr (hdc, DC_ATTR_CHAR_EXTRA)

/**
 * \def GetTextAboveLineExtra(hdc)
 * \brief Retrieves the current spacing above line for the DC.
 *
 * \sa SetTextAboveLineExtra
 */
#define GetTextAboveLineExtra(hdc)  GetDCAttr (hdc, DC_ATTR_ALINE_EXTRA)

/**
 * \def GetTextBellowLineExtra(hdc)
 * \brief Retrieves the current spacing bellow line for the DC.
 *
 * \sa SetTextBellowLineExtra
 */
#define GetTextBellowLineExtra(hdc) GetDCAttr (hdc, DC_ATTR_BLINE_EXTRA)

/**
 * \def SetTextCharacterExtra(hdc, extra)
 * \brief Sets the intercharacter spacing for the DC and returns 
 *        the old spacing value.
 * 
 * \sa GetTextCharacterExtra
 */
#define SetTextCharacterExtra(hdc, extra)       \
                SetDCAttr (hdc, DC_ATTR_CHAR_EXTRA, (DWORD) extra)

/**
 * \def SetTextAboveLineExtra(hdc, extra)
 * \brief Sets the spacing above line for the DC and returns the old value.
 *
 * \sa GetTextAboveLineExtra
 */
#define SetTextAboveLineExtra(hdc, extra)       \
                SetDCAttr (hdc, DC_ATTR_ALINE_EXTRA, (DWORD) extra)

/**
 * \def SetTextBellowLineExtra(hdc, extra)
 * \brief Sets the spacing bellow line for the DC and returns the old value.
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
 * \brief Retrieves the current text-alignment flags of a DC.
 *
 * \param hdc The device context.
 *
 * \return The return value is one or more of the following values:
 *
 *  - TA_TOP\n
 *      Specifies alignment of the x-axis and the top of the bounding 
 *      rectangle.
 *  - TA_BASELINE\n
 *      Specifies alignment of the x-axis and the baseline of the chosen font 
 *      within the bounding rectangle.
 *  - TA_BOTTOM\n
 *      Specifies alignment of the x-axis and the bottom of the bounding 
 *      rectangle.
 *  - TA_LEFT\n
 *      Specifies alignment of the y-axis and the left side of the bounding 
 *      rectangle.
 *  - TA_RIGHT\n
 *      Specifies alignment of the y-axis and the right side of the bounding 
 *      rectangle.
 *  - TA_CENTER\n
 *      Specifies alignment of the y-axis and the center of the bounding 
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
 * \brief Sets text-alignment flags of a DC.
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
#define SetTextAlign(hdc, ta_flags)        \
                SetDCAttr (hdc, DC_ATTR_TEXT_ALIGN, (DWORD)ta_flags)

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
 * \brief Retrieves the last text output position.
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
 *    Determines the width and the height of the rectangle. If there are 
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
#define BMP_TYPE_PRIV_PIXEL     0x00

#ifdef _FOR_MONOBITMAP
  #define BMP_TYPE_MONOKEY      0x40
#endif

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
     *    The bitmap have a private Alpha Mask array.
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
#ifdef _FOR_MONOBITMAP
    Uint32  bmColorRep;
#endif

    /** The width of the bitmap */
    Uint32  bmWidth;
    /** The height of the bitmap */
    Uint32  bmHeight;
    /** The pitch of the bitmap */
    Uint32  bmPitch;
    /** The bits of the bitmap */
    Uint8*  bmBits;

    /** The private pixel format */
    /*void*   bmAlphaPixelFormat;*/
    /** The Alpha Mask array of the bitmap */
    Uint8*  bmAlphaMask;

    /** The Alpha Pitch of the bitmap */
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

#include <stdio.h>
#include "endianrw.h"

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
 * \brief Checks the type of the bitmap in a data source.
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
 * default status is diabled. 
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
 * \fn int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP pBitmap, \
                MG_RWops* area, const char* ext)
 * \brief Loads a device-dependent bitmap from a general data source.
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
MG_EXPORT int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP pBitmap, 
                MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap, \
                const char* spFileName)
 * \brief Loads a device-dependent bitmap from a file.
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
                const void* mem, int size, const char* ext)
 * \brief Loads a device-dependent bitmap from memory.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap, 
                const void* mem, int size, const char* ext);

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
 * \brief Loads MYBITMAP scanlines from a data source one by one.
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
 * \brief Loads a MYBITMAP object from a data source.
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
 * \brief Loads a MYBITMAP object from a file.
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
                const void* mem, int size, const char* ext)
 * \brief Loads a MYBITMAP object from memory.
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
                const void* mem, int size, const char* ext);

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
 * \fn HDC GUIAPI InitSlaveScreen (const char* name, const char* mode)
 * \brief Initializes slave screen.
 * 
 * \param name The gal engine name.
 * \param mode The display mode. For example : 640x480-16bpp. 
 * \return Valid handle on success, HDC_INVALID on failure. 
 *
 */
MG_EXPORT HDC GUIAPI InitSlaveScreen (const char* name, const char* mode);

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
                const void* mem, int size, const char* ext);
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
                const void* mem, int size, const char* ext);

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
 *               int w, int h, const void* mem, int size, const char* ext)
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
                int w, int h, const void* mem, int size, const char* ext);

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

#ifdef _DEBUG
/*
 * Dump avl tree info
 */
void dump_tree (DEVFONT *dev_font);

/*
 * look up a node in bitmap font avl tree.
 */ 
int avl_look_up (DEVFONT *dev_font, char *start_mchar, int n);

/*
 * destroy bitmap font avl tree.
 */ 
void destroy_avl_tree (DEVFONT *dev_font);

#endif /* end of _DEBUG */

#endif /* end of _MGFONT_BMPF */ 

     /**
     * \addtogroup gdi_fns GDI functions
     *
     * @{
     */

     /**
      * \defgroup glyph  Glyph defines and operations
     *
     * MiniGUI maintains some glyph defines and operations
     *
     * @{
     */
typedef int Glyph32;
/**
 * \def INV_GLYPH_VALUE 
 */
#define INV_GLYPH_VALUE    -1

/**
 * \def IS_MBC_GLYPH(glyph)
 * \brief to judge wether the glyph is multibyte glyph
 *
 * \param glyph  glyph value
 */
#define IS_MBC_GLYPH(glyph)  ((glyph) & 0x80000000)

/**
 * \def SET_MBC_GLYPH(glyph)
 * \brief set the glyph with multibyte mask
 *
 * \param glyph glyph value 
 */
#define SET_MBC_GLYPH(glyph) ((glyph) | 0x80000000)

/**
 * \def REAL_GLYPH(glyph)
 * \brief get real glyph value from a glyph
 *
 * \param glyph glyph value 
 */
#define REAL_GLYPH(glyph)    ((glyph) & 0x7FFFFFFF)


/**
 * \def SELECT_DEVFONT(plogfont, glyph)
 * \brief select a device font acording to the glyph value.
 *
 * \param plogfont  pointer to a logical font.
 * \param glyph  glyph value.
 *
 */
#define SELECT_DEVFONT(plogfont, glyph) \
    ((glyph) & 0x80000000 ? \
    (plogfont)->mbc_devfont : \
    (plogfont)->sbc_devfont)

/**
 * \var typedef enum SHAPETYPE
 * \brief Data type of enum.
 */
typedef enum {
    GLYPH_ISOLATED,
    GLYPH_FINAL,
    GLYPH_INITIAL,
    GLYPH_MEDIAL
}SHAPETYPE;

/**
 * \var typedef struct  _GLYPHMAPINFO GLYPHMAPINFO
 * \brief Data type of struct _GLYPHMAPINFO.
 */
typedef struct _GLYPHMAPINFO{
    int byte_index;
    int char_len;
    BOOL is_rtol;
}GLYPHMAPINFO;


#define GLYPH_INFO_TYPE      1
#define GLYPH_INFO_METRICS   2
#define GLYPH_INFO_BMP       4

/*the type of glyph bitmap*/
#define GLYPHBMP_TYPE_MONO      0
#define GLYPHBMP_TYPE_GREY      1
#define GLYPHBMP_TYPE_SUBPIXEL  2
#define GLYPHBMP_TYPE_PRERENDER 3

/*the glyph info structure. */
/**
 * \var typedef struct  _GLYPHINFO GLYPHINFO
 * \brief Data type of struct _GLYPHINFO.
 */
typedef struct _GLYPHINFO
{
    /** mask indicate if the glyph_type, metrics, or bitmap infomation is valid*/
    char mask;
    /** type of glyph*/
    char glyph_type;
    /** type of glyph bitmap*/
    char bmp_type;

    Uint8 padding;

    /** the size of the devfont*/
    int height;
    /** the descent of the devfont*/
    int descent;

    /** The advance value of the glyph. */
    int advance_x, advance_y;
    /** The bounding box of the glyph. */
    int bbox_x, bbox_y;
    int bbox_w, bbox_h;

    /** The size of the glyph bitmap. */
    size_t bmp_size;
    /* The pitch of the glyph bitmap. */
    int bmp_pitch;
    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;

    /** the prerender bitmap */
    BITMAP prbitmap;
} GLYPHINFO;

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
 * \fn Glyph32 GUIAPI GetGlyphShape (LOGFONT* logfont, const char* mchar, \
 *         int mchar_len, SHAPETYPE shape_type)
 * \brief Get the glyph shape of a character.
 *
 * \param logfont The logical font.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of \a mchar in bytes.
 * \param shape_type The shape type of \a mchar.
 *
 * \return The multi-byte character's glyph shape value.
 */
MG_EXPORT Glyph32 GUIAPI GetGlyphShape (LOGFONT* logfont, const char* mchar,
        int mchar_len, SHAPETYPE shape_type);

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
 * \fn int GUIAPI DrawGlyphString (HDC hdc, int x, int y, \
 *      Glyph32* glyph_string, int len, int* adv_x, int* adv_y);
 * \brief Draw a glyph string.
 *
 * This function draws a glyph string to the specific postion of a DC.
 *
 * \param hdc The device context.
 * \param x The output start x position.
 * \param y The output start y position.
 * \param glyph_string The pointer to the glyph string.
 * \param len The length of the glyph string.
 * \param adv_x The pointer used to return the advance in x-coordinate of 
 *        the glyph string, can be NULL.
 * \param adv_y The pointer used to return the advance in y-coordinate of 
 *        the glyph string, can be NULL.
 *
 * \return The advance on baseline.
 */
MG_EXPORT int GUIAPI DrawGlyphString (HDC hdc, int x, int y, Glyph32* glyph_string, 
        int len, int* adv_x, int* adv_y);

/** 
 * \fn int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value, \
 *         GLYPHINFO* glyph_info)
 * \brief Retriev the information of a glyph.
 *
 * This function retrieves the information of a glyph.
 *
 * \param logfont The logical font.
 * \param glyph_value The glyph value.
 * \param glyph_info The glyph information will be returned through this struct.
 *
 * \note You must set glyph_info->mask to indicate what you want.
 *
 * \return The advance of the glyph string on baseline.
 *
 * \sa GLYPHINFO
 */
MG_EXPORT int GUIAPI GetGlyphInfo (LOGFONT* logfont, Glyph32 glyph_value,
        GLYPHINFO* glyph_info);

/** 
 * \fn int GUIAPI GetGlyphsExtent(HDC hdc, Glyph32* glyphs, int nr_glyphs, \
 *         SIZE* size)
 * \brief Get visual extent value of a glyph string.
 *
 * This function gets the extent value of a glyph string on a DC.
 *
 * \param hdc The device context.
 * \param glyphs Input glyph string, input.
 * \param nr_glyphs Input glyph string len, input.
 * \param size Ouput the fit glyph's extent value, Output.
 *
 * \return The extent of the glyph string.
 */
MG_EXPORT int GUIAPI GetGlyphsExtent(HDC hdc, Glyph32* glyphs, int nr_glyphs, 
        SIZE* size);

/** 
 * \fn int GUIAPI GetGlyphsExtentPoint(HDC hdc, Glyph32* glyphs, \
 *         int nr_glyphs, int max_extent, SIZE* size)
 * \brief Get the visual extent value of a glyph string.
 *
 * This function gets the visual extent value of a glpyh string.
 *
 * \param hdc The device context.
 * \param glyphs The pointer to the glyph string.
 * \param nr_glyphs The length of the glyph string len.
 * \param max_extent The maximal output extent value.
 * \param size The real extent of all visual glyphs in the glyph string.
 *
 * \return The the index of the last glyph which can be fit to the extent.
 */
MG_EXPORT int GUIAPI GetGlyphsExtentPoint(HDC hdc, Glyph32* glyphs,
        int nr_glyphs, int max_extent, SIZE* size);

/** \fn int GUIAPI BIDIGetTextLogicalGlyphs(LOGFONT* log_font, \
        const char* text, int text_len, Glyph32** glyphs, \
        GLYPHMAPINFO** glyphs_map)
 * \brief Get logical glyphs string of the text.
 *
 * \param log_font The logical font.
 * \param text The logical text string.
 * \param text_len The lenght of the logical text string in bytes.
 * \param glyphs The pointer to the logical glyph string.
 * \param glyphs_map The position map from the logical glyphs string to 
 *        the logical text.
 *
 * \return The length of the logical glyph string.
 *
 * \sa GLYPHMAPINFO
 */
MG_EXPORT int GUIAPI BIDIGetTextLogicalGlyphs(LOGFONT* log_font, 
        const char* text, int text_len, Glyph32** glyphs, 
        GLYPHMAPINFO** glyphs_map);

/** \fn int GUIAPI BIDIGetTextVisualGlyphs (LOGFONT* log_font, \
        const char* text, int text_len, Glyph32** glyphs, \
        GLYPHMAPINFO** glyphs_map)
 * \brief Get visual glyphs and glyph_map info relative with logical 
 *        string byte index. 
 *
 * \param log_font The logical font.
 * \param text The logical text string.
 * \param text_len The length of the logical text string in bytes.
 * \param glyphs The pointer to the visual glyph string.
 * \param glyphs_map The position map from visual glyphs string to 
 *        the logical text.
 *
 * \return The length of the visual glyph string.
 */
MG_EXPORT int GUIAPI BIDIGetTextVisualGlyphs (LOGFONT* log_font, 
        const char* text, int text_len, Glyph32** glyphs, 
        GLYPHMAPINFO** glyphs_map);

/** \fn Glyph32* GUIAPI BIDILogGlyphs2VisGlyphs (LOGFONT* log_font, \
 *         Glyph32* glyphs, int nr_glyphs, GLYPHMAPINFO* glyphs_map)
 * \brief Reorder the logical glyphs string to visual glyphs string.
 *
 * This function reorders the logical glyphs string to visual 
 * glyphs string. If \a glyphs_map is not NULL, get the visual 
 * glyphs map info.
 * 
 * \param log_font The logical font.
 * \param glyphs The pointer to the glyph string.
 * \param nr_glyphs The length of the glyph string.
 * \param glyphs_map The position map from the logical glyph string to
 *        the visual glyph string.
 *
 * \return The pointer to the visual glyph string.
 */
MG_EXPORT Glyph32* GUIAPI BIDILogGlyphs2VisGlyphs (LOGFONT* log_font,
        Glyph32* glyphs, int nr_glyphs, GLYPHMAPINFO* glyphs_map);

/** 
 * \fn void GUIAPI GetTextRangesLog2Vis (LOGFONT* log_font, char* text, \
        int text_len, int start_index, int end_index, int** ranges, \
        int* nr_ranges)
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
 */
MG_EXPORT void GUIAPI GetTextRangesLog2Vis (LOGFONT* log_font, char* text, 
        int text_len, int start_index, int end_index, int** ranges, 
        int* nr_ranges);

/** 
 * \fn void GUIAPI BIDIGetLogicalEmbeddLevels (LOGFONT* log_font, \
        Glyph32*  glyphs, int nr_glyphs, Uint8** embedding_level_list)
 * \brief Get the logical embedding levels for the logical glyph string
 *        and generate runs by embedding levels, the for reorder to get
 *        visual glyph string.
 *
 * \param log_font The logical font.
 * \param glyphs The pointer to the logical glyph string.
 * \param nr_glyphs The length of the glyph string.
 * \param embedding_level_list The logical embedding level.
 *
 * \return None.
 */
MG_EXPORT void GUIAPI BIDIGetLogicalEmbeddLevels (LOGFONT* log_font, 
        Glyph32*  glyphs, int nr_glyphs, Uint8** embedding_level_list);

/** 
 * \fn void GUIAPI BIDIGetVisualEmbeddLevels (LOGFONT* log_font, \
        Glyph32* glyphs, int nr_glyphs, Uint8**  embedding_level_list)
 * \brief Get the visual embedding levels for the given logical glyph 
 *        string, then you can get the edge for visual glyphs.
 *
 * \param log_font  The logical font.
 * \param glyphs The pointer to the logical glyph string.
 * \param nr_glyphs The length of the glyph string.
 * \param embedding_level_list The embedding level logical to visual.
 *
 * \return void.
 */
MG_EXPORT void GUIAPI BIDIGetVisualEmbeddLevels (LOGFONT* log_font, 
        Glyph32* glyphs, int nr_glyphs, Uint8**  embedding_level_list);

    /** @} end of glyph */
/*
 * \var typedef struct _COMP_CTXT COMP_CTXT
 * \brief The context information of user defined color composition operators.
 *
 * \sa SetUserCompositionOps
 * 
 */   
typedef struct _COMP_CTXT {
    /** the pointer to the destination */
    gal_uint8* cur_dst;

    /** The user context passed to SetUserCompositionOps */
    void* user_comp_ctxt;

    /** the pixel value shoulb be skipped (the color key) */
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
 * \var typedef struct _RASTER_CTXT RASTER_CTXT
 * \brief raster context
 */
typedef struct _RASTER_CTXT {
    HDC dst_dc;
} RASTER_CTXT;

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
MG_EXPORT int GUIAPI SetUserCompositionOps (HDC hdc, CB_COMP_SETPIXEL comp_setpixel, 
        CB_COMP_SETHLINE comp_sethline, CB_COMP_PUTHLINE comp_puthline, void* user_comp_ctxt);

    /** @} end of gdi_fns */

#ifdef _MGGAL_HI3560

//#include <hi_api.h>

/* API speicific to Hi3560 GAL engines */
MG_EXPORT int hi3560GetVideoFD (void);
MG_EXPORT void* hi3560GetFBAddress (void);
MG_EXPORT int hi3560SetScreenAttr (Uint8 siAttr, void* pValue);

#endif /* _MGGAL_HI3560 */

MG_EXPORT int GUIAPI GetTextMCharInfo (PLOGFONT log_font, const char* mstr, int len, 
                int* pos_chars);
MG_EXPORT int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr, int len, 
                int* pos_words, WORDINFO* info_words);
MG_EXPORT int GUIAPI GetFirstMCharLen (PLOGFONT log_font, const char* mstr, int len);
MG_EXPORT int GUIAPI GetLastMCharLen (PLOGFONT log_font, const char* mstr, int len);
MG_EXPORT int GUIAPI GetFirstWord (PLOGFONT log_font, const char* mstr, int len,
                    WORDINFO* word_info);
MG_EXPORT int GUIAPI MB2WCEx (PLOGFONT log_font, void* dest, BOOL wc32, 
                const unsigned char* mchar, int n);
MG_EXPORT int GUIAPI WC2MBEx (PLOGFONT log_font, unsigned char *s, UChar32 wc);
MG_EXPORT int GUIAPI MBS2WCSEx (PLOGFONT log_font, void* dest, BOOL wc32,
                const unsigned char* mstr, int mstr_len, int n,
                int* conved_mstr_len);
MG_EXPORT int GUIAPI WCS2MBSEx (PLOGFONT log_font, unsigned char* dest,
                const void *wcs, int wcs_len, BOOL wc32, int n,
                int* conved_wcs_len);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _MGUI_GDI_H */


