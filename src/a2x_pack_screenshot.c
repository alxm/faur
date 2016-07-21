/*
    Copyright 2010 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_screenshot.v.h"

#define SCREENSHOTS_LIMIT 99999

static bool can_save;
static char* prefix;
static unsigned int number;

void a_screenshot__init(void)
{
    const char* const screens_dir = a_settings_getString("screenshot.dir");

    if(!a_dir_exists(screens_dir)) {
        a_dir_make(screens_dir);
    }

    can_save = true;
    prefix = a_str_merge(screens_dir, "/", a_settings_getString("app.title"), "-");
    number = 0;

    ADir* const dir = a_dir_open(screens_dir);
    a_dir_reverse(dir);

    const char** const pair = a_dir__next(dir);

    if(pair) {
        const char* const file = pair[A_DIR_NAME];

        int start = a_str_lastIndex(file, '-');
        int end = a_str_lastIndex(file, '.');

        if(start != -1 && end != -1 && end - start == 6) {
            char* numberStr = a_str_sub(file, start + 1, end);
            number = atoi(numberStr);
            free(numberStr);

            if(number <= 0) {
                can_save = false;
            }
        } else {
            can_save = false;
        }

        if(!can_save) {
            a_out__error("Can't save screenshots: invalid file '%s'", file);
        }
    }

    a_dir_close(dir);
}

void a_screenshot__uninit(void)
{
    free(prefix);
}

void a_screenshot_save(void)
{
    if(!can_save) {
        a_out__error("Can't save screenshots");
        return;
    }

    if(++number > SCREENSHOTS_LIMIT) {
        a_out__error("%d screenshots limit exceeded", SCREENSHOTS_LIMIT);
        return;
    }

    char num[6];
    snprintf(num, 6, "%05d", number);

    char* const name = a_str_merge(prefix, num, ".png");

    a_out__message("Saving screenshot '%s'", name);
    a_png_write(name, a_pixels, a_width, a_height);

    free(name);
}
