"""
    Copyright 2016 Alex Margarit <alex@alxm.org>
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
"""

import sys

class FColor:
    Black = '0;30'
    DarkGray = '1;30'
    Red = '0;31'
    LightRed = '1;31'
    Green = '0;32'
    LightGreen = '1;32'
    Brown = '0;33'
    Yellow = '1;33'
    Blue = '0;34'
    LightBlue = '1;34'
    Purple = '0;35'
    LightPurple = '1;35'
    Cyan = '0;36'
    LightCyan = '1;36'
    LightGray = '0;37'
    White = '1;37'

class FOutput:
    def __init__(self, Tool):
        self.__tool = Tool
        self.__title = self.__colorfmt(
                        ('[f', FColor.White),
                        ('a', FColor.LightBlue),
                        ('u', FColor.LightGreen),
                        ('r', FColor.Yellow),
                        (f'{self.__tool.name[4 : ]}]', FColor.White))

    def __colorfmt(self, *Args):
        return ''.join(f'\033[{color}m{text}\033[0m' for (text, color) in Args)

    def __worker(self, Tag, TagColor, Text, OutputFile):
        tag = self.__colorfmt((f'[{Tag}]', TagColor))

        for line in Text.splitlines():
            print(f'{self.__title}{tag} {line}', file = OutputFile)

    def note(self, Text):
        self.__worker('Note', FColor.LightGreen, Text, sys.stdout)

    def error(self, Text):
        self.__worker('Error', FColor.LightRed, Text, sys.stderr)
        sys.exit(1)

    def shell(self, Text):
        self.__worker('Shell', FColor.LightPurple, Text, sys.stdout)
