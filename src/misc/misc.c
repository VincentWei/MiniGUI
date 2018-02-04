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
** misc.c: This file include some miscelleous functions. 
**
** Create date: 1998/12/31
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "devfont.h"
#include "window.h"
#include "misc.h"

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

FILE* __mg_tmpfile (void);
int __mg_close_tmpfile (FILE *tmp_fp);
void __mg_rewind (FILE *fp);

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

static BOOL LookForEtcFile (void)
{
    char etcfile [MAX_PATH + 1];
    char buff [10];
    char* etc_value = NULL;

    struct passwd *pwd;

    if ((etc_value = getenv ("MG_CFG_PATH"))) {
        int len = strlen(etc_value);
        if (etc_value[len-1] == '/') 
            sprintf(etcfile, "%s%s", etc_value, ETCFILENAME);
        else 
            sprintf(etcfile, "%s/%s", etc_value, ETCFILENAME);

        if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                ==  ETC_OK) {
            strcpy (ETCFILEPATH, etcfile);
            return TRUE;
        }
    }
 

    if (getcwd (etcfile, MAX_PATH) == NULL) {
        return FALSE;
    }

    strcat (etcfile, "/");
    strcat (etcfile, ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                    ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

#ifndef __NOUNIX__
    if ((pwd = getpwuid (geteuid ())) != NULL) {
        strcpy (etcfile, pwd->pw_dir);
        if (etcfile [strlen (etcfile) - 1] != '/')
            strcat (etcfile, "/");
        strcat (etcfile, ".");
        strcat (etcfile, ETCFILENAME);

        if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                        ==  ETC_OK) {
            strcpy (ETCFILEPATH, etcfile);
            return TRUE;
        }
    }

    strcpy (etcfile, "/usr/local/etc/" ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
            ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    strcpy (etcfile, "/etc/" ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
            ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    return FALSE;

#elif defined(WIN32)
    sprintf(etcfile, "c:\\windows\\%s", ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                    ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    sprintf(etcfile, "c:\\%s", ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                    ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }
    return FALSE;
#elif defined(__THREADX__)
    char etcpath[MAX_PATH]= "/flash/fhas2-cfg/";
    strcpy (etcfile, etcpath);
    strcat (etcfile, ETCFILENAME);
#endif
    
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) 
                    ==  ETC_OK) 
    {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }
    return FALSE;
}


