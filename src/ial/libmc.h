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
#ifndef __LIBMC_H__
#define __LIBMC_H__

#define SESSION_RC					5
#define SESSION_STATUS    			7
#define SESSION_LOG	    				8
#define SESSION_RESERVED 			100

#define MQ_KEY						50000  // KEY OF MESSAGE QUEUE
#define MSG_HEADER_SIZE			32
#define MSG_MAXSIZE				8192

#define MSG_KEY		    				2000
#define MSG_TO_MC        		       	15    //iType using to send a message sent to MC
#define MSG_REQUEST_KEYS			20
#define MSG_REG_APP				21

#define MSG_TOLCD_BROWSERLOADED		31
#define MSG_TOLCD_BROWSERUNLOADED	32

//messages from webserver
#define MSG_WS_PLAYFILE				50
#define MSG_WS_EXITBROWSER			53
#define MSG_WS_TICKETCONFIRMRESULT	54
//messages from player
#define MSG_PL_TICKETCONFIRM			61
#define MSG_PL_TOTAL_TIME				62      //add by fang , send total time to mc
#define MSG_PL_CURR_TIME				63      //add by fang , send current time to mc
#define MSG_PL_STATE_SPEED			64      //add by fang , send state and speed
//messages from lcd
#define MSG_LCD_PLAYFILE				80
#define MSG_LCD_PAUSE					81
#define MSG_LCD_PLAY					82
#define MSG_LCD_STOP					83
#define MSG_LCD_FW						84
#define MSG_LCD_FF						85

//Command messags
#define MSG_CMD_ACTIVATE				10
#define MSG_CMD_DEACTIVATE			11
#define MSG_CMD_PROCESS				12
#define MSG_CMD_UNLOAD				13

#define MSG_CMD_URL					14

//Reply messages
#define MSG_STATUS_REPORT_START		100
#define MSG_APP_STATUS_STANDBY		101
#define MSG_APP_STATUS_ACTIVE  		102
#define MSG_APP_STATUS_UNLOAD 		103

#define MSG_LOG_REPORT					110

#define MSG_STATUS_REPORT_END       	120

// Process Management Type Definitions
typedef enum{
	STATUS_UNLOAD,
	STATUS_STANDBY,
	STATUS_ACTIVE,
}stbStatus_t;

#define ERR_SEND_MSG_FAILED							-5003			//Send message failed.                        sndMsg()
#define SESSION_RESERVED 	100

typedef enum {
    ST_ZeroControl = 0,	 /* Keep this fist */
    ST_McControl,
    ST_PlayerControl,
    ST_UIControl,
    ST_BrowserControl, 
    ST_WebserverControl,
    ST_LCDControl,
    
    ST_LastControl,    /* Keep this Last */
    
    //Addition controls not used in state machine but useful for message transfer
    TO_MC,  
    TO_RC,
    
} stbControlIndex_t;

