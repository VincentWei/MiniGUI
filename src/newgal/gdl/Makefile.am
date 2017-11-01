## Makefile.am for MiniGUI using the Intel GDL video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			-I$(abs_top_srcdir)/src/newgal/ -I/usr/include/

noinst_LTLIBRARIES = libvideo_gdl.la
libvideo_gdl_la_SOURCES = $(GDL_SRCS)

# The Qt virtual FrameBuffer video driver sources
GDL_SRCS = gdl_video.h gdl_video.c
