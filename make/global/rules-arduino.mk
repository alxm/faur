#
# Unique build path
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)
F_BUILD_DIR_ARDUINO_BUILD := $(F_BUILD_DIR)/arduino-build
F_BUILD_DIR_ARDUINO_CACHE := $(F_BUILD_DIR)/arduino-cache

F_BUILD_FILE_INO := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/$(F_CONFIG_DIR_SRC).ino

F_ARDUINO_BUILDER := \
    $(F_SDK_ARDUINO_DIR_INSTALL)/arduino-builder \
        -compile \
        -verbose \
        -warnings=all \
        -jobs $(F_MAKE_PARALLEL_JOBS) \
        -fqbn=$(F_CONFIG_BUILD_ARDUINO_BOARD) \
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
        $(F_BUILD_FILE_INO)

F_ARDUINO_UPLOAD := \
    $(F_SDK_ARDUINO_DIR_INSTALL)/arduino \
        --upload \
        --verbose \
        --verbose-upload \
        --board $(F_CONFIG_BUILD_ARDUINO_BOARD) \
        --port $(F_CONFIG_BUILD_ARDUINO_PORT) \
        --pref build.path=$(F_BUILD_DIR_ARDUINO_BUILD) \
        --pref build.cache=$(F_BUILD_DIR_ARDUINO_CACHE) \
        --pref compiler.c.extra_flags="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" \
        --pref compiler.cpp.extra_flags="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" \
        $(F_BUILD_FILE_INO)

F_MAKE_ALL := dirs $(F_BUILD_FILE_INO)

all : $(F_MAKE_ALL)
	$(F_ARDUINO_BUILDER)

run : $(F_MAKE_ALL)
	$(F_ARDUINO_UPLOAD)

clean :
	rm -rf $(F_BUILD_DIR)

dirs :
	@ mkdir -p $(F_BUILD_DIR_ARDUINO_BUILD)
	@ mkdir -p $(F_BUILD_DIR_ARDUINO_CACHE)

$(F_BUILD_FILE_INO) :
	echo "//" > $@
