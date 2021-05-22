// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Constants
//

#define LOG_BUFFER_CAPACITY 256

#define C_FLAG 0x01
#define Z_FLAG 0x02
#define I_FLAG 0x04
#define D_FLAG 0x08
#define B_FLAG 0x10
#define V_FLAG 0x40
#define N_FLAG 0x80


//
// Enumerations
//

enum_long(CPUREV)
{
    MOS_6510,
    MOS_6502,
    CPUREV_COUNT
};
typedef CPUREV CPURevision;

#ifdef __cplusplus
struct CPURevisionEnum : util::Reflection<CPURevisionEnum, CPURevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < CPUREV_COUNT;
    }
    
    static const char *prefix() { return nullptr; }
    static const char *key(CPURevision value)
    {
        switch (value) {
                
            case MOS_6510:      return "MOS_6510";
            case MOS_6502:      return "MOS_6502";
            case CPUREV_COUNT:  return "???";
        }
        return "???";
    }
};
#endif

enum_u8(INTSRC)
{
    INTSRC_CIA  = 0x01,
    INTSRC_VIC  = 0x02,
    INTSRC_VIA1 = 0x04,
    INTSRC_VIA2 = 0x08,
    INTSRC_EXP  = 0x10,
    INTSRC_KBD  = 0x20
};
typedef INTSRC IntSource;

#ifdef __cplusplus
struct IntSourceEnum : util::Reflection<IntSourceEnum, IntSource> {
    
    static bool isValid(long value)
    {
        return
        value == INTSRC_CIA  ||
        value == INTSRC_VIC  ||
        value == INTSRC_VIA1 ||
        value == INTSRC_VIA2 ||
        value == INTSRC_EXP  ||
        value == INTSRC_KBD;
    }
    
    static const char *prefix() { return "INTSRC"; }
    static const char *key(IntSource value)
    {
        switch (value) {
                
            case INTSRC_CIA:   return "CIA";
            case INTSRC_VIC:   return "VIC";
            case INTSRC_VIA1:  return "VIA1";
            case INTSRC_VIA2:  return "VIA2";
            case INTSRC_EXP:   return "EXP";
            case INTSRC_KBD:   return "KBD";
        }
        return "???";
    }
    
    static std::map <string, long> pairs() { return Reflection::pairs(INTSRC_KBD); }
};
#endif

enum_long(BPTYPE)
{
    BPTYPE_NONE,
    BPTYPE_HARD,
    BPTYPE_SOFT,
    BPTYPE_COUNT
};
typedef BPTYPE BreakpointType;

#ifdef __cplusplus
struct BreakpointTypeEnum : util::Reflection<BreakpointTypeEnum, BreakpointType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < BPTYPE_COUNT;
    }
    
    static const char *prefix() { return "BPTYPE"; }
    static const char *key(BreakpointType value)
    {
        switch (value) {
                
            case BPTYPE_NONE:   return "NONE";
            case BPTYPE_HARD:   return "HARD";
            case BPTYPE_SOFT:   return "SOFT";
            case BPTYPE_COUNT:  return "???";
        }
        return "???";
    }
};
#endif

typedef enum
{
    ADDR_IMPLIED,
    ADDR_ACCUMULATOR,
    ADDR_IMMEDIATE,
    ADDR_ZERO_PAGE,
    ADDR_ZERO_PAGE_X,
    ADDR_ZERO_PAGE_Y,
    ADDR_ABSOLUTE,
    ADDR_ABSOLUTE_X,
    ADDR_ABSOLUTE_Y,
    ADDR_INDIRECT_X,
    ADDR_INDIRECT_Y,
    ADDR_RELATIVE,
    ADDR_DIRECT,
    ADDR_INDIRECT
}
AddressingMode;


//
// Structures
//

typedef struct
{
    bool n;               // Negative flag
    bool v;               // Overflow flag
    bool b;               // Break flag
    bool d;               // Decimal flag
    bool i;               // Interrupt flag
    bool z;               // Zero flag
    bool c;               // Carry flag
}
StatusRegister;
    
typedef struct
{
    u16 pc;   // Program counter
    u16 pc0;  // Frozen program counter (beginning of current instruction)
    
    u8 sp;    // Stack pointer

    u8 a;     // Accumulator
    u8 x;     // First index register
    u8 y;     // Second index register

    u8 adl;   // Address data (low byte)
    u8 adh;   // Address data (high byte)
    u8 idl;   // Input data latch (indirect addressing modes)
    u8 d;     // Data buffer
    
    bool ovl; // Overflow indicator (page boundary crossings)

    StatusRegister sr;
}
Registers;

typedef struct
{
    u64 cycle;
    
    u8 byte1;
    u8 byte2;
    u8 byte3;
    
    u16 pc;
    u8 sp;
    u8 a;
    u8 x;
    u8 y;
    u8 flags;
}
RecordedInstruction;

typedef struct
{
    u64 cycle;

    Registers reg;
    
    u8 irq;
    u8 nmi;
    bool rdy;
    bool jammed;
    
    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;
