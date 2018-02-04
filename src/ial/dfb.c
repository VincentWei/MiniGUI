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
** dfb.c: Low Level Input Engine for DirectFB.
** 
** Created by Xiaogang Du, 2005/12/15
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_DFB

#include <unistd.h>
#include <fcntl.h>

#include <directfb.h>
#include <directfb_keyboard.h>

#include "ial.h"
#include "dfb.h"

#define dbg() printf("%d %s %s\n", __LINE__, __FILE__, __FUNCTION__)
extern IDirectFB *__mg_dfb;
extern IDirectFBSurface *pFrameBuffer;

typedef struct _DeviceInfo DeviceInfo; 
 
struct _DeviceInfo { 
	DFBInputDeviceID           device_id; 
	DFBInputDeviceDescription  desc; 
	DeviceInfo                *next; 
};

static int screen_height = 0;
static int screen_width = 0;

static DeviceInfo *devices_info = NULL;
static IDirectFBEventBuffer *evtbuf;
static IDirectFBInputDevice *mouse_device = NULL;
static unsigned char state[NR_KEYS];
static unsigned int curkey;
static int dev_fd;

static int mouse_x = 0;
static int mouse_y = 0;
static int mouse_button = 0;
 
static void mouse_update(void)
{
	return 0;
}

static void mouse_getxy (int* x, int* y)
{
	*x = mouse_x;
    *y = mouse_y;
}

static int mouse_getbutton(void)
{
	return mouse_button;
}


static unsigned char keycode_scancode [256];
static void init_code_map (void)
{
    keycode_scancode [0x1B] = SCANCODE_ESCAPE;

    keycode_scancode [0x29] = SCANCODE_0;
    keycode_scancode [0x21] = SCANCODE_1;
    keycode_scancode [0x40] = SCANCODE_2;
    keycode_scancode [0x23] = SCANCODE_3;
    keycode_scancode [0x24] = SCANCODE_4;
    keycode_scancode [0x25] = SCANCODE_5;
    keycode_scancode [0x5E] = SCANCODE_6;
    keycode_scancode [0x26] = SCANCODE_7;
    keycode_scancode [0x2A] = SCANCODE_8;
    keycode_scancode [0x28] = SCANCODE_9;

    keycode_scancode [0x30] = SCANCODE_0;
    keycode_scancode [0x31] = SCANCODE_1;
    keycode_scancode [0x32] = SCANCODE_2;
    keycode_scancode [0x33] = SCANCODE_3;
    keycode_scancode [0x34] = SCANCODE_4;
    keycode_scancode [0x35] = SCANCODE_5;
    keycode_scancode [0x36] = SCANCODE_6;
    keycode_scancode [0x37] = SCANCODE_7;
    keycode_scancode [0x38] = SCANCODE_8;
    keycode_scancode [0x39] = SCANCODE_9;

    keycode_scancode [0x2D] = SCANCODE_MINUS;
    keycode_scancode [0x5F] = SCANCODE_MINUS;
    keycode_scancode [0x3D] = SCANCODE_EQUAL;
    keycode_scancode [0x2B] = SCANCODE_EQUAL;
    keycode_scancode [0x08] = SCANCODE_BACKSPACE;
    keycode_scancode [0x09] = SCANCODE_TAB;
    keycode_scancode [0x51] = SCANCODE_Q;
    keycode_scancode [0x57] = SCANCODE_W;
    keycode_scancode [0x45] = SCANCODE_E;
    keycode_scancode [0x52] = SCANCODE_R;
    keycode_scancode [0x54] = SCANCODE_T;
    keycode_scancode [0x59] = SCANCODE_Y;
    keycode_scancode [0x55] = SCANCODE_U;
    keycode_scancode [0x49] = SCANCODE_I;
    keycode_scancode [0x4F] = SCANCODE_O;
    keycode_scancode [0x50] = SCANCODE_P;
    keycode_scancode [0x5B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x7B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x5D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x7D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x0D] = SCANCODE_ENTER;
    keycode_scancode [0x41] = SCANCODE_A;
    keycode_scancode [0x53] = SCANCODE_S;
    keycode_scancode [0x44] = SCANCODE_D;
    keycode_scancode [0x46] = SCANCODE_F;
    keycode_scancode [0x47] = SCANCODE_G;
    keycode_scancode [0x48] = SCANCODE_H;
    keycode_scancode [0x4A] = SCANCODE_J;
    keycode_scancode [0x4B] = SCANCODE_K;
    keycode_scancode [0x4C] = SCANCODE_L;

    keycode_scancode [0x3A] = SCANCODE_SEMICOLON;
    keycode_scancode [0x3B] = SCANCODE_SEMICOLON;

    keycode_scancode [0x27] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x22] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x60] = SCANCODE_GRAVE;
    keycode_scancode [0x7E] = SCANCODE_GRAVE;

    keycode_scancode [0x20] = SCANCODE_SPACE;

    keycode_scancode [0x5C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x7C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x5A] = SCANCODE_Z;
    keycode_scancode [0x58] = SCANCODE_X;
    keycode_scancode [0x43] = SCANCODE_C;
    keycode_scancode [0x56] = SCANCODE_V;
    keycode_scancode [0x42] = SCANCODE_B;
    keycode_scancode [0x4E] = SCANCODE_N;
    keycode_scancode [0x4D] = SCANCODE_M;
    keycode_scancode [0x2C] = SCANCODE_COMMA;
    keycode_scancode [0x3C] = SCANCODE_COMMA;
    keycode_scancode [0x2E] = SCANCODE_PERIOD;
    keycode_scancode [0x3E] = SCANCODE_PERIOD;
    keycode_scancode [0x2F] = SCANCODE_SLASH;
    keycode_scancode [0x3F] = SCANCODE_SLASH;

    keycode_scancode [0x09] = SCANCODE_PRINTSCREEN;
    keycode_scancode [0x06] = SCANCODE_INSERT;
    keycode_scancode [0x7F] = SCANCODE_REMOVE;
}

