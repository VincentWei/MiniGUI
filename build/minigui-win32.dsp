# Microsoft Developer Studio Project File - Name="miniguiwin32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=miniguiwin32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "miniguiwin32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "miniguiwin32.mak" CFG="miniguiwin32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "miniguiwin32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "miniguiwin32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUIWIN32_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /I ".." /I "../include" /I "../src/include" /I "../../include" /I "../../include/jpg" /I "../../include/png" /I "../../include/zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUIWIN32_EXPORTS" /D "__MINIGUI_LIB__" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dirent.lib libpng.lib libjpeg.lib libz.lib pthreadVC1.lib /nologo /dll /machine:I386 /out:"../../../mdesigner/miniguiwin32.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUIWIN32_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".." /I "../include" /I "../src/include" /I "../../include" /I "../../include/jpg" /I "../../include/png" /I "../../include/zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUIWIN32_EXPORTS" /D "__MINIGUI_LIB__" /FR /YX /FD /GZ /TC /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dirent.lib libpng.lib libjpeg.lib libz.lib pthreadVC1.lib /nologo /dll /debug /machine:I386 /out:"../../svn-mdesigner/lib-win/miniguiwin32.dll" /pdbtype:sept /libpath:"D:\minigui\minigui-3.0-lib\lib"

!ENDIF 

# Begin Target

# Name "miniguiwin32 - Win32 Release"
# Name "miniguiwin32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\ial\auto.c
# End Source File
# Begin Source File

SOURCE=..\src\ial\auto.h
# End Source File
# Begin Source File

SOURCE=..\src\ial\dummy.c
# End Source File
# Begin Source File

SOURCE=..\src\ial\dummy.h
# End Source File
# Begin Source File

SOURCE=..\src\ial\ial.c
# End Source File
# Begin Source File

SOURCE=..\src\ial\winial.c
# End Source File
# Begin Source File

SOURCE=..\src\ial\winial.h
# End Source File
# Begin Source File

SOURCE=..\src\ial\wvfb.h
# End Source File
# Begin Source File

SOURCE=..\src\ial\wvfbial.c
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Group "in-core"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\src\font\in-core\rbf_fixedsys_8x15.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\rbf_terminal_8x12.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\rbf_vgaoem_8x8.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\upf_smoothtimes_100_50.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_courier_8x13.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_sansserif_11x13.c"
# ADD CPP /I "../src/font"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_system_14x16.c"
# ADD CPP /I "../src/font"
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\font\big5unimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\bitmapfont.c
# End Source File
# Begin Source File

SOURCE=..\src\font\bitmapfont.h
# End Source File
# Begin Source File

SOURCE="..\src\font\charset-arabic.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\charset-bidi.c"
# End Source File
# Begin Source File

SOURCE=..\src\font\charset.c
# End Source File
# Begin Source File

SOURCE=..\src\font\charset.h
# End Source File
# Begin Source File

SOURCE=..\src\font\convgbmap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\devfont.c
# End Source File
# Begin Source File

SOURCE=..\src\font\euckrunimap.c
# End Source File
# Begin Source File

SOURCE="..\src\font\font-engines.c"
# End Source File
# Begin Source File

SOURCE=..\src\font\fontcache.c
# End Source File
# Begin Source File

SOURCE=..\src\font\fontname.c
# End Source File
# Begin Source File

SOURCE=..\src\font\freetype.c
# End Source File
# Begin Source File

SOURCE=..\src\font\freetype.h
# End Source File
# Begin Source File

SOURCE=..\src\font\freetype2.c
# End Source File
# Begin Source File

SOURCE=..\src\font\freetype2.h
# End Source File
# Begin Source File

SOURCE=..\src\font\gb18030unimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\gbkunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\gbunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\gunichartables.h
# End Source File
# Begin Source File

SOURCE=..\src\font\gunicode.h
# End Source File
# Begin Source File

SOURCE=..\src\font\jisunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\logfont.c
# End Source File
# Begin Source File

SOURCE=..\src\font\mapunitobig5.c
# End Source File
# Begin Source File

SOURCE=..\src\font\mapunitogb.c
# End Source File
# Begin Source File

SOURCE=..\src\font\mapunitogbk.c
# End Source File
# Begin Source File

SOURCE=..\src\font\qpf.c
# End Source File
# Begin Source File

SOURCE=..\src\font\qpf.h
# End Source File
# Begin Source File

SOURCE=..\src\font\rawbitmap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\rawbitmap.h
# End Source File
# Begin Source File

SOURCE=..\src\font\sjisunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\sysfont.c
# End Source File
# Begin Source File

SOURCE=..\src\font\textops.c
# End Source File
# Begin Source File

