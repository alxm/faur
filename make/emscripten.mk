include $(FAUR_PATH)/make/global/defs-first.mk
include $(FAUR_PATH)/make/global/defs-sdk.mk

#
# emsdk_env needs Bash
#
SHELL := /bin/bash

F_MAKE_COMMAND_BUILD := \
    source $(F_SDK_EMSCRIPTEN_ROOT)/emsdk_env.sh \
    && emmake $(MAKE) \
        --file=$(firstword $(MAKEFILE_LIST)) \
        --jobs=$(F_MAKE_PARALLEL_JOBS) \
        --no-builtin-rules \
        --warn-undefined-variables \
        --keep-going

F_CONFIG_APP_NAME_SUFFIX := .html
F_CONFIG_BUILD_FLAGS_C_STANDARD := gnu23
F_CONFIG_BUILD_FLAGS_CPP_STANDARD := gnu++23
F_CONFIG_BUILD_OPT := 3
F_CONFIG_FILES_STORAGE_PREFIX := /faur-$(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME))-idbfs/
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
    --use-port=zlib \
    --use-port=libpng

ifdef F_CONFIG_SYSTEM_EMSCRIPTEN_INITIAL_MEMORY
    F_EMSCRIPTEN_OPTIONS += \
        -s INITIAL_MEMORY=$(F_CONFIG_SYSTEM_EMSCRIPTEN_INITIAL_MEMORY)
endif

ifeq ($(F_CONFIG_LIB_SDL), 1)
    F_EMSCRIPTEN_OPTIONS += \
        -s USE_SDL=1 \
        -s USE_SDL_MIXER=1
else ifeq ($(F_CONFIG_LIB_SDL), 2)
    F_EMSCRIPTEN_OPTIONS += \
        --use-port=sdl2 \
        --use-port=sdl2_mixer
endif

include $(FAUR_PATH)/make/global/defs-config.mk

F_BUILD_FILES_EMBED_EMSCRIPTEN_REL := \
    $(shell find $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_MEDIA) \
        -type f -a \
        \( -name "*$(firstword $(F_CONFIG_FILES_EMBED_EXTS))" \
        $(foreach ext, $(wordlist 2, $(words $(F_CONFIG_FILES_EMBED_EXTS)), \
            $(F_CONFIG_FILES_EMBED_EXTS)), -o -name "*$(ext)") \) )
F_BUILD_FILES_EMBED_EMSCRIPTEN_ABS := $(F_BUILD_FILES_EMBED_EMSCRIPTEN_REL:$(F_DIR_ROOT_FROM_MAKE)/%=%)

F_CONFIG_BUILD_LIBS += \
    -O$(F_CONFIG_BUILD_OPT) \
    $(F_EMSCRIPTEN_OPTIONS) \
    --shell-file $(F_EMSCRIPTEN_SHELL) \
    -lidbfs.js \
    --use-preload-plugins \
    $(foreach f, $(F_BUILD_FILES_EMBED_EMSCRIPTEN_ABS), \
        --preload-file $(F_DIR_ROOT_FROM_MAKE)/$(f)@$(f)) \
    -s FORCE_FILESYSTEM \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s WASM=1

ifdef F_CONFIG_DEBUG
    ifneq ($(F_CONFIG_DEBUG), 0)
        F_CONFIG_BUILD_LIBS += -sASSERTIONS
    endif
endif

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    $(F_EMSCRIPTEN_OPTIONS)

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP_OVERRIDE += \
    -Wno-dollar-in-identifier-extension

include $(FAUR_PATH)/make/global/rules.mk

f__target_run :
	cd $(F_BUILD_DIR_BIN) && $(F_FAUR_DIR_BIN)/faur-build-runweb --file $(F_BUILD_FILE_BIN)
