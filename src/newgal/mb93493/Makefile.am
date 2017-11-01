## Makefile.am for the mb93493 video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			                                    -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_mb93493.la

libvideo_mb93493_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

HDR_FILES = \
	mb93493video.h \
	vdc_init.h

SRC_FILES = \
    mb93493video.c \
	vdc_init.c

EXTRA_DIST=makefile.ng
LIB_NAME=libvideo_mb93493
