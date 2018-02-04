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
** abssig.c: Low Level Input Engine for frv-uclinux ABSSIG.
** 
** Created by Peng Ke, 2004/12/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _ABSSIG_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ial.h"
#include "abssig.h"

static unsigned char state[NR_KEYS] = { 0, 0 };
static int cur_ir_scancode = -1;
static ABS_SIG_INFO asi;

#undef _DEBUG

/************************  Low Level Input Operations **********************/

static int
keyboard_update(void)
{
    int     status;
    unsigned long msg;
    int     key = -1;

    //fprintf(stderr, "%s : %s : %d, cur_ir_scancode %d\n", __FILE__, __FUNCTION__, __LINE__, cur_ir_scancode);
    status = 1;
    if (cur_ir_scancode != -1)
    {
        state[cur_ir_scancode] = 0;
        cur_ir_scancode = -1;
        return NR_KEYS;
    }
    msg = asi.dev_msg;
    if (msg < 0xFF)
        msg &= 0x3F;
    if (msg >= 0x20 && msg < 0x40)
    {
        status = 0;
        msg = ~msg;
        msg &= 0x3F;
    }
    switch (msg)
    {
    case TEL_RMSG_KEY_1:
        key = SCANCODE_1;
        break;
    case TEL_RMSG_KEY_2:
        key = SCANCODE_2;
        break;
    case TEL_RMSG_KEY_3:
        key = SCANCODE_3;
        break;
    case TEL_RMSG_KEY_4:
        key = SCANCODE_4;
        break;
    case TEL_RMSG_KEY_5:
        key = SCANCODE_5;
        break;
    case TEL_RMSG_KEY_6:
        key = SCANCODE_6;
        break;
    case TEL_RMSG_KEY_7:
        key = SCANCODE_7;
        break;
    case TEL_RMSG_KEY_8:
        key = SCANCODE_8;
        break;
    case TEL_RMSG_KEY_9:
        key = SCANCODE_9;
        break;
    case TEL_RMSG_KEY_Asterisk:
        key = SCANCODE_KEYPADMULTIPLY;
        break;
    case TEL_RMSG_KEY_0:
        key = SCANCODE_0;
        break;
    case TEL_RMSG_KEY_Sharp:
        key = SCANCODE_S;
        break;
    case TEL_RMSG_KEY_REDIAL:
        key = SCANCODE_R;
        break;
    case TEL_RMSG_KEY_FLASH:
        key = SCANCODE_F;
        break;
    case TEL_RMSG_KEY_MUTE:
        key = SCANCODE_M;
        break;
    case TEL_RMSG_KEY_HANDFREE:
        key = SCANCODE_Z;
        break;
    case TEL_RMSG_KEY_TP10:
        key = SCANCODE_Q;
        break;
    case TEL_RMSG_KEY_TP20:
        key = SCANCODE_W;
        break;
    case TEL_RMSG_KEY_PINP:
        key = SCANCODE_N;
        break;
    case TEL_RMSG_KEY_PRIVATE:
        key = SCANCODE_P;
        break;
    case TEL_RMSG_KEY_MENU:
        key = SCANCODE_ENTER;
        break;
    case TEL_RMSG_KEY_CANCEL:
        key = SCANCODE_ESCAPE;
        break;
    case TEL_RMSG_KEY_UP:
        key = SCANCODE_CURSORBLOCKUP;
        break;
    case TEL_RMSG_KEY_DOWN:
        key = SCANCODE_CURSORBLOCKDOWN;
        break;
    case TEL_RMSG_KEY_LEFT:
        key = SCANCODE_CURSORBLOCKLEFT;
        break;
    case TEL_RMSG_KEY_RIGHT:
        key = SCANCODE_CURSORBLOCKRIGHT;
        break;
    case TEL_RMSG_KEY_VOLINC:
        key = SCANCODE_KEYPADPLUS;
        break;
    case TEL_RMSG_KEY_VOLDEC:
        key = SCANCODE_KEYPADMINUS;
        break;
//    case TEL_RMSG_KEY_TRANS:
//        key = SCANCODE_T;
//        break;
//    case TEL_RMSG_KEY_HOLD:
//        key = SCANCODE_H;
//        break;
//    case TEL_RMSG_KEY_REGISTER:
//        key = SCANCODE_G;
//        break;
    case TEL_RMSG_KEY_F1:
        key = SCANCODE_F1;
        break;
    case TEL_RMSG_KEY_F2:
        key = SCANCODE_F2;
        break;
    case TEL_RMSG_KEY_F3:
        key = SCANCODE_F3;
        break;
    case TEL_RMSG_KEY_F4:
        key = SCANCODE_F4;
        break;
    case TEL_RMSG_KEY_F5:
        key = SCANCODE_F5;
        break;
    case TELE_SIGNAL_DRV2TSK_OFFHOOK:
        key = SCANCODE_INSERT;
        break;
    case TELE_SIGNAL_DRV2TSK_ONHOOK:
        key = SCANCODE_REMOVE;
        break;
    case TELE_SIGNAL_DRV2TSK_PSTN_DIAL_TONE:
        key = SCANCODE_PAGEUP;
        break;
    case TELE_SIGNAL_DRV2TSK_PSTN_RING_START:
        key = SCANCODE_HOME;
        break;
    case TELE_SIGNAL_DRV2TSK_PSTN_RING_STOP:
        key = SCANCODE_END;
        break;
    case TELE_SIGNAL_DRV2TSK_RINGBACK:
        key = SCANCODE_PAGEDOWN;
        break;
    case TELE_SIGNAL_DRV2TSK_PSTN_CALLER_INFO:
        key = SCANCODE_PRINTSCREEN;
        break;
    default:
        key = -1;
        break;
    }
#ifdef _DEBUG
    fprintf(stderr, "key %d is %s\n", key, status ? "DOWN" : "UP");
#endif
#if 0
    if (status && msg > 0xFF)
        cur_ir_scancode = key;
#else
    if(!status)
        return 0;
    if (key != -1)
        cur_ir_scancode = key;
#endif
    if (key != -1)
        state[key] = status;
    return NR_KEYS;
}

