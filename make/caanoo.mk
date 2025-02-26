include $(FAUR_PATH)/make/global/defs-first.mk
include $(FAUR_PATH)/make/global/defs-sdk.mk

F_CONFIG_APP_NAME_SUFFIX := .gpe
F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC := 0
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := c++98
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_LIB_SDL_CURSOR := 0
F_CONFIG_LIB_SDL_TIME := 0
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_RENDER := F_SCREEN_RENDER_SOFTWARE
F_CONFIG_SCREEN_SIZE_HEIGHT_HW := 240
F_CONFIG_SCREEN_SIZE_WIDTH_HW := 320
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
    -ldl

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    -march=armv5te \
    -mtune=arm9tdmi \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(F_SDK_CAANOO_ROOT)/include \
    -isystem$(F_SDK_CAANOO_ROOT)/DGE/include \
    -isystem$(F_SDK_CAANOO_TOOLCHAIN)/include \
    -isystem$(F_SDK_CAANOO_TOOLCHAIN)/arm-gph-linux-gnueabi/include

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP_OVERRIDE += \
    -Wno-conversion

F_CONFIG_BUILD_FLAGS_C += \
    -fgnu89-inline

F_CONFIG_BUILD_FLAGS_C_OVERRIDE += \
    -Wno-override-init

F_CONFIG_BUILD_TOOL_CC := $(F_SDK_CAANOO_TOOLCHAIN)/bin/arm-gph-linux-gnueabi-gcc
F_CONFIG_BUILD_TOOL_CPP := $(F_SDK_CAANOO_TOOLCHAIN)/bin/arm-gph-linux-gnueabi-g++

include $(FAUR_PATH)/make/global/defs-config.mk
include $(FAUR_PATH)/make/global/rules.mk
