/**
 * \file ucos2_startup.h
 * \author Wei Yongming (ymwei@minigui.org)
 * \date 2002/01/06
 * 
 \verbatim

    Copyright (C) 2002-2007 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: ucos2_startup.c 11749 2009-06-11 06:20:12Z weiym $
 * 
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2007 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#include <minigui/common.h>

#ifdef __UCOSII__

/* --------------------- Interfaces for Common IAL Engine ----------------- */
#ifdef _COMM_IAL

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

/* --------------------- I/O functions -------------------------- */
// for debug purpose

/* Gets a char from uart */
int drv_uart_get_byte (void)
{
    // TODO...
    return 'a';
}

/* Sends a char to uart */
int drv_uart_send_byte (int ch)
{
    // TODO...
    return 0;
}

/* --------------------- Heap management -------------------------- */
#ifdef __TARGET_S3C2410__

#include "2410addr.h"

#define HEAPEND  0x0FF00000

extern char Image$$RW$$Limit[];
static unsigned char* __ucos2_heap = Image$$RW$$Limit;
static unsigned int __ucos2_heap_size = HEAPEND - (unsigned int)__ucos2_heap;

#elif defined (__TARGET_S3C44B0X__)

#define HEAPEND  0x0c700000

extern unsigned int bottom_of_heap;     /* defined in heap.s */
static unsigned char* __ucos2_heap = (char*) bottom_of_heap;
static unsigned int __ucos2_heap_size = HEAPEND - bottom_of_heap;

#else

/* use a static array as the heap */

#define HEAPSIZE  (1024*1024*3)

static unsigned char __ucos2_heap [HEAPSIZE];
static unsigned int __ucos2_heap_size = HEAPSIZE;

#endif

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"

/* 
 * The mutex priority of the malloc.
 * the priority of this mutex should be higher than 16, which
 * is the highest task priority of MiniGUI threads in uC/OS-II.
 */
#define UCOS2_MALLOC_MUTEX_PRIO     15

static OS_EVENT* __ucos2_malloc_mutex;

static int __ucos2_heap_lock (void)
{
    INT8U err;

    OSMutexPend (__ucos2_malloc_mutex, 0, &err);

    if (err == OS_NO_ERR)
        return 0;

    return -1;
}

static int __ucos2_heap_unlock (void)
{
    OSMutexPost (__ucos2_malloc_mutex);
    return 0;
}

/* ------------------- Application entry for uC/OS-II -------------- */

/* MiniGUI application entry defined in other module. */
extern int minigui_entry (int argc, const char* argv []);

/* the stack for main thread */
static char main_stack [MAIN_PTH_DEF_STACK_SIZE];

void minigui_app_entry (void)
{
    INT8U err;

    /* TODO: do some initialization here */

    /*
     * Initialize MiniGUI's own printf system firstly.
     */
    init_minigui_printf (drv_uart_send_byte, drv_uart_get_byte);

    /*
     * Initialize MiniGUI's heap memory management module secondly.
     */
    __ucos2_malloc_mutex = OSMutexCreate (UCOS2_MALLOC_MUTEX_PRIO, &err);
    if (err != OS_NO_ERR) {
        fprintf (stderr, "Can not init mutex object for heap.\n");
        return;
    }

    if (init_minigui_malloc (__ucos2_heap, __ucos2_heap_size, 
            __ucos2_heap_lock, __ucos2_heap_unlock)) {
        fprintf (stderr, "Can not init MiniGUI heap system.\n");
        return;
    }

    /*
     * Initialize MiniGUI's POSIX thread module and call minigui_entry thirdly.
     */
    if (start_minigui_pthread (minigui_entry, 0, NULL, 
            main_stack, MAIN_PTH_DEF_STACK_SIZE)) {
        fprintf (stderr, "Can not init MiniGUI's pthread implementation.\n");
        return;
    }

    /* TODO: Call uC/OS-II OSStart here */
}

#endif /* __UCOSII__ */

