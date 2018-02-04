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
 *          MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *          pSOS, ThreadX, NuCleus, OSE, and Win32.
 */


#include <minigui/common.h>

#ifdef __VXWORKS__

#ifdef _COMM_IAL

#if defined (__TARGET_VXI386__) || defined (__TARGET_PPC__)

static int ascii_to_scancode(int asc2);

static short scancode_table[SCANCODE_USER]=
{ 
        0,		         //0
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        SCANCODE_BACKSPACE,	//8
        SCANCODE_TAB,
        0,
        0,
        0,
        SCANCODE_ENTER,		//13
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        SCANCODE_ESCAPE,	//27
        0,
        0,
        0,
        0,
        SCANCODE_SPACE,
        SCANCODE_1, /*sign (!) 33 */
        SCANCODE_APOSTROPHE, /*sign (")*/
        SCANCODE_3, /*sign(#)*/
        SCANCODE_4, /*sign($)*/
        SCANCODE_5, /*sign(%)*/
        SCANCODE_7, /*sign (&)*/
        SCANCODE_APOSTROPHE, //sign (')
        SCANCODE_9, /*sign (()*/
        SCANCODE_0, /*sign ())*/
        SCANCODE_8, /* sign(*) */
        SCANCODE_EQUAL, /*sign (+)*/
        SCANCODE_COMMA,			//44,
        SCANCODE_MINUS,     	//45
        SCANCODE_PERIOD,
        SCANCODE_SLASH, // /
        SCANCODE_0,		//48                     
        SCANCODE_1,		//49     
        SCANCODE_2,		//50
        SCANCODE_3,		//51
        SCANCODE_4,             //52         
        SCANCODE_5,		//53                    
        SCANCODE_6,		//54
        SCANCODE_7,		//55                      
        SCANCODE_8,		//56                      
        SCANCODE_9,		//57
        SCANCODE_SEMICOLON,	//58:
        SCANCODE_SEMICOLON,	//59;
        0,
        SCANCODE_EQUAL,		//61
        0,
        0,
        SCANCODE_2,     /* @ */
        SCANCODE_A,		//65
        SCANCODE_B,
        SCANCODE_C,
        SCANCODE_D,
        SCANCODE_E,		
        SCANCODE_F,
        SCANCODE_G,
        SCANCODE_H,
        SCANCODE_I,	
        SCANCODE_J,
        SCANCODE_K,
        SCANCODE_L,
        SCANCODE_M,
        SCANCODE_N,
        SCANCODE_O,		
        SCANCODE_P,		                    
        SCANCODE_Q,		
        SCANCODE_R,		
        SCANCODE_S,
        SCANCODE_T,		
        SCANCODE_U,		
        SCANCODE_V,
        SCANCODE_W,		
        SCANCODE_X,
        SCANCODE_Y,		
        SCANCODE_Z,		//90
        SCANCODE_BRACKET_LEFT,  /* [ */
        SCANCODE_BACKSLASH,  /* \ */
        SCANCODE_BRACKET_RIGHT, /* ] */
        SCANCODE_6,
        SCANCODE_MINUS,     	/* _ (95)*/
        0,
        SCANCODE_A,		//97
        SCANCODE_B,
        SCANCODE_C,
        SCANCODE_D,
        SCANCODE_E,		
        SCANCODE_F,
        SCANCODE_G,
        SCANCODE_H,
        SCANCODE_I,	
        SCANCODE_J,
        SCANCODE_K,
        SCANCODE_L,
        SCANCODE_M,
        SCANCODE_N,
        SCANCODE_O,		
        SCANCODE_P,		                    
        SCANCODE_Q,		
        SCANCODE_R,		
        SCANCODE_S,
        SCANCODE_T,		
        SCANCODE_U,		
        SCANCODE_V,
        SCANCODE_W,		
        SCANCODE_X,
        SCANCODE_Y,		
        SCANCODE_Z	,	//122
};

//0xE000
static short scancode_private1_table[22] = 
{
        SCANCODE_HOME,
        SCANCODE_END,
        SCANCODE_INSERT,
        SCANCODE_PAGEUP,
        SCANCODE_PAGEDOWN,
        SCANCODE_CURSORBLOCKLEFT,
        SCANCODE_CURSORBLOCKRIGHT,
        SCANCODE_CURSORBLOCKUP,
        SCANCODE_CURSORBLOCKDOWN,
        SCANCODE_PRINTSCREEN,
        SCANCODE_PAUSE,
        SCANCODE_CAPSLOCK,
        SCANCODE_NUMLOCK,
        SCANCODE_SCROLLLOCK,

        SCANCODE_LEFTSHIFT,
        SCANCODE_RIGHTSHIFT,
        SCANCODE_LEFTCONTROL,
        SCANCODE_RIGHTCONTROL,
        SCANCODE_LEFTALT,
        SCANCODE_RIGHTALT,
        0,
        0,
};

