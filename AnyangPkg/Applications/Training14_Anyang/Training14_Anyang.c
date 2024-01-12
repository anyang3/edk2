#include <Training14_Anyang.h>

volatile UINT8  *gSpiBase = NULL;

/**
  Read Flash Protected Range and Print PRB(0-14bits), RPE(15bit), PRL(16-30bits), WPE(31bit).
  @anyang3_20231219
**/
EFI_STATUS
EFIAPI
EfiReadFlashProtectedRange (VOID)
{
    UINT32              PchSpiBar0;
    UINT32              ProtectedRange;
    UINT8               Index;

    // Get the SpiBar0 Base Address.
    PchSpiBar0 = PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (
                                     DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                                     DEFAULT_PCI_BUS_NUMBER_PCH,
                                     PCI_DEVICE_NUMBER_PCH_SPI,
                                     PCI_FUNCTION_NUMBER_PCH_SPI,
                                     R_SPI_CFG_BAR0
                                     ));
    PchSpiBar0 &= ~(B_SPI_CFG_BAR0_MASK);

    if (PchSpiBar0 == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR : PchSpiBar0 is invalid!\n"));
        ASSERT (FALSE);
    }
    if ((MmioRead32 (PchSpiBar0 + R_SPI_MEM_HSFSC) & B_SPI_MEM_HSFSC_FDV) == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR : SPI Flash Signature invalid, cannot use the Hardware Sequencing registers!\n"));
        ASSERT (FALSE);
    }
    Print(L"SPI Bar0 BASE Address = 0x%x\n", PchSpiBar0);
    Print(L"Data of Bar0 Offset 0x00 = 0x%x\n", MmioRead32 (PchSpiBar0));
    Print(L"Data of Bar0 Offset 0x04 = 0x%x\n", MmioRead32 (PchSpiBar0 + 0x04));
    Print(L"Data of Bar0 Offset 0x10 = 0x%x\n", MmioRead32 (PchSpiBar0 + 0x10));

    //Get Flash Protected Range from SPI Memory Mapped Configuration Registers.
    for (Index = 0; Index < 6; Index++) {
        // Read Flash Protected Range 0-5 (Offset 84h, 88h, 8Ch, 90h, 94h).
        //Data = MmioRead32 (PchSpiBar0 + 0x84 + 4*Index);
        ProtectedRange = MmioRead32 (PchSpiBar0 + 0x84);
        Print(L"Read Flash Protected Range [%d]:\n", Index);
        // Print PRB(0-14bits), RPE(15bit), PRL(16-30bits), WPE(31bit)
        Print(L"PRB = 0x%x, RPE = 0x%x, PRL = 0x%x, WPE = 0x%x\n", ProtectedRange & 0x00007FFF,
            ProtectedRange & BIT15, (ProtectedRange >> 16) & 0x7FFF, ProtectedRange & BIT31);
    }

    return EFI_SUCCESS;
}

