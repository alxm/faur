include $(FAUR_PATH)/make/global/defs.mk

ifndef F_DO_BUILD

F_MAKE_CMD := \
    source $(F_SDK_EMSCRIPTEN_ROOT)/emsdk_env.sh \
    && emmake $(MAKE) \
        -f $(firstword $(MAKEFILE_LIST)) \
        -j$(A_MAKE_PARALLEL_JOBS) \
        F_DO_BUILD=1

all :
	bash -c "$(F_MAKE_CMD)"

% :
	bash -c "$(F_MAKE_CMD) $@"

else

F_CONFIG_BUILD_C_STANDARD := gnu11
F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL ?= 2
F_CONFIG_LIB_SDL_TIME := 1
F_CONFIG_PATH_STORAGE_PREFIX := /faur-idbfs/
F_CONFIG_SCREEN_BPP := 32
F_CONFIG_SCREEN_VSYNC ?= 1
F_CONFIG_SYSTEM_EMSCRIPTEN := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_KEYBOARD := 1
F_CONFIG_TRAIT_NOSLEEP := 1

ifeq ($(F_CONFIG_LIB_SDL), 1)
    F_CONFIG_SCREEN_FORMAT ?= ABGR
else ifeq ($(F_CONFIG_LIB_SDL), 2)
    F_CONFIG_SCREEN_FORMAT ?= RGBA
endif

F_BUILD_EMSCRIPTEN_LIBS := \
    -s USE_SDL=$(F_CONFIG_LIB_SDL) \
    -s USE_SDL_MIXER=$(F_CONFIG_LIB_SDL) \
    -s USE_ZLIB=1 \
    -s USE_LIBPNG=1 \

ifdef F_CONFIG_SYSTEM_EMSCRIPTEN_TOTAL_MEMORY
    F_BUILD_EMSCRIPTEN_LIBS += -s TOTAL_MEMORY=$(F_CONFIG_SYSTEM_EMSCRIPTEN_TOTAL_MEMORY)
endif

F_BUILD_EMSCRIPTEN_EMBED := \
    --use-preload-plugins \
    $(addprefix --preload-file $(F_DIR_ROOT)/, \
        $(join $(F_CONFIG_PATH_EMBED_EMSCRIPTEN), \
               $(addprefix @, $(F_CONFIG_PATH_EMBED_EMSCRIPTEN)))) \

F_CONFIG_APP_NAME_SUFFIX := .html

F_CONFIG_BUILD_LIBS += \
    -O$(F_CONFIG_BUILD_OPT) \
    $(F_BUILD_EMSCRIPTEN_LIBS) \
    $(F_BUILD_EMSCRIPTEN_EMBED) \

F_CONFIG_BUILD_CFLAGS += \
    $(F_BUILD_EMSCRIPTEN_LIBS) \
    -Wno-dollar-in-identifier-extension \
    -Wno-gnu-zero-variadic-macro-arguments \

include $(FAUR_PATH)/make/global/rules.mk

run :
	cd $(F_DIR_BIN) && $(FAUR_PATH)/bin/faur-runweb $(F_FILE_BIN)

endif
