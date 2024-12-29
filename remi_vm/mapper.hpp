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
#include <vector>
#include <memory>

#include "./vm.hpp"

namespace vm {

class mapper_device {
public:
    // Get device name
    virtual const char* name() = 0;
    // At what memory address does this device start and end.
    virtual std::pair<u16, u16> range() = 0;
    // If this returns true, the bus will call the device's read() and write() functions
    // with remapped adresses.
    //
    // Imagine a device over 0x1000 to 0x1050. Without remapping, the addresses passed to the device would be
    // absolute. So if you write to 0x1020, the device will get 0x1020 in its read function. If remap is set to
    // true, however, it will recieve 0x20.
    virtual bool remap_range() = 0;

    // Reads an 8bit value from the device. Ranges are checked automatically.
    virtual u8 read(u16 addr) = 0;
    // Writes an 8bit value into the device.
    virtual void write(u16 addr, u8 val) = 0;

    // Reads a 16bit value from the device. Implemented automatically
    u16 read16(u16 addr);
    // Writes a 16bit value into the device. Implemented automatically
    void write16(u16 addr, u16 val);
};

namespace dev {
    class memory: public mapper_device {
        // Low half (Same across all banks)
        std::unique_ptr<u8[]> lh;

        // High half (bank 0)
        std::unique_ptr<u8[]> hh0;
        // High half (bank 1)
        std::unique_ptr<u8[]> hh1;
        // High half (bank 2)
        std::unique_ptr<u8[]> hh2;
        // High half (bank 3)
        std::unique_ptr<u8[]> hh3;

        const vm::sakuya16c& cpu;
    public:
        memory(const vm::sakuya16c& cpu);

        const char* name() override { return "MEMORY"; }
        std::pair<u16, u16> range() override { return {0x0000, 0xffff}; }
        bool remap_range() override { return false; }

        u8 read(u16 addr) override;
        void write(u16 addr, u8 val) override;
    };
} // namespace dev

class bus {
    std::vector<std::unique_ptr<mapper_device>> mappers;
public:
    bus(const vm::sakuya16c& cpu) { add_mapper(dev::memory(cpu)); }

    // Adds a mapper to the bus.
    template<typename M> requires std::is_base_of_v<mapper_device, M>
    void add_mapper(M&& mapper) { mappers.push_back(std::make_unique<M>(std::forward<M>(mapper))); }\

    std::unique_ptr<mapper_device>& find_mapper_for(u16 addr);
};

} // namespace vm