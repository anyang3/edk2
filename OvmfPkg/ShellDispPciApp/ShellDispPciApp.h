#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
// #include <ShellPkg/Library/UefiShellLib/UefiShellLib.h>

typedef union
{
  UINT32 Int32;
  struct
  {
    UINT32 Register : 8;
    UINT32 Function : 3;
    UINT32 Device : 5;
    UINT32 Bus : 8;
    UINT32 REV : 7;
    UINT32 EN : 1;
  } IoCfg;
  struct
  {
    UINT32 Register : 12;
    UINT32 Function : 3;
    UINT32 Device : 5;
    UINT32 Bus : 8;
    UINT32 Base : 4;
  } MmioCfg;
} PcieCfg;

typedef struct {
  CHAR16    *BaseClass; // Pointer to the PCI base class string
  CHAR16    *SubClass;  // Pointer to the PCI sub class string
  CHAR16    *PIFClass;  // Pointer to the PCI programming interface string
} PCI_CLASS_STRINGS;

typedef struct PCI_CLASS_ENTRY_TAG {
  UINT8                         Code;             // Class, subclass or I/F code
  CHAR16                        *DescText;        // Description string
  struct PCI_CLASS_ENTRY_TAG    *LowerLevelClass; // Subclass or I/F if any
} PCI_CLASS_ENTRY;

//
// Declarations of entries which contain printable strings for class codes
// in PCI configuration space
//
PCI_CLASS_ENTRY  PCIBlankEntry[];
PCI_CLASS_ENTRY  PCISubClass_00[];
PCI_CLASS_ENTRY  PCISubClass_01[];
PCI_CLASS_ENTRY  PCISubClass_02[];
PCI_CLASS_ENTRY  PCISubClass_03[];
PCI_CLASS_ENTRY  PCISubClass_04[];
PCI_CLASS_ENTRY  PCISubClass_05[];
PCI_CLASS_ENTRY  PCISubClass_06[];
PCI_CLASS_ENTRY  PCISubClass_07[];
PCI_CLASS_ENTRY  PCISubClass_08[];
PCI_CLASS_ENTRY  PCISubClass_09[];
PCI_CLASS_ENTRY  PCISubClass_0a[];
PCI_CLASS_ENTRY  PCISubClass_0b[];
PCI_CLASS_ENTRY  PCISubClass_0c[];
PCI_CLASS_ENTRY  PCISubClass_0d[];
PCI_CLASS_ENTRY  PCISubClass_0e[];
PCI_CLASS_ENTRY  PCISubClass_0f[];
PCI_CLASS_ENTRY  PCISubClass_10[];
PCI_CLASS_ENTRY  PCISubClass_11[];
PCI_CLASS_ENTRY  PCISubClass_12[];
PCI_CLASS_ENTRY  PCISubClass_13[];
PCI_CLASS_ENTRY  PCIPIFClass_0100[];
PCI_CLASS_ENTRY  PCIPIFClass_0101[];
PCI_CLASS_ENTRY  PCIPIFClass_0105[];
PCI_CLASS_ENTRY  PCIPIFClass_0106[];
PCI_CLASS_ENTRY  PCIPIFClass_0107[];
PCI_CLASS_ENTRY  PCIPIFClass_0108[];
PCI_CLASS_ENTRY  PCIPIFClass_0109[];
PCI_CLASS_ENTRY  PCIPIFClass_0300[];
PCI_CLASS_ENTRY  PCIPIFClass_0604[];
PCI_CLASS_ENTRY  PCIPIFClass_0609[];
PCI_CLASS_ENTRY  PCIPIFClass_060b[];
PCI_CLASS_ENTRY  PCIPIFClass_0700[];
PCI_CLASS_ENTRY  PCIPIFClass_0701[];
PCI_CLASS_ENTRY  PCIPIFClass_0703[];
PCI_CLASS_ENTRY  PCIPIFClass_0800[];
PCI_CLASS_ENTRY  PCIPIFClass_0801[];
PCI_CLASS_ENTRY  PCIPIFClass_0802[];
PCI_CLASS_ENTRY  PCIPIFClass_0803[];
PCI_CLASS_ENTRY  PCIPIFClass_0904[];
PCI_CLASS_ENTRY  PCIPIFClass_0c00[];
PCI_CLASS_ENTRY  PCIPIFClass_0c03[];
PCI_CLASS_ENTRY  PCIPIFClass_0c07[];
PCI_CLASS_ENTRY  PCIPIFClass_0d01[];
PCI_CLASS_ENTRY  PCIPIFClass_0e00[];

