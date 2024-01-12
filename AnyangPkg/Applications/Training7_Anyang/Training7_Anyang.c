#include <Training7_Anyang.h>

EFI_IPMI_TRANSPORT      *gIpmiTransport = NULL;
AMI_IPMI_FRU_GLOBAL     *gIpmiFruGlobal = NULL;

IPMI_SEL_EVENT_RECORD_DATA  gSelRecordData = {
    // Record Id.
    0000,
    // Record Type.
    IPMI_SEL_SYSTEM_RECORD,
    // Time stamp.
    00000000,
    // Generator Id: BIOS.
    (UINT16) AMI_IPMI_GENERATOR_ID (AMI_IPMI_BIOS_SOFTWARE_ID),
    IPMI_EVM_REVISION,
    // Sensor Type.
    0x12,
    // Sensor Number.
    0x0,
    // Event DIR.
    IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE,
    // Sensor specific Offset for Time stamp Clock Synch Event.
    05,
    // ED2
    00,
    // ED3
    0xFF
    };


/**
    Sends the Event-Log data to the destination. Destination can be a remote
    target like LAN, ICMB, local targets BMC, IPMB or a Firmware-Volume.

    @retval EFI_SUCCESS             Event Log was recorded successfully.
    @retval EFI_NOT_FOUND           Event Log target not found.
    @retval EFI_INVALID_PARAMETER   Invalid Parameter.
    @retval EFI_OUT_OF_RESOURCES    Not enough resources to record data.
    @retval Others                  Status returned from function calls used.

**/
EFI_STATUS
EFIAPI
EfiSendSelData (VOID)
{
    EFI_STATUS                                  Status;
    IPMI_ADD_SEL_ENTRY_RESPONSE                 ResponseData;
    UINT8                                       ResponseDataSize;
    // BOOLEAN                                     Delsupport;

    // Get the IpmiTransportProtocol
    Status = gBS->LocateProtocol (
                    &gEfiDxeIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **)&gIpmiTransport );

    Print(L"gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status);

    if (EFI_ERROR (Status)) {
        return Status;
    }

    //anyang3_20231026D001
    // // Check if SEL is full before logging any entries
    // if (AmiIpmiIsSelFull (
    //          gIpmiTransport,
    //          &Delsupport )) {
    //     return EFI_OUT_OF_RESOURCES;
    // }

    // Send Sel
    Print(L"Test1\n");
    ResponseDataSize = sizeof (ResponseData);
    Status = gIpmiTransport->SendIpmiCommand (
                                gIpmiTransport,
                                IPMI_NETFN_STORAGE,
                                AMI_BMC_LUN,
                                IPMI_STORAGE_ADD_SEL_ENTRY,
                                (UINT8 *)&gSelRecordData,
                                sizeof (gSelRecordData),
                                (UINT8 *)&ResponseData,
                                &ResponseDataSize );

    Print(L"Test2\n");
    Print(L"IPMI_STORAGE_ADD_SEL_ENTRY Status: %r RecordId: %x CompletionCode: %x\n", Status, ResponseData.RecordId, ResponseData.CompletionCode);

    return Status;
}

/** @internal
    Converts the low nibble of a byte to hex unicode character.

    @param[in] Nibble   The Nibble that needs to be converted as Hex Unicode
                        character.

    @return CHAR16  The Hex Unicode character of input nibble.

**/
CHAR16
NibbleToHexChar (
  IN UINT8  Nibble )
{
    Nibble &= 0x0F;
    if (Nibble <= 0x9) {
        return (CHAR16)(Nibble + L'0');
    }

    return (CHAR16)(Nibble - 0xA + L'A');
}

