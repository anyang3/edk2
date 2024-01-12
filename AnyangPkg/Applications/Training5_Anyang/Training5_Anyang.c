/** @file
  This is a test application that demonstrates how to use the C-style entry point
  for a shell application.

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Training5_Anyang.h>

/**
  Disp Acpi Table: RSDP, XSDT, FADT, DSDT
  And Transfer to S5 State.
  @anyang3_20231016
**/
VOID DispAcpiTransS5(VOID)
{
  UINTN                                         i, j, EntryCount;
  CHAR8                                         strBuff[20];
  UINT16                                        DataPm1a, DataPm1b;
  UINT64                                        *EntryPtr;
  EFI_GUID                                      AcpiTableGuid  = ACPI_TABLE_GUID;
  EFI_GUID                                      Acpi2TableGuid = EFI_ACPI_TABLE_GUID;
  EFI_CONFIGURATION_TABLE                       *configTab = NULL;  
  EFI_ACPI_DESCRIPTION_HEADER                   *XSDT, *Entry, *DSDT;
  EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE     *FADT;
  EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Root;

  Print(L"List ACPI Table:\n");
  configTab = gST->ConfigurationTable;
  
  for (i=0; i<gST->NumberOfTableEntries; i++)
  {   
    if ((CompareGuid(&configTab->VendorGuid, &AcpiTableGuid)) ||
      (CompareGuid(&configTab->VendorGuid, &Acpi2TableGuid)))
      { 
        // Print Acpi Table Info
        Print(L"Found Table: %g\n", &configTab->VendorGuid); 
        Print(L"BaseAddress = [0x%p]\n", configTab);
        
        // Print RSDP Info
        Root = configTab->VendorTable;
        Print(L"ROOT SYSTEM DESCRIPTION = [0x%p]\n", Root);
        ZeroMem(strBuff, sizeof(strBuff));
        CopyMem(strBuff, &(Root->Signature), sizeof(UINT64));
        Print(L"RSDP-Signature = [%a] ( ", strBuff);
        for (j=0; j<8; j++)
        {
          Print(L"0x%x ",strBuff[j]);
        }
        Print(L")\n");
        Print(L"RSDP-Revision = [%d]\n", Root->Revision);
        
        // Print RSDT & XSDT Info
        Print(L"RSDT Address = [0x%p], Length = [0x%x]\n", Root->RsdtAddress, Root->Length);
        Print(L"XSDT Address = [0x%lx]\n", Root->XsdtAddress);
        if (Root->Revision >= EFI_ACPI_5_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION)
        {
          XSDT = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(Root->XsdtAddress));
          EntryCount = (XSDT->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) 
                      / sizeof(UINT64);
          ZeroMem(strBuff, sizeof(strBuff));
          CopyMem(strBuff, &(XSDT->Signature), sizeof(UINT32));
          Print(L"XSDT-Sign [%a]\n", strBuff);           
          Print(L"XSDT-length [%d]\n", XSDT->Length);            
          Print(L"XSDT-Counter [%d]\n", EntryCount); 
                  
          EntryPtr = (UINTN *)(XSDT + 1);
          for (j=0; j<EntryCount; j++, EntryPtr++)
          {
            Entry = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(*EntryPtr));
            if (Entry->Signature == 0x50434146) { // "FACP"
              // Print FADT Info
              FADT = (EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE *)(UINTN) Entry;
              Print(L"FADT-Dsdt = [0x%x]\n", FADT->Dsdt);
              Print(L"FADT-xDsdt = [0x%lx]\n", FADT->XDsdt);
            
              // Print DSDT Info
              DSDT = (EFI_ACPI_DESCRIPTION_HEADER *)((UINTN)(FADT->Dsdt));
              ZeroMem(strBuff, sizeof(strBuff));
              CopyMem(strBuff, &(DSDT->Signature), sizeof(UINT32));
              Print(L"DSDT-Sign = [%a]\n", strBuff);
              Print(L"DSDT-Length = [0x%x]\n", DSDT->Length);

              // Turn to S5 State
              // Pm1a
              if(FADT->XPm1aCntBlk.Address != 0)
              {
                Print(L"FADT-XPm1aCntBlk Address = [0x%lx]\n", FADT->XPm1aCntBlk.Address);
                DataPm1a = IoRead16(FADT->XPm1aCntBlk.Address);
                Print(L"FADT-XPm1aCntBlk SrcValue = [0x%x]\n", DataPm1a);

                DataPm1a = (UINT16) ((DataPm1a &~(B_ACPI_IO_PM1_CNT_SLP_TYP + B_ACPI_IO_PM1_CNT_SLP_EN)) | V_ACPI_IO_PM1_CNT_S5);
                DataPm1a = DataPm1a | B_ACPI_IO_PM1_CNT_SLP_EN;
                Print(L"FADT-XPm1aCntBlk DstValue = [0x%x]\n", DataPm1a);

                IoWrite16(FADT->XPm1aCntBlk.Address, DataPm1a);
                DataPm1a = IoRead16(FADT->XPm1aCntBlk.Address);
                Print(L"FADT-XPm1aCntBlk Re-ReadValue = [0x%x]\n", DataPm1a);
              } else {
                Print(L"FADT->Pm1aCntBlk Address = [0x%lx]\n", FADT->Pm1aCntBlk);
                DataPm1a = IoRead16(FADT->Pm1aCntBlk);
                Print(L"FADT-Pm1aCntBlk SrcValue = [0x%x]\n", DataPm1a);

                DataPm1a = (UINT16) ((DataPm1a &~(B_ACPI_IO_PM1_CNT_SLP_TYP + B_ACPI_IO_PM1_CNT_SLP_EN)) | V_ACPI_IO_PM1_CNT_S5);
                DataPm1a = DataPm1a | B_ACPI_IO_PM1_CNT_SLP_EN;
                Print(L"FADT-Pm1aCntBlk DstValue = [0x%x]\n", DataPm1a);

                IoWrite16(FADT->Pm1aCntBlk, DataPm1a);
                DataPm1a = IoRead16(FADT->Pm1aCntBlk);
                Print(L"FADT-Pm1aCntBlk Re-ReadValue = [0x%x]\n", DataPm1a);
              }

              // Pm1b
              if(FADT->XPm1bCntBlk.Address != 0)
              {
                Print(L"FADT-XPm1bCntBlk Address = [0x%lx]\n", FADT->XPm1bCntBlk.Address);
                DataPm1b = IoRead16(FADT->XPm1bCntBlk.Address);
                Print(L"FADT-XPm1bCntBlk SrcValue = [0x%x]\n", DataPm1b);

                DataPm1b = (UINT16) ((DataPm1b &~(B_ACPI_IO_PM1_CNT_SLP_TYP + B_ACPI_IO_PM1_CNT_SLP_EN)) | V_ACPI_IO_PM1_CNT_S5);
                DataPm1b = DataPm1b | B_ACPI_IO_PM1_CNT_SLP_EN;
                Print(L"FADT-XPm1bCntBlk DstValue = [0x%x]\n", DataPm1b);

                IoWrite16(FADT->XPm1bCntBlk.Address, DataPm1b);
                DataPm1b = IoRead16(FADT->XPm1bCntBlk.Address);
                Print(L"FADT-XPm1bCntBlk Re-ReadValue = [0x%x]\n", DataPm1b);
              } else if (FADT->Pm1bCntBlk != 0) {
                Print(L"FADT->Pm1bCntBlk Address = [0x%lx]\n", FADT->Pm1bCntBlk);
                DataPm1b = IoRead16(FADT->Pm1bCntBlk);
                Print(L"FADT-Pm1bCntBlk SrcValue = [0x%x]\n", DataPm1b);

                DataPm1b = (UINT16) ((DataPm1b &~(B_ACPI_IO_PM1_CNT_SLP_TYP + B_ACPI_IO_PM1_CNT_SLP_EN)) | V_ACPI_IO_PM1_CNT_S5);
                DataPm1b = DataPm1b | B_ACPI_IO_PM1_CNT_SLP_EN;
                Print(L"FADT-Pm1bCntBlk DstValue = [0x%x]\n", DataPm1b);

                IoWrite16(FADT->Pm1bCntBlk, DataPm1b);
                DataPm1b = IoRead16(FADT->Pm1bCntBlk);
                Print(L"FADT-Pm1bCntBlk Re-ReadValue = [0x%x]\n", DataPm1b);
              }

              Print(L"Turn to S5 State Success. Power Off.\n");
            }
          }           
        }
      }
    configTab++;
  }
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
  DispAcpiTransS5();
  DEBUG ((EFI_D_ERROR ,"DEBUG test end!\n"));
  
  return EFI_SUCCESS;
}

