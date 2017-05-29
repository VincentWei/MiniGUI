#
# This file contains rules which are shared among multiple platforms.
# used for makefile.ng
#

# ------> Please changes to your own rules specific to platform. <------
TARGET_RULES=build/rules-pc.linux

# ----------------------------------------------------------------------
include $(abs_top_srcdir)/$(TARGET_RULES)

# Common rules definitions for building MiniGUI with GNU Make

# Flags that can be set on the nmake command line:
#   DEBUG=1   for compiling a debugging version
#   PREFIX=Some/Directory       Base directory for installation
#   CONFIGH=YourConfigHeader    for defining the mgconfig.h file to use

ifndef DEBUG
DEBUG=0
endif

ifndef PREFIX
PREFIX = c:/usr
endif

# Directories used by "make install":
prefix = $(PREFIX)
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include
datadir = $(prefix)/share
localedir = $(datadir)/locale
mandir = $(datadir)/man
docdir = $(datadir)/doc/minigui

# The directory where the include files will be installed
libminiguiincludedir = $(includedir)/minigui
libmgextincludedir = $(includedir)/minigui


# Programs used by "make":
CP = cp
LN = cp
RM = rm -f

# Programs used by "make install":
INSTALL = cp
INSTALL_PROGRAM = cp
INSTALL_DATA = cp

ifndef DEPLIBDIR
DEPLIBDIR=$(PREFIX)
endif

######################################################################

#DIRENT_CFLAGS = -Ibuild/win32/dirent
#DIRENT_LIBS = build/win32/dirent/dirent.lib

# Don't know if Freetype2, FriBiDi and some others actually can be
# built with MSVC, but one can produce an import library even if the
# DLL was built with gcc.

FREETYPE2_CFLAGS = -I $(FREETYPE2)/include
FREETYPE2_LIBS = $(FREETYPE2)/obj/freetype-$(FREETYPE2_VER).lib

JPEG_CFLAGS = -I $(JPEG)
JPEG_LIBS = $(JPEG)/jpeg.lib

OPENGL_CFLAGS = # None needed, headers bundled with the compiler
OPENGL_LIBS = opengl32.lib lglu32.lib

PNG_CFLAGS = -I $(DEPLIBDIR)
PNG_LIBS = $(DEPLIBDIR)/lib/png.lib

ifndef NOTHREAD
PTHREAD_CFLAGS = -I $(DEPLIBDIR)/include
PTHREAD_LIBS = $(DEPLIBDIR)/lib/pthreadVC1.lib
else
PTHREAD_CFLAGS =
PTHREAD_LIBS =
endif

TIFF_CFLAGS = -I $(TIFF)/libtiff
# Use single import library for both libtiff DLL versions (with or
# without LZW code). The user selects which DLL to use.
TIFF_NOLZW_LIBS = $(TIFF)/libtiff/tiff.lib $(JPEG_LIBS) $(ZLIB_LIBS) user32.lib
TIFF_LZW_LIBS = $(TIFF_NOLZW_LIBS)
TIFF_LIBS = $(TIFF_NOLZW_LIBS)

ZLIB_CFLAGS = -I $(ZLIB)
ZLIB_LIBS = $(ZLIB)/zlib.lib

ifdef COMPILE_MGEXTLIB
MGEXT_CFLAGS = -D__MGEXT_LIB__
else
MGEXT_CFLAGS =
endif

MG_PRE_DEFINES = -D__MINIGUI_LIB__ $(MGEXT_CFLAGS)

#################################################################################

ifdef LOCAL_INC
INCLUDES = $(LOCAL_INC)
endif

ifndef OBJ
OBJ=o
endif

ifndef LIBA
LIBA=a
endif

#################################################################################

DEPLIBINC = -I$(DEPLIBDIR)/include

CFLAGS += $(OPTIMIZE) $(DEBUGINFO) $(DEPLIBINC) $(INCLUDES) $(DEFINES) $(DEPCFLAGS) $(MG_PRE_DEFINES) -c

DEFS += -D__MINIGUI_LIB__ -D_REENTRANT
CFLAGS += -I$(abs_top_srcdir) $(INCLUDES) $(DEFS) $(INCS)

%.$(OBJ): %.c
	$(CC) $(CFLAGS) $(COFLAG) $< 

ifdef SRC_FILES
OBJ_FILES=$(SRC_FILES:.c=.$(OBJ))
endif

######################################################################

default : all

all :: makefile.ng

ifdef LIB_NAME
all :: $(LIB_NAME).$(LIBA)
endif

$(LIB_NAME).$(LIBA):$(OBJ_FILES)
	$(AR) $(ARFLAGS) $(ARFLAGS_OUT)$@ $^
	@$(RANLIB) $@

clean::
	@-$(RM) *.$(OBJ)
	@-$(RM) *.$(LIBA)
ifeq ($(CC), cl)
	@-$(RM) *.dll
	@-$(RM) *.exe
	@-$(RM) *.res
	@-$(RM) *.i
	@-$(RM) *.err
	@-$(RM) *.map
	@-$(RM) *.sym
	@-$(RM) *.exp
	@-$(RM) *.lk1
	@-$(RM) *.mk1
	@-$(RM) *.pdb
	@-$(RM) *.ilk
endif

ifeq (1, $(TOPLOOP))
install :: installdirs

clean install ::
ifdef SUB_DIRS
	@for i in $(SUB_DIRS); do $(MAKE) -C $$i -f makefile.ng $@; done
endif

all::
	@for i in $(SUB_DIRS); do $(MAKE) -C $$i -f makefile.ng $@; done

else
install :: force

endif

force:
