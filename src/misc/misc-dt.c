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
 ** misc-dt.c: This file include some miscelleous functions. 
 **
 ** Create date: 1998/12/31
 **
 ** Current maintainer: Wei Yongming.
 */

/**************************CHANGE LOG******************************************
 * 20060124 gaopan:
 *  GAOPAN rewrite SetValueToEtcFile,etc_FileCopy and etc_LocateAndCopy
 *  these change will greatly improve etc system write speed.
 *
 * 20051212:
 *  change fprintf releated and LoadEtcFile's sth here, 
 *  don't flush it before we realize fprintf
 *  this file changed to much , good luck.
 *  we realized fprintf in a wired way ,sa fprintf.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <os_file_api.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "misc.h"

#define etc_LocateSection_2 etc_LocateSection//gaopan 2006124 add

#ifndef __NOUNIX__
#    include <unistd.h>
#    include <time.h>
#    include <pwd.h>
#    include <sys/ioctl.h>
#    include <sys/types.h>
#endif

#ifdef WIN32
#include <sys/stat.h>
#endif

#ifdef _MGHAVE_CLIPBOARD
#include "clipboard.h"
#endif

#ifndef WIN32
#define unlink tp_fdelete
#endif
///////////////////////////////////////
///////////////////////////////////////
#if ((defined(__THREADX__) && defined(__TARGET_VFANVIL__)) ||(defined(__NUCLEUS__) && defined(__TARGET_MONACO__)))
#define fputs my_fputs
#define fgets my_fgets
int my_fputs (const char *s, FILE *stream)
{
     int len = strlen (s);
     int ret = 0;
	
     ret = fwrite (s, 1, len, stream);	
     if (ret != len)
	  return EOF;
     else
	  return ret;
}

char * my_fgets (char *s, int size, FILE *stream)
{
     int c;
     int count = 0;

     while (!feof (stream)) {
	  if (fread (&c, 1, 1, stream) != 1) {
	       if (count == 0) 
		    return NULL;
	       else {
		    s[count] = '\0';
		    return s;
	       }
	  }
		
	  if (c == EOF || (char)c == '\r' ||(char)c == '\n') {
	       s [count++] = (char)c;
	       s [count++] = '\0';
	       return s;
	  }
	  else
	       s[count++] = (char)c;
     }
     if (!count)
	  return NULL;
     else
     {
	  s[count] = '\0';
	  return s;
     }	
}
#endif
///////////////////////////////
/* Handle of MiniGUI etc file object */
GHANDLE hMgEtc = 0;

#ifndef _MGINCORE_RES

char ETCFILEPATH [MAX_PATH + 1];
#define ETCFILENAME "MiniGUI.cfg"

static BOOL LookForEtcFile (void)
{
     char etcfile [MAX_PATH + 1];
     char buff [10];
#ifndef __NOUNIX__
     struct passwd *pwd;

     getcwd (etcfile, MAX_PATH);
     strcat (etcfile, "/");
     strcat (etcfile, ETCFILENAME);
     if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
	  strcpy (ETCFILEPATH, etcfile);
	  return TRUE;
     }

     if ((pwd = getpwuid (geteuid ())) != NULL) {
	  strcpy (etcfile, pwd->pw_dir);
	  if (etcfile [strlen (etcfile) - 1] != '/')
	       strcat (etcfile, "/");
	  strcat (etcfile, ".");
	  strcat (etcfile, ETCFILENAME);

	  if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
	       strcpy (ETCFILEPATH, etcfile);
	       return TRUE;
	  }
     }

     strcpy (etcfile, "/usr/local/etc/" ETCFILENAME);
     if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
	  strcpy (ETCFILEPATH, etcfile);
	  return TRUE;
     }

     strcpy (etcfile, "/etc/" ETCFILENAME);
     if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
	  strcpy (ETCFILEPATH, etcfile);
	  return TRUE;
     }

     return FALSE;

#elif defined(WIN32)
     //FIXME, use a macro as path
     char etcpath[MAX_PATH] = "C:/mydll/fhas2-cfg/";
     strcpy(etcfile, etcpath);
     strcat (etcfile, ETCFILENAME);
#elif defined(__THREADX__)
     char etcpath[MAX_PATH]= "/flash/fhas2-cfg/";
     strcpy(etcfile, etcpath);
     strcat (etcfile, ETCFILENAME);
