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
