/*
    Copyright 2011, 2016, 2018-2019 Alex Margarit <alex@alxm.org>
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
static char* g_filePrefix;
static char* g_title;
static char* g_description;
static unsigned g_screenshotNumber;
static FButton* g_button;

static bool lazy_init(void)
{
    // Only interested in the last file, to get the number from its name
    FDir* dir = f_dir_new(F_CONFIG_DIR_SCREENSHOTS);
    FPath* entry = f_list_getLast(f_dir_entriesGet(dir));

    if(entry == NULL) {
        g_isInit = true;
    } else {
        const char* file = f_path_getName(entry);

        int start = f_str_indexGetLast(file, '-');
        int end = f_str_indexGetLast(file, '.');

        if(start != -1 && end != -1 && end - start == 6) {
            char* numberStr = f_str_subGetRange(file, start + 1, end);
            int number = atoi(numberStr);

            if(number > 0) {
                g_screenshotNumber = (unsigned)number;
                g_isInit = true;
            }

            f_mem_free(numberStr);
        }

        if(!g_isInit) {
            f_out__error("Invalid file name '%s'", f_path_getFull(entry));
        }
    }

    f_dir_free(dir);

    if(g_isInit) {
        g_filePrefix = f_str_dup(f_str__fmt512("%s/%s-",
                                               F_CONFIG_DIR_SCREENSHOTS,
                                               F_CONFIG_APP_NAME));

        g_title = f_str_dup(f_str__fmt512(
            "%s %s by %s",
            F_CONFIG_APP_NAME,
            F__APP_VERSION_STRING,
            F_CONFIG_APP_AUTHOR));

        g_description = f_str_dup(f_str__fmt512(
            "%s %s by %s, built %s. Using Faur %s %s.",
            F_CONFIG_APP_NAME,
            F__APP_VERSION_STRING,
            F_CONFIG_APP_AUTHOR,
            F_CONFIG_BUILD_FAUR_TIME,
            F_CONFIG_BUILD_UID,
            F_CONFIG_BUILD_FAUR_GIT));

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

    if(++g_screenshotNumber > F__SCREENSHOTS_LIMIT) {
        f_out__error("%d screenshots limit exceeded", F__SCREENSHOTS_LIMIT);
        return;
    }

    const char* name = f_str__fmt512(
                        "%s%05d.png", g_filePrefix, g_screenshotNumber);

    f_out__info("Saving screenshot '%s'", name);

    #if !F_CONFIG_RENDER_SOFTWARE
        f_platform_api__screenTextureSync();
    #endif

    f_png__write(
        name, f__screen.pixels, f__screen.frame, g_title, g_description);
}

void f_screenshot__init(void)
{
    g_button = f_button_new();
    f_button_bindKey(g_button, F_KEY_F12);
}

void f_screenshot__uninit(void)
{
    f_mem_free(g_filePrefix);
    f_mem_free(g_title);
    f_mem_free(g_description);

    f_button_free(g_button);
}

const FPack f_pack__screenshot = {
    "Screenshot",
    {
        [0] = f_screenshot__init,
    },
    {
        [0] = f_screenshot__uninit,
    },
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
#else // !F_CONFIG_LIB_PNG
const FPack f_pack__screenshot;

void f_screenshot__tick(void)
{
}
#endif // !F_CONFIG_LIB_PNG
