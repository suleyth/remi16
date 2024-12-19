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
#include <span>

#include "./main.hpp"
#include "./debugger.hpp"
#include "../remi_vm/vm.hpp"

void debugger::execute(std::span<u32> program) {
    cpu.set(vm::reg::pc, 0);

    assert(!program.empty());
    assert(program[program.size()-1] == (u32) vm::instr(vm::opcode::hlt));

    vm::instr next_instr = vm::instr(program[0]);
    while (next_instr.op != vm::opcode::hlt) {
        // Fetch instruction
        u16 pc = cpu.reg(vm::reg::pc);
        next_instr = vm::instr(program[pc / 4]);
        // Execute
        cpu.execute(next_instr);
        // Program counter always increments by 4
        cpu.set(vm::reg::pc, pc + 4);
    }
}