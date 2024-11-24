#include "includes.h"

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
        HANDLE hMapFile = NULL;
        while (!hMapFile)
        {
            hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"ProcWatchSharedMem");
        }
        while (!pBuf)
        {
            pBuf = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
            Sleep(100);
        }
    }

    void write_log_data(std::string data)
    {
        CopyMemory((PVOID)pBuf->data, data.c_str(), strlen(data.c_str()) + 1);
        InterlockedExchange(&pBuf->readyFlag, 1);
    }

    void write_import_data(HookLib hl)
    {
        hl.FillImportData(pBuf->imports, pBuf->import_count);
        InterlockedExchange(&pBuf->importsReady, 1);
    }
};