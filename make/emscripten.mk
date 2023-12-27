include $(FAUR_PATH)/make/global/defs.mk

ifndef F_DO_BUILD

F_MAKE_CMD := \
    source $(F_SDK_EMSCRIPTEN_ROOT)/emsdk_env.sh \
    && emmake $(MAKE) \
        -f $(firstword $(MAKEFILE_LIST)) \
        -j$(F_MAKE_PARALLEL_JOBS) \
        F_DO_BUILD=1

all :
	bash -c "$(F_MAKE_CMD)"

% :
	bash -c "$(F_MAKE_CMD) $@"

else

F_CONFIG_APP_NAME_SUFFIX := .html
F_CONFIG_BUILD_FLAGS_C_STANDARD := gnu11
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := gnu++11
F_CONFIG_BUILD_OPT := 3
F_CONFIG_FILES_PREFIX := /faur-idbfs/
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL ?= 2
F_CONFIG_LIB_SDL_MIXER_CHUNK_SIZE ?= 2048
F_CONFIG_OUT_STDERR := stdout
F_CONFIG_SCREEN_VSYNC ?= 1
F_CONFIG_SYSTEM_EMSCRIPTEN := 1
F_CONFIG_TRAIT_CUSTOM_MAIN := 1
F_CONFIG_TRAIT_KEYBOARD := 1

ifeq ($(F_CONFIG_LIB_SDL), 1)
    F_CONFIG_LIB_SDL_MIXER_LIMITED_SUPPORT := 1
    F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_ABGR_8888
endif

ifdef F_CONFIG_SYSTEM_EMSCRIPTEN_SHELL
    F_EMSCRIPTEN_SHELL := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_SYSTEM_EMSCRIPTEN_SHELL)
else
    ifdef F_CONFIG_SCREEN_SIZE_WIDTH
        ifeq ($(shell expr $(F_CONFIG_SCREEN_SIZE_WIDTH) \< 0), 1)
            F_EMSCRIPTEN_SHELL := $(F_FAUR_DIR_MEDIA)/shell-fullscreen.html
        endif
    endif

    F_EMSCRIPTEN_SHELL ?= $(F_FAUR_DIR_MEDIA)/shell.html
endif

#
# Flags are in $(F_SDK_EMSCRIPTEN_ROOT)/upstream/emscripten/src/settings.js
#
F_EMSCRIPTEN_OPTIONS := \
    -s USE_SDL=$(F_CONFIG_LIB_SDL) \
    -s USE_SDL_MIXER=$(F_CONFIG_LIB_SDL) \
    -s USE_ZLIB=1 \
    -s USE_LIBPNG=1 \

ifdef F_CONFIG_SYSTEM_EMSCRIPTEN_INITIAL_MEMORY
    F_EMSCRIPTEN_OPTIONS += -s INITIAL_MEMORY=$(F_CONFIG_SYSTEM_EMSCRIPTEN_INITIAL_MEMORY)
endif

F_CONFIG_BUILD_LIBS += \
    -O$(F_CONFIG_BUILD_OPT) \
    $(F_EMSCRIPTEN_OPTIONS) \
    --shell-file $(F_EMSCRIPTEN_SHELL) \
    -lidbfs.js \
    --use-preload-plugins \
    $(foreach f, $(F_CONFIG_FILES_EMBED_PATHS_EMSCRIPTEN), \
        --preload-file $(F_DIR_ROOT_FROM_MAKE)/$(f)@$(f)) \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s WASM=1 \

ifneq ($(F_CONFIG_DEBUG), 0)
    F_CONFIG_BUILD_LIBS += -sASSERTIONS
endif

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    $(F_EMSCRIPTEN_OPTIONS) \

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP_OVERRIDE += \
    -Wno-dollar-in-identifier-extension \
    -Wno-gnu-zero-variadic-macro-arguments \

include $(FAUR_PATH)/make/global/config.mk
include $(FAUR_PATH)/make/global/rules.mk

run : all
	cd $(F_BUILD_DIR_BIN) && $(F_FAUR_DIR_BIN)/faur-build-runweb --file $(F_BUILD_FILE_BIN)

endif
