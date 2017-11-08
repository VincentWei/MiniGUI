# Microsoft Developer Studio Generated NMAKE File, Based on minigui-win32.dsp
!IF "$(CFG)" == ""
CFG=minigui - Win32 Debug
!MESSAGE No configuration specified. Defaulting to minigui - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "minigui - Win32 Release" && "$(CFG)" != "minigui - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minigui-win32.mak" CFG="minigui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minigui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "minigui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minigui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\minigui.dll"


CLEAN :
	-@erase "$(INTDIR)\12x24-iso8859-1.obj"
	-@erase "$(INTDIR)\6x12-iso8859-1.obj"
	-@erase "$(INTDIR)\8x16-iso8859-1.obj"
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\accelkey.obj"
	-@erase "$(INTDIR)\auto.obj"
	-@erase "$(INTDIR)\big5unimap.obj"
	-@erase "$(INTDIR)\blockheap.obj"
	-@erase "$(INTDIR)\button.obj"
	-@erase "$(INTDIR)\caret.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\combobox.obj"
	-@erase "$(INTDIR)\courier_8x13.obj"
	-@erase "$(INTDIR)\ctrlclass.obj"
	-@erase "$(INTDIR)\ctrlmisc.obj"
	-@erase "$(INTDIR)\cursor.obj"
	-@erase "$(INTDIR)\de-kmap.obj"
	-@erase "$(INTDIR)\de-latin1-kmap.obj"
	-@erase "$(INTDIR)\de-latin1-nodeadkeys-kmap.obj"
	-@erase "$(INTDIR)\defkeymap.obj"
	-@erase "$(INTDIR)\desktop.obj"
	-@erase "$(INTDIR)\devfont.obj"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\dummy.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\element.obj"
	-@erase "$(INTDIR)\endianrw.obj"
	-@erase "$(INTDIR)\es-cp850-kmap.obj"
	-@erase "$(INTDIR)\es-kmap.obj"
	-@erase "$(INTDIR)\euckrunimap.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\ext\vc60.idb"
	-@erase "$(INTDIR)\fixedsys_8x15.obj"
	-@erase "$(INTDIR)\fixstr.obj"
	-@erase "$(INTDIR)\fontname.obj"
	-@erase "$(INTDIR)\fr-kmap.obj"
	-@erase "$(INTDIR)\fr-pc-kmap.obj"
	-@erase "$(INTDIR)\freetype.obj"
	-@erase "$(INTDIR)\freetype2.obj"
	-@erase "$(INTDIR)\gb18030unimap.obj"
	-@erase "$(INTDIR)\gbkunimap.obj"
	-@erase "$(INTDIR)\gbunimap.obj"
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\gothic_12_jiskan.obj"
	-@erase "$(INTDIR)\gothic_14_jiskan.obj"
	-@erase "$(INTDIR)\gothic_16_jiskan.obj"
	-@erase "$(INTDIR)\gothic_6x12rk.obj"
	-@erase "$(INTDIR)\gothic_7x14rk.obj"
	-@erase "$(INTDIR)\gothic_8x16rk.obj"
	-@erase "$(INTDIR)\ial.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\it-kmap.obj"
	-@erase "$(INTDIR)\jisunimap.obj"
	-@erase "$(INTDIR)\jpeg.obj"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\lbm.obj"
	-@erase "$(INTDIR)\listbox.obj"
	-@erase "$(INTDIR)\listmodel.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math3d.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menubutton.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\mgetc.obj"
	-@erase "$(INTDIR)\minigui-win32.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\mybmp.obj"
	-@erase "$(INTDIR)\newgal\vc60.idb"
	-@erase "$(INTDIR)\newgdi\vc60.idb"
	-@erase "$(INTDIR)\newtoolbar.obj"
	-@erase "$(INTDIR)\nposix.obj"
	-@erase "$(INTDIR)\pcx.obj"
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\progressbar.obj"
	-@erase "$(INTDIR)\propsheet.obj"
	-@erase "$(INTDIR)\rawbitmap.obj"
	-@erase "$(INTDIR)\resource.obj"
	-@erase "$(INTDIR)\rwops.obj"
	-@erase "$(INTDIR)\sansserif_11x13.obj"
	-@erase "$(INTDIR)\scrolled.obj"
	-@erase "$(INTDIR)\scrollview.obj"
	-@erase "$(INTDIR)\scrollwnd.obj"
	-@erase "$(INTDIR)\simedit.obj"
	-@erase "$(INTDIR)\sjisunimap.obj"
	-@erase "$(INTDIR)\snprintf.obj"
	-@erase "$(INTDIR)\song-12-gb2312.obj"
	-@erase "$(INTDIR)\song-16-gb2312.obj"
	-@erase "$(INTDIR)\song-24-gb2312.obj"
	-@erase "$(INTDIR)\static.obj"
	-@erase "$(INTDIR)\symb12.obj"
	-@erase "$(INTDIR)\syscfg.obj"
	-@erase "$(INTDIR)\sysfont.obj"
	-@erase "$(INTDIR)\system_14x16.obj"
	-@erase "$(INTDIR)\systext.obj"
	-@erase "$(INTDIR)\terminal_8x12.obj"
	-@erase "$(INTDIR)\textedit.obj"
	-@erase "$(INTDIR)\tga.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\trackbar.obj"
	-@erase "$(INTDIR)\ujisunimap.obj"
	-@erase "$(INTDIR)\varbitmap.obj"
	-@erase "$(INTDIR)\vbf_helvR16.obj"
	-@erase "$(INTDIR)\vbf_helvR21.obj"
	-@erase "$(INTDIR)\vbf_helvR27.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vfnprintf.obj"
	-@erase "$(INTDIR)\vgarom_8x8.obj"
	-@erase "$(INTDIR)\vsnprintf.obj"
	-@erase "$(INTDIR)\winbmp.obj"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winial.obj"
	-@erase "$(INTDIR)\wvfbial.obj"
	-@erase "$(OUTDIR)\minigui.dll"
	-@erase "$(OUTDIR)\minigui.exp"
	-@erase "$(OUTDIR)\minigui.ilk"
	-@erase "$(OUTDIR)\minigui.lib"
	-@erase ".\Release\ext\animation.obj"
	-@erase ".\Release\ext\bmplabel.obj"
	-@erase ".\Release\ext\chkbutton.obj"
	-@erase ".\Release\ext\cmdbutton.obj"
	-@erase ".\Release\ext\colordlg.obj"
	-@erase ".\Release\ext\colorspace.obj"
	-@erase ".\Release\ext\coolbar.obj"
	-@erase ".\Release\ext\filedlg.obj"
	-@erase ".\Release\ext\gif89a.obj"
	-@erase ".\Release\ext\gridview.obj"
	-@erase ".\Release\ext\gridviewcelltype.obj"
	-@erase ".\Release\ext\helpwin.obj"
	-@erase ".\Release\ext\iconview.obj"
	-@erase ".\Release\ext\initmgext.obj"
	-@erase ".\Release\ext\label.obj"
	-@erase ".\Release\ext\listview.obj"
	-@erase ".\Release\ext\mgcontrol.obj"
	-@erase ".\Release\ext\monthcalendar.obj"
	-@erase ".\Release\ext\mywindows.obj"
	-@erase ".\Release\ext\newfiledlg.obj"
	-@erase ".\Release\ext\rotslider.obj"
	-@erase ".\Release\ext\skin.obj"
	-@erase ".\Release\ext\slider.obj"
	-@erase ".\Release\ext\spinbox.obj"
	-@erase ".\Release\ext\treeview.obj"
	-@erase ".\Release\newgal\blit.obj"
	-@erase ".\Release\newgal\blit_0.obj"
	-@erase ".\Release\newgal\blit_1.obj"
	-@erase ".\Release\newgal\blit_A.obj"
	-@erase ".\Release\newgal\blit_N.obj"
	-@erase ".\Release\newgal\gamma.obj"
	-@erase ".\Release\newgal\newgal.obj"
	-@erase ".\Release\newgal\nullvideo.obj"
	-@erase ".\Release\newgal\pixels.obj"
	-@erase ".\Release\newgal\RLEaccel.obj"
	-@erase ".\Release\newgal\stretch.obj"
	-@erase ".\Release\newgal\surface.obj"
	-@erase ".\Release\newgal\video.obj"
	-@erase ".\Release\newgal\winfb.obj"
	-@erase ".\Release\newgal\wvfb.obj"
	-@erase ".\Release\newgal\yuv.obj"
	-@erase ".\Release\newgal\yuv_mmx.obj"
	-@erase ".\Release\newgal\yuv_sw.obj"
	-@erase ".\Release\newgdi\advapi.obj"
	-@erase ".\Release\newgdi\arc.obj"
	-@erase ".\Release\newgdi\attr.obj"
	-@erase ".\Release\newgdi\bitmap.obj"
	-@erase ".\Release\newgdi\clip.obj"
	-@erase ".\Release\newgdi\coor.obj"
	-@erase ".\Release\newgdi\drawtext.obj"
	-@erase ".\Release\newgdi\flood.obj"
	-@erase ".\Release\newgdi\gdi.obj"
	-@erase ".\Release\newgdi\generators.obj"
	-@erase ".\Release\newgdi\icon.obj"
	-@erase ".\Release\newgdi\lbm.obj"
	-@erase ".\Release\newgdi\line.obj"
	-@erase ".\Release\newgdi\logfont.obj"
	-@erase ".\Release\newgdi\map.obj"
	-@erase ".\Release\newgdi\miarc.obj"
	-@erase ".\Release\newgdi\midash.obj"
	-@erase ".\Release\newgdi\mifillarc.obj"
	-@erase ".\Release\newgdi\mifpolycon.obj"
	-@erase ".\Release\newgdi\mispans.obj"
	-@erase ".\Release\newgdi\miwideline.obj"
	-@erase ".\Release\newgdi\palette.obj"
	-@erase ".\Release\newgdi\pcx.obj"
	-@erase ".\Release\newgdi\pixel.obj"
	-@erase ".\Release\newgdi\pixel_ops.obj"
	-@erase ".\Release\newgdi\polygon.obj"
	-@erase ".\Release\newgdi\readbmp.obj"
	-@erase ".\Release\newgdi\rect.obj"
	-@erase ".\Release\newgdi\region.obj"
	-@erase ".\Release\newgdi\screen.obj"
	-@erase ".\Release\newgdi\text.obj"
	-@erase ".\Release\newgdi\tga.obj"
	-@erase ".\Release\newgdi\yuv.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minigui-win32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pthreadVC1.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\minigui.pdb" /machine:I386 /out:"$(OUTDIR)\minigui.dll" /implib:"$(OUTDIR)\minigui.lib" /pdbtype:sept /libpath:"..\..\fhas\build" 