/* Initialize MiniGUI etc file object, call before accessing MiniGUI etc value */
BOOL mg_InitMgEtc (void)
{
    if (!LookForEtcFile ()) {
        fprintf (stderr, "MISC: Can not locate your MiniGUI.cfg file "
                        "or bad files!\n");
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

extern GHANDLE __mg_get_mgetc (void);

BOOL mg_InitMgEtc (void)
{
    hMgEtc = __mg_get_mgetc ();
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

static char* get_section_name (char *section_line)
{
    char* current;
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

    name = current;
    while (*current != ']' && *current != '\n' &&
          *current != ';' && *current != '#' && *current != '\0')
          current++;
    *current = '\0';
    while (*current == ' ' || *current == '\t') {
        *current = '\0';
        current--; 
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


/* This function locate the specified section in the etc file. */
static int etc_LocateSection(FILE* fp, const char* pSection, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1];
    char *name;

    while (TRUE) {
        if (!fgets(szBuff, ETC_MAXLINE, fp)) {
            if (feof (fp))
                return ETC_SECTIONNOTFOUND;
            else
                return ETC_FILEIOFAILED;
        }
        else if (bak_fp && fputs (szBuff, bak_fp) == EOF)
            return ETC_FILEIOFAILED;
        
        name = get_section_name (szBuff);
        if (!name)
            continue;

        if (strcmp (name, pSection) == 0)
            return ETC_OK; 
    }

    return ETC_SECTIONNOTFOUND;
}

/* This function locate the specified key in the etc file. */
static int etc_LocateKeyValue(FILE* fp, const char* pKey, 
                               BOOL bCurSection, char* pValue, int iLen,
                               FILE* bak_fp, char* nextSection)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* current;
    char* value;
    int ret;

    while (TRUE) {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp))
            return ETC_FILEIOFAILED;
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
            
        if (strcmp (current, pKey) == 0) {
            if (pValue)
                strncpy (pValue, value, iLen);

            return ETC_OK; 
        }
        else if (bak_fp && *current != '\0') {
#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
//FIXME, unknown problem with fprintf
            char tmp_nam[256];
            sprintf (tmp_nam , "%s=%s\n", pKey, pValue);
            fputs (tmp_nam, bak_fp);
#else
            fprintf (bak_fp, "%s=%s\n", current, value);
#endif
        }
    }

    return ETC_KEYNOTFOUND;
}

static PETCSECTION etc_NewSection (ETC_S* petc)
{
    PETCSECTION psect;

    if (petc->section_nr == petc->sect_nr_alloc) {
        /* add 5 sections each time we realloc */
        petc->sect_nr_alloc += NR_SECTS_INC_ALLOC;
        petc->sections = realloc (petc->sections, 
                    sizeof (ETCSECTION)*petc->sect_nr_alloc);
    }
    psect = petc->sections + petc->section_nr;

    psect->name = NULL;

    petc->section_nr ++;

    return psect;
}

static void etc_NewKeyValue (PETCSECTION psect, 
                const char* key, const char* value)
{
    if (psect->key_nr == psect->key_nr_alloc) {
        psect->key_nr_alloc += NR_KEYS_INC_ALLOC;
        psect->keys = realloc (psect->keys, 
                            sizeof (char*) * psect->key_nr_alloc);
        psect->values = realloc (psect->values, 
                            sizeof (char*) * psect->key_nr_alloc);
    }

    psect->keys [psect->key_nr] = FixStrDup (key);
    psect->values [psect->key_nr] = FixStrDup (value);
    psect->key_nr ++;
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

static int etc_ReadSection (const char *filebuf, int size, 
                char **nextpos, PETCSECTION psect)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* sect_name;
    unsigned char *p1;
    unsigned char *p2;
    
    psect->name = NULL;
    psect->key_nr = 0;
    psect->keys = NULL;
    psect->values = NULL;

    if (nextpos == NULL || *nextpos == NULL)
        return ETC_SECTIONNOTFOUND;

    p1 = (unsigned char*)*nextpos;
    while (TRUE) {
        int bufflen;

        p2 = (unsigned char*)memfindstring ((const char*)p1,  
                        size - (p1 - (unsigned char *)filebuf));
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

            psect->name = FixStrDup (sect_name);
            psect->key_nr = 0;
            psect->key_nr_alloc = NR_KEYS_INIT_ALLOC;
            psect->keys = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
            psect->values = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
        }
        else { /* read key and value */
            int ret;
            char *key, *value;

            ret = get_key_value (szBuff, &key, &value);
            if (ret < 0)
                continue;
            else if (ret > 0) {  /* another section begins */
                //fseek (fp, -bufflen, SEEK_CUR);
                *nextpos = (char*) p2;
                break;
            }

            etc_NewKeyValue (psect, key, value);
        }
    }

    return ETC_OK;
}

GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
{
    FILE* fp = NULL;
    ETC_S *petc;
    char *filebuf;
    char *pnextread;
    int  nReadByte, nFileSize, i;
#ifdef WIN32    
    struct stat filestat;
#endif

    if (pEtcFile && !(fp = fopen (pEtcFile, "r")))
         return 0;
    
    petc = (ETC_S*) malloc (sizeof(ETC_S));
    
    /* we allocate 15 sections first */
    petc->sections = 
            (PETCSECTION) malloc (sizeof(ETCSECTION)* NR_SECTS_INIT_ALLOC);

    petc->section_nr = 0;
    petc->sect_nr_alloc = NR_SECTS_INIT_ALLOC;

    if (pEtcFile == NULL) { /* return an empty etc object */
        return (GHANDLE) petc;
    }

#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
    nFileSize = tp_flength(fp);
#else
    fstat (fileno(fp), &filestat);
    nFileSize = filestat.st_size;
#endif

    if (nFileSize <= 0) {
        goto endflag;
    }

    filebuf = (char *)calloc (1, nFileSize + 1);
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
    while (etc_ReadSection (filebuf, nFileSize + 1, &pnextread, 
                            petc->sections + petc->section_nr) == ETC_OK) {
        petc->section_nr ++;
        if (petc->section_nr == petc->sect_nr_alloc) {
            /* add 5 sections each time we realloc */
            petc->sect_nr_alloc += NR_SECTS_INC_ALLOC;
            petc->sections = realloc (petc->sections, 
                            sizeof (ETCSECTION)*petc->sect_nr_alloc);
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

    psect->name = NULL;
    psect->key_nr = 0;
    psect->keys = NULL;
    psect->values = NULL;

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

            psect->name = FixStrDup (sect_name);
            psect->key_nr = 0;
            psect->key_nr_alloc = NR_KEYS_INIT_ALLOC;
            psect->keys = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
            psect->values = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
        }
        else { /* read key and value */
            int ret;
            char *key, *value;

            ret = get_key_value (szBuff, &key, &value);
            if (ret < 0)
                continue;
            else if (ret > 0) {  /* another section begins */
                fseek (fp, -bufflen, SEEK_CUR);
                break;
            }

            etc_NewKeyValue (psect, key, value);
        }
    }

    return ETC_OK;
}

GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
{
    FILE* fp = NULL;
    ETC_S *petc;

    if (pEtcFile && !(fp = fopen (pEtcFile, "r")))
         return 0;

    petc = (ETC_S*) malloc (sizeof(ETC_S));
    petc->section_nr = 0;
    petc->sect_nr_alloc = NR_SECTS_INIT_ALLOC;

    /* we allocate 15 sections first */
    petc->sections = 
            (PETCSECTION) malloc (sizeof(ETCSECTION)*NR_SECTS_INIT_ALLOC);

    if (pEtcFile == NULL) { /* return an empty etc object */
        return (GHANDLE) petc;
    }

    while (etc_ReadSection (fp, petc->sections + petc->section_nr) == ETC_OK) {
        petc->section_nr ++;
        if (petc->section_nr == petc->sect_nr_alloc) {
            /* add 5 sections each time we realloc */
            petc->sect_nr_alloc += NR_SECTS_INC_ALLOC;
            petc->sections = realloc (petc->sections, 
                            sizeof(ETCSECTION)*petc->sect_nr_alloc);
        }
    }

    fclose (fp);
    return (GHANDLE)petc;
}

#endif

int GUIAPI UnloadEtcFile (GHANDLE hEtc)
{
    int i;
    ETC_S *petc = (ETC_S*) hEtc;

    if (!petc)
        return -1;

    for (i=0; i<petc->section_nr; i++) {
        PETCSECTION psect = petc->sections + i;
        int j;

        if (!psect->name)
            continue;

        for (j=0; j<psect->key_nr; j++) {
            FreeFixStr (psect->keys [j]);
            FreeFixStr (psect->values [j]);
        }
        free (psect->keys);
        free (psect->values);
        FreeFixStr (psect->name);
    }

    free (petc->sections);
    free (petc);

    return 0;
}

static int etc_GetSectionValue (PETCSECTION psect, const char* pKey, 
                          char* pValue, int iLen)
{
    int i;

    for (i=0; i<psect->key_nr; i++) {
        if (strcmp (psect->keys [i], pKey) == 0) {
            break;
        }
    }

    if (iLen > 0) { /* get value */
        if (i >= psect->key_nr)
            return ETC_KEYNOTFOUND;

        strncpy (pValue, psect->values [i], iLen);
    }
    else { /* set value */
        if (psect->key_nr_alloc <= 0)
            return ETC_READONLYOBJ;

        if (i >= psect->key_nr) {
            etc_NewKeyValue (psect, pKey, pValue);
        }
        else {
            FreeFixStr (psect->values [i]);
            psect->values [i] = FixStrDup (pValue);
        }
    }

    return ETC_OK;
}

