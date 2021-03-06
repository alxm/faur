/*
    Copyright 2011, 2016, 2018-2020 Alex Margarit <alex@alxm.org>
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

#include "f_screenshot.v.h"
#include <faur.v.h>

#if F_CONFIG_LIB_PNG
#define F__SCREENSHOTS_LIMIT 99999

static bool g_isInit;
static char* g_prefix;
static char* g_title;
static char* g_description;
static unsigned g_number;
static FButton* g_button;

static void takeScreenshot(void)
{
    if(!g_isInit) {
        return;
    }

    if(++g_number > F__SCREENSHOTS_LIMIT) {
        f_out__error("%d screenshots limit exceeded", F__SCREENSHOTS_LIMIT);

        return;
    }

    char path[64];

    if(!f_str_fmt(
            path, sizeof(path), false, "%s%05d.png", g_prefix, g_number)) {

        return;
    }

    f_out__info("Saving screenshot '%s'", path);

    #if !F_CONFIG_SCREEN_RENDER_SOFTWARE
        f_platform_api__screenTextureSync();
    #endif

    f_png__write(
        path, f__screen.pixels, f__screen.frame, g_title, g_description);
}

void f_screenshot__init(void)
{
    // Only interested in the last file, to get the number from its name
    FDir* dir = f_dir_new(F_CONFIG_DIR_SCREENSHOTS);
    FPath* entry = f_list_getLast(f_dir_entriesGet(dir));

    if(entry) {
        const char* name = f_path_getName(entry);
        int start = f_str_indexGetLast(name, '-');
        int end = f_str_indexGetLast(name, '.');

        if(start != -1 && end != -1 && end - start == 6) {
            char* numberStr = f_str_subGetRange(name, start + 1, end);
            int number = atoi(numberStr);

            f_mem_free(numberStr);

            if(number > 0) {
                g_number = (unsigned)number;
            }
        }
    }

    f_dir_free(dir);

    char buffer[256];

    if(f_str_fmt(buffer,
                 sizeof(buffer),
                 false,
                 "%s/%s-",
                 F_CONFIG_DIR_SCREENSHOTS,
                 F_CONFIG_APP_NAME)) {

        g_prefix = f_str_dup(buffer);
    }

    if(f_str_fmt(buffer,
                 sizeof(buffer),
                 false,
                 "%s %s by %s",
                 F_CONFIG_APP_NAME,
                 F__APP_VERSION_STRING,
                 F_CONFIG_APP_AUTHOR)) {

        g_title = f_str_dup(buffer);
    }

    if(f_str_fmt(buffer,
                 sizeof(buffer),
                 false,
                 "%s %s by %s, built %s. Using Faur %s %s.",
                 F_CONFIG_APP_NAME,
                 F__APP_VERSION_STRING,
                 F_CONFIG_APP_AUTHOR,
                 F_CONFIG_BUILD_FAUR_TIME,
                 F_CONFIG_BUILD_UID,
                 F_CONFIG_BUILD_FAUR_GIT)) {

        g_description = f_str_dup(buffer);
    }

    if(g_prefix && g_title && g_description) {
        // No spaces in file name
        for(char* s = g_prefix; *s != '\0'; s++) {
            if(*s <= ' ') {
                *s = '-';
            }
        }

        g_isInit = true;
    }

    g_button = f_button_new();
    f_button_bindKey(g_button, F_KEY_F12);
}

void f_screenshot__uninit(void)
{
    f_mem_free(g_prefix);
    f_mem_free(g_title);
    f_mem_free(g_description);

    f_button_free(g_button);
}

const FPack f_pack__screenshot = {
    "Screenshot",
    f_screenshot__init,
    f_screenshot__uninit,
};

void f_screenshot__tick(void)
{
    if(f_button_pressGetOnce(g_button)) {
        takeScreenshot();
    }
}

void f_screenshot_take(void)
{
    takeScreenshot();
}
#endif // F_CONFIG_LIB_PNG