LINK32_OBJS= \
	"$(INTDIR)\button.obj" \
	"$(INTDIR)\combobox.obj" \
	"$(INTDIR)\ctrlmisc.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\listbox.obj" \
	"$(INTDIR)\listmodel.obj" \
	"$(INTDIR)\menubutton.obj" \
	"$(INTDIR)\newtoolbar.obj" \
	"$(INTDIR)\progressbar.obj" \
	"$(INTDIR)\propsheet.obj" \
	"$(INTDIR)\scrolled.obj" \
	"$(INTDIR)\scrollview.obj" \
	"$(INTDIR)\scrollwnd.obj" \
	"$(INTDIR)\simedit.obj" \
	"$(INTDIR)\static.obj" \
	"$(INTDIR)\textedit.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\trackbar.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\minigui-win32.obj" \
	"$(INTDIR)\blockheap.obj" \
	"$(INTDIR)\cursor.obj" \
	"$(INTDIR)\desktop.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\fixstr.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\timer.obj" \
	".\Release\newgdi\advapi.obj" \
	".\Release\newgdi\arc.obj" \
	".\Release\newgdi\attr.obj" \
	".\Release\newgdi\bitmap.obj" \
	".\Release\newgdi\clip.obj" \
	".\Release\newgdi\coor.obj" \
	".\Release\newgdi\drawtext.obj" \
	".\Release\newgdi\flood.obj" \
	".\Release\newgdi\gdi.obj" \
	".\Release\newgdi\generators.obj" \
	".\Release\newgdi\icon.obj" \
	".\Release\newgdi\lbm.obj" \
	".\Release\newgdi\line.obj" \
	".\Release\newgdi\logfont.obj" \
	".\Release\newgdi\map.obj" \
	".\Release\newgdi\miarc.obj" \
	".\Release\newgdi\midash.obj" \
	".\Release\newgdi\mifillarc.obj" \
	".\Release\newgdi\mifpolycon.obj" \
	".\Release\newgdi\mispans.obj" \
	".\Release\newgdi\miwideline.obj" \
	".\Release\newgdi\palette.obj" \
	".\Release\newgdi\pcx.obj" \
	".\Release\newgdi\pixel.obj" \
	".\Release\newgdi\pixel_ops.obj" \
	".\Release\newgdi\polygon.obj" \
	".\Release\newgdi\readbmp.obj" \
	".\Release\newgdi\rect.obj" \
	".\Release\newgdi\region.obj" \
	".\Release\newgdi\screen.obj" \
	".\Release\newgdi\text.obj" \
	".\Release\newgdi\tga.obj" \
	".\Release\newgdi\yuv.obj" \
	"$(INTDIR)\12x24-iso8859-1.obj" \
	"$(INTDIR)\6x12-iso8859-1.obj" \
	"$(INTDIR)\8x16-iso8859-1.obj" \
	"$(INTDIR)\courier_8x13.obj" \
	"$(INTDIR)\fixedsys_8x15.obj" \
	"$(INTDIR)\gothic_12_jiskan.obj" \
	"$(INTDIR)\gothic_14_jiskan.obj" \
	"$(INTDIR)\gothic_16_jiskan.obj" \
	"$(INTDIR)\gothic_6x12rk.obj" \
	"$(INTDIR)\gothic_7x14rk.obj" \
	"$(INTDIR)\gothic_8x16rk.obj" \
	"$(INTDIR)\sansserif_11x13.obj" \
	"$(INTDIR)\song-12-gb2312.obj" \
	"$(INTDIR)\song-16-gb2312.obj" \
	"$(INTDIR)\song-24-gb2312.obj" \
	"$(INTDIR)\symb12.obj" \
	"$(INTDIR)\system_14x16.obj" \
	"$(INTDIR)\terminal_8x12.obj" \
	"$(INTDIR)\vbf_helvR16.obj" \
	"$(INTDIR)\vbf_helvR21.obj" \
	"$(INTDIR)\vbf_helvR27.obj" \
	"$(INTDIR)\vgarom_8x8.obj" \
	"$(INTDIR)\big5unimap.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\devfont.obj" \
	"$(INTDIR)\euckrunimap.obj" \
	"$(INTDIR)\fontname.obj" \
	"$(INTDIR)\freetype.obj" \
	"$(INTDIR)\freetype2.obj" \
	"$(INTDIR)\gb18030unimap.obj" \
	"$(INTDIR)\gbkunimap.obj" \
	"$(INTDIR)\gbunimap.obj" \
	"$(INTDIR)\jisunimap.obj" \
	"$(INTDIR)\rawbitmap.obj" \
	"$(INTDIR)\sjisunimap.obj" \
	"$(INTDIR)\sysfont.obj" \
	"$(INTDIR)\ujisunimap.obj" \
	"$(INTDIR)\varbitmap.obj" \
	".\Release\newgal\winfb.obj" \
	".\Release\newgal\wvfb.obj" \
	".\Release\newgal\nullvideo.obj" \
	".\Release\newgal\blit.obj" \
	".\Release\newgal\blit_0.obj" \
	".\Release\newgal\blit_1.obj" \
	".\Release\newgal\blit_A.obj" \
	".\Release\newgal\blit_N.obj" \
	".\Release\newgal\gamma.obj" \
	".\Release\newgal\newgal.obj" \
	".\Release\newgal\pixels.obj" \
	".\Release\newgal\RLEaccel.obj" \
	".\Release\newgal\stretch.obj" \
	".\Release\newgal\surface.obj" \
	".\Release\newgal\video.obj" \
	".\Release\newgal\yuv.obj" \
	".\Release\newgal\yuv_mmx.obj" \
	".\Release\newgal\yuv_sw.obj" \
	"$(INTDIR)\auto.obj" \
	"$(INTDIR)\dummy.obj" \
	"$(INTDIR)\ial.obj" \
	"$(INTDIR)\winial.obj" \
	"$(INTDIR)\wvfbial.obj" \
	"$(INTDIR)\accelkey.obj" \
	"$(INTDIR)\caret.obj" \
	"$(INTDIR)\ctrlclass.obj" \
	"$(INTDIR)\de-kmap.obj" \
	"$(INTDIR)\de-latin1-kmap.obj" \
	"$(INTDIR)\de-latin1-nodeadkeys-kmap.obj" \
	"$(INTDIR)\defkeymap.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\element.obj" \
	"$(INTDIR)\es-cp850-kmap.obj" \
	"$(INTDIR)\es-kmap.obj" \
	"$(INTDIR)\fr-kmap.obj" \
	"$(INTDIR)\fr-pc-kmap.obj" \
	"$(INTDIR)\it-kmap.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\endianrw.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\math3d.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\nposix.obj" \
	"$(INTDIR)\rwops.obj" \
	"$(INTDIR)\systext.obj" \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\jpeg.obj" \
	"$(INTDIR)\lbm.obj" \
	"$(INTDIR)\mybmp.obj" \
	"$(INTDIR)\pcx.obj" \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\tga.obj" \
	"$(INTDIR)\winbmp.obj" \
	"$(INTDIR)\mgetc.obj" \
	"$(INTDIR)\resource.obj" \
	"$(INTDIR)\syscfg.obj" \
	"$(INTDIR)\snprintf.obj" \
	"$(INTDIR)\vfnprintf.obj" \
	"$(INTDIR)\vsnprintf.obj" \
	".\Release\ext\animation.obj" \
	".\Release\ext\coolbar.obj" \
	".\Release\ext\gif89a.obj" \
	".\Release\ext\gridview.obj" \
	".\Release\ext\gridviewcelltype.obj" \
	".\Release\ext\iconview.obj" \
	".\Release\ext\initmgext.obj" \
	".\Release\ext\listview.obj" \
	".\Release\ext\monthcalendar.obj" \
	".\Release\ext\spinbox.obj" \
	".\Release\ext\treeview.obj" \
	".\Release\ext\bmplabel.obj" \
	".\Release\ext\chkbutton.obj" \
	".\Release\ext\cmdbutton.obj" \
	".\Release\ext\label.obj" \
	".\Release\ext\mgcontrol.obj" \
	".\Release\ext\rotslider.obj" \
	".\Release\ext\skin.obj" \
	".\Release\ext\slider.obj" \
	".\Release\ext\colordlg.obj" \
	".\Release\ext\colorspace.obj" \
	".\Release\ext\filedlg.obj" \
	".\Release\ext\helpwin.obj" \
	".\Release\ext\mywindows.obj" \
	".\Release\ext\newfiledlg.obj"

"$(OUTDIR)\minigui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

