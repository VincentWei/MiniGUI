AM_CPPFLAGS = -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/include
noinst_LTLIBRARIES = libmisc.la

EXTRA_DIST= misc.c systext.c rwops.c endianrw.c nposix.c clipboard.c \
			math.c about.c error.c sockio.c listenfd.c rc4.c rc4.h \
            makefile.ng makefile.msvc

LITE_SRCS=error.c sockio.c listenfd.c


if MG_MINIMALGDI
libmisc_la_SOURCES = misc.c math.c rwops.c nposix.c
else
libmisc_la_SOURCES = misc.c systext.c rwops.c endianrw.c nposix.c clipboard.c \
				math.c about.c license.c \
				$(LITE_SRCS)
endif

SRC_FILES = $(libmisc_la_SOURCES)
LIB_NAME=libmisc
