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

    Copyright (C) 2007~2020, Beijing FMSoft Technologies Co., Ltd.

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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

/*
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks,
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef GUI_EXSTUBS_H
    #define GUI_EXSTUBS_H

#include <stdint.h>

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

#ifdef _MGGAL_DRM

    /**
     * \defgroup external_stubs_dri External Stubs for DRI sub driver
     *
     * @{
     */

/**
 * The struct type represents the DRI sub driver.
 * The concrete struct should be defined by the driver.
 */
struct _DrmDriver;
typedef struct _DrmDriver DrmDriver;

/**
 * The structure type represents the buffer can be used by
 * MiniGUI NEWGAL DRM engine for a hardware surface.
 */
typedef struct _DrmSurfaceBuffer {
    /** The local handle of the buffer */
    uint32_t handle;

    /**
      * The prime file descriptor of the buffer.
      * It has a value >= 0 when this buffer has a prime file descriptor;
      * otherwise it has the value -1;
      */
    int prime_fd;

    /**
      * The global name of the buffer.
      * It has value > 0 when this buffer has a global name;
      * otherwise it has the value 0;
      */
    uint32_t name;

    /**
      * The frame buffer identifier if the buffer has been added as a
      * frame buffer to the system, otherwise it has the value 0.
     */
    uint32_t fb_id;

    /** The DRM format of the buffer. */
    uint32_t drm_format;
    /** The bits per pixel */
    uint32_t bpp:8;
    /** The bytes per pixel */
    uint32_t cpp:8;
    /** Is it a dumb buffer. Since 5.0.0. */
    uint32_t dumb:1;
    /** Is it a scanout buffer. Since 5.0.0. */
    uint32_t scanout:1;

    /** The width of the buffer. */
    uint32_t width;
    /** The height of the buffer. */
    uint32_t height;
    /** The pitch (row stride in bytes) of the buffer. */
    uint32_t pitch;

    /** The whole size in bytes of the buffer */
    size_t size;

    /**
      * The offset from the buffer start to the real pixel data in bytes.
      * It must be equal to or larger than the size of the buffer header.
      * Since 5.0.0.
      */
    off_t offset;

    /**
      * The mapped address of the buffer;
      * NULL when the buffer is not mapped yet.
      *
      * The address of the pixel data:
      *     uint8_t* pixels = this->buff + this->offset;
      */
    uint8_t* buff;
} DrmSurfaceBuffer;

/**
 * The structure type defines the operations for a DRM driver.
 */
