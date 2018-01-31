/**
 * \file static.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2001/12/29
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
 * $Id: static.h 12453 2010-01-12 03:51:55Z dongkai $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_STATIC_H
#define _MGUI_CTRL_STATIC_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_static Static control
     * @{
     */

/**
 * \def CTRL_STATIC
 * \brief The class name of static control.
 */
#define CTRL_STATIC         ("static")

    /**
     * \defgroup ctrl_static_styles Styles of static control
     * @{
     */

/**
 * \def SS_SIMPLE
 * \brief Designates a simple rectangle and displays a single line 
 *        of text flush-left in the rectangle.
 */
#define SS_SIMPLE           0x00000000L

/**
 * \def SS_BLACKRECT
 * \brief Specifies a rectangle filled with the black color.
 */
#define SS_BLACKRECT        0x00000001L

/**
 * \def SS_GRAYRECT
 * \brief Specifies a rectangle filled with the light gray color.
 */
#define SS_GRAYRECT         0x00000002L

/**
 * \def SS_WHITERECT
 * \brief Specifies a rectangle filled with the light white color.
 */
#define SS_WHITERECT        0x00000003L

/**
 * \def SS_BLACKFRAME
 * \brief Specifies a box with a frame drawn with the black color.
 */
#define SS_BLACKFRAME       0x00000004L

/**
 * \def SS_GRAYFRAME
 * \brief Specifies a box with a frame drawn with the light gray color.
 */
#define SS_GRAYFRAME        0x00000005L

/**
 * \def SS_WHITEFRAME
 * \brief Specifies a box with a frame drawn with the light white color.
 */
#define SS_WHITEFRAME       0x00000006L

/**
 * \def SS_GROUPBOX
 * \brief Creates a rectangle in which other controls can be grouped.
 */
#define SS_GROUPBOX         0x00000007L

/**
 * \def SS_ICON
 * \brief Designates an icon displayed in the static control.
 */
#define SS_ICON             0x00000008L

/**
 * \def SS_BITMAP
 * \brief Specifies that a bitmap will be displayed in the static control.
 */
#define SS_BITMAP           0x00000009L

/* Reserved */
#define SS_OWNERDRAW        0x0000000AL

/* Reserved */
#define SS_ENHMETAFILE      0x0000000BL

/** 
 * \def SS_TYPEMASK
 * \brief The mask code of the static control.
 */ 
#define SS_TYPEMASK         0x0000000FL

/**
 * \def SS_LEFT
 * \brief Displays the given text flush-left.
 */
#define SS_LEFT             0x00000010L

/**
 * \def SS_CENTER
 * \brief Displays the given text centered in the rectangle. 
 */
#define SS_CENTER           0x00000020L

/**
 * \def SS_RIGHT
 * \brief Displays the given text flush-right.
 */
#define SS_RIGHT            0x00000030L

/** 
 * \def SS_ALIGNMASK
 * \brief The mask code of the static control.
 */ 

#define SS_ALIGNMASK        0x000000F0L

/**
 * \def SS_NOPREFIX
 * \brief Prevents interpretation of any ampersand (&) characters in 
 *        the control's text as accelerator prefix characters.
 *
 * \note Not implemented so far.
 */
#define SS_NOPREFIX         0x00000100L

/**
 * \def SS_CENTERIMAGE
 * \brief Puts the image in the center of the static control.
 *        Default is top-left aligned.
 */
#define SS_CENTERIMAGE      0x00000200L

/**
 * \def SS_NOWORDWRAP
 * \brief Designates a simple rectangle and displays the given text 
 *        in single line in the rectangle. 
 *
 * Tabs are expanded, but words are not wrapped. 
 * Text that extends past the rectagnel is clipped.
 */
#define SS_NOWORDWRAP       0x00000400L

/**
 * \def SS_LEFTNOWORDWRAP
 * \brief Backward compatibility definition.
 *
 * \sa SS_NOWORDWRAP
 */
#define SS_LEFTNOWORDWRAP   SS_NOWORDWRAP

/**
 * \def SS_REALSIZEIMAGE
 * \brief Does not scale the image.
 */
#define SS_REALSIZEIMAGE    0x00000800L

/**
 * \def SS_NOTIFY
 * \brief Sends the parent window notification messages when the user 
 *        clicks or double-clicks the control.
 */
#define SS_NOTIFY           0x00001000L

    /** @} end of ctrl_static_styles */
    
    /**
     * \defgroup ctrl_static_msgs Messages of static control
     * @{
     */
/* Reserved */
#define STM_SETICON         0xF170
/* Reserved */    
#define STM_GETICON         0xF171

/**
 * \def STM_SETIMAGE
 * \brief Associates a new image (icon or bitmap) with a static control.
 *
 * An application sends an STM_SETIMAGE message to 
 * associate a new image (icon or bitmap) with a static control.
 *
 * \code
 * STM_SETIMAGE
 * HICON image;
 *  or 
 * BITMAP* image;
 *
 * wParam = (WPARAM)image;
 * lParam = 0;
 * \endcode
 * 
 * \param image The handle to an icon if the type of static control type 
 *        is SS_ICON, or the pointer to a BITMAP object if the type is SS_BITMAP.
 *
 * \return The old image (handle or pointer).
 */
#define STM_SETIMAGE        0xF172

/**
 * \def STM_GETIMAGE
 * \brief Retrieves a handle to the image.
 *
 * An application sends an STM_GETIMAGE message to retrieve a handle 
 * to the image associated with a static control.
 *
 * \code
 * STM_GETIMAGE
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The handle to the icon if the type of static control type is SS_ICON, 
 *         or the pointer to the BITMAP object if the type is SS_BITMAP.
 */
#define STM_GETIMAGE        0xF173

#define STM_MSGMAX          0xF174

    /** @} end of ctrl_static_msgs */

    /**
     * \defgroup ctrl_static_ncs Notification codes of static control
     * @{
     */

/**
 * \def STN_DBLCLK
 * \brief Notifies a double-click.
 *
 * The STN_DBLCLK notification message is sent when 
 * the user double-clicks a static control that has the SS_NOTIFY style.
 */
#define STN_DBLCLK          1

/* Not use */
#define STN_ENABLE          2
/* Not use */    
#define STN_DISABLE         3

/**
 * \def STN_CLICKED
 * \brief Notifies that a static control is clicked.
 *
 * The STN_CLICKED notification message is sent 
 * when the user clicks a static control that has the SS_NOTIFY style.
 */
#define STN_CLICKED         4

    /** @} end of ctrl_static_ncs */

    /** @} end of ctrl_static */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_STATIC_H */

