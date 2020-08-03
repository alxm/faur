include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_APP_NAME_SUFFIX := .gpe
F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC := 0
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := c++98
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_LIB_SDL_CURSOR := 0
F_CONFIG_LIB_SDL_TIME := 0
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_HARDWARE_WIDTH := 320
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
F_CONFIG_SYSTEM_CAANOO := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_SLOW_MUL := 1

F_CONFIG_BUILD_LIBS += \
    -L$(F_SDK_CAANOO_ROOT)/lib/target \
    -L$(F_SDK_CAANOO_ROOT)/DGE/lib/target \
    -L$(F_SDK_CAANOO_TOOLCHAIN)/lib \
    -L$(F_SDK_CAANOO_TOOLCHAIN)/arm-gph-linux-gnueabi/lib \
    -lpng12 \
    -lz \
    -lSDL_mixer \
    -lsmpeg \
    -lSDL \
    -lpthread \
    -lm \
    -ldl \

F_CONFIG_BUILD_FLAGS_SHARED += \
    -march=armv5te \
    -mtune=arm9tdmi \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(F_SDK_CAANOO_ROOT)/include \
    -isystem$(F_SDK_CAANOO_ROOT)/DGE/include \
    -isystem$(F_SDK_CAANOO_TOOLCHAIN)/include \
    -isystem$(F_SDK_CAANOO_TOOLCHAIN)/arm-gph-linux-gnueabi/include \
    -Wno-conversion \

F_CONFIG_BUILD_FLAGS_C += \
    -Wno-override-init \
    -fgnu89-inline \

F_TOOLCHAIN_PREFIX := arm-gph-linux-gnueabi-

export PATH    := $(F_SDK_CAANOO_TOOLCHAIN)/bin:$(PATH)
export CC      := $(F_TOOLCHAIN_PREFIX)gcc
export CXX     := $(F_TOOLCHAIN_PREFIX)g++
export AS      := $(F_TOOLCHAIN_PREFIX)as
export AR      := $(F_TOOLCHAIN_PREFIX)ar
export OBJCOPY := $(F_TOOLCHAIN_PREFIX)objcopy
export READELF := $(F_TOOLCHAIN_PREFIX)readelf
export STRIP   := $(F_TOOLCHAIN_PREFIX)strip

include $(FAUR_PATH)/make/global/config.mk
include $(FAUR_PATH)/make/global/rules.mk
