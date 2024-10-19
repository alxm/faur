#
# Source code dirs
#
F_BUILD_DIR_SRC := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)
F_BUILD_DIR_GEN_ROOT := $(F_BUILD_DIR_SRC)/faur_gen
F_BUILD_DIR_GEN_UID := $(F_BUILD_DIR_GEN_ROOT)/$(F_CONFIG_BUILD_UID)
F_BUILD_DIR_GEN := $(F_BUILD_DIR_GEN_UID)/faur_v
F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN)/embed
F_BUILD_DIR_GEN_FAUR_MEDIA := $(F_BUILD_DIR_GEN)/faur_gfx
F_BUILD_DIR_GEN_BIN := $(F_BUILD_DIR_GEN)/bin
F_BUILD_DIR_GEN_GFX := $(F_BUILD_DIR_GEN)/gfx
F_BUILD_DIR_GEN_SFX := $(F_BUILD_DIR_GEN)/sfx

F_BUILD_FLAGS_SHARED_C_AND_CPP := \
    $(F_CONFIG_BUILD_FLAGS_SETTINGS) \
    $(F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP) \
    -I$(F_BUILD_DIR_GEN_UID)

#
# ECS object declarations
#
F_BUILD_FILES_ECS_INIT_C := $(F_BUILD_DIR_GEN)/g_ecs_init.c
F_BUILD_FILES_ECS_INIT_H := $(F_BUILD_DIR_GEN)/g_ecs_init.h

#
# Embedded files and directories
#
F_BUILD_FILES_EMBED_INIT_C := $(F_BUILD_DIR_GEN)/g_embed_init.c

ifneq ($(F_CONFIG_FILES_EMBED_C)$(F_CONFIG_FILES_EMBED_BLOB), 00)
    F_BUILD_FILES_EMBED_FS_BIN_REL := \
        $(shell find $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_MEDIA) \
            \( -type d \) -o \( -type f -a \
            \( -name "*$(firstword $(F_CONFIG_FILES_EMBED_EXTS))" \
            $(foreach ext, $(wordlist 2, $(words $(F_CONFIG_FILES_EMBED_EXTS)), \
                $(F_CONFIG_FILES_EMBED_EXTS)), -o -name "*$(ext)") \) \) )
    F_BUILD_FILES_EMBED_FS_BIN_ABS := $(F_BUILD_FILES_EMBED_FS_BIN_REL:$(F_DIR_ROOT_FROM_MAKE)/%=%)

    F_BUILD_FILES_EMBED_FS_H_REL := $(F_BUILD_FILES_EMBED_FS_BIN_ABS:%=$(F_BUILD_DIR_GEN_EMBED)/%.h)
    F_BUILD_FILES_EMBED_FS_H_ABS := $(F_BUILD_FILES_EMBED_FS_H_REL:$(F_BUILD_DIR_GEN_EMBED)/%=%)
endif

ifeq ($(F_CONFIG_FILES_EMBED_BLOB), 0)
    F_BUILD_FILES_EMBED_FS_BIN_REL :=
    F_BUILD_FILES_EMBED_FS_BIN_ABS :=
endif

ifeq ($(F_CONFIG_FILES_EMBED_C), 0)
    F_BUILD_FILES_EMBED_FS_H_REL :=
    F_BUILD_FILES_EMBED_FS_H_ABS :=
endif

#
# Embedded binary buffers
#
F_BUILD_DIR_EMBED_BIN := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_EMBED)/bin
F_BUILD_FILES_BIN :=

ifeq ($(shell test -d $(F_BUILD_DIR_EMBED_BIN) ; echo $$?), 0)
    F_BUILD_FILES_BIN := $(shell find $(F_BUILD_DIR_EMBED_BIN) -type f)
endif

F_BUILD_FILES_BIN_C := $(F_BUILD_FILES_BIN:$(F_BUILD_DIR_EMBED_BIN)/%=$(F_BUILD_DIR_GEN_BIN)/%.c)
F_BUILD_FILES_BIN_H := $(F_BUILD_FILES_BIN:$(F_BUILD_DIR_EMBED_BIN)/%=$(F_BUILD_DIR_GEN_BIN)/%.h)

#
# Embedded FSprite and FSample objects
#
F_BUILD_DIR_EMBED_GFX := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_EMBED)/gfx
F_BUILD_FILES_GFX_PNG :=