/**
  Read Flash Protected Range and Print PRB(0-14bits), RPE(15bit), PRL(16-30bits), WPE(31bit).
  @anyang3_20231219
**/
EFI_STATUS
EFIAPI
EfiReadFlashProtectedRange2 (VOID)
{
    UINT8               *gSpiBase;
    UINT32              Data;
    UINT8               Index;

    gSpiBase = (UINT8*)(UINTN)(PciSegmentRead32 (PCI_SEGMENT_LIB_ADDRESS (
                                     DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                                     DEFAULT_PCI_BUS_NUMBER_PCH,
                                     PCI_DEVICE_NUMBER_PCH_SPI,
                                     PCI_FUNCTION_NUMBER_PCH_SPI,
                                     R_SPI_CFG_BAR0
                                     )) & (~(UINT32)B_SPI_CFG_BAR0_MASK));


    if (gSpiBase == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR : gSpiBase is invalid!\n"));
        ASSERT (FALSE);
    }
    /*
    if ((*(volatile UINT32*)(gSpiBase) & B_SPI_MEM_HSFSC_FDV) == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR : SPI Flash Signature invalid, cannot use the Hardware Sequencing registers!\n"));
        ASSERT (FALSE);
    }
    */
    Print(L"SPI BASE Address = 0x%x\n", gSpiBase);
    Print(L"Offset 0x00 = 0x%x\n", *(volatile UINT32*)(gSpiBase));
    Print(L"Offset 0x04 = 0x%x\n", *(volatile UINT32*)(gSpiBase + 0x04));
    Print(L"Offset 0x10 = 0x%x\n", *(volatile UINT32*)(gSpiBase + 0x10));
    Print(L"Offset 0xDC = 0x%x\n", *(volatile UINT32*)(gSpiBase + 0xDC));

    //Get Flash Protected Range from SPI Memory Mapped Configuration Registers.
    for (Index = 0; Index < 6; Index++) {
        // Read Flash Protected Range 0-5 (Offset 84h, 88h, 8Ch, 90h, 94h).
        Data = *(volatile UINT32*)(gSpiBase + 0x84 + 4*Index);
        Print(L"Data = 0x%x\n", Data);
        Print(L"Read Flash Protected Range [%d]:\n", Index);
        // Print PRB(0-14bits), RPE(15bit), PRL(16-30bits), WPE(31bit)
        Print(L"PRB = 0x%x, RPE = 0x%x, PRL = 0x%x, WPE = 0x%x\n", Data & 0x00007FFF, Data & BIT15, (Data >> 16) & 0x7FFF, Data & BIT31);
    }

    return EFI_SUCCESS;
}

/**
  Read Bios Control Reg and Print LE(bit 1), WPD(bit 0);
  Write WPD(bit 0) to 1 and Check it;
  Write WPD(bit 0) to 1 by SW SMI and Check the log.
  @anyang3_20231219
**/
EFI_STATUS
EFIAPI
EfiReadWriteBiosControlReg (VOID)
{
    UINT32          PchSpiBiosControlAddr;
    UINT32          PchSpiBiosControlData;
    UINT32          PchSpiBiosControlNewData;


    // Get the SpiBiosControl Data.
    PchSpiBiosControlAddr = PCI_SEGMENT_LIB_ADDRESS (
                                            DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                                            DEFAULT_PCI_BUS_NUMBER_PCH,
                                            PCI_DEVICE_NUMBER_PCH_SPI,
                                            PCI_FUNCTION_NUMBER_PCH_SPI,
                                            SPI_CFG_BIOS_CONTROL_REG
                                            );
    PchSpiBiosControlData = PciSegmentRead32 (PchSpiBiosControlAddr);

    if (PchSpiBiosControlData == 0) {
        DEBUG ((DEBUG_ERROR, "ERROR : PchSpiBiosControl is invalid!\n"));
        ASSERT (FALSE);
    }
    Print(L"Data of Spi Bios Control Reg = 0x%x\n", PchSpiBiosControlData);
    Print(L"Lock Enable(LE) bit = 0x%x\n", (PchSpiBiosControlData >> 1) & 0x1);
    Print(L"Write Protect Disable(WPD) bit = 0x%x\n", PchSpiBiosControlData & 0x1);

    // Write WPD bit to 1 and Check it.
    PchSpiBiosControlData |= 0x1;
    PciSegmentWrite32 (PchSpiBiosControlAddr, PchSpiBiosControlData);
    PchSpiBiosControlNewData = PciSegmentRead32 (PchSpiBiosControlAddr);
    Print(L"Write WPD bit to 1 by PciSegmentWrite32 Success.\n");
    Print(L"New Data of Spi Bios Control Reg = 0x%x\n", PchSpiBiosControlNewData);
    Print(L"New WPD bit = 0x%x\n", PchSpiBiosControlNewData & 0x1);

    // Write WPD(bit 0) to 1 by SW SMI.
    IoWrite8(SW_SMI_IO_ADDRESS, BIOS_CONTROL_SMI);
    Print(L"Write WPD bit to 1 by SW SMI Success.\n");

    return EFI_SUCCESS;
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
    EFI_STATUS  Status;
    Print(L"Test1: EfiReadFlashProtectedRange\n");
    Status = EfiReadFlashProtectedRange();
    Print(L"\n");
    Print(L"Test2: EfiReadWriteBiosControlReg\n");
    Status = EfiReadWriteBiosControlReg();
    return Status;

}