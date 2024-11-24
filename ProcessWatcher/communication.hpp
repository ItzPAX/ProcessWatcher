#pragma once

#include <iostream>
#include <windows.h>
#include <vector>

struct ImportInfo
{
    char name[256];
    char module_name[256];
};

class communication
{
public:
    struct SharedData {
        volatile LONG readyFlag;  // 0 = not ready, 1 = ready
        volatile LONG importsReady; // 0 = not ready, 1 = ready
        volatile LONG hookReady; // 0 = not ready, 1 = ready

        char data[1024];          // String data

        ImportInfo imports[1000];
        int import_count;

        ARG_TYPES params[64];
        ARG_TYPES return_type;
        char func_name[1024];
        int num_params;
    };

    SharedData* pBuf;

public:
    void init()
    {
        HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedData), L"ProcWatchSharedMem");
        if (hMapFile)
        {
            pBuf = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
            if (!pBuf)
                return;

            pBuf->hookReady = 0;
            pBuf->readyFlag = 0;
            pBuf->importsReady = 0;
            pBuf->import_count = 0;
        }
    }

    void read_log_data(std::vector<std::string>& logs)
    {
        if (!pBuf->readyFlag)
            return;

        char buffer[1024];
        strcpy_s(buffer, (char*)pBuf->data);

        std::string str(buffer);

        logs.push_back(str);

        InterlockedExchange(&pBuf->readyFlag, 0);
    }
    void read_import_data(ImportInfo* info, int* import_count)
    {
        if (!pBuf->importsReady)
            return;

        memcpy(info, pBuf->imports, sizeof(pBuf->imports));
        *import_count = pBuf->import_count;

        InterlockedExchange(&pBuf->importsReady, 0);
    }
    void write_hook_data(std::string func_name, ARG_TYPES ret, ARG_TYPES* params, int num_params)
    {
        RtlZeroMemory(pBuf->func_name, sizeof(pBuf->func_name));
        strcpy_s(pBuf->func_name, func_name.c_str());

        pBuf->return_type = ret;
        pBuf->num_params = num_params;
        memcpy(pBuf->params, params, pBuf->num_params * sizeof(ARG_TYPES));

        InterlockedExchange(&pBuf->hookReady, 1);
    }
};