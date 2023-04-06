#include <stdio.h>
#include <string.h>
#include "CDC_ProcessLib.h"
#include "CanConverterProtocol.h"

void CAN_Transmit_Data(uint8_t * processBuffer, uint8_t *fileWriteBuf)
{
    canDataStruct canOut = { 0 };
    uint32_t burstCntr = 0; // used to control the column entry
    //canDataStruct canIn = { 0 };
    //uint32_t nextbufWriteIndex = 0;

    uint8_t canBurstBuffer[numOfElements] = { 0 };
    uint8_t maxCanBurstSize = processBuffer[0];

    maxCanBurstSize--; // decrement/exclude the first size byte

    memcpy(canBurstBuffer, processBuffer + 1, maxCanBurstSize); // copy the CDC_OUT data into a buffer to extract all the can messages

    for (int dataCntr = 0; dataCntr < maxCanBurstSize; )
    {
        canOut.canID = ProtocolCANUnpackID(canBurstBuffer + dataCntr); // index: STX, packet type, len of packet, ID&Canlen
        canOut.canDLC = ProtocolCANUnpackDataLength(canBurstBuffer + dataCntr); // index: STX, packet type, len of packet, ID&Canlen
        canOut.canData = canBurstBuffer + dataCntr + 2;
        canOut.canRTR = ProtocolCANUnpackRTR(canBurstBuffer + dataCntr);

        dataCntr += (canOut.canDLC + 2);

        burstCntr++;

        
        if (burstCntr > 1)// more than one CAN messages, indent the following messages by 2 columns
        {
            snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), 7, ",,%03x ", canOut.canID); // print the ID
        }
        else
        {
            snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), 6, ",%03x ", canOut.canID); // print the ID
        }
        
        snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), 3, "%x ", canOut.canRTR);// followed by RTR
        for (uint32_t dataCpyCntr = 0; dataCpyCntr < canOut.canDLC; dataCpyCntr++) // Followed by the data
        {
            snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), 4, "%02x ", canOut.canData[dataCpyCntr]);
        }

        snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), strlen("\n "), "\n"); // insert and new line for the next can message in the same USB packet
        //nextbufWriteIndex = strlen((char*)fileWriteBuf);
    }
}

void CAN_Receive_Data_Interpret(uint32_t bufReadSize, uint8_t *processBuffer, uint8_t* fileWriteBuf)
{
    uint32_t totalInU2CSize = bufReadSize;
    canDataStruct canIn = { 0 };
    uint32_t burstCntr = 0; // used to control the column entry

    for (uint32_t totalInU2CSizeCntr = 0; totalInU2CSizeCntr < bufReadSize; ) // process the complete buffer
    {
        switch (processBuffer[totalInU2CSizeCntr + 1])
        {
        case 0x01:
            canIn.canDLC = processBuffer[totalInU2CSizeCntr + 2] >> 4;
            canIn.canID = (((uint32_t)processBuffer[totalInU2CSizeCntr + 2] << 8) | processBuffer[totalInU2CSizeCntr + 3]) & 0x7FF; // Std ID only
            canIn.canRTR = (processBuffer[totalInU2CSizeCntr + 2] & 0x08) >> 3;
            canIn.canData = processBuffer + totalInU2CSizeCntr + 4;

            if (burstCntr)
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), 8, ",,,%03x ", canIn.canID); // +1 for the NULL char
            }
            else
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), 7, ",,%03x ", canIn.canID); // +1 for the NULL char
            }
            
            snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), 3, "%x ", canIn.canRTR);// +1 for the NULL char
            for (uint32_t dataCpyCntr = 0; dataCpyCntr < canIn.canDLC; dataCpyCntr++) // +1 for the NULL char
            {
                snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), 4, "%02x ", canIn.canData[dataCpyCntr]);// +1 for the NULL char
            }
            snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), 5, "\n"); // insert and new line for the next can message in the same USB packet
            totalInU2CSizeCntr = 7 + canIn.canDLC;
            break;
        case 0x04:
            if (burstCntr)
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), strlen(",,,ACK\n") + 1, ",,,ACK\n"); // +1 for the NULL char
            }
            else
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), strlen(",,ACK\n") + 1, ",,ACK\n"); // +1 for the NULL char
            }
            //snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), strlen("ACK\n") + 1, "ACK\n"); // +1 for the NULL char
            totalInU2CSizeCntr += 3; // EF 04 BE
            break;
        default: //non standard message received, put it in the buffer
            if (burstCntr)
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), strlen(",,,BAD MSG\n") + 1, ",,,BAD MSG\n"); // +1 for the NULL char
            }
            else
            {
                snprintf((char*)(fileWriteBuf + strlen((char*)fileWriteBuf)), strlen(",,BAD MSG\n") + 1, ",,BAD MSG\n"); // +1 for the NULL char
            }
            //snprintf((char*)fileWriteBuf + strlen((char*)fileWriteBuf), strlen("BAD MSG\n") + 1, "BAD MSG\n"); // +1 for the NULL char
            totalInU2CSizeCntr = bufReadSize;
            break;
        }

        burstCntr++; // increment to indent the next messages by 3 columns
    }
}