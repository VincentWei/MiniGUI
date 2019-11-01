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

    /**
     * \defgroup external_stubs External Stubs
     *
     * The stubs (callbacks) you need to implement for external target.
     *
     * @{
     */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _MGGAL_DRI

#include <stdint.h>

    /**
     * \defgroup external_stubs_dri External Stubs for DRI sub driver
     *
     * @{
     */

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
    uint32_t handle;
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t drm_format;

    uint16_t bpp:8;
    uint16_t cpp:8;

    unsigned long size;
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

/**
 * The struct type defines the operations for a DRI driver.
 */
typedef struct _DriDriverOps {
    /**
     * This operation creates the DriDriver object.
     *
     * \note The driver must implement this operation.
     */
    DriDriver* (*create_driver) (int device_fd);

    /**
     * This operation destroies the DriDriver object.
     *
     * \note The driver must implement this operation.
     */
    void (*destroy_driver) (DriDriver *driver);

    /**
     * This operation flushs the batch buffer of the driver or the hardware cache.
     *
     * \note This operation can be NULL.
     */
    void (* flush_driver) (DriDriver *driver);

    /**
     * This operation creates a buffer with the specified pixel format,
     * width, and height. If succeed, a valid (not zero) buffer identifier,
     * the picth (row stride in bytes), and the handle will be returned.
     * If failed, it returns 0.
     *
     * \note The driver must implement this operation.
     */
    uint32_t (* create_buffer) (DriDriver *driver,
            uint32_t drm_format,
            unsigned int width, unsigned int height,
            unsigned int *pitch);

    /**
     * This operation creates a buffer for the given system global name
     * with the specified pixel format, width, and height. If succeed,
     * a valid (not zero) buffer identifier,
     * the picth (row stride in bytes), and the handle will be returned.
     * If failed, it returns 0.
     *
     * \note The driver must implement this operation.
     */
    uint32_t (* create_buffer_from_name) (DriDriver *driver,
            uint32_t name, uint32_t drm_format,
            unsigned int width, unsigned int height,
            unsigned int *pitch);

    BOOL (* fetch_buffer) (DriDriver *driver,
            uint32_t buffer_id,
            unsigned int *width, unsigned int *height,
            unsigned int *pitch);

    /**
     * This operation maps the buffer into the current process's virtual memory
     * space, and returns the virtual address. If failed, it returns NULL.
     *
     * \note The driver must implement this operation.
     */
    DriSurfaceBuffer* (* map_buffer) (DriDriver *driver,
            uint32_t buffer_id);

    /**
     * This operation un-maps a buffer.
     *
     * \note The driver must implement this operation.
     */
    void (* unmap_buffer) (DriDriver *driver, DriSurfaceBuffer* buffer);

    /**
     * This operation destroies a buffer.
     *
     * \note The driver must implement this operation.
     */
    void (* destroy_buffer) (DriDriver *driver, uint32_t buffer_id);

    /**
     * This operation clears the specific rectangle area of a buffer
     * with the specific pixel value. If succeed, it returns 0.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated clear operation.
     */
    int (* clear_buffer) (DriDriver *driver,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* rc, uint32_t pixel_value);

    /**
     * This operation checks whether a hardware accelerated blit
     * can be done between the source buffer and the destination buffer.
     * If succeed, it returns 0.
     *
     * \note If this operation is set as NULL, it will be supposed that
     * the driver does not support any hardware accelerated blit operation.
     */
    int (* check_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, DriSurfaceBuffer* dst_buf);

    /**
     * This operation copies bits from a source buffer to a destination buffer.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated copy blit.
     */
    int (* copy_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            enum DriColorLogicOp logic_op);

    /**
     * This operation blits pixles from a source buffer with the source alpha value
     * specified to a destination buffer.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blit with alpha.
     */
    int (* alpha_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc, uint8_t alpha);

    /**
     * This operation blits pixles from a source buffer to a destination buffer,
     * but skipping the pixel value specified by \a color_key.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blit with color key.
     */
    int (* key_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc, uint32_t color_key);

    /**
     * This operation blits pixles from a source buffer with the source alpha value
     * specified to a destination buffer, but skipping the pixel value specified.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blit with alpha and color key.
     */
    int (* alpha_key_blit) (DriDriver *driver,
            DriSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DriSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            uint8_t alpha, uint32_t color_key);

} DriDriverOps;

/** Implement this stub to return the DRI driver operations */
DriDriverOps* __dri_ex_driver_get (const char* driver_name);

    /** @} end of external_stubs_dri */

#endif /* _MGGAL_DRI */

#ifdef _MGGAL_COMMLCD

    /**
     * \defgroup external_stubs_commlcd External Stubs for CommLCD NEWGAL Engine
     *
     * The external stubs for COMMLCD NEWGAL engine.
     *
     * @{
     */

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

