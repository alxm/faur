"""
    Copyright 2016, 2017 Alex Margarit

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
    def __init__(self, quiet):
        self.quiet = quiet

    def __colored(self, text, color, newline = False):
        print('\033[{}m{}\033[0m'.format(color, text),
              end = '\n' if newline else '')

    def colored(self, text, color):
        if not self.quiet:
            self.__colored(text, color)

    def coloredln(self, text, color):
        if not self.quiet:
            self.__colored(text, color, True)

    def note(self, text):
        if not self.quiet:
            self.__colored('[ Note ] ', Color.LightGreen)
            print(text)

    def info(self, text):
        if not self.quiet:
            self.__colored('[ Info ] ', Color.LightBlue)
            print(text)

    def error(self, text):
        if not self.quiet:
            self.__colored('[ Error ] ', Color.LightRed)
            print(text)

        sys.exit(1)

    def shell(self, text):
        if not self.quiet:
            self.__colored('[ Shell ] ', Color.LightPurple)
            print(text)