#endif
    
     if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) 
     {
	  strcpy (ETCFILEPATH, etcfile);
	  return TRUE;
     }
     return FALSE;
}


/* Initialize MiniGUI etc file object, call before accessing MiniGUI etc value */
BOOL mg_InitMgEtc (void)
{
	if (!LookForEtcFile ())
     {
	  fprintf (stderr, "MISC: Can not locate your MiniGUI.cfg file or bad files!\n");
	  return FALSE;
     }
     if (hMgEtc)
	  return TRUE;

     if ( !(hMgEtc = LoadEtcFile (ETCFILEPATH)) )
	  return FALSE;
     return TRUE;
}

/* Terminate MiniGUI etc file object */
void mg_TerminateMgEtc (void)
{
     UnloadEtcFile (hMgEtc);
     hMgEtc = 0;
}

#else

extern ETC_S MGETC;

BOOL mg_InitMgEtc (void)
{
#ifndef DYNAMIC_LOAD
     extern ETC_S MGETC;
     hMgEtc = (GHANDLE) &MGETC;
#else
     GHANDLE set_mgetc (void);//gaopan 20060124 add to avoid warning
		
     hMgEtc = set_mgetc ();
#endif
     return TRUE;
}

void mg_TerminateMgEtc (void) { }

#endif /* _MGINCORE_RES */


BOOL mg_InitMisc (void)
{

#ifdef _MGHAVE_CLIPBOARD
     mg_InitClipBoard ();
#endif

     return mg_InitMgEtc ();
}

void mg_TerminateMisc (void)
{
#ifdef _MGHAVE_CLIPBOARD
     mg_TerminateClipBoard ();
#endif

     mg_TerminateMgEtc ();
}

/****************************** ETC file support ******************************/
/* *****************************************************************************************
 * this function copy LEN bytes data from SF's SFSTART to DF 
 * return value indicate how many bytes copied 
 * if less than zero ,means sth wrong.
 * gaopan 20060124 add
 * *****************************************************************************************/
static int etc_DoCopy(FILE* sf,FILE* df,int sfstart,int len)
{
     char* buf = NULL;
     int ret = ETC_FILEIOFAILED;

     /* do nothing if these happened */
     if(len <= 0)
	  return ETC_OK;
     
     /* seek to sf's sfstart */
     if(tp_fseek(sf,sfstart,SEEK_SET))
	  goto end;

     /* malloc buffer */
     if(!(buf = (char *)malloc(len)))
     {
	  ret = -10;		/* ETC_NOMEM */
	  return ret;
     }

     /* read from sf */
     if(!tp_fread(buf,len,1,sf))
	  goto end;
     
     /* write to df */
     if(!tp_fwrite(buf,len,1,df))
	  goto end;
     
     ret = ETC_OK;
end:
     if(buf)
	  free(buf);
     
     return ret;
}

