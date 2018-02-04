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
#include "common.h"

#ifdef _MGIAL_DLCUSTOM

#include <dlfcn.h>
#include "ial.h"
#include "dlcustom.h"

#define DEFAULT_CUSTOM_IAL_LIB_PATH "/usr/lib/libiale_custom.so"

static void *dl_handle = NULL;
static BOOL (*sym_InitDLCustomInput) (INPUT* input, const char* mdev, const char* mtype) = NULL;
static void (*sym_TermDLCustomInput) (void) = NULL;

BOOL InitDLCustomInput (INPUT* input, const char* mdev, const char* mtype)
{
    const char *path;

    if ((path = getenv("MG_ENV_DLCUSTOM_IAL")) == NULL)
    {
        path = DEFAULT_CUSTOM_IAL_LIB_PATH;
    }

    dl_handle = dlopen(path, RTLD_NOW);
    if (dl_handle == NULL)
    {
        fprintf(stderr, "dlopen(%s): %s\n", path, dlerror());
        return FALSE;
    }

    sym_InitDLCustomInput = dlsym(dl_handle, "InitDLCustomInput");
    if (! sym_InitDLCustomInput)
    {
        fprintf(stderr, "dlsym(%s): %s\n", "InitDLCustomInput", dlerror());
        return FALSE;
    }

    sym_TermDLCustomInput = dlsym(dl_handle, "TermDLCustomInput");
    if (! sym_TermDLCustomInput)
    {
        fprintf(stderr, "dlsym(%s): %s\n", "TermDLCustomInput", dlerror());
        return FALSE;
    }

    return sym_InitDLCustomInput(input, mdev, mtype);
}

void TermDLCustomInput (void)
{
    if (sym_TermDLCustomInput)
    {
        sym_TermDLCustomInput();
    }
    if (dl_handle)
    {
        dlclose(dl_handle);
    }
}

#endif  /* _MGIAL_DLCUSTOM*/
