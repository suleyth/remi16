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
#include <cstring>
#include <type_traits>

namespace vm {

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

// 16-bit value with accessors to each byte. This reduces the need for bit operations (so it's all handled by the compiler)
union word {
    u16 val;
    struct {
        u8 lo;
        u8 hi;
    };

    // Implicit constructor from u16
    word(u16 val): val(val) {}
    // Implicit constructor from i16
    word(i16 val) {
        memcpy(&this->val, &val, sizeof(val));
    }
    // Constructor from two u8
    word(u8 lo, u8 hi): lo(lo), hi(hi) {}
};

// The sakuya16c CPU has 16 registers, all 16-bit wide.
//
// The first 8 registers are for specific values, and the rest are general purpose.
enum class reg: u8 {
    // program counter
    pc,
    // accumulator
    ac,
    // stack pointer
    sp,
    // frame pointer
    fp,
    // interrupt mask
    im,
    // current memory bank
    mb,
    // reserved register for pseudo-instructions only
    ps,
    // general purpose floating point register
    fl,
    // general purpose integer registers
    r0, r1, r2, r3, r4, r5, r6, r7,
};

// Operation code for sakuya16c assembly instructions.
enum class opcode: u8 {
    nop = 0,
    hlt,

    mov_lit_reg,
    add_reg_reg,
};

// An instruction is ALWAYS 4 bytes wide, no matter the argument number. 
//
// The first byte is the opcode. 
struct instr {
    opcode op;
    u8 args[3];

    // Constructor (8bit argument - 8bit argument - 8bit argument)
    instr(opcode op, u8 arg1 = 0, u8 arg2 = 0, u8 arg3 = 0): op(op) {
        args[0] = arg1;
        args[1] = arg2;
        args[2] = arg3;
    }

    // Constructor (8bit argument - 16bit argument)
    instr(opcode op, u8 arg1, word arg2): op(op) {
        args[0] = arg1;
        args[1] = arg2.lo;
        args[2] = arg2.hi;
    }
    
    // Constructor (16bit argument - 8bit argument)
    instr(opcode op, word arg1, u8 arg2): op(op) {
        args[0] = arg1.lo;
        args[1] = arg1.hi;
        args[2] = arg2;
    }

    // Constructor from u32
    explicit instr(u32 val) { *reinterpret_cast<u32*>(this) = val; }
    // Transmute to u32
    explicit operator u32() { return *reinterpret_cast<u32*>(this); }
};
static_assert(sizeof(instr) == 4);

struct status {
    // todo
};

// Value returned from an instruction to indicate the state of the CPU
enum class control_flow {
    // Continue
    ok,
    // Halt the CPU for now
    halt,
    // Fatal error, shutdown or reset
    error,
};

// The sakuya16c is a 16-bit fantasy CPU made for the remi16 fantasy console.
//
// It executes 4-byte instructions (dwordcode?)
struct sakuya16c {
    // Registers are stored contiguously in an array and indexed by helper methods.
    u16 registers[16] = {};
    // Status flags
    status status = {};

    // Sets the value of a register.
    inline void set(reg reg, u16 val) { registers[u8(reg)] = val; }
    // Gets the value of a register.
    inline u16 reg(reg reg) { return registers[u8(reg)]; }

    inline void reset() {
        memset(registers, 0, sizeof(registers));
        status = {};
    }
};

// Executes a single instruction.
control_flow execute(sakuya16c& cpu, instr instr);

} // namespace vm