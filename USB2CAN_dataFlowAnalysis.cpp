// USB2CAN_dataFlowAnalysis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fileapi.h>
#include <sys/stat.h>

#define elementSize 1 //read 1 byte at a time
#define numOfElements   500 // make this equal to size of buf to which it will be written after the read operation
#define readFileName "C:\\HarmonyProjects_unreleasedInitialU2C\\U2C Aura pausing testing\\z1 up move compressed data.csv"
#define writeFileName "C:\\HarmonyProjects_unreleasedInitialU2C\\U2C Aura pausing testing\\z1 up move dataonly data.csv"

struct stat file_status;

bool checkTheDatatoCopy(uint8_t* buf)
{
    bool bufCopy = false;
    bool bufReadSize = false;
    uint32_t commaDelimiterCntr = 0;
    uint8_t hexBuf[500] = { 0 };
    uint32_t hexBufCntr = 0;
    uint32_t dataStartIndex = 0;

    for (int i = 0; i < strlen((char*)buf); i++)
    {
        if (bufCopy && commaDelimiterCntr == 3) // 3rd column detected, now start seperating the data into each cell
        {
            if (buf[i] == ' ') // replace the space by comma
            {
                //buf[i] = ',';
                hexBuf[hexBufCntr] = ',';
                hexBufCntr++;
            }
            else
            {
                hexBuf[hexBufCntr] = (uint8_t)std::strtol((char*)buf + i, NULL, 16);
                hexBufCntr++;
                i++; // the above function has already taken the 2 character string and converted into a hex number
            }
        }
        if (bufReadSize == true && !bufCopy) // read the size column only
        {
            if (buf[i] > '0' && buf[i] <= '9')
            {
                bufCopy = true;
                //break;
            }
            else
                break;
        }
        if (buf[i] == ',')
        {
            commaDelimiterCntr++;
            bufReadSize = true;
            dataStartIndex = i + 1; // save the 3rd comma's index + 1 because that is when the data starts
        }
    }

    if (bufCopy) // replace the string data in the buf with the hex data
    {
        memcpy(buf + dataStartIndex, hexBuf, hexBufCntr);
        dataStartIndex = (uint32_t)strlen((char*)buf);

        buf[dataStartIndex] = '\n';
        buf[dataStartIndex + 1] = 0;
        //strcat_s((char*)buf, strlen((char*)buf), "\n");
    }

    return bufCopy;
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
        while (sizetoCopy_Process)
        {
            if (fgets((char*)buf, sizeof(buf), in_file) != NULL)
            {
                if (checkTheDatatoCopy(buf))
                {
                    fprintf_s(out_file, "%s", buf);
                }

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
