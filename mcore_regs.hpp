#pragma once
#include <cstdint>
#include <mcore_utils>


     /* 
    --------------------------------------------------------------------------------
    ------------------------ Coretex-M7 Register Descripton ------------------------
    ------------------------ Generating from file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/system_registers_m7.xml ------------------------
    ------------------------ Generating from file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/nvic_registers_v7m.xml ------------------------
    ------------------------ Generating from file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/cache_registers_v7em.xml ------------------------
    ------------------------ Generating from file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/mpu_registers_v7m.xml ------------------------
    --------------------------------------------------------------------------------
    */
// Generated from xml file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/system_registers_m7.xml
// --------------------------------------------
//Control: System Control registers
// --------------------------------------------
namespace Control {

    // ACTLR: Disables certain aspects of functionality within the processor
    struct _ACTLR : Register <0xE000E008, ReadWrite, _ACTLR> {
        // Disables floating point instructions completing out of order with respect to integer instructions
        using DISOOFP = Field <_ACTLR, 9, 1>; // Bits [9]
        // Disable automatic update of CONTROL.FPCA
        using DISFPCA = Field <_ACTLR, 8, 1>; // Bits [8]
        // Disables folding of IT instructions
        using DISFOLD = Field <_ACTLR, 2, 1>; // Bits [2]
        // Disables write buffer use during default memory map accesses
        using DISDEFWBUF = Field <_ACTLR, 1, 1>; // Bits [1]
        // Disables interruption of multi-cycle instructions
        using DISMCYCINT = Field <_ACTLR, 0, 1>; // Bits [0]
    };

    // ICSR: Provides software control of the NMI, PendSV, and SysTick exceptions, and provides interrupt status information
    struct _ICSR : Register <0xE000ED04, ReadWrite, _ICSR> {
        // Activates an NMI exception or reads back the current state
        using NMIPENDSET = Field <_ICSR, 31, 1>; // Bits [31]
        // Sets a pending PendSV interrupt or reads back the current state
        using PENDSVSET = Field <_ICSR, 28, 1>; // Bits [28]
        // Clears a pending PendSV interrupt
        using PENDSVCLR = Field <_ICSR, 27, 1>; // Bits [27]
        // Sets a pending SysTick or reads back the current state
        using PENDSTSET = Field <_ICSR, 26, 1>; // Bits [26]
        // Clears a pending SysTick, whether set here or by the timer hardware
        using PENDSTCLR = Field <_ICSR, 25, 1>; // Bits [25]
        // Indicates whether a pending exception will be serviced on exit from debug halt state
        using ISRPREEMPT = Field <_ICSR, 23, 1>; // Bits [23]
        // Indicates if an external configurable, NVIC generated, interrupt is pending
        using ISRPENDING = Field <_ICSR, 22, 1>; // Bits [22]
        // The exception number for the highest priority pending exception. 0 indicates no pending exceptions
        using VECTPENDING = Field <_ICSR, 12, 9>; // Bits [20:12]
        // In Handler mode, indicates whether there is an active exception other than the exception indicated by the current value of the IPSR
        using RETTOBASE = Field <_ICSR, 11, 1>; // Bits [11]
        // The exception number for the current executing exception
        using VECTACTIVE = Field <_ICSR, 0, 9>; // Bits [8:0]
    };

    // VTOR: Holds the vector table address
    struct _VTOR : Register <0xE000ED08, ReadWrite, _VTOR> {
        // Bits [31:7] of the vector table address
        using TBLOFF = Field <_VTOR, 7, 25>; // Bits [31:7]
        // Determines whether the vector table is in the code or SRAM memory region
        using TBLBASE = Field <_VTOR, 29, 1>; // Bits [29]
    };

    // DEMCR: Manages vector catch behaviour and DebugMonitor handling when debugging
    struct _DEMCR : Register <0xE000EDFC, ReadOnly, _DEMCR> {
        // Enable the DebugMonitor exception
        using MON_EN = Field <_DEMCR, 16, 1>; // Bits [16]
        // Sets or clears the pending state of the DebugMonitor exception
        using MON_PEND = Field <_DEMCR, 17, 1>; // Bits [17]
    };

    // AIRCR: Sets or returns interrupt control data
    struct _AIRCR : Register <0xE000ED0C, ReadWrite, _AIRCR> {
        // Vector Key
        using VECTKEY = Field <_AIRCR, 16, 16>; // Bits [31:16]
        // UNKNOWN
        using VECTKEYSTAT = Field <_AIRCR, 16, 16>; // Bits [31:16]
        // Indicates the memory system data endianness
        using ENDIANNESS = Field <_AIRCR, 15, 1>; // Bits [15]
        // Priority grouping, indicates the binary point position.
        using PRIGROUP = Field <_AIRCR, 8, 3>; // Bits [10:8]
        // System Reset Request
        using SYSRESETREQ = Field <_AIRCR, 2, 1>; // Bits [2]
        // Clears all active state information for fixed and configurable exceptions
        using VECTCLRACTIVE = Field <_AIRCR, 1, 1>; // Bits [1]
    };

    // SCR: Sets or returns system control data
    struct _SCR : Register <0xE000ED10, ReadWrite, _SCR> {
        // Determines whether an interrupt transition from inactive state to pending state is a wakeup event
        using SEVONPEND = Field <_SCR, 4, 1>; // Bits [4]
        // Hint indicating that waking from sleep might take longer
        using SLEEPDEEP = Field <_SCR, 2, 1>; // Bits [2]
        // whether, on an exit from an ISR that returns to the base level of execution priority, the processor enters a sleep state
        using SLEEPONEXIT = Field <_SCR, 1, 1>; // Bits [1]
    };

    // CCR: Configuration and Control
    struct _CCR : Register <0xE000ED14, ReadWrite, _CCR> {
        // Setting this bit to 1 enables branch prediction, also called program flow prediction
        using BP = Field <_CCR, 18, 1>; // Bits [18]
        // This is a global enable bit for instruction caches
        using IC = Field <_CCR, 17, 1>; // Bits [17]
        // This is a global enable bit for data and unified caches
        using DC = Field <_CCR, 16, 1>; // Bits [16]
        // Determines whether the exception entry sequence guarantees 8-byte stack frame alignment, adjusting the SP if necessary before saving state
        using STKALIGN = Field <_CCR, 9, 1>; // Bits [9]
        // Determines the effect of precise data access faults on handlers running at priority -1 or priority -2
        using BFHFNMIGN = Field <_CCR, 8, 1>; // Bits [8]
        // Controls the trap on divide by 0
        using DIV_0_TRP = Field <_CCR, 4, 1>; // Bits [4]
        // Controls the trapping of unaligned word or halfword accesses
        using UNALIGN_TRP = Field <_CCR, 3, 1>; // Bits [3]
        // Controls whether unprivileged software can access the STIR
        using USERSETMPEND = Field <_CCR, 1, 1>; // Bits [1]
        // Controls whether the processor can enter Thread mode at an execution priority level other than base level
        using NONBASETHRDENA = Field <_CCR, 0, 1>; // Bits [0]
    };

    // SHPR1: Sets or returns priority for system handlers 4-7
    struct _SHPR1 : Register <0xE000ED18, ReadWrite, _SHPR1> {
        // Priority of system handler 7
        using PRI_7 = Field <_SHPR1, 24, 8>; // Bits [31:24]
        // Priority of system handler 6, UsageFault
        using PRI_6 = Field <_SHPR1, 16, 8>; // Bits [23:16]
        // Priority of system handler 5, BusFault
        using PRI_5 = Field <_SHPR1, 8, 8>; // Bits [15:8]
        // Priority of system handler 4, MemManage
        using PRI_4 = Field <_SHPR1, 0, 8>; // Bits [7:0]
    };

    // SHPR2: Sets or returns priority for system handlers 8-11
    struct _SHPR2 : Register <0xE000ED1C, ReadWrite, _SHPR2> {
        // Priority of system handler 11, SVCall
        using PRI_11 = Field <_SHPR2, 24, 8>; // Bits [31:24]
        // Priority of system handler 10
        using PRI_10 = Field <_SHPR2, 16, 8>; // Bits [23:16]
        // Priority of system handler 9
        using PRI_9 = Field <_SHPR2, 8, 8>; // Bits [15:8]
        // Priority of system handler 8
        using PRI_8 = Field <_SHPR2, 0, 8>; // Bits [7:0]
    };

    // SHPR3: Sets or returns priority for system handlers 12-15
    struct _SHPR3 : Register <0xE000ED20, ReadWrite, _SHPR3> {
        // Priority of system handler 15, SysTick
        using PRI_15 = Field <_SHPR3, 24, 8>; // Bits [31:24]
        // Priority of system handler 14, PendSV
        using PRI_14 = Field <_SHPR3, 16, 8>; // Bits [23:16]
        // Priority of system handler 13
        using PRI_13 = Field <_SHPR3, 8, 8>; // Bits [15:8]
        // Priority of system handler 4, DebugMonitor
        using PRI_12 = Field <_SHPR3, 0, 8>; // Bits [7:0]
    };

    // SHCSR: Controls and provides the active and pending status of system exceptions
    struct _SHCSR : Register <0xE000ED24, ReadWrite, _SHCSR> {
        // Enable UsageFault
        using USGFAULTENA = Field <_SHCSR, 18, 1>; // Bits [18]
        // Enable BusFault
        using BUSFAULTENA = Field <_SHCSR, 17, 1>; // Bits [17]
        // Enable MemManage fault
        using MEMFAULTENA = Field <_SHCSR, 16, 1>; // Bits [16]
        // SVCall pending
        using SVCALLPENDED = Field <_SHCSR, 15, 1>; // Bits [15]
        // BusFault pending
        using BUSFAULTPENDED = Field <_SHCSR, 14, 1>; // Bits [14]
        // MemManage pending
        using MEMFAULTPENDED = Field <_SHCSR, 13, 1>; // Bits [13]
        // UsageFault pending
        using USGFAULTPENDED = Field <_SHCSR, 12, 1>; // Bits [12]
        // SysTick active
        using SYSTICKACT = Field <_SHCSR, 11, 1>; // Bits [11]
        // PendSV active
        using PENDSVACT = Field <_SHCSR, 10, 1>; // Bits [10]
        // Monitor active
        using MONITORACT = Field <_SHCSR, 8, 1>; // Bits [8]
        // SVCall active
        using SVCALLACT = Field <_SHCSR, 7, 1>; // Bits [7]
        // UsageFault active
        using USGFAULTACT = Field <_SHCSR, 3, 1>; // Bits [3]
        // BusFault active
        using BUSFAULTACT = Field <_SHCSR, 1, 1>; // Bits [1]
        // MemManage active
        using MEMFAULTACT = Field <_SHCSR, 0, 1>; // Bits [0]
    };

    // CFSR: Contains the three Configurable Fault Status Registers MMFSR, BFSR, UFSR
    struct _CFSR : Register <0xE000ED28, ReadWrite, _CFSR> {
        // Divide by zero error
        using DIVBYZERO = Field <_CFSR, 25, 1>; // Bits [25]
        // Unaligned access error
        using UNALIGNED = Field <_CFSR, 24, 1>; // Bits [24]
        // Coprocessor access error
        using NOCP = Field <_CFSR, 19, 1>; // Bits [19]
        // Integrity check error on EXC_RETURN
        using INVPC = Field <_CFSR, 18, 1>; // Bits [18]
        // Instruction executed with invalid EPSR.T or EPSR.IT field
        using INVSTATE = Field <_CFSR, 17, 1>; // Bits [17]
        // Processor has attempted to execute an undefined instruction.
        using UNDEFINSTR = Field <_CFSR, 16, 1>; // Bits [16]
        // BFAR has valid contents
        using BFARVALID = Field <_CFSR, 15, 1>; // Bits [15]
        // Bus fault during FP lazy state preservation
        using LSPERR = Field <_CFSR, 13, 1>; // Bits [13]
        // Derived bus fault on exception entry
        using STKERR = Field <_CFSR, 12, 1>; // Bits [12]
        // Derived bus fault on exception return
        using UNSTKERR = Field <_CFSR, 11, 1>; // Bits [11]
        // Imprecise data access error
        using IMPRECISERR = Field <_CFSR, 10, 1>; // Bits [10]
        // Precise data access error
        using PRECISERR = Field <_CFSR, 9, 1>; // Bits [9]
        // Bus fault on an instruction prefetch
        using IBUSERR = Field <_CFSR, 8, 1>; // Bits [8]
        // MMAR has valid contents
        using MMARVALID = Field <_CFSR, 7, 1>; // Bits [7]
        // MemManage fault during FP lazy state preservation
        using MLSPERR = Field <_CFSR, 5, 1>; // Bits [5]
        // Derived MemManage fault on exception entry
        using MSTKERR = Field <_CFSR, 4, 1>; // Bits [4]
        // Derived MemManage fault on exception return
        using MUNSTKERR = Field <_CFSR, 3, 1>; // Bits [3]
        // Data access violation. The MMAR shows the data address that the load or store tried to access
        using DACCVIOL = Field <_CFSR, 1, 1>; // Bits [1]
        // MPU or Execute Never (XN) default memory map access violation on an instruction fetch
        using IACCVIOL = Field <_CFSR, 0, 1>; // Bits [0]
    };

