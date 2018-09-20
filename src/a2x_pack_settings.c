/*
    Copyright 2010, 2016-2018 Alex Margarit

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

#include "a2x_pack_settings.v.h"

#include "a2x_pack_mem.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_str.v.h"
#include "a2x_pack_strhash.v.h"

typedef enum {
    A_SETTING__INVALID = -1,
    A_SETTING__INT,
    A_SETTING__UINT,
    A_SETTING__BOOL,
    A_SETTING__COLOR,
    A_SETTING__STR,
    A_SETTING__NUM
} ASettingType;

typedef enum {
    A_SETTING__SET_ANY = 0,
    A_SETTING__SET_ONCE = A_UTIL_BIT(0),
    A_SETTING__SET_FROZEN = A_UTIL_BIT(1),
    A_SETTING__SET_USER = A_UTIL_BIT(2),
} ASettingUpdate;

typedef struct {
    ASettingType type;
    ASettingUpdate update;
    union {
        int integer;
        unsigned uinteger;
        bool boolean;
        APixel pixel;
        char* string;
    } value;
} ASetting;

static AStrHash* g_settings; // table of ASetting
static const char* g_settingTypeNames[A_SETTING__NUM + 1] = {
    "int",
    "uint",
    "bool",
    "color",
    "str",
    "invalid"
};

static bool parseBool(const char* Value)
{
    return a_str_equal(Value, "yes")
        || a_str_equal(Value, "y")
        || a_str_equal(Value, "true")
        || a_str_equal(Value, "t")
        || a_str_equal(Value, "da")
        || a_str_equal(Value, "on")
        || a_str_equal(Value, "1");
}

static ASetting* getValidate(const char* Key, ASettingType Type)
{
    ASetting* s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
    } else if(s->type != Type) {
        a_out__error("Setting '%s' is %s, not %s",
                     Key,
                     g_settingTypeNames[s->type],
                     g_settingTypeNames[Type]);
        s = NULL;
    }

    return s;
}

static ASetting* setValidate(const char* Key, bool UserSet)
{
    ASetting* s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
    } else if(UserSet && s->update & A_SETTING__SET_FROZEN) {
        a_out__error("Setting '%s' is frozen", Key);
        s = NULL;
    } else if(!UserSet && s->update & A_SETTING__SET_USER) {
        a_out__warning("Cannot overwrite user-set '%s'", Key);
        s = NULL;
    }

    return s;
}

static void add(ASettingType Type, ASettingUpdate Update, const char* Key, const char* Value)
{
    ASetting* s = a_mem_malloc(sizeof(ASetting));

    s->type = Type;
    s->update = Update;

    switch(Type) {
        case A_SETTING__INT: {
            s->value.integer = atoi(Value);
        } break;

        case A_SETTING__UINT: {
            s->value.uinteger = (unsigned)atoi(Value);
        } break;

        case A_SETTING__BOOL: {
            s->value.boolean = parseBool(Value);
        } break;

        case A_SETTING__COLOR: {
            s->value.pixel = a_pixel_fromHex((uint32_t)strtol(Value, NULL, 16));
        } break;

        case A_SETTING__STR: {
            s->value.string = a_str_dup(Value);
        } break;

        default: {
            a_out__error("Setting '%s' has invalid type", Key);
        } return;
    }

    a_strhash_add(g_settings, Key, s);
}

static void set(const char* Key, const char* Value, bool UserSet)
{
    ASetting* s = setValidate(Key, UserSet);

    if(s == NULL) {
        return;
    }

    switch(s->type) {
        case A_SETTING__INT: {
            s->value.integer = atoi(Value);
        } break;

        case A_SETTING__UINT: {
            int x = atoi(Value);

            if(x < 0) {
                a_out__error("Invalid value '%s' for setting '%s'", Value, Key);
            }

            s->value.uinteger = (unsigned)x;
        } break;

        case A_SETTING__BOOL: {
            s->value.boolean = parseBool(Value);
        } break;

        case A_SETTING__COLOR: {
            s->value.pixel = a_pixel_fromHex((uint32_t)strtol(Value, NULL, 16));
        } break;

        case A_SETTING__STR: {
            free(s->value.string);
            s->value.string = a_str_dup(Value);
        } break;

        default: {
            a_out__error("Setting '%s' has invalid type", Key);
        } return;
    }

    if(s->update & A_SETTING__SET_ONCE) {
        s->update |= A_SETTING__SET_FROZEN;
    }

    if(UserSet) {
        s->update |= A_SETTING__SET_USER;
    }
}

static bool flip(const char* Key, bool UserSet)
{
    ASetting* s = setValidate(Key, UserSet);

    if(s == NULL) {
        return false;
    } else if(s->type != A_SETTING__BOOL) {
        a_out__error("Setting '%s' is not a boolean - can't flip it", Key);
        return false;
    }

    s->value.boolean = !s->value.boolean;

    if(s->update & A_SETTING__SET_ONCE) {
        s->update |= A_SETTING__SET_FROZEN;
    }

    if(UserSet) {
        s->update |= A_SETTING__SET_USER;
    }

    return s->value.boolean;
}

void a_settings__init(void)
{
    g_settings = a_strhash_new();
    extern const char* a_app__buildtime;

    add(A_SETTING__STR, A_SETTING__SET_ONCE, "app.title", "Untitled");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "app.version", "0");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "app.author", "(unknown)");
    add(A_SETTING__STR, A_SETTING__SET_FROZEN, "app.buildtime", a_app__buildtime);
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "app.conf", "a2x.cfg");
    add(A_SETTING__BOOL, A_SETTING__SET_ANY, "app.output.on", "1");
    add(A_SETTING__BOOL, A_SETTING__SET_ANY, "app.output.verbose", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "app.tool", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "app.gp2xMenu", "0");
    add(A_SETTING__UINT, A_SETTING__SET_ONCE, "app.mhz", "0");

    add(A_SETTING__UINT, A_SETTING__SET_ONCE, "fps.tick", "30");
    add(A_SETTING__UINT, A_SETTING__SET_ONCE, "fps.draw", "30");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "fps.draw.skip", "0");
    add(A_SETTING__UINT, A_SETTING__SET_ONCE, "fps.draw.skip.max", "2");

    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "video.on", "1");
    add(A_SETTING__INT, A_SETTING__SET_ONCE, "video.width", "320");
    add(A_SETTING__INT, A_SETTING__SET_ONCE, "video.height", "240");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "video.vsync", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "video.doubleBuffer", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "video.fullscreen", "0");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "video.fullscreen.button", "key.f4");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "video.fixWizTearing", "0");
    add(A_SETTING__COLOR, A_SETTING__SET_ONCE, "video.color.border", "0x1f0f0f");
    add(A_SETTING__COLOR, A_SETTING__SET_ONCE, "video.color.key", "0xFF00FF");
    add(A_SETTING__COLOR, A_SETTING__SET_ONCE, "video.color.limit", "0x00FF00");
    add(A_SETTING__COLOR, A_SETTING__SET_ONCE, "video.color.end", "0x00FFFF");

    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "sound.on", "1");
    add(A_SETTING__INT, A_SETTING__SET_ANY, "sound.music.scale", "100");
    add(A_SETTING__INT, A_SETTING__SET_ANY, "sound.sample.scale", "100");
    add(A_SETTING__INT, A_SETTING__SET_ONCE, "sound.sample.channels.total", "64");
    add(A_SETTING__INT, A_SETTING__SET_ONCE, "sound.sample.channels.reserved", "32");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "sound.volbar.background", "0x1f0f0f");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "sound.volbar.border", "0x3f8fdf");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "sound.volbar.fill", "0x9fcf3f");

    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "input.hideCursor", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ANY, "input.trackMouse", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "input.switchAxes", "0");
    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "input.invertAxes", "0");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "input.mapfile", "gamecontrollerdb.txt");

    add(A_SETTING__STR, A_SETTING__SET_ONCE, "screenshot.dir", "./screenshots");
    add(A_SETTING__STR, A_SETTING__SET_ONCE, "screenshot.button", "key.f12");

    add(A_SETTING__BOOL, A_SETTING__SET_ONCE, "console.on", "0");
    add(A_SETTING__STR,
        A_SETTING__SET_ONCE,
        "console.button",
        "key.f11 gamepad.b.l+gamepad.b.r+gamepad.b.a+gamepad.b.b+gamepad.b.x+gamepad.b.y");

    a_settings__application();

    A_STRHASH_ITERATE(g_settings, ASetting*, s) {
        if(s->update & A_SETTING__SET_ONCE) {
            s->update |= A_SETTING__SET_FROZEN;
        }
    }

    #if A_BUILD_LIB_SDL == 2
        a_settings__set("video.doubleBuffer", "1");
        a_settings__set("video.vsync", "1");
    #endif

    #if A_BUILD_SYSTEM_EMSCRIPTEN
        a_settings__set("video.vsync", "1");
    #endif

    #if A_BUILD_SYSTEM_GP2X || A_BUILD_SYSTEM_WIZ || A_BUILD_SYSTEM_CAANOO || A_BUILD_SYSTEM_PANDORA
        a_settings__set("video.fullscreen", "1");

        #if A_BUILD_SYSTEM_WIZ
            if(a_settings_getBool("video.fixWizTearing")) {
                a_settings__set("video.doubleBuffer", "1");
            }
        #endif
    #endif
}

void a_settings__uninit(void)
{
    A_STRHASH_ITERATE(g_settings, ASetting*, s) {
        if(s->type == A_SETTING__STR) {
            free(s->value.string);
        }

        free(s);
    }

    a_strhash_free(g_settings);
}

void a_settings_set(const char* Key, const char* Value)
{
    set(Key, Value, true);
}

void a_settings__set(const char* Key, const char* Value)
{
    set(Key, Value, false);
}

bool a_settings_flip(const char* Key)
{
    return flip(Key, true);
}

bool a_settings__flip(const char* Key)
{
    return flip(Key, false);
}

const char* a_settings_getString(const char* Key)
{
    ASetting* s = getValidate(Key, A_SETTING__STR);

    return s ? s->value.string : "";
}

bool a_settings_getBool(const char* Key)
{
    ASetting* s = getValidate(Key, A_SETTING__BOOL);

    return s ? s->value.boolean : false;
}

int a_settings_getInt(const char* Key)
{
    ASetting* s = getValidate(Key, A_SETTING__INT);

    return s ? s->value.integer : 0;
}

unsigned a_settings_getUnsigned(const char* Key)
{
    ASetting* s = getValidate(Key, A_SETTING__UINT);

    return s ? s->value.uinteger : 0;
}

APixel a_settings_getPixel(const char* Key)
{
    ASetting* s = getValidate(Key, A_SETTING__COLOR);

    return s ? s->value.pixel : 0;
}
