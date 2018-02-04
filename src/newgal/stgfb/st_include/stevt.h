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
 @File   stevt.h                                                               
 @brief                                                                         
   Exported types and function for Event Handler driver                                                                               
                                                                               
                                                                               
*/
#ifndef __STEVT_H
#define __STEVT_H

#include "stddefs.h"
#include "stos.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------- */
/* --- Defines for Event Handler Driver --- */
/* ---------------------------------------- */

/* --- Driver identity number --- */

#define STEVT_DRIVER_ID    18
#define STEVT_DRIVER_BASE               (STEVT_DRIVER_ID << 16)


typedef enum STEVT_MemSizeFlag_e
{
    STEVT_UNKNOWN_SIZE = 0,
    STEVT_USER_DEFINED =  (STEVT_DRIVER_BASE + STEVT_DRIVER_ID) /* 0x00120012 */
} STEVT_MemSizeFlag_t;


/* --------------------------------------------------- */
/* --- Defines structures for Event Handler Driver --- */
/* --------------------------------------------------- */

/* --- Define the reason for the invocation of callback function --- */

typedef enum STEVT_CallReason_e
{
    CALL_REASON_NOTIFY_CALL
} STEVT_CallReason_t;

/* --- Define the Event Constant --- */

typedef U32 STEVT_EventConstant_t;

/* --- Define the EventID --- */

typedef U32 STEVT_EventID_t;

/* --- Define the Subscriber identifier --- */

typedef U32 STEVT_SubscriberID_t;

/* --- Define the handler of an open EH --- */

typedef U32 STEVT_Handle_t;


/* --- Define the initialization parameters --- */

typedef struct STEVT_InitParams_s
{
    U32 EventMaxNum;
    U32 ConnectMaxNum;
    U32 SubscrMaxNum;
    ST_Partition_t *MemoryPartition;
    STEVT_MemSizeFlag_t MemorySizeFlag;
    U32 MemoryPoolSize;
} STEVT_InitParams_t;

/* --- Define the termination parameters --- */

typedef struct STEVT_TermParams_s
{
    BOOL ForceTerminate;
} STEVT_TermParams_t;

/* --- Define the open parameters --- */

typedef struct STEVT_OpenParams_s
{
     /* TBD */
    int dummy;
} STEVT_OpenParams_t;

/* --- Define the get capability parameters --- */

typedef struct STEVT_CapabilityParams_s
{
    /* TBD */
    int dummy;
} STEVT_CapabilityParams_t;

/* --- Define the callback functions --- */

typedef void (*STEVT_CallbackProc_t)(
    STEVT_CallReason_t Reason,
    STEVT_EventConstant_t Event,
    const void *EventData
);

typedef void (*STEVT_DeviceCallbackProc_t)(
    STEVT_CallReason_t Reason,
    const ST_DeviceName_t RegistrantName,
    STEVT_EventConstant_t Event,
    const void *EventData,
    const void *SubscriberData_p
);

#if defined(ST_OSLINUX) && defined(MODULE)
/* Only to be called from Kernel space. */
/* When the size is requested using STEVT_SubscribeDeviceEventReqSize()
   then this function def is called as callback
*/

typedef void (*STEVT_DeviceCallbackProcWithSize_t)(
    STEVT_CallReason_t Reason,
    const ST_DeviceName_t RegistrantName,
    STEVT_EventConstant_t Event,
    const void *EventData,
    const void *SubscriberData_p,
    U32 EventDataSize
);

#endif

/* --- Specify the functions to be called on event notify,
       register, unregister --- */

typedef struct STEVT_SubscribeParams_s
{
    STEVT_CallbackProc_t NotifyCallback;
} STEVT_SubscribeParams_t;

typedef struct STEVT_DeviceSubscribeParams_s
{
    STEVT_DeviceCallbackProc_t NotifyCallback;
    void *SubscriberData_p;
} STEVT_DeviceSubscribeParams_t;

#if defined(ST_OSLINUX) && defined(MODULE)
typedef struct STEVT_DeviceSubscribeParamsWithSize_s
{
    STEVT_DeviceCallbackProcWithSize_t NotifyCallback;
    void *SubscriberData_p;
} STEVT_DeviceSubscribeParamsWithSize_t;
#endif

/* ------------------------------------------- */
/* --- Define driver specific errors codes --- */
/* ------------------------------------------- */


/* --- Errors --- */

enum { STEVT_ERROR_INVALID_EVENT_ID = (STEVT_DRIVER_BASE + 1),
       STEVT_ERROR_INVALID_SUBSCRIBER_ID,
       STEVT_ERROR_ALREADY_SUBSCRIBED,
       STEVT_ERROR_ALREADY_REGISTERED,
       STEVT_ERROR_NO_MORE_SPACE,
       STEVT_ERROR_INVALID_EVENT_NAME,
       STEVT_ERROR_ALREADY_UNREGISTERED,
       STEVT_ERROR_MISSING_NOTIFY_CALLBACK,
       STEVT_ERROR_NOT_SUBSCRIBED,
       STEVT_ERROR_INTERRUPT_HANDLER
     };

/* -------------------------------------------- */
/* --- API function of Event Handler Driver --- */
/* -------------------------------------------- */

/* --- Init/Term the EH Driver --- */

ST_ErrorCode_t STEVT_Init (const ST_DeviceName_t EventHandlerName,
                           const STEVT_InitParams_t *InitParams);

ST_ErrorCode_t STEVT_Term (const ST_DeviceName_t EventHandlerName,
                           const STEVT_TermParams_t *TermParams);

/* --- Open/Close the EH Driver --- */

ST_ErrorCode_t STEVT_Open (const ST_DeviceName_t EventHandlerName,
                           const STEVT_OpenParams_t *OpenParams,
                           STEVT_Handle_t *Handle);

ST_ErrorCode_t STEVT_Close (STEVT_Handle_t Handle);

/* --- Get functions --- */

ST_ErrorCode_t STEVT_GetCapability (const ST_DeviceName_t EventHandlerName,
                                    STEVT_CapabilityParams_t *CapabilityParams);

ST_Revision_t STEVT_GetRevision (void);

U32 STEVT_GetAllocatedMemory(STEVT_Handle_t Handle);

/* --- Registrant functions --- */

/* The internal functions have the Event name as well, to gather
   statistics. It is the string version of whatever the developer used
   (hopefully a #define or enum value).
 */
ST_ErrorCode_t STEVT_RegisterInt (STEVT_Handle_t Handle,
                               STEVT_EventConstant_t Event,
                               STEVT_EventID_t *EventID,
			        const char *EventName);


#define STEVT_Register(h,e,i)	STEVT_RegisterInt(h,e,i, #e)

ST_ErrorCode_t STEVT_RegisterDeviceEventInt  (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t Event,
                                           STEVT_EventID_t *EventID,
					    const char *EventName);
#define STEVT_RegisterDeviceEvent(h,n,e,i)	\
		STEVT_RegisterDeviceEventInt(h,n,e,i,#e)

ST_ErrorCode_t STEVT_Unregister (STEVT_Handle_t Handle,
                                 STEVT_EventConstant_t Event);

ST_ErrorCode_t STEVT_UnregisterDeviceEvent (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t Event);


#if defined( ST_OSLINUX )
ST_ErrorCode_t STEVT_NotifyWithSize (STEVT_Handle_t Handle,
                                     STEVT_EventID_t EventID,
                                     const void *EventData,
                                     U32 DataSize);

#define  STEVT_Notify(h,e,dp) STEVT_NotifyWithSize((h),(e),(dp), (dp != NULL)?sizeof(*dp):0)

ST_ErrorCode_t STEVT_NotifySubscriberWithSize (STEVT_Handle_t Handle,
                                       STEVT_EventID_t EventID,
                                       const void *EventData,
                                       STEVT_SubscriberID_t SubscriberID,
                                       U32 DataSize);

#define  STEVT_NotifySubscriber(h,e,dp,s) STEVT_NotifySubscriberWithSize(h,e,dp,s,(dp != NULL) ?sizeof(*dp):0)

#else

ST_ErrorCode_t STEVT_Notify (STEVT_Handle_t Handle,
                             STEVT_EventID_t EventID,
                             const void *EventData);

ST_ErrorCode_t STEVT_NotifySubscriber (STEVT_Handle_t Handle,
                                       STEVT_EventID_t EventID,
                                       const void *EventData,
                                       STEVT_SubscriberID_t SubscriberID);

#endif

#ifdef ST_OSLINUX
#define STEVT_EVENT_DATA_TYPE_CAST
#else
#define STEVT_EVENT_DATA_TYPE_CAST (const void *)
#endif

/* --- Suscriber functions --- */


ST_ErrorCode_t STEVT_GetSubscriberID (STEVT_Handle_t Handle,
                                      STEVT_SubscriberID_t *SubscriberID);

ST_ErrorCode_t STEVT_Subscribe (STEVT_Handle_t Handle,
                                STEVT_EventConstant_t Event,
                                const STEVT_SubscribeParams_t *SubscribeParams);

#if defined (ST_OSLINUX) && defined(MODULE)
/* This function duplicates STEVT_Subscribe() and instructs evt to
   call the "WithSize" callback with the size of the event data.
   This is required for the linux stevt ioctl driver.

   This function should only be called from kernel space.
*/

ST_ErrorCode_t STEVT_SubscribeDeviceEventWithSize(STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t Event,
                                           const STEVT_DeviceSubscribeParamsWithSize_t *SubscribeParams);
#endif

#if defined (ST_OSLINUX) && !defined(MODULE)
ST_ErrorCode_t STEVT_SetBufferSize(U32 Size);

/* Enable: TRUE or disable: FALSE the acknowldegement of CB function return */
void STEVT_CBAck(BOOL state);
#endif

ST_ErrorCode_t STEVT_SubscribeDeviceEvent (STEVT_Handle_t Handle,
                                           const ST_DeviceName_t RegistrantName,
                                           STEVT_EventConstant_t Event,
                                           const STEVT_DeviceSubscribeParams_t *SubscribeParams);

ST_ErrorCode_t STEVT_Unsubscribe (STEVT_Handle_t Handle,
                                  STEVT_EventConstant_t Event);

ST_ErrorCode_t STEVT_UnsubscribeDeviceEvent (STEVT_Handle_t Handle,
                                             const ST_DeviceName_t RegistrantName,
                                             STEVT_EventConstant_t Event);


#ifdef __cplusplus
}
#endif

#endif   /* #ifndef __STEVT_H */
