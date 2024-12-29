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
#include "./mapper.hpp"
#include <cassert>

namespace vm {

u16 mapper_device::read16(u16 addr) { 
    // Read first byte
    u8 b1 = read(addr);

    // Check range
    auto [range_start, range_end] = range();
    if (remap_range()) {
        range_end = range_end - range_start;
        range_start = 0;
    }
    if (addr == 0xffff) {
        // TODO out of range interrupt
        assert(false && "out of range");
        return u16(b1);
    }

    // check that second byte address is in range   
    u16 b2_addr = addr + 1; 
    if (b2_addr >= range_start && b2_addr <= range_end) {
        u8 b2 = read(b2_addr);
        return word(b1, b2).val;
    } else {
        // TODO out of range interrupt
        assert(false && "byte 2 address out of device range");
        return u16(b1);
    }
}

void mapper_device::write16(u16 addr, u16 val) {
    // Write first byte
    auto word_val = word(val);
    write(addr, word_val.lo);

    // Check range
    auto [range_start, range_end] = range();
    if (remap_range()) {
        range_end = range_end - range_start;
        range_start = 0;
    }
    if (addr == 0xffff) {
        // TODO out of range interrupt
        assert(false && "out of range");
        return;
    }

    // check that second byte address is in range   
    u16 b2_addr = addr + 1; 
    if (b2_addr >= range_start && b2_addr <= range_end) {
        write(b2_addr, word_val.hi);
    } else {
        // TODO out of range interrupt
        assert(false && "byte 2 address out of device range");
        return;
    }
}

std::unique_ptr<mapper_device>& bus::find_mapper_for(u16 addr) {
    // Addresses above 0x8000 are guaranteed to be raw memory not claimed by the stack,
    // hardware devices, or executing ROM code. Therefore we can just return the first mapper
    // (the "MEMORY" mapper).
    if (addr > 0x8000) {
        return mappers[0];
    }

    for (auto& mapper : mappers) {
        auto [range_start, range_end] = mapper->range();
        if (addr >= range_start && addr <= range_end) {
            return mapper;
        }
    }

    assert(false && "no mapper found for address");
    return mappers[0];
}


// Devices
dev::memory::memory(const vm::sakuya16c& cpu): cpu(cpu) {
    lh = std::unique_ptr<u8[]>(new u8[0x8000]);

    hh0 = std::unique_ptr<u8[]>(new u8[0x8000-1]);
    hh1 = std::unique_ptr<u8[]>(new u8[0x8000-1]);
    hh2 = std::unique_ptr<u8[]>(new u8[0x8000-1]);
    hh3 = std::unique_ptr<u8[]>(new u8[0x8000-1]);
}

u8 dev::memory::read(u16 addr) { 
    if (addr <= 0x8000) {
        return lh[addr];
    }

    u16 mb = cpu.reg(reg::mb) % 4; // 4 is the number of memory banks
    switch (mb) {
    case 0: return hh0[addr - 0x8000];
    case 1: return hh1[addr - 0x8000];
    case 2: return hh2[addr - 0x8000];
    case 3: return hh3[addr - 0x8000];
    default: 
        assert(false && "unreachable");
        return 0;
    }
}

void dev::memory::write(u16 addr, u8 val) {
    if (addr <= 0x8000) {
        lh[addr] = val;
        return;
    }

    u16 mb = cpu.reg(reg::mb) % 4; // 4 is the number of memory banks
    switch (mb) {
    case 0: hh0[addr - 0x8000] = val; break;
    case 1: hh1[addr - 0x8000] = val; break;
    case 2: hh2[addr - 0x8000] = val; break;
    case 3: hh3[addr - 0x8000] = val; break;
    default: 
        assert(false && "unreachable");
    }
}

} // namespace vm