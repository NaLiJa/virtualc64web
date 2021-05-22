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

class Funplay : public Cartridge {
    
public:

    Funplay(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "Funplay"; }
    CartridgeType getCartridgeType() const override { return CRT_FUNPLAY; }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void pokeIO1(u16 addr, u8 value) override;
};
