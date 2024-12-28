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
#include <fstream>
#include <iostream>

#include <remi_vm/vm.hpp>

#include "./main.hpp"

int main(int argc, char** argv) {
    u32 program[] = {
        (u32) vm::instr(vm::opcode::nop),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(u16(2)), u8(vm::reg::r1)),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(u16(2)), u8(vm::reg::r2)),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(i16(-32734)), u8(vm::reg::r3)),
        (u32) vm::instr(vm::opcode::add_reg_reg, u8(vm::reg::r1), u8(vm::reg::r2)),
        (u32) vm::instr(vm::opcode::hlt),
    };

    // write test rom file
    std::ofstream rom("./test_rom.remi16", std::ios::out | std::ios::binary | std::ios::trunc);
    // magic (4 bytes)
    write(rom, u8(0x7f)); write(rom, u8('r')); write(rom, u8('1')); write(rom, u8('6'));
    // Major version (1 byte)
    write(rom, u8(0));
    // Minor version (1 byte)
    write(rom, u8(1));

    // TODO we need a reliable way of going back to each region and setting the correct offset after everything is written
    // For now though we can just hardcode it since this is just a test file.
    region regions[] = {
        // main
        region {.id = 0, .offset = 24, .size = std::size(program) * sizeof(u32), .loadat = 0x7f00, .bank = 0, .reserved = 0},
    };

    // Region count
    write(rom, u16(std::size(regions)));

    // Write regions
    for (u32 i = 0; i < std::size(regions); i++) {
        write(rom, u32(regions[i].id));
        write(rom, u32(regions[i].offset));
        write(rom, u16(regions[i].size));
        write(rom, u16(regions[i].loadat));
        write(rom, u16(regions[i].bank));
        write(rom, u16(regions[i].reserved));
    }

    // write bytes of all regions (but we just have main region for now so)
    for (u32 i = 0; i < std::size(program); i++) {
        write(rom, program[i]);
    }

    rom.close();
    return 0;
}