static char* get_section_name (char *section_line)
{
     char* current;
     char* tail;
     char* name;

     if (!section_line)
	  return NULL;

     current = section_line;

     while (*current == ' ' ||  *current == '\t') current++; 

     if (*current == ';' || *current == '#')
	  return NULL;

     if (*current++ == '[')
	  while (*current == ' ' ||  *current == '\t') current ++;
     else
	  return NULL;

     name = tail = current;
     while (*tail != ']' && *tail != '\n' &&
	    *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;
     *tail = '\0';
     while (*tail == ' ' || *tail == '\t') {
	  *tail = '\0';
	  tail--; 
     }

     return name;
}

static int get_key_value (char *key_line, char **mykey, char **myvalue)
{
     char* current;
     char* tail;
     char* value;

     if (!key_line)
	  return -1;

     current = key_line;

     while (*current == ' ' ||  *current == '\t') current++; 

     if (*current == ';' || *current == '#')
	  return -1;

     if (*current == '[')
	  return 1;

     if (*current == '\n' || *current == '\0')
	  return -1;

     tail = current;
     while (*tail != '=' && *tail != '\n' &&
	    *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;

     value = tail + 1;
     if (*tail != '=')
	  *value = '\0'; 

     *tail-- = '\0';
     while (*tail == ' ' || *tail == '\t') {
	  *tail = '\0';
	  tail--; 
     }
        
     tail = value;
     while (*tail != '\n' && *tail != '\0') tail++;
     *tail = '\0'; 

     if (mykey)
	  *mykey = current;
     if (myvalue)
	  *myvalue = value;

     return 0;
}


/* This function locate the specified section in the etc file.
 * gaopan 20060124 rewrite this function
 * now ,this function do nothing except locate rw pointer
 */
static int etc_LocateSection(FILE* fp, const char* pSection)
{
     char szBuff[ETC_MAXLINE + 1];
     char *name = NULL;

     /* this loop look for section's name and set rw pointer just right after these section*/
     while (TRUE)
     {
	  /* if end of file or any thing wrong */
	  if (!fgets(szBuff, ETC_MAXLINE, fp))
	  {
	       /* this means sth wrong */
	       if (!feof(fp))
		    return ETC_FILEIOFAILED;
	  }
	  /* else we should check if this section is that we are looking for */
	  name = get_section_name (szBuff);
	  if (!name)
	       continue;

	  /* if found speciafied section ,return ,leave rw pointer right after section's line*/
	  if (strcmp (name, pSection) == 0)
	       return ETC_OK;
     }

     return ETC_SECTIONNOTFOUND;
}

/* This function locate the specified section in the etc file. */
/* gaopan 20060124 remark following instead with macro */
/* static int etc_LocateSection_2 (FILE* fp, const char* pSection, FILE* bak_fp) */
/* { */
/*     char szBuff[ETC_MAXLINE + 1]; */
/*     char line[ETC_MAXLINE + 1]; */
/*     char *name; */

/*     while (TRUE) { */
/*         if (!fgets(line, ETC_MAXLINE, fp)) { */
/*             if (feof (fp)) */
/*                 return ETC_SECTIONNOTFOUND; */
/*             else */
/*                 return ETC_FILEIOFAILED; */
/*         } */

/*         strcpy (szBuff, line); */
/*         name = get_section_name (szBuff); */

/*         if (name && strcmp (name, pSection) == 0) */
/*             return ETC_OK;  */
/*         else if (bak_fp && fputs (line, bak_fp) == EOF) */
/*             return ETC_FILEIOFAILED; */
/*     } */

/*     return ETC_SECTIONNOTFOUND; */
/* } */

static int etc_FindNextSection(FILE* fp)
{
     char szBuff[ETC_MAXLINE + 1];
     char *name;

     while (TRUE) {
	  int len;
	  if (!fgets(szBuff, ETC_MAXLINE, fp)) {
	       if (feof (fp))
		    return ETC_OK;
	       else
		    return ETC_FILEIOFAILED;
	  }

	  len = strlen(szBuff);
	  name = get_section_name (szBuff);
	  if (name) {
	       fseek (fp, -len, SEEK_CUR);
	       break;
	  }
     }

     return ETC_OK; 
}

/* This function locate the specified key in the etc file. */
static int etc_LocateKeyValue(FILE* fp, const char* pKey, char* pValue, int iLen)

{
     char szBuff[ETC_MAXLINE + 1 + 1];
     char* current;
     char* value;
     int ret;

     while (TRUE) {
	  int bufflen;

	  if (!fgets(szBuff, ETC_MAXLINE, fp))
	       return ETC_KEYNOTFOUND;
	  bufflen = strlen (szBuff);
	  if (szBuff [bufflen - 1] == '\n')
	       szBuff [bufflen - 1] = '\0';

	  ret = get_key_value (szBuff, &current, &value);
	  if (ret < 0)
	       continue;
	  else if (ret > 0) {
	       fseek (fp, -bufflen, SEEK_CUR);
	       return ETC_KEYNOTFOUND;
	  }
            
	  if (strcmp (current, pKey) == 0) 
	  {
	       if (pValue)
		    strncpy (pValue, value, iLen);
/* **************************************************************
 * MOVE FILE POINTER TO THE BEGIN OF THIS LINE,VERY VERY VERY IMPORTANT FOR OUR GOAL:ONE WRITE,SA SETVALUETOETCFILE,AND FINGURE OUT WHY WE SEEK BAK 
 * ***************************************************************/

	       fseek (fp, -bufflen, SEEK_CUR);
	       return ETC_OK; 
	  }
     }

     return ETC_KEYNOTFOUND;
}


#if defined (WIN32) || (defined (__THREADX__) && defined (__TARGET_VFANVIL__))
static char *memfindstring (const char *buf, int size)
{
     int i;
    
     for (i = 0; i < size; i++) {
	  if (buf [i] == '\0')
	       continue;
	  else 
	       return (char *)(buf + i);
     }
     return NULL;
}
static int etc_ReadSection (const char *filebuf, int size, char **nextpos, PETCSECTION psect)
{
     char szBuff[ETC_MAXLINE + 1 + 1];
     char* sect_name;
     int max_key_nr = 5;
     unsigned char *p1;
     unsigned char *p2;
     static int nCurpos = 0;
    
     psect->name = NULL;
     psect->key_nr = 0;

     if (nextpos == NULL || *nextpos == NULL)
	  return ETC_SECTIONNOTFOUND;

     p1 = (unsigned char*)*nextpos;
     while (TRUE) {
	  int bufflen;

	  p2 = (unsigned char*)memfindstring ((const char*)p1,  size - (p1 - (unsigned char *)filebuf));
	  if (p2 == NULL) {
	       if (psect->name) {
		    *nextpos = NULL;
		    break;
	       }
	       else
		    return ETC_SECTIONNOTFOUND;
	  }

	  strcpy (szBuff, (const char*)p2);
	  bufflen = strlen (szBuff);
	  p1 = p2 + bufflen;

	  if (!psect->name) { /* read section name */
	       sect_name = get_section_name (szBuff);
	       if (!sect_name)
		    continue;

	       psect->name = strdup (sect_name);
	       psect->key_nr = 0;
	       psect->keys = malloc (sizeof(char*)*max_key_nr);
	       psect->values = malloc (sizeof(char*)*max_key_nr);
	  }
	  else { /* read key and value */
	       int ret;
	       char *key, *value;

	       if (psect->key_nr == max_key_nr) {
		    max_key_nr += 5;
		    psect->keys = (char **) realloc (psect->keys, sizeof(char*)*max_key_nr);
		    psect->values = (char **) realloc (psect->values, sizeof(char*)*max_key_nr);
	       }

	       ret = get_key_value (szBuff, &key, &value);
	       if (ret < 0)
		    continue;
	       else if (ret > 0) {  /* another section begins */
		    //fseek (fp, -bufflen, SEEK_CUR);
		    *nextpos = (char*) p2;
		    break;
	       }

	       *(psect->keys + psect->key_nr) = strdup (key);
	       *(psect->values + psect->key_nr) = strdup (value);
	       psect->key_nr ++;
	  }
     }

     return ETC_OK;
}

GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
{
     FILE* fp;
     int max_sect_nr = 15;
     ETC_S *petc;
     char *filebuf = NULL;//gaopan Bug00001206MiniGUI的LoadEtcFile存在隐患,filebuf指针没有初始化,后续的free可能出问题
     char *pnextread;
     int  nReadByte, nFileSize, i;
#ifdef WIN32    
     struct stat filestat;
#endif

     if (!(fp = fopen(pEtcFile, "r")))
	  return 0;
    
     petc = (ETC_S*) malloc (sizeof(ETC_S));
     petc->section_nr = 0;

     /* we allocate 15 sections first */
     petc->sections = (PETCSECTION) malloc (sizeof(ETCSECTION)*max_sect_nr);

#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
     nFileSize = tp_flength(fp);
#else
     fstat (fileno(fp), &filestat);
     nFileSize = filestat.st_size;
#endif

     if (nFileSize <= 0) {
	  goto endflag;
     }

     filebuf = (char *)CALLOC (1, nFileSize + 1);
     if (filebuf == NULL)
	  goto endflag;

     nReadByte = fread (filebuf, 1, nFileSize, fp);
/*
  if (nReadByte != nFileSize) {
  goto endflag;
  }
*/
     filebuf [nFileSize] = '\0';
     for (i = 0; i< nFileSize; i++) {
	  //if (filebuf [i] == '\n' || filebuf [i] == '\r')
	  if (filebuf [i] == 0x0D || filebuf [i] == 0x0A)
	       filebuf [i] = '\0';
     }
    
     pnextread = filebuf;
     while (etc_ReadSection (filebuf, nFileSize + 1, &pnextread, petc->sections + petc->section_nr) == ETC_OK) {
	  petc->section_nr ++;
	  if (petc->section_nr == max_sect_nr) {
	       /* add 5 sections each time we realloc */
	       max_sect_nr += 5;
	       petc->sections = realloc (petc->sections, sizeof(ETCSECTION)*max_sect_nr);
	  }
     }

endflag:
     fclose (fp);
     free (filebuf);
     return (GHANDLE)petc;
}
#else

static int etc_ReadSection (FILE* fp, PETCSECTION psect)
{
     char szBuff[ETC_MAXLINE + 1 + 1];
     char* sect_name;
     int max_key_nr = 5;

     psect->name = NULL;
     psect->key_nr = 0;

     while (TRUE) {
	  int bufflen;

	  if (!fgets(szBuff, ETC_MAXLINE, fp)) {
	       if (feof (fp)) {
		    if (psect->name)
			 break;
		    else
			 return ETC_SECTIONNOTFOUND;
	       }
	       else
		    return ETC_FILEIOFAILED;
	  }

	  bufflen = strlen (szBuff);
	  if (szBuff [bufflen - 1] == '\n')
	       szBuff [bufflen - 1] = '\0';

	  if (!psect->name) { /* read section name */
	       sect_name = get_section_name (szBuff);
	       if (!sect_name)
		    continue;

	       psect->name = strdup (sect_name);
	       psect->key_nr = 0;
	       psect->keys = (char **)malloc (sizeof(char*)*max_key_nr);
	       psect->values = (char **)malloc (sizeof(char*)*max_key_nr);
	  }
	  else { /* read key and value */
	       int ret;
	       char *key, *value;

	       if (psect->key_nr == max_key_nr) {
		    max_key_nr += 5;
		    psect->keys = (char **) realloc (psect->keys, sizeof(char*)*max_key_nr);
		    psect->values = (char **) realloc (psect->values, sizeof(char*)*max_key_nr);
	       }

	       ret = get_key_value (szBuff, &key, &value);
	       if (ret < 0)
		    continue;
	       else if (ret > 0) {  /* another section begins */
		    fseek (fp, -bufflen, SEEK_CUR);
		    break;
	       }

	       *(psect->keys + psect->key_nr) = strdup (key);
	       *(psect->values + psect->key_nr) = strdup (value);
	       psect->key_nr ++;
	  }
     }

     return ETC_OK;
}

GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
{
     FILE* fp;
     int max_sect_nr = 15;
     ETC_S *petc;

     if (!(fp = fopen(pEtcFile, "r")))
	  return 0;

     petc = (ETC_S*) malloc (sizeof(ETC_S));
     petc->section_nr = 0;
     /* we allocate 15 sections first */
     petc->sections = (PETCSECTION) malloc (sizeof(ETCSECTION)*max_sect_nr);

     while (etc_ReadSection (fp, petc->sections + petc->section_nr) == ETC_OK) {
	  petc->section_nr ++;
	  if (petc->section_nr == max_sect_nr) {
	       /* add 5 sections each time we realloc */
	       max_sect_nr += 5;
	       petc->sections = realloc (petc->sections, sizeof(ETCSECTION)*max_sect_nr);
	  }
     }

     fclose (fp);
     return (GHANDLE)petc;
}
#endif

static int etc_WriteSection (FILE* fp, PETCSECTION psect, BOOL bSectName)
{
     int i;

     if (bSectName)
	  fprintf (fp, "[%s]\n", psect->name);

     for (i=0; i<psect->key_nr;i++) {
	  fprintf (fp, "%s=%s\n", psect->keys[i], psect->values[i]);
     }

     fprintf (fp, "\n");

     return ETC_OK;
}

/* *********************************************************************************
 * gaopan 20060124 rewrite this function
 * now, this function directly copy data from sf to df from where sf's file r/w 
 * pointer to end of file
 * ******************************************************************************* */
static int etc_FileCopy (FILE* sf, FILE* df)
{
     int start = 0,sflen = 0;
    
     /* see where copy should start */
     if(-1 == (start = tp_ftell(sf)))
	  return ETC_FILEIOFAILED;
     
     /* see file length,sflen - start result how long should be copied */
     if(-1 == (sflen = tp_flength(sf)))
	  return ETC_FILEIOFAILED;

     /* resort etc_DoCopy To Copy Data */
     if(df)
	  return etc_DoCopy(sf,df,start,sflen - start);
     return ETC_OK;
}

//int ReviseSectionInEtcFile (const char* pEtcFile, PETCSECTION psect, 
//                            BOOL bRemove)
//{
//    FILE* etc_fp;
//    FILE* tmp_fp;
//    int rc;
//
//#ifndef HAVE_TMPFILE
//    char tmp_nam [256];
//
//    sprintf (tmp_nam, "/tmp/mg-etc-tmp-%lx", time(NULL));
//    if ((tmp_fp = fopen (tmp_nam, "w+")) == NULL)
//        return ETC_TMPFILEFAILED;
//#else
//    if ((tmp_fp = tmpfile ()) == NULL)
//        return ETC_TMPFILEFAILED;
//#endif
//
//    /* open or create a etc file */
//    if (!(etc_fp = fopen (pEtcFile, "r+"))) {
//        fclose (tmp_fp);
//#ifndef HAVE_TMPFILE
// unlink (tmp_nam);
//#endif
// if (!(etc_fp = fopen (pEtcFile, "w"))) {
// return ETC_FILEIOFAILED;
// }
// fprintf (etc_fp, "[%s]\n", psect->name);
////TODO
// fclose (etc_fp);
// return ETC_OK;
// }
//
// rc = etc_LocateSection_2 (etc_fp, psect->name);
//
// if (rc == ETC_SECTIONNOTFOUND) {
// fclose (tmp_fp);
//#ifndef HAVE_TMPFILE
// unlink (tmp_nam);
//#endif
// if (!bRemove) {
// etc_WriteSection (etc_fp, psect, TRUE);
// fclose (etc_fp);
// return ETC_OK;
// }
// else
// return ETC_SECTIONNOTFOUND;
// }
// else if (rc == ETC_FILEIOFAILED) {
// fclose (tmp_fp);
//#ifndef HAVE_TMPFILE
// unlink (tmp_nam);
//#endif
// fclose (etc_fp);
// return ETC_FILEIOFAILED;
// }
//
// if(!bRemove) {
// etc_WriteSection (tmp_fp, psect, TRUE);
// }
//
// etc_FindNextSection (etc_fp);
//
// if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
// goto error;
// 
// // replace etc content with tmp file content
// // truncate etc content first
// fclose (etc_fp);
//
// if (!(etc_fp = fopen (pEtcFile, "w"))) {
// fclose (tmp_fp);
//#ifndef HAVE_TMPFILE
// unlink (tmp_nam);
//#endif
// return ETC_FILEIOFAILED;
// }
// 
// rewind (tmp_fp);
// rc = etc_FileCopy (tmp_fp, etc_fp);
//
//error:
// fclose (etc_fp);
// fclose (tmp_fp);
//#ifndef HAVE_TMPFILE
// unlink (tmp_nam);
//#endif
// return rc;
//}

//int GUIAPI SaveSectionToEtcFile (const char* pEtcFile, PETCSECTION psect)
//{
//    return ReviseSectionInEtcFile (pEtcFile, psect, FALSE);
//}

//int GUIAPI RemoveSectionInEtcFile (const char* pEtcFile, const char *pSection)
//{
//    ETCSECTION sect;
//
//    sect.name = (char *)pSection;
//    sect.key_nr = 0;
//
//    return ReviseSectionInEtcFile (pEtcFile, &sect, TRUE);
//}

int GUIAPI UnloadEtcFile (GHANDLE hEtc)
{
     int i;
     ETC_S *petc = (ETC_S*) hEtc;

     if (!petc)
	  return -1;

     for (i=0; i<petc->section_nr; i++) {
	  PETCSECTION psect = petc->sections + i;
	  int j;

	  if (!psect)
	       continue;

	  for (j=0; j<psect->key_nr; j++) {
	       free (psect->keys[j]);
	       free (psect->values[j]);
	  }
	  free (psect->keys);
	  free (psect->values);
	  free (psect->name);
     }

     free (petc->sections);
     free (petc);

     return 0;
}

int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection,
                            const char* pKey, char* pValue, int iLen)
{
     int i;
     ETC_S *petc = (ETC_S*) hEtc;
     PETCSECTION psect = NULL;

     if (!petc || !pValue)
	  return -1;

     for (i=0; i<petc->section_nr; i++) {
	  psect = petc->sections + i;
	  if (!psect)
	       continue;

	  if (strcmp (psect->name, pSection) == 0) {
	       break;
	  }
     }

     if (i >= petc->section_nr)
	  return ETC_SECTIONNOTFOUND;

     for (i=0; i<psect->key_nr; i++) {
	  if (strcmp (psect->keys[i], pKey) == 0) {
	       break;
	  }
     }

     if (i >= psect->key_nr)
	  return ETC_KEYNOTFOUND;

     if (iLen > 0) { /* get value */
	  strncpy (pValue, psect->values[i], iLen);
     }
     else { /* set value */
	  free (psect->values[i]);
	  psect->values[i] = strdup(pValue);
     }

     return ETC_OK;
}

int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection,
                               const char* pKey, int* value)
{
     int ret;
     char szBuff [51];

     ret = GetValueFromEtc (hEtc, pSection, pKey, szBuff, 50);
     if (ret < 0) {
	  return ret;
     }

     *value = strtol (szBuff, NULL, 0);
     if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
	  return ETC_INTCONV;

     return ETC_OK;
}