SOURCE=..\src\font\ujisunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\upf.c
# End Source File
# Begin Source File

SOURCE=..\src\font\upf.h
# End Source File
# Begin Source File

SOURCE=..\src\font\varbitmap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\varbitmap.h
# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\main\main.c
# End Source File
# Begin Source File

SOURCE="..\src\main\minigui-win32.c"
# End Source File
# End Group
# Begin Group "newgal"

# PROP Default_Filter ""
# Begin Group "wvfb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\newgal\wvfb\winfb.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/newgal"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../src/newgal" /I "../"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\winfb.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\wvfb.c
# ADD CPP /I "../src/newgal"
# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\wvfb.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\newgal\blit.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_0.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_1.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_A.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_A.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_N.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\leaks.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\newgal.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\dummy\nullvideo.c
# ADD CPP /I "../src/newgal"
# End Source File
# Begin Source File

SOURCE=..\src\newgal\dummy\nullvideo.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\pixels.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\pixels_c.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\RLEaccel.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\RLEaccel_c.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\stretch.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\stretch_c.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\surface.c
# End Source File
# Begin Source File

SOURCE=..\src\newgal\sysvideo.h
# End Source File
# Begin Source File

SOURCE=..\src\newgal\video.c
# End Source File
# End Group
# Begin Group "control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\control\bidiedit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\bidiedit_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\button.c
# End Source File
# Begin Source File

SOURCE=..\src\control\button_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\combobox.c
# End Source File
# Begin Source File

SOURCE=..\src\control\combobox_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\ctrlmisc.c
# End Source File
# Begin Source File

SOURCE=..\src\control\ctrlmisc.h
# End Source File
# Begin Source File

SOURCE=..\src\control\edit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\listbox.c
# End Source File
# Begin Source File

SOURCE=..\src\control\listbox_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\listmodel.c
# End Source File
# Begin Source File

SOURCE=..\src\control\listmodel.h
# End Source File
# Begin Source File

SOURCE=..\src\control\medit_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\menubutton.c
# End Source File
# Begin Source File

SOURCE=..\src\control\menubutton_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\newtoolbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\newtoolbar_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\progressbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\progressbar_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\propsheet.c
# End Source File
# Begin Source File

SOURCE=..\src\control\propsheet_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrolled.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrolled.h
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollview.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollview_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollwnd.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollwnd.h
# End Source File
# Begin Source File

SOURCE=..\src\control\simedit_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\static.c
# End Source File
# Begin Source File

SOURCE=..\src\control\static_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\text.h
# End Source File
# Begin Source File

SOURCE=..\src\control\textedit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\textedit_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\control\trackbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\trackbar_impl.h
# End Source File
# End Group
# Begin Group "ex_ctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\ex_ctrl\animation.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\animation_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\coolbar.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\coolbar_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\gif89a.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\gridview.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\gridview_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\gridviewcelltype.c
# ADD CPP /I "../src/control"
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\iconview.c
# ADD CPP /I "../src/control"
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\iconview_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\listview.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\listview_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\monthcalendar.c
# ADD CPP /I "../src/control"
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\monthcalendar_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\spinbox.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\spinbox_impl.h
# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\treeview.c

!IF  "$(CFG)" == "miniguiwin32 - Win32 Release"

# ADD CPP /I "../src/control"

!ELSEIF  "$(CFG)" == "miniguiwin32 - Win32 Debug"

# ADD CPP /I "../control" /I "../src/control"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\ex_ctrl\treeview_impl.h
# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\gui\accelkey.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\arabickeymap.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\caret.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\ctrlclass.c
# End Source File
# Begin Source File

SOURCE="..\src\gui\de-kmap.c"
# End Source File
# Begin Source File

SOURCE="..\src\gui\de-latin1-kmap.c"
# End Source File
# Begin Source File

SOURCE="..\src\gui\de-latin1-nodeadkeys-kmap.c"
# End Source File
# Begin Source File

SOURCE=..\src\gui\defkeymap.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\dialog.c
# End Source File
# Begin Source File

SOURCE="..\src\gui\es-cp850-kmap.c"
# End Source File
# Begin Source File

SOURCE="..\src\gui\es-kmap.c"
# End Source File
# Begin Source File

SOURCE="..\src\gui\fr-kmap.c"
# End Source File
# Begin Source File

SOURCE="..\src\gui\fr-pc-kmap.c"
# End Source File
# Begin Source File

SOURCE=..\src\gui\hebrewkeymap.c
# End Source File
# Begin Source File

SOURCE="..\src\gui\it-kmap.c"
# End Source File
# Begin Source File

SOURCE=..\src\gui\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\keyboard.h
# End Source File
# Begin Source File

