#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/SmbiosLib.h>
#include <Library/PcdLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/MpService.h>
#include <Register/Intel/ArchitecturalMsr.h>

/**
  CPUID Signature Information

  @param   EAX  CPUID_SIGNATURE (0x00)

  @retval  EAX  Returns the highest value the CPUID instruction recognizes for
                returning basic processor information. The value is returned is
                processor specific.
  @retval  EBX  First 4 characters of a vendor identification string.
  @retval  ECX  Last 4 characters of a vendor identification string.
  @retval  EDX  Middle 4 characters of a vendor identification string.

  <b>Example usage</b>
  @code
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;

  AsmCpuid (CPUID_SIGNATURE, &Eax, &Ebx, &Ecx, &Edx);
  @endcode
**/
#define CPUID_SIGNATURE                         0x00

///
/// @{ CPUID signature values returned by Intel processors
///
#define CPUID_SIGNATURE_GENUINE_INTEL_EBX  SIGNATURE_32 ('G', 'e', 'n', 'u')
#define CPUID_SIGNATURE_GENUINE_INTEL_EDX  SIGNATURE_32 ('i', 'n', 'e', 'I')
#define CPUID_SIGNATURE_GENUINE_INTEL_ECX  SIGNATURE_32 ('n', 't', 'e', 'l')
///
/// @}
///

/**
  CPUID Version Information

  @param   EAX  CPUID_VERSION_INFO (0x01)

  @retval  EAX  Returns Model, Family, Stepping Information described by the
                type CPUID_VERSION_INFO_EAX.
  @retval  EBX  Returns Brand, Cache Line Size, and Initial APIC ID described by
                the type CPUID_VERSION_INFO_EBX.
  @retval  ECX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_ECX.
  @retval  EDX  CPU Feature Information described by the type
                CPUID_VERSION_INFO_EDX.

  <b>Example usage</b>
  @code
  CPUID_VERSION_INFO_EAX  Eax;
  CPUID_VERSION_INFO_EBX  Ebx;
  CPUID_VERSION_INFO_ECX  Ecx;
  CPUID_VERSION_INFO_EDX  Edx;

  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  @endcode
**/
#define CPUID_VERSION_INFO                      0x01

