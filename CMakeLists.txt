# remi16 - 16-bit retro fantasy console
# Copyright (C) 2025 - suleyth
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Project setup
cmake_minimum_required(VERSION 3.20)
project(remi16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Separate CMake files from output binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Disable in-source builds
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not supported. Please make a build directory and run CMake from there."
    )
endif()

# Disable 32-bit builds
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR
        "32-bit builds are not supported."
    )
endif()

# Debug build macro
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(REMI16_DEBUG=1)
else()
    add_compile_definitions(REMI16_DEBUG=0)
endif()

# Packages
include(CPM.cmake)
CPMAddPackage(
        NAME SDL3
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG preview-3.1.6
        OPTIONS
        "SDL3_DISABLE_INSTALL ON"
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        # "SDL_STATIC_PIC ON"
        "SDL_WERROR OFF"
)

# Targets
# Debugger (main executable)
add_executable(
    remi_debugger
    
    ./remi_debugger/main.cpp

    # vendored ImGui dependencies
    ./vendor/imgui/imgui.cpp
    ./vendor/imgui/imgui_demo.cpp
    ./vendor/imgui/imgui_draw.cpp
    ./vendor/imgui/imgui_tables.cpp
    ./vendor/imgui/imgui_widgets.cpp

    ./vendor/imgui/backends/imgui_impl_sdl3.cpp
    ./vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(remi_debugger PRIVATE "./" "./vendor/imgui")

# VM (cpu)
add_library(
    remi_vm STATIC

    ./remi_vm/vm.cpp
)
target_include_directories(remi_vm PRIVATE "./")

# Libraries
target_link_libraries(remi_debugger PRIVATE remi_vm SDL3::SDL3-static)