/** @internal
    Converts binary input string to Unicode hexadecimal string.

    @param[out]     Str                     The output unicode hex string.
    @param[in, out] HexStringBufferLength   On input, length of the unicode buffer.
                                            On output, unicode hex string length.
    @param[in]      Buf                     The Binary input string.
    @param[in]      Len                     The Binary input string length.

    @retval EFI_BUFFER_TOO_SMALL  Str string buffer length is not sufficient.
    @retval EFI_SUCCESS           The Binary input string is converted successfully.

**/
EFI_STATUS
BufToHexString (
  OUT    CHAR16     *Str,
  IN OUT UINTN      *HexStringBufferLength,
  IN     UINT8      *Buf,
  IN     UINTN      Len )
{
    UINTN   Idx;
    UINT8   Byte;
    UINTN   StrLen;

    /* Make sure string is either passed or allocate enough.
      It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
      Plus the Unicode termination character.*/
    StrLen = Len * 2;
    if (StrLen > ((*HexStringBufferLength) - 1)) {
        *HexStringBufferLength = StrLen + 1;
        return EFI_BUFFER_TOO_SMALL;
    }

    *HexStringBufferLength = StrLen + 1;
    // Ends the string.
    Str[StrLen] = L'\0';

    for (Idx = 0; Idx < Len; Idx++) {
        Byte = Buf[Idx];
        Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
        Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
    }

    return EFI_SUCCESS;
}

/** @internal
    Converts binary buffer to Unicode character.

    @param[out] UnicodeStr  The Unicode string to be written to.
                            The buffer must be large enough.
    @param[in]  BinPtr      The binary string to be converted.
    @param[in]  Length      Length of the binary string to be converted.

    @return CHAR16* The address to the Unicode string - same as UnicodeStr.

**/
CHAR16*
IpmiBinary2Unicode (
  OUT CHAR16    *UnicodeStr,
  IN  UINT8     *BinPtr,
  IN  UINTN     Length )
{
    EFI_STATUS    Status;
    UINTN         BufferLen;

    /* 1 byte of the binary buffer takes 2 unicode characters (4 bytes) for hex representation.
       BufferLen includes space for hex representation + "h" suffix + null terminator.*/
    BufferLen = (Length * 2 + 2) * sizeof (UINT16);

    Status = BufToHexString (
                UnicodeStr,
                &BufferLen,
                BinPtr,
                Length );

    if (EFI_ERROR(Status)) {
        UnicodeStr[0] = L'\0';
    } else {
        UnicodeStr[Length * 2] = L'h';
        UnicodeStr[Length * 2 + 1] = L'\0';
    }

    return UnicodeStr;
}

/** @internal
    Converts 6bit ASCII characters to Unicode.

    @param[out] UnicodeStr  The Unicode string to be written to. The buffer must
                            be large enough.
    @param[in]  AsciiStr    The 6bit ASCII string to be converted.
    @param[in]  Length      Length of the ASCII string to be converted.

    @retval CHAR16* The address to the Unicode string.

**/
CHAR16*
Ipmi6bAscii2Unicode (
  OUT CHAR16    *UnicodeStr,
  IN  UINT8     *AsciiStr,
  IN  UINTN     Length )
{
    UINTN   StrIndex1;
    UINTN   StrIndex2;
    UINT32  Str;

    /* Unpack 3 6bit value (24 bits) to 4 8bit value at a time (3 bytes to 4 bytes)
       Also 6bit to 8bit value has to be offset by 0x20.
       i.e., 000000b -> 0x20, 000001b -> 0x21, etc.*/

    // Convert AsciiStr up to length that is multiple of 3.
    for (StrIndex1 = 0; StrIndex1 < Length / 3; StrIndex1++) {
        Str = *((UINT32 *) (AsciiStr + StrIndex1 * 3));
        for (StrIndex2 = 0; StrIndex2 < 4; StrIndex2++) {
            UnicodeStr[StrIndex1 * 4 + StrIndex2] = (CHAR8) (Str & 0x3F) + 0x20;
            Str >>= 6;
        }
    }

    // Convert rest of AsciiStr entries.
    Str = *((UINT32 *) (AsciiStr + StrIndex1 * 3));
    for (StrIndex2 = 0; StrIndex2 < (Length - ((Length / 3) * 3)); StrIndex2++) {
        UnicodeStr[StrIndex1 * 4 + StrIndex2] = (CHAR8) (Str & 0x3F) + 0x20;
        Str >>= 6;
    }

    UnicodeStr[StrIndex1 * 4 + StrIndex2] = '\0';

    return UnicodeStr;
}
/**
    Converts ASCII characters to Unicode.

    @param[OUT] UnicodeStr   The Unicode string to be written to. The buffer must
                             be large enough.
    @param[IN]  AsciiStr     The ASCII string to be converted.
    @param[IN]  Length       Length of the ASCII string to be converted.

    @retval CHAR16* The address to the Unicode string - same as UnicodeStr.

**/

