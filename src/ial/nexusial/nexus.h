/*
** $Id$
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
**
** Created by ChenLei, 2009/01/16
*/

#ifndef GUI_IAL_NEXUS_H
    #define GUI_IAL_NEXUS_H

/* report key value macro define */
#define RC_UP		0x9034
#define RC_DOWN		0x8035
#define RC_LEFT		0x7036
#define RC_RIGHT	0x6037
#define RC_OK		0xE011

#define RC_0		0x0000
#define RC_1		0xF001
#define RC_2		0xE002
#define RC_3		0xD003
#define RC_4		0xC004
#define RC_5		0xB005
#define RC_6		0xA006
#define RC_7		0x9007
#define RC_8		0x8008
#define RC_9		0x7009

#define RC_CHANNEL_UP			0x500B
#define RC_CHANNEL_DOWN			0x400C
#define RC_SELECT				0xE011
#define RC_EXIT					0xD012
#define RC_VOLUME_UP			0x300D
#define RC_VOLUME_DOWN			0x200E

#define RC_ESCAPE	0xD012	

#define RC_MENU         0x6019
#define RC_GUIDE        0xd030


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitNexusInput (INPUT* input);
void    TermNexusInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_NEXUS_H */
