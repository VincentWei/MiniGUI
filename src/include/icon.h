/*
** $Id: icon.h 8944 2007-12-29 08:29:16Z xwyan $
**
** icon.h: the head file of Icon Support Lib.
**
** Copyright (c) 2003 ~ 2007 Feynman Software.
** Copyright (c) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/06
*/

#ifndef GUI_ICON_H
    #define GUI_ICON_H

/* Struct definitions */
typedef struct _ICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
}ICONDIRENTRY;

#define SIZEOF_ICONDIRENTRY 16
typedef struct _ICONDIR {
    WORD cdReserved;
    WORD cdType;    // must be 1.
    WORD cdCount;
}ICONDIR;

typedef struct _ICON {
    Uint32  width;
    Uint32  height;
    Uint32  pitch;
    BYTE*   AndBits;
    BYTE*   XorBits;
}ICON;

typedef ICON* PICON;

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Initialization and termination. */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_ICON_H

