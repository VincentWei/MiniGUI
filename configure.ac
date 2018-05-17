dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT(libminigui, 3.2.0)
AC_CONFIG_SRCDIR(src/main/main.c)

dnl Set various version strings - taken gratefully from the SDL sources
#
# Making releases:
# Change the version, then:
#   MINIGUI_MICRO_VERSION += 1;
#   MINIGUI_INTERFACE_AGE += 1;
#   MINIGUI_BINARY_AGE += 1;
# if any functions have been added, set MINIGUI_INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set MINIGUI_BINARY_AGE and MINIGUI_INTERFACE_AGE to 0.
#
MINIGUI_MAJOR_VERSION=3
MINIGUI_MINOR_VERSION=2
MINIGUI_MICRO_VERSION=0
MINIGUI_INTERFACE_AGE=0
MINIGUI_BINARY_AGE=0
MINIGUI_VERSION=$MINIGUI_MAJOR_VERSION.$MINIGUI_MINOR_VERSION.$MINIGUI_MICRO_VERSION

AC_SUBST(MINIGUI_MAJOR_VERSION)
AC_SUBST(MINIGUI_MINOR_VERSION)
AC_SUBST(MINIGUI_MICRO_VERSION)
AC_SUBST(MINIGUI_INTERFACE_AGE)
AC_SUBST(MINIGUI_BINARY_AGE)
AC_SUBST(MINIGUI_VERSION)

