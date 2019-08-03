include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_DEBUG ?= 1
A_CONFIG_BUILD_OPT ?= -O0 -g
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

A_PLATFORM_LIBS := \
    -lpng \
    -lm \
    -rdynamic \

A_PLATFORM_CFLAGS := \

include $(A2X_PATH)/make/global/rules.mk

all : $(A2X_FILE_EDITOR_TAGS) $(A_FILE_SDKCONFIG_MK)

$(A_FILE_SDKCONFIG_MK) : $(A_FILE_SDKCONFIG_SRC) $(A2X_PATH)/bin/a2x_sdkconfig
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_sdkconfig $< $@

$(A_FILE_SDKCONFIG_SRC) :
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_sdkconfig $@
