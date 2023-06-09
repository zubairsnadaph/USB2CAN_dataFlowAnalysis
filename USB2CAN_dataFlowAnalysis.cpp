// USB2CAN_dataFlowAnalysis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fileapi.h>
#include <sys/stat.h>
#include <string.h>
#include <sstream>
#include "CanConverterProtocol.h"
#include "CDC_ProcessLib.h"

//using namespace std;

#define elementSize 1 //read 1 byte at a time
#define readFileName "C:\\HarmonyProjects_unreleasedInitialU2C\\U2C Aura pausing testing\\z1 up move dataonly data.csv"
#define writeFileName "C:\\HarmonyProjects_unreleasedInitialU2C\\U2C Aura pausing testing\\z1 up move dataflow.csv"

//const uint32_t numOfElements = 500; // make this equal to size of buf to which it will be written after the read operation
uint8_t cdcProcessBuffer[numOfElements];
char InOutACK[2][20] = { ",ACK\n", ",,ACK\n" };

struct stat file_status;

void checkTheDatatoCopy(uint8_t* buf)
{
    bool bufSizeReadDone = false;
    bool dataDirIN = false;
    int bufReadSize = 0;
    uint32_t commaDelimiterCntr = 0;
    uint8_t hexBuf[500] = { 0 };
    uint32_t hexBufCntr = 0;
    uint32_t dataStartIndex = 0;
    uint32_t timestampData = 0;
    char timeStampString[50] = { 0 };
    std::string date = (char*)buf;
    

    for (int asciiCntr = 0; asciiCntr < strlen((char*)buf); asciiCntr++)
    {
        switch (commaDelimiterCntr)
        {
        case 1:
            //dataStartIndex = asciiCntr; // save the instance to check the second delimiter
            timestampData = 60 * std::strtol((char*)(buf + asciiCntr), NULL, 10); //receive the minute and convert to seconds
            //snprintf(timeStampString, 50, "%d", 60 * timestampData);
            asciiCntr = (int)date.find(":") + 1; // increment the index to seconds char

            timestampData += std::strtol((char*)(buf + asciiCntr), NULL, 10); //receive seconds and add to converted seconds (above)
            timestampData *= 1000000;// convert to microseconds
            //snprintf(timeStampString + strlen(timeStampString), 50 - strlen(timeStampString), "%d", timestampData);
            asciiCntr = (int)date.find(".") + 1; // increment the index to milli sec char
            dataStartIndex = asciiCntr; // save the instance to check the second occurance of "."

            timestampData += 1000 * std::strtol((char*)(buf + asciiCntr), NULL, 10); //receive milliseconds, convert to micro and add to previous
            //snprintf(timeStampString + strlen(timeStampString), 50 - strlen(timeStampString), "%d", timestampData);
            asciiCntr = (int)date.find(".", dataStartIndex) + 1; // increment the index to microsec char

            timestampData += std::strtol((char*)(buf + asciiCntr), NULL, 10); //receive microseconds and add to previous
            snprintf(timeStampString + strlen(timeStampString), 50 - strlen(timeStampString), "%d", timestampData);
            asciiCntr = (int)date.find(",", dataStartIndex); // increment the index to ',',no +1 increment to satisfy the condition after breaking here
            break;

        case 2:
            if (bufSizeReadDone)
            {
                break;
            }
            bufReadSize = (uint8_t)std::strtol((char*)buf + asciiCntr, NULL, 10);
            bufSizeReadDone = true;
            //dataStartIndex = asciiCntr - 1; // save to prevent overwrite of index column
            break;

        case 3:
            dataDirIN = 1 + strncmp("CDC IN", (char*)buf + asciiCntr, strlen("CDC IN")); //+1 since the comp returns -1 for CDC OUT and 0 for CDC IN
            asciiCntr += (12 - dataDirIN); // avoid reading the "CDC OUT DATA/CDC IN DATA" info
            break;

        case 4:
            for (int dataCntr = 0; dataCntr < bufReadSize; dataCntr++) // convert the ascii data to numeric
            {
                hexBuf[dataCntr] = (uint8_t)std::strtol((char*)buf + asciiCntr + 3*dataCntr, NULL, 16);
            }

            snprintf((char*)(buf + (int)date.find(",") + 1), numOfElements, "%s.",timeStampString); // copy the filtered date into the buffer, \
            //dot at the end to read the index at which point the writig will begin
            date = (char*)buf;
            dataStartIndex = (int)date.find(".");
            buf[dataStartIndex] = 0; // overwrite the "."

            if (bufReadSize == 3) // the data is ACK IN/OUT
            {
                snprintf((char*)(buf + dataStartIndex), strlen(InOutACK[dataDirIN]) + 1, "%s", InOutACK[dataDirIN]); // +1 for the NULL char
                return;
            }

            if (!dataDirIN) // Convert the CDC out data into the CAN message
            {               
                uint32_t maxCanPackets = (uint32_t)hexBuf[2];
                uint16_t crcCalculate = ProtocolCalculateCRC(hexBuf + 2, maxCanPackets);
                uint16_t crcInBuff = (uint16_t)hexBuf[bufReadSize - 2] << 8 | (uint16_t)hexBuf[bufReadSize - 3];
                if (crcInBuff != crcCalculate)
                {
                    // change this to print it to the csv file
                    snprintf((char*)(buf + dataStartIndex), strlen(",BAD CRC\n") + 1, ",BAD CRC\n"); // +1 for the NULL char
                    return;
                }
                
                CAN_Transmit_Data(hexBuf + 2, buf + dataStartIndex); //+2 to avoid reading the protocol bytes; 
                return;
            }
            else // Convert the CDC IN data into the CAN message
            {
                
                CAN_Receive_Data_Interpret(bufReadSize, hexBuf, buf + dataStartIndex); //+1 for move to one more column right
                return;
            }
            
            break;
        }

        if (buf[asciiCntr] == ',')
        {
            commaDelimiterCntr++;
        }
    }
}

