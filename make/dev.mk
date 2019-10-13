include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_DEBUG ?= 1
A_CONFIG_BUILD_DEBUG_ALLOC ?= 1
A_CONFIG_BUILD_OPT ?= -O0 -g
A_CONFIG_LIB_PNG ?= 1
A_CONFIG_LIB_SDL ?= 2
A_CONFIG_LIB_SDL_TIME := 1
A_CONFIG_SYSTEM_LINUX := 1
A_CONFIG_TRAIT_DESKTOP := 1
A_CONFIG_TRAIT_KEYBOARD := 1

ifeq ($(A_CONFIG_LIB_SDL), 1)
    A_CONFIG_LIB_SDL_CONFIG := sdl-config
else ifeq ($(A_CONFIG_LIB_SDL), 2)
    A_CONFIG_LIB_SDL_CONFIG := sdl2-config
endif

A_CONFIG_BUILD_LIBS += \
    -lpng \
    -lm \
    -rdynamic \
    -pie \

A_CONFIG_BUILD_CFLAGS += \
    -fpie \

include $(A2X_PATH)/make/global/rules.mk

all : $(A2X_FILE_EDITOR_TAGS)
