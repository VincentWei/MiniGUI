
## Makefile.am for the dri video driver
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/newgal/ -I$(DRM_INC_DIR)

noinst_LTLIBRARIES = libvideo_dri.la

# The dri video driver sources
SRC_FILES = \
    drivideo.c

HDR_FILES= \
    drivideo.h

libvideo_dri_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng

LIB_NAME=libvideo_dri
