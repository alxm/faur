#
# Process and reconcile build settings
#
include $(FAUR_PATH)/make/global/config.mk

#
# Unique build path
#
F_BUILD_DIR := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/builds/$(F_CONFIG_BUILD_UID)

#
# Application that gets built
#
F_BUILD_DIR_BIN := $(F_BUILD_DIR)/bin
F_BUILD_FILE_BIN := $(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME))$(F_CONFIG_APP_NAME_SUFFIX)

#
# Convenient symlinks available in target's bin dir
#
F_BUILD_LINK_BIN_ASSETS := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_ASSETS)
F_BUILD_LINK_BIN_SCREENSHOTS := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_SCREENSHOTS)

#
# Object dirs
#
F_BUILD_DIR_FAUR_O := $(F_BUILD_DIR)/obj/faur
F_BUILD_DIR_PROJ_O := $(F_BUILD_DIR)/obj/proj

F_BUILD_DIR_GEN_O := $(F_BUILD_DIR_PROJ_O)/faur_gen

ifneq ($(F_CONFIG_PATH_SRC_GEN), 0)
    F_BUILD_DIR_GEN_C := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/faur_gen
else
    F_BUILD_DIR_GEN_C := $(F_BUILD_DIR_GEN_O)
endif

F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN_O)/embed
F_BUILD_DIR_GEN_GFX_C := $(F_BUILD_DIR_GEN_C)/gfx
F_BUILD_DIR_GEN_GFX_O := $(F_BUILD_DIR_GEN_O)/gfx
F_BUILD_DIR_GEN_SFX_C := $(F_BUILD_DIR_GEN_C)/sfx
F_BUILD_DIR_GEN_SFX_O := $(F_BUILD_DIR_GEN_O)/sfx

#
# Application source and object files
#
F_BUILD_FILES_SRC_C := $(shell find $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC) -type f -name "*.c")
F_BUILD_FILES_SRC_C := $(F_BUILD_FILES_SRC_C:$(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/%=%)
F_BUILD_FILES_SRC_O := $(F_BUILD_FILES_SRC_C:%=$(F_BUILD_DIR_PROJ_O)/%.o)

#
# Project root-relative paths, and the file that implements f_embed__populate
#
F_BUILD_FILES_EMBED_BIN := $(shell $(F_FAUR_DIR_BIN)/faur-gather -q $(F_DIR_ROOT_FROM_MAKE) $(F_CONFIG_PATH_EMBED))
F_BUILD_FILES_EMBED_H := $(F_BUILD_FILES_EMBED_BIN:%=$(F_BUILD_DIR_GEN_EMBED)/%.h)
F_BUILD_FILES_EMBED_POPULATE := $(F_BUILD_DIR_GEN_EMBED)/embed.c

#
# Embedded FSprite and FSample objects
#
F_BUILD_FILES_GFX_BIN := $(shell $(F_FAUR_DIR_BIN)/faur-gather -q --no-dirs $(F_DIR_ROOT_FROM_MAKE) $(F_CONFIG_PATH_GFX))
F_BUILD_FILES_GFX_C := $(F_BUILD_FILES_GFX_BIN:%=$(F_BUILD_DIR_GEN_GFX_C)/%.c)
F_BUILD_FILES_GFX_H := $(F_BUILD_FILES_GFX_BIN:%=$(F_BUILD_DIR_GEN_GFX_C)/%.h)

F_BUILD_FILES_SFX_BIN := $(shell $(F_FAUR_DIR_BIN)/faur-gather -q --no-dirs $(F_DIR_ROOT_FROM_MAKE) $(F_CONFIG_PATH_SFX))
F_BUILD_FILES_SFX_C := $(F_BUILD_FILES_SFX_BIN:%=$(F_BUILD_DIR_GEN_SFX_C)/%.c)
F_BUILD_FILES_SFX_H := $(F_BUILD_FILES_SFX_BIN:%=$(F_BUILD_DIR_GEN_SFX_C)/%.h)

#
# All application object files
#
F_BUILD_FILES_O := \
    $(F_BUILD_FILES_SRC_O) \
    $(F_BUILD_FILES_EMBED_POPULATE:=.o) \
    $(F_BUILD_FILES_GFX_BIN:%=$(F_BUILD_DIR_GEN_GFX_O)/%.c.o) \
    $(F_BUILD_FILES_SFX_BIN:%=$(F_BUILD_DIR_GEN_SFX_O)/%.c.o) \

#
# Faur lib files
#
F_BUILD_FILE_FAUR_LIB := $(F_BUILD_DIR_FAUR_O)/faur.a

F_BUILD_FILES_FAUR_C := $(shell find $(F_FAUR_DIR_SRC) -type f \( -name "*.c" -o -name "*.cpp" \))
F_BUILD_FILES_FAUR_O := $(F_BUILD_FILES_FAUR_C:$(F_FAUR_DIR_SRC)/%=$(F_BUILD_DIR_FAUR_O)/%.o)

F_BUILD_FILES_FAUR_PUBLIC_HEADERS := \
    $(F_FAUR_DIR_SRC)/general/f_system_includes.h \
    $(shell find $(F_FAUR_DIR_SRC) -type f -name "*.p.h")

#
# Compiler flags
#
F_BUILD_FLAGS_SHARED := \
    -MMD \
    -MP \
    -Wall \
    -Wextra \
    -Wconversion \
    -Wcast-align \
    -Wformat-security \
    -Werror \
    -fstrict-aliasing \
    -D_XOPEN_SOURCE \
    -I$(F_FAUR_DIR_SRC) \
    -I$(F_BUILD_DIR_FAUR_O) \
    -I$(F_BUILD_DIR_PROJ_O) \
    -O$(F_CONFIG_BUILD_OPT) \
    $(F_CONFIG_BUILD_FLAGS_SHARED) \

ifeq ($(F_CONFIG_BUILD_DEBUG), 1)
    F_BUILD_FLAGS_SHARED += -g
else
    F_BUILD_FLAGS_SHARED += -s
endif

F_BUILD_FLAGS_C := \
    $(F_BUILD_FLAGS_SHARED) \
    $(F_CONFIG_BUILD_FLAGS_C) \
    -std=$(F_CONFIG_BUILD_FLAGS_C_STANDARD) \

ifneq ($(F_CONFIG_BUILD_FLAGS_C_PEDANTIC), 0)
    F_BUILD_FLAGS_C += -pedantic -pedantic-errors
endif

F_BUILD_FLAGS_CPP := \
    $(F_BUILD_FLAGS_SHARED) \
    $(F_CONFIG_BUILD_FLAGS_CPP) \
    -std=$(F_CONFIG_BUILD_FLAGS_CPP_STANDARD) \

ifneq ($(F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC), 0)
    F_BUILD_FLAGS_CPP += -pedantic -pedantic-errors
endif

#
# Default make targets
#
F_MAKE_ALL := \
    $(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) \
    $(F_BUILD_LINK_BIN_ASSETS) \
    $(F_BUILD_LINK_BIN_SCREENSHOTS) \

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
.SECONDARY : $(F_BUILD_FILES_GFX_C) $(F_BUILD_FILES_SFX_C)

#
# Not file targets
#
.PHONY : all run clean $(F_CONFIG_MAKE_CLEAN) copystatic

all : $(F_MAKE_ALL)

#
# Auto-generated object dependencies
#
-include $(F_BUILD_FILES_O:.o=.d) $(F_BUILD_FILES_FAUR_O:.o=.d)

#
# Main app
#
$(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) : $(F_BUILD_FILES_O) $(F_BUILD_FILE_FAUR_LIB)
	@ mkdir -p $(@D)
	$(CC) -o $@ $^ $(F_CONFIG_BUILD_LIBS)

$(F_BUILD_LINK_BIN_ASSETS) :
	@ mkdir -p $(@D)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_ASSETS) $@

