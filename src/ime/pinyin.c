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
** $Id: pinyin.c 7336 2007-08-16 03:40:38Z xgwang $
**
** pinyin.c: The GB2312 pinyin module.
**
** Porting to MiniGUI from CCE by Zheng Xiang
**
** Copyright (C) 2000 by Zheng Xiang (zx@minigui.org) 
** Copyright (C) 2000 ~ 2002 Wei Yongming
** Copyright (C) 2003 ~ 2007 Feynman Software.
** 
** Current maintainer: Wei Yongming
**
** Create date: 2000/11/01
*/

/*
 * Copyright 1999.1 by Li ZhenChun  zhchli@163.net
 *
 * CCE - Console Chinese Environment -
 * Copyright (C) 1998-1999 Rui He (herui@cs.duke.edu)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
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
** TODO: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef  __NOUNIX__
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif
 
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "endianrw.h"

#include "hzinput.h"
#include "pinyin.h"

static void PinyinKeyPressed(InputModule *inmd, char ch, LPARAM lParam);
static void SelectKeyPressed(InputModule *inmd, char ch, LPARAM lParam);
static void CreatePyMsg(InputModule *inmd);
static BOOL LoadPinyinTable (InputModule *inmd, char* pathname);
static BOOL LoadUsrPhrase( InputModule *inmd, char *pathname);
static BOOL LoadSysPhrase(InputModule *inmd, char *pathname);
static inline u_char *GetPhrase(ChoiceItem *p);
static inline u_char *GetFreq(ChoiceItem *p);
static int ParsePy(InputModule *inmd, char *pybuf, PYString pinyin[]);
static void FindMatchPhrase(InputModule *inmd,PYString pinyin[],int lenpy);
static void FillForwardSelection(InputModule *inmd,int startpos);
static void FillBackwardSelection(InputModule *inmd,int lastpos);
static int EffectPyNum(PYString pinyin[],int len);
static void SortOutput(InputModule *inmd);
static int QueryPhrase(InputModule *inmd, u_char *key, int len);
static void AdjustPhraseFreq (InputModule *inmd);
static void SaveUsrPhraseToMem (InputModule *inmd,u_char *str,u_char *key,int len,int freq);
static BOOL SaveUsrPhrase(InputModule *inmd, char *pathname);
static BOOL SaveSysPhrase(InputModule *inmd, char *pathname);

InputModule Pinyin_Module;

static void get_user_phrase_file (const char* tabpath, char* file_name)
{
#ifndef __NOUNIX__
    struct passwd *pwd;

    if ((pwd = getpwuid (geteuid ())) != NULL) {
        strcpy (file_name, pwd->pw_dir);
        if (file_name [strlen (file_name) - 1] != '/')
            strcat (file_name, "/");
        strcat (file_name, ".usrphrase.tab");
    }
    else {
        strcpy (file_name, tabpath);
        strcat (file_name, "usrphrase.tab");
    }
#elif defined(WIN32)
	strcpy (file_name, tabpath);
	strcat (file_name, "usrphrase.tab");
#endif
}

BOOL InitPinyinInput (const char* tabpath, InputModule *inmd)
{
    char file_name [MAX_PATH + 1];

    strcpy (file_name, tabpath);
    strcat (file_name, "pinyin.map");
    if (!LoadPinyinTable (inmd, file_name))
        return FALSE;

    strcpy (file_name, tabpath);
    strcat (file_name, "sysphrase.tab");
    if (!LoadSysPhrase (inmd, file_name))
        return FALSE;

    get_user_phrase_file (tabpath, file_name);
    if (!LoadUsrPhrase (inmd, file_name))
        return FALSE;

    ResetPinyinInput (inmd);
    return TRUE;
}

void PinyinInputCleanup (const char* tabpath, InputModule *inmd)
{
    char file_name [MAX_PATH + 1];
    AdjustPhraseFreq (inmd);   // lower the freq to [0,50)

    strcpy (file_name, tabpath);
    strcat (file_name, "sysphrase.tab");
    SaveSysPhrase (inmd, file_name);

    get_user_phrase_file (tabpath, file_name);
    SaveUsrPhrase (inmd, file_name);
}

void RefreshPYInputArea (InputModule *inmd, HDC hDC, BOOL bTwoLines)
{
  char str[100];

  strcpy(str, "¡¾Æ´Òô¡¿");
  strcat(str, inmd->iapybuf);
  TextOut(hDC, 2, 2, str);
  strcpy(str, inmd->iahzbuf);

  if (bTwoLines)
  {
        PLOGFONT logfont;
        SIZE size;
        logfont = GetSystemFont(SYSLOGFONT_CONTROL);
        SelectFont (HDC_SCREEN, logfont);
        GetTextExtent (HDC_SCREEN, str, strlen(str), &size);
        TextOut(hDC, 2, size.cy, str);
  }
  else
  {
    TextOut(hDC, 150, 2, str);
  }
}

void Pinyin_HZFilter(InputModule* inmd, unsigned char key, LPARAM lParam)
{
  if ( (key>='a' && key<='z') || (key=='\''&& strlen(inmd->inbuf)) || key=='\010' || key=='\177') 
      PinyinKeyPressed(inmd,key,lParam);
  else if (!strlen(inmd->inbuf)) {
     __mg_ime_outchar(key, lParam);
     return;
  }

  switch(key)
  {
     case '=':
        FillForwardSelection(inmd,inmd->endpos+1);
        break;
     case '-':
        FillBackwardSelection(inmd,inmd->startpos-1);
        break;
     case '\033':  //ESCAPE
        ResetPinyinInput(inmd);
        break;
     default:  // select some keys
       if ( (key>='1' && key<='9') || key=='0' || key==' ')
           SelectKeyPressed(inmd,key,lParam);
       break;
  }
}

void ResetPinyinInput(InputModule *inmd)
{
  *(inmd->inbuf)='\0';
  *(inmd->inbuftmp)='\0';
  *(inmd->pybuftmp)='\0';
  *(inmd->iapybuf)='\0';
  *(inmd->iahzbuf)='\0';

  inmd->len = 0;
  inmd->lenpy = 0;
  inmd->pinyinpos=0;
  inmd->lenkey=0;
  inmd->key[0]='\0';
}

static void PinyinKeyPressed(InputModule *inmd,char ch,LPARAM lParam)
{
  /* parameter strbuf is the newly inputed pinyin, inbuf the
    is the whole inputed pinyin, inbuftmp is the unselected pinyin */

  char strbuf[2];
  char *inbuf=inmd->inbuf;
  char *pybuftmp=inmd->pybuftmp;
    /* already selected Hanzi buffer */
  char *inbuftmp=inmd->inbuftmp;
    /* inputed pinyin buffer */

  char chtmp;
  int count;

  strbuf[0] = ch;
  strbuf[1] = '\0';

 /* \010 = Ctrl+H, \177 = BackSpace */
  if (ch == '\010' || ch == '\177')  // BackSpace
  {
      if (!strlen(inbuf)){
          __mg_ime_outchar(ch, lParam);
          return;
      }
      else if (!strlen(inbuftmp))
      {
          strcpy(inbuftmp,inbuf);
          *pybuftmp='\0';  // clear all the selected chars, reparse
      }
      else
      {
          inbuf[strlen(inbuf)-1] = '\0';
          inbuftmp[strlen(inbuftmp)-1] = '\0';  // cut one pinyin-char off
          if (!strlen(inbuf))
          {
              ResetPinyinInput(inmd);
              return;
          }
      }
  }
  else  //other than BackSpace, ch = a-z or '
  {
      if (strlen(inbuf) + strlen(strbuf) + 1 < MAX_INPUT_BUF) {
          strcat(inbuf,strbuf);
          strcat(inbuftmp,strbuf);
      } 
      else {
          Ping ();
#ifdef _DEBUG
          fprintf(stderr, "buffer overrun\n");
#endif
      }
  }

  if (!strlen(pybuftmp)) inmd->pinyinpos = 0;
    /* first pinyin char */

   // parse the unselected pinyin(inbuftmp) input
  count = ParsePy(inmd,inbuftmp,inmd->pinyin + inmd->pinyinpos);

  inmd->lenpy = inmd->pinyinpos + count;

    /* exclude the last i/u/v-beginning pinyin */
  if (inmd->lenpy > 0)
  {
      chtmp = inmd->pinyin[inmd->lenpy-1][0];
      if (chtmp=='i' || chtmp=='u' || chtmp=='v')
      {
          inbuf[strlen(inbuf)-1] = '\0';
          inbuftmp[strlen(inbuftmp)-1] = '\0';
          inmd->lenpy--;
          return;
      }
  }

    /* Too many chars now */
  if (EffectPyNum(inmd->pinyin,inmd->lenpy) > MAX_PHRASE_LEN)
  {
      inbuf[strlen(inbuf)-1] = '\0';
      inbuftmp[strlen(inbuftmp)-1] = '\0';
      inmd->lenpy--;
      return;
  }

  FindMatchPhrase(inmd,inmd->pinyin + inmd->pinyinpos,
           inmd->lenpy-inmd->pinyinpos);
  FillForwardSelection(inmd,0);
  CreatePyMsg(inmd);
  return;
}

