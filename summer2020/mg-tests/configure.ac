dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT(mg-tests, 5.0.0)
#AC_CONFIG_SRCDIR(src/helloworld.c)

dnl ========================================================================
dnl Need for cross-compiling
AC_CANONICAL_SYSTEM

dnl ========================================================================
dnl Init automake
AM_INIT_AUTOMAKE

dnl ========================================================================
dnl Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL

dnl ========================================================================
dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl ========================================================================
dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/time.h unistd.h)

PKG_CHECK_MODULES([MINIGUI], [minigui >= 5.0.0])
#PKG_CHECK_MODULES([MGEFF], [mgeff >= 1.5.0])

PKG_CHECK_MODULES(DRM, [libudev >= 200, libdrm >= 2.4], video_drm="yes",
    video_drm="no; the test code requires libdrm 2.4+ and libudev 200+")
if test "x$video_drm" = "xyes"; then
    DRM_INC_DIR="`$PKG_CONFIG --variable includedir libdrm`/libdrm"
    AC_SUBST(DRM_INC_DIR)
else
    AC_MSG_WARN([$video_drm])
fi

PKG_CHECK_MODULES(DRMINTEL, [libdrm_intel >= 2.4], video_drm_intel="yes",
    video_drm_intel="no; the test code requires libdrm_intel 2.4+")
if test "x$video_drm_intel" = "xyes"; then
    CPPFLAGS="$CPPFLAGS -DHAVE_DRM_INTEL"
else
    AC_MSG_WARN([$video_drm_intel])
fi

AC_CHECK_DECLS(_MGRM_PROCESSES, runmode_procs="yes", foo=bar, [#include <minigui/common.h>])

dnl ========================================================================
dnl Write Output

LIBS="$LIBS $MINIGUI_LIBS"

if test "$ac_cv_prog_gcc" = "yes"; then
    CPPFLAGS="$CPPFLAGS -D_DEFAULT_SOURCE -D_GNU_SOURCE"
    CFLAGS="$CFLAGS -std=c99 -Wall -Werror -Wstrict-prototypes -pipe   -lmygpio -lfreertos  -I /usr/local/include "
fi

AM_CONDITIONAL(MGRM_PROCESSES, test "x$runmode_procs" = "xyes")

AC_OUTPUT(
    Makefile
    m4/Makefile
   
    comm-engines/Makefile

)
