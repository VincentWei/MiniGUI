## Makefile.am for the UTStarcom PocketMedia video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_utpmc.la
libvideo_utpmc_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

SRC_FILES = utpmcvideo.c utpmcvideo-lite.c
HDR_FILES = utpmcvideo-lite.h utpmcvideo.h

EXTRA_DIST=makefile.ng
LIB_NAME=libvideo_utpmc
