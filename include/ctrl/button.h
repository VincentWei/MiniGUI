/**
 * \file button.h
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
 * $Id: button.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_BUTTON_H
#define _MGUI_CTRL_BUTTON_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_button Button control
     * @{
     */

/**
 * \def BTN_WIDTH_BORDER
 * \brief The width of button border.
 */
#define BTN_WIDTH_BORDER    4

/**
 * \def CTRL_BUTTON
 * \brief The class name of button control.
 */
#define CTRL_BUTTON         ("button")

    /**
     * \defgroup ctrl_button_styles Styles of button control
     * @{
     */

/**
 * \def BS_PUSHBUTTON
 * \brief Creates a push button that is a pane that displays
 * either a piece of text or an image and when it is pressed it performs an
 * action.
 */
#define BS_PUSHBUTTON       0x00000000L

/**
 * \def BS_DEFPUSHBUTTON
 * \brief Creates a push button that behaves like a BS_PUSHBUTTON style button.
 
 * Creates a push button that behaves like a BS_PUSHBUTTON style button, 
 * but also has a heavy black border.  If the button is in a dialog box, 
 * the user can select the button by pressing the enter key, 
 * even when the button does not have the input focus. 
 * This style is useful for enabling the user to quickly select 
 * the most likely (default) option.
 */
#define BS_DEFPUSHBUTTON    0x00000001L

/**
 * \def BS_CHECKBOX
 * \brief Creates a check button is a button that can be either selected 
 * or deselected, and its selection is independent of the selections of 
 * any other buttons.
 *
 * By default, the text is displayed to the right of the check box. 
 * To display the text to the left of the check box, combine this flag 
 * with the BS_LEFTTEXT style (or with the equivalent BS_RIGHTBUTTON style).
 */
#define BS_CHECKBOX         0x00000002L

/**
 * \def BS_AUTOCHECKBOX
 * \brief Creates a button that is almost the same as a check box.
 *
 * Creates a button that is the same as a check box, 
 * except that the check state automatically toggles between 
 * checked and unchecked each time the user selects the check box.
 */
#define BS_AUTOCHECKBOX     0x00000003L

/**
 * \def BS_RADIOBUTTON
 * \brief Creates a radio button that can be either selected
 * or deselected, but when selecting it any other buttons in 
 * its group will be cleared. 
 *
 * By default, the text is displayed to the right of the circle. 
 * To display the text to the left of the circle, combine this flag 
 * with the BS_LEFTTEXT style (or with the equivalent BS_RIGHTBUTTON style). 
 * Use radio buttons for groups of related, but mutually exclusive choices.
 */
#define BS_RADIOBUTTON      0x00000004L

/**
 * \def BS_AUTORADIOBUTTON
 * \brief Creates a button that is almost the same as a radio button.
 * 
 * Creates a button that is the same as a radio button, 
 * except that when the user selects it, The system automatically 
 * sets the button's check state to checked
 * and automatically sets the check state for all other buttons 
 * in the same group to unchecked.
 */
#define BS_AUTORADIOBUTTON  0x00000005L

/**
 * \def BS_3STATE
 * \brief Creates a button that is almost the same as a check box.
 * 
 * Creates a button that is the same as a check box, except 
 * that the box can be grayed as well as checked or unchecked.
 * Use the grayed state to show that the state of the check box 
 * is not determined.
 */
#define BS_3STATE           0x00000006L

/**
 * \def BS_AUTO3STATE
 * \brief Creates a button that is almost the same as a three-state check box.
 *
 * Creates a button that is the same as a three-state check box, 
 * except that the box changes its state when the user selects it.
 * The state cycles through checked, grayed, and unchecked.
 */
#define BS_AUTO3STATE       0x00000007L
/* Not use */
#define BS_GROUPBOX         0x00000008L
/* Not use */
#define BS_USERBUTTON       0x00000009L


/**
 * \def BS_OWNERDRAW
 * \brief Creates an owner-drawn button.
 *
 * \note Not implemented so far.
 */
#define BS_OWNERDRAW        0x0000000AL

/**
 * \def BS_TYPEMASK
 * \brief The type mask of style of button.
 */
#define BS_TYPEMASK         0x0000000FL

/**
 * \def BS_TEXT
 * \brief Specifies that the button displays text.
 */
