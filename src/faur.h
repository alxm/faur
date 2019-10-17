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

#ifndef A_INC_A2X_H
#define A_INC_A2X_H

#ifdef __cplusplus
#define A_EXTERN_C_START extern "C" {
#define A_EXTERN_C_END }
#else
#define A_EXTERN_C_START
#define A_EXTERN_C_END
#endif

A_EXTERN_C_START
#include "collision/a_collide.p.h"
#include "collision/a_grid.p.h"
#include "data/a_bitfield.p.h"
#include "data/a_block.p.h"
#include "data/a_listit.p.h"
#include "data/a_list.p.h"
#include "data/a_strhash.p.h"
#include "ecs/a_collection.p.h"
#include "ecs/a_component.p.h"
#include "ecs/a_ecs.p.h"
#include "ecs/a_entity.p.h"
#include "ecs/a_system.p.h"
#include "ecs/a_template.p.h"
#include "files/a_dir.p.h"
#include "files/a_embed.p.h"
#include "files/a_file_embedded.p.h"
#include "files/a_file.p.h"
#include "files/a_file_real.p.h"
#include "files/a_path.p.h"
#include "general/a_console.p.h"
#include "general/a_fps.p.h"
#include "general/a_main.p.h"
#include "general/a_menu.p.h"
#include "general/a_out.p.h"
#include "general/a_state.p.h"
#include "graphics/a_color.p.h"
#include "graphics/a_draw.p.h"
#include "graphics/a_fade.p.h"
#include "graphics/a_font.p.h"
#include "graphics/a_pixels.p.h"
#include "graphics/a_png.p.h"
#include "graphics/a_screen.p.h"
#include "graphics/a_screenshot.p.h"
#include "graphics/a_spritelayers.p.h"
#include "graphics/a_sprite.p.h"
#include "input/a_analog.p.h"
#include "input/a_button.p.h"
#include "input/a_controller.p.h"
#include "input/a_input.p.h"
#include "input/a_touch.p.h"
#include "math/a_fix.p.h"
#include "math/a_math.p.h"
#include "math/a_random.p.h"
#include "memory/a_mem.p.h"
#include "platform/a_platform.p.h"
#include "platform/graphics/a_sdl_blit.p.h"
#include "platform/graphics/a_sdl_draw.p.h"
#include "platform/graphics/a_sdl_video.p.h"
#include "platform/graphics/a_software_blit.p.h"
#include "platform/graphics/a_software_draw.p.h"
#include "platform/input/a_sdl_input.p.h"
#include "platform/sound/a_sdl_sound.p.h"
#include "platform/system/a_emscripten.p.h"
#include "platform/system/a_gp2x.p.h"
#include "platform/system/a_linux.p.h"
#include "platform/system/a_pandora.p.h"
#include "platform/system/a_sdl.p.h"
#include "platform/system/a_wiz.p.h"
#include "sound/a_sound.p.h"
#include "strings/a_strbuilder.p.h"
#include "strings/a_str.p.h"
#include "time/a_time.p.h"
#include "time/a_timer.p.h"
A_EXTERN_C_END

#endif // A_INC_A2X_H
