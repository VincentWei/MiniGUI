/*
** $Id: clipboard.h 11102 2008-10-23 01:58:25Z tangjianbin $
**
** clipboard.h: the internal clipboard APIs.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Create Date: 2003/12/xx
*/

#ifndef MG_CLIPBOARD
#define MG_CLIPBOARD
#ifdef _MGHAVE_CLIPBOARD
#define CLIPBOARD_BUFFLENGTH	1024

#ifdef _cplusplus
extern "C" {
#endif /* _cplusplus */

void mg_InitClipBoard(void);
void mg_TerminateClipBoard(void);

#define CBREQ_CREATE        1
#define CBREQ_DESTROY       2
#define CBREQ_SETDATA       3
#define CBREQ_GETDATALEN    4
#define CBREQ_GETDATA       5
#define CBREQ_GETBYTE       6

typedef struct _REQ_CLIPBOARD
{
    int id;                                 /* request id */
    char name [LEN_CLIPBOARD_NAME + 1];     /* the name of the clipboard involved */
    int cbop;                               /* clipboard operation types */

    //int ret;                                /* return value of the request */
    //void *data;                             /* buffer of attached or returned data */
    size_t len_data;                        /* length of the attached data. */

} REQ_CLIPBOARD;


#ifdef _cplusplus
}
#endif /* _cplusplus */

#endif
#endif
