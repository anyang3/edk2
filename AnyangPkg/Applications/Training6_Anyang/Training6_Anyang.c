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
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/Smbios.h>
#include <Protocol/SimpleTextOut.h>

/**
  Add Type11(OEM String) To Smbios
  @anyang3_20231020
**/
EFI_STATUS
AddSmbiosOemTypeTable (
  IN  EFI_SMBIOS_PROTOCOL  *Smbios,
  IN  UINT8                OemStrCnt,
  IN  CHAR8                **OemStr
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  SMBIOS_TABLE_TYPE11              *SmbiosRecord;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Out;
  UINTN                            StrIdx, StrSize, TotolSize;
  CHAR8                            *Str;

  // Locate Simple Text Output Protocol
  Status = gBS->LocateProtocol(&gEfiSimpleTextOutProtocolGuid, NULL, (VOID**)&Out);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ShellAppMain Could not locate Simple Text Output protocol.%r\n", Status));
    return Status;
  }

  // Calculate the size of the fixed record and optional string pack
  TotolSize = sizeof (SMBIOS_TABLE_TYPE11);
  if (OemStr == NULL) {
    TotolSize += 2; // Min string section is double null
  } else if (OemStr[0] == NULL) {
    TotolSize += 2; // Min string section is double null
  } else {
    for (StrIdx = 0; StrIdx < OemStrCnt; StrIdx++) {
      StrSize = AsciiStrSize(OemStr[StrIdx]);
      TotolSize += StrSize;
    }
    // Don't forget the terminating double null
    TotolSize += 1;
  }

  // Report OEM String to Type 11 SMBIOS Record
  Print(L"OemStrCnt: %d\n", OemStrCnt);
  Print(L"OemTotolSize: %d\n", TotolSize);
  SmbiosRecord = (SMBIOS_TABLE_TYPE11*)AllocatePool (TotolSize);
  if (SmbiosRecord == NULL) {
    DEBUG ((DEBUG_ERROR, "AllocateZeroPool Returned NULL pointer\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (SmbiosRecord, TotolSize);
  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_OEM_STRINGS;
  SmbiosRecord->Hdr.Length = (UINT8) sizeof (SMBIOS_TABLE_TYPE11);
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->StringCount = OemStrCnt;

  if (OemStr != NULL) {
    // Append string pack
    Str = (CHAR8 *)(SmbiosRecord) + SmbiosRecord->Hdr.Length;

    for (StrIdx = 0; StrIdx < OemStrCnt; StrIdx++) {
      StrSize = AsciiStrSize(OemStr[StrIdx]);
      CopyMem (Str, (CHAR8 *)OemStr[StrIdx], StrSize);
      Str += StrSize;
    }
    *Str = 0;
  }

  // Add Oem Table to Smbios
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
  FreePool(SmbiosRecord);
  if (EFI_ERROR(Status)) {
    Print(L"Add Oem String Error!\n");
  } else {
    Print(L"Add Oem String Success.\n");
  }
  return Status;
}

/**
  Return SMBIOS string for the given string number.

  @param[in] Smbios         Pointer to SMBIOS structure.
  @param[in] StringNumber   String number to return. -1 is used to skip all strings and
                            point to the next SMBIOS structure.

  @return Pointer to string, or pointer to next SMBIOS strcuture if StringNumber == -1
**/
CHAR8 *
GetSmbiosString (
  IN  SMBIOS_TABLE_TYPE11   *Smbios,
  IN  UINT16                StrCnt
  )
{
  UINT16        StrIdx;
  CHAR8         *Str;

  ASSERT (Smbios != NULL);

  // Skip over formatted section
  Str = (CHAR8 *)Smbios + Smbios->Hdr.Length;

  // Look through unformated section
  for (StrIdx = 1; StrIdx <= StrCnt; StrIdx++) {
    if (StrCnt == StrIdx) {
      return Str;
    }

    // Skip string
    for ( ; *Str != 0; Str++) {
      // Print(L"(String: %x)", *String);
    }
    Str++;

    if (*Str == 0) {
      // If double NULL then we are done.
      // Return pointer to next structure in Smbios.
      // if you pass in a -1 you will always get here
      Smbios = (SMBIOS_TABLE_TYPE11 *)++Str;
      return NULL;
    }
  }
  return NULL;
}

/**
  Disp Smbios Type11(OEM String) Table 
  @anyang3_20231020
**/
EFI_STATUS
DispSmbiosOemTypeTable (
  IN  EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  EFI_SMBIOS_TYPE                  SmbiosType;
  EFI_SMBIOS_TABLE_HEADER          *SmbiosHeader;
  SMBIOS_TABLE_TYPE11              *SmbiosRecord;
  UINT8                            OemStrCnt;
  CHAR8                            *OemStr;
  UINTN                            StrIdx;
  UINT8                            Offset;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Out;

  // Locate Simple Text Output Protocol
  Status = gBS->LocateProtocol(&gEfiSimpleTextOutProtocolGuid, NULL, (VOID**)&Out);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "ShellAppMain Could not locate Simple Text Output protocol.%r\n", Status));
    return Status;
  }

  // Scan all "OEM Strings" tables.
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = SMBIOS_TYPE_OEM_STRINGS;
  for (Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL);
       !EFI_ERROR (Status);
       Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL))
  {
    if (SmbiosHeader->Length < sizeof(SMBIOS_TABLE_TYPE11)) {
      // Malformed table header, skip to next.
      continue;
    }

    SmbiosRecord = (SMBIOS_TABLE_TYPE11 *)SmbiosHeader;
    Print(L"SmbiosType = %d, SmbiosHandle = 0x%x\n", SmbiosRecord->Hdr.Type, SmbiosRecord->Hdr.Handle);
    OemStrCnt = SmbiosRecord->StringCount;
    Print(L"OemStrCnt = %d\n", OemStrCnt);

    for (StrIdx = 1; StrIdx <= OemStrCnt; StrIdx++) {
      Print(L"OemStr%d = ", StrIdx);
      OemStr = GetSmbiosString(SmbiosRecord, StrIdx);
      Offset = 0;
      while(*(OemStr + Offset) != 0) {
        Print(L"%c", *(OemStr + Offset));
        Offset++;
      }
      Print(L"\n");
    }
  }

  return Status;
}

