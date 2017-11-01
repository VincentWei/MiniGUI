AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			-I$(abs_top_srcdir)/src/newgal/ -I/opt/winbond/workdir/swlinux/include

SRC_FILES=svpxxosd.c
HDR_FILES = svphead.h svpxxosd.h

noinst_LTLIBRARIES = libvideo_svpxxosd.la
libvideo_svpxxosd_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

LIB_NAME=libvideo_svpxxosd

EXTRA_DIST=makefile.ng

