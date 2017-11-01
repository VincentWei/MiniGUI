## Makefile.am for the video driver on DirectFB

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/


SRC_FILES = dfbvideo.c st7167_video.c

HDR_FILES= dfbvideo.h st7167_video.h

noinst_LTLIBRARIES = libvideo_dfb.la

libvideo_dfb_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng

LIB_NAME=libvideo_dfb
