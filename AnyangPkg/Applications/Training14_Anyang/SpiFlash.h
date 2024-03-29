//***********************************************************************
//*                                                                     *
//*                      Copyright (c) 1985-2021, AMI.                  *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file SpiFlash.h
    This file contains the structure and function prototypes needed
    for the Flash driver

**/

#ifndef _SPIFlash_H_
#define _SPIFlash_H_

#include <FlashPart.h>

BOOLEAN
IsTopSwapOn(
    VOID
);

VOID
SpiFlashLock(
    UINT16  MfgDevId,
    BOOLEAN Lock
);

//-------------------------------------------------------------------
// Supported SPI devices
// MFG and Device code
#define SST_25LF040A                0x44BF
#define SST_25LF040                 0x40BF
#define SST_25LF080A                0x80BF

#define SST_25VF040B                0x8DBF
#define SST_25VF080B                0x8EBF
#define SST_25VF016B                0x41BF
#define SST_25VF032B                0x4ABF
#define SST_25VFxxx_ID              0x25BF
#define SST_26VFxxx_ID              0x26BF

//PMC 25LVxxx Device ID/Manufacturer ID
#define PMC_25LV040                 0x7E9D
#define PMC_25LVxxx_ID              0x9D7F

#define ATMEL_26DF041               0x441F
#define ATMEL_26DF081               0x451F
#define ATMEL_26DF161               0x461F
#define ATMEL_26DF321               0x471F  // ATMEL 26DF321 32Mbit
#define ATMEL_25DF641               0x481F  // ATMEL 25DF641 64Mbit
#define ATMEL_25DQ161               0x861F
#define ADESTO_AT25SF641            0x321F
#define ADESTO_AT25SL128A           0x421F
#define ADESTO_AT25SL641            0x431F

//Winbond W25Xxx Device ID/Manufacturer ID
#define WINBOND_W25X10              0x10EF
#define WINBOND_W25X20              0x11EF
#define WINBOND_W25X40              0x12EF
#define WINBOND_W25X80              0x13EF
#define WINBOND_W25X16              0x14EF
#define WINBOND_25Xxx_ID            0x30EF
#define WINBOND_25Qxx_ID            0x40EF
#define WINBOND_25QxxW_ID           0x50EF
#define WINBOND_25QxxF_ID           0x60EF
#define WINBOND_25QxxJ_DTR_ID       0x70EF
#define WINBOND_25MxxJ_ID           0x71EF
#define WINBOND_25QxxJ_IM_ID        0x80EF
#define WINBOND_77QxxJ_ID           0x8AEF

// MXIC Device ID/Manufacturer ID
#define MX25L4005_DEVICE_ID         0x13C2
#define MX25L8005_DEVICE_ID         0x14C2
#define MX25L1605_DEVICE_ID         0x15C2
#define MX25L3205_DEVICE_ID         0x16C2
#define MX25L6405_DEVICE_ID         0x17C2
#define MXIC_25Lxxxx_ID             0x20C2
#define MXIC_25Lxx35_ID             0x24C2
#define MXIC_25Lxx36_ID             0x5EC2
#define MXIC_25Uxx35_ID             0x25C2
#define MXIC_25Rxx35_ID             0x28C2
#define MXIC_77Lxx50F_ID            0x75C2  //MX77L25650F

// Spansion S25FL008A Device ID/Manufacturer ID
#define SPANSION_25FL004A           0x1201
#define SPANSION_25FL008A           0x1301
#define SPANSION_25FLxxx_ID         0x0201
#define SPANSION_25FLxxxP_ID        0x2001
#define SPANSION_25FL1xxK_ID        0x4001      // S25FL132K / S25FL164K
#define SPANSION_25FLxxL_ID         0x6001      // S25FL064L

// ESMT F25L00xA Device ID/Manufacturer ID
#define ESMT_25L004A                0x128C
#define ESMT_25L008A                0x138C
#define ESMT_25L016A                0x148C
#define ESMT_25L032A                0x158C
#define ESMT_25LxxxT_ID             0x208C
#define ESMT_25LxxxB_ID             0x218C
#define ESMT_25Lxx_ID               0x408C
#define ESMT_25LxxQ_ID              0x418C

