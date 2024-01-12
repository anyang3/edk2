#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseFdtLib/LibFdtSupport.h>
#include <Library/IoLib.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi50.h>

#define R_ACPI_IO_PM1_CNT                        0x04
#define S_ACPI_IO_PM1_CNT                        4
#define B_ACPI_IO_PM1_CNT_SLP_EN                 0x2000
#define B_ACPI_IO_PM1_CNT_SLP_TYP                0x1C00
#define V_ACPI_IO_PM1_CNT_S0                     0x0000
#define V_ACPI_IO_PM1_CNT_S1                     0x0400
#define V_ACPI_IO_PM1_CNT_S3                     0x1400
#define V_ACPI_IO_PM1_CNT_S4                     0x1800
#define V_ACPI_IO_PM1_CNT_S5                     0x1C00
#define B_ACPI_IO_PM1_CNT_GBL_RLS                0x0004
#define B_ACPI_IO_PM1_CNT_BM_RLD                 0x0002
#define B_ACPI_IO_PM1_CNT_SCI_EN                 0x0001

#define B_ACPI_IO_SMI_STS_PM1_STS_REG            BIT8