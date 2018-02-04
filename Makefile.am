SUBDIRS = src 3rd-party include rtos etc m4 cmake build

EXTRA_DIST = \
			minigui.pc.in \
            GNUmakefile makefile.ng makefile.msvc rules.make \
            Version INSTALL LICENSE \
            README.md RELEASE-NOTES.md \
            license-policy-en.md license-policy-zh.md 

# The directory where the include files will be installed
libminiguiincludedir = $(includedir)/minigui

# Which header files to install
libminiguiinclude_HEADERS = mgconfig.h

docs:
	doxygen Doxyfile

dist-nolicense: dist
	./clean_license.sh $(DIST_ARCHIVES)

dist:
distcheck-nolicense:dist-nolicense distcheck

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = minigui.pc