$(F_BUILD_LINK_BIN_SCREENSHOTS) :
	@ mkdir -p $(@D)
	@ mkdir -p $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS) $@

#
# Project source code
#
$(F_BUILD_DIR_PROJ_O)/%.c.o : $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

#
# Embedded files and objects
#
$(F_BUILD_DIR_GEN_EMBED)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-bin
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-bin $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) f__bin_

$(F_BUILD_FILES_EMBED_POPULATE) : $(F_BUILD_FILES_EMBED_H) $(F_FAUR_DIR_BIN)/faur-embed
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-embed $@ $(F_BUILD_DIR_GEN_EMBED) f__bin_ $(F_BUILD_FILES_EMBED_H:$(F_BUILD_DIR_GEN_EMBED)/%=%)

$(F_BUILD_DIR_GEN_GFX_C)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_DIR_GEN_GFX_C)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-gfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_DIR_GEN_SFX_C)/%.c : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

$(F_BUILD_DIR_GEN_SFX_C)/%.h : $(F_DIR_ROOT_FROM_MAKE)/% $(F_FAUR_DIR_BIN)/faur-sfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-sfx $< $@ $(<:$(F_DIR_ROOT_FROM_MAKE)/%=%)

$(F_BUILD_DIR_GEN_O)/%.c.o : $(F_BUILD_DIR_GEN_C)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

$(F_BUILD_DIR_GEN_O)/%.c.o : $(F_BUILD_DIR_GEN_O)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

$(F_BUILD_FILES_SRC_O) : $(F_BUILD_FILES_GFX_H) $(F_BUILD_FILES_SFX_H)

#
# Faur lib
#
$(F_BUILD_FILE_FAUR_LIB) : $(F_BUILD_FILES_FAUR_O)
	@ mkdir -p $(@D)
	$(AR) rs$(F_CONFIG_BUILD_FLAGS_AR) $@ $^

$(F_BUILD_DIR_FAUR_O)/%.c.o : $(F_FAUR_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

$(F_BUILD_DIR_FAUR_O)/%.cpp.o : $(F_FAUR_DIR_SRC)/%.cpp
	@ mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(F_BUILD_FLAGS_CPP)

$(F_FAUR_FILE_GEANY_TAGS) : $(F_BUILD_FILES_FAUR_PUBLIC_HEADERS)
	test ! -d $(@D) || CFLAGS="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" geany -g $@ $^

#
# Action targets
#
clean : $(F_CONFIG_MAKE_CLEAN)
	rm -rf $(F_BUILD_DIR)

cleangen :
	rm -rf $(F_BUILD_DIR_GEN_C) $(F_BUILD_DIR_GEN_O)

run : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" ./$(F_BUILD_FILE_BIN)

copystatic :
	@ mkdir -p $(F_BUILD_DIR_BIN)
	rsync --archive --progress --human-readable $(F_CONFIG_PATH_STATIC_COPY:%=$(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/static/%/) $(F_BUILD_DIR_BIN)
