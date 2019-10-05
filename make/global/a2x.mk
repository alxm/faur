A2X_DIR_OBJ := $(A_DIR_OBJ_A2X)
A2X_DIR_GEN := $(A2X_DIR_OBJ)/a2x__gen

A2X_EMBED_PATHS := \
    media/console.png \
    media/fontgrid.png \
    media/fontgrid_keyed.png \

A2X_FILES_SRC_GEN_H := $(A2X_EMBED_PATHS:%=$(A2X_DIR_GEN)/%.h)

A2X_FILES_PUBLIC_A2X_HEADERS := \
    $(A2X_DIR_SRC)/general/a_system_includes.h \
    $(shell find $(A2X_DIR_SRC) -type f -name "*.p.h")
A2X_FILES_PRIVATE_A2X_HEADERS := $(shell find $(A2X_DIR_SRC) -type f -name "*.v.h")
A2X_FILE_PUBLIC_A2X_LIB := $(A2X_DIR_OBJ)/a2x.a
A2X_FILE_EDITOR_TAGS := $(HOME)/.config/geany/tags/a2x.c.tags
A2X_FILES_SRC_C := $(shell find $(A2X_DIR_SRC) -type f -name "*.c")
A2X_FILES_OBJ := $(A2X_FILES_SRC_C:$(A2X_DIR_SRC)/%=$(A2X_DIR_OBJ)/%.o)

A2X_INFO_COMPILE_TIME := $(shell date "+%Y-%m-%d\ %H:%M:%S")
A2X_INFO_GIT_HASH := $(shell cd $(A2X_PATH) && git rev-parse --verify HEAD)

A2X_GENERIC_CFLAGS := \
    $(A_GENERIC_CFLAGS) \
    -I$(A2X_DIR_GEN) \
    -DA_CONFIG_BUILD_GIT_HASH=\"$(A2X_INFO_GIT_HASH)\" \
    -DA_CONFIG_BUILD_TIMESTAMP=\"$(A2X_INFO_COMPILE_TIME)\" \

$(A2X_FILE_PUBLIC_A2X_LIB) : $(A2X_FILES_OBJ)
	@ mkdir -p $(@D)
	$(AR) rs$(A_CONFIG_BUILD_AR_FLAGS) $@ $(A2X_FILES_OBJ)

$(A2X_DIR_OBJ)/%.c.o : $(A2X_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(A2X_GENERIC_CFLAGS)

$(A2X_DIR_OBJ)/files/a_embed.c.o : $(A2X_FILES_SRC_GEN_H)

$(A2X_DIR_GEN)/%.h : $(A2X_DIR_ROOT)/% $(A2X_PATH)/bin/a2x_bin
	@ mkdir -p $(@D)
	$(A2X_PATH)/bin/a2x_bin $< $@ $(<:$(A2X_DIR_ROOT)/%=%) a__bin__

$(A2X_FILE_EDITOR_TAGS) : $(A2X_FILES_PUBLIC_A2X_HEADERS)
	test ! -d $(@D) || CFLAGS="$(A_CONFIG_BUILD_CFLAGS)" geany -g -P $@ $^
