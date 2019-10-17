/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

#ifndef A_INC_A2X_V_H
#define A_INC_A2X_V_H

#include "a2x.h"

A_EXTERN_C_START
#include "collision/a_collide.v.h"
#include "collision/a_grid.v.h"
#include "data/a_bitfield.v.h"
#include "data/a_block.v.h"
#include "data/a_listit.v.h"
#include "data/a_list.v.h"
#include "data/a_strhash.v.h"
#include "ecs/a_collection.v.h"
#include "ecs/a_component.v.h"
#include "ecs/a_ecs.v.h"
#include "ecs/a_entity.v.h"
#include "ecs/a_system.v.h"
#include "ecs/a_template.v.h"
#include "files/a_dir.v.h"
#include "files/a_embed.v.h"
#include "files/a_file_embedded.v.h"
#include "files/a_file_real.v.h"
#include "files/a_file.v.h"
#include "files/a_path.v.h"
#include "general/a_console.v.h"
#include "general/a_fps.v.h"
#include "general/a_main.v.h"
#include "general/a_menu.v.h"
#include "general/a_out.v.h"
#include "general/a_state.v.h"
#include "graphics/a_color.v.h"
#include "graphics/a_draw.v.h"
#include "graphics/a_fade.v.h"
#include "graphics/a_font.v.h"
#include "graphics/a_pixels.v.h"
#include "graphics/a_png.v.h"
#include "graphics/a_screenshot.v.h"
#include "graphics/a_screen.v.h"
#include "graphics/a_spritelayers.v.h"
#include "graphics/a_sprite.v.h"
#include "input/a_analog.v.h"
#include "input/a_button.v.h"
#include "input/a_controller.v.h"
#include "input/a_input.v.h"
#include "input/a_touch.v.h"
#include "math/a_fix.v.h"
#include "math/a_math.v.h"
#include "math/a_random.v.h"
#include "memory/a_mem.v.h"
#include "platform/a_platform.v.h"
#include "platform/graphics/a_sdl_blit.v.h"
#include "platform/graphics/a_sdl_draw.v.h"
#include "platform/graphics/a_sdl_video.v.h"
#include "platform/graphics/a_software_blit.v.h"
#include "platform/graphics/a_software_draw.v.h"
#include "platform/input/a_sdl_input.v.h"
#include "platform/sound/a_sdl_sound.v.h"
#include "platform/system/a_emscripten.v.h"
#include "platform/system/a_gp2x.v.h"
#include "platform/system/a_linux.v.h"
#include "platform/system/a_pandora.v.h"
#include "platform/system/a_sdl.v.h"
#include "platform/system/a_wiz.v.h"
#include "sound/a_sound.v.h"
#include "strings/a_strbuilder.v.h"
#include "strings/a_str.v.h"
#include "time/a_timer.v.h"
#include "time/a_time.v.h"
A_EXTERN_C_END

#endif // A_INC_A2X_V_H
