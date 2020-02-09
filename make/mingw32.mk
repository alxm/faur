include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_APP_NAME_SUFFIX := .exe
F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 2
F_CONFIG_LIB_SDL_CONFIG := $(F_SDK_MINGW32_ROOT)/i686-w64-mingw32/bin/sdl2-config
F_CONFIG_SYSTEM_MINGW := 1
F_CONFIG_TRAIT_DESKTOP := 1
F_CONFIG_TRAIT_KEYBOARD := 1

F_CONFIG_BUILD_LIBS += \
    -u _SDL_main \
    -lpng \
    -lm \

F_CONFIG_BUILD_FLAGS_SHARED += \
    -D__USE_MINGW_ANSI_STDIO=1 \

F_TOOLCHAIN_PREFIX := i686-w64-mingw32-

export CC      := $(F_TOOLCHAIN_PREFIX)gcc
export CXX     := $(F_TOOLCHAIN_PREFIX)g++
export AS      := $(F_TOOLCHAIN_PREFIX)as
export AR      := $(F_TOOLCHAIN_PREFIX)ar
export OBJCOPY := $(F_TOOLCHAIN_PREFIX)objcopy
export READELF := $(F_TOOLCHAIN_PREFIX)readelf
export STRIP   := $(F_TOOLCHAIN_PREFIX)strip

include $(FAUR_PATH)/make/global/rules.mk

run : all
	cd $(F_BUILD_DIR_BIN) && wine ./$(F_BUILD_FILE_BIN)
