#
# Process and reconcile build settings
#
include $(FAUR_PATH)/make/global/config.mk

F_DIR_BUILD_UID := $(F_DIR_ROOT)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

#
# Object files dir for current platform
#
F_DIR_OBJ := $(F_DIR_BUILD_UID)/obj
F_DIR_OBJ_APP := $(F_DIR_OBJ)/app
F_DIR_OBJ_FAUR := $(F_DIR_OBJ)/faur

#
# Subdir for generated code and its object files
#
F_DIR_GEN := $(F_DIR_OBJ_APP)/faur_gen
F_DIR_GEN_EMBED := $(F_DIR_GEN)/embed
F_DIR_GEN_EXTRA := $(F_DIR_GEN)/extra
F_DIR_GEN_GFX := $(F_DIR_GEN)/gfx
F_DIR_GEN_SFX := $(F_DIR_GEN)/sfx

#
# The final bin that gets built
#
F_DIR_BIN := $(F_DIR_BUILD_UID)/bin
F_FILE_BIN := $(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME))$(F_CONFIG_APP_NAME_SUFFIX)
F_FILE_BIN_TARGET := $(F_DIR_BIN)/$(F_FILE_BIN)
F_FILE_BIN_LINK_ASSETS := $(F_DIR_BIN)/$(F_CONFIG_DIR_ASSETS)
F_FILE_BIN_LINK_SCREENSHOTS := $(F_DIR_BIN)/$(F_CONFIG_DIR_SCREENSHOTS)
F_FILE_BIN_LINKS := $(F_FILE_BIN_LINK_ASSETS) $(F_FILE_BIN_LINK_SCREENSHOTS)

#
# Project root-relative file and dir paths
#
F_FILES_EMBED_BIN := $(shell $(FAUR_PATH)/bin/faur-gather -q $(F_DIR_ROOT) $(F_CONFIG_PATH_EMBED))
F_FILES_SRC_GEN_H := $(F_FILES_EMBED_BIN:%=$(F_DIR_GEN_EMBED)/%.h)

#
# Implements f_embed__populate
#
F_FILES_SRC_GEN_EMBED_DOT_C := $(F_DIR_GEN_EMBED)/embed.c

#
# Graphics data
#
F_FILES_GFX_BIN += $(shell $(FAUR_PATH)/bin/faur-gather -q --no-dirs $(F_DIR_ROOT) $(F_CONFIG_PATH_GFX))
F_FILES_GFX_C := $(F_FILES_GFX_BIN:%=$(F_DIR_GEN_GFX)/%.c)
F_FILES_GFX_H := $(F_FILES_GFX_BIN:%=$(F_DIR_GEN_GFX)/%.h)

#
# Sound effects data
#
F_FILES_SFX_BIN += $(shell $(FAUR_PATH)/bin/faur-gather -q --no-dirs $(F_DIR_ROOT) $(F_CONFIG_PATH_SFX))
F_FILES_SFX_C := $(F_FILES_SFX_BIN:%=$(F_DIR_GEN_SFX)/%.c)
F_FILES_SFX_H := $(F_FILES_SFX_BIN:%=$(F_DIR_GEN_SFX)/%.h)

#
# C source files
#
F_FILES_SRC_C := $(shell find $(F_DIR_ROOT)/$(F_CONFIG_DIR_SRC) -type f -name "*.c")
F_FILES_SRC_C := $(F_FILES_SRC_C:$(F_DIR_ROOT)/$(F_CONFIG_DIR_SRC)/%=%)

#
# All the object files
#
F_FILES_OBJ_APP := $(F_FILES_SRC_C:%=$(F_DIR_OBJ_APP)/%.o)

F_FILES_OBJ_GEN_EMBED := $(F_FILES_SRC_GEN_EMBED_DOT_C:=.o)
F_FILES_OBJ_GEN_GFX := $(F_FILES_GFX_C:=.o)
F_FILES_OBJ_GEN_SFX := $(F_FILES_SFX_C:=.o)
F_FILES_OBJ_GEN := $(F_FILES_OBJ_GEN_EMBED) $(F_FILES_OBJ_GEN_GFX) $(F_FILES_OBJ_GEN_SFX)

F_FILES_OBJ_EXTRA := $(F_CONFIG_BUILD_SRC_EXTRA:%=$(F_DIR_GEN_EXTRA)%.o)

F_FILES_OBJ := $(F_FILES_OBJ_APP) $(F_FILES_OBJ_GEN) $(F_FILES_OBJ_EXTRA)

#
# Compiler flags for all targets
#
F_GENERIC_FLAGS_SHARED := \
    -MMD \
    -Wall \
    -Wextra \
    -Wconversion \
    -Wcast-align \
    -Wformat-security \
    -Werror \
    -fstrict-aliasing \
    -D_XOPEN_SOURCE \
    -I$(FAUR_DIR_SRC) \
    -I$(F_DIR_OBJ_FAUR) \
    -I$(F_DIR_OBJ_APP) \
    -O$(F_CONFIG_BUILD_OPT) \
    $(F_CONFIG_BUILD_FLAGS_SHARED) \

