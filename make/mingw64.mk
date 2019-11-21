include $(FAUR_PATH)/make/global/defs.mk

MINGW_BIN := $(F_SDK_MINGW64_ROOT)/$(F_SDK_MINGW64_ARCH)/bin

F_CONFIG_APP_NAME_SUFFIX := .exe
F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 2
F_CONFIG_LIB_SDL_CONFIG := $(MINGW_BIN)/sdl2-config
F_CONFIG_SYSTEM_MINGW := 1
F_CONFIG_TRAIT_DESKTOP := 1
F_CONFIG_TRAIT_KEYBOARD := 1

F_CONFIG_BUILD_LIBS += \
    -u SDL_main \
    -lpng \
    -lm \

F_CONFIG_BUILD_FLAGS_SHARED += \

PREFIX := $(F_SDK_MINGW64_ARCH)-

export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export READELF := $(PREFIX)readelf
export STRIP   := $(PREFIX)strip

include $(FAUR_PATH)/make/global/rules.mk

run :
	cd $(F_DIR_BIN) && wine ./$(F_FILE_BIN)
