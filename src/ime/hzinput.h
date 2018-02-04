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
** $Id: hzinput.h 7336 2007-08-16 03:40:38Z xgwang $
** 
** hzinput.h: head file for GB2312 IME.
**
** Copyright (C) 2000 by Zheng Xiang (zx@minigui.org) 
** Copyright (C) 2000, 2001, 2002, Wei Yongming
** Copyright (C) 2003 ~ 2007 Feynman Software.
*/

#ifndef __HZINPUT_H__
#define __HZINPUT_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
 
#define INPUT_BGCOLOR           8
  /* LightBlack */
#define INPUT_FGCOLOR           15
  /* LightWhite */
#define INPUT_AREAY          ((18 * 24)+10) 

#define MAX_INPUT_LENGTH        15

#define MAGIC_NUMBER            "CCEGB"
#define CIN_ENAME_LENGTH        24
#define CIN_CNAME_LENGTH        16
#define MAX_PHRASE_LENGTH       20 
#define SELECT_KEY_LENGTH       16 
#define END_KEY_LENGTH          16
#define MAX_SEL_LENGTH 			58

#define InputAreaX              16
/* key of toggle input method */

#define NR_INPUTMETHOD	10

typedef struct {
    unsigned long key1;        /* sizeof(ITEM=12) */
    unsigned long key2;
    unsigned short ch;
    unsigned short frequency;
} ITEM;

typedef struct {
    char magic_number[sizeof(MAGIC_NUMBER)];    /* magic number */
    char ename[CIN_ENAME_LENGTH];               /* ascii name */
    char cname[CIN_CNAME_LENGTH];               /* prompt */
    char selkey[SELECT_KEY_LENGTH];             /* select keys */

    char last_full;         /* last full code need a more SPACE? */
    int TotalKey;           /* number of keys needed */
    int MaxPress;           /* Max len of keystroke */
    int MaxDupSel;          /* how many keys used to select */
    int TotalChar;          /* Defined characters */

    unsigned char KeyMap[128];     /* Map 128 chars to 64(6 bit) key index */
    unsigned char KeyName[64];     /* Map 64 key to 128 chars */
    unsigned short KeyIndex[64];   /* 64 key first index of TotalChar */

    int PhraseNum;          /* Total Phrase Number */
    FILE *PhraseFile;       /* *.tab.phr Phrase File */
    FILE *AssocFile;        /* *.tab.lx LianXiang File */
    ITEM *item;             /* item */
} hz_input_table;

typedef struct {
        short l, t, r, b;
} hit_addr;  

void __mg_ime_writemsg (BYTE* buffer, int len, LPARAM lParam, BOOL bDByte);
void __mg_ime_outchar (unsigned char ch, LPARAM lParam);

#ifdef WIN32
extern int fread_long_win32(char* buff, size_t size, int count, FILE* fp);
#define fread fread_long_win32

#define F_OK 00
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
