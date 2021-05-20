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

void
Dinamic::_reset(bool hard)
{
    Cartridge::_reset(hard);
}

u8
Dinamic::peekIO1(u16 addr)
{
    if (addr <= 0xDE0F) bankIn(addr & 0xF);
    return 0;
}

u8
Dinamic::spypeekIO1(u16 addr) const
{
    return 0;
}
