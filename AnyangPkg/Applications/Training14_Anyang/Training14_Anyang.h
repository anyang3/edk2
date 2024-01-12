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
#include <Register/Intel/ArchitecturalMsr.h>
#include <FlashPart.h>
#include <SpiFlash.h>
#include <Library/PciLib.h>
#include <Library/PciSegmentLib.h>

#define FLASH_VALID_SIGNATURE           0x0FF0A55A
#define DEFAULT_PCI_SEGMENT_NUMBER_PCH  0        // The default PCH PCI segment
#define DEFAULT_PCI_BUS_NUMBER_PCH      0        // The default PCH PCI bus number
#define PCI_DEVICE_NUMBER_PCH_SPI       31       // SPI Controller (D31:F5)
#define PCI_FUNCTION_NUMBER_PCH_SPI     5        // SPI Controller (D31:F5)
#define R_SPI_CFG_BAR0                  0x10     // SPI_BAR0 Offset
#define B_SPI_CFG_BAR0_MASK             0x0FFF
#define R_SPI_MEM_HSFSC                 0x04     // Hardware Sequencing Flash Status and Control Register(32bits)
#define B_SPI_MEM_HSFSC_FDV             BIT14    // Flash Descriptor Valid, once valid software can use hareware sequencing regs
#define SPI_CFG_BIOS_CONTROL_REG        0xDC     // SPI_BIOS_CONTROL Register 
#define SPI_CFG_BIOS_CONTROL_EISS_BIT   0x20     // Bit-5: Enable InSMM.STS (EISS), This bit is locked by LE
#define SPI_CFG_BIOS_CONTROL_LE_BIT     0x02     // Bit-1: Lock Enable (LE), Once set to 1, this bit can only be cleared by PLTRST#
#define SPI_CFG_BIOS_CONTROL_WPD_BIT    0x1      // Bit-0: Write Protect Disable(WPD)
#define SW_SMI_IO_ADDRESS               0xB2     
#define BIOS_CONTROL_SMI                0x66     // The SW SMI number of Bios Control
