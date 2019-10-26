#
# make/global/defs.mk is included by every platform Makefile on line 1
#
F_CONFIG_BUILD_PLATFORM := \
    $(basename $(notdir \
        $(word \
            $(shell expr $(words $(MAKEFILE_LIST)) \- 1), \
            $(MAKEFILE_LIST))))

FAUR_DIR_ROOT := $(realpath $(FAUR_PATH))
FAUR_DIR_SRC := $(FAUR_DIR_ROOT)/src

F_DIR_ROOT := ../..
F_DIR_ROOT_FROM_BIN := ../../../..
F_DIR_CONFIG := $(HOME)/.config/faur
F_FILE_SDK := $(F_DIR_CONFIG)/sdk.mk

#
# To support app and author names with spaces
#
F_MAKE_SPACE := $(F_MAKE_SPACE) $(F_MAKE_SPACE)
F_MAKE_SPACE_DASH = $(subst $(F_MAKE_SPACE),-,$1)
F_MAKE_SPACE_ESCAPE = $(subst $(F_MAKE_SPACE),\$(F_MAKE_SPACE),$1)

A_MAKE_PARALLEL_JOBS := 8

#
# Custom SDK paths
#
ifneq ($(wildcard $(F_FILE_SDK)),)
include $(F_FILE_SDK)
endif

F_SDK_ARDUINO_DIR ?= /opt/arduino
F_SDK_ARDUINO_MAKEFILE ?= /opt/Arduino-Makefile

F_SDK_CAANOO_ROOT ?= /opt/gph_sdk
F_SDK_CAANOO_TOOLCHAIN ?= tools/gcc-4.2.4-glibc-2.7-eabi

F_SDK_EMSCRIPTEN_ROOT ?= /opt/emsdk

F_SDK_MINGW32_ROOT ?= /usr
F_SDK_MINGW32_ARCH ?= i686-w64-mingw32

F_SDK_MINGW64_ROOT ?= /usr
F_SDK_MINGW64_ARCH ?= x86_64-w64-mingw32

F_SDK_OPEN2X_ROOT ?= /opt/open2x
F_SDK_OPEN2X_TOOLCHAIN ?= gcc-4.1.1-glibc-2.3.6

F_SDK_PANDORA_ROOT ?= /opt/pandora
F_SDK_PANDORA_TOOLCHAIN ?= arm-2013.11
F_SDK_PANDORA_UTILS ?= sdk_utils
