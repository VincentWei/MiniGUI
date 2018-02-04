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
 @File   stevt_ioctl.h                                                         
 @brief                                                                         
                                                                               
                                                                               
                                                                               
*/
#ifndef STEVT_IOCTL_H
#define STEVT_IOCTL_H

#include <linux/ioctl.h>   /* Defines macros for ioctl numbers */
/* Include to have magic number declaration */
#include "linuxcommon.h"
#include "stevt.h"

/* This defines the event block that is returned to the read thread containing the event info. */

/* [TotalDataSize][Reason][Event][SubscribeParams][[RegistrantName][EventData]] */

/* The read thread reads the total size allocates memory for the rest of the data and reads it.
   The callback is called and the memory freed.
*/

#define STEVT_EVTREADBLK_CTRL_NOTDEVCB 0x00000001  /* Bit to indicate what callback to call. */
#define STEVT_EVTREADBLK_CTRL_REGNAME  0x00000002  /* Null registrant name */
#define STEVT_EVTREADBLK_CTRL_CBACK    0x00000004  /* The CB needs to be acknowledeged  */

typedef struct STEVT_EventReadBlock_s{
    STEVT_CallReason_t Reason;
    STEVT_EventConstant_t Event;
    STEVT_DeviceSubscribeParams_t SubscribeParams;
    U32 Control;  /* Control Bits see above. */
    U32 SubCompletion;
    U8 Data[]; /* The event data is preceeded by the (16 byte)registrant if STEVT_EVTREADBLK_CTRL_REGNAME set in control */

}STEVT_EventReadBlock_t;

/*** IOCtl defines ***/
#if !defined(STEVT_IOCTL_MAGIC_NUMBER)
#define STEVT_IOCTL_MAGIC_NUMBER 0xFF
#endif

                                                                                                 
typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t          ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STEVT_InitParams_t      InitParams;
} STEVT_Ioctl_Init_t;

#define STEVT_IOC_INIT                    _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 0, STEVT_Ioctl_Init_t*)

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STEVT_TermParams_t      TermParams;
} STEVT_Ioctl_Term_t;

#define STEVT_IOC_TERM               _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 1, STEVT_Ioctl_Term_t *)

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    ST_DeviceName_t         DeviceName;
    STEVT_OpenParams_t      OpenParams;
    STEVT_Handle_t          Handle;
} STEVT_Ioctl_Open_t;

#define STEVT_IOC_OPEN               _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 2, STEVT_Ioctl_Open_t *)

typedef struct
{
    /* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;

    /* Parameters to the function */
    STEVT_Handle_t         Handle;
} STEVT_Ioctl_Close_t;

#define STEVT_IOC_CLOSE              _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 3, STEVT_Ioctl_Close_t *)

/*
ST_ErrorCode_t STEVT_NotifyWithSize (STEVT_Handle_t Handle,
                             STEVT_EventID_t EventID,
                             const void *EventData,
                             U32 DataSize)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventID_t EventID;       /* In */
    U32 DataSize;                  /* In. Size of data pointed to by EventData */
/* Error code retrieved by STAPI function */
    ST_ErrorCode_t  ErrorCode;
    U8 EventData[];                /* In */
} STEVT_Ioctl_NotifyWithSize_t;

#define STEVT_IOC_NOTIFYWITHSIZE _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 4, STEVT_Ioctl_NotifyWithSize_t *)

/*
ST_ErrorCode_t STEVT_NotifySubscriberWithSize (STEVT_Handle_t Handle,
                                       STEVT_EventID_t EventID,
                                       const void *EventData,
                                       STEVT_SubscriberID_t SubscriberID,
                                       U32 DataSize)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventID_t EventID;       /* In */
    U32 DataSize;                  /* In. Size of data pointed to by EventData */
/* Error code retrieved by STAPI function */
    STEVT_SubscriberID_t SubscriberID;
    ST_ErrorCode_t  ErrorCode;
    U8 EventData[];                /* In */
} STEVT_Ioctl_NotifySubscriberWithSize_t;

#define STEVT_IOC_NOTIFYSUBSCRIBERWITHSIZE _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 5, STEVT_Ioctl_NotifySubscriberWithSize_t *)

/*
ST_ErrorCode_t STEVT_Register (STEVT_Handle_t        Handle,
                               STEVT_EventConstant_t EventConst,
                               STEVT_EventID_t      *EventID)
*/    
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventConstant_t EventConst; /* In */
    const char		 *EventName;		/* In */
    short		  EventNameSize;	/* In */
    STEVT_EventID_t EventID;       /* Out */

    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_Register_t;

#define STEVT_IOC_REGISTER _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 6, STEVT_Ioctl_Register_t *)
/*
ST_ErrorCode_t STEVT_RegisterDeviceEvent  (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t EventConst,
                                           STEVT_EventID_t *EventID)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    ST_DeviceName_t RegistrantName; /* In */
    STEVT_EventConstant_t EventConst; /* In */
    const char		 *EventName;		/* In */
    short		  EventNameSize;	/* In */
    STEVT_EventID_t EventID;       /* Out */

    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_RegisterDeviceEvent_t;

