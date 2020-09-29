#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>

#if defined(_MGIAL_COMM) && defined(__TARGET_EXTERNAL__)
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/exstubs.h>


#include <wiringPi.h>
#include "touch.h"




int __comminput_init (void)
{
    
    GT9147_Init();
    GT9147_Config();

    return 0;
}

/* return 0 when there is really a touch event */
int __comminput_ts_getdata (short *x, short *y, short *button)
{
    
 
    GT9147_Scan();
    for(int t=0;t<CT_MAX_TOUCH;t++)
    {
        if((tp_dev.sta)&(1<<t))//有触屏点按下了
        {
                printf("%d,%d\r\n",tp_dev.x[t],tp_dev.y[t]);
                *x=tp_dev.x[t]/5; *y=tp_dev.y[t]/6; *button = 0;
                return 0;            
        }
        
    }
    
    
    return -1;
}

/* return 0 when there is really a key event */
int __comminput_kb_getdata (short *key, short *status)
{

    return -1;
}

int __comminput_wait_for_input (struct timeval *timeout)
{
    return COMM_MOUSEINPUT;
}

void __comminput_deinit (void)
{

}

#endif /* _MGIAL_COMM && __TARGET_EXTERNAL__ */