OUTDIR=.\..\..\fhas\build
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\fhas\build
# End Custom Macros

ALL : "$(OUTDIR)\minigui.dll" "$(OUTDIR)\minigui-win32.bsc"


CLEAN :
	-@erase "$(INTDIR)\12x24-iso8859-1.obj"
	-@erase "$(INTDIR)\12x24-iso8859-1.sbr"
	-@erase "$(INTDIR)\6x12-iso8859-1.obj"
	-@erase "$(INTDIR)\6x12-iso8859-1.sbr"
	-@erase "$(INTDIR)\8x16-iso8859-1.obj"
	-@erase "$(INTDIR)\8x16-iso8859-1.sbr"
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\about.sbr"
	-@erase "$(INTDIR)\accelkey.obj"
	-@erase "$(INTDIR)\accelkey.sbr"
	-@erase "$(INTDIR)\advapi.obj"
	-@erase "$(INTDIR)\advapi.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\arc.obj"
	-@erase "$(INTDIR)\arc.sbr"
	-@erase "$(INTDIR)\attr.obj"
	-@erase "$(INTDIR)\attr.sbr"
	-@erase "$(INTDIR)\auto.obj"
	-@erase "$(INTDIR)\auto.sbr"
	-@erase "$(INTDIR)\big5unimap.obj"
	-@erase "$(INTDIR)\big5unimap.sbr"
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bitmap.sbr"
	-@erase "$(INTDIR)\blit.obj"
	-@erase "$(INTDIR)\blit.sbr"
	-@erase "$(INTDIR)\blit_0.obj"
	-@erase "$(INTDIR)\blit_0.sbr"
	-@erase "$(INTDIR)\blit_1.obj"
	-@erase "$(INTDIR)\blit_1.sbr"
	-@erase "$(INTDIR)\blit_A.obj"
	-@erase "$(INTDIR)\blit_A.sbr"
	-@erase "$(INTDIR)\blit_N.obj"
	-@erase "$(INTDIR)\blit_N.sbr"
	-@erase "$(INTDIR)\blockheap.obj"
	-@erase "$(INTDIR)\blockheap.sbr"
	-@erase "$(INTDIR)\bmplabel.obj"
	-@erase "$(INTDIR)\bmplabel.sbr"
	-@erase "$(INTDIR)\button.obj"
	-@erase "$(INTDIR)\button.sbr"
	-@erase "$(INTDIR)\caret.obj"
	-@erase "$(INTDIR)\caret.sbr"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\charset.sbr"
	-@erase "$(INTDIR)\chkbutton.obj"
	-@erase "$(INTDIR)\chkbutton.sbr"
	-@erase "$(INTDIR)\clip.obj"
	-@erase "$(INTDIR)\clip.sbr"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\clipboard.sbr"
	-@erase "$(INTDIR)\cmdbutton.obj"
	-@erase "$(INTDIR)\cmdbutton.sbr"
	-@erase "$(INTDIR)\colordlg.obj"
	-@erase "$(INTDIR)\colordlg.sbr"
	-@erase "$(INTDIR)\colorspace.obj"
	-@erase "$(INTDIR)\colorspace.sbr"
	-@erase "$(INTDIR)\combobox.obj"
	-@erase "$(INTDIR)\combobox.sbr"
	-@erase "$(INTDIR)\coolbar.obj"
	-@erase "$(INTDIR)\coolbar.sbr"
	-@erase "$(INTDIR)\coor.obj"
	-@erase "$(INTDIR)\coor.sbr"
	-@erase "$(INTDIR)\courier_8x13.obj"
	-@erase "$(INTDIR)\courier_8x13.sbr"
	-@erase "$(INTDIR)\ctrlclass.obj"
	-@erase "$(INTDIR)\ctrlclass.sbr"
	-@erase "$(INTDIR)\ctrlmisc.obj"
	-@erase "$(INTDIR)\ctrlmisc.sbr"
	-@erase "$(INTDIR)\cursor.obj"
	-@erase "$(INTDIR)\cursor.sbr"
	-@erase "$(INTDIR)\de-kmap.obj"
	-@erase "$(INTDIR)\de-kmap.sbr"
	-@erase "$(INTDIR)\de-latin1-kmap.obj"
	-@erase "$(INTDIR)\de-latin1-kmap.sbr"
	-@erase "$(INTDIR)\de-latin1-nodeadkeys-kmap.obj"
	-@erase "$(INTDIR)\de-latin1-nodeadkeys-kmap.sbr"
	-@erase "$(INTDIR)\defkeymap.obj"
	-@erase "$(INTDIR)\defkeymap.sbr"
	-@erase "$(INTDIR)\desktop.obj"
	-@erase "$(INTDIR)\desktop.sbr"
	-@erase "$(INTDIR)\devfont.obj"
	-@erase "$(INTDIR)\devfont.sbr"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\dialog.sbr"
	-@erase "$(INTDIR)\drawtext.obj"
	-@erase "$(INTDIR)\drawtext.sbr"
	-@erase "$(INTDIR)\dummy.obj"
	-@erase "$(INTDIR)\dummy.sbr"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\edit.sbr"
	-@erase "$(INTDIR)\element.obj"
	-@erase "$(INTDIR)\element.sbr"
	-@erase "$(INTDIR)\endianrw.obj"
	-@erase "$(INTDIR)\endianrw.sbr"
	-@erase "$(INTDIR)\es-cp850-kmap.obj"
	-@erase "$(INTDIR)\es-cp850-kmap.sbr"
	-@erase "$(INTDIR)\es-kmap.obj"
	-@erase "$(INTDIR)\es-kmap.sbr"
	-@erase "$(INTDIR)\euckrunimap.obj"
	-@erase "$(INTDIR)\euckrunimap.sbr"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\event.sbr"
	-@erase "$(INTDIR)\filedlg.obj"
	-@erase "$(INTDIR)\filedlg.sbr"
	-@erase "$(INTDIR)\fixedsys_8x15.obj"
	-@erase "$(INTDIR)\fixedsys_8x15.sbr"
	-@erase "$(INTDIR)\fixstr.obj"
	-@erase "$(INTDIR)\fixstr.sbr"
	-@erase "$(INTDIR)\flood.obj"
	-@erase "$(INTDIR)\flood.sbr"
	-@erase "$(INTDIR)\fontname.obj"
	-@erase "$(INTDIR)\fontname.sbr"
	-@erase "$(INTDIR)\fr-kmap.obj"
	-@erase "$(INTDIR)\fr-kmap.sbr"
	-@erase "$(INTDIR)\fr-pc-kmap.obj"
	-@erase "$(INTDIR)\fr-pc-kmap.sbr"
	-@erase "$(INTDIR)\freetype.obj"
	-@erase "$(INTDIR)\freetype.sbr"
	-@erase "$(INTDIR)\freetype2.obj"
	-@erase "$(INTDIR)\freetype2.sbr"
	-@erase "$(INTDIR)\gamma.obj"
	-@erase "$(INTDIR)\gamma.sbr"
	-@erase "$(INTDIR)\gb18030unimap.obj"
	-@erase "$(INTDIR)\gb18030unimap.sbr"
	-@erase "$(INTDIR)\gbkunimap.obj"
	-@erase "$(INTDIR)\gbkunimap.sbr"
	-@erase "$(INTDIR)\gbunimap.obj"
	-@erase "$(INTDIR)\gbunimap.sbr"
	-@erase "$(INTDIR)\gdi.obj"
	-@erase "$(INTDIR)\gdi.sbr"
	-@erase "$(INTDIR)\generators.obj"
	-@erase "$(INTDIR)\generators.sbr"
	-@erase "$(INTDIR)\gif89a.obj"
	-@erase "$(INTDIR)\gif89a.sbr"
	-@erase "$(INTDIR)\gothic_12_jiskan.obj"
	-@erase "$(INTDIR)\gothic_12_jiskan.sbr"
	-@erase "$(INTDIR)\gothic_14_jiskan.obj"
	-@erase "$(INTDIR)\gothic_14_jiskan.sbr"
	-@erase "$(INTDIR)\gothic_16_jiskan.obj"
	-@erase "$(INTDIR)\gothic_16_jiskan.sbr"
	-@erase "$(INTDIR)\gothic_6x12rk.obj"
	-@erase "$(INTDIR)\gothic_6x12rk.sbr"
	-@erase "$(INTDIR)\gothic_7x14rk.obj"
	-@erase "$(INTDIR)\gothic_7x14rk.sbr"
	-@erase "$(INTDIR)\gothic_8x16rk.obj"
	-@erase "$(INTDIR)\gothic_8x16rk.sbr"
	-@erase "$(INTDIR)\gridview.obj"
	-@erase "$(INTDIR)\gridview.sbr"
	-@erase "$(INTDIR)\gridviewcelltype.obj"
	-@erase "$(INTDIR)\gridviewcelltype.sbr"
	-@erase "$(INTDIR)\helpwin.obj"
	-@erase "$(INTDIR)\helpwin.sbr"
	-@erase "$(INTDIR)\ial.obj"
	-@erase "$(INTDIR)\ial.sbr"
	-@erase "$(INTDIR)\icon.obj"
	-@erase "$(INTDIR)\icon.sbr"
	-@erase "$(INTDIR)\iconview.obj"
	-@erase "$(INTDIR)\iconview.sbr"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\init.sbr"
	-@erase "$(INTDIR)\initmgext.obj"
	-@erase "$(INTDIR)\initmgext.sbr"
	-@erase "$(INTDIR)\it-kmap.obj"
	-@erase "$(INTDIR)\it-kmap.sbr"
	-@erase "$(INTDIR)\jisunimap.obj"
	-@erase "$(INTDIR)\jisunimap.sbr"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\keyboard.sbr"
	-@erase "$(INTDIR)\label.obj"
	-@erase "$(INTDIR)\label.sbr"
	-@erase "$(INTDIR)\line.obj"
	-@erase "$(INTDIR)\line.sbr"
	-@erase "$(INTDIR)\listbox.obj"
	-@erase "$(INTDIR)\listbox.sbr"
	-@erase "$(INTDIR)\listmodel.obj"
	-@erase "$(INTDIR)\listmodel.sbr"
	-@erase "$(INTDIR)\listview.obj"
	-@erase "$(INTDIR)\listview.sbr"
	-@erase "$(INTDIR)\logfont.obj"
	-@erase "$(INTDIR)\logfont.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\math3d.obj"
	-@erase "$(INTDIR)\math3d.sbr"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menu.sbr"
	-@erase "$(INTDIR)\menubutton.obj"
	-@erase "$(INTDIR)\menubutton.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\mgcontrol.obj"
	-@erase "$(INTDIR)\mgcontrol.sbr"
	-@erase "$(INTDIR)\mgetc.obj"
	-@erase "$(INTDIR)\mgetc.sbr"
	-@erase "$(INTDIR)\miarc.obj"
	-@erase "$(INTDIR)\miarc.sbr"
	-@erase "$(INTDIR)\midash.obj"
	-@erase "$(INTDIR)\midash.sbr"
	-@erase "$(INTDIR)\mifillarc.obj"
	-@erase "$(INTDIR)\mifillarc.sbr"
	-@erase "$(INTDIR)\mifpolycon.obj"
	-@erase "$(INTDIR)\mifpolycon.sbr"
	-@erase "$(INTDIR)\minigui-win32.obj"
	-@erase "$(INTDIR)\minigui-win32.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\mispans.obj"
	-@erase "$(INTDIR)\mispans.sbr"
	-@erase "$(INTDIR)\miwideline.obj"
	-@erase "$(INTDIR)\miwideline.sbr"
	-@erase "$(INTDIR)\monthcalendar.obj"
	-@erase "$(INTDIR)\monthcalendar.sbr"
	-@erase "$(INTDIR)\mywindows.obj"
	-@erase "$(INTDIR)\mywindows.sbr"
	-@erase "$(INTDIR)\newfiledlg.obj"
	-@erase "$(INTDIR)\newfiledlg.sbr"
	-@erase "$(INTDIR)\newgal.obj"
	-@erase "$(INTDIR)\newgal.sbr"
	-@erase "$(INTDIR)\newgdi\vc60.idb"
	-@erase "$(INTDIR)\newgdi\vc60.pdb"
	-@erase "$(INTDIR)\newtoolbar.obj"
	-@erase "$(INTDIR)\newtoolbar.sbr"
	-@erase "$(INTDIR)\nposix.obj"
	-@erase "$(INTDIR)\nposix.sbr"
	-@erase "$(INTDIR)\nullvideo.obj"
	-@erase "$(INTDIR)\nullvideo.sbr"
	-@erase "$(INTDIR)\palette.obj"
	-@erase "$(INTDIR)\palette.sbr"
	-@erase "$(INTDIR)\pixel.obj"
	-@erase "$(INTDIR)\pixel.sbr"
	-@erase "$(INTDIR)\pixel_ops.obj"
	-@erase "$(INTDIR)\pixel_ops.sbr"
	-@erase "$(INTDIR)\pixels.obj"
	-@erase "$(INTDIR)\pixels.sbr"
	-@erase "$(INTDIR)\polygon.obj"
	-@erase "$(INTDIR)\polygon.sbr"
	-@erase "$(INTDIR)\progressbar.obj"
	-@erase "$(INTDIR)\progressbar.sbr"
	-@erase "$(INTDIR)\propsheet.obj"
	-@erase "$(INTDIR)\propsheet.sbr"
	-@erase "$(INTDIR)\rawbitmap.obj"
	-@erase "$(INTDIR)\rawbitmap.sbr"
	-@erase "$(INTDIR)\readbmp.obj"
	-@erase "$(INTDIR)\readbmp.sbr"
	-@erase "$(INTDIR)\rect.obj"
	-@erase "$(INTDIR)\rect.sbr"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\region.sbr"
	-@erase "$(INTDIR)\resource.obj"
	-@erase "$(INTDIR)\resource.sbr"
	-@erase "$(INTDIR)\RLEaccel.obj"
	-@erase "$(INTDIR)\RLEaccel.sbr"
	-@erase "$(INTDIR)\rotslider.obj"
	-@erase "$(INTDIR)\rotslider.sbr"
	-@erase "$(INTDIR)\rwops.obj"
	-@erase "$(INTDIR)\rwops.sbr"
	-@erase "$(INTDIR)\sansserif_11x13.obj"
	-@erase "$(INTDIR)\sansserif_11x13.sbr"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\screen.sbr"
	-@erase "$(INTDIR)\scrolled.obj"
	-@erase "$(INTDIR)\scrolled.sbr"
	-@erase "$(INTDIR)\scrollview.obj"
	-@erase "$(INTDIR)\scrollview.sbr"
	-@erase "$(INTDIR)\scrollwnd.obj"
	-@erase "$(INTDIR)\scrollwnd.sbr"
	-@erase "$(INTDIR)\simedit.obj"
	-@erase "$(INTDIR)\simedit.sbr"
	-@erase "$(INTDIR)\sjisunimap.obj"
	-@erase "$(INTDIR)\sjisunimap.sbr"
	-@erase "$(INTDIR)\skin.obj"
	-@erase "$(INTDIR)\skin.sbr"
	-@erase "$(INTDIR)\slider.obj"
	-@erase "$(INTDIR)\slider.sbr"
	-@erase "$(INTDIR)\snprintf.obj"
	-@erase "$(INTDIR)\snprintf.sbr"
	-@erase "$(INTDIR)\song-12-gb2312.obj"
	-@erase "$(INTDIR)\song-12-gb2312.sbr"
	-@erase "$(INTDIR)\song-16-gb2312.obj"
	-@erase "$(INTDIR)\song-16-gb2312.sbr"
	-@erase "$(INTDIR)\song-24-gb2312.obj"
	-@erase "$(INTDIR)\song-24-gb2312.sbr"
	-@erase "$(INTDIR)\spinbox.obj"
	-@erase "$(INTDIR)\spinbox.sbr"
	-@erase "$(INTDIR)\static.obj"
	-@erase "$(INTDIR)\static.sbr"
	-@erase "$(INTDIR)\stretch.obj"
	-@erase "$(INTDIR)\stretch.sbr"
	-@erase "$(INTDIR)\surface.obj"
	-@erase "$(INTDIR)\surface.sbr"
	-@erase "$(INTDIR)\symb12.obj"
	-@erase "$(INTDIR)\symb12.sbr"
	-@erase "$(INTDIR)\syscfg.obj"
	-@erase "$(INTDIR)\syscfg.sbr"
	-@erase "$(INTDIR)\sysfont.obj"
	-@erase "$(INTDIR)\sysfont.sbr"
	-@erase "$(INTDIR)\system_14x16.obj"
	-@erase "$(INTDIR)\system_14x16.sbr"
	-@erase "$(INTDIR)\systext.obj"
	-@erase "$(INTDIR)\systext.sbr"
	-@erase "$(INTDIR)\terminal_8x12.obj"
	-@erase "$(INTDIR)\terminal_8x12.sbr"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\text.sbr"
	-@erase "$(INTDIR)\textedit.obj"
	-@erase "$(INTDIR)\textedit.sbr"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\timer.sbr"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\toolbar.sbr"
	-@erase "$(INTDIR)\trackbar.obj"
	-@erase "$(INTDIR)\trackbar.sbr"
	-@erase "$(INTDIR)\treeview.obj"
	-@erase "$(INTDIR)\treeview.sbr"
	-@erase "$(INTDIR)\ujisunimap.obj"
	-@erase "$(INTDIR)\ujisunimap.sbr"
	-@erase "$(INTDIR)\varbitmap.obj"
	-@erase "$(INTDIR)\varbitmap.sbr"
	-@erase "$(INTDIR)\vbf_helvR16.obj"
	-@erase "$(INTDIR)\vbf_helvR16.sbr"
	-@erase "$(INTDIR)\vbf_helvR21.obj"
	-@erase "$(INTDIR)\vbf_helvR21.sbr"
	-@erase "$(INTDIR)\vbf_helvR27.obj"
	-@erase "$(INTDIR)\vbf_helvR27.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vfnprintf.obj"
	-@erase "$(INTDIR)\vfnprintf.sbr"
	-@erase "$(INTDIR)\vgarom_8x8.obj"
	-@erase "$(INTDIR)\vgarom_8x8.sbr"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\video.sbr"
	-@erase "$(INTDIR)\vsnprintf.obj"
	-@erase "$(INTDIR)\vsnprintf.sbr"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winfb.obj"
	-@erase "$(INTDIR)\winfb.sbr"
	-@erase "$(INTDIR)\winial.obj"
	-@erase "$(INTDIR)\winial.sbr"
	-@erase "$(INTDIR)\wvfb.obj"
	-@erase "$(INTDIR)\wvfb.sbr"
	-@erase "$(INTDIR)\wvfbial.obj"
	-@erase "$(INTDIR)\wvfbial.sbr"
	-@erase "$(INTDIR)\yuv.obj"
	-@erase "$(INTDIR)\yuv.sbr"
	-@erase "$(INTDIR)\yuv_mmx.obj"
	-@erase "$(INTDIR)\yuv_mmx.sbr"
	-@erase "$(INTDIR)\yuv_sw.obj"
	-@erase "$(INTDIR)\yuv_sw.sbr"
	-@erase "$(OUTDIR)\minigui-win32.bsc"
	-@erase "$(OUTDIR)\minigui.dll"
	-@erase "$(OUTDIR)\minigui.exp"
	-@erase "$(OUTDIR)\minigui.ilk"
	-@erase "$(OUTDIR)\minigui.lib"
	-@erase "$(OUTDIR)\minigui.pdb"
	-@erase ".\debug\gif.obj"
	-@erase ".\debug\gif.sbr"
	-@erase ".\debug\jpeg.obj"
	-@erase ".\debug\jpeg.sbr"
	-@erase ".\Debug\lbm.obj"
	-@erase ".\Debug\lbm.sbr"
	-@erase ".\debug\mybmp.obj"
	-@erase ".\debug\mybmp.sbr"
	-@erase ".\Debug\newgdi\yuv.obj"
	-@erase ".\Debug\pcx.obj"
	-@erase ".\Debug\pcx.sbr"
	-@erase ".\debug\png.obj"
	-@erase ".\debug\png.sbr"
	-@erase ".\Debug\tga.obj"
	-@erase ".\Debug\tga.sbr"
	-@erase ".\debug\winbmp.obj"
	-@erase ".\debug\winbmp.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minigui-win32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\button.sbr" \
	"$(INTDIR)\combobox.sbr" \
	"$(INTDIR)\ctrlmisc.sbr" \
	"$(INTDIR)\edit.sbr" \
	"$(INTDIR)\listbox.sbr" \
	"$(INTDIR)\listmodel.sbr" \
	"$(INTDIR)\menubutton.sbr" \
	"$(INTDIR)\newtoolbar.sbr" \
	"$(INTDIR)\progressbar.sbr" \
	"$(INTDIR)\propsheet.sbr" \
	"$(INTDIR)\scrolled.sbr" \
	"$(INTDIR)\scrollview.sbr" \
	"$(INTDIR)\scrollwnd.sbr" \
	"$(INTDIR)\simedit.sbr" \
	"$(INTDIR)\static.sbr" \
	"$(INTDIR)\textedit.sbr" \
	"$(INTDIR)\toolbar.sbr" \
	"$(INTDIR)\trackbar.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\minigui-win32.sbr" \
	"$(INTDIR)\blockheap.sbr" \
	"$(INTDIR)\cursor.sbr" \
	"$(INTDIR)\desktop.sbr" \
	"$(INTDIR)\event.sbr" \
	"$(INTDIR)\fixstr.sbr" \
	"$(INTDIR)\init.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\timer.sbr" \
	"$(INTDIR)\advapi.sbr" \
	"$(INTDIR)\arc.sbr" \
	"$(INTDIR)\attr.sbr" \
	"$(INTDIR)\bitmap.sbr" \
	"$(INTDIR)\clip.sbr" \
	"$(INTDIR)\coor.sbr" \
	"$(INTDIR)\drawtext.sbr" \
	"$(INTDIR)\flood.sbr" \
	"$(INTDIR)\gdi.sbr" \
	"$(INTDIR)\generators.sbr" \
	"$(INTDIR)\icon.sbr" \
	".\Debug\lbm.sbr" \
	"$(INTDIR)\line.sbr" \
	"$(INTDIR)\logfont.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\miarc.sbr" \
	"$(INTDIR)\midash.sbr" \
	"$(INTDIR)\mifillarc.sbr" \
	"$(INTDIR)\mifpolycon.sbr" \
	"$(INTDIR)\mispans.sbr" \
	"$(INTDIR)\miwideline.sbr" \
	"$(INTDIR)\palette.sbr" \
	".\Debug\pcx.sbr" \
	"$(INTDIR)\pixel.sbr" \
	"$(INTDIR)\pixel_ops.sbr" \
	"$(INTDIR)\polygon.sbr" \
	"$(INTDIR)\readbmp.sbr" \
	"$(INTDIR)\rect.sbr" \
	"$(INTDIR)\region.sbr" \
	"$(INTDIR)\screen.sbr" \
	"$(INTDIR)\text.sbr" \
	".\Debug\tga.sbr" \
	"$(INTDIR)\12x24-iso8859-1.sbr" \
	"$(INTDIR)\6x12-iso8859-1.sbr" \
	"$(INTDIR)\8x16-iso8859-1.sbr" \
	"$(INTDIR)\courier_8x13.sbr" \
	"$(INTDIR)\fixedsys_8x15.sbr" \
	"$(INTDIR)\gothic_12_jiskan.sbr" \
	"$(INTDIR)\gothic_14_jiskan.sbr" \
	"$(INTDIR)\gothic_16_jiskan.sbr" \
	"$(INTDIR)\gothic_6x12rk.sbr" \
	"$(INTDIR)\gothic_7x14rk.sbr" \
	"$(INTDIR)\gothic_8x16rk.sbr" \
	"$(INTDIR)\sansserif_11x13.sbr" \
	"$(INTDIR)\song-12-gb2312.sbr" \
	"$(INTDIR)\song-16-gb2312.sbr" \
	"$(INTDIR)\song-24-gb2312.sbr" \
	"$(INTDIR)\symb12.sbr" \
	"$(INTDIR)\system_14x16.sbr" \
	"$(INTDIR)\terminal_8x12.sbr" \
	"$(INTDIR)\vbf_helvR16.sbr" \
	"$(INTDIR)\vbf_helvR21.sbr" \
	"$(INTDIR)\vbf_helvR27.sbr" \
	"$(INTDIR)\vgarom_8x8.sbr" \
	"$(INTDIR)\big5unimap.sbr" \
	"$(INTDIR)\charset.sbr" \
	"$(INTDIR)\devfont.sbr" \
	"$(INTDIR)\euckrunimap.sbr" \
	"$(INTDIR)\fontname.sbr" \
	"$(INTDIR)\freetype.sbr" \
	"$(INTDIR)\freetype2.sbr" \
	"$(INTDIR)\gb18030unimap.sbr" \
	"$(INTDIR)\gbkunimap.sbr" \
	"$(INTDIR)\gbunimap.sbr" \
	"$(INTDIR)\jisunimap.sbr" \
	"$(INTDIR)\rawbitmap.sbr" \
	"$(INTDIR)\sjisunimap.sbr" \
	"$(INTDIR)\sysfont.sbr" \
	"$(INTDIR)\ujisunimap.sbr" \
	"$(INTDIR)\varbitmap.sbr" \
	"$(INTDIR)\winfb.sbr" \
	"$(INTDIR)\wvfb.sbr" \
	"$(INTDIR)\nullvideo.sbr" \
	"$(INTDIR)\blit.sbr" \
	"$(INTDIR)\blit_0.sbr" \
	"$(INTDIR)\blit_1.sbr" \
	"$(INTDIR)\blit_A.sbr" \
	"$(INTDIR)\blit_N.sbr" \
	"$(INTDIR)\gamma.sbr" \
	"$(INTDIR)\newgal.sbr" \
	"$(INTDIR)\pixels.sbr" \
	"$(INTDIR)\RLEaccel.sbr" \
	"$(INTDIR)\stretch.sbr" \
	"$(INTDIR)\surface.sbr" \
	"$(INTDIR)\video.sbr" \
	"$(INTDIR)\yuv.sbr" \
	"$(INTDIR)\yuv_mmx.sbr" \
	"$(INTDIR)\yuv_sw.sbr" \
	"$(INTDIR)\auto.sbr" \
	"$(INTDIR)\dummy.sbr" \
	"$(INTDIR)\ial.sbr" \
	"$(INTDIR)\winial.sbr" \
	"$(INTDIR)\wvfbial.sbr" \
	"$(INTDIR)\accelkey.sbr" \
	"$(INTDIR)\caret.sbr" \
	"$(INTDIR)\ctrlclass.sbr" \
	"$(INTDIR)\de-kmap.sbr" \
	"$(INTDIR)\de-latin1-kmap.sbr" \
	"$(INTDIR)\de-latin1-nodeadkeys-kmap.sbr" \
	"$(INTDIR)\defkeymap.sbr" \
	"$(INTDIR)\dialog.sbr" \
	"$(INTDIR)\element.sbr" \
	"$(INTDIR)\es-cp850-kmap.sbr" \
	"$(INTDIR)\es-kmap.sbr" \
	"$(INTDIR)\fr-kmap.sbr" \
	"$(INTDIR)\fr-pc-kmap.sbr" \
	"$(INTDIR)\it-kmap.sbr" \
	"$(INTDIR)\keyboard.sbr" \
	"$(INTDIR)\menu.sbr" \
	"$(INTDIR)\window.sbr" \
	"$(INTDIR)\about.sbr" \
	"$(INTDIR)\clipboard.sbr" \
	"$(INTDIR)\endianrw.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\math3d.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\nposix.sbr" \
	"$(INTDIR)\rwops.sbr" \
	"$(INTDIR)\systext.sbr" \
	"$(INTDIR)\mgetc.sbr" \
	"$(INTDIR)\resource.sbr" \
	"$(INTDIR)\syscfg.sbr" \
	"$(INTDIR)\snprintf.sbr" \
	"$(INTDIR)\vfnprintf.sbr" \
	"$(INTDIR)\vsnprintf.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\coolbar.sbr" \
	"$(INTDIR)\gif89a.sbr" \
	"$(INTDIR)\gridview.sbr" \
	"$(INTDIR)\gridviewcelltype.sbr" \
	"$(INTDIR)\iconview.sbr" \
	"$(INTDIR)\initmgext.sbr" \
	"$(INTDIR)\listview.sbr" \
	"$(INTDIR)\monthcalendar.sbr" \
	"$(INTDIR)\spinbox.sbr" \
	"$(INTDIR)\treeview.sbr" \
	"$(INTDIR)\bmplabel.sbr" \
	"$(INTDIR)\chkbutton.sbr" \
	"$(INTDIR)\cmdbutton.sbr" \
	"$(INTDIR)\label.sbr" \
	"$(INTDIR)\mgcontrol.sbr" \
	"$(INTDIR)\rotslider.sbr" \
	"$(INTDIR)\skin.sbr" \
	"$(INTDIR)\slider.sbr" \
	"$(INTDIR)\colordlg.sbr" \
	"$(INTDIR)\colorspace.sbr" \
	"$(INTDIR)\filedlg.sbr" \
	"$(INTDIR)\helpwin.sbr" \
	"$(INTDIR)\mywindows.sbr" \
	"$(INTDIR)\newfiledlg.sbr" \
	".\debug\gif.sbr" \
	".\debug\jpeg.sbr" \
	".\debug\mybmp.sbr" \
	".\debug\png.sbr" \
	".\debug\winbmp.sbr"

