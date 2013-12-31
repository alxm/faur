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

#include "a2x_app_settings.v.h"

typedef enum Setting_t {
    STR, BOOL, INT
} Setting_t;

typedef enum Update_t {
    SET_ONCE, SET_ANY, SET_FROZEN
} Update_t;

typedef struct Setting {
    Setting_t type;
    Update_t update;

    union {
        int integer;
        bool boolean;
        char string[64];
    } value;
} Setting;

static StrHash* settings;
static bool frozen = false;

static void add(Setting_t type, Update_t update, const char* key, const char* val);
static int parseBool(const char* val);
static void set(const char* key, const char* val, bool respect);
static bool flip(const char* key, bool respect);

void a_settings__defaults(void)
{
    settings = a_strhash_new();

    add(STR, SET_ONCE, "app.title", "Untitled");
    add(STR, SET_ONCE, "app.version", "0");
    add(STR, SET_ONCE, "app.author", "Unknown");
    add(STR, SET_ONCE, "app.compiled", "?");
    add(STR, SET_ONCE, "app.conf", "a2x.cfg");
    add(BOOL, SET_ANY, "app.quiet", "0");
    add(BOOL, SET_ONCE, "app.tool", "0");
    add(BOOL, SET_ONCE, "app.gp2xMenu", "0");
    add(INT, SET_ANY, "app.mhz", "0");

    add(BOOL, SET_ONCE, "video.window", "1");
    add(INT, SET_ONCE, "video.width", "320");
    add(INT, SET_ONCE, "video.height", "240");
    add(BOOL, SET_ONCE, "video.fake", "0");
    add(INT, SET_ONCE, "video.scale", "1");
    add(BOOL, SET_ANY, "video.fullscreen", "0");
    add(BOOL, SET_ONCE, "video.wizTear", "0");

    add(BOOL, SET_ANY, "sound.on", "0");
    add(INT, SET_ANY, "sound.music.scale", "100");
    add(INT, SET_ANY, "sound.sfx.scale", "100");

    add(BOOL, SET_ANY, "input.trackMouse", "0");

    add(INT, SET_ONCE, "fps.rate", "60");
    add(BOOL, SET_ANY, "fps.track", "0");

    add(STR, SET_ONCE, "screenshot.dir", "./screenshots");
    add(STR, SET_ONCE, "screenshot.button", "pc.F12, pandora.s");
}

void a_settings__freeze(void)
{
    frozen = true;
}

void a2x_set(const char* key, const char* val)
{
    set(key, val, true);
}

void a2x__set(const char* key, const char* val)
{
    set(key, val, false);
}

bool a2x_flip(const char* key)
{
    return flip(key, true);
}

bool a2x__flip(const char* key)
{
    return flip(key, false);
}

char* a2x_str(const char* key)
{
    Setting* const s = a_strhash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return NULL;
    } else if(s->type != STR) {
        a_error("Setting '%s' is not a string", key);
        return NULL;
    } else {
        return s->value.string;
    }
}

bool a2x_bool(const char* key)
{
    Setting* const s = a_strhash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return false;
    } else if(s->type != BOOL) {
        a_error("Setting '%s' is not a boolean", key);
        return false;
    } else {
        return s->value.boolean;
    }
}

int a2x_int(const char* key)
{
    Setting* const s = a_strhash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return 0;
    } else if(s->type != INT) {
        a_error("Setting '%s' is not an integer", key);
        return 0;
    } else {
        return s->value.integer;
    }
}

static void add(Setting_t type, Update_t update, const char* key, const char* val)
{
    Setting* const s = malloc(sizeof(Setting));

    s->type = type;
    s->update = update;

    switch(type) {
        case INT: {
            s->value.integer = atoi(val);
        } break;

        case BOOL: {
            s->value.boolean = parseBool(val);
        } break;

        case STR: {
            strncpy(s->value.string, val, 63);
        } break;
    }

    a_strhash_add(settings, key, s);
}

static int parseBool(const char* val)
{
    return a_str_same(val, "yes")
        || a_str_same(val, "y")
        || a_str_same(val, "true")
        || a_str_same(val, "t")
        || a_str_same(val, "da")
        || a_str_same(val, "on")
        || a_str_same(val, "1");
}

static void set(const char* key, const char* val, bool respect)
{
    Setting* const s = a_strhash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return;
    } else if(respect
        && (s->update == SET_FROZEN || (s->update == SET_ONCE && frozen))) {
        a_error("Setting '%s' is frozen", key);
        return;
    }

    switch(s->type) {
        case INT: {
            s->value.integer = atoi(val);
        } break;

        case BOOL: {
            s->value.boolean = parseBool(val);
        } break;

        case STR: {
            strncpy(s->value.string, val, sizeof(s->value.string) - 1);
        } break;
    }

    if(s->update == SET_ONCE) {
        s->update = SET_FROZEN;
    }
}

static bool flip(const char* key, bool respect)
{
    Setting* const s = a_strhash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return false;
    } else if(s->type != BOOL) {
        a_error("Setting '%s' is not a boolean - can't flip it", key);
        return false;
    } else if(respect
        && (s->update == SET_FROZEN || (s->update == SET_ONCE && frozen))) {
        a_error("Setting '%s' is frozen", key);
        return false;
    }

    s->value.boolean ^= 1;

    if(s->update == SET_ONCE) {
        s->update = SET_FROZEN;
    }

    return s->value.boolean;
}
