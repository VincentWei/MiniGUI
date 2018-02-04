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
** pci_smi.c: accelerated NEWGAL>FBCON driver for Silicon Motion graphics 
**      chipsets.
**
** Author: WEI Yongming (2009/07/19)
*/

#include <stdio.h>
#include <error.h>
#include <string.h>

/*#define _DEBUG*/
#include "common.h"

#ifdef _MGHAVE_PCIACCESS

#include "newgal.h"
#include "blit.h"
#include "fbvideo.h"

#include <pciaccess.h>

#include "compiler.h"
#include "pciinfo.h"
#include "pcivideo.h"
#include "pci_smi.h"

/* Supported chipsets */
static SymTabRec SMIChipsets[] =
{
    { PCI_CHIP_SMI910, "Lynx"    },
    { PCI_CHIP_SMI810, "LynxE"   },
    { PCI_CHIP_SMI820, "Lynx3D"  },
    { PCI_CHIP_SMI710, "LynxEM"  },
    { PCI_CHIP_SMI712, "LynxEM+" },
    { PCI_CHIP_SMI720, "Lynx3DM" },
    { PCI_CHIP_SMI731, "Cougar3DR" },
    { -1,             NULL      }
};

static SMIRec smi_rec;

static void SMI_EnableMmio (void)
{
#if 0
    SMIPtr pSmi = &smi_rec;
    CARD8 tmp;

    vgaHWPtr hwp = VGAHWPTR(pScrn);

    /*
     * Enable chipset (seen on uninitialized secondary cards) might not be
     * needed once we use the VGA softbooter
     */
    vgaHWSetStdFuncs(hwp);

    /* Enable linear mode */
    outb(pSmi->PIOBase + VGA_SEQ_INDEX, 0x18);
    tmp = inb(pSmi->PIOBase + VGA_SEQ_DATA);
    pSmi->SR18Value = tmp;                                        /* PDR#521 */
    outb(pSmi->PIOBase + VGA_SEQ_DATA, tmp | 0x11);

    /* Enable 2D/3D Engine and Video Processor */
    outb(pSmi->PIOBase + VGA_SEQ_INDEX, 0x21);
    tmp = inb(pSmi->PIOBase + VGA_SEQ_DATA);
    pSmi->SR21Value = tmp;                                        /* PDR#521 */
    outb(pSmi->PIOBase + VGA_SEQ_DATA, tmp & ~0x03);
#endif
}

static void SMI_DisableMmio (void)
{
#if 0
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    SMIPtr pSmi = &smi_rec;

    vgaHWSetStdFuncs(hwp);

    /* Disable 2D/3D Engine and Video Processor */
    outb(pSmi->PIOBase + VGA_SEQ_INDEX, 0x21);
    outb(pSmi->PIOBase + VGA_SEQ_DATA, pSmi->SR21Value);        /* PDR#521 */

    /* Disable linear mode */
    outb(pSmi->PIOBase + VGA_SEQ_INDEX, 0x18);
    outb(pSmi->PIOBase + VGA_SEQ_DATA, pSmi->SR18Value);        /* PDR#521 */
#endif
}

