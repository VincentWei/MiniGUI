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
** $Id: hzinput.c 8944 2007-12-29 08:29:16Z xwyan $
**
** hzinput.c: The GB2312 IME window.
**
** Orignally by KANG Xiaoning, some code from cce by He Rui.
**
** Copyright (C) 1998, 1999 He Rui (herui@cs.duke.edu)
** Copyright (C) 1999 Kang Xiaoning.
** Copyright (C) 2000 Zheng Xiang.
** Copyright (C) 2000, 2001, 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Current maintainer: Wei Yongming
** Create date: 1999/04/01
**
** Used abbreviations:
**  ime: input method
**  punc: punctuate mark
**
*/ 

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
** Modify records:
**
**  Who             When        Where           For What
**-----------------------------------------------------------------------------
**  WEI Yongming    1999/06/14  Tsinghua        Message dispatching
**  GONG Xiaorui    1999/07/01  HongShiFang     Soft keyboard
**  WEI Yongming    1999/07/17  Tsinghua        Add some local functions
**  WEI Yongming    1999/08/24  Tsinghua        Remove soft keyboard.
**  WEI Yongming    1999/08/29  Tsinghua        Handle some special keys.
**  Wei Yongming    1999/09/01  Tsinghua        Translat punc marks.
**  Wei Yongming    1999/09/01  Tsinghua        Translat punc marks.
**  Wei Yongming    2000/11/06  BluePoint       Merge pinyin module by Zheng Xiang.
**
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h" 
#include "minigui.h" 
#include "gdi.h" 
#include "window.h" 
#include "control.h" 
#include "endianrw.h" 
#include "misc.h"

#include "hzinput.h"
#include "pinyin.h"

/************ private functions *******************/
static BOOL hz_input_init(void);
static void hz_input_done(void);
static void unload_input_table(int i);
static BOOL hz_filter(unsigned char key, LPARAM lParam);
static int toggle_input_method(void);
static void toggle_half_full(void);
static void set_active_input_method(int);
static void refresh_input_method_area(HWND hwnd, HDC hDC);
static void DispSelection(HDC hDC);

static void FindMatchKey(void);
static void FillMatchChars(int j);
static void FillAssociateChars(int index);
static void FindAssociateKey(int index);

static hz_input_table* IntCode_Init(void);
static void IntCode_FindMatchKey(void);
static void IntCode_FillMatchChars(int index);
static void intcode_hz_filter(unsigned char key,LPARAM lParam);

/***************************************************************************
 *                           variable defines                              *
 ***************************************************************************/

#define VERSION_STRING    "Chinese Input"
static BOOL bTwoLines;
static char ime_tab_path [MAX_PATH + 1];

static const unsigned char    fullchar[]     =
    "¡¡£¡¡±££¡ç£¥£¦¡¯£¨£©£ª£«£¬£­£®£¯£°£±£²£³£´£µ£¶£·£¸£¹£º£»£¼£½£¾£¿"
    "£À£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú¡²£Ü¡³£Þ¡õ"
    "¡®£á£â£ã£ä£å£æ£ç£è£é£ê£ë£ì£í£î£ï£ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú£û£ü£ý¡«  ";

#define CPUNCTYPE_1TO1      0
#define CPUNCTYPE_1TO2      1
#define CPUNCTYPE_CYCLE     2

#define CTRLBTN_WIDTH       16
#define CTRLBTN_HEIGHT      16

typedef struct PuncMark
{
    unsigned char   epunc;
    int             type;
    int             currone;          // only for cycle
    unsigned char*  cpunc;
}PUNCMARK;

static PUNCMARK puncmark[] = 
{
    {'`', 0, 0, "¡¤"},
    {'~', 0, 0, "¡«"},
    {'!', 0, 0, "£¡"},
    {'@', 0, 0, "¡æ"},
    {'#', 0, 0, "££"},
    {'$', 0, 0, "£¤"},
    {'%', 0, 0, "£¥"},
    {'^', CPUNCTYPE_1TO2, 0, "¡­¡­"},
    {'&', 0, 0, "¡ì"},
    {'*', 0, 0, "¡Á"},
    {'_', CPUNCTYPE_1TO2, 0, "¡ª¡ª"},
    {'(', CPUNCTYPE_CYCLE, 0, "£¨£©"},
    {')', CPUNCTYPE_CYCLE, 0, "¡²¡³"},
    {'{', CPUNCTYPE_CYCLE, 0, "¡¶¡·"},
    {'}', CPUNCTYPE_CYCLE, 0, "¡´¡µ"},
    {'[', CPUNCTYPE_CYCLE, 0, "¡¼¡½"},
    {']', CPUNCTYPE_CYCLE, 0, "¡¾¡¿"},
    {'\\', 0, 0, "¡Â"},
    {'|', 0, 0, "¡ù"},
    {';', 0, 0, "£»"},
    {':', 0, 0, "£º"},
    {'\'', CPUNCTYPE_CYCLE, 0, "¡®¡¯"},
    {'\"', CPUNCTYPE_CYCLE, 0, "¡°¡±"},
    {',', 0, 0, "£¬"},
    {'.', 0, 0, "¡£"},
    {'<', CPUNCTYPE_CYCLE, 0, "¡¸¡¹"},
    {'>', CPUNCTYPE_CYCLE, 0, "¡º¡»"},
    {'?', 0, 0, "£¿"},
    {'/', 0, 0, "¡¢"}
};

static int IsOpened;        /* 1 for opened, 0 for closed */
static int IsHanziInput;    /* 0 for ascii, 1 for hanzi input */
static int IsFullChar;      /* 0 for half char, 1 for full char */
static int IsFullPunc;      /* 0 for half punctuate marks, 1 for full marks */
 
static int CurIME;
static int nIMENr;          /* effetive IME method number */

static HWND sg_hTargetWnd = 0; // target window of ime.

static hz_input_table *input_table[NR_INPUTMETHOD],*cur_table = NULL;
static char seltab[16][MAX_PHRASE_LENGTH];

static int CurSelNum=0;   /* Current Total Selection Number */
static unsigned long InpKey[MAX_INPUT_LENGTH],save_InpKey[MAX_INPUT_LENGTH];
   /* Input key buffer */
static int InputCount,InputMatch, StartKey,EndKey;
static int save_StartKey,save_EndKey, save_MultiPageMode, 
           save_NextPageIndex, save_CurrentPageIndex;
