//
// $Id: winial.h 12488 2010-01-25 08:40:59Z huzhaolin $
//
// winial.h: head file of Windows IAL
//
// Copyright (C) 2004 ~ 2007, Feynman Software 
//

#ifndef _IAL_WINIAL_H
#define _IAL_WINIAL_H

typedef struct _WVFbKeyData
{
    unsigned short keycode;
    unsigned short keystate;
    BYTE press;
    BYTE repeat;
} WVFbKeyData;

typedef struct _WVFbMouseData
{
    unsigned short x;
    unsigned short y;
    unsigned short btn;
    unsigned short pad;
} WVFbMouseData;

typedef struct _WVFbEventData
{
    int event_type;
    union {
        WVFbKeyData kbdata;
        WVFbMouseData mousedata;
    };
} WVFbEventData;

#endif /* _IAL_WINIAL_H */

