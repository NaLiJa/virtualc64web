// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"

class DriveMemory : public C64Component {
    
private:
    
    // Reference to the connected disk drive
    class Drive &drive;
    
public:
    
    // RAM (2 KB) and ROM (16 KB)
    u8 ram[0x0800];
    u8 rom[0x4000];
    
    
    //
    // Initializing
    //
    
public:
    
    DriveMemory(C64 &ref, Drive &drive);
    const char *getDescription() const override { return "DriveMemory"; }

private:
    
    void _reset(bool hard) override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << ram
        << rom;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing RAM
    //
    
public:
    
    // Reads a value from memory
    u8 peek(u16 addr);
    u8 peekZP(u8 addr) { return ram[addr]; }
    u8 peekStack(u8 sp) { return ram[0x100 + sp]; }
    
    // Emulates an idle read access
    void peekIdle(u16 addr) { }
    void peekZPIdle(u8 addr) { }
    void peekStackIdle(u8 sp) { }
    void peekIOIdle(u16 addr) { }
    
    // Reads a value from memory without side effects
    u8 spypeek(u16 addr) const;

    // Writes a value into memory
    void poke(u16 addr, u8 value);
    void pokeZP(u8 addr, u8 value) { ram[addr] = value; }
    void pokeStack(u8 sp, u8 value) { ram[0x100 + sp] = value; }
};
