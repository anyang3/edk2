//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file FlashPart.h
    This file contains the structure and function prototypes needed
    for the Flash driver

**/

#ifndef __FLASH_H_
#define __FLASH_H_
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

//-------------------------------------------------------------------
// block and sector sizes
#define SECTOR_SIZE_256B    0x100       // 4kBytes sector size
#define SECTOR_SIZE_4KB     0x1000      // 4kBytes sector size
#define SECTOR_SIZE_8KB     0x2000      // 8kbytes sector size
#define SECTOR_SIZE_32KB    0x8000      // 32Kbytes block size
#define SECTOR_SIZE_64KB    0x10000     // 64kBytes sector size
#define SECTOR_SIZE_128KB   0x20000     // 128kBytes sector size
#define SECTOR_SIZE_256KB   0x40000     // 256kBytes sector size
#define SECTOR_SIZE_512KB   0x80000     // 512kBytes sector size

//============================================================================
// Flash part module function prototypes

typedef
VOID
(EFIAPI *FLASH_READ_COMMAND)(
    volatile UINT8  *ByteAddress,
    UINT8           *Byte,
    UINT32          *Length,
    BOOLEAN         IsMappedAddress
);

typedef
BOOLEAN
(EFIAPI *FLASH_ERASE_COMMAND)(
    volatile UINT8  *BlockAddress
);

typedef
EFI_STATUS
(EFIAPI *FLASH_ERASE_COMMAND_EX)(
    volatile UINT8  *BlockAddress,
    UINT32          Length,
    BOOLEAN         IsMappedAddress
);

typedef
BOOLEAN
(EFIAPI *FLASH_PROGRAM_COMMAND)(
    volatile UINT8  *ByteAddress,
    UINT8           *Byte,
    UINT32          *Length,
    BOOLEAN         IsMappedAddress
);

typedef
VOID
(EFIAPI *FLASH_BLOCK_WRITE_ENABLE)(
    volatile UINT8  *BlockAddress,
    UINT32          Length,
    BOOLEAN         IsMappedAddress
);

typedef
VOID
(EFIAPI *FLASH_BLOCK_WRITE_DISABLE)(
    volatile UINT8  *BlockAddress,
    UINT32          Length,
    BOOLEAN         IsMappedAddress
    
);

typedef
VOID
(EFIAPI *FLASH_DEVICE_WRITE_ENABLE)(
    VOID
);

typedef
VOID
(EFIAPI *FLASH_DEVICE_WRITE_DISABLE)(
    VOID
);

typedef
BOOLEAN
(EFIAPI *FLASH_GET_ERASE_GRANULARITY)(
    UINT32  **Granularity,
    UINT32  *NumberOfEntries
);

typedef
VOID
(EFIAPI *FLASH_VIRTUAL_FIXUP)(
    EFI_RUNTIME_SERVICES *Rs
);

//============================================================================
// Flash Part Structure Types
/**
    This structure contains data that describe the different commands for
    the flash parts
**/
typedef
struct _OPCODE_CMD
{
    UINT8   Opcode;
    UINT8   OpcodeType;
}OPCODE_CMD;

/**
    This structure contains data that describe flash part information that
    is needed for each family of parts for the interface to work properly
**/
typedef
struct _FLASH_INFO
{
    OPCODE_CMD  Write;
    OPCODE_CMD  Read;
    OPCODE_CMD  Erase;
    OPCODE_CMD  Erase64K;
    OPCODE_CMD  ReadStatus;
    OPCODE_CMD  ReadId;
    OPCODE_CMD  WriteStatus;
    OPCODE_CMD  WriteStatusEnable;
    OPCODE_CMD  WriteEnable;
    UINT32      PageSize;
    UINT32      SectorSize;
}FLASH_INFO;

/**
    This structure contains variables that define the data to be read from
    or written to the flash part
**/
typedef
struct _FLASH_WRITE_DATA
{
    BOOLEAN Data;
    UINT32  Length;
    UINT8   *Values;
}FLASH_WRITE_DATA;

/**
    This structure defines the generic interface for the flash part
**/
typedef
struct _FLASH_PART
{
    FLASH_READ_COMMAND          FlashReadCommand;
    FLASH_ERASE_COMMAND         FlashEraseCommand;
    FLASH_ERASE_COMMAND_EX      FlashEraseCommandEx;
    FLASH_PROGRAM_COMMAND       FlashProgramCommand;
    FLASH_BLOCK_WRITE_ENABLE    FlashBlockWriteEnable;
    FLASH_BLOCK_WRITE_DISABLE   FlashBlockWriteDisable;
    FLASH_DEVICE_WRITE_ENABLE   FlashDeviceWriteEnable;
    FLASH_DEVICE_WRITE_DISABLE  FlashDeviceWriteDisable;
    FLASH_GET_ERASE_GRANULARITY FlashGetEraseGranularity;
    FLASH_VIRTUAL_FIXUP         FlashVirtualFixup;
    UINT32                      FlashProgramSize;
    UINT32                      FlashSectorSize;
    UINT8                       *FlashPartNumber;
}FLASH_PART;

#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif