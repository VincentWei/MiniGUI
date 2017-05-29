/*
** $Id: iale_custom.h 
**
** iale_custom.h:. the head file of dlcustom engine sample.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
*/

#ifndef IALE_CUSTOM_H
#define IALE_CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDLCustomDummyInput (INPUT* input, const char* mdev, const char* mtype);
void    TermDLCustomDummyInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* IALE_CUSTOM_H*/


