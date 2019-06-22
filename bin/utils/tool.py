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

from utils.output import Output

class Tool:
    def __init__(self, arg_names):
        quiet = False

        args = [a.strip() for a in sys.argv[1 : ]]
        args = [a for a in args if len(a) > 0]

        if len(args) > 0 and args[0] == '-q':
            quiet = True
            args = args[1 : ]

        arg_names = arg_names.split()
        required_num = 0
        optional_num = 0
        has_tail = False

        self.name = os.path.basename(sys.argv[0])
        self.output = Output(self.name, quiet)
        self.arg_db = {}

        for name in arg_names:
            if has_tail:
                self.output.error('... must be the last argument')
            elif name == '...':
                if optional_num > 0:
                    self.output.error('Cannot have both optional args and ...')

                has_tail = True
            elif name[0] == '[' and name[-1] == ']':
                optional_num += 1
            elif optional_num > 0:
                self.output.error('Cannot have normal args after optional args')
            else:
                required_num += 1

        if len(args) < required_num:
            message = 'Usage: {}'.format(self.name)

            for arg in arg_names:
                message += ' {}'.format(arg)

            self.output.error(message)

        if has_tail:
            arg_names = arg_names[ : -1]
            self.arg_db['...'] = args[required_num : ]

        for name, value in zip(arg_names, args):
            self.arg_db[name] = value

        if 'A2X_PATH' in os.environ:
            a2x_path = os.environ['A2X_PATH']
        else:
            a2x_path = os.path.join(os.path.dirname(__file__), '..', '..')

        self.dir_a2x = os.path.realpath(
                        os.path.expandvars(os.path.expanduser(a2x_path)))

        self.dir_bin = os.path.join(self.dir_a2x, 'bin')
        self.dir_cfg = os.path.join(os.environ['HOME'], '.config', 'a2x')
        self.dir_make = os.path.join(self.dir_a2x, 'make')
        self.dir_src = os.path.join(self.dir_a2x, 'src')

        if not os.path.exists(self.dir_cfg):
            os.makedirs(self.dir_cfg)
        elif not os.path.isdir(self.dir_cfg):
            self.output.error('{} is not a dir'.format(self.dir_cfg))

    def exit(self):
        sys.exit(0)

    def get_arg(self, name):
        if name in self.arg_db:
            return self.arg_db[name]
        else:
            return ''

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
                self.output.shell('    {}'.format(line))

        if status != 0:
            sys.exit(status)

    def sanitizeFileNameForCVar(self, FileName):
        return FileName.replace('.', '_').replace('-', '_').replace('/', '_')
