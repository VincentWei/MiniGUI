/*
**  $Id: em86gfx.h 7351 2007-08-16 04:55:58Z xgwang $
**  
**  em86gfx.c: NEWGAL driver for EM86xx GFX.
**
**  Copyright (C) 2007 Feynman Software.
*/

#ifndef _GAL_GFXVIDEO_H
#define _GAL_GFXVIDEO_H

#ifndef ALLOW_OS_CODE
#define ALLOW_OS_CODE
#endif 

#include <dcc/include/dcc.h>

#include "sysvideo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define _THIS	GAL_VideoDevice * this

struct GAL_PrivateVideoData {
    struct RUA * pRUA;
    struct DCC * pDCC;
    struct DCCOSDProfile DCC_profile;
    struct DCCVideoSource * pOSD_source;
    RMuint32 OSD_scaler;
    RMuint32 LumaAddr;
    RMuint32 LumaSize;
    RMuint32 ChromaAddr;
    RMuint32 ChromaSize;

    struct GFXEngine_Open_type GFX_profile;
    RMuint32 GFXtarget;
};

#define pRUA            (((struct GAL_PrivateVideoData *)this->hidden)->pRUA)
#define pDCC            (((struct GAL_PrivateVideoData *)this->hidden)->pDCC)
#define DCC_profile     (((struct GAL_PrivateVideoData *)this->hidden)->DCC_profile)
#define pOSD_source     (((struct GAL_PrivateVideoData *)this->hidden)->pOSD_source)
#define OSD_scaler      (((struct GAL_PrivateVideoData *)this->hidden)->OSD_scaler)
#define LumaAddr        (((struct GAL_PrivateVideoData *)this->hidden)->LumaAddr)
#define LumaSize        (((struct GAL_PrivateVideoData *)this->hidden)->LumaSize)
#define ChromaAddr      (((struct GAL_PrivateVideoData *)this->hidden)->ChromaAddr)
#define ChromaSize      (((struct GAL_PrivateVideoData *)this->hidden)->ChromaSize)
#define GFX_profile     (((struct GAL_PrivateVideoData *)this->hidden)->GFX_profile)
#define GFXtarget       (((struct GAL_PrivateVideoData *)this->hidden)->GFXtarget)

#define TIMEOUT_US 1000000

#define WAIT_FOR_COMMANDS
#ifdef WAIT_FOR_COMMANDS
#define SEND_GFX_COMMAND(pRUA, moduleID, propertyID, pValue, ValueSize) \
{ \
	RMstatus err; \
	RMuint32 n = 5; \
	struct RUAEvent evt; \
	evt.ModuleID = moduleID; \
	evt.Mask = RUAEVENT_COMMANDCOMPLETION; \
	do { \
		err = RUASetProperty(pRUA, moduleID, propertyID, pValue, ValueSize, 0); \
		if(err == RM_PENDING) { \
			while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
				RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
		} \
		n--; \
	} while ((n > 0) && (err == RM_PENDING)); \
	if (err != RM_OK) { \
		RMDBGLOG((ENABLE, "Can't send command to command fifo\n" )); \
		return err; \
	} \
	while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
		RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
} \

#else													

#define SEND_GFX_COMMAND(pRUA, moduleID, propertyID, pValue, ValueSize) \
{ \
	RMstatus err; \
	RMuint32 n; \
	n = 5; \
	do { \
		err = RUASetProperty(pRUA, moduleID, propertyID, pValue, ValueSize, 0); \
		if ((err == RM_PENDING)) { \
 			struct RUAEvent evt; \
			evt.ModuleID = moduleID; \
			evt.Mask = RUAEVENT_COMMANDCOMPLETION; \
			while (RUAWaitForMultipleEvents(pRUA, &evt, 1, TIMEOUT_US, NULL) != RM_OK) \
				RMDBGLOG((ENABLE, "Waiting for a command to finish\n")); \
                } \
                        n--; \
	} while((n > 0) && (err == RM_PENDING)); \
	if (err != RM_OK) { \
		RMDBGLOG((ENABLE, "Can't send command to command fifo\n" )); \
		return err; \
	} \
}

#endif /* WAIT_FOR_COMMANDS */


inline void * RMMalloc(RMuint32 size)
{
    return malloc(size);
}

inline void RMFree(void * ptr)
{
    free (ptr);
}

inline void * RMMemset(void * s, RMuint8 c, RMuint32 n)
{
    return memset(s, c, n);
}

inline void * RMMemcpy(void * dest, const void * src, RMuint32 n)
{
    return memcpy(dest, src, n);
}

inline RMint32 RMMemcmp(const void * s1, const void * s2, RMuint32 n)
{
    return memcmp(s1, s2, n);
}

inline void * RMCalloc(RMuint32 nmemb, RMuint32 size)
{
    void * ptr = RMMalloc(nmemb * size);
    if (ptr != NULL) RMMemset(ptr, 0, nmemb * size);
    return ptr;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_GFXVIDEO_H */
