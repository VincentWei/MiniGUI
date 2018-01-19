# also for makefile.ng and makefile.msvc
AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libkernel.la

if MG_MINIMALGDI
libkernel_la_SOURCES = \
                       init-lite.c fixstr.c \
                       blockheap.c 
else
libkernel_la_SOURCES = timer.c \
                       init.c init-lite.c \
                       desktop-ths.c desktop-sa.c desktop-procs.c \
                       cursor.c cursor-procs.c cursor-sa.c \
                       sharedres.c \
                       message.c event.c \
                       blockheap.c fixstr.c slotset.c zorder.c
endif
# hash.c

EXTRA_DIST= desktop.c desktop-comm.c cursor-comm.c makefile.ng makefile.msvc

# for makefile.ng and makefile.msvc
LIB_NAME = libkernel
SRC_FILES = $(libkernel_la_SOURCES)
