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

static char* screens_dir;

void a_screenshot__init(void)
{
    screens_dir = a2x_str("screenshot.dir");

    if(!a_dir_exists(screens_dir)) {
        a_dir_make(screens_dir);
    }
}

void a_screenshot_save(void)
{
    Dir* const dir = a_dir_open(screens_dir);
    a_dir_reverse(dir);

    int number = 1;

    if(a_dir_iterate(dir)) {
        const char* const file = a_dir_current(dir)[0];

        int start = a_str_lastIndex(file, '-');
        int end = a_str_lastIndex(file, '.');

        if(start == -1 || end == -1 || end - start != 6) {
            a_error("Found invalid screenshot file: %s", file);
        } else {
            char* const num = a_str_sub(file, start + 1, end);
            int num_i = atoi(num);

            if(num_i > 0) {
                number = num_i + 1;
                a_out("Saving screenshot %d", number);
            } else {
                a_error("Found invalid screenshot file: %s", file);
            }

            free(num);
        }
    }

    if(number == -1) {
        a_error("Could not take screenshot");
        return;
    }

    char name[256];
    sprintf(name, "%s/%s-%05d.png", screens_dir, a2x_str("app.title"), number);

    a_png_write(name, a_pixels, a_width, a_height);

    //cleanup:

    a_dir_close(dir);
}