/**
 * The structue represented the screen information of CommmLCD engine.
 */
struct commlcd_info {
    /** The vertical resolution of the screen. */
    int     height;

    /** The horizontal resolution of the screen. */
    int     width;

    /** The length of one scan line in bytes. */
    int     pitch;

    /** The color depth (bits-per-pixel). */
    int     bpp;

    /**
     * The pixel type, can be one of the following values:
     *  - COMMLCD_PSEUDO_RGB332
     *  - COMMLCD_TRUE_RGB555
     *  - COMMLCD_TRUE_RGB565
     *  - COMMLCD_TRUE_RGB888
     *  - COMMLCD_TRUE_RGB0888
     *  - COMMLCD_TRUE_ARGB1555
     *  - COMMLCD_TRUE_ARGB8888
     *  - COMMLCD_TRUE_ABRG8888
     */
    int     type;

    /**
     * The update method, can be one of the following values:
     *  - COMMLCD_UPDATE_NONE\n
     *      No update method.
     *  - COMMLCD_UPDATE_SYNC
     *      The update method should be called synchronously.
     *  - COMMLCD_UPDATE_ASYNC
     *      The update method should be called asynchronously.
     */
    int     update_method;

    /**
     * The pointer to the frame buffer of the screen.
     */
    Uint8*  fb;
};

/**
 * Implement this stub to initialize the CommLCD engine.
 * Return 0 for success.
 */
int __commlcd_drv_init (void);

/**
 * Implement this stub to initialize the CommLCD engine.
 *
 * \param li The pointer to struct commlcd_info. This structure
 *      will be used to return the LCD surface information.
 * \param width The desired horizontal resolution of the screen.
 * \param height The desired vertical resolution of the screen.
 * \param depth The desired depth of the screen.
 *
 * Return 0 for success, otherwise for failure.
 */
int __commlcd_drv_getinfo (struct commlcd_info *li,
        int width, int height, int depth);

/**
 * Implement this stub to release (destroy) the CommLCD engine.
 *
 * Return 0 for success, otherwise for failure.
 */
int __commlcd_drv_release (void);

/**
 * Implement this stub to set the pallete when the pixel type
 * is COMMLCD_PSEUDO_RGB332.
 *
 * \param firstcolor
 *          The index value of the first entry of the pallete.
 * \param ncolors
 *          The number of entries of the pallete.
 * \param colors
 *          The array of the colors for each entries in the pallete.
 *
 * Return 0 for success, otherwise for failure.
 */
int __commlcd_drv_setclut (int firstcolor, int ncolors, GAL_Color *colors);

/**
 * Implement this stub to update the screen.
 *
 * \param rc_dirty The rectangle contains the dirty region.
 *
 * Return 0 for success, otherwise for failure.
 */
int __commlcd_drv_update (const RECT* rc_dirty);

    /** @} end of external_stubs_commlcd */

#endif /* _MGGAL_COMMLCD */

#ifdef _MGIAL_COMM

    /**
     * \defgroup external_stubs_commial External Stubs for COMM IAL Engine
     *
     * The external stubs for COMM IAL engine
     *
     * @{
     */

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

#define COMM_MOUSELBUTTON  0x01
#define COMM_MOUSERBUTTON  0x04

/**
 * Implement this stub to initialize the COMM IAL engine.
 * Return 0 for success, otherwise for failure.
 */
int __comminput_init (void);

/**
 * Implement this stub to return the mouse event data.
 * The implementation should return the position of mouse or touch,
 * and the button status through the pointers \a x, \a y, and \a button.
 * The position should be in absolute coordinates, and the button
 * will have \a COMM_MOUSELBUTTON set if left button was pressed,
 * and \a COMM_MOUSERBUTTON set if right button was pressed.
 *
 * Return 0 for success, otherwise for failure.
 */
int __comminput_ts_getdata (short *x, short *y, short *button);

/**
 * Implement this stub to return the keyboard event data.
 * The implementation should return the key scancode pressed or released
 * through the pointer \a key, and the pressed or released status of the
 * key through \a status.
 *
 * Return 0 for success, otherwise for failure.
 */
int __comminput_kb_getdata (short *key, short *status);

/**
 * Implement this stub to wait for an event.
 *
 * \retval 0
 *      The timeout period expires.
 * \retval COMM_MOUSEINPUT
 *      There is an mouse moved, button pressed, or button released event.
 * \retval COMM_KBINPUT
 *      There is a key pressed or released event.
 * \retval others
 *      Failed.
 */
int __comminput_wait_for_input (struct timeval *timeout);

/**
 * Implement this stub to de-initialize the COMM IAL engine.
 */
void __comminput_deinit (void);

    /** @} end of external_stubs_commial */

#endif /* _MGIAL_COMM */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

    /** @} end of external_stubs */

#endif /* GUI_EXSTUBS_H */

