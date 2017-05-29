#ifndef _AUTO_PROTOCOL_H
#define _AUTO_PROTOCOL_H

typedef struct _auto_ial_input_event {
    long long int timestamp;
    int type; /* 0: keyborad, 1: mouse */
    union {
        struct {
            int scancode;
            int type; /* 0: UP, 1: DOWN */
        } key_event;
        struct {
            int x;
            int y;
            int buttons; /* IAL_MOUSE_LEFTBUTTON, *_RIGHTBUTTON, *_MIDDLEBUTTON */
        } mouse_event;
    } u;
} AUTO_IAL_INPUT_EVENT;

#endif
