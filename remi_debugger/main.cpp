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

#include <remi_vm/vm.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>
#include "../vendor/imgui/backends/imgui_impl_sdl3.h"
#include "../vendor/imgui/backends/imgui_impl_sdlrenderer3.h"

#include "./debugger.hpp"

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    // Initialize window and renderer
    SDL_Window* window = SDL_CreateWindow("Remi16", 1280, 720, SDL_WINDOW_HIDDEN);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, true);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Initialize VM
    auto console = debugger {};
    // Run test program
    u32 program[] = {
        (u32) vm::instr(vm::opcode::nop),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(u16(2)), u8(vm::reg::r1)),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(u16(2)), u8(vm::reg::r2)),
        (u32) vm::instr(vm::opcode::mov_lit_reg, vm::word(i16(-32734)), u8(vm::reg::r3)),
        (u32) vm::instr(vm::opcode::add_reg_reg, u8(vm::reg::r1), u8(vm::reg::r2)),
        (u32) vm::instr(vm::opcode::hlt),
    };
    console.load_program(0x00, program);

    // Show window only after everything is loaded
    SDL_ShowWindow(window);
    bool running = true;
    while (running) {
        // ---------------------------------------- Process system events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            switch (e.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = false;
                break;
            }
        }
        // Don't update if the window is minimized
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // ---------------------------------------- Update
        ImGui::DockSpaceOverViewport();
        console.draw_imgui();
        ImGui::ShowDemoWindow();

        // ---------------------------------------- Draw
        // Clear to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render test rect
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        auto test_rect = SDL_FRect { 100.0, 100.0, 100.0, 100.0 };
        SDL_RenderFillRect(renderer, &test_rect);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}