#define BS_TEXT             0x00000000L

/**
 * \def BS_LEFTTEXT
 * \brief Places text on the left side.
 *
 * Places text on the left side of the radio button 
 * or check box when combined with a radio button or check box style.
 */
#define BS_LEFTTEXT         0x00000020L

/**
 * \def BS_ICON
 * \brief Specifies that the button displays an icon.
 */
#define BS_ICON             0x00000040L

/**
 * \def BS_BITMAP
 * \brief Specifies that the button displays a bitmap.
 */
#define BS_BITMAP           0x00000080L
/**
 * \def BS_CONTENTMASK
 * \brief The content mask of style of button.
 */
#define BS_CONTENTMASK      0x000000F0L

/**
 * \def BS_LEFT
 * \brief Left-justifies the text in the button rectangle.
 *
 * However, if the button is a check box or radio button that 
 * does not have the BS_RIGHTBUTTON style, the text is left 
 * justified on the right side of the check box or radio button.
 */
#define BS_LEFT             0x00000100L

/**
 * \def BS_RIGHT
 * \brief Right-justifies text in the button rectangle.
 * 
 * However, if the button is a check box or radio button that 
 * does not have the BS_RIGHTBUTTON style, the text is 
 * right justified on the right side of the check box or radio button.
 */
#define BS_RIGHT            0x00000200L

/**
 * \def BS_CENTER
 * \brief Centers text horizontally in the button rectangle.
 */
#define BS_CENTER           0x00000300L

/**
 * \def BS_TOP
 * \brief Places text at the top of the button rectangle.
 */
#define BS_TOP              0x00000400L

/**
 * \def BS_BOTTOM
 * \brief Places text at the bottom of the button rectangle.
 */
#define BS_BOTTOM           0x00000800L

/**
 * \def BS_VCENTER
 * \brief Places text in the middle (vertically) of the button rectangle.
 */
#define BS_VCENTER          0x00000C00L

/**
 * \def BS_REALSIZEIMAGE
 * \brief Does not scale the image.
 */
#define BS_REALSIZEIMAGE    0x00000F00L

/**
 * \def BS_ALIGNMASK
 * \brief The alignment mask of style of button.
 */
#define BS_ALIGNMASK        0x00000F00L

/**
 * \def BS_PUSHLIKE
 * \brief Makes a button look and act like a push button.
 *
 * Makes a button (such as a check box, three-state check box, or radio button) 
 * look and act like a push button. The button looks raised when it isn't 
 * pushed or checked, and sunken when it is pushed or checked.
 */
#define BS_PUSHLIKE         0x00001000L

/**
 * \def BS_MULTLINE
 * \brief Wraps the button text to multiple lines.
 *
 * Wraps the button text to multiple lines if the text string is 
 * too long to fit on a single line in the button rectangle.
 */
#define BS_MULTLINE         0x00002000L

/**
 * \def BS_NOTIFY
 * \brief Enables a button to send notification messages to its parent window.
 */
#define BS_NOTIFY           0x00004000L

/**
 * \def BS_CHECKED
 * \brief Makes a button checked initially.
 */
#define BS_CHECKED          0x00004000L

/**
 * \def BS_FLAT
 * \brief The flat style of button.
 */
#define BS_FLAT             0x00008000L

/**
 * \def BS_NOBORDER
 * \brief The no border style of button. 
 */
#define BS_NOBORDER         0x00010000L

/**
 * \def BS_RIGHTBUTTON
 * \brief The right style of button.
 * \sa BS_LEFTTEXT
 */
#define BS_RIGHTBUTTON      BS_LEFTTEXT

    /** @} end of ctrl_button_styles */

    /**
     * \defgroup ctrl_button_states States of button control
     * @{
     */

/**
 * \def BST_POSE_MASK
 * \brief The pose(normal, hilite, pushed and disable) mask of 
 *        status of button.
 */
#define BST_POSE_MASK        0x0003

/**
 * \def BST_NORMAL
 * \brief Specifies the normal state.
 */
#define BST_NORMAL      0x0000

/**
 * \def BST_HILITE
 * \brief Specifies the hilite state.
 */
#define BST_HILITE      0x0001

/**
 * \def BST_PUSHED
 * \brief Specifies the pushed state.
 */
