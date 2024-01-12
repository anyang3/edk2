#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/SmbiosLib.h>
#include <Library/Token.h>
#include <Library/PcdLib.h>
#include <AmiFru.h>
#include <Library/IpmiLib.h>
// #include <Library/AmiSelLib.h>  //anyang3_20231026D001
#include <Library/IpmiCommandLib.h>
#include <Library/AmiBmcCommonInterfaceLib.h>
#include <Include/AmiIpmiNetFnStorageDefinitions.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <IndustryStandard/IpmiNetFnSensorEvent.h>

#define CHASSIS_TYPE_LENGTH                     1
#define CHASSIS_TYPE_OFFSET                     2
#define CHASSIS_PART_NUMBER                     3

#define PRODUCT_MFG_OFFSET                      3
#define BOARD_MFG_OFFSET                        6
#define FRU_DEVICE_LOCATOR_RECORD               0x11
#define MANAGEMENT_ACCESS_RECORD                0x03
#define SYSTEM_UUID_SUB_RECORD_TYPE             0x07
#define RECORD_CHECKSUM_SIZE                    sizeof(UINT8)

#define FRU_DATA_LENGTH_MASK                    0x3F
#define FRU_DATA_TYPE_MASK                      0xC0

#define IPMI_FRU_COMMAND_RETRY_DELAY            30000
#define IPMI_FRU_COMMAND_RETRY_COUNTER          3
#define FRU_CC_CMD_SUCCESS                      0x00
#define FRU_CC_DEVICE_BUSY                      0x81
#define FRU_CC_WRITE_PROTECTED                  0x80
#define MAX_REQ_RES_DATA_SIZE                   0xFF

#define MAX_STRING_SIZE                         0xFF // As per latest SMBIOS Spec no Limit for String Size so taking MAX length of the String size
#define SIZE_OF_SMBIOS_STRUCTURE_END_MACRO      sizeof(UINT16)  /* Macro represent size of Smbios structure end. Every Smbios type ends with 0x0000.*/

//lily_20210805D001_start
#define  FRU_SYSTEM_MANUFACTURER    L"FruSysMfg"
#define  FRU_SYSTEM_PRODUCT_NAME    L"FruSysProd"
#define  FRU_SYSTEM_VERSION         L"FruSysPart"
#define  FRU_SYSTEM_SERIAL_NUMBER   L"FruSysSn"
#define  FRU_SYSTEM_SKU_NUMBER      L"FruSysExtra"
#ifndef  FRU_SYSTEM_UUID
#define  FRU_SYSTEM_UUID            L"FruSysUuid"
#endif
#define  FRU_BOARD_MANUFACTURER     L"FruBrdMfg"
#define  FRU_BOARD_PRODUCT_NAME     L"FruBrdProd"
#define  FRU_BOARD_VERSION          L"FruBrdPart"
#define  FRU_BOARD_SERIAL_NUMBER    L"FruBrdSn"
#define  FRU_BOARD_ASSERT_TAG       L"FruBrdTag"
#define  FRU_CHASSIS_MANUFACTURER   L"FruChsMfg"
#define  FRU_CHASSIS_TYPE           L"FruChsType"
#define  FRU_CHASSIS_VERSION        L"FruChsPart"
#define  FRU_CHASSIS_SERIAL_NUMBER  L"FruChsSn"
#define  FRU_CHASSIS_ASSERT_TAG     L"FruChsTag"
//lily_20210805D001_end