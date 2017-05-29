/**
 * \file ctrlhelper.h
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
 * $Id: ctrlhelper.h 10829 2008-08-26 07:47:17Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_CTRLHELPER_H
#define _MGUI_CTRL_CTRLHELPER_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \addtogroup misc_fns Miscellaneous functions
     * @{
     */

/**
 * \enum SBPolicyType
 * \brief Scroll bar display policies in scrolled window
 */
typedef enum
{
  /**
   * The scroll bar is always visible
   */
  SB_POLICY_ALWAYS,
  /**
   * The scroll bar is shown or hided automatically
   */
  SB_POLICY_AUTOMATIC,
  /** The scroll bar is never visbile
   */
  SB_POLICY_NEVER
} SBPolicyType;

#define DF_3DBOX_NORMAL     0x0000
#define DF_3DBOX_PRESSED    0x0001
#define DF_3DBOX_STATEMASK  0x000F
#define DF_3DBOX_NOTFILL    0x0000
#define DF_3DBOX_FILL       0x0010

/**
 * \fn void GUIAPI DisabledTextOutEx (HDC hdc, HWND hwnd, int x, int y, const char* szText)
 * \brief Outputs disabled (grayed) text.
 *
 * This function outputs a grayed text at the specified position.
 *
 * \param hdc The device context.
 * \param hwnd Tell the function to draw with the color definitions of this window.
 * \param x The x-coordinate of start point.
 * \param y The y-coordinate of start point.
 * \param szText The null-terminated text to be output.
 *
 * \sa TextOut, DrawText
 */
MG_EXPORT void GUIAPI DisabledTextOutEx (HDC hdc, HWND hwnd, int x, int y, const char* szText);

/**
 * \fn HWND CreateToolTipWin (HWND hParentWnd, int x, int y,\
                int timeout_ms, const char* text, ...)
 * \brief Creates a tool tip window.
 *
 * This function creates a tool tip window and returns the handle to it.
 * You can call \a DestroyToolTipWin to destroy it. This function also 
 * receives \a printf-like arguments to format a string. 
 *
 * Note that the tool tip window will disappear automatically after the 
 * specified milliseconds by \a timeout_ms if \a timeout_ms is larger 
 * than 9 ms.
 *
 * \param hParentWnd The hosting main window.
 * \param x The position of the tool tip window.
 * \param y The position of the tool tip window.
 * \param timeout_ms The timeout value of the tool tip window. 
 * \param text The format string.
 *
 * \return The handle to the tool tip window on success, HWND_INVALID on error.
 *
 * \sa ResetToolTipWin, DestroyToolTipWin
 */
MG_EXPORT HWND CreateToolTipWin (HWND hParentWnd, int x, int y, 
                int timeout_ms, const char* text, ...);

/**
 * \fn void ResetToolTipWin (HWND hwnd, int x, int y,\
                const char* text, ...)
 * \brief Resets a tool tip window.
 *
 * This function resets the tool tip window specified by \a hwnd, including its
 * position, text displayed in it, and the visible status. If the tool tip is
 * invisible, it will become visible.
 *
 * This function also receives \a printf-like arguments to format a string. 
 *
 * \param hwnd The tool tip window handle returned by \a CreateToolTipWin.
 * \param x The new position of the tool tip window.
 * \param y The new position of the tool tip window.
 * \param text The new format string.
 *
 * \sa CreateToolTipWin, DestroyToolTipWin
 */
MG_EXPORT void ResetToolTipWin (HWND hwnd, int x, int y, 
                const char* text, ...);

/**
 * \fn void DestroyToolTipWin (HWND hwnd)
 * \brief Destroies a tool tip window.
 *
 * This function destroies the specified tool tip window \a hwnd, which 
 * is returned by \a CreateToolTipWin.
 *
 * \param hwnd The handle to the tool tip window.
 * 
 * \sa CreateToolTipWin
 */
MG_EXPORT void DestroyToolTipWin (HWND hwnd);

/**
 * \fn void GUIAPI NotifyParentEx (HWND hwnd, int id, int code, DWORD add_data)
 * \brief Sends a notification message to the parent.
 *
 * By default, the notification from a control will be sent to its parent
 * window within a MSG_COMMAND messsage.
 *
 * Since version 1.2.6, MiniGUI defines the Nofication Callback Procedure 
 * for control. You can specify a callback function for a control by calling
 * \a SetNotificationCallback to receive and handle the notification from 
 * the control.
 *
 * If you have defined the Notificaton Callback Procedure for the control,
 * calling NotifyParentEx will call the notification callback procedure,
 * not send the notification message to the parent.
 *
 * Note that if the control has WS_EX_NOPARENTNOTIFY style, this function
 * will not notify the parent.
 *
 * \param hwnd The handle to current control window.
 * \param id The identifier of current control.
 * \param code The notification code.
 * \param add_data The additional data of the notification.
 *
 * \sa SetNotificationCallback
 */
MG_EXPORT void GUIAPI NotifyParentEx (HWND hwnd, int id, int code, DWORD add_data);

/**
 * \def NotifyParent(hwnd, id, code)
 * \brief Sends a notification message to the parent, 
 *        but without additional data.
 *
 * \param hwnd The handle to current control window.
 * \param id The identifier of current control.
 * \param code The notification code.
 *
 * \note This function is actually a macro of NotifyParentEx with 
 *       \a dwAddData being zero.
 *
 * \sa NotifiyParentEx
 */
#define NotifyParent(hwnd, id, code) \
                NotifyParentEx(hwnd, id, code, 0)

/**
 * \var typedef int (*STRCMP) (const char* s1, const char* s2, size_t n)
 * \brief Type of general strncmp function.
 *
 * The function compares the two strings \a s1 and \a s2. It returns
 * an integer less than, equal to, or greater than zero if  \a s1 is found,
 * respectively, to be less than, to match, or be greater than \a s2.
 *
 * Note that it only compares the first (at most) \a n characters of s1 and s2. 
 */
typedef int (*STRCMP) (const char* s1, const char* s2, size_t n);

MG_EXPORT int GUIAPI 
DefaultPageProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

    /** @} end of misc_fns */

    /** @} end of global_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_CTRLHELPER_H */

