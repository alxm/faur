FAUR_DIR_OBJ := $(A_DIR_OBJ_FAUR)
FAUR_DIR_GEN := $(FAUR_DIR_OBJ)/faur__gen

FAUR_EMBED_PATHS := \
    media/console.png \
    media/fontgrid.png \
    media/fontgrid_keyed.png \

FAUR_FILES_SRC_GEN_H := $(FAUR_EMBED_PATHS:%=$(FAUR_DIR_GEN)/%.h)

FAUR_FILES_PUBLIC_FAUR_HEADERS := \
    $(FAUR_DIR_SRC)/general/a_system_includes.h \
    $(shell find $(FAUR_DIR_SRC) -type f -name "*.p.h")
FAUR_FILES_PRIVATE_FAUR_HEADERS := $(shell find $(FAUR_DIR_SRC) -type f -name "*.v.h")
FAUR_FILE_PUBLIC_FAUR_LIB := $(FAUR_DIR_OBJ)/faur.a
FAUR_FILE_EDITOR_TAGS := $(HOME)/.config/geany/tags/faur.c.tags
FAUR_FILES_SRC_C := $(shell find $(FAUR_DIR_SRC) -type f -name "*.c")
FAUR_FILES_OBJ := $(FAUR_FILES_SRC_C:$(FAUR_DIR_SRC)/%=$(FAUR_DIR_OBJ)/%.o)

FAUR_INFO_COMPILE_TIME := $(shell date "+%Y-%m-%d\ %H:%M:%S")
FAUR_INFO_GIT_HASH := $(shell cd $(FAUR_PATH) && git rev-parse --verify HEAD)

FAUR_GENERIC_CFLAGS := \
    $(A_GENERIC_CFLAGS) \
    -I$(FAUR_DIR_GEN) \
    -DA_CONFIG_BUILD_GIT_HASH=\"$(FAUR_INFO_GIT_HASH)\" \
    -DA_CONFIG_BUILD_TIMESTAMP=\"$(FAUR_INFO_COMPILE_TIME)\" \

$(FAUR_FILE_PUBLIC_FAUR_LIB) : $(FAUR_FILES_OBJ)
	@ mkdir -p $(@D)
	$(AR) rs$(A_CONFIG_BUILD_AR_FLAGS) $@ $(FAUR_FILES_OBJ)

$(FAUR_DIR_OBJ)/%.c.o : $(FAUR_DIR_SRC)/%.c
	@ mkdir -p $(@D)
	$(CC) -c -o $@ $< $(FAUR_GENERIC_CFLAGS)

$(FAUR_DIR_OBJ)/files/a_embed.c.o : $(FAUR_FILES_SRC_GEN_H)

$(FAUR_DIR_GEN)/%.h : $(FAUR_DIR_ROOT)/% $(FAUR_PATH)/bin/faur-bin
	@ mkdir -p $(@D)
	$(FAUR_PATH)/bin/faur-bin $< $@ $(<:$(FAUR_DIR_ROOT)/%=%) a__bin__

$(FAUR_FILE_EDITOR_TAGS) : $(FAUR_FILES_PUBLIC_FAUR_HEADERS)
	test ! -d $(@D) || CFLAGS="$(A_CONFIG_BUILD_CFLAGS)" geany -g -P $@ $^