CHAR16*
IpmiAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  UINT8          *AsciiStr,
  IN  UINTN           Length )
{
    UINTN   i;

    for (i = 0; i < Length; i++) {
        UnicodeStr[i] = AsciiStr[i];
    }

    UnicodeStr[i] = '\0';

    return UnicodeStr;
}
/** @internal
    Reads the FRU data from InputDataBuffer and convert it to unicode string.

    @param[in]  InputDataBuffer         Input FRU data buffer.
    @param[in]  Type                    FRU data encoded type.
    @param[in]  Length                  FRU data length.
    @param[out] OutputUnicodeString     Output unicode string buffer.

    @retval EFI_SUCCESS           FRU data converted to unicode successfully.
    @retval EFI_UNSUPPORTED       FRU data type not supported.
    @retval EFI_OUT_OF_RESOURCES  Buffer allocation failed for unicode string.

**/
EFI_STATUS
GetUnicodeFromEncodedFru (
  IN  UINT8     *InputDataBuffer,
  IN  UINT8     Type,
  IN  UINT8     Length,
  OUT CHAR16    **OutputUnicodeString )
{
    //huangyw4_20211011D001-start
    if(Length>=2 && *(InputDataBuffer + 1)==' ' && *(InputDataBuffer + 2)==' ')
        Length = 2;  //Cut to two space
    //huangyw4_20211011D001-end
    switch (Type) {
        case 0:
            if (FRU_SMBIOS_BINARY_TYPE_CODE_SUPPORT) {
                /* Binary coding. Allocate space for hex representation + "h" suffix
                   + null terminator.*/
                *OutputUnicodeString = AllocateRuntimeZeroPool ((Length * 2 + 2) * sizeof(UINT16));
                if (*OutputUnicodeString != NULL) {
                    IpmiBinary2Unicode (
                        *OutputUnicodeString,
                        (InputDataBuffer + 1),
                        Length );
                    return EFI_SUCCESS;
                }
            }
            break;

        case 2:
            // 6 bit ASCII Packed coding.
            *OutputUnicodeString = AllocateRuntimeZeroPool ((((Length * 8) / 3) + 2));
            if (*OutputUnicodeString != NULL) {
                Ipmi6bAscii2Unicode (
                    *OutputUnicodeString,
                    (InputDataBuffer + 1),
                    Length );
                return EFI_SUCCESS;
            }
            break;

        case 3:
            // ASCII coding.
            *OutputUnicodeString = AllocateRuntimeZeroPool (((Length * 2) + 2));
            if (*OutputUnicodeString != NULL) {
                IpmiAscii2Unicode (
                    *OutputUnicodeString,
                    (InputDataBuffer + 1),
                    Length );
                return EFI_SUCCESS;
            }
            break;

        default:
            return EFI_UNSUPPORTED;
    }
    return EFI_OUT_OF_RESOURCES;
}

/** @internal
    Reads the FRU data from InputDataBuffer and save it in OutputUnicodeString.

    @param[in]  InputDataBuffer         Input FRU data buffer.
    @param[out] OutputUnicodeString     Output unicode string buffer.

    @retval EFI_SUCCESS     FRU data converted to unicode successfully.
    @retval EFI_NOT_FOUND   FRU data length is not valid in InputDataBuffer.
    @retval Others          Return status of function used.

**/