ifeq ($(shell test -d $(F_BUILD_DIR_EMBED_GFX) ; echo $$?), 0)
    F_BUILD_FILES_GFX_PNG := $(shell find $(F_BUILD_DIR_EMBED_GFX) -type f -name "*.png")
endif

F_BUILD_FILES_GFX_C := $(F_BUILD_FILES_GFX_PNG:$(F_BUILD_DIR_EMBED_GFX)/%=$(F_BUILD_DIR_GEN_GFX)/%.c)
F_BUILD_FILES_GFX_H := $(F_BUILD_FILES_GFX_PNG:$(F_BUILD_DIR_EMBED_GFX)/%=$(F_BUILD_DIR_GEN_GFX)/%.h)

F_BUILD_DIR_EMBED_SFX := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_EMBED)/sfx
F_BUILD_FILES_SFX_WAV :=

ifeq ($(shell test -d $(F_BUILD_DIR_EMBED_SFX) ; echo $$?), 0)
    F_BUILD_FILES_SFX_WAV := $(shell find $(F_BUILD_DIR_EMBED_SFX) -type f -name "*.wav")
endif

F_BUILD_FILES_SFX_C := $(F_BUILD_FILES_SFX_WAV:$(F_BUILD_DIR_EMBED_SFX)/%=$(F_BUILD_DIR_GEN_SFX)/%.c)
F_BUILD_FILES_SFX_H := $(F_BUILD_FILES_SFX_WAV:$(F_BUILD_DIR_EMBED_SFX)/%=$(F_BUILD_DIR_GEN_SFX)/%.h)

F_BUILD_FILES_FAUR_GFX_PNG := $(shell find $(F_FAUR_DIR_MEDIA) -type f -name "f_*.png")
F_BUILD_FILES_FAUR_GFX_C := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_MEDIA)/%=$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.c)
F_BUILD_FILES_FAUR_GFX_H := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_MEDIA)/%=$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h)

#
# All generated source code
#
F_BUILD_FILES_GEN_C := \
    $(F_BUILD_FILES_ECS_INIT_C) \
    $(F_BUILD_FILES_EMBED_INIT_C) \
    $(F_BUILD_FILES_BIN_C) \
    $(F_BUILD_FILES_GFX_C) \
    $(F_BUILD_FILES_SFX_C) \
    $(F_BUILD_FILES_FAUR_GFX_C)

F_BUILD_FILES_GEN_H := \
    $(F_BUILD_FILES_ECS_INIT_H) \
    $(F_BUILD_FILES_BIN_H) \
    $(F_BUILD_FILES_GFX_H) \
    $(F_BUILD_FILES_SFX_H)

#
# Generated files used by application
#
F_BUILD_FILE_GEN_INC_C := $(F_BUILD_DIR_GEN)/include.c
F_BUILD_FILE_GEN_INC_H := $(F_BUILD_DIR_GEN)/include.h

#
# Unique build path and application that gets built
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/targets/$(F_CONFIG_BUILD_UID)
F_BUILD_DIR_BIN := $(F_BUILD_DIR)/bin
F_BUILD_FILE_BIN := $(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME))$(F_CONFIG_APP_NAME_SUFFIX)

#
# Public targets
#
all :
	@ $(call F_MAKE_PRINT_START,Setup)
	$(F_MAKE_COMMAND_SETUP) f__target_setup
	@ $(call F_MAKE_PRINT_DONE,Setup)
	@ $(call F_MAKE_PRINT_START,Code generation)
	$(F_MAKE_COMMAND_GEN) f__target_gen
	@ $(call F_MAKE_PRINT_DONE,Code generation)
	@ $(call F_MAKE_PRINT_START,Build)
	@ sleep 1
	$(F_MAKE_COMMAND_BUILD) f__target_build
	@ $(call F_MAKE_PRINT_DONE,Build)
	@ $(call F_MAKE_PRINT_START,Post)
	$(F_MAKE_COMMAND_POST) f__target_post
	@ $(call F_MAKE_PRINT_DONE,Post)
	@ $(call F_MAKE_PRINT,All done)

run : f__target_run

clean :
	rm -rf $(F_BUILD_DIR)
	rm -rf $(F_BUILD_DIR_GEN_UID)

#
# Internal targets
#
f__target_setup :

f__target_gen : $(F_BUILD_FILE_GEN_INC_C) $(F_BUILD_FILE_GEN_INC_H) $(F_BUILD_FILES_FAUR_GFX_H)

f__target_build : $(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN)

f__target_post :

