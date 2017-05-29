/**
 * \file spinbox.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 \verbatim

    Copyright (C) 2002-2008 Feynman Software
    Copyright (C) 1998-2002 Wei Yongming

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: spinbox.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef EXT_SPINBOX_H
#define EXT_SPINBOX_H

#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_spinbox SpinBox control
     * @{
     */

/**
 * \def CTRL_SPINBOX
 * \brief The class name of spinbox control.
 */
#define CTRL_SPINBOX                ("SpinBox")

/** Structure of the spinbox info */
typedef struct _SPININFO
{
    /** Maximum position value */
    int max;
    /** Minimum position value */
    int min;
    /** Current position value */
    int cur;
} SPININFO;
/**
 * \var typedef SPININFO *PSPININFO;
 * \brief Data type of the pointer to a SPININFO.
 */
typedef SPININFO *PSPININFO;

/**
 * \fn void GetSpinBoxSize (DWORD dwStyle, int* w, int* h)
 * \brief Gets size of a vertical spin box control.
 *
 * The spin box control in MiniGUI has fixed size. 
 * This function gets the size of a spin box control.
 *
 * \param dwStyle The style of the spin box.
 * \param w The width of the spin box control will be returned through this argument.
 * \param h The height of the spin box control will be returned through this argument.
 */
void GetSpinBoxSize (DWORD dwStyle, int* w, int* h);

    /**
     * \defgroup mgext_ctrl_spinbox_styles Styles of spinbox control
     * @{
     */

/**
 * \def SPS_AUTOSCROLL
 * \brief The spinbox control can automatically scroll, 
 *        and disable itself when reach maximal or minimal value.
 */
#define SPS_AUTOSCROLL          0x00000001L

/**
 * \def SPS_HORIZONTAL
 * \brief The spinbox control is horizontal. The default is vertical.
 */
#define SPS_HORIZONTAL          0x00000010L

/**
 * \def SPS_TYPE_NORMAL
 * \brief The spinbox control contains the up and left arrows. 
 */
#define SPS_TYPE_NORMAL         0x00000000L

/**
 * \def SPS_TYPE_UPARROW
 * \brief The spinbox control contains only the up/left arrow. 
 */
#define SPS_TYPE_UPARROW        0x00000100L

/**
 * \def SPS_TYPE_DOWNARROW
 * \brief The spinbox control contains only the down/right arrow. 
 */
#define SPS_TYPE_DOWNARROW      0x00000200L

/**
 * \def SPS_TYPE_MASK
 * \brief The mask code of spinbox control.
 */ 
#define SPS_TYPE_MASK           0x00000F00L

    /** @} end of mgext_ctrl_spinbox_styles */

    /**
     * \defgroup mgext_ctrl_spinbox_msgs Messages of spinbox control
     * @{
     */
/**
 * \def KS_SPINPOST
 * \brief The flag of the spinbox control message.
 */
#define KS_SPINPOST             0x00010000

/**
 * \def SPM_SETTARGET
 * \brief Sets the target window of the spinbox.
 *
 * When the user click the up/left or down/right arrow of the spin box, it will
 * emulate the down and up of the key SCANCODE_CURSORBLOCKUP or
 * SCANCODE_CURSORBLOCKDOWN, and post MSG_KEYDOWN and MSG_KEYUP
 * message to the target window. Note that the shifit key status of the
 * message will have the flag KS_SPINPOST set.
 *
 * \code
 * SPM_SETTARGET
 * HWND hTarget;
 *
 * wParam = 0;
 * lParam = (LPARAM)hTarget;
 * \endcode
 *
 * \param hTarget Handle of the target window.
 * \return Always be zero.
 */
#define SPM_SETTARGET           0xF300

/**
 * \def SPM_GETTARGET
 * \brief Gets the target window of the spinbox.
 *
 * \code
 * SPM_GETTARGET
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The handle to the target window.
 */
#define SPM_GETTARGET           0xF301

/**
 * \def SPM_SETINFO
 * \brief Sets the parameter information of the spinbox.
 *
 * \code
 * SPM_SETINFO
 * PSPININFO newinfo;
 *
 * wParam = 0;
 * lParam = (LPARAM)newinfo;
 * \endcode
 *
 * \param newinfo Pointer to the SPININFO structure storing the new spinbox info.
 *
 * \return Zero on success, -1 to indicate invalid parameter.
 *
 * \sa SPININFO
 */
#define SPM_SETINFO             0xF302

/**
 * \def SPM_GETINFO
 * \brief Gets the parameter infos of the spinbox.
 *
 * \code
 * SPM_GETINFO
 * PSPININFO info;
 *
 * wParam = 0;
 * lParam = (LPARAM)info;
 * \endcode
 *
 * \param info Pointer to the SPININFO structure retreiving the spinbox info.
 *
 * \return Zero on success, -1 to indicate invalid parameter.
 *
 * \sa SPININFO
 */
#define SPM_GETINFO             0xF303

/**
 * \def SPM_DISABLEUP
 * \brief Disable the ability to scroll up.
 *
 * \code
 * SPM_DISABLEUP
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define SPM_DISABLEUP           0xF304

/**
 * \def SPM_DISABLEDOWN
 * \brief Disable the ability to scroll down.
 *
 * \code
 * SPM_DISABLEDOWN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define SPM_DISABLEDOWN         0xF305

/**
 * \def SPM_ENABLEUP
 * \brief Enable the ability to scroll up.
 *
 * \code
 * SPM_ENABLEUP
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define SPM_ENABLEUP            0xF306

/**
 * \def SPM_ENABLEDOWN
 * \brief Enable the ability to scroll down.
 *
 * \code
 * SPM_ENABLEDOWN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be zero.
 */
#define SPM_ENABLEDOWN          0xF307

/**
 * \def SPM_SETCUR
 * \brief Sets the current position in the range of the spinbox.
 *
 * \code
 * SPM_SETCUR
 * int pos;
 *
 * wParam = (WPARAM)pos;
 * lParam = 0;
 * \endcode
 *
 * \param pos The current position to set.
 *
 * \return Zero on success, -1 to indicate invalid parameter.
 */
#define SPM_SETCUR              0xF308

/**
 * \def SPM_GETCUR
 * \brief Gets the current position in the range of the spinbox.
 *
 * \code
 * SPM_GETCUR
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The current position in the range of the spinbox.
 */
#define SPM_GETCUR              0xF309

    /** @} end of mgext_ctrl_spinbox_msgs */

    /**
     * \defgroup mgext_ctrl_spinbox_ncs Notification codes of spinbox control
     * @{
     */

/**
 * \def SPN_REACHMIN
 * \brief Notifies that the spin box has reached the minimum limit.
 */
#define SPN_REACHMIN            1

/**
 * \def SPN_REACHMAX
 * \brief Notifies that the spin box has reached the maximum limit.
 */
#define SPN_REACHMAX            2

    /** @} end of mgext_ctrl_spinbox_ncs */

    /** @} end of mgext_ctrl_spinbox */

    /** @} end of controls */

#ifdef  __cplusplus
}
#endif

#endif /* EXT_SPINBOX_H */