static BOOL SMI_MapMem (_THIS)
{
    SMIPtr pSmi = &smi_rec;
    CARD32 memBase;
    unsigned char config;

    /* Map the Lynx register space */
    switch (pSmi->Chipset) {
    default:
        memBase = PCI_REGION_BASE(pSmi->PciInfo, 0, REGION_MEM) + 0x400000;
        pSmi->MapSize = 0x10000;
        break;
    case SMI_COUGAR3DR:
        memBase = PCI_REGION_BASE(pSmi->PciInfo, 1, REGION_MEM);
        pSmi->MapSize = 0x200000;
        break;
    case SMI_LYNX3D:
        memBase = PCI_REGION_BASE(pSmi->PciInfo, 0, REGION_MEM) + 0x680000;
        pSmi->MapSize = 0x180000;
        break;
    case SMI_LYNXEM:
    case SMI_LYNXEMplus:
        memBase = PCI_REGION_BASE(pSmi->PciInfo, 0, REGION_MEM) + 0x400000;
        pSmi->MapSize = 0x400000;
        break;
    case SMI_LYNX3DM:
        memBase = PCI_REGION_BASE(pSmi->PciInfo, 0, REGION_MEM);
        pSmi->MapSize = 0x200000;
        break;
    }

    _MG_PRINTF ("NEWGAL>FBCON>SMI: Physical MMIO at 0x%08lX\n", (unsigned long)memBase);

    {
        void** result = (void*)(&pSmi->MapBase);
        int err = pci_device_map_range(pSmi->PciInfo,
                        memBase,
                        pSmi->MapSize,
                        PCI_DEV_MAP_FLAG_WRITABLE,
                        result);
        if (err) {
            perror ("pci_device_map_range");
            _MG_PRINTF ("NEWGAL>FBCON>SMI: pci_device_map_range failure: %s.\n", strerror (err));
            return FALSE;
        }
    }

    if (pSmi->MapBase == NULL) {
        _MG_PRINTF ("NEWGAL>FBCON>SMI: Internal error: could not map MMIO registers.\n");
        return FALSE;
    }

    switch (pSmi->Chipset) {
    default:
        pSmi->DPRBase = pSmi->MapBase + 0x8000;
        pSmi->VPRBase = pSmi->MapBase + 0xC000;
        pSmi->CPRBase = pSmi->MapBase + 0xE000;
        pSmi->IOBase  = NULL;
        pSmi->DataPortBase = pSmi->MapBase;
        pSmi->DataPortSize = 0x8000;
        break;
    case SMI_COUGAR3DR:
        pSmi->DPRBase = pSmi->MapBase + 0x000000;
        pSmi->VPRBase = pSmi->MapBase + 0x000800;
        pSmi->CPRBase = pSmi->MapBase + 0x001000;
        pSmi->FPRBase = pSmi->MapBase + 0x005800;
        pSmi->IOBase  = pSmi->MapBase + 0x0C0000;
        pSmi->DataPortBase = pSmi->MapBase + 0x100000;
        pSmi->DataPortSize = 0x100000;
        break;
    case SMI_LYNX3D:
        pSmi->DPRBase = pSmi->MapBase + 0x000000;
        pSmi->VPRBase = pSmi->MapBase + 0x000800;
        pSmi->CPRBase = pSmi->MapBase + 0x001000;
        pSmi->IOBase  = pSmi->MapBase + 0x040000;
        pSmi->DataPortBase = pSmi->MapBase + 0x080000;
        pSmi->DataPortSize = 0x100000;
        break;
    case SMI_LYNXEM:
    case SMI_LYNXEMplus:
        pSmi->DPRBase = pSmi->MapBase + 0x008000;
        pSmi->VPRBase = pSmi->MapBase + 0x00C000;
        pSmi->CPRBase = pSmi->MapBase + 0x00E000;
        pSmi->IOBase  = pSmi->MapBase + 0x300000;
        pSmi->DataPortBase = pSmi->MapBase /*+ 0x100000*/;
        pSmi->DataPortSize = 0x8000 /*0x200000*/;
        break;
    case SMI_LYNX3DM:
        pSmi->DPRBase = pSmi->MapBase + 0x000000;
        pSmi->VPRBase = pSmi->MapBase + 0x000800;
        pSmi->CPRBase = pSmi->MapBase + 0x001000;
        pSmi->IOBase  = pSmi->MapBase + 0x0C0000;
        pSmi->DataPortBase = pSmi->MapBase + 0x100000;
        pSmi->DataPortSize = 0x100000;
        break;
    }

    _MG_PRINTF ("NEWGAL>FBCON>SMI: Logical MMIO at %p - %p\n", pSmi->MapBase,
           pSmi->MapBase + pSmi->MapSize - 1);
    _MG_PRINTF ("NEWGAL>FBCON>SMI: DPR=%p, VPR=%p, IOBase=%p\n",
           pSmi->DPRBase, pSmi->VPRBase, pSmi->IOBase);
    _MG_PRINTF ("NEWGAL>FBCON>SMI: DataPort=%p - %p\n", pSmi->DataPortBase,
           pSmi->DataPortBase + pSmi->DataPortSize - 1);
    pSmi->memPhysBase = PCI_REGION_BASE(pSmi->PciInfo, 0, REGION_MEM);

    SMI_EnableMmio ();

    /* Next go on to detect amount of installed ram */
    config = VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x71);

    /* And compute the amount of video memory and offscreen memory */
    pSmi->videoRAMKBytes = 0;

    switch (pSmi->Chipset) {
    default:
    {
        int mem_table[4] = { 1, 2, 4, 0 };
        pSmi->videoRAMKBytes = mem_table[(config >> 6)] * 1024;
        break;
    }
    case SMI_LYNX3D:
    {
        int mem_table[4] = { 0, 2, 4, 6 };
        pSmi->videoRAMKBytes = mem_table[(config >> 6)] * 1024 + 512;
        break;
    }
    case SMI_LYNX3DM:
    {
        int mem_table[4] = { 16, 2, 4, 8 };
        pSmi->videoRAMKBytes = mem_table[(config >> 6)] * 1024;
        break;
    }
    case SMI_COUGAR3DR:
    {
        /* DANGER - Cougar3DR BIOS is broken - hardcode video ram size */
        /* per instructions from Silicon Motion engineers */
        pSmi->videoRAMKBytes = 16 * 1024;
        break;
    }
    }

    pSmi->videoRAMBytes = pSmi->videoRAMKBytes * 1024;

    _MG_PRINTF ("NEWGAL>FBCON>SMI: videoram: %dkB\n", pSmi->videoRAMKBytes);

    if (pSmi->videoRAMBytes) {
        /* Map the frame buffer */
        if (pSmi->Chipset == SMI_LYNX3DM) 
            pSmi->fbMapOffset = 0x200000;
        else
            pSmi->fbMapOffset = 0x0;

        pSmi->FBOffset = 0;

        pSmi->fbOffset = pSmi->FBOffset + pSmi->fbMapOffset;

        {
            void** result = (void*)&pSmi->FBBase;
            int err = pci_device_map_range(pSmi->PciInfo,
                        pSmi->memPhysBase + pSmi->fbMapOffset,
                        pSmi->videoRAMBytes,
                        PCI_DEV_MAP_FLAG_WRITABLE |
                        PCI_DEV_MAP_FLAG_WRITE_COMBINE,
                        result);
      
            if (err) {
                perror ("pci_device_map_range");
                _MG_PRINTF ("NEWGAL>FBCON>SMI: pci_device_map_range failure.\n");
                return FALSE;
            }
        }

        if (pSmi->FBBase == NULL) {
            _MG_PRINTF ("NEWGAL>FBCON>SMI: Internal error: could not "
                "map framebuffer.\n");
            return FALSE;
        }

        _MG_PRINTF ("NEWGAL>FBCON>SMI: Physical frame buffer at 0x%08lX offset: 0x%08lX\n",
                pSmi->memPhysBase, pSmi->fbOffset);
        _MG_PRINTF ("NEWGAL>FBCON>SMI: Logical frame buffer at %p - %p\n", pSmi->FBBase,
                pSmi->FBBase + pSmi->videoRAMBytes - 1);

        /* Set up offset to hwcursor memory area.  It's a 1K chunk at the end of
         * the frame buffer.
         */
        pSmi->FBCursorOffset = pSmi->videoRAMBytes - 1024;

        _MG_PRINTF ("NEWGAL>FBCON>SMI: Cursor Offset: %08lX\n",
                (unsigned long)pSmi->FBCursorOffset);

        /* set up the fifo reserved space */
        if (VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x30) & 0x01)/* #1074 */ {
            CARD32 fifoOffset = 0;
            fifoOffset |= VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x46) << 3;
            fifoOffset |= VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x47) << 11;
            fifoOffset |= (VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x49)
                & 0x1C) << 17;
            pSmi->FBReserved = fifoOffset;    /* PDR#1074 */
        }
        else {
            pSmi->FBReserved = pSmi->videoRAMBytes - 2048;
        }

        _MG_PRINTF ("NEWGAL>FBCON>SMI: Reserved: %08lX\n",
                (unsigned long)pSmi->FBReserved);

    }

