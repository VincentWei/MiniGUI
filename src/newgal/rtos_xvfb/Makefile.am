
## Makefile.am for MiniGUI using the RTOS virtual framebuffer video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/ -I/usr/include/

noinst_LTLIBRARIES = libvideo_rtos_xvfb.la
libvideo_rtos_xvfb_la_SOURCES = $(RTOS_XVFB_SRCS)

# The RTOS virtual FrameBuffer video driver sources
RTOS_XVFB_SRCS = rtos_xvfb.h rtos_xvfb.c
