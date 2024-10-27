#pragma once

#include <iostream>
#include <windows.h>

namespace comm
{
    struct SharedData {
        volatile LONG readyFlag;  // 0 = not ready, 1 = ready
        char data[1024];          // String data
    };

    SharedData* pBuf;

    void init()
    {
        HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 1024, L"ProcWatchSharedMem");
        if (hMapFile)
        {
            pBuf = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 1024);
            if (!pBuf)
                return;

            pBuf->readyFlag = 0;
        }
    }

    static std::string read_log_data()
    {
        if (!pBuf->readyFlag)
            return "";

        InterlockedExchange(&pBuf->readyFlag, 0);

        char buffer[1024];
        strcpy_s(buffer, (char*)pBuf->data);

        std::string str(buffer);

        MessageBoxA(NULL, str.c_str(), "Message", NULL);
        return str;
    }
}