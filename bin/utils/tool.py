"""
    Copyright 2016 Alex Margarit

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

import os
import sys

from utils.output import Output, Color

class Tool:
    def __init__(self, arg_names):
        self.args = sys.argv[1 : ]
        self.arg_names = arg_names.split()
        self.args_db = {}
        self.name = os.path.basename(sys.argv[0])

        current_dir = os.path.dirname(__file__)
        self.bin_dir = os.path.abspath(os.path.join(current_dir, '..'))
        self.a2x_dir = os.path.abspath(os.path.join(self.bin_dir, '..'))
        self.src_dir = os.path.join(self.a2x_dir, 'src')

    def title(self):
        arguments = ' '.join(self.args) + ' ' if len(self.args) > 0 else ''
        whole_text = ' {} {}'.format(self.name, arguments)
        border = '-' * len(whole_text)

        Output.coloredln(border, Color.DarkGray)
        Output.colored(' a', Color.LightBlue)
        Output.colored('2', Color.LightGreen)
        Output.colored('x', Color.Yellow)
        Output.colored('{} '.format(self.name[3 : ]), Color.White)
        print(arguments)
        Output.coloredln(border, Color.DarkGray)

    def usage(self):
        message = 'Usage: {}'.format(self.name)

        for arg in self.arg_names:
            message += ' {}'.format(arg)

        Output.error(message)

    def validate(self):
        required_num = 0
        optional_num = 0

        for name in self.arg_names:
            if name[0] == '[' and name[-1] == ']':
                optional_num += 1
            else:
                required_num += 1

                # Optional args are only allowed after required args
                if optional_num > 0:
                    self.usage()

        if not required_num <= len(self.args) <= required_num + optional_num:
            self.usage()

        for name, value in zip(self.arg_names, self.args):
            self.args_db[name] = value

    def get_arg(self, name):
        if name in self.args_db:
            return self.args_db[name]
        else:
            return None

    def main(self):
        Output.error('{} does not implement main'.format(self.name))

    def run(self):
        self.title()
        self.validate()
        self.main()
        Output.coloredln('[ Done ]', Color.LightGreen)

    def makedir(self, name):
        Output.info('Making dir {}'.format(name))
        os.mkdir(name)

    def writefile(self, name, contents):
        Output.info('Writing file {}'.format(name))

        with open(name, 'w') as f:
            f.write(contents)

    def readbytes(self, name):
        Output.info('Reading bytes from {}'.format(name))

        with open(name, 'rb') as f:
            return f.read()

    def readtext(self, name):
        Output.info('Reading text from {}'.format(name))

        with open(name, 'rU') as f:
            return f.read()
