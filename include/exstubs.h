/**
 * \file exstubs.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2019/04/07
 *
 * \brief This file is the header for NEWGAL/IAL engines which should be
 *  implemented by an external module.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2007~2018, Beijing FMSoft Technologies Co., Ltd.

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
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef GUI_EXSTUBS_H
    #define GUI_EXSTUBS_H

/* The pixel format defined by depth */
#define COMMLCD_PSEUDO_RGB332   1
#define COMMLCD_TRUE_RGB555     2
#define COMMLCD_TRUE_RGB565     3
#define COMMLCD_TRUE_RGB888     4
#define COMMLCD_TRUE_RGB0888    5
#define COMMLCD_TRUE_ARGB1555   6
#define COMMLCD_TRUE_ARGB8888   7
#define COMMLCD_TRUE_ABRG8888   8

#define COMMLCD_UPDATE_NONE     0
#define COMMLCD_UPDATE_SYNC     1
#define COMMLCD_UPDATE_ASYNC    2

struct commlcd_info {
    int     height;         // vertical resolution of the screen
    int     width;          // horinzontal resolution of the screen
    int     pitch;          // Length of one scan line in bytes
    int     bpp;            // Depth (bits-per-pixel)
    int     type;           // Pixel type
    int     update_method;  // Update method: none, asynchronously, or synchronously.
    Uint8*  fb;             // Frame buffer
};

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

#define COMM_MOUSELBUTTON  0x01
#define COMM_MOUSERBUTTON  0x04

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MGGAL_DRI

#include <stdint.h>

/**
 * The struct type represents the DRI sub driver.
 * The concrete struct should be defined by the driver.
 */
struct _DriDriver;
typedef struct _DriDriver DriDriver;

/**
 * The struct type represents the bufffer can be used by
 * MiniGUI NEWGAL engine for hardware surface.
 */
typedef struct _DriSurfaceBuffer {
    uint32_t buff_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;

    uint32_t drm_format;
    //uint32_t depth:8;
    uint16_t bpp:8;
    uint16_t cpp:8;

    uint8_t* pixels;
} DriSurfaceBuffer;

/**
 * The color logic operations.
 */
enum DriColorLogicOp {
   COLOR_LOGICOP_CLEAR = 0,
   COLOR_LOGICOP_NOR = 1,
   COLOR_LOGICOP_AND_INVERTED = 2,
   COLOR_LOGICOP_COPY_INVERTED = 3,
   COLOR_LOGICOP_AND_REVERSE = 4,
   COLOR_LOGICOP_INVERT = 5,
   COLOR_LOGICOP_XOR = 6,
   COLOR_LOGICOP_NAND = 7,
   COLOR_LOGICOP_AND = 8,
   COLOR_LOGICOP_EQUIV = 9,
   COLOR_LOGICOP_NOOP = 10,
   COLOR_LOGICOP_OR_INVERTED = 11,
   COLOR_LOGICOP_COPY = 12,
   COLOR_LOGICOP_OR_REVERSE = 13,
   COLOR_LOGICOP_OR = 14,
   COLOR_LOGICOP_SET = 15
};

typedef struct _DriDriverOps {
    /**
     * This operator creates the DriDriver object.
     *
     * \note The driver must implement this operator.
     */
    DriDriver* (*create_driver) (int device_fd);

    /**
     * This operator destroies the DriDriver object.
     *
     * \note The driver must implement this operator.
     */
    void (*destroy_driver) (DriDriver *driver);

    /**
     * This operator flushs the batch buffer of the driver or the hardware cache.
     *
     * \note This operator can be NULL.
     */
    void (* flush_driver) (DriDriver *driver);

    /**
     * This operator creates a buffer with the specified pixel format,
     * width, and height. If succeed, a valid (not zero) buffer identifier
     * and the picth (row stride in bytes) will be returned.
     * If failed, it returns 0.
     *
     * \note The driver must implement this operator.
     */
    uint32_t (* create_buffer) (DriDriver *driver,
            uint32_t drm_format,
            unsigned int width, unsigned int height,
            unsigned int *pitch);

    BOOL (* fetch_buffer) (DriDriver *driver,
            uint32_t buffer_id,
            unsigned int *width, unsigned int *height,
            unsigned int *pitch);

    /**
     * This operator maps the buffer into the current process's virtual memory
     * space, and returns the virtual address. If failed, it returns NULL.
     *
     * \note The driver must implement this operator.
     */
    DriSurfaceBuffer* (* map_buffer) (DriDriver *driver,
            uint32_t buffer_id);

    /**
     * This operator un-maps a buffer.
     *
     * \note The driver must implement this operator.
     */
    void (* unmap_buffer) (DriDriver *driver, DriSurfaceBuffer* buffer);

    /**
     * This operator destroies a buffer.
     *
     * \note The driver must implement this operator.
     */
    void (* destroy_buffer) (DriDriver *driver, uint32_t buffer_id);

    /**
     * This operator clears the specific rectangle area of a buffer
     * with the specific pixel value. If succeed, it returns 0.
     *
     * \note If this operator is set as NULL, the driver does not support
     * hardware accelerated clear operation.
     */
    int (* clear_buffer) (DriDriver *driver,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* rc, uint32_t pixel_value);

    /**
     * This operator checks whether a hardware accelerated blit
     * can be done between the source buffer and the destination buffer.
     * If succeed, it returns 0.
     *
     * \note If this operator is set as NULL, it will be supposed that
     * the driver does not support any hardware accelerated blit operation.
     */
    int (* check_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, DriSurfaceBuffer* dst_buf);

    /**
     * This operator copies bits from a source buffer to a destination buffer.
     *
     * \note If this operator is set as NULL, the driver does not support
     * hardware accelerated copy blit.
     */
    int (* copy_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            enum DriColorLogicOp logic_op);

    /**
     * This operator blits pixles from a source buffer with the source alpha value
     * specified to a destination buffer.
     *
     * \note If this operator is set as NULL, the driver does not support
     * hardware accelerated blit with alpha.
     */
    int (* alpha_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc, uint8_t alpha);

    /**
     * This operator blits pixles from a source buffer to a destination buffer,
     * but skipping the pixel value specified by \a color_key.
     *
     * \note If this operator is set as NULL, the driver does not support
     * hardware accelerated blit with color key.
     */
    int (* key_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc, uint32_t color_key);

    /**
     * This operator blits pixles from a source buffer with the source alpha value
     * specified to a destination buffer, but skipping the pixel value specified.
     *
     * \note If this operator is set as NULL, the driver does not support
     * hardware accelerated blit with alpha and color key.
     */
    int (* alpha_key_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            uint8_t alpha, uint32_t color_key);

} DriDriverOps;

/* implement this stub to return the DRI driver operators */
DriDriverOps* __dri_ex_driver_get (const char* driver_name);

#endif /* _MGGAL_DRI */

/* external stubs for COMMLCD NEWGAL engine */
#ifdef _MGGAL_COMMLCD

int __commlcd_drv_init (void);
int __commlcd_drv_getinfo (struct commlcd_info *li,
        int width, int height, int depth);
int __commlcd_drv_release (void);
int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors);
int __commlcd_drv_update (const RECT* rc_dirty);

#endif /* _MGGAL_COMMLCD */

#ifdef _MGIAL_COMM

int __comminput_init (void);
int __comminput_ts_getdata (short *x, short *y, short *button);
int __comminput_kb_getdata (short *key, short *status);
int __comminput_wait_for_input (struct timeval *timeout);
void __comminput_deinit (void);

#endif /* _MGIAL_COMM */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_EXSTUBS_H */