    // HFSR: Shows the cause of any hard faults
    struct _HFSR : Register <0xE000ED2C, ReadWrite, _HFSR> {
        // Indicates a Debug event has occurred
        using DEBUGEVT = Field <_HFSR, 31, 1>; // Bits [31]
        // Indicates a fault with configurable priority has been escalated to a HardFault
        using FORCED = Field <_HFSR, 30, 1>; // Bits [30]
        // Indicates a fault has occurred because of a vector table read error on exception processing
        using VECTTBL = Field <_HFSR, 1, 1>; // Bits [1]
    };

    // DFSR: Shows, at the top level, why a debug event occurred
    struct _DFSR : Register <0xE000ED30, ReadWrite, _DFSR> {
        // Indicates a debug event generated because of the assertion of EDBGRQ
        using EXTERNAL = Field <_DFSR, 4, 1>; // Bits [4]
        // Indicates triggering of a Vector catch
        using VCATCH = Field <_DFSR, 3, 1>; // Bits [3]
        // Indicates a debug event generated by the DWT
        using DWTTRAP = Field <_DFSR, 2, 1>; // Bits [2]
        // Indicates a debug event generated by BKPT instruction execution or a breakpoint match in FPB
        using BKPT = Field <_DFSR, 1, 1>; // Bits [1]
        // Indicates a debug event generated by C_HALT or C_STEP request or setting DEMCR.MON_STEP
        using HALTED = Field <_DFSR, 0, 1>; // Bits [0]
    };

    // MMFAR: Shows the address of the memory location that caused an MMU fault
    struct _MMFAR : Register <0xE000ED34, ReadWrite, _MMFAR> {
    };

    // BFAR: Shows the address associated with a precise data access fault
    struct _BFAR : Register <0xE000ED38, ReadWrite, _BFAR> {
    };

    // AFSR: Latched version of the AUXFAULT inputs
    struct _AFSR : Register <0xE000ED3C, ReadWrite, _AFSR> {
    };

    // CPACR: Specifies the access privileges for coprocessors
    struct _CPACR : Register <0xE000ED88, ReadWrite, _CPACR> {
        // Defines access permissions for the CP11 coprocessor.
        using CP11 = Field <_CPACR, 22, 2>; // Bits [23:22]
        // Defines access permissions for the CP10 coprocessor.
        using CP10 = Field <_CPACR, 20, 2>; // Bits [21:20]
        // Defines access permissions for the CP7 coprocessor.
        using CP7 = Field <_CPACR, 14, 2>; // Bits [15:14]
        // Defines access permissions for the CP6 coprocessor.
        using CP6 = Field <_CPACR, 12, 2>; // Bits [13:12]
        // Defines access permissions for the CP5 coprocessor.
        using CP5 = Field <_CPACR, 10, 2>; // Bits [11:10]
        // Defines access permissions for the CP4 coprocessor.
        using CP4 = Field <_CPACR, 8, 2>; // Bits [9:8]
        // Defines access permissions for the CP3 coprocessor.
        using CP3 = Field <_CPACR, 6, 2>; // Bits [7:6]
        // Defines access permissions for the CP2 coprocessor.
        using CP2 = Field <_CPACR, 4, 2>; // Bits [5:4]
        // Defines access permissions for the CP1 coprocessor.
        using CP1 = Field <_CPACR, 2, 2>; // Bits [3:2]
        // Defines access permissions for the CP0 coprocessor.
        using CP0 = Field <_CPACR, 0, 2>; // Bits [1:0]
    };

    // STIR: Provides a mechanism for software to generate an interrupt
    struct _STIR : Register <0xE000EF00, WriteOnly, _STIR> {
        // Indicates the interrupt to be triggered. The value written is (ExceptionNumber - 16)
        using INTID = Field <_STIR, 0, 9>; // Bits [8:0]
    };

} // namespace Control

// --------------------------------------------
//ID: ID registers
// --------------------------------------------
namespace ID {

    // CPUID: Identification information for the processor
    struct _CPUID : Register <0xE000ED00, ReadOnly, _CPUID> {
        // Implementer code
        using Implementer = Field <_CPUID, 24, 8>; // Bits [31:24]
        // Implementation defined
        using Variant = Field <_CPUID, 20, 4>; // Bits [23:20]
        // Indicates the architecture
        using Constant = Field <_CPUID, 16, 4>; // Bits [19:16]
        // Indicates part number
        using Partno = Field <_CPUID, 4, 12>; // Bits [15:4]
        // Indicates revision
        using Revision = Field <_CPUID, 0, 4>; // Bits [3:0]
    };

    struct _ID_PFR0 : Register <0xE000ED40, ReadOnly, _ID_PFR0> {
        // Thumb instruction set support
        using State1 = Field <_ID_PFR0, 4, 4>; // Bits [7:4]
    };

    struct _ID_PFR1 : Register <0xE000ED44, ReadOnly, _ID_PFR1> {
        // M profile programmers' model
        using M_Profile = Field <_ID_PFR1, 8, 4>; // Bits [11:8]
    };

    struct _ID_DFR0 : Register <0xE000ED48, ReadOnly, _ID_DFR0> {
        // Debug model, M profile
        using M_Profile = Field <_ID_DFR0, 20, 4>; // Bits [23:20]
    };

    // ID_AFR0: Implementation defined features
    struct _ID_AFR0 : Register <0xE000ED4C, ReadOnly, _ID_AFR0> {
    };

    // ID_MMFR0: Memory Model Features
    struct _ID_MMFR0 : Register <0xE000ED50, ReadOnly, _ID_MMFR0> {
        // Indicates support for Auxiliary registers
        using Auxiliary_registers = Field <_ID_MMFR0, 20, 4>; // Bits [23:20]
        // Indicates support for TCM
        using TCM_support = Field <_ID_MMFR0, 16, 4>; // Bits [19:16]
        // Indicates the number of shareability levels implemented
        using Shareability_levels = Field <_ID_MMFR0, 12, 4>; // Bits [15:12]
        // Indicates the outermost shareability domain implemented
        using Outermost_shareability = Field <_ID_MMFR0, 8, 4>; // Bits [11:8]
        // Indicates support for a PMSA
        using PMSA = Field <_ID_MMFR0, 4, 4>; // Bits [7:4]
    };

    // ID_MMFR1: Memory Model Features
    struct _ID_MMFR1 : Register <0xE000ED54, ReadOnly, _ID_MMFR1> {
    };

    // ID_MMFR2: Memory Model Features
    struct _ID_MMFR2 : Register <0xE000ED58, ReadOnly, _ID_MMFR2> {
        // Indicates support for wait-for-interrupt stalling.
        using WFI = Field <_ID_MMFR2, 24, 4>; // Bits [27:24]
    };

    // ID_MMFR3: Memory Model Features
    struct _ID_MMFR3 : Register <0xE000ED5C, ReadOnly, _ID_MMFR3> {
    };

    // ID_ISAR0: Instruction Set Attributes
    struct _ID_ISAR0 : Register <0xE000ED60, ReadOnly, _ID_ISAR0> {
        // Indicates the supported divide instructions
        using Divide_instrs = Field <_ID_ISAR0, 24, 4>; // Bits [27:24]
        // Indicates the supported debug instructions
        using Debug_instrs = Field <_ID_ISAR0, 20, 4>; // Bits [23:20]
        // Indicates the supported coprocessor instructions
        using Coproc_instrs = Field <_ID_ISAR0, 16, 4>; // Bits [19:16]
        // Indicates support for combined compare and branch instructions.
        using CmpBranch_instrs = Field <_ID_ISAR0, 12, 4>; // Bits [15:12]
        // Indicates support for bitfield instructions.
        using Bitfield_instrs = Field <_ID_ISAR0, 8, 4>; // Bits [11:8]
        // Indicates support for bit counting instructions.
        using BitCount_instrs = Field <_ID_ISAR0, 4, 4>; // Bits [7:4]
    };

    // ID_ISAR1: Instruction Set Attributes
    struct _ID_ISAR1 : Register <0xE000ED64, ReadOnly, _ID_ISAR1> {
        // Indicates support for instructions that branch between ARM and Thumb code.
        using Interwork_instrs = Field <_ID_ISAR1, 24, 4>; // Bits [27:24]
        // Indicates support for immediate instructions.
        using Immediate_instrs = Field <_ID_ISAR1, 20, 4>; // Bits [23:20]
        // Indicates support for IfThen instructions.
        using IfThen_instrs = Field <_ID_ISAR1, 16, 4>; // Bits [19:16]
        // Indicates support for sign or zero extend instructions.
        using Extend_instrs = Field <_ID_ISAR1, 12, 4>; // Bits [15:12]
    };

    // ID_ISAR2: Instruction Set Attributes
    struct _ID_ISAR2 : Register <0xE000ED68, ReadOnly, _ID_ISAR2> {
        // Indicates the supported reversal instructions.
        using Reversal_instrs = Field <_ID_ISAR2, 28, 4>; // Bits [31:28]
        // Indicates the supported advanced unsigned multiply instructions.
        using MultU_instrs = Field <_ID_ISAR2, 20, 4>; // Bits [23:20]
        // Indicates the supported advanced signed multiply instructions.
        using MultS_instrs = Field <_ID_ISAR2, 16, 4>; // Bits [19:16]
        // Indicates the supported additional multiply instructions.
        using Mult_instrs = Field <_ID_ISAR2, 12, 4>; // Bits [15:12]
        // Indicates the supported multi-access interruptible instructions.
        using MultiAccessInt_instrs = Field <_ID_ISAR2, 8, 4>; // Bits [11:8]
        // Indicates the supported memory hint instructions.
        using MultiAccessInt_instrs = Field <_ID_ISAR2, 4, 4>; // Bits [7:4]
        // Indicates the supported additional load and store instructions.
        using LoadStore_instrs = Field <_ID_ISAR2, 0, 4>; // Bits [3:0]
    };

    // ID_ISAR3: Instruction Set Attributes
    struct _ID_ISAR3 : Register <0xE000ED6C, ReadOnly, _ID_ISAR3> {
        // Indicates support for true NOP instructions.
        using TrueNOP_instrs = Field <_ID_ISAR3, 24, 4>; // Bits [27:24]
        // Indicates support for Thumb copy instructions.
        using ThumbCopy_instrs = Field <_ID_ISAR3, 20, 4>; // Bits [23:20]
        // Indicates support for table branch instructions.
        using TabBranch_instrs = Field <_ID_ISAR3, 16, 4>; // Bits [19:16]
        // Indicates support for synchronization primitive instructions.
        using SynchPrim_instrs = Field <_ID_ISAR3, 12, 4>; // Bits [15:12]
        // Indicates support for SVC instructions.
        using SVC_instrs = Field <_ID_ISAR3, 8, 4>; // Bits [11:8]
        // Indicates support for Single Instruction Multiple Data (SIMD) instructions.
        using SIMD_instrs = Field <_ID_ISAR3, 4, 4>; // Bits [7:4]
        // Indicates support for saturate instructions.
        using Saturate_instrs = Field <_ID_ISAR3, 0, 4>; // Bits [3:0]
    };

