/**
 * \file newtoolbar.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim

    Copyright (C) 2002-2008 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    All rights reserved by Feynman Software.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

 \endverbatim
 */

/*
 * $Id: newtoolbar.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_NEWTOOLBAR_H
#define _MGUI_CTRL_NEWTOOLBAR_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_newtoolbar NewToolBar control
     *
     * \note You should pass information of the control through 
     * the argument \a dwAddData of \a CreateWindowEx function.
     *
     * \code
     * NTBINFO ntb_info;
     *
     * CreateWindowEx (CTRL_TOOLBAR, ..., (DWORD) &ntb_info);
     * \endcode
     *
     * \sa NTBINFO
     *
     * @{
     */

/**
 * \def CTRL_NEWTOOLBAR
 * \brief The class name of newtoolbar control.
 */
#define CTRL_NEWTOOLBAR            ("newtoolbar")

/** Structure of the newtoolbar control information */
typedef struct _NTBINFO
{
    /** Images for displaying tool bar item. */
    PBITMAP image;

    /** Number of bitmap cells in the image. */
    int nr_cells;

    /**
     * Number of cell columns in the image.
     * It should be one of the following values:
     *  - 0, 4\n
     *  Four rows, the cells have normal, highlight, pushed, and disabled states.
     *  - 1\n
     *  The cells have only normal state.
     *  - 2\n
     *  The cells have normal and highlight states.
     *  - 3\n
     *  The cells have normal, highlight, and pushed states.
     */
    int nr_cols;

    /**
     * Width of bitmap cell. If w_cell is zero, it will be 
     * equal to (width_of_image / nr_cols).
     */
    int w_cell;

    /**
     * Height of bitmap cell. If h_cell is zero, it will be 
     * equal to (height_of_image / nr_cells).
     */
    int h_cell;
    
} NTBINFO;

/**
 * \var typedef void (* HOTSPOTPROC)(HWND hwnd, int id, const RECT* cell, int x, int y)
 * \brief Type of the hotspot-clicked callback procedure.
 *
 * \param hwnd  The handle to the control.
 * \param id    The identifier of the button in the NewToolBar control.
 * \param cell  The rectangle of the button in screen coordinates.
 * \param x     The x-coordinate of the mouse down point.
 * \param y     THe y-coordinate of the mouse down point.
 */
typedef void (* HOTSPOTPROC)(HWND hwnd, int id, const RECT* cell, int x, int y);

/* Internal */
#define NTB_TEXT_LEN        15
/* Internal */
#define NTB_TIP_LEN         255

#define NTBIF_PUSHBUTTON    0x0001
#define NTBIF_HOTSPOTBUTTON  0x0002
#define NTBIF_SEPARATOR     0x0003
#define NTBIF_CHECKBUTTON   0x0005
#define NTBIF_NEWLINE       0x0008
#define NTBIF_TYPEMASK      0x000F

/**
 * \def NTBIF_DISABLED
 * \brief The item is disabled.
 */
#define NTBIF_DISABLED      0x0010
/**
 * \def NTBIF_CHECKED
 * \brief The item is checked.
 */
#define NTBIF_CHECKED       0x0020

#define MTB_WHICH_FLAGS     0x0001
#define MTB_WHICH_ID        0x0002
#define MTB_WHICH_TEXT      0x0004
#define MTB_WHICH_TIP       0x0008
#define MTB_WHICH_CELL      0x0010
#define MTB_WHICH_HOTSPOT   0x0020
#define MTB_WHICH_ADDDATA   0x0040

/** Structure of the new toolbar item info */
typedef struct _NTBITEMINFO
{
    /**
     * Which fields are valid when sets/retrives the item information 
     * (ignored when add item). It can be OR'd with the following values:
     *
     *  - MTB_WHICH_FLAGS\n     The flags is valid.
     *  - MTB_WHICH_ID\n        The identifier is valid.
     *  - MTB_WHICH_TEXT\n      The text is valid.
     *  - MTB_WHICH_TIP\n       The tip text is valid.
     *  - MTB_WHICH_CELL\n      The bitmap cell index is valid.
     *  - MTB_WHICH_HOTSPOT\n   The hotspot rect is valid.
     *  - MTB_WHICH_ADDDATA\n   The additional data is valid.
     */
    DWORD       which;

    /**
     * Flags of the item. It can be OR'd with the following values:
     *
     *  - NTBIF_PUSHBUTTON\n    The item is a normal push button.
     *  - NTBIF_CHECKBUTTON\n   The item is a normal check button.
     *  - NTBIF_NEWLINE\n       The item is a newline tag.
     *  - NTBIF_HOTSPOTBUTTON\n The item is a menu button with hotspot.
     *  - NTBIF_SEPARATOR\n     The item is a separator.
     *  - NTBIF_DISABLED\n      The item is disabled.
     */
    DWORD       flags;

    /**
     * Identifier of the item. When the user clicked the item, this control
     * will send a notification message to the parent window
     * with the notification code to be equal to this identifier.
     */
    int         id;

    /**
     * Text of the item. This text will be displayed under the bitmap
     * if the control have NTBS_WITHTEXT style.
     */
    char*       text;

    /** Tooltip of the item, not used, reserved. */
    char*       tip;

    /** Index of bitmap cell. */
    int         bmp_cell;

    /** Hotpot-clicked callback procedure for menubutton. */
    HOTSPOTPROC  hotspot_proc;

    /** Rectangle region of hotspot in the cell. */
    RECT        rc_hotspot;

    /** Additional data of the item */
    DWORD       add_data;
} NTBITEMINFO;

