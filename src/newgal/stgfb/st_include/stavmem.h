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
/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided “AS IS”, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File   stavmem.h                                                             
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/
#ifndef __STAVMEM_H
#define __STAVMEM_H

/* Includes ----------------------------------------------------------------- */

#include "stddefs.h"

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants ------------------------------------------------------- */

#define STAVMEM_DRIVER_ID       39
#define STAVMEM_DRIVER_BASE     (STAVMEM_DRIVER_ID << 16)


enum
{              /* max of 32 chars: | */
    STAVMEM_ERROR_FUNCTION_NOT_IMPLEMENTED = STAVMEM_DRIVER_BASE,
    STAVMEM_ERROR_MAX_PARTITION,            /* No more partition available for the user */
    STAVMEM_ERROR_INVALID_PARTITION_HANDLE, /* Encountered invalid partition handle */
    STAVMEM_ERROR_PARTITION_FULL,           /* no large enough unused memory places */
    STAVMEM_ERROR_CREATED_PARTITION,        /* Still at least one created partition */

    STAVMEM_ERROR_MAX_BLOCKS,               /* No more block available for the user */
    STAVMEM_ERROR_INVALID_BLOCK_HANDLE,     /* Encountered invalid block handle */
    STAVMEM_ERROR_ALLOCATED_BLOCK,          /* Still at least one allocated block */

    STAVMEM_ERROR_BLOCK_IN_FORBIDDEN_ZONE,  /* Block is crossing a forbidden zone */

    STAVMEM_ERROR_GPDMA_OPEN,               /* STGPDMA_Open() failed */
    STAVMEM_ERROR_GPDMA_CLOSE               /* STGPDMA_Close() failed */
};


typedef enum STAVMEM_AllocMode_e
{
    STAVMEM_ALLOC_MODE_INVALID,     /* Do NOT use, it is API internal */
    STAVMEM_ALLOC_MODE_RESERVED,    /* Do NOT use, it is API internal */
    STAVMEM_ALLOC_MODE_FORBIDDEN,   /* Do NOT use, it is API internal */

    /* Use elements below only */
    STAVMEM_ALLOC_MODE_TOP_BOTTOM,  /* Blocks allocated from top to bottom */
    STAVMEM_ALLOC_MODE_BOTTOM_TOP   /* Blocks allocated from bottom to top */
} STAVMEM_AllocMode_t;

typedef enum STAVMEM_DeviceType_e
{
    STAVMEM_DEVICE_TYPE_GENERIC,
    STAVMEM_DEVICE_TYPE_VIRTUAL    /* Virtual mapping for shared memory */
} STAVMEM_DeviceType_t;

/* Exported Types ----------------------------------------------------------- */

/* Structure for ranges of memory */
typedef struct STAVMEM_MemoryRange_s
{
    void *StartAddr_p;              /* First address of the range */
    void *StopAddr_p;               /* Last address of the range */
} STAVMEM_MemoryRange_t;


/* STAVMEM driver capabilities */
typedef struct STAVMEM_Capability_s
{
    U32  MaxPartition;                   /* Maximum number of partitions supported */
    U32  *FillBlock1DPatternSize;        /* Table of supported sizes for pattern in FillBlock1D (NULL means all sizes) */
    U32  FillBlock2DPatternHeight[2];    /* Table of supported height for 2D pattern in FillBlock1D (NULL means all sizes) */
    U32  *FillBlock2DPatternWidth;       /* Table of supported width for 2D pattern in FillBlock1D (NULL means all sizes) */
    BOOL IsCopyFillCapable;              /* availability of Copy and Fill functionas*/

} STAVMEM_Capability_t;

/* Parameters required to initialize shared memory management using virtual addresses. */
typedef struct STAVMEM_SharedMemoryVirtualMapping_s
{
    void * PhysicalAddressSeenFromCPU_p;
    void * PhysicalAddressSeenFromDevice_p;
    void * PhysicalAddressSeenFromDevice2_p;
    void * VirtualBaseAddress_p;
    U32 VirtualSize;
    U32 VirtualWindowOffset;
    U32 VirtualWindowSize;
} STAVMEM_SharedMemoryVirtualMapping_t;


