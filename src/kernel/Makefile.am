# also for makefile.ng and makefile.msvc
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include
AM_CFLAGS = -Wno-zero-length-bounds

noinst_LTLIBRARIES = libkernel.la

libkernel_la_SOURCES = timer.c init.c init-lite.c \
                       desktop-ths.c desktop-sa.c desktop-procs.c \
                       cursor.c cursor-procs.c cursor-sa.c \
                       sharedres.c message.c event.c listenfd.c \
                       blockheap.c fixstr.c slotset.c zorder.c sem-manager.c \
                       compsor-manager.c compsor-fallback.c

EXTRA_DIST= desktop.c desktop-comm.c cursor-comm.c makefile.ng makefile.msvc

# for makefile.ng and makefile.msvc
LIB_NAME = libkernel
SRC_FILES = $(libkernel_la_SOURCES)
