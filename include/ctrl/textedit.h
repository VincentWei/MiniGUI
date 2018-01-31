/**
 * \file textedit.h
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
 * $Id: textedit.h 12871 2010-05-07 06:13:42Z wanzheng $
 *
 *          MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *          pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_TEXTEDIT_H
#define _MGUI_CTRL_TEXTEDIT_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_textedit TextEdit control
     * @{
     */

/**
 * \def CTRL_TEXTEDIT
 * \brief The class name of textedit control.
 */
#define CTRL_TEXTEDIT           ("textedit")


/** 
 * \var typedef GHANDLE HTEXTDOC
 * \brief Text document/buffer object
 */
typedef GHANDLE HTEXTDOC;


    /**
     * \defgroup ctrl_textedit_styles Styles of textedit control
     * @{
     */

#define TES_BASELINE            ES_BASELINE
#define TES_AUTOWRAP            ES_AUTOWRAP
#define TES_TITLE               ES_TITLE

    /** @} end of ctrl_textedit_styles */

    /**
     * \defgroup ctrl_textedit_msgs Messages of textedit control
     * @{
     */

#define TEM_RESETCONTENT        0xF400
#define TEM_ADDLINE             0xF401

#define TEM_GETBKGNDINVALID     0xF402

    /** @} end of ctrl_textedit_msgs */

    /**
     * \defgroup ctrl_textedit_ncs Notification codes of textedit control
     * @{
     */

#define TEN_CLICK               1


    /** @} end of ctrl_textedit_ncs */

    /** @} end of ctrl_textedit */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_TEXTEDIT_H */

