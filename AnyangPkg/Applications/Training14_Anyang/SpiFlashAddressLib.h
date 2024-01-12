//***********************************************************************
//*                                                                     *
//*                      Copyright (c) 1985-2022, AMI.                  *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/**
    This function convert input mmio address to flash part offset.

    @param  MmioAddress Memory mapped address which maps BIOS image
    @param  FlashOffset Flash part offset

    @retval TRUE If input MmioAddress is already flash part offset, or it's 
                 converted to flash part offset successfully
    @retval FALSE Failed to convert MmioAddress to flash part
**/
BOOLEAN
PlatformConvertAddressToOffset(
    IN  UINTN   MmioAddress,
    OUT UINTN   *FlashOffset
);

/**
    This function convert input flash part offset to mmio address.

    @param  FlashOffset Flash part offset
    @param  MmioAddress Memory mapped address which maps BIOS image
    @param  AvailableLength Length of converted address to maximum address

    @retval TRUE If input FlashOffset is already mmio address, or it's 
                 converted to mmio address successfully
    @retval FALSE Failed to convert FlashOffset to mmio address
**/
BOOLEAN
PlatformConvertOffsetToAddress(
    IN  UINTN   FlashOffset,
    OUT UINTN   *MmioAddress,
    OUT UINTN   *AvailableLength
);