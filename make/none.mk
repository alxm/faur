include $(FAUR_PATH)/make/global/defs-first.mk
include $(FAUR_PATH)/make/global/defs-sdk.mk
include $(FAUR_PATH)/make/global/defs-config.mk
include $(FAUR_PATH)/make/global/rules.mk

f__target_run :
	cd $(F_BUILD_DIR_BIN) && ./$(F_BUILD_FILE_BIN)
