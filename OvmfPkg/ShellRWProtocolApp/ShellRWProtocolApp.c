/** @file
  This is a test application that demonstrates how to use the C-style entry point
  for a shell application.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/ShellRWProtocol.h>
#include <Protocol/Smbios.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>

/*
  Print out help information.
  @param[in]  None.
  @return     None
*/
VOID
PrintHelpInfo (
  VOID
  )
{
  Print(L"\n");
  Print(L"Usage: ReadWriteTest [-?]\n");
  Print(L"  -?: Print help info.\n");
  Print(L"  -i: \n");
  Print(L"      -r [port]: Read data from the I/O port.\n");
  Print(L"      -w [port] [value]: Write data to the I/O port.\n");
  Print(L"  -m: \n");
  Print(L"      -r [addr]: Read data from the memory address.\n");
  Print(L"      -w [addr] [value]: Write data to the memory address.\n");
  Print(L"  -c: CMOS.\n");
  Print(L"      -r [offset]: Read data from the cmos offset.\n");
  Print(L"      -w [offset] [value]: Write data to the cmos offset.\n");
  Print(L"  -p: PCI.\n");
  Print(L"      -i [addr][len]: Read data from the pcie addr by I/O.\n");
  Print(L"      -m [addr][len]: Read data from the pcie addr by MMIO.\n");
  Print(L"\n");
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

  UINTN                   ArgIndex = 1;
  BOOLEAN                 PrintHelp = FALSE;
  EFI_STATUS              Status;
  EFI_SHELL_RW_PROTOCOL   *myProtocol = NULL;

  DEBUG ((EFI_D_ERROR , "DEBUG test start!\n"));
  
  Status = gBS->LocateProtocol (
                    &gEfiShellRWProtocolGuid,
                    NULL,
                    (VOID **)&myProtocol
                    );

  if(EFI_ERROR(Status)) {
    return FALSE;
  }

  myProtocol->PrintVersion(myProtocol);

  while (ArgIndex < argCount) {
    if (ppArgs[ArgIndex] != NULL) {
      if (StrCmp(L"-?", ppArgs[ArgIndex]) == 0) {
        PrintHelp = TRUE;
        break;
      } else if (StrCmp(L"-i", ppArgs[ArgIndex]) == 0) {
        ArgIndex++;
        if (ArgIndex != argCount && StrCmp(L"-r", ppArgs[ArgIndex]) == 0) {
          // Read data from the I/O port.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINTN port = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"port: %08x\n", port);
            myProtocol->ReadIo(myProtocol, port);
          }
        } else if (ArgIndex != argCount && StrCmp(L"-w", ppArgs[ArgIndex]) == 0) {
          // Write data to the I/O port.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINTN port = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"port: %08x\n", port);
            ArgIndex++;
            if (ArgIndex == argCount) {
              PrintHelp = TRUE;
              break;
            } else {
              UINT8 value = (UINT8)StrHexToUintn(ppArgs[ArgIndex]);
              Print(L"value: %02x\n", value);
              myProtocol->WriteIo(myProtocol, port, value);
            }
          }
        } else {
          PrintHelp = TRUE;
          break;
        }
      } else if (StrCmp(L"-m", ppArgs[ArgIndex]) == 0) {
        ArgIndex++;
        if (ArgIndex != argCount && StrCmp(L"-r", ppArgs[ArgIndex]) == 0) {
          // Read data from the memory address.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINTN addr = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"addr: 0x%08x%08x\n", addr >> 32, addr & 0xFFFFFFFF);
            myProtocol->ReadMem(myProtocol, addr);
          }
        } else if (ArgIndex != argCount && StrCmp(L"-w", ppArgs[ArgIndex]) == 0) {
          // Write data to the memory address.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINTN addr = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"addr: 0x%08x%08x\n", addr >> 32, addr & 0xFFFFFFFF);
            ArgIndex++;
            if (ArgIndex == argCount) {
              PrintHelp = TRUE;
              break;
            } else {
              UINTN value = StrHexToUintn(ppArgs[ArgIndex]);
              Print(L"value: 0x%08x%08x\n", value >> 32, value & 0xFFFFFFFF);
              myProtocol->WriteMem(myProtocol, addr, value);
            }
          }
        } else {
          PrintHelp = TRUE;
          break;
        }
      } else if (StrCmp(L"-c", ppArgs[ArgIndex]) == 0) {
        ArgIndex++;
        if (ArgIndex != argCount && StrCmp(L"-r", ppArgs[ArgIndex]) == 0) {
          // Read data from the cmos offset.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINT8 offset = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"offset: %02x\n", offset);
            myProtocol->ReadCmos(myProtocol, offset);
          }
        } else if (ArgIndex != argCount && StrCmp(L"-w", ppArgs[ArgIndex]) == 0) {
          // Write data to the cmos offset.
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            UINT8 offset = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"offset: %02x\n", offset);
            ArgIndex++;
            if (ArgIndex == argCount) {
              PrintHelp = TRUE;
              break;
            } else {
              UINT8 value = StrHexToUintn(ppArgs[ArgIndex]);
              Print(L"value: %02x\n", value);
              myProtocol->WriteCmos(myProtocol, offset, value);
            }
          }
        }
      } else if (StrCmp(L"-p", ppArgs[ArgIndex]) == 0) {
        ArgIndex++;
        if (ArgIndex != argCount && StrCmp(L"-i", ppArgs[ArgIndex]) == 0) {
          // Read data from the pcie addr by I/O.
          if (8 == argCount) {
            PrintHelp = TRUE;
            break;    
          } else {
            ArgIndex++;
            UINT8 bus = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"bus: %02x\n", bus);
            ArgIndex++;
            UINT8 dev = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"dev: %02x\n", dev);
            ArgIndex++;
            UINT8 func = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"func: %02x\n", func);
            ArgIndex++;
            UINT8 len = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"len: %02x\n", len);
            myProtocol->ReadPcieByIo(myProtocol, bus, dev, func, len);
          }
        } else if (ArgIndex != argCount && StrCmp(L"-m", ppArgs[ArgIndex]) == 0) {
          // Read data from the pcie addr by MMIO.
          if (9 == argCount) {
            PrintHelp = TRUE;
            break;    
          } else {
            ArgIndex++;
            UINT8 base = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"base: %02x\n", base);
            ArgIndex++;
            UINT8 bus = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"bus: %02x\n", bus);
            ArgIndex++;
            UINT8 dev = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"dev: %02x\n", dev);
            ArgIndex++;
            UINT8 func = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"func: %02x\n", func);
            ArgIndex++;
            UINT8 len = StrHexToUintn(ppArgs[ArgIndex]);
            Print(L"len: %02x\n", len);
            myProtocol->ReadPcieByMmio(myProtocol, base, bus, dev, func, len);
          }
        }
      }else {
        PrintHelp = TRUE;
        break;
      }
    } else {
      PrintHelp = TRUE;
      break;
    }
    ArgIndex++;
  }

  DEBUG ((EFI_D_ERROR ,"DEBUG test end!\n"));

  if (PrintHelp) {
    PrintHelpInfo ();
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

