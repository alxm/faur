include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_DEBUG_FATAL_SPIN := 1
A_CONFIG_BUILD_MAIN := 0
A_CONFIG_BUILD_OPT := 3
A_CONFIG_BUILD_FIX_LUT := 1
A_CONFIG_FPS_HISTORY := 1
A_CONFIG_OUTPUT_ENABLED := 0
A_CONFIG_SCREEN_HARDWARE_WIDTH := 80
A_CONFIG_SCREEN_HARDWARE_HEIGHT := 64
A_CONFIG_SOUND_ENABLED := 0
A_CONFIG_SYSTEM_GAMEBUINO := 1

include $(A2X_PATH)/make/global/config.mk

ifndef A_DO_BUILD

A_MAKE_CMD := $(MAKE) \
                -C $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC) \
                -f $(realpath $(firstword $(MAKEFILE_LIST))) \
                -j8 \
                OBJDIR=$(realpath $(A_DIR_ROOT))/$(A_CONFIG_DIR_BUILD)/builds/$(A_CONFIG_BUILD_UID) \
                A_DIR_SOURCE=$(realpath $(A_DIR_ROOT))/$(A_CONFIG_DIR_SRC) \
                A_DO_BUILD=1

all :
	bash -c "$(A_MAKE_CMD)"

% :
	bash -c "$(A_MAKE_CMD) $@"

else

ARDUINO_DIR = $(A_SDK_ARDUINO_DIR)
ARDMK_DIR = $(A_SDK_ARDUINO_MAKEFILE)

BOARD_TAG = gamebuino_meta_native
ALTERNATE_CORE_PATH = $(wildcard $(HOME)/.arduino15/packages/gamebuino/hardware/samd/1.*)

ARDUINO_LIBS = faur Gamebuino_META SPI

OPTIMIZATION_LEVEL = $(A_CONFIG_BUILD_OPT)

include $(ARDMK_DIR)/Sam.mk

A_DIR_GEN := $(OBJDIR)/userlibs/a2x/a2x_gen

# Gamebuino_META needs __SKETCH_NAME__ (usually defined by Arduino IDE)
A_CONFIG_BUILD_CFLAGS += \
    -I$(A_DIR_GEN) \
    -D__SKETCH_NAME__=\"$(TARGET).ino\" \

CPPFLAGS := $(A_CONFIG_BUILD_CFLAGS) $(CPPFLAGS)

# Libraries don't build well with Link Time Optimization
CFLAGS += -fno-lto
CXXFLAGS += -fno-lto
LDFLAGS += -fno-lto

A_FILES_CONFIG := \
    $(A_DIR_GEN)/sketch/config-faur.h \
    $(A_DIR_GEN)/sketch/config-gamebuino.h \

$(USER_LIB_OBJS) $(LOCAL_OBJS) : $(A_FILES_CONFIG)

$(A_DIR_GEN)/sketch/%.h : $(A_DIR_SOURCE)/%.h
	@ mkdir -p $(@D)
	cp $< $@

endif
