/*
    Copyright 2010, 2016, 2018 Alex Margarit

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

#include "a2x_pack_conf.v.h"

#include "a2x_pack_file.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_str.v.h"

void a_conf__init(void)
{
    const char* conf_name = a_settings_stringGet(A_SETTING_FILE_CONFIG);

    if(!a_file_exists(conf_name)) {
        return;
    }

    AFile* f = a_file_new(conf_name, "r");

    if(f == NULL) {
        return;
    }

    a_out__message("a_conf__init: Edit config file '%s'", conf_name);

    while(a_file_lineRead(f)) {
        char* key = NULL;
        char* value = NULL;
        char* line = a_str_trim(a_file_lineBufferGet(f));

        if(strlen(line) >= 2 && line[0] == '/' && line[1] == '/') {
            free(line);
            continue;
        }

        key = a_str_prefixGetToFirst(line, '=');
        value = a_str_suffixGetFromFirst(line, '=');

        if(key && value) {
            char* key_trim = a_str_trim(key);
            char* val_trim = a_str_trim(value);

            ASettingId id = a_settings__stringToId(key_trim);

            if(id == A_SETTING_INVALID) {
                a_out__error("Unknown setting %s", key_trim);
            } else {
                switch(a_settings__typeGet(id)) {
                    case A__SETTING_TYPE_INT: {
                        a_settings_intSet(id, atoi(val_trim));
                    } break;

                    case A__SETTING_TYPE_INTU: {
                        a_settings_intuSet(id, (unsigned)atoi(val_trim));
                    } break;

                    case A__SETTING_TYPE_BOOL: {
                        a_settings_boolSet(id,
                                           a_str_equal(val_trim, "1")
                                        || a_str_equal(val_trim, "yes")
                                        || a_str_equal(val_trim, "y")
                                        || a_str_equal(val_trim, "true")
                                        || a_str_equal(val_trim, "t")
                                        || a_str_equal(val_trim, "on"));
                    } break;

                    case A__SETTING_TYPE_STR: {
                        a_settings_stringSet(id, val_trim);
                    } break;

                    case A__SETTING_TYPE_PIXEL: {
                        long hex = strtol(val_trim, NULL, 16);
                        a_settings_pixelSet(id, a_pixel_fromHex((uint32_t)hex));
                    } break;

                    default: break;
                }
            }

            free(key_trim);
            free(val_trim);
        }

        free(key);
        free(value);
        free(line);
    }

    a_file_free(f);
}