"$(OUTDIR)\minigui-win32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib pthreadVC1.lib /nologo /verbose /dll /incremental:yes /pdb:"$(OUTDIR)\minigui.pdb" /debug /machine:I386 /out:"$(OUTDIR)\minigui.dll" /implib:"$(OUTDIR)\minigui.lib" /pdbtype:sept /libpath:"..\..\fhas\build" 
LINK32_OBJS= \
	"$(INTDIR)\button.obj" \
	"$(INTDIR)\combobox.obj" \
	"$(INTDIR)\ctrlmisc.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\listbox.obj" \
	"$(INTDIR)\listmodel.obj" \
	"$(INTDIR)\menubutton.obj" \
	"$(INTDIR)\newtoolbar.obj" \
	"$(INTDIR)\progressbar.obj" \
	"$(INTDIR)\propsheet.obj" \
	"$(INTDIR)\scrolled.obj" \
	"$(INTDIR)\scrollview.obj" \
	"$(INTDIR)\scrollwnd.obj" \
	"$(INTDIR)\simedit.obj" \
	"$(INTDIR)\static.obj" \
	"$(INTDIR)\textedit.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\trackbar.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\minigui-win32.obj" \
	"$(INTDIR)\blockheap.obj" \
	"$(INTDIR)\cursor.obj" \
	"$(INTDIR)\desktop.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\fixstr.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\advapi.obj" \
	"$(INTDIR)\arc.obj" \
	"$(INTDIR)\attr.obj" \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\clip.obj" \
	"$(INTDIR)\coor.obj" \
	"$(INTDIR)\drawtext.obj" \
	"$(INTDIR)\flood.obj" \
	"$(INTDIR)\gdi.obj" \
	"$(INTDIR)\generators.obj" \
	"$(INTDIR)\icon.obj" \
	".\Debug\lbm.obj" \
	"$(INTDIR)\line.obj" \
	"$(INTDIR)\logfont.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\miarc.obj" \
	"$(INTDIR)\midash.obj" \
	"$(INTDIR)\mifillarc.obj" \
	"$(INTDIR)\mifpolycon.obj" \
	"$(INTDIR)\mispans.obj" \
	"$(INTDIR)\miwideline.obj" \
	"$(INTDIR)\palette.obj" \
	".\Debug\pcx.obj" \
	"$(INTDIR)\pixel.obj" \
	"$(INTDIR)\pixel_ops.obj" \
	"$(INTDIR)\polygon.obj" \
	"$(INTDIR)\readbmp.obj" \
	"$(INTDIR)\rect.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\text.obj" \
	".\Debug\tga.obj" \
	".\Debug\newgdi\yuv.obj" \
	"$(INTDIR)\12x24-iso8859-1.obj" \
	"$(INTDIR)\6x12-iso8859-1.obj" \
	"$(INTDIR)\8x16-iso8859-1.obj" \
	"$(INTDIR)\courier_8x13.obj" \
	"$(INTDIR)\fixedsys_8x15.obj" \
	"$(INTDIR)\gothic_12_jiskan.obj" \
	"$(INTDIR)\gothic_14_jiskan.obj" \
	"$(INTDIR)\gothic_16_jiskan.obj" \
	"$(INTDIR)\gothic_6x12rk.obj" \
	"$(INTDIR)\gothic_7x14rk.obj" \
	"$(INTDIR)\gothic_8x16rk.obj" \
	"$(INTDIR)\sansserif_11x13.obj" \
	"$(INTDIR)\song-12-gb2312.obj" \
	"$(INTDIR)\song-16-gb2312.obj" \
	"$(INTDIR)\song-24-gb2312.obj" \
	"$(INTDIR)\symb12.obj" \
	"$(INTDIR)\system_14x16.obj" \
	"$(INTDIR)\terminal_8x12.obj" \
	"$(INTDIR)\vbf_helvR16.obj" \
	"$(INTDIR)\vbf_helvR21.obj" \
	"$(INTDIR)\vbf_helvR27.obj" \
	"$(INTDIR)\vgarom_8x8.obj" \
	"$(INTDIR)\big5unimap.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\devfont.obj" \
	"$(INTDIR)\euckrunimap.obj" \
	"$(INTDIR)\fontname.obj" \
	"$(INTDIR)\freetype.obj" \
	"$(INTDIR)\freetype2.obj" \
	"$(INTDIR)\gb18030unimap.obj" \
	"$(INTDIR)\gbkunimap.obj" \
	"$(INTDIR)\gbunimap.obj" \
	"$(INTDIR)\jisunimap.obj" \
	"$(INTDIR)\rawbitmap.obj" \
	"$(INTDIR)\sjisunimap.obj" \
	"$(INTDIR)\sysfont.obj" \
	"$(INTDIR)\ujisunimap.obj" \
	"$(INTDIR)\varbitmap.obj" \
	"$(INTDIR)\winfb.obj" \
	"$(INTDIR)\wvfb.obj" \
	"$(INTDIR)\nullvideo.obj" \
	"$(INTDIR)\blit.obj" \
	"$(INTDIR)\blit_0.obj" \
	"$(INTDIR)\blit_1.obj" \
	"$(INTDIR)\blit_A.obj" \
	"$(INTDIR)\blit_N.obj" \
	"$(INTDIR)\gamma.obj" \
	"$(INTDIR)\newgal.obj" \
	"$(INTDIR)\pixels.obj" \
	"$(INTDIR)\RLEaccel.obj" \
	"$(INTDIR)\stretch.obj" \
	"$(INTDIR)\surface.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\yuv.obj" \
	"$(INTDIR)\yuv_mmx.obj" \
	"$(INTDIR)\yuv_sw.obj" \
	"$(INTDIR)\auto.obj" \
	"$(INTDIR)\dummy.obj" \
	"$(INTDIR)\ial.obj" \
	"$(INTDIR)\winial.obj" \
	"$(INTDIR)\wvfbial.obj" \
	"$(INTDIR)\accelkey.obj" \
	"$(INTDIR)\caret.obj" \
	"$(INTDIR)\ctrlclass.obj" \
	"$(INTDIR)\de-kmap.obj" \
	"$(INTDIR)\de-latin1-kmap.obj" \
	"$(INTDIR)\de-latin1-nodeadkeys-kmap.obj" \
	"$(INTDIR)\defkeymap.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\element.obj" \
	"$(INTDIR)\es-cp850-kmap.obj" \
	"$(INTDIR)\es-kmap.obj" \
	"$(INTDIR)\fr-kmap.obj" \
	"$(INTDIR)\fr-pc-kmap.obj" \
	"$(INTDIR)\it-kmap.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\endianrw.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\math3d.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\nposix.obj" \
	"$(INTDIR)\rwops.obj" \
	"$(INTDIR)\systext.obj" \
	"$(INTDIR)\mgetc.obj" \
	"$(INTDIR)\resource.obj" \
	"$(INTDIR)\syscfg.obj" \
	"$(INTDIR)\snprintf.obj" \
	"$(INTDIR)\vfnprintf.obj" \
	"$(INTDIR)\vsnprintf.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\coolbar.obj" \
	"$(INTDIR)\gif89a.obj" \
	"$(INTDIR)\gridview.obj" \
	"$(INTDIR)\gridviewcelltype.obj" \
	"$(INTDIR)\iconview.obj" \
	"$(INTDIR)\initmgext.obj" \
	"$(INTDIR)\listview.obj" \
	"$(INTDIR)\monthcalendar.obj" \
	"$(INTDIR)\spinbox.obj" \
	"$(INTDIR)\treeview.obj" \
	"$(INTDIR)\bmplabel.obj" \
	"$(INTDIR)\chkbutton.obj" \
	"$(INTDIR)\cmdbutton.obj" \
	"$(INTDIR)\label.obj" \
	"$(INTDIR)\mgcontrol.obj" \
	"$(INTDIR)\rotslider.obj" \
	"$(INTDIR)\skin.obj" \
	"$(INTDIR)\slider.obj" \
	"$(INTDIR)\colordlg.obj" \
	"$(INTDIR)\colorspace.obj" \
	"$(INTDIR)\filedlg.obj" \
	"$(INTDIR)\helpwin.obj" \
	"$(INTDIR)\mywindows.obj" \
	"$(INTDIR)\newfiledlg.obj" \
	".\debug\gif.obj" \
	".\debug\jpeg.obj" \
	".\debug\mybmp.obj" \
	".\debug\png.obj" \
	".\debug\winbmp.obj"

