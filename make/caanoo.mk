include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_OPT := 3
A_CONFIG_INPUT_MOUSE_CURSOR := 0
A_CONFIG_LIB_PNG := 1
A_CONFIG_LIB_SDL := 1
A_CONFIG_SCREEN_FULLSCREEN := 1
A_CONFIG_SCREEN_HARDWARE_WIDTH := 320
A_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
A_CONFIG_SYSTEM_CAANOO := 1
A_CONFIG_SYSTEM_LINUX := 1

CAANOO_SDK := $(A_SDK_CAANOO_ROOT)/$(A_SDK_CAANOO_TOOLCHAIN)

A_CONFIG_APP_NAME_SUFFIX := .gpe

A_CONFIG_BUILD_LIBS += \
    -L$(A_SDK_CAANOO_ROOT)/lib/target \
    -L$(A_SDK_CAANOO_ROOT)/DGE/lib/target \
    -L$(CAANOO_SDK)/lib \
    -L$(CAANOO_SDK)/arm-gph-linux-gnueabi/lib \
    -lpng12 \
    -lz \
    -lSDL_mixer \
    -lsmpeg \
    -lSDL \
    -lpthread \
    -lm \
    -ldl \

A_CONFIG_BUILD_CFLAGS += \
    -march=armv5te \
    -mtune=arm9tdmi \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(A_SDK_CAANOO_ROOT)/include \
    -isystem$(A_SDK_CAANOO_ROOT)/DGE/include \
    -isystem$(CAANOO_SDK)/include \
    -isystem$(CAANOO_SDK)/arm-gph-linux-gnueabi/include \
    -Wno-conversion \
    -Wno-override-init \

PREFIX := arm-gph-linux-gnueabi-

export PATH    := $(CAANOO_SDK)/bin:$(PATH)
export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export READELF := $(PREFIX)readelf
export STRIP   := $(PREFIX)strip

include $(A2X_PATH)/make/global/rules.mk