f__target_run : all

#
# Not file targets
#
.PHONY : all run clean
.PHONY : f__target_setup f__target_gen f__target_build f__target_post f__target_run

#
# Include platform-specific build rules
#
ifneq ($(F_CONFIG_SYSTEM_ARDUINO), 0)
    include $(FAUR_PATH)/make/global/rules-arduino.mk
else
    include $(FAUR_PATH)/make/global/rules-standard.mk
endif

#
# ECS init code
#
$(F_BUILD_FILES_ECS_INIT_C) $(F_BUILD_FILES_ECS_INIT_H) : $(F_FAUR_DIR_BIN)/faur-build-ecs-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-ecs-init --gen-file $@ --com $(F_CONFIG_ECS_COM) --sys $(F_CONFIG_ECS_SYS)

#
# Embedded files
#
$(F_BUILD_DIR_GEN_EMBED)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-file
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-file --bin-file $< --gen-file $@ --original-path $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) --var-prefix f__bin_ --exts $(F_CONFIG_FILES_EMBED_EXTS)

$(F_BUILD_FILES_EMBED_INIT_C) : $(F_BUILD_FILES_EMBED_FS_H_REL) $(F_FAUR_DIR_BIN)/faur-build-embed-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-init --gen-file $@ --bin-dir $(F_DIR_ROOT_FROM_MAKE) --var-prefix f__bin_ --headers $(F_BUILD_FILES_EMBED_FS_H_ABS)

#
# Embedded buffers
#
$(F_BUILD_DIR_GEN_BIN)/%.c : $(F_BUILD_DIR_EMBED_BIN)/% $(F_FAUR_DIR_BIN)/faur-build-embed-bin
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-bin --bin-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

$(F_BUILD_DIR_GEN_BIN)/%.h : $(F_BUILD_DIR_EMBED_BIN)/% $(F_FAUR_DIR_BIN)/faur-build-embed-bin
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-bin --bin-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

#
# Embedded objects
#
$(F_BUILD_DIR_GEN_GFX)/%.c : $(F_BUILD_DIR_EMBED_GFX)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx --image-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) --color-key-hex $(F_CONFIG_COLOR_SPRITE_KEY) --screen-format $(F_CONFIG_SCREEN_FORMAT) --render-mode $(F_CONFIG_SCREEN_RENDER)

$(F_BUILD_DIR_GEN_GFX)/%.h : $(F_BUILD_DIR_EMBED_GFX)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx --image-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) --color-key-hex $(F_CONFIG_COLOR_SPRITE_KEY) --screen-format $(F_CONFIG_SCREEN_FORMAT) --render-mode $(F_CONFIG_SCREEN_RENDER)

$(F_BUILD_DIR_GEN_SFX)/%.c : $(F_BUILD_DIR_EMBED_SFX)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx --wav-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) --sound-format $(F_CONFIG_SOUND_FORMAT)

$(F_BUILD_DIR_GEN_SFX)/%.h : $(F_BUILD_DIR_EMBED_SFX)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx --wav-file $< --gen-file $@ --var-suffix $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) --sound-format $(F_CONFIG_SOUND_FORMAT)

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.c : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx --image-file $< --gen-file $@ --var-suffix $(notdir $(basename $<)) --color-key-hex $(F_CONFIG_COLOR_SPRITE_KEY) --screen-format $(F_CONFIG_SCREEN_FORMAT) --render-mode $(F_CONFIG_SCREEN_RENDER) --expose-extern

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx --image-file $< --gen-file $@ --var-suffix $(notdir $(basename $<)) --color-key-hex $(F_CONFIG_COLOR_SPRITE_KEY) --screen-format $(F_CONFIG_SCREEN_FORMAT) --render-mode $(F_CONFIG_SCREEN_RENDER) --expose-extern

#
# Files that bundle up the generated code
#
$(F_BUILD_FILE_GEN_INC_C) : $(F_BUILD_FILES_GEN_C) $(F_FAUR_DIR_BIN)/faur-build-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-inc --gen-file $@ --files $(F_BUILD_FILES_GEN_C:$(F_BUILD_DIR_GEN)/%=%)

$(F_BUILD_FILE_GEN_INC_H) : $(F_BUILD_FILES_GEN_H) $(F_FAUR_DIR_BIN)/faur-build-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-inc --gen-file $@ --files $(F_BUILD_FILES_GEN_H:$(F_BUILD_DIR_GEN)/%=%)