static void SelectKeyPressed(InputModule *inmd,char ch,LPARAM lParam)
{
  ChoiceItem *phr=inmd->sel;

  char *pybuftmp=inmd->pybuftmp;
    /* already selected Hanzi buffer */
  char *inbuftmp=inmd->inbuftmp;
    /* inputed pinyin buffer */

  int i,j;
  u_char *fq;
  char strhz[MAX_PHRASE_LEN*2+1];
  int pos,idx;

  if (!inmd->len){
        __mg_ime_outchar(ch,lParam);
        return;
  }

  if (ch == ' ') idx = 0;
  else if (ch == '0') idx = 9;
  else idx = (int)ch-(int)'1';

  idx += inmd->startpos;

  if (idx > inmd->endpos)  return;  // out of range selection!

  strncpy(strhz,GetPhrase(phr+idx), MAX_PHRASE_LEN*2+1);
  if (strlen(pybuftmp) + strlen(strhz) + 1 < MAX_INPUT_BUF) {
      strcat(pybuftmp,strhz);
  } else {
      Ping ();
#ifdef _DEBUG
      fprintf(stderr, "buffer overrun\n");
#endif
  }
  inmd->key[0] |= phr[idx].head->key[0] << inmd->lenkey;

  for(i=1; i<=phr[idx].head->len; i++)
      inmd->key[(inmd->lenkey)++ +1] = phr[idx].head->key[i];
  /* pybuftmp, already selected chars */
  if (strlen(pybuftmp)/2 == (size_t)EffectPyNum(inmd->pinyin,inmd->lenpy))
  {
      if (strlen(strhz) == strlen(pybuftmp) )
      {
          fq = GetFreq(phr+idx);
          if (*fq < 250) (*fq)++;
              /* strhz is the last phrase/char, equal, existing phrase/char
                 increase its frequency */
      }
      else if(strlen(pybuftmp) > 2)
      {
          SaveUsrPhraseToMem(inmd,
                    pybuftmp,inmd->key,strlen(pybuftmp)/2,1);
              // not equal and pybuftmp, save the new phrase, 0 is user phrase
      }

      __mg_ime_writemsg(pybuftmp, strlen(pybuftmp), lParam, 1); //1 means as the format of WORD
 
      ResetPinyinInput(inmd);
      return;//strlen(strbuf);
          /* All the pinyin are translated to char/phrases!  */
  }
  else  // not yet, some unselected pinyin exist
  {
      // forward the pinyinpos pointer
      for(pos = strlen(strhz)/2;  pos > 0 ; inmd->pinyinpos++)
      {
         ch = inmd->pinyin[inmd->pinyinpos][0];
         if (ch=='i' || ch=='u' || ch=='v' || ch<'a' || ch>'z') continue;
         pos--;
      }

      FindMatchPhrase(inmd, inmd->pinyin + inmd->pinyinpos,
               inmd->lenpy - inmd->pinyinpos);

      FillForwardSelection(inmd,0);

      *inbuftmp = '\0';  // put the rest of the pinyin into inbuftmp
      for(j = inmd->pinyinpos; j < inmd->lenpy; j++) {
          if (strlen(inbuftmp) + strlen(inmd->pinyin[j]) + 1 < MAX_INPUT_BUF) {
              strcat(inbuftmp, inmd->pinyin[j]);
          } 
          else {
              Ping ();
#ifdef _DEBUG
              fprintf(stderr, "buffer overrun\n");
#endif
          }
      }

      CreatePyMsg(inmd);

      return;
   }
}