EFI_STATUS
GetFruData (
  IN  UINT8     *InputDataBuffer,
  OUT CHAR16    **OutputUnicodeString 
  )
{
    UINT8       Type = (((*InputDataBuffer) & FRU_DATA_TYPE_MASK) >> 0x06);
    UINT8       Length = ((*InputDataBuffer) & FRU_DATA_LENGTH_MASK);
    EFI_STATUS  Status;

    if (Length > 0) {
        Status = GetUnicodeFromEncodedFru (
                    InputDataBuffer,
                    Type,
                    Length,
                    OutputUnicodeString );
    } else {
        return EFI_NOT_FOUND;
    }
    return Status;
}

/**
    Display the Fru Area data to the destination.

    @retval EFI_SUCCESS             Event Log was recorded successfully.
    @retval EFI_NOT_FOUND           Event Log target not found.
    @retval EFI_INVALID_PARAMETER   Invalid Parameter.
    @retval EFI_OUT_OF_RESOURCES    Not enough resources to record data.
    @retval Others                  Status returned from function calls used.

**/
EFI_STATUS
EFIAPI
EfiDispFruInfo (VOID)
{
    EFI_STATUS                                      Status;
    AMI_IPMI_FRU_GLOBAL                             *FruPrivate;
    AMI_IPMI_FRU_COMMON_HEADER                      FruCommonHeader;
    UINTN                                           Index;
    UINTN                                           Offset = 0;
    UINTN                                           Length;
    UINT8                                           *TempPtr;
    UINT8                                           PcdSize;
    CHAR16                                          *StrPtr;
    CHAR16                                          *AssetTagString = NULL;
    CHAR16                                          *ManufacturerString = NULL;
    CHAR16                                          *ProductName = NULL;
    UINTN                                           SizeofBuffer;
    UINT8                                           FruRecIndex = 0;
    AMI_IPMI_FRU_MULTI_RECORD_HEADER                MultiRecordHeader;
    BOOLEAN                                         IsSystemUuidSubRecordType = FALSE;
    UINT8                                           SubRecordType = 0;
    AMI_IPMI_SYSTEM_UUID_SUB_RECORD_WITH_CHECKSUM   SystemUuidSubRecordwithChecksum;
    UINTN                                           GetFruRedirDataSize;
    EFI_GUID                                        *Uuid = NULL;

    // Get the RedirFruProtocol
    Status = gBS->LocateProtocol (
                    &gEfiRedirFruProtocolGuid,
                    NULL,
                    (VOID **)&gIpmiFruGlobal->IpmiRedirFruProtocol );

    Print(L"gEfiRedirFruProtocolGuid Status: %r \n", Status);

    if (EFI_ERROR (Status)) {
        return Status;
    }


    /* Search through the FRU records in mIpmiFruGlobal->FruDeviceInfo
       to identify FRU_DEVICE_ID record.*/
    if (FRU_DEVICE_ID != 0xFF) {
        FruPrivate = AMI_IPMI_INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS (gIpmiFruGlobal);
        for (Index = 0; Index < FruPrivate->NumSlots; Index++) {
            if ((FruPrivate->FruDeviceInfo[Index].Valid) && (FruPrivate->FruDeviceInfo[Index].FruDevice.Bits.FruDeviceId == FRU_DEVICE_ID)) {
                FruRecIndex = (UINT8) Index;
                break;
            }
        }
    }

    // Get FruCommonHeader
    GetFruRedirDataSize = sizeof (AMI_IPMI_FRU_COMMON_HEADER);
    Status = gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData (
                            &gIpmiFruGlobal->IpmiRedirFruProtocol,
                            FruRecIndex,
                            0,
                            &GetFruRedirDataSize,
                            (UINT8*) &FruCommonHeader );

    if (EFI_ERROR (Status)) {
        return Status;
    }
    if (CalculateCheckSum8 (
            (UINT8 *)&FruCommonHeader,
            sizeof (FruCommonHeader) ) != 0) {
        DEBUG((DEBUG_ERROR, "FRU header invalid.\n"));
        return Status;
    }

    // Prepare Manufacturer data (SMBIOS Type 1).
    Offset = FruCommonHeader.ProductInfoStartingOffset * 8;
    if (Offset > 0) {
        Length = 0;
        // Size of the length of FRU area to read.
        GetFruRedirDataSize = sizeof (UINT8);
        Status = gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData (
                                &gIpmiFruGlobal->IpmiRedirFruProtocol,
                                FruRecIndex,
                                (Offset + 1),
                                &GetFruRedirDataSize,
                                (UINT8*) &Length );

        if (!EFI_ERROR(Status) && (Length > 0)) {
            Length = Length * 8;
            TempPtr = (UINT8*)AllocateRuntimeZeroPool (Length);
            if (TempPtr != NULL) {
                Status = gIpmiFruGlobal->IpmiRedirFruProtocol.GetFruRedirData  (
                                        &gIpmiFruGlobal->IpmiRedirFruProtocol,
                                        FruRecIndex,
                                        Offset,
                                        &Length,
                                        TempPtr );
                if (!EFI_ERROR (Status)) {
                    // Product Manufacturer.
                    Offset = PRODUCT_MFG_OFFSET;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                    Status = GetFruData (
                                     &TempPtr[Offset],
                                     &ManufacturerString );
                    if (!EFI_ERROR (Status)) {
                        SizeofBuffer = StrnSizeS (ManufacturerString, MAX_STRING_SIZE);
                        PcdSize = (UINT8)PcdGetSize(AmiPcdSystemManufacturer);
                        if (SizeofBuffer > PcdSize) {
                            DEBUG ((DEBUG_INFO, "AmiPcdSystemManufacturer:SizeofBuffer > PcdSize so truncating the string\n"));
                            SizeofBuffer = PcdSize - 2;//To avoid overwriting last byte of PCD data(Null terminator)
                        }
                        DEBUG ((DEBUG_INFO, "SystemManufacturer: ManufacturerString: %S\n", ManufacturerString));
                        PcdSetPtrS (
                                AmiPcdSystemManufacturer,
                                &SizeofBuffer,
                                (VOID*)ManufacturerString );
                        DEBUG ((DEBUG_INFO, "SystemManufacturer: %S\n", (CHAR16 *) PcdGetPtr (AmiPcdSystemManufacturer)));
                    }

                    // Get Product Name
                    Offset = Offset + Length + 1;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];

                    Status = GetFruData (
                                    &TempPtr[Offset], 
                                    &ProductName);

                    Offset = Offset + Length + 1;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];

                    /* Product Model Number, concatenate it to the existing
                       (if exists) Product Name.*/
                    SizeofBuffer = StrnSizeS (ProductName, MAX_STRING_SIZE);
                    PcdSize = (UINT8)PcdGetSize(AmiPcdSystemProductName);
                    if (SizeofBuffer > PcdSize) {
                        DEBUG ((DEBUG_INFO, "AmiPcdSystemProductName:SizeofBuffer > PcdSize so truncating the string \n"));
                        SizeofBuffer = PcdSize - 2;//To avoid overwriting last byte of PCD data(Null terminator)
                    }
                    DEBUG ((DEBUG_INFO, " AmiPcdSystemProductName: ProductName: %S\n", ProductName));
                    PcdSetPtrS (
                                    AmiPcdSystemProductName,
                                    &SizeofBuffer,
                                    (VOID*)ProductName );

                    DEBUG ((DEBUG_INFO, "SystemProductName: %S\n", (CHAR16 *) PcdGetPtr (AmiPcdSystemProductName)));
                    FreePool (ProductName);
					          //wjh_20210129D001-start
                    if (!EFI_ERROR (Status)) {
                        Status = GetFruData (
                                        &TempPtr[Offset],
                                        &StrPtr );
                        if (!EFI_ERROR (Status)) {
                            SizeofBuffer = StrnSizeS (StrPtr, MAX_STRING_SIZE);
                            PcdSize = (UINT8)PcdGetSize(AmiPcdSystemSKUNumber);
                            if (SizeofBuffer > PcdSize) {
                                DEBUG ((DEBUG_INFO, "AmiPcdSystemSKUNumber:SizeofBuffer > PcdSize so truncating the string \n"));
                                SizeofBuffer = PcdSize - 2;//To avoid overwriting last byte of PCD data(Null terminator)
                            }
                            DEBUG ((DEBUG_INFO, "AmiPcdSystemSKUNumber: StrPtr: %S \n", StrPtr));
                            PcdSetPtrS (
                                    AmiPcdSystemSKUNumber,
                                    &SizeofBuffer,
                                    (VOID*)StrPtr );
                            DEBUG ((DEBUG_INFO, "AmiPcdSystemSKUNumber: %S\n", (CHAR16 *) PcdGetPtr (AmiPcdSystemSKUNumber)));
                            FreePool (StrPtr);
                        }
                    } 
					          //wjh_20210129D001-end

                    // Product Version.
                    Offset = Offset + Length + 1;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                    Status = GetFruData (
                                     &TempPtr[Offset],
                                     &StrPtr );
                    if (!EFI_ERROR (Status)) {
                        SizeofBuffer = StrnSizeS (StrPtr, MAX_STRING_SIZE);
                        PcdSize = (UINT8)PcdGetSize(AmiPcdSystemVersion);
                        if (SizeofBuffer > PcdSize) {
                            DEBUG ((DEBUG_INFO, "AmiPcdSystemVersion:SizeofBuffer > PcdSize so truncating the string \n"));
                            SizeofBuffer = PcdSize - 2;//To avoid overwriting last byte of PCD data(Null terminator)
                        }
                        DEBUG ((DEBUG_INFO, "AmiPcdSystemVersion: StrPtr: %S \n", StrPtr));
                        PcdSetPtrS (
                                AmiPcdSystemVersion,
                                &SizeofBuffer,
                                (VOID*)StrPtr );
                        DEBUG ((DEBUG_INFO, "AmiPcdSystemVersion: %S\n", (CHAR16 *) PcdGetPtr (AmiPcdSystemVersion)));
                        FreePool (StrPtr);
                    }

                    // Product Serial Number.
                    Offset = Offset + Length + 1;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                    Status = GetFruData (
                                     &TempPtr[Offset],
                                     &StrPtr );
                        
                    if (!EFI_ERROR (Status)) {
                        SizeofBuffer = StrnSizeS (StrPtr, MAX_STRING_SIZE);
                        PcdSize = (UINT8)PcdGetSize(AmiPcdSystemSerialNumber);
                        if (SizeofBuffer > PcdSize) {
                            DEBUG ((DEBUG_INFO, "AmiPcdSystemSerialNumber:SizeofBuffer > PcdSize so truncating the string \n"));
                            SizeofBuffer = PcdSize - 2;//To avoid overwriting last byte of PCD data(Null terminator)
                        }
                        DEBUG ((DEBUG_INFO, "AmiPcdSystemSerialNumber: StrPtr: %S\n", StrPtr));
                        PcdSetPtrS (
                                AmiPcdSystemSerialNumber,
                                &SizeofBuffer,
                                (VOID*)StrPtr );
                        DEBUG ((DEBUG_INFO, "AmiPcdSystemSerialNumber: %S\n", (CHAR16 *) PcdGetPtr (AmiPcdSystemSerialNumber)));  
                        FreePool (StrPtr);
                    }

                    // Product Asset Tag.
                    Offset = Offset + Length + 1;
                    Length = FRU_DATA_LENGTH_MASK & TempPtr[Offset];
                    Status = GetFruData (
                                     &TempPtr[Offset],
                                     &AssetTagString );
                    if (EFI_ERROR (Status)) {
                        DEBUG ((DEBUG_ERROR, "GetFRUData :AssetTagString: Status %r\n",Status));
                    }
                }
            }
            FreePool (TempPtr);
        }
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
  EFI_STATUS                       Status;
  Status = EfiSendSelData();
  return Status;

}