/*
** $Id: pcivideo.c$
**  
** pcivideo.c: PCIAccess based accelerated video driver implementation.
**
** Copyright (C) 2009 Feynman Software.
**
** Author: WEI Yongming (2009/07/19)
*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _DEBUG
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

