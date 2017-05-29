
#ifndef XVFB_H
#define XVFB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <minigui/common.h>

typedef struct _XVFBHeader {
    unsigned int info_size;
    int width;
    int height;
    int depth;

    /* The flag indicating the Most Significant Bits (MSB) 
     * is left when depth is less than 8. */
    Uint8  MSBLeft;

    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    int pitch;

    /* true for dirty, and should reset to false after refreshing the dirty area */
    int dirty;   
    int dirty_rc_l, dirty_rc_t, dirty_rc_r, dirty_rc_b;

    /* true for changed, and should reset to false after reflecting the change */
    int palette_changed;  
    int palette_offset;

    int fb_offset;
} XVFBHeader;

typedef struct _XVFBKEYDATA
{
    unsigned short key_code;
    unsigned short key_state;
} XVFBKEYDATA;

typedef struct _XVFBMOUSEDATA
{
    unsigned short x;
    unsigned short y;
    unsigned short btn;
} XVFBMOUSEDATA;

typedef struct _XVFBEVENT
{
    int event_type;
    union {
        XVFBKEYDATA kb_data;
        XVFBMOUSEDATA mouse_data;
    };
} XVFBEVENT;

typedef struct _XVFBPalEntry {
    unsigned char r, g, b, a;
} XVFBPalEntry;


extern XVFBHeader* __mg_rtos_xvfb_header;
extern void* __mg_rtos_xvfb_event_buffer;


/***
 * alloc virtual framebuffer
 */
XVFBHeader* xVFBAllocVirtualFrameBuffer (int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, BOOL MSBLeft);

/***
 * alloc event loop buffer
 */
void* xVFBCreateEventBuffer (int nr_events);

/***
 *notify new key and mouse event
 */
int xVFBNotifyNewEvent (const void* xvfb_event_buffer, XVFBEVENT* event);


#ifdef __cplusplus
}
#endif

#endif