    // ID_ISAR4: Instruction Set Attributes
    struct _ID_ISAR4 : Register <0xE000ED70, ReadOnly, _ID_ISAR4> {
        // Indicates support for saturate instructions.
        using PSR_M_instrs = Field <_ID_ISAR4, 24, 4>; // Bits [27:24]
        // Indicates support for Synchronization Primitives
        using SynchPrim_instrs_frac = Field <_ID_ISAR4, 20, 4>; // Bits [23:20]
        // Indicates the supported barrier instructions.
        using Barrier_instrs = Field <_ID_ISAR4, 16, 4>; // Bits [19:16]
        // Indicates support for Writeback addressing modes
        using Writeback_instrs = Field <_ID_ISAR4, 8, 4>; // Bits [11:8]
        // Indicates the support for instructions with shifts:
        using WithShifts_instrs = Field <_ID_ISAR4, 4, 4>; // Bits [7:4]
        // Indicates the supported unprivileged instructions
        using Unpriv_instrs = Field <_ID_ISAR4, 0, 4>; // Bits [3:0]
    };

} // namespace ID

// --------------------------------------------
//FPE: FP System Control registers
// --------------------------------------------
namespace FPE {

    // FPCCR: Holds control data for the floating-point unit
    struct _FPCCR : Register <0xE000EF34, ReadWrite, _FPCCR> {
        // When this bit is set to 1, execution of a floating-point instruction sets the CONTROL.FPCA bit to 1
        using ASPEN = Field <_FPCCR, 31, 1>; // Bits [31]
        // Enables lazy context save of FP state
        using LSPEN = Field <_FPCCR, 30, 1>; // Bits [30]
        // Indicates whether the software executing when the processor allocated the FP stack frame was able to set the DebugMonitor exception to pending
        using MONRDY = Field <_FPCCR, 8, 1>; // Bits [8]
        // Indicates whether the software executing when the processor allocated the FP stack frame was able to set the BusFault exception to pending
        using BFRDY = Field <_FPCCR, 6, 1>; // Bits [6]
        // Indicates whether the software executing when the processor allocated the FP stack frame was able to set the MemManage exception to pending
        using MMRDY = Field <_FPCCR, 5, 1>; // Bits [5]
        // Indicates whether the software executing when the processor allocated the FP stack frame was able to set the HardFault exception to pending
        using HFRDY = Field <_FPCCR, 4, 1>; // Bits [4]
        // Indicates the processor mode when it allocated the FP stack frame
        using THREAD = Field <_FPCCR, 3, 1>; // Bits [3]
        // Indicates the privilege level of the software executing when the processor allocated the FP stack frame
        using USER = Field <_FPCCR, 1, 1>; // Bits [1]
        // Indicates whether Lazy preservation of the FP state is active
        using LSPACT = Field <_FPCCR, 0, 1>; // Bits [0]
    };

    // FPCAR: Holds the location of the unpopulated floating-point register space allocated on an exception stack frame
    struct _FPCAR : Register <0xE000EF38, ReadWrite, _FPCAR> {
    };

    // FPDSCR: Holds the default values for the floating-point status control data that the processor assigns to the FPSCR when it creates a new floating-point context
    struct _FPDSCR : Register <0xE000EF3C, ReadWrite, _FPDSCR> {
        // Default value for FPSCR.AHP
        using AHP = Field <_FPDSCR, 26, 1>; // Bits [26]
        // Default value for FPSCR.DN
        using DN = Field <_FPDSCR, 25, 1>; // Bits [25]
        // Default value for FPSCR.FZ
        using FZ = Field <_FPDSCR, 24, 1>; // Bits [24]
        // Default value for FPSCR.RMode
        using RMode = Field <_FPDSCR, 22, 2>; // Bits [23:22]
    };

    // MVFR0: Describes the features provided by the Floating-point extension
    struct _MVFR0 : Register <0xE000EF40, ReadOnly, _MVFR0> {
        // Indicates the rounding modes supported by the FP floating-point hardware
        using FP_Rounding_modes = Field <_MVFR0, 28, 4>; // Bits [31:28]
        // Indicates the hardware support for FP short vectors
        using Short_vectors = Field <_MVFR0, 24, 4>; // Bits [27:24]
        // Indicates the hardware support for FP square root operations
        using Square_root = Field <_MVFR0, 20, 4>; // Bits [23:20]
        // Indicates the hardware support for FP divide operations
        using Divide = Field <_MVFR0, 16, 4>; // Bits [19:16]
        // Indicates whether the FP hardware implementation supports exception trapping
        using FP_exception_trapping = Field <_MVFR0, 12, 4>; // Bits [15:12]
        // Indicates the hardware support for FP double-precision operations
        using Double_precision = Field <_MVFR0, 8, 4>; // Bits [11:8]
        // Indicates the hardware support for FP single-precision operations
        using Single_precision = Field <_MVFR0, 4, 4>; // Bits [7:4]
        // Indicates the size of the FP register bank
        using A_SIMD = Field <_MVFR0, 0, 4>; // Bits [3:0]
    };

    // MVFR1: Describes the features provided by the Floating-point extension
    struct _MVFR1 : Register <0xE000EF44, ReadOnly, _MVFR1> {
        // Indicates whether the FP supports fused multiply accumulate operations
        using FP_fused_MAC = Field <_MVFR1, 28, 4>; // Bits [31:28]
        // Indicates whether the FP supports half-precision floating-point conversion operations
        using FP_HPFP = Field <_MVFR1, 24, 4>; // Bits [27:24]
        // Indicates whether the FP hardware implementation supports only the Default NaN mode
        using D_NaN = Field <_MVFR1, 4, 4>; // Bits [7:4]
        // Indicates whether the FP hardware implementation supports only the Flush-to-Zero mode of operation
        using FtZ = Field <_MVFR1, 0, 4>; // Bits [3:0]
    };

    // MVFR2: Describes the features provided by the Floating-point extension
    struct _MVFR2 : Register <0xE000EF48, ReadOnly, _MVFR2> {
        // Indicates whether the FP hardware implementation supports Floating-point selection, Floating-point Conversion to Integer with Direct Rounding Modes, Floating-point Round to Integral Floating-point and Floating-point MaxNum and MinNum
        using VFP_Misc = Field <_MVFR2, 4, 4>; // Bits [7:4]
    };

} // namespace FPE

// --------------------------------------------
//SysTick: System Timer registers
// --------------------------------------------
namespace SysTick {

    // STCSR: Controls the system timer and provides status data
    struct _STCSR : Register <0xE000E010, ReadWrite, _STCSR> {
        // Indicates whether the counter has counted to 0 since the last read of this register
        using COUNTFLAG = Field <_STCSR, 16, 1>; // Bits [16]
        // Indicates the SysTick clock source
        using CLKSOURCE = Field <_STCSR, 2, 1>; // Bits [2]
        // Indicates whether counting to 0 causes the status of the SysTick exception to change to pending
        using TICKINT = Field <_STCSR, 1, 1>; // Bits [1]
        // Indicates the enabled status of the SysTick counter
        using ENABLE = Field <_STCSR, 0, 1>; // Bits [0]
    };

    // STRVR: Sets or reads the reload value of the SYST_CVR register
    struct _STRVR : Register <0xE000E014, ReadWrite, _STRVR> {
        // The value to load into the SYST_CVR register when the counter reaches 0
        using RELOAD = Field <_STRVR, 0, 24>; // Bits [23:0]
    };

    // STCVR: Reads or clears the current counter value
    struct _STCVR : Register <0xE000E018, ReadWrite, _STCVR> {
        // This is the value of the counter at the time it is sampled
        using CURRENT = Field <_STCVR, 0, 24>; // Bits [23:0]
    };

    // STCR: Reads the calibration value and parameters for SysTick
    struct _STCR : Register <0xE000E01C, ReadOnly, _STCR> {
        // Indicates whether the IMPLEMENTATION DEFINED reference clock is provided
        using NOREF = Field <_STCR, 31, 1>; // Bits [31]
        // Indicates whether the 10ms calibration value is exact
        using SKEW = Field <_STCR, 30, 1>; // Bits [30]
        // Optionally, holds a reload value to be used for 10ms (100Hz) timing, subject to system clock skew errors
        using TENMS = Field <_STCR, 0, 24>; // Bits [23:0]
    };

} // namespace SysTick

// --------------------------------------------
//ImpDef: Implementation defined registers
// --------------------------------------------
namespace ImpDef {

    // ITCMCR: Controls whether an access is mapped to TCM or AXIM
    struct _ITCMCR : Register <0xE000EF90, ReadWrite, _ITCMCR> {
        // Indicates the size of the relevant TCM
        using SZ = Field <_ITCMCR, 3, 4>; // Bits [6:3]
        // When enabled the processor will guarantee to honour the retry output on the corresponding TCM interface
        using RETEN = Field <_ITCMCR, 2, 1>; // Bits [2]
        // Indicates that all sub-chunk writes to a given TCM use a RMW sequence
        using RMW = Field <_ITCMCR, 1, 1>; // Bits [1]
        // Enable TCM. When a TCM is disabled all accesses are made to the AXIM
        using EN = Field <_ITCMCR, 0, 1>; // Bits [0]
    };

    // DTCMCR: Controls whether an access is mapped to TCM or AXIM
    struct _DTCMCR : Register <0xE000EF94, ReadWrite, _DTCMCR> {
        // Indicates the size of the relevant TCM
        using TCM_Size = Field <_DTCMCR, 3, 4>; // Bits [6:3]
        // When enabled the processor will guarantee to honour the retry output on the corresponding TCM interface
        using RETEN = Field <_DTCMCR, 2, 1>; // Bits [2]
        // Indicates that all sub-chunk writes to a given TCM use a RMW sequence
        using RMW = Field <_DTCMCR, 1, 1>; // Bits [1]
        // Enable TCM. When a TCM is disabled all accesses are made to the AXIM
        using EN = Field <_DTCMCR, 0, 1>; // Bits [0]
    };

    // AHBSCR: Controls the priority of AHBS traffic
    struct _AHBSCR : Register <0xE000EFA0, ReadWrite, _AHBSCR> {
        // Use the to demote access priority of the requestor selected by the AHBSCR.CTL
        using INITCOUNT = Field <_AHBSCR, 11, 5>; // Bits [15:11]
        // Threshold execution priority for AHBS traffic demotion.
        using TPRI = Field <_AHBSCR, 2, 9>; // Bits [10:2]
        // AHBS prioritisation control
        using CTL = Field <_AHBSCR, 0, 2>; // Bits [1:0]
    };

    // CACR: Controls L1 ECC and the L1 cache coherency usage model
    struct _CACR : Register <0xE000EF9C, ReadWrite, _CACR> {
        // Enables Force WT in the instruction and data cache
        using WT = Field <_CACR, 2, 1>; // Bits [2]
        // Enable TCM. Enables ECC in the instruction and data cache
        using ECC = Field <_CACR, 1, 1>; // Bits [1]
        // Enables cache coherency usage, globally visible writes
        using SIWT = Field <_CACR, 0, 1>; // Bits [0]
    };

    // AHBPCR: Controls accesses to AHBP or AXIM
    struct _AHBPCR : Register <0xE000EF98, ReadWrite, _AHBPCR> {
        // Size of AHBP
        using SZ = Field <_AHBPCR, 1, 3>; // Bits [3:1]
        // Enables AHBP. When disabled all accesses are made to the AXIM
        using EN = Field <_AHBPCR, 0, 1>; // Bits [0]
    };

    // ABFSR: Specifies the source of the asynchronous bus faults
    struct _ABFSR : Register <0xE000EFA8, ReadWrite, _ABFSR> {
        // Asynchronous fault on EPPB interface
        using EPPB = Field <_ABFSR, 4, 1>; // Bits [4]
        // Asynchronous fault on AXIM interface
        using AXIM = Field <_ABFSR, 3, 1>; // Bits [3]
        // Asynchronous fault on AHBP interface
        using AHBP = Field <_ABFSR, 2, 1>; // Bits [2]
        // Asynchronous fault on DTCM interface
        using DTCM = Field <_ABFSR, 1, 1>; // Bits [1]
        // Asynchronous fault on ITCM interface
        using ITCM = Field <_ABFSR, 0, 1>; // Bits [0]
    };

} // namespace ImpDef