/**
  CPUID Version Information returned in EAX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  SteppingId:4;        ///< [Bits   3:0] Stepping ID
    UINT32  Model:4;             ///< [Bits   7:4] Model
    UINT32  FamilyId:4;          ///< [Bits  11:8] Family
    UINT32  ProcessorType:2;     ///< [Bits 13:12] Processor Type
    UINT32  Reserved1:2;         ///< [Bits 15:14] Reserved
    UINT32  ExtendedModelId:4;   ///< [Bits 19:16] Extended Model ID
    UINT32  ExtendedFamilyId:8;  ///< [Bits 27:20] Extended Family ID
    UINT32  Reserved2:4;         ///< Reserved
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EAX;

///
/// @{ Define value for bit field CPUID_VERSION_INFO_EAX.ProcessorType
///
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_ORIGINAL_OEM_PROCESSOR     0x00
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_INTEL_OVERDRIVE_PROCESSOR  0x01
#define CPUID_VERSION_INFO_EAX_PROCESSOR_TYPE_DUAL_PROCESSOR             0x02
///
/// @}
///

/**
  CPUID Version Information returned in EBX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 7:0] Provides an entry into a brand string table that contains
    /// brand strings for IA-32 processors.
    ///
    UINT32  BrandIndex:8;
    ///
    /// [Bits 15:8] Indicates the size of the cache line flushed by the CLFLUSH
    /// and CLFLUSHOPT instructions in 8-byte increments. This field was
    /// introduced in the Pentium 4 processor.
    ///
    UINT32  CacheLineSize:8;
    ///
    /// [Bits 23:16] Maximum number of addressable IDs for logical processors
    /// in this physical package.
    ///
    /// @note
    /// The nearest power-of-2 integer that is not smaller than EBX[23:16] is
    /// the number of unique initial APICIDs reserved for addressing different
    /// logical processors in a physical package. This field is only valid if
    /// CPUID.1.EDX.HTT[bit 28]= 1.
    ///
    UINT32  MaximumAddressableIdsForLogicalProcessors:8;
    ///
    /// [Bits 31:24] The 8-bit ID that is assigned to the local APIC on the
    /// processor during power up. This field was introduced in the Pentium 4
    /// processor.
    ///
    UINT32  InitialLocalApicId:8;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EBX;

/**
  CPUID Version Information returned in ECX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] Streaming SIMD Extensions 3 (SSE3).  A value of 1 indicates the
    /// processor supports this technology
    ///
    UINT32  SSE3:1;
    ///
    /// [Bit 1] A value of 1 indicates the processor supports the PCLMULQDQ
    /// instruction.  Carryless Multiplication
    ///
    UINT32  PCLMULQDQ:1;
    ///
    /// [Bit 2] 64-bit DS Area.  A value of 1 indicates the processor supports
    /// DS area using 64-bit layout.
    ///
    UINT32  DTES64:1;
    ///
    /// [Bit 3] MONITOR/MWAIT.  A value of 1 indicates the processor supports
    /// this feature.
    ///
    UINT32  MONITOR:1;
    ///
    /// [Bit 4] CPL Qualified Debug Store.  A value of 1 indicates the processor
    /// supports the extensions to the Debug Store feature to allow for branch
    /// message storage qualified by CPL
    ///
    UINT32  DS_CPL:1;
    ///
    /// [Bit 5] Virtual Machine Extensions.  A value of 1 indicates that the
    /// processor supports this technology.
    ///
    UINT32  VMX:1;
    ///
    /// [Bit 6] Safer Mode Extensions. A value of 1 indicates that the processor
    /// supports this technology
    ///
    UINT32  SMX:1;
    ///
    /// [Bit 7] Enhanced Intel SpeedStep(R) technology.  A value of 1 indicates
    /// that the processor supports this technology
    ///
    UINT32  EIST:1;
    ///
    /// [Bit 8] Thermal Monitor 2.  A value of 1 indicates whether the processor
    /// supports this technology
    ///
    UINT32  TM2:1;
    ///
    /// [Bit 9] A value of 1 indicates the presence of the Supplemental Streaming
    /// SIMD Extensions 3 (SSSE3). A value of 0 indicates the instruction
    /// extensions are not present in the processor.
    ///
    UINT32  SSSE3:1;
    ///
    /// [Bit 10] L1 Context ID.  A value of 1 indicates the L1 data cache mode
    /// can be set to either adaptive mode or shared mode. A value of 0 indicates
    /// this feature is not supported. See definition of the IA32_MISC_ENABLE MSR
    /// Bit 24 (L1 Data Cache Context Mode) for details
    ///
    UINT32  CNXT_ID:1;
    ///
    /// [Bit 11] A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
    /// MSR for silicon debug
    ///
    UINT32  SDBG:1;
    ///
    /// [Bit 12] A value of 1 indicates the processor supports FMA (Fused Multiple
    ///  Add) extensions using YMM state.
    ///
    UINT32  FMA:1;
    ///
    /// [Bit 13] CMPXCHG16B Available.  A value of 1 indicates that the feature
    /// is available.
    ///
    UINT32  CMPXCHG16B:1;
    ///
    /// [Bit 14] xTPR Update Control.  A value of 1 indicates that the processor
    /// supports changing IA32_MISC_ENABLE[Bit 23].
    ///
    UINT32  xTPR_Update_Control:1;
    ///
    /// [Bit 15] Perfmon and Debug Capability:  A value of 1 indicates the
    /// processor supports the performance and debug feature indication MSR
    /// IA32_PERF_CAPABILITIES.
    ///
    UINT32  PDCM:1;
    UINT32  Reserved:1;
    ///
    /// [Bit 17] Process-context identifiers.  A value of 1 indicates that the
    /// processor supports PCIDs and that software may set CR4.PCIDE to 1.
    ///
    UINT32  PCID:1;
    ///
    /// [Bit 18] A value of 1 indicates the processor supports the ability to
    /// prefetch data from a memory mapped device.  Direct Cache Access.
    ///
    UINT32  DCA:1;
    ///
    /// [Bit 19] A value of 1 indicates that the processor supports SSE4.1.
    ///
    UINT32  SSE4_1:1;
    ///
    /// [Bit 20] A value of 1 indicates that the processor supports SSE4.2.
    ///
    UINT32  SSE4_2:1;
    ///
    /// [Bit 21] A value of 1 indicates that the processor supports x2APIC
    /// feature.
    ///
    UINT32  x2APIC:1;
    ///
    /// [Bit 22] A value of 1 indicates that the processor supports MOVBE
    /// instruction.
    ///
    UINT32  MOVBE:1;
    ///
    /// [Bit 23] A value of 1 indicates that the processor supports the POPCNT
    /// instruction.
    ///
    UINT32  POPCNT:1;
    ///
    /// [Bit 24] A value of 1 indicates that the processor's local APIC timer
    /// supports one-shot operation using a TSC deadline value.
    ///
    UINT32  TSC_Deadline:1;
    ///
    /// [Bit 25] A value of 1 indicates that the processor supports the AESNI
    /// instruction extensions.
    ///
    UINT32  AESNI:1;
    ///
    /// [Bit 26] A value of 1 indicates that the processor supports the
    /// XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV
    /// instructions, and XCR0.
    ///
    UINT32  XSAVE:1;
    ///
    /// [Bit 27] A value of 1 indicates that the OS has set CR4.OSXSAVE[Bit 18]
    /// to enable XSETBV/XGETBV instructions to access XCR0 and to support
    /// processor extended state management using XSAVE/XRSTOR.
    ///
    UINT32  OSXSAVE:1;
    ///
    /// [Bit 28] A value of 1 indicates the processor supports the AVX instruction
    /// extensions.
    ///
    UINT32  AVX:1;
    ///
    /// [Bit 29] A value of 1 indicates that processor supports 16-bit
    /// floating-point conversion instructions.
    ///
    UINT32  F16C:1;
    ///
    /// [Bit 30] A value of 1 indicates that processor supports RDRAND instruction.
    ///
    UINT32  RDRAND:1;
    ///
    /// [Bit 31] Always returns 0.
    ///
    UINT32  NotUsed:1;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_ECX;

/**
  CPUID Version Information returned in EDX for CPUID leaf
  #CPUID_VERSION_INFO.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] Floating Point Unit On-Chip. The processor contains an x87 FPU.
    ///
    UINT32  FPU:1;
    ///
    /// [Bit 1] Virtual 8086 Mode Enhancements.  Virtual 8086 mode enhancements,
    /// including CR4.VME for controlling the feature, CR4.PVI for protected
    /// mode virtual interrupts, software interrupt indirection, expansion of
    /// the TSS with the software indirection bitmap, and EFLAGS.VIF and
    /// EFLAGS.VIP flags.
    ///
    UINT32  VME:1;
    ///
    /// [Bit 2] Debugging Extensions.  Support for I/O breakpoints, including
    /// CR4.DE for controlling the feature, and optional trapping of accesses to
    /// DR4 and DR5.
    ///
    UINT32  DE:1;
    ///
    /// [Bit 3] Page Size Extension.  Large pages of size 4 MByte are supported,
    /// including CR4.PSE for controlling the feature, the defined dirty bit in
    /// PDE (Page Directory Entries), optional reserved bit trapping in CR3,
    /// PDEs, and PTEs.
    ///
    UINT32  PSE:1;
    ///
    /// [Bit 4] Time Stamp Counter.  The RDTSC instruction is supported,
    /// including CR4.TSD for controlling privilege.
    ///
    UINT32  TSC:1;
    ///
    /// [Bit 5] Model Specific Registers RDMSR and WRMSR Instructions.  The
    /// RDMSR and WRMSR instructions are supported. Some of the MSRs are
    /// implementation dependent.
    ///
    UINT32  MSR:1;
    ///
    /// [Bit 6] Physical Address Extension.  Physical addresses greater than 32
    /// bits are supported: extended page table entry formats, an extra level in
    /// the page translation tables is defined, 2-MByte pages are supported
    /// instead of 4 Mbyte pages if PAE bit is 1.
    ///
    UINT32  PAE:1;
    ///
    /// [Bit 7] Machine Check Exception.  Exception 18 is defined for Machine
    /// Checks, including CR4.MCE for controlling the feature. This feature does
    /// not define the model-specific implementations of machine-check error
    /// logging, reporting, and processor shutdowns. Machine Check exception
    /// handlers may have to depend on processor version to do model specific
    /// processing of the exception, or test for the presence of the Machine
    /// Check feature.
    ///
    UINT32  MCE:1;
    ///
    /// [Bit 8] CMPXCHG8B Instruction.  The compare-and-exchange 8 bytes(64 bits)
    /// instruction is supported (implicitly locked and atomic).
    ///
    UINT32  CX8:1;
    ///
    /// [Bit 9] APIC On-Chip.  The processor contains an Advanced Programmable
    /// Interrupt Controller (APIC), responding to memory mapped commands in the
    /// physical address range FFFE0000H to FFFE0FFFH (by default - some
    /// processors permit the APIC to be relocated).
    ///
    UINT32  APIC:1;
    UINT32  Reserved1:1;
    ///
    /// [Bit 11] SYSENTER and SYSEXIT Instructions.  The SYSENTER and SYSEXIT
    /// and associated MSRs are supported.
    ///
    UINT32  SEP:1;
    ///
    /// [Bit 12] Memory Type Range Registers.  MTRRs are supported. The MTRRcap
    /// MSR contains feature bits that describe what memory types are supported,
    /// how many variable MTRRs are supported, and whether fixed MTRRs are
    /// supported.
    ///
    UINT32  MTRR:1;
    ///
    /// [Bit 13] Page Global Bit.  The global bit is supported in paging-structure
    /// entries that map a page, indicating TLB entries that are common to
    /// different processes and need not be flushed. The CR4.PGE bit controls
    /// this feature.
    ///
    UINT32  PGE:1;
    ///
    /// [Bit 14] Machine Check Architecture. A value of 1 indicates the Machine
    /// Check Architecture of reporting machine errors is supported. The MCG_CAP
    /// MSR contains feature bits describing how many banks of error reporting
    /// MSRs are supported.
    ///
    UINT32  MCA:1;
    ///
    /// [Bit 15] Conditional Move Instructions.  The conditional move instruction
    /// CMOV is supported. In addition, if x87 FPU is present as indicated by the
    /// CPUID.FPU feature bit, then the FCOMI and FCMOV instructions are supported.
    ///
    UINT32  CMOV:1;
    ///
    /// [Bit 16] Page Attribute Table.  Page Attribute Table is supported. This
    /// feature augments the Memory Type Range Registers (MTRRs), allowing an
    /// operating system to specify attributes of memory accessed through a
    /// linear address on a 4KB granularity.
    ///
    UINT32  PAT:1;
    ///
    /// [Bit 17] 36-Bit Page Size Extension.  4-MByte pages addressing physical
    /// memory beyond 4 GBytes are supported with 32-bit paging. This feature
    /// indicates that upper bits of the physical address of a 4-MByte page are
    /// encoded in bits 20:13 of the page-directory entry. Such physical
    /// addresses are limited by MAXPHYADDR and may be up to 40 bits in size.
    ///
    UINT32  PSE_36:1;
    ///
    /// [Bit 18] Processor Serial Number.  The processor supports the 96-bit
    /// processor identification number feature and the feature is enabled.
    ///
    UINT32  PSN:1;
    ///
    /// [Bit 19] CLFLUSH Instruction.  CLFLUSH Instruction is supported.
    ///
    UINT32  CLFSH:1;
    UINT32  Reserved2:1;
    ///
    /// [Bit 21] Debug Store.  The processor supports the ability to write debug
    /// information into a memory resident buffer.  This feature is used by the
    /// branch trace store (BTS) and precise event-based sampling (PEBS)
    /// facilities.
    ///
    UINT32  DS:1;
    ///
    /// [Bit 22] Thermal Monitor and Software Controlled Clock Facilities.  The
    /// processor implements internal MSRs that allow processor temperature to
    /// be monitored and processor performance to be modulated in predefined
    /// duty cycles under software control.
    ///
    UINT32  ACPI:1;
    ///
    /// [Bit 23] Intel MMX Technology.  The processor supports the Intel MMX
    /// technology.
    ///
    UINT32  MMX:1;
    ///
    /// [Bit 24] FXSAVE and FXRSTOR Instructions.  The FXSAVE and FXRSTOR
    /// instructions are supported for fast save and restore of the floating
    /// point context. Presence of this bit also indicates that CR4.OSFXSR is
    /// available for an operating system to indicate that it supports the
    /// FXSAVE and FXRSTOR instructions.
    ///
    UINT32  FXSR:1;
    ///
    /// [Bit 25] SSE.  The processor supports the SSE extensions.
    ///
    UINT32  SSE:1;
    ///
    /// [Bit 26] SSE2.  The processor supports the SSE2 extensions.
    ///
    UINT32  SSE2:1;
    ///
    /// [Bit 27] Self Snoop.  The processor supports the management of
    /// conflicting memory types by performing a snoop of its own cache
    /// structure for transactions issued to the bus.
    ///
    UINT32  SS:1;
    ///
    /// [Bit 28] Max APIC IDs reserved field is Valid.  A value of 0 for HTT
    /// indicates there is only a single logical processor in the package and
    /// software should assume only a single APIC ID is reserved. A value of 1
    /// for HTT indicates the value in CPUID.1.EBX[23:16] (the Maximum number of
    /// addressable IDs for logical processors in this package) is valid for the
    /// package.
    ///
    UINT32  HTT:1;
    ///
    /// [Bit 29] Thermal Monitor.  The processor implements the thermal monitor
    /// automatic thermal control circuitry (TCC).
    ///
    UINT32  TM:1;
    UINT32  Reserved3:1;
    ///
    /// [Bit 31] Pending Break Enable.  The processor supports the use of the
    /// FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is
    /// asserted) to signal the processor that an interrupt is pending and that
    /// the processor should return to normal operation to handle the interrupt.
    /// Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
    ///
    UINT32  PBE:1;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32    Uint32;
} CPUID_VERSION_INFO_EDX;