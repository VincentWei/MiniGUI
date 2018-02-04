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
** skyeye-ep7312.h: the head file of Low Level Input Engine for 
**              touch screen SkyEye EP7312 simulation
**
** Created by Wei Yongming, 2005/03/10
*/

#ifndef GUI_IAL_SKYEYEEP7312_H
    #define GUI_IAL_SKYEYEEP7312_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitSkyEyeEP7312Input (INPUT* input, const char* mdev, const char* mtype);
void    TermSkyEyeEP7312Input (void);

#define EP7312TS_VERSION 2

/*----------------------------------------------------------------------------*/

/* Pen events */
#define EV_PEN_DOWN    0
#define EV_PEN_UP      1
#define EV_PEN_MOVE    2

/* Definition for the ioctl of the driver */
/* Device is type misc then major=10 */
#define EP7312TS_MAJOR  10

#define TS_PARAMS_GET _IOR(EP7312TS_MAJOR, 0, struct ts_drv_params)
#define TS_PARAMS_SET _IOR(EP7312TS_MAJOR, 1, struct ts_drv_params)

/*----------------------------------------------------------------------------*/

/* Available info from pen position and status */
struct ts_pen_info {
  int x,y;    /* pen position                                      */
  int dx,dy;  /* delta move from last position                     */
  int event;  /* event from pen (DOWN,UP,CLICK,MOVE)               */
  int state;  /* state of pen (DOWN,UP,ERROR)                      */
  int ev_no;  /* no of the event                                   */
  unsigned long ev_time;  /* time of the event (ms) since ts_open  */
};

/* Structure that define touch screen parameters */
struct ts_drv_params {
  int version;     /* version number for check, returned by TS_PARAMS_GET
		    */
  int version_req; /* version number for check, that MUST be set to
		    * EP7312TS_VERSION prior to do TS_PARAMS_SET,
		    * or to -1 to bypass checking (do not do this usually)
		    */
  int x_ratio_num; /*                        */
  int x_ratio_den; /*                        */
  int y_ratio_num; /*                        */
  int y_ratio_den; /*                        */
  int x_offset;    /*                        */
  int y_offset;    /*                        */
  int xy_swap;     /*                        */
  int x_min;       /*                        */
  int y_min;       /*                        */
  int x_max;       /*                        */
  int y_max;       /*                        */
  int mv_thrs;     /* minimum delta move to considere the pen start to move */
  int follow_thrs; /* minimum delta move to follow the pen move when the pen
                    * is moving
		    */
  int sample_ms;   /* time between sampling (ms) */
  int deglitch_on; /* whether to filter glitches at pen down */
  int event_queue_on; /* switch on and off the event queue */
};

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_SKYEYEEP7312_H */