// Generated from xml file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/nvic_registers_v7m.xml
// --------------------------------------------
//NVIC: Nested Vectored Interrupt Controller registers
// --------------------------------------------
namespace NVIC {

    // ICTR: Provides information about the interrupt controller
    struct _ICTR : Register <0xE000E004, ReadOnly, _ICTR> {
        // The total number of interrupt lines supported n, defined in groups of 32. That is, the total number of interrupt lines is up to (32*(INTLINESNUM+1)). However, the absolute maximum number of interrupts is 496, corresponding to the INTLINESNUM value 0b1111.
        using INTLINESNUM = Field <_ICTR, 0, 4>; // Bits [3:0]
    };

    // NVIC_ISER0: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER0 : Register <0xE000E100, ReadWrite, _NVIC_ISER0> {
    };

    // NVIC_ISER1: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER1 : Register <0xE000E104, ReadWrite, _NVIC_ISER1> {
    };

    // NVIC_ISER2: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER2 : Register <0xE000E108, ReadWrite, _NVIC_ISER2> {
    };

    // NVIC_ISER3: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER3 : Register <0xE000E10C, ReadWrite, _NVIC_ISER3> {
    };

    // NVIC_ISER4: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER4 : Register <0xE000E110, ReadWrite, _NVIC_ISER4> {
    };

    // NVIC_ISER5: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER5 : Register <0xE000E114, ReadWrite, _NVIC_ISER5> {
    };

    // NVIC_ISER6: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER6 : Register <0xE000E118, ReadWrite, _NVIC_ISER6> {
    };

    // NVIC_ISER7: Enables, or reads the enable state of a group of interrupts
    struct _NVIC_ISER7 : Register <0xE000E11C, ReadWrite, _NVIC_ISER7> {
    };

    // NVIC_ICER0: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER0 : Register <0xE000E180, ReadWrite, _NVIC_ICER0> {
    };

    // NVIC_ICER1: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER1 : Register <0xE000E184, ReadWrite, _NVIC_ICER1> {
    };

    // NVIC_ICER2: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER2 : Register <0xE000E188, ReadWrite, _NVIC_ICER2> {
    };

    // NVIC_ICER3: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER3 : Register <0xE000E18C, ReadWrite, _NVIC_ICER3> {
    };

    // NVIC_ICER4: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER4 : Register <0xE000E190, ReadWrite, _NVIC_ICER4> {
    };

    // NVIC_ICER5: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER5 : Register <0xE000E194, ReadWrite, _NVIC_ICER5> {
    };

    // NVIC_ICER6: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER6 : Register <0xE000E198, ReadWrite, _NVIC_ICER6> {
    };

    // NVIC_ICER7: Disables, or reads the enable state of a group of interrupts
    struct _NVIC_ICER7 : Register <0xE000E19C, ReadWrite, _NVIC_ICER7> {
    };

    // NVIC_ISPR0: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR0 : Register <0xE000E200, ReadWrite, _NVIC_ISPR0> {
    };

    // NVIC_ISPR1: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR1 : Register <0xE000E204, ReadWrite, _NVIC_ISPR1> {
    };

    // NVIC_ISPR2: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR2 : Register <0xE000E208, ReadWrite, _NVIC_ISPR2> {
    };

    // NVIC_ISPR3: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR3 : Register <0xE000E20C, ReadWrite, _NVIC_ISPR3> {
    };

    // NVIC_ISPR4: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR4 : Register <0xE000E210, ReadWrite, _NVIC_ISPR4> {
    };

    // NVIC_ISPR5: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR5 : Register <0xE000E214, ReadWrite, _NVIC_ISPR5> {
    };

    // NVIC_ISPR6: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR6 : Register <0xE000E218, ReadWrite, _NVIC_ISPR6> {
    };

    // NVIC_ISPR7: For a group of interrupts, changes interrupt status to pending, or shows the current pending status
    struct _NVIC_ISPR7 : Register <0xE000E21C, ReadWrite, _NVIC_ISPR7> {
    };

    // NVIC_ICPR0: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR0 : Register <0xE000E280, ReadWrite, _NVIC_ICPR0> {
    };

    // NVIC_ICPR1: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR1 : Register <0xE000E284, ReadWrite, _NVIC_ICPR1> {
    };

    // NVIC_ICPR2: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR2 : Register <0xE000E288, ReadWrite, _NVIC_ICPR2> {
    };

    // NVIC_ICPR3: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR3 : Register <0xE000E28C, ReadWrite, _NVIC_ICPR3> {
    };

    // NVIC_ICPR4: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR4 : Register <0xE000E290, ReadWrite, _NVIC_ICPR4> {
    };

    // NVIC_ICPR5: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR5 : Register <0xE000E294, ReadWrite, _NVIC_ICPR5> {
    };

    // NVIC_ICPR6: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR6 : Register <0xE000E298, ReadWrite, _NVIC_ICPR6> {
    };

    // NVIC_ICPR7: For a group of interrupts, clears the interrupt pending status, or shows the current pending status
    struct _NVIC_ICPR7 : Register <0xE000E29C, ReadWrite, _NVIC_ICPR7> {
    };

    // NVIC_IABR0: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR0 : Register <0xE000E300, ReadWrite, _NVIC_IABR0> {
    };

    // NVIC_IABR1: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR1 : Register <0xE000E304, ReadWrite, _NVIC_IABR1> {
    };

    // NVIC_IABR2: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR2 : Register <0xE000E308, ReadWrite, _NVIC_IABR2> {
    };

    // NVIC_IABR3: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR3 : Register <0xE000E30C, ReadWrite, _NVIC_IABR3> {
    };

    // NVIC_IABR4: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR4 : Register <0xE000E310, ReadWrite, _NVIC_IABR4> {
    };

    // NVIC_IABR5: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR5 : Register <0xE000E314, ReadWrite, _NVIC_IABR5> {
    };

    // NVIC_IABR6: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR6 : Register <0xE000E318, ReadWrite, _NVIC_IABR6> {
    };

    // NVIC_IABR7: For a group of 32 interrupts, shows whether each interrupt is active
    struct _NVIC_IABR7 : Register <0xE000E31C, ReadWrite, _NVIC_IABR7> {
    };

    // NVIC_IPR0: Sets or reads interrupt priorities
    struct _NVIC_IPR0 : Register <0xE000E400, ReadWrite, _NVIC_IPR0> {
        // Priority of interrupt 3
        using PRI_N3 = Field <_NVIC_IPR0, 24, 8>; // Bits [31:24]
        // Priority of interrupt 2
        using PRI_N2 = Field <_NVIC_IPR0, 16, 8>; // Bits [23:16]
        // Priority of interrupt 1
        using PRI_N1 = Field <_NVIC_IPR0, 8, 8>; // Bits [15:8]
        // Priority of interrupt 0
        using PRI_N0 = Field <_NVIC_IPR0, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR1: Sets or reads interrupt priorities
    struct _NVIC_IPR1 : Register <0xE000E404, ReadWrite, _NVIC_IPR1> {
        // Priority of interrupt 7
        using PRI_N3 = Field <_NVIC_IPR1, 24, 8>; // Bits [31:24]
        // Priority of interrupt 6
        using PRI_N2 = Field <_NVIC_IPR1, 16, 8>; // Bits [23:16]
        // Priority of interrupt 5
        using PRI_N1 = Field <_NVIC_IPR1, 8, 8>; // Bits [15:8]
        // Priority of interrupt 4
        using PRI_N0 = Field <_NVIC_IPR1, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR2: Sets or reads interrupt priorities
    struct _NVIC_IPR2 : Register <0xE000E408, ReadWrite, _NVIC_IPR2> {
        // Priority of interrupt 11
        using PRI_N3 = Field <_NVIC_IPR2, 24, 8>; // Bits [31:24]
        // Priority of interrupt 10
        using PRI_N2 = Field <_NVIC_IPR2, 16, 8>; // Bits [23:16]
        // Priority of interrupt 9
        using PRI_N1 = Field <_NVIC_IPR2, 8, 8>; // Bits [15:8]
        // Priority of interrupt 8
        using PRI_N0 = Field <_NVIC_IPR2, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR3: Sets or reads interrupt priorities
    struct _NVIC_IPR3 : Register <0xE000E40C, ReadWrite, _NVIC_IPR3> {
        // Priority of interrupt 15
        using PRI_N3 = Field <_NVIC_IPR3, 24, 8>; // Bits [31:24]
        // Priority of interrupt 14
        using PRI_N2 = Field <_NVIC_IPR3, 16, 8>; // Bits [23:16]
        // Priority of interrupt 13
        using PRI_N1 = Field <_NVIC_IPR3, 8, 8>; // Bits [15:8]
        // Priority of interrupt 12
        using PRI_N0 = Field <_NVIC_IPR3, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR4: Sets or reads interrupt priorities
    struct _NVIC_IPR4 : Register <0xE000E410, ReadWrite, _NVIC_IPR4> {
        // Priority of interrupt 19
        using PRI_N3 = Field <_NVIC_IPR4, 24, 8>; // Bits [31:24]
        // Priority of interrupt 18
        using PRI_N2 = Field <_NVIC_IPR4, 16, 8>; // Bits [23:16]
        // Priority of interrupt 17
        using PRI_N1 = Field <_NVIC_IPR4, 8, 8>; // Bits [15:8]
        // Priority of interrupt 16
        using PRI_N0 = Field <_NVIC_IPR4, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR5: Sets or reads interrupt priorities
    struct _NVIC_IPR5 : Register <0xE000E414, ReadWrite, _NVIC_IPR5> {
        // Priority of interrupt 23
        using PRI_N3 = Field <_NVIC_IPR5, 24, 8>; // Bits [31:24]
        // Priority of interrupt 22
        using PRI_N2 = Field <_NVIC_IPR5, 16, 8>; // Bits [23:16]
        // Priority of interrupt 21
        using PRI_N1 = Field <_NVIC_IPR5, 8, 8>; // Bits [15:8]
        // Priority of interrupt 20
        using PRI_N0 = Field <_NVIC_IPR5, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR6: Sets or reads interrupt priorities
    struct _NVIC_IPR6 : Register <0xE000E418, ReadWrite, _NVIC_IPR6> {
        // Priority of interrupt 27
        using PRI_N3 = Field <_NVIC_IPR6, 24, 8>; // Bits [31:24]
        // Priority of interrupt 26
        using PRI_N2 = Field <_NVIC_IPR6, 16, 8>; // Bits [23:16]
        // Priority of interrupt 25
        using PRI_N1 = Field <_NVIC_IPR6, 8, 8>; // Bits [15:8]
        // Priority of interrupt 24
        using PRI_N0 = Field <_NVIC_IPR6, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR7: Sets or reads interrupt priorities
    struct _NVIC_IPR7 : Register <0xE000E41C, ReadWrite, _NVIC_IPR7> {
        // Priority of interrupt 31
        using PRI_N3 = Field <_NVIC_IPR7, 24, 8>; // Bits [31:24]
        // Priority of interrupt 30
        using PRI_N2 = Field <_NVIC_IPR7, 16, 8>; // Bits [23:16]
        // Priority of interrupt 29
        using PRI_N1 = Field <_NVIC_IPR7, 8, 8>; // Bits [15:8]
        // Priority of interrupt 28
        using PRI_N0 = Field <_NVIC_IPR7, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR8: Sets or reads interrupt priorities
    struct _NVIC_IPR8 : Register <0xE000E420, ReadWrite, _NVIC_IPR8> {
        // Priority of interrupt 35
        using PRI_N3 = Field <_NVIC_IPR8, 24, 8>; // Bits [31:24]
        // Priority of interrupt 34
        using PRI_N2 = Field <_NVIC_IPR8, 16, 8>; // Bits [23:16]
        // Priority of interrupt 33
        using PRI_N1 = Field <_NVIC_IPR8, 8, 8>; // Bits [15:8]
        // Priority of interrupt 32
        using PRI_N0 = Field <_NVIC_IPR8, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR9: Sets or reads interrupt priorities
    struct _NVIC_IPR9 : Register <0xE000E424, ReadWrite, _NVIC_IPR9> {
        // Priority of interrupt 39
        using PRI_N3 = Field <_NVIC_IPR9, 24, 8>; // Bits [31:24]
        // Priority of interrupt 38
        using PRI_N2 = Field <_NVIC_IPR9, 16, 8>; // Bits [23:16]
        // Priority of interrupt 37
        using PRI_N1 = Field <_NVIC_IPR9, 8, 8>; // Bits [15:8]
        // Priority of interrupt 36
        using PRI_N0 = Field <_NVIC_IPR9, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR10: Sets or reads interrupt priorities
    struct _NVIC_IPR10 : Register <0xE000E428, ReadWrite, _NVIC_IPR10> {
        // Priority of interrupt 43
        using PRI_N3 = Field <_NVIC_IPR10, 24, 8>; // Bits [31:24]
        // Priority of interrupt 42
        using PRI_N2 = Field <_NVIC_IPR10, 16, 8>; // Bits [23:16]
        // Priority of interrupt 41
        using PRI_N1 = Field <_NVIC_IPR10, 8, 8>; // Bits [15:8]
        // Priority of interrupt 40
        using PRI_N0 = Field <_NVIC_IPR10, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR11: Sets or reads interrupt priorities
    struct _NVIC_IPR11 : Register <0xE000E42C, ReadWrite, _NVIC_IPR11> {
        // Priority of interrupt 47
        using PRI_N3 = Field <_NVIC_IPR11, 24, 8>; // Bits [31:24]
        // Priority of interrupt 46
        using PRI_N2 = Field <_NVIC_IPR11, 16, 8>; // Bits [23:16]
        // Priority of interrupt 45
        using PRI_N1 = Field <_NVIC_IPR11, 8, 8>; // Bits [15:8]
        // Priority of interrupt 44
        using PRI_N0 = Field <_NVIC_IPR11, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR12: Sets or reads interrupt priorities
    struct _NVIC_IPR12 : Register <0xE000E430, ReadWrite, _NVIC_IPR12> {
        // Priority of interrupt 51
        using PRI_N3 = Field <_NVIC_IPR12, 24, 8>; // Bits [31:24]
        // Priority of interrupt 50
        using PRI_N2 = Field <_NVIC_IPR12, 16, 8>; // Bits [23:16]
        // Priority of interrupt 49
        using PRI_N1 = Field <_NVIC_IPR12, 8, 8>; // Bits [15:8]
        // Priority of interrupt 48
        using PRI_N0 = Field <_NVIC_IPR12, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR13: Sets or reads interrupt priorities
    struct _NVIC_IPR13 : Register <0xE000E434, ReadWrite, _NVIC_IPR13> {
        // Priority of interrupt 55
        using PRI_N3 = Field <_NVIC_IPR13, 24, 8>; // Bits [31:24]
        // Priority of interrupt 54
        using PRI_N2 = Field <_NVIC_IPR13, 16, 8>; // Bits [23:16]
        // Priority of interrupt 53
        using PRI_N1 = Field <_NVIC_IPR13, 8, 8>; // Bits [15:8]
        // Priority of interrupt 52
        using PRI_N0 = Field <_NVIC_IPR13, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR14: Sets or reads interrupt priorities
    struct _NVIC_IPR14 : Register <0xE000E438, ReadWrite, _NVIC_IPR14> {
        // Priority of interrupt 59
        using PRI_N3 = Field <_NVIC_IPR14, 24, 8>; // Bits [31:24]
        // Priority of interrupt 58
        using PRI_N2 = Field <_NVIC_IPR14, 16, 8>; // Bits [23:16]
        // Priority of interrupt 57
        using PRI_N1 = Field <_NVIC_IPR14, 8, 8>; // Bits [15:8]
        // Priority of interrupt 56
        using PRI_N0 = Field <_NVIC_IPR14, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR15: Sets or reads interrupt priorities
    struct _NVIC_IPR15 : Register <0xE000E43C, ReadWrite, _NVIC_IPR15> {
        // Priority of interrupt 63
        using PRI_N3 = Field <_NVIC_IPR15, 24, 8>; // Bits [31:24]
        // Priority of interrupt 62
        using PRI_N2 = Field <_NVIC_IPR15, 16, 8>; // Bits [23:16]
        // Priority of interrupt 61
        using PRI_N1 = Field <_NVIC_IPR15, 8, 8>; // Bits [15:8]
        // Priority of interrupt 60
        using PRI_N0 = Field <_NVIC_IPR15, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR16: Sets or reads interrupt priorities
    struct _NVIC_IPR16 : Register <0xE000E440, ReadWrite, _NVIC_IPR16> {
        // Priority of interrupt 67
        using PRI_N3 = Field <_NVIC_IPR16, 24, 8>; // Bits [31:24]
        // Priority of interrupt 66
        using PRI_N2 = Field <_NVIC_IPR16, 16, 8>; // Bits [23:16]
        // Priority of interrupt 65
        using PRI_N1 = Field <_NVIC_IPR16, 8, 8>; // Bits [15:8]
        // Priority of interrupt 64
        using PRI_N0 = Field <_NVIC_IPR16, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR17: Sets or reads interrupt priorities
    struct _NVIC_IPR17 : Register <0xE000E444, ReadWrite, _NVIC_IPR17> {
        // Priority of interrupt 71
        using PRI_N3 = Field <_NVIC_IPR17, 24, 8>; // Bits [31:24]
        // Priority of interrupt 70
        using PRI_N2 = Field <_NVIC_IPR17, 16, 8>; // Bits [23:16]
        // Priority of interrupt 69
        using PRI_N1 = Field <_NVIC_IPR17, 8, 8>; // Bits [15:8]
        // Priority of interrupt 68
        using PRI_N0 = Field <_NVIC_IPR17, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR18: Sets or reads interrupt priorities
    struct _NVIC_IPR18 : Register <0xE000E448, ReadWrite, _NVIC_IPR18> {
        // Priority of interrupt 75
        using PRI_N3 = Field <_NVIC_IPR18, 24, 8>; // Bits [31:24]
        // Priority of interrupt 74
        using PRI_N2 = Field <_NVIC_IPR18, 16, 8>; // Bits [23:16]
        // Priority of interrupt 73
        using PRI_N1 = Field <_NVIC_IPR18, 8, 8>; // Bits [15:8]
        // Priority of interrupt 72
        using PRI_N0 = Field <_NVIC_IPR18, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR19: Sets or reads interrupt priorities
    struct _NVIC_IPR19 : Register <0xE000E44C, ReadWrite, _NVIC_IPR19> {
        // Priority of interrupt 79
        using PRI_N3 = Field <_NVIC_IPR19, 24, 8>; // Bits [31:24]
        // Priority of interrupt 78
        using PRI_N2 = Field <_NVIC_IPR19, 16, 8>; // Bits [23:16]
        // Priority of interrupt 77
        using PRI_N1 = Field <_NVIC_IPR19, 8, 8>; // Bits [15:8]
        // Priority of interrupt 76
        using PRI_N0 = Field <_NVIC_IPR19, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR20: Sets or reads interrupt priorities
    struct _NVIC_IPR20 : Register <0xE000E450, ReadWrite, _NVIC_IPR20> {
        // Priority of interrupt 83
        using PRI_N3 = Field <_NVIC_IPR20, 24, 8>; // Bits [31:24]
        // Priority of interrupt 82
        using PRI_N2 = Field <_NVIC_IPR20, 16, 8>; // Bits [23:16]
        // Priority of interrupt 81
        using PRI_N1 = Field <_NVIC_IPR20, 8, 8>; // Bits [15:8]
        // Priority of interrupt 80
        using PRI_N0 = Field <_NVIC_IPR20, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR21: Sets or reads interrupt priorities
    struct _NVIC_IPR21 : Register <0xE000E454, ReadWrite, _NVIC_IPR21> {
        // Priority of interrupt 87
        using PRI_N3 = Field <_NVIC_IPR21, 24, 8>; // Bits [31:24]
        // Priority of interrupt 86
        using PRI_N2 = Field <_NVIC_IPR21, 16, 8>; // Bits [23:16]
        // Priority of interrupt 85
        using PRI_N1 = Field <_NVIC_IPR21, 8, 8>; // Bits [15:8]
        // Priority of interrupt 84
        using PRI_N0 = Field <_NVIC_IPR21, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR22: Sets or reads interrupt priorities
    struct _NVIC_IPR22 : Register <0xE000E458, ReadWrite, _NVIC_IPR22> {
        // Priority of interrupt 91
        using PRI_N3 = Field <_NVIC_IPR22, 24, 8>; // Bits [31:24]
        // Priority of interrupt 90
        using PRI_N2 = Field <_NVIC_IPR22, 16, 8>; // Bits [23:16]
        // Priority of interrupt 89
        using PRI_N1 = Field <_NVIC_IPR22, 8, 8>; // Bits [15:8]
        // Priority of interrupt 88
        using PRI_N0 = Field <_NVIC_IPR22, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR23: Sets or reads interrupt priorities
    struct _NVIC_IPR23 : Register <0xE000E45C, ReadWrite, _NVIC_IPR23> {
        // Priority of interrupt 95
        using PRI_N3 = Field <_NVIC_IPR23, 24, 8>; // Bits [31:24]
        // Priority of interrupt 94
        using PRI_N2 = Field <_NVIC_IPR23, 16, 8>; // Bits [23:16]
        // Priority of interrupt 93
        using PRI_N1 = Field <_NVIC_IPR23, 8, 8>; // Bits [15:8]
        // Priority of interrupt 92
        using PRI_N0 = Field <_NVIC_IPR23, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR24: Sets or reads interrupt priorities
    struct _NVIC_IPR24 : Register <0xE000E460, ReadWrite, _NVIC_IPR24> {
        // Priority of interrupt 99
        using PRI_N3 = Field <_NVIC_IPR24, 24, 8>; // Bits [31:24]
        // Priority of interrupt 98
        using PRI_N2 = Field <_NVIC_IPR24, 16, 8>; // Bits [23:16]
        // Priority of interrupt 97
        using PRI_N1 = Field <_NVIC_IPR24, 8, 8>; // Bits [15:8]
        // Priority of interrupt 96
        using PRI_N0 = Field <_NVIC_IPR24, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR25: Sets or reads interrupt priorities
    struct _NVIC_IPR25 : Register <0xE000E464, ReadWrite, _NVIC_IPR25> {
        // Priority of interrupt 103
        using PRI_N3 = Field <_NVIC_IPR25, 24, 8>; // Bits [31:24]
        // Priority of interrupt 102
        using PRI_N2 = Field <_NVIC_IPR25, 16, 8>; // Bits [23:16]
        // Priority of interrupt 101
        using PRI_N1 = Field <_NVIC_IPR25, 8, 8>; // Bits [15:8]
        // Priority of interrupt 100
        using PRI_N0 = Field <_NVIC_IPR25, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR26: Sets or reads interrupt priorities
    struct _NVIC_IPR26 : Register <0xE000E468, ReadWrite, _NVIC_IPR26> {
        // Priority of interrupt 107
        using PRI_N3 = Field <_NVIC_IPR26, 24, 8>; // Bits [31:24]
        // Priority of interrupt 106
        using PRI_N2 = Field <_NVIC_IPR26, 16, 8>; // Bits [23:16]
        // Priority of interrupt 105
        using PRI_N1 = Field <_NVIC_IPR26, 8, 8>; // Bits [15:8]
        // Priority of interrupt 104
        using PRI_N0 = Field <_NVIC_IPR26, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR27: Sets or reads interrupt priorities
    struct _NVIC_IPR27 : Register <0xE000E46C, ReadWrite, _NVIC_IPR27> {
        // Priority of interrupt 111
        using PRI_N3 = Field <_NVIC_IPR27, 24, 8>; // Bits [31:24]
        // Priority of interrupt 110
        using PRI_N2 = Field <_NVIC_IPR27, 16, 8>; // Bits [23:16]
        // Priority of interrupt 109
        using PRI_N1 = Field <_NVIC_IPR27, 8, 8>; // Bits [15:8]
        // Priority of interrupt 108
        using PRI_N0 = Field <_NVIC_IPR27, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR28: Sets or reads interrupt priorities
    struct _NVIC_IPR28 : Register <0xE000E470, ReadWrite, _NVIC_IPR28> {
        // Priority of interrupt 115
        using PRI_N3 = Field <_NVIC_IPR28, 24, 8>; // Bits [31:24]
        // Priority of interrupt 114
        using PRI_N2 = Field <_NVIC_IPR28, 16, 8>; // Bits [23:16]
        // Priority of interrupt 113
        using PRI_N1 = Field <_NVIC_IPR28, 8, 8>; // Bits [15:8]
        // Priority of interrupt 112
        using PRI_N0 = Field <_NVIC_IPR28, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR29: Sets or reads interrupt priorities
    struct _NVIC_IPR29 : Register <0xE000E474, ReadWrite, _NVIC_IPR29> {
        // Priority of interrupt 119
        using PRI_N3 = Field <_NVIC_IPR29, 24, 8>; // Bits [31:24]
        // Priority of interrupt 118
        using PRI_N2 = Field <_NVIC_IPR29, 16, 8>; // Bits [23:16]
        // Priority of interrupt 117
        using PRI_N1 = Field <_NVIC_IPR29, 8, 8>; // Bits [15:8]
        // Priority of interrupt 116
        using PRI_N0 = Field <_NVIC_IPR29, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR30: Sets or reads interrupt priorities
    struct _NVIC_IPR30 : Register <0xE000E478, ReadWrite, _NVIC_IPR30> {
        // Priority of interrupt 123
        using PRI_N3 = Field <_NVIC_IPR30, 24, 8>; // Bits [31:24]
        // Priority of interrupt 122
        using PRI_N2 = Field <_NVIC_IPR30, 16, 8>; // Bits [23:16]
        // Priority of interrupt 121
        using PRI_N1 = Field <_NVIC_IPR30, 8, 8>; // Bits [15:8]
        // Priority of interrupt 120
        using PRI_N0 = Field <_NVIC_IPR30, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR31: Sets or reads interrupt priorities
    struct _NVIC_IPR31 : Register <0xE000E47C, ReadWrite, _NVIC_IPR31> {
        // Priority of interrupt 127
        using PRI_N3 = Field <_NVIC_IPR31, 24, 8>; // Bits [31:24]
        // Priority of interrupt 126
        using PRI_N2 = Field <_NVIC_IPR31, 16, 8>; // Bits [23:16]
        // Priority of interrupt 125
        using PRI_N1 = Field <_NVIC_IPR31, 8, 8>; // Bits [15:8]
        // Priority of interrupt 124
        using PRI_N0 = Field <_NVIC_IPR31, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR32: Sets or reads interrupt priorities
    struct _NVIC_IPR32 : Register <0xE000E480, ReadWrite, _NVIC_IPR32> {
        // Priority of interrupt 131
        using PRI_N3 = Field <_NVIC_IPR32, 24, 8>; // Bits [31:24]
        // Priority of interrupt 130
        using PRI_N2 = Field <_NVIC_IPR32, 16, 8>; // Bits [23:16]
        // Priority of interrupt 129
        using PRI_N1 = Field <_NVIC_IPR32, 8, 8>; // Bits [15:8]
        // Priority of interrupt 128
        using PRI_N0 = Field <_NVIC_IPR32, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR33: Sets or reads interrupt priorities
    struct _NVIC_IPR33 : Register <0xE000E484, ReadWrite, _NVIC_IPR33> {
        // Priority of interrupt 135
        using PRI_N3 = Field <_NVIC_IPR33, 24, 8>; // Bits [31:24]
        // Priority of interrupt 134
        using PRI_N2 = Field <_NVIC_IPR33, 16, 8>; // Bits [23:16]
        // Priority of interrupt 133
        using PRI_N1 = Field <_NVIC_IPR33, 8, 8>; // Bits [15:8]
        // Priority of interrupt 132
        using PRI_N0 = Field <_NVIC_IPR33, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR34: Sets or reads interrupt priorities
    struct _NVIC_IPR34 : Register <0xE000E488, ReadWrite, _NVIC_IPR34> {
        // Priority of interrupt 139
        using PRI_N3 = Field <_NVIC_IPR34, 24, 8>; // Bits [31:24]
        // Priority of interrupt 138
        using PRI_N2 = Field <_NVIC_IPR34, 16, 8>; // Bits [23:16]
        // Priority of interrupt 137
        using PRI_N1 = Field <_NVIC_IPR34, 8, 8>; // Bits [15:8]
        // Priority of interrupt 136
        using PRI_N0 = Field <_NVIC_IPR34, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR35: Sets or reads interrupt priorities
    struct _NVIC_IPR35 : Register <0xE000E48C, ReadWrite, _NVIC_IPR35> {
        // Priority of interrupt 143
        using PRI_N3 = Field <_NVIC_IPR35, 24, 8>; // Bits [31:24]
        // Priority of interrupt 142
        using PRI_N2 = Field <_NVIC_IPR35, 16, 8>; // Bits [23:16]
        // Priority of interrupt 141
        using PRI_N1 = Field <_NVIC_IPR35, 8, 8>; // Bits [15:8]
        // Priority of interrupt 140
        using PRI_N0 = Field <_NVIC_IPR35, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR36: Sets or reads interrupt priorities
    struct _NVIC_IPR36 : Register <0xE000E490, ReadWrite, _NVIC_IPR36> {
        // Priority of interrupt 147
        using PRI_N3 = Field <_NVIC_IPR36, 24, 8>; // Bits [31:24]
        // Priority of interrupt 146
        using PRI_N2 = Field <_NVIC_IPR36, 16, 8>; // Bits [23:16]
        // Priority of interrupt 145
        using PRI_N1 = Field <_NVIC_IPR36, 8, 8>; // Bits [15:8]
        // Priority of interrupt 144
        using PRI_N0 = Field <_NVIC_IPR36, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR37: Sets or reads interrupt priorities
    struct _NVIC_IPR37 : Register <0xE000E494, ReadWrite, _NVIC_IPR37> {
        // Priority of interrupt 151
        using PRI_N3 = Field <_NVIC_IPR37, 24, 8>; // Bits [31:24]
        // Priority of interrupt 150
        using PRI_N2 = Field <_NVIC_IPR37, 16, 8>; // Bits [23:16]
        // Priority of interrupt 149
        using PRI_N1 = Field <_NVIC_IPR37, 8, 8>; // Bits [15:8]
        // Priority of interrupt 148
        using PRI_N0 = Field <_NVIC_IPR37, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR38: Sets or reads interrupt priorities
    struct _NVIC_IPR38 : Register <0xE000E498, ReadWrite, _NVIC_IPR38> {
        // Priority of interrupt 155
        using PRI_N3 = Field <_NVIC_IPR38, 24, 8>; // Bits [31:24]
        // Priority of interrupt 154
        using PRI_N2 = Field <_NVIC_IPR38, 16, 8>; // Bits [23:16]
        // Priority of interrupt 153
        using PRI_N1 = Field <_NVIC_IPR38, 8, 8>; // Bits [15:8]
        // Priority of interrupt 152
        using PRI_N0 = Field <_NVIC_IPR38, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR39: Sets or reads interrupt priorities
    struct _NVIC_IPR39 : Register <0xE000E49C, ReadWrite, _NVIC_IPR39> {
        // Priority of interrupt 159
        using PRI_N3 = Field <_NVIC_IPR39, 24, 8>; // Bits [31:24]
        // Priority of interrupt 158
        using PRI_N2 = Field <_NVIC_IPR39, 16, 8>; // Bits [23:16]
        // Priority of interrupt 157
        using PRI_N1 = Field <_NVIC_IPR39, 8, 8>; // Bits [15:8]
        // Priority of interrupt 156
        using PRI_N0 = Field <_NVIC_IPR39, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR40: Sets or reads interrupt priorities
    struct _NVIC_IPR40 : Register <0xE000E4A0, ReadWrite, _NVIC_IPR40> {
        // Priority of interrupt 163
        using PRI_N3 = Field <_NVIC_IPR40, 24, 8>; // Bits [31:24]
        // Priority of interrupt 162
        using PRI_N2 = Field <_NVIC_IPR40, 16, 8>; // Bits [23:16]
        // Priority of interrupt 161
        using PRI_N1 = Field <_NVIC_IPR40, 8, 8>; // Bits [15:8]
        // Priority of interrupt 160
        using PRI_N0 = Field <_NVIC_IPR40, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR41: Sets or reads interrupt priorities
    struct _NVIC_IPR41 : Register <0xE000E4A4, ReadWrite, _NVIC_IPR41> {
        // Priority of interrupt 167
        using PRI_N3 = Field <_NVIC_IPR41, 24, 8>; // Bits [31:24]
        // Priority of interrupt 166
        using PRI_N2 = Field <_NVIC_IPR41, 16, 8>; // Bits [23:16]
        // Priority of interrupt 165
        using PRI_N1 = Field <_NVIC_IPR41, 8, 8>; // Bits [15:8]
        // Priority of interrupt 164
        using PRI_N0 = Field <_NVIC_IPR41, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR42: Sets or reads interrupt priorities
    struct _NVIC_IPR42 : Register <0xE000E4A8, ReadWrite, _NVIC_IPR42> {
        // Priority of interrupt 171
        using PRI_N3 = Field <_NVIC_IPR42, 24, 8>; // Bits [31:24]
        // Priority of interrupt 170
        using PRI_N2 = Field <_NVIC_IPR42, 16, 8>; // Bits [23:16]
        // Priority of interrupt 169
        using PRI_N1 = Field <_NVIC_IPR42, 8, 8>; // Bits [15:8]
        // Priority of interrupt 168
        using PRI_N0 = Field <_NVIC_IPR42, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR43: Sets or reads interrupt priorities
    struct _NVIC_IPR43 : Register <0xE000E4AC, ReadWrite, _NVIC_IPR43> {
        // Priority of interrupt 175
        using PRI_N3 = Field <_NVIC_IPR43, 24, 8>; // Bits [31:24]
        // Priority of interrupt 174
        using PRI_N2 = Field <_NVIC_IPR43, 16, 8>; // Bits [23:16]
        // Priority of interrupt 173
        using PRI_N1 = Field <_NVIC_IPR43, 8, 8>; // Bits [15:8]
        // Priority of interrupt 172
        using PRI_N0 = Field <_NVIC_IPR43, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR44: Sets or reads interrupt priorities
    struct _NVIC_IPR44 : Register <0xE000E4B0, ReadWrite, _NVIC_IPR44> {
        // Priority of interrupt 179
        using PRI_N3 = Field <_NVIC_IPR44, 24, 8>; // Bits [31:24]
        // Priority of interrupt 178
        using PRI_N2 = Field <_NVIC_IPR44, 16, 8>; // Bits [23:16]
        // Priority of interrupt 177
        using PRI_N1 = Field <_NVIC_IPR44, 8, 8>; // Bits [15:8]
        // Priority of interrupt 176
        using PRI_N0 = Field <_NVIC_IPR44, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR45: Sets or reads interrupt priorities
    struct _NVIC_IPR45 : Register <0xE000E4B4, ReadWrite, _NVIC_IPR45> {
        // Priority of interrupt 183
        using PRI_N3 = Field <_NVIC_IPR45, 24, 8>; // Bits [31:24]
        // Priority of interrupt 182
        using PRI_N2 = Field <_NVIC_IPR45, 16, 8>; // Bits [23:16]
        // Priority of interrupt 181
        using PRI_N1 = Field <_NVIC_IPR45, 8, 8>; // Bits [15:8]
        // Priority of interrupt 180
        using PRI_N0 = Field <_NVIC_IPR45, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR46: Sets or reads interrupt priorities
    struct _NVIC_IPR46 : Register <0xE000E4B8, ReadWrite, _NVIC_IPR46> {
        // Priority of interrupt 187
        using PRI_N3 = Field <_NVIC_IPR46, 24, 8>; // Bits [31:24]
        // Priority of interrupt 186
        using PRI_N2 = Field <_NVIC_IPR46, 16, 8>; // Bits [23:16]
        // Priority of interrupt 185
        using PRI_N1 = Field <_NVIC_IPR46, 8, 8>; // Bits [15:8]
        // Priority of interrupt 184
        using PRI_N0 = Field <_NVIC_IPR46, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR47: Sets or reads interrupt priorities
    struct _NVIC_IPR47 : Register <0xE000E4BC, ReadWrite, _NVIC_IPR47> {
        // Priority of interrupt 191
        using PRI_N3 = Field <_NVIC_IPR47, 24, 8>; // Bits [31:24]
        // Priority of interrupt 190
        using PRI_N2 = Field <_NVIC_IPR47, 16, 8>; // Bits [23:16]
        // Priority of interrupt 189
        using PRI_N1 = Field <_NVIC_IPR47, 8, 8>; // Bits [15:8]
        // Priority of interrupt 188
        using PRI_N0 = Field <_NVIC_IPR47, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR48: Sets or reads interrupt priorities
    struct _NVIC_IPR48 : Register <0xE000E4C0, ReadWrite, _NVIC_IPR48> {
        // Priority of interrupt 195
        using PRI_N3 = Field <_NVIC_IPR48, 24, 8>; // Bits [31:24]
        // Priority of interrupt 194
        using PRI_N2 = Field <_NVIC_IPR48, 16, 8>; // Bits [23:16]
        // Priority of interrupt 193
        using PRI_N1 = Field <_NVIC_IPR48, 8, 8>; // Bits [15:8]
        // Priority of interrupt 192
        using PRI_N0 = Field <_NVIC_IPR48, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR49: Sets or reads interrupt priorities
    struct _NVIC_IPR49 : Register <0xE000E4C4, ReadWrite, _NVIC_IPR49> {
        // Priority of interrupt 199
        using PRI_N3 = Field <_NVIC_IPR49, 24, 8>; // Bits [31:24]
        // Priority of interrupt 198
        using PRI_N2 = Field <_NVIC_IPR49, 16, 8>; // Bits [23:16]
        // Priority of interrupt 197
        using PRI_N1 = Field <_NVIC_IPR49, 8, 8>; // Bits [15:8]
        // Priority of interrupt 196
        using PRI_N0 = Field <_NVIC_IPR49, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR50: Sets or reads interrupt priorities
    struct _NVIC_IPR50 : Register <0xE000E4C8, ReadWrite, _NVIC_IPR50> {
        // Priority of interrupt 203
        using PRI_N3 = Field <_NVIC_IPR50, 24, 8>; // Bits [31:24]
        // Priority of interrupt 202
        using PRI_N2 = Field <_NVIC_IPR50, 16, 8>; // Bits [23:16]
        // Priority of interrupt 201
        using PRI_N1 = Field <_NVIC_IPR50, 8, 8>; // Bits [15:8]
        // Priority of interrupt 200
        using PRI_N0 = Field <_NVIC_IPR50, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR51: Sets or reads interrupt priorities
    struct _NVIC_IPR51 : Register <0xE000E4CC, ReadWrite, _NVIC_IPR51> {
        // Priority of interrupt 207
        using PRI_N3 = Field <_NVIC_IPR51, 24, 8>; // Bits [31:24]
        // Priority of interrupt 206
        using PRI_N2 = Field <_NVIC_IPR51, 16, 8>; // Bits [23:16]
        // Priority of interrupt 205
        using PRI_N1 = Field <_NVIC_IPR51, 8, 8>; // Bits [15:8]
        // Priority of interrupt 204
        using PRI_N0 = Field <_NVIC_IPR51, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR52: Sets or reads interrupt priorities
    struct _NVIC_IPR52 : Register <0xE000E4D0, ReadWrite, _NVIC_IPR52> {
        // Priority of interrupt 211
        using PRI_N3 = Field <_NVIC_IPR52, 24, 8>; // Bits [31:24]
        // Priority of interrupt 210
        using PRI_N2 = Field <_NVIC_IPR52, 16, 8>; // Bits [23:16]
        // Priority of interrupt 209
        using PRI_N1 = Field <_NVIC_IPR52, 8, 8>; // Bits [15:8]
        // Priority of interrupt 208
        using PRI_N0 = Field <_NVIC_IPR52, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR53: Sets or reads interrupt priorities
    struct _NVIC_IPR53 : Register <0xE000E4D4, ReadWrite, _NVIC_IPR53> {
        // Priority of interrupt 215
        using PRI_N3 = Field <_NVIC_IPR53, 24, 8>; // Bits [31:24]
        // Priority of interrupt 214
        using PRI_N2 = Field <_NVIC_IPR53, 16, 8>; // Bits [23:16]
        // Priority of interrupt 213
        using PRI_N1 = Field <_NVIC_IPR53, 8, 8>; // Bits [15:8]
        // Priority of interrupt 212
        using PRI_N0 = Field <_NVIC_IPR53, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR54: Sets or reads interrupt priorities
    struct _NVIC_IPR54 : Register <0xE000E4D8, ReadWrite, _NVIC_IPR54> {
        // Priority of interrupt 219
        using PRI_N3 = Field <_NVIC_IPR54, 24, 8>; // Bits [31:24]
        // Priority of interrupt 218
        using PRI_N2 = Field <_NVIC_IPR54, 16, 8>; // Bits [23:16]
        // Priority of interrupt 217
        using PRI_N1 = Field <_NVIC_IPR54, 8, 8>; // Bits [15:8]
        // Priority of interrupt 216
        using PRI_N0 = Field <_NVIC_IPR54, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR55: Sets or reads interrupt priorities
    struct _NVIC_IPR55 : Register <0xE000E4DC, ReadWrite, _NVIC_IPR55> {
        // Priority of interrupt 223
        using PRI_N3 = Field <_NVIC_IPR55, 24, 8>; // Bits [31:24]
        // Priority of interrupt 222
        using PRI_N2 = Field <_NVIC_IPR55, 16, 8>; // Bits [23:16]
        // Priority of interrupt 221
        using PRI_N1 = Field <_NVIC_IPR55, 8, 8>; // Bits [15:8]
        // Priority of interrupt 220
        using PRI_N0 = Field <_NVIC_IPR55, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR56: Sets or reads interrupt priorities
    struct _NVIC_IPR56 : Register <0xE000E4E0, ReadWrite, _NVIC_IPR56> {
        // Priority of interrupt 227
        using PRI_N3 = Field <_NVIC_IPR56, 24, 8>; // Bits [31:24]
        // Priority of interrupt 226
        using PRI_N2 = Field <_NVIC_IPR56, 16, 8>; // Bits [23:16]
        // Priority of interrupt 225
        using PRI_N1 = Field <_NVIC_IPR56, 8, 8>; // Bits [15:8]
        // Priority of interrupt 224
        using PRI_N0 = Field <_NVIC_IPR56, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR57: Sets or reads interrupt priorities
    struct _NVIC_IPR57 : Register <0xE000E4E4, ReadWrite, _NVIC_IPR57> {
        // Priority of interrupt 231
        using PRI_N3 = Field <_NVIC_IPR57, 24, 8>; // Bits [31:24]
        // Priority of interrupt 230
        using PRI_N2 = Field <_NVIC_IPR57, 16, 8>; // Bits [23:16]
        // Priority of interrupt 229
        using PRI_N1 = Field <_NVIC_IPR57, 8, 8>; // Bits [15:8]
        // Priority of interrupt 228
        using PRI_N0 = Field <_NVIC_IPR57, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR58: Sets or reads interrupt priorities
    struct _NVIC_IPR58 : Register <0xE000E4E8, ReadWrite, _NVIC_IPR58> {
        // Priority of interrupt 235
        using PRI_N3 = Field <_NVIC_IPR58, 24, 8>; // Bits [31:24]
        // Priority of interrupt 234
        using PRI_N2 = Field <_NVIC_IPR58, 16, 8>; // Bits [23:16]
        // Priority of interrupt 233
        using PRI_N1 = Field <_NVIC_IPR58, 8, 8>; // Bits [15:8]
        // Priority of interrupt 232
        using PRI_N0 = Field <_NVIC_IPR58, 0, 8>; // Bits [7:0]
    };

    // NVIC_IPR59: Sets or reads interrupt priorities
    struct _NVIC_IPR59 : Register <0xE000E4EC, ReadWrite, _NVIC_IPR59> {
        // Priority of interrupt 239
        using PRI_N3 = Field <_NVIC_IPR59, 24, 8>; // Bits [31:24]
        // Priority of interrupt 238
        using PRI_N2 = Field <_NVIC_IPR59, 16, 8>; // Bits [23:16]
        // Priority of interrupt 237
        using PRI_N1 = Field <_NVIC_IPR59, 8, 8>; // Bits [15:8]
        // Priority of interrupt 236
        using PRI_N0 = Field <_NVIC_IPR59, 0, 8>; // Bits [7:0]
    };

} // namespace NVIC

// Generated from xml file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/cache_registers_v7em.xml
// --------------------------------------------
//Cache: Cache Control
// --------------------------------------------
namespace Cache {

    // CLIDR: Cache Level ID Register
    struct _CLIDR : Register <0xE000ED78, ReadOnly, _CLIDR> {
        // Indicates the Level of Unification Uniprocessor for the cache hierarchy
        using LoUU = Field <_CLIDR, 27, 3>; // Bits [29:27]
        // Level of Coherency
        using LoC = Field <_CLIDR, 24, 3>; // Bits [26:24]
        // Indicates the Level of Unification Inner Shareable for the cache hierarchy
        using LoUIS = Field <_CLIDR, 21, 3>; // Bits [23:21]
        // Cache Type 7
        using CL7 = Field <_CLIDR, 18, 3>; // Bits [20:18]
        // Cache Type 6
        using CL6 = Field <_CLIDR, 15, 3>; // Bits [17:15]
        // Cache Type 5
        using CL5 = Field <_CLIDR, 12, 3>; // Bits [14:12]
        // Cache Type 4
        using CL4 = Field <_CLIDR, 9, 3>; // Bits [11:9]
        // Cache Type 3
        using CL3 = Field <_CLIDR, 6, 3>; // Bits [8:6]
        // Cache Type 2
        using CL2 = Field <_CLIDR, 3, 3>; // Bits [5:3]
        // Cache Type 1
        using CL1 = Field <_CLIDR, 0, 3>; // Bits [2:0]
    };

    // CTR: Cache Type Register
    struct _CTR : Register <0xE000ED7C, ReadOnly, _CTR> {
        // ARMv7 register format
        using F = Field <_CTR, 29, 3>; // Bits [31:29]
        // Cache Writeback Granule
        using CWG = Field <_CTR, 24, 4>; // Bits [27:24]
        // Exclusives Reservation Granule
        using ERG = Field <_CTR, 20, 4>; // Bits [23:20]
        // Log2 of the number of words in the smallest cache line of all the data and unified caches under the core control
        using DMinLine = Field <_CTR, 16, 4>; // Bits [19:16]
        // Indicates the level 1 instruction cache policy for indexing and tagging
        using L1IP = Field <_CTR, 14, 2>; // Bits [15:14]
        // Log2 of the number of words in the smallest cache line of all the instruction caches under the control of the processor
        using IminLine = Field <_CTR, 0, 4>; // Bits [3:0]
    };

    // CCSIDR: Cache Size ID Register
    struct _CCSIDR : Register <0xE000ED80, ReadOnly, _CCSIDR> {
        // Write-Through
        using WT = Field <_CCSIDR, 31, 1>; // Bits [31]
        // Write-Back
        using WB = Field <_CCSIDR, 30, 1>; // Bits [30]
        // Read-Allocation
        using RA = Field <_CCSIDR, 29, 1>; // Bits [29]
        // Write-Allocation
        using WA = Field <_CCSIDR, 28, 1>; // Bits [28]
        // Number of sets
        using NS = Field <_CCSIDR, 13, 15>; // Bits [27:13]
        // Associativity
        using A = Field <_CCSIDR, 3, 10>; // Bits [12:3]
        // Line Size
        using LS = Field <_CCSIDR, 0, 3>; // Bits [2:0]
    };

    // CSSELR: Cache Size Selection Register
    struct _CSSELR : Register <0xE000ED84, ReadWrite, _CSSELR> {
        // Level
        using L = Field <_CSSELR, 1, 3>; // Bits [3:1]
        // Type
        using IND = Field <_CSSELR, 0, 1>; // Bits [0]
    };

    // ICIALLU: Instruction Cache Invalidate All to PoU
    struct _ICIALLU : Register <0xE000EF50, WriteOnly, _ICIALLU> {
    };

    // ICIMVAU: Instruction cache invalidate by MVA to PoU
    struct _ICIMVAU : Register <0xE000EF58, WriteOnly, _ICIMVAU> {
    };

    // DCIMVAC: Data cache invalidate by MVA to PoC
    struct _DCIMVAC : Register <0xE000EF5C, WriteOnly, _DCIMVAC> {
    };

    // DCISW: Data cache invalidate by set and way
    struct _DCISW : Register <0xE000EF60, WriteOnly, _DCISW> {
    };

    // DCCMVAU: Data cache clean by MVA to PoU
    struct _DCCMVAU : Register <0xE000EF64, WriteOnly, _DCCMVAU> {
    };

    // DCCMVAC: Data cache clean by MVA to PoC
    struct _DCCMVAC : Register <0xE000EF68, WriteOnly, _DCCMVAC> {
    };

    // DCCSW: Data cache clean by set and way
    struct _DCCSW : Register <0xE000EF6C, WriteOnly, _DCCSW> {
    };

    // DCCIMVAC: Data cache clean and invalidate by MVA to PoC
    struct _DCCIMVAC : Register <0xE000EF70, WriteOnly, _DCCIMVAC> {
    };

    // DCCISW: Data cache clean and invalidate by set and way
    struct _DCCISW : Register <0xE000EF74, WriteOnly, _DCCISW> {
    };

    // BPIALL: Branch predictor invalidate all
    struct _BPIALL : Register <0xE000EF78, WriteOnly, _BPIALL> {
    };

} // namespace Cache

// Generated from xml file: C:/Users/doomt/Documents/GitHub/MCORE/RegistersGen/xml-svd/mpu_registers_v7m.xml
// --------------------------------------------
//MPU: Memory Protection Unit
// --------------------------------------------
namespace MPU {

    // MPU_TYPE: MPU Type Register
    struct _MPU_TYPE : Register <0xE000ED90, ReadOnly, _MPU_TYPE> {
        // Indicates support for separate instruction and data address maps
        using SEPARATE = Field <_MPU_TYPE, 0, 1>; // Bits [0]
        // Number of regions supported by the MPU
        using DREGION = Field <_MPU_TYPE, 8, 8>; // Bits [15:8]
        // Number of instruction regions supported by the MPU
        using IREGION = Field <_MPU_TYPE, 16, 8>; // Bits [23:16]
    };

    // MPU_CTRL: MPU Control Register
    struct _MPU_CTRL : Register <0xE000ED94, ReadWrite, _MPU_CTRL> {
        // Enables the MPU
        using ENABLE = Field <_MPU_CTRL, 0, 1>; // Bits [0]
        // When the ENABLE bit is set to 1, controls whether handlers executing with priority less than 0 access memory with the MPU enabled or with the MPU disabled
        using HFNMIENA = Field <_MPU_CTRL, 1, 1>; // Bits [1]
        // When the ENABLE bit is set to 1, Enables the default memory map as a background region for privileged access
        using PRIVDEFENA = Field <_MPU_CTRL, 2, 1>; // Bits [2]
    };

    // MPU_RNR: MPU Region Number Register
    struct _MPU_RNR : Register <0xE000ED98, ReadWrite, _MPU_RNR> {
        // Indicates the memory region accessed by MPU_RBAR and MPU_RSAR
        using REGION = Field <_MPU_RNR, 0, 8>; // Bits [7:0]
    };

    // MPU_RBAR: MPU Region Base Address Register
    struct _MPU_RBAR : Register <0xE000ED9C, ReadWrite, _MPU_RBAR> {
        // On writes, can specify the number of the region to update. On reads, returns bits [3:0] of MPU_RNR
        using REGION = Field <_MPU_RBAR, 0, 4>; // Bits [3:0]
        // On writes, indicates whether the region to update is specified by MPU_RNR.REGION, or by the REGION value specified in this write. When using the REGION value specified by this write, MPU_RNR.REGION is updated to this value
        using VALID = Field <_MPU_RBAR, 4, 1>; // Bits [4]
        // Base address of the region
        using ADDR = Field <_MPU_RBAR, 5, 27>; // Bits [31:5]
    };

    // MPU_RASR: MPU Region Attribute and Size Register
    struct _MPU_RASR : Register <0xE000EDA0, ReadWrite, _MPU_RASR> {
        // Enables this region
        using ENABLE = Field <_MPU_RASR, 0, 1>; // Bits [0]
        // Indicates the region size
        using SIZE = Field <_MPU_RASR, 1, 5>; // Bits [5:1]
        // Subregion Disable
        using SRD = Field <_MPU_RASR, 8, 8>; // Bits [15:8]
        // MPU Region Attribute field
        using B = Field <_MPU_RASR, 16, 1>; // Bits [16]
        // MPU Region Attribute field
        using C = Field <_MPU_RASR, 17, 1>; // Bits [17]
        // MPU Region Attribute field: Sharable
        using S = Field <_MPU_RASR, 18, 1>; // Bits [18]
        // MPU Region Attribute field
        using TEX = Field <_MPU_RASR, 19, 3>; // Bits [21:19]
        // Access permissions
        using AP = Field <_MPU_RASR, 24, 3>; // Bits [26:24]
        // Execute Never
        using XN = Field <_MPU_RASR, 28, 1>; // Bits [28]
    };

    // MPU_RBAR_A1: MPU alias register for MPU Region Base Address Register
    struct _MPU_RBAR_A1 : Register <0xE000EDA4, ReadWrite, _MPU_RBAR_A1> {
        // On writes, can specify the number of the region to update. On reads, returns bits [3:0] of MPU_RNR
        using REGION = Field <_MPU_RBAR_A1, 0, 4>; // Bits [3:0]
        // On writes, indicates whether the region to update is specified by MPU_RNR.REGION, or by the REGION value specified in this write. When using the REGION value specified by this write, MPU_RNR.REGION is updated to this value
        using VALID = Field <_MPU_RBAR_A1, 4, 1>; // Bits [4]
        // Base address of the region
        using ADDR = Field <_MPU_RBAR_A1, 5, 27>; // Bits [31:5]
    };

    // MPU_RASR_A1: MPU alias register for MPU Region Attribute and Size Register
    struct _MPU_RASR_A1 : Register <0xE000EDA8, ReadWrite, _MPU_RASR_A1> {
        // Enables this region
        using ENABLE = Field <_MPU_RASR_A1, 0, 1>; // Bits [0]
        // Indicates the region size
        using SIZE = Field <_MPU_RASR_A1, 1, 5>; // Bits [5:1]
        // Subregion Disable
        using SRD = Field <_MPU_RASR_A1, 8, 8>; // Bits [15:8]
        // MPU Region Attribute field
        using B = Field <_MPU_RASR_A1, 16, 1>; // Bits [16]
        // MPU Region Attribute field
        using C = Field <_MPU_RASR_A1, 17, 1>; // Bits [17]
        // MPU Region Attribute field: Sharable
        using S = Field <_MPU_RASR_A1, 18, 1>; // Bits [18]
        // MPU Region Attribute field
        using TEX = Field <_MPU_RASR_A1, 19, 3>; // Bits [21:19]
        // Access permissions
        using AP = Field <_MPU_RASR_A1, 24, 3>; // Bits [26:24]
        // Execute Never
        using XN = Field <_MPU_RASR_A1, 28, 1>; // Bits [28]
    };

    // MPU_RBAR_A2: MPU alias register for MPU Region Base Address Register
    struct _MPU_RBAR_A2 : Register <0xE000EDAC, ReadWrite, _MPU_RBAR_A2> {
        // On writes, can specify the number of the region to update. On reads, returns bits [3:0] of MPU_RNR
        using REGION = Field <_MPU_RBAR_A2, 0, 4>; // Bits [3:0]
        // On writes, indicates whether the region to update is specified by MPU_RNR.REGION, or by the REGION value specified in this write. When using the REGION value specified by this write, MPU_RNR.REGION is updated to this value
        using VALID = Field <_MPU_RBAR_A2, 4, 1>; // Bits [4]
        // Base address of the region
        using ADDR = Field <_MPU_RBAR_A2, 5, 27>; // Bits [31:5]
    };

    // MPU_RASR_A2: MPU alias register for MPU Region Attribute and Size Register
    struct _MPU_RASR_A2 : Register <0xE000EDB0, ReadWrite, _MPU_RASR_A2> {
        // Enables this region
        using ENABLE = Field <_MPU_RASR_A2, 0, 1>; // Bits [0]
        // Indicates the region size
        using SIZE = Field <_MPU_RASR_A2, 1, 5>; // Bits [5:1]
        // Subregion Disable
        using SRD = Field <_MPU_RASR_A2, 8, 8>; // Bits [15:8]
        // MPU Region Attribute field
        using B = Field <_MPU_RASR_A2, 16, 1>; // Bits [16]
        // MPU Region Attribute field
        using C = Field <_MPU_RASR_A2, 17, 1>; // Bits [17]
        // MPU Region Attribute field: Sharable
        using S = Field <_MPU_RASR_A2, 18, 1>; // Bits [18]
        // MPU Region Attribute field
        using TEX = Field <_MPU_RASR_A2, 19, 3>; // Bits [21:19]
        // Access permissions
        using AP = Field <_MPU_RASR_A2, 24, 3>; // Bits [26:24]
        // Execute Never
        using XN = Field <_MPU_RASR_A2, 28, 1>; // Bits [28]
    };

    // MPU_RBAR_A3: MPU alias register for MPU Region Base Address Register
    struct _MPU_RBAR_A3 : Register <0xE000EDB4, ReadWrite, _MPU_RBAR_A3> {
        // On writes, can specify the number of the region to update. On reads, returns bits [3:0] of MPU_RNR
        using REGION = Field <_MPU_RBAR_A3, 0, 4>; // Bits [3:0]
        // On writes, indicates whether the region to update is specified by MPU_RNR.REGION, or by the REGION value specified in this write. When using the REGION value specified by this write, MPU_RNR.REGION is updated to this value
        using VALID = Field <_MPU_RBAR_A3, 4, 1>; // Bits [4]
        // Base address of the region
        using ADDR = Field <_MPU_RBAR_A3, 5, 27>; // Bits [31:5]
    };

    // MPU_RASR_A3: MPU alias register for MPU Region Attribute and Size Register
    struct _MPU_RASR_A3 : Register <0xE000EDB8, ReadWrite, _MPU_RASR_A3> {
        // Enables this region
        using ENABLE = Field <_MPU_RASR_A3, 0, 1>; // Bits [0]
        // Indicates the region size
        using SIZE = Field <_MPU_RASR_A3, 1, 5>; // Bits [5:1]
        // Subregion Disable
        using SRD = Field <_MPU_RASR_A3, 8, 8>; // Bits [15:8]
        // MPU Region Attribute field
        using B = Field <_MPU_RASR_A3, 16, 1>; // Bits [16]
        // MPU Region Attribute field
        using C = Field <_MPU_RASR_A3, 17, 1>; // Bits [17]
        // MPU Region Attribute field: Sharable
        using S = Field <_MPU_RASR_A3, 18, 1>; // Bits [18]
        // MPU Region Attribute field
        using TEX = Field <_MPU_RASR_A3, 19, 3>; // Bits [21:19]
        // Access permissions
        using AP = Field <_MPU_RASR_A3, 24, 3>; // Bits [26:24]
        // Execute Never
        using XN = Field <_MPU_RASR_A3, 28, 1>; // Bits [28]
    };

} // namespace MPU


    /* 
    ---------------------------------------------------------------------------------
    --------------------- End of Coretex-M7 Register Descripton ---------------------
    ---------------------------------------------------------------------------------
    */
