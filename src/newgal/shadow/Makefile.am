AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

SRC_FILES = shadow.c shadow_refresh_msb_right.c shadow_refresh_msb_left.c

HDR_FILES = shadow.h

noinst_LTLIBRARIES = libvideo_shadow.la
libvideo_shadow_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_shadow
