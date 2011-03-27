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
        int boolean;
        char string[64];
    } value;
} Setting;

static Hash* settings;

static void add(Setting_t const type, const Update_t update, const char* const key, const char* const val);
static int parseBool(const char* const val);
static void set(const char* const key, const char* const val, const int respect);
static void flip(const char* const key, const int respect);

void a2x__defaults(void)
{
    settings = a_hash_set();

    add(STR, SET_ONCE, "title", "Untitled");
    add(STR, SET_ONCE, "version", "0");
    add(STR, SET_ONCE, "author", "Unknown");
    add(STR, SET_ONCE, "compiled", "?");
    add(STR, SET_ONCE, "conf", "a2x.cfg");

    add(BOOL, SET_ANY, "quiet", "0");
    add(BOOL, SET_ONCE, "window", "0");
    add(BOOL, SET_ONCE, "tool", "0");
    add(BOOL, SET_ONCE, "gp2xMenu", "0");
    add(BOOL, SET_ANY, "trackFps", "0");
    add(BOOL, SET_ANY, "sound", "0");
    add(BOOL, SET_ANY, "trackMouse", "0");
    add(BOOL, SET_ONCE, "fakeScreen", "0");
    add(BOOL, SET_ONCE, "doubleRes", "0");
    add(BOOL, SET_ONCE, "fixWizTear", "0");

    add(INT, SET_ANY, "mhz", "0");
    add(INT, SET_ONCE, "width", "320");
    add(INT, SET_ONCE, "height", "240");
    add(INT, SET_ONCE, "fps", "60");
    add(INT, SET_ANY, "musicScale", "100");
    add(INT, SET_ANY, "sfxScale", "100");
}

void a2x_set(const char* const key, const char* const val)
{
    set(key, val, 1);
}

void a2x__set(const char* const key, const char* const val)
{
    set(key, val, 0);
}

void a2x_flip(const char* const key)
{
    flip(key, 1);
}

void a2x__flip(const char* const key)
{
    flip(key, 0);
}

char* a2x_str(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

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

int a2x_bool(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return 0;
    } else if(s->type != BOOL) {
        a_error("Setting '%s' is not a boolean", key);
        return 0;
    } else {
        return s->value.boolean;
    }
}

int a2x_int(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

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

static void add(Setting_t const type, const Update_t update, const char* const key, const char* const val)
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

    a_hash_add(settings, key, s);
}

static int parseBool(const char* const val)
{
    return a_str_same(val, "yes")
        || a_str_same(val, "y")
        || a_str_same(val, "true")
        || a_str_same(val, "t")
        || a_str_same(val, "da")
        || a_str_same(val, "on")
        || a_str_same(val, "1");
}

static void set(const char* const key, const char* const val, const int respect)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return;
    } else if(s->update == SET_FROZEN && respect) {
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

static void flip(const char* const key, const int respect)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
    } else if(s->type != BOOL) {
        a_error("Setting '%s' is not a boolean - can't flip it", key);
    } else if(s->update == SET_FROZEN && respect) {
        a_error("Setting '%s' is frozen", key);
        return;
    } else {
        s->value.boolean ^= 1;
    }
}
