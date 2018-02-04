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
 ** ipb5310remote.c: Low Level Input Engine for iPB5310 remote control.
 ** 
 ** Created by ChenLei, 2009/01/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_NEXUS

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/kd.h>
#include <linux/keyboard.h>

#include "ial.h"
#include "nexus.h"

/*
 ** Broadcom specific includes.
 */ 
#include "nexus_types.h"
#include "nexus_ir_input.h"

#define _DEBUG 
//#undef _DEBUG

static NEXUS_IrInputHandle irInput;
static NEXUS_IrInputSettings irSettings;

static int fdstream[2];

static char state[NR_KEYS]={0};
static int cur_key = -2;

static int keycode2scan(unsigned int key)
{
		switch(key)
		{
				case RC_UP:
						return SCANCODE_CURSORBLOCKUP;
				case RC_DOWN:
						return SCANCODE_CURSORBLOCKDOWN;
				case RC_LEFT:
						return SCANCODE_CURSORBLOCKLEFT;
				case RC_RIGHT:
						return SCANCODE_CURSORBLOCKRIGHT;
				case RC_OK:
						return SCANCODE_ENTER;
				case RC_ESCAPE:
						return SCANCODE_ESCAPE;
				case RC_CHANNEL_UP:
						return SCANCODE_LEFTWIN;
				case RC_CHANNEL_DOWN:
						return SCANCODE_RIGHTWIN;
				case RC_0:
						return SCANCODE_0; 
				case RC_1:
						return SCANCODE_1; 
				case RC_2:
						return SCANCODE_2; 
				case RC_3:
						return SCANCODE_3; 
				case RC_4:
						return SCANCODE_4; 
				case RC_5:
						return SCANCODE_5; 
				case RC_6:
						return SCANCODE_6; 
				case RC_7:
						return SCANCODE_7; 
				case RC_8:
						return SCANCODE_8; 
				case RC_9:
						return SCANCODE_9; 
                                case RC_MENU:   return SCANCODE_SLASH;
                                case RC_GUIDE:  return SCANCODE_F12;
				default:
						return -1;
		}
}

static int keyboard_update(void)
{
        if (cur_key < 0)
        {
				memset(state, 0, sizeof(state));
                cur_key = -2;
        }
        else
        {
                int scancode = keycode2scan(cur_key); 
                if (scancode == -1)
                {
                    fprintf(stderr, "IAL: unknown key pressed <0x%x>\n", cur_key);
                }

                if(scancode < 0)
                {
                        cur_key = -1;
                        return 0;
                }
                else
                {
                        state[scancode] = 1;
                }
        }

        return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
        return (char *) state;
}

static void NEXUS_IrDataReady(void *context, int param)
{
	NEXUS_Error			rc;
	NEXUS_IrInputEvent	irEvent;
	size_t				numEventRead;
	bool				overflow;

    do
    {
            unsigned int key;
            int repeat;
            rc = NEXUS_IrInput_GetEvents(irInput, &irEvent, 1, &numEventRead, &overflow);	
            if (numEventRead > 0)
            {
                    key = irEvent.code;
                    repeat = irEvent.repeat;
                    write(fdstream[1], &key, sizeof(key));
                    write(fdstream[1], &repeat, sizeof(repeat));
            }
    }
    while (numEventRead > 0);
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
        fd_set rfds;
        int e;

        /* triger a key_up event */
        if (cur_key >= -1)
        {
                select (maxfd+1, in, out, except, timeout);
                cur_key = -1;
                return IAL_KEYEVENT;
        }

        if(which & IAL_KEYEVENT)
        {
                if(!in)
                {
                        in = &rfds;
                        FD_ZERO(in);
                }

                FD_SET(fdstream[0], in);
                if(fdstream[0] > maxfd) maxfd = fdstream[0];
        }	

        e = select (maxfd+1, in, out, except, timeout);

        if(e > 0)
        {
                if(FD_ISSET(fdstream[0], in))
                {
                        unsigned int key;
                        int repeat;
                        FD_CLR(fdstream[0], in);
                        read(fdstream[0], &key, sizeof(key));
                        read(fdstream[0], &repeat, sizeof(repeat));

                        if (repeat)
                        {
                            return 0;
                        }
                        else
                        {
                            cur_key = key;
                        }
                        return IAL_KEYEVENT;
                }
                else
                {
                        return 0;
                }
        }
        if(e == 0)
        {
                return 0;
        }	
        else /* e < 0 */
        {
            return -1;
        }
}


BOOL InitNexusInput (INPUT* input)
{
		pipe(fdstream);
		NEXUS_IrInput_GetDefaultSettings(&irSettings);
		irSettings.dataReady.callback = NEXUS_IrDataReady; 
		irSettings.mode				  = NEXUS_IrInputMode_eRemoteA;
		
		irInput = NEXUS_IrInput_Open(0, &irSettings);
		
		if(irInput == NULL)
		{
				fprintf(stderr, "InitNexus: Cannot open remote control!\n");
				return false;
		}

	
		input->update_keyboard = keyboard_update;
		input->get_keyboard_state = keyboard_getstate;
		input->set_leds = NULL;

		input->wait_event = wait_event;

		return true;
}

void TermNexusInput (void)
{
		if(irInput != NULL)
				NEXUS_IrInput_Close(irInput);
}

#endif /* _NEXUS_IAL */
