AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

SRC_FILES = commlcd.c extern.c \
			ecos_generic.c ose_mx21.c axlinux_c33l05.c
HDR_FILES = commlcd.h axlinux_c33l05.h

noinst_LTLIBRARIES = libvideo_commlcd.la
libvideo_commlcd_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_commlcd
