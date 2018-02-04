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
** nposix.c: This file include some miscelleous functions not 
**      provided by POSIX. 
**
** Create date: 2003/11/22
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef HAVE_TIME
#include <time.h>
#endif

#ifndef HAVE_STRDUP
char* strdup (const char *string)
{
    char *new_string = malloc(strlen(string)+1);
    strcpy(new_string,string);
    return new_string;
}
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2)
{
    char c1, c2;

    while (1) {
        c1 = tolower (*s1);
        c2 = tolower (*s2);

        if (*s1 == 0 || *s2 == 0 || c1 != c2)
            break;
        s1++;
        s2++;
    }

    return ((unsigned char)c1 - (unsigned char)c2);
}
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp (const char *s1, const char *s2, unsigned int n)
{
    size_t i;
    char c1 = 0, c2 = 0;

    for (i = 0; i < n; i++) {
        c1 = tolower (*s1);
        c2 = tolower (*s2);
        if (*s1 == 0 || *s2 == 0 || c1 != c2)
            break;
        s1++;
        s2++;
    }

    if (i == n)
        return 0;

    return ((unsigned char)c1 - (unsigned char)c2);
}
#endif

#ifdef WIN32
int fread_long_win32(char* buff, size_t size, int count, FILE* fp)
{
    char *tmp;	
    size_t ret, rest = size * count;
    int len;

    if(rest <= 1024)
    {
        return fread(buff, size, count, fp);
    }

    tmp = buff;
    ret = 0;
    while(rest > 0)
    {	  
        if((len = fread(tmp, 1, 1024, fp)) != 1024)
        {
            ret += len;
            break;
        }
        rest -= 1024;
        ret += 1024;
        tmp += 1024;
    }
    return ret / size;
}
#endif

#if defined (__VXWORKS__) ||defined(__ECOS__) || defined (_EM86_IAL) || defined (_EM85_IAL)
#include <math.h>
double hypot(double x, double y)
{
    return sqrt (x*x + y*y);
}
#endif

#if defined (__VXWORKS__) ||defined(WIN32) || defined (__NUCLEUS_MNT__) || defined (_EM86_IAL) || defined (_EM85_IAL)

/* 
 * Implementation of cbrt(x) for Win32 platform.
 * cbrt(x): Return cube root of x
 */

#if (MGUI_BYTEORDER == MGUI_BIG_ENDIAN) || defined (__arm__)

typedef union
{
    double value;
    struct {
        Uint32 msw;
        Uint32 lsw;
    } parts;
} ieee_double_shape_type;

#else

typedef union
{
    double value;
    struct {
        Uint32 lsw;
        Uint32 msw;
    } parts;
} ieee_double_shape_type;

#endif

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)          \
do {                                \
    ieee_double_shape_type gh_u;    \
    gh_u.value = (d);               \
    (i) = gh_u.parts.msw;           \
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)           \
do {                                \
    ieee_double_shape_type gl_u;    \
    gl_u.value = (d);               \
    (i) = gl_u.parts.lsw;           \
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)     \
do {                                \
    ieee_double_shape_type iw_u;    \
    iw_u.parts.msw = (ix0);         \
    iw_u.parts.lsw = (ix1);         \
    (d) = iw_u.value;               \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)          \
do {                                \
    ieee_double_shape_type sh_u;    \
    sh_u.value = (d);               \
    sh_u.parts.msw = (v);           \
    (d) = sh_u.value;               \
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)           \
do {                                \
    ieee_double_shape_type sl_u;    \
    sl_u.value = (d);               \
    sl_u.parts.lsw = (v);           \
    (d) = sl_u.value;               \
} while (0)


static const Uint32 
    B1 = 715094163, /* B1 = (682-0.03306235651)*2**20 */
    B2 = 696219795; /* B2 = (664-0.03306235651)*2**20 */