static void CreatePyMsg(InputModule *inmd)
{
  int i;

  strncpy(inmd->iapybuf,inmd->pybuftmp, MAX_INPUT_BUF-1);
  for(i=inmd->pinyinpos; i<inmd->lenpy; i++)
  {
      if (strlen(inmd->iapybuf)+strlen(inmd->pinyin[i])+2 >= MAX_INPUT_BUF-1) {
#ifdef _DEBUG
          fprintf(stderr, "buffer overrun\n");
#endif
      }
      strcat(inmd->iapybuf,inmd->pinyin[i]);   // MAX_PY_LEN = 7
      if (inmd->pinyin[i+1][0] == '\'' || inmd->pinyin[i][0] == '\'')
          continue;
      else
          strcat(inmd->iapybuf," ");
  }
}

static BOOL LoadPinyinTable (InputModule *inmd, char* pathname)
{
    FILE *stream;
    char str[250],strpy[15],strhz[241];
    int i=0, j=0, lastpy=0, curpy;
   
    if ( (stream = fopen (pathname, "rb" )) == NULL ) {
        fprintf(stderr, "IME (Pinyin): Can not open Pinyin Table to read.\n");
        return FALSE;
    }

    while ( !feof( stream )) {
        if ( fgets(str,250,stream) != NULL) {
            sscanf(str,"%14s %240s",strpy,strhz);
            curpy = strpy[0]-'a';
            if (curpy != lastpy) j = 0; 
            strncpy( inmd->pytab[curpy][j].py,strpy, MAX_PY_LEN);
            inmd->pytab[curpy][j].key = i+1;
            lastpy = curpy;
            i++,j++;
        }
    }
    fclose(stream);
  
    return TRUE;
}