static const char *
keyboard_getstate(void)
{
    return (char *)state;
}

#ifdef _LITE_VERSION
static int
wait_event(int which, int maxfd, fd_set * in, fd_set * out, fd_set * except,
           struct timeval *timeout)
#else
static int
wait_event(int which, fd_set * in, fd_set * out, fd_set * except,
           struct timeval *timeout)
#endif
{
    fd_set  rfds;
    int     retvalue = 0;
    int     e;

    if (!in)
    {
        in = &rfds;
        FD_ZERO(in);
    }

    if ((which & IAL_KEYEVENT) && asi.dev_fd >= 0)
    {
        FD_SET(asi.dev_fd, in);
#ifdef _LITE_VERSION
        if (asi.dev_fd > maxfd)
            maxfd = asi.dev_fd;
#endif
    }

#ifdef _LITE_VERSION
    e = select(maxfd + 1, in, out, except, timeout);
#else
    e = select(FD_SETSIZE, in, out, except, timeout);
#endif

    if (cur_ir_scancode != -1)
    {
        retvalue |= IAL_KEYEVENT;
        FD_CLR(asi.dev_fd, in);
        return retvalue;
    }

    if (e > 0)
    {
        if (asi.dev_fd >= 0 && FD_ISSET(asi.dev_fd, in))
        {
            FD_CLR(asi.dev_fd, in);
            if (ABSSIG_receive_msg_from_driver(&asi) == ABSSIG_SUCCESS)
                retvalue |= IAL_KEYEVENT;
        }
    }
    else if (e < 0)
    {
        return -1;
    }

    return retvalue;
}

void
get_mouse_xy(int *x, int *y)
{
}
int
get_mouse_button(void)
{
    return 0;
}

BOOL
InitABSSIGInput(INPUT * input, const char *mdev, const char *mtype)
{
    memset(&asi, 0, sizeof(asi));
    asi.dev_type = UD_DSP_1;

    if (ABSSIG_open_tel_device(&asi) != ABSSIG_SUCCESS)
    {
        fprintf(stderr, "ABSSIG: Can not open tel device!\n");
        return FALSE;
    }
    input->update_mouse = NULL;
    input->get_mouse_xy = get_mouse_xy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = get_mouse_button;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    return TRUE;
}

void
TermABSSIGInput(void)
{
    fprintf(stderr, "%s : %s : %d\n", __FILE__, __FUNCTION__, __LINE__);
    ABSSIG_close_tel_device(&asi);
    memset(&asi, 0, sizeof(asi));
}

#endif /* _ABSSIG_IAL */
