#include <Training9_Anyang.h>

// The maximum supported CPUID leaf index starting from leaf 0x00000000.
UINT32  gMaximumBasicFunction    = CPUID_SIGNATURE;

/**
  Display CPUID_SIGNATURE leaf.
**/
VOID
CpuidSignature (
  VOID
  )
{
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
  CHAR8  Signature[13];

  AsmCpuid (CPUID_SIGNATURE, &Eax, &Ebx, &Ecx, &Edx);

  Print (L"CPUID_SIGNATURE (Leaf %08x)\n", CPUID_SIGNATURE);
  *(UINT32 *)(Signature + 0) = Ebx;
  *(UINT32 *)(Signature + 4) = Edx;
  *(UINT32 *)(Signature + 8) = Ecx;
  Signature [12] = 0;
  Print (L"Signature = %a\n", Signature);

  gMaximumBasicFunction = Eax;
}

/**
  Read CPU Model, Family and stepping ID.

  @retval EFI_SUCCESS             Event Log was recorded successfully.
  @retval EFI_NOT_FOUND           Event Log target not found.
  @retval EFI_INVALID_PARAMETER   Invalid Parameter.
  @retval EFI_OUT_OF_RESOURCES    Not enough resources to record data.
  @retval Others                  Status returned from function calls used.
  @anyang3_20231107
**/
EFI_STATUS
EFIAPI
EfiReadCpuInfo (VOID)
{
  CPUID_VERSION_INFO_EAX  Eax;
  CPUID_VERSION_INFO_EBX  Ebx;
  CPUID_VERSION_INFO_ECX  Ecx;
  CPUID_VERSION_INFO_EDX  Edx;
  UINT32                  DispFamily;
  UINT32                  DispModel;
  UINT32                  DispSteppingID;

  CpuidSignature();
  if (CPUID_VERSION_INFO > gMaximumBasicFunction) {
    return EFI_OUT_OF_RESOURCES;
  }

  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  Print (L"CPUID_VERSION_INFO (Leaf %08x)\n", CPUID_VERSION_INFO);
  Print (L"EAX=%08x, EBX=%08x, ECX=%08x, EDX=%08x\n", Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32);

  DispFamily = Eax.Bits.FamilyId;
  if (DispFamily == 0x0F) {
    DispFamily |= (Eax.Bits.ExtendedFamilyId << 4);
  }

  DispModel = Eax.Bits.Model;
  if (DispFamily == 0x06 || DispFamily == 0x0f) {
    DispModel |= (Eax.Bits.ExtendedModelId << 4);
  }

  DispSteppingID = Eax.Bits.SteppingId;
  Print(L"Family = %x, Model = %x, SteppingID = %x\n", DispFamily, DispModel, DispSteppingID);

  return EFI_SUCCESS;
}

/**
  Read CPU Cache L1/L2/L3 Size.

  @retval EFI_SUCCESS             Event Log was recorded successfully.
  @retval EFI_NOT_FOUND           Event Log target not found.
  @retval EFI_INVALID_PARAMETER   Invalid Parameter.
  @retval EFI_OUT_OF_RESOURCES    Not enough resources to record data.
  @retval Others                  Status returned from function calls used.
  @anyang3_20231107
**/
EFI_STATUS
EFIAPI
EfiReadCpuCacheSize (VOID)
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  EFI_SMBIOS_TYPE                  SmbiosType;
  EFI_SMBIOS_TABLE_HEADER          *SmbiosHeader;
  SMBIOS_TABLE_TYPE7               *SmbiosRecord;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Out;
  EFI_SMBIOS_PROTOCOL              *Smbios;

  // Locate Simple Text Output Protocol
  Status = gBS->LocateProtocol(&gEfiSimpleTextOutProtocolGuid, NULL, (VOID**)&Out);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ShellAppMain Could not locate Simple Text Output protocol.%r\n", Status));
    return Status;
  }
  
  // Locate Smbios Protocol
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ShellAppMain Could not locate SMBIOS protocol.%r\n", Status));
    return Status;
  }
  Print(L"Smbios Protocol Status: %r\n", Status);

  // Scan all "Cache Info" tables.
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = SMBIOS_TYPE_CACHE_INFORMATION;  //Type 7
  for (Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL);
       !EFI_ERROR (Status);
       Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL))
  {
    if (SmbiosHeader->Length < sizeof(SMBIOS_TABLE_TYPE7)) {
      // Malformed table header, skip to next.
      continue;
    }

    SmbiosRecord = (SMBIOS_TABLE_TYPE7 *)SmbiosHeader;
    Print(L"SmbiosType = %d, SmbiosHandle = 0x%x\n", SmbiosRecord->Hdr.Type, SmbiosRecord->Hdr.Handle);
  }

  return Status;
}

/**
  Read X2APIC APICID from MSR 802H.

  @retval EFI_SUCCESS             Event Log was recorded successfully.
  @retval EFI_NOT_FOUND           Event Log target not found.
  @retval EFI_INVALID_PARAMETER   Invalid Parameter.
  @retval EFI_OUT_OF_RESOURCES    Not enough resources to record data.
  @retval Others                  Status returned from function calls used.
  @anyang3_20231107
**/
EFI_STATUS
EFIAPI
EfiReadX2APICID (VOID)
{
    EFI_STATUS                       Status;
    UINTN                            Count;
    UINTN                            NumberofCpus;
    UINTN                            NumberOfEnabledProc;
    UINTN                            ApicId;
    UINTN                            PlatformIdQword;
    EFI_MP_SERVICES_PROTOCOL         *MpService;
    
    // Locate MpService Protocol
    Status = gBS->LocateProtocol (
                &gEfiMpServiceProtocolGuid,
                NULL,
                (VOID**)&MpService
                );
    
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "UefiMain Could not locate MpService protocol.%r\n", Status));
        return Status ;
    }
    Print(L"MpServices Protocol Status: %r\n", Status);

    Status = MpService->GetNumberOfProcessors (
                           MpService,
                           &NumberofCpus,
                           &NumberOfEnabledProc
                           );
    
    Print(L"GetNumberOfProcessors Status: %r, NumberofCpus: %d\n", Status, NumberofCpus);
    
    for(Count = 0; Count < NumberofCpus; Count++) 
    {
        ApicId = AsmReadMsr64 (MSR_IA32_X2APIC_APICID);
        Print(L"ApicId = 0x%x, CoreId: 0x%x\n", ApicId, ((ApicId >> 1) & 0x7));
        PlatformIdQword = AsmReadMsr64 (MSR_IA32_PLATFORM_ID);
        Print(L"PlatformIdQword = 0x%x\n", PlatformIdQword);
    }
                           
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
  Print(L"Test1: EfiReadCpuInfo\n");
  Status = EfiReadCpuInfo();
  Print(L"\n");
  Print(L"Test2: EfiReadCpuCacheSize\n");
  Status = EfiReadCpuCacheSize();
  return Status;

}