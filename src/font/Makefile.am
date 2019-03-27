SUBDIRS = in-core

noinst_LTLIBRARIES = libfont.la

SRC_FILES = charset.c charset-arabic.c legacy-bidi.c charset-unicode.c \
            unicode-comp.c unicode-script.c language-code.c \
            sysfont.c logfont.c devfont.c fontname.c \
            rawbitmap.c varbitmap.c qpf.c upf.c \
            fontcache.c freetype2.c font-engines.c\
            gbunimap.c gbkunimap.c gb18030unimap.c big5unimap.c \
            ujisunimap.c sjisunimap.c euckrunimap.c \
            textops.c \
            mapunitogb.c mapunitogbk.c mapunitobig5.c mapunitogb18030.c \
            bitmapfont.c scripteasy.c \
            unicode-emoji.c unicode-vop.c \
            unicode-bidi.c \
            unicode-break.c unicode-break-arabic.c unicode-break-indic.c unicode-break-thai.c \
            unicode-joining-types.c unicode-joining.c unicode-shape.c \
            unicode-shape-arabic.c harzbuff-minigui-funcs.c \
            unicode-iterators.c

HDR_FILES = charset.h rawbitmap.h varbitmap.h freetype2.h qpf.h se_minigui.h \
            upf.h bitmapfont.h unicode-bidi-tables.h \
            unicode-tables.h unicode-break-tables.h unicode-script-table.h \
            unicode-decomp.h unicode-comp.h \
            unicode-emoji-tables.h \
            legacy-bidi-tables.inc \
            unicode-bidi.h unicode-vop-table.inc \
            unicode-joining-types-list.inc \
            unicode-joining-type-table.inc \
            unicode-arabic-shaping-table.inc \
            unicode-arabic-misc-table.inc \
            unicode-bidi-bidi-types-list.inc \
            unicode-bidi-bidi-type-table.inc \
            unicode-bidi-brackets-table.inc \
            unicode-bidi-brackets-type-table.inc \
            unicode-bidi-mirroring-table.inc \
            script-language-table.inc \
            emoji_presentation_scanner.inc

EXTRA_DIST = convgbmap.c jisunimap.c $(SRC_FILES) $(HDR_FILES) \
             makefile.ng makefile.msvc

AM_CPPFLAGS = $(FT2_INC_DIR) $(HB_INC_DIR)
AM_CPPFLAGS += -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

libfont_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

LIB_NAME=libfont
