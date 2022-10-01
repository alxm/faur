/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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
*/

#ifndef F_INC_PLATFORM_FILES_STANDARD_FILE_V_H
#define F_INC_PLATFORM_FILES_STANDARD_FILE_V_H

#include "f_standard_file.p.h"

#include "../f_platform.v.h"
#include "../../files/f_file.v.h"
#include "../../files/f_path.v.h"

extern FCallApi_DirCreate f_platform_api_std__dirCreate;

extern FCallApi_FileStat f_platform_api_std__fileStat;

extern FCallApi_FileBufferRead f_platform_api_std__fileBufferRead;
extern FCallApi_FileBufferWrite f_platform_api_std__fileBufferWrite;

extern FCallApi_FileNew f_platform_api_std__fileNew;
extern FCallApi_FileFree f_platform_api_std__fileFree;

extern FCallApi_FileSeek f_platform_api_std__fileSeek;
extern FCallApi_FileRead f_platform_api_std__fileRead;
extern FCallApi_FileWrite f_platform_api_std__fileWrite;
extern FCallApi_FileWritef f_platform_api_std__fileWritef;

extern FCallApi_FilePrint f_platform_api_std__filePrint;

extern FCallApi_FileFlush f_platform_api_std__fileFlush;

extern FCallApi_FileReadChar f_platform_api_std__fileReadChar;
extern FCallApi_FileReadCharUndo f_platform_api_std__fileReadCharUndo;

#endif // F_INC_PLATFORM_FILES_STANDARD_FILE_V_H
