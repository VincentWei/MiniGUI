
export CROSS=arm-hismall-linux-

TARGET=hi3560ainit
SRC=hi3560a_init.c

CFLAGS=-g

LIBS=-lstdc++ 

PLATFORM_ROOT=$(PREFIX)

CFG_INC_3560E += -I$(PLATFORM_ROOT)/sdk/include/comm
CFG_INC_3560E += -I$(PLATFORM_ROOT)/sdk/include/sdk

CFG_LIB_3560E += -L$(PLATFORM_ROOT)/sdk/lib/sdk/
CFG_LIB_3560E += -L$(PLATFORM_ROOT)/sdk/lib/comm/

COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libhigo.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libz.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libpng.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libjpeg.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libunify.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libapi_endplayer.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libapi_subplayer.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libapi.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libapi_extdrv.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libtde.a
COMMON_LIBS_3560E += $(PLATFORM_ROOT)/sdk/lib/sdk/libpanel.a

all:
	$(CROSS)gcc $(SRC) -o $(TARGET) $(CFLAGS) $(CFG_INC_3560E) $(CFG_LIB_3560E) $(COMMON_LIBS_3560E) $(LIBS)

.PHONY:
clean:
	- rm $(TARGET)
