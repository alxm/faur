/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
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

#include "f_block.v.h"
#include <faur.v.h>

static FBlock* blockNew(const char* Content)
{
    FBlock* block = f_pool__alloc(F_POOL__BLOCK);

    block->text = f_str_dup(Content);

    return block;
}

static void blockAdd(FBlock* Parent, FBlock* Child, bool Prepend)
{
    if(Parent->blocks == NULL) {
        Parent->blocks = f_list_new();
        Parent->index = f_hash_newStr(16, false);
    }

    FList* indexList = f_hash_get(Parent->index, Child->text);

    if(indexList == NULL) {
        indexList = f_list_new();
        f_hash_add(Parent->index, Child->text, indexList);
    }

    if(Prepend) {
        f_list_addFirst(Parent->blocks, Child);
        f_list_addFirst(indexList, Child);
    } else {
        f_list_addLast(Parent->blocks, Child);
        f_list_addLast(indexList, Child);
    }
}

static void blockCommitLines(FBlock* Block)
{
    if(Block->blocks != NULL) {
        f_mem_free(Block->array);

        Block->array = (const FBlock**)f_list_toArray(Block->blocks);
        Block->arrayLen = f_list_sizeGet(Block->blocks);
    }
}

static inline const FBlock* blockGet(const FBlock* Block, unsigned LineNumber)
{
    if(LineNumber == 0) {
        return Block;
    }

    if(LineNumber <= Block->arrayLen) {
        return Block->array[LineNumber - 1];
    }

    return NULL;
}

FBlock* f_block_new(const char* File)
{
    F__CHECK(File != NULL);

    FFile* f = f_file_new(File, F_FILE_READ);

    if(f == NULL) {
        F__FATAL("f_block_new(%s): Cannot open file", File);
    }

    FBlock* root = blockNew("");
    FList* stack = f_list_new();
    int lastIndent = -1;

    f_list_push(stack, root);

    while(f_file_lineRead(f)) {
        const char* const lineStart = f_file_lineBufferGet(f);
        const char* textStart = lineStart;

        while(*textStart == ' ') {
            textStart++;
        }

        int indentCharsNum = (int)(textStart - lineStart);
        int currentIndent = indentCharsNum / 4;

        if((indentCharsNum % 4) != 0 || currentIndent > lastIndent + 1) {
            F__FATAL("f_block_new(%s): Bad indent on line %d <%s>",
                     File,
                     f_file_lineNumberGet(f),
                     textStart);
        }

        // Each subsequent entry has -1 indentation, pop until reach parent
        while(currentIndent <= lastIndent) {
            blockCommitLines(f_list_pop(stack));
            lastIndent--;
        }

        lastIndent = currentIndent;

        FBlock* parent = f_list_peek(stack);
        FBlock* block = blockNew(textStart);

        blockAdd(parent, block, false);
        f_list_push(stack, block);
    }

    while(!f_list_sizeIsEmpty(stack)) {
        blockCommitLines(f_list_pop(stack));
    }

    f_file_free(f);
    f_list_free(stack);

    return root;
}

void f_block_free(FBlock* Block)
{
    if(Block->references-- > 0) {
        return;
    }

    if(Block->blocks) {
        f_list_freeEx(Block->blocks, (FCallFree*)f_block_free);
        f_hash_freeEx(Block->index, (FCallFree*)f_list_free);
        f_mem_free(Block->array);
    }

    f_mem_free(Block->text);
    f_pool_release(Block);
}

void f_block__refInc(FBlock* Block)
{
    Block->references++;
}

void f_block__merge(FBlock* Dst, const FBlock* Src)
{
    if(Src->blocks == NULL) {
        return;
    }

    F_LIST_ITERATE_REV(Src->blocks, FBlock*, b) {
        blockAdd(Dst, b, true);
        f_block__refInc(b);
    }

    blockCommitLines(Dst);
}

const FList* f_block_blocksGet(const FBlock* Block)
{
    const FList* l = &f__list_empty;

    if(Block != NULL && Block->blocks != NULL) {
        l = Block->blocks;
    }

    return l;
}

const FBlock* f_block_keyGetBlock(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    const FBlock* b = NULL;

    if(Block != NULL) {
        b = f_list_getFirst(f_block_keyGetBlocks(Block, Key));
    }

    return b;
}

const FList* f_block_keyGetBlocks(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    const FList* l = &f__list_empty;

    if(Block != NULL && Block->index) {
        FList* list = f_hash_get(Block->index, Key);

        if(list) {
            l = list;
        }
    }

    return l;
}