// ST Micro/Micron/Numonyx M25Pxx Device ID/Manufacturer ID
#define STM_M25P40                  0x1320
#define STM_M25P80                  0x1420
#define STM_M25P32                  0x2020
#define STM_25PExx_ID               0x8020
#define STM_25PXxx_ID               0x7120      // M25PX16/32
#define STM_25PFxx_ID               0x7020      // M25PF32
#define STM_25Pxx_ID                0x2020      // M25P16/32/64
#define Numonyx_25Qxx_ID            0xBA20      // N25Q16/32/64/128
#define Numonyx_25QxxA_ID           0xBB20      // N25Q16/32/64/128
#define XMC_25QUxx_ID               0x3820      // XMC25QU64A
#define XMC_25QHxxB_ID              0x4020      // XM25QH32B
#define XMC_25QHxx_ID               0x6020      // XM25QHxxx
#define XMC_25QUxxB_ID              0x5020      // XMC_XM25QUxxxB
#define XMC_25QUxxC_ID              0x4120      // XMC_XM25QUxxxC
#define XMC_25RHxxC_ID              0x4320      // XM25RH
#define XMC_25RUxxC_ID              0x4420      // XM25RU

// AMIC 25Lxx Device ID/Manufacturer ID
#define AMIC_25Lxx_ID               0x377F
#define AMIC_25Lxxx_ID              0x3037
#define AMIC_25LQxxx_ID             0x4037

// EON  25P/Fxx Device ID/Manufacturer ID
#define EON_25Fxx_ID                0x311C
#define EON_25Pxx_ID                0x201C
#define EON_25Qxx_ID                0x301C
#define EON_25Sxx_ID                0x381C
#define EON_25QHxx_ID               0x701C
#define EON_25SXxx_ID               0x781C

// INTEL 25Fxxx Device ID/Manufacturer ID
#define INTEL_25Fxxx_ID             0x8989
#define INTEL_25Fxxx_ID             0x8989

// Giga Device GD25Qxx Device ID/Manufacturer ID
#define GD_25Qxx_ID                 0x40C8
#define GD_25LQxx_ID                0x60C8
#define GD_25LBxx_ID                0x67C8
#define GD_25Bxx_ID                 0x47C8      // GD25B512ME
#define GD_55WRxx_ID                0x65C8      // GD55WR512M

// FIDELIX Device FM_25Qxx Device ID/Manufacturer ID
#define FM_25Qxx_ID                 0x32F8
#define FM_25Mxx_ID                 0x43F8

// Shanghai Fudan Microelectronics Device ID/Manufacturer ID
#define SHFM_25Qxx_ID               0x40A1  //FM_25Qxx
#define SHFM_25Wxx_ID               0x28A1  //FM_25Wxx

// ISSI IC25LP064A/B Device ID/Manufacturer ID
#define ISSI_25LPxx_ID              0x609D
#define ISSI_25WPxx_ID              0x709D

//XTX XT25F64B Device ID/Manufacturer ID
#define XT_25Fxx_ID                 0x400B
#define XT_25Qxx_ID                 0x600B

//-------------------------------------------------------------------
//  NOTE: Assuming that 8Mbit flash will only contain a 4Mbit binary.
//        Treating 4Mbit and 8Mbit devices the same.

//-------------------------------------------------------------------
// BIOS Base Address
#define BIOS_BASE_ADDRESS_4M 0xfff80000
#define BIOS_BASE_ADDRESS_8M 0xfff00000

//-------------------------------------------------------------------
// Flash commands

// Command for Read the Serial Flash Discoverable Paramenters (SFDP)
#define SPI_READ_SFDP_COMMAND             0x5A

// Commands for SST 25LFxx and 25VFxx parts
#define SPI_SST25LF_COMMAND_WRITE         0x02
#define SPI_SST25LF_COMMAND_READ          0x03
#if defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 0)
#define SPI_SST25VF_COMMAND_READ          0x03        // Read Byte
#elif defined(FAST_READ_SUPPORT)&& (FAST_READ_SUPPORT == 1)
#define SPI_SST25VF_COMMAND_READ          0x0B        // Fast Read
#elif defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 2)
#define SPI_SST25VF_COMMAND_READ          0x3B        // Dual Output Fast Read
#endif
#define SPI_SST25LF_COMMAND_ERASE         0x20
#define SPI_SST25LF_COMMAND_ERASE_64K     0xD8
#define SPI_SST25LF_COMMAND_WRITE_DISABLE 0x04
#define SPI_SST25LF_COMMAND_READ_STATUS   0x05
#define SPI_SST25LF_COMMAND_WRITE_ENABLE  0x06
//#define SPI_SST25LF_COMMAND_READ_ID       0xAB
#define SPI_SST25LF_COMMAND_READ_ID       0x90
#define SPI_SST25VF_COMMAND_READ_ID       0x9F
#define SPI_SST25LF_COMMAND_WRITE_S_EN    0x50
#define SPI_SST25LF_COMMAND_WRITE_S       0x01

