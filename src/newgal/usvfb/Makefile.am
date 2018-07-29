AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

SRC_FILES = usvfb.c 
HDR_FILES = usvfb.h

noinst_LTLIBRARIES = libvideo_usvfb.la
libvideo_usvfb_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_usvfb
