AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

SUBDIRS = bmp icon font cursor 
if MG_ENABLE_SPLASH
if MG_ENABLE_SCREENSAVER
SUBDIRS+=license
endif
endif

noinst_LTLIBRARIES = libsysres.la

libsysres_la_SOURCES = resource.c incoreres.c mgetc.c resmgr.c resmgr.h

SRC_FILES=$(libsysres_la_SOURCES)
LIB_NAME=libsysres

libsysres_la_LIBADD= bmp/libincorebmps.la \
	icon/libincoreicons.la \
	font/libincorefonts.la \
	cursor/libincorecursors.la 

EXTRA_DIST=makefile.ng makefile.msvc