/* Function: GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue, int iLen);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   The buffer will store the value of the key.
 *     iLen:     The max length of value string.
 * Return:
 *     int               meaning
 *     ETC_FILENOTFOUND           The etc file not found. 
 *     ETC_SECTIONNOTFOUND        The section is not found. 
 *     ETC_EKYNOTFOUND        The Key is not found.
 *     ETC_OK            OK.
 */
int GUIAPI GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue, int iLen)
{
     FILE* fp;

     if (!(fp = fopen(pEtcFile, "r")))
	  return ETC_FILENOTFOUND;

     if (pSection)
	  if (etc_LocateSection (fp, pSection) != ETC_OK) {
	       fclose (fp);
	       return ETC_SECTIONNOTFOUND;
	  }

     if (etc_LocateKeyValue (fp, pKey, pValue, iLen) != ETC_OK) 
     {
	  fclose (fp);
	  return ETC_KEYNOTFOUND;
     }

     fclose (fp);
     return ETC_OK;
}

/* Function: GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 * Return:
 *     int                      meaning
 *     ETC_FILENOTFOUND             The etc file not found. 
 *     ETC_SECTIONNOTFOUND          The section is not found. 
 *     ETC_EKYNOTFOUND              The Key is not found.
 *     ETC_OK                       OK.
 */