#if 0
    /* Assign hwp->MemBase & IOBase here */
    hwp = VGAHWPTR(pScrn);
    if (pSmi->IOBase != NULL) {
        vgaHWSetMmioFuncs(hwp, pSmi->MapBase, pSmi->IOBase - pSmi->MapBase);
    }
    vgaHWGetIOBase(hwp);

    /* Map the VGA memory when the primary video */
    if (xf86IsPrimaryPci(pSmi->PciInfo)) {
        hwp->MapSize = 0x10000;
        if (!vgaHWMapMem(pScrn)) {
            return FALSE;
        }
        pSmi->PrimaryVidMapped = TRUE;
    }
#endif

    return TRUE;
}

static void SMI_UnmapMem (_THIS)
{
    SMIPtr pSmi = &smi_rec;

#if 0
    /* Unmap VGA mem if mapped. */
    if (pSmi->PrimaryVidMapped) {
        vgaHWUnmapMem(pScrn);
        pSmi->PrimaryVidMapped = FALSE;
    }
#endif

    SMI_DisableMmio ();

    pci_device_unmap_range (pSmi->PciInfo, (void*) pSmi->MapBase, pSmi->MapSize);
    if (pSmi->FBBase != NULL) {
        pci_device_unmap_range (pSmi->PciInfo, (void*) pSmi->FBBase,
                        pSmi->videoRAMBytes);
    }
}

