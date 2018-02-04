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
 * Todo:
 *  - reorganize code and funcionize some repeated code block.
 *  - Comment
 */ 

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <gtk/gtk.h>

#include <minigui/common.h>
#include <minigui/minigui.h>

#include <minigui/xvfb.h>
//#define USE_MSBLEFT 

static guchar *xvfb_buf;
static guchar *gtk_buf;

static guint32 *pal_entry;
static gint n_colors = 0;


static unsigned short translate_scancode (unsigned short keycode, BOOL is_numlock)
{
    switch (keycode)
    {
        case 9 ... 96:
            switch(keycode-8)
            {
                case SCANCODE_KEYPAD0:
                    if (is_numlock)
                        return SCANCODE_0;
                    else
                        return SCANCODE_INSERT;

                case SCANCODE_KEYPAD1:
                    if (is_numlock)
                        return SCANCODE_1;
                    else
                        return SCANCODE_END;

                case SCANCODE_KEYPAD2:
                    if (is_numlock)
                        return SCANCODE_2;
                    else
                        return SCANCODE_CURSORBLOCKDOWN;

                case SCANCODE_KEYPAD3:
                    if (is_numlock)
                        return SCANCODE_3;
                    else
                        return SCANCODE_PAGEDOWN;

                case SCANCODE_KEYPAD4:
                    if (is_numlock)
                        return SCANCODE_4;
                    else
                        return SCANCODE_CURSORBLOCKLEFT;

                case SCANCODE_KEYPAD5:
                    if (is_numlock)
                        return SCANCODE_5;
                    else
                        return SCANCODE_5;

                case SCANCODE_KEYPAD6:
                    if (is_numlock)
                        return SCANCODE_6;
                    else
                        return SCANCODE_CURSORBLOCKRIGHT;

                case SCANCODE_KEYPAD7:
                    if (is_numlock)
                        return SCANCODE_7;
                    else
                        return SCANCODE_HOME;

                case SCANCODE_KEYPAD8:
                    if (is_numlock)
                        return SCANCODE_8;
                    else
                        return SCANCODE_CURSORBLOCKUP;

                case SCANCODE_KEYPAD9:
                    if (is_numlock)
                        return SCANCODE_9;
                    else
                        return SCANCODE_PAGEUP;

                case SCANCODE_KEYPADPERIOD:
                    if (is_numlock)
                        return SCANCODE_PERIOD;
                    else
                        return SCANCODE_REMOVE;

                default:
                    return keycode-8;

            }

        case 97 ... 100:
            return keycode+5;
        case 102 ... 107:
            return keycode+4;
        case 108 ... 109:
            return keycode-12;
        case 110:
            return 119;
        case 112:
            return 98;
        case 113:
            return 100;
        case 115 ... 117:
            return keycode+10;

        default:
            return keycode;
    }
}

//int minigui_entry(int argc, char *argv[]);

static void* start_minigui_app (void* argv)
{
    minigui_entry (1, argv);
    return NULL;
}


int
on_darea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{       
    switch (__mg_rtos_xvfb_header->depth) {

        case 1: 
        case 2:
        case 4: 
        case 8: {
            GdkRgbCmap *cmap;

            cmap = gdk_rgb_cmap_new (pal_entry, 1<<__mg_rtos_xvfb_header->depth);                
            if (__mg_rtos_xvfb_header->depth < 8) {
                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        0,
                        0,
                        __mg_rtos_xvfb_header->width,
                        __mg_rtos_xvfb_header->height,
                        GDK_RGB_DITHER_NORMAL,
                        gtk_buf,
                        __mg_rtos_xvfb_header->width,
                        cmap);
            }
            else { 
                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        0,
                        0,
                        __mg_rtos_xvfb_header->width,
                        __mg_rtos_xvfb_header->height,
                        GDK_RGB_DITHER_NORMAL,
                        xvfb_buf,
                        __mg_rtos_xvfb_header->pitch,
                        cmap);
            }

            gdk_rgb_cmap_free (cmap);
            break;
        }


        case 16: { 
            gdk_draw_rgb_image(widget->window,
                    widget->style->fg_gc[GTK_STATE_NORMAL],
                    0,
                    0,
                    __mg_rtos_xvfb_header->width,
                    __mg_rtos_xvfb_header->height,
                    GDK_RGB_DITHER_MAX,
                    gtk_buf,
                    __mg_rtos_xvfb_header->width * 3);
            break;
        }

        case 24: {               
            gdk_draw_rgb_image(widget->window,
                    widget->style->fg_gc[GTK_STATE_NORMAL],
                    0,
                    0,
                    __mg_rtos_xvfb_header->width,
                    __mg_rtos_xvfb_header->height,
                    GDK_RGB_DITHER_MAX,
                    xvfb_buf,
                    __mg_rtos_xvfb_header->pitch);

            break;
        }

        case 32: {

            gdk_draw_rgb_32_image(widget->window,
                    widget->style->fg_gc[GTK_STATE_NORMAL],
                    0,
                    0,
                    __mg_rtos_xvfb_header->width,
                    __mg_rtos_xvfb_header->height,
                    GDK_RGB_DITHER_MAX,
                    xvfb_buf, 
                    __mg_rtos_xvfb_header->width*4);
            break;
        }

    }

    return FALSE;
}

