#ifndef CDC_PROCESSLIB_H
#define CDC_PROCESSLIB_H
#include <stdio.h>
#include <cstdint>

#define numOfElements 500 // make this equal to size of buf to which it will be written after the read operation
//extern const uint32_t numOfElements; // make this equal to size of buf to which it will be written after the read operation

typedef struct
{
    uint32_t canID;
    uint32_t canDLC;
    uint8_t canRTR;
    uint8_t*canData;
}canDataStruct;

void CAN_Transmit_Data(uint8_t* processBuffer, uint8_t* fileWriteBuf);
void CAN_Receive_Data_Interpret(uint32_t bufReadSize, uint8_t* processBuffer, uint8_t* fileWriteBuf);

//extern canDataStruct canOut, canIn;

#endif // !CDC_PROCESSLIB_H

