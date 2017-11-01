AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libnewgdi.la

if MG_MINIMALGDI
SRC_FILES = gdi.c attr.c clip.c coor.c rect.c  \
              bitmap.c \
              pixel.c pixel_ops.c \
              region.c polygon.c 
else
SRC_FILES = gdi.c attr.c clip.c map.c coor.c rect.c  \
              palette.c readbmp.c icon.c screen.c bitmap.c \
              pixel.c line.c arc.c pixel_ops.c \
              region.c generators.c polygon.c flood.c \
              advapi.c midash.c mispans.c miwideline.c \
              mifillarc.c mifpolycon.c miarc.c rotatebmp.c \
			  text.c glyph.c bidi.c \
			  textout.c tabbedtextout.c drawtext.c 
endif

HDR_FILES = drawtext.h mi.h midc.h mistruct.h miwideline.h \
            pixel_ops.h mifillarc.h mispans.h polygon.h mifpoly.h 

libnewgdi_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libnewgdi
