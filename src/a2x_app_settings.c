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

#include "a2x_app_settings.h"

A2xSettings a2xSet;

void a2x_defaults(void)
{
    #define copy(s1, s2) ({ strncpy(s1, s2, 63); s1[63] = '\0'; })

    copy(a2xSet.title, "Untitled");
    copy(a2xSet.version, "0");
    copy(a2xSet.author, "Unknown");
    copy(a2xSet.compiled, "?");
    copy(a2xSet.conf, "a2x.cfg");
    a2xSet.quiet = 0;
    a2xSet.window = 0;
    a2xSet.gp2xMenu = 1;
    a2xSet.mhz = 0;
    a2xSet.width = 0;
    a2xSet.height = 0;
    a2xSet.fps = 60;
    a2xSet.trackFps = 0;
    a2xSet.sound = 0;
    a2xSet.musicScale = 100;
    a2xSet.sfxScale = 100;
    a2xSet.trackMouse = 0;
    a2xSet.fakeScreen = 0;
    a2xSet.fixWizTear = 0;
}

void a2x_set(const char* const var, const char* const val)
{
    #define str(variable)                      \
    ({                                         \
        if(a_str_same(var, #variable)) {       \
            strncpy(a2xSet.variable, val, 63); \
            a2xSet.variable[63] = '\0';        \
            return;                            \
        }                                      \
    })

    #define num(variable)                \
    ({                                   \
        if(a_str_same(var, #variable)) { \
            a2xSet.variable = atoi(val); \
            return;                      \
        }                                \
    })

    #define boo(variable)                  \
    ({                                     \
        if(a_str_same(var, #variable)) {   \
            a2xSet.variable =              \
                   a_str_same(val, "yes")  \
                || a_str_same(val, "y")    \
                || a_str_same(val, "true") \
                || a_str_same(val, "t")    \
                || a_str_same(val, "on")   \
                || a_str_same(val, "1")    \
                ;                          \
            return;                        \
        }                                  \
    })

    str(title);
    str(version);
    str(author);
    str(compiled);
    str(conf);
    boo(quiet);
    boo(window);
    boo(gp2xMenu);
    num(mhz);
    num(width);
    num(height);
    num(fps);
    boo(trackFps);
    boo(sound);
    num(musicScale);
    num(sfxScale);
    boo(trackMouse);
    boo(fakeScreen);
    boo(fixWizTear);
}