int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection,
                            const char* pKey, char* pValue, int iLen)
{
    int i, empty_section = -1;
    ETC_S *petc = (ETC_S*) hEtc;
    PETCSECTION psect = NULL;

    if (!petc || !pValue)
        return -1;

    for (i=0; i<petc->section_nr; i++) {
        psect = petc->sections + i;
        if (!psect->name) {
           empty_section = i;
           continue;
        }

        if (strcmp (psect->name, pSection) == 0) {
            break;
        }
    }

    if (i >= petc->section_nr) {
        if (iLen > 0)
            return ETC_SECTIONNOTFOUND;
        else {
            if (petc->sect_nr_alloc <= 0)
                return ETC_READONLYOBJ;

            if (empty_section >= 0)
                psect = petc->sections + empty_section;
            else {
                psect = etc_NewSection (petc);
            }

            if (psect->name == NULL) {
                psect->key_nr = 0;
                psect->name = FixStrDup (pSection);
                psect->key_nr_alloc = NR_KEYS_INIT_ALLOC;
                psect->keys = malloc (sizeof (char* ) * NR_KEYS_INIT_ALLOC);
                psect->values = malloc (sizeof (char* ) * NR_KEYS_INIT_ALLOC);
            }
        }
    }

    return etc_GetSectionValue (psect, pKey, pValue, iLen);
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
    char tempSection [ETC_MAXLINE + 2];

    if (!(fp = fopen(pEtcFile, "r")))
         return ETC_FILENOTFOUND;

    if (pSection)
         if (etc_LocateSection (fp, pSection, NULL) != ETC_OK) {
             fclose (fp);
             return ETC_SECTIONNOTFOUND;
         }

    if (etc_LocateKeyValue (fp, pKey, pSection != NULL, 
                pValue, iLen, NULL, tempSection) != ETC_OK) {
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
int GUIAPI GetIntValueFromEtcFile (const char* pEtcFile, const char* pSection,
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

static int etc_CopyAndLocate (FILE* etc_fp, FILE* tmp_fp, 
                const char* pSection, const char* pKey, char* tempSection)
{
    if (pSection && etc_LocateSection (etc_fp, pSection, tmp_fp) != ETC_OK)
        return ETC_SECTIONNOTFOUND;

    if (etc_LocateKeyValue (etc_fp, pKey, pSection != NULL, 
                NULL, 0, tmp_fp, tempSection) != ETC_OK)
        return ETC_KEYNOTFOUND;

    return ETC_OK;
}

static int etc_FileCopy (FILE* sf, FILE* df)
{
    char line [ETC_MAXLINE + 1];
    
    while (fgets (line, ETC_MAXLINE + 1, sf) != NULL)
        if (fputs (line, df) == EOF) {
            return ETC_FILEIOFAILED;
        }

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
 */
int GUIAPI SetValueToEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;
    char tempSection [ETC_MAXLINE + 2] = {0};

    if ((tmp_fp = __mg_tmpfile ()) == NULL)
        return ETC_TMPFILEFAILED;

    if (!(etc_fp = fopen (pEtcFile, "r+"))) {
        __mg_close_tmpfile (tmp_fp);

        if (!(etc_fp = fopen (pEtcFile, "w"))) {
            return ETC_FILEIOFAILED;
        }
        fprintf (etc_fp, "[%s]\n", pSection);
        fprintf (etc_fp, "%s=%s\n", pKey, pValue);
        fclose (etc_fp);
        return ETC_OK;
    }

    switch (etc_CopyAndLocate (etc_fp, tmp_fp, pSection, pKey, tempSection)) {
    case ETC_SECTIONNOTFOUND:
        fprintf (tmp_fp, "\n[%s]\n", pSection);
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;
    
    case ETC_KEYNOTFOUND:
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        fprintf (tmp_fp, "%s\n", tempSection);
    break;

    default:
#if defined(__THREADX__) && defined(__TARGET_VFANVIL__)
/* FIXME, unknown problem with fprintf */
    {
        char tmp_nam[256];
        sprintf (tmp_nam , "%s=%s\n", pKey, pValue);
        fputs (tmp_nam, tmp_fp);
    }
#else
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
#endif
        break;
    }

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
        goto error;
    
    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);
    if (!(etc_fp = fopen (pEtcFile, "w"))) {
        __mg_close_tmpfile (tmp_fp);
        return ETC_FILEIOFAILED;
    }
    
    __mg_rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    __mg_close_tmpfile (tmp_fp);
    return rc;
}

static int etc_WriteSection (FILE* fp, PETCSECTION psect, BOOL bSectName)
{
    int i;

    if (psect->name == NULL)
        return ETC_OK;

    if (bSectName)
        fprintf (fp, "[%s]\n", psect->name);

    for (i = 0; i < psect->key_nr; i++) {
        fprintf (fp, "%s=%s\n", psect->keys [i], psect->values [i]);
    }

    if (fprintf (fp, "\n") != 1)
        return ETC_FILEIOFAILED;

    return ETC_OK;
}

int GUIAPI SaveEtcToFile (GHANDLE hEtc, const char* file_name)
{
    int i;
    FILE* fp;
    ETC_S *petc = (ETC_S*) hEtc;

    if (petc == NULL)
        return ETC_INVALIDOBJ;

    if (!(fp = fopen (file_name, "w"))) {
        return ETC_FILEIOFAILED;
    }

    for (i = 0; i < petc->section_nr; i++) {
        if (etc_WriteSection (fp, petc->sections + i, TRUE)) {
            fclose (fp);
            return ETC_FILEIOFAILED;
        }
    }

    fclose (fp);
    return ETC_OK;
}

GHANDLE GUIAPI FindSectionInEtc (GHANDLE hEtc, 
                const char* pSection, BOOL bCreateNew)
{
    int i, empty_section = -1;
    ETC_S *petc = (ETC_S*) hEtc;
    ETCSECTION* psect = NULL;

    if (petc == NULL || pSection == NULL)
        return 0;

    for (i = 0; i < petc->section_nr; i++) {
        if (petc->sections [i].name == NULL) {
            empty_section = i;
        }
        else if (strcmp (petc->sections [i].name, pSection) == 0)
            return (GHANDLE) (petc->sections + i);
    }

    /* not found */
    if (bCreateNew) {
        if (petc->sect_nr_alloc <= 0)
            return (GHANDLE) ETC_READONLYOBJ;

        if (empty_section >= 0) {
            psect = petc->sections + empty_section;
        }
        else {
            psect = etc_NewSection (petc);
        }
        
        if (psect->name == NULL) {
            psect->key_nr = 0;
            psect->name = FixStrDup (pSection);
            psect->key_nr_alloc = NR_KEYS_INIT_ALLOC;
            psect->keys = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
            psect->values = malloc (sizeof (char*) * NR_KEYS_INIT_ALLOC);
        }
    }

    return (GHANDLE)psect;
}

int GUIAPI GetValueFromEtcSec (GHANDLE hSect, 
                const char* pKey, char* pValue, int iLen)
{
    PETCSECTION psect = (PETCSECTION)hSect;

    if (psect == NULL)
        return ETC_INVALIDOBJ;

    return etc_GetSectionValue (psect, pKey, pValue, iLen);
}

int GUIAPI GetIntValueFromEtcSec (GHANDLE hSect, const char* pKey, int* pValue)
{
    PETCSECTION psect = (PETCSECTION)hSect;
    int ret;
    char szBuff [51];

    if (psect == NULL)
        return ETC_INVALIDOBJ;

    ret = etc_GetSectionValue (psect, pKey, szBuff, 50);
    if (ret < 0) {
        return ret;
    }

    *pValue = strtol (szBuff, NULL, 0);
    if ((*pValue == LONG_MIN || *pValue == LONG_MAX) && errno == ERANGE)
        return ETC_INTCONV;

    return ETC_OK;
}

int GUIAPI SetValueToEtcSec (GHANDLE hSect, const char* pKey, char* pValue)
{
    PETCSECTION psect = (PETCSECTION)hSect;

    if (psect == NULL)
        return ETC_INVALIDOBJ;

    return etc_GetSectionValue (psect, pKey, pValue, -1);
}

int GUIAPI RemoveSectionInEtc (GHANDLE hEtc, const char* pSection)
{
    int i;
    PETCSECTION psect;

    if (hEtc == 0)
        return ETC_INVALIDOBJ;

    if (((ETC_S*)hEtc)->sect_nr_alloc == 0)
        return ETC_READONLYOBJ;

    psect = (PETCSECTION) FindSectionInEtc (hEtc, pSection, FALSE);
    if (psect == NULL)
        return ETC_SECTIONNOTFOUND;

    for (i = 0; i < psect->key_nr; i ++) {
        FreeFixStr (psect->keys [i]);
        FreeFixStr (psect->values [i]);
    }
    free (psect->keys);
    free (psect->values);
    FreeFixStr (psect->name);

    psect->key_nr = 0;
    psect->name = NULL;
    psect->keys = NULL;
    psect->values = NULL;

    return ETC_OK;
}

/* This function locate the specified section in the etc file. */
static int etc_LocateSection_2 (FILE* fp, const char* pSection, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1];
    char line[ETC_MAXLINE + 1];
    char *name;

    while (TRUE) {
        if (!fgets(line, ETC_MAXLINE, fp)) {
            if (feof (fp))
                return ETC_SECTIONNOTFOUND;
            else
                return ETC_FILEIOFAILED;
        }

        strcpy (szBuff, line);
        name = get_section_name (szBuff);

        if (name && strcmp (name, pSection) == 0)
            return ETC_OK; 
        else if (bak_fp && fputs (line, bak_fp) == EOF)
            return ETC_FILEIOFAILED;
    }

    return ETC_SECTIONNOTFOUND;
}

static int etc_FindNextSection (FILE* fp)
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

static int etc_ReviseSectionInEtcFile (const char* pEtcFile, PETCSECTION psect, 
                            BOOL bRemove)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;

    if ((tmp_fp = __mg_tmpfile ()) == NULL)
        return ETC_TMPFILEFAILED;

    /* open or create a etc file */
    if (!(etc_fp = fopen (pEtcFile, "r+"))) {
        __mg_close_tmpfile (tmp_fp);

        if (bRemove) {
            rc = ETC_SECTIONNOTFOUND;
        }
        else {
            if (!(etc_fp = fopen (pEtcFile, "w"))) {
                return ETC_FILEIOFAILED;
            }

            rc = etc_WriteSection (etc_fp, psect, TRUE);
            fclose (etc_fp);
        }

        return rc;
    }

    rc = etc_LocateSection_2 (etc_fp, psect->name, tmp_fp);

    if (rc == ETC_SECTIONNOTFOUND) {
        __mg_close_tmpfile (tmp_fp);
        if (!bRemove) {
            etc_WriteSection (etc_fp, psect, TRUE);
            fclose (etc_fp);
            return ETC_OK;
        }
        else
            return ETC_SECTIONNOTFOUND;
    }
    else if (rc == ETC_FILEIOFAILED) {
        __mg_close_tmpfile (tmp_fp);
        fclose (etc_fp);
        return ETC_FILEIOFAILED;
    }

    if(!bRemove) {
        etc_WriteSection (tmp_fp, psect, TRUE);
    }

    etc_FindNextSection (etc_fp);

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
        goto error;
    
    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);

    if (!(etc_fp = fopen (pEtcFile, "w"))) {
        __mg_close_tmpfile (tmp_fp);
        return ETC_FILEIOFAILED;
    }
    
    __mg_rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    __mg_close_tmpfile (tmp_fp);
    return rc;
}