static unsigned int keycode_to_scancode (unsigned int keycode)
{
    if(keycode > 255) //not ascii
    {
        switch (keycode) {
            case (61697) ... (61706):
                return SCANCODE_F1 + keycode - 61695;
            case 61707:
                return SCANCODE_F11;
            case 61708:
                return SCANCODE_F12;
            case 61953:
                return SCANCODE_LEFTSHIFT;
            case 61954:
                return SCANCODE_LEFTCONTROL;
            case 61956:
                return SCANCODE_LEFTALT;
            case 62208:
                return SCANCODE_CAPSLOCK;
            case 62209:
                return SCANCODE_NUMLOCK;
            case 62210:
                return SCANCODE_SCROLLLOCK;
            case 61450:
                return SCANCODE_BREAK;
            case 61440:
                return SCANCODE_CURSORBLOCKLEFT;
            case 61441:
                return SCANCODE_CURSORBLOCKRIGHT;
            case 61442: 
                return SCANCODE_CURSORBLOCKUP;
            case 61443: 
                return SCANCODE_CURSORBLOCKDOWN;
            case 61445:
                return SCANCODE_HOME;
            case 61446:
                return SCANCODE_END;
            case 61444:
                return SCANCODE_INSERT;
            case 61447:
                return SCANCODE_PAGEUP;
            case 61448:
                return SCANCODE_PAGEDOWN;
            default:
                break;
        }
    }
    if(keycode >= 'a'  && keycode <= 'z') {
        keycode = keycode -  ('a' -'A');
    }

    unsigned int scancode = keycode_scancode [keycode];
    return scancode;
}
static int keyboard_update (void)
{
    return NR_KEYS;
}

static const char *keyboard_get_state(void)
{
    return (char *)state;
}