#define STEVT_IOC_REGISTERDEVICEEVENT _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 7, STEVT_Ioctl_RegisterDeviceEvent_t *)

/*
ST_ErrorCode_t STEVT_Unregister (STEVT_Handle_t Handle,
                                 STEVT_EventConstant_t Event)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventConstant_t EventConst; /* In */

    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_Unregister_t;

#define STEVT_IOC_UNREGISTER _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 8, STEVT_Ioctl_Unregister_t *)

/*
ST_ErrorCode_t STEVT_UnregisterDeviceEvent (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t EventConst)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    ST_DeviceName_t RegistrantName; /* In */
    STEVT_EventConstant_t EventConst; /* In */

    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_UnregisterDeviceEvent_t;

#define STEVT_IOC_UNREGISTERDEVICEEVENT _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 9, STEVT_Ioctl_UnregisterDeviceEvent_t *)

/*
ST_ErrorCode_t STEVT_Subscribe (STEVT_Handle_t Handle,
                                STEVT_EventConstant_t EventConst,
                                const STEVT_SubscribeParams_t *SubscribeParams)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventConstant_t EventConst; /* In */
    STEVT_DeviceSubscribeParams_t SubscribeParams; /* In */
    BOOL CBAck; /* In. The subscriber wants the CB thread to ACK and the Notify to wait until the ACK is recieved */ 
    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_Subscribe_t;

#define STEVT_IOC_SUBSCRIBE _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 10, STEVT_Ioctl_Subscribe_t *)

/*
ST_ErrorCode_t STEVT_SubscribeDeviceEvent (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t EventConst,
                                           const STEVT_DeviceSubscribeParams_t *SubscribeParams)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    ST_DeviceName_t RegistrantName; /* In */
    STEVT_EventConstant_t EventConst; /* In */
    STEVT_DeviceSubscribeParams_t SubscribeParams; /* In */
    BOOL CBAck; /* In. The subscriber wants the CB thread to ACK and the Notify to wait until the ACK is recieved */ 
    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_SubscribeDeviceEvent_t;

#define STEVT_IOC_SUBSCRIBEDEVICEEVENT _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 11, STEVT_Ioctl_SubscribeDeviceEvent_t *)

/*
ST_ErrorCode_t STEVT_Unsubscribe (STEVT_Handle_t Handle,
                                  STEVT_EventConstant_t EventConst)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_EventConstant_t EventConst; /* In */
    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_Unsubscribe_t;

#define STEVT_IOC_UNSUBSCRIBE _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 12, STEVT_Ioctl_Unsubscribe_t *)

/*
ST_ErrorCode_t STEVT_UnsubscribeDeviceEvent (STEVT_Handle_t Handle,
                                             const ST_DeviceName_t RegistrantName,
                                             STEVT_EventConstant_t EventConst)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    ST_DeviceName_t RegistrantName; /* In */
    STEVT_EventConstant_t EventConst;  /* In */
    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_UnsubscribeDeviceEvent_t;

#define STEVT_IOC_UNSUBSCRIBEDEVICEEVENT _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 13, STEVT_Ioctl_UnsubscribeDeviceEvent_t *)
/*
ST_ErrorCode_t STEVT_GetSubscriberID (STEVT_Handle_t Handle,
                                      STEVT_SubscriberID_t *SubscriberID)
*/
typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    STEVT_SubscriberID_t SubscriberID; /* In */
    ST_ErrorCode_t  ErrorCode;     /* Out */
} STEVT_Ioctl_GetSubscriberID_t;

#define STEVT_IOC_GETSUBSCRIBERID _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 14, STEVT_Ioctl_GetSubscriberID_t *)

/*
ST_Revision_t STEVT_GetRevision(void)
*/
#define STEVT_IOC_GETREVISION _IOWR( STEVT_IOCTL_MAGIC_NUMBER, 15, char* )

/*
U32 STEVT_GetAllocatedMemory(STEVT_Handle_t Handle)
*/

typedef struct
{
    /* Parameters to the function */
    STEVT_Handle_t         Handle; /* In */
    U32 AllocateMemory;            /* Out */
} STEVT_Ioctl_GetAllocatedMemory_t;

#define STEVT_IOC_GETALLOCATEDMEMORY _IOWR(STEVT_IOCTL_MAGIC_NUMBER, 16, STEVT_Ioctl_GetAllocatedMemory_t *)

/* Terminate the reading task */
#define STEVT_IOC_TERMREAD _IOW(STEVT_IOCTL_MAGIC_NUMBER, 17, void*)

/* Allows Notify to return */
#define STEVT_IOC_CALLBACK_RET _IOW(STEVT_IOCTL_MAGIC_NUMBER, 18, U32* )

#define STEVT_IOC_SETBUFFERSIZE _IOW(STEVT_IOCTL_MAGIC_NUMBER, 19, U32* )

#endif
