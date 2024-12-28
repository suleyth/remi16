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
#include <remi_vm/vm.hpp>

#include <vector>
#include <unordered_map>
#include <fstream>

#include "./main.hpp"

struct rom_region {
    u32 rom_offset;
    u32 size;
    u16 loadat;
    u16 bank;
};

struct loaded_rom {
    std::ifstream file;
    std::unordered_map<u32, rom_region> regions;
    std::unordered_map<u32, std::vector<u8>> loaded_region_data;

    // Crashes if region doesn't exist.
    const std::vector<u8>& get_region(u32 region_id);
};

loaded_rom load_rom_from_file(const char* filename);