int set_palette (guint32 *palette, int n_colors, XVFBPalEntry *colors)
{
    int i;

    if (palette == NULL)
        return 0;

    for (i = 0; i < n_colors; i++)
        *palette++ = (0xff << 24) | ((colors[i].r & 0xff) << 16) | ((colors[i].g & 0xff) << 8) | (colors[i].b & 0xff);

    return 1;
}

int
Repaint(gpointer data)
{
    int dirty_l = __mg_rtos_xvfb_header->dirty_rc_l;
    int dirty_r = __mg_rtos_xvfb_header->dirty_rc_r;
    int dirty_t = __mg_rtos_xvfb_header->dirty_rc_t;
    int dirty_b = __mg_rtos_xvfb_header->dirty_rc_b;



    int pitch = __mg_rtos_xvfb_header->pitch;
    int color_depth = __mg_rtos_xvfb_header->depth;

    int buffer_width = __mg_rtos_xvfb_header->width;

    GtkWidget * widget = (GtkWidget *)data;
    XVFBPalEntry *p;


    if (color_depth <= 8 && __mg_rtos_xvfb_header->palette_changed == 1) {
        p = (XVFBPalEntry *)(
                (unsigned char *)__mg_rtos_xvfb_header + 
                __mg_rtos_xvfb_header->palette_offset
                );

        set_palette (pal_entry, n_colors, p);
        __mg_rtos_xvfb_header->palette_changed = 0;
    }


    if (__mg_rtos_xvfb_header->dirty == TRUE) {

        guchar *sp;
        guchar *dp;
        int row, col;                    

        switch (__mg_rtos_xvfb_header->depth) {

            case 1: {
                GdkRgbCmap *cmap;
                cmap = gdk_rgb_cmap_new (pal_entry, 2);
                int bit;
                guchar c;

                int gtk_pitch = buffer_width;
                dirty_l -= dirty_l % 8;

                for (row=dirty_t; row<dirty_b; row++)
                {
                    sp = xvfb_buf + pitch*row + dirty_l/8;
                    dp = gtk_buf + gtk_pitch*row + dirty_l;
                    for (col = dirty_l; col < dirty_r; col++) {
                        bit = col %8;
                        if (bit == 0)
                            c = *sp++;
#ifdef USE_MSBLEFT
                        *dp++ = (c >> (7-bit)) & 0x01;
#else
                        *dp++ = (c >> bit) & 0x01;
#endif
                    }
                }

                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_NORMAL,
                        gtk_buf + dirty_t*gtk_pitch + dirty_l,
                        gtk_pitch,
                        cmap);
                gdk_rgb_cmap_free (cmap);
                break;
            }

            case 2: {
                GdkRgbCmap *cmap;
                int bit;
                guchar c;

                int gtk_pitch = buffer_width;
                dirty_l -= dirty_l % 4;

                cmap = gdk_rgb_cmap_new (pal_entry, 4);

                for (row=dirty_t; row<dirty_b; row++)
                {
                    sp = xvfb_buf + pitch*row + dirty_l/4;
                    dp = gtk_buf + gtk_pitch*row + dirty_l;

                    for (col=dirty_l; col<dirty_r; col++) {
                        bit = (col% 4) << 1;
                        if (bit == 0)
                            c = *sp++;
#ifdef USE_MSBLEFT
                        *dp++ = (c >> (6-bit)) & 0x03;
#else
                        *dp++ = (c >> bit) & 0x03;
#endif
                    }
                }


                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_NORMAL,
                        gtk_buf + gtk_pitch*dirty_t + dirty_l,
                        gtk_pitch,
                        cmap);

                gdk_rgb_cmap_free (cmap);

                break;
            }            

            case 4: {
                GdkRgbCmap *cmap;
                guchar c;
                int bit;

                int gtk_pitch = buffer_width;

                cmap = gdk_rgb_cmap_new (pal_entry, 16);

                dirty_l -= dirty_l % 2;

                for (row=dirty_t; row<dirty_b; row++)
                {
                    sp = xvfb_buf + pitch*row + dirty_l/2;
                    dp = gtk_buf + gtk_pitch*row + dirty_l;
                    for (col=dirty_l; col<dirty_r; col++) {
                        bit = (col% 2) << 2;
                        if (bit == 0)
                            c = *sp++;
#ifdef USE_MSBLEFT
                        *dp++ = (c >>(4-bit)) & 0x0f;
#else
                        *dp++ = (c >> bit) & 0x0f;
#endif
                    }
                }

                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_NORMAL,
                        gtk_buf + gtk_pitch*dirty_t + dirty_l,
                        gtk_pitch,
                        cmap);

                gdk_rgb_cmap_free (cmap);

                break;
            }

            case 8: {
                GdkRgbCmap *cmap;               


                cmap = gdk_rgb_cmap_new (pal_entry, 256);

                gdk_draw_indexed_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_NORMAL,
                        xvfb_buf + pitch*dirty_t + dirty_l,
                        pitch,
                        cmap);

                gdk_rgb_cmap_free (cmap);

                break;
            }            

            case 16: {
                Uint16 word;
                int gtk_pitch = buffer_width*3;
                for (row = dirty_t; row < dirty_b; row++)
                {
                    sp = xvfb_buf + pitch*row + dirty_l*2;
                    dp = gtk_buf + gtk_pitch*row + dirty_l*3;

                    for (col = 0; col < buffer_width; col++) {

                        word = *((Uint16*)sp);
                        sp+=2;

                        *dp++ = ((word & 0xF800) >> 11) << 3; /* red */                        
                        *dp++ = ((word & 0x07E0) >> 5) << 2;  /* green */              
                        *dp++ = (word & 0x001F) << 3;         /* blue */                         

                    }
                }

                gdk_draw_rgb_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_MAX,
                        gtk_buf + dirty_t*gtk_pitch +dirty_l*3,
                        gtk_pitch);

                break;
            }            

            case 24: {

                gdk_draw_rgb_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_MAX,
                        xvfb_buf + dirty_t*pitch + dirty_l*3,
                        pitch);

                break;
            }

            case 32: {
                gdk_draw_rgb_32_image(widget->window,
                        widget->style->fg_gc[GTK_STATE_NORMAL],
                        dirty_l,
                        dirty_t,
                        dirty_r - dirty_l,
                        dirty_b - dirty_t,
                        GDK_RGB_DITHER_MAX,
                        xvfb_buf + dirty_t*pitch + dirty_l*4,
                        pitch);
                break;
            }

        }

        __mg_rtos_xvfb_header->dirty = FALSE;

