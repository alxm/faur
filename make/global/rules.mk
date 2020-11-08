#
# Unique build path
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

#
# Source code dirs
#
F_BUILD_DIR_SRC := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)
F_BUILD_DIR_GEN := $(F_BUILD_DIR_SRC)/faur_gen
F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN)/embed
F_BUILD_DIR_GEN_GFX := $(F_BUILD_DIR_GEN)/gfx
F_BUILD_DIR_GEN_SFX := $(F_BUILD_DIR_GEN)/sfx

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
# Project root-relative paths, and the file that implements f_embed__populate
#
ifneq ($(F_CONFIG_PATH_EMBED),)
    F_BUILD_FILES_EMBED_BIN := $(shell find $(F_CONFIG_PATH_EMBED:%=$(F_DIR_ROOT_FROM_MAKE)/%))
    F_BUILD_FILES_EMBED_NAMES := $(F_BUILD_FILES_EMBED_BIN:$(F_DIR_ROOT_FROM_MAKE)/%=%.h)
    F_BUILD_FILES_EMBED_TARGET := $(F_BUILD_FILES_EMBED_NAMES:%=$(F_BUILD_DIR_GEN_EMBED)/%)
endif

F_BUILD_FILES_EMBED_INIT := $(F_BUILD_DIR_GEN)/g_embed_init.c

#
# Embedded FSprite and FSample objects
#
F_BUILD_FILES_GFX_C := $(F_CONFIG_PATH_GFX:%=$(F_BUILD_DIR_GEN_GFX)/%.c)
F_BUILD_FILES_GFX_H := $(F_CONFIG_PATH_GFX:%=$(F_BUILD_DIR_GEN_GFX)/%.h)
F_BUILD_FILES_SFX_C := $(F_CONFIG_PATH_SFX:%=$(F_BUILD_DIR_GEN_SFX)/%.c)
F_BUILD_FILES_SFX_H := $(F_CONFIG_PATH_SFX:%=$(F_BUILD_DIR_GEN_SFX)/%.h)

#
# All generated source code
#
F_BUILD_FILES_GEN_C := \
    $(F_BUILD_FILES_ECS_INIT) \
    $(F_BUILD_FILES_EMBED_INIT) \
    $(F_BUILD_FILES_GFX_C) \
    $(F_BUILD_FILES_SFX_C) \

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
# Turn off default suffix rules
#
.SUFFIXES :

#
# Not file targets
#
.PHONY : all clean cleangen copystatic run valgrind $(F_CONFIG_BUILD_MAKE_CLEAN)

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
clean : $(F_CONFIG_BUILD_MAKE_CLEAN)
	rm -rf $(F_BUILD_DIR)

cleangen :
	rm -rf $(F_BUILD_DIR_GEN)
