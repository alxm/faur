include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC := 0
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := c++98
F_CONFIG_BUILD_OPT := 3
F_CONFIG_INPUT_MOUSE_CURSOR := 0
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_HARDWARE_WIDTH := 320
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
F_CONFIG_SYSTEM_CAANOO := 1
F_CONFIG_SYSTEM_LINUX := 1

CAANOO_SDK := $(F_SDK_CAANOO_ROOT)/$(F_SDK_CAANOO_TOOLCHAIN)

F_CONFIG_APP_NAME_SUFFIX := .gpe

F_CONFIG_BUILD_LIBS += \
    -L$(F_SDK_CAANOO_ROOT)/lib/target \
    -L$(F_SDK_CAANOO_ROOT)/DGE/lib/target \
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

F_CONFIG_BUILD_FLAGS_SHARED += \
    -march=armv5te \
    -mtune=arm9tdmi \
    -msoft-float \
    -ffast-math \
    -fomit-frame-pointer \
    -isystem$(F_SDK_CAANOO_ROOT)/include \
    -isystem$(F_SDK_CAANOO_ROOT)/DGE/include \
    -isystem$(CAANOO_SDK)/include \
    -isystem$(CAANOO_SDK)/arm-gph-linux-gnueabi/include \
    -Wno-conversion \

F_CONFIG_BUILD_FLAGS_C += \
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

include $(FAUR_PATH)/make/global/rules.mk
