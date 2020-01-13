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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** anon_file.c: This file include some miscelleous functions.
** Based on Mesa's os_create_anonymous_file function.
**
** Create date: 2020/01/08
**
** Current maintainer: Wei Yongming.
*/

#include "common.h"

#ifndef __NOUNIX__

#include "misc.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef _MGUSE_SHMOPEN
#include <sys/mman.h>
#elif defined(HAVE_MEMFD_CREATE)
#include <sys/syscall.h>
#include <linux/memfd.h>
#include <stdlib.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

#if !(defined(_MGUSE_SHMOPEN) || defined(HAVE_MEMFD_CREATE) || defined(HAVE_MKOSTEMP))
static int set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;

err:
    close(fd);
    return -1;
}
#endif

#if !(defined(_MGUSE_SHMOPEN) || defined(HAVE_MEMFD_CREATE))
static int create_tmpfile_cloexec(char *tmpname, mode_t rw_modes)
{
    int fd;

    umask (rw_modes);

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
#else
    fd = mkstemp(tmpname);
#endif

    if (fd < 0) {
        return fd;
    }

#ifndef HAVE_MKOSTEMP
    fd = set_cloexec_or_close(fd);
#endif

    unlink(tmpname);
    return fd;
}
#endif

/*
 * Create a new, unique, anonymous file of the given size, and
 * return the file descriptor for it. The file descriptor is set
 * CLOEXEC. The file is immediately suitable for mmap()'ing
 * the given size at offset zero.
 *
 * An optional name for debugging can be provided as the second argument.
 *
 * The file should not have a permanent backing store like a disk,
 * but may have if MG_RUNTIME_TMPDIR is not properly implemented in the system.
 *
 * If memfd or SHM_ANON is supported, the filesystem is not touched at all.
 * Otherwise, the file name is deleted from the file system.
 *
 * The file is suitable for buffer sharing between processes by
 * transmitting the file descriptor over Unix sockets using the
 * SCM_RIGHTS methods.
 */
int __mg_create_anonymous_file(off_t size, const char *debug_name,
        mode_t rw_modes)
{
    int fd, ret;
#ifdef _MGUSE_SHMOPEN
    fd = shm_open (SHM_ANON, O_CREAT | O_RDWR | O_CLOEXEC, rw_modes);
#elif defined(HAVE_MEMFD_CREATE)
    (void)rw_modes;
    if (!debug_name)
        debug_name = "minigui-shared";
    fd = syscall(SYS_memfd_create, debug_name, MFD_CLOEXEC);
    /* TODO: use fcntl(2) F_ADD_SEALS to restrict the further
        modifications on this memfd.
     */
#else
    const char *path;
    char *name;

    path = getenv("MG_RUNTIME_TMPDIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    if (debug_name)
        asprintf(&name, "%s/minigui-shared-%s-XXXXXX", path, debug_name);
    else
        asprintf(&name, "%s/minigui-shared-XXXXXX", path);
    if (!name)
        return -1;

    fd = create_tmpfile_cloexec(name, rw_modes);
    free (name);
#endif

    if (fd < 0)
        return -1;

    ret = ftruncate(fd, size);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}
#endif

