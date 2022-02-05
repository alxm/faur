include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_APP_NAME_SUFFIX := .gpe
F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC := 0
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := c++98
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_LIB_SDL_CURSOR := 0
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_RENDER_SOFTWARE := 1
F_CONFIG_SCREEN_SIZE_HEIGHT_HW := 240
F_CONFIG_SCREEN_SIZE_WIDTH_HW := 320
F_CONFIG_SOUND_VOLUME_BAR := 1
F_CONFIG_SYSTEM_GP2X := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_SLOW_MUL := 1

F_CONFIG_BUILD_LIBS += \
    -static \
    -L$(F_SDK_OPEN2X_TOOLCHAIN)/lib \
    -L$(F_SDK_OPEN2X_TOOLCHAIN)/arm-open2x-linux/lib \
    -lpng12 \
    -lz \
    -lSDL_mixer \
    -lsmpeg \
    -lmikmod \
    -logg \
    -lvorbisidec \
    -lSDL \
    -lpthread \
    -lm \
    -lstdc++ \
    -ldl \

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    -mcpu=arm920t \
    -mtune=arm920t \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(F_SDK_OPEN2X_TOOLCHAIN)/include \
    -isystem$(F_SDK_OPEN2X_TOOLCHAIN)/arm-open2x-linux/include \
    -Wno-conversion \

F_TOOLCHAIN_PREFIX := arm-open2x-linux-

export PATH    := $(F_SDK_OPEN2X_TOOLCHAIN)/bin:$(PATH)
export CC      := $(F_TOOLCHAIN_PREFIX)gcc
export CXX     := $(F_TOOLCHAIN_PREFIX)g++
export AS      := $(F_TOOLCHAIN_PREFIX)as
export AR      := $(F_TOOLCHAIN_PREFIX)ar
export OBJCOPY := $(F_TOOLCHAIN_PREFIX)objcopy
export READELF := $(F_TOOLCHAIN_PREFIX)readelf
export STRIP   := $(F_TOOLCHAIN_PREFIX)strip

include $(FAUR_PATH)/make/global/config.mk
include $(FAUR_PATH)/make/global/rules.mk
