## Makefile.am for MiniGUI using the Windows virtual framebuffer video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
            -I$(abs_top_srcdir)/src/newgal/ -I$(abs_top_srcdir)/

noinst_LTLIBRARIES = libvideo_wvfb.la

# The Windows virtual FrameBuffer video driver sources
SRC_FILES = wvfb.c winfb.c
HDR_FILES = wvfb.h winfb.h

libvideo_wvfb_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_wvfb
