#pragma once
#include "includes.h"

enum CPUID
{
    vendor_and_max_standard_fn_number = 0x0,
    processor_feature_identifier = 0x1,
    ext_perfmon_and_debug = 0x80000022,
    svm_features = 0x8000000A,
};

enum MSR : UINT64
{
    APIC_BAR = 0x1b,
    VM_CR = 0xC0010114,
    EFER = 0xC0000080,
    PAT = 0x277,  /*  Page Atrribute Table, see   MSR0000_0277    */
    VM_HSAVE_PA = 0xC0010117
};

/*  TSS */
struct TaskStateSegment 
{ 
    int32_t reserved1;
    uintptr_t stack_rings[3];
    uintptr_t reserved2;
    void* ist[7];
    int64_t reserved3;
    int16_t reserved4;
    uint16_t iomap_base;
};

/*  from osdev wiki */
struct InterruptDescriptor64 
{
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
};

/*  the vmcb structures are typed out by Satoshi Tanda @tandasat    */
#pragma pack(1)
struct VmcbControlArea
{
    UINT16 InterceptCrRead;             // +0x000
    UINT16 InterceptCrWrite;            // +0x002
    UINT16 InterceptDrRead;             // +0x004
    UINT16 InterceptDrWrite;            // +0x006
    UINT32 InterceptException;          // +0x008
    UINT32 InterceptVec3;               // +0x00c
    UINT32 InterceptVec4;               // +0x010
    UINT8 Reserved1[0x03c - 0x014];     // +0x014 there should be intercept vector 5 here
    UINT16 PauseFilterThreshold;        // +0x03c
    UINT16 PauseFilterCount;            // +0x03e
    UINT64 IopmBasePa;                  // +0x040
    UINT64 MsrpmBasePa;                 // +0x048
    UINT64 TscOffset;                   // +0x050
    UINT32 GuestAsid;                   // +0x058
    UINT32 TlbControl;                  // +0x05c
    UINT64 VIntr;                       // +0x060
    UINT64 InterruptShadow;             // +0x068
    UINT64 ExitCode;                    // +0x070
    UINT64 ExitInfo1;                   // +0x078
    UINT64 ExitInfo2;                   // +0x080
    UINT64 ExitIntInfo;                 // +0x088
    UINT64 NpEnable;                    // +0x090
    UINT64 AvicApicBar;                 // +0x098
    UINT64 GuestPaOfGhcb;               // +0x0a0
    UINT64 EventInj;                    // +0x0a8
    UINT64 NCr3;                        // +0x0b0
    UINT64 LbrVirtualizationEnable;     // +0x0b8
    UINT64 VmcbClean;                   // +0x0c0
    UINT64 NRip;                        // +0x0c8
    UINT8 NumOfBytesFetched;            // +0x0d0
    UINT8 GuestInstructionBytes[15];    // +0x0d1
    UINT64 AvicApicBackingPagePointer;  // +0x0e0
    UINT64 Reserved2;                   // +0x0e8
    UINT64 AvicLogicalTablePointer;     // +0x0f0
    UINT64 AvicPhysicalTablePointer;    // +0x0f8
    UINT64 Reserved3;                   // +0x100
    UINT64 VmcbSaveStatePointer;        // +0x108
    UINT8 Reserved4[0x400 - 0x110];     // +0x110
};
#pragma pack()

static_assert(sizeof(VmcbControlArea) == 0x400,
    "Vmcbcontrol_area Size Mismatch");

