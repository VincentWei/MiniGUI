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
 * \file window.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/26
 *
 * \brief This file includes windowing interfaces of MiniGUI.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

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
 * $Id: window.h 13674 2010-12-06 06:45:01Z wanzheng $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_WINDOW_H
#define _MGUI_WINDOW_H

#include "common.h"

#include <stddef.h>
#include <stdlib.h>

#ifdef _MGHAVE_VIRTUAL_WINDOW
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef HAVE_POLL
#include <poll.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \defgroup msgs Messages
     * @{
     */

/* Definitions of common messages. */
#define MSG_NULLMSG         0x0000
#define MSG_SYNCMSG         0x0000

    /**
     * \defgroup mouse_msgs Mouse event messages
     * @{
     */

/* Group 1 from 0x0001 to 0x000F, the mouse messages. */
#define MSG_FIRSTMOUSEMSG   0x0001

/**
 * \def MSG_LBUTTONDOWN
 * \brief Left mouse button down message.
 *
 * This message is posted to the window when the user presses down
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONDOWN
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONUP, key_defs
 *
 * Example:
 * \include buttondown.c
 */
#define MSG_LBUTTONDOWN     0x0001

/**
 * \def MSG_LBUTTONUP
 * \brief Left mouse button up message.
 *
 * This message is posted to the window when the user releases up
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONUP
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONDOWN, key_defs
 */
#define MSG_LBUTTONUP       0x0002

/**
 * \def MSG_LBUTTONDBLCLK
 * \brief Left mouse button double clicked message.
 *
 * This message is posted to the window when the user double clicks
 * the left button of the mouse in the client area of the window.
 *
 * \code
 * MSG_LBUTTONDBLCLK
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONDBLCLK, key_defs
 */
#define MSG_LBUTTONDBLCLK   0x0003

/**
 * \def MSG_MOUSEMOVE
 * \brief The mouse moved message.
 *
 * This message is posted to the window when the user moves the mouse
 * in the client area of the window.
 *
 * \code
 * MSG_MOUSEMOVE
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa key_defs
 */
#define MSG_MOUSEMOVE       0x0004

/**
 * \def MSG_RBUTTONDOWN
 * \brief Right mouse button down message.
 *
 * This message is posted to the window when the user presses down
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONDOWN
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONUP, key_defs
 *
 * Example:
 * \include buttondown.c
 */
#define MSG_RBUTTONDOWN     0x0005

/**
 * \def MSG_RBUTTONUP
 * \brief Right mouse button up message.
 *
 * This message is posted to the window when the user releases up
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONUP
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONDOWN, key_defs
 */
#define MSG_RBUTTONUP       0x0006

/**
 * \def MSG_RBUTTONDBLCLK
 * \brief Right mouse button double clicked message.
 *
 * This message is posted to the window when the user double clicks
 * the right button of the mouse in the client area of the window.
 *
 * \code
 * MSG_RBUTTONDBLCLK
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONDBLCLK, key_defs
 */
#define MSG_RBUTTONDBLCLK   0x0007

/**
 * \def MSG_MBUTTONDOWN
 * \brief Middle mouse button down message.
 *
 * This message is posted to the window when the user presses down
 * the middle button of the mouse in the client area of the window.
 *
 * \code
 * MSG_MBUTTONDOWN
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_MBUTTONUP, key_defs
 *
 * Since 4.0.0
 */
#define MSG_MBUTTONDOWN     0x0008

/**
 * \def MSG_MBUTTONUP
 * \brief Middle mouse button up message.
 *
 * This message is posted to the window when the user releases up
 * the middle button of the mouse in the client area of the window.
 *
 * \code
 * MSG_MBUTTONUP
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_MBUTTONDOWN, key_defs
 *
 * Since 4.0.0
 */
#define MSG_MBUTTONUP       0x0009

/**
 * \def MSG_MBUTTONDBLCLK
 * \brief Middle mouse button double clicked message.
 *
 * This message is posted to the window when the user double clicks
 * the middle button of the mouse in the client area of the window.
 *
 * \code
 * MSG_MBUTTONDBLCLK
 * DWORD key_flags = (DWORD)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param key_flags The shift key status when this message occurred.
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_LBUTTONDBLCLK, key_defs
 */
#define MSG_MBUTTONDBLCLK   0x000A

#define MSG_NCMOUSEOFF      0x000A

/**
 * \def MSG_NCLBUTTONDOWN
 * \brief Left mouse button down message in the non-client area.
 *
 * This message is posted to the window when the user presses down
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONDOWN
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONUP, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONDOWN   0x000B

/**
 * \def MSG_NCLBUTTONUP
 * \brief Left mouse button up message in the non-client area.
 *
 * This message is posted to the window when the user releases up
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONUP
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONDOWN, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONUP     0x000C

/**
 * \def MSG_NCLBUTTONDBLCLK
 * \brief Left mouse button double clicked in the non-client area.
 *
 * This message is posted to the window when the user double clicks
 * the left button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCLBUTTONDBLCLK
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONDBLCLK, MSG_NCHITTEST
 */
#define MSG_NCLBUTTONDBLCLK 0x000D

/**
 * \def MSG_NCMOUSEMOVE
 * \brief Mouse moves in the non-client area.
 *
 * This message is posted to the window when the user moves the mouse
 * in the non-client area of the window.
 *
 * \code
 * MSG_NCMOUSEMOVE
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCHITTEST
 */
#define MSG_NCMOUSEMOVE     0x000E

/**
 * \def MSG_NCRBUTTONDOWN
 * \brief Right mouse button down message in the non-client area.
 *
 * This message is posted to the window when the user presses down
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONDOWN
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONUP, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONDOWN   0x000F

/**
 * \def MSG_NCRBUTTONUP
 * \brief Right mouse button up message in the non-client area.
 *
 * This message is posted to the window when the user releases up
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONUP
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCRBUTTONDOWN, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONUP     0x0010

/**
 * \def MSG_NCRBUTTONDBLCLK
 * \brief Right mouse button double clicked in the non-client area.
 *
 * This message is posted to the window when the user double clicks
 * the right button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCRBUTTONDBLCLK
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONDBLCLK, MSG_NCHITTEST
 */
#define MSG_NCRBUTTONDBLCLK 0x0011

/**
 * \def MSG_NCMBUTTONDOWN
 * \brief Middle mouse button down message in the non-client area.
 *
 * This message is posted to the window when the user presses down
 * the middle button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCMBUTTONDOWN
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCMBUTTONUP, MSG_NCHITTEST
 */
#define MSG_NCMBUTTONDOWN   0x0012

/**
 * \def MSG_NCMBUTTONUP
 * \brief Middle mouse button up message in the non-client area.
 *
 * This message is posted to the window when the user releases up
 * the middle button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCMBUTTONUP
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCMBUTTONDOWN, MSG_NCHITTEST
 */
#define MSG_NCMBUTTONUP     0x0013

/**
 * \def MSG_NCMBUTTONDBLCLK
 * \brief Middle mouse button double clicked in the non-client area.
 *
 * This message is posted to the window when the user double clicks
 * the middle button of the mouse in the non-client area of the window.
 *
 * \code
 * MSG_NCMBUTTONDBLCLK
 * int hit_code = (int)wParam;
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param hit_code The hit test code which tells the area of the mouse.
 * \param x_pos,y_pos The position of the mouse in window coordinates.
 *
 * \sa MSG_NCLBUTTONDBLCLK, MSG_NCHITTEST
 */
#define MSG_NCMBUTTONDBLCLK 0x0014

#define MSG_LASTMOUSEMSG    0x0014

    /** @} end of mouse_msgs */

    /**
     * \defgroup key_msgs Key event messages
     * @{
     */

/* Group 2 from 0x0015 to 0x001F, the key messages. */
#define MSG_FIRSTKEYMSG     0x0015

/**
 * \def MSG_KEYDOWN
 * \brief User presses a key down.
 *
 * This message is posted to the current active window when the user
 * presses a key down.
 *
 * \code
 * MSG_KEYDOWN
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_KEYUP, key_defs
 *
 * Example:
 *
 * \include keydown.c
 */
#define MSG_KEYDOWN         0x0015

/**
 * \def MSG_CHAR
 * \brief A character translated from MSG_KEYDOWN message.
 *
 * This message is translated from a MSG_KEYDOWN message by \a TranslateMessage
 * and sent to the current active window.
 *
 * \code
 * MSG_CHAR
 * unsigned char ch_buff [4];
 * ch_buff [0] = FIRSTBYTE(wParam);
 * ch_buff [1] = SECONDBYTE(wParam);
 * ch_buff [2] = THIRDBYTE(wParam);
 * ch_buff [3] = FOURTHBYTE(wParam);
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param ch_buff The buffer to store the bytes of the character.
 * \param key_flags The shift key status when this message occurred.
 *
 * \note Please use \a FIRSTBYTE ~ \a FOURTHBYTE to get the bytes
 *       if the character is a multi-byte character. Use MSG_UTF8CHAR
 *       to handle the characters encoded in UTF-8.
 *
 * \sa MSG_SYSCHAR, TranslateMessage, key_defs
 */
#define MSG_CHAR            0x0016

/**
 * \def MSG_KEYUP
 * \brief User releases up a key.
 *
 * This message is posted to the current active window when the user
 * releases up a key.
 *
 * \code
 * MSG_KEYUP
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the released key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_KEYDOWN, key_defs
 */
#define MSG_KEYUP           0x0017

/**
 * \def MSG_SYSKEYDOWN
 * \brief User presses down a key when \<Alt\> key is down.
 *
 * This message is posted to the current active window when the user
 * presses down a key as \<Alt\> key is down.
 *
 * \code
 * MSG_SYSKEYDOWN
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSKEYUP, MSG_SYSCHAR, key_defs
 */
#define MSG_SYSKEYDOWN      0x0018

/**
 * \def MSG_SYSCHAR
 * \brief A system character translated from MSG_SYSKEYDOWN message.
 *
 * This message is translated from a MSG_SYSKEYDOWN message by
 * \a TranslateMessage and sent to the current active window.
 *
 * \code
 * MSG_SYSCHAR
 * int ch = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param ch The ASCII code of the pressed key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_CHAR, TranslateMessage, key_defs
 */
#define MSG_SYSCHAR         0x0019

/**
 * \def MSG_SYSKEYUP
 * \brief User releases up a key when \<Alt\> key is down.
 *
 * This message is posted to the current active window when the user
 * releases up a key as \<Alt\> key is down.
 *
 * \code
 * MSG_SYSKEYUP
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param scancode The scan code of the released key.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSKEYDOWN, key_defs
 */
#define MSG_SYSKEYUP        0x001A


/* keyboard longpress supported */

/**
 * \def MSG_KEYLONGPRESS
 * \brief A key is long pressed.
 *
 * This message is sent when a key is pressed exceed user-defined long
 * time value.
 *
 * \code
 * MSG_KEYLONGPRESS
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 */
#define MSG_KEYLONGPRESS    0x001B

/**
 * \def MSG_KEYALWAYSPRESS
 * \brief A key is always pressed.
 *
 * This message is sent when a key is pressed to exceed user-defined
 * always time value.
 *
 * \code
 * MSG_KEYLONGPRESS
 * int scancode = (int)wParam;
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 */
#define MSG_KEYALWAYSPRESS  0x001C

/**
 * \def MSG_KEYSYM
 * \brief A key symbol translated from MSG_KEYDOWN messages.
 *
 * This message is translated from a MSG_KEYDOWN message by
 * \a TranslateMessage and sent to the current active window.
 *
 * Note that one translation may generate a key symbol made by more than one
 * character, e.g., when using default keymap, DEL key will generate the
 * key symbol "^[[3~".
 *
 * \code
 * MSG_KEYSYM
 * int index = HIBYTE (wParam);
 * int keysym = LOBYTE (wParam);
 * DWORD key_flags = (DWORD)lParam;
 * \endcode
 *
 * \param index The index of the key symbol in one translation,
 *        zero for a new translation, and the \a keysym is the first symbol.
 * \param keysym The code of the key symbol.
 * \param key_flags The shift key status when this message occurred.
 *
 * \sa MSG_SYSCHAR, TranslateMessage, key_defs
 */
#define MSG_KEYSYM          0x001D

/**
 * \def MSG_UTF8CHAR
 * \brief A character translated from MSG_KEYDOWN message.
 *
 * This message generally sent by a IME window to the current active window.
 * The chararcter will be encoded in UTF-8.
 *
 * \code
 * MSG_UTF8CHAR
 * unsigned char ch_utf8 [6];
 * ch_utf8 [0] = FIRSTBYTE(wParam);
 * ch_utf8 [1] = SECONDBYTE(wParam);
 * ch_utf8 [2] = THIRDBYTE(wParam);
 * ch_utf8 [3] = FOURTHBYTE(wParam);
 * ch_utf8 [4] = FIRSTBYTE(lParam);
 * ch_utf8 [5] = SECONDBYTE(lParam);
 * \endcode
 *
 * \param ch_utf8 The buffer to save the character in UTF-8.
 *
 * \sa MSG_CHAR, key_defs
 */
#define MSG_UTF8CHAR        0x001E

/**
 * \def DEF_LPRESS_TIME
 * \brief Default long pressed time of a key.
 *
 * \sa MSG_KEYLONGPRESS
 */
#define DEF_LPRESS_TIME     200

/**
 * \def DEF_APRESS_TIME
 * \brief Default always pressed time of a key.
 *
 * \sa MSG_KEYALWAYSPRESS
 */
#define DEF_APRESS_TIME     1000
/**
 * \def DEF_INTERVAL_TIME
 * \brief Default send MSG_KEYLONGPRESS in interval value.
 *
 */
#define DEF_INTERVAL_TIME   200

extern DWORD __mg_key_longpress_time;
extern DWORD __mg_key_alwayspress_time;
extern DWORD __mg_interval_time;

/**
 * \def SetKeyLongPressTime(time)
 * \brief User set default long pressed time of a key.
 */
#define SetKeyLongPressTime(time)       \
    do {                                \
        __mg_key_longpress_time = time; \
    } while (0)

/**
 * \def SetKeyAlwaysPressTime(time)
 * \brief User set default always pressed time of a key.
 */
#define SetKeyAlwaysPressTime(time)     \
    do {                                \
        __mg_key_alwayspress_time = time; \
    } while (0)

/**
 * \def SetIntervalTime(time)
 * \brief User set default interval time that MSG_KEYLONGPRESS is sent.
 */
#define SetIntervalTime(time)           \
    do {                                \
        __mg_interval_time = time;      \
    } while (0)

#define MSG_LASTKEYMSG      0x001F

    /** @} end of key_msgs */

    /**
     * \defgroup post_event_msgs User-machine Interaction messages
     * @{
     */

/* Group 3 from 0x0020 to 0x005F, User-machine Interaction messages. */
#define MSG_FIRSTPOSTMSG    0x0020

/**
 * \def MSG_SETCURSOR
 * \brief Set cursor shape in the client area.
 *
 * This message is posted to the window under the cursor when the user moves
 * the mouse in order to give the chance to change the cursor shape.
 * The default handler set the cursor shape to the default cursor of the window.
 * If you set a new cursor shape, your message handler should return
 * immediately.
 *
 * \code
 * MSG_SETCURSOR
 * int cx = LOSWORD (lParam);
 * int cy = HISWORD (lParam);
 * \endcode
 *
 * \param cx,cy The client coordinates of the cursor.
 *
 * Example:
 *
 * \include setcursor.c
 *
 * \sa MSG_NCSETCURSOR
 */
#define MSG_SETCURSOR       0x0020

    #define HT_MASK             0xFF

    #define HT_UNKNOWN          0x00
    #define HT_OUT              0x01
    #define HT_MENUBAR          0x02
    #define HT_TRANSPARENT      0x03

    #define HT_BORDER           0x04
    #define HT_CLIENT           0x0C

    #define HT_NEEDCAPTURE      0x10
    #define HT_ICON             0x14
    #define HT_CLOSEBUTTON      0x15
    #define HT_MAXBUTTON        0x16
    #define HT_MINBUTTON        0x17
    #define HT_HSCROLL          0x18
    #define HT_VSCROLL          0x19

    #define HT_DRAGGABLE        0x20
    #define HT_CAPTION          0x20

    /*indicate cursor at border */
    #define HT_BORDER_MASK      0x28
    #define HT_BORDER_TOP       0x28
    #define HT_BORDER_BOTTOM    0x29
    #define HT_BORDER_LEFT      0x2A
    #define HT_BORDER_RIGHT     0x2B

    /*indicate cursor at border corner*/
    #define HT_CORNER_MASK      0x2C
    #define HT_CORNER_TL        0x2C
    #define HT_CORNER_TR        0x2D
    #define HT_CORNER_BL        0x2E
    #define HT_CORNER_BR        0x2F

    /*new scrollbar hittest value
     * can be AND'ed with HT_NEEDCAPTURE*/
    #define HT_SB_MASK          0x50

    #define HT_SB_LEFTARROW     0x50
    #define HT_SB_RIGHTARROW    0x51
    #define HT_SB_LEFTSPACE     0x52
    #define HT_SB_RIGHTSPACE    0x53
    #define HT_SB_HTHUMB        0x54

    #define HT_SB_VMASK         0x58

    #define HT_SB_UPARROW       0x58
    #define HT_SB_DOWNARROW     0x59
    #define HT_SB_UPSPACE       0x5a
    #define HT_SB_DOWNSPACE     0x5b
    #define HT_SB_VTHUMB        0x5c

    #define HT_SB_UNKNOWN       0x5f

/*user defined hittest code are 0x80 ~ 0x8F*/
#define HT_USER_MASK            0x80


/**
 * \def MSG_NCHITTEST
 * \brief Hit test in non-client area.
 * This is an async message.
 */
#define MSG_NCHITTEST       0x0021

/**
 * \def MSG_HITTEST
 * \brief Hit test in non-client area.
 *
 * \sa MSG_NCHITTEST
 */
#define MSG_HITTEST         MSG_NCHITTEST

/**
 * \def MSG_CHANGESIZE
 * \brief Change window size.
 */
#define MSG_CHANGESIZE      0x0022

/* reserved */
#define MSG_QUERYNCRECT     0x0023

/**
 * \def MSG_QUERYCLIENTAREA
 * \brief Query client area.
 */
#define MSG_QUERYCLIENTAREA 0x0024

/**
 * \def MSG_SIZECHANGING
 * \brief Indicates the size of the window is being changed.
 *
 * This message is sent to the window when the size is being changed.
 * If you want to control the actual position and size of the window when
 * the size is being changed (this may be caused by \a MoveWindow or
 * other functions), you should handle this message, and return the actual
 * position and size of the window through the second parameter.
 *
 * \code
 * MSG_SIZECHANGING
 * const RECT* rcExpect = (const RECT*)wParam;
 * RECT* rcResult = (RECT*)lParam;
 * \endcode
 *
 * \param rcExpect The expected size of the window after changing.
 * \param rcResult The actual size of the window after changing.
 *
 * Example:
 *
 * \include msg_sizechanging.c
 */
#define MSG_SIZECHANGING    0x0025

/**
 * \def MSG_SIZECHANGED
 * \brief Indicates the size of the window has been changed.
 *
 * This message is sent to the window when the size has been changed.
 * If you want adjust the size of the client area of the window,
 * you should handle this message, change the values of the client area,
 * and return non-zero value to indicate that the client area has been
 * modified.
 *
 * \code
 * MSG_SIZECHANGED
 * RECT* rcClient = (RECT*)lParam;
 * \endcode
 *
 * \param rcClient The pointer to a RECT structure which contains
 *        the new client area.
 *
 * Example:
 *
 * \include msg_sizechanged.c
 */
#define MSG_SIZECHANGED     0x0026

/**
 * \def MSG_CSIZECHANGED
 * \brief Indicates the size of the client area of the window has been changed.
 *
 * This message is sent as a notification to the window when the size of
 * the client area has been changed.
 *
 * \code
 * MSG_CSIZECHANGED
 * int client_width = (int)wParam;
 * int client_height = (int)lParam;
 * \endcode
 *
 * \param client_width The width of the client area.
 * \param client_height The height of the client area.
 */
#define MSG_CSIZECHANGED     0x0027

/**
 * \def MSG_SETFOCUS
 * \brief Indicates that the window has gained the input focus.
 *
 * \param lparam The parameter passed into used for pass setfocus
 *        msg to child control if lparam > 0.
 *
 * This message is sent to the window procedure
 * after the window gains the input focus.
 */
#define MSG_SETFOCUS        0x0030

/**
 * \def MSG_KILLFOCUS
 * \brief Indicates that the window has lost the input focus.
 *
 * \param lparam The parameter passed into used for pass killfocus
 *        msg to child control if lparam > 0.
 *
 * This message is sent to the window procedure
 * after the window losts the input focus.
 */
#define MSG_KILLFOCUS       0x0031

/**
 * \def MSG_MOUSEACTIVE
 * \brief Indicates that the window has gained the input focus because
 * the user clicked the window.
 *
 * This message is sent to the window procedure
 * after the user clicked the window and it has gained the input focus.
 */
#define MSG_MOUSEACTIVE     0x0032

/**
 * \def MSG_ACTIVE
 * \brief Indicates that the window has gained the input focus because
 * the user clicked the window.
 *
 * This message is sent to the window procedure
 * after the user clicked the window and it has gained the input focus.
 */
#define MSG_ACTIVE          0x0033

/**
 * \def MSG_CHILDHIDDEN
 * \brief Hide child window.
 */
#define MSG_CHILDHIDDEN     0x0034
    #define RCTM_CLICK          1
    #define RCTM_KEY            2
    #define RCTM_MESSAGE        3
    #define RCTM_SHOWCTRL       4

/**
 * \def MSG_ACTIVEMENU
 * \brief Indicates that the user activates the menu bar and tracks it.
 *
 * This message is sent to the window procedure when the user
 * activates the menu bar and tracks it.
 *
 * If you want to change the states of menu items in the submenu
 * before displaying it, you can handle this message.
 *
 * \code
 * MSG_ACTIVEMENU
 * int pos = (int)wParam;
 * HMENU submenu = (HMENU)lParam;
 * \endcode
 *
 * \param pos The position of the activated submenu. The position value of the
 *        first submenu is 0.
 * \param submenu The handle to the activated submenu.
 *
 * Example:
 *
 * \include activemenu.c
 */
#define MSG_ACTIVEMENU      0x0040

/**
 * \def MSG_DEACTIVEMENU
 * \brief Indicates the end of the tracking of a menu bar or a popup menu.
 *
 * This message is sent to the window procedure when the user has
 * closed the tracking menu bar or popup menu.
 *
 * \code
 * MSG_DEACTIVEMENU
 * HMENU menubar = (HMENU)wParam;
 * HMENU submenu = (HMENU)lParam;
 * \endcode
 *
 * \param menubar The handle to the menu bar. It will be zero when the
 *        deactivated menu is a popup menu.
 * \param submenu The handle to the submenu.
 */
#define MSG_DEACTIVEMENU    0x0041

    /**
     * \defgroup ctrl_scrollbar_ncs Notification codes of srollbar control
     * @{
     */

/**
 * \def SB_LINEUP
 * \brief The SB_LINEUP notification message is sent when the user clicked
 * the up arrow on the bar.
 */
#define SB_LINEUP           0x01a

/**
 * \def SB_LINEDOWN
 * \brief The SB_LINEDOWN notification message is sent when the user clicked
 * the down arrow on the bar.
 */
#define SB_LINEDOWN         0x02

/**
 * \def SB_LINELEFT
 * \brief The SB_LINELEFT notification message is sent when the user clicked
 * the left arrow on the bar.
 */
#define SB_LINELEFT         0x03

/**
 * \def SB_LINERIGHT
 * \brief The SB_LINERIGHT notification message is sent when the user clicked
 * the right arrow on the bar.
 */
#define SB_LINERIGHT        0x04

/**
 * \def SB_PAGEUP
 * \brief The SB_PAGEUP notification message is sent when the user clicked
 * the page up area on the bar.
 */
#define SB_PAGEUP           0x05

/**
 * \def SB_PAGEDOWN
 * \brief The SB_PAGEDOWN notification message is sent when the user clicked
 * the page down area on the bar.
 */
#define SB_PAGEDOWN         0x06

/**
 * \def SB_PAGELEFT
 * \brief The SB_PAGELEFT notification message is sent when the user clicked
 * the page left area on the bar.
 */
#define SB_PAGELEFT         0x07

/**
 * \def SB_PAGERIGHT
 * \brief The SB_PAGERIGHT notification message is sent when the user clicked
 * the page right area on the bar.
 */
#define SB_PAGERIGHT        0x08

/**
 * \def SB_THUMBPOSITION
 * \brief The SB_THUMBPOSITION notification message is sent when the user set
 * a new thumb position.
 */
#define SB_THUMBPOSITION    0x09

/**
 * \def SB_THUMBTRACK
 * \brief The SB_THUMBTRACK notification message is sent when the user is
 * draging and tracking the thumb.
 */
#define SB_THUMBTRACK       0x0A

/**
 * \def SB_TOP
 * \brief The SB_TOP notification message is sent when the user move thumb
 * to minimum position
 */
#define SB_TOP              0x0B

/**
 * \def SB_BOTTOM
 * \brief The SB_BOTTOM notification message is sent when the user move
 * thumb to maximum position
 */
#define SB_BOTTOM           0x0C

/**
 * \def SB_ENDSCROLL
 * \brief The SB_ENDSCROLL notification message is sent when the thumb at
 * the end of bar
 */
#define SB_ENDSCROLL        0x0D

     /** @} end of ctrl_scrollbar_ncs */

/**
 * \def MSG_HSCROLL
 * \brief Indicates that the user has clicked the horizontal scroll bar.
 *
 * This message is sent to the window procedure when the user has clicked
 * the horizontal scroll bar and changed the position of the thumb.
 *
 * \code
 * MSG_HSCROLL
 * int hs_nc = (int)wParam;
 * \endcode
 *
 * \param hs_nc The scrolling code, can be one of the following values:
 *      - SB_LINELEFT\n
 *        The user clicked the left arrow on the bar.
 *      - SB_LINERIGHT\n
 *        The user clicked the right arrow on the bar.
 *      - SB_PAGELEFT\n
 *        The user clicked the left page area on the bar.
 *      - SB_PAGERIGHT\n
 *        The user clicked the right page area on the bar.
 *      - SB_THUMBPOSITION\n
 *        The user set a new thumb position.
 *      - SB_THUMBTRACK\n
 *        The user is draging and tracking the thumb.
 *      - SB_ENDSCROLL\n
 *        The end of scrolling.
 */
#define MSG_HSCROLL         0x0042

/**
 * \def MSG_VSCROLL
 * \brief Indicates that the user has clicked the vertical scroll bar.
 *
 * This message is sent to the window procedure when the user has clicked
 * the vertical scroll bar and changed the position of the thumb.
 *
 * \code
 * MSG_HSCROLL
 * int vs_nc = (int)wParam;
 * \endcode
 *
 * \param vs_nc The scrolling code, can be one of the following values:
 *      - SB_LINEUP\n
 *        The user clicked the up arrow on the bar.
 *      - SB_LINEDOWN\n
 *        The user clicked the down arrow on the bar.
 *      - SB_PAGEUP\n
 *        The user clicked the up page area on the bar.
 *      - SB_PAGEDOWN\n
 *        The user clicked the down page area on the bar.
 *      - SB_THUMBPOSITION\n
 *        The user set a new thumb position.
 *      - SB_THUMBTRACK\n
 *        The user is draging and tracking the thumb.
 *      - SB_ENDSCROLL\n
 *        The end of scrolling.
 */
#define MSG_VSCROLL         0x0043

/**
 * \def MSG_NCSETCURSOR
 * \brief Set cursor shape in the non-client area.
 *
 * This message is posted to the window under the cursor when the user moves
 * the mouse in order to give the chance to change the cursor shape.
 * The default handler set the cursor shape to the default cursor of the window.
 * If you set a new cursor shape, your message handler should return
 * immediately.
 *
 * \sa MSG_SETCURSOR
 */
#define MSG_NCSETCURSOR     0x0044

/**
 * \def MSG_MOUSEMOVEIN
 * \brief Indicates the mouse is moved in/out the area of the window.
 *
 * This message is posted to the window when the user moves the mouse
 * in/out the area of the window.
 *
 * \code
 * MSG_MOUSEMOVEIN
 * BOOL in_out = (BOOL)wParam;
 * \endcode
 *
 * \param in_out Indicates whether the mouse has been moved in the window
 *      or out the window.
 */
#define MSG_MOUSEMOVEIN     0x0050

/**
 * \def MSG_WINDOWDROPPED
 * \brief Indicates that user dropped window.
 * server to client; (wParam, lParam): result rectangle.
 */
#define MSG_WINDOWDROPPED   0x0051

#define MSG_LASTPOSTMSG     0x005F

    /** @} end of post_event_msgs */

    /**
     * \defgroup creation_msgs Window creation messages
     * @{
     */

    /* Group 4 from 0x0060 to 0x007F, the creation messages. */
#define MSG_FIRSTCREATEMSG  0x0060

/**
 * \def MSG_CREATE
 * \brief Indicates the window has been created, and gives you a chance to initialize your private objects.
 *
 * This messages is sent to the window after the window has been created
 * and registered to the system. You can initialize your own objects when
 * you receive this message, and return zero to the system in order to
 * indicates the success of your initialization. If you return non-zero to
 * the system after handled this message, the created window will be
 * destroyed immediately.
 *
 * \code
 * MSG_CREATE for main windows:
 * HWND hosting = (HWND)wParam;
 * PMAINWINCREATE create_info = (PMAINWINCREATE)lParam;
 *
 * MSG_CREATE for virtual windows:
 * HWND hosting = (HWND)wParam;
 * DWORD add_data = (DWORD)lParam;
 *
 * MSG_CREATE for controls:
 * HWND parent = (HWND)wParam;
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param create_info The pointer to the MAINWINCREATE structure which is
 *      passed to CreateMainWindow function.
 * \param hosting The handle to the hosting window of the new main/virtual
 *      window.
 * \param parent The handle to the parent window of the new control.
 * \param add_data The first additional data passed to CreateVirtualWindow or
 *      CreateWindowEx function.
 *
 * \sa CreateMainWindow, CreateVirtualWindow, CreateWindowEx, MAINWINCREATE
 */
#define MSG_CREATE          0x0060

/**
 * \def MSG_NCCREATE
 * \brief Indicates the window has been created, but has not registered to
 *        the system.
 *
 * This message is sent to the window after the window has been created,
 * but not registered the system. Like MSG_CREATE message, you can
 * initialize your own objects when you receive this message, but can not
 * create child windows of the window, and can not get a device context
 * to paint.
 *
 * If you return non-zero to the system after handled this message,
 * the created window will be destroyed immediately.
 *
 * \code
 * MSG_NCCREATE for main windows:
 * PMAINWINCREATE create_info = (PMAINWINCREATE)lParam;
 *
 * MSG_NCCREATE for controls:
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param create_info The pointer to the MAINWINCREATE structure which is
 *        passed to CreateMainWindow function.
 * \param add_data The first additional data passed to CreateWindowEx function.
 *
 * \sa CreateMainWindow, CreateWindowEx, MAINWINCREATE
 */
#define MSG_NCCREATE        0x0061

/*Not use*/
#define MSG_INITPANES       0x0062
#define MSG_DESTROYPANES    0x0063

/**
 * \def MSG_DESTROY
 * \brief Indicates the window will be destroyed.
 *
 * This message is sent to the window when \a DestroyMainWindow
 * or \a DestroyWindow is calling. You can destroy your private objects
 * when receiving this message.
 *
 * If you return non-zero to the system after handle this message, the process
 * of \a DestroyMainWindow and \a DestroyWindow will return immediately.
 *
 * \sa DestroyMainWindow, DestroyWindow
 */
#define MSG_DESTROY         0x0064

/* Not use */
#define MSG_NCDESTROY       0x0065

/**
 * \def MSG_CLOSE
 * \brief Indicates the user has clicked the closing box on the caption.
 *
 * This message is sent to the window when the user has clicked the closing box
 * on the caption of the window.
 */
#define MSG_CLOSE           0x0066

/* Not use */
#define MSG_NCCALCSIZE      0x0067

/*
 * Not implemented, this message is reserved for future use.
 *
 * \def MSG_MAXIMIZE
 * \brief Indicates the user has clicked the maximizing box on the caption.
 *
 * This message is sent to the window when the user has clicked the maximizing
 * box on the caption of the window.
 */
#define MSG_MAXIMIZE        0x0068

/*
 * Not implemented, this message is reserved for future use.
 *
 * \def MSG_MINIMIZE
 * \brief Indicates the user has clicked the minimizing box on the caption.
 *
 * This message is sent to the window when the user has clicked the
 * minimizing box on the caption of the window.
 */
#define MSG_MINIMIZE        0x0069

/*
 * Not implemented, this message is reserved for future use.
 *
 * \def MSG_HELP
 * \brief Indicates the user has clicked the help box on the caption.
 *
 * This message is sent to the window when the user has clicked the
 * help box on the caption of the window.
 */
#define MSG_HELP            0x006A

/**
 * \def MSG_GESTURETEST
 * \brief This message will be sent to a main window for gesture test.
 *
 * The client should call \a SetMainWindowGestureFlags to set the gesture flags
 * for a main window after got this message.
 *
 * Since 5.0.6.
 */
#define MSG_GESTURETEST     0x006B

#define MSG_LASTCREATEMSG   0x006F

    /** @} end of creation_msgs */

    /**
     * \defgroup extra_input_msgs Extra input messages
     *
     * In addition to the standard keyboard and mouse messages,
     * MiniGUI generates extra input messages for input events from
     * other input devices, including multi-touch panel, tablet pad,
     * joystick, and so on. We call these messages as 'extra input messages'.
     * The messages can be classified the following types:
     *
     * - Axis messages: the messages generated by a pointer axis like mouse wheel.
     * - Button messages: the messages generated by a button on joystick.
     * - Multi-touch messages: the messages generated by a multi-touch panel.
     * - Gesture messages: the gesture messages.
     * - Tablet tool messages: the messages generated by a tablet tool.
     * - Tablet pad messages: the messages generated by a tablet pad.
     * - Switch messages: the messages generated by a switch.
     * - User-defined messages: the messages generated by a user-defined device.
     *
     * Note that the buttons other than left, right, and middle buttons on a mouse
     * will be treated as generic buttons.
     *
     * Since 4.0.0.
     *
     * @{
     */

    /* Group 4 from 0x0070 to 0x009F, the extra input messages. */
#define MSG_FIRSTEXTRAINPUTMSG  0x0070

#define AXIS_SCROLL_INVALID     0
#define AXIS_SCROLL_VERTICAL    1
#define AXIS_SCROLL_HORIZONTAL  2

#define AXIS_SOURCE_INVALID     0
#define AXIS_SOURCE_WHEEL       1
#define AXIS_SOURCE_FINGER      2
#define AXIS_SOURCE_CONTINUOUS  3
#define AXIS_SOURCE_WHEEL_TILT  4

/**
 * \def MSG_EXIN_AXIS
 * \brief Indicates an axis input event.
 *
 * This message is sent to the active window when the user
 * operates the axis of a pointer device such as a mouse.
 *
 * \code
 * MSG_EXIN_AXIS
 * int scroll = LOSWORD(wParam);
 * int source = HISWORD(wParam);
 * int value  = LOSWORD(lParam);
 * int value_discrete = HISWORD(lParam);
 * \endcode
 *
 * \param scroll one of AXIS_SCROLL_VERTICAL or AXIS_SCROLL_HORIZONTAL
 * \param source one of AXIS_SOURCE_WHEEL, AXIS_SOURCE_FINGER,
 *      AXIS_SOURCE_CONTINUOUS, or AXIS_SOURCE_WHEEL_TILT.
 * \param value The axis value.
 * \param value_discrete The axis value in discrete steps.
 */
#define MSG_EXIN_AXIS                   0x0070

/**
 * \def MSG_EXIN_BUTTONDOWN
 * \brief Indicates the user has pressed a button on joystick or
 * other input device.
 *
 * This message is sent to the active window when the user
 * pressed a button on joystick or other input device.
 *
 * \code
 * MSG_EXIN_BUTTONDOWN
 * unsigned int button = (unsigned int)wParam;
 * unsigned int nr_down_btns = (unsigned int)lParam;
 * \endcode
 *
 * \param button The button value. On Linux, the button values are defined
 *      in <linux/input-event-codes.h> file, and with `BTN_` prefix.
 * \param nr_down_btns The total number of buttons pressed.
 */
#define MSG_EXIN_BUTTONDOWN             0x0071

/**
 * \def MSG_EXIN_BUTTONUP
 * \brief Indicates the user has released a button on joystick or
 * other input device.
 *
 * This message is sent to the active window when the user
 * released a button on joystick or other input device.
 *
 * \code
 * MSG_EXIN_BUTTONUP
 * unsigned int button = (unsigned int)wParam;
 * unsigned int nr_down_btns = (unsigned int)lParam;
 * \endcode
 *
 * \param button The button value. On Linux, the button values are defined
 *      in <linux/input-event-codes.h> file, and with `BTN_` prefix.
 * \param nr_down_btns The total number of buttons pressed.
 */
#define MSG_EXIN_BUTTONUP               0x0072

/**
 * \def MSG_EXIN_TOUCH_DOWN
 * \brief Indicates a touch down event.
 *
 * \code
 * MSG_EXIN_TOUCH_DOWN
 * int x = LOSWORD(lParam);
 * int y = HISWORD(lParam);
 * \endcode
 *
 * \param x,y The position of touch.
 */
#define MSG_EXIN_TOUCH_DOWN             0x0073

/**
 * \def MSG_EXIN_TOUCH_UP
 * \brief Indicates a touch up event.
 *
 * Note that this message has not any parameters.
 */
#define MSG_EXIN_TOUCH_UP               0x0074

/**
 * \def MSG_EXIN_TOUCH_MOTION
 * \brief Indicates a touch move event.
 *
 * \code
 * MSG_EXIN_TOUCH_MOTION
 * int x = LOSWORD(lParam);
 * int y = HISWORD(lParam);
 * \endcode
 *
 * \param x,y The position of touch.
 */
#define MSG_EXIN_TOUCH_MOTION           0x0075

/**
 * \def MSG_EXIN_TOUCH_CANCEL
 * \brief Indicates a cancelled touch event.
 *
 * Note that this message has not any parameters.
 */
#define MSG_EXIN_TOUCH_CANCEL           0x0076

/**
 * \def MSG_EXIN_TOUCH_FRAME
 * \brief Indicates the end of a set of touchpoints at one device sample time.
 *
 * \code
 * MSG_EXIN_TOUCH_FRAME
 * int slot = (int)wParam;
 * unsigned int seat_slot = (unsigned int)lParam;
 * \endcode
 *
 * \param slot The slot of the touch event.
 *      Please see the Linux kernel's multitouch protocol B documentation
 *      for more information. If the touch event has no assigned slot, for example,
 *      if it is from a single touch device, slot will be -1.
 * \param seat_slot The seat slot of the touch event.
 *      A seat slot is a non-negative seat wide unique identifier
 *      of an active touch point.
 */
#define MSG_EXIN_TOUCH_FRAME            0x0077

#define SWITCH_INVALID                  0
#define SWITCH_LID                      1
#define SWITCH_TABLET_MODE              2

#define SWITCH_STATE_INVALID            0
#define SWITCH_STATE_ON                 1
#define SWITCH_STATE_OFF                2

/**
 * \def MSG_EXIN_SWITCH_TOGGLE
 * \brief Indicates the toggle event of a switch.
 *
 * \code
 * MSG_EXIN_SWITCH_TOGGLE
 * int switch_id = wParam;
 * int switch_state = lParam;
 * \endcode
 *
 * \param switch_id The identifier of the switch, one of SWITCH_LID
 *      or SWITCH_TABLET_MODE.
 * \param switch_state The state of the switch, one of SWITCH_STATE_ON
 *      or SWITCH_STATE_OFF.
 */
#define MSG_EXIN_SWITCH_TOGGLE          0x007A

/**
 * \def MSG_EXIN_GESTURE_SWIPE_BEGIN
 * \brief Indicates the beginning of a swipe gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_SWIPE_BEGIN
 * int nr_figures = wParam;
 * \endcode
 *
 * \param nr_figures The number of fingers used for the gesture.
 */
#define MSG_EXIN_GESTURE_SWIPE_BEGIN    0x0080

/**
 * \def MSG_EXIN_GESTURE_SWIPE_UPDATE
 * \brief Indicates update of a swipe gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_SWIPE_UPDATE
 * int nr_figures = (int)wParam;
 * int dx = LOSWORD(lParam);
 * int dy = HISWORD(lParam);
 * \endcode
 *
 * \param nr_figures The number of fingers used for the gesture.
 * \param dx, dy The motion delta between the last and the current
 *      MSG_EXIN_GESTURE_SWIPE_UPDATE message.
 */
#define MSG_EXIN_GESTURE_SWIPE_UPDATE   0x0081

/**
 * \def MSG_EXIN_GESTURE_SWIPE_END
 * \brief Indicates the end of a swipe gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_SWIPE_END
 * int nr_figures = (int)wParam;
 * BOOL is_cancelled = (BOOL)lParam;
 * \endcode
 *
 * \param nr_figures The number of fingers used for the gesture.
 * \param is_cancelled TRUE if the gesture ended normally, or if it was cancelled.
 */
#define MSG_EXIN_GESTURE_SWIPE_END      0x0082

/**
 * \def MSG_EXIN_GESTURE_PINCH_BEGIN
 * \brief Indicates the beginning of a pinch gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_PINCH_BEGIN
 * int nr_figures = (int)wParam;
 * unsigned int scale = (unsigned int)lParam;
 * \endcode
 *
 * \param nr_figures The number of fingers used for the gesture.
 * \param scale The absolute scale of a pinch gesture.
 *      The scale is the division of the current distance between
 *      the fingers and the distance at the start of the gesture.
 *      Note that the initial scale value is 100.
 */
#define MSG_EXIN_GESTURE_PINCH_BEGIN    0x0083

/**
 * \def MSG_EXIN_GESTURE_PINCH_UPDATE
 * \brief Indicates the beginning of a pinch gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_PINCH_UPDATE
 * unsigned int scale = LOWORD(wParam);
 * int da = HISWORD(wParam);
 * int dx = LOSWORD(lParam);
 * int dy = HISWORD(lParam);
 * \endcode
 *
 * \param scale The absolute scale of a pinch gesture.
 *      The scale is the division of the current distance between
 *      the fingers and the distance at the start of the gesture.
 *      Note that the initial scale value is 100.
 * \param da The angle delta in 1/50 degrees between the last and the current
 *      MSG_EXIN_GESTURE_PINCH_UPDATE message.
 * \param dx, dy The motion delta between the last and the current
 *      MSG_EXIN_GESTURE_PINCH_UPDATE message.
 */
#define MSG_EXIN_GESTURE_PINCH_UPDATE   0x0084

/**
 * \def MSG_EXIN_GESTURE_PINCH_END
 * \brief Indicates the end of a swipe gesture.
 *
 * \code
 * MSG_EXIN_GESTURE_PINCH_END
 * int nr_figures = (int)LOSWORD(wParam);
 * BOOL is_cancelled = (BOOL)HISWORD(wParam);
 * unsigned int scale = (unsigned int)lParam;
 * \endcode
 *
 * \param nr_figures The number of fingers used for the gesture.
 * \param is_cancelled TRUE if the gesture ended normally, or if it was cancelled.
 * \param scale The absolute scale of a pinch gesture.
 *      The scale is the division of the current distance between
 *      the fingers and the distance at the start of the gesture.
 *      Note that the initial scale value is 100.
 */
#define MSG_EXIN_GESTURE_PINCH_END      0x0085

/**
 * \def MSG_EXIN_END_CHANGES
 * \brief Indicates the end of one or more parameter changes
 *      of one hardware event.
 *
 * \code
 * MSG_EXIN_END_CHANGES
 * int count = (int)wParam;
 * \endcode
 *
 * \param count The count of parameters changed.
 */
#define MSG_EXIN_END_CHANGES            0x008F

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

#define TABLET_TOOL_PROXIMITY_STATE_INVALID 0
#define TABLET_TOOL_PROXIMITY_STATE_OUT     1
#define TABLET_TOOL_PROXIMITY_STATE_IN      2

#define TABLET_TOOL_TIP_INVALID         0
#define TABLET_TOOL_TIP_UP              1
#define TABLET_TOOL_TIP_DOWN            2

#define TABLET_BUTTON_STATE_INVALID     0
#define TABLET_BUTTON_STATE_RELEASED    1
#define TABLET_BUTTON_STATE_PRESSED     2

/**
 * \def MSG_EXIN_TABLET_TOOL_AXIS
 * \brief Indicates an axis of the tablet tool has changed state.
 *
 * For a tablet tool, one or more axes may changed in one hardware event.
 * MiniGUI will send one or more MSG_EXIN_TABLET_TOOL_XXX messages followed
 * by a MSG_EXIN_END_CHANGES message to the current active window.
 * Each MSG_EXIN_TABLET_TOOL_XXX message for a changed axis.
 *
 * \code
 * MSG_EXIN_TABLET_TOOL_AXIS
 * int which = (int)wParam;
 * long value = (long)lParam;
 * \endcode
 *
 * \param which The axis identifier, can be one of the following identifiers:
 *      - TABLET_TOOL_X:\n
 *          The value contains the current absolute x coordinate of
 *          the tablet tool, transformed to screen coordinates,
 *          but scaled 10 times.
 *      - TABLET_TOOL_Y:\n
 *          The value containsthe current absolute y coordinate of
 *          the tablet tool, transformed to screen coordinates,
 *          but scaled 10 times.
 *      - TABLET_TOOL_PRESSURE:\n
 *          The value contains the current pressure being applied on
 *          the tool in use, normalized to the range [0, 1000].
 *      - TABLET_TOOL_DISTANCE:\n
 *          The value contains the current distance from the tablet's sensor,
 *          normalized to the range [0, 1000].
 *      - TABLET_TOOL_TILT_X:\n
 *          The value contains the current tilt along the X axis of the
 *          tablet's current logical orientation, in 1/50 degrees off the
 *          tablet's z axis.
 *      - TABLET_TOOL_TILT_Y:\n
 *          The value contains the current tilt along the Y axis of the
 *          tablet's current logical orientation, in 1/50 degrees off the
 *          tablet's z axis.
 *      - TABLET_TOOL_ROTATION:\n
 *          The value contains the current z rotation of the tool in
 *          1/50 degrees, clockwise from the tool's logical neutral position.
 *      - TABLET_TOOL_SLIDER:\n
 *          The value contains the current position of the slider on the tool,
 *          normalized to the range [-1000, 1000].
 *      - TABLET_TOOL_WHEEL:\n
 *          The value contains the delta for the wheel in 1/50 degrees.
 * \param value The value of the axis.
 */
#define MSG_EXIN_TABLET_TOOL_AXIS       0x0090

/**
 * \def MSG_EXIN_TABLET_TOOL_PROXIMITY
 * \brief Indicates that a tool has come in or out of proximity of the tablet.
 *
 * For a tablet tool, one or more axes may changed in one hardware event.
 * MiniGUI will send one or more MSG_EXIN_TABLET_TOOL_XXX messages followed
 * by a MSG_EXIN_END_CHANGES message to the current active window.
 * Each MSG_EXIN_TABLET_TOOL_XXX message for a changed axis.
 *
 * \code
 * MSG_EXIN_TABLET_TOOL_PROXIMITY
 * int which = (int)LOSWORD(wParam);
 * int state = (int)HISWORD(wParam);
 * long value = (long)lParam;
 * \endcode
 *
 * \param which The axis identifier, please see \sa MSG_EXIN_TABLET_TOOL_AXIS.
 * \param state The proximity state, can be TABLET_TOOL_PROXIMITY_STATE_OUT
 *      or TABLET_TOOL_PROXIMITY_STATE_IN.
 * \param value The value of the axis.
 */
#define MSG_EXIN_TABLET_TOOL_PROXIMITY  0x0091

/**
 * \def MSG_EXIN_TABLET_TOOL_TIP
 * \brief Indicates that a tool has come in contact with the surface of the tablet.
 *
 * For a tablet tool, one or more axes may changed in one hardware event.
 * MiniGUI will send one or more MSG_EXIN_TABLET_TOOL_XXX messages followed
 * by a MSG_EXIN_END_CHANGES message to the current active window.
 * Each MSG_EXIN_TABLET_TOOL_XXX message for a changed axis.
 *
 * \code
 * MSG_EXIN_TABLET_TOOL_TIP
 * int which = (int)LOSWORD(wParam);
 * int state = (int)HISWORD(wParam);
 * long value = (long)lParam;
 * \endcode
 *
 * \param which The axis identifier, please see \sa MSG_EXIN_TABLET_TOOL_AXIS.
 * \param state The tip state, can be TABLET_TOOL_TIP_UP
 *      or TABLET_TOOL_TIP_DOWN.
 * \param value The value of the axis.
 */
#define MSG_EXIN_TABLET_TOOL_TIP        0x0092

/**
 * \def MSG_EXIN_TABLET_TOOL_BUTTON
 * \brief Indicates that a tool has changed a logical button state on the tablet.
 *
 * \code
 * MSG_EXIN_TABLET_TOOL_BUTTON
 * unsigned int button = (unsigned int)wParam;
 * int state = (int)lParam;
 * \endcode
 *
 * \param button The button identifier, which is a semantic button code as defined
 *      in <linux/input.h>.
 * \param state The button state, can be TABLET_BUTTON_STATE_RELEASED or
 *      TABLET_BUTTON_STATE_PRESSED.
 */
#define MSG_EXIN_TABLET_TOOL_BUTTON     0x0093

/**
 * \def MSG_EXIN_TABLET_PAD_BUTTON
 * \brief Indicates that a button pressed on the tablet pad.
 *
 * \code
 * MSG_EXIN_TABLET_PAD_BUTTON
 * unsigned int mode = (unsigned int)wParam;
 * unsigned int button_number = (unsigned int)LOSWORD(lParam);
 * int button_state = (int)HISWORD(lParam);
 * \endcode
 *
 * \param mode The mode the button that triggered this event is in.
 * \param button_number The button number, which is NOT a semantic button code
 *      as defined in <linux/input.h>.
 * \param button_state The button state, can be TABLET_BUTTON_STATE_RELEASED or
 *      TABLET_BUTTON_STATE_PRESSED.
 */
#define MSG_EXIN_TABLET_PAD_BUTTON      0x0094

#define TABLET_PAD_RING_SOURCE_UNKNOWN  0
#define TABLET_PAD_RING_SOURCE_FINGER   1

/**
 * \def MSG_EXIN_TABLET_PAD_RING
 * \brief Indicates that a status change on the tablet ring.
 *
 * \code
 * MSG_EXIN_TABLET_PAD_RING
 * unsigned int mode = (unsigned int)LOWORD(wParam);
 * unsigned int source = (unsigned int)HIWORD(wParam);
 * int position = (int)LOSWORD(lParam);
 * unsigned int number = (unsigned int)HIWORD(lParam);
 * \endcode
 *
 * \param mode The mode the ring that triggered this event is in.
 * \param source The source of the event, can be TABLET_PAD_RING_SOURCE_UNKNOWN
 *      or TABLET_PAD_RING_SOURCE_FINGER.
 * \param position The current position of the ring, in 1/50 degrees
 *      counterclockwise from the northern-most point of the ring in
 *      the tablet's current logical orientation.
 *      When the source is TABLET_PAD_RING_SOURCE_FINGER and the finger is lifted
 *      from the ring the value will be less than 0.
 * \param number The number of the ring that has changed state,
 *      with 0 being the first ring
 */
#define MSG_EXIN_TABLET_PAD_RING        0x0095

#define TABLET_PAD_STRIP_SOURCE_UNKNOWN 0
#define TABLET_PAD_STRIP_SOURCE_FINGER  1

/**
 * \def MSG_EXIN_TABLET_PAD_STRIP
 * \brief Indicates that a status change on the tablet strip.
 *
 * \code
 * MSG_EXIN_TABLET_PAD_STRIP
 * unsigned int mode = (unsigned int)LOWORD(wParam);
 * unsigned int source = (unsigned int)HIWORD(wParam);
 * int position = (int)LOSWORD(lParam);
 * unsigned int number = (unsigned int)HIWORD(lParam);
 * \endcode
 *
 * \param mode The mode the strip that triggered this event is in.
 * \param source The source of the event, can be TABLET_PAD_STRIP_SOURCE_UNKNOWN
 *      or TABLET_PAD_STRIP_SOURCE_FINGER.
 * \param position The current position of the strip,
 *      normalized to the range [0, 100].
 *      When the source is TABLET_PAD_STRIP_SOURCE_FINGER and the finger is lifted
 *      from the strip, the value will be less than 0.
 * \param number The number of the strip that has changed state,
 *      with 0 being the first strip.
 */
#define MSG_EXIN_TABLET_PAD_STRIP       0x0096

/**
 * \def MSG_EXIN_USER_BEGIN
 * \brief Indicates that the beginning of a user-defined extra input event.
 *
 * \code
 * MSG_EXIN_USER_BEGIN
 * WPARAM wParam;
 * LPARAM lParam;
 * \endcode
 *
 * \param wParam The first parameter of this message.
 * \param lParam The second parameter of this message.
 */
#define MSG_EXIN_USER_BEGIN             0x009A

/**
 * \def MSG_EXIN_USER_UPDATE
 * \brief Indicates that the update of a user-defined extra input event.
 *
 * \code
 * MSG_EXIN_USER_UPDATE
 * WPARAM wParam;
 * LPARAM lParam;
 * \endcode
 *
 * \param wParam The first parameter of this message.
 * \param lParam The second parameter of this message.
 */
#define MSG_EXIN_USER_UPDATE            0x009B

/**
 * \def MSG_EXIN_USER_END
 * \brief Indicates that the end of a user-defined extra input event.
 *
 * \code
 * MSG_EXIN_USER_END
 * WPARAM wParam;
 * LPARAM lParam;
 * \endcode
 *
 * \param wParam The first parameter of this message.
 * \param lParam The second parameter of this message.
 */
#define MSG_EXIN_USER_END               0x009C

#define MSG_LASTEXTRAINPUTMSG           0x009F

    /** @} end of extra_input_msgs */

    /**
     * \defgroup paint_msgs Window painting messages
     * @{
     */

/* Group 5 from 0x00A0 to 0x00CF, the paint messages. */
#define MSG_FIRSTPAINTMSG   0x00A0

/**
 * \def MSG_SHOWWINDOW
 * \brief Indicates that the window has been shown or hidden.
 *
 * This message is sent to the window as a notification after the window
 * has been shown or hidden (due to the calling of the function ShowWindow).
 *
 * \code
 * MSG_SHOWWINDOW
 * int show_cmd = (int)wParam;
 * \endcode
 *
 * \param show_cmd The command to show or hide, can be one of
 * the following values:
 *      - SW_SHOW\n
 *        Shows the window.
 *      - SW_HIDE\n
 *        Hides the window.
 *      - SW_SHOWNORMAL\n
 *        Shows the window, and if the window is a main window
 *        sets it to be the topmost main window in its z-node level.
 *
 * \sa ShowWindow
 */
#define MSG_SHOWWINDOW      0x00A0

/**
 * \def MSG_MOVEWINDOW
 * \brief Indicates that the window has been moved.
 *
 * This message is sent to the window as a notification after the window
 * has been moved (due to the calling of the function MoveWindow).
 *
 * \code
 * MSG_MOVEWINDOW
 * int lx = LOSWORD(wParam);
 * int ty = HISWORD(wParam);
 * int rx = LOSWORD(lParam);
 * int by = HISWORD(lParam);
 * \endcode
 *
 * \param lx, ty, rx, by The new rectangle coordinates of the window.
 *
 * \sa MoveWindow
 *
 * Since 5.0.0
 */
#define MSG_MOVEWINDOW      0x00A1

/**
 * \def MSG_ERASEBKGND
 * \brief Sent to the window to erase the background.
 *
 * This message is sent to the window if the whole or a part of the background
 * should be erased.
 *
 * \code
 * MSG_ERASEBKGND
 * HDC hdc = (HDC)wParam;
 * const RECT* inv_rect = (const RECT*)lParam;
 * \endcode
 *
 * \param hdc The device context.
 * \param inv_rect The pointer to a RECT structure contains the rectangle
 *        should be erase. The rectangle is in client coordinates system.
 *        If it is NULL, the whole client area should be erased.
 */
#define MSG_ERASEBKGND      0x00B0      /* this is an async message */

/**
 * \def MSG_PAINT
 * \brief Sent to the window if the window contains an invalid region.
 *
 * This message is sent to the window if the window contains an invalid region.
 *
 * \code
 * MSG_PAINT
 * const CLIPRGN* inv_rgn = (const CLIPRGN*) lParam;
 * \endcode
 *
 * \param inv_rgn The pointer to the invalid region of the window.
 */
#define MSG_PAINT           0x00B1

/**
 * \def MSG_NCPAINT
 * \brief Indicates that paints non-client area.
 *
 * \code
 * MSG_NCPAINT
 * HDC hdc = (HDC)wParam;
 * const RECT* inv_rect = (const RECT*)lParam;
 * \endcode
 *
 * \param hdc The device context.
 * \param inv_rect The pointer to a RECT structure contains the rectangle
 *        should be paint. The rectangle is in client coordinates system.
 *        If it is NULL, the whole nc client area should be paint.
 */
#define MSG_NCPAINT         0x00B2

/**
 * \def MSG_NCACTIVATE
 * \brief Indicates that active non-client area of main window.
 */
#define MSG_NCACTIVATE      0x00B3

/**
 * \def MSG_SYNCPAINT
 * \brief Indicates that actives and paints main window synchronously.
 */
#define MSG_SYNCPAINT       0x00B4

#define MSG_LASTPAINTMSG    0x00BF

    /** @} end of paint_msgs */

    /**
     * \defgroup desktop_msgs Internal desktop messages
     * @{
     */

/* Group 6 from 0x00C0 to 0x00EF, the internal desktop messages. */
#define MSG_FIRSTSESSIONMSG 0x00C0

#define MSG_STARTSESSION    0x00C0
#define MSG_QUERYENDSESSION 0x00C1
#define MSG_ENDSESSION      0x00C2
#define MSG_REINITSESSION   0x00C3

/* Since 5.0.0 */
#define MSG_REINITDESKOPS   0x00C4

#define MSG_ERASEDESKTOP    0x00CE
#define MSG_PAINTDESKTOP    0x00CF

#define MSG_DT_MOUSEOFF     0x00D0

/**
 * \def MSG_DT_LBUTTONDOWN
 * \brief Left mouse button down message on the desktop.
 *
 * This message is posted to the desktop window when the user presses down
 * the left button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_LBUTTONDOWN
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_LBUTTONUP, MSG_LBUTTONDOWN
 */
#define MSG_DT_LBUTTONDOWN      0x00D1

/**
 * \def MSG_DT_LBUTTONUP
 * \brief Left mouse button up message on the desktop.
 *
 * This message is posted to the desktop window when the user releases up
 * the left button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_LBUTTONUP
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_LBUTTONDOWN, MSG_LBUTTONUP
 */
#define MSG_DT_LBUTTONUP        0x00D2

/**
 * \def MSG_DT_LBUTTONDBLCLK
 * \brief Left mouse button double clicked message on the desktop.
 *
 * This message is posted to the desktop window when the user double clicks
 * the left button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_LBUTTONDBLCLK
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in client coordinates.
 *
 * \sa MSG_RBUTTONDBLCLK
 */
#define MSG_DT_LBUTTONDBLCLK    0x00D3

/**
 * \def MSG_DT_MOUSEMOVE
 * \brief The mouse moved message on the desktop.
 *
 * This message is posted to the desktop window when the user moves the mouse
 * in the area of the desktop window.
 *
 * \code
 * MSG_DT_MOUSEMOVE
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_MOUSEMOVE
 */
#define MSG_DT_MOUSEMOVE        0x00D4

/**
 * \def MSG_DT_RBUTTONDOWN
 * \brief Right mouse button down message on the desktop.
 *
 * This message is posted to the desktop window when the user presses down
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_RBUTTONDOWN
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_RBUTTONUP, MSG_RBUTTONDOWN
 *
 */
#define MSG_DT_RBUTTONDOWN      0x00D5

/**
 * \def MSG_DT_RBUTTONUP
 * \brief Right mouse button up message on the desktop.
 *
 * This message is posted to the desktop window when the user releases up
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_RBUTTONUP
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_RBUTTONDOWN, MSG_RBUTTONUP
 */
#define MSG_DT_RBUTTONUP        0x00D6

/**
 * \def MSG_DT_RBUTTONDBLCLK
 * \brief Right mouse button double clicked message on the desktop.
 *
 * This message is posted to the desktop window when the user double clicks
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_RBUTTONDBLCLK
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_LBUTTONDBLCLK
 */
#define MSG_DT_RBUTTONDBLCLK    0x00D7

/**
 * \def MSG_DT_MBUTTONDOWN
 * \brief Right mouse button down message on the desktop.
 *
 * This message is posted to the desktop window when the user presses down
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_MBUTTONDOWN
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_MBUTTONUP, MSG_RBUTTONDOWN
 *
 */
#define MSG_DT_MBUTTONDOWN      0x00D8

/**
 * \def MSG_DT_MBUTTONUP
 * \brief Right mouse button up message on the desktop.
 *
 * This message is posted to the desktop window when the user releases up
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_MBUTTONUP
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_DT_MBUTTONDOWN, MSG_RBUTTONUP
 */
#define MSG_DT_MBUTTONUP        0x00D9

/**
 * \def MSG_DT_MBUTTONDBLCLK
 * \brief Right mouse button double clicked message on the desktop.
 *
 * This message is posted to the desktop window when the user double clicks
 * the right button of the mouse in the area of the desktop window.
 *
 * \code
 * MSG_DT_MBUTTONDBLCLK
 * int x_pos = LOSWORD (lParam);
 * int y_pos = HISWORD (lParam);
 * \endcode
 *
 * \param x_pos,y_pos The position of the mouse in desktop coordinates.
 *
 * \sa MSG_LBUTTONDBLCLK
 */
#define MSG_DT_MBUTTONDBLCLK    0x00DA

#define MSG_DT_KEYOFF           0x00CB

/**
 * \def MSG_DT_KEYDOWN
 * \brief User presses a key down on the desktop.
 *
 * This message is posted to the desktop window when the user
 * presses a key down.
 *
 * \code
 * MSG_DT_KEYDOWN
 * int scancode = (int)wParam;
 * \endcode
 *
 * \param scancode The scan code of the pressed key.
 *
 * \sa MSG_DT_KEYUP
 *
 */
#define MSG_DT_KEYDOWN          0x00E0

#define MSG_DT_CHAR             0x00E1

/**
 * \def MSG_DT_KEYUP
 * \brief User releases up a key on the desktop.
 *
 * This message is posted to the desktop window when the user
 * releases up a key.
 *
 * \code
 * MSG_DT_KEYUP
 * int scancode = (int)wParam;
 * \endcode
 *
 * \param scancode The scan code of the released key.
 *
 * \sa MSG_DT_KEYDOWN
 */
#define MSG_DT_KEYUP            0x00E2

#define MSG_DT_SYSKEYDOWN       0x00E3
#define MSG_DT_SYSCHAR          0x00E4
#define MSG_DT_SYSKEYUP         0x00E5

#define MSG_DT_KEYLONGPRESS     0x00E6
#define MSG_DT_KEYALWAYSPRESS   0x00E7

#define MSG_LASTSESSIONMSG      0x00EF

    /** @} end of desktop_msgs */

    /**
     * \defgroup window_msgs Internal window management messages
     * @{
     */

/* Group 7 from 0x00F0 to 0x010F, Internal window management messages. */
#define MSG_FIRSTWINDOWMSG  0x00F0

#define MSG_ADDNEWMAINWIN   0x00F0
#define MSG_REMOVEMAINWIN   0x00F1
#define MSG_MOVETOTOPMOST   0x00F2
#define MSG_SETACTIVEMAIN   0x00F3
#define MSG_GETACTIVEMAIN   0x00F4
#define MSG_SHOWMAINWIN     0x00F5
#define MSG_HIDEMAINWIN     0x00F6
#define MSG_MOVEMAINWIN     0x00F7
#define MSG_SETCAPTURE      0x00F8
#define MSG_GETCAPTURE      0x00F9

#define MSG_ENDTRACKMENU    0x00FA
#define MSG_TRACKPOPUPMENU  0x00FB
#define MSG_CLOSEMENU       0x00FC
#define MSG_SCROLLMAINWIN   0x00FD
#define MSG_CARET_CREATE    0x00FE
#define MSG_CARET_DESTROY   0x00FF

#define MSG_ENABLEMAINWIN   0x0100
#define MSG_ISENABLED       0x0101

#define MSG_SETWINCURSOR    0x0102

#define MSG_GETNEXTMAINWIN  0x0103

/* Since 5.0.0 */
#define MSG_SETALWAYSTOP    0x0104

typedef struct _COMPOSITINGINFO {
    int     type;
    DWORD   arg;
} COMPOSITINGINFO;

/* Since 5.0.0 */
#define MSG_SETCOMPOSITING  0x0105

/* Since 5.0.0 */
#define MSG_DUMPZORDER      0x0106

/* Since 5.0.0 */
#define MSG_SETAUTOREPEAT   0x0107

/* Since 5.0.6 */
#define MSG_SETGESTUREFLAGS 0x0108

#define MSG_SHOWGLOBALCTRL  0x010A
#define MSG_HIDEGLOBALCTRL  0x010B

typedef struct _DRAGINFO {
    int     location;
    int     init_x, init_y;
} DRAGINFO;

/* client to server; wParam: hwnd, lParam: DRAGINFO. */
#define MSG_STARTDRAGWIN    0x010C
/* client to server; wParam: hwnd, lParam: 0L. */
#define MSG_CANCELDRAGWIN   0x010D

#define MSG_CHANGECAPTION   0x010E

struct _RECT4MASK;
typedef struct _WINMASKINFO {
    int                 nr_rcs;
    struct _RECT4MASK*  rcs;
} WINMASKINFO;

/* Since 5.0.0 */
#define MSG_SETWINDOWMASK   0x010F

#define MSG_LASTWINDOWMSG   0x010F

    /** @} end of window_msgs */

    /**
     * \defgroup ctrl_msgs Dialog and control messages
     * @{
     */

/* Group 8 from 0x0120 to 0x013F, the dialog and control messages. */
#define MSG_FIRSTCONTROLMSG 0x0120

/**
 * \def MSG_COMMAND
 * \brief The command message, indicates a notification message from child
 *        window, or the user has selected a menu item.
 *
 * This message sent to the window when the user has selected a menu item, or
 * a child window has sent a notification message to the parent.
 *
 * \code
 * MSG_COMMAND
 * int id = LOWORD(wParam);
 * int code = HIWORD(wParam);
 * HWND hwnd = (HWND)lParam;
 * \endcode
 *
 * \param id The identifier of the menu item or the child window.
 * \param code The notification code.
 * \param hwnd The handle to the control.
 *
 * \note If you use `MSG_COMMAND` message to handle the notification
 * sent from other windows, you should make sure the identifier
 * value and the notification code do not exceed half of the maximal
 * value of a DWORD (32-bit on 64-bit platform, and 16-bit on 32-bit
 * platform). If you use a pointer as the identifier,
 * the code above will not work.
 *
 * Instead, we recommend strongly that you use a NOTIFYPOROC
 * callback to handle the notification generated by a window.
 *
 * \sa NotifyWindow, NotifyParentEx, SetNotificationCallback 
 */
#define MSG_COMMAND         0x0120

/**
 * \def MSG_SYSCOMMAND
 * \brief The system command message.
 */
#define MSG_SYSCOMMAND      0x0121

/**
 * \def MSG_GETDLGCODE
 * \brief Get dialog code.
 */
#define MSG_GETDLGCODE      0x0122

/**
 * \def MSG_INITDIALOG
 * \brief Ready to initialize the controls in a dialog box.
 *
 * This message is sent to the dialog in order that you can initialize the
 * controls in the dialog box.
 *
 * \code
 * MSG_INITDIALOG
 * HWND focus_hwnd = (HWND)wParam;
 * LPARAM lparam = (LPARAM)lParam;
 * \endcode
 *
 * \param focus_hwnd The handle to the control which will gain the input focus.
 * \param lparam The parameter passed into the dialog box
 *        through \a DialogBoxIndirectParam function.
 *
 * \return Returns non-zero value to set the input focus to \a focus_hwnd,
 *         else do not set focus.
 *
 * \sa DialogBoxIndirectParam
 *
 * Example:
 *
 * \include msg_initdialog.c
 */
#define MSG_INITDIALOG      0x0123

/*Reserved*/
#define MSG_NEXTDLGCTRL     0x0124
#define MSG_ENTERIDLE       0x0125

/**
 * \def MSG_DLG_GETDEFID
 * \brief Get default push button ID first.
 */
#define MSG_DLG_GETDEFID    0x0126

/**
 * \def MSG_DLG_SETDEFID
 * \brief Set default push button ID first.
 */
#define MSG_DLG_SETDEFID    0x0127

/* #define MSG_DLG_REPOSITION  0x0128 */

/**
 * \def MSG_ISDIALOG
 * \brief Send to a window to query whether the window is a dialog window.
 *
 * \note This is a asynchronical message.
 */
#define MSG_ISDIALOG        0x0128

/**
 * \def MSG_INITPAGE
 * \brief Ready to initialize the controls in a property page.
 *
 * This message is sent to the page in order that you can initialize the
 * controls in the page.
 *
 * \code
 * MSG_INITPAGE
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param add_data The additional data in DLGTEMPLATE structure passed
 *        through PSM_ADDPAGE message.
 *
 * \sa ctrl_propsheet
 */
#define MSG_INITPAGE        0x0129

/**
 * \def MSG_SHOWPAGE
 * \brief Indicates the page will be shown or hidden.
 *
 * This message is sent to the page when the page will be shown or hidden.
 *
 * \code
 * MSG_SHOWPAGE
 * HWND focus_hwnd = (HWND)wParam;
 * int show_cmd = (int)lParam;
 * \endcode
 *
 * \param focus_hwnd The handle to the child which will gain the input focus
 *        if showing the page.
 * \param show_cmd The show command, can be one of the following values:
 *      - SW_SHOW\n The page will be shown.
 *      - SW_HIDE\n The page will be hidden.
 *
 * \sa ctrl_propsheet
 */
#define MSG_SHOWPAGE        0x012A

/**
 * \def MSG_SHEETCMD
 * \brief Indicates that a PSM_SHEETCMD message had been sent to the
 * PropertySheet control.
 *
 * This message is sent to the property page when the property sheet contains
 * the page received the PSM_SHEETCMD message.
 *
 * \code
 * MSG_SHEETCMD
 * WPARAM param1 = wParam;
 * LPARAM param2 = lParam;
 * \endcode
 *
 * \param param1 The wParam of PSM_SHEETCMD message.
 * \param param2 The lParam of PSM_SHEETCMD message.
 *
 * \sa ctrl_propsheet, PSM_SHEETCMD
 */
#define MSG_SHEETCMD        0x012B

/**
 * \def MSG_INITCONTAINER
 * \brief This message will be sent to the container window procedure after
 * the container window is created.
 *
 * This message is sent to the container in order that you can initialize the
 * controls in the container.
 *
 * \code
 * MSG_INITCONTAINER
 * DWORD add_data = (DWORD)lParam;
 * \endcode
 *
 * \param add_data The additional data in \a CONTAINERINFO structure passed
 *        through the argument of \a dwAddData when creating the ScrollView
 *        control by calling CreateWindowEx.
 *
 * \sa ctrl_scrollview, CONTAINERINFO
 */
#define MSG_INITCONTAINER   0x012C

/**
 * \def MSG_SVCONTCMD
 * \brief This message will be sent to the parent of the ScrollView control
 *        when the container of the ScrollView control reveived a MSG_COMMAND
 *        message.
 *
 * This message will be sent to the parent of the ScrollView when
 * the container of the ScrollView control reveived a MSG_COMMAND message.
 * Generally, the notification of the child control in the container will be
 * sent via MSG_COMMAND to the container. If you have not defined your own
 * window procedure for the container, this message gives a chance for the
 * parent of the ScrollView control to handle the notifications come from
 * the controls in the container.
 *
 * Note that you can also define your window procedure for the container, and
 * handle the notification from the child control in this procedure.
 *
 * \code
 * MSG_SVCONTCMD
 * WPARAM param1 = wParam;
 * WPARAM param2 = lParam;
 * \endcode
 *
 * \param param1 The wParam of MSG_COMMAND message.
 * \param param2 The lParam of MSG_COMMAND message.
 *
 * \sa ctrl_scrollview, MSG_COMMAND
 */
#define MSG_SVCONTCMD       0x012D

/**
 * \def MSG_FREEZECTRL
 * \brief Send this message to freeze or thaw the paint action of
 *      the control.
 *
 * \code
 * MSG_FREEZECTRL
 * BOOL bFrozen;
 *
 * wParam = bFrozen
 * lParam = 0;
 *
 * \endcode
 *
 * \note Note that implemented only in scrollview and listview.
 *
 * \param bFrozen to freeze or to thaw.
 */
#define MSG_FREEZECTRL      0x012E

/**
 * \def MSG_FONTCHANGING
 * \brief Indicates the user is trying to change the font of the window.
 *
 * This message is sent to the window when the user is trying to change
 * the font of the window by calling \a SetWindowFont.
 * If you return non-zero after handling this message, \a SetWindowFont
 * will return immediately, i.e., the default window font will not change.
 *
 * \code
 * MSG_FONTCHANGING
 * PLOGFONT log_font = (PLOGFONT)lParam;
 * \endcode
 *
 * \param log_font The pointer to the new window logical font.
 *
 * \sa SetWindowFont
 */
#define MSG_FONTCHANGING    0x0130


/**
 * \def MSG_FONTCHANGED
 * \brief Indicates the window font has been changed.
 *
 * This message is sent to the window after the window font has changed.
 * Some window should be repainted to reflect the new window font.
 */
#define MSG_FONTCHANGED     0x0131

/**
 * \def MSG_GETTEXTLENGTH
 * \brief Send to the control or the main/virtual window to get the length
 *      of the text or caption.
 *
 * This message is sent to the control or the main/virtual window when you
 * call \a GetWindowTextLength function to get the lenght of the text or
 * the caption.
 *
 * \code
 * MSG_GETTEXTLENGTH
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The length of the text.
 *
 * \sa GetWindowTextLength
 */
#define MSG_GETTEXTLENGTH   0x0132

/**
 * \def MSG_GETTEXT
 * \brief Send to the control to get the text, or send to the main/virtual
 *      window to get the caption.
 *
 * This message is sent to the control or the main/virtual window when you
 * call \a GetWindowText function to get the text or the caption.
 *
 * \code
 * MSG_GETTEXT
 * int max_len;
 * char* text_buf;
 *
 * wParam = (WPARAM)max_len;
 * lParam = (LPARAM)text_buf;
 * \endcode
 *
 * \param max_len The maximal number of characters can be copied to the buffer.
 * \param text_buf The pointer to a buffer receives the text.
 * \return The length of the window text string.
 *
 * \sa GetWindowText
 */
#define MSG_GETTEXT         0x0133

/**
 * \def MSG_SETTEXT
 * \brief Send to the control to set the text, or send to the main/virtual
 *      window to set the caption.
 *
 * This message is sent to the control or the main/virtual window when you
 * call \a SetWindowText function to set the text or the caption.
 *
 * \code
 * MSG_SETTEXT
 * char* text_buf;
 *
 * wParam = 0;
 * lParam = (LPARAM)text_buf;
 * \endcode
 *
 * \param text_buf The pointer to a buffer contains the text.
 *
 * \return The return value is equal to zero if the text is set.
 *
 * \sa SetWindowText
 */
#define MSG_SETTEXT         0x0134

/**
 * \def MSG_ENABLE
 * \brief Indicates the window is disabled/enabled.
 *
 * This message is sent to the window if the window has been disabled or enabled.
 *
 * \code
 * MSG_ENABLE
 * BOOL enabled = (BOOL)wParam;
 * \endcode
 *
 * \param enabled Indicates whether the window was disabled or enabled.
 */
#define MSG_ENABLE          0x0135

/**
 * \def MSG_NOTIFICATION
 * \brief Indicate a notification message.
 *
 * This message is an internal message which indicates the message is a
 * notification sent by calling \a NotifyWindow or \a NotifyParentEx functions.
 *
 * For this message, \a DispatchMessage function will call the notification
 * callback procedure if the target window has been set the notification
 * callback procedure, or convert the message to a MSG_COMMAND message and
 * pass it to the window procedure.
 *
 * \code
 * MSG_NOTIFICATION
 * LINT id = (LINT)wParam;
 * int  nc = (int)lParam;
 * DWORD adData = msg->time;
 * \endcode
 *
 * Since 5.0.0.
 *
 * \sa MSG_COMMAND, SetNotificationCallback, NotifyWindow, NotifyParentEx
 */
#define MSG_NOTIFICATION    0x0136

#define MSG_LASTCONTROLMSG  0x013F

    /** @} end of ctrl_msgs */

    /**
     * \defgroup system_msgs System messages
     * @{
     */

/* Group 9 from 0x0140 to 0x016F, the system messages. */
#define MSG_FIRSTSYSTEMMSG  0x0140

#define MSG_QUIT            0x0140

/**
 * \def MSG_IDLE
 * \brief Indicates the system enters idle loop.
 *
 * This message is sent to the all main windows when the system
 * enters idle loop.
 */
#define MSG_IDLE            0x0142

#define MSG_TIMEOUT         0x0143

/**
 * \def MSG_TIMER
 * \brief Indicates a timer has expired.
 *
 * This message is sent to the window when a timer expired.
 *
 * \code
 * MSG_TIMER
 * LINT timer_id = (LINT)wParam;
 * DWORD tick_count = (DWORD)lParam;
 * \endcode
 *
 * \param timer_id The identifier of the timer has expired.
 * \param tick_count The tick count when the timer had expired.
 */
#define MSG_TIMER           0x0144

#define MSG_CARETBLINK      0x0145

#ifdef HAVE_SELECT

/**
 * \def MSG_FDEVENT
 * \brief Indicates an event of registered file descriptor occurred.
 *
 * You can use \a RegisterListenFD to register a file desciptor to
 * MiniGUI for listening.
 *
 * When there is a read/write/except event on the fd, MiniGUI
 * will post a MSG_FDEVENT message with \a wParam being equal to
 * MAKELONG (fd, type), and the \a lParam being set to be the context
 * to the target window.
 *
 * \code
 * MSG_FDEVENT
 * int fd = LOWORD(wParam);
 * int type = HIWORD(wParam);
 * void* context = (void*)lParam;
 * \endcode
 *
 * \param fd The listened file descriptor.
 * \param type The event type, can be Or'd with one or more of
 *      POLLIN, POLLOUT, and POLLERR.
 * \param context A context value.
 *
 * \note The file descriptor here was assumed as a unsigned short integer
 *      on a 32-bit platform.
 *
 * \sa RegisterListenFD
 */
#define MSG_FDEVENT         0x0146

#endif /* defined HAVE_SELECT */

#ifdef _MGRM_PROCESSES

/**
 * \def MSG_SRVNOTIFY
 * \brief Indicates a notification from the server of MiniGUI-Processes.
 *
 * This message will be broadcasted to all of the main windows
 * in a client process when the client receives a MSG_SRVNOTIFY message
 * from the server.
 *
 * The server, i.e. 'mginit' defines the meaning of two parameters of
 * this message.
 *
 * \note Only available on MiniGUI-Processes.
 */
#define MSG_SRVNOTIFY       0x0147

#define MSG_UPDATECLIWIN    0x0148

/* Since 5.0.0; the server send this message to the client
   if the client moved to a new layer */
#define MSG_LAYERCHANGED    0x0149

#endif /* defined _MGRM_PROCESSES */

/* Since 5.0.0: for managing message thread */
#define MSG_MANAGE_MSGTHREAD    0x014A
    #define MSGTHREAD_SIGNIN        0x00
    #define MSGTHREAD_SIGNOUT       0x01

/* Since 5.0.0: for calculating the default position */
#define MSG_CALC_POSITION       0x014B

/* Since 5.0.6: for waking up the client */
#define MSG_WAKEUP_CLIENT       0x014C

/**
 * \def MSG_DOESNEEDIME
 * \brief Send to a window to query whether the window needs to open
 *        IME window.
 *
 * The system will send this message when the window gain the input focus
 * to determine whether the window needs to open IME window.
 *
 * The application should handle this message and return TRUE when
 * the window need IME window.  Default window procedure returns FALSE.
 *
 * \note This is an asynchronous message.
 */
#define MSG_DOESNEEDIME     0x0150

/* flag code. */
#define IME_STATUS_REGISTERED   0x0000
#define IME_STATUS_ENABLED      0x0001
#define IME_STATUS_AUTOTRACK    0x0002
#define IME_STATUS_LANGUAGE     0x0003
    #define IME_LANGUAGE_UNKNOWN    0x0000
    #define IME_LANGUAGE_LATIN      0x0001
    #define IME_LANGUAGE_ZHCN       0x0002
    #define IME_LANGUAGE_ZHTW       0x0003
#define IME_STATUS_ENCODING     0x0004
    #define IME_ENCODING_LOCAL      0x0000
    #define IME_ENCODING_UTF8       0x0001
#define IME_STATUS_VERSION          0x0005
#define IME_STATUS_USER_MIN         0x0006
#define IME_STATUS_USER_MAX         0x0020

/* IME edit type */
#define IME_WINDOW_TYPE_NOT_EDITABLE    0x0000
#define IME_WINDOW_TYPE_EDITABLE        0x0001
#define IME_WINDOW_TYPE_READONLY        0x0002
#define IME_WINDOW_TYPE_PASSWORD        0x0003

/*Internal use*/
#define MSG_IME_REGISTER    0x0151
#define MSG_IME_UNREGISTER  0x0152
#define MSG_IME_OPEN        0x0153
#define MSG_IME_CLOSE       0x0154
#define MSG_IME_SETSTATUS   0x0156
#define MSG_IME_GETSTATUS   0x0157
#define MSG_IME_SETTARGET   0x0158
#define MSG_IME_GETTARGET   0x0159
#define MSG_IME_SETPOS      0x015A
#define MSG_IME_GETPOS      0x015B

/*  new ime msg. */
#define MSG_IME_SET_TARGET_INFO   0x015C
#define MSG_IME_GET_TARGET_INFO   0x015D

#define MSG_SHOWMENU        0x0160
#define MSG_HIDEMENU        0x0161

#define MSG_ADDTIMER        0x0162
#define MSG_REMOVETIMER     0x0163
#define MSG_RESETTIMER      0x0164

#define MSG_WINDOWCHANGED   0x0165

#define MSG_BROADCASTMSG    0x0166

#define MSG_REGISTERWNDCLASS    0x0167
#define MSG_UNREGISTERWNDCLASS  0x0168
#define MSG_NEWCTRLINSTANCE     0x0169
#define MSG_REMOVECTRLINSTANCE  0x016A
#define MSG_GETCTRLCLASSINFO    0x016B
#define MSG_CTRLCLASSDATAOP     0x016C
    #define CCDOP_GETCCI        0x01
    #define CCDOP_SETCCI        0x02

#define MSG_REGISTERHOOKFUNC    0x016D
#define MSG_REGISTERHOOKWIN     0x016E
#define MSG_UNREGISTERHOOKWIN   0x016F

#define MSG_LASTSYSTEMMSG   0x016F

    /** @} end of system_msgs */

    /**
     * \defgroup menu_msgs Internal menu messages
     * @{
     */

/* Group 10 from 0x0170 to 0x018F, the menu messages */
#define MSG_FIRSTMENUMSG    0x0170

/*Internal use*/
#define MSG_INITMENU        0x0170
#define MSG_INITMENUPOPUP   0x0171
#define MSG_MENUSELECT      0x0172
#define MSG_MENUCHAR        0x0173
#define MSG_ENTERMENULOOP   0x0174
#define MSG_EXITMENULOOP    0x0175
#define MSG_CONTEXTMENU     0x0176
#define MSG_NEXTMENU        0x0177

#define MSG_LASTMENUMSG     0x018F

    /** @} end of menu_msgs */

    /**
     * \defgroup user_msgs User-defined messages
     * @{
     */

#define MSG_FIRSTUSERMSG    0x0800


/**
 * \def MSG_USER
 * \brief The first user-defined message.
 *
 * MiniGUI reserved the range from 0x0800 to 0xEFFF for user-defined messages.
 * MSG_USER is the first user-defined message you can use it by your own.
 */
#define MSG_USER            0x0800

#define MSG_LASTUSERMSG     0xEFFF

    /** @} end of user_msgs */

    /** @} end of msgs */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \defgroup msg_fns Message functions
     * @{
     */

    /**
     * \defgroup msg_pass_fns Message passing functions
     * @{
     */

/**
 * The message structure.
 * \sa GetMessage, PostMessage, msgs
 */
typedef struct _MSG
{
    /** The handle to the window which receives this message. */
    HWND            hwnd;
    /** The message identifier. */
    UINT            message;
    /** The first parameter of the message
      (a unsigned integer with pointer precision). */
    WPARAM          wParam;
    /** The second parameter of the message
      (a unsigned integer with pointer precision). */
    LPARAM          lParam;
    /** Time (ticks when the message generated);
      reuse for the additional data of a notification. */
    DWORD           time;
#ifdef _MGHAVE_VIRTUAL_WINDOW
    /* pointer to the sync MSG (internal use) */
    void*           pSyncMsg;
#endif
} MSG;
typedef MSG* PMSG;

/**
 * \def PM_NOREMOVE
 *
 * \sa PeekMessage PeekMessageEx
 */
#define PM_NOREMOVE     0x0000

/**
 * \def PM_REMOVE
 *
 * \sa PeekMessage PeekMessageEx PeekPostMessage
 */
#define PM_REMOVE       0x0001

/**
 * \def PM_NOYIELD
 *
 * \sa PeekMessage PeekMessageEx PeekPostMessage
 */
#define PM_NOYIELD      0x0002

/**
 * \fn BOOL PeekMessageEx (PMSG pMsg, HWND hWnd, \
 *               UINT nMsgFilterMin, UINT nMsgFilterMax, \
 *               BOOL bWait, UINT uRemoveMsg)
 * \brief Peek a message from the message queue of a main window.
 *
 * This function peeks a message from the message queue of the window \a hWnd;
 * if \a bWait is TRUE, it will wait for the message, else return immediatly.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param nMsgFilterMin The min identifier of the message that should be peeked.
 * \param nMsgFilterMax The max identifier of the message that should be peeked.
 * \param bWait Whether to wait for a message.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekPostMessage, HavePendingMessage, PostMessage
 */
MG_EXPORT BOOL GUIAPI PeekMessageEx (PMSG pMsg, HWND hWnd,
                UINT nMsgFilterMin, UINT nMsgFilterMax,
                BOOL bWait, UINT uRemoveMsg);

/**
 * \fn BOOL GetMessage (PMSG pMsg, HWND hMainWnd)
 * \brief Get a message from the message queue of a main window.
 *
 * This function gets a message from the message queue of the main window
 * \a hMainWnd, and returns until there is a message in the message queue.
 *
 * \param pMsg Pointer to the result message.
 * \param hMainWnd Handle to the window.
 *
 * \return FALSE on MSG_QUIT have been found or on error, else gets a message.
 *
 * \sa HavePendingMessage, PostQuitMessage, MSG
 *
 * Example:
 *
 * \include getmessage.c
 */
static inline BOOL GUIAPI GetMessage (PMSG pMsg, HWND hWnd)
{
    return PeekMessageEx (pMsg, hWnd, 0, 0, TRUE, PM_REMOVE);
}

/**
 * \fn BOOL WaitMessage (PMSG pMsg, HWND hMainWnd)
 * \brief Wait for a message from the message queue of a main window.
 *
 * This function waits for a message from the message queue of the main
 * window \a hMainWnd, and returns until there is a message in the message
 * queue. Unlike \a GetMessage, this function does not remove the message
 * from the message queue.
 *
 * \param pMsg Pointer to the result message.
 * \param hMainWnd Handle to the window.
 *
 * \return 0 on MSG_QUIT have been found, else gets a message.
 *
 * \sa HavePendingMessage, PostQuitMessage, MSG
 */
MG_EXPORT BOOL GUIAPI WaitMessage (PMSG pMsg, HWND hMainWnd);

/**
 * \fn BOOL HavePendingMessage (HWND hMainWnd)
 * \brief Check if there is any pending message in the message queue of
 * a main window.
 *
 * This function checks whether there is any pending message in the
 * message queue of the main window \a hMainWnd.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return TRUE for pending message, FALSE for empty message queue.
 *
 * \sa GetMessage, MSG
 */
MG_EXPORT BOOL GUIAPI HavePendingMessage (HWND hMainWnd);

/**
 * \fn BOOL PeekMessage (PMSG pMsg, HWND hWnd, \
 *               UINT nMsgFilterMin, UINT nMsgFilterMax, UINT uRemoveMsg)
 * \brief Peek a message from the message queue of a main window
 *
 * This function peeks a message from the message queue of the window \a hWnd
 * and returns immediatly. Unlike \a GetMessage, this function does not wait
 * for a message.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param nMsgFilterMin The min identifier of the message that should be peeked.
 * \param nMsgFilterMax The max identifier of the message that should be peeked.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekPostMessage, HavePendingMessage, PostMessage
 */
static inline BOOL GUIAPI PeekMessage (PMSG pMsg, HWND hWnd, UINT nMsgFilterMin,
                         UINT nMsgFilterMax, UINT uRemoveMsg)
{
    return PeekMessageEx (pMsg, hWnd, nMsgFilterMin, nMsgFilterMax,
                           FALSE, uRemoveMsg);
}

/**
 * \fn BOOL PeekPostMessage (PMSG pMsg, HWND hWnd,
                UINT nMsgFilterMin, UINT nMsgFilterMax, UINT uRemoveMsg)
 * \brief Peek a post message from the message queue of a main window
 *
 * This function peeks a message from the message queue of the window \a hWnd
 * and returns immediatly. Unlike \a PeekMessage, this function only peek a
 * post message.
 *
 * \param pMsg Pointer to the result message.
 * \param hWnd The handle to the window.
 * \param nMsgFilterMin The min identifier of the message that should be peeked.
 * \param nMsgFilterMax The max identifier of the message that should be peeked.
 * \param uRemoveMsg Whether remove the message from the message queue.
 *      Should be the following values:
 *      - PM_NOREMOVE\n
 *        Leave it in the message queue.
 *      - PM_REMOVE
 *        Remove it from the message queue.
 *      - PM_NOYIELD
 *        Nouse now.
 *
 * \return TRUE if there is a message peeked, or FALSE.
 *
 * \sa GetMessage, PeekMessage, HavePendingMessage, PostMessage
 */
MG_EXPORT BOOL GUIAPI PeekPostMessage (PMSG pMsg, HWND hWnd, UINT nMsgFilterMin,
                UINT nMsgFilterMax, UINT uRemoveMsg);

/**
 * \fn int PostMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
 * \brief Posts a message into the message queue of a window and returns
 * immediatly.
 *
 * This function posts a message into the message queue of the window \a hWnd
 * and returns immediately.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The identifier of the message.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return ERR_OK on success, < 0 on errors.
 *
 * \retval ERR_OK Post message successfully.
 * \retval ERR_QUEUE_FULL The message queue is full.
 * \retval ERR_INV_HWND Invalid window handle.
 *
 * \sa SendMessage
 */
MG_EXPORT int GUIAPI PostMessage (HWND hWnd, UINT nMsg,
                WPARAM wParam, LPARAM lParam);

/**
 * \fn LRESULT SendMessage (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
 * \brief Send a message to a window and wait for the handling result.
 *
 * This function sends a message to the window \a hWnd, and will return
 * until the message-handling process returns.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The identifier of the message.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The return value of the message handler. Two special return values
 *  are reserved for the errors:
 *
 * \retval ERR_INV_HWND Invalid window handle.
 * \retval ERR_MSG_CANCELED The message handling was canceled by
 *  the target window. This may occur when the target window which
 *  is running in another thread is destroyed.
 *
 * \note The special return values are negative prime numbers
 *  (-2 and -3). Therefore, it is safe if you return a valid 4- or 8-byte
 *  aligned pointer value for the successful result. If you use a negative
 *  numbers to indicate the error status of your message handler, please
 *  choose a different value other than -2 or -3.
 *
 * \sa PostMessage
 */
MG_EXPORT LRESULT GUIAPI SendMessage (HWND hWnd, UINT nMsg,
                WPARAM wParam, LPARAM lParam);

/**
 * \fn void GUIAPI SetAutoRepeatMessage (HWND hwnd, UINT msg,
            WPARAM wParam, LPARAM lParam)
 * \brief Set the auto-repeat message.
 *
 * This function sets the auto-repeat message. When the default message
 * procedure receives an MSG_IDLE message, the default handler will send
 * the auto-repeat message to the target window as a notification message.
 *
 * \param hwnd The handle to the target window. Set it to zero
 *        to disable the auto-repeat message.
 * \param msg The identifier of the auto-repeat message.
 * \param wParam The first parameter of the auto-repeat message.
 * \param lParam The second parameter of the auto-repeat message.
 */
MG_EXPORT void GUIAPI SetAutoRepeatMessage (HWND hwnd, UINT msg,
                WPARAM wParam, LPARAM lParam);

#ifdef _MGRM_PROCESSES

#define CLIENTS_TOPMOST          -1
#define CLIENTS_ALL              -2
#define CLIENTS_EXCEPT_TOPMOST   -3
#define CLIENT_ACTIVE            -4

/**
 * \fn int Send2Client (const MSG* msg, int cli)
 * \brief Send a message to a client.
 *
 * This function sends a message to the specified client \a cli.
 *
 * \param msg The pointer to the message.
 * \param cli Either be the identifier of the targe client or one of the
 *        following values:
 *      - CLIENT_ACTIVE\n
 *        The current active client on the topmost layer.
 *      - CLIENTS_TOPMOST\n
 *        All clients in the topmost layer.
 *      - CLIENTS_EXCEPT_TOPMOST\n
 *        All clients except clients in the topmost layer.
 *      - CLIENTS_ALL\n
 *        All clients.
 *
 * \return The number of bytes sent, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \sa Send2TopMostClients, Send2ActiveWindow
 */
int GUIAPI Send2Client (const MSG* msg, int cli);

/**
 * \fn BOOL Send2TopMostClients (UINT nMsg, WPARAM wParam, LPARAM lParam)
 * \brief Send a message to all clients in the topmost layer.
 *
 * This function sends the message specified by (\a nMsg, \a wParam, \a lParam)
 * to all clients in the topmost layer.
 *
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 * \return TRUE on success, FALSE on error.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \note The message will be sent to the virtual desktop of the target client.
 *
 * \sa Send2Client, Send2ActiveWindow
 */
BOOL GUIAPI Send2TopMostClients (UINT nMsg, WPARAM wParam, LPARAM lParam);

/**
 * \fn BOOL Send2ActiveWindow (const MG_Layer* layer,
 *      UINT nMsg, WPARAM wParam, LPARAM lParam);
 * \brief Send a message to the active window in a layer.
 *
 * This function sends the message specified by (\a nMsg, \a wParam, \a lParam)
 * to the current active window in the specific layer (\a layer).
 *
 * \param layer The pointer to the layer.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note This function is only defined for MiniGUI-Processes, and
 *       can be called only by the server, i.e. \a mginit.
 *
 * \sa Send2Client
 */
BOOL GUIAPI Send2ActiveWindow (const MG_Layer* layer,
                 UINT nMsg, WPARAM wParam, LPARAM lParam);

#endif /* _MGRM_PROCESSES */

#ifdef _MGHAVE_VIRTUAL_WINDOW

/**
 * \fn LRESULT PostSyncMessage (HWND hWnd, UINT nMsg,
 *      WPARAM wParam, LPARAM lParam)
 * \brief Post a synchronical message to a window which is in different thread.
 *
 * This function posts the synchronical message specified by
 * (\a nMsg, \a wParam, \a lParam) to the window \a hWnd which
 * is in a different thread. This function will return until
 * the message is handled by the window procedure of that window.
 *
 * \note The destination window must belong to other thread.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The return value of the message handler.
 *
 * \sa SendMessage
 */
MG_EXPORT LRESULT GUIAPI PostSyncMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);

/**
 * \fn LRESULT SendAsyncMessage (HWND hWnd, UINT nMsg,
 *      WPARAM wParam, LPARAM lParam)
 * \brief Send an asynchronical message to a window.
 *
 * This function sends the asynchronical message specified by
 * (\a nMsg, \a wParam, \a lParam) to the window \a hWnd
 * which might be in a different thread. This function calls
 * the window procedure immediately, so it is very dangerous.
 * You should make sure that the message handler is thread safe.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The return value of the message handler.
 *
 * \note This function may corrupt your data.
 *
 * \sa PostSyncMessage
 */
MG_EXPORT LRESULT GUIAPI SendAsyncMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);

#else /* defined _MGHAVE_VIRTUAL_WINDOW */

/**
 * \def SendAsyncMessage
 * \brief An alias of \a SendMessage if _MGHAVE_VIRTUAL_WINDOW
 *        is not enabled.
 *
 * \sa SendMessage
 */
#define SendAsyncMessage SendMessage

#endif /* not defined _MGHAVE_VIRTUAL_WINDOW */

/**
 * \fn int SendNotifyMessage (HWND hWnd, UINT nMsg,
 *      WPARAM wParam, LPARAM lParam)
 * \brief Send a notification message to a window.
 *
 * This function sends the notification message specified by
 * (\a nMsg, \a wParam, \a lParam) to the window \a hWnd. This function
 * puts the notification message at the tail of the message queue and
 * returns immediately.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return 0 if all OK, < 0 on error.
 *
 * \sa SendMessage, PostMessage
 */
MG_EXPORT int GUIAPI SendNotifyMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);

/**
 * \fn int SendPriorNotifyMessage (HWND hWnd, UINT nMsg,
 *      WPARAM wParam, LPARAM lParam)
 * \brief Send a prior notification message to a window.
 *
 * This function sends the notification message specified by
 * (\a nMsg, \a wParam, \a lParam) to the window \a hWnd. This function
 * puts the notification message at the head of the message queue and
 * returns immediately.
 *
 * \param hWnd The handle to the window.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return 0 if all OK, < 0 on error.
 *
 * \sa SendMessage, PostMessage
 */
MG_EXPORT int GUIAPI SendPriorNotifyMessage (HWND hWnd, UINT nMsg,
        WPARAM wParam, LPARAM lParam);

/**
 * \fn int GUIAPI NotifyWindow (HWND hWnd, LINT id, int code, DWORD dwAddData)
 * \brief Send a notification message to a window.
 *
 * This function sends a notification message to the target window \a hWnd.
 * By default, the notification will be packed as a MSG_NOTIFICATION message
 * and be sent to the target window. This function will return immediately
 * after putting the message to the message queue of the target window.
 *
 * If you have set the notification callback procedure for the target window,
 * DispatchMessage will call the procedure in the context of the thread of
 * that window, otherwise, the function will convert this message to a
 * MSG_COMMAND message and dispatch the MSG_COMMAND message to the
 * window procedure.
 *
 * \param hWnd The handle to target window.
 * \param id The identifier of the source.
 * \param code The notification code.
 * \param dwAddData The additional data of the notification.
 *
 * \return 0 if all OK, < 0 on error.
 *
 * \sa MSG_COMMAND, MSG_NOTIFICATION, SetNotificationCallback
 *
 * Since 5.0.0
 */
MG_EXPORT int GUIAPI NotifyWindow (HWND hWnd, LINT id, int code,
        DWORD dwAddData);

/**
 * \fn int BroadcastMessage (UINT nMsg, WPARAM wParam, LPARAM lParam)
 * \brief Broadcast a message to all main windows on the desktop.
 *
 * This function posts the message specified by (\a nMsg, \a wParam, \a lParam)
 * to all the main windows on the desktop.
 *
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 * \return 0 if all OK, < 0 on error.
 *
 * \sa PostMessage
 */
MG_EXPORT int GUIAPI BroadcastMessage (UINT nMsg, WPARAM wParam, LPARAM lParam);

#ifdef _MGHAVE_VIRTUAL_WINDOW

/**
 * \fn int BroadcastMessageInThisThread (UINT nMsg,
 *      WPARAM wParam, LPARAM lParam)
 * \brief Broadcast a message to all main/virtual windows in the current thread.
 *
 * This function posts the message specified by (\a nMsg, \a wParam, \a lParam)
 * to all the main/virtual windows in the current thread.
 *
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \return The number of windows posted the message; < 0 on error.
 *
 * \note This function only available when the support for virtual window
 *      is enabled. If the support for virtual window is not enabled,
 *      this function is defined as an alias of \a BroadcastMessage.
 *
 * \sa BroadcastMessage, PostMessage
 */
MG_EXPORT int GUIAPI BroadcastMessageInThisThread (UINT nMsg,
        WPARAM wParam, LPARAM lParam);

#else   /* defined _MGHAVE_VIRTUAL_WINDOW */

#define BroadcastMessageInThisThread BroadcastMessage

#endif  /* not defined _MGHAVE_VIRTUAL_WINDOW */

/**
 * \fn int PostQuitMessage (HWND hWnd)
 * \brief Puts a MSG_QUIT message into the message queue of a main window.
 *
 * This function puts a MSG_QUIT message into the message queue of the
 * main window \a hWnd. The next call to \a GetMessage will return 0.
 *
 * \param hWnd The handle to the main window.
 * \return 0 if all OK, < 0 on error.
 *
 * \sa GetMessage
 */
MG_EXPORT int GUIAPI PostQuitMessage (HWND hWnd);

#define KBD_LAYOUT_DEFAULT  "default"
#define KBD_LAYOUT_FRPC     "frpc"
#define KBD_LAYOUT_FR       "fr"
#define KBD_LAYOUT_DE       "de"
#define KBD_LAYOUT_DELATIN1 "delatin1"
#define KBD_LAYOUT_IT       "it"
#define KBD_LAYOUT_ES       "es"
#define KBD_LAYOUT_ESCP850  "escp850"
#define KBD_LAYOUT_HEBREW   "hebrew"
#define KBD_LAYOUT_ARABIC   "arabic"

/**
 * \fn BOOL SetKeyboardLayout (const char* kbd_layout)
 * \brief Set a new keyboard layout.
 *
 * This function sets the keymaps to translate key scancodes to MSG_CHAR
 * or MSG_KEYSYM messages. The default keymaps is for US PC keyboard
 * layout, you can call this function to set a different keyboard layout.
 * The argument of \a kbd_layout specifies the name of the keyboard layout.
 *
 * \param kbd_layout The keyboard layout name. It can be
 *        one of the following values:
 *
 *      - KBD_LAYOUT_DEFAULT\n
 *        The default keyboard layout, i.e., US PC.
 *      - KBD_LAYOUT_FRPC\n
 *        The France PC keyboard layout.
 *      - KBD_LAYOUT_FR\n
 *        The France keyboard layout.
 *      - KBD_LAYOUT_DE\n
 *        The German keyboard layout.
 *      - KBD_LAYOUT_DELATIN1\n
 *        The German Latin1 keyboard layout.
 *      - KBD_LAYOUT_IT\n
 *        The Italian keyboard layout.
 *      - KBD_LAYOUT_ES\n
 *        The Spanish keyboard layout.
 *      - KBD_LAYOUT_ESCP850\n
 *        The Spanish CP850 keyboard layout.
 *      - KBD_LAYOUT_HEBREW\n
 *        The hebrew keyboard layout.
 *      - KBD_LAYOUT_ARABIC\n
 *        The arabic keyboard layout.
 *
 * \return TRUE for success, otherwise FALSE.
 *
 * \sa TranslateMessage, MSG_CHAR, MSG_KEYSYM
 */
MG_EXPORT BOOL GUIAPI SetKeyboardLayout (const char* kbd_layout);

#ifdef _MGCHARSET_UNICODE

MG_EXPORT int GUIAPI ToUnicode (UINT keycode, const BYTE* kbd_state,
        wchar_t* wcs, int wcs_len, const char* kbd_layout);

MG_EXPORT int GUIAPI ToAscii (UINT keycode, const BYTE* kbd_state,
        WORD* ch, const char* kbd_layout);

#endif /* _MGCHARSET_UNICODE */

/**
 * \fn BOOL TranslateMessage (PMSG pMsg)
 * \brief Translates key down and key up messages to MSG_CHAR
 *        message and post it into the message queue.
 *
 * This function translates key down and key up message to an MSG_CHAR
 * message or some MSG_KEYSYM messages, and send the message(s) to
 * the window procedure as a notification message. If the message is
 * not a key message, this function does nothing.
 *
 * The behavior of this function is inflected by the current
 * keyboard layout. The default keyboard layout is US PC keyboard, but
 * you can call \a SetKeyboardLayout function to set a different keyboard
 * layout.
 *
 * \param pMsg The pointer of message.
 * \return A boolean indicates whether the message is a key message.
 *
 * \sa SetKeyboardLayout, MSG_CHAR, MSG_KEYSYM
 */
MG_EXPORT BOOL GUIAPI TranslateMessage (PMSG pMsg);

/**
 * \fn BOOL GUIAPI TranslateKeyMsgToChar (int message, \
                WPARAM wParam, LPARAM lParam, WORD *ch)
 * \brief Translates a key down and key up message to a corresponding character.
 *
 * This function translates a key down and key up message to a character.
 * If the message is not a key message, this function does nothing.
 *
 * The behavior of this function is inflected by the current
 * keyboard layout. The default keyboard layout is US PC keyboard, but
 * you can call \a SetKeyboardLayout function to set a different keyboard
 * layout.
 *
 * \param message The type of message.
 * \param wParam Message parameter.
 * \param lParam Message parameter.
 * \param ch A string buffer for storing translated characters.
 *
 * \return A boolean indicates whether the message is a key message.
 *
 * \sa TranslateMessage
 */
MG_EXPORT BOOL GUIAPI TranslateKeyMsgToChar (int message,
                WPARAM wParam, LPARAM lParam, WORD *ch);

/**
 * \fn LRESULT DispatchMessage (PMSG pMsg)
 * \brief Dispatches a message to the window's callback procedure.
 *
 * This function dispatches the message pointed to by \a pMsg to the
 * target window's callback procedure.
 *
 * \param pMsg The pointer to the message.
 * \return The return value of the message handler.
 *
 * \sa GetMessage
 *
 * Example:
 *
 * \include getmessage.c
 */
MG_EXPORT LRESULT GUIAPI DispatchMessage (PMSG pMsg);

/**
 * \fn int ThrowAwayMessages (HWND pMainWnd)
 * \brief Removes all messages in the message queue associated with a window.
 *
 * This function removes all messages which are associated with
 * the specified window \a pMainWnd.
 *
 * \param pMainWnd The handle to the window.
 * \return The number of thrown messages.
 *
 * \sa EmptyMessageQueue
 */
MG_EXPORT int GUIAPI ThrowAwayMessages (HWND pMainWnd);

/**
 * \fn BOOL EmptyMessageQueue (HWND hWnd)
 * \brief Empty a message queue.
 *
 * This function empties the message queue which is used by the window specified
 * by \a hWnd.
 *
 * \param hWnd The handle to the main window.
 *
 * \return TRUE on all success, FALSE on error.
 *
 * \sa ThrowAwayMessages
 */
MG_EXPORT BOOL GUIAPI EmptyMessageQueue (HWND hWnd);

#ifdef _MGHAVE_MSG_STRING

/**
 * \fn const char* GUIAPI Message2Str (UINT message)
 * \brief Translates a message identifier to the message string.
 *
 * This function returns the message string of the message identifier \a message.
 * E.g. this function will return the string of "MSG_CHAR" for MSG_CHAR message.
 *
 * \param message The message identifier.
 * \return The message string.
 *
 * \note Only available if defined _MGHAVE_MSG_STRING.
 *
 * \sa PrintMessage
 */
MG_EXPORT const char* GUIAPI Message2Str (UINT message);

/**
 * \fn void GUIAPI PrintMessage (FILE* fp, HWND hWnd, \
                UINT nMsg, WPARAM wParam, LPARAM lParam)
 * \brief Prints a message in readable string form to a stdio stream.
 *
 * This function prints the message specified by (\a nMsg, \a wParam, \a lParam)
 * in readable string form to the stdio stream \a fp.
 *
 * \param fp The pointer to the FILE object.
 * \param hWnd The target window of the message.
 * \param nMsg The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 * \sa Message2Str
 */
MG_EXPORT void GUIAPI PrintMessage (FILE* fp, HWND hWnd,
                UINT nMsg, WPARAM wParam, LPARAM lParam);

#endif /* defined _MGHAVE_MSG_STRING */

    /** @} end of msg_pass_fns */

#ifdef HAVE_SELECT
    /**
     * \defgroup listenfd_fns Listening a file descriptor
     *
     * Register/Unregister a listening file descriptor to the message queue
     * of the current thread.
     *
     * When you need to listen to a file descriptor, you can use \a select(2)
     * system call. In MiniGUI, you can also register it to MiniGUI to
     * be a listened fd, and when there is a read/write/except event on
     * the registered fd, MiniGUI will sent a notification message to
     * the registered window.
     *
     * The functions in this group are only available to MiniGUI-Processes
     * and MiniGUI-Standalone before 5.0.0. Since 5.0.0, you can register a
     * file descriptor to be listened for all runtime modes as long as the
     * underlying system supports select().
     *
     * Example:
     *
     * \include listenfd.c
     *
     * @{
     */

/**
 * \def MAX_NR_LISTEN_FD
 * \brief The max number of listen fd which user can use.
 *
 * \note Deprecated since 5.0.0. MiniGUI will try to allocate enough space to
 *      manage all listening file descriptors.
 */
#define MAX_NR_LISTEN_FD   4

#ifdef WIN32
#ifndef POLLIN
#define POLLIN  0x001
#endif

#ifndef POLLOUT
#define POLLOUT 0x004
#endif

#ifndef POLLERR
#define POLLERR 0x008
#endif

#endif /* WIN32 */

/**
 * \fn BOOL GUIAPI RegisterListenFD (int fd, int type,
                HWND hwnd, void* context)
 * \brief Register a file descriptor to be listened in the message loop.
 *
 * This function registers the file desciptor \a fd to be listened in the
 * message loop of the current message thread.
 *
 * When there is a read/write/except event on this \a fd, MiniGUI
 * will post a MSG_FDEVENT message with wParam being equal to
 * MAKELONG (fd, type), and the lParam being set to \a context
 * to the target window \a hwnd.
 *
 * \param fd The file descriptor to be listened.
 * \param type The type of the event to be listened, can be Or'd with
 *      one or more of POLLIN, POLLOUT, and POLLERR.
 * \param hwnd The handle to the window will receive MSG_FDEVENT message.
 * \param context The value will be passed to the window as lParam of
 *        MSG_FDEVENT message.
 *
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available when the underlying system has select().
 *
 * \sa UnregisterListenFD, system_msgs
 */
MG_EXPORT BOOL GUIAPI RegisterListenFD (int fd, int type,
                HWND hwnd, void* context);

/**
 * \fn BOOL GUIAPI UnregisterListenFD (int fd)
 * \brief Unregister a being listened file descriptor.
 *
 * This function unregisters the being listened file descriptor \a fd.
 *
 * \param fd The file descriptor to be unregistered, should be a being
 *        listened file descriptor.
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available when the underlying system has select().
 *
 * \sa RegisterListenFD
 */
MG_EXPORT BOOL GUIAPI UnregisterListenFD (int fd);

    /** @} end of listenfd_fns */

#endif /* defined HAVE_SELECT */

    /**
     * \defgroup msg_hook_fns Message or event hook functions
     * @{
     */

#define HOOK_OP_MASK        0x00FF
#define HOOK_GOON           0x0000
#define HOOK_STOP           0x0001

#define HOOK_EVENT_MASK     0xFF00
#define HOOK_EVENT_KEY      0x0100
#define HOOK_EVENT_MOUSE    0x0200
#define HOOK_EVENT_EXTRA    0x0400


/**
 * \var typedef int (* MSGHOOK) (void* context, HWND dst_wnd,
 *      UINT msg, WPARAM wparam, LPARAM lparam)
 * \brief Type of message hook function.
 *
 * This is the type of a message hook function.
 *
 * \param context The context which was set when you registered
 *      the hook function.
 * \param dst_wnd The handle of the original destination window of the message.
 * \param msg The message identifier.
 * \param wparam The first parameter of the message.
 * \param lparam The second paramter of the message.
 *
 * \return A value indicating stopping or continuing the subsequent handling
 *      of the message, can be one of the following values:
 *      - HOOK_GOON\n
 *        Indicate continuing to handle the message.
 *      - HOOK_STOP\n
 *        Indicate stopping to handle the message.
 */
typedef int (* MSGHOOK) (void* context, HWND dst_wnd,
        UINT msg, WPARAM wparam, LPARAM lparam);

/**
 * \fn MSGHOOK GUIAPI RegisterEventHookFunc (int event_type,
 *      MSGHOOK hook, void* context)
 * \brief Registers an input event message hook function.
 *
 * This function registers an input event message hook function pointed
 * to by \a hook. When the desktop receives an input event message with
 * the specified event type \a event_type, it will call the hook function
 * first, and passes the \a context value to the hook as the first argument.
 *
 * \param event_type Which type of event to be hooked. The value of
 *      this argument should be a one of the following values:
 *      - HOOK_EVENT_KEY\n
 *        To hook all key events.
 *      - HOOK_EVENT_MOUSE\n
 *        To hook all mouse events.
 *      - HOOK_EVENT_EXTRA\n
 *        To hook all extra input events.
 * \param hook The pointer to the hook function. This function will unregister
 *      the old hook for the specified event type if \a hook is NULL.
 * \param context The context value will be passed to the hook function.
 *
 * \return The pointer to the old hook function for the specified event type.
 *
 * \note The hook function will be called in the context of
 *      desktop or event thread. you should note the thread safety when
 *      implementing the hook function.
 *
 * \sa RegisterEventHookWindow, MSGHOOK
 *
 * Since 5.0.0
 */
MG_EXPORT MSGHOOK GUIAPI RegisterEventHookFunc (int event_type,
        MSGHOOK hook, void* context);

/**
 * \fn MSGHOOK GUIAPI RegisterKeyMsgHook (void* context, MSGHOOK hook)
 * \brief Register a key message hook function.
 *
 * This function registers a key message hook pointed to by \a hook.
 * When the desktop receives a key message, it will call the hook function
 * first, and passes the \a context value to the hook as the first argument.
 *
 * \param context The context value will be passed to the hook.
 * \param hook The pointer to the hook function. This function will
 *      unregister the old hook if \a hook is NULL.
 *
 * \return The pointer to the old hook function.
 *
 * \sa RegisterEventHookFunc, MSGHOOK
 */
static inline MSGHOOK GUIAPI RegisterKeyMsgHook (void* context, MSGHOOK hook)
{
    return RegisterEventHookFunc (HOOK_EVENT_KEY, hook, context);
}

/**
 * \fn MSGHOOK GUIAPI RegisterMouseMsgHook (void* context, MSGHOOK hook)
 * \brief Register a mouse message hook function.
 *
 * This function registers a mouse message hook pointed to by \a hook.
 * When the desktop receives a mouse message, it will call the  hook function
 * first, and passes the \a context value to the hook as the first argument.
 *
 * \param context The context value will be passed to the hook function.
 * \param hook The pointer to the hook function. This function will
 *      unregister the old hook if \a hook is NULL.
 *
 * \return The pointer to the old hook function.
 *
 * \sa RegisterEventHookFunc, MSGHOOK
 */
static inline MSGHOOK GUIAPI RegisterMouseMsgHook (void* context, MSGHOOK hook)
{
    return RegisterEventHookFunc (HOOK_EVENT_MOUSE, hook, context);
}

/**
 * \fn BOOL GUIAPI RegisterEventHookWindow (HWND hwnd, DWORD flags)
 * \brief Register an input event message hook window.
 *
 * This function registers the specified window \a hwnd as an input
 * event message hook window. When MiniGUI receives an input event
 * message, it will post it to the hook window first.
 *
 * \param hwnd The hook hwnd.
 * \param flags The flags indicating the event types hooked event and whether stop
 *      or continue handling the hooked events. The value of this argument should
 *      be OR'd with one or more event types and HOOK_GOON or HOOK_STOP:
 *      - HOOK_EVENT_KEY\n
 *        To hook all key events.
 *      - HOOK_EVENT_MOUSE\n
 *        To hook all mouse events.
 *      - HOOK_EVENT_EXTRA\n
 *        To hook all extra input events.
 *      - HOOK_GOON\n
 *        Indicate continuing to handle the message.
 *      - HOOK_STOP\n
 *        Indicate stopping to handle the message.
 *
 * \return The handle of old hook window.
 *
 * \note This function be be called by a client of MiniGUI-Processes.
 *       For the server, you can use SetServerEventHook.
 *
 * \sa UnregisterEventHookWindow
 *
 * Since 5.0.0.
 */
MG_EXPORT BOOL GUIAPI RegisterEventHookWindow (HWND hwnd, DWORD flags);

/**
 * \fn BOOL GUIAPI UnregisterEventHookWindow (HWND hwnd)
 * \brief Unregister an input event message hook window.
 *
 * This function unregisters the specified window \a hwnd from
 * the hook list.
 *
 * \param hwnd The hook hwnd.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa RegisterEventHookWindow
 *
 * Since 5.0.0.
 */
MG_EXPORT BOOL GUIAPI UnregisterEventHookWindow (HWND hwnd);

/**
 * \fn HWND GUIAPI RegisterKeyHookWindow (HWND hwnd, DWORD flag)
 * \brief Register a key message hook window.
 *
 * This function registers a window specified by \a hwnd as the key message
 * hook window. When MiniGUI receives a key message, it will post it to the
 * hooked window first.
 *
 * \param hwnd The handle of the hook window. This function will unregister
 *      the old hook if \a hwnd is HWND_NULL.
 * \param flag Indicating whether stop or continue handling the hooked messages;
 *      HOOK_GOON to continue, HOOK_STOP to stop.
 *
 * \return The handle of old hook window.
 *
 * \sa UnregisterEventHookWindow, RegisterMouseHookWindow
 */
MG_EXPORT HWND GUIAPI RegisterKeyHookWindow (HWND hwnd, DWORD flag);

/**
 * \fn HWND GUIAPI RegisterMouseHookWindow (HWND hwnd, DWORD flag)
 * \brief Registers a mouse message hook window.
 *
 * This function registers a window specified by \a hwnd as the mouse message
 * hook window. When MiniGUI receives a mouse message, it will post it to the
 * hooked window first.
 *
 * \param hwnd The handle of the hook window. This function will unregister
 *      the old hook if \a hwnd is HWND_NULL.
 * \param flag Indicating whether stop or continue handling the hooked messages;
 *      HOOK_GOON to continue, HOOK_STOP to stop.
 *
 * \return The handle of old hook window.
 *
 * \sa UnregisterEventHookWindow, RegisterMouseHookWindow
 */
MG_EXPORT HWND GUIAPI RegisterMouseHookWindow (HWND hwnd, DWORD flag);

#ifndef _MGRM_THREADS

/**
 * \var typedef int (* SRVEVTHOOK) (PMSG pMsg)
 * \brief The type of the event hook.
 *
 * You can call \a SetServerEventHook to set an event hook
 * in the server of the MiniGUI-Processes or under MiniGUI-Standalone
 * runtime mode.
 *
 * If the event hook returns HOOK_GOON, MiniGUI will continue to
 * handle the event, and post it to the active client.
 * If the hook returns HOOK_STOP, MiniGUI will cancel subsequent handling.
 *
 * \note Deprecated since 5.0.0; Use \a RegisterEventHookFunc() instead.
 *
 * \sa SetServerEventHook, RegisterEventHookFunc
 */
typedef int (* SRVEVTHOOK) (PMSG pMsg);

/**
 * \fn SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
 * \brief Set an event hook in the server of MiniGUI-Processes.
 *
 * This function sets the event hook as \a SrvEvtHook for the server
 * of MiniGUI-Processes or MiniGUI-Standalone.
 *
 * \param SrvEvtHook The pointer to the hook, NULL to cancel the hook.
 *
 * \return The old hook function.
 *
 * \note Only available for MiniGUI-Processes and MiniGUI-Standalone runtime
 *      modes. Under MiniGUI-Processes, only the server can call this function.
 *
 * \note Deprecated since 5.0.0; Use \a RegisterEventHookFunc() instead.
 *
 * \sa SRVEVTHOOK, RegisterEventHookFunc
 */
MG_EXPORT SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook);

#endif /* not defined _MGRM_THREADS */

    /** @} end of msg_hook_fns */

    /** @} end of msg_fns */

    /** @} end of fns */

    /**
     * \defgroup styles Window styles
     * @{
     */

#define WS_CTRLMASK         0x0000FFFFL

/**
 * \def WS_NONE
 * \brief None style.
 */
#define WS_NONE             0x00000000L

/* bits in this mask are only for main windows */
#define WS_CAPTIONBAR_MASK  0xF0000000L

/**
 * \def WS_MINIMIZEBOX
 * \brief Creates a window with minimizing box on caption.
 * \note This style is valid only for main window.
 */
#define WS_MINIMIZEBOX      0x80000000L

/**
 * \def WS_MAXIMIZEBOX
 * \brief Creates a window with maximizing box on caption.
 * \note This style is valid only for main window.
 */
#define WS_MAXIMIZEBOX      0x40000000L

/**
 * \def WS_CAPTION
 * \brief Creates a main window with caption.
 */
#define WS_CAPTION          0x20000000L

/**
 * \def WS_SYSMENU
 * \brief Creates a main window with system menu.
 */
#define WS_SYSMENU          0x10000000L

/* bits in this mask are both for main windows and controls */
#define WS_STATUS_MASK      0x0F000000L

/**
 * \def WS_VISIBLE
 * \brief Creates a window initially visible.
 */
#define WS_VISIBLE          0x08000000L

/**
 * \def WS_DISABLED
 * \brief Creates a window initially disabled.
 */
#define WS_DISABLED         0x04000000L

/**
 * \def WS_VSCROLL
 * \brief Creates a window with vertical scroll bar.
 */
#define WS_VSCROLL          0x02000000L

/**
 * \def WS_HSCROLL
 * \brief Creates a window with horizontal scroll bar.
 */
#define WS_HSCROLL          0x01000000L

/* bits in this mask are both for main window and controls */
#define WS_BORDER_MASK      0x00F00000L

/**
 * \def WS_DLGFRAME
 * \brief The main window has a fixed frame, i.e. user can not
 *        drag the border of the window.
 */
#define WS_DLGFRAME         0x00800000L

/**
 * \def WS_THICKFRAME
 * \brief Creates a main window with thick frame.
 */
#define WS_THICKFRAME       0x00400000L

/**
 * \def WS_THINFRAME
 * \brief Creates a main window with thin frame.
 */
#define WS_THINFRAME        0x00200000L

/**
 * \def WS_BORDER
 * \brief Creates a window with border.
 */
#define WS_BORDER           0x00100000L

/* bits in this mask are reuse for main windows and controls;
   bits have different meanings for main windows and controls.*/
#define WS_MISC_MASK        0x000F0000L

/**
 * \def WS_CHILD
 * \brief Indicates the window is a child.
 */
#define WS_CHILD            0x00080000L

/**
 * \def WS_GROUP
 * \brief Indicates the control is the leader of a group.
 * \note This style is valid only for controls.
 */
#define WS_GROUP            0x00040000L

/**
 * \def WS_TABSTOP
 * \brief Indicates the user can set the input focus to the control by
 *        using Tab key.
 * \note This style is valid only for controls.
 */
#define WS_TABSTOP          0x00020000L

/* Main window states -- reserved for future use. */
#define WS_MINIMIZE         0x00040000L
#define WS_MAXIMIZE         0x00020000L

/**
 * \def WS_ALWAYSTOP
 * \brief Indicates the main window is always on top of others.
 *
 * Since 5.0.0.
 */
#define WS_ALWAYSTOP        0x00010000L

/* Obsolete styles, back-compatibility definitions. */
#define WS_OVERLAPPED       0x00000000L
#define WS_ABSSCRPOS        0x00000000L

#define WS_MAINWIN_ONLY_MASK    (WS_CAPTIONBAR_MASK | WS_ALWAYSTOP)
#define WS_CONTROL_ONLY_MASK    (WS_CTRLMASK)

/**
 * \def WS_EX_NONE
 * \brief No any extended window style.
 */
#define WS_EX_NONE                  0x00000000L

/**
 * \def WS_EX_CONTROL_MASK
 * \brief The extended style mask for control use.
 */
#define WS_EX_CONTROL_MASK          0x0000000FL

    /**
     * \defgroup main_window_type_styles Styles for main window types/levels
     *
     * Before 5.0.0, you can create a topmost main window with the style
     * \a WS_EX_TOPMOST in order to show the main window
     * above all normal windows, and if you use MiniGUI-Processes runtime mode,
     * the server (`mginit`) will always create global main windows.
     *
     * Since 5.0.0, we introduce a concept of zorder levels for main windows.
     * There are eight levels in MiniGUI from top to bottom:
     *
     *  - The tooltip level. 
     *  - The system/global level.
     *  - The screen lock level.
     *  - The docker level.
     *  - The higher level.
     *  - The normal level.
     *  - The launcher level.
     *  - The desktop or wallpaper.
     *
     * We use new styles like \a WS_EX_WINTYPE_GLOBAL to create main windows in
     * different levels. For historical reasons, you can still use the style
     * \a WS_EX_TOPMOST, but MiniGUI will create a main window in the higher
     * level for this style.
     *
     * By default, without the style \a WS_EX_TOPMOST or a style like
     * \a WS_EX_WINTYPE_GLOBAL, MiniGUI will create a main window in
     * the normal level.
     *
     * The desktop is the only main window in the desktop level. Any MiniGUI
     * process instance has a virtual desktop window. The desktop window is
     * an internal window object, so no API is provided for app to create or
     * manage it.
     *
     * Note that, under MiniGUI-Processes runtime mode, only the first client
     * creates the first main window in a z-order level other than higher and
     * normal levels can create another main window in the same z-order level.
     * And only the server can create a main window in the global z-order level.
     *
     * This is a security design for the multi-process runtime environment.
     *
     * @{
     */

/**
 * \def WS_EX_WINTYPE_MASK
 * \brief The style mask for main window type.
 *
 * \note This mask value is equal to the style mask \a WS_EX_CONTROL_MASK;
 *      The former is for main window, and the later for control.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_MASK          0x0000000FL

/**
 * \def WS_EX_WINTYPE_TOOLTIP
 * \brief The type for a tooltip main window.
 *
 * Use this style when you want to create a tooltip main window.
 * A tooltip main window will be shown above other types of main windows.
 *
 * \note The maximal number of all main windows in the tooltip level is 16.
 *      MiniGUI will create a main windows in higher level if there is no room
 *      in the tooltip level.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_TOOLTIP       0x00000001L

/**
 * \def WS_EX_WINTYPE_GLOBAL
 * \brief The type for a system/global main window.
 *
 * Use this style when you want to create a system/global main window.
 * A system/global main window will be shown above other types of main windows.
 *
 * \note Under MiniGUI-Processes runtime mode, only the server (`mginit`) can
 *  create main windows in the system level, and any main windows created
 *  by the server is a system main window.
 *
 *
 * \note Under MiniGUI-Processes runtime mode, the windows in this level will be kept
 *  visible on all layers.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_GLOBAL        0x00000002L

/**
 * \def WS_EX_WINTYPE_SCREENLOCK
 * \brief The type for a main window in the screen lock level.
 *
 * Use this style when you want to create a main window in the
 * screen lock level. A main window in the screen lock level will
 * be shown below the system main windows and above the other main windows.
 *
 * \note Under MiniGUI-Processes runtime mode, only clients on the default (`mginit`)
 *  layer can create a main window in this level, and only the client which creates
 *  the first main window in the screen lock level can create other
 *  main windows in the this level. For other clients, a main window in
 *  the higher level will be created.
 *
 * \note Under MiniGUI-Processes runtime mode, the windows in this level will be kept
 *  visible on all layers.
 *
 * \note The maximal number of all main windows in the screen lock level is 8.
 *  MiniGUI will create a main windows in higher level if there is no room
 *  in the screen lock level.
 * 
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_SCREENLOCK    0x00000003L

/**
 * \def WS_EX_WINTYPE_DOCKER
 * \brief The type for a main window in the docker level.
 *
 * Use this style when you want to create a main window in the
 * docker level. A main window in the docker level will
 * be shown below the main windows in the screen lock level and
 * above the main windows in the higher level.
 *
 * \note Under MiniGUI-Processes runtime mode, only clients on the default (`mginit`)
 *  layer can create a main window in this levele, and only the client which creates
 *  the first main window in the docker level can create other main windows
 *  in the docker level. For other clients, a main window in the higher
 *  level will be created.
 *
 * \note Under MiniGUI-Processes runtime mode, the windows in this level will be kept
 *  visible on all layers.
 *
 * \note The maximal number of all main windows in the docker level is 8.
 *  MiniGUI will create a main windows in higher level if there is no room
 *  in the docker level.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_DOCKER        0x00000004L

/**
 * \def WS_EX_WINTYPE_HIGHER
 * \brief The type for a main window in the higher level.
 *
 * Use this style when you want to create a main window in the
 * higher level. A main window in the higher level will
 * be shown below the main windows in the docker level and
 * above the main windows in the normal level.
 *
 * \note The maximal number of all main windows in the higher level is 16
 *      by default. An attempt to create a main window in higher level
 *      will fail if there is no room in the higher level.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_HIGHER        0x00000005L

/**
 * \def WS_EX_WINTYPE_NORMAL
 * \brief The type for a main window in the normal level.
 *
 * Use this style when you want to create a main window in the
 * normal level (default). A main window in the normal level will
 * be shown below the main windows in the higher level and
 * above the main windows in the launcher level.
 *
 * \note The maximal number of all main windows in the normal level is 128
 *      by default. An attempt to create a main window in normal level
 *      will fail if there is no room in the normal level.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_NORMAL        0x00000006L

/**
 * \def WS_EX_WINTYPE_LAUNCHER
 * \brief The type for a main window in the launcher level.
 *
 * Use this style when you want to create a main window in the
 * launcher level. A main window in the launcher level will
 * be shown below the main windows in the normal level and
 * above the wallpaper.
 *
 * \note Under MiniGUI-Processes runtime mode, only clients on the default (`mginit`)
 *  layer can create a main window in this levele, and only the client which creates
 *  the first main window in the launcher level can create other
 *  main windows in the launcher level. For other clients, a main window
 *  in the normal level will be created.
 *
 * \note Under MiniGUI-Processes runtime mode, the windows in this level will be kept
 *  visible on all layers.
 *
 * \note The maximal number of all main windows in the launcher level is 8.
 *  MiniGUI will create a main windows in normal level if there is no room
 *  in the launcher level.
 *
 * Since 5.0.0.
 */
#define WS_EX_WINTYPE_LAUNCHER      0x00000007L

    /** @} end of main_window_type_styles */

/**
 * \def WS_EX_TROUNDCNS
 * \brief The window have round corners at top edge.
 */
#define WS_EX_TROUNDCNS         0x00000010L

/**
 * \def WS_EX_BROUNDCNS
 * \brief The window have round corners at bottom edge.
 */
#define WS_EX_BROUNDCNS         0x00000020L

/**
 * \def WS_EX_USEPRIVATECDC
 * \brief The window has its own private client device context.
 *
 * \sa CreatePrivateClientDC
 */
#define WS_EX_USEPRIVATECDC     0x00000040L

/**
 * \def WS_EX_NOTDRAGGABLE
 * \brief A style controlling whether a main window can be dragged by using mouse.
 */
#define WS_EX_NOTDRAGGABLE      0x00000080L
#define WS_EX_DRAGGABLE         0x00000000L

/**
 * \def WS_EX_USEPARENTFONT
 * \brief The window uses the font of its parent as the default font.
 */
#define WS_EX_USEPARENTFONT     0x00000100L

/**
 * \def WS_EX_USEPARENTCURSOR
 * \brief The window uses the cursor of its parent as the default cursor.
 */
#define WS_EX_USEPARENTCURSOR   0x00000200L

/**
 * \def WS_EX_USEPARENTRDR
 * \brief The window use the same window renderer as parent.
 */
#define WS_EX_USEPARENTRDR      0x00000400L

/**
 * \def WS_EX_NOPARENTNOTIFY
 * \brief The window will not send any notification to the parent.
 *
 * \sa NotifyParentEx
 */
#define WS_EX_NOPARENTNOTIFY    0x00000800L

/**
 * \def WS_EX_AUTOSECONDARYDC
 * \brief The window creates its own secondary device context automatically.
 *
 * \sa CreateSecondaryDC
 */
#define WS_EX_AUTOSECONDARYDC   0x00001000L

/**
 * \def WS_EX_TRANSPARENT
 * \brief The window is transparent.
 * \note This style is only implemented for controls.
 */
#define WS_EX_TRANSPARENT       0x00002000L

/**
 * \def WS_EX_TOPMOST
 * \brief The main window is in the higher level.
 *
 * Before 5.0.0,
 * you can create a main window with this style in order to show the main window
 * above all normal windows, and if you use MiniGUI-Processes runtime mode,
 * the server (`mginit`) will always create global main windows.
 *
 * Since 5.0.0, we introduce a concept of levels for main windows. We can
 * use new styles like \a WS_EX_WINTYPE_GLOBAL to create main windows in
 * different levels. For historical reasons, you can still use this style,
 * but MiniGUI will create a main window in the higher level for this styele.
 *
 * \sa main_window_type_styles
 */
#define WS_EX_TOPMOST           0x00004000L

/**
 * \def WS_EX_TOOLWINDOW
 * \brief The main window is a tool window, which can not gain the input focus.
 */
#define WS_EX_TOOLWINDOW        0x00008000L

/* The group reserved for BIDI support */
#define WS_EX_RIGHT             0x00010000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00020000L
#define WS_EX_LTRREADING        0x00000000L

/**
 * \def WS_EX_LEFTSCROLLBAR
 * \brief The horizontal scroll bar of the window is located at left.
 */
#define WS_EX_LEFTSCROLLBAR     0x00040000L

/**
 * \def WS_EX_RIGHTSCROLLBAR
 * \brief The horizontal scroll bar of the window is located at right (default).
 */
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

/**
 * \def WS_EX_DLGHIDE
 * \brief The dialog won't show immediately after it is created.
 */
#define WS_EX_DLGHIDE           0x00100000L

/**
 * \def WS_EX_AUTOPOSITION
 * \brief The position of the main window will be determined by system.
 *
 * If a main window has this extend style when creating it, MiniGUI will
 * determine the position in the screen for the main window. If the width
 * or the height of the window specified in MAINWINCREATE structure is zero,
 * MiniGUI will also determine a default size for the main window.
 *
 * Under the compositing schema, the compositor is responsible to calculate
 * the position and the size for a main window.
 *
 * Since 5.0.0
 */
#define WS_EX_AUTOPOSITION      0x00200000L

/**
 * \def WS_EX_NOCLOSEBOX
 * \brief The main window has no closing box on its caption.
 */
#define WS_EX_NOCLOSEBOX        0x00400000L

/**
 * \def WS_EX_CLIPCHILDREN
 * \brief When paint the window, the children areas will be clipped.
 */
#define WS_EX_CLIPCHILDREN      0x00800000L

/**
 * \def WS_EX_LFRDR_MASK
 * \brief The style mask for LF renderer internal usage.
 */
#define WS_EX_LFRDR_MASK        0x0F000000L

/**
 * \def WS_EX_LFRDR_CUSTOM1
 * \brief The user-defined extended style. It's often used to process
 *        user-defined hotspot area.
 */
#define WS_EX_LFRDR_CUSTOM1     0x01000000L

/**
 * \def WS_EX_LFRDR_CUSTOM2
 * \brief The user-defined extended style.
 *
 * \sa WS_EX_LFRDR_CUSTOM1
 */
#define WS_EX_LFRDR_CUSTOM2     0x02000000L

/**
 * \def WS_EX_LFRDR_CUSTOM3
 * \brief The user-defined extended style.
 *
 * \sa WS_EX_LFRDR_CUSTOM1
 */
#define WS_EX_LFRDR_CUSTOM3     0x04000000L

/**
 * \def WS_EX_LFRDR_CUSTOM4
 * \brief The user-defined extended style.
 *
 * \sa WS_EX_LFRDR_CUSTOM1
 */
#define WS_EX_LFRDR_CUSTOM4     0x08000000L

/* Obsolete style, back-compatibility definitions. */
#define WS_EX_IMECOMPOSE        0x00000000L

/**
 * \def WS_EX_INTERNAL_MASK
 * \brief The style mask for internal use.
 *
 * Do not use the bits in this mask for applications.
 */
#define WS_EX_INTERNAL_MASK     0xF0000000L

    /** @} end of styles */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \defgroup lf_fns Look and Feel Renderer management functions
     * @{
     */

/**
 * \def WE_FONTS_NUMBER
 * \brief The number of window element font attributes.
 */
#define WE_FONTS_NUMBER             4

/**
 * \def WE_METRICS_NUMBER
 * \brief The number of window element metric attributes.
 */
#define WE_METRICS_NUMBER           6

/**
 * \def WE_COLORS_NUMBER
 * \brief The number of window element color attributes.
 */
#define WE_COLORS_NUMBER            14

/* Window type mask */
#define WE_ATTR_TYPE_MASK           0xF000
#define WE_ATTR_TYPE_COLOR_MASK     0x0F00
#define WE_ATTR_INDEX_MASK          0x00FF

#define WE_ATTR_TYPE_FONT           0x1000
#define WE_ATTR_TYPE_METRICS        0x2000

#define WE_ATTR_TYPE_COLOR          0x3000
#define WE_ATTR_TYPE_FGC            0x3000
#define WE_ATTR_TYPE_BGCA           0x3100
#define WE_ATTR_TYPE_BGCB           0x3200

/* window element id */
#define WE_CAPTION                  0
#define WE_MENU                     1
#define WE_MESSAGEBOX               2
#define WE_TOOLTIP                  3
#define WE_WND_BORDER               4
#define WE_SCROLLBAR                5
#define WE_INACTIVE_CAPTION         6
#define WE_WINDOW                   7
#define WE_THREED_BODY              8
#define WE_SELECTED_ITEM            9
#define WE_DISABLED_ITEM            10
#define WE_HIGHLIGHT_ITEM           11
#define WE_SIGNIFICANT_ITEM         12
#define WE_DESKTOP                  13

/* window element attr id */

/* Window caption */
#define WE_FONT_CAPTION             (WE_CAPTION | WE_ATTR_TYPE_FONT)
#define WE_METRICS_CAPTION          (WE_CAPTION | WE_ATTR_TYPE_METRICS)
#define WE_FGC_ACTIVE_CAPTION       (WE_CAPTION | WE_ATTR_TYPE_FGC)
#define WE_BGCA_ACTIVE_CAPTION      (WE_CAPTION | WE_ATTR_TYPE_BGCA)
#define WE_BGCB_ACTIVE_CAPTION      (WE_CAPTION | WE_ATTR_TYPE_BGCB)

/* Menu */
#define WE_FONT_MENU                (WE_MENU | WE_ATTR_TYPE_FONT)
#define WE_METRICS_MENU             (WE_MENU | WE_ATTR_TYPE_METRICS)
#define WE_FGC_MENU                 (WE_MENU | WE_ATTR_TYPE_FGC)
#define WE_BGC_MENU                 (WE_MENU | WE_ATTR_TYPE_BGCA)

/* Message box */
#define WE_FONT_MESSAGEBOX          (WE_MESSAGEBOX | WE_ATTR_TYPE_FONT)
#define WE_FGC_MESSAGEBOX           (WE_MESSAGEBOX | WE_ATTR_TYPE_FGC)

/* Tool tips */
#define WE_FONT_TOOLTIP             (WE_TOOLTIP | WE_ATTR_TYPE_FONT)
#define WE_FGC_TOOLTIP              (WE_TOOLTIP | WE_ATTR_TYPE_FGC)
#define WE_BGC_TOOLTIP              (WE_TOOLTIP | WE_ATTR_TYPE_BGCA)

/* Window border */
#define WE_METRICS_WND_BORDER       (WE_WND_BORDER | WE_ATTR_TYPE_METRICS)
#define WE_FGC_ACTIVE_WND_BORDER    (WE_WND_BORDER | WE_ATTR_TYPE_FGC)
#define WE_FGC_INACTIVE_WND_BORDER  (WE_WND_BORDER | WE_ATTR_TYPE_BGCA)

/* Scroll bar */
#define WE_METRICS_SCROLLBAR        (WE_SCROLLBAR | WE_ATTR_TYPE_METRICS)

/* Inactive caption */
#define WE_FGC_INACTIVE_CAPTION     (WE_INACTIVE_CAPTION | WE_ATTR_TYPE_FGC)
#define WE_BGCA_INACTIVE_CAPTION    (WE_INACTIVE_CAPTION | WE_ATTR_TYPE_BGCA)
#define WE_BGCB_INACTIVE_CAPTION    (WE_INACTIVE_CAPTION | WE_ATTR_TYPE_BGCB)

/* Window */
#define WE_FGC_WINDOW               (WE_WINDOW | WE_ATTR_TYPE_FGC)
#define WE_BGC_WINDOW               (WE_WINDOW | WE_ATTR_TYPE_BGCA)

/* 3d body */
#define WE_FGC_THREED_BODY          (WE_THREED_BODY | WE_ATTR_TYPE_FGC)
#define WE_MAINC_THREED_BODY        (WE_THREED_BODY | WE_ATTR_TYPE_BGCA)

/* Selected items */
#define WE_FGC_SELECTED_ITEM        (WE_SELECTED_ITEM | WE_ATTR_TYPE_FGC)
#define WE_BGC_SELECTED_ITEM        (WE_SELECTED_ITEM | WE_ATTR_TYPE_BGCA)
#define WE_BGC_SELECTED_LOSTFOCUS   (WE_SELECTED_ITEM | WE_ATTR_TYPE_BGCB)

/* Invalidate items */
#define WE_FGC_DISABLED_ITEM        (WE_DISABLED_ITEM | WE_ATTR_TYPE_FGC)
#define WE_BGC_DISABLED_ITEM        (WE_DISABLED_ITEM | WE_ATTR_TYPE_BGCA)

/* Highlight items */
#define WE_FGC_HIGHLIGHT_ITEM       (WE_HIGHLIGHT_ITEM | WE_ATTR_TYPE_FGC)
#define WE_BGC_HIGHLIGHT_ITEM       (WE_HIGHLIGHT_ITEM | WE_ATTR_TYPE_BGCA)

/* Significant items */
#define WE_FGC_SIGNIFICANT_ITEM     (WE_SIGNIFICANT_ITEM | WE_ATTR_TYPE_FGC)
#define WE_BGC_SIGNIFICANT_ITEM     (WE_SIGNIFICANT_ITEM | WE_ATTR_TYPE_BGCA)

/* Desktop */
#define WE_BGC_DESKTOP              (WE_DESKTOP | WE_ATTR_TYPE_BGCA)

#define WE_ATTR_TYPE_ICON           0x4000
#define WE_ATTR_TYPE_ICON_MASK      0x0F00
#define WE_ATTR_TYPE_SMALLICON      0x4000
#define WE_ATTR_TYPE_LARGEICON      0x4100

#define WE_LARGEICON_APPLICATION    (WE_ATTR_TYPE_LARGEICON | 0x00)
#define WE_LARGEICON_HAND           (WE_ATTR_TYPE_LARGEICON | 0x01)
#define WE_LARGEICON_QUESTION       (WE_ATTR_TYPE_LARGEICON | 0x02)
#define WE_LARGEICON_EXCLAMATION    (WE_ATTR_TYPE_LARGEICON | 0x03)
#define WE_LARGEICON_ASTERISK       (WE_ATTR_TYPE_LARGEICON | 0x04)
#define WE_LARGEICON_STOP           WE_LARGEICON_HAND
#define WE_LARGEICON_INFORMATION    WE_LARGEICON_ASTERISK

#define WE_SMALLICON_APPLICATION    (WE_ATTR_TYPE_SMALLICON | 0x00)
#define WE_SMALLICON_HAND           (WE_ATTR_TYPE_SMALLICON | 0x01)
#define WE_SMALLICON_QUESTION       (WE_ATTR_TYPE_SMALLICON | 0x02)
#define WE_SMALLICON_EXCLAMATION    (WE_ATTR_TYPE_SMALLICON | 0x03)
#define WE_SMALLICON_ASTERISK       (WE_ATTR_TYPE_SMALLICON | 0x04)
#define WE_SMALLICON_STOP           WE_SMALLICON_HAND
#define WE_SMALLICON_INFORMATION    WE_SMALLICON_ASTERISK

/**
 * \def WE_ATTR_TYPE_RDR
 * \brief The specific type for LF renderer.
 */
#define WE_ATTR_TYPE_RDR                0x5000

#define WE_LFFLAT_NUMBER                1
#ifdef _MGLF_RDR_FLAT

#define WE_LFFLAT_TAB_NRMLCLR           (WE_ATTR_TYPE_RDR | 0)
#endif

#define WE_LFSKIN_NUMBER                28
#ifdef _MGLF_RDR_SKIN
#define WE_LFSKIN_WND_BKGND             (WE_ATTR_TYPE_RDR | 0)

#define WE_LFSKIN_CAPTION               (WE_ATTR_TYPE_RDR | 1)
#define WE_LFSKIN_CAPTION_BTN           (WE_ATTR_TYPE_RDR | 2)

#define WE_LFSKIN_SCROLLBAR_VSHAFT      (WE_ATTR_TYPE_RDR | 3)
#define WE_LFSKIN_SCROLLBAR_VTHUMB      (WE_ATTR_TYPE_RDR | 4)
#define WE_LFSKIN_SCROLLBAR_HSHAFT      (WE_ATTR_TYPE_RDR | 5)
#define WE_LFSKIN_SCROLLBAR_HTHUMB      (WE_ATTR_TYPE_RDR | 6)
#define WE_LFSKIN_SCROLLBAR_ARROWS      (WE_ATTR_TYPE_RDR | 7)

#define WE_LFSKIN_BORDER_TOP            (WE_ATTR_TYPE_RDR | 8)
#define WE_LFSKIN_BORDER_BOTTOM         (WE_ATTR_TYPE_RDR | 9)
#define WE_LFSKIN_BORDER_LEFT           (WE_ATTR_TYPE_RDR | 10)
#define WE_LFSKIN_BORDER_RIGHT          (WE_ATTR_TYPE_RDR | 11)

#define WE_LFSKIN_ARROWS                (WE_ATTR_TYPE_RDR | 12)
#define WE_LFSKIN_ARROWS_SHELL          (WE_ATTR_TYPE_RDR | 13)

#define WE_LFSKIN_RADIOBUTTON           (WE_ATTR_TYPE_RDR | 14)
#define WE_LFSKIN_CHECKBUTTON           (WE_ATTR_TYPE_RDR | 15)
#define WE_LFSKIN_PUSHBUTTON            (WE_ATTR_TYPE_RDR | 16)

#define WE_LFSKIN_TREE                  (WE_ATTR_TYPE_RDR | 17)

#define WE_LFSKIN_HEADER                (WE_ATTR_TYPE_RDR | 18)
#define WE_LFSKIN_TAB                   (WE_ATTR_TYPE_RDR | 19)

#define WE_LFSKIN_PROGRESS_HCHUNK       (WE_ATTR_TYPE_RDR | 20)
#define WE_LFSKIN_PROGRESS_VCHUNK       (WE_ATTR_TYPE_RDR | 21)
#define WE_LFSKIN_PROGRESS_HTRACKBAR    (WE_ATTR_TYPE_RDR | 22)
#define WE_LFSKIN_PROGRESS_VTRACKBAR    (WE_ATTR_TYPE_RDR | 23)

#define WE_LFSKIN_TBSLIDER_H            (WE_ATTR_TYPE_RDR | 24)
#define WE_LFSKIN_TRACKBAR_VERT         (WE_ATTR_TYPE_RDR | 25)
#define WE_LFSKIN_TRACKBAR_HORZ         (WE_ATTR_TYPE_RDR | 26)
#define WE_LFSKIN_TBSLIDER_V            (WE_ATTR_TYPE_RDR | 27)
#endif

/**
 * \var typedef LRESULT (* WNDPROC)(HWND, int, WPARAM, LPARAM)
 * \brief Type of the window callback procedure.
 */
typedef LRESULT (* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

/**
 * \var typedef void (* NOTIFPROC)(HWND hwnd, LINT id, int nc, DWORD add_data)
 * \brief Type of the notification callback procedure.
 *
 * This is the function type of notification callback procedure.
 * If you set the notification callback procedure for a window.
 * When MiniGUI got a notification message, MiniGUI will call this
 * callback procedure in the context of the target window thread.
 *
 * If you have not set the notification callback procedure of the target
 * window, MiniGUI will send a MSG_COMMAND message to the window procedure.
 *
 * Since 5.0.0, you can also set the notification callback procedure for
 * a main window or a virtual window. You can call \a NotifyWindow function
 * to send a notify message to the traget window.
 *
 * \note The type of \a id changed from int to LINT since v3.2.
 *
 * \sa SetNotificationCallback, NotifyWindow, NotifyParentEx
 */
typedef void (* NOTIFPROC) (HWND hwnd, LINT id, int nc, DWORD add_data);

/** The window element attributes structure. */
typedef struct _WINDOW_ELEMENT_ATTR {
    /** The attribute identifier. */
    int we_attr_id;
    /** The attribute value. */
    DWORD we_attr;
} WINDOW_ELEMENT_ATTR;

struct _WINDOW_ELEMENT_RENDERER;
typedef struct _WINDOW_ELEMENT_RENDERER* PWERENDERER;

#define SBS_NORMAL                  0x00
#define SBS_DISABLED                0x01
#define SBS_HIDE                    0x02

/**
 * The scrollbar information structure in look and feel.
 */
typedef struct _LFSCROLLBARINFO {
    /** The minimum value of scrollbar range. */
    int  minPos;
    /** The max value of scrollbar range. */
    int  maxPos;
    /** The current scrollbar pos. */
    int  curPos;
    /** The step per page.  */
    int  pageStep;
    /** The length of arrow. */
    int  arrowLen;
    /** The start pixel of thumb. */
    int  barStart;
    /** The length of thumb. */
    int  barLen;
    /** The status of scrollbar. */
    int status;
} LFSCROLLBARINFO;

typedef LFSCROLLBARINFO* PLFSCROLLBARINFO;

/**
 * The scrollbar data construction for scrollbar control.
 */
typedef struct _SCROLLBARDATA {
    /** The minimum value of scrollbar range. */
    int  minPos;
    /** The max value of scrollbar range. */
    int  maxPos;
    /** The current scrollbar pos. */
    int  curPos;
    /** The step per page.  */
    int  pageStep;
    /** The length of arrow. */
    int  arrowLen;
    /** The start pixel of thumb. */
    int  barStart;
    /** The length of thumb. */
    int  barLen;
    /* mouse position before tracking thumb.
     * it represents the distance from x mouse postion to barStart
     * for horizontal scrollbar.
     * it represents the distance from y mouse postion to barStart
     * for vertical  scrollbar.
     */
    int  track_pos;
    /** The status of scrollbar. */
    int status;
} SCROLLBARDATA;

typedef SCROLLBARDATA* PSCROLLBARDATA;

/** left or up arrow is in disabled state */
#define SBS_DISABLED_LTUP       0x0100

/** right or bottom arrow is in disabled  state */
#define SBS_DISABLED_BTDN       0x0200

/** left or up arrow is in hilited state */
#define SBS_HILITE_LTUP         0x0400

/** right or bottom arrow is in hilited state */
#define SBS_HILITE_BTDN         0x0800

/** thumb is in hilite state as cursor is in the area of thumb */
#define SBS_HILITE_THUMB        0x0010

/** left or up arrow is in pressed state */
#define SBS_PRESSED_LTUP        0x0020

/** right or bottom arrow is in pressed state */
#define SBS_PRESSED_BTDN        0x0040

/** thumb is in pressed state as left button presses it */
#define SBS_PRESSED_THUMB       0x0080


#define LFRDR_WINTYPE_UNKNOWN           0
#define LFRDR_WINTYPE_DIALOG            1
#define LFRDR_WINTYPE_MAINWIN           2
#define LFRDR_WINTYPE_CONTROL           3

/** The window style information structure. Used by
 *  \a calc_we_metrics renderer function.
 */
typedef struct _LFRDR_WINSTYLEINFO
{
    /** The styles of window.*/
    int         dwStyle;

    /** The window type, can be one of the values:
     *      - LFRDR_WINTYPE_DIALOG\n
     *        For dialog.
     *      - LFRDR_WINTYPE_MAINWIN\n
     *        For main window.
     *      - LFRDR_WINTYPE_CONTROL\n
     *        For control.
     */
    int         winType;
} LFRDR_WINSTYLEINFO;

/**
 * The scrollbar information structure. Used by
 * \a draw_scrollbar and \a get_scrollbar_rect
 * renderer function.
 */
typedef struct _LFRDR_TRACKBARINFO
{
    int nMin;
    int nMax;
    int nPos;
    int nTickFreq;
}LFRDR_TRACKBARINFO;

#define LFRDR_METRICS_MASK              0xFF00
#define LFRDR_METRICS_BORDER            0x0100
#define LFRDR_METRICS_CAPTION_H         0x0200
#define LFRDR_METRICS_ICON_H            0x0300
#define LFRDR_METRICS_ICON_W            0x0400
#define LFRDR_METRICS_MENU_H            0x0500
#define LFRDR_METRICS_VSCROLL_W         0x0600
#define LFRDR_METRICS_HSCROLL_H         0x0700
#define LFRDR_METRICS_MINWIN_WIDTH      0x0800
#define LFRDR_METRICS_MINWIN_HEIGHT     0x0900

#define LFRDR_SB_MINBARLEN        10

/**
 * \def LEN_RENDERER_NAME
 * \brief The maximum length of a renderer name.
 */
#define LEN_RENDERER_NAME           15

/* button status used by button and 3dbox.  */
#define LFRDR_BTN_STATUS_MASK       0x03
#define LFRDR_BTN_STATUS_NORMAL     0x00
#define LFRDR_BTN_STATUS_HILITE     0x01
#define LFRDR_BTN_STATUS_PRESSED    0x02
#define LFRDR_BTN_STATUS_DISABLED   0x03

/* a flag used by radio button and check button. */
#define LFRDR_BTN_STATUS_SELECTED   0x04

/* a flag used by button on capton bar. */
#define LFRDR_BTN_STATUS_INACTIVE   0x08

/* thick or thin frame used by 3dbox. */
#define LFRDR_3DBOX_THICKFRAME      0x20

/* filled 3dbox */
#define LFRDR_3DBOX_FILLED          0x40

/*select status used by checkmark or radio.*/
#define LFRDR_MARK_SELECTED_MASK    0x05
#define LFRDR_MARK_ALL_SELECTED     0x01
#define LFRDR_MARK_HALF_SELECTED    0x04

/* a flag whether a checkmark or radio has a shell or not.*/
#define LFRDR_MARK_HAVESHELL        0x02

/*the direction of arrow.*/
#define LFRDR_ARROW_DIRECT_MASK     0x0F00
#define LFRDR_ARROW_LEFT            0x0000
#define LFRDR_ARROW_RIGHT           0x0100
#define LFRDR_ARROW_UP              0x0200
#define LFRDR_ARROW_DOWN            0x0300

/* hollow arrow or filled */
#define LFRDR_ARROW_NOFILL          0x1000
#define LFRDR_ARROW_HAVESHELL       0x2000

/* minimum of menu margin */
#define LFRDR_MENUITEMOFFY_MIN      4

#define LFRDR_3DBOX_COLOR_LIGHTEST  0x01
#define LFRDR_3DBOX_COLOR_DARKEST   0x02
#define LFRDR_3DBOX_COLOR_LIGHTER   0x03
#define LFRDR_3DBOX_COLOR_DARKER    0x04

/* flag of propsheet tab */
#define LFRDR_TAB_BOTTOM            0x0001
#define LFRDR_TAB_ACTIVE            0x0010
#define LFRDR_TAB_ICON              0x0100

/* for fold icon*/
#define LFRDR_TREE_WITHICON         0x08
#define LFRDR_TREE_FOLD             0x01

/* for connecting line*/
#define LFRDR_TREE_CHILD            0x02
#define LFRDR_TREE_NEXT             0x04

#define SYSICO_ITEM_NUMBER          5

/** The window element renderer structure. */
typedef struct _WINDOW_ELEMENT_RENDERER {
    /** The renderer name. */
    const char name[LEN_RENDERER_NAME+1];

    /** The pointer to initialize function. It is used to initialize basic
     *  renderer information (metrics, color, font, icon, etc) and private
     *  information.
     */
    int (*init) (PWERENDERER renderer);

    /** The pointer to release function. It is used to release renderer
     *  resource.
     *  \sa init
     */
    int (*deinit) (PWERENDERER renderer);

    /** The function pointer to calculate lighter, lightest, darker
     * and darkest color according to main color.
     *
     *  \param color The main color.
     *  \param flag The color type, can be one of the values:
     *      - LFRDR_3DBOX_COLOR_DARKER \n
     *        darker color.
     *      - LFRDR_3DBOX_COLOR_DARKEST \n
     *        darkest color.
     *      - LFRDR_3DBOX_COLOR_LIGHTER \n
     *        lighter color.
     *      - LFRDR_3DBOX_COLOR_LIGHTEST \n
     *        lightest color.
     */
    DWORD (*calc_3dbox_color) (DWORD color, int flag);

    /** The pointer to draw 3dbox function.
     *
     * \param flag, can be OR'ed by the following values:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *    Normal status.
     *  - LFRDR_BTN_STATUS_HILITE\n
     *    Highlight status.
     *  - LFRDR_BTN_STATUS_PRESSED\n
     *    Pushed status.
     *  - LFRDR_BTN_STATUS_DISABLED\n
     *    Disabled status.
     *  - LFRDR_BTN_STATUS_SELECTED\n
     *    Selected status.
     *
     *  - LFRDR_3DBOX_THICKFRAME\n
     *    Use thick/thin frame.
     *  - LFRDR_3DBOX_FILLED\n
     *    Filled rectangle.
     */
    void (*draw_3dbox) (HDC hdc, const RECT* pRect, DWORD color, DWORD flag);


    /** The pointer to draw radio function.
     *
     * \param status, can be OR'ed by the following values:
     *  - LFRDR_MARK_ALL_SELECTED\n
     *    All selected status.
     *  - LFRDR_MARK_HALF_SELECTED\n
     *    Half selected status.
     *  - LFRDR_MARK_HAVESHELL\n
     *    Have shell for mark.
     */
    void (*draw_radio) (HDC hdc, const RECT* pRect, DWORD color, int status);

    /** The pointer to draw checkbox function.
     *  \sa draw_radio
     */
    void (*draw_checkbox) (HDC hdc, const RECT* pRect, DWORD color,
            int status);

    /** The pointer to draw checkmark function.
     *  \sa draw_radio
     */
    void (*draw_checkmark) (HDC hdc, const RECT* pRect, DWORD color,
            int status);

    /** The pointer to draw arrow function.
     *
     * \param status, can be OR'ed by the following values:
     *  - LFRDR_ARROW_LEFT\n
     *    Left arrow.
     *  - LFRDR_ARROW_RIGHT\n
     *    Right arrow.
     *  - LFRDR_ARROW_UP\n
     *    Up arrow.
     *  - LFRDR_ARROW_DOWN\n
     *    Down arrow.
     *  - LFRDR_ARROW_NOFILL\n
     *    No filled arrow area.
     *  - LFRDR_ARROW_HAVESHELL\n
     *    Have shell for arrow.
     */
    void (*draw_arrow) (HWND hWnd, HDC hdc, const RECT* pRect, DWORD color, int status);

    /** The pointer to draw fold function, this function used
     *  by listview and treeview.
     *
     * \param status, can be OR'ed by the following values:
     *  - LFRDR_TREE_FOLD\n
     *    Draw opening/closing folder.
     *  - LFRDR_TREE_CHILD\n
     *    Draw connecting status between parent and child node.
     *  - LFRDR_TREE_NEXT\n
     *    Draw connecting status between current and next node.
     *
     *  \param next The number of child node.
     */
    void (*draw_fold) (HWND hWnd, HDC hdc, const RECT* pRect, DWORD color,
            int status, int next);

    /** The pointer to draw focus frame function.*/
    void (*draw_focus_frame) (HDC hdc, const RECT *pRect, DWORD color);

    /** The pointer to draw normal item function, this function used by listbox.*/
    void (*draw_normal_item) (HWND hWnd, HDC hdc, const RECT* pRect,
            DWORD color);

    /** The pointer to draw highlight item function, this function used by listbox.*/
    void (*draw_hilite_item) (HWND hWnd, HDC hdc, const RECT* pRect,
            DWORD color);

    /** The pointer to draw disabled item function, this function used by listbox. */
    void (*draw_disabled_item) (HWND hWnd, HDC hdc, const RECT* pRect,
            DWORD color);

    /** The pointer to draw significant item function, this function used by listbox.*/
    void (*draw_significant_item) (HWND hWnd, HDC hdc, const RECT* pRect,
            DWORD color);

    /** The pointer to draw push button function, this function used
     *  by button.
     *
     * \param status, can be OR'ed by the following values:
     *  - BST_NORMAL\n
     *  - BST_HILITE\n
     *  - BST_PUSHED\n
     *  - BST_DISABLE\n
     *  - BST_UNCHECKED\n
     *  - BST_CHECKED\n
     *  - BST_INDETERMINATE\n
     *  - BST_FOCUS\n
     */
    void (*draw_push_button) (HWND hWnd, HDC hdc, const RECT* pRect,
            DWORD color1, DWORD color2, int status);

    /** The pointer to draw radio button function, the function ued
     *  by button.
     *
     * \param status, can be OR'ed by the following values:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *    Normal status.
     *  - LFRDR_BTN_STATUS_HILITE\n
     *    Highlight status.
     *  - LFRDR_BTN_STATUS_PRESSED\n
     *    Pushed status.
     *  - LFRDR_BTN_STATUS_DISABLED\n
     *    Disabled status.
     *
     *  - LFRDR_BTN_STATUS_SELECTED\n
     *    Selected status.
     */
    void (*draw_radio_button) (HWND hWnd, HDC hdc, const RECT* pRect, int status);

    /** The pointer to draw check button function, this function used
     *  by button.
     *
     * \param status, can be OR'ed by the following values:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *    Normal status.
     *  - LFRDR_BTN_STATUS_HILITE\n
     *    Highlight status.
     *  - LFRDR_BTN_STATUS_PRESSED\n
     *    Pushed status.
     *  - LFRDR_BTN_STATUS_DISABLED\n
     *    Disabled status.
     *
     *  - LFRDR_BTN_STATUS_SELECTED\n
     *    Selected status.
     */
    void (*draw_check_button) (HWND hWnd, HDC hdc, const RECT* pRect, int status);

    /** The pointer to draw window border function.*/
    void (*draw_border) (HWND hWnd, HDC hdc, BOOL is_active);

    /** The pointer to draw window text and icon function.*/
    void (*draw_caption) (HWND hWnd, HDC hdc, BOOL is_active);

    /** The pointer to draw caption button function.
     *
     * \param ht_code The hittest code. If 0, it should draw all buttons.
     *  \param state The button state, can be one of the values:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *    Normal status.
     *  - LFRDR_BTN_STATUS_HILITE\n
     *    Highlight status.
     *  - LFRDR_BTN_STATUS_PRESSED\n
     *    Pushed status.
     *  - LFRDR_BTN_STATUS_DISABLED\n
     *    Disabled status.
     */
    void (*draw_caption_button) (HWND hwnd, HDC hdc, int ht_code, int state);

    /** The pointer to draw scrollbar function.
     *
     * \param sb_pos The position need to be paint.
     *   If 0, it should draw all the scrollbar.
     * \param status, which is one of the value:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *    Normal status.
     *  - LFRDR_BTN_STATUS_HILITE\n
     *    Highlight status.
     *  - LFRDR_BTN_STATUS_PRESSED\n
     *    Pushed status.
     *  - LFRDR_BTN_STATUS_DISABLED\n
     *    Disabled status.
     */
    void (*draw_scrollbar) (HWND hWnd, HDC hdc, int sb_pos);

    /** The pointer to get trackbar rect function. */
    void (*calc_trackbar_rect) (HWND hWnd, LFRDR_TRACKBARINFO *info,
            DWORD dwStyle, const RECT* rcClient, RECT* rcRuler,
            RECT* rcBar, RECT* rcBorder);

    /** The pointer to draw trackbar function. */
    void (*draw_trackbar) (HWND hWnd, HDC hdc, LFRDR_TRACKBARINFO *info);

    /** The pointer to calculate window element area function.
     *
     * \param which The hit test code of the part to be calculated.
     * \param we_area The reactangle for calculating result.
     *
     * */
    int (*calc_we_area) (HWND hWnd, int which, RECT* we_area);

    /** The pointer to calculate window element metrics function.
     *
     * \param which, can be one of the following values:
     *  - LFRDR_METRICS_BORDER\n
     *    The metrics of window border.
     *  - LFRDR_METRICS_CAPTION_H\n
     *    The height of window caption.
     *  - LFRDR_METRICS_MENU_H\n
     *    The height of window menu.
     *  - LFRDR_METRICS_VSCROLL_W\n
     *    The width of vertical scrollbar.
     *  - LFRDR_METRICS_HSCROLL_H\n
     *    The height of horizontal scrollbar.
     *  - LFRDR_METRICS_ICON_H\n
     *    The height of window icon.
     *  - LFRDR_METRICS_ICON_W\n
     *    The width of window icon.
     *  - LFRDR_METRICS_MINWIN_WIDTH\n
     *    The minimize width of window.
     *  - LFRDR_METRICS_MINWIN_HEIGHT\n
     *    The minimize height of window.
     */
    int (*calc_we_metrics) (HWND hWnd,
            LFRDR_WINSTYLEINFO* style_info, int which);

    /** The pointer to hittest function.
     *
     * \param x, y The position of the mouse in screen coordinates.
     * \return HT_XXX or HT_USER_XXX, hittest value.
     */
    int (*hit_test) (HWND hWnd, int x, int y);

    /** The pointer to user-defined hotspot function.*/
    int (*on_click_hotspot) (HWND hWnd, int which);

    /** The pointer to draw user-defined hotspot function.
     *
     * \param ht_code The hittest code. If 0, it should draw all
     *        user-defined hotspot.
     * \param state The button state, can be one of the values:
     *  - LFRDR_BTN_STATUS_NORMAL\n
     *  - LFRDR_BTN_STATUS_HILITE\n
     *  - LFRDR_BTN_STATUS_PRESSED\n
     */
    void (*draw_custom_hotspot) (HWND hWnd, HDC hdc, int ht_code, int state);

    /** The pointer to calculate scrollbar's thumb area function.
     *
     * \param vertical Vertical scrollbar or Horizontal scrollbar.
     * \param sb_info The scrollbar information for calculating result.
     *
     */
    void (*calc_thumb_area) (HWND hWnd, BOOL vertical,
            LFSCROLLBARINFO* sb_info);

    /** The pointer to output disabled text function.*/
    void (*disabled_text_out) (HWND hWnd, HDC hdc, const char* spText,
                    PRECT rc, DWORD dt_fmt);

    /** The pointer to draw propsheet's tab function.
     *
     * \param flag, can be OR'ed by the following values:
     *  - LFRDR_TAB_BOTTOM\n
     *    Use bottom tab.
     *  - LFRDR_TAB_ACTIVE\n
     *    Active tab.
     *  - LFRDR_TAB_ICON\n
     *    With icon.
     */
    void (*draw_tab) (HWND hWnd, HDC hdc, RECT *rect, char *title,
                DWORD color, int flag, HICON icon);

    /** The pointer to draw progressbar's chunk/bar function.*/
    void (*draw_progress) (HWND hWnd, HDC hdc,
            int nMax, int nMin, int nPos, BOOL fVertical);

    /** The pointer to draw table header function.
     *  this function used by listview and gridview.
     */
    void (*draw_header) (HWND hWnd, HDC hdc, const RECT* pRect, DWORD color);

    /** The pointer to get renderer private data function.
     *
     * \param we_attr_id The id pointed renderer private data.
     */
    DWORD (*on_get_rdr_attr) (struct _WINDOW_ELEMENT_RENDERER*, int we_attr_id);

    /** The pointer to set renderer private data function.
     *
     * \param we_attr_id The id pointed renderer private data.
     * \param we_attr The new renderer private data.
     *
     */
    DWORD (*on_set_rdr_attr) (struct _WINDOW_ELEMENT_RENDERER*, int we_attr_id, DWORD we_attr, BOOL change);

    /** The pointer to erase background function.*/
    void (*erase_background) (HWND hWnd, HDC hdc, const RECT *rect);

    /** The pointer to draw menu normal item function. */
    void (*draw_normal_menu_item) (HWND hWnd, HDC hdc, const RECT* pRect,
                        DWORD color);

    /** The pointer to draw menu highlight item function. */
    void (*draw_hilite_menu_item) (HWND hWnd, HDC hdc, const RECT* pRect,
                        DWORD color);

    /** The pointer to draw menu disabled item function. */
    void (*draw_disabled_menu_item) (HWND hWnd, HDC hdc, const RECT* pRect,
                        DWORD color);

    /** The window element metrics information. */
    int we_metrics [WE_METRICS_NUMBER];

    /** The window element color information. */
    DWORD we_colors [WE_COLORS_NUMBER][3];

    /** The window element font information. */
    PLOGFONT we_fonts [WE_FONTS_NUMBER];

    /** The renderer icon information. */
    HICON  we_icon [2][SYSICO_ITEM_NUMBER];

    /** The reference count. */
    unsigned int refcount;

    /** The private information. */
    const void* private_info;
} WINDOW_ELEMENT_RENDERER;

/**
 * The window information structure.
 *
 * \note The layout of this structure changed since 5.0.0 to
 *      support virtual window.
 */
typedef struct _WINDOWINFO
{
    unsigned char   _padding1;
    unsigned char   _padding2;
    unsigned short  _padding3;
    void*           _padding4;

    /** The caption of window.*/
    const char*     spCaption;

    /** The identifier of window.
      * \note The type changed from int to LINT since 3.2.
      */
    LINT            id;

    /** The window procedure */
    WNDPROC         WinProc;

    /** The notification callback procedure. */
    NOTIFPROC       NotifProc;

    /** The first additional data of this window */
    DWORD           dwAddData;
    /** The second additional data of this window */
    DWORD           dwAddData2;

    void*           _padding5;
    void*           _padding6;
    void*           _padding7;
    void*           _padding8;
    void*           _padding9;

    /** The position and size of the window. N/A for virtual window. */
    int left, top;
    int right, bottom;

    /** The position and size of client area. N/A for virtual window. */
    int cl, ct;
    int cr, cb;

    /** The styles of window. N/A for virtual window. */
    DWORD dwStyle;
    /** The extended styles of window. N/A for virtual window. */
    DWORD dwExStyle;

    /** The index of z-node for this window
      * (only for a main window and a control as main window. 
      * N/A for virtual window. */
    int idx_znode;

    /** The background pixel value of this window. N/A for virtual window. */
    gal_pixel iBkColor;

    /** The handle of menu. N/A for virtual window. */
    HMENU hMenu;
    /** The handle of accelerator table. N/A for virtual window. */
    HACCEL hAccel;
    /** The handle of cursor. N/A for virtual window. */
    HCURSOR hCursor;
    /** The handle of icon. N/A for virtual window. */
    HICON hIcon;
    /** The handle of system menu. N/A for virtual window. */
    HMENU hSysMenu;
    /** The pointer to logical font. N/A for virtual window. */
    PLOGFONT pLogFont;

    /** The vertical scrollbar information. N/A for virtual window. */
    LFSCROLLBARINFO vscroll;
    /** The horizontal scrollbar information. N/A for virtual window. */
    LFSCROLLBARINFO hscroll;

    /** The window renderer. N/A for virtual window. */
    WINDOW_ELEMENT_RENDERER* we_rdr;
} WINDOWINFO;

/**
 * \fn const WINDOWINFO* GUIAPI GetWindowInfo (HWND hWnd)
 * \brief Get the handle of window information.
 *
 * This function gets the handle to the window information \
 * from the specified window handle \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The handle to the window information.
 */
static inline const WINDOWINFO* GUIAPI GetWindowInfo (HWND hWnd)
{
    return (WINDOWINFO*)hWnd;
}

/**
 * \fn BOOL GUIAPI InitWindowElementAttrs (PWERENDERER rdr)
 *
 * This function initializes window element attributes of the
 * specified renderer \a rdr according to the appropriate section
 * in configuration file.
 *
 * \param rdr The pointer to the specified renderer.
 *
 * \return If succeed, return TRUE; otherwise return FALSE.
 */
MG_EXPORT BOOL GUIAPI InitWindowElementAttrs (PWERENDERER rdr);

/**
 * \fn DWORD GUIAPI GetWindowElementAttr (HWND hwnd, int we_attr_id)
 * \brief Get a window element data.
 *
 * This function gets a window element data which is identified by
 * \a we_attr_id.
 *
 * \param hwnd The handle to the window.
 * \param we_attr_id The window element id number, can be one of the
 *        following values:
 *
 *      - WE_FONT_CAPTION\n
 *        The caption font.
 *      - WE_FONT_MENU\n
 *        The menu font. Used by menu and listbox.
 *      - WE_FONT_MESSAGEBOX\n
 *        The messagebox font. Resevered.
 *      - WE_FONT_TOOLTIP\n
 *        The tooltip font. Used by tooltip window.
 *
 *      - WE_METRICS_CAPTION\n
 *        The caption size.
 *      - WE_METRICS_CAPTION_BUTTON\n
 *        The caption button size.
 *      - WE_METRICS_MENU\n
 *        The menu size.
 *      - WE_METRICS_WND_BORDER\n
 *        The border size.
 *      - WE_METRICS_SCROLLBAR\n
 *        The scrollbar size.
 *
 *      - WE_FGC_ACTIVE_CAPTION\n
 *        The active caption foreground color.
 *      - WE_BGCA_ACTIVE_CAPTION\n
 *        The active caption background color A.
 *      - WE_BGCB_ACTIVE_CAPTION\n
 *        The active caption background color B.
 *      - WE_FGC_MENU\n
 *        The menu foreground color.
 *      - WE_BGC_MENU\n
 *        The menu background color.
 *      - WE_FGC_MESSAGEBOX\n
 *        The messagebox foreground color. Reserved.
 *      - WE_FGC_TOOLTIP\n
 *        The tooltip foreground color. Only used by tooltip window.
 *      - WE_BGC_TOOLTIP\n
 *        The tooltip background color. Only used by tooltip window.
 *      - WE_FGC_ACTIVE_WND_BORDER\n
 *        The active border foreground color.
 *      - WE_FGC_INACTIVE_WND_BORDER\n
 *        The inactive border foreground color.
 *      - WE_FGC_INACTIVE_CAPTION\n
 *        The inactive caption foreground color.
 *      - WE_BGCA_INACTIVE_CAPTION\n
 *        The inactive caption background color A.
 *      - WE_BGCB_INACTIVE_CAPTION\n
 *        The inactive caption background color B.
 *      - WE_FGC_WINDOW\n
 *        The window foreground color. Used by window and control.
 *      - WE_BGC_WINDOW\n
 *        The window background color. Used by window and control.
 *      - WE_FGC_THREED_BODY\n
 *        The 3dbox foreground color. Mainly used by drawing 3dbox
 *        object, such as dialog and button, etc.
 *      - WE_MAINC_THREED_BODY\n
 *        The 3dbox background color. Mainly used by drawing menubar,
 *        dialog, header and button, etc.
 *      - WE_FGC_SELECTED_ITEM\n
 *        The selected item foreground color. Used by menu and control.
 *      - WE_BGC_SELECTED_ITEM\n
 *        The selected item background color. Used by menu and control.
 *      - WE_BGC_SELECTED_LOSTFOCUS\n
 *        The lostfocus selected item background color. Only Used by mledit.
 *      - WE_FGC_DISABLED_ITEM\n
 *        The disabled item foreground color. Used by menu and control.
 *      - WE_BGC_DISABLED_ITEM\n
 *        The disabled item background color. Used by menu and control.
 *      - WE_FGC_HIGHLIGHT_ITEM\n
 *        The highlight item foreground color. Used by menu and control.
 *      - WE_BGC_HIGHLIGHT_ITEM\n
 *        The highlight item background color. Used by menu and control.
 *      - WE_FGC_SIGNIFICANT_ITEM\n
 *        The significant item foreground color. Used by listbox.
 *      - WE_BGC_SIGNIFICANT_ITEM\n
 *        The significant item background color. Used by listbox.
 *      - WE_BGC_DESKTOP\n
 *        The desktop background color. Only used by desktop.
 *
 * \return If succeed, return the window element data; otherwise return -1.
 *
 * \sa SetWindowElementAttr
 */
MG_EXPORT DWORD GUIAPI GetWindowElementAttr (HWND hwnd, int we_attr_id);

/**
 * \fn DWORD GUIAPI SetWindowElementAttr (HWND hwnd, int we_attr_id, \
                        DWORD we_attr)
 * \brief Set a new window element data specified by \a we_attr_id.
 *
 * This function sets a new window element data \a we_attr to MiniGUI,
 * which is identified by \a we_attr_id.
 *
 * \param hwnd The handle to the window.
 * \param we_attr_id The window element id.
 * \param we_attr The new window element data.
 *
 * \return If succeed, return the old window element data; otherwise return -1.
 *
 * \sa GetWindowElementAttr
 */
MG_EXPORT DWORD GUIAPI SetWindowElementAttr (HWND hwnd, int we_attr_id,
        DWORD we_attr);

/**
 * \fn gal_pixel GUIAPI GetWindowElementPixelEx (HWND hwnd, \
                HDC hdc, int we_attr_id)
 * \brief Get the pixel value of a window element.
 *
 * This function gets the pixel value of a window element which is identified
 * by \a we_attr_id and \a hdc.
 *
 * \param hwnd The handle to the window.
 * \param hdc The device context. If -1, it will return pixel value according
 *            to window handle \a hwnd; otherwise return pixel value according
 *            to pixel format in \a hdc.
 * \param we_attr_id The window element id number.
 *
 * \return If succeed, return the window element pixel color data; otherwise
 *         return -1.
 *
 * \sa GetWindowElementAttr
 */
MG_EXPORT gal_pixel GUIAPI GetWindowElementPixelEx (HWND hwnd,
        HDC hdc, int we_attr_id);

/**
 * \fn WINDOW_ELEMENT_RENDERER* GUIAPI GetWindowRendererFromName (const char* name)
 * \brief Get window renderer from name.
 *
 * \param name The renderer name.
 *
 * This function gets window renderer from the specified name \a name,
 * which is case-insensitive.
 *
 * \return The pointer to the window renderer for success, NULL for failure.
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one returns
 *      a const value.
 */
MG_EXPORT WINDOW_ELEMENT_RENDERER*
    GUIAPI GetWindowRendererFromName (const char* name);

/**
 * \fn BOOL GUIAPI AddWindowElementRenderer (const char* name, \
                        const WINDOW_ELEMENT_RENDERER* we_rdr)
 * \brief Add a window element renderer to the system.
 *
 * This function adds a new window renderer to MiniGUI system.
 *
 * \param name The renderer name.
 * \param we_rdr The pointer to a new renderer.
 *
 * \return TRUE for success, FALSE for failure.
 */
MG_EXPORT BOOL GUIAPI AddWindowElementRenderer (const char* name,
        const WINDOW_ELEMENT_RENDERER* we_rdr);

/**
 * \fn BOOL BOOL GUIAPI RemoveWindowElementRenderer (const char* name)
 * \brief Remove a window renderer from MiniGUI.
 *
 * This function only removes a window renderer named by \a name.
 *
 * \param name The renderer name.
 *
 * \return TRUE for success, FALSE for failure.
 */
MG_EXPORT BOOL GUIAPI RemoveWindowElementRenderer (const char* name);

/**
 * \fn WINDOW_ELEMENT_RENDERER* GUIAPI GetDefaultWindowElementRenderer (void)
 * \brief Get the default window renderer.
 *
 * This function gets the default window renderer in MiniGUI.
 *
 * \return the pointer to the default renderer.
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one returns
 *      a const value.
 */
MG_EXPORT WINDOW_ELEMENT_RENDERER*
        GUIAPI GetDefaultWindowElementRenderer (void);

/**
 * \fn const char* GUIAPI SetDefaultWindowElementRenderer (const char* name)
 * \brief Set a window element renderer as the default system renderer.
 *
 * This function sets a window element renderer as the default system
 * renderer.
 *
 * \param name The renderer name.
 *
 * \return the old renderer's name for success, NULL for failure.
 */
MG_EXPORT const char* GUIAPI SetDefaultWindowElementRenderer (const char* name);

/**
 * \fn BOOL GUIAPI SetWindowElementRenderer (HWND hwnd, \
                            const char* werdr_name, \
                            const WINDOW_ELEMENT_ATTR* we_attrs)
 *
 * \brief Set a window element renderer and window element attributes
 *        for window specified by \a hwnd.
 *
 * This function sets the window handled by \a hWnd to use a different
 * window element renderer OR'ed window element attributes and updates
 * window in time.
 *
 * \param hwnd The handle to the window.
 * \param werdr_name The renderer name.
 * \param we_attrs The pointer to new window element attributes.
 *
 * \return TRUE for success, FALSE for failure.
 */
MG_EXPORT BOOL GUIAPI SetWindowElementRenderer (HWND hWnd,
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs);

/**
 * \fn BOOL GUIAPI InitRendererSystemIcon (const char* rdr_name, \
                            HICON *large_icon, HICON *small_icon);
 * \brief Initialize large and small system icons in the specified renderer
 *        \a rdr_name.
 *
 * \param rdr_name The renderer name.
 * \param large_icon The pointer to large system icons.
 * \param small_icon The pointer to small system icons.
 *
 * \return TRUE for success, FALSE for failure.
 */
MG_EXPORT BOOL InitRendererSystemIcon (const char* rdr_name,
        HICON *small_icon, HICON *large_icon);

/**
 * \fn void GUIAPI TermRendererSystemIcon (HICON *large_icon, \
        HICON *small_icon);
 * \brief Release large and small system icon resources.
 *
 * \param large_icon The pointer to large system icons.
 * \param small_icon The pointer to small system icons.
 *
 * \sa InitRendererSystemIcon
 */
MG_EXPORT void TermRendererSystemIcon (HICON *small_icon,
        HICON *large_icon);

    /** @} end of lf_fns */

    /**
     * \defgroup res_manage Resource managment functions
     * @{
     */
/**
 * \fn BOOL GUIAPI RegisterResFromFile (HDC hdc, const char* file);
 * \brief Register a device-dependent bitmap from a file.
 *
 * This function load a device-dependent bitmap from a file and register it to
 * bitmap cache.
 *
 * \param hdc The device context.
 * \param file The name of image file. The suffix indicates image type such as
 *  bmp, png, jpg etc.
 *
 * \return TRUE on success, FALSE on error.
 *
 */
MG_EXPORT BOOL GUIAPI RegisterResFromFile (HDC hdc, const char* file);


/**
 * \fn BOOL GUIAPI RegisterResFromMem (HDC hdc, const char* file, \
 *              const unsigned char* data, size_t data_size);
 *
 * \brief Register a device-dependent bitmap to bitmap cache from memory.
 *
 * This function load a device-dependent bitmap from memory and register it to
 * bitmap cache.
 *
 * \param hdc The device context.
 * \param file The name of image file. The suffix indicates image type such as
 *  bmp, png, jpg etc.
 * \param data The pointer to image data.
 * \param data_size The size of image data.
 *
 * \return TRUE on success, FALSE on error.
 *
 */
MG_EXPORT BOOL GUIAPI RegisterResFromMem (HDC hdc, const char* file,
        const unsigned char* data, size_t data_size);

/**
 * \fn BOOL GUIAPI RegisterResFromBitmap (const char* file, const BITMAP* bmp);
 * \brief Register a device-dependent bitmap to bitmap cache.
 *
 * This function register a device-dependent bitmap to bitmap cache.
 *
 * \param file The name of image file. The suffix indicates image type such as
 *  bmp, png, jpg etc.
 * \param bmp The pointer to a BITMAP object.
 *
 * \return TRUE on success, FALSE on error.
 *
 */
MG_EXPORT BOOL GUIAPI RegisterResFromBitmap (const char* file, const BITMAP* bmp);

/**
 * \fn const BITMAP* RetrieveRes (const char *file)
 * \brief Get a BITMAP object from cache according to the specified resource
 * file name.
 *
 * This function gets a BITMAP object from cache according to the specified
 * resource file name \a file.
 *
 * \param file The specified file name.
 *
 * \return a BITMAP object for success, NULL for error.
 */
MG_EXPORT const BITMAP* GUIAPI RetrieveRes (const char *file);

/**
 * \fn void UnregisterRes (const char *file)
 * \brief Unregister the BITMAP object from cache according to the specified
 * resource file name.
 *
 * This function unregister the BITMAP object from cache according to the
 * specified resource file name \a file and the BITMAP \a bmp.
 *
 * \param file The specified file name.
 *
 * \return None.
 *
 */
MG_EXPORT void GUIAPI UnregisterRes (const char *file);

/**
 * \fn BOOL GUIAPI RegisterSystemBitmap (HDC hdc, const char* rdr_name,
 *      const char* id);
 * \brief Register a device-dependent bitmap from id to BITMAP cache.
 *
 * This function loads a device-dependent bitmap for the specified window
 * elemeent renderer named \a rdr_name and the identifier \a id, and registers
 * it to the system BITMAP cache.
 *
 * \param hdc The device context.
 * \param rdr_name The name of window element renderer. NULL for default
 *        renderer.
 * \param id The identifier of the system bitmap.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa UnregisterSystemBitmap
 */
MG_EXPORT BOOL GUIAPI RegisterSystemBitmap (HDC hdc, const char* rdr_name,
        const char* id);

/**
 * \fn void GUIAPI UnregisterSystemBitmap (HDC hdc, const char* rdr_name,
 *      const char* id);
 * \brief Unregister a BITMAP object from the system BITMAP cache.
 *
 * This function unregisters the BITMAP object identified by \a id for the
 * window element renderer named by \a rdr_name.
 *
 * \param hdc The device context.
 * \param rdr_name The name of window element renderer. NULL for default
 *        renderer.
 * \param id The id of system image.
 *
 * \sa RegisterSystemBitmap
 */
MG_EXPORT void GUIAPI UnregisterSystemBitmap (HDC hdc, const char* rdr_name,
        const char* id);

/* define the key of resource */
typedef DWORD RES_KEY;
#define RES_KEY_INVALID 0

/* define the incore res type */
typedef struct _INNER_RES {
    RES_KEY key;
    const Uint8* data;
    size_t data_len;

    /* A special param recognized by the TYPE_OPS; normally is NULL.
     * If the data is a raw png, jpeg, bmp file content,
     * it should be the extention name of the filename:
     * INNER_RES res[]= { ... { ..., ... ,.., "png" } .. };
     * */
    const void* additional;
} INNER_RES;

/** The type of resource */
enum emResType {
    RES_TYPE_INVALID = 0xFF,

    /**
     * Images supported by MiniGUI, such as PNG, JPEG, BMP and so on.
     * Fot this type,  LoadResource returns a BITMAP object.
     */
    RES_TYPE_IMAGE = 0,

    /**
     * All kinds of image supported by MiniGUI.
     * For this type, LoadResource returns a MYBITMAP object.
     */
    RES_TYPE_MYBITMAP,

    /**
     * The icon file or data.
     * For this type, LoadResource loads the icon and returns HICON.
     */
    RES_TYPE_ICON,

#ifdef _MGHAVE_CURSOR
    /**
     * The cursor file or data.
     * For this type, LoadResource returns HCURSOR.
     */
    RES_TYPE_CURSOR,
#endif

    /**
     * The runtime config file or data support by MiniGUI.
     * For this type, LoadResource returns GHANDLE.
     * You can use it for \a GetValueFromEtc and \a GetIntValueFromEtc.
     */
    RES_TYPE_ETC,

    /* NOT IMPLEMENT */
    RES_TYPE_TEXT,
    /* NOT IMPLEMENT */
    RES_TYPE_MENU,
    /* NOT IMPLEMENT */
    RES_TYPE_WINDOW,

    /**
     * The type for inner resource..
     * Fot this type, LoadResource returns the raw data of INNER_RES,
     * it's a pointer of MEM_RES.
     * Only support incore Resource, if the resource is stored in file
     * LoadResource will return NULL.
     */
    RES_TYPE_MEM_RES,

    /**
     * Make the LoadResource return a logfont object.
     */
    RES_TYPE_FONT,

    /**
     * The beginning of user defined resource type.
     */
    RES_TYPE_USER,

    /*
     * The max valid value of resource type;
     * the user defined type must less this value.
     */
    RES_TYPE_USER_MAX = 0x7F
};

/* the return value of LoadResource with type RES_TYPE_MEM_RES */
typedef struct _MEM_RES {
    Uint8 *data;
    size_t data_len;
} MEM_RES;

typedef struct _FONT_RES {
    LOGFONT logfont;
    RES_KEY key;
} FONT_RES;

/* the original source of resource
 * depend on the values, the res manager
 * would select corrent loader to
 * load resource form disk or mem
 */

#define REF_SRC_NOTYPE 0x00

/* indicate that the resource is stored in disk */
#define REF_SRC_FILE 0x01

/* indicate that the resource is stored in memory*/
#define REF_SRC_INNER 0x02

/* indicate that the resource is stored in shared memory*/
#define REF_SRC_SHARED 0x03

/* indicate that the resource is stored in outside( NOT SUPPORTED) */
#define REF_SRC_OUTSIDE 0x04

/* indicate that the resource is stored in a stream object (NOT SUPPORTED) */
#define REF_SRC_STREAM 0x05

/* indicate that the resource is create by MiniGUI */
#define REF_SRC_LOGIC 0x06

/* the RESOUCE type for loadding */
typedef struct _RESOURCE {
    void* data; // store the pointer of cached resource
    union {
        void* src;
        INNER_RES* inner;
        char* file;
    } source; // indicate the stored format of resource
} RESOURCE;

/* The operations of one type of resource */
typedef struct _RES_TYPE_OPS {
    /* To get a resource
     * return the buffered resource pointer
     * param res the in and out param of resource.
     * param src_type, one of REF_SRC_XXX
     * param usr_param
     * */
    void* (*get_res_data)(RESOURCE* res, int src_type, DWORD usr_param);
    /* To unload a buffered resource
     */
    void (*unload)(RESOURCE* res, int src_type);
} RES_TYPE_OPS;

/* Return value and error code of resource manager */
enum emResReturn {
    RES_RET_OK = 0,
    RES_RET_INVALID_PARAM,
    RES_RET_NOT_EXIST_KEY, //RES_KEY is not exist
    RES_RET_LOAD_FILE_FAILED,
    RES_RET_LOAD_MEM_FAILED,
    RES_RET_UNKNOWN_TYPE,
    RES_RET_NOT_IMPLEMENTED,
    RES_RET_INUSED //the res or object is in used, cannot be unloaded
};

/**
 * \fn const char* GetResPath (void)
 * \brief Get the currrent user's resource path.
 *
 * \return the user's resource path; NULL if have not been set.
 *
 * \sa SetResPath
 *
 **/
MG_EXPORT const char* GetResPath (void);

/**
 * \fn int SetResPath (const char* path);
 * \brief Set user's resource path into the search paths of resource manager.
 *
 * If you set this value, resource mamanger will try to find a resource in the path.
 *
 * \param path The path which store the resources. It can be an absolute path or relative path.
 *
 * \return RES_RET_OK for success or error code.
 *
 * \sa GetResPath
 *
 **/
MG_EXPORT int SetResPath (const char* path);

/**
 * \fn int AddInnerRes (const INNER_RES* inner_res, int count, BOOL copy)
 * \brief Add some incore resource into the resource manager in order to
 * call LoadResource to get those resource.
 *
 * \param inner_res The pointer to the incore resource entry array.
 * \param count The number of the inner resource entries.
 * \param copy TRUE indicate that resource manager will copy the entries of
 *        the inner resource array, or the resource manager will just save the
 *        pointer.
 *
 * \return RES_RET_OK for success or error code.
 */
MG_EXPORT int AddInnerRes (INNER_RES* inner_res, int count, BOOL copy);

/*
 *  NOT IMPLEMENT
 */
MG_EXPORT int AddSharedRes (const char* shared_name);

/**
 * \fn int RegisterResType (int type, RES_TYPE_OPS* ops)
 * \brief Register a new user-defined type into resource manager,
 * so that the LoadResource can load the special resource automatically.
 *
 * \param type The user-defined type. It must >=RES_TYPE_USER and <RES_TYPE_USER_MAX
 * \param ops the operations of the resource.
 *
 * \return RES_RET_OK for success or error code
 *
 * \sa RES_TYPE_OPS, enum emResType, UnregisterResType
 */
MG_EXPORT int RegisterResType (int type, RES_TYPE_OPS* ops);

/**
 * \fn int UnregisterResType (int type)
 * \brief Unregister a user-defined resource type
 *
 * \param type The user-defined type registered by calling RegisterResType.
 *
 * \return RES_RET_OK or error code
 *
 * \sa RegisterResType
 */
MG_EXPORT int UnregisterResType (int type);

/**
 * \fn void* LoadResource (const char* res_name, int type, DWORD usr_param)
 * \brief Load a resource from disk or memory.
 *
 * This function loads a resource from disk or memory. If the resource is loaded,
 * it will return the buffered one and increase the reference count. This
 * function will try to find a loader for the resource first according to
 * \a type.
 *
 * \param res_name The resource name. If res_name is an absolute path
 *        (the leading character is '/'), it will load resource from disk directly,
 *        else it will:
 *         - step 1) try to find the resource in loaded buffer.
 *         - step 2) if step 1 failed, try to load from incore memory.
 *         - step 3) if step 2 failed, try to find the file in the path set by SetResPath and load it.
 *         - step 4) if step 3 failed, try to find the file in the path where MiniGUI.cfg locates.
 *         - step 5) return NULL if all steps faild
 *
 * \param type The resource type what you want to load. It must be one of the following values:
 *         - RES_TYPE_IMAGE: A BITMAP object (BITMAP*).
 *         - RES_TYPE_MYBITMAP: A MYBITMAP object (MYBITMAP*).
 *         - RES_TYPE_ICON: An icon (HICON).
 *         - RES_TYPE_CURSOR: A cursor (HCURSOR).
 *         - RES_TYPE_ETC: A configuration (GHANDLE).
 *         - RES_TYPE_TEXT: NOT implemented.
 *         - RES_TYPE_MENU: NOT implemented.
 *         - RES_TYPE_WINDOW: NOT implemented.
 *         - RES_TYPE_MEM_RES: A in-memory resource (MEM_RES*).
 *         - RES_TYPE_USER ~ RES_TYPE_USER_MAX: A user-defined resource type (void*).
 *
 * \param usr_param The param pass to RES_TYPE_OPS.get_res_data. For resource type
 *        RES_TYPE_IMAGE and RES_TYPE_ICON, it must be a handle to DC.
 *
 * \return The pointer of resource, see the parameter type.
 *
 * \sa enum emResType, MEM_RES, SetResPath, AddInnerRes, RegisterResType, GetResource
 */
MG_EXPORT void* LoadResource (const char* res_name, int type, DWORD usr_param);

/**
 * \fn void* GetResource (RES_KEY key);
 * \brief Retrieve and return a buffered resource by the key.
 *
 * You should call LoadResource firstly, and then call GetResource when you need it.
 * Note that GetResource will not increase the reference count automatically.
 *
 * \param key The key of the resource (use Str2Key(res_name) to get the key).
 *
 * \return the pointer to the resource; NULL if the resource does not exist.
 *
 * \sa LoadResource, Str2Key
 */
MG_EXPORT void* GetResource (RES_KEY key);

/**
 * \fn int AddResRef(RES_KEY key);
 *
 * \brief Increase the reference count of a buffered resource.
 *
 *  When you get a resource, you should call this function to ensure that
 *  the resource can not be unloaded when you still use it.
 *  This function must be called after GetResource.
 *
 *  \param key The key of resource (use Str2Key(res_name) to get the key).
 *
 *  \return The reference count; -1 if the resource does not exist.
 *
 * \sa LoadResource, GetResource, ReleaseRes, Str2Key
 */
MG_EXPORT int AddResRef (RES_KEY key);

/**
 * \fn int ReleaseRes (RES_KEY key);
 * \brief Release the buffered resource. It will decrease the reference count of
 * the resource. If the reference count is less then 0, the resource will be released.
 *
 * \param key The key of the resource (use Str2Key(res_name) to get the key).
 *
 * \return The reference count; -1 if the resource does not exist.
 *
 * \sa LoadResource, GetResource, AddResRef, Str2Key
 */
MG_EXPORT int ReleaseRes (RES_KEY key);

#define LoadMyBitmapFromRes(res_name, pal)                      \
    (MYBITMAP*)LoadResource(res_name,                           \
            RES_TYPE_MYBITMAP, (DWORD)(pal))

#define GetMyBitmapFromRes(key)                                 \
    (MYBITMAP*)GetResource(key)

#define LoadBitmapFromRes(hdc, res_name)                        \
    (BITMAP*)LoadResource(res_name, RES_TYPE_IMAGE, (DWORD)hdc)

#define GetBitmapFromRes(key)                                   \
    (BITMAP*)GetResource(key)

#define GetIconFromRes(key)                                     \
    (HICON)GetResource(key)

#ifndef _MGHAVE_CURSOR
#define GetCursorFromRes(key)                                   \
    (HCURSOR)GetResource(key)
#endif

#define GetEtcFromRes(key)                                      \
    (GHANDLE)GetResource(key)

#define LoadLogicFontFromRes(font_name)                         \
    (PLOGFONT)LoadResource(font_name, RES_TYPE_FONT, 0)

#define GetLogicFontFromRes(font_name)                          \
    (PLOGFONT)GetResource(Str2Key(font_name))

#define ReleaseLogicFont(font)                                  \
    ReleaseRes(((FONT_RES*)(font))->key)

#define ReleaseLogicFontByName(font_name)                       \
    ReleaseRes(Str2Key(font_name))

/**
 * \fn RES_KEY Str2Key (const char* str);
 * \brief Translate a string to an unsigned long (RES_KEY), which is used to
 * find a resource in the resource manager.
 *
 * \param str The name of resource. Normaly, it's a path and filename of the resource.
 *
 * \return The key.
 *
 * \sa GetResource, AddResRef, ReleaseRes
 */
MG_EXPORT RES_KEY Str2Key (const char* str);


#ifdef __TARGET_MSTUDIO__
/**
 * \fn int GUIAPI GetWindowZOrder(HWND hWnd);
 * \brief Get The Control ZOrder in the parent window
 *
 * \param hWnd Handler of a control
 *
 * \return return index of ZOrder or 0 if hWnd is a MainWindow or -1 if error
 *
 */
MG_EXPORT int GUIAPI GetWindowZOrder(HWND hWnd);

/**
 * \fn int GUIAPI SetWindowZOrder(HWND hWnd, int zorder);
 * \brief Set The Control ZOrder in the parent window
 *
 * \param hWnd handler of a control
 * \param zorder the index of ZOrder
 *
 * \return return the new index of ZOrder or 0 if hWnd is a MainWindow
 */
MG_EXPORT int GUIAPI SetWindowZOrder(HWND hWnd, int zorder);
#endif /* defined __TARGET_MSTUDIO__ */

    /** @} end of res_manage */

    /**
     * \defgroup window_fns Windowing functions
     * @{
     */

#ifdef _MGHAVE_VIRTUAL_WINDOW
    /**
     * \defgroup virtual_window_fns Virtual Window
     *
     * You know that we can post or send a message to other windows which
     * may run in another thread under MiniGUI-Threads. The MiniGUI
     * messaging functions such as \a PostMessage(), \a SendMessage(),
     * \a SendNotifyMessage(), and the window callback procedure
     * provide a flexible, efficient, safe, and easy-to-use data transfer
     * and synchronization mechanism for your multithreaded applications.
     *
     * For example, you can send or post a message to a window from a
     * general purpose thread which may download a file from the remote
     * server under MiniGUI-Threads.
     *
     * But can we use the MiniGUI messaging mechanism under
     * MiniGUI-Processes and MiniGUI-Standalone runtime modes for
     * multithreading purpose? For example, we may download a file in a
     * general thread and inform a window when the file is ready.
     *
     * Furthermore, if we want to use the MiniGUI messaging mechanism in
     * a general thread to handle messages from other threads, how to do this?
     *
     * The virtual window provides a solution for the requirements above.
     * A virtual window is a special window object which does not have
     * a visible window area. But after you create a virtual window in
     * a different thread, you can use the MiniGUI messaging mechanism
     * to post or send messages between the current main window thread
     * and the new thread.
     *
     * In MiniGUI, we call a thread creating a main window as a GUI thread,
     * and a thread creating a virtual window as a message thread.
     *
     * It is important to know the following key points about virtual
     * window:
     *
     *  - It is enabled automatically under MiniGUI-Threads runtime mode.
     *  - It can be enabled by using the compile-time configuration option
     *    `--enable-virtualwindow`, or define `_MGHAVE_VIRTUAL_WINDOW` macro
     *    under MiniGUI-Processes and MiniGUI-Standalone runtime modes.
     *  - You can create multiple GUI threads under MiniGUI-Threads, but you
     *    cannot create multiple GUI threads under MiniGUI-Processes and
     *    MiniGUI-Standalone runtime modes.
     *  - Regardless of the runtime mode, you can create multiple message
     *    threads, and you can also create multiple virtual windows in
     *    one message thread.
     *  - It is possible to create a virtual window in a GUI thread, although
     *    we do not encourage to do this. In other words, a GUI
     *    thread is also a message thread. On the other hand, you cannot
     *    create a main window in a message thread.
     *  - Essentially, a virtual window is a simplified main window.
     *    It consumes very little memory space, but provides a complete
     *    MiniGUI messaging mechanism for a general multithreaded app.
     *  - When virtual window is enabled (or under MiniGUI-Threads runtime mode),
     *    you can use the MiniGUI messaging facilities to post or send messages
     *    to a window, or notify a window from a general thread.
     *
     * A virtual window will get the following system messages in its life
     * life-cycle:
     *
     *  - MSG_CREATE: this message will be sent to the virtual window when
     *    you call \a CreateVirtualWindow function.
     *  - MSG_CLOSE: this message will be sent to the virtual window when
     *    the system asks to close the virtual window.
     *  - MSG_DESTROY: this message will be sent to the virtual window when
     *    the system tries to destroy the virtual window, or after you
     *    called \a DestroyVirtualWindow function.
     *  - MSG_IDLE: When there is no any message in the message queue, all
     *    virtual windows living in the message thread will get this idle
     *    message.
     *  - MSG_TIMER: When a timer expired after you call \a SetTimer to
     *    set up a timer for a virtual window.
     *  - MSG_QUIT: quit the message loop.
     *  - MSG_GETTEXT: Send to window procedure to get the caption.
     *  - MSG_SETTEXT: Send to window procedure to set the caption.
     *  - MSG_GETTEXTLENGTH: Send to window procedure to get the length of caption.
     *  - MSG_FDEVENT: Send to window procedure when there is a read/write/except
     *    event on a listened file descriptor.
     *
     * The following functions work for a virtual window:
     *
     *  - DefaultWindowProc
     *  - GetWindowId
     *  - SetWindowId
     *  - GetThreadByWindow
     *  - GetWindowAdditionalData
     *  - SetWindowAdditionalData
     *  - GetWindowAdditionalData2
     *  - SetWindowAdditionalData2
     *  - GetClassName: always returns 'VIRTWINDOW' for a virtual window.
     *  - GetWindowCallbackProc
     *  - SetWindowCallbackProc
     *  - GetWindowCaption
     *  - SetWindowCaption
     *  - GetWindowTextLength
     *  - GetWindowText
     *  - SetWindowText
     *  - GetNotificationCallback
     *  - SetNotificationCallback
     *  - SetWindowLocalData
     *  - GetWindowLocalData
     *  - RemoveWindowLocalData
     *  - RegisterEventHookWindow
     *  - UnregisterEventHookWindow
     *  - RegisterKeyHookWindow
     *  - RegisterMouseHookWindow
     *  - RegisterListenFD
     *  - UnregisterListenFD
     *
     * Since 5.0.0.
     *
     * @{
     */

/**
 * \fn int GUIAPI CreateThreadForMessaging (pthread_t* thread,
 *      pthread_attr_t* attr, void * (*start_routine)(void *), void* arg,
 *      BOOL joinable, size_t stack_size)
 * \brief Create a message thread for main windows or virtual windows.
 *
 * The function creates a message thread for main windows or virtual windows
 * by calling the POSIX thread function \a pthread_create(), and setting up
 * the message queue for the thread.
 *
 * \param thread The buffer to return the thread identifier if
 *      the thread was successfully created.
 * \param attr The pointer to the thread attribute.
 * \param start_routine The entry function of the thread.
 * \param arg The argument will be passed to \a start_routine.
 * \param joinable Whether to create a joinable thread.
 * \param stack_size The stack size in kilobytes, zero for default (4KiB).
 *
 * \return On success, it returns 0; on error, an error number returned.
 *      Exception the error numbers defined by pthread_create(), this
 *      function may return ENOMEM when failed to allocate memory for
 *      message queue.
 *
 * \note The last two arguments work only when \a attr is NULL.
 *
 * \sa pthread_create
 *
 * Since 5.0.0
 */
MG_EXPORT int GUIAPI CreateThreadForMessaging (pthread_t* thread,
        pthread_attr_t* attr, void * (*start_routine)(void *), void* arg,
        BOOL joinable, size_t stack_size);

/**
 * \fn BOOL GUIAPI GetThreadByWindow (HWND hWnd, pthread_t* thread)
 * \brief Get the thread identifier which a window belongs to.
 *
 * \param hWnd The handle to a window, which may be a main window,
 *      virtual window, or a control.
 * \param thread The buffer to store the thread identifier.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI GetThreadByWindow (HWND hWnd, pthread_t* thread);

/**
 * \fn BOOL GUIAPI IsWindowInThisThread (HWND hWnd)
 * \brief Determine whether a window was created in this thread.
 *
 * \param hWnd The handle to a window, which may be a main window,
 *      virtual window, or a control.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI IsWindowInThisThread (HWND hWnd);

/**
 * \fn BOOL GUIAPI VirtualWindowCleanup (HWND hVirtWnd)
 * \brief Cleanup the system resource associated with a virtual window.
 *
 * This function cleans up the system resource such as the message queue
 * associated with the virual window \a hVirtWnd. \a DestroyVirtualWindow
 * does not destroy all resource used by the virtual window, therefore,
 * you should call this function after calling \a DestroyVirtualWindow and
 * skipping out from the message loop. After calling this function, the
 * virtual window object will be destroyed actually.
 *
 * \param hVirtWnd The handle to the virtual window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa DestroyVirtualWindow
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI VirtualWindowCleanup (HWND hVirtWnd);

/**
 * \fn HWND GUIAPI CreateVirtualWindow (HWND hHosting, WNDPROC WndProc,
 *      const char* spCaption, LINT id, DWORD dwAddData)
 * \brief Create a virtual window.
 *
 * This function creates a virtual window for the purpose of
 * multi-thread messaging.
 *
 * \param hHosting The hosting virutal window.
 * \param WndProc The window callback procedure.
 * \param spCaption The caption of the virtual window.
 * \param id The long integer (pointer size) identifier of the virtual window.
 * \param dwAddData The additional data for the window.
 *
 * \return The handle to the new virtual window;
 *      HWND_INVALID indicates an error.
 *
 * \sa VirtualWindowCleanup
 *
 * Example:
 *
 * \include createvirtualwindow.c
 *
 * Since 5.0.0.
 */
MG_EXPORT HWND GUIAPI CreateVirtualWindow (HWND hHosting, WNDPROC WndProc,
        const char* spCaption, LINT id, DWORD dwAddData);

/**
 * \fn BOOL GUIAPI DestroyVirtualWindow (HWND hWnd)
 * \brief Destroy a virtual window.
 *
 * This function destroys a virtual window.
 *
 * \param hWnd The handle to the virtual window.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa VirtualWindowCleanup
 *
 * Example:
 *
 * \include createvirtualwindow.c
 *
 * Since 5.0.0.
 */
MG_EXPORT BOOL GUIAPI DestroyVirtualWindow (HWND hWnd);

    /** @} end of virtual_window_fns */

#endif /* defined _MGHAVE_VIRTUAL_WINDOW */

    /**
     * \defgroup window_create_fns Window creating/destroying
     * @{
     */

extern MG_EXPORT HWND __mg_hwnd_desktop;

/**
 * \def HWND_DESKTOP
 * \brief Desktop window handle.
 */
#define HWND_DESKTOP        __mg_hwnd_desktop

/**
 * \def HWND_NULL
 * \brief Null window handle.
 */
#define HWND_NULL           ((HWND)0)

/**
 * \def HWND_INVALID
 * \brief Invalid window handle.
 */
#define HWND_INVALID        ((HWND)-1)

#define HWND_OTHERPROC      ((HWND)-1)

/**
 * Structure defines a main window.
 */
typedef struct _MAINWINCREATE {
    /** The style of the main window */
    DWORD dwStyle;

    /** The extended style of the main window */
    DWORD dwExStyle;

    /** The caption of the main window */
    const char* spCaption;

    /** The handle to the menu of the main window */
    HMENU hMenu;

    /** The handle to the cursor of the main window */
    HCURSOR hCursor;

    /** The handle to the icon of the main window */
    HICON hIcon;

    /** The hosting main window */
    HWND  hHosting;

    /** The window callback procedure */
    LRESULT (*MainWindowProc)(HWND, UINT, WPARAM, LPARAM);

    /** The position of the main window in the screen coordinates */
    int lx, ty, rx, by;

    /**
     * The background pixel value of the main window.
     */
    gal_pixel iBkColor;

    /** The first private data associated with the main window */
    DWORD dwAddData;

    /** Reserved, do not use */
    DWORD dwReserved;
} MAINWINCREATE;
typedef MAINWINCREATE* PMAINWINCREATE;

#ifdef _MGRM_THREADS
/**
 * \fn int GUIAPI CreateThreadForMainWindow (pthread_t* thread,
 *      pthread_attr_t* attr, void * (*start_routine)(void *), void* arg)
 * \brief Create a thread for main window.
 *
 * The function creates a thread for main window by calling the POSIX
 * thread function pthread_create().
 *
 * \param thread The buffer to return the thread identifier if
 *      successfully created.
 * \param attr  The pointer to the thread attribute.
 * \param start_routine The function which is the entry of the thread.
 * \param arg   The argument will be passed to \a start_routine.
 *
 * \return The return value of pthread_create (0 on success).
 *
 * \sa CreateThreadForMessaging, pthread_create
 */
static inline int GUIAPI CreateThreadForMainWindow (pthread_t* thread,
        pthread_attr_t* attr, void * (*start_routine)(void *), void* arg)
{
    return CreateThreadForMessaging (thread, attr, start_routine, arg,
            TRUE, 16);
}

/**
 * \fn pthread_t GUIAPI GetMainWinThread (HWND hMainWnd)
 * \brief Get the thread identifier which main window belongs to.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return The thread identifier.
 *
 * \note Deprecated; use \a GetThreadByWindow() instead.
 */
static inline pthread_t GUIAPI GetMainWinThread (HWND hMainWnd)
{
#ifdef WIN32
    pthread_t ret;
    memset (&ret, 0, sizeof (pthread_t));
#else
    pthread_t ret = 0;
#endif

    GetThreadByWindow (hMainWnd, &ret);
    return ret;
}

/**
 * \fn int GUIAPI WaitMainWindowClose (HWND hWnd, void** retval)
 * \brief Suspend execution of the calling thread which main window belongs
 * to until the target thread terminates, unless the target thread has already
 * terminated.
 *
 * This function waits for the terminate of the thread which the main window
 * \a hWnd belongs to by calling the system POSIX thread function
 * \a pthread_join().
 *
 * \param hWnd The handle to the main window.
 * \param retval The buffer used to return the exit code of the target thread.
 *
 * \return The return value of pthread_join (0 on success).
 *
 * \note Deprecated; use GetThreadByWindow() and pthread_join() instead.
 *
 * \sa GetThreadByWindow
 */
MG_EXPORT int GUIAPI WaitMainWindowClose (HWND hWnd, void** returnval);
#endif /* defined _MGRM_THREADS */

/**
 * \fn BOOL GUIAPI MainWindowCleanup (HWND hMainWnd)
 * \brief Cleans up system resource associated with a main window.
 *
 * This function cleans up the system resource such as the message queue
 * associated with the main window \a hMainWnd. \a DestroyMainWindow does not
 * destroy all resource used by a main window, therefore, you should call
 * this function after calling \a DestroyMainWindow and skipping out from
 * the message loop. After calling this function, the main window object
 * will be destroyed actually.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa DestroyMainWindow
 *
 * \note Since 5.0.0, this function returns a BOOL value. If you try
 *      to clean up a main window in a thread other than it belongs to,
 *      the function will fail.
 */
MG_EXPORT BOOL GUIAPI MainWindowCleanup (HWND hMainWnd);

/**
 * \fn BOOL MainWindowThreadCleanup (HWND hMainWnd)
 * \brief Cleanup the main window.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Deprecated; please use \a MainWindowCleanup() instead.
 *
 * \sa MainWindowCleanup
 */
static inline BOOL MainWindowThreadCleanup (HWND hMainWnd)
{
    return MainWindowCleanup (hMainWnd);
}

/* The flags for the surface pixel format */
#define ST_PIXEL_MASK           0x00FF
#define ST_PIXEL_DEFAULT        0x0000
#define ST_PIXEL_ARGB4444       0x0001
#define ST_PIXEL_ARGB1555       0x0002
#define ST_PIXEL_ARGB8888       0x0003
#define ST_PIXEL_XRGB565        0x0004

/* other flags for future use */

/* for default surface flags */
#define ST_DEFAULT              (ST_PIXEL_DEFAULT)

#define CT_SYSTEM_MASK          0x0000FF
#define CT_OPAQUE               0x000000
#define CT_COLORKEY             0x000001
#define CT_ALPHACHANNEL         0x000002
#define CT_LOGICALPIXEL         0x000003
#define CT_ALPHAPIXEL           0x000004
#define CT_BLURRED              0x000005
#define CT_MAX_VALUE            0xFFFFFF

/**
 * \fn HWND GUIAPI CreateMainWindowEx2 (PMAINWINCREATE create_info, LINT id,
 *      const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
 *      unsigned int surf_flag, DWORD bkgnd_color,
 *      int compos_type, DWORD ct_arg)
 * \brief Creates a main window with specified compositing type and identifier.
 *
 * This function creates a main window by using information and the specified
 * compositing type and identifier, then returns the handle to the main window.
 *
 * \param create_info The pointer to a MAINWINCREATE structure.
 * \param werdr_name The name of window element renderer. NULL for default
 *      renderer.
 * \param we_attrs The pointer to window element attribute table. NULL for
 *      default window attribute table.
 * \param id The window identifier.
 * \param surf_flag The flag for the surface of the main window
 *      under compositing schema. The value of this argument can be one
 *      of the following values:
 *          - ST_PIXEL_DEFAULT\n
 *            Creating a surface which compliant to HDC_SCREEN.
 *          - ST_PIXEL_ARGB4444\n
 *            Creating a surface for this main window with
 *            the pixel format ARGB4444.
 *          - ST_PIXEL_ARGB1555\n
 *            Creating a surface for this main window with
 *            the pixel format ARGB1555.
 *          - ST_PIXEL_ARGB8888\n
 *            Creating a surface for this main window with
 *            the pixel format ARGB8888.
 * \param bkgnd_color The background color of the main window if you specify
 *  the main window's surface type other than the default. In this case,
 *  you must use this argument to specify the background color of the main
 *  window instead of the pixel value of the field (\a iBkColor) in
 *  \a MAINWINCREATE structure.
 *  The value of this field is a 32-bit RGBA quadruple essentially.
 *  You can use a value returned by \a MakeRGBA macro for this argument.
 *  Note that if you use the surface type \a ST_PIXEL_DEFAULT when creating
 *  the main window, you can still use the pixel values specified in
 *  \a MAINWINCREATE structure which is compliant to \a HDC_SCREEN, e.g.,
 *  a value in \a SysPixelIndex array.
 * \param compos_type The compositing type of the main window.
 * \param ct_arg The compositing argument of the main window.
 *
 * \return The handle to the new main window; HWND_INVALID indicates an error.
 *
 * \note Note When you specify a surface type other than ST_PIXEL_DEFAULT, you
 *  must use \a bkgnd_color to specify the background color of
 *  the main window.
 *
 * \sa CreateMainWindowEx, CreateMainWindow, MAINWINCREATE, styles
 *
 * Example:
 *
 * \include createmainwindow.c
 */
MG_EXPORT HWND GUIAPI CreateMainWindowEx2 (PMAINWINCREATE create_info, LINT id,
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
        unsigned int surf_flag, DWORD bkgnd_color,
        int compos_type, DWORD ct_arg);

/**
 * \fn HWND GUIAPI CreateMainWindowEx (PMAINWINCREATE pCreateInfo, \
 *                const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs, \
 *                const char* window_name, const char* layer_name);
 * \brief Creates a main window.
 *
 * This function creates a main window by using some information, and returns
 * the handle to the main window.
 *
 * \param pCreateInfo The pointer to a MAINWINCREATE structure.
 * \param werdr_name The name of window element renderer. NULL for default
 *      renderer.
 * \param we_attrs The pointer to window element attribute table. NULL for
 *      default window attribute table.
 * \param window_name The window name, ignored currently.
 * \param layer_name The layer name, ignored currently.
 *
 * \return The handle to the new main window; HWND_INVALID indicates an error.
 *
 * \sa CreateMainWindow, MAINWINCREATE, styles
 *
 * \note Since 5.0.0, this function is implemented as an inline function calling
 *  \a CreateMainWindowEx2. When calling CreateMainWindowEx2:
 *      - We pass 0 for the identifier.
 *      - We pass 0xFFFFFFFFUL for the background color.
 *      - We discard the values passed to window_name and layer_name.
 *
 * Example:
 *
 * \include createmainwindow.c
 *
 * \sa CreateMainWindowEx2
 */
static inline HWND GUIAPI CreateMainWindowEx (PMAINWINCREATE pCreateInfo,
                const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
                const char* window_name, const char* layer_name)
{
    return CreateMainWindowEx2 (pCreateInfo, 0L, werdr_name, we_attrs,
            ST_DEFAULT, 0xFFFFFFFFUL, CT_OPAQUE, 0);
}

/**
 * \fn HWND GUIAPI CreateMainWindow (PMAINWINCREATE pCreateInfo)
 * \brief A simplified version of CreateMainWindowEx.
 *
 * This function creates a main window by calling CreateMainWindow function
 * and passing NULL for werdr_name, we_attrs, window_name, and layer_name
 * parameters.
 *
 * \sa CreateMainWindowEx
 */
static inline HWND GUIAPI CreateMainWindow (PMAINWINCREATE pCreateInfo)
{
    return CreateMainWindowEx (pCreateInfo, NULL, NULL, NULL, NULL);
}

/**
 * \fn BOOL GUIAPI DestroyMainWindow (HWND hWnd)
 * \brief Destroys a main window.
 *
 * This function destroys the main window specified by \a hWnd.
 * It does not release all system resource used by the main window.
 * You should call \a MainWindowThreadCleanup to destroy the main window
 * actually.
 *
 * \param hWnd The handle to the main window.
 * \return TRUE on success, FALSE on error.
 *
 * \sa MainWindowThreadCleanup
 *
 * Example:
 *
 * \include destroymainwindow.c
 */
MG_EXPORT BOOL GUIAPI DestroyMainWindow (HWND hWnd);

typedef void (*CB_FREE_LOCAL_DATA) (DWORD local_data);

/**
 * \fn BOOL GUIAPI SetWindowLocalData (HWND hwnd, const char* data_name,
        DWORD local_data, CB_FREE_LOCAL_DATA cb_free)
 * \brief Set the local data bound with a name for a window.
 *
 * This function sets the local data as \a local_data which is bound with the
 * name \a data_name for the specified window \a hwnd. If you passed a non-NULL
 * function pointer for \a cb_free, the system will call this function to free
 * the local data when you destroy the window, remove the local data, or when
 * you call this function to overwrite the old local data for the name.
 *
 * \param hwnd The handle to the window, which can be a main window, a virtual
 *  window or a control.
 * \param data_name The name of the local data.
 * \param local_data The value of the local data.
 * \param cb_free A callback function which will be called automatically by the
 *  system to free the local data when the window is being destroyed or the local
 *  data is being removed or overwritten. If it is NULL, the system does nothing
 *  to the local data.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetWindowLocalData, RemoveWindowLocalData 
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI SetWindowLocalData (HWND hwnd, const char* data_name,
        DWORD local_data, CB_FREE_LOCAL_DATA cb_free);

/**
 * \fn BOOL GUIAPI RemoveWindowLocalData (HWND hwnd, const char* data_name)
 * \brief Remove the local data bound with a name for a window.
 *
 * This function removes the local data which is bound with the
 * name \a data_name for the specified window \a hwnd. When you pass NULL
 * for \a data_name, this function will remove all local data of the window.
 * Note that this function will call the callback procedure for releasing
 * the local data, if you had set it, when removing the local data.
 *
 * \param hwnd The handle to the window, which can be a main window, a virtual
 *  window or a control.
 * \param data_name The name for the local data.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetWindowLocalData, GetWindowLocalData 
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI RemoveWindowLocalData (HWND hwnd, const char* data_name);

/**
 * \fn BOOL GUIAPI GetWindowLocalData (HWND hwnd, const char* data_name,
        DWORD *local_data, CB_FREE_LOCAL_DATA* cb_free)
 * \brief Retrieve the local data bound with a name for a window.
 *
 * This function retrieves the local data which is bound with the
 * name \a data_name for the specified window \a hwnd. 
 *
 * \param hwnd The handle to the window, which can be a main window, a virtual
 *  window or a control.
 * \param data_name The name for the local data.
 * \param local_data The pointer to a DWORD variable to return the local data
 *  if it is not NULL.
 * \param cb_free The pointer to a CB_FREE_LOCAL_DATA variable to return
 *  the pointer to the callback function which is used to free the local data
 *  if it is not NULL.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetWindowLocalData, RemoveWindowLocalData 
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI GetWindowLocalData (HWND hwnd, const char* data_name,
        DWORD *local_data, CB_FREE_LOCAL_DATA* cb_free);

/**
 * \fn BOOL GUIAPI SetWindowMask (HWND hWnd, const MYBITMAP* mask)
 *
 * \brief Set window (a main window, or a child window which is
 * also known as "control")'s Mask Rect with MYBITMAP data.
 *
 * \param hWnd The handle to the window.
 * \param mask The mask of the window which indicate the transparency of
 * each pixel on this window.
 *
 * \return return TRUE on success, otherwise FALSE.
 *
 * \sa SetWindowRegion
 */
MG_EXPORT BOOL GUIAPI SetWindowMask (HWND hWnd, const MYBITMAP* mask);

/**
 * \fn BOOL GUIAPI SetWindowMaskEx (HWND hWnd, HDC hdc, const BITMAP* mask)
 *
 * \brief Set window (a main window, or a child window which is
 * also known as "control")'s Mask Rect with BITMAP data,
 *
 * \param hWnd The handle to the window.
 * \param hdc The reference dc which indicate the colorformat of the \a mask.
 * \param mask The mask of the window which indicate the transparency of
 * each pixel on this window.
 *
 * \return return TRUE on success, otherwise FALSE.
 *
 * \sa SetWindowRegion
 */
MG_EXPORT BOOL GUIAPI SetWindowMaskEx (HWND hWnd, HDC hdc, const BITMAP* mask);

/**
 * \fn BOOL GUIAPI SetMainWindowAlwaysTop (HWND hWnd, BOOL fSet)
 * \brief Set or cancel a main window being always top.
 *
 * This function set or cancel a main window being always top on others.
 * If it succeed, the main window will have the style \a WS_ALWAYSTOP.
 *
 * \param hWnd The handle to the window.
 * \param fSet Set or cancel the always top style; TRUE to set, FALSE to cancel.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetWindowStyle, WS_ALWAYSTOP
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI SetMainWindowAlwaysTop (HWND hWnd, BOOL fSet);

#define GF_SWIPE_HORZ   ZOF_GF_SWIPE_HORZ
#define GF_SWIPE_VERT   ZOF_GF_SWIPE_VERT

/**
 * \fn BOOL GUIAPI SetMainWindowGestureFlags (HWND hWnd, DWORD dwFlags)
 * \brief Set the gesture flags of a main window.
 *
 * This function sets the gesture flags of a specific main window.
 *
 * Generally, a main window which needs a specific gesture should handle
 * \a MSG_GESTURETEST and call this function to set the correct gesture flags for
 * the main window.
 *
 * The MiniGUI Core will send the MSG_GESTURETEST message as a notification
 * to a main window when it tries to handle a gesture globally. If the main window
 * sets the flag of the gesture in a specific time, e.g. 100ms, MiniGUI Core will
 * pass the gesture events to the main window other than handle the gesture by itself.
 *
 * \param hWnd The handle to the main window.
 * \param dwFlags The new gesture flags, can be OR'd with the following values:
 *  - GF_SWIPE_HORZ\n
 *      The main window handles horizontal swipe gesture.
 *  - GF_SWIPE_VERT\n
 *      The main window handles vertical swipe gesture.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa MSG_GESTURETEST
 *
 * Since 5.0.6
 */
MG_EXPORT BOOL GUIAPI SetMainWindowGestureFlags (HWND hWnd, DWORD dwFlags);

#ifdef _MGSCHEMA_COMPOSITING

/**
 * \fn BOOL GUIAPI SetMainWindowCompositing (HWND hWnd,
        int type, DWORD arg)
 * \brief Set the compositing type of a main window.
 *
 * This function set the compositing type of a main window.
 *
 * \param hWnd The handle to the window.
 * \param type The compositing type, can be one of the following values:
 *  - CT_OPAQUE\n
 *      The main window is opaque. This is the default compositing type of
 *      a main window. The control as main window always use this
 *      compositing type.
 *  - CT_COLORKEY\n
 *      Use the specified color as the transparency key. You should also specify
 *      the color along with the parameter \a arg in a RGBA quadruple value.
 *  - CT_ALPHACHANNEL\n
 *      Use the specified alpha channel value. You should also specify
 *      the alpha channel value (0~255) along with the parameter \a arg.
 *  - CT_LOGICALPIXEL\n
 *      Do the given color logical operation. You should specify the color
 *      logical raster operation along with the pararmeter \a arg. See \a ColorLogicalOp.
 *  - CT_ALPHAPIXEL\n
 *      The alpha component value of the source and/or the destination pixel go
 *      into effect. You can specify the color blend method along
 *      with the pararmeter \a arg. See \a ColorBlendMethod. Note that a specific
 *      compositor may not support this compositing type. The built-in `fallback`
 *      compositor uses the blend mode argument when using Pixman, otherwise,
 *      uses the Porter Duff blend mode: source over destination (COLOR_BLEND_PD_SRC_OVER).
 *  - CT_BLURRED\n
 *      Apply a Gaussian blur to the background of the main window. You should
 *      also specify the radius of the blur (0 ~ 255) in pixles along with the
 *      paramter \a arg. It defines the value of the standard deviation to the
 *      Gaussian function, i.e., how many pixels on the screen blend into each
 *      other; thus, a larger value will create more blur. A value of 0 leaves
 *      the input unchanged. Note that the built-in `fallback` compositor
 *      does not support this compositing type.
 * \param arg The compositing argument.
 *
 * \return return TRUE on success, otherwise FALSE.
 *
 * \note This function only available when _MGSCHEMA_COMPOSITING is defined.
 *
 * \note A customized compositor can also define other compositing type.
 *      Please make sure that the customized compositing type should be
 *      less than \a CT_MAX_VALUE.
 *
 * \sa CreateMainWindowEx
 *
 * Since 5.0.0
 */
MG_EXPORT BOOL GUIAPI SetMainWindowCompositing (HWND hWnd,
        int type, DWORD arg);

#endif /* defined _MGSCHEMA_COMPOSITING */

/**
 * \fn BOOL GUIAPI SetWindowRegion (HWND hWnd, const CLIPRGN* region)
 * \brief Set window's Mask Rect with CLIPRGN data.
 *
 * \param hWnd The handle to the window.
 * \param region The region to indicate which part of the window is visible.
 *
 * \return return TRUE on success, otherwise FALSE.
 */
BOOL GUIAPI SetWindowRegion (HWND hWnd, const CLIPRGN* region);

/**
 * \fn BOOL GUIAPI GetWindowRegion (HWND hWnd, CLIPRGN* region)
 * \brief The function obtains a copy of the window region of a window.
 *
 * This function obtains a copy of the window region of a window.
 * The window region of a window is set by calling the SetWindowRegion
 * or SetWindowMaskEx function. The window region determines the area
 * within the window where the system permits drawing. The system
 * does not display any portion of a window that lies outside of
 * the window region.
 *
 * \param hWnd Handle to the window whose window region is to be obtained.
 * \param region Pointer to the region which will be modified to
 *        represent the window region.
 *
 * \return return TRUE on success, otherwise FALSE.
 *
 * \sa SetWindowRegion, SetWindowMaskEx
 */
BOOL GUIAPI GetWindowRegion (HWND hWnd, CLIPRGN* region);

LRESULT GUIAPI PreDefMainWinProc (HWND hWnd, UINT message,
                WPARAM wParam, LPARAM lParam);

LRESULT GUIAPI PreDefDialogProc (HWND hWnd,
                UINT message, WPARAM wParam, LPARAM lParam);

LRESULT GUIAPI PreDefControlProc (HWND hWnd, UINT message,
                WPARAM wParam, LPARAM lParam);

#ifdef _MGHAVE_VIRTUAL_WINDOW
LRESULT GUIAPI PreDefVirtualWinProc (HWND hWnd, UINT message,
                WPARAM wParam, LPARAM lParam);
#endif

/**
 * \fn LRESULT DefaultWindowProc (HWND hWnd, UINT message, \
                WPARAM wParam, LPARAM lParam)
 * \brief The default window callback procedure.
 *
 * This window procedure can be used for main windows, dialog boxes,
 * and child windows.
 *
 * This function is the default window callback procedure.
 * You should call this function for all messages you do not want to handle
 * in your window procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 */
MG_EXPORT LRESULT GUIAPI DefaultWindowProc (HWND hWnd, UINT message,
                WPARAM wParam, LPARAM lParam);

/**
 * \var WNDPROC __mg_def_proc[]
 * \brief The default window callback procedure array.
 *
*/
#ifdef _MGHAVE_VIRTUAL_WINDOW
extern MG_EXPORT WNDPROC __mg_def_proc[4];
#else
extern MG_EXPORT WNDPROC __mg_def_proc[3];
#endif

/**
 * \def DefaultMainWinProc
 * \brief Is the default main window callback procedure.
 *
 * This function is the default main window callback procedure.
 * You should call this function for all messages, you do not want to handle
 * in your main window procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 *
 */
#define DefaultMainWinProc (__mg_def_proc[0])

/**
 * \def DefaultDialogProc
 * \brief The default dialog box procedure.
 *
 * This function is the default dialog box procedure.
 * You should call this function in your dialog box procedure
 * to process the unhandled messages.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first message parameter.
 * \param lParam The second message parameter.
 *
 * \return The return value of the message handler.
 *
 */
#define DefaultDialogProc  (__mg_def_proc[1])

/**
 * \def DefaultControlProc
 * \brief The default control callback procedure.
 *
 * This function is the default control callback procedure.
 * You should call this function for all messages, you do not want to handle
 * in your own control procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 */
#define DefaultControlProc (__mg_def_proc[2])

#ifdef _MGHAVE_VIRTUAL_WINDOW
/**
 * \def DefaultVirtualWinProc
 * \brief The default window callback procedure for virtual windows.
 *
 * This function is the default window callback procedure for virtual windows.
 * You should call this function for any message which you do not want to handle
 * in your own callback procedure.
 *
 * \param hWnd The handle to the window.
 * \param message The message identifier.
 * \param wParam The first parameter of the message.
 * \param lParam The second parameter of the message.
 */
#define DefaultVirtualWinProc (__mg_def_proc[3])
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

#ifdef _DEBUG
MG_EXPORT void GUIAPI DumpWindow (FILE* fp, HWND hWnd);
#endif /* _DEBUG */

    /** @} end of window_create_fns */

    /**
     * \defgroup window_general_fns General window operations
     * @{
     */

#define SW_HIDE              0x0000
#define SW_SHOW              0x0010
#define SW_SHOWNORMAL        0x0100

/**
 * \fn void GUIAPI UpdateWindow (HWND hWnd, BOOL bErase)
 * \brief Updates a window.
 *
 * This function updates the window specified by \a hWnd.
 * It will redraw the caption, the frame, and the menu bar of the window.
 * It will invalidate the client area of the window as well, and
 * if \b bErase is TRUE, the client area will be erased by using
 * background color.
 *
 * \param hWnd The handle to the window.
 * \param bErase Indicates whether to erase the client area of the window.
 *
 * \sa InvalidateRect
 */
MG_EXPORT void GUIAPI UpdateWindow (HWND hWnd, BOOL bErase);

/**
 * \fn void GUIAPI UpdateInvalidClient (HWND hWnd, BOOL bErase)
 * \brief Updates the invalid client areas in a window.
 *
 * This function updates the client areas specified by \a hWnd.
 * It will redraw the invalid client area of the window, and
 * if \b bErase is TRUE, the client area will be erased by using
 * background color.
 *
 * \param hWnd The handle to the window.
 * \param bErase Indicates whether to erase the client area of the window.
 *
 * \sa UpdateWindow
 */
MG_EXPORT void GUIAPI UpdateInvalidClient (HWND hWnd, BOOL bErase);

/**
 * \fn BOOL GUIAPI ShowWindow (HWND hWnd, int iCmdShow)
 * \brief Shows or hides a window.
 *
 * This function shows or hides the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param iCmdShow The command to show or hide, can be one of
 * the following values:
 *      - SW_SHOW\n
 *        Shows the window.
 *      - SW_HIDE\n
 *        Hides the window.
 *      - SW_SHOWNORMAL\n
 *        Shows the window, and if the window is a main window
 *        sets it to be the top most main window.
 * \return TRUE on sucess, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI ShowWindow (HWND hWnd, int iCmdShow);

/**
 * \fn BOOL GUIAPI EnableWindow (HWND hWnd, BOOL fEnable)
 * \brief Enables of disables a window.
 *
 * This function enables or disables mouse and keyboard input
 * to the specified window \a hWnd. When input is disabled, the window
 * does not receive input such as mouse clicks and key presses.
 * When input is enabled, the window receives all input.
 *
 * \param hWnd The handle to the window.
 * \param fEnable Indicates whether to enable or disable the window,
 *        TRUE for enable.
 * \return The old enable state of the window.
 *
 * \sa IsWindowEnabled
 */
MG_EXPORT BOOL GUIAPI EnableWindow (HWND hWnd, BOOL fEnable);

/**
 * \fn BOOL GUIAPI IsWindowEnabled (HWND hWnd)
 * \brief Determine whether the specified window is enabled for mouse
 *        and keyboard input.
 *
 * This function returns the enable/disable state of the window specified by
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The enable/disable state of the window, TRUE for enabled.
 *
 * \sa EnableWindow
 */
MG_EXPORT BOOL GUIAPI IsWindowEnabled (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetClientRect(HWND hWnd, PRECT prc)
 * \brief Retrieve the client rectangle of a window.
 *
 * This function retrieves the coordinates of the client area of
 * the window specified by \a hWnd. The client coordinates specify
 * the upper-left and lower-right corners of the client area.
 * Because client coordinates are relative to the upper-left corner of
 * a window's client area, the coordinates of the upper-left corner
 * are always (0,0).
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure receives the client rectangle.
 *
 * \return TRUE on sucess, otherwise FALSE.
 *
 * \note Note that the coordinates of the upper-left corner are always zero.
 *
 * \sa MoveWindow
 */
MG_EXPORT BOOL GUIAPI GetClientRect(HWND hWnd, PRECT prc);

/**
 * \fn gal_pixel GUIAPI DWORD2PixelByWindow (HWND hWnd, DWORD dwColor)
 * \brief Convert a DWORD color to gal_pixel for a window.
 *
 * This function converts a color in DWORD to the pixel value according to
 * the surface of the main window.
 *
 * \param hWnd The handle to the window.
 * \param dwColor The color value in DWORD.
 *
 * \return The converted pixel value.
 */
MG_EXPORT gal_pixel GUIAPI DWORD2PixelByWindow (HWND hWnd, DWORD dwColor);

/**
 * \fn gal_pixel GUIAPI GetWindowBkColor (HWND hWnd)
 * \brief Returns the current background color of a window.
 *
 * This function returns the pixel value of the current background color of
 * the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pixel value of the background color.
 *
 * \note The type of return value changed from int to gal_pixel since v3.2.
 *
 * \sa SetWindowBkColor
 */
MG_EXPORT gal_pixel GUIAPI GetWindowBkColor (HWND hWnd);

/**
 * \fn gal_pixel GUIAPI SetWindowBkColor (HWND hWnd, gal_pixel new_bkcolor)
 * \brief Set the background color of a window.
 *
 * This function sets the background color of the specified window \a hWnd
 * to be new pixel value \a new_backcolor. You should call \a UpdateWindow
 * or \a InvalidateRect in order that the new background color runs into
 * affect.
 *
 * \param hWnd The handle to the window.
 * \param new_bkcolor The pixel value of the new background color.
 * \return The pixel value of the old background color.
 *
 * \note The type of return value and \a new_bkcolor changed from int to
 *       gal_pixel since v3.2.
 *
 * \sa GetWindowBkColor
 */
MG_EXPORT gal_pixel GUIAPI SetWindowBkColor (HWND hWnd, gal_pixel new_bkcolor);

/**
 * \fn PLOGFONT GUIAPI GetWindowFont (HWND hWnd)
 * \brief Retrieve the default font of a window.
 *
 * This function retrieves the default font of the specified
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The default logical font of the window.
 *
 * \sa SetWindowFont
 */
MG_EXPORT PLOGFONT GUIAPI GetWindowFont (HWND hWnd);

/**
 * \fn PLOGFONT GUIAPI SetWindowFont (HWND hWnd, PLOGFONT pLogFont)
 * \brief Set the default font of a window.
 *
 * This function sets the default font of the specified window \a hWnd
 * to be the logical font \a pLogFont. This function will send an
 * MSG_FONTCHANGING message to the window. If the handler of the message
 * returns non-zero value, this function will return immediately with the
 * unchanged default font. Or, after the new default font set, this function
 * will send an MSG_FONTCHANGED message to the window as a notification.
 *
 * \param hWnd The handle to the window.
 * \param pLogFont The new default logical font. If it is NULL, this function
 * will set the default font to be the system wchar font.
 *
 * \return The old default logical font of the window, NULL on error.
 *
 * \sa GetWindowFont, GetSystemFont
 */
MG_EXPORT PLOGFONT GUIAPI SetWindowFont (HWND hWnd, PLOGFONT pLogFont);

/**
 * \fn HCURSOR GUIAPI GetWindowCursor (HWND hWnd)
 * \brief Retrieve the current cursor of a window.
 *
 * This function retrieves the current cursor of the specified
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the cursor.
 *
 * \sa SetWindowCursor
 */
MG_EXPORT HCURSOR GUIAPI GetWindowCursor (HWND hWnd);

/**
 * \fn HCURSOR GUIAPI SetWindowCursor (HWND hWnd, HCURSOR hNewCursor)
 * \brief Set the current cursor of a window.
 *
 * This function sets the current cursor of the specified window \a hWnd with
 * argument \a hNewCursor.
 *
 * \param hWnd The handle to the window.
 * \param hNewCursor The handle to the new cursor.
 * \return The handle to the old cursor.
 *
 * \sa GetWindowCursor
 */
MG_EXPORT HCURSOR GUIAPI SetWindowCursor (HWND hWnd, HCURSOR hNewCursor);

/**
 * \fn HICON GUIAPI GetWindowIcon (HWND hWnd)
 * \brief Retrieve the current icon of a window.
 *
 * This function retrieves the current icon of the specified
 * window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the icon.
 *
 * \sa SetWindowIcon
 */
MG_EXPORT HICON GUIAPI GetWindowIcon (HWND hWnd);

/**
 * \fn HICON GUIAPI SetWindowIcon (HWND hWnd, HICON hIcon, BOOL bRedraw)
 * \brief Set the current icon of a window.
 *
 * This function sets the current icon of the specified window \a hWnd with
 * argument \a hIcon.
 *
 * \param hWnd The handle to the window.
 * \param hIcon The handle to the new icon.
 * \param bRedraw Indicates whether to update the whole window.
 *
 * \return The handle to the old icon.
 *
 * \note Note that MiniGUI only provides icon support for main window so far.
 *
 * \sa GetWindowIcon
 */
MG_EXPORT HICON GUIAPI SetWindowIcon (HWND hWnd, HICON hIcon, BOOL bRedraw);

/**
 * \fn DWORD GUIAPI GetWindowStyle (HWND hWnd)
 * \brief Retrieve the style of a window.
 *
 * This function retrieves the style of the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The style of the window.
 *
 * \sa GetWindowExStyle
 */
MG_EXPORT DWORD GUIAPI GetWindowStyle (HWND hWnd);

/**
 * \fn DWORD GUIAPI GetWindowExStyle (HWND hWnd)
 * \brief Retrieve the extended style of a window.
 *
 * This function retrieves the extended style of the window specified by \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The extended style of the window.
 *
 * \sa GetWindowStyle
 */
MG_EXPORT DWORD GUIAPI GetWindowExStyle (HWND hWnd);

/**
 * \fn BOOL GUIAPI ExcludeWindowStyle (HWND hWnd, DWORD dwStyle)
 * \brief Removes the specific style of a window.
 *
 * This function removes the specific style of the window
 * specified by \a hWnd.
 *
 * Note that you should be very careful with changing the styles of a window
 * on the fly by calling this function. You are strongly recommended to only
 * change the customizable window styles (bits in WS_CTRLMASK) by calling
 * this function.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific style which will be removed.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ExcludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI ExcludeWindowStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI IncludeWindowStyle (HWND hWnd, DWORD dwStyle)
 * \brief Includes the specific style of a window.
 *
 * This function includes the specific style of the window
 * specified by \a hWnd.
 *
 * Note that you should be very careful with changing the styles of a window
 * on the fly by calling this function. You are strongly recommended to only
 * change the customizable window styles (bits in WS_CTRLMASK) by calling
 * this function.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific style which will be included.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IncludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI IncludeWindowStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI ExcludeWindowExStyle (HWND hWnd, DWORD dwStyle)
 * \brief Removes the specific extended style of a window.
 *
 * This function removes the specific extended style of the window
 * specified by \a hWnd.
 *
 * Note that you should be very careful with changing the extended styles of
 * a window on the fly by calling this function. You are strongly recommended
 * to only change the customizable window styles (bits in
 * WS_EX_CONTROL_MASK | WS_EX_CONTROL_MASK) by calling this function.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific extended style which will be removed.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ExcludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI ExcludeWindowExStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn BOOL GUIAPI IncludeWindowExStyle (HWND hWnd, DWORD dwStyle)
 * \brief Includes the specific extended style of a window.
 *
 * This function includes the specific extended style of the window
 * specified by \a hWnd.
 *
 * Note that you should be very careful with changing the extended styles of
 * a window on the fly by calling this function. You are strongly recommended
 * to only change the customizable window styles (bits in
 * WS_EX_CONTROL_MASK | WS_EX_CONTROL_MASK) by calling this function.
 *
 * \param hWnd The handle to the window.
 * \param dwStyle The specific extended style which will be included.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IncludeWindowStyle
 */
MG_EXPORT BOOL GUIAPI IncludeWindowExStyle (HWND hWnd, DWORD dwStyle);

/**
 * \fn WNDPROC GUIAPI GetWindowCallbackProc (HWND hWnd)
 * \brief Retrieve the callback procedure of a window.
 *
 * This function retrieves the window callback procedure of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The callback procedure of the window.
 *
 * \sa SetWindowCallbackProc
 */
MG_EXPORT WNDPROC GUIAPI GetWindowCallbackProc (HWND hWnd);

/**
 * \fn WNDPROC GUIAPI SetWindowCallbackProc (HWND hWnd, WNDPROC newProc)
 * \brief Set the callback procedure of a window.
 *
 * This function sets the window callback procedure of the specified window
 * \a hWnd to be the procedure \a newProc.
 *
 * \param hWnd The handle to the window.
 * \param newProc The new callback procedure of the window.
 * \return The old callback procedure of the window.
 *
 * \sa GetWindowCallbackProc
 *
 * Example:
 *
 * \include subclass.c
 */
MG_EXPORT WNDPROC GUIAPI SetWindowCallbackProc (HWND hWnd, WNDPROC newProc);

/**
 * \fn DWORD GUIAPI GetWindowAdditionalData (HWND hWnd)
 * \brief Retrieve the first additional data of a window.
 *
 * This function retrieves the first additional data of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The first additional data of the window.
 *
 * \sa SetWindowAdditionalData
 */
MG_EXPORT DWORD GUIAPI GetWindowAdditionalData (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowAdditionalData (HWND hWnd, DWORD newData)
 * \brief Set the first additional data of a window.
 *
 * This function sets the first additional data of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param newData The new first additional data of the window.
 * \return The old first additional data of the window.
 *
 * \note For a dialog box created by \a DialogBoxIndirectParam,
 *       its second additional data used internally by the system to save
 *       the return value of the box. So you should avoid setting the
 *       second additional data of a dialog box.
 *
 * \sa GetWindowAdditionalData
 */
MG_EXPORT DWORD GUIAPI SetWindowAdditionalData (HWND hWnd, DWORD newData);

/**
 * \fn DWORD GUIAPI GetWindowAdditionalData2 (HWND hWnd)
 * \brief Retrieve the second additional data of a window.
 *
 * This function retrieves the second additional data of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The second additional data of the window.
 *
 * \sa SetWindowAdditionalData2
 */
MG_EXPORT DWORD GUIAPI GetWindowAdditionalData2 (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowAdditionalData2 (HWND hWnd, DWORD newData)
 * \brief Set the second additional data of a window.
 *
 * This function sets the second additional data of the specified window
 * \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param newData The new second additional data of the window.
 * \return The old second additional data of the window.
 *
 * \note For a control created by \a CreateWindowEx, its second additional
 *       data is reserved for internal use. So you should avoid setting
 *       the second additional data of a control.
 *
 * \sa GetWindowAdditionalData2
 */
MG_EXPORT DWORD GUIAPI SetWindowAdditionalData2 (HWND hWnd, DWORD newData);

/**
 * \fn DWORD GUIAPI GetWindowClassAdditionalData (HWND hWnd)
 * \brief Retrieve the additional data of a control class.
 *
 * This function retrieves the additional data of the control class to which
 * the specified control \a hWnd belongs.
 *
 * \param hWnd The handle to the control.
 *
 * \return The additional data of the control class to which the control
 *         belongs.
 *
 * \note Note that this function always returns zero for the main window.
 *
 * \sa SetWindowClassAdditionalData
 */
MG_EXPORT DWORD GUIAPI GetWindowClassAdditionalData (HWND hWnd);

/**
 * \fn DWORD GUIAPI SetWindowClassAdditionalData (HWND hWnd, DWORD newData)
 * \brief Set the additional data of a control class.
 *
 * This function sets the additional data of the control class to which
 * the specified control \a hWnd belongs.
 *
 * \param hWnd The handle to the control.
 * \param newData The new additional data of the control class.
 * \return The old additional data of the control class.
 *
 * \note This function always does nothing and returns zero for the main window.
 *
 * \sa SetWindowClassAdditionalData
 */
MG_EXPORT DWORD GUIAPI SetWindowClassAdditionalData (HWND hWnd, DWORD newData);

/**
 * \fn const char* GUIAPI GetWindowCaption (HWND hWnd)
 * \brief Retrieve the caption of a window.
 *
 * This function retrieves the caption of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pointer to the caption string of the window.
 *
 * \sa SetWindowCaption
 */
MG_EXPORT const char* GUIAPI GetWindowCaption (HWND hWnd);

/**
 * \fn BOOL GUIAPI SetWindowCaption (HWND hWnd, const char* spCaption)
 * \brief Set the caption of a window.
 *
 * This function sets the caption of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param spCaption The pointer to the new caption of the window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetWindowCaption, SetWindowText
 */
MG_EXPORT BOOL GUIAPI SetWindowCaption (HWND hWnd, const char* spCaption);

/**
 * \fn BOOL GUIAPI InvalidateRect (HWND hWnd, const RECT* prc, BOOL bEraseBkgnd)
 * \brief Makes a rectangle region in the client area of a window invalid.
 *
 * This function adds a rectangle pointed to by \a prc to the specified
 * window's update region. The update region represents the portion of
 * the window's client area that must be redrawn, and erase background
 * if argument \a bReaseBkgnd is set.
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure which defines the
 *        invalid rectangle.
 * \param bEraseBkgnd Indicates whether the background should be erased.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa MSG_PAINT
 */
MG_EXPORT BOOL GUIAPI InvalidateRect (HWND hWnd,
                const RECT* prc, BOOL bEraseBkgnd);

/**
 * \fn BOOL GUIAPI InvalidateRegion (HWND hWnd, const CLIPRGN* pRgn, BOOL bErase)
 * \brief Invalidates the client area within the specified region.
 *
 * This function invalidates the client area within the specified region
 * by adding it to the current update region of a window. The invalidated
 * region, along with all other areas in the update region, is marked for
 * painting when the next MSG_PAINT message occurs.
 *
 * \param hWnd Handle to the window with an update region that is to be
 *        modified.
 * \param pRgn Pointer to the region to be added to the update region.
 *        The region is assumed to have client coordinates. If this
 *        parameter is NULL, the entire client area is added to the
 *        update region.
 * \param bErase Specifies whether the background within the
 *        update region should be erased when the update region is processed.
 *        If this parameter is TRUE, the background is erased. If the
 *        parameter is FALSE, the background remains unchanged.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Invalidated areas accumulate in the update region until the
 *       next MSG_PAINT message is processed or until the region is
 *       validated by using the ValidateRect or ValidateRegion function.
 *       MiniGUI sends a MSG_PAINT message to a window whenever its
 *       update region is not empty and there are no other messages
 *       in the application queue for that window.
 *       The specified region must have been created by using one of
 *       the region functions.
 *       If the bErase parameter is TRUE for any part of the update region,
 *       the background in the entire region is erased, not just in
 *       the specified part.
 *
 * \sa MSG_PAINT, InvalidateRect, ValidateRegion
 */
MG_EXPORT BOOL GUIAPI InvalidateRegion (HWND hWnd, const CLIPRGN* pRgn, BOOL bErase);

/**
 * \fn BOOL GUIAPI ValidateRect (HWND hWnd, const RECT* rect)
 * \brief Validates the client area within a rectangle by removing
 *        the rectangle from the update region of the specified window.
 *
 * This function validates the client area within a rectangle by
 * removing the rectangle from the update region of the specified window.
 *
 * \param hWnd Handle to the window whose update region is to be modified.
 * \param rect Pointer to a RECT structure that contains the client
 *        coordinates of the rectangle to be removed from the update region.
 *        If this parameter is NULL, the entire client area is removed.
 *
 * \return If the function succeeds, the return value is nonzero.
 *         If the function fails, the return value is zero.
 *
 * \note The BeginPaint function automatically validates the entire
 *       client area. Neither the ValidateRect nor ValidateRegion function
 *       should be called if a portion of the update region must be
 *       validated before the next MSG_PAINT message is generated.
 *       The system continues to generate MSG_PAINT messages until
 *       the current update region is validated.
 *
 * \sa MSG_PAINT, BeginPaint, ValidateRegion, InvalidateRect
 */
MG_EXPORT BOOL GUIAPI ValidateRect (HWND hWnd, const RECT* rect);

/**
 * \fn BOOL GUIAPI ValidateRegion (HWND hWnd, const CLIPRGN* pRgn)
 * \brief Validates the client area within a region by removing the
 *        region from the current update region of the specified window.
 *
 * This function validates the client area within a region by removing
 * the region \a pRgn from the current update region of the specified
 * window \a hWnd.
 *
 * \param hWnd Handle to the window whose update region is to be modified.
 * \param pRgn A region that defines the area to be removed from the
 *        update region. If this parameter is NULL, the entire client area
 *        is removed.
 *
 * \return If the function succeeds, the return value is nonzero.
 *         If the function fails, the return value is zero.
 *
 * \note The specified region must have been created by a region function.
 *       The region coordinates are assumed to be client coordinates.
 *       The BeginPaint function automatically validates the entire client area.
 *       Neither the ValidateRect nor ValidateRegion function should be called
 *       if a portion of the update region must be validated before the
 *       next MSG_PAINT message is generated.
 *
 * \sa MSG_PAINT, BeginPaint, ValidateRect, InvalidateRegion
 */
MG_EXPORT BOOL GUIAPI ValidateRegion (HWND hWnd, const CLIPRGN* pRgn);

/**
 * \fn HDC GUIAPI BeginPaint(HWND hWnd)
 * \brief Prepares a window for painting.
 *
 * This function prepares the specified window \a hWnd for painting.
 * This function is called in the handler of MSG_PAINT message normally.
 * It returns a device context including the update region of the window.
 * When you are done with the device context, you should call \a EndPaint
 * to finish the painting of the window.
 *
 * \param hWnd The handle to the window.
 * \return The device context.
 *
 * \sa EndPaint
 */
MG_EXPORT HDC GUIAPI BeginPaint(HWND hWnd);

/**
 * \fn void GUIAPI EndPaint(HWND hWnd, HDC hdc)
 * \brief Marks the end of painting in a window.
 *
 * This function marks the end of painting in the specified window.
 * This function is required for each call to the \a BeginPaint function,
 * but only after painting is complete.
 *
 * \param hWnd The handle to the window.
 * \param hdc The device context returned by BeginPaint.
 *
 * \sa BeginPaint
 */
MG_EXPORT void GUIAPI EndPaint(HWND hWnd, HDC hdc);

/**
 * \fn BOOL GUIAPI GetUpdateRect (HWND hWnd, RECT* update_rect)
 * \brief Retrieve the bounding box of the update region of a window.
 *
 * This function retrieves the bounding box of the update region of
 * the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param update_rect The pointer to a RECT structure which will contains
 *        the bounding box of the update region.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa InvalidateRect
 */
MG_EXPORT BOOL GUIAPI GetUpdateRect (HWND hWnd, RECT* update_rect);

/**
 * \fn int GUIAPI GetUpdateRegion (HWND hWnd, PCLIPRGN pRgn)
 * \brief Copy the update region of a window to a region.
 *
 * This function copies the update region of the specified window \a hWnd
 * into the region \a pRgn.
 *
 * \param hWnd The handle to the window.
 * \param pRgn The pointer to a region which receives the update region
 *        of the window.
 *
 * \return The region type.
 * \retval NULLREGION     Region is empty.
 * \retval SIMPLEREGION   Region is a single rectangle.
 * \retval COMPLEXREGION  Region is more than one rectangle.
 * \retval -1             An error occurred.
 *
 * \sa GetUpdateRect
 */
MG_EXPORT int GUIAPI GetUpdateRegion (HWND hWnd, PCLIPRGN pRgn);

/**
 * \fn int GUIAPI ClientWidthToWindowWidthEx (DWORD dwStyle, \
                     int win_type, int cw)
 * \brief Calculates main window width from the width of the client area.
 *
 * This function calculates the window width from the width of the
 * client area.
 *
 * \param dwStyle The style of window.
 * \param win_type The type of window.
 * \param cw The width of the client area.
 *
 * \return The width of the main window.
 *
 * \sa ClientHeightToWindowHeightEx
 */
MG_EXPORT int GUIAPI ClientWidthToWindowWidthEx (DWORD dwStyle,
        int win_type, int cw);

/**
 * \fn int GUIAPI ClientHeightToWindowHeightEx (DWORD dwStyle, int win_type, \
                int ch, BOOL hasMenu)
 * \brief Calculates window height from the height of the client area.
 *
 * This function calculates the main window height from the height of
 * the client area.
 *
 * \param dwStyle The style of window.
 * \param win_type The type of window.
 * \param ch The height of the client area.
 * \param hasMenu Indicates whether the main window has menu.
 * \return The height of the main window.
 *
 * \sa ClientWidthToWindowWidthEx
 */
MG_EXPORT int GUIAPI ClientHeightToWindowHeightEx (DWORD dwStyle,
        int win_type, int ch, BOOL hasMenu);

#define ClientWidthToWindowWidth(dwStyle, cw) \
    ClientWidthToWindowWidthEx (dwStyle, LFRDR_WINTYPE_MAINWIN, cw)

#define ClientHeightToWindowHeight(dwStyle, ch, hasMenu) \
    ClientHeightToWindowHeightEx (dwStyle, LFRDR_WINTYPE_MAINWIN, ch, hasMenu)

/**
 * \fn BOOL AdjustWindowRectEx (RECT* pRect, DWORD dwStyle, BOOL bMenu,
                DWORD dwExStyle)
 * \brief Calculates the required size of the window rectangle
 *        based on the desired size of the client rectangle.
 *
 * This function calculates the required size of the window rectangle,
 * based on the desired size of the client rectangle. The window rectangle
 * can then be passed to the CreateMainWindow or CreateWindow function to
 * create a window whose client area is the desired size.
 *
 * A client rectangle is the smallest rectangle that completely encloses
 * a client area. A window rectangle is the smallest rectangle that
 * completely encloses the window, which includes the client area and
 * the nonclient area.
 *
 * The AdjustWindowRectEx function does not add extra space when a menu bar
 * wraps to two or more rows.
 *
 * The AdjustWindowRectEx function takes the WS_VSCROLL or WS_HSCROLL
 * styles into account.
 *
 * \param pRect Pointer to a RECT structure that contains the coordinates
 *        of the top-left and bottom-right corners of the desired client area.
 *        When the function returns, the structure contains the coordinates
 *        of the top-left and bottom-right corners of the window to
 *        accommodate the desired client area.
 * \param dwStyle Specifies the window style of the window whose required
 *        size is to be calculated.
 * \param bMenu Specifies whether the window has a menu.
 * \param dwExStyle Specifies the extended window style of the window whose
 *        required size is to be calculated.
 *
 * \return If the function succeeds, the return value is nonzero.
 *         If the function fails, the return value is zero.
 *
 * \sa ClientWidthToWindowWidth, ClientHeightToWindowHeight
 */
MG_EXPORT BOOL GUIAPI AdjustWindowRectEx (RECT* pRect, DWORD dwStyle,
                BOOL bMenu, DWORD dwExStyle);

/**
 * \fn void GUIAPI ClientToScreen (HWND hWnd, int* x, int* y)
 * \brief Converts the client coordinates of a point to screen coordinates.
 *
 * This function converts the client coordinates of the specified point
 * \a (*x,*y) in the specified window \a hWnd to screen coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa ScreenToClient
 */
MG_EXPORT void GUIAPI ClientToScreen (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI ScreenToClient (HWND hWnd, int* x, int* y)
 * \brief Converts the screen coordinates of a point to client coordinates.
 *
 * This function converts the screen coordinates of the specified point
 * \a (*x,*y) to client coordinates of the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa ClientToScreen
 */
MG_EXPORT void GUIAPI ScreenToClient (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI ClientToWindow  (HWND hWnd, int* x, int* y)
 * \brief Converts the client coordinates to the window coordinates.
 *
 * This function converts the client coordinates of the specified point
 * \a (*x,*y) in the specified window \a hWnd to the window coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa WindowToClient
 */
MG_EXPORT void GUIAPI ClientToWindow(HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI WindowToClient (HWND hWnd, int* x, int* y)
 * \brief Converts the window coordinates to client coordinates.
 *
 * This function converts the window coordinates of the specified point
 * \a (*x,*y) in the specified window \a hWnd to the client coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa WindowToScreen
 */
MG_EXPORT void GUIAPI WindowToClient(HWND hWnd, int* x, int* y);


/**
 * \fn void GUIAPI WindowToScreen (HWND hWnd, int* x, int* y)
 * \brief Converts the window coordinates of a point to screen coordinates.
 *
 * This function converts the window coordinates of the specified point
 * \a (*x,*y) in the specified window \a hWnd to the screen coordinates.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa ScreenToWindow
 */
MG_EXPORT void GUIAPI WindowToScreen (HWND hWnd, int* x, int* y);

/**
 * \fn void GUIAPI ScreenToWindow (HWND hWnd, int* x, int* y)
 * \brief Converts the screen coordinates of a point to window coordinates.
 *
 * This function converts the screen coordinates of the specified point
 * \a (*x,*y) to the window coordinates of the specfied window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param x The pointer to the x coordinate.
 * \param y The pointer to the y coordinate.
 *
 * \sa WindowToScreen
 */
MG_EXPORT void GUIAPI ScreenToWindow (HWND hWnd, int* x, int* y);

/**
 * \fn BOOL GUIAPI IsMainWindow (HWND hWnd)
 * \brief Determine whether a window is a main window.
 *
 * This function determines whether the specified window \a hWnd is
 * a main window or not.
 *
 * \param hWnd The handle to the window.
 *
 * \return TRUE for main window, otherwise FALSE.
 *
 * \sa IsControl, IsWindow
 */
MG_EXPORT BOOL GUIAPI IsMainWindow (HWND hWnd);

#ifdef _MGHAVE_VIRTUAL_WINDOW
/**
 * \fn BOOL GUIAPI IsVirtualWindow (HWND hWnd)
 * \brief Determine whether a window is a virtual window.
 *
 * This function determines whether the specified window \a hWnd is
 * a virtual window or not.
 *
 * \param hWnd The handle to the window.
 *
 * \return TRUE for a virtual window, otherwise FALSE.
 *
 * \sa IsMainWindow, IsWindow, IsControl
 */
MG_EXPORT BOOL GUIAPI IsVirtualWindow (HWND hWnd);
#endif  /* defined _MGHAVE_VIRTUAL_WINDOW */

/**
 * \fn BOOL GUIAPI IsControl (HWND hWnd)
 * \brief Determine whether a window is a control.
 *
 * This function determines whether the specified window \a hWnd is a control.
 *
 * \param hWnd The handle to the window.
 *
 * \return TRUE for control, otherwise FALSE.
 *
 * \sa IsMainWindow, IsWindow
 */
MG_EXPORT BOOL GUIAPI IsControl (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsWindow (HWND hWnd)
 * \brief Determine whether a window handle identifies an existing window.
 *
 * This function determines whether the specified window handle \a hWnd
 * identifies an existing window.
 *
 * \param hWnd The window handle.
 *
 * \return TRUE for window, otherwise FALSE.
 *
 * \sa IsMainWindow, IsControl
 */
MG_EXPORT BOOL GUIAPI IsWindow (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsDialog (HWND hWnd)
 * \brief Determine whether a window handle identifies a dialog window.
 *
 * This function determines whether the specified window handle \a hWnd
 * identifies a dialog window.
 *
 * \param hWnd The window handle.
 *
 * \return TRUE for dialog window, otherwise FALSE.
 *
 * \sa IsMainWindow, IsControl
 */
MG_EXPORT BOOL GUIAPI IsDialog (HWND hWnd);

/**
 * \fn HWND GUIAPI GetParent (HWND hWnd)
 * \brief Retrieve the handle to a child window's parent window.
 *
 * This function retrieves the handle to the specified child window's
 * parent window.
 *
 * \param hWnd The handle to the child window.
 *
 * \return The handle to the parent, HWND_INVALID indicates an error.
 *
 * \note For a main window, this function always returns HWNL_NULL.
 *       For HWND_DESKTOP or an invalid window handle,
 *       HWND_INVALID will be returned.
 *
 * \sa GetMainWindowHandle
 */
MG_EXPORT HWND GUIAPI GetParent (HWND hWnd);

/**
 * \fn HWND GUIAPI GetMainWindowHandle (HWND hWnd)
 * \brief Retrieve the handle to the main window contains a window.
 *
 * This function retrieves the handle to the main window which contains the
 * specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the main window, HWND_INVALID indicates an error.
 *
 * \note For a main window, this function always returns the handle to itself.
 *       For HWND_DESKTOP, HWND_DESKTOP is returned.
 *
 * \sa GetParent
 */
MG_EXPORT HWND GUIAPI GetMainWindowHandle (HWND hWnd);

/**
 * \fn BOOL GUIAPI IsWindowVisible (HWND hWnd)
 * \brief Retrieve the visibility state of the specified window.
 *
 * This function retrieves the visibility state of the specified window \a hWnd.
 *
 * \param hWnd Handle to the window to test.
 *
 * \return If the specified window, its parent window, its parent's parent
 *         window, and so forth, all have the WS_VISIBLE style, the return
 *         value is nonzero. Otherwise the return value is zero.
 *
 * \sa ShowWindow
 */
MG_EXPORT BOOL GUIAPI IsWindowVisible (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetWindowRect (HWND hWnd, PRECT prc)
 * \brief Retrieve the dimensions of the bounding rectangle of a window.
 *
 * This function retrieves the dimension of the bounding rectangle of
 * the specified window \a hWnd. The dimensions are given in parent's
 * client coordinates (screen coordinates for main window) that are
 * relative to the upper-left corner of the parent's client area (screen).
 *
 * \param hWnd The handle to the window.
 * \param prc The pointer to a RECT structure which will contains the
 *        window rectangle.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetClientRect
 */
MG_EXPORT BOOL GUIAPI GetWindowRect (HWND hWnd, PRECT prc);

/**
 * \fn HWND GUIAPI GetNextChild (HWND hWnd, HWND hChild)
 * \brief Retrieve the next control in a window.
 *
 * This function retrieves the next child of the specified
 * window \a hWnd. If you pass HWND_NULL for the argument
 * of \a hChild, the function will return the first child of the window.
 *
 * \param hWnd The handle to the window.
 * \param hChild The handle to a child of the window.
 * \return The handle of the next child. If you pass HWND_NULL for
 *         the argument of \a hChild, it will return the first child of the
 *         window. When the child you passed is the last one, this function
 *         will return HWND_NULL. If hWnd is not the parent of hChild,
 *         this function will return HWND_INVALID.
 *
 * \sa GetNextMainWindow
 *
 * \note Do not use HWND_DESKTOP for \a hWnd or \a hChild.
 *
 * Example:
 *
 * \include nextcontrol.c
 */
MG_EXPORT HWND GUIAPI GetNextChild (HWND hWnd, HWND hChild);

/**
 * \fn HWND GUIAPI GetNextMainWindow (HWND hMainWnd)
 * \brief Retrieve the next main window in the system according to the zorder.
 *
 * This function retrieves the next main window of the specified
 * main window \a hMainWnd.
 *
 * \param hMainWnd The handle to the main window.
 *
 * \return The handle of the next main window. If you pass HWND_NULL
 *         for the argument of \a hMainWnd, it will return the topmost main
 *         window. When the main window you passed is the bottommost one,
 *         this function will return HWND_NULL. If hMainWnd is not a main
 *         window, the function will return HWND_INVALID.
 *
 * \sa GetNextChild
 */
MG_EXPORT HWND GUIAPI GetNextMainWindow (HWND hMainWnd);

#define WIN_SEARCH_METHOD_MASK  0xFF00
#define WIN_SEARCH_METHOD_BFS   0x0000
#define WIN_SEARCH_METHOD_DFS   0x0100

#define WIN_SEARCH_FILTER_MASK  0x00FF
#define WIN_SEARCH_FILTER_MAIN  0x0001
#define WIN_SEARCH_FILTER_VIRT  0x0002

/**
 * \fn HWND GUIAPI GetHostedById (HWND hHosting,
 *      LINT lId, DWORD dwSearchFflags)
 * \brief Retrieve a hosted main window or virtual window by identifier.
 *
 * All main windows and/or virtual windows in a thread form a window tree.
 * The root window of the tree may be HWND_DESKTOP or the first main/virtual
 * window created in the thread.
 *
 * This function retrieves the first window which has the specified identifier
 * \a id in the window tree of the current thread.
 *
 * \param hHosting The handle to a main or virtual window in the thread,
 *  which will be the root of the sub window tree to search. If it is
 *  HWND_NULL, this function will use the root window of the current thread.
 * \param lId The identifier.
 * \param dwSearchFflags The search flags, should be OR'd with a search method
 *  value and one or two search filter values:
 *      - WIN_SEARCH_METHOD_BFS\n
 *        use BFS (breadth-first search).
 *      - WIN_SEARCH_METHOD_DFS\n
 *        use DFS (depth-first search).
 *      - WIN_SEARCH_FILTER_MAIN\n
 *        search main windows.
 *      - WIN_SEARCH_FILTER_VIRT\n
 *        search virtual windows.
 *
 * \return The handle to the first main window or virtual window which has the
 *  specified identifier in the searching sub window tree.
 *  If the current thread is not a message thread, it returns HWND_INVALID.
 *  If there is no window matches the identifier and the search flags,
 *  it returns HWND_NULL.
 *
 * \sa GetRootWindow, GetHosting, GetFirstHosted, GetNextHosted
 */
MG_EXPORT HWND GUIAPI GetHostedById (HWND hHosting,
        LINT lId, DWORD dwSearchFlags);

/**
 * \fn LINT GUIAPI GetWindowId (HWND hWnd)
 * \brief Get the identifier of a window.
 *
 * This function returns the identifier of the specified window \a hWnd.
 *
 * \return The identifier of the window. This function returns -1 for
 *  an invalid window handle. Therefore, you should avoid to use -1 as
 *  a valid identifier of a window.
 *
 * \sa SetWindowId
 */
MG_EXPORT LINT GUIAPI GetWindowId (HWND hWnd);

/**
 * \fn LINT GUIAPI SetWindowId (HWND hWnd, LINT lNewId)
 * \brief Set the identifier of a window.
 *
 * This function sets the identifier of the specified window \a hWnd to
 * \a lNewId and returns the old identifier.
 *
 * \return The old identifier of the window. This function returns -1 for
 *  an invalid window handle. Therefore, you should avoid to use -1 as
 *  a valid identifier of a window.
 *
 * \sa GetWindowId
 */
MG_EXPORT LINT GUIAPI SetWindowId (HWND hWnd, LINT lNewId);

/**
 * \fn HWND GUIAPI GetRootWindow (int* nrWins)
 * \brief Retrieve the root window of the current thread.
 *
 * All main windows and/or virtual windows in a thread form a window tree.
 * The root window of the tree may be HWND_DESKTOP or the first main/virtual
 * window created in the thread.
 *
 * This function retrieves and returns the root window in the current thread.
 *
 * \param nrWins A pointer to an integer used to return the number of total
 *  windows in the current thread. It can be NULL.
 *
 * \return The handle to the root window. If the current thread is not
 *  a message thread, it returns HWND_INVALID. If there is no
 *  any window created in the current thread, it returns HWND_NULL.
 *
 * \sa GetHosting, GetFirstHosted, GetNextHosted, GetMainWindowById
 */
MG_EXPORT HWND GUIAPI GetRootWindow (int* nrWins);

/**
 * \fn HWND GUIAPI GetHosting (HWND hWnd)
 * \brief Retrieve the hosting window of a main window or a virtual window.
 *
 * All main windows and/or virtual windows created by a thread form a
 * window hosting tree. The root window of the tree may be HWND_DESKTOP
 * or the first main/virtual window created by the thread.
 *
 * This function retrieves the hosting window of the specified main window
 * or virtual window \a hWnd.
 *
 * For a root window in the current thread, this function returns HWND_NULL.
 *
 * \param hWnd The handle to the main window.
 *
 * \return The handle to the hosting window. If error occurs, for example,
 *  an invalid handle, the specified window is not in the current thread,
 *  or the current thread is not a message thread, this function returns
 *  HWND_INVALID. For a root window in the current thread, this function
 *  returns HWND_NULL.
 *
 * \sa GetRootWindow, GetFirstHosted, GetNextHosted
 */
MG_EXPORT HWND GUIAPI GetHosting (HWND hWnd);

/**
 * \fn HWND GUIAPI GetFirstHosted (HWND hHosting)
 * \brief Retrieve the first hosted window of a main window or a virtual window.
 *
 * All main windows and/or virtual windows in a thread form a window tree.
 * The root window of the tree may be HWND_DESKTOP or the first main/virtual
 * window created in the thread.
 *
 * This function retrieves the first hosted main/virtual window of
 * the specified main window \a hMainWnd.
 *
 * \param hHosting The handle to a main window or a virtual window.
 *
 * \return The handle to the first hosted window. If an invalid window
 *  handle is passed for \a hHosting, HWND_INVALID will be returned.
 *  If the specified window do not have a hosted window, this function
 *  returns HWND_NULL.
 *
 * \sa GetHosting, GetNextHosted
 */
MG_EXPORT HWND GUIAPI GetFirstHosted (HWND hHosting);

/**
 * \fn HWND GUIAPI GetNextHosted (HWND hHosting, HWND hHosted)
 * \brief Retrieve the next hosted window of a main window or a virtual window.
 *
 * All main windows and/or virtual windows in a thread form a window tree.
 * The root window of the tree may be HWND_DESKTOP or the first main/virtual
 * window created in the thread.
 *
 * This function retrieves the next hosted main/virtual window of the specified
 * main/virtual window \a hHosting. If \a hHosted is HWND_NULL, it is
 * equivalent to call GetFirstHosted(hHosting).

 * \param hHosting The handle to the hosting window.
 * \param hHosted The handle to a known hosted window. This function
 *        will return the next hosted window.
 *
 * \return The handle to the next hosted main or virtual window.
 *  It returns HWND_NULL when \a hHosted is the last hosted window.
 *  If invalid window handles are passed, or if \a hHosted is not a hosted
 *  window of \a hHosting, this function returns HWND_INVALID.
 *
 * \sa GetHosting, GetFirstHosted
 */
MG_EXPORT HWND GUIAPI GetNextHosted (HWND hHosting, HWND hHosted);

/**
 * \fn int GUIAPI GetWindowTextLength (HWND hWnd)
 * \brief Retrieve the length of a window's text.
 *
 * This function retrieves the length, in characters, of the specified
 * window's text. The function retrieves the length of the text by sending
 * an MSG_GETTEXTLENGTH message to the window.
 *
 * \param hWnd The handle to the window.
 *
 * \return The length of the window's text.
 *
 * \sa GetWindowText
 */
MG_EXPORT int GUIAPI GetWindowTextLength (HWND hWnd);

/**
 * \fn int GUIAPI GetWindowText (HWND hWnd, char* spString, int nMaxLen)
 * \brief Copies the text of a window's into a buffer.
 *
 * This function copies the text of the specified window \a hWnd into the buffer
 * pointed to by \a spString. The function gets the window text by sending
 * an MSG_GETTEXT message to the window.
 *
 * \param hWnd The handle to the window.
 * \param spString The pointer to a buffer receives the text.
 * \param nMaxLen The maximal number of characters can be copied to the buffer.
 *
 * \return The length of the window text string.
 *
 * \sa GetWindowCaption, SetWindowText
 */
MG_EXPORT int GUIAPI GetWindowText (HWND hWnd, char* spString, int nMaxLen);

/**
 * \fn BOOL GUIAPI SetWindowText (HWND hWnd, const char* spString)
 * \brief Set the text of a window.
 *
 * This function copies the string in the buffer pointed to by \a spString
 * to be the text of the specified window \a hWnd. The function sets
 * the window text by sending an MSG_SETTEXT message to the window.
 *
 * \param hWnd The handle to the window.
 * \param spString The pointer to the buffer.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetWindowCaption, GetWindowText
 */
MG_EXPORT BOOL GUIAPI SetWindowText (HWND hWnd, const char* spString);

/**
 * \fn HWND GUIAPI GetFocusChild (HWND hParent)
 * \brief Retrieve the handle to the window's active child that has
 *        the keyboard focus.
 *
 * This function retrieves the handle to the window's active child that has
 * the keyboard focus.
 *
 * \param hParent The handle to the parent window.
 *
 * \return The handle to the active child.
 *
 * \sa SetFocusChild, SetNullFoucs
 */
MG_EXPORT HWND GUIAPI GetFocusChild (HWND hParent);

/**
 * \fn HWND GUIAPI SetNullFocus (HWND hParent)
 * \brief Cancels the current active child and set the focus child to be null.
 *
 * This function cancels the current active child and set the focus child
 * of the window \a hParent to be null.
 *
 * \param hParent The handle to the parent window.
 *
 * \return The handle to the old active child.
 *
 * \sa GetFocusChild, SetFocusChild
 */
MG_EXPORT HWND GUIAPI SetNullFocus (HWND hParent);

/**
 * \fn HWND GUIAPI SetFocusChild (HWND hWnd)
 * \brief Set the active child of a window.
 *
 * This function sets the specified window \a hWnd as the active child of
 * its parent.
 *
 * \param hWnd The handle to the window.
 *
 * \return The handle to the old active child of its parent.
 *
 * \sa GetFocusChild, SetNullFocus
 */
MG_EXPORT HWND GUIAPI SetFocusChild (HWND hWnd);

/**
 * \def SetFocus
 * \sa SetFocusChild
 */
#define SetFocus SetFocusChild

/**
 * \def GetFocus
 * \sa GetFocusChild
 */
#define GetFocus GetFocusChild

/**
 * \fn HWND GUIAPI GetActiveWindow (void)
 * \brief Retrieve the main window handle of the active main window.
 *
 * This function retrieves the main window handle of the active main window
 * which receives the input.
 *
 * \return The handle to the active main window.
 *
 * \sa SetActiveWindow, GetFocusChild
 */
MG_EXPORT HWND GUIAPI GetActiveWindow (void);

/**
 * \fn HWND GUIAPI SetActiveWindow (HWND hMainWnd)
 * \brief Set a main window to be the active main window.
 *
 * This function sets the specified main window \a hMainWnd to be the
 * active main window which receives the input.
 *
 * \param hMainWnd The handle to the new active main window.
 *
 * \return The handle to the old active main window.
 *
 * \sa GetActiveWindow, SetFocusChild
 */
MG_EXPORT HWND GUIAPI SetActiveWindow (HWND hMainWnd);

/**
 * \def GetForegroundWindow
 * \sa GetActiveWindow
 */
#define GetForegroundWindow GetActiveWindow

/**
 * \def SetForegroundWindow
 * \sa SetActiveWindow
 */
#define SetForegroundWindow SetActiveWindow

/**
 * \fn HWND GUIAPI GetCapture(void)
 * \brief Retrieve the handle to the window (if any) that has captured
 * the mouse.
 *
 * This function retrieves the handle to the window (if any) that has captured
 * the mouse.  Only one window at a time can capture the mouse; this window
 * receives mouse input whether or not the cursor is within its borders.
 *
 * \return The handle to the window that has captured the mouse, 0 for
 *         no window captures the mouse.
 *
 * \sa SetCapture
 */
MG_EXPORT HWND GUIAPI GetCapture(void);

/**
 * \fn HWND GUIAPI SetCapture(HWND hWnd)
 * \brief Set the mouse capture to the specified window.
 *
 * This function sets the mouse capture to the specified window \a hWnd.
 * Once a window has captured the mouse, all mouse input is directed to
 * that window, regardless of whether the cursor is within the borders
 * of that window. Only one window at a time can capture the mouse.
 *
 * \param hWnd The handle to the window.
 *
 * \return The old capture window.
 *
 * \sa GetCapture
 */
MG_EXPORT HWND GUIAPI SetCapture(HWND hWnd);

/**
 * \fn void GUIAPI ReleaseCapture(void)
 * \brief Releases the mouse capture from a window and restores normal mouse
 *        input processing.
 *
 * This function releases the mouse capture from a window and restores normal
 * mouse input processing. A window that has captured the mouse receives
 * all mouse input, regardless of the position of the cursor.
 *
 * \sa GetCapture, SetCapture
 */
MG_EXPORT void GUIAPI ReleaseCapture(void);

/**
 * \fn HWND GUIAPI GetWindowUnderCursor(void)
 * \brief Retrieve the handle to the window (if any) which is just
 *        beneath the mouse cursor.
 *
 * This function retrieves the handle to the window (if any) that is under
 * the mouse cursor. If a parent window and a child window are all under
 * the mouse cursor, the handle to the child window will be returned.
 *
 * \return The handle to the window under the mouse cursor,
 *         HWND_NULL for no window is under the mouse cursor.
 */
MG_EXPORT HWND GUIAPI GetWindowUnderCursor (void);

/**
 * \fn HWND GUIAPI WindowFromPointEx (POINT pt, BOOL bRecursion)
 * \brief Retrieve a handle to the window that contains the specified point.
 *
 * This function retrieves a handle to the main window that contains the
 * specified point \a pt.
 *
 * \param pt Specifies a POINT structure that defines the point to be checked.
 * \param bRecursion Try find the child window of the mMainWindow
 *
 * \return The return value is a handle to the main window that contains
 *         the point. If no main window exists at the given point,
 *         the return value is HWND_NULL.
 */
MG_EXPORT HWND GUIAPI WindowFromPointEx (POINT pt, BOOL bRecursion);

#define WindowFromPoint(pt)  WindowFromPointEx(pt, TRUE)

#define CWP_ALL             0x0000
#define CWP_SKIPINVISIBLE   0x0001
#define CWP_SKIPDISABLED    0x0002
#define CWP_SKIPTRANSPARENT 0x0004

/**
 * \fn HWND GUIAPI ChildWindowFromPointEx (HWND hParent, POINT pt,
                    UINT uFlags)
 * \brief Retrieve a handle to the child window that contains the
 *        speicified point and meets the certain criteria.
 *
 * This function determines which, if any, of the child windows
 * belonging to a parent window contains the specified point.
 * The function can ignore invisible, disabled, and transparent
 * child windows. The search is restricted to immediate child windows.
 * Grandchildren, and deeper descendant windows are not searched.
 *
 * \param hParent Handle to the parent window.
 * \param pt Specifies a POINT structure that defines the client
 *        coordinates (relative to hParent) of the point to be checked.
 * \param uFlags Specifies which child windows to skip. This parameter can be
 *        one or more of the following values.
 *          - CWP_ALL\n
 *            Does not skip any child windows
 *          - CWP_SKIPINVISIBLE\n
 *            Skips invisible child windows
 *          - CWP_SKIPDISABLED\n
 *            Skips disabled child windows
 *          - CWP_SKIPTRANSPARENT\n
 *            Skips transparent child windows
 *
 * \return The return value is a handle to the first child window
 *         that contains the point and meets the criteria specified
 *         by uFlags. If the point is within the parent window but
 *         not within any child window that meets the criteria,
 *         the return value is a handle to the parent window.
 *         If the point lies outside the parent window or if the
 *         function fails, the return value is HWND_NULL.
 *
 * \sa ChildWindowFromPoint
 */
MG_EXPORT HWND GUIAPI ChildWindowFromPointEx (HWND hParent, POINT pt,
                UINT uFlags);

/**
 * \fn HWND GUIAPI ChildWindowFromPoint (HWND hParent, POINT pt)
 * \brief Retrieve a handle to the child window that contains the
 *        speicified point.
 *
 * This function determines which, if any, of the child windows
 * belonging to a parent window contains the specified point by calling
 * ChildWindowFromPointEx and passing CWP_ALL to uFlags parameter.
 *
 * \sa ChildWindowFromPointEx
 */
static inline HWND GUIAPI ChildWindowFromPoint (HWND hParent, POINT pt)
{
    return ChildWindowFromPointEx (hParent, pt, CWP_ALL);
}

/**
 * \fn BOOL GUIAPI MoveWindow (HWND hWnd, int x, int y, int w, int h, \
 *               BOOL fPaint)
 * \brief Changes the position and dimensions of a window.
 *
 * This function changes the position and dimensions of the specified window
 * \a hWnd.  For a main window, the position and dimensions are relative to
 * the upper-left corner of the screen. For a control, they are relative to
 * the upper-left corner of the parent window's client area.
 *
 * \param hWnd The handle to the window.
 * \param x The new x coordinate of the upper-left corner of the window.
 * \param y The new y coordinate of the upper-left corner of the window.
 * \param w The new width of the window.
 * \param h The new height of the window.
 * \param fPaint Indicates whether the window should be repainted.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ScrollWindow
 */
MG_EXPORT BOOL GUIAPI MoveWindow (HWND hWnd, int x, int y, int w, int h,
                BOOL fPaint);

#define SW_INVALIDATE       0x01
#define SW_ERASE            0x02
#define SW_SCROLLCHILDREN   0x04

/**
 * \fn int GUIAPI ScrollWindowEx (HWND hWnd, int dx, int dy,
                const RECT *prcScroll, const RECT *prcClip,
                PCLIPRGN pRgnUpdate, PRECT prcUpdate, UINT flags)
 * \brief Scrolls the content of a window's client area.
 *
 * This function scrolls the content of a window's client area.
 *
 * If the SW_INVALIDATE and SW_ERASE flags are not specified,
 * ScrollWindowEx does not invalidate the area that is scrolled from.
 * If either of these flags is set, ScrollWindowEx invalidates this area.
 * The area is not updated until the application calls the UpdateWindow
 * function or retrieves the MSG_PAINT message from the application queue.
 *
 * If the window has the WS_CLIPCHILDREN style, the returned areas
 * specified by pRgnUpdate and prcUpdate represent the total area of
 * the scrolled window that must be updated, including any areas in
 * child windows that need updating.
 *
 * If the SW_SCROLLCHILDREN flag is specified, the system does not
 * properly update the screen if part of a child window is scrolled.
 * The part of the scrolled child window that lies outside the source
 * rectangle is not erased and is not properly redrawn in its new
 * destination. To move child windows that do not lie completely
 * within the rectangle specified by prcScroll, use the DeferWindowPos
 * function. The cursor is repositioned if the SW_SCROLLCHILDREN flag is set
 * and the caret rectangle intersects the scroll rectangle.
 *
 * All input and output coordinates (for prcScroll, prcClip, prcUpdate,
 * and pRgnUpdate) are determined as client coordinates.
 *
 * \param hWnd Handle to the window where the client area is to be scrolled.
 * \param dx Specifies the amount, in device units, of horizontal scrolling.
 *        This parameter must be a negative value to scroll to the left.
 * \param dy Specifies the amount, in device units, of vertical scrolling.
 *        This parameter must be a negative value to scroll up.
 * \param prcScroll Pointer to a RECT structure that specifies the
 *        portion of the client area to be scrolled. If this parameter
 *        is NULL, the entire client area is scrolled.
 * \param prcClip Pointer to a RECT structure that contains the coordinates
 *        of the clipping rectangle. Only device bits within the clipping
 *        rectangle are affected. Bits scrolled from the outside of the
 *        rectangle to the inside are painted; bits scrolled from the inside
 *        of the rectangle to the outside are not painted. This parameter
 *        may be NULL.
 * \param pRgnUpdate Pointer to the region that is modified to hold the
 *        region invalidated by scrolling. This parameter may be NULL.
 * \param prcUpdate Pointer to a RECT structure that receives the
 *        boundaries of the rectangle invalidated by scrolling. This
 *        parameter may be NULL.
 * \param flags Specifies flags that control scrolling. This parameter
 *        can be one of the following values:
 *        - SW_ERASE\n
 *          Erases the newly invalidated region by sending a MSG_ERASEBKGND
 *          message to the window when specified with the SW_INVALIDATE flag.
 *        - SW_INVALIDATE\n
 *          Invalidates the region identified by the pRgnUpdate parameter
 *          after scrolling.
 *        - SW_SCROLLCHILDREN\n
 *          Scrolls all child windows that intersect the rectangle pointed
 *          to by the prcScroll parameter. The child windows are scrolled
 *          by the number of pixels specified by the dx and dy parameters.
 *          MiniGUI calls MoveWindow to all child windows that
 *          intersect the prcScroll rectangle, even if they do not move.
 *
 * \return If the function succeeds, the return value is SIMPLEREGION
 *         (rectangular invalidated region), COMPLEXREGION (nonrectangular
 *         invalidated region; overlapping rectangles), or NULLREGION
 *         (no invalidated region).
 *         If the function fails, the return value is -1.
 */
MG_EXPORT int GUIAPI ScrollWindowEx (HWND hWnd, int dx, int dy,
                const RECT *prcScroll, const RECT *prcClip,
                PCLIPRGN pRgnUpdate, PRECT prcUpdate, UINT flags);

/**
 * \fn void GUIAPI ScrollWindow (HWND hWnd, int dx, int dy, \
                const RECT* prcScroll, const RECT* prcClip)
 * \brief Scrolls the content of a window's client area.
 *
 * This function scrolls the content of the specified window's client area.
 *
 * \param hWnd The handle to the window.
 * \param dx The new x coordinate of the origin in the client coordinates
 *        system after scrolling.
 * \param dy The new y coordinate of the origin in the client coordinates
 *        system after scrolling.
 * \param prcScroll The rectangle of the area which will be scrolled actually.
 *        NULL for whole client area.
 * \param prcClip A rectangle, all children covered totally by this rectangle
 *        will be moved after scrolling. All of the children will be
 *        moved if \a prcClip is NULL.
 *
 * \sa MoveWindow
 */
static inline void GUIAPI ScrollWindow (HWND hWnd, int dx, int dy,
               const RECT* prcScroll, const RECT* prcClip)
{
    ScrollWindowEx (hWnd, dx, dy, prcScroll, prcClip,
                NULL, NULL, SW_ERASE | SW_INVALIDATE | SW_SCROLLCHILDREN);
}

#if 0
/* deprecated. */
#define GetWindowElementColor(iItem)   \
        GetWindowElementPixelEx(HWND_NULL, (HDC)-1, iItem)

/* deprecated. */
#define GetWindowElementColorEx(hWnd, iItem)   \
        GetWindowElementPixelEx(hWnd, (HDC)-1, iItem)
#endif

/**
 * \def GetWindowElementPixel
 * \brief Get window element pixel value.
 * \sa GetWindowElementPixelEx
 */
#define GetWindowElementPixel(hWnd, iItem)          \
        GetWindowElementPixelEx(hWnd, HDC_INVALID, iItem)

    /** @} end of window_general_fns */

    /**
     * \defgroup sys_bmps System bitmap/icon operations
     * @{
     */
#define SYSBMP_RADIOBUTTON          "radiobutton"
#define SYSBMP_CHECKBUTTON          "checkbutton"
#define SYSBMP_BGPICTURE            "bgpicture"
#define SYSBMP_BGPICPOS             "bgpicpos"

/* Obsolete definitions; back-compatibility definitions. */
#define SYSBMP_IMECTRLBTN          "IMEctrlbtn"
#define SYSBMP_LOGO                "logo"

/**
 * \fn const BITMAP* GUIAPI GetSystemBitmapEx2 (HDC hdc,
 *      const char* rdr_name, const char* id)
 * \brief Retrieve the system bitmap object by identifier.
 *
 * This function retrieves and returns the system bitmap object specified by
 * the renderer name \a rdr_name and the identifier \a id. This function
 * returns the BITMAP object which is compliant to the specified device context
 * \a hdc.
 *
 * \param hdc The handle to the device context.
 * \param rdr_name The renderer name.
 * \param id The identifier of the system bitmap object, can be
 *           one of the following values:
 *
 *      - SYSBMP_RADIOBUTTON\n
 *      - SYSBMP_CHECKBUTTON\n
 *      - SYSBMP_BGPICTURE\n
 *
 * \return The pointer to the system bitmap object.
 *
 * \note Since 5.0.0, if you use the compositing schema, a main window may
 *  use a private surface which is not compliant to the screen surface. Under
 *  this situation, you should use this function to load the system bitmap
 *  object instead of using \a GetSystemBitmapEx.
 *
 * \sa GetSystemBitmapEx
 *
 * Since 5.0.0.
 */
MG_EXPORT const BITMAP* GUIAPI GetSystemBitmapEx2 (HDC hdc,
        const char* rdr_name, const char* id);

/**
 * \fn const BITMAP* GUIAPI GetSystemBitmapEx (const char* rdr_name,
 *      const char* id)
 * \brief Retrieve the system bitmap object by identifier.
 *
 * This function retrieves and returns the system bitmap object by the renderer
 * name \a rdr_name and the identifier \a id. This function returns a BITMAP
 * object which complies which is compliant to HDC_SCREEN.
 *
 * \param rdr_name The renderer name.
 * \param id The identifier of the system bitmap object, can be
 *           one of the following values:
 *
 *      - SYSBMP_RADIOBUTTON\n
 *      - SYSBMP_CHECKBUTTON\n
 *      - SYSBMP_BGPICTURE\n
 *
 * \return The pointer to the system bitmap object.
 *
 * \sa GetLargeSystemIcon, GetSmallSystemIcon
 */
static inline const BITMAP* GUIAPI GetSystemBitmapEx (const char* rdr_name,
        const char* id)
{
    return GetSystemBitmapEx2 (HDC_SCREEN, rdr_name, id);
}

/**
 * \fn PBITMAP GUIAPI GetSystemBitmap (HWND hWnd, const char* id)
 * \brief Retrieve the system bitmap object by identifier.
 *
 * This function retrieves and returns the system bitmap object by
 * its identifier \a id for the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \param id The identifier of the system bitmap object, can be
 *  one of the following values:
 *
 *      - SYSBMP_RADIOBUTTON\n
 *      - SYSBMP_CHECKBUTTON\n
 *      - SYSBMP_BGPICTURE\n
 *
 * \return The pointer to the system bitmap object.
 *
 * \sa GetSystemBitmapEx2
 */
MG_EXPORT const BITMAP* GUIAPI GetSystemBitmap (HWND hWnd, const char* id);

/**
 * \def GetSystemBitmapByHwnd(hWnd, id)
 * \brief An alias of GetSystemBitmap.
 *
 * \sa GetSystemBitmap
 */
#define GetSystemBitmapByHwnd(hWnd, id) GetSystemBitmap ((hWnd), (id))

/**
 * \fn void GUIAPI TermSystemBitmapEx (const char* id, \
                     const char* rdr_name, PBITMAP bmp)
 * \brief Release the system bitmap object by identifier.
 *
 * \param id The identifier of the system bitmap object, can be
 * one of the following values:
 *
 *      - SYSBMP_RADIOBUTTON\n
 *      - SYSBMP_CHECKBUTTON\n
 *      - SYSBMP_BGPICTURE\n
 *
 * \param rdr_name The renderer name.
 * \param bmp The pointer to the system bitmap object.
 *
 * \return none.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT void GUIAPI TermSystemBitmapEx (const char* id,
        const char* rdr_name, PBITMAP bmp);

/**
 * \fn void GUIAPI TermSystemBitmap (HWND hWnd, const char* id, PBITMAP bmp)
 * \brief Release the system bitmap object by identifier.
 *
 * \param hWnd The handle to the window.
 * \param id The identifier of the system bitmap object, can be
 * one of the following values:
 *
 *      - SYSBMP_RADIOBUTTON\n
 *      - SYSBMP_CHECKBUTTON\n
 *      - SYSBMP_BGPICTURE\n
 *
 * \param bmp The pointer to the system bitmap object.
 *
 * \return none.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT void GUIAPI TermSystemBitmap (HWND hWnd, const char* id, PBITMAP bmp);

/*for backward compatibility */
#define IDI_APPLICATION         0
#define IDI_HAND                1
#define IDI_STOP                IDI_HAND
#define IDI_QUESTION            2
#define IDI_EXCLAMATION         3
#define IDI_ASTERISK            4
#define IDI_INFORMATION         IDI_ASTERISK

#define SYSICON_FT_DIR          "dir"
#define SYSICON_FT_FILE         "file"

#define SYSICON_TREEFOLD        "treefold"
#define SYSICON_TREEUNFOLD      "treeunfold"

/**
 * \fn HICON GUIAPI LoadSystemIconEx (HDC hdc, \
         const char* rdr_name, const char* szItemName, int which)
 * \brief Loads an icon from information defined in MiniGUI.cfg.
 *
 * This function loads an icon from information defined in MiniGUI.cfg.
 *
 * \param hdc The device context.
 * \param rdr_name The renderer name.
 * \param szItemName The key name in the section of renderer name in MiniGUI.cfg.
 * \param which Tell the function to load which icon. 0 for the small icon,
 *        and 1 for large icon.
 *
 * \return The handle to the loaded icon.
 *
 * \sa LoadIconFromFile, DestroyIcon
 */
MG_EXPORT HICON GUIAPI LoadSystemIconEx (HDC hdc,
        const char* rdr_name, const char* szItemName, int which);

/**
 * \fn HICON GUIAPI LoadSystemIcon (const char* szItemName, int which)
 * \brief Uses default renderer and HDC_SCREEN to load an icon from
 * information defined in MiniGUI.cfg.
 *
 * This function loads an icon from information defined in MiniGUI.cfg.
 *
 * \param szItemName The key name in the section of renderer name in MiniGUI.cfg.
 * \param which Tell the function to load which icon.
 *
 * \return The handle to the loaded icon.
 *
 * \sa LoadIconFromFile, DestroyIcon
 */
MG_EXPORT HICON GUIAPI LoadSystemIcon (const char* szItemName, int which);

/**
 * \fn HICON GUIAPI GetLargeSystemIconEx (HWND hWnd, int iItem)
 * \brief Retrieve a large system icon by its identifier in default renderer.
 *
 * This function retrieves the handle to a large (32x32) system icon
 * by its identifier \a id.
 *
 * \param hWnd The handle to the window.
 * \param iItem The identifier of the icon. It can be one of the
 *        following values:
 *      - IDI_APPLICATION\n The application icon.
 *      - IDI_STOP\n        The stop icon.
 *      - IDI_QUESTION\n    The question mark icon.
 *      - IDI_EXCLAMATION\n The exclamation mark icon.
 *      - IDI_INFORMATION\n The information mark icon.
 *
 * \return The handle to the icon.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT HICON GUIAPI GetLargeSystemIconEx (HWND hWnd, int iItem);

/**
 * \fn HICON GUIAPI GetSmallSystemIconEx (HWND hWnd, int iItem)
 * \brief Retrieve a small system icon by its identifier.
 *
 * This function retrieves the handle to a small (16x16) system icon by
 * its identifier \a id.
 *
 * \param hWnd The handle to the window.
 * \param iItem The identifier of the icon. It can be one of the
 *        following values:
 *      - IDI_APPLICATION\n The application icon.
 *      - IDI_STOP\n        The stop icon.
 *      - IDI_QUESTION\n    The question mark icon.
 *      - IDI_EXCLAMATION\n The exclamation mark icon.
 *      - IDI_INFORMATION\n The information mark icon.
 *
 * \return The handle to the icon.
 *
 * \sa GetSystemBitmap
 */
MG_EXPORT HICON GUIAPI GetSmallSystemIconEx (HWND hWnd, int iItem);

/**
 * \def GetLargeSystemIcon
 * \brief Retrieve a large (32x32) system icon by its identifier
 *        in default renderer.
 * \sa GetSmallSystemIconEx
 */
#define GetLargeSystemIcon(iItem) GetLargeSystemIconEx(HWND_NULL, iItem)

/**
 * \def GetSmallSystemIcon
 * \brief Retrieve a small (16x16) system icon by its identifier
 *        in default renderer.
 * \sa GetSmallSystemIconEx
 */
#define GetSmallSystemIcon(iItem) GetSmallSystemIconEx(HWND_NULL, iItem)

    /** @} end of sys_bmps */

    /**
     * \defgroup scrollbar_fns Scroll bar operations
     * @{
     */

#define SB_HORZ     1
#define SB_VERT     2

/**
 * \fn BOOL GUIAPI EnableScrollBar (HWND hWnd, int iSBar, BOOL bEnable)
 * \brief Enables or disables one scroll bar arrows.
 *
 * This function enables or disables one scroll bar arrows.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param bEnable Indicates whether to enable or disable the scroll bar.
 * \return TRUE on success, FALSE on error.
 *
 * \sa ShowScrollBar
 */
MG_EXPORT BOOL GUIAPI EnableScrollBar (HWND hWnd, int iSBar, BOOL bEnable);

/**
 * \fn BOOL GUIAPI GetScrollPos (HWND hWnd, int iSBar, int* pPos)
 * \brief Retrieve the current position of the scroll box (thumb) in the
 *        specified scroll bar.
 *
 * This function retrieves the current position of the scroll box (thumb) in
 * the specified scroll bar. The current position is a relative value that
 * depends on the current scrolling range.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param pPos The pointer to a integer which receives the position value.
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollPos
 */
MG_EXPORT BOOL GUIAPI GetScrollPos (HWND hWnd, int iSBar, int* pPos);

/**
 * \fn BOOL GUIAPI GetScrollRange (HWND hWnd, int iSBar, int* pMinPos, int* pMaxPos)
 * \brief Retrieve the minimum and maximum position values for the specified
 * scroll bar.
 *
 * This function retrieves the minimum and maximum position values for
 * the specified scroll bar.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param pMinPos The pointer to a integer which receives the minimum
 *        position value.
 * \param pMaxPos The pointer to a integer which receives the maximum
 *        position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollRange
 */
MG_EXPORT BOOL GUIAPI GetScrollRange (HWND hWnd, int iSBar,
                int* pMinPos, int* pMaxPos);

/**
 * \fn BOOL GUIAPI SetScrollPos (HWND hWnd, int iSBar, int iNewPos)
 * \brief Set the position of the scroll box (thumb) of the specified
 * scroll bar.
 *
 * This function sets the position of the scroll box (thumb) of the specified
 * scroll bar, and if needed, redraws the scroll bar to reflect the new
 * position of the scroll box.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param iNewPos The new position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollPos
 */
MG_EXPORT BOOL GUIAPI SetScrollPos (HWND hWnd, int iSBar, int iNewPos);

/**
 * \fn BOOL GUIAPI SetScrollRange (HWND hWnd, int iSBar, \
 *               int iMinPos, int iMaxPos)
 * \brief Set the minimum and maximum position values for the specified
 * scroll bar.
 *
 * This function sets the minimum and maximum position values for the
 * specified scroll bar, and if needed, redraws the scroll bar to reflect
 * the new position values of the scroll box.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param iMinPos The new minimum position value.
 * \param iMaxPos The new maximum position value.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollRange
 */
MG_EXPORT BOOL GUIAPI SetScrollRange (HWND hWnd, int iSBar,
                int iMinPos, int iMaxPos);

/**
 * \fn BOOL GUIAPI ShowScrollBar (HWND hWnd, int iSBar, BOOL bShow)
 * \brief Shows or hides the specified scroll bar.
 *
 * This function shows or hides the specified scroll bar.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param bShow Indicates whether show or hide the scrollbar.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa EnableScrollBar
 */
MG_EXPORT BOOL GUIAPI ShowScrollBar (HWND hWnd, int iSBar, BOOL bShow);

#define SIF_RANGE           0x0001
#define SIF_PAGE            0x0002
#define SIF_POS             0x0004
#define SIF_DISABLENOSCROLL 0x0008
#define SIF_TRACKPOS        0x0010
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)

/** left or up arrow */
#define SB_ARROW_LTUP   0x0001

/** right or bottom arrow */
#define SB_ARROW_BTDN   0x0002

/** left,up, right or bottom arrow */
#define SB_ARROW_BOTH  (SB_ARROW_LTUP | SB_ARROW_BTDN)

/**
 * Scroll bar information structure.
 */
typedef struct _SCROLLINFO
{
    /** Size of the structrue in bytes */
    UINT    cbSize;
    /**
     * A flag indicates which fields contain valid values,
     * can be OR'ed value of the following values:
     *      - SIF_RANGE\n
     *        Retrieve or sets the range of the scroll bar.
     *      - SIF_PAGE\n
     *        Retrieve or sets the page size of the scroll bar.
     *      - SIF_POS\n
     *        Retrieve or sets the position of the scroll bar.
     *      - SIF_DISABLENOSCROLL\n
     *        Hides the scroll when disabled, not implemented so far.
     */
    UINT    fMask;
    /** The minimum position value of the scroll bar */
    int     nMin;
    /** The maximum position value of the scroll bar */
    int     nMax;
    /** The page size of the scroll bar */
    UINT    nPage;
    /** The position value of the scroll bar */
    int     nPos;
#if 0
    int     nTrackPos;
#endif
} SCROLLINFO, *PSCROLLINFO;

/**
 * \fn BOOL GUIAPI SetScrollInfo (HWND hWnd, int iSBar, \
 *                const SCROLLINFO* lpsi, BOOL fRedraw)
 * \brief Set the parameters of a scroll bar.
 *
 * This function sets the parameters of a scroll bar, including the
 * minimum and maximum scrolling positions, the page size, and the position
 * of the scroll box (thumb). The function also redraws the scroll bar,
 * if requested.
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param lpsi The pointer to the structure of scroll bar information.
 * \param fRedraw A boolean indicates whether to redraw the scroll bar.
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetScrollInfo, SCROLLINFO
 */
MG_EXPORT BOOL GUIAPI SetScrollInfo (HWND hWnd, int iSBar,
                const SCROLLINFO* lpsi, BOOL fRedraw);

/**
 * \fn BOOL GUIAPI GetScrollInfo (HWND hWnd, int iSBar, PSCROLLINFO lpsi)
 * \brief Retrieve the parameters of a scroll bar.
 *
 * This function retrieves the parameters of a scroll bar, including the
 * minimum and maximum scrolling positions, the page size, and the position
 * of the scroll box (thumb).
 *
 * \param hWnd The handle to the window.
 * \param iSBar Indicates to enable or disable which scroll bar, can be one of
 *        the following values:
 *      - SB_HORZ\n
 *        The horizontal scroll bar.
 *      - SB_VERT\n
 *        The vertical scroll bar.
 * \param lpsi The pointer to a structure of SCROLLINFO which receives the
 *        parameters.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetScrollInfo, SCROLLINFO
 */
MG_EXPORT BOOL GUIAPI GetScrollInfo (HWND hWnd, int iSBar, PSCROLLINFO lpsi);

    /** @} end of scrollbar_fns */

    /**
     * \defgroup class_fns Window class operations
     * @{
     */

/* Class styles -- not supported so far */
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_KEYCVTWINDOW     0x0004
#define CS_DBLCLKS          0x0008

#define CS_OWNDC            0x0020
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080

#define CS_NOKEYCVT         0x0100
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800

#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000
#define CS_IME              0x8000

#define COP_STYLE           0x0001
#define COP_HCURSOR         0x0002
#define COP_BKCOLOR         0x0004
#define COP_WINPROC         0x0008
#define COP_ADDDATA         0x0010

/**
 * Structure defines a window class
 */
typedef struct _WNDCLASS {
    /** The class name */
    const char* spClassName;

    /** The mask of class information, can be OR'd with the following values:
      * - COP_STYLE\n
      *   Retrieve the style of the window class.
      * - COP_HCURSOR\n
      *   Retrieve the cursor of the window class.
      * - COP_BKCOLOR\n
      *   Retrieve the background pixel value of the window class.
      * - COP_WINPROC
      *   Retrieve the window procedure of the window class.
      * - COP_ADDDATA\n
      *   Retrieve the additional data of the window class.
      */
    DWORD   opMask;

    /** Window style for all instances of this window class */
    DWORD   dwStyle;

    /** Extended window style for all instances of this window class */
    DWORD   dwExStyle;

    /** Cursor handle to all instances of this window class */
    HCURSOR hCursor;

#ifndef _MGSCHEMA_COMPOSITING
    /**
     * The background pixel value for all instances of this window class.
     *
     * Note that this field only available for shared frame buffer schema.
     * Under compositing schema, you must use \a dwBkColor field to
     * specify the background color for a control.
     * This introduces a source code incompatibility, you should change
     * you code with a conditional compilation statement block:
     *
     * \code
     * #ifdef _MGSCHEMA_COMPOSITING
     *      MyClass.dwBkColor   = RGBA_lightwhite;
     * #else
     *      MyClass.iBkColor    = PIXEL_lightwhite;
     * #endif
     * \endcode
     */
    gal_pixel iBkColor;
#else   /* not defined _MGSCHEMA_COMPOSITING */
    /**
     * The background color for all instances of this window class.
     *
     * Note that under compositing schema, you must use this field
     * to specify the background color of a control class instead of
     * the pixel value (\a iBkColor).
     *
     * The value of this field is a 32-bit RGBA quadruple essentially.
     * You should use a value returned by \a MakeRGBA macro for this field.
     */
    DWORD dwBkColor;
#endif  /* defined _MGSCHEMA_COMPOSITING */

    /** Window callback procedure of all instances of this window class */
    LRESULT (*WinProc) (HWND, UINT, WPARAM, LPARAM);

    /** The private additional data associated with this window class */
    DWORD dwAddData;
} WNDCLASS;
typedef WNDCLASS* PWNDCLASS;

#define MAINWINCLASSNAME    ("MAINWINDOW")
#define VIRTWINCLASSNAME    ("VIRTWINDOW")
#define ROOTWINCLASSNAME    ("ROOTWINDOW")

/**
 * \fn BOOL GUIAPI RegisterWindowClass (PWNDCLASS pWndClass)
 * \brief Registers a window class.
 *
 * This function registers a window class.
 * Later on, you can create a window of the registered class.
 *
 * \param pWndClass The pointer to a WNDCLASS structure which specifies
 *        the information of the window class.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa UnregisterWindowClass, WNDCLASS
 *
 * Example:
 *
 * \include registerwindowclass.c
 */
MG_EXPORT BOOL GUIAPI RegisterWindowClass (PWNDCLASS pWndClass);

/**
 * \fn BOOL GUIAPI UnregisterWindowClass (const char* szClassName)
 * \brief Undoes the effect of \a RegisterWindowClass.
 *
 * This function unregisters a registered window class specified by
 * \a szClassName.
 *
 * \param szClassName The name of the class to be unregistered.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa RegisterWindowClass
 */
MG_EXPORT BOOL GUIAPI UnregisterWindowClass (const char* szClassName);

/**
 * \fn const char* GUIAPI GetClassName (HWND hWnd)
 * \brief Retrieve the name of the class to which the specified window belongs.
 *
 * This function retrieves the name of the class to which
 * the specified window \a hWnd belongs.
 *
 * \param hWnd The handle to the window.
 *
 * \return The pointer to a const class name string, NULL on error.
 *
 * \sa RegisterWindowClass
 */
MG_EXPORT const char* GUIAPI GetClassName (HWND hWnd);

/**
 * \fn BOOL GUIAPI GetWindowClassInfo (PWNDCLASS pWndClass)
 * \brief Retrieve the information of the specified window class.
 *
 * This function retrieves the information of a window class.
 * The window class to be retrived is specified by \a pWndClass->spClassName.
 *
 * \param pWndClass The pointer to a WNDCLASS structure, which specifies the
 *        window class to be retrived via \a spClassName field, and
 *        returns the information through other fields.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetWindowClassInfo
 */
MG_EXPORT BOOL GUIAPI GetWindowClassInfo (PWNDCLASS pWndClass);

/**
 * \fn BOOL GUIAPI SetWindowClassInfo (const WNDCLASS* pWndClass)
 * \brief Set the information of the specified window class.
 *
 * This function sets the information of a window class.
 * The window class to be operated is specified by \a pWndClass->spClassName.
 *
 * \param pWndClass The pointer to a WNDCLASS structure, which specifies
 *        the new information of the window class.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa GetWindowClassInfo
 */
MG_EXPORT BOOL GUIAPI SetWindowClassInfo (const WNDCLASS* pWndClass);

    /** @} end of class_fns */

    /**
     * \defgroup control_fns Control creating/destroying
     * @{
     */

/**
 * \fn HWND GUIAPI CreateWindowEx2 (const char* spClassName, \
 *               const char* spCaption, DWORD dwStyle, DWORD dwExStyle, \
 *               LINT id, int x, int y, int w, int h, HWND hParentWnd, \
 *               const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs, \
 *               DWORD dwAddData)
 * \brief Creates a child window with extended style, renderer and
 * attributes table.
 *
 * This function creates a child window (also known as "control") with
 * extended style, renderer and attributes table. It specifies the window
 * class, the window title, the window style, the window extended style,
 *  the initial position, and the size of the window, etc.  The function also
 * specifies the window's parent or owner.
 *
 * \param spClassName The class name of the control.
 * \param spCaption The caption of the control.
 * \param dwStyle The control style.
 * \param dwExStyle The extended control style.
 * \param id The identifier of the control.
 * \param x x,y: The initial position of the control in the parent window.
 * \param y x,y: The initial position of the control in the parent window.
 * \param w The initial width of the control.
 * \param h The initial height of the control.
 * \param hParentWnd The handle to the parent window.
 * \param werdr_name The window renderer name. NULL for default renderer.
 * \param we_attrs The pointer to window element attribute table.
 *                 NULL for default attribute table.
 * \param dwAddData The first private additional data of the control.
 *        Note that some control classes use this value to initialize
 *        some properties of the new control instance. For these control
 *        classes, you should pass a valid value to it.
 *
 * \return The handle to the new control, HWND_INVALID on error.
 *
 * \sa CreateWindowEx, CreateMainWindow, CTRLDATA
 */
MG_EXPORT HWND GUIAPI CreateWindowEx2 (const char* spClassName,
        const char* spCaption, DWORD dwStyle, DWORD dwExStyle,
        LINT id, int x, int y, int w, int h, HWND hParentWnd,
        const char* werdr_name, const WINDOW_ELEMENT_ATTR* we_attrs,
        DWORD dwAddData);

/**
 * \fn HWND GUIAPI CreateWindowEx (const char* spClassName,
                 const char* spCaption, DWORD dwStyle, DWORD dwExStyle,
                 LINT id, int x, int y, int w, int h, HWND hParentWnd,
                 DWORD dwAddData)
 * \brief A shortcut version of CreateWindowEx2.
 *
 * This function creates a child window by calling CreateWindowEx2 function
 * and passing NULL for both werdr_name and we_attrs parameters.
 *
 * \sa CreateWindowEx2
 */
static inline HWND GUIAPI CreateWindowEx (const char* spClassName,
                const char* spCaption, DWORD dwStyle, DWORD dwExStyle,
                LINT id, int x, int y, int w, int h, HWND hParentWnd,
                DWORD dwAddData)
{
    return CreateWindowEx2 (spClassName, spCaption, dwStyle, dwExStyle,
                id, x, y, w, h, hParentWnd, NULL, NULL, dwAddData);
}

/**
 * \fn BOOL GUIAPI DestroyWindow (HWND hWnd)
 * \brief Destroys a specified control.
 *
 * This function destroys the specified control \a hWnd, which is created
 * by \a CreateWindowEx or CreateWindowEx2.
 *
 * \param hWnd The handle to the control.
 * \return TRUE on success, FALSE on error.
 *
 * \sa CreateWindowEx
 */
MG_EXPORT BOOL GUIAPI DestroyWindow (HWND hWnd);

/**
 * \fn NOTIFPROC GUIAPI SetNotificationCallback (HWND hwnd,
 *          NOTIFPROC notif_proc)
 * \brief Set a new notification callback procedure for a window.
 *
 * This function sets the new notification callback procedure (\a notif_proc)
 * for the specified window \a hwnd.
 *
 * In the early versions, the notification message will be sent to
 * the target window as a MSG_COMMAND message.
 *
 * Since version 1.2.6, MiniGUI defines the Notification Callback Procedure
 * for a window. You can specify a callback function for a window by calling
 * \a SetNotificationCallback to receive and handle the notifications from
 * its children in the procedure.
 *
 * Since version 5.0.0, the notification callback procedure also works for
 * a main window or a virtual window.
 *
 * If you did not set the notification callback function of the target window,
 * you must handle MSG_COMMAND message in the window procedure of the
 * target window. However, due to historical reasons, MSG_COMMAND has
 * the following restrictions:
 *  - The additional data you specified when calling \a NotifyWindow or
 *    \a NotifyParentEx will be lost.
 *  - The value of the identifier and the notification code cannot exceed
 *    a WORD.
 *
 * \param hwnd The handle to the window.
 * \param notif_proc The new notification callback procedure, can be NULL.
 *
 * \return The old notification callback procedure.
 *
 * \sa NOTIFPROC, GetNotificationCallback, NotifyWindow, NotifyParentEx
 */
MG_EXPORT NOTIFPROC GUIAPI SetNotificationCallback (HWND hwnd,
                NOTIFPROC notif_proc);

/**
 * \fn NOTIFPROC GUIAPI GetNotificationCallback (HWND hwnd)
 * \brief Get the notification callback procedure of a control.
 *
 * This function gets the new notification callback procedure of
 * the control of \a hwnd.
 *
 * \param hwnd The handle to the control.
 *
 * \return The notification callback procedure.
 *
 * \sa NOTIFPROC, SetNotificationCallback
 */
MG_EXPORT NOTIFPROC GUIAPI GetNotificationCallback (HWND hwnd);

/**
 * \def CreateWindow(class_name, caption, style, id, x, y, w, h, parent,
 *      add_data)
 * \brief A simplified version of \a CreateWindowEx.
 *
 * \sa CreateWindowEx
 */
static inline HWND GUIAPI CreateWindow (const char* spClassName,
                const char* spCaption, DWORD dwStyle,
                LINT id, int x, int y, int w, int h, HWND hParentWnd,
                DWORD dwAddData)
{
    return CreateWindowEx2 (spClassName, spCaption, dwStyle, WS_EX_NONE,
                id, x, y, w, h, hParentWnd, NULL, NULL, dwAddData);
}

    /** @} end of control_fns */

/******************************** Timer Support ******************************/
    /**
     * \defgroup timer_fns Timer operations
     * @{
     */

/**
 * \var typedef BOOL (* TIMERPROC)(HWND, LINT, DWORD)
 * \brief Type of the timer callback procedure.
 *
 * This is the prototype of the callback procedure of a timer created by
 * \a SetTimerEx.
 * MiniGUI will call the timer procedure instead of sending MSG_TIMER message.
 *
 * If the return value of a timer procedure is FALSE, the timer will be killed
 * by MiniGUI automatically. This can be used to implement a one-shot timer.
 *
 * \sa SetTimerEx
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one:
 *
 *      BOOL (* TIMERPROC)(HWND, int, unsigned int)
 */
typedef BOOL (* TIMERPROC)(HWND, LINT, DWORD);

/**
 * \fn BOOL GUIAPI SetTimerEx (HWND hWnd, LINT id, DWORD speed, \
 *              TIMERPROC timer_proc)
 * \brief Creates a timer with the specified timeout value.
 *
 * This function creates a timer with the specified timeout value \a speed.
 * Note that the timeout value is in the unit of 10 ms.
 * When the timer expires, an MSG_TIMER message will be send to the
 * window \a hWnd if \a timer_proc is NULL, otherwise MiniGUI will call
 * \a timer_proc by passing \a hWnd, \a id, and the tick count when this
 * timer had expired to this callback procedure.
 *
 * Since 5.0.0, if the specified timer already exists when you call
 * this function, MiniGUI will reset the timer by using the new parameters.
 *
 * \param hWnd The window receives the MSG_TIMER message. If \a timer_proc
 *        is not NULL, MiniGUI will call \a timer_proc instead sending
 *        MSG_TIMER message to this window. If you use timer callback
 *        procedure, \a hWnd can be any value you can pass.
 * \param id The identifier of the timer, will be passed to the window
 *        with MSG_TIMER message as the first parameter of the message.
 * \param speed The timeout value of the timer. Note that the timeout value
 *        is in unit of 10 ms.
 * \param timer_proc The timer callback procedure. If this argument is NULL,
 *        MiniGUI will send MSG_TIMER to the window procedure of \a hWnd.
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetTimer, ResetTimerEx, KillTimer, MSG_TIMER
 *
 * \note You should set, reset, and kill a timer in the same message thread
 *      if your enabled support for the virtual window.
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one:
 *
 *      BOOL SetTimerEx (HWND hWnd, int id, unsigned int speed, \
 *              TIMERPROC timer_proc);
 *
 * Example:
 *
 * \include settimer.c
 */
MG_EXPORT BOOL GUIAPI SetTimerEx (HWND hWnd, LINT id, DWORD speed,
                TIMERPROC timer_proc);

/**
 * \def SetTimer(hwnd, id, speed)
 * \brief The backward compatibility version of SetTimerEx.
 *
 * \sa SetTimerEx
 */
#define SetTimer(hwnd, id, speed) \
                SetTimerEx(hwnd, id, speed, NULL)

/**
 * \fn int GUIAPI KillTimer (HWND hWnd, LINT id)
 * \brief Destroys a timer.
 *
 * This function destroys the specified timer \a id.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer. If \a id is 0, this function will
 *      kill all timers of created by the window.
 *
 * \return The number of actually killed timer.
 *
 * \sa SetTimer
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one:
 *      int GUIAPI KillTimer (HWND hWnd, int id)
 */
MG_EXPORT int GUIAPI KillTimer (HWND hWnd, LINT id);

/**
 * \fn BOOL GUIAPI ResetTimerEx (HWND hWnd, LINT id, DWORD speed, \
 *              TIMERPROC timer_proc)
 * \brief Adjusts a timer with a different timeout value or different
 *        timer callback procedure.
 *
 * This function resets a timer with the specified timeout \a speed value.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer.
 * \param speed The new timeout value.
 * \param timer_proc The new timer callback procedure. If \a timer_proc
 *        is INV_PTR, the setting of timer callback procedure will
 *        not change.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa SetTimerEx
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one:
 *      BOOL GUIAPI ResetTimerEx (HWND hWnd, int id, unsigned int speed, TIMERPROC timer_proc)
 */
MG_EXPORT BOOL GUIAPI ResetTimerEx (HWND hWnd, LINT id, DWORD speed,
                TIMERPROC timer_proc);

/**
 * \def ResetTimer(hwnd, id, speed)
 * \brief The backward compatibility version of ResetTimerEx.
 *
 * \sa ResetTimerEx
 */
#define ResetTimer(hwnd, id, speed) \
                ResetTimerEx(hwnd, id, speed, (TIMERPROC)INV_PTR)

/**
 * \fn BOOL GUIAPI IsTimerInstalled (HWND hWnd, LINT id)
 * \brief Determine whether a timer is installed.
 *
 * This function determines whether a timer with identifier \a id for
 * a window \a hwnd has been installed in the current thread.
 *
 * \param hWnd The window owns the timer.
 * \param id The identifier of the timer.
 *
 * \return TRUE for installed, otherwise FALSE.
 *
 * \sa SetTimer, HaveFreeTimer
 *
 * \note The prototype had changed since MiniGUI v3.2; the old one:
 *      BOOL GUIAPI IsTimerInstalled (HWND hWnd, int id)
 */
MG_EXPORT BOOL GUIAPI IsTimerInstalled (HWND hWnd, LINT id);

/**
 * \fn BOOL GUIAPI HaveFreeTimer (void)
 * \brief Determine whether there is any free timer slot in the current
 *      thread.
 *
 * This function determines whether there is any free timer slot in the
 * current thread.
 *
 * \return TRUE for yes, otherwise FALSE.
 *
 * \sa IsTimerInstalled
 */
MG_EXPORT BOOL GUIAPI HaveFreeTimer (void);

    /** @} end of timer_fns */

    /**
     * \defgroup ime_fns IME Window functions
     * @{
     */

/**
 *  \ struct defines a ime target info
 * */
typedef struct _IME_TARGET_INFO
{
   /** The type of the edit box:
     * - IME_WINDOW_TYPE_READONLY\n
     *   The edit box is readonly.
     *
     * - IME_WINDOW_TYPE_PASSWORD\n
     *   The edit box is readonly.
     *
     * - IME_WINDOW_TYPE_NOT_EDITABLE\n
     *   The edit box is not editable.
     *
     */
    int     iEditBoxType;

    /* The position of the caret */
    POINT   ptCaret;

    /* The rect of the edit box */
    RECT    rcEditBox;
} IME_TARGET_INFO;

/**
 * \fn int GUIAPI RegisterIMEWindow (HWND hWnd)
 * \brief Registers an IME window.
 *
 * This function registers the specified window \a hWnd as the IME window of
 * the MiniGUI. After that, the keyboard input will be sent to IME window
 * first. Note that only one IME window can be registered.
 *
 * \param hWnd The handle to your IME window.
 *
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_TOOMUCHIMEWND Already have an IME window registered.
 * \retval ERR_INV_HWND Invalid main window handle.
 *
 * \sa UnregisterIMEWindow
 */
MG_EXPORT int GUIAPI RegisterIMEWindow (HWND hWnd);

/**
 * \fn int GUIAPI UnregisterIMEWindow (HWND hWnd)
 * \brief Unregisters an IME window.
 *
 * This function undoes the effect of \a RegisterIMEWindow.
 *
 * \param hWnd The handle to the current IME window.
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOSUCHIMEWND The window is not the current IME window.
 *
 * \sa RegisterIMEWindow
 */
MG_EXPORT int GUIAPI UnregisterIMEWindow (HWND hWnd);

/**
 * \fn int GUIAPI GetIMEStatus (int StatusCode)
 * \brief Retrieve status of the current IME window.
 *
 * This function retrieves status of the current IME window.
 *
 * \param StatusCode The item to be retrived, can be one of the following
 * values:
 *      - IME_STATUS_REGISTERED\n
 *        Is there any registered IME window?
 *      - IME_STATUS_ENABLED\n
 *        Is the IME window enabled?
 *      - IME_STATUS_AUTOTRACK\n
 *        Does the IME window autotrack the input focus?
 *      - IME_STATUS_LANGUAGE\n
 *        Retrieve the selected input method. The return value can be
 *        one of the following values:
 *          - IME_LANGUAGE_UNKNOWN\n
 *            Unknown
 *          - IME_LANGUAGE_LATIN\n
 *            Latin
 *          - IME_LANGUAGE_ZHCN\n
 *            Chinese simplified
 *          - IME_LANGUAGE_ZHTW\n
 *            Chinese traditional
 *      - IME_STATUS_ENCODING\n
 *        Retrieve the output encoding. The return value can be one of
 *        the following values:
 *          - IME_ENCODING_LOCAL\n
 *            The local encoding
 *          - IME_ENCODING_UTF8\n
 *            UTF-8
 *      - IME_STATUS_VERSION\n
 *        Retrieve the version of the IME
 *      - IME_STATUS_USER_MIN, IME_STATUS_USER_MAX
 *        Reserved for user. The return value and its meaning is determined
 *        by the specific IME implementation.
 *
 * \return The status of the item specified by \a StatusCode,
 *        ERR_IME_NOIMEWND if error occurred.
 *
 * \sa SetIMEStatus
 */
MG_EXPORT int GUIAPI GetIMEStatus (int StatusCode);

/**
 * \fn int GUIAPI SetIMEStatus (int StatusCode, int Value)
 * \brief Set the status of the current IME window.
 *
 * This function sets the status of the current IME window.
 *
 * \param StatusCode The item to be set. Please see GetIMEStatus for more
 * information.
 * \param Value The status value of the item.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOIMEWND There is no any IME window registered.
 *
 * \sa GetIMEStatus
 */
MG_EXPORT int GUIAPI SetIMEStatus (int StatusCode, int Value);

/**
 * \fn int GUIAPI GetIMETargetInfo (IME_TARGET_INFO* info)
 * \brief Retrieve the target info of the current IME window.
 *
 * This function retrieves the target info of the current IME window.
 *
 * \param info The item to be retrived. The target info is
 *        return by the current IME Window.
 *
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \sa GetIMETargetInfo
 */
MG_EXPORT int GUIAPI GetIMETargetInfo (IME_TARGET_INFO *info);

/**
 * \fn int GUIAPI SetIMETargetInfo (const IME_TARGET_INFO *info)
 * \brief Set the target info of the current IME window.
 *
 * This function sets the target info of the current IME window.
 *
 * \param info The set target info.
 *
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOIMEWND There is no any IME window registered.
 *
 * \sa SetIMETargetInfo
 */
MG_EXPORT int GUIAPI SetIMETargetInfo (const IME_TARGET_INFO *info);

/**
 * \fn int GUIAPI GetIMEPos (POINT* pt)
 * \brief Retrieve the position of the current IME window.
 *
 * NOTE that this function is deprecated.
 *
 * This function retrieves the position of the current IME window.
 *
 * \param pt The item to be retrived. The positon is
 *        return by the current IME Window.
 *
 * \return The current IME window positon.
 *        ERR_IME_NOIMEWND if error occurred.
 *
 * \sa GetIMEPos
 */
MG_EXPORT int GUIAPI GetIMEPos (POINT* pt);

/**
 * \fn int GUIAPI SetIMEPos (POINT* pt)
 * \brief Set the position of the current IME window.
 *
 * NOTE that this function is deprecated.
 *
 * This function sets the position of the current IME window.
 *
 * \param pt The set position value.
 *
 * \return ERR_OK on success, otherwise less than zero.
 *
 * \retval ERR_OK Success.
 * \retval ERR_IME_NOIMEWND There is no any IME window registered.
 *
 * \sa SetIMEPos
 */
MG_EXPORT int GUIAPI SetIMEPos (const POINT* pt);

    /** @} end of ime_fns */

    /**
     * \defgroup accel_fns Accelerator operations
     * @{
     */

/**
 * \fn HACCEL GUIAPI CopyAcceleratorTable (HACCEL hacc)
 * \brief Copies the specified accelerator table.
 *
 * This function copies the specified accelerator table.
 * The function is used to obtain the accelerator table data that
 * corresponds to an accelerator table handle.
 *
 * \param hacc The handle to the accelerator table.
 * \return The handle to the copied new accelerator table.
 */
MG_EXPORT HACCEL GUIAPI CopyAcceleratorTable (HACCEL hacc);

/**
 * \fn int GUIAPI DeleteAccelerators (HACCEL hacc, int key, DWORD keymask)
 * \brief Deletes an accelerator from the accelerator table.
 *
 * This function deletes an accelerator from the accelerator table.
 *
 * \param hacc The handle to the accelerator table.
 * \param key The key value of the accelerator.
 * \param keymask The shift key state.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa AddAccelerators, DestroyAcceleratorTable
 */
MG_EXPORT int GUIAPI DeleteAccelerators (HACCEL hacc, int key, DWORD keymask);

#define ACCEL_SHIFT     KS_SHIFT
#define ACCEL_ALT       KS_ALT
#define ACCEL_CTRL      KS_CTRL

/**
 * \fn int GUIAPI AddAccelerators (HACCEL hacc, int key, \
 *               DWORD keymask, WPARAM wParam, LPARAM lParam)
 * \brief Adds an accelerator to the accelerator table.
 *
 * This function adds an accelerator to the accelerator table.
 * It's called after you call \a CreateAcceleratorTable function.
 *
 * \param hacc The handle to the accelerator table.
 * \param key The key value of the accelerator. Note that you can define the
 *        key value by either scancode or ASCII code. If you define the key
 *        with scancode, the key value should be equal to (scancode + 256).
 * \param keymask The shift key state, can be OR'ed value of the following
 *        values:
 *      - KS_SHIFT\n
 *        Shift key should be pressed.
 *      - KS_ALT\n
 *        Alt key should be pressed.
 *      - KS_CTRL\n
 *        Ctrl key should be pressed.
 * \param wParam The first parameter of the MSG_COMMAND when receiving such
 *        a accelerator key.
 * \param lParam The second parameter of the MSG_COMMAND when receiving such
 *        a accelerator key.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa CreateAcceleratorTable, DeleteAccelerators
 */
MG_EXPORT int GUIAPI AddAccelerators (HACCEL hacc, int key,
                DWORD keymask, WPARAM wParam, LPARAM lParam);

/**
 * \fn int GUIAPI DestroyAcceleratorTable (HACCEL hacc)
 * \brief Destroys an accelerator table.
 *
 * This function destroys the specified accelerator table \a hacc.
 * Before closing a window, you must call this function to destroy
 * each accelerator table that is created by using \a CreateAcceleratorTable
 * function.
 *
 * \param hacc The handle to the accelerator table.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa CreateAcceleratorTable
 */
MG_EXPORT int GUIAPI DestroyAcceleratorTable (HACCEL hacc);

/**
 * \fn HACCEL GUIAPI CreateAcceleratorTable (HWND hWnd)
 * \brief Creates an empty accelerator table.
 *
 * This function creates an empty accelerator table.
 *
 * \param hWnd The handle to the main window.
 * \return The handle to the new accelerator table, zero when error.
 *
 * \sa AddAccelerators, DestroyAcceleratorTable
 */
MG_EXPORT HACCEL GUIAPI CreateAcceleratorTable (HWND hWnd);

/**
 * \fn int GUIAPI TranslateAccelerator (HACCEL hAccel, PMSG pMsg)
 * \brief Translates an accelerator key message to MSG_COMMAND messge
 * and sends it to the window procedure.
 *
 * \param hAccel The handle to the accelerator table.
 * \param pMsg The pointer to the MSG structure to be translated.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TranslateMessage
 */
MG_EXPORT int GUIAPI TranslateAccelerator (HACCEL hAccel, PMSG pMsg);

    /** @} end of accel_fns */

    /**
     * \defgroup caret_fns Caret operations
     * @{
     */

/**
 * \fn BOOL GUIAPI CreateCaret (HWND hWnd, PBITMAP pBitmap, \
 *               int nWidth, int nHeight)
 * \brief Creates a new shape for the system caret and assigns ownership of
 * the caret to the specified window.
 *
 * This function creates a new shape for the system caret and assigns
 * ownership of the caret to the specified window \a hWnd. The caret shape
 * can be a line, a block, or a bitmap.
 *
 * \param hWnd The owner of the caret.
 * \param pBitmap The bitmap shape of the caret. It can be NULL.
 * \param nWidth The width of the caret.
 * \param nHeight The height of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ActiveCaret, ChangeCaretSize
 */
MG_EXPORT BOOL GUIAPI CreateCaret (HWND hWnd, PBITMAP pBitmap,
                int nWidth, int nHeight);

/**
 * \fn BOOL GUIAPI ChangeCaretSize (HWND hWnd, int newWidth, int newHeight)
 * \brief Changes the size of the caret.
 *
 * This function change the size of the caret owned by the specified window
 * \a hWnd. Note that the new size of the caret should be smaller than
 * the original size which is specified when you call \a CreateCaret.
 *
 * \param hWnd The owner of the caret.
 * \param newWidth The new width of the caret.
 * \param newHeight The new height of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateCaret
 */
MG_EXPORT BOOL GUIAPI ChangeCaretSize (HWND hWnd, int newWidth, int newHeight);

/**
 * \fn BOOL GUIAPI ActiveCaret (HWND hWnd)
 * \brief Activates the caret owned by a window.
 *
 * This function activates the caret owned by the specified window \a hWnd.
 * After activating the caret, the caret will begin flashing automatically.
 *
 * \param hWnd The owner of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa HideCaret, ShowCaret
 */
MG_EXPORT BOOL GUIAPI ActiveCaret (HWND hWnd);

/**
 * \fn UINT GUIAPI GetCaretBlinkTime (HWND hWnd)
 * \brief Returns the elapsed time, in milliseconds, required to invert the
 *        caret's pixels.
 *
 * This function returns the elapsed time, in milliseconds, required to
 * invert the caret's pixels. The default blink time of a caret is 500
 * milliseconds.
 *
 * \param hWnd The owner of the caret.
 *
 * \return The elapsed time in milliseconds. Returns zero on error.
 *
 * \sa SetCaretBlinkTime
 */
MG_EXPORT UINT GUIAPI GetCaretBlinkTime (HWND hWnd);

/**
 * \fn BOOL GUIAPI SetCaretBlinkTime (HWND hWnd, UINT uTime)
 * \brief Set the caret blink time to the specified number of milliseconds.
 *
 * This function sets the caret blink time to the specified number of
 * milliseconds. The blink time is the elapsed time, in milliseconds,
 * required to invert the caret's pixels. The default blink time of a caret
 * is 500 milliseconds.
 *
 * \param hWnd The owner of the caret.
 * \param uTime New blink time in milliseconds.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetCaretBlinkTime
 */
MG_EXPORT BOOL GUIAPI SetCaretBlinkTime (HWND hWnd, UINT uTime);

/**
 * \fn BOOL GUIAPI DestroyCaret (HWND hWnd)
 * \brief Destroys a caret.
 *
 * This function destroys the caret's current shape,
 * frees the caret from the window, and removes the caret from the screen.
 *
 * \param hWnd The owner of the caret.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateCaret
 */
MG_EXPORT BOOL GUIAPI DestroyCaret (HWND hWnd);

/**
 * \fn BOOL GUIAPI HideCaretEx (HWND hWnd, BOOl ime)
 * \brief Hides a caret.
 *
 * This function removes the caret from the screen.
 * Hiding a caret does not destroy its current shape or invalidate the
 * insertion point.
 *
 * \param hWnd The owner of the caret.
 *
 * \param ime TRUE if you want to close the IME window (if there is any)
 * at the same time.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ShowCaretEx
 */
MG_EXPORT BOOL GUIAPI HideCaretEx (HWND hWnd, BOOL ime);

/**
 * \fn BOOL GUIAPI HideCaret (HWND hWnd)
 * \brief Hides a caret.
 *
 * This function removes the caret from the screen and close the IME window.
 * Hiding a caret does not destroy its current shape or invalidate the
 * insertion point.
 *
 * \param hWnd The owner of the caret.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ShowCaret
 */
static inline BOOL GUIAPI HideCaret (HWND hWnd)
{
    return HideCaretEx (hWnd, TRUE);
}

/**
 * \fn BOOL GUIAPI ShowCaretEx (HWND hWnd, BOOL ime)
 * \brief Shows a caret.
 *
 * This function makes the caret visible on the screen at the caret's
 * current position. When the caret becomes visible, it begins flashing
 * automatically.
 *
 * \param hWnd The owner of the caret.
 *
 * \param ime TRUE if you want to let the hidden IME window (if there is any)
 * show up at the same time.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa HideCaretEx
 */
MG_EXPORT BOOL GUIAPI ShowCaretEx (HWND hWnd, BOOL ime);

/**
 * \fn BOOL GUIAPI ShowCaret (HWND hWnd)
 * \brief Shows a caret.
 *
 * This function makes the caret visible on the screen at the caret's
 * current position. When the caret becomes visible, it begins flashing
 * automatically.
 *
 * \param hWnd The owner of the caret.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa HideCaret
 */
static inline BOOL GUIAPI ShowCaret (HWND hWnd)
{
    return ShowCaretEx (hWnd, TRUE);
}

/**
 * \fn BOOL GUIAPI SetCaretPos (HWND hWnd, int x, int y)
 * \brief Moves the caret to a new position.
 *
 * This function moves the caret to the specified coordinates \a (x,y).
 *
 * \param hWnd The owner of the caret.
 * \param x x,y: The position of the caret in client coordiantes of
 *        the owner window.
 * \param y x,y: The position of the caret in client coordiantes of
 *        the owner window.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetCaretPos
 */
MG_EXPORT BOOL GUIAPI SetCaretPos (HWND hWnd, int x, int y);

/**
 * \fn BOOL GUIAPI GetCaretPos (HWND hWnd, PPOINT pPt)
 * \brief Get the caret position.
 *
 * This function copies the caret's position, in client coordinates,
 * to the specified POINT structure \a pPt.
 *
 * \param hWnd The owner of the caret.
 * \param pPt The pointer to the buffer saves the caret's position.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetCaretPos
 */
MG_EXPORT BOOL GUIAPI GetCaretPos (HWND hWnd, PPOINT pPt);

    /** @} end of caret_fns */

    /**
     * \defgroup menu_fns Menu operations
     * @{
     */

/* Menu flags */
#define MF_INSERT           0x00000000L
#define MF_CHANGE           0x00000080L
#define MF_APPEND           0x00000100L
#define MF_DELETE           0x00000200L
#define MF_REMOVE           0x00001000L

#define MF_BYCOMMAND        0x00000000L
#define MF_BYPOSITION       0x00000400L

#define MF_SEPARATOR        0x00000800L

#define MF_ENABLED          0x00000000L
#define MF_GRAYED           0x00000001L
#define MF_DISABLED         0x00000002L

#define MF_UNCHECKED        0x00000000L
#define MF_CHECKED          0x00000008L
#define MF_USECHECKBITMAPS  0x00000200L

#define MF_STRING           0x00000000L
#define MF_BITMAP           0x00000004L
#define MF_OWNERDRAW        0x00000100L

#define MF_POPUP            0x00000010L
#define MF_MENUBARBREAK     0x00000020L
#define MF_MENUBREAK        0x00000040L

#define MF_UNHILITE         0x00000000L
#define MF_HILITE           0x00000080L

#define MF_DEFAULT          0x00001000L
#define MF_SYSMENU          0x00002000L
#define MF_HELP             0x00004000L
#define MF_RIGHTJUSTIFY     0x00004000L

#define MF_MOUSESELECT      0x00008000L
#define MF_END              0x00000080L

#define MFT_STRING          MF_STRING /* 0x00000000L */
#define MFT_BITMAP          MF_BITMAP /* 0x00000004L */
#define MFT_BMPSTRING       0x00010000L
#define MFT_MENUBARBREAK    MF_MENUBARBREAK /* 0x00000020L */
#define MFT_MENUBREAK       MF_MENUBREAK /* 0x00000040L */
#define MFT_OWNERDRAW       MF_OWNERDRAW /* 0x00000100L */
#define MFT_RADIOCHECK      0x00000200L
#define MFT_MARKCHECK       0x00000400L
#define MFT_SEPARATOR       MF_SEPARATOR /* 0x00000800L */

#define MFT_RIGHTORDER      0x00002000L
#define MFT_RIGHTJUSTIFY    MF_RIGHTJUSTIFY

#define MFS_GRAYED          0x00000003L
#define MFS_DISABLED        MFS_GRAYED
#define MFS_CHECKED         MF_CHECKED
#define MFS_HILITE          MF_HILITE
#define MFS_ENABLED         MF_ENABLED
#define MFS_UNCHECKED       MF_UNCHECKED
#define MFS_UNHILITE        MF_UNHILITE
#define MFS_DEFAULT         MF_DEFAULT

/* System Menu Command Values */
  #define SC_SIZE         0xF000
  #define SC_MOVE         0xF010
  #define SC_MINIMIZE     0xF020
  #define SC_MAXIMIZE     0xF030
  #define SC_NEXTWINDOW   0xF040
  #define SC_PREVWINDOW   0xF050
  #define SC_CLOSE        0xF060
  #define SC_VSCROLL      0xF070
  #define SC_HSCROLL      0xF080
  #define SC_MOUSEMENU    0xF090
  #define SC_KEYMENU      0xF100
  #define SC_ARRANGE      0xF110
  #define SC_RESTORE      0xF120
  #define SC_TASKLIST     0xF130
  #define SC_SCREENSAVE   0xF140
  #define SC_HOTKEY       0xF150

  #define SC_DEFAULT      0xF160
  #define SC_MONITORPOWER 0xF170
  #define SC_CONTEXTHELP  0xF180
  #define SC_SEPARATOR    0xF00F

/* MENUITEMINFO flags */
  #define MIIM_STATE       0x00000001
  #define MIIM_ID          0x00000002
  #define MIIM_SUBMENU     0x00000004
  #define MIIM_CHECKMARKS  0x00000008
  #define MIIM_TYPE        0x00000010
  #define MIIM_DATA        0x00000020
  #define MIIM_STRING      0x00000040
  #define MIIM_BITMAP      0x00000080
  #define MIIM_FTYPE       0x00000100


/* Flags for TrackPopupMenu */
  #define TPM_LEFTBUTTON  0x0000L
  #define TPM_RIGHTBUTTON 0x0002L
  #define TPM_LEFTALIGN   0x0000L
  #define TPM_CENTERALIGN 0x0004L
  #define TPM_RIGHTALIGN  0x0008L
  #define TPM_TOPALIGN        0x0000L
  #define TPM_VCENTERALIGN    0x0010L
  #define TPM_BOTTOMALIGN     0x0020L

  #define TPM_HORIZONTAL      0x0000L     /* Horz alignment matters more */
  #define TPM_VERTICAL        0x0040L     /* Vert alignment matters more */
  #define TPM_NONOTIFY        0x0080L     /* Don't send any notification msgs */
  #define TPM_RETURNCMD       0x0100L

  #define TPM_DESTROY         0x1000L       /* Destroy menu after tracking */
  #define TPM_SYSCMD          0x2000L       /* Send system command */
  #define TPM_DEFAULT         0x0000L       /* Default tracking flag */

/* return codes for MSG_MENUCHAR */
#define MNC_IGNORE  0
#define MNC_CLOSE   1
#define MNC_EXECUTE 2
#define MNC_SELECT  3

/**
 * Structure defines a menu item
 */
typedef struct _MENUITEMINFO {

    /**
     * Used by \a GetMenuItemInfo and \a SetMenuItemInfo functions,
     * can be OR'ed with the following values:
     *      - MIIM_STATE\n
     *        Get/set the state of the menu item.
     *      - MIIM_ID\n
     *        Get/set the identifier of the menu item.
     *      - MIIM_SUBMENU\n
     *        Get/set the sub-menu of the menu item.
     *      - MIIM_CHECKMARKS\n
     *        Get/set the check/uncheck bitmap.
     *      - MIIM_TYPE\n
     *        Get/set the type and the type data of the menu item.
     *      - MIIM_DATA\n
     *        Get/set the item data of the menu item.
     */
    UINT                mask;

    /**
     * Type of menu item, can be one of the following values:
     *      - MFT_STRING\n
     *        A normal string menu item.
     *
     *      - MFT_BITMAP\n
     *        A bitmap menu item. When the menu item is this type,
     *        the member \a typedata will be the pointer to the normal BITMAP object,
     *        the member \a uncheckedbmp will be the pointer to the hilighted BITMAP object,
     *        the member \a checkedbmp will be the pointer to the checked BITMAP object.
     *
     *      - MFT_BMPSTRING\n
     *        A bitmap menu item followed by a string. When the menu item is this type,
     *        the member \a typedata will be the pointer to the text string,
     *        the member \a uncheckedbmp will be the pointer to the normal BITMAP object,
     *        and the member \a checkedbmp will be the pointer to the checked BITMAP object.
     *
     *      - MFT_SEPARATOR\n
     *        A separator in menu.
     *
     *      - MFT_RADIOCHECK\n
     *        A normal string with a radio check mark.
     *
     *      - MFT_MARKCHECK\n
     *        A normal string with a check box mark.
     */
    UINT                type;

    /**
     * State of the menu item, can be one of the following values:
     *      - MFS_GRAYED\n
     *        The menu item is disabled.
     *      - MFS_DISABLED\n
     *        The menu item is disabled.
     *      - MFS_CHECKED\n
     *        The menu item is checked.
     *      - MFS_ENABLED\n
     *        The menu item is enabled.
     *      - MFS_UNCHECKED\n
     *        The menu item is unchecked.
     */
    UINT                state;

    /** The identifier of the menu item.
      * \note The type changed from int to LINT since v3.2.
      */
    LINT                 id;

    /** The handle to the sub-menu if this menu contains a sub menu */
    HMENU               hsubmenu;

    /** The pointer to a BITMAP object used for unchecked/highlited bitmap menu item */
    PBITMAP             uncheckedbmp;

    /** The pointer to a BITMAP object used for checked bitmap menu item */
    PBITMAP             checkedbmp;

    /** The private data attached to the menu item */
    DWORD               itemdata;

    /** The data of this menu item, used to pass the string or the pointer to the BITMAP object of the menu item */
    DWORD               typedata;

    /**
     * Used by \a GetMenuItemInfo function to indicate the maximal length
     * of the string
     */
    UINT                cch;
} MENUITEMINFO;
typedef MENUITEMINFO* PMENUITEMINFO;

/**
 * \fn HMENU GUIAPI CreateMenu (void)
 * \brief Creates an empty menu.
 *
 * This function creates a menu. The menu is initially empty, but it can be
 * filled with menu items by using the \a InsertMenuItem functions.
 *
 * \return The handle to the menu, zero when error.
 *
 * \sa InsertMenuItem
 */
MG_EXPORT HMENU GUIAPI CreateMenu (void);

/**
 * \fn HMENU GUIAPI CreatePopupMenu ( PMENUITEMINFO pmii)
 * \brief Creates a drop-down menu or submenu.
 *
 * This function creates a drop-down menu or submenu. The menu is initially
 * empty. You can insert or append menu items by using the \a InsertMenuItem
 * function.
 *
 * \param pmii Menu item information used to create the popup menu.
 *
 * \return The handle to the popup menu.
 *
 * \sa InsertMenuItem, MENUITEMINFO
 */
MG_EXPORT HMENU GUIAPI CreatePopupMenu ( PMENUITEMINFO pmii);

/**
 * \fn HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle)
 * \brief Creates a system menu.
 *
 * This function creates a system menu for the main window specified by
 * \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param dwStyle The style of the main window.
 *
 * \return The handle to the system menu.
 *
 * \sa CreateMenu, CreatePopupMenu
 */
MG_EXPORT HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle);

/**
 * \fn int GUIAPI InsertMenuItem (HMENU hmnu, LINT item, \
 *                UINT flag, PMENUITEMINFO pmii)
 * \brief Inserts a new menu item at the specified position in a menu.
 *
 * This function inserts a new menu item specified by \a pmmi at the
 * specified position (determined by \a item and \a flag) in the menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 * \param item The insertion position.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *        The new menu item will insert bellow the item whose id is \a item.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii Menu item information used to create the popup menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_RES_ALLOCATION Can not allocate new menu item.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa RemoveMenu, MENUITEMINFO
 */
MG_EXPORT int GUIAPI InsertMenuItem (HMENU hmnu, LINT item,
                            UINT flag, PMENUITEMINFO pmii);

/**
 * \fn int GUIAPI RemoveMenu (HMENU hmnu, LINT item, UINT flag)
 * \brief Deletes a menu item or detaches a submenu from the specified menu.
 *
 * This function deletes a menu item or detaches a submenu from the specified
 * menu \a hmnu. If the item is a normal menu item, the function will delete
 * the item. If the item is a submenu, the function will detache the submenu
 * for the menu, but not delete the submenu.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa InsertMenuItem, DeleteMenu
 */
MG_EXPORT int GUIAPI RemoveMenu (HMENU hmnu, LINT item, UINT flag);

/**
 * \fn int GUIAPI DeleteMenu (HMENU hmnu, LINT item, UINT flag)
 * \brief Deletes an item from the specified menu.
 *
 * This function deletes an item from the specified menu \a hmnu.
 * Either the item is a normal menu item or a submenu, this function
 * will delete the item.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 * \retval ERR_INVALID_HMENU \a hmnu is an invalid menu.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa InsertMenuItem, RemoveMenu
 */
MG_EXPORT int GUIAPI DeleteMenu (HMENU hmnu, LINT item, UINT flag);

/**
 * \fn int GUIAPI DestroyMenu (HMENU hmnu)
 * \brief Destroys the specified menu and frees any memory that the menu
 * occupies.
 *
 * This function destroys the specified menu \a hmnu and frees any memory
 * that the menu occupies.
 *
 * \param hmnu The handle to the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \retval ERR_INVALID_HANDLE \a hmnu is not a handle to menu.
 *
 * \sa RemoveMenu, DeleteMenu
 */
MG_EXPORT int GUIAPI DestroyMenu (HMENU hmnu);

/**
 * \fn int GUIAPI IsMenu (HMENU hmnu)
 * \brief Determine whether a handle is a menu handle.
 *
 * This function determines whether the handle specified by \a hmnu is a
 * menu handle.
 *
 * \param hmnu The handle to a menu.
 *
 * \return The function returns 0 for none menu handle,
 *         otherwise the type of the menu.
 *
 * \sa CreateMenu
 */
MG_EXPORT int GUIAPI IsMenu (HMENU hmnu);

/**
 * \fn HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu)
 * \brief Assigns a new menu to the specified main window.
 *
 * This functionn assigns the specified menu \a hmnu to
 * the main window specified by \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param hmnu The handle to the new menu.
 *
 * \return The handle to the old menu of the window.
 *
 * \sa GetMenu
 */
MG_EXPORT HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu);

/**
 * \fn HMENU GUIAPI GetMenu (HWND hwnd)
 * \brief Retrieve the handle to the menu assigned to the given main window.
 *
 * This function retrieves the handle to the menu assigned to
 * the given main window \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \return The handle to the menu of the window.
 *
 * \sa SetMenu
 */
MG_EXPORT HMENU GUIAPI GetMenu (HWND hwnd);

/**
 * \fn void GUIAPI DrawMenuBar (HWND hwnd)
 * \brief Redraws the menu bar of the specified main window.
 *
 * This function redraws the menu bar of the specified main window.
 * If the menu bar changes after the system has created the window,
 * this function must be called to draw the changed menu bar.
 *
 * \param hwnd The handle to the main window.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT void GUIAPI DrawMenuBar (HWND hwnd);

/**
 * \fn int GUIAPI TrackMenuBar (HWND hwnd, int pos)
 * \brief Displays the specified submenu.
 *
 * This function displays the specified submenu in the menu bar of
 * the specified main window \a hwnd.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the submenu. The position value of the
 *      first submenu is 0.
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TrackPopupMenu, CreateMenu
 */
MG_EXPORT int GUIAPI TrackMenuBar (HWND hwnd, int pos);

/**
 * \fn int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags, \
 *               int x, int y, HWND hwnd)
 * \brief Displays and tracks a popup menu.
 *
 * This function displays a shortcut menu at the specified location
 * and tracks the selection of items on the menu. The shortcut menu
 * can appear anywhere on the screen.
 *
 * \param hmnu The handle to the popup menu.
 * \param uFlags The tracking flags, can be OR'ed value of the following values:
 *      - TPM_LEFTALIGN\n
 *        Horz alignement is left.
 *      - TPM_CENTERALIGN\n
 *        Horz alignement is center.
 *      - TPM_RIGHTALIGN\n
 *        Horz alignement is right.
 *      - TPM_TOPALIGN\n
 *        Vert alignement is top.
 *      - TPM_VCENTERALIGN\n
 *        Vert alignement is center.
 *      - TPM_BOTTOMALIGN\n
 *        Vert alignement is bottom.
 *      - TPM_DESTROY\n
 *        Destroys the popup menu after finishing tracking.
 *      - TPM_SYSCMD\n
 *        Send an MSG_SYSCOMMAND message to the window when the use select a
 *        menu item.
 * \param x The x coordinate of the position of the popup menu.
 * \param y The y coordinate of the position of the popup menu.
 * \param hwnd The handle to the window which will receive
 *        the MSG_COMMAND or MSG_SYSCOMMAND message.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \sa TrackMenuBar, CreatePopupMenu
 */
MG_EXPORT int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags,
                int x, int y, HWND hwnd);

/**
 * \fn HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc)
 * \brief Retrieve the rect of a menu bar item.
 *
 * This function retrieves the rect of the menu bar item specified
 * by the parameter \a pos.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the item. The position value of the
 *        first item is 0.
 * \param prc The pointer to a RECT object, which will receive the rectangle.
 *
 * \return The function returns the handle to the menu bar if success,
 *      otherwise returns 0.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc);

#define LFRDR_MENU_STATE_HILITE       0x01
#define LFRDR_MENU_STATE_NORMAL       0x02
#define LFRDR_MENU_STATE_DISABLED     0x04

/**
 * \fn BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flag)
 * \brief Draws the specified menubar item with 3D effect.
 *
 * This function draws the specified menubar item with 3D effect.
 *
 * \param hwnd The handle to the main window.
 * \param pos The position of the item. The position value of the
 *        first item is 0.
 * \param flag The drawing flag, can be one of the following values:
 *      - LFRDR_MENU_STATE_HILITE\n
 *        hilite item.
 *      - LFRDR_MENU_STATE_NORMAL\n
 *        normal item.
 *      - LFRDR_MENU_STATE_DISABLED\n
 *        disabled item.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa TrackMenuBar
 */
MG_EXPORT BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flag);

/**
 * \fn int GUIAPI GetMenuItemCount (HMENU hmnu)
 * \brief Determine the number of items in a menu.
 *
 * This function determines the number of items in the specified menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 *
 * \return The number of the items in the menu.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT int GUIAPI GetMenuItemCount (HMENU hmnu);

/**
 * \fn LINT GUIAPI GetMenuItemID (HMENU hmnu, int pos)
 * \brief Retrieve the menu item identifier of a menu item at specified
 *        position in a menu.
 *
 * This function retrieves the menu item identifier of a menu item at
 * the specified position \a pos in the specified menu \a hmnu.
 *
 * \param hmnu The handle to the menu.
 * \param pos The position of the menu item. The position value of the
 *        first item is 0.
 *
 * \return The identifier of the items in the menu.
 * \note The type of return value changed from int to LINT since v3.2.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT LINT GUIAPI GetMenuItemID (HMENU hmnu, int pos);

/**
 * \fn int GUIAPI GetMenuItemInfo (HMENU hmnu, LINT item, \
 *               UINT flag, PMENUITEMINFO pmii)
 * \brief Retrieve information about a menu item.
 *
 * This function retrieves information about a menu item, and returns the
 * information via \a pmii.
 *
 * \param hmnu [in] Handle to the menu that contains the menu item.
 * \param item The position of the menu item or submenu.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii [in, out] Pointer to a \a MENUITEMINFO structure that specifies
 * the information to retrieve and receive information about the menu item.
 *
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI GetMenuItemInfo (HMENU hmnu, LINT item,
                            UINT flag, PMENUITEMINFO pmii);

/*Reserved*/
int GUIAPI GetMenuItemRect (HWND hwnd, HMENU hmnu,
                LINT item, PRECT prc);

/**
 * \fn HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu)
 * \brief Retrieve the submenu of the specified popup menu.
 *
 * This function retrieves the submenu of the specified popup menu.
 *
 * \param hpppmnu The handle to the popup menu.
 *
 * \return The handle to the submenu of the popup menu.
 *
 * \sa CreatePopupMenu
 */
MG_EXPORT HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu);

/**
 * \fn HMENU GUIAPI StripPopupHead (HMENU hpppmnu)
 * \brief Strips the title of the popup menu.
 *
 * This function strips the title of the popup menu \a hpppmnu.
 *
 * \param hpppmnu The handle to the popup menu.
 *
 * \return The handle to the submenu whose title has been stripped.
 *
 * \sa GetPopupSubMenu
 */
MG_EXPORT HMENU GUIAPI StripPopupHead (HMENU hpppmnu);

/**
 * \fn HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos)
 * \brief Retrieve the handle to the submenu activated by the specified menu
 *        item.
 *
 * This function retrieves the handle to the drop-down menu or submenu
 * activated by the specified menu item.
 *
 * \param hmnu The handle to the menu.
 * \param pos The position of the menu item. The position value of the
 *        first item is 0.
 *
 * \return The handle to the submenu; 0 if the menu item can not activate
 *         a submenu.
 *
 * \sa GetPopupSubMenu
 */
MG_EXPORT HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos);

/**
 * \fn HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag)
 * \brief Allows the application to access the window menu (also known as
 *        the system menu) for copying and modifying.
 *
 * This function returns the handle to the system menu of the main window.
 * This allows the application to access the window menu (also known as
 * the system menu) for copying and modifying.
 *
 * \param hwnd The handle to the main window.
 * \param flag Ignored currently; reserved for future use.
 *
 * \return The handle to the system menu; 0 if the main window has not
 *         a system menu.
 *
 * \sa GetMenu, SetMenu
 */
MG_EXPORT HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag);

/**
 * \fn UINT GUIAPI EnableMenuItem (HMENU hmnu, LINT item, UINT flag)
 * \brief Enables, disables, or grays the specified menu item.
 *
 * This function enables, disables, or grays the specified menu item.
 *
 * \param hmnu [in] Handle to the menu.
 * \param item [in] Specifies the menu item or submenu to be enabled, disabled,
 * or grayed, as determined by the \a flag parameter. This parameter specifies
 * an item in a menu bar, menu or submenu.
 * \param flag [in] Controls the interpretation of the \a item parameter and
 * indicates whether the menu item is enabled, disabled, or grayed. This
 * parameter must be a combination of either MF_BYPOSITION or MF_BYCOMMAND.
 *
 * \return The return value specifies the previous state of the menu item.
 * If the menu item does not exist, the return value is -1.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa GetMenuItemInfo
 */
MG_EXPORT UINT GUIAPI EnableMenuItem (HMENU hmnu, LINT item, UINT flag);

/**
 * \fn LINT GUIAPI CheckMenuRadioItem (HMENU hmnu, LINT first, LINT last, \
 *               LINT checkitem, UINT flag)
 * \brief Check a specified menu item and makes it a radio item.
 *
 * This function checks a specified menu item and makes it a radio item.
 * At the same time, the function unchecks all other menu items in the
 * associated group and clears the radio-item type flag for those items.
 *
 * \param hmnu The handle to the menu.
 * \param first The position of the first item in the group.
 * \param last The position of the last item in the group.
 * \param checkitem The position of the menu item to check.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \note The type of \a first, \a last, \a checkitem changed from
 *       int to LINT since v3.2.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI CheckMenuRadioItem (HMENU hmnu, LINT first, LINT last,
                            LINT checkitem, UINT flag);

/**
 * \fn int GUIAPI SetMenuItemBitmaps (HMENU hmnu, LINT item, \
 *                UINT flag, PBITMAP hBmpUnchecked, PBITMAP hBmpChecked)
 * \brief Associates the specified bitmap with a menu item.
 *
 * This function associates the specified bitmap with a menu item.
 * Whether the menu item is checked or unchecked, the system displays the
 * appropriate bitmap next to the menu item.
 *
 * \param hmnu The handle to the menu.
 * \param item The position of the menu item.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param hBmpUnchecked The pointer to the unchecked BITMAP object.
 * \param hBmpChecked The pointer to the checked BITMAP object.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa SetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI SetMenuItemBitmaps (HMENU hmnu, LINT item, UINT flag,
                            PBITMAP hBmpUnchecked, PBITMAP hBmpChecked);

/**
 * \fn int GUIAPI SetMenuItemInfo (HMENU hmnu, LINT item, \
 *               UINT flag, PMENUITEMINFO pmii)
 * \brief Changes information about a menu item.
 *
 * This function changes information about the specified menu item.
 *
 * \param hmnu [in] Handle to the menu that contains the menu item.
 * \param item [in] Specifies the identifier or position of the menu item
 * to change. The meaning of this parameter depends on the value of \a flag.
 * \param flag Indicates the position base, can be one of the following values:
 *      - MF_BYCOMMAND\n
 *        The position value \a item is based on the command identifier.
 *      - MF_BYPOSITION\n
 *        The position value \a item is based on the position in the menu.
 * \param pmii The pointer to a MENUITEMINFO structure contains the information.
 *
 * \return The function returns 0 for success, non-zero for failure.
 *
 * \note The type of \a item changed from int to LINT since v3.2.
 *
 * \sa GetMenuItemInfo, MENUITEMINFO
 */
MG_EXPORT int GUIAPI SetMenuItemInfo (HMENU hmnu, LINT item,
                            UINT flag, PMENUITEMINFO pmii);

    /** @} end of menu_fns */

    /**
     * \defgroup dialog_fns Dialog operations
     * @{
     */

/* Dialog codes */
/**
 * \def DLGC_WANTARROWS
 * \brief Control wants arrow keys.
 */
#define DLGC_WANTARROWS     0x0001
/**
 * \def DLGC_WANTTAB
 * \brief Control wants tab keys
 */
#define DLGC_WANTTAB        0x0002
/**
 * \def DLGC_WANTALLKEYS
 * \brief Control wants all keys.
 */
#define DLGC_WANTALLKEYS    0x0004
/**
 * \def DLGC_WANTCHARS
 * \brief Want MSG_CHAR messages.
 */
#define DLGC_WANTCHARS      0x0008

/**
 * \def DLGC_WANTENTER
 * \brief Control wants enter keys.
 */
#define DLGC_WANTENTER      0x0010
/**
 * \def DLGC_HASSETSEL
 * \brief Understands EM_SETSEL message.
 */
#define DLGC_HASSETSEL      0x0080

/**
 * \def DLGC_DEFPUSHBUTTON
 * \brief Default pushbutton.
 */
#define DLGC_DEFPUSHBUTTON  0x0100
/**
 * \def DLGC_PUSHBUTTON
 * \brief Non-default pushbutton.
 */
#define DLGC_PUSHBUTTON     0x0200
/**
 * \def DLGC_RADIOBUTTON
 * \brief Radio button.
 */
#define DLGC_RADIOBUTTON    0x0400
/**
 * \def DLGC_3STATE
 * \brief 3 States button item.
 */
#define DLGC_3STATE         0x0800
/**
 * \def DLGC_STATIC
 * \brief Static item: don't include.
 */
#define DLGC_STATIC         0x1000
/**
 * \def DLGC_BUTTON
 * \brief Button item: can be checked.
 */
#define DLGC_BUTTON         0x2000

/**
 * Structure which defines a control.
 */
typedef struct _CTRLDATA
{
    /** Class name of the control */
    const char* class_name;
    /** Control style */
    DWORD       dwStyle;
    /** Control position in dialog */
    int         x, y, w, h;
    /** Control identifier
      * \note The type changed from int to LINT since v3.2.
      */
    LINT         id;
    /** Control caption */
    const char* caption;
    /** Additional data */
    DWORD       dwAddData;
    /** Control extended style */
    DWORD       dwExStyle;

    /** window element renderer name */
    const char* werdr_name;

    /** table of we_attrs */
    const WINDOW_ELEMENT_ATTR* we_attrs;
} CTRLDATA;
typedef CTRLDATA* PCTRLDATA;

/**
 * Structure which defines a dialogbox.
 *
 * Example:
 *
 * \include dlgtemplate.c
 */
typedef struct _DLGTEMPLATE
{
    /** Dialog box style */
    DWORD       dwStyle;
    /** Dialog box extended style */
    DWORD       dwExStyle;
    /** Dialog box position */
    int         x, y, w, h;
    /** Dialog box caption */
    const char* caption;
    /** Dialog box icon */
    HICON       hIcon;
    /** Dialog box menu */
    HMENU       hMenu;
    /** Number of controls */
    int         controlnr;
    /** Poiter to control array */
    PCTRLDATA   controls;
    /** Addtional data, must be zero */
    DWORD       dwAddData;
} DLGTEMPLATE;
typedef DLGTEMPLATE* PDLGTEMPLATE;

/**
 * \fn HWND GUIAPI CreateMainWindowIndirectParamEx (PDLGTEMPLATE pDlgTemplate, \
 *               HWND hOwner, WNDPROC WndProc, LPARAM lParam, \
 *               const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs, \
 *               const char* window_name, const char* layer_name);
 * \brief Uses a dialog template and other information to create a modeless
 *        main window and controls in it, and pass a parameter to the window
 *        procedure.
 *
 * This function uses a dialog template pointed to by \a pDlgTemplate and
 * other information to create a modeless main window and controls in it.
 * The parameter specified by \a lParam will be passed to the window procedure
 * as the second paramter of MSG_INITDIALOG message.
 *
 * \param pDlgTemplate The pointer to a DLGTEMPLATE structure.
 * \param hOwner The handle to the hosting main window.
 * \param WndProc The window procedure of the new main window.
 * \param lParam The parameter will be passed to the window procedure.
 * \param werdr_name The name of window element renderer. NULL for default
 *                 renderer.
 * \param we_attrs The pointer to window element attribute table. NULL for
 *                 default window attribute table.
 * \param window_name The window name; reserved for future use.
 * \param layer_name The layer name; reserved for future use.
 *
 * \return Handle to the new main window, HWND_INVALID on error.
 *
 * \sa CreateMainWindowIndirectParam, DestroyMainWindowIndirect,
 *     DialogBoxIndirectParam, DLGTEMPLATE
 */

MG_EXPORT HWND GUIAPI CreateMainWindowIndirectParamEx (PDLGTEMPLATE pDlgTemplate,
        HWND hOwner, WNDPROC WndProc, LPARAM lParam,
        const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs,
        const char* window_name, const char* layer_name);

/**
 * \fn HWND GUIAPI CreateMainWindowIndirectParam (
                   PDLGTEMPLATE pDlgTemplate, HWND hOwner,
                   WNDPROC WndProc, LPARAM lParam)
 * \brief A simplified version of CreateMainWindowIndirectParamEx.
 *
 * This function creates a main window by calling
 * CreateMainWindowIndirectParamEx function and passing NULL for
 * werdr_name, we_attrs, window_name, and layer_name parameters.
 *
 * \sa CreateMainWindowIndirectParamEx
 */
static inline HWND GUIAPI CreateMainWindowIndirectParam (
                PDLGTEMPLATE pDlgTemplate, HWND hOwner,
                WNDPROC WndProc, LPARAM lParam)
{
    return CreateMainWindowIndirectParamEx (pDlgTemplate, hOwner,
            WndProc, lParam, NULL, NULL, NULL, NULL);
}

/**
 * \fn HWND GUIAPI CreateMainWindowIndirect (
                    PDLGTEMPLATE pDlgTemplate, HWND hOwner,
                    WNDPROC WndProc)
 * \brief A simplified version of CreateMainWindowIndirectParam.
 *
 * This function creates a main window by calling CreateMainWindowIndirectParam
 * function and passing 0 for lParam parameter.
 *
 * \sa CreateMainWindowIndirectParam
 */
static inline HWND GUIAPI CreateMainWindowIndirect (
                PDLGTEMPLATE pDlgTemplate, HWND hOwner, WNDPROC WndProc)
{
    return CreateMainWindowIndirectParam (pDlgTemplate, hOwner, WndProc, 0);
}

/**
 * \fn BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin)
 * \brief Destroys a main window created by \a CreateMainWindowIndirectParam.
 *
 * This function destroys the main window which was created by
 * \a CreateMainWindowIndirectParam function.
 *
 * \param hMainWin The handle to the main window.
 *
 * \sa CreateMainWindowIndirectParam
 */
MG_EXPORT BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin);

/**
 * \fn int GUIAPI DialogBoxIndirectParamEx (PDLGTEMPLATE pDlgTemplate, \
 *               HWND hOwner, WNDPROC DlgProc, LPARAM lParam, \
 *               const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs, \
 *               const char* window_name, const char* layer_name);
 * \brief Creates a modal dialog box from a dialog box template
 *        in memory and other information.
 *
 * This function creates a modal dialog box from a dialog box template
 * in memory and other information. Before displaying the dialog box,
 * the function passes an application-defined value to the dialog box
 * procedure as the second parameter of the MSG_INITDIALOG message.
 * An application can use this value to initialize the controls in the
 * dialog box.
 *
 * \param pDlgTemplate The pointer to a DLGTEMPLATE structure.
 * \param hOwner The handle to the hosting main window.
 * \param DlgProc The window procedure of the new dialog box.
 * \param lParam The parameter will be passed to the window procedure.
 * \param werdr_name The name of window element renderer. NULL for
 *                   default renderer.
 * \param we_attrs The pointer to window element attribute table.
 *                 NULL for default window attribute table.
 * \param window_name The window name; reserved for future use.
 * \param layer_name The layer name; reserved for future use.
 *
 * \return Return value of the dialog box, i.e., the second argument
 *         passed to EndDialog function which closes the dialog box.
 *
 * \sa EndDialog, DialogBoxIndirectParam, CreateMainWindowIndirectParam,
 *     DLGTEMPLATE
 */

MG_EXPORT int GUIAPI DialogBoxIndirectParamEx (PDLGTEMPLATE pDlgTemplate,
        HWND hOwner, WNDPROC DlgProc, LPARAM lParam,
        const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs,
        const char* window_name, const char* layer_name);

/**
 * \fn int GUIAPI DialogBoxIndirectParam (PDLGTEMPLATE pDlgTemplate,
                HWND hOwner, WNDPROC DlgProc, LPARAM lParam)
 * \brief A simplified version of DialogBoxIndirectParamEx.
 *
 * This function creates a dialog box by calling
 * DialogBoxIndirectParamEx function and passing NULL for
 * werdr_name, we_attrs, window_name, and layer_name parameters.
 *
 * \sa DialogBoxIndirectParamEx
 */
static inline int GUIAPI DialogBoxIndirectParam (PDLGTEMPLATE pDlgTemplate,
                HWND hOwner, WNDPROC DlgProc, LPARAM lParam)
{
    return DialogBoxIndirectParamEx (pDlgTemplate, hOwner, DlgProc, lParam,
                NULL, NULL, NULL, NULL);
}

/**
 * \fn BOOL GUIAPI EndDialog (HWND hDlg, int endCode)
 * \brief Destroys a modal dialog box, causing MiniGUI to end any processing
 *        for the dialog box.
 *
 * This function destroys the modal dialog box \a hDlg created by
 * \a DialogBoxIndirectParam and ends any processing for the dialog box.
 * The argument \a endCode will be returned by \a DialogBoxIndirectParam
 * as the return value.
 *
 * \param hDlg The handle to the dialog box.
 * \param endCode The value will be returned by \a DialogBoxIndirectParam.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \sa DialogBoxIndirectParam
 */
MG_EXPORT BOOL GUIAPI EndDialog (HWND hDlg, int endCode);

/**
 * \fn void GUIAPI DestroyAllControls (HWND hWnd)
 * \brief Destroys all controls in a window.
 *
 * This function destroys all controls (child windows) in a window.
 *
 * \param hWnd The handle to the window.
 *
 * \sa DestroyWindow
 */
MG_EXPORT void GUIAPI DestroyAllControls (HWND hWnd);

/**
 * \fn HWND GUIAPI GetDlgDefPushButton (HWND hWnd)
 * \brief Get the default push button control in a window.
 *
 * This function gets the handle to the default push button
 * (with BS_DEFPUSHBUTTON style) in the specified window \a hWnd.
 *
 * \param hWnd The handle to the window.
 * \return The handle to the default push button,
 *         zero for non default push button in the window.
 */
MG_EXPORT HWND GUIAPI GetDlgDefPushButton (HWND hWnd);

/**
 * \fn LINT GUIAPI GetDlgCtrlID (HWND hwndCtl)
 * \brief Get the integer identifier of a control.
 *
 * This function gets the integer identifier of the control \a hwndCtl.
 *
 * \param hwndCtl The handle to the control.
 *
 * \return The identifier of the control, -1 for error.
 *
 * \note The type of return value changed from int to LINT since v3.2.
 *
 * \sa GetDlgItem
 */
MG_EXPORT LINT GUIAPI GetDlgCtrlID (HWND hwndCtl);

/**
 * \fn HWND GUIAPI GetDlgItem (HWND hDlg, LINT nIDDlgItem)
 * \brief Retrieve the handle to a control in a dialog box.
 *
 * This function retrieves the handle to a control, whose identifier is
 * \a nIDDlgItem, in the specified dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \return The handle to the control, zero for not found.
 */
MG_EXPORT HWND GUIAPI GetDlgItem (HWND hDlg, LINT nIDDlgItem);

/**
 * \fn UINT GUIAPI GetDlgItemInt (HWND hDlg, LINT nIDDlgItem, \
 *               BOOL *lpTranslated, BOOL bSigned)
 * \brief Translates the text of a control in a dialog box into an integer
 *        value.
 *
 * This function translates the text of the control, whose identifier is
 * \a nIDDlgItem in the dialog box \a hDlg into an integer value.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpTranslated The pointer to a boolean value, which indicates whether
 *        translated successfully.
 * \param bSigned Indicates whether handle the text as a signed integer.
 *
 * \return The translated 32-bit integer.
 *
 * \note MiniGUI uses \a strtol or \a strtoul to convert the string value
 * to a 32-bit integer, and pass the base as 0. Thus, the valid string value
 * should be in the following forms:
 *
 *  - [+|-]0x[0-9|A-F]*\n
 *    Will be read in base 16.
 *  - [+|-]0[0-7]*\n
 *    Will be read in base 8.
 *  - [+|-][1-9][0-9]*\n
 *    Will be read in base 10.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa GetDlgItemText, SetDlgItemInt
 */
MG_EXPORT UINT GUIAPI GetDlgItemInt (HWND hDlg, LINT nIDDlgItem,
                BOOL *lpTranslated, BOOL bSigned);

/**
 * \fn int GUIAPI GetDlgItemText (HWND hDlg, LINT nIDDlgItem, \
 *               char* lpString, int nMaxCount)
 * \brief Retrieve the title or text associated with a control in a dialog box.
 *
 * This function retrieves the title or text associated with a control, whose
 * identifier is \a nIDDlgItem in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpString The pointer to a buffer which receives the text.
 * \param nMaxCount The maximal length of the string, not including
 *        the null character.
 *
 * \return The length of the null-terminated text.
 *
 * \note The buffer should at least have size of (\a nMaxCount + 1).
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa GetDlgItemInt, GetDlgItemText2
 */
MG_EXPORT int GUIAPI GetDlgItemText (HWND hDlg, LINT nIDDlgItem,
                char* lpString, int nMaxCount);

/**
 * \fn char* GUIAPI GetDlgItemText2 (HWND hDlg, LINT id, int* lenPtr)
 * \brief Retrieve the title or text associated with a control in a dialog box.
 *
 * This function is similiar as \a GetDlgItemText function,
 * but it allocates memory for the text and returns the pointer
 * to the allocated buffer. You should free the buffer when done by using
 * \a free function.
 *
 * \param hDlg The handle to the dialog box.
 * \param id The identifier of the control.
 * \param lenPtr The pointer to an integer which receives the length of the text
 *        if it is not NULL.
 *
 * \return The pointer to the allocated buffer.
 *
 * \note The type of \a id changed from int to LINT since v3.2.
 *
 * \sa GetDlgItemText
 */
MG_EXPORT char* GUIAPI GetDlgItemText2 (HWND hDlg, LINT id, int* lenPtr);

/**
 * \fn HWND GUIAPI GetNextDlgGroupItem (HWND hDlg, \
 *               HWND hCtl, BOOL bPrevious)
 * \brief Retrieve the handle to the first control in a group of controls
 *        that precedes (or follows) the specified control in a dialog box.
 *
 * This function retrieves the handle to the first control in a group of
 * controls that precedes (or follows) the specified control \a hCtl in
 * the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param hCtl The handle to the control.
 * \param bPrevious A boolean value indicates to retrive the preceding or
 *        following control. TRUE for preceding control.
 *
 * \return The handle to the preceding or following control.
 *
 * \sa GetNextDlgTabItem
 */
MG_EXPORT HWND GUIAPI GetNextDlgGroupItem (HWND hDlg,
                HWND hCtl, BOOL bPrevious);

/**
 * \fn HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
 * \brief Retrieve the handle to the first control that has the WS_TABSTOP
 *        style that precedes (or follows) the specified control.
 *
 * This function retrieves the handle to the first control that has the
 * WS_TABSTOP style that precedes (or follows) the specified control \a hCtl
 * in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param hCtl The handle to the control.
 * \param bPrevious A boolean value indicates to retrive the preceding or
 *        following control. TRUE for preceding control.
 *
 * \return The handle to the preceding or following control.
 *
 * \sa GetNextDlgGroupItem
 */
MG_EXPORT HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious);

/**
 * \fn LRESULT GUIAPI SendDlgItemMessage (HWND hDlg, LINT nIDDlgItem, \
 *               UINT message, WPARAM wParam, LPARAM lParam)
 * \brief Send a message to the specified control in a dialog box.
 *
 * This function sends a message specified by (\a message, \a wParam, \a lParam)
 * to the specified control whose identifier is \a nIDDlgItem in the dialog
 * box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param message The message identifier.
 * \param wParam The first message parameter.
 * \param lParam The second message parameter.
 *
 * \return The return value of the message handler.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa SendMessage, GetDlgItem
 */
MG_EXPORT LRESULT GUIAPI SendDlgItemMessage (HWND hDlg, LINT nIDDlgItem,
                UINT message, WPARAM wParam, LPARAM lParam);

/**
 * \fn BOOL GUIAPI SetDlgItemInt (HWND hDlg, LINT nIDDlgItem, \
 *               UINT uValue, BOOL bSigned)
 * \brief Set the text of a control in a dialog box to the string
 *        representation of a specified integer value.
 *
 * This function sets the text of the control whose identifier is \a nIDDlgItem
 * in the dialog box \a hDlg to the string representation of
 * the specified integer value \a nValue.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param uValue The 32-bit integer value.
 * \param bSigned A boolean value indicates whether the integer value is a
 *        signed integer.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa GetDlgItemInt, SetDlgItemText
 */
MG_EXPORT BOOL GUIAPI SetDlgItemInt (HWND hDlg, LINT nIDDlgItem,
                UINT uValue, BOOL bSigned);

/**
 * \fn BOOL GUIAPI SetDlgItemText (HWND hDlg, int nIDDlgItem, \
 *               const char* lpString)
 * \brief Set the title or text of a control in a dialog box.
 *
 * This function sets the title or text of the control whose identifier
 * is \a nIDDlgItem in the dialog box \a hDlg to the string pointed to
 * by \a lpString.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param lpString The pointer to the string.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa GetDlgItemText, SetDlgItemInt
 */
MG_EXPORT BOOL GUIAPI SetDlgItemText (HWND hDlg, LINT nIDDlgItem,
                const char* lpString);

#ifdef _MGCTRL_BUTTON

/**
 * \fn void GUIAPI CheckDlgButton (HWND hDlg, LINT nIDDlgItem, int nCheck)
 * \brief Changes the check status of a button control.
 *
 * This function changes the check status of the button control whose
 * identifier is \a nIDDlgItem in the dialog box \a hDlg.
 *
 * \param hDlg The handle to the dialog box.
 * \param nIDDlgItem The identifier of the control.
 * \param nCheck The state of the button. If the button is a normal button,
 *        the value being zero means the button is checked, otherwise unchecked.
 *        If the button is a special button with three states, the value
 *        can be one of the following values:
 *          - BST_UNCHECKED\n
 *            The button is unchecked.
 *          - BST_CHECKED\n
 *            The button is checked.
 *          - BST_INDETERMINATE\n
 *            The button is in indeterminate state.
 *
 * \note The type of \a nIDDlgItem changed from int to LINT since v3.2.
 *
 * \sa CheckRadioButton, IsDlgButtonChecked
 */
MG_EXPORT void GUIAPI CheckDlgButton (HWND hDlg, LINT nIDDlgItem, int nCheck);

/**
 * \fn void GUIAPI CheckRadioButton (HWND hDlg, \
 *               LINT idFirstButton, LINT idLastButton, LINT idCheckButton)
 * \brief Adds a check mark to (checks) a specified radio button in a group
 *        and removes a check mark from (clears) all other radio buttons in
 *        the group.
 *
 * This function adds a check mark to (checks) the specified radio button
 * \a idCheckButton in a group between \a idFirstButton and \a idLastButton,
 * and removes a check mark from (clears) all other radio buttons in the group.
 *
 * \param hDlg The handle to the dialog box.
 * \param idFirstButton The identifier of the first control in the group.
 * \param idLastButton The identifier of the last control in the group.
 * \param idCheckButton The identifier of the control to be checked.
 *
 * \note The type of \a idFirstButton, \a idLastButton, and \a idCheckButton
 *       changed from int to LINT since v3.2.
 *
 * \sa CheckDlgButton
 */
MG_EXPORT void GUIAPI CheckRadioButton (HWND hDlg,
                LINT idFirstButton, LINT idLastButton, LINT idCheckButton);

/**
 * \fn int GUIAPI IsDlgButtonChecked (HWND hDlg, LINT idButton)
 * \brief Determine whether a button control has a check mark next to it or
 *        whether a three-state button control is grayed, checked, or neither.
 *
 * This function determines whether the button control whose identifier is
 * \a idButton has a check mark next to it or whether a three-state button
 * control is grayed, checked, or neither.
 *
 * \param hDlg The handle to the dialog box.
 * \param idButton The identifier of the button.
 *
 * \return The check state of the button. If the button is a normal button,
 *        the value being zero means the button is checked, otherwise unchecked.
 *        If the button is a special button with three states, the value
 *        can be one of the following values:
 *          - BST_UNCHECKED\n
 *            The button is unchecked.
 *          - BST_CHECKED\n
 *            The button is checked.
 *          - BST_INDETERMINATE\n
 *            The button is in indeterminate state.
 *
 * \note The type of \a idButton tem changed from int to LINT since v3.2.
 *
 * \sa CheckDlgButton
 */
MG_EXPORT int  GUIAPI IsDlgButtonChecked (HWND hDlg, LINT idButton);
#endif

     /** @} end of dialog_fns */

     /**
      * \defgroup msgbox_fns Message box operations
      * @{
      */

#if defined(_MGCTRL_STATIC) && defined (_MGCTRL_BUTTON)

/* Standard control IDs */
#define IDC_STATIC    0
#define IDOK          1
#define IDCANCEL      2
#define IDABORT       3
#define IDRETRY       4
#define IDIGNORE      5
#define IDYES         6
#define IDNO          7

#define MINID_RESERVED      0xF001
#define MAXID_RESERVED      0xFFFF

#define MB_OK                   0x00000000
#define MB_OKCANCEL             0x00000001
#define MB_YESNO                0x00000002
#define MB_RETRYCANCEL          0x00000003
#define MB_ABORTRETRYIGNORE     0x00000004
#define MB_YESNOCANCEL          0x00000005
#define MB_CANCELASBACK         0x00000008  /* customized style */
#define MB_TYPEMASK             0x00000007

#define MB_ICONHAND             0x00000010
#define MB_ICONQUESTION         0x00000020
#define MB_ICONEXCLAMATION      0x00000030
#define MB_ICONASTERISK         0x00000040
#define MB_ICONMASK             0x000000F0

#define MB_ICONINFORMATION      MB_ICONASTERISK
#define MB_ICONSTOP             MB_ICONHAND

#define MB_DEFBUTTON1           0x00000000
#define MB_DEFBUTTON2           0x00000100
#define MB_DEFBUTTON3           0x00000200
#define MB_DEFMASK              0x00000F00

/* #define MB_APPLMODAL            0x00000000 */
/* #define MB_SYSTEMMODAL          0x00001000 */
/* #define MB_TASKMODAL            0x00002000 */

#define MB_NOFOCUS              0x00008000

#define MB_ALIGNCENTER          0x00000000
#define MB_ALIGNTOPLEFT         0x00010000
#define MB_ALIGNBTMLEFT         0x00020000
#define MB_ALIGNTOPRIGHT        0x00030000
#define MB_ALIGNBTMRIGHT        0x00040000
#define MB_ALIGNMASK            0x00070000

#define MB_BASEDONPARENT        0x00080000  /* default is desktop. */

/**
 * \fn int GUIAPI MessageBox (HWND hParentWnd, const char* pszText, \
 *               const char* pszCaption, DWORD dwStyle)
 * \brief Displays a message box within one, two, or three push buttons.
 *
 * This function displays a message box within one, two, or three buttons,
 * and returns the identifier of the button clicked by the user. You can
 * pass \a dwStyle argument with different values to control the number of
 * buttons, the button titles, and so on.
 *
 * \param hParentWnd The handle to the hosting main window.
 * \param pszText The message text will be displayed in the message box.
 * \param pszCaption The caption of the message box.
 * \param dwStyle Specifies the contents and behavior of the dialog box.
 * This parameter can be a combination of flags from the following groups
 * of flags.
 *
 *          To indicate the buttons displayed in the message box, specify
 *          one of the following values.
 *          - MB_OK\n
 *            Create a message box within only one button whose title is "OK".
 *          - MB_OKCANCEL\n
 *            Create a message box within two buttons whose title are "OK"
 *            and "Cancel" respectively.
 *          - MB_YESNO\n
 *            Create a message box within two buttons whose title are "Yes"
 *            and "No" respectively.
 *          - MB_RETRYCANCEL\n
 *            Create a message box within two buttons whose title are "Retry"
 *            and "Cancel" respectively.
 *          - MB_ABORTRETRYIGNORE\n
 *            Create a message box within three buttons whose title are
 *            "Abort", "Retry" and "Ignore" respectively.
 *          - MB_YESNOCANCEL\n
 *            Create a message box within three buttons whose title are
 *            "Yes", "No" and "Cancel" respectively.
 *          - MB_CANCELASBACK\n
 *            Title "Cancel" replaced with title "Previous".
 *
 *          To display an icon in the message box, specify one of the following
 *          values.
 *          - MB_ICONHAND\n
 *            Display a hand/stop icon in the message box.
 *          - MB_ICONQUESTION\n
 *            Display a question mark icon in the message box.
 *          - MB_ICONEXCLAMATION\n
 *            Display a exclamation mark icon in the message box.
 *          - MB_ICONASTERISK\n
 *            Display an information icon in the message box.
 *          - MB_ICONINFORMATION\n
 *            Display an information icon in the message box.
 *          - MB_ICONSTOP\n
 *            Display a hand/stop icon in the message box.
 *
 *          To indicate the default button, specify one of the following values.
 *          - MB_DEFBUTTON1\n
 *            The first button is the default button.
 *          - MB_DEFBUTTON2\n
 *            The second button is the default button.
 *          - MB_DEFBUTTON3\n
 *            The third button is the default button.
 *
 *          To indicate the default text alignment, specify one of the following
 *          values.
 *          - MB_ALIGNCENTER\n
 *            The message box is center aligned.
 *          - MB_ALIGNTOPLEFT\n
 *            The message box is upper-left aligned.
 *          - MB_ALIGNBTMLEFT\n
 *            The message box is lower-left aligned.
 *          - MB_ALIGNTOPRIGHT\n
 *            The message box is upper-right aligned.
 *          - MB_ALIGNBTMRIGHT\n
 *            The message box is lower-right aligned.
 *
 *          To indicate the default align based on hosting window, specify the following value.
 *          - MB_BASEDONPARENT\n
 *            A flag indicates that the alignments above are based on the
 *            hosting window, not the desktop.
 *
 * \return The identifier of the button clicked by user, can be one of the
 *         following values:
 *          - IDOK\n
 *            The "OK" button clicked.
 *          - IDCANCEL\n
 *            The "Cancel" or "Previous" button clicked.
 *          - IDABORT\n
 *            The "Abort" button clicked.
 *          - IDRETRY\n
 *            The "Retry" button clicked.
 *          - IDIGNORE\n
 *            The "Ignore" button clicked.
 *          - IDYES\n
 *            The "Yes" button clicked.
 *          - IDNO\n
 *            The "No" button clicked.
 *
 * \note This function will call \a GetSysText to translate the button
 *       titles to localized text.
 *
 * \sa GetSysText, DialogBoxIndirectParam
 */
MG_EXPORT int GUIAPI MessageBox (HWND hParentWnd, const char* pszText,
                      const char* pszCaption, DWORD dwStyle);

#endif /* _MGCTRL_STATIC && _MGCTRL_BUTTON */

/**
 * \fn void GUIAPI MessageBeep (DWORD dwBeep)
 * \brief Makes a sound of beep.
 *
 * This function makes a sound of beep. We ignore \a dwBeep argument so far.
 *
 * \param dwBeep The beep type, ignored so far.
 *
 * \sa Beep, Ping, Tone
 */
MG_EXPORT void GUIAPI MessageBeep (DWORD dwBeep);

    /** @} end of msgbox_fns */
    /** @} end of window_fns */
    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WINDOW_H */

