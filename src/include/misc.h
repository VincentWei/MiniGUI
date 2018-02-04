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
** misc.h: the head file for Miscellous module.
**
** Create date: 1999/01/03
*/

#ifndef GUI_MISC_H
    #define GUI_MISC_H

/* Function definitions */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifndef HAVE_SYS_TIME_H

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* microseconds */
};

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

#else
  #include <sys/time.h>
#endif

unsigned int __mg_os_get_random_seed (void);
void __mg_os_time_delay (int ms);

#define NR_KEYS_INIT_ALLOC      8
#define NR_KEYS_INC_ALLOC       4

#define NR_SECTS_INIT_ALLOC     16
#define NR_SECTS_INC_ALLOC      8

extern GHANDLE hMgEtc;

BOOL mg_InitMgEtc (void);
void mg_TerminateMgEtc (void);

BOOL mg_InitMisc (void);
void mg_TerminateMisc (void);

BOOL mg_InitSystemRes (void);
void mg_TerminateSystemRes (void);

int __mg_lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set);
void __mg_slot_set_use (unsigned char* bitmap, int index);
int __mg_slot_clear_use (unsigned char* bitmap, int index);

BOOL mg_is_abs_path(const char* path);
int mg_path_joint(char* dst, int dst_size, const char* abs_path, const char* sub_path);
#if defined (__VXWORKS__) || defined(WIN32) || defined (__NUCLEUS_MNT__) || defined (_EM86_IAL) || defined (_EM85_IAL)
double cbrt(double x);
#endif

#if defined (__VXWORKS__) || defined(__ECOS__) || defined (_EM86_IAL) || defined (_EM85_IAL)
double hypot(double x, double y);
#endif

#ifndef HAVE_STRDUP
char* strdup (const char *string);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp (const char *s1, const char *s2, unsigned int n);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_MISC_H */

