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

import os
import subprocess
import sys

from utils.output import Output

class Tool:
    def __init__(self, ArgNames, FlagNames = ''):
        required_num = 0
        optional_num = 0
        has_tail = False
        arg_names = ArgNames.split()
        flag_names = ['-q'] + FlagNames.split()

        self.name = os.path.basename(sys.argv[0])
        self.out = Output(self)
        self.arg_names = arg_names
        self.arg_db = {}
        self.flag_names = flag_names
        self.flag_db = {}

        arg_values = [a.strip() for a in sys.argv[1 : ]]
        arg_values = [a for a in arg_values if len(a) > 0]

        for value in arg_values:
            if value in flag_names:
                self.flag_db[value] = True
                arg_values = arg_values[1 : ]
            else:
                break

        for name in arg_names:
            if has_tail:
                self.out.error('... must be the last argument')
            elif name == '...':
                if optional_num > 0:
                    self.out.error('Cannot have both optional args and ...')

                has_tail = True
            elif name[0] == '[' and name[-1] == ']':
                optional_num += 1
            elif optional_num > 0:
                self.out.error('Cannot have normal args after optional args')
            else:
                required_num += 1

        if len(arg_values) < required_num:
            self.usage('All required arguments must be present')

        if has_tail:
            arg_names = arg_names[ : -1]
            self.arg_db['...'] = arg_values[required_num : ]

        for name, value in zip(arg_names, arg_values):
            self.arg_db[name] = value

        if 'FAUR_PATH' in os.environ:
            faur_path = os.environ['FAUR_PATH']
        else:
            faur_path = f'{os.path.dirname(__file__)}/../..'

        self.dir_faur = os.path.realpath(
                            os.path.expandvars(os.path.expanduser(faur_path)))

        self.dir_bin = f'{self.dir_faur}/bin'
        self.dir_cfg = f'{os.environ["HOME"]}/.config/faur'
        self.dir_make = f'{self.dir_faur}/make'
        self.dir_src = f'{self.dir_faur}/src'

        if not os.path.exists(self.dir_cfg):
            os.makedirs(self.dir_cfg)
        elif not os.path.isdir(self.dir_cfg):
            self.out.error(f'{self.dir_cfg} is not a dir')

    def usage(self, ErrorMessage = None):
        message = ''

        if ErrorMessage:
            message += ErrorMessage + '\n'

        message += f'Usage: {self.name}'

        for flag in self.flag_names:
            message += f' [{flag}]'

        for arg in self.arg_names:
            message += f' {arg}'

        self.out.error(message)

    def get_cmd(self):
        return ' '.join([os.path.basename(sys.argv[0])] + sys.argv[1 : ])

    def get_arg(self, Name):
        if Name in self.arg_db:
            return self.arg_db[Name]
        else:
            return ''

    def get_flag(self, Flag):
        return Flag in self.flag_db

    def write_bytes(self, Name, Contents):
        with open(Name, 'wb') as f:
            f.write(bytes(Contents))

    def write_text(self, Name, Contents):
        with open(Name, 'w') as f:
            f.write(Contents)

    def read_bytes(self, Name):
        with open(Name, 'rb') as f:
            return f.read()

    def read_text(self, Name):
        with open(Name, 'rU') as f:
            return f.read()

    def read_text_lines(self, Name):
        with open(Name, 'rU') as f:
            return f.readlines()

    def list_dir(self, Path):
        if not os.path.isdir(Path):
            self.out.error(f'{Path} is not a dir')

        entries = os.listdir(Path)
        entries.sort()

        return entries

    def check_files_exist(self, *Paths):
        for f in Paths:
            if not os.path.exists(f):
                self.out.error(f'{f} does not exist')

    def check_files_not_exist(self, *Paths):
        for f in Paths:
            if os.path.exists(f):
                self.out.error(f'{f} already exists')

    def shell(self, Cmd):
        self.out.shell(Cmd)
        status, output = subprocess.getstatusoutput(Cmd)

        for line in output.splitlines():
            self.out.shell(f'    {line}')

        if status != 0:
            sys.exit(status)

    def sanitize_c_var(self, Name):
        return Name.replace('.', '_').replace('-', '_').replace('/', '_')
