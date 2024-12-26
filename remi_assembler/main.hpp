// remi16 - 16-bit retro fantasy console
// Copyright (C) 2025 - suleyth
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once
#include <cstdint>
#include <fstream>
#include <type_traits>

// typedef cstdint types so they're easier to type
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using usize = size_t;
using isize = std::make_signed_t<usize>;

// Region of memory in ROM
struct region {
    // identifier (0 if it's main entry point)
    u32 id;
    // offset in ROM to region
    u32 offset;
    
    // size of region in bytes
    u16 size;
    // where to load region in RAM (0 for random)
    u16 loadat;
    // memory bank to load region in RAM to (65535 for random)
    u16 bank;
    // padding for now but may be used for something else
    u16 reserved;
};

// helper to write binary data to ROM file (because the << operator writes in string format so we can't use that)
template <typename T>
void write(std::ofstream& rom, T* data) { rom.write((const char*) data, sizeof(T)); }
template <typename T>
void write(std::ofstream& rom, T data) { rom.write((const char*) &data, sizeof(T)); }