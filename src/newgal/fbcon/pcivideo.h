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

#ifndef _GAL_pcivideo_h
#define _GAL_pcivideo_h

typedef struct pci_device *pciVideoPtr;

#define PCI_DEV_VENDOR_ID(_pcidev) ((_pcidev)->vendor_id)
#define PCI_DEV_DEVICE_ID(_pcidev) ((_pcidev)->device_id)
#define PCI_DEV_REVISION(_pcidev)  ((_pcidev)->revision)

#define PCI_SUB_VENDOR_ID(_pcidev) ((_pcidev)->subvendor_id)
#define PCI_SUB_DEVICE_ID(_pcidev) ((_pcidev)->subdevice_id)

/* pci-rework functions take a 'pci_device' parameter instead of a tag */
#define PCI_DEV_TAG(_pcidev)        (_pcidev)

/* PCI_DEV macros, typically used in printf's, add domain ? XXX */
#define PCI_DEV_BUS(_pcidev)       ((_pcidev)->bus)
#define PCI_DEV_DEV(_pcidev)       ((_pcidev)->dev)
#define PCI_DEV_FUNC(_pcidev)      ((_pcidev)->func)

/* pci-rework functions take a 'pci_device' parameter instead of a tag */
#define PCI_CFG_TAG(_pcidev)        (_pcidev)

/* PCI_CFG macros, typically used in DRI init, contain the domain */
#define PCI_CFG_BUS(_pcidev)      (((_pcidev)->domain << 8) | \
                                    (_pcidev)->bus)
#define PCI_CFG_DEV(_pcidev)       ((_pcidev)->dev)
#define PCI_CFG_FUNC(_pcidev)      ((_pcidev)->func)

#define PCI_REGION_BASE(_pcidev, _b, _type) ((_pcidev)->regions[(_b)].base_addr)
#define PCI_REGION_SIZE(_pcidev, _b)        ((_pcidev)->regions[(_b)].size)

#define PCI_READ_BYTE(_pcidev, _value_ptr, _offset) \
    pci_device_cfg_read_u8((_pcidev), (_value_ptr), (_offset))

#define PCI_READ_LONG(_pcidev, _value_ptr, _offset) \
    pci_device_cfg_read_u32((_pcidev), (_value_ptr), (_offset))

#define PCI_WRITE_LONG(_pcidev, _value, _offset) \
    pci_device_cfg_write_u32((_pcidev), (_value), (_offset))

typedef struct _PCI_VIDEO_DRIVER {
    const char* driver_name;
    int (*probe) (_THIS, const struct fb_fix_screeninfo*, struct _PCI_VIDEO_DRIVER*);
    int (*init) (_THIS, const GAL_Surface* current);
    int (*cleanup) (_THIS);
    void* drv_data;
} PCI_VIDEO_DRIVER;

/* defined in pci_smi.c */
int SMI_Probe (_THIS, const struct fb_fix_screeninfo* fb_finfo, PCI_VIDEO_DRIVER* driver);

#endif /* _GAL_pcivideo_h */

