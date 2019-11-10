include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 2
F_CONFIG_LIB_SDL_CONFIG := sdl2-config
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_DESKTOP := 1
F_CONFIG_TRAIT_KEYBOARD := 1

F_CONFIG_BUILD_LIBS += \
    -L/usr/lib/i386-linux-gnu \
    -lpng \
    -lm \
    -pie \

F_CONFIG_BUILD_FLAGS_SHARED += \
    -m32 \
    -fpie \

include $(FAUR_PATH)/make/global/rules.mk
