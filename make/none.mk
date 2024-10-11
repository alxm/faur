include $(FAUR_PATH)/make/global/defs.mk
include $(FAUR_PATH)/make/global/config.mk
include $(FAUR_PATH)/make/global/rules.mk

f__target_run :
	cd $(F_BUILD_DIR_BIN) && ./$(F_BUILD_FILE_BIN)
