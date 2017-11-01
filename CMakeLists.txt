cmake_minimum_required (VERSION 2.6)

project (MiniGUI)

# use for MiniGUI common.h to find mgconfig.h
add_definitions (-D__CMAKE_PROJECT__)
# define if this is a MiniGUI project
add_definitions (-D__MINIGUI_LIB__)
# define _GNU_SOURCE for pthread_rwlock_t
add_definitions (-D_GNU_SOURCE)

# the version number.
set (MINIGUI_MAJOR_VERSION 3)
set (MINIGUI_MINOR_VERSION 0)
set (MINIGUI_MICRO_VERSION 13)

set (PROJECT_NICKNAME "minigui")
set (MGLIB_SUFFIX "")
set (MINIGUI_VERSION ${MINIGUI_MAJOR_VERSION}.${MINIGUI_MINOR_VERSION}.${MINIGUI_MICRO_VERSION})
set (MINIGUI_SOVERSION ${MINIGUI_MAJOR_VERSION}) 
set (MINIGUI_INCLUDE_LIST "")
set (MINIGUI_LIBRARIES_LIST "")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# include cmake file
include (common)
include (minigui-private)

############## check the libraries ################
__find_threadslib (MINIGUI_LIBRARIES_LIST MINIGUI_INCLUDE_LIST)
__find_mlib (have_libm MINIGUI_LIBRARIES_LIST MINIGUI_INCLUDE_LIST)
############## check static lib ################
mg_declare_option_and_set_value (_SHAREDLIB_OUTPUT output_sharedlib BOOL ON "whether output the shared lib")
if (_SHAREDLIB_OUTPUT)
    set (output_lib_type " Dynamic")
endif (_SHAREDLIB_OUTPUT)
mg_declare_option_and_set_value (_STATICLIB_OUTPUT output_staticlib BOOL OFF "whether output the static lib")
if (_STATICLIB_OUTPUT)
    set (output_lib_type "${output_lib_type} Static")
endif(_STATICLIB_OUTPUT)

############## BASE ################
mg_set_value (_HAVE_MATH_LIB ON IF WIN32 OR have_libm)
mg_set_value (_MGTIMER_UNIT_10MS ON)

############ LICENSE ###############
__declare_license_option (_MG_ENABLE_SPLASH        license_splash       BOOL  ON  "enable splash")
__declare_license_option (_MG_ENABLE_SCREENSAVER   license_screensaver  BOOL  ON  "enable screensaver")
mg_declare_option (with_licensepicture STRING "minigui"
    "with license picture: minigui/mdolphin/hybridos")
mg_declare_option_and_set_value (_MG_PRODUCTID            license_productid    BOOL  OFF "insert a productid into the library file")

if (with_licensepicture STREQUAL "minigui")
    mg_set_value (LICENSE_PICTURE "minigui")
elseif (with_licensepicture STREQUAL "mdolphin")
    mg_set_value (LICENSE_PICTURE "mdolphin")
elseif (with_licensepicture STREQUAL "hybridos")
    mg_set_value (LICENSE_PICTURE "hybridos")
else ()
    message (FATAL "There is no license picture named ${with_licensepicture}")
