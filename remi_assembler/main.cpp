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
#include <filesystem>

int main(int argc, char** argv) {
    // write current working directory
    std::cout << std::filesystem::current_path() << std::endl;
    
    // open argv[1] and write content to stdout
    std::ifstream input_asm(argv[1], std::ios::in);
    // print contents
    std::cout << input_asm.rdbuf();

    // write test rom file
    std::ofstream output_rom("./test_rom.remi16", std::ios::out | std::ios::binary);
    output_rom << "a";
    output_rom.close();

    return 0;
}