/* Parameters required to initialise STAVMEM */
typedef struct STAVMEM_InitParams_s
{
    STAVMEM_DeviceType_t    DeviceType;                 /* allow make difference between some wanted / unwanted features */
    ST_Partition_t          *CPUPartition_p;            /* Where the module can allocate memory for its internal usage */
    ST_Partition_t          *NCachePartition_p;         /* Not used */
    U32                     MaxPartitions;              /* Max number of partitions that the user can create inside the working space */
    U32                     MaxBlocks;                  /* Max number of blocks that the user can allocate inside the working space */
    U32                     MaxForbiddenRanges;         /* Max number of forbidden ranges that have to be taken into account */
    U32                     MaxForbiddenBorders;        /* Max number of forbidden borders that have to be taken into account */
    U32                     MaxNumberOfMemoryMapRanges; /* Max number of sub-spaces to take into account in all partitions, must be at least 1 */

    void                    *OptimisedMemAccessStrategy_p;  /* Not used */

    void                    *BlockMoveDmaBaseAddr_p;    /* Base address of the block move DMA peripheral registers */
    void                    *CacheBaseAddr_p;           /* Base address of the cache registers */
    void                    *VideoBaseAddr_p;           /* Base address of the video registers */
    STAVMEM_SharedMemoryVirtualMapping_t *  SharedMemoryVirtualMapping_p; /* used only if virtual shared memory management */
    void                    *SDRAMBaseAddr_p;           /* Base address of the shared SDRAM in the CPU memory map, useless if virtual shared memory */
    U32                     SDRAMSize;                  /* Size of the SDRAM available, useless if virtual shared memory */

    U32                     NumberOfDCachedRanges;      /* Number of ranges to take into account in the following array */
    STAVMEM_MemoryRange_t   *DCachedRanges_p;           /* Used to inform STAVMEM of cached memory, so that it can preserve cache coherence */
    ST_DeviceName_t         GpdmaName;                  /* Not used */

} STAVMEM_InitParams_t;


/* Parameters required to create a memory partition */
typedef struct STAVMEM_CreatePartitionParams_s
{
    U32                     NumberOfPartitionRanges;      /* Number of ranges to take into account in the following array must be at least 1 */
    STAVMEM_MemoryRange_t * PartitionRanges_p;
} STAVMEM_CreatePartitionParams_t;


/* Parameters required to delete a memory partition */
typedef struct STAVMEM_DeletePartitionParams_s
{
    BOOL ForceDelete;               /* Force deletion: free of allocated blocks */
} STAVMEM_DeletePartitionParams_t;


/* Handle to a memory partition  */
typedef U32 STAVMEM_PartitionHandle_t;
#define STAVMEM_INVALID_PARTITION_HANDLE ((STAVMEM_PartitionHandle_t) NULL)

/* Parameters required to terminate STAVMEM */
typedef struct STAVMEM_TermParams_s
{
    BOOL ForceTerminate;            /* Force termination: deletion of existing partitions */
} STAVMEM_TermParams_t;


/* Handle to a memory block */
typedef U32 STAVMEM_BlockHandle_t;
#define STAVMEM_INVALID_BLOCK_HANDLE ((STAVMEM_BlockHandle_t) NULL)


/* Parameters required to allocate a memory block */
typedef struct STAVMEM_AllocBlockParams_s
{
    STAVMEM_PartitionHandle_t PartitionHandle;  /* Partition in which to allocate the block */
    U32 Size;                       /* Size of the block to allocate in bytes. 0 means full size. */
    U32 Alignment;                  /* Alignement of the first address of the block to allocate. */
    STAVMEM_AllocMode_t AllocMode;  /* Way the block is allocated:
                                        -STAVMEM_ALLOC_MODE_TOP_BOTTOM: from top to bottom
                                        -STAVMEM_ALLOC_MODE_BOTTOM_TOP: from bottom to top*/

    U32 NumberOfForbiddenRanges;    /* Number of forbidden ranges to take into account in the following array */
    STAVMEM_MemoryRange_t *ForbiddenRangeArray_p; /* Used to forbid memory allocation to be on certain memory zones. */
    U32 NumberOfForbiddenBorders;   /* Number of forbidden borders to take into account in the following array */
    void **ForbiddenBorderArray_p;  /* Used to forbid memory allocation to cross certain memory borders: borders below pointed address */

} STAVMEM_AllocBlockParams_t;

