#
# Unique build path
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

#
# Source code dirs
#
F_BUILD_DIR_SRC := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)
F_BUILD_DIR_GEN_ROOT := $(F_BUILD_DIR_SRC)/faur_gen
F_BUILD_DIR_GEN := $(F_BUILD_DIR_GEN_ROOT)/faur_v
F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN)/embed
F_BUILD_DIR_GEN_FAUR_MEDIA := $(F_BUILD_DIR_GEN)/faur_gfx
F_BUILD_DIR_GEN_GFX := $(F_BUILD_DIR_GEN)/gfx
F_BUILD_DIR_GEN_SFX := $(F_BUILD_DIR_GEN)/sfx

F_CONFIG_BUILD_FLAGS_SHARED += -I$(F_BUILD_DIR_GEN_ROOT)

#
# Generated ECS init code
#
F_BUILD_FILES_ECS_INIT := $(F_BUILD_DIR_GEN)/g_ecs_init.c
F_BUILD_FILES_ECS_HEADERS := $(shell find $(F_BUILD_DIR_SRC) \
				-type f \
				-name "*.h" \
				-not -path "$(F_BUILD_DIR_GEN)/*" \
				-exec \
				    grep \
					-l \
					-e "extern FSystem s_" \
					-e "extern FComponent c_" \
					-e "extern FCallEntityInit e_" \
					{} +)

#
# Project root-relative paths
#
F_BUILD_FILES_EMBED_BIN := $(foreach f, $(F_CONFIG_FILES_EMBED_PATHS), $(shell find $(f:%=$(F_DIR_ROOT_FROM_MAKE)/%)))
F_BUILD_FILES_EMBED_NAMES := $(F_BUILD_FILES_EMBED_BIN:$(F_DIR_ROOT_FROM_MAKE)/%=%.h)
F_BUILD_FILES_EMBED_TARGET := $(F_BUILD_FILES_EMBED_NAMES:%=$(F_BUILD_DIR_GEN_EMBED)/%)

#
# The file that implements f_embed__populate
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
F_BUILD_FILES_FAUR_GFX_H := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_MEDIA)/%=$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h)
F_BUILD_FILES_FAUR_GFX_C := $(F_BUILD_FILES_FAUR_GFX_H:%.h=%.c)

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
F_MAKE_PREREQS := $(F_BUILD_FILE_GEN_INC_C) $(F_BUILD_FILE_GEN_INC_H)

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
# ECS init code
#
$(F_BUILD_FILES_ECS_INIT) : $(F_BUILD_FILES_ECS_HEADERS) $(F_FAUR_DIR_BIN)/faur-build-ecs-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-ecs-init $@ $(F_BUILD_FILES_ECS_HEADERS)

#
# Embedded files
#
$(F_BUILD_DIR_GEN_EMBED)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-bin
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-bin $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) f__bin_

$(F_BUILD_FILES_EMBED_INIT) : $(F_BUILD_FILES_EMBED_TARGET) $(F_FAUR_DIR_BIN)/faur-build-embed-init
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-init $@ $(F_BUILD_DIR_GEN_EMBED) f__bin_ $(F_BUILD_FILES_EMBED_NAMES)

#
# Embedded objects
#
$(F_BUILD_DIR_GEN_GFX)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_DIR_GEN_GFX)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_DIR_GEN_SFX)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

$(F_BUILD_DIR_GEN_SFX)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-build-embed-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.c : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_DIR_GEN_FAUR_MEDIA)/%.h : $(F_FAUR_DIR_MEDIA)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY)

#
# Files that bundle up the generated code
#
$(F_BUILD_FILE_GEN_INC_C) : $(F_BUILD_FILES_GEN_C) $(F_FAUR_DIR_BIN)/faur-build-embed-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-inc --include-c $@ $(F_BUILD_FILES_GEN_C:$(F_BUILD_DIR_GEN)/%=%)

$(F_BUILD_FILE_GEN_INC_H) : $(F_BUILD_FILES_GEN_H) $(F_FAUR_DIR_BIN)/faur-build-embed-inc
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-inc --include-h $@ $(F_BUILD_FILES_GEN_H:$(F_BUILD_DIR_GEN)/%=%)

#
# Action targets
#
clean :
	rm -rf $(F_BUILD_DIR)

cleangen :
	rm -rf $(F_BUILD_DIR_GEN_ROOT)

cleanall : clean cleangen

#
# Turn off Make default suffix rules
#
.SUFFIXES :

#
# Not file targets
#
.PHONY : all all_build clean cleanall cleangen
