/**
 * \file animation.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

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
 * $Id: animation.h 10865 2008-08-27 06:52:22Z wangjian $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef EXT_ANIMATION_H
#define EXT_ANIMATION_H


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_animation ANIMATION control and animation GIF (GIF87a/GIF89a) support
     * @{
     */

/** Animation frame structure. */
typedef struct _ANIMATIONFRAME
{
    /** The disposal method (from GIF89a specification):
     *  Indicates the way in which the graphic is to be treated after being displayed.
     *  - 0\n No disposal specified. The decoder is not required to take any action.
     *  - 1\n Do not dispose. The graphic is to be left in place. 
     *  - 2\n Restore to background color. The area used by the frame must be restored to 
     *        the background color.
     *  - 3\n Restore to previous. The decoder is required to restore the area overwritten by 
     *        the frmae with what was there prior to rendering the frame.
     */
    int disposal;
    /** The x-coordinate of top-left corner of the frame in whole animation screen. */
    int off_x;
    /** The y-coordinate of top-left corner of the frame in whole animation screen. */
    int off_y;
    /** The width of the frame. */
    unsigned int width;
    /** The height of the frame. */
    unsigned int height;

    /** The time of the frame will be display, in the unit of animation time_unit. */
    unsigned int delay_time;
    /** The memdc compatible with the gif image. */
    HDC mem_dc;
    /** The bits of the mem_dc, should be freed after deleting the mem_dc. */
    Uint8* bits;

    /** The next frame */
    struct _ANIMATIONFRAME* next;
    /** The previous frame */
    struct _ANIMATIONFRAME* prev;
} ANIMATIONFRAME;

/** Animation structure */
typedef struct _ANIMATION
{
    /** The width of the animation. */
    unsigned int width;
    /** The height of the animation. */
    unsigned int height;

    /** The background color */
    RGB bk;

    /** The number of all frames. */
    int nr_frames;
    /**
     * The unit of the time will be used count the delay time of every frame.
     * The default is 1, equal to 10ms.
     */
    int time_unit;
    /** Pointer to the animation frame.*/
    ANIMATIONFRAME* frames;
} ANIMATION;

/**
 * \fn ANIMATION* CreateAnimationFromGIF89a (HDC hdc, MG_RWops* area)
 * \brief Creates an ANIMATION obeject from a GIF 89a data source.
 *
 * This function load a GIF 89a graphic from the data source \a area,
 * and create an ANIMATION object from the GIF 89a data.
 *
 * \param hdc The dc will be used to create BITMAP object for the animation frame.
 * \param area The data source.
 *
 * \return This function returns an ANIMATION object when success, otherwise NULL.
 *
 * \sa DestroyAnimation, ANIMATION
 */
ANIMATION* CreateAnimationFromGIF89a (HDC hdc, MG_RWops* area);

/**
 * \fn ANIMATION* CreateAnimationFromGIF89aFile (HDC hdc, const char* file)
 * \brief Creates an ANIMATION obeject from a GIF 89a file.
 *
 * This function load a GIF 89a graphic from the file \a file,
 * and create an ANIMATION object.
 *
 * \param hdc The dc will be used to create BITMAP object for the animation frame.
 * \param file The file name.
 *
 * \return This function returns an ANIMATION object when success, otherwise NULL.
 *
 * \sa DestroyAnimation, ANIMATION
 */
MG_EXPORT ANIMATION* CreateAnimationFromGIF89aFile (HDC hdc, const char* file);

/**
 * \fn ANIMATION* CreateAnimationFromGIF89aMem (HDC hdc, const void* mem, int size)
 * \brief Creates an ANIMATION obeject from a GIF 89a memory data.
 *
 * This function load a GIF 89a graphic from the memory \a mem which
 * is \a size long and create an ANIMATION object from the GIF 89a data.
 *
 * \param hdc The dc will be used to create BITMAP object for the animation frame.
 * \param mem The pointer to the memory.
 * \param size The size of the memory.
 *
 * \return This function returns an ANIMATION object when success, otherwise NULL.
 *
 * \sa DestroyAnimation, ANIMATION
 */
MG_EXPORT ANIMATION* CreateAnimationFromGIF89aMem (HDC hdc, const void* mem, int size);

/**
 * \fn void DestroyAnimation (ANIMATION* anim, BOOL free_it)
 * \brief Destories an ANIMATION object.
 *
 * This function destroies the ANIMATION object \a anim, and
 * free it if \a free_it is TRUE.
 *
 * \param anim Pointer to the ANIMATION object.
 * \param free_it Specify whether free the object by calling \a free(3).
 *
 * \sa CreateAnimationFromGIF89a, ANIMATION
 */