"$(OUTDIR)\minigui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("minigui-win32.dep")
!INCLUDE "minigui-win32.dep"
!ELSE 
!MESSAGE Warning: cannot find "minigui-win32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "minigui - Win32 Release" || "$(CFG)" == "minigui - Win32 Debug"
SOURCE=..\src\control\button.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\button.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\button.obj"	"$(INTDIR)\button.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\combobox.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\combobox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\combobox.obj"	"$(INTDIR)\combobox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\ctrlmisc.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\ctrlmisc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\ctrlmisc.obj"	"$(INTDIR)\ctrlmisc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\edit.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\edit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\edit.obj"	"$(INTDIR)\edit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\listbox.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\listbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\listbox.obj"	"$(INTDIR)\listbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\listmodel.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\listmodel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\listmodel.obj"	"$(INTDIR)\listmodel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\menubutton.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\menubutton.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\menubutton.obj"	"$(INTDIR)\menubutton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\newtoolbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\newtoolbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\newtoolbar.obj"	"$(INTDIR)\newtoolbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\progressbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\progressbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\progressbar.obj"	"$(INTDIR)\progressbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\propsheet.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\propsheet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\propsheet.obj"	"$(INTDIR)\propsheet.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\scrolled.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\scrolled.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\scrolled.obj"	"$(INTDIR)\scrolled.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\scrollview.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\scrollview.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\scrollview.obj"	"$(INTDIR)\scrollview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\scrollwnd.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\scrollwnd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\scrollwnd.obj"	"$(INTDIR)\scrollwnd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\simedit.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\simedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\simedit.obj"	"$(INTDIR)\simedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\static.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\static.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\static.obj"	"$(INTDIR)\static.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\textedit.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\textedit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\textedit.obj"	"$(INTDIR)\textedit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\toolbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\toolbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\toolbar.obj"	"$(INTDIR)\toolbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\control\trackbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\trackbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\trackbar.obj"	"$(INTDIR)\trackbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\main\main.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\main\minigui-win32.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\minigui-win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\minigui-win32.obj"	"$(INTDIR)\minigui-win32.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\blockheap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\blockheap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\blockheap.obj"	"$(INTDIR)\blockheap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\cursor.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\cursor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\cursor.obj"	"$(INTDIR)\cursor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\desktop.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\desktop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\desktop.obj"	"$(INTDIR)\desktop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\event.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\event.obj"	"$(INTDIR)\event.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\fixstr.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\fixstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\fixstr.obj"	"$(INTDIR)\fixstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\init.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\init.obj"	"$(INTDIR)\init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\message.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\message.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\message.obj"	"$(INTDIR)\message.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\kernel\timer.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\timer.obj"	"$(INTDIR)\timer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\newgdi\advapi.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\advapi.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\advapi.obj"	"$(INTDIR)\advapi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\arc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\arc.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\arc.obj"	"$(INTDIR)\arc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\attr.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\attr.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\attr.obj"	"$(INTDIR)\attr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\bitmap.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\bitmap.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\bitmap.obj"	"$(INTDIR)\bitmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\clip.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\clip.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\clip.obj"	"$(INTDIR)\clip.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\coor.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\coor.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\coor.obj"	"$(INTDIR)\coor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\drawtext.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\drawtext.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\drawtext.obj"	"$(INTDIR)\drawtext.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\flood.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\flood.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\flood.obj"	"$(INTDIR)\flood.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\gdi.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\gdi.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\gdi.obj"	"$(INTDIR)\gdi.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\generators.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\generators.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\generators.obj"	"$(INTDIR)\generators.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\icon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\icon.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\icon.obj"	"$(INTDIR)\icon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\lbm.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\lbm.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\lbm.obj"	"$(INTDIR)\lbm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\line.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\line.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\line.obj"	"$(INTDIR)\line.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\logfont.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\logfont.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\logfont.obj"	"$(INTDIR)\logfont.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\map.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\map.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\map.obj"	"$(INTDIR)\map.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\miarc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\miarc.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\miarc.obj"	"$(INTDIR)\miarc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\midash.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\midash.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\midash.obj"	"$(INTDIR)\midash.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\mifillarc.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\mifillarc.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\mifillarc.obj"	"$(INTDIR)\mifillarc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\mifpolycon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\mifpolycon.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\mifpolycon.obj"	"$(INTDIR)\mifpolycon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\mispans.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\mispans.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\mispans.obj"	"$(INTDIR)\mispans.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\miwideline.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\miwideline.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\miwideline.obj"	"$(INTDIR)\miwideline.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\palette.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\palette.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\palette.obj"	"$(INTDIR)\palette.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\pcx.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\pcx.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\pcx.obj"	"$(INTDIR)\pcx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\pixel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\pixel.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\pixel.obj"	"$(INTDIR)\pixel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\pixel_ops.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\pixel_ops.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\pixel_ops.obj"	"$(INTDIR)\pixel_ops.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\polygon.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\polygon.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\polygon.obj"	"$(INTDIR)\polygon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\readbmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\readbmp.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\readbmp.obj"	"$(INTDIR)\readbmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\rect.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\rect.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\rect.obj"	"$(INTDIR)\rect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\region.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\region.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\region.obj"	"$(INTDIR)\region.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\screen.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\screen.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\screen.obj"	"$(INTDIR)\screen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\text.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\text.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\text.obj"	"$(INTDIR)\text.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\tga.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\tga.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\tga.obj"	"$(INTDIR)\tga.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgdi\yuv.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgdi/" /Fd"Release\newgdi/" /FD /c 

