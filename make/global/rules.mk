#
# Unique build path
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

#
# Source code dirs
#
F_BUILD_DIR_SRC := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)
F_BUILD_DIR_GEN_ROOT := $(F_BUILD_DIR_SRC)/faur_gen
F_BUILD_DIR_GEN_UID := $(F_BUILD_DIR_GEN_ROOT)/$(F_CONFIG_BUILD_UID)
F_BUILD_DIR_GEN := $(F_BUILD_DIR_GEN_UID)/faur_v
F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN)/embed
F_BUILD_DIR_GEN_FAUR_MEDIA := $(F_BUILD_DIR_GEN)/faur_gfx
F_BUILD_DIR_GEN_GFX := $(F_BUILD_DIR_GEN)/gfx
F_BUILD_DIR_GEN_SFX := $(F_BUILD_DIR_GEN)/sfx

F_BUILD_FLAGS_SHARED_C_AND_CPP := \
    $(F_CONFIG_BUILD_FLAGS_SETTINGS) \
    $(F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP) \
    -I$(F_BUILD_DIR_GEN_UID) \

#
# The file that initializes ECS
#
F_BUILD_FILES_ECS_INIT := $(F_BUILD_DIR_GEN)/g_ecs_init.c
F_BUILD_FILES_ECS_HEADERS := $(shell find $(F_BUILD_DIR_SRC) \
				-type f \
				-name "*.h" \
				-not -path "$(F_BUILD_DIR_GEN_ROOT)/*" \
				-exec \
				    grep \
					-l \
					-e "extern const FSystem s_" \
					-e "extern FComponent c_" \
					{} +)

#
# Project root-relative paths to embedded files
#
F_BUILD_FILES_EMBED_BIN_PATHS_C_REL := $(foreach f, $(F_CONFIG_FILES_EMBED_PATHS_C), $(shell find $(f:%=$(F_DIR_ROOT_FROM_MAKE)/%)))
F_BUILD_FILES_EMBED_BIN_PATHS_C_ABS := $(F_BUILD_FILES_EMBED_BIN_PATHS_C_REL:$(F_DIR_ROOT_FROM_MAKE)/%=%)

F_BUILD_FILES_EMBED_BIN_H := $(F_BUILD_FILES_EMBED_BIN_PATHS_C_ABS:%=%.h)
F_BUILD_FILES_EMBED_BIN_H_TARGETS := $(F_BUILD_FILES_EMBED_BIN_H:%=$(F_BUILD_DIR_GEN_EMBED)/%)

#
# The file that contains embedded file entries
#
F_BUILD_FILES_EMBED_INIT := $(F_BUILD_DIR_GEN)/g_embed_init.c

#
# Embedded FSprite and FSample objects
#
F_BUILD_FILES_GFX_C := $(F_CONFIG_FILES_EMBED_OBJ_SPRITE:%=$(F_BUILD_DIR_GEN_GFX)/%.c)
F_BUILD_FILES_GFX_H := $(F_CONFIG_FILES_EMBED_OBJ_SPRITE:%=$(F_BUILD_DIR_GEN_GFX)/%.h)
F_BUILD_FILES_SFX_C := $(F_CONFIG_FILES_EMBED_OBJ_SAMPLE:%=$(F_BUILD_DIR_GEN_SFX)/%.c)
F_BUILD_FILES_SFX_H := $(F_CONFIG_FILES_EMBED_OBJ_SAMPLE:%=$(F_BUILD_DIR_GEN_SFX)/%.h)

F_BUILD_FILES_FAUR_GFX_PNG := $(shell find $(F_FAUR_DIR_MEDIA) -type f -name "g_*.png")
F_BUILD_FILES_FAUR_GFX_C := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_MEDIA)/%=$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.c)
F_BUILD_FILES_FAUR_GFX_H := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_MEDIA)/%=$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h)

#
# All generated source code
#
F_BUILD_FILES_GEN_C := \
    $(F_BUILD_FILES_ECS_INIT) \
    $(F_BUILD_FILES_EMBED_INIT) \
    $(F_BUILD_FILES_GFX_C) \
    $(F_BUILD_FILES_SFX_C) \
    $(F_BUILD_FILES_FAUR_GFX_C) \

F_BUILD_FILES_GEN_H := \
    $(F_BUILD_FILES_GFX_H) \
    $(F_BUILD_FILES_SFX_H) \

#
# Generated files used by application
#
F_BUILD_FILE_GEN_INC_C := $(F_BUILD_DIR_GEN)/include.c
F_BUILD_FILE_GEN_INC_H := $(F_BUILD_DIR_GEN)/include.h

#
# Default make targets
#
F_MAKE_ALL :=

#
# Declare default target first
#
all : all_build

#
# Include platform-specific build rules
#
ifneq ($(F_CONFIG_SYSTEM_ARDUINO), 0)
    include $(FAUR_PATH)/make/global/rules-arduino.mk
else
    include $(FAUR_PATH)/make/global/rules-default.mk
endif

#
# Default target
#
all_build : $(F_MAKE_ALL)

#
# Code generation target
#
gen : $(F_BUILD_FILE_GEN_INC_C) $(F_BUILD_FILE_GEN_INC_H) $(F_BUILD_FILES_FAUR_GFX_H)

#
# ECS init code
#
$(F_BUILD_FILES_ECS_INIT) : $(F_BUILD_FILES_ECS_HEADERS) $(F_FAUR_DIR_BIN)/faur-build-ecs-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-ecs-init $@ $(F_BUILD_FILES_ECS_HEADERS)

#
# Embedded files
#
$(F_BUILD_DIR_GEN_EMBED)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-file
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-file $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) f__bin_

$(F_BUILD_FILES_EMBED_INIT) : $(F_BUILD_FILES_EMBED_BIN_H_TARGETS) $(F_FAUR_DIR_BIN)/faur-build-embed-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-init $@ $(F_BUILD_DIR_GEN_EMBED) f__bin_ $(F_BUILD_FILES_EMBED_BIN_H)

#
# Embedded objects
#
$(F_BUILD_DIR_GEN_GFX)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY) $(F_CONFIG_SCREEN_FORMAT)

$(F_BUILD_DIR_GEN_GFX)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY) $(F_CONFIG_SCREEN_FORMAT)

$(F_BUILD_DIR_GEN_SFX)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_SOUND_FORMAT)

$(F_BUILD_DIR_GEN_SFX)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_SOUND_FORMAT)

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.c : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY) $(F_CONFIG_SCREEN_FORMAT)

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY) $(F_CONFIG_SCREEN_FORMAT)

#
# Files that bundle up the generated code
#
$(F_BUILD_FILE_GEN_INC_C) : $(F_BUILD_FILES_GEN_C) $(F_FAUR_DIR_BIN)/faur-build-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-inc --include-c $@ $(F_BUILD_FILES_GEN_C:$(F_BUILD_DIR_GEN)/%=%)

$(F_BUILD_FILE_GEN_INC_H) : $(F_BUILD_FILES_GEN_H) $(F_FAUR_DIR_BIN)/faur-build-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-inc --include-h $@ $(F_BUILD_FILES_GEN_H:$(F_BUILD_DIR_GEN)/%=%)

#
# Action targets
#
clean :
	rm -rf $(F_BUILD_DIR)

cleangen :
	rm -rf $(F_BUILD_DIR_GEN_UID)

cleanall : clean cleangen

#
# Turn off Make default suffix rules
#
.SUFFIXES :

#
# Not file targets
#
.PHONY : all all_build clean cleanall cleangen gen