#ifdef _DEBUG
        fprintf (stderr, "dirty == TRUE and window is repained. \n");
#endif
    }        


    return TRUE;
}

int
on_motion(GtkWidget *widget, GdkEventMotion*event)
{
    XVFBEVENT event_data;
    int x = event->x;
    int y = event->y;

    event_data.event_type = 0;
    event_data.mouse_data.x = (x<0) ? 0 : x;
    event_data.mouse_data.y = (y<0) ? 0 : y;

    event_data.mouse_data.btn = 0;
    if (event->state & GDK_BUTTON1_MASK)
        event_data.mouse_data.btn |= 0x0001; 
    if (event->state & GDK_BUTTON3_MASK)
        event_data.mouse_data.btn |= 0x0002; 
    
    if (xVFBNotifyNewEvent (__mg_rtos_xvfb_event_buffer, &event_data) == 2)
    {
        close_app();
        return FALSE;
    }

#ifdef _DEBUG
    fprintf (stderr, "==================current mouse state = %d\n", event->state);    
#endif
    
    return TRUE;
}

int
on_click(GtkWidget *widget, GdkEventButton * event)
{
    XVFBEVENT event_data;    
    int x = event->x;;
    int y = event->y;

    event_data.event_type = 0;
    event_data.mouse_data.x = (x<0) ? 0 : x;
    event_data.mouse_data.y = (y<0) ? 0 : y;

#ifdef _DEBUG
    fprintf (stderr, "button type = %d. \n", event->type);
#endif
    
    if(event->type == GDK_BUTTON_RELEASE) {
        event_data.mouse_data.btn = 0;
#ifdef _DEBUG
        fprintf (stderr, "=======button release. \n");
#endif
    }
    else {
        if (event->button == 1)
            event_data.mouse_data.btn = 0x0001;
        else if (event->button == 3)
            event_data.mouse_data.btn = 0x0002;
#ifdef _DEBUG
        fprintf (stderr, "===========button = %d \n", event->button);
#endif
    }
    
    if (xVFBNotifyNewEvent (__mg_rtos_xvfb_event_buffer, &event_data) == 2)
    {
        close_app();
        return FALSE;
    }
        
    return TRUE;
}

