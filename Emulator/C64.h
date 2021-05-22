// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

// General
#include "C64Component.h"
#include "Serialization.h"
#include "MsgQueue.h"

// Data types and constants
#include "C64Types.h"

// Loading and saving
#include "Snapshot.h"
#include "T64File.h"
#include "D64File.h"
#include "G64File.h"
#include "PRGFile.h"
#include "Folder.h"
#include "P00File.h"
#include "RomFile.h"
#include "TAPFile.h"
#include "CRTFile.h"
#include "FSDevice.h"

// Sub components
#include "ExpansionPort.h"
#include "IEC.h"
#include "Keyboard.h"
#include "ControlPort.h"
#include "C64Memory.h"
#include "DriveMemory.h"
#include "FlashRom.h"
#include "VICII.h"
#include "SIDBridge.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"
#include "Oscillator.h"
#include "RegressionTester.h"
#include "RetroShell.h"

// Cartridges
#include "Cartridge.h"
#include "CustomCartridges.h"

// Peripherals
#include "Drive.h"
#include "Datasette.h"
#include "Mouse.h"


/* A complete virtual C64. This class is the most prominent one of all. To run
 * the emulator, it is sufficient to create a single object of this type. All
 * subcomponents are created automatically. The public API gives you control
 * over the emulator's behaviour such as running and pausing the emulation.
 * Please note that most subcomponents have their own public API. E.g., to
 * query information from VICII, you need to invoke a method on c64.vicii.
 */
class C64 : public HardwareComponent {
        
    // The currently set inspection target (only evaluated in debug mode)
    InspectionTarget inspectionTarget;

    
    //
    // Sub components
    //
    
public:
    
    // Core components
    C64Memory mem = C64Memory(*this);
    C64CPU cpu = C64CPU(*this, mem);
    VICII vic = VICII(*this);
    CIA1 cia1 = CIA1(*this);
    CIA2 cia2 = CIA2(*this);
    SIDBridge sid = SIDBridge(*this);

    // Logic board
    Oscillator oscillator = Oscillator(*this);
    
    // Keyboard
    Keyboard keyboard = Keyboard(*this);
    
    // Control ports
    ControlPort port1 = ControlPort(*this, PORT_ONE);
    ControlPort port2 = ControlPort(*this, PORT_TWO);
    
    // Expansion port (cartridge port)
    ExpansionPort expansionport = ExpansionPort(*this);
    
    // IEC bus (connects the VC1541 floppy drives)
    IEC iec = IEC(*this);
    
    // Floppy drives
    Drive drive8 = Drive(DRIVE8, *this);
    Drive drive9 = Drive(DRIVE9, *this);
    
    // Datasette
    Datasette datasette = Datasette(*this);
    
    // Command console
 //   RetroShell retroShell = RetroShell(*this);
    
    // Communication channel to the GUI
    MsgQueue msgQueue = MsgQueue(*this);

    // Regression test manager
    RegressionTester regressionTester;
    
    
    //
    // Frame, rasterline, and rasterline cycle information
    //
    
    // The total number of frames drawn since power up
    u64 frame;
    
    /* The currently drawn rasterline. The first rasterline is numbered 0. The
     * number of the last rasterline varies between PAL and NTSC models.
     */
    u16 rasterLine;
    
    /* The currently executed rasterline cycle. The first rasterline cycle is
     * numbered 1. The number of the last cycle varies between PAL and NTSC
     * models.
     */
    u8 rasterCycle;
    
    // Clock frequency
    u32 frequency;
    
    // Duration of a CPU cycle in 1/10 nano seconds
    u64 durationOfOneCycle;
    
    /* The VICII function table. Each entry in this table is a pointer to a
     * VICII method executed in a certain rasterline cycle. vicfunc[0] is a
     * stub. It is never called, because the first cycle is numbered 1.
     */
    void (VICII::*vicfunc[66])(void);
    
    
    //
    // Emulator thread
    //
    
private:
    
    // The current emulator state
    EmulatorState state = EMULATOR_STATE_OFF;
    
    /* Run loop control. This variable is checked at the end of each runloop
     * iteration. Most of the time, the variable is 0 which causes the runloop
     * to repeat. A value greater than 0 means that one or more runloop control
     * flags are set. These flags are flags processed and the loop either
     * repeats or terminates depending on the provided flags.
     */
    u32 runLoopCtrl = 0;
        
    // The invocation counter for implementing suspend() / resume()
    isize suspendCounter = 0;
    
    // The emulator thread
    pthread_t p = (pthread_t)0;
    

    //
    // Operation modes
    //
    
    /* Indicates if the emulator should be executed in warp mode. To speed up
     * emulation (e.g., during disk accesses), the virtual hardware may be put
     * into warp mode. In this mode, the emulation thread is no longer paused
     * to match the target frequency and runs as fast as possible.
     */
    bool warp = false;
    