//
// Base class strings entries
//
PCI_CLASS_ENTRY  gClassStringList[] = {
  {
    0x00,
    L"Pre 2.0 device",
    PCISubClass_00
  },
  {
    0x01,
    L"Mass Storage Controller",
    PCISubClass_01
  },
  {
    0x02,
    L"Network Controller",
    PCISubClass_02
  },
  {
    0x03,
    L"Display Controller",
    PCISubClass_03
  },
  {
    0x04,
    L"Multimedia Device",
    PCISubClass_04
  },
  {
    0x05,
    L"Memory Controller",
    PCISubClass_05
  },
  {
    0x06,
    L"Bridge Device",
    PCISubClass_06
  },
  {
    0x07,
    L"Simple Communications Controllers",
    PCISubClass_07
  },
  {
    0x08,
    L"Base System Peripherals",
    PCISubClass_08
  },
  {
    0x09,
    L"Input Devices",
    PCISubClass_09
  },
  {
    0x0a,
    L"Docking Stations",
    PCISubClass_0a
  },
  {
    0x0b,
    L"Processors",
    PCISubClass_0b
  },
  {
    0x0c,
    L"Serial Bus Controllers",
    PCISubClass_0c
  },
  {
    0x0d,
    L"Wireless Controllers",
    PCISubClass_0d
  },
  {
    0x0e,
    L"Intelligent IO Controllers",
    PCISubClass_0e
  },
  {
    0x0f,
    L"Satellite Communications Controllers",
    PCISubClass_0f
  },
  {
    0x10,
    L"Encryption/Decryption Controllers",
    PCISubClass_10
  },
  {
    0x11,
    L"Data Acquisition & Signal Processing Controllers",
    PCISubClass_11
  },
  {
    0x12,
    L"Processing Accelerators",
    PCISubClass_12
  },
  {
    0x13,
    L"Non-Essential Instrumentation",
    PCISubClass_13
  },
  {
    0xff,
    L"Device does not fit in any defined classes",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

//
// Subclass strings entries
//
PCI_CLASS_ENTRY  PCIBlankEntry[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_00[] = {
  {
    0x00,
    L"All devices other than VGA",
    PCIBlankEntry
  },
  {
    0x01,
    L"VGA-compatible devices",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_01[] = {
  {
    0x00,
    L"SCSI",
    PCIPIFClass_0100
  },
  {
    0x01,
    L"IDE controller",
    PCIPIFClass_0101
  },
  {
    0x02,
    L"Floppy disk controller",
    PCIBlankEntry
  },
  {
    0x03,
    L"IPI controller",
    PCIBlankEntry
  },
  {
    0x04,
    L"RAID controller",
    PCIBlankEntry
  },
  {
    0x05,
    L"ATA controller with ADMA interface",
    PCIPIFClass_0105
  },
  {
    0x06,
    L"Serial ATA controller",
    PCIPIFClass_0106
  },
  {
    0x07,
    L"Serial Attached SCSI (SAS) controller ",
    PCIPIFClass_0107
  },
  {
    0x08,
    L"Non-volatile memory subsystem",
    PCIPIFClass_0108
  },
  {
    0x09,
    L"Universal Flash Storage (UFS) controller ",
    PCIPIFClass_0109
  },
  {
    0x80,
    L"Other mass storage controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_02[] = {
  {
    0x00,
    L"Ethernet controller",
    PCIBlankEntry
  },
  {
    0x01,
    L"Token ring controller",
    PCIBlankEntry
  },
  {
    0x02,
    L"FDDI controller",
    PCIBlankEntry
  },
  {
    0x03,
    L"ATM controller",
    PCIBlankEntry
  },
  {
    0x04,
    L"ISDN controller",
    PCIBlankEntry
  },
  {
    0x05,
    L"WorldFip controller",
    PCIBlankEntry
  },
  {
    0x06,
    L"PICMG 2.14 Multi Computing",
    PCIBlankEntry
  },
  {
    0x07,
    L"InfiniBand controller",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other network controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_03[] = {
  {
    0x00,
    L"VGA/8514 controller",
    PCIPIFClass_0300
  },
  {
    0x01,
    L"XGA controller",
    PCIBlankEntry
  },
  {
    0x02,
    L"3D controller",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other display controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ PCIBlankEntry
  }
};

PCI_CLASS_ENTRY  PCISubClass_04[] = {
  {
    0x00,
    L"Video device",
    PCIBlankEntry
  },
  {
    0x01,
    L"Audio device",
    PCIBlankEntry
  },
  {
    0x02,
    L"Computer Telephony device",
    PCIBlankEntry
  },
  {
    0x03,
    L"Mixed mode device",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other multimedia device",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_05[] = {
  {
    0x00,
    L"RAM memory controller",
    PCIBlankEntry
  },
  {
    0x01,
    L"Flash memory controller",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other memory controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_06[] = {
  {
    0x00,
    L"Host/PCI bridge",
    PCIBlankEntry
  },
  {
    0x01,
    L"PCI/ISA bridge",
    PCIBlankEntry
  },
  {
    0x02,
    L"PCI/EISA bridge",
    PCIBlankEntry
  },
  {
    0x03,
    L"PCI/Micro Channel bridge",
    PCIBlankEntry
  },
  {
    0x04,
    L"PCI/PCI bridge",
    PCIPIFClass_0604
  },
  {
    0x05,
    L"PCI/PCMCIA bridge",
    PCIBlankEntry
  },
  {
    0x06,
    L"NuBus bridge",
    PCIBlankEntry
  },
  {
    0x07,
    L"CardBus bridge",
    PCIBlankEntry
  },
  {
    0x08,
    L"RACEway bridge",
    PCIBlankEntry
  },
  {
    0x09,
    L"Semi-transparent PCI-to-PCI bridge",
    PCIPIFClass_0609
  },
  {
    0x0A,
    L"InfiniBand-to-PCI host bridge",
    PCIBlankEntry
  },
  {
    0x0B,
    L"Advanced Switching to PCI host bridge",
    PCIPIFClass_060b
  },
  {
    0x80,
    L"Other bridge type",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_07[] = {
  {
    0x00,
    L"Serial controller",
    PCIPIFClass_0700
  },
  {
    0x01,
    L"Parallel port",
    PCIPIFClass_0701
  },
  {
    0x02,
    L"Multiport serial controller",
    PCIBlankEntry
  },
  {
    0x03,
    L"Modem",
    PCIPIFClass_0703
  },
  {
    0x04,
    L"GPIB (IEEE 488.1/2) controller",
    PCIBlankEntry
  },
  {
    0x05,
    L"Smart Card",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other communication device",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_08[] = {
  {
    0x00,
    L"PIC",
    PCIPIFClass_0800
  },
  {
    0x01,
    L"DMA controller",
    PCIPIFClass_0801
  },
  {
    0x02,
    L"System timer",
    PCIPIFClass_0802
  },
  {
    0x03,
    L"RTC controller",
    PCIPIFClass_0803
  },
  {
    0x04,
    L"Generic PCI Hot-Plug controller",
    PCIBlankEntry
  },
  {
    0x05,
    L"SD Host controller",
    PCIBlankEntry
  },
  {
    0x06,
    L"IOMMU",
    PCIBlankEntry
  },
  {
    0x07,
    L"Root Complex Event Collector",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other system peripheral",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_09[] = {
  {
    0x00,
    L"Keyboard controller",
    PCIBlankEntry
  },
  {
    0x01,
    L"Digitizer (pen)",
    PCIBlankEntry
  },
  {
    0x02,
    L"Mouse controller",
    PCIBlankEntry
  },
  {
    0x03,
    L"Scanner controller",
    PCIBlankEntry
  },
  {
    0x04,
    L"Gameport controller",
    PCIPIFClass_0904
  },
  {
    0x80,
    L"Other input controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0a[] = {
  {
    0x00,
    L"Generic docking station",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other type of docking station",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0b[] = {
  {
    0x00,
    L"386",
    PCIBlankEntry
  },
  {
    0x01,
    L"486",
    PCIBlankEntry
  },
  {
    0x02,
    L"Pentium",
    PCIBlankEntry
  },
  {
    0x10,
    L"Alpha",
    PCIBlankEntry
  },
  {
    0x20,
    L"PowerPC",
    PCIBlankEntry
  },
  {
    0x30,
    L"MIPS",
    PCIBlankEntry
  },
  {
    0x40,
    L"Co-processor",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other processor",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0c[] = {
  {
    0x00,
    L"IEEE 1394",
    PCIPIFClass_0c00
  },
  {
    0x01,
    L"ACCESS.bus",
    PCIBlankEntry
  },
  {
    0x02,
    L"SSA",
    PCIBlankEntry
  },
  {
    0x03,
    L"USB",
    PCIPIFClass_0c03
  },
  {
    0x04,
    L"Fibre Channel",
    PCIBlankEntry
  },
  {
    0x05,
    L"System Management Bus",
    PCIBlankEntry
  },
  {
    0x06,
    L"InfiniBand",
    PCIBlankEntry
  },
  {
    0x07,
    L"IPMI",
    PCIPIFClass_0c07
  },
  {
    0x08,
    L"SERCOS Interface Standard (IEC 61491)",
    PCIBlankEntry
  },
  {
    0x09,
    L"CANbus",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other bus type",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0d[] = {
  {
    0x00,
    L"iRDA compatible controller",
    PCIBlankEntry
  },
  {
    0x01,
    L"",
    PCIPIFClass_0d01
  },
  {
    0x10,
    L"RF controller",
    PCIBlankEntry
  },
  {
    0x11,
    L"Bluetooth",
    PCIBlankEntry
  },
  {
    0x12,
    L"Broadband",
    PCIBlankEntry
  },
  {
    0x20,
    L"Ethernet (802.11a - 5 GHz)",
    PCIBlankEntry
  },
  {
    0x21,
    L"Ethernet (802.11b - 2.4 GHz)",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other type of wireless controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0e[] = {
  {
    0x00,
    L"I2O Architecture",
    PCIPIFClass_0e00
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_0f[] = {
  {
    0x01,
    L"TV",
    PCIBlankEntry
  },
  {
    0x02,
    L"Audio",
    PCIBlankEntry
  },
  {
    0x03,
    L"Voice",
    PCIBlankEntry
  },
  {
    0x04,
    L"Data",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other satellite communication controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_10[] = {
  {
    0x00,
    L"Network & computing Encrypt/Decrypt",
    PCIBlankEntry
  },
  {
    0x01,
    L"Entertainment Encrypt/Decrypt",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other Encrypt/Decrypt",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_11[] = {
  {
    0x00,
    L"DPIO modules",
    PCIBlankEntry
  },
  {
    0x01,
    L"Performance Counters",
    PCIBlankEntry
  },
  {
    0x10,
    L"Communications synchronization plus time and frequency test/measurement ",
    PCIBlankEntry
  },
  {
    0x20,
    L"Management card",
    PCIBlankEntry
  },
  {
    0x80,
    L"Other DAQ & SP controllers",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_12[] = {
  {
    0x00,
    L"Processing Accelerator",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCISubClass_13[] = {
  {
    0x00,
    L"Non-Essential Instrumentation Function",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

//
// Programming Interface entries
//
PCI_CLASS_ENTRY  PCIPIFClass_0100[] = {
  {
    0x00,
    L"SCSI controller",
    PCIBlankEntry
  },
  {
    0x11,
    L"SCSI storage device SOP using PQI",
    PCIBlankEntry
  },
  {
    0x12,
    L"SCSI controller SOP using PQI",
    PCIBlankEntry
  },
  {
    0x13,
    L"SCSI storage device and controller SOP using PQI",
    PCIBlankEntry
  },
  {
    0x21,
    L"SCSI storage device SOP using NVMe",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0101[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"OM-primary",
    PCIBlankEntry
  },
  {
    0x02,
    L"PI-primary",
    PCIBlankEntry
  },
  {
    0x03,
    L"OM/PI-primary",
    PCIBlankEntry
  },
  {
    0x04,
    L"OM-secondary",
    PCIBlankEntry
  },
  {
    0x05,
    L"OM-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x06,
    L"PI-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x07,
    L"OM/PI-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x08,
    L"OM-secondary",
    PCIBlankEntry
  },
  {
    0x09,
    L"OM-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x0a,
    L"PI-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x0b,
    L"OM/PI-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x0c,
    L"OM-secondary",
    PCIBlankEntry
  },
  {
    0x0d,
    L"OM-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x0e,
    L"PI-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x0f,
    L"OM/PI-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x80,
    L"Master",
    PCIBlankEntry
  },
  {
    0x81,
    L"Master, OM-primary",
    PCIBlankEntry
  },
  {
    0x82,
    L"Master, PI-primary",
    PCIBlankEntry
  },
  {
    0x83,
    L"Master, OM/PI-primary",
    PCIBlankEntry
  },
  {
    0x84,
    L"Master, OM-secondary",
    PCIBlankEntry
  },
  {
    0x85,
    L"Master, OM-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x86,
    L"Master, PI-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x87,
    L"Master, OM/PI-primary, OM-secondary",
    PCIBlankEntry
  },
  {
    0x88,
    L"Master, OM-secondary",
    PCIBlankEntry
  },
  {
    0x89,
    L"Master, OM-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x8a,
    L"Master, PI-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x8b,
    L"Master, OM/PI-primary, PI-secondary",
    PCIBlankEntry
  },
  {
    0x8c,
    L"Master, OM-secondary",
    PCIBlankEntry
  },
  {
    0x8d,
    L"Master, OM-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x8e,
    L"Master, PI-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x8f,
    L"Master, OM/PI-primary, OM/PI-secondary",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0105[] = {
  {
    0x20,
    L"Single stepping",
    PCIBlankEntry
  },
  {
    0x30,
    L"Continuous operation",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0106[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"AHCI",
    PCIBlankEntry
  },
  {
    0x02,
    L"Serial Storage Bus",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0107[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"Obsolete",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0108[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"NVMHCI",
    PCIBlankEntry
  },
  {
    0x02,
    L"NVM Express",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0109[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"UFSHCI",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0300[] = {
  {
    0x00,
    L"VGA compatible",
    PCIBlankEntry
  },
  {
    0x01,
    L"8514 compatible",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0604[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"Subtractive decode",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0609[] = {
  {
    0x40,
    L"Primary PCI bus side facing the system host processor",
    PCIBlankEntry
  },
  {
    0x80,
    L"Secondary PCI bus side facing the system host processor",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_060b[] = {
  {
    0x00,
    L"Custom",
    PCIBlankEntry
  },
  {
    0x01,
    L"ASI-SIG Defined Portal",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0700[] = {
  {
    0x00,
    L"Generic XT-compatible",
    PCIBlankEntry
  },
  {
    0x01,
    L"16450-compatible",
    PCIBlankEntry
  },
  {
    0x02,
    L"16550-compatible",
    PCIBlankEntry
  },
  {
    0x03,
    L"16650-compatible",
    PCIBlankEntry
  },
  {
    0x04,
    L"16750-compatible",
    PCIBlankEntry
  },
  {
    0x05,
    L"16850-compatible",
    PCIBlankEntry
  },
  {
    0x06,
    L"16950-compatible",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0701[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x01,
    L"Bi-directional",
    PCIBlankEntry
  },
  {
    0x02,
    L"ECP 1.X-compliant",
    PCIBlankEntry
  },
  {
    0x03,
    L"IEEE 1284",
    PCIBlankEntry
  },
  {
    0xfe,
    L"IEEE 1284 target (not a controller)",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0703[] = {
  {
    0x00,
    L"Generic",
    PCIBlankEntry
  },
  {
    0x01,
    L"Hayes-compatible 16450",
    PCIBlankEntry
  },
  {
    0x02,
    L"Hayes-compatible 16550",
    PCIBlankEntry
  },
  {
    0x03,
    L"Hayes-compatible 16650",
    PCIBlankEntry
  },
  {
    0x04,
    L"Hayes-compatible 16750",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0800[] = {
  {
    0x00,
    L"Generic 8259",
    PCIBlankEntry
  },
  {
    0x01,
    L"ISA",
    PCIBlankEntry
  },
  {
    0x02,
    L"EISA",
    PCIBlankEntry
  },
  {
    0x10,
    L"IO APIC",
    PCIBlankEntry
  },
  {
    0x20,
    L"IO(x) APIC interrupt controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0801[] = {
  {
    0x00,
    L"Generic 8237",
    PCIBlankEntry
  },
  {
    0x01,
    L"ISA",
    PCIBlankEntry
  },
  {
    0x02,
    L"EISA",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0802[] = {
  {
    0x00,
    L"Generic 8254",
    PCIBlankEntry
  },
  {
    0x01,
    L"ISA",
    PCIBlankEntry
  },
  {
    0x02,
    L"EISA",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0803[] = {
  {
    0x00,
    L"Generic",
    PCIBlankEntry
  },
  {
    0x01,
    L"ISA",
    PCIBlankEntry
  },
  {
    0x02,
    L"EISA",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0904[] = {
  {
    0x00,
    L"Generic",
    PCIBlankEntry
  },
  {
    0x10,
    L"",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0c00[] = {
  {
    0x00,
    L"",
    PCIBlankEntry
  },
  {
    0x10,
    L"Using 1394 OpenHCI spec",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0c03[] = {
  {
    0x00,
    L"UHCI",
    PCIBlankEntry
  },
  {
    0x10,
    L"OHCI",
    PCIBlankEntry
  },
  {
    0x20,
    L"EHCI",
    PCIBlankEntry
  },
  {
    0x30,
    L"xHCI",
    PCIBlankEntry
  },
  {
    0x80,
    L"No specific programming interface",
    PCIBlankEntry
  },
  {
    0xfe,
    L"(Not Host Controller)",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0c07[] = {
  {
    0x00,
    L"SMIC",
    PCIBlankEntry
  },
  {
    0x01,
    L"Keyboard Controller Style",
    PCIBlankEntry
  },
  {
    0x02,
    L"Block Transfer",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0d01[] = {
  {
    0x00,
    L"Consumer IR controller",
    PCIBlankEntry
  },
  {
    0x10,
    L"UWB Radio controller",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};

PCI_CLASS_ENTRY  PCIPIFClass_0e00[] = {
  {
    0x00,
    L"Message FIFO at offset 40h",
    PCIBlankEntry
  },
  {
    0x01,
    L"",
    PCIBlankEntry
  },
  {
    0x00,
    NULL,
    /* null string ends the list */ NULL
  }
};