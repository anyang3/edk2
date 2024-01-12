#include <Training7_Anyang.h>

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