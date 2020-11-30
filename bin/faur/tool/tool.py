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
import re
import subprocess
import sys

from faur.tool.args import Args
from faur.tool.files import Files
from faur.tool.output import Output

class Tool:
    def __init__(self, ArgNames, FlagNames = ''):
        self.name = os.path.basename(sys.argv[0])
        self.out = Output(self)
        self.args = Args(self, ArgNames, FlagNames)
        self.files = Files(self)
        self.sanitize_pattern = re.compile('[\.\-/]')

        self.args.init(sys.argv[1 : ])
        self.files.init()

    def usage(self, ErrorMessage = None):
        message = ''

        if ErrorMessage:
            message += ErrorMessage + '\n'

        message += f'Usage: {self.name}'
        message += self.args.usage()

        self.out.error(message)

    def get_cmd(self):
        return ' '.join([os.path.basename(sys.argv[0])] + sys.argv[1 : ])

    def run_shell(self, Cmd):
        self.out.shell(Cmd)
        status, output = subprocess.getstatusoutput(Cmd)

        for line in output.splitlines():
            self.out.shell(f'    {line}')

        if status != 0:
            sys.exit(status)

    def sanitize_c_var(self, Name):
        return self.sanitize_pattern.sub('_', Name)
