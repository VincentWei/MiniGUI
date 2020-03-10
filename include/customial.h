///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/**
 * \file customial.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2007/06/06
 *
 * \brief This file is the head file for Custom IAL Engine.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2007~2020, Beijing FMSoft Technologies Co., Ltd.

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
 * $Id: customial.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef GUI_IAL_CUSTOM_H
    #define GUI_IAL_CUSTOM_H

#define IAL_MOUSE_LEFTBUTTON    (1 << 0)
#define IAL_MOUSE_RIGHTBUTTON   (1 << 1)
#define IAL_MOUSE_MIDDLEBUTTON  (1 << 2)

/* deprecated since 4.0.0
#define IAL_MOUSE_FOURTHBUTTON  (1 << 3)
#define IAL_MOUSE_FIFTHBUTTON   (1 << 4)
#define IAL_MOUSE_SIXTHBUTTON   (1 << 5)
#define IAL_MOUSE_SEVENTHBUTTON (1 << 6)
#define IAL_MOUSE_EIGHTHBUTTON  (1 << 7)

#define IAL_MOUSE_SIDEBUTTON    IAL_MOUSE_FOURTHBUTTON
#define IAL_MOUSE_EXTRABUTTON   IAL_MOUSE_FIFTHBUTTON
#define IAL_MOUSE_FORWARDBUTTON IAL_MOUSE_SIXTHBUTTON
#define IAL_MOUSE_BACKBUTTON    IAL_MOUSE_SEVENTHBUTTON
#define IAL_MOUSE_TASKBUTTON    IAL_MOUSE_EIGHTHBUTTON
#define IAL_MOUSE_RESETBUTTON   IAL_MOUSE_EIGHTHBUTTON
*/

#define IAL_EVENT_MOUSE         (1 << 0)
    #define IAL_MOUSEEVENT          IAL_EVENT_MOUSE
#define IAL_EVENT_KEY           (1 << 1)
    #define IAL_KEYEVENT            IAL_EVENT_KEY

#define IAL_EVENT_EXTRA         (1 << 2)

/* The event identifiers, should keep sync with MSG_EXIN_XXX */
#ifndef AXIS_SCROLL_INVALID
    #define AXIS_SCROLL_INVALID     0
    #define AXIS_SCROLL_VERTICAL    1
    #define AXIS_SCROLL_HORIZONTAL  2

    #define AXIS_SOURCE_INVALID     0
    #define AXIS_SOURCE_WHEEL       1
    #define AXIS_SOURCE_FINGER      2
    #define AXIS_SOURCE_CONTINUOUS  3
    #define AXIS_SOURCE_WHEEL_TILT  4
#endif

#define IAL_EVENT_AXIS                   0x0070
#define IAL_EVENT_BUTTONDOWN             0x0071
#define IAL_EVENT_BUTTONUP               0x0072

#define IAL_EVENT_TOUCH_DOWN             0x0073
#define IAL_EVENT_TOUCH_UP               0x0074
#define IAL_EVENT_TOUCH_MOTION           0x0075
#define IAL_EVENT_TOUCH_CANCEL           0x0076
#define IAL_EVENT_TOUCH_FRAME            0x0077

#ifndef SWITCH_INVALID
    #define SWITCH_INVALID                  0
    #define SWITCH_LID                      1
    #define SWITCH_TABLET_MODE              2

    #define SWITCH_STATE_INVALID            0
    #define SWITCH_STATE_ON                 1
    #define SWITCH_STATE_OFF                2
#endif

#define IAL_EVENT_SWITCH_TOGGLE          0x007A

#define IAL_EVENT_GESTURE_SWIPE_BEGIN    0x0080
#define IAL_EVENT_GESTURE_SWIPE_UPDATE   0x0081
#define IAL_EVENT_GESTURE_SWIPE_END      0x0082
#define IAL_EVENT_GESTURE_PINCH_BEGIN    0x0083
#define IAL_EVENT_GESTURE_PINCH_UPDATE   0x0084
#define IAL_EVENT_GESTURE_PINCH_END      0x0085

