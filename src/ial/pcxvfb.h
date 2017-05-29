/*
** $Id: pcxvfb.h 12488 2010-01-25 08:40:59Z huzhaolin $
**
** pcxvfb.h: head file of Input Engine for PCX Virtual FrameBuffer
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
**
** All rights reserved by Feynman Software.
**
** Created by xgwang, 2007/09/28
*/

#ifndef _IAL_PCXVFB_H
#define _IAL_PCXVFB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MOUSE_TYPE          0
#define KB_TYPE             1
#define CAPTION_TYPE        2
#define IME_TYPE            3
#define IME_MESSAGE_TYPE    4
#define SHOW_HIDE_TYPE      5
#define XVFB_CLOSE_TYPE     6

struct PCXVFbKeyData
{
    unsigned int unicode;
    unsigned int modifiers;
    BYTE press;
    BYTE repeat;
};

typedef struct _XVFBKEYDATA
{
        unsigned short key_code;
		unsigned short key_state;
} XVFBKEYDATA;

typedef struct _XVFBMOUSEDATA
{
        unsigned short x;
        unsigned short y;
        unsigned int   button;
} XVFBMOUSEDATA;

typedef struct _XVFBEVENTDATA
{
        int event_type;
        union 
        {
                XVFBKEYDATA   key;
                XVFBMOUSEDATA mouse;
        } data;
} XVFBEVENT;

typedef struct _XVFBCaptionEventData{
        int event_type;
        int size;
        char buff[0];
}XVFBCaptionEventData;

typedef struct _XVFBIMEEventData{
        int event_type;
        int ime_open;
}XVFBIMEEventData;

typedef struct _XVFBIMEMSGEventData{
    int event_type;
    int size;
    char buff[0];
}XVFBIMEMSGEventData;

typedef struct _XVFBShowHideEventData{
    int event_type;
    int show;           /* 0 to hide, 1 to show */
}XVFBShowHideEventData;

extern int __mg_pcxvfb_server_sockfd;
extern int __mg_pcxvfb_client_sockfd;

BOOL InitPCXVFBInput (INPUT* input, const char* mdev, const char* mtype);
void TermPCXVFBInput (void);

MG_EXPORT void GUIAPI VFBOpenIME(int bOpen);
MG_EXPORT void GUIAPI VFBSetIMETextCallback(void (*setIMEText)(void *, const char* text),
        void *user_data);

MG_EXPORT void GUIAPI VFBSetCaption(const char* caption);
MG_EXPORT void GUIAPI VFBShowWindow(int show);
MG_EXPORT void GUIAPI VFBAtExit(void (*callback)(void));

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_PCXVFB_H */