/* Type of the function that must be specified to perform data preservation when re-allocating */
typedef void (*STAVMEM_PreserveDataFunction_t) (
    void* SrcAddress_p,
    void* DestAddress_p,
    U32   Size
);

/* Parameters required to re-allocate a memory block */
typedef struct STAVMEM_ReAllocBlockParams_s
{
    STAVMEM_PartitionHandle_t PartitionHandle;  /* Partition in which the block was allocated*/
    U32 Size;                       /* New size to re-allocate the block in bytes. 0 means full size */
    U32 NumberOfForbiddenRanges;    /* Number of forbidden ranges to take into account in the following array */
    STAVMEM_MemoryRange_t *ForbiddenRangeArray_p; /* Used to forbid memory allocation to be on certain memory zones. */
    U32 NumberOfForbiddenBorders;   /* Number of forbidden borders to take into account in the following array */
    void **ForbiddenBorderArray_p;  /* Used to forbid memory allocation to cross certain memory borders: borders below pointed address */
    BOOL PreserveData;              /* Set this to preserve the data of the re-allocated block.
                                       If not set, data must be considered as lost after ReAlloc of the block */
    STAVMEM_PreserveDataFunction_t  PreserveDataFunction;   /* Function to perform data preservation */

} STAVMEM_ReAllocBlockParams_t;


/* Parameters required to free a memory block */
typedef struct STAVMEM_FreeBlockParams_s
{
    STAVMEM_PartitionHandle_t PartitionHandle;  /* Partition in which the block was allocated*/

} STAVMEM_FreeBlockParams_t;


/* Parameters returned when getting memory block parameters */
typedef struct STAVMEM_BlockParams_s
{
    U32 Size;                       /* Size of the block in bytes. 0 means full size */
    void *StartAddr_p;              /* First address of the block */
    STAVMEM_AllocMode_t AllocMode;  /* How the block was allocated */
    U32 Alignment;                  /* Alignment of the block first address */

} STAVMEM_BlockParams_t;

/* Exported Variables ------------------------------------------------------- */
extern STAVMEM_SharedMemoryVirtualMapping_t * STAVMEM_VIRTUAL_MAPPING_AUTO_P;

/* Exported Macros ---------------------------------------------------------- */
#if !defined(ST_OSLINUX) || defined(MODULE)
#if defined (ST_OSWINCE) && defined (DEBUG)
#ifndef STAPI_QUIET
#pragma message ("Use DEBUG STAVMEM functs")
#endif
void* STAVMEM_VirtualToCPU (void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_VirtualToDevice(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_CPUToDevice(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_DeviceToCPU(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p) ;
void* STAVMEM_DeviceToVirtual(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_VirtualToDevice2(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_CPUToVirtual(void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
char STAVMEM_IsAddressVirtual (void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
char STAVMEM_IsDataInVirtualWindow (void* Address, int Size, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
void* STAVMEM_IfVirtualThenToCPU (void* Address, STAVMEM_SharedMemoryVirtualMapping_t *ShMemVirtualMap_p);
#else /*defined (ST_OSWINCE) && defined (DEBUG)*/
#define STAVMEM_VirtualToCPU(Address, SharedMemoryVirtualMapping_p)                             \
            ((void *)(   (U8 *)(Address)                                                        \
                       - (   (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)       \
                           + (SharedMemoryVirtualMapping_p)->VirtualWindowOffset)               \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromCPU_p)))
/*usefull for cases were cpu is defferent from virtual*/
#define STAVMEM_CPUToVirtual(Address, SharedMemoryVirtualMapping_p)                             \
            ((void *)(   (U8 *)(Address)                                                        \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromCPU_p) \
                       + ( (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)      \
                       + (  SharedMemoryVirtualMapping_p)->VirtualWindowOffset)))

#define STAVMEM_VirtualToDevice(Address, SharedMemoryVirtualMapping_p)                             \
            ((void *)(   (U8 *)(Address)                                                           \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)              \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromDevice_p)))

#define STAVMEM_CPUToDevice(Address, SharedMemoryVirtualMapping_p)                                 \
            ((void *)(   (U8 *)(Address)                                                           \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromCPU_p)     \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromDevice_p)))

#define STAVMEM_DeviceToCPU(Address, SharedMemoryVirtualMapping_p)                                 \
            ((void *)(   (U8 *)(Address)                                                           \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromDevice_p)     \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromCPU_p)))
