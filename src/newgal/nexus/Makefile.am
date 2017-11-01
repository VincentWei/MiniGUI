
## Makefile.am for the NEXUS video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_nexus.la

# The nexus video driver sources
SRC_FILES =             \
    nexusvideo.c nexusvideo_pri.c

HDR_FILES= \
    nexusvideo.h nexusvideo_pri.h

libvideo_nexus_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

libvideo_nexus_la_CFLAGS = -DBDBG_DEBUG_BUILD=1 -D_GNU_SOURCE=1 -DLINUX -pipe -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -Wall -pedantic -W -Wstrict-prototypes -Wno-unused-value -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -mips32 -DUCLIBC_VERSION=\"libuClibc-0.9.29.so\"  \
-DNEXUS_FPGA_SUPPORT -DNEXUS_MODE_ -DNEXUS_FRONTEND_3420 -DNEXUS_FRONTEND_2940 -DNEXUS_FRONTEND_4501 -DNEXUS_FRONTEND_4506 -DNEXUS_FRONTEND_3510 -DBCM_BOARD=97405 -DNEXUS_TUNER_SUPPORT -DBCHP_CHIP=7405 -DBCHP_VER=BCHP_VER_B0 -DBMEM_REENTRANT_CONFIG=BMEM_REENTRANT -DBINT_REENTRANT_CONFIG=BINT_REENTRANT -DNEXUS_PROFILE_OS_linuxuser -DNEXUS_BASE_OS_linuxuser=1 -DNEXUS_HAS_PLATFORM -DNEXUS_HAS_I2C -DNEXUS_HAS_SURFACE -DNEXUS_HAS_GRAPHICS2D -DNEXUS_HAS_DISPLAY -DNEXUS_HAS_PWM -DNEXUS_HAS_TRANSPORT -DNEXUS_HAS_SECURITY -DNEXUS_HAS_DMA -DNEXUS_HAS_VIDEO_DECODER -DNEXUS_HAS_AUDIO -DNEXUS_HAS_FRONTEND -DNEXUS_HAS_SPI -DNEXUS_HAS_GPIO -DNEXUS_HAS_LED -DNEXUS_HAS_IR_INPUT -DNEXUS_HAS_IR_BLASTER -DNEXUS_HAS_INPUT_CAPTURE -DNEXUS_HAS_UHF_INPUT -DNEXUS_HAS_KEYPAD -DNEXUS_HAS_CORE -DNEXUS_HAS_HDMI_OUTPUT -DNEXUS_HAS_SYNC_CHANNEL -DNEXUS_HAS_RFM -DNEXUS_HAS_ASTM -DNEXUS_HAS_SMARTCARD -DNEXUS_HAS_UART -DNEXUS_HAS_BASE -DNEXUS_HAS_FILE -DNEXUS_HAS_PLAYBACK -DNEXUS_HAS_RECORD -DB_HAS_NAV_PLAYER=1 \
-I$(NEXUS_INCLUDEDIR)/nexus/platforms/97405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/i2c/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/surface/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/graphics2d/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/display/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/pwm/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/transport/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/security/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/dma/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/video_decoder/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/audio/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/common/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/3420/include/ \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/2940/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/4501/include/ \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/4506/include/ \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/frontend/3510/include/ \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/spi/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/gpio/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/led/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/ir_input/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/ir_blaster/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/input_capture/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/uhf_input/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/keypad/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/core/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/hdmi_output/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/sync_channel/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/rfm/7405/include  \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/astm/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/smartcard/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/uart/7405/include \
-I$(NEXUS_INCLUDEDIR)/nexus/base/include \
-I$(NEXUS_INCLUDEDIR)/nexus/base/include/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/file/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/file/include/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/playback/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/record/include \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/playback/src \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/file/include/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/playback/src \
-I$(NEXUS_INCLUDEDIR)/nexus/modules/file/include/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/../BSEAV/lib/bcmplayer/include \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/kni/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/std \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/std/config \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/std/types/linuxuser \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/dbg \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/chp \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/chp/7405 \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/chp/7405/rdb/b0 \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/reg \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/basemodules/err \
-I$(NEXUS_INCLUDEDIR)/nexus/../magnum/commonutils/lst \
-I$(NEXUS_INCLUDEDIR)/nexus/../BSEAV/lib/netaccel/include


libvideo_nexus_la_LDFLAGS = -lpthread -lnexus -lm -lz -L$(NEXUS_LIBDIR) -Wl,-rpath-link=$(NEXUS_LIBDIR)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_nexus