#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    int retvalue = 0;
    //DFBEvent evt;
	DFBInputEvent evt;

	if (evtbuf->WaitForEventWithTimeout( evtbuf, 10, 0 ) == DFB_TIMEOUT) 
	{
		return 0;
	    //sleep( 1 );
    }
	else{
        /* TODO: support mouse right button */
        DFBInputDeviceButtonMask mouse_mask = 0;
		int res = evtbuf->GetEvent(evtbuf, DFB_EVENT(&evt));
		if(res == DFB_OK)
		{
			//fprintf(stderr, "**********evt.axis=%d********\n", evt.axis);
		}
        
        if (mouse_device != NULL) 
            mouse_device->GetButtons(mouse_device, &mouse_mask);

		//FD_CLR (dev_fd, in);
		switch (evt.type) {
			case DIET_KEYPRESS:
                {
                    curkey = evt.key_symbol;
                    int pckey = keycode_to_scancode(curkey);
                    if(pckey) {
                        state[pckey] = 1;
                    }
                    retvalue |= IAL_KEYEVENT;
                }
				break;
			case DIET_KEYRELEASE:
                {
                    curkey = evt.key_symbol;
                    int pckey = keycode_to_scancode(curkey);
                    if(pckey) {
                        state[pckey] = 0;
                    }

                    retvalue |= IAL_KEYEVENT;
                }
				break;
			case DIET_AXISMOTION:
			case DIET_BUTTONPRESS:
			case DIET_BUTTONRELEASE:
				if (evt.flags & DIEF_AXISABS) {
					switch (evt.axis) {
						case DIAI_X:
							mouse_x = evt.axisabs;
							break;
						case DIAI_Y:
							mouse_y = evt.axisabs;
							break;
						case DIAI_Z:
							fprintf (stderr, "Z axis (abs): %d", evt.axisabs);
							break;
						default:
							fprintf (stderr, "Axis %d (abs): %d", evt.axis, evt.axisabs);
							break;
					 }
			   } else if (evt.flags & DIEF_AXISREL) {
				   switch (evt.axis) {
					   case DIAI_X:
						   mouse_x += evt.axisrel;
						   if(mouse_x > screen_width)
							   mouse_x = screen_width;
						   if(mouse_x < 0)
							   mouse_x = 0;
						   break;
					   case DIAI_Y:
						   mouse_y += evt.axisrel;
						   if(mouse_y > screen_height)
							   mouse_y = screen_height;
						   if(mouse_y < 0)
							   mouse_y = 0;
						   break;
					   case DIAI_Z:
						   fprintf (stderr, "Z axis (rel): %d", evt.axisrel);
						   break;
					   default:
						   fprintf (stderr, "Axis %d (rel): %d", evt.axis, evt.axisrel);
						   break;
					 }
				}
			   
			   if((DIET_BUTTONPRESS == evt.type) || (DIET_BUTTONRELEASE == evt.type))
			   {
#if 0
				   if((IAL_MOUSE_LEFTBUTTON | IAL_MOUSE_RIGHTBUTTON) == mouse_button) {
					   mouse_button = 0;
                   } else {
                       if (mouse_mask == DIBM_RIGHT)
					       mouse_button = IAL_MOUSE_RIGHTBUTTON;
                       else
					       mouse_button = IAL_MOUSE_LEFTBUTTON;
                   }
#endif
                   if(IAL_MOUSE_LEFTBUTTON == mouse_button)
                       mouse_button = 0;
                   else
                       mouse_button = IAL_MOUSE_LEFTBUTTON;
			   }
				retvalue |= IAL_MOUSEEVENT;
				break;
		}
    }
    return retvalue;
}
static DFBEnumerationResult
enum_input_device( DFBInputDeviceID device_id, DFBInputDeviceDescription desc, void *data )
{
	DeviceInfo **devices = data;
	DeviceInfo  *device; 
	device = malloc( sizeof(DeviceInfo) ); 
	device->device_id = device_id;
	device->desc      = desc;
	device->next      = *devices;
	*devices = device; 
	return DFENUM_OK;
}


BOOL InitDFBInput (INPUT* input, const char* mdev, const char* mtype)
{
    init_code_map();
    DFBResult ret;
    DeviceInfo *device = NULL;

	//DeviceInfo *devices = NULL; 
	//__mg_dfb->EnumInputDevices( __mg_dfb, enum_input_device, &devices );
	__mg_dfb->EnumInputDevices( __mg_dfb, enum_input_device, &devices_info );
    ret = __mg_dfb->CreateInputEventBuffer(__mg_dfb, DICAPS_ALL, DFB_FALSE, &evtbuf); 
    if (ret) {
        fprintf(stderr, "CreateInputEventBuffer error\n");
        return FALSE;
    }

    /* get dfb mouse device */
    device = devices_info; 
    while (device != NULL) {
        if (device->desc.type == DIDTF_MOUSE) {
            __mg_dfb->GetInputDevice(__mg_dfb, device->device_id, mouse_device);
            break;
        }

        device = device->next; 
    }

	pFrameBuffer->GetSize(pFrameBuffer, &screen_width, &screen_height);

    memset (state, 0, sizeof (state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermDFBInput (void)
{
    DeviceInfo  *device = devices_info; 
    while (devices_info != NULL) {
        device = devices_info;
        devices_info = devices_info->next; 
        free(device);
    }
}

#endif /* _MGIAL_DFB */