ifeq ($(F_CONFIG_BUILD_DEBUG), 1)
    F_GENERIC_FLAGS_SHARED += -g
else
    F_GENERIC_FLAGS_SHARED += -s
endif

F_GENERIC_FLAGS_C := \
    $(F_GENERIC_FLAGS_SHARED) \
    $(F_CONFIG_BUILD_FLAGS_C) \
    -std=$(F_CONFIG_BUILD_FLAGS_C_STANDARD) \

ifneq ($(F_CONFIG_BUILD_FLAGS_C_PEDANTIC), 0)
    F_GENERIC_FLAGS_C += -pedantic -pedantic-errors
endif

F_GENERIC_FLAGS_CPP := \
    $(F_GENERIC_FLAGS_SHARED) \
    $(F_CONFIG_BUILD_FLAGS_CPP) \
    -std=$(F_CONFIG_BUILD_FLAGS_CPP_STANDARD) \

ifneq ($(F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC), 0)
    F_GENERIC_FLAGS_CPP += -pedantic -pedantic-errors
endif

F_MAKE_ALL := $(F_FILE_BIN_TARGET) $(F_FILE_BIN_LINKS)

ifdef F_CONFIG_PATH_STATIC_COPY
    F_MAKE_ALL += copystatic
endif

#
# Turn off default suffix rules
#
.SUFFIXES :

#
# Keep intermediary C files around for debugging
#
.SECONDARY : $(F_FILES_GFX_C) $(F_FILES_SFX_C)

#
# Not file targets
#
.PHONY : all run clean $(F_CONFIG_MAKE_CLEAN) copystatic

all : $(F_MAKE_ALL)

#
# Faur lib build rules
#
include $(FAUR_PATH)/make/global/faur.mk

#
# Auto-generated object dependencies
#
-include $(F_FILES_OBJ:.o=.d) $(FAUR_FILES_OBJ:.o=.d)

$(F_FILES_OBJ_APP) : $(F_FILES_GFX_H) $(F_FILES_SFX_H)

$(F_FILE_BIN_TARGET) : $(F_FILES_OBJ) $(FAUR_FILE_PUBLIC_FAUR_LIB)
	@ mkdir -p $(@D)
	$(CC) -o $@ $^ $(F_CONFIG_BUILD_LIBS)

$(F_DIR_GEN_EMBED)/%.h : $(F_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-bin
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-bin $< $@ $(<:$(F_DIR_ROOT)/%=%) f__bin_

$(F_FILES_SRC_GEN_EMBED_DOT_C) : $(F_FILES_SRC_GEN_H) $(FAUR_PATH)/bin/faur-embed
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-embed $@ $(F_DIR_GEN_EMBED) f__bin_ $(F_FILES_SRC_GEN_H:$(F_DIR_GEN_EMBED)/%=%)

$(F_DIR_GEN_GFX)/%.c : $(F_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-gfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-gfx $< $@ $(<:$(F_DIR_ROOT)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_DIR_GEN_GFX)/%.h : $(F_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-gfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-gfx $< $@ $(<:$(F_DIR_ROOT)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_DIR_GEN_SFX)/%.c : $(F_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-sfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-sfx $< $@ $(<:$(F_DIR_ROOT)/%=%)

$(F_DIR_GEN_SFX)/%.h : $(F_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-sfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-sfx $< $@ $(<:$(F_DIR_ROOT)/%=%)

$(F_DIR_OBJ_APP)/%.c.o : $(F_DIR_ROOT)/$(F_CONFIG_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_GENERIC_FLAGS_C)

$(F_DIR_GEN_EXTRA)%.c.o : %.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_GENERIC_FLAGS_C)

$(F_DIR_GEN)/%.c.o : $(F_DIR_GEN)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_GENERIC_FLAGS_C)

$(F_FILE_BIN_LINK_ASSETS) :
	@ mkdir -p $(@D)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_ASSETS) $@

$(F_FILE_BIN_LINK_SCREENSHOTS) :
	@ mkdir -p $(@D)
	@ mkdir -p $(F_DIR_ROOT)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS) $@

clean : $(F_CONFIG_MAKE_CLEAN)
	rm -rf $(F_DIR_BUILD_UID)

run : all
	cd $(F_DIR_BIN) && ./$(F_FILE_BIN)

copystatic :
	@ mkdir -p $(F_DIR_BIN)
	rsync --archive --progress --human-readable $(F_CONFIG_PATH_STATIC_COPY:%=$(F_DIR_ROOT)/$(F_CONFIG_DIR_BUILD)/static/%/) $(F_DIR_BIN)