static int NextPageIndex,CurrentPageIndex,MultiPageMode;
/* When enter MultiPageMode:
   StartKey .... CurrentPageIndex .... NextPageIndex .... EndKey 
*/
static unsigned long val1, val2,key1,key2;
static int IsAssociateMode;
static int CharIndex[15];   // starting index of matched char/phrases

static const BITMAP* sg_bmpIME;    // the bmp of the IME control button

static int UseAssociateMode = 0;

/* 6 bit a key mask */
static const unsigned long mask[]=
{
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
  0x3F000000, 0x3FFC0000, 0x3FFFF000, 0x3FFFFFC0, 0x3FFFFFFF, 0x3FFFFFFF,
  0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF
};

/***************************************************************************
 *                          some local functions                           *
 ***************************************************************************/

static hz_input_table* load_input_method(char *filename)
{
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  int i;
#endif

  int  nread;
  FILE *fd;
  char phrase_filename[100],assoc_filename[100];
  hz_input_table *table;
  
  table = calloc (1, sizeof(hz_input_table));
  if (table == NULL)
    return NULL;

  fd = fopen(filename, "rb");
  if (fd == NULL) {
    free (table);
    return NULL;
  }

  nread = fread(table, sizeof(hz_input_table),1,fd);
  if (nread != 1) {
    goto fail;
  }
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  table->TotalKey = ArchSwap32(table->TotalKey);
  table->MaxPress = ArchSwap32(table->MaxPress);
  table->MaxDupSel= ArchSwap32(table->MaxDupSel);
  table->TotalChar= ArchSwap32(table->TotalChar);
  table->PhraseNum= ArchSwap32(table->PhraseNum);
  for (i = 0; i < 64; i++) {
    table->KeyIndex [i] = ArchSwap16(table->KeyIndex[i]);
  }
#endif

  if( strcmp(MAGIC_NUMBER, table->magic_number) ) {
    goto fail;
  }

  table->item = (ITEM *)malloc (sizeof(ITEM) * table->TotalChar); 
  if ( table->item == NULL ) {
    goto fail;
  }
 
  if (fread (table->item, sizeof(ITEM), table->TotalChar, fd) < table->TotalChar)
    goto fail;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  for (i = 0; i < table->TotalChar; i++) {
      table->item[i].key1 = ArchSwap32(table->item[i].key1);
      table->item[i].key2 = ArchSwap32(table->item[i].key2);
      table->item[i].frequency = ArchSwap16(table->item[i].frequency);
  }
#endif

  fclose (fd);
 
  if (table->PhraseNum > 0) {
     strcpy( phrase_filename, filename );
     strcat( phrase_filename, ".phr" );
     strcpy( assoc_filename, filename );
     strcat( assoc_filename, ".lx");
     
     table->PhraseFile = fopen( phrase_filename, "rb" );
     table->AssocFile = fopen( assoc_filename, "rb");

     if (table->PhraseFile == NULL || table->AssocFile == NULL) {
          printf("Load Phrase/Assoc File error!\n");
          free(table->item);
          free(table);
          return NULL;
     }
  }
  else {
       table->PhraseFile = table->AssocFile = NULL;
  }

  return table;

fail:
  free (table->item);
  free (table);
  fclose (fd);
  return NULL;
}

static void free_input_method(hz_input_table *table)
{
    free(table->item);
    free(table);
}

static void ClrIn(void)
{
  memset(InpKey, 0, sizeof( InpKey)); 
  memset( seltab, 0, sizeof( seltab));

  MultiPageMode = NextPageIndex = CurrentPageIndex = 0;
  CurSelNum = InputCount = InputMatch = 0;
  IsAssociateMode = 0;   /* lian xiang */
}

static void FindAssociateKey(int index)
{
    FILE *fp = cur_table->AssocFile;
    int ofs[2],offset;
   
    if (index < 0xB0A1) 
    {
       StartKey = EndKey = 0;
       return;  /* no match */
    }  
   
    offset = (index / 256 - 0xB0) * 94 + index % 256 - 0xA1; 
    fseek(fp, offset * sizeof(int), SEEK_SET);
    fread(ofs,sizeof(int),2,fp);
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    ofs[0] = ArchSwap32(ofs[0]);
    ofs[1] = ArchSwap32(ofs[1]);
#endif
    StartKey = 72 * 94 + 1 + ofs[0];
    EndKey = 72 * 94 + 1 + ofs[1];
}

static void load_phrase( int phrno, char *tt )
{
  FILE *fp = cur_table->PhraseFile;
  int ofs[2], len;

  fseek( fp, ( phrno + 1 ) << 2, SEEK_SET );
  fread( ofs, sizeof(int), 2, fp );
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
  ofs[0] = ArchSwap32(ofs[0]);
  ofs[1] = ArchSwap32(ofs[1]);
#endif
  len = ofs[1] - ofs[0];

  if ( len > 128 || len <= 0 ) {
     strcpy( tt, "error" );
     return;
  }

  ofs[0] += ( cur_table->PhraseNum + 1 ) << 2;
   /* Add the index area length */
  fseek( fp, ofs[0], SEEK_SET );
  fread( tt, 1, len, fp );
  tt[len] = 0;
}

static void putstr (unsigned char *p ,LPARAM lParam)
{
    int index,len = strlen(p);
    __mg_ime_writemsg(p, len, lParam, TRUE);
    if (InputCount <= InputMatch)  /* All Match */
    {
        index = (int)p[len-2] * 256 + p[len-1]; 
        ClrIn();
        if (UseAssociateMode)
        {
             FindAssociateKey(index); 
             CurrentPageIndex = StartKey;
             MultiPageMode = 0;
             FillAssociateChars(StartKey);  
             if (CurSelNum > 0)
             {
                IsAssociateMode = 1;
             }
        }
    }
    else
    {
      int nCount = InputCount - InputMatch,nMatch = InputMatch,i;
      MultiPageMode = NextPageIndex = CurrentPageIndex = 0;
      InputCount = InputMatch = 0;

      for(i = 0; i < nCount; i++)
        save_InpKey[i] = InpKey[nMatch+i];

      memset(InpKey, 0, sizeof(InpKey));
      for(i = 1; i <= nCount; i++)  /* feed the additional keys */
      {
         InpKey[InputCount] = save_InpKey[InputCount];
         InputCount++;
         if (InputCount <= InputMatch+1)
         {
             FindMatchKey();
             MultiPageMode = 0;
             CurrentPageIndex = StartKey;
             FillMatchChars(StartKey);
         } 
      }
      if (InputMatch == 0)    /* left key has no match, delete */
      {
         ClrIn(); 
         return;
      }
    }
}

