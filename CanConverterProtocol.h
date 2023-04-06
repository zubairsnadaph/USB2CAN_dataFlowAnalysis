/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    CanConverterProtocol.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
 /* ************************************************************************** */

#ifndef CAN_CONVERTER_PROTOCOL_H    /* Guard against multiple inclusion */
#define CAN_CONVERTER_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

 /* TODO:  Include other files here if needed. */

 /* **************WinPREP protocol enveloping messages ******************* */
#define STX 0xef
#define ETX 0xbe
#define PROTOCOL_START          STX
#define PROTOCOL_END            ETX
//#define PACKET_TYPE_PING        0x02
//#define PACKET_TYPE_PING_ACK    0x03

enum PACKET_TYPE
{
    PACKET_TYPE_UNKNOWN = 0,
    PACKET_TYPE_CAN_MESSAGE,
    PACKET_TYPE_PING,
    PACKET_TYPE_PING_ACK,
    PACKET_TYPE_ACK,
    PACKET_TYPE_ERROR_MESSAGE,
    PACKET_TYPE_CAN_MESSAGE_COLLECTION,
    PACKET_TYPE_CAN_ERROR_DATA,
    PACKET_TYPE_INVALID
};

enum ERROR_CODES
{
    SYNC_LOST = 0,
    INVALID_MESSAGE_TYPE,
    BAD_CRC,
    CAN_ERROR
};


uint32_t ProtocolCANUnpackID(uint8_t* writeBuffer);
uint8_t ProtocolCANUnpackRTR(uint8_t* writeBuffer);
uint32_t ProtocolCANUnpackDataLength(uint8_t* writeBuffer);
uint16_t ProtocolCalculateCRC(uint8_t* inputByte, uint8_t length);

#endif /* CAN_CONVERTER_PROTOCOL_H */

/* *****************************************************************************
 End of File
 */
