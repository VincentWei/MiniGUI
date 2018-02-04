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
 @File   stos.h                                                                
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/
#if !defined __STOS_H
#define __STOS_H

#ifndef STOS_NO_SYMBOL_COMPAT
#define OS21_SYMBOL_COMPAT
#define STOS_TIME_SYMBOL_COMPAT
#endif

#ifndef STOS_FORCE_DEPRECATED
#define STOS_NO_DEPRECATED
#endif

/* Includes ----------------------------------------------------------------- */
#include "stddefs.h"

#ifdef ST_OSLINUX
#if defined(STAPIREF_COMPAT)
#else
/* STFAE - START - Some drivers or applications need to access registers from userspace */
#include "stsys.h"
/* STFAE - STOP  - Some drivers or applications need to access registers from userspace */
#endif /* STAPIREF_COMPAT */
#endif

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                                OS20 include                             **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#ifdef ST_OS20
    #include <interrup.h>
    #include <kernel.h>
    #include <message.h>
    #include <mutex.h>
    #include <partitio.h>
    #include <semaphor.h>
    #include <task.h>
    #include <ostime.h>
    #include <cache.h>
    
    #include "string.h"
    
    #include "stcommon.h"
#endif

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                             OS21 include                                **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#if defined (ST_OS21) && !defined(ST_OSWINCE)
    #include <os21.h>
    #include <time.h>
    #if defined (ARCHITECTURE_ST40)
        #include <os21/st40.h>
        #include <os21/st40/cache.h>
    #endif /*#if defined ARCHITECTURE_ST40*/
    
    #include "string.h"
    
    #include "stcommon.h"
#endif


/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                       Linux kernel include                              **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#if defined(ST_OSLINUX) && defined(MODULE)
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif

/* next line to check */
#include <linux/module.h>
#include <linux/kernel.h> /* Needed for KERN_ALERT */
#include <linux/init.h>   /* Needed for the macros */
/* end to check */

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>    /* for verify_area */
#include <linux/errno.h> /* for -EBUSY */

#include <asm/io.h>       /* for ioremap */
#include <asm/atomic.h>
#include <asm/system.h>

#include <linux/ioport.h> /* for ..._mem_region */

#include <linux/spinlock.h>

#include <linux/time.h>
#include <linux/delay.h>

#include <asm/irq.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <linux/wait.h>
#include <linux/poll.h>

#include <linux/slab.h>
#include <linux/vmalloc.h>

#include <linux/netdevice.h>    /* ??? SET_MODULE_OWNER ??? */
#include <linux/cdev.h>         /* cdev_alloc */

#include <asm/param.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>

#include <asm/semaphore.h>

#include "linuxwrapper.h"
#include "stcommon.h"
#include "linuxcommon.h"
#endif /* ST_OSLINUX && MODULE */



/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                         Linux user include                              **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#if defined(ST_OSLINUX) && !defined(MODULE)
/* We want to use the GNU extension to POSIX.
   There is no GPL contamination : Posix lib is LGPL and not modified 
*/
#define _GNU_SOURCE 1 
#include <linux/version.h>


#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/poll.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#include "string.h"

#include "linuxwrapper.h"
#include "stcommon.h"
#include "linuxcommon.h"
#endif /* ST_OSLINUX && ! MODULE */



#ifdef STOS_NO_DEPRECATED
#define STOS_DEPRECATED
#else
#define STOS_DEPRECATED    __attribute__ ((deprecated))
#endif


