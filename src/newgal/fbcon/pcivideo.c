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
** pcivideo.c: PCIAccess based accelerated video driver implementation.
**
** Author: WEI Yongming (2009/07/19)
*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#ifdef _MGHAVE_PCIACCESS

#include <pciaccess.h>

#include "minigui.h"
#include "newgal.h"
#include "fbvideo.h"
#include "pcivideo.h"

static PCI_VIDEO_DRIVER pci_drivers [] = 
{
    {"siliconmotion", SMI_Probe, NULL, NULL}
};

int FB_ProbePCIAccelDriver (_THIS, const struct fb_fix_screeninfo* fb_finfo)
{
    int i;
    int ret;

    if (pci_system_init ()) {
        _MG_PRINTF ("NEWGAL>FBCON: Couldn't initialize PCI system\n");
        return -1;
    }

    ret = 0;
    for (i = 0; i < TABLESIZE (pci_drivers); i++) {
        if (pci_drivers [i].probe (this, fb_finfo, pci_drivers + i) == 0) {
            ret = i + 1;
            break;
        }
    }

    if (ret)
        return ret;

    return -1;
}

int FB_InitPCIAccelDriver (_THIS, const GAL_Surface* current)
{
    return pci_drivers [pci_accel_driver - 1].init (this, current);
}

int FB_CleanupPCIAccelDriver (_THIS)
{
    if (pci_accel_driver > 0 && pci_accel_driver <= TABLESIZE (pci_drivers)) {
        pci_drivers [pci_accel_driver - 1].cleanup (this);
        pci_system_cleanup ();
        return 0;
    }

    return -1;
}

#endif /* _MGHAVE_PCIACCESS */

