#
# Dirs and files for arduino-builder
#
F_BUILD_DIR_ARDUINO_BUILD := $(F_BUILD_DIR)/arduino-build
F_BUILD_DIR_ARDUINO_CACHE := $(F_BUILD_DIR)/arduino-cache

F_BUILD_FILE_INO := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/$(F_CONFIG_DIR_SRC).ino
F_BUILD_FILE_INO_BIN := $(F_BUILD_DIR_ARDUINO_BUILD)/$(F_CONFIG_DIR_SRC).ino.bin

#
# All code files as build prerequisites
#
F_BUILD_FILES_ALL_SOURCES := \
    $(F_BUILD_FILES_GEN_C) \
    $(F_BUILD_FILES_GEN_H) \
    $(shell find $(F_FAUR_DIR_SRC) -type f) \
    $(shell find $(F_BUILD_DIR_SRC) -type f -not -path "$(F_BUILD_DIR_GEN_ROOT)/*") \

#
# Internal targets
#
f__target_setup : $(F_BUILD_DIR_ARDUINO_BUILD) $(F_BUILD_DIR_ARDUINO_CACHE)

f__target_gen : $(F_BUILD_FILE_INO)

ifeq ($(F_CONFIG_SYSTEM_ODROID_GO), 0)
f__target_run :
	$(F_SDK_ARDUINO_DIR_INSTALL)/arduino \
		--upload \
		--verbose \
		--verbose-upload \
		--board $(F_CONFIG_SYSTEM_ARDUINO_BOARD) \
		--port $(F_CONFIG_SYSTEM_ARDUINO_PORT) \
		--pref sketchbook.path=$(F_SDK_ARDUINO_DIR_SKETCHBOOK) \
		--pref build.path=$(F_BUILD_DIR_ARDUINO_BUILD) \
		--pref build.cache=$(F_BUILD_DIR_ARDUINO_CACHE) \
		--pref compiler.c.extra_flags="$(F_BUILD_FLAGS_SHARED_C_AND_CPP)" \
		--pref compiler.cpp.extra_flags="$(F_BUILD_FLAGS_SHARED_C_AND_CPP)" \
		--preserve-temp-files \
		$(F_BUILD_FILE_INO)
endif

#
# Dirs and files for arduino-builder
#
$(F_BUILD_FILE_INO) :
	echo "// For arduino-builder" > $@

$(F_BUILD_DIR_ARDUINO_BUILD) $(F_BUILD_DIR_ARDUINO_CACHE):
	@ mkdir -p $@

#
# Main app
#
$(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) : $(F_BUILD_FILE_INO_BIN)
	@ mkdir -p $(@D)
	cp $< $@

$(F_BUILD_FILE_INO_BIN) : $(F_BUILD_FILES_ALL_SOURCES)
	$(F_SDK_ARDUINO_DIR_INSTALL)/arduino-builder \
		-compile \
		-verbose \
		-warnings=all \
		-jobs $(F_MAKE_PARALLEL_JOBS) \
		-fqbn=$(F_CONFIG_SYSTEM_ARDUINO_BOARD) \
		-hardware $(F_SDK_ARDUINO_DIR_INSTALL)/hardware \
		-hardware $(F_SDK_ARDUINO_DIR_15)/packages \
		-tools $(F_SDK_ARDUINO_DIR_INSTALL)/tools-builder \
		-tools $(F_SDK_ARDUINO_DIR_15)/packages \
		-built-in-libraries $(F_SDK_ARDUINO_DIR_INSTALL)/libraries \
		-libraries $(F_SDK_ARDUINO_DIR_SKETCHBOOK)/libraries \
		-libraries $(F_FAUR_DIR_ROOT)/.. \
		-build-path $(F_BUILD_DIR_ARDUINO_BUILD) \
		-build-cache $(F_BUILD_DIR_ARDUINO_CACHE) \
		-prefs=build.warn_data_percentage=75 \
		-prefs=compiler.c.extra_flags="$(F_BUILD_FLAGS_SHARED_C_AND_CPP)" \
		-prefs=compiler.cpp.extra_flags="$(F_BUILD_FLAGS_SHARED_C_AND_CPP)" \
		$(F_BUILD_FILE_INO)
