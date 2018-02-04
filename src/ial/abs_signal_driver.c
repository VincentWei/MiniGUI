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
** abs_signal_driver.c: this is the process file of abs signal driver module
**
** Author:	zangguangtong
*/

#include <stdio.h>
#include <sys/fcntl.h>

#ifdef SIGNAL_SOCKET_TEST
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <asm/dsp.h>
#include "abs_signal_driver.h"


int ABSSIG_translate_msg_from_driver_to_app (void *pmsg, ABS_SIG_INFO *asi);
int ABSSIG_translate_msg_from_app_to_driver(void *pmsg, ABS_SIG_INFO *asi);

int __mg_g_abssig_debug_mode = 0;
int __mg_g_abssig_fd = ABSSIG_INVALID_FD;

int ABSSIG_open_tel_device (ABS_SIG_INFO *asi)
{	
#ifndef SIGNAL_SOCKET_TEST
	
	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}
	
	asi->dev_fd = ABSSIG_INVALID_FD;

	switch(asi->dev_type)
	{
		case UD_DSP_1:
		{
			int socket_val;
			char name[20];
				
			sprintf(name, "/dev/tel");
			socket_val = open(name,O_RDWR);
			if(socket_val == ABSSIG_INVALID_FD)
			{
				DBGABSSIG_ERR(("Can't open tel device in ABSSIG_open_tel_device!\n"));
				asi->dev_fd = ABSSIG_INVALID_FD;
				return ABSSIG_INVALID_FD;
			}
	
			__mg_g_abssig_fd = asi->dev_fd = socket_val;			
            fprintf(stderr, "opened dev_fd %d\n", socket_val);
			return ABSSIG_SUCCESS;
		}
		default:
			return ABSSIG_FAIL;
	}
#else
	int test_socket;
	struct sockaddr_in sin;
	int tos;
	int opt;
	int len;

	int socket_val;
	char name[20];
	sprintf(name, "/dev/tel");
	socket_val = open(name,O_RDWR);
	if(socket_val == ABSSIG_INVALID_FD)
	{
		DBGABSSIG_ERR(("Can't open tel device in ABSSIG_open_tel_device!\n"));
		return ABSSIG_INVALID_FD;
	}

  	test_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (test_socket == -1)
	{
		DBGABSSIG_ERR(("UDP socket open failed!"));
		return -1;
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(1556);
	if (bind(test_socket, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		DBGABSSIG_ERR(("UDP(%d) socket bind failed!\n"));
		close(test_socket);
		return -1;
	}
	/*
	opt = 1; 
	len = sizeof(opt);   
   	setsockopt(test_socket,SOL_SOCKET,SO_REUSEADDR,&opt,&len); 
	if (fcntl(test_socket, F_SETFL, O_NONBLOCK) == -1) 
	{
		DBGABSSIG_ERR(("fcntl fail!\n"));
		close(test_socket);
		return -1;
	}
	*/
	
	__mg_g_abssig_fd = test_socket;
	return test_socket;
#endif
}


int ABSSIG_close_tel_device (ABS_SIG_INFO *asi)
{
	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}
	
	switch(asi->dev_type)
	{
		case UD_DSP_1:
			{
				close(asi->dev_fd);
				__mg_g_abssig_fd = ABSSIG_INVALID_FD;
				asi->dev_fd = ABSSIG_INVALID_FD;
			}
			break;
		default:
			return ABSSIG_FAIL;
	}
	/*
	if(fd != __mg_g_abssig_fd)
	{
		DBGABSSIG_ERR(("Input fd is wrong in ABSSIG_close_tel_device!Input fd is %d and fd of tel device is %d \n",fd,__mg_g_abssig_fd));
		return ABSSIG_FAIL;
	}
	close(fd); 
	__mg_g_abssig_fd = ABSSIG_INVALID_FD;
	*/
	return ABSSIG_SUCCESS;
}


int ABSSIG_receive_msg_from_driver(ABS_SIG_INFO *asi)
{
#ifndef SIGNAL_SOCKET_TEST
	int translate_result = ABSSIG_FAIL;
	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}
	
	switch(asi->dev_type)
	{
		case UD_DSP_1:
			{
			telesignal_msg tel_msg;

			if(asi->dev_fd == ABSSIG_INVALID_FD)
			{
				DBGABSSIG_ERR(("FD of tel device is invalid in ABSSIG_receive_msg_from_driver!\n"));
				return ABSSIG_FAIL;
			}

			read(__mg_g_abssig_fd,&tel_msg,sizeof(tel_msg));			
			translate_result = ABSSIG_translate_msg_from_driver_to_app(&tel_msg, asi);
			}
			break;
		default:
			break;
	}
	return translate_result;