/**
 * \var PNTBITEMINFO
 * \brief Data type of pointer to a NTBITEMINFO;
 */
typedef NTBITEMINFO* PNTBITEMINFO;

    /**
     * \defgroup ctrl_newtoolbar_styles Styles of NewToolBar control
     * @{
     */

/**
 * \def NTBS_WITHTEXT
 * \brief Displays text with the item bitmap. 
 */
#define NTBS_WITHTEXT               0x000000001L

/**
 * \def NTBS_TEXTRIGHT
 * \brief Displays text at the right side of the item bitmap. 
 */
#define NTBS_TEXTRIGHT              0x000000002L

/**
 * \def NTBS_DRAWSTATES
 * \brief Draws the button states with 3D frame, and does not
 *        use the highlight, pushed and disabled bitmap cell.
 */
#define NTBS_DRAWSTATES             0x000000004L

/**
 * \def NTBS_DRAWSEPARATOR
 * \brief Draws the separator bar.
 */
#define NTBS_DRAWSEPARATOR          0x000000008L

/**
 * \def NTBS_HORIZONTAL
 * \brief The NewToolbar will be displayed horizontally.
 */
#define NTBS_HORIZONTAL             0x00000000L

/**
 * \def NTBS_VERTICAL
 * \brief The NewToolbar will be displayed vertically.
 */
#define NTBS_VERTICAL               0x00000010L

/**
 *
 * \def NTBS_MULTLINE
 * \brief The Newtoolbar item will be displayed in several lines.
 * In this style , your should define the toolbar RECT in CreateWindwEx.
 */
#define NTBS_MULTLINE               0x00000020L

    /** @} end of ctrl_newtoolbar_styles */

    /**
     * \defgroup ctrl_newtoolbar_msgs Messages of NewToolBar control
     * @{
     */
/** Newtoolbar return value */
#define NTB_OKAY        0
/** Newtoolbar return value */
#define NTB_ERR         (-1)
/** Newtoolbar return value */
#define NTB_ERR_SPACE   (-2)
/** Newtoolbar return value */
#define NTB_ERR_DUPID   (-3)

/**
 * \def NTBM_ADDITEM
 * \brief Adds an item to a newtoolbar.
 *
 * \code
 * NTBM_ADDITEM
 * NTBITEMINFO *ntbii;
 *
 * wParam = 0;
 * lParam = (LPARAM)ntbii;
 * \endcode
 *
 * \param ntbii Pointer to the data storing the newtoobar item info.
 *
 * \return NTB_OKAY on success, else one of the following values:
 *      - NTB_ERR_SPACE\n No enongh space to allocate memory for new item.
 *      - NTB_ERR_DUPID\n Duplicated identifier with an existed item.
 */
#define NTBM_ADDITEM            0xFE00

/**
 * \def NTBM_GETITEM
 * \brief Retrives the information of an item in a newtoolbar control.
 *
 * \code
 * NTBM_GETITEM
 * int id;
 * NTBITEMINFO *ntbii;
 *
 * wParam = id;
 * lParam = (LPARAM)ntbii;
 * \endcode
 *
 * \param id The identifier of the item.
 * \param ntbii Pointer to the data storing the newtoobar item info.
 *
 * \return NTB_OKAY on success, else NTB_ERR.
 */
#define NTBM_GETITEM            0xFE01

/**
 * \def NTBM_SETITEM
 * \brief Sets the information of an item in a newtoolbar control.
 *
 * \code
 * NTBM_SETITEM
 * int id;
 * NTBITEMINFO ntbii;
 *
 * wParam = id;
 * lParam = (LPARAM)&ntbii;
 * \endcode
 *
 * \param id The identifier of the item.
 * \param ntbii The structure for storing the newtoobar item info.
 *
 * \return NTB_OKAY on success, else NTB_ERR.
 */
#define NTBM_SETITEM            0xFE02

/**
 * \def NTBM_ENABLEITEM
 * \brief Enables/Disables an item in a newtoolbar control.
 *
 * \code
 * NTBM_ENABLEITEM
 * int id;
 * BOOL enable;
 *
 * wParam = id;
 * lParam = enable;
 * \endcode
 *
 * \param id The identifier of the item.
 * \param enable True to enable item; false to disable item.
 *
 * \return NTB_OKAY on success, else NTB_ERR.
 */
#define NTBM_ENABLEITEM         0xFE03

/**
 * \def NTBM_SETBITMAP
 * \brief Sets the bitmap of  a newtoolbar control.
 *
 * \code
 * NTBM_SETBITMAP
 * NTBINFO ntbi;
 *
 * wParam = 0;
 * lParam = (LPARAM)&ntbi;
 * \endcode
 *
 * \param ntbi  The structure for storing the newtoobar info.
 *
 * \return NTB_OKAY on success, else NTB_ERR.
 */
#define NTBM_SETBITMAP          0xFE04


#define NTBM_MSGMAX             0xFE10

    /** @} end of ctrl_newtoolbar_msgs */

    /** @} end of ctrl_newtoolbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_NEWTOOLBAR_H */

