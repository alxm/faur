"""
    Copyright 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of a2x, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import os
import subprocess
import sys

from utils.output import Output, Color

class Tool:
    def __init__(self, arg_names):
        quiet = False

        args = [a.strip() for a in sys.argv[1 : ]]
        args = [a for a in args if len(a) > 0]

        if len(args) > 0 and args[0] == '-q':
            quiet = True
            args = args[1 : ]

        self.name = os.path.basename(sys.argv[0])
        self.output = Output(self.name, quiet)
        self.arg_names = arg_names.split()
        self.arg_values = args
        self.arg_db = {}
        self.args_tail = self.arg_values[len(self.arg_names) :]

        required_num = 0
        optional_num = 0

        for name in self.arg_names:
            if name[0] == '[' and name[-1] == ']':
                optional_num += 1
            else:
                required_num += 1

                # Optional args are only allowed after all required args
                if optional_num > 0:
                    self.usage()

        if len(self.arg_values) < required_num:
            self.usage()

        for name, value in zip(self.arg_names, self.arg_values):
            self.arg_db[name] = value

        current_dir = os.path.dirname(__file__)
        self.dir_a2x = os.path.abspath(os.path.join(current_dir, '..', '..'))
        self.dir_bin = os.path.join(self.dir_a2x, 'bin')
        self.dir_cfg = os.path.join(os.environ['HOME'], '.config', 'a2x')
        self.dir_make = os.path.join(self.dir_a2x, 'make')
        self.dir_src = os.path.join(self.dir_a2x, 'src')

        if not os.path.exists(self.dir_cfg):
            os.makedirs(self.dir_cfg)
        elif not os.path.isdir(self.dir_cfg):
            self.output.error('{} is not a dir'.format(self.dir_cfg))

    def usage(self):
        message = 'Usage: {}'.format(self.name)

        for arg in self.arg_names:
            message += ' {}'.format(arg)

        self.output.error(message)

    def get_arg(self, name):
        if name in self.arg_db:
            return self.arg_db[name]
        else:
            return ''

    def get_arg_tail(self):
        return self.args_tail

    def main(self):
        self.output.error('{} does not implement main'.format(self.name))

    def writefile(self, name, contents):
        self.output.info('Writing file {}'.format(name))

        with open(name, 'w') as f:
            f.write(contents)

    def readbytes(self, name):
        with open(name, 'rb') as f:
            return f.read()

    def readtext(self, name):
        with open(name, 'rU') as f:
            return f.read()

    def readtextlines(self, name):
        with open(name, 'rU') as f:
            return f.readlines()

    def listdir(self, path):
        if not os.path.isdir(path):
            self.output.error('{} is not a dir'.format(path))

        return sorted(os.listdir(path))

    def shell(self, cmd):
        self.output.shell(cmd)
        status, output = subprocess.getstatusoutput(cmd)

        if not self.output.quiet:
            for line in output.splitlines():
                print('    {}'.format(line))

        if status != 0:
            sys.exit(status)

    def sanitizeFileNameForCVar(self, FileName):
        return FileName.replace('.', '_').replace('-', '_').replace('/', '_')
