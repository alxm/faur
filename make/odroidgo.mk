include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_GEN_CODE := 0
F_CONFIG_DEBUG_CONSOLE := 0
F_CONFIG_DEBUG_FATAL_SPIN := 1
F_CONFIG_FILES_EMBED_ENABLED := 0
F_CONFIG_FILES_PREFIX := "/"
F_CONFIG_FILES_STANDARD := 0
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_HARDWARE_WIDTH := 320
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
F_CONFIG_SCREEN_SIZE_WIDTH := 80
F_CONFIG_SCREEN_SIZE_HEIGHT := 64
F_CONFIG_SCREEN_ZOOM := 3
F_CONFIG_SOUND_ENABLED := 0
F_CONFIG_SYSTEM_ARDUINO := 1
F_CONFIG_SYSTEM_ODROID_GO := 1
F_CONFIG_TRAIT_CUSTOM_MAIN := 1

include $(FAUR_PATH)/make/global/config.mk

F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)
F_BUILD_DIR_ARDUINO_BUILD := $(F_BUILD_DIR)/arduino-build
F_BUILD_DIR_ARDUINO_CACHE := $(F_BUILD_DIR)/arduino-cache

F_ARDUINO_BUILDER := \
    $(F_SDK_ARDUINO_DIR_INSTALL)/arduino-builder \
        -compile \
        -verbose \
        -warnings=all \
        -jobs $(F_MAKE_PARALLEL_JOBS) \
        -fqbn=$(F_SDK_ARDUINO_ODROID_GO_BOARD) \
        -hardware $(F_SDK_ARDUINO_DIR_INSTALL)/hardware \
        -hardware $(F_SDK_ARDUINO_DIR_15)/packages \
        -tools $(F_SDK_ARDUINO_DIR_INSTALL)/tools-builder \
        -tools $(F_SDK_ARDUINO_DIR_15)/packages \
        -built-in-libraries $(F_SDK_ARDUINO_DIR_INSTALL)/libraries \
        -libraries $(F_SDK_ARDUINO_DIR_SKETCHBOOK)/libraries \
        -build-path $(F_BUILD_DIR_ARDUINO_BUILD) \
        -build-cache $(F_BUILD_DIR_ARDUINO_CACHE) \
        -prefs=build.warn_data_percentage=75 \
        -prefs=compiler.c.extra_flags="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" \
        -prefs=compiler.cpp.extra_flags="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" \
        $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/$(F_CONFIG_DIR_SRC).ino

F_ARDUINO_UPLOAD := \
    $(F_SDK_ARDUINO_DIR_INSTALL)/arduino \
        --upload \
	--verbose \
	--verbose-upload \
        --board $(F_SDK_ARDUINO_ODROID_GO_BOARD) \
        --port $(F_SDK_ARDUINO_ODROID_GO_PORT) \
        --pref build.path=$(F_BUILD_DIR_ARDUINO_BUILD) \
        --pref build.cache=$(F_BUILD_DIR_ARDUINO_CACHE) \
        $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/$(F_CONFIG_DIR_SRC).ino

all : dirs
	$(F_ARDUINO_BUILDER)

run : dirs
	$(F_ARDUINO_UPLOAD)

clean :
	rm -rf $(F_BUILD_DIR)

dirs :
	@ mkdir -p $(F_BUILD_DIR_ARDUINO_BUILD)
	@ mkdir -p $(F_BUILD_DIR_ARDUINO_CACHE)
