/**
 * \file coolbar.h
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
 * $Id: coolbar.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef EXT_COOLBAR_H
#define EXT_COOLBAR_H


#ifdef  __cplusplus
extern "C" {
#endif

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup mgext_ctrl_coolbar CoolBar control
     * @{
     */

/**
 * \def CTRL_COOLBAR
 * \brief The class name of coolbar control.
 */
#define CTRL_COOLBAR            ("CoolBar")

#define TYPE_BARITEM            1
#define TYPE_BMPITEM            2
#define TYPE_TEXTITEM           3

/** Structure of the coolbar item info */
typedef struct _COOLBARITEMINFO
{
    /** Reserved, do not use. */
    int insPos;

    /**
     * Identifier of the item. When the user clicked the item, this control
     * will send a notification message to the parent window
     * with the notification code to be equal to this identifier.
     */
    int id;

    /**
     * Type of the item, can be one of the following values:
     * - TYPE_BARITEM
     *   The item is a separator (a vertical line).
     * - TYPE_BMPITEM
     *   The item is a bitmap.
     * - TYPE_TEXTITEM
     *   The item is text.
     */
    int ItemType;

    /**
     * Pointer to the bitmap of the item, valid when the type is TYPE_BMPITEM.
     */
    PBITMAP Bmp;

    /**
     * Hint text of the item, will be displayed in the tooltip window.
     */
    const char *ItemHint;

    /**
     * Caption of the item, valid when the type is TPYE_TEXTITEM.
     */
    const char *Caption;

    /** Additional data of the item */
    DWORD dwAddData;
} COOLBARITEMINFO;

/**
 * \var typedef struct COOLBARITEMINFO *PCOOLBARITEMINFO;
 * \brief Data type of the pointer to a COOLBARITEMINFO.
 */
typedef COOLBARITEMINFO *PCOOLBARITEMINFO;

    /**
     * \defgroup mgext_ctrl_coolbar_styles Styles of coolbar control
     * @{
     */

/**
 * \def CBS_BMP_16X16
 * \brief The size of the item bitmap is 16x16.
 */
#define CBS_BMP_16X16           0x0000

/**
 * \def CBS_BMP_32X32
 * \brief The size of the item bitmap is 32x32.
 */
#define CBS_BMP_32X32           0x0001

/**
 * \def CBS_BMP_CUSTOM
 * \brief The item bitmap has customized size.
 *
 * \note For the control with this style, you should pass
 *       the width and the height of the item bitmap by 
 *       the argument \a dwAddData of \a CreateWindowEx function.
 *
 * \code
 * int item_width = 20;
 * int item_height = 20;
 *
 * CreateWindowEx (CTRL_COOLBAR, ..., MAKELONG (item_width, item_height)));
 * \endcode
 */
#define CBS_BMP_CUSTOM          0x0002

/**
 * \def CBS_USEBKBMP
 * \brief The control has a background bitmap.
 *
 * \note For a CoolBar control with this style, you should pass the bitmap file name
 *       by the argument \a spCaption of \a CreateWindowEx function.
 *
 * \code
 * const char* file_bkgnd = "res/my_bkgnd.gif";
 *
 * CreateWindowEx (CTRL_COOLBAR, file_bkgnd, ...);
 * \endcode
 */
#define CBS_USEBKBMP            0x0004

    /** @} end of mgext_ctrl_coolbar_styles */

    /**
     * \defgroup mgext_ctrl_coolbar_msgs Messages of coolbar control
     * @{
     */

/**
 * \def CBM_ADDITEM
 * \brief Adds a new item in a coolbar control.
 *
 * \code
 * CBM_ADDITEM
 * COOLBARITEMINFO *newIteminfo;
 *
 * wParam = 0;
 * lParam = (LPARAM)newIteminfo;
 * \endcode
 *
 * \param newIteminfo Pointer to the item info structure of the new item 
 *             to be added.
 *
 * \return Zero when success, otherwise less than 0;
 */
#define CBM_ADDITEM             0xFE00

/**
 * \def CBM_ENABLE
 * \brief Sets an item to be enabled or disabled.
 *
 * \code
 * CBM_ENABLE
 * int id;
 * BOOL enabled;
 *
 * wParam = (WPARAM)id;
 * lParam = (LPARAM)enabled;
 * \endcode
 *
 * \param id The identifier of the item to change.
 * \param enabled TRUE to enable the item, FALSE to disable the item.
 *
 * \return Zero when success, otherwise less than 0.
 */
#define CBM_ENABLE              0xFE01

    /** @} end of mgext_ctrl_coolbar_msgs */

    /** @} end of mgext_ctrl_coolbar */

    /** @} end of controls */
#ifdef  __cplusplus
}
#endif

#endif /* EXT_COOLBAR_H */