int GUIAPI RemoveSectionInEtcFile (const char* pEtcFile, const char *pSection)
{
    ETCSECTION sect;

    sect.name = (char *)pSection;
    sect.key_nr = 0;

    return etc_ReviseSectionInEtcFile (pEtcFile, &sect, TRUE);
}

int GUIAPI SaveSectionToEtcFile (const char* pEtcFile, PETCSECTION psect)
{
    return etc_ReviseSectionInEtcFile (pEtcFile, psect, FALSE);
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

char * strtrimall( char *src)
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

BOOL mg_is_abs_path(const char* path)
{
    if (NULL != path) {
#ifdef WIN32
        if (strlen(path) >= 2 && ':' == path[1]) {
            return TRUE;
        }
#else
        if (strlen(path) >= 1 && '/' == path[0]) {
            return TRUE;
        }
#endif
    }
    return FALSE;
}

int mg_path_joint(char* dst, int dst_size, const char* abs_path, const char* sub_path)
{
    if (NULL != dst && NULL != abs_path && NULL != sub_path) {
        if (mg_is_abs_path(abs_path) && (!mg_is_abs_path(sub_path))) {
            if (dst_size >= strlen(abs_path) + strlen(sub_path) 
                        + 2/* size of split '/' and terminator '\0' */) {
                sprintf(dst, "%s/%s", abs_path, sub_path);
                return 0;
            }
        }
    }
    return -1;
}

