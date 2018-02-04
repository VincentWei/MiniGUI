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
 @File   linuxcommon.h                                                         
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/

#ifndef _LINUX_COMMON_H_
#define _LINUX_COMMON_H_

/* Include */
/* ------- */
#ifdef MODULE
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/init.h>   
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>    
#include <linux/errno.h> 
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/netdevice.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/atomic.h>
#include <asm/irq.h>
#include <asm/param.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#else
#define _GNU_SOURCE 1 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/poll.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#endif

/* Common include */
/* -------------- */
#include "stddefs.h"
#include "stsys.h"

/* Define major number for drivers */
/* ------------------------------- */
#define LXLOAD_IOCTL_MAJOR       0
#define STFASTFILTER_IOCTL_MAJOR 0
#define STAVMEM_IOCTL_MAJOR      0
#define STTUNER_IOCTL_MAJOR      0
#define STMERGE_IOCTL_MAJOR      0
#define STPTI4_IOCTL_MAJOR       0
#define STDEMUX_IOCTL_MAJOR      0
#define STVID_IOCTL_MAJOR        0
#define STAUDLX_IOCTL_MAJOR      0
#define STVTG_IOCTL_MAJOR        0
#define STVOUT_IOCTL_MAJOR       0
#define STHDMI_IOCTL_MAJOR       0
#define STLAYER_IOCTL_MAJOR      0
#define STBLIT_IOCTL_MAJOR       0
#define STFDMA_IOCTL_MAJOR       0
#define STVMIX_IOCTL_MAJOR       0
#define STDENC_IOCTL_MAJOR       0
#define STGXOBJ_IOCTL_MAJOR      0 
#define STGFB_IOCTL_MAJOR        0 
#define STEVT_IOCTL_MAJOR        0
#define STCLKRV_IOCTL_MAJOR      0
#define STSMART_IOCTL_MAJOR      0
#define STVBI_IOCTL_MAJOR        0
#define STCC_IOCTL_MAJOR         0
#define STI2C_IOCTL_MAJOR        0
#define STTTX_IOCTL_MAJOR        0
#define STBLAST_IOCTL_MAJOR      0
#define STCOMMON_IOCTL_MAJOR      0
#define STSUBT_IOCTL_MAJOR       0
#define STPCCRD_IOCTL_MAJOR      0
#define STSYS_IOCTL_MAJOR        0
#define STCRYPT_IOCTL_MAJOR      0
#define GSECHAL_IOCTL_MAJOR      0
#define STTKDMA_IOCTL_MAJOR      0
#define STVIN_IOCTL_MAJOR        0
#define STBUFFER_IOCTL_MAJOR     0
#define STINJECT_IOCTL_MAJOR     0
#define STFRONTEND_IOCTL_MAJOR   0
#define STTUNER_CORE_MAJOR       0
#define STMERGE_CORE_MAJOR       0
#define STPTI4_CORE_MAJOR        0
#define STDEMUX_CORE_MAJOR       0
#define STVID_CORE_MAJOR         0
#define STAUDLX_CORE_MAJOR       0
#define STVTG_CORE_MAJOR         0
#define STVOUT_CORE_MAJOR        0
#define STHDMI_CORE_MAJOR        0
#define STLAYER_CORE_MAJOR       0
#define STBLIT_CORE_MAJOR        0
#define STVMIX_CORE_MAJOR        0
#define STDENC_CORE_MAJOR        0
#define STGXOBJ_CORE_MAJOR       0 
#define STGFB_CORE_MAJOR         0 
#define STEVT_CORE_MAJOR         0
#define STCLKRV_CORE_MAJOR       0
#define STSMART_CORE_MAJOR       0
#define STVBI_CORE_MAJOR         0
#define STCC_CORE_MAJOR          0
#define STI2C_CORE_MAJOR         0
#define STPIO_CORE_MAJOR         0
#define STTTX_CORE_MAJOR         0
#define STBLAST_CORE_MAJOR       0
#define STFDMA_CORE_MAJOR        0
#define STCRYPT_CORE_MAJOR       0
#define GSECHAL_CORE_MAJOR       0
#define STTKDMA_CORE_MAJOR       0
#define STSUBT_CORE_MAJOR        0
#define STPCCRD_CORE_MAJOR       0
#define STVIN_CORE_MAJOR         0
#define STKEYSCN_CORE_MAJOR      0
#define STPCCRD_CORE_MAJOR       0
#define STBUFFER_CORE_MAJOR      0
#define STINJECT_CORE_MAJOR      0
#define STFRONTEND_CORE_MAJOR    0

