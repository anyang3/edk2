/** @file
  This is a test application that demonstrates how to use the C-style entry point
  for a shell application.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <ShellDispPciApp.h>

EFI_STATUS 
PciDevicePresent (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo,
  OUT PCI_TYPE00                      *Pci,
  IN  UINT8                           Bus,
  IN  UINT8                           Device,
  IN  UINT8                           Func
  )
{

  EFI_STATUS  Status;
  UINTN      Address;
  // Create PCI address map
  Address = EFI_PCI_ADDRESS (Bus, Device, Func, 0);
  // Read the Vendor ID
  Status = PciRootBridgeIo->Pci.Read (
                PciRootBridgeIo,
                EfiPciWidthUint32,
                Address,
                1,
                Pci);
  if (!EFI_ERROR (Status) && (Pci->Hdr).VendorId != 0xffff) {
    // Read the entire config header for the device
    Status = PciRootBridgeIo->Pci.Read (
                PciRootBridgeIo,
                EfiPciWidthUint32,
                Address,
                sizeof(PCI_TYPE00) / sizeof(UINT32),
                Pci);
    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}

/**
  Generates printable Unicode strings that represent PCI device class,
  subclass and programmed I/F based on a value passed to the function.

  @param[in] ClassCode      Value representing the PCI "Class Code" register read from a PCI device. 
  @param[in, out] ClassStrings   Pointer of PCI_CLASS_STRINGS structure
**/
VOID
PciGetClassStrings (
  IN      UINT32             ClassCode,
  IN OUT  PCI_CLASS_STRINGS  *ClassStrings
  )
{
  INTN             Index;
  UINT8            Code;
  PCI_CLASS_ENTRY  *CurrentClass;

  //
  // Assume no strings found
  //
  ClassStrings->BaseClass = L"UNDEFINED";
  ClassStrings->SubClass  = L"UNDEFINED";
  ClassStrings->PIFClass  = L"UNDEFINED";

  CurrentClass = gClassStringList;
  Code         = (UINT8)(ClassCode >> 16);
  Index        = 0;

  while (Code != CurrentClass[Index].Code) {
    if (NULL == CurrentClass[Index].DescText) {
      return;
    }

    Index++;
  }

  ClassStrings->BaseClass = CurrentClass[Index].DescText;
  if (NULL == CurrentClass[Index].LowerLevelClass) {
    return;
  }

  CurrentClass = CurrentClass[Index].LowerLevelClass;
  Code         = (UINT8)(ClassCode >> 8);
  Index        = 0;

  while (Code != CurrentClass[Index].Code) {
    if (NULL == CurrentClass[Index].DescText) {
      return;
    }

    Index++;
  }

  ClassStrings->SubClass = CurrentClass[Index].DescText;
  if (NULL == CurrentClass[Index].LowerLevelClass) {
    return;
  }

  CurrentClass = CurrentClass[Index].LowerLevelClass;
  Code         = (UINT8)ClassCode;
  Index        = 0;

  while (Code != CurrentClass[Index].Code) {
    if (NULL == CurrentClass[Index].DescText) {
      return;
    }

    Index++;
  }

  ClassStrings->PIFClass = CurrentClass[Index].DescText;
  return;
}

/**
  Print strings that represent PCI device class, subclass and programmed I/F.

  @param[in] ClassCodePtr   Points to the memory which stores register Class Code in PCI configuration space.
  @param[in] IncludePIF     If the printed string should include the programming I/F part
**/
VOID
PciPrintClassCode (
  IN UINT8    *ClassCodePtr,
  IN BOOLEAN  IncludePIF
  )
{
  UINT32             ClassCode;
  PCI_CLASS_STRINGS  ClassStrings;

  ClassCode  = 0;
  ClassCode |= (UINT32)ClassCodePtr[0];
  ClassCode |= (UINT32)(ClassCodePtr[1] << 8);
  ClassCode |= (UINT32)(ClassCodePtr[2] << 16);

  PciGetClassStrings (ClassCode, &ClassStrings);

  if (IncludePIF) {
    //
    // Print base class, sub class, and programming inferface name
    //
    ShellPrintEx (
      -1,
      -1,
      L"%s - %s - %s\n",
      ClassStrings.BaseClass,
      ClassStrings.SubClass,
      ClassStrings.PIFClass
      );
  } else {
    //
    // Only print base class and sub class name
    //
    ShellPrintEx (
      -1,
      -1,
      L"%s - %s\n",
      ClassStrings.BaseClass,
      ClassStrings.SubClass
      );
  }
}

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

