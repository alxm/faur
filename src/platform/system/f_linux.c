/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
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

#include "f_linux.v.h"
#include <faur.v.h>

#if A_CONFIG_SYSTEM_LINUX
#include <signal.h>

static void f__term(int Signal)
{
    A_UNUSED(Signal);

    exit(0);
}

void f_platform_linux__init(void)
{
    struct sigaction s;
    memset(&s, 0, sizeof(struct sigaction));

    s.sa_handler = f__term;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;

    if(sigaction(SIGTERM, &s, NULL) != 0) {
        f_out__error("sigaction(SIGTERM) failed");
    }
}
#endif // A_CONFIG_SYSTEM_LINUX
