# The input Makefile for MiniGUI header files

SUBDIRS = ctrl

# The directory where the include files will be installed
libminiguiincludedir = $(includedir)/minigui


# Which header files to install
COMM_HDRS =     \
    common.h endianrw.h fixedmath.h \
    minigui.h gdi.h window.h control.h \
    own_malloc.h  own_stdio.h \
    xvfb.h customial.h \
    dti.c 

UCOSII_HDRS=ucos2_pthread.h ucos2_semaphore.h
THREADX_HDRS=threadx_pthread.h threadx_semaphore.h
NUCLEUS_HDRS=nucleus_pthread.h nucleus_semaphore.h
VXWORKS_HDRS=vxworks_pthread.h vxworks_semaphore.h
WIN32_HDRS=win32_pthread.h win32_semaphore.h win32_sched.h win32_dirent.h
OSE_HDRS=ose_semaphore.h
PSOS_HDRS=psos_pthread.h psos_semaphore.h

EXTRA_DIST=$(UCOSII_HDRS) $(THREADX_HDRS) $(NUCLEUS_HDRS) $(VXWORKS_HDRS) \
           $(OSE_HDRS) $(PSOS_HDRS) $(WIN32_HDRS) makefile.ng makefile.msvc

#CLEANFILES=pthread.h semaphore.h

libminiguiinclude_HEADERS=$(COMM_HDRS) $(UCOSII_HDRS) $(THREADX_HDRS) \
				$(NUCLEUS_HDRS) $(VXWORKS_HDRS) $(OSE_HDRS) $(PSOS_HDRS) $(WIN32_HDRS)

if MGUSE_OWN_PTHREAD
install-data-local:
	cp ucos2_pthread.h  $(includedir)/pthread.h
	cp ucos2_semaphore.h  $(includedir)/semaphore.h
endif
