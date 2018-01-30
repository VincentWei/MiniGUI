/**
 * \file vcongui.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 * \brief This file includes interfaces for libvcongui, which provides 
 *        a virtual console on MiniGUI.
 *
 * \note vcongui had moved to mGUtils.
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
 * $Id: vcongui.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 *
 * Some idea and source come from CCE (Console Chinese Environment) 
 * Thank He Rui and Takashi MANABE for their great work and good license.
 *
 * The copyright statement of CCE and KON2:
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
 *
 * CCE - Console Chinese Environment -
 * Copyright (C) 1998-1999 Rui He (herui@cs.duke.edu)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#ifndef	_MGUI_VCONGUI_H
#define	_MGUI_VCONGUI_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup vcongui_fns Interfaces of VCOnGUI library (libvcongui)
     *
     * Libvcongui provides a virtual console in a main window of MiniGUI.
     *
     * @{
     */

#ifndef __cplusplus
#ifndef __ECOS__
typedef	enum {false, true} bool;
#endif
#endif

#define VCONGUI_VERSION "Version 0.5 (Nov. 2001)"

#define MIN_COLS    10
#define MAX_COLS    100
#define MIN_ROWS    10
#define MAX_ROWS    60

#define GetCharWidth GetSysCharWidth
#define GetCCharWidth GetSysCCharWidth
#define GetCharHeight GetSysCharHeight

/**
 * Information of child process created by libvcongui.
 * \sa VCOnMiniGUI
 */
typedef struct _CHILDINFO
{
    /** Whether display start up messages. */
    bool startupMessage;
    /** Customized string will be displayed as a startup message. */
    const char* startupStr;
    /**
     * The program should be executed when startup
     * (If it is NULL, MiniGUI will try to execute the default shell).
     */
    const char* execProg;
    /** The argument of the startup program if \a execProg is NULL. */
    const char* execArgs;
    
    /**
     * The customized default window procedure of vcongui main window
     * (If it is NULL, MiniGUI will call the default main window procedure).
     */
    WNDPROC     DefWinProc;
    /** Whether display a menu */
    bool        fMenu;
    /** The initial position of the vcongui main window. */
    int         left, top;
    /** The number of terminal rows and columns. */
    int         rows, cols;
}CHILDINFO;
/** Data type of pointer to a CHILDINFO */
typedef CHILDINFO* PCHILDINFO;

/**
 * \fn void* VCOnMiniGUI (void* data)
 * \brief Creates a vcongui main window.
 *
 * This function creates a vcongui main window, and enter a message loop.
 * 
 * \param data The pointer to a CHILDINFO structure.
 */
void* VCOnMiniGUI (void* data);

#ifndef _LITE_VERSION
void* NewVirtualConsole (PCHILDINFO pChildInfo);
#endif

    /** @} end of vcongui_fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_VCONGUI_H */


