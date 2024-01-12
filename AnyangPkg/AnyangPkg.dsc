
################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Anyang
  PLATFORM_GUID                  = 98fefe09-0be4-4994-94f6-e8665f2a76a1
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/AnyangPkg
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = NOOPT|DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  #
  # Defines for default states.  These can be changed on the command line.
  # -D FLAG=VALUE
  #
  DEFINE SECURE_BOOT_ENABLE      = FALSE
  DEFINE SMM_REQUIRE             = FALSE
  DEFINE SOURCE_DEBUG_ENABLE     = FALSE
  DEFINE TPM_ENABLE              = FALSE
  DEFINE TPM_CONFIG_ENABLE       = FALSE


!include NetworkPkg/NetworkDefines.dsc.inc

  #
  # Device drivers
  #
  DEFINE PVSCSI_ENABLE           = TRUE
  DEFINE MPT_SCSI_ENABLE         = TRUE
  DEFINE LSI_SCSI_ENABLE         = FALSE

  #
  # Flash size selection. Setting FD_SIZE_IN_KB on the command line directly to
  # one of the supported values, in place of any of the convenience macros, is
  # permitted.
  #
!ifdef $(FD_SIZE_1MB)
  DEFINE FD_SIZE_IN_KB           = 1024
!else
!ifdef $(FD_SIZE_2MB)
  DEFINE FD_SIZE_IN_KB           = 2048
!else
!ifdef $(FD_SIZE_4MB)
  DEFINE FD_SIZE_IN_KB           = 4096
!else
  DEFINE FD_SIZE_IN_KB           = 4096
!endif
!endif
!endif

[BuildOptions]
  GCC:RELEASE_*_*_CC_FLAGS             = -DMDEPKG_NDEBUG
  INTEL:RELEASE_*_*_CC_FLAGS           = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS            = /D MDEPKG_NDEBUG
!if $(TOOL_CHAIN_TAG) != "XCODE5" && $(TOOL_CHAIN_TAG) != "CLANGPDB"
  GCC:*_*_*_CC_FLAGS                   = -mno-mmx -mno-sse
!endif
!if $(SOURCE_DEBUG_ENABLE) == TRUE
  MSFT:*_*_X64_GENFW_FLAGS  = --keepexceptiontable
  GCC:*_*_X64_GENFW_FLAGS   = --keepexceptiontable
  INTEL:*_*_X64_GENFW_FLAGS = --keepexceptiontable
!endif

  #
  # Disable deprecated APIs.
  #
  MSFT:*_*_*_CC_FLAGS = /D DISABLE_NEW_DEPRECATED_INTERFACES
  INTEL:*_*_*_CC_FLAGS = /D DISABLE_NEW_DEPRECATED_INTERFACES
  GCC:*_*_*_CC_FLAGS = -D DISABLE_NEW_DEPRECATED_INTERFACES

!include NetworkPkg/NetworkBuildOptions.dsc.inc

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_*_DLINK_FLAGS = -z common-page-size=0x1000
  XCODE:*_*_*_DLINK_FLAGS = -seg1addr 0x1000 -segalign 0x1000
  XCODE:*_*_*_MTOC_FLAGS = -align 0x1000
  CLANGPDB:*_*_*_DLINK_FLAGS = /ALIGN:4096

# Force PE/COFF sections to be aligned at 4KB boundaries to support page level
# protection of DXE_SMM_DRIVER/SMM_CORE modules
[BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.SMM_CORE]
  GCC:*_*_*_DLINK_FLAGS = -z common-page-size=0x1000
  XCODE:*_*_*_DLINK_FLAGS = -seg1addr 0x1000 -segalign 0x1000
  XCODE:*_*_*_MTOC_FLAGS = -align 0x1000
  CLANGPDB:*_*_*_DLINK_FLAGS = /ALIGN:4096

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PciSegmentLib|MdePkg/Library/BasePciSegmentLibPci/BasePciSegmentLibPci.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsicSev.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf

  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf

  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform.
#
################################################################################
[PcdsFeatureFlag]

[PcdsFixedAtBuild]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|$(DEBUG_PROPERTY_MASK)
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|$(DEBUG_PRINT_ERROR_LEVEL)

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsDynamicDefault]
  
[PcdsDynamicHii]

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform.
#
################################################################################
[Components]
  AnyangPkg/Applications/Training1_Anyang/Training1_Anyang.inf {
    <LibraryClasses>
    ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  }
  AnyangPkg/Applications/Training6_Anyang/Training6_Anyang.inf {
    <LibraryClasses>
    SmbiosLib|EmulatorPkg/Library/SmbiosLib/SmbiosLib.inf
  }
  AnyangPkg/Applications/Training14_Anyang/Training14_Anyang.inf