".\Release\newgdi\yuv.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Debug/minigui-win32.pch" /YX /Fo"Debug/newgdi/" /Fd"Debug/newgdi/" /FD /GZ /Zm200 /c 

".\Debug\newgdi\yuv.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE="..\src\font\in-core\12x24-iso8859-1.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\12x24-iso8859-1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\12x24-iso8859-1.obj"	"$(INTDIR)\12x24-iso8859-1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\6x12-iso8859-1.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\6x12-iso8859-1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\6x12-iso8859-1.obj"	"$(INTDIR)\6x12-iso8859-1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\8x16-iso8859-1.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\8x16-iso8859-1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\8x16-iso8859-1.obj"	"$(INTDIR)\8x16-iso8859-1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\courier_8x13.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\courier_8x13.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\courier_8x13.obj"	"$(INTDIR)\courier_8x13.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\fixedsys_8x15.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\fixedsys_8x15.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\fixedsys_8x15.obj"	"$(INTDIR)\fixedsys_8x15.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_12_jiskan.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_12_jiskan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_12_jiskan.obj"	"$(INTDIR)\gothic_12_jiskan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_14_jiskan.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_14_jiskan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_14_jiskan.obj"	"$(INTDIR)\gothic_14_jiskan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_16_jiskan.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_16_jiskan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_16_jiskan.obj"	"$(INTDIR)\gothic_16_jiskan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_6x12rk.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_6x12rk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_6x12rk.obj"	"$(INTDIR)\gothic_6x12rk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_7x14rk.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_7x14rk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_7x14rk.obj"	"$(INTDIR)\gothic_7x14rk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\gothic_8x16rk.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gothic_8x16rk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gothic_8x16rk.obj"	"$(INTDIR)\gothic_8x16rk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\sansserif_11x13.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\sansserif_11x13.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\sansserif_11x13.obj"	"$(INTDIR)\sansserif_11x13.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\song-12-gb2312.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\song-12-gb2312.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\song-12-gb2312.obj"	"$(INTDIR)\song-12-gb2312.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\song-16-gb2312.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\song-16-gb2312.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\song-16-gb2312.obj"	"$(INTDIR)\song-16-gb2312.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\song-24-gb2312.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\song-24-gb2312.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\song-24-gb2312.obj"	"$(INTDIR)\song-24-gb2312.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\symb12.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\symb12.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\symb12.obj"	"$(INTDIR)\symb12.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\system_14x16.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\system_14x16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\system_14x16.obj"	"$(INTDIR)\system_14x16.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\terminal_8x12.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\terminal_8x12.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\terminal_8x12.obj"	"$(INTDIR)\terminal_8x12.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\vbf_helvR16.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vbf_helvR16.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vbf_helvR16.obj"	"$(INTDIR)\vbf_helvR16.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\vbf_helvR21.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vbf_helvR21.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vbf_helvR21.obj"	"$(INTDIR)\vbf_helvR21.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\vbf_helvR27.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vbf_helvR27.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vbf_helvR27.obj"	"$(INTDIR)\vbf_helvR27.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\font\in-core\vgarom_8x8.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vgarom_8x8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vgarom_8x8.obj"	"$(INTDIR)\vgarom_8x8.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\big5unimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\big5unimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\big5unimap.obj"	"$(INTDIR)\big5unimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\charset.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\charset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\charset.obj"	"$(INTDIR)\charset.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\devfont.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\devfont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\devfont.obj"	"$(INTDIR)\devfont.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\euckrunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\euckrunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\euckrunimap.obj"	"$(INTDIR)\euckrunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\fontname.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\fontname.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\fontname.obj"	"$(INTDIR)\fontname.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\freetype.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\freetype.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\freetype.obj"	"$(INTDIR)\freetype.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\freetype2.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\freetype2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\freetype2.obj"	"$(INTDIR)\freetype2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\gb18030unimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gb18030unimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gb18030unimap.obj"	"$(INTDIR)\gb18030unimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\gbkunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gbkunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gbkunimap.obj"	"$(INTDIR)\gbkunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\gbunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gbunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\gbunimap.obj"	"$(INTDIR)\gbunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\jisunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\jisunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\jisunimap.obj"	"$(INTDIR)\jisunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\rawbitmap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\rawbitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\rawbitmap.obj"	"$(INTDIR)\rawbitmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\sjisunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\sjisunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\sjisunimap.obj"	"$(INTDIR)\sjisunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\sysfont.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\sysfont.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\sysfont.obj"	"$(INTDIR)\sysfont.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\ujisunimap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\ujisunimap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\ujisunimap.obj"	"$(INTDIR)\ujisunimap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\font\varbitmap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\varbitmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\varbitmap.obj"	"$(INTDIR)\varbitmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\newgal\wvfb\winfb.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\winfb.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\winfb.obj"	"$(INTDIR)\winfb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\wvfb\wvfb.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\wvfb.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\wvfb.obj"	"$(INTDIR)\wvfb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\dummy\nullvideo.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\nullvideo.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\nullvideo.obj"	"$(INTDIR)\nullvideo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\blit.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\blit.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\blit.obj"	"$(INTDIR)\blit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\blit_0.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\blit_0.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\blit_0.obj"	"$(INTDIR)\blit_0.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\blit_1.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\blit_1.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\blit_1.obj"	"$(INTDIR)\blit_1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\blit_A.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\blit_A.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\blit_A.obj"	"$(INTDIR)\blit_A.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\blit_N.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\blit_N.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\blit_N.obj"	"$(INTDIR)\blit_N.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\gamma.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\gamma.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\gamma.obj"	"$(INTDIR)\gamma.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\newgal.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\newgal.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\newgal.obj"	"$(INTDIR)\newgal.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\pixels.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\pixels.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\pixels.obj"	"$(INTDIR)\pixels.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\RLEaccel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\RLEaccel.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\RLEaccel.obj"	"$(INTDIR)\RLEaccel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\stretch.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\stretch.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\stretch.obj"	"$(INTDIR)\stretch.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\surface.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\surface.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\surface.obj"	"$(INTDIR)\surface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\video.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\video.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\video.obj"	"$(INTDIR)\video.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\yuv.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\yuv.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\yuv.obj"	"$(INTDIR)\yuv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\yuv_mmx.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\yuv_mmx.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\yuv_mmx.obj"	"$(INTDIR)\yuv_mmx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\newgal\yuv_sw.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\newgal/" /Fd"Release\newgal/" /FD /c 

