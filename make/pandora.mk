include $(A2X_PATH)/make/global/defs.mk

A_CONFIG_BUILD_AR_FLAGS := T
A_CONFIG_BUILD_OPT := -O3 -s
A_CONFIG_INPUT_MOUSE_CURSOR := 0
A_CONFIG_LIB_PNG := 1
A_CONFIG_LIB_SDL := 1
A_CONFIG_LIB_SDL_TIME := 1
A_CONFIG_SCREEN_FULLSCREEN := 1
A_CONFIG_SCREEN_HARDWARE_WIDTH := 400
A_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
A_CONFIG_SYSTEM_LINUX := 1
A_CONFIG_SYSTEM_PANDORA := 1
A_CONFIG_TRAIT_KEYBOARD := 1

PANDORA_SDK := $(A_SDK_PANDORA_ROOT)/$(A_SDK_PANDORA_TOOLCHAIN)
PANDORA_UTILS := $(A_SDK_PANDORA_ROOT)/$(A_SDK_PANDORA_UTILS)

A_CONFIG_BUILD_LIBS += \
    -L$(PANDORA_SDK)/lib \
    -L$(PANDORA_SDK)/usr/lib \
    -L$(PANDORA_SDK)/arm-none-linux-gnueabi/lib \
    -lpng12 \
    -lz \
    -lSDL_mixer \
    -lmad \
    -lSDL \
    -lts \
    -lpthread \
    -lm \
    -ldl \

A_CONFIG_BUILD_CFLAGS += \
    -march=armv7-a \
    -mcpu=cortex-a8 \
    -mtune=cortex-a8 \
    -mfpu=neon \
    -mfloat-abi=softfp \
    -ffast-math \
    -fsingle-precision-constant \
    -ftree-vectorize \
    -fomit-frame-pointer \
    -isystem$(PANDORA_SDK)/include \
    -isystem$(PANDORA_SDK)/usr/include \
    -isystem$(PANDORA_SDK)/arm-none-linux-gnueabi/include \

PREFIX := arm-none-linux-gnueabi-

export PATH    := $(PANDORA_SDK)/bin:$(PATH)
export CC      := $(PREFIX)gcc
export CXX     := $(PREFIX)g++
export AS      := $(PREFIX)as
export AR      := $(PREFIX)ar
export OBJCOPY := $(PREFIX)objcopy
export READELF := $(PREFIX)readelf
export STRIP   := $(PREFIX)strip
export LD      := $(PREFIX)ld

include $(A2X_PATH)/make/global/rules.mk

A_DIR_PND_BASE := $(A_DIR_BUILD_STATIC)/pnd
A_DIR_PND_STAGING := $(A_DIR_BUILD_UID)/pnd
A_FILE_PND := $(call A_MAKE_SPACE_DASH,$(A_CONFIG_APP_AUTHOR)).$(call A_MAKE_SPACE_DASH,$(A_CONFIG_APP_NAME)).pnd
A_FILE_PND_TARGET := $(PWD)/$(A_DIR_BIN)/$(A_FILE_PND)

all : $(A_FILE_PND_TARGET)

$(A_FILE_PND_TARGET) : $(A_FILE_BIN_TARGET)
	@ mkdir -p $(@D) $(A_DIR_PND_STAGING)
	rsync --archive --delete --progress --human-readable $(A_DIR_PND_BASE)/ $(A_DIR_PND_STAGING)
	rsync --archive --delete --progress --human-readable $(A_DIR_ROOT)/$(A_CONFIG_DIR_ASSETS) $(A_DIR_PND_STAGING)
	cp $(A_FILE_BIN_TARGET) $(A_DIR_PND_STAGING)
	$(PANDORA_UTILS)/pnd_make.sh -c \
		-d $(PWD)/$(A_DIR_PND_STAGING) \
		-p $(A_FILE_PND_TARGET) \
		-x $(PWD)/$(A_DIR_PND_STAGING)/doc/PXML.xml \
		-i $(PWD)/$(A_DIR_PND_STAGING)/img/icon.png