dnl ========================================================================
dnl libtool versioning
LT_RELEASE=$MINIGUI_MAJOR_VERSION.$MINIGUI_MINOR_VERSION
LT_CURRENT=`expr $MINIGUI_MICRO_VERSION - $MINIGUI_INTERFACE_AGE`
LT_REVISION=$MINIGUI_INTERFACE_AGE
LT_AGE=`expr $MINIGUI_BINARY_AGE - $MINIGUI_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CANONICAL_SYSTEM

dnl ========================================================================
dnl Init automake
AM_INIT_AUTOMAKE

AC_CONFIG_HEADER(mgconfig.h)

dnl ========================================================================
dnl Check for tools
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ========================================================================
dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS(sys/types.h sys/time.h termio.h unistd.h math.h locale.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(time mktime localtime strdup strcasecmp strncasecmp strerror setlocale getpt)

dnl ========================================================================
dnl User selectable options

dnl System wide options
devel_mode="no"
detail_debug="no"
trace_message="no"
message_string="no"

runtime_mode="procs"

stand_alone="no"

incore_res="no"
use_miniguientry="no"
fixed_math="yes"
double_click="yes"
build_cursor_support="yes"
build_clipboard="yes"
use_own_stdio="no"
use_own_malloc="no"
use_own_pthread="no"

build_adv2dapi="yes"
build_minimalgdi="no"

dnl Options for look and feel renderer
with_flat_lf="yes"
with_skin_lf="yes"

dnl IAL engine-specific options
dnl software IAL engines
build_dummy_ial_engine="yes"
build_auto_ial_engine="no"
build_random_ial_engine="no"
build_net_ial_engine="no"

dnl common IAL engines
build_console_ial_engine="yes"
build_console_ps2="yes"
build_console_imps2="yes"
build_console_ms="yes"
build_console_ms3="yes"
build_console_gpm="no"
build_text_mode="yes"

dnl the customized ial engine
build_dlcustom_ial_engine="no"
build_custom_ial_engine="no"
build_comm_ial_engine="no"

dnl ial engines for simulaters
build_qvfb_ial_engine="no"
build_wvfb_ial_engine="no"
build_dfb_ial_engine="no"
build_qemu_ial_engine="no"

dnl ial engines for real hardware
build_cisco_touchpad_ial_engine="no"
build_mstar_ial_engine="no"
build_ipaq_h3600_ial_engine="no"
build_nexus_ial_engine="no"
build_ipaq_h5400_ial_engine="no"
build_tslib_ial_engine="no"
build_jz4740_ial_engine="no"
build_lide_ial_engine="no"
build_2440_ial_engine="no"
build_davinci6446_ial_engine="no"

dnl Font related options
build_rbf_support="yes"
build_incorerbf_vgaoem="yes"
build_incorerbf_terminal="yes"
build_incorerbf_fixedsys="yes"

build_vbf_support="yes"
build_incorefont_sansserif="yes"
build_incorefont_courier="yes"
build_incorefont_system="yes"

build_upf_support="yes"
build_incorefont_times="yes"

build_sef_support="no"
build_qpf_support="no"
use_ttf_lib="ft2"
build_ttfcache_support="no"
build_bmpf_support="yes"

dnl Charset related options
build_latin2_support="no"
build_latin3_support="no"
build_latin4_support="no"
build_cyrillic_support="no"
build_arabic_support="no"
build_greek_support="no"
build_hebrew_support="no"
build_latin5_support="no"
build_latin6_support="no"
build_thai_support="no"
build_latin7_support="no"
build_latin8_support="no"
build_latin9_support="yes"
build_latin10_support="no"

build_gb_support="yes"
build_gbk_support="yes"
build_gb18030_support="no"
build_big5_support="no"
build_euckr_support="no"
build_eucjp_support="no"
build_shiftjis_support="no"

build_unicode_support="yes"

dnl Keyboard layout specific options
use_kbd_hebrewpc="no"
use_kbd_arabicpc="no"
use_kbd_frpc="no"
use_kbd_fr="no"
use_kbd_de="no"
use_kbd_delatin1="no"
use_kbd_it="no"
use_kbd_es="no"
use_kbd_escp850="no"

dnl Image related options
build_save_bitmap="yes"

build_gif_support="yes"
build_jpg_support="yes"
build_png_support="yes"
build_pcx_support="no"
build_lbm_support="no"
build_tga_support="no"

dnl Menu options
build_menu="yes"

dnl Misc options
build_misc_mousecalibrate="yes"
build_misc_aboutdlg="yes"
build_misc_savescreen="yes"

dnl Control related options
build_ctrl_static="yes"
build_ctrl_button="yes"
build_ctrl_sledit="yes"
build_ctrl_bidisledit="no"
build_ctrl_textedit_new="yes"
build_ctrl_listbox="yes"
build_ctrl_progressbar="yes"
build_ctrl_combobox="yes"
build_ctrl_propsheet="yes"
build_ctrl_trackbar="yes"
build_ctrl_scrollbar="yes"
build_ctrl_newtoolbar="yes"
build_ctrl_menubutton="yes"
build_ctrl_scrollview="yes"
build_ctrl_textedit="no"
build_ctrl_monthcal="no"
build_ctrl_treeview="no"
build_ctrl_treeview_rdr="no"
build_ctrl_spinbox="yes"
build_ctrl_coolbar="no"
build_ctrl_listview="yes"
build_ctrl_iconview="no"
build_ctrl_gridview="no"
build_ctrl_animation="yes"

enable_video_dummy="yes"
enable_video_pc_xvfb="yes"
enable_video_rtos_xvfb="no"
enable_video_qvfb="no"
enable_video_wvfb="no"
enable_video_dfb="no"
enable_video_dfb_st7167="no"
enable_video_stgfb="no"

enable_video_fbcon="yes"
enable_video_commlcd="no"
enable_video_shadow="no"
enable_video_mlshadow="no"

enable_video_em86gfx="no"
enable_video_em85xxyuv="no"
enable_video_em85xxosd="no"
enable_video_svpxxosd="no"
enable_video_bf533="no"
enable_video_mb93493="no"
enable_video_utpmc="no"
enable_video_hi35xx="no"
enable_video_hi3560a="no"
enable_video_nexus="no"
enable_video_s3c6410="no"
enable_video_sigma8654="no"
enable_video_mstar="no"
enable_video_custom="no"

dnl settings for license management
build_productid="no"
build_splash="yes"
build_screensaver="no"

AC_ARG_ENABLE(develmode,
[  --enable-develmode       developer mode <default=no>],
devel_mode=$enableval)

AC_ARG_ENABLE(detaildebug,
[  --enable-detaildebug     detailed debug info <default=no>],
detail_debug=$enableval)

AC_ARG_ENABLE(tracemsg,
[  --enable-tracemsg        trace messages of MiniGUI <default=no>],
trace_message=$enableval)

AC_ARG_ENABLE(msgstr,
[  --enable-msgstr          include symbol name of message <default=no>],
message_string=$enableval)

AC_ARG_WITH(runmode,
[  --with-runmode=procs/ths/sa  the MiniGUI runtime mode <default=procs>],
runtime_mode=$withval)

AC_ARG_ENABLE(standalone,
[  --enable-standalone      build MiniGUI-Standalone version <default=no>],
stand_alone=$enableval)

AC_ARG_ENABLE(incoreres,
[  --enable-incoreres       use incore resource instead file IO to initialize MiniGUI <default=no>],
incore_res=$enableval)

AC_ARG_ENABLE(miniguientry,
[  --enable-miniguientry    use minigui_entry function in MiniGUI <default=no>],
use_miniguientry=$enableval)

AC_ARG_ENABLE(fixedmath,
[  --enable-fixedmath       include fixed math routines <default=yes>],
fixed_math=$enableval)

AC_ARG_ENABLE(dblclk,
[  --enable-dblclk          mouse button can do double click <default=yes>],
double_click=$enableval)

AC_ARG_ENABLE(cursor,
[  --enable-cursor          include cursor support <default=yes>],
build_cursor_support=$enableval)

AC_ARG_ENABLE(clipboard,
[  --enable-clipboard       include clipboard support <default=yes>],
build_clipboard=$enableval)

AC_ARG_ENABLE(ownstdio,
[  --enable-ownstdio        use own implementation of stdio functions <default=no>],
use_own_stdio=$enableval)

AC_ARG_ENABLE(ownmalloc,
[  --enable-ownmalloc       use own implementation of malloc functions <default=no>],
use_own_malloc=$enableval)

AC_ARG_ENABLE(ownpthread,
[  --enable-ownpthread      use own implementation of pthread functions <default=no>],
use_own_pthread=$enableval)

AC_ARG_ENABLE(adv2dapi,
[  --enable-adv2dapi        include advanced 2D graphics APIs <default=yes>],
build_adv2dapi=$enableval)

AC_ARG_ENABLE(minimalgdi,
[  --enable-minimalgdi      build a minimal GDI library only <default=no>],
build_minimalgdi=$enableval)

AC_ARG_ENABLE(productid,
[  --enable-productid       insert a productid into the library file <default=no>],
build_productid=$enableval)

AC_ARG_ENABLE(splash,
[  --enable-splash          enable splash <default=yes>],
build_splash=$enableval)

AC_ARG_ENABLE(screensaver,
[  --enable-screensaver     enable screensaver <default=no>],
build_screensaver=$enableval)

AC_ARG_ENABLE(flatlf,
[  --enable-flatlf          include flat Look and Feel renderer <default=yes>],
with_flat_lf=$enableval)

AC_ARG_ENABLE(skinlf,
[  --enable-skinlf          include skin Look and Feel renderer <default=yes>],
with_skin_lf=$enableval)

AC_ARG_ENABLE(customial,
[  --enable-customial       build the customer IAL engine <default=no>],
build_custom_ial_engine=$enableval)

AC_ARG_ENABLE(dlcustomial,
[  --enable-dlcustomial     build the dlcustom IAL engine <default=no>],
build_dlcustom_ial_engine=$enableval)

AC_ARG_ENABLE(netial,
[  --enable-netial          build the IAL engine for net <default=no>],
build_net_ial_engine=$enableval)

AC_ARG_ENABLE(cisco_touchpad_ial,
[  --enable-cisco_touchpad_ial	build the IAL engine for Cisco Touchpad <default=no>],
build_cisco_touchpad_ial_engine=$enableval)

AC_ARG_ENABLE(mstar_ial,
[  --enable-mstar_ial       build the IAL engine for MStar <default=no>],
build_mstar_ial_engine=$enableval)

AC_ARG_ENABLE(ipaqh3600ial,
[  --enable-ipaqh3600ial    build the IAL engine for iPAQ H3600 <default=no>],
build_ipaq_h3600_ial_engine=$enableval)

AC_ARG_ENABLE(nexusial,
[  --enable-nexusial        build the IAL engine for Nexus <default=no>],
build_nexus_ial_engine=$enableval)

AC_ARG_ENABLE(ipaqh5400ial,
[  --enable-ipaqh5400ial    build the IAL engine for iPAQ H5400 <default=no>],
build_ipaq_h5400_ial_engine=$enableval)

AC_ARG_ENABLE(tslibial,
[  --enable-tslibial        build the IAL engine for TSLIB <default=no>],
build_tslib_ial_engine=$enableval)

AC_ARG_ENABLE(dummyial,
[  --enable-dummyial        build the Dummy IAL engine <default=yes>],
build_dummy_ial_engine=$enableval)

AC_ARG_ENABLE(autoial,
[  --enable-autoial         build the Automatic IAL engine <default=no>],
build_auto_ial_engine=$enableval)

AC_ARG_ENABLE(randomial,
[  --enable-randomial       build the Random IAL engine <default=no>],
build_random_ial_engine=$enableval)

AC_ARG_ENABLE(commial,
[  --enable-commial         build the COMM IAL engine <default=no>],
build_comm_ial_engine=$enableval)

AC_ARG_ENABLE(qvfbial,
[  --enable-qvfbial         build the QVFB IAL engine <default=no>],
build_qvfb_ial_engine=$enableval)

AC_ARG_ENABLE(qemuial,
[  --enable-qemuial         build the QEMU IAL engine <default=no>],
build_qemu_ial_engine=$enableval)

AC_ARG_ENABLE(wvfbial,
[  --enable-wvfbial         build the WVFB IAL engine <default=no>],
build_wvfb_ial_engine=$enableval)

AC_ARG_ENABLE(jz4740ial,
[  --enable-jz4740ial       build the JZ4740 IAL engine <default=no>],
build_jz4740_ial_engine=$enableval)

AC_ARG_ENABLE(lide,
[  --enable-lide            build the lide IAL engine <default=no>],
build_lide_ial_engine=$enableval)

AC_ARG_ENABLE(2440ial,
[  --enable-2440ial         build the 2440 IAL engine <default=no>],
build_2440_ial_engine=$enableval)

AC_ARG_ENABLE(davinci6446ial,
[  --enable-davinci6446ial  build the DAVINCI6446 IAL engine <default=no>],
build_davinci6446_ial_engine=$enableval)

AC_ARG_ENABLE(dfbial,
[  --enable-dfbial          build the DFB IAL engine <default=no>],
build_dfb_ial_engine=$enableval)

AC_ARG_ENABLE(consoleial,
[  --enable-consoleial      build the console (Linux console) IAL engine <default=yes>],
build_console_ial_engine=$enableval)

AC_ARG_ENABLE(consoleps2,
[  --enable-consoleps2      build the console engine subdriver for PS2 mouse <default=yes>],
build_console_ps2=$enableval)

AC_ARG_ENABLE(consoleimps2,
[  --enable-consoleimps2    build the console engine subdriver for IntelligentMouse (IMPS/2) mouse <default=yes>],
build_console_imps2=$enableval)

AC_ARG_ENABLE(consolems,
[  --enable-consolems       build the console engine subdirver for old MS serial mouse <default=yes>],
build_console_ms=$enableval)

AC_ARG_ENABLE(consolems3,
[  --enable-consolems3      build the console engine subdirver for MS3 mouse <default=yes>],
build_console_ms3=$enableval)

AC_ARG_ENABLE(consolegpm,
[  --enable-consolegpm      build the console engine subdirver for GPM daemon <default=no>],
build_console_gpm=$enableval)

AC_ARG_ENABLE(textmode,
[  --enable-textmode        Linux system have console (text mode) on FrameBuffer <default=yes>],
build_text_mode=$enableval)

AC_ARG_ENABLE(rbfsupport,
[  --enable-rbfsupport      include raw bitmap font support <default=yes>],
build_rbf_support=$enableval)

AC_ARG_ENABLE(rbfvgaoem,
[  --enable-rbfvgaoem       include incore RBF font of ISO8859-1 VGAOEM (8x8) font <default=yes>],
build_incorerbf_vgaoem=$enableval)

AC_ARG_ENABLE(rbfterminal,
[  --enable-rbfterminal     include incore RBF font of ISO8859-1 Terminal (8x12) font <default=yes>],
build_incorerbf_terminal=$enableval)

AC_ARG_ENABLE(rbffixedsys,
[  --enable-rbffixedsys     include incore RBF font of ISO8859-1 FixedSys (8x15) font <default=yes>],
build_incorerbf_fixedsys=$enableval)

AC_ARG_ENABLE(vbfsupport,
[  --enable-vbfsupport      include var bitmap font support <default=yes>],
build_vbf_support=$enableval)

AC_ARG_ENABLE(fontsserif,
[  --enable-fontsserif      include incore font SansSerif (11x13) <default=yes>],
build_incorefont_sansserif=$enableval)

AC_ARG_ENABLE(fontcourier,
[  --enable-fontcourier     include incore font Courier (8x13) <default=yes>],
build_incorefont_courier=$enableval)

AC_ARG_ENABLE(fontsystem,
[  --enable-fontsystem      include incore font System (14x16) <default=yes>],
build_incorefont_system=$enableval)

AC_ARG_ENABLE(upfsupport,
[  --enable-upfsupport      build support for FMSoft Unicode Prerendered Font (UPF) <default=yes>],
build_upf_support=$enableval)

AC_ARG_ENABLE(fonttimes,
[  --enable-fonttimes       include incore Times UPF fonts (12x10 and 17x14) <default=yes>],
build_incorefont_times=$enableval)

AC_ARG_ENABLE(qpfsupport,
[  --enable-qpfsupport      build support for Qt Prerendered Font (QPF) <default=no>],
build_qpf_support=$enableval)

AC_ARG_ENABLE(sefsupport,
[  --enable-sefsupport      build support for koxomo scripteasy (SEF) <default=no>],
build_sef_support=$enableval)

AC_ARG_WITH(ttfsupport,
[  --with-ttfsupport=ft1/ft2/none   How to support TrueType font (FreeType 1/FreeType 2/None) <default=none>],
use_ttf_lib=$withval)

AC_ARG_ENABLE(ttfcache,
[  --enable-ttfcache        include ttf cache support <default=no>],
build_ttfcache_support=$enableval)

AC_ARG_ENABLE(bmpfsupport,
[  --enable-bmpfsupport     build support for Bitmap Font (bmpf) <default=yes>],
build_bmpf_support=$enableval)

AC_ARG_ENABLE(latin2support,
[  --enable-latin2support   include East European (Latin 2, ISO8859-2) charset support <default=no>],
build_latin2_support=$enableval)

AC_ARG_ENABLE(latin3support,
[  --enable-latin3support   include South European (Latin 3, ISO8859-3) charset support <default=no>],
build_latin3_support=$enableval)

AC_ARG_ENABLE(latin4support,
[  --enable-latin4support   include North European (Latin 4, ISO8859-4) charset support <default=no>],
build_latin4_support=$enableval)

AC_ARG_ENABLE(cyrillicsupport,
[  --enable-cyrillicsupport include Cyrillic (ISO8859-5) charset support <default=no>],
build_cyrillic_support=$enableval)

AC_ARG_ENABLE(arabicsupport,
[  --enable-arabicsupport   include Arabic (ISO8859-6) charset support <default=no>],
build_arabic_support=$enableval)

AC_ARG_ENABLE(greeksupport,
[  --enable-greeksupport    include Greek (ISO8859-7) charset support <default=no>],
build_greek_support=$enableval)

AC_ARG_ENABLE(hebrewsupport,
[  --enable-hebrewsupport   include Hebrew (ISO8859-8) charset support <default=no>],
build_hebrew_support=$enableval)

AC_ARG_ENABLE(latin5support,
[  --enable-latin5support   include Turkish (Latin 5, ISO8859-9) charset support <default=no>],
build_latin5_support=$enableval)

AC_ARG_ENABLE(latin6support,
[  --enable-latin6support   include Nordic, Latin 6, ISO8859-10) charset support <default=no>],
build_latin6_support=$enableval)

AC_ARG_ENABLE(thaisupport,
[  --enable-thaisupport     include Thai (ISO8859-11) charset support <default=yes>],
build_thai_support=$enableval)

AC_ARG_ENABLE(latin7support,
[  --enable-latin7support   include Latin 7 (ISO8859-13) charset support <default=no>],
build_latin7_support=$enableval)

AC_ARG_ENABLE(latin8support,
[  --enable-latin8support   include Latin 8 (ISO8859-14) charset support <default=no>],
build_latin8_support=$enableval)

AC_ARG_ENABLE(latin9support,
[  --enable-latin9support   include Latin 9 (ISO8859-15, West Extended) charset support <default=yes>],
build_latin9_support=$enableval)

AC_ARG_ENABLE(latin10support,
[  --enable-latin10support  include Latin 10 (ISO8859-16, Romanian) charset support <default=no>],
build_latin10_support=$enableval)

AC_ARG_ENABLE(gbsupport,
[  --enable-gbsupport       include EUC encoding of GB2312 charset support <default=yes>],
build_gb_support=$enableval)

AC_ARG_ENABLE(gbksupport,
[  --enable-gbksupport      include GBK charset support <default=yes>],
build_gbk_support=$enableval)

AC_ARG_ENABLE(gb18030support,
[  --enable-gb18030support  include GB18030-0 charset support <default=no>],
build_gb18030_support=$enableval)

AC_ARG_ENABLE(big5support,
[  --enable-big5support     include BIG5 charset support <default=no>],
build_big5_support=$enableval)

AC_ARG_ENABLE(euckrsupport,
[  --enable-euckrsupport    include support for EUC encoding of KSC5636 and KSC5601 charsets <default=no>],
build_euckr_support=$enableval)

AC_ARG_ENABLE(eucjpsupport,
[  --enable-eucjpsupport    include support for EUC encoding of JISX0201 and JISX0208 charsets <default=no>],
build_eucjp_support=$enableval)

AC_ARG_ENABLE(shiftjissupport,
[  --enable-shiftjissupport include support for Shift-JIS encoding of JISX0201 and JISX0208 charsets <default=no>],
build_shiftjis_support=$enableval)

AC_ARG_ENABLE(unicodesupport,
[  --enable-unicodesupport  include UNICODE (ISO-10646-1 and UTF-8 encoding) support <default=yes>],
build_unicode_support=$enableval)

AC_ARG_ENABLE(kbdhebrewpc,
[  --enable-kbdhebrewpc     include keyboard layout for Hebrew PC keyboard <default=no>],
use_kbd_hebrewpc=$enableval)

AC_ARG_ENABLE(kbdarabicpc,
[  --enable-kbdarabicpc     include keyboard layout for Arabic PC keyboard <default=no>],
use_kbd_arabicpc=$enableval)

AC_ARG_ENABLE(kbdfrpc,
[  --enable-kbdfrpc         include keyboard layout for French PC keyboard (non-US 102 keys) <default=no>],
use_kbd_frpc=$enableval)

AC_ARG_ENABLE(kbdfr,
[  --enable-kbdfr           include keyboard layout for French <default=no>],
use_kbd_fr=$enableval)

AC_ARG_ENABLE(kbdde,
[  --enable-kbdde           include keyboard layout for German <default=no>],
use_kbd_de=$enableval)

AC_ARG_ENABLE(kbddelatin1,
[  --enable-kbddelatin1     include keyboard layout for German Latin1 <default=no>],
use_kbd_delatin1=$enableval)

AC_ARG_ENABLE(kbdit,
[  --enable-kbdit           include keyboard layout for Italian <default=no>],
use_kbd_it=$enableval)

AC_ARG_ENABLE(kbdes,
[  --enable-kbdes           include keyboard layout for Spanish <default=no>],
use_kbd_es=$enableval)

AC_ARG_ENABLE(kbdescp850,
[  --enable-kbdescp850      include keyboard layout for Spanish CP850 <default=no>],
use_kbd_escp850=$enableval)

AC_ARG_ENABLE(savebitmap,
[  --enable-savebitmap      include SaveBitmap-related functions <default=yes>],
build_save_bitmap=$enableval)

AC_ARG_ENABLE(pcxsupport,
[  --enable-pcxsupport      include PCX file support <default=no>],
build_pcx_support=$enableval)

AC_ARG_ENABLE(lbmsupport,
[  --enable-lbmsupport      include LBM/PBM file support <default=no>],
build_lbm_support=$enableval)

AC_ARG_ENABLE(tgasupport,
[  --enable-tgasupport      include TGA file support <default=no>],
build_tga_support=$enableval)

AC_ARG_ENABLE(gifsupport,
[  --enable-gifsupport      include GIF file support <default=yes>],
build_gif_support=$enableval)

AC_ARG_ENABLE(jpgsupport,
[  --enable-jpgsupport      include JPG file support <default=yes>],
build_jpg_support=$enableval)

AC_ARG_ENABLE(pngsupport,
[  --enable-pngsupport      include PNG file support <default=yes>],
build_png_support=$enableval)

AC_ARG_ENABLE(menu,
[  --enable-menu            include menu support <default=yes>],
build_menu=$enableval)

AC_ARG_ENABLE(mousecalibrate,
[  --enable-mousecalibrate  include code doing mouse calibration <default=yes>],
build_misc_mousecalibrate=$enableval)

AC_ARG_ENABLE(aboutdlg,
[  --enable-aboutdlg        include About Dialog Box <default=yes>],
build_misc_aboutdlg=$enableval)

AC_ARG_ENABLE(savescreen,
[  --enable-savescreen      include code for screenshots <default=yes>],
build_misc_savescreen=$enableval)

AC_ARG_ENABLE(ctrlstatic,
[  --enable-ctrlstatic      include STATIC control <default=yes>],
build_ctrl_static=$enableval)

AC_ARG_ENABLE(ctrlbutton,
[  --enable-ctrlbutton      include BUTTON control <default=yes>],
build_ctrl_button=$enableval)

AC_ARG_ENABLE(ctrlsledit,
[  --enable-ctrlsledit      include Single-Line EDIT control <default=yes>],
build_ctrl_sledit=$enableval)

AC_ARG_ENABLE(ctrlbidisledit,
[  --enable-ctrlbidisledit  include Single-Line BIDI EDIT control <default=no>],
build_ctrl_bidisledit=$enableval)

dnl enable new textedit
AC_ARG_ENABLE(newtextedit,
[  --enable-ctrlnewtextedit include the new implementation of TEXTEDIT control <default=yes>],
build_ctrl_textedit_new=$enableval)

AC_ARG_ENABLE(ctrllistbox,
[  --enable-ctrllistbox     include LISTBOX control <default=yes>],
build_ctrl_listbox=$enableval)

AC_ARG_ENABLE(ctrlpgbar,
[  --enable-ctrlpgbar       include PROGRESSBAR control <default=yes>],
build_ctrl_progressbar=$enableval)

AC_ARG_ENABLE(ctrlcombobox,
[  --enable-ctrlcombobox    include COMBOBOX control <default=yes>],
build_ctrl_combobox=$enableval)

AC_ARG_ENABLE(ctrlpropsheet,
[  --enable-ctrlpropsheet   include PROPSHEET control <default=yes>],
build_ctrl_propsheet=$enableval)

AC_ARG_ENABLE(ctrltrackbar,
[  --enable-ctrltrackbar    include TRACKBAR control <default=no>],
build_ctrl_trackbar=$enableval)

AC_ARG_ENABLE(ctrlscrollbar,
[  --enable-ctrlscrollbar   include SCROLLBAR control <default=no>],
build_ctrl_scrollbar=$enableval)

AC_ARG_ENABLE(ctrlnewtoolbar,
[  --enable-ctrlnewtoolbar  include NEWTOOLBAR control <default=yes>],
build_ctrl_newtoolbar=$enableval)

AC_ARG_ENABLE(ctrlmenubtn,
[  --enable-ctrlmenubtn     include MENUBUTTON control <default=yes>],
build_ctrl_menubutton=$enableval)

AC_ARG_ENABLE(ctrlscrollview,
[  --enable-ctrlscrollview  include SCROLLVIEW and SCROLLWINDOW controls <default=no>],
build_ctrl_scrollview=$enableval)

AC_ARG_ENABLE(ctrltextedit,
[  --enable-ctrltextedit    include old TEXTEDIT control implementation <default=no>],
build_ctrl_textedit=$enableval)

AC_ARG_ENABLE(ctrlmonthcal,
[  --enable-ctrlmonthcal    include MONTHCALENDAR control <default=no>],
build_ctrl_monthcal=$enableval)

AC_ARG_ENABLE(ctrltreeview,
[  --enable-ctrltreeview    include TREEVIEW control <default=no>],
build_ctrl_treeview=$enableval)

AC_ARG_ENABLE(ctrltreeview-rdr,
[  --enable-ctrltreeview-rdr    include TREEVIEWRDR control using LFRDR <default=no>],
build_ctrl_treeview_rdr=$enableval)

AC_ARG_ENABLE(ctrlspinbox,
[  --enable-ctrlspinbox     include SPINBOX control <default=yes>],
build_ctrl_spinbox=$enableval)

AC_ARG_ENABLE(ctrlcoolbar,
[  --enable-ctrlcoolbar     include COOLBAR control <default=no>],
build_ctrl_coolbar=$enableval)

AC_ARG_ENABLE(ctrllistview,
[  --enable-ctrllistview    include LISTVIEW control <default=yes>],
build_ctrl_listview=$enableval)

AC_ARG_ENABLE(ctrliconview,
[  --enable-ctrliconview    include ICONVIEW control <default=no>],
build_ctrl_iconview=$enableval)

AC_ARG_ENABLE(ctrlgridview,
[  --enable-ctrlgridview    include GRIDVIEW control (test) <default=no>],
build_ctrl_gridview=$enableval)

AC_ARG_ENABLE(ctrlanimation,
[  --enable-ctrlanimation   include ANIMATION control and GIF87a/GIF89a support <default=yes>],
build_ctrl_animation=$enableval)

dnl Set up the Null video driver.
CheckDummyVideo()
{
    AC_ARG_ENABLE(videodummy,
[  --enable-videodummy      include dummy NEWGAL engine <default=yes>],
    enable_video_dummy=$enableval)

    if test "x$enable_video_dummy" = "xyes"; then
      AC_DEFINE(_MGGAL_DUMMY, 1,
                [Define if include dummy NEWGAL engine])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS dummy"
      VIDEO_DRIVERS="$VIDEO_DRIVERS dummy/libvideo_null.la"
    fi
}

dnl Find the framebuffer console includes
CheckFBCON()
{
    AC_ARG_ENABLE(videofbcon,
[  --enable-videofbcon      include FrameBuffer console NEWGAL engine <default=yes>],
    enable_video_fbcon=$enableval)
    if test x$enable_video_fbcon = xyes; then
        AC_MSG_CHECKING(for FrameBuffer console support)
        video_fbcon=no
        AC_TRY_COMPILE([
         #include <linux/fb.h>
         #include <linux/kd.h>
         #include <linux/keyboard.h>
        ],[
        ],[
        video_fbcon=yes
        ])
        AC_MSG_RESULT($video_fbcon)
        if test x$video_fbcon = xyes; then
            AC_DEFINE(_MGGAL_FBCON, 1,
                [Define if include FrameBuffer console NEWGAL engine])
            VIDEO_SUBDIRS="$VIDEO_SUBDIRS fbcon"
            VIDEO_DRIVERS="$VIDEO_DRIVERS fbcon/libvideo_fbcon.la"

            have_pciaccess="yes"
            AC_CHECK_LIB(pciaccess, pci_system_init,
                DEP_LIBS="$DEP_LIBS -lpciaccess", have_pciaccess="no")

            if test x$have_pciaccess = xyes; then
                AC_DEFINE(_MGHAVE_PCIACCESS, 1,
                [Define if PCIAccess lib is available])
                LDFLAGS="$LDFLAGS -lpciaccess"

            fi
        fi
    fi
}

dnl Check Qt Virtual FrameBuffer
CheckQVFB()
{
    AC_ARG_ENABLE(videoqvfb,
[  --enable-videoqvfb       include Qt Virtual FrameBuffer NEWGAL engine <default=yes>],
    enable_video_qvfb=$enableval)

    if test "x$enable_video_qvfb" = "xyes"; then
        AC_DEFINE(_MGGAL_QVFB, 1,
                [Define if include Qt Virtual FrameBuffer NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS qvfb"
        VIDEO_DRIVERS="$VIDEO_DRIVERS qvfb/libvideo_qvfb.la"
    fi
}

dnl Check X Virtual FrameBuffer
CheckXVFB()
{
    AC_ARG_ENABLE(videopcxvfb,
[  --enable-videopcxvfb     include PC Virtual FrameBuffer NEWGAL engine, such as qvfb, mvfb, gvfb or wvfb <default=yes>],
    enable_video_pc_xvfb=$enableval)

    AC_ARG_ENABLE(videortosxvfb,
[  --enable-videortosxvfb   include RTOS Virtual FrameBuffer NEWGAL engine <default=no>. Please disable pcxvfb to enable rtosxvfb],
    enable_video_rtos_xvfb=$enableval)

	if test "x$enable_video_pc_xvfb" = "xyes"; then
dnl	CheckPCXVFB
        AC_DEFINE(_MGGAL_PCXVFB, 1,
                [Define if include PC Virtual FrameBuffer NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS pcxvfb"
        VIDEO_DRIVERS="$VIDEO_DRIVERS pcxvfb/libvideo_pcxvfb.la"
	else 	
		if test "x$enable_video_rtos_xvfb" = "xyes"; then
dnl	CheckRTOSXVFB
        AC_DEFINE(_MGGAL_RTOSXVFB, 1,
                [Define if include RTOS Virtual FrameBuffer NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS rtos_xvfb"
        VIDEO_DRIVERS="$VIDEO_DRIVERS rtos_xvfb/libvideo_rtos_xvfb.la"
		fi
	fi
}


dnl Check Windows Virtual FrameBuffer
CheckWVFB()
{
    AC_ARG_ENABLE(videowvfb,
[  --enable-videowvfb       include Windows Virtual FrameBuffer NEWGAL engine <default=no>],
    enable_video_wvfb=$enableval)

    if test "x$enable_video_wvfb" = "xyes"; then
        AC_DEFINE(_MGGAL_WVFB, 1,
                [Define if include windows Virtual FrameBuffer NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS wvfb"
        VIDEO_DRIVERS="$VIDEO_DRIVERS wvfb/libvideo_wvfb.la"
        dnl AC_DEFINE(_WVFB_IAL, 1, [Define if include wvfb ial support])
    fi
}

dnl Set up the Common LCD video driver.
CheckCOMMLCD()
{
    AC_ARG_ENABLE(videocommlcd,
[  --enable-videocommlcd    include NEWGAL engine for Common LCD <default=no>],
    enable_video_commlcd=$enableval)

    if test "x$enable_video_commlcd" = "xyes"; then
      AC_DEFINE(_MGGAL_COMMLCD, 1,
                [Define if include NEWGAL engine for Common LCD])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS commlcd"
      VIDEO_DRIVERS="$VIDEO_DRIVERS commlcd/libvideo_commlcd.la"
    fi
}

dnl Set up the MLShadow video driver.
CheckMLShadowVideo()
{
    AC_ARG_ENABLE(videomlshadow,
[  --enable-videomlshadow   include MLShadow NEWGAL engine <default=no>],
    enable_video_mlshadow=$enableval)

    if test "x$enable_video_mlshadow" = "xyes"; then
      AC_DEFINE(_MGGAL_MLSHADOW, 1,
                [Define if include MLShadow NEWGAL engine])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS mlshadow"
      VIDEO_DRIVERS="$VIDEO_DRIVERS mlshadow/libvideo_mlshadow.la"
      DEP_LIBS="$DEP_LIBS -lpthread"
    fi
}

dnl Set up the Shadow video driver.
CheckShadowVideo()
{
    AC_ARG_ENABLE(videoshadow,
[  --enable-videoshadow     include Shadow NEWGAL engine <default=no>],
    enable_video_shadow=$enableval)

    if test "x$enable_video_shadow" = "xyes"; then
      AC_DEFINE(_MGGAL_SHADOW, 1,
                [Define if include Shadow NEWGAL engine])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS shadow"
      VIDEO_DRIVERS="$VIDEO_DRIVERS shadow/libvideo_shadow.la"
      DEP_LIBS="$DEP_LIBS -lpthread"
    fi
}

dnl Set up the EM85xx YUV video driver.
CheckEM86GFX()
{ AC_ARG_ENABLE(videoem86gfx,
[  --enable-videoem86gfx    include NEWGAL engine for EM86xx GFX <default=no>],
    enable_video_em86gfx=$enableval)

    if test "x$enable_video_em86gfx" = "xyes"; then
      AC_DEFINE(_MGGAL_EM86GFX, 1,
                [Define if include NEWGAL engine for EM86xx GFX])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS em86gfx"
      VIDEO_DRIVERS="$VIDEO_DRIVERS em86gfx/libvideo_em86gfx.la"
    fi
}

dnl Set up the EM85xx YUV video driver.
CheckEM85xxYUV()
{ AC_ARG_ENABLE(videoem85xxyuv,
[  --enable-videoem85xxyuv  include NEWGAL engine for EM85xx YUV <default=no>],
    enable_video_em85xxyuv=$enableval)

    if test "x$enable_video_em85xxyuv" = "xyes"; then
      AC_DEFINE(_MGGAL_EM85XXYUV, 1,
                [Define if include NEWGAL engine for EM85xx YUV])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS em85xxyuv"
      VIDEO_DRIVERS="$VIDEO_DRIVERS em85xxyuv/libvideo_em85xxyuv.la"
    fi
}

dnl Set up the EM85xx OSD video driver.
CheckEM85xxOSD()
{ AC_ARG_ENABLE(videoem85xxosd,
[  --enable-videoem85xxosd  include NEWGAL engine for EM85xx OSD <default=no>],
    enable_video_em85xxosd=$enableval)

    if test "x$enable_video_em85xxosd" = "xyes"; then
      AC_DEFINE(_MGGAL_EM85XXOSD, 1,
                [Define if include NEWGAL engine for EM85xx OSD])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS em85xxosd"
      VIDEO_DRIVERS="$VIDEO_DRIVERS em85xxosd/libvideo_em85xxosd.la"
    fi
}

dnl Set up svpxxosd video driver.
CheckSvpxxosdVideo()
{
    AC_ARG_ENABLE(videosvpxxosd,
[  --enable-videosvpxxosd   include SVPXXOSD NEWGAL engine <default=no>],
    enable_video_svpxxosd=$enableval)

    if test "x$enable_video_svpxxosd" = "xyes"; then
        AC_DEFINE(_MGGAL_SVPXXOSD, 1,
                [Define if include NEWGAL engine for SVPXX OSD])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS svpxxosd"
        VIDEO_DRIVERS="$VIDEO_DRIVERS svpxxosd/libvideo_svpxxosd.la"
    fi
}

dnl Set up the video driver for BF533 OSD via SPI
CheckBF533Video()
{
    AC_ARG_ENABLE(videobf533,
[  --enable-videobf533      include NEWGAL engine for BF533 OSD via SPI <default=no>],
    enable_video_bf533=$enableval)

    if test "x$enable_video_bf533" = "xyes"; then
      AC_DEFINE(_MGGAL_BF533, 1,
                [Define if include NEWGAL engine for BF533 OSD via SPI])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS bf533"
      VIDEO_DRIVERS="$VIDEO_DRIVERS bf533/libvideo_bf533.la"
    fi
}

dnl Set up the video driver for mb93493 YUV FrameBuffer driver
CheckMB93493Video()
{
    AC_ARG_ENABLE(videomb93493,
[  --enable-videomb93493    include NEWGAL engine for mb93493 YUV FrameBuffer driver <default=no>],
    enable_video_mb93493=$enableval)

    if test "x$enable_video_mb93493" = "xyes"; then
      AC_DEFINE(_MGGAL_MB93493, 1,
                [Define if include NEWGAL engine for mb93493 YUV FrameBuffer driver])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS mb93493"
      VIDEO_DRIVERS="$VIDEO_DRIVERS mb93493/libvideo_mb93493.la"
    fi
}

dnl Set up the video driver for UTPMC.
CheckUTPMCVideo()
{
    AC_ARG_ENABLE(videoutpmc,
[  --enable-videoutpmc      include NEWGAL engine for UTPMC <default=no>],
    enable_video_utpmc=$enableval)

    if test "x$enable_video_utpmc" = "xyes"; then
      AC_DEFINE(_MGGAL_UTPMC, 1,
                [Define if include NEWGAL engine for UTPMC])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS utpmc"
      VIDEO_DRIVERS="$VIDEO_DRIVERS utpmc/libvideo_utpmc.la"
    fi
}

dnl Set up the video driver for DirectFB.
CheckDirectFBVideo()
{
    AC_ARG_ENABLE(videodfb,
[  --enable-videodfb        include NEWGAL engine for DirectFB <default=no>],
    enable_video_dfb=$enableval)

    if test x$enable_video_dfb = xyes; then
        AC_MSG_CHECKING(for DirectFB support)
        video_dfb=no
        AC_TRY_COMPILE([
         #include <directfb.h>
        ],[
        ],[
        video_dfb=yes
        ])
        AC_MSG_RESULT($video_dfb)
        if test x$video_dfb = xyes; then
            AC_DEFINE(_MGGAL_DFB, 1,
                [Define if include NEWGAL engine for DirectFB])
            VIDEO_SUBDIRS="$VIDEO_SUBDIRS dfb"
            VIDEO_DRIVERS="$VIDEO_DRIVERS dfb/libvideo_dfb.la"
            LIBS="$LIBS -ldirectfb"

            dnl Set up the video driver for ST7167.
            AC_ARG_ENABLE(videost7167,
[  --enable-videost7167     include NEWGAL engine for ST7167 on DirectFB <default=no>],
    enable_video_dfb_st7167=$enableval)

            if test x$enable_video_dfb_st7167 = xyes; then
                AC_DEFINE(_MGGAL_DFB_ST7167, 1,
                    [Define if include ST7167 subdriver for NEWGAL engine of DirectFB])
            fi
        fi
    fi
}

dnl Set up the video driver for STGFB(ST7105,ST7109,ST7167 native ioctl).
CheckSTGFBVideo()
{
    AC_ARG_ENABLE(videostgfb,
[  --enable-videostgfb      include NEWGAL engine for STGFB <default=no>],
    enable_video_stgfb=$enableval)

    if test "x$enable_video_stgfb" = "xyes"; then
        AC_DEFINE(_MGGAL_STGFB, 1,
                [Define if include NEWGAL engine for STGFB])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS stgfb"
        VIDEO_DRIVERS="$VIDEO_DRIVERS stgfb/libvideo_stgfb.la"
        CFLAGS="$CFLAGS -DST_OSLINUX -DST_7105 -DARCHITECTURE_ST40 -DDEFINED_BOOL"
    fi
}

dnl Check HI3510/HI3560 video driver
CheckHI35XXVideo()
{
    AC_ARG_ENABLE(videohi35xx,
[  --enable-videohi35xx     include Hi35xx Video  NEWGAL engine <default=no>],
    enable_video_hi35xx=$enableval)

    if test "x$enable_video_hi35xx" = "xyes"; then
        AC_DEFINE(_MGGAL_HI3510, 1,
                [Define if include Hi35XX Video NEWGAL engine])
        AC_DEFINE(_MGGAL_HI3560, 1,
                [Define if include Hi35XX Video NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS hisi"
        VIDEO_DRIVERS="$VIDEO_DRIVERS hisi/libvideo_hisifbcon.la"
    fi
}

dnl Check HI3560A video driver
CheckHI3560AVideo()
{
    AC_ARG_ENABLE(videohi3560a,
[  --enable-videohi3560a    include Hi3560A Video  NEWGAL engine <default=no>],
    enable_video_hi3560a=$enableval)

    if test "x$enable_video_hi3560a" = "xyes"; then
        AC_DEFINE(_MGGAL_HI3560A, 1,
                [Define if include Hi3560A Video NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS hisi"
        VIDEO_DRIVERS="$VIDEO_DRIVERS hisi/libvideo_hisifbcon.la"
    fi
}

dnl Check Intel GDL video driver
CheckGDLVideo()
{
    AC_ARG_ENABLE(videogdl,
[  --enable-videogdl        include GDL Video NEWGAL engine <default=no>],
    enable_video_gdl=$enableval)

    if test "x$enable_video_gdl" = "xyes"; then
        AC_DEFINE(_MGGAL_GDL, 1,
                [Define if include GDL Video NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS gdl"
        VIDEO_DRIVERS="$VIDEO_DRIVERS gdl/libvideo_gdl.la"
    fi
}

dnl Set up the sigma video driver.
CheckSigmaVideo()
{
    AC_ARG_ENABLE(videosigma8654,
[  --enable-videosigma8654  include sigma8654 NEWGAL engine <default=no>],
                  enable_video_sigma8654=$enableval)

    if test "x$enable_video_sigma8654" = "xyes"; then
        AC_DEFINE(_MGGAL_SIGMA8654, 1,
                  [Define if include sigma8654 NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS sigma8654 "
        VIDEO_DRIVERS="$VIDEO_DRIVERS sigma8654/libvideo_sigma8654.la"
    fi
}

dnl Set up the mstar video driver.
CheckMStarVideo()
{
    AC_ARG_ENABLE(videomstar,
[  --enable-videomstar      include mstar NEWGAL engine <default=no>],
                  enable_video_mstar=$enableval)

    if test "x$enable_video_mstar" = "xyes"; then
        AC_DEFINE(_MGGAL_MSTAR, 1,
                  [Define if include mstar NEWGAL engine])
        VIDEO_SUBDIRS="$VIDEO_SUBDIRS mstar "
        VIDEO_DRIVERS="$VIDEO_DRIVERS mstar/libvideo_mstar.la"
    fi
}

dnl Set up the custom video driver.
CheckCustomVideo()
{
    AC_ARG_ENABLE(videocustom,
[  --enable-videocustom     include custom NEWGAL engine <default=no>],
                  enable_video_custom=$enableval)

    if test "x$enable_video_custom" = "xyes"; then
        AC_DEFINE(_MGGAL_CUSTOMGAL, 1,
                  [Define if include custom NEWGAL engine])
    fi
}

dnl Set up the Nexus video driver.
CheckNexusVideo()
{
    AC_ARG_ENABLE(videonexus,
[  --enable-videonexus      include nexus NEWGAL engine <default=no>],
    enable_video_nexus=$enableval)

    if test "x$enable_video_nexus" = "xyes"; then
      AC_DEFINE(_MGGAL_NEXUS, 1,
                [Define if include nexus NEWGAL engine])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS nexus"
      VIDEO_DRIVERS="$VIDEO_DRIVERS nexus/libvideo_nexus.la"
      DEP_LIBS="$DEP_LIBS -lnexus -lpthread -lm -lz"
    fi
}


AC_ARG_WITH(nexus-includedir,
[  --with-nexus-includedir=DIR  Where to find header files of nexus library ],
[ case "$withval" in
    yes|no)
        AC_MSG_WARN([--with-nexus-includedir called without argument - will use default])
       ;;
    * )
        NEXUS_INCLUDEDIR="$withval"
        AC_SUBST(NEXUS_INCLUDEDIR)
        ;;
esac ]
)

AC_ARG_WITH(nexus-libdir,
[  --with-nexus-libdir=DIR      Where to find .so files of nexus library ],
[ case "$withval" in
    yes|no)
        AC_MSG_WARN([--with-nexus-libdir called without argument - will use default])
        ;;
    * )
        NEXUS_LIBDIR="$withval"
        AC_SUBST(NEXUS_LIBDIR)
        ;;
esac ]
)

dnl Set up the s3c6410 video driver.
CheckS3C6410Video()
{
    AC_ARG_ENABLE(videos3c6410,
[  --enable-videos3c6410    include s3c6410 NEWGAL engine <default=no>],
    enable_video_s3c6410=$enableval)

    if test "x$enable_video_s3c6410" = "xyes"; then
      AC_DEFINE(_MGGAL_S3C6410, 1,
                [Define if include s3c6410 NEWGAL engine])
      VIDEO_SUBDIRS="$VIDEO_SUBDIRS s3c6410"
      VIDEO_DRIVERS="$VIDEO_DRIVERS s3c6410/libvideo_s3c6410.la"
    fi
}

dnl ========================================================================
dnl Checks for option-specific libraries and headers.

dnl Checks NewGAL Engine.
{
  CheckDummyVideo
  CheckFBCON
  CheckQVFB
  CheckXVFB
  CheckWVFB
  CheckCOMMLCD
  CheckShadowVideo
  CheckMLShadowVideo
  CheckEM85xxOSD
  CheckEM85xxYUV
  CheckEM86GFX
  CheckSvpxxosdVideo
  CheckBF533Video
  CheckMB93493Video
  CheckUTPMCVideo
  CheckDirectFBVideo
  CheckSTGFBVideo
  CheckHI35XXVideo
  CheckHI3560AVideo
  CheckGDLVideo
  CheckNexusVideo
  CheckS3C6410Video
  CheckSigmaVideo
  CheckMStarVideo
  CheckCustomVideo
}

dnl ========================================================================
dnl Checks for libraries.

dnl Check for JPEG library.
if test "x$build_jpg_support" != "xno"; then
    AC_CHECK_LIB(jpeg, jpeg_std_error,
        DEP_LIBS="$DEP_LIBS -ljpeg",
        build_jpg_support=no, -lm)
fi

dnl Check for PNG library.
if test "x$build_png_support" != "xno"; then
    AC_CHECK_LIB(png, png_check_sig,
        DEP_LIBS="$DEP_LIBS -lpng",
        build_png_support=no, -lz -lm)
fi

AC_DEFINE_UNQUOTED(MINIGUI_MAJOR_VERSION, $MINIGUI_MAJOR_VERSION,
            [Major version of MiniGUI])
AC_DEFINE_UNQUOTED(MINIGUI_MINOR_VERSION, $MINIGUI_MINOR_VERSION,
            [Minor version of MiniGUI])
AC_DEFINE_UNQUOTED(MINIGUI_MICRO_VERSION, $MINIGUI_MICRO_VERSION,
            [Micro version of MiniGUI])
AC_DEFINE_UNQUOTED(MINIGUI_INTERFACE_AGE, $MINIGUI_INTERFACE_AGE,
            [Interface age of MiniGUI])
AC_DEFINE_UNQUOTED(MINIGUI_BINARY_AGE, $MINIGUI_BINARY_AGE,
            [Binary age of MiniGUI])

dnl ========================================================================
dnl Finally add some flags

if test "x$build_custom_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_CUSTOM, 1, [Define if include IAL engine for customer board])
fi

if test "x$build_dlcustom_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_DLCUSTOM, 1, [Define if include dlcustom IAL engine])
    DEP_LIBS="$DEP_LIBS -ldl"
fi

if test "x$build_sef_support" = "xyes"; then
    DEP_LIBS="$DEP_LIBS -lscripteasy"
fi

if test "x$build_net_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_NET, 1,
            [Define if include IAL engine for net])
fi

AC_ARG_WITH(libsuffix,
[  --with-libsuffix=ARG     Configure the suffix of MiniGUI library name.])

case "$with_libsuffix" in
    mvfb)
        conf="mvfb.cfg"
        MGLIB_SUFFIX="mvfb"
    ;;
    "")
        conf="MiniGUI.cfg"
        case "$runtime_mode" in
            sa)
                MGLIB_SUFFIX="sa"
            ;;
            ths)
                MGLIB_SUFFIX="ths"
            ;;
            *)
                MGLIB_SUFFIX="procs"
            ;;
        esac
    ;;
    *)
        conf="MiniGUI.cfg"
        MGLIB_SUFFIX="$withval"
    ;;
esac

AC_ARG_WITH(osname,
   [  --with-osname=linux/uclinux/ecos/ucos2/swlinux/vxworks/win32/darwin/threadx/cygwin/nucleus/ose/psos])

dnl configure.in file is used only for the following OSes: linux/uclinux/swlinux/ecos/darwin/cygwin
dnl for other OSes, you should use makefile.ng or else.

case "$with_osname" in
  uclinux)
    AC_DEFINE(__uClinux__, 1,
            [Define if compile for uClinux])
    ;;
  ecos)
    AC_DEFINE(__ECOS__, 1,
            [Define if compile for eCos])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    ;;
  ucos2)
    AC_DEFINE(__UCOSII__, 1,
            [Define if compile for uC/OS-II])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    incore_res="yes"
    use_own_malloc="yes"
    use_own_stdio="yes"
    use_own_pthread="yes"
    ;;
  swlinux)
    AC_DEFINE(__WINBOND_SWLINUX__, 1,
                [Define if compile for Winbond SWLinux])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    ;;
  vxworks)
    AC_DEFINE(__VXWORKS__, 1,
            [Define if compile for VxWorks])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    ;;
  cygwin)
    AC_DEFINE(__CYGWIN__, 1,
            [Define if compile for Cygwin platform])
    ;;
  win32)
    AC_DEFINE(WIN32, 1,
            [Define if compile for Win32 platform])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    ;;
  darwin)
    AC_DEFINE(__DARWIN__, 1,
            [Define if compile for OpenDarwin])
    ;;
  threadx)
    AC_DEFINE(__THREADX__, 1,
            [Define if compile for ThreadX])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    incore_res="yes"
    use_own_malloc="yes"
    use_own_stdio="yes"
    use_own_pthread="yes"
    ;;
  nucleus)
    AC_DEFINE(__NUCLEUS__, 1,
            [Define if compile for Nucleus])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    incore_res="yes"
    use_own_malloc="yes"
    use_own_stdio="yes"
    use_own_pthread="yes"
    ;;
  ose)
    AC_DEFINE(__OSE__, 1,
            [Define if compile for OSE])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    incore_res="yes"
    use_own_pthread="yes"
    ;;
  psos)
    AC_DEFINE(__PSOS__, 1,
            [Define if compile for pSOS])
    AC_DEFINE(__NOUNIX__, 1,
            [Define if compile for non-UNIX like OS])
    ;;
  *)
    with_osname="linux"
    AC_DEFINE(__LINUX__, 1,
            [Define if compile for Linux])
    ;;
esac

dnl AC_ARG_WITH(licensepicture,
dnl   [  --with-licensepicture=minigui/mdolphin/hybridos])
dnl case "$with_licensepicture" in
dnl     minigui|mdolphin|hybridos)
dnl         LICENSE_PICTURE=$with_licensepicture
dnl         ;;
dnl     *)
dnl         LICENSE_PICTURE=minigui
dnl         ;;
dnl esac

AC_ARG_WITH(targetname,
  [  --with-targetname=unknown/customer/fmsoft/mstudio/stb810/vfanvil/vxi386/qvfb/fbcon/mx21/monaco/c33l05/bfin/vxppc/
                        S3C6410/S3C2440/S3C2410/hi3560a     Define the target board name])

case "$with_targetname" in
  fmsoft)
    AC_DEFINE(__TARGET_FMSOFT__, 1,
	    [Define for FMSoft internal use])
   ;;
  mstudio)
    AC_DEFINE(__TARGET_MSTUDIO__, 1,
	    [Define for FMSoft miniStudio])
   ;;
  stb810)
    AC_DEFINE(__TARGET_STB810__, 1,
            [Define for Philips STB810 target])
    ;;
  vfanvil)
    AC_DEFINE(__TARGET_VFANVIL__, 1,
            [Define for VirualFone ANVIL target])
    ;;
  monaco)
    AC_DEFINE(__TARGET_MONACO__, 1,
            [Define for Monaco ANVIL target])
    ;;
  mx21)
    AC_DEFINE(__TARGET_MX21__, 1,
            [Define for OSE on mx21])
    ;;
  vxi386)
    AC_DEFINE(__TARGET_VXI386__, 1,
            [Define for VxWorks on i386])
    ;;
  vxppc)
    AC_DEFINE(__TARGET_PPC__, 1,
            [Define for VxWorks on PowerPC])
    ;;
  bfin)
    AC_DEFINE(__TARGET_BLACKFIN__, 1,
	    [Define for Blackfin run uClinux])
   ;;
  c33l05)
    AC_DEFINE(__TARGET_C33L05__, 1,
	    [Define for EPSON C33L05 (axLinux)])
   ;;
  S3C6410)
    CFLAGS="$CFLAGS -D_WITH_TARGET_S3C6410"
   ;;
  S3C2440)
    CFLAGS="$CFLAGS -D_WITH_TARGET_S3C2440"
   ;;
  S3C2410)
    CFLAGS="$CFLAGS -D_WITH_TARGET_S3C2410"
   ;;
  hi3560a)
    CFLAGS="$CFLAGS -D_WITH_TARGET_HI3560A"
   ;;
  *)
    with_targetname="unknown"
    CFLAGS="$CFLAGS -D_WITH_TARGET_NONE"
    AC_DEFINE(__TARGET_UNKNOWN__, 1,
            [Define for unknown target])
    ;;
esac


if test "x$with_flat_lf" = "xyes"; then
    AC_DEFINE(_MGLF_RDR_FLAT, 1,
            [Define if include flat Look and Feel])
fi

if test "x$with_skin_lf" = "xyes"; then
    AC_DEFINE(_MGLF_RDR_SKIN, 1,
            [Define if include skin Look and Feel])
fi

if test "x$use_own_malloc" = "xyes"; then
  AC_DEFINE(_MGUSE_OWN_MALLOC, 1,
            [Define if use own implementation of malloc functions])
fi

if test "x$use_own_stdio" = "xyes"; then
  AC_DEFINE(_MGUSE_OWN_STDIO, 1,
            [Define if use own implementation of stdio functions])
fi

if test "x$use_own_pthread" = "xyes"; then
  AC_DEFINE(_MGUSE_OWN_PTHREAD, 1,
            [Define if use own implementation of pthread functions])
fi

CFLAGS="$CFLAGS -D__MINIGUI_LIB__"

case "$runtime_mode" in
    sa)
        AC_DEFINE(_MGRM_STANDALONE, 1,
            [Define if build MiniGUI-Standalone])
        AC_DEFINE(_STAND_ALONE, 1,
            [Define if build MiniGUI-Standalone (back-compatibility definition)])
        AC_DEFINE(_LITE_VERSION, 1,
            [Define if build MiniGUI-Standalone (back-compatibility definition)])
        MINIGUI_RUNMODE="sa"
    ;;
    ths)
        AC_DEFINE(_MGRM_THREADS, 1,
            [Define if build MiniGUI-Threads])
        CFLAGS="$CFLAGS -D_REENTRANT"
        MINIGUI_RUNMODE="ths"
    ;;
    *)
        AC_DEFINE(_MGRM_PROCESSES, 1,
            [Define if build MiniGUI-Processes])
        AC_DEFINE(_LITE_VERSION, 1,
            [Define if build MiniGUI-Processes (back-compatibility definition)])
        MINIGUI_RUNMODE="procs"
    ;;
esac

if test "x$incore_res" = "xno"; then
    AC_DEFINE_UNQUOTED(ETCFILENAME, "${conf}", [MiniGUI configure file name])
fi

AC_SUBST(MGLIB_SUFFIX)
AC_DEFINE_UNQUOTED(_MGLIB_SUFFIX, "${MGLIB_SUFFIX}", [MiniGUI library suffix])

if test "x$use_miniguientry" = "xyes"; then
  AC_DEFINE(_USE_MINIGUIENTRY, 1,
            [Define if use minigui_entry function in MiniGUI])
fi

if test "x$incore_res" = "xyes"; then
  AC_DEFINE(_MGINCORE_RES, 1,
            [Define if build MiniGUI for no file I/O system (use in-core resources)])
fi

no_math="no"
AC_CHECK_LIB(m, pow, DEP_LIBS="$DEP_LIBS -lm", no_math="yes")

if test "x$no_math" = "xno"; then
    AC_DEFINE(_HAVE_MATH_LIB, 1,
            [Define if have math library])
fi

if test "x$no_math" = "xyes"; then
    build_adv2dapi="no"
fi

if test "x$build_adv2dapi" = "xyes"; then
    AC_DEFINE(_MGHAVE_ADV_2DAPI, 1,
        [Define if include advanced 2D graphics APIs])
fi

if test "x$fixed_math" = "xyes"; then
    AC_DEFINE(_MGHAVE_FIXED_MATH, 1,
            [Define if include fixed math routines])
fi

if test "x$devel_mode" = "xyes"; then
    message_string="yes"
    if test "x$detail_debug" = "xyes"; then
        trace_message="yes"
    fi

    build_auto_ial_engine="yes"
    build_random_ial_engine="yes"
    build_net_ial_engine="yes"

    build_qpf_support="yes"

    build_latin2_support="yes"
    build_latin3_support="yes"
    build_latin4_support="yes"
    build_cyrillic_support="yes"
    build_arabic_support="yes"
    build_greek_support="yes"
    build_hebrew_support="yes"
    build_latin5_support="yes"
    build_latin6_support="yes"
    build_thai_support="yes"
    build_latin7_support="yes"
    build_latin8_support="yes"
    build_latin10_support="yes"

    build_gb18030_support="yes"
    build_big5_support="yes"
    build_euckr_support="yes"
    build_eucjp_support="yes"
    build_shiftjis_support="yes"

    use_kbd_hebrewpc="yes"
    use_kbd_arabicpc="yes"
    use_kbd_frpc="yes"
    use_kbd_fr="yes"
    use_kbd_de="yes"
    use_kbd_delatin1="yes"
    use_kbd_it="yes"
    use_kbd_es="yes"
    use_kbd_escp850="yes"

    build_ctrl_bidisledit="yes"
    build_ctrl_textedit="yes"
    build_ctrl_scrollview="yes"
    build_ctrl_monthcal="yes"
    build_ctrl_treeview="yes"
    build_ctrl_treeview_rdr="yes"
    build_ctrl_spinbox="yes"
    build_ctrl_coolbar="yes"
    build_ctrl_listview="yes"
    build_ctrl_iconview="yes"
    build_ctrl_gridview="yes"

    enable_video_shadow="yes"
    enable_video_mlshadow="yes"
    enable_video_custom="yes"
fi

dnl Depedencies
if test "x$build_ctrl_treeview" = "xyes"; then
    build_ctrl_scrollview="yes"
fi

if test "x$build_ctrl_treeview_rdr" = "xyes"; then
    build_ctrl_scrollview="yes"
fi

if test "x$trace_message" = "xyes"; then
    message_string="yes"

    AC_DEFINE(_MGHAVE_TRACE_MSG, 1,
            [Define if trace message dispatching of MiniGUI])
fi

if test "x$message_string" = "xyes"; then
    AC_DEFINE(_MGHAVE_MSG_STRING, 1,
            [Define if include message string names])
fi

AC_DEFINE(_MGTIMER_UNIT_10MS, 1,
            [Define if the unit of timer is 10ms])

if test "x$double_click" = "xyes"; then
    AC_DEFINE(_MGMISC_DOUBLE_CLICK, 1,
            [Define if mouse button can do double click])
fi

if test "x$build_dummy_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_DUMMY, 1,
            [Define if include the dummy IAL engine])
fi

if test "x$build_auto_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_AUTO, 1,
            [Define if include the automatic IAL engine])
fi

if test "x$build_random_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_RANDOM, 1,
            [Define if include the random IAL engine])
fi

if test "x$build_qvfb_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_QVFB, 1,
            [Define if include the QVFB IAL engine])
fi

if test "x$build_wvfb_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_WVFB, 1,
            [Define if include the WVFB IAL engine])
fi

if test "x$build_jz4740_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_JZ4740, 1,
            [Define if include the JZ4740 IAL engine])
fi

if test "x$build_lide_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_LIDE, 1,
            [Define if include the lide IAL engine])
fi

if test "x$build_2440_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_2440, 1,
            [Define if include the 2440 IAL engine])
fi

if test "x$build_davinci6446_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_DAVINCI6446, 1,
            [Define if include the DAVINCI6446 IAL engine])
fi

if test "x$build_dfb_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_DFB, 1,
            [Define if include the DFB IAL engine])
fi

if test "x$build_qemu_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_QEMU, 1,
            [Define if include the QEMU IAL engine])
fi

if test "x$build_custom_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_CUSTOM, 1,
            [Define if include IAL engine for customer's board])
fi

if test "x$build_net_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_NET, 1,
            [Define if include IAL engine for net's board])
fi

if test "x$build_comm_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_COMM, 1,
            [Define if include the common IAL engine])
fi

if test "x$build_cisco_touchpad_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_CISCO_TOUCHPAD, 1,
            [Define if include IAL engine for Cisco touchpad])
    DEP_LIBS="$DEP_LIBS -lhid -lusb -lusb-1.0"
fi

if test "x$build_mstar_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_MSTAR, 1,
            [Define if include IAL engine for MStar])
fi

if test "x$build_ipaq_h3600_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_IPAQ_H3600, 1,
            [Define if include IAL engine for iPAQ H3600])
fi

if test "x$build_nexus_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_NEXUS, 1,
            [Define if include IAL engine for Nexus])
fi

if test "x$build_ipaq_h5400_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_IPAQ_H5400, 1,
            [Define if include IAL engine for iPAQ H5400])
fi

if test "x$build_tslib_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_TSLIB, 1,
            [Define if include IAL engine for TSLIB])
    DEP_LIBS="$DEP_LIBS -lts"
fi

if test "x$build_console_ial_engine" = "xyes"; then
    AC_DEFINE(_MGIAL_CONSOLE, 1,
            [Define if include console (Linux console) IAL engine])
fi

if test "x$build_console_ps2" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_PS2, 1,
            [Define if include PS2 mouse subdriver])
fi

if test "x$build_console_imps2" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_IMPS2, 1,
            [Define if include IMPS2 mouse subdriver])
fi

if test "x$build_console_ms" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_MS, 1,
            [Define if include MS mouse subdriver])
fi

if test "x$build_console_ms3" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_MS3, 1,
            [Define if include MS3 mouse subdriver])
fi

if test "x$build_console_gpm" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_GPM, 1,
            [Define if include GPM mouse subdriver])
fi

if test "x$build_text_mode" = "xyes"; then
    AC_DEFINE(_MGCONSOLE_TEXTMODE, 1,
            [Define if your Linux have text mode])
fi

if test "x$build_cursor_support" = "xyes"; then
    AC_DEFINE(_MGHAVE_CURSOR, 1,
            [Define if include cursor support])
fi

if test "x$build_clipboard" = "xyes"; then
    AC_DEFINE(_MGHAVE_CLIPBOARD, 1,
            [Define if include clipboard support])
fi

if test "x$build_rbf_support" = "xyes"; then
    AC_DEFINE(_MGFONT_RBF, 1,
            [Define if support raw bitmap fonts])

  if test "x$build_incorerbf_vgaoem" = "xyes"; then
    AC_DEFINE(_MGINCORERBF_LATIN1_VGAOEM, 1,
            [Define if include in-core VGAOEM RBF for ISO8859-1])
  fi

  if test "x$build_incorerbf_terminal" = "xyes"; then
    AC_DEFINE(_MGINCORERBF_LATIN1_TERMINAL, 1,
            [Define if include in-core Terminal RBF for ISO8859-1])
  fi

  if test "x$build_incorerbf_fixedsys" = "xyes"; then
    AC_DEFINE(_MGINCORERBF_LATIN1_FIXEDSYS, 1,
            [Define if include in-core FixedSys RBF for ISO8859-1])
  fi

fi

if test "x$build_vbf_support" = "xyes"; then
    AC_DEFINE(_MGFONT_VBF, 1,
            [Define if support var bitmap fonts])

  if test "x$build_incorefont_sansserif" = "xyes"; then
    AC_DEFINE(_MGINCOREFONT_SANSSERIF, 1,
            [Define if include in-core font: SansSerif])
  fi

  if test "x$build_incorefont_courier" = "xyes"; then
    AC_DEFINE(_MGINCOREFONT_COURIER, 1,
            [Define if include in-core font: Courier])
  fi

  if test "x$build_incorefont_system" = "xyes"; then
    AC_DEFINE(_MGINCOREFONT_SYSTEM, 1,
            [Define if include in-core font: System])
  fi

fi

if test "x$build_sef_support" = "xyes"; then

    build_unicode_support="yes"

    AC_DEFINE(_MGFONT_SEF, 1,
            [Define if support SEF scripteary font])
fi

if test "x$build_qpf_support" = "xyes"; then

    build_unicode_support="yes"

    AC_DEFINE(_MGFONT_QPF, 1,
            [Define if support QPF font])
fi

if test "x$build_upf_support" = "xyes"; then

    build_unicode_support="yes"

    AC_DEFINE(_MGFONT_UPF, 1,
            [Define if support UPF font])

    if test "x$build_incorefont_times" = "xyes"; then
        AC_DEFINE(_MGINCOREFONT_TIMES, 1,
            [Define if include in-core UPF Times fonts])
    fi
fi

dnl Check FreeType lib
case "$use_ttf_lib" in
  ft1)
dnl Check for FreeType1 library
    AC_CHECK_LIB(ttf, TT_FreeType_Version,
            DEP_LIBS="$DEP_LIBS -lttf",
            use_ttf_lib="none")
    ;;
  ft2)
dnl Check for FreeType2 library
    AC_CHECK_LIB(freetype, FT_Library_Version,
        DEP_LIBS="$DEP_LIBS -lfreetype -lz -lpng12",
        use_ttf_lib="none")
    ;;
esac

dnl Dealing with TTF cache size 
case "$use_ttf_lib" in
    ft1|ft2)
    if test "x$build_ttfcache_support" = "xyes"; then
        AC_DEFINE(_MGFONT_TTF_CACHE, 1,
                  [Define if include ttf cache])

        AC_ARG_WITH(ttfcachesize,
                    [  --with-ttfcachesize=[64/128/256/512/1024]])

        case "$with_ttfcachesize" in
            64)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 64,
                          [Define if compile max ttf cahce size for 64k])
            ;;
            128)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 128,
                          [Define if compile max ttf cahce size for 128k])
            ;;
            256)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 256,
                          [Define if compile max ttf cahce size for 256k])
            ;;
            512)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 512,
                          [Define if compile max ttf cahce size for 512k])
            ;;
            1024)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 1024,
                          [Define if compile max ttf cahce size for 1024k])
            ;;
            *)
                AC_DEFINE(_MGTTF_CACHE_SIZE, 256,
                          [Define if compile max ttf cahce size for 256k])
            ;;
        esac

        AC_ARG_WITH(ttfcachenum,
                    [  --with-mttfcachenum=[10/20/40]])
        case "$with_mttfcachenum" in
            10)
                AC_DEFINE(_MGMAX_TTF_CACHE, 10,
                          [Define if compile max ttf cahce number for 10])
            ;;
            20)
                AC_DEFINE(_MGMAX_TTF_CACHE, 20,
                          [Define if compile max ttf cahce number for 20])
            ;;
            40)
                AC_DEFINE(_MGMAX_TTF_CACHE, 40,
                          [Define if compile max ttf cahce number for 40])
            ;;
            *)
                AC_DEFINE(_MGMAX_TTF_CACHE, 10,
                          [Define if compile max ttf cahce number for 10 (default value)])
            ;;
        esac
    fi
    ;;
esac

dnl Dealing with CFLAGS and LDFLAGS
case "$use_ttf_lib" in
    ft1)
        build_unicode_support="yes"

        AC_DEFINE(_MGFONT_TTF, 1,
                 [Define if support TrueType font])

        AC_ARG_WITH(ft1_includes,
               [  --with-ft1-includes=DIR    where the FreeType1 includes are])

        if test "x$with_ft1_includes" = "x"; then
            FT1_INC_DIR="-I/usr/include/freetype1"
        else
            FT1_INC_DIR="-I$with_ft1_includes"
        fi

        DEP_LIBS="$DEP_LIBS -lttf"
        AC_SUBST(FT1_INC_DIR)
    ;;
    ft2)
        build_unicode_support="yes"
        AC_DEFINE(_MGFONT_FT2, 1,
             [Define if support TrueType font based on FreeType2])

        AC_ARG_WITH(ft2_includes,
               [  --with-ft2-includes=DIR    where the FreeType2 includes are])

        if test "x$with_ft2_includes" = "x"; then
            FT2_INC_DIR="-I/usr/include/freetype2"
        else
            FT2_INC_DIR="-I$with_ft2_includes"
        fi

        DEP_LIBS="$DEP_LIBS -lfreetype"
        AC_SUBST(FT2_INC_DIR)
    ;;
esac

if test "x$build_bmpf_support" = "xyes"; then
    AC_DEFINE(_MGFONT_BMPF, 1,
            [Define if support Bitmap fonts])
fi


if test "x$build_latin2_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN2, 1,
            [Define if support Latin 2 charset])
fi

if test "x$build_latin3_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN3, 1,
            [Define if support Latin 3 charset])
fi

if test "x$build_latin4_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN4, 1,
            [Define if support Latin 4 charset])
fi

if test "x$build_cyrillic_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_CYRILLIC, 1,
            [Define if support Cyrillic charset])
fi

if test "x$build_arabic_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_ARABIC, 1,
            [Define if support Arabic charset])
fi

if test "x$build_greek_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_GREEK, 1,
            [Define if support Greek charset])
fi

if test "x$build_hebrew_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_HEBREW, 1,
            [Define if support Hebrew charset])
fi

if test "x$build_latin5_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN5, 1,
            [Define if support Latin 5 charset])
fi

if test "x$build_latin6_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN6, 1,
            [Define if support Latin 6 charset])
fi

if test "x$build_thai_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_THAI, 1,
            [Define if support Thai charset])
fi

if test "x$build_latin7_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN7, 1,
            [Define if support Latin 7 charset])
fi

if test "x$build_latin8_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN8, 1,
            [Define if support Latin 8 charset])
fi

if test "x$build_latin9_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN9, 1,
            [Define if support Latin 9 charset])
fi

if test "x$build_latin10_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_LATIN10, 1,
            [Define if support Latin 10 charset])
fi

if test "x$build_gb_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_GB, 1,
            [Define if support GB2312 charset])
fi

if test "x$build_gbk_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_GBK, 1,
            [Define if support GBK charset])
fi

if test "x$build_gb18030_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_GB18030, 1,
            [Define if support GB18030 charset])
fi

if test "x$build_big5_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_BIG5, 1,
            [Define if support BIG5 charset])
fi

if test "x$build_euckr_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_EUCKR, 1,
            [Define if support EUCKR charset])
fi

if test "x$build_eucjp_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_EUCJP, 1,
            [Define if support EUCJP charset])
fi

if test "x$build_shiftjis_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_SHIFTJIS, 1,
            [Define if support SHIFTJIS charset])
fi

if test "x$build_unicode_support" = "xyes"; then
    AC_DEFINE(_MGCHARSET_UNICODE, 1,
            [Define if support UNICODE])
fi

if test "x$use_kbd_hebrewpc" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_HEBREWPC, 1,
            [Define if use the Hebrew PC keyboard layout])
fi

if test "x$use_kbd_arabicpc" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_ARABICPC, 1,
            [Define if use the Arabic PC keyboard layout])
fi

if test "x$use_kbd_frpc" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_FRPC, 1,
            [Define if use the French PC keyboard layout])
fi

if test "x$use_kbd_fr" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_FR, 1,
            [Define if use the French keyboard layout])
fi

if test "x$use_kbd_de" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_DE, 1,
            [Define if use the German keyboard layout])
fi

if test "x$use_kbd_delatin1" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_DELATIN1, 1,
            [Define if use the German-Latin1 keyboard layout])
fi

if test "x$use_kbd_it" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_IT, 1,
            [Define if use the Italian keyboard layout])
fi

if test "x$use_kbd_es" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_ES, 1,
            [Define if use the Spanish keyboard layout])
fi

if test "x$use_kbd_escp850" = "xyes"; then
    AC_DEFINE(_MGKBDLAYOUT_ESCP850, 1,
            [Define if use the Spanish CP850 keyboard layout])
fi

if test "x$build_save_bitmap" = "xyes"; then
    AC_DEFINE(_MGMISC_SAVEBITMAP, 1,
            [Define if include SaveBitmap function])
else
    build_misc_savescreen="no"
fi

if test "x$build_pcx_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_PCX, 1,
            [Define if support PCX bmp file format])
fi

if test "x$build_lbm_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_LBM, 1,
            [Define if support LBM bmp file format])
fi

if test "x$build_tga_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_TGA, 1,
            [Define if support TGA bmp file format])
fi

if test "x$build_gif_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_GIF, 1,
            [Define if support GIF bmp file format])
fi

if test "x$build_jpg_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_JPG, 1,
            [Define if support JPEG bmp file format])
fi

if test "x$build_png_support" = "xyes"; then
    AC_DEFINE(_MGIMAGE_PNG, 1,
            [Define if support PNG bmp file format])
fi

if test "x$build_menu" = "xyes"; then
    AC_DEFINE(_MGHAVE_MENU, 1,
            [Define if support menu])
else
    build_ctrl_menubutton="no"
fi

if test "x$build_misc_mousecalibrate" = "xyes"; then
    AC_DEFINE(_MGHAVE_MOUSECALIBRATE, 1,
            [Define if include code for mouse calibration])
fi

if test "x$build_misc_aboutdlg" = "xyes"; then
  if test "x$build_ctrl_button" = "xyes"; then
    AC_DEFINE(_MGMISC_ABOUTDLG, 1,
            [Define if include About MiniGUI Dialog Box])
  fi
fi

if test "x$build_misc_savescreen" = "xyes"; then
    AC_DEFINE(_MGMISC_SAVESCREEN, 1,
            [Define if include code for screenshots])
fi

if test "x$build_ctrl_static" = "xyes"; then
    AC_DEFINE(_MGCTRL_STATIC, 1,
            [Define if include STATIC control])
fi

if test "x$build_ctrl_button" = "xyes"; then
    AC_DEFINE(_MGCTRL_BUTTON, 1,
            [Define if include BUTTON control])
fi

if test "x$build_ctrl_sledit" = "xyes"; then
    AC_DEFINE(_MGCTRL_SLEDIT, 1,
            [Define if include SLEDIT control])
fi

if test "x$build_ctrl_bidisledit" = "xyes"; then
    AC_DEFINE(_MGCTRL_BIDISLEDIT, 1,
            [Define if include BIDISLEDIT control])
fi

if test "x$build_ctrl_listbox" = "xyes"; then
    AC_DEFINE(_MGCTRL_LISTBOX, 1,
            [Define if include LISTBOX control])
fi

if test "x$build_ctrl_progressbar" = "xyes"; then
    AC_DEFINE(_MGCTRL_PROGRESSBAR, 1,
            [Define if include PROGRESSBAR control])
fi

if test "x$build_ctrl_combobox" = "xyes"; then
    AC_DEFINE(_MGCTRL_COMBOBOX, 1,
            [Define if include COMBOBOX control])
fi

if test "x$build_ctrl_propsheet" = "xyes"; then
    AC_DEFINE(_MGCTRL_PROPSHEET, 1,
            [Define if include PROPSHEET control])
fi

if test "x$build_ctrl_trackbar" = "xyes"; then
    AC_DEFINE(_MGCTRL_TRACKBAR, 1,
            [Define if include TRACKBAR control])
fi

if test "x$build_ctrl_scrollbar" = "xyes"; then
    AC_DEFINE(_MGCTRL_SCROLLBAR, 1,
            [Define if include SCROLLBAR control])
fi

if test "x$build_ctrl_newtoolbar" = "xyes"; then
    AC_DEFINE(_MGCTRL_NEWTOOLBAR, 1,
            [Define if include NEWTOOLBAR control])
fi

if test "x$build_ctrl_menubutton" = "xyes"; then
    AC_DEFINE(_MGCTRL_MENUBUTTON, 1,
            [Define if include MENUBUTTON control])
fi

if test "x$build_ctrl_scrollview" = "xyes"; then
    AC_DEFINE(_MGCTRL_SCROLLVIEW, 1,
            [Define if include SCROLLVIEW control])
fi

if test "x$build_ctrl_textedit" = "xyes"; then
    AC_DEFINE(_MGCTRL_TEXTEDIT, 1,
            [Define if include TEXTEDIT control])
fi

if test "x$build_ctrl_monthcal" = "xyes"; then
    AC_DEFINE(_MGCTRL_MONTHCAL, 1,
            [Define if include MONTHCALENDAR control])
fi

if test "x$build_ctrl_treeview" = "xyes"; then
    AC_DEFINE(_MGCTRL_TREEVIEW, 1,
            [Define if include TREEVIEW control])
fi

if test "x$build_ctrl_treeview_rdr" = "xyes"; then
    AC_DEFINE(_MGCTRL_TREEVIEW_RDR, 1,
            [Define if include TREEVIEWRDR control])
fi

if test "x$build_ctrl_spinbox" = "xyes"; then
    AC_DEFINE(_MGCTRL_SPINBOX, 1,
            [Define if include SPINBOX control])
fi

if test "x$build_ctrl_coolbar" = "xyes"; then
    AC_DEFINE(_MGCTRL_COOLBAR, 1,
            [Define if include COOLBAR control])
fi

if test "x$build_ctrl_listview" = "xyes"; then
    AC_DEFINE(_MGCTRL_LISTVIEW, 1,
            [Define if include LISTVIEW control])
fi

if test "x$build_ctrl_gridview" = "xyes"; then
    AC_DEFINE(_MGCTRL_GRIDVIEW, 1,
            [Define if include GRIDVIEW control])
fi

if test "x$build_ctrl_iconview" = "xyes"; then
    AC_DEFINE(_MGCTRL_ICONVIEW, 1,
            [Define if include ICONVIEW control])
fi

if test "x$build_ctrl_animation" = "xyes"; then
    AC_DEFINE(_MGCTRL_ANIMATION, 1,
            [Define if include ANIMATION control])
fi

if test "x$build_ctrl_textedit_new" = "xyes"; then
    AC_DEFINE(_MGCTRL_TEXTEDIT, 1,
            [Define if include TEXTEDIT control])
	AC_DEFINE(_MGCTRL_TEXTEDIT_USE_NEW_IMPL, 1,
			[Define if use new implementation of TEXTEDIT control])
fi

if test "x$build_splash" = "xyes"; then
    CFLAGS="$CFLAGS -D_MG_ENABLE_SPLASH=1"
fi

if test "x$build_screensaver" = "xyes"; then
    CFLAGS="$CFLAGS -D_MG_ENABLE_SCREENSAVER=1"
fi

if test "x$build_minimalgdi" = "xyes"; then
    AC_DEFINE(_MG_MINIMALGDI, 1,
            [Define if build the mgeff support version])
fi

if test "x$build_productid" = "xyes"; then
    AC_DEFINE(_MG_PRODUCTID, 1,
            [Define if insert a productid into the library file])
fi

dnl ========================================================================
dnl Write output

dnl define _GNU_SOURCE for pthread_rwlock_t
CFLAGS="$CFLAGS -D_GNU_SOURCE"

if test "x$devel_mode" = "xyes"; then
    CPPFLAGS="$CPPFLAGS -D_DEBUG"
    if test "x$detail_debug" = "xyes"; then
        CPPFLAGS="$CPPFLAGS -DDEBUG"
    fi

    if test "$ac_cv_prog_gcc" = "yes"; then
        CPPFLAGS="$CPPFLAGS -Wall -Werror"
    fi
else
    CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -O2 -Wstrict-prototypes -pipe"
fi

if test "x$runtime_mode" = "xths"; then
    DEP_LIBS="$DEP_LIBS -lpthread"
fi

AC_SUBST(VIDEO_SUBDIRS)
AC_SUBST(VIDEO_DRIVERS)
AC_SUBST(DEP_LIBS)
AC_SUBST(MINIGUI_RUNMODE)

AM_CONDITIONAL(MGRM_PROCESSES, test "x$runtime_mode" = "xprocs")
AM_CONDITIONAL(MGRM_STANDALONE, test "x$runtime_mode" = "xsa")
AM_CONDITIONAL(MGIAL_CONSOLE, test "x$build_console_ial_engine" = "xyes")
AM_CONDITIONAL(MGIAL_NEXUS, test "x$build_nexus_ial_engine" = "xyes")
AM_CONDITIONAL(MGIAL_DLCUSTOM, test "x$build_dlcustom_ial_engine" = "xyes")
AM_CONDITIONAL(UCOSII, test "x$with_osname" = "xucos2")
AM_CONDITIONAL(MGUSE_OWN_PTHREAD, test "x$use_own_pthread" = "xyes")
AM_CONDITIONAL(MGFONT_SEF, test "x$build_sef_support" = "xyes")

AM_CONDITIONAL(MGIAL_CUSTOM, test "x$build_custom_ial_engine" = "xyes")
AM_CONDITIONAL(MGIAL_NET, test "x$build_net_ial_engine" = "xyes")
AM_CONDITIONAL(MGLIB_MVFB, test "x$with_libsuffix" = "xmvfb")
AM_CONDITIONAL(MG_MINIMALGDI, test "x$build_minimalgdi" = "xyes")
AM_CONDITIONAL(MG_ENABLE_SPLASH, test "x$build_splash" = "xyes")
AM_CONDITIONAL(MG_ENABLE_SCREENSAVER, test "x$build_screensaver" = "xyes")

AC_OUTPUT(
minigui.pc
Makefile
cmake/Makefile
src/Makefile
src/control/Makefile
src/ex_ctrl/Makefile
src/font/Makefile
src/font/in-core/Makefile
src/newgal/Makefile
src/newgal/fbcon/Makefile
src/newgal/qvfb/Makefile
src/newgal/pcxvfb/Makefile
src/newgal/wvfb/Makefile
src/newgal/commlcd/Makefile
src/newgal/shadow/Makefile
src/newgal/mlshadow/Makefile
src/newgal/em86gfx/Makefile
src/newgal/em85xxyuv/Makefile
src/newgal/em85xxosd/Makefile
src/newgal/dummy/Makefile
src/newgal/svpxxosd/Makefile
src/newgal/bf533/Makefile
src/newgal/mb93493/Makefile
src/newgal/utpmc/Makefile
src/newgal/dfb/Makefile
src/newgal/hisi/Makefile
src/newgal/rtos_xvfb/Makefile
src/newgal/nexus/Makefile
src/newgal/s3c6410/Makefile
src/newgal/sigma8654/Makefile
src/newgal/mstar/Makefile
src/newgal/gdl/Makefile
src/newgal/stgfb/Makefile
src/ial/Makefile
src/ial/native/Makefile
src/ial/netial/Makefile
src/ial/nexusial/Makefile
src/ial/dlcustom/Makefile
src/mybmp/Makefile
src/newgdi/Makefile
src/gui/Makefile
src/libc/Makefile
src/misc/Makefile
src/main/Makefile
src/kernel/Makefile
src/server/Makefile
src/client/Makefile
src/standalone/Makefile
src/sysres/Makefile
src/sysres/bmp/Makefile
src/sysres/icon/Makefile
src/sysres/font/Makefile
src/sysres/cursor/Makefile
src/sysres/license/Makefile
src/sysres/license/c_files/Makefile
src/include/Makefile
src/textedit/Makefile
include/Makefile
include/ctrl/Makefile
rtos/Makefile
etc/Makefile
3rd-party/Makefile
3rd-party/scripteasy/Makefile
m4/Makefile
build/Makefile
)

dnl configs/Makefile
dnl scripts/Makefile
dnl scripts/lxdialog/Makefile
dnl build/Makefile

dnl generate license pictures
dnl echo
dnl echo "Going to generate license pictures ..."
dnl cd src/sysres/license/
dnl ./runme.sh $LICENSE_PICTURE > /dev/null
dnl if [[ $? -ne 0 ]]; then
dnl    echo "WARNING: failed to generate license pictures."
dnl else
dnl    echo "Done."
dnl fi