static BOOL LoadUsrPhrase (InputModule *inmd, char *pathname)
{
    FILE *stream;
    UsrPhrase *kph,*tmp;
    int i,j,ahead,fsize;
    u_short count;
    u_char len, size;

    if (access (pathname, F_OK)) {
        for (i = 0; i < MAX_PY_NUM; i++)
            inmd->usrph[i] = NULL;
        return TRUE;
    }

    if ((stream = fopen (pathname, "rb")) == NULL ) {
        fprintf (stderr, "IME (Pinyin): %s file can't be opened.\n", pathname);
        return FALSE;
    }

    if (fseek (stream, -4, SEEK_END) == -1 ||
        fread (&fsize, sizeof(int), 1, stream) != 1 ||
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
        (fsize = ArchSwap32 (fsize)) == 0 ||
#endif
        fsize != ftell (stream) - 4)
    {
       fprintf (stderr, "IME (Pinyin): %s is not a valid pinyin phrase file.\n", pathname);
       fclose (stream);
       return FALSE;
    }

    fseek (stream, 0, SEEK_SET);

    for(i = 1; i < MAX_PY_NUM; i++) {
       inmd->usrph[i] = NULL;
       fread (&count, sizeof(count), 1, stream);
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
       count = ArchSwap16 (count);
#endif
       if (count == 0) continue;

       for(j = 0; j < count; j++)
       {
          fread(&len,sizeof(len),1,stream);
          fread(&size,sizeof(size),1,stream);
          
          if ((kph = (UsrPhrase *)malloc(6+len+1+(2*len+1)*size)) == NULL) {
                fprintf (stderr, "IME (Pinyin): No enough memory.\n");
                fclose (stream);
                return FALSE;
          }

          kph->len = len;
          kph->count = size;
          kph->next = NULL;
          fread (kph->key, sizeof(char), len+1, stream);
          fread (kph->key + len + 1, 2*len+1, size, stream);

          ahead = kph->key[1];
          ahead |= (kph->key[0] & 0x01) << 8;

         if (inmd->usrph[ahead] == NULL) 
            inmd->usrph[ahead] = kph;
         else
         {
            tmp = inmd->usrph[ahead];
            while (tmp->next != NULL)
                 tmp = tmp->next;
            tmp->next = kph;
         }
      }
   }

   fclose(stream);
   return TRUE;
}

// Load the system and user phrase library
// the phrase file can be combined just by cat a >> b

static BOOL LoadSysPhrase (InputModule *inmd, char *pathname)
{
    FILE *stream;
    Phrase *kph;
    SysPhrase *sysph;
    char *p;
    int i,j;

    if ( (stream = fopen(pathname , "rb" )) == NULL ) {
        fprintf (stderr, "IME (Pinyin): %s file can't open\n", pathname);
        return FALSE;
    }

    if (fseek (stream, -4, SEEK_END) == -1 ||
      !(inmd->sys_size = MGUI_ReadLE32FP (stream)) ||
      inmd->sys_size != ftell (stream)-4)   // error!!
    {
        fprintf (stderr, "IME (Pinyin): %s is not a valid pinyin phrase file.\n", 
                pathname);
        fclose (stream);
        return FALSE;
    }
 
    fseek (stream, 0, SEEK_SET);

    if ((p = malloc(inmd->sys_size)) == NULL) {
        fprintf (stderr, "IME (Pinyin): No enough memory.\n");
        fclose (stream);
        return FALSE;
    }

    if (fread (p, inmd->sys_size, 1, stream) != 1) {
        fprintf (stderr, "IME (Pinyin): Loading system phrase error.\n");
        fclose (stream);
        free (p);
        return FALSE;
    }
   
    for(i = 1; i < MAX_PY_NUM; i++) {
      inmd->sysph[i] = sysph = (SysPhrase*)p;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
      sysph->count = ArchSwap16 (sysph->count);
#endif

      p = (char*)sysph->phrase;
      for(j = 0; j < sysph->count; j++) {
         kph = (Phrase*)p;
         p += SizeOfPhrase (kph->len,kph->count);  // skip the string 
      }
   }

   fclose (stream);
   return TRUE;
}

