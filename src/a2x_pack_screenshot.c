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

static bool g_isInit;
static char* g_filePrefix;
static unsigned int g_screenshotNumber;

static bool lazy_init(void)
{
    const char* screensDir = a_settings_getString("screenshot.dir");

    if(a_dir_exists(screensDir)) {
        ADir* dir = a_dir_open(screensDir);

        if(a_dir_num(dir) == 0) {
            g_isInit = true;
        } else {
            const char* file;
            const char* fullPath;

            // Only interested in the last file, to get the number from its name
            a_dir_reverse(dir);

            A_DIR_ITERATE(dir, file, fullPath) {
                int start = a_str_lastIndex(file, '-');
                int end = a_str_lastIndex(file, '.');

                if(start != -1 && end != -1 && end - start == 6) {
                    char* numberStr = a_str_sub(file, start + 1, end);
                    int number = atoi(numberStr);
                    free(numberStr);

                    if(number > 0) {
                        g_screenshotNumber = number;
                        g_isInit = true;
                        break;
                    }
                }

                a_out__error("Invalid file name %s", fullPath);
                break;
            }
        }

        a_dir_close(dir);
    } else {
        a_out__message("Making screenshots dir: %s", screensDir);
        a_dir_make(screensDir);
        g_isInit = true;
    }

    if(g_isInit) {
        g_filePrefix = a_str_merge(screensDir,
                                   "/",
                                   a_settings_getString("app.title"),
                                   "-");
    }

    return g_isInit;
}

void a_screenshot__init(void)
{
    g_isInit = false;
    g_filePrefix = NULL;
    g_screenshotNumber = 0;
}

void a_screenshot__uninit(void)
{
    if(g_filePrefix != NULL) {
        free(g_filePrefix);
    }
}

void a_screenshot_save(void)
{
    if(!g_isInit && !lazy_init()) {
        return;
    }

    if(++g_screenshotNumber > SCREENSHOTS_LIMIT) {
        a_out__error("%d screenshots limit exceeded", SCREENSHOTS_LIMIT);
        return;
    }

    char num[6];
    snprintf(num, 6, "%05d", g_screenshotNumber);
    char* name = a_str_merge(g_filePrefix, num, ".png");

    a_out__message("Saving screenshot '%s'", name);
    a_png_write(name, a_screen__pixels, a_screen__width, a_screen__height);

    free(name);
}
