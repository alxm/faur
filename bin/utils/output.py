"""
    Copyright 2016-2017, 2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

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

class Color:
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

class Output:
    def __init__(self, tool):
        self.tool = tool

    def __colored(self, text, color):
        print('\033[{}m{}\033[0m'.format(color, text), end = '')

    def __title(self):
        self.__colored('[', Color.White)
        self.__colored('a', Color.LightBlue)
        self.__colored('2', Color.LightGreen)
        self.__colored('x', Color.Yellow)
        self.__colored('{}]'.format(self.tool.name[3 : ]), Color.White)

    def __worker(self, tag, color, text):
        if not self.tool.get_flag('-q'):
            for line in text.splitlines():
                self.__title()
                self.__colored('{} '.format(tag), color)

                print(line)

    def note(self, text):
        self.__worker('[Note]', Color.LightGreen, text)

    def info(self, text):
        self.__worker('[Info]', Color.LightBlue, text)

    def error(self, text):
        self.__worker('[Error]', Color.LightRed, text)
        sys.exit(1)

    def shell(self, text):
        self.__worker('[Shell]', Color.LightPurple, text)