/* Define list of ioctl magic number */
/* --------------------------------- */
#define STAUDLX_IOCTL_MAGIC_NUMBER      0xC0
#define STAVMEM_IOCTL_MAGIC_NUMBER      0xC1
#define STBLAST_IOCTL_MAGIC_NUMBER      0xC2
#define STBLIT_IOCTL_MAGIC_NUMBER       0xC3
#define STCC_IOCTL_MAGIC_NUMBER         0xC4
#define STCLKRV_IOCTL_MAGIC_NUMBER      0xC5
#define STCOMMON_IOCTL_MAGIC_NUMBER     0xC6
#define STCRYPT_IOCTL_MAGIC_NUMBER      0xC7
#define STDENC_IOCTL_MAGIC_NUMBER       0xC8
#define STEVT_IOCTL_MAGIC_NUMBER        0xC9
#define STFASTFILTER_IOCTL_MAGIC_NUMBER 0xCA
#define STFDMA_IOCTL_MAGIC_NUMBER       0xCB
#define STGFB_IOCTL_MAGIC_NUMBER        0xCC
#define STHDMI_IOCTL_MAGIC_NUMBER       0xCD
#define STI2C_IOCTL_MAGIC_NUMBER        0xCE
#define STINJECT_IOCTL_MAGIC_NUMBER     0xCF
#define STLAYER_IOCTL_MAGIC_NUMBER      0xD0
#define STMERGE_IOCTL_MAGIC_NUMBER      0xD1
#define STNET_IOCTL_MAGIC_NUMBER        0xD2
#define STPCPD_IOCTL_MAGIC_NUMBER       0xD3
#define STPIO_IOCTL_MAGIC_NUMBER        0xD4
#define STPOD_IOCTL_MAGIC_NUMBER        0xD5
#define STPTI_IOCTL_MAGIC_NUMBER        0xD6
#define STPWM_IOCTL_MAGIC_NUMBER        0xD7
#define STSMART_IOCTL_MAGIC_NUMBER      0xD8
#define STSPI_IOCTL_MAGIC_NUMBER        0xD9
#define STSYS_IOCTL_MAGIC_NUMBER        0xDA
#define STTTX_IOCTL_MAGIC_NUMBER        0xDB
#define STTUNER_IOCTL_MAGIC_NUMBER      0xDC
#define STVBI_IOCTL_MAGIC_NUMBER        0xDD
#define STVID_IOCTL_MAGIC_NUMBER        0xDE
#define STVIN_IOCTL_MAGIC_NUMBER        0xDF
#define STVMIX_IOCTL_MAGIC_NUMBER       0xE0
#define STVOUT_IOCTL_MAGIC_NUMBER       0xE1
#define STVTG_IOCTL_MAGIC_NUMBER        0xE2
#define STSUBT_IOCTL_MAGIC_NUMBER       0xE3
#define STKEYSCN_IOCTL_MAGIC_NUMBER     0xE4
#define STGXOBJ_IOCTL_MAGIC_NUMBER      0xE5
#define STBUFFER_IOCTL_MAGIC_NUMBER     0xE6
#define STPOWER_IOCTL_MAGIC_NUMBER      0xE7
#define STFSK_IOCTL_MAGIC_NUMBER        0xE8
#define STPCCRD_IOCTL_MAGIC_NUMBER      0xE9
#define STFRONTEND_IOCTL_MAGIC_NUMBER   0xEA
#define STUART_IOCTL_MAGIC_NUMBER       0xEB 

#endif