SOURCE=..\src\gui\lf_classic.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\lf_common.h
# End Source File
# Begin Source File

SOURCE=..\src\gui\lf_flat.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\lf_manager.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\lf_skin.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\linux_kd.h
# End Source File
# Begin Source File

SOURCE=..\src\gui\linux_keyboard.h
# End Source File
# Begin Source File

SOURCE=..\src\gui\linux_types.h
# End Source File
# Begin Source File

SOURCE=..\src\gui\menu.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\window.c
# End Source File
# End Group
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\kernel\blockheap.c
# End Source File
# Begin Source File

SOURCE="..\src\kernel\cursor-procs.c"
# End Source File
# Begin Source File

SOURCE="..\src\kernel\cursor-sa.c"
# End Source File
# Begin Source File

SOURCE=..\src\kernel\cursor.c
# End Source File
# Begin Source File

SOURCE="..\src\kernel\desktop-ths.c"
# End Source File
# Begin Source File

SOURCE=..\src\kernel\event.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\fixstr.c
# End Source File
# Begin Source File

SOURCE="..\src\kernel\init-lite.c"
# End Source File
# Begin Source File

SOURCE=..\src\kernel\init.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\message.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\sharedres.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\slotset.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\timer.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\zorder.c
# End Source File
# End Group
# Begin Group "libc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\libc\snprintf.c
# End Source File
# Begin Source File

SOURCE=..\src\libc\vfnprintf.c
# End Source File
# Begin Source File

SOURCE=..\src\libc\vsnprintf.c
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\misc\about.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\clipboard.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\endianrw.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\error.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\listenfd.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\math.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\misc.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\nposix.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\rwops.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\sockio.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\systext.c
# End Source File
# End Group
# Begin Group "mybmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mybmp\gif.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\jpeg.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\lbm.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\mybmp.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\pcx.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\png.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\tga.c
# End Source File
# Begin Source File

SOURCE=..\src\mybmp\winbmp.c
# End Source File
# End Group
# Begin Group "newgdi"

# PROP Default_Filter ""
# Begin Group "sysres"

# PROP Default_Filter ""
# Begin Group "bmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sysres\bmp\_bmp_inner_res.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\classic_check_button.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\classic_radio_button.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\flat_check_button.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\flat_radio_button.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_arrows.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_arrows_shell.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_bborder.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_bkgnd.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_caption.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_checkbtn.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_cpn_btn.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_header.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_lborder.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_pb_htrack.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_pb_htruck.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_pb_vtrack.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_pb_vtruck.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_pushbtn.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_radiobtn.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_rborder.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_sb_arrows.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_sb_hshaft.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_sb_hthumb.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_sb_vshaft.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_sb_vthumb.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tab.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tb_horz.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tb_vert.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tborder.bmp.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tbslider_h.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tbslider_v.gif.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\bmp\skin_tree.bmp.c
# End Source File
# End Group
# Begin Group "font No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sysres\font\_font_inner_res.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font00.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font01.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font02.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font03.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font04.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font05.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font06.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font07.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font08.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font09.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font10.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font11.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font12.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font13.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font14.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font15.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font16.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font17.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font18.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font19.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font20.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font21.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font22.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font23.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font24.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font25.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font26.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font27.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font28.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font29.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font30.c"
# End Source File
# Begin Source File

SOURCE="..\src\sysres\font\incore-font31.c"
# End Source File
# End Group
# Begin Group "icon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sysres\icon\_icon_inner_res.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\excalmatory-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\excalmatory.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\failed-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\failed.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\fold-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\fold.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\folder-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\folder.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\form-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\form.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\help-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\help.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\textfile-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\textfile.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\unfold-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\unfold.ico.c
# End Source File
# Begin Source File

SOURCE="..\src\sysres\icon\warning-flat.ico.c"
# End Source File
# Begin Source File

SOURCE=..\src\sysres\icon\warning.ico.c
# End Source File
# End Group
# Begin Group "cursor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sysres\cursor\cursors.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\sysres\incoreres.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\mgetc.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\resmgr.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\resmgr.h
# End Source File
# Begin Source File

SOURCE=..\src\sysres\resource.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\newgdi\advapi.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\arc.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\attr.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\bidi.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\bitmap.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\clip.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\coor.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\drawtext.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\drawtext.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\flood.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\gdi.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\generators.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\glyph.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\icon.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\line.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\map.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mi.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miarc.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\midash.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\midc.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifillarc.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifillarc.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifpoly.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifpolycon.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mispans.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mispans.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mistruct.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miwideline.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miwideline.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\palette.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel_ops.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel_ops.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\polygon.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\polygon.h
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\readbmp.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\rect.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\region.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\rotatebmp.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\screen.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\tabbedtextout.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\text.c
# End Source File
# Begin Source File

