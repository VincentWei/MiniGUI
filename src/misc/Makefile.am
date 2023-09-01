AM_CPPFLAGS = -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/include
noinst_LTLIBRARIES = libmisc.la

EXTRA_DIST= misc.c systext.c rwops.c endianrw.c nposix.c clipboard.c \
            math.c about.c error.c sockio.c rc4.c rc4.h \
            makefile.ng makefile.msvc

libmisc_la_SOURCES = misc.c systext.c rwops.c endianrw.c nposix.c clipboard.c \
                math.c about.c license.c anon_file.c rbtree.c map.c \
                error.c sockio.c lock_file.c

SRC_FILES = $(libmisc_la_SOURCES)
LIB_NAME=libmisc
