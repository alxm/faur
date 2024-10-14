#
# make/global/defs.mk is included by every platform Makefile on line 1
#
F_BUILD_PLATFORM := \
    $(basename $(notdir \
        $(word \
            $(shell expr $(words $(MAKEFILE_LIST)) \- 1), \
            $(MAKEFILE_LIST))))

#
# Used to run sub-make
#
F_MAKE_PARALLEL_JOBS := 8

#
# To support app and author names with spaces
#
F_MAKE_SPACE :=
F_MAKE_SPACE := $(F_MAKE_SPACE) $(F_MAKE_SPACE)
F_MAKE_SPACE_DASH = $(subst $(F_MAKE_SPACE),-,$1)
F_MAKE_SPACE_ESCAPE = $(subst $(F_MAKE_SPACE),\$(F_MAKE_SPACE),$1)

#
# Where the framework lives
#
F_FAUR_DIR_ROOT := $(realpath $(FAUR_PATH))
F_FAUR_DIR_BIN := $(F_FAUR_DIR_ROOT)/bin
F_FAUR_DIR_MEDIA := $(F_FAUR_DIR_ROOT)/media
F_FAUR_DIR_SRC := $(F_FAUR_DIR_ROOT)/src
F_FAUR_DIR_CONFIG := $(HOME)/.config/faur

#
# From <project>/build/make/ to <project> and
# from <project>/build/targets/build_uid/bin/ to <project>
#
F_DIR_ROOT_FROM_MAKE := ../..
F_DIR_ROOT_FROM_BIN := ../../../..

#
# Custom SDK paths and Geany C tags
#
F_FAUR_FILE_SDK_MK := $(F_FAUR_DIR_CONFIG)/sdk.mk
F_FAUR_FILE_GEANY_TAGS := $(HOME)/.config/geany/tags/faur.c.tags

#
# Include the user SDK paths, then conditionally-set the default paths
#
-include $(F_FAUR_FILE_SDK_MK)
include $(FAUR_PATH)/make/global/sdk.mk