//
// See "VMCB Layout, State Save Area"
//
typedef struct VmcbSaveStateArea
{
    UINT16 EsSelector;                  // +0x000
    UINT16 EsAttrib;                    // +0x002
    UINT32 EsLimit;                     // +0x004
    UINT64 EsBase;                      // +0x008
    UINT16 CsSelector;                  // +0x010
    UINT16 CsAttrib;                    // +0x012
    UINT32 CsLimit;                     // +0x014
    UINT64 CsBase;                      // +0x018
    UINT16 SsSelector;                  // +0x020
    UINT16 SsAttrib;                    // +0x022
    UINT32 SsLimit;                     // +0x024
    UINT64 SsBase;                      // +0x028
    UINT16 DsSelector;                  // +0x030
    UINT16 DsAttrib;                    // +0x032
    UINT32 DsLimit;                     // +0x034
    UINT64 DsBase;                      // +0x038
    UINT16 FsSelector;                  // +0x040
    UINT16 FsAttrib;                    // +0x042
    UINT32 FsLimit;                     // +0x044
    UINT64 FsBase;                      // +0x048
    UINT16 GsSelector;                  // +0x050
    UINT16 GsAttrib;                    // +0x052
    UINT32 GsLimit;                     // +0x054
    UINT64 GsBase;                      // +0x058
    UINT16 GdtrSelector;                // +0x060
    UINT16 GdtrAttrib;                  // +0x062
    UINT32 GdtrLimit;                   // +0x064
    UINT64 GdtrBase;                    // +0x068
    UINT16 LdtrSelector;                // +0x070
    UINT16 LdtrAttrib;                  // +0x072
    UINT32 LdtrLimit;                   // +0x074
    UINT64 LdtrBase;                    // +0x078
    UINT16 IdtrSelector;                // +0x080
    UINT16 IdtrAttrib;                  // +0x082
    UINT32 IdtrLimit;                   // +0x084
    UINT64 IdtrBase;                    // +0x088
    UINT16 TrSelector;                  // +0x090
    UINT16 TrAttrib;                    // +0x092
    UINT32 TrLimit;                     // +0x094
    UINT64 TrBase;                      // +0x098
    UINT8 Reserved1[0x0cb - 0x0a0];     // +0x0a0
    UINT8 Cpl;                          // +0x0cb
    UINT32 Reserved2;                   // +0x0cc
    UINT64 Efer;                        // +0x0d0
    UINT8 Reserved3[0x148 - 0x0d8];     // +0x0d8
    UINT64 Cr4;                         // +0x148
    UINT64 Cr3;                         // +0x150
    UINT64 Cr0;                         // +0x158
    UINT64 Dr7;                         // +0x160
    UINT64 Dr6;                         // +0x168
    UINT64 Rflags;                      // +0x170
    UINT64 Rip;                         // +0x178
    UINT8 Reserved4[0x1d8 - 0x180];     // +0x180
    UINT64 Rsp;                         // +0x1d8
    UINT8 Reserved5[0x1f8 - 0x1e0];     // +0x1e0
    UINT64 Rax;                         // +0x1f8
    UINT64 Star;                        // +0x200
    UINT64 LStar;                       // +0x208
    UINT64 CStar;                       // +0x210
    UINT64 SfMask;                      // +0x218
    UINT64 KernelGsBase;                // +0x220
    UINT64 SysenterCs;                  // +0x228
    UINT64 SysenterEsp;                 // +0x230
    UINT64 SysenterEip;                 // +0x238
    UINT64 Cr2;                         // +0x240
    UINT8 Reserved6[0x268 - 0x248];     // +0x248
    UINT64 GPat;                        // +0x268
    UINT64 DbgCtl;                      // +0x270
    UINT64 BrFrom;                      // +0x278
    UINT64 BrTo;                        // +0x280
    UINT64 LastExcepFrom;               // +0x288
    UINT64 LastExcepTo;                 // +0x290 ??? This isnt in VMCB layout chart
    UINT64 DBGEXTNCFG;                  // +0x298

    UINT8 Reserved7[0x670 - 0x2A0];     // +0x2A0

    struct GuestLBRStack
    {
        UINT64 address;
        UINT64 target;
    } lbr_stack[16];                    // +0x670

    UINT64 LBR_SELECT;                  // +0x770
};

struct SegmentDescriptor
{
    union
    {
        UINT64 AsUInt64;
        struct
        {
            UINT16 LimitLow;        // [0:15]
            UINT16 BaseLow;         // [16:31]
            UINT32 BaseMiddle : 8;  // [32:39]
            UINT32 Type : 4;        // [40:43]
            UINT32 System : 1;      // [44]
            UINT32 Dpl : 2;         // [45:46]
            UINT32 Present : 1;     // [47]
            UINT32 LimitHigh : 4;   // [48:51]
            UINT32 Avl : 1;         // [52]
            UINT32 LongMode : 1;    // [53]
            UINT32 DefaultBit : 1;  // [54]
            UINT32 Granularity : 1; // [55]
            UINT32 BaseHigh : 8;    // [56:63]
        };
    };
};
static_assert(sizeof(SegmentDescriptor) == 8,
    "SEGMENT_DESCRIPTOR Size Mismatch");

