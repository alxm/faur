include $(A2X_PATH)/make/global/defs.mk
include $(A_FILE_SDKCONFIG_MK)

ifndef A_DO_BUILD

A_MAKE_CMD := \
    source $(A_SDK_EMSCRIPTEN_ROOT)/emsdk_env.sh \
    && emmake $(MAKE) -f $(firstword $(MAKEFILE_LIST)) -j A_DO_BUILD=1

all :
	bash -c "$(A_MAKE_CMD)"

% :
	bash -c "$(A_MAKE_CMD) $@"

else

A_CONFIG_BUILD_C_STANDARD := gnu11
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

ifdef A_CONFIG_SYSTEM_EMSCRIPTEN_TOTAL_MEMORY
    A_BUILD_EMSCRIPTEN_LIBS += -s TOTAL_MEMORY=$(A_CONFIG_SYSTEM_EMSCRIPTEN_TOTAL_MEMORY)
endif

A_BUILD_EMSCRIPTEN_EMBED := \
    --use-preload-plugins \
    $(addprefix --preload-file $(A_DIR_ROOT)/, \
        $(join $(A_CONFIG_PATH_EMBED_EMSCRIPTEN), \
               $(addprefix @, $(A_CONFIG_PATH_EMBED_EMSCRIPTEN)))) \

A_PLATFORM_BIN_SUFFIX := .html

A_PLATFORM_LIBS := \
    $(A_CONFIG_BUILD_OPT) \
    $(A_BUILD_EMSCRIPTEN_LIBS) \
    $(A_BUILD_EMSCRIPTEN_EMBED) \

A_PLATFORM_CFLAGS := \
    $(A_BUILD_EMSCRIPTEN_LIBS) \
    -Wno-dollar-in-identifier-extension \
    -Wno-gnu-zero-variadic-macro-arguments \

include $(A2X_PATH)/make/global/rules.mk

run :
	cd $(A_DIR_BIN) && $(A2X_PATH)/bin/a2x_runweb $(A_FILE_BIN)

endif
