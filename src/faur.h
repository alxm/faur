/*
    Copyright 2019-2020 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_FAUR_H
#define F_INC_FAUR_H

#ifdef __cplusplus
#define F_EXTERN_C_START extern "C" {
#define F_EXTERN_C_END }
#else
#define F_EXTERN_C_START
#define F_EXTERN_C_END
#endif

F_EXTERN_C_START
#include "collision/f_collide.p.h"
#include "collision/f_grid.p.h"
#include "data/f_bitfield.p.h"
#include "data/f_block.p.h"
#include "data/f_hash.p.h"
#include "data/f_listit.p.h"
#include "data/f_list.p.h"
#include "ecs/f_collection.p.h"
#include "ecs/f_component.p.h"
#include "ecs/f_ecs.p.h"
#include "ecs/f_entity.p.h"
#include "ecs/f_system.p.h"
#include "ecs/f_template.p.h"
#include "files/f_blob.p.h"
#include "files/f_dir.p.h"
#include "files/f_file.p.h"
#include "files/f_path.p.h"
#include "general/f_console.p.h"
#include "general/f_fps.p.h"
#include "general/f_init.p.h"
#include "general/f_main.p.h"
#include "general/f_menu.p.h"
#include "general/f_out.p.h"
#include "general/f_state.p.h"
#include "graphics/f_align.p.h"
#include "graphics/f_color.p.h"
#include "graphics/f_draw.p.h"
#include "graphics/f_fade.p.h"
#include "graphics/f_font.p.h"
#include "graphics/f_screen.p.h"
#include "graphics/f_screenshot.p.h"
#include "graphics/f_sprite.p.h"
#include "graphics/f_spritelayers.p.h"
#include "graphics/f_spritesheet.p.h"
#include "input/f_analog.p.h"
#include "input/f_button.p.h"
#include "input/f_controller.p.h"
#include "input/f_input.p.h"
#include "input/f_touch.p.h"
#include "math/f_fix.p.h"
#include "math/f_math.p.h"
#include "math/f_random.p.h"
#include "memory/f_mem.p.h"
#include "platform/video/f_gamebuino_video.p.h"
#include "sound/f_channel.p.h"
#include "sound/f_music.p.h"
#include "sound/f_sample.p.h"
#include "strings/f_strbuilder.p.h"
#include "strings/f_str.p.h"
#include "time/f_time.p.h"
#include "time/f_timer.p.h"
F_EXTERN_C_END

#endif // F_INC_FAUR_H