int GUIAPI GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
				  const char* pKey, int* value)
{
     int ret;
     char szBuff [51];

     ret = GetValueFromEtcFile (pEtcFile, pSection, pKey, szBuff, 50);
     if (ret < 0)
	  return ret;

     *value = strtol (szBuff, NULL, 0);
     if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
	  return ETC_INTCONV;

     return ETC_OK;
}

/* **********************************************************************************
 * gaopan 20060124 changed sth here.
 * after this function ,file rw pointer will point to right place,which means:
 * if the key found,the pointer will point to key's start
 * if the key not found,the pionter will point to the end of all keys of this sections 
 * this behavior make caller know where to split old config file
 * **********************************************************************************/
static int etc_CopyAndLocate (FILE* etc_fp,const char* pSection, const char* pKey)
{
     if (pSection && etc_LocateSection (etc_fp, pSection) != ETC_OK)
	  return ETC_SECTIONNOTFOUND;

     if (etc_LocateKeyValue (etc_fp, pKey, NULL, 0) != ETC_OK)
	  return ETC_KEYNOTFOUND;

     return ETC_OK;
}

/* Function: SetValueToEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   Value.
 * Return:
 *     int                      meaning
 *     ETC_FILENOTFOUND         The etc file not found.
 *     ETC_TMPFILEFAILED        Create tmp file failure.
 *     ETC_OK                   OK.
 * gaopan 20060124 rewrite this function, now,we just write once.
 */
