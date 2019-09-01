#
# Process and reconcile build settings
#
include $(A2X_PATH)/make/global/config.mk

A_DIR_BUILD_SHARED := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/shared
A_DIR_BUILD_STATIC := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/static
A_DIR_BUILD_UID := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/builds/$(A_CONFIG_BUILD_UID)

#
# Object files dir for current platform
#
A_DIR_OBJ := $(A_DIR_BUILD_UID)/obj
A_DIR_OBJ_APP := $(A_DIR_OBJ)/app
A_DIR_OBJ_A2X := $(A_DIR_OBJ)/a2x

#
# Subdir for generated code and its object files
#
A_DIR_GEN := $(A_DIR_OBJ_APP)/a2x_gen
A_DIR_GEN_EMBED := $(A_DIR_GEN)/embed
A_DIR_GEN_GFX := $(A_DIR_GEN)/gfx

#
# The final bin that gets built
#
A_DIR_BIN := $(A_DIR_BUILD_UID)/bin
A_FILE_BIN_TARGET := $(A_DIR_BIN)/$(A_CONFIG_APP_BIN)
A_FILE_BIN_LINK_ASSETS := $(A_DIR_BIN)/$(A_CONFIG_DIR_ASSETS)
A_FILE_BIN_LINK_SCREENSHOTS := $(A_DIR_BIN)/$(A_CONFIG_DIR_SCREENSHOTS)
A_FILE_BIN_LINKS := $(A_FILE_BIN_LINK_ASSETS) $(A_FILE_BIN_LINK_SCREENSHOTS)

#
# Project root-relative file and dir paths
#
A_FILES_EMBED_BIN := $(shell $(A2X_PATH)/bin/a2x_gather -q $(A_DIR_ROOT) $(A_CONFIG_PATH_EMBED))
A_FILES_SRC_GEN_H := $(A_FILES_EMBED_BIN:%=$(A_DIR_GEN_EMBED)/%.h)

#
# Implements a_embed__populate
#
A_FILES_SRC_GEN_EMBED_DOT_C := $(A_DIR_GEN_EMBED)/embed.c

#
# Graphics data
#
A_FILES_GFX_BIN += $(shell $(A2X_PATH)/bin/a2x_gather -q --no-dirs $(A_DIR_ROOT) $(A_CONFIG_PATH_GFX))
A_FILES_GFX_C := $(A_FILES_GFX_BIN:%=$(A_DIR_GEN_GFX)/%.c)
A_FILES_GFX_H := $(A_FILES_GFX_BIN:%=$(A_DIR_GEN_GFX)/%.h)

#
# C source files
#
A_FILES_SRC_C := $(shell find $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC) -type f -name "*.c")
A_FILES_SRC_C := $(A_FILES_SRC_C:$(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC)/%=%)

#
# All the object files
#
A_FILES_OBJ_APP := $(A_FILES_SRC_C:%=$(A_DIR_OBJ_APP)/%.o)
A_FILES_OBJ_GEN_EMBED := $(A_FILES_SRC_GEN_EMBED_DOT_C:=.o)
A_FILES_OBJ_GEN_GFX := $(A_FILES_GFX_C:=.o)
A_FILES_OBJ_GEN := $(A_FILES_OBJ_GEN_EMBED) $(A_FILES_OBJ_GEN_GFX)
A_FILES_OBJ := $(A_FILES_OBJ_APP) $(A_FILES_OBJ_GEN)

#
# Compiler flags for all targets
#
A_GENERIC_CFLAGS := \
    -DA2X=1 \
    -std=c99 \
    -Wall \
    -Wextra \
    -Wconversion \
    -Wcast-align \
    -Wformat-security \
    -Werror \
    -pedantic \
    -pedantic-errors \
    -fstrict-aliasing \
    -D_XOPEN_SOURCE \
    -I$(A_DIR_OBJ_A2X) \
    -I$(A_DIR_OBJ_APP) \
    $(A_PLATFORM_CFLAGS) \
    $(A_CONFIG_BUILD_CFLAGS) \
    $(A_CONFIG_BUILD_OPT) \

.PHONY : all run clean cleanbin $(A_CONFIG_MAKE_CLEAN)

.SECONDARY : $(A_FILES_GFX_C)

all : $(A_FILE_BIN_TARGET) $(A_FILE_BIN_LINKS)

#
# a2x header and lib build rules
#
include $(A2X_PATH)/make/global/a2x.mk

$(A_FILES_OBJ) : $(A2X_FILE_PUBLIC_A2X_HEADER)

$(A_FILES_OBJ_GEN) : $(A2X_FILE_PRIVATE_A2X_HEADER)

$(A_FILES_OBJ_APP) : $(A_FILES_GFX_H)

$(A_FILE_BIN_TARGET) : $(A_FILES_OBJ) $(A2X_FILE_PUBLIC_A2X_LIB)
	@ mkdir -p $(@D)
	$(CC) -o $@ $^ $(A_CONFIG_BUILD_LIBS) $(A_PLATFORM_LIBS)

$(A_DIR_GEN_EMBED)/%.h : $(A_DIR_ROOT)/% $(A2X_PATH)/bin/a2x_bin
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_bin $< $@ $(<:$(A_DIR_ROOT)/%=%) a__bin_

$(A_FILES_SRC_GEN_EMBED_DOT_C) : $(A_FILES_SRC_GEN_H) $(A2X_PATH)/bin/a2x_embed
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_embed $@ $(A_DIR_GEN_EMBED) a__bin_ $(A_FILES_SRC_GEN_H:$(A_DIR_GEN_EMBED)/%=%)

$(A_DIR_GEN_GFX)/%.c : $(A_DIR_ROOT)/% $(A2X_PATH)/bin/a2x_gfx
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_gfx $< $@ $(<:$(A_DIR_ROOT)/%=%) $(A_CONFIG_SCREEN_BPP) $(A_CONFIG_COLOR_SPRITE_KEY)

$(A_DIR_GEN_GFX)/%.h : $(A_DIR_ROOT)/% $(A2X_PATH)/bin/a2x_gfx
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_gfx $< $@ $(<:$(A_DIR_ROOT)/%=%) $(A_CONFIG_SCREEN_BPP) $(A_CONFIG_COLOR_SPRITE_KEY)

$(A_DIR_OBJ_APP)/%.c.o : $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(A_GENERIC_CFLAGS)

$(A_DIR_GEN)/%.c.o : $(A_DIR_GEN)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(A_GENERIC_CFLAGS)

$(A_FILE_BIN_LINK_ASSETS) :
	@ mkdir -p $(@D)
	ln -s $(A_DIR_ROOT_FROM_BIN)/$(A_CONFIG_DIR_ASSETS) $@

$(A_FILE_BIN_LINK_SCREENSHOTS) :
	@ mkdir -p $(@D)
	@ mkdir -p $(A_DIR_BUILD_SHARED)/$(A_CONFIG_DIR_SCREENSHOTS)
	ln -s $(A_DIR_ROOT_FROM_BIN)/build/shared/$(A_CONFIG_DIR_SCREENSHOTS) $@

clean : $(A_CONFIG_MAKE_CLEAN)
	rm -rf $(A_DIR_BUILD_UID)

run : all
	cd $(A_DIR_BIN) && ./$(A_CONFIG_APP_BIN)