typedef struct _DrmDriverOps {
    /**
     * This operation creates the DrmDriver object.
     *
     * \note The driver must implement this operation.
     */
    DrmDriver* (*create_driver) (int device_fd);

    /**
     * This operation destroys the DrmDriver object.
     *
     * \note The driver must implement this operation.
     */
    void (*destroy_driver) (DrmDriver *driver);

    /**
     * This operation flushes the batch buffer of the driver or
     * the hardware cache.
     *
     * \note This operation can be NULL.
     */
    void (* flush_driver) (DrmDriver *driver);

    /**
     * This operation creates a surface buffer with the specified pixel format,
     * header size, width, and height. If succeed, a valid DrmSurfaceBuffer
     * object will be returned; NULL on error. Note that the field of `buff`
     * of the DrmSurfaceBuffer object is NULL until the \a map_buffer was called.
     *
     * \note The driver must implement this operation and fill all fields of
     *  the new DrmSurfaceBuffer object.
     */
    DrmSurfaceBuffer* (* create_buffer) (DrmDriver *driver,
            uint32_t drm_format, uint32_t hdr_size,
            uint32_t width, uint32_t height);

    /**
     * This operation creates a buffer from a given and possibly foreign handle
     * with the size of the buffer. If succeed, a valid DrmSurfaceBuffer object
     * will be returned; NULL on error.
     *
     * \note This operation can be NULL. Note that the handle might was created
     *  by a foreign module. If implemented, the driver must
     *  fill the correct prime_fd, handle, name, and size fields of the new
     *  DrmSurfaceBuffer object.
     */
    DrmSurfaceBuffer* (* create_buffer_from_handle) (DrmDriver *driver,
            uint32_t handle, size_t size);

    /**
     * This operation creates a buffer for the given system global name
     * If succeed, a valid DrmSurfaceBuffer object will be returned;
     * NULL on error.
     *
     * \note This operation can be NULL. If implemented, the driver must
     *  fill the correct prime_fd, handle, name, and size fields of the new
     *  DrmSurfaceBuffer object.
     */
    DrmSurfaceBuffer* (* create_buffer_from_name) (DrmDriver *driver,
            uint32_t name);

    /**
     * This operation creates a buffer for the given PRIME file descriptor
     * with the specified pixel format, width, height, and pitch. If succeed,
     * a valid DrmSurfaceBuffer object will be returned; NULL on error.
     *
     * \note This operation can be NULL. If implemented, the driver must
     *  fill the correct prime_fd, handle, name, and size fields of the new
     *  DrmSurfaceBuffer object.
     */
    DrmSurfaceBuffer* (* create_buffer_from_prime_fd) (DrmDriver *driver,
            int prime_fd, size_t size);

    /**
     * This operation maps the buffer into the current process's virtual memory
     * space, and returns the virtual address. If failed, it returns NULL.
     *
     * When \a for_scanout is not zero, the buffer will be used for scan out
     * frame buffer.
     *
     * \note The driver must implement this operation. The driver must
     *  set a valid value for buff field of the DrmSurfaceBuffer object
     *  on success.
     */
    uint8_t* (* map_buffer) (DrmDriver *driver, DrmSurfaceBuffer* buffer,
            int for_scanout);

    /**
     * This operation un-maps a buffer.
     *
     * \note The driver must implement this operation. The driver must
     *  set NULL for buff field of the DrmSurfaceBuffer object
     *  on success.
     */
    void (* unmap_buffer) (DrmDriver *driver, DrmSurfaceBuffer* buffer);

    /**
     * This operation destroys a buffer.
     *
     * \note The driver must implement this operation.
     */
    void (* destroy_buffer) (DrmDriver *driver, DrmSurfaceBuffer* buffer);

    /**
     * This operation clears the specific rectangle area of a buffer
     * with the specific pixel value. If succeed, it returns 0.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated clear operation.
     */
    int (* clear_buffer) (DrmDriver *driver,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* rc, uint32_t pixel_value);

    /**
     * This operation checks whether a hardware accelerated blit
     * can be done between the source buffer and the destination buffer.
     * If succeed, it returns 0.
     *
     * \note If this operation is set as NULL, it will be supposed that
     * the driver does not support any hardware accelerated blitting operation.
     */
    int (* check_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, DrmSurfaceBuffer* dst_buf);

    /**
     * This operation copies bits from a source buffer to a destination buffer.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated copy blitting.
     *
     * \note Currently, the logical operation is ignored.
     */
    int (* copy_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            ColorLogicalOp logic_op);

    /**
     * This operation blits pixels from a source buffer with the source alpha
     * value specified to a destination buffer.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blitting with alpha.
     */
    int (* alpha_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            uint8_t alpha);

    /**
     * This operation blits pixels from a source buffer to a destination buffer,
     * but skipping the pixel value specified by \a color_key.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blitting with color key.
     */
    int (* key_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            uint32_t color_key);

    /**
     * This operation blits pixels from a source buffer with the source alpha
     * value specified to a destination buffer, but skipping the pixel value
     * specified.
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blitting with alpha and color key.
     */
    int (* alpha_key_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            uint8_t alpha, uint32_t color_key);

    /**
     * This operation blits pixels from a source buffer with the source alpha
     * value of pixels to the destination buffer, and with the specified color
     * compositing/blending method (\a ColorBlendMethod).
     *
     * \note If this operation is set as NULL, the driver does not support
     * hardware accelerated blitting with alpha on basis per pixel.
     *
     * \note Currently, the color compositing/blending method is ignored.
     */
    int (* alpha_pixel_blit) (DrmDriver *driver,
            DrmSurfaceBuffer* src_buf, const GAL_Rect* src_rc,
            DrmSurfaceBuffer* dst_buf, const GAL_Rect* dst_rc,
            ColorBlendMethod blend_method);

} DrmDriverOps;

/** The current version of DRM driver. */
#define DRM_DRIVER_VERSION  1

/**
 * Implement this stub to return the DRI driver operations
 *
 * This function takes three arguments and returns NULL or
 * a valid pointer of DrmDriverOps to MiniGUI.
 *
 * This function can return different DrmDriverOps to MiniGUI according to
 * the driver name and device file descriptor. In this way, your DRM engine
 * driver implementation can support multiple GPUs.
 *
 * If the external stub returns NULL, MiniGUI will try to use the dumb
 * frame buffer instead.
 *
 * \param driver_name This argument gives the driver name determined
 *  by MiniGUI. Generally, it is the kernel driver name for your GPU.
 *  For example, for Intel i915/i965 GPUs, the driver name will be `i915`.
 * \param device_fd This argument gives the file descriptor of the
 *  opened DRI device.
 * \param version A pointer to an integer which will be used to return
 *  the interface version of the DRM engine driver.
 *
 * \return NULL or a valid pointer to DrmDriverOps.
 *
 * \note We use the version control since 4.0.7. It will be initialized
 *  to zero by MiniGUI before calling this function. Because an old
 *  driver for MiniGUI 4.0.6 or earlier will not change the value,
 *  MiniGUI will deny to load the old driver.
 *
 *  The constant \a DRM_DRIVER_VERSION defines the current version code.
 */
DrmDriverOps* __drm_ex_driver_get (const char* driver_name, int device_fd,
        int* version);

    /** @} end of external_stubs_dri */

#endif /* _MGGAL_DRM */

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

