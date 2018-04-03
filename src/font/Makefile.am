SUBDIRS = in-core

noinst_LTLIBRARIES = libfont.la

SRC_FILES = charset.c charset-arabic.c charset-bidi.c \
            sysfont.c logfont.c devfont.c fontname.c \
			rawbitmap.c varbitmap.c qpf.c upf.c \
            fontcache.c freetype1.c freetype2.c font-engines.c\
            gbunimap.c gbkunimap.c gb18030unimap.c big5unimap.c \
            ujisunimap.c sjisunimap.c euckrunimap.c \
            textops.c mapunitogb.c mapunitogbk.c mapunitobig5.c mapunitogb18030.c \
            bitmapfont.c scripteasy.c

HDR_FILES = charset.h rawbitmap.h varbitmap.h freetype1.h freetype2.h qpf.h  \
			upf.h bitmapfont.h gunicode.h gunichartables.h se_minigui.h

EXTRA_DIST = convgbmap.c jisunimap.c $(SRC_FILES) $(HDR_FILES) \
             makefile.ng makefile.msvc

AM_CPPFLAGS = $(FT1_INC_DIR) $(FT2_INC_DIR)
AM_CPPFLAGS += -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

libfont_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

LIB_NAME=libfont
