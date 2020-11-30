"""
    Copyright 2016-2020 Alex Margarit <alex@alxm.org>
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

class Args:
    def __init__(self, Tool, ArgNames, FlagNames):
        self.__tool = Tool
        self.arg_names = ArgNames.split()
        self.arg_db = {}
        self.flag_names = ['-q'] + FlagNames.split()
        self.flag_db = {}

    def init(self, Argv):
        required_num = 0
        optional_num = 0
        has_tail = False
        arg_names = self.arg_names

        arg_values = [a.strip() for a in Argv]
        arg_values = [a for a in arg_values if len(a) > 0]

        for value in arg_values:
            if value in self.flag_names:
                self.flag_db[value] = True
                arg_values = arg_values[1 : ]
            else:
                break

        for name in arg_names:
            if has_tail:
                self.__tool.out.error('... must be the last argument')
            elif name == '...':
                if optional_num > 0:
                    self.__tool.out.error(
                        'Cannot have both optional args and ...')

                has_tail = True
            elif name[0] == '[' and name[-1] == ']':
                optional_num += 1
            elif optional_num > 0:
                self.__tool.out.error(
                    'Cannot pass required args after optional args')
            else:
                required_num += 1

        if len(arg_values) < required_num:
            self.__tool.usage('All required arguments must be present')

        if has_tail:
            arg_names = arg_names[ : -1]
            self.arg_db['...'] = arg_values[required_num : ]

        for name, value in zip(arg_names, arg_values):
            self.arg_db[name] = value

    def usage(self):
        message = ''

        for flag in self.flag_names:
            message += f' [{flag}]'

        for arg in self.arg_names:
            message += f' {arg}'

        return message

    def get(self, Name):
        if Name in self.arg_db:
            return self.arg_db[Name]
        else:
            return ''

    def get_flag(self, Flag):
        return Flag in self.flag_db
