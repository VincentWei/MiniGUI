
## Makefile.am for MiniGUI using the Qt virtual framebuffer video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			-I$(abs_top_srcdir)/src/newgal/ -I/usr/include/

noinst_LTLIBRARIES = libvideo_qvfb.la
libvideo_qvfb_la_SOURCES = $(QVFB_SRCS)

# The Qt virtual FrameBuffer video driver sources
QVFB_SRCS = qvfb.h qvfb.c