//0xEf00
static short scancode_private2_table[13] = 
{
        0,//0
        SCANCODE_F1,
        SCANCODE_F2,
        SCANCODE_F3,
        SCANCODE_F4,
        SCANCODE_F5,
        SCANCODE_F6,
        SCANCODE_F7,
        SCANCODE_F8,
        SCANCODE_F9,
        SCANCODE_F10,
        SCANCODE_F11,
        SCANCODE_F12,
};

static int ascii_to_scancode (int asc2)
{
    unsigned int ret, index=0;
    
    ret = (asc2) & 0xef00;
    index = asc2 & 0x00ff;
    
    if (ret==0) {
        return scancode_table[index]; 
    } else if (ret == 0xe000) {
        return scancode_private1_table [index];
    } else if (ret == 0xef00) {
        return scancode_private2_table[index];
    }      
    return 0;
}

#endif

#ifdef __TARGET_VXI386__

/*WINML3.0 reference*/
#include <ugl/uglinput.h>
#include <ugl/uglugi.h>
#include <ugl/ugl.h>
#include <ugl/uglucode.h>

#define VX_MOUSEINPUT    0x01
#define VX_KBINPUT       0x02

static UGL_INPUT_SERVICE_ID inputServiceId;
static UGL_DEVICE_ID devId;
static UGL_MSG msg;
static UGL_GC_ID gc;	
static UGL_INPUT_DEV_ID pointerDevId;
static UGL_INT32 pdType;
static UGL_CDDB_ID cursorDdb;    

int __comminput_init (void)
{
    devId = (UGL_DEVICE_ID)uglRegistryFind (UGL_DISPLAY_TYPE, 0, 0,0)->id;
    inputServiceId = (UGL_INPUT_SERVICE_ID)uglRegistryFind (
            UGL_INPUT_SERVICE_TYPE,  0, 0,0)->id;
    return 0;
}

int __comminput_ts_getdata (short *x, short *y, short *button)
{   
   	*x = msg.data.pointer.position.x;
   	*y = msg.data.pointer.position.y;

    *x = *x<1024 ? *x : 1024;
    *y = *y<768 ? *y : 768;
    
   	*button = (msg.data.pointer.buttonState) ;
    return 0;
}

int __comminput_kb_getdata (short *key, short *status)
{
   *key = ascii_to_scancode (msg.data.keyboard.key);
   if (msg.data.keyboard.modifiers & UGL_KBD_KEYDOWN)
   	   *status = 1;
   else
   	   *status = 0;
   
   return 0; 
}

int __comminput_wait_for_input (void)
{
    UGL_STATUS status;
    __mg_os_time_delay(10);
    
    status = uglInputMsgGet (inputServiceId, &msg, UGL_NO_WAIT);
    
    if (status != UGL_STATUS_Q_EMPTY)
    {
        if (msg.type == MSG_KEYBOARD){
            msg.type = 0;
            return VX_KBINPUT;
        }else if (msg.type == MSG_POINTER) {
    	    msg.type = 0;
            return VX_MOUSEINPUT;
        }
    }
    return 0;
}

void __comminput_deinit (void)
{
	uglGcDestroy (gc);
    uglDeinitialize();
}

#elif defined (__TARGET_PPC__) /*__TARGET_VXI386__*/

/*WINML2.0 reference*/
#include <ugl/uglinput.h>
#include <ugl/uglugi.h>
#include <ugl/ugl.h>
#include <ugl/uglucode.h>

#define VX_MOUSEINPUT    0x01
#define VX_KBINPUT       0x02

static UGL_EVENT_SERVICE_ID eventServiceId;
static UGL_INPUT_EVENT *pInputEvent;
static UGL_EVENT event;

int __comminput_init (void)
{
    if (uglDriverFind (UGL_EVENT_SERVICE_TYPE, 0, 
            (UGL_UINT32 *)&eventServiceId) != UGL_STATUS_OK) {
        return 1; 
    }

    return 0;
}

int __comminput_ts_getdata (short *x, short *y, short *button)
{   
    int tmp;

   	*x = pInputEvent->x;
   	*y = pInputEvent->y;

    *x<1024 ? *x : 1024 ;
    *y<768 ? *y : 768 ;
    
   	*button = (pInputEvent->type.pointer.buttonState) ;

    return 0;
}

int __comminput_kb_getdata (short *key, short *status)
{
   *key = ascii_to_scancode (pInputEvent->type.keyboard.key);
   if (pInputEvent->modifiers & UGL_KEYBOARD_KEYDOWN)
   	   *status = 1;
   else
   	   *status = 0;
   
   return 0; 
}

