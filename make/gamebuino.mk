include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_DEBUG_FATAL_SPIN := 1
F_CONFIG_BUILD_MAIN := 0
F_CONFIG_BUILD_OPT := s
F_CONFIG_CONSOLE_ENABLED := 0
F_CONFIG_FILES_STANDARD := 0
F_CONFIG_FPS_CONSTANT_CREDIT := 1
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_HARDWARE_WIDTH := 80
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 64
F_CONFIG_SOUND_ENABLED_MUSIC := 0
F_CONFIG_SOUND_VOLUME_ADJUSTABLE := 0
F_CONFIG_SYSTEM_ARDUINO := 1
F_CONFIG_SYSTEM_GAMEBUINO := 1
F_CONFIG_TRAIT_LOW_MEM := 1

include $(FAUR_PATH)/make/global/config.mk

ifndef F_DO_BUILD

F_MAKE_CMD := \
    $(MAKE) \
	-C $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC) \
	-f $(realpath $(firstword $(MAKEFILE_LIST))) \
	F_DIR_ROOT=$(F_DIR_ROOT) \
	F_DO_BUILD=1

all :
	$(F_MAKE_CMD) -j$(F_MAKE_PARALLEL_JOBS)

upload : all
	$(F_MAKE_CMD) reset do_sam_upload

% :
	$(F_MAKE_CMD) $@

else

ARDUINO_DIR = $(F_SDK_ARDUINO_ROOT)
ARDMK_DIR = $(F_SDK_ARDUINO_MAKEFILE)

BOARD_TAG = gamebuino_meta_native
ALTERNATE_CORE_PATH = $(wildcard $(HOME)/.arduino15/packages/gamebuino/hardware/samd/1.*)

ARDUINO_LIBS = faur Gamebuino_META SPI
OPTIMIZATION_LEVEL = $(F_CONFIG_BUILD_OPT)
OBJDIR=$(F_DIR_ROOT)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

include $(ARDMK_DIR)/Sam.mk

#
# Gamebuino_META needs <objdir>/sketch/config-gamebuino.h
#
F_GAMEBUINO_DIR_GEN := $(OBJDIR)/userlibs/faur/faur_gen
F_GAMEBUINO_CONFIG_SRC := $(F_DIR_ROOT)/$(F_CONFIG_DIR_SRC)/config-gamebuino.h
F_GAMEBUINO_CONFIG_DST := $(F_GAMEBUINO_DIR_GEN)/sketch/config-gamebuino.h

#
# Arduino IDE usually defines __SKETCH_NAME__, which Gamebuino_META lib uses
#
F_CONFIG_BUILD_FLAGS_SHARED += \
    -I$(F_GAMEBUINO_DIR_GEN) \
    -D__SKETCH_NAME__=\"$(TARGET).ino\" \

CPPFLAGS := $(F_CONFIG_BUILD_FLAGS_SHARED) $(CPPFLAGS)

#
# Libraries don't build well with Link Time Optimization
#
CFLAGS += -fno-lto
CXXFLAGS += -fno-lto
LDFLAGS += -fno-lto

$(USER_LIB_OBJS) $(LOCAL_OBJS) : $(F_GAMEBUINO_CONFIG_DST)

$(F_GAMEBUINO_CONFIG_DST) : $(F_GAMEBUINO_CONFIG_SRC)
	@ mkdir -p $(@D)
	cp $< $@

endif
