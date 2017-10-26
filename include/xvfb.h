/**
 * \file xvfb.h
 * \author FMSoft
 * \date 2007/10/25
 * 
 * This file includes global and miscellaneous interfaces of xVFB for MiniGUI.
 *
 \verbatim

    Copyright (C) 2002-2012 FMSoft.
    Copyright (C) 1998-2002 Wei Yongming.

    All rights reserved by FMSoft (http://www.fmsoft.cn).

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: xvfb.h 7682 2007-09-26 03:27:02Z wangxuguang $
 * 
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2012 FMSoft.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_XVFB_H
  #define _MGUI_XVFB_H


#ifdef __cplusplus
extern "C" {
#endif  /*__cplusplus*/

    /**
     * \addtogroup global_vars Global variables
     * @{
     */

    /**
     * \defgroup xvfb_vars x virtual framebuffer 
     * @{
     */

/** infomation header of xvfb frame buffer. */    
typedef struct _XVFBHeader {

    /** size of infomation header*/
    unsigned int info_size;

    /** width of x virtual framebuffer */
    int width;

    /** height of x virtual framebuffer */
    int height;

    /** color depth of x virtual framebuffer */
    int depth;

    /** 
     * The flag indicating the Most Significant Bits (MSB)
     * is left when depth is less than 8. 
     */
    Uint8  MSBLeft;

    /** the pixel red color mask for x virtual framebuffer. */
    Uint32 Rmask;
 
    /** the pixel green color mask for x virtual framebuffer. */
    Uint32 Gmask;

    /** the pixel blue color mask for x virtual framebuffer. */
    Uint32 Bmask;

    /** the pixel alpha mask for x virtual framebuffer. */
    Uint32 Amask;

    /** The pitch of x virtual framebuffer. */
    int pitch;

    /** 
     * Flag indicate whether x virtual framebuffer should be update,
     * and should reset to false after refreshing the dirty area 
     */
    int dirty;   

    /** the left of dirty area */
    int dirty_rc_l;
    /** the top of dirty area */
    int dirty_rc_t;
    /** the right of dirty area */
    int dirty_rc_r;
    /** the bottom of dirty area */
    int dirty_rc_b;

    /** 
     * Flag indicate whether palette have been changed, 
     * and should reset to false after reflecting the change.
     */
    int palette_changed;  

    /** the offset off palette based on address of XVFBHeader. */
    int palette_offset;

    /** the offset off framebuffer based on address of XVFBHeader. */
    int fb_offset;
} XVFBHeader;


/** data of keyboard event */
typedef struct _XVFBKEYDATA
{
    /** the scancode in minigui of the keyboard. */
    unsigned short key_code;

    /** 
     * the keyboard state
     * - 0  pressed
     * - 1  released
     */ 
    unsigned short key_state;
} XVFBKEYDATA;

/** data of mouse event */
typedef struct _XVFBMOUSEDATA
{
    /** x coordinate of mouse */
    unsigned short x;
    
    /** y coordinate of mouse */
    unsigned short y;

    /** 
     * buttons pressed of mouse, can be OR'ed by following values: 
     * - 0x0001 left button pressed
     * - 0x0002 right button pressed  
     */
    unsigned short btn;
} XVFBMOUSEDATA;


/** event sended to minigui by x virtual framebuffer*/
typedef struct _XVFBEVENT
{
    /** 
     * - 0 the event is mouse event
     * - 1 the event is keyboard event  
     */
    int event_type;

    union {
        /** keyboard event data*/
        XVFBKEYDATA kb_data;
        /** mouse event data*/
        XVFBMOUSEDATA mouse_data;
    };
} XVFBEVENT;


/** the entry in palette*/
typedef struct _XVFBPalEntry {
    /** red of color in palette*/
    unsigned char r;
    /** gree of color in palette*/
    unsigned char g;
    /** blue of color in palette*/
    unsigned char b;
    /** alpha weight of color in palette*/
    unsigned char a;
} XVFBPalEntry;


/**
 * \var XVFBHeader* __mg_rtos_xvfb_event_buffer
 * \brief shared memory header of x virtual framebuffer.
 */
extern MG_EXPORT XVFBHeader* __mg_rtos_xvfb_header;

/**
 * \var void* __mg_rtos_xvfb_event_buffer
 * \brief event circular buffer
 *
 * x virtual framebuffer put event to it,
 * and minigui get event from it.
 */
extern MG_EXPORT void* __mg_rtos_xvfb_event_buffer;
    /** @} end of xvfb_vars */

    /** @} end of global_vars */


/**
 * \fn XVFBHeader* xVFBAllocVirtualFrameBuffer ( \
 *                  int width, int height, int depth, \
 *                   Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, \
 *                   BOOL MSBLeft)
 * \brief create virtual framebuffer
 * \param width the width of virtual framebuffer 
 * \param height the height of virtual framebuffer
 * \param depth the color depth of virtual framebuffer
 * \param Rmask the pixel red color mask
 * \param Gmask the pixel green color mask
 * \param Bmask the pixel blue color mask
 * \param Amask the pixel alpha mask
 * \param MSBLeft The flag indicating the Most Significant Bits (MSB) \
 *                  is left when depth is less than 8. 
 * \return the header of virtual framebuffer  
 */
MG_EXPORT XVFBHeader* GUIAPI xVFBAllocVirtualFrameBuffer (int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, BOOL MSBLeft);

/**
 * \fn void GUIAPI xVFBFreeVirtualFrameBuffer (XVFBHeader* buf)
 * \brief free virtual framebuffer.
 *
 * \param buf The pointer to virtual framebuffer.
 */
MG_EXPORT void GUIAPI xVFBFreeVirtualFrameBuffer (XVFBHeader* buf);

/**
 * \fn int xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event)
 * \brief create event circular buffer.
 * \param nr_events capability of the event buffer
 * \return the address of event circular buffer.
 */
MG_EXPORT void* GUIAPI xVFBCreateEventBuffer (int nr_events);


/**
 * \fn void GUIAPI xVFBDestroyEventBuffer (void* event_buf)
 * \brief destroy event circular buffer.
 * \param event_buf The pointer to event circular buffer.
 */
MG_EXPORT void GUIAPI xVFBDestroyEventBuffer (void* event_buf);

/**
 * \fn int xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event)
 * \brief put a event to event circular buffer
 * \param xvfb_event_buffer event circular buffer 
 * \param event event shall be put into xvfb_event_buffer
 * \return 
 *      - 0 indicate the event has been put into buffer
 *      - 1 indicate the buffer is full
 *      - 2 the buffer has been distroyed (MiniGUI terminated).
 */
MG_EXPORT int GUIAPI xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event);


#ifdef __cplusplus
}
#endif

#endif