/**
  Update Smbios Type11(OEM String) Table with the given SmbiosHandle and StrNum
  @anyang3_20231020
**/
EFI_STATUS
UpdateSmbiosOemTypeTable (
  IN  EFI_SMBIOS_PROTOCOL  *Smbios,
  IN  UINT16  Handle,  
  IN  UINT8   StrNum,
  IN  CHAR8   *Str
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  EFI_SMBIOS_TYPE                  SmbiosType;
  EFI_SMBIOS_TABLE_HEADER          *SmbiosHeader;
  SMBIOS_TABLE_TYPE11              *SmbiosRecord;
  UINTN                            StrIdx;

  // Scan all "OEM Strings" tables.
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = SMBIOS_TYPE_OEM_STRINGS;
  for (Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL);
       !EFI_ERROR (Status);
       Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL))
  {
    if (SmbiosHeader->Length < sizeof(SMBIOS_TABLE_TYPE11)) {
      // Malformed table header, skip to next.
      continue;
    }

    // Update Smbios Str with the given SmbiosHandle and StrNum
    SmbiosRecord = (SMBIOS_TABLE_TYPE11 *)SmbiosHeader;
    StrIdx = StrNum;
    if (Handle == SmbiosRecord->Hdr.Handle) {
      Status = Smbios->UpdateString(Smbios, &Handle, &StrIdx, Str);
      if (EFI_ERROR(Status)) {
        Print(L"Update Oem String Error!\n");
      } else {
        Print(L"Update Oem String Success.\n");
      }
      return Status;
    }
  }

  Print(L"Wrong SmbiosRecord!\n");
  return EFI_INVALID_PARAMETER;
}

/**
 Remove Smbios Type11(OEM String) Table with the given SmbiosHandle
  @anyang3_20231020
**/
EFI_STATUS
RemoveSmbiosOemTypeTable (
  IN  EFI_SMBIOS_PROTOCOL  *Smbios,
  IN  UINT16  Handle        
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_TABLE_HEADER          *SmbiosHeader;
  SMBIOS_TABLE_TYPE11              *SmbiosRecord;
  EFI_SMBIOS_HANDLE                SmbiosHandle;
  EFI_SMBIOS_TYPE                  SmbiosType;

  // Scan all "OEM Strings" tables.
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = SMBIOS_TYPE_OEM_STRINGS;
  for (Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL);
       !EFI_ERROR (Status);
       Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHeader, NULL))
  {
    if (SmbiosHeader->Length < sizeof(SMBIOS_TABLE_TYPE11)) {
      // Malformed table header, skip to next.
      continue;
    }

    // Remove Smbios Str with the given SmbiosHandle
    SmbiosRecord = (SMBIOS_TABLE_TYPE11 *)SmbiosHeader;
    if (Handle == SmbiosRecord->Hdr.Handle) {
      Status = Smbios->Remove(Smbios, Handle);
      if (EFI_ERROR(Status)) {
        Print(L"Remove Oem String Error!\n");
      } else {
        Print(L"Remove Oem String Success.\n");
      }
      return Status;
    }
  }

  Print(L"Wrong SmbiosRecord!\n");
  return EFI_INVALID_PARAMETER;
}

