
## Makefile.am for the drm video driver
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/newgal/ -I$(DRM_INC_DIR)

noinst_LTLIBRARIES = libvideo_drm.la

# The drm video driver sources
SRC_FILES = \
    drmvideo.c

HDR_FILES= \
    drmvideo.h

libvideo_drm_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng

LIB_NAME=libvideo_drm
