/*
** $Id: cursor.h 12871 2010-05-07 06:13:42Z wanzheng $
**
** cursor.h: the head file of Cursor Support Lib.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/06
*/

#ifndef GUI_CURSOR_H
    #define GUI_CURSOR_H

/* Struct definitions */
typedef struct _CURSORDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wXhotspot;
    WORD wYhotspot;
    DWORD lBytesInRes;
    DWORD dwImageOffset;
}CURSORDIRENTRY;

typedef struct _CURSORDIR {
    WORD cdReserved;
    WORD cdType;	// must be 2.
    WORD cdCount;
}CURSORDIR;

typedef struct _CURSOR {
    int xhotspot;
    int yhotspot;
    int width;
    int height;
    void* AndBits;
    void* XorBits;
}CURSOR;
typedef CURSOR* PCURSOR;

#define CURSORWIDTH	    32
#define CURSORHEIGHT	32
#define MONOSIZE	    (CURSORWIDTH*CURSORHEIGHT/8)
#define MONOPITCH       4

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Initialization and termination. */
BOOL mg_InitCursor (void);
void mg_TerminateCursor(void);

#ifdef _MGHAVE_CURSOR
#ifndef _MGRM_THREADS

/* Use shared memory to emulate MSG_MOUSEMOVE message
 */
#define _MG_CONFIG_FAST_MOUSEMOVE 1

/* show cursor hidden by client GDI function */
void kernel_ReShowCursor (void);
#else
Uint8* GetPixelUnderCursor (int x, int y, gal_pixel* pixel);
#endif

#endif /* _MGHAVE_CURSOR */

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc);

/* Mouse event helper. */
BOOL kernel_RefreshCursor (int* x, int* y, int* button);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_CURSOR_H */
