
## Makefile.am for the null video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_null.la

# The SDL null video driver sources
SRC_FILES =             \
    nullvideo.c

HDR_FILES= \
    nullvideo.h

libvideo_null_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_null
