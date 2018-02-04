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
*/

#ifndef __PCI_SMI_MMIO_H__
#define __PCI_SMI_MMIO_H__

/******************************************************************************/
/*            M A C R O S                          */
/******************************************************************************/

#ifdef _DEBUG
#    define ENTER_PROC(PROCNAME)    _MG_PRINTF ("ENTER\t" PROCNAME \
                                "(%d)\n", __LINE__)
#    define DEBUG_PROC(PROCNAME)    _MG_PRINTF ("DEBUG\t" PROCNAME \
                                "(%d)\n", __LINE__)
#    define LEAVE_PROC(PROCNAME)    _MG_PRINTF ("LEAVE\t" PROCNAME \
                                "(%d)\n", __LINE__)
#else
#    define ENTER_PROC(PROCNAME)
#    define DEBUG_PROC(PROCNAME)
#    define LEAVE_PROC(PROCNAME)
#endif

enum region_type {
    REGION_MEM,
    REGION_IO 
};

typedef struct {
    /* accel additions */
    CARD32          AccelCmd;    /* Value for DPR0C */
    CARD32          Stride;        /* Stride of frame buffer */
    CARD32          ScissorsLeft;    /* Left/top of current scissors */
    CARD32          ScissorsRight;    /* Right/bottom of current scissors */
    BOOL            ClipTurnedOn;    /* Clipping was turned on by the previous command */
    CARD8           SR18Value;    /* PDR#521: original SR18 value */
    CARD8           SR21Value;    /* PDR#521: original SR21 value */

    /* int             vgaCRIndex, vgaCRReg; */
    int             width, height;  /* Width and height of the screen */
    int             Bpp;            /* Bytes per pixel */
    int             depth;          /* Bits per pixel */
    
    pciVideoPtr     PciInfo;    /* PCI info vars */
    int             Chipset;    /* Chip info, set using PCI above */
    int             ChipRev;

    int             videoRAMBytes;
    int             videoRAMKBytes;

    unsigned char * MapBase;    /* Base of mapped memory */
    int             MapSize;    /* Size of mapped memory */
    CARD8 *         DPRBase;    /* Base of DPR registers */
    CARD8 *         VPRBase;    /* Base of VPR registers */
    CARD8 *         CPRBase;    /* Base of CPR registers */
    CARD8 *         FPRBase;    /* Base of FPR registers - for 0730 chipset */
    CARD8 *         DataPortBase;    /* Base of data port */
    int             DataPortSize;    /* Size of data port */
    CARD8 *         IOBase;        /* Base of MMIO VGA ports */
    IOADDRESS       PIOBase;    /* Base of I/O ports */
    unsigned char * FBBase;        /* Base of FB */
    CARD32          FBOffset;    /* Current visual FB starting location */
    CARD32          FBCursorOffset;    /* Cursor storage location */
    CARD32          FBReserved;    /* Reserved memory in frame buffer */

    CARD32          fbMapOffset;    /* offset for fb mapping */

    unsigned long   memPhysBase;
    unsigned long   fbOffset;

    BOOL            NeedToSync;

    int             GEResetCnt;    /* Limit the number of errors printed using a counter */
    BOOL            NoPCIRetry;    /* Disable PCI retries */

    /* Shadow frame buffer (rotation) */
    BOOL            shadowFB;    /* Flag if shadow buffer is used */
    int             rotate;        /* Rotation flags */

    _THIS;
} SMIRec, *SMIPtr;

#define SET_SYNC_FLAG(pSmi)     (pSmi)->NeedToSync = TRUE

#define SMI_LYNX_SERIES(chip)        ((chip & 0xF0F0) == 0x0010)
#define SMI_LYNX3D_SERIES(chip)        ((chip & 0xF0F0) == 0x0020)
#define SMI_COUGAR_SERIES(chip) ((chip & 0xF0F0) == 0x0030)
#define SMI_LYNXEM_SERIES(chip) ((chip & 0xFFF0) == 0x0710)
#define SMI_LYNXM_SERIES(chip)        ((chip & 0xFF00) == 0x0700)

/* Chip tags */
#define PCI_SMI_VENDOR_ID        PCI_VENDOR_SMI
#define SMI_UNKNOWN                        0
#define SMI_LYNX                PCI_CHIP_SMI910
#define SMI_LYNXE                PCI_CHIP_SMI810
#define SMI_LYNX3D                PCI_CHIP_SMI820
#define SMI_LYNXEM                PCI_CHIP_SMI710
#define SMI_LYNXEMplus                PCI_CHIP_SMI712
#define SMI_LYNX3DM                PCI_CHIP_SMI720
#define SMI_COUGAR3DR           PCI_CHIP_SMI731