static void SMI_EngineReset (void);
static void SMI_DisableClipping (void);

static void SMI_GEReset (int from_timeout, int line, char *file)
{
    SMIPtr pSmi = &smi_rec;
    CARD8 tmp;

    ENTER_PROC("SMI_GEReset");

    if (from_timeout) {
        if (pSmi->GEResetCnt++ < 10) {
            _MG_PRINTF ("\tSMI_GEReset called from %s line %d\n", file, line);
        }
    }
    else {
        WaitIdleEmpty();
    }

    tmp = VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x15);
    VGAOUT8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x15, tmp | 0x30);

    WaitIdleEmpty();

    VGAOUT8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x15, tmp);
    SMI_EngineReset();

    LEAVE_PROC("SMI_GEReset");
}

/* The sync function for the GE */
static void SMI_AccelSync (void)
{
    SMIPtr pSmi = &smi_rec;

    ENTER_PROC("SMI_AccelSync");

    WaitIdleEmpty(); /* #161 */

    LEAVE_PROC("SMI_AccelSync");
}

static void SMI_EngineReset (void)
{
    SMIPtr pSmi = &smi_rec;
    CARD32 DEDataFormat = 0;
    int i;
    int xyAddress[] = { 320, 400, 512, 640, 800, 1024, 1280, 1600, 2048 };

    ENTER_PROC("SMI_EngineReset");

    pSmi->Stride = (pSmi->width * pSmi->Bpp + 15) & ~15;

    switch (pSmi->depth) {
    case 8:
        DEDataFormat = 0x00000000;
        break;
    case 16:
        pSmi->Stride >>= 1;
        DEDataFormat = 0x00100000;
        break;
    case 24:
        DEDataFormat = 0x00300000;
        break;
    case 32:
        pSmi->Stride >>= 2;
        DEDataFormat = 0x00200000;
        break;
    }

    for (i = 0; i < sizeof(xyAddress) / sizeof(xyAddress[0]); i++) {
        if (pSmi->rotate) {
            if (xyAddress[i] == pSmi->height) {
                DEDataFormat |= i << 16;
                break;
            }
        } else {
            if (xyAddress[i] == pSmi->width) {
                DEDataFormat |= i << 16;
                break;
            }
        }
    }

    WaitIdleEmpty();
    WRITE_DPR(pSmi, 0x10, (pSmi->Stride << 16) | pSmi->Stride);
    WRITE_DPR(pSmi, 0x1C, DEDataFormat);
    WRITE_DPR(pSmi, 0x24, 0xFFFFFFFF);
    WRITE_DPR(pSmi, 0x28, 0xFFFFFFFF);
    WRITE_DPR(pSmi, 0x3C, (pSmi->Stride << 16) | pSmi->Stride);
    WRITE_DPR(pSmi, 0x40, pSmi->FBOffset >> 3);
    WRITE_DPR(pSmi, 0x44, pSmi->FBOffset >> 3);

    SMI_DisableClipping ();

    LEAVE_PROC ("SMI_EngineReset");
}

/******************************************************************************/
/*  Clipping                                                                      */
/******************************************************************************/

