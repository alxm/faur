#
# Application that gets built
#
F_BUILD_DIR_BIN := $(F_BUILD_DIR)/bin
F_BUILD_FILE_BIN := $(call F_MAKE_SPACE_DASH,$(F_CONFIG_APP_NAME))$(F_CONFIG_APP_NAME_SUFFIX)

#
# Convenient symlinks available in target's bin dir
#
F_BUILD_LINK_BIN_MEDIA := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_MEDIA)
F_BUILD_LINK_BIN_SCREENSHOTS := $(F_BUILD_DIR_BIN)/$(F_CONFIG_DIR_SCREENSHOTS)

#
# Object dirs
#
F_BUILD_DIR_FAUR_O := $(F_BUILD_DIR)/obj/faur
F_BUILD_DIR_PROJ_O := $(F_BUILD_DIR)/obj/proj

#
# All application source code and resulting objects
#
F_BUILD_FILES_SRC_C := $(shell find $(F_BUILD_DIR_SRC) -type f -name "*.c" -not -path "$(F_BUILD_DIR_GEN)/*")
F_BUILD_FILES_SRC_O := $(F_BUILD_FILES_SRC_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

F_BUILD_FILES_C := \
    $(F_BUILD_FILES_SRC_C) \
    $(F_BUILD_FILES_GEN_C) \

F_BUILD_FILES_O := \
    $(F_BUILD_FILES_C:$(F_BUILD_DIR_SRC)/%=$(F_BUILD_DIR_PROJ_O)/%.o)

#
# Faur lib files
#
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
F_MAKE_ALL += \
    $(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) \
    $(F_BUILD_LINK_BIN_MEDIA) \
    $(F_BUILD_LINK_BIN_SCREENSHOTS) \

ifdef F_CONFIG_FILES_COPY_STATIC
    F_MAKE_ALL += copystatic
endif

#
# Auto-generated object dependencies
#
-include $(F_BUILD_FILES_O:.o=.d) $(F_BUILD_FILES_FAUR_O:.o=.d)

#
# Main app
#
$(F_BUILD_DIR_BIN)/$(F_BUILD_FILE_BIN) : $(F_BUILD_FILES_O) $(F_BUILD_FILES_FAUR_O)
	@ mkdir -p $(@D)
	$(CC) -o $@ $^ $(F_CONFIG_BUILD_LIBS)

$(F_BUILD_LINK_BIN_MEDIA) :
	@ mkdir -p $(@D)
	ln -s $(F_DIR_ROOT_FROM_BIN)/$(F_CONFIG_DIR_MEDIA) $@

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
# Project source code, not including generated code
#
$(F_BUILD_FILES_SRC_O) : $(F_MAKE_PREREQS)

#
# Faur lib
#
$(F_BUILD_FILES_FAUR_O) : $(F_BUILD_FILES_FAUR_GFX_H)

$(F_BUILD_DIR_FAUR_O)/%.c.o : $(F_FAUR_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(F_BUILD_FLAGS_C)

$(F_BUILD_DIR_FAUR_O)/%.cpp.o : $(F_FAUR_DIR_SRC)/%.cpp
	@ mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(F_BUILD_FLAGS_CPP)

$(F_FAUR_FILE_GEANY_TAGS) : $(F_BUILD_FILES_FAUR_PUBLIC_HEADERS)
	test ! -d $(@D) || CFLAGS="$(F_CONFIG_BUILD_FLAGS_SETTINGS)" geany -g $@ $^

$(F_FAUR_FILE_SDK_MK) :
	touch $@

#
# Action targets
#
run : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" ./$(F_BUILD_FILE_BIN)

valgrind : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind ./$(F_BUILD_FILE_BIN)

valgrindall : all
	cd $(F_BUILD_DIR_BIN) && LD_LIBRARY_PATH=".:$$LD_LIBRARY_PATH" valgrind --leak-check=full --track-origins=yes ./$(F_BUILD_FILE_BIN

copystatic :
	@ mkdir -p $(F_BUILD_DIR_BIN)
	rsync --archive --progress --human-readable $(F_CONFIG_FILES_COPY_STATIC:%=$(F_DIR_ROOT_FROM_MAKE)/$(F_CONFIG_DIR_BUILD)/static/%/) $(F_BUILD_DIR_BIN)

#
# Not file targets
#
.PHONY : copystatic run valgrind