    /* Indicates if the current warp mode is locked. By default, this variable
     * false. It is set to true by the regression tester to prevent the GUI
     * from disabling warp mode during an ongoing regression test.
     */
    bool warpLock = false;
    
    /* Indicates whether C64 is running in ultimax mode. Ultimax mode can be
     * enabled by external cartridges by pulling game line low and keeping
     * exrom line high. In ultimax mode, most of the C64's RAM and ROM is
     * invisible.
     */
    bool ultimax = false;
    
    
    //
    // Snapshot storage
    //
    
private:
    
    Snapshot *autoSnapshot = nullptr;
    Snapshot *userSnapshot = nullptr;
    
    //
    // Initializing
    //
    
public:
    
    C64();
    ~C64();
    const char *getDescription() const override { return "C64"; }
    void prefix() const override;

    // Prepares the emulator for regression testing
    void initialize(C64Model model);

    void reset(bool hard);
    void hardReset() { reset(true); }
    void softReset() { reset(false); }

private:
    
    void _reset(bool hard) override;

    
    //
    // Configuring
    //

public:
    
    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;
    
    bool configure(Option option, i64 value) throws;
    bool configure(Option option, long id, i64 value) throws;

    // Configures the C64 to match a specific C64 model
    void configure(C64Model model);
        
    // Updates the VICII function table according to the selected model
    void updateVicFunctionTable();

private:

    bool setConfigItem(Option option, i64 value) override;

    
    //
    // Analyzing
    //
    
public:
       
    void inspect();
    InspectionTarget getInspectionTarget() const;
    void setInspectionTarget(InspectionTarget target);
    void clearInspectionTarget() { setInspectionTarget(INSPECTION_TARGET_NONE); }
    
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
        
        << frequency
        << durationOfOneCycle;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {
            
            worker
            
            << frame
            << rasterLine
            << rasterCycle
            << ultimax;
        }
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Controlling
    //
    
public:

    bool isPoweredOff() const override { return state == EMULATOR_STATE_OFF; }
    bool isPoweredOn() const override { return state != EMULATOR_STATE_OFF; }
    bool isPaused() const override { return state == EMULATOR_STATE_PAUSED; }
    bool isRunning() const override { return state == EMULATOR_STATE_RUNNING; }

    void powerOn();
    void powerOff();
    void run();
    void pause();
    void shutdown();
    
    void setWarp(bool enable);
    bool inWarpMode() const { return warp; }
    void lockWarpMode() { warpLock = true; }
    void unlockWarpMode() { warpLock = false; }

    void setDebug(bool enable);
    bool inDebugMode() const { return debugMode; }
    
private:

    void _powerOn() override;
    void _powerOff() override;
    void _run() override;
    void _pause() override;
    void _setWarp(bool enable) override;
    void _setDebug(bool enable) override;


    //
    // Working with the emulator thread
    //

public:
    
    // Returns true if the currently executed thread is the emulator thread
    bool isEmulatorThread() { return pthread_self() == p; }

    /* Returns true if a call to powerOn() will be successful.
     * It returns false, e.g., if no Rom is installed.
     */
    bool isReady(ErrorCode *err = nullptr) const;
    
    
    //
    // Accessing the message queue
    //
    
public:
        
    // Feeds a notification message into message queue
    void putMessage(MsgType msg, u64 data = 0) { msgQueue.put(msg, data); }
    
    /* The thread enter function. This (private) method is invoked when the
     * emulator thread launches. It has to be declared public to make it
     * accessible by the emulator thread.
     */
    void threadWillStart();
    
    /* The thread exit function. This (private) method is invoked when the
     * emulator thread terminates. It has to be declared public to make it
     * accessible by the emulator thread.
     */
    void threadDidTerminate();
        
    /* The C64 run loop.
     * This function is one of the most prominent ones. It implements the
     * outermost loop of the emulator and therefore the place where emulation
     * starts. If you want to understand how the emulator works, this function
     * should be your starting point.
     */
    void runLoop();
    
    /* Runs or pauses the emulator.
     */
    void stopAndGo();

    /* Executes a single instruction.
     * This function is used for single-stepping through the code inside the
     * debugger. It starts the execution thread and terminates it after the
     * next instruction has been executed.
     */
    void stepInto();
    
    /* Emulates the C64 until the instruction following the current one is
     * reached. This function is used for single-stepping through the code
     * inside the debugger. It sets a soft breakpoint to PC+n where n is the
     * length bytes of the current instruction and starts the emulator thread.
     */
    void stepOver();
    
    /* Emulates the C64 until the end of the current frame. Under certain
     * circumstances the function may terminate earlier, in the middle of a
     * frame. This happens, e.g., if the CPU jams or a breakpoint is reached.
     * It is save to call the function in the middle of a frame. In this case,
     * the C64 is emulated until the curent frame has been completed.
     */
    void executeOneFrame();
    