int __comminput_wait_for_input (void)
{
    UGL_STATUS status;
    __mg_os_time_delay(10);
    
    status = uglInputEventGet (eventServiceId, UGL_NO_WAIT, 
            (UGL_EVENT *)pInputEvent, sizeof (UGL_EVENT));
    
    if (status == UGL_STATUS_OK) {
        if (pInputEvent->header.type == UGL_EVENT_TYPE_KEYBOARD){
            pInputEvent->header.type = 0;
            return VX_KBINPUT;
        }else if (pInputEvent->header.type == UGL_EVENT_TYPE_POINTER) {
            pInputEvent->header.type = 0;
            return VX_MOUSEINPUT;
        }
    }
    return 0;
}

void __comminput_deinit (void)
{
    uglDeinitialize();
}

#else /*__TARGET_PPC__*/

/* Please implemente the following functions if your MiniGUI is 
 * configured to use the comm IAL engine */

#define COMM_MOUSEINPUT    0x01  // mouse event
#define COMM_KBINPUT       0x02  // keyboard event

/*
 * Initialize the input device here.
 *
 * return zero on success, non-zero on error.
 */
int __comminput_init (void)
{
    /* TODO */
    return 0;
}

/*
 * Waits for input for keyboard and touchpanel. 
 * If no data, this function should go into sleep;
 * when data is available, keyboard or touchpanel driver should wake up
 * the task/thread in MiniGUI who call __comminput_wait_for_input.
 *
 * Normal implementation makes this function sleep on a RTOS event object, 
 * such as semaphore, and then returns COMM_MOUSEINPUT or COMM_KBINPUT 
 * according to type of the input event.
 */
int __comminput_wait_for_input (void)
{
    /* TODO */
    return 0;
}

/*
 * Gets touchpanel position and button data.
 * x, y   : position values
 * button : Non-zero value means pen is down.
 */
int __comminput_ts_getdata (short *x, short *y, short *button)
{
    /* TODO */
    return 0;
}

/*
 * Gets keyboard key data.
 * key        : return MiniGUI scancode of the key.
 * key_status : key down or up, non-zero value means down.
 */
int __comminput_kb_getdata (short *key, short *key_status)
{
    /* TODO */
    return 0;
}

/*
 * De-initialize the input device here.
 * return: void.
 */
void  __comminput_deinit (void)
{
    /* TODO */
    return;
}

#endif /* for unknown targets */

#endif /* _COMM_IAL */

/*------------- Interfaces for COMMON LCD NEWGAL engine  ------------------- */
#ifdef _MGGAL_COMMLCD

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332    1
#define COMMLCD_TRUE_RGB555      2

#define COMMLCD_TRUE_RGB565      3
#define COMMLCD_TRUE_RGB888      4
#define COMMLCD_TRUE_RGB0888     5

/* Please implemente the following functions if your MiniGUI is 
 * configured to use the comm IAL engine */

struct commlcd_info {
    short height, width;  // Pixels
    short bpp;            // Depth (bits-per-pixel)
    short type;           // Pixel type
    short rlen;           // Length of one raster line in bytes
    void  *fb;            // Address of the frame buffer
};

#ifdef __TARGET_VXI386__

#include <ugl/uglugi.h>
#include <ugl/uglinfo.h>
#include <ugl/ugl.h>

static UGL_DEVICE_ID devId;
#ifndef _COMM_IAL
static UGL_GC_ID gc;
#endif
    
int __commlcd_drv_init (void)
{
    if (uglInitialize() == UGL_STATUS_ERROR)
        return 1;
    
    return 0; 
}

int __commlcd_drv_getinfo (struct commlcd_info *li)
{
    UGL_MODE_INFO modeInfo;

    
    /* Obtain display device identifier */
    devId = (UGL_DEVICE_ID) uglRegistryFind (UGL_DISPLAY_TYPE,  
                    0, 0, 0)->id;
   
    /* Create a graphics context */
    gc = uglGcCreate (devId);
    
#ifndef _VESA_SUPPORT
    uglInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);
    li->type = COMMLCD_PSEUDO_RGB332;
#else
    uglPcBiosInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);
    li->type = COMMLCD_TRUE_RGB565;
#endif

    li->height = modeInfo.height;
    li->width = modeInfo.width;
    li->fb = modeInfo.fbAddress;
    li->bpp = modeInfo.colorDepth;
    li->rlen = (li->bpp*li->width + 7) / 8;
    return 0;
}

int __commlcd_drv_release (void)
{
    /* TODO: release your LCD device */
    return 0;
}