struct SegmentAttribute
{
    union
    {
        uint16_t as_uint16;
        struct
        {
            uint16_t type : 4;        // [0:3]
            uint16_t system : 1;      // [4]
            uint16_t dpl : 2;         // [5:6]
            uint16_t present : 1;     // [7]
            uint16_t avl : 1;         // [8]
            uint16_t long_mode : 1;   // [9]
            uint16_t default_bit : 1; // [10]
            uint16_t granularity : 1; // [11]
            uint16_t reserved1 : 4;   // [12:15]
        } fields;
    };
};


struct GuestRegisters
{
    UINT64  r15;
    UINT64  r14;
    UINT64  r13;
    UINT64  r12;
    UINT64  r11;
    UINT64  r10;
    UINT64  r9;
    UINT64  r8;
    UINT64  rdi;
    UINT64  rsi;
    UINT64  rbp;
    UINT64  rsp;
    UINT64  rbx;
    UINT64  rdx;
    UINT64  rcx;
    UINT64  rax;
};


extern "C" void _sgdt(void* Descriptor);

extern "C" int16_t __readtr();


/*	Core::X86::Msr::EFER	*/
struct  MsrEfer
{
    union 
    {
        __int64	flags;
        struct
        {
            uint32_t    syscall : 1;
            uint32_t    reserved : 7;
            uint32_t    long_mode_enable : 1;
            uint32_t    reserved2 : 1;
            uint32_t    long_mode_active : 1;
            uint32_t    nxe : 1;
            uint32_t    svme : 1;
            uint32_t    lmsle : 1;
            uint32_t    ffxse : 1;
            uint32_t    tce : 1;
            uint32_t    reserved3 : 1;
            uint32_t    m_commit : 1;
            uint32_t    intwb : 1;
            __int64	 reserved4 : 45;
        };
    };
};

/*	 Core::X86::Msr::APIC_BAR	*/
struct MsrApicBar
{
    union
    {
        UINT64 Flags;
        struct
        {
            UINT64 Reserved1 : 8;           // [0:7]
            UINT64 BootstrapProcessor : 1;  // [8]
            UINT64 Reserved2 : 1;           // [9]
            UINT64 EnableX2ApicMode : 1;    // [10]
            UINT64 EnableXApicGlobal : 1;   // [11]
            UINT64 ApicBase : 24;           // [12:35]
        };
    };
};



/*	 Core::X86::Msr::VM_CR	*/
union MsrVmcr
{
    struct 
    {
        int		reserved1 : 1;
        int		intercept_init : 1;
        int		reserved2 : 1;
        int		svm_lock : 1;
        int		svme_disable : 1;
        int		reserved3 : 27;
        int		reserved4 : 32;
    };
    int64_t	flags;
};


/*  vector 2 for exceptions */

union InterceptVector2
{
    struct
    {
        int32_t intercept_de : 1;
        int32_t intercept_db : 1;
        int32_t intercept_nmi : 1;
        int32_t intercept_bp : 1;
        int32_t intercept_of : 1;
        int32_t intercept_br : 1;
        int32_t intercept_ud : 1;
        int32_t intercept_nm : 1;
        int32_t intercept_df : 1;
        int32_t reserved1 : 1;
        int32_t intercept_ts : 1;
        int32_t intercept_np : 1;
        int32_t intercept_ss : 1;
        int32_t intercept_gp : 1;
        int32_t intercept_pf : 1;
        int32_t pad1 : 1;
        int32_t pad2 : 1;
        int32_t pad3 : 1;
        int32_t pad4 : 1;
        int32_t pad5 : 1;
        int32_t pad6 : 1;
        int32_t pad7 : 1;
        int32_t pad8 : 1;
        int32_t pad9 : 1;
        int32_t pad10 : 1;
        int32_t pad11 : 1;
        int32_t pad12 : 1;
        int32_t pad13 : 1;
        int32_t pad14 : 1;
        int32_t pad15 : 1;
        int32_t intercept_db2 : 1;
        int32_t pad17 : 1;
    };
    int32_t as_int32;
};
static_assert(sizeof(InterceptVector2) == 0x4, "InterceptVector2 Size Mismatch");