SOURCE=..\src\newgdi\textout.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\include\accelkey.h
# End Source File
# Begin Source File

SOURCE=..\src\include\bidi.h
# End Source File
# Begin Source File

SOURCE=..\src\include\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\src\include\blockheap.h
# End Source File
# Begin Source File

SOURCE=..\src\include\client.h
# End Source File
# Begin Source File

SOURCE=..\src\include\clipboard.h
# End Source File
# Begin Source File

SOURCE=..\src\include\cliprect.h
# End Source File
# Begin Source File

SOURCE=..\src\include\ctrlclass.h
# End Source File
# Begin Source File

SOURCE=..\src\include\cursor.h
# End Source File
# Begin Source File

SOURCE=..\src\include\dc.h
# End Source File
# Begin Source File

SOURCE=..\src\include\devfont.h
# End Source File
# Begin Source File

SOURCE=..\src\include\drawsemop.h
# End Source File
# Begin Source File

SOURCE=..\src\include\element.h
# End Source File
# Begin Source File

SOURCE=..\src\include\event.h
# End Source File
# Begin Source File

SOURCE=..\src\include\fontname.h
# End Source File
# Begin Source File

SOURCE=..\src\include\gal.h
# End Source File
# Begin Source File

SOURCE=..\src\include\glyph.h
# End Source File
# Begin Source File

SOURCE=..\src\include\ial.h
# End Source File
# Begin Source File

SOURCE=..\src\include\icon.h
# End Source File
# Begin Source File

SOURCE=..\src\include\incoreres.h
# End Source File
# Begin Source File

SOURCE=..\src\include\inline.h
# End Source File
# Begin Source File

SOURCE=..\src\include\internals.h
# End Source File
# Begin Source File

SOURCE=..\src\include\list.h
# End Source File
# Begin Source File

SOURCE=..\src\include\md5.h
# End Source File
# Begin Source File

SOURCE=..\src\include\memops.h
# End Source File
# Begin Source File

SOURCE=..\src\include\menu.h
# End Source File
# Begin Source File

SOURCE=..\src\include\misc.h
# End Source File
# Begin Source File

SOURCE=..\src\include\msgstr.h
# End Source File
# Begin Source File

SOURCE=..\src\include\newgal.h
# End Source File
# Begin Source File

SOURCE=..\src\include\ourhdr.h
# End Source File
# Begin Source File

SOURCE=..\src\include\readbmp.h
# End Source File
# Begin Source File

SOURCE=..\src\include\server.h
# End Source File
# Begin Source File

SOURCE=..\src\include\sharedres.h
# End Source File
# Begin Source File

SOURCE=..\src\include\sockio.h
# End Source File
# Begin Source File

SOURCE=..\src\include\sysfont.h
# End Source File
# Begin Source File

SOURCE=..\src\include\sysres.h
# End Source File
# Begin Source File

SOURCE=..\src\include\timer.h
# End Source File
# Begin Source File

SOURCE=..\src\include\zorder.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\template.c
# End Source File
# Begin Source File

SOURCE=..\src\template.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "ctrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\ctrl\animation.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\button.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\coolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\ctrlhelper.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\edit.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\gridview.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\iconview.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\listview.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\menubutton.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\monthcal.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\newtoolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\progressbar.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\propsheet.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\scrollbar.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\scrollview.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\spinbox.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\static.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\textedit.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\trackbar.h
# End Source File
# Begin Source File

SOURCE=..\include\ctrl\treeview.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\include\common.h
# End Source File
# Begin Source File

SOURCE=..\include\control.h
# End Source File
# Begin Source File

SOURCE=..\include\customial.h
# End Source File
# Begin Source File

SOURCE=..\include\endianrw.h
# End Source File
# Begin Source File

SOURCE=..\include\fixedmath.h
# End Source File
# Begin Source File

SOURCE=..\include\gdi.h
# End Source File
# Begin Source File

SOURCE=..\mgconfig.h
# End Source File
# Begin Source File

SOURCE=..\include\minigui.h
# End Source File
# Begin Source File

SOURCE=..\include\nucleus_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\nucleus_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\ose_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\own_malloc.h
# End Source File
# Begin Source File

SOURCE=..\include\own_stdio.h
# End Source File
# Begin Source File

SOURCE=..\include\psos_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\psos_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\threadx_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\threadx_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\ucos2_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\ucos2_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\vxworks_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\vxworks_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_dirent.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_pthread.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_sched.h
# End Source File
# Begin Source File

SOURCE=..\include\win32_semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\window.h
# End Source File
# Begin Source File

SOURCE=..\include\xvfb.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