/* I/O Functions */
static __inline__ CARD8
VGAIN8_INDEX(SMIPtr pSmi, int indexPort, int dataPort, CARD8 index)
{
    if (pSmi->IOBase) {
        MMIO_OUT8(pSmi->IOBase, indexPort, index);
        return(MMIO_IN8(pSmi->IOBase, dataPort));
    } else {
        outb(pSmi->PIOBase + indexPort, index);
        return(inb(pSmi->PIOBase + dataPort));
    }
}

static __inline__ void
VGAOUT8_INDEX(SMIPtr pSmi, int indexPort, int dataPort, CARD8 index, CARD8 data)
{
    if (pSmi->IOBase) {
        MMIO_OUT8(pSmi->IOBase, indexPort, index);
        MMIO_OUT8(pSmi->IOBase, dataPort, data);
    } else {
        outb(pSmi->PIOBase + indexPort, index);
        outb(pSmi->PIOBase + dataPort, data);
    }
}

static __inline__ CARD8
VGAIN8(SMIPtr pSmi, int port)
{
    if (pSmi->IOBase) {
        return(MMIO_IN8(pSmi->IOBase, port));
    } else {
        return(inb(pSmi->PIOBase + port));
    }
}

static __inline__ void
VGAOUT8(SMIPtr pSmi, int port, CARD8 data)
{
    if (pSmi->IOBase) {
        MMIO_OUT8(pSmi->IOBase, port, data);
    } else {
        outb(pSmi->PIOBase + port, data);
    }
}

#define OUT_SEQ(pSmi, index, data)        \
        VGAOUT8_INDEX((pSmi), VGA_SEQ_INDEX, VGA_SEQ_DATA, (index), (data))
#define IN_SEQ(pSmi, index)                        \
        VGAIN8_INDEX((pSmi), VGA_SEQ_INDEX, VGA_SEQ_DATA, (index))

#define WRITE_DPR(pSmi, dpr, data)        MMIO_OUT32(pSmi->DPRBase, dpr, data); 
#define READ_DPR(pSmi, dpr)               MMIO_IN32(pSmi->DPRBase, dpr)
#define WRITE_VPR(pSmi, vpr, data)        MMIO_OUT32(pSmi->VPRBase, vpr, data);
#define READ_VPR(pSmi, vpr)               MMIO_IN32(pSmi->VPRBase, vpr)
#define WRITE_CPR(pSmi, cpr, data)        MMIO_OUT32(pSmi->CPRBase, cpr, data);
#define READ_CPR(pSmi, cpr)               MMIO_IN32(pSmi->CPRBase, cpr)
#define WRITE_FPR(pSmi, fpr, data)        MMIO_OUT32(pSmi->FPRBase, fpr, data);
#define READ_FPR(pSmi, fpr)               MMIO_IN32(pSmi->FPRBase, fpr)

/* 2D Engine commands */
#define SMI_TRANSPARENT_SRC     0x00000100
#define SMI_TRANSPARENT_DEST    0x00000300

#define SMI_OPAQUE_PXL          0x00000000
#define SMI_TRANSPARENT_PXL     0x00000400

#define SMI_MONO_PACK_8         0x00001000
#define SMI_MONO_PACK_16        0x00002000
#define SMI_MONO_PACK_32        0x00003000

#define SMI_ROP2_SRC            0x00008000
#define SMI_ROP2_PAT            0x0000C000
#define SMI_ROP3                0x00000000

#define SMI_BITBLT              0x00000000
#define SMI_RECT_FILL           0x00010000
#define SMI_TRAPEZOID_FILL      0x00030000
#define SMI_SHORT_STROKE        0x00060000
#define SMI_BRESENHAM_LINE      0x00070000
#define SMI_HOSTBLT_WRITE       0x00080000
#define SMI_HOSTBLT_READ        0x00090000
#define SMI_ROTATE_BLT          0x000B0000

#define SMI_SRC_COLOR           0x00000000
#define SMI_SRC_MONOCHROME      0x00400000

#define SMI_GRAPHICS_STRETCH    0x00800000

#define SMI_ROTATE_CW           0x01000000
#define SMI_ROTATE_CCW          0x02000000

#define SMI_MAJOR_X             0x00000000
#define SMI_MAJOR_Y             0x04000000

#define SMI_LEFT_TO_RIGHT       0x00000000
#define SMI_RIGHT_TO_LEFT       0x08000000

#define SMI_COLOR_PATTERN       0x40000000
#define SMI_MONO_PATTERN        0x00000000

#define SMI_QUICK_START         0x10000000
#define SMI_START_ENGINE        0x80000000

#define MAXLOOP 0x100000        /* timeout value for engine waits */

#define ENGINE_IDLE()                \
        ((VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x16) & 0x08) == 0)
#define FIFO_EMPTY()                \
        ((VGAIN8_INDEX(pSmi, VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x16) & 0x10) != 0)

