#
# Process and reconcile build settings
#
include $(FAUR_PATH)/make/global/config.mk

A_DIR_BUILD_SHARED := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/shared
A_DIR_BUILD_STATIC := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/static
A_DIR_BUILD_UID := $(A_DIR_ROOT)/$(A_CONFIG_DIR_BUILD)/builds/$(A_CONFIG_BUILD_UID)

#
# Object files dir for current platform
#
A_DIR_OBJ := $(A_DIR_BUILD_UID)/obj
A_DIR_OBJ_APP := $(A_DIR_OBJ)/app
A_DIR_OBJ_FAUR := $(A_DIR_OBJ)/faur

#
# Subdir for generated code and its object files
#
A_DIR_GEN := $(A_DIR_OBJ_APP)/faur_gen
A_DIR_GEN_EMBED := $(A_DIR_GEN)/embed
A_DIR_GEN_EXTRA := $(A_DIR_GEN)/extra
A_DIR_GEN_GFX := $(A_DIR_GEN)/gfx

#
# The final bin that gets built
#
A_DIR_BIN := $(A_DIR_BUILD_UID)/bin
A_FILE_BIN := $(call A_MAKE_SPACE_DASH,$(A_CONFIG_APP_NAME))$(A_CONFIG_APP_NAME_SUFFIX)
A_FILE_BIN_TARGET := $(A_DIR_BIN)/$(A_FILE_BIN)
A_FILE_BIN_LINK_ASSETS := $(A_DIR_BIN)/$(A_CONFIG_DIR_ASSETS)
A_FILE_BIN_LINK_SCREENSHOTS := $(A_DIR_BIN)/$(A_CONFIG_DIR_SCREENSHOTS)
A_FILE_BIN_LINKS := $(A_FILE_BIN_LINK_ASSETS) $(A_FILE_BIN_LINK_SCREENSHOTS)

#
# Project root-relative file and dir paths
#
A_FILES_EMBED_BIN := $(shell $(FAUR_PATH)/bin/faur-gather -q $(A_DIR_ROOT) $(A_CONFIG_PATH_EMBED))
A_FILES_SRC_GEN_H := $(A_FILES_EMBED_BIN:%=$(A_DIR_GEN_EMBED)/%.h)

#
# Implements a_embed__populate
#
A_FILES_SRC_GEN_EMBED_DOT_C := $(A_DIR_GEN_EMBED)/embed.c

#
# Graphics data
#
A_FILES_GFX_BIN += $(shell $(FAUR_PATH)/bin/faur-gather -q --no-dirs $(A_DIR_ROOT) $(A_CONFIG_PATH_GFX))
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

A_FILES_OBJ_EXTRA := $(A_CONFIG_BUILD_SRC_EXTRA:%=$(A_DIR_GEN_EXTRA)%.o)

A_FILES_OBJ := $(A_FILES_OBJ_APP) $(A_FILES_OBJ_GEN) $(A_FILES_OBJ_EXTRA)

#
# Compiler flags for all targets
#
A_GENERIC_CFLAGS := \
    -MMD \
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
    -I$(FAUR_DIR_SRC) \
    -I$(A_DIR_OBJ_FAUR) \
    -I$(A_DIR_OBJ_APP) \
    $(A_CONFIG_BUILD_CFLAGS) \
    -O$(A_CONFIG_BUILD_OPT) \
    -std=$(A_CONFIG_BUILD_C_STANDARD) \

ifeq ($(A_CONFIG_BUILD_DEBUG), 1)
    A_GENERIC_CFLAGS += -g
else
    A_GENERIC_CFLAGS += -s
endif

A_MAKE_ALL := $(A_FILE_BIN_TARGET) $(A_FILE_BIN_LINKS)

ifdef A_CONFIG_PATH_STATIC_COPY
    A_MAKE_ALL += copystatic
endif

.PHONY : all run clean $(A_CONFIG_MAKE_CLEAN) copystatic

.SECONDARY : $(A_FILES_GFX_C)

all : $(A_MAKE_ALL)

#
# Faur lib build rules
#
include $(FAUR_PATH)/make/global/faur.mk

#
# Object dependencies
#
-include $(A_FILES_OBJ:.o=.d) $(FAUR_FILES_OBJ:.o=.d)

$(A_FILES_OBJ_APP) : $(A_FILES_GFX_H)

$(A_FILE_BIN_TARGET) : $(A_FILES_OBJ) $(FAUR_FILE_PUBLIC_FAUR_LIB)
	@ mkdir -p $(@D)
	$(CC) -o $@ $^ $(A_CONFIG_BUILD_LIBS)

$(A_DIR_GEN_EMBED)/%.h : $(A_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-bin
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-bin $< $@ $(<:$(A_DIR_ROOT)/%=%) a__bin_

$(A_FILES_SRC_GEN_EMBED_DOT_C) : $(A_FILES_SRC_GEN_H) $(FAUR_PATH)/bin/faur-embed
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-embed $@ $(A_DIR_GEN_EMBED) a__bin_ $(A_FILES_SRC_GEN_H:$(A_DIR_GEN_EMBED)/%=%)

$(A_DIR_GEN_GFX)/%.c : $(A_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-gfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-gfx $< $@ $(<:$(A_DIR_ROOT)/%=%) $(A_CONFIG_SCREEN_BPP) $(A_CONFIG_COLOR_SPRITE_KEY)

$(A_DIR_GEN_GFX)/%.h : $(A_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-gfx
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-gfx $< $@ $(<:$(A_DIR_ROOT)/%=%) $(A_CONFIG_SCREEN_BPP) $(A_CONFIG_COLOR_SPRITE_KEY)

$(A_DIR_OBJ_APP)/%.c.o : $(A_DIR_ROOT)/$(A_CONFIG_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(A_GENERIC_CFLAGS)

$(A_DIR_GEN_EXTRA)%.c.o : %.c
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
	cd $(A_DIR_BIN) && ./$(A_FILE_BIN)

copystatic :
	@ mkdir -p $(A_DIR_BIN)
	rsync --archive --progress --human-readable $(A_CONFIG_PATH_STATIC_COPY:%=$(A_DIR_BUILD_STATIC)/%/) $(A_DIR_BIN)
