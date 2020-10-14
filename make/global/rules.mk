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

#
# Source code dirs
#
F_BUILD_DIR_SRC := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_SRC)
F_BUILD_DIR_GEN := $(F_BUILD_DIR_SRC)/faur_gen
F_BUILD_DIR_GEN_EMBED := $(F_BUILD_DIR_GEN)/embed
F_BUILD_DIR_GEN_GFX := $(F_BUILD_DIR_GEN)/gfx
F_BUILD_DIR_GEN_SFX := $(F_BUILD_DIR_GEN)/sfx

#
# Application source code
#
F_BUILD_FILES_SRC_C := $(shell find $(F_BUILD_DIR_SRC) -type f -name "*.c" -not -path "$(F_BUILD_DIR_GEN)/*")
F_BUILD_FILES_SRC_O := $(F_BUILD_FILES_SRC_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

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
					-e "extern FEntityInit e_" \
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
# All application source code and object files
#
F_BUILD_FILES_GEN_C := \
    $(F_BUILD_FILES_ECS_INIT) \
    $(F_BUILD_FILES_EMBED_INIT) \
    $(F_BUILD_FILES_GFX_C) \
    $(F_BUILD_FILES_SFX_C) \

F_BUILD_FILES_C := \
    $(F_BUILD_FILES_SRC_C) \
    $(F_BUILD_FILES_GEN_C) \

F_BUILD_FILES_O := \
    $(F_BUILD_FILES_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

#
# Faur lib files
#
F_BUILD_FILE_FAUR_LIB := $(F_BUILD_DIR_FAUR_O)/faur.a

F_BUILD_FILES_FAUR_GFX_PNG := $(shell find $(F_FAUR_DIR_MEDIA) -type f -name "g_*.png")
F_BUILD_FILES_FAUR_GFX_H := $(F_BUILD_FILES_FAUR_GFX_PNG:$(F_FAUR_DIR_ROOT)/%=$(F_FAUR_DIR_SRC_GEN)/%.h)

F_BUILD_FILES_FAUR_C := \
    $(shell find $(F_FAUR_DIR_SRC) \
	-not -path "$(F_FAUR_DIR_SRC_GEN)/*" \
	-type f \
	\( -name "*.c" -o -name "*.cpp" \)) \
    $(F_BUILD_FILES_FAUR_GFX_H:%.h=%.c) \

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

ifeq ($(F_CONFIG_DEBUG), 0)
    F_BUILD_FLAGS_SHARED += -s
else
    F_BUILD_FLAGS_SHARED += -g
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
    $(F_BUILD_FILES_GEN_C) \
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
# Not file targets
#
.PHONY : all clean cleangen copystatic run valgrind $(F_CONFIG_BUILD_MAKE_CLEAN)

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
# Project source code, including generated code
#
$(F_BUILD_DIR_PROJ_O)/%.c.o : $(F_BUILD_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

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
# So application C files can use these generated headers
#
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

$(F_FAUR_DIR_SRC_GEN)/%.c : $(F_FAUR_DIR_ROOT)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_FAUR_DIR_SRC_GEN)/%.h : $(F_FAUR_DIR_ROOT)/% $(F_FAUR_DIR_BIN)/faur-build-embed-gfx
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-build-embed-gfx $< $@ _$(notdir $(basename $<)) $(F_CONFIG_COLOR_SPRITE_KEY)

$(F_BUILD_FILES_FAUR_O) : $(F_BUILD_FILES_FAUR_GFX_H)

$(F_FAUR_FILE_GEANY_TAGS) : $(F_BUILD_FILES_FAUR_PUBLIC_HEADERS)
	test ! -d $(@D) || CFLAGS="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" geany -g $@ $^

$(F_FAUR_FILE_SDK_MK) :
	touch $@

#
# Action targets
#
clean : $(F_CONFIG_BUILD_MAKE_CLEAN)
	rm -rf $(F_BUILD_DIR)

cleangen :
	rm -rf $(F_BUILD_DIR_GEN)

run : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" ./$(F_BUILD_FILE_BIN)

valgrind : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind ./$(F_BUILD_FILE_BIN)

copystatic :
	@ mkdir -p $(F_BUILD_DIR_BIN)
	rsync --archive --progress --human-readable $(F_CONFIG_PATH_STATIC_COPY:%=$(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/static/%/) $(F_BUILD_DIR_BIN)
