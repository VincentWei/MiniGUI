MAJOR_VERSION = 3
MINOR_VERSION = 0
MICRO_VERSION = 13
EXTRAVERSION =

MINIGUI_RELEASE=$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)$(EXTRAVERSION)

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)
TOPDIR	:= $(shell /bin/pwd)

HPATH   	= $(TOPDIR)/include

HOSTCC  	= gcc
HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

#
# Include the make variables (CC, etc...)
#

MAKEFILES	= $(TOPDIR)/.config
PERL		= perl
AWK		= gawk

export	MAJOR_VERSION MINOR_VERSION MICRO_VERSION EXTRAVERSION MINIGUI_RELEASE \
	CONFIG_SHELL TOPDIR HPATH HOSTCC HOSTCFLAGS \
	CPP AR NM STRIP OBJCOPY OBJDUMP MAKE MAKEFILES PERL AWK

-include Makefile

.PHONY: doit menuconfig defconfig

doit:
	@echo "You may need to run configure script to build Makefile."

menuconfig:
	$(MAKE) -C scripts/lxdialog -f makefile.lx all
	$(CONFIG_SHELL) scripts/Menuconfig configs/config.in

defconfig:
	@cp configs/defconfig .defconfig
	$(MAKE) -C scripts/lxdialog -f makefile.lx all
	$(CONFIG_SHELL) scripts/Menuconfig configs/config.in
	@rm .defconfig

nognu:
	$(MAKE) -f makefile.ng

cleanall:
	$(MAKE) -f makefile.ng clean