#else	
	struct sockaddr from;
	socklen_t len;
	telesignal_msg tel_msg;
	int translate_result;	
	
	recvfrom(__mg_g_abssig_fd, &tel_msg, sizeof(telesignal_msg), 0, &from, &len);
	tel_msg.type = ntohl(tel_msg.type);
	translate_result = ABSSIG_translate_msg_from_driver_to_app(&tel_msg);
	return translate_result;
#endif

}


int ABSSIG_send_msg_to_driver (ABS_SIG_INFO *asi)
{
#ifndef SIGNAL_SOCKET_TEST

	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}

	switch(asi->dev_type)
	{
		case UD_DSP_1:
		{
			telesignal_msg tel_msg;
			int translate_result;

			if(asi->dev_fd == ABSSIG_INVALID_FD)
			{
				DBGABSSIG_ERR(("FD of tel device is invalid in ABSSIG_send_msg_to_driver!\n"));
				return ABSSIG_FAIL;
			}

			translate_result = ABSSIG_translate_msg_from_app_to_driver(&tel_msg, asi);
			if(translate_result == ABSSIG_FAIL)
			{
				DBGABSSIG_ERR(("Fails when translate the message from application,so can't send message to driver in ABSSIG_send_msg_to_driver! \n"));
				return ABSSIG_FAIL;
			}			
			write(asi->dev_fd, &tel_msg, sizeof(tel_msg));
		}
			break;
		default:
			return ABSSIG_FAIL;
	}
	return ABSSIG_SUCCESS;
#else
	char message_type[256];

	switch(abssig_signal_msg->msg_type)
	{
		case SIGNAL_SIG2DRV_SWITCH_PSTN:
			strcpy(message_type,"switch to PSTN");
			break;
		case SIGNAL_SIG2DRV_SWITCH_VOIP:
			strcpy(message_type,"switch to VOIP");
			break;
		case SIGNAL_SIG2DRV_DIAL_TONE:
			strcpy(message_type,"play dial tone");
			break;
		case SIGNAL_SIG2DRV_DIAL_TONE_STOP:
			strcpy(message_type,"stop dial tone");
			break;
		case SIGNAL_SIG2DRV_CALLBACK_TONE:
			strcpy(message_type,"play callback tone");
			break;
		case SIGNAL_SIG2DRV_CALLBACK_TONE_STOP:
			strcpy(message_type,"stop callback tone");
			break;
		case SIGNAL_SIG2DRV_BUSY_TONE:
			strcpy(message_type,"play busy tone");
			break;
		case SIGNAL_SIG2DRV_BUSY_TONE_STOP:
			strcpy(message_type,"stop busy tone");
			break;
		case SIGNAL_SIG2DRV_BLARE_TONE:
			strcpy(message_type,"play blare tone");
			break;
		case SIGNAL_SIG2DRV_BLARE_TONE_STOP:
			strcpy(message_type,"stop blare tone");
			break;
		case SIGNAL_SIG2DRV_START_RING:
			strcpy(message_type,"start ring");
			break;
		case SIGNAL_SIG2DRV_STOP_RING:
			strcpy(message_type,"stop ring");
			break;
		case SIGNAL_SIG2DRV_PSTN_OFFHOOK:
			strcpy(message_type,"notify PSTN OFFHOOK");
			break;
		case SIGNAL_SIG2DRV_PSTN_DTMF:
			strcpy(message_type,"notify PSTN DTMF");
			break;
		default:
			strcpy(message_type,"wrong message type");
			break;
	}
	DBGABSSIG_INFO(("\nReceived message is %s in ABSSIG_send_msg_to_driver!",message_type));
	DBGABSSIG_INFO(("\nLine number is %d in ABSSIG_send_msg_to_driver! \n",abssig_signal_msg->line_num));
	return ABSSIG_SUCCESS;
#endif
}


