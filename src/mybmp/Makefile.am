AM_CPPFLAGS = $(WEBP_CFLAGS)
AM_CPPFLAGS += -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libmybmp.la

SRC_FILES = mybmp.c winbmp.c gif.c jpeg.c png12.c png16.c webp.c lbm.c pcx.c tga.c

libmybmp_la_SOURCES = $(SRC_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libmybmp
