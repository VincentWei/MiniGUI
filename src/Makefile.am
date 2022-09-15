COMMON_SUBDIRS = libc misc main kernel sysres ial control ex_ctrl font gui mybmp include textedit newgal newgdi

COMMON_LADD =                   \
    libc/liblibc.la             \
    misc/libmisc.la             \
    control/libcontrol.la       \
    ex_ctrl/libexctrl.la        \
    font/libfont.la             \
    font/in-core/libincore.la   \
    ial/libial.la               \
    gui/libgui.la               \
    main/libmain.la             \
    mybmp/libmybmp.la           \
    kernel/libkernel.la         \
    sysres/libsysres.la         \
    textedit/libtextedit.la     \
    newgal/libnewgal.la         \
    newgdi/libnewgdi.la

DIST_SUBDIRS = $(COMMON_SUBDIRS) newgal newgdi server client standalone
SUBDIRS = $(COMMON_SUBDIRS) @RUNMODE_SUBDIRS@

MGSOURCES = template.c template.h
MGLIBADD =  $(COMMON_LADD) @RUNMODE_LADD@
MGLDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

lib_LTLIBRARIES=libminigui_@MGLIB_SUFFIX@.la
libminigui_@MGLIB_SUFFIX@_la_LIBADD=$(MGLIBADD) @DEP_LIBS@
libminigui_@MGLIB_SUFFIX@_la_SOURCES=$(MGSOURCES) 
libminigui_@MGLIB_SUFFIX@_la_LDFLAGS=$(MGLDFLAGS) 

CLEANFILES=*.a

EXTRA_DIST=makefile.ng makefile.msvc