// PMC commands
#define SPI_PMC25LV_COMMAND_WRITE         0x02
#define SPI_PMC25LV_COMMAND_READ          0x03
#define SPI_PMC25LV_COMMAND_ERASE         0xD7
#define SPI_PMC25LV_COMMAND_WRITE_DISABLE 0x04
#define SPI_PMC25LV_COMMAND_READ_STATUS   0x05
#define SPI_PMC25LV_COMMAND_WRITE_ENABLE  0x06
#define SPI_PMC25LV_COMMAND_READ_ID       0xAB
#define SPI_PMC25LV_COMMAND_WRITE_S_EN    0x06
#define SPI_PMC25LV_COMMAND_WRITE_S       0x01

// Atmel commands
#define SPI_AT26DF_COMMAND_WRITE         0x02
#if defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 0)
#define SPI_AT26DF_COMMAND_READ          0x03        // Read Byte
#elif defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 1)
#define SPI_AT26DF_COMMAND_READ          0x0B        // Fast Read
#elif defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 2)
#define SPI_AT26DF_COMMAND_READ          0x3B        // Dual Output Fast Read
#endif
#define SPI_AT26DF_COMMAND_ERASE         0x20
#define SPI_AT26DF_COMMAND_ERASE_64K     0xD8
#define SPI_AT26DF_COMMAND_WRITE_DISABLE 0x04
#define SPI_AT26DF_COMMAND_READ_STATUS   0x05
#define SPI_AT26DF_COMMAND_WRITE_ENABLE  0x06
#define SPI_AT26DF_COMMAND_READ_ID       0x9F
#define SPI_AT26DF_COMMAND_WRITE_S_EN    0x00
#define SPI_AT26DF_COMMAND_WRITE_S       0x01
// ATMEL 26DF321 32Mbit SPI Flash part support
#define SPI_AT26DF_COMMAND_PROTECT       0x36
#define SPI_AT26DF_COMMAND_UNPROTECT     0x39

// Winbond W25Xxx serial flash commands
#define SPI_WIN_W25X_COMMAND_WRITE         0x02     // page program
#define SPI_WIN_W25X_COMMAND_READ          0x03     // Read Byte
#define SPI_WIN_W25X_COMMAND_ERASE         0x20     // erase 4K sectors
#define SPI_WIN_W25X_COMMAND_WRITE_DISABLE 0x04     // Write Disable
#define SPI_WIN_W25X_COMMAND_READ_STATUS   0x05     // read Status Register
#define SPI_WIN_W25X_COMMAND_WRITE_ENABLE  0x06     // Write Enable
#define SPI_WIN_W25X_COMMAND_READ_ID       0x90     // Manufacturer/Device ID
#define SPI_WIN_W25X_COMMAND_WRITE_S       0x01     // Write Status register

// MXIC MX25Lx005 commands
#define SPI_MXIC_M25L_COMMAND_WRITE             0x02        // page program
#define SPI_MXIC_M25L_COMMAND_READ              0x03        // Read Byte
#define SPI_MXIC_M25L_COMMAND_ERASE             0x20        // erase 4K sectors
#define SPI_MXIC_M25L_COMMAND_WRITE_DISABLE     0x04        // Write Disable
#define SPI_MXIC_M25L_COMMAND_READ_STATUS       0x05        // read Status Register
#define SPI_MXIC_M25L_COMMAND_WRITE_ENABLE      0x06        // Write Enable
#define SPI_MXIC_M25L_COMMAND_READ_ID           0x9F        // Manufacturer/Device ID
#define SPI_MXIC_M25L_COMMAND_WRITE_S           0x01        // Write Status register

// Spansion S25FL00xA commands
#define SPI_S25FL00xA_COMMAND_WRITE             0x02        // page program
#define SPI_S25FL00xA_COMMAND_READ              0x03        // Read Byte
#define SPI_S25FL00xA_COMMAND_ERASE             0xD8        // erase 64K sectors
#define SPI_S25FL00xA_COMMAND_WRITE_DISABLE     0x04        // Write Disable
#define SPI_S25FL00xA_COMMAND_READ_STATUS       0x05        // read Status Register
#define SPI_S25FL00xA_COMMAND_WRITE_ENABLE      0x06        // Write Enable
#define SPI_S25FL00xA_COMMAND_READ_ID           0x9F        // Manufacturer/Device ID
#define SPI_S25FL00xA_COMMAND_WRITE_S           0x01        // Write Status register

// ESMT F25L00xA commands
#define SPI_ESMT_F25L00x_COMMAND_WRITE          0x02        // page program
#define SPI_ESMT_F25L00x_COMMAND_READ           0x03        // Read Byte
#define SPI_ESMT_F25L00x_COMMAND_ERASE          0x20        // erase 4K sectors
#define SPI_ESMT_F25L00x_COMMAND_WRITE_DISABLE  0x04        // Write Disable
#define SPI_ESMT_F25L00x_COMMAND_READ_STATUS    0x05        // read Status Register
#define SPI_ESMT_F25L00x_COMMAND_WRITE_ENABLE   0x06        // Write Enable
#define SPI_ESMT_F25L00x_COMMAND_READ_ID        0x90        // Manufacturer/Device ID
#define SPI_ESMT_F25L00x_COMMAND_WRITE_S_EN     0x50        // Write Status Enable register
#define SPI_ESMT_F25L00x_COMMAND_WRITE_S        0x01        // Write Status register

