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

#include <remi_vm/vm.hpp>

#include "./main.hpp"
#include "./debugger.hpp"


// Temporary function to load a sakuya16c assembly program into the debugger
// (later on programs will be loaded by ROMs)
//
// for now it sets the PC register to 0 before executing the program
//
// Crashes if the last instruction in `program` is not HLT or if `program` is empty.
void debugger::load_program(usize addr, std::span<u32> program) {
    cpu.set(vm::reg::pc, 0);

    this->program_addr = addr;
    this->program = program;
    
    assert(!program.empty());
    assert(program[program.size()-1] == (u32) vm::instr(vm::opcode::hlt));
}

vm::instr debugger::step() {
    // Fetch instruction
    u16 pc = cpu.reg(vm::reg::pc);
    // `program` is an u32 array, but `pc` is supposed to be a byte index. Hence the need for division
    vm::instr next_instr = vm::instr(program[pc / 4]);
    if (next_instr.op != vm::opcode::hlt) {
        // Execute
        vm::execute(cpu, next_instr);
        // Program counter always increments by 4 after executing
        cpu.set(vm::reg::pc, pc + 4);
    }

    return next_instr;
}

// Executes a sakuya16c assembly program. The execution will not stop until a HLT instruction is encountered.
void debugger::execute() {
    while (step().op != vm::opcode::hlt) {}
}