/* Wait until "v" queue entries are free */
#define        WaitQueue(v)          \
    do {                   \
        if (pSmi->NoPCIRetry) {          \
            int loop = MAXLOOP; mem_barrier();          \
            while (!FIFO_EMPTY())           \
                if (loop-- == 0) break;          \
            if (loop <= 0) SMI_GEReset(1, __LINE__, __FILE__);  \
        }                   \
    } while (0)

/* Wait until GP is idle */
#define WaitIdle()  \
    do {                  \
        int loop = MAXLOOP; mem_barrier(); \
        while (!ENGINE_IDLE())                   \
            if (loop-- == 0) break;           \
        if (loop <= 0) SMI_GEReset(1, __LINE__, __FILE__);   \
    } while (0)

/* Wait until GP is idle and queue is empty */
#define        WaitIdleEmpty()           \
    do {                   \
        WaitQueue(MAXFIFO);           \
        WaitIdle();                 \
    } while (0)

#define RGB8_PSEUDO      (-1)
#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2

/* register defines so we're not hardcoding numbers */

#define FPR00                                   0x0000

/* video window formats - I=indexed, P=packed */
#define FPR00_FMT_8I                            0x0
#define FPR00_FMT_15P                           0x1
#define FPR00_FMT_16P                           0x2
#define FPR00_FMT_32P                           0x3
#define FPR00_FMT_24P                           0x4
#define FPR00_FMT_8P                            0x5
#define FPR00_FMT_YUV422                        0x6
#define FPR00_FMT_YUV420                        0x7

/* possible bit definitions for FPR00 - VWI = Video Window 1 */
#define FPR00_VWIENABLE                         0x00000008
#define FPR00_VWITILE                           0x00000010
#define FPR00_VWIFILTER2                        0x00000020
#define FPR00_VWIFILTER4                        0x00000040
#define FPR00_VWIKEYENABLE                      0x00000080
#define FPR00_VWIGDF_SHIFT                      16
#define FPR00_VWIGDENABLE                       0x00080000
#define FPR00_VWIGDTILE                         0x00100000

#define FPR00_MASKBITS                          0x0000FFFF

#define FPR04                                   0x0004
#define FPR08                                   0x0008
#define FPR0C                                   0x000C
#define FPR10                                   0x0010
#define FPR14                                   0x0014
#define FPR18                                   0x0018
#define FPR1C                                   0x001C
#define FPR20                                   0x0020
#define FPR24                                   0x0024
#define FPR58                                   0x0058
#define FPR5C                                   0x005C
#define FPR68                                   0x0068
#define FPRB0                                   0x00B0
#define FPRB4                                   0x00B4
#define FPRC4                                   0x00C4
#define FPRCC                                   0x00CC

#define FPR158                      0x0158
#define FPR158_MASK_MAXBITS         0x07FF
#define FPR158_MASK_BOUNDARY        0x0800
#define FPR15C                      0x015C
#define FPR15C_MASK_HWCCOLORS       0x0000FFFF
#define FPR15C_MASK_HWCADDREN       0xFFFF0000
#define FPR15C_MASK_HWCENABLE       0x80000000

/* panel sizes returned by the bios */

#define PANEL_640x480   0x00
#define PANEL_800x600   0x01
#define PANEL_1024x768  0x02
#define PANEL_1280x1024 0x03
#define PANEL_1600x1200 0x04
#define PANEL_1400x1050 0x0A

#if !defined (MetroLink) && !defined (VertDebug)
#define VerticalRetraceWait()                        \
do                                    \
{                                    \
    if (VGAIN8_INDEX(pSmi, vgaCRIndex, vgaCRData, 0x17) & 0x80)        \
    {                                    \
    while ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x00);    \
    while ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x08);    \
    while ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x00);    \
    }                                    \
} while (0)
#else
#define SPIN_LIMIT 1000000
#define VerticalRetraceWait()                        \
do                                    \
{                                    \
    if (VGAIN8_INDEX(pSmi, vgaCRIndex, vgaCRData, 0x17) & 0x80)        \
    {                                    \
    volatile unsigned long _spin_me;                \
    for (_spin_me = SPIN_LIMIT;                    \
         ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x00) &&     \
         _spin_me;                            \
         _spin_me--);                        \
    if (!_spin_me)                            \
        ErrorF("smi: warning: VerticalRetraceWait timed out.\n");    \
    for (_spin_me = SPIN_LIMIT;                    \
         ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x08) &&     \
         _spin_me;                            \
         _spin_me--);                        \
    if (!_spin_me)                            \
        ErrorF("smi: warning: VerticalRetraceWait timed out.\n");    \
    for (_spin_me = SPIN_LIMIT;                    \
         ((VGAIN8(pSmi, vgaIOBase + 0x0A) & 0x08) == 0x00) &&     \
         _spin_me;                            \
         _spin_me--);                        \
    if (!_spin_me)                            \
        ErrorF("smi: warning: VerticalRetraceWait timed out.\n");    \
    }                                \
} while (0)
#endif

#endif /* !__PCI_SMI_MMIO_H__ */