".\Release\newgal\yuv_sw.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\yuv_sw.obj"	"$(INTDIR)\yuv_sw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\ial\auto.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\auto.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\auto.obj"	"$(INTDIR)\auto.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ial\dummy.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\dummy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\dummy.obj"	"$(INTDIR)\dummy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ial\ial.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\ial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\ial.obj"	"$(INTDIR)\ial.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ial\winial.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\winial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\winial.obj"	"$(INTDIR)\winial.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\ial\wvfbial.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\wvfbial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\wvfbial.obj"	"$(INTDIR)\wvfbial.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\accelkey.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\accelkey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\accelkey.obj"	"$(INTDIR)\accelkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\caret.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\caret.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\caret.obj"	"$(INTDIR)\caret.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\ctrlclass.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\ctrlclass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\ctrlclass.obj"	"$(INTDIR)\ctrlclass.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\de-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\de-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\de-kmap.obj"	"$(INTDIR)\de-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\de-latin1-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\de-latin1-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\de-latin1-kmap.obj"	"$(INTDIR)\de-latin1-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\de-latin1-nodeadkeys-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\de-latin1-nodeadkeys-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\de-latin1-nodeadkeys-kmap.obj"	"$(INTDIR)\de-latin1-nodeadkeys-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\defkeymap.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\defkeymap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\defkeymap.obj"	"$(INTDIR)\defkeymap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\dialog.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\dialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\dialog.obj"	"$(INTDIR)\dialog.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\element.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\element.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\element.obj"	"$(INTDIR)\element.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\es-cp850-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\es-cp850-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\es-cp850-kmap.obj"	"$(INTDIR)\es-cp850-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\es-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\es-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\es-kmap.obj"	"$(INTDIR)\es-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\fr-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\fr-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\fr-kmap.obj"	"$(INTDIR)\fr-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\fr-pc-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\fr-pc-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\fr-pc-kmap.obj"	"$(INTDIR)\fr-pc-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE="..\src\gui\it-kmap.c"

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\it-kmap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\it-kmap.obj"	"$(INTDIR)\it-kmap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\keyboard.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\keyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\keyboard.obj"	"$(INTDIR)\keyboard.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\menu.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\menu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\menu.obj"	"$(INTDIR)\menu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\gui\window.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\window.obj"	"$(INTDIR)\window.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\about.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\about.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\about.obj"	"$(INTDIR)\about.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\clipboard.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\clipboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\clipboard.obj"	"$(INTDIR)\clipboard.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\endianrw.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\endianrw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\endianrw.obj"	"$(INTDIR)\endianrw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\math.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\math3d.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\math3d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\math3d.obj"	"$(INTDIR)\math3d.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\misc.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\misc.obj"	"$(INTDIR)\misc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\nposix.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\nposix.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\nposix.obj"	"$(INTDIR)\nposix.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\rwops.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\rwops.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\rwops.obj"	"$(INTDIR)\rwops.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\misc\systext.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\systext.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\systext.obj"	"$(INTDIR)\systext.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\gif.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\gif.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\debug\gif.obj"	".\debug\gif.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\jpeg.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\jpeg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\debug\jpeg.obj"	".\debug\jpeg.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\lbm.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\lbm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\Debug\lbm.obj"	".\Debug\lbm.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\mybmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\mybmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\debug\mybmp.obj"	".\debug\mybmp.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\pcx.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\pcx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\Debug\pcx.obj"	".\Debug\pcx.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\png.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\png.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\debug\png.obj"	".\debug\png.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\tga.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\tga.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\Debug\tga.obj"	".\Debug\tga.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\mybmp\winbmp.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\winbmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


