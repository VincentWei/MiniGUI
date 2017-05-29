/**
 * \file threadx_startup.c
 * \author Xiaowei Yan
 * \date 2005/01/15
 *
 \verbatim

    Copyright (C) 2005-2007 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: threadx_startup.c 8944 2007-12-29 08:29:16Z xwyan $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2005-2006 Feynman Software.
 */


#include <minigui/common.h>

#ifdef __THREADX__

/* --------------------- Interfaces for Common IAL Engine ----------------- */
#ifdef _COMM_IAL

#ifdef __TARGET_VFANVIL__

/* The following functions are defined for __TARGET_VFANVIL__ */

#define COMM_MOUSEINPUT    0x01  // mouse event
#define COMM_KBINPUT       0x02  // keyboard event

/*
 * Initialize the input device here.
 *
 * return zero on success, non-zero on error.
 */
int __comminput_init (void)
{
    tp_man_kp_init();
    return 0;
}

#define COMM_KBINPUT       0x02

#include "keypad.h"

static short keycode_convert_scancode (UINT8 keycode)
{
	short scancode = 0;
	
	switch (keycode) {
		case KBD_KEY_SEND:
			scancode = SCANCODE_F2;
	    	break;
   		case KBD_KEY_END:
   			scancode = SCANCODE_F1;
	    	break;
   		case KBD_KEY_REG:
   			scancode = SCANCODE_CURSORBLOCKUP;
	    	break;
	    case KBD_KEY_INFO_MODE:
	    	scancode = SCANCODE_CURSORBLOCKDOWN;
	    	break;
	    case KBD_KEY_CLEAR:
	    	scancode = SCANCODE_F3;
	    	break;
	    case KBD_KEY_CAMP:
	    	scancode = SCANCODE_ESCAPE;
	    	break;
	    case KBD_KEY_BLANK1:                
	    	scancode = SCANCODE_CURSORBLOCKLEFT;
	    	break;
	    case KBD_KEY_INFO_SELECT:
	    	scancode = SCANCODE_CURSORBLOCKRIGHT;
	    	break;
/*	    case KBD_KEY_POWER:
	    	scancode = SCANCODE_POWER;
	    	break;
	*/    case KBD_KEY_BLANK2:       
	    	scancode = SCANCODE_F6;
	    	break;
	    case KBD_KEY_1:
	    	scancode = SCANCODE_1;
	    	break;
	    case KBD_KEY_2:
	    	scancode = SCANCODE_2;
	    	break;
	    case KBD_KEY_3:
	    	scancode = SCANCODE_3;
	    	break;
/*	    case KBD_KEY_STAR:
	    	scancode = SCANCODE_F7;
	    	break;
	*/case KBD_KEY_UP: 
	    	scancode = SCANCODE_0;
	    	break;  
	case KBD_KEY_4:
	    	scancode = SCANCODE_4;
	    	break;
	    case KBD_KEY_5:
	    	scancode = SCANCODE_5;
	    	break;
	    case KBD_KEY_6:
	    	scancode = SCANCODE_6;
	    	break;
/*	    case KBD_KEY_0:
	    	scancode = SCANCODE_0;
	    	break;
	*/    case KBD_KEY_DOWN:
	    	scancode = SCANCODE_F7;
	    	break;
	    case KBD_KEY_7:
	    	scancode = SCANCODE_7;
	    	break;
	    case KBD_KEY_8:
	    	scancode = SCANCODE_8;
	    	break;
	    case KBD_KEY_9:
	    	scancode = SCANCODE_9;
	    	break;
/*	    case KBD_KEY_HASH:
	    	scancode = SCANCODE_CURSORBLOCKRIGHT;
	    	break;
	*/    case KBD_NO_KEY: //no key press ,timeout
	    	scancode = 0;
	    	break;
	}
	
	return scancode;
}

/*
 * Gets keyboard key data.
 * key        : return MiniGUI scancode of the key.
 * key_status : key down or up, non-zero value means down.
 */
int __comminput_kb_getdata (short *key, short *status)
{
	MAN_KP_INFO kbdinfo;
	
	if (tp_man_kp_get_data (&kbdinfo) == 0)
	{
		*key = keycode_convert_scancode (kbdinfo.scancode);
		*status = kbdinfo.status;
	}
	
	if (*key != 0)
		return 0;
	return -1;
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
	MAN_KP_INFO kbdinfo;
	
	if (tp_man_wait_input (30) == MAN_KPINPUT)
			return COMM_KBINPUT;

	return 0;
}

/*
 * Gets touchpanel position and button data.
 * x, y   : position values
 * button : Non-zero value means pen is down.
 */
int __comminput_ts_getdata (short *x, short *y, short *button)
{
	return 0;
}

/*
 * De-initialize the input device here.
 * return: void.
 */
void  __comminput_deinit (void)
{
    return;
}

#else /* __TARGET_VFANVIL__ */

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

#endif /* !__TARGET_VFANVIL__ */

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

#endif /* _MGGAL_COMMLCD */

/* Functions to read/write serial driver */
extern int serial_read (void);
extern int serial_write (int ch);

#endif /* __TARGET_VFANVIL__ */

#include "tx_api.h"

/* use a static array as the heap */
#define HEAPSIZE  (1024*1024*3)
static unsigned char __threadx_heap [HEAPSIZE];
static unsigned int __threadx_heap_size = HEAPSIZE;

static TX_MUTEX __threadx_malloc_mutex;

#define NAME_MUTEX_MALLOC   "Mutex4Malloc"

static int __threadx_heap_lock (void)
{
    UINT status;

    status = tx_mutex_get (&__threadx_malloc_mutex, TX_WAIT_FOREVER);
    if (status == TX_SUCCESS)
        return 0;

    return -1;
}

static int __threadx_heap_unlock (void)
{
    UINT status;

    status = tx_mutex_put (&__threadx_malloc_mutex);
    if (status == TX_SUCCESS)
        return 0;

    return -1;
}

/* ------------------- Application entry for ThreadX -------------- */

/* MiniGUI application entry defined in other module. */
extern int minigui_entry (int argc, const char* argv []);

/* the stack for main thread */
static char main_stack [MAIN_PTH_DEF_STACK_SIZE];

void minigui_app_entry (void)
{
    /* TODO: do some initialization here */

    /*
     * Initialize MiniGUI's own printf system firstly.
     */
#ifdef  __TARGET_VFANVIL__
    init_minigui_printf (serial_write, serial_read);
#endif

    /*
     * Initialize MiniGUI's heap memory management module secondly.
     */
    if (tx_mutex_create (&__threadx_malloc_mutex, NAME_MUTEX_MALLOC,
            TX_NO_INHERIT) != TX_SUCCESS) {
        fprintf (stderr, "Can not init mutex object for heap.\n");
        return;
    }

    if (init_minigui_malloc (__threadx_heap, __threadx_heap_size, 
            __threadx_heap_lock, __threadx_heap_unlock)) {
        fprintf (stderr, "Can not init MiniGUI heap system.\n");
        return;
    }

    /*
     * Initialize MiniGUI's POSIX thread module and call minigui_entry thirdly.
     */
    if (start_minigui_pthread (minigui_entry, 0, NULL, 
            main_stack, MAIN_PTH_DEF_STACK_SIZE)) {
        fprintf (stderr, "Can not init MiniGUI pthread system for ThreadX.\n");
        return;
    }
}

#endif /* __THREADX__ */