static int epunc2cpunc (unsigned char epunc, unsigned char* cpunc)
{
    int i;
    
    for (i = 0; i < 29; i++) {
        if (puncmark [i].epunc == epunc) {
            switch (puncmark [i].type)
            {
                case CPUNCTYPE_1TO1:
                    memcpy (cpunc, puncmark [i].cpunc, 2);
                    return 2;
                case CPUNCTYPE_1TO2:
                    memcpy (cpunc, puncmark [i].cpunc, 4);
                    return 4;
                case CPUNCTYPE_CYCLE:
                    memcpy (cpunc, puncmark[i].cpunc + puncmark[i].currone, 2);
                    if (puncmark[i].currone == 0)
                        puncmark[i].currone = 2;
                    else
                        puncmark[i].currone = 0;
                    return 2;
                default:
                    return 0;
            }
        }
    }

    return 0;
}

/* After add/delete a char, search the matched char/phrase, update the
   StartKey/EndKey key,  save the related keys at first, if no match
   is found, we may restore its original value
*/
static void FindMatchKey(void)
{
  save_StartKey = StartKey;
  save_EndKey = EndKey;
  save_MultiPageMode = MultiPageMode;
  save_NextPageIndex = NextPageIndex;
  save_CurrentPageIndex = CurrentPageIndex;

  val1 = InpKey[4] | (InpKey[3]<<6) | (InpKey[2]<<12) | (InpKey[1]<<18) | 
        (InpKey[0]<<24);
  val2 = InpKey[9] | (InpKey[8]<<6) | (InpKey[7]<<12) | (InpKey[6]<<18) | 
        (InpKey[5]<<24);

  if (InputCount == 1)
     StartKey = cur_table->KeyIndex[InpKey[0]];
  else
     StartKey = CharIndex[InputCount-1];

  EndKey = cur_table->KeyIndex[InpKey[0]+1];

  for (; StartKey < EndKey; StartKey++)
  {
      key1 = (cur_table->item[StartKey].key1 & mask[InputCount+5]);
      key2 = (cur_table->item[StartKey].key2 & mask[InputCount]);

      if (key1 > val1) break;
      if (key1 < val1) continue;
      if (key2 < val2) continue;
      break;
  }
  CharIndex[InputCount] = StartKey;
}

/*  Find the matched chars/phrases and fill it into SelTab
    The starting checked index is j 
 
    The Selection Line 1xxx 2xxx,  80-20=60 60/2=30 chinese chars only
    0-9 Selection can contain only 30 chinese chars
*/

static void FillAssociateChars(int index)
{
    unsigned char str[25];
    int PhraseNo, CurLen = 0;
   
    CurSelNum = 0; 
    while( CurSelNum < cur_table->MaxDupSel && index < EndKey &&
              CurLen < MAX_SEL_LENGTH)
    {
         fseek( cur_table->AssocFile, index << 2, SEEK_SET );
         fread( &PhraseNo, sizeof(int), 1, cur_table->AssocFile );
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
         PhraseNo = ArchSwap32(PhraseNo);
#endif
         load_phrase( PhraseNo, str );
         strcpy(seltab[CurSelNum],str+2);
         CurLen += strlen(seltab[CurSelNum++]);
         index++;
    }

    /* check if more than one page */
    if ( index < EndKey && CurSelNum == cur_table->MaxDupSel )
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = index;
        MultiPageMode = 1;
    }
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

static void FillMatchChars(int j)
{
    int SelNum = 0, CurLen = 0;
    //bzero( seltab, sizeof( seltab ) );

    while( ( cur_table->item[j].key1 & mask[InputCount+5] ) == val1 &&
            ( cur_table->item[j].key2 & mask[InputCount] ) == val2 &&
              SelNum < cur_table->MaxDupSel && j < EndKey && 
              CurLen < MAX_SEL_LENGTH)
    {
          unsigned short ch = cur_table->item[j].ch;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
          ch = ArchSwap16 (ch);
#endif

          if (ch < 0xA1A1)
              load_phrase (ch, seltab[SelNum]);
          else
          {
              memcpy (&seltab[SelNum], &(cur_table->item[j].ch), 2);
              seltab [SelNum][2] = '\0';
          }
          CurLen += strlen(seltab[SelNum++]);
          j++;
    }
    
    if (SelNum == 0)  /* some match found */
    {
        StartKey = save_StartKey;
        EndKey = save_EndKey;
        MultiPageMode = save_MultiPageMode;
        NextPageIndex = save_NextPageIndex;
        CurrentPageIndex = save_CurrentPageIndex;
        return;    /* keep the original selection */
    }

    CurSelNum = SelNum;

    for(SelNum = CurSelNum; SelNum < 16; SelNum++)
       seltab[SelNum][0] = '\0';  /* zero out the unused area */
    InputMatch = InputCount; /* until now we have some matches */

    /* check if more than one page */
    if ( j < EndKey && (cur_table->item[j].key1 & mask[InputCount+5]) == val1 &&
         ( cur_table->item[j].key2 & mask[InputCount] ) == val2 &&
          CurSelNum == cur_table->MaxDupSel )
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = j;
        MultiPageMode = 1;
    }  
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

