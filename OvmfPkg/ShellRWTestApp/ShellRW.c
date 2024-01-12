
#include<ShellRWTestApp.h>


/*
  Read data from the given IO port.
  @param[in]  port  The I/O port to read.
  @return  EFI_SUCCESS The value was read from the I/O port.
*/
EFI_STATUS
EFIAPI
ReadIo (
  IN UINTN Port
  )
{  
  Print (L"Read from the IO port %08x success. Value: %02x\n", 
      Port, 
      IoRead8(Port));
  return EFI_SUCCESS;
}

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
  )
{
  Print (L"Write to the IO port %08x success. Value: %02x\n", 
      port, 
      IoWrite8(port, value));
  return EFI_SUCCESS;
}

/*
  Read data from the given Memory address.
  @param[in]  addr  The Memory address to read.
  @return  EFI_SUCCESS The value was read from the Memory address.
*/
EFI_STATUS
EFIAPI
ReadMem (
  IN UINTN addr
  )
{  
  UINTN *index;
  index = (UINTN *)addr;
  Print (L"Read from the Mem address 0x%08x%08x success. Value: 0x%08x%08x\n", 
      addr >> 32, addr & 0xFFFFFFFF, 
      (*index) >> 32, (*index) & 0xFFFFFFFF);
  return EFI_SUCCESS;
}

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
  )
{
  volatile UINTN *index;
  index = (UINTN *)addr;
  *index = value;
  asm volatile("" ::: "memory");
  Print (L"Write to the Mem address 0x%08x%08x success. Value: 0x%08x%08x\n", 
      addr >> 32, addr & 0xFFFFFFFF, 
      (*index) >> 32, (*index) & 0xFFFFFFFF);
  return EFI_SUCCESS;
}

/*
  Read data from the given CMOS offset.
  @param[in]  offset  The CMOS offset to read.
  @return  EFI_SUCCESS The value was read from the CMOS offset.
*/
EFI_STATUS
EFIAPI
ReadCmos (
  IN UINT8 offset
  )
{
  IoWrite8(0x70, offset);
  Print (L"Read from the CMOS offset %02x success. Value: %02x\n", 
      offset, 
      IoRead8(0x71));
  return EFI_SUCCESS;
}

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
  )
{
  IoWrite8(0x70, offset);
  IoWrite8(0x71, value);
  Print (L"Write to the CMOS offset %02x success. Value: %02x\n", 
      offset, 
      IoRead8(0x71));
  return EFI_SUCCESS;
}

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
  )
{
  UINT8 i;
  UINT32 value;
  PcieCfg config = {
    .IoCfg.EN = 1,
    .IoCfg.Bus = bus,
    .IoCfg.Device = dev,
    .IoCfg.Function = func
  };

  for(i = 0; i < len; i += 4)
  {
    config.IoCfg.Register = i;
    IoWrite32(0x0CF8, config.Int32);
    value = IoRead32(0x0CFC);
    Print(L"%02x %02x %02x %02x  -  %08x\n",
      value & 0x000000FF,
      (value & 0x0000FF00) >> 8,
      (value & 0x00FF0000) >> 16,
      value >> 24,
      config.Int32);
  }
  return EFI_SUCCESS;
}

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
) 
{
  UINT8 i;
  UINT32 value;
  PcieCfg config = {
    .MmioCfg.Base = base,  // E
    .MmioCfg.Bus = bus,
    .MmioCfg.Device = dev,  // 2->1
    .MmioCfg.Function = func
  };

  for(i = 0; i < len; i += 4)
  {
    config.MmioCfg.Register = i;
    value = MmioRead32(config.Int32);
    Print(L"%02x %02x %02x %02x  -  %08x\n",
      value & 0x000000FF,
      (value & 0x0000FF00) >> 8,
      (value & 0x00FF0000) >> 16,
      value >> 24,
      config.Int32);
  }

  return EFI_SUCCESS;
}