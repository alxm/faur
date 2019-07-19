/*
    Copyright 2011, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a_screenshot.v.h"
#include <a2x.v.h>

#define A__SCREENSHOTS_LIMIT 99999

static bool g_isInit;
static char* g_filePrefix;
static char* g_title;
static char* g_description;
static unsigned g_screenshotNumber;
static AButton* g_button;

static bool lazy_init(void)
{
    ADir* dir = a_dir_new(A_CONFIG_DIR_SCREENSHOTS);

    if(dir != NULL) {
        // Only interested in the last file, to get the number from its name
        APath* entry = a_list_getLast(a_dir_entriesListGet(dir));

        if(entry == NULL || a_path_getName(entry)[0] == '.') {
            g_isInit = true;
        } else {
            const char* file = a_path_getName(entry);

            int start = a_str_indexGetLast(file, '-');
            int end = a_str_indexGetLast(file, '.');

            if(start != -1 && end != -1 && end - start == 6) {
                char* numberStr = a_str_subGetRange(file, start + 1, end);
                int number = atoi(numberStr);

                if(number > 0) {
                    g_screenshotNumber = (unsigned)number;
                    g_isInit = true;
                }

                free(numberStr);
            }

            if(!g_isInit) {
                a_out__error("Invalid file name '%s'", a_path_getFull(entry));
            }
        }

        a_dir_free(dir);
    }

    if(g_isInit) {
        g_filePrefix = a_str_dup(a_str__fmt512("%s/%s-",
                                               A_CONFIG_DIR_SCREENSHOTS,
                                               A_CONFIG_APP_TITLE));

        g_title = a_str_dup(a_str__fmt512(
            "%s %s by %s",
            A_CONFIG_APP_TITLE,
            A_CONFIG_APP_VERSION_STRING,
            A_CONFIG_APP_AUTHOR));

        g_description = a_str_dup(a_str__fmt512(
            "%s %s by %s, built %s. Running a2x %s %s.",
            A_CONFIG_APP_TITLE,
            A_CONFIG_APP_VERSION_STRING,
            A_CONFIG_APP_AUTHOR,
            A_CONFIG_BUILD_TIMESTAMP,
            A_CONFIG_BUILD_UID,
            A_CONFIG_BUILD_GIT_HASH));

        if(g_filePrefix && g_title && g_description) {
            // No spaces in file name
            for(char* s = g_filePrefix; *s != '\0'; s++) {
                if(*s <= ' ') {
                    *s = '-';
                }
            }
        } else {
            g_isInit = false;
        }
    }

    return g_isInit;
}

static void takeScreenshot(void)
{
    if(!g_isInit && !lazy_init()) {
        return;
    }

    if(++g_screenshotNumber > A__SCREENSHOTS_LIMIT) {
        a_out__error("%d screenshots limit exceeded", A__SCREENSHOTS_LIMIT);
        return;
    }

    const char* name = a_str__fmt512(
                        "%s%05d.png", g_filePrefix, g_screenshotNumber);

    a_out__info("Saving screenshot '%s'", name);

    #if !A_CONFIG_LIB_RENDER_SOFTWARE
        a_platform_api__screenTextureRead(a__screen.pixels, a__screen.frame);
    #endif

    a_png__write(
        name, a__screen.pixels, a__screen.frame, g_title, g_description);
}

void a_screenshot__init(void)
{
    g_button = a_button_new();
    a_button_bind(g_button, A_KEY_F12);
}

void a_screenshot__uninit(void)
{
    free(g_filePrefix);
    free(g_title);
    free(g_description);

    a_button_free(g_button);
}

const APack a_pack__screenshot = {
    "Screenshot",
    {
        [0] = a_screenshot__init,
    },
    {
        [0] = a_screenshot__uninit,
    },
};

void a_screenshot__tick(void)
{
    if(a_button_pressGetOnce(g_button)) {
        takeScreenshot();
    }
}

void a_screenshot_take(void)
{
    takeScreenshot();
}
