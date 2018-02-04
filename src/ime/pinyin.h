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
#ifndef	_INPUT_MODULE_H_
#define	_INPUT_MODULE_H_

#define MAX_PY_NUM 410
 /* 1-405 */
#define MAX_EACH_PY 38
#define MAX_EACH_HZ 241
#define MAX_PY_LEN 7
#define MAX_SELECT_PH 1000
#define MAX_RES_NAME_LEN 60
#define MAX_CHAR_NUM 10
#define MAX_INPUT_BUF 100
#define MAX_HZ_BUF 250

#define MAX_PHRASE_LEN 6
#define MAX_PHRASE_COUNT 250
  /* same keyphrase phrase count, 106 in fact */

#define SEL_AREA_WIDTH 65
#define PINYIN_AREA_WIDTH 20

/***************/
#ifdef WIN32
typedef unsigned short u_short;
typedef unsigned char u_char;
#endif
/***************/

/* Max single char pinyin length: 6, zhuang, chuang ... */

typedef struct 
{
  u_short key;
  char py[MAX_PY_LEN];   // 7 + 2 = 9 bytes
} __attribute__((packed)) PinYin;

/* UsrPhrase is a linked list structure */
typedef struct _UsrPhrase
{
    struct _UsrPhrase *next;
    u_char reserve;// Byte alignment
    u_char len;    // phrase len
    u_char count;  // phrase number
    u_char key[1]; // pinyin key [len+1]
} __attribute__((packed)) UsrPhrase;

#define SizeOfPhrase(len,count)  ( 2 + (len)+1 + (count)*((len)*2+1) )

typedef struct _Phrase
{
	u_char len;   // phrase len
	u_char count; // phrase number
	u_char key[1];  // pinyin key [len+1]	
} __attribute__((packed)) Phrase;

typedef struct _SysPhrase
{
	u_short count;
	Phrase phrase[1];
} __attribute__((packed)) SysPhrase;


typedef struct _ChoiceItem
{
   Phrase *head;  // pointer to the len field
   u_char index;    // index of the phrase in that 
} __attribute__((packed)) ChoiceItem;

typedef char PYString[MAX_PY_LEN];

typedef struct _InputModule
{
  PinYin pytab[26][MAX_EACH_PY];
      // MAX_EACH_PY = 38    a[], b[], c[] ....
      // map the pinyin to keys
  SysPhrase *sysph[MAX_PY_NUM];   // system phrases
  int sys_size;

  int FuzzyPinyin;   // zh-ch-sh z-c-s
  UsrPhrase *usrph[MAX_PY_NUM];  //user defined phrase

  char inbuf[MAX_INPUT_BUF];	/* input buffer for keystrokes */
      // whole inputed pinyin string
  char inbuftmp[MAX_INPUT_BUF];
      // un-selected pinyin string
  char pybuftmp[MAX_INPUT_BUF];
      // selected pinyin string, hanzi

  PYString pinyin[2*MAX_PHRASE_LEN];
     // MAX_PY_LEN = 7, MAX_PHRASE_LEN = 6
  int lenpy;
      // current total pinyin len
  int pinyinpos;
     // current pinyin position

  u_char key[MAX_PHRASE_LEN+1];
  int lenkey;   // the pinyin keys of current selected phrase

  ChoiceItem sel[MAX_SELECT_PH];  // final selection
  Phrase *tempsel[MAX_PHRASE_LEN][500];  // temperoray usage
  int seltotal[MAX_PHRASE_LEN];

  int len;   // total selectable char/phrases
  int startpos;
  int endpos;   // startpos and endpos of showed selection

  char iapybuf[MAX_INPUT_BUF];
      // selected hanzi and inputed pinyin area
  char iahzbuf[MAX_HZ_BUF];
      // MAX_HZ_BUF = 250

} __attribute__((packed)) InputModule;   // about 30KB

/* PinYin input */
extern BOOL InitPinyinInput (const char* tabpath, InputModule* inmd);
extern void PinyinInputCleanup (const char* tabpath, InputModule *inmd);
extern void ResetPinyinInput (InputModule *inmd);
extern void Pinyin_HZFilter (InputModule* inmd, unsigned char key, LPARAM lParam);
extern void RefreshPYInputArea (InputModule *inmd, HDC hDC, BOOL bTwoLines);

extern InputModule Pinyin_Module;

#endif

