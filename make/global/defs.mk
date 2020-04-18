#
# make/global/defs.mk is included by every platform Makefile on line 1
#
F_BUILD_PLATFORM := \
    $(basename $(notdir \
        $(word \
            $(shell expr $(words $(MAKEFILE_LIST)) \- 1), \
            $(MAKEFILE_LIST))))

#
# Used when calling make recursively
#
F_MAKE_PARALLEL_JOBS := 8

#
# To support app and author names with spaces
#
F_MAKE_SPACE := $(F_MAKE_SPACE) $(F_MAKE_SPACE)
F_MAKE_SPACE_DASH = $(subst $(F_MAKE_SPACE),-,$1)
F_MAKE_SPACE_ESCAPE = $(subst $(F_MAKE_SPACE),\$(F_MAKE_SPACE),$1)

#
# Where the framework lives
#
F_FAUR_DIR_ROOT := $(realpath $(FAUR_PATH))
F_FAUR_DIR_BIN := $(F_FAUR_DIR_ROOT)/bin
F_FAUR_DIR_MEDIA := $(F_FAUR_DIR_ROOT)/media
F_FAUR_DIR_SRC := $(F_FAUR_DIR_ROOT)/src
F_FAUR_DIR_SRC_GEN := $(F_FAUR_DIR_SRC)/generated
F_FAUR_DIR_CONFIG := $(HOME)/.config/faur

#
# From <project>/build/make/ to <project> and
# from <project>/build/builds/build_uid/bin/ to <project>
#
F_DIR_ROOT_FROM_MAKE := ../..
F_DIR_ROOT_FROM_BIN := ../../../..

#
# May already have been set by parent make before changing dir
#
F_DIR_ROOT ?= $(realpath $(F_DIR_ROOT_FROM_MAKE))

#
# Custom SDK paths and Geany C tags
#
F_FAUR_FILE_SDK_MK := $(F_FAUR_DIR_CONFIG)/sdk.mk
F_FAUR_FILE_GEANY_TAGS := $(HOME)/.config/geany/tags/faur.c.tags

-include $(F_FAUR_FILE_SDK_MK)

F_SDK_ARDUINO_ROOT ?= /opt/arduino
F_SDK_ARDUINO_MAKEFILE ?= /opt/Arduino-Makefile

F_SDK_CAANOO_ROOT ?= /opt/gph_sdk
F_SDK_CAANOO_TOOLCHAIN ?= /opt/gph_sdk/tools/gcc-4.2.4-glibc-2.7-eabi

F_SDK_EMSCRIPTEN_ROOT ?= /opt/emsdk

F_SDK_MINGW32_ROOT ?= /usr
F_SDK_MINGW64_ROOT ?= /usr

F_SDK_OPEN2X_TOOLCHAIN ?= /opt/open2x/gcc-4.1.1-glibc-2.3.6

F_SDK_PANDORA_TOOLCHAIN ?= /opt/pandora/arm-2013.11
F_SDK_PANDORA_UTILS ?= /opt/pandora/sdk_utils
