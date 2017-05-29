# Microsoft Developer Studio Project File - Name="minigui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=minigui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "minigui-pda-win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minigui-pda-win32.mak" CFG="minigui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minigui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "minigui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minigui - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pthreadVC1.lib /nologo /dll /incremental:yes /machine:I386 /out:"Release/minigui.dll" /pdbtype:sept /libpath:"..\..\fhas\build"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\fhas2\build"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\pthread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /I "..\..\include\minigui" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR /FD /GZ /Zm200 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pthreadVC1.lib dirent.lib /nologo /verbose /dll /debug /machine:I386 /out:"..\..\fhas2\build\minigui.dll" /pdbtype:sept /libpath:"..\..\fhas2\build"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "minigui - Win32 Release"
# Name "minigui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "minigui"

# PROP Default_Filter ""
# Begin Group "control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\control\button.c
# End Source File
# Begin Source File

SOURCE=..\src\control\combobox.c
# End Source File
# Begin Source File

SOURCE=..\src\control\ctrlmisc.c
# End Source File
# Begin Source File

SOURCE=..\src\control\edit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\listbox.c
# End Source File
# Begin Source File

SOURCE=..\src\control\listmodel.c
# End Source File
# Begin Source File

SOURCE=..\src\control\menubutton.c
# End Source File
# Begin Source File

SOURCE=..\src\control\newtoolbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\progressbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\propsheet.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrolled.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollview.c
# End Source File
# Begin Source File

SOURCE=..\src\control\scrollwnd.c
# End Source File
# Begin Source File

SOURCE=..\src\control\simedit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\static.c
# End Source File
# Begin Source File

SOURCE=..\src\control\textedit.c
# End Source File
# Begin Source File

SOURCE=..\src\control\toolbar.c
# End Source File
# Begin Source File

SOURCE=..\src\control\trackbar.c
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
# Begin Group "kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\kernel\blockheap.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\cursor.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\desktop.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\event.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\fixstr.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\hash.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\init.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\message.c
# End Source File
# Begin Source File

SOURCE=..\src\kernel\timer.c
# End Source File
# End Group
# Begin Group "newgdi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\newgdi\advapi.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\arc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\attr.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\bitmap.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\clip.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\coor.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\drawtext.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\drawtext.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\flood.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\gdi.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\generators.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\icon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\lbm.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\line.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\logfont.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\map.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mi.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miarc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\midash.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\midc.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifillarc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifillarc.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifpoly.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mifpolycon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mispans.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mispans.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\mistruct.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miwideline.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\miwideline.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\palette.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pcx.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel_ops.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\pixel_ops.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\polygon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\polygon.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\readbmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\rect.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\region.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\screen.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\text.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\tga.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgdi\yuv.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgdi"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "Debug/newgdi"
# SUBTRACT CPP /Fr

!ENDIF 

# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Group "incore"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\src\font\in-core\12x24-iso8859-1.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\6x12-iso8859-1.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\8x16-iso8859-1.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\courier_8x13.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\fixedsys_8x15.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_12_jiskan.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_14_jiskan.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_16_jiskan.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_6x12rk.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_7x14rk.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\gothic_8x16rk.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\sansserif_11x13.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\song-12-gb2312.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\song-16-gb2312.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\song-24-gb2312.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\symb12.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\system_14x16.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\terminal_8x12.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_helvR16.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_helvR21.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vbf_helvR27.c"
# End Source File
# Begin Source File

SOURCE="..\src\font\in-core\vgarom_8x8.c"
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\font\big5unimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\charset.c
# End Source File
# Begin Source File

SOURCE=..\src\font\charset.h
# End Source File
# Begin Source File

SOURCE=..\src\font\devfont.c
# End Source File
# Begin Source File

SOURCE=..\src\font\euckrunimap.c
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

SOURCE=..\src\font\jisunimap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\mapunitogb.c
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

