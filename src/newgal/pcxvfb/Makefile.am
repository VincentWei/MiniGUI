
## Makefile.am for MiniGUI using the Qt virtual framebuffer video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/ -I/usr/include/

noinst_LTLIBRARIES = libvideo_pcxvfb.la
libvideo_pcxvfb_la_SOURCES = $(PCXVFB_SRCS)

# The PCXVFB virtual FrameBuffer video driver sources
PCXVFB_SRCS = pcxvfb.h pcxvfb.c 
