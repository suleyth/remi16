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
#include <imgui.h>
#include <remi_vm/vm.hpp>

#include "./main.hpp"
#include "./debugger.hpp"


// How to format a register value
enum class regview {
    unsigned_, signed_, hex,
};

// UI state for the cpu viewer
static struct {
    regview regviews[16] = {};
} cpu_ui;

// Renders ImGui table row for a register
void reg_imgui(vm::sakuya16c &cpu, const char* name, vm::reg reg, regview& view) {
    // Emphasize button
    ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 15);
    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
    ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0);
    if (ImGui::Button(name)) {
        // Switch view
            switch (view) {
            case regview::unsigned_: view = regview::signed_; break;
            case regview::signed_: view = regview::hex; break;
            case regview::hex: view = regview::unsigned_; break;
            }
    }
    ImGui::PopStyleVar(3);

    // Register value
    ImGui::TableNextColumn();
    switch (view) {
    case regview::unsigned_: {
        ImGui::Text("%u", cpu.reg(reg));
    } break;
    case regview::signed_: {
        // This is the only thing guaranteed to work across all compilers and architectures, AFAIK
        u16 unsigned_val = cpu.reg(reg);
        i16 signed_val = 0;
        memcpy(&signed_val, &unsigned_val, sizeof(signed_val));
        ImGui::Text("%i", signed_val);
    } break;
    case regview::hex: {
        ImGui::Text("$%x", cpu.reg(reg));
    } break;
    }

    ImGui::TableNextColumn();
}

// Renders ImGui related to the sakuya16c CPU.
void cpu_imgui(vm::sakuya16c &cpu) {
    ImGui::Begin("sakuya16c");

    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders 
        | ImGuiTableFlags_RowBg 
        | ImGuiTableFlags_NoHostExtendX 
        | ImGuiTableFlags_SizingFixedFit;

    // Table for the first 8 specific registers
    ImGui::BeginTable("Registers", 2, table_flags);
        ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 45.0f);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        reg_imgui(cpu, "pc", vm::reg::pc, cpu_ui.regviews[0]);
        reg_imgui(cpu, "ac", vm::reg::ac, cpu_ui.regviews[1]);
        reg_imgui(cpu, "sp", vm::reg::sp, cpu_ui.regviews[2]);
        reg_imgui(cpu, "fp", vm::reg::fp, cpu_ui.regviews[3]);
        reg_imgui(cpu, "im", vm::reg::im, cpu_ui.regviews[4]);
        reg_imgui(cpu, "mb", vm::reg::mb, cpu_ui.regviews[5]);
        reg_imgui(cpu, "ps", vm::reg::ps, cpu_ui.regviews[6]);
        reg_imgui(cpu, "fl", vm::reg::fl, cpu_ui.regviews[7]);
    ImGui::EndTable();

    ImGui::SameLine();

    // Table for the last 8 general purpose registers
    ImGui::BeginTable("GeneralRegisters", 2, table_flags);
        ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 45.0f);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        reg_imgui(cpu, "r0", vm::reg::r0, cpu_ui.regviews[8]);
        reg_imgui(cpu, "r1", vm::reg::r1, cpu_ui.regviews[9]);
        reg_imgui(cpu, "r2", vm::reg::r2, cpu_ui.regviews[10]);
        reg_imgui(cpu, "r3", vm::reg::r3, cpu_ui.regviews[11]);
        reg_imgui(cpu, "r4", vm::reg::r4, cpu_ui.regviews[12]);
        reg_imgui(cpu, "r5", vm::reg::r5, cpu_ui.regviews[13]);
        reg_imgui(cpu, "r6", vm::reg::r6, cpu_ui.regviews[14]);
        reg_imgui(cpu, "r7", vm::reg::r7, cpu_ui.regviews[15]);
    ImGui::EndTable();

    ImGui::End();
}

// Dissassembly syntax highlighting constants (from ayu-mirage)
constexpr ImVec4 COLOR_OPCODE = ImVec4(1.0f, 0.67f, 0.4f, 1.0f); // #FFAD66
constexpr ImVec4 COLOR_REGISTER = ImVec4(0.94f, 0.52f, 0.47f, 1.0f); // #F28779
constexpr ImVec4 COLOR_LITERAL = ImVec4(0.87f, 0.74f, 1.0f, 1.0f); // #DFBFFF
constexpr ImVec4 COLOR_HIGHLIGHT = ImVec4(0.2f, 0.2f, 0.8f, 0.5f);

// Returns a string representation of an opcode
const char* opcode_name(vm::opcode opcode, bool overloaded_name = false);

// UI state for the cpu viewer
static struct {
    bool show_overload = false;
} disasm_ui;

const char* reg_name(vm::reg reg);