endif (with_licensepicture STREQUAL "minigui")
############## GAL ################
# check fbcon
set (code_check_fbcon "
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
int main () {
    return 0;
}
")
mg_declare_option_and_set_value_with_compile ("${code_check_fbcon}" _MGGAL_FBCON gal_fbcon BOOL  ON   "include FrameBuffer console NEWGAL engine")

# check directfb
set (code_check_directfb "
#include <directfb.h>
int main () {
    return 0;
}
")
mg_declare_option_and_set_value_with_compile ("${code_check_directfb}" _MGGAL_DFB gal_dfb   BOOL   OFF   "include NEWGAL engine for DirectFB")

mg_declare_option_and_set_value (_MGGAL_DUMMY     gal_dummy     BOOL   ON   "include dummy NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_QVFB      gal_qvfb      BOOL   OFF  "include Qt Virtual FrameBuffer NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_WVFB      gal_wvfb      BOOL   OFF  "include windows Virtual FrameBuffer NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_COMMLCD   gal_commlcd   BOOL   OFF  "include NEWGAL engine for Common LCD")
mg_declare_option_and_set_value (_MGGAL_SHADOW    gal_shadow    BOOL   OFF  "include Shadow NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_MLSHADOW  gal_mlshadow  BOOL   OFF  "include MLShadow NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_EM85XXOSD gal_em85xxosd BOOL   OFF  "include NEWGAL engine for EM85xx OSD")
mg_declare_option_and_set_value (_MGGAL_EM85XXYUV gal_em85xxyuv BOOL   OFF  "include NEWGAL engine for EM85xx YUV")
mg_declare_option_and_set_value (_MGGAL_EM86GFX   gal_em86gfx   BOOL   OFF  "include NEWGAL engine for EM86xx GFX")
mg_declare_option_and_set_value (_MGGAL_SVPXXOSD  gal_svpxxosd  BOOL   OFF  "include SVPXXOSD NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_BF533     gal_bf533     BOOL   OFF  "include NEWGAL engine for BF533 OSD via SPI")
mg_declare_option_and_set_value (_MGGAL_MB93493   gal_mb93493   BOOL   OFF  "include NEWGAL engine for mb93493 YUV FrameBuffer driver")
mg_declare_option_and_set_value (_MGGAL_UTPMC     gal_utpmc     BOOL   OFF  "include NEWGAL engine for UTPMC")
mg_declare_option_and_set_value (_MGGAL_NEXUS     gal_nexus     BOOL   OFF  "include nexus NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_S3C6410   gal_s3c6410   BOOL   OFF  "include s3c6410 NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_CUSTOMGAL gal_custom    BOOL   OFF  "include custom NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_SIGMA8654 gal_sigma8654 BOOL   OFF  "include sigma8654 NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_MSTAR     gal_mstar     BOOL   OFF  "include mstar NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_HI3560A   gal_hi3560a   BOOL   OFF  "include hi3560a NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_GDL       gal_gdl       BOOL   OFF  "include gdl NEWGAL engine")
mg_declare_option_and_set_value (_MGGAL_ST7167    gal_ST7167    BOOL   OFF  "include NEWGAL engine for st7167")
mg_declare_option_and_set_value (_MGGAL_STGFB     gal_STGFB     BOOL   OFF  "include stgfb NEWGAL engine ST7105 ST7109")

if (_MGGAL_STGFB)
    add_definitions (-D_ST_OSLINUX)
    add_definitions (-D_ST_7105)
    add_definitions (-DARCHITECTURE_ST40)
    add_definitions (-DDEFINED_BOOL)
endif (_MGGAL_STGFB)

# if rtosxvfb enable, pcxvfb must disable
mg_declare_option (gal_rtosxvfb BOOL   OFF   "include RTOS Virtual FrameBuffer NEWGAL engine. Note: you must disable pcxvfb, if you want to enable rtosxvfb")
mg_declare_option (gal_pcxvfb   BOOL   ON   "include PC Virtual FrameBuffer NEWGAL engine, such as qvfb, mvfb, gvfb or wvfb")
mg_set_value (_MGGAL_PCXVFB gal_pcxvfb IF NOT gal_rtosxvfb)
mg_set_value (_MGGAL_RTOSXVFB ON IF gal_rtosxvfb)

# define _MGGAL_HI3510 and _MGGAL_HI3560 at the same time
mg_declare_option (gal_hi35xx BOOL OFF "include Hi35xx Video  NEWGAL engine")
mg_set_value (_MGGAL_HI3510     gal_hi35xx)
mg_set_value (_MGGAL_HI3560     gal_hi35xx)

if (gal_shadow OR gal_mlshadow)
    set (MINIGUI_LIBRARIES_LIST "${MINIGUI_LIBRARIES_LIST} -lpthread")
endif (gal_shadow OR gal_mlshadow)

############## IAL ################
mg_declare_option_and_set_value (_MGIAL_CUSTOM          ial_custom           BOOL   OFF   "build the IAL engine for customer's board")
mg_declare_option_and_set_value (_MGIAL_NET             ial_net              BOOL   OFF   "build the IAL engine for net")
mg_declare_option_and_set_value (_MGIAL_CISCO_TOUCHPAD  ial_ciscotouchpad    BOOL   OFF   "build the IAL engine for Cisco Touchpad")
mg_declare_option_and_set_value (_MGIAL_MSTAR           ial_mstar            BOOL   OFF   "build the IAL engine for MStar")
mg_declare_option_and_set_value (_MGIAL_IPAQ_H3600      ial_ipaqh3600        BOOL   OFF   "build the IAL engine for iPAQ H3600")
mg_declare_option_and_set_value (_MGIAL_NEXUS           ial_nexus            BOOL   OFF   "build the IAL engine for Nexus")
mg_declare_option_and_set_value (_MGIAL_IPAQ_H5400      ial_ipaqh5400        BOOL   OFF   "build the IAL engine for iPAQ H5400")
mg_declare_option_and_set_value (_MGIAL_TSLIB           ial_tslib            BOOL   OFF   "build the IAL engine for TSLIB")
mg_declare_option_and_set_value (_MGIAL_DUMMY           ial_dummy            BOOL   ON    "build the Dummy IAL engine")
mg_declare_option_and_set_value (_MGIAL_AUTO            ial_auto             BOOL   OFF   "build the Automatic IAL engine")
mg_declare_option_and_set_value (_MGIAL_RANDOM          ial_random           BOOL   OFF   "build the Random IAL engine")
mg_declare_option_and_set_value (_MGIAL_COMM            ial_comm             BOOL   OFF   "build the COMM IAL engine")
mg_declare_option_and_set_value (_MGIAL_QVFB            ial_qvfb             BOOL   OFF   "build the QVFB IAL engine")
mg_declare_option_and_set_value (_MGIAL_QEMU            ial_qemu             BOOL   OFF   "build the QEMU IAL engine")
mg_declare_option_and_set_value (_MGIAL_WVFB            ial_wvfb             BOOL   OFF   "build the WVFB IAL engine")
mg_declare_option_and_set_value (_MGIAL_JZ4740          ial_jz4740           BOOL   OFF   "build the JZ4740 IAL engine")
mg_declare_option_and_set_value (_MGIAL_LIDE            ial_lide             BOOL   OFF   "build the lide IAL engine")
mg_declare_option_and_set_value (_MGIAL_2440            ial_2440             BOOL   OFF   "build the 2440 IAL engine")
mg_declare_option_and_set_value (_MGIAL_DAVINCI6446     ial_davinci6446      BOOL   OFF   "build the DAVINCI6446 IAL engine")
mg_declare_option_and_set_value (_MGIAL_DFB             ial_dfb              BOOL   OFF   "build the DFB IAL engine")
if (WIN32)
    mg_declare_option_and_set_value (_MGIAL_CONSOLE         ial_console      BOOL   OFF   "build the console (Linux console) IAL engine")
    mg_declare_option_and_set_value (_MGIAL_DLCUSTOM        ial_dlcustom     BOOL   OFF   "build the dlcustom IAL engine")
else ()
    mg_declare_option_and_set_value (_MGIAL_CONSOLE         ial_console      BOOL   ON    "build the console (Linux console) IAL engine")
    mg_declare_option_and_set_value (_MGIAL_DLCUSTOM        ial_dlcustom     BOOL   ON    "build the dlcustom IAL engine")
endif (WIN32)
############## IAL CONSOLE ################
mg_declare_option_and_set_value (_MGCONSOLE_PS2       ial_consoleps2      BOOL   ON   "build the console engine subdriver for PS2 mouse")
mg_declare_option_and_set_value (_MGCONSOLE_IMPS2     ial_consoleimps2    BOOL   ON   "build the console engine subdriver for IntelligentMouse (IMPS/2) mouse")
mg_declare_option_and_set_value (_MGCONSOLE_MS        ial_consolems       BOOL   ON   "build the console engine subdirver for old MS serial mouse")
mg_declare_option_and_set_value (_MGCONSOLE_MS3       ial_consolems3      BOOL   ON   "build the console engine subdirver for MS3 mouse")
mg_declare_option_and_set_value (_MGCONSOLE_GPM       ial_consolegpm      BOOL   OFF  "build the console engine subdirver for GPM daemon")
mg_declare_option_and_set_value (_MGCONSOLE_TEXTMODE  ial_consoletextmode BOOL   ON   "Linux system have console (text mode) on FrameBuffer")

############## FONT ################
mg_declare_option_and_set_value (_MGFONT_BMPF     font_bmpfsupport     BOOL   ON   "build support for Bitmap Font (bmpf)")
mg_declare_option_and_set_value (_MGFONT_UPF      font_upfsupport      BOOL   ON   "build support for FMSOft's Unicode Prerendered Font (UPF)")
mg_declare_option_and_set_value (_MGFONT_QPF      font_qpfsupport      BOOL   OFF  "build support for Qt Prerendered Font (QPF)")
mg_declare_option_and_set_value (_MGFONT_SEF      font_sefsupport      BOOL   OFF  "build support for koxomo scripteasy (SEF)")
mg_declare_option_and_set_value (_MGFONT_RBF      font_rbfsupport      BOOL   ON   "include raw bitmap font support")
mg_declare_option_and_set_value (_MGFONT_VBF      font_vbfsupport      BOOL   ON   "include var bitmap font support")

mg_declare_option (font_unicodesupport    BOOL    ON      "include UNICODE (ISO-10646-1 and UTF-8 encoding) support")
mg_declare_option (with_fontttfsupport    STRING  "none"  "with_fontttfsupport=ft1/ft2/none")
mg_declare_option (with_ft2includes       PATH    ""      "where the FreeType2 includes are")
mg_declare_option (font_ttfenablecache    BOOL    OFF     "include ttf cache support")
mg_declare_option (font_ttfcachesize      STRING  "256"   "font_ttfcachesize=64/128/256/512/1024")
mg_declare_option (font_ttfcachenum       STRING  "10"    "font_ttfcachenum=10/20/40")
mg_declare_option (font_ttfgrayadjust     BOOL    OFF      "adjust ttf font bitmap gray_value(range(0~255))")
mg_declare_option (font_ttfgrayadjust_value STRING  "50"   "font_ttfcachesize range(0~255)")

__find_freetype (with_fontttfsupport with_ft1includes with_ft2includes MINIGUI_LIBRARIES_LIST MINIGUI_INCLUDE_LIST)

# mg_set_value (/* Define what to do */ IF with_fontttfsupport STREQUAL "none")
mg_set_value (_MGFONT_TTF ON IF with_fontttfsupport STREQUAL "ft1")
mg_set_value (_MGFONT_FT2 ON IF with_fontttfsupport STREQUAL "ft2")

if (font_ttfgrayadjust)
    mg_set_value (_MGFONT_TTF_GRAYADJUST ON)
    set (_MGFONT_TTF_GRAYADJUST_VALUE ${font_ttfgrayadjust_value})
    message (FATAL ${font_ttfgrayadjust_value})
endif (font_ttfgrayadjust)

if (with_fontttfsupport STREQUAL "none")
else ()
    # enable font_unicodesupport
    mg_set_variables_on (font_unicodesupport)
    if (font_ttfenablecache)
        mg_set_value (_MGFONT_TTF_CACHE ON)
        mg_set_number_select_value (_MGTTF_CACHE_SIZE font_ttfcachesize 64 128 256 512 1024)
        mg_set_number_select_value (_MGMAX_TTF_CACHE font_ttfcachenum 10 20 40)
    endif (font_ttfenablecache)
endif (with_fontttfsupport STREQUAL "none")
if (font_bmpfsupport OR font_upfsupport OR font_sefsupport)
    mg_set_variables_on (font_unicodesupport)
endif (font_bmpfsupport OR font_upfsupport OR font_sefsupport)

############## INCOREFONT ################
if (font_rbfsupport)
    mg_declare_option_and_set_value (_MGINCORERBF_LATIN1_VGAOEM   incorefont_rbfvgaoem        BOOL   ON   "include incore RBF font of ISO8859-1 VGAOEM (8x8) font")
    mg_declare_option_and_set_value (_MGINCORERBF_LATIN1_TERMINAL incorefont_rbfterminal      BOOL   ON   "include incore RBF font of ISO8859-1 Terminal (8x12) font")
    mg_declare_option_and_set_value (_MGINCORERBF_LATIN1_FIXEDSYS incorefont_rbffixedsys      BOOL   ON   "include incore RBF font of ISO8859-1 FixedSys (8x15) font")
endif (font_rbfsupport)
if (font_vbfsupport)
    mg_declare_option_and_set_value (_MGINCOREFONT_SANSSERIF      incorefont_sserif      BOOL   ON   "include incore font SansSerif (11x13)")
    mg_declare_option_and_set_value (_MGINCOREFONT_COURIER        incorefont_courier     BOOL   ON   "include incore font Courier (8x13)")
    mg_declare_option_and_set_value (_MGINCOREFONT_SYSTEM         incorefont_system      BOOL   ON   "include incore font System (14x16)")
endif (font_vbfsupport)
if (font_upfsupport)
    mg_declare_option_and_set_value (_MGINCOREFONT_TIMES          incorefont_times       BOOL   ON   "include incore Times UPF fonts (12x10 and 17x14)")
endif (font_upfsupport)

############## CHARSET ################
mg_declare_option_and_set_value (_MGCHARSET_LATIN2    charset_latin2support       BOOL   ON   "include East European (Latin 2 ISO8859-2) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN3    charset_latin3support       BOOL   ON   "include South European (Latin 3 ISO8859-3) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN4    charset_latin4support       BOOL   ON   "include North European (Latin 4 ISO8859-4) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN5    charset_latin5support       BOOL   ON   "include Turkish (Latin 5 ISO8859-9) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN6    charset_latin6support       BOOL   ON   "include Nordic Latin 6 ISO8859-10) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN7    charset_latin7support       BOOL   ON   "include Latin 7 (ISO8859-13) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN8    charset_latin8support       BOOL   ON   "include Latin 8 (ISO8859-14) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN9    charset_latin9support       BOOL   ON   "include Latin 9 (ISO8859-15 West Extended) charset support")
mg_declare_option_and_set_value (_MGCHARSET_LATIN10   charset_latin10support      BOOL   ON   "include Latin 10 (ISO8859-16 Romanian) charset support")
mg_declare_option_and_set_value (_MGCHARSET_CYRILLIC  charset_cyrillicsupport     BOOL   ON   "include Cyrillic (ISO8859-5) charset support")
mg_declare_option_and_set_value (_MGCHARSET_ARABIC    charset_arabicsupport       BOOL   ON   "include Arabic (ISO8859-6) charset support")
mg_declare_option_and_set_value (_MGCHARSET_GREEK     charset_greeksupport        BOOL   ON   "include Greek (ISO8859-7) charset support")
mg_declare_option_and_set_value (_MGCHARSET_HEBREW    charset_hebrewsupport       BOOL   ON   "include Hebrew (ISO8859-8) charset support")
mg_declare_option_and_set_value (_MGCHARSET_THAI      charset_thaisupport         BOOL   ON   "include Thai (ISO8859-11) charset support")
mg_declare_option_and_set_value (_MGCHARSET_GB        charset_gbsupport           BOOL   ON   "include EUC encoding of GB2312 charset support")
mg_declare_option_and_set_value (_MGCHARSET_GBK       charset_gbksupport          BOOL   ON   "include GBK charset support")
mg_declare_option_and_set_value (_MGCHARSET_GB18030   charset_gb18030support      BOOL   OFF  "include GB18030-0 charset support")
mg_declare_option_and_set_value (_MGCHARSET_BIG5      charset_big5support         BOOL   ON   "include BIG5 charset support")
mg_declare_option_and_set_value (_MGCHARSET_EUCKR     charset_euckrsupport        BOOL   ON   "include support for EUC encoding of KSC5636 and KSC5601 charsets")
mg_declare_option_and_set_value (_MGCHARSET_EUCJP     charset_eucjpsupport        BOOL   ON   "include support for EUC encoding of JISX0201 and JISX0208 charsets")
mg_declare_option_and_set_value (_MGCHARSET_SHIFTJIS  charset_shiftjissupport     BOOL   ON   "include support for Shift-JIS encoding of JISX0201 and JISX0208 charsets")
mg_set_value (_MGCHARSET_UNICODE font_unicodesupport)

############## CTRL ################
mg_declare_option_and_set_value (_MGCTRL_STATIC       ctrl_static         BOOL   ON   "include STATIC control")
mg_declare_option_and_set_value (_MGCTRL_BUTTON       ctrl_button         BOOL   ON   "include BUTTON control")
mg_declare_option_and_set_value (_MGCTRL_SLEDIT       ctrl_sledit         BOOL   ON   "include Single-Line EDIT control")
mg_declare_option_and_set_value (_MGCTRL_BIDISLEDIT   ctrl_bidisledit     BOOL   ON   "include Single-Line BIDI EDIT control")
mg_declare_option_and_set_value (_MGCTRL_LISTBOX      ctrl_listbox        BOOL   ON   "include LISTBOX control")
mg_declare_option_and_set_value (_MGCTRL_PROGRESSBAR  ctrl_progressbar    BOOL   ON   "include PROGRESSBAR control")
mg_declare_option_and_set_value (_MGCTRL_NEWTOOLBAR   ctrl_newtoolbar     BOOL   ON   "include NEWTOOLBAR control")
mg_declare_option_and_set_value (_MGCTRL_TRACKBAR     ctrl_trackbar       BOOL   ON   "include TRACKBAR control")
mg_declare_option_and_set_value (_MGCTRL_COMBOBOX     ctrl_combobox       BOOL   ON   "include COMBOBOX control")
mg_declare_option_and_set_value (_MGCTRL_PROPSHEET    ctrl_propsheet      BOOL   ON   "include PROPSHEET control")
mg_declare_option_and_set_value (_MGCTRL_SCROLLVIEW   ctrl_scrollview     BOOL   ON   "include SCROLLVIEW and SCROLLWINDOW controls")
mg_declare_option_and_set_value (_MGCTRL_MONTHCAL     ctrl_monthcal       BOOL   ON   "include MONTHCALENDAR control")
mg_declare_option_and_set_value (_MGCTRL_TREEVIEW     ctrl_treeview       BOOL   ON   "include TREEVIEW control")
mg_declare_option_and_set_value (_MGCTRL_TREEVIEW_RDR ctrl_treeviewrdr    BOOL   OFF  "include TREEVIEWRDR control using LFRDR")
mg_declare_option_and_set_value (_MGCTRL_SPINBOX      ctrl_spinbox        BOOL   ON   "include SPINBOX control")
mg_declare_option_and_set_value (_MGCTRL_COOLBAR      ctrl_coolbar        BOOL   ON   "include COOLBAR control")
mg_declare_option_and_set_value (_MGCTRL_LISTVIEW     ctrl_listview       BOOL   ON   "include LISTVIEW control")
mg_declare_option_and_set_value (_MGCTRL_ICONVIEW     ctrl_iconview       BOOL   ON   "include ICONVIEW control")
mg_declare_option_and_set_value (_MGCTRL_GRIDVIEW     ctrl_gridview       BOOL   OFF  "include GRIDVIEW control(test)")
mg_declare_option_and_set_value (_MGCTRL_ANIMATION    ctrl_animation      BOOL   ON   "include ANIMATION control and GIF87a/GIF89a support")
mg_declare_option_and_set_value (_MGCTRL_SCROLLBAR    ctrl_scrollbar      BOOL   ON   "include SCROLLBAR control and GIF87a/GIF89a support")
mg_declare_option_and_set_value (_MGCTRL_TEXTEDIT_USE_NEW_IMPL     ctrl_newtextedit    BOOL   ON   "include the new implementation of TEXTEDIT control")
mg_declare_option (ctrl_textedit       BOOL   OFF   "include old TEXTEDIT control implementation")
if (ctrl_newtextedit)
    mg_set_variables_on (ctrl_textedit)
endif (ctrl_newtextedit)
mg_set_value (_MGCTRL_TEXTEDIT ctrl_textedit)

############## MISC ################
mg_declare_option_and_set_value (_DEBUG               misc_debug         BOOL  OFF     "build with debugging messages")
mg_declare_option_and_set_value (_USE_MINIGUIENTRY    misc_miniguientry  BOOL  OFF     "use minigui_entry function in MiniGUI")
mg_declare_option (misc_incoreres  BOOL   OFF   "use incore resource instead file IO to initialize MiniGUI")
mg_declare_option_and_set_value (_MGMISC_DOUBLE_CLICK     misc_dblclk         BOOL   ON   "mouse button can do double click")
mg_declare_option_and_set_value (_MGMISC_SAVEBITMAP       misc_savebitmap     BOOL   ON   "include SaveBitmap-related functions")
mg_declare_option (misc_aboutdlg    BOOL   ON   "include About Dialog Box")
mg_set_value (_MGMISC_ABOUTDLG misc_aboutdlg IF ctrl_button)
mg_declare_option (misc_savescreen  BOOL   ON   "include code for screenshots")
mg_set_value (_MGMISC_SAVESCREEN misc_savescreen IF misc_savebitmap)
############### MISC_USEOWN ###################
mg_declare_option (misc_useownmalloc   BOOL   OFF    "use own implementation of malloc functions")
mg_declare_option (misc_useownstdio    BOOL   OFF    "use own implementation of stdio functions")
mg_declare_option (misc_useownpthread  BOOL   OFF    "use own implementation of pthread functions")
################## MISC HAVE ######################
mg_declare_option_and_set_value (_MGHAVE_MENU             misc_havemenu            BOOL   ON   "include menu support")
mg_declare_option_and_set_value (_MGHAVE_CLIPBOARD        misc_haveclipboard       BOOL   ON   "include clipboard support")
mg_declare_option_and_set_value (_MGHAVE_CURSOR           misc_havecursor          BOOL   ON   "include cursor support")
mg_declare_option_and_set_value (_MGHAVE_FIXED_MATH       misc_havefixedmath       BOOL   ON   "include fixed math routines")
mg_declare_option_and_set_value (_MGHAVE_MOUSECALIBRATE   misc_havemousecalibrate  BOOL   ON   "include code doing mouse calibration")
mg_declare_option_and_set_value (_MGHAVE_TRACE_MSG        misc_havetracemsg        BOOL   OFF  "trace messages of MiniGUI")
mg_declare_option (misc_havemsgstr    BOOL   OFF   "include symbol name of message")
mg_set_value (_MGHAVE_MSG_STRING misc_havemsgstr IF misc_havetracemsg)
mg_declare_option (misc_haveadv2dapi  BOOL   ON    "include advanced 2D graphics APIs")
mg_set_value (_MGHAVE_ADV_2DAPI misc_haveadv2dapi IF WIN32 OR have_libm)

############## KEYBORADLAYOUT ################
mg_declare_option_and_set_value (_MGKBDLAYOUT_HEBREWPC    kbd_hebrewpc     BOOL   OFF   "include keyboard layout for Hebrew PC keyboard")
mg_declare_option_and_set_value (_MGKBDLAYOUT_ARABICPC    kbd_arabicpc     BOOL   OFF   "include keyboard layout for Arabic PC keyboard")
mg_declare_option_and_set_value (_MGKBDLAYOUT_FRPC        kbd_frpc         BOOL   OFF   "include keyboard layout for French PC keyboard (non-US 102 keys)")
mg_declare_option_and_set_value (_MGKBDLAYOUT_FR          kbd_fr           BOOL   OFF   "include keyboard layout for French")
mg_declare_option_and_set_value (_MGKBDLAYOUT_DE          kbd_de           BOOL   OFF   "include keyboard layout for German")
mg_declare_option_and_set_value (_MGKBDLAYOUT_DELATIN1    kbd_delatin1     BOOL   OFF   "include keyboard layout for German Latin1")
mg_declare_option_and_set_value (_MGKBDLAYOUT_IT          kbd_it           BOOL   OFF   "include keyboard layout for Italian")
mg_declare_option_and_set_value (_MGKBDLAYOUT_ES          kbd_es           BOOL   OFF   "include keyboard layout for Spanish")
mg_declare_option_and_set_value (_MGKBDLAYOUT_ESCP850     kbd_escp850      BOOL   OFF   "include keyboard layout for Spanish CP850")

############## LF_RDR #######################
mg_declare_option_and_set_value (_MGLF_RDR_FLAT   lfrdr_flat  BOOL   ON   "include flat Look and Feel renderer")
mg_declare_option_and_set_value (_MGLF_RDR_SKIN   lfrdr_skin  BOOL   ON   "include skin Look and Feel renderer")


############## CHECK OS NAME ################
if (NOT WIN32)
    mg_declare_option (with_osname STRING "linux" "select osname: linux/uclinux/ecos/ucos2/swlinux/vxworks/win32/darwin/threadx/cygwin/nucleus/ose/psos")
else ()
    mg_declare_option (with_osname STRING "win32" "select osname: linux/uclinux/ecos/ucos2/swlinux/vxworks/win32/darwin/threadx/cygwin/nucleus/ose/psos")
endif (NOT WIN32)

if (with_osname STREQUAL "uclinux")
    mg_set_value (__uClinux__ ON)
elseif (with_osname STREQUAL "ecos")
    mg_set_value (__ECOS__ ON)
    mg_set_value (__NOUNIX__ ON)
elseif (with_osname STREQUAL "ucos2")
    mg_set_value (__UCOSII__ ON)
    mg_set_value (__NOUNIX__ ON)
    mg_set_variables_on (misc_incoreres misc_useownmalloc misc_useownstdio misc_useownpthread)
elseif (with_osname STREQUAL "swlinux")
    mg_set_value (__WINBOND_SWLINUX__ ON)
    mg_set_value (__NOUNIX__ ON)
elseif (with_osname STREQUAL "vxworks")
    mg_set_value (__VXWORKS__ ON)
    mg_set_value (__NOUNIX__ ON)
elseif (with_osname STREQUAL "win32")
    mg_set_value (WIN32 ON)
    mg_set_value (__NOUNIX__ ON)
    add_definitions (-D__NOUNIX__)
elseif (with_osname STREQUAL "darwin")
    mg_set_value (__DARWIN__ ON)
elseif (with_osname STREQUAL "threadx")
    mg_set_value (__THREADX__ ON)
    mg_set_value (__NOUNIX__ ON)
    mg_set_variables_on (misc_incoreres misc_useownmalloc misc_useownstdio misc_useownpthread)
elseif (with_osname STREQUAL "nucleus")
    mg_set_value (__NUCLEUS__ ON)
    mg_set_value (__NOUNIX__ ON)
    mg_set_variables_on (misc_incoreres misc_useownmalloc misc_useownstdio misc_useownpthread)
elseif (with_osname STREQUAL "ose")
    mg_set_value (__OSE__ ON)
    mg_set_value (__NOUNIX__ ON)
    mg_set_variables_on (misc_incoreres misc_useownpthread)
elseif (with_osname STREQUAL "psos")
    mg_set_value (__PSOS__ ON)
    mg_set_value (__NOUNIX__ ON)
else ()
    mg_set_value (__LINUX__ ON)
    set (with_osname "linux")
endif (with_osname STREQUAL "uclinux")
mg_set_value (_MGUSE_OWN_MALLOC ${misc_useownmalloc})
mg_set_value (_MGUSE_OWN_PTHREAD ${misc_useownpthread})
mg_set_value (_MGUSE_OWN_STDIO ${misc_useownstdio})

############## with target name ################
mg_declare_option (with_targetname STRING "unknown"
    "with targetname: unknown/stb810/vfanvil/vxi386/qvfb/fbcon/mx21/monaco/c33l05/bfin/vxppc/fmsoft/mstudio/S3C6410/S3C2440/S3C2410/hi3560a")
if (with_targetname STREQUAL "stb810")
    mg_set_value (__TARGET_STB810__ ON)
elseif (with_targetname STREQUAL "vfanvil")
    mg_set_value (__TARGET_VFANVIL__ ON)
elseif (with_targetname STREQUAL "monaco")
    mg_set_value (__TARGET_MONACO__ ON)
elseif (with_targetname STREQUAL "mx21")
    mg_set_value (__TARGET_MX21__ ON)
elseif (with_targetname STREQUAL "vxi386")
    mg_set_value (__TARGET_VXI386__ ON)
elseif (with_targetname STREQUAL "vxppc")
    mg_set_value (__TARGET_PPC__ ON)
elseif (with_targetname STREQUAL "bfin")
    mg_set_value (__TARGET_BLACKFIN__ ON)
elseif (with_targetname STREQUAL "c33l05")
    mg_set_value (__TARGET_C33L05__ ON)
elseif (with_targetname STREQUAL "fmsoft")
    mg_set_value (__TARGET_FMSOFT__ ON)
elseif (with_targetname STREQUAL "mstudio")
    mg_set_value (__TARGET_MSTUDIO__ ON)
elseif (with_targetname STREQUAL "S3C6410")
    add_definitions (-D_WITH_TARGET_S3C6410)
elseif (with_targetname STREQUAL "S3C2440")
    add_definitions (-D_WITH_TARGET_S3C2440)
elseif (with_targetname STREQUAL "S3C2410")
    add_definitions (-D_WITH_TARGET_S3C2410)
elseif (with_targetname STREQUAL "hi3560a")
    add_definitions (-D_WITH_TARGET_HI3560A)
else ()
    mg_set_value (__TARGET_UNKNOWN__ ON)
    add_definitions (-D_WITH_TARGET_NONE)
endif (with_targetname STREQUAL "stb810")

################### threads standalone or processes version #########################
mg_declare_option (runmode_standalone BOOL   OFF   "build MiniGUIStandalone version")
mg_declare_option (runmode_processes  BOOL   OFF   "build MiniGUIProcesses version")
if (runmode_standalone)
    mg_set_variables_on (runmode_processes)
endif (runmode_standalone)
if (runmode_processes)
    if (runmode_standalone)
        mg_set_variables_on (MGRM_STANDALONE)
        mg_set_value (_MGRM_STANDALONE ON)
        mg_set_value (_STAND_ALONE ON)
        set (output_lib_mode Standalone)
    else ()
        mg_set_value (_MGRM_PROCESSES ON)
        set (output_lib_mode Processes)
    endif (runmode_standalone)
    mg_set_value (_LITE_VERSION ON)
else ()
    mg_set_value (_MGRM_THREADS ON)
    add_definitions (-D_REENTRANT)
    set (output_lib_mode Threads)
endif (runmode_processes)
mg_declare_option_and_set_value (_MG_MINIMALGDI       runmode_minimalgdi    BOOL  OFF     "build a minimal GDI library only")
if (runmode_minimalgdi)
    set (output_lib_mode "${output_lib_mode} - minimalgdi")
endif (runmode_minimalgdi)

########### ETCFILENAME and LIB_SUFFIX #############
mg_declare_option (with_libsuffix  STRING "" "Configure the suffix of MiniGUI library name.")
if (with_libsuffix STREQUAL "mvfb")
    set (MINIGUI_CFG "mvfb.cfg")
    set (MGLIB_SUFFIX "mvfb")
elseif (with_libsuffix STREQUAL "")
    set (MINIGUI_CFG "MiniGUI.cfg")
    if (runmode_processes)
        if (runmode_standalone)
            set (MGLIB_SUFFIX "sa")
        else ()
            set (MGLIB_SUFFIX "procs")
        endif(runmode_standalone)
    else ()
        set (MGLIB_SUFFIX "ths")
    endif(runmode_processes)
else ()
    set (MINIGUI_CFG "MiniGUI.cfg")
    #TODO: withval=""
    set (MGLIB_SUFFIX "${with_libsuffix}")
endif (with_libsuffix STREQUAL "mvfb")
mg_set_value (_MGLIB_SUFFIX ${MGLIB_SUFFIX})
mg_set_value (_MGINCORE_RES misc_incoreres)
mg_set_value (ETCFILENAME ${MINIGUI_CFG} IF NOT misc_incoreres)
#message(STATUS "the mode is ${_MGLIB_SUFFIX}, MINIGUI_CFG is ${MINIGUI_CFG}")

############## check header files ################
include (CheckIncludeFiles) 
check_include_files(alloca.h HAVE_ALLOCA_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(locale.h HAVE_LOCALE_H)
check_include_files(math.h HAVE_MATH_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(termio.h HAVE_TERMIO_H)
check_include_files(unistd.h HAVE_UNISTD_H)

############## check functions ################
include(CheckFunctionExists)
check_function_exists (alloca HAVE_ALLOCA)
check_function_exists (time HAVE_TIME)
check_function_exists (getpagesize HAVE_GETPAGESIZE)
check_function_exists (mktime HAVE_MKTIME)
check_function_exists (localtime HAVE_LOCALTIME)
check_function_exists (strdup HAVE_STRDUP)
check_function_exists (strcasecmp HAVE_STRCASECMP)
check_function_exists (strncasecmp HAVE_STRNCASECMP)
check_function_exists (strerror HAVE_STRERROR)
check_function_exists (getpt HAVE_GETPT)
check_function_exists (mmap HAVE_MMAP)
check_function_exists (setlocale HAVE_SETLOCALE)
check_function_exists (vprintf HAVE_VPRINTF)

############## IMAGE ################
mg_declare_option_and_set_value (_MGIMAGE_PCX image_pcxsupport   BOOL   OFF   "include PCX file support")
mg_declare_option_and_set_value (_MGIMAGE_LBM image_lbmsupport   BOOL   OFF   "include LBM/PBM file support")
mg_declare_option_and_set_value (_MGIMAGE_TGA image_tgasupport   BOOL   OFF   "include TGA file support")
mg_declare_option_and_set_value (_MGIMAGE_GIF image_gifsupport   BOOL   ON    "include GIF file support")

############## ADD LIBRARIES ################
mg_declare_option (image_pngsupport   BOOL   ON    "include PNG file support")
__find_pnglib (image_pngsupport MINIGUI_LIBRARIES_LIST MINIGUI_INCLUDE_LIST)
mg_set_value (_MGIMAGE_PNG image_pngsupport)

mg_declare_option (image_jpgsupport  BOOL   ON   "include JPEG file suppport")
__find_jpeglib (image_jpgsupport MINIGUI_LIBRARIES_LIST MINIGUI_INCLUDE_LIST)
mg_set_value (_MGIMAGE_JPG image_jpgsupport)

if (NOT WIN32 AND ial_dlcustom)
    set (MINIGUI_LIBRARIES_LIST "${MINIGUI_LIBRARIES_LIST} -ldl")
endif (NOT WIN32 AND ial_dlcustom)

if (gal_fbcon)
    find_library (find_pciaccess pciaccess)
    if (find_pciaccess)
        mg_set_value (_MGHAVE_PCIACCESS ON) #set libpciaccess.so
        set (MINIGUI_LIBRARIES_LIST "${MINIGUI_LIBRARIES_LIST} -lpciaccess")
    endif (find_pciaccess)
endif (gal_fbcon)

################CTRL:need misc condition###################
mg_declare_option (ctrl_menubutton     BOOL   ON    "include MENUBUTTON control")
if (NOT misc_havemenu)
    set (ctrl_menubutton OFF)
endif (NOT misc_havemenu)
mg_set_value (_MGCTRL_MENUBUTTON ctrl_menubutton)

############################ set conditional ################################
mg_set_conditional(MGRM_PROCESSES      runmode_processes ON)
mg_set_conditional(MGRM_STANDALONE     runmode_standalone ON)
mg_set_conditional(MGIAL_CONSOLE       ial_console ON)
mg_set_conditional(MGIAL_NEXUS         ial_nexus ON)
mg_set_conditional(MGIAL_DLCUSTOM      ial_dlcustom ON)
mg_set_conditional(UCOSII              with_osname "ucos2")
mg_set_conditional(MGUSE_OWN_PTHREAD   misc_useownpthread ON)
mg_set_conditional(MGFONT_SEF          font_sefsupport ON)
mg_set_conditional(MGIAL_CUSTOM        ial_custom ON)
mg_set_conditional(MGIAL_NET           ial_net ON)
mg_set_conditional(MGLIB_MVFB          with_libsuffix "mvfb")
mg_set_conditional(MG_MINIMALGDI       runmode_minimalgdi ON)

if (font_sefsupport)
    set (MINIGUI_LIBRARIES_LIST "${MINIGUI_LIBRARIES_LIST} -lscripteasy")
endif (font_sefsupport)
    
# add sub directory
# it should be set after macros setup
add_subdirectory (src)

################################ .pc and mgconfig.h ################################
# generate the .pc file used by pkg-config
set (prefix ${CMAKE_INSTALL_PREFIX})
set (exec_prefix "\${prefix}")
set (libdir "\${exec_prefix}/lib")
set (includedir "\${prefix}/include")
configure_file(
    "${PROJECT_SOURCE_DIR}/minigui.pc.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NICKNAME}.pc")
# configure header file of mgconfig to pass the CMake settings
# to the source code
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake_mgconfig.h.in"
    "${PROJECT_BINARY_DIR}/mgconfig.h"
    )

################################## files to install ####################################
set (etc_files "")
if (MGLIB_MVFB)
    set (etc_files "mvfb.cfg")
else ()
    set (etc_files "${PROJECT_SOURCE_DIR}/etc/${MINIGUI_CFG}")
endif (MGLIB_MVFB)

# etc
IF (WIN32)
    set (RESPATH "$ENV{PREFIX}/share/minigui/res/")
    configure_file(
        "${PROJECT_SOURCE_DIR}/etc/MiniGUI.cfg.win32.in"
        "${PROJECT_BINARY_DIR}/etc/MiniGUI.cfg.win32"
        )
    install (FILES ${PROJECT_BINARY_DIR}/etc/MiniGUI.cfg.win32
        DESTINATION etc
        RENAME MiniGUI.cfg)
ELSE()
    install (FILES "${etc_files}" DESTINATION etc)
ENDIF (WIN32)

# include
install (FILES  "${PROJECT_BINARY_DIR}/mgconfig.h"
    DESTINATION include/${PROJECT_NICKNAME}
)

install (DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include/${PROJECT_NICKNAME}
    FILES_MATCHING PATTERN "*.h"
                   PATTERN "*.c"
                   PATTERN ".svn" EXCLUDE
    )

# pkgconfig
install (FILES "${PROJECT_BINARY_DIR}/${PROJECT_NICKNAME}.pc"
    DESTINATION lib/pkgconfig)

# Cmake Modules
install (DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/"
    DESTINATION share/minigui/cmake/Modules
    FILES_MATCHING PATTERN "*.cmake"
                   PATTERN "*private*" EXCLUDE
                   PATTERN ".svn" EXCLUDE
                   )

##################### make dist package source #########################
set(CPACK_PACKAGE_VERSION_MAJOR ${MINIGUI_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINIGUI_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${MINIGUI_MICRO_VERSION})
set(CPACK_SOURCE_GENERATOR "TBZ2")

set(CPACK_SOURCE_IGNORE_FILES
      "/build/;/configs/;/ext/;/doc/;/examples/;/pe-scripts/;/scripts/;
      /build-adp/;/vcongui/;/mywindows/;/ve-scripts/;/build_dir/;.o$;
      /.bzr/;~$;/.svn/;.swp$;Makefile.am$;makefile*;.edited$;.sh$;.old$;
      configure.in$;config.log$;
      ${CPACK_SOURCE_IGNORE_FILES}")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${PROJECT_NICKNAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CPACK_SOURCE_PACKAGE_FILE_NAME}.${CPACK_PACKAGE_VERSION_PATCH}-${with_osname}")

include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
set_target_properties (dist PROPERTIES OUTPUT_NAME ${CPACK_SOURCE_PACKAGE_FILE_NAME})

##################################cfg info display######################################
message("=============================================")
message("MiniGUI Version :  ${MINIGUI_VERSION}")
message("Run mode        :  ${output_lib_mode}")
message("OS Name         :  ${with_osname}")
message("Target board    :  ${with_targetname}")
message("Library type    :  ${output_lib_type}")
message("=============================================")
message("")

set (module_list "license" "gal" "ial" "lfrdr" "ctrl" "charset" "font" "incorefont" "image" "kbd" "misc")

foreach(i ${module_list})
    mg_sort_option_group (${i})
    message ("*********************************************")
    if (${i} STREQUAL "console")
        message ("* ial ${i}")
    elseif (${i} STREQUAL "license")
        message ("* license-management")
    elseif (${i} STREQUAL "lfrdr")
        message ("* look and feel renderer")
    elseif (${i} STREQUAL "kbd")
        message ("* keyboard-layout")
    elseif (${i} STREQUAL "ctrl")
        message ("* control")
    else ()
        message("* ${i}")
    endif (${i} STREQUAL "console")
    message ("*********************************************")
    message ("* Enabled:")
    mg_print_list (${_group_${i}_ON_})
    message ("* Disabled:")
    mg_print_list (${_group_${i}_OFF_})
endforeach (i ${module_options})

# message ("=============================================")
# message ("= CFLAGS when building MiniGUI")
# message ("=============================================")
# message ("${CMAKE_C_FLAGS}")

# message ("=============================================")
# message ("= CFLAGS & LDFLAGS when building MiniGUI")
# message ("=============================================")
# message ("= LDFLAGS:
# message ("    ${CMAKE_C_FLAGS}")
# message ("-D:")
# message ("-I:${CMAKE_INSTALL_PREFIX}/include")
# message ("-L:${CMAKE_INSTALL_PREFIX}/lib")
# message ("-l:")
# message ("")
# message ("=============================================")
# message ("= CFLAGS & LDFLAGS when building Application")
# message ("=============================================")
# message ("-D:")
# message ("-I:${CMAKE_INSTALL_PREFIX}/include")
# message ("-L:${CMAKE_INSTALL_PREFIX}/lib")
# message ("-l:")
# message ("")
 
if (NOT license_productid)
    message ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    message ("!!")
    message ("!! WARNING")
    message ("!!")
    message ("!!")
    message ("!! Product ID is *DISABLE*")
    message ("!!")
    message ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    message ("")
endif(NOT license_productid)