static void AdjustPhraseFreq (InputModule *inmd)
{
  UsrPhrase *uph;
  SysPhrase *sysph;
  Phrase *sph;
  int i,j,k,index;
  char *p;

  for(i = 1; i < MAX_PY_NUM; i++)
  {
     // user phrases
     for(uph = inmd->usrph[i]; uph != NULL; uph = uph->next)
     {
         for(k =0; k < uph->count; k++)
         {
            index = uph->len+1 + (2*uph->len+1)*k + 2*uph->len;
            if (uph->key[index] > 25)
                uph->key[index] = 25 + (uph->key[index]-25)/10;
         }
     }   

     // system phrases
     sysph = inmd->sysph[i];
     p = (char*)sysph->phrase;   // count = total pinyin number
     for(j = 0; j < sysph->count; j++)
     {
        sph = (Phrase *)p;
        for(k = 0; k < sph->count; k++)
        {
            index = sph->len+1 + (2*sph->len+1)*k + 2*sph->len;
            if (sph->key[index] > 25)
                 sph->key[index] = 25 + (sph->key[index] - 25)/10;
        }   
        p += SizeOfPhrase(sph->len,sph->count);
     }
  }

  return;
}

static BOOL SaveSysPhrase (InputModule *inmd, char *pathname)
{
    FILE *out;
    char *p;

    if ( (out = fopen( pathname, "wb")) == NULL) {
        fprintf (stderr, "IME (Pinyin): %s cant open for saving.\n", pathname);
        return FALSE;
    }

    p = (char*)(inmd->sysph[1]);

#if MGUI_BYTEORDER == MGUI_LIL_ENDIAN
    fwrite (p, inmd->sys_size, 1, out);
    fwrite (&(inmd->sys_size), sizeof(int), 1, out);
#else
  {
    int i, j;
    for (i = 1; i < MAX_PY_NUM; i++) {
        SysPhrase *sysph = (SysPhrase*)p;
        int phrase_size;

        MGUI_WriteLE16FP (out, sysph->count);

        p = (char*)sysph->phrase;
        for (j = 0; j < sysph->count; j++) {
            Phrase* kph = (Phrase*)p;
            phrase_size = SizeOfPhrase (kph->len, kph->count);
            fwrite (p, phrase_size, 1, out);
            p += phrase_size;
        }
    }
    MGUI_WriteLE32FP (out, inmd->sys_size);
    p = (char*)(inmd->sysph[1]);
  }
#endif

    free (p);
    fclose (out);
    return TRUE;
}

static BOOL SaveUsrPhrase (InputModule *inmd, char *pathname)
{
    int i,fsize = 0, tmpcount;
    FILE *out;
    UsrPhrase *p0,*q0;
    u_short total;
    u_char len,count;

    if ((out = fopen (pathname,"wb")) == NULL) {
        fprintf (stderr, "IME (Pinyin): %s cant open for saving.\n", pathname);
        return FALSE;
    }

    for (i=1; i<MAX_PY_NUM; i++) {
      total = 0;
      fsize += sizeof(u_short); // for total

      p0 =  inmd->usrph[i];
      if (p0 == NULL) {
        MGUI_WriteLE16FP (out, total);
        continue;
      }

      // first, calculate the different pinyin key phrases
      total++;
      for (p0 = p0->next; p0 != NULL; p0 = p0->next) {
         len = p0->len;
         for (q0 = inmd->usrph[i]; q0 != p0; q0 = q0->next)
            if (q0->len == len && !memcmp(p0->key, q0->key, len+1)) break;
         if (p0 == q0) total++;  
      }

      MGUI_WriteLE16FP (out, total);

      for (p0 = inmd->usrph[i]; p0 != NULL; p0 = p0->next) {
          if (p0->count == 0) continue;

          tmpcount = p0->count;
          len = p0->len;
          for(q0 = p0->next; q0 != NULL; q0 = q0->next)
             if (q0->len == len && !memcmp(q0->key,p0->key,len+1))
                 tmpcount += q0->count;

          if (tmpcount > 255)
              tmpcount = 255;
          else count = (u_char)tmpcount;
 
          fsize += SizeOfPhrase(len,count);

          // u_char count, count must less than 256 phrases       
          fwrite(&len,sizeof(len),1,out);
          fwrite(&count,sizeof(count),1,out);
          fwrite(p0->key,len+1,1,out);

          for(q0 = p0; q0 != NULL; q0 = q0->next)
             if (q0->len == len && !memcmp(q0->key,p0->key,len+1))
             {
                 fwrite(q0->key+len+1,q0->count*(2*len+1),1,out);
                 q0->count = 0; 
             }
       }
    }

    MGUI_WriteLE32FP (out, fsize);
    fclose (out);

    for (i = 1; i < MAX_PY_NUM; i++)
       free (inmd->usrph[i]);

    return TRUE;
}