#define BST_PUSHED      0x0002

/**
 * \def BST_DISABLE
 * \brief Specifies the disable state.
 */
#define BST_DISABLE     0x0003


/**
 * \def BST_CHECK_MASK
 * \brief The check(unchecked, checked and indeterminate) mask of 
 *        status of button.
 */
#define BST_CHECK_MASK      0x000c

/**
 * \def BST_UNCHECKED
 * \brief Indicates the button is unchecked.
 */
#define BST_UNCHECKED       0x0000

/**
 * \def BST_CHECKED
 * \brief Indicates the button is checked.
 */
#define BST_CHECKED         0x0004

/**
 * \def BST_INDETERMINATE
 * \brief Indicates the button is grayed because 
 * the state of the button is indeterminate.
 */
#define BST_INDETERMINATE   0x0008

/**
 * \def BST_FOCUS
 * \brief Specifies the focus state.
 */
#define BST_FOCUS           0x0010

    /** @} end of ctrl_button_states */

    /**
     * \defgroup ctrl_button_msgs Messages of button control
     * @{
     */

/**
 * \def BM_GETCHECK
 * \brief Retrieves the check state of a radio button or check box.
 *
 * An application sends a BM_GETCHECK message to retrieve 
 * the check state of a radio button or check box.
 *
 * \code
 * BM_GETCHECK
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \returns An integer indicates whether the button is checked.
 *
 * \retval BST_UNCHECKED The button is not checked.
 * \retval BST_CHECKED The button is checked.
 * \retval BST_INDETERMINATE The button is grayed because the state of the button is indeterminate.
 *
 * \sa ctrl_button_states
 */
#define BM_GETCHECK             0xF0F0

/**
 * \def BM_SETCHECK
 * \brief Sets the check state of a radio button or check box.
 *
 * An application sends a BM_SETCHECK message to set 
 * the check state of a radio button or check box.
 *
 * \code
 * BM_SETCHECK
 * int check_state;
 *
 * wParam = (WPARAM)check_state;
 * lParam = 0;
 * \endcode
 *
 * \param check_state The check state of button, can be one of the following values:
 *      - BST_UNCHECKED\n
 *        Want the button to be unchecked.
 *      - BST_CHECKED\n
 *        Want the button to be checked.
 *      - BST_INDETERMINATE\n
 *        Want the button to be grayed if it is a three states button.
 * \returns The old button state.
 */
#define BM_SETCHECK             0xF0F1

/**
 * \def BM_GETSTATE
 * \brief Gets the state of a button or check box.
 *
 * An application sends a BM_GETSTATE message to 
 * determine the state of a button or check box.
 *
 * \code
 * BM_GETSTATE
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \returns An integer indicates the button state.
 *
 * \sa ctrl_button_states
 */
#define BM_GETSTATE             0xF0F2

/**
 * \def BM_SETSTATE
 * \brief Sets the state of a button.
 *
 * An application sends a BM_SETSTATE message to set the state of a 
 * button.
 *
 * \code
 * BM_SETSTATE
 * int push_state;
 *
 * wParam = (WPARAM)push_state;
 * lParam = 0;
 * \endcode
 *
 * \param push_state The push state of a button, can be one of the following values:
 *      - Zero\n
 *        Want the button to be unpushed.
 *      - Non zero\n
 *        Want the button to be pushed.
 *
 * \returns The old button state.
 */
#define BM_SETSTATE             0xF0F3

/**
 * \def BM_SETSTYLE
 * \brief Changes the style of a button.
 *
 * An application sends a BM_SETSTYLE message to change the style of a button.
 *
 * \code
 * BM_SETSTYLE
 * int button_style;
 *
 * wParam = (WPARAM)button_style;
 * lParam = 0;
 * \endcode
 *
 * \param button_style The styles of a button.
 *
 * \returns Always be zero.
 *
 * \sa ctrl_button_styles
 */
#define BM_SETSTYLE             0xF0F4

