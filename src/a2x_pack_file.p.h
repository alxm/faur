/*
    Copyright 2010 Alex Margarit

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
*/

#ifndef A2X_PACK_FILE_PH
#define A2X_PACK_FILE_PH

#include "a2x_app_includes.h"

typedef FILE File;
typedef DIR Dir;

typedef struct FilePath FilePath;
typedef struct FileReader FileReader;

#include "a2x_pack_list.p.h"

/*#if A_PLATFORM_GP2X || A_PLATFORM_WIZ || A_PLATFORM_PANDORA
    #define a_sync() sync()
#else*/
    #define a_sync()
//#endif

#if A_PLATFORM_WINDOWS
    #define a_file_openRead(path)   fopen((path), "rb")
    #define a_file_openWrite(path)  fopen((path), "wb")
    #define a_file_openAppend(path) fopen((path), "ab")
    #define a_file_openUpdate(path) fopen((path), "r+b")
#else
    #define a_file_openRead(path)   fopen((path), "r")
    #define a_file_openWrite(path)  fopen((path), "w")
    #define a_file_openAppend(path) fopen((path), "a")
    #define a_file_openUpdate(path) fopen((path), "r+")
#endif

#define a_file_openReadText(path)   fopen((path), "r")
#define a_file_openWriteText(path)  fopen((path), "w")
#define a_file_openAppendText(path) fopen((path), "a")
#define a_file_openUpdateText(path) fopen((path), "r+")

#define a_file_close(f) fclose((f)); a_sync()

#define a_file_r(f, value)         fread(&(value), sizeof((value)), 1, (f))
#define a_file_rp(f, buffer, size) fread((buffer), (size), 1, (f))

#define a_file_w(f, value)         fwrite(&(value), sizeof((value)), 1, (f))
#define a_file_wp(f, buffer, size) fwrite((buffer), (size), 1, (f))

#define a_file_jumpStart(f, o)     fseek((f), (o), SEEK_SET)
#define a_file_jumpEnd(f, o)       fseek((f), (o), SEEK_END)
#define a_file_jump(f, o)          fseek((f), (o), SEEK_CUR)

#define a_file_rewind(f)           rewind((f))
#define a_file_end(f)              feof((f))

#define a_file_putKey(f, key)  a_file_wp((f), (key), a_str_size((key)))
#define a_file_jumpKey(f, key) a_file_jumpStart((f), a_str_size((key)))

#define a_file_openDir(path) opendir((path))
#define a_file_closeDir(f)   closedir((f))

#define a_file_makeDir(path) mkdir((path), S_IRWXU); a_sync()

#define a_file_validName(s) (strlen(s) > 0 && (s)[0] != '.')

extern int a_file_exists(const char* const path);
extern int a_file_dirExists(const char* const path);
extern int a_file_hasKey(const char* const path, const char* const key);
extern int a_file_isDir(const char* const f);
extern int a_file_size(const char* const f);

extern List* a_file_list(const char* const path, int (*selector)(const struct dirent* f));
extern void a_file_freeFilePath(FilePath* const f);
extern char* a_file_pathName(const FilePath* const f);
extern char* a_file_pathFull(const FilePath* const f);

extern uint8_t* a_file_toBuffer(const char* const path);

extern FileReader* a_file_makeReader(const char* const file);
extern void a_file_freeReader(FileReader* const f);
extern int a_file_readLine(FileReader* const fr);
extern char* a_file_getLine(const FileReader* const fr);

#endif // A2X_PACK_FILE_PH
