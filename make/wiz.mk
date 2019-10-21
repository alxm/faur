include $(FAUR_PATH)/make/global/defs.mk

OPEN2X := $(F_SDK_OPEN2X_ROOT)/$(F_SDK_OPEN2X_TOOLCHAIN)

F_CONFIG_BUILD_OPT := 3
F_CONFIG_INPUT_MOUSE_CURSOR := 0
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_HARDWARE_WIDTH := 320
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
F_CONFIG_SYSTEM_WIZ_SCREEN_FIX ?= 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_SYSTEM_WIZ := 1

F_CONFIG_APP_NAME_SUFFIX := .gpe

F_CONFIG_BUILD_LIBS += \
    -L$(OPEN2X)/lib \
    -L$(OPEN2X)/arm-open2x-linux/lib \
    -lpng12 \
    -lz \
    -lSDL_mixer \
    -lsmpeg \
    -lSDL \
    -lpthread \
    -lm \
    -ldl \

F_CONFIG_BUILD_CFLAGS += \
    -march=armv5te \
    -mtune=arm9tdmi \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(OPEN2X)/include \
    -isystem$(OPEN2X)/arm-open2x-linux/include \
    -Wno-conversion \

PREFIX := arm-open2x-linux-

export PATH    := $(OPEN2X)/bin:$(PATH)
export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export READELF := $(PREFIX)readelf
export STRIP   := $(PREFIX)strip

include $(FAUR_PATH)/make/global/rules.mk
