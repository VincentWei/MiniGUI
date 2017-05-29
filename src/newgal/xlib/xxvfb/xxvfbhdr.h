#ifndef _XXVFBHDR_H
#define _XXVFBHDR_H
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>


#define MOUSE_TYPE   0
#define KB_TYPE      1
#define CAPTION_TYPE 2
#define IME_TYPE     3
#define IME_MESSAGE_TYPE 4
#define SHOW_HIDE_TYPE   5
#define XXVFB_CLOSE_TYPE  0xFFFFFFFF

typedef struct _XXVFbHeader
{
	unsigned int info_size;

    int width;
    int height;
    int depth;
    int pitch;

    int dirty;
	int dirty_rc_l;
	int dirty_rc_t;
	int dirty_rc_r;
	int dirty_rc_b;

	int palette_changed;
	int palette_offset;

	int fb_offset;

    int MSBLeft;
    
    int Rmask;
    int Gmask;
    int Bmask;
    int Amask;
}XXVFBHeader;

typedef struct _XXVFBPalEntry {
    unsigned char r, g, b, a;
} XXVFBPalEntry;

typedef struct _XXVFBInfo
{
    Display *display;
    Window win;
    GC gc;
    Visual *visual;
    Colormap colormap;
    int dev_depth;
    int sockfd;

    XXVFBHeader* xhdr;
}XXVFBInfo;

typedef struct _XXVFBKeyData
{
    unsigned short key_code;
    unsigned short key_state;
} XXVFBKeyData;

typedef struct _XXVFBMouseData
{
    unsigned short x;
    unsigned short y;
    unsigned int   button;
} XXVFBMouseData;

typedef struct _XXVFBEventData
{
    int event_type;
    union 
    {
        XXVFBKeyData key;
        XXVFBMouseData mouse;
    } data;
} XXVFBEventData;

#endif /* _XXVFBHDR_H */
