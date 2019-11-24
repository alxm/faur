F_FILES_GFX := \
	console_19x7.png \
	font_6x8.png \
	font_keyed_6x8.png \
	palette.png \

F_DIR_GEN := ../src/generated/media

F_FILES_C := $(F_FILES_GFX:%=$(F_DIR_GEN)/%.c)
F_FILES_H := $(F_FILES_GFX:%=$(F_DIR_GEN)/%.h)

.SUFFIXES :

.PHONY : gen

gen : $(F_FILES_C) $(F_FILES_H)

$(F_DIR_GEN)/%.c : % $(FAUR_PATH)/bin/faur-gfx
	$(FAUR_PATH)/bin/faur-gfx $< $@ _$(basename $<) 0xff00ff

$(F_DIR_GEN)/%.h : % $(FAUR_PATH)/bin/faur-gfx
	$(FAUR_PATH)/bin/faur-gfx $< $@ _$(basename $<) 0xff00ff

clean :
	rm -f $(F_FILES_C) $(F_FILES_H)
