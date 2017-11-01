AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

SRC_FILES = mlshadow-ths.c mlshadow-proc.c 
HDR_FILES = mlshadow-ths.h mlshadow-proc.h

noinst_LTLIBRARIES = libvideo_mlshadow.la
libvideo_mlshadow_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_mlshadow
