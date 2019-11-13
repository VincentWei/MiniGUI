/**
 * \file exstubs.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2019/04/07
 *
 * \brief This file is the header for NEWGAL/IAL engines which should be
 *  implemented by an external module.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2007~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

/*
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef GUI_EXSTUBS_H
    #define GUI_EXSTUBS_H

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332   1
#define COMMLCD_TRUE_RGB555     2
#define COMMLCD_TRUE_RGB565     3
#define COMMLCD_TRUE_RGB888     4
#define COMMLCD_TRUE_RGB0888    5
#define COMMLCD_TRUE_ARGB1555   6
#define COMMLCD_TRUE_ARGB8888   7
#define COMMLCD_TRUE_ABRG8888   8

#define COMMLCD_UPDATE_NONE     0
#define COMMLCD_UPDATE_SYNC     1
#define COMMLCD_UPDATE_ASYNC    2

struct commlcd_info {
    int     height;         // vertical resolution of the screen
    int     width;          // horinzontal resolution of the screen
    int     pitch;          // Length of one scan line in bytes
    int     bpp;            // Depth (bits-per-pixel)
    int     type;           // Pixel type
    int     update_method;  // Update method: none, asynchronously, or synchronously.
    Uint8*  fb;             // Frame buffer
};

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

#define COMM_MOUSELBUTTON  0x01
#define COMM_MOUSERBUTTON  0x04

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* external stubs for COMMLCD NEWGAL engine */
#ifdef _MGGAL_COMMLCD

int __commlcd_drv_init (void);
int __commlcd_drv_getinfo (struct commlcd_info *li,
        int width, int height, int depth);
int __commlcd_drv_release (void);
int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors);
int __commlcd_drv_update (const RECT* rc_dirty);

#endif /* _MGGAL_COMMLCD */

#ifdef _MGIAL_COMM

int __comminput_init (void);
int __comminput_ts_getdata (short *x, short *y, short *button);
int __comminput_kb_getdata (short *key, short *status);
int __comminput_wait_for_input (struct timeval *timeout);
void __comminput_deinit (void);

#endif /* _MGIAL_COMM */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_EXSTUBS_H */

