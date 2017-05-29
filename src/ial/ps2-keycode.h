/*
** $Id: ps2-keycode.h 7335 2007-08-16 03:38:27Z xgwang $
**
** ps2-keycode.h: the head file for IAL Engine of EM85xx Front Panel and Remote Controller
**
** Copyright (C) 2004 ~ 2007, Feynman Software.
**
** Created by Wei Yongming, 2004/05/19
*/

#ifndef GUI_PS2_KEYCODE_H
    #define GUI_PS2_KEYCODE_H

#define KS_DOWN     0x0100
#define KS_UP       0x0200
#define KS_UNKNOWN  0x0000

#define KBD_ESCAPEE0    0xe0        /* in */
#define KBD_ESCAPEE1    0xe1        /* in */

#define K_NONE          0xff

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

unsigned short mg_ps2_converter (int ps2_fd);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_PS2_KEYCODE_H */


