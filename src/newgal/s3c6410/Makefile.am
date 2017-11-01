
## Makefile.am for the S3C6410 video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_s3c6410.la

# The s3c6410 video driver sources
SRC_FILES = gal-s3c6410.c

HDR_FILES = gal-s3c6410.h

libvideo_s3c6410_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

libvideo_s3c6410_la_CFLAGS = 

EXTRA_DIST=

LIB_NAME=libvideo_s3c6410