#if 0
static void SMI_SetClippingRectangle (int left, int top, int right, int bottom)
{
    SMIPtr pSmi = &smi_rec;

    ENTER_PROC("SMI_SetClippingRectangle");
    _MG_PRINTF ("left=%d top=%d right=%d bottom=%d\n", left, top, right, bottom);

    /* CZ 26.10.2001: this code prevents offscreen pixmaps being drawn ???
        left   = max(left, 0);
        top    = max(top, 0);
        right  = min(right, pSmi->width);
        bottom = min(bottom, pSmi->height);
    */

    if (pSmi->depth == 24) {
        left  *= 3;
        right *= 3;

        if (pSmi->Chipset == SMI_LYNX) {
            top    *= 3;
            bottom *= 3;
        }
    }

    pSmi->ScissorsLeft = (top << 16) | (left & 0xFFFF) | 0x2000;
    pSmi->ScissorsRight = (bottom << 16) | (right & 0xFFFF);

    pSmi->ClipTurnedOn = FALSE;

    WaitQueue(2);
    WRITE_DPR(pSmi, 0x2C, pSmi->ScissorsLeft);
    WRITE_DPR(pSmi, 0x30, pSmi->ScissorsRight);

    LEAVE_PROC("SMI_SetClippingRectangle");
}
#endif

static void SMI_DisableClipping (void)
{
    SMIPtr pSmi = &smi_rec;

    ENTER_PROC("SMI_DisableClipping");

    pSmi->ScissorsLeft = 0;
    if (pSmi->depth == 24) {
        if (pSmi->Chipset == SMI_LYNX) {
            pSmi->ScissorsRight = ((pSmi->height * 3) << 16) | (pSmi->width * 3);
        } else {
            pSmi->ScissorsRight = (pSmi->height << 16) | (pSmi->width * 3);
        }
    } else {
        pSmi->ScissorsRight = (pSmi->height << 16) | pSmi->width;
    }

    pSmi->ClipTurnedOn = FALSE;

    WaitQueue(2);
    WRITE_DPR(pSmi, 0x2C, pSmi->ScissorsLeft);
    WRITE_DPR(pSmi, 0x30, pSmi->ScissorsRight);

    LEAVE_PROC("SMI_DisableClipping");
}

#define GXclear         0x0        /* 0 */
#define GXand           0x1        /* src AND dst */
#define GXandReverse    0x2        /* src AND NOT dst */
#define GXcopy          0x3        /* src */
#define GXandInverted   0x4        /* NOT src AND dst */
#define GXnoop          0x5        /* dst */
#define GXxor           0x6        /* src XOR dst */
#define GXor            0x7        /* src OR dst */
#define GXnor           0x8        /* NOT src AND NOT dst */
#define GXequiv         0x9        /* NOT src XOR dst */
#define GXinvert        0xa        /* NOT dst */
#define GXorReverse     0xb        /* src OR NOT dst */
#define GXcopyInverted  0xc        /* NOT src */
#define GXorInverted    0xd        /* NOT src OR dst */
#define GXnand          0xe        /* NOT src OR NOT dst */
#define GXset           0xf        /* 1 */

static CARD8 SMI_SolidRop[16] =    /* table stolen from KAA */
{
    /* GXclear      */      0x00,         /* 0 */
    /* GXand        */      0xA0,         /* src AND dst */
    /* GXandReverse */      0x50,         /* src AND NOT dst */
    /* GXcopy       */      0xF0,         /* src */
    /* GXandInverted*/      0x0A,         /* NOT src AND dst */
    /* GXnoop       */      0xAA,         /* dst */
    /* GXxor        */      0x5A,         /* src XOR dst */
    /* GXor         */      0xFA,         /* src OR dst */
    /* GXnor        */      0x05,         /* NOT src AND NOT dst */
    /* GXequiv      */      0xA5,         /* NOT src XOR dst */
    /* GXinvert     */      0x55,         /* NOT dst */
    /* GXorReverse  */      0xF5,         /* src OR NOT dst */
    /* GXcopyInverted*/     0x0F,         /* NOT src */
    /* GXorInverted */      0xAF,         /* NOT src OR dst */
    /* GXnand       */      0x5F,         /* NOT src OR NOT dst */
    /* GXset        */      0xFF,         /* 1 */
};

static CARD8 SMI_BltRop[16] =    /* table stolen from KAA */
{
    /* GXclear      */      0x00,         /* 0 */
    /* GXand        */      0x88,         /* src AND dst */
    /* GXandReverse */      0x44,         /* src AND NOT dst */
    /* GXcopy       */      0xCC,         /* src */
    /* GXandInverted*/      0x22,         /* NOT src AND dst */
    /* GXnoop       */      0xAA,         /* dst */
    /* GXxor        */      0x66,         /* src XOR dst */
    /* GXor         */      0xEE,         /* src OR dst */
    /* GXnor        */      0x11,         /* NOT src AND NOT dst */
    /* GXequiv      */      0x99,         /* NOT src XOR dst */
    /* GXinvert     */      0x55,         /* NOT dst */
    /* GXorReverse  */      0xDD,         /* src OR NOT dst */
    /* GXcopyInverted*/     0x33,         /* NOT src */
    /* GXorInverted */      0xBB,         /* NOT src OR dst */
    /* GXnand       */      0x77,         /* NOT src OR NOT dst */
    /* GXset        */      0xFF,         /* 1 */
};

