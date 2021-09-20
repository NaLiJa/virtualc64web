// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class WarpSpeed : public Cartridge {
    
public:
    
    WarpSpeed(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "WarpSpeed"; }
    CartridgeType getCartridgeType() const override { return CRT_WARPSPEED; }

    void resetCartConfig() override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;

    
    //
    // Operating buttons
    //
    
public:
    
    isize numButtons() const override { return 1; }
    const string getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
};
