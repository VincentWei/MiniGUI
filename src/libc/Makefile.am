AM_CPPFLAGS = -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/

noinst_LTLIBRARIES = liblibc.la

SRC_FILES=malloc.c \
        defdev.c stdioinlines.c \
        fnprintf.c fprintf.c printf.c vfnprintf.c \
        snprintf.c  sprintf.c vsnprintf.c vfscanf.c  \
        vsscanf.c fscanf.c  sscanf.c scanf.c \
        ucos2_pthread.c ucos2_mutex.c ucos2_sem.c \
        sysvipc_private.c sysvipc_mutex.c sysvipc_sem.c \
        threadx_pprivate.c threadx_pthread.c threadx_mutex.c threadx_sem.c \
        nucleus_pthread.c nucleus_mutex.c nucleus_sem.c \
        vxworks_pthread.c vxworks_mutex.c vxworks_sem.c \
		ose_sem.c \
        psos_pprivate.c psos_pthread.c psos_mutex.c psos_sem.c

HDR_FILES=ucos2_pprivate.h sysvipc_private.h ieeefp.h \
		threadx_pprivate.h nucleus_pprivate.h vxworks_pprivate.h \
		psos_pprivate.h

EXTRA_DIST=makefile.ng makefile.msvc vxworks_sem.c

liblibc_la_SOURCES=$(SRC_FILES) $(HDR_FILES)
LIB_NAME=liblibc
