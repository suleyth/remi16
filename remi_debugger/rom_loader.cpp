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
#include <cassert>

#include "./rom_loader.hpp"

// helpers to read binary data from ROM file (because the >> operator reads in string format so we can't use that)
template <typename T>
T read(std::ifstream& file) {
    T data = {};
    file.read((char*) &data, sizeof(T));
    return data;
}

// Loads a remi16 ROM from a file. Crashes if the file doesn't exist or is not a remi16 ROM file.s
loaded_rom load_rom_from_file(const char* filename) {
    loaded_rom rom = {};
    rom.file = std::ifstream(filename, std::ios::in | std::ios::binary);

    // magic (4 bytes)
    u8 magic[4];
    rom.file.read((char*) &magic, sizeof(magic));

    if (magic[0] != 0x7f || magic[1] != 'r' || magic[2] != '1' || magic[3] != '6') {
        assert(false && "Invalid ROM file");
    }

    // major version (1 byte)
    u8 major_version = read<u8>(rom.file);
    // minor version (1 byte)
    u8 minor_version = read<u8>(rom.file);

    // region count (2 bytes)
    u16 region_count = read<u16>(rom.file);
    // Populate regions
    rom.regions.reserve(region_count);
    for (u16 i = 0; i < region_count; i++) {
        // region id (4 bytes)
        u32 region_id = read<u32>(rom.file);
        // region offset (4 bytes)
        u32 region_offset = read<u32>(rom.file);
        // region size (2 bytes)
        u16 region_size = read<u16>(rom.file);
        // region loadat (2 bytes)
        u16 region_loadat = read<u16>(rom.file);
        // region bank (2 bytes)
        u16 region_bank = read<u16>(rom.file);
        // reserved
        u16 reserved = read<u16>(rom.file);

        rom.regions[region_id] = rom_region {region_offset, region_size, region_loadat, region_bank, };
    }

    return rom;
}

const std::vector<u8>& loaded_rom::get_region(u32 region_id) {
    if (!loaded_region_data.contains(region_id)) {
        assert(regions.contains(region_id));

        loaded_region_data[region_id] = std::vector<u8>(regions[region_id].size);
        file.seekg(regions[region_id].rom_offset);
        file.read((char*) loaded_region_data[region_id].data(), regions[region_id].size);

        return loaded_region_data[region_id];
    } else {
        return loaded_region_data[region_id];
    }
}