int ABSSIG_translate_msg_from_driver_to_app(void *pmsg, ABS_SIG_INFO *asi)
{
	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}
	if(NULL == pmsg)
	{
		DBGABSSIG_ERR(("tel message is NULL in ABSSIG_translate_msg_from_driver_to_app!\n"));
		return ABSSIG_FAIL;
	}
	
	switch(asi->dev_type)
	{
		case UD_DSP_1:
		{
			telesignal_msg *tel_msg = (telesignal_msg *)pmsg;
			switch(tel_msg->type)
			{
				case TELE_SIGNAL_DRV2TSK_OFFHOOK:
					asi->dev_msg = TEL_RMSG_OFFHOOK;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
					
				case TELE_SIGNAL_DRV2TSK_ONHOOK:
					asi->dev_msg = TEL_RMSG_ONHOOK;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
					
				case TELE_SIGNAL_DRV2TSK_DTMF:
					asi->dev_msg = tel_msg->message.dtmf_data.phonenum[0];
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
					
				case TELE_SIGNAL_DRV2TSK_PSTN_DIAL_TONE:
					asi->dev_msg = TEL_RMSG_INVALID;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
					
				case TELE_SIGNAL_DRV2TSK_PSTN_RING_START:
					asi->dev_msg = TEL_RMSG_INVALID;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
					
				case TELE_SIGNAL_DRV2TSK_PSTN_RING_STOP:
					asi->dev_msg = TEL_RMSG_INVALID;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;

				case TELE_SIGNAL_DRV2TSK_RINGBACK:				
					asi->dev_msg = TEL_RMSG_INVALID;
					asi->dev_id = tel_msg->message.control_data.linenum;
					break;
				
				case TELE_SIGNAL_DRV2TSK_VOICE_DATA:			
					asi->dev_msg = TEL_RMSG_INVALID;
					asi->dev_id = tel_msg->message.control_data.linenum;
					return ABSSIG_FAIL;
				
				default:
					return ABSSIG_FAIL;
			}
		}
			break;
		default:
			return ABSSIG_FAIL;
	}
	return ABSSIG_SUCCESS;
}

int ABSSIG_translate_msg_from_app_to_driver(void *pmsg, ABS_SIG_INFO *asi)
{
	if (NULL == asi)
	{
		return ABSSIG_FAIL;
	}
	if (NULL == pmsg)
	{
		DBGABSSIG_ERR(("tel message is NULL in ABSSIG_translate_msg_from_app_to_driver!\n"));
		return ABSSIG_FAIL;
	}

	switch (asi->dev_type)
	{
		case UD_DSP_1:
		{
			telesignal_msg *tel_msg = (telesignal_msg *)pmsg;
			
			switch(asi->dev_msg)
			{
				case TEL_SMSG_SWITCH_PSTN:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_SWITCH_PSTN;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_SWITCH_VOIP:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_SWITCH_VOIP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_DIAL_TONE:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_DIAL_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_DIAL_TONE_STOP:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_DIAL_TONE_STOP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_CALLBACK_TONE:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_OTHERRING_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_CALLBACK_TONE_STOP:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_OTHERRING_TONE_STOP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_BUSY_TONE:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_BUSY_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_BUSY_TONE_STOP:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_BUSY_TONE_STOP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_BLARE_TONE:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_BLARE_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_BLARE_TONE_STOP:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_BLARE_TONE_STOP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_START_RING:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_RING_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_STOP_RING:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_RING_TONE_STOP;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_PSTN_OFFHOOK:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_PSTN_OFFHOOK;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_PSTN_DTMF:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_PSTN_DTMF;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				case TEL_SMSG_PSTN_ENDCALL:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_PSTN_ENDCALL;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

#if 0
				case TEL_SMSG_PSTN_TALK:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_PSTN_TALK;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
#endif

				case TEL_SMSG_SWITCH_HANDSET:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_SWITCH_HANDSET;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_SWITCH_SPEAKER:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_SWITCH_SPEAKER;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_KEY_TONE:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_KEY_TONE;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_KEY_VOLINC:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_VOL_INC;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;
				case TEL_SMSG_KEY_VOLDEC:
					tel_msg->type = TELE_SIGNAL_TSK2DRV_VOL_DEC;
					tel_msg->message.control_data.linenum = asi->dev_id;
					break;

				default:
					DBGABSSIG_ERR(("The type of message from signal module is wrong in ABSSIG_translate_msg_from_app_to_driver!message type is %d \n",asi->dev_msg));
					return ABSSIG_FAIL;

			}
		}
			break;
		
		default :
			return ABSSIG_FAIL;
	}
	return ABSSIG_SUCCESS;
}