/*modified to take in consideration cases were virtual!=cpu*/
#define STAVMEM_DeviceToVirtual(Address, SharedMemoryVirtualMapping_p)                                 \
            ((void *)(   (U8 *)(Address)                                                           \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromDevice_p)     \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)))

#define STAVMEM_VirtualToDevice2(Address, SharedMemoryVirtualMapping_p)                             \
            ((void *)(   (U8 *)(Address)                                                           \
                       - (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)              \
                       + (U8 *)((SharedMemoryVirtualMapping_p)->PhysicalAddressSeenFromDevice2_p)))


#define STAVMEM_IsAddressVirtual(Address, SharedMemoryVirtualMapping_p)                             \
            (   ((U8 *)(Address) >= (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p))   \
             && ((U8 *)(Address) <  ((U8 *)(  (SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p) \
                                            + (SharedMemoryVirtualMapping_p)->VirtualSize)))

#define STAVMEM_IsDataInVirtualWindow(Address, Size, SharedMemoryVirtualMapping_p)                             \
            (   ( (U8 *)(Address) >= (   (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p)          \
                                       + (SharedMemoryVirtualMapping_p)->VirtualWindowOffset ))                \
             && ( (U8 *)(Address) + (Size) <= (   (U8 *)((SharedMemoryVirtualMapping_p)->VirtualBaseAddress_p) \
                                                + (SharedMemoryVirtualMapping_p)->VirtualWindowOffset          \
                                                + (SharedMemoryVirtualMapping_p)->VirtualWindowSize )))

#define STAVMEM_IfVirtualThenToCPU(Address, SharedMemoryVirtualMapping_p)    \
            (STAVMEM_IsAddressVirtual(Address, SharedMemoryVirtualMapping_p) \
            ? (STAVMEM_VirtualToCPU(Address, SharedMemoryVirtualMapping_p))  \
            : Address)
#endif	/* ST_OSWINCE */
#else /* redefine macros to use in user mode  for FULL KERNEL version */
void  *STAVMEM_VirtualToCPU(void *Address, STAVMEM_SharedMemoryVirtualMapping_t * const SharedMemoryVirtualMapping_p);
void  *STAVMEM_VirtualToDevice(void *Address, STAVMEM_SharedMemoryVirtualMapping_t * const SharedMemoryVirtualMapping_p);

#endif
/* Exported Functions ------------------------------------------------------- */

/* Standard API functions */
ST_Revision_t STAVMEM_GetRevision(void);
ST_ErrorCode_t STAVMEM_GetCapability(const ST_DeviceName_t DeviceName, STAVMEM_Capability_t  * const Capability_p);
ST_ErrorCode_t STAVMEM_Init(const ST_DeviceName_t DeviceName, const STAVMEM_InitParams_t * const InitParams_p);
ST_ErrorCode_t STAVMEM_Term(const ST_DeviceName_t DeviceName, const STAVMEM_TermParams_t * const TermParams_p);

/* General functions */
ST_ErrorCode_t STAVMEM_GetFreeSize(const ST_DeviceName_t DeviceName, U32 *TotalFreeSize_p);
ST_ErrorCode_t STAVMEM_GetSharedMemoryVirtualMapping(STAVMEM_SharedMemoryVirtualMapping_t * const SharedMemoryVirtualMapping_p);

/*#ifdef STAVMEM_DEBUG_MEMORY_STATUS*/
#ifdef ST_OSWINCE
ST_ErrorCode_t STAVMEM_GetMemoryStatus(const ST_DeviceName_t DeviceName);
#else
ST_ErrorCode_t STAVMEM_GetMemoryStatus(const ST_DeviceName_t DeviceName, char *buf, int *len, U8 PartitionIndex );
#endif
/*#endif*/
/* Memory partitions functions */
ST_ErrorCode_t STAVMEM_CreatePartition(const ST_DeviceName_t DeviceName,
                                       const STAVMEM_CreatePartitionParams_t *CreateParams_p,
                                       STAVMEM_PartitionHandle_t *PartitionHandle_p);
ST_ErrorCode_t STAVMEM_DeletePartition(const ST_DeviceName_t DeviceName,
                                       const STAVMEM_DeletePartitionParams_t *DeleteParams_p,
                                       STAVMEM_PartitionHandle_t *PartitionHandle_p);
ST_ErrorCode_t STAVMEM_GetPartitionFreeSize(STAVMEM_PartitionHandle_t PartitionHandle, U32 *PartitionFreeSize_p);

/* Memory blocks functions */
ST_ErrorCode_t STAVMEM_AllocBlock(const STAVMEM_AllocBlockParams_t *AllocBlockParams_p, STAVMEM_BlockHandle_t *BlockHandle_p);
ST_ErrorCode_t STAVMEM_ReAllocBlock(const STAVMEM_ReAllocBlockParams_t *ReAllocBlockParams_p, STAVMEM_BlockHandle_t *BlockHandle_p);
ST_ErrorCode_t STAVMEM_FreeBlock(const STAVMEM_FreeBlockParams_t *FreeBlockParams_p, STAVMEM_BlockHandle_t *BlockHandle_p);
ST_ErrorCode_t STAVMEM_GetBlockParams(STAVMEM_BlockHandle_t BlockHandle, STAVMEM_BlockParams_t *BlockParams_p);
ST_ErrorCode_t STAVMEM_GetBlockAddress(STAVMEM_BlockHandle_t BlockHandle, void **Address_p);
ST_ErrorCode_t STAVMEM_GetBlockAddressPhysical(STAVMEM_BlockHandle_t BlockHandle, void **Address_Phyp);
ST_ErrorCode_t STAVMEM_GetBlockAddressCached(STAVMEM_BlockHandle_t BlockHandle, void **Address_Cp);
ST_ErrorCode_t STAVMEM_GetBlockAddressUncached(STAVMEM_BlockHandle_t BlockHandle, void **Address_NCp);
ST_ErrorCode_t STAVMEM_GetBlockAddressKernelUncached(STAVMEM_BlockHandle_t BlockHandle, void **Address_NCp);

#ifndef STAVMEM_NO_COPY_FILL /*flag added to compile and execute STAVMEM without copy and fill functions*/
/* Memory access functions */
ST_ErrorCode_t STAVMEM_CopyBlock1D(void * const SrcAddress, void * const DestAddress, const U32 Size);
ST_ErrorCode_t STAVMEM_CopyBlock2D(void * const SrcAddress, const U32 SrcWidth, const U32 SrcHeight, const U32 SrcPitch,
                                   void * const DestAddress, const U32 DestPitch);
ST_ErrorCode_t STAVMEM_FillBlock1D(void * const Pattern, const U32 PatternSize, void * const DestAddress, const U32 DestSize);
ST_ErrorCode_t STAVMEM_FillBlock2D(void * const Pattern, const U32 PatternWidth, const U32 PatternHeight, const U32 PatternPitch,
                                   void * const DestAddress, const U32 DestWidth, const U32 DestHeight, const U32 DestPitch);
#endif  /*STAVMEM_NO_COPY_FILL*/

#if defined(ST_OSLINUX)
STAVMEM_PartitionHandle_t STAVMEM_GetPartitionHandle( U32 PartitionID );
ST_ErrorCode_t STAVMEM_AllocBuffer( int PartitionNumber, U32 Size, U32  Alignment, void **BufferAddress_p );
ST_ErrorCode_t STAVMEM_FreeBuffer( int PartitionNumber, void *BufferAddress_p );
ST_ErrorCode_t STAVMEM_GetSharedMemoryVirtualMapping2( U32 PartitionID, STAVMEM_SharedMemoryVirtualMapping_t * const SharedMemoryVirtualMapping_p );
#endif

/* C++ support */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __STAVMEM_H */

/* End of stavmem.h */
