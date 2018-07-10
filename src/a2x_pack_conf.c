/*
    Copyright 2010, 2016 Alex Margarit

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

#include "a2x_system_includes.h"
#include "a2x_pack_conf.v.h"

#include "a2x_pack_file.v.h"
#include "a2x_pack_out.v.h"
#include "a2x_pack_settings.v.h"
#include "a2x_pack_str.v.h"

void a_conf__init(void)
{
    const char* conf_name = a_settings_getString("app.conf");

    if(!a_file_exists(conf_name)) {
        return;
    }

    AFile* f = a_file_open(conf_name, "r");

    if(!f) {
        return;
    }

    a_out__message("You can edit config in %s", conf_name);

    while(a_file_lineRead(f)) {
        char* key = NULL;
        char* value = NULL;
        char* line = a_str_trim(a_file_lineGet(f));

        if(strlen(line) >= 2 && line[0] == '/' && line[1] == '/') {
            goto next;
        }

        key = a_str_getPrefixFirstFind(line, '=');
        value = a_str_getSuffixFirstFind(line, '=');

        if(key && value) {
            char* key_trim = a_str_trim(key);
            char* val_trim = a_str_trim(value);

            a_settings_set(key_trim, val_trim);

            free(key_trim);
            free(val_trim);
        }

next:
        free(key);
        free(value);
        free(line);
    }

    a_file_close(f);
}
