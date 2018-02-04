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
/******************************************************************************
 *
 * Author:	zangguangtong
 *
 * File name:	abs_signal_driver.h
 *	
 * Abstract:	this is the head file of abs signal driver module
 *
 ******************************************************************************
 * Revision History
 *
 * Time         		Rev  	by	     				contents
 * -----------  ---  ---     -------------------------------------------
 * 29-01-2004  	1.0  		zangguangtong   		written
 *
 *****************************************************************************/
#ifndef _ABS_SIGNAL_DRIVER_H
#define _ABS_SIGNAL_DRIVER_H

//#define SIGNAL_SOCKET_TEST

/* about debug infomation */
//#define   DEBUG_ABSSIG_INFO
#ifdef   DEBUG_ABSSIG_INFO
	#define DBGABSSIG_INFO(a)	{printf a ;}
#else
	#define DBGABSSIG_INFO(a)	{}
#endif
#define DBGABSSIG_ERR(a)	{if(__mg_g_abssig_debug_mode) printf a ;}

#define ABSSIG_INVALID_FD		-1
#define ABSSIG_FAIL			-1
#define ABSSIG_SUCCESS			0

/* telephone message: receive from driver */
#define TEL_RMSG_INVALID	0x00
#define TEL_RMSG_KEY_1		0x01
#define TEL_RMSG_KEY_2		0x02
#define TEL_RMSG_KEY_3		0x03
#define TEL_RMSG_KEY_4		0x04
#define TEL_RMSG_KEY_5		0x05
#define TEL_RMSG_KEY_6		0x06
#define TEL_RMSG_KEY_7		0x07
#define TEL_RMSG_KEY_8		0x08
#define TEL_RMSG_KEY_9		0x09
#define TEL_RMSG_KEY_Asterisk	0x0A  // *
#define TEL_RMSG_KEY_0	0x0B 
#define TEL_RMSG_KEY_Sharp	0x0C // #
#define TEL_RMSG_KEY_REDIAL	0x0D
#define TEL_RMSG_KEY_FLASH	0x0E
#define TEL_RMSG_KEY_MUTE	0x0F
#define TEL_RMSG_KEY_HANDFREE	0x10
#define TEL_RMSG_KEY_TP10	0x11
#define TEL_RMSG_KEY_TP20	0x12
#define TEL_RMSG_KEY_PINP	0x13
#define TEL_RMSG_KEY_PRIVATE	0x14
#define TEL_RMSG_KEY_MENU	0x15
#define TEL_RMSG_KEY_CANCEL	0x16
#define TEL_RMSG_KEY_UP		0x17
#define TEL_RMSG_KEY_DOWN	0x18
#define TEL_RMSG_KEY_LEFT	0x19
#define TEL_RMSG_KEY_RIGHT	0x1A
#define TEL_RMSG_KEY_VOLINC	0x1B
#define TEL_RMSG_KEY_VOLDEC	0x1C
#define TEL_RMSG_KEY_PAGEUP	0x1D
#define TEL_RMSG_KEY_PAGEDOWN	0x1E
#define TEL_RMSG_KEY_F1		0x1F
#define TEL_RMSG_KEY_F2		0x20
#define TEL_RMSG_KEY_F3		0x21
#define TEL_RMSG_KEY_F4		0x22
#define TEL_RMSG_KEY_F5		0x23
#define TEL_RMSG_KEY_F6		0x24
#define TEL_RMSG_ONHOOK		0x25
#define TEL_RMSG_OFFHOOK	0x26


/* telephone message: send to driver */
#define TEL_SMSG_SWITCH_PSTN			0x2001
#define TEL_SMSG_SWITCH_VOIP			0x2002
#define	TEL_SMSG_DIAL_TONE			0x2003
#define	TEL_SMSG_DIAL_TONE_STOP			0x2004
#define	TEL_SMSG_CALLBACK_TONE			0x2005
#define	TEL_SMSG_CALLBACK_TONE_STOP		0x2006
#define	TEL_SMSG_BUSY_TONE			0x2007
#define	TEL_SMSG_BUSY_TONE_STOP			0x2008
#define	TEL_SMSG_BLARE_TONE			0x2009
#define	TEL_SMSG_BLARE_TONE_STOP		0x200a
#define	TEL_SMSG_START_RING			0x200b
#define	TEL_SMSG_STOP_RING			0x200c
#define TEL_SMSG_SWITCH_HANDSET			0x200d
#define TEL_SMSG_SWITCH_SPEAKER			0x200e
#define TEL_SMSG_PSTN_OFFHOOK			0x200f	
#define TEL_SMSG_PSTN_DTMF			0x2010
#define TEL_SMSG_PSTN_ENDCALL			0x2011
#define TEL_SMSG_PSTN_TALK			0x2012
#define TEL_SMSG_KEY_TONE			0x2013
#define TEL_SMSG_KEY_VOLINC		0x2014
#define TEL_SMSG_KEY_VOLDEC		0x2015

/* ABS TEL Structure */
typedef struct _abs_sig_info_
{
	unsigned int dev_type; // device type
#define INVALID		0
#define UD_DSP_1	1//UDtech DSP driver for voice pump;
	int dev_fd; // device id
	unsigned long dev_msg; // device msg
	unsigned char dev_id;  // device channel id
}ABS_SIG_INFO;

extern int __mg_g_abssig_debug_mode;

extern int ABSSIG_open_tel_device (ABS_SIG_INFO *asi);
extern int ABSSIG_close_tel_device (ABS_SIG_INFO *asi);
extern int ABSSIG_receive_msg_from_driver (ABS_SIG_INFO *asi);
extern int ABSSIG_send_msg_to_driver (ABS_SIG_INFO *asi);

#endif


