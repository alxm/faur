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

class FParam:
    def __init__(self, IsOptional, IsList):
        self.is_optional = IsOptional
        self.is_list = IsList
        self.__values = []

    def is_empty(self):
        return len(self.__values) == 0

    def get(self):
        if self.is_list:
            return self.__values
        elif len(self.__values) > 0:
            return self.__values[0]
        else:
            return None

    def set(self, Argument):
        self.__values.append(Argument)

class FArgs:
    def __init__(self, Tool, ParamNames):
        self.__tool = Tool
        self.__params = {}

        for name in ParamNames.split():
            is_optional = False
            is_list = False

            if name[0] == '[' and name[-1] == ']':
                name = name[1 : -1]
                is_optional = True

            if name.endswith('...'):
                name = name[ : -3]
                is_list = True

            self.__params[name] = FParam(is_optional, is_list)

    def init(self, Argv):
        param = None

        for arg in Argv:
            if arg.startswith('--'):
                if arg[2 : ] in self.__params:
                    param = self.__params[arg[2 : ]]
                    continue

            if param is None:
                self.__tool.usage(f'Invalid argument {arg}')

            param.set(arg)

        for name, param in self.__params.items():
            if param.is_empty() and not param.is_optional:
                self.__tool.usage(f'Missing argument --{name}')

    def usage(self):
        values = ['Lorem', 'Ipsum', 'Dolor', 'Sit', 'Amet']
        values_index = 0
        message = ''

        for name, param in self.__params.items():
            message += ' '

            if param.is_optional:
                message += '['

            message += f'--{name} \033[3m{values[values_index]}\033[0m'

            if param.is_list:
                message += '...'

            if param.is_optional:
                message += ']'

            values_index = (values_index + 1) % len(values)

        return message

    def get(self, Name):
        if Name not in self.__params:
            self.__tool.out.error(f'Unknown parameter {Name}')

        return self.__params[Name].get()