MG_EXPORT void DestroyAnimation (ANIMATION* anim, BOOL free_it);

/** Control class name of ANIMATION control. */
#define CTRL_ANIMATION                  ("Animation")

/** Return values of ANIMATION control. */
#define ANIMATION_OKAY                  0
/** Return values of ANIMATION control. */
#define ANIMATION_ERR                   1

    /**
     * \defgroup mgext_ctrl_animation_styles Styles of ANIMATION control
     *
     * This control can be used to play the animation object, and
     * you should specify the animation object when you create the
     * control by using the dwAddData:
     *
     * \code
     *
     *  HWND hwnd;
     *  ANIMATION* anim = CreateAnimationFromGIF89aFile (HDC_SCREEN, "banner.gif");
     *
     *  if (anim == NULL)
     *      goto error;
     *
     *  hwnd = CreateWindow (CTRL_ANIMATION,
     *                    "",
     *                    WS_VISIBLE | ANS_AUTOLOOP,
     *                    100,
     *                    10, 10, 300, 200, hWnd, (DWORD)anim);
     *  SendMessage (hwnd, ANM_STARTPLAY, 0, 0);
     *
     * \endcode
     *
     * @{
     */

/**
 * \def ANS_AUTOLOOP
 * \brief Loop playing the animation automatically.
 */
#define ANS_AUTOLOOP                    0x0001L

/**
 * \def ANS_SCALED
 * \brief Scale the animation to the control size.
 */
#define ANS_SCALED                      0x0002L

/**
 * \def ANS_FITTOANI
 * \brief Resize the control to fit the animation.
 */
#define ANS_FITTOANI                    0x0004L

/**
 * \def ANS_WINBGC 
 * \brief Use the background color of the window.
 */
#define ANS_WINBGC                      0x0010L

    /** @} end of mgext_ctrl_animation_styles */

    /**
     ** \defgroup mgext_control_animation_msgs Messages of ANIMATION control
     * @{
     */

/**
 * \def ANM_SETANIMATION
 * \brief Sets the animation object for the control.
 *
 * An application can send ANM_SETANIMATION to set the animation object of a control.
 *
 * \code
 * ANM_SETANIMATION
 * ANIMATION* anim;
 *
 * wParam = 0;
 * lParam = (LPARAM)anim;
 * \endcode
 *
 * \return The old animation object.
 */
#define ANM_SETANIMATION                0xF110

/**
 * \def ANM_GETANIMATION
 * \brief Gets the animation object of the control.
 *
 * An application can send ANM_GETANIMATION to retrive the animation object of a control.
 *
 * \code
 * ANM_GETANIMATION
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current animation object of the control.
 */
#define ANM_GETANIMATION                0xF111

/**
 * \def ANM_STARTPLAY
 * \brief Indicates the control to start playing the animation.
 *
 * The animation will not be played when create the control, 
 * an application should send ANM_STARTPLAY to an animation control
 * to start playing the animation.
 *
 * \code
 * ANM_STARTPLAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define ANM_STARTPLAY                   0xF112

/**
 * \def ANM_PAUSE_RESUME
 * \brief Indicates the control to pause/resume playing the animation.
 *
 * An application can send ANM_PAUSE_RESUME to an animation control
 * to pause/resume playing the animation.
 *
 * \code
 * ANM_PAUSE_RESUME
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define ANM_PAUSE_RESUME                0xF113

/**
 * \def ANM_STOPPLAY
 * \brief Indicates the control to stop playing the animation.
 *
 * An application can send ANM_STOPPLAY to an animation control
 * to stop playing the animation. The control will display the
 * first frame of the animation.
 *
 * \code
 * ANM_STOPPLAY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define ANM_STOPPLAY                    0xF114

#define ANM_MSGMAX                      0xF120

    /** @} end of mgext_ctrl_animation_msgs */

    /**
     * \defgroup mgext_ctrl_animation_ncs Notification code of ANIMATION control
     * @{
     */
/**
 * \def ANNC_CLICKED
 * \brief Indicates the user has clicked the control
 */
#define ANNC_CLICKED         1

/**
 * \def ANNC_DBLCLK
 * \brief Indicates the user has double clicked the control
 */
#define ANNC_DBLCLK          2

/**
 * \def ANNC_NOFRAME
 * \brief Indicates that there is no frame to play.
 */
#define ANNC_NOFRAME         3

    /** @} end of mgext_ctrl_animation_ncs */

    /** @} end of mgext_ctrl_animation */

    /** @} end of controls */

#ifdef  __cplusplus
}
#endif

#endif /* EXT_ANIMATION_H */