VOID
PciPrintCapList (
  IN UINT8 Bus,
  IN UINT8 Dev,
  IN UINT8 Func
  ) 
{
  UINT32 Value;
  UINT8 Index;
  PcieCfg Config;
  Config.IoCfg.EN = 1;
  Config.IoCfg.Bus = Bus;
  Config.IoCfg.Device = Dev;
  Config.IoCfg.Function = Func;
  Config.IoCfg.Register = 0x34;

  IoWrite32(0x0CF8, Config.Int32);
  Value = IoRead32(0x0CFC);
  Index = Value & 0x000000FF;

  do {
    // Print(L"Index:%d ", Index);
    Config.IoCfg.Register = Index;
    IoWrite32(0x0CF8, Config.Int32);
    Value = IoRead32(0x0CFC);
    Print(L"%02x-", (Value & 0x000000FF));
    Index = (Value & 0x0000FF00) >> 8;
  } while(Index != 0);

  // Print(L"Index:%d ", Index);
  Config.IoCfg.Register = Index;
  IoWrite32(0x0CF8, Config.Int32);
  Value = IoRead32(0x0CFC);
  Print(L"%02x", (Value & 0x000000FF));
}

/**
  UEFI application entry point which has an interface similar to a
  standard C main function.

  The ShellCEntryLib library instance wrappers the actual UEFI application
  entry point and calls this ShellAppMain function.

  @param[in] argCount     The number of items in Argv.
  @param[in] ppArgs     Array of pointers to strings.

  @retval  0               The application exited normally.
  @retval  Other           An error occurred.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN  argCount,
  IN CHAR16 **ppArgs
  )
{

  EFI_STATUS                        Status;
  UINTN                             Index;
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo = NULL;
  // EFI_PCI_IO_PROTOCOL               *PciIo = NULL;
  PCI_TYPE00                        Pci;
  UINT8                             Bus, Dev, Func;
  UINTN                             PciDeviceNumber = 0;

  DEBUG ((EFI_D_ERROR , "DEBUG test start!\n"));

  Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer);

  if(EFI_ERROR(Status)) {
    Print(L"Call LocatePciRootBridgeIo failed, Can't find protocol!\n");
    return FALSE;
  }

  for(Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol(
                HandleBuffer[Index],
                &gEfiPciRootBridgeIoProtocolGuid,
                (VOID **)&PciRootBridgeIo);

    if(!EFI_ERROR(Status)) {
      Print(L"DevNo | Bus Dev Func | Vendor Device Class CapList | PciDevName\n");
      for(Bus = 0; Bus < PCI_MAX_BUS; Bus++) {
        for(Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
          for(Func = 0; Func <= PCI_MAX_FUNC; Func++) {
            Status = PciDevicePresent(PciRootBridgeIo, &Pci, Bus, Dev, Func);
            if(Status == EFI_SUCCESS) {
              PciDeviceNumber++;
              // Print PciDevNo, Bus, Dev, Func
              Print(L"%-7d %-03x %-03x %-06x ", PciDeviceNumber, Bus, Dev, Func);
              // Print Vendor, Device, ClassCode
              Print(L"%-06x %-06x %-05x ", Pci.Hdr.VendorId, Pci.Hdr.DeviceId, Pci.Hdr.ClassCode[0]);
              // Print Capability List
              if(((Pci.Hdr.Status >> 4) & 1) == 1) {
                // Capability List is enabled
                PciPrintCapList(Bus, Dev, Func);
              } else {
                Print(L"xxxxxxxx");
              }
              // Print PciDevName
              Print(L"  ");
              PciPrintClassCode(Pci.Hdr.ClassCode, FALSE);
            }
          }
        }
      }
    }
  }

  DEBUG ((EFI_D_ERROR ,"DEBUG test end!\n"));

  return EFI_SUCCESS;
}

