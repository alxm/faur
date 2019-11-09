include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_DEBUG ?= 1
F_CONFIG_BUILD_DEBUG_ALLOC ?= 1
F_CONFIG_BUILD_OPT ?= 0
F_CONFIG_LIB_PNG ?= 1
F_CONFIG_LIB_SDL ?= 2
F_CONFIG_LIB_SDL_TIME := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_DESKTOP := 1
F_CONFIG_TRAIT_KEYBOARD := 1

ifeq ($(F_CONFIG_LIB_SDL), 1)
    F_CONFIG_LIB_SDL_CONFIG := sdl-config
else ifeq ($(F_CONFIG_LIB_SDL), 2)
    F_CONFIG_LIB_SDL_CONFIG := sdl2-config
endif

F_CONFIG_BUILD_LIBS += \
    -lpng \
    -lm \
    -rdynamic \
    -pie \

F_CONFIG_BUILD_FLAGS_SHARED += \
    -fpie \

include $(FAUR_PATH)/make/global/rules.mk

all : $(FAUR_FILE_EDITOR_TAGS)