/*
  Print out help information.
*/
VOID
PrintHelpInfo (
  VOID
  )
{
  Print(L"\n");
  Print(L"Usage: Training6_Anyang.efi [-?]\n");
  Print(L"  -help: Print help info.\n");
  Print(L"  -add [StrCnt] [String1] [String2]...: Add Smbios type oem table with the given string.\n");
  Print(L"  -disp: Display Smbios type oem table.\n");
  Print(L"  -update [SmbiosHandle] [StrNum] [string]: update Smbios type oem table's string.\n");
  Print(L"  -remove [SmbiosHandle]: Remove Smbios type oem table.\n");
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
  EFI_STATUS                       Status;
  EFI_SMBIOS_PROTOCOL              *Smbios;
  UINT8                            StrIndex, StrCnt, StrNum, Offset;
  UINT16                           SmbiosHandle;
  UINTN                            ArgIndex          = 1;
  UINT8                            MaxCnt            = 255;
  BOOLEAN                          PrintHelp         = FALSE;
  CHAR8                            *StrBuf[255];
  CHAR8                            **StrPtr;
  CHAR8                            *NewStr;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Out;

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

  // Allocate String Pool
  NewStr = (CHAR8 *)AllocateZeroPool (SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR8));
  for (StrIndex = 0; StrIndex < MaxCnt; StrIndex++) {
    StrBuf[StrIndex] = (CHAR8 *)AllocateZeroPool (SMBIOS_STRING_MAX_LENGTH * sizeof(CHAR8));
  }
  StrPtr = StrBuf;
  if (StrBuf == NULL || NewStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Hanle Argument
  while (ArgIndex < argCount) {
    if (ppArgs[ArgIndex] != NULL) {
      if (StrCmp(L"-help", ppArgs[ArgIndex]) == 0) {
        PrintHelp = TRUE;
        break;
      } else if (StrCmp(L"-add", ppArgs[ArgIndex]) == 0) {
        // Add Smbios type oem table with the given string
        ArgIndex++;
        if (ArgIndex == argCount) {
          PrintHelp = TRUE;
          break;
        } else {
          StrCnt = StrHexToUintn(ppArgs[ArgIndex]);
          if (StrCnt > MaxCnt) {
            return EFI_OUT_OF_RESOURCES;
          }
        }
        ArgIndex++;
        Print(L"InsertStr: ");
        for (StrIndex = 0; StrIndex < StrCnt && ArgIndex != argCount; StrIndex++, ArgIndex++) {
          if (StrLen(ppArgs[ArgIndex]) > SMBIOS_STRING_MAX_LENGTH) {
            return EFI_BUFFER_TOO_SMALL;
          } else {
            Out->OutputString(Out, ppArgs[ArgIndex]);
            Print(L" ");
            Offset = 0;
            while(*(ppArgs[ArgIndex] + Offset) != 0) {
              // Transfer ppArgs[ArgIndex](CHAR16*) to StrPtr[StrIndex](CHAR8*)
              *(StrPtr[StrIndex] + Offset) = (CHAR8)*(ppArgs[ArgIndex] + Offset);
              Offset++;
            }
            *(StrPtr[StrIndex] + Offset) = 0;
          }
        }
        Print(L"\n");
        Status = AddSmbiosOemTypeTable(Smbios, StrCnt, StrBuf);
      } else if (StrCmp(L"-disp", ppArgs[ArgIndex]) == 0) {
        // Display Smbios type oem table
        Status = DispSmbiosOemTypeTable(Smbios);
      } else if (StrCmp(L"-update", ppArgs[ArgIndex]) == 0) {
        // update Smbios type oem table's string
        ArgIndex++;
        if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
        } else {
          SmbiosHandle = StrHexToUintn(ppArgs[ArgIndex]);
          ArgIndex++;
          if (ArgIndex == argCount) {
            PrintHelp = TRUE;
            break;
          } else {
            StrNum = StrHexToUintn(ppArgs[ArgIndex]);
            ArgIndex++;
            if (ArgIndex == argCount) {
              PrintHelp = TRUE;
              break;
            } else {
              Offset = 0;
              while(*(ppArgs[ArgIndex] + Offset) != 0) {
              // Transfer ppArgs[ArgIndex](CHAR16*) to NewStr(CHAR8*)
              *(NewStr + Offset) = (CHAR8)*(ppArgs[ArgIndex] + Offset);
              Offset++;
            }
            *(NewStr + Offset) = 0;
            }
          }
        }
        Status = UpdateSmbiosOemTypeTable(Smbios, SmbiosHandle, StrNum, NewStr);
      } else if (StrCmp(L"-remove", ppArgs[ArgIndex]) == 0) {
        // Remove Smbios type oem table
        ArgIndex++;
        if (ArgIndex == argCount) {
          PrintHelp = TRUE;
          break;
        } else {
          SmbiosHandle = StrHexToUintn(ppArgs[ArgIndex]);
        }
        Status = RemoveSmbiosOemTypeTable(Smbios, SmbiosHandle);
      } else {
        PrintHelp = TRUE;
        break;
      }
    }
    ArgIndex++;
  }

  // Free String Pool
  FreePool(NewStr);
  for (StrIndex = 0; StrIndex < MaxCnt; StrIndex++) {
    FreePool(StrBuf[StrIndex]);
  }

  if (PrintHelp) {
    PrintHelpInfo ();
    return EFI_INVALID_PARAMETER;
  }
  
  return EFI_SUCCESS;
}
