include $(FAUR_PATH)/make/global/defs.mk

F_CONFIG_BUILD_FLAGS_AR := T
F_CONFIG_BUILD_OPT := 3
F_CONFIG_INPUT_MOUSE_CURSOR := 0
F_CONFIG_LIB_PNG := 1
F_CONFIG_LIB_SDL := 1
F_CONFIG_SCREEN_FORMAT := F_COLOR_FORMAT_RGB_565
F_CONFIG_SCREEN_FULLSCREEN := 1
F_CONFIG_SCREEN_HARDWARE_WIDTH := 400
F_CONFIG_SCREEN_HARDWARE_HEIGHT := 240
F_CONFIG_SYSTEM_LINUX := 1
F_CONFIG_SYSTEM_PANDORA := 1
F_CONFIG_TRAIT_KEYBOARD := 1

PANDORA_SDK := $(F_SDK_PANDORA_ROOT)/$(F_SDK_PANDORA_TOOLCHAIN)
PANDORA_UTILS := $(F_SDK_PANDORA_ROOT)/$(F_SDK_PANDORA_UTILS)

F_CONFIG_BUILD_LIBS += \
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

F_CONFIG_BUILD_FLAGS_SHARED += \
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

include $(FAUR_PATH)/make/global/rules.mk

F_DIR_PND_BASE := $(F_DIR_ROOT)/$(F_CONFIG_DIR_BUILD)/static/pnd
F_DIR_PND_STAGING := $(F_DIR_BUILD_UID)/pnd
F_FILE_PND := $(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_AUTHOR)).$(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME)).pnd
F_FILE_PND_TARGET := $(PWD)/$(F_DIR_BIN)/$(F_FILE_PND)

all : $(F_FILE_PND_TARGET)

$(F_FILE_PND_TARGET) : $(F_FILE_BIN_TARGET)
	@ mkdir -p $(@D) $(F_DIR_PND_STAGING)
	rsync --archive --delete --progress --human-readable $(F_DIR_PND_BASE)/ $(F_DIR_PND_STAGING)
	rsync --archive --delete --progress --human-readable $(F_DIR_ROOT)/$(F_CONFIG_DIR_ASSETS) $(F_DIR_PND_STAGING)
	cp $(F_FILE_BIN_TARGET) $(F_DIR_PND_STAGING)
	$(PANDORA_UTILS)/pnd_make.sh -c \
		-d $(PWD)/$(F_DIR_PND_STAGING) \
		-p $(F_FILE_PND_TARGET) \
		-x $(PWD)/$(F_DIR_PND_STAGING)/doc/PXML.xml \
		-i $(PWD)/$(F_DIR_PND_STAGING)/img/icon.png
