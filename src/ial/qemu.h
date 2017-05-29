/*
 ** qemu.h:. the head file of Low Level Input Engine for QEMU
 **
 ** Copyright (C) 2003 ~ 2007 Feynman Software.
 **
 ** Created by Libo Jiao, 2007/03/04
 */

#ifndef GUI_IAL_QEMU_H
    #define GUI_IAL_QEMU_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    BOOL    InitQEMUInput (INPUT* input, const char* mdev, const char* mtype); 
    void    TermQEMUInput (void); 

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_FiguerOA_H */