int keyboard_handler ( GtkWidget *wp, GdkEventKey *event, gpointer data )
{
    XVFBEVENT event_data;

    /*if keypad 5 but no numlock*/
    if (event->hardware_keycode == 84 && !(event->state & GDK_MOD2_MASK))
        return TRUE;

    event_data.event_type = 1;
    event_data.kb_data.key_code = translate_scancode(event->hardware_keycode, 
                    event->state & GDK_MOD2_MASK);

    if ( event->type == GDK_KEY_PRESS )    
        event_data.kb_data.key_state = 1;
    else if (event->type == GDK_KEY_RELEASE)
        event_data.kb_data.key_state = 0;


    if (xVFBNotifyNewEvent (__mg_rtos_xvfb_event_buffer, &event_data) == 2)
    {
        close_app();
        return FALSE;
    }
        

#ifdef _DEBUG
    fprintf (stderr, "Keyboard event %d %s",
             event->keyval, gdk_keyval_name(event->keyval) );

    if ( event->type == GDK_KEY_PRESS )
        fprintf (stderr, " pressed" );
    else if ( event->type == GDK_KEY_RELEASE )
        fprintf (stderr, " released" );
    else
        fprintf (stderr, " event-type-%d", event->type );
    
    if ( event->length > 0 )
        fprintf (stderr, " string: %s\n", event->string );
#endif

    return TRUE;
} 
  


static depth_to_rgbmask(int color_depth, int* r_mask, int* g_mask, int* b_mask, int* a_mask)
{
    switch (color_depth)
    {
        case 16:
            *r_mask = 0x0000F800;
            *g_mask = 0x000007E0;
            *b_mask = 0x0000001F;
            *a_mask = 0;
            break;
        case 24:
            *r_mask = 0x000000FF;
            *g_mask = 0x0000FF00;
            *b_mask = 0x00FF0000;
            *a_mask = 0;
            break;
        case 32:
            *r_mask = 0x000000FF;
            *g_mask = 0x0000FF00;
            *b_mask = 0x00FF0000;
            *a_mask = 0xFF000000;
            break;
        default:
            break;
    }
    
}

static int init_xvfb (int buffer_width, int buffer_height, int color_depth)
{
    int data_size;
    int r_mask;
    int g_mask;
    int b_mask;
    int a_mask;

    depth_to_rgbmask(color_depth, &r_mask, &g_mask, &b_mask, &a_mask);

#ifdef USE_MSBLEFT
    __mg_rtos_xvfb_header = xVFBAllocVirtualFrameBuffer (buffer_width, buffer_height, color_depth, r_mask , g_mask, b_mask, a_mask, TRUE);
#else
    __mg_rtos_xvfb_header = xVFBAllocVirtualFrameBuffer (buffer_width, buffer_height, color_depth, r_mask , g_mask, b_mask, a_mask, FALSE);
#endif

    xvfb_buf = (unsigned char *)__mg_rtos_xvfb_header + __mg_rtos_xvfb_header->fb_offset;
    
    __mg_rtos_xvfb_event_buffer = xVFBCreateEventBuffer (20);

    
    if (color_depth <= 8) {
        n_colors = 1 << color_depth;
        pal_entry = (guint32 *)malloc (n_colors * sizeof(guint32));       
    }
    

    /* Alloc gtk_buf when color_depth>8 or color_depth <8. If color_depth ==8, then alloc nothing.*/
    if (color_depth > 8)
        data_size = __mg_rtos_xvfb_header->width * __mg_rtos_xvfb_header->height * 3 ;
    else if (color_depth <= 8)
        data_size = __mg_rtos_xvfb_header->width * __mg_rtos_xvfb_header->height * 8 / 8;;
    
    gtk_buf = (guchar *)malloc (data_size);    
        
    return 0;
}

