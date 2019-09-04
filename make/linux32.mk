include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_OPT := -O3 -s
A_CONFIG_LIB_SDL := 2
A_CONFIG_LIB_SDL_CONFIG := sdl2-config
A_CONFIG_LIB_SDL_TIME := 1
A_CONFIG_SYSTEM_LINUX := 1
A_CONFIG_TRAIT_DESKTOP := 1
A_CONFIG_TRAIT_KEYBOARD := 1

A_PLATFORM_LIBS := \
    -L/usr/lib/i386-linux-gnu \
    -lpng \
    -lm \
    -pie \

A_PLATFORM_CFLAGS := \
    -m32 \
    -fpie \

include $(A2X_PATH)/make/global/rules.mk