    /* Emulates the C64 until the end of the current rasterline. This function
     * is called inside executeOneFrame().
     */
    void executeOneLine();
    
    // Executes a single clock cycle
    void executeOneCycle();
    void _executeOneCycle();

    /* Finishes the current instruction. This function is called when the
     * emulator threads terminates in order to reach a clean state. It emulates
     * the CPU until the next fetch cycle is reached.
     */
    void finishInstruction();
    
    // Finishes the current frame
    void finishFrame();
    
private:
    
    // Invoked before executing the first cycle of a rasterline
    void beginRasterLine();
    
    // Invoked after executing the last cycle of a rasterline
    void endRasterLine();
    
    // Invoked after executing the last rasterline of a frame
    void endFrame();
    
    
    //
    // Managing the emulator thread
    //
    
public:
    
    /* Pauses the emulation thread temporarily. Because the emulator is running
     * in a separate thread, the GUI has to pause the emulator before changing
     * it's internal state. This is done by embedding the code inside a
     * suspend / resume block:
     *
     *           suspend();
     *           do something with the internal state;
     *           resume();
     *
     * It it safe to nest multiple suspend() / resume() blocks.
     */
    void suspend();
    void resume();
    
    /* Sets or clears a run loop control flag. The functions are thread-safe
     * and can be called from inside or outside the emulator thread.
     */
    void setActionFlags(u32 flags);
    void clearActionFlags(u32 flags);
    
    // Convenience wrappers for controlling the run loop
    void signalAutoSnapshot() { setActionFlags(ACTION_FLAG_AUTO_SNAPSHOT); }
    void signalUserSnapshot() { setActionFlags(ACTION_FLAG_USER_SNAPSHOT); }
    void signalBreakpoint() { setActionFlags(ACTION_FLAG_BREAKPOINT); }
    void signalWatchpoint() { setActionFlags(ACTION_FLAG_WATCHPOINT); }
    void signalInspect() { setActionFlags(ACTION_FLAG_INSPECT); }
    void signalJammed() { setActionFlags(ACTION_FLAG_CPU_JAM); }
    void signalStop() { setActionFlags(ACTION_FLAG_STOP); }
    void signalExpPortNmi() { setActionFlags(ACTION_FLAG_EXTERNAL_NMI); }

    //
    // Handling snapshots
    //
    
public:
    
    /* Requests a snapshot to be taken. Once the snapshot is ready, a message
     * is written into the message queue. The snapshot can then be picked up by
     * calling latestAutoSnapshot() or latestUserSnapshot(), depending on the
     * requested snapshot type.
     */
    void requestAutoSnapshot();
    void requestUserSnapshot();

    // Returns the most recent snapshot or nullptr if none was taken
    Snapshot *latestAutoSnapshot();
    Snapshot *latestUserSnapshot();
    
    /* Loads the current state from a snapshot file. This function is not
     * thread-safe and must not be called on a running emulator.
     */
    bool loadFromSnapshot(Snapshot *snapshot);
    
    
    //
    // Handling Roms
    //
    
public:
    
    // Computes a Rom checksum
    u32 romCRC32(RomType type) const;
    u64 romFNV64(RomType type) const;
     
    // Returns a unique identifier for the installed ROMs
    RomIdentifier romIdentifier(RomType type) const;
    
    // Returns printable titles for the installed ROMs
    const char *romTitle(RomType type) const;
    
    // Returns printable sub titles for the installed ROMs
    const char *romSubTitle(u64 fnv) const;
    const char *romSubTitle(RomType type) const;
    
    // Returns printable revision strings or hash values for the installed ROMs
    const char *romRevision(RomType type) const;
    
    // Checks if a certain Rom is present
    bool hasRom(RomType type) const;
    bool hasMega65Rom(RomType type) const;

private:
    
    // Returns a revision string if a Mega65 Rom is installed
    char *mega65BasicRev() const;
    char *mega65KernalRev() const;

public:
    
    // Installs a Rom
    void loadRom(const string &path) throws;
    void loadRom(const string &path, ErrorCode *ec);
    void loadRom(RomFile *file);
    
    // Erases an installed Rom
    void deleteRom(RomType type);
    
    // Saves a Rom to disk
    void saveRom(RomType rom, const string &path) throws;
    void saveRom(RomType rom, const string &path, ErrorCode *ec);

    
    //
    // Flashing files
    //
    
    // Flashes a single file into memory
    bool flash(AnyFile *file);
    bool flash(AnyCollection *file, isize item);
    bool flash(const FSDevice &fs, isize item);
    
    //
    // Set and query ultimax mode
    //
    
public:
    
    // Returns the ultimax flag
    bool getUltimax() const { return ultimax; }
    
    /* Setter for ultimax mode. When the peek / poke lookup table is updated,
     * this function is called if a certain combination is present on the Game
     * and Exrom lines.
     */
    void setUltimax(bool b) { ultimax = b; }
};
