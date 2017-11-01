AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/font/

noinst_LTLIBRARIES = libincore.la

libincore_la_SOURCES =  rbf_fixedsys_8x15.c \
						rbf_terminal_8x12.c  \
						rbf_vgaoem_8x8.c      \
						vbf_courier_8x13.c    \
						vbf_sansserif_11x13.c \
						vbf_system_14x16.c \
 						upf_times_12x10.c \
						upf_times_17x14.c


EXTRA_DIST=makefile.ng makefile.msvc

SRC_FILES=$(libincore_la_SOURCES)
LIB_NAME=libincore