static void SaveUsrPhraseToMem(InputModule *inmd,u_char *str,u_char *key,int len,int freq)
{
  UsrPhrase *kph, *tmp;
  short ahead;
  
  if (len<1) return;

  ahead = (short)key[1];
  ahead |= (key[0] & 0x01) << 8;

  /* allocate a new one, not combine, will combine during saving */
  if ((kph = (UsrPhrase *)malloc(4+SizeOfPhrase(len,1))) == NULL)
      {printf("Not enough memory\n");return;}

  kph->len = len;
  memcpy(kph->key,key,len+1);
  kph->count = 1; 
  kph->next = NULL;
  memcpy(kph->key + len+1,str,len*2);
  kph->key[len+1+2*len] = freq;

  if (inmd->usrph[ahead] == NULL) 
      inmd->usrph[ahead] = kph;
  else
  {
      tmp = inmd->usrph[ahead];
      while (tmp->next != NULL)
         tmp = tmp->next;
      tmp->next = kph;
  }
}

static inline u_char *GetPhrase(ChoiceItem *p)
{
   static char temp[2*MAX_PHRASE_LEN+1];
   int len = (int)(p->head->len);
   if (len > MAX_PHRASE_LEN) {
#ifdef _DEBUG
       fprintf(stderr, "buffer overrun\n");
#endif
   }
   strncpy(temp,p->head->key + len + 1 + p->index *(2*len+1),2*len);
   temp[2*len] = '\0';
   return temp;
}

static int ParsePy(InputModule *inmd, char *pybuf, PYString pinyin[])
{
  int len, ahead,i, total = 0;
  int offset = 0, count, valid;
 
  len = strlen(pybuf);
  if (len < 1 || len > MAX_PHRASE_LEN * (MAX_PY_LEN+1) ) return 0;
  
  count = 2;  /* 1 always valid */
  while (offset + count <= len)
  {
      if (pybuf[offset] == '\'')  // delimitor ' 
      {
          strcpy(pinyin[total++],"\'");
          offset++; count = 2; continue;
      }

      ahead = pybuf[offset] - 'a';
      if (ahead < 0 || ahead > 25) return 0;

      // test if this is a valid pinyin prefix
      valid = 0;
      for(i=0; inmd->pytab[ahead][i].key; i++)
      {
         if ( !strncmp(pybuf+offset,inmd->pytab[ahead][i].py,count) )
         {
            valid = 1; break;
         }
      } 

     if (valid) count++;
     else
     {
         strncpy(pinyin[total], pybuf+offset, count-1);
         pinyin[total++][count-1] = '\0';
         offset += count-1;
         count = 2;
     }
  }

  // copy the remaining pinyin
  if (offset < len)
  {
    strncpy(pinyin[total], pybuf+offset, count-1); 
    pinyin[total++][count-1] = '\0';
  }
  return total;
}

