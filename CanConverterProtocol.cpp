/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
 /* ************************************************************************** */

 /* ************************************************************************** */
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
 /* ************************************************************************** */

 /* This section lists the other files that are included in this file.
  */

  /* TODO:  Include other files here if needed. */


  /* ************************************************************************** */
  /* ************************************************************************** */
  /* Section: File Scope or Global Data                                         */
  /* ************************************************************************** */
  /* ************************************************************************** */

  /*  A brief description of a section can be given directly below the section
      banner.
   */

   /* ************************************************************************** */
   /** Descriptive Data Item Name

     @Summary
       Brief one-line summary of the data item.

     @Description
       Full description, explaining the purpose and usage of data item.
       <p>
       Additional description in consecutive paragraphs separated by HTML
       paragraph breaks, as necessary.
       <p>
       Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

     @Remarks
       Any additional remarks
    */
#include "CanConverterProtocol.h"



    /******************************************************************************
     Function:
        uint32_t ProtocolCANUnpackID(uint8_t *writeBuffer)
     Remarks:
     Returns the ID out of the packet
     */
uint32_t ProtocolCANUnpackID(uint8_t* writeBuffer)
{
    uint32_t canID = 0;

    canID = ((*writeBuffer & 0x07) << 8U) | (*(writeBuffer + 1));

    return(canID);
}

/******************************************************************************
 Function:
    uint8_t ProtocolCANUnpackRTR(uint8_t *writeBuffer)
 Remarks:
 Returns the RTR bit
 */
uint8_t ProtocolCANUnpackRTR(uint8_t* writeBuffer)
{
    uint8_t RTR = 0;

    RTR = (*writeBuffer & 0x08) >> 3U;

    return(RTR);
}
/******************************************************************************
 Function:
    uint32_t ProtocolCANUnpackDataLength(uint8_t *writeBuffer)
 Remarks: Returns the length of CAN packet out of the protocol packet
 */
uint32_t ProtocolCANUnpackDataLength(uint8_t* writeBuffer)
{
    uint32_t canPayloadLen = 0;

    canPayloadLen = (*writeBuffer & 0xF0) >> 4U;

    return (canPayloadLen);
}

/******************************************************************************
 Function:
    uint16_t ProtocolCalculateCRC(uint8_t *inputByte, uint8_t length)
 Remarks: Evaluates CRC for USB protocol
 */
uint16_t ProtocolCalculateCRC(uint8_t* inputByte, uint8_t length)
{
    uint16_t crcBuff = 0; /* init our buffer to 0 */
    uint16_t input;
    int i;
    int j;
    uint16_t x16; /* we'll use this to hold the XOR mask */
    for (i = 0; i < length; i++)
    {
        input = *inputByte;
        inputByte++;
        for (j = 0; j < 8; j++)
        {
            /* XOR current D0 and next input bit to determine x16 value */
            if ((crcBuff & 0x0001) ^ (input & 0x01))
                x16 = 0x8408;
            else
                x16 = 0x0000;
            /* shift crc buffer */
            crcBuff = crcBuff >> 1;
            /* XOR in the x16 value */
            crcBuff ^= x16;
            /* shift input for next iteration */
            input = input >> 1;
        }
    }
    return crcBuff;
}


/* *****************************************************************************
 End of File
 */