int GUIAPI SetValueToEtcFile (const char* pEtcFile, const char* pSection,
			      const char* pKey, char* pValue)
{
     FILE* pf;
     int ret = ETC_FILEIOFAILED;
     int writepoint = -1;	/* where new section-key-value should be insert */
     int backuppoint = -1;	/* from where we should backup to buffer and save to file later */
     int backuplen = -1;	/* how much bytes need to save to buffer */
     char* backupbuf = NULL;
     int buflen;		/* this buflen is new key-value's len plus backuplen */
     char newline = 0;
	
     if(tp_fexist((char*)pEtcFile)) /* 0 means exist */
     {
	  if (!(pf = tp_fopen (pEtcFile, "w")))
	       return ETC_FILEIOFAILED;
		
	  fprintf (pf, "[%s]\n%s=%s\n", pSection,pKey,pValue);
	  ret = ETC_OK;
	  goto end;
     }
	
     if (!(pf = tp_fopen (pEtcFile, "r+")))
	  return ETC_FILEIOFAILED;
	
     switch (ret = etc_CopyAndLocate (pf, pSection, pKey)) 
     {
     case ETC_SECTIONNOTFOUND:
	  /* write section and key and value ,nothing need to do */
	  if(tp_fseek(pf,0,SEEK_END))
	       goto end;
	  /* prefix a '\n' help us out of warry about new line*/
	  fprintf(pf,"\n[%s]\n%s=%s",pSection,pKey,pValue);
	  goto end;
     case ETC_OK:
     {
	  /* count writepoint and backuppoint */
	  char buf[ETC_MAXLINE+1];
	  if(-1 == (writepoint = tp_ftell(pf))
	     || !fgets(buf, ETC_MAXLINE, pf)	  /* GETS USED TO SKIP CURRENT SECTION,BECAUSE IT SHOULD BE REWRITE!!!! */
	     || -1 == (backuppoint = tp_ftell(pf)))
	       goto end;
	  break;
     }
     case ETC_KEYNOTFOUND:
     {
	  /* count writepoint and backuppoint */
	  if(-1 == (writepoint = tp_ftell(pf)))
	       goto end;
	  backuppoint = writepoint;
	  break;
     }
     default:
	  goto end;
     }

     /* read backup data to buffer */
     if(-1 == (backuplen = tp_flength(pf)))
	  goto end;
     backuplen -= backuppoint;
     buflen = strlen(pKey) + strlen(pValue) + 10 + backuplen;/* in fact ,extra 3 is enough,one '=' and two '\n' char  */

     /* malloc buffer to write */
     if(!(backupbuf = (char *)malloc(buflen)))
     {
	  ret = -10;		/* ETC_NOMEM */
	  goto end;
     }
     memset(backupbuf,0,buflen);

     //check if need write extra '\n' before new key-value
     if(tp_fseek(pf,-1,SEEK_CUR) || !tp_fread(&newline,1,1,pf))
	  goto end;

     /* format new key-value ,if need ,we insert a '\n' at first*/
     if(newline == '\n')
	  sprintf(backupbuf,"%s=%s\n",pKey,pValue);
     else
	  sprintf(backupbuf,"\n%s=%s\n",pKey,pValue);

     /* read from file to buf,right after new key-value */
     if(backuplen && !tp_fread(backupbuf + strlen(backupbuf),backuplen,1,pf))
	  goto end;

     /* save backup to file */
     if(tp_fseek(pf,writepoint,SEEK_SET))
	  goto end;

     if(!tp_fwrite(backupbuf,strlen(backupbuf),1,pf))
	  goto end;
     /* it seems everything right */
     ret = ETC_OK;
end:
     if(backupbuf)
	  free(backupbuf);
     fclose (pf);
     return ret;
}

