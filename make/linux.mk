include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_OPT := 3
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 2
F_CONFIG_LIB_SDL_CONFIG := sdl2-config
F_CONFIG_OUT_COLOR_TEXT := 1
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_TRAIT_DESKTOP := 1

F_CONFIG_BUILD_LIBS += \
    -lpng \
    -lm \
    -pie \

F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    -fpie \

include $(FAUR_PATH)/make/global/config.mk
include $(FAUR_PATH)/make/global/rules.mk

f__target_run :
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" ./$(F_BUILD_FILE_BIN)

valgrind : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind --num-callers=64 ./$(F_BUILD_FILE_BIN)

valgrindall : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind --num-callers=64 --leak-check=full --track-origins=yes ./$(F_BUILD_FILE_BIN)