static void SMI_SetupForSolidFill (int color, int rop, unsigned int planemask)
{
    SMIPtr pSmi = &smi_rec;

    pSmi->AccelCmd = SMI_SolidRop [rop]
                   | SMI_BITBLT
                   | SMI_START_ENGINE;

    if (pSmi->ClipTurnedOn) {
        WaitQueue(4);
        WRITE_DPR(pSmi, 0x2C, pSmi->ScissorsLeft);
        pSmi->ClipTurnedOn = FALSE;
    } else {
        WaitQueue(3);
    }

    WRITE_DPR(pSmi, 0x14, color);
    WRITE_DPR(pSmi, 0x34, 0xFFFFFFFF);
    WRITE_DPR(pSmi, 0x38, 0xFFFFFFFF);
}

static void SMI_SubsequentSolidFillRect (int x, int y, int w, int h)
{
    SMIPtr pSmi = &smi_rec;

    if (pSmi->depth == 24) {
        x *= 3;
        w *= 3;

        if (pSmi->Chipset == SMI_LYNX) {
            y *= 3;
        }
    }

    WaitQueue(3);
    WRITE_DPR(pSmi, 0x04, (x << 16) | (y & 0xFFFF));
    WRITE_DPR(pSmi, 0x08, (w << 16) | (h & 0xFFFF));
    WRITE_DPR(pSmi, 0x0C, pSmi->AccelCmd);
}

/******************************************************************************/
/*        Screen to Screen Copies                                                      */
/******************************************************************************/

static void SMI_SetupForScreenToScreenCopy (int xdir, int ydir, int rop,
                               unsigned int planemask, int trans)
{
    SMIPtr pSmi = &smi_rec;

    _MG_PRINTF ("xdir=%d ydir=%d rop=%02X trans=%08X\n", xdir, ydir, rop, trans);

    pSmi->AccelCmd = SMI_BltRop [rop]
                   | SMI_BITBLT
                   | SMI_START_ENGINE;

    if ((xdir == -1) || (ydir == -1)) {
        pSmi->AccelCmd |= SMI_RIGHT_TO_LEFT;
    }

    if (trans != -1) {
        pSmi->AccelCmd |= SMI_TRANSPARENT_SRC | SMI_TRANSPARENT_PXL;
        WaitQueue(1);
        WRITE_DPR(pSmi, 0x20, trans);
    }

    if (pSmi->ClipTurnedOn) {
        WaitQueue(1);
        WRITE_DPR(pSmi, 0x2C, pSmi->ScissorsLeft);
        pSmi->ClipTurnedOn = FALSE;
    }
}

static void SMI_SubsequentScreenToScreenCopy (int x1, int y1, int x2,
                                 int y2, int w, int h)
{
    SMIPtr pSmi = &smi_rec;

    _MG_PRINTF ("x1=%d y1=%d x2=%d y2=%d w=%d h=%d\n", x1, y1, x2, y2, w, h);

    if (pSmi->AccelCmd & SMI_RIGHT_TO_LEFT) {
        x1 += w - 1;
        y1 += h - 1;
        x2 += w - 1;
        y2 += h - 1;
    }

    if (pSmi->depth == 24) {
        x1 *= 3;
        x2 *= 3;
        w  *= 3;

        if (pSmi->Chipset == SMI_LYNX) {
            y1 *= 3;
            y2 *= 3;
        }

        if (pSmi->AccelCmd & SMI_RIGHT_TO_LEFT) {
            x1 += 2;
            x2 += 2;
        }
    }

    WaitQueue(4);
    WRITE_DPR(pSmi, 0x00, (x1 << 16) + (y1 & 0xFFFF));
    WRITE_DPR(pSmi, 0x04, (x2 << 16) + (y2 & 0xFFFF));
    WRITE_DPR(pSmi, 0x08, (w  << 16) + (h  & 0xFFFF));
    WRITE_DPR(pSmi, 0x0C, pSmi->AccelCmd);
}

