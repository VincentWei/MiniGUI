/**
 * \file textedit.h
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
 * $Id: textedit.h 12871 2010-05-07 06:13:42Z wanzheng $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 *             Copyright (C) 2002-2008 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
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

