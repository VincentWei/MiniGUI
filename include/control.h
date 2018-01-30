/**
 * \file control.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2001/12/29
 * 
 * \brief This file includes interfaces of standard controls of MiniGUI.
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
 * $Id: control.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 */

#ifndef _MGUI_CONTROL_H
#define _MGUI_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "ctrl/ctrlhelper.h"

    /**
     * \defgroup controls Standard controls
     * @{
     */

/* NOTE: control messages start from 0xF000 to 0xFFFF */
#define MSG_FIRSTCTRLMSG    0xF000
#define MSG_LASTCTRLMSG     0xFFFF

    /** @} end of controls */

#ifdef _MGCTRL_STATIC
#include "ctrl/static.h"
#endif /* _MGCTRL_STATIC */

#ifdef _MGCTRL_BUTTON
#include "ctrl/button.h"
#endif /* _MGCTRL_BUTTON */

#if defined(_MGCTRL_SLEDIT) || defined(_MGCTRL_MLEDIT)
#include "ctrl/edit.h"
#endif /* _MGCTRL_EDIT || _MGCTRL_MEDIT */

#ifdef _MGCTRL_PROGRESSBAR
#include "ctrl/progressbar.h"
#endif /* _MGCTRL_PROGRESSBAR */

#ifdef _MGCTRL_LISTBOX
#include "ctrl/listbox.h"
#endif /* _MGCTRL_LISTBOX */

#ifdef _MGCTRL_PROPSHEET
#include "ctrl/propsheet.h"
#endif /* _MGCTRL_PROPSHEET*/

#ifdef _MGCTRL_COMBOBOX
#include "ctrl/combobox.h"
#endif /* _MGCTRL_COMBOBOX */

#ifdef _MGCTRL_NEWTOOLBAR
#include "ctrl/newtoolbar.h"
#endif /* _MGCTRL_NEWTOOLBAR */

#ifdef _MGCTRL_MENUBUTTON
#include "ctrl/menubutton.h"
#endif /* _MGCTRL_MENUBUTTON */

#ifdef _MGCTRL_TRACKBAR
#include "ctrl/trackbar.h"
#endif /* _MGCTRL_TRACKBAR */

#ifdef _MGCTRL_SCROLLVIEW
#include "ctrl/scrollview.h"
#endif /* _MGCTRL_SCROLLVIEW */

#ifdef _MGCTRL_SCROLLBAR
#include "ctrl/scrollbar.h"
#endif /* _MGCTRL_SCROLLBAR */

#ifdef _MGCTRL_TEXTEDIT
#include "ctrl/textedit.h"
#endif /* _MGCTRL_TEXTEDIT */

#ifdef _MGCTRL_SPINBOX
#include "ctrl/spinbox.h"
#endif /* _MGCTRL_SPINBOX */

#ifdef _MGCTRL_MONTHCAL
#include "ctrl/monthcal.h"
#endif /* _MGCTRL_MONTHCAL */

#ifdef _MGCTRL_COOLBAR
#include "ctrl/coolbar.h"
#endif /* _CTRL_COOLBAR */

#ifdef _MGCTRL_LISTVIEW
#include "ctrl/listview.h"
#endif  /* _MGCTRL_LISTVIEW */

#ifdef _MGCTRL_TREEVIEW
#include "ctrl/treeview.h"
#endif /* _MGCTRL_TREEVIEW */

#ifdef _MGCTRL_GRIDVIEW
#include "ctrl/gridview.h"
#endif /* _MGCTRL_GRIDVIEW */

#ifdef _MGCTRL_ICONVIEW
#include "ctrl/iconview.h"
#endif  /* _MGCTRL_ICONVIEW */

#ifdef _MGCTRL_ANIMATION
#include "ctrl/animation.h"
#endif  /* _MGCTRL_ANIMATION */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CONTROL_H */