SOURCE=..\src\font\varbitmap.c
# End Source File
# Begin Source File

SOURCE=..\src\font\varbitmap.h
# End Source File
# End Group
# Begin Group "newgal"

# PROP Default_Filter ""
# Begin Group "wvfb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\newgal\wvfb\winfb.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\winfb.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\wvfb.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\wvfb\wvfb.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "dummy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\newgal\dummy\nullvideo.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\dummy\nullvideo.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\src\newgal\blit.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_0.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_1.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_A.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_A.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\blit_N.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\gamma.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\leaks.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\newgal.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\pixels.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\pixels_c.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\RLEaccel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\RLEaccel_c.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\stretch.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\stretch_c.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\surface.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\sysvideo.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\video.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\yuv.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\yuv_mmx.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\yuv_sw.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\yuv_sw_c.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\newgal\yuvfuncs.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\newgal"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
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
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\gui\accelkey.c
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

SOURCE=..\src\gui\element.c
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

SOURCE="..\src\gui\it-kmap.c"
# End Source File
# Begin Source File

SOURCE=..\src\gui\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\src\gui\keyboard.h
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

SOURCE=..\src\misc\math.c
# End Source File
# Begin Source File

SOURCE=..\src\misc\math3d.c
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

SOURCE=..\src\misc\systext.c
# End Source File
# End Group
# Begin Group "mybmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\mybmp\gif.c

!IF  "$(CFG)" == "minigui - Win32 Release"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "debug\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mybmp\jpeg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "debug\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mybmp\mybmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "debug\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mybmp\png.c

!IF  "$(CFG)" == "minigui - Win32 Release"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "debug\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mybmp\winbmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

# PROP Intermediate_Dir "debug\"

!ENDIF 

# End Source File
# End Group
# Begin Group "sysres"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\sysres\mgetc.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\resource.c
# End Source File
# Begin Source File

SOURCE=..\src\sysres\syscfg.c
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
# End Group
# Begin Group "ext"

# PROP Default_Filter ""
# Begin Group "control No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\control\animation.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\animation.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\coolbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\coolbar.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\gif89a.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\gridview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\gridview.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\gridviewcelltype.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\iconview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\iconview.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\initmgext.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\listview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\listview.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\monthcalendar.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\monthcalendar.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\spinbox.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\spinbox.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\treeview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\control\treeview.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "skin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\skin\bmplabel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\bmplabel.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\chkbutton.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\chkbutton.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\cmdbutton.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\cmdbutton.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\item_comm.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\label.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\label.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\mgcontrol.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\mgcontrol.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\rotslider.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\rotslider.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\skin.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\slider.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\skin\slider.h

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "mywins"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\mywins\colordlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\mywins\colorspace.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\mywins\filedlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\mywins\helpwin.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\mywins\mywindows.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ext\mywins\newfiledlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

# PROP Intermediate_Dir "Release\ext"

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ext\include\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\colorspace.h
# End Source File
# Begin Source File

SOURCE=..\include\common.h
# End Source File
# Begin Source File

SOURCE=..\config.h
# End Source File
# Begin Source File

SOURCE=..\include\control.h
# End Source File
# Begin Source File

SOURCE=..\include\endianrw.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\fixedmath.h
# End Source File
# Begin Source File

SOURCE=..\include\gdi.h
# End Source File
# Begin Source File

SOURCE=..\src\include\hash.h
# End Source File
# Begin Source File

SOURCE=..\src\include\internals.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\mgext.h
# End Source File
# Begin Source File

SOURCE=..\include\minigui.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\mywindows.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\newfiledlg.h
# End Source File
# Begin Source File

SOURCE=..\include\png.h
# End Source File
# Begin Source File

SOURCE=..\include\pngconf.h
# End Source File
# Begin Source File

SOURCE=..\ext\include\skin.h
# End Source File
# Begin Source File

SOURCE=..\include\window.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
