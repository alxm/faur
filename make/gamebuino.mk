include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_DEBUG_FATAL := F_DEBUG_FATAL_SPIN
F_CONFIG_FPS_CONSTANT_CREDIT := 1
F_CONFIG_LIB_STDLIB_FILES := 0
F_CONFIG_OUT_CONSOLE_ENABLED := 0
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_RENDER := F_SCREEN_RENDER_SOFTWARE
F_CONFIG_SCREEN_SCREENSHOTS_ENABLED := 0
F_CONFIG_SCREEN_SIZE_HEIGHT_HW := 64
F_CONFIG_SCREEN_SIZE_WIDTH_HW := 80
F_CONFIG_SOUND_FORMAT := F_SOUND_FORMAT_GAMEBUINO_META
F_CONFIG_SOUND_SAMPLE_NEEDS_RUNTIME_OBJECT := 0
F_CONFIG_SYSTEM_ARDUINO := 1
F_CONFIG_SYSTEM_ARDUINO_BOARD := gamebuino:samd:gamebuino_meta_native
F_CONFIG_SYSTEM_ARDUINO_PORT ?= /dev/ttyACM0
F_CONFIG_SYSTEM_GAMEBUINO := 1
F_CONFIG_TRAIT_CUSTOM_MAIN := 1
F_CONFIG_TRAIT_LOW_MEM := 1
F_CONFIG_TRAIT_NO_SEEDING := 1

include $(FAUR_PATH)/make/global/config.mk

#
# Gamebuino_META lib config
#
F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += \
    -DFOLDER_NAME=\"$(F_CONFIG_APP_NAME)\" \
    -DDISPLAY_MODE=DISPLAY_MODE_RGB565 \
    -DSOUND_CHANNELS=1 \
    -DSOUND_FRE=22050 \

ifneq ($(F_CONFIG_SYSTEM_GAMEBUINO_NO_EXIT), 0)
    F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP += -DHOME_MENU_NO_EXIT=1
endif

include $(FAUR_PATH)/make/global/rules.mk