static void FindMatchPhrase(InputModule *inmd,PYString pinyin[],int lenpy)
{
  int lenkey,keytmp;
  int i,j,k;
  int ahead,tmplen, count=0;
  int pykey[MAX_PHRASE_LEN][MAX_EACH_PY+1];
          // MAX_PHRASE_LEN=6, MAX_EACH_PY = 38, a[], b[]
  u_char py[MAX_PY_LEN+2];
  u_char key[MAX_PHRASE_LEN+1];
  u_char keyarr[MAX_PHRASE_LEN][500][MAX_PHRASE_LEN+1];
    // temporary array, 500 items

  int lenarr[MAX_PHRASE_LEN],result;
  char ch;

  if (!lenpy)
  {
    inmd->len = 0;
    return;
  }

  /* first of all, fill the pykey array */
  for (i=0; i<lenpy; i++)
  {
    ch = pinyin[i][0];
    if (ch == 'i' || ch == 'u' || ch == 'v' || ch < 'a' || ch > 'z') 
        continue;  // ignore the i/u/v beginning and non a-z 

    ahead = pinyin[i][0] - 'a';
    lenkey=0;
    tmplen=strlen(pinyin[i]);
    for(j=0; (keytmp = inmd->pytab[ahead][j].key); j++)
    {
      if ( tmplen == 1 || !strncmp(pinyin[i],inmd->pytab[ahead][j].py,tmplen))
          // prefix match
      {
        pykey[count][lenkey++] = keytmp;
        continue;
      }
      else if (inmd->FuzzyPinyin && (ch == 'z' || ch == 'c' || ch == 's')) 
      { 
         if (pinyin[i][1] != 'h')
         {
            strncpy(py+1,pinyin[i], MAX_PY_LEN);
            py[0] = py[1]; 
            py[1] = 'h';
         }
         else
         {
            strncpy(py,pinyin[i]+1, MAX_PY_LEN);
            py[0] = ch;
         }  
         if (!strncmp(py,inmd->pytab[ahead][j].py,strlen(py)))
            pykey[count][lenkey++] = keytmp;
      }
    }
    pykey[count++][lenkey] = 0;
  }  // for i = 1 to lenpy, pykey array filled

  if (count > MAX_PHRASE_LEN - 1) {
#ifdef _DEBUG
      fprintf(stderr, "buffer overrun\n");
#endif
      return;
  }
  for(i=0; i<MAX_PHRASE_LEN; i++) lenarr[i]=0;

  for(i = 0; i < 8; i++) inmd->seltotal[i] = 0;

  /* for the first char */
  for(k=0; pykey[0][k]; k++)
  {
    key[1] = pykey[0][k] & 0xff;
    key[0]='\0';
    key[0] |= (pykey[0][k] & 0x0100) >> 8;

    //  single char phrase
    result = QueryPhrase(inmd, key, 1);
    if (result > 0)  //save the possible multiple-char phrases 
        memcpy(keyarr[0][lenarr[0]++],key,2);
  }

  /* count is the real pinyin number, parse the remaining */
  for(i=1; i<count; i++)
    for(j=0; j<lenarr[i-1]; j++)  // for all the possible phrase match
      for(k=0; pykey[i][k]; k++)
      {
        memcpy(key,keyarr[i-1][j],i+1);
        key[i+1] = pykey[i][k] & 0xff;
        key[0] |= (pykey[i][k] & 0x0100) >> (8-i);

        result = QueryPhrase(inmd, key, i+1);
        if (result > 0)
          memcpy(keyarr[i][lenarr[i]++], key,i+2);
      }

  SortOutput(inmd);
}

static void SortOutput(InputModule *inmd)
{
  int i,j,k,lenph,totalph = 0;
  ChoiceItem phtmp, *ph = inmd->sel;

  totalph = 0;
  for(i = MAX_PHRASE_LEN-1; i >= 0; i--)
  {
      lenph = 0;
      for(j = 0; j < inmd->seltotal[i]; j++)
      {
         for(k = 0; k < inmd->tempsel[i][j]->count; k++)
         {
            inmd->sel[totalph+lenph].head = inmd->tempsel[i][j];
            inmd->sel[totalph+lenph].index = k;
            lenph++;
         }
      }

     // sort the phrases 
     for(k=0; k<lenph; k++)
     for(j=k; j<lenph; j++)
     if (*GetFreq(ph+k) < *GetFreq(ph+j))
     {
        phtmp = ph[k];
        ph[k] = ph[j];
        ph[j] = phtmp;
     }

     totalph += lenph; 
     ph += lenph;
  }

  inmd->len = totalph;  // total possible phrase selection
}

static inline u_char *GetFreq(ChoiceItem *p)
{
   int len = (int)(p->head->len);
   return (u_char*)(p->head->key + len + 1 + p->index *(2*len+1)+ 2*len);
}

