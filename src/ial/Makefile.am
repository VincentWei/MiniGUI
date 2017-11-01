AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include

noinst_LTLIBRARIES = libial.la

if MGIAL_CONSOLE
NATIVE_SUBDIRS=native
endif

if MGIAL_NET
NETIAL_SUBDIRS=netial
endif

if MGIAL_NEXUS
NEXUS_SUBDIRS=nexusial
endif

if MGIAL_DLCUSTOM
DLCUSTOM_SUBDIRS=dlcustom
endif

SUBDIRS=$(NATIVE_SUBDIRS) $(NEXUS_SUBDIRS) $(NETIAL_SUBDIRS) $(DLCUSTOM_SUBDIRS)

DIST_SUBDIR=native nexusial netial

EXTRA_DIST=spec_general_ts_drv.html \
           makefile.ng makefile.msvc

COMMON_SRCS = ial.c

DUMMY_SRCS = dummy.c dummy.h
AUTO_SRCS = auto.c auto.h auto-protocol.h
JZ4740_SRCS = jz4740.c jz4740.h
SHANDONG_LIDE_SRCS = lide.c lide.h
2440_SRCS = 2440.c 2440.h
DAVINCI6446_SRCS=davinci6446.c davinci6446.h
RANDOM_SRCS = random.c random.h
PCXVFB_SRCS = pcxvfbial.c pcxvfb.h mgsock.c
RTOSXVFB_SRCS = rtos_xvfb.c rtos_xvfb.h
COMMINPUT_SRCS = comminput.c comminput.h
IPAQ_H3600_SRCS = ipaq-h3600.c ipaq-h3600.h
IPAQ_H5400_SRCS = ipaq-h5400.c ipaq-h5400.h
QEMU_SRCS = qemu.h qemu.c
TSLIB_SRCS = tslibial.h tslibial.c

QVFB_SRCS = qvfb.c qvfb.h
WVFB_SRCS = wvfbial.c wvfb.h
CISCO_TOUCHPAD_SRCS = cisco_touchpad.h cisco_touchpad.c
MSTAR_SRCS = mstarial.h mstarial.c
DFB_SRCS = dfb.h dfb.c

if MGIAL_CONSOLE
native_libial_la_LIBADD = native/libnative.la
endif

if MGIAL_NET
netial_libial_la_LIBADD = netial/libnetial.la
endif

if MGIAL_NEXUS
nexus_libial_la_LIBADD = nexusial/libnexusial.la
endif

if MGIAL_DLCUSTOM
dlcustom_libdlcustom_la_LIBADD = dlcustom/libdlcustom.la
endif

libial_la_LIBADD = $(native_libial_la_LIBADD) $(nexus_libial_la_LIBADD) $(netial_libial_la_LIBADD) \
				   $(dlcustom_libdlcustom_la_LIBADD)

libial_la_SOURCES = $(COMMON_SRCS) $(DUMMY_SRCS) $(JZ4740_SRCS) \
					$(AUTO_SRCS) $(RANDOM_SRCS) $(2440_SRCS) $(DAVINCI6446_SRCS) \
                    $(PCXVFB_SRCS) $(RTOSXVFB_SRCS) \
                    $(COMMINPUT_SRCS) $(QVFB_SRCS) $(WVFB_SRCS) \
					$(QEMU_SRCS) $(IPAQ_H3600_SRCS) $(IPAQ_H5400_SRCS) \
					$(TSLIB_SRCS) $(SHANDONG_LIDE_SRCS) $(CISCO_TOUCHPAD_SRCS) \
					$(MSTAR_SRCS) $(DFB_SRCS)
