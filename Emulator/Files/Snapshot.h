// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"
#include "Constants.h"

typedef struct {
    
    // Header signature
    char magicBytes[4];
    
    // Version number
    u8 major;
    u8 minor;
    u8 subminor;
    
    // Thumbnail image
    struct {
        u16 width, height;
        u32 screen[TEX_HEIGHT * TEX_WIDTH];
        
    } screenshot;
    
    // Creation date
    time_t timestamp;
}
SnapshotHeader;

class Snapshot : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
     
    static Snapshot *makeWithC64(class C64 *c64);

    
    //
    // Initializing
    //
        
    Snapshot() { };
    Snapshot(usize capacity);
        
    
    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "Snapshot"; }

    
    //
    // Methods from AnyFile
    //
        
    FileType type() const override { return FILETYPE_V64; }
    
    
    //
    // Accessing
    //
        
    // Returns a pointer to the snapshot header
    SnapshotHeader *header() const { return (SnapshotHeader *)data; }

    // Checks the snapshot version number
    bool isTooOld() const;
    bool isTooNew() const;
    bool matches() { return !isTooOld() && !isTooNew(); }
    
    u8 *getData() { return data + sizeof(SnapshotHeader); }
    
    // Queries time and screenshot properties
    time_t timeStamp() const { return header()->timestamp; }
    u8 *imageData() const { return (u8 *)(header()->screenshot.screen); }
    usize imageWidth() const { return header()->screenshot.width; }
    usize imageHeight() const { return header()->screenshot.height; }
    
    // Records a screenshot
    void takeScreenshot(class C64 *c64);
};
