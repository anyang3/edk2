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
#include <Library/DebugLib.h>
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

  DEBUG ((EFI_D_ERROR , "DEBUG test start!\n"));
  Print(L"Hello Shell!\n");
  DEBUG ((EFI_D_ERROR ,"DEBUG test end!\n"));

  PrintHelpInfo();
  
  return EFI_SUCCESS;
}

