/*
** $Id: dmg-stb.h 7335 2007-08-16 03:38:27Z xgwang $
**
** tf-stb.h:. the head file of Intel DMG GEN10 STB IAL engine.
**
** Copyright (C) 2003 ~ 2007, Feynman Software.
**
** Created by Wei Yongming, 2003/12/14
*/

#ifndef GUI_IAL_DMGSTB_H
    #define GUI_IAL_DMGSTB_H

#define DMG_IR_BACK         0x0004

#define DMG_IR_UP           0x0010
#define DMG_IR_DOWN         0x0020
#define DMG_IR_LEFT         0x0040
#define DMG_IR_RIGHT        0x0080

#define DMG_IR_PLAY         0x0100
#define DMG_IR_STOP         0x0200

#define DMG_IR_ENTER        0x4000

#define DMG_KB_UP           SCANCODE_CURSORBLOCKUP      /* ,< */
#define DMG_KB_DOWN         SCANCODE_CURSORBLOCKDOWN    /* .> */
#define DMG_KB_LEFT         SCANCODE_CURSORBLOCKLEFT    /* [{ */
#define DMG_KB_RIGHT        SCANCODE_CURSORBLOCKRIGHT   /* ]} */
#define DMG_KB_ENTER        SCANCODE_ENTER              /* Enter */

#define DMG_KB_STOP         SCANCODE_Q              /* 'q' */
#define DMG_KB_PLAY         SCANCODE_P              /* 'p' */
#define DMG_KB_BACK         SCANCODE_R              /* 'r' */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDMGSTBInput (INPUT* input, const char* mdev, const char* mtype);
void    TermDMGSTBInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DMGSTB_H */


