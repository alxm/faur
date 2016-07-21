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

#include "a2x_pack_mem.v.h"

void* a_mem_malloc(size_t Size)
{
    void* const ptr = malloc(Size);

    if(ptr == NULL) {
        a_out__fatal("a_mem_malloc(%u) failed", Size);
    }

    return ptr;
}

void* a_mem_encodeRLE(const void* Data, int Length, int Size, int* EncodedLength)
{
    int space = 0;

    void* dst = NULL;
    uint8_t* dst8 = NULL;
    uint16_t* dst16 = NULL;
    uint32_t* dst32 = NULL;

    const uint8_t* const data8 = Data;
    const uint16_t* const data16 = Data;
    const uint32_t* const data32 = Data;

    for(int a = 0; a < 2; a++) {
        if(a == 1) {
            dst = a_mem_malloc(space * Size);

            dst8 = dst;
            dst16 = dst;
            dst32 = dst;

            if(EncodedLength) {
                *EncodedLength = space;
            }
        }

        for(int i = 0; i < Length; ) {
            unsigned int span = 1;

            for(int j = i; j < Length - 1; j++) {
                if((Size == 1 && data8[j] == data8[j + 1]) || (Size == 2 && data16[j] == data16[j + 1]) || (Size == 4 && data32[j] == data32[j + 1])) {
                    span++;

                    if((Size == 1 && span == 0xff) || (Size == 2 && span == 0xffff) || (Size == 4 && span == 0xffffffff)) {
                        break;
                    }
                } else {
                    break;
                }
            }

            if(span > 2 || (span == 2 && ((Size == 1 && data8[i] == 0) || (Size == 2 && data16[i] == 0) || (Size == 4 && data32[i] == 0)))) {
                if(a == 0) {
                    // TRIGGER LENGTH VALUE
                    space += 1 + 1 + 1;
                } else {
                    switch(Size) {
                        case 1: {
                            *dst8++ = 0;
                            *dst8++ = span;
                            *dst8++ = data8[i];
                        } break;

                        case 2: {
                            *dst16++ = 0;
                            *dst16++ = span;
                            *dst16++ = data16[i];
                        } break;

                        case 4: {
                            *dst32++ = 0;
                            *dst32++ = span;
                            *dst32++ = data32[i];
                        } break;
                    }
                }

                i += span;
            } else if((Size == 1 && data8[i] == 0) || (Size == 2 && data16[i] == 0) || (Size == 4 && data32[i] == 0)) {
                if(a == 0) {
                    // TRIGGER TRIGGER
                    space += 1 + 1;
                } else {
                    switch(Size) {
                        case 1: {
                            *dst8++ = 0;
                            *dst8++ = 0;
                        } break;

                        case 2: {
                            *dst16++ = 0;
                            *dst16++ = 0;
                        } break;

                        case 4: {
                            *dst32++ = 0;
                            *dst32++ = 0;
                        } break;
                    }
                }

                i += 1;
            } else {
                if(a == 0) {
                    // VALUE
                    space += 1;
                } else {
                    switch(Size) {
                        case 1: {
                            *dst8++ = data8[i];
                        } break;

                        case 2: {
                            *dst16++ = data16[i];
                        } break;

                        case 4: {
                            *dst32++ = data32[i];
                        } break;
                    }
                }

                i += 1;
            }
        }
    }

    return dst;
}

void* a_mem_decodeRLE(const void* Data, int Length, int Size, int* DecodedLength)
{
    int space = 0;

    void* dst = NULL;
    uint8_t* dst8 = NULL;
    uint16_t* dst16 = NULL;
    uint32_t* dst32 = NULL;

    const uint8_t* const data8 = Data;
    const uint16_t* const data16 = Data;
    const uint32_t* const data32 = Data;

    for(int a = 0; a < 2; a++) {
        if(a == 1) {
            dst = a_mem_malloc(space * Size);

            dst8 = dst;
            dst16 = dst;
            dst32 = dst;

            if(DecodedLength) {
                *DecodedLength = space;
            }
        }

        for(int i = 0; i < Length; ) {
            if(i + 1 < Length && ((Size == 1 && data8[i] == 0) || (Size == 2 && data16[i] == 0) || (Size == 4 && data32[i] == 0))) {
                if((Size == 1 && data8[i + 1] == 0) || (Size == 2 && data16[i + 1] == 0) || (Size == 4 && data32[i + 1] == 0)) {
                    if(a == 0) {
                        space += 1;
                    } else {
                        switch(Size) {
                            case 1: {
                                *dst8++ = 0;
                            } break;

                            case 2: {
                                *dst16++ = 0;
                            } break;

                            case 4: {
                                *dst32++ = 0;
                            } break;
                        }
                    }

                    // TRIGGER TRIGGER
                    i += 1 + 1;
                } else {
                    if(a == 0) {
                        switch(Size) {
                            case 1: {
                                space += data8[i + 1];
                            } break;

                            case 2: {
                                space += data16[i + 1];
                            } break;

                            case 4: {
                                space += data32[i + 1];
                            } break;
                        }
                    } else {
                        switch(Size) {
                            case 1: {
                                for(int j = data8[i + 1]; j--; ) {
                                    *dst8++ = data8[i + 2];
                                }
                            } break;

                            case 2: {
                                for(int j = data16[i + 1]; j--; ) {
                                    *dst16++ = data16[i + 2];
                                }
                            } break;

                            case 4: {
                                for(int j = data32[i + 1]; j--; ) {
                                    *dst32++ = data32[i + 2];
                                }
                            } break;
                        }
                    }

                    // TRIGGER LENGTH VALUE
                    i += 1 + 1 + 1;
                }
            } else {
                if(a == 0) {
                    space += 1;
                } else {
                    switch(Size) {
                        case 1: {
                            *dst8++ = data8[i];
                        } break;

                        case 2: {
                            *dst16++ = data16[i];
                        } break;

                        case 4: {
                            *dst32++ = data32[i];
                        } break;
                    }
                }

                // VALUE
                i += 1;
            }
        }
    }

    return dst;
}
