# MiniGUI

A mature cross-platform windowing system and GUI system for embedded and IoT devices.


## TABLE OF CONTENTS

  1. Introduction
  1. A brief building instruction
  1. New features in version 3.2.x
  1. The runtime modes of MiniGUI
  1. History
  1. About the authors
  1. If you have a problem
  1. A little FAQ
  1. Not free for commercial use
  1. Copying


## INTRODUCTION
    
MiniGUI is a mature cross-platform windowing system and a GUI (Graphics 
User Interface) support system for embedded systems and smart IoT devices.
This is the version 3.2.x of MiniGUI maintained by WEI Yongming 
(Vincent Wei, https://github.com/VincentWei).

MiniGUI is released under GPLv3 and the dual-licensing applies to commercial
use.

MiniGUI aims to provide a fast, stable, full-featured, and cross-platform
windowing and GUI support system, which is especially fit for 
embedded systems or smart IoT devices based-on Linux/uClinux, eCos, and other 
traditional RTOSes, such as VxWorks, ThreadX, Nucleus, pSOS, uC/OS-II, 
and OSE.

This is the MiniGUI core source tree, which provides windowing 
and graphics interfaces as well as a lot of standard controls (tookit). 

Besides MiniGUI core, FMSoft also provides some components
for the developers to develop app much easier:

  * mGUtils: A MiniGUI component which contains miscellaneous utilities 
    like ColorSelectionDialogBox, FileOpenDialogBox, and so on. 

  * mGPlus: A MiniGUI component which provides support for advanced graphics 
    functions like path, gradient, anti-aliase stretch, and color combination.

  * mGEff: mGEff provides an animation framework for MiniGUI app. 
     It also provides some popular UI/UE special effects.

  * mGNCS: This component provides a new control set for MiniGUI app. 
    By using mGNCS, you can use miniStudio to develop MiniGUI app in 
    WYSIWYG way. 

  * mGNCS4Touch: This component provides some new controls, which are 
    compliant to the new control API spec of mGNCS, for devices with 
    a touch screen.

There are also some legacy MiniGUI components. We will not maintain these
components for MiniGUI core v3.2:

  * mGi: An input method component for MiniGUI. This component provides
    some typical input methods (such as softkey, hand-writing, and predict
    text input for mobile phone) for MiniGUI apps. 

  * mGp: A printing component for MiniGUI. By using mGp, you can print
    out the graphics created by MiniGUI app to general-purpose printers.

  * mG3d: A simple three-dimension component for MiniGUI. This component
    provides a simple 3D API for app. You can use mG3d to create simple
    3D UI.

MiniGUI is a free software project. In December 1998, the initiator of 
FMSoft, Wei Yongming, began to develop MiniGUI under the GNU General Public 
License (GPL). In September 2002, the core developers of 
MiniGUI founded FMSoft and started the commercial marketing with 
the free software. By now, FMSoft still continues to release MiniGUI 
as a free software project.

You can download the source code tarball of MiniGUI and the dependent libraries
from the download zone of the site:

  http://www.minigui.com

FMSoft also releases some open source apps for MiniGUI on the site above, 
for examples:

  * mDolphin, licensed under Apache 2.0, is a full-featured 
    WEB/WAP browser, which is developed based on the open source browser 
    core WebKit and uses MiniGUI as its underlying graphics support system.

  * mGallery, licensed under Apache 2.0, intends to 
    provide a total solution for PMP (Portable Multimedia Player), 
    which uses MiniGUI as the graphics platform.

  * mSpider, licensed under GPL 3.0, intends to provide a 
    light-weight MiniGUI-based web browser (HTML 3.2 and part JavaScript)

  * mEagle, licensed under GPL 3.0, is an embedded GIS development platform 
    which addresses the needs of map browse, query, analysis, etc

FMSoft had created the public repositories for MiniGUI core, MiniGUI 
components, and other open source apps on GitHub. You can visit them on:

https://github.com/VincentWei 

FMSoft also releases miniStudio, which is a WYSIWYG IDE for MiniGUI, as a shareware.
You can also download it from the official site of MiniGUI.

http://www.minigui.com/en/download


## A BRIEF BUILDING INSTRUCTION

This instruction assumes that you are using Ubuntu Linux.

### Prerequisites

You can run `apt install <package_name>` to install the software on Ubuntu Linux.

 * Building tools:
    * autoconf
 * Dependent libraries (all are optional):
    * libjpeg64-dev
    * libpng12-dev
    * libfreetype6-dev

### Building

MiniGUI core uses GNU autoconf/automake scripts to configure and build the project.

Run

    $ ./configure; make; sudo make install

to configure, make, and install the headers and the libraries. If there is not
`configure` file, please run

    $ ./autogen.sh

to generate the script.

MiniGUI core also provides some configuration options to customize the features.
For more information, please run

    $ ./configure --help


## NEW FEATURES IN VERSION 3.2.x

We introduce some new features in MiniGUI Version 3.2.x:

  * The support for 64-bit platform. Note that some APIs have changed
    to support 64-bit platform. For more information, please refer to
    RELEASE-NOTES.md file.

Because of the changes of some APIs, we recommended strongly that you 
use this version for new projects.

## THE RUNTIME MODES OF MINIGUI

You can configure and compile MiniGUI as one of three runtime modes:

  * 'MiniGUI-Threads': A program running on MiniGUI-Threads can create
    multiple cascaded windows in different threads, and all the windows
    belong to a single process. MiniGUI-Threads is fit for some real-time
    systems on Linux/uClinux, eCos, uC/OS-II, VxWorks, pSOS, ThreadX, 
    and OSE.

  * 'MiniGUI-Processes': A program running on MiniGUI-Processes is 
    an independent process, which can also create multiple windows. 
    MiniGUI-Processes is fit for some complex embedded systems, such as 
    PDAs, Thin-Clients or STBs. This mode is only useful for full-featured 
    UNIX-like operating systems, like Linux.

  * 'MiniGUI-Standalone': A single process version of MiniGUI. This mode
    is useful for some small systems, like uClinux or RTOSes.

The original MiniGUI (version 1.0) run in MiniGUI-Threads mode.
It is based on POSIX-compliant thread library. And this thread-based 
architecture of MiniGUI is very fit for most traditional embedded 
operating systems, such as eCos, uC/OS-II, and VxWorks. However, 
if you use embedded Linux, the architecture like X Window will have 
better stability and scalability, because of the independent memory 
address space of every process.

Since version 2.0, we provides a new runtime mode called MiniGUI-Processes.
You can use MiniGUI-Processes to run more than one MiniGUI-based 
program in the form of UNIX process at the same time. MiniGUI-Processes 
is a full-featured multi-process version of original MiniGUI -- You 
can run a program based on MiniGUI-Processes from a program called 
'mginit'. Just like X Window, the former process is called a client, 
and the latter the server. 

Clients connect to the server via UNIX domain socket, and the server 
receives and responses requests from clients. The server provides shared 
resources for clients, manage window objects, and sends mouse 
(or touch screen) and keyboard events to the active top-most client. 
If a client exits or dies for some reasons, it will not damage other 
clients and the server.


## HISTORY

About ten years have pasted since MiniGUI was initially created at 
the end of 1998, and now it becomes much reliable and stable. 
During the past years, many remarkable changes have taken place in 
the MiniGUI project.

At the original, the version 0.2.xx was based on SVGALib and PThread 
(LinuxThreads). Then the version 0.3.xx had many enhanced features 
including Graphics Abstract Layer (GAL), Input Abstract Layer (IAL), 
and the support for multiple charsets and multiple fonts. 

In version 1.0.00, we designed MiniGUI-Lite, which was more fit for 
some complex embedded systems, such as PDAs, Thin-Clients, or STBs. 
MiniGUI-Lite made the embedded systems more stable and efficient. 
The version 1.0.00 provided the native fbcon engine based on Linux 
frame buffer device directly as well. 

In the development of version 1.1.00, we re-wrote the GAL and GDI 
interfaces, and provided some advanced video features, such as raster 
operation, transparent blitting, alpha blending, and video hardware 
acceleration. We also provided some important GDI functions, such as 
Ellipse, Arc, Polygon, and FloodFill.

In the development of version 1.6.0, we added advanced 2D graphics
functions to MiniGUI. It provides the abstract logical pen and brush
objects, and can be used to render advanced graphics objects.

In the development of version 2.0.x, we developed MiniGUI-Processes.

In the development of version 3.0.x, we introduced Look & Feel Renderer,
BIDI text support, Non-rectangular window, double-buffer window, and
other main features.

A brief history description of the development progress lay below:

  0)  1994 ~ 1996:    MiniGUI DOS version.
  1)  Dec, 1998:      Began to write.
  2)  Apr, 1999:      The skeleton of MiniGUI, windowing sub-system 
      and basic graphics device interfaces.
  3)  May, 1999:      Timer, menu, and the skeleton of control sub-system.
  4)  Jun, 1999:      Chinese input method (by Kang Xiaoning).
  5)  July, 1999:     GIF and JPG loading support (by LI Zhuo).
  6)  Aug, 1999:      Some standard controls (Portion by Zhao Jianghua).
  7)  Sep, 1999:      Dialog boxes and message boxes.
  8)  Sep, 1999:      Snapshot of screen or window.
  9)  Jan., 2000:     VCOnGUI (Virtual Console on MiniGUI) version 0.2.02.
  10) Mar., 2000:     Linux distribution installer based-on MiniGUI for 
      HappyLinux 1.0.
  11) Jun., 2000:     Began to develop version 1.0.xx.
  12) Sep., 2000:     MiniGUI version 0.3.06 released.
  13) Oct., 2000:     MiniGUI version 0.9.00 released. 
  14) Nov., 2000:     MiniGUI version 0.9.96 released. 
  15) Jan., 2001:     MiniGUI version 0.9.98 released. You can build 
      MiniGUI-Lite from this version.
  16) Jan., 2001:     MiniGUI version 1.0.00Beta1 released.
  17) Feb., 2001:     MiniGUI version 1.0.00Pre1 released.
  18) Apr., 2001:     MiniGUI version 1.0.00 released (2001/04/16).
  18) Sep., 2001:     MiniGUI version 1.1.0Pre1 released (2001/09/12).
  19) Sep., 2001:     MiniGUI version 1.0.9 released (2001/09/17), 
      this will be the last version of source branch 1.0.x. 
  20) Oct., 2001:     MiniGUI version 1.1.0Pre4 released (2001/10/22). 
      This version have new GAL and GDI interfaces.
  21) Mar., 2002:     MiniGUI official version 1.1.0 released (2002/03/08). 
      MiniGUI API Reference documentation version 0.9 released.
  22) Mar., 2002:     MiniGUI API Reference documentation version 1.0 released.
  23) Apr., 2002:     MiniGUI the new stable version 1.2.0 released (2002/04/11). 
  24) Sep., 2002:     The main developers of MiniGUI founded a 
      new software corporation: Beijing FMSoft Technology Co., Ltd.. 
      And the development and maintenance of MiniGUI changes from 
      loosely-knit team to business organization.
  25) Mar., 2003:     MiniGUI official version 1.2.5 released (2003/03/23).
  25) May., 2003:     MiniGUI official version 1.2.6 released (2003/05/18).
  26) Sep., 2003:     MiniGUI official version 1.3.1 released (2003/09/11).
  27) Jan., 2004:     MiniGUI official version 1.5.1 released (2004/01/18).
  28) Feb., 2004:     MiniGUI tested on eCos and uC/OS-II.
  29) Jun., 2004:     MiniGUI tested on VxWorks.
  30) Jan., 2005:     MiniGUI tested on ThreadX and pSOS.
  31) Feb., 2005:     MiniGUI official version 1.6.1 released.
  32) Aug., 2005:     MiniGUI beta version 2.0.0 released.
  33) Sep., 2005:     MiniGUI official version 1.6.8 released.
  34) Oct., 2005:     MiniGUI official version 2.0.2 released.
  35) Jun., 2006:     MiniGUI provides support for OSE.
  36) Jul., 2006:     MiniGUI official version 2.0.3/1.6.9 released.
  37) Nov., 2007:     MiniGUI official version 2.0.4/1.6.10 released.
  38) Apr., 2009:     MiniGUI official version 3.0.2 released.
  39) Oct., 2009:     MiniGUI official version 3.0.4 released.
  40) Jan., 2010:     MiniGUI official version 3.0.6 released.
  41) May., 2010:     MiniGUI official version 3.0.8 released.
  42) Aug., 2010:     MiniGUI official version 3.0.10 released.
  43) Oct., 2010:     MiniGUI official version 3.0.12 released.
  44) Aug., 2017:     FMSoft released the latest source code of MiniGUI under GPL 3.0.
  45) Aug., 2017:     FMSoft released the latest source code of mDolphin under Apache 2.0.
  46) Dec., 2017:     FMSoft updated MiniGUI and miniStudio for latest Linux distributions.
  47) Feb., 2018:     FMSoft released MiniGUI version 3.2.


