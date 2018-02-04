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
 ** ial.h: the head file of Input Abstract Layer
 **
 ** Create data: 2000/06/13
 */

#ifndef GUI_IAL_H
#define GUI_IAL_H

#ifndef WIN32
#   ifndef HAVE_SYS_TIME_H
#       define fd_set void
#   else

#       ifdef __OSE__
#           define fd_set void
#       endif

#       ifndef __NONEED_FD_SET_TYPE
#           include <unistd.h>
#           include <sys/types.h>
#           include <sys/time.h>
#       endif
#   endif
#endif

#include "mgsock.h"
#include "misc.h"
#include "gal.h"
#include "customial.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MGGAL_SHADOW
#define _ROT_DIR_CW 0x02
#define _ROT_DIR_CCW 0x04
#define _ROT_DIR_HFLIP 0x08
#define _ROT_DIR_VFLIP 0x10
#endif
extern void (*__mg_ial_change_mouse_xy_hook) (int* x, int* y);
extern DWORD __mg_shadow_rotate_flags;

extern GAL_Surface* __gal_screen;
extern INPUT* __mg_cur_input;

#ifdef _MGHAVE_MOUSECALIBRATE
    typedef void (*DO_MOUSE_CALIBRATE_PROC) (int* x, int* y);
    extern DO_MOUSE_CALIBRATE_PROC __mg_do_mouse_calibrate;
    extern POINT __mg_mouse_org_pos;
#endif

#define IAL_InitInput           (*__mg_cur_input->init_input)
#define IAL_TermInput           (*__mg_cur_input->term_input)
#define IAL_UpdateMouse         (*__mg_cur_input->update_mouse)

//#define IAL_GetMouseXY          (*__mg_cur_input->get_mouse_xy)
static inline void IAL_GetMouseXY(int *x, int* y) {

        if (__mg_cur_input && __mg_cur_input->get_mouse_xy) {
            (*__mg_cur_input->get_mouse_xy) (x, y);
#ifdef _MGHAVE_MOUSECALIBRATE
            __mg_mouse_org_pos.x = *x;
            __mg_mouse_org_pos.y = *y;
#endif
        }

#ifdef _MGHAVE_MOUSECALIBRATE
        if (__mg_do_mouse_calibrate) __mg_do_mouse_calibrate (x, y);
#endif

        if (*x < 0) *x = 0;
        if (*y < 0) *y = 0;
        
        if (__mg_ial_change_mouse_xy_hook)
            __mg_ial_change_mouse_xy_hook (x, y);
        else
        {
            if (*x > (__gal_screen->w-1)) *x = (__gal_screen->w-1);
            if (*y > (__gal_screen->h-1)) *y = (__gal_screen->h-1);
        }
}

#define IAL_GetMouseButton      (*__mg_cur_input->get_mouse_button)

//#define IAL_SetMouseXY          if (__mg_cur_input->set_mouse_xy) (*__mg_cur_input->set_mouse_xy)
static inline void IAL_SetMouseXY (int x, int y) {
    if (__mg_cur_input->set_mouse_xy) {
#ifdef _MGGAL_SHADOW
        if (__mg_shadow_rotate_flags & _ROT_DIR_CW){
            int tmp = x;
            x = (__gal_screen->h-1) - y;
            y = tmp;
        }
        else if (__mg_shadow_rotate_flags & _ROT_DIR_CCW){
            int tmp = y;
            y = (__gal_screen->w-1) - x;
            x = tmp;
        }else if (__mg_shadow_rotate_flags & _ROT_DIR_HFLIP){
            x =  __gal_screen->w - x;
        }else if (__mg_shadow_rotate_flags & _ROT_DIR_VFLIP){
            y =  __gal_screen->h - y;
        }
#endif
        (*__mg_cur_input->set_mouse_xy) (x, y);
    }
}

//#define IAL_SetMouseRange       if (__mg_cur_input->set_mouse_range) (*__mg_cur_input->set_mouse_range)
static inline void IAL_SetMouseRange (int minx, int miny, int maxx, int maxy) {

#ifdef _MGGAL_SHADOW
    if (__mg_shadow_rotate_flags)
        return;
#endif
    if (__mg_cur_input->set_mouse_range) (*__mg_cur_input->set_mouse_range)(minx, miny, maxx, maxy);

}

#define IAL_SuspendMouse        if (__mg_cur_input->suspend_mouse) (*__mg_cur_input->suspend_mouse)
#define IAL_UpdateKeyboard      (*__mg_cur_input->update_keyboard)
#define IAL_GetKeyboardState    (*__mg_cur_input->get_keyboard_state)
#define IAL_SuspendKeyboard     if (__mg_cur_input->suspend_keyboard) (*__mg_cur_input->suspend_keyboard)
#define IAL_SetLeds(leds)       if (__mg_cur_input->set_leds) (*__mg_cur_input->set_leds) (leds)

static inline int IAL_ResumeMouse (void)
{
    if (__mg_cur_input->resume_mouse)
        return __mg_cur_input->resume_mouse ();
    else
        return -1;
}

static inline int IAL_ResumeKeyboard (void)
{
    if (__mg_cur_input->resume_keyboard)
        return __mg_cur_input->resume_keyboard ();
    else
        return -1;
}

#define IAL_WaitEvent           (*__mg_cur_input->wait_event)

#define IAL_MType               (__mg_cur_input->mtype)
#define IAL_MDev                (__mg_cur_input->mdev)

int mg_InitIAL (void);
void mg_TerminateIAL (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_H */

