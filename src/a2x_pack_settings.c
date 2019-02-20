/*
    Copyright 2010, 2016-2019 Alex Margarit
    This file is part of a2x, a C video game framework.

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

#include "a2x_pack_settings.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strhash.v.h"

typedef enum {
    A__SETTING_FLAG_NONE = 0,
    A__SETTING_FLAG_SET_ONCE = A_FLAG_BIT(0),
    A__SETTING_FLAG_FROZEN = A_FLAG_BIT(1),
    A__SETTING_FLAG_CHANGED = A_FLAG_BIT(2),
    A__SETTING_FLAG_FREE_STRING = A_FLAG_BIT(3),
} ASettingFlag;

typedef struct {
    const char* id;
    ASettingType type;
    ASettingFlag flags;
    ASettingCallback* callback;
    union {
        int integer;
        unsigned integeru;
        bool boolean;
        char* string;
        APixel pixel;
    } value;
} ASetting;

static const char* g_typeNames[A__SETTING_TYPE_NUM] = {
    [A__SETTING_TYPE_INT] = "int",
    [A__SETTING_TYPE_INTU] = "intu",
    [A__SETTING_TYPE_BOOL] = "bool",
    [A__SETTING_TYPE_STR] = "str",
    [A__SETTING_TYPE_COLOR] = "color",
};

#ifndef A_CONFIG_SCREEN_WIDTH
    #define A_CONFIG_SCREEN_WIDTH 320
#endif

#ifndef A_CONFIG_SCREEN_HEIGHT
    #define A_CONFIG_SCREEN_HEIGHT 240
#endif

#ifndef A_CONFIG_SCREEN_FULLSCREEN
    #define A_CONFIG_SCREEN_FULLSCREEN false
#endif

#define A__SETTING(Id, Type, Flags, UnionMember, Value) \
    [Id] = {#Id, Type, Flags, NULL, .value.UnionMember = Value}

static ASetting g_settings[A_SETTING_NUM] = {
    A__SETTING(A_SETTING_APP_TITLE, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "Untitled"),
    A__SETTING(A_SETTING_APP_VERSION, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "0.1.0"),
    A__SETTING(A_SETTING_APP_AUTHOR, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "(unknown),"),

    A__SETTING(A_SETTING_OUTPUT_ON, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, true),
    A__SETTING(A_SETTING_OUTPUT_VERBOSE, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, false),
    A__SETTING(A_SETTING_OUTPUT_CONSOLE, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, false),

    A__SETTING(A_SETTING_FPS_TICK, A__SETTING_TYPE_INTU, A__SETTING_FLAG_SET_ONCE, integeru, 30),
    A__SETTING(A_SETTING_FPS_DRAW, A__SETTING_TYPE_INTU, A__SETTING_FLAG_SET_ONCE, integeru, 30),

    A__SETTING(A_SETTING_VIDEO_WIDTH, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, A_CONFIG_SCREEN_WIDTH),
    A__SETTING(A_SETTING_VIDEO_HEIGHT, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, A_CONFIG_SCREEN_HEIGHT),
    A__SETTING(A_SETTING_VIDEO_ZOOM, A__SETTING_TYPE_INT, A__SETTING_FLAG_NONE, integer, 1),
    A__SETTING(A_SETTING_VIDEO_MAX_WINDOW, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, true),
    A__SETTING(A_SETTING_VIDEO_VSYNC, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, false),
    A__SETTING(A_SETTING_VIDEO_DOUBLEBUFFER, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, false),
    A__SETTING(A_SETTING_VIDEO_FULLSCREEN, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, A_CONFIG_SCREEN_FULLSCREEN),

    A__SETTING(A_SETTING_COLOR_SCREEN_BORDER, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_NONE, integeru, 0x1f0f0f),
    A__SETTING(A_SETTING_COLOR_VOLBAR_BACKGROUND, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0x1f0f0f),
    A__SETTING(A_SETTING_COLOR_VOLBAR_BORDER, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0x3f8fdf),
    A__SETTING(A_SETTING_COLOR_VOLBAR_FILL, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0x9fcf3f),
    A__SETTING(A_SETTING_COLOR_KEY, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0xFF00FF),
    A__SETTING(A_SETTING_COLOR_LIMIT, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0x00FF00),
    A__SETTING(A_SETTING_COLOR_END, A__SETTING_TYPE_COLOR, A__SETTING_FLAG_SET_ONCE, integeru, 0x00FFFF),

    A__SETTING(A_SETTING_SOUND_MUTE, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, false),
    A__SETTING(A_SETTING_SOUND_SAMPLE_CHANNELS_TOTAL, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, 64),
    A__SETTING(A_SETTING_SOUND_SAMPLE_CHANNELS_RESERVED, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, 32),
    A__SETTING(A_SETTING_SOUND_VOLUME_SCALE_MUSIC, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, 100),
    A__SETTING(A_SETTING_SOUND_VOLUME_SCALE_SAMPLE, A__SETTING_TYPE_INT, A__SETTING_FLAG_SET_ONCE, integer, 100),

    A__SETTING(A_SETTING_INPUT_MOUSE_CURSOR, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_NONE, boolean, true),
    A__SETTING(A_SETTING_INPUT_MOUSE_TRACK, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, false),
    A__SETTING(A_SETTING_INPUT_ANALOG_AXES_SWITCH, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, false),
    A__SETTING(A_SETTING_INPUT_ANALOG_AXES_INVERT, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, false),

    A__SETTING(A_SETTING_FILE_SCREENSHOTS, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "./screenshots"),
    A__SETTING(A_SETTING_FILE_CONFIG, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "a2x.cfg"),
    A__SETTING(A_SETTING_FILE_GAMEPADMAP, A__SETTING_TYPE_STR, A__SETTING_FLAG_SET_ONCE, string, "gamecontrollerdb.txt"),

    A__SETTING(A_SETTING_SYSTEM_GP2X_MENU, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, true),
    A__SETTING(A_SETTING_SYSTEM_GP2X_MHZ, A__SETTING_TYPE_INTU, A__SETTING_FLAG_SET_ONCE, integeru, 0),
    A__SETTING(A_SETTING_SYSTEM_WIZ_FIXTEARING, A__SETTING_TYPE_BOOL, A__SETTING_FLAG_SET_ONCE, boolean, true),
};

static AStrHash* g_settingsIndex; // table of ASetting

void a_settings__init(void)
{
    g_settingsIndex = a_strhash_new();

    for(int s = 0; s < A_SETTING_NUM; s++) {
        a_strhash_add(g_settingsIndex, g_settings[s].id, (void*)(ptrdiff_t)s);
    }
}

void a_settings__init2(void)
{
    for(int s = 0; s < A_SETTING_NUM; s++) {
        if(g_settings[s].type == A__SETTING_TYPE_COLOR
            && !A_FLAG_TEST_ANY(g_settings[s].flags, A__SETTING_FLAG_CHANGED)) {

            g_settings[s].value.pixel = a_pixel_fromHex(
                                            g_settings[s].value.integeru);
        }
    }
}

void a_settings__init3(void)
{
    for(int s = 0; s < A_SETTING_NUM; s++) {
        if(A_FLAG_TEST_ANY(g_settings[s].flags, A__SETTING_FLAG_SET_ONCE)) {
            A_FLAG_SET(g_settings[s].flags, A__SETTING_FLAG_FROZEN);
        }
    }
}

void a_settings__uninit(void)
{
    for(int s = 0; s < A_SETTING_NUM; s++) {
        if(A_FLAG_TEST_ANY(g_settings[s].flags, A__SETTING_FLAG_FREE_STRING)) {
            free(g_settings[s].value.string);
        }
    }

    a_strhash_free(g_settingsIndex);
}

ASettingId a_settings__stringToId(const char* Key)
{
    if(!a_strhash_contains(g_settingsIndex, Key)) {
        return A_SETTING_INVALID;
    }

    return (ASettingId)a_strhash_get(g_settingsIndex, Key);
}

const char* a_settings__idToString(ASettingId Setting)
{
    return g_settings[Setting].id;
}

ASettingType a_settings__typeGet(ASettingId Setting)
{
    return g_settings[Setting].type;
}

void a_settings__callbackSet(ASettingId Setting, ASettingCallback* Callback, bool RunNow)
{
    g_settings[Setting].callback = Callback;

    if(Callback && RunNow) {
        Callback(Setting);
    }
}

bool a_settings_isDefault(ASettingId Setting)
{
    return !A_FLAG_TEST_ANY(g_settings[Setting].flags, A__SETTING_FLAG_CHANGED);
}

static ASetting* validate(ASettingId Setting, ASettingType Type, bool Write)
{
    if(g_settings[Setting].type != Type) {
        a_out__error("Setting %s is type %s, not %s",
                     g_settings[Setting].id,
                     g_typeNames[g_settings[Setting].type],
                     g_typeNames[Type]);
        return NULL;
    }

    if(Write
        && A_FLAG_TEST_ANY(g_settings[Setting].flags, A__SETTING_FLAG_FROZEN)) {

        a_out__error("Setting %s is frozen", g_settings[Setting].id);
        return NULL;
    }

    return &g_settings[Setting];
}

bool a_settings_boolGet(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_BOOL, false);

    if(s == NULL) {
        return false;
    }

    return s->value.boolean;
}

void a_settings_boolSet(ASettingId Setting, bool Value)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_BOOL, true);

    if(s == NULL) {
        return;
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED);
    s->value.boolean = Value;

    if(s->callback) {
        s->callback(Setting);
    }
}

bool a_settings_boolFlip(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_BOOL, true);

    if(s == NULL) {
        return false;
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED);
    s->value.boolean = !s->value.boolean;

    if(s->callback) {
        s->callback(Setting);
    }

    return s->value.boolean;
}

int a_settings_intGet(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_INT, false);

    if(s == NULL) {
        return 0;
    }

    return s->value.integer;
}

void a_settings_intSet(ASettingId Setting, int Value)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_INT, true);

    if(s == NULL) {
        return;
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED);
    s->value.integer = Value;

    if(s->callback) {
        s->callback(Setting);
    }
}

unsigned a_settings_intuGet(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_INTU, false);

    if(s == NULL) {
        return 0;
    }

    return s->value.integeru;
}

void a_settings_intuSet(ASettingId Setting, unsigned Value)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_INTU, true);

    if(s == NULL) {
        return;
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED);
    s->value.integeru = Value;

    if(s->callback) {
        s->callback(Setting);
    }
}

const char* a_settings_stringGet(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_STR, false);

    if(s == NULL) {
        return 0;
    }

    return s->value.string;
}

void a_settings_stringSet(ASettingId Setting, const char* Value)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_STR, true);

    if(s == NULL) {
        return;
    }

    if(A_FLAG_TEST_ANY(s->flags, A__SETTING_FLAG_FREE_STRING)) {
        free(s->value.string);
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED | A__SETTING_FLAG_FREE_STRING);
    s->value.string = a_str_dup(Value);

    if(s->callback) {
        s->callback(Setting);
    }
}

APixel a_settings_colorGet(ASettingId Setting)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_COLOR, false);

    if(s == NULL) {
        return 0;
    }

    return s->value.pixel;
}

void a_settings_colorSet(ASettingId Setting, uint32_t Hexcode)
{
    ASetting* s = validate(Setting, A__SETTING_TYPE_COLOR, true);

    if(s == NULL) {
        return;
    }

    A_FLAG_SET(s->flags, A__SETTING_FLAG_CHANGED);
    s->value.pixel = a_pixel_fromHex(Hexcode);

    if(s->callback) {
        s->callback(Setting);
    }
}