## ABOUT THE AUTHORS

The original author of MiniGUI is WEI Yongming, and now MiniGUI is 
maintained by FMSoft. For more information, please browse 
our home page: http://www.fmsoft.cn.


## IF YOU HAVE A PROBLEM

If you have any technical problem, advice or comment, please send 
messages to consult@minigui.com.


## A LITTLE FAQ

Q: Is GPL'd MiniGUI free for commercial use?

A: Simply no. FMSoft releases MiniGUI under GPLv3 license. 
   It is free for those who are 100% GPL and those who never copy, modify 
   and distribute MiniGUI. But if you want to use these GPL'd versions 
   for commerce, you should get the commercial license from FMSoft first.

Q: Which operating system does MiniGUI support?

A: MiniGUI can run on Linux/uClinux, uC/OS-II, eCos, ThreadX, pSOS, 
   VxWorks, ThreadX, OSE, and even Win32. Any other real-time OSes can 
   be supported theoretically.

Q: Which architecture does MiniGUI support?

A: x86 32/64-bit, ARM 32/64-bit (e.g., ARMv7 and ARM Cortex-A7),
   PowerPC, MIPS, DragonBall, ColdFire, Blackfin, and so on.

Q: Does MiniGUI offer the support for grey LCD with 4bpp (bits per pixel)?

A: Yes, it offers. It even offers the support for 1bpp and 2bpp LCD. 


## NOT FREE FOR COMMERCIAL USE

MiniGUI and its components are licensed under the GPL license. 
So any links about MiniGUI must follow GPL. If you cannot accept GPL, 
you need to be licensed from FMSoft. If you wonder that if you need 
the commercial license of MiniGUI, please refer to the LICENSE-POLICY file.

We provide you the commercial license of MiniGUI according to the number 
of the run-time binary copies of MiniGUI. If you are interested in 
the commercial license of MiniGUI, please write to sales@minigui.com.  


## COPYING

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

