AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

SRC_FILES = hi3510_fbvideo.c gal_hi3560a.c hi3560_fbvideo.c pix_array.c tde.c
HDR_FILES = hi3510_fb.h hi3560_fb.h hi3510_fbvideo.h hi3560_fbvideo.h \
			hi_tde.h tde_reg.h gal_hi3560a.h

noinst_LTLIBRARIES = libvideo_hisifbcon.la
libvideo_hisifbcon_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=

LIB_NAME=libvideo_hisifbcon
