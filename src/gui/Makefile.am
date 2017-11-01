AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libgui.la

EXTRA_DIST = de-latin1-nodeadkeys-kmap.c makefile.ng makefile.msvc

SRC_FILES = accelkey.c caret.c ctrlclass.c dialog.c menu.c window.c \
    keyboard.c \
    defkeymap.c fr-pc-kmap.c fr-kmap.c de-kmap.c de-latin1-kmap.c \
	hebrewkeymap.c arabickeymap.c \
    it-kmap.c es-kmap.c es-cp850-kmap.c \
	lf_manager.c lf_classic.c lf_flat.c lf_skin.c lf_common.c

HDR_FILES = keyboard.h linux_kd.h  linux_keyboard.h  \
			linux_types.h lf_common.h

libgui_la_SOURCES=$(SRC_FILES) $(HDR_FILES)

LIB_NAME=libgui
