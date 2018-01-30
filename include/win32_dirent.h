/**
 * \file win32_dirent.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 * \brief This file includes dir entries APIs on Win32 platform.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: minigui.h 13674 2010-12-06 06:45:01Z wanzheng $
 * 
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */
#ifndef _DIRENT_WIN32_
#define _DIRENT_WIN32_

#ifdef __cplusplus
extern "C" {
#endif
    
#define DLL_EXPORT    __declspec(dllexport)
#include <stdio.h>
#include <time.h>
    
typedef int ff_t;
typedef int mode_t;
typedef int dev_t;
typedef int nlink_t;
typedef int uid_t;
typedef int st_gid;
typedef int gid_t;
typedef struct DIR DIR;

struct dirent
{
    int d_ino;
    ff_t d_off;
    signed short int d_reclen;
    unsigned char d_type;
    char d_name[256];    
};

#define S_ISLNK(st_mode) ((st_mode) & 0020000) ? 1 : 0
#define S_ISREG(st_mode) ((st_mode) & 0100000) ? 1 : 0
#define S_ISDIR(st_mode) ((st_mode) & 0040000) ? 1 : 0

DLL_EXPORT DIR *opendir(const char *name);

DLL_EXPORT int closedir(DIR *dir);

DLL_EXPORT struct dirent *readdir(DIR *dir);

DLL_EXPORT int chdir(const char *path);

DLL_EXPORT char *getcwd(char *buf, size_t size);

DLL_EXPORT void rewinddir(DIR *dir);

DLL_EXPORT int mkdir(const char *pathname, mode_t mode);

DLL_EXPORT int rmdir(const char *pathname);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /*_DIRENT_WIN32_*/
