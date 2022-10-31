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

import os

class Files:
    def __init__(self, Tool):
        self.__tool = Tool

        if 'FAUR_PATH' in os.environ:
            faur_path = os.environ['FAUR_PATH']
        else:
            faur_path = f'{os.path.dirname(__file__)}/../..'

        self.dir_faur = os.path.realpath(
                            os.path.expandvars(os.path.expanduser(faur_path)))

        self.dir_bin = f'{self.dir_faur}/bin'
        self.dir_cfg = f'{os.environ["HOME"]}/.config/faur'
        self.dir_src = f'{self.dir_faur}/src'

    def init(self):
        if not os.path.exists(self.dir_cfg):
            os.makedirs(self.dir_cfg)
        elif not os.path.isdir(self.dir_cfg):
            self.__tool.out.error(f'{self.dir_cfg} is not a dir')

    def assert_exist(self, *Paths):
        for f in Paths:
            if not os.path.exists(f):
                self.__tool.out.error(f'{f} does not exist')

    def assert_not_exist(self, *Paths):
        for f in Paths:
            if os.path.exists(f):
                self.__tool.out.error(f'{f} already exists')

    def list_dir(self, Path):
        if not os.path.isdir(Path):
            self.__tool.out.error(f'{Path} is not a dir')

        entries = os.listdir(Path)
        entries.sort()

        return entries

    def read_bytes(self, Name):
        with open(Name, 'rb') as f:
            return f.read()

    def read_text(self, Name):
        with open(Name, 'r') as f:
            return f.read()

    def read_text_lines(self, Name):
        with open(Name, 'r') as f:
            return f.readlines()

    def write_bytes(self, Name, Contents):
        with open(Name, 'wb') as f:
            f.write(bytes(Contents))

    def write_text(self, Name, Contents):
        with open(Name, 'w') as f:
            f.write(Contents)