union InterceptVector4
{ 
    struct
    {
        int32_t intercept_vmrun : 1; // Intercept VMRUN instruction.
        int32_t intercept_vmmcall : 1; // vmmcall  instruction.
        int32_t Intercept_vmload : 1;   // intercept VMLOAD instruction.
        int32_t intercept_vmsave : 1; 
        int32_t Intercept_stgi : 1; 
        int32_t Intercept_clgi : 1;
        int32_t Intercept_skinit : 1;
        int32_t intercept_RDTSCP : 1;
        int32_t intercept_icebp : 1;
        int32_t Intercept_wbinvd : 1;
        int32_t Intercept_monitor : 1;  
        int32_t intercept_mwait : 1;
        int32_t intercept_mwait2 : 1;
        int32_t Intercept_xsetbv : 1;
        int32_t Intercept_rdpru : 1;
        int32_t intercept_efer : 1;
        int32_t write_cr0 : 1;
        int32_t write_cr1 : 1;
        int32_t write_cr2 : 1;
        int32_t write_cr3 : 1;
        int32_t write_cr4 : 1;
        int32_t write_cr5 : 1;
        int32_t write_cr6 : 1;
        int32_t write_cr7 : 1;
        int32_t write_cr8 : 1;
        int32_t write_cr9 : 1;
        int32_t write_cr10 : 1;
        int32_t write_cr11 : 1;
        int32_t write_cr12 : 1;
        int32_t write_cr13 : 1;
        int32_t write_cr14 : 1;
        int32_t write_cr15 : 1;
    };
    int32_t	as_int32;
};
static_assert(sizeof(InterceptVector4) == 0x4, "InterceptVector4 Size Mismatch");


/*	#include <pshpack1.h> to remove struct alignment, so we wont have GDTR value issues	*/
#include <pshpack1.h>
struct DescriptorTableRegister
{
    uint16_t limit;
    uintptr_t base;
};

static_assert(sizeof(DescriptorTableRegister) == 0xA, "DESCRIPTOR_TABLE_REGISTER Size Mismatch");

/*  15.20 Event Injection   */
union EventInjection
{
    struct
    {
        int     vector : 8;
        int     type : 3;
        int     push_error_code : 1;
        int     reserved : 19;
        int     valid : 1;
        int     error_code : 32;
    };
    int64_t fields;
};

struct NestedPageFaultInfo1
{
    union
    {
        uint64_t as_uint64;
        struct
        {
            uint64_t valid : 1;                   // [0]
            uint64_t write : 1;                   // [1]
            uint64_t user : 1;                    // [2]
            uint64_t reserved : 1;                // [3]
            uint64_t execute : 1;                 // [4]
            uint64_t reserved2 : 27;              // [5:31]
            uint64_t guestphysicaladdress : 1;    // [32]
            uint64_t guestpagetables : 1;         // [33]
        } fields;
    };
};

struct PageFaultErrorCode
{
    union
    {
        uint32_t as_uint32;
        struct
        {
            uint32_t valid : 1;
            uint32_t write : 1;
            uint32_t user : 1;
            uint32_t reserved : 1;
            uint32_t execute : 1;
            uint32_t protection_key : 1;
            uint32_t shadow_stack : 1;
            uint32_t reserved2 : 24;
            uint32_t rmp : 1;
        } fields;
    };
};
static_assert(sizeof(PageFaultErrorCode) == sizeof(uint32_t), "PageFaultErrorCode must be 32 bits!");



union ADDRESS_TRANSLATION_HELPER
{
    //
    // Indexes to locate paging-structure entries corresponds to this virtual
    // address.
    //
    struct
    {
        UINT64 Unused : 12;         //< [11:0]
        UINT64 Pt : 9;              //< [20:12]
        UINT64 Pd : 9;              //< [29:21]
        UINT64 Pdpt : 9;            //< [38:30]
        UINT64 Pml4 : 9;            //< [47:39]
    } AsIndex;

    //
    // The page offset for each type of pages. For example, for 4KB pages, bits
    // [11:0] are treated as the page offset and Mapping4Kb can be used for it.
    //
    union
    {
        UINT64 Mapping4Kb : 12;     //< [11:0]
        UINT64 Mapping2Mb : 21;     //< [20:0]
        UINT64 Mapping1Gb : 30;     //< [29:0]
    } AsPageOffset;

    UINT64 AsUInt64;
};

/*  must be 4KB aligned     */
struct VMCB
{
    VmcbControlArea control_area;
    VmcbSaveStateArea save_state_area;
    char pad[PAGE_SIZE - sizeof(VmcbControlArea) - sizeof(VmcbSaveStateArea)];
};