include ../global/defs

A_MAKE_TARGETS += $(A_FILE_EDITOR_TAGS)

include ../platforms/dev
include rules

.PHONY : config

config : $(A_FILE_SDKCONFIG_MK)

A_FILE_SDKCONFIG_SRC := $(A_DIR_CONFIG)/sdk.config

$(A_FILE_SDKCONFIG_MK) : $(A_FILE_SDKCONFIG_SRC) $(A2X_PATH)/bin/a2x_sdkconfig
	@ mkdir -p $(@D)
	@ $(A2X_PATH)/bin/a2x_sdkconfig $< $@

$(A_FILE_SDKCONFIG_SRC) :
	@ mkdir -p $(@D)
	@ $(A2X_PATH)/bin/a2x_sdkconfig $@
