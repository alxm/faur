FAUR_DIR_ROOT := $(realpath $(FAUR_PATH))
FAUR_DIR_SRC := $(FAUR_DIR_ROOT)/src

#
# make/global/defs is included by every platform Makefile on line 1
#
A_CONFIG_BUILD_PLATFORM := \
    $(basename $(notdir \
        $(word \
            $(shell expr $(words $(MAKEFILE_LIST)) \- 1), \
            $(MAKEFILE_LIST))))

A_DIR_ROOT := ../..
A_DIR_ROOT_FROM_BIN := ../../../..
A_DIR_CONFIG := $(HOME)/.config/faur

#
# To support app and author names with spaces
#
A_MAKE_SPACE := $(A_MAKE_SPACE) $(A_MAKE_SPACE)
A_MAKE_SPACE_DASH = $(subst $(A_MAKE_SPACE),-,$1)
A_MAKE_SPACE_ESCAPE = $(subst $(A_MAKE_SPACE),\$(A_MAKE_SPACE),$1)

#
# Custom SDK paths
#
-include $(A_DIR_CONFIG)/sdk.mk

A_SDK_ARDUINO_DIR ?= /opt/arduino
A_SDK_ARDUINO_MAKEFILE ?= /opt/Arduino-Makefile

A_SDK_CAANOO_ROOT ?= /opt/gph_sdk
A_SDK_CAANOO_TOOLCHAIN ?= tools/gcc-4.2.4-glibc-2.7-eabi

A_SDK_EMSCRIPTEN_ROOT ?= /opt/emsdk

A_SDK_MINGW32_ROOT ?= /usr
A_SDK_MINGW32_ARCH ?= i686-w64-mingw32

A_SDK_MINGW64_ROOT ?= /usr
A_SDK_MINGW64_ARCH ?= x86_64-w64-mingw32

A_SDK_OPEN2X_ROOT ?= /opt/open2x
A_SDK_OPEN2X_TOOLCHAIN ?= gcc-4.1.1-glibc-2.3.6

A_SDK_PANDORA_ROOT ?= /opt/pandora
A_SDK_PANDORA_TOOLCHAIN ?= arm-2013.11
A_SDK_PANDORA_UTILS ?= sdk_utils
