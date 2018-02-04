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
** pcxvfb.h: head file of Input Engine for PCX Virtual FrameBuffer
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
