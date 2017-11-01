
## Makefile.am for MiniGUI using the X virtual framebuffer video driver

AM_CPPFLAGS=-I/usr/include -I../../../include -I../../include -I../ 

noinst_LTLIBRARIES = libvideo_xlib.la
libvideo_xlib_la_SOURCES = $(XXVFB_SRCS)

# The Qt virtual FrameBuffer video driver sources
XXVFB_SRCS = xlibvideo.c  xlibvideo.h
