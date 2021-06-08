AM_CPPFLAGS = $(FT2_INC_DIR) $(HB_INC_DIR) $(PIXMAN_CFLAGS)
AM_CPPFLAGS += -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libnewgdi.la

SRC_FILES = gdi.c attr.c clip.c map.c coor.c rect.c  \
            palette.c readbmp.c icon.c screen.c bitmap.c \
            pixel.c line.c arc.c pixel_ops.c \
            region.c generators.c polygon.c flood.c \
            advapi.c midash.c mispans.c miwideline.c \
            mifillarc.c mifpolycon.c miarc.c rotatebmp.c \
            text.c achar-uchar.c glyph.c legacy-bidi.c \
            textout.c tabbedtextout.c drawtext.c \
            simple-glyph-renderer.c glyph-shaped.c \
            textruns.c \
            shape-glyphs-basic.c shape-glyphs-complex.c \
            layout.c layout-utils.c layout-ellipsize.c

HDR_FILES = glyph.h drawtext.h mi.h midc.h mistruct.h miwideline.h \
            pixel_ops.h mifillarc.h mispans.h polygon.h mifpoly.h \
            textruns.h layout.h

libnewgdi_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libnewgdi
