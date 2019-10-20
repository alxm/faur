/*
    Copyright 2018-2019 Alex Margarit <alex@alxm.org>
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

struct FBlock {
    char* text; // own content
    FList* blocks; // list of FBlock indented under this block
    FStrHash* index; // table of FList of FBlock, the blocks indexed by name
    const FBlock** array; // the blocks indexed by line # relative to parent
    unsigned arrayLen; // number of blocks under parent
};

static FBlock* blockNew(const char* Content)
{
    FBlock* block = f_mem_zalloc(sizeof(FBlock));

    block->text = f_str_dup(Content);

    return block;
}

static void blockFree(FBlock* Block)
{
    if(Block->blocks) {
        f_list_freeEx(Block->blocks, (FFree*)blockFree);
        f_strhash_freeEx(Block->index, (FFree*)f_list_free);
        f_mem_free(Block->array);
    }

    f_mem_free(Block->text);
    f_mem_free(Block);
}

static void blockAdd(FBlock* Parent, FBlock* Child)
{
    if(Parent->blocks == NULL) {
        Parent->blocks = f_list_new();
        Parent->index = f_strhash_new();
    }

    FList* indexList = f_strhash_get(Parent->index, Child->text);

    if(indexList == NULL) {
        indexList = f_list_new();
        f_strhash_add(Parent->index, Child->text, indexList);
    }

    f_list_addLast(Parent->blocks, Child);
    f_list_addLast(indexList, Child);
}

static void blockCommitLines(FBlock* Block)
{
    if(Block->blocks != NULL) {
        Block->array = (const FBlock**)f_list_toArray(Block->blocks);
        Block->arrayLen = f_list_sizeGet(Block->blocks);
    }
}

static inline const FBlock* blockGet(const FBlock* Block, unsigned LineNumber)
{
    if(LineNumber == 0) {
        return Block;
    }

    if(Block && LineNumber <= Block->arrayLen) {
        return Block->array[LineNumber - 1];
    }

    return NULL;
}

FBlock* f_block_new(const char* File)
{
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
            F__FATAL("f_block_new: Bad indent in %s:%d <%s>",
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

        blockAdd(parent, block);
        f_list_push(stack, block);
    }

    while(!f_list_isEmpty(stack)) {
        blockCommitLines(f_list_pop(stack));
    }

    f_file_free(f);
    f_list_free(stack);

    return root;
}

void f_block_free(FBlock* Block)
{
    if(!f_str_equal(Block->text, "")) {
        F__FATAL("f_block_free: Must call on root block");
    }

    blockFree(Block);
}

const FList* f_block_blocksGet(const FBlock* Block)
{
    return Block->blocks ? Block->blocks : &f__list_empty;
}

const FBlock* f_block_keyGetBlock(const FBlock* Block, const char* Key)
{
    return f_list_getFirst(f_block_keyGetBlocks(Block, Key));
}

const FList* f_block_keyGetBlocks(const FBlock* Block, const char* Key)
{
    if(Block->index) {
        FList* list = f_strhash_get(Block->index, Key);

        if(list) {
            return list;
        }
    }

    return &f__list_empty;
}

bool f_block_keyExists(const FBlock* Block, const char* Key)
{
    return Block->index && f_strhash_contains(Block->index, Key);
}

int f_block_lineGetInt(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    return line ? atoi(line->text) : 0;
}

unsigned f_block_lineGetIntu(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    return line ? (unsigned)atoi(line->text) : 0;
}

FFix f_block_lineGetFix(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    return line ? f_fix_fromDouble(atof(line->text)) : 0;
}

FFixu f_block_lineGetAngle(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    if(line) {
        return f_fix_angleFromDegf((unsigned)atoi(line->text));
    } else {
        return 0;
    }
}

FPixel f_block_lineGetPixel(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    return line ? f_pixel_fromHex((uint32_t)strtol(line->text, NULL, 16)) : 0;
}

const char* f_block_lineGetString(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);

    return line ? line->text : "";
}

FVectorInt f_block_lineGetCoords(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);
    FVectorInt v = {0, 0};

    if(line) {
        sscanf(line->text, "%d, %d", &v.x, &v.y);
    }

    return v;
}

FVectorFix f_block_lineGetCoordsf(const FBlock* Block, unsigned LineNumber)
{
    const FBlock* line = blockGet(Block, LineNumber);
    FVectorFix v = {0, 0};

    if(line) {
        double x = 0, y = 0;
        sscanf(line->text, "%lf, %lf", &x, &y);

        v.x = f_fix_fromDouble(x);
        v.y = f_fix_fromDouble(y);
    }

    return v;
}

int f_block_lineGetFmt(const FBlock* Block, unsigned LineNumber, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    int ret = f_block_lineGetFmtv(Block, LineNumber, Format, args);

    va_end(args);

    return ret;
}

int f_block_lineGetFmtv(const FBlock* Block, unsigned LineNumber, const char* Format, va_list Args)
{
    const FBlock* line = blockGet(Block, LineNumber);

    if(line) {
        return vsscanf(line->text, Format, Args);
    }

    return -1;
}

int f_block_keyGetFmt(const FBlock* Block, const char* Key, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    int ret = f_block_lineGetFmtv(
                f_block_keyGetBlock(Block, Key), 1, Format, args);

    va_end(args);

    return ret;
}