/* return value: Has output? */
static BOOL hz_filter(unsigned char key, LPARAM lParam)
{
    int inkey = 0,vv;
    char *is_sel_key = (char*)0;

    if (IsHanziInput) {
        unsigned char cc [2];

        if (IsFullPunc) {
            unsigned char cpunc[4];
            int len;
        
            len = epunc2cpunc (key, cpunc);

            if (len != 0) {
                __mg_ime_writemsg(cpunc, len, lParam, TRUE);
                ClrIn();
                return TRUE;
            }
        }
    
        if (IsFullChar && key >= ' ' && key <= 127) {
            key = (key - ' ') << 1;
            cc[0] = (unsigned char)(fullchar[key]);
            cc[1] = (unsigned char)(fullchar[key+1]);
       
            __mg_ime_writemsg(cc, 2, lParam, TRUE);
            return FALSE;
        }
    }
    else {
        __mg_ime_outchar (key, lParam);
        return FALSE;
    }

    if (CurIME == 0) {
        intcode_hz_filter(key, lParam);
        return TRUE;
    }

#ifdef _IME_GB2312_PINYIN
    if (CurIME == 1) {
        Pinyin_HZFilter (&Pinyin_Module, key, lParam);
        return TRUE;
    }
#endif

    switch ( key ) {
    case '\010':  /* BackSpace Ctrl+H */
    case '\177':  /* BackSpace */
        if ( InputCount > 0 ) 
        {
           InpKey[--InputCount]=0;
           if (InputCount == 0)
           {
               ClrIn();
  
           }
           else if (InputCount < InputMatch)
           {
               FindMatchKey();
               MultiPageMode = 0;
               CurrentPageIndex = StartKey;
               FillMatchChars(StartKey); 
           }
  
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case '\033':  /* ESCAPE */
        if (InputCount > 0) 
        {
           ClrIn();
 
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case '-':
        if ( MultiPageMode )
        {
           if ( CurrentPageIndex > StartKey) 
                CurrentPageIndex = CurrentPageIndex - cur_table->MaxDupSel;
           else CurrentPageIndex = StartKey;
           if (IsAssociateMode)
                FillAssociateChars(CurrentPageIndex);
           else FillMatchChars(CurrentPageIndex);
 
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case '=':
        if ( MultiPageMode )
        {
           CurrentPageIndex = NextPageIndex;
           if (IsAssociateMode)
               FillAssociateChars(CurrentPageIndex);
           else FillMatchChars(CurrentPageIndex);
           
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case ' ':
        if ( CurSelNum == 0 )
           __mg_ime_outchar(key,lParam);
        if ( seltab[0][0] )
           putstr(seltab[0],lParam);
        break;

     default:

        inkey   = cur_table->KeyMap[key];
        is_sel_key = strchr( cur_table->selkey, key);
        vv = is_sel_key - cur_table->selkey; 
          /* selkey index, strchr may return NULL */
 
      /* if a key is simultaneously inkey & is_sel_key, then selkey first?*/
        if ( (!inkey && !is_sel_key) || 
             (!inkey && is_sel_key && (CurSelNum == 0 || seltab[vv][0] == 0)) )
        {
           IsAssociateMode = 0;
           ClrIn();
           __mg_ime_outchar(key,lParam);
           break;
        }

        if (is_sel_key && CurSelNum > 0 && seltab[vv][0])
        {
            putstr(seltab[vv],lParam);
            break;
        }
        
        /* now it must be inkey? */
        IsAssociateMode = 0; 
        if ( inkey >= 1 && InputCount < MAX_INPUT_LENGTH )
            InpKey[InputCount++] = inkey;
             
        if (InputCount <= InputMatch+1) 
        {
             FindMatchKey();
             CurrentPageIndex = StartKey;
             MultiPageMode = 0;
             FillMatchChars(StartKey);
             if (InputCount >= cur_table->MaxPress && CurSelNum == 1 &&
                 cur_table->last_full)
             {
                // left only one selection 
                 putstr(seltab[0],lParam);
             }
        }
        
        break;

  } /* switch */

  return TRUE;
}


static void DispSelection(HDC hDC)
{
  int i, pos = 1;

  char str[100];
  char minibuf[2];
  minibuf[1]=0;
  str[0]=0;
  if (MultiPageMode && CurrentPageIndex != StartKey) 
  {
    strcat(str,"< ");
      pos += 2;
  }

  for( i = 0; i < CurSelNum; i++ )
  {
     if ( !seltab[i][0] ) {
        if (CurIME == 0 && i == 0) 
            continue; 
        else
            break;
     }

     minibuf[0]= input_table[CurIME]->selkey[i];
     strcat(str,minibuf);
     strcat(str,seltab[i]);
     strcat(str," "); 
  }

  if ( MultiPageMode && NextPageIndex != StartKey)  
  {
     strcat (str,"> ");
  }
  
  if (bTwoLines)
    TextOut (hDC, 2, 18, str);
  else
    TextOut (hDC, 150, 2, str);
}

static void unload_input_table(int i)
{
  if (((i >= 0) && (i < 10)) && input_table[i])
  {
    if (i == CurIME)
    {
      if (IsHanziInput)
          toggle_input_method();
      CurIME = 0;
    }

    if (input_table[i]->PhraseFile)
        fclose(input_table[i]->PhraseFile);
    if (input_table[i]->AssocFile)
        fclose(input_table[i]->AssocFile);

    free_input_method(input_table[i]);
    input_table[i] = NULL;
  }
}

static int toggle_input_method(void)
{
  if (input_table[CurIME] || CurIME == 1)
  {
    IsHanziInput ^= 1;

    cur_table = input_table[CurIME];
    ClrIn();
  }  

  return IsHanziInput;
}

static void toggle_half_full(void)
{
   IsFullChar ^= 1;
   ClrIn();
}

static void toggle_punc (void)
{
   IsFullPunc ^= 1;

   ClrIn();
}

static BOOL hz_input_init(void)
{
    char szFileName[MAX_PATH + 1];
    char szKey[10];
    char szValue[MAX_NAME + 1];
    int number;
    int i;

    if( GetMgEtcValue ("imeinfo", "imetabpath",
            ime_tab_path, MAX_PATH) < 0 ) {
        fprintf (stderr, "IME: Get imetab file path error!\n");
        return FALSE;
    }

    IsOpened = 1;
    IsHanziInput = 1;    // default is hanzi input 
    IsFullChar = 0;
    IsFullPunc = 0;
    CurIME = 0;
    input_table[0] = IntCode_Init();
    if (input_table[0] == NULL)
        return FALSE;

#ifdef _IME_GB2312_PINYIN
    if (!InitPinyinInput (ime_tab_path, &Pinyin_Module)) {
        fprintf (stderr, "IME: Init Pinyin module failure!\n");
        unload_input_table (0);
        return FALSE;
    }
    nIMENr = 2;
#else
    nIMENr = 1;
#endif

    if ((sg_bmpIME = GetSystemBitmap (HWND_DESKTOP, SYSBMP_IMECTRLBTN)) 
            == NULL) {
        fprintf (stderr, "IME: Load the control button failure!\n");
        return FALSE;
    }

    if( GetMgEtcValue  ("imeinfo", "imenumber",
            szValue, 10) < 0 )
        goto error;
        
    number = atoi (szValue);
    if(number <= 0)
        goto error;

    number = (number < NR_INPUTMETHOD) ? number : NR_INPUTMETHOD;

    for (i = 0; i < number; i++) {
        sprintf(szKey, "ime%d", i);
        if( GetMgEtcValue ("imeinfo",
            szKey, szValue, MAX_NAME) < 0 )
        goto error;
        
#ifdef _DEBUG
        fprintf (stderr, "IME: Loading Input Method %d: %s\n",i, szValue);
#endif

        strcpy(szFileName, ime_tab_path);
        strcat(szFileName, szValue);
        strcat(szFileName, ".tab");
        input_table [i + nIMENr] = load_input_method(szFileName);

        if (!input_table [i + nIMENr])
            goto error;
    }

    nIMENr += number;

    if (nIMENr > 1)
        CurIME = 1;
    cur_table = input_table [CurIME];

    return TRUE;
    
error:
#ifdef _IME_GB2312_PINYIN
    for (i = 0; i < nIMENr; i++) {
        if (i != 1)
            unload_input_table (i);
    }
    PinyinInputCleanup (ime_tab_path, &Pinyin_Module);
#else
    for (i = 0; i < nIMENr; i++)
        unload_input_table (i);
#endif

    return FALSE;
}

static void hz_input_done(void)
{
  int i;

  for (i = 0; i < NR_INPUTMETHOD; i++)
    if (input_table[i]) {
        if (input_table[i]->PhraseFile)
            fclose(input_table[i]->PhraseFile);
        if (input_table[i]->AssocFile)
            fclose(input_table[i]->AssocFile);

        free_input_method(input_table[i]);
    }

#ifdef _IME_GB2312_PINYIN
    PinyinInputCleanup (ime_tab_path, &Pinyin_Module);
#endif
}

static void set_active_input_method(int active)
{
#if _IME_GB2312_PINYIN
  if (input_table[active] || active == 1)
#else
  if (input_table[active])
#endif
  {
    CurIME = active;
    cur_table = input_table[active];
    ClrIn();
#if _IME_GB2312_PINYIN
    if (CurIME == 1 || CurIME == 0)
#else
    if (CurIME == 0)
#endif
        UseAssociateMode = 0;
    else 
        UseAssociateMode = 1;
  }
}

static void refresh_input_method_area(HWND hwnd, HDC hDC)
{
    RECT rc;
    char str[100];
    int i;
    char minibuf[2];
    WINDOWINFO *info;
    
    GetClientRect (hwnd, &rc);
    FillBoxWithBitmapPart (hDC, rc.right - 4 - 32, 2, 
                           CTRLBTN_WIDTH, CTRLBTN_HEIGHT,
                           0, 0,
                           sg_bmpIME,
                           0, (IsFullChar*CTRLBTN_HEIGHT));
        
    FillBoxWithBitmapPart (hDC, rc.right - 4 - 16, 2, 
                           CTRLBTN_WIDTH, CTRLBTN_HEIGHT,
                           0, 0,
                           sg_bmpIME,
                           CTRLBTN_WIDTH, (IsFullPunc*CTRLBTN_HEIGHT));
        
    if (!IsHanziInput) {
        rc.left += 2;
        rc.top += 2;
        info = (WINDOWINFO*)GetWindowInfo (hwnd);
        info->we_rdr->disabled_text_out (hwnd, hDC, "¡¾ En ¡¿", &rc, DT_SINGLELINE);
    }
#ifdef _IME_GB2312_PINYIN
    else if (CurIME != 1)
#else
    else
#endif
    {
        minibuf[1] = 0;
        strcpy (str, input_table[CurIME]->cname);
        
        for( i = 0; i <= MAX_INPUT_LENGTH ; i++)
        {
            if (i < InputCount)
                minibuf[0] = input_table[CurIME]->KeyName[InpKey[i]];
            else 
                minibuf[0]=' ';
                
            if (i == InputMatch && InputCount > InputMatch && i != 0)
                strcat (str,"-");
                
            strcat (str,minibuf);
        }
        
        TextOut (hDC, 2, 2, str);                                        
        DispSelection (hDC);
    }
#ifdef _IME_GB2312_PINYIN
    else
        RefreshPYInputArea (&Pinyin_Module, hDC, bTwoLines);
#endif
}

/*************************************************************************
 *                       Internal Code Input Method                      *
 *************************************************************************/

static hz_input_table *IntCode_Init(void)
{
  hz_input_table *table;
  int i,index;

  UseAssociateMode = 1;  /* force to no associate */
  table = malloc(sizeof(hz_input_table));
  if (table == NULL)
     {
#ifdef _DEBUG
      fprintf(stderr, "IME: load_input_method");
#endif
      return NULL;
     } 

   /* reset to zero. */
   memset (table, 0, sizeof (hz_input_table));

   strcpy(table->magic_number,MAGIC_NUMBER);
   strcpy(table->ename, "IntCode");
   strcpy(table->cname, "¡¾ÄÚÂë¡¿");
   strcpy(table->selkey, "0123456789abcdef");

   table->last_full = 1;
   for(i = 0; i < 128; i++)
   {
      table->KeyMap[i] = 0;
      if ((i >= '0' && i <= '9') || (i >= 'a' && i <= 'f'))
      {
         if (i >= '0' && i <= '9')
            index = i - '0';
         else index = i -'a' + 10;
         table->KeyMap[i] = index;
         table->KeyName[index] = toupper(i);
      }
   }    
   return table;
}

/* StartKey <= index < EndKey is valid */
static void IntCode_FindMatchKey(void)
{
   unsigned long Key = (InpKey[0] << 12) | (InpKey[1] << 8);
   switch(InputCount)
   {
      case 0:
      case 1:
         StartKey = EndKey = 0;    /* not display selection */
         break;
      case 2:
         StartKey = Key + 0xA1; 
         EndKey = Key + 0xFF;  /* A1-A9,B0-F7 A1-FE */
         break;
      case 3:
         StartKey = Key + (InpKey[2] << 4);
         if (InpKey[2] == 10) StartKey++; /* A1 */
         EndKey = StartKey + 0x10;  
         if (InpKey[2] == 15) EndKey--;  /* FE */ 
         break;
    }
}

/* ABCD  AB then C=a..f, Sel=0-9 begin display selection
        ABC then D=0-f,  Sel=0-f 
*/
static void IntCode_FillMatchChars(int index)
{
    int MaxSel,i;
   
    CurSelNum = 0;
    if (InputCount < 2) return;
    if (InputCount == 2) MaxSel = 10; else MaxSel = 16;
    
    if (index % 256 == 0xA1 && InputCount == 3) 
    {
       seltab[0][0] = '\0';
       CurSelNum++;
    }

    while( CurSelNum < MaxSel && index < EndKey)
    {
         seltab[CurSelNum][0] = index / 256; 
         seltab[CurSelNum][1] = index % 256;
         seltab[CurSelNum][2] = '\0';
         CurSelNum++;
         index++;
    }
    
    for(i = CurSelNum; i < 16; i++)
       seltab[i][0] = '\0';  /* zero out the unused area */

    InputMatch = InputCount;

    /* check if more than one page */
    if ( index < EndKey && CurSelNum == MaxSel && MaxSel == 10)
    {
      /* has another matched key, so enter MultiPageMode, has more pages */
        NextPageIndex = index;
        MultiPageMode = 1;
    }  
    else if (MultiPageMode)
    {
        NextPageIndex = StartKey; /* rotate selection */
    }
    else MultiPageMode = 0;
}

static void intcode_hz_filter(unsigned char key,LPARAM lParam)
{
  int inkey = 0;

  switch ( key )
  {
     case '\010':  /* BackSpace Ctrl+H */
     case '\177':  /* BackSpace */
        if ( InputCount > 0 ) 
        {
           InpKey[--InputCount]=0;
           if (InputCount == 0)
           {
               ClrIn();
           }
           else if (InputCount >= 2)
           {
               IntCode_FindMatchKey();
               MultiPageMode = 0;
               CurrentPageIndex = StartKey;
               IntCode_FillMatchChars(StartKey); 
               
           }
        }
        else __mg_ime_outchar(key,lParam);
        break;

   case '\033':  /* ESCAPE */
        if (InputCount > 0) 
        {
           ClrIn();
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case '-':
        if ( MultiPageMode )
        {
           if ( CurrentPageIndex > StartKey) 
                CurrentPageIndex = CurrentPageIndex - 10;
           else CurrentPageIndex = StartKey;
           IntCode_FillMatchChars(CurrentPageIndex);
           
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case '=':
        if ( MultiPageMode )
        {
           CurrentPageIndex = NextPageIndex;
           IntCode_FillMatchChars(CurrentPageIndex);
           
        }
        else __mg_ime_outchar(key,lParam);
        break;

     case ' ':
        if ( CurSelNum == 0 )
           __mg_ime_outchar(key,lParam);
        if ( seltab[0][0] )
           putstr(seltab[0],lParam);
        break;

     default:
        inkey   = cur_table->KeyMap[key];
        switch(InputCount)
        {
           case 0:  /* A-F */
           case 1:  /* 0-F */
           case 2:  /* A-F */
              if (inkey >=0 && !(InputCount != 1 && inkey < 10) &&
                   !(InputCount == 1 && 
                      ( (InpKey[0]==10 && inkey>9) ||       //A1-A9 
                        (InpKey[0]==10 && inkey==0) ||
                        (InpKey[0]==15 && inkey>7)) ) )      //B0-F7  
              {
                  InpKey[InputCount++] = inkey;
                  
                  if (InputCount >=2)
                  {
                      IntCode_FindMatchKey();
                      MultiPageMode = 0;
                      CurrentPageIndex = StartKey;
                      IntCode_FillMatchChars(StartKey);
                  
                  }
                  
              }
              else if (InputCount == 2 && inkey >= 0 && inkey < CurSelNum &&
                       seltab[inkey][0])
              {
                   putstr(seltab[inkey],lParam);
              }
              else /* not select key or input key */
              {
                  ClrIn();
                  __mg_ime_outchar(key,lParam);
                  return;
              }
              break;
           case 3:   /* output char */
              if (inkey >=1 && !(InpKey[2] == 10 && inkey == 0) &&
                  !(InpKey[2] == 15 && inkey == 15))
              {
                  seltab[0][0] = (InpKey[0] << 4) | InpKey[1];
                  seltab[0][1] = (InpKey[2] << 4) | inkey ;
                  seltab[0][2] = '\0';
                  putstr(seltab[0],lParam);
              }
              else
              {
                  ClrIn();
                  __mg_ime_outchar(key,lParam);
                  return;
              }
              break;
        }
  } /* switch */
}

/*************** below are interface to other modules **************/
/*  use these functions in other modules only */

/* set current active input method                   */
/* this function sets the active input method to ime */
/* ime value can be: 0 for internal code             */
/*                   1 for pinyin                    */
/*                   2 for wubi                      */
static void imeSetStatus(int iStatusCode, int Value)
{
    switch (iStatusCode) {
        case IS_ENABLE:
            if (Value && !IsHanziInput)
                toggle_input_method ();
            else if (!Value && IsHanziInput)
                toggle_input_method ();
        break;

        case IS_FULLCHAR:
            if (Value && !IsFullChar)
                toggle_half_full ();
            else if (!Value && IsFullChar)
                toggle_half_full ();
        break;

        case IS_FULLPUNC:
            if (Value && !IsFullPunc)
                toggle_punc ();
            else if (!Value && IsFullPunc)
                toggle_punc ();
        break;

        case IS_METHOD:
            if (Value < 0) {
                Value = (CurIME + 1) % nIMENr;
                set_active_input_method (Value);
            }
            else {
                if (Value >= nIMENr)
                    Value = nIMENr - 1;

                if (Value != CurIME)
                    set_active_input_method (Value);
                else
                    return;
            }
        break;

        default:
        break;
    }
}

static int imeGetStatus(int iStatusCode)
{
    switch (iStatusCode) {
        case IS_ENABLE:
            return IsHanziInput;
        break;

        case IS_FULLCHAR:
            return IsFullChar;
        break;

        case IS_FULLPUNC:
            return IsFullPunc;
        break;

        case IS_METHOD:
            return CurIME;
        break;

        default:
        break;
    }

    return 0;
}

static inline BOOL imeIsSpecialChar (int chr)
{
    if (chr == '\t' || chr == '\n' || chr == '\r')
        return TRUE;

    return FALSE;
}

static inline BOOL imeIsSpecialKey (int scancode)
{
    if (scancode == SCANCODE_TAB
            || scancode == SCANCODE_ENTER
            || scancode > SCANCODE_F1)
        return TRUE;

    return FALSE;
}

static inline BOOL imeIsEffectiveIME (void)
{
    return IsHanziInput || IsFullChar || IsFullPunc;
}

static int clean_left_ctrl = 1;


/*************************************************************************
 *                        extern function                                *
 *************************************************************************/
void __mg_ime_writemsg (BYTE *buffer, int len, LPARAM lParam, BOOL bDByte)
{
    WORD wDByte;
    int i;

    if (bDByte) {
        for (i=0; i<len; i+=2) {
            wDByte = MAKEWORD16 (buffer[i], buffer[i+1]);
            if (sg_hTargetWnd)
                PostMessage (sg_hTargetWnd, MSG_CHAR, wDByte, 0);
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            else
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, wDByte, 0);
#endif
        }
    }
    else {
        for (i=0; i<len; i++) {
            if (sg_hTargetWnd)
                PostMessage (sg_hTargetWnd, MSG_CHAR, buffer[i], 0);
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            else
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, buffer[i], 0);
#endif
        }
    }
}

void __mg_ime_outchar (unsigned char c, LPARAM lParam)
{
    int scancode = 0;

    switch (c) {
    case '\010':
    case '\177':  /* BackSpace */
        scancode = SCANCODE_BACKSPACE;
        break;
    case '\033':
        scancode = SCANCODE_ESCAPE;
        break;
    case '\t':
        scancode = SCANCODE_TAB;
        break;
    }
 
    if (scancode) {
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        if (!sg_hTargetWnd) {
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, scancode, (LPARAM)KS_IMEPOST);
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, scancode, (LPARAM)KS_IMEPOST);
            return;
        }
#endif
        PostMessage (sg_hTargetWnd, MSG_KEYDOWN, scancode, (LPARAM)KS_IMEPOST);
        PostMessage (sg_hTargetWnd, MSG_KEYUP, scancode, (LPARAM)KS_IMEPOST);
    }
    else
        __mg_ime_writemsg (&c, 1, lParam, FALSE);
}

/* this is the ime window proc */
int GBIMEWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static BOOL fCaptured = FALSE;
    static int oldx, oldy;
    int x, y;
    int maxx, maxy;
    RECT rcWindow;
    RECT *prcExpect, *prcResult;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (message == MSG_KEYDOWN || message == MSG_KEYUP) {
        if (wParam < SCANCODE_KEYPAD7 || wParam > SCANCODE_KEYPADPERIOD)
        {
            if (IsOpened && IsHanziInput) {
                MSG Msg = {hWnd, message, wParam, lParam};
                TranslateMessage (&Msg);
            }
        }
    }
    else if (message == MSG_SYSKEYDOWN) {
        if (sg_hTargetWnd)
            PostMessage (sg_hTargetWnd, message, wParam, lParam);
        else
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
    }
    else if (message == MSG_SYSKEYUP) {
        if (sg_hTargetWnd)
            PostMessage (sg_hTargetWnd, message, wParam, lParam);
        else
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
    }
#endif

    switch (message) {
        case MSG_NCCREATE:
            if (hz_input_init())
                /* Register before show the window. */
                SendMessage (HWND_DESKTOP, MSG_IME_REGISTER, (WPARAM)hWnd, 0);
            else
                return -1;
        break;

        case MSG_IME_OPEN:
            if (!IsOpened) {
                ShowWindow(hWnd, SW_SHOW);
                IsOpened = 1;
            }
        break;

        case MSG_IME_CLOSE:
            if (IsOpened) {
                ShowWindow(hWnd, SW_HIDE);
                IsOpened = 0;
            }
        break;
        
        case MSG_IME_SETSTATUS:
            imeSetStatus ((int)wParam, (int)lParam);
        break;

        case MSG_IME_GETSTATUS:
            return imeGetStatus ((int)wParam);
        break;

        case MSG_IME_SETTARGET:
            if (sg_hTargetWnd != hWnd)
                sg_hTargetWnd = (HWND)wParam;
        break;

        case MSG_IME_GETTARGET:
            return (int)sg_hTargetWnd;
        break;

        case MSG_SETFOCUS:
        case MSG_KILLFOCUS:
            return 0;

        case MSG_CHAR:
            if (IsOpened && !imeIsSpecialChar (wParam)) {
                if (hz_filter ((BYTE)wParam, lParam))
                    InvalidateRect (hWnd, NULL, TRUE);
            }
        return 0;

        case MSG_KEYDOWN:
            if (wParam != SCANCODE_LEFTCONTROL)
                clean_left_ctrl = 0;
            else
                clean_left_ctrl = 1;

            if (IsOpened) {
                if (wParam == SCANCODE_SPACE && (lParam & KS_CTRL)) {
                    toggle_half_full ();
                    InvalidateRect (hWnd, NULL, TRUE);
                    return 0;
                }
                else if (wParam == SCANCODE_PERIOD && (lParam & KS_CTRL)) {
                    toggle_punc ();
                    InvalidateRect (hWnd, NULL, TRUE);
                    return 0;
                }
                else if ((wParam == SCANCODE_LEFTSHIFT) && (lParam & KS_CTRL)) {
                    imeSetStatus (IS_METHOD, -1);
                    InvalidateRect (hWnd, NULL, TRUE);
                    return 0;
                }
        
#ifndef _LITE_VERSION
                if (sg_hTargetWnd && imeIsEffectiveIME () && imeIsSpecialKey (wParam))
                    PostMessage (sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
#else
                if (!IsHanziInput || (imeIsEffectiveIME () && imeIsSpecialKey (wParam))) {
                    if (sg_hTargetWnd)
                        PostMessage (sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
#ifndef _STAND_ALONE
                    else
                        Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#endif
                }
#endif
            }
#ifdef _LITE_VERSION
            else {
                if (sg_hTargetWnd)
                    PostMessage (sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
#ifndef _STAND_ALONE
                else
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#endif
            }
#endif
        return 0;

        case MSG_KEYUP:
            if (IsOpened) {
                if (wParam == SCANCODE_LEFTCONTROL && clean_left_ctrl ) {
                    toggle_input_method ();
                    InvalidateRect (hWnd, NULL, TRUE);
                }

#ifndef _LITE_VERSION
                if (sg_hTargetWnd && imeIsEffectiveIME () && imeIsSpecialKey (wParam))
                    PostMessage (sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
#else
                if (!IsHanziInput || (imeIsEffectiveIME () && imeIsSpecialKey (wParam))) {
                    if (sg_hTargetWnd)
                        PostMessage (sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
#ifndef _STAND_ALONE
                    else
                        Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
#endif
                }
#endif
            }
#ifdef _LITE_VERSION
            else {
                if (sg_hTargetWnd)
                    PostMessage (sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
#ifndef _STAND_ALONE
                else
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
#endif
            }
#endif
        return 0;
        
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetBkColor (hdc, GetWindowElementPixel (hWnd, WE_MAINC_THREED_BODY));
            refresh_input_method_area (hWnd, hdc);
            EndPaint (hWnd, hdc);
        return 0;

        case MSG_LBUTTONDOWN:
        {
            RECT rc;
            int zwidth = GetSysCCharWidth();
            int zheight = GetSysCharHeight();

            GetClientRect (hWnd, &rc);

            oldx = LOWORD (lParam);
            oldy = HIWORD (lParam);
            
            if (IsHanziInput && oldx > 0 && oldx < 4*zwidth && oldy > 2 && oldy < zheight+2){ 
                imeSetStatus (IS_METHOD, -1);
                InvalidateRect (hWnd, NULL, TRUE);
                return 0;
            }

            if (oldx > rc.right - 4 - 32 && oldx < rc.right - 4 - 16 && oldy > 2 && oldy < 16+2){
                toggle_half_full ();
                InvalidateRect (hWnd, NULL, TRUE);
                return 0;
            }
            if (oldx > rc.right - 4 - 16 && oldx < rc.right - 4 && oldy > 2 && oldy < 16+2){
                toggle_punc ();
                InvalidateRect (hWnd, NULL, TRUE);
                return 0;
            }
            
            SetCapture (hWnd);
            ClientToScreen (hWnd, &oldx, &oldy);
            fCaptured = TRUE;
        }
        break;

        case MSG_LBUTTONUP:
            ReleaseCapture ();
            fCaptured = FALSE;
        break;
        
        case MSG_MOUSEMOVE:
            if (fCaptured) {
                GetWindowRect (hWnd, &rcWindow);
                x = LOSWORD (lParam);
                y = HISWORD (lParam);
                OffsetRect (&rcWindow, x - oldx, y - oldy);
                MoveWindow (hWnd, rcWindow.left, rcWindow.top,
                              RECTW (rcWindow), RECTH (rcWindow), TRUE);
                oldx = x;
                oldy = y;
            }
        break;

        case MSG_SIZECHANGING:
            prcExpect = (PRECT)wParam;
            prcResult = (PRECT)lParam;

            *prcResult = *prcExpect;
            if (prcExpect->left < 0) {
                prcResult->left = 0;
                prcResult->right = RECTWP (prcExpect);
            }
            if (prcExpect->top < 0) {
                prcResult->top  = 0;
                prcResult->bottom = RECTHP (prcExpect);
            }

            maxx = GetGDCapability (HDC_SCREEN, GDCAP_HPIXEL);
            maxy = GetGDCapability (HDC_SCREEN, GDCAP_VPIXEL);

            if (prcExpect->right > maxx) {
                prcResult->right = maxx;
                prcResult->left = maxx - RECTWP (prcExpect);
            }

#if 0
            if (prcExpect->bottom > (maxy-80)) {
                prcResult->bottom = maxy-78;
                prcResult->top = (maxy-78) - RECTHP (prcExpect);
            }
#else
            if (prcExpect->bottom > maxy) {
                prcResult->bottom = maxy;
                prcResult->top = maxy - RECTHP (prcExpect);
            }
#endif

        return 0;
        
        case MSG_CLOSE:
            SendMessage (HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM)hWnd, 0);
            hz_input_done ();
            DestroyMainWindow (hWnd);
#ifndef _LITE_VERSION
            PostQuitMessage (hWnd);
#endif
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitIMEWinCreateInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_ABSSCRPOS | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_TOPMOST;
    pCreateInfo->spCaption = "The IME Window" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = GBIMEWinProc;
    pCreateInfo->lx = GetGDCapability (HDC_SCREEN, GDCAP_MAXX) - 400; 
    pCreateInfo->ty = GetGDCapability (HDC_SCREEN, GDCAP_MAXY) - 80; 
    pCreateInfo->rx = GetGDCapability (HDC_SCREEN, GDCAP_MAXX);
    pCreateInfo->by = GetGDCapability (HDC_SCREEN, GDCAP_MAXY) - 40;
    pCreateInfo->iBkColor = 
        GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY); 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = 0;
}

#ifdef _LITE_VERSION
HWND GBIMEWindowEx (HWND hosting, int lx, int ty, int rx, int by, BOOL two_lines)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;

#ifndef _STAND_ALONE
    if (!mgIsServer)
        return HWND_INVALID;
#endif

    InitIMEWinCreateInfo (&CreateInfo);
    CreateInfo.hHosting = hosting;
    if (lx != 0 || ty != 0 || rx != 0 || by != 0) {
        CreateInfo.lx = lx;
        CreateInfo.ty = ty;
        CreateInfo.rx = rx;
        CreateInfo.by = by;
        bTwoLines = two_lines;
    }
    else
        bTwoLines = TRUE;

    hMainWnd = CreateMainWindow(&CreateInfo);
    if (hMainWnd == HWND_INVALID)
        return HWND_INVALID;

    ShowWindow (hMainWnd, SW_SHOWNORMAL); 

    return hMainWnd;
}
#else

typedef struct ime_info
{
    sem_t wait;
    HWND hwnd;
} IME_INFO;

static void* start_gb_ime (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    IME_INFO* ime_info = (IME_INFO*) data;
    HWND ime_hwnd;

    InitIMEWinCreateInfo (&CreateInfo);
    bTwoLines = TRUE;

    ime_hwnd = ime_info->hwnd = CreateMainWindow (&CreateInfo);
    sem_post (&ime_info->wait);
    if (ime_hwnd == HWND_INVALID)
        return NULL;

    while (GetMessage (&Msg, ime_hwnd) ) {

        if (Msg.message == MSG_KEYDOWN || Msg.message == MSG_KEYUP) {
            if (IsOpened && IsHanziInput)
                TranslateMessage (&Msg);
            else if (sg_hTargetWnd) {
                PostMessage (sg_hTargetWnd, 
                    Msg.message, Msg.wParam, Msg.lParam | KS_IMEPOST);
            }
        }

        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (ime_hwnd);

    return NULL;
}

static pthread_t imethread;

/* the argument of 'hosting' is ignored. */
HWND GBIMEWindow (HWND hosting)
{
    IME_INFO ime_info;
    pthread_attr_t new_attr;

    sem_init (&ime_info.wait, 0, 0);

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&imethread, &new_attr, start_gb_ime, &ime_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&ime_info.wait);
    sem_destroy (&ime_info.wait);

    return ime_info.hwnd;
}

#endif