void draw_instr_arguments(vm::instr instr) {
    auto draw_lit_reg = [](vm::instr instr) {
        u16 lit = vm::word(instr.args[0], instr.args[1]).val;
        vm::reg reg = static_cast<vm::reg>(instr.args[2]);

        ImGui::TextColored(COLOR_LITERAL, "$%04x", lit);
        ImGui::SameLine(0, 0);
        ImGui::Text(", ");
        ImGui::SameLine(0, 0);
        ImGui::TextColored(COLOR_REGISTER, "%s", reg_name(reg));
    };

    auto draw_reg_reg = [](vm::instr instr) {
        vm::reg reg1 = static_cast<vm::reg>(instr.args[0]);
        vm::reg reg2 = static_cast<vm::reg>(instr.args[1]);

        ImGui::TextColored(COLOR_REGISTER, "%s", reg_name(reg1));
        ImGui::SameLine(0, 0);
        ImGui::Text(", ");
        ImGui::SameLine(0, 0);
        ImGui::TextColored(COLOR_REGISTER, "%s", reg_name(reg2));
    };

    switch (instr.op) {
    case vm::opcode::mov_lit_reg:
        draw_lit_reg(instr);
        break;
    case vm::opcode::add_reg_reg:
        draw_reg_reg(instr);
        break;
    default:
        break;
    }
}

// Draws dissassembly of the current running program
void debugger::draw_current_program_imgui() {
    ImGui::Begin("Current Program");
    ImGui::Checkbox("Show Overload", &disasm_ui.show_overload);
    ImGui::SameLine();
    ImGui::Text("| Size: $%zx | Instructions: %zu", program.size_bytes(), program.size());
    ImGui::Separator();

    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders 
        | ImGuiTableFlags_RowBg 
        | ImGuiTableFlags_SizingFixedFit
        | ImGuiTableFlags_ScrollY;

    auto current_running_instr = vm::instr(program[cpu.reg(vm::reg::pc) / 4]);
    if (current_running_instr.op == vm::opcode::hlt) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Step")) step();
    ImGui::SameLine();
    if (ImGui::Button("Execute")) execute();

    if (current_running_instr.op == vm::opcode::hlt) {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        cpu.reset();

        // TODO set to appropriate value
        program_addr = 0; 
        cpu.set(vm::reg::pc, 0);
    }

    if (current_running_instr.op == vm::opcode::hlt) {
        ImGui::SameLine();
        ImGui::Text("Program halted");
    }

    ImGui::BeginTable("Program", 2, table_flags);
        ImGui::TableSetupColumn("Addr", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
        ImGui::TableSetupColumn("Instruction", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

    u16 pc = cpu.reg(vm::reg::pc);
    
    usize addr_counter = program_addr;
    for (usize program_idx = 0; program_idx < program.size(); program_idx++) {
        auto next_instr = vm::instr(program[program_idx]);
        ImGui::Text("$%04zx", addr_counter);
        addr_counter += 4;
        ImGui::TableNextColumn();
        if (program_idx == pc / 4) {
            // Set cell color to blue
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::ColorConvertFloat4ToU32(COLOR_HIGHLIGHT));
        }

        ImGui::TextColored(COLOR_OPCODE, "%s", opcode_name(next_instr.op, disasm_ui.show_overload));
        ImGui::SameLine();
        draw_instr_arguments(next_instr);
        ImGui::TableNextColumn();
    }
    ImGui::EndTable();


    ImGui::End();
}

// Renders all debugger ImGui
void debugger::draw_imgui() {
    cpu_imgui(cpu);
    draw_current_program_imgui();
}


// Opcode enum to string
const char* opcode_name(vm::opcode opcode, bool overloaded_name) {
    switch (opcode) {
    case vm::opcode::nop: return "nop";
    case vm::opcode::hlt: return "hlt";
    case vm::opcode::mov_lit_reg: return overloaded_name ? "mov_lit_reg" : "mov";
    case vm::opcode::add_reg_reg: return overloaded_name ? "add_reg_reg" : "add";
    }
    return "???";
}

// Reg enum to string
const char* reg_name(vm::reg reg) {
    switch (reg) {
    case vm::reg::r0: return "#r0";
    case vm::reg::r1: return "#r1";
    case vm::reg::r2: return "#r2";
    case vm::reg::r3: return "#r3";
    case vm::reg::r4: return "#r4";
    case vm::reg::r5: return "#r5";
    case vm::reg::r6: return "#r6";
    case vm::reg::r7: return "#r7";

    case vm::reg::pc: return "#pc";
    case vm::reg::ac: return "#ac";
    case vm::reg::sp: return "#sp";
    case vm::reg::fp: return "#fp";
    case vm::reg::im: return "#im";
    case vm::reg::mb: return "#mb";
    case vm::reg::ps: return "#ps";
    case vm::reg::fl: return "#fl";
    }
    return "#??";
}