/* C++ support */
#if defined __cplusplus
    extern "C" {
#endif


/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                             Common type                                 **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

/* Interrupt type management */
typedef enum
{
    STOS_FAST_IRQ           = 0x01,
    STOS_SHARED_IRQ         = 0x02,
    STOS_RANDOM_SEED_IRQ    = 0x04
} STOS_InterruptType_t;

typedef struct STOS_InterruptParam_s
{
    STOS_InterruptType_t    InterruptType;
} STOS_InterruptParam_t;

enum{
    STOS_TaskFlags_Suspended = 1 /* no availlable for OS20 nor posix */
};

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                              OS20 type                                  **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#ifdef ST_OS20
/* Time management */
typedef clock_t                     STOS_Clock_t;

typedef int STOS_Error_t;
#define STOS_SUCCESS    (0)
#define STOS_FAILURE    (-1)

typedef partition_t                 STOS_Partition_t;

typedef task_context_t              STOS_TaskContext_t;
enum {
    STOS_TaskContext_Task       =  task_context_task     ,
    STOS_TaskContext_Interrupt  =  task_context_interrupt
};

typedef task_t                      STOS_Task_t;
typedef tdesc_t                     STOS_TaskDesc_t;
typedef task_flags_t                STOS_TaskFlags_t;

typedef mutex_t                     STOS_Mutex_t;
typedef semaphore_t                 STOS_Semaphore_t;

typedef message_queue_t             STOS_MessageQueue_t    ;
#endif

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                              OS21 type                                  **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#ifdef ST_OS21
/* Time management */
typedef osclock_t                   STOS_Clock_t;

typedef int STOS_Error_t;
#define STOS_SUCCESS    (OS21_SUCCESS)
#define STOS_FAILURE    (OS21_FAILURE)

typedef partition_t                 STOS_Partition_t;
typedef task_context_t              STOS_TaskContext_t;
enum {
    STOS_TaskContext_Task       =  task_context_task     ,
    STOS_TaskContext_Interrupt  =  task_context_system
};
typedef task_t                      STOS_Task_t;
typedef void *                      STOS_TaskDesc_t;
typedef task_flags_t                STOS_TaskFlags_t;

typedef mutex_t                     STOS_Mutex_t;
typedef semaphore_t                 STOS_Semaphore_t;

typedef message_queue_t             STOS_MessageQueue_t    ;
#endif


#if defined(ST_OSLINUX )
/* Linux specific directories */
#define STAPI_DEVICE_DIRECTORY              "/dev/stapi/"       /* Devices directory */
/* Avoiding debug.h... */
#define DEBUG_NOT_CONNECTED 0
#endif
/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                           Linux kernel type                             **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#if defined(ST_OSLINUX ) && defined (MODULE)

/* Parhaps this should go in a separate debug header file. */
#ifdef MODULE
#ifdef STAPI_ASSERT_ENABLED
#define assert(expr)                                                \
    do {                                                            \
        if(unlikely(!(expr))) {                                     \
            printk(KERN_ERR "Assertion failed! %s,%s,%s,line=%d\n", \
                #expr,__FILE__,__FUNCTION__,__LINE__);              \
        }                                                           \
    } while (0)
#else
#define assert(expr)
#endif
#endif

/* Statistics directory (root is /proc directory)
   This did not work the way people were using it: the same directory got created multiple times,
   which does not work right. We can use one subdirectory, but it must be created once.
 */
#define STAPI_STAT_DIRECTORY                (module_name(THIS_MODULE))	/* Module name */

/* Setting Linux paging values*/
#define LINUX_PAGE_SIZE         PAGE_SIZE
#define LINUX_PAGE_ALIGNMENT    PAGE_SIZE

/* Time management */
/* LM: In kernel mode, jiffies is the right clock definition but may
introduce warnings during compilations as long as function are not
ported to stos.c. jiffies are unsigned long */
typedef unsigned long STOS_Clock_t ;
/* This is *only* okay because we never de-reference it. It'll need to
 * be fixed if we ever do.
 */
#define TIMEOUT_INFINITY    ((STOS_Clock_t *)NULL)
#define TIMEOUT_IMMEDIATE   ((STOS_Clock_t *)-1)

typedef int STOS_Error_t;
#define STOS_SUCCESS    (0)
#define STOS_FAILURE    (-1)

typedef int                 STOS_Partition_t;




#define MAX_USER_PRIORITY    99
#define MIN_USER_PRIORITY    1

typedef int                       STOS_TaskContext_t;
enum {
    STOS_TaskContext_Task      =   0     ,
    STOS_TaskContext_Interrupt =   1
};
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
typedef struct task_struct STOS_Task_t;
#else
typedef task_t             STOS_Task_t;
#endif
typedef void *             STOS_TaskDesc_t;
typedef int                STOS_TaskFlags_t;


#ifdef OPTIMIZED_SEMAPHORES
/* Just a Linux semaphore */
typedef struct semaphore STOS_Semaphore_t;
#else  /* OPTIMIZED_SEMAPHORES */
typedef struct STOS_Semaphore_s
{
    wait_queue_head_t   wait;
    atomic_t            count;
} STOS_Semaphore_t;
#endif  /* OPTIMIZED_SEMAPHORES */

#ifdef DO_NOT_SUPPORT_RECURSIVE_MUTEX
typedef struct semaphore STOS_Mutex_t;
#else   /* DO_NOT_SUPPORT_RECURSIVE_MUTEX */
typedef struct {
	STOS_Task_t *owner;
	unsigned int count;
	wait_queue_head_t wait;
} STOS_Mutex_t;
#endif   /* DO_NOT_SUPPORT_RECURSIVE_MUTEX */


typedef struct
{
    STOS_Semaphore_t *    MsgSemaphore_p;
    STOS_Semaphore_t *    ClaimSemaphore_p;
    int                   Index;
} STOS_MessageQueue_t;
#endif


/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                            Linux user type                              **
 **                                 Posix                                   **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#if defined(ST_OSLINUX ) && !defined (MODULE)

/* Time management */
/* This is *only* okay because we never de-reference it. It'll need to
 * be fixed if we ever do.
 */
#define TIMEOUT_INFINITY    ((STOS_Clock_t *)NULL)
#define TIMEOUT_IMMEDIATE   ((STOS_Clock_t *)-1)
typedef long long       STOS_Clock_t ;

typedef int STOS_Error_t;
#define STOS_SUCCESS    (0)
#define STOS_FAILURE    (-1)

typedef int                 STOS_Partition_t;


#define MAX_USER_PRIORITY    99
#define MIN_USER_PRIORITY    1

typedef int                       STOS_TaskContext_t;
enum {
    STOS_TaskContext_Task       =  0     ,
    STOS_TaskContext_Interrupt  =  1
};
typedef pthread_t           STOS_Task_t;
typedef void *              STOS_TaskDesc_t;
typedef int                 STOS_TaskFlags_t;

typedef sem_t       STOS_Semaphore_t;
typedef pthread_mutex_t STOS_Mutex_t;

typedef struct
{
    STOS_Semaphore_t *    MsgSemaphore_p;
    STOS_Semaphore_t *    ClaimSemaphore_p;
    int                   Index;
} STOS_MessageQueue_t;

#endif

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                              WinCE type                                 **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/
#ifdef ST_OSWINCE
typedef   /* To define */      STOS_Clock_t;


typedef int                 STOS_Partition_t;

typedef int                       STOS_TaskContext_t;
enum {
    STOS_TaskContext_Task       =  0     ,
    STOS_TaskContext_Interrupt  =  1
};
typedef void                STOS_Task_t;
typedef void                STOS_TaskDesc_t;
typedef void                STOS_TaskFlags_t;


typedef int STOS_Error_t;
#define STOS_SUCCESS    (0)
#define STOS_FAILURE    (-1)


typedef   /* To define */      STOS_Mutex_t;

#endif

/* Defines ----------------------------------------------------------------- */
#if !defined __STLITE_H                /* This part below was taken from stlite.h */

#ifndef ST_OSWINCE
/* The following is required for (legacy) PTI and TestTool compatibility,
  which require the identifiers boolean, true, and false */
#if defined __cplusplus
/* true and false are already defined. Define the type the same size
  as under C compilation below (DDTS 18417) */
    typedef int boolean;
#else
        #if !defined(true) && !defined(false)
            typedef enum {false = 0, true = 1} boolean;
        #endif
#endif  /* __cplusplus */

/***********************************
 Toolset compatibility issues
***********************************/
/* ST20/ST40 __inline compatibility */
#if !defined __inline
    #if defined (ARCHITECTURE_ST40)
        #define __inline __inline__
    #endif
#endif

#endif /* !ST_OSWINCE */

#endif  /* #if !defined __STLITE_H */    /* This part above was taken from stlite.h */

/***************************************************************************************\
****************************************************************************************
****************************************************************************************
****************************************************************************************
****                                                                                ****
****                                                                                ****
****                          FUNCTION PROTOTYPE                                    ****
****                                                                                ****
****                                                                                ****
****************************************************************************************
****************************************************************************************
****************************************************************************************
***************************************************************************************/


/* Exported Functions ------------------------------------------------------- */
ST_Revision_t   STOS_GetRevision(void);

/* Tasks related functions */
ST_ErrorCode_t  STOS_TaskCreate (void (*Function)(void* Param),
                                       void* Param,
                                       STOS_Partition_t* StackPartition,
                                       size_t StackSize,
                                       void** Stack,
                                       STOS_Partition_t* TaskPartition,
                                       STOS_Task_t** Task,
                                       STOS_TaskDesc_t* Tdesc,
                                       int Priority,
                                       const char* Name,
                                       STOS_TaskFlags_t Flags );
ST_ErrorCode_t  STOS_TaskResume ( STOS_Task_t* Task);
ST_ErrorCode_t  STOS_TaskWait   ( STOS_Task_t** Task, const STOS_Clock_t * TimeOutValue_p );
ST_ErrorCode_t  STOS_TaskEnter  ( void * Param );
ST_ErrorCode_t  STOS_TaskExit   ( void * Param );
ST_ErrorCode_t  STOS_TaskDelete (STOS_Task_t* Task,
                                 STOS_Partition_t* TaskPartition,
                                 void* Stack,
                                 STOS_Partition_t* StackPartition );
STOS_TaskContext_t STOS_TaskContext(STOS_Task_t **task, int* level);


/* SEMAPHORES FUNCTIONS */

STOS_Semaphore_t * STOS_SemaphoreCreateFifo           (STOS_Partition_t * Partition_p, const int InitialValue);
STOS_Semaphore_t * STOS_SemaphoreCreateFifoTimeOut    (STOS_Partition_t * Partition_p, const int InitialValue);
STOS_Semaphore_t * STOS_SemaphoreCreatePriority       (STOS_Partition_t * Partition_p, const int InitialValue);
STOS_Semaphore_t * STOS_SemaphoreCreatePriorityTimeOut(STOS_Partition_t * Partition_p, const int InitialValue);
int  STOS_SemaphoreDelete(STOS_Partition_t * Partition_p, STOS_Semaphore_t * Semaphore_p);
void STOS_SemaphoreSignal(STOS_Semaphore_t * Semaphore_p);
int  STOS_SemaphoreWait(STOS_Semaphore_t * Semaphore_p);
int  STOS_SemaphoreWaitTimeOut  (STOS_Semaphore_t * Semaphore_p, const STOS_Clock_t * TimeOutValue);


/* Uncached memory access functions */
void    STOS_memsetPhysicalMemory(void* PhysicalAddress, U8 Pattern, U32 Size);

void    STOS_memsetUncached(void* PhysicalAddress, U8 Pattern, U32 Size)  STOS_DEPRECATED ;
void    STOS_memcpyUncachedToUncached(void* uncached_dest, void* uncached_src, U32 Size) STOS_DEPRECATED ;
void    STOS_memcpyCachedToUncached(void* uncached_dest, void* uncached_src, U32 Size) STOS_DEPRECATED ;
void    STOS_memcpyUncachedToCached(void* uncached_dest, void* uncached_src, U32 Size) STOS_DEPRECATED ;
void    STOS_memcpyCachedToCached(void* cached_dest, void* cached_src, U32 Size) STOS_DEPRECATED ;

void * STOS_memcpy(void * dst, const void * src, size_t count);



/* Memory Management */
void*   STOS_MemoryAllocate(STOS_Partition_t *part, size_t size);
void    STOS_MemoryDeallocate(STOS_Partition_t *part, void* block);
void*   STOS_MemoryReallocate (STOS_Partition_t* Partition, void* Block, size_t Requested, size_t OldSize);
void*   STOS_MemoryAllocateClear(STOS_Partition_t* Partition, size_t nelem, size_t elsize);

void    STOS_debug_print_Allocators(U32 limit);

#if defined(ARCHITECTURE_ST40)
#define STOS_DCACHE_LINE_SIZE    (32)
#elif defined(ARCHITECTURE_ST20)
#define STOS_DCACHE_LINE_SIZE    (16)
#elif defined(ARCHITECTURE_ST200)
#define STOS_DCACHE_LINE_SIZE    (32)
#else
#error STOS_DCACHE_LINE_SIZE no defined : architecture not known
#endif

/* Regions management related functions */
#if defined(ST_OSLINUX)
#if defined(MODULE)

#define STOS_InvalidateRegion(Addr, size)           dma_cache_inv((void *)(Addr), (int)(size))
#define STOS_FlushRegion(Addr, size)                dma_cache_wback((void *)(Addr), (int)(size))
#define STOS_PurgeRegion(Addr, size)                dma_cache_wback_inv((void *)(Addr), (int)(size))

void* STOS_VirtToPhys(void* vAddr);
void* STOS_MapRegisters(void* pAddr, U32 Length, char * Name);
void  STOS_UnmapRegisters(void* vAddr, U32 Length);
void* STOS_MapPhysToCached(void* pAddr, U32 Length);
void* STOS_MapPhysToUncached(void* pAddr, U32 Length);
void  STOS_UnmapPhysToCached(void* vAddr, U32 Length);
void  STOS_UnmapPhysToUncached(void* vAddr, U32 Length);

#else
#define STOS_VirtToPhys(vAddr)                      ((void *)NULL)  /* Not available in User */

#define STOS_InvalidateRegion(Addr, size)                           /* Not available in User */
#define STOS_FlushRegion(Addr, size)                                /* Not available in User */
#define STOS_PurgeRegion(Addr, size)                                /* Not available in User */

#if defined(STAPIREF_COMPAT)
#define STOS_MapRegisters(pAddr, Length, Name)      (NULL)          /* Not available in User */
#define STOS_UnmapRegisters(vAddr, Length)                          /* Not available in User */
#else
/* STFAE - START - Some drivers or applications need to access registers from userspace */
#define STOS_MapRegisters(pAddr, Length, Name)      (void *)STSYS_MapRegisters((U32)pAddr,Length,Name)
#define STOS_UnmapRegisters(vAddr, Length)          STSYS_UnmapRegisters((U32)vAddr,Length)
/* STFAE - STOP  - Some drivers or applications need to access registers from userspace */
#endif /* STAPIREF_COMPAT */

#define STOS_MapPhysToCached(pAddr, Length)         (NULL)          /* Not available in User */
#define STOS_MapPhysToUncached(pAddr, Length)       (NULL)          /* Not available in User */
#define STOS_UnmapPhysToCached(vAddr, Length)                       /* not available in User */
#define STOS_UnmapPhysToUncached(vAddr, Length)                     /* not available in User */
#endif  /* MODULE */



#elif defined(ST_OS21)

#if defined(STAPIREF_COMPAT)
#else
/* STFAE - Need to define NATIVE_CORE for 5301 */
#if !defined(NATIVE_CORE)
#if defined(ST_5301)
#define NATIVE_CORE
#endif
#endif
#endif /* STAPIREF_COMPAT */

#ifdef NATIVE_CORE
/* On host (PC or workstation) running the VSOC simulator no map needed and map could't be done by modifiyng  address MSB */
#define STOS_VirtToPhys(vAddr)                      (vAddr)
#define STOS_MapRegisters(pAddr, Length, Name)      (pAddr)
#define STOS_UnmapRegisters(vAddr, Length)

#define STOS_MapPhysToUncached(pAddr, Length)       (pAddr)
#define STOS_MapPhysToCached(pAddr, Length)         (pAddr)
#define STOS_UnmapPhysToCached(vAddr, Length)
#define STOS_UnmapPhysToUncached(vAddr, Length)
#else   /* NATIVE_CORE */

void  * STOS_VirtToPhys(void * vAddr);
#if (DVD_ADDRESSMODE==32)
#define STOS_MapRegisters(pAddr, Length, Name)      vmem_create(pAddr,Length,NULL,VMEM_CREATE_UNCACHED)
#define STOS_UnmapRegisters(vAddr, Length)          (void)vmem_delete(vAddr)
#define STOS_MapPhysToUncached(pAddr, Length)       vmem_create(pAddr,Length,NULL,VMEM_CREATE_UNCACHED)
#define STOS_MapPhysToCached(pAddr, Length)         vmem_create(pAddr,Length,NULL,VMEM_CREATE_CACHED)
#define STOS_UnmapPhysToCached(vAddr, Length)       (void)vmem_delete(vAddr)
#define STOS_UnmapPhysToUncached(vAddr, Length)     (void)vmem_delete(vAddr)
#else /* DVD_ADDRESSMODE==32 */
#define STOS_MapRegisters(pAddr, Length, Name)      ((void *)((U32)(pAddr) | 0xA0000000))
#define STOS_UnmapRegisters(vAddr, Length)
#define STOS_MapPhysToUncached(pAddr, Length)       ((void *)((U32)(pAddr) | 0xA0000000))
#define STOS_MapPhysToCached(pAddr, Length)         ((void *)((((U32)pAddr) & 0xDFFFFFFF) | 0x80000000))
#define STOS_UnmapPhysToCached(vAddr, Length)
#define STOS_UnmapPhysToUncached(vAddr, Length)
#endif /* DVD_ADDRESSMODE==32 */

#endif  /* NATIVE_CORE */

#define STOS_InvalidateRegion(Addr, size)           do {  if (size) {cache_invalidate_data((void *)(Addr), (unsigned int)(size)); } } while(0)
#define STOS_FlushRegion(Addr, size)                do {  if (size) {cache_flush_data((void *)(Addr), (unsigned int)(size)); } } while(0)
#define STOS_PurgeRegion(Addr, size)                do {  if (size) {cache_purge_data((void *)(Addr), (unsigned int)(size)); } } while(0)

#elif defined(ST_OS20)
#define STOS_VirtToPhys(vAddr)                      (vAddr)
/*  Cache invalidate is completly dangerous under ST20, remove this feature */
#define STOS_InvalidateRegion(Addr, size)           do {} while(0)
#define STOS_FlushRegion(Addr, size)                (void)cache_flush_data(0, 0)
/* Cache invalidate is completly dangerous under ST20, remove this feature */
#define STOS_PurgeRegion(Addr, size)                (void)cache_flush_data(0, 0)


#define STOS_MapRegisters(pAddr, Length, Name)      (pAddr)
#define STOS_UnmapRegisters(vAddr, Length)

#define STOS_MapPhysToUncached(pAddr, Length)       (pAddr)
#define STOS_MapPhysToCached(pAddr, Length)         (pAddr)
#define STOS_UnmapPhysToCached(vAddr, Length)
#define STOS_UnmapPhysToUncached(vAddr, Length)

#elif defined(ST_OSWINCE)
/* Memory model is the same as the OS21 29bits one : same macro will be used */
#define STOS_VirtToPhys(vAddr)                      ((void *)((U32)(vAddr) & 0x1FFFFFFF))
#define STOS_MapRegisters(pAddr, Length, Name)      ((void *)((U32)(pAddr) | 0xA0000000))
#define STOS_UnmapRegisters(vAddr, Length)

#define STOS_MapPhysToUncached(pAddr, Length)       ((void *)((U32)(pAddr) | 0xA0000000))
#define STOS_MapPhysToCached(pAddr, Length)         ((void *)((((U32)pAddr) & 0xDFFFFFFF) | 0x80000000))
#define STOS_UnmapPhysToCached(vAddr, Length)
#define STOS_UnmapPhysToUncached(vAddr, Length)

/* maybe the following macro need some update */
#define STOS_InvalidateRegion(Addr, size)
#define STOS_FlushRegion(Addr, size)
#define STOS_PurgeRegion(Addr, size)
#endif

#define STOS_AddressTranslate(newbase,oldbase,oldaddr)      ((void *)((U32)(newbase) + ((U32)(oldaddr) - (U32)(oldbase))))



void STOS_TaskLock(void);
void STOS_TaskUnlock(void);


/* Time management */
/* Time functions */
U32 STOS_GetClocksPerSecond(void);

STOS_Clock_t STOS_TimeNow(void);
STOS_Clock_t STOS_TimeMinus(STOS_Clock_t t1, STOS_Clock_t t2);
STOS_Clock_t STOS_TimePlus(STOS_Clock_t t1, STOS_Clock_t t2);
int STOS_TimeAfter(STOS_Clock_t t1,STOS_Clock_t t2);


STOS_Mutex_t *  STOS_MutexCreateFifo (void);
STOS_Mutex_t *  STOS_MutexCreatePriority(void);
int             STOS_MutexDelete (STOS_Mutex_t *mutex);
int             STOS_MutexLock (STOS_Mutex_t* mutex);
int             STOS_MutexTryLock(STOS_Mutex_t * mutex);
int             STOS_MutexRelease (STOS_Mutex_t* mutex);


/* Delays related functions */
void  STOS_TaskDelay(STOS_Clock_t ticks);
void  STOS_TaskDelayUntil(STOS_Clock_t timeout);
void  STOS_TaskDelayUs(int microsec);


static void STOS_TaskSchedule(void);
static void STOS_TaskYield(void);

/* Interrupts management */
void STOS_InterruptLock(void);
void STOS_InterruptUnlock(void);

#ifdef ST_OSLINUX
#ifdef MODULE
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
#define STOS_INTERRUPT_DECLARE(Fct, Param)      irqreturn_t Fct(int irq, void* Param)
#define STOS_INTERRUPT_PROTOTYPE(Fct)           irq_handler_t Fct
#define STOS_INTERRUPT_CAST(Fct)                (irqreturn_t(*)(int, void*))Fct
#define STOS_INTERRUPT_EXIT(ret)                return (ret == STOS_SUCCESS ? IRQ_HANDLED : IRQ_NONE)
#else
#define STOS_INTERRUPT_DECLARE(Fct, Param)      irqreturn_t Fct(int irq, void* Param, struct pt_regs* regs)
#define STOS_INTERRUPT_PROTOTYPE(Fct)           irqreturn_t(*Fct)(int, void* , struct pt_regs*)
#define STOS_INTERRUPT_CAST(Fct)                (irqreturn_t(*)(int, void* , struct pt_regs* ))Fct
#define STOS_INTERRUPT_EXIT(ret)                return (ret == STOS_SUCCESS ? IRQ_HANDLED : IRQ_NONE)
#endif

STOS_Error_t STOS_InterruptEnable(unsigned int Number,unsigned int Level); 
STOS_Error_t STOS_InterruptDisable(unsigned int Number,unsigned int Level);

/* No need on linux kernel: all interrupt are cleared inside the kernel IT controler */
#define STOS_InterruptClear(Number,Level)   (STOS_SUCCESS)

#else
/* Not applicable in user space*/
#define STOS_INTERRUPT_DECLARE(Fct, Param)      void Fct(void * Param)
#define STOS_INTERRUPT_PROTOTYPE(Fct)           void(*Fct)(void*)
#define STOS_INTERRUPT_CAST(Fct)                (void(*)(void*))Fct
#define STOS_INTERRUPT_EXIT(ret)                return

#define STOS_InterruptEnable(Number,Level)          (STOS_FAILURE)
#define STOS_InterruptDisable(Number,Level)         (STOS_FAILURE)
#define STOS_InterruptClear(Number,Level)                 (STOS_FAILURE)
#endif

#elif defined(ST_OSWINCE)
#define STOS_INTERRUPT_DECLARE(Fct, Param)      int Fct(void * Param)
#define STOS_INTERRUPT_PROTOTYPE(Fct)           int(*Fct)(void*)
#define STOS_INTERRUPT_CAST(Fct)                (int(*)(void*))Fct
#define STOS_INTERRUPT_EXIT(ret)                return(ret == STOS_SUCCESS ? OS21_SUCCESS : OS21_FAILURE)

int STOS_InterruptEnable(unsigned int interruptId, unsigned int level);
int STOS_InterruptDisable(unsigned int interruptId, unsigned int level);
#define STOS_InterruptClear(Number,Level) /* Not defined */

//Should be defined to be used in MSTV debug application to set STMES exception.
unsigned long STOS_GetNewSysintr(unsigned int interruptId);
void STOS_ReleaseSysintr(unsigned int sysintr);

#elif defined(ST_OS21)
#define STOS_INTERRUPT_DECLARE(Fct, Param)      int Fct(void * Param)
#define STOS_INTERRUPT_PROTOTYPE(Fct)           int(*Fct)(void*)
#define STOS_INTERRUPT_CAST(Fct)                (int(*)(void*))Fct
#define STOS_INTERRUPT_EXIT(ret)                return(ret == OS21_SUCCESS ? STOS_SUCCESS : STOS_FAILURE)

#define STOS_InterruptEnable(Number,Level)          (interrupt_enable(interrupt_handle(Number)) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptDisable(Number,Level)         (interrupt_disable(interrupt_handle(Number)) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptClear(Number,Level)           (interrupt_clear(interrupt_handle(Number)) == 0 ? STOS_SUCCESS : STOS_FAILURE)

#elif defined ST_OS20
#define STOS_INTERRUPT_DECLARE(Fct, Param)      void Fct(void * Param)

#define STOS_INTERRUPT_PROTOTYPE(Fct)           void(*Fct)(void*)
#define STOS_INTERRUPT_CAST(Fct)                (void(*)(void*))Fct
#define STOS_INTERRUPT_EXIT(ret)                return

#if defined(STAPI_INTERRUPT_BY_NUMBER)
#define STOS_InterruptEnable(Number,Level)          (interrupt_enable_number(Number) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptDisable(Number,Level)         (interrupt_disable_number(Number) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptClear(Number,Level)           (interrupt_clear_number(Number) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#else
#define STOS_InterruptEnable(Number,Level)          (interrupt_enable(Level) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptDisable(Number,Level)         (interrupt_disable(Level) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#define STOS_InterruptClear(Number,Level)           (interrupt_clear(Level) == 0 ? STOS_SUCCESS : STOS_FAILURE)
#endif

#endif
STOS_Error_t STOS_InterruptInstallConfigurable( U32 InterruptNumber,
                                    U32 InterruptLevel,
                                    STOS_INTERRUPT_PROTOTYPE(Fct),
                                    STOS_InterruptParam_t * InterruptParam_p,
                                    char * IrqName,
                                    void * Params);
STOS_Error_t STOS_InterruptInstall( U32 InterruptNumber,
                                    U32 InterruptLevel,
                                    STOS_INTERRUPT_PROTOTYPE(Fct),
                                    char * IrqName,
                                    void * Params);
STOS_Error_t STOS_InterruptUninstall(U32 InterruptNumber,
                                     U32 InterruptLevel,
                                     void * Params);
STOS_Error_t STOS_InterruptUninstallConfigurable(U32 InterruptNumber,
                                     U32 InterruptLevel,
                                     STOS_INTERRUPT_PROTOTYPE(Fct),
                                     STOS_InterruptParam_t * InterruptParam_p,
                                     void * Params);




/* Message Queue related functions */
#define STOS_MESSAGEQUEUE_MEMSIZE(Size, Nbr)            ((Size)*(Nbr))

STOS_MessageQueue_t * STOS_MessageQueueCreate(size_t ElementSize, unsigned int NoElements);
int STOS_MessageQueueDelete(STOS_MessageQueue_t* MessageQueue);
void* STOS_MessageQueueClaimTimeout(STOS_MessageQueue_t *queue, STOS_Clock_t * time_end_p);
void* STOS_MessageQueueClaim(STOS_MessageQueue_t * MessageQueue);
void STOS_MessageQueueRelease (STOS_MessageQueue_t* MessageQueue, void* Message);
void STOS_MessageQueueSend(STOS_MessageQueue_t *queue, void *message);
void *STOS_MessageQueueReceive (STOS_MessageQueue_t* MessageQueue);
void *STOS_MessageQueueReceiveTimeout (STOS_MessageQueue_t* MessageQueue, STOS_Clock_t * ticks);


/**********************************************************/
/*              LINUX KERNEL ONLY FUNCTION                */
/*                PREFIXED by STLINUX_                    */

/**********************************************************/
/**********************************************************/
/*                 DEVICES REGISTRATION                   */
/**********************************************************/
#if defined(ST_OSLINUX) && defined(MODULE)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,12)
#define class_simple                                        class
#define class_simple_create(a,b)                            ((struct class *)1)
#define class_simple_destroy(a)
#define class_simple_device_add(a,b,c,d_args...)            ((struct class_device *)1)
#define class_simple_device_remove(a)
#define io_remap_page_range(vma, vaddr, pfn, size, prot)    io_remap_pfn_range(vma, vaddr, (pfn) >> PAGE_SHIFT, size, prot)
#endif

int STLINUX_DeviceRegister(struct file_operations *stdevice_fops,
                         U32                     nbdevices,
                         char                   *DeviceName,
                         unsigned int           *DeviceMajor_p,
                         struct cdev            **stdevice_cdev,
                         struct class_simple    **stdevice_class);
int STLINUX_DeviceUnregister(U32                     nbdevices,
                           char                   *DeviceName,
                           unsigned int            DeviceMajor,
                           struct cdev            *stdevice_cdev,
                           struct class_simple    *stdevice_class);

STOS_Clock_t STLINUX_TimeNowUser(void);

/* *****************  Random procedure ***************** */
unsigned long rand(void);

/**********************************************************/
/*                   REGISTERS MAPPING                    */
/**********************************************************/
void * STLINUX_MapRegion(void * Address_p, U32 Width, char * RegionName);
void   STLINUX_UnmapRegion(void * MappedAddress_p, U32 Width);
int    STLINUX_MMapMethod(struct file *filp, struct vm_area_struct *vma);
#endif /* defined(ST_OSLINUX) && defined(MODULE) */

#if defined(ST_OSLINUX) && !defined(MODULE)
struct timeval  STOS_Clockt2Timeval(STOS_Clock_t clock) ;
#endif

/***************************************************************************************\
****************************************************************************************
****************************************************************************************
****************************************************************************************
****                                                                                ****
****                                                                                ****
****                          INLINED definition                                    ****
****                                                                                ****
****                                                                                ****
****************************************************************************************
****************************************************************************************
****************************************************************************************
***************************************************************************************/

/* Delay functions */
/* Time functions */
#if defined(ST_OSLINUX)
    #ifdef MODULE
    /* Jiffies */
    #define STLINUX_GetClocksPerSecond()      (HZ)
    #define STOS_GetClocksPerSecond()      (HZ)
    #else
    /* In user mode, it is up to us to define a base here. Let's set it to ms */
    #define STLINUX_GetClocksPerSecond()      (1000)
    #define STOS_GetClocksPerSecond()      (1000)
    #endif
#elif defined(ST_OS21) || defined(ST_OSWINCE)
    #define STOS_GetClocksPerSecond()     time_ticks_per_sec()
#elif defined (ST_OS20)
    /* from STCOMMON, we keep stcomm dependancie only fom ST20...
       Maybe we could recode the ST_GetClocksPerSecond function in STOS... */
    #define STOS_GetClocksPerSecond()     ST_GetClocksPerSecond() 
#endif


#if defined(ST_OSWINCE)
inline void STOS_TaskDelayUs(int microsec)   { STOS_TaskDelay(((unsigned __int64)(microsec)*ST_GetClocksPerSecond())/1000000); }
#endif

#if defined (ST_OS21) 
#define STOS_TaskDelay(ticks)          task_delay(ticks)
#define STOS_TaskDelayUntil(timeout)  task_delay_until(timeout)
#define STOS_TaskDelayUs(microsec)    do{                                      \
    U32     ClksPerSecs = ST_GetClocksPerSecond();                             \
    U32     Divisor = ((microsec) / (0xFFFFFFFF/ClksPerSecs)) + 1;             \
                                                                               \
    /* Original formula is: (microsec*ClksPerSecs)/1000000 which     */        \
    /* translates microseconds into ticks                            */        \
    /* but because of U32 calculus limitations, Divisor has been     */        \
    /* introduced in order to be sure that                           */        \
    /* (microsec*ClksPerSecs) does not exceeds U32 limit (0xFFFFFFFF)*/        \
    /* On 7100: from 1 to 16000 microsecs, no loss of accuracy in delays */    \
    /*         above 16000 microsecs, acceptable loss of accuracy may happen */\
    task_delay( ((((U32)(microsec)/Divisor))*ClksPerSecs)/(1000000/Divisor) ); \
} while(0)

#endif
#if defined (ST_OS20)
#define STOS_TaskDelay(ticks)           task_delay(ticks)
#define STOS_TaskDelayUntil(timeout)    task_delay_until(timeout)
#define STOS_TaskDelayUs(microsec)                                                              \
                do {                                                                            \
                    U32     ClksPerSecs = ST_GetClocksPerSecond();                              \
                    U32     Divisor = ((microsec) / (0xFFFFFFFF/ClksPerSecs)) + 1;              \
                                                                                                \
                    /* Original formula is: (microsec*ClksPerSecs)/1000000 which     */         \
                    /* translates microseconds into ticks                            */         \
                    /* but because of U32 calculus limitations, Divisor has been     */         \
                    /* introduced in order to be sure that                           */         \
                    /* (microsec*ClksPerSecs) does not exceeds U32 limit (0xFFFFFFFF)*/         \
                    /* On 7100: from 1 to 16000 microsecs, no loss of accuracy in delays */     \
                    /*          above 16000 microsecs, acceptable loss of accuracy may happen */\
                    task_delay( ((((U32)(microsec)/Divisor))*ClksPerSecs)/(1000000/Divisor) );  \
                } while (0)
#endif

/* Task scheduling related functions */
#ifdef ST_OSLINUX
#ifdef MODULE
inline void STOS_TaskSchedule(void)             { schedule(); } /* Was schedule(); ?? */
inline void STOS_TaskYield(void)                { yield(); }

#else
__inline void STOS_TaskSchedule()             { sched_yield(); }
__inline void STOS_TaskYield()                { sched_yield(); }
#endif

#elif defined(ST_OSWINCE)
void STOS_TaskSchedule();
void STOS_TaskYield();

#elif defined (ST_OS21)
__inline void STOS_TaskSchedule(void)             { task_reschedule(); }
__inline void STOS_TaskYield(void)                { task_yield(); } 

#elif defined (ST_OS20)
#define STOS_TaskSchedule()             do { task_reschedule(); } while(0)
#define STOS_TaskYield()                do { task_reschedule(); } while(0)
#endif


#if defined(ST_OS20) || defined(ST_OS21) || defined(ST_OSWINCE)
    #define STOS_TimeNow()         time_now()
    #define STOS_TimeMinus(t1,t2)  time_minus(t1,t2)
    #define STOS_TimePlus(t1,t2)   time_plus(t1,t2)
    #define STOS_TimeAfter(t1,t2)  time_after(t1,t2)
#endif

/* Mutex  functions defintion */
#if defined(ST_OS21) || defined(ST_OS20)
#define STOS_MutexCreateFifo()      mutex_create_fifo()
#define STOS_MutexCreatePriority()  mutex_create_priority()
#define STOS_MutexDelete(mutex)     mutex_delete(mutex)
#define STOS_MutexLock(mutex)       mutex_lock(mutex)
#define STOS_MutexRelease(mutex)    mutex_release(mutex)
#define STOS_MutexTryLock(mutex)    mutex_trylock(mutex)
#endif


#if defined(ST_OS20)
#define STOS_InterruptLock()                    interrupt_lock()
#define STOS_InterruptUnlock()                  interrupt_unlock()
#elif defined(ST_OSLINUX ) && !defined (MODULE)
/* interrupt no usable from user space. */
#define STOS_InterruptLock()    do { } while(0)
#define STOS_InterruptUnlock()  do { } while(0)
#define interrupt_lock()        do { } while(0)
#define interrupt_unlock()      do { } while(0)
#endif


/* Race conditions Management */
#if defined(ST_OSLINUX)
#if defined(MODULE)
    #define STOS_TaskLock()        task_lock(current)
    #define STOS_TaskUnlock()      task_unlock(current)
#endif

#elif defined(ST_OSWINCE)

#elif defined(ST_OS20) || defined(ST_OS21)
    #define STOS_TaskLock()        task_lock()
    #define STOS_TaskUnlock()      task_unlock()
#endif  /* LINUX */

#if defined(ST_OSLINUX)
   #define STOS_SemaphoreCreateFifo(part,val)              STOS_SemaphoreCreateFifoTimeOut(part, val)
   #define STOS_SemaphoreCreatePriority(part,val)          STOS_SemaphoreCreateFifoTimeOut(part, val)
   #define STOS_SemaphoreCreatePriorityTimeOut(part,val)   STOS_SemaphoreCreateFifoTimeOut(part, val)
#endif


#ifndef ST_OSWINCE
#define STOS_memcpy(cached_dest, cached_src, size)      memcpy(cached_dest, cached_src, size)
#endif /* !ST_OSWINCE */


/***************************************************************************************\
****************************************************************************************
****************************************************************************************
****************************************************************************************
****                                                                                ****
****                                                                                ****
****                        DEPRECATED DEFINITIONS                                  ****
****                                                                                ****
****                                                                                ****
****************************************************************************************
****************************************************************************************
****************************************************************************************
***************************************************************************************/


#ifdef OS21_SYMBOL_COMPAT
/* OS21 compatibility symbol [[[ */

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                            OS21 type Compat                             **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

/* OS20 Deprecated usage */
#ifdef ST_OS20

typedef STOS_Clock_t           osclock_t       STOS_DEPRECATED;

#ifdef task_flags_no_min_stack_size
#undef task_flags_no_min_stack_size
#endif
enum {
    task_flags_no_min_stack_size = 0    
    /* always set in OS21 task creation */
} STOS_DEPRECATED;

#if OS20_VERSION_MAJOR >= 2 && OS20_VERSION_MINOR >= 7
   #define interrupt_status(a,b) interrupt_status(a,b,0)
#endif

#endif

/* OS21 Deprecated usage */
#ifdef ST_OS21

typedef STOS_TaskDesc_t      tdesc_t       STOS_DEPRECATED;
#ifdef task_flags_high_priority_process
#undef task_flags_high_priority_process
#endif

enum 
{
    task_flags_high_priority_process = 0 
    /* not use inside STOS even for OS20 */
} STOS_DEPRECATED;

#endif

/* Linux kernel Deprecated usage */
#if defined(ST_OSLINUX) &&  defined (MODULE)

enum {
     OS21_SUCCESS = 0                 ,
     OS21_FAILURE = -1                
} STOS_DEPRECATED;

typedef STOS_Clock_t           osclock_t       STOS_DEPRECATED;
typedef STOS_Mutex_t           mutex_t         STOS_DEPRECATED;

#ifdef task_flags_no_min_stack_size
#undef task_flags_no_min_stack_size
#endif
#ifdef task_flags_suspended
#undef task_flags_suspended
#endif 
#ifdef task_flags_high_priority_process
#undef task_flags_high_priority_process
#endif 
enum {
    task_flags_no_min_stack_size = 0  ,
    /* always set in OS21 task creation */
    task_flags_suspended =1 ,
    /* not use inside STOS even for OS20 nor OS21 */
    task_flags_high_priority_process = 2 
    /* not use inside STOS even for OS20 */
} STOS_DEPRECATED ;
enum
{
    task_context_task      = STOS_TaskContext_Task      ,
    task_context_interrupt = STOS_TaskContext_Interrupt 
} STOS_DEPRECATED ;
typedef STOS_TaskContext_t     task_context_t  STOS_DEPRECATED;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
typedef STOS_Task_t            task_t          STOS_DEPRECATED;
#endif
typedef STOS_TaskDesc_t        tdesc_t         STOS_DEPRECATED;
typedef STOS_TaskFlags_t       task_flags_t    STOS_DEPRECATED;


typedef STOS_Semaphore_t     semaphore_t     STOS_DEPRECATED;

/* Message Queue management */
typedef STOS_MessageQueue_t    message_queue_t STOS_DEPRECATED;
#endif


/* Linux user Deprecated usage */
#if defined(ST_OSLINUX) && !defined (MODULE)

enum {
     OS21_SUCCESS = 0                 ,
     OS21_FAILURE = -1                
} STOS_DEPRECATED;

typedef STOS_Clock_t           osclock_t       STOS_DEPRECATED;
typedef STOS_Mutex_t           mutex_t         STOS_DEPRECATED;

#ifdef task_flags_no_min_stack_size
#undef task_flags_no_min_stack_size
#endif
#ifdef task_flags_suspended
#undef task_flags_suspended
#endif 
#ifdef task_flags_high_priority_process
#undef task_flags_high_priority_process
#endif 
enum {
    task_flags_no_min_stack_size = 0  ,
    /* always set in OS21 task creation */
    task_flags_suspended =1 ,
    /* not use inside STOS even for OS20 nor OS21 */
    task_flags_high_priority_process = 2   
    /* not use inside STOS even for OS20 */
} STOS_DEPRECATED;
enum
{
    task_context_task      = STOS_TaskContext_Task      ,
    task_context_interrupt = STOS_TaskContext_Interrupt
} STOS_DEPRECATED;
typedef STOS_TaskContext_t    task_context_t STOS_DEPRECATED;
typedef STOS_Task_t           task_t         STOS_DEPRECATED;
typedef STOS_TaskDesc_t       tdesc_t        STOS_DEPRECATED;
typedef STOS_TaskFlags_t      task_flags_t   STOS_DEPRECATED;


typedef STOS_Semaphore_t     semaphore_t     STOS_DEPRECATED;

/* Message Queue management */
typedef STOS_MessageQueue_t    message_queue_t STOS_DEPRECATED;
#endif


/* WinCE Deprecated usage */
#ifdef ST_OSWINCE
typedef STOS_Clock_t           osclock_t       STOS_DEPRECATED;
typedef STOS_Mutex_t           mutex_t         STOS_DEPRECATED;

typedef STOS_Semaphore_t       semaphore_t     STOS_DEPRECATED;
typedef STOS_Task_t            task_t          STOS_DEPRECATED;

/* Message Queue management */
typedef STOS_MessageQueue_t    message_queue_t STOS_DEPRECATED;
#endif



#ifdef  ST_OSLINUX 
/* now deprecated. */
/* Memory Management */
void*   memory_allocate(STOS_Partition_t *part, size_t size) STOS_DEPRECATED;
void    memory_deallocate(STOS_Partition_t *part, void* block) STOS_DEPRECATED;
void*   memory_reallocate (STOS_Partition_t* Partition, void* Block, size_t Requested, size_t OldSize) STOS_DEPRECATED;
void*   memory_allocate_clear(STOS_Partition_t* Partition, size_t nelem, size_t elsize) STOS_DEPRECATED;

/* Mutex Management */
/* mutex recursive support */


STOS_Mutex_t * mutex_create_fifo(void) STOS_DEPRECATED;
int mutex_lockit(STOS_Mutex_t *mutex) STOS_DEPRECATED;
#undef mutex_release
int mutex_release(STOS_Mutex_t *mutex) STOS_DEPRECATED;
int mutex_delete(STOS_Mutex_t *mutex) STOS_DEPRECATED;
#define mutex_create_priority() mutex_create_fifo()


/* Semaphores functions */
#if defined SEMAPHORE_DEBUG
int semaphore_init_fifo_fileline(STOS_Semaphore_t *sem, int count, char * file, U32 line) STOS_DEPRECATED;
STOS_Semaphore_t * semaphore_create_fifo_timeout_fileline(int count, char * file, U32 line) STOS_DEPRECATED;
#define semaphore_init_fifo(sem, count)         semaphore_init_fifo_fileline(sem, count, __FILE__, __LINE__)
#define semaphore_create_fifo_timeout(count)    semaphore_create_fifo_timeout_fileline(count, __FILE__, __LINE__)
#else
int semaphore_init_fifo(STOS_Semaphore_t *sem, int count) STOS_DEPRECATED;
STOS_Semaphore_t * semaphore_create_fifo_timeout(int count) STOS_DEPRECATED;
#endif  /* SEM_DEBUG */

int semaphore_wait(STOS_Semaphore_t *sem) STOS_DEPRECATED;
int semaphore_signal(STOS_Semaphore_t *sem) STOS_DEPRECATED;
int semaphore_wait_timeout(STOS_Semaphore_t *sem, STOS_Clock_t *timeout) STOS_DEPRECATED;
int semaphore_delete (STOS_Semaphore_t* Semaphore) STOS_DEPRECATED;

/* These are not aliases because creating an alias to a function
 * means it won't be inlined, just localized. */
#define semaphore_init_fifo_timeout(s,c)    semaphore_init_fifo(s,c)
#define semaphore_init_priority(s,c)        semaphore_init_fifo(s,c)
#define semaphore_create_fifo(c)            semaphore_create_fifo_timeout(c)
#define semaphore_create_priority(c)        semaphore_create_fifo_timeout(c)



/* Messages functions */
void message_init_queue_timeout(STOS_MessageQueue_t* MessageQueue,
                                void* memory, size_t ElementSize,
                                unsigned int NoElements) /*STOS_DEPRECATED*/;
void message_init_queue(STOS_MessageQueue_t* MessageQueue,
                        void* memory, size_t ElementSize,
                        unsigned int NoElements) /*STOS_DEPRECATED*/;
STOS_MessageQueue_t * message_create_queue_timeout(size_t ElementSize, unsigned int NoElements) STOS_DEPRECATED;
int message_delete_queue(STOS_MessageQueue_t* MessageQueue) STOS_DEPRECATED;
void* message_claim_timeout(STOS_MessageQueue_t *queue, STOS_Clock_t * time_end_p) STOS_DEPRECATED;
void* message_claim(STOS_MessageQueue_t * MessageQueue) STOS_DEPRECATED;
void message_release (STOS_MessageQueue_t* MessageQueue, void* Message) STOS_DEPRECATED;
void message_send(STOS_MessageQueue_t *queue, void *message) STOS_DEPRECATED;
void *message_receive (STOS_MessageQueue_t* MessageQueue) STOS_DEPRECATED;
void *message_receive_timeout (STOS_MessageQueue_t* MessageQueue, STOS_Clock_t * ticks) STOS_DEPRECATED;


/* Clock functions */
STOS_Clock_t      time_now (void) STOS_DEPRECATED ;
STOS_Clock_t      time_minus (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
STOS_Clock_t      time_plus (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
int          time_after_STAPI (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
#ifndef MODULE
int          time_after (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
#endif
int task_delay(int ticks) STOS_DEPRECATED ;
int task_delay_until(STOS_Clock_t timeout) STOS_DEPRECATED ;

/* Context functions */
task_context_t task_context(STOS_Task_t **task, int* level) STOS_DEPRECATED;

#ifdef MODULE
void interrupt_lock(void) STOS_DEPRECATED;
void interrupt_unlock(void) STOS_DEPRECATED;
#endif 

#ifndef MODULE
void task_lock(void) STOS_DEPRECATED;
void task_unlock(void) STOS_DEPRECATED;
struct timeval  Clockt2Timeval(STOS_Clock_t clock) ; 
STOS_Clock_t         get_time_convert(struct timeval *tv) STOS_DEPRECATED; 
STOS_Clock_t		 get_time_now (void) STOS_DEPRECATED;
#endif  /* !MODULE */


/* STSDK - FAE/STAPIREF_COMPAT - START - To get event time in kernel */
#ifdef MODULE
#ifdef HND_SDK_MIGRATION
U32		       get_time (void);   /* Please use STLINUX_TimeNowUser() */
#endif
#endif
/* STSDK - FAE/STAPIREF_COMPAT - STOP  - To get event time in kernel */


#ifndef MODULE
/* defining debug functions */
/*please use directly the need function, debugopen or ask for a STOS_sepcific function */
static __inline int STOS_DEPRECATED debugopen (char* x, char*y) { return fileno(fopen(x,y)) ; }
static __inline int STOS_DEPRECATED debugread (int x, void* y, int z) { return read(x,y,z); }
static __inline int STOS_DEPRECATED debugwrite(int x, void* y, int z) {return write(x,y,z); }
static __inline int STOS_DEPRECATED debugclose(int x) {return close(x); }
#endif

#endif /* ST_OSLINUX */

/* End OS21 compatibility symbol ]]]*/
#endif /* OS21_SYMBOL_COMPAT */



#ifdef STOS_TIME_SYMBOL_COMPAT
STOS_Clock_t      STOS_time_now (void) STOS_DEPRECATED ;
STOS_Clock_t      STOS_time_minus (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
STOS_Clock_t      STOS_time_plus (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
int               STOS_time_after (STOS_Clock_t Time1, STOS_Clock_t Time2) STOS_DEPRECATED ;
#endif

/* C++ support */
#if defined __cplusplus
}
#endif


#endif /* #if !defined __STOS_H */

/* End of stos.h */



