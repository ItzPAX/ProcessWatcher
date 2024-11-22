#pragma once

#include <iostream>
#include <windows.h>
#include <vector>

struct ImportInfo
{
    char name[256];
    char module_name[256];
};

namespace comm
{
    struct SharedData {
        volatile LONG readyFlag;  // 0 = not ready, 1 = ready
        volatile LONG importsReady; // 0 = not ready, 1 = ready
        char data[1024];          // String data
        ImportInfo imports[1024];
        int import_count;
    };

    static SharedData* pBuf;

    static void init()
    {
        HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedData), L"ProcWatchSharedMem");
        if (hMapFile)
        {
            pBuf = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
            if (!pBuf)
                return;

            pBuf->readyFlag = 0;
            pBuf->importsReady = 0;
            pBuf->import_count = 0;
        }
    }

    static void read_log_data(std::vector<std::string>& logs)
    {
        if (!pBuf->readyFlag)
            return;

        char buffer[1024];
        strcpy_s(buffer, (char*)pBuf->data);

        std::string str(buffer);

        logs.push_back(str);

        InterlockedExchange(&pBuf->readyFlag, 0);
    }
    static void read_import_data(ImportInfo* info, int* import_count)
    {
        if (!pBuf->importsReady)
            return;

        memcpy(info, pBuf->imports, sizeof(pBuf->imports));
        *import_count = pBuf->import_count;

        InterlockedExchange(&pBuf->importsReady, 0);
    }
}