/**
 * \def BM_CLICK
 * \brief Simulates the user clicking a button.
 *
 * An application sends a BM_CLICK message to simulate the user clicking a button.
 *
 * \code
 * BM_CLICK
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define BM_CLICK                0xF0F5


/**
 * \def BM_ENABLE
 * \brief enable or disable the button. 
 *If it is disabled, it can receive mouse and key messages 
 *but don't response them
 *
 * \code
 * BM_CLICK
 *
 * wParam = (BOOL)enable;
 * lParam = 0;
 * \endcode
 * \param enable TRUE--enable the button
 *               FALSE--disable the button.
 * \returns Always be zero.
 */
#define BM_ENABLE               0xF0F6

/**
 * \def BM_GETIMAGE
 * \brief Retrieves the handle to the image.
 *
 * An application sends a BM_GETIMAGE message to 
 * retrieve a handle to the image (icon or bitmap) associated with the button.
 *
 * \code
 * BM_GETIMAGE
 * int image_type;
 *
 * wParam = (WPARAM)&image_type;
 * lParam = 0;
 * \endcode
 *
 * \param image_type The type of a button image will be returned through this buferr.
 *        It can be one of the following values:
 *      - BM_IMAGE_BITMAP\n
 *        Bitmap of a button.
 *      - BM_IMAGE_ICON\n
 *        Icon of a button.
 *
 * \returns A handle of the bitmap or icon of the button, zero when error.
 */
#define BM_GETIMAGE             0xF0F7

#define BM_IMAGE_BITMAP         1
#define BM_IMAGE_ICON           2
    
/**
 * \def BM_SETIMAGE
 * \brief Associates a new image (icon or bitmap) with the button.
 *
 * An application sends a BM_SETIMAGE message to 
 * associate a new image (icon or bitmap) with the button.
 *
 * Please use BM_IMAGE_BITMAP or BM_IMAGE_ICON as the first parameter of the message 
 * to indicate the type of button control image:
 *  - BM_IMAGE_BITMAP\n
 *          Specifies the type of image to associate with the button to be a bitmap.
 *  - BM_IMAGE_ICON\n
 *          Specifies the type of image to associate with the button to be an icon.
 */
#define BM_SETIMAGE             0xF0F8

#define BM_MSGMAX               0xF100

    /** @} end of ctrl_button_msgs */
    
    /**
     * \defgroup ctrl_button_ncs Notification codes of button control
     * @{
     */

/**
 * \def BN_CLICKED
 * \brief The BN_CLICKED notification message is sent when the user clicks a button.
 */
#define BN_CLICKED          0
#define BN_DOUBLECLICKED    1
/**
 * \def BN_DBLCLK
 * \brief The BN_DBLCLK notification message is sent when the user double-clicks a button.
 */
#define BN_DBLCLK           BN_DOUBLECLICKED

/**
 * \def BN_HILITE
 * \brief The BN_HILITE notification message is sent when the user hilite a button
 *      (the user moves the mouse onto it)
 */
#define BN_HILITE           2

/**
 * \def BN_UNHILITE
 * \brief The BN_UNHILITE notification message is sent when the user unhilite a button, 
 *      (the user moves the mouse leaving it)
 */
#define BN_UNHILITE         3

/**
 * \def BN_PUSHED
 * \brief The BN_PUSHED notification message is sent when the user pushes a button.
 */
#define BN_PUSHED           4

/**
 * \def BN_UNPUSHED
 * \brief The BN_UNPUSHED notification message is sent when the user unpushes a button.
 */
#define BN_UNPUSHED         5

/**
 * \def BN_DISABLE
 * \brief The BN_DISABLE notification message is sent when the user disables a button.
 * (the user sends BM_ENABLE(wParam == FALSE) to it)
 */
#define BN_DISABLE          6

/**
 * \def BN_ENABLE
 * \brief The BN_DISABLE notification message is sent when the user disables a button.
 * (the user sends BM_ENABLE(wParam == TRUE) to it)
 */
#define BN_ENABLE           7

/**
 * \def BN_SETFOCUS
 * \brief The BN_SETFOCUS notification message is sent when a button receives the keyboard focus.
 */
#define BN_SETFOCUS         8

/**
 * \def BN_KILLFOCUS
 * \brief The BN_KILLFOCUS notification message is sent when a button loses the keyboard focus.
 */
#define BN_KILLFOCUS        9

#define BN_PAINT            10   /* not supported */

    /** @} end of ctrl_button_ncs */

    /** @} end of ctrl_button */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_BUTTON_H */