#if 0 //olsversion
typedef enum key_id_e
{
			KEY_ID_START = 0,
			KEY_RC_PLAY,
			KEY_RC_STOP,
			KEY_RC_PAUSE,
			KEY_RC_SLOW,
			KEY_RC_OK,
			KEY_RC_UP,
			KEY_RC_DOWN,
			KEY_RC_LEFT,
			KEY_RC_RIGHT,
			KEY_RC_10,
			KEY_RC_1,
			KEY_RC_2,
			KEY_RC_3,
			KEY_RC_4,
			KEY_RC_5,
			KEY_RC_6,
			KEY_RC_7,
			KEY_RC_8,
			KEY_RC_9,
			KEY_RC_0,
			KEY_RC_POWER,
			KEY_RC_OPEN,
			KEY_RC_VOL_INC,
			KEY_RC_VOL_DEC,
			KEY_RC_AB,
			KEY_RC_MUTE,
			KEY_RC_DISPLAY,
			KEY_RC_ZOOM,
			KEY_RC_SETUP,
			KEY_RC_FREV,
			KEY_RC_FFWD,
			KEY_RC_SKIP_DEC,
			KEY_RC_SKIP_INC,
			KEY_RC_AUDIO,
			KEY_RC_ANGLE,
			KEY_RC_SUBTITLE,
			KEY_RC_SEARCH,
			KEY_RC_TITLE,
			KEY_RC_RETURN,
			KEY_RC_CLR,
			KEY_RC_MENU,
			KEY_RC_PGUP,
			KEY_RC_PGDN,
			KEY_RC_REFRESH,
			KEY_RC_TAB,
			KEY_RC_HOME,
			KEY_RC_NEXT,
			KEY_RC_PREV,
			KEY_RC_REPEAT,
			KEY_RC_RED,
			KEY_RC_GREEN,
			KEY_RC_YELLOW,
			KEY_RC_BLUE,
			KEY_RC_CAPS,
			KEY_RC_NUM,
			KEY_PN_UP,
			KEY_PN_DOWN,
			KEY_PN_LEFT,
			KEY_PN_RIGHT,
			KEY_PN_OK,
			KEY_PN_STANDBY,
			KEYNUM  //keep this last
} key_id_t;
#endif //oldversion
typedef enum key_id_e
{
			KEY_ID_START = 0,
			KEY_PLAY ,
			KEY_STOP,
			KEY_PAUSE,
			KEY_SLOW,
			KEY_FFWD,
			KEY_FREV,
			KEY_PREV,
			KEY_NEXT,
			KEY_OK,
			KEY_CANCEL,
			KEY_UP,
			KEY_DOWN,
			KEY_LEFT,
			KEY_RIGHT,
			KEY_PGUP,
			KEY_PGDN,
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			KEY_10,
			KEY_POWER,
			KEY_OPEN,
			KEY_VOLINC,
			KEY_VOLDEC,
			KEY_MUTE,
			KEY_AB,
			KEY_REPEAT,
			KEY_DISPLAY,
			KEY_ZOOM,
			KEY_SETUP,
			KEY_SKIP_DEC,
			KEY_SKIP_INC,
			KEY_AUDIO,
			KEY_ANGLE,
			KEY_SUBTITLE,
			KEY_SEARCH,
			KEY_TITLE,
			KEY_RETURN,
			KEY_CLR,
			KEY_MENU,
			KEY_BACK,
			KEY_REFRESH,
			KEY_TAB,
			KEY_HOME,
			KEY_RED,
			KEY_GREEN,
			KEY_YELLOW,
			KEY_BLUE,
			KEY_CAPS,
			KEY_NUM,
			KEY_MOUSE,
			KEYNUM  //keep this last
} key_id_t;


typedef enum key_source_e_ {
		RC,
		PN,
		nodef,
} key_source_t;

typedef struct _KeyReg_s_{
	int keyid;
	key_source_t keysource;
}KeyReg_t;


typedef struct _MSGHd_s_{
	unsigned int type;			//Identity of message
	unsigned int msgId;		//Message value
	unsigned int session;		//Unique session key for a whole request
	unsigned int pid;			//Sender PID
	unsigned int size;			//Size of content
	int reserved[3];
}MsgHd_t;
typedef struct _MSG_s_{
	unsigned int type;			//Identity of message
	unsigned int msgId;		//Message value
	unsigned int session;		//Unique session key for a whole request
	unsigned int pid;			//Sender PID
	unsigned int size;			//Size of content
	int reserved[3];
	unsigned char content[MSG_MAXSIZE - MSG_HEADER_SIZE];
}Msg_t;

typedef struct key_detail_s
{
	key_id_t keyid;
	key_source_t keysource;
} key_detail_t;


//extern int gMsgQueueId;
extern int initMsgQueue( unsigned int aiCtrlIndx );
extern int initMsgQueueForMsgLog( void );
extern int rcvMcMsgWait(Msg_t *pMsg, unsigned int aiTimeOut);
extern int rcvMcMsgNoWait(Msg_t *pMsg);
extern int sndMsgToMc( unsigned int aiMsg, unsigned int aiSession, unsigned int aiSize, char *apContent );
extern int requestKeys(unsigned int aCount, KeyReg_t *pKeyReg);
//extern int processCommand(Msg_t *pMsg);
extern unsigned int getMsgId( Msg_t *pMsg );
extern unsigned int getMsgSession( Msg_t *pMsg );
extern unsigned int getMsgSize( Msg_t *pMsg );
extern unsigned char *getMsgContent( Msg_t *pMsg );
extern int acknowledgeCmd( unsigned int aMsgId );
extern unsigned int createSession(void);


 #endif