/****************************** Ping and Beep *********************************/
#ifdef WIN32
void mswin_ping(void);
#endif

void GUIAPI Ping(void)
{
#ifdef WIN32
	mswin_ping();
#else
    putchar ('\a');
    fflush (stdout);
#endif
}

#if !defined (__NOUNIX__) && !defined(__CYGWIN__) && defined(i386)

#include <linux/kd.h>
#include <asm/param.h>

void GUIAPI Tone (int frequency_hz, int duration_ms)
{
     /* FIXME: Tone will not work in X Window */
     long argument = (1190000 / frequency_hz) | ((duration_ms / (1000/HZ)) << 16);

     ioctl (0, KDMKTONE, (long) argument);
}
#endif


char* strnchr (const char* s, size_t n, int c)
{
     size_t i;
    
     for (i=0; i<n; i++) {
	  if ( *s == c)
	       return (char *)s;

	  s ++;
     }

     return NULL;
}

int substrlen (const char* text, int len, int delimiter, int* nr_delim)
{
     char* substr;

     *nr_delim = 0;

     if ( (substr = strnchr (text, len, delimiter)) == NULL)
	  return len;

     len = substr - text;

     while (*substr == delimiter) {
	  (*nr_delim) ++;
	  substr ++;
     }

     return len;
}

char* strtrimall (char *src)
{
     int  nIndex1;
     int  nLen;

     if (src == NULL)
	  return NULL;

     if (src [0] == '\0')
	  return src;

     nLen = strlen (src);

     nIndex1 = 0;
     while (isspace ((int)src[nIndex1]))
	  nIndex1 ++;

     if (nIndex1 == nLen) {
	  *src = '\0';
	  return src;
     }

     strcpy (src, src + nIndex1);

     nLen = strlen (src);
     nIndex1 = nLen - 1;
     while (isspace ((int)src[nIndex1]))
	  nIndex1 --;

     src [nIndex1 + 1] = '\0';  

     return src;
}


