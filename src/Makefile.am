if MG_MINIMALGDI
COMMON_SUBDIRS = misc kernel sysres include 

COMMON_LADD =               	\
    kernel/libkernel.la     	\
	sysres/libsysres.la         \
    misc/libmisc.la         	
else
COMMON_SUBDIRS = libc misc main kernel sysres ial control ex_ctrl font gui mybmp include textedit

COMMON_LADD =               	\
    libc/liblibc.la         	\
    misc/libmisc.la         	\
    control/libcontrol.la   	\
    ex_ctrl/libexctrl.la   		\
    font/libfont.la         	\
    font/in-core/libincore.la   \
    ial/libial.la           	\
    gui/libgui.la           	\
    main/libmain.la         	\
    mybmp/libmybmp.la       	\
    kernel/libkernel.la     	\
    sysres/libsysres.la         \
	textedit/libtextedit.la
endif

if MGRM_STANDALONE
LITE_LADD = standalone/libstandalone.la
LITE_SUBDIRS = standalone
else
if MGRM_PROCESSES
LITE_LADD = server/libserver.la client/libclient.la
LITE_SUBDIRS = server client
else
LITE_LADD = 
LITE_SUBDIRS = 
endif
endif

NEWGAL_LADD = newgal/libnewgal.la newgdi/libnewgdi.la
NEWGAL_SUBDIRS = newgal newgdi

DIST_SUBDIRS = $(COMMON_SUBDIRS) newgal newgdi server client standalone 
SUBDIRS = $(COMMON_SUBDIRS) $(NEWGAL_SUBDIRS) $(LITE_SUBDIRS) 

MGSOURCES = template.c template.h
MGLIBADD =  $(COMMON_LADD)      \
			$(LITE_LADD)        \
			$(NEWGAL_LADD)
MGLDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

lib_LTLIBRARIES=libminigui_@MGLIB_SUFFIX@.la
libminigui_@MGLIB_SUFFIX@_la_LIBADD=$(MGLIBADD)
libminigui_@MGLIB_SUFFIX@_la_SOURCES=$(MGSOURCES) 
libminigui_@MGLIB_SUFFIX@_la_LDFLAGS=$(MGLDFLAGS) 

CLEANFILES=*.a

EXTRA_DIST=makefile.ng makefile.msvc
