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
#include <span>

#include <remi_vm/vm.hpp>
#include <remi_vm/mapper.hpp>

#include "./main.hpp"
#include "./rom_loader.hpp"

// sakuya16c assembly debugger
class debugger {
    vm::sakuya16c cpu;
    vm::bus bus;
    
    loaded_rom rom;
    // temporary
    std::span<u32> program;
    u16 program_addr = 0;
public:
    debugger(const char* rom_path);


    void execute();
    vm::instr step();

    // ImGui methods
    void draw_imgui();
    void draw_current_program_imgui();
};