static int SMI_FillHWRect (_THIS, GAL_Surface *dst, GAL_Rect *rect, Uint32 color)
{
    SMIPtr pSmi = &smi_rec;
    int x = rect->x;
    int y = rect->y;

    if (this->screen->pixels != dst->pixels) {
        y += ((UINT)dst->pixels - (UINT)this->screen->pixels)/this->screen->pitch;
        x += (((UINT)dst->pixels - (UINT)this->screen->pixels)%this->screen->pitch)/pSmi->Bpp;
    }

    SMI_SetupForSolidFill (color, GXcopy, ~0);
    SMI_SubsequentSolidFillRect (x, y, rect->w, rect->h);

#if 0
    SET_SYNC_FLAG(pSmi);
#else
    SMI_AccelSync ();
#endif

    return 0;
}

static void SMI_BlitRect (int srcx, int srcy, int w, int h, int dstx,
             int dsty)
{
    int xdir = ((srcx < dstx) && (srcy == dsty)) ? -1 : 1;
    int ydir = (srcy < dsty) ? -1 : 1;

    SMI_SetupForScreenToScreenCopy (xdir, ydir, GXcopy, ~0, -1);
    SMI_SubsequentScreenToScreenCopy (srcx, srcy, dstx, dsty, w, h);

#if 0
    SET_SYNC_FLAG(pSmi);
#else
    SMI_AccelSync ();
#endif
}

static void SMI_BlitTransRect (int srcx, int srcy, int w, int h, int dstx,
                  int dsty, unsigned long color)
{
    int xdir = ((srcx < dstx) && (srcy == dsty)) ? -1 : 1;
    int ydir = (srcy < dsty) ? -1 : 1;

    SMI_SetupForScreenToScreenCopy (xdir, ydir, GXcopy, ~0, color);
    SMI_SubsequentScreenToScreenCopy (srcx, srcy, dstx, dsty, w, h);

#if 0
    SET_SYNC_FLAG(pSmi);
#else
    SMI_AccelSync ();
#endif
}

