#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>

/* 
  PCIe Address By I/O or MMIO
*/
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

/*
  Read data from the given IO port.
  @param[in]  port  The I/O port to read.
  @return  EFI_SUCCESS The value was read from the I/O port.
*/
EFI_STATUS
EFIAPI
ReadIo (
  IN UINTN Port
  );

/*
  Write data to the given IO port.
  @param[in]  port  The I/O port to write.
  @param[in]  value The value to write to the I/O port.
  @return  EFI_SUCCESS The value was written to the I/O port.
*/
EFI_STATUS
EFIAPI
WriteIo (
  IN UINTN port,
  IN UINT8 value
  );

/*
  Read data from the given Memory address.
  @param[in]  addr  The Memory address to read.
  @return  EFI_SUCCESS The value was read from the Memory address.
*/
EFI_STATUS
EFIAPI
ReadMem (
  IN UINTN addr
  );

/*
  Write data to the given Memory address.
  @param[in]  addr  The Memory address to write.
  @param[in]  value The value to write to the Memory address.
  @return  EFI_SUCCESS The value was written to the Memory address.
*/
EFI_STATUS
EFIAPI
WriteMem (
  IN UINTN addr,
  IN UINTN value
  );

/*
  Read data from the given CMOS offset.
  @param[in]  offset  The CMOS offset to read.
  @return  EFI_SUCCESS The value was read from the CMOS offset.
*/
EFI_STATUS
EFIAPI
ReadCmos (
  IN UINT8 offset
  );

/*
  Write data to the given CMOS offset.
  @param[in]  offset  The CMOS offset to write.
  @param[in]  value The value to write to the CMOS offset.
  @return  EFI_SUCCESS The value was written to the CMOS offset.
*/
EFI_STATUS
EFIAPI
WriteCmos (
  IN UINT8 offset,
  IN UINT8 value
  );

/*
  Read data from the given PCIe address by I/O.
  @param[in]  addr  The PCIe address by I/O to read.
  @param[in]  len  The count of data to read.
  @return  EFI_SUCCESS The value was read from the PCIe address by I/O.
*/
EFI_STATUS
EFIAPI
ReadPcieByIo (
  IN UINT8 bus,
  IN UINT8 dev,
  IN UINT8 func,
  UINT8 len
  );

/*
  Read data from the given PCIe address by MMIO.
  @param[in]  addr  The PCIe address by MMIO to read.
  @param[in]  len  The count of data to read.
  @return  EFI_SUCCESS The value was read from the PCIe address by MMIO.
*/
EFI_STATUS
EFIAPI
ReadPcieByMmio (
  IN UINT8 base, 
  IN UINT8 bus, 
  IN UINT8 dev, 
  IN UINT8 func,
  UINT8 len
);