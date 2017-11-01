AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/ -I/EM85xx_with_GUI/kernelmodule

noinst_LTLIBRARIES = libvideo_em85xxosd.la
libvideo_em85xxosd_la_SOURCES = $(EM85XXOSD_SRCS)

EM85XXOSD_SRCS=em85xxosd.c em85xxosd.h realmagichwl_userland_api.c