static const double
C =  5.42857142857142815906e-01, /* 19/35     = 0x3FE15F15, 0xF15F15F1 */
D = -7.05306122448979611050e-01, /* -864/1225 = 0xBFE691DE, 0x2532C834 */
E =  1.41428571428571436819e+00, /* 99/70     = 0x3FF6A0EA, 0x0EA0EA0F */
F =  1.60714285714285720630e+00, /* 45/28     = 0x3FF9B6DB, 0x6DB6DB6E */
G =  3.57142857142857150787e-01; /* 5/14      = 0x3FD6DB6D, 0xB6DB6DB7 */

double cbrt (double x) 
{
    Sint32 hx;
    double r,s,t=0.0,w;
    Uint32 sign;
    Uint32 high,low;

    GET_HIGH_WORD (hx, x);
    sign=hx&0x80000000;         /* sign= sign(x) */
    hx ^=sign;
    if(hx>=0x7ff00000) return(x+x); /* cbrt(NaN,INF) is itself */
    GET_LOW_WORD(low,x);
    if((hx|low)==0) 
        return(x);        /* cbrt(0) is itself */

    SET_HIGH_WORD(x,hx);    /* x <- |x| */
    /* rough cbrt to 5 bits */
    if(hx<0x00100000)         /* subnormal number */
      {SET_HIGH_WORD(t,0x43500000);    /* set t= 2**54 */
       t*=x; GET_HIGH_WORD(high,t); SET_HIGH_WORD(t,high/3+B2);
      }
    else
      SET_HIGH_WORD(t,hx/3+B1);


    /* new cbrt to 23 bits, may be implemented in single precision */
    r=t*t/x;
    s=C+r*t;
    t*=G+F/(s+E+D/s);    

    /* chopped to 20 bits and make it larger than cbrt(x) */ 
    GET_HIGH_WORD(high,t);
    INSERT_WORDS(t,high+0x00000001,0);


    /* one step newton iteration to 53 bits with error less than 0.667 ulps */
    s=t*t;        /* t*t is exact */
    r=x/s;
    w=t+t;
    r=(r-t)/(w+r);    /* r-s is exact */
    t=t+t*r;

    /* retore the sign bit */
    GET_HIGH_WORD (high,t);
    SET_HIGH_WORD (t,high|sign);
    return (t);
}

#if 0

double cbrt (double x)
{
    double x1, x2;
    double x3;

    if (x <= 1E-6) 
        return 0.0;

    x2 = x;
    while (1) {
        x1 = x2;
        x2 = (2.0 * x1 + x / (x1 *x1)) /3.0;
        x3 = (x2 - x1) / x1;
        if (x3 < 0.0) 
            x3 = -x3;
        if (x3 < 1E-6) break;
    }

    return x2;
}
#endif

#endif /* WIN32 */

/*
#ifndef HAVE_GETTIMEOFDAY
#ifdef __VXWORKS__
int gettimeofday(struct timeval *tv, struct timezone* tz)
{
    struct timespec ts;

    if (!tv)
        return -1;

    if (clock_gettime (CLOCK_REALTIME, &ts) < 0)
        return -1;

    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec*1000;

    return 0;
}
#endif
#endif
*/

#ifdef __CYGWIN__

#include <errno.h>

int sem_getvalue (sem_t *sem, int *sval)
{
    sem_trywait (sem);
    if (errno == EAGAIN)
        *sval = 0;
    else
        *sval = 1;
    return 0;
}

/*
int shmget(key_t key, int size, int shmflg)
{
    return 0;
}
void *shmat(int shmid, const void *shmaddr, int shmflg)
{
    return NULL;
}
int shmdt(const void *shmaddr)
{
    return 0;
}
*/

#endif

/* ------------------------------ time delay -------------------------------- */

#ifndef __NOUNIX__
  #include <unistd.h>
#elif defined (__ECOS__)
  #include <cyg/kernel/kapi.h>
#elif defined (__UCOSII__)
  #include "os_cpu.h"
  #include "os_cfg.h"
  #include "ucos_ii.h"
#elif defined (__WINBOND_SWLINUX__)
  #include "sys/unistd.h"
  #include "linux/delay.h"
