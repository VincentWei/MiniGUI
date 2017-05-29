/*
** $Id: c33l05.c 10951 2008-09-11 03:51:14Z xwyan $
**
** c33l05.c: Low Level Input Engine for c33l05, EPSONARM7-LCD-R4 Dev Boards.
** 
** Copyright (C) 2007 Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"


#ifdef _C33L05_IAL

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "c33l05.h"

/* for data reading from /dev/ts */
typedef struct  {
	int x,y;    
	int dx,dy;
	int event;
	int pressure; 
	int ev_no;  
	unsigned long ev_time;  
}TS_EVENT;


static TS_EVENT ts_event;


static unsigned char state [NR_KEYS];
static int ts = -1;
static int mousex = 0;
static int mousey = 0;




/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
/*
    if (mousex < 0) mousex = 0;
    if (mousey < 0) mousey = 0;
    if (mousex > 239) mousex = 239;
    if (mousey > 319) mousey = 319;
*/
    *x = mousex;
    *y = mousey;

//    printf ("mousex = %d, mousey = %d\n", mousex, mousey);
}

static int mouse_getbutton(void)
{
    return (ts_event.pressure ? IAL_MOUSE_LEFTBUTTON:0 );
}

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    retvalue = 0;
    int    e;
	int bytes_read=0;
    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        FD_SET (ts, in);
    }
    e = select (FD_SETSIZE, in, out, except, timeout) ;

    if (e > 0) 
	{ 
        if (ts >= 0 && FD_ISSET (ts, in)) 
	{
            FD_CLR (ts, in);
	    //usleep(1);
            ts_event.x=0;
            ts_event.y=0;

		bytes_read =  read (ts, &ts_event, sizeof (TS_EVENT));      
        while(bytes_read<sizeof(TS_EVENT))
			bytes_read+=read(ts,(char *)&ts_event + bytes_read,sizeof(TS_EVENT) - bytes_read);


	
				mousex = ts_event.x;
				mousey = ts_event.y;

  //              printf ("mouse down: ts_event.x = %d, ts_event.y = %d\n", ts_event.x, ts_event.y);
            
				ts_event.pressure = ( ts_event.pressure == 0 ? IAL_MOUSE_LEFTBUTTON:0);
            retvalue |= IAL_MOUSEEVENT;
        }

    } 
    else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitC33L05Input(INPUT* input, const char* mdev, const char* mtype)
{
    ts = open (mdev, O_NONBLOCK);
    if (ts < 0) {
        fprintf (stderr, "C33L05: Can not open touch screen!\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    ts_event.x = ts_event.y = 0;
	ts_event.pressure &= 0;
    
    return TRUE;
}

void TermC33L05Input(void)
{
    if (ts >= 0)
        close(ts);    
}

#endif /* __TARGET_C33L05__ */