#ifndef TABLET_TOOL_X
    #define TABLET_TOOL_PROXIMITY_STATE_INVALID 0
    #define TABLET_TOOL_PROXIMITY_STATE_OUT     1
    #define TABLET_TOOL_PROXIMITY_STATE_IN      2

    #define TABLET_TOOL_TIP_INVALID         0
    #define TABLET_TOOL_TIP_UP              1
    #define TABLET_TOOL_TIP_DOWN            2

    #define TABLET_BUTTON_STATE_INVALID     0
    #define TABLET_BUTTON_STATE_RELEASED    1
    #define TABLET_BUTTON_STATE_PRESSED     2

    #define TABLET_PAD_RING_SOURCE_UNKNOWN  0
    #define TABLET_PAD_RING_SOURCE_FINGER   1

    #define TABLET_PAD_STRIP_SOURCE_UNKNOWN 0
    #define TABLET_PAD_STRIP_SOURCE_FINGER  1

    #define TABLET_TOOL_CHANGED_X           (1 << 0)
    #define TABLET_TOOL_CHANGED_Y           (1 << 1)
    #define TABLET_TOOL_CHANGED_PRESSURE    (1 << 2)
    #define TABLET_TOOL_CHANGED_DISTANCE    (1 << 3)
    #define TABLET_TOOL_CHANGED_TILT_X      (1 << 4)
    #define TABLET_TOOL_CHANGED_TILT_Y      (1 << 5)
    #define TABLET_TOOL_CHANGED_ROTATION    (1 << 6)
    #define TABLET_TOOL_CHANGED_SLIDER      (1 << 7)
    #define TABLET_TOOL_CHANGED_SIZE_MAJOR  (1 << 8)
    #define TABLET_TOOL_CHANGED_SIZE_MINOR  (1 << 9)
    #define TABLET_TOOL_CHANGED_WHEEL       (1 << 10)

    #define TABLET_TOOL_X                   0
    #define TABLET_TOOL_Y                   1
    #define TABLET_TOOL_PRESSURE            2
    #define TABLET_TOOL_DISTANCE            3
    #define TABLET_TOOL_TILT_X              4
    #define TABLET_TOOL_TILT_Y              5
    #define TABLET_TOOL_ROTATION            6
    #define TABLET_TOOL_SLIDER              7
    #define TABLET_TOOL_SIZE_MAJOR          8
    #define TABLET_TOOL_SIZE_MINOR          9
    #define TABLET_TOOL_WHEEL               10

#endif

#define IAL_EVENT_TABLET_TOOL_AXIS       0x0090
#define IAL_EVENT_TABLET_TOOL_PROXIMITY  0x0091
#define IAL_EVENT_TABLET_TOOL_TIP        0x0092
#define IAL_EVENT_TABLET_TOOL_BUTTON     0x0093

#define IAL_EVENT_TABLET_PAD_BUTTON      0x0094
#define IAL_EVENT_TABLET_PAD_RING        0x0095
#define IAL_EVENT_TABLET_PAD_STRIP       0x0096

#define IAL_EVENT_USER_BEGIN             0x009A
#define IAL_EVENT_USER_UPDATE            0x009B
#define IAL_EVENT_USER_END               0x009C

#define NR_PACKED_SUB_EVENTS            16

typedef struct _EXTRA_INPUT_EVENT {
    int event;
    WPARAM wparam;
    LPARAM lparam;

    // for tablet tool messages
    unsigned int    params_mask;
    WPARAM          wparams[NR_PACKED_SUB_EVENTS];
    LPARAM          lparams[NR_PACKED_SUB_EVENTS];
} EXTRA_INPUT_EVENT;

#define IAL_LEN_MDEV            127

typedef struct tagINPUT {
    char*   id;

    // Initialization and termination
    BOOL (*init_input) (struct tagINPUT *input, const char* mdev, const char* mtype);
    void (*term_input) (void);

    // Mouse operations
    int  (*update_mouse) (void);
    void (*get_mouse_xy) (int* x, int* y);
    void (*set_mouse_xy) (int x, int y);
    int  (*get_mouse_button) (void);
    void (*set_mouse_range) (int minx, int miny, int maxx, int maxy);
    void (*suspend_mouse) (void);
    int (*resume_mouse) (void);

    // Keyboard operations
    int  (*update_keyboard) (void);
    const char* (*get_keyboard_state) (void);
    void (*suspend_keyboard) (void);
    int (*resume_keyboard) (void);
    void (*set_leds) (unsigned int leds);

    // Event loop
    int (*wait_event) (int which, int maxfd, fd_set *in, fd_set *out,
            fd_set *except, struct timeval *timeout);

    // New wait event method for getting extra input events; since 4.0.0
    int (*wait_event_ex) (int maxfd, fd_set *in, fd_set *out,
            fd_set *except, struct timeval *timeout, EXTRA_INPUT_EVENT* extra);

    // since 5.0.0, save the mouse device
    char*   mdev;
} INPUT;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MGIAL_CUSTOM
extern BOOL InitCustomInput (INPUT* input, const char* mdev, const char* mtype);
extern void TermCustomInput (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_CUSTOM_H */

