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

#ifndef F_INC_FAUR_V_H
#define F_INC_FAUR_V_H

#include "faur.h"

F_EXTERN_C_START
#include "data/f_block.v.h"
#include "data/f_hash.v.h"
#include "data/f_list.v.h"
#include "ecs/f_collection.v.h"
#include "ecs/f_component.v.h"
#include "ecs/f_ecs.v.h"
#include "ecs/f_entity.v.h"
#include "ecs/f_system.v.h"
#include "ecs/f_template.v.h"
#include "files/f_blob.v.h"
#include "files/f_embed.v.h"
#include "files/f_file_embedded.v.h"
#include "files/f_file.v.h"
#include "files/f_path.v.h"
#include "general/f_console.v.h"
#include "general/f_fps.v.h"
#include "general/f_init.v.h"
#include "general/f_main.v.h"
#include "general/f_out.v.h"
#include "general/f_state.v.h"
#include "general/f_sym.v.h"
#include "graphics/f_align.v.h"
#include "graphics/f_color.v.h"
#include "graphics/f_fade.v.h"
#include "graphics/f_font.v.h"
#include "graphics/f_pixels.v.h"
#include "graphics/f_png.v.h"
#include "graphics/f_screenshot.v.h"
#include "graphics/f_screen.v.h"
#include "graphics/f_sprite.v.h"
#include "input/f_button.v.h"
#include "input/f_input.v.h"
#include "math/f_fix.v.h"
#include "math/f_random.v.h"
#include "memory/f_mem.v.h"
#include "memory/f_pool.v.h"
#include "platform/f_platform.v.h"
#include "platform/graphics/f_software_blit.v.h"
#include "platform/video/f_sdl_video.v.h"
#include "platform/input/f_odroid_go_input.v.h"
#include "platform/input/f_sdl_input.v.h"
#include "platform/sound/f_sdl_sound.v.h"
#include "platform/system/f_emscripten.v.h"
#include "platform/system/f_gp2x.v.h"
#include "platform/system/f_linux.v.h"
#include "platform/system/f_odroid_go.v.h"
#include "platform/system/f_pandora.v.h"
#include "platform/system/f_sdl.v.h"
#include "platform/system/f_wiz.v.h"
#include "sound/f_sample.v.h"
#include "sound/f_sound.v.h"
#include "strings/f_str.v.h"
#include "time/f_timer.v.h"
F_EXTERN_C_END

#endif // F_INC_FAUR_V_H
