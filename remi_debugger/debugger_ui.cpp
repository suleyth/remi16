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
#include <string>

#include "./main.hpp"
#include "../remi_vm/vm.hpp"
#include "./debugger.hpp"

#include <imgui.h>

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
    // Register cell is blue and white on hover
    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));
    ImGui::Text("%s", name);
    if (ImGui::IsItemHovered()) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)));
    }

    // On right click, popup that lets you format the register value in different ways
    std::string popup_name = std::string(name) + "_popup";
    if (ImGui::BeginPopupContextItem(popup_name.c_str())) {
        // Make button more tight
        ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 1);
        ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2);
    
        if (ImGui::Button("Unsigned")) { 
            view = regview::unsigned_; 
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Signed")) { 
            view = regview::signed_; 
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Hex"))  { 
            view = regview::hex; 
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::EndPopup();
    }

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
    cpu.set(vm::reg::fl, 65535);

    ImGui::Begin("sakuya16c");

    ImGuiTableFlags table_flags = ImGuiTableFlags_Borders 
        | ImGuiTableFlags_RowBg 
        | ImGuiTableFlags_NoHostExtendX 
        | ImGuiTableFlags_SizingFixedFit;

    // Table for the first 8 specific registers
    ImGui::BeginTable("Registers", 2, table_flags);
        ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
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
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
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

// Renders all debugger ImGui
void debugger::draw_imgui() {
    cpu_imgui(cpu);
}
