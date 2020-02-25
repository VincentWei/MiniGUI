///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** misc.h: the head file for Miscellous module.
**
** Create date: 1999/01/03
*/

#ifndef GUI_MISC_H
    #define GUI_MISC_H

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

#ifndef __NOUNIX__
  #include <sys/types.h>
#endif

#include <string.h> // for strrchr

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

unsigned int __mg_os_get_random_seed (void);
void __mg_os_time_delay (int ms);
void __mg_os_start_time (void);
DWORD __mg_os_get_time_ms (void);
DWORD __mg_os_get_time_ticks (void);
time_t __mg_os_time (time_t* timep);
time_t __mg_os_mktime (struct tm *tmp);
struct tm *__mg_os_localtime (const time_t * timep);

#define NR_KEYS_INIT_ALLOC      8
#define NR_KEYS_INC_ALLOC       4

#define NR_SECTS_INIT_ALLOC     16
#define NR_SECTS_INC_ALLOC      8

extern GHANDLE hMgEtc;

#ifdef _MGSLICE_FALLBACK
static inline BOOL mg_InitSliceAllocator(void) {
    return TRUE;
}
static inline void mg_TerminateSliceAllocator(void) { }
#else   /* defined _MGSLICE_FALLBACK */
BOOL mg_InitSliceAllocator(void);
void mg_TerminateSliceAllocator(void);
#endif  /* not defined _MGSLICE_FALLBACK */

BOOL mg_InitMgEtc (void);
void mg_TerminateMgEtc (void);

BOOL mg_InitMisc (void);
void mg_TerminateMisc (void);

BOOL mg_InitSystemRes (void);
void mg_TerminateSystemRes (void);

#ifdef _MGSCHEMA_COMPOSITING
int __mg_alloc_sem_for_shared_surf (void);
int __mg_free_sem_for_shared_surf (int sem_num);
#endif /* _MGSCHEMA_COMPOSITING */

int __mg_lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set);
static inline
void __mg_slot_set_use (unsigned char* bitmap, int index) {
    bitmap += index >> 3;
    *bitmap &= (~(0x80 >> (index % 8)));
}

static inline
int __mg_slot_clear_use (unsigned char* bitmap, int index) {
    bitmap += index >> 3;
    if (*bitmap & (0x80 >> (index % 8)))
        return 0;

    *bitmap |= (0x80 >> (index % 8));
    return 1;
}

/* get the number of idle mask rectangles */
static inline
int __mg_get_nr_idle_slots (unsigned char* bitmap, int len_bmp)
{
    int idle = 0;
    int i, j;

    for (i = 0; i < len_bmp; i++) {
        for (j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j))
                idle++;
        }
        bitmap++;
    }
    return idle;
}

BOOL __mg_is_abs_path (const char* path);
int __mg_path_joint (char* dst, int dst_size, const char* abs_path,
        const char* sub_path);

/* Since 5.0.0 */
static inline
const char* __mg_get_extension (const char* filename) {
    const char* ext;
    ext = strrchr (filename, '.');
    if (ext)
        return ext + 1;
    return NULL;
}

// Extract integers in pattern like "100x100" or "100,100", and returns
// the number of integers extracted successfully.
int __mg_extract_integers (const char* s, int c, int* ab, int max_nr);

/* Since 5.0.0 */
#ifndef __NOUNIX__
int __mg_create_anonymous_file (off_t size, const char* debug_name,
        mode_t rw_modes);
#endif

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