int main()
{
    errno_t err, err1;
    uint8_t buf[500] = { 0 };
    DWORD fileSize = 0;
    int32_t sizetoCopy_Process = 0;

    FILE* in_file;
    FILE* out_file;
    //FILE* new_out_file;

    err = fopen_s(&in_file, readFileName, "r+");
    err1 = fopen_s(&out_file, writeFileName, "w");

    if (err != 0)
    {
        std::cout << "Unable to open \"z1 up move compressed data.csv\" file\n";

        return 0;
    }
    else if (err1 != 0)
    {
        std::cout << "Unable to open \"z1 up move dataonly data.csv\" file\n";

        return 0;
    }

    err = stat(readFileName, &file_status);

    if (err < 0)
    {
        std::cout << "Unable to read the file size!\n";

        return 0;
    }

    sizetoCopy_Process = file_status.st_size;

    if (err == 0)
    {
        if (freopen_s(&out_file, writeFileName, "w", out_file) != 0)
        {
            std::cout << "Unable to reopen the \"z1 up move dataonly data.csv\" file\n";

            return 0;
        }

        fprintf_s(out_file, "Index, Time (us), CDC OUT (ID RTR DATA), CDC IN (ID RTR DATA)\n"); // write the column headers

        while (sizetoCopy_Process)
        {
            if (fgets((char*)buf, sizeof(buf), in_file) != NULL)
            {
                checkTheDatatoCopy(buf);

                fprintf_s(out_file, "%s", buf);

                sizetoCopy_Process -= (uint32_t)strlen((char*)buf);
            }
            else
            {
                break;
            }
        }

        std::cout << "the file content is/are:\n" << buf;
    }
    else
    {
        std::cout << "file could not be read or does not exist\n";
    }

    _fcloseall();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
