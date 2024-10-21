include $(FAUR_PATH)/make/global/defs-first.mk
include $(FAUR_PATH)/make/global/defs-sdk.mk

F_CONFIG_DEBUG ?= 1
F_CONFIG_LIB_PNG ?= 1
F_CONFIG_LIB_SDL ?= 2
F_CONFIG_OUT_COLOR_TEXT := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_DESKTOP := 1

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
    -fsanitize=undefined

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    -fpie \
    -fsanitize=undefined

include $(FAUR_PATH)/make/global/defs-config.mk
include $(FAUR_PATH)/make/global/rules.mk

f__target_post : $(F_FAUR_FILE_GEANY_TAGS)

f__target_run :
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" ./$(F_BUILD_FILE_BIN)

valgrind : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind --num-callers=64 ./$(F_BUILD_FILE_BIN)

valgrindall : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind --num-callers=64 --leak-check=full --track-origins=yes ./$(F_BUILD_FILE_BIN)