".\debug\winbmp.obj"	".\debug\winbmp.sbr" : $(SOURCE)
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sysres\mgetc.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\mgetc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\mgetc.obj"	"$(INTDIR)\mgetc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sysres\resource.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\resource.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\resource.obj"	"$(INTDIR)\resource.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\sysres\syscfg.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\syscfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\syscfg.obj"	"$(INTDIR)\syscfg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\libc\snprintf.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\snprintf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\snprintf.obj"	"$(INTDIR)\snprintf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\libc\vfnprintf.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vfnprintf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vfnprintf.obj"	"$(INTDIR)\vfnprintf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\libc\vsnprintf.c

!IF  "$(CFG)" == "minigui - Win32 Release"


"$(INTDIR)\vsnprintf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"


"$(INTDIR)\vsnprintf.obj"	"$(INTDIR)\vsnprintf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\ext\control\animation.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\animation.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\animation.obj"	"$(INTDIR)\animation.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\coolbar.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\coolbar.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\coolbar.obj"	"$(INTDIR)\coolbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\gif89a.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\gif89a.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\gif89a.obj"	"$(INTDIR)\gif89a.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\gridview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\gridview.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\gridview.obj"	"$(INTDIR)\gridview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\gridviewcelltype.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\gridviewcelltype.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\gridviewcelltype.obj"	"$(INTDIR)\gridviewcelltype.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\iconview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\iconview.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\iconview.obj"	"$(INTDIR)\iconview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\initmgext.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\initmgext.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\initmgext.obj"	"$(INTDIR)\initmgext.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\listview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\listview.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\listview.obj"	"$(INTDIR)\listview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\monthcalendar.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\monthcalendar.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\monthcalendar.obj"	"$(INTDIR)\monthcalendar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\spinbox.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\spinbox.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\spinbox.obj"	"$(INTDIR)\spinbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\control\treeview.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\treeview.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\treeview.obj"	"$(INTDIR)\treeview.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\bmplabel.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\bmplabel.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\bmplabel.obj"	"$(INTDIR)\bmplabel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\chkbutton.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\chkbutton.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\chkbutton.obj"	"$(INTDIR)\chkbutton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\cmdbutton.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\cmdbutton.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\cmdbutton.obj"	"$(INTDIR)\cmdbutton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\label.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\label.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\label.obj"	"$(INTDIR)\label.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\mgcontrol.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\mgcontrol.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\mgcontrol.obj"	"$(INTDIR)\mgcontrol.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\rotslider.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\rotslider.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\rotslider.obj"	"$(INTDIR)\rotslider.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\skin.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\skin.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\skin.obj"	"$(INTDIR)\skin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\skin\slider.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\slider.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\slider.obj"	"$(INTDIR)\slider.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\colordlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\colordlg.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\colordlg.obj"	"$(INTDIR)\colordlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\colorspace.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\colorspace.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\colorspace.obj"	"$(INTDIR)\colorspace.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\filedlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\filedlg.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\filedlg.obj"	"$(INTDIR)\filedlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\helpwin.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\helpwin.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\helpwin.obj"	"$(INTDIR)\helpwin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\mywindows.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\mywindows.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\mywindows.obj"	"$(INTDIR)\mywindows.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\ext\mywins\newfiledlg.c

!IF  "$(CFG)" == "minigui - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\..\include\pthread-win32" /I "..\\" /I "..\include" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /Fp"Release/minigui-win32.pch" /YX /Fo"Release\ext/" /Fd"Release\ext/" /FD /c 

".\Release\ext\newfiledlg.obj" : $(SOURCE)
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "minigui - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include\\" /I "..\..\include\thread-win32" /I "..\src\include" /I "..\src\include\control" /I "..\src\font" /I "..\src\newgal" /I "..\ext\include\\" /I "..\ext\control\\" /I "..\ext\skin\\" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIGUI_EXPORTS" /D "__MINIGUI_LIB__" /D "__NOUNIX__" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\minigui-win32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm200 /c 

"$(INTDIR)\newfiledlg.obj"	"$(INTDIR)\newfiledlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

