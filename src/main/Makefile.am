# also for makefile.ng and makefile.msvc
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libmain.la

EXTRA_DIST=main.c makefile.ng makefile.msvc

libmain_la_SOURCES = main.c minigui-win32.c

# for makefile.ng and makefile.msvc
LIB_NAME=libmain
SRC_FILES=$(libmain_la_SOURCES)