static int SMI_HWAccelBlit (GAL_Surface *src, GAL_Rect *srcrect,
                       GAL_Surface *dst, GAL_Rect *dstrect)
{
    SMIPtr pSmi = &smi_rec;
    int srcx = srcrect->x;
    int srcy = srcrect->y;
    int dstx = dstrect->x;
    int dsty = dstrect->y;

    if (pSmi->this->screen->pixels != src->pixels) {
        srcy += ((UINT)src->pixels - (UINT)pSmi->this->screen->pixels)
                    /pSmi->this->screen->pitch;
        srcx += (((UINT)src->pixels - (UINT)pSmi->this->screen->pixels)
                    %pSmi->this->screen->pitch)/pSmi->Bpp;
    }
    if (pSmi->this->screen->pixels != dst->pixels) {
        dsty += ((UINT)dst->pixels - (UINT)pSmi->this->screen->pixels)
                    /pSmi->this->screen->pitch;
        dstx += (((UINT)dst->pixels - (UINT)pSmi->this->screen->pixels)
                    %pSmi->this->screen->pitch)/pSmi->Bpp;
    }

    if ((src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        SMI_BlitTransRect (srcx, srcy, srcrect->w, srcrect->h, 
                    dstx, dsty, src->format->colorkey);
    }
    else {
        SMI_BlitRect (srcx, srcy, srcrect->w, srcrect->h, 
                    dstx, dsty);
    }

    return 0;
}

/* Wait for vertical retrace */
static void SMI_WaitVBL (_THIS)
{
#if 1
    SMIPtr pSmi = &smi_rec;
    WaitIdle ();
#else
    VerticalRetraceWait ();
#endif
}

static void SMI_WaitIdle (_THIS)
{
#if 1
    SMI_AccelSync ();
#else
    SMIPtr pSmi = &smi_rec;
    WaitIdle ();
#endif
}

/* Sets video mem colorkey and accelerated blit function */
static int SMI_SetHWColorKey (_THIS, GAL_Surface *surface, Uint32 key)
{
    return 0;
}

static int SMI_CheckHWBlit (_THIS, GAL_Surface *src, GAL_Surface *dst)
{
    int accelerated;

    /* Set initial acceleration on */
    src->flags |= GAL_HWACCEL;

    /* Set the surface attributes */
    if ( (src->flags & GAL_SRCALPHA) == GAL_SRCALPHA ) {
        src->flags &= ~GAL_HWACCEL;
    }

    if (src->format->BitsPerPixel < 8)
        src->flags &= ~GAL_HWACCEL;

    if (src->format->BitsPerPixel == 24 && 
            (src->flags & GAL_SRCCOLORKEY) == GAL_SRCCOLORKEY) {
        src->flags &= ~GAL_HWACCEL;
    }

    /* Check to see if final surface blit is accelerated */
    accelerated = !!(src->flags & GAL_HWACCEL);
    if (accelerated) {
        src->map->hw_blit = SMI_HWAccelBlit;
    }

    return (accelerated);
}

static int SMI_Init (_THIS, const GAL_Surface* current)
{
    SMIPtr pSmi = &smi_rec;

    if (!SMI_MapMem (this))
        return -1;

    pSmi->this = this;
    pSmi->width = current->w;
#if 0
    pSmi->height = current->h;
#else
    pSmi->height = mapped_memlen / current->pitch;
#endif
    pSmi->Bpp = current->format->BytesPerPixel;
    pSmi->depth = current->format->BitsPerPixel;

    pSmi->GEResetCnt = 0;
    pSmi->NoPCIRetry = TRUE;
    pSmi->shadowFB = FALSE;
    pSmi->rotate = 0;

    SMI_GEReset (0, __LINE__, __FILE__);
    SMI_DisableClipping ();

    /* We have hardware accelerated surface functions */
    this->CheckHWBlit = SMI_CheckHWBlit;
    wait_vbl = SMI_WaitVBL;
    wait_idle = SMI_WaitIdle;

    /* SMI has an accelerated color fill */
    this->info.blit_fill = 1;
    this->FillHWRect = SMI_FillHWRect;

    /* SMI has accelerated normal and colorkey blits */
    this->info.blit_hw = 1;
    this->info.blit_hw_A = 0;
    this->info.blit_hw_CC = 1;
    this->SetHWColorKey = SMI_SetHWColorKey;
    return 0;
}

static int SMI_Cleanup (_THIS)
{
    SMI_UnmapMem (this);
    return 0;
}

int SMI_Probe (_THIS, const struct fb_fix_screeninfo* fb_finfo, PCI_VIDEO_DRIVER* drv)
{
    struct pci_device_iterator * iter;
    struct pci_device * dev;
    struct pci_id_match id_match;
    int num_chipset;

    id_match.vendor_id = PCI_VENDOR_SMI;
    id_match.device_id = PCI_MATCH_ANY;
    id_match.subvendor_id = PCI_MATCH_ANY;
    id_match.subdevice_id = PCI_MATCH_ANY;
    id_match.device_class = 0x030000;
    id_match.device_class_mask = 0xFFFFFF;
    id_match.match_data = 0;

    iter = pci_id_match_iterator_create (&id_match);

    while ((dev = pci_device_next (iter)) != NULL) {
        _MG_PRINTF ("NEWGAL>FBCON>SMI: found PCI video device by SMI: 0x%x (0x%x, 0x%x)\n", 
                dev->device_id, dev->subvendor_id, dev->subdevice_id);

        int i = 0;
        while (SMIChipsets [i].token > 0) {
            if (dev->device_id == SMIChipsets [i].token) {
                // found a chipset supported by this driver.
                num_chipset = i;
                goto found;
            }

            i ++;
        }
    }

found:
    if (dev) {
        SMIPtr pSmi = &smi_rec;

        _MG_PRINTF ("NEWGAL>FBCON>SMI: found SMI chipset: %d, %s (id of fb is %s)\n",
                SMIChipsets [num_chipset].token, 
                SMIChipsets [num_chipset].name,
                fb_finfo->id);

        drv->init = SMI_Init;
        drv->cleanup = SMI_Cleanup;

        pSmi->PciInfo = dev;
        pSmi->Chipset = PCI_DEV_DEVICE_ID(pSmi->PciInfo);
        pSmi->ChipRev = PCI_DEV_REVISION(pSmi->PciInfo);

        pci_device_probe (dev);
    }
    else {
        _MG_PRINTF ("NEWGAL>FBCON>SMI: not found any SMI chipset.\n");
    }

    pci_iterator_destroy (iter);

    return dev ? 0 : -1;
}

#endif /* _MGHAVE_PCIACCESS */