static void FillForwardSelection(InputModule *inmd,int startpos)
{
    char *iahzbuf=inmd->iahzbuf;

    int i,count;
//    int SelAreaWidth = dispInfo.tx_avail - 10 - PINYIN_AREA_WIDTH;
    char strtmp[2*MAX_PHRASE_LEN+10];
    if (startpos > inmd->len - 1 || startpos < 0)
        return ;   // non-forwardable, keep the iahzbuf intact
    iahzbuf[0] = '\0';
    if (inmd->len < 1) return;  // clear the iahzbuf

    count = 0;   // backup the starting position
    inmd->startpos = startpos;
    inmd->endpos = startpos - 1;
    if (inmd->startpos > 0)
       snprintf(inmd->iahzbuf,MAX_HZ_BUF,"< ");
    else snprintf(inmd->iahzbuf,MAX_HZ_BUF,"  ");

    while(inmd->endpos < inmd->len-1 && count < 10)
    {
       snprintf(strtmp,sizeof(strtmp),"%d%s ",(count+1)%10,
             GetPhrase(inmd->sel+inmd->endpos+1));
       strcat(iahzbuf,strtmp);
       inmd->endpos++;
       count++;
    }
    if (inmd->endpos < inmd->len - 1 && count >= 1)
    {
      for(i = strlen(iahzbuf); i < SEL_AREA_WIDTH-2; i++)
         iahzbuf[i] = ' ';
      iahzbuf[MAX_HZ_BUF-2] = '>';
      iahzbuf[MAX_HZ_BUF-1] = '\0';
    }
}

static void FillBackwardSelection(InputModule *inmd,int lastpos)
{
    char *iahzbuf=inmd->iahzbuf;

 //   int SelAreaWidth = dispInfo.tx_avail - 10 - PINYIN_AREA_WIDTH;

    int count,ialen;
    char strbuf[2*MAX_PHRASE_LEN+10];

    if (lastpos < 0 || lastpos > inmd->len-1)
        return;   // iahzbuf intact
    iahzbuf[0] = '\0';
    if (inmd->len < 1) return; // clear iahzbuf

    count = 0;
    inmd->endpos = lastpos;
    ialen = 2;   // leftmost "< " or "  "
    inmd->startpos = lastpos+1;
    while(inmd->startpos > 0 && count < 10)
    {
        strncpy(strbuf,GetPhrase(inmd->sel+inmd->startpos-1),sizeof(strbuf));
        ialen += strlen(strbuf)+2;
        if (ialen+2 > MAX_HZ_BUF) break;
        count++;
        inmd->startpos--;
    }
    FillForwardSelection(inmd,inmd->startpos);
}

static int EffectPyNum(PYString pinyin[],int len)
{
  int i;
  char ch;
  int count=0;

  for(i=0; i<len; i++)
  {
    ch = pinyin[i][0];
    if (ch == 'i' || ch == 'u' || ch == 'v') continue;
    if (ch < 'a' || ch >'z') continue;
    count++;
  }
  return count;
}


static int QueryPhrase(InputModule *inmd, u_char *key, int len)
{
  short ahead;
  UsrPhrase *uph;
  char *p;
  SysPhrase *sysph;
  Phrase *sph;
  int i;
  u_char phkey[MAX_PHRASE_LEN+1];
  short mask=0;
  int j,count = 0;

  if (len<1) return 0;

  ahead = (short)key[1];
  ahead |= (key[0] & 0x01) << 8;

  for(i=0;i<len;i++)
    mask += 1<<i;
   
   for( uph = inmd->usrph[ahead]; uph != NULL; uph = uph->next)
   {
      if (uph->len < len) continue;

      if (len > MAX_PHRASE_LEN) {
#ifdef _DEBUG
          fprintf(stderr, "IME (Pinyin): buffer overrun\n");
#endif
//          abort();
          return -1;
      }
      memcpy(phkey,uph->key,len+1);
      phkey[0] &= mask;
      if (!memcmp(phkey,key,len+1))  // match
      {
          if (uph->len == len )   // exact match
          {
              inmd->tempsel[len-1][ inmd->seltotal[len-1]++ ] = 
                   (Phrase*)( ((char*)uph) + 4 );
           }
          else count++;  // calculate the phrase longer than len
      }
   }

    // search in user phrase lib first, then system phrase libray 
     sysph = inmd->sysph[ahead]; 
     p = (char*)sysph->phrase;   // count = total pinyin number
     for(j = 0; j < sysph->count; j++)
     {
        sph = (Phrase *)p;
        if (sph->len >= len)
        {
           if (len > MAX_PHRASE_LEN) {
#ifdef _DEBUG
                fprintf(stderr, "IME (Pinyin): buffer overrun\n");
#endif
//                abort();
                return -1;
           }
           memcpy(phkey,sph->key,len+1);
           phkey[0] &= mask;
           if (!memcmp(phkey,key,len+1)) // match
           {
               if (sph->len == len)
                 inmd->tempsel[len-1][ inmd->seltotal[len-1]++ ] = sph;
               else count++;
           }
        }
        p += SizeOfPhrase(sph->len,sph->count);
  }
   
  return count;
}
 
