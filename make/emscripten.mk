include $(A2X_PATH)/make/global/defs.mk

ifndef A_DO_BUILD

all :
	emmake $(MAKE) -f $(firstword $(MAKEFILE_LIST)) A_DO_BUILD=1

% :
	emmake $(MAKE) -f $(firstword $(MAKEFILE_LIST)) A_DO_BUILD=1 $@

else

A_CONFIG_BUILD_OPT := -O3
A_CONFIG_LIB_SDL ?= 2
A_CONFIG_LIB_SDL_TIME := 1
A_CONFIG_PATH_STORAGE_PREFIX := /a2x-idbfs/
A_CONFIG_SCREEN_BPP := 32
A_CONFIG_SCREEN_VSYNC ?= 1
A_CONFIG_SYSTEM_EMSCRIPTEN := 1
A_CONFIG_SYSTEM_LINUX := 1
A_CONFIG_TRAIT_KEYBOARD := 1
A_CONFIG_TRAIT_NOSLEEP := 1

ifeq ($(A_CONFIG_LIB_SDL), 1)
    A_CONFIG_SCREEN_FORMAT ?= ABGR
else ifeq ($(A_CONFIG_LIB_SDL), 2)
    A_CONFIG_SCREEN_FORMAT ?= RGBA
endif

A_BUILD_EMSCRIPTEN_LIBS := \
    -s USE_SDL=$(A_CONFIG_LIB_SDL) \
    -s USE_SDL_MIXER=$(A_CONFIG_LIB_SDL) \
    -s USE_ZLIB=1 \
    -s USE_LIBPNG=1 \
    -s TOTAL_MEMORY=67108864 \

A_BUILD_EMSCRIPTEN_EMBED := \
    --use-preload-plugins \
    $(addprefix --preload-file $(A_DIR_ROOT)/, \
        $(join $(A_CONFIG_PATH_EMBED_EMSCRIPTEN), \
               $(addprefix @, $(A_CONFIG_PATH_EMBED_EMSCRIPTEN)))) \

A_PLATFORM_LIBS := \
    $(A_CONFIG_BUILD_OPT) \
    $(A_BUILD_EMSCRIPTEN_LIBS) \
    $(A_BUILD_EMSCRIPTEN_EMBED) \

A_PLATFORM_CFLAGS := \
    $(A_BUILD_EMSCRIPTEN_LIBS) \
    -Wno-dollar-in-identifier-extension \

include $(A2X_PATH)/make/global/rules.mk

endif
