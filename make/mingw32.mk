include $(FAUR_PATH)/make/global/defs.mk

MINGW_BIN := $(A_SDK_MINGW32_ROOT)/$(A_SDK_MINGW32_ARCH)/bin

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_OPT := 3
A_CONFIG_LIB_PNG := 1
A_CONFIG_LIB_SDL := 2
A_CONFIG_LIB_SDL_CONFIG := $(MINGW_BIN)/sdl2-config
A_CONFIG_LIB_SDL_TIME := 1
A_CONFIG_SYSTEM_MINGW := 1
A_CONFIG_TRAIT_DESKTOP := 1
A_CONFIG_TRAIT_KEYBOARD := 1

A_CONFIG_APP_NAME_SUFFIX := .exe

A_CONFIG_BUILD_LIBS += \
    -u _SDL_main \
    -lpng \
    -lm \

A_CONFIG_BUILD_CFLAGS += \

PREFIX := $(A_SDK_MINGW32_ARCH)-

export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export READELF := $(PREFIX)readelf
export STRIP   := $(PREFIX)strip

include $(FAUR_PATH)/make/global/rules.mk

run :
	cd $(A_DIR_BIN) && wine ./$(A_FILE_BIN)
