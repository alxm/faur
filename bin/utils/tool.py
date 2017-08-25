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

import os
import subprocess
import sys

from utils.output import Output, Color

class Tool:
    def __init__(self, arg_names):
        quiet = False
        self.args = sys.argv[1 : ]

        if len(self.args) > 0 and self.args[0] == '-q':
            quiet = True
            self.args = self.args[1 : ]

        self.arg_names = arg_names.split()
        self.args_db = {}
        self.name = os.path.basename(sys.argv[0])
        self.output = Output(quiet)

        current_dir = os.path.dirname(__file__)
        self.dir_a2x = os.path.abspath(os.path.join(current_dir, '..', '..'))
        self.dir_bin = os.path.join(self.dir_a2x, 'bin')
        self.dir_cfg = os.path.join(self.dir_a2x, 'cfg')
        self.dir_make = os.path.join(self.dir_a2x, 'make')
        self.dir_src = os.path.join(self.dir_a2x, 'src')

    def title(self):
        arguments = ' '.join(self.args) + ' ' if len(self.args) > 0 else ''
        whole_text = ' {} {}'.format(self.name, arguments)
        border = '-' * len(whole_text)

        self.output.coloredln(border, Color.DarkGray)
        self.output.colored(' a', Color.LightBlue)
        self.output.colored('2', Color.LightGreen)
        self.output.colored('x', Color.Yellow)
        self.output.colored('{} '.format(self.name[3 : ]), Color.White)
        print(arguments)
        self.output.coloredln(border, Color.DarkGray)

    def done(self):
        self.output.coloredln('[ Done ]', Color.LightGreen)

    def usage(self):
        message = 'Usage: {}'.format(self.name)

        for arg in self.arg_names:
            message += ' {}'.format(arg)

        self.output.error(message)

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
            return ''

    def main(self):
        self.output.error('{} does not implement main'.format(self.name))

    def run(self):
        self.title()
        self.validate()
        self.main()
        self.done()

    def makedir(self, name):
        self.output.info('Making dir {}'.format(name))
        os.makedirs(name)

    def symlink(self, Target, Name):
        self.output.info('New symlink {} to {}'.format(Name, Target))
        os.symlink(Target, Name)

    def writefile(self, name, contents):
        self.output.info('Writing file {}'.format(name))

        with open(name, 'w') as f:
            f.write(contents)

    def readbytes(self, name):
        self.output.info('Reading bytes from {}'.format(name))

        with open(name, 'rb') as f:
            return f.read()

    def readtext(self, name):
        self.output.info('Reading text from {}'.format(name))

        with open(name, 'rU') as f:
            return f.read()

    def shell(self, cmd):
        self.output.shell(cmd)
        status, output = subprocess.getstatusoutput(cmd)

        if not self.output.quiet:
            for line in output.splitlines():
                print('    {}'.format(line))

        if status != 0:
            sys.exit(status)
