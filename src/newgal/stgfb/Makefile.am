
## Makefile.am for the video driver on ST FrameBuffer

TOP_DIR=../../..
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/ -I$(abs_top_srcdir)/src/newgal/stgfb/st_include


SRC_FILES = stgfb_video.c

HDR_FILES= stgfb_video.h

noinst_LTLIBRARIES = libvideo_stgfb.la

libvideo_stgfb_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng

LIB_NAME=libvideo_stgfb
