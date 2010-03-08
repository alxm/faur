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

#include "a2x_app_settings.p.h"
#include "a2x_app_settings.v.h"

typedef enum Setting_t {
    A_STR, A_BOOL, A_INT
} Setting_t;

typedef enum Update_t {
    A_CONSTANT, A_UPDATE, A_BLOCKED
} Update_t;

typedef struct Setting {
    Setting_t type;
    Update_t update;

    union {
        int aInt;
        int aBool;
        String64 aStr;
    } value;
} Setting;

static HashTable* settings;

static void add(Setting_t const type, const Update_t update, const char* const key, const char* const val);
static int parseBool(const char* const val);

void a2x_defaults(void)
{
    settings = a_hash_set(32);

    add(A_STR, A_CONSTANT, "title", "Untitled");
    add(A_STR, A_CONSTANT, "version", "0");
    add(A_STR, A_CONSTANT, "author", "Unknown");
    add(A_STR, A_CONSTANT, "compiled", "?");
    add(A_STR, A_CONSTANT, "conf", "a2x.cfg");

    add(A_BOOL, A_UPDATE, "quiet", "0");
    add(A_BOOL, A_CONSTANT, "window", "0");
    add(A_BOOL, A_CONSTANT, "tool", "0");
    add(A_BOOL, A_CONSTANT, "gp2xMenu", "0");
    add(A_BOOL, A_UPDATE, "trackFps", "0");
    add(A_BOOL, A_UPDATE, "sound", "0");
    add(A_BOOL, A_UPDATE, "trackMouse", "0");
    add(A_BOOL, A_CONSTANT, "fakeScreen", "0");
    add(A_BOOL, A_CONSTANT, "fixWizTear", "0");

    add(A_INT, A_UPDATE, "mhz", "0");
    add(A_INT, A_CONSTANT, "width", "0");
    add(A_INT, A_CONSTANT, "height", "0");
    add(A_INT, A_CONSTANT, "fps", "60");
    add(A_INT, A_UPDATE, "musicScale", "100");
    add(A_INT, A_UPDATE, "sfxScale", "100");
}

void a2x_set(const char* const key, const char* const val)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return;
    } else if(s->update == A_BLOCKED) {
        a_error("Setting '%s' is constant", key);
        return;
    }

    switch(s->type) {
        case A_INT: {
            s->value.aInt = atoi(val);
        } break;

        case A_BOOL: {
            s->value.aBool = parseBool(val);
        } break;

        case A_STR: {
            strncpy(s->value.aStr, val, 63);
        } break;
    }

    if(s->update == A_CONSTANT) {
        s->update = A_BLOCKED;
    }
}

char* a2x_str(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return NULL;
    } else if(s->type != A_STR) {
        a_error("Setting '%s' is not a string", key);
        return NULL;
    } else {
        return s->value.aStr;
    }
}

int a2x_bool(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return 0;
    } else if(s->type != A_BOOL) {
        a_error("Setting '%s' is not a boolean", key);
        return 0;
    } else {
        return s->value.aBool;
    }
}

int a2x_int(const char* const key)
{
    Setting* const s = a_hash_get(settings, key);

    if(s == NULL) {
        a_error("Setting '%s' does not exist", key);
        return 0;
    } else if(s->type != A_INT) {
        a_error("Setting '%s' is not an integer", key);
        return 0;
    } else {
        return s->value.aInt;
    }
}

static void add(Setting_t const type, const Update_t update, const char* const key, const char* const val)
{
    Setting* const s = malloc(sizeof(Setting));

    s->type = type;
    s->update = update;

    switch(type) {
        case A_INT: {
            s->value.aInt = atoi(val);
        } break;

        case A_BOOL: {
            s->value.aBool = parseBool(val);
        } break;

        case A_STR: {
            strncpy(s->value.aStr, val, 63);
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
