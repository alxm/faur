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

#include "a2x_pack_settings.v.h"

typedef enum ASettingType {
    A_SETTING_STR, A_SETTING_BOOL, A_SETTING_INT
} ASettingType;

typedef enum ASettingUpdate {
    A_SETTING_SET_ONCE, A_SETTING_SET_ANY, A_SETTING_SET_FROZEN
} ASettingUpdate;

typedef struct ASetting {
    ASettingType type;
    ASettingUpdate update;

    union {
        int integer;
        bool boolean;
        char* string;
    } value;
} ASetting;

static AStrHash* g_settings;
static bool g_settingsAreFrozen = false;

static int parseBool(const char* Value)
{
    return a_str_same(Value, "yes")
        || a_str_same(Value, "y")
        || a_str_same(Value, "true")
        || a_str_same(Value, "t")
        || a_str_same(Value, "da")
        || a_str_same(Value, "on")
        || a_str_same(Value, "1");
}

static void add(ASettingType Type, ASettingUpdate Update, const char* Key, const char* DefaultValue)
{
    ASetting* const s = a_mem_malloc(sizeof(ASetting));

    s->type = Type;
    s->update = Update;

    switch(Type) {
        case A_SETTING_INT: {
            s->value.integer = atoi(DefaultValue);
        } break;

        case A_SETTING_BOOL: {
            s->value.boolean = parseBool(DefaultValue);
        } break;

        case A_SETTING_STR: {
            s->value.string = a_str_dup(DefaultValue);
        } break;
    }

    a_strhash_add(g_settings, Key, s);
}

static void set(const char* Key, const char* Value, bool HonorFrozen)
{
    ASetting* const s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
        return;
    } else if(HonorFrozen
        && (s->update == A_SETTING_SET_FROZEN || (s->update == A_SETTING_SET_ONCE && g_settingsAreFrozen))) {
        a_out__error("Setting '%s' is frozen", Key);
        return;
    }

    switch(s->type) {
        case A_SETTING_INT: {
            s->value.integer = atoi(Value);
        } break;

        case A_SETTING_BOOL: {
            s->value.boolean = parseBool(Value);
        } break;

        case A_SETTING_STR: {
            free(s->value.string);
            s->value.string = a_str_dup(Value);
        } break;
    }

    if(s->update == A_SETTING_SET_ONCE) {
        s->update = A_SETTING_SET_FROZEN;
    }
}

static bool flip(const char* Key, bool HonorFrozen)
{
    ASetting* const s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
        return false;
    } else if(s->type != A_SETTING_BOOL) {
        a_out__error("Setting '%s' is not a boolean - can't flip it", Key);
        return false;
    } else if(HonorFrozen
        && (s->update == A_SETTING_SET_FROZEN || (s->update == A_SETTING_SET_ONCE && g_settingsAreFrozen))) {
        a_out__error("Setting '%s' is frozen", Key);
        return false;
    }

    s->value.boolean = !s->value.boolean;

    if(s->update == A_SETTING_SET_ONCE) {
        s->update = A_SETTING_SET_FROZEN;
    }

    return s->value.boolean;
}

void a_settings__init(void)
{
    g_settings = a_strhash_new();
    extern const char* a_app__buildtime;

    add(A_SETTING_STR, A_SETTING_SET_ONCE, "app.title", "Untitled");
    add(A_SETTING_STR, A_SETTING_SET_ONCE, "app.version", "0");
    add(A_SETTING_STR, A_SETTING_SET_ONCE, "app.author", "(unknown)");
    add(A_SETTING_STR, A_SETTING_SET_FROZEN, "app.buildtime", a_app__buildtime);
    add(A_SETTING_STR, A_SETTING_SET_ONCE, "app.conf", "a2x.cfg");
    add(A_SETTING_BOOL, A_SETTING_SET_ANY, "app.output.on", "1");
    add(A_SETTING_BOOL, A_SETTING_SET_ANY, "app.output.verbose", "0");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "app.tool", "0");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "app.gp2xMenu", "0");
    add(A_SETTING_INT, A_SETTING_SET_ONCE, "app.mhz", "0");

    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "video.on", "1");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "video.window", "1");
    add(A_SETTING_INT, A_SETTING_SET_ONCE, "video.width", "320");
    add(A_SETTING_INT, A_SETTING_SET_ONCE, "video.height", "240");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "video.doubleBuffer", "0");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "video.fullscreen", "0");
    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "video.wizTear", "0");

    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "sound.on", "0");
    add(A_SETTING_INT, A_SETTING_SET_ANY, "sound.music.scale", "100");
    add(A_SETTING_INT, A_SETTING_SET_ANY, "sound.sfx.scale", "100");

    add(A_SETTING_BOOL, A_SETTING_SET_ANY, "input.trackMouse", "0");

    add(A_SETTING_INT, A_SETTING_SET_ONCE, "fps.rate", "60");
    add(A_SETTING_BOOL, A_SETTING_SET_ANY, "fps.track", "0");

    add(A_SETTING_STR, A_SETTING_SET_ONCE, "screenshot.dir", "./screenshots");
    add(A_SETTING_STR, A_SETTING_SET_ONCE, "screenshot.button", "pc.F12, pandora.s");

    add(A_SETTING_BOOL, A_SETTING_SET_ONCE, "console.on", "0");
    add(A_SETTING_STR, A_SETTING_SET_ONCE, "console.button", "pc.F11");
}

void a_settings__uninit(void)
{
    A_STRHASH_ITERATE(g_settings, ASetting*, s) {
        if(s->type == A_SETTING_STR) {
            free(s->value.string);
        }

        free(s);
    }

    a_strhash_free(g_settings);
}

void a_settings__freeze(void)
{
    g_settingsAreFrozen = true;

    #if A_USE_LIB_SDL2
        a_settings__set("video.doubleBuffer", "1");
    #endif
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
    ASetting* const s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
        return NULL;
    } else if(s->type != A_SETTING_STR) {
        a_out__error("Setting '%s' is not a string", Key);
        return NULL;
    } else {
        return s->value.string;
    }
}

bool a_settings_getBool(const char* Key)
{
    ASetting* const s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
        return false;
    } else if(s->type != A_SETTING_BOOL) {
        a_out__error("Setting '%s' is not a boolean", Key);
        return false;
    } else {
        return s->value.boolean;
    }
}

int a_settings_getInt(const char* Key)
{
    ASetting* const s = a_strhash_get(g_settings, Key);

    if(s == NULL) {
        a_out__error("Setting '%s' does not exist", Key);
        return 0;
    } else if(s->type != A_SETTING_INT) {
        a_out__error("Setting '%s' is not an integer", Key);
        return 0;
    } else {
        return s->value.integer;
    }
}
