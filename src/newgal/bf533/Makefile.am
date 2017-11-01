## Makefile.am for the bf533 video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/newgal/

SRC_FILES =             \
    bf533video.c

HDR_FILES = \
    bf533video.h

noinst_LTLIBRARIES = libvideo_bf533.la
libvideo_bf533_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng

LIB_NAME=libvideo_bf533