#ifndef _VESA_SUPPORT    
int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    int i;
   
    UGL_ARGB argb [256];
    if (ncolors > 256) ncolors = 256;
    
    for (i = 0; i < ncolors; i++)
        argb [i] = UGL_MAKE_RGB (colors[i].r, colors[i].g, colors[i].b);
    
    uglClutSet (devId, firstcolor, argb, ncolors);
    return 1;
}
#else
int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    return 0;
}
#endif

#elif defined (__TARGET_PPC__) /* __TARGET_VXI386 */

#include <ugl/uglugi.h>
#include <ugl/uglinfo.h>
#include <ugl/ugl.h>

static UGL_DEVICE_ID devId;
#ifndef _COMM_IAL
static UGL_GC_ID gc;
#endif
    
int __commlcd_drv_init (void)
{
    if (uglInitialize() == UGL_STATUS_ERROR)
        return 1;
    
    return 0; 
}

int __commlcd_drv_getinfo (struct commlcd_info *li)
{
    UGL_MODE_INFO modeInfo;

    
    /* Obtain display device identifier */
    uglDriverFind (UGL_DISPLAY_TYPE, 0, (UGL_UINT32 *)&devId);
   
    /* Create a graphics context */
    gc = uglGcCreate (devId);
    
    uglInfo (devId, UGL_MODE_INFO_REQ, &modeInfo);

    li->height = modeInfo.height;
    li->width = modeInfo.width;
    li->fb = modeInfo.fbAddress;
    //li->bpp = modeInfo.colorDepth;
    li->bpp = 32;
    printf ("Current color depth is %d \n", modeInfo.colorDepth);
    li->rlen = (li->bpp*1024 + 7) / 8;
    return 0;
}

int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    /* TODO: set hardware pallete if your LCD is 8-bpp. */
    return 0;
}

int __commlcd_drv_release (void)
{
    uglGcDestroy (gc); 
    uglDeinitialize();
    return 0;
}

#else /* __TARGET_PPC__ */

int __commlcd_drv_init (void)
{
    /* TODO: Do LCD initialization here, if you have not. */ 
    return 0;
}

/* This should be a pointer to the real framebuffer of your LCD */
static char a_fb [320*240*2];

int __commlcd_drv_getinfo (struct commlcd_info *li)
{
    /* TODO: 
     * Set LCD information in a commlcd_info structure pointed by li
     * according to properties of your LCD.
     */
	li->width  = 320;
	li->height = 240;
	li->bpp    = 16;
	li->type   = 0;
	li->rlen   = 320*2;
	li->fb     = a_fb;

    return 0;
}

int __commlcd_drv_release (void)
{
    /* TODO: release your LCD device */
    return 0;
}

int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors)
{
    /* TODO: set hardware pallete if your LCD is 8-bpp. */
    return 0;
}

#endif /* for unknown targets */

#endif /* _MGGAL_COMMLCD */

#include <semLib.h>

#ifdef _MGUSE_OWN_MALLOC
/* use a static array as the heap */
#define HEAPSIZE  (1024*1024*3)
static unsigned char __vxworks_heap [HEAPSIZE];
static unsigned int __vxworks_heap_size = HEAPSIZE;

static int __vxworks_heap_lock (void)
{
    /* TODO */
}

static int __vxworks_heap_unlock (void)
{
    /* TODO */
}

#endif

/* ------------------- Application entry for VxWorks -------------- */

/* MiniGUI application entry defined in other module. */
extern int minigui_entry (int argc, const char* argv []);

#ifdef _MGUSE_OWN_PTHREAD
/* the stack for main thread */
static char main_stack [MAIN_PTH_DEF_STACK_SIZE];
#endif

void minigui_app_entry (void)
{
    /* TODO: do some OS initialization here */

#ifdef _MGUSE_OWN_STDIO
    /*
     * Initialize MiniGUI's own printf system firstly.
     */
    init_minigui_printf (putchar, getchar);
#endif

#if _MGUSE_OWN_MALLOC
    /*
     * Initialize MiniGUI's heap memory management module secondly.
     */

    /* TODO: call VxWorks system call to create a mutex. */
    if (init_minigui_malloc (__vxworks_heap, __vxworks_heap_size, 
            __vxworks_heap_lock, __vxworks_heap_unlock)) {
        fprintf (stderr, "Can not init MiniGUI heap system.\n");
        return;
    }
#endif

#ifdef _MGUSE_OWN_PTHREAD
    /*
     * Initialize MiniGUI's POSIX thread module and call minigui_entry thirdly.
     */
    if (start_minigui_pthread (minigui_entry, 0, NULL, 
            main_stack, MAIN_PTH_DEF_STACK_SIZE)) {
        fprintf (stderr, "Can not init MiniGUI's pthread implementation.\n");
        return;
    }
#else
    minigui_entry (0, 0);
#endif
}

#endif /* __VXWORKS__ */