bool f_block_keyExists(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    bool b = false;

    if(Block != NULL && Block->index != NULL) {
        b = f_hash_contains(Block->index, Key);
    }

    return b;
}

int f_block_lineGetInt(const FBlock* Block, unsigned LineNumber)
{
    int i = 0;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            i = atoi(line->text);
        }
    }

    return i;
}

int f_block_keyGetInt(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetInt(f_block_keyGetBlock(Block, Key), 1);
}

unsigned f_block_lineGetIntu(const FBlock* Block, unsigned LineNumber)
{
    unsigned u = 0;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            u = (unsigned)atoi(line->text);
        }
    }

    return u;
}

unsigned f_block_keyGetIntu(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetIntu(f_block_keyGetBlock(Block, Key), 1);
}

FFix f_block_lineGetFix(const FBlock* Block, unsigned LineNumber)
{
    FFix f = 0;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            f = f_fix_fromDouble(atof(line->text));
        }
    }

    return f;
}

FFix f_block_keyGetFix(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetFix(f_block_keyGetBlock(Block, Key), 1);
}

FFixu f_block_lineGetAngle(const FBlock* Block, unsigned LineNumber)
{
    FFixu a = 0;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            a = f_fix_angleFromDegf((unsigned)atoi(line->text));
        }
    }

    return a;
}

FFixu f_block_keyGetAngle(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetAngle(f_block_keyGetBlock(Block, Key), 1);
}

FColorPixel f_block_lineGetPixel(const FBlock* Block, unsigned LineNumber)
{
    FColorPixel p = 0;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            p = f_color_pixelFromHex((uint32_t)strtol(line->text, NULL, 16));
        }
    }

    return p;
}

FColorPixel f_block_keyGetPixel(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetPixel(f_block_keyGetBlock(Block, Key), 1);
}

const char* f_block_lineGetString(const FBlock* Block, unsigned LineNumber)
{
    const char* s = "";

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            s = line->text;
        }
    }

    return s;
}

const char* f_block_keyGetString(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetString(f_block_keyGetBlock(Block, Key), 1);
}

FVecInt f_block_lineGetCoords(const FBlock* Block, unsigned LineNumber)
{
    FVecInt v = {0, 0};

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            sscanf(line->text, "%d, %d", &v.x, &v.y);
        }
    }

    return v;
}

FVecInt f_block_keyGetCoords(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetCoords(f_block_keyGetBlock(Block, Key), 1);
}

FVecFix f_block_lineGetCoordsf(const FBlock* Block, unsigned LineNumber)
{
    FVecFix v = {0, 0};

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            double x = 0, y = 0;
            sscanf(line->text, "%lf, %lf", &x, &y);

            v.x = f_fix_fromDouble(x);
            v.y = f_fix_fromDouble(y);
        }
    }

    return v;
}

FVecFix f_block_keyGetCoordsf(const FBlock* Block, const char* Key)
{
    F__CHECK(Key != NULL);

    return f_block_lineGetCoordsf(f_block_keyGetBlock(Block, Key), 1);
}

int f_block_lineGetFmt(const FBlock* Block, unsigned LineNumber, const char* Format, ...)
{
    F__CHECK(Format != NULL);

    int err = -1;

    if(Block != NULL) {
        va_list args;
        va_start(args, Format);

        err = f_block_lineGetFmtv(Block, LineNumber, Format, args);

        va_end(args);
    }

    return err;
}

int f_block_keyGetFmt(const FBlock* Block, const char* Key, const char* Format, ...)
{
    F__CHECK(Key != NULL);
    F__CHECK(Format != NULL);

    int err = -1;

    if(Block != NULL) {
        va_list args;
        va_start(args, Format);

        err = f_block_lineGetFmtv(
                f_block_keyGetBlock(Block, Key), 1, Format, args);

        va_end(args);
    }

    return err;
}

int f_block_lineGetFmtv(const FBlock* Block, unsigned LineNumber, const char* Format, va_list Args)
{
    F__CHECK(Format != NULL);

    int err = -1;

    if(Block != NULL) {
        const FBlock* line = blockGet(Block, LineNumber);

        if(line != NULL) {
            err = vsscanf(line->text, Format, Args);
        }
    }

    return err;
}

int f_block_keyGetFmtv(const FBlock* Block, const char* Key, const char* Format, va_list Args)
{
    F__CHECK(Key != NULL);
    F__CHECK(Format != NULL);

    return
        f_block_lineGetFmtv(f_block_keyGetBlock(Block, Key), 1, Format, Args);
}
