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

#include "./vm.hpp"

namespace vm {

// Instructions
namespace op {
    // NOP - Performs no operation. 
    //
    // Takes no arguments. (null - null - null)
    control_flow nop(sakuya16c& cpu, instr instr) { return control_flow::ok; }

    // HLT - Halts the CPU. 
    //
    // Takes no arguments. (null - null - null)
    control_flow hlt(sakuya16c& cpu, instr instr) { return control_flow::halt; }

    // MOV (lit, reg) - Moves a literal value into a register. 
    //
    // Takes two arguments. (16bit literal - 8bit register)
    control_flow mov_lit_reg(sakuya16c& cpu, instr instr) {
        auto* lit = reinterpret_cast<u16*>(&instr.args[0]);
        auto reg = static_cast<vm::reg>(instr.args[2]);

        cpu.set(reg, *lit);

        return control_flow::ok;
    }

    // ADD (reg, reg) - Adds the values of two registers and stores the result in the accumulator. 
    // 
    // Takes two arguments. (8bit register - 8bit register - null)
    control_flow add_reg_reg(sakuya16c& cpu, instr instr) {
        auto reg1 = static_cast<vm::reg>(instr.args[0]);
        auto reg2 = static_cast<vm::reg>(instr.args[1]);

        auto val1 = cpu.reg(reg1);
        auto val2 = cpu.reg(reg2);

        cpu.set(vm::reg::ac, val1 + val2);

        return control_flow::ok;
    }
}

// Instruction lookup table by opcode (this is to avoid a giant switch statement)
typedef control_flow (*opcode_func)(sakuya16c& cpu, instr instr);
static opcode_func opcode_table[] = {
    op::nop,
    op::hlt,

    op::mov_lit_reg,
    op::add_reg_reg,
};

// TODO: add a test to verify the instruction lookup table and that it
// always matches the opcode enum.

// Executes an instruction fetched from the lookup table.
control_flow execute(sakuya16c& cpu, instr instr) { 
    opcode_func func = opcode_table[usize(instr.op)];
    return func(cpu, instr); 
}

} // namespace vm