// ST Micro M25Pxx commands
#define SPI_M25Pxx_COMMAND_WRITE            0x02        // page program
#define SPI_M25Pxx_COMMAND_READ             0x03        // Read Byte
#define SPI_M25Pxx_COMMAND_ERASE            0xD8        // erase 64K sectors
#define SPI_M25Pxx_COMMAND_WRITE_DISABLE    0x04        // Write Disable
#define SPI_M25Pxx_COMMAND_READ_STATUS      0x05        // read Status Register
#define SPI_M25Pxx_COMMAND_WRITE_ENABLE     0x06        // Write Enable
#define SPI_M25Pxx_COMMAND_READ_ID          0x9F        // Manufacturer/Device ID
#define SPI_M25Pxx_COMMAND_WRITE_S          0x01        // Write Status register

// ST Micro M25Pxx commands
#define SPI_M25PExx_COMMAND_WRITE           0x02        // page program
#if defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 0)
#define SPI_M25PExx_COMMAND_READ            0x03        // Read Byte
#elif defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 1)
#define SPI_M25PExx_COMMAND_READ            0x0B        // Fast Read
#elif defined(FAST_READ_SUPPORT) && (FAST_READ_SUPPORT == 2)
#define SPI_M25PExx_COMMAND_READ            0x3B        //  Dual Output Fast Read
#endif
#define SPI_M25PExx_COMMAND_ERASE_4K        0x20
#define SPI_M25PExx_COMMAND_ERASE           0xD8        // erase 64K sectors
#define SPI_M25PExx_COMMAND_PAGE_ERASE      0xDB        // erase 256 byte pages
#define SPI_M25PExx_COMMAND_WRITE_DISABLE   0x04        // Write Disable
#define SPI_M25PExx_COMMAND_READ_STATUS     0x05        // read Status Register
#define SPI_M25PExx_COMMAND_WRITE_ENABLE    0x06        // Write Enable
#define SPI_M25PExx_COMMAND_READ_ID         0x9F        // Manufacturer/Device ID
#define SPI_M25PExx_COMMAND_WRITE_S         0x01        // Write Status register

#define SPI_INVALID_COMMAND                 0xFF

//#define SECTOR_SIZE_4KB   0x1000  // Common 4kBytes sector size
//#define SECTOR_SIZE_64KB  0x10000  // Common 64kBytes sector size
#define BLOCK_SIZE_64KB     0x00010000  // Common 64kBytes block size
#define MAX_FWH_SIZE        0x00100000  // 8Mbit (Note that this can also be used for the 4Mbit )

extern const UINT8 SpiMaxTransfer;

// SPI default opcode slots
extern const UINT8 SpiOpcodeWriteIndex;
extern const UINT8 SpiOpcodeReadIndex;
extern const UINT8 SpiOpcodeEraseIndex;
extern const UINT8 SpiOpcodeWriteStatusIndex;

#endif

//Intel Flash Region Definitions
//Region usage will be defined in PCH SPI programming guide
typedef
enum
{
    FlashRegionDescriptor = 0,
    FlashRegionBios,
    FlashRegionMe, //ME, CSME, SPS or IFWI
    FlashRegionGbE,
    FlashRegionPlatformData,
    FlashRegion5, //Device expansion 1
    FlashRegion2ndBios,
    FlashRegion7,
    FlashRegionEc, //EC or BMC
    FlashRegion9, //Device expansion 2
    FlashRegionA, //IE
    FlashRegion10GbE,
    FlashRegionC,
    FlashRegionD,
    FlashRegionE,
    FlashRegionF,
    FlashRegionMax
}FLASH_REGION_TYPE;

//Intel Flash Region Operation
typedef
enum
{
    FlashRegionRead = 0,
    FlashRegionErase,
    FlashRegionWrite
}FLASH_OPERATION;

#pragma pack(push, 1)
/**
    This structure contains variables that define the detals and command
    set of flash part.
**/
typedef
struct _EX_FLASH_PART
{
    FLASH_INFO  FlashCommandMenu;
    UINT32      FlashCapacity;
    UINT32      FlashVenDevId;
    UINT8       AAIWordProgram;
    UINT8       GlobalBlockUnlock;
    UINT8       Reserved[2];
}EX_FLASH_PART;

#pragma pack(pop)