int
close_app (GtkWidget *widget, gpointer data)
{
    if (pal_entry != NULL) {
        free (pal_entry);
        n_colors = 0;
    }

    if (gtk_buf != NULL)
        free (gtk_buf);
    
    gtk_main_quit ();

    xVFBFreeVirtualFrameBuffer(__mg_rtos_xvfb_header);
    xVFBDestroyEventBuffer(__mg_rtos_xvfb_event_buffer);

    fprintf (stderr, "close_app is occurs. \n");
   return TRUE;
}

static void analyze_args(int argc, char** argv, int* width, int* height, int* depth)
{
    int ch;
    while ((ch=getopt(argc, argv, "w:h:d:")) != -1)
    {
        switch (ch)
        {
            case 'w':
                *width = atoi(optarg);
                break;
            case 'h':
                *height = atoi(optarg);
                break;
            case 'd':
                *depth = atoi(optarg);
                break;
            default:
                break;
        }
    }



}

int main( int argc, char *argv[] )
{
    GtkWidget *window, *darea;
    pthread_t minigui_thread;
    int buffer_width = 800;
    int buffer_height = 600;
    int color_depth = 16;

    analyze_args(argc, argv, &buffer_width, &buffer_height, &color_depth);


    gtk_init (&argc, &argv);
    gdk_rgb_init();

    
    init_xvfb (buffer_width, buffer_height, color_depth);

    if ( pthread_create( &minigui_thread, NULL,  start_minigui_app, argv) ) {
        printf("error creating thread.");
        abort();
    }
    fprintf (stderr, "MiniGUI thread is created and continue to run gtk. \n");

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title ((GtkWindow *)window, "RTOS_XVFB Program");
    gtk_window_set_default_size ((GtkWindow *)window, buffer_width, buffer_height);
    g_signal_connect (window, "destroy", G_CALLBACK (close_app), NULL);
    
    
    darea=gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(darea), buffer_width, buffer_height);
    gtk_container_add(GTK_CONTAINER(window), darea);

    gtk_signal_connect(GTK_OBJECT (darea), "expose_event",
                       GTK_SIGNAL_FUNC(on_darea_expose),NULL);

    gtk_signal_connect(GTK_OBJECT (darea), "motion_notify_event",
                       GTK_SIGNAL_FUNC(on_motion),NULL);
        
    gtk_signal_connect(GTK_OBJECT (darea), "button_press_event",
                       GTK_SIGNAL_FUNC(on_click),NULL);

    gtk_signal_connect(GTK_OBJECT (darea), "button_release_event",
                       GTK_SIGNAL_FUNC(on_click),NULL);

    /* We never see the release event, unless we add the press
     * event to the mask.*/    
    gtk_widget_add_events ( GTK_WIDGET(darea), GDK_BUTTON_RELEASE_MASK );
    gtk_widget_add_events ( GTK_WIDGET(darea), GDK_BUTTON_PRESS_MASK );

    /* Now, try to work the magic to get keyboard events */
    GTK_WIDGET_SET_FLAGS ( darea, GTK_CAN_FOCUS );

    gtk_widget_add_events ( GTK_WIDGET(darea), GDK_KEY_PRESS_MASK );
    g_signal_connect ( darea, "key_press_event", G_CALLBACK(keyboard_handler), NULL );
    g_signal_connect ( darea, "key_release_event", G_CALLBACK(keyboard_handler), NULL );    
    

    /* Update screen 1/20 second */
    gtk_timeout_add(50, Repaint,(gpointer)darea);

    gtk_widget_add_events(darea, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);    
    gtk_widget_show_all(window);
    
    gtk_main ();
    
    return 0;
}
