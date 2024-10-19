#
# Convenient symlinks available in target's bin dir
#
F_BUILD_LINK_BIN_MEDIA := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_MEDIA)
F_BUILD_LINK_BIN_SCREENSHOTS := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_SCREENSHOTS)

#
# Default files blob
#
F_BUILD_FILE_BLOB := $(F_BUILD_DIR_BIN)/$(F_CONFIG_FILES_EMBED_BLOB_FILE)

#
# Object dirs
#
F_BUILD_DIR_FAUR_O := $(F_BUILD_DIR)/obj/faur
F_BUILD_DIR_PROJ_O := $(F_BUILD_DIR)/obj/proj

#
# All application source code and resulting objects
#
F_BUILD_FILES_SRC_C := $(shell find $(F_BUILD_DIR_SRC) -type f -name "*.c" -not -path "$(F_BUILD_DIR_GEN_ROOT)/*")
F_BUILD_FILES_SRC_O := $(F_BUILD_FILES_SRC_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

F_BUILD_FILES_PROJ_C := $(F_BUILD_FILES_SRC_C) $(F_BUILD_FILES_GEN_C) $(F_BUILD_FILE_GEN_INC_C)
F_BUILD_FILES_PROJ_O := $(F_BUILD_FILES_PROJ_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

#
# Faur lib files
#
F_BUILD_FILES_FAUR_C := $(shell find $(F_FAUR_DIR_SRC) -type f \( -name "*.c" -o -name "*.cpp" \))
F_BUILD_FILES_FAUR_O := $(F_BUILD_FILES_FAUR_C:$(F_FAUR_DIR_SRC)/%=$(F_BUILD_DIR_FAUR_O)/%.o)

F_BUILD_FILES_FAUR_PUBLIC_HEADERS := \
    $(F_FAUR_DIR_SRC)/general/f_system_includes.h \
    $(shell find $(F_FAUR_DIR_SRC) -type f -name "*.p.h")

#
# All object files
#
F_BUILD_FILES_O := $(F_BUILD_FILES_PROJ_O) $(F_BUILD_FILES_FAUR_O)

#
# Compiler flags
#
F_BUILD_FLAGS_SHARED_C_AND_CPP += \
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
    -O$(F_CONFIG_BUILD_OPT)

ifeq ($(F_CONFIG_SYSTEM_EMSCRIPTEN), 0)
    ifeq ($(F_CONFIG_DEBUG), 0)
        F_BUILD_FLAGS_SHARED_C_AND_CPP += -s
    else
        F_BUILD_FLAGS_SHARED_C_AND_CPP += -g
    endif
endif

ifdef F_CONFIG_LIB_SDL_CONFIG
    F_BUILD_FLAGS_SHARED_C_AND_CPP += $(shell $(F_CONFIG_LIB_SDL_CONFIG) --cflags)
endif

F_BUILD_FLAGS_C := \
    $(F_BUILD_FLAGS_SHARED_C_AND_CPP) \
    $(F_CONFIG_BUILD_FLAGS_C) \
    -std=$(F_CONFIG_BUILD_FLAGS_C_STANDARD) \
    $(F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP_OVERRIDE) \
    $(F_CONFIG_BUILD_FLAGS_C_OVERRIDE)

ifneq ($(F_CONFIG_BUILD_FLAGS_C_PEDANTIC), 0)
    F_BUILD_FLAGS_C += -pedantic -pedantic-errors
endif

F_BUILD_FLAGS_CPP := \
    $(F_BUILD_FLAGS_SHARED_C_AND_CPP) \
    $(F_CONFIG_BUILD_FLAGS_CPP) \
    -std=$(F_CONFIG_BUILD_FLAGS_CPP_STANDARD) \
    $(F_CONFIG_BUILD_FLAGS_SHARED_C_AND_CPP_OVERRIDE) \
    $(F_CONFIG_BUILD_FLAGS_CPP_OVERRIDE)

ifneq ($(F_CONFIG_BUILD_FLAGS_CPP_PEDANTIC), 0)
    F_BUILD_FLAGS_CPP += -pedantic -pedantic-errors
endif

#
# Libraries to link with
#
F_BUILD_LIBS := $(F_CONFIG_BUILD_LIBS)

ifdef F_CONFIG_LIB_SDL_CONFIG
    ifeq ($(F_CONFIG_LIB_SDL), 1)
        F_BUILD_LIBS += -lSDL_mixer
    else ifeq ($(F_CONFIG_LIB_SDL), 2)
        F_BUILD_LIBS += -lSDL2_mixer
    endif

    F_BUILD_LIBS += $(shell $(F_CONFIG_LIB_SDL_CONFIG) --libs)
endif

#
# Static files to copy to the build bin dir
#
F_BUILD_DIR_STATIC ?=
F_BUILD_DIR_STATIC_PREFIX := $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/static

ifeq ($(shell test -d $(F_BUILD_DIR_STATIC_PREFIX)/$(F_BUILD_PLATFORM) ; echo $$?), 0)
    F_BUILD_DIR_STATIC += $(F_BUILD_PLATFORM)
endif

ifeq ($(F_CONFIG_LIB_SDL), 2)
    ifeq ($(shell test -d $(F_BUILD_DIR_STATIC_PREFIX)/sdl2 ; echo $$?), 0)
        F_BUILD_DIR_STATIC += sdl2
    endif
endif

#
# Internal targets
#
f__target_post : $(F_BUILD_LINK_BIN_MEDIA) $(F_BUILD_LINK_BIN_SCREENSHOTS)

ifdef F_BUILD_DIR_STATIC
    f__target_post : copystatic
endif

ifneq ($(F_CONFIG_FILES_EMBED_BLOB), 0)
    f__target_post : $(F_BUILD_FILE_BLOB)
endif

copystatic :
	@ mkdir -p $(F_BUILD_DIR_BIN)
	rsync \
		--archive \
		--progress \
		--human-readable \
		$(F_BUILD_DIR_STATIC:%=$(F_BUILD_DIR_STATIC_PREFIX)/%/) \
		$(F_BUILD_DIR_BIN)

#
# Not file targets
#
.PHONY : valgrind valgrindall copystatic

#
# Auto-generated object dependencies
#
-include $(F_BUILD_FILES_O:.o=.d)

#
# Main app
#
$(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) : $(F_BUILD_FILES_O)
	@ mkdir -p $(@D)
	$(F_CONFIG_BUILD_TOOL_CPP) -v -o $@ $^ $(F_BUILD_LIBS)

$(F_BUILD_LINK_BIN_MEDIA) :
	@ mkdir -p $(@D)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_MEDIA) $@

$(F_BUILD_LINK_BIN_SCREENSHOTS) :
	@ mkdir -p $(@D)
	@ mkdir -p $(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_BUILD)/shared/$(F_CONFIG_DIR_SCREENSHOTS) $@

$(F_BUILD_FILE_BLOB) : $(F_BUILD_FILES_EMBED_FS_BIN_REL) $(F_FAUR_DIR_BIN)/faur-blob
	@ mkdir -p $(@D)
	$(F_FAUR_DIR_BIN)/faur-blob --blob-file $@ --root-dir $(F_DIR_ROOT_FROM_MAKE) --files $(F_BUILD_FILES_EMBED_FS_BIN_ABS)

#
# Project source code, including generated code
#
$(F_BUILD_DIR_PROJ_O)/%.c.o : $(F_BUILD_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(F_CONFIG_BUILD_TOOL_CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

#
# Dependencies on generated code
#
$(F_BUILD_FILES_SRC_O) : $(F_BUILD_FILE_GEN_INC_H)

$(F_BUILD_FILES_FAUR_O) : $(F_BUILD_FILES_FAUR_GFX_H)

#
# Faur lib
#
$(F_BUILD_DIR_FAUR_O)/%.c.o : $(F_FAUR_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(F_CONFIG_BUILD_TOOL_CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

$(F_BUILD_DIR_FAUR_O)/%.cpp.o : $(F_FAUR_DIR_SRC)/%.cpp
	@ mkdir -p $(@D)
	$(F_CONFIG_BUILD_TOOL_CPP) -c -o $@ $< $(F_BUILD_FLAGS_CPP)

$(F_FAUR_FILE_GEANY_TAGS) : $(F_BUILD_FILES_FAUR_PUBLIC_HEADERS)
	test ! -d $(@D) || CFLAGS="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" geany -g $@ $^

$(F_FAUR_FILE_SDK_MK) :
	touch $@
