SUBDIRS = src 3rd-party include rtos etc m4 cmake build

EXTRA_DIST = minigui.pc.in \
            GNUmakefile makefile.ng makefile.msvc rules.make \
            INSTALL LICENSE \
            Version.md README.md RELEASE-NOTES.md \
            LICENSE-POLICY-en.md LICENSE-POLICY-zh.md

# The directory where the include files will be installed
libminiguiincludedir = $(includedir)/minigui

# Which header files to install
libminiguiinclude_HEADERS = mgconfig.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = minigui.pc

docs:
	doxygen Doxyfile.procs
	doxygen Doxyfile.ths
	doxygen Doxyfile.sal

#dist-nolicense: dist
#	./clean_license.sh $(DIST_ARCHIVES)
#
#dist:
#distcheck-nolicense:dist-nolicense distcheck