#elif defined (__VXWORKS__)
  #include "taskLib.h"
  #include "sysLib.h"
#elif defined (__PSOS__)
  #include "time.h"
#elif defined (__THREADX__)
  #include "tx_api.h"
  #include "tx_tim.h"
#elif defined (__OSE__)
  #include "ose.h"
  #include <time.h>
#endif

/* A portable version of get random seed */
unsigned int __mg_os_get_random_seed (void)
{
#if defined (__UCOSII__)
    return OSTimeGet ();
#elif defined (__THREADX__)
    /* system clock ticks*/
    return tx_time_get();
#elif defined (__NUCLEUS__)
    /* TODO */
    return 0;
#elif defined (__UCOSII__)
    return 0;
#else
    /*other os use time, for example: linux, OSE, etc.*/
    return time (NULL);
#endif
}

void __mg_os_time_delay (int ms)
{
#ifndef __NOUNIX__
    /* use select instead of usleep */
    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=ms*1000;
    while (select (0, NULL, NULL, NULL, &timeout) < 0);
#elif defined (__UCOSII__)
    OSTimeDly (OS_TICKS_PER_SEC * ms / 1000);
#elif defined (__VXWORKS__) 
    taskDelay (sysClkRateGet() * ms / 1000);
#elif defined (__PSOS__) || defined (__ECOS__)
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000000;
    nanosleep (&ts, NULL);
#elif defined (WIN32)
    void __stdcall Sleep (DWORD dwMilliSeconds);
    Sleep (ms);
#elif defined (__THREADX__)
    tx_thread_sleep (ms/10);
#elif defined (__NUCLEUS__)
    NU_Sleep (ms/10);
#elif defined (__OSE__)
    delay(ms);
#endif
}

/* ------------------------------ I/O -------------------------------- */
void __mg_rewind (FILE *fp)
{
#if defined(__THREADX__) && defined(__TARGET_VFANVIL__)
    fseek(fp, 0, SEEK_SET);
#else
    rewind (fp);
#endif
}

FILE* __mg_tmpfile (void)
{
#ifdef HAVE_TMPFILE
    return tmpfile ();
#else
  #if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
    return fopen ("/flash/fhas2-cfg/tmp", "r+");
  #else
    return fopen ("/tmp/mg-etc-tmp", "w+");
  #endif
#endif
}

int __mg_close_tmpfile (FILE *tmp_fp)
{
    return fclose (tmp_fp);
}

#ifdef __TARGET_C33L05__

#undef HAVE_TIME
#undef HAVE_MKTIME
#undef HAVE_LOCALTIME

#include <stdarg.h>
int snprintf( char *s, size_t size, const char *format, ... )
{
    int rc;
    va_list ap;

    va_start (ap, format);

    rc = vsprintf (s, format, ap);

    va_end (ap);

    return rc;
}

int __modsi3 (int numer, int denom)
{
	int quot = numer / denom;
	return numer - (quot * denom);
}

#endif

time_t __mg_time (time_t * timer)
{
#if defined (HAVE_TIME)
    return time (timer);
#else
    return 0;
#endif
}

time_t __mg_mktime (struct tm * timeptr)
{
#if defined (HAVE_MKTIME)
    return mktime (timeptr);
#else
    return 0;
#endif
}

struct tm *__mg_localtime (const time_t * timer)
{
#if defined (HAVE_LOCALTIME)
    return localtime (timer);
#else
    static struct tm _19800101_tm = {
        0, /* tm_sec: seconds */
        0, /* tm_min: minutes */
        0, /* tm_hour: hours */
        1, /* tm_mday: day of the month (1st) */
        0, /* tm_mon: month (Jan) */
        80, /* tm_year: year (1980) */
        2, /* tm_wday: day of the week (1980/01/01 is Tuesday) */
        0, /* tm_yday: day in the year */
        0, /* tm_isdst daylight saving time */
    };

    return &_19800101_tm;
#endif
}

