#
# make/global/defs is included by every platform Makefile on line 1
#
A_CONFIG_BUILD_PLATFORM := \
    $(basename $(notdir \
        $(word \
            $(shell expr $(words $(MAKEFILE_LIST)) \- 1), \
            $(MAKEFILE_LIST))))

A_DIR_CONFIG := $(HOME)/.config/a2x
A_FILE_SDKCONFIG_SRC := $(A_DIR_CONFIG)/sdk.config
A_FILE_SDKCONFIG_MK := $(A_FILE_SDKCONFIG_SRC).mk

A_DIR_ROOT := ../..
A_DIR_ROOT_FROM_BIN := ../../../..

#
# To support app and author names with spaces
#
A_MAKE_SPACE := $(A_MAKE_SPACE) $(A_MAKE_SPACE)
A_MAKE_SPACE_DASH = $(subst $(A_MAKE_SPACE),-,$1)
A_MAKE_SPACE_ESCAPE = $(subst $(A_MAKE_SPACE),\$(A_MAKE_SPACE),$1)
