// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

u8
Westermann::peekIO2(u16 addr)
{
    // Reading from I/O 2 switched the cartridge on
    expansionport.setCartridgeMode(CRTMODE_8K);
    return 0;
}

u8
Westermann::spypeekIO2(u16 addr) const
{
    return 0;
}
